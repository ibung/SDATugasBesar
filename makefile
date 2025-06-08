CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -Iinclude
TARGET = runprogram

SOURCES := $(wildcard src/*.c)
OBJECTS := $(SOURCES:src/%.c=src/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all clean run