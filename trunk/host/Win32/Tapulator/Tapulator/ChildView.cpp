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


// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "Tapulator.h"
#include "ChildView.h"
#include "MainFrm.h"
#include ".\childview.h"
#include "MoreKeys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CMainFrame* pFrm = (CMainFrame*) AfxGetMainWnd();
	// Do not call CWnd::OnPaint() for painting messages
	pFrm->PaintToppy(&dc);
}


BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	CMainFrame* pFrm = (CMainFrame*) AfxGetMainWnd();
	if (pFrm->IsTapLoaded())
		return 0;

	return CWnd::OnEraseBkgnd(pDC);
}

void RaiseKeyEvent(int keyCode, int hardwareKeyCode)
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	pFrame->GetTapModule()->RaiseKeyEvent(keyCode, hardwareKeyCode);
}


void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch ( nChar )
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		RaiseKeyEvent(RKEY_0+nChar-48, RAWKEY_0+nChar-48);
		break;
	case VK_F1:
		RaiseKeyEvent(RKEY_NewF1, RAWKEY_Red);
		break;
	case VK_F2:
		RaiseKeyEvent(RKEY_F2, RAWKEY_Green);
		break;
	case VK_F3:
		RaiseKeyEvent(RKEY_F3, RAWKEY_Yellow);
		break;
	case VK_F4:
		RaiseKeyEvent(RKEY_F4, RAWKEY_Blue);
		break;
	case VK_F5:
		RaiseKeyEvent(RKEY_Sat, RAWKEY_PIPSwap_Sat);
		break;
	case VK_F6:
		RaiseKeyEvent(RKEY_Ab, RAWKEY_White_Ab);
		break;
	case VK_LEFT:
		RaiseKeyEvent(RKEY_VolDown, RAWKEY_Left);
		break;
	case VK_RIGHT:
		RaiseKeyEvent(RKEY_VolUp, RAWKEY_Right);
		break;
	case VK_UP:
		RaiseKeyEvent(RKEY_ChUp, RAWKEY_Up);
		break;
	case VK_DOWN:
		RaiseKeyEvent(RKEY_ChDown, RAWKEY_Down);
		break;
	case VK_ESCAPE:
		RaiseKeyEvent(RKEY_Exit, RAWKEY_Exit_AudioTrk);
		break;
	case VK_RETURN:
		RaiseKeyEvent(RKEY_Ok, RAWKEY_Ok);
		break;
	case 'A':
		RaiseKeyEvent(RKEY_PlayList, RAWKEY_PlayList);
		break;
	case 'I':
		RaiseKeyEvent(RKEY_Info, RAWKEY_Info);
		break;
	case 'M':
		RaiseKeyEvent(RKEY_Menu, RAWKEY_Menu);
		break;
	case 'G':
		RaiseKeyEvent(RKEY_Guide, RAWKEY_Guide);
		break;
	case 'E':
		RaiseKeyEvent(RKEY_Exit, RAWKEY_Exit_AudioTrk);
		break;
	case 'L':
		RaiseKeyEvent(RKEY_TvRadio, RAWKEY_List_TVRad);
		break;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
