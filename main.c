/* main source file for the tetris game */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tetris.h"

int main(int argc, char **argv)
{
	parm_t *s = calloc(1, sizeof(*s));

	if (s == NULL) {
		perror("initialization failure");
		abort();
	}
	set_parms_addr(s);
	s->argc = argc;
	s->argv = argv;
	init_game(s);
	if (init_screen(s) != ok) {
		fprintf(stderr, "screen initialization failure\n");
		abort();
	}

	while (start_game(s) == newgame) {
		reinit_game(s);
	}
	terminate_program(s);
	return 0;
}

static parm_t *P_parms = NULL;
void set_parms_addr(parm_t *s)
{
	P_parms = s;
}
parm_t *get_parms_addr(void)
{
	return P_parms;
}
