//------------------------------ Information --------------------------------------
//
// keymap.c
//
// Author:	John De Angelis  email: j_de_angelis@hotmail.com
// Date:	July 2004
// Version:	1.0 - initial
//			1.1 - add getKeyName - returns name of a key given a value
//				  add getNextLogicalKeyVal - returns next logical key value for a name
//
// Description:
//
//			implementation of a keymapper, so TAPs can externalise their RC key 
//			mappings to logical settings. This allows users to change the keys 
//			that TAPs use by changing the keymap (ini) file
//
//---------------------------------------------------------------------------------

/*
**
*/

#include "tap.h"
#define printf TAP_Print

#include "ini.h"
#include "keymap.h"
#include "Utils.h"    /* for logMessage  */

LOGKEYNODE* keyMap[MAX_KEYMAPS];
int nextEntry = 0;	/* next keymap entry */

char* KEYNAMES[] = 
{
	"RKEY_0", "RKEY_1", "RKEY_2", "RKEY_3", "RKEY_4", "RKEY_5", "RKEY_6", "RKEY_7", "RKEY_8", "RKEY_9", "RKEY_Power", 
	"RKEY_Recall", "RKEY_Mute", "RKEY_NoUse", "RKEY_Uhf", "RKEY_VolDown", "RKEY_Sleep", "RKEY_Easy", "RKEY_ChUp", 
	"RKEY_AudioTrk", "RKEY_Info", "RKEY_VolUp", "RKEY_Guide", "RKEY_Exit", "RKEY_TvRadio", "RKEY_NoUse4 ", "RKEY_Pause", 
	"RKEY_NoUse5", "RKEY_Menu", "RKEY_ChDown", "RKEY_Ok", "RKEY_Fav", "RKEY_Subt", "RKEY_TvSat", "RKEY_F1", "RKEY_F2", 
	"RKEY_F3", "RKEY_F4", "RKEY_Cmd_0", "RKEY_Cmd_1", "RKEY_Cmd_2", "RKEY_Cmd_3", "RKEY_Cmd_4", "RKEY_Cmd_5", "RKEY_Cmd_6", 
	"RKEY_Cmd_7", "RKEY_Cmd_8", "RKEY_Cmd_9", "RKEY_Cmd_a", "RKEY_Cmd_b", "RKEY_Cmd_c", "RKEY_Cmd_d", "RKEY_Cmd_e", 
	"RKEY_Cmd_f", "RKEY_Teletext", "RKEY_Rewind", "RKEY_Play", "RKEY_Forward", "RKEY_Stop", "RKEY_Record", "RKEY_NewF1", 
	"RKEY_Sat", "RKEY_Prev", "RKEY_PlayList", "RKEY_Next", "RKEY_Bookmark", "RKEY_Goto", "RKEY_Check", "RKEY_Ab", "RKEY_Slow", 
	"FKEY_ChUp", "FKEY_ChDown", "FKEY_VolUp", "FKEY_VolDown", "FKEY_Ok", "FKEY_Exit"
};

int KEYVALUES[] = 
{
	0x10000, 0x10001, 0x10002, 0x10003, 0x10004, 0x10005, 0x10006, 0x10007, 0x10008, 0x10009, 0x1000a, 0x1000b, 
	0x1000c, 0x1000d, 0x1000e, 0x1000f, 0x10010, 0x10011, 0x10012, 0x10013, 0x10014, 0x10015, 0x10016, 0x10017, 
	0x10018, 0x10019, 0x1001a, 0x1001b, 0x1001c, 0x1001d, 0x1001e, 0x10020, 0x10021, 0x10022, 0x10023, 0x10024, 
	0x10025, 0x10026, 0x10027, 0x10028, 0x10029, 0x1002a, 0x1002b, 0x1002c, 0x1002d, 0x1002e, 0x1002f, 0x10030, 
	0x10031, 0x10032, 0x10033, 0x10034, 0x10035, 0x10036, 0x10037, 0x10038, 0x10039, 0x1003a, 0x1003c, 0x1003d, 
	0x1003f, 0x10040, 0x10041, 0x10042, 0x10043, 0x10024, 0x10025, 0x10026, 0x1003e, 0x1003b, 0x00001, 0x00002, 
	0x00003, 0x00004, 0x00005, 0x00006
};

const int noOfKeys = ( sizeof KEYNAMES ) / ( sizeof KEYNAMES[0] );


/*
** LoadKeyMap
**
** Loads the keymap from the given file and returns a handle to the map. 
** The handle is the array index+1.
** Returns 0 for any errors
**
*/

int loadKeyMap(char* appname, char* keymapfile)
{
	int length;
	char buffer[MAX_STR_LEN];
	char* nextKey;
	char iniBuff[MAX_INI];
	int retHandle;
	KEYVALNODE *binKey;
	LOGKEYNODE *headKey = NULL;
	KEYDATA *keyData;

	if (nextEntry == -1) return 0;   //max'ed out the keymap list - error out

	buffer[0]='\0';

	length = GetPrivateProfileString(appname, (char *)NULL, (char*)NULL, buffer, MAX_STR_LEN, keymapfile);
	if (length==0) return 0;

	//for each logical key get the value - which is the actual key for the TAP
	nextKey = buffer;  // final dir is marked with double null
	while(strlen(nextKey)>0)
	{

		//keyData = (KEYDATA *) malloc(sizeof(KEYDATA));
		//keyData->_keyList = NULL;
		//keyData->_len = 0;
		//keyData->_next = 0;
		//strcpy(keyData->_logKeyName, nextKey);

		//addKeyNode(&headKey, *keyData);
		addKeyNode(&headKey, nextKey);

		if (GetPrivateProfileString(appname, nextKey, "", iniBuff, MAX_INI, keymapfile) > 0)
		{
			binKey = (KEYVALNODE *) malloc(sizeof(KEYVALNODE));
			binKey->_next = NULL;
			//look for iniBUFF in our KEYNAMES and save the equivalent KEYVALUE
			binKey->_keyVal = getKeyVal(iniBuff);
			//check for commas in next version
			//attach to key structure
			headKey->_keydata._len = 1;
			headKey->_keydata._next = 1;
			headKey->_keydata._keyList = binKey;
		}
		nextKey += strlen(nextKey) + 1;
	}
	keyMap[nextEntry] = headKey;
	retHandle = nextEntry;

	//Max keymaps = 512 - in future, re-use released keymaps
	if (++nextEntry > MAX_KEYMAPS - 1) nextEntry = -1;

	return retHandle+1;  //ie to cater for 0'th index not being an error
}

/*
** Add a key to the beginning of the chain and make it the new head
*/
void addKeyNode(LOGKEYNODE **head, char* key)
{
	LOGKEYNODE *tmp;
	if ((tmp = (LOGKEYNODE *) malloc(sizeof(LOGKEYNODE))) == NULL)
	{
		/* some error here */
		return;
	}
	tmp->_keydata._keyList = NULL;
	tmp->_keydata._len = 0;
	tmp->_keydata._next = 0;
	strcpy(tmp->_keydata._logKeyName, key);
	//tmp->_keydata = keyData;
	tmp->_next = *head;
	*head = tmp;
}

/*
** FreeMap releases the memory taken up by the map structure(s)
** Map index = handle -1
*/

int freeMap(int handle)
{
	LOGKEYNODE *le = keyMap[handle-1];
	freeKeyNode(le);
	return 1;
}

void freeKeyNode(LOGKEYNODE* le)
{
	LOGKEYNODE *tmp;
	while (le != NULL)
	{
		freeKeyData(le->_keydata);     /* Don't forget to free memory within the list! */
		tmp = le->_next;
		free(le);
		le = tmp;
	}
}

void freeKeyData(KEYDATA kd)
{
	KEYVALNODE *tmp, *hd = kd._keyList;
	while (hd != NULL)
	{
		tmp = hd->_next;
		free(hd);
		hd = tmp;
	}
}

int getKey(int handle, char* key)
{
	return 1;
}

/*
** Look up the keyname and return the integer value, or 0 if not found
*/

int getKeyVal(char* keyName)
{
	int i;
	for (i=0; i<noOfKeys; i++)
	{
		if (strcasecmp(keyName, KEYNAMES[i])==0)
			return KEYVALUES[i];
	}
	return 0;  //key not found - hopefully user notices and does something about it!
}

/*
** Look up the keyname and return the integer value, or 0 if not found
*/

char *getKeyName(int keyVal)
{
	int i;
	for (i=0; i<noOfKeys; i++)
	{
		if (KEYVALUES[i] == keyVal)
			return KEYNAMES[i];
	}
	return NULL;  //key not found - hopefully user notices and does something about it!
}

/*
** Look up the logical (user's) keyname and return the next integer value in the list, or 0 if not found or end of list
** Currently only supports single valued items
*/

int getNextLogicalKeyVal(char* keyName, int handle)
{
	char msg[50];
	LOGKEYNODE *le;
	
	if (handle < 1) {
		logMessage(_ERROR, "Invalid keymap handle");
		return 0;
	}

	sprintf(msg, "getNLKeyVal: comparing for key /%s/, #keys=%d", keyName, noOfKeys);
	logMessage( _INFO, msg );

	le = keyMap[handle-1];

	while (le){
		sprintf(msg, "getNLKeyVal: comparing with name /%s/", le->_keydata._logKeyName);
		logMessage( _INFO, msg );

		if (strcasecmp(keyName, le->_keydata._logKeyName)==0) {
			sprintf(msg, "getNLKeyVal: matched. next: %d, len: %d, keyval: /%d/", le->_keydata._next, le->_keydata._len, le->_keydata._keyList->_keyVal);
			logMessage( _INFO, msg );

			if (le->_keydata._next++ <= le->_keydata._len) {
				return le->_keydata._keyList->_keyVal;
			} else {
				le->_keydata._next = 1;
				return 0;	//end of list
			}

		}
		le = le->_next;
	}
	return 0;  //key not found - hopefully user notices and does something about it!
}

/*
** Look up the logical (user's) keyname and return the integer value, or 0 if not found
*/

int getLogicalKeyVal(char* keyName, int handle)
{
	char msg[50];
	LOGKEYNODE *le;
	
	if (handle < 1) {
		logMessage(_ERROR, "Invalid keymap handle");
		return 0;
	}

	sprintf(msg, "getLKeyVal: comparing for key /%s/, #keys=%d", keyName, noOfKeys);
	logMessage( _INFO, msg );

	le = keyMap[handle-1];

	while (le){
		sprintf(msg, "getLKeyVal: comparing with name /%s/", le->_keydata._logKeyName);
		logMessage( _INFO, msg );

		if (strcasecmp(keyName, le->_keydata._logKeyName)==0) {
			sprintf(msg, "getLKeyVal: matched. keyval: /%d/", le->_keydata._keyList->_keyVal);
			logMessage( _INFO, msg );

			return le->_keydata._keyList->_keyVal;
		}
		le = le->_next;
	}
	return 0;  //key not found - hopefully user notices and does something about it!
}

/*
** For debugging - dump the keymap table
*/

void dumpKeyVals()
{
	int i;
	LOGKEYNODE *le;

	printf("Dumping keymap entries...\n");
	for (i=0; i<nextEntry; i++)
	{
		printf("....[%d]\n", i);
		le = keyMap[i];
		while (le){
			printf("....Name: %s\n", le->_keydata._logKeyName);
			printf("....Number of values: %i\n", le->_keydata._len);
			printf("....Next value: %i\n", le->_keydata._next);
			printf("....KeyVal: %d\n", le->_keydata._keyList->_keyVal);
			le = le->_next;
		}
	}
}

void initKeyMap(int fl)
{
	int i;
	//init keys structure due to bug in gnu compiler as documented in TAP API doc
	for( i=0; i < noOfKeys; i++ )
		KEYNAMES[i] = (char *)((dword)(KEYNAMES[i]) + _tap_startAddr);

	/* Make sure any package we use is initialised */
	initIni(fl);
}

