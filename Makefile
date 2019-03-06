# Makefile for the Legend of Saladir
# (C)1997/1998 Erno Tuomainen
# 

CC=g++
#LFLAGS=-o
CFLAGS=-std=c++11 -Wall -s -O6 -c
#CFLAGS=-Wall -g -c
CPPFLAGS=
RM=rm
TARGET=saladir
TARGET2=saladir_static
VERSION=034
RELNAME=saladir_$(VERSION)_`date '+%d%m%Y'`_i386
RELDIR=./release/$(RELNAME)

#DEBUGFLAGS=-DMSS
#DEBUGLIBS=-lmss

EXTRADEFINES=-DSALADIR_RELEASE -Dlinux
#EXTRADEFINES=-D_TIME_MEASUREMENT_
LIBS=-lncurses -lm

objects = mycurses.o generate.o output.o dice.o \
	birth.o doors.o init.o memory.o raiselev.o \
	utility.o caves.o dungeon.o inventor.o message.o \
	shops.o variable.o classes.o file.o items.o monster.o \
	skills.o skills_c.o creature.o magic.o tables.o \
	damage.o main.o player.o process.o time.o \
	talk.o options.o compress.o score.o weather.o \
	scrolls.o weapons.o search.o status.o quest.o \
	textview.o cmd.o traps.o outworld.o pathfind.o \
	imanip.o NPCtalk.o

all: build_s saladir

saladir: $(objects)
	$(CC) -o $(TARGET) $(objects) $(LIBS) $(DEBUGLIBS)

static: $(objects)
	$(CC) -static -o $(TARGET2) $(objects) $(LIBS) $(DEBUGLIBS)

zippy:
	$(RM) distro.zip
	zip distro.zip saladir saladir.txt Linux_readme.txt outworld.map \
	file_id.diz history.txt license.txt

linux_distr:
	mkdir $(RELDIR)
	mkdir $(RELDIR)/help
	cp saladir $(RELDIR)
	cp saladir_static $(RELDIR)
	cp outworld.map $(RELDIR)
	cp help/*.txt $(RELDIR)/help
	cp help/*.hlp $(RELDIR)/help
	cp help/*.diz $(RELDIR)/help
#	chown -R saladir.moongate *
	tar --directory=/saladir/release -czvf $(RELNAME).tar.gz ./$(RELNAME)
	echo "Archive $(RELNAME).tar.gz created"
#	$(RM) linux_distro.zip
#	zip linux_distro.zip saladir Linux_readme.txt \
#	outworld.map file_id.diz saladir_static \
#f	help/*.txt help/*.hlp

cleanall:
	$(RM) $(objects)

tmpclean:
	$(RM) $(HOME)/saladir.is.running
	$(RM) $(HOME)/.saladirtmp/*.tmp
	$(RM) $(HOME)/.saladirtmp/core


rights:
	chown saladir.saladir saladir
	chmod 4755 saladir

sbackup:
	zip backup/linux_src_`date '+%d%m%Y'`.zip *.cc *.h docs/* \
	*.txt *.TXT Makefile *.map *.MAP *.txt utils/* pathfinder/* \
	help/*

build_s:
	./utils/buildver b

version_s:
	./utils/buildver m

include $(objects:.o=.d)

$(objects): %.o: %.cc
	$(CC) $(DEBUGFLAGS) $(EXTRADEFINES) $(CFLAGS) $(CPPFLAGS) $< -o $@

%.d: %.cc
	$(SHELL) -ec '$(CC) -MM $(CPPFLAGS) $< \
    	| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
      	[ -s $@ ] || rm -f $@'


