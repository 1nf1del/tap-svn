#include <string.h>
#include "../libFireBird.h"

//Return codes:     0: OK
//                 -1: not enough memory
//                 -2: buffer overrun in subdir search
//                 -3: parameter error

int HDD_TranslateDirCluster (dword DirClusterNr, char* CurrentDir)
{
  dword                 ParentCluster, Slot;
  byte                  *DiskBuffer;
  dword                 SPC = HDD_GetClusterSize();
  char                  Path [FBLIB_DIR_SIZE], *p;

  if (!CurrentDir || (DirClusterNr > 131071)) return -3;

  DiskBuffer = TAP_MemAlloc (1024 << 7);   //Room for 1024 directory entries of 128 bytes each. Should be enough for everyone ;-)
  if (DiskBuffer == NULL) return -1;

  memset (Path, 0, sizeof(Path));

  while (DirClusterNr > 0)
  {
    //Get the cluster number of the parent directory (slot 1)
    HDD_ReadSectorDMA ((DirClusterNr + 1) * SPC, 1, DiskBuffer);
    ParentCluster = *(dword *)(DiskBuffer + (1 << 7) + 8);

    //Find the directory which points the our previous current directory
    HDD_ReadSectorDMA ((ParentCluster + 1) * SPC, 1024 >> 2, DiskBuffer);
    for (Slot = 0; Slot < 1024; Slot++)
    {
      if ((DiskBuffer[Slot << 7] == ATTR_FOLDER) && (*(dword *) (DiskBuffer + (Slot << 7) + 8) == DirClusterNr))
      {
        strcat (Path, "/");
        strcat (Path, DiskBuffer + (Slot << 7) + 0x14);
        break;
      }
    }

    //Did we reach the end of the buffer without locating the subdirectory?
    if (Slot == 1024)
    {
      TAP_MemFree (DiskBuffer);
      return -2;
    }

    DirClusterNr = ParentCluster;
  }

  TAP_MemFree (DiskBuffer);

  //Now we have the path but in reverse order.
  CurrentDir [0] = 0x00;
  while (strlen (Path) != 0)
  {
    p = strrchr(Path, '/');
    if (p != NULL)
    {
      strcat (CurrentDir, p);
      *p = 0;
    }
    else Path [0] = 0x00;
  }

  return 0;
}
