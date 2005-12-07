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
#include "afxwin.h"
class CConfiguration;

// CConfigurationDialog dialog

class CConfigurationDialog : public CDialog
{
	DECLARE_DYNAMIC(CConfigurationDialog)

public:
	CConfigurationDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigurationDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void SaveData();

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_FSPathEdit;
	CEdit m_EPGEdit;
	afx_msg void OnBnClickedButtonBrowseFs();
	afx_msg void OnBnClickedButtonBrowseEpg();
	afx_msg void OnBnClickedButtonApply();
	int m_bLogCritical;
	int m_bLogError;
	int m_bLogWarning;
	int m_bLogUnimpl;
	int m_bLogInfo;
	int m_bLogVerbose;
	int m_bLogUser;

private:
	CConfiguration* m_pConfig;
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	int m_iStartupFolder;
};
