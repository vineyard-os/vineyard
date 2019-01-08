include config/all

HDD				:= bin/hdd.img
CD				:= bin/cd.iso

setup: $(OVMF) $(UNI-VGA)

hdd: $(HDD)

include kernel/Makefile
include kernel/efistub/Makefile

$(HDD): $(LOADER) $(KERNEL)
	dd if=/dev/zero of=$(HDD) bs=1k count=1440
	mformat -i $(HDD) -f 1440 ::
	mmd -i $(HDD) ::/EFI
	mmd -i $(HDD) ::/EFI/BOOT
	mcopy -i $(HDD) $(LOADER) ::/EFI/BOOT/BOOTX64.EFI
	mcopy -i $(HDD) $(KERNEL) ::/kernel

$(CD): $(HDD)
	mkdir -p iso
	cp $(HDD) iso
	$(call run,"ISO",xorriso -as mkisofs -R -f -e $(shell basename $(HDD)) -no-emul-boot -o $(CD) iso)

test: $(LOADER) $(KERNEL) $(OVMF) $(EMU_REQ)
	$(call run_normal,"QEMU",$(EMU) $(EMUFLAGS) $(EMU_TARGET))

clean:
	$(call run,"RM",rm -f $(KERNEL) $(LOADER) $(KERNEL_OBJ) $(LOADER_OBJ) $(KERNEL_DEP))

clean-bin: clean
	$(call run,"RM",rm -rf $(HDD) $(CD) iso compile_commands.json .json .deps bin)

clean-font:
	$(call run,"RM", rm -f kernel/driver/graphics/font.c)

distclean: clean-bin clean-font clean-uni-vga clean-acpica

$(OVMF):
	mkdir -p bin
	$(call run,"WGET",wget $(OVMF_URL) -O $(OVMF) -qq)

$(UNI-VGA):
	mkdir -p $(UNI-VGA_DIR)
	wget -qq $(UNI-VGA_URL) -O $(UNI-VGA_DIR)/uni-vga.tar.gz
	tar xzf $(UNI-VGA_DIR)/uni-vga.tar.gz --strip-components=1 -C $(UNI-VGA_DIR)

$(ACPICA_DIR):
	wget $(ACPICA_URL) -O $(ACPICA_TAR)
	mkdir -p $(ACPICA_DIR)
	tar -xf $(ACPICA_TAR) -C $(ACPICA_DIR) --strip-components=1
	mkdir -p kernel/acpica
	cp $(ACPICA_DIR)/source/components/{dispatcher,events,executer,hardware,parser,namespace,utilities,tables,resources}/*.c kernel/acpica/
	mkdir -p kernel/include/acpica
	cp -R $(ACPICA_DIR)/source/include/* kernel/include/acpica/
	patch -p0 < patches/acpica.patch
	php util/acpica-fixup kernel/acpica

clean-uni-vga:
	$(call run,"RM", rm -rf $(UNI-VGA_DIR))

clean-acpica:
	$(call run,"RM", rm -rf $(ACPICA_DIR_C) $(ACPICA_DIR_H) $(ACPICA_DIR) $(ACPICA_TAR))

.PHONY: setup test clean clean-bin clean-font clean-uni-vga clean-acpica

.SUFFIXES:
.SUFFIXES: .c .o

.SILENT:
