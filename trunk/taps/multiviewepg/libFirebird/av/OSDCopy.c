#include "../libFireBird.h"

void OSDCopy (word rgn, dword x, dword y, dword w, dword h, word items, eCopyDirection direction)
{
  word n;
  dword nw, nh;

  nw = w;
  nh = h;
  items++;

  // copy by doubling
  for (n = 1; n <= items >> 1; n <<= 1)
  {
    TAP_Osd_Copy(rgn, rgn, x, y, nw, nh, (direction == X ? x + nw : x), (direction == X ? y : y + nh), TRUE);

    if (direction == X) nw <<= 1;
    else nh <<= 1;
  }

  n = items - n;

  //copy remaining part
  if (n)
  {
    if (direction == X) w = n * w;
    else h = n * h;

    TAP_Osd_Copy(rgn, rgn, x, y, w, h, (direction == X ? x + nw : x), (direction == X ? y : y + nh), TRUE);
  }
}
