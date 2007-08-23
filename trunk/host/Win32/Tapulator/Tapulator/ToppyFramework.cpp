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

#include "StdAfx.h"
#include ".\toppyframework.h"
#include "Window.h"
#include "TapModule.h"
#include <stdio.h>
#include <io.h>
#include <math.h>
#include "OSDRegion.h"
#include "MJDUtil.h"
#include "Heap.h"
#include "MainFrm.h"
#include "Connfiguration.h"

ToppyFramework::ToppyFramework(TapModule* pModule) : m_EPGImpl(&m_ChannelList), m_TheState(&m_ChannelList, &m_EPGImpl), m_Timers(&m_ChannelList)
{
	m_bFindFinished = true;
	m_bExitFlag = false;
	m_pMod = pModule;
	m_hCurrentFind = 0;
	m_iFirstTick = GetTickCount();

	if (GetConfig()->GetStartInAutoStart() == 1)
	{
		m_sCurrentFolder = "/ProgramFiles/Auto Start";
	}
	else
	{
		m_sCurrentFolder = "/ProgramFiles";
	}

	CString sRequiredFolder = MakePath("");
	DWORD dwAttrs = ::GetFileAttributes(sRequiredFolder);
	if ((dwAttrs == INVALID_FILE_ATTRIBUTES) || (((dwAttrs && FILE_ATTRIBUTE_DIRECTORY) == 0)))
	{
		AfxMessageBox("The Toppy FileSystem Folder specified as the initial working folder ('"+sRequiredFolder+"') does not exist."
			" This will lead to problems if Hdd functions are called.", MB_OK, 0);

	}
}

ToppyFramework::~ToppyFramework(void)
{
	if (m_hCurrentFind != NULL)
		FindClose(m_hCurrentFind);
	// TODO: delete outstanding file*/close files
}

#define DSTCOPY 0x00AA0029

bool ToppyFramework::NeedsRepaint()
{
	return m_OSDregions.IsDirty() || m_TheState.IsDirty();
}

void ToppyFramework::Draw(CDC* pDC)
{
	CDC* pMemDC = GetDC();


	if (NeedsRepaint())
	{
		m_TheState.Draw(pMemDC);

		CDC osdDC;
		osdDC.CreateCompatibleDC(pDC);
		osdDC.SetBkColor(TRANSPARENT_COLOUR);
		CBitmap bmOsd;
		bmOsd.CreateCompatibleBitmap(pDC, 720, 576);
		osdDC.SelectObject(&bmOsd);
		osdDC.FillSolidRect(0,0,720,576,TRANSPARENT_COLOUR);
		if (m_OSDregions.Draw(&osdDC))
		{
			CDC monoDC;
			monoDC.CreateCompatibleDC(pDC);
			CBitmap maskBm;
			maskBm.CreateBitmap(720, 576, 1, 1, NULL);
			monoDC.SelectObject(maskBm);
			osdDC.SetBkColor(TRANSPARENT_COLOUR);
			monoDC.BitBlt(0, 0, 720, 576, &osdDC, 0, 0, SRCCOPY);

			CDC tempDC;
			tempDC.CreateCompatibleDC(pDC);
			CBitmap tempBM;
			tempBM.CreateCompatibleBitmap(pDC, 720, 576);
			tempDC.SelectObject(&tempBM);
			tempDC.BitBlt(0,0, 720, 576, pMemDC, 0, 0, SRCCOPY);

			BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255-(BYTE)(GetConfig()->GetOsdTransparency() * 255/100), 0};
			pMemDC->AlphaBlend(0, 0, 720, 576, &osdDC, 0, 0, 720, 576, bf );
	//
	////		 now contains correct over OSD area, wrong in background
	//
			pMemDC->MaskBlt(0,0, 720, 576, &tempDC, 0, 0, maskBm, 0, 0, MAKEROP4(SRCCOPY, DSTCOPY));
			//memDC.BitBlt(0, 0, 720, 576, &osdDC, 0, 0, SRCCOPY);
		}
	}

	pDC->BitBlt(0,0, 720, 576, pMemDC, 0, 0, SRCCOPY); 
	ReleaseDC();
}

CMainFrame* ToppyFramework::GetMainFrame()
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	return pFrame;
}

CConfiguration* ToppyFramework::GetConfig()
{
	return GetMainFrame()->GetConfig();
}

CString ToppyFramework::MakePath(const CString& sName)
{
	return GetConfig()->GetRootFolderPath() + m_sCurrentFolder + "\\" + sName;
}

CString ToppyFramework::MakeRootPath(const CString& sPath)
{
	return GetConfig()->GetRootFolderPath() + "\\" + sPath;
}

CString ToppyFramework::MakeRelativePath(const CString& sPath)
{
	CString result = m_sCurrentFolder;

	if (sPath.IsEmpty() || sPath == ".")
		return result;

	if (sPath == "..")
		result = "../";

	if (sPath.Left(1) == "/")
		return MakeRootPath(sPath.Mid(1));
	return MakePath(sPath);
}

void ToppyFramework::Win_SetDefaultColor(TYPE_Window* win)
{
	Window::SetDefaultColor(win);
}

void ToppyFramework::Win_Create(TYPE_Window *win, word rgn, dword x, dword y, dword w, dword h, byte save, byte bScr)
{
	m_Windows.Create(win, m_OSDregions.Find(rgn), x, y, w, h, save, bScr);
}

void ToppyFramework::Win_SetTitle( TYPE_Window *win, const char *str, BYTE fntType, BYTE fntSize )
{
	m_Windows.Find(win)->SetTitle(str, fntType, fntSize);
}

void ToppyFramework::Win_SetColor(TYPE_Window * win, WORD titleBack, WORD titleText, WORD bodyBack, WORD bodyText, WORD border, WORD shadow, WORD dark, WORD light)
{
	Window::SetColor(win, titleBack, titleText, bodyBack, bodyText, border, shadow, dark, light);
}

void ToppyFramework::Win_Draw( TYPE_Window *win )
{
	m_Windows.Find(win)->Draw();
}

DWORD ToppyFramework::Win_Delete( TYPE_Window *win )
{
	m_Windows.Destroy(win);
	return 0; //TODO: correct return value?
}

void ToppyFramework::Win_SetFont( TYPE_Window *win, BYTE fntType, BYTE fntSize )
{
	m_Windows.Find(win)->SetFont(fntType, fntSize);
}

void ToppyFramework::Win_AddItem( TYPE_Window *win, char *str )
{
	m_Windows.Find(win)->AddItem(str);
}

DWORD ToppyFramework::Win_GetSelection( TYPE_Window *win )
{
	return 	m_Windows.Find(win)->GetSelection();
}

void ToppyFramework::Win_SetSelection( TYPE_Window *win, DWORD pos )
{
	m_Windows.Find(win)->SetSelection(pos);
}

void ToppyFramework::Win_Action( TYPE_Window *win, DWORD key )
{
	m_Windows.Find(win)->Action(key);
}

void ToppyFramework::Win_SetAvtice( TYPE_Window *win, DWORD itemNo, BYTE active )
{
	m_Windows.Find(win)->SetActive(itemNo, active);
}

void ToppyFramework::Win_SetDrawItemFunc( TYPE_Window *win, TYPE_DrawItemFunc func )
{
	m_Windows.Find(win)->SetDrawItemFunc(func);
}

void ToppyFramework::SystemProc()
{
	Sleep(1);
	GetMainFrame()->RepaintIfNeeded();
	MSG msg;
	BOOL bRet = false;
    while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
    { 
        if (bRet == -1)
        {
			break;
            // handle the error and possibly exit
        }
        else
        {
            TranslateMessage(&msg); 
            DispatchMessage(&msg); 
			return;
        }
    } 

	throw std::exception("AppExit"); // not a very clean way to exit :-(
}

DWORD ToppyFramework::GetState(DWORD *state, DWORD *subState )
{
	return m_TheState.GetState(state, subState);
}

void ToppyFramework::ExitNormal(void )
{
//	m_State = STATE_Tap; //??
	return;
}

void ToppyFramework::EnterNormal(void )
{
//	m_State = STATE_Normal; //??
	return;
}

void ToppyFramework::Exit(void)
{
	Heap::GetTheHeap()->DumpLeaks();
	m_bExitFlag = true;
//	throw std::exception("TAP_Exit");
}


BYTE ToppyFramework::KbHit(void)
{
	// RS232 - do not implement
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

BYTE ToppyFramework::GetCh(void)
{
	// RS232 - do not implement
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

void ToppyFramework::PutCh(BYTE c)
{
	// RS232 - do not implement
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return;
}

void ToppyFramework::PutByte(BYTE c)
{
	// RS232 - do not implement
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return;
}

void ToppyFramework::CheckBufferSize(const char* buffer, int iMaxLen, const char* failPoint)
{
	int iLen = strlen(buffer);
	if (iLen <= iMaxLen)
		return;

	CString sMsg;
	sMsg.Format("Buffer overrun in API : in %s got %d chars where limit is %d", failPoint, iLen, iMaxLen);

	LogError(sMsg);
	AfxMessageBox(sMsg + "\n(This error will crash the toppy...)", 0, 0);
}


void ToppyFramework::PrintSys(const void *fmt, va_list args)
{
	char buf[2048];
	_vsnprintf(buf, 2048, (LPCTSTR)fmt, args);

	CheckBufferSize(buf, 255, "TAP_PrintSys");

	LogUser((const char*)fmt, args);
	return;
}

void ToppyFramework::Print(const void *fmt, va_list args)
{
	char buf[2048];
	_vsnprintf(buf, 2048, (LPCTSTR)fmt, args);

	CheckBufferSize(buf, 255, "TAP_Print");

    LogUser((const char*)fmt, args);
	return;
}


void ToppyFramework::SysOsdControl(TYPE_TapSysOsdId osdId, bool ctrl )
{
	m_TheState.SysOsdControl(osdId, ctrl);
	return;
}

void ToppyFramework::SetBaudRate(DWORD baudRate )
{
	// RS232 - do not implement
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return;
}

WORD ToppyFramework::SetBk(bool onOff )
{
	// Undocumented
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}


// -- COMMON FUNCTION ---------------------------------------------------
int ToppyFramework::SetSoundLevel(BYTE soundLevel)
{
	return m_TheState.SetSystemVar(SYSVAR_Vol, soundLevel & 0x7F);
}

int ToppyFramework::GetTime(WORD *mjd, BYTE *hour, BYTE *min, BYTE *sec)
{
	*mjd = MJDUtil::Today();
	SYSTEMTIME time;
	GetSystemTime(&time);
	*hour = (BYTE) time.wHour;
	*min = (BYTE) time.wMinute;
	*sec = (BYTE) time.wSecond;
	return 1;
}

void ToppyFramework::MemInfo(DWORD *heapSize, DWORD *freeHeapSize, DWORD *availHeapSize)
{
	Heap::GetTheHeap()->MemInfo(heapSize, freeHeapSize, availHeapSize);
	return;
}

void* ToppyFramework::MemAlloc(DWORD size)
{
	return Heap::GetTheHeap()->Allocate(size);
}

int ToppyFramework::MemFree(const void *addr)
{
	Heap::GetTheHeap()->Release((void*) addr);
	return 1;
}

DWORD ToppyFramework::GetTick(void)
{
	return (GetTickCount()-m_iFirstTick)/10;
}

void ToppyFramework::Delay(DWORD dm10)
{
	Sleep(dm10 * 10); // should we do stuff (like idle events) here?
	return;
}

int ToppyFramework::ExtractMjd(WORD mjd, WORD *year, BYTE *month, BYTE *day, BYTE *weekDay)
{
	return MJDUtil::ExtractMJD(mjd, *year, *month, *day, *weekDay);
}

int ToppyFramework::MakeMjd(WORD year, BYTE month, BYTE day)
{
	return MJDUtil::MakeMJD(year, month, day);
}

#define PI 3.141592654 // more than enough for integral degrees!

long ToppyFramework::Sin(long mul, long th )
{
	double dResult = mul;
	dResult *= sin(th/180.0*PI);
	return (long) dResult;
}

long ToppyFramework::Cos(long mul, long th )
{
	double dResult = mul;
	dResult *= cos(th/180.0*PI);
	return (long) dResult;
}

void ToppyFramework::SPrint(void *s, const void *fmt, va_list args)
{
	vsprintf((char*) s, (const char*) fmt, args);
	return;
}

int ToppyFramework::GetSignalLevel(void)
{
	return 75;
}

int ToppyFramework::GetSignalQuality(void)
{
	return 100;
}

int ToppyFramework::SetInfoboxTime(int sec )
{
	// Delegate To State
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

int ToppyFramework::PlayPCM(void *pcmdata, int size, int freq, VoidVoidFuncPtr CallBack )
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

int ToppyFramework::CaptureScreen(int mainSub, TYPE_VideoBank *videoBank )
{
	// Captures TV - OSD included? Ignore
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

int ToppyFramework::SetSystemVar(int varId, int value )
{
	return m_TheState.SetSystemVar(varId, value);
}

int ToppyFramework::GetSystemVar(int varId )
{
	return m_TheState.GetSystemVar(varId);
}

int ToppyFramework::WriteSystemVar(void )
{
	return m_TheState.WriteSystemVar();
}

void ToppyFramework::GenerateEvent( WORD evtCode, DWORD param1, DWORD param2 )
{
	// should this come to us?
	GetMainFrame()->GetTapModule()->TAP_Event(evtCode, param1, param2);
}


// -- OSD FUNCTION ---------------------------------------------------
int ToppyFramework::Osd_Create( DWORD x, DWORD y, DWORD w, DWORD h, BYTE lutIdx, int flag )
{
	return m_OSDregions.Create(x,y,w,h,lutIdx,flag);
}

int ToppyFramework::Osd_Delete(WORD rgn)
{
	m_OSDregions.Destroy(rgn);
	return 0;
}

int ToppyFramework::Osd_Move(WORD rgn, DWORD x, DWORD y)
{
	return m_OSDregions.Find(rgn)->Move(x,y);
}

int ToppyFramework::Osd_FillBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, DWORD color)
{
	return m_OSDregions.Find(rgn)->FillBox(x,y,w,h,color);
}

int ToppyFramework::Osd_PutBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, const void *data, bool sprite, BYTE dataFormat )
{
	return m_OSDregions.Find(rgn)->PutBox(x,y,w,h, data, sprite, dataFormat);
}

int ToppyFramework::Osd_GetBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, void *data)
{
	return m_OSDregions.Find(rgn)->GetBox(x,y,w,h,data);
}

int ToppyFramework::Osd_SetLut(BYTE lutIdx, BYTE lut[][4] )
{
	return Regions::SetLut(lutIdx, lut);
}

int ToppyFramework::Osd_Ctrl(WORD rgn, DWORD ctrl)
{
	return m_OSDregions.Find(rgn)->Ctrl(ctrl);
}

int ToppyFramework::Osd_SetTransparency(WORD rgn, char rt)
{
	return m_OSDregions.Find(rgn)->SetTransparency(rt);
}


int ToppyFramework::Osd_Draw3dBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, DWORD c1, DWORD c2 )
{
	return m_OSDregions.Find(rgn)->Draw3dBox(x,y,w,h,c1,c2);
}

int ToppyFramework::Osd_Draw3dBoxFill(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, DWORD c1, DWORD c2, DWORD fc )
{
	return m_OSDregions.Find(rgn)->Draw3dBoxFill(x,y,w,h,c1,c2,fc);
}


int ToppyFramework::Osd_DrawRectangle(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, DWORD t, DWORD color)
{
	return m_OSDregions.Find(rgn)->DrawRectangle(x,y,w,h,t,color);
}

int ToppyFramework::Osd_DrawPixmap(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, void *pixmap, bool sprite, BYTE dataFormat )
{
	return m_OSDregions.Find(rgn)->DrawPixmap(x,y,w,h,pixmap, sprite, dataFormat);
}

int ToppyFramework::Osd_Zoom(int xzoom, int yzoom )
{
	return Regions::Zoom(xzoom,yzoom);
}

int ToppyFramework::Osd_GetBaseInfo(TYPE_OsdBaseInfo *info )
{
	return Regions::GetBaseInfo(info);
}

int ToppyFramework::Osd_Copy(WORD srcRgnNum, WORD dstRgnNum, DWORD srcX, DWORD srcY, DWORD w, DWORD h, DWORD dstX, DWORD dstY,  bool sprite)
{
	return m_OSDregions.Find(dstRgnNum)->Copy(m_OSDregions.Find(srcRgnNum), srcX, srcY, w, h, dstX, dstY, sprite);
}

int	ToppyFramework::Osd_PutGd(WORD rgn, int x, int y, TYPE_GrData * gd, bool sprite)
{
	return m_OSDregions.Find(rgn)->PutGd(x,y, gd, sprite);
}

BYTE* ToppyFramework::Osd_SaveBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h )
{
	return m_OSDregions.Find(rgn)->SaveBox(x,y,w,h);
}

void ToppyFramework::Osd_RestoreBox(WORD rgn, DWORD x, DWORD y, DWORD w, DWORD h, void *data )
{
	return m_OSDregions.Find(rgn)->RestoreBox(x,y,w,h,data);
}


int ToppyFramework::Osd_GetPixel(WORD rgn, DWORD x, DWORD y, void *pix )
{
	return m_OSDregions.Find(rgn)->GetPixel(x,y, pix);
}

int ToppyFramework::Osd_PutPixel(WORD rgn, DWORD x, DWORD y, DWORD pix )
{
	return m_OSDregions.Find(rgn)->PutPixel(x,y,pix);
}


// -- HDD FUNCTION ---------------------------------------------------

TYPE_File* ToppyFramework::Hdd_Fopen(char *name )
{
	CString sName(name);
	if ((sName.Find("/")!=-1) || (sName.Find("\\")!=-1))
	{
		LogError("Tried to open a file with a path in the name - %s", name);
        return 0;
	}

	FILE* handle = fopen(MakePath(name), "rb+"); // open for update
	if (handle == NULL)
	{
		LogError("Failed to open file %s", (LPCSTR) MakePath(name));
		return 0;
	}

	TYPE_File* pToppyHandle = new TYPE_File();
	ZeroMemory(pToppyHandle, sizeof(TYPE_File));
	strncpy(pToppyHandle->name, name, 100);

	m_mapOpenFiles[pToppyHandle] = handle;
	pToppyHandle->size = Hdd_Flen(pToppyHandle);

	LogInfo("Opened %s - handle 0x%x, TYPE_File 0x%x", (LPCSTR) MakePath(name), handle, pToppyHandle);

	pToppyHandle->startCluster = GetStartClusterHash(name);
	pToppyHandle->totalCluster = 0;
	// TODO: more initialization?
	return pToppyHandle;
}

DWORD ToppyFramework::Hdd_Fread(void *buf, DWORD blksize, DWORD blk, TYPE_File *file )
{
	FILE* handle = m_mapOpenFiles[file];
	if (handle == 0)
		return 0;

	int iPos = ftell(handle);

	int iResult =  fread(buf, blksize, blk, handle); //TODO: correct return value?

	if (iResult != blk)
		LogError("Tried to read %d blocks * %d bytes but only got %d blocks from file %x", blk, blksize, iResult, handle);
	else
		LogVerbose("Read %d blocks * %d bytes from TYPE_File 0x%x", blk, blksize, file);

	return iResult;
}

DWORD ToppyFramework::Hdd_Fwrite(void *buf, DWORD blksize, DWORD blk, TYPE_File *file )
{
	FILE* handle = m_mapOpenFiles[file];
	if (handle == 0)
		return 0;

	return fwrite(buf, blksize, blk, handle); //TODO: correct return value?
}

void ToppyFramework::Hdd_Fclose(TYPE_File *file )
{
	FILE* handle = m_mapOpenFiles[file];
	if (handle == 0)
	{
		LogError("Tried to close TYPE_File 0x%x which isn't open", file);
		return;
	}

	fclose(handle);
	LogInfo("Closed -	TYPE_File 0x%x", file);
	m_mapOpenFiles.erase(file);
	delete file;
}

DWORD ToppyFramework::GetStartClusterHash(const CString& filename)
{
	CString sName = MakePath(filename);
	sName.Replace("\\", "/");
	sName.Replace("//", "/");
	if (sName.Right(2) == "/.")
		sName = sName.Left(sName.GetLength()-2);
	if (sName.Right(3) == "/..")
	{
		sName = sName.Left(sName.GetLength()-3);
		int iIndex = sName.ReverseFind('/');
		sName = sName.Left(iIndex);
	}

	DWORD result = 0;
	for (int i=0; i<sName.GetLength(); i++)
	{
		result = _rotl(result,1) ^ sName[i];
	}

	return result;

}

void ToppyFramework::PopulateTYPE_File(TYPE_File* file, WIN32_FIND_DATA& findData)
{
	ZeroMemory(file, sizeof(TYPE_File));
	strncpy(file->name, findData.cFileName, 100);
	file->size = findData.nFileSizeLow;
	if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		if (strcmp(file->name, ".")==0)
			file->attr = 0xf1;
		else if (strcmp(file->name, "..")==0)
			file->attr = 0xf0;
		else file->attr = ATTR_FOLDER;
	}
	else
		file->attr = ATTR_NORMAL;
	file->startCluster = GetStartClusterHash(findData.cFileName);
}

DWORD ToppyFramework::Hdd_FindFirst(TYPE_File *file )
{
	//TODO: exclude "." in root dir?
	if (m_hCurrentFind)
		FindClose(m_hCurrentFind);
	WIN32_FIND_DATA findData;
	m_hCurrentFind = FindFirstFile(MakePath("*.*"), &findData);
	if (m_hCurrentFind == NULL)
		return 0;
	PopulateTYPE_File(file, findData);
	int iCount = CountFilesInCurrentFolder();
	m_bFindFinished = false;

	if (m_sCurrentFolder.IsEmpty())
	{
		CString name = file->name;
		if (name == ".." || name == ".")
			strcpy(file->name, "__ROOT__");
	}
	m_iFindCount = 0;
	return iCount;
}


DWORD ToppyFramework::Hdd_FindNext(TYPE_File *file )
{
	if (m_bFindFinished)
		return 0;

	bool bResult = false;

	do
	{
		bResult = true;
		WIN32_FIND_DATA findData;
		if (!FindNextFile(m_hCurrentFind, &findData))
		{
			m_bFindFinished = true;
			return ++m_iFindCount;
		}
		
		PopulateTYPE_File(file, findData);
		if (m_sCurrentFolder.IsEmpty())
		{
			CString name = file->name;
			if (name == ".." || name == ".")
				bResult = false;
		}
	} while (!bResult);

	return ++m_iFindCount;
}

DWORD ToppyFramework::Hdd_Fseek(TYPE_File *file, long pos, long where )
{
	FILE* handle = m_mapOpenFiles[file];
	if (handle == 0)
		return 0;

	if (IsLengthForFlenFSeekBug(file->size) && where != SEEK_END)
		pos-=512;

	return fseek(handle, pos, where); //TODO: correct return value?
}

bool ToppyFramework::IsLengthForFlenFSeekBug(DWORD dwLen)
{
	if (dwLen < 1023)
		return false;

	if (dwLen % 512)
		return false;

	return true;
}

DWORD ToppyFramework::Hdd_Flen(TYPE_File *file )
{
	FILE* handle = m_mapOpenFiles[file];
	DWORD iPos = ftell(handle);
	fseek(handle, 0, SEEK_END);
	DWORD dwRes = ftell(handle);
	fseek(handle, iPos, SEEK_SET);
	if (IsLengthForFlenFSeekBug(dwRes))
		dwRes-=512;
	return dwRes;
}

DWORD ToppyFramework::Hdd_Ftell(TYPE_File *file )
{
	FILE* handle = m_mapOpenFiles[file];
	return ftell(handle); //TODO: correct return value?
}

DWORD ToppyFramework::Hdd_TotalSize(void )
{
	ULARGE_INTEGER bytesAvail;
	ULARGE_INTEGER totalBytes;
	ULARGE_INTEGER totalFreeBytes;
	::GetDiskFreeSpaceEx(GetConfig()->GetRootFolderPath().Left(3), &bytesAvail, &totalBytes, &totalFreeBytes);

	LogInfo("Hdd_TotalSize - assuming return is in Kb");
	totalBytes.QuadPart /= 1024;
	return totalBytes.LowPart;
}

DWORD ToppyFramework::Hdd_FreeSize(void )
{
	ULARGE_INTEGER bytesAvail;
	ULARGE_INTEGER totalBytes;
	ULARGE_INTEGER totalFreeBytes;
	::GetDiskFreeSpaceEx(GetConfig()->GetRootFolderPath().Left(3), &bytesAvail, &totalBytes, &totalFreeBytes);

	LogInfo("Hdd_FreeSize - assuming return is in Kb");
	totalFreeBytes.QuadPart /= 1024;
	return totalFreeBytes.LowPart;
}

WORD ToppyFramework::Hdd_Delete(char *name )
{
	return ::DeleteFile(MakePath(name)); //TODO: correct return value?
}

bool ToppyFramework::Hdd_Exist(char *name )
{
	return (_access(MakePath(name), 0) == 0);
}

DWORD ToppyFramework::Hdd_Create(char *name, BYTE attr )
{
	CString sName(name);
	if ((sName.Find("/")!=-1) || (sName.Find("\\")!=-1))
	{
		LogError("Tried to create a file with a path in the name - %s", name);
        return 0;
	}

	if (attr == ATTR_FOLDER)
	{
		return (::CreateDirectory(MakePath(name), 0) == 0) ? 1 : 0;
	}
	//TODO: other attributes
	else
	{
		FILE* file = fopen(MakePath(name), "wb");
		if (file == 0)
			return 1;
		fclose(file);
		return 0; 
	}
}

WORD ToppyFramework::Hdd_ChangeDir(char *dir )
{
	CString sChange = dir;
	if (sChange == ".")
		return 1;

	if (sChange.ReverseFind(L'/') == sChange.GetLength() -1)
	{
		return 0; // toppy doesn't like stuff ending with /
	}
	else
	{
		if (sChange.ReverseFind(L'.') == sChange.GetLength() - 1)
			sChange += "/"; // however this routine likes it so we put one on to make the code below simpler
	}


	while (sChange.Left(3) == "../")
	{
		if (m_sCurrentFolder == "/")
			return 1;

		int iTrim = m_sCurrentFolder.ReverseFind('/');
		if (iTrim >= 0)
			m_sCurrentFolder = m_sCurrentFolder.Left(iTrim);

		sChange = sChange.Mid(3);		
	}

	if (sChange.IsEmpty())
		return 1;

	word result = 1;

	CString sNewDir = MakePath(sChange);
	if (_access(sNewDir, 00) != 0)
	{
		result = 0;
		while (true)
		{
			int iTrim = sChange.ReverseFind('/');
			sChange = sChange.Left(iTrim);
			if (sChange.IsEmpty())
				break;

			sNewDir = MakePath(sChange);
			if (_access(sNewDir, 00) == 0)
				break;
		}
	}

	if (!sChange.IsEmpty())
		m_sCurrentFolder += "/";
	m_sCurrentFolder += sChange;
	return result;
}


int ToppyFramework::Hdd_PlayTs(char *name )
{
	return m_TheState.PlayTs(MakePath(name));
}

int ToppyFramework::Hdd_StopTs(void)
{
	return m_TheState.StopTs();
}

int ToppyFramework::Hdd_PlayMp3(char *name )
{
	return m_TheState.PlayMp3(MakePath(name));
}

int ToppyFramework::Hdd_StopMp3(void )
{
	return m_TheState.StopMp3();
}

bool ToppyFramework::Hdd_Rename(  	char *oldName, char *newName )
{
	return ::rename(MakePath(oldName), MakePath(newName))!= 0;
}

bool ToppyFramework::Hdd_StartRecord(void )
{
	return m_TheState.StartRecord(MakeRootPath("DataFiles"));
}

bool ToppyFramework::Hdd_StopRecord(BYTE recSlot )
{
	return m_TheState.StopRecord(recSlot);
}

bool ToppyFramework::Hdd_GetRecInfo(BYTE recSlot, TYPE_RecInfo *recInfo )
{
	return m_TheState.GetRecInfo(recSlot, recInfo);
}

bool ToppyFramework::Hdd_GetPlayInfo(TYPE_PlayInfo *playInfo )
{
	return m_TheState.GetPlayInfo(playInfo);
}

bool ToppyFramework::Hdd_GetHddID(char *modelNo, char *serialNo)
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}


bool ToppyFramework::Hdd_ChangePlaybackPos(DWORD blockPos )
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

bool ToppyFramework::Hdd_GotoBookmark(void )
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

bool ToppyFramework::Hdd_SetBookmark(void )
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

bool ToppyFramework::Hdd_Move( char *from_dir, char *to_dir, char *filename )
{
	CString from = MakeRelativePath(from_dir) + "/" + filename;
	CString to = MakeRelativePath(to_dir) + "/" + filename;
	::MoveFile( from, to );
	return 0;
}


// -- STRING FUNCTION ---------------------
int ToppyFramework::Osd_PutS(WORD rgn, DWORD x, DWORD y, DWORD maxX, const char * str, WORD fcolor, WORD bcolor, BYTE fntType, BYTE fntSize, BYTE bDot, BYTE align)
{
	return m_OSDregions.Find(rgn)->Osd_PutS(x,y,maxX, str, fcolor, bcolor, fntType, fntSize, bDot, align);
}

int ToppyFramework::Osd_GetW(const char *str, BYTE fntType, BYTE fntSize )
{
	return m_OSDregions.GetW(str, fntType, fntSize);
}

int ToppyFramework::Osd_PutString(WORD rgn, DWORD x, DWORD y, DWORD maxX, const char * str, WORD fcolor, WORD bcolor, BYTE fntType, BYTE fntSize, BYTE nextLine)
{
	return m_OSDregions.Find(rgn)->Osd_PutString(x,y,maxX, str, fcolor, bcolor, fntType, fntSize, nextLine);
}

int ToppyFramework::Osd_DrawString(const char * str, DWORD dstWidth, WORD color, BYTE * dest, DWORD maxWidth, BYTE fntType, BYTE fntSize)
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

int ToppyFramework::Osd_PutStringAf(WORD rgn, DWORD x, DWORD y, DWORD maxX, const char * str, WORD fcolor, WORD bcolor, BYTE fntType, BYTE fntSize, BYTE nextLine)
{
	return m_OSDregions.Find(rgn)->Osd_PutStringAf(x,y,maxX, str, fcolor, bcolor, fntType, fntSize, nextLine);
}


int ToppyFramework::Channel_GetTotalNum(int *nTvSvc, int *nRadioSvc )
{
	return m_ChannelList.GetTotalNum(nTvSvc, nRadioSvc);
}

int ToppyFramework::Channel_GetFirstInfo(int svcType, TYPE_TapChInfo *chInfo )
{
	return m_ChannelList.GetFirstInfo(svcType, chInfo);
}

int ToppyFramework::Channel_GetNextInfo(TYPE_TapChInfo *chInfo )
{
	return m_ChannelList.GetNextInfo(chInfo);
}

int ToppyFramework::Channel_GetInfo(int svcType, int svcNum, TYPE_TapChInfo *chInfo )
{
	return m_ChannelList.GetInfo(svcType, svcNum, chInfo);
}

int ToppyFramework::Channel_GetCurrent(int *tvRadio, int *svcNum )
{
	return m_ChannelList.GetCurrent(tvRadio, svcNum);
}

int ToppyFramework::Channel_GetCurrentSubTV(int *svcNum )
{
	return m_ChannelList.GetCurrentSubTV(svcNum);
}

int ToppyFramework::Channel_Start(int mainSub, int svcType, int chNum )
{
	return m_ChannelList.Start(mainSub, svcType, chNum);
}

int ToppyFramework::Channel_Scale(int mainSub, long x, long y, long w, long h, bool anim )
{
	return m_ChannelList.Scale(mainSub, x, y, w, h, anim);
}

int ToppyFramework::Channel_IsPlayable(int mainSub, int svcType, int chNum )
{
	return m_ChannelList.IsPlayable(mainSub,svcType, chNum);
}

int ToppyFramework::Channel_Move(int mainSub, int dir )
{
	return m_ChannelList.Move(mainSub, dir);
}

int ToppyFramework::Channel_Stop(int mainSub )
{
	return m_ChannelList.Stop(mainSub);
}

int ToppyFramework::Channel_GetTotalAudioTrack(void )
{
	return m_ChannelList.GetTotalAudioTrack();
}

char* ToppyFramework::Channel_GetAudioTrackName(int trkNum, char *trkName )
{
	return m_ChannelList.GetAudioTrackName(trkNum, trkName);
}

int ToppyFramework::Channel_SetAudioTrack(int trkNum )
{
	return m_ChannelList.SetAudioTrack(trkNum);
}

int ToppyFramework::Channel_GetTotalSubtitleTrack(void )
{
	return m_ChannelList.GetTotalSubtitleTrack();
}

char* ToppyFramework::Channel_GetSubtitleTrackName(int trkNum, char *trkName )
{
	return m_ChannelList.GetSubtitleTrackName(trkNum, trkName);
}

int ToppyFramework::Channel_SetSubtitleTrack(int trkNum )
{
	return m_ChannelList.SetSubtitleTrack(trkNum);
}

bool ToppyFramework::Channel_IsStarted(int mainSub )
{
	return m_ChannelList.IsStarted(mainSub);
}

// --- EPG FUNCTIONS ---------------------------------------------------

TYPE_TapEvent* ToppyFramework::GetEvent(int svcType, int svcNum, int *eventNum )
{
	return m_EPGImpl.GetEvent(svcType, svcNum, eventNum);
}

TYPE_TapEvent* ToppyFramework::GetCurrentEvent(int svcType, int svcNum )
{
	return m_EPGImpl.GetCurrentEvent(svcType, svcNum);
}

bool ToppyFramework::PutEvent(BYTE *eitData )
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

WORD ToppyFramework::ControlEit(bool enable )
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

bool ToppyFramework::EPG_UpdateEvent( int svcType, int svcNum, DWORD evtid, DWORD runningStatus )
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

bool ToppyFramework::EPG_DeleteEvent( int svcType, int svcNum, DWORD evtid )
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

BYTE* ToppyFramework::EPG_GetExtInfo(TYPE_TapEvent *tapEvtInfo )
{
	return m_EPGImpl.GetExtInfo(tapEvtInfo);
}


// --- USB FUNCTIONS ---------------------------------------------------

void ToppyFramework::Usb_Read(BYTE *pBuff, WORD size, VoidWordFuncPtr pFunc )
{
	// USB, not for implementation yet
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return;
}

void ToppyFramework::Usb_Write(BYTE *pBuff, WORD size, VoidWordFuncPtr pFunc )
{
	// USB, not for implementation yet
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return;
}

void ToppyFramework::Usb_PacketRead(BYTE *pBuff, WORD size, WordWordFuncPtr pFunc )
{
	// USB, not for implementation yet
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return;
}

void ToppyFramework::Usb_PacketWrite(BYTE *pBuff, WORD size, VoidWordFuncPtr pFunc , DWORD cmd )
{
	// USB, not for implementation yet
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return;
}

void ToppyFramework::Usb_Cancel(void)
{
	// USB, not for implementation yet
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return;
}


// --- TIMER FUNCTIONS ---------------------------------------------------

int ToppyFramework::Timer_GetTotalNum(void )
{
	return m_Timers.GetTotalNum();
}

bool ToppyFramework::Timer_GetInfo(int entryNum, TYPE_TimerInfo *info )
{
	return m_Timers.GetInfo(entryNum, info);
}

int ToppyFramework::Timer_Add(TYPE_TimerInfo *info )
{
	return m_Timers.Add(info);
}

int ToppyFramework::Timer_Modify(int entryNum, TYPE_TimerInfo *info )
{
	return m_Timers.Modify(entryNum, info);
}

bool ToppyFramework::Timer_Delete(int entryNum )
{
	return m_Timers.Delete(entryNum);
}


// --- VFD FUNCTIONS ---------------------------------------------------

int ToppyFramework::Vfd_GetStatus(void )
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return 0;
}

void ToppyFramework::Vfd_Control(bool underTapCtrl )
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return;
}

void ToppyFramework::Vfd_SendData(BYTE *data, BYTE dataLen )
{
	LogUnimplemented(__FUNCTION__ "(" __FUNCSIG__ ")");
	return;
}

int ToppyFramework::CountFilesInCurrentFolder()
{
	//TODO: things missing in root folder?
	int iCount = 0;
	WIN32_FIND_DATA findData;

	HANDLE hFind = FindFirstFile(MakePath("*.*"), &findData);
	if (hFind == 0)
		return 0;
	iCount++;
	while (FindNextFile(hFind, &findData))
	{
		iCount++;
	}
	FindClose(hFind);
	return iCount;
}

void ToppyFramework::RaiseEventToFirmware(unsigned short event, unsigned long param1, unsigned long param2 )
{
	m_TheState.Event(event, param1, param2);
}

bool ToppyFramework::IsTapExited()
{
	return m_bExitFlag;
}
