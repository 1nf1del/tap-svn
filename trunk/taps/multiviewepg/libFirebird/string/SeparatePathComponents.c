#include                <string.h>
#include                "../libFireBird.h"

void SeparatePathComponents (char *FullName, char *Path, char *FileName, char *FileExt)
{
  char                  *Slash, *Dot;

  if (FullName == NULL) return;

  Slash = strrchr (FullName, '/');
  Dot   = strrchr (FullName, '.');
  if (Dot < Slash) Dot = NULL;

  if (Path)
  {
    if (Slash)
    {
      strncpy (Path, FullName, Slash - FullName + 1);
      Path [Slash - FullName + 1] = '\0';
    }
    else Path [0] = '\0';
  }

  //Dot needs to be > Slash else it is not part of the file name
  if (FileExt)
  {
    if (Dot) strcpy (FileExt, Dot);
        else FileExt [0] = '\0';
  }

  if (FileName)
  {
    if (!Slash) Slash = FullName - 1;
    if (!Dot) Dot = strrchr (FullName, '\0');
    strncpy (FileName, Slash + 1, Dot - Slash - 1);
    FileName [Dot - Slash - 1] = '\0';
  }
}
