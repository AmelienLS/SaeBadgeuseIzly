CC = avr-g++
CFLAGS = -g -Os -w -mmcu=atmega4809
SRC = src/main.cpp src/board_init.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = main.elf

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)