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

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Tapulator.h"
#include "Morekeys.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "ToppyFramework.h"
#include ".\mainfrm.h"
#include "RemoteDialog.h"
#include "InfoDialog.h"
#include "Connfiguration.h"
#include "ConfigurationDialog.h"
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE(WM_USER + 123, OnInitTap)
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_OPTIONS, OnFileOptions)
	ON_COMMAND(ID_VIEW_INFO, OnViewInfo)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INFO, OnUpdateViewInfo)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_pFramework = 0;
	m_pConfigData = new CConfiguration();
	m_pRemote = NULL;
}

CMainFrame::~CMainFrame()
{	
	delete m_pFramework;
	m_pFramework = NULL;
	delete m_pConfigData;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
	GetFramework();

	this->SetWindowPos(0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	CRect rcThis;
	GetWindowRect(rcThis);

	m_pRemote = new CRemoteDialog();
	m_pRemote->Create(this, IDD_REMOTE, CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_ALIGN_RIGHT, 777 );
	m_pRemote->SetWindowText("Remote");

	m_pInfoDialog = new CInfoDialog(this);
	m_pInfoDialog->Create(IDD_INFODIALOG, this);
	m_pInfoDialog->SetWindowPos(0, 0, rcThis.bottom, 0, 0,  SWP_NOZORDER | SWP_NOSIZE);
	m_pInfoDialog->ShowWindow(SW_SHOW);

	EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	m_pRemote->EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	DockControlBar(m_pRemote);

	m_pRemote->ShowWindow(SW_SHOW);

	if (__argc > 1)
	{
		CString sParam = __targv[1];
		LoadTap(sParam);
	}

	SetTimer(123, 10, NULL);

	m_wndView.SetFocus();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


BOOL CMainFrame::LoadTap(CString sFileName)
{
	if (!m_TapInfo.LoadModule(sFileName))
	{
		AfxMessageBox(m_TapInfo.GetError());
		return FALSE;
	}

	m_TapInfo.SetFramework(GetFramework());

	this->PostMessage(WM_USER+123, 0,0);
	return TRUE;
}

ToppyFramework* CMainFrame::GetFramework()
{
	if (m_pFramework == NULL)
	{
		m_pFramework = new ToppyFramework(&m_TapInfo);
	}
	return m_pFramework;
}

void CMainFrame::PaintToppy(CDC* pDC)
{
	if (m_pFramework)
		m_pFramework->Draw(pDC);

}
void CMainFrame::OnClose()
{
	m_pRemote->DestroyWindow();
	delete m_pRemote;

	m_pInfoDialog->DestroyWindow();
	delete m_pInfoDialog;

	CFrameWnd::OnClose();
}

bool CMainFrame::IsTapLoaded()
{
	return m_pFramework != NULL;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
}

void CMainFrame::OnSizing(UINT fwSide, LPRECT pRect)
{
	CFrameWnd::OnSizing(fwSide, pRect);
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: Add your message handler code here and/or call default

	CFrameWnd::OnGetMinMaxInfo(lpMMI);

	CRect rectWindow, rectClient;
	CRect toolBar;
	GetWindowRect(rectWindow);
	GetClientRect(rectClient);
	if (IsWindow(m_wndToolBar))
		m_wndToolBar.GetWindowRect(toolBar);

	lpMMI->ptMaxTrackSize.x = 920;
	lpMMI->ptMaxTrackSize.y = 620;
	lpMMI->ptMinTrackSize.x = 920;
	lpMMI->ptMinTrackSize.y = 620;
}

LRESULT CMainFrame::OnInitTap(WPARAM wParam, LPARAM lParam)
{
	int iResult = m_TapInfo.TAP_Main(); 
	return 0;
}

TapModule* CMainFrame::GetTapModule()
{
	return &m_TapInfo;
}

void CMainFrame::LogEvent(CString sMessage)
{
	m_pInfoDialog->LogInfo(sMessage);
}

void CMainFrame::UpdateStatusInfo()
{
	m_pInfoDialog->UpdateHeapInfo();
}

void CMainFrame::RefreshFramework()
{
	delete m_pFramework;
	m_pFramework = NULL;
	GetFramework();
	RedrawWindow();
}

void CMainFrame::RepaintIfNeeded()
{
	RedrawWindow();
}

void CMainFrame::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (m_TapInfo.IsInTSRMode())
	{
		m_TapInfo.TAP_Event(EVT_IDLE, 0, 0);
		RepaintIfNeeded();
	}

	CFrameWnd::OnTimer(nIDEvent);
}

CConfiguration* CMainFrame::GetConfig()
{
	return m_pConfigData;
}

void CMainFrame::OnFileOptions()
{
	CConfigurationDialog dlg;
	dlg.DoModal();
	m_pConfigData->Load();
}

void CMainFrame::OnViewInfo()
{
	if ( m_pInfoDialog->IsWindowVisible() )
        m_pInfoDialog->ShowWindow( SW_HIDE );
	else
        m_pInfoDialog->ShowWindow( SW_SHOW );
}

void CMainFrame::OnUpdateViewInfo(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_pInfoDialog->IsWindowVisible() );
}
#include <winuser.h>

void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	m_wndView.OnKeyDown(nChar, nRepCnt, nFlags);	

	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
