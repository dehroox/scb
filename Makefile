MAKEFLAGS += --no-print-directory -s

# Compiler and flags
CC := clang
BASE_CFLAGS := -std=c23 -Wpedantic -fno-common \
               -fno-plt -fno-semantic-interposition -fstrict-enums \
               -fstrict-return -fvisibility=hidden -fstack-protector-strong \
               -ftrivial-auto-var-init=pattern -fstrict-flex-arrays=3 \
               -Wall -Wextra -Wdouble-promotion -Wformat=2 -Wnull-dereference \
               -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations \
               -Wshadow -Wundef -Wfloat-equal -Wcast-align -Wpointer-arith \
               -Wwrite-strings -Wunused-parameter -Wpacked \
               -Wpadded -Wredundant-decls -Wcast-qual \
               -Wconversion -Wswitch-default -Wswitch-enum

# Directories
SRCDIR := src
INCDIR := include
OBJDIR := obj
BINDIR := bin

# Files
SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEPENDS := $(OBJECTS:.o=.d)

# Default build type
BUILD_TYPE ?= dev

# Conditionally define directories and flags based on BUILD_TYPE
ifeq ($(BUILD_TYPE),debug)
	TARGET := $(BINDIR)/main-debug.exe
	CFLAGS := $(BASE_CFLAGS) -DDEBUG -O0 -g
else ifeq ($(BUILD_TYPE),release)
	TARGET := $(BINDIR)/main-release.exe
	CFLAGS := $(BASE_CFLAGS) -DNDEBUG -O3 -flto -pipe
else
	TARGET := $(BINDIR)/main-dev.exe
	CFLAGS := $(BASE_CFLAGS) -O2
endif

CPPFLAGS += -I$(INCDIR)
LDFLAGS +=

# Default target
.PHONY: all clean build debug release install help distclean print-vars run

all: build

# Create directories if they don't exist
$(OBJDIR) $(BINDIR):
	@mkdir -p $@

# Include auto-generated dependencies
-include $(DEPENDS)

# Compile source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@

# Link executable from object files
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# Generate compile_commands.json using bear (if available), otherwise fallback
compile_commands.json: $(SOURCES)
	bear --output $@ -- $(MAKE) $(TARGET); 

# Build with compile_commands.json
build: compile_commands.json $(TARGET)

# Clean build files
clean:
	@rm -rf $(OBJDIR) $(BINDIR) compile_commands.json

# Very clean - remove all generated files
distclean: clean
	@rm -f cscope.*

# Run the built executable
run: $(TARGET)
	@./$(TARGET)

# Install binary (example)
install: $(TARGET)
	@echo "Installing $(TARGET) to /usr/local/bin/"
	@cp $(TARGET) /usr/local/bin/

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Build default (dev)"
	@echo "  build     - Build with compile_commands.json"
	@echo "  debug     - Build with debug flags"
	@echo "  release   - Build with optimization flags"
	@echo "  clean     - Remove build files"
	@echo "  distclean - Remove all generated files"
	@echo "  run       - Run the built executable"
	@echo "  install   - Install binary to system"
	@echo "  help      - Show this help"

# Print variables for debugging
print-vars:
	@echo "BUILD_TYPE = $(BUILD_TYPE)"
	@echo "SOURCES = $(SOURCES)"
	@echo "OBJECTS = $(OBJECTS)"
	@echo "CFLAGS = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "CPPFLAGS = $(CPPFLAGS)"

# Specific build targets
debug:
	@$(MAKE) BUILD_TYPE=debug

release:
	@$(MAKE) BUILD_TYPE=release

.DEFAULT_GOAL := build