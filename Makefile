uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(uname_S),Darwin)
CC=gcc
CFLAGS=-Wall -Werror -g -std=c99
LIBS=-framework OpenGL -framework GLUT
endif

ifeq ($(uname_S),Linux)
CC=clang
CFLAGS=-Wall -Werror -g
LIBS=-lGL -lglut
endif

SOURCES=gltetris.c
EXECUTABLE=gltetris

gltetris: $(SOURCES)
	$(CC) $(CFLAGS) $(LIBS) $(SOURCES) -o $(EXECUTABLE)

clean:
	rm $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

