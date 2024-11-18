CC=gcc

LIB=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -std=c2x -g

pbi: src/pbi.c src/main.c
	$(CC) $(LIB) src/pbi.c src/main.c -o src/pbi

pbifp:
	$(CC) $(LIB) src/pbifp.c src/main.c -o src/pbifp

all: src/pbi
