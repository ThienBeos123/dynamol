#ifndef ____ARM64_MACROS_H
#define ____ARM64_MACROS_H

#if defined(__ELF__)
    #define FUNC_TYPE(name)  .type name, %function
    #define FUNC_SIZE(name)  .size name, .- name
#elif defined(__MACH__)
    #define FUNC_TYPE(name)
    #define FUNC_SIZE(name)
#else
    #define FUNC_TYPE(name)
    #define FUNC_SIZE(name)
#endif



#endif