//------------------------------ Information --------------------------------------
//
// keyHandler
//
//---------------------------------------------------------------------------------

#include <string.h>	//strcmpi, strtok

#include "tap.h"
#include "macro.h"
#include "keyHandler.h"
//#include "Utils.h"
#include "keymap.h"

//------------------------------ Types --------------------------------------
//

typedef struct _macroKeyNode {
	int		macroKeyType;
	dword	macroVal;
} MACRONODE;

//KEYNODE - each macro key is described by one of these blocks
typedef struct _keyNode{
	char		keyName[MAXKEYNAME];
	dword		keyVal;
	int			keyCount;
	MACRONODE	keyMacroKeys[MAXMACROKEYS];
	//dword	keyMacroKeys[MAXMACROKEYS];
} KEYNODE;



//------------------------------ Global Variables --------------------------------------
//

static int _keyMacroCount		= 0;		//number of macros in our list
static KEYNODE *_keyMacros[MAXMACROS];		//the macro table
int	_macroPos			= 0;		//current index into the macro table
int	_currMPos			= -1;		//current index into the macro table
dword _macroMenu;					//current menu in a macro

extern dword _mainState;		//current state 
extern int _functionState;


//All the submenu names - these can be used as menu macro keywords
static char* SUBNAMES[] = 
{
	"MainMenu", "TimeMenu", "TimeSettingMenu", "InstallationMenu", "TimerMenu", "LanguageMenu", "ParentMenu", 
	"ParentLockMenu", "AvMenu", "ChListMenu", "FavChListMenu", "SearchMenu", "LnbSettingMenu", "Diseqc12SettingMenu", 
	"USALSMenu", "CiMenu", "Ci", "CiSlotMenu", "SatSearch", "TpSearch", "SmatvSearch", 
	"ChannelEditMenu", "InformationMenu", "SystemMenu", "GameMenu", "OtaMenu", "IrdStatus", "Carlendar", 
	"PvrRecord", "PvrReservation", "ReservationList", "Normal", "Audio", "Subt", "Ttx", 
	"Fav", "Sat", "PvrTimeSearch", "PvrPlayingSearch", "PvrRecSearch", "PipList", "Multifeed", 
	"List", "Epg", "Game", "PvrList", "ExeList", "Exec", "TtxEmul"
};

static const int _noOfStates = ( sizeof SUBNAMES ) / ( sizeof SUBNAMES[0] );
	
//initial phase
extern int _phase;


//------------------------------ Functions --------------------------------------
//


void initSubStateTable()
{
	int i;
	//init keys structure due to bug in gnu compiler as documented in TAP API doc
	for( i=0; i < _noOfStates; i++ )
		SUBNAMES[i] = (char *)((dword)(SUBNAMES[i]) + _tap_startAddr);

}

//--------------------------------------------------------------------------------------
//	getSubstateVal - look up the substate and return the corresponding value
//--------------------------------------------------------------------------------------

dword getSubstateVal(char *menu)
{
	int i;
	for (i=0; i<_noOfStates; i++)
	{
		if (strcmpi(menu, SUBNAMES[i])==0)
			return i;
	}
	return -1;  //key not found - hopefully user notices and does something about it!
}


//--------------------------------------------------------------------------------------
//	loadFunctionKeys
//--------------------------------------------------------------------------------------

void loadFunctionKeys()
{
	int length;
	char buffer[MAX_STR_LEN];
	char iniBuff[MAX_INI];
	char *nextKey, *tkey;
	KEYNODE *keyNode;
    char *token;

	//load all the lines except for ExitTAP and Function

	buffer[0]='\0';

	length = GetPrivateProfileString("function keys", (char *)NULL, (char*)NULL, buffer, MAX_STR_LEN, KEYMAPFILE);
	if (length==0) return ;

	TAP_Print("Loading function keys...\r\n");

	//for each logical key get the value - which is the actual key for the TAP
	nextKey = buffer;  // final dir is marked with double null
	while(strlen(nextKey)>0)
	{
		bool ok = TRUE;
		//save the keyname in our list
		keyNode = (KEYNODE *) malloc(sizeof(KEYNODE));
		strcpy(keyNode->keyName, nextKey);
		keyNode->keyVal = getKeyVal(nextKey);
		keyNode->keyCount = 0;

		TAP_Print("...Adding key %s", nextKey);
		if (GetPrivateProfileString("function keys", nextKey, "", iniBuff, MAX_INI, KEYMAPFILE) > 0)
		{
			//iniBuff contains the list of key names separated by commas
			//save the list of macro key values
			token = strtok(iniBuff, MACRODELIMITERS);
			while (token != NULL)
			{
				//see if its a macro keyword
				if (*token == '|')
				{
					TAP_Print("...looking for substate %s\r\n", token++);
					if ((keyNode->keyMacroKeys[keyNode->keyCount].macroVal = getSubstateVal(token)) == 0)
					{
						//uh-oh, bad entry in the macro - ignore this macro
						TAP_Print("...Can't getSubstateVal for key: %s, keyname: %s\r\n", nextKey, token);
						ok = FALSE;
						break;
					}
					keyNode->keyMacroKeys[keyNode->keyCount++].macroKeyType = menuType;
				}
				else //a key
				{
					if ((keyNode->keyMacroKeys[keyNode->keyCount++].macroVal = getKeyVal(token)) == 0)
					{
						//uh-oh, bad entry in the macro - ignore this macro
						TAP_Print("...Can't getKeyVal for key: %s, keyname: %s\r\n", nextKey, token);
						ok = FALSE;
						break;
					}
					keyNode->keyMacroKeys[keyNode->keyCount].macroKeyType = keyType;
				}

				TAP_Print(", %s", token);

				/* Get the next bit */
				token = strtok(NULL, MACRODELIMITERS);
			}
			if (keyNode->keyCount == 0) ok = FALSE;   //no macro keys found!
		}
		else
			ok = FALSE;

		//if everything is ok for this line, add it to our macro list, otherwise free the temporary memory allocated
		if (ok)
		{
			_keyMacros[_keyMacroCount++] = keyNode;
			TAP_Print("\r\n");
		}
		else
		{
			free((KEYNODE *)keyNode);
			TAP_Print("Error loading function key %s - discarding...\r\n");
		}

		nextKey += strlen(nextKey) + 1;
	}

}

//--------------------------------------------------------------------------------------
//	loadOptions
//--------------------------------------------------------------------------------------

void loadOptions()
{
	int length;
	char buffer[MAX_STR_LEN];
	char iniBuff[MAX_INI];
	char *nextKey;

	//load all the lines except for ExitTAP and Function

	buffer[0]='\0';

	length = GetPrivateProfileString("options", (char *)NULL, (char*)NULL, buffer, MAX_STR_LEN, KEYMAPFILE);
	if (length==0) return;

	TAP_Print("Loading options...\r\n");

	nextKey = buffer;  // final dir is marked with double null
	while(strlen(nextKey)>0)
	{
		if (strcmpi(nextKey, "FunctionState") == 0)
		{
			TAP_Print("...Getting value for %s\n", nextKey);
			if (GetPrivateProfileString("options", nextKey, "", iniBuff, MAX_INI, KEYMAPFILE) > 0)
			{
				if (strcmpi(iniBuff, "AlwaysOn")==0)
				{
					_functionState = _AlwaysOn;
				} else if (strcmpi(iniBuff, "UseKeyOnce")==0)
				{
					_functionState = _UseKeyOnce;
				} else if (strcmpi(iniBuff, "UseKeyEveryTime")==0)
				{
					_functionState = _UseKeyEveryTime;
				}
				//else ignore
			}
		}

		nextKey += strlen(nextKey) + 1;
	}

}

//--------------------------------------------------------------------------------------
//	checkKey - see if the key is in our expected list. Return keypos (1 based) or 0 if not found
//--------------------------------------------------------------------------------------

int checkKey(dword key)
{
	int i;

	TAP_Print("Looking for key %d...\r\n", key);
	for (i=0; i<_keyMacroCount; i++)
	{
		TAP_Print("?key %d = val %d\r\n", key, _keyMacros[i]->keyVal);
		if (key == _keyMacros[i]->keyVal) 
		{
			TAP_Print("matched\n");
			return i+1;
		}
	}
	return 0;
}

//--------------------------------------------------------------------------------------
//	ExecuteMenu - we're currently executing a macro and the current command is a menu.
//              - set to submenu 'menu'. Must be currently in a menu
//--------------------------------------------------------------------------------------

//have another phase: executingMacroMenu so we know we need to come back here
int ExecuteMenu()
{
	static dword saveMenu;
	static bool inSubMenu = FALSE;
	static bool starting;
	dword mState, sState;
	dword firstState;
	static int loopCounter = 0;
	const int maxLoop = 30;

	TAP_Print("Executing menu %d\r\n", _macroMenu);

	/*
	if (_mainState != STATE_Menu)
	{
		//TAP_Print("Not in a menu - can't execute menu command %d, state: %d\r\n", _macroMenu, _mainState);
		ShowMessage("Not in a menu - can't execute menu command", FALSE, 300);
		return 0;
	}
	*/

	//check what phase we're in
	if ( _phase == _executingMacro )  //starting
	{
		//saveMenu = _macroMenu;
		//press OK
		TAP_Print("EM: Sending OK...\r\n", _macroMenu);
		TAP_GenerateEvent(EVT_KEY, RKEY_Ok, 0);  //select a sub menu
		_phase = _executingMacroMenu;
		starting = TRUE;
		return 1;  //ok
	}
	ef ( _phase == _executingMacroMenu )  //been here before and in a sub menu
	{
		TAP_GetState( &mState, &sState );
		TAP_Print("EM: mState: %d, sState = %d\r\n", mState, sState);
		if (starting) //save the initial menu in case we loop around
		{
			firstState = sState;
			starting = FALSE;
			TAP_Print("EM: First substate = %d, looking for menu: %d\r\n", sState, _macroMenu);
			loopCounter = 0;
		}
		else
		{
			TAP_Print("EM: #%d: substate = %d, looking for menu: %d\r\n", ++loopCounter, sState, _macroMenu);
			//is the current menu the same as the first one? If so then we've looped and haven't found it - msg and abort the macro
			if (sState == firstState)  //we've looped around
			{
				//ShowMessage("EM: Error finding menu in macro", FALSE, 300);
				TAP_Print("EM: Error finding menu in macro\r\n");
				return 0;
			}
			if (loopCounter > maxLoop)
			{
				TAP_Print("EM: Loop count exceeded - exiting\r\n");
				return 0;
			}
		}

		//is this the menu the one we want
		//if so change phase back to executingMacro and return
		if (sState == _macroMenu)
		{
			TAP_Print("EM: ********** Found Menu!!!! ***************\r\n");
			_phase = _executingMacro;
			return 1;
		}

		//else
		//go back one then next
		TAP_Print("EM: Sending Menu,Up,Ok keys...\r\n");
		TAP_GenerateEvent(EVT_KEY, RKEY_Menu, 0);
		TAP_GenerateEvent(EVT_KEY, RKEY_ChUp, 0);
		TAP_GenerateEvent(EVT_KEY, RKEY_Ok, 0);
		return 1;

	}

	ShowMessage("Unexpected error in ExecuteMenu", FALSE, 300);
	return 0;
}

//--------------------------------------------------------------------------------------
//	ExecutMacroV2 - execute the next key/menu in macro for the key in position _macroPos
//              - current key/menu is identified by currMPos
//				- V2 handles menu commands
//--------------------------------------------------------------------------------------

void ExecuteMacroV2()
{
	dword p1, mState, sState;
	//int i;
	//static int currMPos;

	TAP_Print("Executing macro...\r\n");
	_phase = _executingMacro;

	//if (initial) currMPos = -1;  //initialise starting position on first entry

	for (++_currMPos; _currMPos<_keyMacros[_macroPos]->keyCount; _currMPos++)
	{

		//somewhere in the macro
		p1 = _keyMacros[_macroPos]->keyMacroKeys[_currMPos].macroVal;
		TAP_Print("Executing macro type: %s, pos: %d, val: %d for key: %s\r\n",  
			(_keyMacros[_macroPos]->keyMacroKeys[_currMPos].macroKeyType==0)?"KEY":"MENU", _currMPos, p1, _keyMacros[_macroPos]->keyName);

		//if its a menu command
		if (_keyMacros[_macroPos]->keyMacroKeys[_currMPos].macroKeyType == menuType)
		{
			_macroMenu = p1;
			if (ExecuteMenu() == 0) break; //error!
			return;  //give toppy time to update state
		}
		else  //its a key
		{
			//else execute the key
			TAP_GenerateEvent( EVT_KEY, p1, 0 );
			if (p1 == RKEY_Menu) return;   //give the menu key time to update the toppy state

			//TAP_GetState( &mState, &sState );
			//_mainState = mState;  _subState = sState;
		}
	}

	//End of macro or error
	_phase = _initialState;
	_currMPos = -1;
	return;

}

//--------------------------------------------------------------------------------------
//	ExecutMacro - execute the next key/menu in macro for the key in position _macroPos
//              - current key/menu is identified by currMPos
//--------------------------------------------------------------------------------------

void ExecuteMacro(bool initial)
{
	dword p1, mState, sState;
	int i;
	static int currMPos;

	TAP_Print("Executing macro...\r\n");
	//_phase = _executingMacro;

	if (initial) currMPos = -1;  //initialise starting position on first entry

	for (i=0; i<_keyMacros[_macroPos]->keyCount; i++)
	{

		p1 = _keyMacros[_macroPos]->keyMacroKeys[i].macroVal;
		TAP_Print("Executing macro type: %s, pos: %d, val: %d for key: %s\r\n",  
			(_keyMacros[_macroPos]->keyMacroKeys[i].macroKeyType==0)?"KEY":"MENU", i, p1, _keyMacros[_macroPos]->keyName);

		//if its a key command
		if (_keyMacros[_macroPos]->keyMacroKeys[i].macroKeyType == keyType)
		{
			//execute the key
			TAP_GenerateEvent( EVT_KEY, p1, 0 );
		}
		else  //its not a key - error!
		{
			ShowMessage("Found invalid key in macro", FALSE, 300);
		}
	}
	_phase = _initialState;
	return;

}

//--------------------------------------------------------------------------------------
//	freeFunctionKeys - free storage allocate to macro key names
//--------------------------------------------------------------------------------------

void freeFunctionKeys()
{
	int i;

	for (i=0; i<_keyMacroCount; i++)
		free((KEYNODE *)_keyMacros[i]);

}

