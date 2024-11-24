#include "cell.h"

#include "util.h"
#include "grid.h"

const struct Cell airCell = { CELL_AIR };

void updatePowder(struct Cell cell, struct Coord crd) {
  u32 dx = (rand() & 2) - 1;
  
  struct Cell nbr = getCell(crd, 0, 1);
  if((nbr.type & SPEC_MASK) == IS_GAS || (nbr.type & SPEC_MASK) == IS_LIQUID) {
    setCell(crd, 0, 0, nbr);
    setCell(crd, 0, 1, cell);
    return;
  }
  
  nbr = getCell(crd, dx, 1);
  if((nbr.type & SPEC_MASK) == IS_GAS) {
    setCell(crd, 0, 0, nbr);
    setCell(crd, dx, 1, cell);
  }
}

void updateLiquid(struct Cell cell, struct Coord crd) {
  if((getCell(crd, 0, 1).type & SPEC_MASK) == IS_GAS) {
    setCell(crd, 0, 0, airCell);
    setCell(crd, 0, 1, cell);
    return;
  }

  u32 dx = (rand() & 2) - 1;
  if((getCell(crd, dx, 0).type & SPEC_MASK) == IS_GAS) {
    setCell(crd, 0, 0, airCell);
    setCell(crd, dx, 0, cell);
  } else if((getCell(crd, -dx, 0).type & SPEC_MASK) == IS_GAS) {
    setCell(crd, 0, 0, airCell);
    setCell(crd, -dx, 0, cell);
  }
}

void updateHole(struct Cell cell, struct Coord crd) {
  static int dx[] = { -1, 0, 1,  0 };
  static int dy[] = {  0, 1, 0, -1 };

  for(int i = 0; i < 4; i++) {
    struct Cell nbr = getCell(crd, dx[i], dy[i]);
    if(nbr.type != cell.type)
      setCell(crd, dx[i], dy[i], airCell);
  }
}

void updateFaucet(struct Cell cell, struct Coord crd) {
  static int dx[] = { -1, 0, 1,  0 };
  static int dy[] = {  0, 1, 0, -1 };

  if(cell.reg1) {
    for(int i = 0; i < 4; i++)
      if(getCell(crd, dx[i], dy[i]).type == CELL_AIR)
        placeOnGrid(crd.x + dx[i], crd.y + dy[i], cell.reg1);
  } else {
    for(int i = 0; i < 4; i++) {
      struct Cell nbr = getCell(crd, dx[i], dy[i]);
      if(nbr.type == cell.type && nbr.reg1 != CELL_AIR)
        cell.reg1 = nbr.reg1;
      
      if(nbr.type != cell.type && nbr.type != CELL_AIR)
        cell.reg1 = nbr.type;
    }

    setCell(crd, 0, 0, cell);
  }
}

void updateMimic(struct Cell cell, struct Coord crd) {
  static int dx[] = { -1, 0, 1,  0 };
  static int dy[] = {  0, 1, 0, -1 };

  for(int i = 0; i < 4; i++) {
    struct Cell nbr = getCell(crd, dx[i], dy[i]);
    if(nbr.type != CELL_AIR && nbr.type != cell.type)
      placeOnGrid(crd.x, crd.y, nbr.type);
  }
}

void (*const cellUpdateFnTbl[64])(struct Cell, struct Coord) = {
  [CELL_WATER] = updateLiquid,
  [CELL_SAND] = updatePowder,
  [CELL_HOLE] = updateHole,
  [CELL_FAUCET] = updateFaucet,
  [CELL_MIMIC] = updateMimic
};