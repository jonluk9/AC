#pragma once


// MessageDlg dialog

class MessageDlg : public CDialog
{
	DECLARE_DYNAMIC(MessageDlg)

public:
	MessageDlg(CWnd *pParent = NULL);   // standard constructor
	virtual ~MessageDlg();

// Dialog Data
	enum { IDD = IDD_AC9000_MESSAGE };

protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
