/////////////////////////////////////////////////////////////////
//	TAManager.cpp : interface of the CTAManager class
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

#include "HouseKeeping.h"

#define TAManager_EXTERN
#include "TAManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CTAManager, CAC9000Stn)

CTAManager::CTAManager()
{
}

CTAManager::~CTAManager()
{
}

BOOL CTAManager::InitInstance()
{
	CAC9000Stn::InitInstance();

	return TRUE;
}

INT CTAManager::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CAC9000Stn::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////
VOID CTAManager::Operation()
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

VOID CTAManager::UpdateOutput()
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

	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	Sleep(UPDATEOUTPUT_SLEEP);
}

VOID CTAManager::RegisterVariables()
{
	try
	{
		// -------------------------------------------------------
		// AC9000Stn Variable and Function
		// -------------------------------------------------------
		// -------------------------------------------------------
		RegVariable(GetStation() + _T("bModError"), &m_bModError);

		RegVariable(GetStation() + _T("bDebugSeq"), &HMI_bDebugSeq);
		RegVariable(GetStation() + _T("bLogMotion"), &HMI_bLogMotion);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetModSelected"), HMI_SetModSelected);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDebugSeq"), HMI_SetDebugSeq);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetLogMotion"), HMI_SetLogMotion);

	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
}

/////////////////////////////////////////////////////////////////
//Modified Virtual Operational Functions
/////////////////////////////////////////////////////////////////
VOID CTAManager::PreStartOperation()
{
	m_bModError = FALSE;
	
	// Delete me later
	m_bModSelected = TRUE;
	//if (!m_bModSelected)
	//{	
	//	TA_MANAGER_Status = ST_MOD_NOT_SELECTED;
	//	m_qSubOperation = TA_MANAGER_STOP_Q;
	//	m_bStopCycle = FALSE;
	//}
	//else
	{	
		m_lCurrCheck = ACFWH_1;		//init
		m_lCurrNotCheck = ACFWH_2;		//init
		TA_MANAGER_Status = ST_IDLE;
		m_qSubOperation = TA_MANAGER_IDLE_Q;
		m_bStopCycle = FALSE;
	}

	Motion(FALSE);
	State(IDLE_Q);
	
	m_deqDebugSeq.clear(); //20131010
	m_qPreviousSubOperation = m_qSubOperation;
	UpdateDebugSeq(GetSequenceString(), TA_MANAGER_Status, "Current Check", m_lCurrCheck);

	CAC9000Stn::PreStartOperation();
}

VOID CTAManager::AutoOperation()
{
	// stop command received: end operation after cycle completed
	if (Command() == glSTOP_COMMAND || g_bStopCycle)
	{
		m_bStopCycle = TRUE;
	}

	OperationSequence();

	// cycle completed or motor error or abort requested: end operation immediately
	if (
		(m_bModSelected && m_qSubOperation == TA_MANAGER_STOP_Q) || 
		(!m_bModSelected && m_bStopCycle) || 
		GMP_SUCCESS != m_nLastError || IsAbort() 
	   )
	{
		if (m_fHardware && m_bModSelected)
		{
			// Add me later
		}

		if (GMP_SUCCESS != m_nLastError)
		{
			m_bModError = TRUE;
		}

		TA_MANAGER_Status = ST_STOP;
		Command(glSTOP_COMMAND);
		State(STOPPING_Q);
		m_bStopCycle = FALSE;

		CAC9000Stn::StopOperation();
	}
}


/////////////////////////////////////////////////////////////////
//Operational Sequences
/////////////////////////////////////////////////////////////////
CString CTAManager::GetSequenceString()
{
	switch (m_qSubOperation)
	{
	case TA_MANAGER_IDLE_Q:
		return "TA_MANAGER_IDLE_Q";

	case TA_MANAGER_CHECK_ACFWH_REQUEST_LOAD_Q:
		return "TA_MANAGER_CHECK_ACFWH_REQUEST_LOAD_Q";

	case TA_MANAGER_CHECK_TA1_SAFE_TO_MOVE_Q:
		return "TA_MANAGER_CHECK_TA1_SAFE_TO_MOVE_Q";

	case TA_MANAGER_WAIT_TA1_ACK_Q:
		return "TA_MANAGER_WAIT_TA1_ACK_Q";

	case TA_MANAGER_CHECK_ACFWH_REQUEST_UNLOAD_Q:
		return "TA_MANAGER_CHECK_ACFWH_REQUEST_UNLOAD_Q";

	case TA_MANAGER_CHECK_TA2_SAFE_TO_MOVE_Q:
		return "TA_MANAGER_CHECK_TA2_SAFE_TO_MOVE_Q";

	case TA_MANAGER_WAIT_TA2_ACK_Q:
		return "TA_MANAGER_WAIT_TA2_ACK_Q";

	case TA_MANAGER_STOP_Q:
		return "TA_MANAGER_STOP_Q";
	}

	return "ASSASSINATE JASON";
}

VOID CTAManager::OperationSequence()
{
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");
	
	PRINT_DATA	stPrintData;

	m_nLastError = GMP_SUCCESS;

	CString szMsg = "";

	switch (m_qSubOperation)
	{
	case TA_MANAGER_IDLE_Q:	// 0
		
		m_qSubOperation = TA_MANAGER_CHECK_ACFWH_REQUEST_LOAD_Q;		// go to 1

		break;

	case TA_MANAGER_CHECK_ACFWH_REQUEST_LOAD_Q: // 1
		if (m_bStopCycle)
		{
			m_qSubOperation = TA_MANAGER_STOP_Q;	// go to 7
		}
		else if (ACF_WH_Status[ACFWH_1] == ST_STOP && ACF_WH_Status[ACFWH_2] == ST_STOP )
		{
			m_qSubOperation = TA_MANAGER_STOP_Q;		// go to 7		
		}
		else if (ACF_WH_Status[m_lCurrCheck] == ST_READY_LOAD)
		{
			m_qSubOperation = TA_MANAGER_CHECK_TA1_SAFE_TO_MOVE_Q;		// go to 2
		}
		else
		{
			m_qSubOperation = TA_MANAGER_CHECK_ACFWH_REQUEST_UNLOAD_Q;		// go to 4
		}
		break;

	case TA_MANAGER_CHECK_TA1_SAFE_TO_MOVE_Q: // 2
		if (m_bStopCycle)
		{
			m_qSubOperation = TA_MANAGER_STOP_Q;	// go to 7
		}
		//else if (m_lCurrCheck < TA2_POSN && m_lCurrCheck != TA1_POSN)
		else if (TA2_POSN == MAX_NUM_OF_ACFWH)	//TA2 at Standby POSN, TA1 is safe to move
		{
			if (m_lCurrCheck == ACFWH_1)
			{
				TA_MANAGER_Status = ST_ACFWH1_REQ_LOAD;
			}
			else
			{
				TA_MANAGER_Status = ST_ACFWH2_REQ_LOAD;
			}
			m_qSubOperation = TA_MANAGER_WAIT_TA1_ACK_Q;		// go to 3
		}
		else // TA1 can't place glass, wait // go to main loop wait again
		{
			m_qSubOperation = TA_MANAGER_CHECK_ACFWH_REQUEST_UNLOAD_Q;		// go to 4			
		}
		break;

	case TA_MANAGER_WAIT_TA1_ACK_Q: // 3
		if (TA1_Status == ST_STOP || TA2_Status == ST_STOP)
		{
			m_qSubOperation = TA_MANAGER_STOP_Q;		// go to 7			
		}
		else if (m_lCurrCheck == ACFWH_1 && (TA1_Status == ST_ACFWH1_ACK_LOAD || TA1_Status == ST_ACK || TA1_Status == ST_EMPTY))
		{
			TA_MANAGER_Status = ST_IDLE;
			m_qSubOperation = TA_MANAGER_CHECK_ACFWH_REQUEST_UNLOAD_Q;		// go to 4			
		}
		else if (m_lCurrCheck == ACFWH_2 && (TA1_Status == ST_ACFWH2_ACK_LOAD || TA1_Status == ST_ACK || TA1_Status == ST_EMPTY))
		{
			TA_MANAGER_Status = ST_IDLE;
			m_qSubOperation = TA_MANAGER_CHECK_ACFWH_REQUEST_UNLOAD_Q;		// go to 4			
		}
		break;

	case TA_MANAGER_CHECK_ACFWH_REQUEST_UNLOAD_Q: // 4
		if (m_bStopCycle)
		{
			m_qSubOperation = TA_MANAGER_STOP_Q;	// go to 7
		}
		else if (ACF_WH_Status[ACFWH_1] == ST_STOP && ACF_WH_Status[ACFWH_2] == ST_STOP )
		{
			m_qSubOperation = TA_MANAGER_STOP_Q;		// go to 7			
		}
		else if (
				 (
					 (ACF_WH_Status[m_lCurrCheck] == ST_PICK_READY1_EMPTY2 ) ||
					 (ACF_WH_Status[m_lCurrCheck] == ST_PICK_ERR1_EMPTY2 ) ||
					 (ACF_WH_Status[m_lCurrCheck] == ST_PB_PR_IN_PROCESS ) && (ACF_WH_Status[m_lCurrNotCheck] != ST_READY_LOAD && ACF_WH_Status[m_lCurrNotCheck] != ST_IDLE && ACF_WH_Status[m_lCurrNotCheck] != ST_EMPTY) 
				 ) && 
				 TA2_Status == ST_EMPTY
				)
		{
			m_qSubOperation = TA_MANAGER_CHECK_TA2_SAFE_TO_MOVE_Q;		// go to 5
		}
		else
		{
			m_lCurrNotCheck = m_lCurrCheck;
			m_lCurrCheck = (ACFWHUnitNum)((m_lCurrCheck + 1) % MAX_NUM_OF_ACFWH);
			m_qSubOperation = TA_MANAGER_CHECK_ACFWH_REQUEST_LOAD_Q;		// go to 1
		}
		break;

	case TA_MANAGER_CHECK_TA2_SAFE_TO_MOVE_Q: // 5
		if (m_bStopCycle)
		{
			m_qSubOperation = TA_MANAGER_STOP_Q;	// go to 7
		}
		//else if (m_lCurrCheck > TA1_POSN && m_lCurrCheck != TA2_POSN)
		else if (TA1_POSN == ACF_NONE)		//TA1 at Standby POSN, TA2 is safe to move
		{
			if (m_lCurrCheck == ACFWH_1)
			{
				TA_MANAGER_Status = ST_ACFWH1_REQ_UNLOAD;
			}
			else
			{
				TA_MANAGER_Status = ST_ACFWH2_REQ_UNLOAD;
			}
			m_qSubOperation = TA_MANAGER_WAIT_TA2_ACK_Q;		// go to 6
		}
		else
		{
			m_lCurrCheck = (ACFWHUnitNum)((m_lCurrCheck + 1) % MAX_NUM_OF_ACFWH);
			m_qSubOperation = TA_MANAGER_CHECK_ACFWH_REQUEST_LOAD_Q;		// go to 1
		}
		break;

	case TA_MANAGER_WAIT_TA2_ACK_Q: // 6
		if (TA1_Status == ST_STOP || TA2_Status == ST_STOP)
		{
			m_qSubOperation = TA_MANAGER_STOP_Q;		// go to 99
		}
		else if (m_lCurrCheck == ACFWH_1 && TA2_Status == ST_ACFWH1_ACK_UNLOAD)
		{
			TA_MANAGER_Status = ST_IDLE;
			m_lCurrCheck = (ACFWHUnitNum)((m_lCurrCheck + 1) % MAX_NUM_OF_ACFWH);
			m_qSubOperation = TA_MANAGER_CHECK_ACFWH_REQUEST_LOAD_Q;		// go to 1			
		}
		else if (m_lCurrCheck == ACFWH_2 && TA2_Status == ST_ACFWH2_ACK_UNLOAD)
		{
			TA_MANAGER_Status = ST_IDLE;            
			m_lCurrCheck = (ACFWHUnitNum)((m_lCurrCheck + 1) % MAX_NUM_OF_ACFWH);
			m_qSubOperation = TA_MANAGER_CHECK_ACFWH_REQUEST_LOAD_Q;		// go to 1			
		}
		break;

	case TA_MANAGER_STOP_Q: // 7
		break;
	}

	if (m_qPreviousSubOperation != m_qSubOperation)
	{
		if (HMI_bDebugSeq)
		{
			CString szMsg;
			if (TA_MANAGER_STOP_Q != m_qSubOperation)
			{
				szMsg.Format("TA Manager Go State: %ld", m_qSubOperation);
			}
			else
			{
				szMsg.Format("TA Manager Go Stop");
			}
			//DisplayMessage(szMsg);
		}
		UpdateDebugSeq(GetSequenceString(), TA_MANAGER_Status, "Current Check", m_lCurrCheck);
		m_qPreviousSubOperation = m_qSubOperation;

		if (HMI_bDebugSeq)
		{
			for (INT i = 0; i < MAX_PRINT_DATA; i++)
			{
				stPrintData.szTitle[i]	= "";
			}

			stPrintData.szTitle[0].Format("%ld", GetTickCount());
			stPrintData.szTitle[GetStationID() * MAX_STATION_SEQ_DATA + 1].Format("%s", GetSequenceString());
			stPrintData.szTitle[GetStationID() * MAX_STATION_SEQ_DATA + 2].Format("%ld, %ld, %ld, TA1POSN=%ld, TA2POSN=%ld", TA_MANAGER_Status, TA1_Status, TA2_Status, TA1_POSN, TA2_POSN);

			LogFile->log(stPrintData.szTitle);
		}
	}
}
