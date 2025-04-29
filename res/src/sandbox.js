const req = fetch("res/sandbox.wasm");
const sandboxBinary = await WebAssembly.instantiateStreaming(req);

export const grid = { };
export const api = sandboxBinary.instance.exports;

export function init(width, height) {
  api.init(width, height);

  grid.width = width;
  grid.height = height;
  grid.data = new Uint8Array(
    api.memory.buffer, api.grid,
    2 * width * height
  );
}
