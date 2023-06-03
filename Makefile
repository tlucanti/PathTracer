
all: cl

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

cl:
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
