CC = gcc
CFLAGS = -Isrc/include/SDL2 -Isrc/include/rEngine -Isrc/include/rEngine/core
LDFLAGS = -Lsrc/lib -lSDL2main -lSDL2  
TARGET = main

SRC = $(wildcard src/*.c) $(wildcard src/include/rEngine/*.c) $(wildcard src/include/rEngine/core/*.c)

all: $(TARGET) postbuild

$(TARGET):
	$(CC) $(SRC) $(CFLAGS) -o $(TARGET) $(LDFLAGS)

postbuild:
	copy src\lib\SDL2.dll .\ 2> NUL

clean:
	del /Q $(TARGET).exe 2> NUL