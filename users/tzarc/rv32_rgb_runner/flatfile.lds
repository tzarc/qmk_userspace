__heap_size = 256;
__stack_size = 256;

MEMORY
{
    TEXT (rwx) : ORIGIN = 0x80000000, LENGTH = 2048  # 2kB total for code + RAM
}

ENTRY(_start)

SECTIONS
{
	.text : ALIGN(16) {
		__TEXT_BEGIN__ = .;
		*(.runner_base)
        *(.thunks.*)
		*(.entry.text)
		*(.init.literal)
		*(.init)
		*(.text)
		*(.literal .text .literal.* .text.* .stub)
		*(.out_jump.literal.*)
		*(.out_jump.*)
		__TEXT_END__ = .;
	} >TEXT

	.data : ALIGN(16) {
		__DATA_BEGIN__ = .;
		*(.rodata)
		*(.rodata.*)
		*(.gnu.linkonce.r.*)
		*(.rodata1)
		*(.dynsbss)
		*(.gnu.linkonce.sb.*)
		*(.scommon)
		*(.gnu.linkonce.sb2.*)
		*(.sbss)
		*(.sbss.*)
		*(.sbss2)
		*(.sbss2.*)
		*(.dynbss)
		*(.data)
		*(.data.*)
		*(.got)
		*(.got.*)
		__DATA_END__ = .;
	} >TEXT

	.bss : ALIGN(16) {
		__BSS_BEGIN__ = .;
		*(.bss)
		*(.bss.*)
		__BSS_END__ = .;
	} >TEXT

	.heap : ALIGN(16) {
		_sheap = .;
		. = . + __heap_size;
		_eheap = .;
	} >TEXT

	.stack : ALIGN(16) {
		_estack = .;
		. = . + __stack_size;
		_sstack = .;
	} >TEXT

	/DISCARD/ :
	{
		*(.interp)
		*(.dynsym)
		*(.dynstr)
		*(.header)
	} : phdr
}
