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
	double setTimes(double &prevTime, double &curTime);
	double getFPS(double prevTime, double curTime);
};

class Object
{
public:
	int id, red, green, blue, alpha, xPos, yPos, width, height, depth;
	bool visible;
	string type;
	void collision_react();
};

class Player
{
public:
	double xPos, yPos;
	int width, height;
	double xVel, yVel;
	bool can_jump;
	bool alive = true;
	void draw();
	vector<Object> move_x(vector<Object> &obj, double dt, bool left, bool right);
	vector<Object> move_y(vector<Object> &obj, double dt);

	int collide_x(vector<Object> &obj, double dt);
	int collide_y(vector<Object> &obj, double dt);
	void collision_react(Object obj);

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
	yVel = yVel + (dt*500000);
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

vector<Object> Player::move_x(vector<Object> &obj, double dt, bool left, bool right)
{
	xVel = 0;
	if (left)
		xVel -= 4000;
	if (right)
		xVel += 4000;
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
	return obj;
}

vector<Object> Player::move_y(vector<Object> &obj, double dt)
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
	return obj;
}

int Player::collide_x(vector<Object> &obj, double dt)
{
	int index = 0;
	for (vector<Object>::iterator it = obj.begin(); it != obj.end(); it++)
	{
		if (it->visible)
		{
			if (((yPos + height) >= it->yPos) && (yPos <= (it->yPos + it->height)))
			{
				if ((xVel > 0) && ((xPos + width) >= it->xPos))
				{
					if ((xPos + width - (dt*xVel)) <= (it->xPos))
					{
						collision_react(*it);
						obj[index].collision_react();
						return it->xPos;
					}
				}
				else if ((xVel < 0) && ((xPos <= (it->xPos + it->width))))
				{
					if((xPos - (dt*xVel)) >= (it->xPos + it->width))
					{
						collision_react(*it);
						obj[index].collision_react();
						return it->xPos;
					}
				}
			}
		}
		index++;
	}
	return -1;
}

int Player::collide_y(vector<Object> &obj, double dt)
{
	int index = 0;
	for (vector<Object>::iterator it = obj.begin(); it != obj.end(); it++)
	{
		if (it->visible)
		{
			if (((xPos + width) >= it->xPos) && (xPos <= (it->xPos + it->width)))
			{
				if ((yVel > 0) && ((yPos + height) >= it->yPos))
				{
					if ((yPos + height - (dt*yVel)) <= (it->yPos))
					{
						collision_react(*it);
						obj[index].collision_react();
						return it->yPos;
					}
				}
				else if ((yVel < 0) && ((yPos <= (it->yPos + it->height))))
				{
					if ((yPos - (dt*yVel)) >= (it->yPos + it->height))
					{
						collision_react(*it);
						obj[index].collision_react();
						return it->yPos;
					}
				}
			}
		}
		index++;
	}
	return -1;
}

void Player::jump(vector<Object> obj)
{
	yVel = -7500;
}

void Player::collision_react(Object obj)
{
	if(obj.type == "trap_door")
		return;
	else if(obj.type == "block")
		return;
	else if(obj.type == "fire")
	{
		alive = false;
		return;
	}
}

void Object::collision_react()
{
	if(type == "trap_door")
	{
		visible = false;
		return;
	}
	else if(type == "block")
	{
		return;
	}
	else if(type == "fire")
	{
		return;
	}
}

double Engine::setTimes(double &prevTime, double &curTime)
{
	prevTime = curTime;
	curTime = (((double)clock())/CLOCKS_PER_SEC);
	return (curTime-prevTime);
}

double Engine::getFPS(double prevTime, double curTime)
{
	return (1/(curTime-prevTime));
}

int initLevel(vector<Object> &objects, int argc, char** args)
{
	string level = "";
	if (argc > 1)
	{
		level = "levels/";
		level.append(args[1]);
	}
	else if (argc == 1)
	{
		level = "levels/test.txt";
	}

	ifstream inFile(level.c_str());

	string temp;
	int number = 0;
	int space = 0, space2 = 0;
	string data;

	Object obj;
	int id = 0;

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

		obj.visible = true;

		objects.push_back(obj);
		number++;
	}
	inFile.close();
	return number;
}

void drawObject(Object obj)
{
	if (obj.visible)
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
	double prevTime = (((double)clock())/CLOCKS_PER_SEC), curTime = (((double)clock())/CLOCKS_PER_SEC);
	double fps;
	bool left = false, right = false;
	bool up = false;
	int numFrames = 1;
	Player player(75, 550);

	vector<Object> objects;
	initLevel(objects, argc, argv);

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
		dt = engine.setTimes(prevTime, curTime);

		fps = engine.getFPS(prevTime, curTime);

		cout << "Alive? ";
		if (player.alive)
			cout << "Yes" << endl;
		else
			cout << "No" << endl;

		if (up && player.can_jump)
		{
			player.jump(objects);
		}

		player.gravity(dt);

		objects = player.move_x(objects, dt, left, right);
		objects = player.move_y(objects, dt);


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
