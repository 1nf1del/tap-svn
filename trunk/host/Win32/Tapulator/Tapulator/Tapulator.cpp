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

// Tapulator.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Tapulator.h"
#include "MainFrm.h"
#include ".\tapulator.h"
#include "Heap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const TCHAR _afxFileSection[] = _T("Recent File List");
const TCHAR _afxFileEntry[] = _T("File%d");

// CTapulatorApp

BEGIN_MESSAGE_MAP(CTapulatorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1, OnUpdateRecentFileMenu)
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)
END_MESSAGE_MAP()


// CTapulatorApp construction

CTapulatorApp::CTapulatorApp()
{
}

CTapulatorApp::~CTapulatorApp()
{
	Heap::DoneWithHeap();
}

// The one and only CTapulatorApp object

CTapulatorApp theApp;

// CTapulatorApp initialization

BOOL CTapulatorApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	m_pRecentFileList = new CRecentFileList(0, _afxFileSection, _afxFileEntry, 10 );
	m_pRecentFileList->ReadList();

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object

	if (!AcceptsLicense())
		return FALSE;

	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);
	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}

void CTapulatorApp::OnUpdateRecentFileMenu(CCmdUI* pCmdUI)
{
	ASSERT_VALID(this);
	if (m_pRecentFileList == NULL) // no MRU files
		pCmdUI->Enable(FALSE);
	else
		m_pRecentFileList->UpdateMenu(pCmdUI);
}


BOOL CTapulatorApp::OnOpenRecentFile(UINT nID)
{
	int nIndex = nID - ID_FILE_MRU_FILE1;

	if ( ((CMainFrame*)m_pMainWnd)->LoadTap((*m_pRecentFileList)[nIndex]) )
		AddToRecentFileList( (*m_pRecentFileList)[nIndex] );
	else
		m_pRecentFileList->Remove(nIndex);

	return TRUE;
}

// CTapulatorApp message handlers



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CTapulatorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CTapulatorApp message handlers


void CTapulatorApp::OnFileOpen()
{
	CFileDialog dlf(TRUE, "dll", 0, 0, "TAP modules (*.dll)|*.dll||", m_pMainWnd, 0);
	if (dlf.DoModal() != IDOK)
		return;

	if ( ((CMainFrame*)m_pMainWnd)->LoadTap(dlf.GetFileName()) )
		AddToRecentFileList( dlf.GetFileName() );
}

bool CTapulatorApp::AcceptsLicense()
{
	if (GetProfileInt("General", "LicenseAccepted", 0) == 1)
		return true;

	if (AfxMessageBox("Please be aware that this program and libtapulator.lib are licensed under the GNU General Pulic License. It is a license violation to link this library with code that is licensed in a manner incompatible with the GPL.\n\nThis specifically prohibits you from using this emulator to develop a TAP for which you do not make the source code freely available.\n\nPlease indicate if you accept this License condition or not.",
		MB_YESNO, 0) == IDNO)
		return false;

	WriteProfileInt("General", "LicenseAccepted", 1);

	return true;
}
