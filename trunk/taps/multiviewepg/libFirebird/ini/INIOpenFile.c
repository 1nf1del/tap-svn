#include "FBLib_ini.h"
#include "../libFireBird.h"

char                    *INIBuffer = NULL, *INICommentBuffer = NULL;
dword                   BufferSize = 0;

//---------------------------------------  INIOpenFile --------------------------------
//
INILOCATION INIOpenFile (char *FileName, char *AppName)
{
  TYPE_File             *f;
  dword                 flen, ret = 0;
  INILOCATION           INILocation;

  HDD_TAP_PushDir();
  INILocation = INILocateFile(FileName, AppName);

  if (INILocation == INILOCATION_NotFound)
  {
    HDD_TAP_PopDir();
    BufferSize = 512;
    if (!(INIBuffer = malloc (BufferSize))) return INILOCATION_NotFound;
    memset (INIBuffer, 0, BufferSize);
    return INILOCATION_NewFile;
  }

  InitTAPAPIFix();

  f = TAP_Hdd_Fopen (FileName);
  if (f == NULL)
  {
    HDD_TAP_PopDir();
    return INILOCATION_NotFound;
  }

  flen = TAP_Hdd_Flen (f);
  BufferSize = ((flen >> 9) + 1) << 9;

  INIBuffer = malloc (BufferSize);
  if (INIBuffer) ret = TAP_Hdd_Fread (INIBuffer, flen, 1, f);
  TAP_Hdd_Fclose (f);

  memset(INIBuffer + flen, 0, BufferSize - flen);

  if(INIBuffer && !flen)
  {
    INIBuffer[0] = '\x0d';
    INIBuffer[1] = '\x0a';
    INIBuffer[2] = 0;

    ret = 1;
  }

  HDD_TAP_PopDir();

  return (INIBuffer && (ret == 1) ? INILocation : INILOCATION_NotFound);
}
