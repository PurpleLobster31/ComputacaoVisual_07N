CC := gcc
CSTD := -std=c99
WARN := -Wall -Wextra
SRC_DIR := src
BUILD_DIR := build
TARGET := $(BUILD_DIR)/programa

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

UNAME_S := $(shell uname -s)

# --- Linux / WSL: usa pkg-config, que já resolve todos os includes/libs do sistema ---
ifeq ($(UNAME_S),Linux)
    PKG_CFLAGS := $(shell pkg-config --cflags sdl3 sdl3-image sdl3-ttf)
    PKG_LIBS   := $(shell pkg-config --libs sdl3 sdl3-image sdl3-ttf)
endif

# --- macOS: Homebrew não deixa as libs em include/lib padrão do sistema ---
ifeq ($(UNAME_S),Darwin)
    BREW_PREFIX := $(shell brew --prefix)
    PKG_CFLAGS := -I$(BREW_PREFIX)/include
    PKG_LIBS   := -L$(BREW_PREFIX)/lib -lSDL3 -lSDL3_image -lSDL3_ttf
endif

CFLAGS := $(CSTD) $(WARN) $(PKG_CFLAGS)
LDFLAGS := $(PKG_LIBS)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET) $(ARGS)

clean:
	rm -rf $(BUILD_DIR)