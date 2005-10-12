//------------------------------ Information --------------------------------------
//
// Tools
//
// Author:	various
// Description:
//
//			Several useful common functions
//---------------------------------------------------------------------------------


#include "tap.h"
#include "gcode.h"
#include "window.h"

#define MAXFILENAME 50

int _selectMode;


extern int _phase;
extern int _menuSel;					//current selection on the main menu

extern int _gcode;						//the current gcode value
extern char _gcodeStr[GCODEDIGITS+1];
extern int _currDigit;					//the digit on screen (0..5 0 is leftmost)
extern int _tchannel;					//current timer channel
extern int _tday;						//current timer day
extern int _tmonth;						//etc
extern int _tyear;						//
extern int _tstarttime;					//
extern int _tduration;					//
extern int _tuner;
extern int _reservation;				//one time reservation
extern char _filename[MAXFILENAME];		//recording's filename
extern int _keymapHndl;					//handle to our keymap
extern int _currentLine;

//default keys for different functions - can be overriden by keymap.ini
extern int _startGUIKey;
extern int _exitGUIKey;
extern int _exitTAPKey;
extern int _confirmKey;
extern int _numberUpKey;
extern int _numberDownKey;
extern int _numberRightKey;
extern int _numberLeftKey;

int _menuHndl;					//handle to our menu window
int _detailsHndl;				//handle to our details window
int _gcodeHndl;				//handle to our details window
int _rgn = 0;

//------------------------------ atoi --------------------------------------
//
int atoi(char s[])
{
	int i, n;

	n = 0;
	for (i=0; isspace(s[i]); i++)	//skip white space
		;

	for (n=0; isdigit(s[i]); i++)
	{
		n = 10 * n + (s[i] - '0');
	}
	return n;
}

//------------------------------ atoi --------------------------------------
//
char * DayOfWeek()
{

	static char day[3];
	word year;
	byte month, aday, weekday;

	//make a date in toppy mjd format
	//extract it out again, this time it gives us the day of the week

	if ( TAP_ExtractMjd( TAP_MakeMjd( (word) _tyear, (byte) _tmonth, (byte) _tday ), &year, &month, &aday, &weekday ) == 0 )
		weekday = 99;

	switch (weekday)
	{
	case 0:
		strcpy(day, "Mon");	
		break;
	case 1:
		strcpy(day, "Tue");	
		break;
	case 2:
		strcpy(day, "Wed");	
		break;
	case 3:
		strcpy(day, "Thu");	
		break;
	case 4:
		strcpy(day, "Fri");	
		break;
	case 5:
		strcpy(day, "Sat");	
		break;
	case 6:
		strcpy(day, "Sun");
		break;
	default:
		strcpy(day, "???");	
		break;
	}
	return day;

}

//------------------------------ ShowMenu --------------------------------------
//
void ShowMenu ()
{

	_menuHndl = CreateWindow("GCode Menu", 225, 120, 220, 120);
	AddWindowLine( _menuHndl, "Stop GCode Application" );
	AddWindowLine( _menuHndl, "Next TAP" );
	SetMinWindowLine( _menuHndl, 1);
	SetMaxWindowLine( _menuHndl, 2);
	MoveToWindowLine(_menuHndl, 2);

	_phase = _menuActive;
	return;
}

//------------------------------ DeleteMenu --------------------------------------
//
void DeleteMenu ()
{
	DeleteWindow (_menuHndl);
	_phase = _noScreens;
	return;
}

//------------------------------  --------------------------------------
//
void SetPreviousMenuOption ()
{
	PreviousWindowLine (_menuHndl);
	return;
}


//------------------------------  --------------------------------------
//
void SetNextMenuOption ()
{
	NextWindowLine (_menuHndl);
	return;
}

//------------------------------  --------------------------------------
//
int GetCurrentMenuLine ()
{
	return GetCurrentWindowLine (_menuHndl);
}

//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------

void HighlightGCode()
{
	int i = _currDigit;

	TAP_Osd_PutString( _rgn, 225+125, 170+4, 500, _gcodeStr, COLOR_Black, COLOR_None, 0, FNT_Size_1622, FALSE );
	TAP_Osd_PutString( _rgn, 225+125+(i*9), 170+4, 500, "_", COLOR_Black, COLOR_None, 0, FNT_Size_1622, FALSE );
	TAP_Print("gcode = '%s'\r\n", _gcodeStr);
}

//--------------------------------------------------------------------------------------
//	DrawGCode - draws the mini GCode window
//--------------------------------------------------------------------------------------

void DrawGCode( void )
{
	int i, y;

	TAP_Osd_FillBox( _rgn, X, Y, W, 35, COLOR_User13 );
	TAP_Osd_PutStringAf1926( _rgn, X+5, Y+4, -1, "G-Code", COLOR_Black, COLOR_None );

	TAP_Osd_PutStringAf1926( _rgn, X+125, Y+4, X+W-4, _gcodeStr, COLOR_DarkCyan, COLOR_None );
	//TAP_Osd_PutString( _rgn, 225+125, 170+4, 500, _gcodeStr, COLOR_Black, COLOR_None, 0, FNT_Size_1622, FALSE );
	HighlightItem();
}

//--------------------------------------------------------------------------------------
//	ExitGUI
//--------------------------------------------------------------------------------------

void ExitGUI( void )
{
	_phase = _noScreens;	//We're showing the mini window
	TAP_Osd_Delete(_rgn);	//Remove GUI region
	//DeleteGCode();
	TAP_EnterNormal();		//Allow other OSDs
}

//--------------------------------------------------------------------------------------
//	EnterGUI
//--------------------------------------------------------------------------------------

void EnterGUI( void )
{
	_gcode = 0;				//initialise gcode value
	strcpy(_gcodeStr, "      ");
	_currDigit = 0;
	_tuner = 1;
	_reservation = RESERVE_TYPE_Onetime;
	_phase = _gcodeShow;	//We're showing the mini window

	TAP_ExitNormal();		//disable other OSDs
	//_rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, OSD_Flag_Plane2 );	//FALSE
	_rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );
}

//------------------------------ ShowDetails --------------------------------------
//
void ShowDetails ()
{
	char hdr[50];

	_detailsHndl = CreateWindow("GCode Timer Details", 125, 120, 420, 220);

	TAP_SPrint(_filename, "Ch %d %02d:%02d %02d/%02d/%04d.rec", 
								_tchannel, _tstarttime/100, _tstarttime%100, _tday, _tmonth, _tyear ); 

	//TAP_Print("Ch: %d, Start: %d, Dur: %d, %02d/%02d/%02d\r\n", channel_ret, starttime_ret, duration_ret, day_ret, month_ret, year_ret);


	AddWindowLine( _detailsHndl, "Record                       On" );
	AddWindowLine( _detailsHndl, "Type                          TV" );
	TAP_SPrint(hdr, "Service                       %d. %s", _tchannel, GetChannelName(_tchannel)); 
	AddWindowLine( _detailsHndl, hdr );
	TAP_SPrint(hdr, "Tuner Select                Tuner %d", _tuner+1); 
	AddWindowLine( _detailsHndl, hdr );
	TAP_SPrint(hdr, "Mode                          %d [%s]", _reservation, GetReservationType(_reservation)); 
	AddWindowLine( _detailsHndl, hdr );
	TAP_SPrint(hdr, "Date                           %02d/%02d/%4d [-%s]", _tday, _tmonth, _tyear, DayOfWeek()); 
	AddWindowLine( _detailsHndl, hdr );
	TAP_SPrint(hdr, "Start Time                   %02d:%02d", _tstarttime/100, _tstarttime%100); 
	AddWindowLine( _detailsHndl, hdr );
	TAP_SPrint(hdr, "Duration                      %d min", _tduration); 
	AddWindowLine( _detailsHndl, hdr );
	TAP_SPrint(hdr, "File Name                    %s", _filename ); 
	AddWindowLine( _detailsHndl, hdr );
	AddWindowLine( _detailsHndl, "  OK to Save, Exit to Cancel" );

	SetMinWindowLine( _detailsHndl, 3);
	SetMaxWindowLine( _detailsHndl, 8);
	//MoveToWindowLine( _detailsHndl, 3);
	MoveToWindowLine( _detailsHndl, _currentLine);

	_phase = _gcodeDetails;
	return;
}


//------------------------------ DeleteDetails --------------------------------------
//
void DeleteDetails ()
{
	DeleteWindow (_detailsHndl);
	_phase = _noScreens;
	return;

}

//------------------------------  --------------------------------------
//
void SetPreviousDetailsOption ()
{
	PreviousWindowLine (_detailsHndl);
	_phase = _gcodeDetails;
	return;
}


//------------------------------  --------------------------------------
//
void SetNextDetailsOption ()
{
	NextWindowLine (_detailsHndl);
	_phase = _gcodeDetails;
	return;
}

//------------------------------  --------------------------------------
//
void ChangeDetailsOption (dword key)
{
	//inc or dec current option depending on what it is
	_phase = _gcodeDetails;
	_currentLine = GetCurrentWindowLine (_detailsHndl);

	switch (_currentLine)
	{
	case 3:	//channel
		if (key == _numberRightKey)
		{
			//change to next channel
			_tchannel = GetNextChannel(_tchannel);
		}
		ef (key == _numberLeftKey)
		{
			//change to previous channel
			_tchannel = GetPreviousChannel(_tchannel);
		}
		break;
	case 4: //tuner
		if (key == _numberRightKey)
		{
			if (++_tuner > 1) _tuner = 0;
		}
		ef (key == _numberLeftKey)
		{
			if (--_tuner < 0) _tuner = 1;
		}
		break;
	case 5: //reservation
		if (key == _numberRightKey)
		{
			//change to next reservation type
			if (++_reservation >= N_ReserveType)
				_reservation = RESERVE_TYPE_Onetime;

		}
		ef (key == _numberLeftKey)
		{
			//change to previous reservation type
			if (--_reservation < RESERVE_TYPE_Onetime)
				_reservation = RESERVE_TYPE_WeekDay;
		}
		break;
	case 6: //date
		break;
	case 7: //time
		break;
	case 8: //duration
		if (key == _numberRightKey)
		{
			_tduration = _tduration + 5;
		}
		ef (key == _numberLeftKey)
		{
			if (_tduration >= 10) _tduration = _tduration - 5;
		}
		break;
	default: //
		return;
	}

	//the only way to change data in a window is to reshow the whole window
	DeleteDetails();	//remove current window
	ShowDetails();		//show new data
	return;
}


//------------------------------ ShowMessage --------------------------------------
//
void ShowMessage(char *msg, int delay)
{
	int rgn; // Region number returned by TAP_Osd_Create
	int width;

	rgn = TAP_Osd_Create(0, 0, 720, 576, 0, FALSE); // Define a region that covers the whole screen
	width = TAP_Osd_GetW(msg, 0, FNT_Size_1926);
	TAP_Osd_FillBox(rgn,((720-width)/2)-50,(576/2)-25,width+100,50,COLOR_DarkGray);
	TAP_Osd_PutS(rgn, (720-width)/2, (576/2)-14, -1, msg, COLOR_White, COLOR_DarkGray, 0, FNT_Size_1926, FALSE, ALIGN_LEFT);
	TAP_Osd_FillBox(rgn,0,0,1,1,COLOR_None); // This seems to be necessary to prevent the box crashing. Why? I don't know...
	TAP_Delay(delay);
	TAP_Osd_Delete(rgn); // Delete the region, and therefore clear anything we have printed within it

}

