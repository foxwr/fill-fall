all:
	clang -O3 -fno-builtin --target=wasm32 -nostdlib -Wl,--no-entry,--export-dynamic -fvisibility=hidden -fno-pic -o res/sandbox.wasm src/*.c