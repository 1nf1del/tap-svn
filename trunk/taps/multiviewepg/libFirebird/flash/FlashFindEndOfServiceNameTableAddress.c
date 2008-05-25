#include "FBLib_flash.h"

//--------------------------------------- FlashFindEndOfServiceNameTableAddress --------------------------------
//
dword FlashFindEndOfServiceNameTableAddress (void)
{
  dword                 LastEntry = 0;
  TYPE_Service          *Svc;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0)) return 0;

  //Find the highest service name table pointer in the TV services
  (dword) Svc = FlashBlockOffset [TVServices] + 4;
  while (Svc->TunerNum != 0)
  {
    if (Svc->NameOffset > LastEntry) LastEntry = Svc->NameOffset;
    Svc++;
  }

  //Repeat for all radio services
  (dword) Svc = FlashBlockOffset [RadioServices] + 4;
  while (Svc->TunerNum != 0)
  {
    if (Svc->NameOffset > LastEntry) LastEntry = Svc->NameOffset;
    Svc++;
  }

  LastEntry = LastEntry + FlashBlockOffset [ServiceNames] + 4;
  LastEntry += strlen ((char*)LastEntry);

  return LastEntry + 1;
}
