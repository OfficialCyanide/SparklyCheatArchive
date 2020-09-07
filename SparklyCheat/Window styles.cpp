#include "CDrawManagerEx.h"
#include "Fonts.h"
#include "Menu.h"
#include <ctime>

void Menu::WaterMark(const char* name, int x, int y, bool background)
{
	static float hue = 0;
	if (rainbow)
	{
		if (hue >= 360) hue = 0;
		else hue += 0.1f;
	}

	DWORD color = rainbow? gDrawEx.FromHSB(hue / 360, 0.8f, 0.8f, 255) : colors.red;
	DWORD color2 = rainbow ? gDrawEx.FromHSB((360 - hue) / 360, 0.8f, 0.8f, 255) : colors.light;

	int tw = 0, th = 0;
	gInts.Surface->GetTextSize(fonts.cambria, fonts.to_wchar(name), tw, th);

	if (background)
	{
		static Vertex_t verts[4];
		verts[0] = Vertex_t(Vector2(x + 10, y - 10));
		verts[1] = Vertex_t(Vector2(x + tw + 10, y - 10));
		verts[2] = Vertex_t(Vector2(x + tw + 10, y + th + 20));
		verts[3] = Vertex_t(Vector2(x + 10, y + th + 20));

		for (int i = 0; i < 4; i++)
		{
			Vector2 newVec = verts[i].m_Position;
			newVec = rotate_vec2d(Vector2(x + tw / 2, y + th / 2), DEG2RAD1(10), newVec);

			verts[i].m_Position = newVec;
		}
		gDrawEx.DrawPolygon(4, verts, Textures.darkGray);
	}

	gDrawEx.DrawString(x + 14, y + 4, color2, fonts.cambria, name);

	// Replaced colors.red with color
	gDraw.DrawRect(x, y, tw, 5, color);
	gDraw.DrawRect(x, y + 5, 5, th / 2, color);

	gDraw.DrawRect(x + 31, y + 14 + th, tw, 5, color);
	gDraw.DrawRect(x + 31 + tw - 5, y + 14 + th / 2, 5, th / 2, color);
}

void handleInput(MenuItem &item)
{
	static clock_t ldown = clock(), rdown = clock();

	if (!gMenu.canFocus("Items"))
	{
		ldown = clock(), rdown = clock();
		return;
	}

	if (!gMenu.keyDown(VK_LBUTTON))
		ldown = clock();
	if (!gMenu.keyDown(VK_RBUTTON))
		rdown = clock();

	if (gMenu.mb == e_mb::LCLICK || (clock() - ldown > 500 && (clock() - ldown) % 2 == 0))
	{
		// If you're about to go below the min, then go back to the max
		if (item.value[0] - item.step < item.min)
			item.value[0] = item.max;

		else // Otherwise, keep going down
			item.value[0] -= item.step;
	}
	else if (gMenu.mb == e_mb::RCLICK || (clock() - rdown > 500 && (clock() - rdown) % 2 == 0))
	{
		// If you're about to go above the max, then go back to the min
		if (item.value[0] + item.step > item.max)
			item.value[0] = item.min;

		else // Otherwise, keep going up
			item.value[0] += item.step;
	}
}

template <class T>
void Menu::handleValue(T &value, float min, float max, float step, string focus)
{
	static clock_t ldown = clock(), rdown = clock();

	if (focus != "")
	{
		if (!canFocus(focus))
		{
			ldown = clock(), rdown = clock();
			return;
		}
	}

	if (!keyDown(VK_LBUTTON))
		ldown = clock();
	if (!keyDown(VK_RBUTTON))
		rdown = clock();

	if (mb == e_mb::LCLICK || (clock() - ldown > 500 && (clock() - ldown) % 5 == 0))
	{
		// If you're about to go below the min, then go back to the max
		if (value - step < min)
			value = max;

		else // Otherwise, keep going down
			value -= step;
	}
	else if (mb == e_mb::RCLICK || (clock() - rdown > 500 && (clock() - rdown) % 5 == 0))
	{
		// If you're about to go above the max, then go back to the min
		if (value + step > max)
			value = min;

		else // Otherwise, keep going up
			value += step;
	}
}

// BetterMenu style
// -----------------------------------------------------------------------------------------------------------
void wTools::bToolbar(int x, int y, int w, int h)
{
	gDraw.DrawRect(x, y, w, 20, colors.dark); // Draw title bar
	gDraw.DrawRect(x, y + 20, w, 2, colors.bluTeam); // Extra blue thingy below it
}
void wTools::bBackground(int x, int y, int w, int h)
{
	gDraw.DrawRect(x, y, w, h, colors.menuBack);
}
void wTools::bOutline(int x, int y, int w, int h)
{
	gDraw.OutlineRect(x - 1, y - 1, w, h, colors.black);
}
void wTools::bTab(int x, int y, bool selected, const char* text)
{
	DWORD color = selected ? colors.bluTeam : colors.light;
	gDrawEx.DrawString(x, y, color, fonts.titleFont, text);
}

int wTools::bCheckbox(MenuItem &item, int x, int y, bool draw)
{
	int height = 14;
	if (!draw)
		return height;

	DWORD color = colors.light;
	if (gMenu.mouseOver(x, y, item.width, height))
	{
		color = colors.bluTeam;
		handleInput(item);
	}

	gDraw.OutlineRect(x, y, 14, 14, colors.gray); // Draw checkbox outline
	gDraw.DrawRect(x + 1, y + 1, 12, 12, colors.dark); // Draw checkbox background
	gDrawEx.DrawString(x + 20, y, color, fonts.itemFont, item.name);

	if (item.value[0] == 1) // If the value = true, add a check in the box
		gDraw.DrawRect(x + 3, y + 3, 8, 8, colors.bluTeam);

	return height;
}

int wTools::bSlider(MenuItem &item, int x, int y, bool draw)
{
	int height = 26;
	if (!draw)
		return height;

	DWORD color = colors.light;
	if (gMenu.mouseOver(x, y, item.width, height))
	{
		color = colors.bluTeam;
		handleInput(item);
	}

	int percent = item.width * ((int)item.value[0] - item.min) / (item.max - item.min);

	gDrawEx.DrawString(x, y, color, fonts.itemFont, item.name);
	gDraw.DrawRect(x, y + 15, item.width, 8, colors.dark); // Slider background
	gDraw.DrawRect(x, y + 15, percent, 8, color); // Slider progress
	gDraw.OutlineRect(x, y + height, item.width, 1, colors.gray); // Extra line below slider

																  // Draw the value next to the progress of the slider
	gDrawEx.DrawString(percent + x + 3, y + 12, color, fonts.itemFont, fonts.floatStr(item.value[0]).c_str());

	return height;
}

int wTools::bList(MenuItem &item, int x, int y, bool draw)
{
	int height = 26;
	if (!draw)
		return height;

	DWORD color = colors.light;
	if (gMenu.mouseOver(x, y, item.width, height))
	{
		color = colors.bluTeam;

		if (gMenu.mb == e_mb::LCLICK && gMenu.canFocus("Items"))
			item.focused = !item.focused;
	}

	gDrawEx.DrawString(x, y, color, fonts.itemFont, item.name);

	gDraw.DrawRect(x, y + 15, item.width, 8, colors.dark); // Draw the background
														   
	string value = item.strValue[0]; // Current item
	if (value.length() > 14)
		value = value.substr(0, 14) + "...";

	gDrawEx.DrawString(x + 3, y + 13, color, fonts.itemFont,value.c_str());
	gDraw.OutlineRect(x, y + height, item.width, 1, colors.gray); // Extra line below the listbox

	return height;
}

void wTools::bListbox(MenuItem &item, int x, int y)
{
	static int tw, th = 0;
	if (!th)
		gInts.Surface->GetTextSize(fonts.itemFont, L"Text", tw, th);

	int h = bList(item, x, y, false) + 5;
	if (gMenu.mb == e_mb::LCLICK &&
		!gMenu.mouseOver(x - 1, y, item.width + 2, th * item.list.size() + h))
	{
		item.focused = false;
		return;
	}

	for (int i = 0; i < item.list.size(); i++)
	{
		DWORD color = COLORCODE(30, 30, 30, 255);
		if (gMenu.mouseOver(x, y + h, item.width, th - 1))
		{
			color = colors.bluTeam;

			if (gMenu.mb == e_mb::LCLICK)
			{
				item.strValue[0] = item.list[i];
				item.focused = false;
				return;
			}
		}

		gDraw.DrawRect(x, y + h, item.width, th, color);
		gDrawEx.DrawString(x + 2, y + h, colors.light, fonts.itemFont, item.list[i].c_str());
		h += th;
	}
	gDraw.OutlineRect(x - 1, y + bList(item, x, y, false) + 4, item.width + 2, th * item.list.size() + 2, colors.lightGray);
}

int wTools::bColor(MenuItem &item, int x, int y, bool draw)
{
	int height = 26;
	if (!draw)
		return height;

	DWORD color = colors.light;
	DWORD hideColor = COLORCODE(0, 0, 0, 150);
	if (gMenu.mouseOver(x, y, item.width, height))
	{
		color = colors.bluTeam;
		hideColor = 0;

		if (gMenu.mb == e_mb::LCLICK && gMenu.canFocus("Items"))
			item.focused = !item.focused;
	}

	gDrawEx.DrawString(x, y, color, fonts.itemFont, item.name);
	gDraw.DrawRect(x, y + 15, item.width, 8, item.color[0]); // Draw the color
	gDraw.DrawRect(x, y + 15, item.width, 8, hideColor); // Make it dark until it's hovered over
	gDraw.OutlineRect(x, y + height, item.width, 1, colors.gray); // Extra line below the color

	return height;
}

void wTools::bColorbox(MenuItem &item, int x, int y)
{
	y += gMenu.menuStyle[0].drawColor(item, 0, 0, false);
	int w = 120, h = 70;

	if (gMenu.mb == e_mb::LCLICK && !gMenu.mouseOver(x, y, w, h))
	{
		item.focused = false;
		return;
	}

	gDraw.OutlineRect(x, y, w, h, colors.black); // Outline
	gDraw.OutlineRect(x + 1, y + 1, w - 2, h - 2, COLORCODE(60, 60, 60, 255));

	// Making the color bar
	gDraw.DrawRect(x + 2, y + 2, 28, h - 4, COLORCODE(50, 50, 50, 255));
	gDrawEx.DrawLine(x + 30, y + 1, x + 30, y + h - 2, COLORCODE(60, 60, 60, 255));
	gDrawEx.DrawLine(x + 31, y + 1, x + 31, y + h - 2, COLORCODE(35, 35, 35, 255));
	gDraw.DrawRect(x + 6, y + 6, 20, h - 12, item.color[0]);

	x += 32, y += 2, w -= 34, h -= 4; // Making the inner control box
	for (int i = 0; i < 4; i++) // pCode shortening
	{
		DWORD color = i < 2 ? COLORCODE(44, 44, 44, 255) : COLORCODE(35 + ((i - 2) * 25), 35 + ((i - 2) * 25), 35 + ((i - 2) * 25), 255);
		gDraw.OutlineRect(x + i, y + i, w - (i * 2), h - (i * 2), color);
	}
	gDraw.DrawRect(x + 4, y + 4, w - 8, h - 8, COLORCODE(50, 50, 50, 255));

	x += 8, y += 8, w -= 16, h = 14; // Controlling and drawing the RGB values
	int rgb[] = { RED(*item.color), GREEN(*item.color), BLUE(*item.color) };
	static const string strRgb[] = { "Red", "Green", "Blue" };
	for (int i = 0; i < 3; i++)
	{
		DWORD color = colors.light;
		if (gMenu.mouseOver(x, y, w, h))
		{
			color = colors.bluTeam;
			gMenu.handleValue(rgb[i], 0, 255, 1);
		}
		gDrawEx.DrawString(x, y, color, fonts.smallFont, string(strRgb[i] + " = " + to_string(rgb[i])).c_str());
		y += 18;
	}
	*item.color = COLORCODE(rgb[0], rgb[1], rgb[2], ALPHA(*item.color));
}

// Compact style
// -----------------------------------------------------------------------------------------------------------
void wTools::cToolbar(int x, int y, int w, int h)
{
	gDrawEx.RainbowRect(x, y, w, 3, false, 161, 194, 255); // Rainbow thingy
}
void wTools::cOutline(int x, int y, int w, int h)
{
	for (int i = 1; i <= 6; i++) // Draw the outline
	{
		DWORD currentColor = COLORCODE(35, 35, 35, 255);
		if (i == 1 || i == 5)
			currentColor = COLORCODE(57, 57, 57, 255);
		else if (i == 6)
			currentColor = colors.black;

		gDraw.OutlineRect(x - i, y - i, w + (i * 2), h + (i * 2), currentColor);
	}
}
void wTools::cBackground(int x, int y, int w, int h)
{
	gDraw.DrawRect(x, y, w, h, COLORCODE(17, 17, 17, 255));
}

int wTools::cCheckbox(MenuItem &item, int x, int y, bool draw)
{
	//int height = 0, textWidth = 0;
	int height = 11;
	if (!draw)
		return height;

	DWORD color = colors.light;
	if (gMenu.mouseOver(x, y, item.width, height))
	{
		color = colors.bluTeam;
		handleInput(item);
	}

	gDrawEx.DrawString(x + 16, y, color, fonts.outlinedFont, item.name);

	// Draw checkbox background
	gDrawEx.GradientRect(x + 1, y + 3, 8, 8, true, COLORCODE(50, 50, 50, 255), COLORCODE(30, 30, 30, 255));
	gDraw.OutlineRect(x, y + 2, 10, 10, colors.black); // Draw checkbox outline

	if (item.value[0] == 1) // If the value = true, add a check in the box
		gDraw.DrawRect(x + 2, y + 4, 6, 6, colors.bluTeam);

	return height;
}

int wTools::cSlider(MenuItem &item, int x, int y, bool draw)
{
	int height = 19;
	if (!draw)
		return height;

	DWORD color = colors.light;
	if (gMenu.mouseOver(x, y, item.width, height))
	{
		color = colors.bluTeam;
		handleInput(item);
	}

	int percent = item.width * ((int)item.value[0] - item.min) / (item.max - item.min);

	gDrawEx.DrawString(x, y - 2, color, fonts.outlinedFont, item.name);
	gDraw.OutlineRect(x, y + 10, item.width + 2, 10, colors.black); // Slider outline
																	// Slider background
	gDrawEx.GradientRect(x + 1, y + 11, item.width, 8, true, COLORCODE(50, 50, 50, 255), COLORCODE(30, 30, 30, 255));
	gDraw.DrawRect(x + 1, y + 11, percent, 8, color); // Slider progress
													  // Draw the value next to the progress of the slider
	gDrawEx.DrawString(percent + x + 4, y + 9, color, fonts.outlinedFont, fonts.floatStr(item.value[0]).c_str());

	return height;
}

int wTools::cList(MenuItem &item, int x, int y, bool draw)
{
	int height = 19;
	if (!draw)
		return height;

	DWORD color = colors.light;
	if (gMenu.mouseOver(x, y, item.width, height))
	{
		color = colors.bluTeam;

		if (gMenu.mb == e_mb::LCLICK && gMenu.canFocus("Items"))
			item.focused = !item.focused;
	}

	gDrawEx.DrawString(x, y - 2, color, fonts.outlinedFont, item.name);
	gDraw.OutlineRect(x, y + 10, item.width + 2, 10, colors.black); // Outline
																	// Draw the background
	gDrawEx.GradientRect(x + 1, y + 11, item.width, 8, true, COLORCODE(50, 50, 50, 255), COLORCODE(30, 30, 30, 255));

	string value = item.strValue[0]; // Current item
	if (value.length() > 14)
		value = value.substr(0, 14) + "...";

	gDrawEx.DrawString(x + 3, y + 9, color, fonts.outlinedFont, value.c_str());

	return height;
}

void wTools::cListbox(MenuItem &item, int x, int y)
{
	static int tw, th = 0;
	if (!th)
	{
		gInts.Surface->GetTextSize(fonts.outlinedFont, L"Text", tw, th);
		th -= 3;
	}

	int h = cList(item, x, y, false) + 5;
	if (gMenu.mb == e_mb::LCLICK &&
		!gMenu.mouseOver(x - 1, y, item.width + 2, th * item.list.size() + h))
	{
		item.focused = false;
		return;
	}

	for (int i = 0; i < item.list.size(); i++)
	{
		DWORD color = COLORCODE(70, 70, 70, 255);
		if (gMenu.mouseOver(x, y + h, item.width, th - 1))
		{
			color = colors.dark;

			if (gMenu.mb == e_mb::LCLICK)
			{
				item.strValue[0] = item.list[i];
				item.focused = false;
				return;
			}
		}

		gDraw.DrawRect(x, y + h, item.width, th, color);
		gDrawEx.DrawString(x + 2, y + h, colors.light, fonts.outlinedFont, item.list[i].c_str());
		h += th;
	}
	gDraw.OutlineRect(x - 1, y + cList(item, x, y, false) + 4, item.width + 2, th * item.list.size() + 2, colors.lightGray);
}

int wTools::cColor(MenuItem &item, int x, int y, bool draw)
{
	int height = 19;
	if (!draw)
		return height;

	DWORD color = colors.light;
	DWORD hideColor = COLORCODE(0, 0, 0, 150);
	if (gMenu.mouseOver(x, y, item.width, height))
	{
		color = colors.bluTeam;
		hideColor = 0;

		if (gMenu.mb == e_mb::LCLICK && gMenu.canFocus("Items"))
			item.focused = !item.focused;
	}

	gDrawEx.DrawString(x, y - 2, color, fonts.outlinedFont, item.name);
	gDraw.OutlineRect(x, y + 10, item.width + 2, 10, colors.black); // Outline

	gDraw.DrawRect(x + 1, y + 11, item.width, 8, *item.color); 	// Draw the color
	gDraw.DrawRect(x + 1, y + 11, item.width, 8, hideColor); // Keep it dark until hovered over


	return height;
}

// LMAOBOX style
// -----------------------------------------------------------------------------------------------------------
void wTools::lToolbar(int x, int y, int w, int h)
{
	gDraw.DrawRect(x, y, w, 5, colors.bluTeam);
}
void wTools::lBackground(int x, int y, int w, int h)
{
	gDraw.DrawRect(x, y, w, h, colors.menuBack);
}
void wTools::drawNull(int x, int y, int w, int h) { return; }
void wTools::lTab(int x, int y, bool enabled, const char *text)
{
	DWORD color = enabled ? colors.bluTeam : colors.light;
	gDrawEx.DrawString(x, y, color, fonts.lbox, text);
}

int wTools::lCheckbox(MenuItem &item, int x, int y, bool draw)
{
	int height = 18;
	if (!draw)
		return height;
	DWORD color = item.value[0] ? colors.light : colors.lightGray;

	if (gMenu.mouseOver(x, y, item.width, height))
	{
		gDraw.DrawRect(x, y, item.width, height, colors.bluTeam);
		handleInput(item);
	}

	gDrawEx.DrawString(x + 4, y + 3, color, fonts.tf2, item.name);

	return height;
}

int wTools::lSlider(MenuItem &item, int x, int y, bool draw)
{
	int height = 28;
	if (!draw)
		return height;

	if (gMenu.mouseOver(x, y, item.width, height))
	{
		gDraw.DrawRect(x, y, item.width, height, colors.bluTeam);
		handleInput(item);
	}

	int percent = (item.width - 8) * ((int)item.value[0] - item.min) / (item.max - item.min);


	gDrawEx.DrawString(x + 4, y + 3, colors.light, fonts.tf2, item.name);
	gDraw.DrawRect(x + 4, y + 15, item.width - 8, 8, colors.gray);
	gDraw.DrawRect(x + 4, y + 15, percent, 8, colors.light);
	gDrawEx.DrawString(x + 7 + percent, y + 15, colors.light, fonts.tf2, fonts.floatStr(item.value[0]).c_str());

	return height;
}

int wTools::lList(MenuItem &item, int x, int y, bool draw)
{
	int height = 28;
	if (!draw)
		return height;

	if (gMenu.mouseOver(x, y, item.width, height))
	{
		gDraw.DrawRect(x, y, item.width, height, colors.bluTeam);

		if (gMenu.mb == e_mb::LCLICK && gMenu.canFocus("Items"))
			item.focused = !item.focused;
	}

	gDrawEx.DrawString(x + 4, y + 3, colors.light, fonts.tf2, item.name);
	gDraw.DrawRect(x + 4, y + 15, item.width - 8, 8, colors.gray); // Draw the background

	string value = item.strValue[0]; // Current item
	if (value.length() > 14)
		value = value.substr(0, 14) + "...";

	gDrawEx.DrawString(x + 3, y + 13, colors.light, fonts.tf2, value.c_str());

	return height;
}

int wTools::lColor(MenuItem &item, int x, int y, bool draw)
{
	int height = 28;
	if (!draw)
		return height;

	DWORD hideColor = (0, 0, 0, 150);
	if (gMenu.mouseOver(x, y, item.width, height))
	{
		gDraw.DrawRect(x, y, item.width, height, colors.bluTeam);
		hideColor = 0;

		if (gMenu.mb == e_mb::LCLICK && gMenu.canFocus("Items"))
			item.focused = !item.focused;
	}

	gDrawEx.DrawString(x + 4, y + 3, colors.light, fonts.tf2, item.name);

	gDraw.DrawRect(x + 4, y + 15, item.width - 8, 8, *item.color); // Draw the background
	gDraw.DrawRect(x + 4, y + 15, item.width - 8, 8, hideColor); // Keep it dark until hovered over

	return height;
}

// Discord style
// ------------------------------------------------------------------------------------------------------------
void wTools::dToolbar(int x, int y, int w, int h)
{
	gDraw.DrawRect(x,y, w, 20, COLORCODE(60, 60, 60, 255));
}
void wTools::dBackground(int x, int y, int w, int h)
{
	gDraw.DrawRect(x, y + 20, w, h - 20, COLORCODE(40, 40, 40, 240));
	gDrawEx.GradientRect(x, y + 20, w, 6, true, COLORCODE(0, 0, 0, 100), COLORCODE(0, 0, 0, 0));
}
void wTools::dOutline(int x, int y, int w, int h)
{
	for (int i = 1; i < 9; i++)
	{
		DWORD color = COLORCODE(0, 0, 0, 80 - (i * 10));
		gDraw.OutlineRect(x - i, y - i, w + (i * 2), h + (i * 2), color);
	}
}

int wTools::dCheckbox(MenuItem &item, int x, int y, bool draw)
{
	int height = 13;
	if (!draw)
		return height;

	if (gMenu.mouseOver(x, y, item.width, 13))
	{
		gDrawEx.RoundRect(x, y, item.width, 15, 5, Textures.lightGray);
		handleInput(item);
	}

	gDrawEx.DrawString(x + 5, y, item.value[0] ? colors.light : colors.lightGray, fonts.smallFont, item.name);

	return height;
}

int wTools::dSlider(MenuItem &item, int x, int y, bool draw)
{
	int height = 25;
	if (!draw)
		return height;

	if (gMenu.mouseOver(x, y, item.width, height))
	{
		gDrawEx.RoundRect(x, y, item.width, height + 2, 5, Textures.lightGray);
		handleInput(item);
	}

	int percent = (item.width - 10) * ((int)item.value[0] - item.min) / (item.max - item.min);

	gDrawEx.DrawString(x + 5, y, colors.light, fonts.smallFont, item.name);
	gDrawEx.RoundRect(x + 4, y + 13, item.width - 8, 7, 4, Textures.darkGray);
	gDrawEx.RoundRect(x + 5, y + 14, percent, 5, 3, Textures.light);
	gDrawEx.DrawString(x + 5 + percent, y + 13, colors.light, fonts.smallFont, fonts.floatStr(item.value[0]).c_str());

	return height;
}

int wTools::dList(MenuItem &item, int x, int y, bool draw)
{
	int height = 22;
	if (!draw)
		return height;

	if (gMenu.mouseOver(x, y, item.width, height))
	{
		gDrawEx.RoundRect(x, y, item.width, height + 2, 5, Textures.lightGray);

		if (gMenu.mb == e_mb::LCLICK && gMenu.canFocus("Items"))
			item.focused = !item.focused;
	}

	gDrawEx.DrawString(x + 5, y, colors.light, fonts.smallFont, item.name);
	gDrawEx.RoundRect(x + 3, y + 13, item.width - 7, 10, 4, Textures.darkGray);

	string value = item.strValue[0]; // Current item
	if (value.length() > 14)
		value = value.substr(0, 14) + "...";

	gDrawEx.DrawString(x + 3, y + 10, colors.light, fonts.smallFont, value.c_str());

	return height;
}

int wTools::dColor(MenuItem &item, int x, int y, bool draw)
{
	int height = 22;
	if (!draw)
		return height;

	DWORD hideColor = COLORCODE(0, 0, 0, 150);
	if (gMenu.mouseOver(x, y, item.width, height))
	{
		gDrawEx.RoundRect(x, y, item.width, height + 2, 5, Textures.lightGray);
		hideColor = 0;

		if (gMenu.mb == e_mb::LCLICK && gMenu.canFocus("Items"))
			item.focused = !item.focused;
	}

	gDrawEx.DrawString(x + 5, y, colors.light, fonts.smallFont, item.name);
	gDraw.DrawRect(x + 3, y + 13, item.width - 7, 10, *item.color);
	gDraw.DrawRect(x + 3, y + 13, item.width - 7, 10, hideColor);

	return height;
}