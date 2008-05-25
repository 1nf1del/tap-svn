#include "FBLib_tap.h"

TYPE_Started_Tap        fbl_started_tap;
TYPE_Parametered_Tap    fbl_parametered_tap;


//--------------------------------------- HDD_TAP_Start --------------------------------
//
dword HDD_TAP_Start (char *TAPFileName, bool BatchMode, void* ParameterBlock, dword *TAPID)
{
  tTAPTableInfo         TAPInfo;
  dword                 ret;
  dword                 DotDirStructureBackup [7];
  dword                 StartTAP;

  fbl_started_tap.magic = 0;
  fbl_parametered_tap.magic = 0;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return 0;

  ret = HDD_TAP_GetIDByFileName (TAPFileName);
  if (ret == 0) return 0;

  //Set the TAPID and batch flag
  fbl_started_tap.magic = MAGIC;
  fbl_started_tap.id = ret;
  fbl_started_tap.batchmode = BatchMode;
  if (TAPID != NULL) *TAPID = fbl_started_tap.id;

  //Set the batch pointer in the TAP header
  HDD_TAP_GetInfo (TAP_TableIndex, &TAPInfo);                                      //Get all details about this TAP
  *(dword*)(TAPInfo.LoadAddress + 0x24) = (dword) &fbl_started_tap;

  //Set the TAPID and parameter block
  fbl_parametered_tap.magic = MAGIC;
  fbl_parametered_tap.id = ret;
  fbl_parametered_tap.pParameterBlock = (dword)ParameterBlock;

  //Set the parameter pointer in the TAP header
  HDD_TAP_GetInfo (TAP_TableIndex, &TAPInfo);                                      //Get all details about this TAP
  *(dword*)(TAPInfo.LoadAddress + 0x28) = (dword) &fbl_parametered_tap;


  //Locate the TAPStart routine in the firmware
  if (!(StartTAP = FIS_fwTAPStart())) return 0;

  memcpy (&DotDirStructureBackup [0], (void*)pFWWD, 2 * sizeof (dword));      //As it is quite hard to make it stable, backup all pointers
  memcpy (&DotDirStructureBackup [2], (void*)pWD1 , 5 * sizeof (dword));

  *(dword*)pWD5  = TAPInfo.pDotDirStructure;                                  //Set all involved pointers to the directory context of this TAP
  *(dword*)pWD1  = TAPInfo.pDotDirStructure;
  *(dword*)pFWWD = TAPInfo.pDotDirStructure;

  ret = CallFirmware (StartTAP, (dword)&TAPFileName[0], 0, 0, 0);             //Call TAP_Load

  *(dword*)TAP_IndexAddress = TAP_TableIndex;                                 //Restore the TAPIndex value (still points to the new TAP)
  memcpy ((void*)pFWWD, &DotDirStructureBackup [0], 2 * sizeof (dword));      //Restore all DotDir pointers
  memcpy ((void*)pWD1 , &DotDirStructureBackup [2], 5 * sizeof (dword));

  //Reset the batch flag and start parameters
  memset(&fbl_started_tap, 0, sizeof(fbl_started_tap));
  memset(&fbl_parametered_tap, 0, sizeof(fbl_parametered_tap));

  return 1;
}
