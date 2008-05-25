#include "../libFireBird.h"

//-------------------------------- HDD_FindPMT ---------------------------
//
bool HDD_FindPMT (byte *pBuffer, dword BufferSize, tRECHeaderInfo *RECHeaderInfo)
{
  dword                 i = 0, j;
  byte                  ANDMask [6] = {0xFF, 0xC0, 0x00, 0xD0, 0xFF, 0xFF};
  byte                  PMTMask [6] = {0x47, 0x40, 0x00, 0x10, 0x00, 0x02};
  dword                 DescrPt;
  short                 SectionLength, DescriptorLength, ProgramInfoLength;
  bool                  AudioFound = FALSE, VideoFound = FALSE;
  bool                  AdaptationField;
  dword                 AdaptationFieldLength = 0;

  do
  {
    //PMTs may contain an adaptation field
    AdaptationField = ((pBuffer [i + 3] & 0x20) != 0);
    if (AdaptationField) AdaptationFieldLength = pBuffer [i + 4] + 1;
                    else AdaptationFieldLength = 0;

    //Check if the first 6 bytes of an TS packet matches a PMT packet
    for (j = 0; j < 6; j++)
    {
      if (j > 3)
      {
        if ((pBuffer [i + j + AdaptationFieldLength] & ANDMask [j]) != PMTMask [j]) break;
      }
      else
      {
        if ((pBuffer [i + j] & ANDMask [j]) != PMTMask [j]) break;
      }
    }

    //If the above loop didn't break, we've found a PMT
    if (j > 5)
    {
#ifdef DEBUG_FIREBIRDLIB
      TAP_Print ("PMT found @ 0x%8.8x\n", i);
#endif

      //The following IDs have a constant distance from the packet header
      memcpy (&RECHeaderInfo->SIPMTPID, pBuffer + i + 0x01, 2);
      RECHeaderInfo->SIPMTPID = RECHeaderInfo->SIPMTPID & 0x1FFF;
      memcpy (&RECHeaderInfo->SIServiceID, pBuffer + i + AdaptationFieldLength + 0x08, 2);
      memcpy (&RECHeaderInfo->SIPCRPID, pBuffer + i + AdaptationFieldLength + 0x0d, 2);
      RECHeaderInfo->SIPCRPID = RECHeaderInfo->SIPCRPID & 0x1FFF;

      memcpy (&SectionLength, pBuffer + i + AdaptationFieldLength + 0x06, 2);
      SectionLength &= 0xFFF;
      memcpy (&ProgramInfoLength, pBuffer + i + AdaptationFieldLength + 0x0f, 2);
      ProgramInfoLength &= 0xFFF;

      //Run through all descriptors and search for the Audio and Video descriptor
      SectionLength -= (ProgramInfoLength + 9 + 4);
      DescrPt = i + ProgramInfoLength + AdaptationFieldLength + 17;
      while (SectionLength > 0)
      {
      	memcpy (&DescriptorLength, pBuffer + DescrPt + 3, 2);
      	DescriptorLength &= 0x0FFF;
#ifdef DEBUG_FIREBIRDLIB
        TAP_Print ("SectionLength = %d, DescrPt = 0x%8.8x, DescriptorLength = %d\n", SectionLength, DescrPt, DescriptorLength);
#endif

      	if (pBuffer [DescrPt] == 2)		//MPEG2 Video Descriptor
      	{
#ifdef DEBUG_FIREBIRDLIB
      	  TAP_Print ("MPEG2 Video descriptor @ 0x%4.4x\n", DescrPt);
#endif
      	  memcpy (&RECHeaderInfo->SIVideoPID, pBuffer + DescrPt + 1, 2);
      	  RECHeaderInfo->SIVideoPID = RECHeaderInfo->SIVideoPID & 0x1FFF;
      	  VideoFound=TRUE;
        }

        if (pBuffer [DescrPt] == 3)		//MPEG1 Audio Descriptor
        {
#ifdef DEBUG_FIREBIRDLIB
          TAP_Print ("MPEG1 Audio descriptor @ 0x%4.4x\n", DescrPt);
#endif
          memcpy (&RECHeaderInfo->SIAudioPID, pBuffer + DescrPt + 1, 2);
      	  RECHeaderInfo->SIAudioPID = ((RECHeaderInfo->SIAudioPID & 0x1FFF) | 0x8000);
          AudioFound=TRUE;
        }

        if (pBuffer [DescrPt] == 4)		//MPEG2 Audio Descriptor
        {
#ifdef DEBUG_FIREBIRDLIB
          TAP_Print ("MPEG2 Audio descriptor @ 0x%4.4x\n", DescrPt);
#endif
          memcpy (&RECHeaderInfo->SIAudioPID, pBuffer + DescrPt + 1, 2);
      	  RECHeaderInfo->SIAudioPID = ((RECHeaderInfo->SIAudioPID & 0x1FFF) | 0x8000);
          AudioFound=TRUE;
        }

        if (VideoFound && AudioFound) break;

        SectionLength -= (DescriptorLength + 5);
        DescrPt += (DescriptorLength + 5);
      }

#ifdef DEBUG_FIREBIRDLIB
      TAP_Print ("SID=0x%4.4x, PMT=0x%4.4x, PCR PID=0x%4.4x, VPID=0x%4.4x, APID=0x%4.4x\n", RECHeaderInfo->SIServiceID,
                                                                                            RECHeaderInfo->SIPMTPID,
                                                                                            RECHeaderInfo->SIPCRPID,
                                                                                            RECHeaderInfo->SIVideoPID,
                                                                                            RECHeaderInfo->SIAudioPID);
#endif

      //A PMT has been located but it didn't contain an Audio or Video PID
      if (!AudioFound & !VideoFound) return FALSE;

      return TRUE;
    }
    i += 188;
  } while (i < (BufferSize - 188));

#ifdef DEBUG_FIREBIRDLIB
      TAP_Print ("No PMT found\n", i);
#endif

  //Nothing has been found
  return FALSE;
}
