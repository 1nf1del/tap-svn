//------------------------------ Information --------------------------------------
//
// Menu
//
// Author:	John De Angelis
// Description:
//			Menu tool based on window kit
// Version: 1.1
//
//---------------------------------------------------------------------------------

#include "tap.h"
#include "window.h"

static int _menuHndl;					//handle to our menu window

#define MAXLINES  20
#define MAXTITLE  32

static int menuLineCount = 0;		//number of lines in array (1-based)
static char *menuLines[MAXLINES];	//array of menu lines - need to be filled by caller
static char menuTitle[MAXTITLE];	//menu's title


//------------------------------ NewMenu --------------------------------------
//
void NewMenu (char *title)
{
	strcpy(menuTitle, title);
}

//------------------------------ ShowMenu --------------------------------------
//
void ShowMenu ()
{
	int i;

	//todo - calculate size of window based on longest line and number of lines
	_menuHndl = CreateWindow(menuTitle, 225, 120, 220, 120);

	for (i=0; i<menuLineCount; i++)
		AddWindowLine( _menuHndl, menuLines[i] );

	SetMinWindowLine( _menuHndl, 1);
	SetMaxWindowLine( _menuHndl, menuLineCount);
	MoveToWindowLine( _menuHndl, menuLineCount);	//default to last line (usually Pass key on to next TAP)

	return;
}

//------------------------------ DeleteMenu --------------------------------------
//
void DeleteMenu ()
{
	DeleteWindow (_menuHndl);
	return;
}

//------------------------------ SetPreviousMenuOption --------------------------------------
//
void SetPreviousMenuOption ()
{
	PreviousWindowLine (_menuHndl);
	return;
}


//------------------------------ SetNextMenuOption --------------------------------------
//
void SetNextMenuOption ()
{
	NextWindowLine (_menuHndl);
	return;
}

//------------------------------ GetCurrentMenuOption --------------------------------------
//
int GetCurrentMenuOption()
{
	return GetCurrentWindowLine(_menuHndl);
}

//------------------------------ AddMenuLine --------------------------------------
//
void AddMenuLine(const char line[])
{
	char *p;
	if (++menuLineCount > MAXLINES)
	{
		--menuLineCount;
		return;
	}

	if ( (p = (char *)malloc(strlen(line)+1)) == NULL ) return;
	strcpy(p, line);
	menuLines[menuLineCount-1] = p;
	return;
}

//------------------------------ FreeMenuLines --------------------------------------
//
void FreeMenuLines()
{
	int i;
	//free menu lines
	for (i=1; i<menuLineCount; i++)
	{
		free((char *)menuLines[i-1]);
	}
}
