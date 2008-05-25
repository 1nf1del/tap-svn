#include "../libFireBird.h"

void HDD_SetSkipFlag (TYPE_File *fp, bool SkipFlag)
{
  if (fp == NULL || fp->handle == NULL) return;

  fp->skip = ((tFileSlot *) fp->handle)->Skip = (SkipFlag ? 1 : 0);

  return;
}
