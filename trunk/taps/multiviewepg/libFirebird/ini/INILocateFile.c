#include "../libFireBird.h"

//Attention: this function will stay in the directory where it has found the file
//It is up to the caller to use HDD_TAP_PushDir() and HDD_TAP_PopDir()

INILOCATION INILocateFile (char *FileName, char *AppName)
{
  INILOCATION           location = INILOCATION_NotFound;
  dword                 dir = (dword) -1;

  if (FileName)
  {
    //Current directory?
    if (TAP_Hdd_Exist(FileName)) location = INILOCATION_AtCurrentDir;
    //Else search (which automatically activates tapapifix)
    else
    {
      dir = HDD_TAP_GetCurrentDirCluster();

      if (HDD_ChangeDir("/ProgramFiles"))
      {
        //Try /ProgramFiles
        if (TAP_Hdd_Exist(FileName))
        {
          location = INILOCATION_AtProgramFiles;
          dir = HDD_TAP_GetCurrentDirCluster();
        }

        if (TAP_Hdd_ChangeDir("Settings"))
        {
          //Try /ProgramFiles/Settings
          if (TAP_Hdd_Exist(FileName))
          {
            location = INILOCATION_AtSettings;
            dir = HDD_TAP_GetCurrentDirCluster();
          }

          //Try /ProgramFiles/Settings/<AppName>
          if (AppName && *AppName && (*AppName != '/') && HDD_ChangeDir(AppName) && TAP_Hdd_Exist(FileName))
          {
            location = INILOCATION_AtAppName;
            dir = (dword) -1;
          }
        }
      }
    }
  }

  //Restore the directory where the file has been found
  if ((int) dir != -1) HDD_TAP_SetCurrentDirCluster(dir);

  return location;
}
