#include <stdio.h>
#include <string.h>
#include <tap.h>
#include "tapapifix.h"
#ifdef WIN32
#include <memory.h>
#endif

extern int _appl_version;

#ifdef WIN32
int _appl_version = 0x1225;
#endif

bool (*real_TAP_Hdd_GetRecInfo)( byte recSlot, TYPE_RecInfo *recInfo ) = 0;
bool (*real_TAP_Hdd_GetPlayInfo)( TYPE_PlayInfo *playInfo ) = 0;
dword (*real_TAP_Hdd_Flen)( TYPE_File *file ) = 0;
dword (*real_TAP_Hdd_Fseek)( TYPE_File *file, long pos, long where ) = 0;
word (*real_TAP_Hdd_ChangeDir)( char* dir ) = 0;


// Workaround for TAP_Hdd_GetPlayInfo and TAP_Hdd_GetRecInfo buffer overrun bugs
bool Fixed_TAP_Hdd_GetPlayInfo( TYPE_PlayInfo* playInfo )
{
	static TYPE_PlayInfo safeInfo;
	static char overrun[128];
	static int overrunCheck;

	overrunCheck = 1;
	if ( !real_TAP_Hdd_GetPlayInfo( &safeInfo ) )
		return FALSE;

	if ( overrunCheck != 1 )
	{
		TAP_Print( "TAP_Hdd_GetPlayInfo buffer overrun." );
	}

	memcpy( playInfo, &safeInfo, sizeof(TYPE_PlayInfo) );

	return TRUE;
}


bool Fixed_TAP_Hdd_GetRecInfo( byte recSlot, TYPE_RecInfo* recInfo )
{
	static TYPE_RecInfo safeInfo;
	static char overrun[128];
	static int overrunCheck;

	overrunCheck = 1;
	if ( !real_TAP_Hdd_GetRecInfo( recSlot, &safeInfo ) )
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
	dword length = real_TAP_Hdd_Flen( file );
	return length == 0 || (length % 512) ? length : (length + 512);
}


// and TAP_Hdd_Fseek SEEK_END works incorrectly
dword Fixed_TAP_Hdd_Fseek( TYPE_File *file, long pos, long where )
{
	if ( where != SEEK_END || (file && (file->size == 0 || (file->size % 512))) )
		return real_TAP_Hdd_Fseek( file, pos, where );

	return real_TAP_Hdd_Fseek( file, pos+512, SEEK_END );
}


// Fix single directory and inverted result issues in pre 0x1200 firmware
word Fixed_TAP_Hdd_ChangeDir( char *dir )
{
	char *startPos;
	char *endPos;

	if ( _appl_version >= 0x1200 )
		return real_TAP_Hdd_ChangeDir( dir );
	//TAP_Print("Fixed_TAP_Hdd_ChangeDir %s\n", dir );

	// TAP_Hdd_ChangeDir returns 0 for success on pre 0x1200 firmware
	startPos = dir;
	if ( *startPos == '/')
	{
		// Move to the root directory
		TYPE_File fp;
		int i;

		// Loop until Root found allow maximum of 20 levels before failing
		i = 20;
		TAP_Hdd_FindFirst( &fp );
		while ( strcmp( fp.name, "__ROOT__" ) && --i )
		{
//			TAP_Print("%s: move up\n", fp.name );
			if ( real_TAP_Hdd_ChangeDir( ".." ) ) // shouldn't be a ! here
				return FALSE;
			TAP_Hdd_FindFirst( &fp );
		}
		if ( i == 0 )
			return FALSE;
		++startPos;
	}

	endPos = startPos;
	while ( *endPos )
	{
		endPos = startPos;
		while ( *endPos != 0 && *endPos != '/' )
			++endPos;

		if ( *endPos == 0 )
			break;

		*endPos = 0;
//		TAP_Print("change to:%s\n", startPos );
		if ( real_TAP_Hdd_ChangeDir( startPos ) ) // shouldn't be a ! here
		{
			*endPos = '/';
			return FALSE;
		}
		*endPos = '/';
		startPos = endPos+1;
	}
//	TAP_Print("change to:%s\n", startPos );
	return !real_TAP_Hdd_ChangeDir( startPos ); // should be a ! here
}



#define FIXAPI(api) if (real_##api == 0) {real_##api = api; api = Fixed_##api;}

void InitTAPAPIFix()
{
	FIXAPI(TAP_Hdd_GetPlayInfo);
	FIXAPI(TAP_Hdd_GetRecInfo);
	FIXAPI(TAP_Hdd_Flen);
	FIXAPI(TAP_Hdd_Fseek);
	FIXAPI(TAP_Hdd_ChangeDir);
}
