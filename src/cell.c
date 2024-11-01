#include "cell.h"

#include "util.h"
#include "grid.h"

void updateSand(struct Cell cell, struct Coord crd) {
  int dx = rand() % 3 - 1;
  
  if(getCell(crd, 0, 1).type == CELL_EMPTY) {
    setCell(crd, 0, 0, (struct Cell) { .type = CELL_EMPTY });
    setCell(crd, 0, 1, cell);
  } else if(getCell(crd, dx, 1).type == CELL_EMPTY) {
    setCell(crd, 0, 0, (struct Cell) { .type = CELL_EMPTY });
    setCell(crd, dx, 1, cell);
  }
}

void (*const cellUpdateFnTbl[64])(struct Cell, struct Coord) = {
  [CELL_SAND] = updateSand,
};