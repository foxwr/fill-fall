const screen = document.getElementById("screen");
const gl = screen.getContext("webgl2");

const sandbox = (await WebAssembly.instantiateStreaming(fetch('res/sandbox.wasm'))).instance.exports;

const size = sandbox.getGridSize();
screen.width = screen.height = size;

const gridData = new Uint8ClampedArray(sandbox.memory.buffer, sandbox.grid, 4 * size * size);

const vsSource = await fetch("res/shader/vert.glsl").then(d => d.text());
const fsSource = await fetch("res/shader/frag.glsl").then(d => d.text());

// Compile shader
function compileShader(src, type) {
  const shader = gl.createShader(type);
  gl.shaderSource(shader, src);
  gl.compileShader(shader);

  const success = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
  if(!success) console.error("Shader error:", gl.getShaderInfoLog(shader));

  return shader;
}

// Set up shaders and program
const program = gl.createProgram();
gl.attachShader(program, compileShader(vsSource, gl.VERTEX_SHADER));
gl.attachShader(program, compileShader(fsSource, gl.FRAGMENT_SHADER));
gl.linkProgram(program);
gl.useProgram(program);

// Set up vertex data
const vertexData = new Float32Array([
  -1,  1,    1,  1,   -1, -1,
   1,  1,    1, -1,   -1, -1
]);

const buffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
gl.bufferData(gl.ARRAY_BUFFER, vertexData, gl.STATIC_DRAW);

const posLoc = gl.getAttribLocation(program, "pos");
gl.enableVertexAttribArray(posLoc);
gl.vertexAttribPointer(posLoc, 2, gl.FLOAT, false, 0, 0);



const texture = gl.createTexture();
gl.bindTexture(gl.TEXTURE_2D, texture);
gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA8UI, size, size, 0, gl.RGBA_INTEGER, gl.UNSIGNED_BYTE, null);
gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

const texLoc = gl.getUniformLocation(program, 'tex');
gl.uniform1i(texLoc, 0);

gl.viewport(0, 0, screen.width, screen.height);

requestAnimationFrame(draw);
setInterval(loop, 10);

let active, x, y;
screen.onmousemove = onmouse;
screen.onmousedown = onmouse;
screen.onmouseup = onmouse;

function onmouse(e) {
  active = e.buttons & 1;
  x = e.offsetX;
  y = e.offsetY;
}

let dx, dy = 0;
function loop() {
  if(dy == 0) {
    dy = 10;
    dx = ~~(Math.random() * size); 
  }
  sandbox.placeOnGrid(dx, 0, 1); dy--;
  if(active) sandbox.placeOnGrid(x, y, 1);
  sandbox.updateGrid();
}

function draw() {
  gl.texSubImage2D(gl.TEXTURE_2D, 0, 0, 0, size, size, gl.RGBA_INTEGER, gl.UNSIGNED_BYTE, gridData);
  gl.drawArrays(gl.TRIANGLES, 0, 6);
  requestAnimationFrame(draw);
}