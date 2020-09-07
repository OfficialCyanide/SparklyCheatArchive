#pragma once
#include "SDK.h"

class Config
{
public:
	float save_config = false;
	float reload_config = false;

	float reload_chatspam = false;

	bool welcome = false;

	void loadCfg();
	//void loadChatspam();
	void saveCfg();
	//void saveChatspam();

	void init();
	HMODULE hInstance = NULL;
private:
	string getDirectory();
#define CFG_PATH string(getDirectory() + "\\Sparkly Cheat settings")

	bool fileExists(const string& name);
	string getProperty(int iLine);
	bool isInt(string input);
};
extern Config gConfig;