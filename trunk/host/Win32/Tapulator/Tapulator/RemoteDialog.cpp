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
#include "MoreKeys.h"

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
END_MESSAGE_MAP()


void CRemoteDialog::RaiseKeyEvent(int keyCode, int hardwareKeyCode)
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	pFrame->GetTapModule()->RaiseKeyEvent(keyCode, hardwareKeyCode);
	pFrame->SetFocus();

}
// CRemoteDialog message handlers

void CRemoteDialog::OnBnClickedBpower()
{
	RaiseKeyEvent(RKEY_Power, RAWKEY_Power);
}


void CRemoteDialog::OnBnClickedBmute()
{
	RaiseKeyEvent(RKEY_Mute, RAWKEY_Mute);
}

void CRemoteDialog::OnBnClickedRadio()
{
	RaiseKeyEvent(RKEY_TvRadio, RAWKEY_TVRadio_Uhf);
}


void CRemoteDialog::OnBnClickedSource()
{
	RaiseKeyEvent(RKEY_TvSat, RAWKEY_TvSat); 
}


void CRemoteDialog::OnBnClickedOpt()
{
	RaiseKeyEvent(RKEY_Sleep, RAWKEY_Opt_Sleep); 
}


void CRemoteDialog::OnBnClickedNum1()
{
	RaiseKeyEvent(RKEY_1, RAWKEY_1);
}


void CRemoteDialog::OnBnClickedNum2()
{
	RaiseKeyEvent(RKEY_2, RAWKEY_2);
}


void CRemoteDialog::OnBnClickedNum3()
{
	RaiseKeyEvent(RKEY_3, RAWKEY_3);
}


void CRemoteDialog::OnBnClickedNum4()
{
	RaiseKeyEvent(RKEY_4, RAWKEY_4);
}


void CRemoteDialog::OnBnClickedNum5()
{
	RaiseKeyEvent(RKEY_5, RAWKEY_5);
}


void CRemoteDialog::OnBnClickedNum6()
{
	RaiseKeyEvent(RKEY_6,  RAWKEY_6);
}


void CRemoteDialog::OnBnClickedNum7()
{
	RaiseKeyEvent(RKEY_7,  RAWKEY_7);
}


void CRemoteDialog::OnBnClickedNum8()
{
	RaiseKeyEvent(RKEY_8,  RAWKEY_8);
}


void CRemoteDialog::OnBnClickedNum9()
{
	RaiseKeyEvent(RKEY_9,  RAWKEY_9);
}


void CRemoteDialog::OnBnClickedRecall()
{
	RaiseKeyEvent(RKEY_Recall, RAWKEY_Recall);
}


void CRemoteDialog::OnBnClickedNum0()
{
	RaiseKeyEvent(RKEY_0, RAWKEY_0);
}


void CRemoteDialog::OnBnClickedInfo()
{
	RaiseKeyEvent(RKEY_Info, RAWKEY_Info);
}


void CRemoteDialog::OnBnClickedGuide()
{
	RaiseKeyEvent(RKEY_Guide,  RAWKEY_Guide);
}


void CRemoteDialog::OnBnClickedList()
{
	RaiseKeyEvent(RKEY_TvRadio,  RAWKEY_List_TVRad); //??
}


void CRemoteDialog::OnBnClickedUp()
{
	RaiseKeyEvent(RKEY_ChUp,  RAWKEY_Up);
}


void CRemoteDialog::OnBnClickedMenu()
{
	RaiseKeyEvent(RKEY_Menu, RAWKEY_Menu);
}


void CRemoteDialog::OnBnClickedExit()
{
	RaiseKeyEvent(RKEY_Exit, RAWKEY_Exit_AudioTrk);
}


void CRemoteDialog::OnBnClickedOklist()
{
	RaiseKeyEvent(RKEY_Ok,  RAWKEY_Ok);
}


void CRemoteDialog::OnBnClickedLeft()
{
	RaiseKeyEvent(RKEY_VolDown,  RAWKEY_Left);
}


void CRemoteDialog::OnBnClickedRight()
{
	RaiseKeyEvent(RKEY_VolUp,  RAWKEY_Right);
}


void CRemoteDialog::OnBnClickedVolup()
{
	RaiseKeyEvent(RKEY_VolUp, RAWKEY_VolUp_Fav);
}


void CRemoteDialog::OnBnClickedVoldown()
{
	RaiseKeyEvent(RKEY_VolDown,  RAWKEY_VolDown_Exit);
}


void CRemoteDialog::OnBnClickedDown()
{
	RaiseKeyEvent(RKEY_ChDown, RAWKEY_Down);
}


void CRemoteDialog::OnBnClickedProgdown()
{
	RaiseKeyEvent(RKEY_ChDown, RAWKEY_ChDown_Teletext);
}


void CRemoteDialog::OnBnClickedProgup()
{
	RaiseKeyEvent(RKEY_ChUp, RAWKEY_ChUp_Subt );
}


void CRemoteDialog::OnBnClickedRewind()
{
	RaiseKeyEvent(RKEY_Rewind, RAWKEY_Rewind);
}


void CRemoteDialog::OnBnClickedPlay()
{
	RaiseKeyEvent(RKEY_Play, RAWKEY_Play);
}


void CRemoteDialog::OnBnClickedForward()
{
	RaiseKeyEvent(RKEY_Forward, RAWKEY_Forward);
}


void CRemoteDialog::OnBnClickedSlow()
{
	RaiseKeyEvent(RKEY_Slow, RAWKEY_Slow);
}


void CRemoteDialog::OnBnClickedStop()
{
	RaiseKeyEvent(RKEY_Stop, RAWKEY_Stop);
}


void CRemoteDialog::OnBnClickedRecord()
{
	RaiseKeyEvent(RKEY_Record, RAWKEY_Record);
}


void CRemoteDialog::OnBnClickedPause()
{
	RaiseKeyEvent(RKEY_Pause, RAWKEY_Pause);
}


void CRemoteDialog::OnBnClickedPip()
{
	RaiseKeyEvent(RKEY_Prev, RAWKEY_PIP_Prev); // really pip it seems
}


void CRemoteDialog::OnBnClickedArchive()
{
	RaiseKeyEvent(RKEY_PlayList, RAWKEY_PlayList);
}


void CRemoteDialog::OnBnClickedText()
{
	RaiseKeyEvent(RKEY_Next, RAWKEY_Teletext_Next); // yes,yes, but that's what the text key sends 
}


void CRemoteDialog::OnBnClickedRed()
{
	RaiseKeyEvent(RKEY_Red, RAWKEY_Red);
}


void CRemoteDialog::OnBnClickedGreen()
{
	RaiseKeyEvent(RKEY_Green, RAWKEY_Green);
}


void CRemoteDialog::OnBnClickedYellow()
{
	RaiseKeyEvent(RKEY_Yellow, RAWKEY_Yellow);
}


void CRemoteDialog::OnBnClickedBlue()
{
	RaiseKeyEvent(RKEY_Blue, RAWKEY_Blue);
}


void CRemoteDialog::OnBnClickedPipswap()
{
	RaiseKeyEvent(RKEY_Sat, RAWKEY_PIPSwap_Sat); // yep
}


void CRemoteDialog::OnBnClickedWhite()
{
	RaiseKeyEvent(RKEY_Ab, RAWKEY_White_Ab); // Yep
}

