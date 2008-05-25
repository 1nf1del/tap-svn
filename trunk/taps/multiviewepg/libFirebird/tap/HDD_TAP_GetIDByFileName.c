#include "../libFireBird.h"

//--------------------------------------- HDD_TAP_GetIdByFileName --------------------------------
//
dword HDD_TAP_GetIDByFileName (char *TAPFileName)
{
  dword                 ret;
  TYPE_File             *fp;
  dword                 buf[128];

  fp = TAP_Hdd_Fopen (TAPFileName);
  if (!fp) return 0;

  ret = TAP_Hdd_Fread (buf, sizeof(buf), 1, fp);
  TAP_Hdd_Fclose (fp);

  return (ret == 1 ? buf[8] : 0);
}
