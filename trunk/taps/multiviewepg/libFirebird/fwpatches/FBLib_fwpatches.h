#ifndef FBLIB_FWPATCHES_H
#define FBLIB_FWPATCHES_H

#include "tap.h"

#define PATCHLISTSTART  0x000e5000
#define PATCHLISTSIZE   100

extern word           PatchList [PATCHLISTSIZE];
extern dword          PatchListSize;

dword *PatchFindNextListEntry (dword *p);

#endif
