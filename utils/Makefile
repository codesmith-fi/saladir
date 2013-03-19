# Makefile for the Legend of Saladir
# (C)1997/1998 Erno Tuomainen
# 
CC=egcs
#LFLAGS=-o
CFLAGS=-Wall -s -O3 -c
LIBS=-lncurses

all: keys view convert

convert: convert.o
	$(CC) -o convert convert.o $(DEBUGLIBS)

buildver: buildver.o
	$(CC) -o buildver buildver.o $(DEBUGLIBS)

view: view.o sysdep.o textview.o
	$(CC) -o view sysdep.o view.o textview.o $(LIBS) $(DEBUGLIBS)

keys: keys.o 
	$(CC) -o keys keys.o $(LIBS) $(DEBUGLIBS)

view_static: view.o sysdep.o textview.o
	$(CC) -static -o view sysdep.o view.o textview.o $(LIBS) $(DEBUGLIBS)

keys.o: keys.c
	$(CC) $(CFLAGS) $(CPPFLAGS) keys.c -o keys.o

view.o: view.cc 
	$(CC) $(CFLAGS) $(CPPFLAGS) view.cc -o view.o

convert.o: convert.c
	$(CC) $(CFLAGS) $(CPPFLAGS) convert.c -o convert.o

textview.o: textview.cc 
	$(CC) $(CFLAGS) $(CPPFLAGS) textview.cc -o textview.o

sysdep.o: sysdep.cc 
	$(CC) $(CFLAGS) $(CPPFLAGS) sysdep.cc -o sysdep.o

buildver.o: buildver.cc
	$(CC) $(CFLAGS) $(CPPFLAGS) buildver.cc -o buildver.o

