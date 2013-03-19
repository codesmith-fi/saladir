/* Linux test version */

This is NOT a release version. It's for testing purposes only, due to some
recent Linux problems. For some reason "getpwuid()" fails on some Linux
systems. This's needed to get the players (Unix) username and home
directory. This test version now uses a secondary method if the first call
to getpwuid() fails, it examines two environment variables ($HOME and
$USER).

start with

	saladir_static -debug

to get more info if errors occur.

This is a statically linked version (uses glibc2 and ncurses).


	-Erno


