#include "FBLib_tap.h"

bool HDD_TAP_SetCurrentDirCluster (dword DirCluster)
{
  tTAPTableInfo         pTAPInfo;
  byte                  DataBuffer [512];

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return FALSE;

  //Get some information about ourselve
  HDD_TAP_GetInfo (TAP_TableIndex, &pTAPInfo);

  if (*(dword*)(pTAPInfo.pDotDirStructure + 8) == DirCluster) return TRUE;

  HDD_ReadSectorDMA ((DirCluster + 1) * HDD_GetClusterSize(), 1, DataBuffer);
  if ((DataBuffer [0] != 0xf1) && (DataBuffer [0x14] != '.')) return FALSE;

  memcpy ((byte*)pTAPInfo.pDotDirStructure, DataBuffer, 128);

  return TRUE;
}
