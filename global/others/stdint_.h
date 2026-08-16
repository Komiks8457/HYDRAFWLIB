#pragma once

#define INT8_MIN            (-128)
#define INT8_MAX            (127)
#define UINT8_MAX           (255U)

#define INT16_MIN           (-32768)
#define INT16_MAX           (32767)
#define UINT16_MAX          (65535U)

#define INT32_MIN           (-2147483647 - 1)
#define INT32_MAX           (2147483647)
#define UINT32_MAX          (4294967295U)

#define LONG32_MIN          (-2147483647L - 1L)
#define LONG32_MAX          (2147483647L)
#define ULONG32_MAX         (4294967295UL)

#define INT64_MIN           (-9223372036854775807LL - 1)
#define INT64_MAX           (9223372036854775807LL)
#define UINT64_MAX          (18446744073709551615ULL)

typedef signed char         int8_t;
typedef signed short int    int16_t;
typedef signed int          int32_t;
typedef signed long int     long32_t;
typedef signed __int64      int64_t;

typedef unsigned char       uint8_t;
typedef unsigned short int  uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long int   ulong32_t;
typedef unsigned __int64    uint64_t;

typedef uint8_t             _BYTE;
typedef uint16_t            _WORD;
typedef uint32_t            _BOOL;
typedef ulong32_t           _DWORD;
typedef uint64_t            _DLONG;
