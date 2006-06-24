#include <stdlib.h>
#include <string.h>
#include "tap.h"
#include "keymap.h"
#include "tools.h"

#define ID_LIST 0x80000104

TAP_ID( ID_LIST );
TAP_PROGRAM_NAME("List Sample");
TAP_AUTHOR_NAME("deangelj");
TAP_DESCRIPTION("A sample tap that uses the keymap package.");
TAP_ETCINFO(__DATE__);

volatile int _exitFlag = 0;

int selectMode;
TYPE_Window _win;
int _rgn;
bool bmapLoaded = FALSE;
int hndl;

#define printf TAP_Print


//---------------------------------------  ShowMessageWin --------------------------------
//
void ShowMessageWin (char* lpMessage, char* lpMessage1, dword dwDelay)
{
	dword rgn;															// stores rgn-handle for osd
	dword w;															// stores width of message-text

	rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );					// create rgn-handle
	//rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, OSD_Flag_Plane2 );					// create rgn-handle
	w = TAP_Osd_GetW( lpMessage, 0, FNT_Size_1926 ) + 10;				// calculate width of message
	if (TAP_Osd_GetW( lpMessage1, 0, FNT_Size_1926 ) + 10>w)
		w = TAP_Osd_GetW( lpMessage1, 0, FNT_Size_1926 ) + 10;			// if second message is larger, calculate new width of message
	
	if (w > 720) w = 720;												// if message is to long
	TAP_Osd_FillBox(rgn, (720-w)/2-5, 265, w+10, 62, RGB(19,19,19) );	// draw background-box for border
		
	TAP_Osd_PutS(rgn, (720-w)/2, 270, (720+w)/2, lpMessage,		
		RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926,
		FALSE, ALIGN_CENTER);											// show 1. message
	TAP_Osd_PutS(rgn, (720-w)/2, 270+26, (720+w)/2, lpMessage1,	
		RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926,
		FALSE, ALIGN_CENTER);											// show 2. message
	TAP_Delay(dwDelay);													// show it for dwDelay /100 seconds
	TAP_Osd_Delete(rgn);												// release rgn-handle
}




void ShowFileList( dword selNum )
{
	dword totalEntry;
	TYPE_File file;

	//ShowMessageWin("Showing the list", "", 300);

	TAP_ExitNormal();

	_rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );

	TAP_Win_SetDefaultColor( &_win );
	TAP_Win_Create( &_win, _rgn, 100, 100, 200, 200, FALSE, TRUE );
	TAP_Win_SetTitle( &_win, "Files", 0, FNT_Size_1622 );

	// find the files...
	totalEntry = TAP_Hdd_FindFirst( &file );
	TAP_Print("TotalEntries = %d\r\n", totalEntry);

	while( totalEntry-- )
	{
		if (file.attr==ATTR_NORMAL||file.attr==ATTR_TS||file.attr==ATTR_PROGRAM||file.attr==ATTR_FOLDER||file.attr==240)
		{
			TAP_Win_AddItem( &_win, file.name );
		}
		// Size is always 200
		// CurrentPos = 0 or 1
		// Crypt = 0
		// Skip = 2 = locked, = 255 for ..
		// Attr = 208 normal (tap, non rec)
		// StartCluster = #  TotalCluster = #

		//TAP_Print("Name: %s, StartCluster: %d, TotalCluster: %d, PlayLst: %d\r\n", file.name, file.startCluster, file.totalCluster, file.playLst);
		TAP_Print("Name: %s, Attr: %d, Crypt: %d, Skip: %d\r\n", file.name, file.attr, file.crypt, file.skip);
		// Attr:  241 = .  240 = .. __temprec__.ss = 208 (ATTR_NORMAL) __temprec__.ts = 209  UserDir = 242 (ATTR_FOLDER) .rec = 209 (ATTR_TS) 

		if( totalEntry==0 || TAP_Hdd_FindNext( &file ) == 0 )
		{
			break;
		}
	}

    TAP_Win_SetSelection( &_win, selNum );
}


dword TAP_EventHandler( word event, dword param1, dword param2 )
{
    static word fileRgn;
	static int selNum, i;
	static char fileName[256];

    if( event != EVT_KEY )
        return param1;

	if( selectMode == 1 )
	{
		if( param1 == getLogicalKeyVal("ExitKey", hndl))
		{
			TAP_Print("Deleting window/region.....\r\n");
			TAP_Win_Delete( &_win );
			TAP_Osd_Delete( _rgn );
			_exitFlag = 1;
			TAP_EnterNormal();
		}
		else if( param1 == getLogicalKeyVal("OKKey", hndl))
		{
			word wRet;

			selNum = TAP_Win_GetSelection( &_win );

			i = 0;
			while(1)
			{
				fileName[i] = _win.item[selNum][i];
				if( fileName[i]==0 ) break;
				i++;
			}

			//if its a directory then chdir to it and do another file list
			if ((wRet = TAP_Hdd_ChangeDir(fileName))==0)
			{
				//ShowMessageWin("Picked dir", fileName, 300);
				TAP_Print("Deleting window/region.....\r\n");
				TAP_Win_Delete( &_win );
				TAP_Osd_Delete( _rgn );
				ShowFileList(0);
			}
			else
				TAP_Print("Picked file %s\r\n", fileName);
				//ShowMessageWin("Picked file", fileName, 300);
		}
		else
		{
			TAP_Win_Action( &_win, param1 );
		}
	}

	return 0;
}

int TAP_Main(void)
{
	int ret;

	ShowMessageWin("Loading keymap from list.ini", "", 300);

	initKeyMap(_ERROR);

	if (hndl = loadKeyMap("list", "list.ini"))
	{
		ShowFileList( 0 );
		
		selectMode = 1;
		while(1)
		{
			TAP_SystemProc();
			if( _exitFlag ) 
			{
				break;
			}
		}
		freeMap(hndl);
	}
	else
	{
		char err[2048];
		TAP_SPrint(err, "LoadKeymap failed. Code: %d, Msg: %s", GetLastErrorCode(), GetLastErrorString());
		ShowMessageWin(err, "", 500);
	}
	return 0;
}
