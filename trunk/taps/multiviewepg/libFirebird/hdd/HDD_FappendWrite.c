#include <stdio.h>
#include "../libFireBird.h"

bool HDD_FappendWrite (TYPE_File *file, char *data)
{
  char buffer[256 + 512];
  dword len, pos, extra, blks;

  InitTAPAPIFix();

  len = strlen(data);

  if (file == NULL || len > 256) return FALSE;

  strcpy(buffer, data);
  memset(buffer + len, 0, sizeof(buffer) - len);
  pos = file->currentPos;
  extra = 512 - ((pos + len) % 512);

  if (extra == 512) extra = 0;

  if ((blks = TAP_Hdd_Fwrite(buffer, len + extra, 1, file)) != 1) return FALSE;

  pos += len;

  return (TAP_Hdd_Fseek(file, pos, SEEK_SET) == pos);
}
