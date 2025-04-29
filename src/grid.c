#include "grid.h"

#include "util.h"
#include "cell.h"

static u32 tick, width, height;

EXPORT struct Cell grid[512*512];

EXPORT void init(u32 w, u32 h) {
  width = w;
  height = h;
}

EXPORT void placeAt(i32 x, i32 y, i32 sz, u32 kind1, u32 kind2) {
  for(int i = -sz; i <= sz; i++) {
    for(int j = -sz; j <= sz; j++) {
      i32 ox = x + i;
      i32 oy = y + j;
      setCell(0, ox, oy, newCell(kind1, kind2, oy * width + ox));
    }
  }
}

EXPORT void fill(i32 x, i32 y, u32 kind) {
  static int dx[] = { -1, 0, 1,  0 };
  static int dy[] = {  0, 1, 0, -1 };
  
  struct Cell cell = getCell(0, x, y);
  u32 replacedKind = cell.kind1;
  u32 tick = !cell.tick;

  if(replacedKind == CELL_INVALID) return;

  u32 top = 1;
  static i32 stack[512 * 512][2];
  stack[0][0] = x;
  stack[0][1] = y;

  while(top) {
    top--;
    x = stack[top][0];
    y = stack[top][1];

    cell = newCell(kind, 0, y * width + x);
    cell.tick = tick;
    setCell(0, x, y, cell);
    
    for(int i = 0; i < 4; i++) {
      cell = getCell(0, x + dx[i], y + dy[i]);
      if(cell.kind1 == replacedKind && cell.tick != tick) {
        stack[top][0] = x + dx[i];
        stack[top][1] = y + dy[i];
        top++;
      }
    }
  }
}

EXPORT void update(void) {
  tick = !tick;

  static u32 ri[2048] = { -1 };
  if(*ri == -1)
    for(int i = 0; i < width; i++)
      ri[i] = i;

  u32 crd = 0;
  for(int y = 0; y < height; y++) {
    u32 i = rand() % width;
    u32 j = rand() % width;
    u32 a = ri[i];
    ri[i] = ri[j];
    ri[j] = a;

    for(u32 x = 0; x < width; x++) {
      struct Cell *cell = (struct Cell *) grid + crd + ri[x];
      if(cell->tick != tick && cellUpdateFnTbl[cell->kind1]) {
        cell->tick = tick;
        cellUpdateFnTbl[cell->kind1](*cell, crd + ri[x]);
      }
    }

    crd += width;
  }
}

struct Cell getCell(u32 crd, i32 x, i32 y) {
  x += crd % width;
  y += crd / width;

  if(x < 0 || x >= width || y < 0 || y >= height)
    return (struct Cell) { .kind1 = CELL_INVALID };
  else
    return grid[y * width + x];
}

void setCell(u32 crd, i32 x, i32 y, struct Cell cell) {
  x += crd % width;
  y += crd / width;

  if(x >= 0 && x < width && y >= 0 && y < height)
    grid[y * width + x] = cell;
}

void coordToXY(u32 crd, u32 *x, u32 *y) {
  *x = crd % width;
  *y = crd / width;
}
