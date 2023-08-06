CC = gcc
CFLAGS = -Wpedantic -std=gnu99

myShell : myShell.c shellQ.c shellQ.h
	$(CC) $(CFLAGS) myShell.c shellQ.c -o myShell

