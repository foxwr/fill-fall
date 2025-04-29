#pragma once

#include "def.h"

struct Cell getCell(u32 crd, i32 x, i32 y);
void setCell(u32 crd, i32 x, i32 y, struct Cell cell);
void coordToXY(u32 crd, u32 *x, u32 *y);
