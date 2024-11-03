#include "cell.h"

#include "util.h"
#include "grid.h"

const struct Cell airCell = { CELL_AIR };

void updateSand(struct Cell cell, struct Coord crd) {
  int dx = (rand() & 2) - 1;
  
  struct Cell nbr = getCell(crd, 0, 1);
  if(nbr.type == CELL_AIR || nbr.type == CELL_WATER) {
    setCell(crd, 0, 0, nbr);
    setCell(crd, 0, 1, cell);
    return;
  }
  
  nbr = getCell(crd, dx, 1);
  if(nbr.type == CELL_AIR) {
    setCell(crd, 0, 0, nbr);
    setCell(crd, dx, 1, cell);
  }
}

void updateWater(struct Cell cell, struct Coord crd) {
  int dx = (rand() & 2) - 1;
  
  if(getCell(crd, 0, 1).type == CELL_AIR) {
    setCell(crd, 0, 0, airCell);
    setCell(crd, 0, 1, cell);
  } else if(getCell(crd, dx, 1).type == CELL_AIR) {
    setCell(crd, 0, 0, airCell);
    setCell(crd, dx, 1, cell);
  } else if(getCell(crd, -dx, 1).type == CELL_AIR) {
    setCell(crd, 0, 0, airCell);
    setCell(crd, -dx, 1, cell);
  } else if(getCell(crd, dx, 0).type == CELL_AIR) {
    setCell(crd, 0, 0, airCell);
    setCell(crd, dx, 0, cell);
  } else if(getCell(crd, -dx, 0).type == CELL_AIR) {
    setCell(crd, 0, 0, airCell);
    setCell(crd, -dx, 0, cell);
  }
}

void (*const cellUpdateFnTbl[64])(struct Cell, struct Coord) = {
  [CELL_SAND] = updateSand,
  [CELL_WATER] = updateWater
};