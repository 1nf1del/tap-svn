#include <string.h>
#include "FBLib_rec.h"

//--------------------------------------- HDD_DecodeRECHeader --------------------------------
//
SYSTEM_TYPE HDD_DecodeRECHeader (char *Buffer, tRECHeaderInfo *RECHeaderInfo)
{
  dword                 Frq, SR;
  byte                  Modulation, Polarization, BandWidth, LPHPStream;
  dword                 p;
  dword                 sPoints = 0, tPoints = 0, cPoints = 0;
  word                  v1, v2;

  memset (RECHeaderInfo, 0, sizeof (tRECHeaderInfo));
  RECHeaderInfo->HeaderType = ST_UNKNOWN;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return RECHeaderInfo->HeaderType;

  //Is this a REC header?
  p = 0;
  RECHeaderInfo->HeaderMagic     = *(dword*)(&Buffer [p +  0]);
  if (RECHeaderInfo->HeaderMagic != 0x54467263) return RECHeaderInfo->HeaderType;

  //Header block
  RECHeaderInfo->HeaderVersion   = *( word*)(&Buffer [p +  4]);
  memcpy (RECHeaderInfo->HeaderReserved1, &Buffer [p +  6], 2);
  RECHeaderInfo->HeaderDuration  = *( word*)(&Buffer [p +  8]);
  RECHeaderInfo->HeaderSvcNumber = *( word*)(&Buffer [p + 10]);
  RECHeaderInfo->HeaderSvcType   = *( word*)(&Buffer [p + 12]);
  p += 14;

  //Service Info block
  RECHeaderInfo->SISatIndex  = *( byte*)(&Buffer [p +  0]);
  RECHeaderInfo->SIReserved1 = *( byte*)(&Buffer [p +  1]);
  RECHeaderInfo->SITPIdx     = *( word*)(&Buffer [p +  2]) >> 6;
  RECHeaderInfo->SITunerNum  = (*( word*)(&Buffer [p +  2]) >> 4) & 3;
  RECHeaderInfo->SIDelFlag   = (*( word*)(&Buffer [p +  2]) >> 3) & 1;
  RECHeaderInfo->SICASFlag   = (*( word*)(&Buffer [p +  2]) >> 2) & 1;
  RECHeaderInfo->SILockFlag  = (*( word*)(&Buffer [p +  2]) >> 1) & 1;
  RECHeaderInfo->SISkipFlag  = (*( word*)(&Buffer [p +  2])     ) & 1;
  RECHeaderInfo->SIServiceID = *( word*)(&Buffer [p +  4]);
  RECHeaderInfo->SIPMTPID    = *( word*)(&Buffer [p +  6]);
  RECHeaderInfo->SIPCRPID    = *( word*)(&Buffer [p +  8]);
  RECHeaderInfo->SIVideoPID  = *( word*)(&Buffer [p + 10]);
  RECHeaderInfo->SIAudioPID  = *( word*)(&Buffer [p + 12]);

  if (RECHeaderInfo->HeaderVersion == 0x5010)
  {
    memcpy (RECHeaderInfo->SISvcName, &Buffer [p +  14], 28);
    p += 42;
  }
  else
  {
    memcpy (RECHeaderInfo->SISvcName, &Buffer [p +  14], 24);
    p += 38;
  }

  //Use the following rules for the type decision (thx to jkIT)
  //Count 1 point for every matching rule
  //for dvb-s:
  //  Transponder_Info.Polarity & 0x6F == 0 ?
  //  Transponder_Info.Symbol_Rate in [2000...30000] ?
  //  Transponder_Info.Frequency in [10700...12800] ?
  //for dvb-t
  //  Transponder_Info.Bandwidth in [6..8] ?
  //  Transponder_Info.Frequency in [174000...230000] or [470000...862000] ?
  //  Transponder_Info.LP_HP_Stream & 0xFE == 0 ?
  //for dvb-c
  //  Transponder_Info.Frequency in [47000...862000] ?
  //  Transponder_Info.Symbol_Rate in [2000...30000] ?
  //  Transponder_Info.Modulation <= 4 ?

  //Count cable points
  Frq = *(dword*)(&Buffer [p + 0]);
  SR = *(word*)(&Buffer [p + 4]);
  Modulation = *(byte *) (&Buffer [p + 10]);
  if ((Frq >= 47000) && (Frq <= 862000)) cPoints++;
  if ((SR >= 2000) && (SR <= 30000)) cPoints++;
  if (Modulation <= 4) cPoints++;

  //Count sat points
  Frq = *(dword*)(&Buffer [p + 4]);
  SR = *(word*)(&Buffer [p + 8]);
  Polarization = *(byte *) (&Buffer [p + 1]);
  if ((Frq >= 10700) && (Frq <= 12800)) sPoints++;
  if ((SR >= 2000) && (SR <= 30000)) sPoints++;
  if ((Polarization & 0x6F) == 0) sPoints++;

  //Count terrestrial points
  BandWidth = *(byte *) (&Buffer [p + 2]);
  LPHPStream = *(byte *) (&Buffer [p + 10]);
  if (((Frq >= 174000) && (Frq <= 230000)) || ((Frq >= 470000) && (Frq <= 862000))) tPoints++;
  if ((BandWidth >= 6) && (BandWidth <= 8)) tPoints++;
  if ((LPHPStream & 0xFE) == 0) tPoints++;

  //If one system has 3 points and all other have less than 3, use that DVB system
  if ((sPoints == 3) && (tPoints  < 3) && (cPoints  < 3)) RECHeaderInfo->HeaderType = ST_DVBS;

  if ((sPoints <  3) && (tPoints == 3) && (cPoints  < 3))
  {
    //Try to find out if this is a TF5700 Header: compare the date
    v1 = *(word*)(&Buffer [84]);   // StartMJD
    v2 = *(word*)(&Buffer [88]);   // EndMJD
    RECHeaderInfo->HeaderType = (v1 && v2 && (v1 == v2 || v1 + 1 == v2) ? ST_DVBT5700 : ST_DVBT);
  }

  if ((sPoints <  3) && (tPoints  < 3) && (cPoints == 3)) RECHeaderInfo->HeaderType = ST_DVBC;

#ifdef DEBUG_FIREBIRDLIB
  TAP_Print ("FireBirdLib: DecodeRCEHeader: DVBs=%d, DVBt=%d, DVBc=%d\n", sPoints, tPoints, cPoints);
#endif

  //Transponder block
  switch (RECHeaderInfo->HeaderType)
  {
    case ST_DVBS:
    {
      RECHeaderInfo->TPSatIndex     = *( byte*)(&Buffer [p +  0]);
      RECHeaderInfo->TPPolarization = *( byte*)(&Buffer [p +  1]) >> 7;
      RECHeaderInfo->TPMode         = (*( byte*)(&Buffer [p +  1]) >> 4) & 7;
      RECHeaderInfo->TPReserved3    = (*( byte*)(&Buffer [p +  1])     ) & 15;
      memcpy (RECHeaderInfo->TPReserved1,     &Buffer [p +  2], 2);
      RECHeaderInfo->TPFrequency    = *(dword*)(&Buffer [p +  4]);
      RECHeaderInfo->TPSymbolRate   = *( word*)(&Buffer [p +  8]);
      RECHeaderInfo->TPTSID         = *( word*)(&Buffer [p + 10]);
      memcpy (RECHeaderInfo->TPReserved2,     &Buffer [p + 12], 2);
      RECHeaderInfo->TPNetworkID    = *( word*)(&Buffer [p + 14]);

      p += 16;
      break;
    }

    case ST_DVBT:
    case ST_DVBT5700:
    {
      RECHeaderInfo->TPChannelNumber = *( word*)(&Buffer [p +  0]);
      RECHeaderInfo->TPBandwidth     = *( byte*)(&Buffer [p +  2]);
      RECHeaderInfo->TPReserved3     = *( byte*)(&Buffer [p +  3]);
      RECHeaderInfo->TPFrequency     = *(dword*)(&Buffer [p +  4]);
      RECHeaderInfo->TPTSID          = *( word*)(&Buffer [p +  8]);
      RECHeaderInfo->TPLPHPStream    = *( byte*)(&Buffer [p + 10]);
      RECHeaderInfo->TPReserved4     = *( byte*)(&Buffer [p + 11]);
      RECHeaderInfo->TPNetworkID     = *( word*)(&Buffer [p + 12]);
      memcpy (RECHeaderInfo->TPUnknown1,     &Buffer [p + 14], 2);

      p += 16;

      if (RECHeaderInfo->HeaderType == ST_DVBT5700)
      {
        memcpy(RECHeaderInfo->TPUnknown2, &Buffer[p + 16], 8);
        p += 8;
      }
      break;
    }

    case ST_DVBC:
    {
      RECHeaderInfo->TPFrequency  = *(dword*)(&Buffer [p +  0]);
      RECHeaderInfo->TPSymbolRate = *( word*)(&Buffer [p +  4]);
      RECHeaderInfo->TPTSID       = *( word*)(&Buffer [p +  6]);
      RECHeaderInfo->TPNetworkID  = *( word*)(&Buffer [p +  8]);
      RECHeaderInfo->TPModulation = *( byte*)(&Buffer [p + 10]);
      RECHeaderInfo->TPReserved5  = *( byte*)(&Buffer [p + 11]);

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
  memcpy (RECHeaderInfo->EventUnknown2, &Buffer [p], 2);
  RECHeaderInfo->EventDurationHour   = *( byte*)(&Buffer [p +  2]);
  RECHeaderInfo->EventDurationMin    = *( byte*)(&Buffer [p +  3]);
  RECHeaderInfo->EventEventID        = *(dword*)(&Buffer [p +  4]);
  memcpy (&RECHeaderInfo->EventStartTime, &Buffer [p +  8], 4);
  memcpy (&RECHeaderInfo->EventEndTime  , &Buffer [p + 12], 4);
  RECHeaderInfo->EventRunningStatus  = *( byte*)(&Buffer [p + 16]);
  RECHeaderInfo->EventTextLength     = *( byte*)(&Buffer [p + 17]);
  RECHeaderInfo->EventParentalRate   = *( byte*)(&Buffer [p + 18]);
  memcpy (RECHeaderInfo->EventEventName, &Buffer [p + 19], RECHeaderInfo->EventTextLength);
  memcpy (RECHeaderInfo->EventEventDescription, &Buffer [p + 19 + RECHeaderInfo->EventTextLength], 257 - RECHeaderInfo->EventTextLength);
  memcpy (RECHeaderInfo->EventUnknown1, &Buffer [p + 276], 18);

  p += 294;

  //Extended Event block
  RECHeaderInfo->ExtEventTextLength  = *( word*)(&Buffer [p +  0]);
  RECHeaderInfo->ExtEventEventID     = *(dword*)(&Buffer [p +  2]);
  memcpy (RECHeaderInfo->ExtEventText, &Buffer [p +  6], 1024);

  p += 1030;

  //Crypt Flag block
  memcpy (RECHeaderInfo->CryptReserved1, &Buffer [p], 4);
  RECHeaderInfo->CryptFlag =   *( byte*)(&Buffer [p +  4]);
  memcpy (RECHeaderInfo->CryptReserved2, &Buffer [p + 5], 3);

  p += 8;

  //Bookmark block
  memcpy (RECHeaderInfo->Bookmark, &Buffer [p], 64 * sizeof (dword));

  p += 256;

  RECHeaderInfo->Resume = *( dword*)(&Buffer [p + 0]);

  return RECHeaderInfo->HeaderType;
}
