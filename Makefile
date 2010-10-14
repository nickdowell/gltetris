CC=clang
CFLAGS=-Wall -Werror -g
LIBS=-lGL -lglut
SOURCES=gltetris.c
EXECUTABLE=gltetris

gltetris: $(SOURCES)
	$(CC) $(CFLAGS) $(LIBS) $(SOURCES) -o $(EXECUTABLE)

clean:
	rm $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

