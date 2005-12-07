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

// ConfigurationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Tapulator.h"
#include "ConfigurationDialog.h"
#include ".\configurationdialog.h"
#include "Connfiguration.H"
#include "InfoLogger.h"

// CConfigurationDialog dialog

IMPLEMENT_DYNAMIC(CConfigurationDialog, CDialog)
CConfigurationDialog::CConfigurationDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigurationDialog::IDD, pParent)
	, m_bLogCritical(false)
	, m_bLogError(false)
	, m_bLogWarning(false)
	, m_bLogUnimpl(false)
	, m_bLogInfo(false)
	, m_bLogVerbose(false)
	, m_bLogUser(false)
	, m_iStartupFolder(0)
{
	m_pConfig = new CConfiguration();
}

CConfigurationDialog::~CConfigurationDialog()
{
	delete m_pConfig;
}

void CConfigurationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FS, m_FSPathEdit);
	DDX_Control(pDX, IDC_EDIT_EPG, m_EPGEdit);
	DDX_Check(pDX, IDC_CHECK1, m_bLogCritical);
	DDX_Check(pDX, IDC_CHECK2, m_bLogError);
	DDX_Check(pDX, IDC_CHECK3, m_bLogWarning);
	DDX_Check(pDX, IDC_CHECK4, m_bLogUnimpl);
	DDX_Check(pDX, IDC_CHECK5, m_bLogInfo);
	DDX_Check(pDX, IDC_CHECK6, m_bLogVerbose);
	DDX_Check(pDX, IDC_CHECK7, m_bLogUser);
	DDX_Radio(pDX, IDC_RADIO1, m_iStartupFolder);
}


BEGIN_MESSAGE_MAP(CConfigurationDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_FS, OnBnClickedButtonBrowseFs)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_EPG, OnBnClickedButtonBrowseEpg)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CConfigurationDialog message handlers

#pragma warning (disable : 4278)
#import "c:\windows\system32\shell32.dll"

void CConfigurationDialog::OnBnClickedButtonBrowseFs()
{
	CoInitialize(NULL);
	Shell32::IShellDispatch4Ptr pShell("Shell.Application");
	Shell32::Folder3Ptr pFolder = pShell->BrowseForFolder((long)this->m_hWnd, _bstr_t("Choose Root Folder"),
		BIF_NEWDIALOGSTYLE| BIF_NONEWFOLDERBUTTON | BIF_EDITBOX | BIF_RETURNONLYFSDIRS);

	if (pFolder == NULL)
		return; // cancelled
	CString csFolder = pFolder->Self->Path;
	m_FSPathEdit.SetWindowText(csFolder);
	CoUninitialize();
}

void CConfigurationDialog::OnBnClickedButtonBrowseEpg()
{
	CString cs;
	m_EPGEdit.GetWindowText(cs);
	CFileDialog fd(true, ".DAT", cs, OFN_HIDEREADONLY, "EPG Data Files (*.dat)|*.dat|All Files (*.*)|*.*||", this, 0);
	if (fd.DoModal() == IDOK)
	{
		m_EPGEdit.SetWindowText(fd.GetPathName());
	}
}

void CConfigurationDialog::OnBnClickedButtonApply()
{
	UpdateData(true);
	SaveData();
	// TODO: Add your control notification handler code here
}

void CConfigurationDialog::OnBnClickedOk()
{
	UpdateData(true);
	SaveData();
	// TODO: Add your control notification handler code here
	OnOK();
}

void CConfigurationDialog::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

int CConfigurationDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	
	// TODO:  Add your specialized creation code here

	return 0;
}

BOOL CConfigurationDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_EPGEdit.SetWindowText(m_pConfig->GetEPGFile());
	m_FSPathEdit.SetWindowText(m_pConfig->GetRootFolderPath());

	DWORD dwFilter = m_pConfig->GetLoggingFilter();

	m_bLogCritical		= (dwFilter & LOG_SEVERITY_CRIT) != 0;
	m_bLogError			= (dwFilter & LOG_SEVERITY_ERROR) != 0;
	m_bLogWarning		= (dwFilter & LOG_SEVERITY_WARNING) != 0;
	m_bLogUnimpl		= (dwFilter & LOG_SEVERITY_UNIMPL) != 0;
	m_bLogInfo			= (dwFilter & LOG_SEVERITY_INFO) != 0;
	m_bLogVerbose		= (dwFilter & LOG_SEVERITY_VERBOSE) != 0;
	m_bLogUser			= (dwFilter & LOG_SEVERITY_USER) != 0;

	m_iStartupFolder = m_pConfig->GetStartInAutoStart();

	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigurationDialog::SaveData()
{
	CString cs;
	m_EPGEdit.GetWindowText(cs);
	m_pConfig->SetEPGFile(cs);

	m_FSPathEdit.GetWindowText(cs);
	m_pConfig->SetRootFolderPath(cs);

	DWORD dwFilter = 0;
	dwFilter |= m_bLogCritical ? LOG_SEVERITY_CRIT : 0;
	dwFilter |= m_bLogError ? LOG_SEVERITY_ERROR : 0;
	dwFilter |= m_bLogWarning ? LOG_SEVERITY_WARNING : 0;
	dwFilter |= m_bLogUnimpl ? LOG_SEVERITY_UNIMPL : 0;
	dwFilter |= m_bLogInfo ? LOG_SEVERITY_INFO : 0;
	dwFilter |= m_bLogVerbose ? LOG_SEVERITY_VERBOSE : 0;
	dwFilter |= m_bLogUser ? LOG_SEVERITY_USER : 0;

	m_pConfig->SetLoggingFiler(dwFilter);
	m_pConfig->SetStartInAutoStart(m_iStartupFolder);
	m_pConfig->Save();
}