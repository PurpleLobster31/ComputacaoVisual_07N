# GNU Make: macOS, Linux/WSL e terminal MSYS2 UCRT64 no Windows.
UNAME_S := $(shell uname -s)
SRC_DIR := src
BUILD_DIR := build
EXE_EXT :=
PLATFORM_LIBS :=
ifneq (,$(filter MINGW% MSYS% UCRT%,$(UNAME_S)))
    EXE_EXT := .exe
    PLATFORM_LIBS := -mconsole
endif
TARGET := $(BUILD_DIR)/programa$(EXE_EXT)

# Respeita CC definido pelo usuario. No Mac, evita o gcc da Apple (Clang).
ifeq ($(origin CC),default)
    CC := gcc
    ifeq ($(UNAME_S),Darwin)
        GCC_PREFIX := $(shell brew --prefix gcc 2>/dev/null)
        CC := $(firstword $(wildcard $(GCC_PREFIX)/bin/gcc-[0-9]*))
    endif
endif

PKG_CONFIG ?= pkg-config
PACKAGES := sdl3 sdl3-image sdl3-ttf
PKG_CFLAGS = $(shell $(PKG_CONFIG) --cflags $(PACKAGES))
PKG_LIBS = $(shell $(PKG_CONFIG) --libs $(PACKAGES))
CFLAGS ?= -std=c99 -Wall -Wextra

SRCS := $(sort $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/image/*.c $(SRC_DIR)/ui/*.c))
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all check-deps clean run info assets
all: $(TARGET) assets

check-deps:
	@test -n "$(CC)" && command -v "$(CC)" >/dev/null 2>&1 || { echo "Erro: compilador ausente. Consulte o README (no Mac: brew install gcc)."; exit 1; }
	@command -v "$(PKG_CONFIG)" >/dev/null 2>&1 || { echo "Erro: instale pkg-config/pkgconf conforme o README."; exit 1; }
	@$(PKG_CONFIG) --print-errors --exists 'sdl3 >= 3.2.0' 'sdl3-image >= 3.2.0' 'sdl3-ttf >= 3.2.0'

$(TARGET): $(OBJS) | check-deps
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(PKG_LIBS) $(LDLIBS) $(PLATFORM_LIBS) -o "$@"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c Makefile | check-deps
	@mkdir -p "$(dir $@)"
	$(CC) $(CPPFLAGS) $(CFLAGS) $(PKG_CFLAGS) -MMD -MP -c "$<" -o "$@"

FONT := assets/fonts/PixelifySans-VariableFont_wght.ttf
assets: $(BUILD_DIR)/$(FONT)

$(BUILD_DIR)/$(FONT): $(FONT)
	@mkdir -p "$(dir $@)"
	cp "$<" "$@"

run: all
	./$(TARGET) $(ARGS)

info: check-deps
	@uname -sm
	@$(CC) --version
	@$(PKG_CONFIG) --version
	@$(PKG_CONFIG) --modversion $(PACKAGES)

clean:
	rm -rf "$(BUILD_DIR)"

-include $(DEPS)

TEST_BINS := $(addprefix $(BUILD_DIR)/,$(addsuffix $(EXE_EXT),test_histogram test_ui test_equalization test_resolution test_save))
.PHONY: test test-build
test-build: $(TEST_BINS) assets

test: test-build
	@mkdir -p "$(BUILD_DIR)/save-test"
	cd "$(BUILD_DIR)/save-test" && ../test_save$(EXE_EXT)
	./$(BUILD_DIR)/test_resolution$(EXE_EXT)
	./$(BUILD_DIR)/test_histogram$(EXE_EXT)
	./$(BUILD_DIR)/test_ui$(EXE_EXT) kodim23.png $(BUILD_DIR)/ui-preview.png
	./$(BUILD_DIR)/test_equalization$(EXE_EXT) kodim23.png $(BUILD_DIR)/equalized-preview.png

$(BUILD_DIR)/test_histogram$(EXE_EXT): tests/test_histogram.c src/image/histogram.c src/image/histogram.h Makefile | check-deps
	@mkdir -p "$(BUILD_DIR)"
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc $(PKG_CFLAGS) tests/test_histogram.c src/image/histogram.c $(LDFLAGS) $(PKG_LIBS) $(LDLIBS) $(PLATFORM_LIBS) -o "$@"

UI_TEST_SRCS := tests/test_ui.c src/ui/histogram_view.c src/ui/text_render.c src/ui/button.c src/image/histogram.c src/image/image_loader.c src/image/grayscale.c
$(BUILD_DIR)/test_ui$(EXE_EXT): $(UI_TEST_SRCS) $(wildcard src/ui/*.h src/image/*.h) Makefile | check-deps
	@mkdir -p "$(BUILD_DIR)"
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc $(PKG_CFLAGS) $(UI_TEST_SRCS) $(LDFLAGS) $(PKG_LIBS) $(LDLIBS) $(PLATFORM_LIBS) -o "$@"

EQUALIZATION_TEST_SRCS := tests/test_equalization.c $(filter-out src/main.c,$(SRCS))
$(BUILD_DIR)/test_equalization$(EXE_EXT): $(EQUALIZATION_TEST_SRCS) src/main.c $(wildcard src/ui/*.h src/image/*.h) Makefile | check-deps
	@mkdir -p "$(BUILD_DIR)"
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc $(PKG_CFLAGS) $(EQUALIZATION_TEST_SRCS) $(LDFLAGS) $(PKG_LIBS) $(LDLIBS) $(PLATFORM_LIBS) -o "$@"

RESOLUTION_TEST_SRCS := tests/test_resolution.c $(filter-out src/main.c,$(SRCS))
$(BUILD_DIR)/test_resolution$(EXE_EXT): $(RESOLUTION_TEST_SRCS) src/main.c $(wildcard src/ui/*.h src/image/*.h) Makefile | check-deps
	@mkdir -p "$(BUILD_DIR)"
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc $(PKG_CFLAGS) $(RESOLUTION_TEST_SRCS) $(LDFLAGS) $(PKG_LIBS) $(LDLIBS) $(PLATFORM_LIBS) -o "$@"

.PHONY: test-window
test-window: $(BUILD_DIR)/test_resolution$(EXE_EXT) assets
	./$(BUILD_DIR)/test_resolution$(EXE_EXT) kodim23.png

SAVE_TEST_SRCS := tests/test_save.c $(filter-out src/main.c,$(SRCS))
$(BUILD_DIR)/test_save$(EXE_EXT): $(SAVE_TEST_SRCS) src/main.c $(wildcard src/ui/*.h src/image/*.h) Makefile | check-deps
	@mkdir -p "$(BUILD_DIR)"
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc $(PKG_CFLAGS) $(SAVE_TEST_SRCS) $(LDFLAGS) $(PKG_LIBS) $(LDLIBS) $(PLATFORM_LIBS) -o "$@"
