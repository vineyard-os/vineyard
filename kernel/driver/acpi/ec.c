#include <acpi/ec.h>
#include <cpu/ports.h>
#include <stdio.h>
#include <vy.h>

no_warn(-Wunused-parameter)
no_warn_clang(-Wnull-pointer-arithmetic)

#define EC_EVENT_OUTPUT_BUFFER_FULL 0x01
#define EC_EVENT_INPUT_BUFFER_EMPTY 0x02

#define EC_COMMAND_READ 0x80
#define EC_COMMAND_WRITE 0x81
#define EC_BURST_ENABLE 0x82
#define EC_BURST_DISABLE 0x83
#define EC_QUERY 0x84

static uint16_t data_io = 0;
static uint16_t command_io = 0;

/* ACPI Embedded controller resource discovery */
static acpi_status acpi_ec_io_ports(struct acpi_resource *resource, void *context) {
	if(resource->type != ACPI_RESOURCE_TYPE_IO) {
		return AE_OK;
	}

	if(data_io == 0) {
		data_io = resource->data.io.minimum;
	} else if(command_io == 0) {
		command_io = resource->data.io.minimum;
	} else {
		return AE_CTRL_TERMINATE;
	}

	return AE_OK;
}

static acpi_status acpi_ec_parse(acpi_handle handle, uint32_t Level, void *context, void **retval) {
	acpi_status status = acpi_walk_resources(handle, (acpi_string) METHOD_NAME__CRS, acpi_ec_io_ports, NULL);
	if(ACPI_FAILURE(status)) {
		panic("ACPI failure %u @ acpi_walk_resources", status);
	}

	return AE_OK;
}

/* ACPI EC raw read/write I/O */
static uint8_t acpi_ec_read_data(void) {
	return inb(data_io);
}

static void acpi_ec_write_data(uint8_t cmd) {
	outb(data_io, cmd);
}

static uint8_t acpi_ec_read_command(void) {
	return inb(command_io);
}

static void acpi_ec_write_command(uint8_t cmd) {
	outb(command_io, cmd);
}

/* ACPI EC I/O events */
static void acpi_ec_poll_event(uint8_t event) {
	acpi_status status = AE_NO_HARDWARE_RESPONSE;

	while(status != AE_OK) {
		uint8_t res = acpi_ec_read_command();

		if(event == EC_EVENT_INPUT_BUFFER_EMPTY) {
			/* if bit 2 is clear, the input buffer is empty */
			status = ((res & 2) == 0) ? AE_OK : AE_NO_HARDWARE_RESPONSE;
		} else if(event == EC_EVENT_OUTPUT_BUFFER_FULL) {
			/* if bit 1 is set, the output buffer is filled */
			status = ((res & 1) != 0) ? AE_OK : AE_NO_HARDWARE_RESPONSE;
		}
	}
}

/* ACPI EC commands */
static void acpi_ec_command(uint8_t command) {
	uint8_t event;

	/* depending on the command being run we have to wait for an event to complete it, so we're figuring out which */
	switch(command) {
		case EC_COMMAND_READ:
		case EC_COMMAND_WRITE:
		case EC_BURST_DISABLE:
			event = EC_EVENT_INPUT_BUFFER_EMPTY;
			break;
		case EC_BURST_ENABLE:
		case EC_QUERY:
			event = EC_EVENT_OUTPUT_BUFFER_FULL;
			break;
		default:
			panic("EC: invalid command %#x\n", command);
	}

	/* first, make sure there is no interfering clutter */
	acpi_ec_poll_event(EC_EVENT_INPUT_BUFFER_EMPTY);

	/* actually write out the command */
	acpi_ec_write_command(command);

	/* poll until the EC is ready */
	acpi_ec_poll_event(event);
}

static void acpi_ec_burst_enable(void) {
	acpi_ec_command(EC_BURST_ENABLE);
	uint8_t ret = acpi_ec_read_data();

	/* bit 4 of the status register tells us whether we're in burst mode */
	if(!(ret & 0x10)) {
		panic("EC: failed to enable burst");
	}
}

static void acpi_ec_burst_disable(void) {
	acpi_ec_command(EC_BURST_DISABLE);
}

/* ACPI EC single-byte I/O */
static uint8_t acpi_ec_read_1(uint8_t addr) {
	acpi_ec_command(EC_COMMAND_READ);
	acpi_ec_write_data(addr);
	acpi_ec_poll_event(EC_EVENT_OUTPUT_BUFFER_FULL);
	return acpi_ec_read_data();
}

static void acpi_ec_write_1(uint8_t addr, uint8_t val) {
	acpi_ec_command(EC_COMMAND_WRITE);
	acpi_ec_write_data(addr);
	acpi_ec_poll_event(EC_EVENT_INPUT_BUFFER_EMPTY);
	acpi_ec_write_data(val);
	acpi_ec_poll_event(EC_EVENT_INPUT_BUFFER_EMPTY);
}

/* ACPI EC multi-byte I/O */
static void acpi_ec_read(uint8_t addr, size_t len, uint8_t *buf) {
	acpi_ec_burst_enable();

	for(size_t reg = 0; reg < len; reg++) {
		buf[reg] = acpi_ec_read_1((addr + reg) & 0xFF);
	}

	acpi_ec_burst_disable();
}

static void acpi_ec_write(uint8_t addr, size_t len, uint8_t *buf) {
	acpi_ec_burst_enable();

	for(size_t reg = 0; reg < len; reg++) {
		acpi_ec_write_1((addr + reg) & 0xFF, buf[reg]);
	}

	acpi_ec_burst_disable();
}

/* initialize the ACPI Embedded Controller */
void acpi_ec_init(void) {
	acpi_get_devices((acpi_string) "PNP0C09", acpi_ec_parse, NULL, NULL);

	/* TODO: the spec says something about an interrupt-driven interface instead of polling-driven; investigate! */

	/* this handler will be called when ACPICA wants us to to EC-related work */
	acpi_status status = acpi_install_address_space_handler(ACPI_ROOT_OBJECT, ACPI_ADR_SPACE_EC, &acpi_ec_handler, NULL, NULL);
	if(ACPI_FAILURE(status)) {
		acpi_terminate();
		panic("ACPI failure %u @ acpi_install_address_space_handler", status);
	}
}

/* handle the requests made to the EC */
acpi_status acpi_ec_handler(uint32_t function, acpi_physical_address address, uint32_t bits, uint64_t *value, void *handler_context, void *region_context) {
	if(function != ACPI_READ && function != ACPI_WRITE) {
		return AE_BAD_PARAMETER;
	}

	if(address + (bits >> 3) > 0xFF) {
		return AE_BAD_ADDRESS;
	}

	if(bits % 8 || !value) {
		return AE_BAD_PARAMETER;
	}

	if(function == ACPI_READ) {
		acpi_ec_read(address & 0xFF, bits >> 3, (uint8_t *) value);
	} else if(function == ACPI_WRITE) {
		acpi_ec_write(address & 0xFF, bits >> 3, (uint8_t *) value);
	}

	return AE_OK;
}
