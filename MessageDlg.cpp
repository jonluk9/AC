// MessageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AC9000.h"
#include "MessageDlg.h"


// MessageDlg dialog

IMPLEMENT_DYNAMIC(MessageDlg, CDialog)
MessageDlg::MessageDlg(CWnd *pParent /*=NULL*/)
	: CDialog(MessageDlg::IDD, pParent)
{
}

MessageDlg::~MessageDlg()
{
}

void MessageDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MessageDlg, CDialog)
END_MESSAGE_MAP()


// MessageDlg message handlers
