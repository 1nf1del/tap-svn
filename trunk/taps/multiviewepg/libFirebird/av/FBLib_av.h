#ifndef FBLIB_AV_H
#define FBLIB_AV_H

#include "tap.h"

extern bool LibInitialized;
extern dword fbl_rgn;

typedef struct
{
  byte                  unused1;
  byte                  nTrk;
  byte                  curTrk;
  byte                  unused2;
  word                  AudioPID [64];
  char                  Lang [64][32];
} tAudioTracks;


extern tAudioTracks     *pAudioTracks;
extern dword            RECShadow;


inline void YUV2RGB(word, word, word, byte *, byte *, byte *);
inline void YUV2RGB2(word, word, word, int *, int *, int *);
void BMP_WriteHeader(TYPE_File *, int, int);
void InitAudioPointers (void);


#endif
