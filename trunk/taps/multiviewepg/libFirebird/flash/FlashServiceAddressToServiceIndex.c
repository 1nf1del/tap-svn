#include "FBLib_flash.h"

//--------------------------------------- FlashServiceAddressToServiceIndex --------------------------------
//
word FlashServiceAddressToServiceIndex (TYPE_Service *ServiceAddress)
{
  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0)) return 0;

  if ((dword) ServiceAddress >= FlashBlockOffset [RadioServices]) return (((dword) ServiceAddress - FlashBlockOffset [RadioServices]) / sizeof(TYPE_Service));
                                                             else return (((dword) ServiceAddress - FlashBlockOffset [TVServices]) / sizeof(TYPE_Service));
}
