#include "FBLib_rec.h"

// returns:
//          0, meaning failure
//          1, meaning ok - no changes
//          2, meaning playtime adjusted

int HDD_RecalcPlaytime (TYPE_File *f, tRECHeaderInfo *Header)
{
  byte *buffer;
  int cluster;
  dword FirstPCR, LastPCR;
  int success = 0;

  if ((buffer = TAP_MemAlloc(PCRSECTORS << 9)))
  {
    if ((cluster = HDD_LiveFS_GetLastCluster(f->startCluster)) > 0)
    {
      if ((cluster = HDD_LiveFS_GetPreviousCluster(cluster)) > 0)
      {
        HDD_ReadSectorDMA((cluster + 1) * HDD_GetClusterSize(), PCRSECTORS, buffer);
        LastPCR = HDD_FindPCR(buffer, PCRSECTORS << 9);

        HDD_ReadSectorDMA((f->startCluster + 1) * HDD_GetClusterSize(), PCRSECTORS, buffer);
        FirstPCR = HDD_FindPCR(buffer, PCRSECTORS << 9);

        if (FirstPCR && LastPCR && (HDD_DecodeRECHeader(buffer, Header) != ST_UNKNOWN))
        {
          success = 1;

          // check whether the last PCR is smaller than the first one (PCR overrun)
          // and fix this by adding 1591 minutes (2^33 / 90 kHz / 60)
          if (LastPCR < FirstPCR) LastPCR += 1591;

          if (Header->HeaderDuration != (word) (LastPCR - FirstPCR))
          {
            Header->HeaderDuration = (word) (LastPCR - FirstPCR);
            HDD_EncodeRECHeader(buffer, Header, Header->HeaderType);
            HDD_WriteSectorDMA((f->startCluster + 1) * HDD_GetClusterSize(), (sizeof(tRECHeaderInfo) >> 9) + 1, buffer);
            success = 2;
          }
        }
      }
    }

    TAP_MemFree(buffer);
  }

  return success;
}
