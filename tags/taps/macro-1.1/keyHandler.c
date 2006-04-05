//------------------------------ Information --------------------------------------
//
// keyHandler
//
//		This file contains functions for the macro TAP that handle the key
//		sequences - from loading/saving to the options file, to managing the macros
//		in memory.
//
//---------------------------------------------------------------------------------

#ifdef WIN32
#include "TAP_Emulations.h"
#endif

#include <string.h>	//strcmpi, strtok

#include "tap.h"
#include "macro.h"
#include "keyhandler.h"
#include "keymap.h"
#include "logging.h"
#include "ini.h"

//------------------------------ Types --------------------------------------
//


//------------------------------ Global Variables --------------------------------------
//

int _keyMacroCount		= 0;		//number of macros in our list
KEYNODE *_keyMacros[MAXMACROS];		//the macro table
int	_macroPos			= 0;		//current index into the macro table
int	_currMPos			= -1;		//current index into the macro table
dword _macroMenu;					//current menu in a macro

extern dword _mainState;		//current state 
extern int _functionState;
extern int _functionKey;
extern int _delayCount;  //counter used to provide a delay to allow Toppy to update its mainState and subState
extern dword _delayTicks;  //


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

//Number of seconds allowed between function key and its follow on key - 100=1second
//Can be overridden with the FunctionKeyTime option
int _FunctionKeyTime = 2*100;

//Whether to use the exit menu
//Can be overridden with the ExitMenu option
bool _UseExitMenu = TRUE;

//whether to record delays between keys
bool _RecordDelays = TRUE;

//where to look for colours on menus - this is for M? processing
int _MenuColPixel1 = 80;
int _MenuColPixel2 = 40;

//Pointers to dynamic macros built from the config menu
/*
KEYVALNODE *binKey = NULL;
KEYVALNODE *tailKey = NULL;
*/

typedef struct _dynKeyValNode{
	int		_keyVal;
	int		_keyValType;
	struct	_dynKeyValNode *  _next;
} DYNKEYVALNODE;

DYNKEYVALNODE *binKey = NULL;
DYNKEYVALNODE *tailKey = NULL;



//------------------------------ Functions --------------------------------------
//


//--------------------------------------------------------------------------------------
//	initSubStateTable - initialise the static char table due to a bug in the compiler
//--------------------------------------------------------------------------------------

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
//	SelectedMenuItem - 
//		Find the selected menu item by looking for the highlight colour. 
//		Returns the number of the selected item. 1 is the item at the top of the menu.
//		Disabled menu items are not counted.
//
//		Borrowed from Gary Lynch's timerkey.c
//--------------------------------------------------------------------------------------

int SelectedMenuItem ()
{
    TYPE_OsdBaseInfo osdBI;
    int y;
    int SelectionNumber;
    int Disabled;
	int colPixel;
    word PixelColour;
    word * pAddr;

    TAP_Osd_GetBaseInfo (&osdBI);   // retrieve current OSD
	colPixel = _MenuColPixel1;		// use default column pixel for finding colours

FIND:
	y = 0; Disabled = 0;
    do
    {
        pAddr = osdBI.eAddr + (osdBI.width * (y+134));  //First menu item is 134 pixel from the top of screen
        PixelColour = *(pAddr + colPixel);       //Checking pixels from the left side of the screen
        //PixelColour = *(pAddr + 400);       //Checking 400 pixels from the left side of the screen
        y+=32;       //Menu items are 32 pixels apart
        switch (PixelColour)
		{
			case 0xDB19:
				Disabled++;
				break;
			case 0x9D0E:	//normal
			case 0xA5D5:	//selected
				break;
			default:
				if (colPixel == _MenuColPixel1)
				{
					colPixel = _MenuColPixel2;
					goto FIND;
				}
				//some sort of problem!!
				logOutput( LOG_ERROR, "SelectedMenuItem() - unexpected colours in finding selected item" );
				return 0;
		}
    }
    while ((PixelColour != 0xA5D5) && (y+134 < osdBI.height));  //Selected menu item is colour 0xA5D5, so stop if selected item fount or end of screen is reached.

    if (PixelColour == 0xA5D5)  //Selected item found
    {
        SelectionNumber = (y/32)-Disabled;
    }
    else
    {
        SelectionNumber = 0;    //No selected item found
    }
    return SelectionNumber;
}


//--------------------------------------------------------------------------------------
//	addKeyListToFunctionKeys - adds a keylist to a keynode
//--------------------------------------------------------------------------------------

void addKeyListToFunctionKeys( KEYNODE *kn )
{
	DYNKEYVALNODE *le;

	logOutput( LOG_INFO, "addKeyListToFunctionKeys..." );
	kn->keyCount = 0;
	le = binKey;
	while (le){
		//TAP_Print("....KeyVal: %s\n", getKeyName(le->_keyVal));
		kn->keyMacroKeys[kn->keyCount].macroVal = le->_keyVal;
		kn->keyMacroKeys[kn->keyCount++].macroKeyType = le->_keyValType;
		le = le->_next;
	}
}

//--------------------------------------------------------------------------------------
//	updateFunctionKeys - update the function keylist with the newly	created macro
//
//		mk = the invoker key
//--------------------------------------------------------------------------------------

int updateFunctionKeys( dword mk )
{
	int i;
	KEYNODE *kn;
	bool found = FALSE;

	logOutput( LOG_INFO, "Updating function keys - mk val=%d, str=%s", mk, getKeyName(mk));
	//if the macrokey is already in our list update with the new list
	for (i=0; i<_keyMacroCount; i++)
	{
		kn = _keyMacros[i];
		if (kn->keyVal == mk)
		{
			//already in list
			logOutput( LOG_INFO, "already in list...");
			addKeyListToFunctionKeys( kn );
			return 0;
		}

	}

	logOutput( LOG_INFO, "not found in list...");
	//not found in the list - so add it
	if (++_keyMacroCount < MAXMACROKEYS)
	{
		kn = (KEYNODE *) malloc(sizeof(KEYNODE));
		strcpy( kn->keyName, getKeyName(mk) );
		kn->keyVal = mk;
		addKeyListToFunctionKeys( kn );
		_keyMacros[_keyMacroCount-1] = kn;

		return 0;
	}
	else
	{
		//too many macros - tell the caller
		_keyMacroCount--;
		return -1;
	}

}



//--------------------------------------------------------------------------------------
//	checkKey - see if the key is in our expected list. Return keypos (1 based) or 0 if not found
//--------------------------------------------------------------------------------------

int checkKey(dword key)
{
	int i;

	logOutput( LOG_INFO, "Looking for key %d...", key);
	for (i=0; i<_keyMacroCount; i++)
	{
		logOutput( LOG_INFO, "?key %d = val %d", key, _keyMacros[i]->keyVal);
		if (key == _keyMacros[i]->keyVal) 
		{
			logOutput( LOG_INFO, "matched");
			return i+1;
		}
	}
	return 0;
}

//--------------------------------------------------------------------------------------
//	ExecuteMenuV2 - we're currently executing a macro and the current command is a menu.
//			V2 = using the new M? options - must be in a menu
//			Return: 0 on error, 1 success
//--------------------------------------------------------------------------------------

int ExecuteMenuV2( int menu )
{
	int counter;
	int selectedItem;

	logOutput( LOG_INFO, "ExecuteMenuV2 menu: %d", menu);

	if (_mainState != STATE_Menu)
	{
		ShowMessage("Not in a menu - can't execute menu command", FALSE, 300);
		logOutput( LOG_ERROR, "ExecuteMenuV2: Not in a menu - can't execute menu command - menu: %d", menu);
		return 0;
	}

	// Menu is in _macroMenu

	if ((selectedItem = SelectedMenuItem()) == 0)  //Get highlighted menu item
	{	//Menu's selected item was not found so don't send any keys.
		logOutput( LOG_ERROR, "ExecuteMenuV2: Can't find SelectedMenuItem - exiting");
		return 0;
	}

	// Go to the top of the Main Menu (System Setting) then send 'Ok'

	logOutput( LOG_INFO, "ExecuteMenuV2: selectedItem = %d, menu = %d", selectedItem, menu);
	if (selectedItem < menu)
		for (counter = selectedItem; counter < menu; counter++)
			TAP_GenerateEvent(EVT_KEY, RKEY_ChDown, 0);
	else
		for (counter = selectedItem; counter > menu; counter--)
			TAP_GenerateEvent(EVT_KEY, RKEY_ChUp, 0);

	return 1;  //ok
}

//--------------------------------------------------------------------------------------
//	ExecutMacroV2 - execute the next key/menu in macro for the key in position _macroPos
//              - current key/menu is identified by currMPos
//				- V2 handles menu commands
//--------------------------------------------------------------------------------------

void ExecuteMacroV2()
{
	dword p1, mState, sState;

	logOutput( LOG_INFO, "ExecuteMacroV2...");
	_phase = _executingMacro;

	//if (initial) currMPos = -1;  //initialise starting position on first entry

	for (++_currMPos; _currMPos<_keyMacros[_macroPos]->keyCount; _currMPos++)
	{

		//somewhere in the macro
		p1 = _keyMacros[_macroPos]->keyMacroKeys[_currMPos].macroVal;
		//logOutput( LOG_INFO, "Executing macro type: %s, pos: %d, val: %d for key: %s",  
		//	(_keyMacros[_macroPos]->keyMacroKeys[_currMPos].macroKeyType==0)?"KEY":"MENU", _currMPos, p1, _keyMacros[_macroPos]->keyName);

		switch (_keyMacros[_macroPos]->keyMacroKeys[_currMPos].macroKeyType)
		{
			case menuType:
				logOutput( LOG_INFO, "Executing macro type: %s, pos: %d, val: %d for key: %s", "MENU", _currMPos, p1, _keyMacros[_macroPos]->keyName);
				_macroMenu = p1;  // not necessary?
				if (ExecuteMenuV2(p1) == 0) goto exiting; //error!
				break;  //goto next key

			case keyType:
				logOutput( LOG_INFO, "Executing macro type: %s, pos: %d, val: %d for key: %s", "KEY", _currMPos, p1, _keyMacros[_macroPos]->keyName);
				TAP_GenerateEvent( EVT_KEY, p1, 0 );
				break;	//goto next key

			case delayType:
				logOutput( LOG_INFO, "Executing macro type: %s, pos: %d, val: %d for key: %s", "DELAY", _currMPos, p1, _keyMacros[_macroPos]->keyName);
				_delayTicks = TAP_GetTick();
				return;	//exit so we can delay through the idle loop

			default:
				ShowMessage("Found invalid item in macro", FALSE, 300);
				logOutput( LOG_ERROR, "Found invalid item in macro executing macro type: %s, pos: %d, val: %d for key: %s", "???", _currMPos, p1, _keyMacros[_macroPos]->keyName);
				goto exiting;
		}
	}

exiting:

	_phase = _initialState;
	_currMPos = -1;

	//End of macro or error
	logOutput( LOG_INFO, "exiting executingMacroV2...");
	return;

}

//--------------------------------------------------------------------------------------
//	freeFunctionKeys - free storage allocate to macro key names
//--------------------------------------------------------------------------------------

void freeFunctionKeys()
{
	int i;

	logOutput( LOG_INFO, "freeFunctionKeys()...");

	for (i=0; i<_keyMacroCount; i++)
		free((KEYNODE *)_keyMacros[i]);

}


//--------------------------------------------------------------------------------------
//	deleteFunctionKey - delete the functon key at offset i (1 based)
//--------------------------------------------------------------------------------------

void deleteFunctionKey(int i)
{
	int j;

	logOutput( LOG_INFO, "deleteFunctionKey: i=%d, _keyMacroCount=%d...", i, _keyMacroCount); 

	if (i == 0) return;  //shouldn't be...

	logOutput( LOG_INFO, "deleteFunctionKey: freeing node..."); 
	--i;  //make it 0-based
	//free the deleted keynode
	free((KEYNODE *)_keyMacros[i]);
	//move the later ones up
	for (j=i; j<_keyMacroCount-1; j++)
		 _keyMacros[j] = _keyMacros[j+1];

	//decrement count of macros
	--_keyMacroCount;

}

//--------------------------------------------------------------------------------------
//	dumpFunctionKeys - dump the function key list - for debugging
//--------------------------------------------------------------------------------------

void dumpFunctionKeys()
{
	int i, j;

	logOutput( LOG_INFO, "Dumping FunctionKey list...");
	logOutput( LOG_INFO, "...MacroCount = %d", _keyMacroCount);
	for (i=0; i<_keyMacroCount; i++)
	{

		logOutput( LOG_INFO, "...Macro %d has %d keys, Key is: %s", i, _keyMacros[i]->keyCount, _keyMacros[i]->keyName);
		for (j=0; j<_keyMacros[i]->keyCount; j++)
		{
			switch (_keyMacros[i]->keyMacroKeys[j].macroKeyType)
			{
			case keyType:
				logOutput( LOG_INFO, "......Pos: %d, Type: KEY, Val: %d", j, _keyMacros[i]->keyMacroKeys[j].macroVal);
				break;
			case menuType:
				logOutput( LOG_INFO, "......Pos: %d, Type: MENU, Val: %d", j, _keyMacros[i]->keyMacroKeys[j].macroVal);
				break;
			case delayType:
				logOutput( LOG_INFO, "......Pos: %d, Type: DELAY, Val: %d", j, _keyMacros[i]->keyMacroKeys[j].macroVal);
				break;
			default:
				logOutput( LOG_INFO, "......Pos: %d, Type: ??, Val: %d", j, _keyMacros[i]->keyMacroKeys[j].macroVal);
				break;
			
			}
		}
	}
}

//--------------------------------------------------------------------------------------
//	freeKeyList - free storage allocate for the KeyList
//--------------------------------------------------------------------------------------

void freeKeyList()
{
	DYNKEYVALNODE *tmp, *hd = binKey;
	while (hd != NULL)
	{
		tmp = hd->_next;
		free(hd);
		hd = tmp;
	}
	binKey = NULL;
	tailKey = NULL;
}

//--------------------------------------------------------------------------------------
//	Add a key to the end of the chain
//--------------------------------------------------------------------------------------

void addKey(dword val, int valType)
{
	DYNKEYVALNODE *tmp;
	if ((tmp = (DYNKEYVALNODE *) malloc(sizeof(DYNKEYVALNODE))) == NULL)
	{
		/* some error here */
		return;
	}

	//init the node
	tmp->_next = NULL;
	tmp->_keyVal = val;
	tmp->_keyValType = valType;

	//reset list and tail pointers 
	if (binKey == NULL)
		binKey = tmp;
	else
		tailKey->_next = tmp;

	//point to the new tail
	tailKey = tmp;

}

//--------------------------------------------------------------------------------------
//	dumpKeyList - dump the recorded key list - for debugging
//--------------------------------------------------------------------------------------

void dumpKeyList()
{
	int i;
	//KEYVALNODE *le;
	DYNKEYVALNODE *le;

	logOutput( LOG_INFO, "Dumping keylist entries...");
	le = binKey;
	while (le){
		logOutput( LOG_INFO, "....Val: %d, ValType: %d", le->_keyVal, le->_keyValType);
		//logOutput( LOG_INFO, "....ValType: %s", getKeyName(le->_keyVal));
		le = le->_next;
	}
}


//--------------------------------------------------------------------------------------
//	Decode the functionState numeric to a readable string
//--------------------------------------------------------------------------------------

char *getFunctionState(char *state)
{
	switch (_functionState)
	{
	case _AlwaysOn: 		strcpy(state, "AlwaysOn"); break;
	case _UseKeyOnce:		strcpy(state, "UseKeyOnce"); break;
	case _UseKeyEveryTime:	strcpy(state, "UseKeyEveryTime"); break;
	default:  				strcpy(state, "Undefined"); break;
	}
	return state;

}

//--------------------------------------------------------------------------------------
//	Decode the functionState string to a value
//--------------------------------------------------------------------------------------

int getFunctionStateValue(char *state)
{

	if (strcmpi(state, "AlwaysOn")==0)
	{
		return _AlwaysOn;
	} 
	else if (strcmpi(state, "UseKeyOnce")==0)
	{
		return _UseKeyOnce;
	} 
	else if (strcmpi(state, "UseKeyEveryTime")==0)
	{
		return _UseKeyEveryTime;
	}
	else
		return _functionState;

}
