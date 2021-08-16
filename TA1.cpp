/////////////////////////////////////////////////////////////////
//	TA1.cpp : interface of the CTA1 class
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

#include "PRTaskStn.h"
#include "stdafx.h"
#include "MarkConstant.h"
#include "AC9000.h"
#include "AC9000_Constant.h"
#include "Cal_Util.h"
#include "PR_Util.h"

#define TA1_EXTERN
#include "TA1.h"
#include "TA2.h"
#include "ACFWH.h"
#include "ACF1WH.h"
#include "ACF2WH.h"
#include "InPickArm.h"
#include "WinEagle.h"

#include "SettingFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CTA1, CTransferArm)

CTA1::CTA1()
{
	//I/O and Motors are done!!
	// CAC9000Srn
	m_lStnErrorCode		= IDS_TA1_NOT_SELECTED_ERR;
	m_lModSelectMode	= GLASS_MOD;	

#ifdef TA1_Y
	m_szPRTaskStnName	= GetStation().Trim('_');
	
	m_lAddJobPR_GRAB	= ACFUL_PR_GRAB;
	m_lAddJobPR_PROCESS = ACFUL_PR_PROCESS;
	
	m_lPRStnErrorCode	= IDS_TA1_PR_MOD_NOT_SELECTED;
	m_lPRModSelectMode	= GLASS_MOD;
#endif

	// Motor Related
	m_stMotorX.InitMotorInfo("TA1_X", &m_bModSelected);	//done
	m_stMotorZ.InitMotorInfo("TA1_Z", &m_bModSelected);
#ifdef TA1_Y
	m_stMotorY.InitMotorInfo("TA1_Y", &m_bModSelected);
#endif

	m_myMotorList.Add((CMotorInfo*)&m_stMotorZ);		// For Home Z firstly
	m_myMotorList.Add((CMotorInfo*)&m_stMotorX);
#ifdef TA1_Y
	m_myMotorList.Add((CMotorInfo*)&m_stMotorY);
#endif


	// Si Related
	m_stGlass1VacSnr			= CSiInfo("TA1VACSnr",				"IOMODULE_IN_PORT_4", BIT_0, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stGlass1VacSnr);	
	m_stT1Snr					= CSiInfo("TA1RotaryUpperSnr",		"IOMODULE_IN_PORT_4", BIT_1, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stT1Snr);	
	m_stT2Snr					= CSiInfo("TA1RotaryLowerSnr",		"IOMODULE_IN_PORT_4", BIT_2, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stT2Snr);	
	m_stXLmtSnr					= CSiInfo("TA1XLmtSnr",				"IOMODULE_IN_PORT_4", BIT_3, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stXLmtSnr);	
	m_stZHomeSnr			= CSiInfo("TA1FlipZHomeSnr",		"IOMODULE_IN_PORT_4", BIT_4, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stZHomeSnr);
	m_stZLmtSnr				= CSiInfo("TA1FlipZLmtSnr",			"IOMODULE_IN_PORT_4", BIT_5, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stZLmtSnr);
	m_stYLmtSnr			= CSiInfo("TA1YHomeSnr",		"IOMODULE_IN_PORT_4", BIT_6, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stYLmtSnr);


	// So Related
	m_stGlass1VacSol		= CSoInfo("TA1VacSol",			"IOMODULE_OUT_PORT_3", BIT_0, TA1_SOL, ACTIVE_LOW);
	m_mySoList.Add((CSoInfo*)&m_stGlass1VacSol);
	m_stWeakBlowSol			= CSoInfo("TA1WBSol",			"IOMODULE_OUT_PORT_3", BIT_2, TA1_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stWeakBlowSol);
	m_stRotarySol			= CSoInfo("TA1TSol",			"IOMODULE_OUT_PORT_3", BIT_4, TA1_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stRotarySol);
	//m_stZBrakeSol			= CSoInfo("TA1ZBrakeSol",		"IOMODULE_OUT_PORT_3", BIT_6, TA1_SOL, ACTIVE_LOW);
	//m_mySoList.Add((CSoInfo*)&m_stZBrakeSol);

	m_lPickGlassOffset	= 0;
	
	m_lStandbyLevel		= 0;
	m_lPickLevel		= 0;
	m_bIsVacError = FALSE; //20121224 vac error
	HMI_bStandAlone		= FALSE;

	m_bUseFPCVacuum = FALSE; //20141112

	m_lZMoveLoopLimit	= 5; //20150615
	m_lZMoveLoopCount	= 0;

	m_lXPlaceWhPROffset = 0;
	m_lYPlaceWhPROffset = 0;
	m_lTPlaceWhPROffset = 0;

}

CTA1::~CTA1()
{
}

BOOL CTA1::InitInstance()
{
	CAC9000Stn::InitInstance();

	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CGmpSystem *pGmpSys = pAppMod->m_pCGmpSystem;
	CString		szMsg	= _T("");
	CInPickArm *pCInPickArm	= (CInPickArm*)m_pModule->GetStation("InPickArm"); 

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
				if( GetmySoList().GetAt(i)->GetName() != "TA1TSol")
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

		
		// Avoid Crashing with INWH. 
		// Request by mi 04 july 2011
		// TA1 Z Communicate and Home First
		// TA1 X Communicate
		// TA1 X Home
		// Commutate Motor
		if (
			m_stMotorZ.Commutate() != GMP_SUCCESS ||
			m_stMotorZ.Home(GMP_WAIT) != GMP_SUCCESS ||
			m_stMotorZ.EnableProtection() != GMP_SUCCESS ||
			//MoveTPlacePosn(GMP_WAIT) != GMP_SUCCESS ||
#ifdef TA1_Y
			m_stMotorY.Commutate() != GMP_SUCCESS ||
#endif
			m_stMotorX.Commutate() != GMP_SUCCESS
		   )
		{
			m_bInitInstanceError			= TRUE;
			pAppMod->m_bHWInitError			= TRUE;
		}

		pAppMod->m_bTA1Comm = TRUE;

		// wait for all servo commutated (even with error)
		do
		{
			Sleep(100);
		} while (!pAppMod->IsAllServoCommutated());

#if 1

		DisplayMessage("TA1 waiting INTA home finish before start homing.");
		do
		{
			Sleep(100);
		} while (!pAppMod->m_bInPickArmHome && !pCInPickArm->m_bInitInstanceError);

		DisplayMessage("TA1 start home.");
#endif

		if (!m_bInitInstanceError)
		{
			szMsg = _T("	") + GetName() + " --- homing motors";
			DisplayMessage(szMsg);

			// Home Motor
			if (
				m_stMotorX.Home(GMP_WAIT) != GMP_SUCCESS ||
#ifdef TA1_Y
				m_stMotorY.Home(GMP_WAIT) != GMP_SUCCESS ||
				m_stMotorY.EnableProtection() != GMP_SUCCESS ||
#endif
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
			pAppMod->m_bTA1Home	= TRUE;
		}
	}
	else
	{
		pAppMod->m_bTA1Comm = TRUE;
		pAppMod->m_bTA1Home	= TRUE;
	}
	return TRUE;
}

INT CTA1::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CTransferArm::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CTA1::Operation()
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
VOID CTA1::UpdateOutput()
{
	// Keep it for debug breakpoint

	CTransferArm::UpdateOutput();
}

VOID CTA1::RegisterVariables()
{
	try
	{
		//Register Sensors here 
		// -------------------------------------------------------
		// AC9000Stn Variable and Function
		// -------------------------------------------------------
		RegVariable(GetStation() + _T("bCheckMotor"), &m_bCheckMotor);
		RegVariable(GetStation() + _T("bCheckIO"), &m_bCheckIO);

		RegVariable(GetStation() + _T("bModSelected"), &m_bModSelected);
		RegVariable(GetStation() + _T("bShowModSelected"), &m_bShowModSelected);
		RegVariable(GetStation() + _T("bModError"), &m_bModError);

		RegVariable(GetStation() + _T("bGlass1Exist"), &m_bGlass1Exist);
		//RegVariable(GetStation() + _T("bGlass2Exist"), &m_bGlass2Exist); //edge-cleaner

		RegVariable(GetStation() + _T("bDebugSeq"), &HMI_bDebugSeq);
		RegVariable(GetStation() + _T("bLogMotion"), &HMI_bLogMotion);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetModSelected"), HMI_SetModSelected);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeAll"), HMI_HomeAll);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDebugSeq"), HMI_SetDebugSeq);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetLogMotion"), HMI_SetLogMotion);

		// -------------------------------------------------------
		// TransferArm Variable and Function
		// -------------------------------------------------------

		RegVariable(_T("HMI_bStandAlone"), &HMI_bStandAlone);
		RegVariable(GetStation() + _T("lPickPlaceRetryLimit"), &m_lPickPlaceRetryLimit);


		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleG1VacSol"), HMI_ToggleG1VacSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleWeakBlowSol"), HMI_ToggleWeakBlowSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleTSol"), HMI_ToggleTSol);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPickPlaceRetryLimit"), HMI_SetPickPlaceRetryLimit);

		// -------------------------------------------------------
		// varibales
		// -------------------------------------------------------
		// hardware
		RegVariable(_T("HMI_bTA1XOn"), &m_stMotorX.bHMI_On);
		RegVariable(_T("HMI_lTA1XEncoder"), &m_stMotorX.lHMI_CurrPosn);
		RegVariable(GetStation() + _T("dXDistPerCount"), &m_stMotorX.stAttrib.dDistPerCount);

		RegVariable(_T("HMI_bTA1ZOn"), &m_stMotorZ.bHMI_On);
		RegVariable(_T("HMI_lTA1ZEncoder"), &m_stMotorZ.lHMI_CurrPosn);
		RegVariable(GetStation() + _T("dZDistPerCount"), &m_stMotorZ.stAttrib.dDistPerCount);

		RegVariable(_T("HMI_bTA1YOn"), &m_stMotorY.bHMI_On);
		RegVariable(_T("HMI_lTA1YEncoder"), &m_stMotorY.lHMI_CurrPosn);
		RegVariable(GetStation() + _T("dYDistPerCount"), &m_stMotorY.stAttrib.dDistPerCount);

		RegVariable(_T("HMI_bTA1G1VacSnr"), &m_stGlass1VacSnr.bHMI_Status);
		RegVariable(_T("HMI_bTA1RotaryUpperSnr"), &m_stT1Snr.bHMI_Status);
		RegVariable(_T("HMI_bTA1RotaryLowerSnr"), &m_stT2Snr.bHMI_Status);
		RegVariable(_T("HMI_bTA1XLmtSnr"), &m_stXLmtSnr.bHMI_Status);
		RegVariable(_T("HMI_bTA1ZHomeSnr"), &m_stZHomeSnr.bHMI_Status);
		RegVariable(_T("HMI_bTA1ZLmtSnr"), &m_stZLmtSnr.bHMI_Status);
		RegVariable(_T("HMI_bTA1YLmtSnr"), &m_stYLmtSnr.bHMI_Status);

		RegVariable(_T("HMI_bTA1G1VacSol"), &m_stGlass1VacSol.bHMI_Status);
		RegVariable(_T("HMI_bTA1WeakBlowSol"), &m_stWeakBlowSol.bHMI_Status);
		RegVariable(_T("HMI_bTA1TRotateSol"), &m_stRotarySol.bHMI_Status);
		RegVariable(_T("HMI_bTA1ZBrakeSol"), &m_stZBrakeSol.bHMI_Status);

		RegVariable(_T("HMI_lTA1ZMoveLoopLimit"), &m_lZMoveLoopLimit); //20150615
		RegVariable(_T("HMI_lTA1ZMoveLoopCount"), &m_lZMoveLoopCount);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetZMoveLoopLimit"), HMI_SetZMoveLoopLimit);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("LoopTestZMove"), HMI_LoopTestZMove);

		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleLFPCVacSol"), HMI_ToggleLFPCVacSol);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleRFPCVacSol"), HMI_ToggleRFPCVacSol);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnX"), HMI_PowerOnX);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("CommX"), HMI_CommX);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeX"), HMI_HomeX);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnZ"), HMI_PowerOnZ);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("CommZ"), HMI_CommZ);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeZ"), HMI_HomeZ);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnY"), HMI_PowerOnY);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("CommY"), HMI_CommY);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeY"), HMI_HomeY);
	
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDiagnSteps"), HMI_SetDiagnSteps);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexXPos"), HMI_IndexXPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexXNeg"), HMI_IndexXNeg);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexZPos"), HMI_IndexZPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexZNeg"), HMI_IndexZNeg);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexYPos"), HMI_IndexYPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexYNeg"), HMI_IndexYNeg);

		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PickGlassOperation"), HMI_PickGlassOperation);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PlaceGlassOperation"), HMI_PlaceGlassOperation);

		// -------------------------------------------------------
		// CPRTaskStn Variable and Function
		// -------------------------------------------------------
		RegVariable(GetStation() + _T("bPRSelected"), &m_bPRSelected);	

		RegVariable(GetStation() + _T("lPRDelay"), &m_lPRDelay);
		RegVariable(GetStation() + _T("lCalibPRDelay"), &m_lCalibPRDelay);
		RegVariable(GetStation() + _T("lPRRetryLimit"), &m_lPRRetryLimit);
		RegVariable(GetStation() + _T("lRejectGlassLimit"), &m_lRejectGlassLimit);

		RegVariable(GetStation() + _T("ulCurPRU"), &HMI_ulCurPRU);
		RegVariable(GetStation() + _T("ulPRID"), &HMI_ulPRID);
		//RegVariable(GetStation() + _T("ulCurGlass"), &m_lCurGlass);
		RegVariable(GetStation() + _T("dPRMatchScore"), &HMI_dPRMatchScore);
		RegVariable(GetStation() + _T("dPRAngleRange"), &HMI_dPRAngleRange);
		RegVariable(GetStation() + _T("dPRPercentVar"), &HMI_dPRPercentVar);
		RegVariable(GetStation() + _T("bShowPRSearch"), &HMI_bShowPRSearch);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRMatchScore"), HMI_SetPRMatchScore);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRAngRange"), HMI_SetPRAngRange);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRPercentVar"), HMI_SetPRPercentVar);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SrchPR"), HMI_SrchPR);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ModifyPRCriteria"), HMI_ModifyPRCriteria);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRRetryLimit"), HMI_SetPRRetryLimit);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetRejectGlassLimit"), HMI_SetRejectGlassLimit);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRDelay"), HMI_SetPRDelay);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetCalibPRDelay"), HMI_SetCalibPRDelay);

		RegVariable(GetStation() + _T("bPR1Loaded"),	&pruIN1.bLoaded);
		RegVariable(GetStation() + _T("bAlignPtSet"),	&m_stACFUL.bAlignPtSet);
		RegVariable(GetStation() + _T("bValid"),	&m_stACFUL.bValid);

		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SelectGlassNum"), HMI_SelectGlassNum);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRSelected"), HMI_SetPRSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("UpdateSetupStatus"), HMI_UpdateSetupStatus);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SelectPRU"), HMI_SelectPRU);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRPosn"), HMI_SetPRPosn);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRLevel"), HMI_SetPRLevel);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPR1Posn"), HMI_SetPR1Posn);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPR2Posn"), HMI_SetPR2Posn);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SearchAp1"), HMI_SearchAp1);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SearchAp2"), HMI_SearchAp2);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SearchCentre"), HMI_SearchCentre);
		
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
}

// ------------------------------------------------------------------------
// Virtual TransferArm Function delcaration
// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
// END Virtual TransferArm Function delcaration
// ------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
//Modified Virtual Operational Functions
/////////////////////////////////////////////////////////////////
VOID CTA1::PreStartOperation()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	m_bModError = FALSE;
	m_bGlass1Exist = FALSE;
	m_bGlass2Exist = FALSE;
	m_bIsVacError = FALSE; //20121224 vac error

	//GLASS_FPC_VAC_STATE stVacState1 = GetGlassFPC1_VacSensorOnState();	// 20141112
	//GLASS_FPC_VAC_STATE stVacState2 = GetGlassFPC2_VacSensorOnState();	// 20141112

	if (!m_bModSelected)
	{	
		TA1_Status = ST_MOD_NOT_SELECTED;
		m_qSubOperation = TA1_STOP_Q;
		m_bStopCycle = FALSE;
	}
	else
	{
		if (
			((m_nLastError = SetWeakBlowSol(OFF)) != GMP_SUCCESS) || 
			((m_nLastError = MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) != GMP_SUCCESS) ||
#ifdef TA1_Y
			((m_nLastError = MoveY(TA1_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT)) != GMP_SUCCESS) ||
#endif
			((m_nLastError = MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT)) != GMP_SUCCESS) 
			)
		{
			Command(glSTOP_COMMAND, TRUE);
			TA1_Status = ST_STOP;
			m_qSubOperation = TA1_STOP_Q;
			m_bStopCycle = TRUE;
		}
		else
		{

			if(IsGLASS1_VacSensorOn())
			{
				TA1_GLASS_Status = ST_READY;
			}
			else
			{
				TA1_GLASS_Status = ST_EMPTY;
			}

			TA1_POSN = ACFWH_NONE;
			TA1_Status = ST_IDLE;
			m_qSubOperation = TA1_IDLE_Q;
			m_bStopCycle = FALSE;
			m_lPickPlaceRetryCounter = 0;
		}
	}

	Motion(FALSE);
	State(IDLE_Q);

	m_deqDebugSeq.clear(); //20131010
	m_qPreviousSubOperation = m_qSubOperation;
	//UpdateDebugSeq(GetSequenceString(), TA1_Status, m_bGlass1Exist, m_bGlass2Exist);
	UpdateDebugSeq(GetSequenceString(), TA1_Status, m_bGlass1Exist);

	CAC9000Stn::PreStartOperation();
}

VOID CTA1::AutoOperation()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	// stop command received: end operation after cycle completed

	if (Command() == glSTOP_COMMAND || g_bStopCycle)
	{
		m_bStopCycle = TRUE;
	}

	OperationSequence();

	// cycle completed or motor error or abort requested: end operation immediately
	if (
		(m_bModSelected && m_qSubOperation == TA1_STOP_Q) || 
		(!m_bModSelected && m_bStopCycle) || 
		GMP_SUCCESS != m_nLastError || IsAbort() 
	   )
	{
		if (m_fHardware && m_bModSelected)
		{
			//if(TA1_Status != ST_ERROR)		//Dont Go UP When STOP, BACK to Standby Level in PreStart
			//{
			//	MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT);
			//	MoveTPlacePosn(GMP_WAIT);
			//	MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
			//}

			if (AutoMode == BURN_IN)
			{
				//if (!IsVacStateOn(GetGlassFPC1_VacSensorOnState()))
				if(!IsGLASS1_VacSensorOn())
				{
					SetGlassFPC1_VacSol(OFF);

				}
			}
		}

		if (GMP_SUCCESS != m_nLastError)
		{
			m_bModError = TRUE;
		}

		TA1_Status = ST_STOP;

		Command(glSTOP_COMMAND);
		State(STOPPING_Q);
		m_bStopCycle = FALSE;

		CAC9000Stn::StopOperation();
	}
}

/////////////////////////////////////////////////////////////////
//Operational Sequences
/////////////////////////////////////////////////////////////////
CString CTA1::GetSequenceString()
{
	switch (m_qSubOperation)
	{
	case TA1_IDLE_Q:
		return "TA1_IDLE_Q";

	case TA1_WAIT_INPICKARM_REQUEST_Q:
		return "TA1_WAIT_INPICKARM_REQUEST_Q";

	case TA1_WAIT_INPICKARM_PLACE_COMPLETE_Q:
		return "TA1_WAIT_INPICKARM_PLACE_COMPLETE_Q";

	case TA1_CHECK_PICK_ERROR_Q:
		return "TA1_CHECK_PICK_ERROR_Q";

	case TA1_WAIT_TA_MANAGER_REQUEST_Q:
		return "TA1_WAIT_TA_MANAGER_REQUEST_Q";

	case TA1_PLACE_COF_Q:
		return "TA1_PLACE_COF_Q";

	case TA1_CHECK_PLACE_COF_Q:
		return "TA1_CHECK_PLACE_COF_Q";

	case TA1_DO_2PT_PR_Q:
		return "TA1_DO_2PT_PR_Q";

	case TA1_GRAB_PR2_OK_Q:
		return "TA1_GRAB_PR2_OK_Q";

	case TA1_SEARCH_PR2_OK_Q:
		return "TA1_SEARCH_PR2_OK_Q";

	case TA1_GRAB_N_SEARCH_PR1_OK_Q:
		return "TA1_GRAB_N_SEARCH_PR1_OK_Q";

	case TA1_REJECT_COF_Q:
		return "TA1_REJECT_COF_Q";

	case TA1_DO_BIG_FOV_PR_Q:
		return "TA1_DO_BIG_FOV_PR_Q";

	case TA1_STOP_Q:
		return "TA1_STOP_Q";
	}

	return "ASSASSINATE JASON";
}

VOID CTA1::OperationSequence()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH	= (CACF2WH*)m_pModule->GetStation("ACF2WH");

//	CACFWH *pCACFxWH = NULL;
	m_nLastError = GMP_SUCCESS;

	//GLASS_FPC_VAC_STATE stVacState1;	// 20141112
	//GLASS_FPC_VAC_STATE stVacState2;	// 20141112

//	TA1_X_CTRL_SIGNAL lTempPosn;

	CString szMsg = "";

	switch (m_qSubOperation)
	{
	case TA1_IDLE_Q:	// 0
		if (m_bStopCycle)
		{
			m_qSubOperation = TA1_STOP_Q;	
		}			
		else if(IsGLASS1_VacSensorOn())
		{
			if (
				//((m_nLastError = MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) == GMP_SUCCESS) &&
#ifdef TA1_Y
				((m_nLastError = MoveZ(TA1_Z_CTRL_GO_PR_LEVEL, GMP_NOWAIT)) == GMP_SUCCESS) &&
#endif
				((m_nLastError = MoveTPlacePosn(GMP_WAIT)) == GMP_SUCCESS) 
				)
			{
				TA1_Status = ST_BUSY;
#ifdef TA1_Y
				//m_qSubOperation = TA1_DO_2PT_PR_Q;		//7
				m_qSubOperation = TA1_DO_BIG_FOV_PR_Q;		//12
#else 
				m_qSubOperation = TA1_WAIT_TA_MANAGER_REQUEST_Q;		//4
#endif
			}
		}
		else	//No COF
		{
			if (
				((m_nLastError = MoveTPickPosn(GMP_WAIT)) == GMP_SUCCESS) &&
				((m_nLastError = MoveZ(TA1_Z_CTRL_GO_LOWER_LEVEL, GMP_WAIT)) == GMP_SUCCESS) &&
				((m_nLastError = MoveX(TA1_X_CTRL_GO_PICK_GLASS_POSN, GMP_WAIT)) == GMP_SUCCESS) 
				)
			{
				TA1_Status = ST_EMPTY;
				m_qSubOperation = TA1_WAIT_INPICKARM_REQUEST_Q;		//1
			}
		}

		break;

	case TA1_WAIT_INPICKARM_REQUEST_Q:	// 1

		if (m_bStopCycle)
		{
			m_qSubOperation = TA1_STOP_Q;	
		}
		else if(IN_PICKARM_Status == ST_REQ_PLACE)
		{
			TA1_Status = ST_ACK_REQ_PLACE;
			m_qSubOperation = TA1_WAIT_INPICKARM_PLACE_COMPLETE_Q;		//2
		}

		break;

	case TA1_WAIT_INPICKARM_PLACE_COMPLETE_Q:	// 2
		
		if (IN_PICKARM_Status == ST_STOP)
		{
			m_qSubOperation = TA1_STOP_Q;	
		}
		else if(IN_PICKARM_Status == ST_EMPTY)
		{
			TA1_Status = ST_READY;
			m_qSubOperation = TA1_CHECK_PICK_ERROR_Q;		//3
		}

		break;

	case TA1_CHECK_PICK_ERROR_Q:	// 3
		
		if(IsGLASS1_VacSensorOn() || IsBurnInDiagBond()) //Pick OK
		{
			TA1_GLASS_Status = ST_READY;
			
			if (
				((m_nLastError = MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_NOWAIT)) == GMP_SUCCESS) &&
				((m_nLastError = MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT)) == GMP_SUCCESS) &&
#ifdef TA1_Y
				((m_nLastError = MoveZ(TA1_Z_CTRL_GO_PR_LEVEL, GMP_NOWAIT)) == GMP_SUCCESS) &&
#endif
				((m_nLastError = MoveTPlacePosn(GMP_WAIT)) == GMP_SUCCESS) 
				)
			{
				TA1_Status = ST_BUSY;
#ifdef TA1_Y
				//m_qSubOperation = TA1_DO_2PT_PR_Q;		//7
				m_qSubOperation = TA1_DO_BIG_FOV_PR_Q;		//12
#else 
				m_qSubOperation = TA1_WAIT_TA_MANAGER_REQUEST_Q;		//4
#endif
			}
		}
		else	//Pick NOT_OK
		{
			TA1_Status = ST_ERROR;
			m_qSubOperation = TA1_STOP_Q;	
			DisplayMessage("TA1 ERROR: Pick Glass Retry Limit Hit");
			m_bModError = TRUE;
			SetError(IDS_TA1_PICK_ERR);
			m_bIsVacError = TRUE; //20121224 vac error	
		}
		break;

	case TA1_REJECT_COF_Q:		//11
			
			if(
				((m_nLastError = MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT)) == GMP_SUCCESS) &&
				((m_nLastError = BlowCOF()) == GMP_SUCCESS)
				)
			{
				TA1_Status = ST_IDLE;
				m_qSubOperation = TA1_IDLE_Q;		//0
			}	

		break;

	case TA1_DO_BIG_FOV_PR_Q:			//12

		if(g_bUplookPREnable)
		{
			if(
				((m_nLastError = MoveXYToBigFOVPRPosn(GMP_WAIT)) == GMP_SUCCESS)
				)
			{
				if(IsBigFOVPROK())	//PR OK?
				{
					MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_NOWAIT);
					UpdatePlaceWHPosnOffset();
					m_qSubOperation = TA1_WAIT_TA_MANAGER_REQUEST_Q;		//4
				}
				else	//UL PR Error
				{
					
					m_qSubOperation = TA1_REJECT_COF_Q;		//11
				}
			}
		}
		else
		{
			PlaceOffsetEqualZero();
			m_qSubOperation = TA1_WAIT_TA_MANAGER_REQUEST_Q;		//4
		}
		break;

	case TA1_WAIT_TA_MANAGER_REQUEST_Q:	// 4
		
		if (m_bStopCycle)
		{
			m_qSubOperation = TA1_STOP_Q;	
		}		
		else if((TA_MANAGER_Status == ST_ACFWH1_REQ_LOAD) || (TA_MANAGER_Status == ST_ACFWH2_REQ_LOAD))
		{
			if(TA_MANAGER_Status == ST_ACFWH1_REQ_LOAD)
			{
				m_lCurrCheck = TA1_POSN = ACFWH_1;
			//	pCACFxWH = pCACF1WH;
				TA1_Status = ST_ACFWH1_ACK_LOAD;


				if(g_bUplookPREnable)
				{
					if (
						((m_nLastError = pCACF1WH->MoveRelativeY(-m_lYPlaceWhPROffset, GMP_NOWAIT)) == GMP_SUCCESS) &&
						((m_nLastError = pCACF1WH->MoveRelativeT(-m_lTPlaceWhPROffset, GMP_NOWAIT)) == GMP_SUCCESS) &&
						((m_nLastError = MoveX(TA1_X_CTRL_GO_PLACE_ACF1WH_PR_OFFSET, GMP_WAIT)) == GMP_SUCCESS) 
						)
					{
						pCACF1WH->SyncY();
						pCACF1WH->SyncT();
						m_qSubOperation = TA1_PLACE_COF_Q;		//5
					}
				}
				else
				{
					if (
						((m_nLastError = MoveX(TA1_X_CTRL_GO_PLACE_ACF1WH_OFFSET, GMP_WAIT)) == GMP_SUCCESS)
						)
					{
						m_qSubOperation = TA1_PLACE_COF_Q;		//5
					}
				}
			}
			else if(TA_MANAGER_Status == ST_ACFWH2_REQ_LOAD)
			{
				m_lCurrCheck = TA1_POSN = ACFWH_2;
			//	pCACFxWH = pCACF2WH;
				TA1_Status = ST_ACFWH2_ACK_LOAD;

				if(g_bUplookPREnable)
				{
					if (
						((m_nLastError = pCACF2WH->MoveRelativeY(-m_lYPlaceWhPROffset, GMP_NOWAIT)) == GMP_SUCCESS) &&
						((m_nLastError = pCACF2WH->MoveRelativeT(-m_lTPlaceWhPROffset, GMP_NOWAIT)) == GMP_SUCCESS) &&
						((m_nLastError = MoveX(TA1_X_CTRL_GO_PLACE_ACF2WH_PR_OFFSET, GMP_WAIT)) == GMP_SUCCESS) 
						)
					{
						pCACF2WH->SyncY();
						pCACF2WH->SyncT();
						m_qSubOperation = TA1_PLACE_COF_Q;		//5
					}
				}
				else
				{
					if (
						((m_nLastError = MoveX(TA1_X_CTRL_GO_PLACE_ACF2WH_OFFSET, GMP_WAIT)) == GMP_SUCCESS)
						)
					{
						m_qSubOperation = TA1_PLACE_COF_Q;		//5
					}
				}
			}
		}

		break;

	case TA1_PLACE_COF_Q:	// 5

		if (m_bStopCycle)
		{
			TA1_Status = ST_STOP;
			m_qSubOperation = TA1_STOP_Q;	
		}	
		else if(ACF_WH_Status[m_lCurrCheck] == ST_ACK)
		{
			if(
				((m_nLastError = PlaceGlass(m_lCurrCheck)) == GMP_SUCCESS)
				)
			{
				m_qSubOperation = TA1_CHECK_PLACE_COF_Q;	//6
			}
		}

		break;

	case TA1_CHECK_PLACE_COF_Q:	// 6
		
		if (CheckPlaceStatus() || IsBurnInDiagBond())
		{
			ACF_WH_Status[m_lCurrCheck] = ST_READY;		//ask ACFWH Check COF exist
			m_lPickPlaceRetryCounter = 0;
			TA1_GLASS_Status = ST_EMPTY;
			TA1_Status = ST_EMPTY;

			if (
				((m_nLastError = MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT)) == gnAMS_OK) &&
				((m_nLastError = MoveTPickPosn(GMP_NOWAIT)) == gnAMS_OK) &&
				((m_nLastError = MoveZ(TA1_Z_CTRL_GO_LOWER_LEVEL, GMP_WAIT)) == gnAMS_OK)
			   )
			{
				TA1_POSN = ACFWH_NONE;

				if (m_bStopCycle)
				{
					m_qSubOperation = TA1_STOP_Q;
				}
				else
				{
					m_qSubOperation = TA1_IDLE_Q;	//1
				}
			}
		}
		else
		{
			//retry placing sequence
			if (m_lPickPlaceRetryLimit != 0)
			{
				m_lPickPlaceRetryCounter++;

				if (m_lPickPlaceRetryCounter >= m_lPickPlaceRetryLimit)
				{
					// ERROR!
					TA1_Status = ST_VAC_ERROR;
					m_qSubOperation = TA1_STOP_Q;	
					DisplayMessage("TA1 ERROR: Place Glass Retry Limit Hit");
					m_bModError = TRUE;
					SetError(IDS_TA1_PLACE_ERR);
					//SetError(IDS_MAINBOND_WH_GLASS1_VAC_ERR + PPAction + (m_lCurrTargetPosn * CONVERT_TO_MAPPING));
					m_bIsVacError = TRUE; //20121224 vac error
				}
				else
				{
					m_qSubOperation = TA1_PLACE_COF_Q;	//5
				}
			}
			else
			{
				if (m_bStopCycle)
				{
					m_qSubOperation = TA1_STOP_Q; 
				}	
				else
				{
					m_qSubOperation = TA1_PLACE_COF_Q;    // go to 5
				}	
			}
		}

		break;



	case TA1_STOP_Q:	//99
		break;
	}


	if (m_qPreviousSubOperation != m_qSubOperation)
	{
		if (HMI_bDebugSeq)
		{
			CString szMsg;
			if (TA1_STOP_Q != m_qSubOperation)
			{
				szMsg.Format("TA1 Go State: %ld", m_qSubOperation);
			}
			else
			{
				szMsg.Format("TA1 Go Stop");
			}
			//DisplayMessage(szMsg);
		}

		UpdateDebugSeq(GetSequenceString(), TA1_Status, m_bGlass1Exist);
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
			stPrintData.szTitle[GetStationID() * MAX_STATION_SEQ_DATA + 2].Format("%ld", TA1_Status);

			LogFile->log(stPrintData.szTitle);
		}

	}


}

VOID CTA1::ModulePRSetupPreTask()
{
	//CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	m_bCheckIO = TRUE;
	m_bCheckMotor = TRUE;

	CheckModSelected(TRUE);
	//pCACF1WH->MoveZToStandbyLevel(GLASS1, GMP_NOWAIT);
	//pCACF1WH->MoveXYToStandbyPosn(GMP_NOWAIT);
		
	SelectPRU(HMI_ulCurPRU);
	UpdateSetupStatus();

	SetDiagnSteps(g_lDiagnSteps);
}

VOID CTA1::ModulePRSetupPostTask()
{
	m_bCheckIO			= FALSE;
	m_bCheckMotor		= FALSE;
}

LONG CTA1::HMI_SetPRSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	SetPRSelected(bMode);

	return 0;
}
LONG CTA1::HMI_SelectPRU(IPC_CServiceMessage &svMsg)
{
	ULONG ulPRU;
	svMsg.GetMsg(sizeof(ULONG), &ulPRU);
	
	HMI_ulCurPRU = ulPRU;

	SelectPRU(ulPRU);

	PRS_ClearScreen(ACFUL_CAM);
	PRS_DrawHomeCursor(ACFUL_CAM, FALSE);

	return 0;
}
LONG CTA1::HMI_UpdateSetupStatus(IPC_CServiceMessage &svMsg)
{
	UpdateSetupStatus();
	return 0;
}

VOID CTA1::UpdateSetupStatus()
{
	INT nCurIndex = 0;

	m_stACFUL.bValid = TRUE;

	if (!&pruIN1.bLoaded /*|| !&pruCOF_PR2.bLoaded*/ || !m_stACFUL.bAlignPtSet)
	{
		m_stACFUL.bValid = FALSE;
	}
}

PRU &CTA1::SelectCurPRU(ULONG ulPRU)
{
	switch (ulPRU)
	{
	case 0: // PR1
		return pruIN1;
		break;

	//case 1:	// PR2
	//	return pruCOF_PR2;
	//	break;

	default:
		DisplayMessage("ACFUL SelectCurPRU Error");
		return pruIN1;
	}
}

LONG CTA1::HMI_SetPRPosn(IPC_CServiceMessage &svMsg)
{	
	CString szOldValue;
	szOldValue.Format("X1 = %ld, Y1 = %ld",
		m_mtrBigFOVPRPosn.x, m_mtrBigFOVPRPosn.y);
	//szOldValue.Format("X1 = %ld, Y1 = %ld, X2 = %ld, Y2 = %ld",
		//m_mtrPR1Posn.x, m_mtrPR1Posn.y,
		//m_mtrPR2Posn.x, m_mtrPR2Posn.y);	//20150109

	LONG lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Overwrite Position?");
	if (lAnswer == rMSG_TIMEOUT)
	{
		HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
		return 0;
	}
	else if (lAnswer == rMSG_CONTINUE)
	{
		//LONG lAnswer2 = 0;
		//lAnswer2 = HMISelectionBox("SET PR POSN", "Set Which PR?", "PR", "PR1", "PR2"/*, "PR4"*/);
		//if (lAnswer2 == -1 || lAnswer2 == 11)
		//{
		//	HMIMessageBox(MSG_OK, "SET PR POSN", "Operation Abort!");
		//	return 0;
		//}

		//switch (lAnswer2)
		//{
		//case 0:
			m_mtrBigFOVPRPosn.x = m_stMotorX.GetEncPosn();
			m_mtrBigFOVPRPosn.y = m_stMotorY.GetEncPosn();
			//break;

		//case 1:
		//	m_mtrPR1Posn.x = m_stMotorX.GetEncPosn();
		//	m_mtrPR1Posn.y = m_stMotorY.GetEncPosn();
		//	break;

		//case 2:
		//	m_mtrPR2Posn.x = m_stMotorX.GetEncPosn();
		//	m_mtrPR2Posn.y = m_stMotorY.GetEncPosn();
		//	break;

		//case 3:
		//	m_mtrPR4Posn[m_lCurGlass].x = m_stMotorX.GetEncPosn();
		//	m_mtrPR4Posn[m_lCurGlass].y = m_stMotorY.GetEncPosn();
		//	break;
		//}
	}

	CString szNewValue;
	szNewValue.Format("X1 = %ld, Y1 = %ld",
		m_mtrBigFOVPRPosn.x, m_mtrBigFOVPRPosn.y);	//20150109
	//szNewValue.Format("X1 = %ld, Y1 = %ld, X2 = %ld, Y2 = %ld, X3 = %ld, Y3 = %ld, X4 = %ld, Y4 = %ld",
		//m_mtrPR1Posn[m_lCurGlass].x, m_mtrPR1Posn[m_lCurGlass].y,
		//m_mtrPR2Posn[m_lCurGlass].x, m_mtrPR2Posn[m_lCurGlass].y,
		//m_mtrPR3Posn[m_lCurGlass].x, m_mtrPR3Posn[m_lCurGlass].y);	//20150109
		//m_mtrPR4Posn[m_lCurGlass].x, m_mtrPR4Posn[m_lCurGlass].y);	//20150109

	CString szParameter;
	szParameter.Format("TA1 PR Position");
	LogParameter(__FUNCTION__, szParameter, szOldValue, szNewValue);

	return 0;
}

LONG CTA1::HMI_SetPRLevel(IPC_CServiceMessage &svMsg)
{
	CString szOldValue;
	szOldValue.Format("Z = %ld", m_lPRLevel);

	LONG lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Overwrite Level?");
	if (lAnswer == rMSG_TIMEOUT)
	{
		HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
		return 0;
	}
	else if (lAnswer == rMSG_CONTINUE)
	{
		m_lPRLevel = m_stMotorZ.GetEncPosn();
	}

	CString szNewValue;
	szNewValue.Format("Z = %ld", m_lPRLevel);

	CString szParameter;
	szParameter.Format("TA1 PR Level");
	LogParameter(__FUNCTION__, szParameter, szOldValue, szNewValue);

	return 0;
}

LONG CTA1::HMI_SearchCentre(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	MTR_POSN mtr_Offset = {0,};

	CString szMsg = "";
	MTR_POSN mtrTemp = {0,};
	MTR_POSN mtrTemp0 = {0,};
	MTR_POSN mtrTemp1 = {0,};
	mtrTemp.x = 0;
	mtrTemp.y = 0;
	LONG lStep = 0;

	PR_COORD coCentre;
	coCentre.x = PR_DEF_CENTRE_X;
	coCentre.y = PR_DEF_CENTRE_Y;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "SEARCH PICK POINT", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (!m_bPRSelected)
	{
		szMsg.Format("%s PR Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "SEARCH PICK POINT", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (!pruIN1.bLoaded /*&& m_bBigFOVInspection*/)
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "PR Pattern Not Loaded. Operation Abort!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	//if ((!pruINWH_PR1.bLoaded || !pruINWH_PR2.bLoaded) && !m_bBigFOVInspection)
	//{
	//	HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "PR Pattern Not Loaded. Operation Abort!");
	//	svMsg.InitMessage(sizeof(BOOL), &bResult);
	//	return 1;
	//}

	if (!m_stACFUL.bAlignPtSet)
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Align Pts Not Learned. Operation Abort!");	
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	m_dCurrAlignAngle = 0.0;

	//if (!m_bBigFOVInspection)
	//{
	//	if (!AutoSearchPR1(&pruINWH_PR1, m_emPRMultiLevel))
	//	{
	//		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR1 Failed!");
	//		PRS_DisplayText(INWH_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
	//		svMsg.InitMessage(sizeof(BOOL), &bResult);
	//		return 1;
	//	}

	//	if (!AutoSearchPR2(&pruINWH_PR2, m_emPRMultiLevel))
	//	{
	//		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR2 Failed!");
	//		PRS_DisplayText(INWH_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
	//		svMsg.InitMessage(sizeof(BOOL), &bResult);
	//		return 1;
	//	}
	//}
	//else
	{
		m_dmtrCurrPR1Posn.x = (DOUBLE)m_mtrBigFOVPRPosn.x;
		m_dmtrCurrPR1Posn.y = (DOUBLE)m_mtrBigFOVPRPosn.y;
		if (!AutoSearchPR1(&pruIN1))
		{
			HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR1 Failed!");
			PRS_DisplayText(ACFUL_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return 1;
		}
	}
	PRS_ClearScreen(ACFUL_CAM);

	m_dmtrCurrCenterPRPosn.x = m_dmtrCurrPR1Posn.x;
	m_dmtrCurrCenterPRPosn.y = m_dmtrCurrPR1Posn.y;
	
	CalcAlignPt();

	mtrTemp0.x = DoubleToLong(m_dmtrCurrAlignCentre.x);
	mtrTemp0.y = DoubleToLong(m_dmtrCurrAlignCentre.y);

	MoveAbsoluteXY(DoubleToLong(mtrTemp0.x), DoubleToLong(mtrTemp0.y), GMP_WAIT);

	//PRS_DrawCross(ACFUL_CAM, coCentre, PRS_XLARGE_SIZE, PR_COLOR_RED);
	PRS_DrawHomeCursor(ACFUL_CAM, FALSE);	//20141210
	PRS_DisplayText(ACFUL_CAM, 1, PRS_MSG_ROW1, "Search Succeed!");

	szMsg.Format("Align Angle = %.3f", m_dCurrAlignAngle);
	WriteHMIMess(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CTA1::HMI_SearchAp1(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	CString szMsg = "";

	MTR_POSN mtrTemp0;
	//MTR_POSN mtrTemp1;
	LONG lStep = 0;
	PR_COORD coCentre;
	coCentre.x = PR_DEF_CENTRE_X;
	coCentre.y = PR_DEF_CENTRE_Y;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (!m_bPRSelected)
	{
		szMsg.Format("%s PR Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (!pruIN1.bLoaded /*&& m_bBigFOVInspection*/)
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "PR Pattern Not Loaded. Operation Abort!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	//if ((!pruINWH_PR1.bLoaded || !pruINWH_PR2.bLoaded) && !m_bBigFOVInspection)
	//{
	//	HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "PR Pattern Not Loaded. Operation Abort!");
	//	svMsg.InitMessage(sizeof(BOOL), &bResult);
	//	return 1;
	//}

	if (!m_stACFUL.bAlignPtSet)
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Align Pts Not Learned. Operation Abort!");	
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	m_dCurrAlignAngle = 0.0;

	//if (!m_bBigFOVInspection)
	//{
	//	if (!AutoSearchPR1(&pruINWH_PR1, m_emPRMultiLevel))
	//	{
	//		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR1 Failed!");
	//		PRS_DisplayText(INWH_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
	//		svMsg.InitMessage(sizeof(BOOL), &bResult);
	//		return 1;
	//	}

	//	if (!AutoSearchPR2(&pruINWH_PR2, m_emPRMultiLevel))
	//	{
	//		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR2 Failed!");
	//		PRS_DisplayText(INWH_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
	//		svMsg.InitMessage(sizeof(BOOL), &bResult);
	//		return 1;
	//	}
	//}
	//else
	{
		m_dmtrCurrPR1Posn.x = (DOUBLE)m_mtrBigFOVPRPosn.x;
		m_dmtrCurrPR1Posn.y = (DOUBLE)m_mtrBigFOVPRPosn.y;
		if (!AutoSearchPR1(&pruIN1))
		{
			HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR Failed!");
			PRS_DisplayText(ACFUL_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return 1;
		}
	}

	PRS_ClearScreen(ACFUL_CAM);

	m_dmtrCurrCenterPRPosn.x = m_dmtrCurrPR1Posn.x;
	m_dmtrCurrCenterPRPosn.y = m_dmtrCurrPR1Posn.y;
	
	CalcAlignPt();

	mtrTemp0.x = DoubleToLong(m_dmtrCurrPR1Posn.x + m_dmtrCurrAlign1Offset.x);
	mtrTemp0.y = DoubleToLong(m_dmtrCurrPR1Posn.y + m_dmtrCurrAlign1Offset.y);
	
	MoveAbsoluteXY(mtrTemp0.x, mtrTemp0.y, GMP_WAIT);

	//PRS_DrawCross(ACFUL_CAM, coCentre, PRS_XLARGE_SIZE, PR_COLOR_RED);
	PRS_DrawHomeCursor(ACFUL_CAM, FALSE);	//20141210
	PRS_DisplayText(ACFUL_CAM, 1, PRS_MSG_ROW1, "Search Succeed!");

	szMsg.Format("Align Angle = %.3f", m_dCurrAlignAngle);
	WriteHMIMess(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CTA1::HMI_SearchAp2(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	CString szMsg = "";
	MTR_POSN mtrTemp;
	//MTR_POSN mtrTemp1;
	LONG lStep = 0;
	PR_COORD coCentre;
	coCentre.x = PR_DEF_CENTRE_X;
	coCentre.y = PR_DEF_CENTRE_Y;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (!m_bPRSelected)
	{
		szMsg.Format("%s PR Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (!pruIN1.bLoaded /*&& m_bBigFOVInspection*/)
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "PR Pattern Not Loaded. Operation Abort!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	//if ((!pruINWH_PR1.bLoaded || !pruINWH_PR2.bLoaded) && !m_bBigFOVInspection)
	//{
	//	HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "PR Pattern Not Loaded. Operation Abort!");
	//	svMsg.InitMessage(sizeof(BOOL), &bResult);
	//	return 1;
	//}

	if (!m_stACFUL.bAlignPtSet)
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Align Pts Not Learned. Operation Abort!");	
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	m_dCurrAlignAngle = 0.0;

	//if (!m_bBigFOVInspection)
	//{
	//	if (!AutoSearchPR1(&pruINWH_PR1, m_emPRMultiLevel))
	//	{
	//		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR1 Failed!");
	//		PRS_DisplayText(INWH_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
	//		svMsg.InitMessage(sizeof(BOOL), &bResult);
	//		return 1;
	//	}

	//	if (!AutoSearchPR2(&pruINWH_PR2, m_emPRMultiLevel))
	//	{
	//		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR2 Failed!");
	//		PRS_DisplayText(INWH_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
	//		svMsg.InitMessage(sizeof(BOOL), &bResult);
	//		return 1;
	//	}
	//}
	//else
	{
		m_dmtrCurrPR2Posn.x = (DOUBLE)m_mtrBigFOVPRPosn.x;
		m_dmtrCurrPR2Posn.y = (DOUBLE)m_mtrBigFOVPRPosn.y;
		if (!AutoSearchPR2(&pruIN1))
		{
			HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR1 Failed!");
			PRS_DisplayText(ACFUL_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return 1;
		}
	}

	PRS_ClearScreen(ACFUL_CAM);

	m_dmtrCurrCenterPRPosn.x = m_dmtrCurrPR2Posn.x;
	m_dmtrCurrCenterPRPosn.y = m_dmtrCurrPR2Posn.y;
	
	CalcAlignPt();

	mtrTemp.x = DoubleToLong(m_dmtrCurrPR2Posn.x + m_dmtrCurrAlign2Offset.x);
	mtrTemp.y = DoubleToLong(m_dmtrCurrPR2Posn.y + m_dmtrCurrAlign2Offset.y);

	MoveAbsoluteXY(mtrTemp.x, mtrTemp.y, GMP_WAIT);

	//PRS_DrawCross(ACFUL_CAM, coCentre, PRS_XLARGE_SIZE, PR_COLOR_RED);
	PRS_DrawHomeCursor(ACFUL_CAM, FALSE);	//20141210
	PRS_DisplayText(ACFUL_CAM, 1, PRS_MSG_ROW1, "Search Succeed!");

	szMsg.Format("Align Angle = %.3f", m_dCurrAlignAngle);
	WriteHMIMess(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

VOID CTA1::SelectPRU(ULONG ulPRU, BOOL bIsMove) //20121221
{

	if (HMI_ulCurPRU > 1)
	{
		HMI_ulCurPRU = 0;
	}
	
	m_dCurrAlignAngle = 0.0;

	if (bIsMove) //20121221
	{
		MoveXYToCurPRPosn(ulPRU);
	}
	UpdateHMIPRButton();
}

INT CTA1::MoveXYToCurPRPosn(LONG lPRU)
{
	INT nResult = GMP_FAIL;
	switch (lPRU)
	{
	case 0:
		nResult = MoveXYToBigFOVPRPosn(GMP_WAIT);
	//	nResult = MoveXYToCurrPR1Posn(GMP_WAIT);
		break;

	//case 1:
	//	nResult = MoveXYToCurrPR2Posn(GMP_WAIT);
	//	break;
	}

	if (nResult != GMP_SUCCESS)
	{
		return GMP_FAIL;
	}
	else 
	{
		return (MoveZToPRLevel(GMP_WAIT) || MoveTPlacePosn(GMP_WAIT));
	}

}

VOID CTA1::SetupAlignPt()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	static BOOL	bMovePrevPosn = FALSE;
	BOOL		bResult 	= TRUE;
	CString		szAxis 		= "";
	LONG		lAnswer		= rMSG_TIMEOUT;

	CString		szMsg;
	CString		strMsg;
	MTR_POSN	mtrTemp;

	if (g_ulWizardStep > 3 && g_ulWizardStep != 99)
	{
		g_ulWizardStep = 2;
	}

	g_bWizardEnableBack = FALSE;
	g_bWizardEnableNext = FALSE;
	g_bWizardEnableExit = TRUE;

	SetHmiVariable("SF_bWizardEnable");
	SetHmiVariable("SF_szWizardStep");
	SetHmiVariable("SF_szWizardMess");
	SetHmiVariable("SF_szWizardMessPrev");
	SetHmiVariable("SF_szWizardMessNext");
	SetHmiVariable("SF_szWizardBMPath");
	SetHmiVariable("SF_bWizardEnableBack");
	SetHmiVariable("SF_bWizardEnableNext");
	SetHmiVariable("SF_bWizardEnableExit");


	// Perform Operations
	switch (g_ulWizardStep)
	{
	case 99:
		g_szWizardStep = "";
		g_szWizardMess = "";
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = FALSE;
		g_bWizardEnableExit = TRUE;

		LogAction(__FUNCTION__); // HMI_lCurLSIType == ShuttleLSIType[m_lCurSlave]
		LogAction("AlignPt1.x = %.5f\t AlignPt1.y = %.5f\t AlignPt2.x = %.5f\t AlignPt2.y = %.5f\t", 
				m_stACFUL.dmtrAlignPt1.x, m_stACFUL.dmtrAlignPt1.y,
				m_stACFUL.dmtrAlignPt2.x, m_stACFUL.dmtrAlignPt2.y);
		LogAction("AlignRef.x = %.5f\t AlignRef.y = %.5f\t AlignAng = %.5f", 
				m_dmtrAlignRef.x, m_dmtrAlignRef.y,
				m_dAlignRefAngle);

		break;

	case 0:
	// Search PR
		LogAction(__FUNCTION__);
		LogAction("AlignPt1.x = %.5f\t AlignPt1.y = %.5f\t AlignPt2.x = %.5f\t AlignPt2.y = %.5f\t", 
				m_stACFUL.dmtrAlignPt1.x, m_stACFUL.dmtrAlignPt1.y,
				m_stACFUL.dmtrAlignPt2.x, m_stACFUL.dmtrAlignPt2.y);
		LogAction("AlignRef.x = %.5f\t AlignRef.y = %.5f\t AlignAng = %.5f", 
				m_dmtrAlignRef.x, m_dmtrAlignRef.y,
				m_dAlignRefAngle);

		if (!pruIN1.bLoaded /*|| !pruINWH_PR2.bLoaded*/)
		{
			szMsg.Format("%s PR Pattern Not Loaded. Operation Abort!", GetStnName());
			HMIMessageBox(MSG_OK, "WARNING", szMsg);
			bResult = FALSE;
		}
		else
		{
			//m_lCurGlass = GLASS1;

			//if (IndexGlassToTable((GlassNum)m_lCurGlass) != GMP_SUCCESS)
			//{
			//	bResult = FALSE;
			//}
			//else
			{
				
				if (
					//MoveXY(INWH_XY_CTRL_GO_PR_POSN, GMP_WAIT) != GMP_SUCCESS ||
					//MoveTToStandby(GMP_WAIT) != GMP_SUCCESS ||
					!SetGlassRef(TRUE) 
				)
				{
					bResult = FALSE;
				}
				else
				{
					lAnswer = HMIMessageBox(MSG_YES_NO, "SET ALIGN POINT", "Move to previously set position?");
					if (lAnswer == rMSG_YES)
					{
						bMovePrevPosn = TRUE;
					}
					else
					{
						bMovePrevPosn = FALSE;
					}
					if (bMovePrevPosn)
					{
						//if(!m_bBigFOVInspection)
						//{
						//	mtrTemp.x = (LONG)(m_dmtrCurrPR1Posn.x + m_stACFUL.dmtrAlignPt1.x);
						//	mtrTemp.y = (LONG)(m_dmtrCurrPR1Posn.y + m_stACFUL.dmtrAlignPt1.y);
						//}
						//else
						{
							mtrTemp.x = (LONG)(m_dmtrCurrCenterPRPosn.x + m_stACFUL.dmtrAlignPt1.x);
							mtrTemp.y = (LONG)(m_dmtrCurrCenterPRPosn.y + m_stACFUL.dmtrAlignPt1.y);
						}
					}
					else
					{
						//if(!m_bBigFOVInspection)
						//{
						//	mtrTemp.x = (LONG)m_dmtrCurrPR1Posn.x;
						//	mtrTemp.y = (LONG)m_dmtrCurrPR1Posn.y;
						//}
						//else
						{
							mtrTemp.x = (LONG)m_dmtrCurrCenterPRPosn.x;
							mtrTemp.y = (LONG)m_dmtrCurrCenterPRPosn.y;
						}
					}
					if (
						m_stMotorX.MoveAbsolute(mtrTemp.x, GMP_WAIT)  != GMP_SUCCESS ||
						m_stMotorY.MoveAbsolute(mtrTemp.y,  GMP_WAIT) != GMP_SUCCESS
						)
					{
						bResult = FALSE;
					}
					else
					{
						PRS_DisplayVideo(&pruACFUL_Calib);
						PRS_DrawHomeCursor(ACFUL_CAM, FALSE);

						g_szWizardStep = "Step 1/2";
						g_szWizardMessPrev = "";
						g_szWizardMess = "1. Set Align Pt 1 Position";	
						g_szWizardMessNext = "2. Set Align Pt 2 Position";
						g_szWizardBMPath = "DONE";
					}
				}
			}
			
		}
		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;
		break;

	case 1:
	// 1. Set Align Pick Pt 1 Offset
	// 2. Ready for setting Align Pick Pt 2 Offset
		lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Operation is irreversible. Continue?");
		if (lAnswer == rMSG_TIMEOUT || lAnswer == rMSG_CANCEL)
		{
			bResult = FALSE;
		}
		else
		{
			m_stACFUL.bAlignPtSet = FALSE;

			//if(!m_bBigFOVInspection)
			//{
			//	m_stACFUL.dmtrAlignPt1.x = ((DOUBLE)m_stMotorX.GetEncPosn() - m_dmtrCurrPR1Posn.x);	
			//	m_stACFUL.dmtrAlignPt1.y = ((DOUBLE)m_stMotorY.GetEncPosn() - m_dmtrCurrPR1Posn.y);
			//}
			//else
			{
				m_stACFUL.dmtrAlignPt1.x = ((DOUBLE)m_stMotorX.GetEncPosn() - m_dmtrCurrCenterPRPosn.x);	
				m_stACFUL.dmtrAlignPt1.y = ((DOUBLE)m_stMotorY.GetEncPosn() - m_dmtrCurrCenterPRPosn.y);
			}
			//m_stINWH[GLASS2].dmtrAlignPt1.x = m_stINWH[GLASS1].dmtrAlignPt1.x;
			//m_stINWH[GLASS2].dmtrAlignPt1.y = m_stINWH[GLASS1].dmtrAlignPt1.y;
		
			if (bMovePrevPosn)
			{
				//if(!m_bBigFOVInspection)
				//{
				//	mtrTemp.x = (LONG)(m_dmtrCurrPR2Posn.x + m_stACFUL.dmtrAlignPt2.x);
				//	mtrTemp.y = (LONG)(m_dmtrCurrPR2Posn.y + m_stACFUL.dmtrAlignPt2.y);
				//}
				//else
				{
					mtrTemp.x = (LONG)(m_dmtrCurrCenterPRPosn.x + m_stACFUL.dmtrAlignPt2.x);
					mtrTemp.y = (LONG)(m_dmtrCurrCenterPRPosn.y + m_stACFUL.dmtrAlignPt2.y);
				}
			}
			else
			{
				//if(!m_bBigFOVInspection)
				//{
				//	mtrTemp.x = (LONG)m_dmtrCurrPR2Posn.x;
				//	mtrTemp.y = (LONG)m_dmtrCurrPR2Posn.y;
				//}
				//else
				{
					mtrTemp.x = (LONG)m_dmtrCurrCenterPRPosn.x;
					mtrTemp.y = (LONG)m_dmtrCurrCenterPRPosn.y;
				}
			}
			if (
				m_stMotorX.MoveAbsolute(mtrTemp.x,  GMP_WAIT)  != GMP_SUCCESS ||
				m_stMotorY.MoveAbsolute(mtrTemp.y,  GMP_WAIT) != GMP_SUCCESS)
			{
				bResult = FALSE;
			}
			else
			{
				g_szWizardStep = "Step 2/2";
				g_szWizardMessPrev = "1. Set Align Pt 1 Position";
				g_szWizardMess = "2. Set Align Pt 2 Position";
				g_szWizardMessNext = "";
				g_szWizardBMPath = "";
			}
		}

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;
		break;

	case 2:
	// 2. Set Align Pick Pt 2 position
		//if(!m_bBigFOVInspection)
		//{
		//	m_stACFUL.dmtrAlignPt2.x = ((DOUBLE)m_stMotorX.GetEncPosn() - m_dmtrCurrPR2Posn.x);	
		//	m_stACFUL.dmtrAlignPt2.y = ((DOUBLE)m_stMotorY.GetEncPosn() - m_dmtrCurrPR2Posn.y);
		//}
		//else
		{
			m_stACFUL.dmtrAlignPt2.x = ((DOUBLE)m_stMotorX.GetEncPosn() - m_dmtrCurrCenterPRPosn.x);	
			m_stACFUL.dmtrAlignPt2.y = ((DOUBLE)m_stMotorY.GetEncPosn() - m_dmtrCurrCenterPRPosn.y);
		}
		//m_stINWH[GLASS2].dmtrAlignPt2.x = m_stINWH[GLASS1].dmtrAlignPt2.x;
		//m_stINWH[GLASS2].dmtrAlignPt2.y = m_stINWH[GLASS1].dmtrAlignPt2.y;

		//m_mtrBondOffsetXY.x = DoubleToLong(m_mtrACFCenterPoint.x - (m_dmtrCurrPR1Posn.x + g_stACFDL.dmtrAlignPt1.x + m_dmtrCurrPR2Posn.x + g_stACFDL.dmtrAlignPt2.x) / 2);
		//m_mtrBondOffsetXY.y = DoubleToLong(m_mtrACFCenterPoint.y - (m_dmtrCurrPR1Posn.y + g_stACFDL.dmtrAlignPt1.y + m_dmtrCurrPR2Posn.y + g_stACFDL.dmtrAlignPt2.y) / 2);

		m_stACFUL.bAlignPtSet = TRUE;
		//m_stINWH[GLASS2].bAlignPtSet = TRUE;

		D_MTR_POSN dmtrCurrAlign1Posn, dmtrCurrAlign2Posn;
		//if(!m_bBigFOVInspection)
		//{
		//	dmtrCurrAlign1Posn.x = m_dmtrCurrPR1Posn.x + m_stACFUL.dmtrAlignPt1.x;		// 20160513 Add Align Distance
		//	dmtrCurrAlign1Posn.y = m_dmtrCurrPR1Posn.y + m_stACFUL.dmtrAlignPt1.y;

		//	dmtrCurrAlign2Posn.x = m_dmtrCurrPR2Posn.x + m_stACFUL.dmtrAlignPt2.x;
		//	dmtrCurrAlign2Posn.y = m_dmtrCurrPR2Posn.y + m_stACFUL.dmtrAlignPt2.y;
		//}
		//else
		{
			dmtrCurrAlign1Posn.x = m_dmtrCurrCenterPRPosn.x + m_stACFUL.dmtrAlignPt1.x;		// 20160513 Add Align Distance
			dmtrCurrAlign1Posn.y = m_dmtrCurrCenterPRPosn.y + m_stACFUL.dmtrAlignPt1.y;

			dmtrCurrAlign2Posn.x = m_dmtrCurrCenterPRPosn.x + m_stACFUL.dmtrAlignPt2.x;
			dmtrCurrAlign2Posn.y = m_dmtrCurrCenterPRPosn.y + m_stACFUL.dmtrAlignPt2.y;
		}
		//m_dAlignRefDist =  sqrt(pow((dmtrCurrAlign2Posn.x - dmtrCurrAlign1Posn.x), 2.0) + pow((dmtrCurrAlign2Posn.y - dmtrCurrAlign1Posn.y), 2.0));
		//m_dAlignRefDist[GLASS2] = m_dAlignRefDist[GLASS1];
	
		//if(!m_bBigFOVInspection)
		//{
		//	m_dmtrAlignRef.x = (m_dmtrCurrPR1Posn.x + m_stACFUL.dmtrAlignPt1.x + m_dmtrCurrPR2Posn.x + m_stACFUL.dmtrAlignPt2.x) / 2.0;
		//	m_dmtrAlignRef.y = (m_dmtrCurrPR1Posn.y + m_stACFUL.dmtrAlignPt1.y + m_dmtrCurrPR2Posn.y + m_stACFUL.dmtrAlignPt2.y) / 2.0;
		//}
		//else
		{
			m_dmtrAlignRef.x = (m_dmtrCurrCenterPRPosn.x + m_stACFUL.dmtrAlignPt1.x + m_dmtrCurrCenterPRPosn.x + m_stACFUL.dmtrAlignPt2.x) / 2.0;
			m_dmtrAlignRef.y = (m_dmtrCurrCenterPRPosn.y + m_stACFUL.dmtrAlignPt1.y + m_dmtrCurrCenterPRPosn.y + m_stACFUL.dmtrAlignPt2.y) / 2.0;
		}
		//for (INT i = GLASS1; i < MAX_NUM_OF_GLASS; i++)
		//{
		//	m_stINWH[i].bAlignPtSet			= TRUE;
		//	m_stINWH[i].dmtrAlignPt1.x		= m_stACFUL.dmtrAlignPt1.x;
		//	m_stINWH[i].dmtrAlignPt1.y		= m_stACFUL.dmtrAlignPt1.y;
		//	m_stINWH[i].dmtrAlignPt2.x		= m_stACFUL.dmtrAlignPt2.x;
		//	m_stINWH[i].dmtrAlignPt2.y		= m_stACFUL.dmtrAlignPt2.y;
		//}

		g_szWizardStep = "Step 2/2";
		g_szWizardMessPrev = "2. Set Align Pt 2 Position";
		g_szWizardMess = "DONE";
		g_szWizardMessNext = "DONE";
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = FALSE;
		g_bWizardEnableExit = TRUE;

		//HMI_bXYControl = TRUE;
		break;
	}

	// Failed?
	if (!bResult)
	{
		g_szWizardStep = "FAIL";
		g_szWizardMess = "OPERATION FAIL!";
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = FALSE;
		g_bWizardEnableExit = TRUE;
	}

	SetHmiVariable("SF_bWizardEnable");
	SetHmiVariable("SF_szWizardStep");
	SetHmiVariable("SF_szWizardMess");
	SetHmiVariable("SF_szWizardMessPrev");
	SetHmiVariable("SF_szWizardMessNext");
	SetHmiVariable("SF_szWizardBMPath");
	SetHmiVariable("SF_bWizardEnableBack");
	SetHmiVariable("SF_bWizardEnableNext");
	SetHmiVariable("SF_bWizardEnableExit");
}

INT CTA1::SetGlassRef(BOOL bSetAng)
{
	//DOUBLE dx, dy;

	//if(!m_bBigFOVInspection)
	//{
	//	//MTR_POSN mtrTemp;

	//	//CalPosnAfterRotate(m_mtrPR1Posn[lGlass], m_dCurrAlignAngle, &mtrTemp);
	//	m_dmtrCurrPR1Posn.x = (DOUBLE)m_mtrPR1Posn[lGlass].x;
	//	m_dmtrCurrPR1Posn.y = (DOUBLE)m_mtrPR1Posn[lGlass].y;

	//	if (
	//		(MoveAbsoluteXY((LONG)m_dmtrCurrPR1Posn.x, (LONG)m_dmtrCurrPR1Posn.y, GMP_WAIT) != GMP_SUCCESS) ||
	//		//(MoveZ1(INWH_Z_CTRL_GO_PR_LEVEL, GMP_WAIT) != GMP_SUCCESS)
	//		(MoveZ((WHZNum)lGlass, INWH_Z_CTRL_GO_PR_LEVEL, GMP_WAIT) != GMP_SUCCESS) //20130609
	//	)
	//	{
	//		return FALSE;
	//	}

	//	if (!SearchPRCentre(&pruINWH_PR1))
	//	{
	//		PRS_DisplayText(pruINWH_PR1.nCamera, 1, PRS_MSG_ROW1, "PR Failed!");
	//		return FALSE;
	//	}

	//	PRS_DrawCross(pruINWH_PR1.nCamera, pruINWH_PR1.rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

	//	m_dmtrCurrPR1Posn.x = (DOUBLE)m_stMotorX.GetEncPosn();
	//	m_dmtrCurrPR1Posn.y = (DOUBLE)m_stMotorY.GetEncPosn();

	//	//CalPosnAfterRotate(m_mtrPR2Posn[lGlass], m_dCurrAlignAngle, &mtrTemp);
	//	m_dmtrCurrPR2Posn.x = (DOUBLE)m_mtrPR2Posn[lGlass].x;
	//	m_dmtrCurrPR2Posn.y = (DOUBLE)m_mtrPR2Posn[lGlass].y;

	//	if (MoveAbsoluteXY((LONG)m_dmtrCurrPR2Posn.x, (LONG)m_dmtrCurrPR2Posn.y, GMP_WAIT) != GMP_SUCCESS)
	//	{
	//		return FALSE;
	//	}

	//	if (!SearchPRCentre(&pruINWH_PR2))
	//	{
	//		PRS_DisplayText(pruINWH_PR2.nCamera, 1, PRS_MSG_ROW1, "PR Failed!");
	//		return FALSE;
	//	}
	//	
	//	PRS_DrawCross(pruINWH_PR2.nCamera, pruINWH_PR2.rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);
	//	
	//	m_dmtrCurrPR2Posn.x = (DOUBLE)m_stMotorX.GetEncPosn();
	//	m_dmtrCurrPR2Posn.y = (DOUBLE)m_stMotorY.GetEncPosn();

	//	if (bSetAng)
	//	{
	//		dx = 1.0 * (m_dmtrCurrPR2Posn.x - m_dmtrCurrPR1Posn.x);
	//		dy = 1.0 * (m_dmtrCurrPR2Posn.y - m_dmtrCurrPR1Posn.y);

	//		if (dx != 0)
	//		{
	//			m_dGlassRefAng[lGlass] = (180.0 / PI) * atan((dy / dx));
	//		}
	//		else
	//		{
	//			m_dGlassRefAng[lGlass] = 0.0;
	//		}

	//		CString szMsg = _T("");
	//		szMsg.Format("Glass Reference Angle Found: %.3f", m_dGlassRefAng[lGlass]);
	//		WriteHMIMess(szMsg);
	//		// Debug
	//		szMsg.Format("PR1 Posn: (%ld, %ld), PR2 Posn: (%ld, %ld)", m_mtrPR1Posn[lGlass].x, m_mtrPR1Posn[lGlass].y, m_mtrPR2Posn[lGlass].x, m_mtrPR2Posn[lGlass].y);
	//		DisplayMessage(szMsg);
	//	}
	//}
	//else
	{

		m_dmtrCurrCenterPRPosn.x = (DOUBLE)m_mtrBigFOVPRPosn.x;
		m_dmtrCurrCenterPRPosn.y = (DOUBLE)m_mtrBigFOVPRPosn.y;

		if (
			(MoveAbsoluteXY((LONG)m_dmtrCurrCenterPRPosn.x, (LONG)m_dmtrCurrCenterPRPosn.y, GMP_WAIT) != GMP_SUCCESS) ||
			//(MoveZ1(INWH_Z_CTRL_GO_PR_LEVEL, GMP_WAIT) != GMP_SUCCESS)
			(MoveZ(TA1_Z_CTRL_GO_PR_LEVEL, GMP_WAIT) != GMP_SUCCESS) //20130609
		)
		{
			return FALSE;
		}

		if (!SearchPRCentre(&pruIN1))
		{
			PRS_DisplayText(pruIN1.nCamera, 1, PRS_MSG_ROW1, "PR Failed!");
			return FALSE;
		}

		PRS_DrawCross(pruIN1.nCamera, pruIN1.rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

		m_dmtrCurrCenterPRPosn.x = (DOUBLE)m_stMotorX.GetEncPosn();
		m_dmtrCurrCenterPRPosn.y = (DOUBLE)m_stMotorY.GetEncPosn();

		if (bSetAng)
		{
			m_dGlassRefAng = (DOUBLE) -1.0 * pruIN1.fAng;
		}
	}
	return TRUE;
}

VOID CTA1::CalcAlignPt()
{
	D_MTR_POSN dmtrCurrAlign1Posn;
	D_MTR_POSN dmtrCurrAlign2Posn;
	
	//DOUBLE dx, dy;
	DOUBLE dAngle, dAngleRad, dRefAngleRad;
	
	CString str;

	// if PR not selected, return ref posn
	if (!m_bPRSelected || !m_stACFUL.bAlignPtSet)
	{
		m_dCurrAlignAngle = 0.0;
		//if(!m_bBigFOVInspection)
		//{
		//	m_dmtrCurrPR1Posn.x = (DOUBLE)m_mtrPR1Posn[lGlass].x;
		//	m_dmtrCurrPR1Posn.y = (DOUBLE)m_mtrPR1Posn[lGlass].y;
		//	m_dmtrCurrPR2Posn.x = (DOUBLE)m_mtrPR2Posn[lGlass].x;
		//	m_dmtrCurrPR2Posn.y = (DOUBLE)m_mtrPR2Posn[lGlass].y;
		//}
		//else
		{
			m_dmtrCurrCenterPRPosn.x = (DOUBLE)m_mtrBigFOVPRPosn.x;
			m_dmtrCurrCenterPRPosn.y = (DOUBLE)m_mtrBigFOVPRPosn.y;
		}
	}

	//if(!m_bBigFOVInspection)
	//{
	//	dx = 1.0 * (m_dmtrCurrPR2Posn.x - m_dmtrCurrPR1Posn.x);
	//	dy = 1.0 * (m_dmtrCurrPR2Posn.y - m_dmtrCurrPR1Posn.y);
	//
	//	if (dx != 0)
	//	{
	//		dAngle = (180.0 / PI) * atan((dy / dx));
	//	}
	//	else
	//	{
	//		dAngle = 0;
	//	}
	//}
	//else
	{
		dAngle = (DOUBLE) -1.0 * pruIN1.fAng;
	}

	dAngleRad = -1.0 * (PI / 180.0) * dAngle;
	dRefAngleRad = -1.0 * (PI / 180.0) * m_dGlassRefAng;

	// current align offset
	m_dmtrCurrAlign1Offset.x = (+m_stACFUL.dmtrAlignPt1.x * cos(dRefAngleRad - dAngleRad) 
								+ m_stACFUL.dmtrAlignPt1.y * sin(dRefAngleRad - dAngleRad) 
							   );

	m_dmtrCurrAlign1Offset.y = (-m_stACFUL.dmtrAlignPt1.x * sin(dRefAngleRad - dAngleRad) 
								+ m_stACFUL.dmtrAlignPt1.y * cos(dRefAngleRad - dAngleRad) 
							   );

	m_dmtrCurrAlign2Offset.x = (+m_stACFUL.dmtrAlignPt2.x * cos(dRefAngleRad - dAngleRad) 
								+ m_stACFUL.dmtrAlignPt2.y * sin(dRefAngleRad - dAngleRad) 
							   );

	m_dmtrCurrAlign2Offset.y = (-m_stACFUL.dmtrAlignPt2.x * sin(dRefAngleRad - dAngleRad) 
								+ m_stACFUL.dmtrAlignPt2.y * cos(dRefAngleRad - dAngleRad) 
							   );

	// current align posn
	//if(!m_bBigFOVInspection)
	//{
	//	dmtrCurrAlign1Posn.x = m_dmtrCurrPR1Posn.x + m_dmtrCurrAlign1Offset.x;
	//	dmtrCurrAlign1Posn.y = m_dmtrCurrPR1Posn.y + m_dmtrCurrAlign1Offset.y;
	//	dmtrCurrAlign2Posn.x = m_dmtrCurrPR2Posn.x + m_dmtrCurrAlign2Offset.x;
	//	dmtrCurrAlign2Posn.y = m_dmtrCurrPR2Posn.y + m_dmtrCurrAlign2Offset.y;
	//}
	//else
	{
		dmtrCurrAlign1Posn.x = m_dmtrCurrCenterPRPosn.x + m_dmtrCurrAlign1Offset.x;
		dmtrCurrAlign1Posn.y = m_dmtrCurrCenterPRPosn.y + m_dmtrCurrAlign1Offset.y;
		dmtrCurrAlign2Posn.x = m_dmtrCurrCenterPRPosn.x + m_dmtrCurrAlign2Offset.x;
		dmtrCurrAlign2Posn.y = m_dmtrCurrCenterPRPosn.y + m_dmtrCurrAlign2Offset.y;
	}
	// return align angle
#if 1 //20160826
	//if(!m_bBigFOVInspection)
	//{
	//	dx = 1.0 * (dmtrCurrAlign2Posn.x - dmtrCurrAlign1Posn.x);
	//	dy = 1.0 * (dmtrCurrAlign2Posn.y - dmtrCurrAlign1Posn.y);
	//	if (dx != 0)
	//	{
	//		m_dCurrAlignAngle = (180.0 / PI) * atan((dy / dx));
	//	}
	//	else
	//	{
	//		m_dCurrAlignAngle = 0.0;
	//	}
	//}
	//else
	{
		m_dCurrAlignAngle = (dAngle - m_dGlassRefAng); //20160413
	}
#else
	m_dCurrAlignAngle = dAngle;// - m_dGlassRefAng;
#endif

	// return align centre
	m_dmtrCurrAlignCentre.x = (dmtrCurrAlign1Posn.x + dmtrCurrAlign2Posn.x) / 2.0;
	m_dmtrCurrAlignCentre.y = (dmtrCurrAlign1Posn.y + dmtrCurrAlign2Posn.y) / 2.0;

	//if (HMI_bDebugCal)
	{
		// open debugging file
		CString str;
		MTR_POSN	mtrTemp;
		
		mtrTemp.x = m_mtrAlignCentre.x;
		mtrTemp.y = m_mtrAlignCentre.y;
		
		mtrTemp.x -= DoubleToLong(m_dmtrAlignRef.x);
		mtrTemp.y -= DoubleToLong(m_dmtrAlignRef.y);

		CString szTime = _T(""), szFileName = _T("");
		clock_t clkNow;

		clkNow = clock();
		SYSTEMTIME ltime;		// New Command

		// Get time as 64-bit integer.
		GetLocalTime(&ltime);
		szTime.Format("%u-%u-%u %u:%u:%u", 
			ltime.wYear, ltime.wMonth, ltime.wDay, 
			ltime.wHour, ltime.wMinute, ltime.wSecond);

		str.Format("D:\\sw\\AC9000\\Data\\Calculation Log\\ACFUL_PRPt_%04u%02u%02u.log", ltime.wYear, ltime.wMonth, ltime.wDay);
		FILE *fp = fopen(str, "a+");

		//if(!m_bBigFOVInspection)
		//{
		//	if (fp != NULL)
		//	{
		//		fseek(fp, 0, SEEK_END);
		//		if (ftell(fp) < 10)
		//		{
		//			fprintf(fp, "Time\tPR1x(Pix)\tPR1y(Pix)\tPR2x(Pix)\tPR2y(Pix)\tPR1x(Motor)\tPR1y(Motor)\tPR2x(Motor)\tPR2y(Motor)\tAlign1x\tAlign1y\tAlign2x\tAlign2y\tAlignAngle\n");
		//		}


		//		//Time PR1 PR2 Enc1 Enc2 APt1 APt2 Dist Ang
		//		fprintf(fp, "%04u-%02u-%02u %02u:%02u:%02u\t%.2f\t%.2f\t%.2f\t%.2f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\n",
		//				ltime.wYear, ltime.wMonth, ltime.wDay,
		//				ltime.wHour, ltime.wMinute, ltime.wSecond,
		//				(DOUBLE)pruINWH_PR1.rcoDieCentre.x, (DOUBLE)pruINWH_PR1.rcoDieCentre.y, 
		//				(DOUBLE)pruINWH_PR2.rcoDieCentre.x, (DOUBLE)pruINWH_PR2.rcoDieCentre.y, 
		//				m_dmtrCurrPR1Posn.x, m_dmtrCurrPR1Posn.y, 
		//				m_dmtrCurrPR2Posn.x, m_dmtrCurrPR2Posn.y, 
		//				dmtrCurrAlign1Posn.x, dmtrCurrAlign1Posn.y, 
		//				dmtrCurrAlign2Posn.x, dmtrCurrAlign2Posn.y,
		//				m_dCurrAlignAngle);

		//		fclose(fp);
		//	}
		//}
		//else
		{
			if (fp != NULL)
			{
				fseek(fp, 0, SEEK_END);
				if (ftell(fp) < 10)
				{
					fprintf(fp, "Time\tPR1x(Pix)\tPR1y(Pix)\tPR1x(Motor)\tPR1y(Motor)\tAlign1x\tAlign1y\tAlign2x\tAlign2y\tAlignAngle\n");
				}


				//Time PR1 PR2 Enc1 Enc2 APt1 APt2 Dist Ang
				fprintf(fp, "%04u-%02u-%02u %02u:%02u:%02u\t%.2f\t%.2f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\n",
						ltime.wYear, ltime.wMonth, ltime.wDay,
						ltime.wHour, ltime.wMinute, ltime.wSecond,
						(DOUBLE)pruIN1.rcoDieCentre.x, (DOUBLE)pruIN1.rcoDieCentre.y, 
						m_dmtrCurrCenterPRPosn.x, m_dmtrCurrCenterPRPosn.y, 
						dmtrCurrAlign1Posn.x, dmtrCurrAlign1Posn.y, 
						dmtrCurrAlign2Posn.x, dmtrCurrAlign2Posn.y,
						m_dCurrAlignAngle);

				fclose(fp);
			}
		}
	}
}

VOID CTA1::SetupWizard()
{
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");
	CWinEagle *pCWinEagle = (CWinEagle*)m_pModule->GetStation("WinEagle");
	CSettingFile *pCSettingFile = (CSettingFile*)m_pModule->GetStation("SettingFile");
	
	PRU *pPRU;

	switch (g_ulWizardSubTitle)
	{
	case 0:
		g_szWizardTitle.Format("TA1 UL Align Pt Setup Procedure");
		LogAction("TA1: " + g_szWizardTitle);
		SetHmiVariable("SF_szWizardTitle");
		//pCSettingFile->HMI_bShowIgnoreWindowBtn = FALSE; //20150506
		//SetHmiVariable("SF_bShowIgnoreWindowBtn");
		SetupAlignPt();
		break;

	case 1:
		g_szWizardTitle.Format("TA1 UL PR%d Pattern Setup Procedure", HMI_ulCurPRU + 1);	//p20121022
		LogAction("AcfTA: " + g_szWizardTitle);
		SetHmiVariable("SF_szWizardTitle");
#ifdef PRS_SET_IGNORE_WINDOW_FIRST //20150324
		if (g_ulWizardStep == 0 && 
			(pCWinEagle->HMI_ulPRAlg == PATTERN_MATCHING ||
			 pCWinEagle->HMI_ulPRAlg == EDGE_MATCHING ||
			 pCWinEagle->HMI_ulPRAlg == FIDUCIAL_FITTING ||
			 pCWinEagle->HMI_ulPRAlg == RECTANGLE_MATCHING)
		   ) //20130319 PR ignore windows
		{
			pCSettingFile->HMI_bShowIgnoreWindowBtn = TRUE;
			SetHmiVariable("SF_bShowIgnoreWindowBtn");
		}
		else //20150717
		{
			pCSettingFile->HMI_bShowIgnoreWindowBtn = FALSE;
			SetHmiVariable("SF_bShowIgnoreWindowBtn");
		}
#endif
		pPRU = &SelectCurPRU(HMI_ulCurPRU);
		SetupPRPatternPreTask(pPRU);
		pCWinEagle->SetupPRPattern(pPRU);
		SetupPRPatternPostTask();
		break;

	}
}

VOID CTA1::SetupPRPatternPreTask(PRU *pPRU)
{
	BOOL bResult = TRUE;
	LONG lAnswer = rMSG_TIMEOUT;

	switch (g_ulWizardStep)
	{
	case 0:
		if (
			//IndexGlassToTA(GLASS1) != GMP_SUCCESS ||
			MoveXYToCurPRPosn(HMI_ulCurPRU) != GMP_SUCCESS
		   )
		{
			bResult = FALSE;
		}
		break;

	case 1:
		if (pPRU->bLoaded)
		{
			lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Operation is irreversible. Continue?");
			if (lAnswer == rMSG_TIMEOUT || lAnswer != rMSG_CONTINUE)
			{
				bResult = FALSE;
				break;
			}
		}

		m_stACFUL.bAlignPtSet = FALSE;


		break;
	}
	if (!bResult)
	{
		g_ulWizardStep = 98; 
		g_szWizardStep = "FAIL";
		g_szWizardMess = "OPERATION FAIL!";
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = FALSE;
		g_bWizardEnableExit = TRUE;

	}
}

VOID CTA1::SetupPRPatternPostTask()
{
	switch (g_ulWizardStep)
	{
	case 99:
		MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
		break;
	}
}

VOID CTA1::UpdatePlaceWHPosnOffset()
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	MTR_POSN mtrTemp0;
	MTR_POSN mtrTemp1;
	//LONG lOffsetT;
	CString szMsg = "";
	m_dCurrAlignAngle = 0.0;
	DOUBLE dAngle = 0.0;

	CalcAlignPt();

	m_lXPlaceWhPROffset = DoubleToLong(m_dmtrCurrAlignCentre.x);
	m_lYPlaceWhPROffset = DoubleToLong(m_dmtrCurrAlignCentre.y);

	m_lXPlaceWhPROffset -= DoubleToLong(m_dmtrAlignRef.x);
	m_lYPlaceWhPROffset -= DoubleToLong(m_dmtrAlignRef.y);
	//ACF1WH
	mtrTemp0.x = (pCACF1WH->m_mtrACFAnvilLLPosn.x + pCACF1WH->m_mtrACFAnvilLRPosn.x)/2 - DistanceToCount((DOUBLE)pCACF1WH->m_lLoadXOffset, pCACF1WH->m_stMotorX.stAttrib.dDistPerCount);
	mtrTemp0.y = (pCACF1WH->m_mtrACFAnvilLLPosn.y + pCACF1WH->m_mtrACFAnvilLRPosn.y)/2 + DistanceToCount((DOUBLE)pCACF1WH->m_lLoadYOffset, pCACF1WH->m_stMotorY.stAttrib.dDistPerCount);

	dAngle = 1 * (pCACF1WH->m_dAnvilBlockAngOffset + m_dCurrAlignAngle);

	pCACF1WH->CalPosnAfterRotate(EDGE_A, mtrTemp0, -dAngle, &mtrTemp1);

	m_lPlaceGlass1Offset = mtrTemp1.x - pCACF1WH->m_mtrAlignRefPosnOnWH.x;//mtrTemp0.x;
	pCACF1WH->m_mtrLoadGlassOffset.y = mtrTemp1.y - pCACF1WH->m_mtrAlignRefPosnOnWH.y;//mtrTemp0.y;
	//ACF2WH
	mtrTemp0.x = (pCACF2WH->m_mtrACFAnvilLLPosn.x + pCACF2WH->m_mtrACFAnvilLRPosn.x)/2 - DistanceToCount((DOUBLE)pCACF2WH->m_lLoadXOffset, pCACF2WH->m_stMotorX.stAttrib.dDistPerCount);
	mtrTemp0.y = (pCACF2WH->m_mtrACFAnvilLLPosn.y + pCACF2WH->m_mtrACFAnvilLRPosn.y)/2 + DistanceToCount((DOUBLE)pCACF2WH->m_lLoadYOffset, pCACF2WH->m_stMotorY.stAttrib.dDistPerCount);

	dAngle = 1 * (pCACF2WH->m_dAnvilBlockAngOffset + m_dCurrAlignAngle);

	pCACF2WH->CalPosnAfterRotate(EDGE_A, mtrTemp0, -dAngle, &mtrTemp1);

	m_lPlaceGlass2Offset = mtrTemp1.x - pCACF2WH->m_mtrAlignRefPosnOnWH.x;//mtrTemp0.x;
	pCACF2WH->m_mtrLoadGlassOffset.y = mtrTemp1.y - pCACF2WH->m_mtrAlignRefPosnOnWH.y;//mtrTemp0.y;
	
	// Any Addition Anlge to add here
	m_lTPlaceWhPROffset = AngleToCount(-1 * m_dCurrAlignAngle, pCACF1WH->GetMotorT().stAttrib.dDistPerCount);			
	//lOffsetT[m_lCurGlass] += AngleToCount(m_dUnloadTOffset[m_lCurGlass], m_stMotorT.stAttrib.dDistPerCount);
	//lOffsetT += GetStandbyTPosn();
	
	// Final Result as here
	m_lXPlaceWhPROffset = m_lXPlaceWhPROffset;
	m_lYPlaceWhPROffset = -m_lYPlaceWhPROffset;
}

VOID CTA1::PlaceOffsetEqualZero()
{
	m_lXPlaceWhPROffset = 0;
	m_lYPlaceWhPROffset = 0;
	m_lTPlaceWhPROffset = 0;
}

BOOL CTA1::IsBigFOVPROK()
{
	BOOL bResult = TRUE;
	MTR_POSN mtrOffset = {0,};

	if (!m_bPRSelected)
	{
		mtrOffset.x = 0;
		mtrOffset.y = 0;
		m_lXPlaceWhPROffset = mtrOffset.x;
		m_lYPlaceWhPROffset = mtrOffset.y;
	}

	m_dmtrCurrPR1Posn.x = (DOUBLE)m_mtrTA1PRPosn.x;
	m_dmtrCurrPR1Posn.y = (DOUBLE)m_mtrTA1PRPosn.y;
	if (
		(m_bPRSelected && (SleepWithReturn(m_lPRDelay) == GMP_SUCCESS) && AutoSearchPR1GrabOnly(&pruIN1) && AutoSearchPR1ProcessOnly(&pruIN1)) ||
		(!m_bPRSelected && SleepWithReturn(m_lPRDelay * 2) == GMP_SUCCESS)
		)
	{
		m_dmtrCurrCenterPRPosn = m_dmtrCurrPR1Posn;
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

INT CTA1::BlowCOF()
{
	INT nResult;

	if(
		((nResult = SetGLASS1_VacSol(OFF, GMP_WAIT)) == GMP_SUCCESS)
		)
	{
		if (m_stWeakBlowSol.GetOnDelay() > 0)
		{
			SetWeakBlowSol(ON, GMP_WAIT);
			SetWeakBlowSol(OFF, GMP_NOWAIT);
		}
	}

	return nResult;
}

BOOL CTA1::CheckPlaceStatus()
{
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH	= (CACF2WH*)m_pModule->GetStation("ACF2WH");

	//if (
	//	ACF_WH_Status[m_lCurrCheck] == ST_BUSY || 
	//	ACF_WH_Status[m_lCurrCheck] == ST_MOD_NOT_SELECTED || 
	//	ACF_WH_Status[m_lCurrCheck] == ST_STOP 
	//   )
	//{
	//	return TRUE;
	//}
	if (
		(m_lCurrCheck == ACFWH_1 && pCACF1WH->IsGLASS1_VacSensorOn()) ||
		(m_lCurrCheck == ACFWH_2 && pCACF2WH->IsGLASS1_VacSensorOn())
	   )
	{
		return TRUE;
	}
	return FALSE;
}


GlassNum CTA1::AssignPlaceToWhere()
{
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH	= (CACF2WH*)m_pModule->GetStation("ACF2WH");

	//if (AutoMode == BURN_IN || AutoMode == DIAG_BOND)
	//{
	//	////switch (g_lGlassInputMode)
	//	////{
	//	////case GLASS_1_INPUT:

	//		if (!pCACF1WH->m_bGlass1Exist)
	//		{
	//			return GLASS1;
	//		}
	//		else
	//		{
	//			return NO_GLASS;
	//		}

	//		break;

	//	case GLASS_2_INPUT:

	//		if (!pCACF2WH->m_bGlass1Exist)
	//		{
	//			return GLASS2;
	//		}
	//		else
	//		{
	//			return NO_GLASS;
	//		}

	//		break;

	//	default:
	//		if (!pCACF2WH->m_bGlass1Exist)
	//		{
	//			return GLASS2;
	//		}
	//		else if (!pCACF1WH->m_bGlass1Exist)
	//		{
	//			return GLASS1;
	//		}
	//		else
	//		{
	//			return NO_GLASS;
	//		}
	//	}
	//	
	//}
	//else
	//{
	//	switch (g_lGlassInputMode)
	//	{
	//	case GLASS_1_INPUT:

	//		if (!pCACF1WH->IsGLASS1_VacSensorOn())
	//		{
	//			return GLASS1;
	//		}
	//		else
	//		{
	//			return NO_GLASS;
	//		}

	//		break;

	//	case GLASS_2_INPUT:

	//		if (!pCACF2WH->IsGLASS1_VacSensorOn())
	//		{
	//			return GLASS2;
	//		}
	//		else
	//		{
	//			return NO_GLASS;
	//		}

	//		break;

	//	default:
	//		if (!pCACF2WH->IsGLASS1_VacSensorOn())
	//		{
	//			return GLASS2;
	//		}
	//		else if (!pCACF1WH->IsGLASS1_VacSensorOn())
	//		{
	//			return GLASS1;
	//		}
	//		else
	//		{
	//			return NO_GLASS;
	//		}
	//		break;

	//	}
	//}

	return NO_GLASS;
}

//INT CTA1::PickGlass_DOWN()
//{
//	if (
//		(MoveZ(TA1_Z_CTRL_GO_LOWER_LEVEL, GMP_WAIT) != GMP_SUCCESS) ||
//		(SetGLASS1_VacSol(ON, GMP_WAIT) != GMP_SUCCESS)
//	   )
//	{
//		return GMP_FAIL;
//	}
//	return GMP_SUCCESS;
//}

//INT CTA1::PickGlass_UP()
//{
//	INT nResult = GMP_SUCCESS;
//	
//	if (m_bIsOfflineMode)
//	{
//		m_bGlass1Exist = TRUE;
//	}
//
//	if (MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS)
//	{
//		return GMP_FAIL;
//	}
//	
//
//	return GMP_SUCCESS;
//
//}

INT CTA1::PlaceGlass(ACFWHUnitNum lACFWHnum) // ONLY ONE PLACE EACH TIME
{
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH	= (CACF2WH*)m_pModule->GetStation("ACF2WH");

	CACFWH *pCACFxWH = NULL;
	LONG lDelay;

	if (lACFWHnum <= ACFWH_NONE || lACFWHnum >= MAX_NUM_OF_ACFWH)
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage("CTA1::PlaceGlass : SW Error");
		return GMP_FAIL;
	}

	if (lACFWHnum == ACFWH_1)
	{
		pCACFxWH = pCACF1WH;
	}
	else if (lACFWHnum == ACFWH_2)
	{
		pCACFxWH = pCACF2WH;
	}
	else
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage("CTA1::PlaceGlass : SW Error");
		return GMP_FAIL;
	}


#if 1 //debug only 20121030
	if (HMI_bDebugSeq)	// 20140801 Yip: Only Show Message When Debug Sequence
	{
		if (lACFWHnum == ACFWH_1)
		{
			DisplayMessage("CTA1::PlaceGlass 1");
		}
		else if (lACFWHnum == ACFWH_2)
		{
			DisplayMessage("CTA1::PlaceGlass 2");
		}
	}
#endif
	pCACFxWH->SetGLASS1_VacSol(ON, GMP_WAIT);
	

	if (
		((m_nLastError = MoveZ(TA1_Z_CTRL_GO_PLACE_LEVEL, GMP_WAIT)) == GMP_SUCCESS)
		)
	{
		lDelay = pCACFxWH->m_stGlass1VacSol.GetOnDelay();
		if (m_stGlass1VacSol.GetOffDelay() > lDelay)
		{
			lDelay = m_stGlass1VacSol.GetOffDelay();
		}
		SetGLASS1_VacSol(OFF, GMP_NOWAIT);
		Sleep(lDelay);

		if (m_stWeakBlowSol.GetOnDelay() > 0)
		{
			SetWeakBlowSol(ON, GMP_WAIT);
			SetWeakBlowSol(OFF, GMP_NOWAIT);
		}

		if (
			((m_nLastError = MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) == GMP_SUCCESS)
			)
		{
			return GMP_SUCCESS;
		}
	}

	return GMP_FAIL;
}

//BOOL CTA1::PickGlassOperation()
//{
//	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
//	CInPickArm *pCInPickArm = (CInPickArm*)m_pModule->GetStation("InPickArm");
//
//	CString szMsg;
//	LONG lTimer = 0;
//	//BOOL bInConvWHGlass1Exist = FALSE, bInConvWHGlass2Exist = FALSE;
//	LONG lPickGlass = GLASS1;
//
//	GLASS_FPC_VAC_STATE stVacState1 = GetGlassFPC1_VacSensorOnState();
//	//GLASS_FPC_VAC_STATE stVacState2 = GetGlassFPC2_VacSensorOnState();
//
//	if (!m_bModSelected)
//	{
//		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
//		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", szMsg);
//		return FALSE;
//	}
//
//	if (!pCInPickArm->m_bModSelected)
//	{
//		szMsg.Format("%s Module Not Selected. Operation Abort!", pCInPickArm->GetStnName());
//		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", szMsg);
//		return FALSE;
//	}
//
//	if (!pCInPickArm->IsVacSensorOn(GLASS1))
//	{
//		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "InPickArm has no glass error. Please Pick a glass on InPickArm and retry again.");
//		return FALSE;
//	}
//
//	if (IsVacStateOn(stVacState1))
//	{
//		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Glass exists on TA1 already. Please remove glass and retry again.");
//		return FALSE;
//	}
//
//	//if (
//	//	MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS || 
//	//	pCInPickArm->MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS || 
//	//	pCInPickArm->MoveX(IN_PICKARM_X_CTRL_GO_PLACE_GLASS_OFFSET, GMP_WAIT) != GMP_SUCCESS || 
//	//	pCInPickArm->SetRotarySol(ON, GMP_WAIT) != GMP_SUCCESS || 
//	//	pCInPickArm->MoveZ(IN_PICKARM_Z_CTRL_GO_PLACE_LEVEL, GMP_WAIT) != GMP_SUCCESS || 
//	//	SetRotarySol(OFF, GMP_WAIT) != GMP_SUCCESS || 
//	//	MoveX(TA1_X_CTRL_GO_PICK_GLASS_OFFSET, GMP_WAIT) != GMP_SUCCESS ||
//	//	SetGLASS1_VacSol(ON, GMP_WAIT) != GMP_SUCCESS ||
//	//	MoveZ(TA1_Z_CTRL_GO_PICK_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
//	//	pCInPickArm->SetGLASS1_VacSol(OFF, GMP_WAIT) != GMP_SUCCESS ||
//	//	pCInPickArm->SetWeakBlowSol(ON, GMP_WAIT) != GMP_SUCCESS ||
//	//	pCInPickArm->SetWeakBlowSol(OFF, GMP_NOWAIT) != GMP_SUCCESS ||
//	//	MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS /*||*/
//	//   )
//	//{
//	//	HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "TA1 Pick Glass move error.");
//	//	return FALSE;
//	//}
//
//	if (!IsGLASS1_VacSensorOn())
//	{
//		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "TA1 Pick Glass Error.");
//		return FALSE;
//	}
//
//	return TRUE;
//}

BOOL CTA1::PlaceGlassOperation(ACFWHUnitNum lACFWHnum)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH	= (CACF2WH*)m_pModule->GetStation("ACF2WH");

	CACFWH *pCACFxWH = NULL;
	
	CString szMsg;
	TA1_X_CTRL_SIGNAL lTempCtrl = TA1_X_CTRL_GO_PLACE_ACF1WH_OFFSET;

	GLASS_FPC_VAC_STATE stVacState1 = GetGlassFPC1_VacSensorOnState();	// 20141112
	//GLASS_FPC_VAC_STATE stVacState2 = GetGlassFPC2_VacSensorOnState();	// 20141112

	if (lACFWHnum == ACFWH_1)
	{
		pCACFxWH = pCACF1WH;
	}
	else if (lACFWHnum == ACFWH_2)
	{
		pCACFxWH = pCACF2WH;
	}
	else
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage("CTA1::PlaceGlass : SW Error");
		return FALSE;
	}

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", szMsg);
		return FALSE;
	}

	if (!pCACFxWH->m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", pCACFxWH->GetStnName());
		HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", szMsg);
		return FALSE;
	}


	if (!IsVacStateOn(stVacState1))
	{
		HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", "FPC not exist for placing. Please pick FPC and retry again.");
		return FALSE;
	}
		
	switch (lACFWHnum)
	{
	case ACFWH_1:
		lTempCtrl = TA1_X_CTRL_GO_PLACE_ACF1WH_PR_OFFSET;
		break;

	case ACFWH_2:
		lTempCtrl = TA1_X_CTRL_GO_PLACE_ACF2WH_PR_OFFSET;
		break;

	default:
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage("CTA1::PlaceGlassOperation : SW Error");
		return FALSE;
	}

	pCACFxWH->SetVacSol(GLASS1, ON, GMP_WAIT);
	if (pCACFxWH->IsVacSensorOn(GLASS1))
	{
		szMsg.Format("FPC exists on %s already. Please remove FPC and retry again.", pCACFxWH->GetStnName());
		HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", szMsg);
		return FALSE;
	}
	pCACFxWH->SetVacSol(GLASS1, OFF, GMP_WAIT);

	if(g_bUplookPREnable)
	{
		if(
			(MoveXYToBigFOVPRPosn(GMP_WAIT) == GMP_SUCCESS) &&
			(MoveTPlacePosn(GMP_WAIT) == GMP_SUCCESS) &&
			(MoveZ(TA1_Z_CTRL_GO_PR_LEVEL, GMP_WAIT) == GMP_SUCCESS)
			)
		{
			if(IsBigFOVPROK())	//PR OK?
			{
				MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT);
				UpdatePlaceWHPosnOffset();
			}
			else	//UL PR Error
			{
				szMsg.Format("PR Error.", GetStnName());
				HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", szMsg);
				return FALSE;
			}
		}
	}
	else
	{
		PlaceOffsetEqualZero();
	}

	if (
		MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		MoveTPlacePosn(GMP_WAIT) != GMP_SUCCESS ||
		//pCACF1WH->MoveTToStandby(GMP_WAIT) != GMP_SUCCESS ||
		//pCACF2WH->MoveTToStandby(GMP_WAIT) != GMP_SUCCESS ||
		pCACFxWH->MoveXYToLoadPosn(GMP_NOWAIT) != GMP_SUCCESS ||
		pCACFxWH->MoveT(ACF_WH_T_CTRL_GO_LOAD_POSN_OFFSET, GMP_WAIT) != GMP_SUCCESS ||
		//MoveX(lTempCtrl, GMP_WAIT) != GMP_SUCCESS ||
		pCACFxWH->MoveRelativeY(-m_lYPlaceWhPROffset, GMP_WAIT) != GMP_SUCCESS ||
		pCACFxWH->MoveRelativeT(-m_lTPlaceWhPROffset, GMP_WAIT) != GMP_SUCCESS ||
		MoveX(lTempCtrl, GMP_WAIT) != GMP_SUCCESS ||
		PlaceGlass(lACFWHnum) != GMP_SUCCESS ||
		MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS ||
		MoveTPickPosn(GMP_NOWAIT) != GMP_SUCCESS ||
		SetVacSol(GLASS1, ON, GMP_NOWAIT) != GMP_SUCCESS 
	   )
	{
		return FALSE;
	}
	else 
	{
		// Check Place Success
		if (!pCACFxWH->IsVacSensorOn(GLASS1))
		{
			//SetGLASS1_VacSol(ON, GMP_WAIT);
			szMsg.Format("Place Glass Error: %s Glass Vac Sensor Not On.", pCACFxWH->GetStnName());
			HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", szMsg);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CTA1::IsRotaryUpperSnrOn()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bResult = FALSE;

	if (!pAppMod->IsInitNuDrive())
	{
		return bResult;
	}

	try
	{
		bResult = m_stT1Snr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CTA1::IsRotaryLowerSnrOn()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bResult = FALSE;

	if (!pAppMod->IsInitNuDrive())
	{
		return bResult;
	}

	try
	{
		bResult = m_stT2Snr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CTA1::IsFlipZHomeSnrOn()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bResult = FALSE;

	if (!pAppMod->IsInitNuDrive())
	{
		return bResult;
	}

	try
	{
		bResult = m_stZHomeSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////
//Motor Related
/////////////////////////////////////////////////////////////////
INT CTA1::HomeZ()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	INT		nResult		= GMP_SUCCESS;

	if (m_stMotorZ.Home(GMP_WAIT) == GMP_SUCCESS)
	{
		//m_bModSelected = TRUE;
		//if (IsHomeSnrOn())
		//{
		//	m_stMotorZ.stHomeProfile.bAstate = !m_stMotorZ.stHomeProfile.bAstate;
		//	m_stMotorZ.InitSrchProf(m_stMotorZ.stHomeProfile);
		//	nResult = m_stMotorZ.Search(m_stMotorZ.stHomeProfile, NO_PROCESS_LIST, GMP_WAIT);
		//	m_stMotorZ.stHomeProfile.bAstate = !m_stMotorZ.stHomeProfile.bAstate;
		//	m_stMotorZ.InitSrchProf(m_stMotorZ.stHomeProfile);
		//}
		//else
		//{
		//	nResult = m_stMotorZ.Search(m_stMotorZ.stHomeProfile, NO_PROCESS_LIST, GMP_WAIT);
		//	m_stMotorZ.stHomeProfile.bAstate = !m_stMotorZ.stHomeProfile.bAstate;
		//	m_stMotorZ.InitSrchProf(m_stMotorZ.stHomeProfile);
		//	nResult = m_stMotorZ.Search(m_stMotorZ.stHomeProfile, NO_PROCESS_LIST, GMP_WAIT);
		//	m_stMotorZ.stHomeProfile.bAstate = !m_stMotorZ.stHomeProfile.bAstate;
		//	m_stMotorZ.InitSrchProf(m_stMotorZ.stHomeProfile);
		//}
		//m_stMotorZ.SetPosition(0);
		//MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT);
	}
	else
	{
		return GMP_FAIL;
	}

	return nResult;
}

INT CTA1::MoveX(TA1_X_CTRL_SIGNAL ctrl, BOOL bWait)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	INT		nResult		= GMP_SUCCESS;

	LONG lTempPosn = 0;

	CString szDebug = _T("");
	switch (ctrl)
	{
	case TA1_X_CTRL_GO_STANDBY_POSN:
		lTempPosn = m_lStandbyPosn;
		break;

	case TA1_X_CTRL_GO_PICK_GLASS_POSN:
		lTempPosn = m_lPickGlassPosn;
		break;

	//case TA1_X_CTRL_GO_PICK_GLASS_OFFSET:
	//	lTempPosn = m_lPickGlassPosn + m_lPickGlassOffset;
	//	break;

	case TA1_X_CTRL_GO_PLACE_ACF1WH_POSN:
		lTempPosn = m_lPlaceGlassPosn;
		break;

	case TA1_X_CTRL_GO_PLACE_ACF2WH_POSN:
		lTempPosn = m_lPlaceGlassPosn - DistanceToCount(g_dGToGOffset * 1000, m_stMotorX.stAttrib.dDistPerCount);
		break;

	case TA1_X_CTRL_GO_PLACE_ACF1WH_OFFSET: //20121011
		lTempPosn = m_lPlaceGlassPosn + m_lPlaceGlass1Offset + DistanceToCount((DOUBLE)pCACF1WH->m_lLoadXOffset, m_stMotorX.stAttrib.dDistPerCount);
		break;

	case TA1_X_CTRL_GO_PLACE_ACF2WH_OFFSET: //20121011
		lTempPosn = m_lPlaceGlassPosn - DistanceToCount(g_dGToGOffset * 1000, m_stMotorX.stAttrib.dDistPerCount) + m_lPlaceGlass2Offset + DistanceToCount((DOUBLE)pCACF2WH->m_lLoadXOffset, m_stMotorX.stAttrib.dDistPerCount);
		break;

	case TA1_X_CTRL_GO_PLACE_ACF1WH_PR_OFFSET:
		lTempPosn = m_lPlaceGlassPosn + m_lPlaceGlass1Offset + m_lXPlaceWhPROffset + DistanceToCount((DOUBLE)pCACF1WH->m_lLoadXOffset, m_stMotorX.stAttrib.dDistPerCount);
		break;

	case TA1_X_CTRL_GO_PLACE_ACF2WH_PR_OFFSET:
		lTempPosn = m_lPlaceGlassPosn - DistanceToCount(g_dGToGOffset * 1000, m_stMotorX.stAttrib.dDistPerCount) + m_lPlaceGlass2Offset + m_lXPlaceWhPROffset + DistanceToCount((DOUBLE)pCACF2WH->m_lLoadXOffset, m_stMotorX.stAttrib.dDistPerCount);
		break;
	}

	if (HMI_bDebugCal)	// 20140801 Yip: Only Show Message When Debug Calculation
	{
		szDebug.Format("MotorX Posn: %d", lTempPosn);
		DisplayMessage(szDebug);
	}
	return m_stMotorX.MoveAbsolute(lTempPosn, bWait);
}

INT CTA1::MoveZ(TA1_Z_CTRL_SIGNAL ctrl, BOOL bWait)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	INT		nResult		= GMP_SUCCESS;

	LONG lTempPosn = 0;
	DOUBLE dDistPerCnt	= m_stMotorZ.stAttrib.dDistPerCount;
	DOUBLE dGap = 0.0;

	switch (ctrl)
	{
	case TA1_Z_CTRL_GO_STANDBY_LEVEL:
		lTempPosn = m_lStandbyLevel;
		break;

	case TA1_Z_CTRL_GO_LOWER_LEVEL:
		lTempPosn = m_lLowerLevel;
		break;

	case TA1_Z_CTRL_GO_PLACE_LEVEL:
		if (IsBurnInDiagBond())
		{
			dGap = DistanceToCount(GAP_THICKNESS * FPC_GAP * 1000.0, dDistPerCnt); 
		}
		else
		{
			dGap = 0.0;
		}

		lTempPosn = m_lPlaceLevel + (LONG)dGap;
		break;

	case TA1_Z_CTRL_GO_PR_LEVEL:
		lTempPosn = m_lPRLevel;
		break;

	case TA1_Z_CTRL_GO_CALIB_PR_LEVEL:
		lTempPosn = m_lPRCalibLevel;
		break;

	}

	return m_stMotorZ.MoveAbsolute(lTempPosn, GMP_WAIT);
}

INT CTA1::MoveY(TA1_Y_CTRL_SIGNAL ctrl, BOOL bWait)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	INT		nResult		= GMP_SUCCESS;

	LONG lTempPosn = 0;

	switch (ctrl)
	{
	case TA1_Y_CTRL_GO_STANDBY_POSN:
		lTempPosn = 0;//m_lYStandbyPosn;
		break;


	}

	return m_stMotorY.MoveAbsolute(lTempPosn, GMP_WAIT);
}

INT CTA1::SyncZ()
{
	return m_stMotorZ.Sync();
}

INT CTA1::SyncX()
{

	return m_stMotorX.Sync();
}

INT CTA1::SyncY()
{
	return m_stMotorY.Sync();
}

INT CTA1::MoveZToPRLevel(BOOL bWait)
{
	if (
		MoveZ(TA1_Z_CTRL_GO_PR_LEVEL, bWait) != GMP_SUCCESS
	)
	{
		return GMP_FAIL;
	}

	return GMP_SUCCESS;
}

INT CTA1::MoveZToPRCalibLevel(BOOL bWait)
{

	if (
		MoveZ(TA1_Z_CTRL_GO_CALIB_PR_LEVEL, bWait) != GMP_SUCCESS
	)
	{
		return GMP_FAIL;
	}

	return GMP_SUCCESS;
}

INT CTA1::MoveAbsoluteY(LONG lCntY, BOOL bWait)
{
	if (
		(m_stMotorY.MoveAbsolute(lCntY, bWait) != GMP_SUCCESS)
	)
	{
		return GMP_FAIL;
	}
	
	return GMP_SUCCESS;
}

INT CTA1::MoveAbsoluteXY(LONG lCntX, LONG lCntY, BOOL bWait)
{

	if (
		(m_stMotorX.MoveAbsolute(lCntX, GMP_NOWAIT) != GMP_SUCCESS) ||
		(m_stMotorY.MoveAbsolute(lCntY, GMP_NOWAIT) != GMP_SUCCESS)
	)
	{
		return GMP_FAIL;
	}
	
	if (GMP_WAIT == bWait)
	{
		if (
			(m_stMotorX.Sync() != GMP_SUCCESS) ||
			(m_stMotorY.Sync() != GMP_SUCCESS)
		)
		{
			return GMP_FAIL;
		}
	}
	return GMP_SUCCESS;
}

INT CTA1::MoveRelativeXY(LONG lCntX, LONG lCntY, BOOL bWait)
{

	if (
		(m_stMotorX.MoveRelative(lCntX, GMP_NOWAIT) != GMP_SUCCESS) ||
		(m_stMotorY.MoveRelative(lCntY, GMP_NOWAIT) != GMP_SUCCESS)
	)
	{
		return GMP_FAIL;
	}
	
	if (GMP_WAIT == bWait)
	{
		if (
			(m_stMotorX.Sync() != GMP_SUCCESS) ||
			(m_stMotorY.Sync() != GMP_SUCCESS)
		)
		{
			return GMP_FAIL;
		}
	}
	return GMP_SUCCESS;
}

INT CTA1::MoveXYToBigFOVPRPosn(BOOL bWait)
{
	m_mtrTA1PRPosn.x = m_mtrBigFOVPRPosn.x + (LONG)m_dmtrBigFOVPROffset.x;	//mtrTemp.x;
	m_mtrTA1PRPosn.y = m_mtrBigFOVPRPosn.y + (LONG)m_dmtrBigFOVPROffset.y;	//mtrTemp.y;

	return MoveAbsoluteXY(m_mtrTA1PRPosn.x, m_mtrTA1PRPosn.y, bWait);
}

//INT CTA1::MoveXYToCurrPR1Posn(BOOL bWait)
//{
//	CString szMsg = "";
//	//MTR_POSN mtrTemp;
//
//	szMsg.Format("MoveXYToCurrPR1Posn : m_dCurrAlignAngle = %.3f", m_dCurrAlignAngle);
//	DisplayMessage(szMsg);
//
//	//CalPosnAfterRotate(m_mtrPR1Posn[m_lCurGlass], m_dCurrAlignAngle, &mtrTemp);
//
//	m_dmtrCurrPR1Posn.x = (DOUBLE)m_mtrPR1Posn.x + m_dmtrPR1Offset.x;	//mtrTemp.x;
//	m_dmtrCurrPR1Posn.y = (DOUBLE)m_mtrPR1Posn.y + m_dmtrPR1Offset.y;	//mtrTemp.y;
//
//	return MoveAbsoluteXY((LONG)m_dmtrCurrPR1Posn.x, (LONG)m_dmtrCurrPR1Posn.y, bWait);
//}

//INT CTA1::MoveXYToCurrPR2Posn(BOOL bWait)
//{
//	CString szMsg = "";
//	//MTR_POSN mtrTemp;
//
//	szMsg.Format("MoveXYToCurrPR2Posn : m_dCurrAlignAngle = %.3f", m_dCurrAlignAngle);
//	DisplayMessage(szMsg);
//
//	//CalPosnAfterRotate(m_mtrPR2Posn[m_lCurGlass], m_dCurrAlignAngle, &mtrTemp);
//
//	m_dmtrCurrPR2Posn.x = (DOUBLE)m_mtrPR2Posn.x + m_dmtrPR2Offset.x;	//mtrTemp.x;
//	m_dmtrCurrPR2Posn.y = (DOUBLE)m_mtrPR2Posn.y + m_dmtrPR2Offset.y;	//mtrTemp.y;
//
//	return MoveAbsoluteXY((LONG)m_dmtrCurrPR2Posn.x, (LONG)m_dmtrCurrPR2Posn.y, bWait);
//}


INT CTA1::VacuumON(BOOL bWait)
{
	return SetGLASS1_VacSol(ON, bWait);
}

/////////////////////////////////////////////////////////////////
//I/O Related
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//SETUP Related
/////////////////////////////////////////////////////////////////
VOID CTA1::ModuleSetupPreTask()
{
	//CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH	= (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CInPickArm *pCInPickArm = (CInPickArm*)m_pModule->GetStation("InPickArm");

	pCInPickArm->m_bCheckMotor	= TRUE;
	pCInPickArm->m_bCheckIO		= TRUE;

	pCACF1WH->m_bCheckMotor	= TRUE;
	pCACF1WH->m_bCheckIO		= TRUE;

	pCACF2WH->m_bCheckMotor	= TRUE;
	pCACF2WH->m_bCheckIO		= TRUE;

	m_bCheckMotor		= TRUE;
	m_bCheckIO			= TRUE;
	
	CheckModSelected(TRUE);
	
	SetDiagnSteps(g_lDiagnSteps);

	UpdateModuleSetupPosn();
	UpdateModuleSetupLevel();
	UpdateModuleSetupPara();
}

VOID CTA1::ModuleSetupPostTask()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH	= (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CInPickArm *pCInPickArm = (CInPickArm*)m_pModule->GetStation("InPickArm");

	pCInPickArm->m_bCheckMotor	= FALSE;
	pCInPickArm->m_bCheckIO		= FALSE;

	pCACF1WH->m_bCheckMotor	= FALSE;
	pCACF1WH->m_bCheckIO		= FALSE;

	pCACF2WH->m_bCheckMotor	= FALSE;
	pCACF2WH->m_bCheckIO		= FALSE;

	m_bCheckMotor		= FALSE;
	m_bCheckIO			= FALSE;

}

BOOL CTA1::ModuleSetupSetPosn(LONG lSetupTitle)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	BOOL		bResult	= TRUE;
	MTR_POSN	mtrOld;
	mtrOld.x	= 0;
	mtrOld.y	= 0;
	MTR_POSN TempPosn;
	TempPosn.x	= 0;
	TempPosn.y	= 0;

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
	TempPosn.y = m_stMotorY.GetEncPosn();

	switch (lSetupTitle)
	{
	case 0:
		mtrOld.x			= m_lStandbyPosn;

		m_lStandbyPosn		= TempPosn.x;
		break;

	case 1:
		mtrOld.x			= m_lPickGlassPosn;

		m_lPickGlassPosn	= TempPosn.x;

		lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "OPTION", "Reset Pick Glass Offset?");
		if (lAnswer == rMSG_CONTINUE)
		{
			m_lPickGlassOffset = 0;
		}
		else if (lAnswer == rMSG_TIMEOUT)
		{
			bResult = FALSE;
		}
		break;

	case 2:
		mtrOld.x			= m_lPickGlassOffset;

		m_lPickGlassOffset	= TempPosn.x - m_lPickGlassPosn;

		TempPosn.x			= m_lPickGlassOffset;
		break;

	case 3:
		mtrOld.x			= m_lPlaceGlassPosn;

		m_lPlaceGlassPosn	= TempPosn.x;
		break;

	case 4:
		// Not Shown
		break;

	case 5:
		mtrOld.x			= m_lPlaceGlass1Offset;

		m_lPlaceGlass1Offset	= TempPosn.x - m_lPlaceGlassPosn;

		TempPosn.x			= m_lPlaceGlass1Offset;
		break;

	case 6:
		mtrOld.x			= m_lPlaceGlass2Offset;

		m_lPlaceGlass2Offset	= TempPosn.x - m_lPlaceGlassPosn + DistanceToCount(g_dGToGOffset * 1000, m_stMotorX.stAttrib.dDistPerCount);

		TempPosn.x			= m_lPlaceGlass2Offset;
		break;

	case 7:
		mtrOld.x				= m_mtrPRCalibPosn.x;
		mtrOld.y				= m_mtrPRCalibPosn.y;

		m_mtrPRCalibPosn.x = TempPosn.x;
		m_mtrPRCalibPosn.y = TempPosn.y;
		break;


	}

	CAC9000Stn::ModuleSetupSetPosn(lSetupTitle, "X", mtrOld.x, TempPosn.x);

	return TRUE;
}

BOOL CTA1::ModuleSetupGoPosn(LONG lSetupTitle)
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
			MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS 
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
		nResult = SetupGoPickGlassOffset();
		break;

	case 3:
		nResult = SetupGoPlaceGlass1Posn();
		break;

	case 4:
		nResult = SetupGoPlaceGlass2Posn();
		break;

	case 5:
		nResult = SetupGoPlaceGlassOffset();
		break;

	case 6:
		nResult = SetupGoPlaceGlass2Offset();
		break;

	case 7:
		nResult = SetupGoPRCalibPosn();
		break;
	}
	
	return nResult;
}

BOOL CTA1::ModuleSetupSetLevel(LONG lSetupTitle)
{
	LONG lAnswer = rMSG_TIMEOUT;
	LONG lOldLevel		= 0;
	//LONG lPrePostLevel	= 0;
	LONG lTempLevel = 0;
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

	lTempLevel = m_stMotorZ.GetEncPosn();

	switch (lSetupTitle)
	{
	case 0:
		lOldLevel					= m_lStandbyLevel;

		m_lStandbyLevel				= lTempLevel;
		break;
	
	case 1:
		lOldLevel					= m_lPickLevel;

		m_lPickLevel				= lTempLevel;
		break;

	case 2:
		lOldLevel					= m_lPlaceLevel;

		m_lPlaceLevel				= lTempLevel;
		break;

	case 3:
		lOldLevel					= m_lLowerLevel;

		m_lLowerLevel				= lTempLevel;
		break;

	case 4:
		lOldLevel					= m_lPRLevel;

		m_lPRLevel				= lTempLevel;
		break;
	}

	CAC9000Stn::ModuleSetupSetLevel(lSetupTitle, lOldLevel, lTempLevel);

	return bResult;
}

BOOL CTA1::ModuleSetupGoLevel(LONG lSetupTitle)
{
	INT nResult = GMP_SUCCESS;
	CString szMsg;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "GO POSN OPERATION", szMsg);
		return FALSE;
	}

	switch (lSetupTitle)
	{
	case 0:
		nResult = SetupGoStandbyLevel();
		break;

	case 1:
		nResult = SetupGoPickLevel();	
		break;

	case 2:
		nResult = SetupGoPlaceLevel();	
		break;

	case 3:
		nResult = SetupGoLowerLevel();	
		break;

	case 4:
		nResult = SetupGoPRLevel();	
		break;
	}

	if (nResult != GMP_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CTA1::ModuleSetupSetPara(PARA stPara)
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
		//m_stGlass2WeakBlowSol.SetOnDelay(stPara.lPara); //edge cleaner
		break;

	case 3:
		m_stWeakBlowSol.SetOffDelay(stPara.lPara);
		//m_stGlass2WeakBlowSol.SetOffDelay(stPara.lPara); //edge cleaner
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
	
VOID CTA1::UpdateModuleSetupPosn()
{	
	INT i = 0;
	INT j = 0;

	// Assign Module Setup Posn/Level/Para
	// Position Tab
	i = 0;
	g_stSetupPosn[i].szPosn.Format("Standby Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lStandbyPosn;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 1
	g_stSetupPosn[i].szPosn.Format("Pick Glass Posn");	
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPickGlassPosn;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 2
	g_stSetupPosn[i].szPosn.Format("Pick Glass Offset");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= DEVICE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPickGlassOffset;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 3
	g_stSetupPosn[i].szPosn.Format("Place Glass1 Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPlaceGlassPosn;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 4
	g_stSetupPosn[i].szPosn.Format("Place Glass2 Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPlaceGlassPosn - DistanceToCount(g_dGToGOffset * 1000, m_stMotorX.stAttrib.dDistPerCount);
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 5
	g_stSetupPosn[i].szPosn.Format("Place Glass1 Offset");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= DEVICE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPlaceGlass1Offset;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 6
	g_stSetupPosn[i].szPosn.Format("Place Glass2 Offset");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= DEVICE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPlaceGlass2Offset;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 7
	g_stSetupPosn[i].szPosn.Format("PR Calib Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_mtrPRCalibPosn.x;
	g_stSetupPosn[i].lPosnY			= m_mtrPRCalibPosn.y;

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

VOID CTA1::UpdateModuleSetupLevel()
{
	INT i = 0;
	INT j = 0;

	// Level Tab
	i = 0; //1
	g_stSetupLevel[i].szLevel.Format("Standby Level");
	g_stSetupLevel[i].bEnable		= TRUE;
	g_stSetupLevel[i].bVisible		= TRUE;
	g_stSetupLevel[i].lType			= MACHINE_PARA;
	g_stSetupLevel[i].lLevelZ		= m_lStandbyLevel;

	i++;	// 2
	g_stSetupLevel[i].szLevel.Format("Pick Level");
	g_stSetupLevel[i].bEnable		= TRUE;
	g_stSetupLevel[i].bVisible		= TRUE;
	g_stSetupLevel[i].lType			= MACHINE_PARA;
	g_stSetupLevel[i].lLevelZ		= m_lPickLevel;

	i++;	// 3
	g_stSetupLevel[i].szLevel.Format("Place Level");
	g_stSetupLevel[i].bEnable		= TRUE;
	g_stSetupLevel[i].bVisible		= TRUE;
	g_stSetupLevel[i].lType			= MACHINE_PARA;
	g_stSetupLevel[i].lLevelZ		= m_lPlaceLevel;

	i++;	// 4
	g_stSetupLevel[i].szLevel.Format("Lower Level");
	g_stSetupLevel[i].bEnable		= TRUE;
	g_stSetupLevel[i].bVisible		= TRUE;
	g_stSetupLevel[i].lType			= MACHINE_PARA;
	g_stSetupLevel[i].lLevelZ		= m_lLowerLevel;

	i++;	// 5
	g_stSetupLevel[i].szLevel.Format("PR Level");
	g_stSetupLevel[i].bEnable		= TRUE;
	g_stSetupLevel[i].bVisible		= TRUE;
	g_stSetupLevel[i].lType			= DEVICE_PARA;
	g_stSetupLevel[i].lLevelZ		= m_lPRLevel;

	for (j = i + 1; j < NUM_OF_LEVEL_Z; j++)
	{
		g_stSetupLevel[j].szLevel.Format("Reserved");
		g_stSetupLevel[j].bEnable		= FALSE;
		g_stSetupLevel[j].bVisible		= FALSE;
		g_stSetupLevel[j].lType			= MACHINE_PARA;
		g_stSetupLevel[j].lLevelZ		= 0;
	}
}

VOID CTA1::UpdateModuleSetupPara()
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

INT CTA1::SetupGoStandbyPosn()
{
	INT nResult = GMP_SUCCESS;

	if (MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS)
	{
		return GMP_FAIL;
	}
	nResult = MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);	

	return nResult;
}

INT CTA1::SetupGoPRCalibPosn()
{
	INT nResult = GMP_SUCCESS;

	if (MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS)
	{
		return GMP_FAIL;
	}
	nResult = MoveXYToPRCalibPosn(GMP_WAIT);	

	return nResult;
}

INT CTA1::SetupGoPickGlassPosn()
{
	INT nResult = GMP_SUCCESS;

	if (
		MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS ||
		SetRotarySol(OFF, GMP_WAIT) != GMP_SUCCESS
		)
	{
		return GMP_FAIL;
	}

	nResult = MoveX(TA1_X_CTRL_GO_PICK_GLASS_POSN, GMP_WAIT);

	return nResult;
}

INT CTA1::SetupGoPickGlassOffset()
{
	INT nResult = GMP_SUCCESS;
	LONG lAnswer = rMSG_TIMEOUT;
	BOOL bGlass1 = TRUE;


	if (
		MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS ||
		SetRotarySol(OFF, GMP_WAIT) != GMP_SUCCESS
		)
	{
		return GMP_FAIL;
	}

	nResult = MoveX(TA1_X_CTRL_GO_PICK_GLASS_POSN, GMP_WAIT);

	return nResult;
}

INT CTA1::SetupGoPlaceGlass1Posn()
{
	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2");
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH	= (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CACFWH *pCACFxWH = NULL;
	LONG lGlassNum = NO_GLASS;

	INT nResult = GMP_SUCCESS;
	LONG lAnswer = rMSG_CANCEL;

	if (
		MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS ||
		SetRotarySol(ON, GMP_WAIT) != GMP_SUCCESS
		)
	{
		return GMP_FAIL;
	}

	//lAnswer = HMISelectionBox("TA1 GO PLACE GLASS POSN", "Select Where to Place Glass", "ACF1WH", "ACF2WH");
	//if (lAnswer == -1 || lAnswer == 11)
	//{
	//	HMIMessageBox(MSG_OK, "TA1 GO PLACE GLASS POSN", "Operation Abort!");
	//	return GMP_FAIL;
	//}
	//if (lAnswer == 0)
	//{
	//	lGlassNum = GLASS1;
	//}
	//else if (lAnswer == 1)
	//{
	//	lGlassNum = GLASS2;
	//}
	
	lGlassNum = GLASS1;

	if (lGlassNum <= NO_GLASS || lGlassNum >= MAX_NUM_OF_GLASS)
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage("CTA1::SetupGoPlaceGlass1Posn : SW Error");
		return GMP_FAIL;
	}

	if (lGlassNum == GLASS1)
	{
		pCACFxWH = pCACF1WH;
	}
	//else if (lGlassNum == GLASS2)
	//{
	//	pCACFxWH = pCACF2WH;
	//}
	else
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage("CTA1::PlaceGlass : SW Error");
		return GMP_FAIL;
	}

	if ((nResult = pCACFxWH->MoveXYToLoadPosn(GMP_WAIT)) == GMP_SUCCESS)
	{
		nResult = MoveX(TA1_X_CTRL_GO_PLACE_ACF1WH_POSN, GMP_WAIT);	
	}

	return nResult;
}

INT CTA1::SetupGoPlaceGlass2Posn()
{
	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2");
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH	= (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CACFWH *pCACFxWH = NULL;
	LONG lGlassNum = NO_GLASS;

	INT nResult = GMP_SUCCESS;
	LONG lAnswer = rMSG_CANCEL;

	if (
		MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS ||
		SetRotarySol(ON, GMP_WAIT) != GMP_SUCCESS
		)
	{
		return GMP_FAIL;
	}

	//lAnswer = HMISelectionBox("TA1 GO PLACE GLASS POSN", "Select Where to Place Glass", "ACF1WH", "ACF2WH");
	//if (lAnswer == -1 || lAnswer == 11)
	//{
	//	HMIMessageBox(MSG_OK, "TA1 GO PLACE GLASS POSN", "Operation Abort!");
	//	return GMP_FAIL;
	//}
	//if (lAnswer == 0)
	//{
	//	lGlassNum = GLASS1;
	//}
	//else if (lAnswer == 1)
	//{
	//	lGlassNum = GLASS2;
	//}
	
	lGlassNum = GLASS1;

	if (lGlassNum <= NO_GLASS || lGlassNum >= MAX_NUM_OF_GLASS)
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage("CTA1::SetupGoPlaceGlass1Posn : SW Error");
		return GMP_FAIL;
	}

	if (lGlassNum == GLASS1)
	{
		pCACFxWH = pCACF2WH;
	}
	//else if (lGlassNum == GLASS2)
	//{
	//	pCACFxWH = pCACF2WH;
	//}
	else
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage("CTA1::PlaceGlass : SW Error");
		return GMP_FAIL;
	}

	if ((nResult = pCACFxWH->MoveXYToLoadPosn(GMP_WAIT)) == GMP_SUCCESS)
	{
		nResult = MoveX(TA1_X_CTRL_GO_PLACE_ACF2WH_POSN, GMP_WAIT);	
	}

	return nResult;
}

INT CTA1::SetupGoPlaceGlassOffset() //20121011
{
	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2");
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH	= (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CACFWH *pCACFxWH = NULL;
	LONG lGlassNum = NO_GLASS;

	INT nResult = GMP_SUCCESS;
	LONG lAnswer = rMSG_CANCEL;

	if (
		MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS ||
		SetRotarySol(ON, GMP_WAIT) != GMP_SUCCESS
		)
	{
		return GMP_FAIL;
	}

	//lAnswer = HMISelectionBox("TA1 GO PLACE GLASS POSN", "Select Where to Place Glass", "ACF1WH", "ACF2WH");
	//if (lAnswer == -1 || lAnswer == 11)
	//{
	//	HMIMessageBox(MSG_OK, "TA1 GO PLACE GLASS POSN", "Operation Abort!");
	//	return GMP_FAIL;
	//}
	//if (lAnswer == 0)
	//{
	//	lGlassNum = GLASS1;
	//}
	//else if (lAnswer == 1)
	//{
	//	lGlassNum = GLASS2;
	//}
	
	lGlassNum = GLASS1;

	if (lGlassNum <= NO_GLASS || lGlassNum >= MAX_NUM_OF_GLASS)
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage("CTA1::SetupGoPlaceGlass1Posn : SW Error");
		return GMP_FAIL;
	}

	if (lGlassNum == GLASS1)
	{
		pCACFxWH = pCACF1WH;
	}
	//else if (lGlassNum == GLASS2)
	//{
	//	pCACFxWH = pCACF2WH;
	//}
	else
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage("CTA1::PlaceGlass : SW Error");
		return GMP_FAIL;
	}

	if ((nResult = pCACFxWH->MoveXYToLoadPosn(GMP_WAIT)) == GMP_SUCCESS)
	{
		nResult = MoveX(TA1_X_CTRL_GO_PLACE_ACF1WH_OFFSET, GMP_WAIT);	
	}

	return nResult;
}

INT CTA1::SetupGoPlaceGlass2Offset() //20121011
{
	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2");
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH	= (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CACFWH *pCACFxWH = NULL;
	LONG lGlassNum = NO_GLASS;

	INT nResult = GMP_SUCCESS;
	LONG lAnswer = rMSG_CANCEL;

	if (
		MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS ||
		SetRotarySol(ON, GMP_WAIT) != GMP_SUCCESS
		)
	{
		return GMP_FAIL;
	}

	//lAnswer = HMISelectionBox("TA1 GO PLACE GLASS POSN", "Select Where to Place Glass", "ACF1WH", "ACF2WH");
	//if (lAnswer == -1 || lAnswer == 11)
	//{
	//	HMIMessageBox(MSG_OK, "TA1 GO PLACE GLASS POSN", "Operation Abort!");
	//	return GMP_FAIL;
	//}
	//if (lAnswer == 0)
	//{
	//	lGlassNum = GLASS1;
	//}
	//else if (lAnswer == 1)
	//{
	//	lGlassNum = GLASS2;
	//}
	
	lGlassNum = GLASS1;

	if (lGlassNum <= NO_GLASS || lGlassNum >= MAX_NUM_OF_GLASS)
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage("CTA1::SetupGoPlaceGlass1Posn : SW Error");
		return GMP_FAIL;
	}

	if (lGlassNum == GLASS1)
	{
		pCACFxWH = pCACF2WH;
	}
	//else if (lGlassNum == GLASS2)
	//{
	//	pCACFxWH = pCACF2WH;
	//}
	else
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage("CTA1::PlaceGlass : SW Error");
		return GMP_FAIL;
	}

	if ((nResult = pCACFxWH->MoveXYToLoadPosn(GMP_WAIT)) == GMP_SUCCESS)
	{
		nResult = MoveX(TA1_X_CTRL_GO_PLACE_ACF2WH_OFFSET, GMP_WAIT);	
	}

	return nResult;
}

INT CTA1::SetupGoStandbyLevel()
{
	INT nResult = GMP_SUCCESS;

	nResult = MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT);	

	return nResult;
}

INT CTA1::SetupGoPRLevel()
{
	INT nResult = GMP_SUCCESS;

	nResult = MoveZ(TA1_Z_CTRL_GO_PR_LEVEL, GMP_WAIT);	

	return nResult;
}

INT CTA1::SetupGoLowerLevel()
{
	//CInPickArm *pCInPickArm = (CInPickArm*)m_pModule->GetStation("InPickArm");
	INT nResult = GMP_SUCCESS;
	//LONG lReply;
	CString szMsg;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", szMsg);
		return GMP_FAIL;
	}

	{
		if (MoveZ(TA1_Z_CTRL_GO_LOWER_LEVEL, GMP_WAIT) != GMP_SUCCESS)
		{
			return GMP_FAIL;
		}
	}

	return nResult;

}

INT CTA1::SetupGoPickLevel()
{
	CInPickArm *pCInPickArm = (CInPickArm*)m_pModule->GetStation("InPickArm");
	INT nResult = GMP_SUCCESS;
	LONG lReply;
	CString szMsg;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", szMsg);
		return GMP_FAIL;
	}

	if (!pCInPickArm->m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", pCInPickArm->GetStnName());
		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", szMsg);
		return GMP_FAIL;
	}

	if (
		MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		SetRotarySol(OFF, GMP_WAIT) != GMP_SUCCESS
		)
	{
		return GMP_FAIL;
	}

	lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "Move InPickArm to Place Posn?");
	if (lReply == BUTTON_1)	// YES
	{
		if (
			pCInPickArm->MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
			pCInPickArm->SetRotarySol(ON, GMP_WAIT) != GMP_SUCCESS ||
			MoveX(TA1_X_CTRL_GO_PICK_GLASS_POSN, GMP_WAIT) != GMP_SUCCESS
			) 
		{
			return GMP_FAIL;
		}
	}
	else if (lReply == BUTTON_3)
	{
		return GMP_FAIL;
	}
	
	lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "Move TA1 to Pick Level?");
	if (lReply == BUTTON_1)	// YES
	{
		if (MoveZ(TA1_Z_CTRL_GO_LOWER_LEVEL, GMP_WAIT) != GMP_SUCCESS)
		{
			return GMP_FAIL;
		}
	}
	else if (lReply == BUTTON_3)
	{
		return GMP_FAIL;
	}	

	return nResult;

}

INT CTA1::SetupGoPlaceLevel()
{
	CInPickArm *pCInPickArm = (CInPickArm*)m_pModule->GetStation("InPickArm");
	INT nResult = GMP_SUCCESS;
	LONG lReply;
	CString szMsg;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", szMsg);
		return GMP_FAIL;
	}

	if (!pCInPickArm->m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", pCInPickArm->GetStnName());
		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", szMsg);
		return GMP_FAIL;
	}

	if (
		MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS /*||
		SetRotarySol(OFF, GMP_WAIT) != GMP_SUCCESS*/
		)
	{
		return GMP_FAIL;
	}

	lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "Move TA1 to Place Posn?");
	if (lReply == BUTTON_1)	// YES
	{
		if (
			pCInPickArm->MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
			/*pCInPickArm->*/SetRotarySol(ON, GMP_WAIT) != GMP_SUCCESS ||
			MoveX(TA1_X_CTRL_GO_PLACE_ACF1WH_OFFSET, GMP_WAIT) != GMP_SUCCESS
			) 
		{
			return GMP_FAIL;
		}
	}
	else if (lReply == BUTTON_3)
	{
		return GMP_FAIL;
	}
	
	lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "Move TA1 to Place Level?");
	if (lReply == BUTTON_1)	// YES
	{
		if (MoveZ(TA1_Z_CTRL_GO_PLACE_LEVEL, GMP_WAIT) != GMP_SUCCESS)
		{
			return GMP_FAIL;
		}
	}
	else if (lReply == BUTTON_3)
	{
		return GMP_FAIL;
	}	

	return nResult;

}


LONG CTA1::HMI_SetZMoveLoopLimit(IPC_CServiceMessage &svMsg)	// 20140522 Yip
{
	LONG lLimit;
	svMsg.GetMsg(sizeof(LONG), &lLimit);

	m_lZMoveLoopLimit = lLimit;
	//SetAndLogParameter(__FUNCTION__, "Pick Place Retry Limit", m_lPickPlaceRetryLimit, lLimit);

	return 0;
}

LONG CTA1::HMI_LoopTestZMove(IPC_CServiceMessage &svMsg)
{
	BOOL bMode = TRUE;

	m_bZMoveLoopEnable = TRUE;
	TestZRepeatMove();

	bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

INT CTA1::TestZRepeatMove()
{
	LONG lReply = 0;
	LONG lLoopDelay = 800;
	CString szMsg("");

	if (!HMINumericKeys("Enter Test Loop Delay (ms)", 20*1000, 50, &lLoopDelay))
	{
		HMIMessageBox(MSG_OK, "TA1 Z TEST", "Loop Delay Input Invalid. OPERATION STOP");
		return GMP_FAIL;
	}

	szMsg.Format("LoopDelay = %ld", lLoopDelay);
	DisplayMessage(szMsg);

	lReply = HMIConfirmWindow("Yes", "NO", "Cancel", "OPTION", "Loop Test TA1-Z between StbyLvl and PickLvl?");
	if (lReply == BUTTON_1)	// YES
	{
		m_lZMoveLoopCount = 0;

			if (
				MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
				MoveX(TA1_X_CTRL_GO_PICK_GLASS_POSN, GMP_WAIT) != GMP_SUCCESS ||
				MoveTPlacePosn(GMP_WAIT) != GMP_SUCCESS
				)
			{
				HMIMessageBox(MSG_OK, "TA1 Z MOVE ERROR", "TA1-Z Move Standby Level Error. Operation Abort!");
				return GMP_FAIL;
			}


		while (m_bZMoveLoopEnable && m_lZMoveLoopCount++ < m_lZMoveLoopLimit)
		{
			if (MoveZ(TA1_Z_CTRL_GO_LOWER_LEVEL, GMP_WAIT) != GMP_SUCCESS)
			{
				HMIMessageBox(MSG_OK, "TA1 Z MOVE ERROR", "TA1-Z Move Lower Level Error. Operation Abort!");
				return GMP_FAIL;
			}
			
			if (MoveTPickPosn(GMP_WAIT) != GMP_SUCCESS)
			{
				HMIMessageBox(MSG_OK, "TA1 T MOVE ERROR", "TA1-T Move Place Position Error. Operation Abort!");
				return GMP_FAIL;
			}
			Sleep(100);
			
			if (MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS)
			{
				HMIMessageBox(MSG_OK, "TA1 Z MOVE ERROR", "TA1-Z Move Standby Level Error. Operation Abort!");
				return GMP_FAIL;
			}
			
			if (MoveTPlacePosn(GMP_WAIT) != GMP_SUCCESS)
			{
				HMIMessageBox(MSG_OK, "TA1 T MOVE ERROR", "TA1-T Move Pick Position Error. Operation Abort!");
				return GMP_FAIL;
			}
			Sleep(100);

			SetHmiVariable("HMI_lTA1ZMoveLoopCount");
			Sleep(lLoopDelay);
		}

	}
	else if (lReply == BUTTON_2 || lReply == BUTTON_3) //cancel
	{
		return GMP_SUCCESS;
	}	
	return GMP_SUCCESS;
}


INT CTA1::KeepQuiet()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	INT nResult = GMP_SUCCESS;

	if (!m_fHardware)
	{
		return nResult;
	}

	if (!IsGLASS1_VacSensorOn())
	{
		SetGLASS1_VacSol(OFF, GMP_NOWAIT);
	}
	//if (!IsLFPC_VacSensorOn())
	//{
	//	SetLFPCVacSol(OFF, GMP_NOWAIT);
	//}
	//if (!IsRFPC_VacSensorOn())
	//{
	//	SetRFPCVacSol(OFF, GMP_NOWAIT);
	//}
	return nResult;
}



VOID CTA1::PrintMachineSetup(FILE *fp)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString str = "";

	if (fp != NULL)
	{
		fprintf(fp, "\n<<<<%s>>>>\n", (const char*)GetStnName());
		fprintf(fp, "======================================================\n");
		fprintf(fp, "<<Posn>>\n");
		fprintf(fp, "%30s\t : %ld\n", "Standby Posn",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lStandbyPosn"]);
		fprintf(fp, "%30s\t : %ld\n", "Pick Posn",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPickGlassPosn"]);
		fprintf(fp, "%30s\t : %ld\n", "Place Posn",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPlaceGlassPosn"]);

		fprintf(fp, "<<Level>>\n");
		fprintf(fp, "%30s\t : %ld\n", "Standby Level",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lStandbyLevel"]);
	}

	CAC9000Stn::PrintMachineSetup(fp);
}

VOID CTA1::PrintDeviceSetup(FILE *fp)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString str = "";

	if (fp != NULL)
	{
		fprintf(fp, "\n<<<<%s>>>>\n", (const char*)GetStnName());
		fprintf(fp, "======================================================\n");
		fprintf(fp, "<<Posn>>\n");
		fprintf(fp, "%30s\t : %ld\n", "Pick Glass Offset",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lPickGlassOffset"]);

		fprintf(fp, "<<Level>>\n");
		fprintf(fp, "%30s\t : %ld\n", "Pick Level",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Level"]["m_lPickLevel"]); //20130315
		fprintf(fp, "%30s\t : %ld\n", "Place Level",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Level"]["m_lPlaceLevel"]);

		fprintf(fp, "<<Para>>\n");
		fprintf(fp, "%30s\t : %ld\n", "Vac On Delay",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Delays"][m_stGlass1VacSol.GetName()]["lOnDelay"]);
		fprintf(fp, "%30s\t : %ld\n", "Vac Off Delay",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Delays"][m_stGlass1VacSol.GetName()]["lOffDelay"]);
		fprintf(fp, "%30s\t : %ld\n", "WeakBlow On Delay",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Delays"][m_stWeakBlowSol.GetName()]["lOnDelay"]);
		fprintf(fp, "%30s\t : %ld\n", "WeakBlow Off Delay",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Delays"][m_stWeakBlowSol.GetName()]["lOffDelay"]);
		fprintf(fp, "%30s\t : %ld\n", "Pick Place Retry Limit",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Para"]["m_lPickPlaceRetryLimit"]);

	}

	CAC9000Stn::PrintDeviceSetup(fp);
}

#ifdef EXCEL_MACHINE_DEVICE_INFO
BOOL CTA1::printMachineDeviceInfo()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bResult = TRUE;
	CString szBlank("--");
	CString szModule = GetStnName();
	LONG lValue = 0;
	//DOUBLE dValue = 0.0;

	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lStandbyLevel"];
		bResult = printMachineInfoRow(szModule, CString("Level"), CString("StandbyLevel"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		bResult = CTransferArm::printMachineDeviceInfo();
	}
	//if (bResult)
	//{
	//	bResult = CAC9000Stn::printMachineDeviceInfo();
	//}
	return bResult;
}
#endif

// ------------------------------------------------------------------------
// HMI Command delcaration
// ------------------------------------------------------------------------
LONG CTA1::HMI_HomeAll(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	BOOL	bMode = TRUE;

	if (
		m_stMotorY.Home(GMP_WAIT) != GMP_SUCCESS ||
		HomeZ() != GMP_SUCCESS ||
		MoveTPlacePosn(GMP_WAIT) != GMP_SUCCESS ||
		m_stMotorX.Home(GMP_WAIT) != GMP_SUCCESS
	   )
	{
		HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
	}

	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

//LONG CTA1::HMI_PowerOnZ(IPC_CServiceMessage &svMsg)
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

// Commutate
//LONG CTA1::HMI_CommZ(IPC_CServiceMessage &svMsg)
////{
//	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
//
//
//	m_stMotorZ.Commutate();
//
//	return 0;
//}

// Home
//LONG CTA1::HMI_HomeZ(IPC_CServiceMessage &svMsg)
//{
//	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
//
//	HomeZ();
//	//m_stMotorZ.Home(GMP_WAIT);
//	
//
//	return 0;
//}

LONG CTA1::HMI_SetModSelected(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	//if (bMode)
	//{
	//	if (m_stMotorX.bHMI_On && m_stMotorZ.bHMI_On)
	//	{
	//		SetModSelected(bMode);
	//	}
	//	else
	//	{
	//		HMIMessageBox(MSG_OK, "WARNING", "Please turn on all the motors individually first.");
	//	}
	//}
	//else
	{
		SetModSelected(bMode);
	}

	return 0;
}

LONG CTA1::HMI_IndexZPos(IPC_CServiceMessage &svMsg)
{
	//CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	//if (pAppMod->IsSelectCOG902())//IsSelectEdgeCleaner())
	//{
	//	return 0;
	//}

	m_stMotorZ.MoveRelative(m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CTA1::HMI_IndexZNeg(IPC_CServiceMessage &svMsg)
{
	//CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	//if (pAppMod->IsSelectCOG902())//IsSelectEdgeCleaner())
	//{
	//	return 0;
	//}
	m_stMotorZ.MoveRelative(-m_lDiagnSteps, GMP_WAIT);
	return 0;
}

//Delay
//LONG CTA1::HMI_PickGlassOperation(IPC_CServiceMessage &svMsg)
//{
//	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
//	BOOL bOpDone						= TRUE;
//	//LONG lResponse						= 0;
//	LONG lSelection						= 0;
//	GlassNum lCurPickStatus			= MAX_NUM_OF_GLASS;
//
//	PickGlassOperation();
//	
//	MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
//
//	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
//	return 1;
//}

LONG CTA1::HMI_PlaceGlassOperation(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bOpDone			= TRUE;
	LONG lResponse			= 0;
	ACFWHUnitNum lACFWHx	= ACFWH_1;

	GLASS_FPC_VAC_STATE stVacState1 = GetGlassFPC1_VacSensorOnState();	// 20141112
	//GLASS_FPC_VAC_STATE stVacState2 = GetGlassFPC2_VacSensorOnState();	// 20141112
	
	lResponse = HMISelectionBox("OPTION", "Please Select Place to:", "ACFWH_1", "ACFWH_2");
	if (lResponse == -1 || lResponse == 11)
	{
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
		return 1;
	}

	if(lResponse == 0)
	{
		lACFWHx = ACFWH_1;
	}
	else if(lResponse == 1)
	{
		lACFWHx = ACFWH_2;
	}

	PlaceGlassOperation(lACFWHx);

	MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);

	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}

LONG CTA1::IPC_LoadMachineRecord()
{
	// for uploading records
	CString szPath = "";

	szPath = "d:\\sw\\AC9000\\Record\\" + pruACFUL_Calib.szFileName + ".zip";
	strcpy((char*) & (pruACFUL_Calib.stDownloadRecordCmd.acFilename[0]), szPath);

	return 1;
}

LONG CTA1::IPC_SaveMachineRecord()
{
	CString szPath = "";

	szPath = "d:\\sw\\AC9000\\Record\\" + pruACFUL_Calib.szFileName + ".zip";
	strcpy((char*) & (pruACFUL_Calib.stUploadRecordCmd.acFilename[0]), szPath);

	return 1;
}

LONG CTA1::IPC_LoadDeviceRecord()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	CString str = "";

	// for uploading records
	CString szPath = "";
	CString szDevice = pAppMod->m_szDeviceFile;
	INT nFind = szDevice.ReverseFind('.');

	szPath = "d:\\sw\\AC9000\\Record\\" + szDevice.Left(nFind) + "_" + pruIN1.szFileName + ".zip";
	strcpy((char*) & (pruIN1.stDownloadRecordCmd.acFilename[0]), szPath);

	return 1;
}

LONG CTA1::IPC_SaveDeviceRecord()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	CString str = "";

	// for uploading records
	CString szPath = "";
	CString szDevice = pAppMod->m_szDeviceFile;
	INT nFind = szDevice.ReverseFind('.');

	szPath = "d:\\sw\\AC9000\\Record\\" + szDevice.Left(nFind) + "_" + pruIN1.szFileName + ".zip";
	strcpy((char*) & (pruIN1.stUploadRecordCmd.acFilename[0]), szPath);

	return 1;
}

LONG CTA1::IPC_SaveMachineParam()
{	
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	
	pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lStandbyLevel"]		= m_lStandbyLevel;
	pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lLowerLevel"]		= m_lLowerLevel;

	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPickGlassPosn"]		= m_lPickGlassPosn;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPlaceGlassPosn"]		= m_lPlaceGlassPosn;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrPRCalibPosn.x"]		= m_mtrPRCalibPosn.x;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrPRCalibPosn.y"]		= m_mtrPRCalibPosn.y;

	return CTransferArm::IPC_SaveMachineParam();
}

LONG CTA1::IPC_LoadMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	
	m_lStandbyLevel		= pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lStandbyLevel"];
	m_lLowerLevel		= pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lLowerLevel"];
	m_lPickGlassPosn		= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPickGlassPosn"];
	m_lPlaceGlassPosn	= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPlaceGlassPosn"];
	m_mtrPRCalibPosn.x	= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrPRCalibPosn.x"];
	m_mtrPRCalibPosn.y	= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrPRCalibPosn.y"];

	return CTransferArm::IPC_LoadMachineParam();
}

LONG CTA1::IPC_SaveDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString szTmp;
	szTmp = GetStnName();

	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lPickGlassOffset"]		= m_lPickGlassOffset;
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lPlaceGlass1Offset"]	= m_lPlaceGlass1Offset;
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lPlaceGlass2Offset"]	= m_lPlaceGlass2Offset;
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrBigFOVPRPosn.x"]	= m_mtrBigFOVPRPosn.x;
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrBigFOVPRPosn.y"]	= m_mtrBigFOVPRPosn.y;
	pAppMod->m_smfDevice[GetStnName()]["AlignParam"]["m_dGlassRefAng"]			= m_dGlassRefAng;
	pAppMod->m_smfDevice[GetStnName()]["AlignParam"]["m_dmtrAlignRef.x"]		= m_dmtrAlignRef.x;
	pAppMod->m_smfDevice[GetStnName()]["AlignParam"]["m_dmtrAlignRef.y"]		= m_dmtrAlignRef.y;

	pAppMod->m_smfDevice[GetStnName()]["Level"]["m_lPRLevel"]	= m_lPRLevel;
	pAppMod->m_smfDevice[GetStnName()]["Level"]["m_lPickLevel"]			= m_lPickLevel;
	pAppMod->m_smfDevice[GetStnName()]["Level"]["m_lPlaceLevel"]			= m_lPlaceLevel;

	return CTransferArm::IPC_SaveDeviceParam();
}

LONG CTA1::IPC_LoadDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	
	m_lPickGlassOffset	= pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lPickGlassOffset"];
	m_lPlaceGlass1Offset = pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lPlaceGlass1Offset"]; //20121011
	m_lPlaceGlass2Offset = pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lPlaceGlass2Offset"]; //20121011
	m_mtrBigFOVPRPosn.x = pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrBigFOVPRPosn.x"]; //20121011
	m_mtrBigFOVPRPosn.y = pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrBigFOVPRPosn.y"]; //20121011
	m_dGlassRefAng			= pAppMod->m_smfDevice[GetStnName()]["AlignParam"]["m_dGlassRefAng"];
	m_dmtrAlignRef.x		= pAppMod->m_smfDevice[GetStnName()]["AlignParam"]["m_dmtrAlignRef.x"];
	m_dmtrAlignRef.y		= pAppMod->m_smfDevice[GetStnName()]["AlignParam"]["m_dmtrAlignRef.y"];

	m_lPRLevel = pAppMod->m_smfDevice[GetStnName()]["Level"]["m_lPRLevel"]; //20121011
	m_lPickLevel		= pAppMod->m_smfDevice[GetStnName()]["Level"]["m_lPickLevel"];
	m_lPlaceLevel	= pAppMod->m_smfDevice[GetStnName()]["Level"]["m_lPlaceLevel"];

	return CTransferArm::IPC_LoadDeviceParam();
}

// ------------------------------------------------------------------------
// Virtual CPRTaskStn Function delcaration
// -----------------------------------------------------------------------
CMotorInfo &CTA1::GetMotorX() 
{
	return m_stMotorX;
}

CMotorInfo &CTA1::GetMotorY()
{
	return m_stMotorY;
}

INT CTA1::MoveXYToPRCalibPosn(BOOL bWait)
{
	return MoveAbsoluteXY(m_mtrPRCalibPosn.x, m_mtrPRCalibPosn.y, bWait);
}

INT CTA1::MoveXYToCurrPR1Posn(BOOL bWait)
{
	return MoveAbsoluteXY(m_mtrBigFOVPRPosn.x, m_mtrBigFOVPRPosn.y, bWait);
}

INT CTA1::MoveXYToCurrPR2Posn(BOOL bWait)
{
	return MoveAbsoluteXY(m_mtrBigFOVPRPosn.x, m_mtrBigFOVPRPosn.y, bWait);
}
