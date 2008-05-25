#include      "FBLib_av.h"
#include      "../libFireBird.h"

tAudioTracks            *pAudioTracks = NULL;
dword                   RECShadow = 0;  //This points to a structure which also contains the REC shadow


void InitAudioPointers (void)
{
  dword                 *p, BaseReg;
  short                 Offset;

  //Where is the audio track information structure?
  //Find the first LBU opcode in TAP_Channel_GetTotalAudioTrack
  p = (dword*) TAP_Channel_GetTotalAudioTrack;
  while ((*p & CMD_MASK) != LBU_CMD)
  {
    p++;
    if (p > (dword*)0x84000000) return;
  }
  BaseReg = (*p >> 5) & 0x001f0000;  //Move the register so it has the same position as in the LUI opcode
  Offset  = *p & 0xffff;

  do
  {
    p--;
    if (p < (dword*)0x80000000) return;
  } while (((*p & CMD_MASK) != LUI_CMD) || ((*p & 0x001f0000) != BaseReg));

  pAudioTracks = (tAudioTracks*)((*p << 16) + Offset - 1);  //The -1 is needed for the dword alignment

#ifdef DEBUG_FIREBIRDLIB
  TAP_Print ("FireBirdLib: pAudioTracks: %p", pAudioTracks);
#endif

  //The other pointer we need is the REC header copy
  //Find the first JAL opcode in TAP_Hdd_StartRecord
  p = (dword*) TAP_Hdd_StartRecord;
  while ((*p & CMD_MASK) != JAL_CMD)
  {
    p++;
    if (p > (dword*)0x84000000)
    {
      pAudioTracks = NULL;
      return;
    }
  }

  //GoTo Hdd_StartRecord
  p = (dword*)(ABS_ADDR (*p));

  //Scan for the first LUI opcode
  while ((*p & CMD_MASK) != LUI_CMD)
  {
    p++;
    if (p > (dword*)0x84000000)
    {
      pAudioTracks = NULL;
      return;
    }
  }
  BaseReg   = *p & 0x001f0000;
  RECShadow = (*p & 0xffff) << 16;

  //Scan for the first ADDIU opcode with the same register
  while (((*p & CMD_MASK) != ADDIU_CMD) || ((*p & 0x001f0000) != BaseReg))
  {
    p++;
    if (p > (dword*)0x84000000)
    {
      pAudioTracks = NULL;
      return;
    }
  }

  RECShadow += (short) (*p & 0xffff);

#ifdef DEBUG_FIREBIRDLIB
  TAP_Print (", RECShadow: %p\n", RECShadow);
#endif
}
