#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

class Engine
{
public:
	double getTime(double &curTime);
	double getFPS(double dt, double curFPS, double numFrames);
};

class Object
{
public:
	int red, green, blue, alpha, xPos, yPos, width, height, depth;
	bool render;
	string type;
	void collision_react();
	void collide_blue();
};

class Player
{
public:
	double xPos, yPos;
	int width, height;
	double xVel, yVel;
	bool can_jump;
	void draw();
	void move_x(vector<Object> obj, double dt, bool left, bool right);
	void move_y(vector<Object> obj, double dt);

	int collide_x(vector<Object> obj, double dt);
	int collide_y(vector<Object> obj, double dt);

	void gravity(double dt);
	void jump(vector<Object> obj);
	Player(int X, int Y);
};

Player::Player(int X, int Y)
{
	xPos = X; yPos = Y; width = 20; height = 20, xVel = 0, yVel = 0, can_jump = false;
}

void Player::gravity(double dt)
{
	yVel = yVel + (dt*6500);
}

void Player::draw()
{
	glColor4ub(0, 255, 0, 255);
	glBegin(GL_QUADS);
	glVertex3f(xPos, yPos, 1);
	glVertex3f(xPos + width, yPos, 1);
	glVertex3f(xPos + width, yPos + height, 1);
	glVertex3f(xPos, yPos + height, 1);
	glEnd();
}

void Player::move_x(vector<Object> obj, double dt, bool left, bool right)
{
	xVel = 0;
	if (left)
		xVel -= 500;
	if (right)
		xVel += 500;
	xPos = xPos + (dt * xVel);

	int collision = collide_x(obj, dt);

	if (collision != -1)
	{
		if (xVel > 0)
		{
			xPos -= (xPos + width - collision + 0.1);
		}
		else if (xVel < 0)
		{
			xPos += (collision + 25 - xPos + 0.1);
		}
		xVel = 0;
	}
}

void Player::move_y(vector<Object> obj, double dt)
{
	yPos = yPos + (dt * yVel);

	can_jump = false;

	int collision = collide_y(obj, dt);
	if (collision != -1)
	{
		if (yVel > 0)
		{
			can_jump = true;
			yPos -= (yPos + height - collision + 0.1);
		}
		else if (yVel < 0)
		{
			yPos += (collision + 25 - yPos +0.1);
		}
		yVel = 0;
	}
}

int Player::collide_x(vector<Object> obj, double dt)
{
	for (vector<Object>::iterator it = obj.begin(); it != obj.end(); it++)
	{
		if (((yPos + height) >= it->yPos) && (yPos <= (it->yPos + it->height)))
		{
			if ((xVel > 0) && ((xPos + width) >= it->xPos))
			{
				if ((xPos + width - (dt*xVel)) <= (it->xPos))
				{
					return it->xPos;
				}
			}
			else if ((xVel < 0) && ((xPos <= (it->xPos + it->width))))
			{
				if((xPos - (dt*xVel)) >= (it->xPos + it->width))
				{
					return it->xPos;
				}
			}
		}
	}
	return -1;
}

int Player::collide_y(vector<Object> obj, double dt)
{
	for (vector<Object>::iterator it = obj.begin(); it != obj.end(); it++)
	{
		if (it->render)
		{
			if (((xPos + width) >= it->xPos) && (xPos <= (it->xPos + it->width)))
			{
				if ((yVel > 0) && ((yPos + height) >= it->yPos))
				{
					if ((yPos + height - (dt*yVel)) <= (it->yPos))
					{
						return it->yPos;
					}
				}
				else if ((yVel < 0) && ((yPos <= (it->yPos + it->height))))
				{
					if ((yPos - (dt*yVel)) >= (it->yPos + it->height))
					{
						return it->yPos;
					}
				}
			}
		}
	}
	return -1;
}

void Player::jump(vector<Object> obj)
{
	yVel = -750;
}

double Engine::getTime(double &curTime)
{
	double newTime = clock();
	double dt = newTime - curTime;
	curTime = newTime;
	return (dt/CLOCKS_PER_SEC);
}

double Engine::getFPS(double dt, double curFPS, double numFrames)
{
	double aveDT = (1/curFPS);
	aveDT = (aveDT*numFrames);
	return (1/((aveDT+dt)/(numFrames+1)));
}

int initLevel(vector<Object> &objects)
{
	ifstream inFile("/home/mason/Desktop/test.txt");
	string temp;
	int number = 0;
	int space = 0, space2 = 0;
	string data;
	if(!inFile)
		cout << "Error" << endl;

	Object obj;

	while(getline(inFile, temp))
	{
		cout << temp << endl;

		cout << "number: " << number << endl;

		space = space2 = 0;
		space2 = temp.find(" ", space);
		obj.type = temp.substr(space, space2-space);
		cout << obj.type << endl;

		space = space2;
		space2 = temp.find(" ", space+1);
		obj.red = atoi(temp.substr(space, space2-space).c_str());
		cout << obj.red << endl;

		space = space2;
		space2 = temp.find(" ", space+1);
		obj.green = atoi(temp.substr(space, space2-space).c_str());
		cout << obj.green << endl;

		space = space2;
		space2 = temp.find(" ", space+1);
		obj.blue = atoi(temp.substr(space, space2-space).c_str());
		cout << obj.blue << endl;

		space = space2;
		space2 = temp.find(" ", space+1);
		obj.alpha = atoi(temp.substr(space, space2-space).c_str());
		cout << obj.alpha << endl;

		space = space2;
		space2 = temp.find(" ", space+1);
		obj.xPos = atoi(temp.substr(space, space2-space).c_str());
		cout << obj.xPos << endl;

		space = space2;
		space2 = temp.find(" ", space+1);
		obj.yPos = atoi(temp.substr(space, space2-space).c_str());
		cout << obj.yPos << endl;

		space = space2;
		space2 = temp.find(" ", space+1);
		obj.width = atoi(temp.substr(space, space2-space).c_str());
		cout << obj.width << endl;

		space = space2;
		space2 = temp.find(" ", space+1);
		obj.height = atoi(temp.substr(space, space2-space).c_str());
		cout << obj.height << endl;

		space = space2;
		obj.depth = atoi(temp.substr(space).c_str());
		cout << obj.depth << endl;

		obj.render = true;

		objects.push_back(obj);
		number++;
	}
	inFile.close();
	return number;
}

void drawObject(Object obj)
{
	if (obj.render)
	{
		glColor4ub(obj.red, obj.green, obj.blue, obj.alpha);
		glBegin(GL_QUADS);
		glVertex3f(obj.xPos, obj.yPos, (obj.depth));
		glVertex3f(obj.xPos + obj.width, obj.yPos, (obj.depth));
		glVertex3f(obj.xPos + obj.width, obj.yPos + obj.height, (obj.depth));
		glVertex3f(obj.xPos, obj.yPos + obj.height, (obj.depth));
		glEnd();
	}
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 32 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	SDL_WM_SetCaption( "Game", NULL );
	SDL_SetVideoMode( 900, 600, 32, SDL_OPENGL );

	glClearColor(1, 1, 1, 1);
	glViewport(0, 0, 900, 600);

	glShadeModel(GL_SMOOTH);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);

	bool isRunning = true;

	SDL_Event event;
	Engine engine;
	double dt = 0;
	double curTime = clock();
	double fps;
	bool left = false, right = false;
	bool up = false;
	int numFrames = 1;
	Player player(200, 100);

	vector<Object> objects;
	initLevel(objects);

	while (isRunning)
	{
//EVENTS
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				isRunning = false;
			}
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_LEFT:
					left = true;
					break;
				case SDLK_RIGHT:
					right = true;
					break;
				case SDLK_UP:
					up = true;
					break;
				default:
					break;
				}
			}
			if (event.type == SDL_KEYUP)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_LEFT:
					left = false;
					break;
				case SDLK_RIGHT:
					right = false;
					break;
				case SDLK_UP:
					up = false;
					break;
				default:
					break;
				}
			}
		}
//LOGIC
		dt = engine.getTime(curTime);

		fps = 1/dt;

//		fps = engine.getFPS(dt, fps, numFrames);

		cout << fps << endl;

		if (up && player.can_jump)
		{
			player.jump(objects);
		}

		player.gravity(dt);

		player.move_x(objects, dt, left, right);
		player.move_y(objects, dt);


//RENDERING
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		glOrtho(0, 900, 600, 0, -1, 1);

		for (vector<Object>::iterator it = objects.begin(); it != objects.end(); it++)
		{
			drawObject(*it);
		}

		player.draw();

		glPopMatrix();
		SDL_GL_SwapBuffers();
		SDL_Delay(1);

		numFrames++;
	}

	SDL_Quit();

	return 0;
}
