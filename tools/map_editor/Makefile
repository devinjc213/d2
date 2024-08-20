CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lSDL2 -lSDL2_image

TARGET = dozy_tiler 
SRCS = main.c tilemap.c panel.c ../../shared/logger.c ../../shared/sprite.c ../../shared/tilesheet.c
ASSETS_DIR = ../../assets/Tilesets
BIN_DIR = bin

$(TARGET): $(SRCS) copy_assets
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(SRCS) -o $(BIN_DIR)/$(TARGET) $(LDFLAGS)

copy_assets:
	@mkdir -p $(BIN_DIR)/assets/tilesets
	@cp -R $(ASSETS_DIR)/* $(BIN_DIR)/assets/tilesets

clean:
	rm -f $(BIN_DIR)/$(TARGET)
