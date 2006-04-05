//------------------------------ Information --------------------------------------
//
// macromenu - handles the menus for the macro tool
//
//---------------------------------------------------------------------------------

//Following needed to debug under the TAP_SDK tool
#ifdef WIN32
#include "TAP_Emulations.h"
#endif

#include "tap.h"
#include "macromenu.h"
#include "macro.h"
#include "keyhandler.h"
#include "menu.h"
#include "logging.h"

//------------------------------ Statics --------------------------------------
//

//------------------------------ Types --------------------------------------
//

//------------------------------ Global Variables --------------------------------------
//

//default keys for different functions - can be overriden by keymap.ini
extern int _exitTAPKey;
extern int _functionKey;
extern int _upKey;
extern int _downKey;
extern int _confirmKey;
extern int _deleteKey;
//function key state
extern int _functionState;

extern int	_macroPos;				//current index into the macro table
extern int	_currMPos;				//current index into the macro table
extern int _keyMacroCount;		//number of macros in our list
extern KEYNODE *_keyMacros[MAXMACROS];		//the macro table

//initial phase
extern int _phase;

//Number of seconds allowed between function key and its follow on key - 100=1second
extern int _FunctionKeyTime;

//Whether to use the exit menu
extern bool _UseExitMenu;

//Whether to record delays
extern bool _RecordDelays;

//Delay between keys
static dword _RecordingKeyDelay;

//whether this is the first key recorded - needed so we don't record the delay before the first key
static bool initialRecordedKey;

//------------------------------ Function prototypes --------------------------------------
//

extern void TSRCommanderSetExit (void);


//------------------------------ Start of Subroutines --------------------------------------
//

//--------------------------------------------------------------------------------------
//	DisplayListMenu
//
//  Display the list of macros and allow select/delete
//--------------------------------------------------------------------------------------

void DisplayListMenu(int row)
{

	char sBuff[1024];
	char sBuff2[10];
	int i, j;

	logOutput( LOG_INFO, "DisplayListMenu..."); 
	//delete the menu region
	DeleteMenu();

	//Setup the list

	sprintf(sBuff, "macro List Menu         FKey: %s", getKeyName(_functionKey));
	NewMenu ( sBuff );

	//for each macro, create a menu line of keyname: key,key,etc
	for (i=0; i < _keyMacroCount; i++)
	{
		strcpy(sBuff, _keyMacros[i]->keyName);
		strcat(sBuff, ": ");

		for (j=0; j<_keyMacros[i]->keyCount; j++)
		{
			//if it's a key write out the keyname, else write out the subname
			switch (_keyMacros[i]->keyMacroKeys[j].macroKeyType)
			{
			case keyType:
				strcat( sBuff, getKeyName(_keyMacros[i]->keyMacroKeys[j].macroVal));
				break;
			case delayType:
				sprintf( sBuff2,"D%d", _keyMacros[i]->keyMacroKeys[j].macroVal );
				strcat( sBuff, sBuff2 );
				break;
			case menuType:
				sprintf( sBuff2,"M%d", _keyMacros[i]->keyMacroKeys[j].macroVal );
				strcat( sBuff, sBuff2 );
				break;
			default:
				strcat( sBuff, "???");
				break;
			}
			if (j+1<_keyMacros[i]->keyCount)
				strcat( sBuff, "," );
		}

		AddMenuLine( sBuff );
	}

	ShowMenu(row);

}


//--------------------------------------------------------------------------------------
//	DisplayMenu
//
//  Display the config menu for the macro app
//--------------------------------------------------------------------------------------

void DisplayMenu(int row)
{

	char msg[50], msg2[50];

	logOutput( LOG_INFO, "DisplayMenu..."); 
	//delete the menu region
	DeleteMenu();

	//Setup the config menu

	NewMenu ( "macro Menu" );
	AddMenuLine( "Create macro" );
	AddMenuLine( "List macros" );
	AddMenuLine( "Test macro (not implemented yet)" );

	sprintf( msg, "Function Key: %s", getKeyName(_functionKey));
	AddMenuLine( msg );
	sprintf( msg, "Function Key Time: %d ms", _FunctionKeyTime);
	AddMenuLine( msg );
	sprintf( msg, "Function Key Type: %s", getFunctionState(msg2) );
	AddMenuLine( msg );

	sprintf( msg, "Logging Level: %s", getLoggingLevel(msg2) );
	AddMenuLine( msg );
	sprintf( msg, "Record Delays: %c", (_RecordDelays ? 'Y' : 'N') );
	AddMenuLine( msg );
	AddMenuLine( "Save Options to file" );
	AddMenuLine( "Stop Macro TAP" );
	AddMenuLine( "Next TAP" );

	ShowMenu(row);

}

//--------------------------------------------------------------------------------------
//	CreateMacro
//
//  Allow recording of keys - process is to use the functionKey to end the recording.
//  Note that if any keys are used by other TAPs (e.g. Exit by TSRCommander) then they may
//  not be recorded properly.
//--------------------------------------------------------------------------------------

void CreateMacro( void )
{
	char msg[100];

	logOutput( LOG_INFO, "CreateMacro..."); 

	//init the keylist
	freeKeyList();
	
	if (_RecordDelays) _RecordingKeyDelay = TAP_GetTick();
	initialRecordedKey = TRUE;

	sprintf(msg, "Use %s to end the recording...", getKeyName(_functionKey));
	ShowMessage(msg, FALSE, 100);
	_phase = _recordingMacro;
	return;
}

//--------------------------------------------------------------------------------------
//	ListMacros
//
//  Show the list
//--------------------------------------------------------------------------------------


void ListMacros( void )
{

	DisplayListMenu(0);
	_phase = _listActive;

}

//--------------------------------------------------------------------------------------
//	TestMacro
//
//  Basically a slow way to invoke the macros
//--------------------------------------------------------------------------------------

void TestMacro( void )
{

	logOutput( LOG_INFO, "TestMacro..."); 
	//idea is to list the currently defined macros in a table on the screen and the user
	//would select one - then execute the key sequence
	_phase = _initialState;
	return;
}


//--------------------------------------------------------------------------------------
//	listactive_handler - handle keys when the function key list/table is active
//--------------------------------------------------------------------------------------

dword listactive_handler ( dword param1 )
{
	if (param1 == _upKey)
	{
		SetPreviousMenuOption();
		return 0;
	}
	ef (param1 == _downKey)
	{
		SetNextMenuOption();
		return 0;
	}
	ef (param1 == _deleteKey)
	{
		int i = GetCurrentMenuOption(); //1 based
		logOutput( LOG_INFO, "listactive_handler: currentmenuoption = %d...", i); 
		if (i>0)
			deleteFunctionKey(i);

		logOutput( LOG_INFO, "deleteFunctionKey: calling DisplayListMenu..."); 
		DisplayListMenu(0);
		return 0;
	}
	ef (param1 == _confirmKey)
	{
		//display the macro in its own window
		int i = GetCurrentMenuOption();
		//DeleteMenu();
		switch (i)
		{
			//do something with the current macro
		}
		//DisplayListMenu(i);
		return 0;
	}
	ef (param1 == _exitTAPKey)
	{
		//TAP_Osd_Delete(listRgn);
		//TAP_EnterNormal();
		DeleteMenu();
		_phase = _initialState;
		return 0;
	}

	//if we didn't match above just eat the key
	return 0;

}

//--------------------------------------------------------------------------------------
//	menuactive_handler - handle keys when the menu is active
//--------------------------------------------------------------------------------------

dword menuactive_handler ( dword param1 )
{

	// Key pressed while the tap's menu is onscreen

	//TAP_Print("pressed %s\n", getKeyName(param1));

	if (param1 == _upKey)
	{
		SetPreviousMenuOption();
		return 0;
	}
	ef (param1 == _downKey)
	{
		SetNextMenuOption();
		return 0;
	}
	ef (param1 == _confirmKey)
	{
		int i = GetCurrentMenuOption();
		DeleteMenu();
		switch (i)
		{
			//set the start macro state
			case _optCreateMacro:
				CreateMacro();
				return 0; break;
			case _optListMacros:
				ListMacros();
				return 0; break;
			case _optTestMacro:
				TestMacro();
				break;
			//set the exit flag - will be picked up on next idle event
			case _optExitTAP:
				TSRCommanderSetExit();
				break;
			case _optPassThrough:
				_phase = _initialState;
				return _exitTAPKey; break;
			//every time we press OK cycle through the function keys, looping at the end
			case _optFunctionKey:
				_functionKey = ((_functionKey-0x10000+1)%67)+0x10000;
				DisplayMenu(i);
				return 0; break;
			//every time we press OK add 200ms to time, looping after 2000ms
			case _optFunctionKeyTime:
				_FunctionKeyTime = (_FunctionKeyTime % 2000) + 200;
				DisplayMenu(i);
				return 0; break;
			//every time we press OK cycle through the functionStates, looping at the end
			case _optFunctionState:
				_functionState = (_functionState % 3) + 1;
				DisplayMenu(i);
				return 0; break;
			//every time we press OK cycle through the logging levels, looping at the end
			case _optLoggingLevel:
				setNextLoggingLevel();
				DisplayMenu(i);
				return 0; break;
			//toggle every time we press OK
			case _optRecordDelays:
				_RecordDelays = (_RecordDelays ? FALSE : TRUE );
				DisplayMenu(i);
				return 0; break;
			//save options to ini file
			case _optSaveOptions:
				if (saveOptions() == 0)
					ShowMessage("Options file saved successfully", FALSE, 100);
				else
					ShowMessage("Error saving options file", FALSE, 100);
				break;
			default:
				TAP_Print("Unknown menu option found: %d\n", i);
				break;
		}
		//default action - return to normal state
		_phase = _initialState;
		return 0;
	}
	ef (param1 == _exitTAPKey)
	{
		DeleteMenu();
		_phase = _initialState;
		return 0;
	}

	//if we didn't match above just eat the key
	return 0;

}

//--------------------------------------------------------------------------------------
//	recording_handler - record keys for a macro
//--------------------------------------------------------------------------------------

dword recording_handler ( dword param1 )
{
	char msg[1024];

	logOutput( LOG_INFO, "recording_handler..."); 

	//check if we've got our end macro key
	if (param1 == _functionKey)
	{

		//set the phase for the next time through the key handler
		_phase = _recordingMacroEnd;

		//if so, ask the user for the macro key to associate this macro with
		ShowMessage("Press the key which will be associated with this macro", FALSE, 100);

		return 0;
	}
	else
	{
		if (param1 > 0)
		{
			if (_RecordDelays && !initialRecordedKey)
			{
				//add the delay to our macro list
				dword currTick = TAP_GetTick();
				addKey(currTick - _RecordingKeyDelay, delayType);
				_RecordingKeyDelay = currTick;
			}
			//add the key to our macro list
			addKey(param1, keyType);
			sprintf(msg, "Key: %s\r\n", getKeyName(param1));
			initialRecordedKey = FALSE;
		}
		else
			sprintf(msg, "Invalid key - may have been intercepted by another TAP");

		ShowMessage(msg, FALSE, 100);
		return param1;
	}
}

//--------------------------------------------------------------------------------------
//	recording_handler_end - save the macro key and the list
//--------------------------------------------------------------------------------------

dword recording_handler_end ( dword param1 )
{

	//save the new keylist to our macro list
	//param1 is the macrokey for this keylist

	logOutput( LOG_INFO, "recording_handler_end..."); 

	if (param1 > 0)
	{
		if (updateFunctionKeys( param1 ) == -1)
			ShowMessage("Error adding macro to internal list", FALSE, 100);
		else
			ShowMessage("macro recorded successfully", FALSE, 100);
	}
	else
	{
		ShowMessage("macro Key not received correctly - may have been intercepted by another TAP", FALSE, 100);
	}


	//write the macro out
	//dumpKeyList();

	//free the storage allocated
	freeKeyList();

	//see if we're good...
	dumpFunctionKeys();

	//reset the phase
	_phase = _initialState;
	return 0;
}
