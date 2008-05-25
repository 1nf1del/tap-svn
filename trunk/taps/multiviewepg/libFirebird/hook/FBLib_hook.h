#ifndef FBLIB_HOOK_H
#define FBLIB_HOOK_H

#include "tap.h"
#include "../libFireBird.h"

#define DefaultOPsLength 95
#define MAXHOOKS 100

typedef struct
{
 dword                  *pFWSource;
 dword                  *pHookHandler;
 dword                  *pTAPHandler;
 dword                  *pCPURegs;
 dword                  OriginalWords [2];
 tCPURegs               CPURegs;
 dword                  HookHandler [DefaultOPsLength];
 bool                   Enabled;
} tHooks;

extern bool LibInitialized;
extern tHooks Hooks [];
extern dword NrHooks;

#endif
