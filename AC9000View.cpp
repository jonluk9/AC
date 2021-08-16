/////////////////////////////////////////////////////////////////
//	AC9000View.cpp : implementation of the CAC9000View class
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
#include "AC9000.h"
#include "AC9000Doc.h"
#include "AC9000View.h"
#include "MarkConstant.h"
#include "AC9000_Constant.h"
#include "StringEntryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////
//	Constant Definitions
/////////////////////////////////////////////////////////////////

const INT gnMAX_MESSAGE_LINES		= 1000;


/////////////////////////////////////////////////////////////////
// CAC9000View
/////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CAC9000View, CFormView)


BEGIN_MESSAGE_MAP(CAC9000View, CFormView)
	ON_BN_CLICKED(IDC_GROUP_ID, OnGroupId)
	ON_BN_CLICKED(IDC_AUTO_START, OnAutoStart)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_SELECT_HARDWARE, OnSelectHardware)
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_BN_CLICKED(IDC_LOG_MSG, OnLogMsg)
	ON_BN_CLICKED(IDC_EXEC_HMI, OnExecuteHmi)
	ON_BN_CLICKED(IDC_MODULE_NAME, OnBnClickedModuleName)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////
// CAC9000View construction/destruction
/////////////////////////////////////////////////////////////////

CAC9000View::CAC9000View()
	: CFormView(CAC9000View::IDD)
{
	// TODO: add construction code here
}

CAC9000View::~CAC9000View()
{
}

void CAC9000View::DoDataExchange(CDataExchange *pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSG_LIST, m_lbMessage);
	DDX_Control(pDX, IDC_GROUP_ID, m_btnGroupId);
	DDX_Control(pDX, IDC_MODULE_NAME, m_btnModule);
	DDX_Control(pDX, IDC_AUTO_START, m_btnAutoStart);
	DDX_Control(pDX, IDC_START, m_btnStart);
	DDX_Control(pDX, IDC_LOG_MSG, m_btnLogMessage);
	DDX_Control(pDX, IDC_EXEC_HMI, m_btnExecHmi);
}

BOOL CAC9000View::PreCreateWindow(CREATESTRUCT &cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CFormView::PreCreateWindow(cs);
}

void CAC9000View::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	CAC9000App *pApp = (CAC9000App*) AfxGetApp();
	m_btnGroupId.SetWindowText(pApp->GetGroupID());

	if (pApp->AutoStart())
	{
		m_btnAutoStart.SetCheck(1);
		m_btnStart.EnableWindow(FALSE);
	}
	else
	{
		m_btnAutoStart.SetCheck(0);
	}

	m_btnLogMessage.SetCheck(pApp->LogMessage());
	m_btnExecHmi.SetCheck(pApp->IsExecuteHmi());
}


/////////////////////////////////////////////////////////////////
// CAC9000View diagnostics
/////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void CAC9000View::AssertValid() const
{
	CFormView::AssertValid();
}

void CAC9000View::Dump(CDumpContext &dc) const
{
	CFormView::Dump(dc);
}

CAC9000Doc *CAC9000View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAC9000Doc)));
	return (CAC9000Doc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CAC9000View message handlers
/////////////////////////////////////////////////////////////////////////////

void CAC9000View::OnGroupId() 
{
#if 1 //20120320 lock groupID
	CAC9000App *pApp	= (CAC9000App*) AfxGetApp();
	if (pApp->State() != UN_INITIALIZE_Q)
	{
		CString szMessage = _T("Group ID cannot be changed while appln is Initialized");
		DisplayMessage(szMessage);

		return;
	}
	pApp->SetGroupId("COG");
	m_btnGroupId.SetWindowText("COG");
	DisplayMessage("Set Group ID to COG");
#else
	CStringEntryDlg				dlg;
	CAC9000App *pApp	= (CAC9000App*) AfxGetApp();

	if (pApp->State() != UN_INITIALIZE_Q)
	{
		CString szMessage = _T("Group ID cannot be changed while appln is Initialized");
		DisplayMessage(szMessage);

		return;
	}
	
	dlg.m_szTitle = _T("Group ID");
	m_btnGroupId.GetWindowText(dlg.m_szString);

	if (dlg.DoModal() == IDOK)
	{
		pApp->SetGroupId(dlg.m_szString);
		m_btnGroupId.SetWindowText(dlg.m_szString);
	}
#endif
}

void CAC9000View::OnAutoStart() 
{
	CAC9000App *pApp = (CAC9000App*) AfxGetApp();
	INT nState = m_btnAutoStart.GetCheck();
	
	BOOL bState = FALSE;

	if (nState == 1)
	{
		bState = TRUE;
	}

	pApp->SaveAutoStart(bState);

	if ((nState == 1) && (pApp->State() == UN_INITIALIZE_Q))
	{
		m_btnStart.EnableWindow(FALSE);
		pApp->SetRun();
	}
}

void CAC9000View::OnStart() 
{
	CAC9000App *pApp = (CAC9000App*) AfxGetApp();

	m_btnStart.EnableWindow(FALSE);
	pApp->m_isAppRunning = TRUE; //20111214
	pApp->SetRun();
}

void CAC9000View::OnSelectHardware() 
{
	CAC9000App *pApp = (CAC9000App*) AfxGetApp();
	if (pApp->m_isAppRunning) //20111214
	{
		pApp->SelectHardware(FALSE);
	}
	else
	{
		pApp->SelectHardware(TRUE);
	}
}

void CAC9000View::OnOptions() 
{
	CAC9000App *pApp = (CAC9000App*) AfxGetApp();
	if (pApp->m_isAppRunning) //20111214
	{
		pApp->SetOptions(FALSE);
	}
	else
	{
		pApp->SetOptions(TRUE);
	}
}

void CAC9000View::OnLogMsg() 
{
	CAC9000App *pApp	= (CAC9000App*) AfxGetApp();
	INT nState					= m_btnLogMessage.GetCheck();
	BOOL bState					= FALSE;

	if (nState == 1)
	{
		bState = TRUE;
	}

	pApp->SetLogMessage(bState);
}

void CAC9000View::OnExecuteHmi()
{
	CAC9000App *pApp	= (CAC9000App*) AfxGetApp();
	INT nState					= m_btnExecHmi.GetCheck();
	BOOL bState					= FALSE;

	if (nState == 1)
	{
		bState = TRUE;
	}

	pApp->SetExecuteHmiFlag(bState);
}

VOID CAC9000View::SetGroupID(CString &szGroupID)
{
	m_btnGroupId.SetWindowText(szGroupID);
}

VOID CAC9000View::SetModule(CString &szModule)
{
	m_btnModule.SetWindowText(szModule);
}


/////////////////////////////////////////////////////////////////////////////
// Message Display
/////////////////////////////////////////////////////////////////////////////

VOID CAC9000View::DisplayMessage(const CString &szMessage)
{
	INT nIndex = m_lbMessage.AddString(szMessage);
	m_lbMessage.PostMessage(LB_SETCURSEL, nIndex, 0);
	m_lbMessage.PostMessage(LB_SETCURSEL, -1, 0);

	if (m_lbMessage.GetCount() > gnMAX_MESSAGE_LINES)
	{
		m_lbMessage.DeleteString(0);
	}
}

void CAC9000View::OnBnClickedModuleName()
{
	// TODO: Add your control notification handler code here
}
