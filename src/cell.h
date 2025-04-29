#pragma once

#include "def.h"

#define MASK_FAMILY 0b110000

#define FAMILY_GAS    0b000000
#define FAMILY_LIQUID 0b010000
#define FAMILY_POWDER 0b100000
#define FAMILY_SOLID  0b110000

enum CellKind {
  CELL_AIR = FAMILY_GAS,
  
  CELL_WATER = FAMILY_LIQUID,
  CELL_ACID,

  CELL_SAND = FAMILY_POWDER,

  CELL_INVALID = FAMILY_SOLID,
  CELL_ROCK,
  CELL_SINK,
  CELL_PUMP,
  CELL_CLONER,
  CELL_MIMIC,
  CELL_BRICKS,
  CELL_GLASS,
};

struct Cell {
  u8 kind1    : 6;
  u8 kind2    : 6;
  u8 variant  : 3;
  u8 tick     : 1;
} PACKED;

// extern void (*const familyUpdateFnTbl[4])(struct Cell *);

extern void (*const cellUpdateFnTbl[64])(struct Cell, u32);
extern void (*const cellCreateFnTbl[64])(struct Cell *, u32, u32);

struct Cell newCell(u32 kind1, u32 kind2, u32 crd);
