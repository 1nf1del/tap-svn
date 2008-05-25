#include                "../libFireBird.h"

tBootReason BootReason (void)
{
  volatile word *reason = (word *) 0xa3fffffc;

  return (tBootReason) *reason;
}
