#include                "../ini/FBLib_ini.h"
#include                "FBLib_dialog.h"

bool DialogProfileSave (char *FileName, INILOCATION INILocation, char *AppName)
{
  char                    *OldBuffer, *OldCommentBuffer;
  dword                   OldBufferSize;
  byte                    a, r, g, b;

  #define SPLIT_ARGB(c) {a = c >> 15; r = (c >> 10) & 0x1f; g = (c >> 5) & 0x1f; b = c & 0x1f;}

  if (!FBDialogProfile) return FALSE;

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

  if (INILocation == INILOCATION_AtAppName && AppName && AppName [0])
  {
    TAP_Hdd_Create (AppName, ATTR_FOLDER);
    if (!HDD_ChangeDir (AppName))
    {
      HDD_TAP_PopDir();
      return FALSE;
    }
  }

  OldBuffer = INIBuffer;
  OldCommentBuffer = INICommentBuffer;
  OldBufferSize = BufferSize;

  INIOpenFile (FileName, NULL);

  SPLIT_ARGB(FBDialogProfile->BorderColor);
  INISetARGB ("BorderColor", a, r, g, b);
  SPLIT_ARGB(FBDialogProfile->TitleBackgroundColor);
  INISetARGB ("TitleBackgroundColor", a, r, g, b);
  SPLIT_ARGB(FBDialogProfile->TitleTextColor);
  INISetARGB ("TitleTextColor", a, r, g, b);
  SPLIT_ARGB(FBDialogProfile->ItemsLineBackgroundColor);
  INISetARGB ("ItemsLineBackgroundColor", a, r, g, b);
  SPLIT_ARGB(FBDialogProfile->ItemsLineBackgroundSelectedColor);
  INISetARGB ("ItemsLineBackgroundSelectedColor", a, r, g, b);
  SPLIT_ARGB(FBDialogProfile->ItemsIconBackgroundColor);
  INISetARGB ("ItemsIconBackgroundColor", a, r, g, b);
  SPLIT_ARGB(FBDialogProfile->ItemsIconBackgroundSelectedColor);
  INISetARGB ("ItemsIconBackgroundSelectedColor", a, r, g, b);
  SPLIT_ARGB(FBDialogProfile->ItemsTextColor);
  INISetARGB ("ItemsTextColor", a, r, g, b);
  SPLIT_ARGB(FBDialogProfile->ItemsTextDisabledColor);
  INISetARGB ("ItemsTextDisabledColor", a, r, g, b);
  SPLIT_ARGB(FBDialogProfile->ItemsTextSelectedColor);
  INISetARGB ("ItemsTextSelectedColor", a, r, g, b);
  SPLIT_ARGB(FBDialogProfile->ItemsGridColor);
  INISetARGB ("ItemsGridColor", a, r, g, b);
  SPLIT_ARGB(FBDialogProfile->InfoBackgroundColor);
  INISetARGB ("InfoBackgroundColor", a, r, g, b);
  SPLIT_ARGB(FBDialogProfile->ScrollBarColor);
  INISetARGB ("ScrollBarColor", a, r, g, b);
  SPLIT_ARGB(FBDialogProfile->ButtonIn);
  INISetARGB ("ButtonIn", a, r, g, b);
  SPLIT_ARGB(FBDialogProfile->ButtonOut);
  INISetARGB ("ButtonOut", a, r, g, b);
  SPLIT_ARGB(FBDialogProfile->ProgressBarColor);
  INISetARGB ("ProgressBarColor", a, r, g, b);
  INISetInt  ("ScrollScreen",     FBDialogProfile->ScrollScreen ? 1 : 0);
  INISetInt  ("ScrollWrapAround", FBDialogProfile->ScrollWrapAround ? 1 : 0);
  INISetInt  ("Fade",             FBDialogProfile->Fade);
  INISetInt  ("FontSize",         FBDialogProfile->FontSize);
  INISetInt  ("HDDAccess",        FBDialogProfile->HDDAccess);

  INISaveFile (FileName, INILocation, AppName);
  INICloseFile();

  INIBuffer = OldBuffer;
  INICommentBuffer = OldCommentBuffer;
  BufferSize = OldBufferSize;

  HDD_TAP_PopDir();

  return TRUE;
}
