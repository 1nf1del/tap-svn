#include <stdio.h>
#include <string.h>
#include <tap.h>
#include "tapapifix.h"


#undef TAP_Hdd_GetPlayInfo
#undef TAP_Hdd_GetRecInfo
#undef TAP_Hdd_Flen
#undef TAP_Hdd_Fseek


// Workaround for TAP_Hdd_GetPlayInfo and TAP_Hdd_GetRecInfo buffer overrun bugs
bool Safe_TAP_Hdd_GetPlayInfo( TYPE_PlayInfo* playInfo )
{
	static TYPE_PlayInfo safeInfo;
	static char overrun[128];
	static int overrunCheck;

	overrunCheck = 1;
	if ( !TAP_Hdd_GetPlayInfo( &safeInfo ) )
		return FALSE;

	if ( overrunCheck != 1 )
	{
		TAP_Print( "TAP_Hdd_GetPlayInfo buffer overrun." );
	}

	memcpy( playInfo, &safeInfo, sizeof(TYPE_PlayInfo) );
	
	return TRUE;
}


bool Safe_TAP_Hdd_GetRecInfo( byte recSlot, TYPE_RecInfo* recInfo )
{
	static TYPE_RecInfo safeInfo;
	static char overrun[128];
	static int overrunCheck;

	overrunCheck = 1;
	if ( !TAP_Hdd_GetRecInfo( recSlot, &safeInfo ) )
		return FALSE;

	if ( overrunCheck != 1 )
	{
		TAP_Print( "TAP_Hdd_GetRecInfo buffer overrun." );
	}

	memcpy( recInfo, &safeInfo, sizeof(TYPE_RecInfo) );
	
	return TRUE;
}


// Workaround for TAP_Hdd_Flen multiple of 512 bytes bug
// If a file is a multiple of 512 bytes then TAP_Hdd_Flen returns 512 bytes too little
dword Fixed_TAP_Hdd_Flen( TYPE_File *file )
{
	dword length = TAP_Hdd_Flen( file );
	return length == 0 || (length % 512) ? length : (length + 512);
}


// and TAP_Hdd_Fseek SEEK_END works incorrectly
dword Fixed_TAP_Hdd_Fseek( TYPE_File *file, long pos, long where )
{
	if ( where != SEEK_END || (file && (file->size == 0 || (file->size % 512))) )
		return TAP_Hdd_Fseek( file, pos, where );

	return TAP_Hdd_Fseek( file, pos+512, SEEK_END );
}
