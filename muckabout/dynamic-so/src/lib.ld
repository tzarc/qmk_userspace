MEMORY
{
    FLASH (rwx)            : ORIGIN = 0x00300000, LENGTH = 512K
    RAM(rwx)               : ORIGIN = 0x00400000, LENGTH = 512K
    NONRESIDENT (rwx)      : ORIGIN = 0x00500000, LENGTH = 512K
}

/* Explicitly mention label for the program's entry point */
ENTRY(keymap_init)

SECTIONS
{
    /* The 'text' section contains the main program code. */
    .text :
    {
        *(.keymap_init) /* must be first, so that we can jump directly to init */
        *(.text.startup.main)
        *(.text.main)
        *(.text*)
        *(.glue_7)         /* glue arm to thumb code */
        *(.glue_7t)        /* glue thumb to arm code */
        *(.eh_frame)

        KEEP (*(.init))
        KEEP (*(.fini))

        /* The 'rodata' section contains read-only data,
        * constants, strings, information that won't change.
        * variables defined with const keyword in C */
        *(.rodata*)

        . = ALIGN(4);
    } >FLASH

    .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
    .ARM :
    {
        PROVIDE_HIDDEN (__exidx_start = .);
        *(.ARM.exidx*)
        PROVIDE_HIDDEN (__exidx_end = .);
    } >FLASH

    .preinit_array     :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } >FLASH

    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    } >FLASH

    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } >FLASH

    /* PLT section contains code for accessing the dynamically linked functions
     * ie funtions from shared libraries in a postion independent manner */
    .plt :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__plt_start = .);
        *(.plt)
        . = ALIGN(4);
        PROVIDE_HIDDEN (__plt_end = .);
    } >FLASH

    /* The 'data' section is space set aside in RAM for
     * things like non-zero global and static variables, which can change. */
    .data :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__data_start = .);
        *(.sys_struct)
        *(.data*)
        . = ALIGN(4);
        PROVIDE_HIDDEN (__data_end = .);
    } >RAM

    .got :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__got_start = .);
        *(.got)
        *(.got.plt)
        PROVIDE_HIDDEN (__got_end = .);
    } > RAM

    /* The 'bss' section is similar to the 'data' section,
    *  but its space is initialized to all 0s at the
    *  start of the program. */
    .bss :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__bss_start = .);
        *(.bss*)
        /* A special notation is needed for common symbols,
         * because in many object file formats common symbols
         * do not have a particular input section. The linker
         * treats common symbols as though they are in an
         * input section named ‘COMMON’.*/
        *(COMMON)
        . = ALIGN(4);
        PROVIDE_HIDDEN (__bss_end = .);
    } >RAM

    /* The 'nonresident' section is space set aside in ROM where
       data can be kept out of RAM but still be accessed by the MCU. */
    .nonresident :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__nonresident_start = .);
        *(.nonresident*)
        . = ALIGN(4);
        PROVIDE_HIDDEN (__nonresident_end = .);
    } >NONRESIDENT

    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
