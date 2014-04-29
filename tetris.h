#include <stdbool.h>

/* constants */
#define BLKCH	'#'
#define BLK_PIXEL	4
enum blk_type {
	BLK_SQ, BLK_VL, BLK_HL,
	BLK_MT_U, BLK_MT_D, BLK_MT_L, BLK_MT_R,
	BLK_L_0, BLK_L_1, BLK_L_2, BLK_L_3, BLK_L_4, BLK_L_5,
	NR_BLK_TYPE
};

enum field_parms {
	FIELD_LINE = 22, FIELD_COL = 28,
	FIELD_RAND_COL = FIELD_COL - BLK_PIXEL, GAMEOVER_LINE = 3
};

enum ret_code {
	ok, fail, exitnow, newgame
};

enum update_frequancy {
	itimer_sec = 0, itimer_usec = 500000
};

/* data structures */
struct position {
	int x, y;
};

struct block {
	struct position pos[BLK_PIXEL];
	enum blk_type type;
};

struct wininfo;

struct parms {
	int argc;
	char **argv;
	bool is_game_over;
	bool is_timer_on;
	int score;
	bool has_reached_bottom;
	struct block current_blk, new_blk;
	char field[FIELD_LINE][FIELD_COL];
	struct wininfo *wfield, *wstat, *winfo;
};
typedef struct parms parm_t;

/* forward declaration */
/* main.c */
parm_t *get_parms_addr(void);
void set_parms_addr(parm_t *);
/* screen.c */
int init_screen(parm_t *);
int terminate_screen(parm_t *);
int prompt_user_exiting(parm_t *);
int update_screen(parm_t *s);
int process_events(parm_t *s);
int timer_on(parm_t *);
int timer_off(parm_t *);

/* logic.c */
void terminate_program(parm_t *);
int init_game(parm_t *);
int reinit_game(parm_t *);
struct block block_type_to_struct(enum blk_type, int, int);
int start_game(parm_t *);
void alarm_fcn(int);
int move_up(parm_t *);
int move_down(parm_t *);
int move_left(parm_t *);
int move_right(parm_t *);
