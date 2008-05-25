#include <string.h>
#include "FBLib_ini.h"
#include "../libFireBird.h"

dword                   LangNrStrings = 0;
dword                   *LangStringPtr = NULL;
char                    *LangStrings   = NULL;

INILOCATION LangLoadStrings (char *LangFile, dword NrStrings, int FallbackLang, char *AppName)
{
  dword                 i;
  int                   OSDLan;
  char                  Key [10], s [200], *c;
  dword                 TotalLength;
  char                  *OldBuffer, *OldCommentBuffer;
  dword                 OldBufferSize;
  INILOCATION           LangLocation;

  if (LangStringPtr || LangStrings) LangUnloadStrings();

  OSDLan = TAP_GetSystemVar (SYSVAR_OsdLan);

  if ((NrStrings > 999999) || !iso639_1(OSDLan) || (FallbackLang > LAN_Bulgarian)) return INILOCATION_NotFound;

  OldBuffer = INIBuffer;
  OldCommentBuffer = INICommentBuffer;
  OldBufferSize = BufferSize;
  HDD_TAP_PushDir();
  if ((LangLocation = INIOpenFile (LangFile, AppName)) < INILOCATION_AtCurrentDir)
  {
    if (LangLocation == INILOCATION_NewFile) INICloseFile();
    HDD_TAP_PopDir();
    return INILOCATION_NotFound;
  }

  //First loop: count the length of all strings
  LangStringPtr = TAP_MemAlloc (NrStrings * sizeof(dword));
  memset (LangStringPtr, 0, NrStrings * sizeof(dword));
  TotalLength = 0;
  for (i = 0; i < NrStrings; i++)
  {
    LangStringPtr [i] = TotalLength;
    TAP_SPrint (Key, "%s_%3.3d", iso639_1(OSDLan), i);
    INIGetString (Key, s, "", sizeof(s));

    //If the string is not available in the current language, use the fallback language
    if (!s[0] && (FallbackLang >= 0))
    {
      TAP_SPrint (Key, "%s_%3.3d", iso639_1(FallbackLang), i);
      INIGetString (Key, s, "", sizeof(s));
    }

    //Convert control characters
    c = s;
    while ((c = strchr (c, '\\')))
    {
      if (c[1] == 'r') *c ='\r';
      else if (c[1] == 'n') *c ='\n';
      else continue;

      memmove(c + 1, c + 2, strchr(c, '\0') - c - 1);
    }
    TotalLength += (strlen (s) + 1);
  }

  //Second loop: Read the strings
  LangStrings = TAP_MemAlloc (TotalLength);
  memset (LangStrings, 0, TotalLength);
  for (i = 0; i < NrStrings; i++)
  {
    TAP_SPrint (Key, "%s_%3.3d", iso639_1(OSDLan), i);
    INIGetString (Key, s, "", sizeof(s));

    //If the string is not available in the current language, use the fallback language
    if (!s[0] && (FallbackLang >= 0))
    {
      TAP_SPrint (Key, "%s_%3.3d", iso639_1(FallbackLang), i);
      INIGetString (Key, s, "", sizeof(s));
    }

    //Convert control characters
    c = s;
    while ((c = strchr (c, '\\')))
    {
      if (c[1] == 'r') *c ='\r';
      else if (c[1] == 'n') *c ='\n';
      else continue;

      memmove(c + 1, c + 2, strchr(c, '\0') - c - 1);
    }

    strcpy(&LangStrings [LangStringPtr [i]], s);
  }

  INICloseFile();

  INIBuffer = OldBuffer;
  INICommentBuffer = OldCommentBuffer;
  BufferSize = OldBufferSize;

  LangNrStrings = NrStrings;

  HDD_TAP_PopDir();

  return LangLocation;
}
