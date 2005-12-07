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
#include "afxcmn.h"
#include "afxwin.h"


// CInfoDialog dialog

class CInfoDialog : public CDialog
{
	DECLARE_DYNAMIC(CInfoDialog)

public:
	CInfoDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInfoDialog();

// Dialog Data
	enum { IDD = IDD_INFODIALOG };

	void LogInfo(CString sText);
	void UpdateHeapInfo();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//	CRichEditCtrl m_LogCtrl;
	CEdit m_LogCtrl;
	afx_msg void OnEnChangeEdit1();
	CString m_sBytes;
	CString m_sMaxBytes;
	CString m_sTotalAllocs;
	CString m_sOpenAllocs;
	CString m_sLargestAlloc;
};
