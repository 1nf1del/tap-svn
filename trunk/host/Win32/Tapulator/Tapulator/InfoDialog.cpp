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

// InfoDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Tapulator.h"
#include "InfoDialog.h"
#include ".\infodialog.h"
#include "Heap.h"

// CInfoDialog dialog

IMPLEMENT_DYNAMIC(CInfoDialog, CDialog)
CInfoDialog::CInfoDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CInfoDialog::IDD, pParent)
	, m_sBytes(_T(""))
	, m_sMaxBytes(_T(""))
	, m_sTotalAllocs(_T(""))
	, m_sOpenAllocs(_T(""))
	, m_sLargestAlloc(_T(""))
{
}

CInfoDialog::~CInfoDialog()
{
}

void CInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_LogCtrl);
	DDX_Text(pDX, IDC_BYTES, m_sBytes);
	DDX_Text(pDX, IDC_MAXBYTES, m_sMaxBytes);
	DDX_Text(pDX, IDC_TOTALLOCS, m_sTotalAllocs);
	DDX_Text(pDX, IDC_ALLOCSOPEN, m_sOpenAllocs);
	DDX_Text(pDX, IDC_LARGEST, m_sLargestAlloc);
}


BEGIN_MESSAGE_MAP(CInfoDialog, CDialog)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
END_MESSAGE_MAP()


// CInfoDialog message handlers

void CInfoDialog::LogInfo(CString sText)
{
	m_LogCtrl.SetLimitText(0x70000000); 
	if (sText.Right(2) == "\r\n")
	{

	}
	else if (sText.Right(1) == "\r")
	{
		sText += "\n";
	}
	else if (sText.Right(1) == "\n")
	{
		sText = sText.Left(sText.GetLength() -1) + "\r\n";
	}
	else
	{
		sText += "\r\n";
	}

	CString sExistingText;
	m_LogCtrl.GetWindowText(sExistingText);

	m_LogCtrl.SetWindowText(sExistingText + sText);

	int iScroll = m_LogCtrl.GetLineCount() - m_LogCtrl.GetFirstVisibleLine() - 4;
	m_LogCtrl.LineScroll(iScroll);

	UpdateHeapInfo();
}
void CInfoDialog::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CInfoDialog::UpdateHeapInfo()
{
	Heap* pHeap = Heap::GetTheHeap();

	m_sBytes.Format("%d", pHeap->m_iTotalBytes);
	m_sMaxBytes.Format("%d", pHeap->m_iMaxBytes);
	m_sOpenAllocs.Format("%d", pHeap->m_iOutstandingAllocs);
	m_sTotalAllocs.Format("%d", pHeap->m_iAllocNum);
	m_sLargestAlloc.Format("%d", pHeap->m_iLargestAlloc);

	UpdateData(false);
}