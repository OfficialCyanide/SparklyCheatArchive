#include "Menu.h"
#include "CDrawManagerEx.h"
#include "Fonts.h"

#include "Esp.h"
#include "Misc.h"
#include "Aim.h"
#include "HvH.h"
#include "Players.h"
#include "Hitscan.h"
#include "Config.h"
#include "Trigger.h"
#include "Mathack.h"

Menu gMenu;

void Menu::createGUI()
{
	ItemList aim =
	{
		addBool("Enabled", gAim.enabled),
		addSpace(),
		addList("Aim key", gAim.key, { "None", "Shift", "Mouse1", "Mouse2", "Mouse3", "F", "Alt" }),
		addList("Aim method", gAim.method,{ "FOV", "Distance" }),
		addSpace(),
		addList("Priority", gAim.priority,{ "Head", "Torso", "Feet", "Auto" }),
		addBool("Priority only", gAim.noScan),
		addSpace(),
		addBool("Auto shoot", gAim.autoShoot),
		addBool("Silent", gAim.silent),
		addSlider("Slow aim", gAim.slowAim, 0, 50, 0.5),
		newColumn(),
		addSlider("FOV limit", gAim.fov, 0, 180, 1),
		addBool("Scoped only", gAim.scopedOnly),
		addBool("Show target", gAim.showTarget),
		addBool("Multi point", gAim.multipoint),
		addSpace(),
		addBool("Ignore invis", gAim.ignoreInvis),
		addBool("Ignore disguise", gAim.ignoreDisguise),
		addBool("Ignore uber", gAim.ignoreUber),
		addBool("Ignore bonk", gAim.ignoreBonk),
		addSpace(),
		addBool("Sentries", gAim.aimSentry),
		addBool("Dispensers", gAim.aimDispenser),
		addBool("Teleports", gAim.aimTeleport),
		addSpace(),
		addBool("Wrangler", gAim.wrangler)
	};
	addTab("Aimbot", 150, aim);

	ItemList trigger =
	{
		addBool("Autoshoot", gTrigger.autoshoot),
		addSpace(),
		addList("Aim key", gTrigger.key, { "None", "Shift", "Mouse1", "Mouse2", "Mouse3", "F", "Alt" }),
		addList("Priority", gTrigger.priority,{ "Head", "Torso", "Feet", "All" }),
		addSpace(),
		addBool("Auto backstab", gTrigger.backstab),
		addBool("Auto airblast", gTrigger.airblast),
		addBool("Rage airblast", gTrigger.aimproj),
		newColumn(),
		addBool("Ignore invis", gTrigger.ignoreInvis),
		addBool("Ignore disguise", gTrigger.ignoreDisguise),
		addBool("Ignore uber", gTrigger.ignoreUber),
		addBool("Ignore bonk", gTrigger.ignoreBonk),
		addSpace(),
		addBool("Sentries", gTrigger.aimSentry),
		addBool("Dispensers", gTrigger.aimDispenser),
		addBool("Teleports", gTrigger.aimTeleport)
	};
	addTab("Trigger", 150, trigger);

	ItemList hvh =
	{
		addBool("Anti aim", gAutist.antiaim),
		addList("Pitch", gAutist.pitch,{ "None", "Fake up", "Fake down", "Up", "Down", "Lisp", "Meme spin", "Meme (Scoped)", "Meme (Unscoped)", "Bait (Mouse3)"}),
		addList("Yaw", gAutist.yaw, { "None", "Spin", "Edge", "Fake side", "Jitter (0/90)", "Jitter (0/-90)", "Jitter spin"}),
		addSlider("Yaw add", gAutist.yAngle, -180, 180, 22.5),
		addSpace(),
		addSlider("Speedhack", gAutist.speedAmount, 0, 30, 1),
		addList("Speed key", gAutist.key,{ "None", "Shift", "Mouse1", "Mouse2", "Mouse3", "F", "Alt" }),
		newColumn(),
		addBool("No push", gAutist.nopush),
		addBool("Third person", gAutist.thirdperson),
		addBool("Show real angles", gAutist.realangles),
		addSpace(),
		addBool("Resolver", gAutist.resolver),
		addBool("Remove animations", gAutist.animations),
		addList("Force pitch", gAutist.pForce,{ "Auto", "Up", "Down", "Center" }),
		addList("Force yaw", gAutist.yForce, {"None", "180", "90", "0", "-90"})
	};
	addTab("H v H", 150, hvh);

	ItemList esp =
	{
		addBool("Enabled", gEsp.enabled),
		addSpace(),
		addBool("Players", gEsp.drawPlayer),
		addBool("Team", gEsp.drawTeam),
		addBool("Name", gEsp.drawName),
		addBool("Class", gEsp.drawClass),
		addBool("Bones", gEsp.drawBones),
		addSpace(),
		addList("Invisibility", gEsp.invisible, { "None", "Don't show", "Highlight" }),
		addList("Health", gEsp.drawHealth, { "None", "Text", "Bar (Left)", "Bar (Right)", "Bar (Bottom)", "Bar (Top)" }),
		newColumn(),
		addBool("Projectiles", gEsp.projectiles),
		addBool("Health", gEsp.health),
		addBool("Ammo", gEsp.ammo),
		addSpace(),
		addBool("Sentries", gEsp.sentry),
		addBool("Dispensers", gEsp.dispenser),
		addBool("Teleports", gEsp.teleports),
		addBool("Doors", gEsp.doors),
		addColor("Color", gEsp.objectColor),
		addSpace(),
		addBool("Steam GUIDs (ID3)", gEsp.guids),
		newColumn(),
		addBool("Custom colors", gEsp.customColor),
		addColor("Player color", gEsp.playerColor),
		addSpace(),
		addList("Font", gEsp.font, {"Small", "Medium", "Fancy", "TF2 Build", "Times New Roman", "Arial Black"}),
		addList("Box style", gEsp.boxStyle, { "None", "2D", "2D + Outline", "Corners", "3D" }),
		addList("Text align", gEsp.textAlign, { "Center", "Side", "Bottom" }),
		addSlider("Box thickness", gEsp.boxSize, 1, 20, 1),
		addBool("Drunk boxes", gEsp.drunk)
	};
	addTab("ESP", 150, esp);

	ItemList players =
	{
		addBool("Player list", gPlayers.list),
		//addBool("Spectator list", gPlayers.spectators),
		addSpace(),
		addBool("Radar", gPlayers.radar),
		addSlider("Transparency", gPlayers.transparency, 0, 255, 1),
		addSlider("Icon size", gPlayers.pSize, 2, 20, 1),
		addSlider("Size", gPlayers.size, 50, 500, 1),
		addSlider("Range", gPlayers.maxDist, 2, 10000, 5),
		addSpace(),
		addBool("Team", gPlayers.team),
		addBool("Health", gPlayers.drawHealth)
	};
	addTab("Players", 150, players);

	ItemList misc =
	{
		addBool("Prediction path", gMisc.local),
		addBool("Trace paths", gMisc.trace),
		addBool("Hitboxes", gMisc.hitpos),
		addSpace(),
		addBool("sv_cheats bypass", gMisc.cheatBp),
		addBool("Unlock commands", gMisc.killAllCvars),
		newColumn(),
		addBool("Bunny hop", gMisc.bhop),
		addBool("Viewmodel", gMisc.viewmodel),
		addSlider("Custom FOV", gMisc.fov, 0, 179, 1),
		addList("Sniper scope", gMisc.scope,{ "None", "No zoom", "No scope", "Both" }),
		newColumn(),
		addList("Sequence freeze", gMisc.condkey, { "None", "Shift", "Mouse1", "Mouse2", "Mouse3", "F", "Alt", "R" }),
		addSlider("Amount", gMisc.condAmount, 0, 900, 100),
		addBool("Fast weapon switch", gMisc.wepSwitch)
	};
	addTab("Misc", 150, misc);

	ItemList config =
	{
		addBool("Save config", gConfig.save_config),
		addBool("Reload config", gConfig.reload_config),
		addSpace(),
		addSlider("Menu style", currentStyle, 0, 3, 1),
		addBool("Rainbow", rainbow),
		//addSpace(),
		//addBool("Reload chatspam", gConfig.reload_chatspam)
	};
	addTab("Settings", 150, config);

#ifdef DONATOR
	ItemList idk =
	{
		//addBool("Mathack", gMat.enabled),
		//addList("Hands", gMat.hands,{ "Normal", "Hide", "Wireframe", "Wireframe overlay" }),
		//addSpace(),
		addBool("Effect changer", gMisc.skinChanger),
		addList("Sniper rifle", gMisc.rifle, { "Default", "AWP", "Festive", "Balloonicorn", "Night owl" }),
		addList("Rocker launcher", gMisc.rocket, { "Default", "Festive", "Sand cannon", "Shell shocker" }),
		addList("Unusual weapon", gMisc.uwep, {"None", "Hot", "Cool", "Energy orb", "Isotope"}),
		addList("Unusual taunt", gMisc.utaunt, {"None", "Sparkle", "Fire", "Lanterns", "Electricity", "Pixels", "Disco", "Lights", "Fireworks"}),
		addBool("Australium", gMisc.australium),
		newColumn(),
		addBool("Green screen", gMisc.greenscreen),
		addBool("Set pos", gMisc.setscreen),
		addSlider("Offset x", gMisc.xscreen, -1000, 1000, 1),
		addSlider("Offset y", gMisc.yscreen, -1000, 1000, 1),
		addSlider("Offset z", gMisc.zscreen, -1000, 1000, 1),
		addSlider("Width", gMisc.wscreen, 1, 1000, 1),
		addSlider("Height", gMisc.hscreen, 1, 1000, 1),
		addSlider("Rotation", gMisc.rotscreen, 0, 359, 1),
		addColor("Color", gMisc.colorscreen),
		newColumn(),
		addList("Watermark text", wmText),
		addBool("Show watermark", wmShow),
	};
	addTab("Perks", 150, idk);
#endif

	gPlayers.PlayerWin.w = 120;
	createWindow("Player list", 300, 100, gPlayers.PlayerWin);
	createWindow("Radar", 100, 100, gPlayers.RadarWin);
	createWindow("Spectator list", 300, 100, gPlayers.SpecWin);
}

void Menu::openControlbox(MenuItem &item, int x, int y)
{
	if (canFocus("Controlbox"))
	{
		cControlbox = &item;
		cControlboxPos = { x, y };
		focus = "Controlbox";
	}
}

int Menu::drawItem(MenuItem &item, int x, int y)
{
	switch (item.Type)
	{
	case ItemType::checkbox:
			return menuStyle[0].drawCheckbox(item, x, y, true);
	case ItemType::slider:
			return menuStyle[0].drawSlider(item, x, y, true);
	case ItemType::list:
			if (item.focused == true)
				openControlbox(item, x, y);
			return menuStyle[0].drawList(item, x, y, true);
	case ItemType::space:
			return menuStyle[0].spaceSize;
	case ItemType::color:
		if (item.focused == true)
			openControlbox(item, x, y);
		return menuStyle[0].drawColor(item, x, y, true);
	}

	return 0;
}

void Menu::drawItems(Tab &tab)
{
	// Position of first item
	int x = mPos.x + 120;
	int y = mPos.y + 33;

	for (unsigned int i = 0; i < tab.items.size(); i++) // Draw all the items
	{
		switch (tab.items[i].Type)
		{
		case ItemType::newline:
				y = mPos.y + 33, x += 150; break;
		default:
			y += drawItem(tab.items[i], x, y);
			y += menuStyle->itemSpace;
		}
	}
}

void Menu::paint()
{
	getInput(); // Get mouse and menu key info

	if (!menuToggle)
		return;

	switch ((int)currentStyle)
	{
	case 0: menuStyle = &BetterMenu; break;
	case 1: menuStyle = &Compact; break;
	case 2: menuStyle = &LMAO; break;
	case 3: menuStyle = &Discord; break;
	}

	// Draw the window so items can be drawn on top
	menuStyle->drawWindow(mPos.x, mPos.y, mWidth, mHeight);

	static bool dragging = false;

	if ((mb == e_mb::LCLICK || mb == e_mb::RCLICK) && mouseOver(mPos.x, mPos.y, mWidth, 20))
		dragging = canFocus("Cheat");
	else if (dragging && !(mb == e_mb::LDOWN || mb == e_mb::RDOWN))
		dragging = false, focus = "";

	if (dragging)
	{
		focus = "Cheat";
		if (keyDown(VK_RBUTTON))
		{
			mPos.x = floor(mouse.x / 10) * 10;
			mPos.y = floor(mouse.y / 10) * 10;
		}
		else
		{
			mPos.x += mouse.x - pmouse.x;
			mPos.y += mouse.y - pmouse.y;
		}
	}

	if (mPos.x < 0) // Clamping the window so it doesn't go off-screen
		mPos.x = 0;
	else if (mPos.x + mWidth > gScreen.width)
		mPos.x = gScreen.width - mWidth;

	if (mPos.y < 0)
		mPos.y = 0;
	else if (mPos.y + 33 > gScreen.height)
		mPos.y = gScreen.height - 33;

	if (tabList.size() < 1) // Do we have tabs?
		return;

	// Draw all tabs
	int newX = mPos.x + 15, newY = mPos.y + 30;
	for (int i = 0; i < tabList.size(); i++)
	{
		if ((mb == e_mb::LCLICK || mb == e_mb::RCLICK) && mouseOver(mPos.x, newY, 100, 27))
			currentTab = i;
		menuStyle->drawTab(newX, newY, i == currentTab, tabList[i].name);
		newY += 28;
	}

	drawItems(tabList[currentTab]);

	if (cControlbox != nullptr) // Check to see if we have a listbox open
	{
		if (cControlbox[0].focused)
			switch (cControlbox[0].Type)
			{
			case ItemType::list:
				menuStyle[0].drawListbox(cControlbox[0], cControlboxPos.x, cControlboxPos.y); break;
			case ItemType::color:
				menuStyle[0].drawColorbox(cControlbox[0], cControlboxPos.x, cControlboxPos.y); break;
			}
		else
		{
			cControlbox = nullptr;
			focus = "";
		}
	}
}

void Menu::init()
{
	createGUI(); // Add all the windows
}

void Menu::getInput()
{	
	if ((GetAsyncKeyState(VK_INSERT) & 1) || (GetAsyncKeyState(VK_F11) & 1))
		menuToggle = !menuToggle;

	pmouse = mouse;

	int mousex = 0, mousey = 0;
	gInts.Surface->GetCursorPosition(mousex, mousey);
	mouse.x = mousex, mouse.y = mousey;

	if (keyDown(VK_LBUTTON))
	{
		if (mb == e_mb::NONE) mb = e_mb::LCLICK;
		else mb = e_mb::LDOWN;
	}
	else if (keyDown(VK_RBUTTON))
	{
		if (mb == e_mb::NONE) mb = e_mb::RCLICK;
		else mb = e_mb::RDOWN;
	}
	else
		mb = e_mb::NONE;
}

bool Menu::keyDown(int vKey)
{
	return GetKeyState(vKey) < 0;
}
bool Menu::mouseOver(int x, int y, int w, int h)
{
	return mouse.x >= x && mouse.x <= x + w && mouse.y >= y && mouse.y <= y + h;
}

MenuItem Menu::addSpace()
{
	MenuItem item;
	item.Type = ItemType::space;
	return item;
}

MenuItem Menu::newColumn()
{
	MenuItem item;
	item.Type = ItemType::newline;
	return item;
}

MenuItem Menu::addBool(char name[30], float &value)
{
	MenuItem item;
	item.Type = ItemType::checkbox;
	item.value = &value;
	item.min = 0;
	item.max = 1;
	item.step = 1;
	strcpy(item.name, name);

	return item;
}

MenuItem Menu::addList(char name[30], string &value, vector<string> options)
{
	MenuItem item;
	item.Type = ItemType::list;
	item.strValue = &value;
	item.list = options;
	strcpy(item.name, name);

	return item;
}

MenuItem Menu::addSlider(char name[30], float &value, float min, float max, float step)
{
	MenuItem item;
	item.Type = ItemType::slider;
	item.value = &value;
	item.min = min;
	item.max = max;
	item.step = step;
	strcpy(item.name, name);

	return item;
}

MenuItem Menu::addColor(char name[30], DWORD &value)
{
	MenuItem item;
	item.Type = ItemType::color;
	item.color = &value;
	strcpy(item.name, name);

	return item;
}

void Menu::addTab(char name[30], int width, vector<MenuItem> items)
{
	Tab temp; // Make a temporary window to avoid initialization parameters
	strcpy(temp.name, name);

	for (unsigned int i = 0; i < items.size(); i++) // Set all the item widths to the window's width
		items[i].width = width - 30;

	temp.items = items;
	tabList.push_back(temp); // Add the window
}

void Menu::createWindow(char name[30], int x, int y, Window &win)
{
	strcpy(win.name, name);
	win.x = x, win.y = y;

	windows.push_back(&win);
}

void Menu::drawWindow(Window &win)
{
	int x = win.x - 5, w = win.w + 10;
	int y = win.y - 25, h = win.h + 30;

	if ((mb == e_mb::LCLICK || mb == e_mb::RCLICK) && mouseOver(x, y, w, 20))
		win.dragging = canFocus(win.name);
	else if (win.dragging && (mb != e_mb::LDOWN && mb != e_mb::RDOWN))
		win.dragging = false, focus = "";

	if (win.dragging)
	{
		focus = win.name;
		if (keyDown(VK_RBUTTON))
		{
			win.x = floor(mouse.x / 10) * 10;
			win.y = floor(mouse.y / 10) * 10;
		}
		else
		{
			win.x += mouse.x - pmouse.x;
			win.y += mouse.y - pmouse.y;
		}
	}

	gDraw.DrawRect(x, y, w, h, COLORCODE(40, 40, 40, 255)); // background

	gDraw.OutlineRect(x - 1, y - 1, w + 2, h + 2, colors.black); // outline
	gDraw.OutlineRect(x, y, w, h, COLORCODE(60, 60, 60, 255));

	gDraw.DrawRect(x + 1, y + 1, w - 2, 19, COLORCODE(50, 50, 50, 255)); // topbar
	gDrawEx.DrawLine(x, y + 20, x + w - 1, y + 20, COLORCODE(60, 60, 60, 255));
	gDrawEx.DrawLine(x, y + 21, x + w - 1, y + 21, COLORCODE(35, 35, 35, 255));

	gDrawEx.DrawString(win.x + 6, y + 3, colors.light, fonts.smallFont, win.name);
}