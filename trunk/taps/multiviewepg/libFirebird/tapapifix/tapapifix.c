#include <stdio.h>
#include <string.h>
#include "tapapifix.h"
#include "../libFireBird.h"
#include "../av/FBLib_av.h"

/*****************************************************************************************************************************/
/* tapapifix                                                                                                                 */
/*****************************************************************************************************************************/

bool TAPAPIFixInitiated = FALSE;

// Workaround for TAP_Hdd_GetPlayInfo and TAP_Hdd_GetRecInfo buffer overrun bugs
bool Fixed_TAP_Hdd_GetPlayInfo (TYPE_PlayInfo* playInfo)
{
  static TYPE_PlayInfo  safeInfo;
  static char           overrun[128];
  static int            overrunCheck;

  *overrun = 0;
  overrunCheck = 1;

  if (!real_TAP_Hdd_GetPlayInfo(&safeInfo)) return FALSE;

  memcpy(playInfo, &safeInfo, sizeof(TYPE_PlayInfo));

  return TRUE;
}

bool Fixed_TAP_Hdd_GetRecInfo (byte recSlot, TYPE_RecInfo* recInfo)
{
  static TYPE_RecInfo   safeInfo;
  static char           overrun[128];
  static int            overrunCheck;

  *overrun = 0;
  overrunCheck = 1;

  if (!real_TAP_Hdd_GetRecInfo(recSlot, &safeInfo )) return FALSE;

  memcpy(recInfo, &safeInfo, sizeof(TYPE_RecInfo));

  return TRUE;
}

int Need_Flen_Adjust(void)
{
  static int fwState = 0; // Not decided

  if(fwState == 0)
  {
    TYPE_File           f;
    tFileSlot           s;
    word               *sbytes = (word*)&f.reserved[3];

    f.handle = &s;
    f.totalCluster = 1;
    s.TotalCluster = 1;
    f.unusedByte = HDD_GetClusterSize() * 512 - 512;
    s.UnusedByte = f.unusedByte;
    *sbytes = 0;
    s.BytesUsed = 0;
    s.BytesUsedCRC = 1;
    if(real_TAP_Hdd_Flen(&f) == 512) fwState = 2; // Possible new state, fixed
                                else fwState = 1; // Normal state, broken
  }

  return fwState == 1;  // Needs fixing if broken
}

// workaround for TAP_Hdd_Flen multiple of 512 bytes bug
// if a file (from PC) is a multiple of 512 bytes then TAP_Hdd_Flen returns 512 bytes too little
dword Fixed_TAP_Hdd_Flen (TYPE_File *file)
{
  dword res;
  tFileSlot *fx;

  if (!file || !(fx = file->handle)) return 0;
  res = real_TAP_Hdd_Flen(file);
  if(Need_Flen_Adjust() && fx->BytesUsedCRC != 0 && fx->BytesUsed == 0) res += 512;

  return res;
}


// and TAP_Hdd_Fseek SEEK_END works incorrectly
dword Fixed_TAP_Hdd_Fseek (TYPE_File *file, long pos, long where)
{
  tFileSlot             *fx;

  if ((where == SEEK_END) && file && Need_Flen_Adjust() && (fx = file->handle) && (fx->BytesUsedCRC != 0 && fx->BytesUsed == 0)) return real_TAP_Hdd_Fseek(file, pos + 512, SEEK_END);
  else return real_TAP_Hdd_Fseek(file, pos, where);
}


// Fix single directory and inverted result issues in pre 0x1200 firmware
word Fixed_TAP_Hdd_ChangeDir (char *dir)
{
  char *startPos;
  char *endPos;

  if (_appl_version >= 0x1200) return real_TAP_Hdd_ChangeDir(dir);

  // TAP_Hdd_ChangeDir returns 0 for success on pre 0x1200 firmware
  startPos = dir;

  if (*startPos == '/')
  {
    // Move to the root directory
    TYPE_File fp;
    int i;

    // Loop until Root found allow maximum of 20 levels before failing
    i = 20;
    TAP_Hdd_FindFirst(&fp);

    while (strcmp(fp.name, "__ROOT__") && --i)
    {
      if (real_TAP_Hdd_ChangeDir("..")) // shouldn't be a ! here
        return FALSE;

      TAP_Hdd_FindFirst(&fp);
    }

    if (i == 0) return FALSE;

    ++startPos;
  }

  endPos = startPos;

  while (*endPos)
  {
    endPos = startPos;

    while (*endPos != 0 && *endPos != '/') ++endPos;

    if (*endPos == 0) break;

    *endPos = 0;

    if (real_TAP_Hdd_ChangeDir(startPos)) // shouldn't be a ! here
    {
      *endPos = '/';
      return FALSE;
    }

    *endPos = '/';
    startPos = endPos + 1;
  }

  return !real_TAP_Hdd_ChangeDir(startPos); // should be a ! here
}

// The audio track jumps to the default track after moving to a different playback location
int Fixed_TAP_Channel_SetAudioTrack (int trkNum)
{
  int                   ret = 0;

  ret = real_TAP_Channel_SetAudioTrack (trkNum);
  if (ret != 0) return ret;

  if (pAudioTracks == NULL) InitAudioPointers();

  //Use Topfields method to find out if the user is playing back a REC file
  if (*(byte*)(RECShadow) == PLAYMODE_Playing)
  {
    //The audio PID word is located at offset 0xc2. Patch it with the PID from the audio track list.
    *(word*)(RECShadow + 0xc2) = pAudioTracks->AudioPID [trkNum];
  }

  return 0;
}

// fix the dot bug
word  Fixed_TAP_Hdd_Delete (char *name)
{
  char *dotpos;
  size_t i;

  dotpos = strchr(name, '.');
  i = (dotpos ? strlen(dotpos) : 0);

  if (i > 30)
  {
    if (!TAP_Hdd_Rename(name, "~FireBirdLib.tmp~")) return real_TAP_Hdd_Delete("~FireBirdLib.tmp~");
    else return 65028;
  }
  else return real_TAP_Hdd_Delete(name);
}

dword Fixed_TAP_Hdd_Fwrite( void *buf, dword blksize, dword blk, TYPE_File *file )
{
  dword res = real_TAP_Hdd_Fwrite(buf, blksize, blk, file);

  if(file != NULL)
  {
    tFileSlot *hdl = file->handle;
    dword pos = file->currentPos;
    dword *sz = (dword*)&file->size;
    word *sbytes = (word*)(((byte*)file)+126);
    if((sz[0]==0 && pos>sz[1]) || (hdl->BytesUsedCRC == 0 && hdl->BytesUsedCRC == 0))
    {
      dword   sect = pos % 512;

      hdl->BytesUsed = sect;
      *sbytes = sect;
      sz[1] = pos;
      hdl->BytesUsedCRC = ((sect >> 8) + sect + 1) % 256;
    }
  }
  return res;
}

TYPE_File* Fixed_TAP_Hdd_Fopen( char *name )
{
  tFileSlot *fx;
  TYPE_File *file = real_TAP_Hdd_Fopen(name);
  if(file && (fx = file->handle) && fx->BytesUsedCRC != 0 && fx->BytesUsed == 0 && Need_Flen_Adjust())
    file->size += 512;
  return file;
}

void InitTAPAPIFix (void)
{
  if (!TAPAPIFixInitiated)
  {
    FIXAPI (TAP_Hdd_GetPlayInfo);
    FIXAPI (TAP_Hdd_GetRecInfo);
    FIXAPI (TAP_Hdd_Flen);
    FIXAPI (TAP_Hdd_Fseek);
    FIXAPI (TAP_Hdd_ChangeDir);
    FIXAPI (TAP_Channel_SetAudioTrack);
    FIXAPI (TAP_Hdd_Delete);
    FIXAPI (TAP_Hdd_Fwrite);
    FIXAPI (TAP_Hdd_Fopen);

    TAPAPIFixInitiated = TRUE;
  }
}
