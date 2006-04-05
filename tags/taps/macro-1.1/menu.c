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

#ifdef WIN32
#include "TAP_Emulations.h"
#endif

#include "tap.h"
#include "window.h"

static int _menuHndl;					//handle to our menu window

#define MAXLINES  20
#define MAXTITLE  32

static int menuLineCount = 0;		//number of lines in array (1-based)
static char *menuLines[MAXLINES];	//array of menu lines - need to be filled by caller
static char menuTitle[MAXTITLE];	//menu's title
static int maxMenuLineLen = 0;		//longest menu line length (in screen pixels)

void FreeMenuLines();

//------------------------------ NewMenu --------------------------------------
//
void NewMenu (char *title)
{
	strcpy(menuTitle, title);
	maxMenuLineLen = 0;
}

//------------------------------ ShowMenu --------------------------------------
//
void ShowMenu (int current)
{
	int i, width, left, height;

	//calculate size of window based on longest line and number of lines
	//add a bit for the scroll bar and window bits
	if ((width = maxMenuLineLen+20+20+20) > 500)
		width = 500;

	height = menuLineCount*22;
		
	//TAP_Print("Creating windows: maxlen: %d, width: %d, height: %d\r\n", maxMenuLineLen, width, height);

	_menuHndl = CreateWindow(menuTitle, (720-width)/2, (576-height)/2, width, height);

	for (i=0; i<menuLineCount; i++)
		AddWindowLine( _menuHndl, menuLines[i] );

	SetMinWindowLine( _menuHndl, 1);
	SetMaxWindowLine( _menuHndl, menuLineCount);
	if (current > 0)
		MoveToWindowLine( _menuHndl, current);
	else
		MoveToWindowLine( _menuHndl, menuLineCount);	//default to last line (usually Pass key on to next TAP)

	return;
}

//------------------------------ DeleteMenu --------------------------------------
//
void DeleteMenu ()
{
	DeleteWindow (_menuHndl);
	FreeMenuLines();
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
	int len;

	if (++menuLineCount > MAXLINES)
	{
		--menuLineCount;
		return;
	}

	len = strlen(line);

	if ( (p = (char *)malloc(len+1)) == NULL ) return;
	strcpy(p, line);
	menuLines[menuLineCount-1] = p;

	//calc screen width of line and if longest save it - remember we are using proportional fonts
	len = TAP_Osd_GetW(p, 0, FNT_Size_1622);
	if (len > maxMenuLineLen) maxMenuLineLen = len;

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
	menuLineCount = 0;
	maxMenuLineLen = 0;
}
