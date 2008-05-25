#ifndef IMEM_H
#define IMEM_H

#include "../libFireBird.h"

#define __IMEM__ 1

#if __IMEM__

#define USED 1

typedef struct {
  dword dwSize;
} UNIT;

typedef struct {
	UNIT	*pFree;
	UNIT	*pHeap;
	dword	dwTotal;
	dword	dwFree;
} IMEM;

extern IMEM imem;
extern bool isInitialized;

UNIT* compact( UNIT *pPtr, dword nsize );

#endif

#endif
