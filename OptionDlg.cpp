/////////////////////////////////////////////////////////////////
//	OptionDlg.cpp : implementation file for Dialog box used to configure
//		the option selections in the AC9000
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
#include "OptionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// COptionDlg dialog
/////////////////////////////////////////////////////////////////////////////

COptionDlg::COptionDlg(CWnd *pParent) : CDialog(COptionDlg::IDD, pParent)
	, m_bSelectVer1(false)
	, m_bSelectVer2(false)
	, m_bSelectVer3(false)
	, m_bSelectLitec(false)
	, m_bSelectADAM(false)
	, m_bSelectHeaterOn(false)	// 20140905 Yip
	, m_bSelectCOG902(false)
	//, m_bSelectEdgeCleaner(false)
	, m_bSelectTA4Camera(false)
	, m_bSelectInConvWH(false)
	, m_bIsAppRunning(false) //20120402
{
	//{{AFX_DATA_INIT(COptionDlg)
	//}}AFX_DATA_INIT
}

void COptionDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionDlg)
	DDX_Check(pDX, IDC_VER1_MC_SELECTION, m_bSelectVer1);
	DDX_Check(pDX, IDC_VER2_MC_SELECTION, m_bSelectVer2);
	DDX_Check(pDX, IDC_VER3_MC_SELECTION, m_bSelectVer3);
	DDX_Check(pDX, IDC_LITEC_SELECTION, m_bSelectLitec);
	DDX_Check(pDX, IDC_ADAM_SELECTION, m_bSelectADAM);
	DDX_Check(pDX, IDC_HEATER_ON_SELECTION, m_bSelectHeaterOn);	// 20140905 Yip
	//DDX_Check(pDX, IDC_EDGE_CLEANER_SELECTION, m_bSelectCOG902);
	//DDX_Check(pDX, IDC_TA4_CAMERA_SELECTION, m_bSelectTA4Camera);
	//DDX_Check(pDX, IDC_INCONVWH_SELECTION, m_bSelectInConvWH); //20170724
	
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_VER1_MC_SELECTION, m_ctrSelectVer1Mc);
	DDX_Control(pDX, IDC_VER2_MC_SELECTION, m_ctrSelectVer2Mc);
	DDX_Control(pDX, IDC_VER3_MC_SELECTION, m_ctrSelectVer3Mc);
	DDX_Control(pDX, IDC_LITEC_SELECTION, m_ctrSelectLitec);
	DDX_Control(pDX, IDC_ADAM_SELECTION, m_ctrSelectADAM);
	DDX_Control(pDX, IDC_EDGE_CLEANER_SELECTION, m_ctrSelectEdgeCleaner);
	//DDX_Control(pDX, IDC_TA4_CAMERA_SELECTION, m_ctrSelectTA4Camera);
	//DDX_Control(pDX, IDC_INCONVWH_SELECTION, m_ctrSelectInConvWH); //20170724
	//DDX_Control(pDX, IDC_HEATER_ON_SELECTION, m_ctrSelectHeaterOn);	// 20140905 Yip
	DDX_Control(pDX, IDOK, m_ctrOK); //20120402
}

BOOL COptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_bIsAppRunning) //20120402
	{
		m_ctrSelectVer1Mc.EnableWindow(FALSE);
		m_ctrSelectVer2Mc.EnableWindow(FALSE);
		m_ctrSelectVer3Mc.EnableWindow(FALSE);
		m_ctrSelectLitec.EnableWindow(FALSE);
		m_ctrSelectADAM.EnableWindow(FALSE);
		m_ctrSelectHeaterOn.EnableWindow(FALSE);	// 20140905 Yip
		//m_ctrSelectEdgeCleaner.EnableWindow(FALSE);
		//m_ctrSelectTA4Camera.EnableWindow(FALSE);
		//m_ctrSelectInConvWH.EnableWindow(FALSE);
		m_ctrOK.EnableWindow(FALSE);
	}
	UpdateData(FALSE);
	return TRUE;
}

BEGIN_MESSAGE_MAP(COptionDlg, CDialog)
	//{{AFX_MSG_MAP(COptionDlg)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_VER1_MC_SELECTION, OnBnClickedVer1McSelection)
	ON_BN_CLICKED(IDC_VER2_MC_SELECTION, OnBnClickedVer2McSelection)
	ON_BN_CLICKED(IDC_VER3_MC_SELECTION, OnBnClickedVer3McSelection)
	//	ON_BN_CLICKED(IDC_LITEC_SELECTION, OnBnClickedEcoreSelection)
	ON_BN_CLICKED(IDC_ADAM_SELECTION, OnBnClickedAdamSelection)
	ON_BN_CLICKED(IDC_LITEC_SELECTION, OnBnClickedLitecSelection)
	ON_BN_CLICKED(IDC_EDGE_CLEANER_SELECTION, OnBnClickedEdgeCleanerSelection)
	ON_BN_CLICKED(IDC_TA4_CAMERA_SELECTION, OnBnClickedTa4CameraSelection)
	ON_BN_CLICKED(IDC_INCONVWH_SELECTION, OnBnClickedInConvWHSelection)
	ON_BN_CLICKED(IDC_HEATER_ON_SELECTION, OnBnClickedHeaterOnSelection)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COptionDlg message handlers
/////////////////////////////////////////////////////////////////////////////

void COptionDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void COptionDlg::OnBnClickedVer1McSelection()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_bSelectVer1 = TRUE;
	m_bSelectVer2 = FALSE;
	m_bSelectVer3 = FALSE;

	//if (m_bSelectVer1)
	//{
	//	m_bSelectVer1 = TRUE;
	//	m_bSelectVer2 = FALSE;
	//	m_bSelectVer3 = FALSE;
	//}
	//else
	//{
	//	m_bSelectVer1 = FALSE;
	//	m_bSelectVer2 = TRUE;
	//	m_bSelectVer3 = FALSE;
	//}

	UpdateData(FALSE);
}

void COptionDlg::OnBnClickedVer2McSelection()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_bSelectVer1 = FALSE;
	m_bSelectVer2 = TRUE;
	m_bSelectVer3 = FALSE;
	//if (m_bSelectVer2)
	//{
	//	m_bSelectVer1 = FALSE;
	//	m_bSelectVer2 = TRUE;
	//}
	//else
	//{
	//	m_bSelectVer1 = TRUE;
	//	m_bSelectVer2 = FALSE;
	//}

	UpdateData(FALSE);
}

void COptionDlg::OnBnClickedVer3McSelection()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_bSelectVer1 = FALSE;
	m_bSelectVer2 = FALSE;
	m_bSelectVer3 = TRUE;

	UpdateData(FALSE);
}

//void COptionDlg::OnBnClickedEcoreSelection()
//{
//	// TODO: Add your control notification handler code here
//	UpdateData(TRUE);
//
//	if (m_bSelectLitec)
//	{
//		m_bSelectLitec = TRUE;
//	}
//
//	UpdateData(FALSE);
//}

void COptionDlg::OnBnClickedAdamSelection()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_bSelectADAM)
	{
		if (m_bSelectLitec)
		{
			m_bSelectADAM = TRUE;
		}
		else
		{
			m_bSelectADAM = FALSE;
		}
	}

	UpdateData(FALSE);
}

void COptionDlg::OnBnClickedLitecSelection()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_bSelectLitec)
	{
		m_bSelectLitec = TRUE;
	}

	UpdateData(FALSE);
}

void COptionDlg::OnBnClickedEdgeCleanerSelection()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_bSelectCOG902/*m_bSelectEdgeCleaner*/)
	{
		m_bSelectCOG902 = TRUE;
		//m_bSelectEdgeCleaner = TRUE;
	}
	else
	{
		m_bSelectCOG902 = FALSE;
		//m_bSelectEdgeCleaner = FALSE;
	}

	UpdateData(FALSE);
}

void COptionDlg::OnBnClickedTa4CameraSelection()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	//if (m_bSelectTA4Camera)
	//{
	//	m_bSelectTA4Camera = TRUE;
	//}
	//else
	//{
	//	m_bSelectTA4Camera = FALSE;
	//}

	UpdateData(FALSE);
}

void COptionDlg::OnBnClickedInConvWHSelection()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	//	m_bSelectInConvWH = !m_bSelectInConvWH;

	UpdateData(FALSE);
}

void COptionDlg::OnBnClickedHeaterOnSelection()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_bSelectHeaterOn)	// 20140905 Yip
	{
		m_bSelectHeaterOn = TRUE;
	}

	UpdateData(FALSE);
}
