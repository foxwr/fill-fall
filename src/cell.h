#ifndef CELL_H
#define CELL_H

#include "def.h"

enum CellType {
  CELL_EMPTY = 0,
  CELL_SAND,

  CELL_BORDER = 63
};

struct Cell {
  u8 type : 6;
  u8 tick : 2;
  u8 shft : 8;
  u8 reg1 : 8;
  u8 reg2 : 8;
} PACKED;

extern void (*const cellUpdateFnTbl[64])(struct Cell, struct Coord);

#endif
