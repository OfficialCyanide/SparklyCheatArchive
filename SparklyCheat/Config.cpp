#include "Config.h"
#include <fstream>
#include <string>
#include "Menu.h"
#include "Fonts.h"
#include "Misc.h"

Config gConfig;

void Config::init()
{
	loadCfg();
	//loadChatspam();
}

/*void Config::loadChatspam()
{
	if (!fileExists(CFG_PATH + "\\Chatspam.txt"))
		saveChatspam();
	else
	{
		gMisc.chatSpams.clear(); // Wipe the vector to add custom chat spams
		ifstream myfile(CFG_PATH + "\\Chatspam.txt");
		string line;
		string spam = "\x0";
		while (getline(myfile, line))
		{
			if (line[0] == '/' && line[1] == '/')
			{
				gMisc.chatSpams.push_back(spam);
				spam = "\x0";
				continue;
			}
			if (spam != "\x0")
				spam += "\n";
			spam += line;
		}
	}
}*/

/*void Config::saveChatspam()
{
	CreateDirectory(CFG_PATH.c_str(), NULL);
	ofstream chatspam;
	chatspam.open(CFG_PATH + "\\Chatspam.txt");

	for (int i = 0; i < gMisc.chatSpams.size(); i++)
	{
		chatspam << gMisc.chatSpams[i] << endl;
		chatspam << "// Use comments like these to separate different lines of chatspam" << endl;
	}

	chatspam.close();
}*/

DWORD parseRgba(string color)
{
	byte rgba[4];
	int a = 0;
	byte current = 0;
	for (int i = 0; i < color.length(); i++)
	{
		if (color[i] == ',')
		{
			rgba[a] = current, current = 0, a++;
			continue;
		}

		current = current * 10 + (color[i] - 48);

		if (i == color.length() - 1)
			rgba[a] = current;
	}

	return COLORCODE(rgba[0], rgba[1], rgba[2], rgba[3]);
}

void Config::loadCfg()
{
	if (!fileExists(CFG_PATH + "\\Config.txt"))
	{
		welcome = true;
		saveCfg();
	}
	else
	{
		if (getProperty(0) != SPARKLY_VERSION)
		{
			welcome = true;
			saveCfg();
			return;
		}

		int line = 0;
		for (int a = 0; a < gMenu.tabList.size(); a++)
		{
			line += 2;
			for (int i = 0; i < gMenu.tabList[a].items.size(); i++)
			{
				if (gMenu.tabList[a].items[i].Type == ItemType::space ||
					gMenu.tabList[a].items[i].Type == ItemType::newline)
					continue;

				string value = getProperty(line);
				if (gMenu.tabList[a].items[i].value != nullptr)
				{
					int iValue = stoi(value); // Make sure the value is valid to prevent unexpected bugs
					if (iValue >= gMenu.tabList[a].items[i].min && iValue <= gMenu.tabList[a].items[i].max)
						gMenu.tabList[a].items[i].value[0] = iValue;
				}
				else if (gMenu.tabList[a].items[i].strValue != nullptr)
				{
					// Check to ensure that we don't use incorrect string values (like priority set to "AYY LMAO")
					if (gMenu.tabList[a].items[i].list != vector<string>{})
					{
						bool correct = false;
						for (int b = 0; b < gMenu.tabList[a].items[i].list.size(); b++) // Whew!
							if (value == gMenu.tabList[a].items[i].list[b])
							{
								correct = true;
								break;
							}
						if (!correct)
							value = gMenu.tabList[a].items[i].strValue[0];
					}
					gMenu.tabList[a].items[i].strValue[0] = value;
				}
				else if (gMenu.tabList[a].items[i].color != nullptr)
					gMenu.tabList[a].items[i].color[0] = parseRgba(value);

				line++;
			}
		}
	}
}

string colorStr(DWORD color)
{
	return	to_string(RED(color)) + "," +
			to_string(GREEN(color)) + "," +
			to_string(BLUE(color)) + "," +
			to_string(ALPHA(color));
}

void Config::saveCfg()
{
	CreateDirectory(CFG_PATH.c_str(), NULL);
	ofstream config;
	config.open(CFG_PATH + "\\Config.txt");

	config << "Version = " << SPARKLY_VERSION << endl;
	for (int a = 0; a < gMenu.tabList.size(); a++)
	{
		config << "// " << gMenu.tabList[a].name << endl;
		for (int i = 0; i < gMenu.tabList[a].items.size(); i++)
		{
			if (gMenu.tabList[a].items[i].Type == ItemType::space ||
				gMenu.tabList[a].items[i].Type == ItemType::newline)
				continue;

			config << gMenu.tabList[a].items[i].name << " = ";
			if (gMenu.tabList[a].items[i].value != nullptr)
				config << (int)gMenu.tabList[a].items[i].value[0] << endl;
			else if (gMenu.tabList[a].items[i].strValue != nullptr)
				config << gMenu.tabList[a].items[i].strValue[0] << endl;
			else if (gMenu.tabList[a].items[i].color != nullptr)
				config << colorStr(gMenu.tabList[a].items[i].color[0]) << endl;
		}
		config << endl;
	}
	config.close();
}

string Config::getDirectory()
{
	char buffer[MAX_PATH];
	GetModuleFileName(hInstance, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	return string(buffer).substr(0, pos);
}

bool Config::fileExists(const string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

string Config::getProperty(int iLine)
{
	ifstream myfile(CFG_PATH + "\\Config.txt");
	string line;
	size_t equals;
	int i = 0;
	while (getline(myfile, line))
	{
		if (i == iLine)
		{
			equals = line.find_first_of('=');
			break;
		}
		i++;
	}

	return line.substr(equals + 2, line.length() - equals - 2);
}
bool Config::isInt(string input)
{
	for (int i = 0; i < input.length(); i++)
	{
		char integer = input[i] - 48;
		if (integer < 48 || integer > 58)
			return false;
	}
	return true;
}