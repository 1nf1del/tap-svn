#ifndef TAPAPIFIX_H
#define TAPAPIFIX_H

#include "tap.h"

extern int _appl_version;

#define FIXAPI(api) real_##api = api; api = Fixed_##api

bool  (*real_TAP_Hdd_GetPlayInfo)(TYPE_PlayInfo *playInfo) = 0;
bool  (*real_TAP_Hdd_GetRecInfo)(byte recSlot, TYPE_RecInfo *recInfo) = 0;
dword (*real_TAP_Hdd_Flen)(TYPE_File *file) = 0;
dword (*real_TAP_Hdd_Fseek)(TYPE_File *file, long pos, long where) = 0;
word  (*real_TAP_Hdd_ChangeDir)(char *dir) = 0;
int   (*real_TAP_Channel_SetAudioTrack)(int trkNum) = 0;
word  (*real_TAP_Hdd_Delete)(char *name) = 0;
dword (*real_TAP_Hdd_Fwrite)(void *buf, dword blksize, dword blk, TYPE_File *file) = 0;
TYPE_File* (*real_TAP_Hdd_Fopen)(char *name) = 0;


bool  Fixed_TAP_Hdd_GetPlayInfo(TYPE_PlayInfo *);
bool  Fixed_TAP_Hdd_GetRecInfo(byte, TYPE_RecInfo *);
int   Need_Flen_Adjust(void);
dword Fixed_TAP_Hdd_Flen(TYPE_File *);
dword Fixed_TAP_Hdd_Fseek(TYPE_File *, long, long);
word  Fixed_TAP_Hdd_ChangeDir(char *);
int   Fixed_TAP_Channel_SetAudioTrack(int);
word  Fixed_TAP_Hdd_Delete(char *);
dword Fixed_TAP_Hdd_Fwrite(void *buf, dword blksize, dword blk, TYPE_File *file);
TYPE_File* Fixed_TAP_Hdd_Fopen(char *name);


#endif
