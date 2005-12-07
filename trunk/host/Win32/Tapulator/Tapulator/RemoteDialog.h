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


// CRemoteDialog dialog

class CRemoteDialog : public CDialogBar
{
	DECLARE_DYNAMIC(CRemoteDialog)

public:
	CRemoteDialog();   // standard constructor
	virtual ~CRemoteDialog();

// Dialog Data
	enum { IDD = IDD_REMOTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBpower();
	afx_msg void OnBnClickedBmute();
	afx_msg void OnBnClickedRadio();
	afx_msg void OnBnClickedSource();
	afx_msg void OnBnClickedOpt();
	afx_msg void OnBnClickedNum1();
	afx_msg void OnBnClickedNum2();
	afx_msg void OnBnClickedNum3();
	afx_msg void OnBnClickedNum4();
	afx_msg void OnBnClickedNum5();
	afx_msg void OnBnClickedNum6();
	afx_msg void OnBnClickedNum7();
	afx_msg void OnBnClickedNum8();
	afx_msg void OnBnClickedNum9();
	afx_msg void OnBnClickedRecall();
	afx_msg void OnBnClickedNum0();
	afx_msg void OnBnClickedInfo();
	afx_msg void OnBnClickedGuide();
	afx_msg void OnBnClickedList();
	afx_msg void OnBnClickedUp();
	afx_msg void OnBnClickedMenu();
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedOklist();
	afx_msg void OnBnClickedLeft();
	afx_msg void OnBnClickedRight();
	afx_msg void OnBnClickedVolup();
	afx_msg void OnBnClickedVoldown();
	afx_msg void OnBnClickedDown();
	afx_msg void OnBnClickedProgdown();
	afx_msg void OnBnClickedProgup();
	afx_msg void OnBnClickedRewind();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedForward();
	afx_msg void OnBnClickedSlow();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedRecord();
	afx_msg void OnBnClickedPause();
	afx_msg void OnBnClickedPip();
	afx_msg void OnBnClickedArchive();
	afx_msg void OnBnClickedText();
	afx_msg void OnBnClickedRed();
	afx_msg void OnBnClickedGreen();
	afx_msg void OnBnClickedYellow();
	afx_msg void OnBnClickedBlue();
	afx_msg void OnBnClickedPipswap();
	afx_msg void OnBnClickedWhite();
void EnableButton(CCmdUI* p);

private :
	void RaiseKeyEvent(int keyCode, int hardwareKeyCode);

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
