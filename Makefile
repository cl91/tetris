CC = gcc -Wall -g

all:	curses glut

curses: tetris.h main.o screen_curses.o logic.o
	$(CC) -o tetris_curses main.o screen_curses.o logic.o -lcurses

glut: tetris.h main.o screen_glut.o logic.o
	$(CC) -o tetris_glut main.o screen_glut.o logic.o -lX11 -lGL -lGLU -lglut

win: tetris.h main.o screen_glut.o logic.o
	$(CC) -o tetris_win.exe main.o screen_glut.o logic.o -lfreeglut -lopengl32 -lglu32 -Wl,--subsystem,windows

clean:
	rm -f *.o tetris_curses tetris_glut
