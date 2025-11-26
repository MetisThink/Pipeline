#pragma once

#define NOMNMAX        // problems with min and max if this is not included, macros for max and min
#include <Windows.h>
#include <string>
using namespace std;

#define WINDOW_GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))   // extract X and Y co-ordinates
#define WINDOW_GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

class Window
{
public:

	HWND hwnd;                     // handle - is like a pointer to the window we are going to create
	HINSTANCE hinstance;

	int width;             // width of the window
	int height;            // height of the window
	string name;           // name of window
	int mousex;            // position of mouse x
	int mousey;            // position of mouse y
	bool keys[256];          // 256 possible keys stored in an array
	bool mouseButtons[3];    // 3 mouse buttons

	void initialize(std::string window_name, int window_width, int window_height);

	void updateMouse(int x, int y)
	{
		mousex = x;
		mousey = y;
	}

	void processMessages();

};
