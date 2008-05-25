#include "FBLib_main.h"
#include "../libFireBird.h"

const char            *__fblib_version__ = "__FBLIB_VERSION__ " __FBLIB_VERSION__;

dword                 BIOSAddress;                //The address of the callBIOS routine
dword                 EnqueueEventAddress;        //The address of the EnqueueEvent routine
dword                 SysID, TAPgp, FWgp, TAP_TableAddress, TAP_IndexAddress, TAP_TableIndex;
word                  ApplID;
dword                 pWD1, pWD2, pWD3, pWD4, pWD5, pFWWD, pFWWDBackup;

volatile word    *FlashMemory_Base;
volatile dword   *FlashMemory_Firmware;
volatile dword   *FlashMemory_Settings;
volatile dword   *FlashMemory_Defaults;
volatile dword   *Hardware_bff00000;

volatile tpScalerSettings   *pScalerSettingsMain;
volatile tpScalerSettings   *pScalerSettingsPIP;
volatile tpScalerSettings   *pScalerSettingsUnk1;
volatile tpScalerSettings   *pScalerSettingsUnk2;

bool                    LibInitialized = FALSE;

//--------------------------------------- InitTAPex --------------------------------
//
bool InitTAPex ()
{
  dword                 i;
  word                  SystemID = *((volatile word*)0xa3fffffa);

  if (LibInitialized) return TRUE;

  BIOSAddress             = 0;
  EnqueueEventAddress     = 0;
  TAPgp                   = 0;
  FWgp                    = 0;
  TAP_TableAddress        = 0;
  TAP_IndexAddress        = 0;
  TAP_TableIndex          = 0;
  pWD1                    = 0;
  pWD2                    = 0;
  pWD3                    = 0;
  pWD4                    = 0;
  pWD5                    = 0;
  pFWWD                   = 0;
  pFWWDBackup             = 0;

  SysID    = SystemID;
  ApplID   = _appl_version;

  __asm__ __volatile__
  (
    "sw  $28, 0(%0)\n"        //sw  $gp, 0(&TAPgp)        //Save the TAPs $gp register
    :
    : "r" (&TAPgp)
  );

  //Get the $gp register for the current firmware
  FWgp = FIS_GetGP((dword*)0x80000000);


  BIOSAddress = FIS_fwBIOS();
  EnqueueEventAddress = FIS_fwEnqueueEvent();

  TAP_TableAddress = FIS_vTAPTable();
  TAP_IndexAddress = TAP_TableAddress + 0x0200;
  TAP_TableIndex   = *(dword*)TAP_IndexAddress;

  if ((pWD1 = FIS_vWD1()))
  {
    pWD2 = pWD1 + 0x0004;
    pWD3 = pWD1 + 0x0008;
    pWD4 = pWD1 + 0x000c;
    pWD5 = pWD1 + 0x0010;
  }

  i = (dword)TAP_Hdd_Fclose;
  do
  {
    i += 4;
    if (i > 0x82000000) return FALSE;
  } while ((*(dword*)i & CMD_MASK) != JAL_CMD);
  i = ABS_ADDR(*(dword*)i);
  pFWWD = FWgp + (short)(*(dword*)i & 0xffff);
  pFWWDBackup = pFWWD + 4;


#ifdef DEBUG_FIREBIRDLIB
  TAP_Print ("FireBirdLib: SystemID=%d, Appl=%4.4x, FW$gp=%8.8x, TAP$gp=%8.8x\n", SysID, _appl_version, FWgp, TAPgp);
  TAP_Print ("FireBirdLib: BIOSAddress=%8.8x, Dispatcher=%8.8x, TAP_TableAddress=%8.8x, TableIndex=%d\n\n", BIOSAddress, EnqueueEventAddress, TAP_TableAddress, TAP_TableIndex);
#endif

  FlashMemory_Base                                  = (word  *) 0xbfc00000;
  FlashMemory_Firmware                              = (dword *) 0xbfc10000;
  FlashMemory_Settings                              = (dword *) 0xbfdc0000;
  FlashMemory_Defaults                              = (dword *) 0xbfde0000;
  Hardware_bff00000                                 = (dword *) 0xbff00000;

  pScalerSettingsMain                               = (tpScalerSettings*) 0xb0004000;
  pScalerSettingsPIP                                = (tpScalerSettings*) 0xb0004010;
  pScalerSettingsUnk1                               = (tpScalerSettings*) 0xb0004004;
  pScalerSettingsUnk2                               = (tpScalerSettings*) 0xb0004014;

  LibInitialized = TAP_TableAddress && EnqueueEventAddress && pWD1;

  if (LibInitialized) FlashInitialize (SysID);

  return LibInitialized;
}

