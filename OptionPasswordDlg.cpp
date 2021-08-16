// OptionPasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AC9000.h"
#include "OptionPasswordDlg.h"


// COptionPasswordDlg dialog

IMPLEMENT_DYNAMIC(COptionPasswordDlg, CDialog)
COptionPasswordDlg::COptionPasswordDlg(CWnd *pParent /*=NULL*/)
	: CDialog(COptionPasswordDlg::IDD, pParent)
{
	m_szPassword.Empty();
}

COptionPasswordDlg::~COptionPasswordDlg()
{
}

void COptionPasswordDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STRING_PASSWORD, m_ctrEditPassword);
}


BEGIN_MESSAGE_MAP(COptionPasswordDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// COptionPasswordDlg message handlers

//void COptionPasswordDlg::OnEnChangeStringPassword()
//{
//	// TODO:  If this is a RICHEDIT control, the control will not
//	// send this notification unless you override the CDialog::OnInitDialog()
//	// function and call CRichEditCtrl().SetEventMask()
//	// with the ENM_CHANGE flag ORed into the mask.
//
//	// TODO:  Add your control notification handler code here
//}

void COptionPasswordDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString strText = _T(" ");
	m_ctrEditPassword.GetLine(0, strText.GetBuffer(50), 50);

	m_szPassword = strText;
	OnOK();
}
