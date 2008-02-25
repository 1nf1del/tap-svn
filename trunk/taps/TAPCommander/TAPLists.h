#ifndef __TAPLISTS_H
#define __TAPLISTS_H

#ifdef __cplusplus
extern "C"
{
#endif


bool InitTAPMonitor();
bool GetTAPMemoryUse(int tapIndex, dword *pTotal, dword *pBiggest, dword *pFreed, dword *pDanger);


#ifdef __cplusplus
}
#endif

#endif