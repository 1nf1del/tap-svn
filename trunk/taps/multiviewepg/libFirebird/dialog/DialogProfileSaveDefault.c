#include                "FBLib_dialog.h"

bool DialogProfileSaveDefault (void)
{
  bool                  ret;
  INILOCATION           INILocation;

  ret = HDD_TAP_PushDir();
  INILocation = INILocateFile (DEFAULTPROFILE, NULL);
  if (INILocation == INILOCATION_NotFound) INILocation = INILOCATION_AtProgramFiles;
  if (ret) (void)HDD_TAP_PopDir();

  return DialogProfileSave(DEFAULTPROFILE, INILocation, NULL);
}
