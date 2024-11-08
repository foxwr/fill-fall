#version 300 es

precision lowp usampler2D;
precision mediump float;

in vec2 uv;
out vec4 fragColor;

uniform uint ticks;
uniform usampler2D grid;
uniform sampler2D base;

void main() {
  uint type = texture(grid, uv).r & 63u;
  if(type == 0u) discard;

  uint isLiquid = (type >> 4) & 1u;

  uint md = texture(grid, uv).g + (ticks + uint(uv.y * 43.0 + uv.x * 103.0)) * isLiquid;
  if((md & 16u) == 16u) md = ~md;

  float x = float(type & 15u) / 16.0;
  float y = float(type >> 4) / 4.0;

  float var = float(md & 15u) / 15.0;

  vec4 b = texture(base, vec2(x, y));
  float lum = var * 0.10 + 0.95;

  fragColor = b * lum;
}