KERNEL_SRC_C	:= $(shell find kernel lib \( -path kernel/efistub \) -prune -o -name '*.c' -type f -print)
KERNEL_SRC		:= $(shell find kernel lib \( -path kernel/efistub \) -prune -o -name '*.[cs]' -type f -print)
KERNEL_SRC		:= $(filter-out kernel/font.c,$(KERNEL_SRC)) kernel/driver/graphics/font.c
KERNEL_OBJ_C	:= $(addprefix bin/obj/,$(addsuffix .o,$(KERNEL_SRC_C)))
KERNEL_OBJ		:= $(addprefix bin/obj/,$(addsuffix .o,$(KERNEL_SRC)))
KERNEL_DEP		:= $(addprefix .deps/,$(SRC:.c=.d))
KERNEL			:= bin/hdd/kernel

KERNEL_CFLAGS	:= -Ikernel/include -Ilib/include -mcmodel=kernel -Wno-unreachable-code
KERNEL_LDFLAGS	:= -Tkernel/link.ld

kernel: $(KERNEL) ## build the kernel

$(KERNEL): $(KERNEL_OBJ)
	mkdir -p $(dir $@)
	$(call run,"LD",$(LD) $(LDFLAGS) $(KERNEL_LDFLAGS) $^ -o $@)
ifndef USE_GCC
	sed -e '1s/^/[\n/' -e '$$s/,$$/\n]/' $(addprefix .json/,$(addsuffix .json,$(KERNEL_OBJ_C))) >> compile_commands.json
endif

bin/obj/%.c.o: %.c
	mkdir -p $(dir .deps/$@)
	mkdir -p $(dir .json/$(dir $@))
	mkdir -p $(dir bin/obj/$<)
ifndef USE_GCC
	$(call run,"CC",$(CC) $(CFLAGS) $(KERNEL_CFLAGS) -c $< -o $@ -MF .deps/$(@:.o=.d) -MJ .json/$@.json)
else
	$(call run,"CC",$(CC) $(CFLAGS) $(KERNEL_CFLAGS) -c $< -o $@ -MF .deps/$(@:.o=.d))
endif

bin/obj/%.s.o: %.s
	mkdir -p $(dir bin/obj/$<)
	$(call run,"AS",$(AS) $(ASFLAGS) -o $@ $<)

kernel/driver/graphics/font.c: $(UNI-VGA)
	$(call run,"FONT",util/bdf-to-c $< > $@)
