#ifndef __WINDOW_H
#define __WINDOW_H

int CreateWindow (const char *title, dword x, dword y, dword w, dword h);
int DeleteWindow (int w);
void AddWindowLine(int w, char *line);
void NextWindowLine(int w);
void PreviousWindowLine(int w);
void MoveToWindowLine(int w, int l);
void SetMinWindowLine(int w, int l);
void SetMaxWindowLine(int w, int l);
int GetCurrentWindowLine(int w);
void PutStringToWindow(int w, dword x, dword y, dword maxX, const char *str, dword fcolor, dword bcolor);

#endif
