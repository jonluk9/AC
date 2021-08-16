/////////////////////////////////////////////////////////////////
//	InspOpt.cpp : interface of the CInspOpt class
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

#include "Cal_Util.h"

#include "HouseKeeping.h"

#define INSPOPT_EXTERN
#include "InspOpt.h"
#include "ACFWH.h"
#include "ACF1WH.h"
#include "ACF2WH.h"
#include "WinEagle.h"
#include "HouseKeeping.h"
#include "SettingFile.h"
#include "TimeChart.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CInspOpt, CAC9000Stn)

CInspOpt::CInspOpt()
{
	// CPI9000Srn
	//m_lStnErrorCode		= IDS_INSPOPT_MOT_NOT_SELECTED_ERR;
	//m_lModSelectMode	= GLASS_MOD;

	//m_szPRTaskStnName	= GetStation().Trim('_');
	//m_lPRStnErrorCode	= IDS_INSPOPT_PR_MOD_NOT_SELECTED;
	//m_lPRModSelectMode	= GLASS_MOD;

	INT i = 0;

	// Motor Related
	m_stMotorX.InitMotorInfo("INSPOPT_X", &m_bModSelected);
	m_myMotorList.Add((CMotorInfo*)&m_stMotorX);

	// Si Related
	m_stXLmtSnr		= CSiInfo("INSPDLXHomeSnr",		"IOMODULE_IN_PORT_6", BIT_10, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stXLmtSnr);

	// So Related

	m_mtrStandbyPosn.x			= 0;
	m_mtrStandbyPosn.y			= 0;
}

CInspOpt::~CInspOpt()
{
}

BOOL CInspOpt::InitInstance()
{
	CAC9000Stn::InitInstance();

	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CGmpSystem *pGmpSys = pAppMod->m_pCGmpSystem;
	CString		szMsg	= _T("");

	INT i = 0;

	try
	{
		// Acutators
		//if (pAppMod->IsInitNuDrive())
		//{
		for (i = 0; i < m_myMotorList.GetSize(); i++)
		{
				// Initi Motor Configuration
			GetmyMotorList().GetAt(i)->SetAppStation(pAppMod);
			GetmyMotorList().GetAt(i)->SetStnName(GetStnName());
			if (pAppMod->IsInitNuDrive())
			{
				GetmyMotorList().GetAt(i)->SetGmpChannel(pGmpSys->AddChannel(m_myMotorList.GetAt(i)->stAttrib.szAxis, m_myMotorList.GetAt(i)->stAttrib.dDistPerCount));
				GetmyMotorList().GetAt(i)->InitMtrAxis();
			}
		}
		//}

		if (pAppMod->IsInitNuDrive())
		{
			// SI
			for (i = 0; i < m_mySiList.GetSize(); i++)
			{
				GetmySiList().GetAt(i)->SetGmpPort(
					pGmpSys->AddSiPort(m_mySiList.GetAt(i)->GetPortName(), 
									   m_mySiList.GetAt(i)->GetMask(), 
									   m_mySiList.GetAt(i)->GetActiveState()));
			}

			// SO
			for (i = 0; i < m_mySoList.GetSize(); i++)
			{
				GetmySoList().GetAt(i)->SetGmpPort(
					pGmpSys->AddSoPort(m_mySoList.GetAt(i)->GetPortName(), 
									   m_mySoList.GetAt(i)->GetMask(), 
									   m_mySoList.GetAt(i)->GetActiveState()));

				GetmySoList().GetAt(i)->GetGmpPort().Off();
				GetmySoList().GetAt(i)->bHMI_Status = GetmySoList().GetAt(i)->GetGmpPort().IsOn();				
			}
		}
	}
	catch (CAsmException e)
	{
		pAppMod->m_bHWInitError = TRUE;

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	while (pAppMod->m_bHWInitCheckingPressure) //20130711
	{
		Sleep(100);
	}

	if (m_fHardware && !pAppMod->m_bHWInitError && !pAppMod->m_bDiagnMode)
	{
		szMsg = _T("	") + GetName() + " --- commutating servos";
		DisplayMessage(szMsg);

		// Commutate Motor
		for (i = 0; i < m_myMotorList.GetSize(); i++)
		{
			if (GetmyMotorList().GetAt(i)->Commutate() != GMP_SUCCESS)
			{
				m_bInitInstanceError			= TRUE;
				pAppMod->m_bHWInitError			= TRUE;
			}
		}

		pAppMod->m_bInspOptComm	= TRUE;

		// wait for all servo commutated (even with error)
		do
		{
			Sleep(100);
		} while (!pAppMod->IsAllServoCommutated());

		if (!m_bInitInstanceError)
		{
			szMsg = _T("	") + GetName() + " --- homing motors";
			DisplayMessage(szMsg);

			// Home Motor
			for (i = 0; i < m_myMotorList.GetSize(); i++)
			{
				if (GetmyMotorList().GetAt(i)->Home(GMP_NOWAIT) != GMP_SUCCESS)
				{
					m_bInitInstanceError	= TRUE;
					pAppMod->m_bHWInitError = TRUE;
				}
			}

			// Sync & Enable Protection
			for (i = 0; i < m_myMotorList.GetSize(); i++)
			{
				if (
					GetmyMotorList().GetAt(i)->Sync() != GMP_SUCCESS ||
					GetmyMotorList().GetAt(i)->EnableProtection() != GMP_SUCCESS
				   )
				{
					m_bInitInstanceError	= TRUE;
					pAppMod->m_bHWInitError = TRUE;
				}
				else
				{
					GetmyMotorList().GetAt(i)->bHomeStatus = TRUE;
				}
			}

			if (!pAppMod->m_bHWInitError)
			{
				// Everythings OK
				m_bModSelected			= TRUE;
			}

			pAppMod->m_bInspOptHome	= TRUE;
		}
	}
	else
	{
		pAppMod->m_bInspOptHome	= TRUE;
		pAppMod->m_bInspOptComm	= TRUE;
	}

	return TRUE;
}

INT CInspOpt::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CAC9000Stn::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CInspOpt::Operation()
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

	Sleep(10);
}


/////////////////////////////////////////////////////////////////
//Update Functions
/////////////////////////////////////////////////////////////////

VOID CInspOpt::UpdateOutput()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	INT i = 0;

	if ((m_qState == UN_INITIALIZE_Q) || (m_qState == DE_INITIAL_Q) || (m_qState == AUTO_Q) || (m_qState == DEMO_Q))
	{
		return;
	}

	if (pAppMod->m_bHWInitError)
	{
		return;
	}

	try
	{
		if (m_fHardware && m_bCheckMotor)
		{
			for (i = 0; i < m_myMotorList.GetSize(); i++)
			{
				GetmyMotorList().GetAt(i)->bHMI_On			= GetmyMotorList().GetAt(i)->IsPowerOn();
				GetmyMotorList().GetAt(i)->lHMI_CurrPosn	= GetmyMotorList().GetAt(i)->GetEncPosn();
			}

			if (m_bModSelected)
			{
				CheckModSelected(TRUE);
			}

			//if (m_bEnableMeasure) //20121015
			//{
			//	DOUBLE dx, dy;
			//	dx = CountToDistance(m_stMotorX.GetEncPosn() - m_mtrMeasureSetZeroPt.x, m_stMotorX.stAttrib.dDistPerCount);
			//	dy = CountToDistance(pCurInspWH->m_stMotorY.GetEncPosn() - m_mtrMeasureSetZeroPt.y, pCurInspWH->m_stMotorY.stAttrib.dDistPerCount);
			//	m_dMeasuredDistance = sqrt(pow(dy, 2.0) + pow(dx, 2.0));
			//	m_dMeasuredAngle = calcDisplayAngleDegree(dy, dx);
			//}
		}

		// I/Os
		if (pAppMod->IsInitNuDrive() && m_bCheckIO)
		{
			for (i = 0; i < m_mySiList.GetSize(); i++)
			{
				GetmySiList().GetAt(i)->bHMI_Status			= GetmySiList().GetAt(i)->GetGmpPort().IsOn();
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	Sleep(UPDATEOUTPUT_SLEEP);
}

VOID CInspOpt::RegisterVariables()
{
	try
	{
		// -------------------------------------------------------
		// PI9000Stn Variable and Function
		// -------------------------------------------------------
		// -------------------------------------------------------
		RegVariable(GetStation() + _T("bCheckMotor"), &m_bCheckMotor);
		RegVariable(GetStation() + _T("bCheckIO"), &m_bCheckIO);

		RegVariable(GetStation() + _T("bModSelected"), &m_bModSelected);
		RegVariable(GetStation() + _T("bShowModSelected"), &m_bShowModSelected);
		RegVariable(GetStation() + _T("bModError"), &m_bModError);
		
		RegVariable(GetStation() + _T("bDebugSeq"), &HMI_bDebugSeq);
		RegVariable(GetStation() + _T("bDebugCal"), &HMI_bDebugCal);
		RegVariable(GetStation() + _T("bLogMotion"), &HMI_bLogMotion);

		
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDebugSeq"), HMI_SetDebugSeq);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDebugCal"), HMI_SetDebugCal);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetLogMotion"), HMI_SetLogMotion);

		//// hardware
		RegVariable(_T("HMI_b") + GetStation().Trim("_") + _T("XOn"), &m_stMotorX.bHMI_On);
		//RegVariable(_T("HMI_b") + GetStation().Trim("_") + _T("YOn"), &m_stMotorY.bHMI_On);

		RegVariable(_T("HMI_l") + GetStation().Trim("_") + _T("XEncoder"), &m_stMotorX.lHMI_CurrPosn);
		//RegVariable(_T("HMI_l") + GetStation().Trim("_") + _T("YEncoder"), &m_stMotorY.lHMI_CurrPosn);

		RegVariable(GetStation() + _T("dXDistPerCount"), &m_stMotorX.stAttrib.dDistPerCount);
		//RegVariable(GetStation() + _T("dYDistPerCount"), &m_stMotorY.stAttrib.dDistPerCount);

		//snr
		RegVariable(_T("HMI_bInspOptXLmtSnr"), &m_stXLmtSnr.bHMI_Status);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetModSelected"), HMI_SetModSelected);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeAll"), HMI_HomeAll);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnX"), HMI_PowerOnX);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnY"), HMI_PowerOnY);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("CommX"), HMI_CommX);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("CommY"), HMI_CommY);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeX"), HMI_HomeX);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeY"), HMI_HomeY);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDiagnSteps"), HMI_SetDiagnSteps);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexXPos"), HMI_IndexXPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexXNeg"), HMI_IndexXNeg);
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
}
// ------------------------------------------------------------------------
// Virtual CPRTaskStn Function delcaration
// -----------------------------------------------------------------------
CMotorInfo &CInspOpt::GetMotorX() 
{
	return m_stMotorX;
}

//CMotorInfo &CInspOpt::GetMotorY()
//{
//	CWH1 *pCWH1 = (CWH1*)m_pModule->GetStation("WH1");
//	CWH2 *pCWH2 = (CWH2*)m_pModule->GetStation("WH2");
//	CInspWH *pCurInspWH;
//
//	if(m_lCurInspWH == WH_1)
//	{
//		pCurInspWH = pCWH1;
//	}
//	else
//	{
//		pCurInspWH = pCWH2;
//	}
//	return pCurInspWH->m_stMotorY;
//}


/////////////////////////////////////////////////////////////////
//Modified Virtual Operational Functions
/////////////////////////////////////////////////////////////////
VOID CInspOpt::PreStartOperation()
{
	m_bModError = FALSE;
	m_lRejectGlassCounter = 0;

	if (!m_bModSelected)
	{	
		INSPOPT_Status = ST_MOD_NOT_SELECTED;
		m_qSubOperation = INSPOPT_STOP_Q;
		m_bStopCycle = TRUE;
	}
	else
	{
		if ((m_nLastError = MoveX(INSPOPT_X_CTRL_GO_STANDBY_POSN, GMP_WAIT)) != GMP_SUCCESS)
		{
			Command(glSTOP_COMMAND, TRUE);
			INSPOPT_Status = ST_STOP;
			m_qSubOperation = INSPOPT_STOP_Q;
			m_bStopCycle = TRUE;
		}
		else
		{
			INSPOPT_Status = ST_IDLE;
			m_qSubOperation = INSPOPT_IDLE_Q;
			m_bStopCycle = FALSE;
		}
	}

	Motion(FALSE);
	State(IDLE_Q);

	m_deqDebugSeq.clear(); //20131010
	m_qPreviousSubOperation = m_qSubOperation;
	UpdateDebugSeq(GetSequenceString(), INSPOPT_Status);

	CAC9000Stn::PreStartOperation();
}

VOID CInspOpt::AutoOperation()
{
	// stop command received: end operation after cycle completed
	if (Command() == glSTOP_COMMAND || g_bStopCycle)
	{
		m_bStopCycle = TRUE;
	}

	OperationSequence();

	// cycle completed or motor error or abort requested: end operation immediately
	if (
		(m_bModSelected && m_qSubOperation == INSPOPT_STOP_Q) || 
		(!m_bModSelected && m_bStopCycle) || 
		GMP_SUCCESS != m_nLastError || IsAbort() 
	   )
	{
		if (m_fHardware && m_bModSelected)
		{
			MoveX(INSPOPT_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
		}

		if (GMP_SUCCESS != m_nLastError)
		{
			m_bModError = TRUE;
		}

		INSPOPT_Status = ST_STOP;
		Command(glSTOP_COMMAND);
		State(STOPPING_Q);
		m_bStopCycle = FALSE;

		CAC9000Stn::StopOperation();
	}
}

VOID CInspOpt::CycleOperation()
{
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");

	//if (!m_bSearchPlacementCycle)
	//{
	//	State(STOPPING_Q);

	//	return;
	//}

	// stop command received: end operation after cycle completed
	//if (Command() == glSTOP_COMMAND)
	//{
	//	m_bSearchPlacementCycle = FALSE;
	//}

	PRINT_DATA	stPrintData;
	PRINT_DATA	szPrintTitle;
	CString		szMsg;

	//if (m_bSearchPlacementCycle)
	//{
	//	m_bSearchPlacementCycle = SearchPlacementTest();
	//}

	//if (!m_bSearchPlacementCycle)
	//{
	//	State(STOPPING_Q);
	//}
}

CString CInspOpt::GetSequenceString()
{
	switch (m_qSubOperation)
	{
	case INSPOPT_IDLE_Q:			//0
		return "INSPOPT_IDLE_Q";

	case INSPOPT_WAIT_ACFWH_PR_Q:			//1
		return "INSPOPT_WAIT_ACFWH_PR_Q";

	case INSPOPT_WAIT_ACFWH_PR_COMPLETE_Q:			//2
		return "INSPOPT_WAIT_ACFWH_PR_COMPLETE_Q";

	case INSPOPT_STOP_Q:			//99
		return "INSPOPT_STOP_Q";

	}

	return "Hi";
}

VOID CInspOpt::OperationSequence()
{
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString szMsg;

	PRINT_DATA	stPrintData;

	INT i = 0;

	for (i = 0; i < MAX_PRINT_DATA; i++)
	{
		stPrintData.szTitle[i]	= "";
	}
	
		switch (m_qSubOperation)
		{
		case INSPOPT_IDLE_Q:			//0
			if (m_bStopCycle)
			{
				m_qSubOperation = INSPOPT_STOP_Q;	
			}
			else if(ACF_WH_Status[ACFWH_1] == ST_REQ_PR)
			{
				m_lCurrWH = ACFWH_1;
				INSPOPT_Status = ST_ACFWH1_REQ_PR_ACK;
				m_qSubOperation = INSPOPT_WAIT_ACFWH_PR_Q;		//1
			}
			else if(ACF_WH_Status[ACFWH_2] == ST_REQ_PR)
			{
				m_lCurrWH = ACFWH_2;
				INSPOPT_Status = ST_ACFWH2_REQ_PR_ACK;
				m_qSubOperation = INSPOPT_WAIT_ACFWH_PR_Q;		//1
			}
			break;	


		case INSPOPT_WAIT_ACFWH_PR_Q:			//1
			
			if (
				(ACF_WH_Status[m_lCurrWH] == ST_STOP)
				)
			{
				m_qSubOperation = INSPOPT_STOP_Q;	
			}
			else if(ACF_WH_Status[m_lCurrWH] == ST_PR_IN_PROCESS || ACF_WH_Status[m_lCurrWH] == ST_PB_PR_IN_PROCESS || ACF_WH_Status[m_lCurrWH] == ST_PR_DONE)
			{
				m_qSubOperation = INSPOPT_WAIT_ACFWH_PR_COMPLETE_Q;		//2
			}
			break;	


		case INSPOPT_WAIT_ACFWH_PR_COMPLETE_Q:			//2
			if (
				(ACF_WH_Status[m_lCurrWH] == ST_STOP)
				)
			{
				m_qSubOperation = INSPOPT_STOP_Q;	
			}
			else if(ACF_WH_Status[m_lCurrWH] == ST_PR_DONE || ACF_WH_Status[m_lCurrWH] == ST_PR_ERROR)
			{
				if ((m_nLastError = MoveX(INSPOPT_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT)) == GMP_SUCCESS)
				{
					//Sleep(100);
					INSPOPT_Status = ST_IDLE;
					m_qSubOperation = INSPOPT_IDLE_Q;		//0
				}
			}
			break;	


		case INSPOPT_STOP_Q:			//99

			break;

		}


	if (m_qPreviousSubOperation != m_qSubOperation)
	{
		if (HMI_bDebugSeq)
		{
			CString szMsg;
			CString szFileName;

			if (INSPOPT_STOP_Q != m_qSubOperation)
			{
				szMsg.Format("%s Go State: %s", GetStnName(), GetSequenceString());
			}
			else
			{
				szMsg.Format("%s Go Stop", GetStnName());
			}
			//DisplayMessage(szMsg);
		}
		UpdateDebugSeq(GetSequenceString(), INSPOPT_Status);
		m_qPreviousSubOperation = m_qSubOperation;

		if (HMI_bDebugSeq)
		{
			for (INT i = 0; i < MAX_PRINT_DATA; i++)
			{
				stPrintData.szTitle[i]	= "";
			}

			stPrintData.szTitle[0].Format("%ld", GetTickCount());
			stPrintData.szTitle[GetStationID() * MAX_STATION_SEQ_DATA + 1].Format("%s", GetSequenceString());
			stPrintData.szTitle[GetStationID() * MAX_STATION_SEQ_DATA + 2].Format("%ld", INSPOPT_Status);

			LogFile->log(stPrintData.szTitle);
		}
	}
}



///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
INT CInspOpt::SyncX()
{
	return m_stMotorX.Sync();
}

///
//Temp

INT CInspOpt::MoveX(INSPOPT_X_CTRL_SIGNAL ctrl, BOOL bWait)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	INT nResult = GMP_SUCCESS;

	MTR_POSN	stMove;

	stMove.x	= 0;
	stMove.y	= 0;
	if (pAppMod->State() == IDLE_Q)
	{
		if (!m_bModSelected)
		{
			HMIMessageBox(MSG_OK, "WARNING", "InspOpt Module Not Selected. Operation Abort");
			return GMP_FAIL;
		}
	}

	switch (ctrl)
	{
	case INSPOPT_X_CTRL_GO_STANDBY_POSN:
		stMove.x = m_mtrStandbyPosn.x;
		break;
	}

	return MoveAbsoluteX(stMove.x, bWait);
}

INT CInspOpt::MoveAbsoluteX(LONG lCntX, BOOL bWait)
{
	INT nResultX = GMP_SUCCESS;

	if (
		(nResultX = m_stMotorX.MoveAbsolute(lCntX, GMP_NOWAIT)) == GMP_SUCCESS
	   )
	{
		if (bWait)
		{
			nResultX = m_stMotorX.Sync();
		}
	}

	return nResultX;
}

INT CInspOpt::MoveRelativeX(LONG lCntX, BOOL bWait)
{
	INT nResultX = GMP_SUCCESS;

	if (
		(nResultX = m_stMotorX.MoveRelative(lCntX, GMP_NOWAIT)) == GMP_SUCCESS
	   )
	{
		if (bWait)
		{
			nResultX = m_stMotorX.Sync();
		}
	}

	return nResultX;
}



INT CInspOpt::SetupGoStandbyPosn()
{
	INT nResult = GMP_SUCCESS;

	MoveX(INSPOPT_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);

	return nResult;
}

LONG CInspOpt::HMI_PowerOnX(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	m_stMotorX.PowerOn(bMode);

	bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

// Commutate
LONG CInspOpt::HMI_CommX(IPC_CServiceMessage &svMsg)
{
	m_stMotorX.Commutate();

	return 0;
}

// Home
LONG CInspOpt::HMI_HomeX(IPC_CServiceMessage &svMsg)
{
	m_stMotorX.Home(GMP_WAIT);

	return 0;
}

VOID CInspOpt::SetDiagnSteps(ULONG ulSteps)
{
	switch (ulSteps)
	{
	case 0:
		m_lDiagnSteps = 1;
		break;
	case 1:
		m_lDiagnSteps = 10;
		break;
	case 2:
		m_lDiagnSteps = 100;
		break;
	case 3:
		m_lDiagnSteps = 1000;
		break;
	case 4:
		m_lDiagnSteps = 10000;
		break;
	case 5:
		m_lDiagnSteps = 50000;
		break;
	default:
		m_lDiagnSteps = 1000;
	}
}

LONG CInspOpt::HMI_SetDiagnSteps(IPC_CServiceMessage &svMsg)
{
	ULONG ulSteps;
	svMsg.GetMsg(sizeof(ULONG), &ulSteps);

	SetDiagnSteps(ulSteps);
	return 0;
}

VOID CInspOpt::ModuleSetupPreTask()
{

	m_bCheckMotor		= TRUE;
	m_bCheckIO			= TRUE;
	
	CheckModSelected(TRUE);
	
	SetDiagnSteps(g_lDiagnSteps);

	//UpdateModuleSetupPosn();
	//UpdateModuleSetupLevel();
	//UpdateModuleSetupPara();
}

VOID CInspOpt::ModuleSetupPostTask()
{

	m_bCheckMotor		= FALSE;
	m_bCheckIO			= FALSE;
}

LONG CInspOpt::HMI_IndexXPos(IPC_CServiceMessage &svMsg)
{
	//if (m_lDiagnSteps != 10000)
	{
		m_stMotorX.MoveRelative(m_lDiagnSteps, GMP_WAIT);
	}
	//else
	//{
	//	m_stMotorX.MoveRelative(m_lDiagnSteps * 10, GMP_WAIT);
	//}
	return 0;
}

LONG CInspOpt::HMI_IndexXNeg(IPC_CServiceMessage &svMsg)
{

	//if (m_lDiagnSteps != 10000)
	{
		m_stMotorX.MoveRelative(-m_lDiagnSteps, GMP_WAIT);
	}
	//else
	//{
	//	m_stMotorX.MoveRelative(-m_lDiagnSteps * 10, GMP_WAIT);
	//}
	return 0;
}

LONG CInspOpt::IPC_SaveMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	//CString strTitle, szParamName;

	//// saving positions
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrStandbyPosn.x"]							= m_mtrStandbyPosn.x;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrStandbyPosn.y"]							= m_mtrStandbyPosn.y;

	//pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrPrePRPosn.x"]								= m_mtrPrePRPosn.x;
	//pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrPrePRPosn.y"]								= m_mtrPrePRPosn.y;

	//pAppMod->m_smfMachine[GetStnName()]["Setting"]["m_bGoPrePRPosn"]							= m_bGoPrePRPosn;
	//pAppMod->m_smfMachine[GetStnName()]["Setting"]["m_bDoPR2First"]								= m_bDoPR2First;

	//pAppMod->m_smfMachine[GetStnName()]["Setting"]["m_bFeedBackPlacement"]						= m_bFeedBackPlacement;

	return CAC9000Stn::IPC_SaveMachineParam();
}

LONG CInspOpt::IPC_LoadMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	//CString strTitle, szParamName;

	//// saving positions
	m_mtrStandbyPosn.x		= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrStandbyPosn.x"];
	m_mtrStandbyPosn.y		= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrStandbyPosn.y"];

	//m_mtrPrePRPosn.x		= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrPrePRPosn.x"];
	//m_mtrPrePRPosn.y		= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrPrePRPosn.y"];

	//m_bGoPrePRPosn			= (BOOL)(LONG)pAppMod->m_smfMachine[GetStnName()]["Setting"]["m_bGoPrePRPosn"];
	//m_bDoPR2First			= (BOOL)(LONG)pAppMod->m_smfMachine[GetStnName()]["Setting"]["m_bDoPR2First"];

	//m_bFeedBackPlacement	= (BOOL)(LONG)pAppMod->m_smfMachine[GetStnName()]["Setting"]["m_bFeedBackPlacement"];

	return CAC9000Stn::IPC_LoadMachineParam();
}

LONG CInspOpt::IPC_SaveDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	//CString str,str1;
	//for (INT i = GLASS1; i < MAX_NUM_OF_GLASS; i++)
	//{
	//	str.Format("GLASS%ld", i + 1);

	//	for (INT j = WH_1; j < MAX_NUM_OF_WH; j++)
	//	{
	//		str1.Format("WH%ld", j + 1);
	//		pAppMod->m_smfDevice[GetStnName()]["Posn"][str][str1]["m_mtrPR1Posn.x"]				= m_mtrPR1Posn[j][i].x;
	//		pAppMod->m_smfDevice[GetStnName()]["Posn"][str][str1]["m_mtrPR1Posn.y"]				= m_mtrPR1Posn[j][i].y;
	//		pAppMod->m_smfDevice[GetStnName()]["Posn"][str][str1]["m_mtrPR2Posn.x"]				= m_mtrPR2Posn[j][i].x;
	//		pAppMod->m_smfDevice[GetStnName()]["Posn"][str][str1]["m_mtrPR2Posn.y"]				= m_mtrPR2Posn[j][i].y;
	//		pAppMod->m_smfDevice[GetStnName()]["Level"][str][str1]["m_lULPRLevel"]				= m_lULPRLevel[j][i];
	//	}
	//	pAppMod->m_smfDevice[GetStnName()]["AlignParam"][str]["m_dGlassRefAng"]			= m_dGlassRefAng[i];
	//	pAppMod->m_smfDevice[GetStnName()]["AlignParam"][str]["m_dmtrAlignRef.x"]		= m_dmtrAlignRef[i].x;
	//	pAppMod->m_smfDevice[GetStnName()]["AlignParam"][str]["m_dmtrAlignRef.y"]		= m_dmtrAlignRef[i].y;
	//	pAppMod->m_smfDevice[GetStnName()]["AlignParam"][str]["m_dAlignRefAngle"]		= m_dAlignRefAngle[i];
	//}

	//pAppMod->m_smfDevice[GetStnName()]["InspParam"]["m_dPlacementTolX"]			= m_dPlacementTolX;	
	//pAppMod->m_smfDevice[GetStnName()]["InspParam"]["m_dPlacementTolY"]			= m_dPlacementTolY;	
	//pAppMod->m_smfDevice[GetStnName()]["InspParam"]["m_dPlacementTolAng"]		= m_dPlacementTolAng;

	////pAppMod->m_smfDevice[GetStnName()]["Para"]["HMI_bDebugCal"]						= HMI_bDebugCal;


	return CAC9000Stn::IPC_SaveDeviceParam();
}

LONG CInspOpt::IPC_LoadDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	//CString str,str1;

	//for (INT i = GLASS1; i < MAX_NUM_OF_GLASS; i++)
	//{
	//	str.Format("GLASS%ld", i + 1);

	//	for (INT j = WH_1; j < MAX_NUM_OF_WH; j++)
	//	{
	//		str1.Format("WH%ld", j + 1);

	//		m_mtrPR1Posn[j][i].x		= pAppMod->m_smfDevice[GetStnName()]["Posn"][str][str1]["m_mtrPR1Posn.x"];
	//		m_mtrPR1Posn[j][i].y		= pAppMod->m_smfDevice[GetStnName()]["Posn"][str][str1]["m_mtrPR1Posn.y"];
	//		m_mtrPR2Posn[j][i].x		= pAppMod->m_smfDevice[GetStnName()]["Posn"][str][str1]["m_mtrPR2Posn.x"];
	//		m_mtrPR2Posn[j][i].y		= pAppMod->m_smfDevice[GetStnName()]["Posn"][str][str1]["m_mtrPR2Posn.y"];
	//		m_lULPRLevel[j][i]  		= pAppMod->m_smfDevice[GetStnName()]["Level"][str][str1]["m_lULPRLevel"];
	//	}

	//	m_dGlassRefAng[i]		= pAppMod->m_smfDevice[GetStnName()]["AlignParam"][str]["m_dGlassRefAng"];
	//	m_dmtrAlignRef[i].x		= pAppMod->m_smfDevice[GetStnName()]["AlignParam"][str]["m_dmtrAlignRef.x"];
	//	m_dmtrAlignRef[i].y		= pAppMod->m_smfDevice[GetStnName()]["AlignParam"][str]["m_dmtrAlignRef.y"];
	//	m_dAlignRefAngle[i]		= pAppMod->m_smfDevice[GetStnName()]["AlignParam"][str]["m_dAlignRefAngle"];
	//}
	//m_dPlacementTolX			= pAppMod->m_smfDevice[GetStnName()]["InspParam"]["m_dPlacementTolX"];	
	//m_dPlacementTolY			= pAppMod->m_smfDevice[GetStnName()]["InspParam"]["m_dPlacementTolY"];	
	//m_dPlacementTolAng			= pAppMod->m_smfDevice[GetStnName()]["InspParam"]["m_dPlacementTolAng"];

	//HMI_bDebugCal = (BOOL)(LONG)pAppMod->m_smfDevice[GetStnName()]["Para"]["HMI_bDebugCal"];
	return CAC9000Stn::IPC_LoadDeviceParam();
}



#ifdef EXCEL_MACHINE_DEVICE_INFO
BOOL CInspOpt::printMachineDeviceInfo()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bResult = TRUE;
	CString szBlank("--");
	CString szModule = GetStnName();
	LONG lValue = 0;
	DOUBLE dValue = 0.0;
//	INT i;
	CString strTitle, szParamName;

	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrStandbyPosn.x"];
		bResult = printMachineInfoRow(szModule, CString("Posn"), CString("mtrStandbyPosn.x"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}


	if (bResult)
	{
		bResult = CAC9000Stn::printMachineDeviceInfo();
	}
	return bResult;
}
#endif


