/*
  screen initialization and modification routines
  keyboard message receiving routines
*/

#include "tetris.h"
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <sys/time.h>
#include <signal.h>

struct wininfo {
	WINDOW *wp;
};

enum wfield_parms {
	WFIELD_LINE = FIELD_LINE + 2, WFIELD_COL = FIELD_COL + 2,
	WFIELD_STARTX = 0, WFIELD_STARTY = 0,
};

enum wstat_parms {
	WSTAT_LINE = WFIELD_LINE / 2 + 1, WSTAT_COL = 20,
	WSTAT_STARTX = WFIELD_COL + 1, WSTAT_STARTY = 0,
	NEXT_LABEL_X = 4, NEXT_LABEL_Y = 2,
	NEXT_BLK_X = 6, NEXT_BLK_Y = NEXT_LABEL_Y + 2,
	SCORE_LABEL_X = 4, SCORE_LABEL_Y = NEXT_BLK_Y + BLK_PIXEL + 1,
	SCORE_X = 5, SCORE_Y = SCORE_LABEL_Y + 1
};

enum winfo_parms {
	WINFO_LINE = WFIELD_LINE / 2 - 1, WINFO_COL = WSTAT_COL,
	WINFO_STARTX = WSTAT_STARTX, WINFO_STARTY = WSTAT_LINE,
	INFO_X = 4, INFO_Y = 3
};

enum wpop_parms {
	WPOP_LINE = WFIELD_LINE / 3,
	WPOP_COL = WFIELD_COL + WSTAT_COL - 4,
	WPOP_STARTX = 2, WPOP_STARTY = WFIELD_LINE / 3,
	GAMEOVER_BANNER_STARTX = 18, REALLYQUIT_BANNER_STARTX = 17,
	SCORE_BANNER_STARTX = 12, BUTTON_STARTX = 10
};

enum scrmin_parms {
	SCR_MIN_LINE = WFIELD_LINE, SCR_MIN_COL = WFIELD_COL + WSTAT_COL,
};

int init_screen(parm_t *s)
{
	initscr();
	cbreak();
	nonl();
	noecho();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	clear();
	refresh();

	if (LINES < SCR_MIN_LINE || COLS < SCR_MIN_COL) {
		terminate_screen(s);
		fprintf(stderr, "terminal must be larger than %d lines %d columns\n",
			SCR_MIN_LINE, SCR_MIN_COL);
		return fail;
	}

	s->wfield = malloc(sizeof(WINDOW *));
	s->winfo = malloc(sizeof(WINDOW *));
	s->wstat = malloc(sizeof(WINDOW *));
	s->wfield->wp = newwin(WFIELD_LINE, WFIELD_COL, WFIELD_STARTY, WFIELD_STARTX);
	s->wstat->wp = newwin(WSTAT_LINE, WSTAT_COL, WSTAT_STARTY, WSTAT_STARTX);
	s->winfo->wp = newwin(WINFO_LINE, WINFO_COL, WINFO_STARTY, WINFO_STARTX);
	werase(s->wfield->wp);
	werase(s->wstat->wp);
	werase(s->winfo->wp);
	wrefresh(s->wfield->wp);
	wrefresh(s->wstat->wp);
	wrefresh(s->winfo->wp);

	return ok;
}

int terminate_screen(parm_t *s)
{
	clear();
	refresh();
	endwin();
	free(s->wfield);
	free(s->wstat);
	free(s->winfo);

	return ok;
}

int process_events(parm_t *s)
{
	int c;

	c = getch();
	switch (c) {
	case 'k':
	case KEY_UP:
		move_up(s);
	break;
	case 'j':
	case KEY_DOWN:
		move_down(s);
	break;
	case 'h':
	case KEY_LEFT:
		move_left(s);
	break;
	case 'l':
	case KEY_RIGHT:
		move_right(s);
	break;
	case 'q':
		timer_off(s);
		if (prompt_user_exiting(s) == exitnow)
			terminate_program(s);
		timer_on(s);
		break;
	default:
		break;
	}
	return ok;
}

/* turn off SIGALRM and timer */
int timer_off(parm_t *s)
{
	struct sigaction act = {.sa_handler = SIG_IGN};
	sigaction(SIGALRM, &act, NULL);
	struct itimerval itv = {.it_interval = {.tv_sec = 0, .tv_usec = 0},
				.it_value = {.tv_sec = 0, .tv_usec = 0}};
	setitimer(ITIMER_REAL, &itv, NULL);
	s->is_timer_on = false;
	return ok;
}

/* set SIGALRM and timer */
int timer_on(parm_t *s)
{
	struct sigaction act = {.sa_handler = alarm_fcn};
	sigaction(SIGALRM, &act, NULL);
	struct itimerval itv = {.it_interval = {.tv_sec = itimer_sec, .tv_usec = itimer_usec},
				.it_value = {.tv_sec = itimer_sec, .tv_usec = itimer_usec}};
	setitimer(ITIMER_REAL, &itv, NULL);
	s->is_timer_on = true;
	return ok;
}

int prompt_user_exiting(parm_t *s)
{
	int retval = ok;
	WINDOW *wpop = newwin(WPOP_LINE, WPOP_COL, WPOP_STARTY, WPOP_STARTX);
	int c;

	box(wpop, ACS_VLINE, ACS_HLINE);
	if (s->is_game_over)
		mvwaddstr(wpop, 2, GAMEOVER_BANNER_STARTX, "Game Over!");
	else
		mvwaddstr(wpop, 2, REALLYQUIT_BANNER_STARTX, "Really quit?");
	mvwprintw(wpop, 4, SCORE_BANNER_STARTX, "You'v scored %d points", s->score);
	mvwaddstr(wpop, 6, BUTTON_STARTX, s->is_game_over ?
		  "Quit(q)\tNew game(n)" : "Quit(q)\tContinue(c)");
	touchwin(wpop);
	wrefresh(wpop);
	while (true) {
		c = getch();
		if (c == 'q') {
			retval = exitnow;
			break;
		} else if (c == 'n' && s->is_game_over) {
			retval = newgame;
			break;
		} else if (c == 'c' && !s->is_game_over) {
			retval = ok;
			break;
		}
	}
	clear();
	refresh();
	touchwin(s->wfield->wp);
	touchwin(s->winfo->wp);
	touchwin(s->wstat->wp);
	wrefresh(s->wfield->wp);
	wrefresh(s->winfo->wp);
	wrefresh(s->wstat->wp);

	return retval;
}

static int draw_block(parm_t *s)
{
	int i;

	for (i = 0; i < BLK_PIXEL; i++)
		if (s->current_blk.pos[i].x != 0 || s->current_blk.pos[i].y != 0)
			mvwaddch(s->wfield->wp, s->current_blk.pos[i].y + 1, s->current_blk.pos[i].x + 1, BLKCH);
	return ok;
}

static int draw_field(parm_t *s)
{
	int x, y;

	werase(s->wfield->wp);
	box(s->wfield->wp, ACS_VLINE, ACS_HLINE);
	for(x = 0; x < FIELD_COL; x++)
		for (y = 0; y < FIELD_LINE; y++)
			if (s->field[y][x])
				mvwaddch(s->wfield->wp, y + 1, x + 1, s->field[y][x]);
	return ok;
}

static int draw_stat(parm_t *s)
{
	struct block blk;
	int i;

	werase(s->wstat->wp);
	box(s->wstat->wp, ACS_VLINE, ACS_HLINE);
	mvwaddstr(s->wstat->wp, NEXT_LABEL_Y, NEXT_LABEL_X, "NEXT:");
	blk = block_type_to_struct(s->new_blk.type, NEXT_BLK_X, NEXT_BLK_Y);
	for (i = 0; i < BLK_PIXEL; i++)
		if (blk.pos[i].x != 0 || blk.pos[i].y != 0)
			mvwaddch(s->wstat->wp, blk.pos[i].y, blk.pos[i].x, BLKCH);
	mvwaddstr(s->wstat->wp, SCORE_LABEL_Y, SCORE_LABEL_X, "SCORE:");
	mvwprintw(s->wstat->wp, SCORE_Y, SCORE_X, "%d", s->score);

	return ok;
}

static int draw_info(parm_t *s)
{
	werase(s->winfo->wp);
	box(s->winfo->wp, ACS_VLINE, ACS_HLINE);
	mvwaddstr(s->winfo->wp, INFO_Y, INFO_X, "q -- quit");
	mvwaddstr(s->winfo->wp, INFO_Y + 1, INFO_X, "h -- left");
	mvwaddstr(s->winfo->wp, INFO_Y + 2, INFO_X, "j -- down");
	mvwaddstr(s->winfo->wp, INFO_Y + 3, INFO_X, "k -- up");
	mvwaddstr(s->winfo->wp, INFO_Y + 4, INFO_X, "l -- right");

	return ok;
}

int update_screen(parm_t *s)
{
	draw_field(s);
	draw_block(s);
	wrefresh(s->wfield->wp);
	draw_info(s);
	draw_stat(s);
	wrefresh(s->winfo->wp);
	wrefresh(s->wstat->wp);
	return ok;
}
