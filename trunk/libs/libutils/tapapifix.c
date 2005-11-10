#include <tap.h>
#include "tapapifix.h"


// Workaround functions for buffer overrun problems
#undef TAP_Hdd_GetPlayInfo
#undef TAP_Hdd_GetRecInfo

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
