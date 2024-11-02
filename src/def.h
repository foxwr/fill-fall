#ifndef DEF_H
#define DEF_H

#define PACKED __attribute__((packed))
#define EXPORT __attribute__((visibility("default")))

typedef __UINT8_TYPE__    u8;
typedef __UINT16_TYPE__   u16;
typedef __UINT32_TYPE__   u32;
typedef __UINT64_TYPE__   u64;
typedef __UINTPTR_TYPE__  uptr;

typedef __INT8_TYPE__   i8;
typedef __INT16_TYPE__  i16;
typedef __INT32_TYPE__  i32;
typedef __INT64_TYPE__  i64;
typedef __INTPTR_TYPE__ iptr;

struct Coord { u16 x, y; } PACKED;

#endif
