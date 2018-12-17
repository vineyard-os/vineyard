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
	$(call run,"RM",rm -rf $(HDD) $(CD) iso compile_commands.json .json .deps)

$(OVMF):
	mkdir -p bin
	$(call run,"WGET",wget $(OVMF_URL) -O $(OVMF) -qq)

$(UNI-VGA):
	mkdir -p $(UNI-VGA_DIR)
	wget -qq $(UNI-VGA_URL) -O $(UNI-VGA_DIR)/uni-vga.tar.gz
	tar xzf $(UNI-VGA_DIR)/uni-vga.tar.gz --strip-components=1 -C $(UNI-VGA_DIR)

.PHONY: setup test

.SUFFIXES:
.SUFFIXES: .c .o

.SILENT:
