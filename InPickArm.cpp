/////////////////////////////////////////////////////////////////
//	InPickArm.cpp : interface of the CInPickArm class
//
//	Description:
//		Motion Application
//
//	Date:		Friday, November 20, 2015
//	Revision:	1.00
//
//	By:			Author Unknown
//				Department
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2015.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "AC9000.h"
#include "AC9000_Constant.h"
#include "InPickArm.h"

#include "TimeChart.h"
#include "HouseKeeping.h"
#include "Cal_Util.h"
#include "TA1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//IMPLEMENT_DYNCREATE(CInPickArm, CAC9000Stn)
IMPLEMENT_DYNCREATE(CInPickArm, CTransferArm)

CInPickArm::CInPickArm()
{
	//m_lStnErrorCode		= IDS_IN_PICKARM_NOT_SELECTED_ERR;
	m_lModSelectMode	= GLASS_MOD;

	// Motor Related
	m_stMotorZ.InitMotorInfo("IN_PICK_ARM_Z", &m_bModSelected);
	m_stMotorX.InitMotorInfo("IN_PICK_ARM_X", &m_bModSelected);

	m_myMotorList.Add((CMotorInfo*)&m_stMotorZ);		// Home Z first
	m_myMotorList.Add((CMotorInfo*)&m_stMotorX);

	// Si Related
	m_stXLmtSnr			= CSiInfo("InPickArmXLmtSnr",		"IOMODULE_IN_PORT_4", BIT_18, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stXLmtSnr);	
	m_stZLmtSnr			= CSiInfo("InPickArmZLmtSnr",		"IOMODULE_IN_PORT_4", BIT_19, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stZLmtSnr);	
	m_stZHomeSnr		= CSiInfo("InPickArmZHomeSnr",		"IOMODULE_IN_PORT_4", BIT_20, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stZHomeSnr);	
	m_stT1Snr				= CSiInfo("InPickArmT1Snr",		"IOMODULE_IN_PORT_4", BIT_21, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stT1Snr);
	m_stT2Snr				= CSiInfo("InPickArmT2Snr",		"IOMODULE_IN_PORT_4", BIT_22, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stT2Snr);
	m_stGlass1VacSnr	= CSiInfo("InPickArmVACSnr",		"IOMODULE_IN_PORT_4", BIT_23, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stGlass1VacSnr);
	m_stUSLBackSideDoorSnr	= CSiInfo("USLBackSideDoorSnr",		"IOMODULE_IN_PORT_8", BIT_0, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stUSLBackSideDoorSnr);

	m_stFInIn1		= CSiInfo("FIN_IN1",			"IOMODULE_IN_PORT_8", BIT_18, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stFInIn1);
	m_stFInIn2		= CSiInfo("FIN_IN2",			"IOMODULE_IN_PORT_8", BIT_19, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stFInIn2);
	m_stFInIn3		= CSiInfo("FIN_IN3",			"IOMODULE_IN_PORT_8", BIT_20, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stFInIn3);
	m_stFInIn4		= CSiInfo("FIN_IN4",			"IOMODULE_IN_PORT_8", BIT_21, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stFInIn4);
	m_stFInIn5		= CSiInfo("FIN_IN5",			"IOMODULE_IN_PORT_8", BIT_22, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stFInIn5);
	m_stFInIn6		= CSiInfo("FIN_IN6",			"IOMODULE_IN_PORT_8", BIT_23, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stFInIn6);

	// So Related
	
	m_stRotarySol			= CSoInfo("InPickArmRotarySol",		"IOMODULE_OUT_PORT_3", BIT_9, IN_PICKARM_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stRotarySol);

	m_stGlass1VacSol		= CSoInfo("InPickArmVac1Sol",		"IOMODULE_OUT_PORT_3", BIT_10, IN_PICKARM_SOL, ACTIVE_LOW);
	m_mySoList.Add((CSoInfo*)&m_stGlass1VacSol);

	m_stWeakBlowSol			= CSoInfo("InPickArmWeakBlowSol",	"IOMODULE_OUT_PORT_3", BIT_11, IN_PICKARM_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stWeakBlowSol);

	
	m_stFInOut1	= CSoInfo("FIN-OUT1",		"IOMODULE_OUT_PORT_7", BIT_10, IN_PICKARM_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stFInOut1);
	m_stFInOut2	= CSoInfo("FIN-OUT2",		"IOMODULE_OUT_PORT_7", BIT_11, IN_PICKARM_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stFInOut2);
	m_stFInOut3	= CSoInfo("FIN-OUT3",		"IOMODULE_OUT_PORT_7", BIT_12, IN_PICKARM_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stFInOut3);
	
	m_stFInOut4	= CSoInfo("FIN-OUT4",		"IOMODULE_OUT_PORT_7", BIT_13, IN_PICKARM_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stFInOut4);
	m_stFInOut5	= CSoInfo("FIN-OUT5",		"IOMODULE_OUT_PORT_7", BIT_14, IN_PICKARM_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stFInOut5);
	m_stFInOut6	= CSoInfo("FIN-OUT6",		"IOMODULE_OUT_PORT_7", BIT_15, IN_PICKARM_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stFInOut6);

	m_lPickGlassOffset = 0;
	m_lPlaceGlassOffset = 0;

	m_lStandbyLevel		= 0;
	m_lPickLevel		= 0;
	m_lPlaceLevel		= 0;
	m_bIsVacError = FALSE;
	m_bUseFPCVacuum = FALSE;
	HMI_bStandAlone = FALSE;

}

CInPickArm::~CInPickArm()
{
}

BOOL CInPickArm::InitInstance()
{
	//CTransferArm::InitInstance();
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

				// Reset the Status 
				if( GetmySoList().GetAt(i)->GetName() != "InPickArmRotarySol")
				{
					GetmySoList().GetAt(i)->GetGmpPort().Off();
				}
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

		
		if (
			m_stMotorZ.Commutate() != GMP_SUCCESS ||
			m_stMotorZ.Home(GMP_WAIT) != GMP_SUCCESS ||
			m_stMotorZ.EnableProtection() != GMP_SUCCESS ||
			m_stMotorX.Commutate() != GMP_SUCCESS
		   )
		{
			m_bInitInstanceError			= TRUE;
			pAppMod->m_bHWInitError			= TRUE;
		}

		pAppMod->m_bInPickArmComm = TRUE;

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
			if (
				m_stMotorX.Home(GMP_WAIT) != GMP_SUCCESS ||
				m_stMotorX.EnableProtection() != GMP_SUCCESS 
			   )
			{
				m_bInitInstanceError	= TRUE;
				pAppMod->m_bHWInitError = TRUE;
			}
			else
			{
				// Everythings OK
				m_bModSelected			= TRUE;
			}
			pAppMod->m_bInPickArmHome	= TRUE;
		}
	}
	else
	{
		pAppMod->m_bInPickArmComm = TRUE;
		pAppMod->m_bInPickArmHome	= TRUE;
	}
	return TRUE;
}

INT CInPickArm::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CTransferArm::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CInPickArm::Operation()
{
	switch(State())
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

	//if (m_bUpdateProfile)
	//{
	//	UpdateProfile();
	//}

	//if (m_bUpdatePosition)
	//{
	//	UpdatePosition();
	//}

	Sleep(10);
}


/////////////////////////////////////////////////////////////////
//Update Functions
/////////////////////////////////////////////////////////////////

VOID CInPickArm::UpdateOutput()
{
	if (!m_fHardware)
	{
		return;
	}

	if ((m_qState == UN_INITIALIZE_Q) || (m_qState == DE_INITIAL_Q))
	{
		return;
	}

	CTransferArm::UpdateOutput();
}

//VOID CInPickArm::UpdateProfile()
//{
//	//Add Your Code Here
//	CAC9000App* pAppMod = dynamic_cast<CAC9000App*> (m_pModule);
//	CSingleLock slLock(&pAppMod->m_csMachine);
//
//	m_bUpdateProfile = FALSE;
//
//	try
//	{
//	}
//	catch(CAsmException e)
//	{
//		DisplayException(e);
//	}
//
//	slLock.Unlock();
//}
//
//VOID CInPickArm::UpdatePosition()
//{
//	//Add Your Code Here
//	CAC9000App* pAppMod = dynamic_cast<CAC9000App*> (m_pModule);
//	CSingleLock slLock(&pAppMod->m_csDevice);
//	
//	m_bUpdatePosition = FALSE;
//
//	try
//	{
//	}
//	catch(CAsmException e)
//	{
//		DisplayException(e);
//	}
//
//	slLock.Unlock();
//}

VOID CInPickArm::RegisterVariables()
{
	try
	{
		RegVariable(GetStation() + _T("bCheckMotor"), &m_bCheckMotor);
		RegVariable(GetStation() + _T("bCheckIO"), &m_bCheckIO);

		RegVariable(GetStation() + _T("bModSelected"), &m_bModSelected);
		RegVariable(GetStation() + _T("bShowModSelected"), &m_bShowModSelected);
		RegVariable(GetStation() + _T("bModError"), &m_bModError);

		RegVariable(GetStation() + _T("bGlass1Exist"), &m_bGlass1Exist);

		RegVariable(GetStation() + _T("bDebugSeq"), &HMI_bDebugSeq);
		RegVariable(GetStation() + _T("bLogMotion"), &HMI_bLogMotion);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetModSelected"), HMI_SetModSelected);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeAll"), HMI_HomeAll);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDebugSeq"), HMI_SetDebugSeq);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetLogMotion"), HMI_SetLogMotion);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PickGlassOperation"), HMI_PickGlassOperation);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PlaceGlassOperation"), HMI_PlaceGlassOperation);

		//motor
		RegVariable(_T("HMI_bInPickArmXOn"), &m_stMotorX.bHMI_On);
		RegVariable(_T("HMI_lInPickArmXEncoder"), &m_stMotorX.lHMI_CurrPosn);
		RegVariable(GetStation() + _T("dXDistPerCount"), &m_stMotorX.stAttrib.dDistPerCount);

		RegVariable(_T("HMI_bInPickArmZOn"), &m_stMotorZ.bHMI_On);
		RegVariable(_T("HMI_lInPickArmZEncoder"), &m_stMotorZ.lHMI_CurrPosn);
		RegVariable(GetStation() + _T("dZDistPerCount"), &m_stMotorZ.stAttrib.dDistPerCount);

		//SI
		RegVariable(_T("HMI_bInPickArmXLmtSnr"), &m_stXLmtSnr.bHMI_Status);
		RegVariable(_T("HMI_bInPickArmZLmtSnr"), &m_stZLmtSnr.bHMI_Status);
		RegVariable(_T("HMI_bInPickArmZHomeSnr"), &m_stZHomeSnr.bHMI_Status);
		RegVariable(_T("HMI_bInPickArmT1Snr"), &m_stT1Snr.bHMI_Status);
		RegVariable(_T("HMI_bInPickArmT2Snr"), &m_stT2Snr.bHMI_Status);
		RegVariable(_T("HMI_bInPickArmG1VacSnr"), &m_stGlass1VacSnr.bHMI_Status);

		//SO
		RegVariable(_T("HMI_bInPickArmRotarySol"), &m_stRotarySol.bHMI_Status);
		RegVariable(_T("HMI_bInPickArmG1VacSol"), &m_stGlass1VacSol.bHMI_Status);
		RegVariable(_T("HMI_bInPickArmWeakBlowSol"), &m_stWeakBlowSol.bHMI_Status);

		RegVariable(_T("HMI_bUSLBackSideDoorSnr"), &m_stUSLBackSideDoorSnr.bHMI_Status);
		RegVariable(_T("HMI_bFInIn1"), &m_stFInIn1.bHMI_Status);
		RegVariable(_T("HMI_bFInIn2"), &m_stFInIn2.bHMI_Status);
		RegVariable(_T("HMI_bFInIn3"), &m_stFInIn3.bHMI_Status);
		RegVariable(_T("HMI_bFInIn4"), &m_stFInIn4.bHMI_Status);
		RegVariable(_T("HMI_bFInIn5"), &m_stFInIn5.bHMI_Status);
		RegVariable(_T("HMI_bFInIn6"), &m_stFInIn6.bHMI_Status);

		RegVariable(_T("HMI_bFInOut1"), &m_stFInOut1.bHMI_Status);
		RegVariable(_T("HMI_bFInOut2"), &m_stFInOut2.bHMI_Status);
		RegVariable(_T("HMI_bFInOut3"), &m_stFInOut3.bHMI_Status);
		RegVariable(_T("HMI_bFInOut4"), &m_stFInOut4.bHMI_Status);
		RegVariable(_T("HMI_bFInOut5"), &m_stFInOut5.bHMI_Status);
		RegVariable(_T("HMI_bFInOut6"), &m_stFInOut6.bHMI_Status);

		RegVariable(GetStation() + _T("lPickPlaceRetryLimit"), &m_lPickPlaceRetryLimit);
		RegVariable(GetStation() + _T("bStandAlone"), &HMI_bStandAlone);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleFInOut1"), HMI_ToggleFInOut1);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleFInOut2"), HMI_ToggleFInOut2);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleFInOut3"), HMI_ToggleFInOut3);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleFInOut4"), HMI_ToggleFInOut4);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleFInOut5"), HMI_ToggleFInOut5);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleFInOut6"), HMI_ToggleFInOut6);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnX"), HMI_PowerOnX);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("CommX"), HMI_CommX);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeX"), HMI_HomeX);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnZ"), HMI_PowerOnZ);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("CommZ"), HMI_CommZ);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeZ"), HMI_HomeZ);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleWeakBlowSol"), HMI_ToggleWeakBlowSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleG1VacSol"), HMI_ToggleG1VacSol);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleG2VacSol"), HMI_ToggleG2VacSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleTSol"), HMI_ToggleTSol);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDiagnSteps"), HMI_SetDiagnSteps);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexXPos"), HMI_IndexXPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexXNeg"), HMI_IndexXNeg);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexZPos"), HMI_IndexZPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexZNeg"), HMI_IndexZNeg);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPickPlaceRetryLimit"), HMI_SetPickPlaceRetryLimit);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetStandAlone"), HMI_SetStandAlone);
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
}

VOID CInPickArm::PreStartOperation()
{
	m_bModError = FALSE;
	m_bGlass1Exist = FALSE;
	m_bGlass2Exist = FALSE;
	m_bIsVacError = FALSE;

	if (!m_bModSelected)
	{	
		IN_PICKARM_Status = ST_MOD_NOT_SELECTED;
		m_qSubOperation = IN_PICKARM_STOP_Q;
		m_bStopCycle = FALSE;
	}
	else
	{
		if (
			((m_nLastError = SetWeakBlowSol(OFF)) != GMP_SUCCESS) || 
			((m_nLastError = MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) != GMP_SUCCESS) ||
			((m_nLastError = MoveX(IN_PICKARM_X_CTRL_GO_STANDBY_POSN, GMP_WAIT)) != GMP_SUCCESS) || 
			((m_nLastError = SetRotarySol(ON, GMP_WAIT)) != GMP_SUCCESS)	//ON = Pick, LOAD
		   )
		{
			Command(glSTOP_COMMAND, TRUE);
			IN_PICKARM_Status = ST_STOP;
			m_qSubOperation = IN_PICKARM_STOP_Q;
			m_bStopCycle = TRUE;
		}
		else
		{
			SetCOFMcReady(ON);
			SetCOFPickError(OFF);
			SetCOFPickAction(OFF);
			SetCOFVacOffReq(OFF);

			IN_PICKARM_Status = ST_IDLE;
			m_qSubOperation = IN_PICKARM_IDLE_Q;
			m_bStopCycle = FALSE;
			m_lPickPlaceRetryCounter = 0;
			
			if(IsBurnInDiagBond() && HMI_bStandAlone)
			{
				IN_PICKARM_GLASS_Status = ST_READY;
			}
			else
			{
				IN_PICKARM_GLASS_Status = ST_EMPTY;
			}
		}
	}

	if ((AutoMode == BURN_IN || AutoMode == DIAG_BOND) && m_bModuleNotMoveDuringAutoBurnIn)
	{
		m_stMotorX.m_bEnableMotorMove = FALSE;
	}
	else
	{
		m_stMotorX.m_bEnableMotorMove = TRUE;
	}

	Motion(FALSE);
	State(IDLE_Q);

	m_deqDebugSeq.clear();
	m_qPreviousSubOperation = m_qSubOperation;
	UpdateDebugSeq(GetSequenceString(), IN_PICKARM_Status, IN_PICKARM_GLASS_Status);
	
	CAC9000Stn::PreStartOperation();
}

VOID CInPickArm::AutoOperation()
{
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1");
	// stop command received: end operation after cycle completed
	if (Command() == glSTOP_COMMAND || g_bStopCycle)
	{
		m_bStopCycle = TRUE;
	}

	OperationSequence();

	// cycle completed or motor error or abort requested: end operation immediately
	if (
		(m_bModSelected && m_qSubOperation == IN_PICKARM_STOP_Q) || 
		(!m_bModSelected && m_bStopCycle) || 
		GMP_SUCCESS != m_nLastError || IsAbort() 
	   )
	{
		if (m_fHardware && m_bModSelected)
		{
			MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT);
			if (!m_bIsVacError)
			{
				MoveX(IN_PICKARM_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
			}
			else
			{
				DisplayMessage("InPickArm VAC error not move to standby");
			}
		}

		if (GMP_SUCCESS != m_nLastError)
		{
			m_bModError = TRUE;
		}

		m_stMotorX.m_bEnableMotorMove = TRUE;

		IN_PICKARM_Status = ST_STOP;

		SetCOFMcReady(OFF);
		SetCOFPickAction(OFF);
		SetCOFVacOffReq(OFF);

		Command(glSTOP_COMMAND);
		State(STOPPING_Q);
		m_bStopCycle = FALSE;

		CAC9000Stn::StopOperation();
	}
}

CString CInPickArm::GetSequenceString()
{
	switch (m_qSubOperation)
	{
	case IN_PICKARM_IDLE_Q:
		return "IN_PICKARM_IDLE_Q";

	case IN_PICKARM_CHECK_EXIST_Q:
		return "IN_PICKARM_CHECK_EXIST_Q";

	case IN_PICKARM_WAIT_FOR_PICK_Q:
		return "IN_PICKARM_WAIT_FOR_PICK_Q";

	case IN_PICKARM_WAIT_FOR_PLACE_Q:
		return "IN_PICKARM_WAIT_FOR_PLACE_Q";

	case IN_PICKARM_WAIT_TA1_ACK_Q:
		return "IN_PICKARM_WAIT_TA1_ACK_Q";

	case IN_PICKARM_PICK_COF_Q:
		return "IN_PICKARM_PICK_COF_Q";

	case IN_PICKARM_WAIT_USL_VAC_OFF_Q:
		return "IN_PICKARM_WAIT_USL_VAC_OFF_Q";

	case IN_PICKARM_CHECK_PICK_COF_EXIST_Q:
		return "IN_PICKARM_CHECK_PICK_COF_EXIST_Q";

	case IN_PICKARM_CHECK_USL_RESET_SEND_READY_Q:
		return "IN_PICKARM_CHECK_USL_RESET_SEND_READY_Q";

	case IN_PICKARM_CHECK_COF_STATUS_Q:
		return "IN_PICKARM_CHECK_COF_STATUS_Q";

	case IN_PICKARM_STOP_Q:
		return "IN_PICKARM_STOP_Q";
	}

	return "Unknown State";
}

VOID CInPickArm::OperationSequence()
{

	CTimeChart *pCTimeChart = (CTimeChart*)m_pModule->GetStation("TimeChart");
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");
	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");

	m_nLastError = GMP_SUCCESS;
	static BOOL bOK_COF;
	static BOOL bWaitUSLRun = FALSE; //20160819
	static BOOL bWaitUSLVacOff = FALSE; //20170213
	static clock_t clkNow = 0, clkStart = 0;


	CString szMsg = "";


	switch (m_qSubOperation)
	{
	case IN_PICKARM_IDLE_Q:	// 0
			IN_PICKARM_Status = ST_IDLE;
			m_qSubOperation = IN_PICKARM_CHECK_EXIST_Q;		//1
		break;

	case IN_PICKARM_CHECK_EXIST_Q:	// 1
		if (m_bStopCycle)
		{
			m_qSubOperation = IN_PICKARM_STOP_Q;	
		}
		else if(IsGLASS1_VacSensorOn() || (IsBurnInDiagBond() && IN_PICKARM_GLASS_Status == ST_READY))
		{
			IN_PICKARM_GLASS_Status = ST_READY;

			if(bOK_COF)
			{
				if (
					((m_nLastError = MoveX(IN_PICKARM_X_CTRL_GO_PREPLACE_POSN, GMP_NOWAIT)) == GMP_SUCCESS) && 
					((m_nLastError = MoveTPlacePosn(GMP_WAIT)) == GMP_SUCCESS)	
					)
				{
					IN_PICKARM_Status = ST_REQ_PLACE;
					m_qSubOperation = IN_PICKARM_WAIT_FOR_PLACE_Q;		//3
				}
			}
			else	//NG COF exists
			{
				if(	
					((m_nLastError = MoveX(IN_PICKARM_X_CTRL_GO_REJECT_BIN_POSN, GMP_WAIT)) == GMP_SUCCESS) &&
					((m_nLastError = RejectCOF()) == GMP_SUCCESS) 
					)
				{
					IN_PICKARM_GLASS_Status = ST_EMPTY;
					m_qSubOperation = IN_PICKARM_IDLE_Q;	//0
				}
			}
		}
		else	//No Glass Exist
		{
				IN_PICKARM_GLASS_Status = ST_EMPTY;
				IN_PICKARM_Status = ST_EMPTY;
				m_qSubOperation = IN_PICKARM_WAIT_FOR_PICK_Q;		//2
		}

		break;

	case IN_PICKARM_WAIT_FOR_PICK_Q:	// 2

		clkNow = clock(); //20170213

		if (m_bStopCycle)
		{
			m_qSubOperation = IN_PICKARM_STOP_Q;
			if (bWaitUSLRun) //20160819
			{
				CloseDisplayAlert(IDS_INPICKARM_WAIT_USL_READY_ERR);
				pCHouseKeeping->m_bAlertExist = FALSE;
			}
		}
		else if (
				(IsUSLMcReadyOn() && (IsUSLOkSendReadyOn() || IsUSLNgSendReadyOn())) || 
				/*IsBurnInDiagBond()*/HMI_bStandAlone
				) //20160513
		{
			if (bWaitUSLRun) //20160819
			{
				CloseDisplayAlert(IDS_INPICKARM_WAIT_USL_READY_ERR);
				pCHouseKeeping->m_bAlertExist = FALSE;
			}
			SetCOFPickAction(ON);
			m_qSubOperation = IN_PICKARM_PICK_COF_Q;		//5
		}
		else if (!bWaitUSLRun && !IsUSLMcReadyOn()) //20160819
		{
			bWaitUSLRun = TRUE;
			Sleep(50);
			SetAlert(IDS_INPICKARM_WAIT_USL_READY_ERR);
			WriteHMIMess("*** Waiting COF from USL ***", TRUE);
			Sleep(100);
		}
		else if ((DOUBLE)(labs(clkNow - clkStart)) / CLOCKS_PER_SEC > 60.0) //20170213 timeout
		{
			clkStart = clock();
			if (bWaitUSLRun) //20160819
			{
				CloseDisplayAlert(IDS_INPICKARM_WAIT_USL_READY_ERR);
				pCHouseKeeping->m_bAlertExist = FALSE;
			}
			bWaitUSLRun = TRUE;
			Sleep(50);
			SetAlert(IDS_INPICKARM_WAIT_USL_READY_ERR);
			WriteHMIMess("*** Waiting COF from USL Timeout ***", TRUE);
			Sleep(100);
		}

		break;

	case IN_PICKARM_WAIT_FOR_PLACE_Q:	// 3
		if (m_bStopCycle)
		{
			m_qSubOperation = IN_PICKARM_STOP_Q;	
		}
		else if(TA1_Status == ST_ACK_REQ_PLACE)
		{
			if (
				((m_nLastError = MoveX(IN_PICKARM_X_CTRL_GO_PLACE_GLASS_OFFSET, GMP_WAIT)) == GMP_SUCCESS) && 
				((m_nLastError = pCTA1->VacuumON(GMP_NOWAIT)) == GMP_SUCCESS) && 
				((m_nLastError = MoveZ(IN_PICKARM_Z_CTRL_GO_PLACE_LEVEL, GMP_WAIT)) == GMP_SUCCESS) && 
				((m_nLastError = SetVacSol(GLASS1, OFF, GMP_WAIT)) == GMP_SUCCESS) && 
				((m_nLastError = SetWeakBlowSol(ON, GMP_WAIT)) == GMP_SUCCESS) && 
				((m_nLastError = SetWeakBlowSol(OFF,GMP_NOWAIT)) == GMP_SUCCESS) && 
				((m_nLastError = MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) == GMP_SUCCESS) && 
				((m_nLastError = MoveX(IN_PICKARM_X_CTRL_GO_PICK_GLASS_OFFSET, GMP_NOWAIT)) == GMP_SUCCESS) && 
				((m_nLastError = MoveTPickPosn(GMP_NOWAIT)) == GMP_SUCCESS)		
				)
			{
				IN_PICKARM_GLASS_Status = ST_EMPTY;
				IN_PICKARM_Status = ST_EMPTY;
				m_qSubOperation = IN_PICKARM_WAIT_TA1_ACK_Q;		//4
			}
		}

		break;

	case IN_PICKARM_WAIT_TA1_ACK_Q:	// 4
		if (m_bStopCycle)
		{
			m_qSubOperation = IN_PICKARM_STOP_Q;	
		}
		else if(TA1_Status != ST_ACK_REQ_PLACE)
		{
			IN_PICKARM_Status = ST_IDLE;
			m_qSubOperation = IN_PICKARM_IDLE_Q;		//0
		}
		break;

	case IN_PICKARM_PICK_COF_Q:		//5
			if (!IsUSLMcReadyOn() && !/*IsBurnInDiagBond()*/HMI_bStandAlone)
			{
				SetCOFMcReady(OFF);
				SetCOFPickAction(OFF);
				SetCOFVacOffReq(OFF);

				bWaitUSLRun = FALSE; 
				clkStart = clock(); 
				m_qSubOperation = IN_PICKARM_WAIT_FOR_PICK_Q;		//2
			}
			else 
			{
				if(
					((m_nLastError = MoveX(IN_PICKARM_X_CTRL_GO_PICK_GLASS_OFFSET, GMP_WAIT)) == GMP_SUCCESS) &&
					((m_nLastError = MoveTPickPosn(GMP_WAIT)) == GMP_SUCCESS) &&
					((m_nLastError = SetGLASS1_VacSol(ON, GMP_NOWAIT)) == GMP_SUCCESS) &&
					((m_nLastError = MoveZ(IN_PICKARM_Z_CTRL_GO_PICK_LEVEL, GMP_WAIT)) == GMP_SUCCESS) &&
					((m_nLastError = SetGLASS1_VacSol(ON, GMP_WAIT)) == GMP_SUCCESS) 
					)
				{
					SetCOFVacOffReq(ON);
					clkStart = clock(); //20170213
					bWaitUSLVacOff = FALSE; //20170213
					m_qSubOperation = IN_PICKARM_WAIT_USL_VAC_OFF_Q;		//6
				}
			}
		break;

	case IN_PICKARM_WAIT_USL_VAC_OFF_Q:	// 6
		clkNow = clock(); //20170213

		if (m_bStopCycle && bWaitUSLVacOff)
		{
			//Stop and PickArm Up
			if (bWaitUSLVacOff) //20170213 timtout Check
			{
				bWaitUSLVacOff = FALSE;
				CloseDisplayAlert(IDS_INPICKARM_WAIT_USL_VAC_OFF_ERR);
				pCHouseKeeping->m_bAlertExist = FALSE;
			}

			if (
				(m_nLastError = SetGLASS1_VacSol(OFF, GMP_WAIT)) == GMP_SUCCESS &&
				(m_nLastError = MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) == GMP_SUCCESS &&
				(m_nLastError = MoveX(IN_PICKARM_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT)) == GMP_SUCCESS
				)
			{
				SetCOFMcReady(OFF);
				SetCOFPickAction(OFF);
				SetCOFVacOffReq(OFF);

				m_qSubOperation = IN_PICKARM_STOP_Q;
			}

		}
		else if (!IsUSLMcReadyOn() && !/*IsBurnInDiagBond()*/HMI_bStandAlone)
		{

			if (
				(m_nLastError = SetGLASS1_VacSol(OFF, GMP_WAIT)) == GMP_SUCCESS &&
				(m_nLastError = MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) == GMP_SUCCESS &&
				(m_nLastError = MoveX(IN_PICKARM_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT)) == GMP_SUCCESS
				)
			{
				SetCOFMcReady(OFF);
				SetCOFPickAction(OFF);
				SetCOFVacOffReq(OFF);

				clkStart = clock(); //20170213
				m_qSubOperation = IN_PICKARM_WAIT_FOR_PICK_Q;		//2
			}


			if (bWaitUSLVacOff) //20170213 timtout Check
			{
				bWaitUSLVacOff = FALSE;
				CloseDisplayAlert(IDS_INPICKARM_WAIT_USL_VAC_OFF_ERR);
				pCHouseKeeping->m_bAlertExist = FALSE;
			}
		}
		else if(IsUSLVacOffAckOn() || /*IsBurnInDiagBond()*/HMI_bStandAlone)
		{
			if(
				((m_nLastError = MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_NOWAIT)) == GMP_SUCCESS)
				)
			{
				Sleep(100);	//after Z moved 100ms check Vac Sensor
				m_qSubOperation = IN_PICKARM_CHECK_PICK_COF_EXIST_Q;		//7
			}

			if (bWaitUSLVacOff) //20170213 timtout Check
			{
				bWaitUSLVacOff = FALSE; //20160819
				CloseDisplayAlert(IDS_INPICKARM_WAIT_USL_VAC_OFF_ERR);
				pCHouseKeeping->m_bAlertExist = FALSE;
			}
		}
		else if (!bWaitUSLVacOff && ((DOUBLE)(labs(clkNow - clkStart)) / CLOCKS_PER_SEC > 20.0)) //20170213 timtout Check
		{
			bWaitUSLVacOff = TRUE;
			Sleep(50);
			SetAlert(IDS_INPICKARM_WAIT_USL_VAC_OFF_ERR);
			WriteHMIMess("*** Waiting USL VAC Off Signal ***", TRUE);
			Sleep(100);
		}

		break;

	case IN_PICKARM_CHECK_PICK_COF_EXIST_Q:	// 7

		if (IsGLASS1_VacSensorOn() || IsBurnInDiagBond())
		{
			if(IsBurnInDiagBond() || HMI_bStandAlone)
			{
				bOK_COF = TRUE;
			}
			else
			{
				bOK_COF = (IsUSLOkSendReadyOn() || !IsUSLNgSendReadyOn());
			}

			m_lPickPlaceRetryCounter = 0;
			IN_PICKARM_GLASS_Status = ST_READY;
			IN_PICKARM_Status = ST_READY;
			SetCOFPickAction(OFF);
			SetCOFVacOffReq(OFF);
			theApp.m_ulGlassPicked++;
			theApp.m_ulTotalGlassPicked++;
			SyncZ();
			m_qSubOperation = IN_PICKARM_CHECK_USL_RESET_SEND_READY_Q;		//8
		}
		else //Pick Retry
		{
			if ((m_lPickPlaceRetryLimit !=0) && (m_lPickPlaceRetryCounter < m_lPickPlaceRetryLimit))
			{
				m_lPickPlaceRetryCounter++;
				SyncZ();
				m_qSubOperation = IN_PICKARM_PICK_COF_Q;		//5
			}
			else	//Pick Failed, STOP
			{
				SyncZ();
				m_lPickPlaceRetryCounter = 0;
				SetCOFPickError(ON);
				SetError(IDS_INPICKARM_PICK_COF_ERR);
				IN_PICKARM_Status = ST_ERROR;
				m_qSubOperation = IN_PICKARM_STOP_Q;		//99
			}
		}


		break;

	case IN_PICKARM_CHECK_USL_RESET_SEND_READY_Q:	// 8
		
		if ((!IsUSLOkSendReadyOn() && !IsUSLNgSendReadyOn()) || /*IsBurnInDiagBond()*/HMI_bStandAlone)	//wait USL Reset Send Request
		{
			if(
				((m_nLastError = MoveX(IN_PICKARM_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT)) == GMP_SUCCESS)
				)
			{
				m_qSubOperation = IN_PICKARM_CHECK_COF_STATUS_Q;		//9
			}
		}

		break;

	case IN_PICKARM_CHECK_COF_STATUS_Q:	// 9

			IN_PICKARM_Status = ST_IDLE;
			m_qSubOperation = IN_PICKARM_IDLE_Q;		//0
		break;


	case IN_PICKARM_STOP_Q:	// 99
		break;
	}

	if (m_qPreviousSubOperation != m_qSubOperation)
	{
		if (HMI_bDebugSeq)
		{
			CString szMsg;
			if (IN_PICKARM_STOP_Q != m_qSubOperation)
			{
				szMsg.Format("InBufferTA Go State: %ld", m_qSubOperation);
			}
			else
			{
				szMsg.Format("InBufferTA Go Stop");
			}
			//DisplayMessage(szMsg);
		}
		UpdateDebugSeq(GetSequenceString(), IN_PICKARM_Status, IN_PICKARM_GLASS_Status);
		m_qPreviousSubOperation = m_qSubOperation;

		if (HMI_bDebugSeq)
		{
			PRINT_DATA	stPrintData;
			for (INT i = 0; i < MAX_PRINT_DATA; i++)
			{
				stPrintData.szTitle[i]	= "";
			}

			stPrintData.szTitle[0].Format("%ld", GetTickCount());
			stPrintData.szTitle[GetStationID() * MAX_STATION_SEQ_DATA + 1].Format("%s", GetSequenceString());
			stPrintData.szTitle[GetStationID() * MAX_STATION_SEQ_DATA + 2].Format("%ld", IN_PICKARM_Status);
			stPrintData.szTitle[GetStationID() * MAX_STATION_SEQ_DATA + 3].Format("%ld", IN_PICKARM_GLASS_Status);	

			LogFile->log(stPrintData.szTitle);
		}

	}

}

BOOL CInPickArm::CheckPickStatus()
{
	BOOL bStatus = FALSE;

	switch (PPAction)
	{
	case GLASS1:
		bStatus = IsVacSensorOn(GLASS1);
		break;

	//case GLASS2:
	//	bStatus = IsVacSensorOn(GLASS2);
	//	break;
	
	case MAX_NUM_OF_GLASS:
		bStatus = IsVacSensorOn(GLASS1);
		//bStatus = IsVacSensorOn(GLASS1) && IsVacSensorOn(GLASS2);
		break;

	default:
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage(__FUNCTION__);
		bStatus = FALSE;
	}

	return bStatus;
}

BOOL CInPickArm::SetPickStatus()
{
	BOOL bStatus = TRUE;

	m_bGlass1Exist = TRUE;
	m_bGlass2Exist = FALSE;

	IN_PICKARM_GLASS_Status = ST_READY;

	return bStatus;
}

BOOL CInPickArm::SetPlaceStatus()
{
	BOOL bStatus = TRUE;

	//INBUFFER_WH_Status = ST_PLACE_READY1_EMPTY2;

	return bStatus;
}


/////////////////////////////////////////////////////////////////
//Motor Related
/////////////////////////////////////////////////////////////////
//LONG CInPickArm::HMI_PowerOnZ(IPC_CServiceMessage &svMsg)
//{
//	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
//
//	BOOL bMode;
//	svMsg.GetMsg(sizeof(BOOL), &bMode);
//
//	m_stMotorZ.PowerOn(bMode);
//
//	bMode = TRUE;
//	svMsg.InitMessage(sizeof(BOOL), &bMode);
//	return 1;
//}
//
//LONG CInPickArm::HMI_CommZ(IPC_CServiceMessage &svMsg)
//{
//	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
//
//	m_stMotorZ.Commutate();
//
//	return 0;
//}
//
//
//LONG CInPickArm::HMI_HomeZ(IPC_CServiceMessage &svMsg)
//{
//	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
//
//	m_stMotorZ.Home(GMP_WAIT);
//
//	return 0;
//}



INT CInPickArm::MoveX(IN_PICKARM_X_CTRL_SIGNAL ctrl, BOOL bWait)
{
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1");

	INT		nResult		= GMP_SUCCESS;

	LONG lTempPosn = 0;

	switch (ctrl)
	{

	case IN_PICKARM_X_CTRL_GO_STANDBY_POSN:
		lTempPosn = m_lStandbyPosn;
		break;

	case IN_PICKARM_X_CTRL_GO_PICK_GLASS_POSN:
		lTempPosn = m_lPickGlassPosn;
		break;

	case IN_PICKARM_X_CTRL_GO_PICK_GLASS_OFFSET:
		lTempPosn = m_lPickGlassPosn + m_lPickGlassOffset;
		break;

	case IN_PICKARM_X_CTRL_GO_PREPLACE_POSN:
		lTempPosn = m_lPrePlaceGlassPosn;
		break;

	case IN_PICKARM_X_CTRL_GO_PLACE_GLASS_POSN:
		//lTempPosn = m_lPlaceGlassPosn + DistanceToCount(m_lPlaceXOffset, m_stMotorX.stAttrib.dDistPerCount);
		lTempPosn = m_lPlaceGlassPosn;
		break;

	case IN_PICKARM_X_CTRL_GO_PLACE_GLASS_OFFSET:
		lTempPosn = m_lPlaceGlassPosn + m_lPlaceGlassOffset;
		break;

	case IN_PICKARM_X_CTRL_GO_REJECT_BIN_POSN:
		lTempPosn = m_lRejectBinPosn;
		break;
	}

	return m_stMotorX.MoveAbsolute(lTempPosn, bWait);
}

//LONG CInPickArm::HMI_IndexZPos(IPC_CServiceMessage &svMsg)
//{
//	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
//
//	m_stMotorZ.MoveRelative(m_lDiagnSteps, GMP_WAIT);
//	return 0;
//}
//
//LONG CInPickArm::HMI_IndexZNeg(IPC_CServiceMessage &svMsg)
//{
//	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
//
//	m_stMotorZ.MoveRelative(-m_lDiagnSteps, GMP_WAIT);
//	return 0;
//}

INT CInPickArm::MoveZ(IN_PICKARM_Z_CTRL_SIGNAL ctrl, BOOL bWait)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	INT		nResult		= GMP_SUCCESS;

	LONG lTempPosn = 0;
	DOUBLE dDistPerCnt = 0.0;
	DOUBLE dGap = 0.0;

	dDistPerCnt	= m_stMotorZ.stAttrib.dDistPerCount;

	if (IsBurnInDiagBond())
	{
		dGap = DistanceToCount(GAP_THICKNESS * FPC_GAP * 1000.0, dDistPerCnt); 
	}
	else
	{
		dGap = 0.0;
	}
	switch (ctrl)
	{
	case IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL:
		lTempPosn = m_lStandbyLevel;
		break;

	case IN_PICKARM_Z_CTRL_GO_PICK_LEVEL:
		lTempPosn = m_lPickLevel + (LONG)dGap;
		break;

	case IN_PICKARM_Z_CTRL_GO_PLACE_LEVEL:
		lTempPosn = m_lPlaceLevel + (LONG)dGap;
		break;

	}

	return m_stMotorZ.MoveAbsolute(lTempPosn, GMP_WAIT);
}

INT CInPickArm::RejectCOF()
{
	INT	nResult	= GMP_SUCCESS;
	//addcode

	SetGLASS1_VacSol(OFF, GMP_WAIT);
	SetWeakBlowSol(ON, GMP_WAIT);
	SetWeakBlowSol(OFF, GMP_WAIT);

	return nResult;
}

INT CInPickArm::SetCOFMcReady(BOOL bMode, BOOL bWait)
{
	return SetFInOut1(bMode, bWait);
}

INT CInPickArm::SetCOFPickAction(BOOL bMode, BOOL bWait)
{
	return SetFInOut2(bMode, bWait);
}

INT CInPickArm::SetCOFVacOffReq(BOOL bMode, BOOL bWait)
{
	return SetFInOut3(bMode, bWait);
}

INT CInPickArm::SetCOFPickError(BOOL bMode, BOOL bWait)
{
	return SetFInOut4(bMode, bWait);
}


INT CInPickArm::SetFInOut1(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stFInOut1;

	if (!m_fHardware) //20160128
	{
		return GMP_SUCCESS;
	}

#ifdef OFFLINE
	stTempSo->bHMI_Status = bMode;
	return nResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}
#endif

	try
	{
		if (bMode)
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().On();
			stTempSo->bHMI_Status = TRUE;

			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;

			TC_DelJob(stTempSo->GetTCObjectID());
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CInPickArm::SetFInOut2(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stFInOut2;

	if (!m_fHardware) //20160128
	{
		return GMP_SUCCESS;
	}

#ifdef OFFLINE
	stTempSo->bHMI_Status = bMode;
	return nResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}
#endif

	try
	{
		if (bMode)
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().On();
			stTempSo->bHMI_Status = TRUE;

			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;

			TC_DelJob(stTempSo->GetTCObjectID());
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CInPickArm::SetFInOut3(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stFInOut3;

	if (!m_fHardware) //20160128
	{
		return GMP_SUCCESS;
	}

#ifdef OFFLINE
	stTempSo->bHMI_Status = bMode;
	return nResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}
#endif

	try
	{
		if (bMode)
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().On();
			stTempSo->bHMI_Status = TRUE;

			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;

			TC_DelJob(stTempSo->GetTCObjectID());
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CInPickArm::SetFInOut4(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stFInOut4;

	if (!m_fHardware) //20160128
	{
		return GMP_SUCCESS;
	}

#ifdef OFFLINE
	stTempSo->bHMI_Status = bMode;
	return nResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}
#endif

	try
	{
		if (bMode)
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().On();
			stTempSo->bHMI_Status = TRUE;

			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;

			TC_DelJob(stTempSo->GetTCObjectID());
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CInPickArm::SetFInOut5(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stFInOut5;

	if (!m_fHardware) //20160128
	{
		return GMP_SUCCESS;
	}

#ifdef OFFLINE
	stTempSo->bHMI_Status = bMode;
	return nResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}
#endif

	try
	{
		if (bMode)
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().On();
			stTempSo->bHMI_Status = TRUE;

			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;

			TC_DelJob(stTempSo->GetTCObjectID());
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CInPickArm::SetFInOut6(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stFInOut6;

	if (!m_fHardware) //20160128
	{
		return GMP_SUCCESS;
	}

#ifdef OFFLINE
	stTempSo->bHMI_Status = bMode;
	return nResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}
#endif

	try
	{
		if (bMode)
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().On();
			stTempSo->bHMI_Status = TRUE;

			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;

			TC_DelJob(stTempSo->GetTCObjectID());
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

// ------------------------------------------------------------------------
// HMI Command delcaration
// ------------------------------------------------------------------------
LONG CInPickArm::HMI_HomeAll(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	BOOL	bMode = TRUE;

	if (
		m_stMotorZ.Home(GMP_WAIT) != GMP_SUCCESS ||
		MoveTPickPosn(GMP_WAIT) != GMP_SUCCESS ||
		m_stMotorX.Home(GMP_WAIT) != GMP_SUCCESS
	   )
	{
		HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
	}

	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CInPickArm::HMI_PickGlassOperation(IPC_CServiceMessage &svMsg)
{
	BOOL bOpDone						= TRUE;
	LONG lResponse						= 0;
	BOOL bResult						= FALSE;

	bResult = PickGlassOperation(NO_GLASS);
	
	if (bResult)
	{
		MoveX(IN_PICKARM_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
	}

	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}

LONG CInPickArm::HMI_PlaceGlassOperation(IPC_CServiceMessage &svMsg)
{
	BOOL bOpDone						= TRUE;
	BOOL bResult						= FALSE;
	
	bResult = PlaceGlassOperation(NO_GLASS);

	if (bResult)
	{
		MoveX(IN_PICKARM_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
	}

	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}

LONG CInPickArm::HMI_ToggleFInOut1(IPC_CServiceMessage &svMsg)
{
	if (m_stFInOut1.bHMI_Status)
	{
		SetFInOut1(OFF);
	}
	else
	{
		SetFInOut1(ON);
	}
	return 0;
}

LONG CInPickArm::HMI_ToggleFInOut2(IPC_CServiceMessage &svMsg)
{
	if (m_stFInOut2.bHMI_Status)
	{
		SetFInOut2(OFF);
	}
	else
	{
		SetFInOut2(ON);
	}
	return 0;
}

LONG CInPickArm::HMI_ToggleFInOut3(IPC_CServiceMessage &svMsg)
{
	if (m_stFInOut3.bHMI_Status)
	{
		SetFInOut3(OFF);
	}
	else
	{
		SetFInOut3(ON);
	}
	return 0;
}

LONG CInPickArm::HMI_ToggleFInOut4(IPC_CServiceMessage &svMsg)
{
	if (m_stFInOut4.bHMI_Status)
	{
		SetFInOut4(OFF);
	}
	else
	{
		SetFInOut4(ON);
	}

	return 0;
}

LONG CInPickArm::HMI_ToggleFInOut5(IPC_CServiceMessage &svMsg)
{
	if (m_stFInOut5.bHMI_Status)
	{
		SetFInOut5(OFF);
	}
	else
	{
		SetFInOut5(ON);
	}

	return 0;
}

LONG CInPickArm::HMI_ToggleFInOut6(IPC_CServiceMessage &svMsg)
{
	if (m_stFInOut6.bHMI_Status)
	{
		SetFInOut6(OFF);
	}
	else
	{
		SetFInOut6(ON);
	}
	return 0;
}

LONG CInPickArm::HMI_SetStandAlone(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	
	if (bMode)
	{
		HMI_bStandAlone = TRUE;
	}
	else
	{
		HMI_bStandAlone = FALSE;
	}

	return 0;
}

INT CInPickArm::GetPickAction()
{
	if (AutoMode == BURN_IN || AutoMode == DIAG_BOND)
	{
		PPAction = MAX_NUM_OF_GLASS;
		return GMP_SUCCESS;
	}

	//if (!IsUpStreamInLineSnrOn() || !IsUpStreamReadySnrOn())
	//{
	//	return GMP_FAIL;
	//}
	//else if (
	//	IsUpStreamPanelTransferLeftSnrOn() &&
	//	IsUpStreamPanelTransferRightSnrOn()
	//	)
	//{
	//	PPAction = MAX_NUM_OF_GLASS;
	//	return GMP_SUCCESS;
	//}
	//else if (IsUpStreamPanelTransferLeftSnrOn())
	//{
	//	PPAction = GLASS1;
	//	return GMP_SUCCESS;
	//}
	//else if (IsUpStreamPanelTransferRightSnrOn())
	//{
	//	PPAction = GLASS2;
	//	return GMP_SUCCESS;
	//}
	else
	{
		return ST_IDLE;
	}
}

//GlassNum CInPickArm::GetUpStreamGlassStatus()
//{
//	return MAX_NUM_OF_GLASS;
//}



BOOL CInPickArm::IsUSLBackSideDoorSnrOn() //20160511
{
	BOOL bResult = FALSE;

#ifdef OFFLINE
	return bResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		return bResult;
	}
#endif

	try
	{
		bResult = m_stUSLBackSideDoorSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CInPickArm::IsUSLMcReadyOn() //20160511
{
	BOOL bResult = FALSE;

#ifdef OFFLINE
	return bResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		return bResult;
	}
#endif

	try
	{
		bResult = m_stFInIn1.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CInPickArm::IsUSLOkSendReadyOn() //20160511
{
	BOOL bResult = FALSE;

#ifdef OFFLINE
	return bResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		return bResult;
	}
#endif

	try
	{
		bResult = m_stFInIn2.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CInPickArm::IsUSLNgSendReadyOn() //20160511
{
	BOOL bResult = FALSE;

#ifdef OFFLINE
	return bResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		return bResult;
	}
#endif

	try
	{
		bResult = m_stFInIn3.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CInPickArm::IsUSLVacOffAckOn() //20160511
{
	BOOL bResult = FALSE;

#ifdef OFFLINE
	return bResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		return bResult;
	}
#endif

	try
	{
		bResult = m_stFInIn4.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CInPickArm::PickGlassOperation(GlassNum lGlassNum)
{
	CString strMsg;

	BOOL bWHGlassStatus = FALSE;
	BOOL bArmGlassStatus = FALSE;
	BOOL bResult = TRUE;

	if (!m_bModSelected)
	{
		strMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
		return FALSE;
	}

	if (lGlassNum != NO_GLASS)
	{
		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Contact Software");
		return FALSE;
	}

	// Check current glass status
	bArmGlassStatus = IsVacSensorOn(GLASS1);

	if (!bArmGlassStatus)
	{
		SetVacSol(GLASS1, OFF);
	}
	else
	{
		strMsg.Format("FPC exist on PickArm. Operation Abort!");
		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
		return FALSE;
	}


	//bWHGlassStatus = IsUpStreamPanelTransferLeftSnrOn();

	GlassNum lPPAction = GLASS1;
	if (
		MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		MoveTPickPosn(GMP_WAIT) != GMP_SUCCESS ||
		MoveX(IN_PICKARM_X_CTRL_GO_PICK_GLASS_OFFSET, GMP_WAIT) != GMP_SUCCESS
		)
	{
		return FALSE;
	}

	//lPPAction = GLASS1;
	//SetVacSol(lPPAction, ON, GMP_WAIT);
	if (
		PickGlass(GLASS1) != GMP_SUCCESS
		)
	{
		return FALSE;
	}

	//Check Glass Status
	if (!IsVacSensorOn(GLASS1))
	{
		strMsg.Format("Pick Glass Error: %s Glass Vac Sensor Not On.", GetStnName());
		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
		bResult = FALSE;
	}

	if (bResult)
	{
		if (	MoveX(IN_PICKARM_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS)
		{
			bResult = FALSE;
		}
	}
	return bResult;
}

BOOL CInPickArm::PlaceGlassOperation(GlassNum lGlassNum)
{
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1");

	CString szMsg;
	BOOL bWHGlassStatus = FALSE;
	BOOL bArmGlassStatus = FALSE;
	BOOL bResult = TRUE;
	GlassNum ePPAction = MAX_NUM_OF_GLASS;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", szMsg);
		return FALSE;
	}

	if (!pCTA1->m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", pCTA1->GetStnName());
		HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", szMsg);
		return FALSE;
	}

	if (lGlassNum != NO_GLASS)
	{
		HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", "Contact Software");
		return FALSE;
	}

	SetVacSol(GLASS1, ON);
	pCTA1->SetVacSol(GLASS1, ON);

	Sleep(GetMaxValue(m_stGlass1VacSol.GetOnDelay(), pCTA1->m_stGlass1VacSol.GetOnDelay()));

	// Check current glass status
	bArmGlassStatus = IsVacSensorOn(GLASS1);
	bWHGlassStatus = pCTA1->IsVacSensorOn(GLASS1);

	if (!bArmGlassStatus)
	{
		SetVacSol(GLASS1, OFF);
	}

	if (!bWHGlassStatus)
	{
		pCTA1->SetVacSol(GLASS1, OFF);
	}
	
	if (!bArmGlassStatus)
	{
		szMsg.Format("%s Glass Not Exist. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", szMsg);
		return FALSE;
	}

	if (bWHGlassStatus)
	{
		szMsg.Format("Glass Exist on TA1 already. Operation Abort!");
		HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", szMsg);
		return FALSE;
	}

	ePPAction = GLASS1;
	lGlassNum = GLASS1;

	if (
		SetWeakBlowSol(OFF, GMP_WAIT) != GMP_SUCCESS || 
		MoveTPlacePosn(GMP_WAIT) != GMP_SUCCESS ||
		MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		pCTA1->MoveX(TA1_X_CTRL_GO_PICK_GLASS_POSN, GMP_WAIT) != GMP_SUCCESS ||
		pCTA1->MoveZ(TA1_Z_CTRL_GO_LOWER_LEVEL, GMP_NOWAIT) != GMP_SUCCESS ||
		pCTA1->MoveTPickPosn(GMP_WAIT) != GMP_SUCCESS ||
		pCTA1->SyncZ() != GMP_SUCCESS ||
		MoveX(IN_PICKARM_X_CTRL_GO_PLACE_GLASS_OFFSET, GMP_WAIT) != GMP_SUCCESS 
	   )
	{
		return FALSE;
	}

	if (
		PlaceGlass(GLASS1) != GMP_SUCCESS 

		//MoveZ(IN_PICKARM_Z_CTRL_GO_PLACE_LEVEL, GMP_NOWAIT) != GMP_SUCCESS ||
		//m_stMotorZ.Sync() != GMP_SUCCESS ||
		//pCTA1->SetVacSol(ePPAction, ON, GMP_WAIT) != GMP_SUCCESS ||
		//(bArmGlassStatus && SetVacSol(GLASS1, OFF)  != GMP_SUCCESS) ||
		//SetWeakBlowSol(ON, GMP_WAIT) != GMP_SUCCESS || 
		//SetWeakBlowSol(OFF, GMP_WAIT) != GMP_SUCCESS ||
		//MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, SFM_NOWAIT) != GMP_SUCCESS ||
		//m_stMotorZ.Sync() != GMP_SUCCESS
	   )
	{
		return FALSE;
	}

	//Check Glass Status
	if (!pCTA1->IsGLASS1_VacSensorOn())
	{
		SetGLASS1_VacSol(ON, GMP_WAIT);
		szMsg.Format("Place Glass Error: %s Glass Vac Sensor Not On.", pCTA1->GetStnName());
		HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", szMsg);
		bResult = FALSE;
	}

	if (bResult)
	{
		if (MoveX(IN_PICKARM_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS)
		{
			bResult = FALSE;
		}
	}

	return bResult;
}

//INT CInPickArm::PickGlass_Down(GlassNum lGlassNum)
//{
//	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1");
//	LONG lDelay = 0;
//
//	if (lGlassNum <= NO_GLASS || lGlassNum > MAX_NUM_OF_GLASS || pCTA1 == NULL)
//	{                  
//		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
//		return GMP_FAIL;
//	}
//
//	SetVacSol(lGlassNum, ON, GMP_WAIT);
//
//	switch(lGlassNum)
//	{
//	case GLASS1:
//	//case GLASS2:
//	case MAX_NUM_OF_GLASS:
//		if (	MoveZ(IN_PICKARM_Z_CTRL_GO_PICK_LEVEL, GMP_WAIT) == GMP_SUCCESS)
//		{
//			return GMP_SUCCESS;
//		}
//		break;
//	}
//
//	return GMP_FAIL;
//}

//INT CInPickArm::PickGlass_Up(GlassNum lGlassNum)
//{
//	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1");
//	LONG lDelay = 0;
//
//	if (lGlassNum <= NO_GLASS || lGlassNum > MAX_NUM_OF_GLASS || pCTA1 == NULL)
//	{                  
//		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
//		return GMP_FAIL;
//	}
//
//	SetVacSol(lGlassNum, ON, GMP_NOWAIT);
//
//	switch(lGlassNum)
//	{
//	case GLASS1:
////	case GLASS2:
//	case MAX_NUM_OF_GLASS:
//		if (	MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) == GMP_SUCCESS)
//		{
//			return GMP_SUCCESS;
//		}
//		break;
//	}
//
//	return GMP_FAIL;
//}

INT CInPickArm::PickGlass(GlassNum lGlassNum)
{
	//CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1");
	LONG lDelay = 0;

	if (lGlassNum <= NO_GLASS || lGlassNum > MAX_NUM_OF_GLASS /* || pCTA1 == NULL*/)
	{                  
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		return GMP_FAIL;
	}

	if (HMI_bStandAlone)
	{
		if (
			MoveZ(IN_PICKARM_Z_CTRL_GO_PICK_LEVEL, GMP_WAIT) == GMP_SUCCESS &&
			//TODO UPstreamWh VAC Off
			SetVacSol(lGlassNum, ON, GMP_WAIT) == GMP_SUCCESS &&
			MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) == GMP_SUCCESS
			)
		{
			return GMP_SUCCESS;
		}
	}
	else
	{
		LONG lReply = BUTTON_3;
		
		lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "USL Turret Move To Next Posn?");
		if (lReply == BUTTON_1)
		{
			SetCOFMcReady(ON);
		}
		else if (lReply == BUTTON_3)
		{
			return GMP_FAIL;
		}

		if (
			MoveZ(IN_PICKARM_Z_CTRL_GO_PICK_LEVEL, GMP_WAIT) == GMP_SUCCESS &&
			SetVacSol(lGlassNum, ON, GMP_WAIT) == GMP_SUCCESS &&
			//TODO UPstreamWh VAC Off
			SetCOFPickAction(ON) == GMP_SUCCESS &&
			SetCOFVacOffReq(ON) == GMP_SUCCESS &&
			MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) == GMP_SUCCESS &&
			SetCOFMcReady(OFF) == GMP_SUCCESS 
			)
		{
			return GMP_SUCCESS;
		}
	}

	return GMP_FAIL;
}

INT CInPickArm::PlaceGlass(GlassNum lGlassNum)
{
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1");
	LONG lDelay = 0;

	if (pCTA1 == NULL)
	{
		return GMP_FAIL;
	}

	pCTA1->SetVacSol(lGlassNum, ON, GMP_WAIT);

	lGlassNum = GLASS1;
	if (
		MoveZ(IN_PICKARM_Z_CTRL_GO_PLACE_LEVEL, GMP_WAIT) == GMP_SUCCESS &&
		SetVacSol(lGlassNum, OFF) == GMP_SUCCESS &&
		(m_stWeakBlowSol.GetOnDelay() <= 0 || SetWeakBlowSol(ON, GMP_WAIT) == GMP_SUCCESS ) &&
		(m_stWeakBlowSol.GetOnDelay() <= 0 || SetWeakBlowSol(OFF, GMP_WAIT) == GMP_SUCCESS ) &&
		SleepWithReturn(m_stGlass1VacSol.GetOffDelay()) == GMP_SUCCESS &&
		MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) == GMP_SUCCESS
		)
	{
		return GMP_SUCCESS;
	}
	return GMP_FAIL;
}

/////////////////////////////////////////////////////////////////
//SETUP Related
/////////////////////////////////////////////////////////////////
VOID CInPickArm::ModuleSetupPreTask()
{
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1");

	m_bCheckMotor		= TRUE;
	m_bCheckIO			= TRUE;

	pCTA1->m_bCheckIO	= TRUE;

	CheckModSelected(TRUE);
	
	SetDiagnSteps(g_lDiagnSteps);

	UpdateModuleSetupPosn();
	UpdateModuleSetupLevel();
	UpdateModuleSetupPara();
}

VOID CInPickArm::ModuleSetupPostTask()
{
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1");

	m_bCheckMotor		= FALSE;
	m_bCheckIO			= FALSE;
	pCTA1->m_bCheckIO	= FALSE;
}

BOOL CInPickArm::ModuleSetupSetPosn(LONG lSetupTitle)
{
	BOOL		bResult	= TRUE;
	MTR_POSN	mtrOld = {0, 0};
	MTR_POSN TempPosn = {0, 0};

	LONG lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "OPTION", "Overwrite Position?");
	if (lAnswer == rMSG_TIMEOUT || lAnswer == rMSG_CANCEL)
	{
		HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
		return FALSE;
	}

	//if(g_stSetupPosn[lSetupTitle].lType == MACHINE_PARA && g_ulUserGroup < SERVICE)
	//{
	//	HMIMessageBox(MSG_OK, "WARNING", "You are not allowed to change Machine Parameters");
	//	return FALSE;
	//}

	TempPosn.x = m_stMotorX.GetEncPosn();

	switch (lSetupTitle)
	{
	case 0:
		mtrOld.x			= m_lStandbyPosn;

		m_lStandbyPosn		= TempPosn.x;
		break;

	case 1:
		mtrOld.x			= m_lPickGlassPosn;

		m_lPickGlassPosn	= TempPosn.x;
		break;

	case 2:
		mtrOld.x			= m_lPlaceGlassPosn;

		m_lPlaceGlassPosn	= TempPosn.x;
		break;

	case 3:

		mtrOld.x			= m_lPickGlassOffset;

		m_lPickGlassOffset	= TempPosn.x - m_lPickGlassPosn;
		break;

	case 4:
		mtrOld.x			= m_lPlaceGlassOffset;

		m_lPlaceGlassOffset	= TempPosn.x - m_lPlaceGlassPosn;
		break;

	case 5:
		mtrOld.x			= m_lPrePlaceGlassPosn;

		m_lPrePlaceGlassPosn	= TempPosn.x;
		break;

	case 6:
		mtrOld.x			= m_lRejectBinPosn;

		m_lRejectBinPosn	= TempPosn.x;
		break;

	}

	CAC9000Stn::ModuleSetupSetPosn(lSetupTitle, "X", mtrOld.x, TempPosn.x);

	return TRUE;
}

BOOL CInPickArm::ModuleSetupSetLevel(LONG lSetupTitle)
{
	LONG lAnswer = rMSG_TIMEOUT;
	LONG lOldLevel		= 0;
	LONG lTempLevelZ = 0;
	BOOL bResult = TRUE;

	lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Overwrite Position?");

	if (lAnswer == rMSG_CANCEL || lAnswer == rMSG_TIMEOUT)
	{
		return FALSE;
	}

	// Release me
	//if(g_stSetupLevel[lSetupTitle].lType == MACHINE_PARA && g_ulUserGroup < SERVICE)
	//{
	//	HMIMessageBox(MSG_OK, "WARNING", "You are not allowed to change Machine Parameters");
	//	return FALSE;
	//}

	lTempLevelZ = m_stMotorZ.GetEncPosn();

	switch (lSetupTitle)
	{
	case 0:
		lOldLevel					= m_lStandbyLevel;

		m_lStandbyLevel				= lTempLevelZ;
		break;
	
	case 1:
		lOldLevel					= m_lPickLevel;

		m_lPickLevel				= lTempLevelZ;
		break;

	case 2:
		lOldLevel					= m_lPlaceLevel;

		m_lPlaceLevel				= lTempLevelZ;
		break;

	}

	CAC9000Stn::ModuleSetupSetLevel(lSetupTitle, lOldLevel, lTempLevelZ);

	return bResult;
}

BOOL CInPickArm::ModuleSetupGoPosn(LONG lSetupTitle)
{
	INT nResult = GMP_SUCCESS;
	CString szMsg;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "GO POSN OPERATION", szMsg);
		return FALSE;
	}

	if (
			MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS 
		)	
	{
		return FALSE;
	}

	switch (lSetupTitle)
	{
	case 0:
		nResult = SetupGoStandbyPosn();
		break;
	
	case 1:
		nResult = SetupGoPickGlassPosn();
		break;
		
	case 2:
		nResult = SetupGoPlaceGlassPosn();
		break;

	case 3:
		nResult = SetupGoPickGlassOffset();
		break;

	case 4:
		nResult = SetupGoPlaceGlassOffset();
		break;

	case 5:
		nResult = SetupGoPrePlaceGlassPosn();
		break;

	case 6:
		nResult = SetupGoRejectBinPosn();
		break;
	
	}
	
	return nResult;
}

BOOL CInPickArm::ModuleSetupGoLevel(LONG lSetupTitle)
{
	INT nResult = GMP_SUCCESS;

	CString szTitle = "";
	CString szMsg = "";
	szTitle.Format("GO LEVEL OPERATION");
	szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());

	if (!m_bModSelected)
	{
		HMIMessageBox(MSG_OK, szTitle, szMsg);
		return FALSE;
	}
	
	switch (lSetupTitle)
	{
	case 0:
		nResult = SetupGoStandbyLevel();
		break;

	case 1:
		nResult = SetupGoPickGlassLevel();
		break;

	case 2:
		nResult = SetupGoPlaceGlassLevel();
		break;

	}
	if (nResult != GMP_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CInPickArm::ModuleSetupSetPara(PARA stPara)
{
	switch (stPara.lSetupTitle)
	{
	case 0:
		m_stGlass1VacSol.SetOnDelay(stPara.lPara);
		//m_stGlass2VacSol.SetOnDelay(stPara.lPara);
		break;

	case 1:
		m_stGlass1VacSol.SetOffDelay(stPara.lPara);
		//m_stGlass2VacSol.SetOffDelay(stPara.lPara);
		break;

	case 2:
		m_stWeakBlowSol.SetOnDelay(stPara.lPara);
		break;

	case 3:
		m_stWeakBlowSol.SetOffDelay(stPara.lPara);
		break;

	case 4:
		m_lPickPlaceRetryLimit = stPara.lPara;
		break;

	case 5:
		m_stRotarySol.SetOnDelay(stPara.lPara);
		break;

	case 6:
		m_stRotarySol.SetOffDelay(stPara.lPara);
		break;
	}

	return CAC9000Stn::ModuleSetupSetPara(stPara);
}

VOID CInPickArm::UpdateModuleSetupPosn()
{
	//CInBufferWH *pCInBufferWH	= (CInBufferWH*)m_pModule->GetStation("InBufferWH");

	INT i = 0;
	INT j = 0;

	// Assign Module Setup Posn/Level/Para
	// Position Tab
	i = 0;
	g_stSetupPosn[i].szPosn.Format("Standby Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX		= m_lStandbyPosn;
	g_stSetupPosn[i].lPosnY		= 0;

	i++;	// 1
	g_stSetupPosn[i].szPosn.Format("Pick Glass Posn");	
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPickGlassPosn;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 2
	g_stSetupPosn[i].szPosn.Format("Place Glass Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPlaceGlassPosn;
	g_stSetupPosn[i].lPosnY			= 0;
	
	i++;	// 3
	g_stSetupPosn[i].szPosn.Format("Pick Glass Offset");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= DEVICE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPickGlassOffset;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 4
	g_stSetupPosn[i].szPosn.Format("Place Glass Offset");	
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= DEVICE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPlaceGlassOffset;
	g_stSetupPosn[i].lPosnY			= 0;
	
	i++;	// 5
	g_stSetupPosn[i].szPosn.Format("Pre Place Glass Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPrePlaceGlassPosn;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 6
	g_stSetupPosn[i].szPosn.Format("Reject Bin Posn");	
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lRejectBinPosn;
	g_stSetupPosn[i].lPosnY			= 0;

	for (j = i + 1; j < NUM_OF_POSN_XY; j++)
	{
		g_stSetupPosn[j].szPosn.Format("Reserved");
		g_stSetupPosn[j].bEnable		= FALSE;
		g_stSetupPosn[j].bVisible		= FALSE;
		g_stSetupPosn[j].lType			= MACHINE_PARA;
		g_stSetupPosn[j].lPosnX			= 0;
		g_stSetupPosn[j].lPosnY			= 0;
	}
}

VOID CInPickArm::UpdateModuleSetupLevel()
{
	INT i = 0;
	INT j = 0;

	// Level Tab
	i = 0;
	g_stSetupLevel[i].szLevel.Format("Z Standby Level");
	g_stSetupLevel[i].bEnable		= TRUE;
	g_stSetupLevel[i].bVisible		= TRUE;
	g_stSetupLevel[i].lType			= MACHINE_PARA;
	g_stSetupLevel[i].lLevelZ		= m_lStandbyLevel;

	i++;	// 1
	g_stSetupLevel[i].szLevel.Format("Z Pick Level");
	g_stSetupLevel[i].bEnable		= TRUE;
	g_stSetupLevel[i].bVisible		= TRUE;
	g_stSetupLevel[i].lType			= MACHINE_PARA;
	g_stSetupLevel[i].lLevelZ		= m_lPickLevel;

	i++;	// 2
	g_stSetupLevel[i].szLevel.Format("Z Place Level");
	g_stSetupLevel[i].bEnable		= TRUE;
	g_stSetupLevel[i].bVisible		= TRUE;
	g_stSetupLevel[i].lType			= MACHINE_PARA;
	g_stSetupLevel[i].lLevelZ		= m_lPlaceLevel;

	for (j = i + 1; j < NUM_OF_LEVEL_Z; j++)
	{
		g_stSetupLevel[j].szLevel.Format("Reserved");
		g_stSetupLevel[j].bEnable		= FALSE;
		g_stSetupLevel[j].bVisible		= FALSE;
		g_stSetupLevel[j].lType			= MACHINE_PARA;
		g_stSetupLevel[j].lLevelZ		= 0;
	}
}

VOID CInPickArm::UpdateModuleSetupPara()
{
	INT i = 0;
	INT j = 0;

	// Para Tab
	// Delay
	i = 0;
	g_stSetupPara[i].szPara.Format("Vac On Delay (ms)");
	g_stSetupPara[i].bEnable		= TRUE;
	g_stSetupPara[i].bVisible		= TRUE;
	g_stSetupPara[i].lType			= DEVICE_PARA;
	g_stSetupPara[i].lPara			= m_stGlass1VacSol.GetOnDelay();
	g_stSetupPara[i].dPara			= 0.0;

	i++;	// 1
	g_stSetupPara[i].szPara.Format("Vac Off Delay (ms)");
	g_stSetupPara[i].bEnable		= TRUE;
	g_stSetupPara[i].bVisible		= TRUE;
	g_stSetupPara[i].lType			= DEVICE_PARA;
	g_stSetupPara[i].lPara			= m_stGlass1VacSol.GetOffDelay();
	g_stSetupPara[i].dPara			= 0.0;

	i++;	// 2
	g_stSetupPara[i].szPara.Format("WeakBlow On Delay (ms)");
	g_stSetupPara[i].bEnable		= TRUE;
	g_stSetupPara[i].bVisible		= TRUE;
	g_stSetupPara[i].lType			= DEVICE_PARA;
	g_stSetupPara[i].lPara			= m_stWeakBlowSol.GetOnDelay();
	g_stSetupPara[i].dPara			= 0.0;

	i++;	// 3
	g_stSetupPara[i].szPara.Format("WeakBlow Off Delay (ms)");
	g_stSetupPara[i].bEnable		= TRUE;
	g_stSetupPara[i].bVisible		= TRUE;
	g_stSetupPara[i].lType			= DEVICE_PARA;
	g_stSetupPara[i].lPara			= m_stWeakBlowSol.GetOffDelay();
	g_stSetupPara[i].dPara			= 0.0;

	i++;	// 4
	g_stSetupPara[i].szPara.Format("Pick / Place Retry Limit");
	g_stSetupPara[i].bEnable		= TRUE;
	g_stSetupPara[i].bVisible		= TRUE;
	g_stSetupPara[i].lType			= DEVICE_PARA;
	g_stSetupPara[i].lPara			= m_lPickPlaceRetryLimit;
	g_stSetupPara[i].dPara			= 0;

	i++;	// 5
	g_stSetupPara[i].szPara.Format("T Rotary Cylinder On Delay (ms)");
	g_stSetupPara[i].bEnable		= TRUE;
	g_stSetupPara[i].bVisible		= TRUE;
	g_stSetupPara[i].lType			= DEVICE_PARA;
	g_stSetupPara[i].lPara			= m_stRotarySol.GetOnDelay();
	g_stSetupPara[i].dPara			= 0.0;

	i++;	// 6
	g_stSetupPara[i].szPara.Format("T Rotary Cylinder Off Delay (ms)");
	g_stSetupPara[i].bEnable		= TRUE;
	g_stSetupPara[i].bVisible		= TRUE;
	g_stSetupPara[i].lType			= DEVICE_PARA;
	g_stSetupPara[i].lPara			= m_stRotarySol.GetOffDelay();
	g_stSetupPara[i].dPara			= 0.0;

	// Others
	for (i = i + 1; i < NUM_OF_PARA; i++)
	{
		g_stSetupPara[i].szPara.Format("Reserved");
		g_stSetupPara[i].bEnable		= FALSE;
		g_stSetupPara[i].bVisible		= FALSE;
		g_stSetupPara[i].lType			= MACHINE_PARA;
		g_stSetupPara[i].lPara			= 0;
		g_stSetupPara[i].dPara			= 0.0;
	}
}

INT CInPickArm::SetupGoStandbyPosn()
{
	INT nResult = GMP_SUCCESS;

	if (
		MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS
		)
	{
		return GMP_FAIL;
	}

	nResult = MoveX(IN_PICKARM_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);

	return nResult;
}

INT CInPickArm::SetupGoPrePlaceGlassPosn()
{
	INT nResult = GMP_SUCCESS;

	if (
		MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS
		)
	{
		return GMP_FAIL;
	}

	nResult = MoveX(IN_PICKARM_X_CTRL_GO_PREPLACE_POSN, GMP_WAIT);

	return nResult;
}

INT CInPickArm::SetupGoRejectBinPosn()
{
	INT nResult = GMP_SUCCESS;

	if (
		MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS
		)
	{
		return GMP_FAIL;
	}

	nResult = MoveX(IN_PICKARM_X_CTRL_GO_REJECT_BIN_POSN, GMP_WAIT);

	return nResult;
}

INT CInPickArm::SetupGoPickGlassPosn()
{
	INT nResult = GMP_SUCCESS;

	if (
		MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS
		)
	{
		return GMP_FAIL;
	}

				DOUBLE dProcessTime = 0.0;
				DWORD dwStartTime = GetTickCount();
	nResult = MoveX(IN_PICKARM_X_CTRL_GO_PICK_GLASS_POSN, GMP_WAIT);

//Sleep(500);
				dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
//LONG i = m_stMotorX.stMotionProfile.dAcc;
	return nResult;
}

INT CInPickArm::SetupGoPlaceGlassPosn()
{
	INT nResult = GMP_SUCCESS;

	if (
		MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS 
		)
	{
		return GMP_FAIL;
	}

	nResult = MoveX(IN_PICKARM_X_CTRL_GO_PLACE_GLASS_POSN, GMP_WAIT);

	return nResult;
}

INT CInPickArm::SetupGoPickGlassOffset()
{
	INT nResult = GMP_SUCCESS;

	if (
		MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS
		)
	{
		return GMP_FAIL;
	}

	nResult = MoveX(IN_PICKARM_X_CTRL_GO_PICK_GLASS_OFFSET, GMP_WAIT);

	return nResult;
}

INT CInPickArm::SetupGoPlaceGlassOffset()
{
	INT nResult = GMP_SUCCESS;

	if (
		MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS
		)
	{
		return GMP_FAIL;
	}

	nResult = MoveX(IN_PICKARM_X_CTRL_GO_PLACE_GLASS_OFFSET, GMP_WAIT);

	return nResult;
}

INT CInPickArm::SetupGoStandbyLevel()
{
	INT nResult = GMP_SUCCESS;

	nResult = MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT);

	return nResult;
}

INT CInPickArm::SetupGoPickGlassLevel()
{
	INT nResult = GMP_SUCCESS;

	nResult = MoveZ(IN_PICKARM_Z_CTRL_GO_PICK_LEVEL, GMP_WAIT);

	return nResult;
}

INT CInPickArm::SetupGoPlaceGlassLevel()
{
	INT nResult = GMP_SUCCESS;

	nResult = MoveZ(IN_PICKARM_Z_CTRL_GO_PLACE_LEVEL, GMP_WAIT);

	return nResult;
}



LONG CInPickArm::IPC_SaveMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString str;

	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lStandbyPosn"]		= m_lStandbyPosn;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPickGlassPosn"]		= m_lPickGlassPosn;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPlaceGlassPosn"]		= m_lPlaceGlassPosn;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPrePlaceGlassPosn"]		= m_lPrePlaceGlassPosn;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lRejectBinPosn"]		= m_lRejectBinPosn;

	pAppMod->m_smfMachine[GetStnName()]["Level"]["Z"]["m_lStandbyLevel"]		= m_lStandbyLevel;
	pAppMod->m_smfMachine[GetStnName()]["Level"]["Z"]["m_lPickLevel"]		= m_lPickLevel;
	pAppMod->m_smfMachine[GetStnName()]["Level"]["Z"]["m_lPlaceLevel"]		= m_lPlaceLevel;

	return CTransferArm::IPC_SaveMachineParam();
}

LONG CInPickArm::IPC_LoadMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString str;

	m_lStandbyPosn = pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lStandbyPosn"];
	m_lPickGlassPosn = pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPickGlassPosn"];
	m_lPlaceGlassPosn = pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPlaceGlassPosn"];
	m_lPrePlaceGlassPosn = pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPrePlaceGlassPosn"];
	m_lRejectBinPosn = pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lRejectBinPosn"];

	m_lStandbyLevel = pAppMod->m_smfMachine[GetStnName()]["Level"]["Z"]["m_lStandbyLevel"];
	m_lPickLevel = pAppMod->m_smfMachine[GetStnName()]["Level"]["Z"]["m_lPickLevel"];
	m_lPlaceLevel = pAppMod->m_smfMachine[GetStnName()]["Level"]["Z"]["m_lPlaceLevel"];

	return CTransferArm::IPC_LoadMachineParam();
}

LONG CInPickArm::IPC_SaveDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lPickGlassOffset"]	= m_lPickGlassOffset;
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lPlaceGlassOffset"]		= m_lPlaceGlassOffset;

	pAppMod->m_smfDevice[GetStnName()]["Settings"]["HMI_bStandAlone"]	= HMI_bStandAlone;

	return CTransferArm::IPC_SaveDeviceParam();
}

LONG CInPickArm::IPC_LoadDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	m_lPickGlassOffset = pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lPickGlassOffset"];
	m_lPlaceGlassOffset = pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lPlaceGlassOffset"];

	HMI_bStandAlone			= (BOOL)(LONG)pAppMod->m_smfDevice[GetStnName()]["Settings"]["HMI_bStandAlone"];

	return CTransferArm::IPC_LoadDeviceParam();
}
