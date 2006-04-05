//------------------------------ Information --------------------------------------
//
// Windowing package
//
// Author:	John De Angelis
// Description:
// Version: 1.1
//
//---------------------------------------------------------------------------------

#ifdef WIN32
#include "TAP_Emulations.h"
#endif

#include "tap.h"
#include "window.h"

#define MAXWINDOWS 50

typedef struct _window
{
	word rgn;				//OSD rgn handle
	TYPE_Window win;		//Win handle
	int currentLine;		//current line in the display (1 based)
	int minLine;			//max line (ie. wrap around after this) (1 based)
	bool minSet;			//true if set by API
	int maxLine;			//min line (ie. wrap around before this) (1 based)
	bool maxSet;
	int lineCount;			//no of lines in the display
} WINDOW;

static WINDOW *_windows[MAXWINDOWS];	//window list
static int _windowCount = 0;			//#of windows

//----------------------------------------------------------------------------------
// CreateWindow - creates a window and returns a handle to the window
//----------------------------------------------------------------------------------

int CreateWindow (const char *title, dword x, dword y, dword w, dword h)
{
	int ret;
	WINDOW *win;

	TAP_ExitNormal();	//stop other screen updates happening
	win = (WINDOW *)malloc(sizeof(WINDOW));
	win->rgn = TAP_Osd_Create ( 0, 0, 720, 576, 0, FALSE );	//full screen

	TAP_Win_SetDefaultColor ( &win->win );
	TAP_Win_Create ( &win->win, win->rgn, x, y, w, h, FALSE, TRUE );
	TAP_Win_SetTitle ( &win->win, title, 0, FNT_Size_1622 );
	win->currentLine = 0;
	win->lineCount = 0;
	win->minSet = FALSE;
	win->maxSet = FALSE;
	win->minLine = 0;
	win->maxLine = 0;

	ret = _windowCount++;
	_windows[ret] = win;
	
	return ret;

}

//----------------------------------------------------------------------------------
// DeleteWindow - deletes the window from our list.
//
// Returns window handle (ie >=0) if ok.
// -ve returns for errors
//----------------------------------------------------------------------------------

int DeleteWindow (int w)
{
	WINDOW *win;

	if (w >= _windowCount)
	{
		return -1;  //invalid window handle
	}
	ef (_windows[w] == NULL)
	{
		return -2;	//invalid - using a free'd handle
	}
	win = _windows[w];
	TAP_Win_Delete ( &win->win );
	TAP_Osd_Delete ( win->rgn );
	TAP_EnterNormal();

	free(win);
	_windows[w] = NULL;
	--_windowCount;
	return w;

}

//----------------------------------------------------------------------------------
// AddWindowLine - Adds a line to our window
//----------------------------------------------------------------------------------

void AddWindowLine(int w, char *aline)
{
	if (aline)
	{
		WINDOW *win = _windows[w];
		TAP_Win_AddItem ( &win->win, aline );
		++win->lineCount;
		if (!win->maxSet) win->maxLine = win->lineCount;
		if ((!win->minSet) && (win->minLine == 0)) win->minLine = 1;
	}
}

//----------------------------------------------------------------------------------
// NextWindowLine - Moves to the next line to our window
//----------------------------------------------------------------------------------

void NextWindowLine(int w)
{
	WINDOW *win = _windows[w];

	//TAP_Print("NextWindowLine...\r\n");
	//TAP_Print("...maxSet: %s, minSet: %s...\r\n", win->maxSet?"TRUE":"FALSE", win->minSet?"TRUE":"FALSE");
	//TAP_Print("...lineCount: %d, currentLine: %d...\r\n", win->lineCount, win->currentLine);
	//TAP_Print("...minLine: %d, maxLine: %d...\r\n", win->minLine, win->maxLine);

	if (win->lineCount <= 0) return;	//invalid

	++win->currentLine;

	if (win->maxSet)
	{
		if (win->currentLine > win->maxLine) win->currentLine = win->minLine;
	}
	else //wrap on lineCount
	{
		if (win->currentLine > win->lineCount) win->currentLine = win->minLine;
	}
	MoveToWindowLine( w, win->currentLine );
}

//----------------------------------------------------------------------------------
// PreviousWindowLine - Moves to the previous line to our window
//----------------------------------------------------------------------------------

void PreviousWindowLine(int w)
{
	WINDOW *win = _windows[w];

	//TAP_Print("PreviousWindowLine...\r\n");
	//TAP_Print("...maxSet: %s, minSet: %s...\r\n", win->maxSet?"TRUE":"FALSE", win->minSet?"TRUE":"FALSE");
	//TAP_Print("...lineCount: %d, currentLine: %d...\r\n", win->lineCount, win->currentLine);
	//TAP_Print("...minLine: %d, maxLine: %d...\r\n", win->minLine, win->maxLine);

	--win->currentLine;

	if (win->minSet)
	{
		if (win->currentLine < win->minLine) win->currentLine = win->maxLine;
	}
	else //wrap on 0
	{
		if (win->currentLine < 1) win->currentLine = 1;
	}
	MoveToWindowLine( w, win->currentLine );

}

//----------------------------------------------------------------------------------
// MoveToWindowLine - Moves to the indicated line to our window (1 based)
//----------------------------------------------------------------------------------

void MoveToWindowLine(int w, int line)
{
	WINDOW *win = _windows[w];

	//TAP_Print("Moving to line %d...\r\n", line);

	TAP_Win_SetSelection ( &win->win, line - 1 );
	win->currentLine = line;

}

//----------------------------------------------------------------------------------
// SetMinWindowLine - sets the min line user can move to
//----------------------------------------------------------------------------------

void SetMinWindowLine(int w, int line)
{
	WINDOW *win = _windows[w];

	if ((line < 1) || (line > win->lineCount)) return;  //outside lineCount range

	if ((win->maxSet) && (line > win->maxLine)) return;  //greater than maxline

	win->minLine = line;
	win->minSet = TRUE;

}

//----------------------------------------------------------------------------------
// SetMaxWindowLine - sets the max line user can move to
//----------------------------------------------------------------------------------

void SetMaxWindowLine(int w, int line)
{
	WINDOW *win = _windows[w];

	if ((line < 1) || (line > win->lineCount)) return;  //outside lineCount range

	if ((win->minSet) && (line < win->minLine)) return;  //les than minline

	win->maxLine = line;
	win->maxSet = TRUE;

}

//----------------------------------------------------------------------------------
// GetCurrentWindowLine
//----------------------------------------------------------------------------------

int GetCurrentWindowLine(int w)
{
	WINDOW *win = _windows[w];

	return win->currentLine;
}

//----------------------------------------------------------------------------------
// PutStringToWindow
//----------------------------------------------------------------------------------

void PutStringToWindow(int w, dword x, dword y, dword maxX, const char *str, dword fcolor, dword bcolor)
{
	WINDOW *win = _windows[w];

	TAP_Osd_PutStringAf1926( win->rgn, x, y, maxX, str, fcolor, bcolor );
	//TAP_Osd_PutStringAf( win->rgn, x, y, maxX, str, fcolor, bcolor );
}
