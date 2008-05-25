#include "FBLib_fs.h"

dword FileSelectorKey (dword key, dword flags)
{
  return (((flags & FS_DIALOGWINDOW) && (key == RKEY_Prev)) ||
          ((flags & FS_DIALOGWINDOW) && (key == RKEY_Next)) ||
          key == RKEY_VolUp ||
          key == RKEY_VolDown ||
          key == RKEY_ChUp ||
          key == RKEY_ChDown ||
          key == RKEY_Ok ||
          key == RKEY_Exit ? 0 : key);
}
