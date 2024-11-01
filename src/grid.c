#include "grid.h"

#include "util.h"
#include "cell.h"

static int tick = 0;

EXPORT struct Cell grid[SIZE][SIZE];
EXPORT int getGridSize() { return SIZE; }

EXPORT void placeOnGrid(int x, int y, int type) {
  grid[y][x] = (struct Cell) {
    .type = type,
    .tick = tick,
    .shft = rand() & 63,
    .reg1 = 0xFF,
    .reg2 = 0xFF
  };
}

EXPORT void updateGrid() {
  tick = !tick;
  for(int y = 0; y < SIZE; y++) {
    for(int x = 0; x < SIZE; x++) {
      struct Cell cell = grid[y][x];
      if(cell.tick != tick && cellUpdateFnTbl[cell.type]) {
        cell.tick = tick;
        cellUpdateFnTbl[cell.type](cell, (struct Coord) { .x = x, .y = y });
      }
    }
  }
}

struct Cell getCell(struct Coord crd, int x, int y) {
  x += crd.x;
  y += crd.y;

  if(x < 0 || x >= SIZE || y < 0 || y >= SIZE)
    return (struct Cell) { .type = CELL_BORDER };
  else
    return grid[y][x];
}

void setCell(struct Coord crd, int x, int y, struct Cell cell) {
  x += crd.x;
  y += crd.y;

  if(x >= 0 && x < SIZE && y >= 0 && y < SIZE)
    grid[y][x] = cell;
}