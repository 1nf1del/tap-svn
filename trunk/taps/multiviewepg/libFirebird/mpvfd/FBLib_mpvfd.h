#ifndef FBLIB_MPVFD_H
#define FBLIB_MPVFD_H

#include "../libFireBird.h"

#define DBG_PRINT(a...)

typedef struct
{
  // buffer for the TAP display data
  byte tapData[48];
  byte fwData[48];
  const byte *shortStringMask;
  const byte *longStringMask;
  const byte *bothStringsMask;
  const byte *fullStringsMask;
  DISPLAY_MODE displayMode;
} tDisplayMemory;

typedef struct
{
  byte                  unused1;    //Byte 00

  byte                  unused2:2;    //Byte 01
  byte                  Power:1;
  byte                  Remote:1;
  byte                  ShortDig1_d:1;
  byte                  ShortDig1_e:1;
  byte                  ShortDig1_c:1;
  byte                  ShortDig1_l:1;

  byte                  ShortDig1_m:1;   //Byte 02
  byte                  ShortDig1_k:1;
  byte                  ShortDig1_g1:1;
  byte                  ShortDig1_g2:1;
  byte                  ShortDig1_b:1;
  byte                  ShortDig1_f:1;
  byte                  ShortDig1_j:1;
  byte                  ShortDig1_h:1;

  byte                  ShortDig1_i:1;   //Byte 03
  byte                  ShortDig1_a:1;
  byte                  ShortDig2_l:1;
  byte                  ShortDig2_e:1;
  byte                  ShortDig2_c:1;
  byte                  ShortDig2_d:1;
  byte                  ShortDig2_m:1;
  byte                  ShortDig2_k:1;

  byte                  ShortDig2_g1:1;   //Byte 04
  byte                  ShortDig2_g2:1;
  byte                  ShortDig2_b:1;
  byte                  ShortDig2_f:1;
  byte                  ShortDig2_j:1;
  byte                  ShortDig2_h:1;
  byte                  ShortDig2_i:1;
  byte                  ShortDig2_a:1;

  byte                  Colon:1;    //Byte 05
  byte                  ShortDig3_d:1;
  byte                  ShortDig3_e:1;
  byte                  ShortDig3_c:1;
  byte                  ShortDig3_l:1;
  byte                  ShortDig3_m:1;
  byte                  ShortDig3_k:1;
  byte                  ShortDig3_g1:1;

  byte                  ShortDig3_g2:1;   //Byte 06
  byte                  ShortDig3_b:1;
  byte                  ShortDig3_f:1;
  byte                  ShortDig3_j:1;
  byte                  ShortDig3_h:1;
  byte                  ShortDig3_i:1;
  byte                  ShortDig3_a:1;
  byte                  PostMeridiem:1;

  byte                  AnteMeridiem:1;   //Byte 07
  byte                  Timer:1;
  byte                  ShortDig4_d:1;
  byte                  ShortDig4_e:1;
  byte                  ShortDig4_c:1;
  byte                  ShortDig4_l:1;
  byte                  ShortDig4_m:1;
  byte                  ShortDig4_k:1;

  byte                  ShortDig4_g1:1;   //Byte 08
  byte                  ShortDig4_g2:1;
  byte                  ShortDig4_b:1;
  byte                  ShortDig4_f:1;
  byte                  ShortDig4_j:1;
  byte                  ShortDig4_h:1;
  byte                  ShortDig4_i:1;
  byte                  ShortDig4_a:1;

  byte                  unused3;    //Byte 09
  byte                  unused4;    //Byte 10
  byte                  unused5;    //Byte 11
  byte                  unused6;    //Byte 12
  byte                  unused7;    //Byte 13
  byte                  unused8;    //Byte 14
  byte                  unused9;    //Byte 15
  byte                  unused10;    //Byte 16

  byte                  unused11:2;    //Byte 17
  byte                  LongDig2_d1:1;
  byte                  LongDig3_d1:1;
  byte                  LongDig2_d2:1;
  byte                  LongDig3_d2:1;
  byte                  LongDig2_e:1;
  byte                  LongDig3_e:1;

  byte                  LongDig2_c:1;   //Byte 18
  byte                  LongDig3_c:1;
  byte                  LongDig2_l:1;
  byte                  LongDig3_l:1;
  byte                  LongDig2_m:1;
  byte                  LongDig3_m:1;
  byte                  LongDig2_k:1;
  byte                  LongDig3_k:1;

  byte                  LongDig2_g1:1;   //Byte 19
  byte                  LongDig3_g1:1;
  byte                  LongDig2_g2:1;
  byte                  LongDig3_g2:1;
  byte                  LongDig2_g3:1;
  byte                  LongDig3_g3:1;
  byte                  LongDig2_b:1;
  byte                  LongDig3_b:1;

  byte                  LongDig2_f:1;   //Byte 20
  byte                  LongDig3_f:1;
  byte                  LongDig2_j:1;
  byte                  LongDig3_j:1;
  byte                  LongDig2_h:1;
  byte                  LongDig3_h:1;
  byte                  LongDig2_i:1;
  byte                  LongDig3_i:1;

  byte                  LongDig2_a1:1;   //Byte 21
  byte                  LongDig3_a1:1;
  byte                  LongDig2_a2:1;
  byte                  LongDig3_a2:1;
  byte                  LongDig6_d1:1;
  byte                  LongDig7_d1:1;
  byte                  LongDig6_d2:1;
  byte                  LongDig7_d2:1;

  byte                  LongDig6_e:1;   //Byte 22
  byte                  LongDig7_e:1;
  byte                  LongDig6_c:1;
  byte                  LongDig7_c:1;
  byte                  LongDig6_l:1;
  byte                  LongDig7_l:1;
  byte                  LongDig6_m:1;
  byte                  LongDig7_m:1;

  byte                  LongDig6_k:1;   //Byte 23
  byte                  LongDig7_k:1;
  byte                  LongDig6_g1:1;
  byte                  LongDig7_g1:1;
  byte                  LongDig6_g2:1;
  byte                  LongDig7_g2:1;
  byte                  LongDig6_g3:1;
  byte                  LongDig7_g3:1;

  byte                  LongDig6_b:1;   //Byte 24
  byte                  LongDig7_b:1;
  byte                  LongDig6_f:1;
  byte                  LongDig7_f:1;
  byte                  LongDig6_j:1;
  byte                  LongDig7_j:1;
  byte                  LongDig6_h:1;
  byte                  LongDig7_h:1;

  byte                  LongDig6_i:1;   //Byte 25
  byte                  LongDig7_i:1;
  byte                  LongDig6_a1:1;
  byte                  LongDig7_a1:1;
  byte                  LongDig6_a2:1;
  byte                  LongDig7_a2:1;
  byte                  CD_Center:1;
  byte                  CD_12:1;

  byte                  CD_01:1;    //Byte 26
  byte                  CD_02:1;
  byte                  CD_03:1;
  byte                  CD_04:1;
  byte                  CD_05:1;
  byte                  CD_06:1;
  byte                  CD_07:1;
  byte                  CD_08:1;

  byte                  CD_09:1;    //Byte 27
  byte                  CD_10:1;
  byte                  CD_11:1;
  byte                  Sat:1;
  byte                  Radio:1;
  byte                  TV:1;
  byte                  TimeShift:1;
  byte                  AC3:1;

  byte                  MP3:1;     //Byte 28
  byte                  HDDFull:1;
  byte                  HDDFull_Frame:1;
  byte                  HDDFull_8:1;
  byte                  HDDFull_7:1;
  byte                  HDDFull_6:1;
  byte                  HDDFull_5:1;
  byte                  HDDFull_4:1;

  byte                  HDDFull_3:1;   //Byte 29
  byte                  HDDFull_2:1;
  byte                  HDDFull_1:1;
  byte                  HDDSymbol:1;
  byte                  unused12:4;

  byte                  unused13;    //Byte 30
  byte                  unused14;    //Byte 31
  byte                  unused15;    //Byte 32

  byte                  unused16:2;    //Byte 33
  byte                  LongDig1_d1:1;
  byte                  LongDig8_d1:1;
  byte                  LongDig1_d2:1;
  byte                  LongDig8_d2:1;
  byte                  LongDig1_e:1;
  byte                  LongDig8_e:1;

  byte                  LongDig1_c:1;   //Byte 34
  byte                  LongDig8_c:1;
  byte                  LongDig1_l:1;
  byte                  LongDig8_l:1;
  byte                  LongDig1_m:1;
  byte                  LongDig8_m:1;
  byte                  LongDig1_k:1;
  byte                  LongDig8_k:1;

  byte                  LongDig1_g1:1;       //Byte 35
  byte                  LongDig8_g1:1;
  byte                  LongDig1_g2:1;
  byte                  LongDig8_g2:1;
  byte                  LongDig1_g3:1;
  byte                  LongDig8_g3:1;
  byte                  LongDig1_b:1;
  byte                  LongDig8_b:1;

  byte                  LongDig1_f:1;   //Byte 36
  byte                  LongDig8_f:1;
  byte                  LongDig1_j:1;
  byte                  LongDig8_j:1;
  byte                  LongDig1_h:1;
  byte                  LongDig8_h:1;
  byte                  LongDig1_i:1;
  byte                  LongDig8_i:1;

  byte                  LongDig1_a1:1;       //Byte 37
  byte                  LongDig8_a1:1;
  byte                  LongDig1_a2:1;
  byte                  LongDig8_a2:1;
  byte                  LongDig4_d1:1;
  byte                  LongDig5_d1:1;
  byte                  LongDig4_d2:1;
  byte                  LongDig5_d2:1;

  byte                  LongDig4_e:1;   //Byte 38
  byte                  LongDig5_e:1;
  byte                  LongDig4_c:1;
  byte                  LongDig5_c:1;
  byte                  LongDig4_l:1;
  byte                  LongDig5_l:1;
  byte                  LongDig4_m:1;
  byte                  LongDig5_m:1;

  byte                  LongDig4_k:1;   //Byte 39
  byte                  LongDig5_k:1;
  byte                  LongDig4_g1:1;
  byte                  LongDig5_g1:1;
  byte                  LongDig4_g2:1;
  byte                  LongDig5_g2:1;
  byte                  LongDig4_g3:1;
  byte                  LongDig5_g3:1;

  byte                  LongDig4_b:1;   //Byte 40
  byte                  LongDig5_b:1;
  byte                  LongDig4_f:1;
  byte                  LongDig5_f:1;
  byte                  LongDig4_j:1;
  byte                  LongDig5_j:1;
  byte                  LongDig4_h:1;
  byte                  LongDig5_h:1;

  byte                  LongDig4_i:1;   //Byte 41
  byte                  LongDig5_i:1;
  byte                  LongDig4_a1:1;
  byte                  LongDig5_a1:1;
  byte                  LongDig4_a2:1;
  byte                  LongDig5_a2:1;
  byte                  REC:1;
  byte                  Tuner1:1;

  byte                  Tuner2:1;    //Byte 42
  byte                  RWD:1;
  byte                  :1;
  byte                  Play:1;
  byte                  :1;
  byte                  FFWD:1;
  byte                  Pause:1;
  byte                  Mute:1;

  byte                  unused17:2;    //Byte 43
  byte                  AutoRewindLeftCircle:1;
  byte                  AutoRewindRightCircle:1;
  byte                  Dollar:1;
  byte                  Attention:1;
  byte                  Dolby:1;
  byte                  Computer:1;

  byte                  unused18;    //Byte 44
  byte                  unused19;    //Byte 45
  byte                  unused20;    //Byte 46
  byte                  unused21;    //Byte 47
} tSegmentMemory;

// the VFD display function directly outputs the provided data
// it takes a pointer to an array of 48 bytes
typedef void (*DisplayFunc)(char *pData);

// global display data object used for communication between the hook function and the TAP.
union VFDDISP
{
  tDisplayMemory        Memory;   //the original structure
  tSegmentMemory        Segments;  //replaces the tapData [48] with bitwise access methods
};

extern bool LibInitialized;
extern union VFDDISP VFDDisplay;
extern DisplayFunc sendToVfdDisplay;
extern const dword symbolMap17[];
extern const word symbolMap14[];
extern byte StringsMask[48];
extern const byte longStringMask[48];
extern const byte shortStringMask[48];
extern const byte bothStringsMask[48];
extern const byte fullStringsMask[48];
extern const byte segmentMap17[8][17][2];
extern const byte segmentMap14[4][14][2];
extern bool bFwHookInstalled;
extern dword *pFwSendToVfdDisplay;

void combineVfdData (unsigned char *pData, tDisplayMemory *pVFDDisplay);
void exitHook(void);
void fwHook(dword *pData);
void setSymbol14(dword position, dword symbol);
void setSymbol17(dword position, dword symbol);
dword * findSendToVfdDisplay(dword *pData, dword size);
dword* initCodeWrapper(dword *pFwFuncAddr);

#endif
