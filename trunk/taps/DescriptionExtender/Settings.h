#ifndef __SETTINGS_H
#define __SETTINGS_H


typedef struct
{
	bool insertSpace;
	bool addGenre;
} Settings;

extern Settings settings;

void OptionsMenu_Show();
void OptionsMenu_Close();
dword OptionsMenu_HandleKey( dword key, dword keyHW );

void Settings_Load();
void Settings_Save();
void Settings_Reset();


#endif
