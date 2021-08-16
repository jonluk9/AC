#pragma once
#include "afxwin.h"


// COptionPasswordDlg dialog

class COptionPasswordDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionPasswordDlg)

public:
	COptionPasswordDlg(CWnd *pParent = NULL);   // standard constructor
	virtual ~COptionPasswordDlg();

// Dialog Data
	enum { IDD = IDD_OPTION_PW };
	
	CString m_szPassword;

protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnEnChangeStringPassword();
	CEdit m_ctrEditPassword;
	afx_msg void OnBnClickedOk();
};
