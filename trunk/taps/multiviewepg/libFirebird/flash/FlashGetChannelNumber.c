#include "FBLib_flash.h"

//--------------------------------------- FlashGetChannelNumber --------------------------------
//
void FlashGetChannelNumber (int chType, int chNum, dword *Frequency, dword *ChannelNumber)
{
  TYPE_Service          *FlashService;
  TYPE_TpInfoT          *FlashTpInfo;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0)) return;

  //Don't do anything on non DVB-ts
  if (SystemType != ST_DVBT)
  {
    *Frequency = 0;
    *ChannelNumber = 0;
    return;
  }

  //Let FlashService point to the TV or radio service table
  if (chType == SVC_TYPE_Tv) FlashService = (TYPE_Service *) (FlashBlockOffset [TVServices] + 4);
                        else FlashService = (TYPE_Service *) (FlashBlockOffset [RadioServices] + 4);

  //Set a pointer to the transponder table
  FlashTpInfo = (TYPE_TpInfoT *) (FlashBlockOffset [Transponders] + 8);

  //Return both parameters
  *Frequency  = FlashTpInfo [FlashService [chNum].TPIdx].Frequency;
  *ChannelNumber = FlashTpInfo [FlashService [chNum].TPIdx].ChannelNr;
}
