/////////////////////////////////////////////////////////////////
//	ACFWH.cpp : interface of the CACFWH class
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
#include "math.h"
#include "MarkConstant.h"
#include "AC9000.h"
#include "AC9000_Constant.h"

#include "WinEagle.h"
#include "ACF.h"
#include "TA1.h"
#include "TA2.h"
#include "InspOpt.h"
#include "InPickArm.h"

#include "SettingFile.h"
#include "Cal_Util.h"
#include "PR_Util.h"


#define ACFWH_EXTERN
#include "ACFWH.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CACFWH, CWorkHolder)

CACFWH::CACFWH()
{
	INT i = 0;
	m_lCurACF = ACF_NONE;

	for (INT i = ACF1; i < MAX_NUM_OF_ACF; i++)
	{
		m_stACF[i].bEnable				= FALSE;
		m_stACF[i].bBonded				= FALSE;
		m_stACF[i].dLength				= 0.0;
		m_stACF[i].dWidth				= 0.0;
		m_stACF[i].stBondOffset.x		= 0;
		m_stACF[i].stBondOffset.y		= 0;
		m_stACF[i].stBondOffset.Theta	= 0.0;
		m_stACF[i].szName				= "";
	}

	m_lCurEdge							= EDGE_A;
	m_stACF[ACF1].bEnable				= TRUE;

	m_bGlassWithError					= FALSE;
	m_lCurACF							= ACF1;
	m_dwStartTime						= 0;
	m_dProcessTime						= 0.0;

	m_mtrBaseHeaterPosn.x				= 0;
	m_mtrBaseHeaterPosn.y				= 0;
	m_dLoadAdjXOffset					= 0.0; //20121005
	m_lLoadTOffset						= 0;

	for (INT i = ACF1; i < MAX_NUM_OF_ACF; i++)
	{
		m_lBondOffsetY[i]	= 0;
		m_dBondOffsetAdjY[i] = 0.0; //20120925
	}

	//for (INT i = WHZ_1; i < MAX_NUM_OF_WHZ; i++)
	//{
	//	for (INT j = 0; j < MAX_NUM_OF_ACF; j++)
	//	{
	//		m_lBondLevelOffset[i][j]		= 0;
	//	}
	//}

	HMI_lBondOffsetY			= 0;
#ifdef ACF_POSN_CALIB //20120829
	HMI_dBondOffsetAdjY			= 0.0;
	//HMI_bYZControl				= FALSE;
#endif
	//m_lPolarizerThickness[GLASS1] = 0; //20120927 pin detect polarizer
	//m_lPolarizerThickness[GLASS2] = 0;
	m_bUnloadReady = FALSE; //20140627
}

CACFWH::~CACFWH()
{
	//delete m_pCACF;
}

BOOL CACFWH::InitInstance()
{
	CWorkHolder::InitInstance();

	CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1");

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

			// IOs
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

		if (GetStnName() == "ACF1WH")
		{
			pAppMod->m_bACF1WHComm	= TRUE;	
		}
		else if (GetStnName() == "ACF2WH")	
		{	
			pAppMod->m_bACF2WHComm	= TRUE;	
		}

		// wait for all servo commutated (even with error)
		do
		{
			Sleep(100);
		} while (!pAppMod->IsAllServoCommutated());

#if 1

		DisplayMessage("ACFWH waiting ACF1 home finish before start homing.");
		do
		{
			Sleep(100);
		} while (!pAppMod->m_bACF1Home && !m_pCACF->m_bInitInstanceError);

		DisplayMessage("ACFWH start home.");
#endif

		if (!m_bInitInstanceError)
		{
			szMsg = _T("	") + GetName() + " --- homing motors";
			DisplayMessage(szMsg);

			// Home Motor
			if (
				m_stMotorY.Home(GMP_WAIT) != GMP_SUCCESS ||
				m_stMotorY.EnableProtection() != GMP_SUCCESS ||
				m_stMotorT.Home(GMP_WAIT) != GMP_SUCCESS ||
				m_stMotorT.EnableProtection() != GMP_SUCCESS 
			   )
			{
				m_bInitInstanceError	= TRUE;
				pAppMod->m_bHWInitError = TRUE;
			}

			// wait for other modules to be homed (even with error)
			do
			{
				Sleep(100);
			} while (!pAppMod->m_bTA2Home || !pAppMod->m_bTA1Home);

			if (pAppMod->m_bHWInitError && (pCTA2->IsInitInstanceError() || pCTA1->IsInitInstanceError()))
			{
				m_bInitInstanceError	= TRUE;
			}
			//else if (
			//		 m_stMotorZ[WHZ_1].MoveRelative(-2000, GMP_WAIT) != GMP_SUCCESS ||	// Move Up for searching Indexor
			//		 m_stMotorZ[WHZ_1].Home(GMP_WAIT) != GMP_SUCCESS
			//		)
			//{
			//	m_bInitInstanceError	= TRUE;
			//	pAppMod->m_bHWInitError = TRUE;
			//}
			else
			{
				// Everythings OK
				m_bModSelected			= TRUE;

				//m_stMotorZ[WHZ_1].bHomeStatus = TRUE;
				//m_stMotorZ[WHZ_2].bHomeStatus = TRUE;
			}

			if (GetStnName() == "ACF1WH")
			{	
				pAppMod->m_bACF1WHHome	= TRUE;	
			}
			else if (GetStnName() == "ACF2WH")	
			{	
				pAppMod->m_bACF2WHHome	= TRUE;	
			}
		}
	}
	else
	{
		if (GetStnName() == "ACF1WH")
		{	
			pAppMod->m_bACF1WHHome	= TRUE;
			pAppMod->m_bACF1WHComm	= TRUE;
		}
		else if (GetStnName() == "ACF2WH")
		{	
			pAppMod->m_bACF2WHHome	= TRUE;
			pAppMod->m_bACF2WHComm	= TRUE;
		}
	}

	return TRUE;
}

INT CACFWH::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWorkHolder::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CACFWH::Operation()
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
VOID CACFWH::UpdateOutput()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	INT i = 0;
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH= (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");

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
			if (m_bEnableMeasure) //20121015
			{
				DOUBLE dx, dy;
				dx = CountToDistance(pCInspOpt->m_stMotorX.GetEncPosn() - m_mtrMeasureSetZeroPt.x, pCInspOpt->m_stMotorX.stAttrib.dDistPerCount);
				dy = CountToDistance(m_stMotorY.GetEncPosn() - m_mtrMeasureSetZeroPt.y, m_stMotorY.stAttrib.dDistPerCount);
				m_dMeasuredDistance = sqrt(pow(dy, 2.0) + pow(dx, 2.0));
				m_dMeasuredAngle = calcDisplayAngleDegree(dy, dx);
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
	// Keep it for debug breakbpoint
	CWorkHolder::UpdateOutput();
}

VOID CACFWH::RegisterVariables()
{
	try
	{

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
CMotorInfo &CACFWH::GetMotorX() 
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");

	return pCInspOpt->m_stMotorX;
}

CMotorInfo &CACFWH::GetMotorY()
{
	return m_stMotorY;
}

CMotorInfo &CACFWH::GetMotorT()
{
	return m_stMotorT;
}

INT CACFWH::SyncX()
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");

	return pCInspOpt->m_stMotorX.Sync();
}

INT CACFWH::SyncY()
{
	return m_stMotorY.Sync();
}

INT CACFWH::SyncT()
{
	return m_stMotorT.Sync();
}

INT CACFWH::MoveXYToPRCalibPosn(BOOL bWait)
{
	return OpticMoveAbsoluteXY(m_mtrPRCalibPosn.x, m_mtrPRCalibPosn.y, bWait);
}
// ------------------------------------------------------------------------
// Virtual WorkHolder Function delcaration
// ------------------------------------------------------------------------
BOOL CACFWH::PickGlassOperation(GlassNum lGlassNum)
{	
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");
	CInPickArm *pCInPickArm = (CInPickArm*)m_pModule->GetStation("InPickArm");

	if (
		//pCInPickArm->PickGlassOperation(NO_GLASS) &&
		//pCInPickArm->PlaceGlassOperation(NO_GLASS) &&
		pCTA1->PlaceGlassOperation((ACFWHUnitNum)m_lCurSlave)
	   )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////
//Modified Virtual Operational Functions
/////////////////////////////////////////////////////////////////
VOID CACFWH::PreStartOperation()
{
	m_bModError = FALSE;
	m_bGlass1Exist = FALSE;
	m_bGlass2Exist = FALSE;
	m_bUnloadReady = FALSE; //20140627
	m_bPRDone	   = FALSE;
	m_lRejectGlassCounter	= 0;

	m_lCurEdge				= EDGE_A;
	m_lCurACF				= ACF_1;
	m_dProcessTime			= 0.0;

	if (!m_bModSelected)
	{	
		m_bUnloadReady = TRUE;	// 20140916 Yip: Assume ACFWH Unload Ready If ACFWH Not Selected
		ACF_WH_Status[m_lCurSlave] = ST_MOD_NOT_SELECTED;
		m_qSubOperation = ACF_WH_STOP_Q;
		m_bStopCycle = FALSE;
	}
	else
	{
		//switch (m_lCurSlave)	// 20140918 Yip: Assume ACFWH Unload Ready If Current Glass Not Enabled
		//{
		//case ACFWH_1:
		//	m_bUnloadReady = ((g_lGlassInputMode != GLASS_1_INPUT) && (g_lGlassInputMode != GLASS_1_2_INPUT));
		//	break;
		//case ACFWH_2:
		//	m_bUnloadReady = ((g_lGlassInputMode != GLASS_2_INPUT) && (g_lGlassInputMode != GLASS_1_2_INPUT));
		//	break;
		//}

		if (
			((m_nLastError = SetWeakBlowSol(OFF)) != GMP_SUCCESS) ||
			// 20140616 Yip: Not Vacuum On During Pre Start
//			(AutoMode != BURN_IN && ((m_nLastError = SetGLASS1_VacSol(ON)) != GMP_SUCCESS)) || //20120309
			//((m_nLastError = MoveZToStandbyLevel(GLASS1, GMP_WAIT)) != GMP_SUCCESS) ||
			((m_nLastError = MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT)) != GMP_SUCCESS) ||
			((m_nLastError = MoveT(ACF_WH_T_CTRL_GO_STANDBY, GMP_WAIT)) != GMP_SUCCESS) 
		   )
		{
			Command(glSTOP_COMMAND, TRUE);
			ACF_WH_Status[m_lCurSlave] = ST_STOP;
			m_qSubOperation = ACF_WH_STOP_Q;
			m_bStopCycle = TRUE;
		}
		else
		{
			ACF_WH_Status[m_lCurSlave] = ST_IDLE;
			m_qSubOperation = ACF_WH_IDLE_Q;
			m_bStopCycle = FALSE;

			if(IsBurnInDiagBond())
			{
				ACF_GLASS_Status[m_lCurSlave][GLASS1] = ST_EMPTY;
			}

		}		
	}

	Motion(FALSE);
	State(IDLE_Q);

	m_deqDebugSeq.clear(); //20131010
	m_qPreviousSubOperation = m_qSubOperation;
	UpdateDebugSeq(GetSequenceString(), ACF_WH_Status[m_lCurSlave], ACF_GLASS_Status[m_lCurSlave]);
	
	CAC9000Stn::PreStartOperation();
}

VOID CACFWH::AutoOperation()
{
	// stop command received: end operation after cycle completed
	if (Command() == glSTOP_COMMAND || g_bStopCycle)
	{
		m_bStopCycle = TRUE;
	}

	OperationSequence();

	// cycle completed or motor error or abort requested: end operation immediately
	if (
		(m_bModSelected && m_qSubOperation == ACF_WH_STOP_Q) || 
		(!m_bModSelected && m_bStopCycle) || 
		GMP_SUCCESS != m_nLastError || IsAbort() 
	   )
	{
		if (GMP_SUCCESS != m_nLastError)
		{
			CString szDebug;
			szDebug.Format("!!! last error: m_qSubOperation=%d, ACF_WH_Status=%ld, m_lCurSlave=%ld", m_qSubOperation, ACF_WH_Status[m_lCurSlave], m_lCurSlave);
			DisplayMessage(szDebug);
		}

		if (m_fHardware && m_bModSelected)
		{
//#if 1 //20121224 vac error
			CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
			CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1");
			if ((!pCTA2->m_bIsVacError) && (!pCTA1->m_bIsVacError))
			{
				//MoveZToStandbyLevel(GLASS1, GMP_WAIT);
				MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT);
				MoveT(ACF_WH_T_CTRL_GO_STANDBY, GMP_WAIT);
			}
			else
			{
				if (pCTA2->m_bIsVacError)
				{
					DisplayMessage("TA2 VAC error: ACFWH not move to standby");
				}
				if (pCTA1->m_bIsVacError)
				{
					DisplayMessage("TA1 VAC error: ACFWH not move to standby");
				}
			}
//#else
//			MoveZToStandbyLevel(GLASS1, GMP_WAIT);
//			MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT);
//#endif
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

		ACF_WH_Status[m_lCurSlave] = ST_STOP;
		Command(glSTOP_COMMAND);
		State(STOPPING_Q);
		m_bStopCycle = FALSE;

		CAC9000Stn::StopOperation();
	}
}

/////////////////////////////////////////////////////////////////
//Operational Sequences
/////////////////////////////////////////////////////////////////
CString CACFWH::GetSequenceString()
{

	switch (m_qSubOperation)
	{
	case ACF_WH_IDLE_Q:						//0
		return "ACF_WH_IDLE_Q";

	case ACF_WH_REQUEST_FOR_GLASS_Q:		//1
		return "ACF_WH_REQUEST_FOR_GLASS_Q";

	case ACF_WH_CHECK_GLASS_EXIST_Q:		//2
		return "ACF_WH_CHECK_GLASS_EXIST_Q";

	case ACF_WH_MOVE_TO_CURRENT_EDGE_Q:		//3
		return "ACF_WH_MOVE_TO_CURRENT_EDGE_Q";

	case ACF_WH_DO_PR1_Q:		//4
		return "ACF_WH_DO_PR1_Q";

	case ACF_WH_REDO_PR1_Q:		//5
		return "ACF_WH_REDO_PR1_Q";

	case ACF_WH_DO_PR2_Q:		//6
		return "ACF_WH_DO_PR2_Q";

	case ACF_WH_REDO_PR2_Q:		//7
		return "ACF_WH_REDO_PR2_Q";

	case ACF_WH_CHECK_LAST_PR_COMPLETED_Q:		//8
		return "ACF_WH_CHECK_LAST_PR_COMPLETED_Q";

	case ACF_WH_WAIT_TO_BOND_POSN_Q:			//9
		return "ACF_WH_WAIT_TO_BOND_POSN_Q";

	case ACF_WH_WAIT_ACF_COMPLETE_Q:			//10
		return "ACF_WH_WAIT_ACF_COMPLETE_Q";

	case ACF_WH_CHECK_LAST_BONDED_ON_EDGE_Q:	//11
		return "ACF_WH_CHECK_LAST_BONDED_ON_EDGE_Q";

	case ACF_WH_POSTBOND_CHECK_Q:		//12
		return "ACF_WH_POSTBOND_CHECK_Q";

	case ACF_WH_DO_POST_PR1_Q:			//13
		return "ACF_WH_DO_POST_PR1_Q";

	case ACF_WH_REDO_POST_PR1_Q:		//14
		return "ACF_WH_REDO_POST_PR1_Q";

	case ACF_WH_DO_POST_PR2_Q:			//15
		return "ACF_WH_DO_POST_PR2_Q";

	case ACF_WH_REDO_POST_PR2_Q:		//16
		return "ACF_WH_REDO_POST_PR2_Q";

	case ACF_WH_DO_POST_MP1_Q:			//17
		return "ACF_WH_DO_POST_MP1_Q";

	case ACF_WH_DO_POST_MP2_Q:			//18
		return "ACF_WH_DO_POST_MP2_Q";

	case ACF_WH_CHECK_LAST_POST_PR_ON_EDGE_Q:	//19
		return "ACF_WH_CHECK_LAST_POST_PR_ON_EDGE_Q";

	case ACF_WH_CHECK_ACF_LENGTH_TOL_Q:			//20
		return "ACF_WH_CHECK_ACF_LENGTH_TOL_Q";

	case ACF_WH_GO_UNLOAD_GLASS_Q:				//21
		return "ACF_WH_GO_UNLOAD_GLASS_Q";

	case ACF_WH_WAIT_GLASS_PICKED_Q:			//22
		return "ACF_WH_WAIT_GLASS_PICKED_Q";

	case ACF_WH_WAIT_WINEAGE_ACCEPT_PR1_Q:		//23
		return "ACF_WH_WAIT_WINEAGE_ACCEPT_PR1_Q";

	case ACF_WH_WAIT_USER_FINISH_PR1_Q:			//24
		return "ACF_WH_WAIT_USER_FINISH_PR1_Q";

	case ACF_WH_WAIT_WINEAGE_ACCEPT_PR2_Q:		//25
		return "ACF_WH_WAIT_WINEAGE_ACCEPT_PR2_Q";

	case ACF_WH_WAIT_USER_FINISH_PR2_Q:			//26
		return "ACF_WH_WAIT_USER_FINISH_PR2_Q";

	case ACF_WH_WAIT_PLACE_GLASS_Q:				//27
		return "ACF_WH_WAIT_USER_FINISH_PR2_Q";	
	
	case ACF_WH_WAIT_OPTIC_PERMISSION_Q:		//28
		return "ACF_WH_WAIT_PLACE_GLASS_Q";

	case ACF_WH_WAIT_OPTIC_ACK_Q:				//29
		return "ACF_WH_WAIT_OPTIC_ACK_Q";

	case ACF_WH_WAIT_OPTIC_PB_INSP_PERMISSION_Q:	//30
		return "ACF_WH_WAIT_OPTIC_PB_INSP_PERMISSION_Q";

	case ACF_WH_WAIT_OPTIC_PB_INSP_ACK_Q:		//31
		return "ACF_WH_WAIT_OPTIC_PB_INSP_ACK_Q";

	case ACF_WH_STOP_Q:							//99
		return "ACF_WH_STOP_Q";

	}

	return "Hi";
}


VOID CACFWH::OperationSequence()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");
	CACF *pCACF = (CACF*)m_pModule->GetStation("ACF");
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");


	PRINT_DATA	stPrintData;
	INT			i					= 0;
	LONG		lStep				= 0;
	DOUBLE		dTempACFAngX		= m_dACFAngX;		// For Compensating PostBond Theta (Not Including m_dACFAngX)
	CString		szMsg				= _T("");
	MTR_POSN	mtrTemp;
	LONG		lNextEdge			= EDGE_A;
	LONG		lACFNum				= ACF1;
	BOOL		bSrchResult			= TRUE;
	MTR_POSN	mtr_Offset;

	// Cut Cycle Time
	BOOL bSafeMoveWait = GMP_WAIT;
	static BOOL bSafeMoveSrchPRWait;

	// Log Bond Pt for verification
	for (i = 0; i < MAX_PRINT_DATA; i++)
	{
		stPrintData.szTitle[i]	= "";
	}

	stPrintData.szTitle[0]	= "No";
	stPrintData.szTitle[1]	= "Centre X";
	stPrintData.szTitle[2]	= "Centre Y";
	stPrintData.szTitle[3]	= "Angle";
	stPrintData.szTitle[4]	= "Distance(mm)";
	stPrintData.szTitle[5]	= "PR1 Posn X";
	stPrintData.szTitle[6]	= "PR1 Posn Y";
	stPrintData.szTitle[7]	= "PR1 Angle";
	stPrintData.szTitle[8]	= "PR2 Posn X";
	stPrintData.szTitle[9]	= "PR2 Posn Y";
	stPrintData.szTitle[10]	= "PR2 Angle";

	switch (m_qSubOperation)
	{
	case ACF_WH_IDLE_Q:							// 0
		if (AutoMode == BURN_IN || AutoMode == DIAG_BOND || !IsGLASS1_VacSensorOn())
		{
			m_bGlass1Exist = FALSE;
			ACF_WH_Status[m_lCurSlave] = ST_EMPTY;
			ACF_GLASS_Status[m_lCurSlave][GLASS1] = ST_EMPTY;

			for (i = 0; i < MAX_NUM_OF_ACF; i++)
			{
				g_stACF[i].bBonded = FALSE;
			}

			if (
				((m_nLastError = SetGLASS1_VacSol(OFF)) == GMP_SUCCESS) && 
				((m_nLastError = MoveY(ACF_WH_Y_CTRL_GO_LOAD_POSN_OFFSET, GMP_WAIT)) == GMP_SUCCESS) &&
				((m_nLastError = MoveT(ACF_WH_T_CTRL_GO_LOAD_POSN_OFFSET, GMP_WAIT)) == GMP_SUCCESS) 
			)
			{
				ACF_WH_Status[m_lCurSlave] = ST_READY_LOAD;
				m_qSubOperation = ACF_WH_REQUEST_FOR_GLASS_Q;		// go to 1
			}
		}
		// init state machine with glass
		else
		{
			m_bGlass1Exist = TRUE;
			ACF_GLASS_Status[m_lCurSlave][GLASS1] = ST_READY;
			ACF_WH_Status[m_lCurSlave] = ST_BUSY;

			// process timer starts
			m_dwStartTime = GetTickCount();

			if (m_bGlassWithError)
			{
				ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
				m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
			}
			else
			{
				m_dBondAngle = 0.0;
				//m_lCurACF = FindFirstUnit((EdgeNum)m_lLoadGlassEdge);
				//m_lCurEdge = g_stACF[m_lCurACF].lEdgeInfo;

				if (g_bSelectedReworkMode)
				{
					//Set the Rework LSI to start with
					for (i = ACF1; i < MAX_NUM_OF_ACF; i++)
					{
						if (!m_bReworkACFNum[i])
						{
							g_stACF[i].bBonded = TRUE;
						}
					}
				}
				bSafeMoveSrchPRWait = GMP_WAIT;
				m_qSubOperation = ACF_WH_MOVE_TO_CURRENT_EDGE_Q;	// go to 3					
			}
		}
		break;

	case ACF_WH_REQUEST_FOR_GLASS_Q:			// 1
		if (TA1_Status == ST_STOP)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
		}
		else if (
				((m_lCurSlave == ACFWH_1) && (TA1_Status == ST_ACFWH1_ACK_LOAD)) ||
				((m_lCurSlave == ACFWH_2) && (TA1_Status == ST_ACFWH2_ACK_LOAD)) 
				)
		{
			ACF_WH_Status[m_lCurSlave] = ST_ACK;

			m_qSubOperation = ACF_WH_WAIT_PLACE_GLASS_Q;	// 27
		}
		break;


	case ACF_WH_WAIT_PLACE_GLASS_Q:			// 27
		if (TA1_Status == ST_STOP || TA1_Status == ST_VAC_ERROR)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
		}
		else if(ACF_WH_Status[m_lCurSlave] == ST_READY)
		{
			m_qSubOperation = ACF_WH_CHECK_GLASS_EXIST_Q;	// 2
		}
		break;


	case ACF_WH_CHECK_GLASS_EXIST_Q:			// 2
		if (AutoMode == BURN_IN || AutoMode == DIAG_BOND || IsGLASS1_VacSensorOn())
		{
			ACF_WH_Status[m_lCurSlave] = ST_BUSY;
			ACF_GLASS_Status[m_lCurSlave][GLASS1] = ST_READY;
			

			// process timer starts
			m_dwStartTime = GetTickCount();

			for (i = EDGE_A; i < MAX_EDGE_OF_GLASS; i++)	 // 20151023
			{
				//clear data of previous glass
				m_dUnloadGlassCompensationAngle[i % MAX_EDGE_OF_GLASS] = 0.0;
			}

			m_dBondAngle = 0.0;
			m_bGlass1Exist = TRUE;
			ACF_GLASS_Status[m_lCurSlave][GLASS1] = ST_READY;
			m_bGlassWithError = FALSE;

			// Assign the m_lCurACF starting from the first unit on Load Glass Edge
			//m_lCurACF = FindFirstUnit((EdgeNum)m_lLoadGlassEdge);

			for (i = 0; i < MAX_NUM_OF_ACF; i++)
			{
				g_stACF[i].bBonded = FALSE;
			}

			if (g_bSelectedReworkMode)
			{
				//Set the Rework LSI to start with
				for (i = ACF1; i < MAX_NUM_OF_ACF; i++)
				{
					if (!m_bReworkACFNum[i])
					{
						g_stACF[i].bBonded = TRUE;
					}
				}
			}

			bSafeMoveSrchPRWait = GMP_NOWAIT;

			// Not All ACF Bonded
			// Assign the suitable Edge to start
			m_qSubOperation = ACF_WH_MOVE_TO_CURRENT_EDGE_Q;	// go to 3				
		}
		else
		{
			// Request for replace
			//ACF_WH_Status[m_lCurSlave] = ST_VAC_ERROR;
			//bSafeMoveSrchPRWait = GMP_WAIT;
			//m_qSubOperation = ACF_WH_REQUEST_FOR_GLASS_Q; // go to 1

			// ERROR!
			m_bModError = TRUE;
			m_bGlass1Exist = FALSE;
			ACF_GLASS_Status[m_lCurSlave][GLASS1] = ST_EMPTY;

			ACF_WH_Status[m_lCurSlave] = ST_VAC_ERROR;
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
			WriteHMIMess("*** ERROR: ACF_WH Get Glass Error ***", TRUE);
			if(m_lCurSlave == ACFWH_1)
			{
				SetError(IDS_ACF1WH_GLASS1_VAC_ERR);
			}
			else
			{
				SetError(IDS_ACF2WH_GLASS1_VAC_ERR);
			}
		}
		break;

		// IMPORTANT STATE
		// Check Whether All ACF is bonded
		// Check and assign the next edge to bond
	case ACF_WH_MOVE_TO_CURRENT_EDGE_Q: 		// 3
		if (m_bStopCycle)
		{
			bSafeMoveSrchPRWait = GMP_WAIT;
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
		}
		else if (!g_stACF[m_lCurACF].bBonded)
		{
			// Assign the suitable Edge to start
			if ((m_lCurACF /*= FindFirstUnit((EdgeNum)m_lCurEdge)*/) == ACF_1)
			{
				if (g_bSelectedReworkMode)
				{
					for (INT i = m_lCurACF; i < MAX_NUM_OF_ACF; i++)
					{
						if (m_lCurEdge == g_stACF[i].lEdgeInfo && !g_stACF[i].bBonded)
						{
							m_lCurACF = i % MAX_NUM_OF_ACF;
							break;
						}
						else if (m_lCurEdge != g_stACF[i].lEdgeInfo)
						{
							szMsg.Format("Cannot Find ACF %d on Edge%ld", i, m_lCurEdge);
							DisplayMessage(szMsg);
							WriteHMIMess("*********************************************");
							WriteHMIMess(szMsg);
							WriteHMIMess("*********************************************");
							m_bModError = TRUE;
							m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
						}
					}
				}

				m_dBondAngle = 0.0;

				CalPosnAfterRotate(m_lCurEdge, m_mtrPR1Posn[g_stACF[m_lCurACF].lEdgeInfo], m_dBondAngle, &mtrTemp);
				m_dmtrCurrPR1Posn.x = (DOUBLE)mtrTemp.x;
				m_dmtrCurrPR1Posn.y = (DOUBLE)mtrTemp.y;

				if (
					((m_nLastError = MoveYToPR1Posn(g_stACF[m_lCurACF].lEdgeInfo, GMP_NOWAIT)) == GMP_SUCCESS)
					)
				{
					bSafeMoveSrchPRWait = GMP_WAIT;

					ACF_WH_Status[m_lCurSlave] = ST_REQ_PR;
					m_qSubOperation = ACF_WH_WAIT_OPTIC_PERMISSION_Q;	// go to 28
				}
			}
			else
			{			
				szMsg.Format("NO_ACF is on Edge%ld", m_lCurEdge);
				DisplayMessage(szMsg);
				WriteHMIMess("*********************************************");
				WriteHMIMess(szMsg);
				WriteHMIMess("*********************************************");
				m_bModError = TRUE;
				m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
			}

		}
		else
		{
			if (m_bPBEnable)
			{
				// For Rework Mode, Operation Sequence Resumed with bonded ACF
				if (1/*(m_lCurEdge = GetNextPostBondEdge(g_stACF[m_lCurACF].lEdgeInfo)) != EDGE_NONE*/)
				{
					// Assign the suitable Edge to start
					if (1/*(m_lCurACF = FindFirstUnit((EdgeNum)m_lCurEdge)) != NO_ACF*/)
					{
						m_dBondAngle = 0.0;

						m_dBondAngle = 1 * (m_dCurrAlignAngle - m_dBondOffsetT);

						CalPosnAfterRotate(m_lCurEdge, m_mtrPBPR1Posn, -m_dBondAngle, &mtrTemp);
						m_dmtrCurrPBPR1Posn.x = (DOUBLE)mtrTemp.x;
						m_dmtrCurrPBPR1Posn.y = (DOUBLE)mtrTemp.y;

						if (
							((m_nLastError = MoveAbsoluteT(m_lStandbyPosnT[m_lCurACF] + m_lTOffset, GMP_NOWAIT)) == GMP_SUCCESS) && 
							((m_nLastError = PBMoveY(ACF_WH_XY_CTRL_GO_PBPR1_POSN, GMP_WAIT)) == GMP_SUCCESS )
							)
						{
							//m_qSubOperation = ACF_WH_DO_PR1_Q;	// go to 4

							ACF_WH_Status[m_lCurSlave] = ST_REQ_PR;
							m_qSubOperation = ACF_WH_WAIT_OPTIC_PB_INSP_PERMISSION_Q;	// go to 28
						}
					}
					else
					{
						szMsg.Format("NO_ACF is on Edge%ld", m_lCurEdge);
						DisplayMessage(szMsg);
						WriteHMIMess("*********************************************");
						WriteHMIMess(szMsg);
						WriteHMIMess("*********************************************");
						m_bModError = TRUE;
						m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
					}
				}
				else
				{
					// All ACF on Glass is Bonded and PostBond Searched
					m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
				}
			}
			else
			{
				// All ACF on Glass is Bonded and PostBond Searched
				m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
			}
		}
		break;

	case ACF_WH_WAIT_OPTIC_PERMISSION_Q:		//28
		if (m_bStopCycle)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 21
		}
		else if(
				((m_lCurSlave == ACFWH_1) && (INSPOPT_Status == ST_ACFWH1_REQ_PR_ACK)) ||
				((m_lCurSlave == ACFWH_2) && (INSPOPT_Status == ST_ACFWH2_REQ_PR_ACK)) 
			)
		{
				ACF_WH_Status[m_lCurSlave] = ST_PR_IN_PROCESS;
				
				if (
					(m_nLastError = pCInspOpt->MoveAbsoluteX((LONG)m_dmtrCurrPR1Posn.x, GMP_WAIT)) == GMP_SUCCESS ||
					(m_nLastError = SyncY()) == GMP_SUCCESS 
					)
				{
					m_qSubOperation = ACF_WH_DO_PR1_Q;	// go to 4
				}
		}

		break;

	case ACF_WH_DO_PR1_Q:						// 4
		if (m_bStopCycle)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	
		}
		else
		{
			Sleep(m_lPRDelay);
			if (!AutoSearchPR1GrabOnly(&pruACF1))
			{
				m_qSubOperation = ACF_WH_REDO_PR1_Q;	// go to 5
			}
			else
			{
				CalPosnAfterRotate(EDGE_A, m_mtrPR2Posn[EDGE_A], m_dBondAngle, &mtrTemp);
				m_dmtrCurrPR2Posn.x = (DOUBLE)mtrTemp.x;
				m_dmtrCurrPR2Posn.y = (DOUBLE)mtrTemp.y;

				if (
					((m_nLastError = MoveYToPR2Posn(g_stACF[m_lCurACF].lEdgeInfo, GMP_NOWAIT)) == GMP_SUCCESS) &&
					((m_nLastError = pCInspOpt->MoveAbsoluteX((LONG)m_dmtrCurrPR2Posn.x, GMP_NOWAIT)) == GMP_SUCCESS)
					)
				{
					if (
						AutoSearchPR1ProcessOnly(&pruACF1) &&
						((m_nLastError = pCInspOpt->SyncX()) == GMP_SUCCESS) &&
						((m_nLastError = SyncY()) == GMP_SUCCESS)
					)
					{
						m_bPRError = FALSE;
						m_lPRRetryCounter = 0;
						m_qSubOperation = ACF_WH_DO_PR2_Q;	// go to 6
					}
					else
					{
						//Sleep(500);
						//SynMtrAxis(pCInspOpt->m_stMotorX);
						//PRS_AutoBondMode(FALSE, FALSE);
						//PRS_GrabDispImage(&pruACF1[g_stACF[m_lCurACF].lEdgeInfo]);
						//Sleep(5000);

						//if (g_bIsUserSearchPRDone)
						//{
						//	if ((m_nLastError = MoveXYToPR1Posn(g_stACF[m_lCurACF].lEdgeInfo, GMP_WAIT)) == GMP_SUCCESS)
						//	{
						//		ACF_WH_Status[m_lCurSlave] = ST_REQ_CLICK_PR;
						//		m_qSubOperation = ACF_WH_WAIT_WINEAGE_ACCEPT_PR1_Q;	// go to 5
						//	}
						//}
						//else
						{
							m_qSubOperation = ACF_WH_REDO_PR1_Q;	// go to 5
						}
					}
				}
			}
		}
		break;

	case ACF_WH_REDO_PR1_Q:						// 5
		m_bPRError = TRUE;
		//retry PR
		if (m_lPRRetryLimit != 0)
		{
			m_lPRRetryCounter++;

			if (m_lPRRetryCounter >= m_lPRRetryLimit)
			{
				// update stats
				pAppMod->m_ulACFWHPRError++;
				pAppMod->m_ulTotalACFWHPRError++;

				//reject glass
				if (m_lRejectGlassLimit != 0)
				{
					m_lRejectGlassCounter++;
					m_lPRRetryCounter = 0;

					// ERROR!
					if (m_lRejectGlassCounter >= m_lRejectGlassLimit)
					{
						m_qSubOperation = ACF_WH_STOP_Q;	// go to 99

						if(m_lCurSlave == ACFWH_1)
						{
							WriteHMIMess("*** ERROR: ACF1WH Reject Glass Limit Hit ***", TRUE);
							WriteHMIMess("*** ERROR: ACF1WH PR 1 Error ***", TRUE);
						}
						else
						{
							WriteHMIMess("*** ERROR: ACF2WH Reject Glass Limit Hit ***", TRUE);
							WriteHMIMess("*** ERROR: ACF2WH PR 1 Error ***", TRUE);
						}

						m_bModError = TRUE;
						if(m_lCurSlave == ACFWH_1)
						{
							SetError(IDS_ACF1WH_REJECT_GLASS_LIMIT_HIT);
							SetError(IDS_ACF1WH_PR1_ERR);
						}
						else
						{
							SetError(IDS_ACF2WH_REJECT_GLASS_LIMIT_HIT);
							SetError(IDS_ACF2WH_PR1_ERR);
						}
					}
					else
					{
						ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
						m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
						if(m_lCurSlave == ACFWH_1)
						{
							WriteHMIMess("*** ACF1WH PR 1 Error: Rejecting Glass... ***", TRUE);
						}
						else
						{
							WriteHMIMess("*** ACF2WH PR 1 Error: Rejecting Glass... ***", TRUE);
						}
					}
				}
				else
				{
					ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
					m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
					if(m_lCurSlave == ACFWH_1)
					{
						WriteHMIMess("*** ACF1WH PR 1 Error: Rejecting Glass... ***", TRUE);
					}
					else
					{
						WriteHMIMess("*** ACF2WH PR 1 Error: Rejecting Glass... ***", TRUE);
					}
				}
			}
			else
			{
				if (
					((m_nLastError = MoveYToPR1Posn(g_stACF[m_lCurACF].lEdgeInfo, GMP_WAIT)) == GMP_SUCCESS) &&
					((m_nLastError = pCInspOpt->MoveAbsoluteX((LONG)m_dmtrCurrPR1Posn.x, GMP_WAIT)) == GMP_SUCCESS)
					)
				{
					m_qSubOperation = ACF_WH_DO_PR1_Q;	// go to 4
				}
			}
		}
		else
		{
			if (
				((m_nLastError = MoveYToPR1Posn(g_stACF[m_lCurACF].lEdgeInfo, GMP_WAIT)) == GMP_SUCCESS) &&
				((m_nLastError = pCInspOpt->MoveAbsoluteX((LONG)m_dmtrCurrPR1Posn.x, GMP_WAIT)) == GMP_SUCCESS)
				)
			{
				m_qSubOperation = ACF_WH_DO_PR1_Q;	// go to 4
			}
		}
		break;

	case ACF_WH_DO_PR2_Q:						// 6
		Sleep(m_lPRDelay);
		if (m_bStopCycle)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
		}
		else if (!AutoSearchPR2GrabOnly(&pruACF2))
		{
			m_qSubOperation = ACF_WH_REDO_PR2_Q;	// go to 7
		}
		else
		{
			if (!AutoSearchPR2ProcessOnly(&pruACF2))
			{
				//if (g_bIsUserSearchPRDone)
				//{
				//	ACF_WH_Status[m_lCurSlave] = ST_REQ_CLICK_PR;
				//	m_qSubOperation = ACF_WH_WAIT_WINEAGE_ACCEPT_PR2_Q;	// go to 5
				//}
				//else
				{
					m_qSubOperation = ACF_WH_REDO_PR2_Q;	// go to 7
				}
			}
			else
			{
				m_bPRError = FALSE;
				m_lPRRetryCounter = 0;

				//lNextEdge = GetNextEdge(g_stACF[m_lCurACF].lEdgeInfo, g_stACF);
				//if (
				//	lNextEdge == EDGE_NONE ||						// All ACF On Glass Bonded
				//	lNextEdge != g_stACF[m_lCurACF].lEdgeInfo		// All ACF On Edge Bonded
				//)
				//{
				//	m_qSubOperation = ACF_WH_POSTBOND_CHECK_Q;	// go to 12
				//}
				//else
				{
					//ACF_WH_Status[m_lCurSlave] = ST_REQ_BONDING;
					ACF_WH_Status[m_lCurSlave] = ST_PR_DONE;

					//LONG lPRLevel = 0;
					//DOUBLE dOffset		= (g_stGlass.dThickness3 + g_stGlass.dThickness4) * 1000.0 + m_lPRLevelOffset;
					//if (CalcWHAbsoluteLevel(m_lCalibStageLevel, DoubleToLong(dOffset), m_stWHCalibZ, &lPRLevel))
					//{
					//	if ((fabs(lPRLevel - m_lStandbyLevel) <= 5.0) || ((m_nLastError = MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_NOWAIT)) == GMP_SUCCESS))
					//	{
					//		m_qSubOperation = ACF_WH_WAIT_TO_BOND_POSN_Q;	// go to 9
					//	}
					//}

					//m_qSubOperation = ACF_WH_WAIT_TO_BOND_POSN_Q;	// go to 9
					m_qSubOperation = ACF_WH_WAIT_OPTIC_ACK_Q;	// go to 29
				}
			}
		}
		break;

	case ACF_WH_WAIT_OPTIC_ACK_Q:		//29
		if (m_bStopCycle)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 21
		}
		else if(
				((m_lCurSlave == ACFWH_1) && (INSPOPT_Status != ST_ACFWH1_REQ_PR_ACK)) ||
				((m_lCurSlave == ACFWH_2) && (INSPOPT_Status != ST_ACFWH2_REQ_PR_ACK)) 
			)
		{
				ACF_WH_Status[m_lCurSlave] = ST_REQ_BONDING;
				
				//if (
				//	(m_nLastError = MoveY(Pre Bond Y Posn, GMP_WAIT)) == GMP_SUCCESS //faster Y
				//	)
				{
					m_qSubOperation = ACF_WH_WAIT_TO_BOND_POSN_Q;	// go to 9
				}
		}

		break;


	case ACF_WH_REDO_PR2_Q:						// 7
		m_bPRError = TRUE;
		//retry PR
		if (m_lPRRetryLimit != 0)
		{
			m_lPRRetryCounter++;

			if (m_lPRRetryCounter >= m_lPRRetryLimit)
			{
				// update stats
				pAppMod->m_ulACFWHPRError++;
				pAppMod->m_ulTotalACFWHPRError++;

				//reject glass
				if (m_lRejectGlassLimit != 0)
				{
					m_lRejectGlassCounter++;
					m_lPRRetryCounter = 0;

					// ERROR!
					if (m_lRejectGlassCounter >= m_lRejectGlassLimit)
					{
						m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
						if(m_lCurSlave == ACFWH_1)
						{
							WriteHMIMess("*** ERROR: ACF1WH Reject Glass Limit Hit ***", TRUE);
							WriteHMIMess("*** ERROR: ACF1WH PR 2 Error ***", TRUE);
							m_bModError = TRUE;
							SetError(IDS_ACF1WH_REJECT_GLASS_LIMIT_HIT);
							SetError(IDS_ACF1WH_PR2_ERR);
						}
						else
						{
							WriteHMIMess("*** ERROR: ACF2WH Reject Glass Limit Hit ***", TRUE);
							WriteHMIMess("*** ERROR: ACF2WH PR 2 Error ***", TRUE);
							m_bModError = TRUE;
							SetError(IDS_ACF2WH_REJECT_GLASS_LIMIT_HIT);
							SetError(IDS_ACF2WH_PR2_ERR);
						}
					}
					else
					{
						ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
						m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
						if(m_lCurSlave == ACFWH_1)
						{
							WriteHMIMess("*** ACF1WH PR 2 Error: Rejecting Glass... ***", TRUE);
						}
						else
						{
							WriteHMIMess("*** ACF2WH PR 2 Error: Rejecting Glass... ***", TRUE);
						}
					}
				}
				else
				{
					ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
					m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
					if(m_lCurSlave == ACFWH_1)
					{
						WriteHMIMess("*** ACF1WH PR 2 Error: Rejecting Glass... ***", TRUE);
					}
					else
					{
						WriteHMIMess("*** ACF2WH PR 2 Error: Rejecting Glass... ***", TRUE);
					}

				}
			}
			else
			{
				m_qSubOperation = ACF_WH_DO_PR2_Q;	// go to 6
			}
		}
		else
		{
			m_qSubOperation = ACF_WH_DO_PR2_Q;	// go to 6
		}
		break;

	case ACF_WH_WAIT_TO_BOND_POSN_Q:			// 9

		if (ACF_Status[ACF_1] == ST_STOP)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
		}
		else if (
				ACF_Status[ACF_1] == ST_MOD_NOT_SELECTED
				)
		{
			//CalcBondOffset();
			ACF_WH_Status[m_lCurSlave] = ST_BONDING;
			m_qSubOperation = ACF_WH_WAIT_ACF_COMPLETE_Q;	// go to 10
		}
		else if (
				((m_lCurSlave == ACFWH_1) && (ACF_Status[ACF_1] == ST_ACFWH1_REQ_ACF_ACK)) || 
				((m_lCurSlave == ACFWH_2) && (ACF_Status[ACF_1] == ST_ACFWH2_REQ_ACF_ACK)) /*||
				ACF_Status[ACF_1] == ST_MOD_NOT_SELECTED*/
				)
		{
			if (g_stACF[m_lCurACF].bBonded)
			{
				DisplayMessage("*******************************************************************");
				szMsg.Format("ACF%ld Bonded. Skipping ACF%ld", m_lCurACF + 1, m_lCurACF + 1);
				DisplayMessage(szMsg);
				DisplayMessage("*******************************************************************");

				if (
					g_stACF[m_lCurACF].lEdgeInfo == g_stACF[(m_lCurACF + 1) % MAX_NUM_OF_ACF].lEdgeInfo &&
					g_stACF[(m_lCurACF + 1) % MAX_NUM_OF_ACF].bEnable
				)
				{
					m_lCurACF = (m_lCurACF + 1) % MAX_NUM_OF_ACF;
				}
				else
				{
					m_qSubOperation = ACF_WH_POSTBOND_CHECK_Q;	// go to 12
				}
			}
			else
			{
				CalcBondOffset();
				if (!m_bPRSelected)
				{
					m_lTOffset = 0;
					m_mtrBondOffset.x = 0;
					m_mtrBondOffset.y = 0;
				}
				//CalcBondPt((ACFNum)m_lCurACF);
				// ang correction
//				m_dBondAngle = (m_dCurrAlignAngle + m_dACFAngX - g_stACF[m_lCurACF].stBondOffset.Theta);
//
//				lStep = (LONG)(m_dBondAngle * m_stMotorT.stAttrib.dEncoderScale) + GetStandbyPosnT(g_stACF[m_lCurACF].lEdgeInfo);
//
//				//p20121108
//				// may need to use this following line in case the Theta accuracy of the above line has been compromised.
//				//lStep = AngleToCount(m_dBondAngle, m_stMotorT.stAttrib.dDistPerCount) + GetStandbyPosnT(g_stACF[m_lCurACF].lEdgeInfo);
//
//				//p20121108:end
//#if 1 //20151023
//				m_dUnloadGlassCompensationAngle[g_stACF[m_lCurACF].lEdgeInfo] = m_dBondAngle/*m_dCurrAlignAngle*/;
//#endif
//
//				// current align centre
//				mtrTemp.x = DoubleToLong(m_dmtrCurrAlignCentre.x);
//				mtrTemp.y = DoubleToLong(m_dmtrCurrAlignCentre.y);
//				// compensate m_dmtrCurrAlignCentre with COR
//				CalPosnAfterRotate(g_stACF[m_lCurACF].lEdgeInfo, mtrTemp, m_dBondAngle, &mtrTemp);
//				m_dmtrCurrAlignCentre.x = (DOUBLE)mtrTemp.x;
//				m_dmtrCurrAlignCentre.y = (DOUBLE)mtrTemp.y;
//				mtrTemp.x = DoubleToLong(m_dmtrCurrAlignCentre.x + m_dmtrBondPtOffset.x 
//										 + (g_stACF[m_lCurACF].dLength / 2.0) * pCInspOpt->m_stMotorX.stAttrib.dEncoderScale * cos((PI / 180.0) * m_dACFAngX) 
//										 - (g_stACF[m_lCurACF].dWidth / 2.0) * m_stMotorY.stAttrib.dEncoderScale * sin((PI / 180.0) * m_dACFAngX) 
//										 - ((DOUBLE)g_stACF[m_lCurACF].stBondOffset.x * pCInspOpt->m_stMotorX.stAttrib.dEncoderScale / 1000.0) 
//										);
//				mtrTemp.y = DoubleToLong(m_dmtrCurrAlignCentre.y + m_dmtrBondPtOffset.y 
//										 + (g_stACF[m_lCurACF].dLength / 2.0) * pCInspOpt->m_stMotorX.stAttrib.dEncoderScale * sin((PI / 180.0) * m_dACFAngX) 
//										 + (g_stACF[m_lCurACF].dWidth / 2.0) * m_stMotorY.stAttrib.dEncoderScale * cos((PI / 180.0) * m_dACFAngX) 
//										 - ((DOUBLE)g_stACF[m_lCurACF].stBondOffset.y * m_stMotorY.stAttrib.dEncoderScale / 1000.0) 
//										);

				//DOUBLE dPRLevel = m_lCalibStageLevel + (m_stMotorZ.stAttrib.dEncoderScale*(g_stGlass.dThickness3 + g_stGlass.dThickness4)) + m_lPRLevelOffset;

				if (
					//(/*(fabs(dPRLevel - (DOUBLE)m_lStandbyLevel) <= 5.0) || */((m_nLastError = MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) == GMP_SUCCESS)) &&
					//((m_nLastError = SyncZ()) == GMP_SUCCESS) &&				// NO_WAIT Move in PR2 State
					((m_nLastError = MoveXYToBondPosn(m_lCurACF)) == GMP_SUCCESS) &&  
					((m_nLastError = MoveAbsoluteT(m_lStandbyPosnT[m_lCurACF] + m_lTOffset, GMP_NOWAIT)) == GMP_SUCCESS) && 
					((m_nLastError = SyncT()) == GMP_SUCCESS) && 
					((m_nLastError = m_pCACF->SyncX()) == GMP_SUCCESS) && 
					((m_nLastError = SyncY()) == GMP_SUCCESS) //&& 
					//((m_nLastError = MoveZ(ACF_WH_Z_CTRL_GO_BOND_LEVEL, GMP_WAIT)) == GMP_SUCCESS) 
					)
				{
					//if (HMI_bDebugCal)
					//{
					//	stPrintData.dData[0]	= 0.0;
					//	stPrintData.dData[1]	= m_dmtrCurrAlignCentre.x;
					//	stPrintData.dData[2]	= m_dmtrCurrAlignCentre.y;
					//	stPrintData.dData[3]	= m_dCurrAlignAngle;
					//	stPrintData.dData[4]	= m_dCurrAlignDist / pCInspOpt->m_stMotorX.stAttrib.dEncoderScale;
					//	stPrintData.dData[5]	= m_dmtrCurrPR1Posn.x;
					//	stPrintData.dData[6]	= m_dmtrCurrPR1Posn.y;
					//	stPrintData.dData[7]	= pruACF1.fAng;
					//	stPrintData.dData[8]	= m_dmtrCurrPR2Posn.x;
					//	stPrintData.dData[9]	= m_dmtrCurrPR2Posn.y;
					//	stPrintData.dData[10]	= pruACF2.fAng;

					//	pCHouseKeeping->PrintData("Calculation Log\\ACF_WH_PRLog", stPrintData);
					//}

					//if (m_lCurACF == ACF8 || !g_stACF[m_lCurACF + 1].bEnable)
					//{
					//	m_bLastBonding = TRUE;
					//}
					//LogAction("ACF_WH_Status[m_lCurSlave] = ST_REQ_ACF");
					ACF_WH_Status[m_lCurSlave] = ST_BONDING;
					m_qSubOperation = ACF_WH_WAIT_ACF_COMPLETE_Q;	// go to 10
				}
			}
		}
		break;

	case ACF_WH_WAIT_ACF_COMPLETE_Q:			// 10
		if (ACF_Status[ACF_1] == ST_STOP)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
		}
		else if (
				((m_lCurSlave == ACFWH_1) && (ACF_Status[ACF_1] == ST_ACFWH1_DONE)) || 
				((m_lCurSlave == ACFWH_2) && (ACF_Status[ACF_1] == ST_ACFWH2_DONE)) || 
				ACF_Status[ACF_1] == ST_MOD_NOT_SELECTED
				)
		{
			if (ACF_Status[ACF_1] == ST_MOD_NOT_SELECTED)
			{
				g_stACF[m_lCurACF].bBonded = TRUE;
			}
			pAppMod->m_ulUnitsBonded++;
			pAppMod->m_ulTotalUnitsBonded++;

			ACF_WH_Status[m_lCurSlave] = ST_BUSY;
			//LogAction("ACF_WH_Status = ST_BUSY");

			//if ((m_nLastError = MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_NOWAIT)) == GMP_SUCCESS)
			{
				m_qSubOperation = ACF_WH_CHECK_LAST_BONDED_ON_EDGE_Q;	// go to 11
			}
		}
		break;

	case ACF_WH_CHECK_LAST_BONDED_ON_EDGE_Q:	// 11
		lNextEdge = GetNextEdge(g_stACF[m_lCurACF].lEdgeInfo, g_stACF);
		m_lCurEdge = lNextEdge;
		if (
			lNextEdge == EDGE_NONE ||						// All ACF On Glass Bonded
			lNextEdge != g_stACF[m_lCurACF].lEdgeInfo		// All ACF On Edge Bonded
		)
		{
			m_qSubOperation = ACF_WH_POSTBOND_CHECK_Q;	// go to 12
		}
		else
		{
			// Bond Next ACF On the same Edge
			m_lCurACF = (m_lCurACF + 1) % MAX_NUM_OF_ACF;
			m_qSubOperation = ACF_WH_WAIT_TO_BOND_POSN_Q;	// go to 9
		}
		break;

	case ACF_WH_POSTBOND_CHECK_Q:				// 12
		if (m_bPBEnable)
		{
			ACF_WH_XY_CTRL_SIGNAL lTempCtrl;

			//m_lCurACF = FindFirstUnit((EdgeNum)g_stACF[m_lCurACF].lEdgeInfo); //multi ACF in Multi Edge
			m_lCurACF = ACF1;	// hard set to single ACF
			//CalcBondPt((ACFNum)m_lCurACF);

			// Rotate Theta for PostBond Searching
			//m_dBondAngle = (m_dCurrAlignAngle - g_stACF[m_lCurACF].stBondOffset.Theta);
			//lStep = AngleToCount(m_dBondAngle, m_stMotorT.stAttrib.dDistPerCount) + GetStandbyPosnT(g_stACF[m_lCurACF].lEdgeInfo);
			DOUBLE dAngle = 1 * (m_dCurrAlignAngle - m_dBondOffsetT);

			m_lPBInspectionPt = PB_INSP_BOTH;
			switch (m_lPBInspectionPt)
			{
			case PB_INSP_BOTH:
			case PB_INSP_PT1:
				mtrTemp.x = m_mtrPBPR1Posn.x;
				mtrTemp.y = m_mtrPBPR1Posn.y;
				//CalPosnAfterRotate(g_stACF[m_lCurACF].lEdgeInfo, mtrTemp, -dAngle, &mtrTemp);
				m_dmtrCurrPBPR1Posn.x = (DOUBLE)mtrTemp.x;
				m_dmtrCurrPBPR1Posn.y = (DOUBLE)mtrTemp.y;

				mtrTemp.x = m_mtrPBPR2Posn.x;
				mtrTemp.y = m_mtrPBPR2Posn.y;
				//CalPosnAfterRotate(g_stACF[m_lCurACF].lEdgeInfo, mtrTemp, -dAngle, &mtrTemp);
				m_dmtrCurrPBPR2Posn.x = (DOUBLE)mtrTemp.x;
				m_dmtrCurrPBPR2Posn.y = (DOUBLE)mtrTemp.y;

				lTempCtrl = ACF_WH_XY_CTRL_GO_PBPR1_POSN;
				break;

			case PB_INSP_PT2:
				mtrTemp.x = m_mtrPBPR2Posn.x;
				mtrTemp.y = m_mtrPBPR2Posn.y;
				//CalPosnAfterRotate(g_stACF[m_lCurACF].lEdgeInfo, mtrTemp, -dAngle, &mtrTemp);
				m_dmtrCurrPBPR2Posn.x = (DOUBLE)mtrTemp.x;
				m_dmtrCurrPBPR2Posn.y = (DOUBLE)mtrTemp.y;

				lTempCtrl = ACF_WH_XY_CTRL_GO_PBPR2_POSN;
				break;

			default:
				mtrTemp.x = (LONG)m_dmtrCurrPBPR1Posn.x;
				mtrTemp.y = (LONG)m_dmtrCurrPBPR1Posn.y;
				//CalPosnAfterRotate(g_stACF[m_lCurACF].lEdgeInfo, mtrTemp, -dAngle, &mtrTemp);
				m_dmtrCurrPBPR1Posn.x = (DOUBLE)mtrTemp.x;
				m_dmtrCurrPBPR1Posn.y = (DOUBLE)mtrTemp.y;

				lTempCtrl = ACF_WH_XY_CTRL_GO_PBPR1_POSN;
			}

			//LONG lPRLevel = 0;
			//DOUBLE dThickness = g_stGlass.dThickness3 + g_stGlass.dThickness4;
			//DOUBLE dOffset		= dThickness * 1000.0 + m_lPRLevelOffset;
			//BOOL bResult	= CalcWHAbsoluteLevel(m_lCalibStageLevel, DoubleToLong(dOffset), m_stWHCalibZ, &lPRLevel);

			if (
				//(bResult == TRUE) &&
				//((m_nLastError = SyncZ()) == GMP_SUCCESS) &&
				((m_nLastError = PBMoveY(lTempCtrl, GMP_NOWAIT)) == GMP_SUCCESS) //&& 
				//((m_nLastError = MoveAbsoluteT(lStep, GMP_NOWAIT)) == GMP_SUCCESS) &&
				//((m_nLastError = pCInspOpt->SyncX()) == GMP_SUCCESS) &&	
				//((m_nLastError = SyncY()) == GMP_SUCCESS) &&
				//((m_nLastError = SyncT()) == GMP_SUCCESS) &&
				//((fabs(lPRLevel - m_lStandbyLevel) <= 5.0) || ((m_nLastError = MoveZ(ACF_WH_Z_CTRL_GO_PR_LEVEL, GMP_NOWAIT)) == GMP_SUCCESS)) &&
				//((m_nLastError = SyncZ()) == GMP_SUCCESS)
			)
			{
				//switch (m_lPBInspectionPt)
				//{
				//case PB_INSP_BOTH:
				//case PB_INSP_PT1:
				//	m_qSubOperation = ACF_WH_DO_POST_PR1_Q;	// go to 13
				//	break;

				//case PB_INSP_PT2:
				//	m_qSubOperation = ACF_WH_DO_POST_PR2_Q;	// go to 15
				//	break;

				//default:
				//	m_qSubOperation = ACF_WH_DO_POST_PR1_Q;	// go to 13
				//}		

				
				ACF_WH_Status[m_lCurSlave] = ST_REQ_PR;		//ask Permission
				m_qSubOperation = ACF_WH_WAIT_OPTIC_PB_INSP_PERMISSION_Q;	// go to 30

			}
		}
		else	// No Post Bond Check
		{
			m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 3
		}
		break;

	case ACF_WH_WAIT_OPTIC_PB_INSP_PERMISSION_Q:		//30
		if (m_bStopCycle)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 21
		}
		else if(
				((m_lCurSlave == ACFWH_1) && (INSPOPT_Status == ST_ACFWH1_REQ_PR_ACK)) ||
				((m_lCurSlave == ACFWH_2) && (INSPOPT_Status == ST_ACFWH2_REQ_PR_ACK)) 
			)
		{
				ACF_WH_Status[m_lCurSlave] = ST_PB_PR_IN_PROCESS;
				
				switch (m_lPBInspectionPt)
				{
					case PB_INSP_BOTH:
					case PB_INSP_PT1:
						if (
							((m_nLastError = pCInspOpt->MoveAbsoluteX((LONG)m_dmtrCurrPBPR1Posn.x, GMP_NOWAIT)) == GMP_SUCCESS)
							)
						{
							m_qSubOperation = ACF_WH_DO_POST_PR1_Q;	// go to 13
						}
					break;

					case PB_INSP_PT2:
						if (
							((m_nLastError = pCInspOpt->MoveAbsoluteX((LONG)m_dmtrCurrPBPR2Posn.x, GMP_NOWAIT)) == GMP_SUCCESS)
							)
						{
							m_qSubOperation = ACF_WH_DO_POST_PR2_Q;	// go to 15
						}
						break;

					default:	//PB PR1
						if (
							((m_nLastError = pCInspOpt->MoveAbsoluteX((LONG)m_dmtrCurrPBPR1Posn.x, GMP_NOWAIT)) == GMP_SUCCESS)
							)
						{
							m_qSubOperation = ACF_WH_DO_POST_PR1_Q;	// go to 13
						}
				}
		}

		break;


	case ACF_WH_DO_POST_PR1_Q:					// 13
		if (
			((m_nLastError = pCInspOpt->SyncX()) == GMP_SUCCESS) &&	
			((m_nLastError = SyncY()) == GMP_SUCCESS) 
			)
		{
			Sleep(m_lPRDelay2);
		}
		if (m_bStopCycle)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
		}
		else if (!AutoSearchPBPR1GrabOnly(m_lCurACF))
		{
			m_qSubOperation = ACF_WH_REDO_POST_PR1_Q;	// go to 14
		}
		else
		{
			if (m_lPBInspectionPt == PB_INSP_PT1)
			{
				if (!AutoSearchPBPR1ProcessOnly(m_lCurACF))
				{
					m_qSubOperation = ACF_WH_REDO_POST_PR1_Q;	// go to 14
				}
				else
				{
					m_bPRError = FALSE;
					m_bPBError = FALSE;
					m_lPRRetryCounter = 0;

					m_qSubOperation = ACF_WH_CHECK_ACF_LENGTH_TOL_Q;	// go to 20
				}
			}
			else
			{
				mtrTemp.x = (LONG)m_dmtrCurrPBPR2Posn.x;
				mtrTemp.y = (LONG)m_dmtrCurrPBPR2Posn.y;
				//CalPosnAfterRotate(g_stACF[m_lCurACF].lEdgeInfo, mtrTemp, m_dBondAngle, &mtrTemp);
				m_dmtrCurrPBPR2Posn.x = (DOUBLE)mtrTemp.x;
				m_dmtrCurrPBPR2Posn.y = (DOUBLE)mtrTemp.y;

				if (m_lNumOfPB_MP <= 0)
				{
					if ((m_nLastError = OpticMoveXY(ACF_WH_XY_CTRL_GO_PBPR2_POSN, GMP_NOWAIT)) == GMP_SUCCESS)
					{
						if (
							AutoSearchPBPR1ProcessOnly(m_lCurACF) &&
							((m_nLastError = pCInspOpt->SyncX()) == GMP_SUCCESS) &&
							((m_nLastError = SyncY()) == GMP_SUCCESS)
						)
						{
							m_bPRError = FALSE;
							m_bPBError = FALSE;
							m_lPRRetryCounter = 0;
							m_qSubOperation = ACF_WH_DO_POST_PR2_Q;	// go to 15
						}
						else
						{
							m_qSubOperation = ACF_WH_REDO_POST_PR1_Q;	// go to 14
						}
					}
				}
				else
				{
					if (AutoSearchPBPR1ProcessOnly(m_lCurACF))
					{
						m_bPRError = FALSE;
						m_bPBError = FALSE;
						m_lPRRetryCounter = 0;
						m_qSubOperation = ACF_WH_DO_POST_MP1_Q;	// go to 17
					}
					else
					{
						m_qSubOperation = ACF_WH_REDO_POST_PR1_Q;	// go to 14
					}

				}
			}
		}
		break;

	case ACF_WH_REDO_POST_PR1_Q:				// 14
		m_bPRError = TRUE;
		//retry PR
		if (m_lPRRetryLimit != 0)
		{
			m_lPRRetryCounter++;

			if (m_lPRRetryCounter >= m_lPRRetryLimit)
			{
				// update stats
				pAppMod->m_ulACFWHPBPRError++;
				pAppMod->m_ulTotalACFWHPBPRError++;

				//reject glass
				if (m_lRejectGlassLimit != 0)
				{
					m_lRejectGlassCounter++;
					m_lPRRetryCounter = 0;

					// ERROR!
					if (m_lRejectGlassCounter >= m_lRejectGlassLimit)
					{
						if(m_lCurSlave == ACFWH_1)
						{
							m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
							WriteHMIMess("*** ERROR: ACF1WH Reject Glass Limit Hit ***", TRUE);
							WriteHMIMess("*** ERROR: ACF1WH Post PR 1 Error ***", TRUE);
							m_bModError = TRUE;
							SetError(IDS_ACF1WH_REJECT_GLASS_LIMIT_HIT);
							SetError(IDS_ACF1WH_PBPR_ERR);
						}
						else
						{
							m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
							WriteHMIMess("*** ERROR: ACF2WH Reject Glass Limit Hit ***", TRUE);
							WriteHMIMess("*** ERROR: ACF2WH Post PR 1 Error ***", TRUE);
							m_bModError = TRUE;
							SetError(IDS_ACF2WH_REJECT_GLASS_LIMIT_HIT);
							SetError(IDS_ACF2WH_PBPR_ERR);
						}
					}
					else
					{
						ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
						m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
						if(m_lCurSlave == ACFWH_1)
						{
							WriteHMIMess("*** ACF1WH Post PR 1 Error: Rejecting Glass... ***", TRUE);
						}
						else
						{
							WriteHMIMess("*** ACF2WH Post PR 1 Error: Rejecting Glass... ***", TRUE);
						}
					}
				}
				else
				{
					ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
					m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
						if(m_lCurSlave == ACFWH_1)
						{
							WriteHMIMess("*** ACF1WH Post PR 1 Error: Rejecting Glass... ***", TRUE);
						}
						else
						{
							WriteHMIMess("*** ACF2WH Post PR 1 Error: Rejecting Glass... ***", TRUE);
						}
				}
			}
			else
			{
				if ((m_nLastError = OpticMoveXY(ACF_WH_XY_CTRL_GO_PBPR1_POSN, GMP_WAIT)) == GMP_SUCCESS)
				{
					m_qSubOperation = ACF_WH_DO_POST_PR1_Q;	// go to 13
				}
			}
		}
		else
		{
			if ((m_nLastError = OpticMoveXY(ACF_WH_XY_CTRL_GO_PBPR1_POSN, GMP_WAIT)) == GMP_SUCCESS)
			{
				m_qSubOperation = ACF_WH_DO_POST_PR1_Q;	// go to 13
			}
		}
		break;

	case ACF_WH_DO_POST_PR2_Q:					// 15
		Sleep(m_lPRDelay2);
		if (m_bStopCycle)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
		}
		else if (!AutoSearchPBPR2GrabOnly(m_lCurACF))
		{
			m_qSubOperation = ACF_WH_REDO_POST_PR2_Q;	// go to 16
		}
		else
		{
			if (!AutoSearchPBPR2ProcessOnly(m_lCurACF))
			{
				m_qSubOperation = ACF_WH_REDO_POST_PR2_Q;	// go to 16
			}
			else
			{
				m_bPRError = FALSE;
				m_bPBError = FALSE;
				m_lPRRetryCounter = 0;

				m_qSubOperation = ACF_WH_CHECK_ACF_LENGTH_TOL_Q;	// go to 20
			}
		}
		break;

	case ACF_WH_REDO_POST_PR2_Q:				// 16
		m_bPRError = TRUE;
		//retry PR
		if (m_lPRRetryLimit != 0)
		{
			m_lPRRetryCounter++;

			if (m_lPRRetryCounter >= m_lPRRetryLimit)
			{
				// update stats
				pAppMod->m_ulACFWHPBPRError++;
				pAppMod->m_ulTotalACFWHPBPRError++;

				//reject glass
				if (m_lRejectGlassLimit != 0)
				{
					m_lRejectGlassCounter++;
					m_lPRRetryCounter = 0;

					// ERROR!
					if (m_lRejectGlassCounter >= m_lRejectGlassLimit)
					{
						if(m_lCurSlave == ACFWH_1)
						{
							m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
							WriteHMIMess("*** ERROR: ACF1WH Reject Glass Limit Hit ***", TRUE);
							WriteHMIMess("*** ERROR: ACF1WH Post PR 2 Error ***", TRUE);
							m_bModError = TRUE;
							SetError(IDS_ACF1WH_REJECT_GLASS_LIMIT_HIT);
							SetError(IDS_ACF1WH_PBPR_ERR);
						}
						else
						{
							m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
							WriteHMIMess("*** ERROR: ACF2WH Reject Glass Limit Hit ***", TRUE);
							WriteHMIMess("*** ERROR: ACF2WH Post PR 2 Error ***", TRUE);
							m_bModError = TRUE;
							SetError(IDS_ACF2WH_REJECT_GLASS_LIMIT_HIT);
							SetError(IDS_ACF2WH_PBPR_ERR);
						}
					}
					else
					{
						ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
						m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
						if(m_lCurSlave == ACFWH_1)
						{
							WriteHMIMess("*** ACF1WH Post PR 2 Error: Rejecting Glass... ***", TRUE);
						}
						else
						{
							WriteHMIMess("*** ACF2WH Post PR 2 Error: Rejecting Glass... ***", TRUE);
						}
					}
				}
				else
				{
					ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
					m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
					if(m_lCurSlave == ACFWH_1)
					{
						WriteHMIMess("*** ACF1WH Post PR 2 Error: Rejecting Glass... ***", TRUE);
					}
					else
					{
						WriteHMIMess("*** ACF2WH Post PR 2 Error: Rejecting Glass... ***", TRUE);
					}
				}
			}
			else
			{
				if ((m_nLastError = OpticMoveXY(ACF_WH_XY_CTRL_GO_PBPR2_POSN, GMP_WAIT)) == GMP_SUCCESS)
				{
					m_qSubOperation = ACF_WH_DO_POST_PR2_Q;	// go to 15
				}
			}
		}
		else
		{
			if ((m_nLastError = OpticMoveXY(ACF_WH_XY_CTRL_GO_PBPR2_POSN, GMP_WAIT)) == GMP_SUCCESS)
			{
				m_qSubOperation = ACF_WH_DO_POST_PR2_Q;	// go to 15
			}
		}
		break;

	case ACF_WH_DO_POST_MP1_Q:					// 17
		if (m_bStopCycle)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
		}
		else if (!AutoSearchPB_MPPR1())
		{
			m_bPRError = TRUE;
			//retry PR
			if (m_lPRRetryLimit != 0)
			{
				m_lPRRetryCounter++;

				if (m_lPRRetryCounter >= m_lPRRetryLimit)
				{
					// update stats
					pAppMod->m_ulACFWHPBPRError++;
					pAppMod->m_ulTotalACFWHPBPRError++;

					//reject glass
					if (m_lRejectGlassLimit != 0)
					{
						m_lRejectGlassCounter++;
						m_lPRRetryCounter = 0;

						// ERROR!
						if (m_lRejectGlassCounter >= m_lRejectGlassLimit)
						{
							if(m_lCurSlave == ACFWH_1)
							{
								m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
								WriteHMIMess("*** ERROR: ACF1WH Reject Glass Limit Hit ***", TRUE);
								WriteHMIMess("*** ERROR: ACF1WH Post PR MP 1 Error ***", TRUE);
								m_bModError = TRUE;
								SetError(IDS_ACF1WH_REJECT_GLASS_LIMIT_HIT);
								SetError(IDS_ACF1WH_PBPR_ERR);
							}
							else
							{
								m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
								WriteHMIMess("*** ERROR: ACF2WH Reject Glass Limit Hit ***", TRUE);
								WriteHMIMess("*** ERROR: ACF2WH Post PR MP 1 Error ***", TRUE);
								m_bModError = TRUE;
								SetError(IDS_ACF2WH_REJECT_GLASS_LIMIT_HIT);
								SetError(IDS_ACF2WH_PBPR_ERR);
							}
						}
						else
						{
							ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
							m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
							if(m_lCurSlave == ACFWH_1)
							{
								WriteHMIMess("*** ACF1WH Post PR MP 1 Error: Rejecting Glass... ***", TRUE);
							}
							else
							{
								WriteHMIMess("*** ACF2WH Post PR MP 1 Error: Rejecting Glass... ***", TRUE);
							}
						}
					}
					else
					{
						ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
						m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
						if(m_lCurSlave == ACFWH_1)
						{
							WriteHMIMess("*** ACF1WH Post PR MP 1 Error: Rejecting Glass... ***", TRUE);
						}
						else
						{
							WriteHMIMess("*** ACF2WH Post PR MP 1 Error: Rejecting Glass... ***", TRUE);
						}
					}
				}
				else
				{
					m_qSubOperation = ACF_WH_DO_POST_MP1_Q;	// go to 17
				}
			}
			else
			{
				m_qSubOperation = ACF_WH_DO_POST_MP1_Q;	// go to 17
			}
		}
		else
		{
			m_bPRError = FALSE;
			m_bPBError = FALSE;
			m_lPRRetryCounter = 0;

			if (m_lNumOfPB_MP >= 2)
			{
				m_qSubOperation = ACF_WH_DO_POST_MP2_Q;
			}
			else
			{
				if ((m_nLastError = OpticMoveXY(ACF_WH_XY_CTRL_GO_PBPR2_POSN, GMP_WAIT)) == GMP_SUCCESS)
				{
					m_qSubOperation = ACF_WH_DO_POST_PR2_Q;	// go to 15
				}
			}
		}
		break;

	case ACF_WH_DO_POST_MP2_Q:					// 18
		if (m_bStopCycle)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
		}
		else if (!AutoSearchPB_MPPR2())
		{
			m_bPRError = TRUE;
			//retry PR
			if (m_lPRRetryLimit != 0)
			{
				m_lPRRetryCounter++;

				if (m_lPRRetryCounter >= m_lPRRetryLimit)
				{
					// update stats
					pAppMod->m_ulACFWHPBPRError++;
					pAppMod->m_ulTotalACFWHPBPRError++;

					//reject glass
					if (m_lRejectGlassLimit != 0)
					{
						m_lRejectGlassCounter++;
						m_lPRRetryCounter = 0;

						// ERROR!
						if (m_lRejectGlassCounter >= m_lRejectGlassLimit)
						{
							if(m_lCurSlave == ACFWH_1)
							{
								m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
								WriteHMIMess("*** ERROR: ACF1WH Reject Glass Limit Hit ***", TRUE);
								WriteHMIMess("*** ERROR: ACF1WH Post PR MP 2 Error ***", TRUE);
								m_bModError = TRUE;
								SetError(IDS_ACF1WH_REJECT_GLASS_LIMIT_HIT);
								SetError(IDS_ACF1WH_PBPR_ERR);
							}
							else
							{
								m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
								WriteHMIMess("*** ERROR: ACF2WH Reject Glass Limit Hit ***", TRUE);
								WriteHMIMess("*** ERROR: ACF2WH Post PR MP 2 Error ***", TRUE);
								m_bModError = TRUE;
								SetError(IDS_ACF2WH_REJECT_GLASS_LIMIT_HIT);
								SetError(IDS_ACF2WH_PBPR_ERR);
							}
						}
						else
						{
							ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
							m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
							if(m_lCurSlave == ACFWH_1)
							{
								WriteHMIMess("*** ACF1WH Post PR MP 2 Error: Rejecting Glass... ***", TRUE);
							}
							else
							{
								WriteHMIMess("*** ACF2WH Post PR MP 2 Error: Rejecting Glass... ***", TRUE);
							}
						}
					}
					else
					{
						ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
						m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
						if(m_lCurSlave == ACFWH_1)
						{
							WriteHMIMess("*** ACF1WH Post PR MP 2 Error: Rejecting Glass... ***", TRUE);
						}
						else
						{
							WriteHMIMess("*** ACF2WH Post PR MP 2 Error: Rejecting Glass... ***", TRUE);
						}
					}
				}
				else
				{
					m_qSubOperation = ACF_WH_DO_POST_MP2_Q;	// go to 18
				}
			}
			else
			{
				m_qSubOperation = ACF_WH_DO_POST_MP2_Q;	// go to 18
			}
		}
		else
		{
			m_bPRError = FALSE;
			m_bPBError = FALSE;
			m_lPRRetryCounter = 0;

			if ((m_nLastError = OpticMoveXY(ACF_WH_XY_CTRL_GO_PBPR2_POSN, GMP_WAIT)) == GMP_SUCCESS)
			{
				m_qSubOperation = ACF_WH_DO_POST_PR2_Q;	// go to 15
			}
		}
		break;

	case ACF_WH_CHECK_ACF_LENGTH_TOL_Q:					// 20
		if (m_lPBInspectionPt == PB_INSP_BOTH)
		{
			// Check ACF Length
			if (
				(pruACFPB1[m_lCurACF][OBJECT].stLrnACFCmd.uwIsAlign && pruACFPB2[m_lCurACF][OBJECT].stLrnACFCmd.uwIsAlign) &&
				(m_dACFLengthUpperLmt != 0.0 || m_dACFLengthLowerLmt != 0.0) && 
				AutoMode != BURN_IN
			)
			{
				//check the ACF Accuracy;
				CalcACFAccuracy(m_dmtrCurrPBPR1Posn, m_dmtrCurrPBPR2Posn, HMI_bLogACFLength);
				//if (fabs(m_dCurrACFLength - g_stACF[m_lCurACF-1].dLength) > m_dACFLengthTol)//Remove me
				if (
					m_dCurrACFLength < (g_stACF[m_lCurACF].dLength - m_dACFLengthLowerLmt) || 
					m_dCurrACFLength > (g_stACF[m_lCurACF].dLength + m_dACFLengthUpperLmt)
				)
				{
					if (m_lRejectGlassLimit != 0)
					{
						m_lRejectGlassCounter++;

						// ERROR!
						if (m_lRejectGlassCounter >= m_lRejectGlassLimit)
						{
							if(m_lCurSlave == ACFWH_1)
							{
								m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
								WriteHMIMess("*** ERROR: ACF1WH Reject Glass Limit Hit ***", TRUE);
								WriteHMIMess("*** ERROR: ACF1WH ACF Length Tol. Error ***", TRUE);
								m_bModError = TRUE;
								SetError(IDS_ACF1WH_REJECT_GLASS_LIMIT_HIT);
								SetError(IDS_ACF1WH_ACF_LENGTH_TOL_ERR);
							}
							else
							{
								m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
								WriteHMIMess("*** ERROR: ACF2WH Reject Glass Limit Hit ***", TRUE);
								WriteHMIMess("*** ERROR: ACF2WH ACF Length Tol. Error ***", TRUE);
								m_bModError = TRUE;
								SetError(IDS_ACF2WH_REJECT_GLASS_LIMIT_HIT);
								SetError(IDS_ACF2WH_ACF_LENGTH_TOL_ERR);
							}
						}
						else
						{
							ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
							m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
							if(m_lCurSlave == ACFWH_1)
							{
								WriteHMIMess("*** Alert: ACF1WH Rejecting Invalid ACF Length Glass... ***", TRUE);
							}
							else
							{
								WriteHMIMess("*** Alert: ACF2WH Rejecting Invalid ACF Length Glass... ***", TRUE);
							}
						}
					}
					else
					{
						ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
						m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
						if(m_lCurSlave == ACFWH_1)
						{
							WriteHMIMess("*** Alert: ACF1WH Rejecting Invalid ACF Length Glass... ***", TRUE);
						}
						else
						{
							WriteHMIMess("*** Alert: ACF2WH Rejecting Invalid ACF Length Glass... ***", TRUE);
						}
					}
				}
			}
			else
			{
				m_qSubOperation = ACF_WH_CHECK_LAST_POST_PR_ON_EDGE_Q; // 19
			}
		}
		else
		{
			m_qSubOperation = ACF_WH_CHECK_LAST_POST_PR_ON_EDGE_Q;	// 19
		}
		break;

	case ACF_WH_CHECK_LAST_POST_PR_ON_EDGE_Q:	// 19
		if (
			// Check Enable
			g_stACF[(m_lCurACF + 1) % MAX_NUM_OF_ACF].bEnable &&
			// Check Finished (For All ACF on the same Edge)
			(m_lCurACF + 1 < MAX_NUM_OF_ACF) &&
			// Check Finished
			g_stACF[(m_lCurACF + 1) % MAX_NUM_OF_ACF].lEdgeInfo == g_stACF[m_lCurACF].lEdgeInfo
		)
		{
			m_lCurACF++;
			CalcBondPt((ACFNum)m_lCurACF);

			ACF_WH_XY_CTRL_SIGNAL lTempCtrl;

			switch (m_lPBInspectionPt)
			{
			case PB_INSP_BOTH:
			case PB_INSP_PT1:
				mtrTemp.x = (LONG)m_dmtrCurrPBPR1Posn.x;
				mtrTemp.y = (LONG)m_dmtrCurrPBPR1Posn.y;
				//CalPosnAfterRotate(g_stACF[m_lCurACF].lEdgeInfo, mtrTemp, m_dBondAngle, &mtrTemp);
				m_dmtrCurrPBPR1Posn.x = (DOUBLE)mtrTemp.x;
				m_dmtrCurrPBPR1Posn.y = (DOUBLE)mtrTemp.y;

				lTempCtrl = ACF_WH_XY_CTRL_GO_PBPR1_POSN;
				break;

			case PB_INSP_PT2:
				mtrTemp.x = (LONG)m_dmtrCurrPBPR2Posn.x;
				mtrTemp.y = (LONG)m_dmtrCurrPBPR2Posn.y;
				//CalPosnAfterRotate(g_stACF[m_lCurACF].lEdgeInfo, mtrTemp, m_dBondAngle, &mtrTemp);
				m_dmtrCurrPBPR2Posn.x = (DOUBLE)mtrTemp.x;
				m_dmtrCurrPBPR2Posn.y = (DOUBLE)mtrTemp.y;

				lTempCtrl = ACF_WH_XY_CTRL_GO_PBPR2_POSN;
				break;

			default:
				mtrTemp.x = (LONG)m_dmtrCurrPBPR2Posn.x;
				mtrTemp.y = (LONG)m_dmtrCurrPBPR2Posn.y;
				//CalPosnAfterRotate(g_stACF[m_lCurACF].lEdgeInfo, mtrTemp, m_dBondAngle, &mtrTemp);
				m_dmtrCurrPBPR2Posn.x = (DOUBLE)mtrTemp.x;
				m_dmtrCurrPBPR2Posn.y = (DOUBLE)mtrTemp.y;

				lTempCtrl = ACF_WH_XY_CTRL_GO_PBPR1_POSN;
			}

			if ((m_nLastError = OpticMoveXY(lTempCtrl, GMP_WAIT)) == GMP_SUCCESS)
			{
				switch (m_lPBInspectionPt)
				{
				case PB_INSP_PT1:
				case PB_INSP_BOTH:
					m_qSubOperation = ACF_WH_DO_POST_PR1_Q;	// go to 13
					break;

				case PB_INSP_PT2:
					m_qSubOperation = ACF_WH_DO_POST_PR2_Q;	// go to 15
					break;
				}
			}
		}
		else
		{
			// All ACF On Edge PostBond Checked
			// Bond Next Edge
			m_bPostBondChecked[g_stACF[m_lCurACF].lEdgeInfo] = TRUE;
			
			ACF_WH_Status[m_lCurSlave] = ST_PR_DONE;			//release Optic
			//m_qSubOperation = ACF_WH_MOVE_TO_CURRENT_EDGE_Q;	// go to 3
			m_qSubOperation = ACF_WH_WAIT_OPTIC_PB_INSP_ACK_Q;	// go to 31
		}
		break;

	case ACF_WH_WAIT_OPTIC_PB_INSP_ACK_Q:		//31
		if (m_bStopCycle)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
		}
		else if(
				((m_lCurSlave == ACFWH_1) && (INSPOPT_Status != ST_ACFWH1_REQ_PR_ACK)) ||
				((m_lCurSlave == ACFWH_2) && (INSPOPT_Status != ST_ACFWH2_REQ_PR_ACK)) 
			)
		{
			ACF_WH_Status[m_lCurSlave] = ST_BUSY;
			m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 3
		}

		break;



	case ACF_WH_GO_UNLOAD_GLASS_Q:				// 21
		// unload compensation
		lStep = 0;
		//Unload Posn XY
		mtrTemp.x = m_mtrUnloadGlassPosn.x;// + m_mtrUnloadGlassOffset.x;
		mtrTemp.y = m_mtrUnloadGlassPosn.y + DistanceToCount((DOUBLE)m_lUnloadYOffset, m_stMotorY.stAttrib.dDistPerCount);

		// Glass not to be rejected
		if (
			ACF_WH_Status[m_lCurSlave] != ST_PR_ERROR && 
			ACF_WH_Status[m_lCurSlave] != ST_ERROR
		)
		{
			// Do something
			bSafeMoveWait = GMP_NOWAIT;
		}

		// Fast indexing
		//ACF_WH_Z_CTRL_SIGNAL nTempCtrlZ;

		//if (g_bSafeIndexing)
		//{
		//	nTempCtrlZ = ACF_WH_Z_CTRL_GO_STANDBY_LEVEL;
		//}
		//else
		//{
		//	nTempCtrlZ = ACF_WH_Z_CTRL_GO_PRE_UNLOAD_LEVEL;
		//}

#if 1 //20151023
		m_bACFWHUnloadThetaCompensation = FALSE;
		if (m_bACFWHUnloadThetaCompensation)
		{
			DOUBLE dAngleAverage = 0.0;
			LONG lNumOfAngle = 0;
			for (INT ii = EDGE_A; ii < MAX_EDGE_OF_GLASS; ii++)
			{
				if (fabs(m_dUnloadGlassCompensationAngle[ii]) > 0.0001)
				{
					dAngleAverage += m_dUnloadGlassCompensationAngle[ii];
					lNumOfAngle++;
				}
			}
			if (lNumOfAngle > 0)
			{
				dAngleAverage = dAngleAverage / (DOUBLE)lNumOfAngle;
			}

			for (INT ii = EDGE_A; ii < MAX_EDGE_OF_GLASS; ii++)
			{
				szMsg.Format("ACFWH Unload T. Edge%d, T_degree:%.4f", ii + 1, m_dUnloadGlassCompensationAngle[ii]);
				DisplayMessage(szMsg);
			}
			for (INT ii = EDGE_A; ii < MAX_EDGE_OF_GLASS; ii++)
			{
				m_dUnloadGlassCompensationAngle[ii] = dAngleAverage;
			}

			szMsg.Format("ACFWH Unload T average_degree:%.4f, LastEdge:%d", dAngleAverage, EDGE_A /*GetLastEdge(m_lLoadGlassEdge, g_stACF)*/);
			DisplayMessage(szMsg);
		}
#endif

		if (
			//((m_nLastError = MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) == GMP_SUCCESS) && 
			//((m_nLastError = MoveRelativeT(-lStep, GMP_NOWAIT)) == GMP_SUCCESS) && 
//#if 1 //20151023
			//(m_bACFWHUnloadThetaCompensation || ((m_nLastError = MoveTToCurEdge(GetLastEdge(m_lLoadGlassEdge, g_stACF), TRUE, bSafeMoveWait)) == GMP_SUCCESS)) &&
			//(!m_bACFWHUnloadThetaCompensation || ((m_nLastError = MoveTToCurEdgeWithTCompensation(GetLastEdge(m_lLoadGlassEdge, g_stACF), TRUE, bSafeMoveWait)) == GMP_SUCCESS)) &&
			((m_nLastError = MoveAbsoluteY(mtrTemp.y, GMP_NOWAIT)) == GMP_SUCCESS) &&
//#else
//			((m_nLastError = MoveTToCurEdge(GetLastEdge(m_lLoadGlassEdge, g_stACF), TRUE, bSafeMoveWait)) == GMP_SUCCESS) &&
//			((m_nLastError = MoveAbsoluteXY(mtrTemp.x, mtrTemp.y, GMP_NOWAIT)) == GMP_SUCCESS) &&
//#endif
			(SleepWithReturn(200) == GMP_SUCCESS) && //20120619
			//((m_nLastError = MoveZ(nTempCtrlZ, GMP_NOWAIT)) == GMP_SUCCESS) && 
			//((m_nLastError = SyncX()) == GMP_SUCCESS) && 
			((m_nLastError = SyncY()) == GMP_SUCCESS) && 
			((m_nLastError = SyncT()) == GMP_SUCCESS) //&& 
			//((m_nLastError = SyncZ()) == GMP_SUCCESS) 
			)
		{			
			// process timer ends
			m_dProcessTime = (DOUBLE)(GetTickCount() - m_dwStartTime) / 1000.0;

			if (ACF_WH_Status[m_lCurSlave] == ST_PR_ERROR)
			{
				ACF_WH_Status[m_lCurSlave] = ST_PICK_ERR1_EMPTY2;
				ACF_GLASS_Status[m_lCurSlave][GLASS1] = ST_ERROR;
			}
			else
			{
				ACF_WH_Status[m_lCurSlave] = ST_PICK_READY1_EMPTY2;
				ACF_GLASS_Status[m_lCurSlave][GLASS1] = ST_READY;
				m_lRejectGlassCounter = 0;
			}
			m_qSubOperation = ACF_WH_WAIT_GLASS_PICKED_Q;		// go to 22
		}
		break;

	case ACF_WH_WAIT_GLASS_PICKED_Q:			// 22
		if (TA2_Status == ST_STOP)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
		}
		else if (
			ACF_WH_Status[m_lCurSlave] == ST_EMPTY || 
			TA2_Status == ST_MOD_NOT_SELECTED 
		)
		{
			m_bGlass1Exist = FALSE;
			ACF_GLASS_Status[m_lCurSlave][GLASS1] = ST_EMPTY;
			

			for (i = 0; i < MAX_NUM_OF_ACF; i++)
			{
				g_stACF[i].bBonded = FALSE;
			}

			if (AutoMode == DIAG_BOND || TA2_Status == ST_MOD_NOT_SELECTED)
			{
				Sleep(100);

				if (AutoMode != TEST_BOND && AutoMode != AUTO_BOND)
				{
					//SetWHVacSol(EDGE_A, OFF, GMP_WAIT);
					SetGLASS1_VacSol(OFF, GMP_WAIT);
				}

				//m_nLastError = MoveTToCurEdge(m_lLoadGlassEdge);
			}
			else
			{
				// Faster Move (No Glass on WH)
				ACF_WH_T_CTRL_SIGNAL lTempCtrl;

				switch (m_lLoadGlassEdge)
				{
				case EDGE_A:
					lTempCtrl = ACF_WH_T_CTRL_GO_STANDBY;
					break;
				//case EDGE_B:
				//	lTempCtrl = ACF_WH_T_CTRL_GO_EDGE_B;
				//	break;
				//case EDGE_C:
				//	lTempCtrl = ACF_WH_T_CTRL_GO_EDGE_C;
				//	break;
				//case EDGE_D:
				//	lTempCtrl = ACF_WH_T_CTRL_GO_EDGE_D;
				//	break;

				default:
					lTempCtrl = ACF_WH_T_CTRL_GO_STANDBY;
					break;
				}

				m_nLastError = MoveT(lTempCtrl, GMP_NOWAIT);
			}

			if (m_nLastError == GMP_SUCCESS)
			{
				m_lPrevEdge = m_lLoadGlassEdge;

				if (
					((m_nLastError = MoveY(ACF_WH_Y_CTRL_GO_LOAD_POSN_OFFSET, GMP_WAIT)) == GMP_SUCCESS) && 
					((m_nLastError = SyncT()) == GMP_SUCCESS)
				)
				{
					m_lCurEdge = m_lLoadGlassEdge;

					ACF_WH_Status[m_lCurSlave] = ST_READY_LOAD;
					m_qSubOperation = ACF_WH_REQUEST_FOR_GLASS_Q;	// go to 1
				}
			}
		}
		break;

	case ACF_WH_WAIT_WINEAGE_ACCEPT_PR1_Q:		// 23
		if (WINEAGLE_Status == ST_STOP)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
		}
		else if (ACF_WH_Status[m_lCurSlave] == ST_ACK_CLICK_PR)
		{
			if(m_lCurSlave == ACFWH_1)
			{
				g_szBondingMsg = " ACF1WH Station PR1 Error";
			}
			else
			{
				g_szBondingMsg = " ACF2WH Station PR1 Error";
			}

			PRS_ProcImgSimpleCmd(&pruACF1);

			g_stCursor.prCoord.x = PR_DEF_CENTRE_X;
			g_stCursor.prCoord.y = PR_DEF_CENTRE_Y;

			// Annoy User to click the point
			PRS_SetOnMouseClickCmd(ON);

			g_bShowUserSearchPRDone = TRUE;

			m_qSubOperation = ACF_WH_WAIT_USER_FINISH_PR1_Q;	// go to 24
		}
		break;

	case ACF_WH_WAIT_USER_FINISH_PR1_Q:			// 24
		if (WINEAGLE_Status == ST_STOP)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
		}
		else if (WINEAGLE_Status == ST_DONE)
		{
			pAppMod->m_ulUserInputPRError++;
			pAppMod->m_ulTotalUserInputPRError++;

			PRS_PRPosToMtrOffset(&pruACF1, g_stCursor.prCoord, &mtr_Offset);

			PRS_SetOnMouseClickCmd(OFF);

			m_dmtrCurrPR1Posn.x += (DOUBLE)mtr_Offset.x;
			m_dmtrCurrPR1Posn.y += (DOUBLE)mtr_Offset.y;

			g_bShowUserSearchPRDone = FALSE;

			WINEAGLE_Status = ST_RELEASE;

			ACF_WH_Status[m_lCurSlave] = ST_BUSY;

			if ((m_nLastError = MoveYToPR2Posn(g_stACF[m_lCurACF].lEdgeInfo, GMP_WAIT)) == GMP_SUCCESS)
			{
				m_qSubOperation = ACF_WH_DO_PR2_Q;	// go to 6
			}
		}
		else if (WINEAGLE_Status == ST_USER_REJECT)
		{
			g_bShowUserSearchPRDone = FALSE;

			WINEAGLE_Status = ST_RELEASE;

			// update stats
			pAppMod->m_ulACFWHPRError++;
			pAppMod->m_ulTotalACFWHPRError++;

			//reject glass
			if (m_lRejectGlassLimit != 0)
			{
				m_lRejectGlassCounter++;
				m_lPRRetryCounter = 0;

				// ERROR!
				if (m_lRejectGlassCounter >= m_lRejectGlassLimit)
				{
					if(m_lCurSlave == ACFWH_1)
					{
						m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
						WriteHMIMess("*** ERROR: ACF1WH Reject Glass Limit Hit ***", TRUE);
						WriteHMIMess("*** ERROR: ACF1WH User Reject PR 1 Error ***", TRUE);
						m_bModError = TRUE;
						SetError(IDS_ACF1WH_REJECT_GLASS_LIMIT_HIT);
						SetError(IDS_ACF1WH_PR1_ERR);
					}
					else
					{
						m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
						WriteHMIMess("*** ERROR: ACF2WH Reject Glass Limit Hit ***", TRUE);
						WriteHMIMess("*** ERROR: ACF2WH User Reject PR 1 Error ***", TRUE);
						m_bModError = TRUE;
						SetError(IDS_ACF2WH_REJECT_GLASS_LIMIT_HIT);
						SetError(IDS_ACF2WH_PR1_ERR);
					}
				}
				else
				{
					ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
					m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
					if(m_lCurSlave == ACFWH_1)
					{
						WriteHMIMess("*** ACF1WH User Reject PR 1 Error: Rejecting Glass... ***", TRUE);
					}
					else
					{
						WriteHMIMess("*** ACF2WH User Reject PR 1 Error: Rejecting Glass... ***", TRUE);
					}
				}
			}
			else
			{
				ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
				m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
				if(m_lCurSlave == ACFWH_1)
				{
					WriteHMIMess("*** ACF1WH User Reject PR 1 Error: Rejecting Glass... ***", TRUE);
				}
				else
				{
					WriteHMIMess("*** ACF2WH User Reject PR 1 Error: Rejecting Glass... ***", TRUE);
				}
			}
		}
		break;

	case ACF_WH_WAIT_WINEAGE_ACCEPT_PR2_Q:		// 25
		if (WINEAGLE_Status == ST_STOP)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
		}
		else if (ACF_WH_Status[m_lCurSlave] == ST_ACK_CLICK_PR)
		{
			if(m_lCurSlave == ACFWH_1)
			{
				g_szBondingMsg = " ACF1WH Station PR2 Error";
			}
			else
			{
				g_szBondingMsg = " ACF2WH Station PR2 Error";
			}

			PRS_ProcImgSimpleCmd(&pruACF2);

			g_stCursor.prCoord.x = PR_DEF_CENTRE_X;
			g_stCursor.prCoord.y = PR_DEF_CENTRE_Y;

			// Annoy User to click the point
			PRS_SetOnMouseClickCmd(ON);

			g_bShowUserSearchPRDone = TRUE;

			m_qSubOperation = ACF_WH_WAIT_USER_FINISH_PR2_Q;	// go to 26
		}
		break;

	case ACF_WH_WAIT_USER_FINISH_PR2_Q:			// 26
		if (WINEAGLE_Status == ST_STOP)
		{
			m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
		}
		else if (WINEAGLE_Status == ST_DONE)
		{
			pAppMod->m_ulUserInputPRError++;
			pAppMod->m_ulTotalUserInputPRError++;

			PRS_PRPosToMtrOffset(&pruACF2, g_stCursor.prCoord, &mtr_Offset);

			PRS_SetOnMouseClickCmd(OFF);

			m_dmtrCurrPR2Posn.x += (DOUBLE)mtr_Offset.x;
			m_dmtrCurrPR2Posn.y += (DOUBLE)mtr_Offset.y;

			g_bShowUserSearchPRDone = FALSE;

			WINEAGLE_Status = ST_RELEASE;

			ACF_WH_Status[m_lCurSlave] = ST_REQ_BONDING;

			//LONG lPRLevel = 0;
			//DOUBLE dOffset		= (g_stGlass.dThickness3 + g_stGlass.dThickness4) * 1000.0 + m_lPRLevelOffset;
			//if (CalcWHAbsoluteLevel(m_lCalibStageLevel, DoubleToLong(dOffset), m_stWHCalibZ, &lPRLevel))
			//{
			//	if ((fabs(lPRLevel - m_lStandbyLevel) <= 5.0) || ((m_nLastError = MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_NOWAIT)) == GMP_SUCCESS))
			//	{
			//		m_qSubOperation = ACF_WH_WAIT_TO_BOND_POSN_Q;	// go to 9
			//	}
			//}

			m_qSubOperation = ACF_WH_WAIT_TO_BOND_POSN_Q;	// go to 9
		}
		else if (WINEAGLE_Status == ST_USER_REJECT)
		{
			g_bShowUserSearchPRDone = FALSE;

			WINEAGLE_Status = ST_RELEASE;

			// update stats
			pAppMod->m_ulACFWHPRError++;
			pAppMod->m_ulTotalACFWHPRError++;

			//reject glass
			if (m_lRejectGlassLimit != 0)
			{
				m_lRejectGlassCounter++;
				m_lPRRetryCounter = 0;

				// ERROR!
				if (m_lRejectGlassCounter >= m_lRejectGlassLimit)
				{
					if(m_lCurSlave == ACFWH_1)
					{
						m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
						WriteHMIMess("*** ERROR: ACF1WH Reject Glass Limit Hit ***", TRUE);
						WriteHMIMess("*** ERROR: ACF1WH User Reject PR 2 Error ***", TRUE);
						m_bModError = TRUE;
						SetError(IDS_ACF1WH_REJECT_GLASS_LIMIT_HIT);
						SetError(IDS_ACF1WH_PR2_ERR);
					}
					else
					{
						m_qSubOperation = ACF_WH_STOP_Q;	// go to 99
						WriteHMIMess("*** ERROR: ACF2WH Reject Glass Limit Hit ***", TRUE);
						WriteHMIMess("*** ERROR: ACF2WH User Reject PR 2 Error ***", TRUE);
						m_bModError = TRUE;
						SetError(IDS_ACF2WH_REJECT_GLASS_LIMIT_HIT);
						SetError(IDS_ACF2WH_PR2_ERR);
					}
				}
				else
				{
					ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
					m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
					if(m_lCurSlave == ACFWH_1)
					{
						WriteHMIMess("*** ACF1WH User Reject PR 2 Error: Rejecting Glass... ***", TRUE);
					}
					else
					{
						WriteHMIMess("*** ACF2WH User Reject PR 2 Error: Rejecting Glass... ***", TRUE);
					}
				}
			}
			else
			{
				ACF_WH_Status[m_lCurSlave] = ST_PR_ERROR;
				m_qSubOperation = ACF_WH_GO_UNLOAD_GLASS_Q;	// go to 21
				if(m_lCurSlave == ACFWH_1)
				{
					WriteHMIMess("*** ACF1WH User Reject PR 2 Error: Rejecting Glass... ***", TRUE);
				}
				else
				{
					WriteHMIMess("*** ACF2WH User Reject PR 2 Error: Rejecting Glass... ***", TRUE);
				}
			}
		}
		break;

	case ACF_WH_STOP_Q:							// 99
		break;
	}

	if (m_qPreviousSubOperation != m_qSubOperation)
	{
		if (HMI_bDebugSeq)
		{
			CString szMsg;
			if (ACF_WH_STOP_Q != m_qSubOperation)
			{
				szMsg.Format("ACF%ldWH Go State: %ld", m_lCurSlave + 1, m_qSubOperation);
			}
			else
			{
				szMsg.Format("ACF%ldWH Go Stop", m_lCurSlave + 1);
			}
			//DisplayMessage(szMsg);
		}
		UpdateDebugSeq(GetSequenceString(), ACF_WH_Status[m_lCurSlave], ACF_GLASS_Status[m_lCurSlave]);
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
			stPrintData.szTitle[GetStationID() * MAX_STATION_SEQ_DATA + 2].Format("%ld", ACF_WH_Status[m_lCurSlave]);
			stPrintData.szTitle[GetStationID() * MAX_STATION_SEQ_DATA + 3].Format("%ld", ACF_GLASS_Status[m_lCurSlave][GLASS1]);	

			LogFile->log(stPrintData.szTitle);
		}
	}


//	if (m_qPreviousSubOperation != m_qSubOperation)
//	{
//		CString szMsg;
//		if (HMI_bDebugSeq)
//		{
//			if (ACF_WH_STOP_Q != m_qSubOperation)
//			{
//				szMsg.Format("ACF_WH Go State: %ld", m_qSubOperation);
//			}
//			else
//			{
//				szMsg.Format("ACF_WH Go Stop");
//			}
//			DisplayMessage(szMsg);
//
//			
//
//			for (i = 0; i < MAX_PRINT_DATA; i++)
//			{
//				stPrintData.szTitle[i]	= "";
//			}
//
//			stPrintData.szTitle[0]	= GetSequenceString();
//			stPrintData.szTitle[1].Format("Status: %d", ACF_WH_Status[m_lCurSlave]);
//			stPrintData.szTitle[2].Format("EDGE: %ld", m_lCurEdge);
//
//			stPrintData.dData[0]	= m_qSubOperation;
//			stPrintData.dData[1]	= ACF_WH_Status[m_lCurSlave];
//			stPrintData.dData[2]	= m_lCurEdge;
//
//			pCHouseKeeping->PrintTitle("ACFWH_SequenceLog", stPrintData.szTitle);
//			//pCHouseKeeping->PrintData("ACFWH_SequenceLog", stPrintData);
//		}
//
//		m_qPreviousSubOperation = m_qSubOperation;
//
//		//debug
//#if 1 //20131010
//		STATE_ENTRY stState;
//		stState.szState = GetSequenceString();
//		stState.clkState = clock();
//		//memset(&stState.lStatus[0], 0, sizeof(LONG) * MAX_NUM_DEBUG_STATUS);
//		stState.lStatus[0] = (LONG) ACF_WH_Status[m_lCurSlave];
//		stState.lStatus[1] = (LONG) EMPTY_MARK;
//		stState.stGlassStatus[0].szLabel = "bModError";
//		stState.stGlassStatus[0].lStatus = (LONG) m_bModError;
//		stState.stGlassStatus[1].szLabel = "m_bGlass1Exist";
//		stState.stGlassStatus[1].lStatus = (LONG) m_bGlass1Exist;
//		stState.stGlassStatus[2].szLabel = "m_bPRError";
//		stState.stGlassStatus[2].lStatus = (LONG) m_bPRError;
//		stState.stGlassStatus[3].szLabel = "m_nPRInProgress";
//		stState.stGlassStatus[3].lStatus = (LONG) m_nPRInProgress;
//		stState.stGlassStatus[4].lStatus = (LONG) EMPTY_MARK;
//
//		m_csGetAllStatusLock.Lock();	// 20150604
//		m_deqDebugSeq.push_back(stState);
//		while (!m_deqDebugSeq.empty() && (LONG) m_deqDebugSeq.size() > SEQ_DEQUE_LENGTH)
//		{
//			m_deqDebugSeq.pop_front();
//		}
//		m_csGetAllStatusLock.Unlock();	// 20150604
//#endif
//	}
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

LONG CACFWH::GetStandbyPosnT(LONG lEdge) // No Pop Up Message Allowed
{
	LONG lTemp = 0;

	if (lEdge < 0 || lEdge >= MAX_EDGE_OF_GLASS)
	{
		return 0;
	}

	//if (g_bIsDualIndex)
	//{
	//	switch (lEdge)
	//	{
	//	case EDGE_A:
	//	case EDGE_B:
	//	case EDGE_C:
	//	case EDGE_D:
	//		lTemp = m_lStandbyPosnT[EDGE_A];
	//	}
	//}
	//else
	//{
		lTemp = m_lStandbyPosnT[lEdge];
	//}

	return lTemp;
}

//VOID CACFWH::SetCurEdge()
//{
//	LONG lTempPosn;
//
//	lTempPosn = GetServoCurrentPosn(m_stMotorT.stAttrib.szAxis);
//
//	if (lTempPosn > m_lStandbyPosnT[EDGE_A])
//	{
//		if (lTempPosn > m_lStandbyPosnT[EDGE_A] + AngleToCount(135, m_stMotorT.stAttrib.dDistPerCount))
//		{
//			m_lPrevEdge = EDGE_C;
//			return;
//		}
//		else if (lTempPosn > m_lStandbyPosnT[EDGE_A] + AngleToCount(45, m_stMotorT.stAttrib.dDistPerCount))
//		{
//			m_lPrevEdge = EDGE_B;
//			return;
//		}
//		else
//		{
//			m_lPrevEdge = EDGE_A;
//			return;
//		}
//	}
//	else
//	{
//		if (lTempPosn < m_lStandbyPosnT[EDGE_A] - AngleToCount(135, m_stMotorT.stAttrib.dDistPerCount))
//		{
//			m_lPrevEdge = EDGE_C;
//			return;
//		}
//		else if (lTempPosn < m_lStandbyPosnT[EDGE_A] - AngleToCount(45, m_stMotorT.stAttrib.dDistPerCount))
//		{
//			m_lPrevEdge = EDGE_D;
//			return;
//		}
//		else
//		{
//			m_lPrevEdge = EDGE_A;
//			return;
//		}
//	}
//}

LONG CACFWH::FindFirstUnit(EdgeNum lEdge)
{	
	return GetFirstUnit(lEdge, g_stACF);
}

INT CACFWH::GetNextPostBondEdge(LONG lCurrEdge)
{
	// Define Clockwise & Anti Clockwise Neighbour Edge
	// Make Decision on Closest Next Edge (Cloclwise Neighbour for first Priority)

	INT nResult			= EDGE_NONE;
	INT nNumOfEdge		= 0;

	INT	i = 0;
	EdgeStatus stEdgeStatus[MAX_EDGE_OF_GLASS];

	for (i = 0; i < MAX_EDGE_OF_GLASS; i++)
	{
		stEdgeStatus[i].bUsed	= FALSE;
		stEdgeStatus[i].bBonded	= FALSE;
	}

	// Set the Edge Status 
	// Assume the Bonding ACF/LSI order is Ascending
	for (i = ACF1; i < MAX_NUM_OF_ACF; i++)
	{
		if (g_stACF[i].bEnable)
		{
			switch (g_stACF[i].lEdgeInfo)
			{
			case EDGE_A:
				stEdgeStatus[EDGE_A].bUsed	= TRUE;
				stEdgeStatus[EDGE_A].bBonded = m_bPostBondChecked[EDGE_A];
				break;

			//case EDGE_B:
			//	stEdgeStatus[EDGE_B].bUsed	= TRUE;
			//	stEdgeStatus[EDGE_B].bBonded = m_bPostBondChecked[EDGE_B];
			//	break;

			//case EDGE_C:
			//	stEdgeStatus[EDGE_C].bUsed	= TRUE;
			//	stEdgeStatus[EDGE_C].bBonded = m_bPostBondChecked[EDGE_C];
			//	break;

			//case EDGE_D:
			//	stEdgeStatus[EDGE_D].bUsed	= TRUE;
			//	stEdgeStatus[EDGE_D].bBonded = m_bPostBondChecked[EDGE_D];
			//	break;
			}
		}
	}

	return SrchNextEdge(lCurrEdge, stEdgeStatus);
}


#ifdef ACF_POSN_CALIB //20120917
BOOL CACFWH::ACFOffsetTestCalibOperation_RunPart(BOOL bIsPickGlass, ACF_OFFSET_TEST_CTRL stPart)
{
	CString szTitle;
	LONG lAnswer = rMSG_TIMEOUT;
	BOOL bNew = FALSE;
	LONG lStep = 0;
	INT	nCurIndex = m_pCACF->m_nIndexLast;
	BOOL bIsBondACF = TRUE;

	szTitle.Format("%s OFFSET TEST", GetStnName());

	// reset 
	m_lCurACF = ACF1;

	if (stPart != ACF_OFFSET_TEST_CTRL_END)
	{
		do
		{
			if (m_pCACF->m_emACFIndexed[nCurIndex] != m_lCurACF)
			{
				bNew = TRUE;
			}

			nCurIndex++;
			nCurIndex = nCurIndex % MAX_SIZE_OF_ACF_ARRAY;

		} while (!bNew && nCurIndex != m_pCACF->m_nIndexFirst);

		if (bIsBondACF)
		{
			// new ACF
			if (bNew)			
			{
				lAnswer = HMIMessageBox(MSG_YES_NO, szTitle, "Index a New ACF?");
				if (lAnswer == rMSG_YES)
				{
					if (!m_pCACF->CutACFOperation())
					{
						HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
						return FALSE;
					}
					HMIMessageBox(MSG_CONTINUE, szTitle, "Please clean ACF before the cutter.");	

					m_pCACF->ClearACFArray();

					INT nArrarySize = (LONG)((m_pCACF->m_dCutterToHeadDistance + m_pCACF->m_dCutterToHeadOffset) / m_stACF[m_lCurACF].dLength) + 1;
					m_pCACF->m_dRemainingLength = (m_pCACF->m_dCutterToHeadDistance + m_pCACF->m_dCutterToHeadOffset) - (m_stACF[m_lCurACF].dLength * nArrarySize);

					for (INT i = 0; i < nArrarySize; i++)
					{
						m_pCACF->m_emLastIndexedACF = (ACFNum)(m_lCurACF - 1); // For this operation, it will insert all same length of g_st[m_lCurACF].length

						if (
							m_pCACF->IndexACFIndexer() != GMP_SUCCESS || 
							m_pCACF->MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, GMP_WAIT) != GMP_SUCCESS || 
							!m_pCACF->CutACFOperation()
						   )
						{
							HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
							return FALSE;
						}
					}

					if (
						m_pCACF->MoveRelativeIndexer((LONG)(-1.0 * m_pCACF->m_dRemainingLength * 1000 * m_pCACF->m_stMotorIndex.stAttrib.dDistPerCount * m_pCACF->m_dIndexerFactor * m_pCACF->m_dIndexerCustomFactor), GMP_NOWAIT) != GMP_SUCCESS || //20120706
						m_pCACF->MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, GMP_WAIT) != GMP_SUCCESS
					   )
					{
						HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
						return FALSE;
					}

					m_pCACF->m_dRemainingLength = 0.0;
				}
				else if (lAnswer == rMSG_TIMEOUT)
				{	
					return FALSE;
				}
			}
			// no new ACF
			else 
			{
				if (m_pCACF->m_emACFIndexed[m_pCACF->m_nIndexLast] != (ACFNum)m_lCurACF)
				{
					lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "ACF OFFSET TEST", "Invalid ACF Type. Continue?");
					if (lAnswer == rMSG_CANCEL || lAnswer == rMSG_TIMEOUT)
					{
						HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
						return FALSE;
					}
				}
			}
		}

		if (bIsPickGlass)
		{
			CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
			//pCTA2->PickGlassOperation(NO_GLASS);
		//	pCTA2->PlaceGlassOperation(NO_GLASS);
		}
		// + index glass
		if (
			//(!bIsCalibMode && IndexGlassToTable(GLASS1) != GMP_SUCCESS) ||
			//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
			MoveXYToBondPosn(m_lCurACF) != GMP_SUCCESS
		   )
		{
			HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
			return FALSE;
		}
	} //part

	if (stPart != ACF_OFFSET_TEST_CTRL_BEGIN)
	{

		if (stPart == ACF_OFFSET_TEST_CTRL_END)
		{
			DOUBLE dUserInputYOffset = 0;
			dUserInputYOffset = m_stMotorY.GetEncPosn() - (m_mtrBaseHeaterPosn.y + DistanceToCount(m_lBondOffsetY[m_lCurACF] - m_dBondOffsetAdjY[m_lCurACF], m_stMotorY.stAttrib.dDistPerCount));
			m_lBondOffsetY[m_lCurACF] += (LONG) CountToDistance(dUserInputYOffset, m_stMotorY.stAttrib.dDistPerCount);
			HMI_lBondOffsetY = m_lBondOffsetY[m_lCurACF];
		}
#if 0
		HMIMessageBox(MSG_OK, "ACF Bond Debug Pause", "Press OK to continue");
#else
		//if (
		//	MoveZToBondLevel(m_lCurACF) != GMP_SUCCESS
		//   )
		//{
		//	HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
		//	return FALSE;
		//}

		// Attach ACF
		if (!m_pCACF->AttachACFOperation((ACFNum)m_lCurACF))
		{
			HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
			return FALSE;
		}
#endif
		if (
			//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
			MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS
		   )
		{
			HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
			return FALSE;
		}

		if (bIsBondACF)
		{
			// index + Cut ACF
			m_pCACF->m_emLastIndexedACF = (ACFNum)(m_lCurACF - 1);

			if (m_pCACF->IndexNextACFOperation() != GMP_SUCCESS)
			{
				HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CACFWH::ACFOffsetTestCalibOperation(BOOL bIsPickGlass)
{

	CString szTitle;
	LONG lAnswer = rMSG_TIMEOUT;
	BOOL bNew = FALSE;
	LONG lStep = 0;
	INT	nCurIndex = m_pCACF->m_nIndexLast;
	//BOOL bIsBondACF = FALSE;
		
	szTitle.Format("%s OFFSET TEST", GetStnName());

	// reset 
	m_lCurACF = ACF1;

	do
	{
		if (m_pCACF->m_emACFIndexed[nCurIndex] != m_lCurACF)
		{
			bNew = TRUE;
		}

		nCurIndex++;
		nCurIndex = nCurIndex % MAX_SIZE_OF_ACF_ARRAY;

	} while (!bNew && nCurIndex != m_pCACF->m_nIndexFirst);

	//if(bIsBondACF)
	//{
	//// new ACF
	//if (bNew)			
	//{
	//	lAnswer = HMIMessageBox(MSG_YES_NO, szTitle, "Index a New ACF?");
	//	if (lAnswer == rMSG_YES)
	//	{
	//		if (!m_pCACF->CutACFOperation())
	//		{
	//			HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
	//			return FALSE;
	//		}
	//		HMIMessageBox(MSG_CONTINUE, szTitle, "Please clean ACF before the cutter.");	

	//		m_pCACF->ClearACFArray();

	//		INT nArrarySize = (LONG)((m_pCACF->m_dCutterToHeadDistance + m_pCACF->m_dCutterToHeadOffset) / m_stACF[m_lCurACF].dLength) + 1;
	//		m_pCACF->m_dRemainingLength = (m_pCACF->m_dCutterToHeadDistance + m_pCACF->m_dCutterToHeadOffset) - (m_stACF[m_lCurACF].dLength * nArrarySize);

	//		for (INT i = 0; i < nArrarySize; i++)
	//		{
	//			m_pCACF->m_emLastIndexedACF = (ACFNum)(m_lCurACF-1); // For this operation, it will insert all same length of g_st[m_lCurACF].length

	//			if (
	//				m_pCACF->IndexACFIndexer() != GMP_SUCCESS || 
	//				m_pCACF->MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, GMP_WAIT) != GMP_SUCCESS || 
	//				!m_pCACF->CutACFOperation()
	//				)
	//			{
	//				HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
	//				return FALSE;
	//			}
	//		}

	//		if (
	//			m_pCACF->MoveRelativeIndexer((LONG)(-1.0 * m_pCACF->m_dRemainingLength *1000 * m_pCACF->m_stMotorIndex.stAttrib.dDistPerCount * m_pCACF->m_dIndexerFactor * m_pCACF->m_dIndexerCustomFactor), GMP_NOWAIT) != GMP_SUCCESS || //20120706
	//			m_pCACF->MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, GMP_WAIT) != GMP_SUCCESS
	//			)
	//		{
	//			HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
	//			return FALSE;
	//		}

	//		m_pCACF->m_dRemainingLength = 0.0;
	//	}
	//	else if (lAnswer == rMSG_TIMEOUT)
	//	{	
	//		return FALSE;
	//	}
	//}
	//// no new ACF
	//else 
	//{
	//	if (m_pCACF->m_emACFIndexed[m_pCACF->m_nIndexLast] != (ACFNum)m_lCurACF)
	//	{
	//		lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "ACF OFFSET TEST", "Invalid ACF Type. Continue?");
	//		if (lAnswer == rMSG_CANCEL || lAnswer == rMSG_TIMEOUT)
	//		{
	//			HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
	//			return FALSE;
	//		}
	//	}
	//}
	//}

	if (bIsPickGlass)
	{
		CTA2 *pCTA2	= (CTA2*)m_pModule->GetStation("TA2"); 
		//pCTA2->PickGlassOperation(NO_GLASS);
//		pCTA2->PlaceGlassOperation(NO_GLASS);
	}
	// + index glass
	if (
		//(!bIsCalibMode && IndexGlassToTable(GLASS1) != GMP_SUCCESS) ||
		//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		MoveXYToBondPosn(m_lCurACF) != GMP_SUCCESS /*||*/
		//MoveZToBondLevel(m_lCurACF) != GMP_SUCCESS
	   )
	{
		HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
		return FALSE;
	}
#if 1
	HMIMessageBox(MSG_OK, "ACF Bond Debug Pause", "Press OK to continue");
#else
	// Attach ACF
	if (!m_pCACF->AttachACFOperation((ACFNum)m_lCurACF))
	{
		HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
		return FALSE;
	}
#endif
	if (
		//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
		MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS
	   )
	{
		HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
		return FALSE;
	}

	//if (bIsBondACF)
	//{
	//// index + Cut ACF
	//m_pCACF->m_emLastIndexedACF = (ACFNum)(m_lCurACF-1);

	//if (m_pCACF->IndexNextACFOperation() != GMP_SUCCESS)
	//{
	//	HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
	//	return FALSE;
	//}
	//}
	return TRUE;
}
#endif


BOOL CACFWH::ACFOffsetTestOperation(BOOL bDoACFDL)
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");

	CString szTitle;
	LONG lAnswer = rMSG_TIMEOUT;
	BOOL bNew = FALSE;
	LONG lStep = 0;
	INT	nCurIndex = m_pCACF->m_nIndexLast;
		
	szTitle.Format("%s OFFSET TEST", GetStnName());

	// reset 
	m_lCurACF = ACF1;

	do
	{
		if (m_pCACF->m_emACFIndexed[nCurIndex] != m_lCurACF)
		{
			bNew = TRUE;
		}

		nCurIndex++;
		nCurIndex = nCurIndex % MAX_SIZE_OF_ACF_ARRAY;

	} while (!bNew && nCurIndex != m_pCACF->m_nIndexFirst);

	// new ACF
	if (bNew)			
	{
		lAnswer = HMIMessageBox(MSG_YES_NO, szTitle, "Index a New ACF?");
		if (lAnswer == rMSG_YES)
		{
			if (!m_pCACF->CutACFOperation())
			{
				HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
				return FALSE;
			}
			HMIMessageBox(MSG_CONTINUE, szTitle, "Please clean ACF before the cutter.");	

			m_pCACF->ClearACFArray();

			INT nArrarySize = (LONG)((m_pCACF->m_dCutterToHeadDistance + m_pCACF->m_dCutterToHeadOffset) / m_stACF[m_lCurACF].dLength) + 1;
			m_pCACF->m_dRemainingLength = (m_pCACF->m_dCutterToHeadDistance + m_pCACF->m_dCutterToHeadOffset) - (m_stACF[m_lCurACF].dLength * nArrarySize);

			for (INT i = 0; i < nArrarySize; i++)
			{
				m_pCACF->m_emLastIndexedACF = (ACFNum)(m_lCurACF - 1); // For this operation, it will insert all same length of g_st[m_lCurACF].length

				if (
					m_pCACF->IndexACFIndexer() != GMP_SUCCESS || 
					m_pCACF->MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, GMP_WAIT) != GMP_SUCCESS || 
					!m_pCACF->CutACFOperation()
				   )
				{
					HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
					return FALSE;
				}
			}

			if (
				m_pCACF->MoveRelativeIndexer((LONG)(-1.0 * m_pCACF->m_dRemainingLength * 1000.0 / m_pCACF->m_stMotorIndex.stAttrib.dDistPerCount * m_pCACF->m_dIndexerFactor * m_pCACF->m_dIndexerCustomFactor), GMP_NOWAIT) != GMP_SUCCESS || //20120706
				m_pCACF->MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, GMP_WAIT) != GMP_SUCCESS
			   )
			{
				HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
				return FALSE;
			}

			m_pCACF->m_dRemainingLength = 0.0;
		}
		else if (lAnswer == rMSG_TIMEOUT)
		{	
			return FALSE;
		}
	}
	// no new ACF
	else 
	{
		if (m_pCACF->m_emACFIndexed[m_pCACF->m_nIndexLast] != (ACFNum)m_lCurACF)
		{
			lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "ACF OFFSET TEST", "Invalid ACF Type. Continue?");
			if (lAnswer == rMSG_CANCEL || lAnswer == rMSG_TIMEOUT)
			{
				HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
				return FALSE;
			}
		}
	}

	// + index glass
	if (
		IndexGlassToTable((GlassNum)GLASS1) != GMP_SUCCESS
	   )
	{
		HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
		return FALSE;
	}

	if (bDoACFDL && m_bPRSelected)
	{
		if (
			!DoACFDL() ||
			m_pCACF->MoveZ(ACF_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
			pCInspOpt->MoveX(INSPOPT_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT) != GMP_SUCCESS ||
			MoveTToBondPosn(GMP_WAIT) != GMP_SUCCESS ||
			MoveXYToBondPosn(m_lCurACF) != GMP_SUCCESS ||
			m_pCACF->SyncX() != GMP_SUCCESS ||
			SyncY() != GMP_SUCCESS
			//m_pCACF->MoveZ(ACF_Z_CTRL_GO_SEARCH_LEVEL, GMP_WAIT) != GMP_SUCCESS
			)
		{
			HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
			return FALSE;
		}
	}
	else
	{
		m_mtrBondOffset.x			= 0;
		m_mtrBondOffset.y			= 0;
		m_lTOffset					= 0;
		if (
			m_pCACF->MoveZ(ACF_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
			pCInspOpt->MoveX(INSPOPT_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT) != GMP_SUCCESS ||
			MoveT(ACF_WH_T_CTRL_GO_STANDBY, GMP_WAIT) != GMP_SUCCESS ||
			MoveXYToBondPosn(m_lCurACF) != GMP_SUCCESS ||
			m_pCACF->SyncX() != GMP_SUCCESS ||
			SyncY() != GMP_SUCCESS
			//m_pCACF->MoveZ(ACF_Z_CTRL_GO_SEARCH_LEVEL, GMP_WAIT) != GMP_SUCCESS
			)
		{
			HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
			return FALSE;
		}
	}

	lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Bond ACF?");
	if (lAnswer == rMSG_CONTINUE)
	{
		m_bNotBondACF = FALSE;
	}
	else
	{
		m_bNotBondACF = TRUE;
	}

	if (!m_bNotBondACF)
	{

		// Attach ACF
		if (!m_pCACF->AttachACFOperation((ACFNum)m_lCurACF))
		{
			HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
			return FALSE;
		}

		if (
			m_pCACF->MoveZ(ACF_Z_CTRL_GO_STANDBY_LEVEL, SFM_WAIT) != GMP_SUCCESS ||
			MoveT(ACF_WH_T_CTRL_GO_STANDBY, SFM_WAIT) != GMP_SUCCESS ||
			m_pCACF->MoveX(ACF_X_CTRL_GO_STANDBY_POSN, SFM_WAIT) != GMP_SUCCESS ||
			pCInspOpt->MoveX(INSPOPT_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT) != GMP_SUCCESS ||
			MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, SFM_WAIT) != GMP_SUCCESS
			)
		{
			HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
			return FALSE;
		}

		// index + Cut ACF
		m_pCACF->m_emLastIndexedACF = (ACFNum)(m_lCurACF - 1);

		if (m_pCACF->IndexNextACFOperation() != GMP_SUCCESS)
		{
			HMIMessageBox(MSG_OK, szTitle, "Operation Abort!");
			return FALSE;
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////
//Motor Related
/////////////////////////////////////////////////////////////////
//INT CACFWH::MoveZToStandbyLevel(LONG lGlassNum, BOOL bWait)
//{
//	if (lGlassNum == GLASS1)
//	{
//		return MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, bWait);
//	}
//	else 
//	{
//		DisplayMessage(__FUNCTION__);
//		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
//		return GMP_FAIL;
//	}
//}
//
//INT CACFWH::MoveZToLoadLevel(LONG lGlassNum, BOOL bWait)
//{
//	if (lGlassNum == GLASS1)
//	{
//		return MoveZ(ACF_WH_Z_CTRL_GO_LOAD_LEVEL, bWait);
//	}
//	else 
//	{
//		DisplayMessage(__FUNCTION__);
//		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
//		return GMP_FAIL;
//	}
//}
//
//INT CACFWH::MoveZToPreLoadLevel(LONG lGlassNum, BOOL bWait)
//{
//	if (lGlassNum == GLASS1)
//	{
//		return MoveZ(ACF_WH_Z_CTRL_GO_PRE_LOAD_LEVEL, bWait);
//	}
//	else 
//	{
//		DisplayMessage(__FUNCTION__);
//		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
//		return GMP_FAIL;
//	}
//}
//
//INT CACFWH::MoveZToUnloadLevel(LONG lGlassNum, BOOL bWait)
//{
//	if (lGlassNum == GLASS1)
//	{
//		return MoveZ(ACF_WH_Z_CTRL_GO_UNLOAD_LEVEL, bWait);
//	}
//	else 
//	{
//		DisplayMessage(__FUNCTION__);
//		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
//		return GMP_FAIL;
//	}
//}
INT CACFWH::MoveT(ACF_WH_T_CTRL_SIGNAL ctrl, BOOL bWait)
{
	LONG	lMtrCnt		= 0;
	DOUBLE	dDistPerCnt = m_stMotorT.stAttrib.dDistPerCount;

	switch (ctrl)
	{
	case ACF_WH_T_CTRL_GO_STANDBY:
		lMtrCnt = m_lStandbyPosnT[EDGE_A];
		break;

	case ACF_WH_T_CTRL_GO_LOAD_POSN_OFFSET:
		lMtrCnt = m_lStandbyPosnT[EDGE_A] + m_lLoadTOffset;
		break;

/*	case ACF_WH_T_CTRL_GO_EDGE_B:
		lMtrCnt = GetStandbyPosnT(EDGE_B);
		break;

	case ACF_WH_T_CTRL_GO_EDGE_C:
		lMtrCnt = GetStandbyPosnT(EDGE_C);
		break;

	case ACF_WH_T_CTRL_GO_EDGE_D:
		lMtrCnt = GetStandbyPosnT(EDGE_D);
		break;*/	
	}

	return m_stMotorT.MoveAbsolute(lMtrCnt, bWait);
}


INT CACFWH::MoveTToBondPosn(BOOL bWait)
{
	LONG	lMtrCnt;

	lMtrCnt = m_lStandbyPosnT[EDGE_A] + m_lTOffset;

	if ((m_nLastError = m_stMotorT.MoveAbsolute(lMtrCnt, bWait)) == GMP_SUCCESS)
	{
		return GMP_SUCCESS;
	}
	return GMP_FAIL;
}

INT CACFWH::MoveTToStandby(BOOL bWait)
{
	return MoveT(ACF_WH_T_CTRL_GO_STANDBY, bWait);
}

INT CACFWH::MoveRelativeT(LONG lMtrCnt, BOOL bWait)
{
	LONG lMotorEncCnt = 0;

	LONG lRotaryEncCnt = 0;


	return m_stMotorT.MoveRelative(lMtrCnt, bWait);
}

INT CACFWH::MoveAbsoluteT(LONG lMtrCnt, BOOL bWait)
{
	LONG lMotorEncCnt = 0;


	if (m_stMotorT.MoveAbsolute(lMtrCnt, bWait) != GMP_SUCCESS)
	{
		return GMP_FAIL;
	}

	return GMP_SUCCESS;
}


INT CACFWH::MoveXYToStandbyPosn(BOOL bWait)
{
	return MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, bWait);
}

INT CACFWH::MoveXYToLoadPosn(BOOL bWait)
{
	return MoveY(ACF_WH_Y_CTRL_GO_LOAD_POSN_OFFSET, bWait);
}

INT CACFWH::MoveXYToUnloadPosn(BOOL bWait)
{
	return MoveY(ACF_WH_Y_CTRL_GO_UNLOAD_POSN_OFFSET, bWait);
}

INT CACFWH::MoveXYToBondPosn(LONG lLSINum)
{
	//CINWH *pCINWH = (CINWH*)m_pModule->GetStation("INWH");
	if (lLSINum <= NO_ACF || lLSINum >= MAX_NUM_OF_ACF)
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		return gnAMS_NOTOK;
	}

	MTR_POSN	mtrTemp;
	mtrTemp.x	= 0;
	mtrTemp.y	= 0;


	mtrTemp.x = m_mtrApproxBondPosn.x - DistanceToCount(- m_dLoadAdjXOffset, m_pCACF->m_stMotorX.stAttrib.dDistPerCount) + m_mtrBondOffset.x;
	mtrTemp.y = m_mtrApproxBondPosn.y + DistanceToCount(- m_dBondOffsetAdjY[lLSINum], m_stMotorY.stAttrib.dDistPerCount) + m_mtrBondOffset.y;

	if (
		(m_mtrSafetyPosn.x == 0 && m_mtrSafetyPosn.y == 0) ||
		(m_mtrSafetyPosn.x < mtrTemp.x && m_mtrSafetyPosn.y > mtrTemp.y)
		)
	{
		if (

			((m_nLastError = m_pCACF->m_stMotorX.MoveAbsolute(mtrTemp.x, GMP_WAIT)) == GMP_SUCCESS) &&
			((m_nLastError = m_stMotorY.MoveAbsolute(mtrTemp.y, GMP_WAIT)) == GMP_SUCCESS) 
			)
		{
			return GMP_SUCCESS;
		}
	}
	else
	{
		if ((m_qState != AUTO_Q) && (m_qState != DEMO_Q))
		{
			HMIMessageBox(MSG_OK, "MOVE TO BOND POSN", "ACFWH Bond Posn Exceed Safety Posn! Operation Abort!");
		}
	}

	return GMP_FAIL;
}

INT CACFWH::MoveYToBondPosn(LONG lLSINum)
{
	if (lLSINum <= NO_ACF || lLSINum >= MAX_NUM_OF_ACF)
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		return gnAMS_NOTOK;
	}

	MTR_POSN	mtrTemp;
	mtrTemp.x	= 0;
	mtrTemp.y	= 0;

	//DOUBLE dOffset = (g_stGlass.dWHOffset + g_stGlass.dItoToDieEdge)*1000;
	//
	//dOffset = dOffset - m_lBondOffsetY[lLSINum];

	{ 
	//20120920 debug only
		if ((DOUBLE)m_lBondOffsetY[lLSINum] - m_dBondOffsetAdjY[lLSINum] > 0.0)
		{
			CString szStr;
			szStr.Format("error ACFWH");
			DisplayMessage(szStr);
			szStr.Format("m_lBondOffsetY[lLSINum] - m_dBondOffsetAdjY[lLSINum]= %.4f", (DOUBLE)m_lBondOffsetY[lLSINum] - m_dBondOffsetAdjY[lLSINum]);
			DisplayMessage(szStr);
			return GMP_FAIL;
		}
	}
	mtrTemp.y = m_mtrBaseHeaterPosn.y + DistanceToCount(m_lBondOffsetY[lLSINum] - m_dBondOffsetAdjY[lLSINum], m_stMotorY.stAttrib.dDistPerCount);

	return m_stMotorY.MoveAbsolute(mtrTemp.y, GMP_WAIT);
}

//INT CACFWH::MoveZToBondLevel(LONG lLSINum)
//{
//	LONG	lMtrCnt;
//	BOOL	bResult		= TRUE;
//	DOUBLE	dOffset		= 0.0;
//	
//	DOUBLE dThickness = g_stGlass.dThickness4;
//	
//	if (lLSINum <= NO_LSI || lLSINum >= MAX_NUM_OF_LSI)
//	{
//		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
//		DisplayMessage(__FUNCTION__);
//		return GMP_FAIL;
//	}
//
//	CalcWHAbsoluteLevel(m_lCalibStageLevel[WHZ_1],
//						DoubleToLong(m_lBondLevelOffset[WHZ_1][lLSINum] + (dThickness * 1000.0)), 
//						m_stWHCalibZ, &lMtrCnt, WHZ_1);
//
//	//lMtrCnt		= m_lCalibStageLevel[WHZ_1] + m_lBondLevelOffset[WHZ_1][lLSINum] + DistanceToCount(dThickness * 1000.0, m_stMotorZ[WHZ_1].stAttrib.dDistPerCount);
//	
//	return m_stMotorZ[WHZ_1].MoveAbsolute(lMtrCnt, GMP_WAIT);
//}

INT CACFWH::MoveY(ACF_WH_Y_CTRL_SIGNAL ctrl, BOOL bWait)
{
	MTR_POSN	stMove;
	stMove.x = 0;
	stMove.y = 0;

	switch (ctrl)
	{
	case ACF_WH_Y_CTRL_GO_STANDBY_POSN:
		stMove.y = m_mtrStandbyPosn.y;
		break;

	case ACF_WH_Y_CTRL_GO_APPROX_BOND_POSN:
		stMove.y = m_mtrApproxBondPosn.y;
		break;

	case ACF_WH_Y_CTRL_GO_BASE_HEATER_POSN:
		stMove.y = m_mtrBaseHeaterPosn.y;
		break;

	case ACF_WH_Y_CTRL_GO_LOAD_POSN:
		stMove.y = m_mtrLoadGlassPosn.y;
		break;
			
	case ACF_WH_Y_CTRL_GO_UNLOAD_POSN:
		stMove.y = m_mtrUnloadGlassPosn.y;
		break;

	case ACF_WH_Y_CTRL_GO_LOAD_POSN_OFFSET:
		stMove.y = m_mtrLoadGlassPosn.y /*+ DistanceToCount((DOUBLE)m_lLoadYOffset, m_stMotorY.stAttrib.dDistPerCount)*/ + m_mtrLoadGlassOffset.y;
		break;

	case ACF_WH_Y_CTRL_GO_UNLOAD_POSN_OFFSET:
		stMove.y = m_mtrUnloadGlassPosn.y + DistanceToCount((DOUBLE)m_lUnloadYOffset, m_stMotorY.stAttrib.dDistPerCount);
		break;


	}

	return m_stMotorY.MoveAbsolute(stMove.y, bWait);
}


INT CACFWH::PBMoveY(ACF_WH_XY_CTRL_SIGNAL ctrl, BOOL bWait)
{
	
	MTR_POSN	stMove;
	stMove.x = 0;
	stMove.y = 0;

	switch (ctrl)
	{

	case ACF_WH_XY_CTRL_GO_PBPR1_POSN:
		stMove.x = DoubleToLong(m_dmtrCurrPBPR1Posn.x);
		stMove.y = DoubleToLong(m_dmtrCurrPBPR1Posn.y);	
		break;

	case ACF_WH_XY_CTRL_GO_PBPR2_POSN:
		stMove.x = DoubleToLong(m_dmtrCurrPBPR2Posn.x);
		stMove.y = DoubleToLong(m_dmtrCurrPBPR2Posn.y);	
		break;

	case ACF_WH_XY_CTRL_GO_PBPR_MP1_POSN:
		stMove.x = DoubleToLong(m_dmtrCurrPBPR_MP1Posn.x);
		stMove.y = DoubleToLong(m_dmtrCurrPBPR_MP1Posn.y);	
		break;

	case ACF_WH_XY_CTRL_GO_PBPR_MP2_POSN:
		stMove.x = DoubleToLong(m_dmtrCurrPBPR_MP2Posn.x);
		stMove.y = DoubleToLong(m_dmtrCurrPBPR_MP2Posn.y);	
		break;

	}

	return m_stMotorY.MoveAbsolute(stMove.y, bWait);
}

INT CACFWH::OpticMoveXY(ACF_WH_XY_CTRL_SIGNAL ctrl, BOOL bWait)
{
	
	MTR_POSN	stMove;
	stMove.x = 0;
	stMove.y = 0;

	switch (ctrl)
	{

	case ACF_WH_XY_CTRL_GO_PBPR1_POSN:
		stMove.x = DoubleToLong(m_dmtrCurrPBPR1Posn.x);
		stMove.y = DoubleToLong(m_dmtrCurrPBPR1Posn.y);	
		break;

	case ACF_WH_XY_CTRL_GO_PBPR2_POSN:
		stMove.x = DoubleToLong(m_dmtrCurrPBPR2Posn.x);
		stMove.y = DoubleToLong(m_dmtrCurrPBPR2Posn.y);	
		break;

	case ACF_WH_XY_CTRL_GO_PBPR_MP1_POSN:
		stMove.x = DoubleToLong(m_dmtrCurrPBPR_MP1Posn.x);
		stMove.y = DoubleToLong(m_dmtrCurrPBPR_MP1Posn.y);	
		break;

	case ACF_WH_XY_CTRL_GO_PBPR_MP2_POSN:
		stMove.x = DoubleToLong(m_dmtrCurrPBPR_MP2Posn.x);
		stMove.y = DoubleToLong(m_dmtrCurrPBPR_MP2Posn.y);	
		break;

	}

	return OpticMoveAbsoluteXY(stMove.x, stMove.y, bWait);

}


INT CACFWH::ACFMoveAbsoluteXY(LONG lMtrCntX, LONG lMtrCntY, BOOL bWait)
{
	INT nResult = GMP_FAIL;

	if(
		((nResult = m_pCACF->MoveAbsoluteX(lMtrCntX, bWait)) == GMP_SUCCESS) &&
		((nResult = m_stMotorY.MoveAbsolute(lMtrCntY, bWait)) == GMP_SUCCESS)
		)
	{
		return GMP_SUCCESS;
	}

	return GMP_FAIL;
}

INT CACFWH::OpticMoveAbsoluteXY(LONG lMtrCntX, LONG lMtrCntY, BOOL bWait)
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	INT nResult = GMP_FAIL;

	if(
		((nResult = pCInspOpt->MoveAbsoluteX(lMtrCntX, bWait)) == GMP_SUCCESS) &&
		((nResult = m_stMotorY.MoveAbsolute(lMtrCntY, bWait)) == GMP_SUCCESS)
		)
	{
		return GMP_SUCCESS;
	}

	return GMP_FAIL;
}

INT CACFWH::MoveAbsoluteXY(LONG lMtrCntX, LONG lMtrCntY, BOOL bWait) // For Calibration
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	INT nResult = GMP_FAIL;

	if(
		((nResult = pCInspOpt->MoveAbsoluteX(lMtrCntX, bWait)) == GMP_SUCCESS) &&
		((nResult = m_stMotorY.MoveAbsolute(lMtrCntY, bWait)) == GMP_SUCCESS)
		)
	{
		return GMP_SUCCESS;
	}

	return GMP_FAIL;
}


INT CACFWH::MoveAbsoluteY(LONG lMtrCntY, BOOL bWait)
{
	return m_stMotorY.MoveAbsolute(lMtrCntY, bWait);
}

INT CACFWH::MoveRelativeXY(LONG lMtrCntX, LONG lMtrCntY, BOOL bWait) // for calibration
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	INT nResult = GMP_FAIL;

	if(
		((nResult = pCInspOpt->MoveRelativeX(lMtrCntX, bWait)) == GMP_SUCCESS) &&
		((nResult = m_stMotorY.MoveRelative(lMtrCntY, bWait)) == GMP_SUCCESS)
		)
	{
		return GMP_SUCCESS;
	}

	return GMP_FAIL;
}

INT CACFWH::MoveRelativeY(LONG lMtrCntY, BOOL bWait)
{
	INT nResult = GMP_FAIL;

	if(
		((nResult = m_stMotorY.MoveRelative(lMtrCntY, bWait)) == GMP_SUCCESS)
		)
	{
		return GMP_SUCCESS;
	}

	return GMP_FAIL;
}

INT CACFWH::OpticMoveRelativeXY(LONG lMtrCntX, LONG lMtrCntY, BOOL bWait)
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	INT nResult = GMP_FAIL;

	if(
		((nResult = pCInspOpt->MoveRelativeX(lMtrCntX, bWait)) == GMP_SUCCESS) &&
		((nResult = m_stMotorY.MoveRelative(lMtrCntY, bWait)) == GMP_SUCCESS)
		)
	{
		return GMP_SUCCESS;
	}

	return GMP_FAIL;
}


INT CACFWH::ACFMoveRelativeXY(LONG lMtrCntX, LONG lMtrCntY, BOOL bWait)
{
	INT nResult = GMP_FAIL;

	if(
		((nResult = m_pCACF->MoveRelativeX(lMtrCntX, bWait)) == GMP_SUCCESS) &&
		((nResult = m_stMotorY.MoveRelative(lMtrCntY, bWait)) == GMP_SUCCESS)
		)
	{
		return GMP_SUCCESS;
	}

	return GMP_FAIL;
}



//INT CACFWH::MoveZ(ACF_WH_Z_CTRL_SIGNAL ctrl, BOOL bWait)
//{
//
//	DOUBLE	dThickness	= 0.0;
//	LONG	lOffset		= 0;
//	LONG	lMtrCnt		= 0;
//	BOOL	bResult		= TRUE;
//	DOUBLE	dDistPerCnt = 0.0;
//	DOUBLE dGap = 0.0;
//
//	dDistPerCnt	= m_stMotorZ[WHZ_1].stAttrib.dDistPerCount;
//
//	switch (ctrl)
//	{
//	case ACF_WH_Z_CTRL_GO_STANDBY_LEVEL:
//		lMtrCnt = m_lStandbyLevel[WHZ_1];
//		break;
//
//	case ACF_WH_Z_CTRL_GO_CALIB_STAGE_LEVEL:
//		lMtrCnt = m_lCalibStageLevel[WHZ_1];
//		break;
//
//	case ACF_WH_Z_CTRL_GO_CALIB_LOAD_LEVEL:
//		lMtrCnt = m_lCalibLoadLevel[WHZ_1];
//		break;
//
//	case ACF_WH_Z_CTRL_GO_CALIB_UNLOAD_LEVEL:
//		lMtrCnt = m_lCalibUnloadLevel[WHZ_1];
//		break;
//
//	case ACF_WH_Z_CTRL_GO_APPROX_BOND_LEVEL:
//		lMtrCnt = m_lApproxBondLevel[WHZ_1];
//		break;
//
//	case ACF_WH_Z_CTRL_GO_PRE_LOAD_LEVEL:
//		dThickness = g_stGlass.dThickness1 + g_stGlass.dThickness2 + g_stGlass.dThickness3 + g_stGlass.dThickness4;
//		if(IsBurnInDiagBond())
//		{
//			dGap = FPC_GAP * GAP_THICKNESS * 1000.0;
//		}
//		else
//		{
//			dGap = 2.0 * dThickness * 1000.0;
//		}
//		CalcWHAbsoluteLevel(m_lCalibLoadLevel[WHZ_1],
//			DoubleToLong(dGap) + m_lLoadLevelOffset[WHZ_1] + 1500, m_stWHCalibZ, &lMtrCnt, WHZ_1);
//		break;
//
//	case ACF_WH_Z_CTRL_GO_LOAD_LEVEL:
//		dThickness = g_stGlass.dThickness1 + g_stGlass.dThickness2 + g_stGlass.dThickness3 + g_stGlass.dThickness4;
//
//		if(IsBurnInDiagBond())
//		{
//			dGap = FPC_GAP * GAP_THICKNESS * 1000.0;
//		}
//		else
//		{
//			dGap = dThickness * 1000.0;
//		}
//		CalcWHAbsoluteLevel(m_lCalibLoadLevel[WHZ_1],
//			DoubleToLong(dGap) + m_lLoadLevelOffset[WHZ_1], m_stWHCalibZ, &lMtrCnt, WHZ_1);
//		break;
//
//	case ACF_WH_Z_CTRL_GO_PRE_UNLOAD_LEVEL:
//		dThickness	= g_stGlass.dThickness1 + g_stGlass.dThickness2 + g_stGlass.dThickness3 + g_stGlass.dThickness4;
//
//		if(IsBurnInDiagBond())
//		{
//			dGap = FPC_GAP * GAP_THICKNESS * 1000.0;
//		}
//		else
//		{
//			dGap = 2.0 *dThickness * 1000.0;
//		}
//		CalcWHAbsoluteLevel(m_lCalibUnloadLevel[WHZ_1],
//			DoubleToLong(dGap) + 3000 + m_lUnloadLevelOffset[WHZ_1], m_stWHCalibZ, &lMtrCnt, WHZ_1);
//		break;
//
//	case ACF_WH_Z_CTRL_GO_UNLOAD_LEVEL:
//		dThickness	= g_stGlass.dThickness1 + g_stGlass.dThickness2 + g_stGlass.dThickness3 + g_stGlass.dThickness4;
//
//		if(IsBurnInDiagBond())
//		{
//			dGap = FPC_GAP * GAP_THICKNESS * 1000.0;
//		}
//		else
//		{
//			dGap = dThickness * 1000.0;
//		}
//		CalcWHAbsoluteLevel(m_lCalibUnloadLevel[WHZ_1],
//							DoubleToLong(dGap) + m_lUnloadLevelOffset[WHZ_1], m_stWHCalibZ, &lMtrCnt, WHZ_1);
//		break;
//	}
//
//	return m_stMotorZ[WHZ_1].MoveAbsolute(lMtrCnt, bWait);
//}

INT CACFWH::MoveYToPR1Posn(LONG lEdge, BOOL bWait)
{
	MTR_POSN stMove;

	switch (lEdge)
	{
	case EDGE_A:
		//stMove.x = (LONG)m_dmtrCurrPR1Posn[EDGE_A].x;
		stMove.y = (LONG)m_dmtrCurrPR1Posn.y;
		break;

	//case EDGE_B:
	//	stMove.x = (LONG)m_dmtrCurrPR1Posn[EDGE_B].x;
	//	stMove.y = (LONG)m_dmtrCurrPR1Posn[EDGE_B].y;
	//	break;

	//case EDGE_C:
	//	stMove.x = (LONG)m_dmtrCurrPR1Posn[EDGE_C].x;
	//	stMove.y = (LONG)m_dmtrCurrPR1Posn[EDGE_C].y;

	//case EDGE_D:
	//	stMove.x = (LONG)m_dmtrCurrPR1Posn[EDGE_D].x;
	//	stMove.y = (LONG)m_dmtrCurrPR1Posn[EDGE_D].y;
	//	break;

	default:
		//stMove.x = 0;
		stMove.y = 0;
		break;
	}

	return m_stMotorY.MoveAbsolute(stMove.y, bWait);
}

INT CACFWH::MoveYToPR2Posn(LONG lEdge, BOOL bWait)
{
	MTR_POSN stMove;

	switch (lEdge)
	{
	case EDGE_A:
		//stMove.x = (LONG)m_dmtrCurrPR2Posn[EDGE_A].x;
		stMove.y = (LONG)m_dmtrCurrPR2Posn.y;
		break;

	//case EDGE_B:
	//	stMove.x = (LONG)m_dmtrCurrPR2Posn[EDGE_B].x;
	//	stMove.y = (LONG)m_dmtrCurrPR2Posn[EDGE_B].y;
	//	break;

	//case EDGE_C:
	//	stMove.x = (LONG)m_dmtrCurrPR2Posn[EDGE_C].x;
	//	stMove.y = (LONG)m_dmtrCurrPR2Posn[EDGE_C].y;

	//case EDGE_D:
	//	stMove.x = (LONG)m_dmtrCurrPR2Posn[EDGE_D].x;
	//	stMove.y = (LONG)m_dmtrCurrPR2Posn[EDGE_D].y;
	//	break;

	default:
		//stMove.x = 0;
		stMove.y = 0;
		break;
	}

	return m_stMotorY.MoveAbsolute(stMove.y, bWait);
}

/////////////////////////////////////////////////////////////////
//Calc Related
/////////////////////////////////////////////////////////////////
VOID CACFWH::CalDPosnAfterRotate(D_MTR_POSN dmtrTableCOR, MTR_POSN mtrCurPosn, DOUBLE dAngle, D_MTR_POSN *p_dmtrResultPosn)
{
	DOUBLE dAngleRad;

	// COR not calibrated
	/*if (!m_bCORCalibrated)
	{
	p_dmtrResultPosn->x = mtrCurPosn.x;
	p_dmtrResultPosn->y = mtrCurPosn.y;
	return;
	}*/

	dAngleRad = (PI / 180.0) * dAngle;

	p_dmtrResultPosn->x = (+((DOUBLE)mtrCurPosn.x - dmtrTableCOR.x) * cos(dAngleRad)
						   - ((DOUBLE)mtrCurPosn.y - dmtrTableCOR.y) * sin(dAngleRad)
						   + dmtrTableCOR.x);

	p_dmtrResultPosn->y = (+((DOUBLE)mtrCurPosn.x - dmtrTableCOR.x) * sin(dAngleRad)
						   + ((DOUBLE)mtrCurPosn.y - dmtrTableCOR.y) * cos(dAngleRad)
						   + dmtrTableCOR.y);

	return;
}

VOID CACFWH::CalPosnAfterRotate(LONG lEdge, MTR_POSN mtrCurPosn, DOUBLE dAngle, MTR_POSN *p_mtrResultPosn)
{
	DOUBLE dAngleRad;
	D_MTR_POSN dmtrTableCOR;

	//if (g_bIsDualIndex)
	{
		lEdge = EDGE_A;
	}

	if (lEdge <= EDGE_NONE || lEdge >= MAX_EDGE_OF_GLASS)
	{
		p_mtrResultPosn->x = mtrCurPosn.x;
		p_mtrResultPosn->y = mtrCurPosn.y;

		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage(__FUNCTION__);
		return;
	}

	if (!m_bCORCalibrated[lEdge] || dAngle == 0.0)
	{
		p_mtrResultPosn->x = mtrCurPosn.x;
		p_mtrResultPosn->y = mtrCurPosn.y;
		return;
	}

	dAngleRad = (PI / 180.0) * dAngle;

	INT i = 0;
	INT nIndex = 0;

	if (fabs(dAngle) > m_dCORLUT[lEdge][COR_ARRAY_SIZE - 1])
	{
		nIndex = COR_ARRAY_SIZE - 1;
	}
	else
	{
		for (i = 0; i < COR_ARRAY_SIZE; i++)
		{
			if (fabs(dAngle) <= m_dCORLUT[lEdge][i])
			{
				nIndex = i;
				break;
			}
		}
	}

	// negative angle
	if (dAngle < 0.0)
	{
		dmtrTableCOR.x = m_dmtrTableCOR_Neg[lEdge][nIndex].x;
		dmtrTableCOR.y = m_dmtrTableCOR_Neg[lEdge][nIndex].y;
	}
	// positive ange
	else
	{
		dmtrTableCOR.x = m_dmtrTableCOR_Pos[lEdge][nIndex].x;
		dmtrTableCOR.y = m_dmtrTableCOR_Pos[lEdge][nIndex].y;
	}

	p_mtrResultPosn->x = DoubleToLong(+((DOUBLE)mtrCurPosn.x - dmtrTableCOR.x) * cos(dAngleRad)
									  - ((DOUBLE)mtrCurPosn.y - dmtrTableCOR.y) * sin(dAngleRad)
									  + dmtrTableCOR.x);

	p_mtrResultPosn->y = DoubleToLong(+((DOUBLE)mtrCurPosn.x - dmtrTableCOR.x) * sin(dAngleRad)
									  + ((DOUBLE)mtrCurPosn.y - dmtrTableCOR.y) * cos(dAngleRad)
									  + dmtrTableCOR.y);

	//CString str;
	//DisplayMessage("In CalcPosnAfterRotate()");
	//str.Format("ledge = %ld", m_lCurEdge);
	//DisplayMessage(str);
	//str.Format("x = %ld, y= %ld", dmtrTableCOR.x, dmtrTableCOR.y);
	//DisplayMessage(str);
	//DisplayMessage("Leave CalcPosnAfterRotate()");

	return;
}


/////////////////////////////////////////////////////////////////
//SETUP Related
/////////////////////////////////////////////////////////////////
LONG CACFWH::HMI_MeasureEnable(IPC_CServiceMessage &svMsg) //20121016
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
LONG CACFWH::HMI_MeasureSetZero(IPC_CServiceMessage &svMsg)
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	if (bMode)
	{
		m_bEnableMeasure = TRUE;

		m_mtrMeasureSetZeroPt.x = pCInspOpt->m_stMotorX.GetEncPosn();
		m_mtrMeasureSetZeroPt.y = m_stMotorY.GetEncPosn();

		m_dMeasuredDistance = 0.0;
		m_dMeasuredAngle = 0.0;
	}
	else
	{
		m_bEnableMeasure = FALSE;
	}

	return 0;
}
VOID CACFWH::ModuleSetupPreTask()
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	m_bCheckMotor = TRUE;
	m_bCheckIO	= TRUE;
	pCInspOpt->m_bCheckMotor = TRUE;
	pCInspOpt->m_bCheckIO	= TRUE;
	m_pCACF->m_bCheckMotor = TRUE;
	m_pCACF->m_bCheckIO	= TRUE;

	pCInspOpt->CheckModSelected(TRUE);

	SetDiagnSteps(g_lDiagnSteps);

	UpdateModuleSetupPara();
	UpdateModuleSetupPosn();
	//UpdateModuleSetupLevel();
}

VOID CACFWH::ModuleSetupPostTask()
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	m_bCheckMotor = FALSE;
	m_bCheckIO	= FALSE;
	pCInspOpt->m_bCheckMotor = FALSE;
	pCInspOpt->m_bCheckIO	= FALSE;
	m_pCACF->m_bCheckMotor = FALSE;
	m_pCACF->m_bCheckIO	= FALSE;
}

BOOL CACFWH::ModuleSetupSetPosn(LONG lSetupTitle)
{
	LogAction(__FUNCTION__);
	LogAction("m_lCurSlave = %ld", m_lCurSlave);

	//CPreBondWH *pCPreBondWH = (CPreBondWH*)m_pModule->GetStation("PreBondWH");
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");

	LONG		lAnswer = rMSG_TIMEOUT;
	LONG		lResponse = 0;
	BOOL		bResult = TRUE;
	MTR_POSN	mtrOld;
	mtrOld.x	= 0;
	mtrOld.y	= 0;


	lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Overwrite Position?");

	if (lAnswer == rMSG_CANCEL || lAnswer == rMSG_TIMEOUT)
	{
		return FALSE;
	}
	
	//if(g_stSetupPosn[lSetupTitle].lType == MACHINE_PARA && g_ulUserGroup < SERVICE)
	//{
	//	HMIMessageBox(MSG_OK, "WARNING", "You are not allowed to change Machine Parameters");
	//	return FALSE;
	//}

	MTR_POSN TempPosn;

	TempPosn.x = m_pCACF->m_stMotorX.GetEncPosn();
	TempPosn.y = m_stMotorY.GetEncPosn();

	switch (lSetupTitle)
	{
	case 0:
		mtrOld.x				= m_mtrStandbyPosn.x;
		mtrOld.y				= m_mtrStandbyPosn.y;

		m_mtrStandbyPosn.x = TempPosn.x;
		m_mtrStandbyPosn.y = TempPosn.y;
		break;

	case 1:
		mtrOld.x				= m_mtrBaseHeaterPosn.x;
		mtrOld.y				= m_mtrBaseHeaterPosn.y;

		m_mtrBaseHeaterPosn.x = TempPosn.x;
		m_mtrBaseHeaterPosn.y = TempPosn.y;
		break;


	case 2:
		//mtrOld.x				= m_mtrLoadGlassPosn.x;
		mtrOld.y				= m_mtrLoadGlassPosn.y;

		//m_mtrLoadGlassPosn.x = TempPosn.x;
		m_mtrLoadGlassPosn.y = TempPosn.y;
		break;

	case 3:
		//mtrOld.x				= m_mtrUnloadGlassPosn.x;
		mtrOld.y				= m_mtrUnloadGlassPosn.y;

		//m_mtrUnloadGlassPosn.x = TempPosn.x;
		m_mtrUnloadGlassPosn.y = TempPosn.y;
		break;

	case 4:
		mtrOld.x				= m_mtrApproxBondPosn.x;
		mtrOld.y				= m_mtrApproxBondPosn.y;

		m_mtrApproxBondPosn.x = TempPosn.x;
		m_mtrApproxBondPosn.y = TempPosn.y;
		break;	

	case 5:
	case 6:
		break;	

	case 7:
		TempPosn.x = pCInspOpt->m_stMotorX.GetEncPosn();
		mtrOld.x				= m_mtrPRCalibPosn.x;
		mtrOld.y				= m_mtrPRCalibPosn.y;

		m_mtrPRCalibPosn.x = TempPosn.x;
		m_mtrPRCalibPosn.y = TempPosn.y;
		break;	

	case 8:
	case 9:
		break;	

	case 10:
		mtrOld.x				= m_mtrSafetyPosn.x;
		mtrOld.y				= m_mtrSafetyPosn.y;

		m_mtrSafetyPosn.x = TempPosn.x;
		m_mtrSafetyPosn.y = TempPosn.y;
		break;	

	case 11:
		TempPosn.x = pCInspOpt->m_stMotorX.GetEncPosn();
		mtrOld.x				= pCInspOpt->m_mtrStandbyPosn.x;

		pCInspOpt->m_mtrStandbyPosn.x = TempPosn.x;
		break;	

	case 12:
		TempPosn.x = m_stMotorT.GetEncPosn();
		mtrOld.x				= m_lStandbyPosnT[EDGE_A];

		m_lStandbyPosnT[EDGE_A] = TempPosn.x;
		break;	

	case 13:
		break;	

	case 14:
		TempPosn.x = m_stMotorT.GetEncPosn();
		mtrOld.x				= m_lLoadTOffset;

		m_lLoadTOffset	= TempPosn.x - m_lStandbyPosnT[EDGE_A];

		TempPosn.x			= m_lLoadTOffset;
		break;	
	}

	CAC9000Stn::ModuleSetupSetPosn(lSetupTitle, mtrOld, TempPosn);

	return bResult;
}

BOOL CACFWH::ModuleSetupGoPosn(LONG lSetupTitle)
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	INT nResult = GMP_SUCCESS;

	CString szTitle = "";
	CString szMsg = "";
	szTitle.Format("GO POSN OPERATION");
	szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());

	if (!m_bModSelected)
	{
		HMIMessageBox(MSG_OK, szTitle, szMsg);
		return FALSE;
	}

	if (!m_pCACF->m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", m_pCACF->GetStnName());
		HMIMessageBox(MSG_OK, szTitle, szMsg);
		return FALSE;
	}

	if (
		(m_pCACF->MoveZ(ACF_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS) 
		)
	{
		return GMP_FAIL;
	}

	switch (lSetupTitle)
	{
	case 0:
		nResult = SetupGoStandbyPosn();
		break;

	case 1:
		nResult = SetupGoBaseHeaterPosn();
		break;

	case 2:
		nResult = SetupGoLoadGlassPosn();
		break;

	case 3:
		nResult = SetupGoUnloadGlassPosn();
		break;
	
	case 4:
		nResult = SetupGoApproxBondPosn();
		break;

	case 5:
		nResult = SetupGoUnloadGlassPosnOffset();
		break;

	case 6:
		nResult = SetupGoUnloadGlassPosnOffset();
		break;

	case 7:
		nResult = SetupGoPRCalibPosn();
		break;

	case 8:
	case 9:
		break;

	case 10:
		nResult = SetupGoSafetyPosn();
		break;

	case 11:
		nResult = pCInspOpt->SetupGoStandbyPosn();
		break;

	case 12:
		nResult = SetupGoTStandbyPosn();
		break;

	case 13:
		nResult = SetupGoCORPosn();
		break;

	case 14:
		nResult = SetupGoTLoadOffsetPosn();
		break;

	}

	if (nResult != GMP_SUCCESS)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CACFWH::ModuleSetupSetLevel(LONG lSetupTitle)
{
	//LogAction(__FUNCTION__);
	//LogAction("m_lCurSlave = %ld", m_lCurSlave);

	//LONG	lAnswer							= rMSG_TIMEOUT;
	//LONG	lResponse						= 0;
	BOOL	bResult							= TRUE;
	//LONG	lOldLevel						= 0;
	//LONG	lTempLevel						= 0;
	//LONG	lLevel							= 0;
	//DOUBLE	dDistPerCnt						= 0.0;
	//DOUBLE	dThickness						= 0.0;

	//lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Overwrite Position?");

	//if (lAnswer == rMSG_CANCEL || lAnswer == rMSG_TIMEOUT)
	//{
	//	return FALSE;
	//}

	//if (g_stSetupLevel[lSetupTitle].lType == MACHINE_PARA && g_ulUserGroup < SERVICE)
	//{
	//	HMIMessageBox(MSG_OK, "WARNING", "You are not allowed to change Machine Parameters");
	//	return FALSE;
	//}


	//lTempLevel		= m_stMotorZ[WHZ_1].GetEncPosn();
	//dDistPerCnt		= m_stMotorZ[WHZ_1].stAttrib.dDistPerCount;


	//switch (lSetupTitle)
	//{
	//case 0:
	//	//standby level can't set
	//	break;

	//case 1:
	//	lOldLevel			= m_lCalibStageLevel[WHZ_1];
	//	
	//	m_lCalibStageLevel[WHZ_1]	= lTempLevel;
	//	if (
	//		!CalcWHAbsoluteLevel(m_lCalibStageLevel[WHZ_1], DoubleToLong(-0.3 * 1000.0), m_stWHCalibZ, &m_lStandbyLevel[WHZ_1], WHZ_1)
	//	   )
	//	{
	//		HMIMessageBox(MSG_OK, "WARNING", "Software Position Limit Hit.");
	//	}	

	//	//m_lStandbyLevel[WHZ_1] = m_lCalibStageLevel[WHZ_1] - DistanceToCount(300, dDistPerCnt);
	//	
	//	lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "OPTION", "Reset All Bond Level Offset?");

	//	if (lAnswer == rMSG_CONTINUE)
	//	{
	//		for (INT i = LSI1; i < MAX_NUM_OF_LSI; i++)
	//		{
	//			m_lBondLevelOffset[WHZ_1][i] = 0;
	//		}
	//	}
	//	else if (lAnswer == rMSG_TIMEOUT)
	//	{
	//		bResult = FALSE;
	//		break;
	//	}

	//	break;

	//case 2:
	//	lOldLevel			= m_lCalibLoadLevel[WHZ_1];

	//	m_lCalibLoadLevel[WHZ_1] = lTempLevel;
	//	
	//	lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "OPTION", "Reset Load Level Offset?");

	//	if (lAnswer == rMSG_CONTINUE)
	//	{
	//		m_lLoadLevelOffset[WHZ_1] = 0;
	//	}
	//	else if (lAnswer == rMSG_TIMEOUT)
	//	{
	//		bResult = FALSE;
	//		break;
	//	}

	//	break;

	//case 3:
	//	lOldLevel			= m_lCalibUnloadLevel[WHZ_1];
	//	
	//	m_lCalibUnloadLevel[WHZ_1] = lTempLevel;

	//	lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "OPTION", "Reset Unload Level Offset?");

	//	if (lAnswer == rMSG_CONTINUE)
	//	{
	//		m_lUnloadLevelOffset[WHZ_1] = 0;
	//	}
	//	else if (lAnswer == rMSG_TIMEOUT)
	//	{
	//		bResult = FALSE;
	//		break;
	//	}
	//	break;

	//case 4:
	//	lOldLevel			= m_lApproxBondLevel[WHZ_1];
	//	m_lApproxBondLevel[WHZ_1] = lTempLevel;
	//	break;

	//case 5:
	//	lOldLevel			= m_lLoadLevelOffset[WHZ_1];

	//	dThickness = g_stGlass.dThickness1 + g_stGlass.dThickness2 + g_stGlass.dThickness3 + g_stGlass.dThickness4;
	//	
	//	if (
	//		!CalcWHAbsoluteLevel(m_lCalibLoadLevel[WHZ_1], DoubleToLong(dThickness * 1000.0), m_stWHCalibZ, &lLevel, WHZ_1) ||
	//		!CalcWHLinearOffset(lLevel, lTempLevel, m_stWHCalibZ[WHZ_1], &m_lLoadLevelOffset[WHZ_1])
	//	   )
	//	{
	//		HMIMessageBox(MSG_OK, "WARNING", "Software Position Limit Hit. Operation Abort!");
	//		return FALSE;
	//	}	
	//	//m_lLoadLevelOffset[WHZ_1]	= lTempLevel - m_lCalibLoadLevel[WHZ_1] - DistanceToCount(1000 * dThickness, dDistPerCnt);
	//		
	//	lTempLevel	= m_lLoadLevelOffset[WHZ_1];

	//	break;

	//case 6:
	//	lOldLevel			= m_lUnloadLevelOffset[WHZ_1];

	//	dThickness = g_stGlass.dThickness1 + g_stGlass.dThickness2 + g_stGlass.dThickness3 + g_stGlass.dThickness4;

	//	if (
	//		!CalcWHAbsoluteLevel(m_lCalibUnloadLevel[WHZ_1], DoubleToLong(dThickness * 1000.0), m_stWHCalibZ, &lLevel, WHZ_1) ||
	//		!CalcWHLinearOffset(lLevel, lTempLevel, m_stWHCalibZ[WHZ_1], &m_lUnloadLevelOffset[WHZ_1])
	//	   )

	//	{
	//		HMIMessageBox(MSG_OK, "WARNING", "Software Position Limit Hit. Operation Abort!");
	//		return FALSE;
	//	}	
	//	//m_lUnloadLevelOffset[WHZ_1]	= lTempLevel - m_lCalibUnloadLevel[WHZ_1] - DistanceToCount(1000 * dThickness, dDistPerCnt);
	//		
	//	lTempLevel	= m_lUnloadLevelOffset[WHZ_1];

	//	break;
	//	
	//case 7:
	//	lOldLevel			= m_lBondLevelOffset[WHZ_1][ACF1];

	//	dThickness = g_stGlass.dThickness4;

	//	if (
	//		!CalcWHAbsoluteLevel(m_lCalibStageLevel[WHZ_1], DoubleToLong(dThickness * 1000.0), m_stWHCalibZ, &lLevel, WHZ_1) ||
	//		!CalcWHLinearOffset(lLevel, lTempLevel, m_stWHCalibZ[WHZ_1], &m_lBondLevelOffset[WHZ_1][ACF1])
	//	   )
	//	{
	//		HMIMessageBox(MSG_OK, "WARNING", "Software Position Limit Hit. Operation Abort!");
	//		return FALSE;
	//	}	

	//	//m_lBondLevelOffset[WHZ_1][ACF1] = lTempLevel - m_lCalibStageLevel[WHZ_1] - DistanceToCount(dThickness * 1000, dDistPerCnt);

	//	lTempLevel			= m_lBondLevelOffset[WHZ_1][ACF1];
	//	break;

	//}

//	CAC9000Stn::ModuleSetupSetLevel(lSetupTitle, lOldLevel, lTempLevel);

	return bResult;
}

BOOL CACFWH::ModuleSetupGoLevel(LONG lSetupTitle)
{
	//INT nResult = GMP_SUCCESS;

	//CString szTitle = "";
	//CString szMsg = "";
	//szTitle.Format("GO LEVEL OPERATION", m_lCurSlave + 1);
	//szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());

	//if (!m_bModSelected)
	//{
	//	HMIMessageBox(MSG_OK, szTitle, szMsg);
	//	return FALSE;
	//}
	//
	//// identify which WHZ
	//WHZNum	lCurWHZ	= WHZ_1;

	//if (lSetupTitle <= 10)
	//{
	//	lCurWHZ = WHZ_1;
	//}
	//else
	//{
	//	lCurWHZ	= WHZ_2;
	//}

	//switch (lSetupTitle)
	//{
	//case 0:
	//	nResult = SetupGoStandbyLevel();
	//	break;

	//case 1:
	//	nResult = SetupGoCalibStageLevel();
	//	break;

	//case 2:
	//	nResult = SetupGoCalibLoadLevel();
	//	break;

	//case 3:
	//	nResult = SetupGoCalibUnloadLevel();
	//	break;

	//case 4:
	//	nResult = SetupGoApproxBondLevel();
	//	break;

	//case 5:
	//	nResult = SetupGoLoadLevel();
	//	break;

	//case 6:
	//	nResult = SetupGoUnloadLevel();
	//	break;

	//case 7:
	//	nResult = SetupGoBondLevel(ACF1);
	//	break;
	//}
	//if (nResult != GMP_SUCCESS)
	//{
	//	return FALSE;
	//}

	return TRUE;
}

BOOL CACFWH::ModuleSetupSetPara(PARA stPara)
{
	LogAction(__FUNCTION__);
	LogAction("m_lCurSlave = %ld", m_lCurSlave);

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
		m_stWeakBlow2Sol.SetOnDelay(stPara.lPara);
		break;

	case 3:
		m_stWeakBlowSol.SetOffDelay(stPara.lPara);
		m_stWeakBlow2Sol.SetOffDelay(stPara.lPara);
		break;

	case 4: //20170103
		m_stAnvilVacSol.SetOnDelay(stPara.lPara);
		break;

	case 5: //20170103
		m_stAnvilVacSol.SetOffDelay(stPara.lPara);
		break;

	}

	return CAC9000Stn::ModuleSetupSetPara(stPara);
}

VOID CACFWH::UpdateModuleSetupPosn()
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	INT i = 0;
	INT j = 0;

	// Assign Module Setup Posn/Level/Para
	// Position Tab
	
	i = 0;
	g_stSetupPosn[i].szPosn.Format("Standby Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_mtrStandbyPosn.x;
	g_stSetupPosn[i].lPosnY			= m_mtrStandbyPosn.y;

	i++;	// 1
	g_stSetupPosn[i].szPosn.Format("Base Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_mtrBaseHeaterPosn.x;
	g_stSetupPosn[i].lPosnY			= m_mtrBaseHeaterPosn.y;

	i++;	// 2
	g_stSetupPosn[i].szPosn.Format("Load Glass Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_mtrLoadGlassPosn.x;
	g_stSetupPosn[i].lPosnY			= m_mtrLoadGlassPosn.y;

	i++;	// 3
	g_stSetupPosn[i].szPosn.Format("Unload Glass Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_mtrUnloadGlassPosn.x;
	g_stSetupPosn[i].lPosnY			= m_mtrUnloadGlassPosn.y;

	i++;	// 4
	g_stSetupPosn[i].szPosn.Format("Approx Bond Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_mtrApproxBondPosn.x;
	g_stSetupPosn[i].lPosnY			= m_mtrApproxBondPosn.y;
	
	i++;	// 5
	g_stSetupPosn[i].szPosn.Format("Load Glass Offset (um)");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= DEVICE_PARA;
	g_stSetupPosn[i].lPosnX			= 0;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 6
	g_stSetupPosn[i].szPosn.Format("Unload Glass Offset");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= DEVICE_PARA;
	g_stSetupPosn[i].lPosnX			= 0;
	g_stSetupPosn[i].lPosnY			= 0;

	// HMI Make Me 
	// Make Me like as AC9000
	i++;	// 7
	g_stSetupPosn[i].szPosn.Format("PR Calib Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_mtrPRCalibPosn.x;
	g_stSetupPosn[i].lPosnY			= m_mtrPRCalibPosn.y;

	i = 10;	// 10
	g_stSetupPosn[i].szPosn.Format("Safety Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_mtrSafetyPosn.x;
	g_stSetupPosn[i].lPosnY			= m_mtrSafetyPosn.y;

	i++;	// 11
	g_stSetupPosn[i].szPosn.Format("InspOpt Standby Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= pCInspOpt->m_mtrStandbyPosn.x;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 12
	g_stSetupPosn[i].szPosn.Format("T Standby Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lStandbyPosnT[EDGE_A];
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 13
	g_stSetupPosn[i].szPosn.Format("COR Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= DoubleToLong(m_dmtrTableCOR_Neg[m_lCORCamNum][0].x);
	g_stSetupPosn[i].lPosnY			= DoubleToLong(m_dmtrTableCOR_Neg[m_lCORCamNum][0].y);

	i++;	// 14
	g_stSetupPosn[i].szPosn.Format("T Load Offset (deg)");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= DEVICE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lLoadTOffset;
	g_stSetupPosn[i].lPosnY			= 0;
	m_dLoadTOffset = CountToDistance(m_lLoadTOffset, m_stMotorT.stAttrib.dDistPerCount);

	//i++;	// 12
	//g_stSetupPosn[i].szPosn.Format("T Load Glass Posn");
	//g_stSetupPosn[i].bEnable		= TRUE;
	//g_stSetupPosn[i].bVisible		= TRUE;
	//g_stSetupPosn[i].lType			= MACHINE_PARA;
	//g_stSetupPosn[i].lPosnX			= m_lTLoadGlassPosn;
	//g_stSetupPosn[i].lPosnY			= 0;

	//i++;	// 13
	//g_stSetupPosn[i].szPosn.Format("T Unload Glass Posn");
	//g_stSetupPosn[i].bEnable		= TRUE;
	//g_stSetupPosn[i].bVisible		= TRUE;
	//g_stSetupPosn[i].lType			= MACHINE_PARA;
	//g_stSetupPosn[i].lPosnX			= m_lTUnloadGlassPosn;
	//g_stSetupPosn[i].lPosnY			= 0;

	//i++;	// 14
	//g_stSetupPosn[i].szPosn.Format("T Approx Bond Posn");
	//g_stSetupPosn[i].bEnable		= TRUE;
	//g_stSetupPosn[i].bVisible		= TRUE;
	//g_stSetupPosn[i].lType			= MACHINE_PARA;
	//g_stSetupPosn[i].lPosnX			= m_lTApproxBondPosn;
	//g_stSetupPosn[i].lPosnY			= 0;

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

VOID CACFWH::UpdateModuleSetupLevel()
{
	INT i = 0;
	INT j = 0;

	//// Level Tab
	//i = 0;	// 0 // 10
	//g_stSetupLevel[i].szLevel.Format("Standby Level");
	//g_stSetupLevel[i].bEnable		= TRUE;
	//g_stSetupLevel[i].bVisible		= TRUE;
	//g_stSetupLevel[i].lType			= MACHINE_PARA;
	//g_stSetupLevel[i].lLevelZ		= m_lStandbyLevel[WHZ_1];

	//i++;	// 1
	//g_stSetupLevel[i].szLevel.Format("Calib. Stage Level");
	//g_stSetupLevel[i].bEnable		= TRUE;
	//g_stSetupLevel[i].bVisible		= TRUE;
	//g_stSetupLevel[i].lType			= MACHINE_PARA;
	//g_stSetupLevel[i].lLevelZ		= m_lCalibStageLevel[WHZ_1];

	//i++;	// 2
	//g_stSetupLevel[i].szLevel.Format("Calib. Load Level");
	//g_stSetupLevel[i].bEnable		= TRUE;
	//g_stSetupLevel[i].bVisible		= TRUE;
	//g_stSetupLevel[i].lType			= MACHINE_PARA;
	//g_stSetupLevel[i].lLevelZ		= m_lCalibLoadLevel[WHZ_1];

	//i++;	// 3
	//g_stSetupLevel[i].szLevel.Format("Calib. Unload Level");
	//g_stSetupLevel[i].bEnable		= TRUE;
	//g_stSetupLevel[i].bVisible		= TRUE;
	//g_stSetupLevel[i].lType			= MACHINE_PARA;
	//g_stSetupLevel[i].lLevelZ		= m_lCalibUnloadLevel[WHZ_1];

	//i++;	// 4
	//g_stSetupLevel[i].szLevel.Format("Approx Bond Level");
	//g_stSetupLevel[i].bEnable		= TRUE;
	//g_stSetupLevel[i].bVisible		= TRUE;
	//g_stSetupLevel[i].lType			= MACHINE_PARA;
	//g_stSetupLevel[i].lLevelZ		= m_lApproxBondLevel[WHZ_1];

	//i++;	// 5
	//g_stSetupLevel[i].szLevel.Format("Load Level Offset");
	//g_stSetupLevel[i].bEnable		= TRUE;
	//g_stSetupLevel[i].bVisible		= TRUE;
	//g_stSetupLevel[i].lType			= DEVICE_PARA;
	//g_stSetupLevel[i].lLevelZ		= m_lLoadLevelOffset[WHZ_1];

	//i++;	// 6
	//g_stSetupLevel[i].szLevel.Format("Unload Level Offset");
	//g_stSetupLevel[i].bEnable		= TRUE;
	//g_stSetupLevel[i].bVisible		= TRUE;
	//g_stSetupLevel[i].lType			= DEVICE_PARA;
	//g_stSetupLevel[i].lLevelZ		= m_lUnloadLevelOffset[WHZ_1];

	//i++;	// 7
	//g_stSetupLevel[i].szLevel.Format("Bond Level Offset");
	//g_stSetupLevel[i].bEnable		= TRUE;
	//g_stSetupLevel[i].bVisible		= TRUE;
	//g_stSetupLevel[i].lType			= DEVICE_PARA;
	//g_stSetupLevel[i].lLevelZ		= m_lBondLevelOffset[WHZ_1][ACF1];

	for (j = i + 1; j < NUM_OF_LEVEL_Z; j++)
	{
		g_stSetupLevel[j].szLevel.Format("Reserved");
		g_stSetupLevel[j].bEnable		= FALSE;
		g_stSetupLevel[j].bVisible		= FALSE;
		g_stSetupLevel[j].lType			= MACHINE_PARA;
		g_stSetupLevel[j].lLevelZ		= 0;
	}
}

VOID CACFWH::UpdateModuleSetupPara()
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

	i++; //4 20170103
	g_stSetupPara[i].szPara.Format("Anvil Vac On Delay (ms)");
	g_stSetupPara[i].bEnable		= TRUE;
	g_stSetupPara[i].bVisible		= TRUE;
	g_stSetupPara[i].lType			= DEVICE_PARA;
	g_stSetupPara[i].lPara			= m_stAnvilVacSol.GetOnDelay();
	g_stSetupPara[i].dPara			= 0.0;

	i++; //5 20170103
	g_stSetupPara[i].szPara.Format("Anvil Vac Off Delay (ms)");
	g_stSetupPara[i].bEnable		= TRUE;
	g_stSetupPara[i].bVisible		= TRUE;
	g_stSetupPara[i].lType			= DEVICE_PARA;
	g_stSetupPara[i].lPara			= m_stAnvilVacSol.GetOffDelay();
	g_stSetupPara[i].dPara			= 0.0;
	
	// Others
	for (j = i + 1; j < NUM_OF_PARA; j++)
	{
		g_stSetupPara[j].szPara.Format("Reserved");
		g_stSetupPara[j].bEnable		= FALSE;
		g_stSetupPara[j].bVisible		= FALSE;
		g_stSetupPara[j].lType			= MACHINE_PARA;
		g_stSetupPara[j].lPara			= 0;
		g_stSetupPara[j].dPara			= 0.0;
	}
}

INT CACFWH::SetupGoStandbyPosn()
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	INT nResult = GMP_SUCCESS;

	if (
		pCInspOpt->MoveX(INSPOPT_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT) != GMP_SUCCESS ||
		ACFMoveAbsoluteXY(m_pCACF->m_lStandbyPosnACF, m_mtrStandbyPosn.y, GMP_WAIT) != GMP_SUCCESS
	   )
	{
		nResult = GMP_FAIL;
	}

	return nResult;
}

INT CACFWH::SetupGoBaseHeaterPosn()
{
	INT nResult = GMP_SUCCESS;

	if (
		ACFMoveAbsoluteXY(m_mtrBaseHeaterPosn.x, m_mtrBaseHeaterPosn.y, GMP_WAIT) != GMP_SUCCESS
	   )
	{
		nResult = GMP_FAIL;
	}

	return nResult;
}

INT CACFWH::SetupGoLoadGlassPosn()
{
	INT nResult = GMP_SUCCESS;

	if (
		//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS || 
		MoveY(ACF_WH_Y_CTRL_GO_LOAD_POSN, GMP_WAIT) != GMP_SUCCESS
	   )
	{
		nResult = GMP_FAIL;
	}

	return nResult;
}

INT CACFWH::SetupGoUnloadGlassPosn()
{
	INT nResult = GMP_SUCCESS;
	
	if (
		//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS || 
		MoveY(ACF_WH_Y_CTRL_GO_UNLOAD_POSN, GMP_WAIT) != GMP_SUCCESS
	   )
	{
		nResult = GMP_FAIL;
	}

	return nResult;
}

INT CACFWH::SetupGoApproxBondPosn()
{
	INT nResult = GMP_SUCCESS;

	if (
		ACFMoveAbsoluteXY(m_mtrApproxBondPosn.x, m_mtrApproxBondPosn.y, GMP_WAIT) != GMP_SUCCESS
	   )
	{
		nResult = GMP_FAIL;
	}

	return nResult;
}

INT CACFWH::SetupGoUnloadGlassPosnOffset()
{
	INT nResult = GMP_SUCCESS;
	
	if (
		//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS || 
		MoveY(ACF_WH_Y_CTRL_GO_UNLOAD_POSN_OFFSET, GMP_WAIT) != GMP_SUCCESS
	   )
	{
		nResult = GMP_FAIL;
	}

	return nResult;
}

INT CACFWH::SetupGoBondPosn(ACFNum lACFNum)
{
	INT nResult = GMP_SUCCESS;
	
	if (
		//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS || 
		MoveYToBondPosn(lACFNum) != GMP_SUCCESS
	   )
	{
		nResult = GMP_FAIL;
	}

	return nResult;
}

INT CACFWH::SetupGoPRCalibPosn()
{
	INT nResult = GMP_SUCCESS;
	
	if (
		OpticMoveAbsoluteXY(m_mtrPRCalibPosn.x, m_mtrPRCalibPosn.y, GMP_WAIT) != GMP_SUCCESS
	   )
	{
		nResult = GMP_FAIL;
	}

	return nResult;
}

INT CACFWH::SetupGoSafetyPosn()
{
	INT nResult = GMP_SUCCESS;
	
	if (
		ACFMoveAbsoluteXY(m_mtrSafetyPosn.x, m_mtrSafetyPosn.y, GMP_WAIT) != GMP_SUCCESS
	   )
	{
		nResult = GMP_FAIL;
	}

	return nResult;
}

INT CACFWH::SetupGoCORPosn()
{
	INT nResult = GMP_SUCCESS;
	
	if (
		OpticMoveAbsoluteXY(DoubleToLong(m_dmtrTableCOR_Neg[m_lCORCamNum][0].x), DoubleToLong(m_dmtrTableCOR_Neg[m_lCORCamNum][0].y), GMP_WAIT) != GMP_SUCCESS
	   )
	{
		nResult = GMP_FAIL;
	}

	return nResult;
}

INT CACFWH::SetupGoTStandbyPosn()
{
	INT nResult = GMP_SUCCESS;
	
	if (
		MoveT(ACF_WH_T_CTRL_GO_STANDBY, GMP_WAIT) != GMP_SUCCESS
	   )
	{
		nResult = GMP_FAIL;
	}

	return nResult;
}

INT CACFWH::SetupGoTLoadOffsetPosn()
{
	INT nResult = GMP_SUCCESS;
	
	if (
		MoveT(ACF_WH_T_CTRL_GO_LOAD_POSN_OFFSET, GMP_WAIT) != GMP_SUCCESS
	   )
	{
		nResult = GMP_FAIL;
	}

	return nResult;
}

INT CACFWH::SetupGoStandbyLevel()
{
	INT nResult = GMP_SUCCESS;
		
	//nResult = MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT);

	return nResult;
}

INT CACFWH::SetupGoCalibStageLevel()
{
	INT nResult = GMP_SUCCESS;
	CString str;

	LONG	lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "Move Y to Approx Bond Posn?");
	if (lReply == BUTTON_1)
	{
		if (
			//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS || 
			MoveY(ACF_WH_Y_CTRL_GO_APPROX_BOND_POSN, GMP_WAIT) != GMP_SUCCESS
		   )
		{
			return GMP_FAIL;
		}

	}
	else if (lReply == BUTTON_3)
	{	
		return GMP_FAIL;
	}

	//nResult = MoveZ(ACF_WH_Z_CTRL_GO_CALIB_STAGE_LEVEL, GMP_WAIT);

	return nResult;
}


INT CACFWH::SetupGoCalibLoadLevel()
{
	INT nResult = GMP_SUCCESS;
	CString str;

	LONG	lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "Move Y to Calib Load Glass Posn?");
	if (lReply == BUTTON_1)
	{
		if (
			//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS || 
			MoveY(ACF_WH_Y_CTRL_GO_LOAD_POSN, GMP_WAIT) != GMP_SUCCESS
		   )
		{
			return GMP_FAIL;
		}

	}
	else if (lReply == BUTTON_3)
	{	
		return GMP_FAIL;
	}

	//nResult = MoveZ(ACF_WH_Z_CTRL_GO_CALIB_LOAD_LEVEL, GMP_WAIT);
	
	return nResult;
}

INT CACFWH::SetupGoCalibUnloadLevel()
{
	INT nResult = GMP_SUCCESS;
	CString str;

	LONG	lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "Move Y to Calib Unload Glass Posn?");
	if (lReply == BUTTON_1)
	{
		if (
			//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS || 
			MoveY(ACF_WH_Y_CTRL_GO_UNLOAD_POSN, GMP_WAIT) != GMP_SUCCESS
		   )
		{
			return GMP_FAIL;
		}

	}
	else if (lReply == BUTTON_3)
	{	
		return GMP_FAIL;
	}

	//nResult = MoveZ(ACF_WH_Z_CTRL_GO_CALIB_UNLOAD_LEVEL, GMP_WAIT);
	
	return nResult;

}


INT CACFWH::SetupGoApproxBondLevel()
{
	INT nResult = GMP_SUCCESS;
	CString str;

	LONG	lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "Move Y to Approx Bond Posn?");
	if (lReply == BUTTON_1)
	{
		if (
			//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS || 
			MoveY(ACF_WH_Y_CTRL_GO_APPROX_BOND_POSN, GMP_WAIT) != GMP_SUCCESS
		   )
		{
			return GMP_FAIL;
		}

	}
	else if (lReply == BUTTON_3)
	{	
		return GMP_FAIL;
	}

	//nResult = MoveZ(ACF_WH_Z_CTRL_GO_APPROX_BOND_LEVEL, GMP_WAIT);
	return nResult;
}

INT CACFWH::SetupGoLoadLevel()
{
	INT nResult = GMP_SUCCESS;
	CString str;

	LONG	lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "Move Y to Load Glass Posn?");
	if (lReply == BUTTON_1)
	{
		if (
			//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS || 
			MoveY(ACF_WH_Y_CTRL_GO_LOAD_POSN_OFFSET, GMP_WAIT) != GMP_SUCCESS
		   )
		{
			return GMP_FAIL;
		}

	}
	else if (lReply == BUTTON_3)
	{	
		return GMP_FAIL;
	}

	//nResult = MoveZ(ACF_WH_Z_CTRL_GO_LOAD_LEVEL, GMP_WAIT);
	
	return nResult;
}

INT CACFWH::SetupGoUnloadLevel()
{
	INT nResult = GMP_SUCCESS;
	CString str;

	LONG	lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "Move Y to Unload Glass Posn?");
	if (lReply == BUTTON_1)
	{
		if (
			//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS || 
			MoveY(ACF_WH_Y_CTRL_GO_UNLOAD_POSN_OFFSET, GMP_WAIT) != GMP_SUCCESS
		   )
		{
			return GMP_FAIL;
		}

	}
	else if (lReply == BUTTON_3)
	{	
		return GMP_FAIL;
	}

	//nResult = MoveZ(ACF_WH_Z_CTRL_GO_UNLOAD_LEVEL, GMP_WAIT);

	return nResult;
}

INT CACFWH::SetupGoBondLevel(LONG lLSINum, BOOL bIsInteractive)
{
	INT nResult = GMP_SUCCESS;
	CString str;

	LONG lReply = BUTTON_1;
	if (bIsInteractive) //20130318
	{
		lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "Move Y to Bond Posn?");
	}
	if (lReply == BUTTON_1)
	{
		if (
			//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS || 
			MoveYToBondPosn(lLSINum) != GMP_SUCCESS
		   )
		{
			return GMP_FAIL;
		}

	}
	else if (lReply == BUTTON_3)
	{	
		return GMP_FAIL;
	}

	//nResult = MoveZToBondLevel(lLSINum);

	return nResult;
}

//BOOL CACFWH::IsAnvilVacSensorOn() //20170103
//{
//	BOOL bResult = FALSE;
//
//#ifdef OFFLINE
//	return FALSE;
//#endif 
//
//#ifdef _DEBUG	// 20140917 Yip
//	if (!theApp.IsInitNuDrive())
//	{
//		return FALSE;
//	}
//#endif
//
//	try
//	{
//		bResult = m_stAnvilVacSnr.GetGmpPort().IsOn();
//	}
//	catch (CAsmException e)
//	{
//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//		DisplayException(e);
//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//	}
//
//	return bResult;
//}

//INT CACFWH::SetAnvilVacSol(BOOL bMode, BOOL bWait) //20170103
//{
//	INT nResult = GMP_SUCCESS;
//
//	CSoInfo *stTempSo = &m_stAnvilVacSol;
//
//	if (!m_fHardware) //20160128
//	{
//		return GMP_SUCCESS;
//	}
//
//	if(IsBurnInDiagBond())
//	{
//		Sleep(100);
//		stTempSo->bHMI_Status = bMode;
//		return GMP_SUCCESS;
//	} 
//
//	if (!g_bEnableAnvilVac && bMode == ON &&
//		(State() == AUTO_Q || State() == DEMO_Q || State() == PRESTART_Q || State() == STOPPING_Q)) //20170816
//	{
//		Sleep(5);
//		return GMP_SUCCESS;
//	}
//#ifdef OFFLINE
//	stTempSo->bHMI_Status = bMode;
//	return nResult;
//#endif 
//
//#ifdef _DEBUG	// 20140917 Yip
//	if (!theApp.IsInitNuDrive())
//	{
//		stTempSo->bHMI_Status = bMode;
//		return nResult;
//	}
//#endif
//
//	try
//	{
//		if (bMode)
//		{
//			TC_AddJob(stTempSo->GetTCObjectID());
//
//			if (!g_bDiagMode)
//			{
//				stTempSo->GetGmpPort().On();
//				stTempSo->bHMI_Status = TRUE;
//			}
//			else
//			{
//				stTempSo->GetGmpPort().Off();
//				stTempSo->bHMI_Status = FALSE;
//			}
//
//			if (bWait)
//			{
//				Sleep(stTempSo->GetOnDelay());
//			}
//
//			TC_DelJob(stTempSo->GetTCObjectID());
//		}
//		else
//		{
//			TC_AddJob(stTempSo->GetTCObjectID());
//
//			stTempSo->GetGmpPort().Off();
//			stTempSo->bHMI_Status = FALSE;
//
//			if (bWait)
//			{
//				Sleep(stTempSo->GetOffDelay());
//			}
//
//			TC_DelJob(stTempSo->GetTCObjectID());
//		}
//	}
//	catch (CAsmException e)
//	{
//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//		DisplayException(e);
//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//	}
//
//	return nResult;
//}


///////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//PR Related
/////////////////////////////////////////////////////////////////
//BOOL CACFWH::AutoSearchPR1(EdgeNum lEdge)
//{
//	MTR_POSN mtrTemp;
//	MTR_POSN mtr_offset;
//
//	if (lEdge <= EDGE_NONE || lEdge >= MAX_EDGE_OF_GLASS)
//	{
//		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
//		DisplayMessage(__FUNCTION__);
//		return FALSE;
//	}
//
//	CalPosnAfterRotate(lEdge, m_mtrPR1Posn[lEdge], m_dBondAngle, &mtrTemp);
//	m_dmtrCurrPR1Posn[lEdge].x = (DOUBLE)mtrTemp.x;
//	m_dmtrCurrPR1Posn[lEdge].y = (DOUBLE)mtrTemp.y;
//
//	if (
//		//MoveX
//		(m_nLastError = MoveYToPR1Posn(lEdge, GMP_NOWAIT)) != GMP_SUCCESS || 
//		//(m_nLastError = MoveZ(ACF_WH_Z_CTRL_GO_PR_LEVEL, GMP_NOWAIT)) != GMP_SUCCESS || 
//		//(m_nLastError = SyncX()) != GMP_SUCCESS || 
//		(m_nLastError = SyncY()) != GMP_SUCCESS //|| 
//		//(m_nLastError = SyncZ()) != GMP_SUCCESS 
//	)
//	{
//		return FALSE;
//	}
//
//	if (!m_bPRSelected)
//	{
//		Sleep(m_lPRDelay);
//		return TRUE;
//	}
//
//	if (!SearchPR(&pruACF1))
//	{
//		return FALSE;
//	}
//
//	PRS_PRPosToMtrOffset(&pruACF1, pruACF1.rcoDieCentre, &mtr_offset);
//
//	m_dmtrCurrPR1Posn[lEdge].x += (DOUBLE)mtr_offset.x;
//	m_dmtrCurrPR1Posn[lEdge].y += (DOUBLE)mtr_offset.y;
//
//	return TRUE;
//}
//
//BOOL CACFWH::AutoSearchPR1GrabOnly(EdgeNum lEdge)
//{
//	// debug time
//	//DOUBLE dProcessTime = 0.0;
//	//DWORD dwStartTime = GetTickCount();
//
//	m_nPRInProgress = ST_PR1_IN_PROGRESS;
//
//	if (!m_bPRSelected)
//	{
//		return TRUE;
//	}
//
//	if (lEdge <= EDGE_NONE || lEdge >= MAX_EDGE_OF_GLASS)
//	{
//		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
//		DisplayMessage(__FUNCTION__);
//		return FALSE;
//	}
//
//	if (!SearchPRGrabOnly(&pruACF1))
//	{
//		m_nPRInProgress = ST_PR_IDLE;
//		return FALSE;
//	}
//
//	// debug time
//	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
//	//CString szMsg = "";
//	//szMsg.Format("ACF PR1 Grab Time: %.5f", dProcessTime);
//	//DisplayMessage(szMsg);
//
//	return TRUE;
//}
//
//BOOL CACFWH::AutoSearchPR1ProcessOnly(EdgeNum lEdge)
//{
//	// debug time
//	//DOUBLE dProcessTime = 0.0;
//	//DWORD dwStartTime = GetTickCount();
//
//	MTR_POSN mtr_offset;
//
//	if (!m_bPRSelected)
//	{
//		m_nPRInProgress = ST_PR_IDLE;
//		return TRUE;
//	}
//
//	if (lEdge <= EDGE_NONE || lEdge >= MAX_EDGE_OF_GLASS)
//	{
//		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
//		DisplayMessage(__FUNCTION__);
//		return FALSE;
//	}
//
//	if (!SearchPRProcessOnly(&pruACF1))
//	{
//		m_nPRInProgress = ST_PR_IDLE;
//		return FALSE;
//	}
//
//	PRS_PRPosToMtrOffset(&pruACF1, pruACF1.rcoDieCentre, &mtr_offset);
//
//	m_dmtrCurrPR1Posn[lEdge].x += (DOUBLE)mtr_offset.x;
//	m_dmtrCurrPR1Posn[lEdge].y += (DOUBLE)mtr_offset.y;
//
//	m_nPRInProgress = ST_PR_IDLE;
//
//	// debug time
//	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
//	//CString szMsg = "";
//	//szMsg.Format("ACF PR1 Process Time: %.5f", dProcessTime);
//	//DisplayMessage(szMsg);
//
//	return TRUE;
//}
//
//BOOL CACFWH::AutoSearchPR2(EdgeNum lEdge)
//{
//	MTR_POSN mtrTemp;
//	MTR_POSN mtr_offset;
//
//	if (lEdge <= EDGE_NONE || lEdge >= MAX_EDGE_OF_GLASS)
//	{
//		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
//		DisplayMessage(__FUNCTION__);
//		return FALSE;
//	}
//
//	CalPosnAfterRotate(lEdge, m_mtrPR2Posn[lEdge], m_dBondAngle, &mtrTemp);
//	m_dmtrCurrPR2Posn[lEdge].x = (DOUBLE)mtrTemp.x;
//	m_dmtrCurrPR2Posn[lEdge].y = (DOUBLE)mtrTemp.y;
//
//	if (//MoveX
//		(m_nLastError = MoveYToPR2Posn(lEdge, GMP_NOWAIT)) != GMP_SUCCESS || 
//		//(m_nLastError = MoveZ(ACF_WH_Z_CTRL_GO_PR_LEVEL, GMP_NOWAIT)) != GMP_SUCCESS || 
//		//(m_nLastError = SyncX()) != GMP_SUCCESS || 
//		(m_nLastError = SyncY()) != GMP_SUCCESS //|| 
//		//(m_nLastError = SyncZ()) != GMP_SUCCESS 
//	)
//	{
//		return FALSE;
//	}
//
//	if (!m_bPRSelected)
//	{
//		Sleep(m_lPRDelay);
//		return TRUE;
//	}
//
//	if (!SearchPR(&pruACF2))
//	{
//		return FALSE;
//	}
//
//	PRS_PRPosToMtrOffset(&pruACF2, pruACF2.rcoDieCentre, &mtr_offset);
//
//	m_dmtrCurrPR2Posn[lEdge].x += (DOUBLE)mtr_offset.x;
//	m_dmtrCurrPR2Posn[lEdge].y += (DOUBLE)mtr_offset.y;
//
//	return TRUE;
//}
//
//BOOL CACFWH::AutoSearchPR2GrabOnly(EdgeNum lEdge)
//{
//	// debug time
//	//DOUBLE dProcessTime = 0.0;
//	//DWORD dwStartTime = GetTickCount();
//
//	m_nPRInProgress = ST_PR2_IN_PROGRESS;
//
//	if (!m_bPRSelected)
//	{
//		return TRUE;
//	}
//
//	if (lEdge <= EDGE_NONE || lEdge >= MAX_EDGE_OF_GLASS)
//	{
//		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
//		DisplayMessage(__FUNCTION__);
//		return FALSE;
//	}
//
//	if (!SearchPRGrabOnly(&pruACF2))
//	{
//		m_nPRInProgress = ST_PR_IDLE;
//		return FALSE;
//	}
//
//	// debug time
//	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
//	//CString szMsg = "";
//	//szMsg.Format("ACF PR2 Grab Time: %.5f", dProcessTime);
//	//DisplayMessage(szMsg);
//
//	return TRUE;
//}
//
//BOOL CACFWH::AutoSearchPR2ProcessOnly(EdgeNum lEdge)
//{
//	// debug time
//	//DOUBLE dProcessTime = 0.0;
//	//DWORD dwStartTime = GetTickCount();
//
//	MTR_POSN mtr_offset;
//
//	if (!m_bPRSelected)
//	{
//		m_nPRInProgress = ST_PR_IDLE;
//		return TRUE;
//	}
//
//	if (lEdge <= EDGE_NONE || lEdge >= MAX_EDGE_OF_GLASS)
//	{
//		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
//		DisplayMessage(__FUNCTION__);
//		return FALSE;
//	}
//
//	if (!SearchPRProcessOnly(&pruACF2))
//	{
//		m_nPRInProgress = ST_PR_IDLE;
//		return FALSE;
//	}
//
//	PRS_PRPosToMtrOffset(&pruACF2, pruACF2.rcoDieCentre, &mtr_offset);
//
//	m_dmtrCurrPR2Posn[lEdge].x += (DOUBLE)mtr_offset.x;
//	m_dmtrCurrPR2Posn[lEdge].y += (DOUBLE)mtr_offset.y;
//
//	m_nPRInProgress = ST_PR_IDLE;
//
//	// debug time
//	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
//	//CString szMsg = "";
//	//szMsg.Format("ACF PR2 Process Time: %.5f", dProcessTime);
//	//DisplayMessage(szMsg);
//
//	return TRUE;
//}

BOOL CACFWH::AutoSearchPBPR1()
{
	MTR_POSN mtrTemp;

	mtrTemp.x = (LONG)m_dmtrCurrPBPR1Posn.x;
	mtrTemp.y = (LONG)m_dmtrCurrPBPR1Posn.y;
	CalPosnAfterRotate(m_lCurEdge, mtrTemp, m_dBondAngle, &mtrTemp);
	m_dmtrCurrPBPR1Posn.x = (DOUBLE)mtrTemp.x;
	m_dmtrCurrPBPR1Posn.y = (DOUBLE)mtrTemp.y;

	if (
		//MoveX
		(m_nLastError = OpticMoveXY(ACF_WH_XY_CTRL_GO_PBPR1_POSN, GMP_NOWAIT)) != GMP_SUCCESS || 
		//(m_nLastError = MoveZ(ACF_WH_Z_CTRL_GO_PR_LEVEL, GMP_NOWAIT)) != GMP_SUCCESS || 
		//(m_nLastError = SyncX()) != GMP_SUCCESS || 
		(m_nLastError = SyncY()) != GMP_SUCCESS //|| 
		//(m_nLastError = SyncZ()) != GMP_SUCCESS 
	)
	{
		return FALSE;
	}

	if (!m_bPRSelected)
	{
		Sleep(m_lPRDelay);
		return TRUE;
	}

	if (m_ulPBMode == 0)
	{
		if (!SearchPR(&pruACFPB1[m_lCurACF][OBJECT]))
		{
			return FALSE;
		}

		m_PRCurrPBPR1Corner[PR_UPPER_LEFT].x = pruACFPB1[m_lCurACF][OBJECT].arcoDieCorners[PR_UPPER_LEFT].x;
		m_PRCurrPBPR1Corner[PR_UPPER_LEFT].y = pruACFPB1[m_lCurACF][OBJECT].arcoDieCorners[PR_UPPER_LEFT].y;
		m_PRCurrPBPR1Corner[PR_LOWER_LEFT].x = pruACFPB1[m_lCurACF][OBJECT].arcoDieCorners[PR_LOWER_LEFT].x;
		m_PRCurrPBPR1Corner[PR_LOWER_LEFT].y = pruACFPB1[m_lCurACF][OBJECT].arcoDieCorners[PR_LOWER_LEFT].y;
		m_PRCurrPBPR1Corner[PR_UPPER_RIGHT].x = pruACFPB1[m_lCurACF][OBJECT].arcoDieCorners[PR_UPPER_RIGHT].x;
		m_PRCurrPBPR1Corner[PR_UPPER_RIGHT].y = pruACFPB1[m_lCurACF][OBJECT].arcoDieCorners[PR_UPPER_RIGHT].y;
		m_PRCurrPBPR1Corner[PR_LOWER_RIGHT].x = pruACFPB1[m_lCurACF][OBJECT].arcoDieCorners[PR_LOWER_RIGHT].x;
		m_PRCurrPBPR1Corner[PR_LOWER_RIGHT].y = pruACFPB1[m_lCurACF][OBJECT].arcoDieCorners[PR_LOWER_RIGHT].y;


	}
	else
	{
		if (!CheckPRPBStat(&pruACFPB1[m_lCurACF][OBJECT]))
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CACFWH::AutoSearchPBPR1GrabOnly(LONG lACFNum)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();

	m_nPRInProgress = ST_PB_PR1_IN_PROGRESS;

	if (!m_bPRSelected)
	{
		return TRUE;
	}

	if (m_ulPBMode == 0)
	{
		if (!SearchPRGrabOnly(&pruACFPB1[lACFNum][OBJECT]))
		{
			m_nPRInProgress = ST_PR_IDLE;
			return FALSE;
		}
	}
	else
	{
		if (!CheckPRPBStat(&pruACFPB1[lACFNum][OBJECT]))
		{
			m_nPRInProgress = ST_PR_IDLE;
			return FALSE;
		}
	}

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("ACF Post-PR1 Grab Time: %.5f", dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}

BOOL CACFWH::AutoSearchPBPR1ProcessOnly(LONG lACFNum)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();

	if (!m_bPRSelected)
	{
		m_nPRInProgress = ST_PR_IDLE;
		return TRUE;
	}

	if (lACFNum <= NO_ACF || lACFNum >= MAX_NUM_OF_ACF)
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage(__FUNCTION__);
		return FALSE;
	}

	if (m_ulPBMode == 0)
	{
		if (!SearchPRProcessOnly(&pruACFPB1[lACFNum][OBJECT]))
		{
			m_nPRInProgress = ST_PR_IDLE;
			return FALSE;
		}

		m_PRCurrPBPR1Corner[PR_UPPER_LEFT].x = pruACFPB1[lACFNum][OBJECT].arcoDieCorners[PR_UPPER_LEFT].x;
		m_PRCurrPBPR1Corner[PR_UPPER_LEFT].y = pruACFPB1[lACFNum][OBJECT].arcoDieCorners[PR_UPPER_LEFT].y;
		m_PRCurrPBPR1Corner[PR_LOWER_LEFT].x = pruACFPB1[lACFNum][OBJECT].arcoDieCorners[PR_LOWER_LEFT].x;
		m_PRCurrPBPR1Corner[PR_LOWER_LEFT].y = pruACFPB1[lACFNum][OBJECT].arcoDieCorners[PR_LOWER_LEFT].y;
		m_PRCurrPBPR1Corner[PR_UPPER_RIGHT].x = pruACFPB1[lACFNum][OBJECT].arcoDieCorners[PR_UPPER_RIGHT].x;
		m_PRCurrPBPR1Corner[PR_UPPER_RIGHT].y = pruACFPB1[lACFNum][OBJECT].arcoDieCorners[PR_UPPER_RIGHT].y;
		m_PRCurrPBPR1Corner[PR_LOWER_RIGHT].x = pruACFPB1[lACFNum][OBJECT].arcoDieCorners[PR_LOWER_RIGHT].x;
		m_PRCurrPBPR1Corner[PR_LOWER_RIGHT].y = pruACFPB1[lACFNum][OBJECT].arcoDieCorners[PR_LOWER_RIGHT].y;

	}

	m_nPRInProgress = ST_PR_IDLE;

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("ACF Post-PR1 Process Time: %.5f", dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}

BOOL CACFWH::AutoSearchPBPR2()
{
	MTR_POSN mtrTemp;

	mtrTemp.x = (LONG)m_dmtrCurrPBPR2Posn.x;
	mtrTemp.y = (LONG)m_dmtrCurrPBPR2Posn.y;
	CalPosnAfterRotate(m_lCurEdge, mtrTemp, m_dBondAngle, &mtrTemp);
	m_dmtrCurrPBPR2Posn.x = (DOUBLE)mtrTemp.x;
	m_dmtrCurrPBPR2Posn.y = (DOUBLE)mtrTemp.y;

	if (
		//MoveX
		(m_nLastError = OpticMoveXY(ACF_WH_XY_CTRL_GO_PBPR2_POSN, GMP_NOWAIT)) != GMP_SUCCESS || 
		//(m_nLastError = MoveZ(ACF_WH_Z_CTRL_GO_PR_LEVEL, GMP_NOWAIT)) != GMP_SUCCESS || 
		//(m_nLastError = SyncX()) != GMP_SUCCESS || 
		(m_nLastError = SyncY()) != GMP_SUCCESS //|| 
		//(m_nLastError = SyncZ()) != GMP_SUCCESS 
	)
	{
		return FALSE;
	}

	if (!m_bPRSelected)
	{
		Sleep(m_lPRDelay);
		return TRUE;
	}

	if (m_ulPBMode == 0)
	{
		if (!SearchPR(&pruACFPB2[m_lCurACF][OBJECT]))
		{
			return FALSE;
		}


		m_PRCurrPBPR2Corner[PR_UPPER_LEFT].x = pruACFPB2[m_lCurACF][OBJECT].arcoDieCorners[PR_UPPER_LEFT].x;
		m_PRCurrPBPR2Corner[PR_UPPER_LEFT].y = pruACFPB2[m_lCurACF][OBJECT].arcoDieCorners[PR_UPPER_LEFT].y;
		m_PRCurrPBPR2Corner[PR_LOWER_LEFT].x = pruACFPB2[m_lCurACF][OBJECT].arcoDieCorners[PR_LOWER_LEFT].x;
		m_PRCurrPBPR2Corner[PR_LOWER_LEFT].y = pruACFPB2[m_lCurACF][OBJECT].arcoDieCorners[PR_LOWER_LEFT].y;
		m_PRCurrPBPR2Corner[PR_UPPER_RIGHT].x = pruACFPB2[m_lCurACF][OBJECT].arcoDieCorners[PR_UPPER_RIGHT].x;
		m_PRCurrPBPR2Corner[PR_UPPER_RIGHT].y = pruACFPB2[m_lCurACF][OBJECT].arcoDieCorners[PR_UPPER_RIGHT].y;
		m_PRCurrPBPR2Corner[PR_LOWER_RIGHT].x = pruACFPB2[m_lCurACF][OBJECT].arcoDieCorners[PR_LOWER_RIGHT].x;
		m_PRCurrPBPR2Corner[PR_LOWER_RIGHT].y = pruACFPB2[m_lCurACF][OBJECT].arcoDieCorners[PR_LOWER_RIGHT].y;
	}
	else
	{
		if (!CheckPRPBStat(&pruACFPB2[m_lCurACF][OBJECT]))
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CACFWH::AutoSearchPBPR2GrabOnly(LONG lACFNum)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();

	m_nPRInProgress = ST_PB_PR2_IN_PROGRESS;

	if (!m_bPRSelected)
	{
		return TRUE;
	}

	if (m_ulPBMode == ST_PR_IDLE)
	{
		if (!SearchPRGrabOnly(&pruACFPB2[lACFNum][OBJECT]))
		{
			m_nPRInProgress = ST_PR_IDLE;
			return FALSE;
		}
	}
	else
	{
		if (!CheckPRPBStat(&pruACFPB2[lACFNum][OBJECT]))
		{
			m_nPRInProgress = ST_PR_IDLE;
			return FALSE;
		}
	}

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("ACF Post-PR2 Grab Time: %.5f", dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}

BOOL CACFWH::AutoSearchPBPR2ProcessOnly(LONG lACFNum)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();

	if (!m_bPRSelected)
	{
		m_nPRInProgress = ST_PR_IDLE;
		return TRUE;
	}

	if (lACFNum <= NO_ACF || lACFNum >= MAX_NUM_OF_ACF)
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage(__FUNCTION__);
		return FALSE;
	}

	if (m_ulPBMode == 0)
	{
		if (!SearchPRProcessOnly(&pruACFPB2[lACFNum][OBJECT]))
		{
			m_nPRInProgress = ST_PR_IDLE;
			return FALSE;
		}

		m_PRCurrPBPR2Corner[PR_UPPER_LEFT].x = pruACFPB2[lACFNum][OBJECT].arcoDieCorners[PR_UPPER_LEFT].x;
		m_PRCurrPBPR2Corner[PR_UPPER_LEFT].y = pruACFPB2[lACFNum][OBJECT].arcoDieCorners[PR_UPPER_LEFT].y;
		m_PRCurrPBPR2Corner[PR_LOWER_LEFT].x = pruACFPB2[lACFNum][OBJECT].arcoDieCorners[PR_LOWER_LEFT].x;
		m_PRCurrPBPR2Corner[PR_LOWER_LEFT].y = pruACFPB2[lACFNum][OBJECT].arcoDieCorners[PR_LOWER_LEFT].y;
		m_PRCurrPBPR2Corner[PR_UPPER_RIGHT].x = pruACFPB2[lACFNum][OBJECT].arcoDieCorners[PR_UPPER_RIGHT].x;
		m_PRCurrPBPR2Corner[PR_UPPER_RIGHT].y = pruACFPB2[lACFNum][OBJECT].arcoDieCorners[PR_UPPER_RIGHT].y;
		m_PRCurrPBPR2Corner[PR_LOWER_RIGHT].x = pruACFPB2[lACFNum][OBJECT].arcoDieCorners[PR_LOWER_RIGHT].x;
		m_PRCurrPBPR2Corner[PR_LOWER_RIGHT].y = pruACFPB2[lACFNum][OBJECT].arcoDieCorners[PR_LOWER_RIGHT].y;
	}

	m_nPRInProgress = ST_PR_IDLE;

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("ACF Post-PR2 Process Time: %.5f", dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}

BOOL CACFWH::AutoSearchPB_MPPR1()
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");

	CalcPB_MP();

	if (
		//MoveX
		(m_nLastError = OpticMoveXY(ACF_WH_XY_CTRL_GO_PBPR_MP1_POSN, GMP_NOWAIT)) != GMP_SUCCESS || 
		//(m_nLastError = MoveZ(ACF_WH_Z_CTRL_GO_PR_LEVEL, GMP_NOWAIT)) != GMP_SUCCESS || 
		(m_nLastError = pCInspOpt->SyncX()) != GMP_SUCCESS || 
		(m_nLastError = SyncY()) != GMP_SUCCESS //|| 
		//(m_nLastError = SyncZ()) != GMP_SUCCESS 
	)
	{
		return FALSE;
	}

	Sleep(m_lPRDelay);

	if (!m_bPRSelected)
	{
		return TRUE;
	}

	if (m_ulPBMode == 0)
	{
		if (!SearchPR(&pruACFPB_MP1[m_lCurACF][OBJECT]))
		{
			return FALSE;
		}
	}
	else
	{
		if (!CheckPRPBStat(&pruACFPB_MP1[m_lCurACF][OBJECT]))
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CACFWH::AutoSearchPB_MPPR1GrabOnly(LONG lACFNum)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();

	m_nPRInProgress = ST_PB_PR1_IN_PROGRESS;

	if (!m_bPRSelected)
	{
		return TRUE;
	}

	if (m_ulPBMode == 0)
	{
		if (!SearchPRGrabOnly(&pruACFPB_MP1[lACFNum][OBJECT]))
		{
			m_nPRInProgress = ST_PR_IDLE;
			return FALSE;
		}
	}
	else
	{
		if (!CheckPRPBStat(&pruACFPB_MP1[lACFNum][OBJECT]))
		{
			m_nPRInProgress = ST_PR_IDLE;
			return FALSE;
		}
	}

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("ACF Post-PR1 Grab Time: %.5f", dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}

BOOL CACFWH::AutoSearchPB_MPPR1ProcessOnly(LONG lACFNum)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();

	if (!m_bPRSelected)
	{
		m_nPRInProgress = ST_PR_IDLE;
		return TRUE;
	}

	if (lACFNum <= NO_ACF || lACFNum >= MAX_NUM_OF_ACF)
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage(__FUNCTION__);
		return FALSE;
	}

	if (m_ulPBMode == 0)
	{
		if (!SearchPRProcessOnly(&pruACFPB_MP1[lACFNum][OBJECT]))
		{
			m_nPRInProgress = ST_PR_IDLE;
			return FALSE;
		}		
	}

	m_nPRInProgress = ST_PR_IDLE;

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("ACF Post-PR1 Process Time: %.5f", dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}

BOOL CACFWH::AutoSearchPB_MPPR2()
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");

	CalcPB_MP();

	if (
		//MoveX
		(m_nLastError = OpticMoveXY(ACF_WH_XY_CTRL_GO_PBPR_MP2_POSN, GMP_NOWAIT)) != GMP_SUCCESS || 
		//(m_nLastError = MoveZ(ACF_WH_Z_CTRL_GO_PR_LEVEL, GMP_NOWAIT)) != GMP_SUCCESS || 
		(m_nLastError = pCInspOpt->SyncX()) != GMP_SUCCESS || 
		(m_nLastError = SyncY()) != GMP_SUCCESS //|| 
		//(m_nLastError = SyncZ()) != GMP_SUCCESS 
	)
	{
		return FALSE;
	}

	Sleep(m_lPRDelay);

	if (!m_bPRSelected)
	{
		return TRUE;
	}

	if (m_ulPBMode == 0)
	{
		if (!SearchPR(&pruACFPB_MP2[m_lCurACF][OBJECT]))
		{
			return FALSE;
		}
	}
	else
	{
		if (!CheckPRPBStat(&pruACFPB_MP2[m_lCurACF][OBJECT]))
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CACFWH::AutoSearchPB_MPPR2GrabOnly(LONG lACFNum)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();

	m_nPRInProgress = ST_PB_PR2_IN_PROGRESS;

	if (!m_bPRSelected)
	{
		return TRUE;
	}

	if (m_ulPBMode == ST_PR_IDLE)
	{
		if (!SearchPRGrabOnly(&pruACFPB_MP2[lACFNum][OBJECT]))
		{
			m_nPRInProgress = ST_PR_IDLE;
			return FALSE;
		}
	}
	else
	{
		if (!CheckPRPBStat(&pruACFPB_MP2[lACFNum][OBJECT]))
		{
			m_nPRInProgress = ST_PR_IDLE;
			return FALSE;
		}
	}

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("ACF Post-PR2 Grab Time: %.5f", dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}

BOOL CACFWH::AutoSearchPB_MPPR2ProcessOnly(LONG lACFNum)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();

	if (!m_bPRSelected)
	{
		m_nPRInProgress = ST_PR_IDLE;
		return TRUE;
	}

	if (lACFNum <= NO_ACF || lACFNum >= MAX_NUM_OF_ACF)
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage(__FUNCTION__);
		return FALSE;
	}

	if (m_ulPBMode == 0)
	{
		if (!SearchPRProcessOnly(&pruACFPB_MP2[lACFNum][OBJECT]))
		{
			m_nPRInProgress = ST_PR_IDLE;
			return FALSE;
		}
	}

	m_nPRInProgress = ST_PR_IDLE;

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("ACF Post-PR2 Process Time: %.5f", dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}


//BOOL CACFWH::SearchPR(PRU *pPRU)
//{
//	BOOL bResult = FALSE;
//
//	Sleep(m_lPRDelay);
//
//	if (!m_bPRSelected)
//	{
//		return TRUE;
//	}
//
//	if (!pPRU->bLoaded)
//	{
//		pPRU->rcoDieCentre.x	= PR_DEF_CENTRE_X;
//		pPRU->rcoDieCentre.y	= PR_DEF_CENTRE_Y;
//		pPRU->fAng			= 0.0;
//		pPRU->fScore		= 0.0;
//
//		PRS_DisplayText(pPRU->emCameraNo, 1, PRS_MSG_ROW2, "PR Pattern Not Loaded");
//
//		return FALSE;
//	}
//
//	TC_AddJob(ACF_WH_PR_GRAB);
//	TC_AddJob(ACF_WH_PR_PROCESS);
//
//	switch (pPRU->emRecordType)
//	{
//		// Pattern Matching or Edge Matching
//	case PRS_DIE_TYPE:
//		bResult = PRS_SrchDie(pPRU);
//		break;
//
//		// Shape Fitting
//	case PRS_SHAPE_TYPE:
//		bResult = PRS_DetectShape(pPRU);
//		break;
//
//		// Tmpl Fitting
//	case PRS_TMPL_TYPE:
//		bResult = PRS_SrchTmpl(pPRU);
//		break;
//
//	case PRS_ACF_TYPE:
//		bResult = PRS_SrchACFCmd(pPRU);
//		break;
//	}
//
//	TC_DelJob(ACF_WH_PR_GRAB);
//	TC_DelJob(ACF_WH_PR_PROCESS);
//
//	return bResult;
//}
//
//BOOL CACFWH::SearchPRGrabOnly(PRU *pPRU)
//{
//	BOOL bResult = FALSE;
//
//	if (!pPRU->bLoaded)
//	{
//		pPRU->rcoDieCentre.x	= PR_DEF_CENTRE_X;
//		pPRU->rcoDieCentre.y	= PR_DEF_CENTRE_Y;
//		pPRU->fAng			= 0.0;
//		pPRU->fScore		= 0.0;
//
//		PRS_DisplayText(pPRU->emCameraNo, 1, PRS_MSG_ROW2, "PR Pattern Not Loaded");
//
//		return FALSE;
//	}
//
//	TC_AddJob(ACF_WH_PR_GRAB);
//
//	if (HMI_bLogPRAction)
//	{
//		LogFile->log(__FUNCTION__);
//		LogFile->log("PRU = %ld m_lCurACF = %ld", pPRU->emID, m_lCurACF);
//	}
//
//	switch (pPRU->emRecordType)
//	{
//		// Pattern Matching or Edge Matching
//	case PRS_DIE_TYPE:
//		bResult = PRS_SrchDieGrabOnly(pPRU);
//		break;
//
//		// Shape Fitting
//	case PRS_SHAPE_TYPE:
//		bResult = PRS_DetectShapeGrabOnly(pPRU);
//		break;
//
//		// Tmpl Fitting
//	case PRS_TMPL_TYPE:
//		bResult = PRS_SrchTmplGrabOnly(pPRU);
//		break;
//
//	case PRS_ACF_TYPE:
//		bResult = PRS_SrchACFCmdGrabOnly(pPRU);
//		break;
//	}
//
//	TC_DelJob(ACF_WH_PR_GRAB);
//
//	if (HMI_bLogPRAction)
//	{
//		LogFile->log("Result = %ld", bResult);
//	}
//
//	return bResult;
//}
//
//BOOL CACFWH::SearchPRProcessOnly(PRU *pPRU)
//{
//	BOOL bResult = FALSE;
//
//	if (!pPRU->bLoaded)
//	{
//		pPRU->rcoDieCentre.x	= PR_DEF_CENTRE_X;
//		pPRU->rcoDieCentre.y	= PR_DEF_CENTRE_Y;
//		pPRU->fAng			= 0.0;
//		pPRU->fScore		= 0.0;
//
//		PRS_DisplayText(pPRU->emCameraNo, 1, PRS_MSG_ROW2, "PR Pattern Not Loaded");
//
//		return FALSE;
//	}
//
//	TC_AddJob(ACF_WH_PR_PROCESS);
//
//	if (HMI_bLogPRAction)
//	{
//		LogFile->log(__FUNCTION__);
//		LogFile->log("PRU = %ld m_lCurACF = %ld", pPRU->emID, m_lCurACF);
//	}
//
//	switch (pPRU->emRecordType)
//	{
//		// Pattern Matching or Edge Matching
//	case PRS_DIE_TYPE:
//		bResult = PRS_SrchDieProcessOnly(pPRU);
//		break;
//
//		// Shape Fitting
//	case PRS_SHAPE_TYPE:
//		bResult = PRS_DetectShapeProcessOnly(pPRU);
//		break;
//
//		// Tmpl Fitting
//	case PRS_TMPL_TYPE:
//		bResult = PRS_SrchTmplProcessOnly(pPRU);
//		break;
//
//	case PRS_ACF_TYPE:
//		bResult = PRS_SrchACFCmdProcessOnly(pPRU);
//		break;
//	}
//
//	TC_DelJob(ACF_WH_PR_PROCESS);
//
//	if (HMI_bLogPRAction)
//	{
//		LogFile->log("Result = %ld", bResult);
//	}
//
//	return bResult;
//}
//
//BOOL CACFWH::SearchPRCentre(PRU *pPRU)
//{
//	MTR_POSN mtr_offset;
//	BOOL bResult = FALSE;
//
//	if (!m_bPRSelected)
//	{
//		return TRUE;
//	}
//
//	Sleep(m_lPRDelay);
//
//	if (!pPRU->bLoaded)
//	{
//		pPRU->rcoDieCentre.x	= PR_DEF_CENTRE_X;
//		pPRU->rcoDieCentre.y	= PR_DEF_CENTRE_Y;
//		pPRU->fAng			= 0.0;
//		pPRU->fScore		= 0.0;
//
//		PRS_DisplayText(pPRU->emCameraNo, 1, PRS_MSG_ROW2, "PR Pattern Not Loaded");
//
//		return FALSE;
//	}
//
//	TC_AddJob(ACF_WH_PR_GRAB);
//	TC_AddJob(ACF_WH_PR_PROCESS);
//
//	switch (pPRU->emRecordType)
//	{
//		// Pattern Matching or Edge Matching
//	case PRS_DIE_TYPE:
//		bResult = PRS_SrchDie(pPRU);
//		break;
//
//		// Shape Fitting
//	case PRS_SHAPE_TYPE:
//		bResult = PRS_DetectShape(pPRU);
//		break;
//
//		// Tmpl Fitting
//	case PRS_TMPL_TYPE:
//		bResult = PRS_SrchTmpl(pPRU);
//		break;
//
//	case PRS_ACF_TYPE:
//		bResult = PRS_SrchACFCmd(pPRU);
//		break;
//	}
//
//	TC_DelJob(ACF_WH_PR_GRAB);
//	TC_DelJob(ACF_WH_PR_PROCESS);
//
//	// Die found. Align table to centre
//	if (bResult)
//	{
//		PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);
//
//
//		if (OpticMoveRelativeXY(mtr_offset.x, mtr_offset.y, GMP_WAIT) != GMP_SUCCESS)
//		{
//			bResult = FALSE;
//		}
//	}
//
//	return bResult;
//}
//
//BOOL CACFWH::SearchCurrPRPosn(ACFNum lACFNum)
//{
//	if (lACFNum <= NO_ACF || lACFNum >= MAX_NUM_OF_ACF)
//	{
//		return FALSE;
//	}
//	if (!pruACF1.bLoaded || !pruACF2.bLoaded)
//	{
//		return FALSE;
//	}
//
//	if (
//		//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
//		//MoveX to Standby, get ready
//		MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS //||
//		//MoveTToCurEdge(g_stACF[lACFNum].lEdgeInfo) != GMP_SUCCESS
//	)
//	{
//		return FALSE;
//	}
//
//	m_dBondAngle = 0.0;
//	if (
//		!AutoSearchPR1(&pruACF1) ||
//		!AutoSearchPR2(&pruACF2)
//	)
//	{
//		//HMIMessageBox(MSG_OK, "WARNING", "Search PR Failed!");		// Fail to enter PR Setup Screen
//		return FALSE;
//	}
//
//	CalcBondPt(lACFNum);
//
//	return TRUE;
//}

BOOL CACFWH::CheckPRPBStat(PRU *pPRU)
{
	BOOL bResult;
	DOUBLE dThreshold = 0.0;

	Sleep(m_lPRDelay);

	if (!pPRU->bStatLoaded)
	{
		pPRU->fStatMean		= 0.0;
		pPRU->fStatStdev	= 0.0;

		PRS_DisplayText(pPRU->emCameraNo, 1, PRS_MSG_ROW2, "PR PB Stat Not Loaded");

		return FALSE;
	}

	PR_WORD prCoaxialLevel	= PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT);
	PR_WORD prRingLevel		= PRS_GetLighting(pPRU, PR_RING_LIGHT);
	PR_WORD prCoaxial1Level	= PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT_1);
	PR_WORD prSideLevel		= PRS_GetLighting(pPRU, PR_RING_LIGHT_1);

	if (pPRU->emID == ID_INSP1_ACFPB1_TYPE1)
	{
		PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT, (PR_WORD)m_PBStat1.lCoaxialLevel);
		PRS_SetLighting(pPRU, PR_RING_LIGHT, (PR_WORD)m_PBStat1.lRingLevel);
		PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT_1, (PR_WORD)m_PBStat1.lCoaxial1Level);
		PRS_SetLighting(pPRU, PR_RING_LIGHT_1, (PR_WORD)m_PBStat1.lSideLevel);

		if (m_lPRPBStatMode == 0)
		{
			dThreshold = m_PBStat1.dSDThreshold;
		}
		else
		{
			dThreshold = m_PBStat1.dMeanThreshold;
		}
	}
	else if (pPRU->emID == ID_INSP1_ACFPB2_TYPE1)
	{
		PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT, (PR_WORD)m_PBStat2.lCoaxialLevel);
		PRS_SetLighting(pPRU, PR_RING_LIGHT, (PR_WORD)m_PBStat2.lRingLevel);
		PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT_1, (PR_WORD)m_PBStat2.lCoaxial1Level);
		PRS_SetLighting(pPRU, PR_RING_LIGHT_1, (PR_WORD)m_PBStat2.lSideLevel);

		if (m_lPRPBStatMode == 0)
		{
			dThreshold = m_PBStat2.dSDThreshold;
		}
		else
		{
			dThreshold = m_PBStat2.dMeanThreshold;
		}
	}

	bResult = PRS_GetImageStatInfo(pPRU);

	PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT, prCoaxialLevel);
	PRS_SetLighting(pPRU, PR_RING_LIGHT, prRingLevel);
	PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT_1, prCoaxial1Level);
	PRS_SetLighting(pPRU, PR_RING_LIGHT_1, prSideLevel);

	if (bResult)
	{
		if (m_lPRPBStatMode == 0)		// SD
		{
			if (m_lPRPBStatRegion == 0)		// >=
			{
				if (pPRU->fStatStdev >= dThreshold)
				{
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
			else		// <
			{
				if (pPRU->fStatStdev < dThreshold)
				{
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
		}
		else	// Mean
		{
			if (m_lPRPBStatRegion == 0)		// >=
			{
				if (pPRU->fStatMean >= dThreshold)
				{
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
			else		// <
			{
				if (pPRU->fStatMean < dThreshold)
				{
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

VOID CACFWH::CalcPB_MP()
{
	D_MTR_POSN	dmtrPBPosnOffset	= {0, 0};

	if (m_lNumOfPB_MP != 0)
	{
		dmtrPBPosnOffset.x = (m_dmtrCurrPBPR2Posn.x - m_dmtrCurrPBPR1Posn.x) / (DOUBLE)(m_lNumOfPB_MP + 1);
		dmtrPBPosnOffset.y = (m_dmtrCurrPBPR2Posn.y - m_dmtrCurrPBPR1Posn.y) / (DOUBLE)(m_lNumOfPB_MP + 1);
	}

	m_dmtrCurrPBPR_MP1Posn.x = m_dmtrCurrPBPR1Posn.x + dmtrPBPosnOffset.x;
	m_dmtrCurrPBPR_MP1Posn.y = m_dmtrCurrPBPR1Posn.y + dmtrPBPosnOffset.y;

	if (m_lNumOfPB_MP == 2)
	{
		m_dmtrCurrPBPR_MP2Posn.x = m_dmtrCurrPBPR1Posn.x + (2 * dmtrPBPosnOffset.x);		
		m_dmtrCurrPBPR_MP2Posn.y = m_dmtrCurrPBPR1Posn.y + (2 * dmtrPBPosnOffset.y);
	}
	else
	{
		m_dmtrCurrPBPR_MP2Posn.x = m_dmtrCurrPBPR1Posn.x + dmtrPBPosnOffset.x;
		m_dmtrCurrPBPR_MP2Posn.y = m_dmtrCurrPBPR1Posn.y + dmtrPBPosnOffset.y;
	}
}

VOID CACFWH::CalcACFAccuracy(D_MTR_POSN dmtrPBPR1Posn, D_MTR_POSN dmtrPBPR2Posn, BOOL bDisplayLength)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	MTR_POSN	mtrOffset;
	MTR_POSN	mtrCurrLowerLeftCorner = {0, 0};
	MTR_POSN	mtrCurrLowerRightCorner = {0, 0};
	PRU		*pPRU1;
	PRU		*pPRU2;
	DOUBLE		dx, dy;

	// Debug
	CString szMsg = "";

	pPRU1 = &pruACFPB1[m_lCurACF][OBJECT];
	pPRU2 = &pruACFPB2[m_lCurACF][OBJECT];

	if (pPRU1->stLrnACFCmd.emACFPosition == PR_OBJ_POSITION_LEFT)
	{
		PRS_PRPosToMtrOffset(pPRU1, m_PRCurrPBPR1Corner[PR_LOWER_LEFT], &mtrOffset);
		mtrCurrLowerLeftCorner.x = (LONG)(dmtrPBPR1Posn.x + mtrOffset.x);
		mtrCurrLowerLeftCorner.y = (LONG)(dmtrPBPR1Posn.y + mtrOffset.y);
	}
	else
	{
		PRS_PRPosToMtrOffset(pPRU1, m_PRCurrPBPR1Corner[PR_LOWER_RIGHT], &mtrOffset);
		mtrCurrLowerRightCorner.x = (LONG)(dmtrPBPR1Posn.x + mtrOffset.x);
		mtrCurrLowerRightCorner.y = (LONG)(dmtrPBPR1Posn.y + mtrOffset.y);
	}


	if (pPRU2->stLrnACFCmd.emACFPosition == PR_OBJ_POSITION_LEFT)
	{
		PRS_PRPosToMtrOffset(pPRU2, m_PRCurrPBPR2Corner[PR_LOWER_LEFT], &mtrOffset);
		mtrCurrLowerLeftCorner.x = (LONG)(dmtrPBPR2Posn.x + mtrOffset.x);
		mtrCurrLowerLeftCorner.y = (LONG)(dmtrPBPR2Posn.y + mtrOffset.y);
	}
	else
	{
		PRS_PRPosToMtrOffset(pPRU2, m_PRCurrPBPR2Corner[PR_LOWER_RIGHT], &mtrOffset);
		mtrCurrLowerRightCorner.x = (LONG)(dmtrPBPR2Posn.x + mtrOffset.x);
		mtrCurrLowerRightCorner.y = (LONG)(dmtrPBPR2Posn.y + mtrOffset.y);
	}

	dx = (mtrCurrLowerRightCorner.x - mtrCurrLowerLeftCorner.x);
	dy = (mtrCurrLowerRightCorner.y - mtrCurrLowerLeftCorner.y);

	m_dCurrACFLength = sqrt(pow(dx, 2.0) + pow(dy, 2.0)) * pCInspOpt->m_stMotorX.stAttrib.dDistPerCount / 1000;

	// Debug
	if (bDisplayLength)
	{
		// open debugging file
		FILE *fp = fopen("D:\\sw\\COG930\\Data\\ACF_PostBondPt.log", "a+");
		if (fp != NULL)
		{
			// PR1 PR2 Enc1 Enc2 APt1 APt2 Dist Ang
			//fprintf(fp, "PR1\t\tPR2\t\tAlignCentre\t\tACFLowerLeftCorner\tACFLowerRightCorner\t\tACF Length(mm)");
			fprintf(fp, "\n%.3f, %.3f; %.3f, %.3f; %.3f, %.3f; %ld, %ld; %ld, %ld; %.5f", 
					m_dmtrCurrPR1Posn.x, m_dmtrCurrPR1Posn.y, 
					m_dmtrCurrPR2Posn.x, m_dmtrCurrPR2Posn.y,
					m_dmtrCurrAlignCentre.x, m_dmtrCurrAlignCentre.y,
					mtrCurrLowerLeftCorner.x, mtrCurrLowerLeftCorner.y,
					mtrCurrLowerRightCorner.x, mtrCurrLowerRightCorner.y,
					m_dCurrACFLength);		

			fclose(fp);
		}

		//szMsg.Format("m_dCurrACFLength = %.2f", m_dCurrACFLength);
		//DisplayMessage(szMsg);
	}
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
INT CACFWH::KeepQuiet()
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
	return nResult;
}

VOID CACFWH::PrintMachineSetup(FILE *fp)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString str = "";

	if (fp != NULL)
	{
		fprintf(fp, "\n<<<<%s>>>>\n", (const char*)GetStnName());
		fprintf(fp, "======================================================\n");
		fprintf(fp, "<<Posn>>\n");
		fprintf(fp, "%30s\t : %ld, %ld\n", "Standby Posn", 
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrStandbyPosn.x"], 
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrStandbyPosn.y"]);
		fprintf(fp, "%30s\t : %ld, %ld\n", "Load Glass Posn",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrLoadGlassPosn.x"],
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrLoadGlassPosn.y"]);
		fprintf(fp, "%30s\t : %ld, %ld\n", "Unload Glass Posn",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrUnloadGlassPosn.x"],
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrUnloadGlassPosn.y"]);
		fprintf(fp, "%30s\t : %ld, %ld\n", "Base Posn",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrBaseHeaterPosn.x"],
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrBaseHeaterPosn.y"]);
		fprintf(fp, "%30s\t : %ld, %ld\n", "Approx Bond Posn",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrApproxBondPosn.x"],
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrApproxBondPosn.y"]);


		//fprintf(fp, "<<Level>>\n");
		//for (INT i = WHZ_1; i < MAX_NUM_OF_WHZ; i++)
		//{
		//	str.Format("WHZ_%ld", i + 1);

		//	fprintf(fp, "WHZ_%d\n", i + 1);
		//	fprintf(fp, "%30s\t : %ld\n", "Standby Level",
		//			(LONG)pAppMod->m_smfMachine[GetStnName()]["Level"][str]["m_lStandbyLevel"]);
		//	fprintf(fp, "%30s\t : %ld\n", "Calib. Stage Level",
		//			(LONG)pAppMod->m_smfMachine[GetStnName()]["Level"][str]["m_lCalibStageLevel"]);
		//	fprintf(fp, "%30s\t : %ld\n", "Calib. Load Level",
		//			(LONG)pAppMod->m_smfMachine[GetStnName()]["Level"][str]["m_lCalibLoadLevel"]);
		//	fprintf(fp, "%30s\t : %ld\n", "Calib. Unload Level",
		//			(LONG)pAppMod->m_smfMachine[GetStnName()]["Level"][str]["m_lCalibUnloadLevel"]);
		//	fprintf(fp, "%30s\t : %ld\n", "Approx Bond Level",
		//			(LONG)pAppMod->m_smfMachine[GetStnName()]["Level"][str]["m_lApproxBondLevel"]);

		//}
	}

	CAC9000Stn::PrintMachineSetup(fp);
}

VOID CACFWH::PrintDeviceSetup(FILE *fp)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString str, str1, str2;

	if (fp != NULL)
	{
		fprintf(fp, "\n<<<<%s>>>>\n", (const char*)GetStnName());
		fprintf(fp, "======================================================\n");
		fprintf(fp, "<<Posn>>\n");
		fprintf(fp, "%30s\t : %ld, %ld\n", "Unload Glass Offset",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrUnloadGlassOffset.x"],
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrUnloadGlassOffset.y"]);


//		fprintf(fp, "<<Level>>\n");
//		for (INT i = WHZ_1; i < MAX_NUM_OF_WHZ; i++)
//		{
//			str.Format("WHZ_%ld", i + 1);
//
//			fprintf(fp, "WHZ_%d\n", i + 1);
//			fprintf(fp, "%30s\t : %ld\n", "Load Level Offset",
//					(LONG)pAppMod->m_smfDevice[GetStnName()]["Level"][str]["m_lLoadLevelOffset"]);
//			fprintf(fp, "%30s\t : %ld\n", "Unload Level Offset",
//					(LONG)pAppMod->m_smfDevice[GetStnName()]["Level"][str]["m_lUnloadLevelOffset"]);
//			for (INT j = ACF1; j < MAX_NUM_OF_ACF; j++)
//			{
//				if (m_stACF[j].bEnable)
//				{
//					str1.Format("ACF %ld Bond Level Offset", j + 1);
//					str2.Format("ACF%ld", j + 1);
//#if 1 //klocwork fix 20121211
//					fprintf(fp, "%30s\t : %ld\n", (LPCTSTR)str1,
//							(LONG)pAppMod->m_smfDevice[GetStnName()]["Level"][str]["m_lBondLevelOffset"][str2]);
//#else
//					fprintf(fp, "%30s\t : %ld\n", str1,
//							(LONG)pAppMod->m_smfDevice[GetStnName()]["Level"][str]["m_lBondLevelOffset"][str2]);
//#endif
//				}
//			}
//		}	

		fprintf(fp, "<<Para>>\n");
		fprintf(fp, "%30s\t : %ld\n", "Vac On Delay",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Delays"][m_stGlass1VacSol.GetName()]["lOnDelay"]);
		fprintf(fp, "%30s\t : %ld\n", "Vac Off Delay",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Delays"][m_stGlass1VacSol.GetName()]["lOffDelay"]);
		fprintf(fp, "%30s\t : %ld\n", "WeakBlow On Delay",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Delays"][m_stWeakBlowSol.GetName()]["lOnDelay"]);
		fprintf(fp, "%30s\t : %ld\n", "WeakBlow Off Delay",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Delays"][m_stWeakBlowSol.GetName()]["lOffDelay"]);

		for (INT i = ACF1; i < MAX_NUM_OF_ACF; i++)
		{
			if (m_stACF[i].bEnable)
			{
				str.Format("ACF%ld", i + 1);
				fprintf(fp, "%30s\t : %ld\n", "Bond Offset Y",
						(LONG)pAppMod->m_smfDevice[GetStnName()]["Para"]["m_lBondOffsetY"][str]);
				fprintf(fp, "%30s\t : %.4f\n", "Bond Offset Adj Y",
						(DOUBLE)pAppMod->m_smfDevice[GetStnName()]["Para"]["m_dBondOffsetAdjY"][str]); //20120925
			}
		}
	}
	CAC9000Stn::PrintDeviceSetup(fp);
}

#ifdef EXCEL_MACHINE_DEVICE_INFO
BOOL CACFWH::printMachineDeviceInfo()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bResult = TRUE;
	CString szBlank("--");
	CString szModule = GetStnName();
	LONG lValue = 0;
	DOUBLE dValue = 0.0;

	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrBaseHeaterPosn.x"];
		bResult = printMachineInfoRow(szModule, CString("Posn"), CString("mtrBaseHeaterPosn.x"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrBaseHeaterPosn.y"];
		bResult = printMachineInfoRow(szModule, CString("Posn"), CString("mtrBaseHeaterPosn.y"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}

	if (bResult)
	{
		bResult = CWorkHolder::printMachineDeviceInfo();
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
LONG CACFWH::HMI_SelectACFNum(IPC_CServiceMessage &svMsg)
{
	ULONG ulACFNum;
	svMsg.GetMsg(sizeof(ULONG), &ulACFNum);

	INT nResult = GMP_SUCCESS;
	CString szMsg = "";
	
	if (!m_stACF[ulACFNum].bEnable)
	{
		HMIMessageBox(MSG_OK, "Select ACF", "Please Add ACF Unit, and retry again.");
		return  0;
	}

	m_lCurACF	= ulACFNum;
	
	UpdateModuleSetupPosn();
	UpdateModuleSetupLevel();
	UpdateModuleSetupPara();

	LogAction(__FUNCTION__);
	LogAction("m_lCurSlave = %ld\t", m_lCurSlave);
	LogAction("m_lCurACF = %ld\t", m_lCurACF);

	return FALSE;
}

LONG CACFWH::HMI_SetBondOffsetAdjY(IPC_CServiceMessage &svMsg) //20120925
{
	DOUBLE dValue;
	svMsg.GetMsg(sizeof(DOUBLE), &dValue);

	m_dBondOffsetAdjY[ACF1] = dValue; 
	HMI_dBondOffsetAdjY = m_dBondOffsetAdjY[ACF1];

	//for (INT i = ACF2; i < MAX_NUM_OF_ACF; i++)
	//{
	//	m_dBondOffsetAdjY[i] = m_dBondOffsetAdjY[ACF1];
	//}

	return 0;
}

LONG CACFWH::HMI_SetBondOffsetY(IPC_CServiceMessage &svMsg)
{
	LONG lOffset;
	svMsg.GetMsg(sizeof(LONG), &lOffset);

	m_lBondOffsetY[ACF1] = lOffset;
	HMI_lBondOffsetY = m_lBondOffsetY[ACF1];

	return 0;
}

LONG CACFWH::HMI_SetAdjXLoadOffset(IPC_CServiceMessage &svMsg) //20121005
{
	DOUBLE dValue;
	svMsg.GetMsg(sizeof(DOUBLE), &dValue);

	m_dLoadAdjXOffset = dValue;
	return 0;
}

LONG CACFWH::HMI_ResetLoadOffset(IPC_CServiceMessage &svMsg) //20140507
{
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");

	CString		szpassword = pCHouseKeeping->szEngPassword;

	if (HMIGetPassword("Please enter password", szpassword))
	{
		LogAction(__FUNCTION__);
		m_lLoadXOffset = 0;
		m_lLoadYOffset = 0;
	}
	return 0;
}

LONG CACFWH::HMI_SetXLoadOffset(IPC_CServiceMessage &svMsg)
{
	LONG lDelay;
	svMsg.GetMsg(sizeof(LONG), &lDelay);

	//LONG lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Modifying the Offset would affect the glass position on ACFWH, PreBondWH and MainBond1/2 WH. Continue?");

	//if (lAnswer == rMSG_CONTINUE)
	//{
	m_lLoadXOffset = lDelay;
	//}

	return 1;
}

LONG CACFWH::HMI_SetYLoadOffset(IPC_CServiceMessage &svMsg)
{
	LONG lDelay;
	svMsg.GetMsg(sizeof(LONG), &lDelay);

	//LONG lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Modifying the Offset would affect the glass position on ACFWH, PreBondWH and MainBond1/2 WH. Continue?");

	//if (lAnswer == rMSG_CONTINUE)
	{
		m_lLoadYOffset = lDelay;
	}

	return 1;
}

LONG CACFWH::HMI_SetYUnloadOffset(IPC_CServiceMessage &svMsg)
{
	LONG lDelay;
	svMsg.GetMsg(sizeof(LONG), &lDelay);

	//LONG lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Modifying the Unload Offset would only be effective on glass at position under Inspection Cameras. Continue?");

	//if (lAnswer == rMSG_CONTINUE)
	{
		m_lUnloadYOffset = lDelay;
	}

	return 1;
}

LONG CACFWH::HMI_ACFOffsetTest(IPC_CServiceMessage &svMsg)
{
	BOOL		bStatus = TRUE;
	LONG		lStep = 0;
	CString		szMsg = "";

	if (!m_pCACF->m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", m_pCACF->GetStnName());
		HMIMessageBox(MSG_OK, "ACF OFFSET TEST", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bStatus);
		return 1;
	}

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "ACF OFFSET TEST", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bStatus);
		return 1;
	}

	if (
		((m_nLastError = SetWeakBlowSol(OFF)) != GMP_SUCCESS) ||
		((m_nLastError = m_pCACF->SetHalfCutterOnSol(OFF, SFM_WAIT)) != GMP_SUCCESS) ||
		((m_nLastError = m_pCACF->MoveStripperX(ACF_STRIPPER_X_CTRL_GO_STANDBY_POSN, GMP_WAIT)) != GMP_SUCCESS) ||
		// 20140616 Yip: Not Vacuum On During Pre Start
//			(AutoMode != BURN_IN && ((m_nLastError = SetGLASS1_VacSol(ON)) != GMP_SUCCESS)) || //20120309
		((m_nLastError = m_pCACF->MoveZ(ACF_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) != GMP_SUCCESS) ||
		((m_nLastError = MoveT(ACF_WH_T_CTRL_GO_STANDBY, GMP_NOWAIT)) != GMP_SUCCESS) ||
		((m_nLastError = MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT)) != GMP_SUCCESS) ||
		((m_nLastError = m_stMotorT.Sync()) != GMP_SUCCESS) 
		)
	{
		szMsg.Format("Move to standby fail. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "ACF OFFSET TEST", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bStatus);
		return 1;
	}

	ACFOffsetTestOperation();

	svMsg.InitMessage(sizeof(BOOL), &bStatus);
	return 1;
}

LONG CACFWH::IPC_SaveMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	
	if (m_lCurSlave == ACF_NONE)
	{
		return 1;
	}

	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrBaseHeaterPosn.x"]			= m_mtrBaseHeaterPosn.x;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrBaseHeaterPosn.y"]			= m_mtrBaseHeaterPosn.y;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrPRCalibPosn.x"]				= m_mtrPRCalibPosn.x;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrPRCalibPosn.y"]				= m_mtrPRCalibPosn.y;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrSafetyPosn.x"]				= m_mtrSafetyPosn.x;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrSafetyPosn.y"]				= m_mtrSafetyPosn.y;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lStandbyPosnT"]					= m_lStandbyPosnT[EDGE_A];

	pAppMod->m_smfMachine[GetStnName()]["Para"]["g_bEnableAnvilVac"]				= g_bEnableAnvilVac;

	pAppMod->m_smfMachine[GetStnName()]["ACFOffset"]["m_dBondOffsetT"]						= m_dBondOffsetT;
	pAppMod->m_smfMachine[GetStnName()]["ACFOffset"]["m_dACFRefWidth"]						= m_dACFRefWidth;
	pAppMod->m_smfMachine[GetStnName()]["ACFOffset"]["m_dACFRefLength"]						= m_dACFRefLength;
	pAppMod->m_smfMachine[GetStnName()]["ACFOffset"]["m_mtrACFLLPoint.x"]					= m_mtrACFLLPoint.x;
	pAppMod->m_smfMachine[GetStnName()]["ACFOffset"]["m_mtrACFLLPoint.y"]					= m_mtrACFLLPoint.y;
	pAppMod->m_smfMachine[GetStnName()]["ACFOffset"]["m_mtrACFLRPoint.x"]					= m_mtrACFLRPoint.x;
	pAppMod->m_smfMachine[GetStnName()]["ACFOffset"]["m_mtrACFLRPoint.y"]					= m_mtrACFLRPoint.y;
	
	pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_mtrACFAnvilLLPosn.x"]				= m_mtrACFAnvilLLPosn.x;	//	ACFUL Setup
	pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_mtrACFAnvilLLPosn.y"]				= m_mtrACFAnvilLLPosn.y;	//	ACFUL Setup
	pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_mtrACFAnvilLRPosn.x"]				= m_mtrACFAnvilLRPosn.x;	//	ACFUL Setup
	pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_mtrACFAnvilLRPosn.y"]				= m_mtrACFAnvilLRPosn.y;	//	ACFUL Setup
	pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_dAlignRefAngle"]					= m_dAlignRefAngle;

	pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_mtrAlignRefPosn.x"]				= m_mtrAlignRefPosn.x;
	pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_mtrAlignRefPosn.y"]				= m_mtrAlignRefPosn.y;
	pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_mtrAlignRefPosnOnWH.x"]			= m_mtrAlignRefPosnOnWH.x;
	pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_mtrAlignRefPosnOnWH.y"]			= m_mtrAlignRefPosnOnWH.y;
	pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_dAnvilBlockAngOffset"]				= m_dAnvilBlockAngOffset;
	pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_bACFPlacePosnSetup"]				= m_bACFPlacePosnSetup;
	return CWorkHolder::IPC_SaveMachineParam();
}

LONG CACFWH::IPC_LoadMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	
	if (m_lCurSlave == ACF_NONE)
	{
		return 1;
	}

	m_mtrBaseHeaterPosn.x		= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrBaseHeaterPosn.x"];
	m_mtrBaseHeaterPosn.y		= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrBaseHeaterPosn.y"];
	m_mtrPRCalibPosn.x			= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrPRCalibPosn.x"];
	m_mtrPRCalibPosn.y			= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrPRCalibPosn.y"];
	m_mtrSafetyPosn.x			= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrSafetyPosn.x"];
	m_mtrSafetyPosn.y			= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_mtrSafetyPosn.y"];
	m_lStandbyPosnT[EDGE_A]		= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lStandbyPosnT"];

	m_dBondOffsetT						= pAppMod->m_smfMachine[GetStnName()]["ACFOffset"]["m_dBondOffsetT"];
	m_dACFRefWidth						= pAppMod->m_smfMachine[GetStnName()]["ACFOffset"]["m_dACFRefWidth"];
	m_dACFRefLength						= pAppMod->m_smfMachine[GetStnName()]["ACFOffset"]["m_dACFRefLength"];
	m_mtrACFLLPoint.x					= (LONG)pAppMod->m_smfMachine[GetStnName()]["ACFOffset"]["m_mtrACFLLPoint.x"];
	m_mtrACFLLPoint.y					= (LONG)pAppMod->m_smfMachine[GetStnName()]["ACFOffset"]["m_mtrACFLLPoint.y"];
	m_mtrACFLRPoint.x					= (LONG)pAppMod->m_smfMachine[GetStnName()]["ACFOffset"]["m_mtrACFLRPoint.x"];
	m_mtrACFLRPoint.y					= (LONG)pAppMod->m_smfMachine[GetStnName()]["ACFOffset"]["m_mtrACFLRPoint.y"];

	g_bEnableAnvilVac = (BOOL)(LONG)pAppMod->m_smfMachine[GetStnName()]["Para"]["g_bEnableAnvilVac"];

	m_mtrACFAnvilLLPosn.x				= pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_mtrACFAnvilLLPosn.x"];
	m_mtrACFAnvilLLPosn.y				= pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_mtrACFAnvilLLPosn.y"];
	m_mtrACFAnvilLRPosn.x				= pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_mtrACFAnvilLRPosn.x"];
	m_mtrACFAnvilLRPosn.y				= pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_mtrACFAnvilLRPosn.y"];
	m_dAlignRefAngle						= pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_dAlignRefAngle"];

	m_mtrAlignRefPosn.x					= pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_mtrAlignRefPosn.x"];
	m_mtrAlignRefPosn.y					= pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_mtrAlignRefPosn.y"];
	m_mtrAlignRefPosnOnWH.x				= pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_mtrAlignRefPosnOnWH.x"];
	m_mtrAlignRefPosnOnWH.y				= pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_mtrAlignRefPosnOnWH.y"];
	m_dAnvilBlockAngOffset				= pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_dAnvilBlockAngOffset"];
	m_bACFPlacePosnSetup				= (BOOL)(LONG)pAppMod->m_smfMachine[GetStnName()]["AlignParam"]["m_bACFPlacePosnSetup"];
	return CWorkHolder::IPC_LoadMachineParam();
}

LONG CACFWH::IPC_SaveDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString str, str1;
	
	if (m_lCurSlave == ACF_NONE)
	{
		return 1;
	}

	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lLoadTOffset"]					= m_lLoadTOffset;
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lLoadXOffset"]					= m_lLoadXOffset;
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lLoadYOffset"]					= m_lLoadYOffset;
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_dLoadAdjXOffset"]					= m_dLoadAdjXOffset; //20121005
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrLoadGlassOffset.y"]			= m_mtrLoadGlassOffset.y; //20121005

	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lUnloadYOffset"]					= m_lUnloadYOffset;

	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrPR1Posn.x"]				= m_mtrPR1Posn[ACF1].x;
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrPR1Posn.y"]				= m_mtrPR1Posn[ACF1].y;
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrPR2Posn.x"]				= m_mtrPR2Posn[ACF1].x;
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrPR2Posn.y"]				= m_mtrPR2Posn[ACF1].y;

	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrPBPR1Posn.x"]				= m_mtrPBPR1Posn.x;
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrPBPR1Posn.y"]				= m_mtrPBPR1Posn.y;
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrPBPR2Posn.x"]				= m_mtrPBPR2Posn.x;
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrPBPR2Posn.y"]				= m_mtrPBPR2Posn.y;

	pAppMod->m_smfDevice[GetStnName()]["ACFOffset"]["m_mtrACFCenterPoint.x"]				= m_mtrACFCenterPoint.x;
	pAppMod->m_smfDevice[GetStnName()]["ACFOffset"]["m_mtrACFCenterPoint.y"]				= m_mtrACFCenterPoint.y;
	pAppMod->m_smfDevice[GetStnName()]["AlignParam"]["m_dGlassRefAng"]			= m_dGlassRefAng;
	pAppMod->m_smfDevice[GetStnName()]["AlignParam"]["m_dmtrAlignRef.x"]		= m_dmtrAlignRef.x;
	pAppMod->m_smfDevice[GetStnName()]["AlignParam"]["m_dmtrAlignRef.y"]		= m_dmtrAlignRef.y;
	pAppMod->m_smfDevice[GetStnName()]["AlignParam"]["m_dAlignRefAngle"]		= m_dAlignRefAngle;
	pAppMod->m_smfDevice[GetStnName()]["AlignParam"]["m_dAlignRefDist"]			= m_dAlignRefDist;

	// saving parameters	
	for (INT nCurIndex = ACF1; nCurIndex < MAX_NUM_OF_ACF; nCurIndex++)
	{
		str.Format("ACF%ld", nCurIndex + 1);
		pAppMod->m_smfDevice[GetStnName()]["Para"]["m_lBondOffsetY"][str]			= m_lBondOffsetY[nCurIndex];
		pAppMod->m_smfDevice[GetStnName()]["Para"]["m_dBondOffsetAdjY"][str]		= m_dBondOffsetAdjY[nCurIndex]; //20120925
	}

	//for (INT i = WHZ_1; i < MAX_NUM_OF_WHZ; i++)
	//{
	//	str.Format("WHZ_%ld", i + 1);
	//	for (INT j = ACF1; j < MAX_NUM_OF_ACF; j++)
	//	{
	//		str1.Format("ACF%ld", j + 1);
	//		pAppMod->m_smfDevice[GetStnName()]["Level"][str]["m_lBondLevelOffset"][str1]	= m_lBondLevelOffset[i][j];
	//	}
	//}

	return CWorkHolder::IPC_SaveDeviceParam();
}

LONG CACFWH::IPC_LoadDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString str, str1;
	
	if (m_lCurSlave == ACF_NONE)
	{
		return 1;
	}

	m_lLoadTOffset				= pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lLoadTOffset"];
	m_lLoadXOffset				= pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lLoadXOffset"];
	m_lLoadYOffset				= pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lLoadYOffset"];
	m_dLoadAdjXOffset			= pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_dLoadAdjXOffset"]; //20121005
	m_mtrLoadGlassOffset.y		= pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrLoadGlassOffset.y"]; 

	m_lUnloadYOffset			= pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lUnloadYOffset"];

	m_mtrPR1Posn[ACF1].x		= pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrPR1Posn.x"];
	m_mtrPR1Posn[ACF1].y		= pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrPR1Posn.y"];
	m_mtrPR2Posn[ACF1].x		= pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrPR2Posn.x"];
	m_mtrPR2Posn[ACF1].y		= pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrPR2Posn.y"];

	m_mtrPBPR1Posn.x		= pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrPBPR1Posn.x"];
	m_mtrPBPR1Posn.y		= pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrPBPR1Posn.y"];
	m_mtrPBPR2Posn.x		= pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrPBPR2Posn.x"];
	m_mtrPBPR2Posn.y		= pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_mtrPBPR2Posn.y"];

	m_mtrACFCenterPoint.x		= pAppMod->m_smfDevice[GetStnName()]["ACFOffset"]["m_mtrACFCenterPoint.x"];
	m_mtrACFCenterPoint.y		= pAppMod->m_smfDevice[GetStnName()]["ACFOffset"]["m_mtrACFCenterPoint.y"];

	m_dGlassRefAng			= pAppMod->m_smfDevice[GetStnName()]["AlignParam"]["m_dGlassRefAng"];
	m_dmtrAlignRef.x		= pAppMod->m_smfDevice[GetStnName()]["AlignParam"]["m_dmtrAlignRef.x"];
	m_dmtrAlignRef.y		= pAppMod->m_smfDevice[GetStnName()]["AlignParam"]["m_dmtrAlignRef.y"];
	m_dAlignRefAngle		= pAppMod->m_smfDevice[GetStnName()]["AlignParam"]["m_dAlignRefAngle"];
	m_dAlignRefDist			= pAppMod->m_smfDevice[GetStnName()]["AlignParam"]["m_dAlignRefDist"];

	HMI_dAlignDist = CountToDistance(m_dAlignRefDist, m_stMotorX.stAttrib.dDistPerCount) / 1000.0; // 20160513 display in mm

	// saving parameters
	for (INT nCurIndex = ACF1; nCurIndex < MAX_NUM_OF_ACF; nCurIndex++)
	{
		str.Format("ACF%ld", nCurIndex + 1);
		m_lBondOffsetY[nCurIndex]		= pAppMod->m_smfDevice[GetStnName()]["Para"]["m_lBondOffsetY"][str];
		m_dBondOffsetAdjY[nCurIndex]	= pAppMod->m_smfDevice[GetStnName()]["Para"]["m_dBondOffsetAdjY"][str]; //20120925
	}

	//for (INT i = WHZ_1; i < MAX_NUM_OF_WHZ; i++)
	//{
	//	str.Format("WHZ_%ld", i + 1);
	//	for (INT j = ACF1; j < MAX_NUM_OF_ACF; j++)
	//	{
	//		str1.Format("ACF%ld", j + 1);
	//		m_lBondLevelOffset[i][j]		= pAppMod->m_smfDevice[GetStnName()]["Level"][str]["m_lBondLevelOffset"][str1];
	//	}
	//}

	HMI_lBondOffsetY = m_lBondOffsetY[ACF1];
#ifdef ACF_POSN_CALIB //20120829
	HMI_dBondOffsetAdjY = m_dBondOffsetAdjY[ACF1]; //20120925
#endif

	return CWorkHolder::IPC_LoadDeviceParam();
}

LONG CACFWH::IPC_LoadMachineRecord()
{
	if (m_lCurSlave == ACFWH_1)
	{
		// for uploading records
		CString szPath = "";

		szPath = "d:\\sw\\AC9000\\Record\\" + pruACF_WHCalib.szFileName + ".zip";
		strcpy((char*) & (pruACF_WHCalib.stDownloadRecordCmd.acFilename[0]), szPath);
	}

	return 1;
}

LONG CACFWH::IPC_SaveMachineRecord()
{
	if (m_lCurSlave == ACFWH_1)
	{
		CString szPath = "";

		szPath = "d:\\sw\\AC9000\\Record\\" + pruACF_WHCalib.szFileName + ".zip";
		strcpy((char*) & (pruACF_WHCalib.stUploadRecordCmd.acFilename[0]), szPath);
	}

	return 1;
}

LONG CACFWH::IPC_LoadDeviceRecord()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	if (m_lCurSlave == ACFWH_1)
	{
		CString str = "";

		// for uploading records
		CString szPath = "";
		CString szDevice = pAppMod->m_szDeviceFile;
		INT nFind = szDevice.ReverseFind('.');

		szPath = "d:\\sw\\AC9000\\Record\\" + szDevice.Left(nFind) + "\\" + pruACF1.szFileName + ".zip";
		strcpy((char*) & (pruACF1.stDownloadRecordCmd.acFilename[0]), szPath);
		szPath = "d:\\sw\\AC9000\\Record\\" + szDevice.Left(nFind) + "\\" + pruACF2.szFileName + ".zip";
		strcpy((char*) & (pruACF2.stDownloadRecordCmd.acFilename[0]), szPath);

		szPath = "d:\\sw\\AC9000\\Record\\" + szDevice.Left(nFind) + "\\" + pruACFPB1[ACF1][OBJECT].szFileName + ".zip";
		strcpy((char*) & (pruACFPB1[ACF1][OBJECT].stDownloadRecordCmd.acFilename[0]), szPath);
		szPath = "d:\\sw\\AC9000\\Record\\" + szDevice.Left(nFind) + "\\" + pruACFPB2[ACF1][OBJECT].szFileName + ".zip";
		strcpy((char*) & (pruACFPB2[ACF1][OBJECT].stDownloadRecordCmd.acFilename[0]), szPath);
		
	}

	return 1;
}

LONG CACFWH::IPC_SaveDeviceRecord()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	if (m_lCurSlave == ACFWH_1)
	{
		CString str = "";

		// for uploading records
		CString szPath = "";
		CString szDevice = pAppMod->m_szDeviceFile;
		INT nFind = szDevice.ReverseFind('.');

		szPath = "d:\\sw\\AC9000\\Record\\" + szDevice.Left(nFind) + "\\" + pruACF1.szFileName + ".zip";
		strcpy((char*) & (pruACF1.stUploadRecordCmd.acFilename[0]), szPath);

		szPath = "d:\\sw\\AC9000\\Record\\" + szDevice.Left(nFind) + "\\" + pruACF2.szFileName + ".zip";
		strcpy((char*) & (pruACF2.stUploadRecordCmd.acFilename[0]), szPath);

		szPath = "d:\\sw\\AC9000\\Record\\" + szDevice.Left(nFind) + "\\" + pruACFPB1[ACF1][OBJECT].szFileName + ".zip";
		strcpy((char*) & (pruACFPB1[ACF1][OBJECT].stUploadRecordCmd.acFilename[0]), szPath);

		szPath = "d:\\sw\\AC9000\\Record\\" + szDevice.Left(nFind) + "\\" + pruACFPB2[ACF1][OBJECT].szFileName + ".zip";
		strcpy((char*) & (pruACFPB2[ACF1][OBJECT].stUploadRecordCmd.acFilename[0]), szPath);
	}


	return 1;
}
VOID CACFWH::ModulePRSetupPreTask()
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	m_bCheckMotor = TRUE;
	m_bCheckIO	= TRUE;
	pCInspOpt->m_bCheckMotor = TRUE;
	pCInspOpt->m_bCheckIO	= TRUE;
	m_pCACF->m_bCheckMotor = TRUE;
	m_pCACF->m_bCheckIO	= TRUE;

	CheckModSelected(TRUE);
	pCInspOpt->CheckModSelected(TRUE);
		
	SelectPRU(HMI_ulCurPRU, FALSE);
	UpdateSetupStatus();

	SetDiagnSteps(g_lDiagnSteps);
}

VOID CACFWH::ModulePRSetupPostTask()
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	m_bCheckMotor = FALSE;
	m_bCheckIO	= FALSE;
	pCInspOpt->m_bCheckMotor = FALSE;
	pCInspOpt->m_bCheckIO	= FALSE;
	m_pCACF->m_bCheckMotor = FALSE;
	m_pCACF->m_bCheckIO	= FALSE;

	MoveXYToStandbyPosn(GMP_NOWAIT);
}

LONG CACFWH::HMI_SetPRSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	SetPRSelected(bMode);

	return 0;
}

LONG CACFWH::HMI_PBEnable(IPC_CServiceMessage &svMsg) //20121016
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	if (bMode)
	{
		m_bPBEnable = TRUE;
	}
	else
	{
		m_bPBEnable = FALSE;
	}

	return 0;
}

LONG CACFWH::HMI_SelectPRU(IPC_CServiceMessage &svMsg)
{
	ULONG ulPRU;
	svMsg.GetMsg(sizeof(ULONG), &ulPRU);
	
	HMI_ulCurPRU = ulPRU;

	SelectPRU(ulPRU);

	PRS_ClearScreen(ACF_CAM);
	PRS_DrawHomeCursor(ACF_CAM, FALSE);

	return 0;
}

LONG CACFWH::HMI_SelectPBPRU(IPC_CServiceMessage &svMsg)
{
	ULONG ulPRU;
	svMsg.GetMsg(sizeof(ULONG), &ulPRU);
	
	HMI_ulCurPBPRU = ulPRU;

	SelectPBPRU(ulPRU);

	PRS_ClearScreen(ACF_CAM);
	PRS_DrawHomeCursor(ACF_CAM, FALSE);

	return 0;
}

LONG CACFWH::HMI_UpdateSetupStatus(IPC_CServiceMessage &svMsg)
{
	UpdateSetupStatus();
	return 0;
}

PRU &CACFWH::SelectCurPRU(ULONG ulPRU)
{
	switch (ulPRU)
	{
	case 0: // PR1
		return pruACF1;
		break;

	case 1:	// PR2
		return pruACF2;
		break;

	default:
		DisplayMessage("ACFWH SelectCurPRU Error");
		return pruACF_WHCalib;
	}
}

PRU &CACFWH::SelectCurPBPRU(ULONG ulPBPRU)
{
	switch (ulPBPRU)
	{
	case 0: // ACF PB1 
		return pruACFPB1[ACF_1][OBJECT];
	case 1: // ACF PB2
		return pruACFPB2[ACF_1][OBJECT];
	//case 2:	// ACF_MP1
	//	return pruACFPB_MP1[m_lCurACF][OBJECT];
	//case 3:	// ACF_MP2
	//	return pruACFPB_MP2[m_lCurACF][OBJECT];

	default:
		DisplayMessage("Kill Natalie:: ACF_WH SelectCurPBPRU Error");
		return pruACFPB1[ACF1][OBJECT];
	}
}

VOID CACFWH::UpdateSetupStatus()
{
	INT CurIndex = 0;

	for (CurIndex = ACF1; CurIndex < MAX_NUM_OF_ACF; CurIndex++)
	{
		m_stACF[CurIndex].bValid = TRUE;

		// Check Alignment Pt Ready
		if (!m_stACF[CurIndex].bEnable)
		{
			m_stACF[CurIndex].bValid = FALSE;
		}

		if (m_stACF[CurIndex].dLength == 0.0 || !pruACF1.bLoaded || !pruACF2.bLoaded || !m_stACF[CurIndex].bAlignPtSet || !m_stACF[CurIndex].bEnable)
		{
			m_stACF[CurIndex].bValid = FALSE;
		}

		// Check PostBond Ready
		//if (m_bPBEnable && (!pruACFPB1[CurIndex][OBJECT].bLoaded || !pruACFPB2[CurIndex][OBJECT].bLoaded || !g_stACF[CurIndex].bPostBondPtSet || !g_stACF[CurIndex].bEnable))
		//{
		//	g_stACF[CurIndex].bValid = FALSE;
		//}

		//if (m_bPBEnable && m_lNumOfPB_MP >= 1 && !pruACFPB_MP1[CurIndex][OBJECT].bLoaded)
		//{
		//	g_stACF[CurIndex].bValid = FALSE;
		//}

		//if (m_bPBEnable && m_lNumOfPB_MP >= 2 && !pruACFPB_MP2[CurIndex][OBJECT].bLoaded)
		//{
		//	g_stACF[CurIndex].bValid = FALSE;
		//}

	}
}

VOID CACFWH::SelectPRU(ULONG ulPRU, BOOL bIsMove) //20121221
{
	CString szMsg("");

	if (HMI_ulCurPRU > 2)
	{
		HMI_ulCurPRU = 0;
	}
	
	m_dCurrAlignAngle = 0.0;

	if (m_bModSelected)
	{
		MoveXYToStandbyPosn(GMP_WAIT);
		//m_pCOtherACFWH->MoveXYToStandbyPosn(GMP_WAIT);
		MoveTToStandby(GMP_WAIT);
		Sleep(100);
	}

	if (bIsMove) //20121221
	{
		if (!m_bModSelected)
		{
			szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
			HMIMessageBox(MSG_OK, "WARNING", szMsg);
			return;
		}
		MoveXYToCurPRPosn(ulPRU);
	}
	UpdateHMIPRButton();
}

VOID CACFWH::SelectPBPRU(ULONG ulPBPRU, BOOL bIsMove) //20121221
{
	CString szMsg("");

	if (HMI_ulCurPBPRU > 2)
	{
		HMI_ulCurPBPRU = 0;
	}
	
	m_dCurrAlignAngle = 0.0;

	if (m_bModSelected)
	{
		MoveXYToStandbyPosn(GMP_WAIT);
		//m_pCOtherACFWH->MoveXYToStandbyPosn(GMP_WAIT);
		MoveTToStandby(GMP_WAIT);
		Sleep(100);
	}

	if (bIsMove) //20121221
	{
		if (!m_bModSelected)
		{
			szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
			HMIMessageBox(MSG_OK, "WARNING", szMsg);
			return;
		}
		MoveXYToCurPBPosn(ulPBPRU);
	}
	UpdateHMIPBPRButton();
}

INT CACFWH::MoveXYToCurPRPosn(LONG lPRU)
{
	INT nResult = GMP_FAIL;
	switch (lPRU)
	{
	// Type1 Left Pattern
	case 0:
		nResult = MoveXYToCurrPR1Posn(GMP_WAIT);
		break;

	case 1:
		nResult = MoveXYToCurrPR2Posn(GMP_WAIT);
		break;
	}

	return GMP_FAIL;
}

INT CACFWH::MoveXYToCurPBPosn(LONG lPRU)
{
	INT nResult = GMP_FAIL;
	DOUBLE dAngle = 0.0;
	//MTR_POSN	mtrTemp;

	//if (DoACFDL())
	//{
	//	// Rotoate Theta for PostBond Searching
	//	dAngle = 1 * (m_dCurrAlignAngle - m_dBondOffsetT);

	//	if ((MoveRelativeT(m_lTOffset, GMP_WAIT) != gnAMS_OK))
	//	{
	//		return gnAMS_NOTOK;
	//	}
	//}		

	//mtrTemp.x = (LONG)m_dmtrCurrPBPR1Posn.x;
	//mtrTemp.y = (LONG)m_dmtrCurrPBPR1Posn.y;
	//CalPosnAfterRotate(EDGE_A, mtrTemp, -dAngle, &mtrTemp);
	//m_dmtrCurrPBPR1Posn.x = (DOUBLE)mtrTemp.x;
	//m_dmtrCurrPBPR1Posn.y = (DOUBLE)mtrTemp.y;

	//mtrTemp.x = (LONG)m_dmtrCurrPBPR2Posn.x;
	//mtrTemp.y = (LONG)m_dmtrCurrPBPR2Posn.y;
	//CalPosnAfterRotate(EDGE_A, mtrTemp, -dAngle, &mtrTemp);
	//m_dmtrCurrPBPR2Posn.x = (DOUBLE)mtrTemp.x;
	//m_dmtrCurrPBPR2Posn.y = (DOUBLE)mtrTemp.y;

	switch (lPRU)
	{
	case 0:
		nResult = MoveXYToCurrPB1Posn(GMP_WAIT);
		break;

	case 1:
		nResult = MoveXYToCurrPB2Posn(GMP_WAIT);
		break;
	}

	return !GMP_FAIL;
}

INT CACFWH::MoveXYToCurrPR1Posn(BOOL bWait)
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	INT bResult = GMP_FAIL;
	CString szMsg = "";
	//MTR_POSN mtrTemp;

	szMsg.Format("MoveXYToCurrPR1Posn : ACF%ldWH, m_dCurrAlignAngle = %.3f", m_lCurSlave + 1, m_dCurrAlignAngle);
	DisplayMessage(szMsg);

	//CalPosnAfterRotate(m_mtrPR1Posn[m_lCurGlass], m_dCurrAlignAngle, &mtrTemp);
	m_dmtrCurrPR1Posn.x = m_mtrPR1Posn[ACF1].x;	//mtrTemp.x;
	m_dmtrCurrPR1Posn.y = m_mtrPR1Posn[ACF1].y;	//mtrTemp.y;

	if (
		pCInspOpt->m_stMotorX.MoveAbsolute((LONG)m_dmtrCurrPR1Posn.x, bWait) == GMP_SUCCESS &&
		m_stMotorY.MoveAbsolute((LONG)m_dmtrCurrPR1Posn.y, bWait) == GMP_SUCCESS
		)
	{
		bResult = GMP_SUCCESS;
	}

	return bResult;
}

INT CACFWH::MoveXYToCurrPR2Posn(BOOL bWait)
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	INT bResult = GMP_FAIL;
	CString szMsg = "";
	//MTR_POSN mtrTemp;

	szMsg.Format("MoveXYToCurrPR2Posn : ACF%ldWH, m_dCurrAlignAngle = %.3f", m_lCurSlave + 1, m_dCurrAlignAngle);
	DisplayMessage(szMsg);

	//CalPosnAfterRotate(m_mtrPR2Posn[m_lCurGlass], m_dCurrAlignAngle, &mtrTemp);
	m_dmtrCurrPR2Posn.x = m_mtrPR2Posn[ACF1].x;	//mtrTemp.x;
	m_dmtrCurrPR2Posn.y = m_mtrPR2Posn[ACF1].y;	//mtrTemp.y;

	
	if (
		pCInspOpt->m_stMotorX.MoveAbsolute((LONG)m_dmtrCurrPR2Posn.x, bWait) == GMP_SUCCESS &&
		m_stMotorY.MoveAbsolute((LONG)m_dmtrCurrPR2Posn.y, bWait) == GMP_SUCCESS
		)
	{
		bResult = GMP_SUCCESS;
	}

	return bResult;
}

INT CACFWH::MoveXYToCurrPB1Posn(BOOL bWait)
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	INT bResult = GMP_FAIL;
	CString szMsg = "";
	//MTR_POSN mtrTemp;

	szMsg.Format("MoveXYToCurrPB1Posn : ACF%ldWH, m_dCurrAlignAngle = %.3f", m_lCurSlave + 1, m_dCurrAlignAngle);
	DisplayMessage(szMsg);

	//CalPosnAfterRotate(m_mtrPR1Posn[m_lCurGlass], m_dCurrAlignAngle, &mtrTemp);
	m_dmtrCurrPBPR1Posn.x = m_mtrPBPR1Posn.x;	//mtrTemp.x;
	m_dmtrCurrPBPR1Posn.y = m_mtrPBPR1Posn.y;	//mtrTemp.y;

	if (
		pCInspOpt->m_stMotorX.MoveAbsolute((LONG)m_dmtrCurrPBPR1Posn.x, bWait) == GMP_SUCCESS &&
		m_stMotorY.MoveAbsolute((LONG)m_dmtrCurrPBPR1Posn.y, bWait) == GMP_SUCCESS
		)
	{
		bResult = GMP_SUCCESS;
	}

	return bResult;
}

INT CACFWH::MoveXYToCurrPB2Posn(BOOL bWait)
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	INT bResult = GMP_FAIL;
	CString szMsg = "";
	//MTR_POSN mtrTemp;

	szMsg.Format("MoveXYToCurrPR2Posn : ACF%ldWH, m_dCurrAlignAngle = %.3f", m_lCurSlave + 1, m_dCurrAlignAngle);
	DisplayMessage(szMsg);

	//CalPosnAfterRotate(m_mtrPR2Posn[m_lCurGlass], m_dCurrAlignAngle, &mtrTemp);
	m_dmtrCurrPBPR1Posn.x = m_mtrPBPR2Posn.x;	//mtrTemp.x;
	m_dmtrCurrPBPR1Posn.y = m_mtrPBPR2Posn.y;	//mtrTemp.y;

	
	if (
		pCInspOpt->m_stMotorX.MoveAbsolute((LONG)m_dmtrCurrPBPR1Posn.x, bWait) == GMP_SUCCESS &&
		m_stMotorY.MoveAbsolute((LONG)m_dmtrCurrPBPR1Posn.y, bWait) == GMP_SUCCESS
		)
	{
		bResult = GMP_SUCCESS;
	}

	return bResult;
}

LONG CACFWH::HMI_SetTLoadOffset(IPC_CServiceMessage &svMsg)
{
	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");
	MTR_POSN mtrTemp0, mtrTemp1, mtrGlassAlignPt1;

	mtrGlassAlignPt1.x = m_mtrAlignRefPosnOnWH.x;
	mtrGlassAlignPt1.y = m_mtrAlignRefPosnOnWH.y;

	DOUBLE dAngle;
	svMsg.GetMsg(sizeof(DOUBLE), &dAngle);

	m_dLoadTOffset = dAngle;
	m_lLoadTOffset = DistanceToCount(dAngle, m_stMotorT.stAttrib.dDistPerCount);
	
	mtrTemp0.x = (m_mtrACFAnvilLLPosn.x + m_mtrACFAnvilLRPosn.x)/2 - DistanceToCount((DOUBLE)m_lLoadXOffset, m_stMotorX.stAttrib.dDistPerCount);
	mtrTemp0.y = (m_mtrACFAnvilLLPosn.y + m_mtrACFAnvilLRPosn.y)/2 + DistanceToCount((DOUBLE)m_lLoadYOffset, m_stMotorY.stAttrib.dDistPerCount);

	CalPosnAfterRotate(EDGE_A, mtrTemp0, -dAngle, &mtrTemp1);
	if(m_lCurSlave == ACFWH_1)
	{
		pCTA1->m_lPlaceGlass1Offset = mtrTemp1.x - mtrGlassAlignPt1.x;//mtrTemp0.x;
	}
	else
	{
		pCTA1->m_lPlaceGlass2Offset = mtrTemp1.x - mtrGlassAlignPt1.x;//mtrTemp0.x;
	}
	m_mtrLoadGlassOffset.y = mtrTemp1.y - mtrGlassAlignPt1.y;//mtrTemp0.y;
	
	UpdateModuleSetupPosn();

	return 1;
}

LONG CACFWH::HMI_SetPR1Posn(IPC_CServiceMessage &svMsg)
{	
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	CString szOldValue;
	szOldValue.Format("X1 = %ld, Y1 = %ld",
		m_mtrPR1Posn[ACF1].x, m_mtrPR1Posn[ACF1].y);

	LONG lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Overwrite Position?");
	if (lAnswer == rMSG_TIMEOUT)
	{
		HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
		return 0;
	}
	else if (lAnswer == rMSG_CONTINUE)
	{
		m_mtrPR1Posn[ACF1].x = pCInspOpt->m_stMotorX.GetEncPosn();
		m_mtrPR1Posn[ACF1].y = m_stMotorY.GetEncPosn();
	}

	CString szNewValue;
	szNewValue.Format("X1 = %ld, Y1 = %ld",
		m_mtrPR1Posn[ACF1].x, m_mtrPR1Posn[ACF1].y);

	CString szParameter;
	szParameter.Format("ACF%ldWH PR1 Position", m_lCurSlave + 1);
	LogParameter(__FUNCTION__, szParameter, szOldValue, szNewValue);

	return 0;
}

LONG CACFWH::HMI_SetPR2Posn(IPC_CServiceMessage &svMsg)
{	
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	CString szOldValue;
	szOldValue.Format("X2 = %ld, Y2 = %ld",
		m_mtrPR2Posn[ACF1].x, m_mtrPR2Posn[ACF1].y);

	LONG lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Overwrite Position?");
	if (lAnswer == rMSG_TIMEOUT)
	{
		HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
		return 0;
	}
	else if (lAnswer == rMSG_CONTINUE)
	{

		m_mtrPR2Posn[ACF1].x = pCInspOpt->m_stMotorX.GetEncPosn();
		m_mtrPR2Posn[ACF1].y = m_stMotorY.GetEncPosn();
	}

	CString szNewValue;
	szNewValue.Format(" X2 = %ld, Y2 = %ld",
		m_mtrPR2Posn[ACF1].x, m_mtrPR2Posn[ACF1].y);

	CString szParameter;
	szParameter.Format("ACF%ldWH PR2 Position", m_lCurSlave + 1);
	LogParameter(__FUNCTION__, szParameter, szOldValue, szNewValue);

	return 0;
}

LONG CACFWH::HMI_SetPBPR1Posn(IPC_CServiceMessage &svMsg)
{	
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	CString szOldValue;
	szOldValue.Format("X1 = %ld, Y1 = %ld",
		m_mtrPBPR1Posn.x, m_mtrPBPR1Posn.y);

	LONG lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Overwrite Position?");
	if (lAnswer == rMSG_TIMEOUT)
	{
		HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
		return 0;
	}
	else if (lAnswer == rMSG_CONTINUE)
	{
		m_mtrPBPR1Posn.x = pCInspOpt->m_stMotorX.GetEncPosn();
		m_mtrPBPR1Posn.y = m_stMotorY.GetEncPosn();
	}

	CString szNewValue;
	szNewValue.Format("X1 = %ld, Y1 = %ld",
		m_mtrPBPR1Posn.x, m_mtrPBPR1Posn.y);

	CString szParameter;
	szParameter.Format("ACF%ldWH PBPR1 Position", m_lCurSlave + 1);
	LogParameter(__FUNCTION__, szParameter, szOldValue, szNewValue);

	return 0;
}

LONG CACFWH::HMI_SetPBPR2Posn(IPC_CServiceMessage &svMsg)
{	
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	CString szOldValue;
	szOldValue.Format("X2 = %ld, Y2 = %ld",
		m_mtrPBPR2Posn.x, m_mtrPBPR2Posn.y);

	LONG lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Overwrite Position?");
	if (lAnswer == rMSG_TIMEOUT)
	{
		HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
		return 0;
	}
	else if (lAnswer == rMSG_CONTINUE)
	{

		m_mtrPBPR2Posn.x = pCInspOpt->m_stMotorX.GetEncPosn();
		m_mtrPBPR2Posn.y = m_stMotorY.GetEncPosn();
	}

	CString szNewValue;
	szNewValue.Format(" X2 = %ld, Y2 = %ld",
		m_mtrPBPR2Posn.x, m_mtrPBPR2Posn.y);

	CString szParameter;
	szParameter.Format("ACF%ldWH PR2 Position", m_lCurSlave + 1);
	LogParameter(__FUNCTION__, szParameter, szOldValue, szNewValue);

	return 0;
}

VOID CACFWH::SetupWizard()
{
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");
	CWinEagle *pCWinEagle = (CWinEagle*)m_pModule->GetStation("WinEagle");
	CSettingFile *pCSettingFile = (CSettingFile*)m_pModule->GetStation("SettingFile");
	
	PRU *pPRU;

	switch (g_ulWizardSubTitle)
	{
	case 0:
		g_szWizardTitle.Format("ACF DL Align Pt Setup Procedure");
		LogAction("ACFWH: " + g_szWizardTitle);
		SetHmiVariable("SF_szWizardTitle");
		//pCSettingFile->HMI_bShowIgnoreWindowBtn = FALSE; //20150506
		//SetHmiVariable("SF_bShowIgnoreWindowBtn");
		SetupAlignPt();
		break;

	case 1:
		g_szWizardTitle.Format("ACF DL PR%d Pattern Setup Procedure", HMI_ulCurPRU + 1);	//p20121022
		LogAction("ACFWH: " + g_szWizardTitle);
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
		//SetupPRPatternPreTask(pPRU);
		pCWinEagle->SetupPRPattern(pPRU);
		//SetupPRPatternPostTask();
		break;

	case 2:
		g_szWizardTitle.Format("ACF DL Srch Region Setup Procedure");
		LogAction("ACFWH: " + g_szWizardTitle);
		SetHmiVariable("SF_szWizardTitle");
		pPRU = &SelectCurPRU(HMI_ulCurPRU);
		pCWinEagle->SetupPRSrchArea(pPRU);
		break;

	case 3:

		g_szWizardTitle.Format("ACF%ldWH ACF Setup Procedure", m_lCurSlave + 1);
		LogAction("ACFWH: " + g_szWizardTitle);
		SetHmiVariable("SF_szWizardTitle");
		//pCSettingFile->HMI_bShowIgnoreWindowBtn = FALSE; //20150506
		//SetHmiVariable("SF_bShowIgnoreWindowBtn");
		SetupACFOffset();
		break;

	case 4:

		g_szWizardTitle.Format("ACFWH Load Posn Offset Setup Procedure");
		LogAction("TA1: " + g_szWizardTitle);
		SetHmiVariable("SF_szWizardTitle");
		//pCSettingFile->HMI_bShowIgnoreWindowBtn = FALSE; //20150506
		//SetHmiVariable("SF_bShowIgnoreWindowBtn");
		SetupLoadPosnOffset();

		break;

	case 5:
		g_szWizardTitle.Format("ACF Pattern Setup Procedure");
		LogAction("ACF Post Bond Inspection: " + g_szWizardTitle);
		SetHmiVariable("SF_szWizardTitle");
		pPRU = &SelectCurPBPRU(HMI_ulCurPRU);
		SetupACFPattern();
		break;
	}
}

VOID CACFWH::SetupPRPatternPreTask(PRU *pPRU)
{
	BOOL bResult = TRUE;
	LONG lAnswer = rMSG_TIMEOUT;

	switch (g_ulWizardStep)
	{
	case 0:
		if (
			IndexGlassToTable((GlassNum)GLASS1) != GMP_SUCCESS ||
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

		for (INT i = 0 ; i < MAX_NUM_OF_ACF; i++)
				{
					m_stACF[i].bAlignPtSet = FALSE;
					//m_pCOtherACFWH->m_stACF[i].bAlignPtSet = FALSE;
				}


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

VOID CACFWH::SetupPRPatternPostTask()
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	switch (g_ulWizardStep)
	{
	case 99:
		pCInspOpt->MoveX(INSPOPT_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
		MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT);
		break;
	}
}


BOOL CACFWH::SetGlassRef(BOOL bSetAng)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	DOUBLE dx,dy;

	m_dmtrCurrPR1Posn.x = (DOUBLE)m_mtrPR1Posn[ACF1].x;
	m_dmtrCurrPR1Posn.y = (DOUBLE)m_mtrPR1Posn[ACF1].y;

	if (
		pCInspOpt->m_stMotorX.MoveAbsolute((LONG)m_dmtrCurrPR1Posn.x,  GMP_WAIT)  != GMP_SUCCESS ||
		m_stMotorY.MoveAbsolute((LONG)m_dmtrCurrPR1Posn.y,  GMP_WAIT) != GMP_SUCCESS
	)
	{
		return FALSE;
	}
	if (!SearchPRCentre(&pruACF1))
	{
		PRS_DisplayText(pruACF1.emCameraNo, 1, PRS_MSG_ROW1, "PR Failed!");
		return FALSE;
	}

	PRS_DrawCross(pruACF1.emCameraNo, pruACF1.rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

	m_dmtrCurrPR1Posn.x = (DOUBLE)pCInspOpt->m_stMotorX.GetEncPosn();
	m_dmtrCurrPR1Posn.y = (DOUBLE)m_stMotorY.GetEncPosn();

	m_dmtrCurrPR2Posn.x = (DOUBLE)m_mtrPR2Posn[ACF1].x;
	m_dmtrCurrPR2Posn.y = (DOUBLE)m_mtrPR2Posn[ACF1].y;

	if (
		pCInspOpt->m_stMotorX.MoveAbsolute((LONG)m_dmtrCurrPR2Posn.x,  GMP_WAIT)  != GMP_SUCCESS ||
		m_stMotorY.MoveAbsolute((LONG)m_dmtrCurrPR2Posn.y,  GMP_WAIT)  != GMP_SUCCESS
		)
	{
		return FALSE;
	}

	if (!SearchPRCentre(&pruACF2))
	{
		PRS_DisplayText(pruACF2.emCameraNo, 1, PRS_MSG_ROW1, "PR Failed!");
		return FALSE;
	}
	
	
	PRS_DrawCross(pruACF2.emCameraNo, pruACF2.rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);
	
	m_dmtrCurrPR2Posn.x = (DOUBLE)pCInspOpt->m_stMotorX.GetEncPosn();
	m_dmtrCurrPR2Posn.y = (DOUBLE)m_stMotorY.GetEncPosn();
	if (bSetAng)
	{
		dx = 1.0 * (m_dmtrCurrPR2Posn.x - m_dmtrCurrPR1Posn.x);
		dy = 1.0 * (m_dmtrCurrPR2Posn.y - m_dmtrCurrPR1Posn.y);

		if (dx != 0)
		{
			m_dGlassRefAng = (180.0 / PI) * atan((dy / dx));
		}
		else
		{
			m_dGlassRefAng = 0.0;
		}

		//LONG lStep;
		//lStep = AngleToCount(m_dGlassRefAng, m_stMotorT.stAttrib.dDistPerCount);
		//	
		//if (GetMotorT().MoveRelative(lStep, GMP_WAIT) != GMP_SUCCESS)
		//{
		//	HMIMessageBox(MSG_OK, "SET ALIGN POINT", "Auto Align is aborted!");
		//}
		CString szMsg = _T("");
		szMsg.Format("Glass Reference Angle Found: %.3f", m_dGlassRefAng);
		WriteHMIMess(szMsg);
		// Debug
		szMsg.Format("PR1 Posn: (%.2f, %.2f), PR2 Posn: (%.2f, %.2f)", m_mtrPR1Posn[m_lCurEdge].x, m_mtrPR1Posn[m_lCurEdge].y, m_mtrPR2Posn[m_lCurEdge].x, m_mtrPR2Posn[m_lCurEdge].y);
		DisplayMessage(szMsg);
	}

	return TRUE;
}

VOID CACFWH::SetupAlignPt()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	static BOOL	bMovePrevPosn = FALSE;
	BOOL		bResult 	= TRUE;
	CString		szAxis 		= "";
	LONG		lAnswer		= rMSG_TIMEOUT;

	CString		strMsg;
	MTR_POSN	mtrTemp;

	if (g_ulWizardStep > 3 && g_ulWizardStep != 99)
	{
		g_ulWizardStep = 3;
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


	if (m_lCurACF <= NO_ACF || m_lCurACF >= MAX_NUM_OF_ACF)
	{		
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		DisplayMessage(__FUNCTION__);
		return;
	}

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
				 m_stACF[m_lCurACF].dmtrAlignPt1.x, m_stACF[m_lCurACF].dmtrAlignPt1.y,
				 m_stACF[m_lCurACF].dmtrAlignPt2.x, m_stACF[m_lCurACF].dmtrAlignPt2.y);
		LogAction("AlignRef.x = %.5f\t AlignRef.y = %.5f\t AlignAng = %.5f", 
				  m_dmtrAlignRef.x, m_dmtrAlignRef.y,
				  m_dAlignRefAngle);

		break;

	case 0:
	// 1. Index Glass to ACFWH
	// 2. Search PR
		LogAction(__FUNCTION__);
		LogAction("AlignPt1.x = %.5f\t AlignPt1.y = %.5f\t AlignPt2.x = %.5f\t AlignPt2.y = %.5f\t", 
				 m_stACF[m_lCurACF].dmtrAlignPt1.x, m_stACF[m_lCurACF].dmtrAlignPt1.y,
				 m_stACF[m_lCurACF].dmtrAlignPt2.x, m_stACF[m_lCurACF].dmtrAlignPt2.y);
		LogAction("AlignRef.x = %.5f\t AlignRef.y = %.5f\t AlignAng = %.5f", 
				  m_dmtrAlignRef.x, m_dmtrAlignRef.y,
				  m_dAlignRefAngle);

		//if (IndexGlassToTable((GlassNum)m_lCurSlave) != GMP_SUCCESS)
		//{
		//	bResult = FALSE;
		//}
		//else
		{
			if (
				//m_pCOtherACFWH->MoveT(ACF_WH_T_CTRL_GO_STANDBY, GMP_WAIT) != GMP_SUCCESS ||
				MoveXYToCurrPR1Posn(GMP_WAIT) != GMP_SUCCESS ||
				MoveT(ACF_WH_T_CTRL_GO_STANDBY, GMP_WAIT) != GMP_SUCCESS ||
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
					mtrTemp.x = (LONG)(m_dmtrCurrPR1Posn.x + m_stACF[m_lCurACF].dmtrAlignPt1.x);
					mtrTemp.y = (LONG)(m_dmtrCurrPR1Posn.y + m_stACF[m_lCurACF].dmtrAlignPt1.y);
				}
				else
				{
					mtrTemp.x = (LONG)m_dmtrCurrPR1Posn.x;
					mtrTemp.y = (LONG)m_dmtrCurrPR1Posn.y;
				}
				if (
					pCInspOpt->m_stMotorX.MoveAbsolute(mtrTemp.x, GMP_WAIT)  != GMP_SUCCESS ||
					m_stMotorY.MoveAbsolute(mtrTemp.y,  GMP_WAIT) != GMP_SUCCESS
					)
				{
					bResult = FALSE;
				}
				else
				{
					PRS_DisplayVideo(&pruACF_WHCalib);
					PRS_DrawHomeCursor(ACF_CAM, FALSE);

					g_szWizardStep = "Step 1/2";
					g_szWizardMessPrev = "";
					g_szWizardMess = "1. Set Align Pt 1 Position";	
					g_szWizardMessNext = "2. Set Align Pt 2 Position";
					g_szWizardBMPath = "";
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
			m_stACF[m_lCurACF].bAlignPtSet = FALSE;

			m_stACF[m_lCurACF].dmtrAlignPt1.x = ((DOUBLE)pCInspOpt->m_stMotorX.GetEncPosn() - m_dmtrCurrPR1Posn.x);	
			m_stACF[m_lCurACF].dmtrAlignPt1.y = ((DOUBLE)m_stMotorY.GetEncPosn() - m_dmtrCurrPR1Posn.y);
			//m_pCOtherACFWH->m_stACF[m_lCurACF].dmtrAlignPt1.x = m_stACF[m_lCurACF].dmtrAlignPt1.x;
			//m_pCOtherACFWH->m_stACF[m_lCurACF].dmtrAlignPt1.y = m_stACF[m_lCurACF].dmtrAlignPt1.y;
		
			if (bMovePrevPosn)
			{
				mtrTemp.x = (LONG)(m_dmtrCurrPR2Posn.x + m_stACF[m_lCurACF].dmtrAlignPt2.x);
				mtrTemp.y = (LONG)(m_dmtrCurrPR2Posn.y + m_stACF[m_lCurACF].dmtrAlignPt2.y);
			}
			else
			{
				mtrTemp.x = (LONG)m_dmtrCurrPR2Posn.x;
				mtrTemp.y = (LONG)m_dmtrCurrPR2Posn.y;
			}
			if (
				pCInspOpt->m_stMotorX.MoveAbsolute(mtrTemp.x,  GMP_WAIT)  != GMP_SUCCESS ||
				m_stMotorY.MoveAbsolute(mtrTemp.y,  GMP_WAIT) != GMP_SUCCESS)
			{
				bResult = FALSE;
			}
			else
			{
				g_szWizardStep = "Step 2/2";
				g_szWizardMessPrev = "1. Set Align Pt 1 Position";
				g_szWizardMess = "2. Set Align Pt 2 Position";
				g_szWizardMessNext = "DONE";
				g_szWizardBMPath = "";
			}
		}

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;
		break;

	case 2:
	// 2. Set Align Pick Pt 2 position
		m_stACF[m_lCurACF].dmtrAlignPt2.x = ((DOUBLE)pCInspOpt->m_stMotorX.GetEncPosn() - m_dmtrCurrPR2Posn.x);		
		m_stACF[m_lCurACF].dmtrAlignPt2.y = ((DOUBLE)m_stMotorY.GetEncPosn() - m_dmtrCurrPR2Posn.y);
		//m_pCOtherACFWH->m_stACF[m_lCurACF].dmtrAlignPt2.x = m_stACF[m_lCurACF].dmtrAlignPt2.x;
		//m_pCOtherACFWH->m_stACF[m_lCurACF].dmtrAlignPt2.y = m_stACF[m_lCurACF].dmtrAlignPt2.y;

		//m_mtrBondOffsetXY.x = DoubleToLong(m_mtrACFCenterPoint.x - (m_dmtrCurrPR1Posn.x + m_stACF[m_lCurACF].dmtrAlignPt1.x + m_dmtrCurrPR2Posn.x + m_stACF[m_lCurACF].dmtrAlignPt2.x) / 2);
		//m_mtrBondOffsetXY.y = DoubleToLong(m_mtrACFCenterPoint.y - (m_dmtrCurrPR1Posn.y + m_stACF[m_lCurACF].dmtrAlignPt1.y + m_dmtrCurrPR2Posn.y + m_stACF[m_lCurACF].dmtrAlignPt2.y) / 2);

		m_stACF[m_lCurACF].bAlignPtSet = TRUE;
		//m_pCOtherACFWH->m_stACF[m_lCurACF].bAlignPtSet = TRUE;

		D_MTR_POSN dmtrCurrAlign1Posn, dmtrCurrAlign2Posn;
		dmtrCurrAlign1Posn.x = m_dmtrCurrPR1Posn.x + m_stACF[m_lCurACF].dmtrAlignPt1.x;		// 20160513 Add Align Distance
		dmtrCurrAlign1Posn.y = m_dmtrCurrPR1Posn.y + m_stACF[m_lCurACF].dmtrAlignPt1.y;

		dmtrCurrAlign2Posn.x = m_dmtrCurrPR2Posn.x + m_stACF[m_lCurACF].dmtrAlignPt2.x;
		dmtrCurrAlign2Posn.y = m_dmtrCurrPR2Posn.y + m_stACF[m_lCurACF].dmtrAlignPt2.y;
		m_dAlignRefDist =  sqrt(pow((dmtrCurrAlign2Posn.x - dmtrCurrAlign1Posn.x), 2.0) + pow((dmtrCurrAlign2Posn.y - dmtrCurrAlign1Posn.y), 2.0));
		//m_pCOtherACFWH->m_dAlignRefDist = m_dAlignRefDist;



		g_szWizardStep = "DONE";
		g_szWizardMess = "DONE";
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = FALSE;
		g_bWizardEnableExit = TRUE;
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

LONG CACFWH::HMI_SearchAp1(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	BOOL bResult = TRUE;

	CString szMsg = "";
	MTR_POSN mtrTemp1;
	MTR_POSN mtrTemp;
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

	if (!m_pCOtherACFWH->m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", m_pCOtherACFWH->GetStnName());
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}
	m_pCOtherACFWH->MoveXYToStandbyPosn(GMP_WAIT);

	if (!m_bPRSelected)
	{
		szMsg.Format("%s PR Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (!pruACF1.bLoaded || !pruACF2.bLoaded)
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "PR Pattern Not Loaded. Operation Abort!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (!m_stACF[m_lCurACF].bAlignPtSet)
	{
		szMsg.Format("ACF %ld Align Points not set. Please set Align Pts first.", m_lCurACF + 1);
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Align Pts Not Learned. Operation Abort!");	
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	m_dCurrAlignAngle = 0.0;

	if (m_lCurSlave == ACFWH_2)
	{
		pruACF1.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF2WH;
		pruACF2.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF2WH;
	}
	else
	{
		pruACF1.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF1WH;
		pruACF2.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF1WH;
	}

	MoveTToStandby(GMP_WAIT);

	if (!AutoSearchPR1(&pruACF1))
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR1 Failed!");
		PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}
	pCInspOpt->m_stMotorX.MoveAbsolute(DoubleToLong(m_dmtrCurrPR1Posn.x),  GMP_WAIT);
	m_stMotorY.MoveAbsolute(DoubleToLong(m_dmtrCurrPR1Posn.y),  GMP_WAIT);

	if (!AutoSearchPR2(&pruACF2))
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR2 Failed!");
		PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}
	pCInspOpt->m_stMotorX.MoveAbsolute(DoubleToLong(m_dmtrCurrPR2Posn.x),  GMP_WAIT);
	m_stMotorY.MoveAbsolute(DoubleToLong(m_dmtrCurrPR2Posn.y),  GMP_WAIT);

	PRS_ClearScreen(ACF_CAM);
	
	CalcAlignPt();
	mtrTemp1.x = DoubleToLong(m_dmtrCurrPR1Posn.x + m_dmtrCurrAlign1Offset.x);
	mtrTemp1.y = DoubleToLong(m_dmtrCurrPR1Posn.y + m_dmtrCurrAlign1Offset.y);

	pCInspOpt->m_stMotorX.MoveAbsolute(mtrTemp1.x,  GMP_WAIT);
	m_stMotorY.MoveAbsolute(mtrTemp1.y,  GMP_WAIT);

	Sleep(1000);

	DOUBLE dAngle = 1.0 * (m_dCurrAlignAngle);

	m_lTOffset = DistanceToCount(dAngle, m_stMotorT.stAttrib.dDistPerCount);

	CalPosnAfterRotate(m_lCurEdge, mtrTemp1, -dAngle, &mtrTemp);

//#if 1 //20160815
//	MTR_POSN mtrTemp2 = {0,};
//	//mtrTemp2.x = mtrTemp.x - mtrTemp1.x;
//	mtrTemp2.y = -1 * (mtrTemp.y - mtrTemp1.y);
//	mtrTemp.y = mtrTemp1.y + mtrTemp2.y;
//#endif
	m_stMotorT.MoveRelative(m_lTOffset, GMP_WAIT);
	pCInspOpt->m_stMotorX.MoveAbsolute(mtrTemp.x,  GMP_WAIT);
	m_stMotorY.MoveAbsolute(mtrTemp.y,  GMP_WAIT);
	MoveAbsoluteXY(mtrTemp.x, mtrTemp.y, GMP_WAIT);

	PRS_DrawCross(ACF_CAM, coCentre, PRS_XLARGE_SIZE, PR_COLOR_RED);
	PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Succeed!");

	szMsg.Format("Align Angle = %.5f, Align Distance = %.5f", m_dCurrAlignAngle, CountToDistance(m_dCurrAlignDist, pCInspOpt->m_stMotorX.stAttrib.dDistPerCount) / 1000.0);
	WriteHMIMess(szMsg);

	//MoveXYToStandbyPosn(GMP_WAIT);
	MoveRelativeXY(400, 800, GMP_WAIT);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CACFWH::HMI_SearchAp2(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	BOOL bResult = TRUE;

	CString szMsg = "";
	MTR_POSN mtrTemp1;
	MTR_POSN mtrTemp;
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

	if (!m_pCOtherACFWH->m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", m_pCOtherACFWH->GetStnName());
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}
	m_pCOtherACFWH->MoveXYToStandbyPosn(GMP_WAIT);

	if (!m_bPRSelected)
	{
		szMsg.Format("%s PR Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (!m_stACF[m_lCurACF].bEnable)
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT2", "Current ACF Not Enabled. Operation Abort!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (!pruACF1.bLoaded || !pruACF2.bLoaded)
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "PR Pattern Not Loaded. Operation Abort!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (!m_stACF[m_lCurACF].bAlignPtSet)
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Align Pts Not Learned. Operation Abort!");	
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	m_dCurrAlignAngle = 0.0;

	if (m_lCurSlave == ACFWH_2)
	{
		pruACF1.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF2WH;
		pruACF2.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF2WH;
	}
	else
	{
		pruACF1.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF1WH;
		pruACF2.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF1WH;
	}
	MoveTToStandby(GMP_WAIT);
	if (!AutoSearchPR1(&pruACF1))
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR1 Failed!");
		PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}
	
	if (!AutoSearchPR2(&pruACF2))
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR2 Failed!");
		PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}
	PRS_ClearScreen(ACF_CAM);
	
	CalcAlignPt();


	mtrTemp1.x = DoubleToLong(m_dmtrCurrPR2Posn.x + m_dmtrCurrAlign2Offset.x);
	mtrTemp1.y = DoubleToLong(m_dmtrCurrPR2Posn.y + m_dmtrCurrAlign2Offset.y);

	DOUBLE dAngle = 1.0 * (m_dCurrAlignAngle);

	m_lTOffset = DistanceToCount(dAngle, m_stMotorT.stAttrib.dDistPerCount);

	CalPosnAfterRotate(m_lCurEdge, mtrTemp1, -dAngle, &mtrTemp);

//#if 0 //20160815
//	MTR_POSN mtrTemp2 = {0,};
//	//mtrTemp2.x = mtrTemp.x - mtrTemp1.x;
//	mtrTemp2.y = -1 * (mtrTemp.y - mtrTemp1.y);
//	mtrTemp.y = mtrTemp1.y + mtrTemp2.y;
//#endif

	m_stMotorT.MoveRelative(m_lTOffset, GMP_WAIT);
	pCInspOpt->m_stMotorX.MoveAbsolute(mtrTemp.x,  GMP_WAIT);
	m_stMotorY.MoveAbsolute(mtrTemp.y,  GMP_WAIT);
	//MoveAbsoluteXY(mtrTemp.x, mtrTemp.y, GMP_WAIT);

	PRS_DrawCross(ACF_CAM, coCentre, PRS_XLARGE_SIZE, PR_COLOR_RED);
	PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Succeed!");

	szMsg.Format("Align Angle = %.5f, Align Distance = %.5f", m_dCurrAlignAngle, CountToDistance(m_dCurrAlignDist, pCInspOpt->m_stMotorX.stAttrib.dDistPerCount) / 1000.0);
	WriteHMIMess(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CACFWH::HMI_SearchCentre(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	BOOL bResult = TRUE;

	CString szMsg = "";
	MTR_POSN mtrTemp;
	mtrTemp.x = 0;
	mtrTemp.y = 0;
	MTR_POSN mtrAlignCenter;
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

	if (!m_pCOtherACFWH->m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", m_pCOtherACFWH->GetStnName());
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}
	m_pCOtherACFWH->MoveXYToStandbyPosn(GMP_WAIT);

	if (!m_bPRSelected)
	{
		szMsg.Format("%s PR Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (!m_stACF[m_lCurACF].bEnable)
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN CENTRE", "Current ACF Not Enabled. Operation Abort!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (!pruACF1.bLoaded || !pruACF2.bLoaded)
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "PR Pattern Not Loaded. Operation Abort!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (!m_stACF[m_lCurACF].bAlignPtSet)
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Align Pts Not Learned. Operation Abort!");	
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	m_dCurrAlignAngle = 0.0;

	if (m_lCurSlave == ACFWH_2)
	{
		pruACF1.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF2WH;
		pruACF2.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF2WH;
	}
	else
	{
		pruACF1.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF1WH;
		pruACF2.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF1WH;
	}
	MoveTToStandby(GMP_WAIT);
	if (!AutoSearchPR1(&pruACF1))
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR1 Failed!");
		PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}
	pCInspOpt->m_stMotorX.MoveAbsolute(DoubleToLong(m_dmtrCurrPR1Posn.x),  GMP_WAIT);
	m_stMotorY.MoveAbsolute(DoubleToLong(m_dmtrCurrPR1Posn.y),  GMP_WAIT);

	if (!AutoSearchPR2(&pruACF2))
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR2 Failed!");
		PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}
	pCInspOpt->m_stMotorX.MoveAbsolute(DoubleToLong(m_dmtrCurrPR2Posn.x),  GMP_WAIT);
	m_stMotorY.MoveAbsolute(DoubleToLong(m_dmtrCurrPR2Posn.y),  GMP_WAIT);

	PRS_ClearScreen(ACF_CAM);
	
	CalcAlignPt();

	DOUBLE dAngle = 1.0 * (m_dCurrAlignAngle);

	m_lTOffset = DistanceToCount(dAngle, m_stMotorT.stAttrib.dDistPerCount);

	mtrAlignCenter.x = DoubleToLong(m_dmtrCurrAlignCentre.x);
	mtrAlignCenter.y = DoubleToLong(m_dmtrCurrAlignCentre.y);

	CalPosnAfterRotate(m_lCurEdge, mtrAlignCenter, -dAngle, &mtrTemp);

	m_stMotorT.MoveRelative(m_lTOffset, GMP_WAIT);
	pCInspOpt->m_stMotorX.MoveAbsolute((LONG)mtrTemp.x,  GMP_WAIT);
	m_stMotorY.MoveAbsolute((LONG)mtrTemp.y,  GMP_WAIT);
	//MoveAbsoluteXY(DoubleToLong(m_dmtrCurrAlignCentre.x), DoubleToLong(m_dmtrCurrAlignCentre.y), GMP_WAIT);

	PRS_DrawCross(ACF_CAM, coCentre, PRS_XLARGE_SIZE, PR_COLOR_RED);
	PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Succeed!");

	szMsg.Format("Align Angle = %.5f, Align Distance = %.5f", m_dCurrAlignAngle, CountToDistance(m_dCurrAlignDist, pCInspOpt->m_stMotorX.stAttrib.dDistPerCount) / 1000.0);
	WriteHMIMess(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

VOID CACFWH::CalcAlignPt()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	D_MTR_POSN dmtrCurrAlign1Posn;
	D_MTR_POSN dmtrCurrAlign2Posn;
	
	DOUBLE dx, dy;
	DOUBLE dAngle, dAngleRad, dRefAngleRad;
	
	CString str;


	// if PR not selected, return ref posn
	if (!m_bPRSelected || !m_stACF[m_lCurACF].bAlignPtSet)
	{
		m_dCurrAlignAngle = 0.0;
		m_dmtrCurrPR1Posn.x = (DOUBLE)m_mtrPR1Posn[ACF1].x;
		m_dmtrCurrPR1Posn.y = (DOUBLE)m_mtrPR1Posn[ACF1].y;
		m_dmtrCurrPR2Posn.x = (DOUBLE)m_mtrPR2Posn[ACF1].x;
		m_dmtrCurrPR2Posn.y = (DOUBLE)m_mtrPR2Posn[ACF1].y;
	}

	dx = 1.0 * (m_dmtrCurrPR2Posn.x - m_dmtrCurrPR1Posn.x);
	dy = 1.0 * (m_dmtrCurrPR2Posn.y - m_dmtrCurrPR1Posn.y);

	if (dx != 0)
	{
		dAngle = (180.0 / PI) * atan((dy / dx));
	}
	else
	{
		dAngle = 0;
	}

	dAngleRad = (PI / 180.0) * dAngle;
	dRefAngleRad = (PI / 180.0) * m_dGlassRefAng;

	// current align offset
	m_dmtrCurrAlign1Offset.x = (+m_stACF[m_lCurACF].dmtrAlignPt1.x * cos(dRefAngleRad - dAngleRad) 
								+ m_stACF[m_lCurACF].dmtrAlignPt1.y * sin(dRefAngleRad - dAngleRad) 
							   );

	m_dmtrCurrAlign1Offset.y = (-m_stACF[m_lCurACF].dmtrAlignPt1.x * sin(dRefAngleRad - dAngleRad) 
								+ m_stACF[m_lCurACF].dmtrAlignPt1.y * cos(dRefAngleRad - dAngleRad) 
							   );

	m_dmtrCurrAlign2Offset.x = (+m_stACF[m_lCurACF].dmtrAlignPt2.x * cos(dRefAngleRad - dAngleRad) 
								+ m_stACF[m_lCurACF].dmtrAlignPt2.y * sin(dRefAngleRad - dAngleRad) 
							   );

	m_dmtrCurrAlign2Offset.y = (-m_stACF[m_lCurACF].dmtrAlignPt2.x * sin(dRefAngleRad - dAngleRad) 
								+ m_stACF[m_lCurACF].dmtrAlignPt2.y * cos(dRefAngleRad - dAngleRad) 
							   );

	// current align posn
	dmtrCurrAlign1Posn.x = m_dmtrCurrPR1Posn.x + m_dmtrCurrAlign1Offset.x;
	dmtrCurrAlign1Posn.y = m_dmtrCurrPR1Posn.y + m_dmtrCurrAlign1Offset.y;
	dmtrCurrAlign2Posn.x = m_dmtrCurrPR2Posn.x + m_dmtrCurrAlign2Offset.x;
	dmtrCurrAlign2Posn.y = m_dmtrCurrPR2Posn.y + m_dmtrCurrAlign2Offset.y;

	// return align angle
#if 1 //20160826
	dx = 1.0 * (dmtrCurrAlign2Posn.x - dmtrCurrAlign1Posn.x);
	dy = 1.0 * (dmtrCurrAlign2Posn.y - dmtrCurrAlign1Posn.y);
	if (dx != 0)
	{
		m_dCurrAlignAngle = (180.0 / PI) * atan((dy / dx));
	}
	else
	{
		m_dCurrAlignAngle = 0.0;
	}
#else
	m_dCurrAlignAngle = dAngle;// - m_dGlassRefAng;
#endif

	// return align centre
	m_dmtrCurrAlignCentre.x = (dmtrCurrAlign1Posn.x + dmtrCurrAlign2Posn.x) / 2.0;
	m_dmtrCurrAlignCentre.y = (dmtrCurrAlign1Posn.y + dmtrCurrAlign2Posn.y) / 2.0;
	
	m_dCurrAlignDist = sqrt(pow((dmtrCurrAlign2Posn.x - dmtrCurrAlign1Posn.x), 2.0) + pow((dmtrCurrAlign2Posn.y - dmtrCurrAlign1Posn.y), 2.0));

	//if (HMI_bDebugCal)
	{
		// open debugging file
		CString str;
		MTR_POSN	mtrTemp;
		MTR_POSN	mtrAlignDelta;

		mtrTemp.x = m_mtrAlignCentre.x;
		mtrTemp.y = m_mtrAlignCentre.y;

		mtrTemp.x -= DoubleToLong(m_dmtrAlignRef.x);
		mtrTemp.y -= DoubleToLong(m_dmtrAlignRef.y);

		mtrAlignDelta.x = mtrTemp.x;
		mtrAlignDelta.y = mtrTemp.y;

		CString szTime = _T(""), szFileName = _T("");
		clock_t clkNow;

		clkNow = clock();
		SYSTEMTIME ltime;		// New Command

		// Get time as 64-bit integer.
		GetLocalTime(&ltime);
		szTime.Format("%u-%u-%u %u:%u:%u", 
			ltime.wYear, ltime.wMonth, ltime.wDay, 
			ltime.wHour, ltime.wMinute, ltime.wSecond);

		str.Format("D:\\sw\\AC9000\\Data\\Calculation Log\\ACF%ldWH_PRPt_%04u%02u%02u.log", m_lCurSlave + 1, ltime.wYear, ltime.wMonth, ltime.wDay);
		FILE *fp = fopen(str, "a+");


		if (fp != NULL)
		{	
			fseek(fp, 0, SEEK_END);
			if (ftell(fp) < 10)
			{
				fprintf(fp, "Time\tPR1x(Pix)\tPR1y(Pix)\tPR2x(Pix)\tPR2y(Pix)\tPR1x(Motor)\tPR1y(Motor)\tPR2x(Motor)\tPR2y(Motor)\tAlign1x\tAlign1y\tAlign2x\tAlign2y\tAlignAngle\tAlignDistance(mm)\n");
			}

			// PR1 PR2 Enc1 Enc2 APt1 APt2 Dist Ang
			fprintf(fp, "%s\t%.2f\t%.2f\t%.2f\t%.2f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\n", 
					(const char*)szTime,
					pruACF1.rcoDieCentre.x, pruACF1.rcoDieCentre.y, 
					pruACF2.rcoDieCentre.x, pruACF2.rcoDieCentre.y, 
					m_dmtrCurrPR1Posn.x, m_dmtrCurrPR1Posn.y, 
					m_dmtrCurrPR2Posn.x, m_dmtrCurrPR2Posn.y, 
					dmtrCurrAlign1Posn.x, dmtrCurrAlign1Posn.y, 
					dmtrCurrAlign2Posn.x, dmtrCurrAlign2Posn.y,
					m_dCurrAlignAngle,
					CountToDistance(m_dCurrAlignDist, pCInspOpt->m_stMotorX.stAttrib.dDistPerCount) / 1000.0
					/*mtrAlignDelta.x, mtrAlignDelta.y*/);

			fclose(fp);
		}
	}
}

VOID CACFWH::SetupACFOffset()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");

	BOOL		bResult 	= TRUE;
	CString		szAxis 		= "";
	LONG		lAnswer		= rMSG_TIMEOUT;

	CString		strMsg;

	if (g_ulWizardStep > 3 && g_ulWizardStep != 99)
	{
		g_ulWizardStep = 3;
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
				 m_stACF[m_lCurACF].dmtrAlignPt1.x, m_stACF[m_lCurACF].dmtrAlignPt1.y,
				 m_stACF[m_lCurACF].dmtrAlignPt2.x, m_stACF[m_lCurACF].dmtrAlignPt2.y);
		LogAction("AlignRef.x = %.5f\t AlignRef.y = %.5f\t AlignAng = %.5f", 
				  m_dmtrAlignRef.x, m_dmtrAlignRef.y,
				  m_dAlignRefAngle);

		break;

	case 0:
	// 1. Index Glass to ACFWH
	// 2. Search PR
		LogAction(__FUNCTION__);
		LogAction("AlignPt1.x = %.5f\t AlignPt1.y = %.5f\t AlignPt2.x = %.5f\t AlignPt2.y = %.5f\t", 
				 m_stACF[m_lCurACF].dmtrAlignPt1.x, m_stACF[m_lCurACF].dmtrAlignPt1.y,
				 m_stACF[m_lCurACF].dmtrAlignPt2.x, m_stACF[m_lCurACF].dmtrAlignPt2.y);
		LogAction("AlignRef.x = %.5f\t AlignRef.y = %.5f\t AlignAng = %.5f", 
				  m_dmtrAlignRef.x, m_dmtrAlignRef.y,
				  m_dAlignRefAngle);

		//if (IndexGlassToTable((GlassNum)m_lCurSlave) != GMP_SUCCESS)
		//{
		//	bResult = FALSE;
		//}
		//else
		{
			if (
				m_pCOtherACFWH->MoveXYToStandbyPosn(GMP_WAIT) != GMP_SUCCESS ||
				MoveXYToCurrPR1Posn(GMP_WAIT) != GMP_SUCCESS ||
				MoveT(ACF_WH_T_CTRL_GO_STANDBY, GMP_WAIT) != GMP_SUCCESS
			)
			{
				bResult = FALSE;
			}
			else
			{
				PRS_DisplayVideo(&pruACF_WHCalib);
				PRS_DrawHomeCursor(ACF_CAM, FALSE);

				g_szWizardStep = "Step 1/3";
				g_szWizardMessPrev = "";
				g_szWizardMess = "1. Bond ACF on Glass";	
				g_szWizardMessNext = "2. Locate ACF Lower Left Corner.";
				g_szWizardBMPath = "";
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
			//m_stACFDL.bAlignPtSet = FALSE;
			//m_stACFDL.dmtrAlignPt1.x = 0.0;
			//m_stACFDL.dmtrAlignPt1.y = 0.0;
			//m_stACFDL.dmtrAlignPt2.x = 0.0;
			//m_stACFDL.dmtrAlignPt2.y = 0.0;
			//
			//m_pCOtherACFWH->m_stACFDL.dmtrAlignPt1.x = 0.0;
			//m_pCOtherACFWH->m_stACFDL.dmtrAlignPt1.y = 0.0;
			//m_pCOtherACFWH->m_stACFDL.dmtrAlignPt2.x = 0.0;
			//m_pCOtherACFWH->m_stACFDL.dmtrAlignPt2.y = 0.0;

			m_dLoadAdjXOffset = 0.0;
			//m_dBondOffsetAdjY[m_lCurACF] = 0.0;
			for (INT i = ACF1; i < MAX_NUM_OF_ACF; i++)
			{
				m_dBondOffsetAdjY[i] = 0.0;
			}
			HMI_dBondOffsetAdjY = m_dBondOffsetAdjY[ACF1];
			 m_mtrBondOffset.x = 0;
			 m_mtrBondOffset.y = 0;
			 m_dBondOffsetT = 0.0;
			
		
			if ( 
				!ACFOffsetTestOperation(FALSE) /*||*/
				//MoveZToStandbyLevel(GLASS1, GMP_WAIT) != GMP_SUCCESS ||
				//MoveXYToPRPosn(GMP_WAIT) != GMP_SUCCESS //||
				//MoveX((ACFTA_X_CTRL_SIGNAL)(ACFTA_X_CTRL_GO_DL1_CALIB_POSN + m_lCurSlave), GMP_WAIT) != GMP_SUCCESS
				)
			{
				bResult = FALSE;
			}
			else
			{
				g_szWizardStep = "Step 2/3";
				g_szWizardMessPrev = "1. Bond ACF on Glass";
				g_szWizardMess = "2. Locate ACF Lower Left Corner.";
				g_szWizardMessNext = "3. Locate ACF Lower Right Corner.";
				g_szWizardBMPath = "";
			}
		}

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;
		break;

	case 2:
	// 2. Set Align Pick Pt 2 position
		m_mtrACFLLPoint.x = (pCInspOpt->m_stMotorX.GetEncPosn());		
		m_mtrACFLLPoint.y = (m_stMotorY.GetEncPosn());


		g_szWizardStep = "Step 3/3";
		g_szWizardMessPrev = "2. Locate ACF Lower Left Corner.";
		g_szWizardMess = "3. Locate ACF Lower Right Corner.";
		g_szWizardMessNext = "DONE";
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;
		break;

	case 3:
		m_mtrACFLRPoint.x = (pCInspOpt->m_stMotorX.GetEncPosn());		
		m_mtrACFLRPoint.y = (m_stMotorY.GetEncPosn());

		DOUBLE dx,dy;
		dx = m_mtrACFLRPoint.x - m_mtrACFLLPoint.x;
		dy = m_mtrACFLRPoint.y - m_mtrACFLLPoint.y;
		m_dACFRefWidth = m_stACF[ACF1].dWidth;
		m_dACFRefLength = m_stACF[ACF1].dLength;
		
		m_dBondOffsetT = (180.0 / PI) * atan((dy / dx))/* - CountToDistance((m_lTApproxBondPosn - m_lTStandbyPosn), m_stMotorT.stAttrib.dDistPerCount)*/;
		m_mtrACFCenterPoint.x = (m_mtrACFLRPoint.x + m_mtrACFLLPoint.x) / 2;
		m_mtrACFCenterPoint.y = (m_mtrACFLRPoint.y + m_mtrACFLLPoint.y) / 2;
		
		g_szWizardStep = "DONE";
		g_szWizardMess = "DONE";
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = FALSE;
		g_bWizardEnableExit = TRUE;
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

BOOL CACFWH::DoACFDL()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	BOOL bResult = TRUE;

	CString szMsg = "";
	MTR_POSN mtrTemp;
	mtrTemp.x = 0;
	mtrTemp.y = 0;

	PR_COORD coCentre;
	coCentre.x = PR_DEF_CENTRE_X;
	coCentre.y = PR_DEF_CENTRE_Y;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", szMsg);
		return FALSE;
	}

	if (!m_bPRSelected)
	{
		szMsg.Format("%s PR Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", szMsg);
		return FALSE;
	}

	if (!pruACF1.bLoaded || !pruACF2.bLoaded)
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "PR Pattern Not Loaded. Operation Abort!");
		return FALSE;
	}

	if (!m_stACF[m_lCurACF].bAlignPtSet)
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Align Pts Not Learned. Operation Abort!");	
		return FALSE;
	}

	m_dCurrAlignAngle = 0.0;

	//MoveXY(ACF_WH_XY_CTRL_GO_PR_POSN, GMP_WAIT);
	MoveTToStandby(GMP_WAIT);
	if (m_lCurSlave == ACFWH_2)
	{
		pruACF1.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF2WH;
		pruACF2.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF2WH;
	}
	else
	{
		pruACF1.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF1WH;
		pruACF2.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF1WH;
	}
	if (!AutoSearchPR1(&pruACF1))
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR1 Failed!");
		PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
		return FALSE;
	}

	if (!AutoSearchPR2(&pruACF2))
	{
		HMIMessageBox(MSG_OK, "SEARCH ALIGN PT1", "Search PR2 Failed!");
		PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
		return FALSE;
	}
	PRS_ClearScreen(ACF_CAM);
	MTR_POSN mtrAlignCenter;
	CalcAlignPt();
	DOUBLE dAngle = 1 * (m_dCurrAlignAngle - m_dBondOffsetT);
	m_lTOffset = DistanceToCount(dAngle, m_stMotorT.stAttrib.dDistPerCount);

	mtrAlignCenter.x = DoubleToLong(m_dmtrCurrAlignCentre.x);
	mtrAlignCenter.y = DoubleToLong(m_dmtrCurrAlignCentre.y);

	CalPosnAfterRotate(m_lCurEdge, mtrAlignCenter, -dAngle, &mtrTemp);

	m_mtrBondOffset.x = -1 * (m_mtrACFCenterPoint.x - mtrTemp.x);

	m_mtrBondOffset.y = -1 * (m_mtrACFCenterPoint.y - mtrTemp.y) + DistanceToCount((m_stACF[ACF1].dWidth / 2 * 1000), m_stMotorY.stAttrib.dDistPerCount); 

	szMsg.Format("Bond Posn X: %ld, Y: %ld, T: %ld\nBond T Offset: %.3f", (m_mtrApproxBondPosn.x + DistanceToCount(- m_dLoadAdjXOffset, pCInspOpt->m_stMotorX.stAttrib.dDistPerCount) + m_mtrBondOffset.x), 
		(m_mtrApproxBondPosn.y + DistanceToCount(- m_dBondOffsetAdjY[ACF1], m_stMotorY.stAttrib.dDistPerCount) + m_mtrBondOffset.y), (m_lStandbyPosnT[EDGE_A] + m_lTOffset), dAngle);
	LONG lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", szMsg);
	if (lAnswer == rMSG_CONTINUE)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}

VOID CACFWH::CalcBondOffset()
{
	MTR_POSN mtrAlignCenter, mtrTemp;
	mtrAlignCenter.x = 0;
	mtrAlignCenter.y = 0;
	mtrTemp.x = 0;
	mtrTemp.y = 0;
	CalcAlignPt();
	DOUBLE dAngle = 1 * (m_dCurrAlignAngle - m_dBondOffsetT);
	m_lTOffset = DistanceToCount(dAngle, m_stMotorT.stAttrib.dDistPerCount);

	mtrAlignCenter.x = DoubleToLong(m_dmtrCurrAlignCentre.x);
	mtrAlignCenter.y = DoubleToLong(m_dmtrCurrAlignCentre.y);

	CalPosnAfterRotate(m_lCurEdge, mtrAlignCenter, -dAngle, &mtrTemp);

	m_mtrBondOffset.x = -1 * (m_mtrACFCenterPoint.x - mtrTemp.x);

	m_mtrBondOffset.y = -1 * (m_mtrACFCenterPoint.y - mtrTemp.y) + DistanceToCount((m_stACF[ACF1].dWidth / 2 * 1000), m_stMotorY.stAttrib.dDistPerCount); 
}

LONG CACFWH::HMI_ToggleNotBondACF(IPC_CServiceMessage &svMsg)
{
	m_bNotBondACF = !m_bNotBondACF;
	m_pCOtherACFWH->m_bNotBondACF = m_bNotBondACF;
	return 0;
}

enum
{
	PR1_PR2_MODE = 0, 
	PR1_MODE, 
	PR2_MODE
};

LONG CACFWH::HMI_SearchAndLogPR(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");

	CString szMsg = "";
	CString szDataLogFileName = "";
	BOOL bResult = TRUE;
	BOOL bGoBackStby = FALSE; //20150811
	PR_COORD coCentre;
	CWinEagle *pCWinEagle = (CWinEagle*)m_pModule->GetStation("WinEagle");
	LONG lLoop = 0;
	const LONG lHeaterOnCount = 100, lLoopMax = 1000;
	LONG lPRMode = PR1_PR2_MODE, lAnswer = 0;
	MTR_POSN mtr_CamPR1, mtr_CamPR2;
	mtr_CamPR1.x = 0;
	mtr_CamPR1.y = 0;
	mtr_CamPR2.x = 0;
	mtr_CamPR2.y = 0;

	HMI_lLoopCount = 0;


	lAnswer = HMISelectionBox("ACFDL ALIGN PT LOG", "Select PR Position", "PR1 and PR2", "PR1", "PR2");
	if (lAnswer == -1 || lAnswer == 11)
	{
		HMIMessageBox(MSG_OK, "COF ALIGN DISTANCE LOG", "Operation Abort!");
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (lAnswer == 0)
	{
		lPRMode = PR1_PR2_MODE;
	}
	else if (lAnswer == 1)
	{
		lPRMode = PR1_MODE;
	}
	else
	{
		lPRMode = PR2_MODE;
	}


	if (rMSG_YES == HMIMessageBox(MSG_YES_NO, "ACFDL ALIGN PT LOG", "Go Back To Standby Every Loop?"))
	{
		bGoBackStby = TRUE;
	}
	else
	{
		bGoBackStby = FALSE;
	}
	//SetVacSol(GLASS1, ON, GMP_WAIT);
	//if (!IsVacSensorOn(GLASS1))
	//{
	//	if (rMSG_NO == HMIMessageBox(MSG_YES_NO, "DEBUG LOGGING", "Index Glass to WH?"))
	//		{
	//			HMIMessageBox(MSG_OK, "DEBUG LOGGING", "Operation Abort");
	//			svMsg.InitMessage(sizeof(BOOL), &bResult);
	//			return 1;
	//		}
	//	if (IndexGlassToTable((GlassNum)GLASS1) != GMP_SUCCESS)
	//	{
	//		HMIMessageBox(MSG_OK, "DEBUG LOGGING", "Pick FPC Error. Operation Abort");
	//		svMsg.InitMessage(sizeof(BOOL), &bResult);
	//		return 1;
	//	}
	//}


	PRS_DisplayVideo(&pruACF_WHCalib);
	PRS_DrawHomeCursor(ACF_CAM, FALSE);
	pCWinEagle->UpdateLighting(&pruACF_WHCalib);

	coCentre.x = PR_DEF_CENTRE_X;
	coCentre.y = PR_DEF_CENTRE_Y;

	if (
		MoveT(ACF_WH_T_CTRL_GO_STANDBY, SFM_WAIT) == GMP_SUCCESS 
		)
	{
		if (!m_bPRSelected)
		{
			szMsg.Format("%s PR Module Not Selected. Operation Abort!", GetStnName());
			HMIMessageBox(MSG_OK, "WARNING", szMsg);
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return 1;
		}

		if (!m_stACF[m_lCurACF].bEnable)
		{
			szMsg.Format("Current ACF Not Enabled. Operation Abort!");
			HMIMessageBox(MSG_OK, "WARNING", szMsg);
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return 1;
		}

		if (!pruACF1.bLoaded || !pruACF2.bLoaded)
		{
			szMsg.Format("%s PR Pattern Not Loaded. Operation Abort!", GetStnName());
			HMIMessageBox(MSG_OK, "WARNING", szMsg);
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return 1;
		}
		if (!m_stACF[m_lCurACF].bAlignPtSet || getKlocworkFalse()) //klocwork fix 20121211
		{
			szMsg.Format("%s COF ACFDL Align Pts Not Learned. Operation Abort!", (const char*)GetStnName());
			HMIMessageBox(MSG_OK, "WARNING", szMsg);
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return 1;
		}


		pAppMod->m_clkBondStartTime = clock();
		pCHouseKeeping->m_bPRLoopTest = TRUE;

		if (lPRMode == PR1_MODE)
		{
			
			if (
				(MoveXYToCurrPR1Posn(GMP_NOWAIT)) != GMP_SUCCESS || 
				(pCInspOpt->SyncX()) != GMP_SUCCESS || 
				(SyncY()) != GMP_SUCCESS
				
				)
			{
				HMIMessageBox(MSG_OK, "WARNING", "Move PR1 Posn Failed!");
				svMsg.InitMessage(sizeof(BOOL), &bResult);
				return 1;
			}
			Sleep(m_lPRDelay);
			
		}
		else if (lPRMode == PR2_MODE)
		{
			if (
				(MoveXYToCurrPR2Posn(GMP_NOWAIT)) != GMP_SUCCESS || 
				(pCInspOpt->SyncX()) != GMP_SUCCESS || 
				(SyncY()) != GMP_SUCCESS
				//pCPreBondWH->MoveXYFollowUplook(m_lCurSlave) != GMP_SUCCESS
				)
			{
				HMIMessageBox(MSG_OK, "WARNING", "Move PR2 Posn Failed!");
				svMsg.InitMessage(sizeof(BOOL), &bResult);
				return 1;
			}
			Sleep(m_lPRDelay);
		}

		for (lLoop = 0; lLoop < lLoopMax && pCHouseKeeping->m_bPRLoopTest; lLoop++, HMI_lLoopCount++)
		{
			if (m_lCurSlave == ACFWH_1)
			{
				SetHmiVariable("ACF1WH_lLoopCount");
			}
			else
			{
				SetHmiVariable("ACF2WH_lLoopCount");
			}

			m_dAlignAngle	= 0.0;
#if 1
			MTR_POSN mtr_offset;
			mtr_offset.x = 0;
			mtr_offset.y = 0;

			if (lPRMode == PR1_PR2_MODE)
			{

				if (
					(MoveXYToCurrPR1Posn(GMP_NOWAIT)) != GMP_SUCCESS || 
					(pCInspOpt->SyncX()) != GMP_SUCCESS || 
					(SyncY()) != GMP_SUCCESS
					)
				{
					HMIMessageBox(MSG_OK, "WARNING", "Move PR1 Posn Failed!");
					svMsg.InitMessage(sizeof(BOOL), &bResult);
					return 1;
				}
				Sleep(m_lPRDelay);
			}
			mtr_CamPR1.x = pCInspOpt->m_stMotorX.GetEncPosn();
			mtr_CamPR1.y = m_stMotorY.GetEncPosn();

			if (lPRMode != PR2_MODE)
			{
				if (!SearchPR(&pruACF1, FALSE))
				{
					HMIMessageBox(MSG_OK, "WARNING", "PR1 Search Failed!");
					PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
					svMsg.InitMessage(sizeof(BOOL), &bResult);
					return 1;
				}

				//Test

				PRS_PRPosToMtrOffset(&pruACF1, pruACF1.rcoDieCentre, &mtr_offset);

				m_dmtrCurrPR1Posn.x += (DOUBLE)mtr_offset.x;
				m_dmtrCurrPR1Posn.y += (DOUBLE)mtr_offset.y;
			}
#else
			if (!AutoSearchPR1(&pruACF1))
			{
				HMIMessageBox(MSG_OK, "WARNING", "Search PR1 Failed!");
				PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
				svMsg.InitMessage(sizeof(BOOL), &bResult);
				return 1;
			}
#endif


#if 1
			if (lPRMode == PR1_PR2_MODE)
			{
				if (
					(MoveXYToCurrPR2Posn(GMP_NOWAIT)) != GMP_SUCCESS || 
					(pCInspOpt->SyncX()) != GMP_SUCCESS || 
					(SyncY()) != GMP_SUCCESS
					)
				{
					HMIMessageBox(MSG_OK, "WARNING", "Move PR2 Posn Failed!");
					PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
					svMsg.InitMessage(sizeof(BOOL), &bResult);
					return 1;
				}

				Sleep(m_lPRDelay);
			}

			mtr_CamPR2.x = pCInspOpt->m_stMotorX.GetEncPosn();
			mtr_CamPR2.y = m_stMotorY.GetEncPosn();

			if (lPRMode != PR1_MODE)
			{
				if (!SearchPR(&pruACF2, FALSE))
				{
					HMIMessageBox(MSG_OK, "WARNING", "PR2 Search Failed!");
					PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
					svMsg.InitMessage(sizeof(BOOL), &bResult);
					return 1;
				}


				//Test

				PRS_PRPosToMtrOffset(&pruACF2, pruACF2.rcoDieCentre, &mtr_offset);

				m_dmtrCurrPR2Posn.x += (DOUBLE)mtr_offset.x;
				m_dmtrCurrPR2Posn.y += (DOUBLE)mtr_offset.y;
			}
#else
			if (!AutoSearchPR2(&pruACF2))
			{
				HMIMessageBox(MSG_OK, "WARNING", "Search PR2 Failed!");
				PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Failed!");
				svMsg.InitMessage(sizeof(BOOL), &bResult);
				return 1;
			}
#endif
			PRS_ClearScreen(ACF_CAM);

			CalcAlignPt();

			PRS_DrawCross(ACF_CAM, coCentre, PRS_XLARGE_SIZE, PR_COLOR_RED);
			PRS_DisplayText(ACF_CAM, 1, PRS_MSG_ROW1, "Search Succeed!");

			
				//LogLSIinfo(CString("LSI_Expansion"), lLoop, mtr_CamPR1, mtr_CamPR2, dDistance, m_dCurrAlignAngle, dTemperature);
#if 1 //20150811
			if (bGoBackStby)
			{
				if (
					pCInspOpt->MoveX(INSPOPT_X_CTRL_GO_STANDBY_POSN, SFM_WAIT) != gnAMS_OK ||
					//MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, SFM_WAIT) != gnAMS_OK ||
					MoveT(ACF_WH_T_CTRL_GO_STANDBY, SFM_WAIT) != gnAMS_OK /*||
					MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT) != gnAMS_OK */
				)
				{
					HMIMessageBox(MSG_OK, "WARNING", "Move Standby Posn/level Failed! Abort!");
					svMsg.InitMessage(sizeof(BOOL), &bResult);
					return 1;
				}

				if (lPRMode == PR1_MODE)
				{
					if (
						(MoveXYToCurrPR1Posn(GMP_NOWAIT)) != GMP_SUCCESS || 
						(pCInspOpt->SyncX()) != GMP_SUCCESS || 
						(SyncY()) != GMP_SUCCESS
						)
					{
						HMIMessageBox(MSG_OK, "WARNING", "Move PR1 Posn Failed!");
						svMsg.InitMessage(sizeof(BOOL), &bResult);
						return 1;
					}
					Sleep(m_lPRDelay);
				}
				else if (lPRMode == PR2_MODE)
				{
					if (
						(MoveXYToCurrPR2Posn(GMP_NOWAIT)) != GMP_SUCCESS || 
						(pCInspOpt->SyncX()) != GMP_SUCCESS || 
						(SyncY()) != GMP_SUCCESS
						)
					{
						HMIMessageBox(MSG_OK, "WARNING", "Move PR2 Posn Failed!");
						svMsg.InitMessage(sizeof(BOOL), &bResult);
						return 1;
					}
					Sleep(m_lPRDelay);
				}
			}
#endif

		}
	}

	BOOL bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

BOOL CACFWH::IsRejectLimitHit()
{
	if (m_lRejectGlassLimit != 0)
	{
		if (m_lRejectGlassCounter >= m_lRejectGlassLimit)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}

	}
	else
	{
		return FALSE;
	}
}

VOID CACFWH::GetAndLogXYPosn(ACF_WH_XY_CTRL_SIGNAL lCtrl)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	MTR_POSN TempPosn;
	//LONG	lTempLevel;
	LONG	lTempTPosn;
	//lTempLevel = m_stMotorZ[WHZ_1].GetEncPosn();
	lTempTPosn = m_stMotorT.GetEncPosn();
	TempPosn.x = pCInspOpt->m_stMotorX.GetEncPosn();
	TempPosn.y = m_stMotorY.GetEncPosn();
	CString str = "";	
	str.Format("D:\\sw\\AC9000\\Data\\ACF%ldWH_Encoder.log", m_lCurSlave + 1);
	FILE *fp = fopen(str, "a+");

	if (fp != NULL)
	{
		CString szTime = _T(""), szFileName = _T("");
		clock_t clkNow;

		clkNow = clock();
		SYSTEMTIME ltime;		// New Command

		// Get time as 64-bit integer.
		GetLocalTime(&ltime);
		szTime.Format("%u-%u-%u %u:%u:%u", 
			ltime.wYear, ltime.wMonth, ltime.wDay, 
			ltime.wHour, ltime.wMinute, ltime.wSecond);

		fseek(fp, 0, SEEK_END);
		if (ftell(fp) < 10)
		{
			fprintf(fp, "Time\tCtrl\tEnc X\tEnc Y\tEnc Z\tEnc T\n");
		}

		fprintf(fp, "%s\t%ld\t%ld\t%ld\t%ld\t%ld\n", 
			(const char*)szTime,
			(LONG)lCtrl,
			(LONG)TempPosn.x,
			(LONG)TempPosn.y,
			//(LONG)lTempLevel,
			(LONG)lTempTPosn
			);

		fclose(fp);
	}
}

VOID CACFWH::CalcBondPt(ACFNum nACF)
{
	D_MTR_POSN dmtrCurrAlign1Posn;
	D_MTR_POSN dmtrCurrAlign2Posn;

	DOUBLE dx, dy;
	DOUBLE dAngle, dAngleRad, dRefAngleRad/*, dPostBondRefAngleRad*/;

	// if PR not selected, return approx bond posn
	if (!m_bPRSelected || !m_stACF[nACF].bAlignPtSet || !m_stACF[nACF].bEnable || !m_stACF[nACF].bAlignPtSet)
	{
		m_dmtrCurrPR1Posn.x = (DOUBLE)m_mtrPR1Posn[ACF1].x;
		m_dmtrCurrPR1Posn.y = (DOUBLE)m_mtrPR1Posn[ACF1].y;
		m_dmtrCurrPR2Posn.x = (DOUBLE)m_mtrPR2Posn[ACF1].x;
		m_dmtrCurrPR2Posn.y = (DOUBLE)m_mtrPR2Posn[ACF1].y;
	}

	dx = 1.0 * (m_dmtrCurrPR2Posn.x - m_dmtrCurrPR1Posn.x);
	dy = -1.0 * (m_dmtrCurrPR2Posn.y - m_dmtrCurrPR1Posn.y);

	if (dx != 0.0)
	{
		dAngle = (180.0 / PI) * atan((dy / dx));
	}
	else
	{
		dAngle = 0.0;
	}
	dAngleRad = (PI / 180.0) * dAngle;
	dRefAngleRad = (PI / 180.0) * m_dGlassRefAng;
	//dPostBondRefAngleRad = (PI / 180.0) * m_dGlassRefAng[m_stACF[nACF].lEdgeInfo];

	// current align offset
	m_dmtrCurrAlign1Offset.x = (+m_stACF[nACF].dmtrAlignPt1.x * cos(dRefAngleRad - dAngleRad) 
								- m_stACF[nACF].dmtrAlignPt1.y * sin(dRefAngleRad - dAngleRad) 
							   );

	m_dmtrCurrAlign1Offset.y = (+m_stACF[nACF].dmtrAlignPt1.x * sin(dRefAngleRad - dAngleRad) 
								+ m_stACF[nACF].dmtrAlignPt1.y * cos(dRefAngleRad - dAngleRad) 
							   );

	m_dmtrCurrAlign2Offset.x = (+m_stACF[nACF].dmtrAlignPt2.x * cos(dRefAngleRad - dAngleRad) 
								- m_stACF[nACF].dmtrAlignPt2.y * sin(dRefAngleRad - dAngleRad) 
							   );

	m_dmtrCurrAlign2Offset.y = (+m_stACF[nACF].dmtrAlignPt2.x * sin(dRefAngleRad - dAngleRad) 
								+ m_stACF[nACF].dmtrAlignPt2.y * cos(dRefAngleRad - dAngleRad) 
							   );

	// current align posn
	dmtrCurrAlign1Posn.x = m_dmtrCurrPR1Posn.x + m_dmtrCurrAlign1Offset.x;
	dmtrCurrAlign1Posn.y = m_dmtrCurrPR1Posn.y + m_dmtrCurrAlign1Offset.y;
	dmtrCurrAlign2Posn.x = m_dmtrCurrPR2Posn.x + m_dmtrCurrAlign2Offset.x;
	dmtrCurrAlign2Posn.y = m_dmtrCurrPR2Posn.y + m_dmtrCurrAlign2Offset.y;

	//// current PostBond posn
	//m_dmtrCurrPBPR1Posn.x = m_dmtrCurrPR1Posn[m_stACF[nACF].lEdgeInfo].x + (



	//							+m_stACF[nACF].dmtrPostBondPt1.x * cos(dPostBondRefAngleRad - dAngleRad) 
	//							- m_stACF[nACF].dmtrPostBondPt1.y * sin(dPostBondRefAngleRad - dAngleRad) 
	//						);
	//m_dmtrCurrPBPR1Posn.y = m_dmtrCurrPR1Posn[m_stACF[nACF].lEdgeInfo].y + (



	//							+m_stACF[nACF].dmtrPostBondPt1.x * sin(dPostBondRefAngleRad - dAngleRad) 
	//							+ m_stACF[nACF].dmtrPostBondPt1.y * cos(dPostBondRefAngleRad - dAngleRad) 
	//						);
	//m_dmtrCurrPBPR2Posn.x = m_dmtrCurrPR2Posn[m_stACF[nACF].lEdgeInfo].x + (



	//							+m_stACF[nACF].dmtrPostBondPt2.x * cos(dPostBondRefAngleRad - dAngleRad) 
	//							- m_stACF[nACF].dmtrPostBondPt2.y * sin(dPostBondRefAngleRad - dAngleRad) 
	//						);
	//m_dmtrCurrPBPR2Posn.y = m_dmtrCurrPR2Posn[m_stACF[nACF].lEdgeInfo].y + (



	//							+m_stACF[nACF].dmtrPostBondPt2.x * sin(dPostBondRefAngleRad - dAngleRad) 
	//							+ m_stACF[nACF].dmtrPostBondPt2.y * cos(dPostBondRefAngleRad - dAngleRad) 
	//						);

	// return align angle
	m_dCurrAlignAngle = dAngle - m_dGlassRefAng;

	// return align centre
	m_dmtrCurrAlignCentre.x = (dmtrCurrAlign1Posn.x + dmtrCurrAlign2Posn.x) / 2.0;
	m_dmtrCurrAlignCentre.y = (dmtrCurrAlign1Posn.y + dmtrCurrAlign2Posn.y) / 2.0;

	// return align distance
	m_dCurrAlignDist = sqrt(pow((dmtrCurrAlign2Posn.x - dmtrCurrAlign1Posn.x), 2.0) + pow((dmtrCurrAlign2Posn.y - dmtrCurrAlign1Posn.y), 2.0));
}

LONG CACFWH::HMI_DisplayVideo(IPC_CServiceMessage &svMsg)
{
	CWinEagle *pCWinEagle = (CWinEagle*)m_pModule->GetStation("WinEagle");

	if (m_lCurSlave <= ACFWH_NONE || m_lCurSlave >= MAX_NUM_OF_ACFWH)
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		return 0;
	}

	m_pCOtherACFWH->MoveXYToStandbyPosn(GMP_WAIT);

	PRS_DisplayVideo(&pruACF_WHCalib);
	PRS_DrawHomeCursor(ACF_CAM, FALSE);
	pCWinEagle->UpdateLighting(&pruACF_WHCalib);

	return 0;
}

VOID CACFWH::SetupLoadPosnOffset()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	CInPickArm *pCInPickArm = (CInPickArm*)m_pModule->GetStation("InPickArm");
	BOOL		bResult 	= TRUE;
	CString		szAxis 		= "";
	LONG		lAnswer		= rMSG_TIMEOUT;
	static LONG	lGlass;
	CString		szTitle1, szTitle2, szTitle3;
	MTR_POSN mtrGlassAlignPt2;
	static BOOL	bMovePrevPosn = FALSE;
	
	DOUBLE dx,dy;

	CString		strMsg;

	if (g_ulWizardStep > 6 && g_ulWizardStep != 99)
	{
		g_ulWizardStep = 6;
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

		LogAction(__FUNCTION__); 


		break;

	case 0:
		LogAction(__FUNCTION__);

		lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Operation is irreversible. Continue?");
		if (lAnswer == rMSG_TIMEOUT || lAnswer == rMSG_CANCEL)
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

			if (
				m_pCACF->MoveZ(ACF_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS	||
				MoveT(ACF_WH_T_CTRL_GO_STANDBY, GMP_WAIT) != GMP_SUCCESS	||
				MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS		||
				pCInspOpt->MoveX(INSPOPT_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS
				)
			{
				bResult = FALSE;
			}
			else
			{
				if (bMovePrevPosn)
				{
					if (
						m_stMotorY.MoveAbsolute(m_mtrACFAnvilLLPosn.y, GMP_WAIT) != GMP_SUCCESS ||
						pCInspOpt->m_stMotorX.MoveAbsolute(m_mtrACFAnvilLLPosn.x, GMP_WAIT) != GMP_SUCCESS
						)
					{
						bResult = FALSE;
					}
				}
				if (bResult)
				{
					PRS_DisplayVideo(&pruACF_WHCalib);
					PRS_DrawHomeCursor(ACF_CAM, FALSE);

					g_szWizardStep = "Step 1/6";
					g_szWizardMessPrev = "";
					g_szWizardMess.Format("1. Locate ACF%ldWH Anvil Block Upper-Left Corner", m_lCurSlave + 1) ;	
					g_szWizardMessNext.Format("2. Locate ACF%ldWH Anvil Block Upper-Right Corner", m_lCurSlave + 1);
					g_szWizardBMPath = "";
				}
			}
		}
		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;
		break;

	case 1:

			m_mtrACFAnvilLLPosn.x = (pCInspOpt->m_stMotorX.GetEncPosn());	
			m_mtrACFAnvilLLPosn.y = (m_stMotorY.GetEncPosn());
		
			m_bACFPlacePosnSetup = FALSE;
			m_dAnvilBlockAngOffset = 0.0;
			
			m_lLoadXOffset = 0;
			m_lLoadYOffset = 0;
			m_lLoadTOffset = 0;
			m_mtrLoadGlassOffset.x = 0;
			m_mtrLoadGlassOffset.y = 0;
			if(m_lCurSlave == ACFWH_1)
			{
				pCTA1->m_lPlaceGlass1Offset = 0;
			}
			else
			{
				pCTA1->m_lPlaceGlass2Offset = 0;
			}

			if (bMovePrevPosn)
			{
				if (
					m_stMotorY.MoveAbsolute(m_mtrACFAnvilLRPosn.y, GMP_WAIT) != GMP_SUCCESS ||
					pCInspOpt->m_stMotorX.MoveAbsolute(m_mtrACFAnvilLRPosn.x, GMP_WAIT) != GMP_SUCCESS
					)
				{
					bResult = FALSE;
				}
			}
			if (bResult)
			{
				g_szWizardStep = "Step 2/6";
				g_szWizardMessPrev.Format("1. Locate ACF%ldWH Anvil Block Upper-Left Corner", m_lCurSlave + 1);
				g_szWizardMess.Format("2. Locate ACF%ldWH Anvil Block Upper-Right Corner", m_lCurSlave + 1);
				g_szWizardMessNext.Format("3. Index Glass%ld to TA1 from INPA. Search Align Pt", m_lCurSlave + 1);
				g_szWizardBMPath = "";
			}
		
		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;
		break;

	case 2:
		m_mtrACFAnvilLRPosn.x = (pCInspOpt->m_stMotorX.GetEncPosn());	
		m_mtrACFAnvilLRPosn.y = (m_stMotorY.GetEncPosn());

		dx = m_mtrACFAnvilLRPosn.x - m_mtrACFAnvilLLPosn.x;
		dy = m_mtrACFAnvilLRPosn.y - m_mtrACFAnvilLLPosn.y;
		m_dAnvilBlockAngOffset = (180.0 / PI) * atan((dy / dx));

		g_szWizardStep = "Step 3/6";
		g_szWizardMessPrev.Format("2. Locate ACF%ldWH Upper-Right Corner", m_lCurSlave + 1);
		g_szWizardMess.Format("3. Index Glass%ld to TA1 from INPA.", m_lCurSlave + 1);
		g_szWizardMessNext.Format("4. Place Glass to ACF%ldWH.", m_lCurSlave + 1);
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;
		break;


	case 3:
		//if (
		//	!pCInPickArm->PlaceGlassOperation(NO_GLASS)
		//	)
		//{
		//	bResult = FALSE;
		//}
		//else
		{
			PRS_DisplayVideo(&pruACF_WHCalib);
			PRS_DrawHomeCursor(ACF_CAM, FALSE);
			g_szWizardStep = "Step 4/6";
			g_szWizardMessPrev.Format("3. Glass%ld will be to ACF%ldWH from TA1. Search Align Pt", m_lCurSlave + 1);
			g_szWizardMess.Format("4. Place Glass to ACF%ldWH.", m_lCurSlave + 1);
			g_szWizardMessNext = ("5. Locate Align Pt1 Position (Glass/COF Upper-Left Corner)");
			g_szWizardBMPath = "";
		}


		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;
		break;

	case 4:
		//m_bPRSelected = FALSE;
		g_bUplookPREnable = FALSE;
		if (
			!pCTA1->PlaceGlassOperation((ACFWHUnitNum)m_lCurSlave)
			)
		{
			bResult = FALSE;
		}
		else
		{
			if (bMovePrevPosn)
			{
				if (
					m_stMotorY.MoveAbsolute(m_mtrAlignRefPosnOnWH.y, GMP_WAIT) != GMP_SUCCESS ||
					pCInspOpt->m_stMotorX.MoveAbsolute(m_mtrAlignRefPosnOnWH.x, GMP_WAIT) != GMP_SUCCESS
					)
				{
					bResult = FALSE;
				}
			}
			if (bResult)
			{
				g_szWizardStep = "Step 5/6";
				g_szWizardMessPrev.Format("4. Place Glass to ACF%ldWH.", m_lCurSlave + 1);
				g_szWizardMess = ("5. Locate Align Pt1 Position (Glass/COF Upper-Left Corner)");
				g_szWizardMessNext = ("6. Locate Align Pt2 Position (Glass/COF Upper-Right Corner)");
				g_szWizardBMPath = "";
			}
		}
		g_bUplookPREnable = TRUE;


		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;
		break;

	case 5:
		// Set	Align Pt1 Position On WH
		m_mtrAlignRefPosnOnWH.x = (pCInspOpt->m_stMotorX.GetEncPosn());	
		m_mtrAlignRefPosnOnWH.y = (m_stMotorY.GetEncPosn());

		g_szWizardStep = "Step 6/6";
		g_szWizardMessPrev = ("5. Locate Align Pt1 Position (Glass/COF Upper-Left Corner)");
		g_szWizardMess = ("6. Locate Align Pt2 Position (Glass/COF Upper-Right Corner)");
		g_szWizardMessNext = ("DONE");
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;
		break;
	
	case 6:
		// Set	Align Pt2 Position On WH
		mtrGlassAlignPt2.x = (pCInspOpt->m_stMotorX.GetEncPosn());	
		mtrGlassAlignPt2.y = (m_stMotorY.GetEncPosn());

		//MTR_POSN mtrTemp0, mtrTemp1;
		DOUBLE dAlignAngle = 0.0;
		dx = mtrGlassAlignPt2.x - m_mtrAlignRefPosnOnWH.x;
		dy = mtrGlassAlignPt2.y - m_mtrAlignRefPosnOnWH.y;

		m_mtrAlignRefPosnOnWH.x = (mtrGlassAlignPt2.x + m_mtrAlignRefPosnOnWH.x)/2;
		m_mtrAlignRefPosnOnWH.y = (mtrGlassAlignPt2.y + m_mtrAlignRefPosnOnWH.y)/2;

		dAlignAngle = (180.0 / PI) * atan((dy / dx));

		m_dACFRefWidth	= m_stACF[ACF1].dWidth;
		m_dACFRefLength = m_stACF[ACF1].dLength;
		
		m_dAnvilBlockAngOffset = m_dAnvilBlockAngOffset - dAlignAngle;
		
		//pCACFWH->CalPosnAfterRotate(mtrGlassAlignPt1, -m_dAnvilBlockAngOffset[lGlass], &mtrTemp0);
		//pCACFWH->CalPosnAfterRotate(m_mtrACFAnvilLRPosn[lGlass], -m_dAnvilBlockAngOffset[lGlass], &mtrTemp1);
		//m_mtrRefPlacePosnOffset[lGlass].x = mtrTemp1.x - mtrTemp0.x;
		//m_mtrRefPlacePosnOffset[lGlass].y = mtrTemp1.y - mtrTemp0.y;
		m_bACFPlacePosnSetup = TRUE;
		
		UpDateACFTAPlaceACFWHPosn();

		g_szWizardStep = "DONE";
		g_szWizardMess = "DONE";
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = FALSE;
		g_bWizardEnableExit = TRUE;
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

VOID CACFWH::UpDateACFTAPlaceACFWHPosn()
{
	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	MTR_POSN mtrTemp0, mtrTemp1, mtrGlassAlignPt1;
	DOUBLE dAngle = 0.0;

	mtrGlassAlignPt1.x = m_mtrAlignRefPosnOnWH.x;
	mtrGlassAlignPt1.y = m_mtrAlignRefPosnOnWH.y;

	dAngle = 1 * m_dAnvilBlockAngOffset;

	m_lLoadTOffset = DistanceToCount(dAngle, m_stMotorT.stAttrib.dDistPerCount);
	
	mtrTemp0.x = (m_mtrACFAnvilLLPosn.x + m_mtrACFAnvilLRPosn.x)/2 - DistanceToCount((DOUBLE)m_lLoadXOffset, m_stMotorX.stAttrib.dDistPerCount);
	mtrTemp0.y = (m_mtrACFAnvilLLPosn.y + m_mtrACFAnvilLRPosn.y)/2 + DistanceToCount((DOUBLE)m_lLoadYOffset, m_stMotorY.stAttrib.dDistPerCount);

	CalPosnAfterRotate(EDGE_A, mtrTemp0, -dAngle, &mtrTemp1);
	if(m_lCurSlave == ACFWH_1)
	{
		pCTA1->m_lPlaceGlass1Offset = mtrTemp1.x - mtrGlassAlignPt1.x;//mtrTemp0.x;
	}
	else
	{
		pCTA1->m_lPlaceGlass2Offset = mtrTemp1.x - mtrGlassAlignPt1.x;//mtrTemp0.x;
	}
	m_mtrLoadGlassOffset.y = mtrTemp1.y - mtrGlassAlignPt1.y;//mtrTemp0.y;
}

//LONG CACFWH::HMI_SetCurrentSetupGlass(IPC_CServiceMessage &svMsg) //20150302
//{
//	BOOL bResult = TRUE;
//	LONG lValue = 0;
//	svMsg.GetMsg(sizeof(LONG), &lValue);
//
//	if (	lValue != (LONG) GLASS1/* && lValue != (LONG) GLASS2*/)
//	{
//		HMIMessageBox(MSG_OK, "SET CURRENT SETUP GLASS", "Input Value Invalid.");
//	}
//	else
//	{
//		m_lCurSetupGlass = (GlassNum) lValue;
//	}
//
//	UpdateHMIPRButton();
//
//	svMsg.InitMessage(sizeof(BOOL), &bResult);
//    return 1;
//}

LONG CACFWH::HMI_SrchPBPR(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	pPRU = &SelectCurPBPRU(HMI_ulCurPBPRU);

	PRS_DisplayVideo(pPRU);

	if(pPRU->emRecordType == PRS_ACF_TYPE)
	{
		pPRU->stSrchACFCmd.emDisplayInfo				= PR_ACF_DISPLAY_CORNERS_AND_REGIONS;
		SrchACFCmd(pPRU);
		pPRU->stSrchACFCmd.emDisplayInfo				= PR_ACF_DISPLAY_NONE;
	}
	else
	{
		SrchPR_New(pPRU);	// Vision UI
	}

	UpdateHMIPBPRButton();

	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

//VOID CACFWH::UpdateHMIPBPRButton()
//{
//	PRU *pPRU;
//
//	//Normal PR
//	pPRU = &SelectCurPBPRU(HMI_ulCurPBPRU);
//	HMI_lPRSearchRange = g_lSearchRange;	//20150115 pPRU->stExtShapeCmd.uwSrchRange;
//
//	if(pPRU->emRecordType == PRS_ACF_TYPE)
//	{
//		HMI_ulPRID = pPRU->stSrchACFCmd.uwRecordID;
//		HMI_dPRMatchScore = pPRU->stSrchACFCmd.rMatchScore; //20141217 * 100 ;
//		HMI_dPRAngleRange = pPRU->stSrchACFCmd.rAngleRange;
//		HMI_dPRPercentVar = 5.0;
//		HMI_lDefectThreshold = pPRU->stSrchACFCmd.stACFInsp.ubDefectThreshold;
//		HMI_dPostBdRejectTol = pPRU->dPostBdRejectTol;
//	}
//	else
//	{
//		HMI_ulPRID = pPRU->stSrchCmd.uwRecordID;
//	}
//
//	HMI_bShowPRSearch = TRUE; //pPRU->bLoaded;
//}

VOID CACFWH::GetMouseWindow(PRU *pPRU, PR_REGION *Region)
{

	PR_COORD	coUpperLeft;
	PR_COORD	coLowerRight;

	PRS_GetMouseWindow(pPRU, &coUpperLeft, &coLowerRight);
	PRS_CancelMouseWindow(pPRU);
	
	Region->acoCorners[PR_UPPER_RIGHT].x	= coLowerRight.x;
	Region->acoCorners[PR_UPPER_RIGHT].y	= coUpperLeft.y;
	Region->acoCorners[PR_UPPER_LEFT].x		= coUpperLeft.x;
	Region->acoCorners[PR_UPPER_LEFT].y		= coUpperLeft.y;
	Region->acoCorners[PR_LOWER_LEFT].x		= coUpperLeft.x;
	Region->acoCorners[PR_LOWER_LEFT].y		= coLowerRight.y;
	Region->acoCorners[PR_LOWER_RIGHT].x	= coLowerRight.x;
	Region->acoCorners[PR_LOWER_RIGHT].y	= coLowerRight.y;

}
VOID CACFWH::DrawMouseWindow(PRU *pPRU, PR_REGION Region)
{
	PR_COORD	coUpperLeft;
	PR_COORD	coLowerRight;

	coUpperLeft.x	= Region.acoCorners[PR_UPPER_LEFT].x;
	coUpperLeft.y	= Region.acoCorners[PR_UPPER_LEFT].y;
	coLowerRight.x	= Region.acoCorners[PR_LOWER_RIGHT].x;
	coLowerRight.y	= Region.acoCorners[PR_LOWER_RIGHT].y;

	PRS_CancelMouseWindow(pPRU);
	PRS_UseMouseWindow(pPRU, coUpperLeft, coLowerRight);
	PRS_DrawHomeCursor(ACF_CAM, FALSE);

}

VOID CACFWH::SetupACFPattern()
{
	// Step 0: Select Method;
	// Step 1: Index glass to acfwh, and learn Purpose 0
	// Step 2: Learn the purpose 1 light
	// Step 3: Remove the Glass
	// Step 4: Inex glass
	// Step 4: Learn the Background and remove Glass
	// Step 5: Learn the ACF Region
	// Step 6: Learn the Search Region
	// Step 7: Set Segment Threshold;
	// Step 8: Learn the Defect Region
	// Step 9: Learn the Chip Region
	// Step 10: Learn

	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CSettingFile *pCSettingFile = (CSettingFile*)m_pModule->GetStation("SettingFile");

	BOOL		bResult = TRUE;
	LONG		lStep = 0;
	PRU		*pPRU			= NULL;
	PRU		*pPRUBackground	= NULL;
	CString str;
	
	LONG lSelection = 0;
	LONG lBackgroundInfo = 0;
	LONG lEdgeInfo = 0;
	LONG lAnswer = rMSG_TIMEOUT;
	//PR_COORD	arcoDieCorners[4];

	if (g_ulWizardStep > FINISH_LEARN && g_ulWizardStep != 99)
	{
		g_ulWizardStep = FINISH_LEARN;
	}

	HMI_bCursorControl		= FALSE;
	HMI_bShowSegThreshold	= FALSE;

	g_bWizardEnableBack		= FALSE;
	g_bWizardEnableNext		= FALSE;
	g_bWizardEnableExit		= TRUE;

	SetHmiVariable("SF_bWizardEnable");
	SetHmiVariable("SF_szWizardStep");
	SetHmiVariable("SF_szWizardMess");
	SetHmiVariable("SF_szWizardMessPrev");
	SetHmiVariable("SF_szWizardMessNext");
	SetHmiVariable("SF_szWizardBMPath");
	SetHmiVariable("SF_bWizardEnableBack");
	SetHmiVariable("SF_bWizardEnableNext");
	SetHmiVariable("SF_bWizardEnableExit");
	SetHmiVariable("ACF_WH_bShowPRAlg");

	HMI_bShowSegThreshold	= FALSE;
	HMI_bShowPBMethod		= FALSE;
	HMI_bShowBasicAlg		= FALSE;
	HMI_bShowAdvanceAlg		= FALSE;

	SetHmiVariable("ACF1WH_bXYControl");
	SetHmiVariable("ACF1WH_bTControl");
	SetHmiVariable("ACF1WH_bZControl");
	SetHmiVariable("ACF1WH_bShowSegThreshold");
	SetHmiVariable("ACF1WH_bShowPBMethod");

	//PRS_DisplayVideo(&pruInsp_Calib[m_lCurSlave]);
	//PRS_DrawHomeCursor(ACF_POST_INSP_CAM + m_lCurSlave, FALSE);

	pPRU					= &SelectCurPBPRU(HMI_ulCurPBPRU);
	pPRUBackground			= &SelectCurPBBackground(HMI_ulCurPBPRU);
	//

	// Force the learn ACF, do not need to learn lighting and background
	//if (g_ulWizardStep == SELECT_ALG+1)
	//{
	//	if (
	//		pPRU->stLrnACFCmd.uwIsAlign	== PR_FALSE ||
	//		pPRU->stLrnACFCmd.emACFSegmentAlg == PR_ACF_SEGMENT_ALG_PREPROCESS ||
	//		pPRU->stLrnACFCmd.emACFSegmentAlg == PR_ACF_SEGMENT_ALG_NO_PREPROCESS)
	//	{
	//		//reset only one lighitng
	//		pPRUBackground->stShareImgCmd.uwNumOfPurpose	= 1;
	//		pPRU->stLrnACFCmd.uwNumOfPurpose				= 1;

	//		g_ulWizardStep = LEARN_ACF_REGION;
	//	}
	//}

	// Perform Operations
	switch (g_ulWizardStep)
	{
	case 99:
		PRS_CancelMouseWindow(pPRU);
		PRS_ClearTextRow(ACF_CAM, PRS_MSG_ROW1);

		g_szWizardStep = "";
		g_szWizardMess = "";
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = FALSE;
		g_bWizardEnableExit = TRUE;
		
		HMI_bShowSegThreshold	= FALSE;
		HMI_bShowPBMethod		= FALSE;
		HMI_bShowBasicAlg		= FALSE;
		HMI_bShowAdvanceAlg		= FALSE;

		break;

	case SELECT_ALG:
		pPRU					= &SelectCurPBPRU(HMI_ulCurPBPRU); //20141216
		pPRUBackground			= &SelectCurPBBackground(HMI_ulCurPBPRU);

		HMI_lPBMethod = pPRU->lPBMethod;
		if (pPRU->lPBMethod == BASIC)
		{
			HMI_bShowBasicAlg		= TRUE;
			HMI_bShowAdvanceAlg		= FALSE;
			HMI_bShowACFAlignAlg	= FALSE;
			//Reset the all Data
			if (
				pPRU->stLrnACFCmd.uwIsAlign == PR_FALSE &&
				pPRU->stLrnACFCmd.emACFSegmentAlg == PR_ACF_SEGMENT_ALG_NO_PREPROCESS &&
				pPRU->stLrnACFCmd.emACFAlignAlg == PR_ACF_ALIGN_ALG_PARALLEL_LINE_POINT_FITTING
			)
			{
				m_lIsAlign	= 0;
				m_lAlignAlg = 0;
				g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\ParallelLine_NonAlign.png";
				SetHmiVariable("SF_szWizardBMPath");
			}
			else if (
				pPRU->stLrnACFCmd.uwIsAlign == PR_TRUE &&
				pPRU->stLrnACFCmd.emACFSegmentAlg == PR_ACF_SEGMENT_ALG_PREPROCESS &&
				pPRU->stLrnACFCmd.emACFAlignAlg == PR_ACF_ALIGN_ALG_PARALLEL_LINE_POINT_FITTING
			)
			{ 
				m_lIsAlign	= 1;
				m_lAlignAlg = 0;
				g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\ParallelLine_Align.png";
				SetHmiVariable("SF_szWizardBMPath");
			}
			else if (
				pPRU->stLrnACFCmd.uwIsAlign == PR_TRUE &&
				pPRU->stLrnACFCmd.emACFSegmentAlg == PR_ACF_SEGMENT_ALG_NO_PREPROCESS &&
				pPRU->stLrnACFCmd.emACFAlignAlg == PR_ACF_ALIGN_ALG_3LINE_FITTING
			)
			{	
				m_lIsAlign	= 1;
				m_lAlignAlg = 1;
				g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\LineFitting_Align.png";
				SetHmiVariable("SF_szWizardBMPath");
			}
			else
			{
				m_lIsAlign	= 0;
				m_lAlignAlg = 0;
				pPRU->stLrnACFCmd.uwIsAlign			= PR_FALSE;
				pPRU->stLrnACFCmd.emACFSegmentAlg	= PR_ACF_SEGMENT_ALG_NO_PREPROCESS;
				pPRU->stLrnACFCmd.emACFAlignAlg		= PR_ACF_ALIGN_ALG_PARALLEL_LINE_POINT_FITTING;
				g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\ParallelLine_NonAlign.png";
				SetHmiVariable("SF_szWizardBMPath");
				WriteHMIMess("Convert To Non_Align Mode");
			}
			SetHmiVariable("ACF_WH_lIsAlign");
			SetHmiVariable("ACF_WH_lAlignAlg");

		}
		else
		{
			HMI_bShowBasicAlg = FALSE;
			HMI_bShowAdvanceAlg = TRUE;
			HMI_lACFSegmentAlg	= pPRU->stLrnACFCmd.emACFSegmentAlg;
			HMI_lACFAlignAlg	= pPRU->stLrnACFCmd.emACFAlignAlg;
			HMI_lACFIsAlign		= pPRU->stLrnACFCmd.uwIsAlign;

			if (pPRU->stLrnACFCmd.uwIsAlign == FALSE)
			{
				HMI_bShowACFAlignAlg	= FALSE;
			}
			else
			{
				HMI_bShowACFAlignAlg	= TRUE;
			}
		}

		HMI_bShowPBMethod = TRUE;

		SetHmiVariable("ACF_WH_bShowBasicAlg");
		SetHmiVariable("ACF_WH_bShowAdvanceAlg");
		SetHmiVariable("ACF1WH_bShowPBMethod");

		g_szWizardStep = "Step 1/3";
		g_szWizardMessPrev = "";
		g_szWizardMess = "1.Please select the Algorithm";
		g_szWizardMessNext = "2. Adjust the lighting";	
		//g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;
#if 1 //20120830
		lAnswer = HMISelectionBox("LEARN PR", "Please Select ACF Transparency:", "OPAQUE", "SEMI", "TRANSPARENT");

		if (lAnswer == -1 || lAnswer == 11)
		{
		//default value
			pPRU->stLrnACFCmd.emACFTransparency = PR_OBJ_TRANSPARENCY_OPAQUE;
			HMI_bShowBasicAlg		= FALSE;
			HMI_bShowAdvanceAlg		= FALSE;
			HMI_bShowACFAlignAlg	= FALSE;
			bResult = FALSE;
		}
		if (lAnswer == 0)
		{
			pPRU->stLrnACFCmd.emACFTransparency = PR_OBJ_TRANSPARENCY_OPAQUE;
		}
		else if (lAnswer == 1)
		{
			pPRU->stLrnACFCmd.emACFTransparency = PR_OBJ_TRANSPARENCY_SEMI;
		}
		else if (lAnswer == 2)
		{
			pPRU->stLrnACFCmd.emACFTransparency = PR_OBJ_TRANSPARENCY_TRANSPARENT;
		}
		else
		{
			pPRU->stLrnACFCmd.emACFTransparency = PR_OBJ_TRANSPARENCY_OPAQUE;
		}
#endif
		break;


	case LEARN_ACF_REGION: //Learn the ACF // only Substraction, ensure user remove the glass
		//if (IndexGlassToTable((GlassNum)GLASS1) != GMP_SUCCESS)
		//{
		//	bResult = FALSE;
		//}
		//else
		{
			if (MoveXYToCurPBPosn(HMI_ulCurPBPRU))
			{
				bResult = FALSE;
				break;
			}

			if (
				pPRU->stLrnACFCmd.emACFSegmentAlg == PR_ACF_SEGMENT_ALG_PREPROCESS ||
				pPRU->stLrnACFCmd.emACFSegmentAlg == PR_ACF_SEGMENT_ALG_NO_PREPROCESS
			)
			{
				lBackgroundInfo = HMISelectionBox("ACF LEARN POSTBOND PR OPERATION", "Please select the ACF Surface:", "Dark", "Bright");

				if (lBackgroundInfo == -1 || lBackgroundInfo == 11)
				{
					bResult = FALSE;
					break;
				}
				else
				{
					switch (lBackgroundInfo)
					{
					case 0:
						pPRU->stLrnACFCmd.emACFAttribute = PR_OBJ_ATTRIBUTE_DARK;
						pPRU->stSrchACFCmd.stACFInsp.emDefectAttribute	= PR_DEFECT_ATTRIBUTE_BRIGHT;
						HMI_ulPRPostBdAttrib = PR_DEFECT_ATTRIBUTE_BRIGHT; //20121107
						break;

					case 1:
						pPRU->stLrnACFCmd.emACFAttribute = PR_OBJ_ATTRIBUTE_BRIGHT;	
						pPRU->stSrchACFCmd.stACFInsp.emDefectAttribute	= PR_DEFECT_ATTRIBUTE_DARK;
						HMI_ulPRPostBdAttrib = PR_DEFECT_ATTRIBUTE_DARK; //20121107

						break;
					}
				}
			}
			else // For Substraction and runtime substraction, onlt support back acf 
			{
				pPRU->stLrnACFCmd.emACFAttribute = PR_OBJ_ATTRIBUTE_DARK;
				pPRU->stSrchACFCmd.stACFInsp.emDefectAttribute	= PR_DEFECT_ATTRIBUTE_DARK;
				HMI_ulPRPostBdAttrib = PR_DEFECT_ATTRIBUTE_DARK; //20121107
			}
		

			lEdgeInfo = HMISelectionBox("ACF LEARN POSTBOND PR OPERATION", "Please select the ACF edge to be inspected:", "Left Edge", "Right Edge");

			if (lEdgeInfo == -1 || lEdgeInfo == 11)
			{
				bResult = FALSE;
				break;
			}
			else
			{
				switch (lEdgeInfo)
				{
				case 0:
					pPRU->stLrnACFCmd.emACFPosition = PR_OBJ_POSITION_LEFT;		// Bright Background = Dark ACF Surface
					break;

				case 1:
					pPRU->stLrnACFCmd.emACFPosition = PR_OBJ_POSITION_RIGHT;		// Bright Background = Dark ACF Surface
					break;
				}
			}
			// Draw ACF Region
			DrawMouseWindow(pPRU, pPRU->stLrnACFCmd.stACFRegion);

			//Preset only one DefectMultRegion
			pPRU->stLrnACFCmd.stDefectMultRegion.uwNMultRegion						= 1;
			pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[0].uwNumOfCorners		= 4;



			if (pPRU->stLrnACFCmd.uwIsAlign == PR_FALSE)
			{
				g_szWizardMessPrev = "5. Remove the Glass, and place a Glass With ACF to InConveyor";
				g_szWizardMess = "6. Learn ACF Area";
				g_szWizardMessNext = "7. Learn Defect Area";	
				g_szWizardBMPath = "";

				// Skip all Adjust threshold, and search Area
				g_ulWizardStep = ADJUST_THERSHOLD;
			}
			else if (pPRU->stLrnACFCmd.emACFSegmentAlg == PR_ACF_SEGMENT_ALG_NO_PREPROCESS)
			{
				g_szWizardMessPrev = "5. Remove the Glass, and place a Glass With ACF to InConveyor";
				g_szWizardMess = "6. Learn ACF Area";
				g_szWizardMessNext = "7. Adjust the Segment Threshold";	
				g_szWizardBMPath = "";

				//skip search Area
				g_ulWizardStep = LEARN_SRCH_REGION;
			}
			else
			{
				g_szWizardMessPrev = "5. Remove the Glass, and place a Glass With ACF to InConveyor";
				g_szWizardMess = "6. Learn ACF Area";
				g_szWizardMessNext = "7. Learn The Search Area";	
				g_szWizardBMPath = "";
			}
		}

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;
		break;

	case LEARN_SRCH_REGION:
		GetMouseWindow(pPRU, &pPRU->stLrnACFCmd.stACFRegion);
			
		// Draw ACF Region
		DrawMouseWindow(pPRU, pPRU->stLrnACFCmd.stSrchRegion);

		g_szWizardMessPrev = "5. Remove the Glass, and place a Glass With ACF to InConveyor";
		g_szWizardMess = "6. Learn the Search Area";
		g_szWizardMessNext = "7. Adjust the Segment Threshold";	
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;
		break;

	case ADJUST_THERSHOLD: //Learn the Thershold or move the Learn Defect Region
		if (pPRU->stLrnACFCmd.emACFSegmentAlg == PR_ACF_SEGMENT_ALG_NO_PREPROCESS)
		{
			GetMouseWindow(pPRU, &pPRU->stLrnACFCmd.stACFRegion);
			
			pPRU->stLrnACFCmd.stSrchRegion.acoCorners[0].x	= pPRU->stLrnACFCmd.stACFRegion.acoCorners[0].x + 30 * 16;
			pPRU->stLrnACFCmd.stSrchRegion.acoCorners[0].y	= pPRU->stLrnACFCmd.stACFRegion.acoCorners[0].y - 30 * 16;
			pPRU->stLrnACFCmd.stSrchRegion.acoCorners[1].x	= pPRU->stLrnACFCmd.stACFRegion.acoCorners[1].x - 30 * 16;
			pPRU->stLrnACFCmd.stSrchRegion.acoCorners[1].y	= pPRU->stLrnACFCmd.stACFRegion.acoCorners[1].y - 30 * 16;
			pPRU->stLrnACFCmd.stSrchRegion.acoCorners[2].x	= pPRU->stLrnACFCmd.stACFRegion.acoCorners[2].x - 30 * 16;
			pPRU->stLrnACFCmd.stSrchRegion.acoCorners[2].y	= pPRU->stLrnACFCmd.stACFRegion.acoCorners[2].y + 30 * 16;;
			pPRU->stLrnACFCmd.stSrchRegion.acoCorners[3].x	= pPRU->stLrnACFCmd.stACFRegion.acoCorners[3].x + 30 * 16;
			pPRU->stLrnACFCmd.stSrchRegion.acoCorners[3].y	= pPRU->stLrnACFCmd.stACFRegion.acoCorners[3].y + 30 * 16;;


		}
		else
		{
			GetMouseWindow(pPRU, &pPRU->stLrnACFCmd.stSrchRegion);
		}

		//pre-set PRThreshold Substraction = 50 or =100 // Request by Tony
		pPRU->stLrnACFCmd.ubSegThreshold = 100;
		HMI_lSegThreshold = pPRU->stLrnACFCmd.ubSegThreshold;

		//Display the Segment Image
		if (pPRU->stLrnACFCmd.emACFSegmentAlg == PR_ACF_SEGMENT_ALG_SUBTRACTION || pPRU->stLrnACFCmd.emACFSegmentAlg == PR_ACF_SEGMENT_ALG_RT_SUBTRACTION)
		{
			PRS_DisplayACFInfo(pPRU, pPRUBackground, PR_ACF_DISPLAY_SUBTRACT_IMG);
		}
		else
		{
			PRS_DisplayACFInfo(pPRU, pPRUBackground, PR_ACF_DISPLAY_SEGMENT_IMG);
		}

		g_szWizardStep = "2/3";
		g_szWizardMessPrev = "6. Learn ACF Area";
		g_szWizardMess = "7. Adjust the Segment Threshold";
		g_szWizardMessNext = "8. Define inspect region for Defect postbond";
		g_szWizardBMPath = "";
		
		g_bWizardEnableBack		= FALSE;
		g_bWizardEnableNext		= TRUE;
		g_bWizardEnableExit		= TRUE;
		HMI_bShowSegThreshold	= TRUE;

		break;

	case LEARN_DEFECT_REGION:
		if (pPRU->stLrnACFCmd.uwIsAlign == PR_FALSE)
		{
			pPRU->stLrnACFCmd.stSrchRegion.acoCorners[0].x	= PR_DEF_WIN_LRC_X;
			pPRU->stLrnACFCmd.stSrchRegion.acoCorners[0].y	= PR_DEF_WIN_ULC_Y;
			pPRU->stLrnACFCmd.stSrchRegion.acoCorners[1].x	= PR_DEF_WIN_ULC_X;
			pPRU->stLrnACFCmd.stSrchRegion.acoCorners[1].y	= PR_DEF_WIN_ULC_Y;
			pPRU->stLrnACFCmd.stSrchRegion.acoCorners[2].x	= PR_DEF_WIN_ULC_X;
			pPRU->stLrnACFCmd.stSrchRegion.acoCorners[2].y	= PR_DEF_WIN_LRC_Y;
			pPRU->stLrnACFCmd.stSrchRegion.acoCorners[3].x	= PR_DEF_WIN_LRC_X;
			pPRU->stLrnACFCmd.stSrchRegion.acoCorners[3].y	= PR_DEF_WIN_LRC_Y;


			GetMouseWindow(pPRU, &pPRU->stLrnACFCmd.stACFRegion);
		}

		m_lTotalDefectMultRegion = pPRU->stLrnACFCmd.stDefectMultRegion.uwNMultRegion;
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[m_lTotalDefectMultRegion - 1].uwNumOfCorners		= 4;

		DrawMouseWindow(pPRU, pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[m_lTotalDefectMultRegion - 1]);

		g_szWizardStep = "2/3";
		g_szWizardMessPrev = "7. Adjust the Segment Threshold";
		g_szWizardMess.Format("8. Define inspect region for %d Defect postbond", m_lTotalDefectMultRegion);
		//g_szWizardMess = "8. Define inspect region for Defect postbond";
		g_szWizardMessNext = "DONE";
		g_szWizardBMPath = "";
		
		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;

		break;

	case LEARN_CHIP_REGION: //Learn the ChipArea
		GetMouseWindow(pPRU, &pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[m_lTotalDefectMultRegion - 1]);
		//str.Format("Learn %ld DefectMultRegion?", m_lTotalDefectMultRegion+1);
		//if (m_lTotalDefectMultRegion < 4)
		//{
		//	lAnswer = HMIMessageBox(MSG_YES_NO, "WARNING", str)
		//	if (lAnswer == rMSG_YES)
		//	{
		//		pPRU->stLrnACFCmd.stDefectMultRegion.uwNMultRegion++;
		//		m_lTotalDefectMultRegion = pPRU->stLrnACFCmd.stDefectMultRegion.uwNMultRegion;
		//		DrawMouseWindow(pPRU, pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[m_lTotalDefectMultRegion-1]);
		//		g_ulWizardStep = LEARN_DEFECT_REGION;
		//		g_bWizardEnableBack = FALSE;
		//		g_bWizardEnableNext = TRUE;
		//		g_bWizardEnableExit = TRUE;

		//		break;
		//	}
		//	else if (lAnswer == rMSG_TIMEOUT)
		//	{
		//		bResult = FALSE;
		//		break;
		//	}
		//}

		pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1]						= pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[0];
		DrawMouseWindow(pPRU, pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1]);

		g_szWizardStep = "2/3";
		g_szWizardMessPrev = "8. Define inspect region for Defect postbond";
		g_szWizardMess = "9. Define Chip region for ACF postbond";
		g_szWizardMessNext = "";
		g_szWizardBMPath = "";
		
		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;

		break;

	
	case FINISH_LEARN:
		pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].uwNumOfCorners	= pPRU->stLrnACFCmd.stACFRegion.uwNumOfCorners;
		pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0]					= pPRU->stLrnACFCmd.stACFRegion;
		pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].uwNumOfCorners = pPRU->stLrnACFCmd.stACFRegion.uwNumOfCorners;
		
		GetMouseWindow(pPRU, &pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1]);
		

		// Add the Background Image
		if (
			pPRU->stLrnACFCmd.emACFSegmentAlg == PR_ACF_SEGMENT_ALG_SUBTRACTION ||
			pPRU->stLrnACFCmd.emACFSegmentAlg == PR_ACF_SEGMENT_ALG_RT_SUBTRACTION
		)
		{
			pPRU->stLrnACFCmd.uwPreSubtractStationID	= (PR_UWORD)pPRUBackground->lStationID;
			pPRU->stLrnACFCmd.ulPreSubtractImageID		= pPRUBackground->lImageID;
		}
		else
		{
			pPRU->stLrnACFCmd.uwPreSubtractStationID	= 0;
			pPRU->stLrnACFCmd.ulPreSubtractImageID		= 0;
		}

		// PB set the Search MinChipArea
		pPRU->stSrchACFCmd.stACFInsp.aeMinChipArea = 
			(PR_AREA)(
				(DOUBLE)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_UPPER_RIGHT].x - pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_LOWER_LEFT].x) * 
				(DOUBLE)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_LOWER_LEFT].y - pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_UPPER_RIGHT].y) * 
				pPRU->dPostBdRejectTol / 100.0 
			);

		pPRU->stSrchACFCmd.stACFInsp.aeMinTotalDefectArea[0]	= pPRU->stSrchACFCmd.stACFInsp.aeMinChipArea;
		pPRU->stSrchACFCmd.stACFInsp.aeMinSingleDefectArea[0]	= pPRU->stSrchACFCmd.stACFInsp.aeMinChipArea;


		pPRU->emRecordType = PRS_ACF_TYPE;
		bResult = LrnACFCmd(pPRU, pPRUBackground);
		
		UpdateHMIPBPRButton();

		g_szWizardStep = "2/3";
		g_szWizardMessPrev = "";
		g_szWizardMess = "Done";
		g_szWizardMessNext = "";
		g_szWizardBMPath = "";	

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = FALSE;
		g_bWizardEnableExit = TRUE;
		break;
	}

	if (!bResult)
	{
		//Free Background Record while PRU not learned when learn Background pPRU->bLoad = FALSE
		// pPRUbackground is will be free by Lrn ACF Cmd. 
		// pPRUBackground Will be reset by LrnACFCmd
		if (pPRUBackground->bLoaded)
		{
			PRS_RemoveShareImg(pPRUBackground);
		}

		PRS_CancelMouseWindow(pPRU);
		PRS_ClearTextRow(ACF_CAM, PRS_MSG_ROW1);

		g_szWizardStep = "FAIL";
		g_szWizardMess = "OPERATION FAIL!";
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = FALSE;
		g_bWizardEnableExit = TRUE;

		HMI_bCursorControl		= FALSE;
		HMI_bShowSegThreshold	= FALSE;

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