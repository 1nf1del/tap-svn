//------------------------------ Information --------------------------------------
//
// GCODE
//
// Author:	John De Angelis  email: j_de_angelis@hotmail.com
// Date:	4 September 2004
// Version:	1.0 - 04/09/04 - initial release
//			1.1 - 07/09/04
//				- compare with global key vals instead of getLogical each time
//				- freeMap on exit
//				- display version on startup
//			1.2 - fix bug in gcfuncs - use 2 digit year instead of 4 digit
//			1.3 - 09/09/04
//				- fix bug where if year is 2 digit timeradd says it adds ok but doesn't
//				- properly map channel to service number when adding timer
//			1.4 - since channel names are different between different locations, added
//				  option to load channels from an options file
//				- added menu for exiting TAP to pass on the exit key as per std
//			1.5 - 21/9/04
//				- enhance menu to display timer details and allow some modifications
//				- make the gcode entry easier
//				- create timer with a filename so timer_extend works
//			1.51 - 22/9/04
//				- include keymap.ini with StartGUI = Exit instead of the '0' key
//				- faster i/o to ini file using optimisations
//			1.6 - 18/10/04
//				- fix bug in left/right in duration
//				- fix bug in getting currentline in changedetails screen
//
// Description:
//			This TAP is a TSR that emulates the GCODE functionality of VCRs.
//			GCODEs can be found for most TV programs in printed TV Guides.
//			Enter 1 to 6 digits, then press OK to confirm or Exit to cancel.
//			On confirmation, the TAP will convert the code to timer details and
//			display for confirmation on the screen. You will optionally be able to
//			change the tuner and the filename. Press OK to confirm and the timer
//			will be created.
//
// NB:		This TAP will ONLY funtion with GCODEs of 1-6 characters. G-codes can
//			theoretically go up to 8 digits...
//			Note to our European brethren: G-code is the same as Showview, so this
//			should also work for your Showview codes.
//
// Default Key Assignments: (override with keymapper)
//		Guide:		Brings up the GCODE screen
//		0:			Exits the TAP
//		OK:			Selects things
//		Exit:		Quits things
//		< and >:	Moves along the Gcode display
//		^ and v:	Increments or decrements the current digit in the Gcode display
//
//---------------------------------------------------------------------------------

#include <math.h>	//pow function
#include <string.h>	//strcmpi

#include "tap.h"
#include "gcode.h"
#include "gcfuncs.h"
//#include "Utils.h"
#include "keymap.h"

#define ID_GCODE 0x80000102
#define PROGRAMNAME "GCode"

//------------------------------ TAP Required --------------------------------------
//
TAP_ID( ID_GCODE );
TAP_PROGRAM_NAME( PROGRAMNAME );
TAP_AUTHOR_NAME("deangelj");
TAP_DESCRIPTION("Create timer entries using g-code");
TAP_ETCINFO(__DATE__);

//------------------------------ Statics --------------------------------------
//
static word _rgn;					//the window handle

//------------------------------ Global Variables --------------------------------------
//
//------------------------------ Global Variables --------------------------------------
//
int _gcode = 0;						//the current gcode value
char _gcodeStr[GCODEDIGITS+1];
//int _currDigit = GCODEDIGITS - 1;	//the digit on screen (5..0 5 is leftmost)
int _currDigit = 0;					//the digit on screen (0 is leftmost)
int _tchannel;						//current timer channel
int _tday;							//current timer day
int _tmonth;						//etc
int _tyear;							//
int _tstarttime;					//
int _tduration;						//
int _tuner = 1;						//tuner 2
int _reservation = RESERVE_TYPE_Onetime;	//one time reservation
char _filename[MAXFILENAME];		//recording's filename
int _keymapHndl;					//handle to our keymap
int _menuSel;						//current selection on the main menu

//default keys for different functions - can be overriden by keymap.ini
int _startGUIKey	= RKEY_Guide;
int _exitGUIKey		= RKEY_Exit;
//int _exitTAPKey		= RKEY_0;
int _exitTAPKey		= RKEY_Exit;
int _confirmKey		= RKEY_Ok;
int _numberUpKey	= RKEY_ChUp;
int _numberDownKey	= RKEY_ChDown;
int _numberRightKey	= RKEY_VolUp;
int _numberLeftKey	= RKEY_VolDown;

//initial phase
int _phase = _noScreens;
int _currentLine;

CHANNEL *_channelTbl[MAXCHANNELS];
int _channelCount = 0;



static int _minLCN;
static int _maxLCN;


//------------------------------ Functions --------------------------------------
//


//--------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------

#define MAX_RESERVATION sizeof("EveryWeekend")

char *GetReservationType(int ch)
{
	static char resName[MAX_RESERVATION];

	switch (ch)
	{
	case RESERVE_TYPE_Onetime:
		strcpy(resName, "Onetime");
		break;
	case RESERVE_TYPE_Everyday:
		strcpy(resName, "Everyday");
		break;
	case RESERVE_TYPE_EveryWeekend:
		strcpy(resName, "EveryWeekend");
		break;
	case RESERVE_TYPE_Weekly:
		strcpy(resName, "Weekly");
		break;
	case RESERVE_TYPE_WeekDay:
		strcpy(resName, "WeekDay");
		break;
	default: 
		strcpy(resName, "Unknown");
		break;
	}

	return resName;
}


//--------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------

char *GetChannelName(int ch)
{
	static char chName[MAX_SvcName];
	int i;

	for (i=0; i<_channelCount; i++)
	{
		if (_channelTbl[i]->lcn == ch)
		{
			strcpy(chName, _channelTbl[i]->chName);
			return chName;
		}
	}
	strcpy(chName, "[none]");
	return chName;
}


//--------------------------------------------------------------------------------------
//	GetNextChannel - get the next numerical lcn in our table
//--------------------------------------------------------------------------------------

int GetNextChannel(int ch)
{
	int nextCh = _maxLCN;
	int i;

	//if highest channel goto first
	if (ch == _maxLCN) return _minLCN;

	//get the next lcn
	for (i=0; i<_channelCount; i++)
	{
		if ((_channelTbl[i]->lcn > ch) && (_channelTbl[i]->lcn < nextCh))
		{
			nextCh = _channelTbl[i]->lcn;
		}
	}
	return nextCh;
}


//--------------------------------------------------------------------------------------
//	GetPreviousChannel - get the previous numerical lcn in our table
//--------------------------------------------------------------------------------------

int GetPreviousChannel(int ch)
{
	int prevCh = _minLCN;
	int i;

	//if lowest channel goto last
	if (ch == _minLCN) return _maxLCN;

	//get the previous lcn
	for (i=0; i<_channelCount; i++)
	{
		if ((_channelTbl[i]->lcn < ch) && (_channelTbl[i]->lcn > prevCh))
		{
			prevCh = _channelTbl[i]->lcn;
		}
	}
	return prevCh;
}


//--------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------

void SetupChannelTable()
{
	TYPE_TapChInfo chInfo;
	int i, j, r, t;
	bool found;

	int length;
	char buffer[MAX_STR_LEN];
	char* nextChannel;
	char iniBuff[MAX_INI];
	CHANNEL *ch;


	//load the channels from gcode.ini
	//count in _channelCount
	
	buffer[0]='\0';

	length = GetPrivateProfileString("Channels", (char *)NULL, (char*)NULL, buffer, MAX_STR_LEN, OPTIONSFILE);
	if (length==0) 
	{
		//fatal!
		return ;
	}

	_minLCN = 1000;  //use something bigger than the biggest
	_maxLCN = 0;
	//for each logical key get the value - which is the actual key for the TAP
	nextChannel = buffer;  // final dir is marked with double null
	while(strlen(nextChannel)>0)
	{

		if (GetPrivateProfileString("Channels", nextChannel, "", iniBuff, MAX_INI, OPTIONSFILE) > 0)
		{
			ch = (CHANNEL *) malloc(sizeof(CHANNEL));
			ch->lcn = atoi(nextChannel);
			strcpy(ch->chName, iniBuff);
			ch->svcNum = -1;  //ie no value
			_channelTbl[_channelCount++] = ch;
			//setup min and max lcn for later use
			if (ch->lcn < _minLCN) _minLCN = ch->lcn;
			if (ch->lcn > _maxLCN) _maxLCN = ch->lcn;
			TAP_Print("Added channel %d, %s\n", ch->lcn, ch->chName);
		}
		nextChannel += strlen(nextChannel) + 1;
	}

	TAP_Channel_GetTotalNum( &t, &r );
	for (i=0; i<_channelCount; i++)
	{
		found = FALSE;

		//scroll through the channel info and save the svc numbers for when we add timers
		for (j=0; j<t; j++)
		{
			TAP_Channel_GetInfo( SVC_TYPE_Tv, j, &chInfo );
			if ( strcmpi(chInfo.chName, _channelTbl[i]->chName) == 0 )
			{
				//found it
				_channelTbl[i]->svcNum = j;
				TAP_Print("Ch: %d, Name: %s, SvcNum: %d\r\n", _channelTbl[i]->lcn, _channelTbl[i]->chName, j);
				found = TRUE;
				break;
			}
		}

		if (!found)
		{
			TAP_Print("Couldn't find service for ch: %s\r\n", _channelTbl[i]->chName);
		}
	}

}

//--------------------------------------------------------------------------------------
//	Do the startup stuff here - incl loading the keymap file, options file
//--------------------------------------------------------------------------------------

int Startup()
{
	char *key;
	int keyVal;
	
	//	Show something if we have been manually started, ie after a reasonable start time
	if ( TAP_GetTick() > REASONABLE_START_TIME*60*100 )  //in 10ms clicks
	{
		char msg[50];
		sprintf(msg, "Starting G-Code TAP %s...", GCODEVERSION);
		ShowMessage(msg, 200);
	}

	/* Need to init the package before use - only show errors/fatals */
	initKeyMap(_ERROR);

	//Add the service numbers to the channel table
	//Also need to init the Utils package before doing this. InitKeymap above does this
	SetupChannelTable();

	/* Load the keymap structures from the ini file */
	if ((_keymapHndl = loadKeyMap(PROGRAMNAME, "keymap.ini")) == 0)
	{
		//error - either keymap.ini isn't there, or there's no section for gcode - show a message
		//out the serial port
		TAP_Print("LoadKeyMap failed. Code: %d, Msg: %s\r\n", GetLastErrorCode(), GetLastErrorString());
	}
	else
	{
		//Get the key mappings from the keymap file. If we get any sort of error use defaults
		int tmp;
		if ((tmp = getLogicalKeyVal("StartGUI", _keymapHndl)) > 0) _startGUIKey = tmp;
		if ((tmp = getLogicalKeyVal("ExitGUI", _keymapHndl)) > 0) _exitGUIKey = tmp;
		if ((tmp = getLogicalKeyVal("ExitTAP", _keymapHndl)) > 0) _exitTAPKey = tmp;
		if ((tmp = getLogicalKeyVal("Confirm", _keymapHndl)) > 0) _confirmKey = tmp;
		if ((tmp = getLogicalKeyVal("NumberUp", _keymapHndl)) > 0) _numberUpKey = tmp;
		if ((tmp = getLogicalKeyVal("NumberDown", _keymapHndl)) > 0) _numberDownKey = tmp;
		if ((tmp = getLogicalKeyVal("NumberRight", _keymapHndl)) > 0) _numberRightKey = tmp;
		if ((tmp = getLogicalKeyVal("NumberLeft", _keymapHndl)) > 0) _numberLeftKey = tmp;
	}

	//Get the start key name
	if (key = getKeyName(_startGUIKey))
	{
		if ( TAP_GetTick() > REASONABLE_START_TIME*60*100 )  //in 10ms clicks
		{

			char keyend[20];
			char msg[50];

			strcpy(keyend, &key[5]);  //remove RKEY_
			sprintf(msg, "G-Code TAP started - Press %s to Display Menu", keyend);
			ShowMessage(msg, 200);
		}
	}
	else
	{
		ShowMessage("Can't find startGUI key", 300);
		return 1;
	}
	return 0;
}

//--------------------------------------------------------------------------------------
//	get the gcode details and show on the screen
//--------------------------------------------------------------------------------------

void ShowGCodeDetails()
{
	int ret, day_ret, month_ret, year_ret, channel_ret, starttime_ret, duration_ret, y, i;
	char str[100];

	_gcode = atoi(_gcodeStr);
	if (decode(_gcode, &day_ret, &month_ret, &year_ret, &channel_ret, &starttime_ret, &duration_ret) == 0)
	{
		//display error message on screen
		ShowMessage("Error decoding g-code - please check the number and try again", 100);
		return;
	}

	//Save the decoded settings in our global variables so we can access it later
	_tchannel = channel_ret;
	_tday = day_ret;
	_tmonth = month_ret;
	_tyear = year_ret;
	_tstarttime = starttime_ret;
	_tduration = duration_ret;

	_currentLine = 3;  //top line
	ShowDetails();

	//set phase so that if we get ok/cancel we know what to do in event handler
	_phase = _gcodeDetails;

}

//--------------------------------------------------------------------------------------
//	SetTimerDetails - creates a timer entry using the current details
//--------------------------------------------------------------------------------------

void SetTimerDetails()
{
	TYPE_TimerInfo timerInfo;
	int retval, i;
	bool found;

    memset( &timerInfo, 0, sizeof(TYPE_TimerInfo) );

	timerInfo.isRec = 1;  //Recording
	timerInfo.tuner = _tuner;  //tuner2
	timerInfo.svcType = SVC_TYPE_Tv;

	//find the channel's svcNum
	found = FALSE;
	for (i=0; i<_channelCount; i++)
	{
		if (_channelTbl[i]->lcn == _tchannel)
		{
			timerInfo.svcNum = (word)_channelTbl[i]->svcNum;
			//TAP_Print("Adding svc: %d for channel: %d\r\n", _channelTbl[i]->svcNum, _tchannel);
			found = TRUE;
			break;
		}
	}
	if (!found)
	{
		char msg[100];
		sprintf(msg, "Can't find channel %d in service number table", _tchannel);
		ShowMessage(msg, 300);
		return;
	}

	//timerInfo.reservationType = RESERVE_TYPE_Onetime;	//?One time versus recurring
	timerInfo.reservationType = _reservation;
	//timerInfo.nameFix = 1;	//to fix the name
	strcpy( timerInfo.fileName, _filename );
	timerInfo.duration = _tduration;

	timerInfo.startTime = (TAP_MakeMjd( (word)_tyear, (byte)_tmonth, (byte)_tday ) << 16)|((_tstarttime/100)<<8)|(_tstarttime%100);

	retval = TAP_Timer_Add(&timerInfo);
	if (retval == 0) {
		ShowMessage("Successfully added timer", 100);
	} else {
		char msg[100];
		if (retval == 1)
		{
			ShowMessage("Error Adding Entry! (Invalid Entry or no resource available)", 300);
		}
		ef (retval == 2)
		{
			ShowMessage("Error Adding Entry! (Invalid Tuner)", 300);
		}
		ef (retval >= 0xFFFF0000) 
		{
			TAP_SPrint(msg,"Error Adding Entry! (Conflicts with entry %d)",(retval & 0x00000FFF)+1);
			ShowMessage(msg,300);
		}
		else
		{
			ShowMessage("Error Adding Entry! (Unknown Error)", 300);
		}
	}

}

//--------------------------------------------------------------------------------------
//	GoLeft - goes left one
//--------------------------------------------------------------------------------------

void GoLeft()
{
	//delete item to left
	//if on the last digit just clear the current one
	TAP_Print("Current digit: %d, gcode = '%s'\r\n", _currDigit, _gcodeStr);

	if ((_currDigit >= GCODEDIGITS - 1) && (_gcodeStr[_currDigit] != ' '))
		_gcodeStr[_currDigit] = ' ';
	else if (_currDigit > 0)
		_gcodeStr[--_currDigit] = ' ';
		//_gcodeStr[_currDigit--] = ' ';
}

//--------------------------------------------------------------------------------------
//	GoRight - goes right one
//--------------------------------------------------------------------------------------

void GoRight()
{
	if (_currDigit < (GCODEDIGITS-1))
		_gcodeStr[_currDigit++] = ' ';
}

//--------------------------------------------------------------------------------------
//	HighlightItem - puts underscore(s) under the current item
//--------------------------------------------------------------------------------------

void HighlightItem()
{
	//int i = _currDigit + 1;
	int i = _currDigit;
	TAP_Osd_PutStringAf1926( _rgn, X+125+(i*11), Y+4, X+W-4, "_", COLOR_Black, COLOR_None );
}

//--------------------------------------------------------------------------------------
//	ExitTAP
//--------------------------------------------------------------------------------------

void ExitTAP( void )
{
	if (_keymapHndl) freeMap(_keymapHndl);
	ShowMessage("Exiting G-Code TAP...", 100);
	TAP_Exit();
}

//--------------------------------------------------------------------------------------
//	TAP_EventHandler - callback required to handle events
//--------------------------------------------------------------------------------------

dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	int r, t, pwr, dig;
	dword mainState, subState;

    if( event != EVT_KEY )
        return param1;
    
	TAP_GetState( &mainState, &subState );
    
    if( mainState != STATE_Normal )
    {
        return param1;
    }

	switch (_phase)
	{
	case _noScreens:
		//if( param1 == RKEY_Guide )
		if( param1 == _startGUIKey )
		{
			//enter gcode gui state
			EnterGUI();
			DrawGCode();
			//HighlightGCode();
			return 0;
		}
        //ef( param1 == RKEY_0 )
        ef( param1 == _exitTAPKey )
    	{
			//show exit menu
			ShowMenu();
    		return 0;
    	}
		break;
	

	//Add code to handle number keys
	case _gcodeShow:
		if ( (param1 == _exitGUIKey) || (param1 == _startGUIKey) )
		{
			//back to normal state
			ExitGUI();
			//DeleteGCode();
			return 0;
		}
		ef ( param1 == _confirmKey )
		{
			//get the gcode details and show on the screen
			ExitGUI();
			ShowGCodeDetails();
			return 0;
		}
		ef ( param1 == _numberRightKey )
		{
			GoRight();
			DrawGCode();
			//HighlightGCode();
			return 0;
		}
		ef ( param1 == _numberLeftKey )
		{
			GoLeft();
			DrawGCode();
			//HighlightGCode();
			return 0;
		}
		ef ( param1 >= RKEY_0 && param1 <= RKEY_9 )
		{
			//set current digit to (param1 - 0x10000)
			_gcodeStr[_currDigit] = (param1 - 0x10000) + 48;
			if (_currDigit < GCODEDIGITS - 1)
				_currDigit++;
			//HighlightGCode();
			DrawGCode();

			return 0;
		}
		break;
	
	case _gcodeDetails:
		if ( param1 == _confirmKey )
		{
			//we've showed the timer details and they've ok'd - now set the timer
			SetTimerDetails();
			//back to normal state
			DeleteDetails();
			return 0;
		}
		ef ( (param1 == _exitGUIKey) || (param1 == _startGUIKey) )
		{
			//back to normal state
			DeleteDetails();
			return 0;
		}
		ef ( param1 == _numberUpKey )
		{
			SetPreviousDetailsOption();
			return 0;
		}
		ef ( param1 == _numberDownKey )
		{
			SetNextDetailsOption();
			return 0;
		}
		ef ( (param1 == _numberLeftKey) || (param1 == _numberRightKey) )
		{
			ChangeDetailsOption(param1);
			return 0;
		}
		break;
	
	case _menuActive:
		if ( param1 == _numberUpKey )
		{
			SetPreviousMenuOption();
			return 0;
		}
		ef ( param1 == _numberDownKey )
		{
			SetNextMenuOption();
			return 0;
		}
		ef ( param1 == _confirmKey )
		{
			int i = GetCurrentMenuLine();
			DeleteMenu();
			if (i == 1)
			{
				ExitTAP();
				return 0;
			}
			ef (i == 2)
			{
				return _exitTAPKey;
			}
		}
		ef ( param1 == _exitGUIKey)
		{
			//back to normal state
			DeleteMenu();
			return 0;
		}
		//if we select to exit the TAP then do it else allow the key to pass through
		break;
	
	}
	return param1;

}

//--------------------------------------------------------------------------------------
//	Main - where it all begins...
//--------------------------------------------------------------------------------------

int TAP_Main(void)
{
	if (Startup()==0)
		return 1;  //normal TSR
	else
		return 0;  //exit if error
}

