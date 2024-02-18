MEMORY
{
    TEXT (rx)              : ORIGIN = 0x0F400000, LENGTH = 0x00100000
    DATA (rwx)             : ORIGIN = 0x0F500000, LENGTH = 0x00100000
    NONRESIDENT (r)        : ORIGIN = 0x0F600000, LENGTH = 0x00100000
    OTHER (r)              : ORIGIN = 0xD0000000, LENGTH = 0x00100000
    LIBINFO (r)            : ORIGIN = 0xE0000000, LENGTH = 0x00100000
}

SECTIONS
{
    .lib_info : {
        LONG(__text_start);
        LONG(__text_end - __text_start);
        LONG(__code_start);
        LONG(__code_end - __code_start);
        LONG(__preinit_array_start);
        LONG((__preinit_array_end - __preinit_array_start)/4);
        LONG(__init_array_start);
        LONG((__init_array_end - __init_array_start)/4);
        LONG(__fini_array_start);
        LONG((__fini_array_end - __fini_array_start)/4);
        LONG(__data_start);
        LONG(__data_end - __data_start);
        LONG(__bss_start);
        LONG(__bss_end - __bss_start);
        LONG(__nonresident_start);
        LONG(__nonresident_end - __nonresident_start);
        LONG(__keymap_init);
    } >LIBINFO

    /* The 'text' section contains the main program code. */
    .text : {
        __text_section = LOADADDR(.text);
        __keymap_init = ABSOLUTE(keymap_init);

        PROVIDE_HIDDEN (__text_start = .);

        PROVIDE_HIDDEN (__code_start = .);
        *(.text.startup.main)
        *(.text.main)
        *(.text*)
        *(.glue_7)         /* glue arm to thumb code */
        *(.glue_7t)        /* glue thumb to arm code */
        *(.eh_frame)
        PROVIDE_HIDDEN (__code_end = .);

        . = ALIGN(4);

        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(SORT(.preinit_array.*)))
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);

        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);

        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);

        . = ALIGN(4);

        PROVIDE_HIDDEN (__rodata_start = .);
        KEEP (*(.rodata*))
        PROVIDE_HIDDEN (__rodata_end = .);

        . = ALIGN(4);

        PROVIDE_HIDDEN (__text_end = .);
    } >TEXT

    .data : {
        PROVIDE_HIDDEN (__data_start = .);
        *(.sys_struct)
        *(.data*)
        . = ALIGN(4);
        PROVIDE_HIDDEN (__data_end = .);
    } >DATA

    .bss : {
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
    } >DATA

    /* The 'nonresident' section is space set aside in ROM where
       data can be kept out of RAM but still be accessed by the MCU. */
    .nonresident : {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__nonresident_start = .);
        *(.nonresident*)
        . = ALIGN(4);
        PROVIDE_HIDDEN (__nonresident_end = .);
    } >NONRESIDENT

    .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >OTHER
    .ARM : {
        PROVIDE_HIDDEN (__exidx_start = .);
        *(.ARM.exidx*)
        PROVIDE_HIDDEN (__exidx_end = .);
    } >OTHER

    .got : {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__got_start = .);
        *(.got)
        *(.got.plt)
        *(.igot.plt)
        PROVIDE_HIDDEN (__got_end = .);
    } >OTHER

    .dynamic : {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__dynamic_start = .);
        *(.dynamic)
        *(.dynamic*)
        . = ALIGN(4);
        PROVIDE_HIDDEN (__dynamic_end = .);
    } >OTHER

    /* PLT section contains code for accessing the dynamically linked functions
     * ie funtions from shared libraries in a postion independent manner */
    .plt : {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__plt_start = .);
        *(.plt)
        . = ALIGN(4);
        PROVIDE_HIDDEN (__plt_end = .);
    } >OTHER

    /* Remove information from the standard libraries */
    /DISCARD/ : {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    .ARM.attributes 0 : { *(.ARM.attributes) } >OTHER
}
