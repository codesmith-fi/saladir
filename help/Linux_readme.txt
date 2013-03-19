
***************************************************************************
How to set up the Legend of Saladir for Linux			19.04.1998
***************************************************************************


Please read this document to the end, it might prevent some problems.

*********************
1. Linux version info
*********************

Linux version of the game has been developed using kernel V2.0.33, GCC
2.8.1-1, GNU libc 2.0.7-5 (aka libc6) and ncurses 4.2(b980307-1). 

Dynamic version requires atleast these libraries (using ldd):

	libncurses.so.4 => /lib/libncurses.so.4 (0x40003000)
        libm.so.6 => /lib/libm.so.6 (0x40046000)
        libc.so.6 => /lib/libc.so.6 (0x4005f000)
        /lib/ld-linux.so.2 => /lib/ld-linux.so.2 (0x00000000)

Static version shouldn't have any extra requirements than the executable
itself.

Linux version has been tested with several linux systems. There're known
problems in some Linux system, but in some other it has worked. One problem
was with curses output, some Linux systems can get "oddly colored" output,
blinking characters or even bigger areas etc. The reason for this is yet
unknown.


In the Linux systems, Saladir stores all the needed temporary files in the
home directory of the current user who is playing the game. The temporary
directory will be called "~/.saladirtmp/". Do not delete it while playing
the game.

The home directory will also contain a special file "saladir.is.running"
which indicates that the game is running. When Saladir is started, it will
check for the existence of this file and won't start again if the file is
found.

***************
2. Installation
***************


There is this one thing to consider when setting up Saladir for Linux. 

Do you want to have a global score file which records scores from everyone
who's playing the game in your Linux system (requires root privileges to set
it up)? See case B below!

  or

You just want to play the game yourself (and you don't have root privileges
to the system)? See case A below!

There're two ways to set the game up. 

A) Just extract the game archive to your own home directory
   and run it from there.

   Create a directory for the game, ie "~/saladir and copy all
   files from the archive to this directory.

   You should be able to play the game. Please note that the highscore
   file will be stored in your home directory and it only records your
   scores.

B) Use a global data directory for game files, and store the executable 
   in a directory accessable by all users of your system. When saladir
   is set up this way, it will share the highscore file between every
   user who has been using it. (it will be stored in /etc/saladir).

   All the needed data files will be stored in "/etc/saladir/".
   The main executable should be stored ie. in "/usr/local/bin/",
   somewhere in your path. Must be accessable by all users who want
   to play the game.

   This process requires root access from you and a bit of work. (If 
   you've set up ADOM for your system the procedure is quite similar).

   The procedure requires you to set up a new user and group accounts
   which will be used only by the game while it's running. 

   Step by step instructions for setting a global data directory:

   1) log in as root
   2) create a new user and group "saladir". You can use any
      user/group name you want, but reserve that user/group only for
      the game saladir.

      example commands:

      [root@moongate /]# groupadd saladir
           - Creates a new group called saladir

      [root@moongate /]# useradd -g saladir saladir
           - creates a new user "saladir" with initial group 
	     set to "saladir"

   2) copy the main executable to ie. "/usr/local/bin/" or any
      other directory which is in the path and to where all users
      have a read access 

      [root@moongate /]# cp saladir /usr/local/bin

   3) create a directory "/etc/saladir/", and copy all data and document
      files to this directory.

      [root@moongate /]# mkdir /etc/saladir
      [root@moongate /]# cp outworld.map /etc/saladir
      [root@moongate /]# cp *.txt /etc/saladir

   4) change the owner of the directory and all files in it for
      the group "saladir" and the user "saladir".

      [root@moongate /]# chown -R saladir.saladir /etc/saladir/

   5) set read and write access only 
      for the owner (user,group) which is "saladir". Don't give any
      rights for others.
   
      The directory needs +rwx

      [root@moongate /]# chmod 770 /etc/saladir

      All files in it need +rw

      [root@moongate /]# chmod 660 /etc/saladir/*

   6) Final step requires you to set the SUID bit in the main executable
      and the owners again to "saladir"

      [root@moongate /]# chmod 4755 /usr/local/bin/saladir
      [root@moongate /]# chown saladir.saladir /usr/local/bin/saladir

   A bit confuzed, are you? I hope to add an install script which will do
   all this for you. Later.


What does all this mess actually mean? (The technical stuff)
--------------------------------------

The main goal with this procedure is to "protect" game specific files
ie. HISCORE files from manual user tampering. If you're going to have a
global highscore file for the game, then it probably should be protected
somehow so that not every user can go and manually change their scores.

In Unix/Linux systems, every file and process has an owner. When a new
process is started, it inherits the owner status from the user who started
the process. UNLESS the file has SUID bit set.

When a SUID-bit is set, the process inherits the owner status from the file
being executed. Also, all processes can CHANGE their persona (owner) while
running. Bute they can only change the owner between the USER who has
executed the file and the owner which has been stored in the executed file.

Let's take an example, you have this kind of executable file:

   -rwsr-xr-x   1 saladir  saladir    557622 Feb 12 20:12 saladir

The owner user is "saladir" and the owner group is a group called
"saladir". In the status bits you can see 's', this is the SUID (Set User
ID) bit.

Let's say that a user "aragorn" has logged in to your Linux box, and after a
while he executes this game. Normally (if no SUID is set) the new process
has an owner "aragorn" but in this case SUID is set and the process inherits
the owner "saladir".

Simple, but what can we do with this?

Now, when the game needs to save a "highscore"-file it uses this feature and
changes its persona to "saladir".  Because the process is owned by
"saladir", the highscore file stored in the disk will also be owned by
"saladir". So, the only way the user "aragorn" can access (write) to this
file is by gaining a great high score inside the game which will then be
saved to the score file by the game itself.

I bet you're now even more confuzed? 

Of course it's a security risk to give process a way to change it's
persona. That's why we have set up a special user/group account for the
game. This user does only have access to the GAME specific files and nothing
more. 

Important note:
---------------

If the directory "/etc/saladir" exists, then the data files must also be
held there. So if the game has access to this directory, it will fail
loading data files if they are not found in this directory.


-Erno Tuomainen
 ernomat@evitech.fi
 http://www.evitech.fi/~ernomat/index.html



