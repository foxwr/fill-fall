all:
	clang -std=c99 -Wall -Werror -O3 -s -fno-builtin --target=wasm32 -nostdlib -Wl,--no-entry,--export-dynamic -fvisibility=hidden -o res/sandbox.wasm src/*.c