#include "../libFireBird.h"

bool HDD_PausePlayback (bool Pause)
{
  byte                 *PlayMode           = (byte*)FIS_vPlaySlot();
  dword                 fwSetPlaybackSpeed = FIS_fwSetPlaybackSpeed();
  byte                 *vPlaybackPaused    = (byte*)FIS_vPlaybackPaused();

  if (!PlayMode || !fwSetPlaybackSpeed || !vPlaybackPaused) return FALSE;

  //Playback or chase play only
  if (*PlayMode != 2 && *PlayMode != 3) return FALSE;

  if (Pause)
  {
    CallFirmware (fwSetPlaybackSpeed, 0, 255, 0, 0);
    *vPlaybackPaused = 1;
  }
  else
  {
    CallFirmware (fwSetPlaybackSpeed, 1000, 255, 0, 0);
    *vPlaybackPaused = 0;
  }

  return TRUE;
}
