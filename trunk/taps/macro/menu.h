#ifndef __MENU_H
#define __MENU_H

void NewMenu (char *title);
void ShowMenu ();
void DeleteMenu ();
void SetPreviousMenuOption ();
void SetNextMenuOption ();
int GetCurrentMenuOption();
void AddMenuLine(const char line[]);
void FreeMenuLines();

#endif

