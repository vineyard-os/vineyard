ENTRY(main)

PAGE = 0x1000;
HIGHER_HALF = 0xFFFFFFFF80000000;
PHYSICAL  = 0x100000;

SECTIONS {
	. = HIGHER_HALF;

	.text ALIGN(PAGE) : AT(ADDR(.text) - HIGHER_HALF + PHYSICAL) {
		*(.text)
	} :code

	.rodata ALIGN(PAGE) : AT(ADDR(.rodata) - HIGHER_HALF + PHYSICAL) {
		*(.rodata)
	} :rodata

	.data ALIGN(PAGE) : AT(ADDR(.data) - HIGHER_HALF + PHYSICAL) {
		*(.data)
	} :data

	.bss ALIGN(PAGE) : AT(ADDR(.bss) - HIGHER_HALF + PHYSICAL) {
		*(.bss)
	}

	/DISCARD/ : {
		*(.comment)
		*(.eh_frame)
		*(.note.gnu.build-id)
	}

	KERNEL_SIZE = . - HIGHER_HALF;
}

PHDRS
{
    code PT_LOAD FLAGS(5); /* R_X */
    data PT_LOAD FLAGS(6); /* RW_ */
	rodata PT_LOAD FLAGS(4); /* R__ */
    note PT_NOTE FLAGS(4); /* R__ */
}
