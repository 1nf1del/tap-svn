#include                "FBLib_av.h"
#include                "../libFireBird.h"

// adapted from bdb's exTAP library

int MHEG_Status (void)
{
  static dword          *fw_mheg_entry = NULL;
  dword                 v0, v1;
  dword                 register_group;

  if (!(fw_mheg_entry = (dword*)FIS_fwMHEGStatus())) return -1;

  register_group = 0;
  v0 = *fw_mheg_entry;

  if (v0)
  {
    v1 = *(dword *) (v0 + 0x8);

    if (v1) register_group = *(dword *) (v1 + 0x34);
  }

  //see  dtt mheg-5 spec. v1.06, section 3.6
  //if register_group == 0 then no mheg is running
  //if register_group == 3 then mheg is running, has claimed <text><red><green><yellow><blue>
  //if register_group == 5 then mheg is running, has claimed group3 + <arrow keys>
  //if register_group == 4 then mheg is running, has claimed group3 + group5 + <number keys>
  return (int) register_group;
}
