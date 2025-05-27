# Makefile untuk Academic Paper Sorter (GCC version)
# Compiler dan flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = 

# Target executable
TARGET = academic_paper_sorter

# Daftar semua file source yang ada (sesuaikan dengan file yang benar-benar ada)
SOURCES = Dzakir_BST.c \
          Fatim_DLLPapers.c \
          Ibnu_SLL.c \
          Rizky_DLLjsonParser.c \
          Abi_MainProgramStack.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Membuat executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete! Executable: $(TARGET)"

# Compile individual source files
Dzakir_BST.o: Dzakir_BST.c
	$(CC) $(CFLAGS) -c Dzakir_BST.c -o Dzakir_BST.o

Fatim_DLLPapers.o: Fatim_DLLPapers.c
	$(CC) $(CFLAGS) -c Fatim_DLLPapers.c -o Fatim_DLLPapers.o

Ibnu_SLL.o: Ibnu_SLL.c
	$(CC) $(CFLAGS) -c Ibnu_SLL.c -o Ibnu_SLL.o

Rizky_DLLjsonParser.o: Rizky_DLLjsonParser.c
	$(CC) $(CFLAGS) -c Rizky_DLLjsonParser.c -o Rizky_DLLjsonParser.o

Abi_MainProgramStack.o: Abi_MainProgramStack.c
	$(CC) $(CFLAGS) -c Abi_MainProgramStack.c -o Abi_MainProgramStack.o

# Target untuk menjalankan program
run: $(TARGET)
	./$(TARGET)

# Membersihkan file hasil compile
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete!"

# Compile dengan debug info
debug: CFLAGS += -DDEBUG -O0
debug: $(TARGET)

# Compile untuk release
release: CFLAGS += -O2 -DNDEBUG
release: $(TARGET)

# Quick compile (semua file sekaligus)
quick:
	$(CC) $(CFLAGS) *.c -o $(TARGET)
	@echo "Quick build complete!"

# Help
help:
	@echo "Available targets:"
	@echo "  all      - Build the project (default)"
	@echo "  run      - Build and run the program"
	@echo "  clean    - Remove compiled files"
	@echo "  debug    - Build with debug flags"
	@echo "  release  - Build with optimization"
	@echo "  quick    - Quick compile all .c files"
	@echo "  help     - Show this help message"

# Phony targets
.PHONY: all clean run debug release quick help