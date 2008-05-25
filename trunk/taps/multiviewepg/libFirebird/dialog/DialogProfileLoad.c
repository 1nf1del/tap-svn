#include                "FBLib_dialog.h"

INILOCATION DialogProfileLoad (char *FileName, char *AppName)
{
  return ProfileLoad(FileName, TRUE, AppName);
}
