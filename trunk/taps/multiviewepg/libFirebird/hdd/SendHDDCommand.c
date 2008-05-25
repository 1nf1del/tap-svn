#include "FBLib_hdd.h"
#include "../libFireBird.h"

volatile byte *HDD_pFeature     = (byte*) 0xb0012d27;   // feature/error register
volatile byte *HDD_pSecCount    = (byte*) 0xb0012d2b;   // sector count register
volatile byte *HDD_pLBALow      = (byte*) 0xb0012d2f;   // sector number register = LBA low register
volatile byte *HDD_pLBAMid      = (byte*) 0xb0012d33;   // cylinder low register = LBA mid register
volatile byte *HDD_pLBAHigh     = (byte*) 0xb0012d37;   // cylinder high register = LBA high register
volatile byte *HDD_pDevice      = (byte*) 0xb0012d3b;   // device/head register
volatile byte *HDD_pStatus      = (byte*) 0xb0012d3f;   // command/status register
volatile byte *HDD_pCommand     = (byte*) 0xb0012d3f;   // command/status register

int SendHDDCommand (byte Feature, byte SecCount, byte LBALow, byte LBAMid, byte LBAHigh, byte Device, byte Command)
{
  int                   Retry;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return 0;

  //ObtainResource (HWRESOURCE_HD, -1);

  Retry = 3;
  do
  {
    busyWait (); *HDD_pFeature = Feature;
    busyWait (); *HDD_pSecCount = SecCount;
    busyWait (); *HDD_pLBALow = LBALow;
    busyWait (); *HDD_pLBAMid = LBAMid;
    busyWait (); *HDD_pLBAHigh = LBAHigh;
    busyWait (); *HDD_pDevice = Device;
    busyWait (); *HDD_pCommand = Command;
    busyWait ();
    Retry--;
  } while (((*HDD_pStatus & 0x01) != 0) && (Retry > 0));
  //ReleaseResource (HWRESOURCE_HD);

  return (*HDD_pStatus & 0x01);
}
