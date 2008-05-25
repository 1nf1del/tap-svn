#ifndef FBLIB_HDD_H
#define FBLIB_HDD_H

#include "tap.h"

//HDD Functions
//               CCD DD
//               SSA AA
//               102 10
//ATA Controller H/W Address: 1011 0000 0000 0001 0010 1101 0... ..xx
//
//
//                      CS1 CS0 DA2 DA1 DA0 R/W
//Data Port (DMA)       0   0   X   X   X   RW                      0xB0012D00

//Data Reg (PIO)        0   1   0   0   0   RW                      0xB0012D20
//Error Reg             0   1   0   0   1   R (W=Feature Reg)       0xB0012D24
//Feature Reg           0   1   0   0   1   W (R=Error Reg)         0xB0012D24
//SectorCount Reg       0   1   0   1   0   RW                      0xB0012D28
//LBALow Reg            0   1   0   1   1   RW                      0xB0012D2C
//LBAMid Reg            0   1   1   0   0   RW                      0xB0012D30
//LBAHigh Reg           0   1   1   0   1   RW                      0xB0012D34
//Device Reg            0   1   1   1   0   RW                      0xB0012D38
//Status Reg            0   1   1   1   1   R (W=Command Reg)       0xB0012D3C
//Command Reg           0   1   1   1   1   W (R=Status)            0xB0012D3C
//
//Alternate Status Reg  1   0   1   1   0   R (W=Control)           0xB0012D58
//Device Control Reg    1   0   1   1   0   W (R=Alternate Status)  0xB0012D58
//Invalid               1   1   X   X   X   X                       0xB0012D60-0xB0012D7F

#define oTAP_Hdd_ReadSectorDMA  0x00
#define oTAP_Hdd_WriteSectorDMA 0x01
#define oTAP_Hdd_Move           0x13

typedef dword (*tTAP_Hdd_ReadSectorDMA)(dword SectorNr, dword NrSectors, void *Buffer);
typedef dword (*tTAP_Hdd_WriteSectorDMA)(dword SectorNr, dword NrSectors, void *Buffer);
typedef dword (*tTAP_Hdd_Move)(char *from_dir, char *to_dir, char *filename);

#define DIR_MAX_STACK 16

typedef struct
{
  dword Cluster;
  dword Entry;
  dword DirLen;
} DirEntry;

extern volatile word *HDD_pData;
extern volatile byte *HDD_pError;
extern volatile byte *HDD_pFeature;
extern volatile byte *HDD_pSecCount;
extern volatile byte *HDD_pLBALow;
extern volatile byte *HDD_pLBAMid;
extern volatile byte *HDD_pLBAHigh;
extern volatile byte *HDD_pDevice;
extern volatile byte *HDD_pStatus;
extern volatile byte *HDD_pCommand;
extern volatile byte *HDD_pAltStatus;
//extern volatile byte *HDD_pDevControl; /* = (byte*) 0xb0012d5b;   // device control register */
extern tTAP_Hdd_ReadSectorDMA TAP_Hdd_ReadSectorDMA;
extern tTAP_Hdd_WriteSectorDMA TAP_Hdd_WriteSectorDMA;

extern int _appl_version;
extern bool LibInitialized;
extern void* (*TAP_GetSystemProc)(int);

void  busyWait(void);
void  Delay(dword);
int   GetClusterPointer(int);
int   SendHDDCommand(byte, byte, byte, byte, byte, byte, byte);
void  SwapWords(word *);
void  SwapDWords(dword *);
int   ReceiveSector(word *);
bool  ChangeDirRoot(void);
dword CalcAbsSectorFromFAT(TYPE_File *, dword);
bool  PushDirEntry (dword Cluster, dword Entry, dword DirLen);
bool  PopDirEntry (dword *Cluster, dword *Entry, dword *DirLen);
bool  GetFileDir(char *, dword, char *);

#endif
