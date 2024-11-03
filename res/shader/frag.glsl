#version 300 es
precision lowp usampler2D;
precision mediump float;

in vec2 uv;
uniform usampler2D tex;
out vec4 frag;

void main() {
  float type = float(texture(tex, uv).r & 63u);
  float var = float(texture(tex, uv).g & 15u) / 15.0;

  vec4 base = vec4(0.85, 0.70, 0.40, 1.00);
  float lum = var * 0.2 + 0.9;

  frag = base * lum * type;
}