#include                "../ini/FBLib_ini.h"
#include                "FBLib_dialog.h"

INILOCATION ProfileLoad (char *FileName, bool useDefaults, char *AppName)
{
  char                    *OldBuffer, *OldCommentBuffer;
  dword                   OldBufferSize;
  byte                    a, r, g, b;
  INILOCATION             ProfileLocation;

  if (!FBDialogProfile) return INILOCATION_NotFound;

  OldBuffer = INIBuffer;
  OldCommentBuffer = INICommentBuffer;
  OldBufferSize = BufferSize;


  ProfileLocation = INIOpenFile (FileName, AppName);

  if (ProfileLocation > INILOCATION_NewFile)
  {
    (void) INIGetARGB ("BorderColor", &a, &r, &g, &b, (useDefaults ? DEFAULTBorderColor : FBDialogProfile->BorderColor));
    FBDialogProfile->BorderColor = ARGB(a, r, g, b);
    (void) INIGetARGB ("TitleBackgroundColor", &a, &r, &g, &b, (useDefaults ? DEFAULTTitleBackgroundColor : FBDialogProfile->TitleBackgroundColor));
    FBDialogProfile->TitleBackgroundColor = ARGB(a, r, g, b);
    (void) INIGetARGB ("TitleTextColor", &a, &r, &g, &b, (useDefaults ? DEFAULTTitleTextColor : FBDialogProfile->TitleTextColor));
    FBDialogProfile->TitleTextColor = ARGB(a, r, g, b);
    (void) INIGetARGB ("ItemsLineBackgroundColor", &a, &r, &g, &b, (useDefaults ? DEFAULTItemsLineBackgroundColor : FBDialogProfile->ItemsLineBackgroundColor));
    FBDialogProfile->ItemsLineBackgroundColor = ARGB(a, r, g, b);
    (void) INIGetARGB ("ItemsLineBackgroundSelectedColor", &a, &r, &g, &b, (useDefaults ? DEFAULTItemsLineBackgroundSelectedColor : FBDialogProfile->ItemsLineBackgroundSelectedColor));
    FBDialogProfile->ItemsLineBackgroundSelectedColor = ARGB(a, r, g, b);
    (void) INIGetARGB ("ItemsIconBackgroundColor", &a, &r, &g, &b, (useDefaults ? DEFAULTItemsIconBackgroundColor : FBDialogProfile->ItemsIconBackgroundColor));
    FBDialogProfile->ItemsIconBackgroundColor = ARGB(a, r, g, b);
    (void) INIGetARGB ("ItemsIconBackgroundSelectedColor", &a, &r, &g, &b, (useDefaults ? DEFAULTItemsIconBackgroundSelectedColor : FBDialogProfile->ItemsIconBackgroundSelectedColor));
    FBDialogProfile->ItemsIconBackgroundSelectedColor = ARGB(a, r, g, b);
    (void) INIGetARGB ("ItemsTextColor", &a, &r, &g, &b, (useDefaults ? DEFAULTItemsTextColor : FBDialogProfile->ItemsTextColor));
    FBDialogProfile->ItemsTextColor = ARGB(a, r, g, b);
    (void) INIGetARGB ("ItemsTextDisabledColor", &a, &r, &g, &b, (useDefaults ? DEFAULTItemsTextDisabledColor : FBDialogProfile->ItemsTextDisabledColor));
    FBDialogProfile->ItemsTextDisabledColor = ARGB(a, r, g, b);
    (void) INIGetARGB ("ItemsTextSelectedColor", &a, &r, &g, &b, (useDefaults ? DEFAULTItemsTextSelectedColor : FBDialogProfile->ItemsTextSelectedColor));
    FBDialogProfile->ItemsTextSelectedColor = ARGB(a, r, g, b);
    (void) INIGetARGB ("ItemsGridColor", &a, &r, &g, &b, (useDefaults ? DEFAULTItemsGridColor : FBDialogProfile->ItemsGridColor));
    FBDialogProfile->ItemsGridColor = ARGB(a, r, g, b);
    (void) INIGetARGB ("InfoBackgroundColor", &a, &r, &g, &b, (useDefaults ? DEFAULTInfoBackgroundColor : FBDialogProfile->InfoBackgroundColor));
    FBDialogProfile->InfoBackgroundColor = ARGB(a, r, g, b);
    (void) INIGetARGB ("ScrollBarColor", &a, &r, &g, &b, (useDefaults ? DEFAULTScrollBarColor : FBDialogProfile->ScrollBarColor));
    FBDialogProfile->ScrollBarColor = ARGB(a, r, g, b);
    (void) INIGetARGB ("ButtonIn", &a, &r, &g, &b, (useDefaults ? DEFAULTButtonIn : FBDialogProfile->ButtonIn));
    FBDialogProfile->ButtonIn = ARGB(a, r, g, b);
    (void) INIGetARGB ("ButtonOut", &a, &r, &g, &b, (useDefaults ? DEFAULTButtonOut : FBDialogProfile->ButtonOut));
    FBDialogProfile->ButtonOut = ARGB(a, r, g, b);
    (void) INIGetARGB ("ProgressBarColor", &a, &r, &g, &b, (useDefaults ? DEFAULTProgressBarColor : FBDialogProfile->ProgressBarColor));
    FBDialogProfile->ProgressBarColor = ARGB(a, r, g, b);
    FBDialogProfile->Fade             = INIGetInt ("Fade",             (useDefaults ? DEFAULTFade : FBDialogProfile->Fade), 0x0000, 0xffff);
    FBDialogProfile->FontSize         = INIGetInt ("FontSize",         (useDefaults ? DEFAULTFontSize : FBDialogProfile->FontSize), 0, 1);
    FBDialogProfile->HDDAccess        = INIGetInt ("HDDAccess",        (useDefaults ? DEFAULTHDDAccess : FBDialogProfile->HDDAccess), 0, 2);
    FBDialogProfile->ScrollScreen     = INIGetInt ("ScrollScreen"    , (useDefaults ? DEFAULTScrollScreen : FBDialogProfile->ScrollScreen)     ? 1 : 0, 0, 1) == 1;
    FBDialogProfile->ScrollWrapAround = INIGetInt ("ScrollWrapAround", (useDefaults ? DEFAULTScrollWrapAround : FBDialogProfile->ScrollWrapAround) ? 1 : 0, 0, 1) == 1;

    FBDialogProfile->Magic = DIALOGPROFILE_MAGIC;

    INICloseFile();
    ProfileDirty();
  }

  INIBuffer = OldBuffer;
  INICommentBuffer = OldCommentBuffer;
  BufferSize = OldBufferSize;

  return ProfileLocation;
}
