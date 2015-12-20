#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

class Object
{
public:
	int red, green, blue, alpha, xPos, yPos, width, height, depth;
	string type;
	Object(string TYPE, int RED, int GREEN, int BLUE, int ALPHA, int X, int Y, int WIDTH, int HEIGHT, int DEPTH);
};

Object::Object(string TYPE, int RED, int GREEN, int BLUE, int ALPHA, int X, int Y, int WIDTH, int HEIGHT, int DEPTH)
{
	type = TYPE; red = RED; blue = BLUE; green = GREEN; alpha = ALPHA; xPos = X; yPos = Y; width = WIDTH; height = HEIGHT, depth = DEPTH;
}

string toString(int number)
{
	stringstream convert;
	convert << number;
	return convert.str();
}

void placeObject(vector<Object>& objects, int x, int y, int& num, string current)
{
	int red, green, blue, numObjects = 0;

	x = x - (x%25);
	y = y - (y%25);

	cout << "X rounded: " << x << ", Y rounded: " << y << endl;

	if (current == "block")
	{
		red = green = blue = 0;
	}
	else if (current == "trap_door")
	{
		red = green = 0;
		blue = 255;
	}
	else if (current == "fire")
	{
		red = 255;
		green = blue = 0;
	}

	for (vector<Object>::iterator it = objects.begin() + 1; it != objects.end(); it++)
	{
		if ((x == it->xPos) && (y == it->yPos))
		{
			if (it->type == current)
				return;
			(it->depth)--;
			numObjects++;
		}
	}

	Object temp(current, red, green, blue, 255, x, y, 25, 25, 0);
	objects.push_back(temp);

	num++;
}

void drawObject(Object obj)
{
	glColor4ub(obj.red, obj.green, obj.blue, obj.alpha);
	glBegin(GL_QUADS);
	glVertex3f(obj.xPos, obj.yPos, (obj.depth));
	glVertex3f(obj.xPos + obj.width, obj.yPos, (obj.depth));
	glVertex3f(obj.xPos + obj.width, obj.yPos + obj.height, (obj.depth));
	glVertex3f(obj.xPos, obj.yPos + obj.height, (obj.depth));
	glEnd();
}

void printFile(vector<Object> obj, string filePath)
{
	ofstream outFile;

	outFile.open(filePath.c_str());

	string temp = "";

	string test = "";

	for (vector<Object>::iterator it = obj.begin(); it != obj.end(); it++)
	{
		temp = it->type + " " + toString(it->red) + " " + toString(it->green) + " " + toString(it->blue) + " " + toString(it->alpha) + " " + toString(it->xPos) + " " + toString(it->yPos) + " " + toString(it->width) + " " + toString(it->height) + " " + toString(it->depth);
		if ((it + 1) != obj.end())
			temp += "\n";
		outFile << temp;
	}
	outFile.close();
}

void removeObj(vector<Object>& obj, int x, int y, int& number)
{

	x = x - (x%25);
	y = y - (y%25);

	for (vector<Object>::iterator it = obj.begin() + 1; it != obj.end(); it++)
	{
		if ((it->xPos == x) && (it->yPos == y))
		{
			if (it->depth == 0)
			{
				it--;
				obj.erase(it+1);
				number--;
				continue;
			}
			it->depth = (it->depth + 1);
		}
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

	string filePath;

	cout << "File Path: ";
	getline(cin, filePath);

	if (filePath == "")
	{
		filePath = "/home/mason/Desktop/test.txt";
	}

	SDL_WM_SetCaption( "Editor", NULL );
	SDL_SetVideoMode( 900, 600, 32, SDL_OPENGL );

	glClearColor(1, 1, 1, 1);
	glViewport(0, 0, 900, 600);

	glShadeModel(GL_SMOOTH);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);

	bool isRunning = true;

	SDL_Event event;

	int xClick = 0, yClick = 0;
	int xHover = 0, yHover = 0;
	bool place = false, remove = false;
	int numObj = 0;
	string curObj = "block";

	vector<Object> objects;

	Object temp("block", 0, 0, 0, 175, xHover, yHover, 25, 25, 1);
	objects.push_back(temp);

	while (isRunning)
	{
//EVENTS
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				isRunning = false;
			}
			if (event.type == SDL_MOUSEBUTTONUP)
			{
				SDL_GetMouseState(&xClick, &yClick);
				cout << "X: " << xClick << ", Y: " << yClick << endl;
				if (event.button.button == SDL_BUTTON_LEFT)
					place = true;
				else if (event.button.button == SDL_BUTTON_RIGHT)
					remove = true;
			}
			if (event.type == SDL_KEYUP)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_1:
					curObj = "block";
					objects[0].type = "block";
					objects[0].red = objects[0].green = objects[0].blue = 0;
					break;
				case SDLK_2:
					curObj = "trap_door";
					objects[0].type = "trap_door";
					objects[0].red = objects[0].green = 0;
					objects[0].blue = 255;
					break;
				case SDLK_3:
					curObj = "fire";
					objects[0].type = "fire";
					objects[0].red = 255;
					objects[0].green = objects[0].blue = 0;
					break;
				default:
					break;
				}
			}
		}
//LOGIC
		if(place)
		{
			placeObject(objects, xClick, yClick, numObj, curObj);
			place = false;
		}
		else if (remove)
		{
			removeObj(objects, xClick, yClick, numObj);
			remove = false;
		}

		SDL_GetMouseState(&xHover, &yHover);
		xHover = xHover - (xHover%25);
		yHover = yHover - (yHover%25);
		objects[0].xPos = xHover;
		objects[0].yPos = yHover;

//RENDERING
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		glOrtho(0, 900, 600, 0, -1, 1);

		for (vector<Object>::iterator it = objects.begin(); it != objects.end(); it++)
		{
			drawObject(*it);
		}

		glPopMatrix();
		SDL_GL_SwapBuffers();
		SDL_Delay(1);
	}

	SDL_Quit();

	objects.erase(objects.begin());

	printFile(objects, filePath);

	return 0;
}
