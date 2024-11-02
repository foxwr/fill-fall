const screen = document.getElementById("screen");
const gl = screen.getContext("2d");

const sandbox = (await WebAssembly.instantiateStreaming(fetch('res/sandbox.wasm'))).instance.exports;

const size = sandbox.getGridSize();
screen.width = screen.height = size;

const gridData = new Uint8ClampedArray(sandbox.memory.buffer, sandbox.grid, 4 * size * size);
const gridImage = new ImageData(gridData, size, size);

requestAnimationFrame(loop);

let active = false, x, y;
screen.onmousemove = (e) => { x = e.offsetX; y = e.offsetY }
screen.onmousedown = (e) => { active = true; screen.onmousemove(e) }
screen.onmouseup = (e) => { active = false }

function loop() {
  if(active) sandbox.placeOnGrid(x, y, 1);
  sandbox.updateGrid();
  gl.putImageData(gridImage, 0, 0);
  requestAnimationFrame(loop);
}