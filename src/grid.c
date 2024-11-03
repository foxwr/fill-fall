#include "grid.h"

#include "util.h"
#include "cell.h"

static int tick = 0;

EXPORT struct Cell grid[SIZE][SIZE];
EXPORT int getGridSize() { return SIZE; }

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

EXPORT void placeOnGrid(int x, int y, int type) {
  struct Coord crd = { x, y };
  struct Cell cell = (struct Cell) {
    .type = type,
    .tick = tick,
    .shft = rand()
  };

  setCell(crd, 0, 0, cell);
}

EXPORT void updateGrid() {
  tick = !tick;

  struct Coord c;
  for(c.y = 0; c.y < SIZE; c.y++) {
    for(c.x = 0; c.x < SIZE; c.x++) {
      struct Cell *cell = &grid[c.y][c.x];
      if(cell->tick != tick && cellUpdateFnTbl[cell->type]) {
        cell->tick = tick;
        cellUpdateFnTbl[cell->type](*cell, c);
      }
    }
  }
}