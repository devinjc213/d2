CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lSDL2 -lSDL2_image

TARGET = doze 
SRCS = main.c ./shared/logger.c ./shared/sprite.c player.c enemy.c game.c input.c
BIN_DIR = bin

$(TARGET): $(SRCS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(SRCS) -o $(BIN_DIR)/$(TARGET) $(LDFLAGS)

clean:
	rm -f $(BIN_DIR)/$(TARGET)
