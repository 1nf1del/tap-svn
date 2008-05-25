#include <string.h>
#include "FBLib_hdd.h"
#include "../libFireBird.h"

bool HDD_GetHddID (char *ModelNo, char *SerialNo, char *FirmwareNo)
{
  char                  Buffer [512];

  if (!HDD_IdentifyDevice (Buffer)) return FALSE;

  if (ModelNo)
  {
    strncpy (ModelNo, &Buffer [0x36], 40);
    ModelNo [40] = 0x00;
    RTrim(ModelNo);
  }

  if (SerialNo)
  {
    strncpy (SerialNo, &Buffer [0x14], 20);
    SerialNo [20] = 0x00;
    RTrim(SerialNo);
  }

  if (FirmwareNo)
  {
    strncpy (FirmwareNo, &Buffer [0x2e], 8);
    FirmwareNo [8] = 0x00;
    RTrim(FirmwareNo);
  }

  return TRUE;
}
