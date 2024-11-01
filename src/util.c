#include "util.h"

u32 rand() {
  static u32 seed = 1;
  seed = 1664525 * seed + 1013904223;
  return seed;
}