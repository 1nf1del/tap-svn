#include <string.h>
#include "FBLib_string.h"

char *ParseLine (char *zeile, size_t *n, char delim)
{
  char *p;

  if (!zeile || !*zeile)
  {
    *n = 0;
    return NULL;
  }

  if (*zeile == delim) zeile++;

  if ((p = strchr(zeile, delim)))
  {
    *n = p - zeile;
    return zeile;
  }
  else
  {
    *n = strlen(zeile);
    return zeile;
  }
}
