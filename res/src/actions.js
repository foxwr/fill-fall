import * as sandbox from "./sandbox.js";

export let paused = false;

function pause() {
  paused = !paused;
}

function reset() {
  sandbox.grid.data.fill(0);
}

export default {
  "Space": pause,
  "Delete": reset
}