#include "../libFireBird.h"

void HDD_TouchFile (TYPE_File *fp)
{
  word                  MJD;
  byte                  Hour, Min, Sec;

  TAP_GetTime (&MJD, &Hour, &Min, &Sec);
  HDD_SetFileDateTime (fp, (MJD << 16) | (Hour << 8) | Min, Sec, 0);
}
