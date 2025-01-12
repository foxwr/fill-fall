#version 300 es

#define AIR 0u
#define TYPE_MASK 0x3Fu
#define SPEC_MASK 0x30u
#define SHFT_MASK 0x0Fu
#define IS_LIQUID 0x10u
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

  uint type = cell.x & TYPE_MASK;
  if(type == AIR) discard;

  uint shift = cell.y & SHFT_MASK;
  uint species = type & SPEC_MASK;

  if(species == IS_LIQUID) {
    float phase = float(ticks + shift) / 16.0 * TAU;
    shift = uint(8.0 * sin(phase) + 8.0);
  }

  vec2 paletteSize = vec2(64, 16);
  vec2 paletteUV = vec2(type, shift) / paletteSize;
  
  COLOR = texture(palette, paletteUV);
}