/************************************************************
			Part of the ukEPG project
		This module handles key mapping for the various remote controls

Name	: RemoteKeys.c
Author	: Darkmatter
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter: 31-05-05	Inception date

	Last change:  USE   1 Aug 105    2:35 pm
**************************************************************/

#include "RemoteKeys.inc"

static char nullText[] = "No Keys defined";

//-----------------------------------------------------------------------
//
bool IsKeyValid( keyCodes_Struct *keyCodes, dword key )
{
	int i;
	key_Struct	*currentItem;

	if ( keyCodes->count == 0 ) return FALSE;							// bounds check
	
	for ( i=1; i<=(keyCodes->count) ; i++ )								// look up the index for this key
	{
		currentItem = keyCodes->keys[i];

	    if ( key == currentItem->keyCode ) return TRUE;					// any matches
	}

	return FALSE;														// found nothing that matches
}



//---------------
//
void KeyAdd( keyCodes_Struct *keyCodes, dword key, char *description )
{
	key_Struct	*newItem;
    
	if ( keyCodes->count >= MAX_NUMBER_OF_KEYS ) return;				// bounds check
    keyCodes->count++;

	newItem = TAP_MemAlloc( sizeof( key_Struct ) );						// allocate some memory to store the pointers to the strings
	newItem->keyCode = key;												// note, the strings are not copied, they remain in the source code.
	newItem->ButtonText = description;
	keyCodes->keys[ keyCodes->count ] = newItem;						// point to the newly allocated memory
}


void KeyDestory( keyCodes_Struct *keyCodes )
{
    int i;
	
	if ( keyCodes->count == 0 ) return;									// bounds check

	for ( i=1; i <= (keyCodes->count) ; i++ )							// for all members of the structure
	{
		TAP_MemFree( keyCodes->keys[i] );								// release memory back to the heap
	}

	keyCodes->count = 0;												// tidy things up - set number of elements to 0
}



//---------------
//
char *GetButtonText( keyCodes_Struct *keyCodes, dword key )
{
	int i;
	key_Struct	*currentItem;

	if ( keyCodes->count == 0 ) return nullText;						// bounds check
	
	for ( i=1; i<=(keyCodes->count); i++ )
	{
		currentItem = keyCodes->keys[i];

	    if ( key == currentItem->keyCode ) 	return currentItem->ButtonText;
	}

	return nullText;													// not found
}



//---------------
//
int GetNumberOfKeys( keyCodes_Struct *keyCodes )
{
	return keyCodes->count;
}


//---------------
//
dword GetNextKey( keyCodes_Struct *keyCodes, dword key )
{
	int i;
	key_Struct	*currentItem;

	if ( keyCodes->count == 0 ) return key;								// bounds check
	
	for ( i=1; i<=(keyCodes->count) ; i++ )								// look up the index for this key
	{
		currentItem = keyCodes->keys[i];

	    if ( key == currentItem->keyCode )	break;
	}

	if ( i < (keyCodes->count) ) i++;									// move to the next key
	else i = 1;

	currentItem = keyCodes->keys[i];									// extract this keys information
		
	return currentItem->keyCode;										// return (the next) keycode
}


dword GetPreviousKey( keyCodes_Struct *keyCodes, dword key )
{
	int i;
	key_Struct	*currentItem;

	if ( keyCodes->count == 0 ) return key;								// bounds check
	
	for ( i=1; i<=(keyCodes->count) ; i++ )								// look up the index for this key
	{
		currentItem = keyCodes->keys[i];

	    if ( key == currentItem->keyCode )	break;
	}

	if ( i > 1 ) i--;													// move to the previous key
	else i = (keyCodes->count);

	currentItem = keyCodes->keys[i];									// extract this keys information
		
	return currentItem->keyCode;										// return (the previous) keycode
}





//---------------
//
void CreateTF5800Keys( keyCodes_Struct *keyCodes )
{
	KeyDestory( keyCodes );												// clear out any old keycodes, and allocated memory
    
    KeyAdd( keyCodes, 0x1000c, "Mute" );
    KeyAdd( keyCodes, 0x10018, "TV/Radio or List" );
    KeyAdd( keyCodes, 0x10022, "TV/Sat" );
    KeyAdd( keyCodes, 0x10010, "Opt" );

    KeyAdd( keyCodes, 0x10001, "1" );
    KeyAdd( keyCodes, 0x10002, "2" );
    KeyAdd( keyCodes, 0x10003, "3" );
    KeyAdd( keyCodes, 0x10004, "4" );
    KeyAdd( keyCodes, 0x10005, "5" );
    KeyAdd( keyCodes, 0x10006, "6" );
    KeyAdd( keyCodes, 0x10007, "7" );
    KeyAdd( keyCodes, 0x10008, "8" );
    KeyAdd( keyCodes, 0x10009, "9" );
    KeyAdd( keyCodes, 0x10000, "0" );
    KeyAdd( keyCodes, 0x1000b, "Recall" );
    KeyAdd( keyCodes, 0x10014, "Info" );

    KeyAdd( keyCodes, 0x1000f, "Right Arrow" );
    KeyAdd( keyCodes, 0x10015, "Left Arrow" );
    KeyAdd( keyCodes, 0x10012, "Up Arrow" );
    KeyAdd( keyCodes, 0x1001d, "Down Arrow" );
    KeyAdd( keyCodes, 0x1001e, "Ok" );
    KeyAdd( keyCodes, 0x1001c, "Menu" );
    KeyAdd( keyCodes, 0x10016, "Guide" );
//    KeyAdd( keyCodes, 0x10018, "List" );								// can't have duplicates
    KeyAdd( keyCodes, 0x10017, "Exit" );

    KeyAdd( keyCodes, 0x1003a, "Fast Forward" );
    KeyAdd( keyCodes, 0x10038, "Rewind" );
    KeyAdd( keyCodes, 0x10039, "Play" );
    KeyAdd( keyCodes, 0x1001a, "Pause" );
    KeyAdd( keyCodes, 0x1003d, "Record" );
    KeyAdd( keyCodes, 0x1003c, "Stop" );
    KeyAdd( keyCodes, 0x1003b, "Slow Motion" );
    KeyAdd( keyCodes, 0x10041, "PIP" );
    KeyAdd( keyCodes, 0x10042, "File List" );
    KeyAdd( keyCodes, 0x10043, "Text" );

    KeyAdd( keyCodes, 0x1003f, "Red" );
    KeyAdd( keyCodes, 0x10024, "Green" );
    KeyAdd( keyCodes, 0x10025, "Yellow" );
    KeyAdd( keyCodes, 0x10026, "Blue" );

    KeyAdd( keyCodes, 0x10040, "PIP Swap" );
    KeyAdd( keyCodes, 0x1003e, "White" );
}



void CreateTF5000Keys( keyCodes_Struct *keyCodes )
{
	KeyDestory( keyCodes );												// clear out any old keycodes, and allocated memory
    
    KeyAdd( keyCodes, RKEY_Mute, "Mute" );
    KeyAdd( keyCodes, RKEY_Mute, "UHF" );
    KeyAdd( keyCodes, RKEY_TvSat, "TV/Sat" );
    KeyAdd( keyCodes, RKEY_Sleep, "Sleep" );

    KeyAdd( keyCodes, RKEY_1, "1" );
    KeyAdd( keyCodes, RKEY_2, "2" );
    KeyAdd( keyCodes, RKEY_3, "3" );
    KeyAdd( keyCodes, RKEY_4, "4" );
    KeyAdd( keyCodes, RKEY_5, "5" );
    KeyAdd( keyCodes, RKEY_6, "6" );
    KeyAdd( keyCodes, RKEY_7, "7" );
    KeyAdd( keyCodes, RKEY_8, "8" );
    KeyAdd( keyCodes, RKEY_9, "9" );
    KeyAdd( keyCodes, RKEY_0, "0" );
    KeyAdd( keyCodes, RKEY_Recall, "Recall" );
    KeyAdd( keyCodes, RKEY_Info, "Info" );

    KeyAdd( keyCodes, RKEY_VolUp, "Right Arrow (volume up)" );
    KeyAdd( keyCodes, RKEY_VolDown, "Left Arrow (volume down)" );
    KeyAdd( keyCodes, RKEY_ChUp, "Up Arrow (channel up)" );
    KeyAdd( keyCodes, RKEY_ChDown, "Down Arrow (channel down)" );
    KeyAdd( keyCodes, RKEY_Ok, "Ok" );
    KeyAdd( keyCodes, RKEY_Menu, "Menu" );
    KeyAdd( keyCodes, RKEY_Guide, "Guide" );
    KeyAdd( keyCodes, RKEY_TvRadio, "TV/Radio" );
    KeyAdd( keyCodes, RKEY_AudioTrk, "Audio Track" );
    KeyAdd( keyCodes, RKEY_Subt, "Subtitle" );
    KeyAdd( keyCodes, RKEY_Teletext, "Teletext" );
    KeyAdd( keyCodes, RKEY_Exit, "Exit" );
    KeyAdd( keyCodes, RKEY_Fav, "Fav" );

    KeyAdd( keyCodes, RKEY_Forward, "Fast Forward" );
    KeyAdd( keyCodes, RKEY_Rewind, "Rewind" );
    KeyAdd( keyCodes, RKEY_Play, "Play" );
    KeyAdd( keyCodes, RKEY_Pause, "Pause" );
    KeyAdd( keyCodes, RKEY_Record, "Record" );
    KeyAdd( keyCodes, RKEY_Stop, "Stop" );
    KeyAdd( keyCodes, RKEY_Slow, "Slow Motion" );
    KeyAdd( keyCodes, RKEY_Prev, "Previous" );
    KeyAdd( keyCodes, RKEY_PlayList, "File List" );
    KeyAdd( keyCodes, RKEY_Next, "Next" );

    KeyAdd( keyCodes, RKEY_Red, "Red" );
    KeyAdd( keyCodes, RKEY_Green, "Green" );
    KeyAdd( keyCodes, RKEY_Yellow, "Yellow" );
    KeyAdd( keyCodes, RKEY_Blue, "Blue" );

    KeyAdd( keyCodes, RKEY_Sat, "Sat" );
    KeyAdd( keyCodes, RKEY_White, "White" );
}

