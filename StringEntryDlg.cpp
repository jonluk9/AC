/////////////////////////////////////////////////////////////////
//	StringEntryDlg.cpp : interface of the StringEntryDlg class
//
//	Description:
//		AC9000 Application Software
//
//	Date:		Thursday, September 09, 2010
//	Revision:	1.00
//
//	By:			AC9000
//				CSP
//
//	Copyright @ ASM Assembly Automation Ltd., 2010.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"       // main symbols
#include "StringEntryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CStringEntryDlg dialog
/////////////////////////////////////////////////////////////////////////////

CStringEntryDlg::CStringEntryDlg(CWnd *pParent) : CDialog(CStringEntryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStringEntryDlg)
	m_szString = _T("");
	//}}AFX_DATA_INIT

	m_szTitle = _T("String Entry");
}

void CStringEntryDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStringEntryDlg)
	DDX_Text(pDX, IDC_STRING, m_szString);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStringEntryDlg, CDialog)
	//{{AFX_MSG_MAP(CStringEntryDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CStringEntryDlg message handlers
/////////////////////////////////////////////////////////////////////////////

BOOL CStringEntryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_szTitle);
	SetWindowPos(&this->wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return TRUE;
}
