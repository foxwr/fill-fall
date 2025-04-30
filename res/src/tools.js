import * as sandbox from "./sandbox.js"

function brush(x, y, kind) {
  sandbox.api.placeAt(x, y, 1, kind, 0);
}

function eraser(x, y) {
  sandbox.api.placeAt(x, y, 2, 0, 0);
}

function cloner(x, y, kind) {
  sandbox.api.placeAt(x, y, 0, 52, kind);
}

function fill(x, y, kind) {
  sandbox.api.fill(x, y, kind);
}

export default {
  "KeyB": brush,
  "KeyE": eraser,
  "KeyC": cloner,
  "KeyG": fill
}