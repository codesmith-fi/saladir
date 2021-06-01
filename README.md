Legend of Saladir
Update 1st of June 2021 by Erno Pakarinen

Just a quick update. I added binaries for Windows DOS from the old 0.34 version (1999 build). Check the README file under Binaries folder. 

This is a textmode roguelike RPG game which is intended to run in console/terminal or DOS box.

Project died in 1999 and has not been continued since. Code is ugly and not reusable nor extendable. Don't take this as an example for good programming/designing practises :-)

It has poor design and no tests but the game engine itself has advanced features. Game does not have very much content but is completely playable.

Quickstart
==========

When game has been started and character has been created press ? to read the instructions. Or check out the file "manual.txt" in help/ folder in sources. The game does not hold your hand much, so for example when you enter a dungeon you will need to equip a light source since dungeons are dark mostly. Press 'q' and equip your lantern in the tool slot. 


Building
========

Windows build is borked atm and will most probably not build with modern compilers. But in Linux (atleast Ubuntu) it should build by just executing "make" in the main source directory. You need GCC and ncurses library. Before compiling "saladir" itself, you need to compile certain utils under utils/ directory

	cd utils
	make

On Ubuntu install by typing:

	sudo apt-get install libncurses5-dev libncursesw5-dev
	sudo apt-get install gcc

To build:

	make
	./saladir

Windows build has not been tested for 10 years!

- Erno Pakarinen (erpakari@gmail.com)

Contact
=======

You can reach me by email @ codesmith.fi@gmail.com

I also lurk in Discord with a id of "xanaki#9472" if you want to contact and talk with me. You can also find me from several coding channels like One Lone Coder and GameFromScratch for example.

My own discord channel is "Knights who say RPG" @ https://discord.gg/P4Uu3GU7tx

My blog I just started is @ https://codesmith.hashnode.dev/

