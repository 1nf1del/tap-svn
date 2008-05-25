#include      <string.h>
#include      "FBLib_av.h"
#include      "../libFireBird.h"

word GetAudioTrackPID (byte Channel, char *Lang)
{
  if (Channel > 63) return 0;

  if (pAudioTracks == NULL) InitAudioPointers();

  if (Lang != NULL)
  {
    strncpy (Lang, pAudioTracks->Lang [Channel], 3);
    Lang [3] = 0;
  }

  return pAudioTracks->AudioPID [Channel];
}
