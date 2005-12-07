//LibTapulator - A library for the Tapulator emulator
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

typedef void (*VoidVoidFuncPtr) (void);
typedef void (*VoidWordFuncPtr) (WORD);
typedef WORD (*WordWordFuncPtr) (WORD);

class IFramework
{
public:
	virtual ~IFramework(void) {};

	virtual	void Win_SetTitle( TYPE_Window *win, const char *str, BYTE fntType, BYTE fntSize ) = 0;
	virtual void Win_SetColor(TYPE_Window * win, WORD titleBack, WORD titleText, WORD bodyBack, WORD bodyText, WORD border, WORD shadow, WORD dark, WORD light) = 0;
	virtual	void Win_SetDefaultColor( TYPE_Window *win ) = 0;
	virtual	void Win_Draw( TYPE_Window *win ) = 0;
	virtual	void Win_Create( TYPE_Window *win, WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, BYTE save, BYTE bScr ) = 0;
	virtual	DWORD Win_Delete( TYPE_Window *win ) = 0;
	virtual	void Win_SetFont( TYPE_Window *win, BYTE fntType, BYTE fntSize ) = 0;
	virtual	void Win_AddItem( TYPE_Window *win, char *str ) = 0;
	virtual	DWORD Win_GetSelection( TYPE_Window *win ) = 0;
	virtual	void Win_SetSelection( TYPE_Window *win, DWORD pos ) = 0;
	virtual	void Win_Action( TYPE_Window *win, DWORD key ) = 0;
	virtual	void Win_SetAvtice( TYPE_Window *win, DWORD itemNo, BYTE active ) = 0;
	virtual	void Win_SetDrawItemFunc( TYPE_Window *win, TYPE_DrawItemFunc func ) = 0;

	virtual void SystemProc() = 0;
	virtual DWORD GetState(DWORD *state, DWORD *subState ) = 0;
	virtual void ExitNormal(void ) = 0;
	virtual void EnterNormal(void ) = 0;
	virtual void Exit(void) = 0;

	virtual BYTE KbHit(void) = 0;
	virtual BYTE GetCh(void) = 0;
	virtual void PutCh(BYTE c) = 0;
	virtual void PutByte(BYTE c) = 0;
	virtual void PrintSys(const void *fmt, va_list args) = 0;
	virtual void Print(const void *fmt, va_list args) = 0;

	virtual void SysOsdControl(TYPE_TapSysOsdId osdId, bool ctrl ) = 0;
	virtual void SetBaudRate(DWORD baudRate ) = 0;
	virtual WORD SetBk(bool onOff ) = 0;

	// -- COMMON FUNCTION ---------------------------------------------------
	virtual int SetSoundLevel(BYTE soundLevel) = 0;
	virtual int GetTime(WORD *mjd, BYTE *hour, BYTE *min, BYTE *sec) = 0;
	virtual void MemInfo(DWORD *heapSize, DWORD *freeHeapSize, DWORD *availHeapSize) = 0;
	virtual void* MemAlloc(DWORD size) = 0;
	virtual int MemFree(const void *addr) = 0;
	virtual DWORD GetTick(void) = 0;
	virtual void Delay(DWORD dm10) = 0;
	virtual int ExtractMjd(WORD mjd, WORD *year, BYTE *month, BYTE *day, BYTE *weekDay) = 0;
	virtual int MakeMjd(WORD year, BYTE month, BYTE day) = 0;
	virtual long Sin(long mul, long th ) = 0;
	virtual long Cos(long mul, long th ) = 0;
	virtual void SPrint(void *s, const void *fmt, va_list args) = 0;
	virtual int GetSignalLevel(void) = 0;
	virtual int GetSignalQuality(void) = 0;
	virtual int SetInfoboxTime(int sec ) = 0;
	virtual int PlayPCM(void *pcmdata, int size, int freq, VoidVoidFuncPtr CallBack ) = 0;
	virtual int CaptureScreen(int mainSub, TYPE_VideoBank *videoBank ) = 0;

	virtual int SetSystemVar(int varId, int value ) = 0;
	virtual int GetSystemVar(int varId ) = 0;
	virtual int WriteSystemVar(void ) = 0;
	virtual void GenerateEvent( WORD evtCode, DWORD param1, DWORD param2 ) = 0;

	// -- OSD FUNCTION ---------------------------------------------------
	virtual int Osd_Create(DWORD x, DWORD y, DWORD w, DWORD h, BYTE lutIdx, int flag ) = 0;
	virtual int Osd_Delete(WORD rgn) = 0;
	virtual int Osd_Move(WORD rgn, DWORD x, DWORD y) = 0;
	virtual int Osd_FillBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, DWORD color) = 0;
	virtual int Osd_PutBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, const void *data, bool sprite, BYTE dataFormat ) = 0;
	virtual int Osd_GetBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, void *data) = 0;
	virtual int Osd_SetLut(BYTE lutIdx, BYTE lut[][4] ) = 0;
	virtual int Osd_Ctrl(WORD rgn, DWORD ctrl) = 0;
	virtual int Osd_SetTransparency(WORD rgn, char rt) = 0;

	virtual int Osd_Draw3dBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, DWORD c1, DWORD c2 ) = 0;
	virtual int Osd_Draw3dBoxFill(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, DWORD c1, DWORD c2, DWORD fc ) = 0;

	virtual int Osd_DrawRectangle(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, DWORD t, DWORD color) = 0;
	virtual int Osd_DrawPixmap(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, void *pixmap, bool sprite, BYTE dataFormat ) = 0;
	virtual int Osd_Zoom(int xzoom, int yzoom ) = 0;
	virtual int Osd_GetBaseInfo(TYPE_OsdBaseInfo *info ) = 0;
	virtual int Osd_Copy(WORD srcRgnNum, WORD dstRgnNum, DWORD srcX, DWORD srcY, DWORD w, DWORD h, DWORD dstX, DWORD dstY,  bool sprite) = 0;
	//  int	(*Osd_PutGd__(WORD rgn, int x, int y, TYPE_GrData * gd, bool sprite);
	virtual int	Osd_PutGd(WORD rgn, int x, int y, TYPE_GrData * gd, bool sprite) = 0;

	virtual BYTE* Osd_SaveBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h ) = 0;
	virtual void Osd_RestoreBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, void *data ) = 0;

	virtual int Osd_GetPixel(WORD rgn, DWORD x, DWORD y, void *pix ) = 0;
	virtual int Osd_PutPixel(WORD rgn, DWORD x, DWORD y, DWORD pix ) = 0;

	// -- HDD FUNCTION ---------------------------------------------------

	virtual TYPE_File* Hdd_Fopen(char *name ) = 0;
	virtual DWORD Hdd_Fread(void *buf, DWORD blksize, DWORD blk, TYPE_File *file ) = 0;
	virtual DWORD Hdd_Fwrite(void *buf, DWORD blksize, DWORD blk, TYPE_File *file ) = 0;
	virtual void Hdd_Fclose(TYPE_File *file ) = 0;
	virtual DWORD Hdd_FindFirst(TYPE_File *file ) = 0;
	virtual DWORD Hdd_FindNext(TYPE_File *file ) = 0;
	virtual DWORD Hdd_Fseek(TYPE_File *file, long pos, long where ) = 0;
	virtual DWORD Hdd_Flen(TYPE_File *file ) = 0;
	virtual DWORD Hdd_Ftell(TYPE_File *file ) = 0;
	virtual DWORD Hdd_TotalSize(void ) = 0;
	virtual DWORD Hdd_FreeSize(void ) = 0;
	virtual WORD Hdd_Delete(char *name ) = 0;
	virtual bool Hdd_Exist(char *name ) = 0;
	virtual DWORD Hdd_Create(char *name, BYTE attr ) = 0;
	virtual WORD Hdd_ChangeDir(char *dir ) = 0;

	virtual int Hdd_PlayTs(char *name ) = 0;
	virtual int Hdd_StopTs(void) = 0;
	virtual int Hdd_PlayMp3(char *name ) = 0;
	virtual int Hdd_StopMp3(void ) = 0;
	virtual bool Hdd_Rename(  	char *oldName, char *newName ) = 0;

	virtual bool Hdd_StartRecord(void ) = 0;
	virtual bool Hdd_StopRecord(BYTE recSlot ) = 0;
	virtual bool Hdd_GetRecInfo(BYTE recSlot, TYPE_RecInfo *recInfo ) = 0;
	virtual bool Hdd_GetPlayInfo(TYPE_PlayInfo *playInfo ) = 0;
	virtual bool Hdd_GetHddID(char *modelNo, char *serialNo) = 0;

	virtual bool Hdd_ChangePlaybackPos(DWORD blockPos ) = 0;
	virtual bool Hdd_GotoBookmark(void ) = 0;
	virtual bool Hdd_SetBookmark(void ) = 0;

	// -- STRING FUNCTION ---------------------
	virtual int Osd_PutS(WORD rgn, DWORD x, DWORD y, DWORD maxX, const char * str, WORD fcolor, WORD bcolor, BYTE fntType, BYTE fntSize, BYTE bDot, BYTE align) = 0;
	virtual int Osd_GetW(const char *str, BYTE fntType, BYTE fntSize ) = 0;
	virtual int Osd_PutString(WORD rgn, DWORD x, DWORD y, DWORD maxX, const char * str, WORD fcolor, WORD bcolor, BYTE fntType, BYTE fntSize, BYTE nextLine) = 0;
	virtual int Osd_DrawString(const char * str, DWORD dstWidth, WORD color, BYTE * dest, DWORD maxWidth, BYTE fntType, BYTE fntSize) = 0;
	virtual int Osd_PutStringAf(WORD rgn, DWORD x, DWORD y, DWORD maxX, const char * str, WORD fcolor, WORD bcolor, BYTE fntType, BYTE fntSize, BYTE nextLine) = 0;

	virtual int Channel_GetTotalNum(int *nTvSvc, int *nRadioSvc ) = 0;
	virtual int Channel_GetFirstInfo(int svcType, TYPE_TapChInfo *chInfo ) = 0;
	virtual int Channel_GetNextInfo(TYPE_TapChInfo *chInfo ) = 0;
	virtual int Channel_GetInfo(int svcType, int svcNum, TYPE_TapChInfo *chInfo ) = 0;
	virtual int Channel_GetCurrent(int *tvRadio, int *svcNum ) = 0;
	virtual int Channel_GetCurrentSubTV(int *svcNum ) = 0;
	virtual int Channel_Start(int mainSub, int svcType, int chNum ) = 0;
	virtual int Channel_Scale(int mainSub, long x, long y, long w, long h, bool anim ) = 0;
	virtual int Channel_IsPlayable(int mainSub, int svcType, int chNum ) = 0;
	virtual int Channel_Move(int mainSub, int dir ) = 0;
	virtual int Channel_Stop(int mainSub ) = 0;
	virtual int Channel_GetTotalAudioTrack(void ) = 0;
	virtual char* Channel_GetAudioTrackName(int trkNum, char *trkName ) = 0;
	virtual int Channel_SetAudioTrack(int trkNum ) = 0;
	virtual int Channel_GetTotalSubtitleTrack(void ) = 0;
	virtual char* Channel_GetSubtitleTrackName(int trkNum, char *trkName ) = 0;
	virtual int Channel_SetSubtitleTrack(int trkNum ) = 0;
	virtual bool Channel_IsStarted(int mainSub ) = 0;

	// --- EPG FUNCTIONS ---------------------------------------------------

	virtual TYPE_TapEvent* GetEvent(int svcType, int svcNum, int *eventNum ) = 0;
	virtual TYPE_TapEvent* GetCurrentEvent(int svcType, int svcNum ) = 0;
	virtual bool PutEvent(BYTE *eitData ) = 0;

	virtual WORD ControlEit(bool enable ) = 0;

	virtual bool EPG_UpdateEvent( int svcType, int svcNum, DWORD evtid, DWORD runningStatus ) = 0;
	virtual bool EPG_DeleteEvent( int svcType, int svcNum, DWORD evtid ) = 0;
	virtual BYTE *EPG_GetExtInfo(TYPE_TapEvent *tapEvtInfo ) = 0;

	// --- USB FUNCTIONS ---------------------------------------------------

	virtual void Usb_Read(BYTE *pBuff, WORD size, VoidWordFuncPtr pFunc ) = 0;
	virtual void Usb_Write(BYTE *pBuff, WORD size, VoidWordFuncPtr pFunc ) = 0;
	virtual void Usb_PacketRead(BYTE *pBuff, WORD size, WordWordFuncPtr pFunc ) = 0;
	virtual void Usb_PacketWrite(BYTE *pBuff, WORD size, VoidWordFuncPtr pFunc , DWORD cmd ) = 0;
	virtual void Usb_Cancel(void) = 0;

	// --- TIMER FUNCTIONS ---------------------------------------------------

	virtual int Timer_GetTotalNum(void ) = 0;
	virtual bool Timer_GetInfo(int entryNum, TYPE_TimerInfo *info ) = 0;
	virtual int Timer_Add(TYPE_TimerInfo *info ) = 0;
	virtual int Timer_Modify(int entryNum, TYPE_TimerInfo *info ) = 0;
	virtual bool Timer_Delete(int entryNum ) = 0;

	// --- VFD FUNCTIONS ---------------------------------------------------

	virtual int Vfd_GetStatus(void ) = 0;
	virtual void Vfd_Control(bool underTapCtrl ) = 0;
	virtual void Vfd_SendData(BYTE *data, BYTE dataLen ) = 0;

};
