#include <string.h>
#include "FBLib_hdd.h"
#include "../libFireBird.h"

bool HDD_ChangeDir (char *Dir)
{
  char                  D [FBLIB_DIR_SIZE];
  char                  *d, *p;

  InitTAPAPIFix();

  strncpy (D, Dir, sizeof(D) - 1);  // we need 1 char for a terminating / later
  D[sizeof(D) - 2] = '\0';

  if (*D == '/')
  {
    if (!ChangeDirRoot()) return FALSE;
    strcpy (D, D + 1);
  }

  strcat (D, "/");
  d = D;

  while ((p = strchr (d, '/')))
  {
    *p = '\0';
    if (*d)
    {
      if (TAP_Hdd_ChangeDir(d) == 0) return FALSE;
    }
    d = p + 1;
  }

  return TRUE;
}
