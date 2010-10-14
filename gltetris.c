#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>
#include <unistd.h>

#define GLUT_KEY_ESC 27

#define GRID_SIZE 20.0f
#define GRID_WIDTH 10
#define GRID_HEIGHT 14

#define CLIP(val, min, max) do { if (val < (min)) { val = (min); } else if (val > (max)) { val = (max); } } while (0)

static int cur_x = GRID_WIDTH / 2;
static int cur_y = GRID_HEIGHT - 1;
static int grid_fill[GRID_WIDTH][GRID_HEIGHT];

static void glut_keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_ESC: exit(0); break;
		default: return;
	}
}

static void glut_special(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_UP: cur_y ++; break;
		case GLUT_KEY_LEFT: cur_x --; break;
		case GLUT_KEY_DOWN: cur_y --; break;
		case GLUT_KEY_RIGHT: cur_x ++; break;
		default: return;
	}
	
	CLIP(cur_x, 0, GRID_WIDTH  - 1);
	CLIP(cur_y, 0, GRID_HEIGHT - 1);

	if (cur_y == 0 || grid_fill[cur_x][cur_y-1] == 1) {
		// current piece has hit the bottom; now out of play
		grid_fill[cur_x][cur_y] = 1;
		cur_y = GRID_HEIGHT - 1;
		cur_x = GRID_WIDTH / 2;
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
	draw_block(cur_x, cur_y);

	glFlush();
}

static void idle()
{
  //glutPostRedisplay();
}

static void init()
{
	memset(grid_fill, sizeof(grid_fill), 0);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, GRID_WIDTH * GRID_SIZE, 0.0, GRID_HEIGHT * GRID_SIZE, 0.0, 1.0);
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
