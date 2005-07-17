//------------------------------ Information --------------------------------------
//
// macro
//
// Author:	John De Angelis  email: j_de_angelis@hotmail.com
// Date:	25 October 2004
// Version:	1.1
// Description:
//			This TAP is a TSR that executes one or more keys for a function key.
//			A function key is a special user-assigned key plus a second key.
//			For example, you can assign Sleep/1 to go forward 6 "goto bookmarks"
//
// Default Key Assignments: (override with keymapper)
//		Sleep:		Function key
//		Exit:		Exits the TAP
//
//---------------------------------------------------------------------------------

#include <string.h>	//strcmpi, strtok

#include "tap.h"
#include "macro.h"
#include "keyhandler.h"
//#include "Utils.h"
#include "keymap.h"

#define ID_MACRO 0x80500002

//------------------------------ TAP Required --------------------------------------
//
TAP_ID( ID_MACRO );
TAP_PROGRAM_NAME( PROGRAMNAME );
TAP_AUTHOR_NAME("deangelj");
TAP_DESCRIPTION("map function keys to key macros");
TAP_ETCINFO(__DATE__);

//------------------------------ Statics --------------------------------------
//
static bool _startedAuto;


//------------------------------ Types --------------------------------------
//
//------------------------------ Global Variables --------------------------------------
//
int _tickCount = 0;					//the last tick count
int _keymapHndl;					//handle to our keymap

//default keys for different functions - can be overriden by keymap.ini
int _exitTAPKey			= RKEY_Exit;
int _functionKey		= RKEY_Sleep;
int _upKey				= RKEY_ChUp;
int _downKey			= RKEY_ChDown;
int _confirmKey			= RKEY_Ok;
//function key state
int _functionState		= _AlwaysOn;

extern int	_macroPos;				//current index into the macro table
extern int	_currMPos;				//current index into the macro table
dword _macroMenu;					//current menu in a macro

dword _mainState		= 0;		//current state 
dword _subState			= 0;

//initial phase
int _phase = _initialState;



//---------------------------------------------------------------------------------
// Call this at start
//---------------------------------------------------------------------------------

void CheckAutoStart()
{
	_startedAuto = ( TAP_GetTick() < REASONABLE_START_TIME*60*100 );  //in 10ms clicks
}


//---------------------------------------------------------------------------------
// Print mem usage
//---------------------------------------------------------------------------------

void PrintMem()
{
	dword heapSize, freeHeapSize, availHeapSize;
	TAP_MemInfo(&heapSize, &freeHeapSize, &availHeapSize);
	TAP_Print("TotalHeap: %d, FreeHeap: %d, AvailHeap: %d\n", heapSize, freeHeapSize, availHeapSize); 
}


//------------------------------ ShowMessage --------------------------------------
//
void ShowMessage(char *msg, bool inStartup, int delay)
{
	int rgn; // Region number returned by TAP_Osd_Create
	int width;

	//if we're trying to print something while we're starting and it's when the Toppy' booting
	if (inStartup && _startedAuto)
	{
		TAP_Print(msg);
		TAP_Print("\r\n");
	}
	else	//normal mode
	{
		rgn = TAP_Osd_Create(0, 0, 720, 576, 0, FALSE); // Define a region that covers the whole screen
		width = TAP_Osd_GetW(msg, 0, FNT_Size_1926);
		TAP_Osd_FillBox(rgn,((720-width)/2)-50,(576/2)-25,width+100,50,COLOR_DarkGray);
		TAP_Osd_PutS(rgn, (720-width)/2, (576/2)-14, -1, msg, COLOR_White, COLOR_DarkGray, 0, FNT_Size_1926, FALSE, ALIGN_LEFT);
		TAP_Osd_FillBox(rgn,0,0,1,1,COLOR_None); // This seems to be necessary to prevent the box crashing. Why? I don't know...
		TAP_Delay(delay);
		TAP_Osd_Delete(rgn); // Delete the region, and therefore clear anything we have printed within it
	}
}

//--------------------------------------------------------------------------------------
//	Do the startup stuff here - incl loading the keymap file
//--------------------------------------------------------------------------------------

int Startup()
{
	char *key;
	int keyVal;
	//bool startedAuto = TRUE;
	char msg[50];

	//Show avail mem
	PrintMem();

	CheckAutoStart();

	sprintf(msg, "Starting %s TAP %s...", PROGRAMNAME, VERSION);
	ShowMessage(msg, TRUE, 200);

	//as the table uses initialisers and due to a bug in the compiler we need to do some "adjustments"
	initSubStateTable();

	/* Need to init the package before use - only show errors/fatals */
	initKeyMap(_ERROR);

	/* Load the keymap structures from the ini file */
	if ((_keymapHndl = loadKeyMap(PROGRAMNAME, KEYMAPFILE)) == 0)
	{
		//error - either keymap.ini isn't there, or there's no section for this program - show a message
		//out the serial port
		//this is a fatal error....
		sprintf(msg, "LoadKeyMap failed. Msg: %s", GetLastErrorString());
		ShowMessage(msg, TRUE, 200);
		return 1;
	}
	else
	{
		//Get the key mappings from the keymap file. If we get any sort of error use defaults
		int tmp;
		if ((tmp = getLogicalKeyVal("ExitTAP", _keymapHndl)) > 0)	_exitTAPKey = tmp;
		if ((tmp = getLogicalKeyVal("Function", _keymapHndl)) > 0)	_functionKey = tmp;
		if ((tmp = getLogicalKeyVal("Up", _keymapHndl)) > 0)		_upKey = tmp;
		if ((tmp = getLogicalKeyVal("Down", _keymapHndl)) > 0)		_downKey = tmp;
		if ((tmp = getLogicalKeyVal("Confirm", _keymapHndl)) > 0)	_confirmKey = tmp;

		//save the macro key mappings
		loadFunctionKeys();

		//save the options
		loadOptions();

		//free storage for the keymap if used
		if (_keymapHndl) freeMap(_keymapHndl);

	}

	//Setup the exit menu

	NewMenu ( "macro Menu" );
	AddMenuLine( "Create macro (not enabled)" );
	AddMenuLine( "Load new macro file (not enabled)" );
	AddMenuLine( "Stop macro Application" );
	AddMenuLine( "Next TAP" );

	//If we need a function key tell the user
	if (!(_functionState == _AlwaysOn))
	{

		//Get the function key name
		if (key = getKeyName(_functionKey))
		{
			char keyend[20];

			strcpy(keyend, &key[5]);  //remove RKEY_
			sprintf(msg, "macro TAP started - %s is your function key", keyend);
			ShowMessage(msg, TRUE, 200);
			//Show avail mem
			PrintMem();

			return 0;
		}
		else
		{
			ShowMessage("Can't find function key in keymap file", TRUE, 300);
			return 1;
		}

	}
	//Show avail mem
	PrintMem();
	return 0;
}

//--------------------------------------------------------------------------------------
//	CreateMacro
//--------------------------------------------------------------------------------------

void CreateMacro( void )
{
	//ask for the function key - then display it on the screen

	//then ask for the key sequences and to end by hitting the function key again

	//when the function key is pressed, ask whether user wants to keep it.
	//if so, save to ini file

	_phase = _initialState;
	return;
}

//--------------------------------------------------------------------------------------
//	LoadMacroFile
//--------------------------------------------------------------------------------------

void LoadMacroFile( void )
{
	//ask for the function key - then display it on the screen

	//then ask for the key sequences and to end by hitting the function key again

	//when the function key is pressed, ask whether user wants to keep it.
	//if so, save to ini file

	_phase = _initialState;
	return;
}

//--------------------------------------------------------------------------------------
//	ExitTAP
//--------------------------------------------------------------------------------------

void ExitTAP( void )
{

	PrintMem();
	//free storage allocate to macro key names
	freeFunctionKeys();

	//delete the menu region
	FreeMenuLines();

	ShowMessage("Exiting TAP...", FALSE, 100);

	//Show avail mem
	PrintMem();

	TAP_Exit();
}


//--------------------------------------------------------------------------------------
//	TAP_EventHandler - callback required to handle events
//--------------------------------------------------------------------------------------

dword TAP_EventHandler( word event, dword param1, dword param2 )
{

	//dword mainState, subState;
	int pos;

	if( event == EVT_IDLE )
    {
		switch (_phase)
		{
		case _executingMacro:
			ExecuteMacroV2();
			break;
		case _executingMacroMenu:
			TAP_Print("event = EVT_IDLE and _executingMacroMenu\n"); 
			TAP_GetState( &_mainState, &_subState );
			if (ExecuteMenu() == 0)  //error!
				_phase = _initialState;
			return 0;
		}
	}

	//just trap key events
    if( event != EVT_KEY )
        return param1;
    
	TAP_Print("event = EVT_KEY\n"); 
	TAP_Print("Key: %d, Phase: %d\n", param1, _phase);

	TAP_GetState( &_mainState, &_subState );
    
	//if we're in the middle of something else let it through -except if we're executing a macro

	//DISABLE: not sure if the following is correct
    //if (( mainState != STATE_Normal ) && ( _phase != _executingMacro ))
	//	return param1;

	switch (_phase)
	{
	case _initialState:

		TAP_Print("state = _initialState\n"); 
        if( param1 == _exitTAPKey )
    	{
			ShowMenu();
 			_phase = _menuActive;
    		return 0;
    	}

		switch (_functionState)
		{
		case _AlwaysOn:
			//if key is one of ours execute macro
			if( pos = checkKey(param1) )
			{
				_macroPos = pos - 1;
				_currMPos = -1;
				_phase = _executingMacro;
				return 0;
			}
			break;
		case _UseKeyOnce:
		case _UseKeyEveryTime:
			TAP_Print("state = UseKeyOnce or UseKeyEveryTime. Param1 = %d\n", param1);
			//if key is functionkey turn change state else ignore key
			if( (int)param1 == _functionKey )
			{
				TAP_Print("Function key pressed\n");
				//user pressed our function key - goto next phase
				_phase = _functionKeyPressed;
				//save click count so that next time in we can compare
				if (_UseKeyEveryTime)
					_tickCount = TAP_GetTick(); //10ms count since system start
				return 0;
			}
			break;
		}
		return param1;

	case _functionKeyPressed:

		TAP_Print("state = _functionKeyPressed\n"); 
		switch (_functionState)
		{
		case _UseKeyOnce:
			TAP_Print("UseKeyOnce\n");
			//if its one of our keys then execute
			if (pos = checkKey(param1))
			{
				_macroPos = pos - 1;
				_currMPos = -1;
				_phase = _executingMacro;
				return 0;
			}
			//if we press it again we turn off use once function
			ef (param1 == _functionKey)
			{
				_phase = _initialState;
				return 0;
			}
			break;
		case _UseKeyEveryTime:
			TAP_Print("UseKeyEveryTime\n");
			//if key is pressed within our time and its one of our keys then execute
			if (TAP_GetTick() - _tickCount < FUNCTIONKEY_TIME)
			{
				//Are we expecting this key? If so, execute the macro for this key
				if (pos = checkKey(param1))
				{
					_macroPos = pos - 1;
					_currMPos = -1;
					_phase = _executingMacro;
					//_phase = _initialState;
					return 0;
				}
			}
			else
				_phase = _initialState;
				//go back to initial if longer than expected

			break;
		}

		return param1;
		break;

	// In the middle of executing a macro

/*
	case _executingMacro:
		//TAP_Print("state = _executingMacro\n"); 
		//ExecuteMacroV2(FALSE);
		return param1;
		break;

	// In the middle of executing a menu in a macro

	case _executingMacroMenu:
		//TAP_Print("state = _executingMacroMenu\n"); 
		//if (ExecuteMenu() == 0)  //error!
		//{
		//	_phase = _initialState;
		//}
		return param1;
		break;
*/

	// Key pressed while the tap's menu is onscreen

	case _menuActive:
		TAP_Print("state = _menuActive\n"); 
		TAP_Print("pressed %d\n", param1);
		if ( param1 == _upKey )
		{
			SetPreviousMenuOption();
			return 0;
		}
		ef ( param1 == _downKey )
		{
			SetNextMenuOption();
			return 0;
		}
		ef ( param1 == _confirmKey )
		{
			int i = GetCurrentMenuOption();
			DeleteMenu();
			_phase = _initialState;
			switch (i)
			{
			case 1:
				CreateMacro();
				return 0;
			case 2:
				LoadMacroFile();
				return 0;
			case 3:
				ExitTAP();
				return 0;
			case 4:
				return _exitTAPKey;
			}
			return 0;
		}
		ef ( param1 == _exitTAPKey )
		{
			DeleteMenu();
			_phase = _initialState;
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

