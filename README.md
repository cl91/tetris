tetris
======

A tetris game with both OpenGL and ncurses backends

Tetris is an old project which I worked on during my free time when I was a freshman in my undergraduate university. It is written in a modular fashion with both OpenGL and ncurses backends for painting the user-interface, and a generic layer for handling the game logic. It is written in C and is tested in both Linux (my primary OS) and Windows (maybe a little rough around the edges as I do not use Windows as my primary OS).


Dependencies and Installation
======

The generic game logic code is ANSI C. The OpenGL backend is dependent upon OpenGL libraries and freeglut. The ncurses backend depends on ncurses.

To compile on Linux, issue:

make

To compile on Windows, issue:

make win

To clean up the workspace, issue

make clean
