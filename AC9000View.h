/////////////////////////////////////////////////////////////////
// AC9000View.h : interface of the CAC9000View class
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


class CAC9000View : public CFormView
{
protected: // create from serialization only
	CAC9000View();

	DECLARE_DYNCREATE(CAC9000View)

public:
	enum { IDD = IDD_AC9000_FORM };

	CListBox m_lbMessage;
	CButton	 m_btnGroupId;
	CButton	 m_btnModule;
	CButton	 m_btnAutoStart;
	CButton	 m_btnStart;
	CButton	 m_btnLogMessage;
	CButton  m_btnExecHmi;

// Attributes
public:
	CAC9000Doc *GetDocument() const;

// Operations
public:
	VOID DisplayMessage(const CString &szMessage);
	VOID SetGroupID(CString &szGroupID);
	VOID SetModule(CString &szGroupID);

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CAC9000View();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Generated message map functions
protected:
	afx_msg void OnGroupId();
	afx_msg void OnAutoStart();
	afx_msg void OnStart();
	afx_msg void OnSelectHardware();
	afx_msg void OnOptions();
	afx_msg void OnLogMsg();
	afx_msg void OnExecuteHmi();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedModuleName();
};

#ifndef _DEBUG  // debug version in AC9000View.cpp
inline CAC9000Doc *CAC9000View::GetDocument() const
{ 
	return reinterpret_cast<CAC9000Doc*>(m_pDocument); 
}
#endif
