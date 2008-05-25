#include <string.h>
#include "FBLib_ini.h"
#include "../libFireBird.h"

bool INIKeyExists (char *Key)
{
  char                  *i = NULL, *j = NULL;
  char                  TempKey [80];

  if (!Key || !INIBuffer) return FALSE;

  strncpy (TempKey, Key, sizeof(TempKey) - 2);
  TempKey[sizeof(TempKey) - 2] = '\0';
  strcat (TempKey, "=");

  INIFindStartEnd (TempKey, &i, &j, BufferSize);

  return (i && j);
}
