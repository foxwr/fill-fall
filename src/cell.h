#ifndef CELL_H
#define CELL_H

#include "def.h"

#define IS_GAS    0b000000
#define IS_LIQUID 0b010000
#define IS_POWDER 0b100000
#define IS_SOLID  0b110000

enum CellType {
  CELL_AIR = IS_GAS,
  
  CELL_WATER = IS_LIQUID,

  CELL_SAND = IS_POWDER,

  CELL_BORDER = IS_SOLID | 15
};

struct Cell {
  u8 type : 6;
  u8 tick : 2;
  u8 shft : 4;
  u8 regH : 4;
  u8 reg1 : 8;
  u8 reg2 : 8;
} PACKED;

extern void (*const cellUpdateFnTbl[64])(struct Cell, struct Coord);

#endif
