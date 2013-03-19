# Makefile for the Legend of Saladir
# (C)1997/1998 Erno Tuomainen
# 
CC=gcc
#LFLAGS=-o
CFLAGS=-Wall -s -O3 -c
LIBS=-lncurses -lm

all: pathfind

pathfind: pfind.o util.o output.o list.o
	$(CC) -o pathfind pfind.o util.o output.o list.o $(LIBS) $(DEBUGLIBS)

#view_static: view.o sysdep.o textview.o
#	$(CC) -static -o view sysdep.o view.o textview.o $(LIBS) $(DEBUGLIBS)

list.o: list.c
	$(CC) $(CFLAGS) $(CPPFLAGS) list.c -o list.o

output.o: output.c 
	$(CC) $(CFLAGS) $(CPPFLAGS) output.c -o output.o

util.o: util.c
	$(CC) $(CFLAGS) $(CPPFLAGS) util.c -o util.o

pfind.o: pfind.c 
	$(CC) $(CFLAGS) $(CPPFLAGS) pfind.c -o pfind.o

