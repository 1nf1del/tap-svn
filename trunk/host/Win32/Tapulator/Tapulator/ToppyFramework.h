//Tapulator - An emulator for the Topfield PVR TAP API
//Copyright (C) 2005  Robin Glover
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// You can contact the author via email at robin.w.glover@gmail.com

#pragma once

#include "..\libtapulator\iframework.h"

#include "Regions.h"
#include "Windows.h"
class TapModule;
#include "InfoLogger.h"
#include "ChannelList.h"
#include "JagFileReader.h"
#include "ToppyState.h"
#include "Timers.h"
class CMainFrame;
class CConfiguration;
#include "BackingBitmap.h"

class ToppyFramework :
	public IFramework, public InfoLogger , public BackingBitmap
{
public:
	ToppyFramework(TapModule* pMod);
	~ToppyFramework(void);

	void Draw(CDC* pDC);
	void RaiseEventToFirmware(unsigned short event, unsigned long param1, unsigned long param2 );
	CMainFrame* GetMainFrame();
	CConfiguration* GetConfig();
	CString MakeRootPath(CString sPath);

	virtual	void Win_SetTitle( TYPE_Window *win, const char *str, BYTE fntType, BYTE fntSize );
	virtual void Win_SetColor(TYPE_Window * win, WORD titleBack, WORD titleText, WORD bodyBack, WORD bodyText, WORD border, WORD shadow, WORD dark, WORD light);
	virtual	void Win_SetDefaultColor( TYPE_Window *win );
	virtual	void Win_Draw( TYPE_Window *win );
	virtual	void Win_Create( TYPE_Window *win, WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, BYTE save, BYTE bScr );
	virtual	DWORD Win_Delete( TYPE_Window *win );
	virtual	void Win_SetFont( TYPE_Window *win, BYTE fntType, BYTE fntSize );
	virtual	void Win_AddItem( TYPE_Window *win, char *str );
	virtual	DWORD Win_GetSelection( TYPE_Window *win );
	virtual	void Win_SetSelection( TYPE_Window *win, DWORD pos );
	virtual	void Win_Action( TYPE_Window *win, DWORD key );
	virtual	void Win_SetAvtice( TYPE_Window *win, DWORD itemNo, BYTE active );
	virtual	void Win_SetDrawItemFunc( TYPE_Window *win, TYPE_DrawItemFunc func );

	virtual void SystemProc();
	virtual DWORD GetState(DWORD *state, DWORD *subState );
	virtual void ExitNormal(void );
	virtual void EnterNormal(void );
	virtual void Exit(void);

	virtual BYTE KbHit(void);
	virtual BYTE GetCh(void);
	virtual void PutCh(BYTE c);
	virtual void PutByte(BYTE c);
	virtual void PrintSys(const void *fmt, va_list args);
	virtual void Print(const void *fmt, va_list args);
	
	virtual void SysOsdControl(TYPE_TapSysOsdId osdId, bool ctrl );
	virtual void SetBaudRate(DWORD baudRate );
	virtual WORD SetBk(bool onOff );

	// -- COMMON FUNCTION ---------------------------------------------------
	virtual int SetSoundLevel(BYTE soundLevel);
	virtual int GetTime(WORD *mjd, BYTE *hour, BYTE *min, BYTE *sec);
	virtual void MemInfo(DWORD *heapSize, DWORD *freeHeapSize, DWORD *availHeapSize);
	virtual void* MemAlloc(DWORD size);
	virtual int MemFree(const void *addr);
	virtual DWORD GetTick(void);
	virtual void Delay(DWORD dm10);
	virtual int ExtractMjd(WORD mjd, WORD *year, BYTE *month, BYTE *day, BYTE *weekDay);
	virtual int MakeMjd(WORD year, BYTE month, BYTE day);
	virtual long Sin(long mul, long th );
	virtual long Cos(long mul, long th );
	virtual void SPrint(void *s, const void *fmt, va_list args);
	virtual int GetSignalLevel(void);
	virtual int GetSignalQuality(void);
	virtual int SetInfoboxTime(int sec );
	virtual int PlayPCM(void *pcmdata, int size, int freq, VoidVoidFuncPtr CallBack );
	virtual int CaptureScreen(int mainSub, TYPE_VideoBank *videoBank );

	virtual int SetSystemVar(int varId, int value );
	virtual int GetSystemVar(int varId );
	virtual int WriteSystemVar(void );
	virtual void GenerateEvent( WORD evtCode, DWORD param1, DWORD param2 );

	// -- OSD FUNCTION ---------------------------------------------------
	virtual int Osd_Create(DWORD x, DWORD y, DWORD w, DWORD h, BYTE lutIdx, int flag );
	virtual int Osd_Delete(WORD rgn);
	virtual int Osd_Move(WORD rgn, DWORD x, DWORD y);
	virtual int Osd_FillBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, DWORD color);
	virtual int Osd_PutBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, const void *data, bool sprite, BYTE dataFormat );
	virtual int Osd_GetBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, void *data);
	virtual int Osd_SetLut(BYTE lutIdx, BYTE lut[][4] );
	virtual int Osd_Ctrl(WORD rgn, DWORD ctrl);
	virtual int Osd_SetTransparency(WORD rgn, char rt);

	virtual int Osd_Draw3dBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, DWORD c1, DWORD c2 );
	virtual int Osd_Draw3dBoxFill(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, DWORD c1, DWORD c2, DWORD fc );

	virtual int Osd_DrawRectangle(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, DWORD t, DWORD color);
	virtual int Osd_DrawPixmap(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, void *pixmap, bool sprite, BYTE dataFormat );
	virtual int Osd_Zoom(int xzoom, int yzoom );
	virtual int Osd_GetBaseInfo(TYPE_OsdBaseInfo *info );
	virtual int Osd_Copy(WORD srcRgnNum, WORD dstRgnNum, DWORD srcX, DWORD srcY, DWORD w, DWORD h, DWORD dstX, DWORD dstY,  bool sprite);
	virtual int	Osd_PutGd(WORD rgn, int x, int y, TYPE_GrData * gd, bool sprite);

	virtual BYTE* Osd_SaveBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h );
	virtual void Osd_RestoreBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, void *data );

	virtual int Osd_GetPixel(WORD rgn, DWORD x, DWORD y, void *pix );
	virtual int Osd_PutPixel(WORD rgn, DWORD x, DWORD y, DWORD pix );

	// -- HDD FUNCTION ---------------------------------------------------

	virtual TYPE_File* Hdd_Fopen(char *name );
	virtual DWORD Hdd_Fread(void *buf, DWORD blksize, DWORD blk, TYPE_File *file );
	virtual DWORD Hdd_Fwrite(void *buf, DWORD blksize, DWORD blk, TYPE_File *file );
	virtual void Hdd_Fclose(TYPE_File *file );
	virtual DWORD Hdd_FindFirst(TYPE_File *file );
	virtual DWORD Hdd_FindNext(TYPE_File *file );
	virtual DWORD Hdd_Fseek(TYPE_File *file, long pos, long where );
	virtual DWORD Hdd_Flen(TYPE_File *file );
	virtual DWORD Hdd_Ftell(TYPE_File *file );
	virtual DWORD Hdd_TotalSize(void );
	virtual DWORD Hdd_FreeSize(void );
	virtual WORD Hdd_Delete(char *name );
	virtual bool Hdd_Exist(char *name );
	virtual DWORD Hdd_Create(char *name, BYTE attr );
	virtual WORD Hdd_ChangeDir(char *dir );

	virtual int Hdd_PlayTs(char *name );
	virtual int Hdd_StopTs(void);
	virtual int Hdd_PlayMp3(char *name );
	virtual int Hdd_StopMp3(void );
	virtual bool Hdd_Rename(  	char *oldName, char *newName );

	virtual bool Hdd_StartRecord(void );
	virtual bool Hdd_StopRecord(BYTE recSlot );
	virtual bool Hdd_GetRecInfo(BYTE recSlot, TYPE_RecInfo *recInfo );
	virtual bool Hdd_GetPlayInfo(TYPE_PlayInfo *playInfo );
	virtual bool Hdd_GetHddID(char *modelNo, char *serialNo);

	virtual bool Hdd_ChangePlaybackPos(DWORD blockPos );
	virtual bool Hdd_GotoBookmark(void );
	virtual bool Hdd_SetBookmark(void );

	// -- STRING FUNCTION ---------------------
	virtual int Osd_PutS(WORD rgn, DWORD x, DWORD y, DWORD maxX, const char * str, WORD fcolor, WORD bcolor, BYTE fntType, BYTE fntSize, BYTE bDot, BYTE align);
	virtual int Osd_GetW(const char *str, BYTE fntType, BYTE fntSize );
	virtual int Osd_PutString(WORD rgn, DWORD x, DWORD y, DWORD maxX, const char * str, WORD fcolor, WORD bcolor, BYTE fntType, BYTE fntSize, BYTE nextLine);
	virtual int Osd_DrawString(const char * str, DWORD dstWidth, WORD color, BYTE * dest, DWORD maxWidth, BYTE fntType, BYTE fntSize);
	virtual int Osd_PutStringAf(WORD rgn, DWORD x, DWORD y, DWORD maxX, const char * str, WORD fcolor, WORD bcolor, BYTE fntType, BYTE fntSize, BYTE nextLine);

	virtual int Channel_GetTotalNum(int *nTvSvc, int *nRadioSvc );
	virtual int Channel_GetFirstInfo(int svcType, TYPE_TapChInfo *chInfo );
	virtual int Channel_GetNextInfo(TYPE_TapChInfo *chInfo );
	virtual int Channel_GetInfo(int svcType, int svcNum, TYPE_TapChInfo *chInfo );
	virtual int Channel_GetCurrent(int *tvRadio, int *svcNum );
	virtual int Channel_GetCurrentSubTV(int *svcNum );
	virtual int Channel_Start(int mainSub, int svcType, int chNum );
	virtual int Channel_Scale(int mainSub, long x, long y, long w, long h, bool anim );
	virtual int Channel_IsPlayable(int mainSub, int svcType, int chNum );
	virtual int Channel_Move(int mainSub, int dir );
	virtual int Channel_Stop(int mainSub );
	virtual int Channel_GetTotalAudioTrack(void );
	virtual char* Channel_GetAudioTrackName(int trkNum, char *trkName );
	virtual int Channel_SetAudioTrack(int trkNum );
	virtual int Channel_GetTotalSubtitleTrack(void );
	virtual char* Channel_GetSubtitleTrackName(int trkNum, char *trkName );
	virtual int Channel_SetSubtitleTrack(int trkNum );
	virtual bool Channel_IsStarted(int mainSub );

	// --- EPG FUNCTIONS ---------------------------------------------------

	virtual TYPE_TapEvent* GetEvent(int svcType, int svcNum, int *eventNum );
	virtual TYPE_TapEvent* GetCurrentEvent(int svcType, int svcNum );
	virtual bool PutEvent(BYTE *eitData );

	virtual WORD ControlEit(bool enable );

	virtual bool EPG_UpdateEvent( int svcType, int svcNum, DWORD evtid, DWORD runningStatus );
	virtual bool EPG_DeleteEvent( int svcType, int svcNum, DWORD evtid );
	virtual BYTE* EPG_GetExtInfo(TYPE_TapEvent *tapEvtInfo );

	// --- USB FUNCTIONS ---------------------------------------------------

	virtual void Usb_Read(BYTE *pBuff, WORD size, VoidWordFuncPtr pFunc );
	virtual void Usb_Write(BYTE *pBuff, WORD size, VoidWordFuncPtr pFunc );
	virtual void Usb_PacketRead(BYTE *pBuff, WORD size, WordWordFuncPtr pFunc );
	virtual void Usb_PacketWrite(BYTE *pBuff, WORD size, VoidWordFuncPtr pFunc , DWORD cmd );
	virtual void Usb_Cancel(void);

	// --- TIMER FUNCTIONS ---------------------------------------------------

	virtual int Timer_GetTotalNum(void );
	virtual bool Timer_GetInfo(int entryNum, TYPE_TimerInfo *info );
	virtual int Timer_Add(TYPE_TimerInfo *info );
	virtual int Timer_Modify(int entryNum, TYPE_TimerInfo *info );
	virtual bool Timer_Delete(int entryNum );

	// --- VFD FUNCTIONS ---------------------------------------------------

	virtual int Vfd_GetStatus(void );
	virtual void Vfd_Control(bool underTapCtrl );
	virtual void Vfd_SendData(BYTE *data, BYTE dataLen );

	bool IsTapExited();
private:
	bool NeedsRepaint();
	CString MakePath(CString sName);
	int CountFilesInCurrentFolder();

	Regions m_OSDregions;
	Windows m_Windows;
	TapModule* m_pMod;
	CString m_sCurrentFolder;

	std::map<TYPE_File*, FILE*> m_mapOpenFiles;
	HANDLE m_hCurrentFind;
	CChannelList m_ChannelList;
	Timers m_Timers;
	JagFileReader m_EPGImpl;
	ToppyState m_TheState;
	DWORD m_iFirstTick;
	bool m_bExitFlag;
	int m_iFindCount;
};
