#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <unistd.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

#define MSEC_PER_SEC 1000

#define GLUT_KEY_ESC 27

#define GRID_SIZE 40.0f
#define GRID_WIDTH 10
#define GRID_HEIGHT 16

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

typedef struct { float r; float g; float b; } color_t;
static color_t colors[] = {
	{ 229.0 / 255.0,  59.0 / 255.0,  76.0 / 255.0 },
	{ 242.0 / 255.0, 135.0 / 255.0,   0.0 / 255.0 },
	{ 153.0 / 255.0, 140.0 / 255.0,   0.0 / 255.0 },
	{  36.0 / 255.0,  89.0 / 255.0,  38.0 / 255.0 },
	{  46.0 / 255.0,  36.0 / 255.0,  51.0 / 255.0 },
};

#define NUM_COLORS sizeof(colors) / sizeof(colors[0])

static int grid_fill [GRID_WIDTH] [GRID_HEIGHT];

static int cur_x = 0;
static int cur_y = 0;

static int cur_shape[4][4];

static int game_over = 0;
static int game_score = 0;
static int game_time_unit = 0; // time (in usecs) between automatic dropping

static int timer_generation = 0;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

static void new_game();

static int  shape_fits(int x, int y);

static void game_update_title();

static void glut_timer_callback(int value);
static void glut_keyboard_callback(unsigned char key, int x, int y);
static void glut_specialkey_callback(int key, int x, int y);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

static void rotate_shape()
{
	int old_shape[4][4];
	memcpy(old_shape, cur_shape, sizeof(old_shape));
	for (int y=0; y<4; y++) {
		for (int x=0; x<4; x++) {
			cur_shape[x][y] = old_shape[y][3-x];
		}
	}
}

static void set_shape_color(int colorIndex)
{
	for (int y=0; y<4; y++) {
		for (int x=0; x<4; x++) {
			cur_shape[x][y] *= colorIndex;
		}
	}
}

static void new_shape()
{
	int color = 0;
	switch (rand() % 7) {
		case 0:
			{
				int shape[4][4] = {
					{ 0, 1, 0, 0 },
					{ 0, 1, 0, 0 },
					{ 0, 1, 0, 0 },
					{ 0, 1, 0, 0 },
				};
				memcpy(cur_shape, shape, sizeof(shape));
				color = 1;
			}
			break;
		case 1:
			{
				int shape[4][4] = {
					{ 0, 0, 0, 0 },
					{ 0, 1, 1, 0 },
					{ 0, 1, 1, 0 },
					{ 0, 0, 0, 0 },
				};
				memcpy(cur_shape, shape, sizeof(shape));
				color = 2;
			}
			break;
		case 2:
			{
				int shape[4][4] = {
					{ 0, 0, 0, 0 },
					{ 1, 1, 1, 0 },
					{ 0, 1, 0, 0 },
					{ 0, 0, 0, 0 },
				};
				memcpy(cur_shape, shape, sizeof(shape));
				color = 3;
			}
			break;
		case 3:
			{
				int shape[4][4] = {
					{ 0, 0, 0, 0 },
					{ 0, 1, 1, 0 },
					{ 0, 0, 1, 1 },
					{ 0, 0, 0, 0 },
				};
				memcpy(cur_shape, shape, sizeof(shape));
				color = 4;
			}
			break;
		case 4:
			{
				int shape[4][4] = {
					{ 0, 0, 0, 0 },
					{ 0, 1, 1, 0 },
					{ 1, 1, 0, 0 },
					{ 0, 0, 0, 0 },
				};
				memcpy(cur_shape, shape, sizeof(shape));
				color = 4;
			}
			break;
		case 5:
			{
				int shape[4][4] = {
					{ 0, 1, 0, 0 },
					{ 0, 1, 0, 0 },
					{ 0, 1, 1, 0 },
					{ 0, 0, 0, 0 },
				};
				memcpy(cur_shape, shape, sizeof(shape));
				color = 5;
			}
			break;
		case 6:
			{
				int shape[4][4] = {
					{ 0, 0, 1, 0 },
					{ 0, 0, 1, 0 },
					{ 0, 1, 1, 0 },
					{ 0, 0, 0, 0 },
				};
				memcpy(cur_shape, shape, sizeof(shape));
				color = 5;
			}
			break;
	}

	set_shape_color(color);
	rotate_shape();

	cur_x = GRID_WIDTH / 2 - 2;
	cur_y = GRID_HEIGHT + 1;
	if (!shape_fits(cur_x, cur_y)) cur_y --;
	if (!shape_fits(cur_x, cur_y)) cur_y --;
	if (!shape_fits(cur_x, cur_y)) {
		memset(cur_shape, sizeof(cur_shape), 0);
		game_over = 1;
		game_update_title();
	}
}

static int shape_fits(int off_x, int off_y)
{
	for (int x=0; x<4; x++) {
		for (int y=0; y<4; y++) {
			if (cur_shape[x][y]) {
				int abs_x = off_x + x;
				int abs_y = off_y - y;
				if ((abs_y < 0 || abs_y >= GRID_HEIGHT) ||
					(abs_x < 0 || abs_x >= GRID_WIDTH) ||
					(grid_fill[abs_x][abs_y]))
					return 0;
			}
		}
	}
	return 1;
}

static int shape_landed()
{
	return shape_fits(cur_x, cur_y - 1) ? 0 : 1;
}

static void game_clear_all_rows()
{
	for (int y=0; y<GRID_HEIGHT; y++) {
		for (int x=0; x<GRID_WIDTH; x++) {
			grid_fill[x][y] = 0;
		}
	}
}

static int game_clear_rows()
{
	int rows_cleared = 0;

	for (int y=0; y<GRID_HEIGHT; y++) {
		int row_full = 1;
		for (int x=0; x<GRID_WIDTH; x++) {
			if (!grid_fill[x][y]) {
				row_full = 0;
				break;
			}
		}
		if (row_full) {
			rows_cleared ++;
			// clear row
			for (int x=0; x<GRID_WIDTH; x++)
				grid_fill[x][y] = 0;
			// drop rows down
			for (int i=y; i<GRID_HEIGHT-1; i++) {
				for (int x=0; x<GRID_WIDTH; x++) {
					grid_fill[x][i] = grid_fill[x][i+1];
					grid_fill[x][i+1] = 0;
				}
			}
			y--; // process same row again
		}
	}

	if (rows_cleared) {
		int points = (1 << (rows_cleared - 1));
		game_score += points;
		game_update_title();
	}

	return rows_cleared;
}

static void burn_shape()
{
	for (int x=0; x<4; x++) {
		for (int y=0; y<4; y++) {
			if (cur_shape[x][y]) {
				grid_fill[cur_x+x][cur_y-y] = cur_shape[x][y];
			}
		}
	}

	game_clear_rows();

	// increase the game speed!
	game_time_unit -= game_time_unit / 100;
}

static void glut_keyboard_callback(unsigned char key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_ESC: exit(0); break;
		case 'r': new_game(); break;
		default: return;
	}
}

static void do_rotate()
{
	int save_shape[4][4];
	memcpy(save_shape, cur_shape, sizeof(cur_shape));
	rotate_shape();
	if (!shape_fits(cur_x, cur_y))
		memcpy(cur_shape, save_shape, sizeof(cur_shape));
}

static void do_move_x(int off_x)
{
	int new_x = cur_x + off_x;
	if (shape_fits(new_x, cur_y))
		cur_x = new_x;
}

static void do_move_down()
{
	if (shape_landed()) {
		// current piece has hit the bottom; now out of play
		burn_shape();
		new_shape();
	}
	cur_y --;

	timer_generation ++; // reschedule auto-drop timer
	glutTimerFunc(game_time_unit, glut_timer_callback, timer_generation);
}

static void glut_specialkey_callback(int key, int x, int y)
{
	if (game_over)
		return;

	switch (key) {
		case GLUT_KEY_UP: do_rotate(); break;
		case GLUT_KEY_LEFT: do_move_x(-1); break;
		case GLUT_KEY_RIGHT: do_move_x(+1); break;
		case GLUT_KEY_DOWN: do_move_down(); break;
		default: return;
	}

	glutPostRedisplay();
}

static void draw_block(int x, int y, int c)
{
	glColor3f(colors[c-1].r, colors[c-1].g, colors[c-1].b);
	float x1 = x  * GRID_SIZE;
	float x2 = x1 + GRID_SIZE;
	float y1 = y  * GRID_SIZE;
	float y2 = y1 + GRID_SIZE;
	glBegin(GL_POLYGON);
	glVertex2f(x1, y1);
	glVertex2f(x1, y2);
	glVertex2f(x2, y2);
	glVertex2f(x2, y1);
	glEnd();
}

static void draw_shape()
{
	for (int x=0; x<4; x++) {
		for (int y=0; y<4; y++) {
			if (cur_shape[x][y]) {
				draw_block(cur_x+x, cur_y-y, cur_shape[x][y]);
			}
		}
	}
}

static void glut_display_callback()
{
	glClear(GL_COLOR_BUFFER_BIT);

	for (int x=0; x<GRID_WIDTH; x++) {
		for (int y=0; y<GRID_HEIGHT; y++) {
			if (grid_fill[x][y]) {
				draw_block(x, y, grid_fill[x][y]);
			}
		}
	}
	
	if (!game_over) {
		draw_shape();
	}

	glFlush();

	glutSwapBuffers();
}

static void glut_timer_callback(int value)
{
	if (game_over)
		return;

	if (value != timer_generation)
		return;

	do_move_down();

	glutTimerFunc(game_time_unit, glut_timer_callback, ++timer_generation);
	glutPostRedisplay();
}

static void game_update_title()
{
	char title[64] = "";
	sprintf(title, "TETRIS   score = %d  %s", game_score, game_over ? "[ GAME OVER ]" : "");
	glutSetWindowTitle(title);
}

static void new_game()
{
	game_over = 0;
	game_score = 0;
	game_update_title();
	timer_generation ++;
	game_time_unit = MSEC_PER_SEC;
	game_clear_all_rows();
	glutTimerFunc(game_time_unit, glut_timer_callback, timer_generation);
	glutPostRedisplay();
	new_shape();
}

static void init()
{
	srand(time(NULL));

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, GRID_WIDTH * GRID_SIZE, 0.0, GRID_HEIGHT * GRID_SIZE, 0.0, 1.0);

	new_game();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(GRID_WIDTH * GRID_SIZE, GRID_HEIGHT * GRID_SIZE);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("gltetris");
	glutDisplayFunc(glut_display_callback);
	glutKeyboardFunc(glut_keyboard_callback);
	glutSpecialFunc(glut_specialkey_callback);
	init();
	glutMainLoop();
	return 0;
}

