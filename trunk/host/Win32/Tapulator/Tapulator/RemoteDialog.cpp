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

// RemoteDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Tapulator.h"
#include "RemoteDialog.h"
#include ".\remotedialog.h"
#include "Mainfrm.h"

#define RKEY_Red        0x1003f
#define RKEY_Green      0x10024
#define RKEY_Yellow     0x10025
#define RKEY_Blue       0x10026

// CRemoteDialog dialog

IMPLEMENT_DYNAMIC(CRemoteDialog, CDialogBar)

CRemoteDialog::CRemoteDialog()
	: CDialogBar()
{
}

CRemoteDialog::~CRemoteDialog()
{
}


void CRemoteDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
}

void CRemoteDialog::EnableButton(CCmdUI* p)
{
	p->Enable();
}

BEGIN_MESSAGE_MAP(CRemoteDialog, CDialogBar)
	ON_UPDATE_COMMAND_UI_RANGE(0, 65535, EnableButton)
	ON_BN_CLICKED(IDC_BPOWER, OnBnClickedBpower)
	ON_BN_CLICKED(IDC_BMUTE, OnBnClickedBmute)
	ON_BN_CLICKED(IDC_RADIO, OnBnClickedRadio)
	ON_BN_CLICKED(IDC_SOURCE, OnBnClickedSource)
	ON_BN_CLICKED(IDC_OPT, OnBnClickedOpt)
	ON_BN_CLICKED(IDC_NUM1, OnBnClickedNum1)
	ON_BN_CLICKED(IDC_NUM2, OnBnClickedNum2)
	ON_BN_CLICKED(IDC_NUM3, OnBnClickedNum3)
	ON_BN_CLICKED(IDC_NUM4, OnBnClickedNum4)
	ON_BN_CLICKED(IDC_NUM5, OnBnClickedNum5)
	ON_BN_CLICKED(IDC_NUM6, OnBnClickedNum6)
	ON_BN_CLICKED(IDC_NUM7, OnBnClickedNum7)
	ON_BN_CLICKED(IDC_NUM8, OnBnClickedNum8)
	ON_BN_CLICKED(IDC_NUM9, OnBnClickedNum9)
	ON_BN_CLICKED(IDC_RECALL, OnBnClickedRecall)
	ON_BN_CLICKED(IDC_NUM0, OnBnClickedNum0)
	ON_BN_CLICKED(IDC_INFO, OnBnClickedInfo)
	ON_BN_CLICKED(IDC_GUIDE, OnBnClickedGuide)
	ON_BN_CLICKED(IDC_LIST, OnBnClickedList)
	ON_BN_CLICKED(IDC_UP, OnBnClickedUp)
	ON_BN_CLICKED(IDC_MENU, OnBnClickedMenu)
	ON_BN_CLICKED(IDC_EXIT, OnBnClickedExit)
	ON_BN_CLICKED(IDC_OKLIST, OnBnClickedOklist)
	ON_BN_CLICKED(IDC_LEFT, OnBnClickedLeft)
	ON_BN_CLICKED(IDC_RIGHT, OnBnClickedRight)
	ON_BN_CLICKED(IDC_VOLUP, OnBnClickedVolup)
	ON_BN_CLICKED(IDC_VOLDOWN, OnBnClickedVoldown)
	ON_BN_CLICKED(IDC_DOWN, OnBnClickedDown)
	ON_BN_CLICKED(IDC_PROGDOWN, OnBnClickedProgdown)
	ON_BN_CLICKED(IDC_PROGUP, OnBnClickedProgup)
	ON_BN_CLICKED(IDC_REWIND, OnBnClickedRewind)
	ON_BN_CLICKED(IDC_PLAY, OnBnClickedPlay)
	ON_BN_CLICKED(IDC_FORWARD, OnBnClickedForward)
	ON_BN_CLICKED(IDC_SLOW, OnBnClickedSlow)
	ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
	ON_BN_CLICKED(IDC_RECORD, OnBnClickedRecord)
	ON_BN_CLICKED(IDC_PAUSE, OnBnClickedPause)
	ON_BN_CLICKED(IDC_PIP, OnBnClickedPip)
	ON_BN_CLICKED(IDC_ARCHIVE, OnBnClickedArchive)
	ON_BN_CLICKED(IDC_TEXT, OnBnClickedText)
	ON_BN_CLICKED(IDC_RED, OnBnClickedRed)
	ON_BN_CLICKED(IDC_GREEN, OnBnClickedGreen)
	ON_BN_CLICKED(IDC_YELLOW, OnBnClickedYellow)
	ON_BN_CLICKED(IDC_BLUE, OnBnClickedBlue)
	ON_BN_CLICKED(IDC_PIPSWAP, OnBnClickedPipswap)
	ON_BN_CLICKED(IDC_WHITE, OnBnClickedWhite)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


void CRemoteDialog::RaiseKeyEvent(int keyCode, int hardwareKeyCode)
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	pFrame->GetTapModule()->RaiseKeyEvent(keyCode, hardwareKeyCode == 0 ? keyCode : hardwareKeyCode);

}
// CRemoteDialog message handlers

void CRemoteDialog::OnBnClickedBpower()
{
	RaiseKeyEvent(RKEY_Power,0x1f0);
}


void CRemoteDialog::OnBnClickedBmute()
{
	RaiseKeyEvent(RKEY_Mute, 0x10c);
}

void CRemoteDialog::OnBnClickedRadio()
{
	RaiseKeyEvent(RKEY_TvRadio,0x143);
}


void CRemoteDialog::OnBnClickedSource()
{
	RaiseKeyEvent(RKEY_TvSat,0x108); 
}


void CRemoteDialog::OnBnClickedOpt()
{
	RaiseKeyEvent(RKEY_Sleep,0x144); 
}


void CRemoteDialog::OnBnClickedNum1()
{
	RaiseKeyEvent(RKEY_Cmd_1,0x111);
}


void CRemoteDialog::OnBnClickedNum2()
{
	RaiseKeyEvent(RKEY_Cmd_2,0x112);
}


void CRemoteDialog::OnBnClickedNum3()
{
	RaiseKeyEvent(RKEY_Cmd_3,0x113);
}


void CRemoteDialog::OnBnClickedNum4()
{
	RaiseKeyEvent(RKEY_Cmd_4,0x114);
}


void CRemoteDialog::OnBnClickedNum5()
{
	RaiseKeyEvent(RKEY_Cmd_5,0x115);
}


void CRemoteDialog::OnBnClickedNum6()
{
	RaiseKeyEvent(RKEY_Cmd_6,0x116);
}


void CRemoteDialog::OnBnClickedNum7()
{
	RaiseKeyEvent(RKEY_Cmd_7,0x117);
}


void CRemoteDialog::OnBnClickedNum8()
{
	RaiseKeyEvent(RKEY_Cmd_8,0x118);
}


void CRemoteDialog::OnBnClickedNum9()
{
	RaiseKeyEvent(RKEY_Cmd_9,0x119);
}


void CRemoteDialog::OnBnClickedRecall()
{
	RaiseKeyEvent(RKEY_Recall,0x11e);
}


void CRemoteDialog::OnBnClickedNum0()
{
	RaiseKeyEvent(RKEY_Cmd_0,0x110);
}


void CRemoteDialog::OnBnClickedInfo()
{
	RaiseKeyEvent(RKEY_Info,0x11d);
}


void CRemoteDialog::OnBnClickedGuide()
{
	RaiseKeyEvent(RKEY_Guide,0x11b);
}


void CRemoteDialog::OnBnClickedList()
{
	RaiseKeyEvent(RKEY_Ok,0x104); //??
}


void CRemoteDialog::OnBnClickedUp()
{
	RaiseKeyEvent(RKEY_ChUp,0x100);
}


void CRemoteDialog::OnBnClickedMenu()
{
	RaiseKeyEvent(RKEY_Menu,0x11a);
}


void CRemoteDialog::OnBnClickedExit()
{
	RaiseKeyEvent(RKEY_Exit,0x105);
}


void CRemoteDialog::OnBnClickedOklist()
{
	RaiseKeyEvent(RKEY_Ok,0x11f);
}


void CRemoteDialog::OnBnClickedLeft()
{
	RaiseKeyEvent(RKEY_VolDown,0x103);
}


void CRemoteDialog::OnBnClickedRight()
{
	RaiseKeyEvent(RKEY_VolUp,0x102);
}


void CRemoteDialog::OnBnClickedVolup()
{
	RaiseKeyEvent(RKEY_VolUp,0x109);
}


void CRemoteDialog::OnBnClickedVoldown()
{
	RaiseKeyEvent(RKEY_VolDown,0x11c);
}


void CRemoteDialog::OnBnClickedDown()
{
	RaiseKeyEvent(RKEY_ChDown,0x101);
}


void CRemoteDialog::OnBnClickedProgdown()
{
	RaiseKeyEvent(RKEY_ChDown,0x147);
}


void CRemoteDialog::OnBnClickedProgup()
{
	RaiseKeyEvent(RKEY_ChUp,107);
}


void CRemoteDialog::OnBnClickedRewind()
{
	RaiseKeyEvent(RKEY_Rewind,0x145);
}


void CRemoteDialog::OnBnClickedPlay()
{
	RaiseKeyEvent(RKEY_Play,0x146);
}


void CRemoteDialog::OnBnClickedForward()
{
	RaiseKeyEvent(RKEY_Forward,0x148);
}


void CRemoteDialog::OnBnClickedSlow()
{
	RaiseKeyEvent(RKEY_Slow,0x149);
}


void CRemoteDialog::OnBnClickedStop()
{
	RaiseKeyEvent(RKEY_Stop,0x14a);
}


void CRemoteDialog::OnBnClickedRecord()
{
	RaiseKeyEvent(RKEY_Record,0x14b);
}


void CRemoteDialog::OnBnClickedPause()
{
	RaiseKeyEvent(RKEY_Pause,0x106);
}


void CRemoteDialog::OnBnClickedPip()
{
	RaiseKeyEvent(RKEY_Prev,0x150); // really pip it seems
}


void CRemoteDialog::OnBnClickedArchive()
{
	RaiseKeyEvent(RKEY_PlayList,0x151);
}


void CRemoteDialog::OnBnClickedText()
{
	RaiseKeyEvent(RKEY_Next,0x152); // yes,yes, but that's what the text key sends 
}


void CRemoteDialog::OnBnClickedRed()
{
	RaiseKeyEvent(RKEY_Red,0x14d);
}


void CRemoteDialog::OnBnClickedGreen()
{
	RaiseKeyEvent(RKEY_Green,0x10d);
}


void CRemoteDialog::OnBnClickedYellow()
{
	RaiseKeyEvent(RKEY_Yellow,0x10e);
}


void CRemoteDialog::OnBnClickedBlue()
{
	RaiseKeyEvent(RKEY_Blue,0x10f);
}


void CRemoteDialog::OnBnClickedPipswap()
{
	RaiseKeyEvent(RKEY_Sat,0x15e); // yep
}


void CRemoteDialog::OnBnClickedWhite()
{
	RaiseKeyEvent(RKEY_Ab,0x14c); // Yep
}


void CRemoteDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	CDialogBar::OnKeyDown(nChar, nRepCnt, nFlags);
}
