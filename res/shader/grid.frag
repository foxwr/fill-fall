#version 300 es

#define KIND(x) ((x) & 0x3Fu)
#define FAMILY(x) ((x) & 0x30u)
#define VARIANT(x) ((x) >> 4 & 0x07u)

#define FAM_LIQUID 0x10u
#define TAU 6.2831

precision lowp usampler2D;
precision mediump float;

in vec2 UV;

uniform uint ticks;
uniform sampler2D palette;
uniform usampler2D grid;

out vec4 COLOR;

void main() {
  uvec4 cell = texture(grid, UV);

  uint kind = KIND(cell.x);
  uint family = FAMILY(kind);
  uint variant = VARIANT(cell.y) << 1;

  if(family == FAM_LIQUID) {
    float phase = float(ticks + variant) / 16.0 * TAU;
    variant = uint(8.0 * sin(phase) + 8.0);
  }

  vec2 paletteSize = vec2(64, 16);
  vec2 paletteUV = vec2(kind, variant) / paletteSize;
  
  COLOR = texture(palette, paletteUV);
}