#include <stdio.h>
#include "../libFireBird.h"

bool HDD_SetCryptFlag (TYPE_File *fp, byte CryptFlag)
{
  dword                 FilePos;
  tRECHeaderInfo        RECHeaderInfo;
  byte                  Buffer [2048];
  TYPE_File             f;

  if (fp == NULL || fp->handle == NULL) return FALSE;

  InitTAPAPIFix();

  FilePos = TAP_Hdd_Ftell (fp);

  if (TAP_Hdd_Fseek (fp, 0, SEEK_SET) != 0) return FALSE;

  if (TAP_Hdd_Fread (Buffer, sizeof(Buffer), 1, fp) != 1 || TAP_Hdd_Fseek (fp, 0, SEEK_SET) != 0) return FALSE;

  HDD_DecodeRECHeader (Buffer, &RECHeaderInfo);

  f = *fp;

  fp->crypt = CryptFlag;
  ((tFileSlot *) fp->handle)->Scrambled = (CryptFlag ? 1 : 0);
  ((tFileSlot *) fp->handle)->CryptFlag = CryptFlag;
  RECHeaderInfo.CryptFlag = CryptFlag;

  HDD_EncodeRECHeader (Buffer, &RECHeaderInfo, RECHeaderInfo.HeaderType);

  if (TAP_Hdd_Fwrite (Buffer, sizeof(Buffer), 1, fp) != 1)
  {
    *fp = f;
    return FALSE;
  }

  return (TAP_Hdd_Fseek (fp, FilePos, SEEK_SET) == FilePos);
}
