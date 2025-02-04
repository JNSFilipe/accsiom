#ifndef AXTYPES_H_
#define AXTYPES_H_

#include <stdint.h>   // Standard integer types
#include <stddef.h>   // Size types
#include <stdbool.h>  // Boolean type

// Default: Immutable Zig-style types (const by default)
typedef const char        ch;
typedef const uint8_t     u8;
typedef const uint16_t    u16;
typedef const uint32_t    u32;
typedef const uint64_t    u64;

typedef const int8_t      i8;
typedef const int16_t     i16;
typedef const int32_t     i32;
typedef const int64_t     i64;

typedef const float       f32;
typedef const double      f64;
typedef const long double f128;

typedef const size_t      usz;   // Unsigned size type
typedef const ptrdiff_t   ptrdif;  // Signed pointer difference type

typedef const bool        b8;  // Boolean (from stdbool.h)

// Mutable versions (explicit opt-in, prefixed with 'm')
typedef char        mch;
typedef uint8_t     mu8;
typedef uint16_t    mu16;
typedef uint32_t    mu32;
typedef uint64_t    mu64;

typedef int8_t      mi8;
typedef int16_t     mi16;
typedef int32_t     mi32;
typedef int64_t     mi64;

typedef float       mf32;
typedef double      mf64;
typedef long double mf128;

typedef size_t      musz;
typedef ptrdiff_t   mptrdif;

typedef bool        mb8;

#endif // AXTYPER_H_
