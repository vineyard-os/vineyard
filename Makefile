HDD				:= ../hdd.img
HDD_VDI			:= ../hdd.vdi
HDD_VMDK		:= ../hdd.vmdk
VBOXMANAGE		?= VBoxManage
VM_NAME			?= vineyard

include config/all

BUILDER			:= ../tools/image-builder/builder

hdd: $(HDD)

include kernel/Makefile
include kernel/efistub/Makefile

$(HDD): $(LOADER)
	$(call run,"BUILD", $(BUILDER) hdd.yaml | bash)

$(HDD_VMDK): $(LOADER) $(KERNEL)
	$(call run,"BUILD",$(BUILDER) hdd.yaml --vmdk | bash)

$(HDD_VDI): $(LOADER) $(KERNEL)
	$(call run,"BUILD",$(BUILDER) hdd.yaml --vdi | bash)

test: setup $(LOADER) $(KERNEL) $(HDD)
	$(call run_normal,"QEMU",$(EMU) $(EMUFLAGS))

test-gdb: setup $(LOADER) $(KERNEL) $(HDD)
	$(call run_normal,"QEMU",$(EMU) $(EMUFLAGS) -s -S)

test-vbox: setup $(LOADER) $(KERNEL) $(HDD_VDI)
	if ! $(VBOXMANAGE) list vms | grep -q \"vineyard\"; then $(VBOXMANAGE) registervm `pwd`/misc/vineyard.vbox; fi
	$(VBOXMANAGE) storageattach $(VM_NAME) --storagectl "SATA" --port 0 --device 0 --medium none 2> /dev/null || true
	$(VBOXMANAGE) closemedium disk $(HDD_VDI)
	$(VBOXMANAGE) storageattach $(VM_NAME) --storagectl "SATA" --port 0 --device 0 --medium $(HDD_VDI) --type hdd
	$(VBOXMANAGE) startvm $(VM_NAME) | grep -v 'VM "$(VM_NAME)"' || true

test-vmware: setup $(LOADER) $(KERNEL) $(HDD_VMDK)
	if ! command -v vmrun &> /dev/null; then echo "Error: VMWare Workstation is not installed"; fi
	if ! grep -q bin/hdd.vmdk misc/vineyard.vmx; then echo -n 'nvme0:0.fileName = "' >> misc/vineyard.vmx && echo -n $(shell pwd) >> misc/vineyard.vmx && echo -n '/bin/hdd.vmdk"' >> misc/vineyard.vmx; fi
	$(call run,"VMWARE",vmrun start misc/vineyard.vmx)

clean:
	$(call run,"RM",rm -f $(KERNEL) $(LOADER) $(KERNEL_OBJ) $(LOADER_OBJ))

clean-bin: clean
	$(call run,"RM",rm -rf $(HDD) compile_commands.json .json .deps bin)

clean-font:
	$(call run,"RM", rm -f kernel/driver/graphics/font.c)

clean-vbox:
	$(call run,"CLEAN", VBoxManage unregistervm vineyard 2> /dev/null || true)

distclean: clean-bin clean-font clean-libacpica clean-vbox
	rm -rf third-party

clean-libacpica:
	$(call run,"RM", rm -f bin/libacpica.a $(ACPICA_OBJ))

.PHONY: setup test test-vbox clean clean-bin clean-font clean-libacpica clean-vbox

.SUFFIXES:
.SUFFIXES: .c .o

.SILENT:
