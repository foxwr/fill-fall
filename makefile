all:
	clang -flto -O3 -s -fno-builtin --target=wasm32 -nostdlib -Wl,--no-entry,--export-dynamic -fvisibility=hidden -fno-pic -o res/sandbox.wasm src/*.c