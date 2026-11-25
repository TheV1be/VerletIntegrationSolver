.PHONY: all compile run clean

all: run

build/compile_commands.json: CMakeLists.txt
	cmake -B build
	ln -sf build/compile_commands.json .

compile: build/compile_commands.json
	cmake --build build -j

run: compile
	./build/sim

clean:
	rm -rf build compile_commands.json

