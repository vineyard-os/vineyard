HDD				:= ../hdd.img
HDD_VDI			:= ../hdd.vdi
HDD_VMDK		:= ../hdd.vmdk
VM_NAME			?= vineyard

include config/all
include kernel/Makefile

test: setup
	if ! brctl show qemu-vy > /dev/null 2>&1; then sudo util/qemu-ifup; fi
	$(call run_normal,"QEMU",$(EMU) $(EMUFLAGS))

test-gdb: setup
	$(call run_normal,"QEMU",$(EMU) $(EMUFLAGS) -s -S)

test-vmware: setup
	if ! command -v vmrun &> /dev/null; then echo "Error: VMWare Workstation is not installed"; fi
	if ! grep -q hdd.vmdk misc/vineyard.vmx; then echo -n 'nvme0:0.fileName = "' >> misc/vineyard.vmx && echo -n $(shell pwd) >> misc/vineyard.vmx && echo -n '/../hdd.vmdk"' >> misc/vineyard.vmx; fi
	$(call run,"VMWARE",vmrun start misc/vineyard.vmx)

clean:
	$(call run,"RM",rm -f $(KERNEL) $(LOADER) $(KERNEL_OBJ) $(LOADER_OBJ))

clean-bin: clean
	$(call run,"RM",rm -rf $(HDD) compile_commands.json .json .deps bin)

clean-font:
	$(call run,"RM", rm -f kernel/driver/graphics/font.c)

distclean: clean-bin clean-font clean-libacpica
	rm -rf third-party

clean-libacpica:
	$(call run,"RM", rm -f bin/libacpica.a $(ACPICA_OBJ))

.PHONY: setup test clean clean-bin clean-font clean-libacpica

.SUFFIXES:
.SUFFIXES: .c .o

.SILENT:
