/////////////////////////////////////////////////////////////////
//	StringEntryDlg.h : header file for Dialog box used to enter
//						string
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

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CStringEntryDlg dialog
/////////////////////////////////////////////////////////////////////////////

class CStringEntryDlg : public CDialog
{
public:
	CString		m_szTitle;

// Construction
public:
	CStringEntryDlg(CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStringEntryDlg)
	enum { IDD = IDD_STRING_ENTRY };
	CString	m_szString;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStringEntryDlg)
protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CStringEntryDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
