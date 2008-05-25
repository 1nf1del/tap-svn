#include "../libFireBird.h"

//--------------------------------------- HDD_EncodeRECHeader --------------------------------
//
void HDD_EncodeRECHeader (char *Buffer, tRECHeaderInfo *RECHeaderInfo, SYSTEM_TYPE HeaderType)
{
  dword                 p;

  //Header block
  p = 0;
  *(dword*)(&Buffer [p +  0]) = RECHeaderInfo->HeaderMagic;
  *( word*)(&Buffer [p +  4]) = RECHeaderInfo->HeaderVersion;
  memcpy   (&Buffer [p +  6], RECHeaderInfo->HeaderReserved1, 2);
  *( word*)(&Buffer [p +  8]) = RECHeaderInfo->HeaderDuration;
  *( word*)(&Buffer [p + 10]) = RECHeaderInfo->HeaderSvcNumber;
  *( word*)(&Buffer [p + 12]) = RECHeaderInfo->HeaderSvcType;
  p += 14;

  //Service Info block
  if (HeaderType == ST_DVBS) *( byte*)(&Buffer [p +  0]) = RECHeaderInfo->SISatIndex;
  else                       *( byte*)(&Buffer [p +  0]) = 0;

  *( byte*)(&Buffer [p +  1]) = RECHeaderInfo->SIReserved1;

  RECHeaderInfo->SITPIdx    &= 0x03ff;
  RECHeaderInfo->SITunerNum &= 0x0003;
  RECHeaderInfo->SIDelFlag  &= 0x01;
  RECHeaderInfo->SICASFlag  &= 0x01;
  RECHeaderInfo->SILockFlag &= 0x01;
  RECHeaderInfo->SISkipFlag &= 0x01;

  *( word*)(&Buffer [p +  2]) = (RECHeaderInfo->SITPIdx    << 6) |
                                (RECHeaderInfo->SITunerNum << 4) |
                                (RECHeaderInfo->SIDelFlag  << 3) |
                                (RECHeaderInfo->SICASFlag  << 2) |
                                (RECHeaderInfo->SILockFlag << 1) |
                                 RECHeaderInfo->SISkipFlag;
  *( word*)(&Buffer [p +  4]) = RECHeaderInfo->SIServiceID;
  *( word*)(&Buffer [p +  6]) = RECHeaderInfo->SIPMTPID;
  *( word*)(&Buffer [p +  8]) = RECHeaderInfo->SIPCRPID;
  *( word*)(&Buffer [p + 10]) = RECHeaderInfo->SIVideoPID;
  *( word*)(&Buffer [p + 12]) = RECHeaderInfo->SIAudioPID;

  if (RECHeaderInfo->HeaderVersion == 0x5010)
  {
    memcpy (&Buffer [p +  14], RECHeaderInfo->SISvcName, 28);
    p += 42;
  }
  else
  {
    memcpy (&Buffer [p +  14], RECHeaderInfo->SISvcName, 24);
    p += 38;
  }

  //Transponder block
  switch (HeaderType)
  {
    case ST_DVBS:
    {
      if (RECHeaderInfo->HeaderType != HeaderType)
      {
        RECHeaderInfo->TPSatIndex     = 0;
        RECHeaderInfo->TPPolarization = 0;
        RECHeaderInfo->TPMode         = 0;
        RECHeaderInfo->TPReserved3    = 0;
        RECHeaderInfo->TPFrequency    = 10700;
        RECHeaderInfo->TPSymbolRate   = 27500;
        RECHeaderInfo->TPTSID         = 0;
        RECHeaderInfo->TPNetworkID    = 0;
      }

      *( byte*)(&Buffer [p +  0]) = RECHeaderInfo->TPSatIndex;
      RECHeaderInfo->TPPolarization &= 1;
      RECHeaderInfo->TPMode         &= 7;
      RECHeaderInfo->TPReserved3    &= 15;
      *( byte*)(&Buffer [p +  1]) = (RECHeaderInfo->TPPolarization << 7) | (RECHeaderInfo->TPMode << 4) | RECHeaderInfo->TPReserved3;
      memcpy   (&Buffer [p +  2], RECHeaderInfo->TPReserved1, 2);
      *(dword*)(&Buffer [p +  4]) = RECHeaderInfo->TPFrequency;
      *( word*)(&Buffer [p +  8]) = RECHeaderInfo->TPSymbolRate;
      *( word*)(&Buffer [p + 10]) = RECHeaderInfo->TPTSID;
      memcpy   (&Buffer [p + 12], RECHeaderInfo->TPReserved2, 2);
      *( word*)(&Buffer [p + 14]) = RECHeaderInfo->TPNetworkID;

      p += 16;
      break;
    }

    case ST_DVBT:
    case ST_DVBT5700:
    {
      if (RECHeaderInfo->HeaderType != HeaderType)
      {
        RECHeaderInfo->TPChannelNumber = 0;
        RECHeaderInfo->TPBandwidth     = 6;
        RECHeaderInfo->TPFrequency     = 174000;
        RECHeaderInfo->TPTSID          = 0;
        RECHeaderInfo->TPLPHPStream    = 0;
        RECHeaderInfo->TPNetworkID     = 0;
      }

      *( word*)(&Buffer [p +  0]) = RECHeaderInfo->TPChannelNumber;
      *( byte*)(&Buffer [p +  2]) = RECHeaderInfo->TPBandwidth;
      *( byte*)(&Buffer [p +  3]) = RECHeaderInfo->TPReserved3;
      *(dword*)(&Buffer [p +  4]) = RECHeaderInfo->TPFrequency;
      *( word*)(&Buffer [p +  8]) = RECHeaderInfo->TPTSID;
      *( byte*)(&Buffer [p + 10]) = RECHeaderInfo->TPLPHPStream;
      *( byte*)(&Buffer [p + 11]) = RECHeaderInfo->TPReserved4;
      *( word*)(&Buffer [p + 12]) = RECHeaderInfo->TPNetworkID;
      memcpy   (&Buffer [p + 14], RECHeaderInfo->TPUnknown1, 2);

      p += 16;

      if (HeaderType == ST_DVBT5700)
      {
        memcpy(&Buffer[p + 16], RECHeaderInfo->TPUnknown2, 8);
        p += 8;
      }
      break;
    }

    case ST_DVBC:
    {
      if (RECHeaderInfo->HeaderType != HeaderType)
      {
        RECHeaderInfo->TPFrequency  = 47000;
        RECHeaderInfo->TPSymbolRate = 2000;
        RECHeaderInfo->TPTSID       = 0;
        RECHeaderInfo->TPNetworkID  = 0;
        RECHeaderInfo->TPModulation = 0;
      }

      *(dword*)(&Buffer [p +  0]) = RECHeaderInfo->TPFrequency;
      *( word*)(&Buffer [p +  4]) = RECHeaderInfo->TPSymbolRate;
      *( word*)(&Buffer [p +  6]) = RECHeaderInfo->TPTSID;
      *( word*)(&Buffer [p +  8]) = RECHeaderInfo->TPNetworkID;
      *( byte*)(&Buffer [p + 10]) = RECHeaderInfo->TPModulation;
      *( byte*)(&Buffer [p + 11]) = RECHeaderInfo->TPReserved5;

      p += 12;
      break;
    }

    default:
    {
      p += 16;
      break;
    }
  }

  //Event block
  memcpy   (&Buffer [p], RECHeaderInfo->EventUnknown2, 2);
  *( byte*)(&Buffer [p +  2]) = RECHeaderInfo->EventDurationHour;
  *( byte*)(&Buffer [p +  3]) = RECHeaderInfo->EventDurationMin;
  *(dword*)(&Buffer [p +  4]) = RECHeaderInfo->EventEventID;
  memcpy (&Buffer [p +  8], &RECHeaderInfo->EventStartTime, 4);
  memcpy (&Buffer [p + 12], &RECHeaderInfo->EventEndTime  , 4);
  *( byte*)(&Buffer [p + 16]) = RECHeaderInfo->EventRunningStatus;
  *( byte*)(&Buffer [p + 17]) = RECHeaderInfo->EventTextLength;
  *( byte*)(&Buffer [p + 18]) = RECHeaderInfo->EventParentalRate;
  memcpy (&Buffer [p + 19], RECHeaderInfo->EventEventName, RECHeaderInfo->EventTextLength);
  memcpy (&Buffer [p + 19 + RECHeaderInfo->EventTextLength], RECHeaderInfo->EventEventDescription, 257 - RECHeaderInfo->EventTextLength);
  memcpy (&Buffer [p + 276], RECHeaderInfo->EventUnknown1, 18);

  p += 294;

  //Extended Event block
  *( word*)(&Buffer [p +  0]) = RECHeaderInfo->ExtEventTextLength;
  *(dword*)(&Buffer [p +  2]) = RECHeaderInfo->ExtEventEventID;
  RECHeaderInfo->ExtEventText [1023] = 0x00;
  memcpy (&Buffer [p +  6], RECHeaderInfo->ExtEventText, 1024);

  p += 1030;

  //Crypt Flag block
  memcpy   (&Buffer [p], RECHeaderInfo->CryptReserved1, 4);
  *( byte*)(&Buffer [p + 4]) = RECHeaderInfo->CryptFlag;
  memcpy   (&Buffer [p + 5], RECHeaderInfo->CryptReserved2, 3);

  p += 8;

  //Bookmark block
  memcpy (&Buffer [p], RECHeaderInfo->Bookmark, 64 * sizeof (dword));

  p += 256;

  *( dword*)(&Buffer [p + 0]) = RECHeaderInfo->Resume;
}
