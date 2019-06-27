include config/all

HDD				:= bin/hdd.img
HDD_VDI			:= bin/hdd.vdi
HDD_VMDK		:= bin/hdd.vmdk
CD				:= bin/cd.iso
VBOXMANAGE		?= VBoxManage
VM_NAME			?= vineyard

setup: $(OVMF) $(UNI-VGA) $(ACPICA_DIR)
	echo "Setup complete, you can now proceed to building vineyard"

hdd: $(HDD)

include kernel/Makefile
include kernel/efistub/Makefile

$(HDD): $(LOADER) $(KERNEL)
	$(call run,"BUILD", dd if=/dev/zero of=$(HDD) bs=48M count=1 status=none)
	echo -e $(HDD_FDISK) | fdisk $(HDD) > /dev/null
	dd if=/dev/zero of=$(HDD).part bs=40M count=1 status=none
	mkfs.fat -F32 -S512 $(HDD).part > /dev/null
	mmd -i $(HDD).part ::/EFI
	mmd -i $(HDD).part ::/EFI/BOOT
	mcopy -i $(HDD).part bin/hdd/efi/boot/bootx64.efi ::/EFI/BOOT
	mcopy -i $(HDD).part bin/hdd/kernel ::
	dd if=$(HDD).part of=$(HDD) bs=512 seek=16384 status=none

$(HDD_VMDK): $(HDD)
	$(call run,"IMG",qemu-img convert -f raw -O vmdk $< $@)

$(HDD_VDI): $(HDD)
	$(call run,"IMG",qemu-img convert -f raw -O vdi $< $@)

$(CD): $(HDD)
	mkdir -p iso
	cp $(HDD) iso
	$(call run,"ISO",xorriso -as mkisofs -R -f -e $(shell basename $(HDD)) -no-emul-boot -o $(CD) iso)

test: setup $(LOADER) $(KERNEL) $(EMU_REQ)
	$(call run_normal,"QEMU",$(EMU) $(EMUFLAGS) $(EMU_TARGET))

test-gdb: setup $(LOADER) $(KERNEL) $(EMU_REQ)
	$(call run_normal,"QEMU",$(EMU) $(EMUFLAGS) $(EMU_TARGET) -s -S)

test-vbox: setup $(LOADER) $(KERNEL) $(HDD_VDI)
	if ! VBoxManage list vms | grep -q \"vineyard\"; then $(VBOXMANAGE) registervm `pwd`/misc/vineyard.vbox; fi
	$(VBOXMANAGE) storageattach $(VM_NAME) --storagectl "SATA" --port 0 --device 0 --medium none 2> /dev/null || true
	$(VBOXMANAGE) closemedium disk $(HDD_VDI)
	$(VBOXMANAGE) storageattach $(VM_NAME) --storagectl "SATA" --port 0 --device 0 --medium $(HDD_VDI) --type hdd
	$(VBOXMANAGE) startvm $(VM_NAME) | grep -v 'VM "$(VM_NAME)"' || true

test-vmware: setup $(LOADER) $(KERNEL) $(HDD_VMDK)
	if ! command -v vmrun &> /dev/null; then echo "Error: VMWare Workstation is not installed"; fi
	if ! grep -q bin/hdd.vmdk misc/vineyard.vmx; then echo -n 'nvme0:0.fileName = "' >> misc/vineyard.vmx && echo -n $(shell pwd) >> misc/vineyard.vmx && echo -n '/bin/hdd.vmdk"' >> misc/vineyard.vmx; fi
	$(call run,"VMWARE",vmrun start misc/vineyard.vmx)

clean:
	$(call run,"RM",rm -f $(KERNEL) $(LOADER) $(KERNEL_OBJ) $(LOADER_OBJ) $(KERNEL_DEP))

clean-bin: clean
	$(call run,"RM",rm -rf $(HDD) $(CD) iso compile_commands.json .json .deps bin)

clean-font:
	$(call run,"RM", rm -f kernel/driver/graphics/font.c)

clean-vbox:
	$(call run,"CLEAN", VBoxManage unregistervm vineyard 2> /dev/null || true)

distclean: clean-bin clean-font clean-uni-vga clean-acpica clean-libacpica clean-vbox
	rm -rf third-party

$(OVMF):
	mkdir -p bin
	$(call run,"WGET",wget $(OVMF_URL) -O $(OVMF) -qq)

$(UNI-VGA):
	mkdir -p $(UNI-VGA_DIR)
	$(call run,"WGET",wget -qq $(UNI-VGA_URL) -O $(UNI-VGA_DIR)/uni-vga.tar.gz)
	tar xzf $(UNI-VGA_DIR)/uni-vga.tar.gz --strip-components=1 -C $(UNI-VGA_DIR)

$(ACPICA_DIR):
	mkdir -p $(ACPICA_DIR)
	$(call run,"WGET",wget $(ACPICA_URL) -O $(ACPICA_TAR) -qq)
	tar -xf $(ACPICA_TAR) -C $(ACPICA_DIR) --strip-components=1
	make -C $(ACPICA_DIR) acpisrc > /dev/null
	mkdir -p kernel/acpica
	cp $(ACPICA_DIR)/source/components/{dispatcher,events,executer,hardware,parser,namespace,utilities,tables,resources}/*.c kernel/acpica/
	$(ACPICA_DIR)/generate/unix/bin/acpisrc -ldqy kernel/acpica/ kernel/acpica/ > /dev/null 2>&1
	mkdir -p kernel/include/acpica
	cp -R $(ACPICA_DIR)/source/include/* kernel/include/acpica/
	$(ACPICA_DIR)/generate/unix/bin/acpisrc -ldqy kernel/include/acpica/ kernel/include/acpica/ > /dev/null 2>&1
	$(call run,"PATCH",patch -s -p0 < patches/acpica.patch)
	$(call run,"FIXUP",php util/acpica-fixup kernel/acpica)

clean-uni-vga:
	$(call run,"RM", rm -rf $(UNI-VGA_DIR))

clean-acpica:
	$(call run,"RM", rm -rf $(ACPICA_DIR_C) $(ACPICA_DIR_H) $(ACPICA_DIR) $(ACPICA_TAR) bin/libacpica.a $(ACPICA_OBJ))

clean-libacpica:
	$(call run,"RM", rm -f bin/libacpica.a $(ACPICA_OBJ))

.PHONY: setup test test-vbox clean clean-bin clean-font clean-uni-vga clean-acpica clean-libacpica clean-vbox

.SUFFIXES:
.SUFFIXES: .c .o

.SILENT:
