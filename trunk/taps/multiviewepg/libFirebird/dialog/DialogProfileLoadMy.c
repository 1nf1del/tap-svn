#include                <string.h>
#include                "FBLib_dialog.h"

INILOCATION DialogProfileLoadMy (char *AppName)
{
  char                  ProfileName[64];

  if (!FBDialogProfile || !AppName || !ProfileMayReload()) return INILOCATION_NotFound;

  memset(ProfileName, 0, sizeof(ProfileName));
  strncpy(ProfileName, AppName, sizeof(ProfileName) - 12);
  strcat(ProfileName, "Profile.ini");

  return ProfileLoad(ProfileName, FALSE, AppName);
}
