#include <string.h>
#include "FBLib_hdd.h"
#include "../libFireBird.h"


byte                    *DirBuffer;

DirEntry                DirEntryStack[DIR_MAX_STACK];
int                     StackDepth = -1;


bool PushDirEntry (dword Cluster, dword Entry, dword DirLen)
{
  if (++StackDepth < DIR_MAX_STACK)
  {
    DirEntryStack[StackDepth].Cluster = Cluster;
    DirEntryStack[StackDepth].Entry = Entry;
    DirEntryStack[StackDepth].DirLen = DirLen;
    return TRUE;
  }
  else
  {
    StackDepth--;
    return FALSE;
  }
}


bool PopDirEntry (dword *Cluster, dword *Entry, dword *DirLen)
{
  if (StackDepth >= 0)
  {
    *Cluster = DirEntryStack[StackDepth].Cluster;
    *Entry = DirEntryStack[StackDepth].Entry;
    *DirLen = DirEntryStack[StackDepth].DirLen;
    StackDepth--;
    return TRUE;
  }
  else return FALSE;
}


bool GetFileDir (char *FileName, dword DirCluster, char *Dir)
{
  dword                 SPC = HDD_GetClusterSize(), i, len;
  byte                  *Entry;

  HDD_ReadSectorDMA((DirCluster + 1) * SPC, 1024 >> 2, DirBuffer);

  // first loop: files
  for (i = 0; i < 1024; i++)
  {
    Entry = DirBuffer + (i << 7);

    if (!*Entry) break;

    if ((*Entry == ATTR_NORMAL || *Entry == ATTR_TS || *Entry == ATTR_PROGRAM) && (strcmp(Entry + 0x14, FileName) == 0)) return TRUE;
  }

  // second loop: subdirectories
  for (i = 0; i < 1024; i++)
  {
    Entry = DirBuffer + (i << 7);

    if (!*Entry) break;

    if (*Entry == ATTR_FOLDER)
    {
      len = strlen(Dir);

      if (!PushDirEntry(DirCluster, i, len)) return FALSE;
      else
      {
        if (len + 1 + strlen(Entry + 0x14) >= FBLIB_DIR_SIZE)
        {
          StackDepth--;
          return FALSE;
        }

        strcat(Dir, "/");
        strcat(Dir, Entry + 0x14);

        if (GetFileDir(FileName, *(dword *) (Entry + 8), Dir)) return TRUE;

        PopDirEntry(&DirCluster, &i, &len);
        Dir[len] = '\0';
        HDD_ReadSectorDMA((DirCluster + 1) * SPC, 1024 >> 2, DirBuffer);
      }
    }
  }

  return FALSE;
}


bool HDD_GetFileDir (char *FileName, eRootDirs Root, char *Dir)
{
  int                   i;
  byte                  *Entry;
  dword                 DirCluster = 0;
  bool                  success;

  if ((int) Root < DIR_ROOT || (int) Root > DIR_MP3) return FALSE;

  if (!(DirBuffer = TAP_MemAlloc(1024 << 7))) return FALSE;   //Room for 1024 directory entries of 128 bytes each. Should be enough for everyone ;-)

  *Dir = '\0';

  if (Root != DIR_ROOT)
  {
    // read 8 root entries
    HDD_ReadSectorDMA (HDD_GetClusterSize(), 8 >> 2, DirBuffer);

    // search start cluster
    for (i = 0; i < 8; i++)
    {
      Entry = DirBuffer + (i << 7);

      if (*Entry == ATTR_FOLDER)
      {
        if (Root == DIR_DATA_FILES)
        {
          if (strcmp(Entry + 0x14, "DataFiles") == 0)
          {
            DirCluster = *(dword *) (Entry + 8);
            strcat(Dir, "/DataFiles");
            break;
          }
        }
        else if (Root == DIR_PROGRAM_FILES)
        {
          if (strcmp(Entry + 0x14, "ProgramFiles") == 0)
          {
            DirCluster = *(dword *) (Entry + 8);
            strcat(Dir, "/ProgramFiles");
            break;
          }
        }
        else if (Root == DIR_MP3)
        {
          if (strcmp(Entry + 0x14, "MP3") == 0)
          {
            DirCluster = *(dword *) (Entry + 8);
            strcat(Dir, "/MP3");
            break;
          }
        }
      }
    }

    if (i == 8)
    {
      TAP_MemFree(DirBuffer);
      return FALSE;
    }
  }

  success = GetFileDir(FileName, DirCluster, Dir);

  TAP_MemFree(DirBuffer);
  StackDepth = -1;

  return success;
}
