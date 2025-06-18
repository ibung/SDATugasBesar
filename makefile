# Makefile untuk mengompilasi program C
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -Iinclude -finput-charset=UTF-8
TARGET = runprogram

SOURCES := $(wildcard src/*.c)
OBJECTS := $(SOURCES:src/%.c=build/%.o)

all: $(TARGET)

# Membuat target utama
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Membuat folder build jika belum ada, lalu compile .c ke build/.o
build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build $(TARGET)

.PHONY: all clean run build