
include .config

all: py

py:
	clang++ \
		-Wall -Wextra -Werror \
		-fdiagnostics-color=always \
		-O0 -g3 -std=c++2a \
		-fPIC -shared -o pathtracer.so \
		-I . \
		-I source \
		-I src \
		-D __clcpp__ \
		-D SCREEN_WIDTH=${SCREEN_WIDTH} \
		-D SCREEN_HEIGHT=${SCREEN_HEIGHT} \
		-D DEFINED_SCREEN_SIZE \
		src/test.cpp

cpp:
	clang++ \
		-Wall -Wextra -Werror \
		-fdiagnostics-color=always \
		-O0 -g3 -std=c++2a \
		-I . \
		-I source \
		-I src \
		-D __clcpp__ \
		src/test.cpp

libcl:
	mkdir -p build
	clang \
		-c -Wall -Wextra \
		-fdiagnostics-color=always \
		-O3 -fomit-frame-pointer \
		-I include \
		-I cllib/include \
		cllib/src/cllib.c src/panic.c
	ar rcs build/libcl.a cllib.o panic.o
	rm -f cllib.o panic.o

clgl:
	clang \
		-Wall -Wextra \
		-fdiagnostics-color=always \
		-O0 -g3 -fno-omit-frame-pointer -fno-inline \
		-I include \
		-I cllib/include \
		-I gllib/include \
		-I ../glfw/include \
		-D CONFIG_DEVICE_TYPE_GPU \
		-D CONFIG_PRINT_PROGRAM_LOG \
		-L ../glfw/build/src \
		cllib/src/cllib.c src/test.c src/panic.c \
		gllib/src/gllib.c \
		-lOpenCL -lOpenGL -lglfw3 -lm -lGLX

gl:
	clang \
		-Wall -Wextra \
		-fdiagnostics-color=always \
		-O0 -g3 -fno-omit-frame-pointer -fno-inline \
		-I ../glfw/include \
		gl.c \
		-L../glfw/build/src \
		-lGL -lGLU -lglut -lglfw3 -lm -ldl -lpthread
