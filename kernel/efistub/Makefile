LOADER_SRC_LIB	:= $(shell find kernel/efistub -name *.c -type l)
LOADER_SRC_C	:= $(shell find kernel/efistub -name *.c -type f -o -type l)
LOADER_SRC_ASM	:= $(shell find kernel/efistub -name *.s -type f)
LOADER_SRC		:= $(LOADER_SRC_C) $(LOADER_SRC_ASM) $(LOADER_SRC_LIB)
LOADER_OBJ_C	:= $(addprefix bin/kernel/,$(addsuffix .o,$(LOADER_SRC_C)))
LOADER_OBJ		:= $(addprefix bin/kernel/,$(addsuffix .o,$(LOADER_SRC)))
LOADER_DEP		:= $(addprefix .deps/,$(SRC:.c=.d))
LOADER			:= bin/hdd/efi/boot/bootx64.efi

LOADER_CFLAGS	:= -Ikernel/include -Ilib/include -Wno-format -Wno-format-security -D STACK=0xFFFFFE0000000000 -D STACK_SIZE=0x21000 -D VINEYARD_LOADER
LOADER_ASFLAGS	:= -D STACK=0xFFFFFE0000000000 -D STACK_SIZE=0x21000

loader: $(LOADER)

$(LOADER): $(LOADER_OBJ)
	mkdir -p $(dir $@)
	$(call run,"LD",$(EFI_LD) $(EFI_LDFLAGS) $^)
	rm -f $(LOADER:.efi=.lib)
ifndef USE_GCC
	sed -e '1s/^/[\n/' -e '$$s/,$$/\n]/' $(addprefix .json/,$(addsuffix .json,$(LOADER_OBJ_C))) > compile_commands.json
endif

kernel/efistub/%.c:
	true

bin/kernel/kernel/efistub/lib/%.c.o: $(LOADER_SRC_LIB)
	mkdir -p $(dir .deps/$@)
	mkdir -p $(dir .json/$(dir $@))
	mkdir -p $(dir bin/kernel/kernel/efistub/$*)
ifndef USE_GCC
	$(call run,"CC",$(EFI_CC) $(EFI_CFLAGS) $(LOADER_CFLAGS) -c kernel/efistub/lib/$*.c -o $@ -MF .deps/$(@:.o=.d) -MJ .json/$@.json)
else
	$(call run,"CC",$(EFI_CC) $(EFI_CFLAGS) $(LOADER_CFLAGS) -c kernel/efistub/lib/$*.c -o $@ -MF .deps/$(@:.o=.d))
endif

bin/kernel/kernel/efistub/%.c.o: $(LOADER_SRC_C)
	mkdir -p $(dir .deps/$@)
	mkdir -p $(dir .json/$(dir $@))
	mkdir -p $(dir bin/kernel/kernel/efistub/$*)
ifndef USE_GCC
	$(call run,"CC",$(EFI_CC) $(EFI_CFLAGS) $(LOADER_CFLAGS) -c kernel/efistub/$*.c -o $@ -MF .deps/$(@:.o=.d) -MJ .json/$@.json)
else
	$(call run,"CC",$(EFI_CC) $(EFI_CFLAGS) $(LOADER_CFLAGS) -c kernel/efistub/$*.c -o $@ -MF .deps/$(@:.o=.d))
endif

bin/kernel/kernel/efistub/%.s.o: $(LOADER_SRC_ASM)
	mkdir -p $(dir bin/kernel/$<)
	$(call run,"AS",$(AS) $(EFI_ASFLAGS) $(LOADER_ASFLAGS) -o $@ kernel/efistub/$*.s)
