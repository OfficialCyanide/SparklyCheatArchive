#pragma once
#include "SDK.h"
#include <vector>
#include <string>
#define SPARKLY_VERSION "3.5.6"
#define DONATOR

enum class ItemType
{
	null,
	checkbox,
	slider,
	list,
	color,
	space,
	newline
};

// Structs to keep all the data of a menu item in one variable
struct MenuItem
{
	char name[30] = "Null";
	ItemType Type = ItemType::null;
	int width = 100;

	float *value = nullptr;
	float min, max, step;

	string *strValue = nullptr;
	vector<string> list = {};
	bool focused = false;

	DWORD *color = nullptr;
}; typedef vector<MenuItem> ItemList;

struct Tab
{
	char name[30] = "Null"; // Make sure we have defaults set
	vector<MenuItem> items;
};

struct Window
{
	char name[30] = "Null";
	int x = 200, y = 200;
	int w = 100, h = 100;
	bool dragging = false;
};

enum class e_mb
{
	LDOWN, LCLICK,
	RDOWN, RCLICK,
	NONE
};

class wTools
{
public:
	static void bToolbar(int x, int y, int w, int h);
	static void bBackground(int x, int y, int w, int h);
	static void bOutline(int x, int y, int w, int h);
	static void bTab(int x, int y, bool selected, const char* text);

	static int bCheckbox(MenuItem &item, int x, int y, bool draw);
	static int bSlider(MenuItem &item, int x, int y, bool draw);
	static int bList(MenuItem &item, int x, int y, bool draw);
	static void bListbox(MenuItem &item, int x, int y);
	static int bColor(MenuItem &item, int x, int y, bool draw);
	static void bColorbox(MenuItem &item, int x, int y);

	static void cToolbar(int x, int y, int w, int h);
	static void cBackground(int x, int y, int w, int h);
	static void cOutline(int x, int y, int w, int h);

	static int cCheckbox(MenuItem &item, int x, int y, bool draw);
	static int cSlider(MenuItem &item, int x, int y, bool draw);
	static int cList(MenuItem &item, int x, int y, bool draw);
	static void cListbox(MenuItem &item, int x, int y);
	static int cColor(MenuItem &item, int x, int y, bool draw);

	static void lToolbar(int x, int y, int w, int h);
	static void lBackground(int x, int y, int w, int h);
	static void drawNull(int x, int y, int w, int h);
	static void lTab(int x, int y, bool selected, const char* text);

	static int lCheckbox(MenuItem &item, int x, int y, bool draw);
	static int lSlider(MenuItem &item, int x, int y, bool draw);
	static int lList(MenuItem &item, int x, int y, bool draw);
	static int lColor(MenuItem &item, int x, int y, bool draw);

	static void dToolbar(int x, int y, int w, int h);
	static void dBackground(int x, int y, int w, int h);
	static void dOutline(int x, int y, int w, int h);

	static int dCheckbox(MenuItem &item, int x, int y, bool draw);
	static int dSlider(MenuItem &item, int x, int y, bool draw);
	static int dList(MenuItem &item, int x, int y, bool draw);
	static int dColor(MenuItem &item, int x, int y, bool draw);
};

struct WindowStyle
{
	int itemSpace;
	int spaceSize; // Determines how many pixels items are separated by when adding a space

	void(*drawToolbar)(int x, int y, int w, int h);
	void(*drawBackground)(int x, int y, int w, int h);
	void(*drawOutline)(int x, int y, int w, int h);
	void(*drawTab)(int x, int y, bool selected, const char* text);

	int(*drawCheckbox)(MenuItem &item, int x, int y, bool draw);
	int(*drawSlider)(MenuItem &item, int x, int y, bool draw);
	int(*drawList)(MenuItem &item, int x, int y, bool draw);
	void(*drawListbox)(MenuItem &item, int x, int y);
	int(*drawColor)(MenuItem &item, int x, int y, bool draw);
	void(*drawColorbox)(MenuItem &item, int x, int y);

	void drawWindow(int x, int y, int w, int h)
	{
		drawBackground(x, y, w, h); drawToolbar(x, y, w, h); drawOutline(x, y, w, h);
	}
};

class Menu
{
public:
	void paint();
	bool menuToggle = false;
	int mWidth = 600, mHeight = 300;
	POINT mPos = { 150, 150 };

	// An array of windows that are automatically rendered
	vector<Tab> tabList;
	int currentTab = 0;

	// ez drawing funcs
	int drawItem(MenuItem &item, int x, int y);
	void drawItems(Tab &tab);
	void openControlbox(MenuItem &item, int x, int y);
	void WaterMark(const char* name, int x, int y, bool background = true);

	string focus = "";
	bool canFocus(string name)
	{
		return focus == "" || focus == name;
	}

	bool mouseOver(int x, int y, int w, int h); // Easy way to get mouse input
	bool keyDown(int vKey);
	POINT mouse = { 0, 0 }; // Current mouse position
	POINT pmouse = { 0, 0 }; // Previous mouse position
	e_mb mb = e_mb::NONE;

	// Functions to easily add menu controls
	MenuItem addSpace();
	MenuItem newColumn();
	MenuItem addBool(char name[30], float &value);
	MenuItem addList(char name[30], string &value, vector<string> options = {});
	MenuItem addSlider(char name[30], float &value, float min, float max, float step);
	MenuItem addColor(char name[30], DWORD &value);

	template <class T>
	void handleValue(T &value, float min, float max, float step, string focus = "");

	void createWindow(char name[30], int x, int y, Window &win);
	void drawWindow(Window &win);

	float rainbow = false;
#ifndef DONATOR
	string wmText = "Sparkly Cheat";
#else
	string wmText = "Donator Build";
#endif
	float wmShow = true;
	void init();
	void getInput(); // Gets all the mouse info

private:
	vector<Window*> windows;
	void addTab(char name[30], int width, vector<MenuItem> items);

	// Window styles!
	WindowStyle BetterMenu = { 5, 10, wTools::bToolbar, wTools::bBackground, wTools::bOutline, wTools::bTab,
		wTools::bCheckbox, wTools::bSlider, wTools::bList, wTools::bListbox, wTools::bColor, wTools::bColorbox };
	WindowStyle Compact = { 5, 10, wTools::cToolbar, wTools::cBackground, wTools::cOutline, wTools::bTab,
		wTools::cCheckbox, wTools::cSlider, wTools::cList, wTools::cListbox, wTools::cColor, wTools::bColorbox };
	WindowStyle LMAO = { 1, 18, wTools::lToolbar, wTools::lBackground, wTools::drawNull, wTools::lTab,
		wTools::lCheckbox, wTools::lSlider, wTools::lList, wTools::bListbox, wTools::lColor, wTools::bColorbox };
	WindowStyle Discord = { 6, 5, wTools::dToolbar, wTools::dBackground, wTools::dOutline, wTools::bTab,
		wTools::dCheckbox, wTools::dSlider, wTools::dList, wTools::bListbox, wTools::dColor, wTools::bColorbox };

public:
	WindowStyle *menuStyle = &BetterMenu; // Multiple menu styles!
private:
	float currentStyle = 1;

	void createGUI(); // Adds the windows to an array

	MenuItem* cControlbox = nullptr;
	POINT cControlboxPos;
};
extern Menu gMenu;