/////////////////////////////////////////////////////////////////
//	Hardware.cpp : implementation file for Dialog box used to configure
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

#include "stdafx.h"
#include "resource.h"
#include "HardwareDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CHardwareDlg dialog
/////////////////////////////////////////////////////////////////////////////

CHardwareDlg::CHardwareDlg(CWnd *pParent) : CDialog(CHardwareDlg::IDD, pParent)
	, m_bPRS(false)
	, m_bTempCtrl(false)
	, m_bDiagnMode(false)
	, m_bInPickArm(false)
	, m_bTA1(false)
	, m_bTA2(false)
	, m_bACF1WH(false)
	, m_bACF2WH(false)
	, m_bACF1(false)
	, m_bInspOpt(false)
{
	//{{AFX_DATA_INIT(CHardware)
	m_bHardware = FALSE;
	//}}AFX_DATA_INIT
}

void CHardwareDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHardwareDlg)
	DDX_Check(pDX, IDC_HW_SELECTION, m_bHardware);
	DDX_Check(pDX, IDC_PRS_SEL, m_bPRS);
	DDX_Check(pDX, IDC_TEMP_CTRL_SEL, m_bTempCtrl);
	DDX_Check(pDX, IDC_DIAGN_MODE_SEL, m_bDiagnMode);
	DDX_Check(pDX, IDC_INPICKARM_SEL, m_bInPickArm);
	DDX_Check(pDX, IDC_TA1_SEL, m_bTA1);
	DDX_Check(pDX, IDC_TA2_SEL, m_bTA2);
	DDX_Check(pDX, IDC_ACF1WH_SEL, m_bACF1WH);
	DDX_Check(pDX, IDC_ACF2WH_SEL, m_bACF2WH);
	DDX_Check(pDX, IDC_ACF1_SEL, m_bACF1);
	DDX_Check(pDX, IDC_INSPOPT_SEL, m_bInspOpt);
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_HW_SELECTION, m_ctrHardware);
	DDX_Control(pDX, IDC_PRS_SEL, m_ctrPRS);
	DDX_Control(pDX, IDC_TEMP_CTRL_SEL, m_ctrTempCtrl);
	DDX_Control(pDX, IDC_DIAGN_MODE_SEL, m_ctrDiagnMode);
	DDX_Control(pDX, IDC_INPICKARM_SEL, m_ctrInPickArm);
	DDX_Control(pDX, IDC_TA1_SEL, m_ctrTA1);
	DDX_Control(pDX, IDC_TA2_SEL, m_ctrTA2);
	DDX_Control(pDX, IDC_ACF1WH_SEL, m_ctrACF1WH);
	DDX_Control(pDX, IDC_ACF2WH_SEL, m_ctrACF2WH);
	DDX_Control(pDX, IDC_ACF1_SEL, m_ctrACF1);
	DDX_Control(pDX, IDC_INSPOPT_SEL, m_ctrInspOpt);
}

BOOL CHardwareDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (!m_bHardware || m_bIsAppRunning)
	{
		if (!m_bIsAppRunning) //20121114
		{
			m_bDiagnMode = FALSE;
			m_ctrDiagnMode.EnableWindow(FALSE);

			m_bInPickArm	= FALSE;
			m_bTA1			= FALSE;
			m_bTA2			= FALSE;
			m_bACF1WH		= FALSE;
			m_bACF2WH		= FALSE;
			m_bACF1			= FALSE;
			m_bInspOpt		= FALSE;
		}

		if (m_bIsAppRunning) //20120402
		{
			m_ctrDiagnMode.EnableWindow(FALSE);
			m_ctrHardware.EnableWindow(FALSE);
			m_ctrPRS.EnableWindow(FALSE);
			m_ctrTempCtrl.EnableWindow(FALSE);
		}
		m_ctrInPickArm.EnableWindow(FALSE);
		m_ctrTA1.EnableWindow(FALSE);
		m_ctrTA2.EnableWindow(FALSE);
		m_ctrACF1WH.EnableWindow(FALSE);
		m_ctrACF2WH.EnableWindow(FALSE);
		m_ctrACF1.EnableWindow(FALSE);
		m_ctrInspOpt.EnableWindow(FALSE);
		m_ctrDiagnMode.EnableWindow(FALSE);
	}
	else
	{
		if (m_bDiagnMode)
		{
			m_ctrInPickArm.EnableWindow(FALSE);
			m_ctrTA1.EnableWindow(FALSE);
			m_ctrTA2.EnableWindow(FALSE);
			m_ctrACF1WH.EnableWindow(FALSE);
			m_ctrACF2WH.EnableWindow(FALSE);
			m_ctrACF1.EnableWindow(FALSE);
			m_ctrInspOpt.EnableWindow(FALSE);
		}
		else
		{
			m_ctrInPickArm.EnableWindow(TRUE);
			m_ctrTA1.EnableWindow(TRUE);
			m_ctrTA2.EnableWindow(TRUE);
			m_ctrACF1WH.EnableWindow(TRUE);
			m_ctrACF2WH.EnableWindow(TRUE);
			m_ctrACF1.EnableWindow(TRUE);
			m_ctrInspOpt.EnableWindow(TRUE);
		}
	}

	UpdateData(FALSE);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CHardwareDlg, CDialog)
	//{{AFX_MSG_MAP(CHardwareDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_HW_SELECTION, OnBnClickedHwSelection)
	ON_BN_CLICKED(IDC_DIAGN_MODE_SEL, OnBnClickedDiagnModeSel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHardwareDlg message handlers
/////////////////////////////////////////////////////////////////////////////

void CHardwareDlg::OnBnClickedHwSelection()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_bHardware)
	{
		m_bInPickArm	= TRUE;
		m_bTA1			= TRUE;
		m_bTA2			= TRUE;
		m_bACF1WH		= TRUE;
		m_bACF2WH		= TRUE;
		m_bACF1			= TRUE;
		m_bInspOpt		= TRUE;
		m_bDiagnMode	= FALSE;

		m_ctrInPickArm.EnableWindow(TRUE);
		m_ctrTA1.EnableWindow(TRUE);
		m_ctrTA2.EnableWindow(TRUE);
		m_ctrACF1WH.EnableWindow(TRUE);
		m_ctrACF2WH.EnableWindow(TRUE);
		m_ctrACF1.EnableWindow(TRUE);
		m_ctrInspOpt.EnableWindow(TRUE);
		m_ctrDiagnMode.EnableWindow(TRUE);
	}
	else
	{
		m_bInPickArm	= FALSE;
		m_bTA1			= FALSE;
		m_bTA2			= FALSE;
		m_bACF1WH		= FALSE;
		m_bACF2WH		= FALSE;
		m_bACF1			= FALSE;
		m_bInspOpt		= FALSE;
		m_bDiagnMode	= FALSE;

		m_ctrInPickArm.EnableWindow(FALSE);
		m_ctrTA1.EnableWindow(FALSE);
		m_ctrTA2.EnableWindow(FALSE);
		m_ctrACF1WH.EnableWindow(FALSE);
		m_ctrACF2WH.EnableWindow(FALSE);
		m_ctrACF1.EnableWindow(FALSE);
		m_ctrInspOpt.EnableWindow(FALSE);
		m_ctrDiagnMode.EnableWindow(FALSE);
	}

	UpdateData(FALSE);

}

void CHardwareDlg::OnBnClickedDiagnModeSel()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	// diagn mode enabled
	if (m_bDiagnMode)
	{
		m_bInPickArm	= TRUE;
		m_bTA1			= TRUE;
		m_bTA2			= TRUE;
		m_bACF1WH		= TRUE;
		m_bACF2WH		= TRUE;
		m_bACF1			= TRUE;
		m_bInspOpt		= TRUE;

		m_ctrInPickArm.EnableWindow(FALSE);
		m_ctrTA1.EnableWindow(FALSE);
		m_ctrTA2.EnableWindow(FALSE);
		m_ctrACF1WH.EnableWindow(FALSE);
		m_ctrACF2WH.EnableWindow(FALSE);
		m_ctrACF1.EnableWindow(FALSE);
		m_ctrInspOpt.EnableWindow(FALSE);
	}
	else
	{
		m_bInPickArm	= TRUE;
		m_bTA1			= TRUE;
		m_bTA2			= TRUE;
		m_bACF1WH		= TRUE;
		m_bACF2WH		= TRUE;
		m_bACF1			= TRUE;
		m_bInspOpt		= TRUE;

		m_ctrInPickArm.EnableWindow(TRUE);
		m_ctrTA1.EnableWindow(TRUE);
		m_ctrTA2.EnableWindow(TRUE);
		m_ctrACF1WH.EnableWindow(TRUE);
		m_ctrACF2WH.EnableWindow(TRUE);
		m_ctrACF1.EnableWindow(TRUE);
		m_ctrInspOpt.EnableWindow(TRUE);
	}
}

void CHardwareDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
