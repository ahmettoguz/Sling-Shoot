/*********
   CTIS164 - Template Source Program
----------
STUDENT : Ahmet Oðuz Ergin
SECTION : 3
HOMEWORK: 2
----------
PROBLEMS: -
----------
ADDITIONAL FEATURES:

 --> I have used state machine to switch screens
 --> There are opening, countdown, animation and End screens
 --> In opening screen you can select difficulty of the game, in left corner you can see the information about it
 --> Speed, the number of target, background and also time of the game is changing according to difficulty of the game
 --> in countdown screen you will wait 5 second before game begin (You can press 's' to skip it)
 --> My targets are bird and while they are flying in x axis they are also moving up and down in y axis
 --> After sling shooted The rope is released and if stone exceed the screen it will come back to old position
 --> There is remaining time in animation screen
 --> There is score in animation screen
 --> You can see the high score in the beginning and the end screen. It is updating in every game
 --> After game is over you can start it again by pressing enter in end screen

*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 700

#define TIMER_PERIOD  1000 // Period for the timer.
#define TIMER_ON         1// 0:disable timer, 1:enable timer

#define D2R 0.0174532

typedef struct {
	double xax, yax, firstyax;
	int r, g, b, updown;
	bool active;
}birds_t;

typedef struct {
	float x, y;
	bool shooted;
}sling_t;

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
//menu screen
char screen, difficulty = 2;
int difficultrecy, besthighscr = 0, timerperi, j, score = 0;
double remaintime, timeCounter, splitsec;
birds_t bird[200];
sling_t sling, stone;


void inittomenu1()
{
	screen = 1;
	difficultrecy = 95 - difficulty * 75;
	timeCounter = 0;
	timerperi = 1000;

	//update highscore
	if (score > besthighscr)
		besthighscr = score;
	score = 0;
}

void startgameinit()
{
	//birds deactivation
	for (int i = 0; i < 50; i++)
	{
		bird[i].active = false;

		bird[i].xax = 720;
		bird[i].updown = 1;
	}

	//set sling
	sling.x = 0;
	sling.y = -210;
	sling.shooted = false;

	//set stone
	{
		stone.x = 0;
		stone.y = -175;
		stone.shooted = false;
	}

	//set time period
	{
		j = 0;
		timerperi = 20;
		timeCounter = 0;
		splitsec = 0;

		if (difficulty == 3)
			remaintime = 16;
		else if (difficulty == 2)
			remaintime = 21;
		else if (difficulty == 1)
			remaintime = 26;
	}

	score = 0;
}

//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char* string, void* font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void dispbackrec()
{
	if (screen == 3)
	{
		glBegin(GL_QUADS);
		if (difficulty == 1)
			glColor3ub(106, 253, 118);
		else if (difficulty == 2)
			glColor3ub(231, 159, 41);
		else if (difficulty == 3)
			glColor3ub(250, 44, 20);

		//glColor3ub(213, 213, 250);
		glVertex2f(-600, -350);
		glVertex2f(600, -350);

		if (difficulty == 1)
			glColor3ub(232, 215, 40);
		else if (difficulty == 2)
			glColor3ub(249, 255, 17);
		else if (difficulty == 3)
			glColor3ub(231, 159, 41);

		//glColor3ub(130, 130, 230);
		glVertex2f(600, 350);
		glVertex2f(-600, 350);

		glEnd();
	}
}

void dismenu(int difficultrecy)
{
	if (screen == 1)
	{
		//big rectangle
		{
			glBegin(GL_QUADS);
			glColor3ub(187, 112, 203);
			glVertex2f(-600, -350);
			glVertex2f(600, -350);

			glColor3ub(247, 53, 82);
			glVertex2f(600, 350);
			glVertex2f(-600, 350);

			glEnd();
		}

		//textbox
		{
			//easy
			{
				glBegin(GL_QUADS);
				glColor3ub(100, 255, 122);
				glVertex2f(-135, 20);
				glVertex2f(110, 20);

				glColor3ub(255, 209, 26);
				glVertex2f(110, -55);
				glVertex2f(-135, -55);

				glEnd();
			}

			//medium
			{
				glBegin(GL_QUADS);
				glColor3ub(255, 209, 26);
				glVertex2f(-135, -55);
				glVertex2f(110, -55);

				glColor3ub(255, 209, 26);
				glVertex2f(110, -130);
				glVertex2f(-135, -130);

				glEnd();
			}

			//hard
			{
				glBegin(GL_QUADS);
				glColor3ub(255, 209, 26);
				glVertex2f(110, -130);
				glVertex2f(-135, -130);

				glColor3ub(255, 40, 40);
				glVertex2f(-135, -205);
				glVertex2f(110, -205);

				glEnd();
			}

			//outrectangle
			{
				//blue
				glColor3ub(255, 23, 0);
				glLineWidth(8);
				glBegin(GL_LINE_LOOP);
				glVertex2f(-135, difficultrecy);
				glVertex2f(110, difficultrecy);
				glVertex2f(110, difficultrecy - 75);
				glVertex2f(-135, difficultrecy - 75);
				glEnd();

				//blue
				glColor3ub(88, 214, 141);
				glLineWidth(5);
				glBegin(GL_LINE_LOOP);
				glVertex2f(-135, difficultrecy);
				glVertex2f(110, difficultrecy);
				glVertex2f(110, difficultrecy - 75);
				glVertex2f(-135, difficultrecy - 75);
				glEnd();

				//black
				glColor3ub(0, 0, 0);
				glLineWidth(3);
				glBegin(GL_LINE_LOOP);
				glVertex2f(-135, difficultrecy);
				glVertex2f(110, difficultrecy);
				glVertex2f(110, difficultrecy - 75);
				glVertex2f(-135, difficultrecy - 75);
				glEnd();
			}
		}

		//texts
		{
			glLineWidth(4);
			glColor3ub(142, 68, 173);
			vprint2(-498, 152, 1, "SlingShot Game");

			glLineWidth(3);
			glColor3ub(0, 0, 0);
			vprint2(-500, 150, 1, "SlingShot Game");

			glLineWidth(2.5);
			glColor3ub(5, 5, 140);
			vprint2(-502, 148, 1, "SlingShot Game");

			glColor3ub(0, 0, 0);
			glLineWidth(4);
			vprint2(-202, 48, 0.3, "Select Difficulty Level");

			glColor3ub(72, 201, 176);
			glLineWidth(1.5);
			vprint2(-200, 50, 0.3, "Select Difficulty Level");

			glColor3ub(0, 0, 0);
			glLineWidth(1.8);
			vprint2(-105, -250, 0.2, "Best score: %d", besthighscr);

			glColor3ub(0, 0, 0);
			glLineWidth(1.8);
			vprint2(350, -270, 0.2, "HW 2 ");
			vprint2(250, -300, 0.2, "By Ahmet Oguz Ergin");

			//lines
			{
				glBegin(GL_LINES);
				glVertex2f(251, -315);
				glVertex2f(537, -315);
				glVertex2f(257, -325);
				glVertex2f(531, -325);
				glEnd();
			}
		}

		//difficult text
		{
			//easy
			glLineWidth(1.5);
			glColor3ub(0, 0, 0);
			vprint2(-50, -30, 0.3, "Easy");
			//medium		
			glLineWidth(1.5);
			glColor3ub(0, 0, 0);
			vprint2(-72, -105, 0.3, "Medium");
			//hard
			glLineWidth(1.5);
			glColor3ub(0, 0, 0);
			vprint2(-50, -180, 0.3, "Hard");
		}

		//triangles for difficulty levels
		{
			//black
			glBegin(GL_TRIANGLES);
			glVertex2f(-190, difficultrecy + -17);
			glVertex2f(-150, difficultrecy + -42);
			glVertex2f(-190, difficultrecy + -67);

			glVertex2f(165, difficultrecy + -17);
			glVertex2f(125, difficultrecy + -42);
			glVertex2f(165, difficultrecy + -67);
			glEnd();

			//blue
			glColor3ub(72, 201, 176);
			glBegin(GL_TRIANGLES);
			glVertex2f(-186, difficultrecy + -25);
			glVertex2f(-158, difficultrecy + -42);
			glVertex2f(-186, difficultrecy + -59);

			glVertex2f(161, difficultrecy + -25);
			glVertex2f(133, difficultrecy + -42);
			glVertex2f(161, difficultrecy + -59);
			glEnd();
		}

		//left information box
		{
			glColor3ub(92, 254, 126);
			glRectf(-600, -350, -250, -235);

			//frame
			glColor3f(0, 0, 0);
			glBegin(GL_LINE_LOOP);
			glVertex2f(-600, -235);
			glVertex2f(-250, -235);
			glVertex2f(-250, -350);
			glVertex2f(-600, -350);
			glEnd();

			glColor3f(0, 0, 0);
			vprint(-580, -260, GLUT_BITMAP_9_BY_15, "Easy: 25 second, 20 bird");
			vprint(-580, -280, GLUT_BITMAP_9_BY_15, "Medium: 20 second, 16 bird");
			vprint(-580, -300, GLUT_BITMAP_9_BY_15, "Hard: 15 second, 12 bird");
			vprint(-580, -340, GLUT_BITMAP_9_BY_15, "Use up / down arrow key and enter");
		}
	}

	if (screen == 2)
	{
		//big rectangle
		{
			glBegin(GL_QUADS);
			glColor3ub(187, 112, 203);
			glVertex2f(-600, -350);
			glVertex2f(600, -350);

			glColor3ub(247, 53, 82);
			glVertex2f(600, 350);
			glVertex2f(-600, 350);

			glEnd();
		}

		//message
		{
			//black frame 
			glLineWidth(4);
			glColor3ub(0, 0, 0);
			vprint2(-312, 149, 0.5, "Game Will Begin In");
			vprint2(-152, -1, 0.5, "%.0f Second", 5 - timeCounter);
			vprint2(-232, -151, 0.5, "GOOD LUCK :)");

			glLineWidth(3);
			glColor3ub(241, 196, 15);
			vprint2(-310, 150, 0.5, "Game Will Begin In");
			vprint2(-150, 0, 0.5, "%.0f Second", 5 - timeCounter);
			vprint2(-230, -150, 0.5, "GOOD LUCK :)");
		}

		//circles

		//black
		glColor3f(0, 0, 0);
		circle(-226, 23, 15);
		circle(233, 21, 15);

		//yellow
		glColor3ub(241, 196, 15);
		circle(-226, 23, 10);
		circle(233, 21, 10);

		//black
		glColor3f(0, 0, 0);
		circle(-226, 23, 4);
		circle(233, 21, 4);
	}
}

void dispBird(birds_t* bird)
{
	for (int i = 0; i < 50; i++)
		if ((bird + i)->active)
		{
			//big body circle
			{
				glColor3ub(bird[i].r, bird[i].g, bird[i].b);
				circle(bird[i].xax, bird[i].yax, 40);
			}
			//wing
			{
				//wing
				{
					glColor3f(1, 1, 1);
					glBegin(GL_POLYGON);
					glVertex2f(bird[i].xax - 31, bird[i].yax + 20);
					glVertex2f(bird[i].xax - 32, bird[i].yax - 8);
					glVertex2f(bird[i].xax + 80, bird[i].yax - 9);
					glEnd();
				}
				//frame
				{
					glLineWidth(2);
					glColor3f(0, 0, 0);
					glBegin(GL_LINE_LOOP);
					glVertex2f(bird[i].xax - 31, bird[i].yax + 20);
					glVertex2f(bird[i].xax - 32, bird[i].yax - 8);
					glVertex2f(bird[i].xax + 80, bird[i].yax - 9);
					glEnd();


					glBegin(GL_LINES);
					for (int j = bird[i].xax - 31; j <= bird[i].xax + 80; j += 15)
					{
						glVertex2f(bird[i].xax - 31, bird[i].yax + 20);
						glVertex2f(j, bird[i].yax - 9);
					}
					glEnd();
				}
			}

			//head circle
			{
				glColor3ub(bird[i].r, bird[i].g, bird[i].b);
				circle(bird[i].xax - 50, bird[i].yax + 26, 25);
			}

			//foot
			{
				//up
				{

					glColor3f(0, 0, 0);
					glLineWidth(4);

					glBegin(GL_LINES);
					glVertex2f(bird[i].xax + 13, bird[i].yax - 18);
					glVertex2f(bird[i].xax + 50, bird[i].yax - 25);

					glVertex2f(bird[i].xax + 59, bird[i].yax - 18);
					glVertex2f(bird[i].xax + 50, bird[i].yax - 25);

					glVertex2f(bird[i].xax + 62, bird[i].yax - 34);
					glVertex2f(bird[i].xax + 50, bird[i].yax - 25);
					glEnd();
				}

				//down
				{
					glColor3f(0, 0, 0);
					glLineWidth(4);

					glBegin(GL_LINES);
					glVertex2f(bird[i].xax + 18, bird[i].yax - 25);
					glVertex2f(bird[i].xax + 40, bird[i].yax - 36);

					glVertex2f(bird[i].xax + 40, bird[i].yax - 36);
					glVertex2f(bird[i].xax + 50, bird[i].yax - 30);

					glVertex2f(bird[i].xax + 40, bird[i].yax - 36);
					glVertex2f(bird[i].xax + 48, bird[i].yax - 48);
					glEnd();
				}
			}

			//eye
			{
				glColor3f(1, 1, 1);
				circle(bird[i].xax - 60, bird[i].yax + 32, 8);
				//circle 
				glColor3f(0, 0, 0);
				circle(bird[i].xax - 62, bird[i].yax + 33, 5);
			}

			//tail
			{
				//tail
				{
					glColor3ub(185, 122, 87);
					glBegin(GL_POLYGON);
					glVertex2f(bird[i].xax + 31, bird[i].yax + 21);
					glVertex2f(bird[i].xax + 46, bird[i].yax + 37);
					glVertex2f(bird[i].xax + 44, bird[i].yax + 21);
					glVertex2f(bird[i].xax + 57, bird[i].yax + 16);
					glVertex2f(bird[i].xax + 33, bird[i].yax + 10);
					glEnd();
				}

				//frame
				{
					glLineWidth(2);
					glColor3f(0, 0, 0);
					glBegin(GL_LINES);
					glVertex2f(bird[i].xax + 32, bird[i].yax + 16);
					glVertex2f(bird[i].xax + 45, bird[i].yax + 22);
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex2f(bird[i].xax + 31, bird[i].yax + 21);
					glVertex2f(bird[i].xax + 46, bird[i].yax + 37);
					glVertex2f(bird[i].xax + 44, bird[i].yax + 21);
					glVertex2f(bird[i].xax + 57, bird[i].yax + 16);
					glVertex2f(bird[i].xax + 33, bird[i].yax + 10);
					glEnd();
				}
			}

			//beak
			{
				//black frame
				{
					glLineWidth(4);
					glColor3f(0, 0, 0);
					glBegin(GL_LINE_LOOP);
					glVertex2f(bird[i].xax - 93, bird[i].yax + 21);
					glVertex2f(bird[i].xax - 70, bird[i].yax + 22);
					glVertex2f(bird[i].xax - 62, bird[i].yax + 14);
					glEnd();


					glBegin(GL_LINE_LOOP);
					glVertex2f(bird[i].xax - 84, bird[i].yax + 5);
					glVertex2f(bird[i].xax - 73, bird[i].yax + 21);
					glVertex2f(bird[i].xax - 62, bird[i].yax + 14);
					glEnd();
				}

				//beak
				{
					glColor3ub(234, 189, 13);
					glBegin(GL_POLYGON);
					glVertex2f(bird[i].xax - 93, bird[i].yax + 21);
					glVertex2f(bird[i].xax - 70, bird[i].yax + 22);
					glVertex2f(bird[i].xax - 62, bird[i].yax + 14);
					glEnd();

					glBegin(GL_POLYGON);
					glVertex2f(bird[i].xax - 84, bird[i].yax + 5);
					glVertex2f(bird[i].xax - 73, bird[i].yax + 21);
					glVertex2f(bird[i].xax - 62, bird[i].yax + 14);
					glEnd();
				}
			}
		}
}

void dispanimac()
{
	if (screen == 3)
	{
		//box for score and second
		{
			//frame
			{
				glColor4f(0, 0, 0, 0.5);
				glBegin(GL_LINE_LOOP);
				glVertex2f(-200, 350);
				glVertex2f(185, 350);
				glVertex2f(185, 302);
				glVertex2f(-200, 302);
				glEnd();
			}

			glColor4ub(120, 100, 150, 50);
			glBegin(GL_POLYGON);
			glVertex2f(-198, 350);
			glVertex2f(183, 350);

			glVertex2f(183, 305);
			glVertex2f(-198, 305);
			glEnd();
		}

		//info
		{
			glColor3f(0, 0, 0);
			if (remaintime - 2 - timeCounter <= 0) //when finished
				vprint(-120, 323, GLUT_BITMAP_9_BY_15, "* Time: 00.00");
			else
				vprint(-120, 323, GLUT_BITMAP_9_BY_15, "- Time: %.0f.%02.0f", remaintime - 2 - timeCounter, fabs(0 - (splitsec - timeCounter * 100)));

			vprint(+40, 323, GLUT_BITMAP_9_BY_15, "score: %d -", score);
		}

		//box for name label
		{
			glColor3ub(92, 254, 126);
			glRectf(-600, 350, -450, 295);
			glColor3f(0, 0, 0);
			vprint(-575, 335, GLUT_BITMAP_9_BY_15, "HOMEWORK #2");
			vprint(-540, 318, GLUT_BITMAP_9_BY_15, "BY");
			vprint(-595, 302, GLUT_BITMAP_9_BY_15, "Ahmet Oguz Ergin");

			//frame
			{
				glLineWidth(2);
				glColor3f(0, 0, 0);
				glBegin(GL_LINE_LOOP);
				glVertex2f(-600, 350);
				glVertex2f(-449, 350);
				glVertex2f(-449, 294);
				glVertex2f(-600, 294);
				glEnd();
			}
		}
	}
}

void dispSling(sling_t s)
{
	if (screen == 3)
	{
		//bottom stick
		{
			//white frame
			{
				glColor3f(1, 1, 1);

				circle(s.x, -325, 24);

				glLineWidth(5);
				glBegin(GL_LINE_LOOP);
				glVertex2f(s.x + 22, -210);
				glVertex2f(s.x + 22, -325);
				glVertex2f(s.x + -22, -325);
				glVertex2f(s.x + -22, -210);
				glEnd();

				glRectf(s.x + 22, -325, s.x - 22, -210);
			}

			glColor3ub(139, 85, 52);
			glRectf(s.x + 22, -325, s.x - 22, -210);

			//circle
			circle(s.x, -325, 22);
		}

		//right stick
		{
			//white frames
			{
				glColor3f(1, 1, 1);

				circle_wire(s.x + 111, s.y + 74, 16);

				glLineWidth(5);
				glBegin(GL_LINE_LOOP);
				glVertex2f(s.x, s.y);
				glVertex2f(s.x + 22, s.y - 22);
				glVertex2f(s.x + 122, s.y + 63);
				glVertex2f(s.x + 100, s.y + 85);
				glEnd();
			}

			glColor3ub(139, 85, 52);
			glBegin(GL_QUADS);
			glVertex2f(s.x, s.y);
			glVertex2f(s.x + 22, s.y - 22);
			glVertex2f(s.x + 122, s.y + 63);
			glVertex2f(s.x + 100, s.y + 85);
			glEnd();

			//circle
			circle(s.x + 111, s.y + 74, 15);
		}

		//left stick
		{
			//white frames
			{
				glColor3f(1, 1, 1);

				circle_wire(s.x - 111, s.y + 74, 16);

				glBegin(GL_LINE_LOOP);
				glVertex2f(s.x, s.y);
				glVertex2f(s.x - 22, s.y - 22);
				glVertex2f(s.x + -122, s.y + 63);
				glVertex2f(s.x + -100, s.y + 85);
				glEnd();
			}

			//rectangle
			glColor3ub(139, 85, 52);
			glBegin(GL_QUADS);
			glVertex2f(s.x, s.y);
			glVertex2f(s.x - 22, s.y - 22);
			glVertex2f(s.x + -122, s.y + 63);
			glVertex2f(s.x + -100, s.y + 85);
			glEnd();

			//circle
			circle(s.x - 111, s.y + 74, 15);
		}

		//lines
		{
			//bottom
			{
				glLineWidth(4);

				glColor3ub(44, 75, 170);
				glBegin(GL_LINES);

				glVertex2f(s.x - 24, s.y - 25);
				glVertex2f(s.x, s.y);

				glVertex2f(s.x, s.y);
				glVertex2f(s.x + 25, s.y - 25);

				glVertex2f(s.x - 25, s.y - 25);
				glVertex2f(s.x + 25, s.y - 25);

				glVertex2f(s.x - 25, s.y - 32);
				glVertex2f(s.x + 25, s.y - 32);

				glVertex2f(s.x - 25, s.y - 40);
				glVertex2f(s.x + 25, s.y - 40);
			}

			//left up
			{
				glVertex2f(s.x - 111, s.y + 53);
				glVertex2f(s.x - 89, s.y + 75);

				glVertex2f(s.x - 107, s.y + 50);
				glVertex2f(s.x - 84, s.y + 73);

				glVertex2f(s.x - 103, s.y + 47);
				glVertex2f(s.x - 79, s.y + 70);
			}

			//right up
			{
				glVertex2f(s.x + 111, s.y + 53);
				glVertex2f(s.x + 89, s.y + 75);

				glVertex2f(s.x + 107, s.y + 50);
				glVertex2f(s.x + 84, s.y + 73);

				glVertex2f(s.x + 103, s.y + 47);
				glVertex2f(s.x + 79, s.y + 70);

				glEnd();
			}

			//rope
			{
				glBegin(GL_LINES);

				if (s.shooted)
				{
					glVertex2f(s.x - 84, s.y + 73);
					glVertex2f(s.x - 30, s.y + 80);

					glVertex2f(s.x + 84, s.y + 73);
					glVertex2f(s.x + 30, s.y + 80);
				}

				else
				{
					glVertex2f(s.x - 84, s.y + 73);
					glVertex2f(s.x - 30, s.y + 35);

					glVertex2f(s.x + 84, s.y + 73);
					glVertex2f(s.x + 30, s.y + 35);
				}

				glEnd();
			}

			//between ropes
			{
				if (s.shooted == true)
					glRectf(s.x - 30, s.y + 95, s.x + 30, s.y + 65);
				else
					glRectf(s.x - 30, s.y + 50, s.x + 30, s.y + 20);
			}
		}
	}
}

void dispStone(sling_t s, sling_t sling)
{
	if (screen == 3)
	{
		if (!s.shooted)
			stone.x = sling.x;

		glLineWidth(5);
		//up
		{
			//frame
			{
				glColor3ub(147, 137, 133);
				glBegin(GL_LINE_LOOP);
				glVertex2f(s.x - 19, s.y + 6);
				glVertex2f(s.x + 3, s.y + 26);
				glVertex2f(s.x + 15, s.y + 13);
				glVertex2f(s.x + 7, s.y + 8);
				glVertex2f(s.x - 8, s.y + 2);
				glEnd();
			}

			glColor3ub(147, 137, 133);
			glBegin(GL_POLYGON);
			glVertex2f(s.x - 19, s.y + 6);
			glVertex2f(s.x + 3, s.y + 26);
			glVertex2f(s.x + 15, s.y + 13);
			glVertex2f(s.x + 7, s.y + 8);
			glVertex2f(s.x - 8, s.y + 2);
			glEnd();
		}

		//right
		{
			//frame
			{
				glColor3ub(90, 80, 80);
				glBegin(GL_LINE_LOOP);
				glVertex2f(s.x + 14, s.y + 15);
				glVertex2f(s.x + 20, s.y + 2);
				glVertex2f(s.x + 15, s.y - 13);
				glVertex2f(s.x + 5, s.y - 6);
				glVertex2f(s.x + 8, s.y + 8);
				glEnd();
			}

			glBegin(GL_POLYGON);
			glColor3ub(90, 80, 80);
			glVertex2f(s.x + 14, s.y + 15);
			glVertex2f(s.x + 20, s.y + 2);
			glVertex2f(s.x + 15, s.y - 13);
			glVertex2f(s.x + 5, s.y - 6);
			glVertex2f(s.x + 8, s.y + 8);
			glEnd();
		}

		//bottom
		{
			//frame
			{
				glColor3ub(70, 60, 60);
				glBegin(GL_LINE_LOOP);
				glVertex2f(s.x + 6, s.y - 5);
				glVertex2f(s.x - 1, s.y - 8);
				glVertex2f(s.x - 14, s.y - 14);
				glVertex2f(s.x + 1, s.y - 20);
				glVertex2f(s.x + 15, s.y - 12);
				glEnd();
			}

			glColor3ub(70, 60, 60);
			glBegin(GL_POLYGON);
			glVertex2f(s.x + 6, s.y - 5);
			glVertex2f(s.x - 1, s.y - 8);
			glVertex2f(s.x - 14, s.y - 14);
			glVertex2f(s.x + 1, s.y - 20);
			glVertex2f(s.x + 15, s.y - 12);
			glEnd();
		}

		//left
		{
			//frame
			{
				glColor3ub(129, 124, 120);
				glBegin(GL_LINE_LOOP);
				glVertex2f(s.x - 18, s.y + 5);
				glVertex2f(s.x - 21, s.y - 1);
				glVertex2f(s.x - 14, s.y - 14);
				glVertex2f(s.x - 4, s.y - 9);
				glVertex2f(s.x - 3, s.y - 1);
				glVertex2f(s.x - 7, s.y + 3);
				glEnd();
			}

			glColor3ub(129, 124, 120);
			glBegin(GL_POLYGON);
			glVertex2f(s.x - 18, s.y + 5);
			glVertex2f(s.x - 21, s.y - 1);
			glVertex2f(s.x - 14, s.y - 14);
			glVertex2f(s.x - 4, s.y - 9);
			glVertex2f(s.x - 3, s.y - 1);
			glVertex2f(s.x - 7, s.y + 3);
			glEnd();
		}

		//inside
		{
			//frame
			{
				glColor3ub(101, 95, 95);
				glBegin(GL_LINE_LOOP);
				glVertex2f(s.x - 7, s.y + 3);
				glVertex2f(s.x - 2, s.y - 3);
				glVertex2f(s.x - 4, s.y - 9);
				glVertex2f(s.x + 6, s.y - 5);
				glVertex2f(s.x + 8, s.y + 8);
				glVertex2f(s.x - 7, s.y + 2);
				glEnd();
			}

			glColor3ub(101, 95, 95);
			glBegin(GL_POLYGON);
			glVertex2f(s.x - 7, s.y + 3);
			glVertex2f(s.x - 2, s.y - 3);
			glVertex2f(s.x - 4, s.y - 9);
			glVertex2f(s.x + 6, s.y - 5);
			glVertex2f(s.x + 8, s.y + 8);
			glVertex2f(s.x - 7, s.y + 2);
			glEnd();
		}
	}
}

void dispLastsc()
{
	if (screen == 4)
	{
		//display screen
		{
			glBegin(GL_QUADS);
			glColor3ub(81, 228, 50);
			glVertex2f(-600, 350);
			glVertex2f(+600, 350);

			glColor3ub(60, 92, 190);
			glVertex2f(+600, -350);
			glVertex2f(-600, -350);
			glEnd();
		}

		//display message
		{
			glLineWidth(6);
			glColor3ub(180, 100, 200);
			vprint2(-494, 136, 1, "! Game Over !");

			glLineWidth(5);
			glColor3ub(154, 80, 186);
			vprint2(-496, 134, 1, "! Game Over !");

			glLineWidth(4);
			glColor3ub(142, 68, 173);
			vprint2(-498, 132, 1, "! Game Over !");

			glLineWidth(3);
			glColor3ub(115, 55, 140);
			vprint2(-500, 130, 1, "! Game Over !");

			glColor3f(1, 1, 0);
			glLineWidth(3);
			vprint2(-121, 28, 0.3, "ENTER", score);

			glColor3f(1, 0, 0);
			glLineWidth(3);
			vprint2(-123, 27, 0.3, "ENTER", score);

			glColor3ub(0, 0, 0);
			glLineWidth(3);
			vprint2(-280, 25, 0.3, "...Press ENTER To Play Again...", score);

			glColor3ub(0, 0, 0);
			glLineWidth(3);
			vprint2(-190, -55, 0.3, "* Your Score: %d *", score);

			if (besthighscr > score)
			{
				glColor3ub(105, 45, 130);
				glLineWidth(4);
				vprint2(-250, -130, 0.3, "--> High score: %d <--", besthighscr);


				glColor3ub(75, 15, 100);
				glLineWidth(3);
				vprint2(-252, -132, 0.3, "--> High score: %d <--", besthighscr);
			}
			else
			{
				//box
				{
					glColor3ub(255, 65, 70);
					glBegin(GL_QUADS);
					glVertex2f(-270, -86);
					glVertex2f(270, -86);

					glColor3ub(245, 149, 52);
					glVertex2f(270, -171);
					glVertex2f(-270, -171);
					glEnd();
				}
				//frame
				{
					glColor3ub(255, 255, 0);
					glBegin(GL_LINE_LOOP);
					glVertex2f(-270, -86);
					glVertex2f(270, -86);

					glColor3ub(0, 0, 0);
					glVertex2f(270, -171);
					glVertex2f(-270, -171);
					glEnd();
				}

				glColor3ub(0, 0, 0);
				glLineWidth(3);
				vprint2(-220, -140, 0.3, "NEW High score: % d ", score);

				vprint2(-210, -230, 0.3, "* Old High score: %d *", besthighscr);
			}
		}
	}
}

void blackscrn()
{
	glColor3f(0, 0, 0);
	glRectf(-1000, +350, -600, -350);
	glRectf(+1000, +350, +600, -350);
	glRectf(-600, +350, +600, +4000);
}

//
// To display onto window using OpenGL commands
//
void display() {
	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	//display background big rectangle
	dispbackrec();

	//display the menu
	dismenu(difficultrecy);

	//display the birds
	dispBird(bird);

	//display the information while animation active
	dispanimac();

	//display sling
	dispSling(sling);

	//display stone
	dispStone(stone, sling);

	//display last screen
	dispLastsc();

	//display black background if user make it fullscreen
	blackscrn();

	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	//press s to skip timer
	if (screen == 2 && key == 115)
	{
		startgameinit();
		screen = 3;
	}

	//get the enter to countdown 
	if (screen == 1 && key == 13)
		screen = 2;

	//activate shooting
	if (key == 32 && screen == 3)
	{
		if (!stone.shooted)
		{
			stone.y += 45;
			stone.shooted = !stone.shooted;
			sling.shooted = !sling.shooted;
		}
	}

	//turn back
	if (screen == 4 && key == 13)
	{
		screen = 1;
		inittomenu1();
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
	}

	if (screen == 1)
	{
		if (difficulty < 3 && difficulty >= 1 && down)
			difficulty++;
		else  if (difficulty <= 3 && difficulty > 1 && up)
			difficulty--;
		inittomenu1();
	}


	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// GLUT to OpenGL coordinate conversion:
	x = x - winWidth / 2;
	y = winHeight / 2 - y;

	if (stat == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
		printf("%d , %d\n", x, y);

	if (stat == GLUT_DOWN && button == GLUT_RIGHT_BUTTON)
		printf("glVertex2f(%d , %d);\n", x, y);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.

	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(timerperi, onTimer, 0);
	// Write your codes here.

	//start action with birds
	if (screen == 3)
	{
		//action in given time
		if (remaintime - timeCounter != 0)
		{
			//display first bird
			if (splitsec == 0 && timeCounter == 0)
			{
				bird[0].r = rand() % ((255 + 1) - 0) + 0;
				bird[0].g = rand() % ((255 + 1) - 0) + 0;
				bird[0].b = rand() % ((255 + 1) - 0) + 0;
				bird[0].yax = rand() % ((290 + 1) - -50) + -50;
				bird[0].firstyax = bird[0].yax;
				bird[0].active = true;
				j++;
			}

			//increase time
			splitsec += 2;

			//increase time
			if (int(splitsec) == splitsec && int(splitsec) % 100 == 0)
				timeCounter++;

			//display bird in every 1 second
			if (difficulty == 3)
				if (int(timeCounter) % 1 == 0 && j <= 11 && int(splitsec) == splitsec && int(splitsec) % 100 == 0)
				{
					bird[j].r = rand() % ((255 + 1) - 0) + 0;
					bird[j].g = rand() % ((255 + 1) - 0) + 0;
					bird[j].b = rand() % ((255 + 1) - 0) + 0;
					bird[j].yax = rand() % ((290 + 1) - -50) + -50;
					bird[j].firstyax = bird[j].yax;
					bird[j].active = true;
					j++;
				}
			if (difficulty == 2)
				if (int(timeCounter) % 1 == 0 && j <= 15 && int(splitsec) == splitsec && int(splitsec) % 100 == 0)
				{
					bird[j].r = rand() % ((255 + 1) - 0) + 0;
					bird[j].g = rand() % ((255 + 1) - 0) + 0;
					bird[j].b = rand() % ((255 + 1) - 0) + 0;
					bird[j].yax = rand() % ((290 + 1) - -50) + -50;
					bird[j].firstyax = bird[j].yax;
					bird[j].active = true;
					j++;
				}
			if (difficulty == 1)
				if (int(timeCounter) % 1 == 0 && j <= 19 && int(splitsec) == splitsec && int(splitsec) % 100 == 0)
				{
					bird[j].r = rand() % ((255 + 1) - 0) + 0;
					bird[j].g = rand() % ((255 + 1) - 0) + 0;
					bird[j].b = rand() % ((255 + 1) - 0) + 0;
					bird[j].yax = rand() % ((290 + 1) - -50) + -50;
					bird[j].firstyax = bird[j].yax;
					bird[j].active = true;
					j++;
				}

			//move to left
			for (int i = 0; i < 50; i++)
				if (bird[i].active)
				{
					if (difficulty == 3)
						bird[i].xax -= 10;
					if (difficulty == 2)
						bird[i].xax -= 7;
					if (difficulty == 1)
						bird[i].xax -= 6;
				}

			//go up or down
			for (int i = 0; i < 50; i++)
				if (bird[i].active)
				{
					//go down
					if (bird[i].yax >= bird[i].firstyax - 45 && bird[i].updown == 1)
					{
						bird[i].yax -= 1.5;

						if (bird[i].yax == bird[i].firstyax - 45)
							bird[i].updown = 2;
					}

					//go down
					if (bird[i].yax <= bird[i].firstyax + 45 && bird[i].updown == 2)
					{
						bird[i].yax += 1.5;

						if (bird[i].yax == bird[i].firstyax + 45)
							bird[i].updown = 1;
					}
				}
		}

		//change the screen when time is out
		else
			screen = 4;

		//move sling
		if (right && sling.x <= 400)
			sling.x += 7;
		if (left && sling.x >= -400)
			sling.x -= 7;

		//stone will go through
		{
			if (stone.shooted)
				stone.y += 8;

			//exceed the screen
			if (stone.y >= 360)
			{
				stone.shooted = !stone.shooted;
				sling.shooted = !sling.shooted;

				stone.y = -175;
				stone.x = sling.x;
			}
		}

		//if bird is shooted 
		for (int i = 0; i < 20; i++)
			if (((stone.x + 30 <= bird[i].xax + 100 && stone.x + 30 >= bird[i].xax - 100) || (stone.x <= bird[i].xax + 100 && stone.x >= bird[i].xax - 100) || (stone.x - 30 <= bird[i].xax + 100 && stone.x - 30 >= bird[i].xax - 100)) && ((stone.y + 30 <= bird[i].yax + 60 && stone.y + 30 >= bird[i].yax - 50) || (stone.y - 30 <= bird[i].yax + 60 && stone.y - 30 >= bird[i].yax - 50)) && bird[i].active && stone.shooted)
			{
				bird[i].active = false;
				score++;
			}
	}

	//countdown for second menu screen
	else if (screen == 2)
	{
		timeCounter++;
		if (5 - timeCounter == 0)
		{
			startgameinit();
			screen = 3;
		}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()
}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	srand(time(NULL));

	inittomenu1();
}

void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(150, 75);
	glutCreateWindow("Bird Hunt by ""Ahmet Oðuz Ergin """);

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}