/////////////////////////////////////////////////////////////////
//	SettingFile.cpp : interface of the CSettingFile class
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
#include "MarkConstant.h"
#include "AC9000.h"
#include "AC9000_Constant.h"

#define SETTING_EXTERN
#include "SettingFile.h"

#include "TA1.h"
#include "TA2.h"
#include "ACF1.h"
//#include "ACF2.h"
#include "ACF1WH.h"
#include "ACF2WH.h"
#include "InPickArm.h"
//#include "Insp1.h"
//#include "Insp2.h"
#include "WinEagle.h"
//#include "PBGlass1PR.h"
//#include "PBGlass2PR.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CSettingFile, CAC9000Stn)

CSettingFile::CSettingFile()
{
#ifdef PRS_SET_IGNORE_WINDOW_FIRST //20150324
	HMI_bShowIgnoreWindow = FALSE;
	HMI_bShowIgnoreWindowBtn = FALSE;
	HMI_bShowIgnoreWindowNextBtn = FALSE;
#endif
}

CSettingFile::~CSettingFile()
{
}

BOOL CSettingFile::InitInstance()
{
	BOOL bTemp;
	bTemp = m_bModSelected;

	CAC9000Stn::InitInstance();
	
	bTemp = m_bModSelected;

	return TRUE;
}

INT CSettingFile::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CAC9000Stn::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CSettingFile::Operation()
{
	switch (State())
	{		
	case IDLE_Q:
		IdleOperation();
		break;

	case DIAGNOSTICS_Q:
		DiagOperation();
		break;

	case SYSTEM_INITIAL_Q:
		InitOperation();
		break;

	case PRESTART_Q:
		PreStartOperation();
		break;

	case CYCLE_Q:
		CycleOperation();
		break;

	case AUTO_Q:
		AutoOperation();
		break;

	case DEMO_Q:
		DemoOperation();
		break;

	case MANUAL_Q:
		ManualOperation();
		break;

	case ERROR_Q:
		ErrorOperation();
		break;

	case STOPPING_Q:
		StopOperation();
		break;

	case DE_INITIAL_Q:
		DeInitialOperation();
		break;

	default:
		State(IDLE_Q);
		Motion(FALSE);
		break;
	}

	UpdateOutput();

	if (!theApp.m_bSaveLoadDeviceMachine) //20150824
	{
		Sleep(10);
	}
	else
	{
		Sleep(10*10);
	}
}


/////////////////////////////////////////////////////////////////
//Update Functions
/////////////////////////////////////////////////////////////////

VOID CSettingFile::UpdateOutput()
{
	if ((m_qState == UN_INITIALIZE_Q) || (m_qState == DE_INITIAL_Q) || (m_qState == AUTO_Q) || (m_qState == DEMO_Q))
	{
		return;
	}

	try
	{
		//Update sensor here
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
}

VOID CSettingFile::RegisterVariables()
{
	try
	{
		// -------------------------------------------------------
		// varibales
		// -------------------------------------------------------
		RegVariable(_T("SF_bWizardEnable"), &g_bWizardEnable);
		RegVariable(_T("SF_szWizardTitle"), &g_szWizardTitle);
		RegVariable(_T("SF_ulWizardSubTitle"), &g_ulWizardSubTitle);
		RegVariable(_T("SF_szWizardStep"), &g_szWizardStep);
		RegVariable(_T("SF_szWizardMess"), &g_szWizardMess);
		RegVariable(_T("SF_szWizardMessPrev"), &g_szWizardMessPrev);
		RegVariable(_T("SF_szWizardMessNext"), &g_szWizardMessNext);
		RegVariable(_T("SF_szWizardBMPath"), &g_szWizardBMPath);
		RegVariable(_T("SF_bWizardEnableBack"), &g_bWizardEnableBack);
		RegVariable(_T("SF_bWizardEnableNext"), &g_bWizardEnableNext);
		RegVariable(_T("SF_bWizardEnableExit"), &g_bWizardEnableExit);
		RegVariable(_T("SF_ulWizardStep"), &g_ulWizardStep);
		RegVariable(_T("SF_lWizardInProgress"), &g_lWizardInProgress);

#ifdef PRS_SET_IGNORE_WINDOW_FIRST //20150324
		RegVariable(_T("SF_bShowIgnoreWindow"), &HMI_bShowIgnoreWindow); //20130319 PR ignore windows
		RegVariable(_T("SF_bShowIgnoreWindowBtn"), &HMI_bShowIgnoreWindowBtn);
		RegVariable(_T("SF_bShowIgnoreWindowNextBtn"), &HMI_bShowIgnoreWindowNextBtn);
		RegVariable(_T("SF_szWizardIgnoreWinTitle"), &g_szWizardIgnoreWinTitle);
		RegVariable(_T("SF_szWizardIgnoreWinSubtitle"), &g_szWizardIgnoreWinSubtitle);
#endif

		CString szMsg;
		for (INT i = 0; i < NUM_OF_POSN_XY; i++)
		{
			szMsg.Format("SF_szSetupPosn%ld", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupPosn[i].szPosn);
			szMsg.Format("SF_bSetupPosn%ldEnable", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupPosn[i].bEnable);
			szMsg.Format("SF_bSetupPosn%ldVisible", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupPosn[i].bVisible);
			szMsg.Format("SF_lSetupPosn%ldType", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupPosn[i].lType);
			szMsg.Format("SF_lSetupPosn%ldX", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupPosn[i].lPosnX);
			szMsg.Format("SF_lSetupPosn%ldY", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupPosn[i].lPosnY);

		}

		for (INT i = 0; i < NUM_OF_LEVEL_Z; i++)
		{
			szMsg.Format("SF_szSetupLevel%ld", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupLevel[i].szLevel);
			szMsg.Format("SF_bSetupLevel%ldEnable", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupLevel[i].bEnable);
			szMsg.Format("SF_bSetupLevel%ldVisible", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupLevel[i].bVisible);
			szMsg.Format("SF_lSetupLevel%ldType", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupLevel[i].lType);
			szMsg.Format("SF_lSetupLevel%ldZ", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupLevel[i].lLevelZ);
		}

		for (INT i = 0; i < NUM_OF_PARA; i++)
		{
			szMsg.Format("SF_szSetupPara%ld", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupPara[i].szPara);
			szMsg.Format("SF_bSetupPara%ldEnable", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupPara[i].bEnable);
			szMsg.Format("SF_bSetupPara%ldVisible", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupPara[i].bVisible);
			szMsg.Format("SF_lSetupPara%ldType", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupPara[i].lType);
			szMsg.Format("SF_dSetupPara%ldData", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupPara[i].dPara);
			szMsg.Format("SF_lSetupPara%ldData", i + 1);
			RegVariable(_T(szMsg),			&g_stSetupPara[i].lPara);
		}

		RegVariable(_T("SF_lSetupInProgress"),		&g_lSetupInProgress);

		// -------------------------------------------------------
		// commands
		// -------------------------------------------------------

		m_comServer.IPC_REG_SERVICE_COMMAND("SF_WizardSetup", HMI_WizardSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND("SF_WizardSetupBack", HMI_WizardSetupBack);
		m_comServer.IPC_REG_SERVICE_COMMAND("SF_WizardSetupNext", HMI_WizardSetupNext);
		m_comServer.IPC_REG_SERVICE_COMMAND("SF_WizardSetupExit", HMI_WizardSetupExit);

		m_comServer.IPC_REG_SERVICE_COMMAND("SF_ModuleSetupPreTask", HMI_ModuleSetupPreTask);
		m_comServer.IPC_REG_SERVICE_COMMAND("SF_ModuleSetupPostTask", HMI_ModuleSetupPostTask);

		m_comServer.IPC_REG_SERVICE_COMMAND("SF_ModuleSetupSetPosn", HMI_ModuleSetupSetPosn);
		m_comServer.IPC_REG_SERVICE_COMMAND("SF_ModuleSetupGoPosn", HMI_ModuleSetupGoPosn);

		m_comServer.IPC_REG_SERVICE_COMMAND("SF_ModuleSetupSetLevel", HMI_ModuleSetupSetLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND("SF_ModuleSetupGoLevel", HMI_ModuleSetupGoLevel);

		m_comServer.IPC_REG_SERVICE_COMMAND("SF_ModuleSetupSetPara", HMI_ModuleSetupSetPara);
	//	m_comServer.IPC_REG_SERVICE_COMMAND("SF_CalcWHZLinearEncoder", HMI_CalcWHZLinearEncoder);

		//m_comServer.IPC_REG_SERVICE_COMMAND("SF_SearchWHZLimits", HMI_SearchWHZLimits);	// 20141104 Yip: Add Functions For Searching Z Limits

#ifdef PRS_SET_IGNORE_WINDOW_FIRST //20150324
		m_comServer.IPC_REG_SERVICE_COMMAND("SF_WizardSetupPRIgnoreWindows", HMI_WizardSetupPRIgnoreWindows); //20130319 PR ignore windows
		m_comServer.IPC_REG_SERVICE_COMMAND("SF_WizardSetupPRIgnoreWindowsNext", HMI_WizardSetupPRIgnoreWindowsNext);
		m_comServer.IPC_REG_SERVICE_COMMAND("SF_WizardSetupPRIgnoreWindowsExit", HMI_WizardSetupPRIgnoreWindowsExit);
#endif
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
}


//-----------------------------------------------------------------------------
// HMI command implementation
//-----------------------------------------------------------------------------
//
LONG CSettingFile::HMI_WizardSetup(IPC_CServiceMessage &svMsg)
{
	CWinEagle *pCWinEagle	= (CWinEagle*)m_pModule->GetStation("WinEagle");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");

	typedef struct
	{
		LONG lStation;
		ULONG ulSubTitle;
	} PARA;

	PARA stPara;

	stPara.lStation = 0;
	stPara.ulSubTitle = 0;

	svMsg.GetMsg(sizeof(PARA), &stPara);

	g_bWizardEnable = TRUE;
	SetHmiVariable("SF_bWizardEnable");

	g_lWizardInProgress = stPara.lStation;
	g_ulWizardSubTitle = stPara.ulSubTitle;
	g_ulWizardStep = 0;

	g_bWizardEnableBack = FALSE;
	g_bWizardEnableNext = FALSE;
	g_bWizardEnableExit = FALSE;

	SetHmiVariable("SF_bWizardEnable");
	SetHmiVariable("SF_szWizardStep");
	SetHmiVariable("SF_szWizardMess");
	SetHmiVariable("SF_szWizardMessPrev");
	SetHmiVariable("SF_szWizardMessNext");
	SetHmiVariable("SF_szWizardBMPath");
	SetHmiVariable("SF_bWizardEnableBack");
	SetHmiVariable("SF_bWizardEnableNext");
	SetHmiVariable("SF_bWizardEnableExit");

	switch (g_lWizardInProgress)
	{

	case SF_WINEAGLE:
		pCWinEagle->SetupWizard();
		break;

	case SF_ACF1WH:
		pCACF1WH->SetupWizard();
		break;

	case SF_ACF2WH:
		pCACF2WH->SetupWizard();
		break;

	case SCREEN_ACF_DOWNLOOK:
		pCACF1WH->SetupWizard();
		break;

	case SCREEN_INSP_DOWNLOOK:
		pCACF2WH->SetupWizard();
		break;

	case SCREEN_ACF_UPLOOK:
		pCTA1->SetupWizard();
		break;


	}

	BOOL bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CSettingFile::HMI_WizardSetupBack(IPC_CServiceMessage &svMsg)
{
	CWinEagle *pCWinEagle	= (CWinEagle*)m_pModule->GetStation("WinEagle");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");


	g_bWizardEnableBack = FALSE;
	g_bWizardEnableNext = FALSE;
	g_bWizardEnableExit = FALSE;

	SetHmiVariable("SF_bWizardEnable");
	SetHmiVariable("SF_szWizardStep");
	SetHmiVariable("SF_szWizardMess");
	SetHmiVariable("SF_szWizardMessPrev");
	SetHmiVariable("SF_szWizardMessNext");
	SetHmiVariable("SF_szWizardBMPath");
	SetHmiVariable("SF_bWizardEnableBack");
	SetHmiVariable("SF_bWizardEnableNext");
	SetHmiVariable("SF_bWizardEnableExit");

	if (g_ulWizardStep > 0)
	{
		g_ulWizardStep--;
	}

	switch (g_lWizardInProgress)
	{
	case SF_WIZARD_IDLE:
		g_szWizardTitle = "";
		DisplayMessage("Kill Sunny");
		break;

	case SF_WINEAGLE:
		g_szWizardTitle = "PR Calibration Setup Procedure";
		SetHmiVariable("SF_szWizardTitle");
		pCWinEagle->SetupWizard();
		break;

	case SF_ACF1WH:
		pCACF1WH->SetupWizard();
		break;

	case SF_ACF2WH:
		pCACF2WH->SetupWizard();
		break;

	case SF_INSP1:
		//pCInsp1->SetupWizard();
		break;

	case SF_INSP2:
		//pCInsp2->SetupWizard();
		break;

	case SCREEN_ACF_DOWNLOOK:
		pCACF1WH->SetupWizard();
		break;

	case SCREEN_INSP_DOWNLOOK:
		pCACF2WH->SetupWizard();
		break;

	case SCREEN_ACF_UPLOOK:
		pCTA1->SetupWizard();
		break;
	}

	BOOL bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CSettingFile::HMI_WizardSetupNext(IPC_CServiceMessage &svMsg)
{
	CWinEagle *pCWinEagle	= (CWinEagle*)m_pModule->GetStation("WinEagle");
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1"); 
	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	g_bWizardEnableBack = FALSE;
	g_bWizardEnableNext = FALSE;
	g_bWizardEnableExit = FALSE;

	SetHmiVariable("SF_bWizardEnable");
	SetHmiVariable("SF_szWizardStep");
	SetHmiVariable("SF_szWizardMess");
	SetHmiVariable("SF_szWizardMessPrev");
	SetHmiVariable("SF_szWizardMessNext");
	SetHmiVariable("SF_szWizardBMPath");
	SetHmiVariable("SF_bWizardEnableBack");
	SetHmiVariable("SF_bWizardEnableNext");
	SetHmiVariable("SF_bWizardEnableExit");

	g_ulWizardStep++;

	switch (g_lWizardInProgress)
	{
	case SF_WIZARD_IDLE:
		g_szWizardTitle = "";
		DisplayMessage("Kill Sunny");
		break;

	case SF_WINEAGLE:
		g_szWizardTitle = "PR Calibration Setup Procedure";
		SetHmiVariable("SF_szWizardTitle");
		pCWinEagle->SetupWizard();
		break;

	case SF_ACF1WH:
		pCACF1WH->SetupWizard();
		break;

	case SF_ACF2WH:
		pCACF2WH->SetupWizard();
		break;

	case SCREEN_ACF_DOWNLOOK:
		pCACF1WH->SetupWizard();
		break;

	case SCREEN_INSP_DOWNLOOK:
		pCACF2WH->SetupWizard();
		break;

	case SCREEN_ACF_UPLOOK:
		pCTA1->SetupWizard();
		break;
	}

	BOOL bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CSettingFile::HMI_WizardSetupExit(IPC_CServiceMessage &svMsg)
{
	CWinEagle *pCWinEagle	= (CWinEagle*)m_pModule->GetStation("WinEagle");
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1"); 
	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	g_bWizardEnableBack = FALSE;
	g_bWizardEnableNext = FALSE;
	g_bWizardEnableExit = FALSE;

	SetHmiVariable("SF_bWizardEnable");
	SetHmiVariable("SF_szWizardStep");
	SetHmiVariable("SF_szWizardMess");
	SetHmiVariable("SF_szWizardMessPrev");
	SetHmiVariable("SF_szWizardMessNext");
	SetHmiVariable("SF_szWizardBMPath");
	SetHmiVariable("SF_bWizardEnableBack");
	SetHmiVariable("SF_bWizardEnableNext");
	SetHmiVariable("SF_bWizardEnableExit");

	g_ulWizardStep = 99;

	switch (g_lWizardInProgress)
	{
	case SF_WIZARD_IDLE:
		g_szWizardTitle = "";
		DisplayMessage("Kill Sunny");
		break;

	case SF_WINEAGLE:
		g_szWizardTitle = "PR Calibration Setup Procedure";
		SetHmiVariable("SF_szWizardTitle");
		pCWinEagle->SetupWizard();
		break;

	case SF_ACF1WH:
		pCACF1WH->SetupWizard();
		break;

	case SF_ACF2WH:
		pCACF2WH->SetupWizard();
		break;

	case SCREEN_ACF_DOWNLOOK:
		pCACF1WH->SetupWizard();
		break;

	case SCREEN_INSP_DOWNLOOK:
		pCACF2WH->SetupWizard();
		break;

	case SCREEN_ACF_UPLOOK:
		pCTA1->SetupWizard();
		break;
	}

	pCWinEagle->HMI_bShowCursorControl	= FALSE; //20130823
	g_bWizardEnable			= FALSE;
	g_szWizardTitle			= "";
	g_ulWizardSubTitle		= 0;
	g_szWizardStep			= "";
	g_szWizardMess			= "";
	g_szWizardMessPrev		= "";
	g_szWizardMessNext		= "";
	g_szWizardBMPath		= "";
	g_bWizardEnableBack		= FALSE;
	g_bWizardEnableNext		= FALSE;
	g_bWizardEnableExit		= FALSE;
	g_ulWizardStep			= 0;
	g_lWizardInProgress		= -1;

	BOOL bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

#ifdef PRS_SET_IGNORE_WINDOW_FIRST //20150324
LONG CSettingFile::HMI_WizardSetupPRIgnoreWindows(IPC_CServiceMessage &svMsg)
{
	CWinEagle *pCWinEagle	= (CWinEagle*)m_pModule->GetStation("WinEagle");
	//CInConv	*pCInConv = (CInConv*)m_pModule->GetStation("InConv"); 
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1"); 
	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
	CTA3 *pCTA3	= (CTA3*)m_pModule->GetStation("TA3");
	CTA4 *pCTA4	= (CTA4*)m_pModule->GetStation("TA4");
	CTA5 *pCTA5	= (CTA5*)m_pModule->GetStation("TA5");
	CINWH *pCINWH = (CINWH*)m_pModule->GetStation("INWH"); 
	COutWH *pCOutWH = (COutWH*)m_pModule->GetStation("OutWH"); 
	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");
	CACF2 *pCACF2 = (CACF2*)m_pModule->GetStation("ACF2");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CPB1 *pCPB1	= (CPB1*)m_pModule->GetStation("PB1");
	CPB2 *pCPB2	= (CPB2*)m_pModule->GetStation("PB2");
	CPBUplook *pCPBUplook = (CPBUplook*)m_pModule->GetStation("PBUplook");
	CPreBondWH *pCPreBondWH	= (CPreBondWH*)m_pModule->GetStation("PreBondWH");
	CMB1 *pCMB1 = (CMB1*)m_pModule->GetStation("MB1");
	CMB2 *pCMB2	= (CMB2*)m_pModule->GetStation("MB2");
	CMB3 *pCMB3	= (CMB3*)m_pModule->GetStation("MB3");


	CMB1WH *pCMB1WH	= (CMB1WH*)m_pModule->GetStation("MB1WH");
	CMB2WH	*pCMB2WH	= (CMB2WH*)m_pModule->GetStation("MB2WH");
	CMB3WH *pCMB3WH	= (CMB3WH*)m_pModule->GetStation("MB3WH");
	CMB4WH	*pCMB4WH	= (CMB4WH*)m_pModule->GetStation("MB4WH");
	CMB5WH *pCMB5WH	= (CMB5WH*)m_pModule->GetStation("MB5WH");
	CMB6WH	*pCMB6WH	= (CMB6WH*)m_pModule->GetStation("MB6WH");

	CShuttle *pCShuttle = (CShuttle*)m_pModule->GetStation("Shuttle");
	CFPCPickArm *pCFPCPickArm	= (CFPCPickArm*)m_pModule->GetStation("FPCPickArm");
	CFPCRejectArm *pCFPCRejectArm = (CFPCRejectArm*)m_pModule->GetStation("FPCRejectArm");
	CTrayInElevator *pCTrayInElevator = (CTrayInElevator*)m_pModule->GetStation("TrayInElevator");
	CTrayOutElevator *pCTrayOutElevator = (CTrayOutElevator*)m_pModule->GetStation("TrayOutElevator");
	COutConv *pCOutConv	= (COutConv*)m_pModule->GetStation("OutConv");
	CInsp1	*pCInsp1	= (CInsp1*)m_pModule->GetStation("Insp1");
	CInsp2	*pCInsp2	= (CInsp2*)m_pModule->GetStation("Insp2");
	CPBGlass1PR *pCPBGlass1PR = (CPBGlass1PR*)m_pModule->GetStation("PBGlass1PR");
	CPBGlass2PR *pCPBGlass2PR = (CPBGlass2PR*)m_pModule->GetStation("PBGlass2PR");
	CInSHWH *pCInSHWH = (CInSHWH*)m_pModule->GetStation("InSHWH");
	CRejectArm *pCRejectArm = (CRejectArm*)m_pModule->GetStation("RejectArm");
	CTrayIndexer *pCTrayIndexer = (CTrayIndexer*)m_pModule->GetStation("TrayIndexer");
	CTI1 *pCTI1 = (CTI1*)m_pModule->GetStation("TI1");
	CTI2 *pCTI2 = (CTI2*)m_pModule->GetStation("TI2");


	PRU *pPRU = NULL;

	LONG lSetupInProgress = 0;
	svMsg.GetMsg(sizeof(LONG), &lSetupInProgress);

	HMI_bShowIgnoreWindow = TRUE;
	HMI_bShowIgnoreWindowNextBtn = TRUE;
	SetHmiVariable("SF_bShowIgnoreWindow");
	SetHmiVariable("SF_bShowIgnoreWindowNextBtn");

	g_ulWizardIgnoreWindowStep = 0;

	g_szWizardIgnoreWinTitle.Format("PR Ignore Windows Setup");
	g_szWizardIgnoreWinSubtitle.Format("Please Select Ignore Region #%ld", g_ulWizardIgnoreWindowStep + 1);

	//pCWinEagle->SetPRIgnoreWindowBegin();
	switch (/*lSetupInProgress*/g_lWizardInProgress)
	{
	//case SCREEN_ACF_WH:
	//	// Show PR Screen
	//	pPRU = &pruACF_WHCalib;
	//	pCWinEagle->SetPRIgnoreWindow(pPRU);
	//	break;
	case SF_WIZARD_IDLE:
		g_szWizardTitle = "";
		DisplayMessage("Kill Sunny");
		break;

	case SF_WINEAGLE:
		//pCWinEagle
		break;
	
	case SF_INCONV:
		break;

	case SF_TA1:
		break;

	case SF_TA2:
		break;

	case SF_TA3:
		break;

	case SF_TA4:
#ifdef TA4_PR
		pPRU = &pCTA4->SelectCurPRU(pCTA4->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindow(pPRU);
		}
#endif
		break;

	case SF_TA5:
		break;

	case SF_INWH:
		//pCINWH
		pPRU = &pCINWH->SelectCurPRU(pCINWH->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindow(pPRU);
		}
		break;

	case SF_ACF1:
		break;

	case SF_ACF2:
		break;

	case SF_ACF1WH:
		break;

	case SF_ACF2WH:
		break;

	case SF_PB1:
		//pCPB1
		pPRU = &pCPB1->SelectCurPRU(pCPB1->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindow(pPRU);
		}
		break;

	case SF_PB2:
		//pCPB2
		pPRU = &pCPB2->SelectCurPRU(pCPB2->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindow(pPRU);
		}
		break;

	case SF_PREBONDWH:
		//pCPreBondWH->SetupWizard(); //20130705
		break;

	case SF_PBUPLOOK:
		break;

	case SF_MB1:
		break;

	case SF_MB2:
		break;

	case SF_MB3:
		break;
	
	case SF_MB1WH:
		break;

	case SF_MB2WH:
		break;

	case SF_SH1:
		pCShuttle->m_lCurSlave = SHUTTLE_1;
		//pCShuttle
		pPRU = &pCShuttle->SelectCurPRU(pCShuttle->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindow(pPRU);
		}
		break;

	case SF_SH2:
		pCShuttle->m_lCurSlave = SHUTTLE_2;
		//pCShuttle->SetupWizard();
		pPRU = &pCShuttle->SelectCurPRU(pCShuttle->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindow(pPRU);
		}
		break;

	case SF_FLIPPER:
		break;

	case SF_FPCPICKARM:
		//pCFPCPickArm
		break;

	case SF_TRAYIN:
		//pCTrayInElevator
		break;

	case SF_TRAYOUT:
		//pCTrayOUTElevator
		break;

	case SF_OUTCONV:
		break;

	case SF_INSP1:
		//pCInsp1
		break;

	case SF_INSP2:
		//pCInsp2
		break;

	case SF_PBGLASS1PR:
		//pCPBGlass1PR
		pPRU = &pCPBGlass1PR->SelectCurPRU(pCPBGlass1PR->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindow(pPRU);
		}
		break;

	case SF_PBGLASS2PR:
		//pCPBGlass2PR
		pPRU = &pCPBGlass2PR->SelectCurPRU(pCPBGlass2PR->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindow(pPRU);
		}
		break;

	case SF_TI1:
		//pCTI1
		break;

	case SF_TI2:
		//pCTI2
		break;

	case SF_MBDL:
		//pCTA4->MBDLSetupWizard();
		pPRU = &pCTA4->SelectCurPRU(pCTA4->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindow(pPRU);
		}
		break;


	case SF_ACFUL:
		//pCTA2
		pPRU = &pCTA2->SelectCurPRU(pCTA2->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindow(pPRU);
		}
		break;
	}

	return 0;
}

LONG CSettingFile::HMI_WizardSetupPRIgnoreWindowsNext(IPC_CServiceMessage &svMsg)
{
	CWinEagle *pCWinEagle = (CWinEagle*)m_pModule->GetStation("WinEagle");
	//CInConv	*pCInConv = (CInConv*)m_pModule->GetStation("InConv"); 
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1"); 
	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
	CTA3 *pCTA3	= (CTA3*)m_pModule->GetStation("TA3");
	CTA4 *pCTA4	= (CTA4*)m_pModule->GetStation("TA4");
	CTA5 *pCTA5	= (CTA5*)m_pModule->GetStation("TA5");
	CINWH *pCINWH = (CINWH*)m_pModule->GetStation("INWH"); 
	COutWH *pCOutWH = (COutWH*)m_pModule->GetStation("OutWH"); 
	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");
	CACF2 *pCACF2 = (CACF2*)m_pModule->GetStation("ACF2");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CPB1 *pCPB1	= (CPB1*)m_pModule->GetStation("PB1");
	CPB2 *pCPB2	= (CPB2*)m_pModule->GetStation("PB2");
	CPBUplook *pCPBUplook = (CPBUplook*)m_pModule->GetStation("PBUplook");
	CPreBondWH *pCPreBondWH	= (CPreBondWH*)m_pModule->GetStation("PreBondWH");
	CMB1 *pCMB1 = (CMB1*)m_pModule->GetStation("MB1");
	CMB2 *pCMB2	= (CMB2*)m_pModule->GetStation("MB2");
	CMB3 *pCMB3	= (CMB3*)m_pModule->GetStation("MB3");


	CMB1WH *pCMB1WH	= (CMB1WH*)m_pModule->GetStation("MB1WH");
	CMB2WH	*pCMB2WH	= (CMB2WH*)m_pModule->GetStation("MB2WH");
	CMB3WH *pCMB3WH	= (CMB3WH*)m_pModule->GetStation("MB3WH");
	CMB4WH	*pCMB4WH	= (CMB4WH*)m_pModule->GetStation("MB4WH");
	CMB5WH *pCMB5WH	= (CMB5WH*)m_pModule->GetStation("MB5WH");
	CMB6WH	*pCMB6WH	= (CMB6WH*)m_pModule->GetStation("MB6WH");

	CShuttle *pCShuttle = (CShuttle*)m_pModule->GetStation("Shuttle");
	CFPCPickArm *pCFPCPickArm	= (CFPCPickArm*)m_pModule->GetStation("FPCPickArm");
	CFPCRejectArm *pCFPCRejectArm = (CFPCRejectArm*)m_pModule->GetStation("FPCRejectArm");
	CTrayInElevator *pCTrayInElevator = (CTrayInElevator*)m_pModule->GetStation("TrayInElevator");
	CTrayOutElevator *pCTrayOutElevator = (CTrayOutElevator*)m_pModule->GetStation("TrayOutElevator");
	COutConv *pCOutConv	= (COutConv*)m_pModule->GetStation("OutConv");
	CInsp1	*pCInsp1	= (CInsp1*)m_pModule->GetStation("Insp1");
	CInsp2	*pCInsp2	= (CInsp2*)m_pModule->GetStation("Insp2");
	CPBGlass1PR *pCPBGlass1PR = (CPBGlass1PR*)m_pModule->GetStation("PBGlass1PR");
	CPBGlass2PR *pCPBGlass2PR = (CPBGlass2PR*)m_pModule->GetStation("PBGlass2PR");
	CInSHWH *pCInSHWH = (CInSHWH*)m_pModule->GetStation("InSHWH");
	CRejectArm *pCRejectArm = (CRejectArm*)m_pModule->GetStation("RejectArm");
	CTrayIndexer *pCTrayIndexer = (CTrayIndexer*)m_pModule->GetStation("TrayIndexer");
	CTI1 *pCTI1 = (CTI1*)m_pModule->GetStation("TI1");
	CTI2 *pCTI2 = (CTI2*)m_pModule->GetStation("TI2");

	LONG lModuleInProgress = 0;
	PRU *pPRU = NULL;
	CString	szMsg = _T(" ");

	svMsg.GetMsg(sizeof(LONG), &lModuleInProgress);

	if (g_ulWizardIgnoreWindowStep > MAX_PR_IGNORE_WINDOWS - 1)
	{
		szMsg.Format("Error: Number of Ignore Windows exceed %d. Operation Abort!", MAX_PR_IGNORE_WINDOWS);
		DisplayMessage(szMsg);
		szMsg.Format("Number of Ignore Windows exceed %d. Operation Abort!", MAX_PR_IGNORE_WINDOWS);
		HMIMessageBox(MSG_OK, "WARNING", szMsg);
		return 0;
	}
	g_ulWizardIgnoreWindowStep++;
	//no more than 8 window
	if (g_ulWizardIgnoreWindowStep >= MAX_PR_IGNORE_WINDOWS)
	{
	//done
		HMI_bShowIgnoreWindowNextBtn = FALSE;
		SetHmiVariable("SF_bShowIgnoreWindowNextBtn");
		g_szWizardIgnoreWinSubtitle.Format("Max %ld Ignore Region Selected. Done", g_ulWizardIgnoreWindowStep);
	}
	else
	{
		g_szWizardIgnoreWinSubtitle.Format("Please Select Ignore Region #%ld", g_ulWizardIgnoreWindowStep + 1);
	}

	switch (/*lModuleInProgress*/g_lWizardInProgress)
	{

	case SF_WIZARD_IDLE:
		DisplayMessage("Kill Sunny");
		break;

	case SF_WINEAGLE:
		//pCWinEagle
		break;
	
	case SF_INCONV:
		break;

	case SF_TA1:
		break;

	case SF_TA2:
		break;

	case SF_TA3:
		break;

	case SF_TA4:
#ifdef TA4_PR
		pPRU = &pCTA4->SelectCurPRU(pCTA4->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowNext(pPRU);
		}
#endif
		break;

	case SF_TA5:
		break;

	case SF_INWH:
		//pCINWH
		pPRU = &pCINWH->SelectCurPRU(pCINWH->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowNext(pPRU);
		}
		break;

	case SF_ACF1:
		break;

	case SF_ACF2:
		break;

	case SF_ACF1WH:
		break;

	case SF_ACF2WH:
		break;

	case SF_PB1:
		//pCPB1
		pPRU = &pCPB1->SelectCurPRU(pCPB1->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowNext(pPRU);
		}
		break;

	case SF_PB2:
		//pCPB2
		pPRU = &pCPB2->SelectCurPRU(pCPB2->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowNext(pPRU);
		}
		break;

	case SF_PREBONDWH:
		//pCPreBondWH->SetupWizard(); //20130705
		break;

	case SF_PBUPLOOK:
		break;

	case SF_MB1:
		break;

	case SF_MB2:
		break;

	case SF_MB3:
		break;
	
	case SF_MB1WH:
		break;

	case SF_MB2WH:
		break;

	case SF_SH1:
		pCShuttle->m_lCurSlave = SHUTTLE_1;
		//pCShuttle
		pPRU = &pCShuttle->SelectCurPRU(pCShuttle->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowNext(pPRU);
		}
		break;

	case SF_SH2:
		pCShuttle->m_lCurSlave = SHUTTLE_2;
		//pCShuttle->SetupWizard();
		pPRU = &pCShuttle->SelectCurPRU(pCShuttle->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowNext(pPRU);
		}
		break;

	case SF_FLIPPER:
		break;

	case SF_FPCPICKARM:
		//pCFPCPickArm
		break;

	case SF_TRAYIN:
		//pCTrayInElevator
		break;

	case SF_TRAYOUT:
		//pCTrayOUTElevator
		break;

	case SF_OUTCONV:
		break;

	case SF_INSP1:
		//pCInsp1
		break;

	case SF_INSP2:
		//pCInsp2
		break;

	case SF_PBGLASS1PR:
		//pCPBGlass1PR
		pPRU = &pCPBGlass1PR->SelectCurPRU(pCPBGlass1PR->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowNext(pPRU);
		}
		break;

	case SF_PBGLASS2PR:
		//pCPBGlass2PR
		pPRU = &pCPBGlass2PR->SelectCurPRU(pCPBGlass2PR->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowNext(pPRU);
		}
		break;

	case SF_TI1:
		//pCTI1
		break;

	case SF_TI2:
		//pCTI2
		break;

	case SF_MBDL:
		//pCTA4->MBDLSetupWizard();
		pPRU = &pCTA4->SelectCurPRU(pCTA4->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowNext(pPRU);
		}
		break;


	case SF_ACFUL:
		//pCTA2
		pPRU = &pCTA2->SelectCurPRU(pCTA2->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowNext(pPRU);
		}
		break;

	}
	return 0;
}

LONG CSettingFile::HMI_WizardSetupPRIgnoreWindowsExit(IPC_CServiceMessage &svMsg)
{
	CWinEagle *pCWinEagle = (CWinEagle*)m_pModule->GetStation("WinEagle");
	//CInConv	*pCInConv = (CInConv*)m_pModule->GetStation("InConv"); 
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1"); 
	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
	CTA3 *pCTA3	= (CTA3*)m_pModule->GetStation("TA3");
	CTA4 *pCTA4	= (CTA4*)m_pModule->GetStation("TA4");
	CTA5 *pCTA5	= (CTA5*)m_pModule->GetStation("TA5");
	CINWH *pCINWH = (CINWH*)m_pModule->GetStation("INWH"); 
	COutWH *pCOutWH = (COutWH*)m_pModule->GetStation("OutWH"); 
	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");
	CACF2 *pCACF2 = (CACF2*)m_pModule->GetStation("ACF2");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CPB1 *pCPB1	= (CPB1*)m_pModule->GetStation("PB1");
	CPB2 *pCPB2	= (CPB2*)m_pModule->GetStation("PB2");
	CPBUplook *pCPBUplook = (CPBUplook*)m_pModule->GetStation("PBUplook");
	CPreBondWH *pCPreBondWH	= (CPreBondWH*)m_pModule->GetStation("PreBondWH");
	CMB1 *pCMB1 = (CMB1*)m_pModule->GetStation("MB1");
	CMB2 *pCMB2	= (CMB2*)m_pModule->GetStation("MB2");
	CMB3 *pCMB3	= (CMB3*)m_pModule->GetStation("MB3");


	CMB1WH *pCMB1WH	= (CMB1WH*)m_pModule->GetStation("MB1WH");
	CMB2WH	*pCMB2WH	= (CMB2WH*)m_pModule->GetStation("MB2WH");
	CMB3WH *pCMB3WH	= (CMB3WH*)m_pModule->GetStation("MB3WH");
	CMB4WH	*pCMB4WH	= (CMB4WH*)m_pModule->GetStation("MB4WH");
	CMB5WH *pCMB5WH	= (CMB5WH*)m_pModule->GetStation("MB5WH");
	CMB6WH	*pCMB6WH	= (CMB6WH*)m_pModule->GetStation("MB6WH");

	CShuttle *pCShuttle = (CShuttle*)m_pModule->GetStation("Shuttle");
	CFPCPickArm *pCFPCPickArm	= (CFPCPickArm*)m_pModule->GetStation("FPCPickArm");
	CFPCRejectArm *pCFPCRejectArm = (CFPCRejectArm*)m_pModule->GetStation("FPCRejectArm");
	CTrayInElevator *pCTrayInElevator = (CTrayInElevator*)m_pModule->GetStation("TrayInElevator");
	CTrayOutElevator *pCTrayOutElevator = (CTrayOutElevator*)m_pModule->GetStation("TrayOutElevator");
	COutConv *pCOutConv	= (COutConv*)m_pModule->GetStation("OutConv");
	CInsp1	*pCInsp1	= (CInsp1*)m_pModule->GetStation("Insp1");
	CInsp2	*pCInsp2	= (CInsp2*)m_pModule->GetStation("Insp2");
	CPBGlass1PR *pCPBGlass1PR = (CPBGlass1PR*)m_pModule->GetStation("PBGlass1PR");
	CPBGlass2PR *pCPBGlass2PR = (CPBGlass2PR*)m_pModule->GetStation("PBGlass2PR");
	CInSHWH *pCInSHWH = (CInSHWH*)m_pModule->GetStation("InSHWH");
	CRejectArm *pCRejectArm = (CRejectArm*)m_pModule->GetStation("RejectArm");
	CTrayIndexer *pCTrayIndexer = (CTrayIndexer*)m_pModule->GetStation("TrayIndexer");
	CTI1 *pCTI1 = (CTI1*)m_pModule->GetStation("TI1");
	CTI2 *pCTI2 = (CTI2*)m_pModule->GetStation("TI2");

	LONG lModuleInProgress = 0;
	PRU *pPRU = NULL;

	HMI_bShowIgnoreWindow = FALSE;
	HMI_bShowIgnoreWindowBtn = TRUE;
	HMI_bShowIgnoreWindowNextBtn = TRUE;
	svMsg.GetMsg(sizeof(LONG), &lModuleInProgress);

	switch (/*lModuleInProgress*//*g_lSetupInProgress*/g_lWizardInProgress)
	{
	//case SCREEN_ACF_WH:
	//	// Show PR Screen
	//	pPRU = &pruACF_WHCalib;
	//	pCWinEagle->SetPRIgnoreWindowFinish(pPRU);
	//	break;
	case SF_WIZARD_IDLE:
		DisplayMessage("Kill Sunny");
		break;

	case SF_WINEAGLE:
		//pCWinEagle
		break;
	
	case SF_INCONV:
		break;

	case SF_TA1:
		break;

	case SF_TA2:
		break;

	case SF_TA3:
		break;

	case SF_TA4:
#ifdef TA4_PR
		pPRU = &pCTA4->SelectCurPRU(pCTA4->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowFinish(pPRU);
		}
#endif
		break;

	case SF_TA5:
		break;

	case SF_INWH:
		//pCINWH
		pPRU = &pCINWH->SelectCurPRU(pCINWH->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowFinish(pPRU);
		}
		break;

	case SF_ACF1:
		break;

	case SF_ACF2:
		break;

	case SF_ACF1WH:
		break;

	case SF_ACF2WH:
		break;

	case SF_PB1:
		//pCPB1
		pPRU = &pCPB1->SelectCurPRU(pCPB1->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowFinish(pPRU);
		}
		break;

	case SF_PB2:
		//pCPB2
		pPRU = &pCPB2->SelectCurPRU(pCPB2->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowFinish(pPRU);
		}
		break;

	case SF_PREBONDWH:
		//pCPreBondWH->SetupWizard(); //20130705
		break;

	case SF_PBUPLOOK:
		break;

	case SF_MB1:
		break;

	case SF_MB2:
		break;

	case SF_MB3:
		break;
	
	case SF_MB1WH:
		break;

	case SF_MB2WH:
		break;

	case SF_SH1:
		pCShuttle->m_lCurSlave = SHUTTLE_1;
		//pCShuttle
		pPRU = &pCShuttle->SelectCurPRU(pCShuttle->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowFinish(pPRU);
		}
		break;

	case SF_SH2:
		pCShuttle->m_lCurSlave = SHUTTLE_2;
		//pCShuttle->SetupWizard();
		pPRU = &pCShuttle->SelectCurPRU(pCShuttle->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowFinish(pPRU);
		}
		break;

	case SF_FLIPPER:
		break;

	case SF_FPCPICKARM:
		//pCFPCPickArm
		break;

	case SF_TRAYIN:
		//pCTrayInElevator
		break;

	case SF_TRAYOUT:
		//pCTrayOUTElevator
		break;

	case SF_OUTCONV:
		break;

	case SF_INSP1:
		//pCInsp1
		break;

	case SF_INSP2:
		//pCInsp2
		break;

	case SF_PBGLASS1PR:
		//pCPBGlass1PR
		pPRU = &pCPBGlass1PR->SelectCurPRU(pCPBGlass1PR->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowFinish(pPRU);
		}
		break;

	case SF_PBGLASS2PR:
		//pCPBGlass2PR
		pPRU = &pCPBGlass2PR->SelectCurPRU(pCPBGlass2PR->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowFinish(pPRU);
		}
		break;

	case SF_TI1:
		//pCTI1
		break;

	case SF_TI2:
		//pCTI2
		break;

	case SF_MBDL:
		//pCTA4->MBDLSetupWizard();
		pPRU = &pCTA4->SelectCurPRU(pCTA4->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowFinish(pPRU);
		}
		break;


	case SF_ACFUL:
		//pCTA2
		pPRU = &pCTA2->SelectCurPRU(pCTA2->HMI_ulCurPRU);
		if (pPRU != NULL)
		{
			pCWinEagle->SetPRIgnoreWindowFinish(pPRU);
		}
		break;

	}

	return 0;
}
#endif

LONG CSettingFile::HMI_ModuleSetupPreTask(IPC_CServiceMessage &svMsg)
{
	CWinEagle *pCWinEagle	= (CWinEagle*)m_pModule->GetStation("WinEagle");
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1"); 
	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CInPickArm *pCInPickArm = (CInPickArm*)m_pModule->GetStation("InPickArm");


	PRU *pPRU = NULL;

	LONG lModuleInProgress = 0;
	BOOL bShowLighting = FALSE;
	INT	 nTmpCam = 0;

	svMsg.GetMsg(sizeof(LONG), &lModuleInProgress);

	g_lSetupInProgress = lModuleInProgress;

	switch (g_lSetupInProgress)
	{
	case SCREEN_IDLE:
		DisplayMessage("Kill JASON");
		break;

	case SCREEN_WINEAGLE:
		pCWinEagle->ModulePRSetupPreTask();
		break;

	case SCREEN_INPA:
		pCInPickArm->ModuleSetupPreTask();
		break;

	case SCREEN_TA1:
		PRS_DisplayVideo(&pruACFUL_Calib);
		PRS_DrawHomeCursor(ACFUL_CAM, FALSE);	
		pCWinEagle->UpdateLighting(&pruACFUL_Calib);
		pCTA1->ModuleSetupPreTask();
		break;
	
	case SCREEN_TA2:
		pCTA2->ModuleSetupPreTask();	
		break;

	case SCREEN_ACF1:
		pCACF1->ModuleSetupPreTask();
		break;

	case SCREEN_ACF1WH:
		PRS_DisplayVideo(&pruACF_WHCalib);
		PRS_DrawHomeCursor(ACF_CAM, FALSE);	
		pCWinEagle->UpdateLighting(&pruACF_WHCalib);
		pCACF1WH->ModuleSetupPreTask();
		break;

	case SCREEN_ACF2WH:
		PRS_DisplayVideo(&pruACF_WHCalib);
		PRS_DrawHomeCursor(ACF_CAM, FALSE);	
		pCWinEagle->UpdateLighting(&pruACF_WHCalib);
		pCACF2WH->ModuleSetupPreTask();
		break;

	case SCREEN_INSP1:
		break;

	case SCREEN_INSP2:
		break;

	case SCREEN_ACF_DOWNLOOK:
		PRS_DisplayVideo(&pruACF_WHCalib);
		PRS_DrawHomeCursor(ACF_CAM, FALSE);	
		pCWinEagle->UpdateLighting(&pruACF_WHCalib);
		pCACF1WH->ModuleSetupPreTask();
		pCACF2WH->ModuleSetupPreTask();
		break;

	case SCREEN_INSP_DOWNLOOK:
		// Show PR Screen
		PRS_DisplayVideo(&pruACF_WHCalib);
		PRS_DrawHomeCursor(ACF_CAM, FALSE);	
		pCWinEagle->UpdateLighting(&pruACF_WHCalib);
		pCACF1WH->ModuleSetupPreTask();
		pCACF2WH->ModuleSetupPreTask();
		break;

	case SCREEN_ACF_UPLOOK:
		// Show PR Screen
		PRS_DisplayVideo(&pruACFUL_Calib);
		PRS_DrawHomeCursor(ACFUL_CAM, FALSE);	
		pCWinEagle->UpdateLighting(&pruACFUL_Calib);
		pCTA1->ModuleSetupPreTask();
		break;

	}
	
	BOOL bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CSettingFile::HMI_ModuleSetupPostTask(IPC_CServiceMessage &svMsg)
{
	CWinEagle *pCWinEagle	= (CWinEagle*)m_pModule->GetStation("WinEagle");
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1"); 
	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CInPickArm *pCInPickArm = (CInPickArm*)m_pModule->GetStation("InPickArm");

	PRU *pPRU = NULL;

	LONG lModuleInProgress = 0;
	BOOL bShowLighting = FALSE;
	INT	 nTmpCam = 0;

	svMsg.GetMsg(sizeof(LONG), &lModuleInProgress);

	g_lSetupInProgress = lModuleInProgress;

	switch (g_lSetupInProgress)
	{
	case SCREEN_IDLE:
		DisplayMessage("Kill JASON");
		break;

	case SCREEN_WINEAGLE:
		pCWinEagle->ModulePRSetupPostTask();
		break;

	case SCREEN_INPA:
		pCInPickArm->ModuleSetupPostTask();
		break;

	case SCREEN_TA1:
		pCTA1->ModuleSetupPostTask();
		break;
	
	case SCREEN_TA2:
		pCTA2->ModuleSetupPostTask();	
		break;

	case SCREEN_ACF1:
		pCACF1->ModuleSetupPostTask();
		break;

	case SCREEN_ACF1WH:
		pCACF1WH->ModuleSetupPostTask();
		break;

	case SCREEN_ACF2WH:
		pCACF2WH->ModuleSetupPostTask();
		break;

	case SCREEN_INSP1:
		break;

	case SCREEN_INSP2:
		break;

	case SCREEN_ACF_DOWNLOOK:
		pCACF1WH->ModulePRSetupPostTask();
		pCACF2WH->ModulePRSetupPostTask();
		break;

	case SCREEN_INSP_DOWNLOOK:
		pCACF1WH->ModulePRSetupPostTask();
		pCACF2WH->ModulePRSetupPostTask();
		break;

	case SCREEN_ACF_UPLOOK:
		pCTA1->ModulePRSetupPostTask();
		break;
	}
	
	BOOL bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CSettingFile::HMI_ModuleSetupSetPosn(IPC_CServiceMessage &svMsg)
{
	CWinEagle *pCWinEagle	= (CWinEagle*)m_pModule->GetStation("WinEagle");
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1"); 
	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CInPickArm *pCInPickArm = (CInPickArm*)m_pModule->GetStation("InPickArm");

	LONG lSetupTitle = 0;
	BOOL bResult = TRUE;

	svMsg.GetMsg(sizeof(LONG), &lSetupTitle);

	if(g_stSetupPosn[lSetupTitle].lType == MACHINE_PARA && g_ulUserGroup < SERVICE) //20150518
	{
		HMIMessageBox(MSG_OK, "WARNING", "You are not allowed to change Machine Position");
		BOOL bMode = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bMode);
		return 1;
	}

	switch (g_lSetupInProgress)
	{
	case SCREEN_IDLE:
		DisplayMessage("Kill JASON");
		break;

	case SCREEN_INPA:
		pCInPickArm->ModuleSetupSetPosn(lSetupTitle);
		break;

	case SCREEN_TA1:
		pCTA1->ModuleSetupSetPosn(lSetupTitle);
		break;
	
	case SCREEN_TA2:
		pCTA2->ModuleSetupSetPosn(lSetupTitle);
		break;

	case SCREEN_ACF1:
		pCACF1->ModuleSetupSetPosn(lSetupTitle);
		break;

	case SCREEN_ACF1WH:
		pCACF1WH->ModuleSetupSetPosn(lSetupTitle);
		break;

	case SCREEN_ACF2WH:
		pCACF2WH->ModuleSetupSetPosn(lSetupTitle);
		break;

	case SCREEN_INSP1:
		break;

	case SCREEN_INSP2:
		break;
	}
	
	BOOL bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CSettingFile::HMI_ModuleSetupGoPosn(IPC_CServiceMessage &svMsg)
{
	CWinEagle *pCWinEagle	= (CWinEagle*)m_pModule->GetStation("WinEagle");
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1"); 
	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CInPickArm *pCInPickArm = (CInPickArm*)m_pModule->GetStation("InPickArm");


	LONG lSetupTitle = 0;
	BOOL bResult = TRUE;

	svMsg.GetMsg(sizeof(LONG), &lSetupTitle);

	switch (g_lSetupInProgress)
	{
	case SCREEN_IDLE:
		DisplayMessage("Kill JASON");
		break;

	case SCREEN_INPA:
		pCInPickArm->ModuleSetupGoPosn(lSetupTitle);
		break;

	case SCREEN_TA1:
		pCTA1->ModuleSetupGoPosn(lSetupTitle);
		break;
	
	case SCREEN_TA2:
		pCTA2->ModuleSetupGoPosn(lSetupTitle);
		break;

	case SCREEN_ACF1:
		pCACF1->ModuleSetupGoPosn(lSetupTitle);
		break;

	case SCREEN_ACF1WH:
		pCACF1WH->ModuleSetupGoPosn(lSetupTitle);
		break;

	case SCREEN_ACF2WH:
		pCACF2WH->ModuleSetupGoPosn(lSetupTitle);
		break;

	case SCREEN_INSP1:
		break;

	case SCREEN_INSP2:
		break;
	}
	
	BOOL bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CSettingFile::HMI_ModuleSetupSetLevel(IPC_CServiceMessage &svMsg)
{
	CWinEagle *pCWinEagle	= (CWinEagle*)m_pModule->GetStation("WinEagle");
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1"); 
	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CInPickArm *pCInPickArm = (CInPickArm*)m_pModule->GetStation("InPickArm");

	LONG lSetupTitle = 0;
	BOOL bResult = TRUE;

	svMsg.GetMsg(sizeof(LONG), &lSetupTitle);

	if(g_stSetupLevel[lSetupTitle].lType == MACHINE_PARA && g_ulUserGroup < SERVICE) //20150518
	{
		HMIMessageBox(MSG_OK, "WARNING", "You are not allowed to change Machine Level");
		BOOL bMode = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bMode);
		return 1;
	}

	switch (g_lSetupInProgress)
	{
	case SCREEN_IDLE:
		DisplayMessage("Kill JASON");
		break;

	case SCREEN_INPA:
		pCInPickArm->ModuleSetupSetLevel(lSetupTitle);
		break;

	case SCREEN_TA1:
		pCTA1->ModuleSetupSetLevel(lSetupTitle);
		break;

	case SCREEN_TA2:
		pCTA2->ModuleSetupSetLevel(lSetupTitle);
		break;

	case SCREEN_ACF1:
		break;

	case SCREEN_ACF1WH:
		//pCACF1WH->ModuleSetupSetLevel(lSetupTitle);	
		break;

	case SCREEN_ACF2WH:
		//pCACF2WH->ModuleSetupSetLevel(lSetupTitle);	
		break;

	case SCREEN_INSP1:
		break;

	case SCREEN_INSP2:
		break;
	}
	
	BOOL bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CSettingFile::HMI_ModuleSetupGoLevel(IPC_CServiceMessage &svMsg)
{
	CWinEagle *pCWinEagle	= (CWinEagle*)m_pModule->GetStation("WinEagle");
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1"); 
	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CInPickArm *pCInPickArm = (CInPickArm*)m_pModule->GetStation("InPickArm");

	LONG lSetupTitle = 0;
	BOOL bResult = TRUE;

	svMsg.GetMsg(sizeof(LONG), &lSetupTitle);

	switch (g_lSetupInProgress)
	{
	case SCREEN_IDLE:
		DisplayMessage("Kill JASON");
		break;

	case SCREEN_INPA:
		pCInPickArm->ModuleSetupGoLevel(lSetupTitle);	
		break;

	case SCREEN_TA1:
		pCTA1->ModuleSetupGoLevel(lSetupTitle);	
		break;
	
	case SCREEN_TA2:
		pCTA2->ModuleSetupGoLevel(lSetupTitle);	
		break;

	case SCREEN_ACF1:
		break;

	case SCREEN_ACF1WH:
		//pCACF1WH->ModuleSetupGoLevel(lSetupTitle);
		break;

	case SCREEN_ACF2WH:
		//pCACF2WH->ModuleSetupGoLevel(lSetupTitle);	
		break;

	case SCREEN_INSP1:
		break;

	case SCREEN_INSP2:
		break;
	}
	
	BOOL bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CSettingFile::HMI_ModuleSetupSetPara(IPC_CServiceMessage &svMsg)
{	
	CWinEagle *pCWinEagle	= (CWinEagle*)m_pModule->GetStation("WinEagle");
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1"); 
	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CInPickArm *pCInPickArm = (CInPickArm*)m_pModule->GetStation("InPickArm");


	PARA stPara;

	stPara.lSetupTitle = 0;
	stPara.lPara = 0;
	stPara.dPara = 0;

	LONG lSetupTitle = 0;
	
	CString szDesiredStnName;
	INT nDesiredStn = 0;

	svMsg.GetMsg(sizeof(PARA), &stPara);

	if(g_stSetupPara[stPara.lSetupTitle].lType == MACHINE_PARA && g_ulUserGroup < SERVICE) //20150518
	{
		HMIMessageBox(MSG_OK, "WARNING", "You are not allowed to change Machine Parameter");
		BOOL bMode = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bMode);
		return 1;
	}

	switch (g_lSetupInProgress)
	{
	case SCREEN_IDLE:
		DisplayMessage("Kill JASON");
		break;

	case SCREEN_INPA:
		pCInPickArm->ModuleSetupSetPara(stPara);
		break;

	case SCREEN_TA1:
		pCTA1->ModuleSetupSetPara(stPara);
		break;
	
	case SCREEN_TA2:
		pCTA2->ModuleSetupSetPara(stPara);	
		break;

	case SCREEN_ACF1:
		//pCACF2->ModuleSetupSetPara(stPara);	
		pCACF1->ModuleSetupSetPara(stPara);
		
		break;

	case SCREEN_ACF1WH:
	case SCREEN_ACF2WH:
		pCACF2WH->ModuleSetupSetPara(stPara);
		pCACF1WH->ModuleSetupSetPara(stPara);
		break;

	case SCREEN_INSP1:
		break;

	case SCREEN_INSP2:
		break;
	}
	
	BOOL bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

//LONG CSettingFile::HMI_CalcWHZLinearEncoder(IPC_CServiceMessage &svMsg) //whz rotary linear calib
//{	
//	CWinEagle *pCWinEagle	= (CWinEagle*)m_pModule->GetStation("WinEagle");
//	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1"); 
//	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
//	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");
//	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
//	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
//	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
//
//	LONG	lMsgLen = svMsg.GetMsgLen();
//	UCHAR	*cName = NULL;
//	CString szMsg;
//
//	if (lMsgLen > 0)
//	{
//		cName = new UCHAR[lMsgLen];
//		svMsg.GetMsg(cName, lMsgLen);
//	}
//	
//	//if (pAppMod->m_CurrMotor == "MB1WH_Z")
//	//{
//	//	pCMB1WH->CalcWHZLinearEncoder();
//	//}
//	//else if (pAppMod->m_CurrMotor == "MB2WH_Z")
//	//{
//	//	pCMB2WH->CalcWHZLinearEncoder();
//	//}
//	//else
//	//{
//	//	szMsg.Format("%s Module Not Found. Operation Abort!", cName == NULL ? "NULL" : (char*)cName);
//	//	HMIMessageBox(MSG_OK, "CALC WHZ LINEAR ENCODER", szMsg);
//	//}
//
//	if (cName != NULL)
//	{
//		delete[] cName;
//	}
//	BOOL bMode = TRUE;
//	svMsg.InitMessage(sizeof(BOOL), &bMode);
//	return 1;
//}

//LONG CSettingFile::HMI_SearchWHZLimits(IPC_CServiceMessage &svMsg)	// 20141104 Yip: Add Functions For Searching Z Limits
//{
//	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
//	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
//	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
//
//	LONG	lMsgLen = svMsg.GetMsgLen();
//	UCHAR	*cName = NULL;
//	CString szMsg;
//
//	if (lMsgLen > 0)
//	{
//		cName = new UCHAR[lMsgLen];
//		svMsg.GetMsg(cName, lMsgLen);
//	}
//	//if (pAppMod->m_CurrMotor == "ACF1WH_Z")
//	//{
//	//	pCACF1WH->SearchZ1Limits();
//	//}
//	//else if (pAppMod->m_CurrMotor == "ACF2WH_Z")
//	//{
//	//	pCACF2WH->SearchZ1Limits();
//	//}
//	//else
//	//{
//	//	szMsg.Format("%s Module Not Found. Operation Abort!", cName == NULL ? "NULL" : (char*)cName);
//	//	HMIMessageBox(MSG_OK, "SEARCH WHZ LIMITS", szMsg);
//	//}
//
//	if (cName != NULL)
//	{
//		delete[] cName;
//	}
//	BOOL bMode = TRUE;
//	svMsg.InitMessage(sizeof(BOOL), &bMode);
//	return 1;
//}
