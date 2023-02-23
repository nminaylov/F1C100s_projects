#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define ALIGN .align 0
#define ALIGN_STR ".align 0"

#define ENTRY(name) \
    .globl name;    \
    ALIGN;          \
    name:

#define WEAK(name) \
    .weak name;    \
    name:

#define END(name) .size name, .- name

#define ENDPROC(name)       \
    .type name, % function; \
    END(name)

#ifdef __cplusplus
}
#endif
