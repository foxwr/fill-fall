const sandbox = (await WebAssembly.instantiateStreaming(fetch('res/sandbox.wasm'))).instance.exports;

const screen = document.getElementById("screen");
const gl = screen.getContext("webgl2");

const size = sandbox.getGridSize();
gl.viewport(0, 0, size, size);
screen.width = screen.height = size;

const gridData = new Uint8Array(sandbox.memory.buffer, sandbox.grid, 4 * size * size);

const program = await createProgram("res/shader/grid");

let scale = 1;
(onresize = resize)();

function resize() {
  scale = size / screen.clientHeight;
  gl.viewport(0, 0, screen.clientHeight, screen.clientHeight);
  screen.width = screen.height = screen.clientHeight;
}

async function createProgram(shPath) {
  const [vsrc, fsrc] = await Promise.all([
    fetch(`${shPath}.vert`).then(v => v.text()),
    fetch(`${shPath}.frag`).then(f => f.text())
  ]);

  const prog = gl.createProgram();
  gl.attachShader(prog, compileShader(vsrc, gl.VERTEX_SHADER));
  gl.attachShader(prog, compileShader(fsrc, gl.FRAGMENT_SHADER));

  gl.linkProgram(prog);
  gl.useProgram(prog);

  return prog;
}

function compileShader(src, type) {
  const shader = gl.createShader(type);
  gl.shaderSource(shader, src);
  gl.compileShader(shader);

  if(!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
    console.error("SHADER ERROR:", gl.getShaderInfoLog(shader));
    gl.deleteShader(shader);
    return null;
  }

  return shader;
}

const vbo = createVertBuffer(
  [-1, +1], [+1, +1], [-1, -1],
  [+1, +1], [+1, -1], [-1, -1]
);

function createVertBuffer(...data) {
  const buffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(data.flat()), gl.STATIC_DRAW);
  return buffer;
}

enableVertBuffer("pos", 2);

function enableVertBuffer(attr, size) {
  const prog = gl.getParameter(gl.CURRENT_PROGRAM);
  const attrLoc = gl.getAttribLocation(prog, attr);
  gl.enableVertexAttribArray(attrLoc);
  gl.vertexAttribPointer(attrLoc, size, gl.FLOAT, false, 0, 0);
}

const baseImg = new Image();
baseImg.src = "res/base.png";
await new Promise(r => baseImg.onload = r)

const baseTex = createTex("base", 0, gl.RGBA, gl.RGBA, baseImg.width, baseImg.height, baseImg);
const gridTex = createTex("grid", 1, gl.RGBA8UI, gl.RGBA_INTEGER, size, size);

function createTex(unif, activeTex, ifmt, fmt, width, height, data) {
  gl.activeTexture(gl.TEXTURE0 + activeTex);

  const tex = gl.createTexture();
  gl.bindTexture(gl.TEXTURE_2D, tex);
  gl.texImage2D(gl.TEXTURE_2D, 0, ifmt, width, height, 0, fmt, gl.UNSIGNED_BYTE, data);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

  const prog = gl.getParameter(gl.CURRENT_PROGRAM); 
  const unifLoc = gl.getUniformLocation(prog, unif);
  gl.uniform1i(unifLoc, activeTex);

  return tex;
}

const ticksLoc = gl.getUniformLocation(program, "ticks");

requestAnimationFrame(draw);

setInterval(loop, 8);
let paused = false;

const menu = document.querySelector("#actions");
menu.querySelector("#pause-btn").onclick = pause;
menu.querySelector("#reset-btn").onclick = reset;

function pause(e) {
  paused = !paused;
}

function reset() {
  gridData.fill(0);
}

let active, x, y;
screen.onmousemove = onmouse;
screen.onmousedown = onmouse;
screen.onmouseup = onmouse;
screen.onmouseout = onmouse;

const picker = document.querySelector(".picker");
let cellType = parseInt(picker.querySelector(".selected").getAttribute("data-p"));
picker.querySelectorAll(`[data-p]`).forEach(e => e.onclick = () => {
  cellType = parseInt(e.getAttribute("data-p"));
  picker.querySelector(`.selected`).classList.remove("selected");
  e.classList.add("selected");
  e.blur();
});

function onmouse(e) {
  let wasActive = active;
  active = e.buttons & 1;

  let ox = x;
  let oy = y;

  x = e.offsetX * scale;
  y = e.offsetY * scale;

  if(!wasActive || !active) return;
  
  let dx = x - ox;
  let dy = y - oy;
  let steps = Math.floor(Math.sqrt(dx*dx + dy*dy))

  for(let i = 1; i < steps; i++) {
    let x = ox + dx / steps * i;
    let y = oy + dy / steps * i;

    if(active) {
      sandbox.placeOnGrid(x  , y  , cellType);
      sandbox.placeOnGrid(x+1, y  , cellType);
      sandbox.placeOnGrid(x  , y+1, cellType);
      sandbox.placeOnGrid(x+1, y+1, cellType);
    }
  }
}

let ticks = 0;
function loop() {
  if(active) {
    sandbox.placeOnGrid(x  , y  , cellType);
    sandbox.placeOnGrid(x+1, y  , cellType);
    sandbox.placeOnGrid(x  , y+1, cellType);
    sandbox.placeOnGrid(x+1, y+1, cellType);
  }
  
  if(!paused) {
    ticks = (ticks + 1) % 256;
    gl.uniform1ui(ticksLoc, ticks);
    sandbox.updateGrid();
  }
}

const timeLoc = gl.getUniformLocation(program, "time");
let start = Date.now();

function draw() {
  gl.uniform1f(timeLoc, (Date.now() - start) / 1000);
  gl.texSubImage2D(gl.TEXTURE_2D, 0, 0, 0, size, size, gl.RGBA_INTEGER, gl.UNSIGNED_BYTE, gridData);
  gl.drawArrays(gl.TRIANGLES, 0, 6);
  requestAnimationFrame(draw);
}