#ifndef __DEFINES_H_
#define __DEFINES_H_
#include "common.h"

typedef uint16_t        u16;
typedef uint8_t         u8;
typedef uint32_t        u32;
typedef int16_t         i16;
typedef int8_t          i8;
typedef int32_t         i32;

typedef _Float16        f16;
typedef float           f32;
typedef double          f64;

#define IDENT_START     "@:abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define IDENT_MIDDLE    ":abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"
#define KEYWORD_CHARS   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define NUMERIC_START   "#$0123456789ABCDEFabcdef"
#define NUMERIC_MIDDLE  "$0123456789ABCDEFabcdef"
#define LABEL_CHARS     "ABCDEFGHIJKLMNOPQRSTUVWXYZ_"
#define REGNAMES        "ABCXYZ"
#define DECIMAL_CHARS   "0123456789"
#define HEX_CHARS       "ABCDEF"

namespace sh {
    constexpr int CPU_MEMORY_SIZE = 0x10000;
    constexpr int CPU_LAST_INDEX  = 0x0ffff;
    constexpr int CPU_VRAM_START  = 0x0bfff;
    constexpr int CPU_STACK_START = CPU_VRAM_START - 0x100;

    constexpr int CPU_ROM_START   = 0x0000;
    constexpr int CPU_RAM_START   = 0x8000;


}

#endif