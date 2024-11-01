const screen = document.getElementById("screen");
const gl = screen.getContext("2d");

const sandbox = (await WebAssembly.instantiateStreaming(fetch('res/sandbox.wasm'))).instance.exports;

const size = sandbox.getGridSize();
screen.width = screen.height = size;

const gridData = new Uint8ClampedArray(sandbox.memory.buffer, sandbox.grid, 4 * size * size);
const gridImage = new ImageData(gridData, size, size);

requestAnimationFrame(loop);

// maxbittker.com/making-sandspiel

let x = 0;
function loop() {
  x = (x + .1) % size;
  sandbox.placeOnGrid(~~x, 40, 1);
  sandbox.updateGrid();
  gl.putImageData(gridImage, 0, 0);
  requestAnimationFrame(loop);
}