#include "cell.h"

#include "util.h"
#include "grid.h"

static const struct Cell EMPTY = { };
static const int dx[] = { -1, 0, 1,  0 };
static const int dy[] = {  0, 1, 0, -1 };

void updatePowder(struct Cell cell, u32 crd) {
  const u32 dx = (rand() & 2) - 1;
  
  struct Cell nbr = getCell(crd, 0, 1);
  if((nbr.kind1 & MASK_FAMILY) == FAMILY_GAS || (nbr.kind1 & MASK_FAMILY) == FAMILY_LIQUID) {
    setCell(crd, 0, 0, nbr);
    setCell(crd, 0, 1, cell);
    return;
  }
  
  nbr = getCell(crd, dx, 1);
  if((nbr.kind1 & MASK_FAMILY) == FAMILY_GAS) {
    setCell(crd, 0, 0, nbr);
    setCell(crd, dx, 1, cell);
  }
}

void updateLiquid(struct Cell cell, u32 crd) {
  if((getCell(crd, 0, 1).kind1 & MASK_FAMILY) == FAMILY_GAS) {
    setCell(crd, 0, 0, EMPTY);
    setCell(crd, 0, 1, cell);
    return;
  }

  u32 dx = (rand() & 2) - 1;

  if((getCell(crd, dx, 0).kind1 & MASK_FAMILY) == FAMILY_GAS) {
    setCell(crd, 0, 0, EMPTY);
    setCell(crd, dx, 0, cell);
  } else if((getCell(crd, -dx, 0).kind1 & MASK_FAMILY) == FAMILY_GAS) {
    setCell(crd, 0, 0, EMPTY);
    setCell(crd, -dx, 0, cell);
  } else if((rand() & 31) == 0) {
    u32 dy = (rand() & 3) - 1;
    struct Cell nbr = getCell(crd, dx, dy);
    u32 spec = nbr.kind1 & MASK_FAMILY;

    if(spec == FAMILY_LIQUID && nbr.kind1 != cell.kind1) {
      setCell(crd, 0, 0, nbr);
      setCell(crd, dx, dy, cell);
    }
  }
}

void updateAcid(struct Cell cell, u32 crd) {
  int i = rand() & 3;
  int kind = getCell(crd, dx[i], dy[i]).kind1;

  if(kind >= FAMILY_POWDER && kind != CELL_INVALID && kind != CELL_GLASS) {
    setCell(crd, dx[i], dy[i], EMPTY);
    setCell(crd, 0, 0, EMPTY);
  } else updateLiquid(cell, crd);
}

static u32 sinked[64] = { };

void updateSink(struct Cell cell, u32 crd) {
  for(int i = 0; i < 4; i++) {
    struct Cell nbr = getCell(crd, dx[i], dy[i]);
    if(nbr.kind1 != cell.kind1 && nbr.kind1 < FAMILY_SOLID) {
      sinked[nbr.kind1]++;
      setCell(crd, dx[i], dy[i], EMPTY);
    }
  }
}

void updatePump(struct Cell cell, u32 crd) {
  u32 v[64], n = 0;
  for(int i = 0; i < 64; i++)
    if(sinked[i])
      v[n++] = i;
  
  if(n == 0) return;

  int kind = rand() % n;

  int i = rand() & 3;
  if(getCell(crd, dx[i], dy[i]).kind1 == CELL_AIR) {
    setCell(crd, dx[i], dy[i], newCell(v[kind], 0, crd));
    sinked[v[kind]]--;
  }
}

void updateCloner(struct Cell cell, u32 crd) {
  if(cell.kind2 >= FAMILY_SOLID) return;
  if(rand() & 7) return;

  int i = rand() & 3;
  if(getCell(crd, dx[i], dy[i]).kind1 == CELL_AIR)
    setCell(crd, dx[i], dy[i], newCell(cell.kind2, 0, crd));
}

void updateMimic(struct Cell cell, u32 crd) {
  for(int i = 0; i < 4; i++) {
    struct Cell nbr = getCell(crd, dx[i], dy[i]);
    if(nbr.kind1 == CELL_INVALID) continue;
    
    if(nbr.kind1 != CELL_AIR && nbr.kind1 != cell.kind1)
      setCell(crd, 0, 0, newCell(nbr.kind1, 0, crd));
  }
}

void createGlass(struct Cell *cell, u32 x, u32 y) {
  int nbrGlass = 0;

  for(int i = 0; i < 4; i++)
    if(getCell(0, x + dx[i], y + dy[i]).kind1 == CELL_GLASS)
      nbrGlass++;

  if(nbrGlass < 4 || ((x + y) % 8 == 0 && (x - y) % 8 < 3)) cell->variant |= 4;
  else cell->variant &= 3;
}

void updateGlass(struct Cell cell, u32 crd) {
  u32 x, y;
  coordToXY(crd, &x, &y);

  createGlass(&cell, x, y);

  setCell(crd, 0, 0, cell);
}

void createBricks(struct Cell *cell, u32 x, u32 y) {
  if(y % 4 == 0 || (x + y / 4 * 2) % 6 == 0)
    cell->variant &= 3;
  else cell->variant |= 4;
}

void (*const cellUpdateFnTbl[64])(struct Cell, u32) = {
  [CELL_WATER] = updateLiquid,
  [CELL_ACID] = updateAcid,
  [CELL_SAND] = updatePowder,
  [CELL_SINK] = updateSink,
  [CELL_PUMP] = updatePump,
  [CELL_CLONER] = updateCloner,
  [CELL_MIMIC] = updateMimic,
  [CELL_GLASS] = updateGlass,
};

void (*const cellCreateFnTbl[64])(struct Cell *, u32, u32) = {
  [CELL_BRICKS] = createBricks,
  [CELL_GLASS] = createGlass
};

struct Cell newCell(u32 kind1, u32 kind2, u32 crd) {
  struct Cell cell = {
    .kind1 = kind1,
    .kind2 = kind2,
    .variant = rand(),
  };

  if(cellCreateFnTbl[kind1]) {
    u32 x, y;
    coordToXY(crd, &x, &y);
    cellCreateFnTbl[kind1](&cell, x, y);
  }

  return cell;
}
