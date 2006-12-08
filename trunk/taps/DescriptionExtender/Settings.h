#ifndef __SETTINGS_H
#define __SETTINGS_H


typedef struct
{
	bool insertSpace;
	bool addGenre;
} Options;

extern Options options;

void OptionsMenu_Show();
void OptionsMenu_Close();
dword OptionsMenu_HandleKey( dword key, dword keyHW );

void LoadSettings();
void SaveSettings();
void ResetSettings();


#endif
