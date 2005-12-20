#ifndef __FIXTAPAPI_H
#define __FIXTAPAPI_H

#ifdef  __cplusplus
extern "C" {
#endif

// Workaround for TAP_Hdd_GetPlayInfo and TAP_Hdd_GetRecInfo buffer overrun bugs
#define TAP_Hdd_GetPlayInfo Safe_TAP_Hdd_GetPlayInfo
#define TAP_Hdd_GetRecInfo Safe_TAP_Hdd_GetRecInfo
// Workaround for TAP_Hdd_Flen multiple of 512 bytes bug
#define TAP_Hdd_Flen Fixed_TAP_Hdd_Flen
#define TAP_Hdd_Fseek Fixed_TAP_Hdd_Fseek

bool Safe_TAP_Hdd_GetPlayInfo( TYPE_PlayInfo* playInfo );
bool Safe_TAP_Hdd_GetRecInfo( byte recSlot, TYPE_RecInfo* recInfo );
dword Fixed_TAP_Hdd_Flen( TYPE_File *file );
dword Fixed_TAP_Hdd_Fseek( TYPE_File *file, long pos, long where );

#ifdef  __cplusplus
}
#endif

#endif
