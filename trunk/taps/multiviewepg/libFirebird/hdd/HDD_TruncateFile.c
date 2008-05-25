#include "../libFireBird.h"

bool HDD_TruncateFile (TYPE_File *File, dword Size)
{
  dword                 TotalCluster, UnusedBytes, NrSectors;
  word                  BytesUsed;
  byte                  BytesUsedCRC;

  if (!File || !(tFileSlot*)File->handle) return FALSE;

  InitTAPAPIFix();

  if (Size > File->currentPos) return FALSE;

  if (Size == 0)
  {
    TotalCluster  = 1;
    UnusedBytes   = HDD_GetClusterSize() << 9;
    BytesUsed     = 0;
    BytesUsedCRC  = 0;
  }
  else
  {
    TotalCluster  = (Size - 1) / (HDD_GetClusterSize() << 9) + 1;
    Size         -= (HDD_GetClusterSize() * (TotalCluster - 1) << 9);
    NrSectors     = (Size & 0x1ff) ? (Size >> 9) + 1 : (Size >> 9);
    UnusedBytes   = (HDD_GetClusterSize() - NrSectors) << 9;
    BytesUsed     = Size & 0x1ff;
    BytesUsedCRC  = ((BytesUsed >> 8) + (BytesUsed & 0xff) + 1) & 0xff;
  }

  if (((tFileSlot*)File->handle)->TotalCluster != TotalCluster)
  {
    //Truncation beyond a cluster boundary is not yet supported

    return FALSE;

    //((tFileSlot*)File->handle)->TotalCluster  = TotalCluster;

    //dword     HDD_LiveFS_GetFAT1Address (void);
    //dword     HDD_LiveFS_GetFAT2Address (void);

  }

  ((tFileSlot*)File->handle)->UnusedByte    = UnusedBytes;
  ((tFileSlot*)File->handle)->BytesUsed     = BytesUsed;
  ((tFileSlot*)File->handle)->BytesUsedCRC  = BytesUsedCRC;

  return TRUE;
}
