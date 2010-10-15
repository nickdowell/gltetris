#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <unistd.h>

#define GLUT_KEY_ESC 27

#define GRID_SIZE 20.0f
#define GRID_WIDTH 10
#define GRID_HEIGHT 14

#define CLIP(val, min, max) do { if (val < (min)) { val = (min); } else if (val > (max)) { val = (max); } } while (0)

static int grid_fill[GRID_WIDTH][GRID_HEIGHT];

static int cur_x = 0;
static int cur_y = 0;

static int cur_shape[4][4];

static void new_game();

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

static void new_shape()
{
	switch (rand() % 5) {
		case 0:
			{
				int shape[4][4] = {
					{ 1, 0, 0, 0 },
					{ 1, 0, 0, 0 },
					{ 1, 0, 0, 0 },
					{ 1, 0, 0, 0 },
				};
				memcpy(cur_shape, shape, sizeof(shape));
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
			}
			break;
		case 2:
			{
				int shape[4][4] = {
					{ 1, 1, 1, 0 },
					{ 0, 1, 0, 0 },
					{ 0, 0, 0, 0 },
					{ 0, 0, 0, 0 },
				};
				memcpy(cur_shape, shape, sizeof(shape));
			}
			break;
		case 3:
			{
				int shape[4][4] = {
					{ 1, 1, 0, 0 },
					{ 0, 1, 1, 0 },
					{ 0, 0, 0, 0 },
					{ 0, 0, 0, 0 },
				};
				memcpy(cur_shape, shape, sizeof(shape));
			}
			break;
		case 4:
			{
				int shape[4][4] = {
					{ 0, 1, 1, 0 },
					{ 1, 1, 0, 0 },
					{ 0, 0, 0, 0 },
					{ 0, 0, 0, 0 },
				};
				memcpy(cur_shape, shape, sizeof(shape));
			}
			break;
	}
	cur_y = GRID_HEIGHT - 1;
	cur_x = GRID_WIDTH / 2;
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
					(grid_fill[abs_x][abs_y] == 1))
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

static void burn_shape()
{
	for (int x=0; x<4; x++) {
		for (int y=0; y<4; y++) {
			if (cur_shape[x][y]) {
				grid_fill[cur_x+x][cur_y-y] = 1;
			}
		}
	}
}

static void glut_keyboard(unsigned char key, int x, int y)
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
	cur_y --;
	if (shape_landed()) {
		// current piece has hit the bottom; now out of play
		burn_shape();
		new_shape();
	}
}

static void glut_special(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_UP: do_rotate(); break;
		case GLUT_KEY_LEFT: do_move_x(-1); break;
		case GLUT_KEY_RIGHT: do_move_x(+1); break;
		case GLUT_KEY_DOWN: do_move_down(); break;
		default: return;
	}

	glutPostRedisplay();
}

static void draw_block(int x, int y)
{
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
				draw_block(cur_x+x, cur_y-y);
			}
		}
	}
}

static void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 1.0, 1.0);
	for (int x=0; x<GRID_WIDTH; x++) {
		for (int y=0; y<GRID_HEIGHT; y++) {
			if (grid_fill[x][y]) {
				draw_block(x, y);
			}
		}
	}
	
	glColor3f(1.0, 0.0, 0.0);
	draw_shape();

	glFlush();
}

static void idle()
{
  //glutPostRedisplay();
}

static void new_game()
{
	memset(grid_fill, sizeof(grid_fill), 0);
	memset(cur_shape, sizeof(cur_shape), 0);
	new_shape();
	glutPostRedisplay();
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
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutCreateWindow("gltetris");
	glutDisplayFunc(display);
	glutKeyboardFunc(glut_keyboard);
	glutSpecialFunc(glut_special);
	glutIdleFunc(idle);
	init();
	glutMainLoop();
	return 0;
}

