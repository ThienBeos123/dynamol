#ifndef _____DNML_ASM_MACROS
#define _____DNML_ASM_MACROS

// EVERY FUNCTION NAMES NEEDS
// TO START WITH AN UNDERSCORE (eg: _x86_add64c)
// FOR OS COMPATIBILITY REASONS

#if defined(__APPLE__)
    #define asm_text_section    .section __TEXT,__text,regular,pure_instructions
    #define asm_rodata_section  .section __TEXT,__const
    #define asm_bss_section     .section __DATA,__bss
    #define asm_data_section    .section __DATA,__data
#elif defined(__linux__) || defined(__unix__)
    #define asm_text_section    .section .text
    #define asm_rodata_section  .section .rodata
    #define asm_bss_section     .section .bss
    #define asm_data_section    .section .data
#elif defined(_WIN64)
    #define asm_text_section    .section .text
    #define asm_rodata_section  .section .rdata, "dr"
    #define asm_bss_section     .section .bss
    #define asm_data_section    .section .data
#else
    #error "Unsupported OS for assembly"
#endif

#endif