#include "../libFireBird.h"

//--------------------------------------- StrEndsWith --------------------------------
//
bool StrEndsWith(char *text, char *postfix)
{

  char                  *pStr = text + (strlen(text) - strlen(postfix));
  char                  *pPost = postfix;

  if (pStr < text) return FALSE;

  if (!*pPost) return TRUE;

  do
  {
    if (*pStr != *pPost) return FALSE;
    pStr++;
    pPost++;
  } while (*pStr);

  return TRUE;
}
