
include .config

all: clgl

clgl:
	clang \
		-Wall -Wextra -Werror \
		-fdiagnostics-color=always \
		-O0 -g3 \
		-I . \
		-I include \
		-I cllib/include \
		-I winlib/include \
		cllib/src/cllib.c \
		winlib/src/winlib.c \
		src/main.c src/panic.c \
		-I ../CLGLInterop/external_sources/glad/include \
		../CLGLInterop/external_sources/glad/src/glad.c \
		-lglfw3 -lOpenCL -lm

py:
	clang++ \
		-Wall -Wextra -Werror \
		-fdiagnostics-color=always \
		-O3 -std=c++2a \
		-fPIC -shared -o pathtracer.so \
		-I . \
		-I source \
		-I src \
		-D __clcpp__ \
		-D SCREEN_WIDTH=${SCREEN_WIDTH} \
		-D SCREEN_HEIGHT=${SCREEN_HEIGHT} \
		-D DEFINED_SCREEN_SIZE \
		src/test.cpp

cl:
	clang \
		-Wall -Wextra -Werror \
		-fdiagnostics-color=always \
		-O0 -g3 \
		-fPIC -shared -o pathtracer.so \
		-I . \
		-I include \
		-I cllib/include \
		-D SCREEN_WIDTH=${SCREEN_WIDTH} \
		-D SCREEN_HEIGHT=${SCREEN_HEIGHT} \
		-D DEFINED_SCREEN_SIZE \
		src/test.c \
		-L build \
		-lcl -lOpenCL


validate:
	clang++ \
		-Wall -Wextra -Werror \
		-fdiagnostics-color=always \
		-fno-inline -fno-omit-frame-pointer \
		-O0 -g3 -std=c++2a \
		-fPIC -shared -o pathtracer.so \
		-I . \
		-I source \
		-I src \
		-D __clcpp__ \
		-D SCREEN_WIDTH=${SCREEN_WIDTH} \
		-D SCREEN_HEIGHT=${SCREEN_HEIGHT} \
		-D DEFINED_SCREEN_SIZE \
		src/test.cpp src/validate.cpp

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

