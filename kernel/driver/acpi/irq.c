#include <acpi/irq.h>
#include <stdio.h>

void acpi_irq_process(struct acpi_device_info *dev, acpi_handle object) {
    acpi_status status;

    if(dev->flags & ACPI_PCI_ROOT_BRIDGE) {
		struct acpi_buffer buffer = {0};
		buffer.length = ACPI_ALLOCATE_BUFFER;

		if((status = acpi_get_irq_routing_table(object, &buffer)) != AE_OK) {
			panic("ACPI failure %u @ acpi_get_irq_routing_table", status);
		}

		struct acpi_pci_routing_table *table = buffer.pointer;

		for(; table->length != 0; table = (struct acpi_pci_routing_table *) ACPI_NEXT_RESOURCE(table)) {
			uint8_t dev_id = (uint8_t) (table->address >> 16);

			if(table->source[0] != 0x00) {
				acpi_handle link_object;

				status = acpi_get_handle(object, table->source, &link_object);
				if(ACPI_FAILURE(status)) {
					panic("ACPI failure %u @ acpi_get_handle", status);
				}

				struct acpi_buffer resbuf;
				resbuf.length = ACPI_ALLOCATE_BUFFER;
				resbuf.pointer = NULL;

				status = acpi_get_current_resources(link_object, &resbuf);
				if(ACPI_FAILURE(status)) {
					panic("ACPI failure %u @ acpi_get_current_resources", status);
				}

				char *rscan = (char *) resbuf.pointer;
				size_t dev_irq = ~0UL;

				while(1) {
					struct acpi_resource *res = (struct acpi_resource *) rscan;

					if(res->type == ACPI_RESOURCE_TYPE_END_TAG) {
						break;
					}

					if(res->type == ACPI_RESOURCE_TYPE_IRQ) {
						dev_irq = res->data.irq.interrupts[table->source_index];
						break;
					}

					if(res->type == ACPI_RESOURCE_TYPE_EXTENDED_IRQ) {
						dev_irq = res->data.extended_irq.interrupts[table->source_index];
						break;
					}

					rscan += res->length;
				}

				free(resbuf.pointer);

				if(dev_irq == ~0UL) {
					panic("no IRQ for device %d from '%s'", dev_id, table->source);
				}

				// printf("dev %2d pin INT%c# -> IRQ%zu (from '%s')\n", dev_id, 'A' + table->pin, dev_irq, table->source);
			} else {
				// printf("dev %2d pin 0x%x -> IRQ%u\n", dev_id, table->pin, table->source_index);
			}
		}
    }
}
