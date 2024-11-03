#version 300 es

in vec2 pos;
out vec2 uv;

void main() {
  uv = (pos) * vec2(0.5,-0.5) + 0.5;
  gl_Position = vec4(pos, 0, 1);
}