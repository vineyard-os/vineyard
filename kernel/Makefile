KERNEL_SRC_C	:= $(shell find kernel lib \( -path kernel/efistub \) -prune -o \( -path kernel/acpica \) -prune -o -name '*.c' -type f -print)
KERNEL_SRC		:= $(shell find kernel lib \( -path kernel/efistub \) -prune -o \( -path kernel/acpica \) -prune -o -name '*.[cs]' -type f -print)
KERNEL_SRC		:= $(filter-out kernel/crti.s,$(filter-out kernel/crtn.s,$(KERNEL_SRC)))
KERNEL_SRC		:= $(filter-out kernel/driver/graphics/font.c,$(KERNEL_SRC)) kernel/driver/graphics/font.c
KERNEL_CRTI		:= bin/kernel/kernel/crti.s.o
KERNEL_CRTN		:= bin/kernel/kernel/crtn.s.o
KERNEL_CRTBEGIN	:= $(shell $(CC) $(CFLAGS) $(KERNEL_CFLAGS) -print-file-name=crtbegin.o)
KERNEL_CRTEND	:= $(shell $(CC) $(CFLAGS) $(KERNEL_CFLAGS) -print-file-name=crtend.o)
KERNEL_OBJ_C	:= $(addprefix bin/kernel/,$(addsuffix .o,$(KERNEL_SRC_C)))

ACPICA_OBJ		:= $(addprefix bin/acpica/,$(addsuffix .o,$(shell find kernel/acpica -name *.c)))

ifndef USE_GCC
	KERNEL_OBJ	:= $(KERNEL_CRTI) $(addprefix bin/kernel/,$(addsuffix .o,$(KERNEL_SRC))) $(KERNEL_CRTN)
else
	KERNEL_OBJ	:= $(KERNEL_CRTI) $(KERNEL_CRTBEGIN) $(addprefix bin/kernel/,$(addsuffix .o,$(KERNEL_SRC))) $(KERNEL_CRTEND) $(KERNEL_CRTN)
endif

KERNEL_DEP		:= $(addprefix .deps/,$(SRC:.c=.d))
KERNEL			:= ../boot/kernel

KERNEL_CFLAGS	:= -Ikernel/include -Ilib/include -mcmodel=kernel -Wno-unreachable-code -fsanitize=undefined
KERNEL_LDFLAGS	:= -Tkernel/link.ld -Lbin -lacpica

ACPICA_CFLAGS	:= $(filter-out -fsanitize=undefined,$(KERNEL_CFLAGS))

kernel: $(KERNEL) ## build the kernel

$(KERNEL): kernel/include/kconfig.h $(KERNEL_OBJ) bin/libacpica.a
	mkdir -p $(dir $@)
	$(call run,"LD",$(LD) $(LDFLAGS) $(KERNEL_OBJ) $(KERNEL_LDFLAGS) -o $@)
ifndef USE_GCC
	sed -e '1s/^/[\n/' -e '$$s/,$$/\n]/' $(addprefix .json/,$(addsuffix .json,$(KERNEL_OBJ_C))) >> compile_commands.json
endif

bin/libacpica.a: $(ACPICA_OBJ)
	$(call run,AR,$(AR) rcs $@ $^)

bin/acpica/%.c.o: %.c
	mkdir -p $(dir .deps/$@)
	mkdir -p $(dir .json/$(dir $@))
	mkdir -p $(dir bin/kernel/$<)
ifndef USE_GCC
	$(call run,"CC",$(CC) $(CFLAGS) $(ACPICA_CFLAGS) -c $< -o $@ -MF .deps/$(@:.o=.d) -MJ .json/$@.json)
else
	$(call run,"CC",$(CC) $(CFLAGS) $(ACPICA_CFLAGS) -c $< -o $@ -MF .deps/$(@:.o=.d))
endif

bin/kernel/%.c.o: %.c
	mkdir -p $(dir .deps/$@)
	mkdir -p $(dir .json/$(dir $@))
	mkdir -p $(dir bin/kernel/$<)
ifndef USE_GCC
	$(call run,"CC",$(CC) $(CFLAGS) $(KERNEL_CFLAGS) -c $< -o $@ -MF .deps/$(@:.o=.d) -MJ .json/$@.json)
else
	$(call run,"CC",$(CC) $(CFLAGS) $(KERNEL_CFLAGS) -c $< -o $@ -MF .deps/$(@:.o=.d))
endif

bin/kernel/%.s.o: %.s
	mkdir -p $(dir bin/kernel/$<)
	$(call run,"AS",$(AS) $(ASFLAGS) -o $@ $<)

kernel/driver/graphics/font.c:
	$(call run,"FONT",util/bdf-to-c third-party/uni-vga/u_vga16.bdf > $@)

kernel/include/kconfig.h: Kconfig .config
	genconfig --header-path $@

-include $(KERNEL_DEP)

.PHONY: $(KERNEL_CRTBEGIN) $(KERNEL_CRTEND)
