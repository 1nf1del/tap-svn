//------------------------------ Information --------------------------------------
//
// ini
//
//		This file contains functions for the macro TAP that handle the ini file
//
//---------------------------------------------------------------------------------

#ifdef WIN32
#include "TAP_Emulations.h"
#endif

#include <string.h>	//strcmpi, strtok

#include "tap.h"
#include "ini.h"
#include "macro.h"
#include "keyhandler.h"
#include "keymap.h"
#include "logging.h"

//------------------------------ Types --------------------------------------
//


//------------------------------ Global Variables --------------------------------------
//

extern int _keyMacroCount;		//number of macros in our list
extern KEYNODE *_keyMacros[MAXMACROS];		//the macro table
extern dword _mainState;		//current state 
extern int _functionState;

extern int _exitTAPKey;
extern int _functionKey;
extern int _upKey;
extern int _downKey;
extern int _confirmKey;
extern int _deleteKey;
extern int _configKey;

extern int _delayCount;  //counter used to provide a delay to allow Toppy to update its mainState and subState

//initial phase
extern int _phase;

//Number of seconds allowed between function key and its follow on key - 100=1second
//Can be overridden with the FunctionKeyTime option
extern int _FunctionKeyTime;

//Whether to use the exit menu
//Can be overridden with the ExitMenu option
extern bool _UseExitMenu;

extern bool _RecordDelays;

extern int _MenuColPixel1;
extern int _MenuColPixel2;

//------------------------------ Functions --------------------------------------
//


//--------------------------------------------------------------------------------------
//	changeDirectoryRoot - change current directory to root from anywhere
//--------------------------------------------------------------------------------------

void changeDirectoryRoot()
{
	TYPE_File	fp;
	int		idx;

	idx=0;
	TAP_Hdd_FindFirst( &fp );
	while( ( strcmp( fp.name, "__ROOT__" ) != 0 ) && ( idx++ < 20 ) )
	{
		TAP_Hdd_ChangeDir("..");
		TAP_Hdd_FindFirst( &fp );
	}
}


//--------------------------------------------------------------------------------------
//	loadKeys - load any keys rempapings from the ini. This uses the 
//			GetPrivateProfileString from libUtils
//--------------------------------------------------------------------------------------

void loadKeys()
{
	int length;
	char buffer[MAX_STR_LEN];
	char iniBuff[MAX_INI];
	char *nextKey;

	//load all the lines except for ExitTAP and Function

	buffer[0]='\0';

	changeDirectoryRoot();
	TAP_Hdd_ChangeDir("ProgramFiles");

	length = GetPrivateProfileString("macro", (char *)NULL, (char*)NULL, buffer, MAX_STR_LEN, KEYMAPFILE);
	if (length==0) return;

	TAP_Print("Loading macro remap keys...\r\n");

	nextKey = buffer;  // final dir is marked with double null
	while(strlen(nextKey)>0)
	{
		TAP_Print("...Getting value for %s\n", nextKey);
		if (GetPrivateProfileString("macro", nextKey, "", iniBuff, MAX_INI, KEYMAPFILE) > 0)
		{
			if (strcmpi(nextKey, "ExitTAP") == 0)
			{
				_exitTAPKey = getKeyVal(iniBuff);
			} 
			else if (strcmpi(nextKey, "Function") == 0)
			{
				_functionKey = getKeyVal(iniBuff);
			}
			else if (strcmpi(nextKey, "Up") == 0)
			{
				_upKey = getKeyVal(iniBuff);
			}
			else if (strcmpi(nextKey, "Down") == 0)
			{
				_downKey = getKeyVal(iniBuff);
			}
			else if (strcmpi(nextKey, "Confirm") == 0)
			{
				_confirmKey = getKeyVal(iniBuff);
			}
			else if (strcmpi(nextKey, "Delete") == 0)
			{
				_deleteKey = getKeyVal(iniBuff);
			}
			else if (strcmpi(nextKey, "Config") == 0)
			{
				_configKey = getKeyVal(iniBuff);
			}
		}
		nextKey += strlen(nextKey) + 1;
	}

}

//--------------------------------------------------------------------------------------
//	loadFunctionKeys - load the expected function keys from the ini. This uses the 
//			GetPrivateProfileString from libUtils
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

	changeDirectoryRoot();
	TAP_Hdd_ChangeDir("ProgramFiles");

	length = GetPrivateProfileString("function keys", (char *)NULL, (char*)NULL, buffer, MAX_STR_LEN, KEYMAPFILE);
	if (length==0) 
	{
		TAP_Print("Can't find [function keys] section in ini file\n");
		return ;
	}

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
				switch (*token)
				{
					//see if its a menu keyword, e.g. "M1" - new for 1.1
					case 'm':
					case 'M':
						TAP_Print("...looking for menu offset %s\r\n", token++);
						if ((keyNode->keyMacroKeys[keyNode->keyCount].macroVal = atoi(token)) == 0)
						{
							//uh-oh, bad entry in the macro - ignore this macro
							TAP_Print("...Bad integer value for key: %s, menu: %s\r\n", nextKey, --token);
							ok = FALSE;
							break;
						}
						keyNode->keyMacroKeys[keyNode->keyCount++].macroKeyType = menuType;
						break;

					//see if its a delay keyword, e.g. "D200" - new for 1.2
					case 'd':
					case 'D':
						TAP_Print("...looking for delay time %s\r\n", token++);
						if ((keyNode->keyMacroKeys[keyNode->keyCount].macroVal = atoi(token)) == 0)
						{
							//uh-oh, bad entry in the macro - ignore this macro
							TAP_Print("...Bad integer value for key: %s, delay: %s\r\n", nextKey, --token);
							ok = FALSE;
							break;
						}
						keyNode->keyMacroKeys[keyNode->keyCount++].macroKeyType = delayType;
						break;

					//else a key
					default:
						//if ((keyNode->keyMacroKeys[keyNode->keyCount++].macroVal = getKeyVal(token)) == 0)
						if ((keyNode->keyMacroKeys[keyNode->keyCount].macroVal = getKeyVal(token)) == 0)
						{
							//uh-oh, bad entry in the macro - ignore this macro
							TAP_Print("...Can't getKeyVal for key: %s, keyname: %s\r\n", nextKey, token);
							ok = FALSE;
							break;
						}
						//keyNode->keyMacroKeys[keyNode->keyCount].macroKeyType = keyType;
						keyNode->keyMacroKeys[keyNode->keyCount++].macroKeyType = keyType;
						break;
					
				}

				TAP_Print(", %s", token);

				/* Get the next bit */
				token = strtok(NULL, MACRODELIMITERS);
			}
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
//	loadOptions - load options from the ini that configures how the tap will run
//		eg.  FunctionState: determines whether the function key needs to be pressed
//							before the macro key. Values supported are:
//					AlwaysOn: No need to press - it's automaticall on
//					UseKeyOnce: Press it once and it's one from then on
//					UseKeyEveryTime: Need to always press the key before the macro key
//
//			 FunctionKeyTime: determines the maximum length of time to wait between the 
//					function key and the macro key (in 100ns increments)
//
//			 ExitMenu: (Y/N) Whether the Exit Menu is displayed if you press the
//					Exit key
//--------------------------------------------------------------------------------------

void loadOptions()
{
	int length;
	char buffer[MAX_STR_LEN];
	char iniBuff[MAX_INI];
	char *nextKey;

	//load all the lines except for ExitTAP and Function

	buffer[0]='\0';

	changeDirectoryRoot();
	TAP_Hdd_ChangeDir("ProgramFiles");

	length = GetPrivateProfileString("options", (char *)NULL, (char*)NULL, buffer, MAX_STR_LEN, KEYMAPFILE);
	if (length==0) return;

	TAP_Print("Loading options...\r\n");

	nextKey = buffer;  // final dir is marked with double null
	while(strlen(nextKey)>0)
	{
		TAP_Print("...Getting value for %s\n", nextKey);
		if (GetPrivateProfileString("options", nextKey, "", iniBuff, MAX_INI, KEYMAPFILE) > 0)
		{

			if (strcmpi(nextKey, "FunctionState") == 0)
			{
				_functionState = getFunctionStateValue(iniBuff);
			} 
			else if (strcmpi(nextKey, "FunctionKeyTime") == 0)
			{
				int i = atoi(iniBuff);
				if (i > 0) _FunctionKeyTime = i;
			} 
			else if (strcmpi(nextKey, "ExitMenu") == 0)
			{
				if (strcmpi(iniBuff, "N")==0) 	_UseExitMenu = FALSE;
			}
			else if (strcmpi(nextKey, "LoggingLevel") == 0)
			{
				setLoggingLevelValueFromString(iniBuff);
			}
			else if (strcmpi(nextKey, "RecordDelays") == 0)
			{
				if (strcmpi(iniBuff, "N")==0) 	_RecordDelays = FALSE;
			}
			else if (strcmpi(nextKey, "MenuColPixel1") == 0)
			{
				int i = atoi(iniBuff);
				if (i > 0) _MenuColPixel1 = i;
			} 
			else if (strcmpi(nextKey, "MenuColPixel2") == 0)
			{
				int i = atoi(iniBuff);
				if (i > 0) _MenuColPixel2 = i;
			} 
		}
		nextKey += strlen(nextKey) + 1;
	}

}

//--------------------------------------------------------------------------------------
//	saveOptions - save options to the ini
//--------------------------------------------------------------------------------------
#define MAXBUFFERSIZE 10240

int saveOptions()
{
    TYPE_File* fp;
	char sIniBuffer[MAXBUFFERSIZE];
	char sBuff[MAX_STR_LEN];
	char sBuff2[MAX_STR_LEN];
	KEYNODE *kn;
	int i, buffLen;

	logOutput( LOG_INFO, "saveOptions...");

	changeDirectoryRoot();
	TAP_Hdd_ChangeDir("ProgramFiles");

	memset( sIniBuffer, 0, MAXBUFFERSIZE );

	//Delete current instance of ini
    if (TAP_Hdd_Exist (KEYMAPFILE))
		TAP_Hdd_Delete(KEYMAPFILE);

	TAP_Hdd_Create (KEYMAPFILE, ATTR_NORMAL);

    if ((fp = TAP_Hdd_Fopen (KEYMAPFILE)))
    {
		strcat( sIniBuffer, "[macro]\r\n" );
		if (_configKey > 0)
		{
			sprintf( sBuff,"Config=%s\r\n", getKeyName(_configKey) );
			strcat( sIniBuffer, sBuff );
		}
		sprintf( sBuff,"Up=%s\r\n", getKeyName(_upKey) );
		strcat( sIniBuffer, sBuff );
		sprintf( sBuff,"Down=%s\r\n", getKeyName(_downKey) );
		strcat( sIniBuffer, sBuff );
		sprintf( sBuff,"Delete=%s\r\n", getKeyName(_deleteKey) );
		strcat( sIniBuffer, sBuff );
		sprintf( sBuff,"Confirm=%s\r\n", getKeyName(_confirmKey) );
		strcat( sIniBuffer, sBuff );
		sprintf( sBuff,"Function=%s\r\n\r\n", getKeyName(_functionKey) );
		strcat( sIniBuffer, sBuff );

		strcat( sIniBuffer, "[function keys]\r\n" );
		//loop through macro list
		for (i=0; i<_keyMacroCount; i++)
		{
			int j;
			kn = _keyMacros[i];

			sprintf( sBuff,"%s=", kn->keyName );
			strcat( sIniBuffer, sBuff );

			for (j=0; j<kn->keyCount; j++)
			{
				//write out the keyname, menu val or delay val
				switch (kn->keyMacroKeys[j].macroKeyType)
				{
				case keyType:
					sprintf( sBuff,"%s", getKeyName(kn->keyMacroKeys[j].macroVal) );
					break;

				case menuType:
					sprintf( sBuff,"M%d", kn->keyMacroKeys[j].macroVal );
					break;

				case delayType:
					sprintf( sBuff,"D%d", kn->keyMacroKeys[j].macroVal );
					break;

				default:
					//uh-oh unexpected
					break;
				
				}

				strcat( sIniBuffer, sBuff );
				if (j+1<kn->keyCount)
					strcat( sIniBuffer, "," );
			}
			strcat( sIniBuffer, "\r\n" );
		}
		strcat( sIniBuffer, "\r\n" );

		strcat( sIniBuffer, "[options]\r\n" );
		sprintf( sBuff,"FunctionState=%s\r\n", getFunctionState(sBuff2) );
		strcat( sIniBuffer, sBuff );
		sprintf( sBuff,"FunctionKeyTime=%d\r\n", _FunctionKeyTime );
		strcat( sIniBuffer, sBuff );
		sprintf( sBuff,"LoggingLevel=%s\r\n", getLoggingLevel(sBuff2) );
		strcat( sIniBuffer, sBuff );
		sprintf( sBuff,"RecordDelays=%c\r\n", (_RecordDelays ? 'Y' : 'N') );
		strcat( sIniBuffer, sBuff );
		sprintf( sBuff,"MenuColPixel1=%d\r\n", _MenuColPixel1 );
		strcat( sIniBuffer, sBuff );
		sprintf( sBuff,"MenuColPixel2=%d\r\n", _MenuColPixel2 );
		strcat( sIniBuffer, sBuff );

		strcat( sIniBuffer, "\r\n" );
		buffLen = strlen( sIniBuffer );
		TAP_Hdd_Fwrite( sIniBuffer, buffLen, 1, fp);

		//zero fill remainder of block
		buffLen = 512 - (buffLen % 512);
		memset( sIniBuffer, 0, buffLen+10 );
		TAP_Hdd_Fwrite( sIniBuffer, buffLen, 1, fp);

		//TAP_Hdd_Fwrite( sIniBuffer, sizeof( sIniBuffer ), 1, fp);
        TAP_Hdd_Fclose (fp);
		return 0;
    }
    else
    {
		return -1;
    }
}

