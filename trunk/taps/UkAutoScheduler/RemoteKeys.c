/************************************************************
			Part of the ukEPG project
		This module handles key mapping for the various remote controls

Name	: RemoteKeys.c
Author	: Darkmatter
Version	: 0.3
For	: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter:	31-05-05	Inception date
	  v0.1 sl8:		20-01-06	All variables initialised
	  v0.2 sl8:		19-02-06	Bug Fix - Left and right keys swapped
    v0.3 jpuhakka:  18-02-08  Multi language support added.

**************************************************************/

#include "RemoteKeys.inc"

//-----------------------------------------------------------------------
//
bool IsKeyValid( keyCodes_Struct *keyCodes, dword key )
{
	int	i = 0;
	key_Struct *currentItem = NULL;

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
	key_Struct *newItem = NULL;
    
	if ( keyCodes->count >= MAX_NUMBER_OF_KEYS ) return;				// bounds check
	keyCodes->count++;

	newItem = TAP_MemAlloc( sizeof( key_Struct ) );						// allocate some memory to store the pointers to the strings
	newItem->keyCode = key;												// note, the strings are not copied, they remain in the source code.
	newItem->ButtonText = description;
	keyCodes->keys[ keyCodes->count ] = newItem;						// point to the newly allocated memory
}


void KeyDestory( keyCodes_Struct *keyCodes )
{
	int i = 0;
	
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
	int i = 0;
	key_Struct *currentItem = NULL;

	if ( keyCodes->count == 0 ) return text_NoKeysDefined;						// bounds check
	
	for ( i=1; i<=(keyCodes->count); i++ )
	{
		currentItem = keyCodes->keys[i];

	    if ( key == currentItem->keyCode ) 	return currentItem->ButtonText;
	}

	return text_NoKeysDefined;													// not found
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
	int i = 0;
	key_Struct *currentItem = NULL;

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
	int i = 0;
	key_Struct *currentItem = NULL;

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
    
    KeyAdd( keyCodes, 0x1000c, text_Mute_button/*see language.c */ );
    KeyAdd( keyCodes, 0x10018, text_TV_RadioOrList_button/*see language.c */ );
    KeyAdd( keyCodes, 0x10022, text_TV_Sat_button/*see language.c */ );
    KeyAdd( keyCodes, 0x10010, text_Opt_button/*see language.c */ );

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
    KeyAdd( keyCodes, 0x1000b, text_Recall_button/*see language.c */ );
    KeyAdd( keyCodes, 0x10014, text_Info_button/*see language.c */ );

    KeyAdd( keyCodes, 0x10015, text_LeftArrow_button/*see language.c */ );
    KeyAdd( keyCodes, 0x10012, text_UpArrow_button/*see language.c */ );
    KeyAdd( keyCodes, 0x1001d, text_DownArrow_button/*see language.c */ );
    KeyAdd( keyCodes, 0x1001e, text_Ok_button/*see language.c */ );
    KeyAdd( keyCodes, 0x1001c, text_Menu_button/*see language.c */ );
    KeyAdd( keyCodes, 0x10016, text_Guide_button/*see language.c */ );
//    KeyAdd( keyCodes, 0x10018, "List" );								// can't have duplicates
    KeyAdd( keyCodes, 0x10017, text_Exit_button/*see language.c */ );

    KeyAdd( keyCodes, 0x1003a, text_FastForward_button/*see language.c */ );
    KeyAdd( keyCodes, 0x10038, text_Rewind_button/*see language.c */ );
    KeyAdd( keyCodes, 0x10039, text_Play_button/*see language.c */ );
    KeyAdd( keyCodes, 0x1001a, text_Pause_button/*see language.c */ );
    KeyAdd( keyCodes, 0x1003d, text_Record_button/*see language.c */ );
    KeyAdd( keyCodes, 0x1003c, text_Stop_button/*see language.c */ );
    KeyAdd( keyCodes, 0x1003b, text_SlowMotion_button/*see language.c */ );
    KeyAdd( keyCodes, 0x10041, text_PIP_button/*see language.c */ );
    KeyAdd( keyCodes, 0x10042, text_FileList_button/*see language.c */ );
    KeyAdd( keyCodes, 0x10043, text_Text_button/*see language.c */ );

    KeyAdd( keyCodes, 0x1003f, text_Red_button/*see language.c */ );
    KeyAdd( keyCodes, 0x10024, text_Green_button/*see language.c */ );
    KeyAdd( keyCodes, 0x10025, text_Yellow_button/*see language.c */ );
    KeyAdd( keyCodes, 0x10026, text_Blue_button/*see language.c */ );

    KeyAdd( keyCodes, 0x10040, text_PIPSwap_button/*see language.c */ );
    KeyAdd( keyCodes, 0x1003e, text_White_button/*see language.c */ );
}



void CreateTF5000Keys( keyCodes_Struct *keyCodes )
{
	KeyDestory( keyCodes );												// clear out any old keycodes, and allocated memory
    
    KeyAdd( keyCodes, RKEY_Mute, text_Mute_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Mute, text_UHF_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_TvSat, text_TV_Sat_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Sleep, text_Sleep_button/*see language.c */ );

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
    KeyAdd( keyCodes, RKEY_Recall, text_Recall_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Info, text_Info_button/*see language.c */ );

    KeyAdd( keyCodes, RKEY_VolUp, text_RightArrow_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_VolDown, text_LeftArrow_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_ChUp, text_UpArrow_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_ChDown, text_DownArrow_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Ok, text_Ok_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Menu, text_Menu_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Guide, text_Guide_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_TvRadio, text_TV_Radio_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_AudioTrk, text_AudioTrack_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Subt, text_Subtitle_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Teletext, text_Teletext_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Exit, text_Exit_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Fav, text_Fav_button/*see language.c */ );

    KeyAdd( keyCodes, RKEY_Forward, text_FastForward_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Rewind, text_Rewind_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Play, text_Play_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Pause, text_Pause_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Record, text_Record_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Stop, text_Stop_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Slow, text_SlowMotion_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Prev, text_Previous_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_PlayList, text_FileList_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Next, text_Next/*see language.c */ );

    KeyAdd( keyCodes, RKEY_Red, text_Red_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Green, text_Green_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Yellow, text_Yellow_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_Blue, text_Blue_button/*see language.c */ );

    KeyAdd( keyCodes, RKEY_Sat, text_Sat_button/*see language.c */ );
    KeyAdd( keyCodes, RKEY_White, text_White_button/*see language.c */ );
}

