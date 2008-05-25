#include <stdio.h>
#include <string.h>
#include "FBLib_ini.h"
#include "../libFireBird.h"

//---------------------------------------  INISaveFile --------------------------------
//
bool INISaveFile (char *FileName, INILOCATION INILocation, char *AppName)
{
  TYPE_File             *f;
  dword                 ret;

  if (INIBuffer == NULL || INILocation >= INILOCATION_NrENUMs) return FALSE;
  if (INILocation == INILOCATION_AtAppName && (!AppName || !AppName [0])) return FALSE;

  if (INILocation < INILOCATION_AtCurrentDir) INILocation = INILOCATION_AtProgramFiles;

  HDD_TAP_PushDir();

  //Make sure that the destination directory exists
  //Nothing to do for INILOCATION_AtCurrentDir
  if (INILocation == INILOCATION_AtProgramFiles || INILocation == INILOCATION_AtSettings || INILocation == INILOCATION_AtAppName)
  {
    if (!HDD_ChangeDir ("/ProgramFiles"))
    {
      HDD_TAP_PopDir();
      return FALSE;
    }
  }

  if (INILocation == INILOCATION_AtSettings || INILocation == INILOCATION_AtAppName)
  {
    TAP_Hdd_Create ("Settings", ATTR_FOLDER);
    if (!HDD_ChangeDir ("Settings"))
    {
      HDD_TAP_PopDir();
      return FALSE;
    }
  }

  if (INILocation == INILOCATION_AtAppName)
  {
    TAP_Hdd_Create (AppName, ATTR_FOLDER);
    if (!HDD_ChangeDir (AppName))
    {
      HDD_TAP_PopDir();
      return FALSE;
    }
  }

  TAP_Hdd_Create (FileName, ATTR_NORMAL);
  if (!(f = TAP_Hdd_Fopen (FileName)))
  {
    HDD_TAP_PopDir();
    return FALSE;
  }

  ret = TAP_Hdd_Fwrite (INIBuffer, BufferSize, 1, f);
  if (ret == 1) HDD_TruncateFile (f, strlen(INIBuffer));
  HDD_TouchFile (f);
  TAP_Hdd_Fclose (f);

  HDD_TAP_PopDir();

  return (ret == 1);
}
