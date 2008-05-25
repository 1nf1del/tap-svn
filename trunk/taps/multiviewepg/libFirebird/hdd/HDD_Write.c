#include "../libFireBird.h"

bool HDD_Write (void *data, dword length, TYPE_File *f)
{
  static byte           block[512];
  static byte           *b XDATA = block;
  bool                  success = TRUE;
  dword                 blen;

  // flush buffer
  if (length == 0)
  {
    if (b > block)
    {
      success = (TAP_Hdd_Fwrite(block, sizeof(block), 1, f) == 1);
      b = block;
    }
  }
  // can't buffer data
  else if (length > sizeof(block)) success = (TAP_Hdd_Fwrite(data, length, 1, f) == 1);
  else
  {
    blen = sizeof(block) - (b - block);

    // not enough space in buffer
    if (length > blen)
    {
      // fill rest of buffer
      memcpy(b, data, blen);
      success = (TAP_Hdd_Fwrite(block, sizeof(block), 1, f) == 1);
      (byte *) data += blen;
      length -= blen;
      b = block;
    }

    // new buffer
    if (b == block) memset(block, 0, sizeof(block));

    // enough space in buffer
    memcpy(b, data, length);
    b += length;
  }

  return success;
}
