#version 300 es

uniform mat3x2 transform;

out vec2 UV;

void main() {
  int idx = gl_VertexID % 3 + gl_VertexID / 3;
  vec3 pos = vec3(idx & 2, idx << 1 & 2, 2) - 1.0;

  UV = pos.xy * vec2(0.5, -0.5) + 0.5;
  gl_Position = vec4(transform * pos, 0, 1);
}