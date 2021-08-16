/////////////////////////////////////////////////////////////////
//	Hardware.h : header file for Dialog box used to configure
//		the controller hardware in the AC9000 
//		application
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
// CHardwareDlg dialog
/////////////////////////////////////////////////////////////////////////////

class CHardwareDlg : public CDialog
{
public:
	CHardwareDlg(CWnd *pParent = NULL);

	// Dialog Data
	//{{AFX_DATA(CHardwareDlg)
	enum { IDD = IDD_HARDWARE };
	BOOL	m_bHardware;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHardwareDlg)
protected:
	virtual void DoDataExchange(CDataExchange *pDX);
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CHardwareDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedHwSelection();

	BOOL	m_bPRS;
	BOOL	m_bTempCtrl;
	BOOL	m_bDiagnMode;
	BOOL	m_bInConveyor;
	BOOL	m_bTA1;
	BOOL	m_bTA2;
	BOOL	m_bTA3;
	BOOL	m_bTA4;
	BOOL	m_bTA5;
	BOOL	m_bOutConveyor;
	BOOL	m_bINWH;
	BOOL	m_bOUTWH;
	BOOL	m_bACF1WH;
	BOOL	m_bACF2WH;
	BOOL	m_bACF1;
	BOOL	m_bACF2;
	BOOL	m_bPreBondWH;
	BOOL	m_bPBUplook;
	BOOL	m_bPB1;
	BOOL	m_bPB2;
	BOOL	m_bMB1WH;
	BOOL	m_bMB2WH;
	BOOL	m_bMB1;
	BOOL	m_bMB2;
	BOOL	m_bTrayIndexer;
	BOOL	m_bChipPickArm;
	BOOL	m_bFlipper;
	BOOL	m_bSH1;
	BOOL	m_bSH2;
	BOOL	m_bIsAppRunning;

	BOOL	m_bFPCPickArm;
	BOOL	m_bFPCRejectArm;
	BOOL	m_bInSHWH;
	BOOL	m_bSH;
	BOOL	m_bMAINBOND3;
	BOOL	m_bMB3WH;
	BOOL	m_bMB4WH;
	BOOL	m_bMB5WH;
	BOOL	m_bMB6WH;
	BOOL	m_bOutWH;
	BOOL	m_bRejectArm;
	BOOL	m_bRejectBelt;
	BOOL	m_bInPickArm;
	BOOL	m_bInspOpt;

	CButton m_ctrHardware; //20120402
	CButton m_ctrPRS;
	CButton m_ctrTempCtrl;
	CButton m_ctrDiagnMode;
	CButton m_ctrInConveyor;
	CButton m_ctrTA1;
	CButton m_ctrTA2;
	CButton m_ctrTA3;
	CButton m_ctrTA4;
	CButton m_ctrTA5;
	CButton m_ctrOutConveyor;
	CButton m_ctrINWH;
	CButton m_ctrACF1WH;
	CButton m_ctrACF2WH;
	CButton m_ctrACF1;
	CButton m_ctrACF2;
	CButton m_ctrPreBondWH;
	CButton m_ctrPBUplook;
	CButton m_ctrPB1;
	CButton m_ctrPB2;
	CButton m_ctrMB1WH;
	CButton m_ctrMB2WH;
	CButton m_ctrMB1;
	CButton m_ctrMB2;
	CButton m_ctrTrayIndexer;

	CButton m_ctrFPCPickArm;
	CButton m_ctrFPCRejectArm;
	CButton m_ctrInSHWH;
	CButton m_ctrSH;
	CButton m_ctrMB3;
	CButton m_ctrMB3WH;
	CButton m_ctrMB4WH;
	CButton m_ctrMB5WH;
	CButton m_ctrMB6WH;
	CButton m_ctrOutWH;
	CButton m_ctrRejectArm;
	CButton m_ctrRejectBelt;
	CButton m_ctrInPickArm;
	CButton m_ctrInspOpt;

	afx_msg void OnBnClickedDiagnModeSel();
	afx_msg void OnBnClickedOk();
};
