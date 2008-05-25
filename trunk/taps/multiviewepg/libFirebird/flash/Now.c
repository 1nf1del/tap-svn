#include "../libFireBird.h"

dword Now (byte *Sec)
{
  word      MJD;
  byte      Hour, Min, DummySec;

  TAP_GetTime (&MJD, &Hour, &Min, (Sec != NULL) ? Sec : &DummySec);

  return DATE(MJD, Hour, Min);
}
