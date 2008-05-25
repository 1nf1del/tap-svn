#include "../libFireBird.h"

bool HDD_isAnyRecording (void)
{
  return HDD_isRecording(0) || HDD_isRecording(1);
}
