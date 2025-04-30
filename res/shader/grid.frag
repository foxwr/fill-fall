#version 300 es

precision lowp usampler2D;
precision mediump float;

in vec2 UV;

uniform uint ticks;
uniform sampler2D palette;
uniform usampler2D grid;

out vec4 COLOR;

const float TAU = 6.2831f;
const uint FAM_LIQUID = 0x10u;
const vec2 PAL_SIZE = vec2(64, 16);

struct Cell { uint kind, family, variant; };

Cell raw2cell(uvec4 raw) {
  return Cell(
    raw.x & 0x3Fu,
    raw.x & 0x30u,
    raw.y >> 3 & 0x0Eu
  );
}

void main() {
  uvec4 raw = texture(grid, UV);
  Cell cell = raw2cell(raw);

  if(cell.family == FAM_LIQUID) {
    float phase = float(ticks + cell.variant) / 16.0 * TAU;
    cell.variant = uint(8.0 * sin(phase) + 8.0);
  }

  vec2 paletteUV = vec2(cell.kind, cell.variant) / PAL_SIZE;
  COLOR = texture(palette, paletteUV);
}