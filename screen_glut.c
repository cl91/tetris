#include <GL/glut.h>
#include <GL/freeglut.h>
#include "tetris.h"
#include <stdio.h>

/* forward declaration */
static void kbdfunc(unsigned char key, int x, int y);
static void reshape(int w, int h);
static void draw_all(void);
static int draw_block(int x, int y);
static int draw_field(parm_t *s);
static int draw_stat(parm_t *s);
static int draw_info(parm_t *s);
static void special_kbdfunc(int key, int x, int y);

/* constants */
#define STAT_COL 16
#define PIXEL_PER_BLOCK 5.0
#define MAX_X (FIELD_COL + STAT_COL + 2) * PIXEL_PER_BLOCK
#define MAX_Y (FIELD_LINE + 2)* PIXEL_PER_BLOCK

int init_screen(parm_t *s)
{
	glutInitWindowSize(3*MAX_X, 3*MAX_Y);
	glutInit(&(s->argc), s->argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow(s->argv[0]);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_POINT_SMOOTH | GL_LINE_SMOOTH);
	glShadeModel(GL_FLAT);
	glutDisplayFunc(draw_all);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(kbdfunc);
	glutSpecialFunc(special_kbdfunc);

	return ok;
}

int terminate_screen(parm_t *s)
{
	/* no need to do anything */
	return ok;
}

int update_screen(parm_t *s)
{
	glutPostRedisplay();

	return ok;
}

int process_events(parm_t *s)
{
	glutMainLoopEvent();
	return ok;
}

static void timerfunc(int);
int timer_on(parm_t *s)
{
	s->is_timer_on = true;
	glutTimerFunc(itimer_sec * 1000 + itimer_usec / 1000, timerfunc, 0);

	return ok;
}

int timer_off(parm_t *s)
{
	s->is_timer_on = false;

	return ok;
}

static void timerfunc(int i)	/* int i ignored */
{
	if (get_parms_addr()->is_timer_on) {
		alarm_fcn(0);
		glutTimerFunc(itimer_sec * 1000 + itimer_usec / 1000, timerfunc, 0);
	}
}

static void kbdfunc(unsigned char key, int x, int y)
{
	switch (key) {
	case 'j':
		move_down(get_parms_addr());
	break;
	case 'k':
		move_up(get_parms_addr());
	break;
	case 'h':
		move_left(get_parms_addr());
	break;
	case 'l':
		move_right(get_parms_addr());
	break;
	case 'q':
		timer_off(get_parms_addr());
		if (prompt_user_exiting(get_parms_addr()) == exitnow)
			terminate_program(get_parms_addr());
		timer_on(get_parms_addr());
		break;
	default:
		break;
	}
}

static void special_kbdfunc(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_DOWN:
		move_down(get_parms_addr());
		break;
	case GLUT_KEY_UP:
		move_up(get_parms_addr());
		break;
	case GLUT_KEY_LEFT:
		move_left(get_parms_addr());
		break;
	case GLUT_KEY_RIGHT:
		move_right(get_parms_addr());
		break;
	default:
		break;
	}
}

static void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, MAX_X, -MAX_Y, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

static void draw_all(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	draw_field(get_parms_addr());
	draw_stat(get_parms_addr());
	draw_info(get_parms_addr());
	glPopMatrix();
	glutSwapBuffers();
}

int prompt_user_exiting(parm_t *s)
{
	if (s->is_game_over)
		return ok;
	else
		return exitnow;
}

static int draw_block(int x, int y)
{
	glRectf(PIXEL_PER_BLOCK*(x+1), -PIXEL_PER_BLOCK*(y+1),
		PIXEL_PER_BLOCK*(x+2)-1, -PIXEL_PER_BLOCK*(y+2)+1);
	return ok;
}

static int draw_box(float x1, float y1, float x2, float y2)
{
	glLineWidth(2.5);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x1, -y1);
	glVertex2f(x2, -y1);
	glVertex2f(x2, -y2);
	glVertex2f(x1, -y2);
	glEnd();
	return ok;
}

static int draw_field(parm_t *s)
{
	int x, y, i;

	draw_box(PIXEL_PER_BLOCK * 0.5, PIXEL_PER_BLOCK * 0.5,
		 PIXEL_PER_BLOCK * (FIELD_COL + 1.5),
		 PIXEL_PER_BLOCK * (FIELD_LINE + 1.5));
	for (i = 0; i < BLK_PIXEL; i++)
		draw_block(s->current_blk.pos[i].x, s->current_blk.pos[i].y);
	for (x = 0; x < FIELD_COL; x++)
		for (y = 0; y < FIELD_LINE; y++)
			if (s->field[y][x])
				draw_block(x, y);
	return ok;
}

static void draw_string(float x, float y, char *str)
{
	glPushMatrix();
	glLoadIdentity();
	glLineWidth(2.0);
	glTranslatef(x, -y, 0);
	glScalef(0.04, 0.04, 0.04);
	for (; *str != '\0'; str++)
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *str);
	glPopMatrix();
}

#define NEXT_X (PIXEL_PER_BLOCK * (FIELD_COL + 6.0))
#define NEXT_Y (PIXEL_PER_BLOCK * 3)
#define NEXT_BLK_X (FIELD_COL + 7)
#define NEXT_BLK_Y (3)
#define SCORE_LABEL_X (NEXT_X)
#define SCORE_LABEL_Y (PIXEL_PER_BLOCK * (NEXT_BLK_Y + BLK_PIXEL + 2.0) + 2.0)
#define SCORE_X (NEXT_X + PIXEL_PER_BLOCK * 1.5)
#define SCORE_Y (SCORE_LABEL_Y + PIXEL_PER_BLOCK * 2.0 - 2.0)
static int draw_stat(parm_t *s)
{
	struct block blk;
	int i;
	char buf[32];

	draw_box(PIXEL_PER_BLOCK * (FIELD_COL + 2.0),
		 PIXEL_PER_BLOCK * 0.5,
		 MAX_X - PIXEL_PER_BLOCK * 0.5,
		 MAX_Y / 2 + PIXEL_PER_BLOCK * 0.5);
	draw_string(NEXT_X, NEXT_Y, "NEXT:");
	blk = block_type_to_struct(s->new_blk.type, NEXT_BLK_X, NEXT_BLK_Y);
	for (i = 0; i < BLK_PIXEL; i++)
		draw_block(blk.pos[i].x, blk.pos[i].y);
	draw_string(SCORE_LABEL_X, SCORE_LABEL_Y, "SCORE:");
	snprintf(buf, sizeof(buf), "%d", s->score);
	draw_string(SCORE_X, SCORE_Y, buf);
	return ok;
}

#define STR_X (PIXEL_PER_BLOCK * (FIELD_COL + 6.0))
#define STR_Y_START (MAX_Y / 2 + PIXEL_PER_BLOCK * 3.5)
#define STR_Y_STEP (PIXEL_PER_BLOCK * 1.5)
static int draw_info(parm_t *s)
{
	draw_box(PIXEL_PER_BLOCK * (FIELD_COL + 2.0),
		 MAX_Y / 2 + PIXEL_PER_BLOCK,
		 MAX_X - PIXEL_PER_BLOCK * 0.5,
		 MAX_Y - PIXEL_PER_BLOCK * 0.5);
	draw_string(STR_X, STR_Y_START, "q -- quit");
	draw_string(STR_X, STR_Y_START + STR_Y_STEP, "j -- down");
	draw_string(STR_X, STR_Y_START + STR_Y_STEP * 2, "k -- up");
	draw_string(STR_X, STR_Y_START + STR_Y_STEP * 3, "h -- left");
	draw_string(STR_X, STR_Y_START + STR_Y_STEP * 4, "l -- right");
	return ok;
}
