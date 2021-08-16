/////////////////////////////////////////////////////////////////
//	TA2.cpp : interface of the CTA2 class
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

#define TA2_EXTERN
#include "TA1.h"
#include "TA2.h"
#include "ACF1WH.h"
#include "ACF2WH.h"
#include "InPickArm.h"

#include "TimeChart.h" //20120831


#include "WinEagle.h" //20141231

#include "SettingFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CTA2, CTransferArm)

CTA2::CTA2()
{
	//I/O and Motors are done!!
	// CAC9000Srn
	m_lStnErrorCode		= IDS_TA2_NOT_SELECTED_ERR;
	m_lModSelectMode	= GLASS_MOD;

	// CPRTaskStn
	//m_szPRTaskStnName	= GetStation().Trim('_'); //20150105
	//m_lPRStnErrorCode	= IDS_TA2_PR_MOD_NOT_SELECTED;
	//m_lPRModSelectMode	= GLASS_MOD;

	// Motor Related
	m_stMotorZ.InitMotorInfo("TA2_Z", &m_bModSelected);
	m_stMotorX.InitMotorInfo("TA2_X", &m_bModSelected);

	m_myMotorList.Add((CMotorInfo*)&m_stMotorZ);
	m_myMotorList.Add((CMotorInfo*)&m_stMotorX);

	// Si Related
	m_stGlass1VacSnr		= CSiInfo("TA2LVacSnr",				"IOMODULE_IN_PORT_4", BIT_8, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stGlass1VacSnr);
	m_stT1Snr				= CSiInfo("TA2RotaryUpperSnr",		"IOMODULE_IN_PORT_4", BIT_9, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stT1Snr);
	m_stT2Snr		= CSiInfo("TA2RotaryLowerSnr",				"IOMODULE_IN_PORT_4", BIT_10, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stT2Snr);
	m_stXLmtSnr				= CSiInfo("TA2LmtSnr",				"IOMODULE_IN_PORT_4", BIT_11, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stXLmtSnr);	
	m_stZHomeSnr			= CSiInfo("TA2FlipZHomeSnr",		"IOMODULE_IN_PORT_4", BIT_12, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stZHomeSnr);	
	m_stZLmtSnr				= CSiInfo("TA2FlipZLmtSnr",			"IOMODULE_IN_PORT_4", BIT_13, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stZLmtSnr);	

	// Handshake IN bits
	m_stFOUT_IN1		= CSiInfo("FOUT_IN1",	"IOMODULE_IN_PORT_6", BIT_11, ACTIVE_LOW); 
	m_mySiList.Add((CSiInfo*)&m_stFOUT_IN1);
	m_stFOUT_IN2		= CSiInfo("FOUT_IN2",	"IOMODULE_IN_PORT_6", BIT_12, ACTIVE_LOW); 
	m_mySiList.Add((CSiInfo*)&m_stFOUT_IN2);
	m_stFOUT_IN3		= CSiInfo("FOUT_IN3",	"IOMODULE_IN_PORT_6", BIT_13, ACTIVE_LOW); 
	m_mySiList.Add((CSiInfo*)&m_stFOUT_IN3);
	m_stFOUT_IN4		= CSiInfo("FOUT_IN4",	"IOMODULE_IN_PORT_6", BIT_14, ACTIVE_LOW); 
	m_mySiList.Add((CSiInfo*)&m_stFOUT_IN4);
	m_stFOUT_IN5		= CSiInfo("FOUT_IN5",	"IOMODULE_IN_PORT_6", BIT_15, ACTIVE_LOW); 
	m_mySiList.Add((CSiInfo*)&m_stFOUT_IN5);
	m_stFOUT_IN6		= CSiInfo("FOUT_IN6",	"IOMODULE_IN_PORT_6", BIT_16, ACTIVE_LOW); 
	m_mySiList.Add((CSiInfo*)&m_stFOUT_IN6);


	// So Related

	// Handshake OUT bits
	m_stFOUT_OUT1			= CSoInfo("FOUT_OUT1",		"IOMODULE_OUT_PORT_6", BIT_11, OTHER_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stFOUT_OUT1);
	m_stFOUT_OUT2			= CSoInfo("FOUT_OUT2",		"IOMODULE_OUT_PORT_6", BIT_12, OTHER_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stFOUT_OUT2);
	m_stFOUT_OUT3			= CSoInfo("FOUT_OUT3",		"IOMODULE_OUT_PORT_6", BIT_13, OTHER_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stFOUT_OUT3);
	m_stFOUT_OUT4			= CSoInfo("FOUT_OUT4",		"IOMODULE_OUT_PORT_6", BIT_14, OTHER_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stFOUT_OUT4);
	m_stFOUT_OUT5			= CSoInfo("FOUT_OUT5",		"IOMODULE_OUT_PORT_6", BIT_15, OTHER_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stFOUT_OUT5);
	m_stFOUT_OUT6			= CSoInfo("FOUT_OUT6",		"IOMODULE_OUT_PORT_6", BIT_16, OTHER_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stFOUT_OUT6);


	m_stGlass1VacSol		= CSoInfo("TA2VacSol",				"IOMODULE_OUT_PORT_3", BIT_1, TA2_SOL, ACTIVE_LOW);
	m_mySoList.Add((CSoInfo*)&m_stGlass1VacSol);
	m_stWeakBlowSol			= CSoInfo("TA2WBSol",				"IOMODULE_OUT_PORT_3", BIT_3, TA2_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stWeakBlowSol);
	m_stRotarySol			= CSoInfo("TA2TSol",				"IOMODULE_OUT_PORT_3", BIT_5, TA2_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stRotarySol);
	//m_stZBrakeSol			= CSoInfo("TA2ZBrakeSol",			"IOMODULE_OUT_PORT_3", BIT_7, TA2_SOL, ACTIVE_LOW);
	//m_mySoList.Add((CSoInfo*)&m_stZBrakeSol);

//	m_lPolarizerThickness[GLASS1] = 0; //20120927 pin detect polarizer
//	m_lPolarizerThickness[GLASS2] = 0;

	m_bIsVacError = FALSE; //20121224 vac error

	m_bUseFPCVacuum = FALSE; //20141112

	HMI_bStandAlone = FALSE;

	//m_lCurGlass	= GLASS1;

	//for (INT i = GLASS1; i < MAX_NUM_OF_GLASS; i++)
	//{
	//	m_stACFUL[i].bAlignPtSet		= FALSE;
	//	m_stACFUL[i].bBonded			= FALSE;
	//	m_stACFUL[i].bEnable			= TRUE;
	//	m_stACFUL[i].bValid			= FALSE;
	//	m_stACFUL[i].dmtrAlignPt1.x	= 0.0;
	//	m_stACFUL[i].dmtrAlignPt1.y	= 0.0;
	//	m_stACFUL[i].dmtrAlignPt2.x	= 0.0;
	//	m_stACFUL[i].dmtrAlignPt2.y	= 0.0;
	//	m_stACFUL[i].dmtrPolarizerPt1.x	= 0.0;
	//	m_stACFUL[i].dmtrPolarizerPt1.y	= 0.0;
	//	m_stACFUL[i].dmtrPolarizerPt2.x	= 0.0;
	//	m_stACFUL[i].dmtrPolarizerPt2.y	= 0.0;

	//	m_dAlignAngle[i]			= 0.0;
	//	m_mtrAlignCentre[i].x		= 0;
	//	m_mtrAlignCentre[i].y		= 0;
	//	m_mtrCurrAlignDelta[i].x	= 0;
	//	m_mtrCurrAlignDelta[i].y	= 0;
	//	m_dGlassRefAng[i]			= 0.0;
	//	m_dmtrAlignRef[i].x			= 0.0;
	//	m_dmtrAlignRef[i].y			= 0.0;
	//	m_dAlignRefAngle[i]			= 0.0;
	//	m_mtrPRCalibPosn.x			= 0;
	//	m_mtrPRCalibPosn.y			= 0;
	//	m_mtrPR1Posn[i].x = 0;
	//	m_mtrPR2Posn[i].y = 0;
	//}
}

CTA2::~CTA2()
{
}

BOOL CTA2::InitInstance()
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

				// Reset the Status 
				if( GetmySoList().GetAt(i)->GetName() != "TA2TSol")
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
			m_stMotorX.Commutate() != GMP_SUCCESS
		   )
		{
			m_bInitInstanceError			= TRUE;
			pAppMod->m_bHWInitError			= TRUE;
		}

		pAppMod->m_bTA2Comm = TRUE;

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
			pAppMod->m_bTA2Home	= TRUE;
		}
	}
	else
	{
		pAppMod->m_bTA2Comm = TRUE;
		pAppMod->m_bTA2Home	= TRUE;
	}
	return TRUE;
}

INT CTA2::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CTransferArm::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////


VOID CTA2::Operation()
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
VOID CTA2::RegisterVariables()
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
		//RegVariable(GetStation() + _T("bGlass2Exist"), &m_bGlass2Exist);

		RegVariable(GetStation() + _T("bDebugSeq"), &HMI_bDebugSeq);
		RegVariable(GetStation() + _T("bLogMotion"), &HMI_bLogMotion);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetModSelected"), HMI_SetModSelected);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeAll"), HMI_HomeAll);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDebugSeq"), HMI_SetDebugSeq);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetLogMotion"), HMI_SetLogMotion);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PickGlassOperation"), HMI_PickGlassOperation);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PlaceGlassOperation"), HMI_PlaceGlassOperation);
		RegVariable(GetStation() + _T("dCycleTime"), &m_dCycleTime);

		// -------------------------------------------------------
		// PR Relative Variable and Function
		// -------------------------------------------------------
		//RegVariable(GetStation() + _T("ulCurGlass"),	&m_lCurGlass);
		//RegVariable(GetStation() + _T("bPR1Loaded"),	&pruACFUL1.bLoaded);
		//RegVariable(GetStation() + _T("bPR2Loaded"),	&pruACFUL2.bLoaded);
		//RegVariable(GetStation() + _T("bAlignPtSet"),	&m_stACFUL[GLASS1].bAlignPtSet);
		//RegVariable(GetStation() + _T("bValid"),		&m_stACFUL[GLASS1].bValid);

		//RegVariable(GetStation() + _T("lPRDelay"), &m_lPRDelay);
		//RegVariable(GetStation() + _T("lCalibPRDelay"), &m_lCalibPRDelay);
		//RegVariable(GetStation() + _T("lPRRetryLimit"), &m_lPRRetryLimit);
		//RegVariable(GetStation() + _T("lRejectGlassLimit"), &m_lRejectGlassLimit);

		//RegVariable(GetStation() + _T("ulCurPRU"), &HMI_ulCurPRU);
		//RegVariable(GetStation() + _T("ulPRID"), &HMI_ulPRID);
		//RegVariable(GetStation() + _T("dPRMatchScore"), &HMI_dPRMatchScore);
		//RegVariable(GetStation() + _T("dPRAngleRange"), &HMI_dPRAngleRange);
		//RegVariable(GetStation() + _T("dPRPercentVar"), &HMI_dPRPercentVar);
		//RegVariable(GetStation() + _T("bShowPRSearch"), &HMI_bShowPRSearch);

		//RegVariable(GetStation() + _T("lPRSearchRange"), &HMI_lPRSearchRange);	 //20150107 Polarizer Detect

		////Meausement Routine
		//RegVariable(GetStation() + _T("dMeasuredDistance"), &m_dMeasuredDistance);
		//RegVariable(GetStation() + _T("dMeasuredAngle"), &m_dMeasuredAngle);
		//RegVariable(GetStation() + _T("bEnableMeasure"), &m_bEnableMeasure);

		//RegVariable(GetStation() + _T("bACFULGlass2AlignPtSet"),	&m_stACFUL[GLASS2].bAlignPtSet);
		//RegVariable(GetStation() + _T("bACFULGlass2Valid"),		&m_stACFUL[GLASS2].bValid);
		// -------------------------------------------------------
		// TransferArm Variable and Function
		// -------------------------------------------------------
		RegVariable(_T("HMI_bTA2XOn"), &m_stMotorX.bHMI_On);
		RegVariable(_T("HMI_lTA2XEncoder"), &m_stMotorX.lHMI_CurrPosn);
		RegVariable(GetStation() + _T("dXDistPerCount"), &m_stMotorX.stAttrib.dDistPerCount);

		RegVariable(_T("HMI_bTA2ZOn"), &m_stMotorZ.bHMI_On);
		RegVariable(_T("HMI_lTA2ZEncoder"), &m_stMotorZ.lHMI_CurrPosn);
		RegVariable(GetStation() + _T("dZDistPerCount"), &m_stMotorZ.stAttrib.dDistPerCount);
		
		RegVariable(_T("HMI_bTA2G1VacSnr"), &m_stGlass1VacSnr.bHMI_Status);
		RegVariable(_T("HMI_bTA2RotaryUpperSnr"), &m_stT1Snr.bHMI_Status);
		RegVariable(_T("HMI_bTA2RotaryLowerSnr"), &m_stT2Snr.bHMI_Status);
		RegVariable(_T("HMI_bTA2XLmtSnr"), &m_stXLmtSnr.bHMI_Status);
		RegVariable(_T("HMI_bTA2FlipZHomeSnr"), &m_stZHomeSnr.bHMI_Status);
		RegVariable(_T("HMI_bTA2FlipZLmtSnr"), &m_stZLmtSnr.bHMI_Status);

		RegVariable(_T("HMI_bTA2G1VacSol"), &m_stGlass1VacSol.bHMI_Status);
		RegVariable(_T("HMI_bTA2WeakBlowSol"), &m_stWeakBlowSol.bHMI_Status);
		RegVariable(_T("HMI_bTA2TRotateSol"), &m_stRotarySol.bHMI_Status);
		RegVariable(_T("HMI_bTA2ZBrakeSol"), &m_stZBrakeSol.bHMI_Status);

		RegVariable(GetStation() + _T("lPickPlaceRetryLimit"), &m_lPickPlaceRetryLimit);
		RegVariable(GetStation() + _T("bStandAlone"), &HMI_bStandAlone);
		RegVariable(GetStation() + _T("bManualPickCOF"), &HMI_bManualPickCOF);

		RegVariable(GetStation() + _T("bUseFPCVacuum"), &m_bUseFPCVacuum); //20141112

		RegVariable(_T("HMI_lTA1ZMoveLoopLimit"), &m_lZMoveLoopLimit); //20150615
		RegVariable(_T("HMI_lTA1ZMoveLoopCount"), &m_lZMoveLoopCount);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetZMoveLoopLimit"), HMI_SetZMoveLoopLimit);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("LoopTestZMove"), HMI_LoopTestZMove);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleUseFPCVacuum"), HMI_ToggleUseFPCVacuum);

		// 20141022
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleLFPCVacSol"), HMI_ToggleLFPCVacSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleRFPCVacSol"), HMI_ToggleRFPCVacSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleTSol"), HMI_ToggleTSol);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnX"), HMI_PowerOnX);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("CommX"), HMI_CommX);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeX"), HMI_HomeX);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnZ"), HMI_PowerOnZ);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("CommZ"), HMI_CommZ);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeZ"), HMI_HomeZ);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleWeakBlowSol"), HMI_ToggleWeakBlowSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleG1VacSol"), HMI_ToggleG1VacSol);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDiagnSteps"), HMI_SetDiagnSteps);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexXPos"), HMI_IndexXPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexXNeg"), HMI_IndexXNeg);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexZPos"), HMI_IndexZPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexZNeg"), HMI_IndexZNeg);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPickPlaceRetryLimit"), HMI_SetPickPlaceRetryLimit);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetStandAlone"), HMI_SetStandAlone);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetManualPickCOF"), HMI_SetManualPickCOF);
		
		RegVariable(_T("HMI_bFOutIn1"), &m_stFOUT_IN1.bHMI_Status);
		RegVariable(_T("HMI_bFOutIn2"), &m_stFOUT_IN2.bHMI_Status);
		RegVariable(_T("HMI_bFOutIn3"), &m_stFOUT_IN3.bHMI_Status);		
		RegVariable(_T("HMI_bFOutIn4"), &m_stFOUT_IN4.bHMI_Status);		
		RegVariable(_T("HMI_bFOutIn5"), &m_stFOUT_IN5.bHMI_Status);		
		RegVariable(_T("HMI_bFOutIn6"), &m_stFOUT_IN6.bHMI_Status);
		
		RegVariable(_T("HMI_bFOutOut1"), &m_stFOUT_OUT1.bHMI_Status);
		RegVariable(_T("HMI_bFOutOut2"), &m_stFOUT_OUT2.bHMI_Status);
		RegVariable(_T("HMI_bFOutOut3"), &m_stFOUT_OUT3.bHMI_Status);		
		RegVariable(_T("HMI_bFOutOut4"), &m_stFOUT_OUT4.bHMI_Status);		
		RegVariable(_T("HMI_bFOutOut5"), &m_stFOUT_OUT5.bHMI_Status);		
		RegVariable(_T("HMI_bFOutOut6"), &m_stFOUT_OUT6.bHMI_Status);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleFOutOut1"), HMI_ToggleFOutOut1);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleFOutOut2"), HMI_ToggleFOutOut2);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleFOutOut3"), HMI_ToggleFOutOut3);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleFOutOut4"), HMI_ToggleFOutOut4);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleFOutOut5"), HMI_ToggleFOutOut5);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleFOutOut6"), HMI_ToggleFOutOut6);

		
		//Measurement routine
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("MeasureSetZero"), HMI_MeasureSetZero);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("MeasureEnable"), HMI_MeasureEnable);
		// -------------------------------------------------------
		// varibales
		// -------------------------------------------------------

		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("DetectLine"), HMI_DetectLine); //20150107 Polarizer Detect
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SrchPR"), HMI_SrchPR);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRRetryLimit"), HMI_SetPRRetryLimit);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetRejectGlassLimit"), HMI_SetRejectGlassLimit);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRDelay"), HMI_SetPRDelay);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetCalibPRDelay"), HMI_SetCalibPRDelay);

		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRMatchScore"), HMI_SetPRMatchScore);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRAngRange"), HMI_SetPRAngRange);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRPercentVar"), HMI_SetPRPercentVar);

		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRSearchRange"), HMI_SetPRSearchRange);	//20150107 Polarizer Detect

		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRSelected"), HMI_SetPRSelected);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("UpdateSetupStatus"), HMI_UpdateSetupStatus);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SelectGlassNum"), HMI_SelectGlassNum);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SelectPRU"), HMI_SelectPRU);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRPosn"), HMI_SetPRPosn);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SearchAp1"), HMI_SearchAp1);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SearchAp2"), HMI_SearchAp2);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SearchCentre"), HMI_SearchCentre);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SearchPolarP1"), HMI_SearchPolarP1);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SearchPolarP2"), HMI_SearchPolarP2);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SearchPolarizer"), HMI_SearchPolarizer);	//20150115

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
VOID CTA2::PreStartOperation()
{
	m_bModError = FALSE;
	m_bGlass1Exist = FALSE;
	m_bGlass2Exist = FALSE;
	m_bIsVacError = FALSE; //20121224 vac error
	
	HMI_bManualPickCOF = FALSE;
	//m_lPRRetryCounter = 0;

//#ifdef ACF_UL_PR //20150124
//	m_bPRDone[GLASS1][PR1] = FALSE;
//	m_bPRDone[GLASS1][PR2] = FALSE;
//	m_bPRDone[GLASS2][PR1] = FALSE;
//	m_bPRDone[GLASS2][PR2] = FALSE;
//	//m_bIsDoTA2PR = FALSE;
//#endif

	if (!m_bModSelected)
	{	
		TA2_Status = ST_MOD_NOT_SELECTED;
		m_qSubOperation = TA2_STOP_Q;
		m_bStopCycle = FALSE;
	}
	else
	{
		LONG lTmpCount = 0;
		//while (!g_bIsTA3Ready && lTmpCount++ < 8000 / 500) //20120321 waiting for TA3 go to standby or timeout
		//{
		//	Sleep(500);
		//}
		if (
			((m_nLastError = SetWeakBlowSol(OFF)) != GMP_SUCCESS) || 
			((m_nLastError = MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_WAIT)) != GMP_SUCCESS) //||
			//((m_nLastError = MoveTPickPosn(GMP_WAIT)) != GMP_SUCCESS) 
		   )
		{
			Command(glSTOP_COMMAND, TRUE);
			TA2_Status = ST_STOP;
			m_qSubOperation = TA2_STOP_Q;
			m_bStopCycle = TRUE;
		}
		else
		{
			if(!IsGLASS1_VacSensorOn() || IsBurnInDiagBond())
			{
				TA2_GLASS_Status = ST_EMPTY;
			}
			else if(IsGLASS1_VacSensorOn() && TA2_GLASS_Status != ST_ERROR)
			{
				TA2_GLASS_Status = ST_READY;
			}

			TA2_POSN = MAX_NUM_OF_ACFWH;
			TA2_Status = ST_IDLE;
			m_qSubOperation = TA2_IDLE_Q;
			m_bStopCycle = FALSE;
			m_lPickPlaceRetryCounter = 0;

			if(HMI_bStandAlone)
			{
				HS_TA2SetMachineReady(ON);
				HS_TA2SetOKSendReady(OFF);
				HS_TA2SetNGSendReady(OFF);
				HS_TA2SetVacOffACK(OFF);
			}
		}
	}

	// cycle time
	m_dwMachineTime = GetTickCount();
	m_dwStartTime = GetTickCount();
	for (m_nCycleCounter = 0; m_nCycleCounter < 10; m_nCycleCounter++)
	{
		m_dTempCycleTime[m_nCycleCounter] = 0;
	}
	m_nCycleCounter = 0;
	m_dCycleTime = 0.0;

	Motion(FALSE);
	State(IDLE_Q);

	m_deqDebugSeq.clear(); //20131010
	m_qPreviousSubOperation = m_qSubOperation;
	UpdateDebugSeq(GetSequenceString(), TA2_Status, TA2_GLASS_Status);
	
	CAC9000Stn::PreStartOperation();
}

VOID CTA2::AutoOperation()
{

//	CTA3 *pCTA3	= (CTA3*)m_pModule->GetStation("TA3");

	// stop command received: end operation after cycle completed
	if (Command() == glSTOP_COMMAND || g_bStopCycle)
	{
		m_bStopCycle = TRUE;
	}

	OperationSequence();

	// cycle completed or motor error or abort requested: end operation immediately
	if (
		(m_bModSelected && m_qSubOperation == TA2_STOP_Q) || 
		//(pCTA3->m_bModSelected && pCTA3->m_qSubOperation == TA3_STOP_Q) ||  //20140513
		(!m_bModSelected && m_bStopCycle) || 
		GMP_SUCCESS != m_nLastError || IsAbort() 
	   )
	{
		if (m_fHardware && m_bModSelected)
		{

			if (!m_bIsVacError) //20121224 vac error
			{
				MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
			}
			else
			{
				DisplayMessage("TA2 VAC error not move to standby");
			}

			if (AutoMode == BURN_IN) //20130620
			{
				if (!IsGLASS1_VacSensorOn())
				{
					SetGLASS1_VacSol(OFF);
				}
			}
		}

		if (GMP_SUCCESS != m_nLastError)
		{
			m_bModError = TRUE;
		}

		TA2_Status = ST_STOP;
		Command(glSTOP_COMMAND);
		State(STOPPING_Q);
		m_bStopCycle = FALSE;

		HS_TA2SetMachineReady(OFF);
		HS_TA2SetOKSendReady(OFF);
		HS_TA2SetNGSendReady(OFF);
		HS_TA2SetVacOffACK(OFF);
		
		CAC9000Stn::StopOperation();
	}
}

/////////////////////////////////////////////////////////////////
//Operational Sequences
/////////////////////////////////////////////////////////////////
CString CTA2::GetSequenceString()
{
	switch (m_qSubOperation)
	{
	case TA2_IDLE_Q:
		return "TA2_IDLE_Q";

	case TA2_WAIT_TA_MANAGER_REQUEST_Q:
		return "TA2_WAIT_TA_MANAGER_REQUEST_Q";

	case TA2_PICK_COF_Q:
		return "TA2_PICK_COF_Q";

	case TA2_CHECK_PICK_ERROR_Q:
		return "TA2_CHECK_PICK_ERROR_Q";

	case TA2_CHECK_COF_ERROR_Q:
		return "TA2_CHECK_COF_ERROR_Q";

	case TA2_REJECT_COF_Q:
		return "TA2_REJECT_COF_Q";

	case TA2_WAIT_PICKARM_ACK_Q:
		return "TA2_WAIT_PICKARM_ACK_Q";

	case TA2_COF_VAC_OFF_REQUEST_Q:
		return "TA2_COF_VAC_OFF_REQUEST_Q";

	case TA2_WAIT_PICK_COF_COMPLETE_Q:
		return "TA2_WAIT_PICK_COF_COMPLETE_Q";


	case TA2_STOP_Q:
		return "TA2_STOP_Q";
	}

	return "ASSASSINATE JASON";
}


VOID CTA2::OperationSequence()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CINWH *pCINWH = (CINWH*)m_pModule->GetStation("INWH");
	CTimeChart *pCTimeChart = (CTimeChart*)m_pModule->GetStation("TimeChart");
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	m_nLastError = GMP_SUCCESS;
	
	CString szMsg = "";

	//BOOL bDebug = FALSE;

	//GLASS_FPC_VAC_STATE stVacState1;	// 20141112
	//GLASS_FPC_VAC_STATE stVacState2;	// 20141112


	switch (m_qSubOperation)
	{
	case TA2_IDLE_Q:	// 0
		
		if (m_bStopCycle)
		{
			m_qSubOperation = TA2_STOP_Q;	
		}
		else if(IsGLASS1_VacSensorOn() || (IsBurnInDiagBond() && (TA2_GLASS_Status == ST_READY)))
		{
			TA2_Status = ST_BUSY;
			m_qSubOperation = TA2_CHECK_COF_ERROR_Q;		//4
		}
		else	//NO COF
		{
			if (
				((m_nLastError = MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT)) == GMP_SUCCESS) &&
				((m_nLastError = MoveTPickPosn(GMP_NOWAIT)) == GMP_SUCCESS) &&
				((m_nLastError = MoveZ(TA2_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) == GMP_SUCCESS)
				)
			{
				TA2_Status = ST_EMPTY;
				m_qSubOperation = TA2_WAIT_TA_MANAGER_REQUEST_Q;		//1
			}
		}

		break;

	case TA2_WAIT_TA_MANAGER_REQUEST_Q:	// 1
		if (m_bStopCycle)
		{
			m_qSubOperation = TA2_STOP_Q;	
		}		
		else if((TA_MANAGER_Status == ST_ACFWH1_REQ_UNLOAD) || (TA_MANAGER_Status == ST_ACFWH2_REQ_UNLOAD))
		{
			if(TA_MANAGER_Status == ST_ACFWH1_REQ_UNLOAD)
			{
				m_lCurrCheck = TA2_POSN = ACFWH_1;
			//	pCACFxWH = pCACF1WH;
				TA2_Status = ST_ACFWH1_ACK_UNLOAD;

				if (
					((m_nLastError = MoveX(TA2_X_CTRL_GO_PICK_ACF1WH_OFFSET, GMP_WAIT)) == GMP_SUCCESS)
					)
				{
					m_qSubOperation = TA2_PICK_COF_Q;		//2
				}
			}
			else if(TA_MANAGER_Status == ST_ACFWH2_REQ_UNLOAD)
			{
				m_lCurrCheck = TA2_POSN = ACFWH_2;
			//	pCACFxWH = pCACF2WH;
				TA2_Status = ST_ACFWH2_ACK_UNLOAD;

				if (
					((m_nLastError = MoveX(TA2_X_CTRL_GO_PICK_ACF2WH_OFFSET, GMP_WAIT)) == GMP_SUCCESS)
					)
				{
					m_qSubOperation = TA2_PICK_COF_Q;		//2
				}
			}
		}
		break;

	case TA2_PICK_COF_Q:	// 2
		
		if (m_bStopCycle)
		{
			TA2_Status = ST_STOP;
			m_qSubOperation = TA2_STOP_Q;	
		}	
		else if(ACF_WH_Status[m_lCurrCheck] == ST_PICK_READY1_EMPTY2)
		//else
		{
			if(
				((m_nLastError = PickGlass(m_lCurrCheck)) == GMP_SUCCESS)
				)
			{
				m_qSubOperation = TA2_CHECK_PICK_ERROR_Q;	//3
			}
		}

		break;

	case TA2_CHECK_PICK_ERROR_Q:	// 3

		if (IsGLASS1_VacSensorOn() || IsBurnInDiagBond())	//Pick OK
		{
			ACF_WH_Status[m_lCurrCheck] = ST_EMPTY;		//tell ACFWH to request NEW Panel
			m_lPickPlaceRetryCounter = 0;
			
			TA2_GLASS_Status = ACF_GLASS_Status[m_lCurrCheck][GLASS1];
			ACF_GLASS_Status[m_lCurrCheck][GLASS1] = ST_EMPTY;

			TA2_Status = ST_BUSY;

			TA2_POSN = MAX_NUM_OF_ACFWH;

			if (
				((m_nLastError = MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_WAIT)) == gnAMS_OK)
			   )
			{
				if (m_bStopCycle)
				{
					m_qSubOperation = TA2_STOP_Q;
				}
				else
				{
					m_qSubOperation = TA2_CHECK_COF_ERROR_Q;	//4
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
					TA2_Status = ST_VAC_ERROR;
					m_qSubOperation = TA2_STOP_Q;	
					DisplayMessage("TA2 ERROR: Pick Glass Retry Limit Hit");
					m_bModError = TRUE;
					SetError(IDS_TA2_PLACE_GLASS_ERR);
					//SetError(IDS_MAINBOND_WH_GLASS1_VAC_ERR + PPAction + (m_lCurrTargetPosn * CONVERT_TO_MAPPING));
					m_bIsVacError = TRUE; //20121224 vac error
				}
				else
				{
					m_qSubOperation = TA2_PICK_COF_Q;	//2
				}
			}
			else
			{
				if (m_bStopCycle)
				{
					m_qSubOperation = TA2_STOP_Q; 
				}	
				else
				{
					m_qSubOperation = TA2_PICK_COF_Q;    // go to 2
				}	
			}
		}

		break;

	case TA2_CHECK_COF_ERROR_Q:	// 4
		
		if (m_bStopCycle)
		{
			m_qSubOperation = TA2_STOP_Q;	
		}		
		else if(/*HMI_bStandAlone ||*/ IsErrorCOF())
		{
			m_qSubOperation = TA2_REJECT_COF_Q;		//5
		}
		else	//Good COF
		{
			if (
				((m_nLastError = MoveTPlacePosn(GMP_NOWAIT)) == GMP_SUCCESS) &&
				((m_nLastError = MoveZ(TA2_Z_CTRL_GO_LOWER_LEVEL, GMP_WAIT)) == GMP_SUCCESS) &&
				((m_nLastError = MoveX(TA2_X_CTRL_GO_PLACE_GLASS_POSN, GMP_WAIT)) == GMP_SUCCESS)
				)
			{
				HS_TA2SetOKSendReady(ON);
				m_qSubOperation = TA2_WAIT_PICKARM_ACK_Q;		//6
			}
		}

		break;

	case TA2_REJECT_COF_Q:	// 5

		if (m_bStopCycle)
		{
			m_qSubOperation = TA2_STOP_Q;	
		}
		else
		{
			if (
				((m_nLastError = MoveTPickPosn(GMP_NOWAIT)) == GMP_SUCCESS) &&
				((m_nLastError = MoveZ(TA2_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) == GMP_SUCCESS) &&
				((m_nLastError = MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_WAIT)) == GMP_SUCCESS) &&
				((m_nLastError = RejectCOF()) == GMP_SUCCESS)
				)
			{
				TA2_Status = ST_IDLE;
				TA2_GLASS_Status = ST_EMPTY;
				m_qSubOperation = TA2_IDLE_Q;		//0
			}
		}

		break;

	case TA2_WAIT_PICKARM_ACK_Q:	// 6
		
		if (m_bStopCycle)
		{
			m_qSubOperation = TA2_STOP_Q;	
		}
		else if((HS_IsFxMachineReady() && HS_IsFxPickAction()) || /*IsBurnInDiagBond()*/HMI_bStandAlone)
		{
			//if (
			//	((m_nLastError = MoveZ(TA2_Z_CTRL_GO_STANDBY_LEVEL, GMP_NOWAIT)) == GMP_SUCCESS) 
			//	)
			{
				m_qSubOperation = TA2_COF_VAC_OFF_REQUEST_Q;		//7
			}
		}
		break;

	case TA2_COF_VAC_OFF_REQUEST_Q:	// 7
		
		if (!HS_IsFxMachineReady() && !HMI_bStandAlone /*|| (IsBurnInDiagBond() && HMI_bStandAlone)*/)
		{
			m_qSubOperation = TA2_WAIT_PICKARM_ACK_Q;	//6
		}
		else if(HS_IsFxVacOffRequest() || (IsBurnInDiagBond() && HMI_bStandAlone))
		{
			if (
				((m_nLastError = SetGLASS1_VacSol(OFF, GMP_WAIT)) == GMP_SUCCESS) &&
				((m_nLastError = SetWeakBlowSol(ON, GMP_WAIT)) == GMP_SUCCESS) &&
				((m_nLastError = SetWeakBlowSol(OFF, GMP_NOWAIT)) == GMP_SUCCESS)
				)
			{
				HS_TA2SetVacOffACK(ON);
				m_qSubOperation = TA2_WAIT_PICK_COF_COMPLETE_Q;		//8
			}
		}
		else if(HMI_bStandAlone)
		{
			//if (
			//	((m_nLastError = SetGLASS1_VacSol(OFF, GMP_WAIT)) == GMP_SUCCESS)
			//	)
			{
				//GetHmiVariable("TA2_bManualPickCOF");
				if(HMI_bManualPickCOF)
				{
					HMI_bManualPickCOF = !HMI_bManualPickCOF;
					//SetHmiVariable("TA2_bManualPickCOF");
					if (
						((m_nLastError = SetGLASS1_VacSol(OFF, GMP_WAIT)) == GMP_SUCCESS)
						)
					{
						m_qSubOperation = TA2_WAIT_PICK_COF_COMPLETE_Q;		//8
					}
				}
			}
		}
		break;

	case TA2_WAIT_PICK_COF_COMPLETE_Q:	// 8
		
		if (HS_IsFxPickError() && !/*IsBurnInDiagBond()*/HMI_bStandAlone)
		{
			m_qSubOperation = TA2_STOP_Q;	
		}
		else if(!HS_IsFxVacOffRequest() || !HS_IsFxPickAction() || /*IsBurnInDiagBond()*/HMI_bStandAlone)
		{
			HS_TA2SetOKSendReady(OFF);
			HS_TA2SetNGSendReady(OFF);
			HS_TA2SetVacOffACK(OFF);
			
			TC_ResetTime();
			m_dTempCycleTime[m_nCycleCounter] = (DOUBLE)(GetTickCount() - m_dwStartTime) / 1000.0;

			if (
				m_dTempCycleTime[0] == 0.0 || 
				m_dTempCycleTime[1] == 0.0 || 
				m_dTempCycleTime[2] == 0.0 || 
				m_dTempCycleTime[3] == 0.0 || 
				m_dTempCycleTime[4] == 0.0 || 
				m_dTempCycleTime[5] == 0.0 || 
				m_dTempCycleTime[6] == 0.0 || 
				m_dTempCycleTime[7] == 0.0 || 
				m_dTempCycleTime[8] == 0.0 || 
				m_dTempCycleTime[9] == 0.0 
				)
			{
				m_dCycleTime = (m_dTempCycleTime[0] + 
					m_dTempCycleTime[1] + 
					m_dTempCycleTime[2] + 
					m_dTempCycleTime[3] + 
					m_dTempCycleTime[4] + 
					m_dTempCycleTime[5] + 
					m_dTempCycleTime[6] + 
					m_dTempCycleTime[7] + 
					m_dTempCycleTime[8] + 
					m_dTempCycleTime[9]) / (m_nCycleCounter + 1);
			}
			else
			{
				m_dCycleTime = (m_dTempCycleTime[0] + 
					m_dTempCycleTime[1] + 
					m_dTempCycleTime[2] + 
					m_dTempCycleTime[3] + 
					m_dTempCycleTime[4] + 
					m_dTempCycleTime[5] + 
					m_dTempCycleTime[6] + 
					m_dTempCycleTime[7] + 
					m_dTempCycleTime[8] + 
					m_dTempCycleTime[9]) / 10;
			}
			if (m_nCycleCounter < 9)
			{
				m_nCycleCounter++;
			}
			else
			{
				m_nCycleCounter = 0;
			}
			m_dwStartTime = GetTickCount();
			pAppMod->m_dUPH = (DOUBLE)(pAppMod->m_ulUnitsBonded / (DOUBLE)(GetTickCount() - m_dwMachineTime)) * 60.0 * 60.0 * 1000.0;

			if (
				((m_nLastError = MoveZ(TA2_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) == GMP_SUCCESS)
				)
			{
				TA2_GLASS_Status = ST_EMPTY;
				TA2_Status = ST_IDLE;
				m_qSubOperation = TA2_IDLE_Q;		//0
			}
		}
		break;


	case TA2_STOP_Q:	// 99
		break;
	}

	if (m_qPreviousSubOperation != m_qSubOperation)
	{
		if (HMI_bDebugSeq)
		{
			CString szMsg;
			if (TA2_STOP_Q != m_qSubOperation)
			{
				szMsg.Format("TA2 Go State: %ld", m_qSubOperation);
			}
			else
			{
				szMsg.Format("TA2 Go Stop");
			}
			//DisplayMessage(szMsg);
		}
		UpdateDebugSeq(GetSequenceString(), TA2_Status, TA2_GLASS_Status);
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
			stPrintData.szTitle[GetStationID() * MAX_STATION_SEQ_DATA + 2].Format("%ld", TA2_Status);
			stPrintData.szTitle[GetStationID() * MAX_STATION_SEQ_DATA + 3].Format("%ld", TA2_GLASS_Status);	

			LogFile->log(stPrintData.szTitle);
		}

	}
}

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
INT CTA2::RejectCOF()
{
	INT	nResult	= GMP_SUCCESS;

	SetGLASS1_VacSol(OFF, GMP_WAIT);
	SetWeakBlowSol(ON, GMP_WAIT);
	SetWeakBlowSol(OFF, GMP_WAIT);
	
	return nResult;
}

GlassNum CTA2::AssignPickFromWhere()
{
	static GlassNum lCurGlass;

	return GLASS1;

	//if (
	//	INWH_Status == ST_PICK_READY1_EMPTY2 ||
	//	INWH_Status == ST_PICK_READY1_ERR2 ||
	//	INWH_Status == ST_PICK_ERR1_READY2 ||
	//	INWH_Status == ST_PICK_ERR1_EMPTY2 ||
	//	INWH_Status == ST_PICK_ERR1_ERR2 ||
	//	INWH_Status == ST_PICK_READY1_READY2 
	//   )
	//{
	//	return GLASS1;
	//}
	//else if (
	//		 INWH_Status == ST_PICK_EMPTY1_READY2 ||
	//		 INWH_Status == ST_PICK_EMPTY1_ERR2
	//		)
	//{
	//	return GLASS2;
	//}
	//else// if (INWH_Status == ST_MOD_NOT_SELECTED)
	//{
	//	lCurGlass = (GlassNum)((lCurGlass + 1) % MAX_NUM_OF_GLASS);
	//	return lCurGlass;
	//}
	//Milton
	//Handled in OS
	//else 
	//{
	//	SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
	//	DisplayMessage(__FUNCTION__);
	//	return NO_GLASS;
	//}
}

/////////////////////////////////////////
//	Handshake with FX9000
////////////////////////////////////////



BOOL CTA2::HS_IsFxMachineReady()
{
	BOOL bResult = FALSE;

		bResult = IsHandShakeBitOn(m_stFOUT_IN1);
	
	return bResult;
}


BOOL CTA2::HS_IsFxPickAction()
{
	BOOL bResult = FALSE;

		bResult = IsHandShakeBitOn(m_stFOUT_IN2);
	
	return bResult;
}

BOOL CTA2::HS_IsFxVacOffRequest()
{
	BOOL bResult = FALSE;

		bResult = IsHandShakeBitOn(m_stFOUT_IN3);
	
	return bResult;
}

BOOL CTA2::HS_IsFxPickError()
{
	BOOL bResult = FALSE;

		bResult = IsHandShakeBitOn(m_stFOUT_IN4);
	
	return bResult;
}

LONG CTA2::HMI_ToggleFOutOut1(IPC_CServiceMessage &svMsg)
{
	if (m_stFOUT_OUT1.bHMI_Status)
	{
		SetHandShakeBit(&m_stFOUT_OUT1, OFF);
	}
	else
	{
		SetHandShakeBit(&m_stFOUT_OUT1, ON);
	}
	return 0;
}

LONG CTA2::HMI_ToggleFOutOut2(IPC_CServiceMessage &svMsg)
{
	if (m_stFOUT_OUT2.bHMI_Status)
	{
		SetHandShakeBit(&m_stFOUT_OUT2, OFF);
	}
	else
	{
		SetHandShakeBit(&m_stFOUT_OUT2, ON);
	}
	return 0;
}

LONG CTA2::HMI_ToggleFOutOut3(IPC_CServiceMessage &svMsg)
{
	if (m_stFOUT_OUT3.bHMI_Status)
	{
		SetHandShakeBit(&m_stFOUT_OUT3, OFF);
	}
	else
	{
		SetHandShakeBit(&m_stFOUT_OUT3, ON);
	}
	return 0;
}

LONG CTA2::HMI_ToggleFOutOut4(IPC_CServiceMessage &svMsg)
{
	if (m_stFOUT_OUT4.bHMI_Status)
	{
		SetHandShakeBit(&m_stFOUT_OUT4, OFF);
	}
	else
	{
		SetHandShakeBit(&m_stFOUT_OUT4, ON);
	}
	return 0;
}

LONG CTA2::HMI_ToggleFOutOut5(IPC_CServiceMessage &svMsg)
{
	if (m_stFOUT_OUT5.bHMI_Status)
	{
		SetHandShakeBit(&m_stFOUT_OUT5, OFF);
	}
	else
	{
		SetHandShakeBit(&m_stFOUT_OUT5, ON);
	}
	return 0;
}

LONG CTA2::HMI_ToggleFOutOut6(IPC_CServiceMessage &svMsg)
{
	if (m_stFOUT_OUT6.bHMI_Status)
	{
		SetHandShakeBit(&m_stFOUT_OUT6, OFF);
	}
	else
	{
		SetHandShakeBit(&m_stFOUT_OUT6, ON);
	}
	return 0;
}


VOID CTA2::HS_TA2SetMachineReady(BOOL bState)
{
	SetHandShakeBit(&m_stFOUT_OUT1, bState);
}

VOID CTA2::HS_TA2SetOKSendReady(BOOL bState)
{
	SetHandShakeBit(&m_stFOUT_OUT2, bState);
}

VOID CTA2::HS_TA2SetNGSendReady(BOOL bState)
{
	SetHandShakeBit(&m_stFOUT_OUT3, bState);
}

VOID CTA2::HS_TA2SetVacOffACK(BOOL bState)
{
	SetHandShakeBit(&m_stFOUT_OUT4, bState);
}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
BOOL CTA2::CheckPickStatus()
{
	BOOL bStatus = FALSE;

	switch (PPAction)
	{
	case GLASS1:
		bStatus = IsVacStateOn(GetGlassFPC1_VacSensorOnState());//IsGLASS1_VacSensorOn() && IsLFPC_VacSensorOn();
		break;

	//case GLASS2:
	//	bStatus = IsVacStateOn(GetGlassFPC2_VacSensorOnState());//IsGLASS2_VacSensorOn() && IsRFPC_VacSensorOn() ;
	//	break;
	
	default:
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage(__FUNCTION__);
		bStatus = FALSE;
	}

	return bStatus;
}

BOOL CTA2::SetPickStatus()
{
	BOOL bStatus = TRUE;

	//switch (PPAction)
	//{
	//case GLASS1:
	//	if (
	//		INWH_Status == ST_PICK_READY1_EMPTY2 ||
	//		INWH_Status == ST_PICK_READY1_READY2 ||
	//		INWH_Status == ST_PICK_ERR1_EMPTY2 ||
	//		INWH_Status == ST_PICK_ERR1_READY2 ||
	//		INWH_Status == ST_PICK_ERR1_ERR2 ||
	//		INWH_Status == ST_PICK_READY1_ERR2
	//	   )
	//	{
	//		m_bGlass1Exist = TRUE;
	//		TA2_GLASS_Status[GLASS1] = INWH_GLASS_Status[GLASS1];
	//		INWH_GLASS_Status[GLASS1] = ST_EMPTY;
	//		
	//		INWH_Status = ST_EMPTY;
	//	}
	//	else if (INWH_Status == ST_MOD_NOT_SELECTED)
	//	{
	//		m_bGlass1Exist = TRUE;

	//		TA2_GLASS_Status[GLASS1] = ST_READY;
	//		INWH_GLASS_Status[GLASS1] = ST_EMPTY;

	//	}
	//	else
	//	{
	//		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
	//		DisplayMessage(__FUNCTION__);
	//		bStatus = FALSE;
	//	}		
	//	break;

	//case GLASS2:
	//	if (
	//		INWH_Status == ST_PICK_EMPTY1_READY2 ||
	//		INWH_Status == ST_PICK_EMPTY1_ERR2
	//	   )
	//	{
	//		m_bGlass2Exist = TRUE;
	//		TA2_GLASS_Status[GLASS2] = INWH_GLASS_Status[GLASS2];
	//		INWH_GLASS_Status[GLASS2] = ST_EMPTY;

	//		INWH_Status = ST_EMPTY;

	//	}
	//	else if (INWH_Status == ST_MOD_NOT_SELECTED)
	//	{
	//		m_bGlass2Exist = TRUE;

	//		TA2_GLASS_Status[GLASS2] = ST_READY;
	//		INWH_GLASS_Status[GLASS2] = ST_EMPTY;
	//	}
	//	else
	//	{
	//		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
	//		DisplayMessage(__FUNCTION__);
	//		bStatus = FALSE;
	//	}
	//	break;

	//default:
	//	SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
	//	DisplayMessage(__FUNCTION__);
	//	bStatus = FALSE;
	//	break;

	//}

	return bStatus;
}

BOOL CTA2::IsACFWHReadyLoad()
{
	//if (TA2_GLASS_Status[GLASS1] != ST_EMPTY && TA2_GLASS_Status[GLASS2] != ST_EMPTY) // 11
	//{
	//	PPAction = MAX_NUM_OF_GLASS;

	//	if (ACF_WH_Status[ACF_1] == ST_MOD_NOT_SELECTED && ACF_WH_Status[ACF_2] == ST_MOD_NOT_SELECTED)
	//	{
	//		return TRUE;
	//	}
	//	else if (ACF_WH_Status[ACF_1] == ST_READY_LOAD && ACF_WH_Status[ACF_2] == ST_READY_LOAD)
	//	{
	//		return TRUE;
	//	}
	//}
	//else if (TA2_GLASS_Status[GLASS1] != ST_EMPTY && TA2_GLASS_Status[GLASS2] == ST_EMPTY) // 10
	//{
	//	PPAction = GLASS1;

	//	if (ACF_WH_Status[ACF_1] == ST_MOD_NOT_SELECTED)
	//	{
	//		return TRUE;
	//	}
	//	else if (
	//			 ACF_WH_Status[ACF_1] == ST_READY_LOAD && 
	//			 (ACF_WH_Status[ACF_2] == ST_MOD_NOT_SELECTED || ACF_WH_Status[ACF_2] == ST_READY_LOAD)
	//			)
	//	{
	//		return TRUE;
	//	}
	//} 
	//else if (TA2_GLASS_Status[GLASS1] == ST_EMPTY && TA2_GLASS_Status[GLASS2] != ST_EMPTY) // 01
	//{
	//	PPAction = GLASS2;

	//	if (ACF_WH_Status[ACF_2] == ST_MOD_NOT_SELECTED)
	//	{
	//		return TRUE;
	//	}
	//	else if (
	//			 ACF_WH_Status[ACF_2] == ST_READY_LOAD && 
	//			 (ACF_WH_Status[ACF_1] == ST_MOD_NOT_SELECTED || ACF_WH_Status[ACF_1] == ST_READY_LOAD)
	//			)
	//	{
	//		return TRUE;
	//	}

	//} 
	//else 
	//{
	//	PPAction = NO_GLASS;

	//	SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
	//	DisplayMessage(__FUNCTION__);

	//}
	
	return FALSE;
}

BOOL CTA2::SetPlaceStatus()
{
	BOOL bStatus = TRUE;

//	switch (PPAction)
//	{
//	case GLASS1:
//		if (TA2_GLASS_Status[GLASS1] == ST_READY)
//		{
//			if (ACF_WH_Status[ACF_1] != ST_MOD_NOT_SELECTED)
//			{
//				ACF_WH_Status[ACF_1] = ST_PLACE_READY1_EMPTY2;
//#if 1 //20140529
//				ACF_GLASS_Status[ACFWH_1][GLASS1] = ST_READY;
//				ACF_GLASS_Status[ACFWH_1][GLASS2] = ST_EMPTY;
//#endif
//			}
//		}
//		else
//		{
//			if (ACF_WH_Status[ACF_1] != ST_MOD_NOT_SELECTED)
//			{
//				ACF_WH_Status[ACF_1] = ST_PLACE_ERR1_EMPTY2;
//#if 1 //20140529
//				ACF_GLASS_Status[ACFWH_1][GLASS1] = ST_ERROR;
//				ACF_GLASS_Status[ACFWH_1][GLASS2] = ST_EMPTY;
//#endif
//			}
//		}
//		break;
//
//	case GLASS2:
//		if (TA2_GLASS_Status[GLASS2] == ST_READY)
//		{
//			if (ACF_WH_Status[ACF_2] != ST_MOD_NOT_SELECTED)
//			{
//				ACF_WH_Status[ACF_2] = ST_PLACE_READY1_EMPTY2;
//#if 1 //20140529
//				ACF_GLASS_Status[ACFWH_2][GLASS1] = ST_READY;
//				ACF_GLASS_Status[ACFWH_2][GLASS2] = ST_EMPTY;
//#endif
//			}
//		}
//		else
//		{
//			if (ACF_WH_Status[ACF_2] != ST_MOD_NOT_SELECTED)
//			{
//				ACF_WH_Status[ACF_2] = ST_PLACE_ERR1_EMPTY2;
//#if 1 //20140529
//				ACF_GLASS_Status[ACFWH_2][GLASS1] = ST_ERROR;
//				ACF_GLASS_Status[ACFWH_2][GLASS2] = ST_EMPTY;
//#endif
//			}
//		}
//		break;
//	
//	case MAX_NUM_OF_GLASS:
//		if (TA2_GLASS_Status[GLASS1] == ST_READY)
//		{
//			if (ACF_WH_Status[ACF_1] != ST_MOD_NOT_SELECTED)
//			{
//				ACF_WH_Status[ACF_1] = ST_PLACE_READY1_EMPTY2;
//#if 1 //20140529
//				ACF_GLASS_Status[ACFWH_1][GLASS1] = ST_READY;
//				ACF_GLASS_Status[ACFWH_1][GLASS2] = ST_EMPTY;
//#endif
//			}
//		}
//		else if (TA2_GLASS_Status[GLASS1] == ST_ERROR)
//		{
//			if (ACF_WH_Status[ACF_1] != ST_MOD_NOT_SELECTED)
//			{
//				ACF_WH_Status[ACF_1] = ST_PLACE_ERR1_EMPTY2;
//#if 1 //20140529
//				ACF_GLASS_Status[ACFWH_1][GLASS1] = ST_ERROR;
//				ACF_GLASS_Status[ACFWH_1][GLASS2] = ST_EMPTY;
//#endif
//			}
//		}
//		
//		if (TA2_GLASS_Status[GLASS2] == ST_READY)
//		{
//			if (ACF_WH_Status[ACF_2] != ST_MOD_NOT_SELECTED)
//			{
//				ACF_WH_Status[ACF_2] = ST_PLACE_READY1_EMPTY2;
//#if 1 //20140529
//				ACF_GLASS_Status[ACFWH_2][GLASS1] = ST_READY;
//				ACF_GLASS_Status[ACFWH_2][GLASS2] = ST_EMPTY;
//#endif
//			}
//		}
//		else if (TA2_GLASS_Status[GLASS2] == ST_ERROR)
//		{
//			if (ACF_WH_Status[ACF_2] != ST_MOD_NOT_SELECTED)
//			{
//				ACF_WH_Status[ACF_2] = ST_PLACE_ERR1_EMPTY2;
//#if 1 //20140529
//				ACF_GLASS_Status[ACFWH_2][GLASS1] = ST_ERROR;
//				ACF_GLASS_Status[ACFWH_2][GLASS2] = ST_EMPTY;
//#endif
//			}
//		}
//		break;
//
//	default:
//		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
//		DisplayMessage(__FUNCTION__);
//		bStatus = FALSE;
//
//		if (ACF_WH_Status[ACF_1] != ST_MOD_NOT_SELECTED)
//		{
//			ACF_WH_Status[ACF_1] = ST_PLACE_ERR1_EMPTY2;
//#if 1 //20140529
//			ACF_GLASS_Status[ACFWH_1][GLASS1] = ST_ERROR;
//			ACF_GLASS_Status[ACFWH_1][GLASS2] = ST_EMPTY;
//#endif
//		}
//
//		if (ACF_WH_Status[ACF_2] != ST_MOD_NOT_SELECTED)
//		{
//			ACF_WH_Status[ACF_2] = ST_PLACE_ERR1_EMPTY2;
//#if 1 //20140529
//			ACF_GLASS_Status[ACFWH_2][GLASS1] = ST_ERROR;
//			ACF_GLASS_Status[ACFWH_2][GLASS2] = ST_EMPTY;
//#endif
//		}
//	}

	return bStatus;
}

BOOL CTA2::CheckPlaceStatus()
{
	//switch (PPAction)
	//{
	//case GLASS1:
	//	if (
	//		ACF_WH_Status[ACF_1] == ST_BUSY ||
	//		ACF_WH_Status[ACF_1] == ST_REQ_ACF ||
	//		ACF_WH_Status[ACF_1] == ST_MOD_NOT_SELECTED ||
	//		ACF_WH_Status[ACF_1] == ST_STOP
	//	   )
	//	{
	//		return TRUE;
	//	}
	//	break;

	//case GLASS2:
	//	if (
	//		ACF_WH_Status[ACF_2] == ST_BUSY ||
	//		ACF_WH_Status[ACF_2] == ST_REQ_ACF ||
	//		ACF_WH_Status[ACF_2] == ST_MOD_NOT_SELECTED ||
	//		ACF_WH_Status[ACF_2] == ST_STOP
	//	   )
	//	{
	//		return TRUE;
	//	}
	//	break;

	//case MAX_NUM_OF_GLASS:
	//	if (
	//		(ACF_WH_Status[ACF_1] == ST_BUSY || ACF_WH_Status[ACF_1] == ST_REQ_ACF || ACF_WH_Status[ACF_1] == ST_MOD_NOT_SELECTED || ACF_WH_Status[ACF_1] == ST_STOP) && 
	//		(ACF_WH_Status[ACF_2] == ST_BUSY || ACF_WH_Status[ACF_2] == ST_REQ_ACF || ACF_WH_Status[ACF_2] == ST_MOD_NOT_SELECTED || ACF_WH_Status[ACF_2] == ST_STOP)
	//	   )
	//	{
	//		return TRUE;
	//	}
	//	break;

	//default:
	//	SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
	//	DisplayMessage(__FUNCTION__);
	//	return FALSE;
	//}

	return FALSE;
}

BOOL CTA2::CheckReplaceStatus()
{
	//switch (PPAction)
	//{
	//case GLASS1:
	//	if (ACF_WH_Status[ACF_1] == ST_VAC_ERROR)
	//	{
	//		return TRUE;
	//	}
	//	break;

	//case GLASS2:
	//	if (ACF_WH_Status[ACF_2] == ST_VAC_ERROR)
	//	{
	//		return TRUE;
	//	}
	//	break;

	//case MAX_NUM_OF_GLASS:
	//	if (ACF_WH_Status[ACF_1] == ST_BUSY && ACF_WH_Status[ACF_2] == ST_VAC_ERROR)
	//	{
	//		PPAction = GLASS2;
	//		return TRUE;
	//	}
	//	else if (ACF_WH_Status[ACF_1] == ST_VAC_ERROR && ACF_WH_Status[ACF_2] == ST_BUSY)
	//	{
	//		PPAction = GLASS1;
	//		return TRUE;
	//	}
	//	else if (ACF_WH_Status[ACF_1] == ST_VAC_ERROR && ACF_WH_Status[ACF_2] == ST_VAC_ERROR)
	//	{
	//		PPAction = MAX_NUM_OF_GLASS;
	//		return TRUE;
	//	}
	//	break;

	//default:
	//	SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
	//	DisplayMessage(__FUNCTION__);
	//	return FALSE;
	//}

	return FALSE;
}

BOOL CTA2::ResetPlaceStatus()
{
	//switch (PPAction)
	//{
	//case GLASS1:
	//	TA2_GLASS_Status[GLASS1] = ST_EMPTY;
	//	m_bGlass1Exist = FALSE;
	//	break;

	//case GLASS2:
	//	TA2_GLASS_Status[GLASS2] = ST_EMPTY;
	//	m_bGlass2Exist = FALSE;
	//	break;

	//case MAX_NUM_OF_GLASS:
	//	TA2_GLASS_Status[GLASS1] = ST_EMPTY;
	//	TA2_GLASS_Status[GLASS2] = ST_EMPTY;
	//	m_bGlass1Exist = FALSE;
	//	m_bGlass2Exist = FALSE;
	//	break;

	//default:
	//	SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
	//	DisplayMessage(__FUNCTION__);
	//	return FALSE;
	//}

	return FALSE;
}

BOOL CTA2::IsInStandbyPosn() //20121227
{
	INT nCurrPosn = 0;
	nCurrPosn = m_stMotorX.GetEncPosn();
	if (nCurrPosn < m_lStandbyPosn - 4000 || nCurrPosn > m_lStandbyPosn + 4000) //4000 Cnt = 2mm
	{
		return FALSE;
	}
	return TRUE;
}


INT CTA2::PickGlass(ACFWHUnitNum lACFWHnum) 
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
		DisplayMessage("CTA2::PickGlass : SW Error");
		return GMP_FAIL;
	}

	if (
		((m_nLastError = MoveZ(TA2_Z_CTRL_GO_PICK_LEVEL, GMP_WAIT)) == GMP_SUCCESS)
		)
	{
		lDelay = pCACFxWH->m_stGlass1VacSol.GetOffDelay();
		if (m_stGlass1VacSol.GetOnDelay() > lDelay)
		{
			lDelay = m_stGlass1VacSol.GetOnDelay();
		}
		SetGLASS1_VacSol(ON, GMP_NOWAIT);
		pCACFxWH->SetGLASS1_VacSol(OFF, GMP_NOWAIT);
		Sleep(lDelay);

		if (pCACFxWH->m_stWeakBlowSol.GetOnDelay() > 0)
		{
			pCACFxWH->SetWeakBlowSol(ON, GMP_WAIT);
			pCACFxWH->SetWeakBlowSol(OFF, GMP_NOWAIT);
		}

		if (
			((m_nLastError = MoveZ(TA2_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) == GMP_SUCCESS)
			)
		{
			return GMP_SUCCESS;
		}
	}

	return GMP_FAIL;

}

//INT CTA2::PlaceGlass(GlassNum lGlassNum) // PLACE TO DIFFERENT WH
//{
//	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
//	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
//
//	LONG lDelay;
//
//	SetPlaceToWHVacSol(lGlassNum, ON);
//	
//	if ((m_nLastError = MovePlaceToWHToLoadLevel(lGlassNum, GMP_WAIT)) == GMP_SUCCESS)
//	{
//		lDelay = CalcPlaceToWHDelay(ON, lGlassNum);
//		
//		if (lDelay < m_stGlass1VacSol.GetOffDelay())
//		{
//			lDelay = m_stGlass1VacSol.GetOffDelay();
//		}
//
//		SetVacSol(lGlassNum, OFF);
//		Sleep(lDelay);
//
//		if (m_stWeakBlowSol.GetOnDelay() > 0)
//		{
//			SetWeakBlowSol(ON, GMP_WAIT);
//			SetWeakBlowSol(OFF, GMP_WAIT);
//		}
//
//		return m_nLastError = MovePlaceToWHToStandbyLevel(lGlassNum, GMP_WAIT);
//	}
//
//	return GMP_FAIL;
//}

BOOL CTA2::PickGlassOperation(ACFWHUnitNum lACFWHnum)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CInPickArm *pCInPickArm = (CInPickArm*)m_pModule->GetStation("InPickArm");
	CTA1 *pCTA1= (CTA1*)m_pModule->GetStation("TA1");

	CACFWH *pCACFxWH = NULL;

	CString strMsg;

	BOOL bWHGlassStatus[MAX_NUM_OF_ACFWH] = {FALSE, FALSE};
	BOOL bArmGlassStatus = FALSE;
	BOOL bResult = TRUE;
	ACFWHUnitNum nACFWHnum;
	TA2_X_CTRL_SIGNAL	TA2XAction = TA2_X_CTRL_GO_PICK_ACF1WH_OFFSET;
	
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
		DisplayMessage("CTA2::PlaceGlass : SW Error");
		return FALSE;
	}
	
	if (!m_bModSelected)
	{
		strMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
		return FALSE;
	}

	
	if (!pCACF1WH->m_bModSelected)
	{
		strMsg.Format("%s Module Not Selected. Operation Abort!", pCACF1WH->GetStnName());
		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
		return FALSE;
	}

	if (!pCACF2WH->m_bModSelected)
	{
		strMsg.Format("%s Module Not Selected. Operation Abort!", pCACF2WH->GetStnName());
		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
		return FALSE;
	}

	if (!pCInPickArm->m_bModSelected)
	{
		strMsg.Format("%s Module Not Selected. Operation Abort!", pCInPickArm->GetStnName());
		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
		return FALSE;
	}


	if (!pCTA1->m_bModSelected)
	{
		strMsg.Format("%s Module Not Selected. Operation Abort!", pCTA1->GetStnName());
		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
		return FALSE;
	}


	//if (lGlassNum != NO_GLASS)
	//{
	//	HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Contact Software");
	//	return FALSE;
	//}


		pCACFxWH->SetVacSol(GLASS1, ON);
		SetVacSol(GLASS1, ON);


	Sleep(GetMaxValue(m_stGlass1VacSol.GetOnDelay(), pCACF1WH->m_stGlass1VacSol.GetOnDelay()));

	// Check current glass status

		bWHGlassStatus[ACFWH_1] = pCACF1WH->IsVacSensorOn(GLASS1);
		bWHGlassStatus[ACFWH_2] = pCACF2WH->IsVacSensorOn(GLASS1);
		bArmGlassStatus = IsVacSensorOn(GLASS1);


		if (!bWHGlassStatus[ACFWH_1])
		{
			pCACF1WH->SetVacSol(GLASS1, OFF);
		}
		
		if (!bWHGlassStatus[ACFWH_2])
		{
			pCACF2WH->SetVacSol(GLASS1, OFF);
		}

		if (!bArmGlassStatus)
		{
			SetVacSol(GLASS1, OFF);
		}

		if(bWHGlassStatus[lACFWHnum])
		{
			nACFWHnum = lACFWHnum;
			if (lACFWHnum == ACFWH_1)
			{
				TA2XAction = TA2_X_CTRL_GO_PICK_ACF1WH_OFFSET;
			}
			else
			{
				TA2XAction = TA2_X_CTRL_GO_PICK_ACF2WH_OFFSET;
			}
		}
		else
		{
			strMsg.Format("No FPC on WorkHolder. Operation Abort!");
			HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
			return FALSE;			
		}

	if (
		pCInPickArm->MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		pCInPickArm->MoveX(IN_PICKARM_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT) != GMP_SUCCESS ||
		pCTA1->MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		pCTA1->MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT) != GMP_SUCCESS ||
		pCACF1WH->MoveT(ACF_WH_T_CTRL_GO_STANDBY, GMP_NOWAIT) != GMP_SUCCESS ||
		pCACF2WH->MoveT(ACF_WH_T_CTRL_GO_STANDBY, GMP_NOWAIT) != GMP_SUCCESS ||
		pCACF1WH->MoveY(ACF_WH_Y_CTRL_GO_UNLOAD_POSN_OFFSET, GMP_NOWAIT) != GMP_SUCCESS ||
		pCACF2WH->MoveY(ACF_WH_Y_CTRL_GO_UNLOAD_POSN_OFFSET, GMP_WAIT) != GMP_SUCCESS 
	   )
	{
		return FALSE;
	}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
//	if ((lGlassNum = CalcPickStatus(bWHGlassStatus, bArmGlassStatus)) == NO_GLASS)
//	{
//		strMsg.Format("Invalid Glass Status at %s and %s", GetStnName(), pCINWH->GetStnName());
//		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
//		return FALSE;
//	}
//	
//	MTR_POSN mtrTempDelta[MAX_NUM_OF_GLASS];
//	LONG lDeltaAngleStep[MAX_NUM_OF_GLASS];
//	MTR_POSN mtrTemp[MAX_NUM_OF_GLASS];
//	LONG lStep[MAX_NUM_OF_GLASS];
//	LONG lCurGlass;
//
//	// Calc Unload Posn
//	//Search PR and Return Posn
//	if (!pCINWH->CalcXYToUnloadOffset(lGlassNum, &mtrTempDelta[0], &lDeltaAngleStep[0]))
//	{	
//		return FALSE;
//	}
//	
//	for (INT i = GLASS1; i < MAX_NUM_OF_GLASS; i++)
//	{
//		lStep[i] = pCINWH->m_lStandbyPosnT + lDeltaAngleStep[i] + AngleToCount(pCINWH->m_dUnloadTOffset[i], pCINWH->m_stMotorT.stAttrib.dDistPerCount);
//		//mtrTemp[i].x = m_lPickGlassPosn + mtrTempDelta[i].x /*+ DistanceToCount(pCINWH->m_lUnloadXOffset[i], m_stMotorX.stAttrib.dDistPerCount)*/;
//		mtrTemp[i].y = pCINWH->m_mtrUnloadGlassPosn.y + mtrTempDelta[i].y /*+ DistanceToCount(pCINWH->m_lUnloadYOffset[i], pCINWH->m_stMotorY.stAttrib.dDistPerCount)*/;
//	}
//
//	//mtrTemp[GLASS1].x = m_lPickGlassPosn + mtrTempDelta[GLASS1].x /*+ DistanceToCount(pCACF1WH->m_lLoadXOffset, m_stMotorX.stAttrib.dDistPerCount)*/;
//	mtrTemp[GLASS1].x = m_lPickGlassPosn + mtrTempDelta[GLASS1].x + DistanceToCount(-pCACF1WH->m_dLoadAdjXOffset, m_stMotorX.stAttrib.dDistPerCount); //20121123
//	//mtrTemp[GLASS2].x = m_lPickGlassPosn + mtrTempDelta[GLASS2].x + DistanceToCount(/*pCACF2WH->m_lLoadXOffset + */g_dModuleOffset*1000.0, m_stMotorX.stAttrib.dDistPerCount);
//	mtrTemp[GLASS2].x = m_lPickGlassPosn + mtrTempDelta[GLASS2].x + DistanceToCount(/*pCACF2WH->m_lLoadXOffset + */g_dModuleOffset * 1000.0 - pCACF2WH->m_dLoadAdjXOffset, m_stMotorX.stAttrib.dDistPerCount); //20121005
//
//	// Move Safety Level First
//	if (
//		pCINWH->MoveZToStandbyLevel(MAX_NUM_OF_GLASS, GMP_WAIT) != GMP_SUCCESS ||
//		MovePlaceToWHToStandbyLevel(MAX_NUM_OF_GLASS , GMP_WAIT) != GMP_SUCCESS
//	   )
//	{
//		return FALSE;
//	}
//
//	if (lGlassNum == GLASS1 || lGlassNum == MAX_NUM_OF_GLASS)
//	{
//		lCurGlass = GLASS1;
//	}
//	else 
//	{
//		lCurGlass = GLASS2;
//	}
//

	//if (
	//	pCINWH->m_stMotorY.MoveAbsolute(mtrTemp[lCurGlass].y, GMP_WAIT) != GMP_SUCCESS ||
	//	pCINWH->m_stMotorT.MoveAbsolute(lStep[lCurGlass], GMP_WAIT) != GMP_SUCCESS ||
	//	m_stMotorX.MoveAbsolute(mtrTemp[lCurGlass].x, GMP_WAIT) != GMP_SUCCESS ||
	//	pCINWH->m_stMotorY.Sync() != GMP_SUCCESS ||
	//	pCINWH->m_stMotorT.Sync() != GMP_SUCCESS ||
	//	m_stMotorX.Sync() != GMP_SUCCESS ||
	//	PickGlass((GlassNum)lCurGlass) != GMP_SUCCESS
	//   )
	//{
	//	return FALSE;
	//}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

	//TA2 Pick Action
	if (
		MoveZ(TA2_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		MoveTPickPosn(GMP_WAIT) != GMP_SUCCESS ||
		MoveX(TA2XAction, GMP_WAIT) != GMP_SUCCESS ||
		PickGlass(nACFWHnum) != GMP_SUCCESS
	   )
	{
		return FALSE;
	}

	//Check TA2 Glass Status
		if (!IsVacStateOn(GetGlassFPC1_VacSensorOnState()))//!IsGLASS1_VacSensorOn())
		{
			if (nACFWHnum == ACFWH_1)
			{
				pCACF1WH->SetGLASS1_VacSol(ON, GMP_WAIT);
			}
			else
			{
				pCACF2WH->SetGLASS1_VacSol(ON, GMP_WAIT);
			}
			strMsg.Format("Pick Glass Error: %s Vac Sensor Not On.", GetStnName());
			HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
			bResult = FALSE;
		}
	


	if (bResult)
	{
		if (MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS)
		{
			bResult = FALSE;
		}
	}

	return bResult;
}

BOOL CTA2::PlaceGlassOperation()	//Flip (Face UP) the Glass for Fx9000 Picking.
{
	CString strMsg;

	BOOL bResult = TRUE;
	
	if (!m_bModSelected)
	{
		strMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", strMsg);
		return FALSE;
	}
	
	//TA2 T Unload
	if (
		MoveZ(TA2_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		MoveX(TA2_X_CTRL_GO_PLACE_GLASS_POSN, GMP_WAIT) != GMP_SUCCESS ||
		MoveTPlacePosn(GMP_WAIT) != GMP_SUCCESS ||
		MoveZ(TA2_Z_CTRL_GO_LOWER_LEVEL, GMP_WAIT) != GMP_SUCCESS
	   )
	{
		return FALSE;
	}

	return bResult;
}

//BOOL CTA2::PlaceGlassOperation(GlassNum lGlassNum)
//{
//	CINWH *pCINWH = (CINWH*)m_pModule->GetStation("INWH");
//	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
//	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
////	CTA3 *pCTA3	= (CTA3*)m_pModule->GetStation("TA3");
//
//	CString szMsg;
//	BOOL bWHGlassStatus[MAX_NUM_OF_GLASS] = {FALSE};
//	BOOL bArmGlassStatus[MAX_NUM_OF_GLASS] = {FALSE};
//	BOOL bResult = TRUE;
//
////	if (!m_bModSelected)
////	{
////		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
////		HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", szMsg);
////		return FALSE;
////	}
////
////	if (!pCINWH->m_bModSelected)
////	{
////		szMsg.Format("%s Module Not Selected. Operation Abort!", pCINWH->GetStnName());
////		HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", szMsg);
////		return FALSE;
////	}
////
////	if (!pCACF1WH->m_bModSelected)
////	{
////		szMsg.Format("%s Module Not Selected. Operation Abort!", pCACF1WH->GetStnName());
////		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", szMsg);
////		return FALSE;
////	}
////
////	if (!pCACF2WH->m_bModSelected)
////	{
////		szMsg.Format("%s Module Not Selected. Operation Abort!", pCACF2WH->GetStnName());
////		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", szMsg);
////		return FALSE;
////	}
////	
////	if (lGlassNum != NO_GLASS)
////	{
////		HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", "Contact Software");
////		return FALSE;
////	}
////
////	for (INT i = GLASS1; i < MAX_NUM_OF_GLASS; i++)
////	{
////		SetVacSol((GlassNum)i, ON);
////		SetPlaceToWHVacSol((GlassNum)i, ON);
////	}
////
////	Sleep(GetMaxValue(m_stGlass1VacSol.GetOnDelay(), pCACF1WH->m_stGlass1VacSol.GetOnDelay()));
////
////	// Check current glass status
////	for (INT i = GLASS1; i < MAX_NUM_OF_GLASS; i++)
////	{
////		//SetVacSol((GlassNum)i, ON, GMP_WAIT);
////		//SetPlaceToWHVacSol(i, ON, GMP_WAIT);
////
////		bArmGlassStatus[i] = IsVacSensorOn((GlassNum)i);
////		bWHGlassStatus[i] = IsPlaceToWHVacSensorOn(i);
////
////		if (!IsVacSensorOn((GlassNum)i))
////		{
////			SetVacSol((GlassNum)i, OFF);
////		}
////		
////		if (!IsPlaceToWHVacSensorOn(i))
////		{
////			SetPlaceToWHVacSol(i, OFF);
////		}
////	}
////
////	if ((lGlassNum = CalcPlaceStatus(bWHGlassStatus, bArmGlassStatus)) == NO_GLASS)
////	{
////		szMsg.Format("Invalid Glass Status at %s and %s", GetStnName(), pCINWH->GetStnName());
////		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", szMsg);
////		return FALSE;
////	}
////
////	if (
////		pCTA3->MoveX(TA3_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS ||
////		pCINWH->MoveZToStandbyLevel(MAX_NUM_OF_GLASS, GMP_WAIT) != GMP_SUCCESS ||
////		MovePlaceToWHToStandbyLevel(MAX_NUM_OF_GLASS , GMP_WAIT) != GMP_SUCCESS ||
////		MovePlaceToWHToLoadPosn(lGlassNum, GMP_WAIT) != GMP_SUCCESS ||
////		MoveX(TA2_X_CTRL_GO_PLACE_GLASS_POSN, GMP_WAIT) != GMP_SUCCESS ||
////		PlaceGlass(lGlassNum) != GMP_SUCCESS
////	   )
////	{
////		return FALSE;
////	}
////
////	Sleep(max(pCACF1WH->m_stAnvilVacSol.GetOnDelay(), pCACF2WH->m_stAnvilVacSol.GetOnDelay())); //20170630
////
////	//Check Glass Status
////	if (lGlassNum == GLASS1 || lGlassNum == MAX_NUM_OF_GLASS)
////	{
////		if (!pCACF1WH->IsGLASS1_VacSensorOn())
////		{
////			pCACF1WH->SetAnvilVacSol(ON, GMP_NOWAIT);
////			SetGLASS1_VacSol(ON, GMP_WAIT);
////			szMsg.Format("Place Glass Error: %s Glass1 Vac Sensor Not On.", pCACF1WH->GetStnName());
////			HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", szMsg);
////			bResult = FALSE;
////		}
////		else if (!pCACF1WH->IsAnvilVacSensorOn()) //20170628
////		{
////			pCACF1WH->SetAnvilVacSol(ON, GMP_NOWAIT);
////			SetGLASS1_VacSol(ON, GMP_WAIT);
////			szMsg.Format("Place Glass Error: %s Glass1 Anvil Vac Sensor Not On.", pCACF1WH->GetStnName());
////			HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", szMsg);
////			bResult = FALSE;
////		}
////	}	
////
////	if (lGlassNum == GLASS2 || lGlassNum == MAX_NUM_OF_GLASS)
////	{
////		if (!pCACF2WH->IsGLASS1_VacSensorOn())
////		{
////			pCACF2WH->SetAnvilVacSol(ON, GMP_NOWAIT);
////			SetGLASS2_VacSol(ON, GMP_WAIT);
////			szMsg.Format("Place Glass Error: %s Glass1 Vac Sensor Not On.", pCACF2WH->GetStnName());
////			HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", szMsg);
////			bResult = FALSE;
////		}
////		else if (!pCACF2WH->IsAnvilVacSensorOn()) //20170628
////		{
////			pCACF2WH->SetAnvilVacSol(ON, GMP_NOWAIT);
////			SetGLASS2_VacSol(ON, GMP_WAIT);
////			szMsg.Format("Place Glass Error: %s Glass1 Anvil Vac Sensor Not On.", pCACF2WH->GetStnName());
////			HMIMessageBox(MSG_OK, "PLACE GLASS OPERATION", szMsg);
////			bResult = FALSE;
////		}
////	}	
////#if 1 //debug only 20121018
////
////	if (bResult)
////	{
////		if (MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS)
////		{
////			bResult = FALSE;
////		}
////	}
////#endif
//	return bResult;
//}

BOOL CTA2::IsPlaceToWHVacSensorOn(LONG lGlassNum)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	//if (lGlassNum == GLASS1)
	//{
	//	return pCACF1WH->IsGLASS1_VacSensorOn();
	//}
	//else if (lGlassNum == GLASS2)
	//{
	//	return pCACF2WH->IsGLASS1_VacSensorOn();
	//}
	//else
	//{
	//	SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
	//	return FALSE;
	//}
	return FALSE;
}


BOOL CTA2::IsErrorCOF()
{
	if(TA2_GLASS_Status == ST_ERROR)
	{
		return TRUE;
	}
	return FALSE;
}


INT CTA2::SetPlaceToWHVacSol(LONG lGlassNum, BOOL bMode, BOOL bWait)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	//LONG lDelay;

	//if (lGlassNum == GLASS1)
	//{
	//	if (
	//		pCACF1WH->SetAnvilVacSol(bMode, GMP_NOWAIT) == GMP_SUCCESS &&
	//		pCACF1WH->SetGLASS1_VacSol(bMode, bWait)  == GMP_SUCCESS
	//		)
	//	{
	//		return GMP_SUCCESS;
	//	}
	//	else
	//	{
	//		return GMP_FAIL;
	//	}
	//}
	//else if (lGlassNum == GLASS2)
	//{
	//	if (
	//		pCACF2WH->SetAnvilVacSol(bMode, GMP_NOWAIT) == GMP_SUCCESS &&
	//		pCACF2WH->SetGLASS1_VacSol(bMode, bWait) == GMP_SUCCESS
	//	)
	//	{
	//		return GMP_SUCCESS;
	//	}
	//	else
	//	{
	//		return GMP_FAIL;
	//	}
	//}
	//else
	//{
	//	if (
	//		pCACF1WH->SetAnvilVacSol(bMode, GMP_NOWAIT) != GMP_SUCCESS ||
	//		pCACF2WH->SetAnvilVacSol(bMode, GMP_NOWAIT) != GMP_SUCCESS ||
	//		pCACF1WH->SetGLASS1_VacSol(bMode, GMP_NOWAIT) != GMP_SUCCESS ||
	//		pCACF2WH->SetGLASS1_VacSol(bMode, GMP_NOWAIT) != GMP_SUCCESS
	//	   )
	//	{
	//		return GMP_FAIL;
	//	}
	//	if (bWait == GMP_WAIT)
	//	{
	//		if (bMode)
	//		{
	//			lDelay = pCACF1WH->m_stGlass1VacSol.GetOnDelay();
	//			if (lDelay < pCACF2WH->m_stGlass1VacSol.GetOnDelay())
	//			{
	//				lDelay = pCACF2WH->m_stGlass1VacSol.GetOnDelay();
	//			}
	//		}
	//		else
	//		{
	//			lDelay = pCACF1WH->m_stGlass1VacSol.GetOffDelay();
	//			if (lDelay < pCACF2WH->m_stGlass1VacSol.GetOffDelay())
	//			{
	//				lDelay = pCACF2WH->m_stGlass1VacSol.GetOffDelay();
	//			}
	//		}
	//		
	//		Sleep(lDelay);

	//		return GMP_SUCCESS;
	//	}
	//}

	return GMP_SUCCESS;
}

LONG CTA2::CalcPlaceToWHDelay(BOOL bMode, LONG lGlassNum)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	LONG lDelay = 0;

	//if (bMode)
	//{
	//	if (lGlassNum == GLASS1)
	//	{
	//		lDelay = pCACF1WH->m_stGlass1VacSol.GetOnDelay();
	//	}
	//	else if (lGlassNum == GLASS2)
	//	{
	//		lDelay = pCACF2WH->m_stGlass1VacSol.GetOnDelay();
	//	}
	//	else
	//	{
	//		lDelay = pCACF1WH->m_stGlass1VacSol.GetOnDelay();
	//		if (lDelay < pCACF2WH->m_stGlass1VacSol.GetOnDelay())
	//		{
	//			lDelay = pCACF2WH->m_stGlass1VacSol.GetOnDelay();
	//		}
	//	}
	//}
	//else
	//{
	//	if (lGlassNum == GLASS1)
	//	{
	//		lDelay = pCACF1WH->m_stGlass1VacSol.GetOffDelay();
	//	}
	//	else if (lGlassNum == GLASS2)
	//	{
	//		lDelay = pCACF2WH->m_stGlass1VacSol.GetOffDelay();
	//	}
	//	else
	//	{
	//		lDelay = pCACF1WH->m_stGlass1VacSol.GetOffDelay();
	//		if (lDelay < pCACF2WH->m_stGlass1VacSol.GetOffDelay())
	//		{
	//			lDelay = pCACF2WH->m_stGlass1VacSol.GetOffDelay();
	//		}
	//	}
	//}

	return lDelay;

}
/////////////////////////////////////////////////////////////////
//Motor Related
/////////////////////////////////////////////////////////////////
INT CTA2::MoveX(TA2_X_CTRL_SIGNAL ctrl, BOOL bWait)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	INT		nResult		= GMP_SUCCESS;

	LONG lTempPosn = 0;

	CString szDebug = _T("");
	switch (ctrl)
	{
	case TA2_X_CTRL_GO_STANDBY_POSN:
		lTempPosn = m_lStandbyPosn;
		break;

	case TA2_X_CTRL_GO_PLACE_GLASS_POSN:
		lTempPosn = m_lPlaceGlassPosn;
		break;

	case TA2_X_CTRL_GO_PLACE_GLASS_OFFSET:
		lTempPosn = m_lPlaceGlassPosn + m_lPlaceGlassOffset;
		break;

	case TA2_X_CTRL_GO_PICK_ACF1WH_POSN:
		lTempPosn = m_lPickGlassPosn;
		break;

	case TA2_X_CTRL_GO_PICK_ACF2WH_POSN:
		lTempPosn = m_lPickGlassPosn - DistanceToCount(g_dGToGOffset * 1000, m_stMotorX.stAttrib.dDistPerCount);
		break;

	case TA2_X_CTRL_GO_PICK_ACF1WH_OFFSET: //20121011
		lTempPosn = m_lPickGlassPosn + m_lPickGlass1Offset;
		break;

	case TA2_X_CTRL_GO_PICK_ACF2WH_OFFSET: //20121011
		lTempPosn = m_lPickGlassPosn - DistanceToCount(g_dGToGOffset * 1000, m_stMotorX.stAttrib.dDistPerCount) + m_lPickGlass2Offset;
		break;
	}

	if (HMI_bDebugCal)	// 20140801 Yip: Only Show Message When Debug Calculation
	{
		szDebug.Format("MotorX Posn: %d", lTempPosn);
		DisplayMessage(szDebug);
	}
	return m_stMotorX.MoveAbsolute(lTempPosn, bWait);
}


//INT CTA2::MoveX(TA2_X_CTRL_SIGNAL ctrl, BOOL bWait)
//{
//	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
//	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
//
//	INT		nResult		= GMP_SUCCESS;
//
//	LONG lTempPosn = 0;
//
//	switch (ctrl)
//	{
//	case TA2_X_CTRL_GO_STANDBY_POSN:
//		lTempPosn = m_lStandbyPosn;
//		break;
//
//	case TA2_X_CTRL_GO_PICK_GLASS_POSN:
//		//lTempPosn = m_lPickGlassPosn /*+ DistanceToCount(pCACF1WH->m_lLoadXOffset, m_stMotorX.stAttrib.dDistPerCount)*/;
//		lTempPosn = m_lPickGlassPosn + DistanceToCount(-pCACF1WH->m_dLoadAdjXOffset, m_stMotorX.stAttrib.dDistPerCount); //20121123
//		break;
//
//	case TA2_X_CTRL_GO_PICK_GLASS2_POSN:
//		//lTempPosn = m_lPickGlassPosn +  DistanceToCount(/*pCACF2WH->m_lLoadXOffset + */g_dModuleOffset * 1000.0, m_stMotorX.stAttrib.dDistPerCount);
//		lTempPosn = m_lPickGlassPosn + DistanceToCount(/*pCACF2WH->m_lLoadXOffset + */g_dModuleOffset * 1000.0 - pCACF2WH->m_dLoadAdjXOffset, m_stMotorX.stAttrib.dDistPerCount);  //20121005
//		break;
//
//	case TA2_X_CTRL_GO_PLACE_GLASS_POSN:
//		lTempPosn = m_lPlaceGlassPosn + DistanceToCount(pCACF1WH->m_lLoadXOffset, m_stMotorX.stAttrib.dDistPerCount);
//		//lTempPosn = m_lPlaceGlassPosn + DistanceToCount(pCACF1WH->m_lLoadXOffset + pCACF1WH->m_dLoadAdjXOffset, m_stMotorX.stAttrib.dDistPerCount); //20121005
//		break;
//
//	case TA2_X_CTRL_GO_PLACE_GLASS_POSN_NO_OFFSET:
//		lTempPosn = m_lPlaceGlassPosn;
//		break;
//	}
//
//	return m_stMotorX.MoveAbsolute(lTempPosn, GMP_WAIT);
//}

INT CTA2::MoveZ(TA2_Z_CTRL_SIGNAL ctrl, BOOL bWait)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	INT		nResult		= GMP_SUCCESS;
	DOUBLE dDistPerCnt = 0.0;
	DOUBLE dGap = 0.0;
	LONG lTempPosn = 0;

	dDistPerCnt	= m_stMotorZ.stAttrib.dDistPerCount;

	switch (ctrl)
	{
	case TA2_Z_CTRL_GO_STANDBY_LEVEL:
		lTempPosn = m_lStandbyLevel;
		break;

	case TA2_Z_CTRL_GO_PICK_LEVEL:
		if (IsBurnInDiagBond())
		{
			dGap = DistanceToCount(GAP_THICKNESS * FPC_GAP * 1000.0, dDistPerCnt); 
		}
		else
		{
			dGap = 0.0;
		}

		lTempPosn = m_lPickLevel + (LONG)dGap;
		break;

	case TA2_Z_CTRL_GO_LOWER_LEVEL:
		if (IsBurnInDiagBond())
		{
			dGap = DistanceToCount(GAP_THICKNESS * FPC_GAP * 1000.0, dDistPerCnt); 
		}
		else
		{
			dGap = 0.0;
		}

		lTempPosn = m_lPlaceLevel - (LONG)dGap;
		break;

	}

	return m_stMotorZ.MoveAbsolute(lTempPosn, GMP_WAIT);
}

INT CTA2::MovePlaceToWHToStandbyLevel(LONG lGlass, BOOL bWait)
{
	//CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	//CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	//if (lGlass == GLASS1)
	//{
	//	return pCACF1WH->MoveZToStandbyLevel(GLASS1, bWait);
	//}
	//else if (lGlass == GLASS2)
	//{
	//	return pCACF2WH->MoveZToStandbyLevel(GLASS1, bWait);
	//}
	//else if (lGlass == MAX_NUM_OF_GLASS)
	//{
	//	if (
	//		pCACF1WH->MoveZToStandbyLevel(GLASS1, GMP_NOWAIT) != GMP_SUCCESS ||
	//		pCACF2WH->MoveZToStandbyLevel(GLASS1, GMP_NOWAIT) != GMP_SUCCESS	
	//	   )
	//	{
	//		return GMP_FAIL;
	//	}
	//	else
	//	{
	//		if (bWait == GMP_WAIT)
	//		{
	//			if (
	//				pCACF1WH->m_stMotorZ[WHZ_1].Sync() != GMP_SUCCESS ||
	//				pCACF2WH->m_stMotorZ[WHZ_1].Sync() != GMP_SUCCESS
	//			   )
	//			{
	//				return GMP_FAIL;
	//			}
	//		}
	//	}
	//	return GMP_SUCCESS;
	//}

	return GMP_FAIL;
}

INT CTA2::MoveTPickPosn(BOOL bWait)
{
	return SetRotarySol(ON, bWait);		//ON = Pick, LOAD,  OFF = Place, Unload
}

INT CTA2::MoveTPlacePosn(BOOL bWait)
{
	return SetRotarySol(OFF, bWait);	//ON = Pick, LOAD,  OFF = Place, Unload
}

INT CTA2::MovePlaceToWHToLoadLevel(LONG lGlass, BOOL bWait)
{
	//CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	//CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	//if (lGlass == GLASS1)
	//{
	//	//Sleep(3000); //20120914 debug only
	//	return pCACF1WH->MoveZToLoadLevel(WHZ_1, bWait);
	//}
	//else if (lGlass == GLASS2)
	//{
	//	return pCACF2WH->MoveZToLoadLevel(WHZ_1, bWait); //MoveZ(ACF_WH_Z_CTRL_GO_LOAD_LEVEL, GMP_NOWAIT);
	//}
	//else if (lGlass == MAX_NUM_OF_GLASS)
	//{
	//	if (
	//		pCACF1WH->MoveZToLoadLevel(WHZ_1, GMP_NOWAIT) != GMP_SUCCESS ||
	//		pCACF2WH->MoveZ(ACF_WH_Z_CTRL_GO_LOAD_LEVEL, GMP_NOWAIT) != GMP_SUCCESS	
	//	   )
	//	{
	//		return GMP_FAIL;
	//	}
	//	else
	//	{
	//		if (bWait == GMP_WAIT)
	//		{
	//			if (
	//				pCACF1WH->m_stMotorZ[WHZ_1].Sync() != GMP_SUCCESS ||
	//				pCACF2WH->m_stMotorZ[WHZ_1].Sync() != GMP_SUCCESS
	//			   )
	//			{
	//				return GMP_FAIL;
	//			}
	//		}
	//	}
	//	return GMP_SUCCESS;
	//}

	return GMP_FAIL;
}

INT CTA2::MovePlaceToWHToLoadPosn(LONG lGlass, BOOL bWait)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	//if (lGlass == GLASS1)
	//{
	//	return pCACF1WH->MoveXYToLoadPosn(bWait);
	//}
	//else if (lGlass == GLASS2)
	//{
	//	return pCACF2WH->MoveXYToLoadPosn(bWait);
	//}
	//else if (lGlass == MAX_NUM_OF_GLASS)
	//{
	//	if (
	//		pCACF1WH->MoveXYToLoadPosn(GMP_NOWAIT) != GMP_SUCCESS ||
	//		pCACF2WH->MoveXYToLoadPosn(GMP_NOWAIT) != GMP_SUCCESS	
	//	   )
	//	{
	//		return GMP_FAIL;
	//	}
	//	else
	//	{
	//		if (bWait == GMP_WAIT)
	//		{
	//			if (
	//				pCACF1WH->m_stMotorY.Sync() != GMP_SUCCESS ||
	//				pCACF2WH->m_stMotorY.Sync() != GMP_SUCCESS
	//			   )
	//			{
	//				return GMP_FAIL;
	//			}
	//		}
	//	}
	//	return GMP_SUCCESS;
	//}

	return GMP_FAIL;
}

INT CTA2::MoveToPickPosn(LONG lGlass, BOOL bWait)
{
	CINWH *pCINWH = (CINWH*)m_pModule->GetStation("INWH");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	LONG lTemp = m_lPickGlassPosn;

	switch (lGlass)
	{
	case GLASS1:
		//lTemp = m_lPickGlassPosn + pCINWH->m_mtrCurrAlignDelta[GLASS1].x;
		//lTemp = m_lPickGlassPosn + pCINWH->m_mtrCurrAlignDelta[GLASS1].x + DistanceToCount(-pCACF1WH->m_dLoadAdjXOffset, m_stMotorX.stAttrib.dDistPerCount); //20121123
		break;

	//case GLASS2:
	//	//lTemp = m_lPickGlassPosn + pCINWH->m_mtrCurrAlignDelta[GLASS2].x + DistanceToCount(g_dModuleOffset*1000.0, m_stMotorX.stAttrib.dDistPerCount);
	//	//lTemp = m_lPickGlassPosn + pCINWH->m_mtrCurrAlignDelta[GLASS2].x + DistanceToCount(g_dModuleOffset * 1000.0 - pCACF2WH->m_dLoadAdjXOffset, m_stMotorX.stAttrib.dDistPerCount); //20121005
	//	break;
	}

	return m_stMotorX.MoveAbsolute(lTemp, bWait);
}

/////////////////////////////////////////////////////////////////
//I/O Related
/////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////
//SETUP Related
/////////////////////////////////////////////////////////////////
VOID CTA2::ModuleSetupPreTask()
{
	m_bCheckMotor		= TRUE;
	m_bCheckIO			= TRUE;
	

	CheckModSelected(TRUE);
	
	SetDiagnSteps(g_lDiagnSteps);

	UpdateModuleSetupLevel();
	UpdateModuleSetupPosn();
	UpdateModuleSetupPara();
}

VOID CTA2::ModuleSetupPostTask()
{
	m_bCheckMotor		= FALSE;
	m_bCheckIO			= FALSE;

}

BOOL CTA2::ModuleSetupSetPosn(LONG lSetupTitle)
{
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");
	BOOL		bResult	= TRUE;
	MTR_POSN	mtrOld;
	mtrOld.x	= 0;
	mtrOld.y	= 0;
	MTR_POSN TempPosn;
	TempPosn.x	= 0;
	TempPosn.y	= 0;
	BOOL		bIsXY = FALSE;

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
		mtrOld.x			= m_lPickGlass1Offset;

		m_lPickGlass1Offset	= TempPosn.x - m_lPickGlassPosn;

		TempPosn.x			= m_lPickGlass1Offset;
		break;

	case 3:
		mtrOld.x			= m_lPlaceGlassPosn;

		m_lPlaceGlassPosn	= TempPosn.x;
		break;

	case 4:
		mtrOld.x			= m_lPlaceGlassOffset;

		m_lPlaceGlassOffset	= TempPosn.x - m_lPlaceGlassPosn;

		TempPosn.x			= m_lPlaceGlassOffset;
		break;

	case 5:
		//mtrOld.x			= m_lPlaceGlassPosn;

		//m_lPlaceGlassPosn	= TempPosn.x;
		break;

	case 6:
		mtrOld.x			= m_lPickGlass2Offset;

		m_lPickGlass2Offset	= TempPosn.x - m_lPickGlassPosn + DistanceToCount(g_dGToGOffset * 1000, m_stMotorX.stAttrib.dDistPerCount);

		TempPosn.x			= m_lPickGlass2Offset;
		break;
	}

	if (bIsXY)
	{
		CAC9000Stn::ModuleSetupSetPosn(lSetupTitle, mtrOld, TempPosn);
	}
	else
	{
		CAC9000Stn::ModuleSetupSetPosn(lSetupTitle, "X", mtrOld.x, TempPosn.x);
	}
	return TRUE;
}

BOOL CTA2::ModuleSetupGoPosn(LONG lSetupTitle)
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
			MoveZ(TA2_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS 
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
		nResult = SetupGoPlaceGlassPosn();
		break;

	case 4:
		nResult = SetupGoPlaceGlassOffset();
		break;

	case 5:
		nResult = SetupGoPickGlass2Posn();
		break;

	case 6:
		nResult = SetupGoPickGlass2Offset();
		break;
	}
	
	return nResult;
}

BOOL CTA2::ModuleSetupGoLevel(LONG lSetupTitle)
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
	}

	if (nResult != GMP_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CTA2::ModuleSetupSetLevel(LONG lSetupTitle)
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
	}

	CAC9000Stn::ModuleSetupSetLevel(lSetupTitle, lOldLevel, lTempLevel);

	return bResult;
}

BOOL CTA2::ModuleSetupSetPara(PARA stPara)
{
	switch (stPara.lSetupTitle)
	{
	case 0:
		m_stGlass1VacSol.SetOnDelay(stPara.lPara);
		m_stGlass2VacSol.SetOnDelay(stPara.lPara);
		break;

	case 1:
		m_stGlass1VacSol.SetOffDelay(stPara.lPara);
		m_stGlass2VacSol.SetOffDelay(stPara.lPara);
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
		//m_stLFPCVacSol.SetOnDelay(stPara.lPara);
		//m_stRFPCVacSol.SetOnDelay(stPara.lPara);
		break;

	case 6:
		m_stRotarySol.SetOffDelay(stPara.lPara);
		//m_stLFPCVacSol.SetOffDelay(stPara.lPara);
		//m_stRFPCVacSol.SetOffDelay(stPara.lPara);
		break;
	}

	return CAC9000Stn::ModuleSetupSetPara(stPara);
}

VOID CTA2::UpdateModuleSetupLevel()
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
	g_stSetupLevel[i].lType			= DEVICE_PARA;
	g_stSetupLevel[i].lLevelZ		= m_lPickLevel;

	i++;	// 3
	g_stSetupLevel[i].szLevel.Format("Place Level");
	g_stSetupLevel[i].bEnable		= TRUE;
	g_stSetupLevel[i].bVisible		= TRUE;
	g_stSetupLevel[i].lType			= DEVICE_PARA;
	g_stSetupLevel[i].lLevelZ		= m_lPlaceLevel;

	i++;	// 4
	g_stSetupLevel[i].szLevel.Format("Lower Level");
	g_stSetupLevel[i].bEnable		= TRUE;
	g_stSetupLevel[i].bVisible		= TRUE;
	g_stSetupLevel[i].lType			= DEVICE_PARA;
	g_stSetupLevel[i].lLevelZ		= m_lLowerLevel;

	for (j = i + 1; j < NUM_OF_LEVEL_Z; j++)
	{
		g_stSetupLevel[j].szLevel.Format("Reserved");
		g_stSetupLevel[j].bEnable		= FALSE;
		g_stSetupLevel[j].bVisible		= FALSE;
		g_stSetupLevel[j].lType			= MACHINE_PARA;
		g_stSetupLevel[j].lLevelZ		= 0;
	}
}

VOID CTA2::UpdateModuleSetupPosn()
{	
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
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
	g_stSetupPosn[i].szPosn.Format("Pick Glass 1 Posn");	
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPickGlassPosn;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 2
	g_stSetupPosn[i].szPosn.Format("Pick Glass 1 Offset");	
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= DEVICE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPickGlass1Offset;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 3
	g_stSetupPosn[i].szPosn.Format("Place Glass Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPlaceGlassPosn;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 4
	g_stSetupPosn[i].szPosn.Format("Place Glass Offset");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= DEVICE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPlaceGlassOffset;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 5
	g_stSetupPosn[i].szPosn.Format("Pick Glass 2 Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPickGlassPosn - DistanceToCount(g_dGToGOffset * 1000, m_stMotorX.stAttrib.dDistPerCount);
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 6
	g_stSetupPosn[i].szPosn.Format("Pick Glass 2 Offset");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= DEVICE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lPickGlass2Offset;
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

VOID CTA2::UpdateModuleSetupPara()
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

INT CTA2::SetupGoStandbyLevel()
{
	INT nResult = GMP_SUCCESS;

	nResult = MoveZ(TA2_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT);	

	return nResult;
}

INT CTA2::SetupGoPickLevel()
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

	//if (!pCInPickArm->m_bModSelected)
	//{
	//	szMsg.Format("%s Module Not Selected. Operation Abort!", pCInPickArm->GetStnName());
	//	HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", szMsg);
	//	return GMP_FAIL;
	//}

	if (
		MoveZ(TA2_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS /*||
		SetRotarySol(OFF, GMP_WAIT) != GMP_SUCCESS*/
		)
	{
		return GMP_FAIL;
	}

	//lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "Move InPickArm to Place Posn?");
	//if (lReply == BUTTON_1)	// YES
	//{
	//	if (
	//		pCInPickArm->MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
	//		pCInPickArm->SetRotarySol(ON, GMP_WAIT) != GMP_SUCCESS ||
	//		MoveX(TA2_X_CTRL_GO_PICK_GLASS_POSN, GMP_WAIT) != GMP_SUCCESS
	//		) 
	//	{
	//		return GMP_FAIL;
	//	}
	//}
	//else if (lReply == BUTTON_3)
	//{
	//	return GMP_FAIL;
	//}
	
	//lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "Move TA2 to Pick Level?");
	//if (lReply == BUTTON_1)	// YES
	{
		if (MoveZ(TA2_Z_CTRL_GO_PICK_LEVEL, GMP_WAIT) != GMP_SUCCESS)
		{
			return GMP_FAIL;
		}
	}
	//else if (lReply == BUTTON_3)
	//{
	//	return GMP_FAIL;
	//}	

	return nResult;

}

INT CTA2::SetupGoLowerLevel()
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
		if (MoveZ(TA2_Z_CTRL_GO_LOWER_LEVEL, GMP_WAIT) != GMP_SUCCESS)
		{
			return GMP_FAIL;
		}
	}

	return nResult;

}

INT CTA2::SetupGoPlaceLevel()
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

	//if (!pCInPickArm->m_bModSelected)
	//{
	//	szMsg.Format("%s Module Not Selected. Operation Abort!", pCInPickArm->GetStnName());
	//	HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", szMsg);
	//	return GMP_FAIL;
	//}

	//if (
	//	MoveZ(TA2_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS /*||
	//	SetRotarySol(OFF, GMP_WAIT) != GMP_SUCCESS*/
	//	)
	//{
	//	return GMP_FAIL;
	//}

	//lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "Move InPickArm to Place Posn?");
	//if (lReply == BUTTON_1)	// YES
	//{
	//	if (
	//		//pCInPickArm->MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
	//		//pCInPickArm->SetRotarySol(ON, GMP_WAIT) != GMP_SUCCESS ||
	//		MoveX(TA2_X_CTRL_GO_PLACE_ACF1WH_OFFSET, GMP_WAIT) != GMP_SUCCESS
	//		) 
	//	{
	//		return GMP_FAIL;
	//	}
	//}
	//else if (lReply == BUTTON_3)
	//{
	//	return GMP_FAIL;
	//}
	
	//lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "Move TA1 to Place Level?");
	//if (lReply == BUTTON_1)	// YES
	{
		if (MoveZ(TA2_Z_CTRL_GO_LOWER_LEVEL, GMP_WAIT) != GMP_SUCCESS)
		{
			return GMP_FAIL;
		}
	}
	//else if (lReply == BUTTON_3)
	//{
	//	return GMP_FAIL;
	//}	

	return nResult;

}

INT CTA2::SetupGoStandbyPosn()
{
	INT nResult = GMP_SUCCESS;

	nResult = MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);	

	return nResult;
}

INT CTA2::SetupGoPickGlassPosn()
{
	INT nResult = GMP_SUCCESS;


	if (
		MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS ||
		SetRotarySol(ON, GMP_WAIT) != GMP_SUCCESS ||
		MoveX(TA2_X_CTRL_GO_PICK_ACF1WH_POSN, GMP_WAIT) != GMP_SUCCESS 
		)
	{
		return GMP_FAIL;
	}

	return nResult;
}

INT CTA2::SetupGoPickGlassOffset()
{
	INT nResult = GMP_SUCCESS;

	if (
		MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS ||
		SetRotarySol(ON, GMP_WAIT) != GMP_SUCCESS ||
		MoveX(TA2_X_CTRL_GO_PICK_ACF1WH_OFFSET, GMP_WAIT) != GMP_SUCCESS 
		)
	{
		return GMP_FAIL;
	}

	return nResult;
}

INT CTA2::SetupGoPlaceGlassPosn()
{
	INT nResult = GMP_SUCCESS;

	if (
		MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS ||
		SetRotarySol(OFF, GMP_WAIT) != GMP_SUCCESS ||
		MoveX(TA2_X_CTRL_GO_PLACE_GLASS_POSN, GMP_WAIT) != GMP_SUCCESS 
		)
	{
		return GMP_FAIL;
	}

	return nResult;
}

INT CTA2::SetupGoPlaceGlassOffset()
{
	INT nResult = GMP_SUCCESS;

	if (
		MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS ||
		SetRotarySol(OFF, GMP_WAIT) != GMP_SUCCESS ||
		MoveX(TA2_X_CTRL_GO_PLACE_GLASS_OFFSET, GMP_WAIT) != GMP_SUCCESS 
		)
	{
		return GMP_FAIL;
	}

	return nResult;
}

INT CTA2::SetupGoPickGlass2Posn()
{
	INT nResult = GMP_SUCCESS;

	if (
		MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS ||
		SetRotarySol(ON, GMP_WAIT) != GMP_SUCCESS ||
		MoveX(TA2_X_CTRL_GO_PICK_ACF2WH_POSN, GMP_WAIT) != GMP_SUCCESS 
		)
	{
		return GMP_FAIL;
	}

	return nResult;
}

INT CTA2::SetupGoPickGlass2Offset()
{
	INT nResult = GMP_SUCCESS;

	if (
		MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS ||
		SetRotarySol(ON, GMP_WAIT) != GMP_SUCCESS ||
		MoveX(TA2_X_CTRL_GO_PICK_ACF2WH_OFFSET, GMP_WAIT) != GMP_SUCCESS 
		)
	{
		return GMP_FAIL;
	}

	return nResult;
}

LONG CTA2::HMI_SetZMoveLoopLimit(IPC_CServiceMessage &svMsg)	// 20140522 Yip
{
	LONG lLimit;
	svMsg.GetMsg(sizeof(LONG), &lLimit);

	m_lZMoveLoopLimit = lLimit;
	//SetAndLogParameter(__FUNCTION__, "Pick Place Retry Limit", m_lPickPlaceRetryLimit, lLimit);

	return 0;
}

LONG CTA2::HMI_LoopTestZMove(IPC_CServiceMessage &svMsg)
{
	BOOL bMode = TRUE;

	m_bZMoveLoopEnable = TRUE;
	TestZRepeatMove();

	bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

INT CTA2::TestZRepeatMove()
{
	LONG lReply = 0;
	LONG lLoopDelay = 800;
	CString szMsg("");

	if (!HMINumericKeys("Enter Test Loop Delay (ms)", 20*1000, 50, &lLoopDelay))
	{
		HMIMessageBox(MSG_OK, "TA2 Z TEST", "Loop Delay Input Invalid. OPERATION STOP");
		return GMP_FAIL;
	}

	szMsg.Format("LoopDelay = %ld", lLoopDelay);
	DisplayMessage(szMsg);

	lReply = HMIConfirmWindow("Yes", "NO", "Cancel", "OPTION", "Loop Test TA2-Z between StbyLvl and PickLvl?");
	if (lReply == BUTTON_1)	// YES
	{
		m_lZMoveLoopCount = 0;

			if (
				MoveZ(TA2_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
				MoveX(TA2_X_CTRL_GO_PLACE_GLASS_POSN, GMP_WAIT) != GMP_SUCCESS ||
				MoveTPlacePosn(GMP_WAIT) != GMP_SUCCESS
				)
			{
				HMIMessageBox(MSG_OK, "TA2 Z MOVE ERROR", "TA2-Z Move Standby Level Error. Operation Abort!");
				return GMP_FAIL;
			}


		while (m_bZMoveLoopEnable && m_lZMoveLoopCount++ < m_lZMoveLoopLimit)
		{
			if (MoveZ(TA2_Z_CTRL_GO_LOWER_LEVEL, GMP_WAIT) != GMP_SUCCESS)
			{
				HMIMessageBox(MSG_OK, "TA2 Z MOVE ERROR", "TA2-Z Move Lower Level Error. Operation Abort!");
				return GMP_FAIL;
			}
			
			if (MoveTPlacePosn(GMP_WAIT) != GMP_SUCCESS)
			{
				HMIMessageBox(MSG_OK, "TA2 T MOVE ERROR", "TA2-T Move Place Position Error. Operation Abort!");
				return GMP_FAIL;
			}
			Sleep(100);
			
			if (MoveZ(TA2_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS)
			{
				HMIMessageBox(MSG_OK, "TA2 Z MOVE ERROR", "TA2-Z Move Standby Level Error. Operation Abort!");
				return GMP_FAIL;
			}
			
			if (MoveTPickPosn(GMP_WAIT) != GMP_SUCCESS)
			{
				HMIMessageBox(MSG_OK, "TA2 T MOVE ERROR", "TA2-T Move Pick Position Error. Operation Abort!");
				return GMP_FAIL;
			}
			Sleep(100);

			SetHmiVariable("HMI_lTA2ZMoveLoopCount");
			Sleep(lLoopDelay);
		}

	}
	else if (lReply == BUTTON_2 || lReply == BUTTON_3) //cancel
	{
		return GMP_SUCCESS;
	}	
	return GMP_SUCCESS;
}


INT CTA2::KeepQuiet()
{
	INT nResult = GMP_SUCCESS;

	if (!m_fHardware)
	{
		return nResult;
	}

	if (!IsGLASS1_VacSensorOn())
	{
		SetGLASS1_VacSol(OFF, GMP_NOWAIT);
	}
	//if (!IsGLASS2_VacSensorOn())
	//{
	//	SetGLASS2_VacSol(OFF, GMP_NOWAIT);
	//}
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

VOID CTA2::PrintMachineSetup(FILE *fp)
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
		fprintf(fp, "%30s\t : %ld\n", "Left Safety Posn",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lLeftSafetyPosn"]);
		fprintf(fp, "%30s\t : %ld\n", "Right Safety Posn",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lRightSafetyPosn"]);
		fprintf(fp, "%30s\t : %ld\n", "Pick Posn",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPickGlassPosn"]);
		fprintf(fp, "%30s\t : %ld\n", "Place Posn",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPlaceGlassPosn"]);
	}

	CAC9000Stn::PrintMachineSetup(fp);
}

VOID CTA2::PrintDeviceSetup(FILE *fp)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString str = "";

	if (fp != NULL)
	{
		fprintf(fp, "\n<<<<%s>>>>\n", (const char*)GetStnName());
		fprintf(fp, "======================================================\n");
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
BOOL CTA2::printMachineDeviceInfo()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*> (m_pModule);
	BOOL bResult = TRUE;
	CString szBlank("--");
	CString szModule = GetStnName();
	LONG lValue = 0;
	DOUBLE dValue = 0.0;

	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrPRCalibPosn.x"];
		bResult = printMachineInfoRow(szModule, CString("Posn"), CString("mtrPRCalibPosn.x"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrPRCalibPosn.y"];
		bResult = printMachineInfoRow(szModule, CString("Posn"), CString("mtrPRCalibPosn.y"), szBlank, szBlank,
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
LONG CTA2::HMI_HomeAll(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	BOOL	bMode = TRUE;

	if (
		m_stMotorZ.Home(GMP_WAIT) != GMP_SUCCESS ||
		//MoveTPlacePosn(GMP_WAIT) != GMP_SUCCESS ||
		m_stMotorX.Home(GMP_WAIT) != GMP_SUCCESS
	   )
	{
		HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
	}

	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CTA2::HMI_PickGlassOperation(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bOpDone			= TRUE;
	BOOL bResult			= TRUE;
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

	bResult = PickGlassOperation(lACFWHx);
	
	if (bResult) //20130104
	{
		MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
	}

	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}

//LONG CTA2::HMI_PlaceGlassOperation(IPC_CServiceMessage &svMsg)
//{
//	BOOL bOpDone						= TRUE;
//	BOOL bResult						= FALSE;
//	
//
//	//if (g_lGlassInputMode == GLASS_1_INPUT)
//	//{
//	//	bResult = PlaceGlassOperation(GLASS1);
//	//}
//	//else if (g_lGlassInputMode == GLASS_2_INPUT)
//	//{
//	//	bResult = PlaceGlassOperation(GLASS2);
//	//}
//	//else if (g_lGlassInputMode == GLASS_1_2_INPUT)
//	//{
//	//	bResult = PlaceGlassOperation(MAX_NUM_OF_GLASS);
//	//}
//
//	bResult = PlaceGlassOperation(NO_GLASS);
//
//	if (bResult) //20130104
//	{
//		MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
//	}
//
//	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
//	return 1;
//}

LONG CTA2::IPC_SaveMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	
	pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lStandbyLevel"]		= m_lStandbyLevel;
	pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lLowerLevel"]		= m_lLowerLevel;

	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPickGlassPosn"]		= m_lPickGlassPosn;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPlaceGlassPosn"]		= m_lPlaceGlassPosn;

	return CTransferArm::IPC_SaveMachineParam();
}

LONG CTA2::IPC_LoadMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	m_lStandbyLevel		= pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lStandbyLevel"];
	m_lLowerLevel		= pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lLowerLevel"];
	m_lPickGlassPosn		= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPickGlassPosn"];
	m_lPlaceGlassPosn	= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPlaceGlassPosn"];

	return CTransferArm::IPC_LoadMachineParam();
}

LONG CTA2::IPC_SaveDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString str1(" ");
	CString str2(" ");

	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lPlaceGlassOffset"]		= m_lPlaceGlassOffset;
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lPickGlass1Offset"]	= m_lPickGlass1Offset;
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lPickGlass2Offset"]	= m_lPickGlass2Offset;

	pAppMod->m_smfDevice[GetStnName()]["Level"]["m_lPickLevel"]			= m_lPickLevel;
	pAppMod->m_smfDevice[GetStnName()]["Level"]["m_lPlaceLevel"]			= m_lPlaceLevel;

	pAppMod->m_smfDevice[GetStnName()]["Settings"]["HMI_bStandAlone"]	= HMI_bStandAlone;

	return CTransferArm::IPC_SaveDeviceParam();
}

LONG CTA2::IPC_LoadDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString str1(" ");
	CString str2(" ");
	
	m_lPlaceGlassOffset	= pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lPlaceGlassOffset"];
	m_lPickGlass1Offset = pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lPickGlass1Offset"]; //20121011
	m_lPickGlass2Offset = pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lPickGlass2Offset"]; //20121011

	m_lPickLevel		= pAppMod->m_smfDevice[GetStnName()]["Level"]["m_lPickLevel"];
	m_lPlaceLevel	= pAppMod->m_smfDevice[GetStnName()]["Level"]["m_lPlaceLevel"];

	HMI_bStandAlone			= (BOOL)(LONG)pAppMod->m_smfDevice[GetStnName()]["Settings"]["HMI_bStandAlone"];

	return CTransferArm::IPC_LoadDeviceParam();
}

LONG CTA2::HMI_ToggleUseFPCVacuum(IPC_CServiceMessage &svMsg)	//20141112
{
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1");
	//CTA3 *pCTA3	= (CTA3*)m_pModule->GetStation("TA3");


	m_bUseFPCVacuum = !m_bUseFPCVacuum;
	pCTA1->m_bUseFPCVacuum = m_bUseFPCVacuum;
	//pCTA3->m_bUseFPCVacuum = m_bUseFPCVacuum;
	return 0;
}

CMotorInfo &CTA2::GetMotorX() 
{
	return m_stMotorX;
}
CMotorInfo &CTA2::GetMotorY()
{
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");

	return pCACF1WH->m_stMotorY;
}

INT CTA2::SyncX()
{
	return m_stMotorX.Sync();
}

INT CTA2::SyncY()
{
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");
	return pCACF1WH->m_stMotorY.Sync();
}

INT CTA2::MoveAbsoluteXY(LONG lCntX, LONG lCntY, BOOL bWait)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	if (
		(m_stMotorX.MoveAbsolute(lCntX, GMP_NOWAIT) != GMP_SUCCESS)||
		(pCACF1WH->m_stMotorY.MoveAbsolute(lCntY, GMP_NOWAIT) != GMP_SUCCESS) 
	)
	{
		return GMP_FAIL;
	}
	
	if (GMP_WAIT == bWait)
	{
		if (
			(m_stMotorX.Sync() != GMP_SUCCESS) ||
			(pCACF1WH->m_stMotorY.Sync() != GMP_SUCCESS)
		)
		{
			return GMP_FAIL;
		}
	}
	return GMP_SUCCESS;
}

INT CTA2::MoveRelativeXY(LONG lCntX, LONG lCntY, BOOL bWait)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	if (
		(m_stMotorX.MoveRelative(lCntX, GMP_NOWAIT) != GMP_SUCCESS)||
		(pCACF1WH->m_stMotorY.MoveRelative(lCntY, GMP_NOWAIT) != GMP_SUCCESS) 
	)
	{
		return GMP_FAIL;
	}
	
	if (GMP_WAIT == bWait)
	{
		if (
			(m_stMotorX.Sync() != GMP_SUCCESS) ||
			(pCACF1WH->m_stMotorY.Sync() != GMP_SUCCESS)
		)
		{
			return GMP_FAIL;
		}
	}
	return GMP_SUCCESS;
}

INT CTA2::IndexGlassToTA2(GlassNum lGlassNum) // 20141031
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	LONG lAnswer = rMSG_TIMEOUT;

	CString strMsg;

	if (m_bIsOfflineMode)
	{
		return GMP_SUCCESS;
	}

	SetVacSol(lGlassNum, ON, GMP_WAIT);

	//if (lGlassNum == GLASS1)
	//{
		SetLFPCVacSol(ON, GMP_WAIT);
	//}
	//else if (lGlassNum == GLASS2)
	//{
	//	SetRFPCVacSol(ON, GMP_WAIT);
	//}

	if (IsVacSensorOn(lGlassNum))
	{
		strMsg.Format("Glass found on %s. Use current Glass for the setup?", GetStnName());
		lAnswer = HMIMessageBox(MSG_YES_NO, "Index Glass to TA2", strMsg);
		if (lAnswer == rMSG_YES)
		{
			return GMP_SUCCESS;
		}
		else if (lAnswer == rMSG_TIMEOUT)
		{
			return GMP_FAIL;
		}

		do
		{
			SetVacSol(lGlassNum, OFF);

			//if (lGlassNum == GLASS1)
			//{
				SetLFPCVacSol(OFF, GMP_WAIT);
			//}
			//else if (lGlassNum == GLASS2)
			//{
			//	SetRFPCVacSol(OFF, GMP_WAIT);
			//}


			strMsg.Format("Please remove Glass from %s.", GetStnName());
			lAnswer = HMIMessageBox(MSG_OK_CANCEL, "Index Glass to TA4", strMsg);

			if (lAnswer == rMSG_OK)
			{
				SetVacSol(lGlassNum, ON, GMP_WAIT);

				//if (lGlassNum == GLASS1)
				//{
					SetLFPCVacSol(ON, GMP_WAIT);
				//}
				//else if (lGlassNum == GLASS2)
				//{
				//	SetRFPCVacSol(ON, GMP_WAIT);
				//}
			}
			else if (lAnswer == rMSG_CANCEL)
			{
				SetVacSol(lGlassNum, ON, GMP_WAIT);

				if (!IsVacSensorOn(lGlassNum))
				{
					SetVacSol(lGlassNum, OFF);

					//if (lGlassNum == GLASS1)
					//{
						SetLFPCVacSol(OFF, GMP_WAIT);
					//}
					//else if (lGlassNum == GLASS2)
					//{
					//	SetRFPCVacSol(OFF, GMP_WAIT);
					//}
				}

				HMIMessageBox(MSG_OK, "Index Glass to TA2", "Operation Abort!");
				return GMP_FAIL;
			}
			else
			{
				SetVacSol(lGlassNum, ON, GMP_WAIT);
				//if (lGlassNum == GLASS1)
				//{
					SetLFPCVacSol(ON, GMP_WAIT);
				//}
				//else if (lGlassNum == GLASS2)
				//{
				//	SetRFPCVacSol(ON, GMP_WAIT);
				//}
				return GMP_FAIL;
			}
		} while (IsVacSensorOn(lGlassNum));
	}

	if (!pAppMod->CheckGlassIndexingPath(GetStnName(), lGlassNum))
	{
		return GMP_FAIL;
	}

	if (pAppMod->IsSelectCOG902())
	{
		lAnswer = HMIMessageBox(MSG_OK_CANCEL, "Index Glass to TA4", "Ready to get glass from COG902.");
	}
	else
	{
		lAnswer = HMIMessageBox(MSG_OK_CANCEL, "Index Glass to TA4", "Please place glass onto InConveyor.");
	}
	if (lAnswer == MSG_OK)
	{
		if (!IndexPickGlassOperation(lGlassNum))
		{
			return GMP_FAIL;
		}
	}
	else
	{
		return GMP_FAIL;
	}

	return GMP_SUCCESS;
}

BOOL CTA2::IndexPickGlassOperation(GlassNum lGlassNum)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");

	LONG lCurGlass = GLASS1;

	//if (lGlassNum == GLASS1 || lGlassNum == MAX_NUM_OF_GLASS)
	//{
	//	lCurGlass = GLASS1;
	//}
	//else
	//{
	//	lCurGlass = GLASS2;
	//}

	do
	{
		//if (
		//	//!pCTA1->PickGlassOperation() //||
		////	!pCTA1->PlaceGlassOperation((GlassNum)lCurGlass)
		//	)
		{
			return FALSE;
		}

		lCurGlass++;

	} while (lGlassNum == MAX_NUM_OF_GLASS && lCurGlass < MAX_NUM_OF_GLASS);

	//if (
	//	!PickGlassOperation(NO_GLASS)
	//   )
	//{
	//	return FALSE;
	//}
	return TRUE;
}

//LONG CTA2::HMI_SelectGlassNum(IPC_CServiceMessage &svMsg)
//{
//	ULONG ulGlassNum;
//	svMsg.GetMsg(sizeof(ULONG), &ulGlassNum);
//
//	INT nResult = GMP_SUCCESS;
//	CString szMsg = "";
//	
//	//m_lCurGlass	= ulGlassNum;
//	
//	UpdateModuleSetupPosn();
//	//UpdateModuleSetupLevel();
//	UpdateModuleSetupPara();
//
//	LogAction(__FUNCTION__);
//	LogAction("m_lCurGlass = %ld\t", m_lCurGlass);
//
//	return FALSE;
//}

//LONG CTA2::HMI_UpdateSetupStatus(IPC_CServiceMessage &svMsg)
//{
//	UpdateSetupStatus();
//	return 0;
//}

LONG CTA2::HMI_MeasureSetZero(IPC_CServiceMessage &svMsg) //20121016
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	if (bMode)
	{
		m_bEnableMeasure = TRUE;

		m_mtrMeasureSetZeroPt.x = m_stMotorX.GetEncPosn();
		m_mtrMeasureSetZeroPt.y = pCACF1WH->m_stMotorY.GetEncPosn();

		m_dMeasuredDistance = 0.0;
		m_dMeasuredAngle = 0.0;
	}
	else
	{
		m_bEnableMeasure = FALSE;
	}

	return 0;
}

LONG CTA2::HMI_MeasureEnable(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	if (bMode)
	{
		m_bEnableMeasure = TRUE;
	}
	else
	{
		m_bEnableMeasure = FALSE;
	}

	return 0;
}

LONG CTA2::HMI_SetStandAlone(IPC_CServiceMessage &svMsg)
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

LONG CTA2::HMI_SetManualPickCOF(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	
	if (bMode)
	{
		HMI_bManualPickCOF = TRUE;
	}
	else
	{
		HMI_bManualPickCOF = FALSE;
	}

	return 0;
}

VOID CTA2::UpdateOutput()
{
	if (m_fHardware && m_bCheckMotor) //20121016
	{
		CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
		if (m_bEnableMeasure)
		{
			DOUBLE dx, dy;
			dx = CountToDistance(m_stMotorX.GetEncPosn() - m_mtrMeasureSetZeroPt.x, m_stMotorX.stAttrib.dDistPerCount);
			dy = CountToDistance(pCACF1WH->m_stMotorY.GetEncPosn() - m_mtrMeasureSetZeroPt.y, pCACF1WH->m_stMotorY.stAttrib.dDistPerCount);
			m_dMeasuredDistance = sqrt(pow(dy, 2.0) + pow(dx, 2.0));
			m_dMeasuredAngle = calcDisplayAngleDegree(dy, dx);
		}
	}
	CTransferArm::UpdateOutput();
}
