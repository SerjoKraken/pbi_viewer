CC=gcc

LIB=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11

#math library needed
pbi:
	$(CC) $(LIB) src/pbi.c src/main.c -o pbi


pbifp:
	$(CC) $(LIB) src/pbifp.c src/main.c -o pbifp

