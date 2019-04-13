include config/all

HDD				:= bin/hdd.img
HDD_VDI			:= bin/hdd.vdi
CD				:= bin/cd.iso
VBOXMANAGE		?= VBoxManage
VM_NAME			?= vineyard

setup: $(OVMF) $(UNI-VGA) $(ACPICA_DIR)
	echo "Setup complete"
	exit

hdd: $(HDD)

define HDD_FDISK
"o\nn\np\n1\n16384\n\nt\nb\nw\n"
endef

include kernel/Makefile
include kernel/efistub/Makefile

$(HDD): $(LOADER) $(KERNEL)
	dd if=/dev/zero of=$(HDD) bs=48M count=1
	echo -e $(HDD_FDISK) | fdisk $(HDD) > /dev/null
	dd if=/dev/zero of=$(HDD).part bs=40M count=1
	mkfs.fat -F32 -S512 $(HDD).part > /dev/null
	mmd -i $(HDD).part ::/EFI
	mmd -i $(HDD).part ::/EFI/BOOT
	mcopy -i $(HDD).part bin/hdd/efi/boot/bootx64.efi ::/EFI/BOOT
	mcopy -i $(HDD).part bin/hdd/kernel ::
	dd if=$(HDD).part of=$(HDD) bs=512 seek=16384 status=none

$(CD): $(HDD)
	mkdir -p iso
	cp $(HDD) iso
	$(call run,"ISO",xorriso -as mkisofs -R -f -e $(shell basename $(HDD)) -no-emul-boot -o $(CD) iso)

test: setup $(LOADER) $(KERNEL) $(EMU_REQ)
	$(call run_normal,"QEMU",$(EMU) $(EMUFLAGS) $(EMU_TARGET))

test-gdb: setup $(LOADER) $(KERNEL) $(EMU_REQ)
	$(call run_normal,"QEMU",$(EMU) $(EMUFLAGS) $(EMU_TARGET) -s -S)

test-vbox: $(LOADER) $(KERNEL) $(HDD)
	qemu-img convert -f raw -O vdi $(HDD) $(HDD_VDI)
	$(VBOXMANAGE) storageattach $(VM_NAME) --storagectl "IDE" --port 0 --device 0 --medium none
	$(VBOXMANAGE) closemedium disk $(HDD_VDI)
	$(VBOXMANAGE) storageattach $(VM_NAME) --storagectl "IDE" --port 0 --device 0 --medium $(HDD_VDI) --type hdd
	$(VBOXMANAGE) startvm $(VM_NAME) | grep -v 'VM "$(VM_NAME)"' || true

clean:
	$(call run,"RM",rm -f $(KERNEL) $(LOADER) $(KERNEL_OBJ) $(LOADER_OBJ) $(KERNEL_DEP))

clean-bin: clean
	$(call run,"RM",rm -rf $(HDD) $(CD) iso compile_commands.json .json .deps bin)

clean-font:
	$(call run,"RM", rm -f kernel/driver/graphics/font.c)

distclean: clean-bin clean-font clean-uni-vga clean-acpica
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
	mkdir -p kernel/acpica
	cp $(ACPICA_DIR)/source/components/{dispatcher,events,executer,hardware,parser,namespace,utilities,tables,resources}/*.c kernel/acpica/
	mkdir -p kernel/include/acpica
	cp -R $(ACPICA_DIR)/source/include/* kernel/include/acpica/
	$(call run,"PATCH",patch -p0 < patches/acpica.patch)
	$(call run,"FIXUP",php util/acpica-fixup kernel/acpica)

clean-uni-vga:
	$(call run,"RM", rm -rf $(UNI-VGA_DIR))

clean-acpica:
	$(call run,"RM", rm -rf $(ACPICA_DIR_C) $(ACPICA_DIR_H) $(ACPICA_DIR) $(ACPICA_TAR))

.PHONY: setup test test-vbox clean clean-bin clean-font clean-uni-vga clean-acpica

.SUFFIXES:
.SUFFIXES: .c .o

.SILENT:
