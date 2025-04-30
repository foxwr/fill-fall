import * as sandbox from "./src/sandbox.js";
import tools from "./src/tools.js";
import actions, { paused } from "./src/actions.js";

sandbox.init(256, 256);

const screen = document.getElementById("screen");
const gl = screen.getContext("webgl2", { antialias: false });

const program = await createProgram("res/shader/grid");

let offx = 0;
let offy = 0;

const transformLoc = gl.getUniformLocation(program, "transform");
let scale = 1;
(onresize = resize)();

function resize() {
  screen.width = screen.clientWidth;
  screen.height = screen.clientHeight;
  scale = Math.floor(Math.min(
    screen.width / sandbox.grid.width,
    screen.height / sandbox.grid.height,
  ) - 0.1);

  gl.viewport(0, 0, screen.width, screen.height);
  gl.uniformMatrix3x2fv(transformLoc, false, [
    scale * sandbox.grid.width / screen.width, 0,
    0, scale * sandbox.grid.height / screen.height,
    offx, -offy
  ]);
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

const paletteImg = new Image();
paletteImg.src = "res/palette.png";
await new Promise(r => paletteImg.onload = r)

createTex("palette", 0, gl.RGBA, gl.RGBA, paletteImg.width, paletteImg.height, paletteImg);
createTex("grid", 1, gl.RG8UI, gl.RG_INTEGER, sandbox.grid.width, sandbox.grid.height);

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

let active, x, y;
screen.addEventListener("pointermove", onmouse, { passive: true });
screen.addEventListener("pointerdown", onmouse, { passive: true });
screen.addEventListener("pointerup", onmouse, { passive: true });
screen.addEventListener("pointerout", onmouse, { passive: true });

const pickerForm = document.forms.picker;
let cellType = parseInt(pickerForm.kind.value);
pickerForm.oninput = () => cellType = parseInt(pickerForm.kind.value);

const toolsForm = document.forms.tools;
let tool = toolsForm.tool.value;
toolsForm.oninput = () => tool = toolsForm.tool.value;

const actionsForm = document.forms.actions;
actionsForm.onclick = e => actions[e.target.getAttribute("data-action")]?.();

let oldx = NaN, oldy = NaN;
function onmouse(e) {
  if(e.buttons & 4) {
    if(isNaN(oldx) || isNaN(oldy)) {
      oldx = e.offsetX;
      oldy = e.offsetY;
    }

    let deltax = e.offsetX - oldx;
    let deltay = e.offsetY - oldy;
    
    offx = Math.min(Math.max(offx + 2 * deltax / screen.width, -1), 1);
    offy = Math.min(Math.max(offy + 2 * deltay / screen.height, -1), 1);
    onresize();

    oldx = e.offsetX;
    oldy = e.offsetY;

    return;
  } else {
    oldx = NaN;
    oldy = NaN;
  }

  let wasActive = active;
  active = e.buttons & 1;

  if(!wasActive && active)
    tools[tool].fn(x, y, cellType);

  let ox = x;
  let oy = y;

  x = (e.offsetX - screen.width * (offx + 1) / 2) / scale + sandbox.grid.width / 2;
  y = (e.offsetY - screen.height * (offy + 1) / 2) / scale + sandbox.grid.height / 2;

  if(!wasActive || !active) return;
  
  let dx = x - ox;
  let dy = y - oy;
  let steps = Math.floor(Math.sqrt(dx*dx + dy*dy))

  for(let i = 1; i < steps; i++) {
    let x = ox + dx / steps * i;
    let y = oy + dy / steps * i;

    if(active && tools[tool].move)
      tools[tool].fn(x, y, cellType);
  }
}

let ticks = 0;
function loop() {
  if(active && tools[tool].loop)
    tools[tool].fn(x, y, cellType);
  
  if(!paused) {
    ticks = (ticks + 0.125) % 16;
    gl.uniform1ui(ticksLoc, ticks);
    sandbox.api.update();
  }
}

function draw() {
  gl.texSubImage2D(gl.TEXTURE_2D, 0, 0, 0, sandbox.grid.width, sandbox.grid.height, gl.RG_INTEGER, gl.UNSIGNED_BYTE, sandbox.grid.data);
  gl.drawArrays(gl.TRIANGLES, 0, 6);
  requestAnimationFrame(draw);
}

window.oncontextmenu = e => e.preventDefault();
window.onkeydown = e => {
  if(actions[e.code]) {
    e.preventDefault();
    actions[e.code]();
  } else if(tools[e.code]) {
    e.preventDefault();
    toolsForm.tool.value = e.code;
    tool = e.code;
  }
}