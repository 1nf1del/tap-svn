#ifndef __FIXTAPAPI_H
#define __FIXTAPAPI_H


// Workaround functions for buffer overrun problems with GetPlayInfo and GetRecInfo
#define TAP_Hdd_GetPlayInfo Safe_TAP_Hdd_GetPlayInfo
#define TAP_Hdd_GetRecInfo Safe_TAP_Hdd_GetRecInfo

bool Safe_TAP_Hdd_GetPlayInfo( TYPE_PlayInfo* playInfo );
bool Safe_TAP_Hdd_GetRecInfo( byte recSlot, TYPE_RecInfo* recInfo );


#endif
