//------------------------------ Information --------------------------------------
//
// macro
//
// Author:	John De Angelis  email: j_de_angelis@hotmail.com
// Date:	25 October 2004
// Version:	1.1 (December 2005)
//			1.11 (Feb 2006)
//				- fix finding selected menu items
//				- fix prob in loading ini keys
// Description:
//			This TAP is a TSR that executes one or more keys for a function key.
//			A function key is a special user-assigned key plus a second key.
//			For example, you can assign Sleep/1 to go forward 6 "goto bookmarks"
//
// Default Key Assignments: (override with ini or config screen)
//		Sleep:		Function key
//		Exit:		Exits the TAP
//
//---------------------------------------------------------------------------------

//Following needed to debug under the TAP_SDK tool
#ifdef WIN32
#include "TAP_Emulations.h"
#endif

#include <string.h>	//strcmpi, strtok

#include "tap.h"
#include "macro.h"
#include "keyhandler.h"
#include "keymap.h"
#include "macromenu.h"
#include "menu.h"
#include "logging.h"

#define ID_MACRO 0x80500002

//------------------------------ TAP Required --------------------------------------
//
TAP_ID( ID_MACRO );
TAP_PROGRAM_NAME( "Macro V1.11" );
TAP_AUTHOR_NAME("deangelj");
TAP_DESCRIPTION("map function keys to key macros");
TAP_ETCINFO(__DATE__);

//------------------------------ TSRCommander stuff --------------------------------------
//

#include "TSRCommander.inc"

//------------------------------ Statics --------------------------------------
//
static bool _startedAuto;


//------------------------------ Types --------------------------------------
//

//------------------------------ Global Variables --------------------------------------
//
int _tickCount = 0;					//the last tick count

//default keys for different functions - can be overriden by keymap.ini
int _exitTAPKey			= RKEY_Exit;
int _functionKey		= RKEY_Sleep;
int _upKey				= RKEY_ChUp;
int _downKey			= RKEY_ChDown;
int _confirmKey			= RKEY_Ok;
int _deleteKey			= RKEY_NewF1;  //Red
int _configKey			= 0;	//to bring up the config. 0 = disabled (use TSRCommander)
//function key state
int _functionState		= _AlwaysOn;
bool _macrosEnabled		= FALSE;   //if usekeyonce is enabled

extern int	_macroPos;				//current index into the macro table
extern int	_currMPos;				//current index into the macro table
extern int _keyMacroCount;		//number of macros in our list
extern KEYNODE *_keyMacros[MAXMACROS];		//the macro table
dword _macroMenu;					//current menu in a macro

dword _mainState		= 0;		//current state 
dword _subState			= 0;

//initial phase
int _phase = _initialState;

int _delayCount = 0;  //counter used to provide a delay to allow Toppy to update its mainState and subState
dword _delayTicks;


//Number of seconds allowed between function key and its follow on key - 100=1second
extern int _FunctionKeyTime;

//Whether to use the exit menu
extern bool _UseExitMenu;

//------------------------------ Function prototypes --------------------------------------
//

dword idle_handler( dword param1 );
dword uart_handler( int incoming_byte );
dword key_handler( dword key );
dword initial_state_handler ( dword param1 );
dword functionkey_pressed_handler ( dword param1 );
void ExitTAP( void );
//void DisplayMenu( int row );


//------------------------------ Start of Subroutines --------------------------------------
//


//------------------------------ TSRCommander required routines --------------------------------------
// 

//Called when user selects Config macro in the TSRCommander dialog
void TSRCommanderConfigDialog()
{
	DisplayMenu(0);
	_phase = _menuActive;
	return;
}

//Called when user selects Exit macro in the TSRCommander dialog
bool TSRCommanderExitTAP (void)
{	
	ExitTAP();
	return TRUE;
}


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
	logOutput( LOG_INFO, "TotalHeap: %d, FreeHeap: %d, AvailHeap: %d", heapSize, freeHeapSize, availHeapSize); 
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
	int keymapHndl;					//handle to our keymap

	//Show avail mem
	PrintMem();

	CheckAutoStart();

	sprintf(msg, "Starting %s TAP %s...", PROGRAMNAME, VERSION);
	ShowMessage(msg, TRUE, 200);

	//as the table uses initialisers and due to a bug in the compiler we need to do some "adjustments"
	initSubStateTable();

	/* Need to init the package before use - only show errors/fatals */
	initKeyMap(_ERROR);

	//get the key mappings for the macro utility
	loadKeys();

	//get the user's macro key mappings
	loadFunctionKeys();

	//get the options
	loadOptions();

	//If we need a function key tell the user
	if (!(_functionState == _AlwaysOn))
	{

		//Get the function key name
		if (key = getKeyName(_functionKey))
		{
			char keyend[20];

			strcpy(keyend, &key[5]);  //remove RKEY_
			sprintf(msg, "Macro TAP started - %s is your function key", keyend);
			logOutput(LOG_INFO, msg);
			ShowMessage(msg, TRUE, 200);
		}
		else
		{
			freeFunctionKeys();
			ShowMessage("Can't decode function key in keymap file", TRUE, 300);
			logOutput(LOG_FATAL, "Can't decode function key in keymap file");
			return 1;
 		}

	}

	//see if we're good...
	dumpFunctionKeys();

	//Show avail mem
	PrintMem();
	return 0;
}

//--------------------------------------------------------------------------------------
//	ExitTAP
//--------------------------------------------------------------------------------------

void ExitTAP( void )
{

	logOutput( LOG_INFO, "ExitTAP..."); 
	PrintMem();

	//free storage allocated to macro key names
	freeFunctionKeys();

	//delete the menu region
	DeleteMenu();

	//free any storage for any inflight recording
	freeKeyList();

	ShowMessage("Exiting macro TAP...", FALSE, 100);

	//Show avail mem
	PrintMem();

	//TAP_Exit();
}


//--------------------------------------------------------------------------------------
//	TAP_EventHandler - callback required to handle events
//--------------------------------------------------------------------------------------

dword TAP_EventHandler( word event, dword param1, dword param2 )
{

	// Get State Information from TAP interface
	TAP_GetState( &_mainState, &_subState );

	// Pass event to appropriate handler
	switch( event )
	{
		case EVT_UART:	return uart_handler	( param1 );  break; 
		case EVT_IDLE:	return idle_handler	( param1 );  break;
		case EVT_KEY: 	return key_handler	( param1 );  break;	
		case EVT_RBACK:
		case EVT_SVCSTATUS:
		case EVT_VIDEO_CONV: return param1;  break;	

		default:		TAP_Print("Invalid event found: %d\n", event); break;
	}
	return param1;
}

//--------------------------------------------------------------------------------------
//	key_handler - handle key/remote control events
//--------------------------------------------------------------------------------------

dword key_handler( dword param1 )
{

	//TAP_Print("event = EVT_KEY\n"); 
	//TAP_Print("Key: %s, Phase: %d\n", getKeyName(param1), _phase);

	//TAP_Print("_phase = %d\n", _phase); 
	//TAP_Print("_functionState = %d\n", _functionState); 

	switch (_phase)
	{
		case _initialState:			return initial_state_handler ( param1 ); break; 
		case _functionKeyPressed:	return functionkey_pressed_handler ( param1 ); break;
		case _executingMacro:		return param1; break;
		case _menuActive:			return menuactive_handler ( param1 ); break;
		case _listActive:			return listactive_handler ( param1 ); break;
		case _recordingMacro:		return recording_handler ( param1 ); break;
		case _recordingMacroEnd:	return recording_handler_end ( param1 ); break;
		default:					TAP_Print("Invalid _phase found: %d\n", _phase); break;
	}

	// if didn't match any of the phases then just return the key
	return param1;

}

//--------------------------------------------------------------------------------------
//	The idle_handler is called every 10ms
//
//  delay processing courtesy of Gary Lynch's TimerKey
//--------------------------------------------------------------------------------------

// Delay after sending keys to allow Toppy to update its status  (5 = 5 * 10ms)
#define KeyDelay 100

dword idle_handler( dword param1 )
{

	TSRCommanderWork();

	//if we're executing a macro see if we need to introduce a delay
	if (_phase == _executingMacro)
	{
		//if the current option is a delay then see if we've exceeded the delay time
		if (_keyMacros[_macroPos]->keyMacroKeys[_currMPos].macroKeyType == delayType)
		{
			//if we have exceeded the delay time then go and continue executing the macro
			if (TAP_GetTick() - _delayTicks > _keyMacros[_macroPos]->keyMacroKeys[_currMPos].macroVal)
			{
				ExecuteMacroV2();
			}
		}
	}

	return param1;

}

//--------------------------------------------------------------------------------------
//	uart_handler - Handle data from the serial port
//--------------------------------------------------------------------------------------

dword uart_handler( int incoming_byte )
{
	return 0;
}

//--------------------------------------------------------------------------------------
//	initial_state_handler - handle keys when in the initial state
//--------------------------------------------------------------------------------------

dword initial_state_handler ( dword param1 )
{
	int pos;

	logOutput( LOG_INFO, "initial_state_handler..."); 

	switch (_functionState)
	{
		//If always on just check if the key starts one of our defined macros
		case _AlwaysOn:
			//if key is one of ours execute macro
			if( pos = checkKey(param1) )
			{
				_macroPos = pos - 1;
				_currMPos = -1;
				_phase = _executingMacro;
				ExecuteMacroV2();
				return 0;
			}
			break;

		//In the initial state we just check for the function key
		case _UseKeyOnce:
			if( (int)param1 == _functionKey )
			{
				if (_macrosEnabled)
				{
					_macrosEnabled = FALSE;
					ShowMessage("Macros are disabled", FALSE, 100);
				}
				else
				{
					_macrosEnabled = TRUE;
					ShowMessage("Macros are enabled", FALSE, 100);
				}
				return 0;
			}
			ef (_macrosEnabled && (pos = checkKey(param1)))
			{
				_macroPos = pos - 1;
				_currMPos = -1;
				_phase = _executingMacro;
				ExecuteMacroV2();
				return 0;
			}
			break;

		case _UseKeyEveryTime:
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

		default:
			TAP_Print("Invalid _functionState found: %d\n", _functionState); 
			break;
	}

	//If we want to use a direct config key (non zero) then see if we've pressed it
	if ( (_configKey > 0) && (param1 == _configKey) )
	{
		DisplayMenu(0);
		_phase = _menuActive;
		return 0;
	}
		
	return param1;

}

//--------------------------------------------------------------------------------------
//	functionkey_pressed_handler - handle keys when in the functionkey pressed state
//--------------------------------------------------------------------------------------

dword functionkey_pressed_handler ( dword param1 )
{

	int pos;

	logOutput( LOG_INFO, "functionkey_pressed_handler..."); 

	switch (_functionState)
	{

	case _UseKeyEveryTime:
		//if key is pressed within our time and its one of our keys then execute
		if (TAP_GetTick() - _tickCount < _FunctionKeyTime)
		{
			//Are we expecting this key? If so, execute the macro for this key
			if (pos = checkKey(param1))
			{
				_macroPos = pos - 1;
				_currMPos = -1;
				_phase = _executingMacro;
				_delayCount = 0;
				ExecuteMacroV2();
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

}

//--------------------------------------------------------------------------------------
//	Main - where it all begins...
//--------------------------------------------------------------------------------------

int TAP_Main(void)
{
	TSRCommanderInit( 0, TRUE );

	if (Startup()==0)
		return 1;  //normal TSR
	else
		return 0;  //exit if error
}

#ifdef WIN32
//--------------------------------------------------------------------
// 
// Stuff to support TAP development on PC

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
// The following line sets some information in the SDK that will be available through the File->Info menuchoice
   setTAP_Information(__tap_program_name__, __tap_author_name__, __tap_description__, __tap_etc_str__, __tap_ud__);

   enableFramebufferUpdate(FALSE); // Set to TRUE to get all OSD output copied to framebuffer.
                                   // (available through GetBaseInfo). Setting this to TRUE will slow things down, so don't
                                   // use if you don't need it.

   enableFramebufferMonitor(FALSE); // Set to TRUE to get the SDK to copy any changed contents of the
                                    // framebuffer to the emulated screen. 

   return InitializeTAP_Environment(hInstance, nCmdShow, __tap_program_name__, TAP_Main, TAP_EventHandler);
} 
#endif
