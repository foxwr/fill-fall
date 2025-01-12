#version 300 es

in vec2 VERTEX;
out vec2 UV;

void main() {
  UV = VERTEX * vec2(0.5,-0.5) + 0.5;
  
  gl_Position = vec4(VERTEX, 0, 1);
}