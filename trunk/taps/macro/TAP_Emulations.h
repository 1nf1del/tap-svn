#ifndef TAP_Emulations_H
#define TAP_Emulations_H 1

#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else 
# define DLLIMPORT __declspec (dllimport)
#endif

#include <windows.h>

#include "tap.h"
#define true 1 == 1
#define false 1 == 0

#ifndef __cplusplus
#ifndef bool
typedef unsigned char bool;
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif

// TAP SDK specialities
typedef int (*TAP_MainFunc)();
typedef dword (*TAP_EventHandlerFunc)( word event, dword param1, dword param2 );

DLLIMPORT int InitializeTAP_Environment(HINSTANCE hInstance, int nCmdShow, const char *ApplicationName, TAP_MainFunc TAP_Main, TAP_EventHandlerFunc TAP_EventHandler);
DLLIMPORT void enableFramebufferMonitor(BOOL doEnable);
DLLIMPORT void enableFramebufferUpdate(BOOL doEnable);

DLLIMPORT void setTAP_ProgramName(const char *ProgramName);
DLLIMPORT void setTAP_AuthorName(const char *AuthorName);
DLLIMPORT void setTAP_Description(const char *Description);
DLLIMPORT void setTAP_Etc(const char *Etc);
DLLIMPORT void setTAP_ID(dword ID);

DLLIMPORT void setTAP_Information(const char *ProgramName, const char *AuthorName, const char *Description, const char *Etc, dword ID);

DLLIMPORT void TAP_Print(char *str, ...);
DLLIMPORT void *TAP_MemAlloc(dword size);
DLLIMPORT byte TAP_MemFree(void *addr);
DLLIMPORT void TAP_SystemProc();
DLLIMPORT void TAP_MemInfo(dword *heapSize, dword *freeHeapSize, dword *availHeapSize);

DLLIMPORT void TAP_GetState(dword *state, dword *subState);
DLLIMPORT int TAP_Osd_GetBaseInfo( TYPE_OsdBaseInfo *info );
DLLIMPORT void TAP_SPrint(char *str, const char *fmt, ...);
DLLIMPORT void TAP_Delay(dword dm10);
DLLIMPORT dword TAP_GetTick();

DLLIMPORT word TAP_GetTime(word *mjd, byte *hour, byte *min, byte *sec);
DLLIMPORT byte TAP_ExtractMjd(word mjd, word *year, byte *month, byte *day, byte *weekDay);
DLLIMPORT word TAP_MakeMjd(word year, byte month, byte day);

DLLIMPORT long TAP_Sin(long mul, long th);
DLLIMPORT long TAP_Cos(long mul, long th);

DLLIMPORT int TAP_CaptureScreen(int mainSub, TYPE_VideoBank *videoBank);
//DLLIMPORT int TAP_PlayPCM(void *pcmdata, int size, int freq, void (*Callback)(void));
DLLIMPORT int TAP_SetInfoboxTime(int sec);
DLLIMPORT void TAP_SetBaudRate(dword baudRate); // this is stubbed only

//Vfd functions
DLLIMPORT int TAP_Vfd_GetStatus();
DLLIMPORT void TAP_Vfd_Control(bool underTapCtrl);
DLLIMPORT void TAP_Vfd_SendData(byte *data, byte dataLen);

//OSD Regions
DLLIMPORT word TAP_Osd_Create(dword x, dword y, dword w, dword h, byte ludidx, int flag);
DLLIMPORT word TAP_Osd_Delete(word rgn);
DLLIMPORT word TAP_Osd_Move(word rgn, dword x, dword y);
DLLIMPORT word TAP_Osd_FillBox(word rgn, dword x, dword y, dword w, dword h, dword color);
DLLIMPORT word TAP_Osd_SetLut(byte lutldx, byte lut[][4]);
DLLIMPORT word TAP_Osd_Copy(word srcRgnNum, word dstRgnNum, dword srcX, dword srcY, dword w, dword h, dword dstX, dword dstY, bool Sprite);
DLLIMPORT byte *TAP_Osd_SaveBox(word rgn, dword x, dword y, dword h, dword w);
DLLIMPORT void TAP_Osd_RestoreBox(word rgn, dword x, dword y, dword w, dword h, byte *data);
DLLIMPORT int TAP_Osd_GetBox(word rgn, dword x, dword y, dword w, dword h, void *data);
DLLIMPORT int TAP_Osd_PutBox(word rgn, dword x, dword y, dword w, dword h, const void *data, bool sprite, byte dataFormat);
DLLIMPORT int TAP_Osd_Zoom(int xzoom, int yzoom);
DLLIMPORT word TAP_Osd_GetPixel(word rgn, dword x, dword y, dword *pix);
DLLIMPORT word TAP_Osd_PutPixel(word rgn, dword x, dword y, dword pix);

DLLIMPORT dword TAP_Osd_PutS(word rgn, dword x, dword y, dword MaxX, const char *str, dword fcolor, dword bcolor, byte fntType, byte fntSize, byte bDot, byte align);
DLLIMPORT dword TAP_Osd_GetW(const char *str, byte fntType, byte fntSize);
DLLIMPORT void TAP_Osd_PutString(word rgn, dword x, dword y, dword maxX, const char * str, dword fcolor, dword bcolor, byte fntType, TYPE_FontSize fntSize, byte nextLine);
DLLIMPORT void TAP_Osd_DrawString(const char * str, dword dstWidth, dword color, byte * dest, dword maxWidth, byte fntType, TYPE_FontSize fntSize);
DLLIMPORT void TAP_Osd_PutStringAf(word rgn, dword x, dword y, dword MaxX, const char *str, dword fcolor, dword bcolor, byte fntType, byte fntSize, byte nextline); //, byte lut[4]);

DLLIMPORT int TAP_Osd_PutGd(word rgn, int x, int y, TYPE_GrData *gd, bool sprite);
DLLIMPORT word TAP_Osd_SetTransparency(word rgn, char rt);
DLLIMPORT word TAP_Osd_DrawRectangle(word rgn, dword x1, dword y1, dword w, dword h, dword t, dword color);
DLLIMPORT word TAP_Osd_DrawPixmap(word rgn, dword x, dword y, dword w, dword h, void *pixmap, bool sprite, byte dataFormat);

// Disk functions
DLLIMPORT dword TAP_Hdd_TotalSize();
DLLIMPORT dword TAP_Hdd_FreeSize();
DLLIMPORT TYPE_File *TAP_Hdd_Fopen(char *name);
DLLIMPORT dword TAP_Hdd_Fread(void *buf, dword blksize, dword blk, TYPE_File *fp);
DLLIMPORT dword TAP_Hdd_Fwrite(void *buf,	dword blksize, dword blk, TYPE_File *fp);
DLLIMPORT void TAP_Hdd_Fclose(TYPE_File *fp);
DLLIMPORT dword TAP_Hdd_Flen(TYPE_File *fp);
DLLIMPORT dword TAP_Hdd_Ftell(TYPE_File *fp);
DLLIMPORT dword TAP_Hdd_Fseek(TYPE_File *fp, long pos, long where);
DLLIMPORT word TAP_Hdd_Delete(char *filename);
DLLIMPORT bool TAP_Hdd_Rename(char *oldName, char *newName);
DLLIMPORT bool TAP_Hdd_Exist(char *filename);
DLLIMPORT dword TAP_Hdd_Create(char *name, byte attr);
DLLIMPORT word TAP_Hdd_ChangeDir(char *dir);
DLLIMPORT dword TAP_Hdd_FindFirst(TYPE_File *file);
DLLIMPORT dword TAP_Hdd_FindNext(TYPE_File *file);
DLLIMPORT bool TAP_Hdd_GetRecInfo(byte recSlot, TYPE_RecInfo *recInfo);

DLLIMPORT bool TAP_Hdd_GetPlayInfo(TYPE_PlayInfo *playInfo);
DLLIMPORT int  TAP_Hdd_PlayTs(char *name);
DLLIMPORT int  TAP_Hdd_StopTs();
DLLIMPORT int  TAP_Hdd_PlayMp3(char *name);
DLLIMPORT int  TAP_Hdd_StopMp3();
DLLIMPORT bool TAP_Hdd_StartRecord();
DLLIMPORT bool TAP_Hdd_StopRecord();
DLLIMPORT bool TAP_Hdd_GetHddID(char *modelNo, char *serialNo);
DLLIMPORT bool TAP_Hdd_ChangePlaybackPos(dword blockPos);
DLLIMPORT bool TAP_Hdd_GotoBookmark();
DLLIMPORT bool TAP_Hdd_SetBookmark();

DLLIMPORT int TAP_GetSystemVar(int varId);
DLLIMPORT int TAP_WriteSystemVar();
DLLIMPORT int TAP_SetSystemVar(int varId, int value);

DLLIMPORT int TAP_GetSignalLevel();
DLLIMPORT int TAP_GetSignalQuality();

DLLIMPORT void TAP_EnterNormal();
DLLIMPORT void TAP_ExitNormal();
DLLIMPORT void TAP_Exit();

DLLIMPORT void TAP_SysOsdControl(TYPE_TapSysOsdId osdid, bool ctrl);

//Event funcs
DLLIMPORT TYPE_TapEvent *TAP_GetEvent(int chType, int chNum, int *eventNum);
DLLIMPORT TYPE_TapEvent *TAP_GetCurrentEvent(int chType, int chNum);
DLLIMPORT word TAP_ControlEit(bool enable);
DLLIMPORT bool TAP_EPG_PutEvent( byte *eitData );
DLLIMPORT bool TAP_EPG_UpdateEvent(int chType, int chNum, dword evtId, dword runningStatus); 
DLLIMPORT bool TAP_EPG_DeleteEvent(int chType, int chNum, dword evtId);
DLLIMPORT byte *TAP_EPG_GetExtInfo( TYPE_TapEvent *tapEvtInfo );

// Channel funcs
DLLIMPORT int TAP_Channel_GetTotalNum(int *nTvSvc, int *nRadioSvc);
DLLIMPORT int TAP_Channel_Scale(int mainSub, long x, long y, long w, long h, bool anim);
DLLIMPORT int TAP_Channel_GetCurrent(int *tvRadio, int *chNum);
DLLIMPORT int TAP_Channel_Start(int mainSub, int chType, int chNum);
DLLIMPORT int TAP_Channel_Stop(int mainSub);
DLLIMPORT int TAP_Channel_GetInfo(int chType, int chNum,  TYPE_TapChInfo *chInfo);
DLLIMPORT int TAP_Channel_IsPlayable(int mainSub, int chType, int chNum);
DLLIMPORT int TAP_Channel_Move(int mainSub, int dir);
DLLIMPORT int TAP_Channel_GetAudioTrackName(int trkNum, char *trkName);
DLLIMPORT int TAP_Channel_SetAudioTrack(int trkNum);
DLLIMPORT int TAP_Channel_GetTotalSubtitleTrack();
DLLIMPORT int TAP_Channel_SetSubtitleTrack(int trkNum);
DLLIMPORT int TAP_Channel_IsStarted(int mainSub);
DLLIMPORT int TAP_Channel_GetCurrentSubTV(int  *svcNum);
DLLIMPORT int TAP_Channel_GetFirstInfo(int svcType, TYPE_TapChInfo *chInfo);
DLLIMPORT int TAP_Channel_GetNextInfo(TYPE_TapChInfo *chInfo);

//DLLIMPORT void DrawLine( word rgn, int Ax, int Ay, int Bx, int By, COLORREF Color );

// Timer funcs
DLLIMPORT int  TAP_Timer_GetTotalNum();
DLLIMPORT bool TAP_Timer_GetInfo(int entryNum, TYPE_TimerInfo *Info);
DLLIMPORT int  TAP_Timer_Add(TYPE_TimerInfo *Info);
DLLIMPORT int  TAP_Timer_Modify(int entryNum, TYPE_TimerInfo *Info);
DLLIMPORT bool TAP_Timer_Delete(int entryNum);

// Window funcs
typedef void (*TYPE_DrawItemFunc)( TYPE_Window *win, dword itemNo );
DLLIMPORT void	TAP_Win_SetTitle( TYPE_Window *win, const char *str, byte fntType, byte fntSize );
DLLIMPORT void	TAP_Win_SetColor(TYPE_Window * win, word titleBack, word titleText, word bodyBack, word bodyText, word border, word shadow, word dark, word light);
DLLIMPORT void	TAP_Win_SetDefaultColor( TYPE_Window *win );
DLLIMPORT void	TAP_Win_Draw( TYPE_Window *win );
DLLIMPORT void	TAP_Win_Create( TYPE_Window *win, word rgn, dword x, dword y, dword w, dword h, byte save, byte bScr );
DLLIMPORT dword	TAP_Win_Delete( TYPE_Window *win );
DLLIMPORT void	TAP_Win_SetFont( TYPE_Window *win, byte fntType, byte fntSize );
DLLIMPORT void	TAP_Win_AddItem( TYPE_Window *win, char *str );
DLLIMPORT dword	TAP_Win_GetSelection( TYPE_Window *win );
DLLIMPORT void	TAP_Win_SetSelection(TYPE_Window *win, dword pos );
DLLIMPORT void	TAP_Win_Action( TYPE_Window *win, dword key );
DLLIMPORT void TAP_Win_SetAvtice( TYPE_Window *win, dword itemNo, byte active );         
DLLIMPORT void TAP_Win_SetDrawItemFunc( TYPE_Window *win, TYPE_DrawItemFunc func );

//USB funcs
DLLIMPORT void TAP_Usb_Read(byte *pBuff, word size, void (*pFunc)(word size));
DLLIMPORT void TAP_Usb_Write(byte *pBuff, word size, void (*pFunc)(word size));
DLLIMPORT void TAP_Usb_PacketRead(byte *pBuff, word size, void(*pFunc)(word size));
DLLIMPORT void TAP_Usb_PacketWrite(byte *pBuff, word size, void (*pFunc)(word size));
DLLIMPORT void TAP_Usb_Cancel();

// RS232 Funcs

byte TAP_KbHit(void);
byte TAP_GetCh(void);
void TAP_PutCh(byte c);
void TAP_PutByte(byte c);
void TAP_PrintSys(const void *fmt, ...);
//void TAP_SysOsdControl( TYPE_TapSysOsdId osdId, bool ctrl );
//void TAP_SetBaudRate( dword baudRate );
word TAP_SetBk( bool onOff );

#ifdef __cplusplus
};
#endif
#endif
