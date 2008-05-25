#include "FBLib_hook.h"
#include "../libFireBird.h"

tHooks                  Hooks [MAXHOOKS] XDATA;
dword                   NrHooks = 0;

//--------------------------------------- HookSet --------------------------------
//
int HookSet (dword *pFWSource, dword *pTAPHandler)
{
  dword                 DefaultOPs[] = {0x27BDFFE8, 0xAFB00000, 0x3C100000, 0x36100000, 0xAFB00004, 0x3C100000, 0x36100000, 0xAFB00008,
                                        0xAE010000, 0xAE020004, 0xAE030008, 0xAE04000C, 0xAE050010, 0xAE060014, 0xAE070018, 0xAE08001C,
                                        0xAE090020, 0xAE0A0024, 0xAE0B0028, 0xAE0C002C, 0xAE0D0030, 0xAE0E0034, 0xAE0F0038, 0xAE110040,
                                        0xAE120044, 0xAE130048, 0xAE14004C, 0xAE150050, 0xAE160054, 0xAE170058, 0xAE18005C, 0xAE190060,
                                        0xAE1A0064, 0xAE1B0068, 0xAE1C006C, 0x27B10018, 0xAE110070, 0xAE1E0074, 0xAE1F0078, 0x26110000,
                                        0x8FB00000, 0xAE30003C, 0x3C100000, 0x36100000, 0xAFB0000C, 0x8E110000, 0xAFB10010, 0x24110000,
                                        0xAE110000, 0x24040000, 0x8FBC0004, 0x8FA50008, 0x0C000000, 0x00000000, 0x8FB0000C, 0x8FB10010,
                                        0xAE110000, 0x8FB00008, 0x8E010000, 0x8E020004, 0x8E030008, 0x8E04000C, 0x8E050010, 0x8E060014,
                                        0x8E070018, 0x8E08001C, 0x8E090020, 0x8E0A0024, 0x8E0B0028, 0x8E0C002C, 0x8E0D0030, 0x8E0E0034,
                                        0x8E0F0038, 0x8E110040, 0x8E120044, 0x8E130048, 0x8E14004C, 0x8E150050, 0x8E160054, 0x8E170058,
                                        0x8E18005C, 0x8E190060, 0x8E1A0064, 0x8E1B0068, 0x8E1C006C, 0x8E1E0074, 0x8E1F0078, 0x8E10003C,
                                        0x27BD0018, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x08000000, 0x00000000};

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return -1;

  if (NrHooks >= MAXHOOKS) return -1;

  Hooks [NrHooks].pFWSource     = pFWSource;
  Hooks [NrHooks].pHookHandler  = &Hooks [NrHooks].HookHandler [0];
  Hooks [NrHooks].pTAPHandler   = pTAPHandler;
  Hooks [NrHooks].pCPURegs      = &Hooks [NrHooks].CPURegs.at;
  Hooks [NrHooks].Enabled       = FALSE;

#ifdef DEBUG_FIREBIRDLIB
  TAP_Print ("FireBirdLib: Setting hook #%d, \nF/W=%8.8x, HookHandler=%8.8x, TAPHandler=%8.8x, CPURegs=%8.8x\n", NrHooks, (dword) Hooks [NrHooks].pFWSource,
                             (dword) Hooks [NrHooks].pHookHandler,
                             (dword) Hooks [NrHooks].pTAPHandler,
                             (dword) Hooks [NrHooks].pCPURegs);
#endif

  //Copy the default hook handler opcodes
  memcpy (&Hooks [NrHooks].pHookHandler [0], &DefaultOPs [0], DefaultOPsLength << 2);

  //Patch the TAP's $gp into the hook routine
  Hooks [NrHooks].pHookHandler [2] = LUI_S0_CMD | (TAPgp >> 16);
  Hooks [NrHooks].pHookHandler [3] = ORI_S0_CMD | (TAPgp & 0xffff);

  //Patch the address of the CPURegs structure into the hook routine
  Hooks [NrHooks].pHookHandler [5] = LUI_S0_CMD | (((dword) Hooks [NrHooks].pCPURegs) >> 16);
  Hooks [NrHooks].pHookHandler [6] = ORI_S0_CMD | (((dword) Hooks [NrHooks].pCPURegs) & 0xffff);

  //Patch the address of the TAP table into the hook routine
  Hooks [NrHooks].pHookHandler [42] = LUI_S0_CMD | (TAP_IndexAddress >> 16);
  Hooks [NrHooks].pHookHandler [43] = ORI_S0_CMD | (TAP_IndexAddress & 0xffff);

  //Patch the TAP_TableAddress index of this TAP into the hook routine
  Hooks [NrHooks].pHookHandler [47] = ADDIU_S1_CMD | TAP_TableIndex;

  //Inform the TAP handler about the hook index
  Hooks [NrHooks].pHookHandler [49] = ADDIU_A0_CMD | NrHooks;

  //Patch the call to the C-Hook function into the hook routine
  Hooks [NrHooks].pHookHandler [52] = JAL_CMD | REL_ADDR ((dword) pTAPHandler);

  //Copy the first two commands of the hooked function into the hook routine
  Hooks [NrHooks].pHookHandler [91] = pFWSource [0];
  Hooks [NrHooks].pHookHandler [92] = pFWSource [1];

  //Patch the return jump to the hooked function into the hook routine
  Hooks [NrHooks].pHookHandler [93] = JMP_CMD | REL_ADDR ((dword) (pFWSource) + 8);

  //Save the original words
  memcpy (&Hooks [NrHooks].OriginalWords [0], &pFWSource [0], 8);

  NrHooks++;
  return NrHooks - 1;
}
