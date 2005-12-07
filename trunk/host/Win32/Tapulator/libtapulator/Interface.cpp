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

#include "stdafx.h"

#include "interface.h"
#include "Iframework.h"
static IFramework* spFramework = 0;

void SetFramework(IFramework* pFramework)
{
	spFramework = pFramework;
}

void impl_TAP_SystemProc()
{
	spFramework->SystemProc();
}

dword impl_TAP_GetState( dword* state, dword* subState )
{
	return spFramework->GetState(state, subState);
}


void impl_TAP_ExitNormal()
{
	spFramework->ExitNormal();
}


void impl_TAP_EnterNormal()
{
	spFramework->EnterNormal();
}


void impl_TAP_Exit()
{
	spFramework->Exit();
}


byte impl_TAP_KbHit()
{
	return spFramework->KbHit();
}


byte impl_TAP_GetCh()
{
	return spFramework->GetCh();
}


void impl_TAP_PutCh(byte c)
{
	spFramework->PutCh(c);
}


void impl_TAP_PutByte(byte c)
{
	spFramework->PutByte(c);
}


void impl_TAP_PrintSys(const void* fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	spFramework->PrintSys(fmt, argList);
	va_end(argList);
}


void impl_TAP_Print(const void* fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	spFramework->Print(fmt, argList);
	va_end(argList);
}


void impl_TAP_SysOsdControl( TYPE_TapSysOsdId osdId, bool ctrl )
{
	spFramework->SysOsdControl(osdId, ctrl );
}


void impl_TAP_SetBaudRate( dword baudRate )
{
	spFramework->SetBaudRate(baudRate );
}


word impl_TAP_SetBk( bool onOff )
{
	return spFramework->SetBk(onOff );
}


// -- COMMON FUNCTION ---------------------------------------------------
int impl_TAP_SetSoundLevel(byte soundLevel)
{
	return spFramework->SetSoundLevel(soundLevel);
}


int impl_TAP_GetTime(word* mjd, byte* hour, byte* min, byte* sec)
{
	return spFramework->GetTime(mjd ,hour,  min, sec);
}


void impl_TAP_MemInfo(dword* heapSize, dword* freeHeapSize, dword* availHeapSize)
{
	spFramework->MemInfo(heapSize, freeHeapSize, availHeapSize);
}


void*	impl_TAP_MemAlloc(dword size)
{
	return spFramework->MemAlloc(size);
}


int impl_TAP_MemFree(const void* addr)
{
	return spFramework->MemFree(addr);
}


dword impl_TAP_GetTick()
{
	return spFramework->GetTick();
}


void impl_TAP_Delay(dword dm10)
{
	spFramework->Delay(dm10);
}


int impl_TAP_ExtractMjd(word mjd, word* year, byte* month, byte* day, byte* weekDay)
{
	return spFramework->ExtractMjd(mjd,year,  month,  day, weekDay);
}


int impl_TAP_MakeMjd(word year, byte month, byte day)
{
	return spFramework->MakeMjd(year,month, day);
}


long impl_TAP_Sin( long mul, long th )
{
	return spFramework->Sin(mul, th );
}


long impl_TAP_Cos( long mul, long th )
{
	return spFramework->Cos(mul, th );
}


void impl_TAP_SPrint(void* s, const void* fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	spFramework->SPrint( s, fmt, argList);
	va_end(argList);
}


int impl_TAP_GetSignalLevel()
{
	return spFramework->GetSignalLevel();
}


int impl_TAP_GetSignalQuality()
{
	return spFramework->GetSignalQuality();
}


int impl_TAP_SetInfoboxTime( int sec )
{
	return spFramework->SetInfoboxTime(sec );
}


int impl_TAP_PlayPCM( void* pcmdata, int size, int freq, void Callback() )
{
	return spFramework->PlayPCM( pcmdata,size, freq, Callback );
}


int impl_TAP_CaptureScreen( int mainSub, TYPE_VideoBank* videoBank )
{
	return spFramework->CaptureScreen(mainSub,videoBank);
}


int impl_TAP_SetSystemVar( int varId, int value )
{
	return spFramework->SetSystemVar(varId, value );
}


int impl_TAP_GetSystemVar( int varId )
{
	return spFramework->GetSystemVar(varId );
}


int impl_TAP_WriteSystemVar()
{
	return spFramework->WriteSystemVar();
}


void impl_TAP_GenerateEvent(  word evtCode, dword param1, dword param2 )
{
	spFramework->GenerateEvent(evtCode,param1, param2 );
}


// -- OSD FUNCTION ---------------------------------------------------
int impl_TAP_Osd_Create( dword x, dword y, dword w, dword h, byte lutIdx, int flag )
{
	return spFramework->Osd_Create(x,y,w,h,lutIdx,flag);
}

int impl_TAP_Osd_Delete(word rgn)
{
	return spFramework->Osd_Delete(rgn);
}


int impl_TAP_Osd_Move(word rgn, dword x, dword y)
{
	return spFramework->Osd_Move(rgn,x, y);
}


int impl_TAP_Osd_FillBox(word rgn, dword x, dword y, dword w, dword h, dword color)
{
	return spFramework->Osd_FillBox(rgn,x, y, w, h, color);
}


int impl_TAP_Osd_PutBox(word rgn, dword x, dword y, dword w, dword h, const void* data, bool sprite, byte dataFormat )
{
	return spFramework->Osd_PutBox(rgn,x, y, w, h,  data, sprite, dataFormat );
}


int impl_TAP_Osd_GetBox(word rgn, dword x, dword y, dword w, dword h, void* data)
{
	return spFramework->Osd_GetBox(rgn,x, y, w, h,data);
}


int impl_TAP_Osd_SetLut(byte lutIdx, byte lut[][4] )
{
	return spFramework->Osd_SetLut(lutIdx, lut );
}


int impl_TAP_Osd_Ctrl(word rgn, dword ctrl)
{
	return spFramework->Osd_Ctrl(rgn, ctrl);
}


int impl_TAP_Osd_SetTransparency( word rgn, char rt)
{
	return spFramework->Osd_SetTransparency(rgn, rt);
}


int impl_TAP_Osd_Draw3dBox( word rgn, dword x, dword y, dword w, dword h, dword c1, dword c2 )
{
	return spFramework->Osd_Draw3dBox(rgn,x, y, w, h, c1, c2 );
}


int impl_TAP_Osd_Draw3dBoxFill( word rgn, dword x, dword y, dword w, dword h, dword c1, dword c2, dword fc )
{
	return spFramework->Osd_Draw3dBoxFill(rgn,x, y, w, h, c1, c2, fc );
}


int impl_TAP_Osd_DrawRectangle(word rgn, dword x, dword y, dword w, dword h, dword t, dword color)
{
	return spFramework->Osd_DrawRectangle(rgn,x, y, w, h, t, color);
}


int impl_TAP_Osd_DrawPixmap(word rgn, dword x, dword y, dword w, dword h, void* pixmap, bool sprite, byte dataFormat )
{
	return spFramework->Osd_DrawPixmap(rgn, x, y, w, h,pixmap, sprite, dataFormat );
}


int impl_TAP_Osd_Zoom( int xzoom, int yzoom )
{
	return spFramework->Osd_Zoom(xzoom, yzoom );
}


int impl_TAP_Osd_GetBaseInfo( TYPE_OsdBaseInfo* info )
{
	return spFramework->Osd_GetBaseInfo(info);
}


int impl_TAP_Osd_Copy(word srcRgnNum, word dstRgnNum, dword srcX, dword srcY, dword w, dword h, dword dstX, dword dstY,  bool sprite)
{
	return spFramework->Osd_Copy(srcRgnNum, dstRgnNum, srcX, srcY, w, h, dstX, dstY, sprite);
}


int TAP_Osd_PutGd(word rgn, int x, int y, TYPE_GrData*  gd, bool sprite)
{
	return spFramework->Osd_PutGd(rgn, x, y, gd, sprite);
}


byte*	impl_TAP_Osd_SaveBox( word rgn, dword x, dword y, dword w, dword h )
{
	return spFramework->Osd_SaveBox(rgn, x, y, w, h);
}


void impl_TAP_Osd_RestoreBox( word rgn, dword x, dword y, dword w, dword h, void* data )
{
	spFramework->Osd_RestoreBox(rgn, x, y, w, h,data);
}


int impl_TAP_Osd_GetPixel( word rgn, dword x, dword y, void* pix )
{
	return spFramework->Osd_GetPixel(rgn, x, y,pix);
}


int impl_TAP_Osd_PutPixel( word rgn, dword x, dword y, dword pix )
{
	return spFramework->Osd_PutPixel(rgn, x, y, pix );
}


// -- HDD FUNCTION ---------------------------------------------------

TYPE_File*	impl_TAP_Hdd_Fopen( char* name )
{
	return spFramework->Hdd_Fopen(name);
}


dword impl_TAP_Hdd_Fread( void* buf, dword blksize, dword blk, TYPE_File* file )
{
	return spFramework->Hdd_Fread(buf, blksize, blk,file);
}


dword impl_TAP_Hdd_Fwrite( void* buf, dword blksize, dword blk, TYPE_File* file )
{
	return spFramework->Hdd_Fwrite(buf, blksize, blk,file);
}


void impl_TAP_Hdd_Fclose( TYPE_File* file )
{
	spFramework->Hdd_Fclose(file);
}


dword impl_TAP_Hdd_FindFirst( TYPE_File* file )
{
	return spFramework->Hdd_FindFirst(file);
}


dword impl_TAP_Hdd_FindNext( TYPE_File* file )
{
	return spFramework->Hdd_FindNext(file);
}


dword impl_TAP_Hdd_Fseek( TYPE_File* file, long pos, long where )
{
	return spFramework->Hdd_Fseek(file, pos, where );
}


dword impl_TAP_Hdd_Flen( TYPE_File* file )
{
	return spFramework->Hdd_Flen(file);
}


dword impl_TAP_Hdd_Ftell( TYPE_File* file )
{
	return spFramework->Hdd_Ftell(file);
}


dword impl_TAP_Hdd_TotalSize()
{
	return spFramework->Hdd_TotalSize();
}


dword impl_TAP_Hdd_FreeSize()
{
	return spFramework->Hdd_FreeSize();
}


word impl_TAP_Hdd_Delete( char* name )
{
	return spFramework->Hdd_Delete(name);
}


bool impl_TAP_Hdd_Exist( char* name )
{
	return spFramework->Hdd_Exist(name);
}


dword impl_TAP_Hdd_Create( char* name, byte attr )
{
	return spFramework->Hdd_Create(name, attr );
}


word impl_TAP_Hdd_ChangeDir( char* dir )
{
	return spFramework->Hdd_ChangeDir(dir);
}


int impl_TAP_Hdd_PlayTs( char* name )
{
	return spFramework->Hdd_PlayTs(name);
}


int impl_TAP_Hdd_StopTs()
{
	return spFramework->Hdd_StopTs();
}


int impl_TAP_Hdd_PlayMp3( char* name )
{
	return spFramework->Hdd_PlayMp3(name);
}


int impl_TAP_Hdd_StopMp3()
{
	return spFramework->Hdd_StopMp3();
}


bool impl_TAP_Hdd_Rename(   	char* oldName, char* newName )
{
	return spFramework->Hdd_Rename( oldName,newName);
}


bool impl_TAP_Hdd_StartRecord()
{
	return spFramework->Hdd_StartRecord();
}


bool impl_TAP_Hdd_StopRecord( byte recSlot )
{
	return spFramework->Hdd_StopRecord(recSlot );
}


bool impl_TAP_Hdd_GetRecInfo( byte recSlot, TYPE_RecInfo* recInfo )
{
	return spFramework->Hdd_GetRecInfo(recSlot,recInfo);
}


bool impl_TAP_Hdd_GetPlayInfo( TYPE_PlayInfo* playInfo )
{
	return spFramework->Hdd_GetPlayInfo(playInfo);
}


bool impl_TAP_Hdd_GetHddID(char* modelNo, char* serialNo)
{
	return spFramework->Hdd_GetHddID(modelNo,serialNo);
}


bool impl_TAP_Hdd_ChangePlaybackPos( dword blockPos )
{
	return spFramework->Hdd_ChangePlaybackPos(blockPos );
}


bool impl_TAP_Hdd_GotoBookmark()
{
	return spFramework->Hdd_GotoBookmark();
}


bool impl_TAP_Hdd_SetBookmark()
{
	return spFramework->Hdd_SetBookmark();
}


// -- STRING FUNCTION ---------------------
int impl_TAP_Osd_PutS(word rgn, dword x, dword y, dword maxX, const char*  str, word fcolor, word bcolor, byte fntType, byte fntSize, byte bDot, byte align)
{
	return spFramework->Osd_PutS(rgn, x, y, maxX,  str, fcolor, bcolor, fntType, fntSize, bDot, align);
}


int impl_TAP_Osd_GetW( const char* str, byte fntType, byte fntSize )
{
	return spFramework->Osd_GetW(  str, fntType, fntSize );
}


int impl_TAP_Osd_PutString(word rgn, dword x, dword y, dword maxX, const char*  str, word fcolor, word bcolor, byte fntType, byte fntSize, byte nextLine)
{
	return spFramework->Osd_PutString(rgn, x, y, maxX,  str, fcolor, bcolor, fntType, fntSize, nextLine);
}


int impl_TAP_Osd_DrawString(const char*  str, dword dstWidth, word color, byte*  dest, dword maxWidth, byte fntType, byte fntSize)
{
	return spFramework->Osd_DrawString( str, dstWidth, color, dest, maxWidth, fntType, fntSize);
}


int impl_TAP_Osd_PutStringAf(word rgn, dword x, dword y, dword maxX, const char*  str, word fcolor, word bcolor, byte fntType, byte fntSize, byte nextLine)
{
	return spFramework->Osd_PutStringAf(rgn, x, y, maxX,  str, fcolor, bcolor, fntType, fntSize, nextLine);
}


// -- WINDOW FUNCTION ---------------------------------------------------
void impl_TAP_Win_SetTitle( TYPE_Window* win, const char* str, byte fntType, byte fntSize )
{
	spFramework->Win_SetTitle(win, str, fntType, fntSize);
}

void impl_TAP_Win_SetColor(TYPE_Window*  win, word titleBack, word titleText, word bodyBack, word bodyText, word border, word shadow, word dark, word light)
{
	spFramework->Win_SetColor(win, titleBack, titleText, bodyBack, bodyText, border, shadow, dark, light);
}

void impl_TAP_Win_SetDefaultColor( TYPE_Window* win )
{
	spFramework->Win_SetDefaultColor(win);
}

void	impl_TAP_Win_Draw 	( TYPE_Window* win )
{
	spFramework->Win_Draw(win);
}

void	impl_TAP_Win_Create	( TYPE_Window* win, word rgn, dword x, dword y, dword w, dword h, byte save, byte bScr )
{
	spFramework->Win_Create(win, rgn, x, y, w, h, save, bScr);
}

dword	impl_TAP_Win_Delete	( TYPE_Window* win )
{
	return spFramework->Win_Delete(win);
}

void	impl_TAP_Win_SetFont	( TYPE_Window* win, byte fntType, byte fntSize )
{
	spFramework->Win_SetFont(win, fntType, fntSize);
}

void	impl_TAP_Win_AddItem	( TYPE_Window* win, char* str )
{
	spFramework->Win_AddItem(win, str);
}

dword impl_TAP_Win_GetSelection( TYPE_Window* win )
{
	return spFramework->Win_GetSelection(win);
}

void impl_TAP_Win_SetSelection( TYPE_Window* win, dword pos )
{
	spFramework->Win_SetSelection(win, pos);
}

void	impl_TAP_Win_Action	( TYPE_Window* win, dword key )
{
	spFramework->Win_Action(win, key);
}


void impl_TAP_Win_SetAvtice( TYPE_Window* win, dword itemNo, byte active )
{
	spFramework->Win_SetAvtice(win, itemNo, active);
}

void impl_TAP_Win_SetDrawItemFunc( TYPE_Window* win, TYPE_DrawItemFunc func )
{
	spFramework->Win_SetDrawItemFunc(win, func);
}


// ---------------------------------------------------------------------

int impl_TAP_Channel_GetTotalNum( int* nTvSvc, int* nRadioSvc )
{
	return spFramework->Channel_GetTotalNum( nTvSvc, nRadioSvc);
}


int impl_TAP_Channel_GetFirstInfo( int svcType, TYPE_TapChInfo* chInfo )
{
	return spFramework->Channel_GetFirstInfo(svcType,chInfo);
}


int impl_TAP_Channel_GetNextInfo( TYPE_TapChInfo* chInfo )
{
	return spFramework->Channel_GetNextInfo(chInfo);
}


int impl_TAP_Channel_GetInfo( int svcType, int svcNum, TYPE_TapChInfo* chInfo )
{
	return spFramework->Channel_GetInfo(svcType, svcNum,chInfo);
}


int impl_TAP_Channel_GetCurrent( int* tvRadio, int* svcNum )
{
	return spFramework->Channel_GetCurrent( tvRadio, svcNum);
}


int impl_TAP_Channel_GetCurrentSubTV(int* svcNum )
{
	return spFramework->Channel_GetCurrentSubTV(svcNum );
}


int impl_TAP_Channel_Start( int mainSub, int svcType, int chNum )
{
	return spFramework->Channel_Start(mainSub, svcType, chNum );
}


int impl_TAP_Channel_Scale( int mainSub, long x, long y, long w, long h, bool anim )
{
	return spFramework->Channel_Scale(mainSub, x, y, w, h, anim );
}


int impl_TAP_Channel_IsPlayable( int mainSub, int svcType, int chNum )
{
	return spFramework->Channel_IsPlayable(mainSub, svcType, chNum );
}


int impl_TAP_Channel_Move( int mainSub, int dir )
{
	return spFramework->Channel_Move(mainSub, dir );
}


int impl_TAP_Channel_Stop( int mainSub )
{
	return spFramework->Channel_Stop(mainSub );
}


int impl_TAP_Channel_GetTotalAudioTrack()
{
	return spFramework->Channel_GetTotalAudioTrack();
}


char* impl_TAP_Channel_GetAudioTrackName( int trkNum, char* trkName )
{
	return spFramework->Channel_GetAudioTrackName(trkNum,trkName);
}


int impl_TAP_Channel_SetAudioTrack( int trkNum )
{
	return spFramework->Channel_SetAudioTrack(trkNum );
}


int impl_TAP_Channel_GetTotalSubtitleTrack()
{
	return spFramework->Channel_GetTotalSubtitleTrack();
}


char* impl_TAP_Channel_GetSubtitleTrackName( int trkNum, char* trkName )
{
	return spFramework->Channel_GetSubtitleTrackName(trkNum,trkName);
}


int impl_TAP_Channel_SetSubtitleTrack( int trkNum )
{
	return spFramework->Channel_SetSubtitleTrack(trkNum );
}


bool impl_TAP_Channel_IsStarted( int mainSub )
{
	return spFramework->Channel_IsStarted(mainSub );
}


// --- EPG FUNCTIONS ---------------------------------------------------

TYPE_TapEvent*	impl_TAP_GetEvent( int svcType, int svcNum, int* eventNum )
{
	return spFramework->GetEvent(svcType, svcNum, eventNum);
}

TYPE_TapEvent*	impl_TAP_GetCurrentEvent( int svcType, int svcNum )
{
	return spFramework->GetCurrentEvent(svcType, svcNum);
}

bool impl_TAP_PutEvent( byte* eitData )
{
	return spFramework->PutEvent(eitData);
}


word impl_TAP_ControlEit( bool enable )
{
	return spFramework->ControlEit(enable );
}


bool impl_TAP_EPG_UpdateEvent(  int svcType, int svcNum, dword evtid, dword runningStatus )
{
	return spFramework->EPG_UpdateEvent(svcType, svcNum, evtid, runningStatus );
}


bool impl_TAP_EPG_DeleteEvent(  int svcType, int svcNum, dword evtid )
{
	return spFramework->EPG_DeleteEvent(svcType, svcNum, evtid );
}


byte* impl_TAP_EPG_GetExtInfo( TYPE_TapEvent* tapEvtInfo )
{
	return spFramework->EPG_GetExtInfo(tapEvtInfo);
}



// --- USB FUNCTIONS ---------------------------------------------------

void impl_TAP_Usb_Read( byte* pBuff, word size, void pFunc(word size) )
{
	spFramework->Usb_Read( pBuff, size, pFunc );
}


void impl_TAP_Usb_Write( byte* pBuff, word size, void pFunc(word size) )
{
	spFramework->Usb_Write( pBuff, size, pFunc );
}


void impl_TAP_Usb_PacketRead( byte* pBuff, word size, word pFunc(word size) )
{
	spFramework->Usb_PacketRead( pBuff, size, pFunc );
}


void impl_TAP_Usb_PacketWrite( byte* pBuff, word size, void pFunc(word size), dword cmd )
{
	spFramework->Usb_PacketWrite( pBuff, size, pFunc, cmd );
}


void impl_TAP_Usb_Cancel()
{
	spFramework->Usb_Cancel();
}


// --- TIMER FUNCTIONS ---------------------------------------------------

int impl_TAP_Timer_GetTotalNum()
{
	return spFramework->Timer_GetTotalNum();
}


bool impl_TAP_Timer_GetInfo( int entryNum, TYPE_TimerInfo* info )
{
	return spFramework->Timer_GetInfo(entryNum,info);
}


int impl_TAP_Timer_Add( TYPE_TimerInfo* info )
{
	return spFramework->Timer_Add(info);
}


int impl_TAP_Timer_Modify( int entryNum, TYPE_TimerInfo* info )
{
	return spFramework->Timer_Modify(entryNum,info);
}


bool impl_TAP_Timer_Delete( int entryNum )
{
	return spFramework->Timer_Delete(entryNum );
}


// --- VFD FUNCTIONS ---------------------------------------------------

int impl_TAP_Vfd_GetStatus()
{
	return spFramework->Vfd_GetStatus();
}


void impl_TAP_Vfd_Control( bool underTapCtrl )
{
	spFramework->Vfd_Control(underTapCtrl );
}


void impl_TAP_Vfd_SendData( byte* data, byte dataLen )
{
	spFramework->Vfd_SendData( data, dataLen );
}




/// Set up the function pointers

// -- SYSTEM FUNCTION ---------------------------------------------------
void	(*TAP_SystemProc)() = &impl_TAP_SystemProc;
dword	(*TAP_GetState)( dword* state, dword* subState ) = &impl_TAP_GetState;
void	(*TAP_ExitNormal)() = &impl_TAP_ExitNormal;
void	(*TAP_EnterNormal)() = &impl_TAP_EnterNormal;
void	(*TAP_Exit)() = &impl_TAP_Exit;

byte	(*TAP_KbHit)() = &impl_TAP_KbHit;
byte	(*TAP_GetCh)() = &impl_TAP_GetCh;
void	(*TAP_PutCh)(byte c) = &impl_TAP_PutCh;
void (*TAP_PutByte)(byte c) = &impl_TAP_PutByte;
void	(*TAP_PrintSys)(const void* fmt, ...) = &impl_TAP_PrintSys;
void	(*TAP_Print)(const void* fmt, ...) = &impl_TAP_Print;

void	(*TAP_SysOsdControl)( TYPE_TapSysOsdId osdId, bool ctrl ) = &impl_TAP_SysOsdControl;
void	(*TAP_SetBaudRate)( dword baudRate ) = &impl_TAP_SetBaudRate;
word	(*TAP_SetBk)( bool onOff ) = &impl_TAP_SetBk;

// -- COMMON FUNCTION ---------------------------------------------------
int	    (*TAP_SetSoundLevel)(byte soundLevel) = &impl_TAP_SetSoundLevel;
int	    (*TAP_GetTime)(word* mjd, byte* hour, byte* min, byte* sec) = &impl_TAP_GetTime;
void	(*TAP_MemInfo)(dword* heapSize, dword* freeHeapSize, dword* availHeapSize) = &impl_TAP_MemInfo;
void*	(*TAP_MemAlloc)(dword size) = &impl_TAP_MemAlloc;
int	    (*TAP_MemFree)(const void* addr) = &impl_TAP_MemFree;
dword	(*TAP_GetTick)() = &impl_TAP_GetTick;
void	(*TAP_Delay)(dword dm10) = &impl_TAP_Delay;
int	    (*TAP_ExtractMjd)(word mjd, word* year, byte* month, byte* day, byte* weekDay) = &impl_TAP_ExtractMjd;
int	    (*TAP_MakeMjd)(word year, byte month, byte day) = &impl_TAP_MakeMjd;
long	(*TAP_Sin)( long mul, long th ) = &impl_TAP_Sin;
long	(*TAP_Cos)( long mul, long th ) = &impl_TAP_Cos;
void	(*TAP_SPrint)(void* s, const void* fmt, ...) = &impl_TAP_SPrint;
int 	(*TAP_GetSignalLevel)() = &impl_TAP_GetSignalLevel;
int 	(*TAP_GetSignalQuality)() = &impl_TAP_GetSignalQuality;
int 	(*TAP_SetInfoboxTime)( int sec ) = &impl_TAP_SetInfoboxTime;
int		(*TAP_PlayPCM)( void* pcmdata, int size, int freq, void (*Callback)() ) = &impl_TAP_PlayPCM;
int 	(*TAP_CaptureScreen)( int mainSub, TYPE_VideoBank* videoBank ) = &impl_TAP_CaptureScreen;

int 	(*TAP_SetSystemVar)( int varId, int value ) = &impl_TAP_SetSystemVar;
int 	(*TAP_GetSystemVar)( int varId ) = &impl_TAP_GetSystemVar;
int 	(*TAP_WriteSystemVar)() = &impl_TAP_WriteSystemVar;
void	(*TAP_GenerateEvent)(  word evtCode, dword param1, dword param2 ) = &impl_TAP_GenerateEvent;

// -- OSD FUNCTION ---------------------------------------------------
int	(*TAP_Osd_Create)( dword x, dword y, dword w, dword h, byte lutIdx, int flag ) = &impl_TAP_Osd_Create;
int	(*TAP_Osd_Delete)(word rgn) = &impl_TAP_Osd_Delete;
int	(*TAP_Osd_Move)(word rgn, dword x, dword y) = &impl_TAP_Osd_Move;
int	(*TAP_Osd_FillBox)(word rgn, dword x, dword y, dword w, dword h, dword color) = &impl_TAP_Osd_FillBox;
int	(*TAP_Osd_PutBox)(word rgn, dword x, dword y, dword w, dword h, const void* data, bool sprite, byte dataFormat ) = &impl_TAP_Osd_PutBox;
int	(*TAP_Osd_GetBox)(word rgn, dword x, dword y, dword w, dword h, void* data) = &impl_TAP_Osd_GetBox;
int	(*TAP_Osd_SetLut)(byte lutIdx, byte lut[][4] ) = &impl_TAP_Osd_SetLut;
int	(*TAP_Osd_Ctrl)(word rgn, dword ctrl) = &impl_TAP_Osd_Ctrl;
int	(*TAP_Osd_SetTransparency)( word rgn, char rt) = &impl_TAP_Osd_SetTransparency;

int	(*TAP_Osd_Draw3dBox)( word rgn, dword x, dword y, dword w, dword h, dword c1, dword c2 ) = &impl_TAP_Osd_Draw3dBox;
int	(*TAP_Osd_Draw3dBoxFill)( word rgn, dword x, dword y, dword w, dword h, dword c1, dword c2, dword fc ) = &impl_TAP_Osd_Draw3dBoxFill;

int	(*TAP_Osd_DrawRectangle)(word rgn, dword x, dword y, dword w, dword h, dword t, dword color) = &impl_TAP_Osd_DrawRectangle;
int	(*TAP_Osd_DrawPixmap)(word rgn, dword x, dword y, dword w, dword h, void* pixmap, bool sprite, byte dataFormat ) = &impl_TAP_Osd_DrawPixmap;
int (*TAP_Osd_Zoom)( int xzoom, int yzoom ) = &impl_TAP_Osd_Zoom;
int (*TAP_Osd_GetBaseInfo)( TYPE_OsdBaseInfo* info ) = &impl_TAP_Osd_GetBaseInfo;
int (*TAP_Osd_Copy)(word srcRgnNum, word dstRgnNum, dword srcX, dword srcY, dword w, dword h, dword dstX, dword dstY,  bool sprite) = &impl_TAP_Osd_Copy;
//  int	(*TAP_Osd_PutGd__)(word rgn, int x, int y, TYPE_GrData*  gd, bool sprite);
//int	TAP_Osd_PutGd(word rgn, int x, int y, TYPE_GrData*  gd, bool sprite) = &impl_TAP_Osd_PutGd;

byte*	(*TAP_Osd_SaveBox)( word rgn, dword x, dword y, dword w, dword h ) = &impl_TAP_Osd_SaveBox;
void	(*TAP_Osd_RestoreBox)( word rgn, dword x, dword y, dword w, dword h, void* data ) = &impl_TAP_Osd_RestoreBox;

int	(*TAP_Osd_GetPixel)( word rgn, dword x, dword y, void* pix ) = &impl_TAP_Osd_GetPixel;
int	(*TAP_Osd_PutPixel)( word rgn, dword x, dword y, dword pix ) = &impl_TAP_Osd_PutPixel;

// -- HDD FUNCTION ---------------------------------------------------

TYPE_File*	(*TAP_Hdd_Fopen)( char* name ) = &impl_TAP_Hdd_Fopen;
dword		(*TAP_Hdd_Fread)( void* buf, dword blksize, dword blk, TYPE_File* file ) = &impl_TAP_Hdd_Fread;
dword		(*TAP_Hdd_Fwrite)( void* buf, dword blksize, dword blk, TYPE_File* file ) = &impl_TAP_Hdd_Fwrite;
void		(*TAP_Hdd_Fclose)( TYPE_File* file ) = &impl_TAP_Hdd_Fclose;
dword		(*TAP_Hdd_FindFirst)( TYPE_File* file ) = &impl_TAP_Hdd_FindFirst;
dword		(*TAP_Hdd_FindNext)( TYPE_File* file ) = &impl_TAP_Hdd_FindNext;
dword		(*TAP_Hdd_Fseek)( TYPE_File* file, long pos, long where ) = &impl_TAP_Hdd_Fseek;
dword		(*TAP_Hdd_Flen)( TYPE_File* file ) = &impl_TAP_Hdd_Flen;
dword		(*TAP_Hdd_Ftell)( TYPE_File* file ) = &impl_TAP_Hdd_Ftell;
dword		(*TAP_Hdd_TotalSize)() = &impl_TAP_Hdd_TotalSize;
dword		(*TAP_Hdd_FreeSize)() = &impl_TAP_Hdd_FreeSize;
word		(*TAP_Hdd_Delete)( char* name ) = &impl_TAP_Hdd_Delete;
bool		(*TAP_Hdd_Exist)( char* name ) = &impl_TAP_Hdd_Exist;
dword		(*TAP_Hdd_Create)( char* name, byte attr ) = &impl_TAP_Hdd_Create;
word		(*TAP_Hdd_ChangeDir)( char* dir ) = &impl_TAP_Hdd_ChangeDir;

int			(*TAP_Hdd_PlayTs)( char* name ) = &impl_TAP_Hdd_PlayTs;
int			(*TAP_Hdd_StopTs)() = &impl_TAP_Hdd_StopTs;
int			(*TAP_Hdd_PlayMp3)( char* name ) = &impl_TAP_Hdd_PlayMp3;
int			(*TAP_Hdd_StopMp3)() = &impl_TAP_Hdd_StopMp3;
bool		(*TAP_Hdd_Rename)(   	char* oldName, char* newName ) = &impl_TAP_Hdd_Rename;

bool		(*TAP_Hdd_StartRecord)() = &impl_TAP_Hdd_StartRecord;
bool		(*TAP_Hdd_StopRecord)( byte recSlot ) = &impl_TAP_Hdd_StopRecord;
bool		(*TAP_Hdd_GetRecInfo)( byte recSlot, TYPE_RecInfo* recInfo ) = &impl_TAP_Hdd_GetRecInfo;
bool		(*TAP_Hdd_GetPlayInfo)( TYPE_PlayInfo* playInfo ) = &impl_TAP_Hdd_GetPlayInfo;
bool		(*TAP_Hdd_GetHddID)(char* modelNo, char* serialNo) = &impl_TAP_Hdd_GetHddID;

bool		(*TAP_Hdd_ChangePlaybackPos)( dword blockPos ) = &impl_TAP_Hdd_ChangePlaybackPos;
bool		(*TAP_Hdd_GotoBookmark)() = &impl_TAP_Hdd_GotoBookmark;
bool		(*TAP_Hdd_SetBookmark)() = &impl_TAP_Hdd_SetBookmark;

// -- STRING FUNCTION ---------------------
int	(*TAP_Osd_PutS)(word rgn, dword x, dword y, dword maxX, const char*  str, word fcolor, word bcolor, byte fntType, byte fntSize, byte bDot, byte align) = &impl_TAP_Osd_PutS;
int	(*TAP_Osd_GetW)( const char* str, byte fntType, byte fntSize ) = &impl_TAP_Osd_GetW;
int	(*TAP_Osd_PutString)(word rgn, dword x, dword y, dword maxX, const char*  str, word fcolor, word bcolor, byte fntType, byte fntSize, byte nextLine) = &impl_TAP_Osd_PutString;
int	(*TAP_Osd_DrawString)(const char*  str, dword dstWidth, word color, byte*  dest, dword maxWidth, byte fntType, byte fntSize) = &impl_TAP_Osd_DrawString;
int	(*TAP_Osd_PutStringAf)(word rgn, dword x, dword y, dword maxX, const char*  str, word fcolor, word bcolor, byte fntType, byte fntSize, byte nextLine) = &impl_TAP_Osd_PutStringAf;

// -- WINDOW FUNCTION ---------------------------------------------------
void	(*TAP_Win_SetTitle)( TYPE_Window* win, const char* str, byte fntType, byte fntSize ) = &impl_TAP_Win_SetTitle;
void	(*TAP_Win_SetColor)(TYPE_Window*  win, word titleBack, word titleText, word bodyBack, word bodyText, word border, word shadow, word dark, word light) = &impl_TAP_Win_SetColor;
void	(*TAP_Win_SetDefaultColor)( TYPE_Window* win ) = &impl_TAP_Win_SetDefaultColor;
void	(*TAP_Win_Draw) 	( TYPE_Window* win ) = &impl_TAP_Win_Draw;
void	(*TAP_Win_Create)	( TYPE_Window* win, word rgn, dword x, dword y, dword w, dword h, byte save, byte bScr ) = &impl_TAP_Win_Create;
dword	(*TAP_Win_Delete)	( TYPE_Window* win ) = &impl_TAP_Win_Delete;
void	(*TAP_Win_SetFont)	( TYPE_Window* win, byte fntType, byte fntSize ) = &impl_TAP_Win_SetFont;
void	(*TAP_Win_AddItem)	( TYPE_Window* win, char* str ) = &impl_TAP_Win_AddItem;
dword	(*TAP_Win_GetSelection)( TYPE_Window* win ) = &impl_TAP_Win_GetSelection;
void	(*TAP_Win_SetSelection)( TYPE_Window* win, dword pos ) = &impl_TAP_Win_SetSelection;
void	(*TAP_Win_Action)	( TYPE_Window* win, dword key ) = &impl_TAP_Win_Action;

void	(*TAP_Win_SetAvtice)( TYPE_Window* win, dword itemNo, byte active ) = &impl_TAP_Win_SetAvtice;
void	(*TAP_Win_SetDrawItemFunc)( TYPE_Window* win, TYPE_DrawItemFunc func ) = &impl_TAP_Win_SetDrawItemFunc;

// ---------------------------------------------------------------------

int (*TAP_Channel_GetTotalNum)( int* nTvSvc, int* nRadioSvc ) = &impl_TAP_Channel_GetTotalNum;
int (*TAP_Channel_GetFirstInfo)( int svcType, TYPE_TapChInfo* chInfo ) = &impl_TAP_Channel_GetFirstInfo;
int (*TAP_Channel_GetNextInfo)( TYPE_TapChInfo* chInfo ) = &impl_TAP_Channel_GetNextInfo;
int (*TAP_Channel_GetInfo)( int svcType, int svcNum, TYPE_TapChInfo* chInfo ) = &impl_TAP_Channel_GetInfo;
int (*TAP_Channel_GetCurrent)( int* tvRadio, int* svcNum ) = &impl_TAP_Channel_GetCurrent;
int (*TAP_Channel_GetCurrentSubTV)(int* svcNum ) = &impl_TAP_Channel_GetCurrentSubTV;
int (*TAP_Channel_Start)( int mainSub, int svcType, int chNum ) = &impl_TAP_Channel_Start;
int (*TAP_Channel_Scale)( int mainSub, long x, long y, long w, long h, bool anim ) = &impl_TAP_Channel_Scale;
int (*TAP_Channel_IsPlayable)( int mainSub, int svcType, int chNum ) = &impl_TAP_Channel_IsPlayable;
int (*TAP_Channel_Move)( int mainSub, int dir ) = &impl_TAP_Channel_Move;
int (*TAP_Channel_Stop)( int mainSub ) = &impl_TAP_Channel_Stop;
int (*TAP_Channel_GetTotalAudioTrack)() = &impl_TAP_Channel_GetTotalAudioTrack;
char* (*TAP_Channel_GetAudioTrackName)( int trkNum, char* trkName ) = &impl_TAP_Channel_GetAudioTrackName;
int (*TAP_Channel_SetAudioTrack)( int trkNum ) = &impl_TAP_Channel_SetAudioTrack;
int (*TAP_Channel_GetTotalSubtitleTrack)() = &impl_TAP_Channel_GetTotalSubtitleTrack;
char* (*TAP_Channel_GetSubtitleTrackName)( int trkNum, char* trkName ) = &impl_TAP_Channel_GetSubtitleTrackName;
int (*TAP_Channel_SetSubtitleTrack)( int trkNum ) = &impl_TAP_Channel_SetSubtitleTrack;
bool (*TAP_Channel_IsStarted)( int mainSub ) = &impl_TAP_Channel_IsStarted;

// --- EPG FUNCTIONS ---------------------------------------------------

TYPE_TapEvent*	(*TAP_GetEvent)( int svcType, int svcNum, int* eventNum ) = &impl_TAP_GetEvent;
TYPE_TapEvent*	(*TAP_GetCurrentEvent)( int svcType, int svcNum ) = &impl_TAP_GetCurrentEvent;
bool (*TAP_PutEvent)( byte* eitData ) = &impl_TAP_PutEvent;

word (*TAP_ControlEit)( bool enable ) = &impl_TAP_ControlEit;

bool (*TAP_EPG_UpdateEvent)(  int svcType, int svcNum, dword evtid, dword runningStatus ) = &impl_TAP_EPG_UpdateEvent;
bool (*TAP_EPG_DeleteEvent)(  int svcType, int svcNum, dword evtid ) = &impl_TAP_EPG_DeleteEvent;
byte* (*TAP_EPG_GetExtInfo)( TYPE_TapEvent* tapEvtInfo ) = &impl_TAP_EPG_GetExtInfo;

// --- USB FUNCTIONS ---------------------------------------------------

void (*TAP_Usb_Read)( byte* pBuff, word size, void (*pFunc)(word size) ) = &impl_TAP_Usb_Read;
void (*TAP_Usb_Write)( byte* pBuff, word size, void (*pFunc)(word size) ) = &impl_TAP_Usb_Write;
void (*TAP_Usb_PacketRead)( byte* pBuff, word size, word (*pFunc)(word size) ) = &impl_TAP_Usb_PacketRead;
void (*TAP_Usb_PacketWrite)( byte* pBuff, word size, void (*pFunc)(word size), dword cmd ) = &impl_TAP_Usb_PacketWrite;
void (*TAP_Usb_Cancel)() = &impl_TAP_Usb_Cancel;

// --- TIMER FUNCTIONS ---------------------------------------------------

int  (*TAP_Timer_GetTotalNum)() = &impl_TAP_Timer_GetTotalNum;
bool (*TAP_Timer_GetInfo)( int entryNum, TYPE_TimerInfo* info ) = &impl_TAP_Timer_GetInfo;
int (*TAP_Timer_Add)( TYPE_TimerInfo* info ) = &impl_TAP_Timer_Add;
int (*TAP_Timer_Modify)( int entryNum, TYPE_TimerInfo* info ) = &impl_TAP_Timer_Modify;
bool (*TAP_Timer_Delete)( int entryNum ) = &impl_TAP_Timer_Delete;

// --- VFD FUNCTIONS ---------------------------------------------------

int (*TAP_Vfd_GetStatus)() = &impl_TAP_Vfd_GetStatus;
void (*TAP_Vfd_Control)( bool underTapCtrl ) = &impl_TAP_Vfd_Control;
void (*TAP_Vfd_SendData)( byte* data, byte dataLen ) = &impl_TAP_Vfd_SendData;

DWORD _tap_startAddr = 0;