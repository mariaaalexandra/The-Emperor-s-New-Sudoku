CC = gcc
CFLAGS = -Wall
SRC = ./src/sudoku.c ./src/cJSON.c

build:
	$(CC) $(CFLAGS) $(SRC) -o ./sudoku

run:
	./sudoku

.PHONY:clean
clean: 
	rm -f ./sudoku
