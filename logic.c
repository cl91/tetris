/* game logic routines */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tetris.h"

/* must confirm to tetris.h */
/* blk.pos[0] is used as a base point */
static struct block Blocks[NR_BLK_TYPE] = {
	{{{0, 0},  {1, 0},  {0, 1},  {1, 1}}, BLK_SQ},
	{{{0, 0},  {1, 0},  {2, 0},  {3, 0}}, BLK_VL},
	{{{0, 0},  {0, 1},  {0, 2},  {0, 3}}, BLK_HL},
	{{{0, 0},  {0, 1},  {-1, 1}, {1, 1}}, BLK_MT_U},
	{{{0, 0},  {1, 0},  {2, 0},  {1, 1}}, BLK_MT_D},
	{{{0, 0},  {0, 1},  {0, 2},  {1, 1}}, BLK_MT_L},
	{{{0, 0},  {0, 1},  {-1, 1}, {0, 2}}, BLK_MT_R},
	{{{0, 0},  {0, 1},  {0, 2},  {1, 2}}, BLK_L_0},
	{{{0, 0},  {0, 1},  {1, 1},  {2, 1}}, BLK_L_1},
	{{{0, 1},  {1, 1},  {2, 1},  {2, 0}}, BLK_L_2},
	{{{0, 0},  {0, 1},  {0, 2},  {-1, 2}}, BLK_L_3},
	{{{0, 0},  {0, 1},  {0, 2},  {1, 0}}, BLK_L_4},
	{{{0, 0},  {1, 0},  {1, 1},  {1, 2}}, BLK_L_5}
};

static enum blk_type Next_blktype[NR_BLK_TYPE] = {
	BLK_SQ, BLK_HL, BLK_VL,
	BLK_MT_D, BLK_MT_L, BLK_MT_R, BLK_MT_U,
	BLK_L_1, BLK_L_2, BLK_L_3, BLK_L_4, BLK_L_5, BLK_L_0
};

static enum blk_type rand_block_type(void)
{
	return rand() % NR_BLK_TYPE;
}

/* return a non-zero random x position */
static int rand_x_pos(void)
{
	int i;

	while ((i = rand() % FIELD_RAND_COL) == 0)
		;
	return i;
}

/* see enum blk_type in tetris.h */
static enum blk_type windup_block_type(enum blk_type t)
{
	if (t >= NR_BLK_TYPE)
		return BLK_SQ;
	return Next_blktype[t];
}

struct block block_type_to_struct(enum blk_type type, int x, int y)
{
	struct block blk = Blocks[type];
	int i;

	for (i = 0; i < BLK_PIXEL; i++) {
		blk.pos[i].x += x;
		blk.pos[i].y += y;
	}
	return blk;
}

static bool has_reached_bottom(parm_t *s)
{
	int i;
	struct block blk = s->current_blk;

	for (i = 0; i < BLK_PIXEL; i++)
		if (s->field[blk.pos[i].y+1][blk.pos[i].x] || blk.pos[i].y >= FIELD_LINE - 1)
			return true;
	return false;
}

static int add_to_field(char field[][FIELD_COL], struct block blk)
{
	int i;

	for (i = 0; i < BLK_PIXEL; i++)
		field[blk.pos[i].y][blk.pos[i].x] = BLKCH;
	return ok;
}

static bool is_field_line_full(char field[][FIELD_COL], int y)
{
	int x;

	for (x = 0; x < FIELD_COL; x++)
		if (field[y][x] == 0)
			return false;
	return true;
}

static int zap_field_line(char field[][FIELD_COL], int line)
{
	int x, y;

	for (x = 0; x < FIELD_COL; x++)
		for (y = line; y > 0; y--)
			field[y][x] = field[y-1][x];
	return ok;
}

static bool is_game_over(parm_t *s)
{
	int x;

	for (x = 0; x < FIELD_COL; x++)
		if (s->field[GAMEOVER_LINE][x])
			return true;
	return false;
}

int move_down(parm_t *s)
{
	int i;
	struct block blk = s->current_blk;

	s->has_reached_bottom = has_reached_bottom(s);
	if (s->has_reached_bottom)
		return fail;
	for (i = 0; i < BLK_PIXEL; i++)
		blk.pos[i].y++;
	s->current_blk = blk;
	update_screen(s);

	return ok;
}

int move_left(parm_t *s)
{
	int i;
	struct block blk = s->current_blk;

	for (i = 0; i < BLK_PIXEL; i++) {
		blk.pos[i].x--;
		if (blk.pos[i].x < 0)
			return fail;
	}
	s->current_blk = blk;
	update_screen(s);

	return ok;
}

int move_right(parm_t *s)
{
	struct block blk = s->current_blk;
	int i;

	for (i = 0; i < BLK_PIXEL; i++) {
		blk.pos[i].x++;
		if (blk.pos[i].x >= FIELD_COL)
			return fail;
	}
	s->current_blk = blk;
	update_screen(s);

	return ok;
}

int move_up(parm_t *s)
{
	struct block newblk, *pb = &(s->current_blk);
	enum blk_type curtype, newtype;
	int x, y;

	curtype = pb->type;
	newtype = windup_block_type(curtype);
	x = pb->pos[0].x - Blocks[curtype].pos[0].x;
	y = pb->pos[0].y - Blocks[curtype].pos[0].y;
	newblk = block_type_to_struct(newtype, x, y);
	s->current_blk = newblk;
	update_screen(s);

	return ok;
}

void terminate_program(parm_t *s)
{
	terminate_screen(s);
	exit(EXIT_SUCCESS);
}

int init_game(parm_t *s)
{
	srand(time(NULL));
	s->has_reached_bottom = false;
	s->current_blk = block_type_to_struct(rand_block_type(), rand_x_pos(), 0);
	s->new_blk = block_type_to_struct(rand_block_type(), rand_x_pos(), 0);

	return ok;
}

int reinit_game(parm_t *s)
{
	s->current_blk = block_type_to_struct(rand_block_type(), rand_x_pos(), 0);
	s->new_blk = block_type_to_struct(rand_block_type(), rand_x_pos(), 0);
	memset(s->field, 0, FIELD_COL * FIELD_LINE);
	s->has_reached_bottom = false;
	s->is_game_over = false;
	s->score = 0;

	return ok;
}

void alarm_fcn(int signum)
{
	parm_t *s = get_parms_addr();
	int y;

	if (s->is_game_over)
		return;
	move_down(s);
	if (s->has_reached_bottom) {
		/* add block to field, check for deletion */
		add_to_field(s->field, s->current_blk);
		for (y = 0; y < FIELD_LINE; y++)
			if (is_field_line_full(s->field, y)) {
				zap_field_line(s->field, y);
				s->score++;
			}

		/* check for game-over condition */
		if (is_game_over(s)) {
			s->is_game_over = true;
			return;
		}

		/* if not game-over, generate a new block and update screen */
		s->current_blk = s->new_blk;
		s->new_blk = block_type_to_struct(rand_block_type(), rand_x_pos(), 0);
		update_screen(s);
		s->has_reached_bottom = false;
	}
}

int start_game(parm_t *s)
{
	update_screen(s);
	/* set SIGALRM and timer */
	timer_on(s);
	/* get the ball rolling */
	while (true) {
		if (s->is_game_over) {
			timer_off(s);
			if (prompt_user_exiting(s) == exitnow)
				return exitnow;
			else
				return newgame;
		}
		process_events(s);
	}
	return exitnow;
}
