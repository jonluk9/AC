/////////////////////////////////////////////////////////////////
//	ACF.cpp : interface of the CACF class
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
#include "TempDisplay.h"



#define ACF_EXTERN
#include "ACF.h"
#include "ACF1.h"
#include "ACFWH.h"

#define ACF_MAX_FORCE	20000.0

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CACF, CAC9000Stn)

LONG			CACF::m_lNumOfPressBeforeAlert = 0; //20150416
LONG			CACF::m_lCurrBondForceMode = NORMAL_BOND_FORCE_MODE;

BOOL	CACF::m_bLogForceSensor = FALSE;
BOOL	CACF::m_bLogForceSensorEndOfBond = FALSE;

CACF::CACF()
{
	m_lCurSlave = ACF_NONE;
	
	// cycle status
	m_dACFProcessTime				= 0.0;

	//Force
	m_bDACValid						= FALSE;
	m_dPressureCtrl					= 0.0;
	for (INT i = 0; i < MAX_NUM_OF_ACF; i++)
	{
		m_dFinalPressureCtrl[i]		= 0.0;
		m_lFinalDACCtrl[i]		= 0;
	}
	m_dUpPressureCtrl				= 0;

	// ACF Indexing Related
	m_bValidIndexedLength			= FALSE;
	for (INT i = 0; i < MAX_SIZE_OF_ACF_ARRAY; i++)
	{
		m_emACFIndexed[i]			= NO_ACF;
	}
	m_emLastIndexedACF				= NO_ACF;
	m_nIndexFirst					= 0;
	m_nIndexLast					= 0;
	
	// ACF Length
	m_dIndexerFactor				= 1.0; //20.0 / 15.0; //1; 20120302
	m_dIndexerCustomFactor			= 1.0; //20120706
	
	// ACF Cutter to Head Distance
	m_dCutterToHeadMachineDist		= 111.2;	// Defined by Machine //71.2
	m_dCutterToHeadDistance			= 0.0;
	m_dCutterToHeadOffset			= 0.0;

	m_dRemainingLength				= 0.0;
	m_dAdvancedLength				= 0.0;
	m_dIndexedLength				= 0.0;

	//Delays
	m_lPreAttachDelay				= 300;
	m_lAttachACFDelay				= 300;
	m_lPressTestDelay				= 300;

	// force calibration
	m_lCalibPreAttachDelay			= 50;
	m_lCalibAttachDelay				= 50;
	m_bCalibDACValid				= FALSE;
	m_dCalibFinalPressureCtrl		= 0;
	m_dCalibUpPressureCtrl			= 0;
	m_dCalibPressureCtrl			= 0;
	m_dCalibForceCtrl				= 0;
	m_lForceCalibPressCycle			= 3;
	m_lCalibFinalDACCtrl			= 0;

	m_lStandbyPosn					= 0;
	m_lOutPosn						= 0;
	m_lOutOffset					= 0;

	m_dStripperXGoMoveLeftSpeedFactor = 1.0;
	m_dStripperXGoMoveRightSpeedFactor = 1.0;

	m_lCurNumOfPressCount = 0; //20150416
	m_lNumOfPressBeforeAlert = 0;
	m_bChangeRubberBandAlertShown = FALSE;

	for (INT j = 0; j < MAX_BOND_FORCE_MODE; j++)
	{
		for (INT i = 0; i < MAX_FORCE_DATA; i++)
		{
			m_stForceCalib[j][i].Force_kgf		= 0;
			m_stForceCalib[j][i].Pressure_bar	= 0;
			m_stForceCalib[j][i].OpenDacValue	= 0;
		}
	}

	m_lCurrBondForceMode = NORMAL_BOND_FORCE_MODE;

	HMI_szACFArray					= "";
	HMI_dFinalPressureCtrl			= 0;
	HMI_dFinalForceCtrl				= 0;
	HMI_dCalibForceCtrl				= 0;
	HMI_lFinalDACCtrl			= 0;

	m_lForceADCValue = 0;
	
	m_dForceBondWeight = 0.0;

	m_dForceSensorFactor = 50.0;	 //MB1 54.08pC/lbf, MB3 54.47pC/lbf 

	//m_bMBForceLogRun[ACF_1] = FALSE;

	m_szLogLabel.Format("--");
	
	//m_bForceTolerenceError = FALSE;
	//m_bForceContactError = FALSE;
	
	m_dForceKgEndOfBonding = 0.0; //20161117
	m_lForceADCEndOfBonding = 0;
	m_hThreadForceADC = NULL;
}

CACF::~CACF()
{
	//delete m_pCACFWH;
}

BOOL CACF::InitInstance()
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

				GetmySoList().GetAt(i)->bHMI_Status = GetmySoList().GetAt(i)->GetGmpPort().IsOn();

			}

			//m_stDACPort.SetGmpPort(pGmpSys->AddDACPort(m_stDACPort.GetPortName()));
			m_stADCPort.SetGmpPort(pGmpSys->AddDACPort(m_stADCPort.GetPortName())); //use dac read function for ADC port
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

		if (GetStnName() == "ACF1")
		{
			pAppMod->m_bACF1Comm	= TRUE;	
		}
		//else if (GetStnName() == "ACF2")	
		//{	
		//	pAppMod->m_bACF2Comm	= TRUE;	
		//}

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
				if (GetmyMotorList().GetAt(i)->Home(GMP_WAIT) != GMP_SUCCESS)
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

			if (GetStnName() == "ACF1")
			{	
				pAppMod->m_bACF1Home	= TRUE;	
			}
			//else if (GetStnName() == "ACF2")	
			//{	
			//	pAppMod->m_bACF2Home	= TRUE;	
			//}
		}
	}
	else
	{
		if (GetStnName() == "ACF1")	
		{	
			pAppMod->m_bACF1Home	= TRUE;
			pAppMod->m_bACF1Comm	= TRUE;	
		}
		//else if (GetStnName() == "ACF2")
		//{	
		//	pAppMod->m_bACF2Home	= TRUE;	
		//	pAppMod->m_bACF2Comm	= TRUE;	
		//}
	}

	return TRUE;
}

INT CACF::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CAC9000Stn::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CACF::Operation()
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

VOID CACF::UpdateOutput()
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

			LONG lForce = 0;
			if (GetADCValue(lForce) == GMP_SUCCESS)
			{
				m_lForceADCValue = lForce;
				m_dForceBondWeight = RawADCToForce(m_lForceADCValue, m_dForceSensorFactor) / 2.2;
			}

			if (m_bModSelected)
			{
				CheckModSelected(TRUE);
			}
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

VOID CACF::UpdateCurPressureValue()
{
	BOOL bValid = TRUE;

	//HMI_dFinalPressureCtrl = m_dFinalPressureCtrl[m_pCACFWH->m_lCurACF];
	HMI_lFinalDACCtrl = m_lFinalDACCtrl[ACF_1]; //20130702

	m_bDACValid = DacToForce(m_lCalibFinalDACCtrl, m_stForceCalib[m_lCurrBondForceMode], &m_dCalibForceCtrl);
	m_bDACValid = DacToForce(HMI_lFinalDACCtrl, m_stForceCalib[m_lCurrBondForceMode], &HMI_dFinalForceCtrl);

	HMI_dCalibForceCtrl			= m_dCalibForceCtrl * (g_bShowForceInNewton * KGF_TO_NEWTON + (1 - (LONG)g_bShowForceInNewton));
}

VOID CACF::RegisterVariables()
{
	try
	{
		//Register Sensors here 
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
VOID CACF::PreStartOperation()
{
	m_bModError = FALSE; //20130510
	if (!m_bModSelected)
	{	
		ACF_Status[m_lCurSlave] = ST_MOD_NOT_SELECTED;
		m_qSubOperation = ACF_STOP_Q;
		m_bStopCycle = FALSE;
	}
	else
	{
		if (
			//(m_nLastError = SleepWithReturn(1000)) != GMP_SUCCESS ||						//20141230

			((m_nLastError = MoveBF(ACF_BF_CTRL_OFF, GMP_NOWAIT)) != GMP_SUCCESS) ||
			((m_nLastError = MoveZ(ACF_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) != GMP_SUCCESS) ||
			((m_nLastError = MoveStripperX(ACF_STRIPPER_X_CTRL_GO_STANDBY_POSN, GMP_WAIT)) != GMP_SUCCESS) ||
			((AutoMode == BURN_IN) && (m_nLastError = SetHalfCutterOnSol(ON, GMP_WAIT)) != GMP_SUCCESS) || 
			((AutoMode == BURN_IN) && (m_nLastError = SetHalfCutterOnSol(OFF, GMP_WAIT)) != GMP_SUCCESS) ||
			//((m_nLastError = SetIonizerPowerEnable(ON, GMP_WAIT)) != GMP_SUCCESS) ||
			getKlocworkFalse() //klocwork fix 20121211
			)
		{
			SetError(m_nLastError);
			ACF_Status[m_lCurSlave] = ST_MOD_NOT_SELECTED;
			m_qSubOperation = ACF_STOP_Q;
			m_bStopCycle = FALSE;
		}
		else
		{
			ACF_Status[m_lCurSlave] = ST_IDLE;
			m_qSubOperation = ACF_IDLE_Q;
			m_bStopCycle = FALSE;
			m_lCurrWH = ACFWH_1;		//initial = ACFWH_1
		}
	}
	m_bChangeRubberBandAlertShown = FALSE; //20150416

	Motion(FALSE);
	State(IDLE_Q);

	m_deqDebugSeq.clear(); //20131010
	m_qPreviousSubOperation = m_qSubOperation;
	UpdateDebugSeq(GetSequenceString(), ACF_Status[m_lCurSlave]);
	
	CAC9000Stn::PreStartOperation();
}

VOID CACF::CycleOperation()
{
	BOOL bIsMoveStripper = TRUE;

	if (!m_bPressCycle)
	{
		State(STOPPING_Q);

		return;
	}

	if (Command() == glSTOP_COMMAND)
	{
		m_bPressCycle = FALSE;
	}

	//SinglePressOperation();
	SinglePressOperation(bIsMoveStripper); //20130318

	if (!m_bPressCycle)
	{
//		SetHeadDownSol(OFF, GMP_WAIT);
		SetDACValue(m_dPressureCtrl);

		State(STOPPING_Q);

		CString szMsg = _T("   ") + GetName() + " --- CycleOperation Stopped";
		DisplayMessage(szMsg);
	}
}

VOID CACF::AutoOperation()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	// stop command received: end operation after cycle completed
	if (Command() == glSTOP_COMMAND || g_bStopCycle)
	{
		m_bStopCycle = TRUE;
	}

	if (m_pCACFWH->m_stACF[ACF1].dLength <= (m_dCutterToHeadDistance + m_dCutterToHeadOffset))
	{
		OperationSequence();
	}
	else
	{
		OperationSequence_Single_ACF();
	}

	// cycle completed or motor error or abort requested: end operation immediately
	if (
		(m_bModSelected && m_qSubOperation == ACF_STOP_Q) || 
		(!m_bModSelected && m_bStopCycle) || 
		GMP_SUCCESS != m_nLastError || IsAbort() 
	   )
	{
		if (m_fHardware && m_bModSelected)
		{
			// Add me later
			MoveStripperX(ACF_STRIPPER_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
			MoveZ(ACF_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT);
		}

		if (GMP_SUCCESS != m_nLastError)
		{
			m_bModError = TRUE;
		}
		pAppMod->m_smfMachine.Update(); //20150502 for saving the ACF Press count

		ACF_Status[m_lCurSlave] = ST_STOP;
		Command(glSTOP_COMMAND);
		State(STOPPING_Q);
		m_bStopCycle = FALSE;

		CAC9000Stn::StopOperation();
	}
}

CString CACF::GetSequenceString()
{
	switch (m_qSubOperation)
	{
	case ACF_IDLE_Q:
		return "ACF_IDLE_Q";

	case ACF_WAIT_REQUEST_Q:
		return "ACF_WAIT_REQUEST_Q";

	case ACF_BOND_OPERATION_Q:
		return "ACF_BOND_OPERATION_Q";

	case ACF_STRIPPER_ON_Q:
		return "ACF_STRIPPER_ON_Q";

	case ACF_STRIPPER_OFF_Q:
		return "ACF_STRIPPER_OFF_Q";

	case ACF_INDEX_OPERATION_Q:
		return "ACF_INDEX_OPERATION_Q";

	case ACF_INDEX_LAST_OPERATION_Q:
		return "ACF_INDEX_LAST_OPERATION_Q";

	case ACF_STOP_Q:
		return "ACF_STOP_Q";
	}

	return "Hi";
}

/////////////////////////////////////////////////////////////////
//Operational Sequences
/////////////////////////////////////////////////////////////////
VOID CACF::OperationSequence()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	m_nLastError = GMP_SUCCESS;

	CString szMsg = "";

	m_nLastError = GMP_SUCCESS;
	DOUBLE dAllowedLength = 0.0;
	DOUBLE dTotalLength = 0.0;
	DOUBLE dRelativeLength = 0.0;

	switch (m_qSubOperation)
	{
	case ACF_IDLE_Q: // 0
		if (m_bStopCycle)
		{
			m_qSubOperation = ACF_STOP_Q;
		}
		else if ((m_nLastError = SyncRoller()) == GMP_SUCCESS)
		{
			m_qSubOperation = ACF_WAIT_REQUEST_Q;	// go to 1
		}
		break;

	case ACF_WAIT_REQUEST_Q:		// 1
		if (m_bStopCycle)
		{
			m_qSubOperation = ACF_STOP_Q;
		}
		else if (ACF_WH_Status[ACFWH_1] == ST_STOP && ACF_WH_Status[ACFWH_2] == ST_STOP)
		{
			m_qSubOperation = ACF_STOP_Q;	// go to 99
		}
		else if (
				 ACF_WH_Status[m_lCurrWH] == ST_REQ_BONDING || 
				(ACF_WH_Status[ACFWH_1] == ST_MOD_NOT_SELECTED && ACF_WH_Status[ACFWH_2] == ST_MOD_NOT_SELECTED)
				)
		{

			//if (
			//	//((m_nLastError = SetHeadUpSol(ON, GMP_NOWAIT)) == GMP_SUCCESS) && //wait before
			//	//((m_nLastError = SetCounterBalanceSol(ON, GMP_NOWAIT)) == GMP_SUCCESS) &&	//wait before
			//	//((m_nLastError = SetDACValue(m_dPressureCtrl)) == GMP_SUCCESS) &&
			//	((m_nLastError = MoveZ(ACF_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) == GMP_SUCCESS) &&
			//	getKlocworkTrue() //klockwork fix 20121211
			//   )
			{
				if(m_lCurrWH == ACFWH_1)
				{
					ACF_Status[m_lCurSlave] = ST_ACFWH1_REQ_ACF_ACK;	//Grant Permission to ACFWH1
				}
				else if(m_lCurrWH == ACFWH_2)
				{
					ACF_Status[m_lCurSlave] = ST_ACFWH2_REQ_ACF_ACK;	//Grant Permission to ACFWH2
				}

				m_qSubOperation = ACF_BOND_OPERATION_Q;	// go to 2
			}
			//else
			//{
			//	// ERROR!
			//	m_qSubOperation = ACF_STOP_Q;	// go to 7
			//	WriteHMIMess("*** ERROR: ACF Z Motor Error ***", TRUE);
			//	m_bModError = TRUE;
			//	SetError(m_nLastError);
			//}
		}
		else
		{
			if(ACF_WH_Status[ACFWH_1] == ST_MOD_NOT_SELECTED)
			{
				m_lCurrWH = ACFWH_2;
			}
			else if(ACF_WH_Status[ACFWH_2] == ST_MOD_NOT_SELECTED)
			{
				m_lCurrWH = ACFWH_1;
			}
			else
			{
				m_lCurrWH = (m_lCurrWH + 1) % MAX_NUM_OF_ACFWH;	//Check Next ACFWH
			}
		}

		break;

	case ACF_BOND_OPERATION_Q:	// 2
		if (ACF_WH_Status[m_lCurrWH] == ST_STOP)
		{
			m_qSubOperation = ACF_STOP_Q;	// go to 7
		}
		else if (
				 ACF_WH_Status[m_lCurrWH] == ST_BONDING || 
				 ACF_WH_Status[m_lCurrWH] == ST_MOD_NOT_SELECTED 
				)
		{
			CString szMsg = "";
			if (HMI_bDebugCal)
			{
				szMsg.Format("ACF Head:%ld, ACF WH Request:%ld", (LONG)(m_emACFIndexed[m_nIndexLast] + 1), m_pCACFWH->m_lCurACF + 1);
				LogAction(szMsg);
				//DisplayMessage(szMsg);
			}

			// check ACF indexed = requested ACF by WH?
			if (
				((m_pCACFWH->m_stACF[m_emACFIndexed[m_nIndexLast]].dLength) != m_pCACFWH->m_stACF[(ACFNum)m_pCACFWH->m_lCurACF].dLength) && 
				ACF_WH_Status[m_lCurrWH] != ST_MOD_NOT_SELECTED
			   )
			{
				//ERROR! Invalid ACF
				m_qSubOperation = ACF_STOP_Q;	// go to 7
				WriteHMIMess("*** ERROR: Invalid ACF ***", TRUE);
				m_bModError = TRUE;
				SetError(IDS_ACF1_INVALID_ACF_ERR);
				
				// Debug
				//if (HMI_bDebugCal)
				//{
				szMsg.Format("ERR ACF Head:%ld, %.8f, ACF WH Request:%ld, %.8f", (LONG)(m_emACFIndexed[m_nIndexLast] + 1), m_pCACFWH->m_stACF[m_emACFIndexed[m_nIndexLast]].dLength, m_pCACFWH->m_lCurACF + 1, m_pCACFWH->m_stACF[(ACFNum)m_pCACFWH->m_lCurACF].dLength);
				//	LogAction(szMsg);
				DisplayMessage(szMsg);
				//}
			}
#ifdef ACF_TAPE_END_SNR
			else if ((AutoMode == AUTO_BOND || AutoMode == TEST_BOND) && IsTapeEndSnrOn()) //20121122
			{
				//ERROR! ACF Tape End Error
				m_qSubOperation = ACF_STOP_Q;	// go to 7
				WriteHMIMess("*** ERROR: ACF Tape End Error ***", TRUE);
				m_bModError = TRUE;
				//SetError(IDS_ACF1_TAPE_END_ERR + (m_lCurSlave * CONVERT_TO_MAPPING));
				SetError(IDS_ACF1_TAPE_END_ERR);
			}
#endif
			else
			{
				//szMsg.Format("OK ACF Head:%ld, %.8f, ACF WH Request:%ld, %.8f", (LONG)(m_emACFIndexed[m_nIndexLast]+1), m_pCACFWH->m_stACF[m_emACFIndexed[m_nIndexLast]].dLength, m_pCACFWH->m_lCurACF+1, m_pCACFWH->m_stACF[(ACFNum)m_pCACFWH->m_lCurACF].dLength);
				//DisplayMessage(szMsg);

				RemoveACFArray();

				if (
					((m_nLastError = MoveBF(ACF_BF_CTRL_DAC_ON, GMP_NOWAIT)) == GMP_SUCCESS) &&
					//((m_nLastError = MoveZ(ACF_Z_CTRL_GO_SEARCH_LEVEL, GMP_WAIT)) == GMP_SUCCESS) &&
					((m_nLastError = MoveZ(ACF_Z_CTRL_GO_BOND_LEVEL, GMP_WAIT)) == GMP_SUCCESS) &&
					//((m_nLastError = SleepWithReturn(m_lPreAttachDelay)) == GMP_SUCCESS) &&
					((m_nLastError = SleepWithReturn(m_lAttachACFDelay)) == GMP_SUCCESS) &&
					((m_nLastError = MoveBF(ACF_BF_CTRL_OFF, GMP_NOWAIT)) == GMP_SUCCESS) &&
					((m_nLastError = MoveZ(ACF_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) == GMP_SUCCESS) &&
					((m_nLastError = MoveStripperX(ACF_STRIPPER_X_CTRL_GO_OUT_OFFSET, GMP_NOWAIT)) == GMP_SUCCESS)
				   )
				{
					//szMsg.Format("After Bonding");
					//DisplayMessage(szMsg);
					m_lCurNumOfPressCount++; //20150416
					if (m_lNumOfPressBeforeAlert != 0 && m_lCurNumOfPressCount >= m_lNumOfPressBeforeAlert 
						&& !m_bChangeRubberBandAlertShown)
					{
						m_bChangeRubberBandAlertShown = TRUE;
						//SetAlert(IDS_ACF1_CHANGE_RUBBER_BAND_ERR + (1000 * m_lCurrWH));
						SetAlert(IDS_ACF1_CHANGE_RUBBER_BAND_ERR);
					}
					pAppMod->m_smfMachine[GetStnName()]["Para"]["m_lCurNumOfPressCount"]			= m_lCurNumOfPressCount;
					//pAppMod->m_smfMachine.Update();

					m_pCACFWH->m_stACF[m_pCACFWH->m_lCurACF].bBonded = TRUE;
					m_qSubOperation = ACF_STRIPPER_ON_Q;	// go to 3
				}
				else
				{
					// ERROR!
					m_qSubOperation = ACF_STOP_Q;	// go to 7
					WriteHMIMess("*** ERROR: ACF Sol Error ***", TRUE);
					m_bModError = TRUE;
					SetError(m_nLastError);
				}
			}
		}
		break;

	case ACF_STRIPPER_ON_Q:	// 3
// 20150204		if ((m_nLastError = SetStripperSol(ON, GMP_WAIT)) == GMP_SUCCESS)
// 20150204		{
		if ((m_nLastError = SyncStripperX()) == GMP_SUCCESS)
		{
			if(m_lCurrWH == ACFWH_1)
			{
				ACF_Status[m_lCurSlave] = ST_ACFWH1_DONE;
			}
			else
			{
				ACF_Status[m_lCurSlave] = ST_ACFWH2_DONE;
			}
			m_qSubOperation = ACF_STRIPPER_OFF_Q;	// go to 4
		}
		else
		{
			// ERROR!
			m_qSubOperation = ACF_STOP_Q;	// go to 6
			WriteHMIMess("*** ERROR: ACF StripperX Error ***", TRUE);
			m_bModError = TRUE;
			SetError(m_nLastError);
		}
		break;

	case ACF_STRIPPER_OFF_Q:		// 4
		if (ACF_WH_Status[m_lCurrWH] == ST_STOP)
		{
			MoveStripperX(ACF_STRIPPER_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
			m_qSubOperation = ACF_STOP_Q;	// go to 7
		}
		else if (
				 ACF_WH_Status[m_lCurrWH] != ST_BONDING ||
				 ACF_WH_Status[m_lCurrWH] == ST_MOD_NOT_SELECTED 
				)
		{
			//LogAction("ACF_Status = ST_IDLE");
//			ACF_Status[m_lCurSlave] = ST_IDLE;	// faster faster	// 20141106 Yip: Wait At Unload Position Until ACF Indexed And Being Cut

			if (
				((m_nLastError = MoveStripperX(ACF_STRIPPER_X_CTRL_GO_STANDBY_POSN, GMP_WAIT)) == GMP_SUCCESS) &&	// 20150212
				//((m_nLastError = MoveZ(ACF_Z_CTRL_GO_CALIB_LEVEL, GMP_NOWAIT)) == GMP_SUCCESS) &&	
				((m_nLastError = MoveX(ACF_X_CTRL_GO_STANDBY_POSN, GMP_WAIT)) == GMP_SUCCESS) &&	
// 20150204				((m_nLastError = SetStripperSol(OFF, GMP_NOWAIT)) == GMP_SUCCESS) &&
				((m_nLastError = SyncRoller()) == GMP_SUCCESS)
			   )
			{
				ACF_Status[m_lCurSlave] = ST_BUSY;
				m_qSubOperation = ACF_INDEX_OPERATION_Q;	// go to 5
			}
			else
			{
				// ERROR!
				m_qSubOperation = ACF_STOP_Q;	// go to 7
				WriteHMIMess("*** ERROR: ACF Sol Error ***", TRUE);
				m_bModError = TRUE;
				SetError(m_nLastError);
			}
		}
		break;

	case ACF_INDEX_OPERATION_Q:	// 5
		CalcIndexedLength();

		if (m_bValidIndexedLength)
		{
			dTotalLength = m_dCutterToHeadDistance + m_dCutterToHeadOffset + m_pCACFWH->m_stACF[m_emACFIndexed[m_nIndexLast]].dLength;
			dAllowedLength = dTotalLength - m_dIndexedLength;
			dRelativeLength = m_pCACFWH->m_stACF[GetNextArrayACFNum()].dLength - m_dAdvancedLength;

			// Debug
			if (dRelativeLength < 0)
			{
				CString szMsg = "";
				szMsg.Format("dRelativeLength = %.2f, Next ACF Length = %.2f, m_dAdvancedLength = %.2f", dRelativeLength, m_pCACFWH->m_stACF[GetNextArrayACFNum()].dLength, m_dAdvancedLength);
				//DisplayMessage(szMsg);
			}

			// Debug
			//szMsg.Format("BEFORE Index: [%.2f - %.2f)(%.2f][...][%.2f][%.2f]", 
			//g_stACF[GetNextArrayACFNum()].dLength, m_dAdvancedLength, m_dAdvancedLength, g_stACF[m_emACFIndexed[m_nIndexLast]].dLength, m_dRemainingLength);
			//DisplayMessage(szMsg);

			// Debug
			if (HMI_bDebugCal)
			{
				szMsg.Format("Allowed Length: %.2f", dAllowedLength);
				LogAction(szMsg);
				//DisplayMessage(szMsg);
			}

			if (m_pCACFWH->m_stACF[GetNextArrayACFNum()].dLength - m_dAdvancedLength <= dAllowedLength)
			{
				// Index IndexFirst to Cutter
				if (
					InsertACFArray() &&
					((m_nLastError = MoveRelativeIndexer(-1.0 * dRelativeLength * 1000 * m_stMotorIndex.stAttrib.dDistPerCount * m_dIndexerFactor * m_dIndexerCustomFactor, GMP_WAIT)) == GMP_SUCCESS) //20120706
				   )
				{
					//if (IsDancingRollerLowerSnrOn())
					//{
					//	m_qSubOperation = ACF_STOP_Q;	// go to 7
					//	WriteHMIMess("*** ERROR: End of ACF Tape ***", TRUE);
					//	m_bModError = TRUE;
					//	SetError(IDS_ACF_TAPE_END_ERR);
					//}
					//// tighten ACF
					//else 
					//{
						// cut ACF
					if (
						((m_nLastError = SetHalfCutterOnSol(ON, GMP_WAIT)) == GMP_SUCCESS) && 
						((m_nLastError = SetHalfCutterOnSol(OFF, GMP_WAIT)) == GMP_SUCCESS)
					   )
					{
						m_dAdvancedLength = 0.0;
						m_qSubOperation = ACF_INDEX_OPERATION_Q;	// 5
					}
					else //20130322
					{ 
						//error cutter down sensor not on
						SetError(m_nLastError);
					}
					//}
				}
				else
				{
					// ERROR!
					m_qSubOperation = ACF_STOP_Q;	// go to 7
					WriteHMIMess("*** ERROR: Insert ACF Array Error ***", TRUE);
					m_bModError = TRUE;
					SetError(IDS_ACF1_INVALID_ACF_ERR);
				}

				// Debug
				//szMsg.Format("AFTER Index: [%.2f - %.2f)(%.2f][%.2f][...][%.2f][%.2f]", 
				//g_stACF[GetNextArrayACFNum()].dLength, m_dAdvancedLength, m_dAdvancedLength, g_stACF[m_emACFIndexed[(m_nIndexFirst-1+MAX_SIZE_OF_ACF_ARRAY) % MAX_SIZE_OF_ACF_ARRAY]].dLength, g_stACF[m_emACFIndexed[m_nIndexLast]].dLength, m_dRemainingLength);
				//DisplayMessage(szMsg);

				//m_qSubOperation = ACF_INDEX_OPERATION_Q;	// 5
			}
			else
			{
				m_qSubOperation = ACF_INDEX_LAST_OPERATION_Q;	// 6
			}
		}
		else
		{
			// ERROR!
			m_qSubOperation = ACF_STOP_Q;	// go to 7
			WriteHMIMess("*** ERROR: Invalid ACF Indexed Length Error ***", TRUE);
			m_bModError = TRUE;
			SetError(IDS_ACF1_INVALID_ACF_INDEXED_LENGTH_ERR);
		}
		break;

	case ACF_INDEX_LAST_OPERATION_Q:	// 6
		// Index IndexLast just completely under the Head
		CalcIndexedLength();

		if (m_bValidIndexedLength)
		{				
			//if (m_bRewindACF)
			//{
			if (
				((m_nLastError = MoveRelativeIndexer(-1.0 * m_dRemainingLength * 1000 * m_stMotorIndex.stAttrib.dDistPerCount * m_dIndexerFactor * m_dIndexerCustomFactor, GMP_NOWAIT)) == GMP_SUCCESS) && //20120706
				((m_nLastError = MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, GMP_NOWAIT)) == GMP_SUCCESS) && 
				((m_nLastError = SyncIndex()) == GMP_SUCCESS) &&
				((m_nLastError = SyncRoller()) == GMP_SUCCESS)
			   )
			{
				m_dRemainingLength = 0.0;
				ACF_Status[m_lCurSlave] = ST_IDLE;	// 20141106 Yip: Wait At Unload Position Until ACF Indexed And Being Cut
				m_qSubOperation = ACF_WAIT_REQUEST_Q;	// go to 1

					// Debug
					//szMsg.Format("FINISHED Index: [%.2f - %.2f)(%.2f][...][%.2f][%.2f]", 
					//g_stACF[GetNextArrayACFNum()].dLength, m_dAdvancedLength, m_dAdvancedLength, g_stACF[m_emACFIndexed[m_nIndexLast]].dLength, m_dRemainingLength);
					//DisplayMessage(szMsg);
			}
				//else
				//{
				//	m_qSubOperation = ACF_STOP_Q;	// go to 7
				//	m_bModError = TRUE;
				//	if (m_nLastError == IDS_ACF_AUX_CYLINDER_NOT_DOWN_ERR)
				//	{
				//		SetError(IDS_ACF_AUX_CYLINDER_NOT_DOWN_ERR);
				//	}
				//}
			//}
		}
		else
		{
			// ERROR!
			m_qSubOperation = ACF_STOP_Q;	// go to 7
			WriteHMIMess("*** ERROR: Invalid ACF Indexed Length Error ***", TRUE);
			m_bModError = TRUE;
			SetError(IDS_ACF1_INVALID_ACF_INDEXED_LENGTH_ERR);
		}
		break;

	case ACF_STOP_Q:
		break;
	}
	
	if (m_qPreviousSubOperation != m_qSubOperation)
	{
		if (HMI_bDebugSeq)
		{
			CString szMsg;
			if (ACF_STOP_Q != m_qSubOperation)
			{
				szMsg.Format("%s Go State: %ld", GetStnName(), m_qSubOperation);
			}
			else
			{
				szMsg.Format("%s Go Stop", GetStnName());
			}
			//DisplayMessage(szMsg);
		}
		UpdateDebugSeq(GetSequenceString(), ACF_Status[m_lCurSlave]);
		m_qPreviousSubOperation = m_qSubOperation;

		if (HMI_bDebugSeq)
		{
			PRINT_DATA	stPrintData;
			for (INT i = 0; i < MAX_PRINT_DATA; i++)
			{
				stPrintData.szTitle[i]	= "";
			}

			stPrintData.szTitle[0].Format("%ld", GetTickCount());
			//stPrintData.szTitle[GetStationID()* MAX_STATION_SEQ_DATA + 1].Format("%s", GetSequenceString());
			stPrintData.szTitle[GetStationID() * MAX_STATION_SEQ_DATA + 2].Format("%ld", ACF_Status[m_lCurSlave]);

			LogFile->log(stPrintData.szTitle);

		}
	}

}

/////////////////////////////////////////////////
// ACF length longer than CutterToHead Distance
/////////////////////////////////////////////////
VOID CACF::OperationSequence_Single_ACF()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	m_nLastError = GMP_SUCCESS;

	CString szMsg = "";

	m_nLastError = GMP_SUCCESS;
	DOUBLE dAllowedLength = 0.0;
	DOUBLE dTotalLength = 0.0;
	DOUBLE dRelativeLength = 0.0;

	switch (m_qSubOperation)
	{
	case ACF_IDLE_Q: // 0
		if ((m_nLastError = SyncRoller()) == GMP_SUCCESS)
		{
			m_qSubOperation = ACF_WAIT_REQUEST_Q;	// go to 1
		}
		break;

	case ACF_WAIT_REQUEST_Q:		// 1
		if (m_bStopCycle)
		{
			m_qSubOperation = ACF_STOP_Q;
		}
		else if (ACF_WH_Status[ACFWH_1] == ST_STOP && ACF_WH_Status[ACFWH_2] == ST_STOP)
		{
			m_qSubOperation = ACF_STOP_Q;	// go to 99
		}
		else if (
				 ACF_WH_Status[m_lCurrWH] == ST_REQ_BONDING || 
				(ACF_WH_Status[ACFWH_1] == ST_MOD_NOT_SELECTED && ACF_WH_Status[ACFWH_2] == ST_MOD_NOT_SELECTED)
				)
		{
			//if (
			//	//((m_nLastError = SetHeadUpSol(ON, GMP_WAIT)) == GMP_SUCCESS) &&
			//	//((m_nLastError = SetCounterBalanceSol(ON, GMP_WAIT)) == GMP_SUCCESS) &&
			//	((m_nLastError = SetDACValue(m_dPressureCtrl)) == GMP_SUCCESS)
			//   )
			
			{
				if(m_lCurrWH == ACFWH_1)
				{
					ACF_Status[m_lCurSlave] = ST_ACFWH1_REQ_ACF_ACK;	//Grant Permission to ACFWH1
				}
				else if(m_lCurrWH == ACFWH_2)
				{
					ACF_Status[m_lCurSlave] = ST_ACFWH2_REQ_ACF_ACK;	//Grant Permission to ACFWH2
				}

				m_qSubOperation = ACF_BOND_OPERATION_Q;	// go to 2
			}

			//else
			//{
			//	// ERROR!
			//	m_qSubOperation = ACF_STOP_Q;	// go to 7
			//	WriteHMIMess("*** ERROR: ACF Sol Error ***", TRUE);
			//	m_bModError = TRUE;
			//	SetError(m_nLastError);
			//}
		}
		else
		{
			if(ACF_WH_Status[ACFWH_1] == ST_MOD_NOT_SELECTED)
			{
				m_lCurrWH = ACFWH_2;
			}
			else if(ACF_WH_Status[ACFWH_2] == ST_MOD_NOT_SELECTED)
			{
				m_lCurrWH = ACFWH_1;
			}
			else
			{
				m_lCurrWH = (m_lCurrWH + 1) % MAX_NUM_OF_ACFWH;	//Check Next ACFWH
			}
		}
		break;

	case ACF_BOND_OPERATION_Q:	// 2
		if (ACF_WH_Status[m_lCurrWH] == ST_STOP)
		{
			m_qSubOperation = ACF_STOP_Q;	// go to 7
		}
		else if (
				 ACF_WH_Status[m_lCurrWH] == ST_BONDING || 
				 ACF_WH_Status[m_lCurrWH] == ST_MOD_NOT_SELECTED 
				)
		{
			CString szMsg = "";
			if (HMI_bDebugCal)
			{
				szMsg.Format("ACF Head:%ld, ACF WH Request:%ld", (LONG)(m_emACFIndexed[m_nIndexLast] + 1), m_pCACFWH->m_lCurACF + 1);
				LogAction(szMsg);
				//DisplayMessage(szMsg);
			}

			// check ACF indexed = requested ACF by WH?
			if (
				//((m_pCACFWH->m_stACF[m_emACFIndexed[m_nIndexLast]].dLength) != m_pCACFWH->m_stACF[(ACFNum)m_pCACFWH->m_lCurACF].dLength) && 
				ACF_WH_Status[m_lCurrWH] == ST_MOD_NOT_SELECTED
			   )
			{
				//ERROR! Invalid ACF
				m_qSubOperation = ACF_STOP_Q;	// go to 7
				WriteHMIMess("*** ERROR: Invalid ACF ***", TRUE);
				m_bModError = TRUE;
				SetError(IDS_ACF1_INVALID_ACF_ERR);
				
				// Debug
				//if (HMI_bDebugCal)
				//{
				//	szMsg.Format("ACF Head:%ld, ACF WH Request:%ld", (LONG)(m_emACFIndexed[m_nIndexLast]+1), pCACF_WH->m_lCurACF+1);
				//	LogAction(szMsg);
				//	//DisplayMessage(szMsg);
				//}
			}
#ifdef ACF_TAPE_END_SNR
			else if ((AutoMode == AUTO_BOND || AutoMode == TEST_BOND) && IsTapeEndSnrOn()) //20121122
			{
				//ERROR! ACF Tape End Error
				m_qSubOperation = ACF_STOP_Q;	// go to 7
				WriteHMIMess("*** ERROR: ACF Tape End Error ***", TRUE);
				m_bModError = TRUE;
				//SetError(IDS_ACF1_TAPE_END_ERR + (m_lCurSlave * CONVERT_TO_MAPPING));
				SetError(IDS_ACF1_TAPE_END_ERR);
			}
#endif
			else
			{

			//	RemoveACFArray();

				if (
					((m_nLastError = MoveBF(ACF_BF_CTRL_DAC_ON, GMP_NOWAIT)) == GMP_SUCCESS) &&
					//((m_nLastError = MoveZ(ACF_Z_CTRL_GO_SEARCH_LEVEL, GMP_WAIT)) == GMP_SUCCESS) &&
					((m_nLastError = MoveZ(ACF_Z_CTRL_GO_BOND_LEVEL, GMP_WAIT)) == GMP_SUCCESS) &&
					//((m_nLastError = SleepWithReturn(m_lPreAttachDelay)) == GMP_SUCCESS) &&
					((m_nLastError = SleepWithReturn(m_lAttachACFDelay)) == GMP_SUCCESS) &&
					((m_nLastError = MoveBF(ACF_BF_CTRL_OFF, GMP_NOWAIT)) == GMP_SUCCESS) &&
					((m_nLastError = MoveZ(ACF_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) == GMP_SUCCESS) &&
					((m_nLastError = MoveStripperX(ACF_STRIPPER_X_CTRL_GO_OUT_OFFSET, GMP_NOWAIT)) == GMP_SUCCESS)
				   )
				{
					m_lCurNumOfPressCount++; //20150416
					if (m_lNumOfPressBeforeAlert != 0 && m_lCurNumOfPressCount >= m_lNumOfPressBeforeAlert 
						&& !m_bChangeRubberBandAlertShown)
					{
						m_bChangeRubberBandAlertShown = TRUE;
						//SetAlert(IDS_ACF1_CHANGE_RUBBER_BAND_ERR + (1000 * m_lCurSlave));
						SetAlert(IDS_ACF1_CHANGE_RUBBER_BAND_ERR);
					}
					pAppMod->m_smfMachine[GetStnName()]["Para"]["m_lCurNumOfPressCount"]			= m_lCurNumOfPressCount;
					//pAppMod->m_smfMachine.Update();

					m_pCACFWH->m_stACF[m_pCACFWH->m_lCurACF].bBonded = TRUE;
					m_qSubOperation = ACF_STRIPPER_ON_Q;	// go to 3
				}
				else
				{
					// ERROR!
					m_qSubOperation = ACF_STOP_Q;	// go to 7
					WriteHMIMess("*** ERROR: ACF Sol Error ***", TRUE);
					m_bModError = TRUE;
					SetError(m_nLastError);
				}
			}
		}
		break;

	case ACF_STRIPPER_ON_Q:	// 3
// 20150204		if ((m_nLastError = SetStripperSol(ON, GMP_WAIT)) == GMP_SUCCESS)
// 20150204		{
		if ((m_nLastError = SyncStripperX()) == GMP_SUCCESS)		
		{
			if(m_lCurrWH == ACFWH_1)
			{
				ACF_Status[m_lCurSlave] = ST_ACFWH1_DONE;
			}
			else
			{
				ACF_Status[m_lCurSlave] = ST_ACFWH2_DONE;
			}
			m_qSubOperation = ACF_STRIPPER_OFF_Q;	// go to 4
		}
		else
		{
			// ERROR!
			m_qSubOperation = ACF_STOP_Q;	// go to 6
			WriteHMIMess("*** ERROR: ACF StripperX Error ***", TRUE);
			m_bModError = TRUE;
			SetError(m_nLastError);
		}
		break;

	case ACF_STRIPPER_OFF_Q:		// 4
		if (ACF_WH_Status[m_lCurrWH] == ST_STOP)
		{
			MoveStripperX(ACF_STRIPPER_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
			m_qSubOperation = ACF_STOP_Q;	// go to 7
		}
		else if (
				 ACF_WH_Status[m_lCurrWH] != ST_BONDING ||
				 ACF_WH_Status[m_lCurrWH] == ST_MOD_NOT_SELECTED 
				)
		{
			//LogAction("ACF_Status = ST_IDLE");
//			ACF_Status[m_lCurSlave] = ST_IDLE;	// faster faster	// 20141106 Yip: Wait At Unload Position Until ACF Indexed And Being Cut

			if (
				((m_nLastError = MoveStripperX(ACF_STRIPPER_X_CTRL_GO_STANDBY_POSN, GMP_WAIT)) == GMP_SUCCESS) &&
// 20150204				((m_nLastError = SetStripperSol(OFF, GMP_NOWAIT)) == GMP_SUCCESS) &&
				((m_nLastError = SyncRoller()) == GMP_SUCCESS)
			   )
			{
				ACF_Status[m_lCurSlave] = ST_BUSY;
				m_qSubOperation = ACF_INDEX_OPERATION_Q;	// go to 5
			}
			else
			{
				// ERROR!
				m_qSubOperation = ACF_STOP_Q;	// go to 7
				WriteHMIMess("*** ERROR: ACF Sol Error ***", TRUE);
				m_bModError = TRUE;
				SetError(m_nLastError);
			}
		}
		break;

	case ACF_INDEX_OPERATION_Q:	// 5
		//CalcIndexedLength();
		
		m_bValidIndexedLength = TRUE;

		if (m_bValidIndexedLength)
		{
			//dTotalLength = m_dCutterToHeadDistance + m_dCutterToHeadOffset + m_pCACFWH->m_stACF[m_emACFIndexed[m_nIndexLast]].dLength;
			//dAllowedLength = dTotalLength - m_dIndexedLength;
			//dRelativeLength = m_pCACFWH->m_stACF[GetNextArrayACFNum()].dLength - m_dAdvancedLength;

			m_dAdvancedLength = m_dCutterToHeadDistance + m_dCutterToHeadOffset;

			//dTotalLength = m_dCutterToHeadDistance + m_dCutterToHeadOffset + m_pCACFWH->m_stACF[ACF1].dLength;
			dAllowedLength = m_pCACFWH->m_stACF[ACF1].dLength;
			dRelativeLength = m_pCACFWH->m_stACF[ACF1].dLength - m_dAdvancedLength;

			// Debug
			if (dRelativeLength < 0)
			{
				CString szMsg = "";
				szMsg.Format("dRelativeLength = %.2f, Next ACF Length = %.2f, m_dAdvancedLength = %.2f", dRelativeLength, m_pCACFWH->m_stACF[GetNextArrayACFNum()].dLength, m_dAdvancedLength);
				//DisplayMessage(szMsg);
			}

			// Debug
			//szMsg.Format("BEFORE Index: [%.2f - %.2f)(%.2f][...][%.2f][%.2f]", 
			//g_stACF[GetNextArrayACFNum()].dLength, m_dAdvancedLength, m_dAdvancedLength, g_stACF[m_emACFIndexed[m_nIndexLast]].dLength, m_dRemainingLength);
			//DisplayMessage(szMsg);

			// Debug
			if (HMI_bDebugCal)
			{
				szMsg.Format("Allowed Length: %.2f", dAllowedLength);
				LogAction(szMsg);
				//DisplayMessage(szMsg);
			}

			if (dRelativeLength <= dAllowedLength)
			{
				// Index IndexFirst to Cutter
				if (
					//InsertACFArray() &&
					((m_nLastError = MoveRelativeIndexer(-1.0 * dRelativeLength * 1000 * m_stMotorIndex.stAttrib.dDistPerCount * m_dIndexerFactor * m_dIndexerCustomFactor, GMP_WAIT)) == GMP_SUCCESS) //20120706
				   )
				{
					//if (IsDancingRollerLowerSnrOn())
					//{
					//	m_qSubOperation = ACF_STOP_Q;	// go to 7
					//	WriteHMIMess("*** ERROR: End of ACF Tape ***", TRUE);
					//	m_bModError = TRUE;
					//	SetError(IDS_ACF_TAPE_END_ERR);
					//}
					//// tighten ACF
					//else 
					//{
						// cut ACF
					if (
						((m_nLastError = SetHalfCutterOnSol(ON, GMP_WAIT)) == GMP_SUCCESS) && 
						((m_nLastError = SetHalfCutterOnSol(OFF, GMP_WAIT)) == GMP_SUCCESS)
					   )
					{
						m_dAdvancedLength = 0.0;
						//	m_qSubOperation = ACF_INDEX_OPERATION_Q;	// 5
						m_qSubOperation = ACF_INDEX_LAST_OPERATION_Q;	// 6
					}
					else //20130322
					{ 
						//error cutter down sensor not on
						SetError(m_nLastError);
					}
					//}
				}
				else
				{
					// ERROR!
					m_qSubOperation = ACF_STOP_Q;	// go to 7
					WriteHMIMess("*** ERROR: Insert ACF Array Error ***", TRUE);
					m_bModError = TRUE;
					SetError(IDS_ACF1_INVALID_ACF_ERR);
				}

				// Debug
				//szMsg.Format("AFTER Index: [%.2f - %.2f)(%.2f][%.2f][...][%.2f][%.2f]", 
				//g_stACF[GetNextArrayACFNum()].dLength, m_dAdvancedLength, m_dAdvancedLength, g_stACF[m_emACFIndexed[(m_nIndexFirst-1+MAX_SIZE_OF_ACF_ARRAY) % MAX_SIZE_OF_ACF_ARRAY]].dLength, g_stACF[m_emACFIndexed[m_nIndexLast]].dLength, m_dRemainingLength);
				//DisplayMessage(szMsg);

				//m_qSubOperation = ACF_INDEX_OPERATION_Q;	// 5
			}
			else
			{
				m_qSubOperation = ACF_INDEX_LAST_OPERATION_Q;	// 6
			}
		}
		//else //klocwork fix 20121211
		//{
		//	// ERROR!
		//	m_qSubOperation = ACF_STOP_Q;	// go to 7
		//	WriteHMIMess("*** ERROR: Invalid ACF Indexed Length Error ***", TRUE);
		//	m_bModError = TRUE;
		//	SetError(IDS_ACF1_INVALID_ACF_INDEXED_LENGTH_ERR + (m_lCurSlave * CONVERT_TO_MAPPING));
		//}
		break;

	case ACF_INDEX_LAST_OPERATION_Q:	// 6
		// Index IndexLast just completely under the Head
		//CalcIndexedLength();

		m_dRemainingLength = m_dCutterToHeadDistance + m_dCutterToHeadOffset;
		m_bValidIndexedLength = TRUE;

		if (m_bValidIndexedLength)
		{				
			//if (m_bRewindACF)
			//{
			if (
				((m_nLastError = MoveRelativeIndexer(-1.0 * m_dRemainingLength * 1000 * m_stMotorIndex.stAttrib.dDistPerCount * m_dIndexerFactor * m_dIndexerCustomFactor, GMP_NOWAIT)) == GMP_SUCCESS) && //20120706
				((m_nLastError = MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, GMP_NOWAIT)) == GMP_SUCCESS) && 
				((m_nLastError = SyncIndex()) == GMP_SUCCESS) &&
				((m_nLastError = SyncRoller()) == GMP_SUCCESS)
			   )
			{
				m_dRemainingLength = 0.0;
				ACF_Status[m_lCurSlave] = ST_IDLE;	// 20141106 Yip: Wait At Unload Position Until ACF Indexed And Being Cut
				m_qSubOperation = ACF_WAIT_REQUEST_Q;	// go to 1

					// Debug
					//szMsg.Format("FINISHED Index: [%.2f - %.2f)(%.2f][...][%.2f][%.2f]", 
					//g_stACF[GetNextArrayACFNum()].dLength, m_dAdvancedLength, m_dAdvancedLength, g_stACF[m_emACFIndexed[m_nIndexLast]].dLength, m_dRemainingLength);
					//DisplayMessage(szMsg);
			}
				//else
				//{
				//	m_qSubOperation = ACF_STOP_Q;	// go to 7
				//	m_bModError = TRUE;
				//	if (m_nLastError == IDS_ACF_AUX_CYLINDER_NOT_DOWN_ERR)
				//	{
				//		SetError(IDS_ACF_AUX_CYLINDER_NOT_DOWN_ERR);
				//	}
				//}
			//}
		}
		//else //klocwork fix 20121211
		//{
		//	// ERROR!
		//	m_qSubOperation = ACF_STOP_Q;	// go to 7
		//	WriteHMIMess("*** ERROR: Invalid ACF Indexed Length Error ***", TRUE);
		//	m_bModError = TRUE;
		//	SetError(IDS_ACF1_INVALID_ACF_INDEXED_LENGTH_ERR + (m_lCurSlave * CONVERT_TO_MAPPING));
		//}
		break;

	case ACF_STOP_Q:
		break;
	}
	
	if (m_qPreviousSubOperation != m_qSubOperation)
	{
		if (HMI_bDebugSeq)
		{
			CString szMsg;
			if (ACF_STOP_Q != m_qSubOperation)
			{
				szMsg.Format("%s Go State: %ld", GetStnName(), m_qSubOperation);
			}
			else
			{
				szMsg.Format("%s Go Stop", GetStnName());
			}
			//DisplayMessage(szMsg);
		}
		UpdateDebugSeq(GetSequenceString(), ACF_Status[m_lCurSlave]);
		m_qPreviousSubOperation = m_qSubOperation;

		if (HMI_bDebugSeq)
		{
			PRINT_DATA	stPrintData;
			for (INT i = 0; i < MAX_PRINT_DATA; i++)
			{
				stPrintData.szTitle[i]	= "";
			}

			stPrintData.szTitle[0].Format("%ld", GetTickCount());
			//stPrintData.szTitle[GetStationID()* MAX_STATION_SEQ_DATA + 1].Format("%s", GetSequenceString());
			stPrintData.szTitle[GetStationID() * MAX_STATION_SEQ_DATA + 2].Format("%ld", ACF_Status[m_lCurSlave]);

			LogFile->log(stPrintData.szTitle);

		}
	}
}


//////////////////////////////////////////////
/////////////////////////////////////////////
INT CACF::SyncX()
{
	return m_stMotorX.Sync();
}

INT CACF::SyncStripperX()
{
	return m_stMotorStripperX.Sync();
}

INT CACF::SyncRoller()
{
	return m_stMotorRoller.Sync();
}

INT CACF::SyncIndex()
{
	return m_stMotorIndex.Sync();
}

INT CACF::MoveZ(ACF_Z_CTRL_SIGNAL ctrl, BOOL bWait)
{
	DOUBLE	dThickness = 0;
	LONG	lMtrCnt	= 0;
	DOUBLE	dDistPerCnt = m_stMotorZ.stAttrib.dDistPerCount;
	INT		nResult = GMP_SUCCESS;

	SRCH_PROFILE	stSrchProf;
	short			sProcListSaveID = -1;

	typedef enum
	{
		NO_MOTION = -1,
		MOVE_MTR_AXIS,
		SRCH_MTR_AXIS,

	} MoveMode;

	LONG lMoveMode = NO_MOTION;

	switch (ctrl)
	{

	case ACF_Z_CTRL_GO_STANDBY_LEVEL:
		lMoveMode = MOVE_MTR_AXIS;
		lMtrCnt = m_lStandbyLevel;
		break;

	case ACF_Z_CTRL_GO_CALIB_LEVEL:
		lMoveMode = MOVE_MTR_AXIS;
		lMtrCnt = m_lCalibLevel;
		break;

	case ACF_Z_CTRL_GO_BOND_LEVEL:
		lMoveMode = MOVE_MTR_AXIS;
		lMtrCnt = m_lBondLevel;
		break;

	case ACF_Z_CTRL_GO_SEARCH_LEVEL:
		lMoveMode = MOVE_MTR_AXIS;
	//???	lMtrCnt = m_lCalibLevel - DistanceToCount(1000.0 * m_stSrchBondLevel.dSrchLimit / 2.0), dDistPerCnt);
		break;
	
	case ACF_Z_CTRL_SRCH_CONTACT_LIMIT:
		
		if (IsBurnInDiagBond()) //no srch action
		{
			Sleep(100);
			return GMP_SUCCESS;
		}

		lMoveMode = SRCH_MTR_AXIS;
		
		//slow motion SRCH under srch lmt over. No contact snr in Z
		//srch lmt level is calculated from Bond level.

		break;

	case ACF_Z_CTRL_SRCH_CALIB_LEVEL:
		
			lMoveMode = SRCH_MTR_AXIS;
			m_stMotorZ.stUsrDefProfile[ACF_Z_SRCH_CONTACT_SNR_ON_LEVEL].stProf = m_stSrchDefault;
			m_stMotorZ.SetSrchProfSi(m_stMotorZ.stUsrDefProfile[ACF_Z_SRCH_CONTACT_SNR_ON_LEVEL].sSrchProfID, m_stSrchDefault);
			stSrchProf = m_stMotorZ.stUsrDefProfile[ACF_Z_SRCH_CONTACT_SNR_ON_LEVEL];

		break;


	case ACF_Z_CTRL_GO_FORCE_CALIB_BOND_LEVEL:
		lMoveMode = MOVE_MTR_AXIS;
		lMtrCnt = m_lCalibLevel - DistanceToCount(1000.0 * (g_stForceCalibrator.dHeight + (m_stSrchForceCalibBondLevel.dSrchLimit / 2.0)), dDistPerCnt);
		break;

	case ACF_Z_CTRL_GO_SRCH_FORCE_CALIB_BOND_CONTACT_LEVEL:
		if (
			m_stSrchForceCalibBondLevel.dSrchLimit != 0.0 &&
			m_stSrchForceCalibBondLevel.dSrchVel != 0.0
		   )
		{
			lMoveMode = SRCH_MTR_AXIS;
			m_stMotorZ.stUsrDefProfile[ACF_Z_SRCH_CONTACT_SNR_ON_LEVEL].stProf = m_stSrchForceCalibBondLevel;
			m_stMotorZ.SetSrchProfSi(m_stMotorZ.stUsrDefProfile[ACF_Z_SRCH_CONTACT_SNR_ON_LEVEL].sSrchProfID, m_stSrchForceCalibBondLevel);
			stSrchProf = m_stMotorZ.stUsrDefProfile[ACF_Z_SRCH_CONTACT_SNR_ON_LEVEL];
		}
		break;

	case ACF_Z_CTRL_GO_SRCH_FORCE_CALIB_BOND_CONTACT_LEVEL_AND_PRESS:
		if (
			m_stSrchForceCalibBondLevel.dSrchLimit != 0.0 &&
			m_stSrchForceCalibBondLevel.dSrchVel != 0.0
		   )
		{
			lMoveMode = SRCH_MTR_AXIS;
			m_stMotorZ.stUsrDefProfile[ACF_Z_SRCH_CONTACT_SNR_ON_LEVEL].stProf = m_stSrchForceCalibBondLevel;
			m_stMotorZ.SetSrchProfSi(m_stMotorZ.stUsrDefProfile[ACF_Z_SRCH_CONTACT_SNR_ON_LEVEL].sSrchProfID, m_stSrchForceCalibBondLevel);
			stSrchProf = m_stMotorZ.stUsrDefProfile[ACF_Z_SRCH_CONTACT_SNR_ON_LEVEL];
		//	sProcListSaveID = PROCESS_HEAD;
		}
		break;


	case ACF_Z_CTRL_GO_SRCH_FORCE_CALIB_POSTBOND_CONTACT_LEVEL:
		if (
			m_stSrchPostBondLevel.dSrchLimit != 0.0 &&
			m_stSrchPostBondLevel.dSrchVel != 0.0
		   )
		{
			lMoveMode = SRCH_MTR_AXIS;
			m_stMotorZ.stUsrDefProfile[ACF_Z_SRCH_CONTACT_SNR_OFF_LEVEL].stProf = m_stSrchPostBondLevel;
			m_stMotorZ.SetSrchProfSi(m_stMotorZ.stUsrDefProfile[ACF_Z_SRCH_CONTACT_SNR_OFF_LEVEL].sSrchProfID, m_stSrchPostBondLevel);
			stSrchProf = m_stMotorZ.stUsrDefProfile[ACF_Z_SRCH_CONTACT_SNR_OFF_LEVEL];
		}
		break;

	//case MAINBOND_Z_CTRL_GO_TYPE1_BOND_LEVEL:
	//	lMoveMode = MOVE_MTR_AXIS;
	//	lMtrCnt = m_lCalibLevel - (LONG)(m_stMotorZ.stAttrib.dDistPerCount * (g_stGlass.dThickness3 + g_stLSIType[LSI_TYPE1].dThickness)) + m_lBondLevelOffset[LSI_TYPE1] - DistanceToCount(1000 * m_stSrchBondLevel.dSrchLimit / 2, m_stMotorZ.stAttrib.dDistPerCount);
	//	break;


	case ACF_Z_CTRL_GO_SRCH_POSTBOND_CONTACT_LEVEL:
		if (
			m_stSrchPostBondLevel.dSrchLimit != 0.0 &&
			m_stSrchPostBondLevel.dSrchVel != 0.0
		   )
		{
			lMoveMode = SRCH_MTR_AXIS;
			m_stMotorZ.stUsrDefProfile[ACF_Z_SRCH_CONTACT_SNR_OFF_LEVEL].stProf = m_stSrchPostBondLevel;
			m_stMotorZ.SetSrchProfSi(m_stMotorZ.stUsrDefProfile[ACF_Z_SRCH_CONTACT_SNR_OFF_LEVEL].sSrchProfID, m_stSrchPostBondLevel);
			stSrchProf = m_stMotorZ.stUsrDefProfile[ACF_Z_SRCH_CONTACT_SNR_OFF_LEVEL];
		}
		break;


	//case MAINBOND_Z_CTRL_GO_DEFAULT_LEFT_SRCH_CALIB_CONTACT_LEVEL:	// Default All Search Setting
	//	if (
	//		m_stSrchForceCalibBondLevel.dSrchLimit != 0.0 &&
	//		m_stSrchForceCalibBondLevel.dSrchVel != 0.0
	//	   )
	//	{
	//		lMoveMode = SRCH_MTR_AXIS;
	//		m_stMotorZ.stUsrDefProfile[MAINBOND_Z_SRCH_LEFT_CONTACT_SNR_ON_LEVEL].stProf = m_stSrchForceCalibBondLevel;
	//		m_stMotorZ.SetSrchProfSi(m_stMotorZ.stUsrDefProfile[MAINBOND_Z_SRCH_LEFT_CONTACT_SNR_ON_LEVEL].sSrchProfID, m_stSrchForceCalibBondLevel);
	//		stSrchProf = m_stMotorZ.stUsrDefProfile[MAINBOND_Z_SRCH_LEFT_CONTACT_SNR_ON_LEVEL];
	//	}
	//	break;

//#ifdef MB_FIND_CONTACT_LEVEL_DIFF //20130615
//	case MAINBOND_Z_CTRL_SRCH_TEMPORARY_CONTACT_LEVEL:	// Default All Search Setting
//		//if (
//		//	m_stSrchForceCalibBondLevel.dSrchLimit != 0.0 &&
//		//	m_stSrchForceCalibBondLevel.dSrchVel != 0.0
//		//	)
//		{
//			lMoveMode = SRCH_MTR_AXIS;
//			m_stMotorZ.stUsrDefProfile[MAINBOND_Z_SRCH_TEMPORARY_CONTACT_SNR_ON_LEVEL].stProf = m_stSrchDefault;
//			m_stMotorZ.SetSrchProfSi(m_stMotorZ.stUsrDefProfile[MAINBOND_Z_SRCH_TEMPORARY_CONTACT_SNR_ON_LEVEL].sSrchProfID, m_stSrchDefault);
//			stSrchProf = m_stMotorZ.stUsrDefProfile[MAINBOND_Z_SRCH_TEMPORARY_CONTACT_SNR_ON_LEVEL];
//		}
//		break;
//#endif
	}

	switch (lMoveMode)
	{
	case MOVE_MTR_AXIS:
		nResult = m_stMotorZ.MoveAbsolute(lMtrCnt, bWait);
		break;

	case SRCH_MTR_AXIS:
		nResult = m_stMotorZ.Search(stSrchProf, sProcListSaveID, bWait);
		break;
	}
	
	return nResult;
}




INT CACF::MoveX(ACF_X_CTRL_SIGNAL ctrl, BOOL bWait)
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
			HMIMessageBox(MSG_OK, "WARNING", "ACF Module Not Selected. Operation Abort");
			return GMP_FAIL;
		}
	}

	switch (ctrl)
	{
	case ACF_X_CTRL_GO_STANDBY_POSN:
		stMove.x = m_lStandbyPosnACF;
		break;
	}

	return MoveAbsoluteX(stMove.x, bWait);
}

INT CACF::MoveBF(ACF_BF_CTRL_SIGNAL ctrl, BOOL bWait)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	INT nResult = GMP_SUCCESS;
	LONG lDAC;

	if (pAppMod->State() == IDLE_Q)
	{
		if (!m_bModSelected)
		{
			HMIMessageBox(MSG_OK, "WARNING", "ACF Module Not Selected. Operation Abort");
			return GMP_FAIL;
		}
	}

	switch (ctrl)
	{

	case ACF_BF_CTRL_OFF:
		lDAC = 0;
		break;

	case ACF_BF_CTRL_DAC_ON:
		lDAC = (LONG)m_lFinalDACCtrl[ACF_1];
		break;

	}

	return m_stMotorBF.OpenDAC(lDAC, 0, m_lAttachACFDelay, 10, bWait);
}

INT CACF::MoveAbsoluteX(LONG lCntX, BOOL bWait)
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

INT CACF::MoveRelativeX(LONG lCntX, BOOL bWait)
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

////////////////////////////////////////////////////
////////////////////////////////////////////////////
BOOL CACF::PressOperation(LONG lPreAttachDelay, LONG lBondDelay, DOUBLE dPressureCtrl, LONG lFinalPressure, DOUBLE dUpPressureCtrl, BOOL bIndexACF)
{
	SetForceChargeAmpOperateSol(OFF, GMP_NOWAIT);

	Sleep(500);

	SetForceChargeAmpOperateSol(ON, GMP_NOWAIT);

	Sleep(500);

	if (m_bLogForceSensor)
	{
		CString szInFilePath2 = FORCE_SENSOR_LOG_PATH;
		m_szLogLabel.Format("Bond Force Log running Press Test");

		//SetForceChargeAmpOperateSol(OFF, GMP_NOWAIT);

		//Sleep(500);

		//SetForceChargeAmpOperateSol(ON, GMP_NOWAIT);

		//Sleep(500);

		if (m_lCurSlave == ACF_1)
		{
			CString szInFileName2 = FORCE_SENSOR_LOG_FILENAME;
			theApp.CheckFileSize(szInFilePath2, szInFileName2, 512); //512KB

			m_bForceLogRun = TRUE;
		}
		StartLogForceThread();
	}

	if (
		//((m_nLastError = SetHeadUpSol(OFF, GMP_WAIT)) != GMP_SUCCESS) || 
		//((m_nLastError = SetHeadDownSol(ON, GMP_WAIT)) != GMP_SUCCESS) ||
		//((m_nLastError = m_stMotorX.MoveAbsolute(m_pCACFWH->m_mtrApproxBondPosn.x, GMP_WAIT)) != GMP_SUCCESS) ||
		//((m_nLastError = m_pCACFWH->MoveY(ACF_WH_Y_CTRL_GO_APPROX_BOND_POSN, GMP_WAIT)) != GMP_SUCCESS) || 
		((m_nLastError = m_stMotorBF.OpenDAC(0, lFinalPressure, 500, 0, GMP_WAIT)) != GMP_SUCCESS) || 
		//((m_nLastError = MoveBF(ACF_BF_CTRL_DAC_ON, GMP_WAIT)) != GMP_SUCCESS) ||
		((m_nLastError = SleepWithReturn(lPreAttachDelay)) != GMP_SUCCESS) ||
		((m_nLastError = MoveZ(ACF_Z_CTRL_GO_BOND_LEVEL, GMP_WAIT)) != GMP_SUCCESS) ||
		((m_nLastError = SleepWithReturn(lBondDelay)) != GMP_SUCCESS) || 
		((m_nLastError = LogForceEndOfBond()) != GMP_SUCCESS) ||
		((m_nLastError = MoveBF(ACF_BF_CTRL_OFF, GMP_NOWAIT)) != GMP_SUCCESS)  || 
		((m_nLastError = MoveZ(ACF_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT)) != GMP_SUCCESS) //||
		//((m_nLastError = SleepWithReturn(2000)) != GMP_SUCCESS) //|| 
		//((m_nLastError = m_pCACFWH->MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT)) != GMP_SUCCESS)
	   )
	{
		SetError(m_nLastError);
		return FALSE;
	}

	if (m_bLogForceSensor)
	{
		m_bForceLogRun = FALSE;
	}

	if (bIndexACF)
	{
#ifdef ACF_TAPE_END_SNR
		if ((AutoMode == AUTO_BOND || AutoMode == TEST_BOND) && IsTapeEndSnrOn()) //20121122
		{
			//SetError(IDS_ACF1_TAPE_END_ERR + (m_lCurSlave * CONVERT_TO_MAPPING));
			SetError(IDS_ACF1_TAPE_END_ERR);
			return FALSE;
		}
		else
#endif
		{
			RemoveACFArray();

			// stripper ACF
			if (
				((m_nLastError = MoveStripperX(ACF_STRIPPER_X_CTRL_GO_OUT_OFFSET, GMP_WAIT)) != GMP_SUCCESS) ||	// 20150212
				((m_nLastError = MoveStripperX(ACF_STRIPPER_X_CTRL_GO_STANDBY_POSN, GMP_WAIT)) != GMP_SUCCESS)	// 20150212
				//((m_nLastError = SetStripperSol(ON, GMP_WAIT)) != GMP_SUCCESS) || 
				//((m_nLastError = SetStripperSol(OFF, GMP_WAIT)) != GMP_SUCCESS)
		   )
			{
				SetError(m_nLastError);
				return FALSE;
			}
		}
	}

	return TRUE;
}

VOID CACF::ForceCalibPressOperation()
{
	INT i = m_lForceCalibPressCycle;
	if (!m_bPressCycle)
	{
		return;
	}

	//SetForceChargeAmpOperateSol(OFF, GMP_NOWAIT);

	//Sleep(500);

	//SetForceChargeAmpOperateSol(ON, GMP_NOWAIT);

	//Sleep(500);

	do
	{

		//m_stMotorZ.MoveRelative(-10000, GMP_WAIT);
		//SetForceChargeAmpOperateSol(OFF, GMP_NOWAIT);

		//Sleep(1000);

		//SetForceChargeAmpOperateSol(ON, GMP_NOWAIT);

		//Sleep(1000);

		if (!PressOperation(m_lCalibPreAttachDelay, m_lCalibAttachDelay, m_dCalibPressureCtrl, m_lCalibFinalDACCtrl, m_dCalibUpPressureCtrl, FALSE))
		{
			// operation failed!
			m_bPressCycle = FALSE;
			break;
		}
		i--;
		
	} while (i != 0);

}

VOID CACF::SinglePressOperation(BOOL bIsMoveStripper)
{
	if (!m_bPressCycle)
	{
		return;
	}

	do
	{
		if (
			//m_pCACFWH->MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS || 
			m_pCACFWH->ACFMoveAbsoluteXY(m_pCACFWH->m_mtrApproxBondPosn.x, m_pCACFWH->m_mtrApproxBondPosn.y, GMP_WAIT) != GMP_SUCCESS /*||*/
			//m_pCACFWH->MoveZ(ACF_WH_Z_CTRL_GO_APPROX_BOND_LEVEL, GMP_WAIT) != GMP_SUCCESS 
		   )
		{
			// operation failed!
			m_bPressCycle = FALSE;
			break;
		}
		
		//if (!PressOperation(m_lCalibPreAttachDelay, m_lCalibAttachDelay, m_dCalibPressureCtrl, m_dCalibFinalPressureCtrl, m_dCalibUpPressureCtrl, FALSE))
		if (!PressOperation(m_lCalibPreAttachDelay, m_lCalibAttachDelay, m_dCalibPressureCtrl, m_lCalibFinalDACCtrl, m_dCalibUpPressureCtrl, bIsMoveStripper)) //20130318
		{
			// operation failed!
			m_bPressCycle = FALSE;
			break;
		}
		
		if (
			//m_pCACFWH->MoveZ(ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS || 
			m_pCACFWH->MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS
		   )
		{
			// operation failed!
			m_bPressCycle = FALSE;
			break;
		}

		Sleep(m_lPressTestDelay);

	} while (0);

}

BOOL CACF::ForceCalibration()
{
	LONG lAnswer = rMSG_TIMEOUT;
	INT i = 0;
	INT j = 0;
	BOOL bWarned = FALSE;
	LONG lTempCalibFinalDACCtrl;
	CString szMsg;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "ACF FORCE CALIBRATION", szMsg);
		return FALSE;
	}

	lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "ACF FORCE CALIBRATION", "Please place force calibrator onto ACF base heater then press Continue.");

	if (lAnswer == rMSG_CANCEL || lAnswer == rMSG_TIMEOUT)
	{
		return FALSE;
	}

	LONG lLowerBound = 0;
	LONG lUpperBound = 0;

	DOUBLE dForceData[MAX_FORCE_DATA];
	LONG lPressureData[MAX_FORCE_DATA];
	DOUBLE dForceDataSample[3];
	DOUBLE dTempForceData = 0.0;

	for (i = 0; i < MAX_FORCE_DATA; i++)
	{
		dForceData[i]		= 0.0;
		lPressureData[i]	= 0;
	}
	for (i = 0; i < 3; i++)
	{
		dForceDataSample[i] = 0.0;
	}
	if (!HMINumericKeys("Enter Pressure Lower Range", 20000, -20000, &lLowerBound))
	{
		return FALSE;
	}

	if (!HMINumericKeys("Enter Pressure Upper Range", 20000, lLowerBound, &lUpperBound))
	{
		return FALSE;
	}

	if (lUpperBound <= lLowerBound)
	{
		HMIMessageBox(MSG_OK, "ACF FORCE CALIBRATION", "ERROR: Upper Bound Must be Larger Than Lower Bound!");
		return FALSE;
	}

	lTempCalibFinalDACCtrl = m_lCalibFinalDACCtrl;
	m_bPressCycle = TRUE;

	// calibration cycle
	for (i = 0; i < MAX_FORCE_DATA; i++)
	{
		lPressureData[i] = lLowerBound + i * (lUpperBound - lLowerBound) / (MAX_FORCE_DATA - 1);
		m_lCalibFinalDACCtrl = lPressureData[i];

		for (j = 0; j < m_lForceCalibPressCycle; j++)
		{
			ForceCalibPressOperation();

			//if (!m_bPressCycle)
			//{
			////	SetHeadDownSol(OFF, GMP_WAIT);
			////	SetCounterBalanceSol(ON, GMP_WAIT);
			//	SetDACValue(m_dPressureCtrl);
			//	return FALSE;
			//}

			// enter force
			szMsg.Format("w/ %.2fbar (%d/%d)", m_lCalibFinalDACCtrl, i + 1, j + 1);
			if (!HMINumericKeys(szMsg, 100.0, 0.0, &dForceDataSample[j]))
			{
				m_bPressCycle = FALSE;

				m_lFinalDACCtrl[ACF1] = 0;
				HMI_dFinalForceCtrl = 0.0;

			//	SetHeadDownSol(OFF, GMP_WAIT);
			//	SetCounterBalanceSol(ON, GMP_WAIT);
				//SetDACValue(m_dPressureCtrl);
				return FALSE;
			}

			if (!bWarned && dForceDataSample[j] > ACF_MAX_FORCE)
			{
				bWarned = TRUE;
				
				lAnswer = HMIMessageBox(MSG_YES_NO, "ACF FORCE CALIBRATION", "Force exceeds recommended limit.  Quit operation?");
				
				if (lAnswer == rMSG_YES || lAnswer == rMSG_TIMEOUT)
				{
					m_bPressCycle = FALSE;

				//	SetHeadDownSol(OFF, GMP_WAIT);
				//	SetCounterBalanceSol(ON, GMP_WAIT);
					//SetDACValue(m_dPressureCtrl);
					return FALSE;
				}
			}
		}

		dTempForceData = 0.0;
		
		for (INT k = 0; k < m_lForceCalibPressCycle; k++)
		{
			dTempForceData += dForceDataSample[k];
		}

		dForceData[i] = dTempForceData / m_lForceCalibPressCycle;

	}

	lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "ACF FORCE CALIBRATION", "Accept New Data?");
	
	if (lAnswer == rMSG_CANCEL || lAnswer == rMSG_TIMEOUT)
	{
		return FALSE;
	}

	for (i = 0; i < MAX_FORCE_DATA; i++)
	{
		m_stForceCalib[m_lCurrBondForceMode][i].OpenDacValue = lPressureData[i];
		m_stForceCalib[m_lCurrBondForceMode][i].Force_kgf = dForceData[i];
	}

	DacToForce(m_lCalibFinalDACCtrl, m_stForceCalib[m_lCurrBondForceMode], &m_dCalibForceCtrl);
	HMI_dCalibForceCtrl	= m_dCalibForceCtrl * (g_bShowForceInNewton * KGF_TO_NEWTON + (1 - (LONG)g_bShowForceInNewton));

//	SetHeadDownSol(OFF, GMP_WAIT);
//	SetCounterBalanceSol(ON, GMP_WAIT);
	//SetDACValue(m_dPressureCtrl);

	m_bPressCycle = FALSE;

	m_lCalibFinalDACCtrl = lTempCalibFinalDACCtrl;


	return TRUE;
}



/////////////////////////////////////////////////////////////////
//ACF Array Function
/////////////////////////////////////////////////////////////////
VOID CACF::ClearACFArray()
{
	for (INT i = 0; i < MAX_SIZE_OF_ACF_ARRAY; i++)
	{
		m_emACFIndexed[i] = NO_ACF;
	}

	m_emLastIndexedACF = NO_ACF;
	m_nIndexFirst = 0;
	m_nIndexLast = 0;
	m_dAdvancedLength = 0.0;

	UpdateACFArrayDisplay();
}

BOOL CACF::InsertACFArray()
{
	ACFNum nTempACF = ACF1;
	INT i = 0;

	CString szMsg = "";

	//szMsg.Format("Before Insert First :   IndexFirst[%ld] = %ld, IndexLast[%ld] = %ld, ", m_nIndexFirst, (LONG)m_emACFIndexed[m_nIndexFirst]+1,  m_nIndexLast, (LONG)m_emACFIndexed[m_nIndexLast]+1);
	//DisplayMessage(szMsg);

	if (m_emACFIndexed[m_nIndexFirst] != NO_ACF)
	{
		szMsg.Format("No Insert First :   IndexFirst[%ld] = %ld, IndexLast[%ld] = %ld, ", m_nIndexFirst, (LONG)m_emACFIndexed[m_nIndexFirst] + 1, m_nIndexLast, (LONG)m_emACFIndexed[m_nIndexLast] + 1);
		DisplayMessage(szMsg);

		return FALSE;
	}

	// check if next ACF is enabled. If not, index ACF1
	if (m_pCACFWH->m_stACF[(m_emLastIndexedACF + 1) % MAX_NUM_OF_ACF].bEnable)
	{
		m_emACFIndexed[m_nIndexFirst] = (ACFNum)((m_emLastIndexedACF + 1) % MAX_NUM_OF_ACF);
	}
	else
	{
		m_emACFIndexed[m_nIndexFirst] = ACF1;
		// Add me Rework
	}

	m_emLastIndexedACF = m_emACFIndexed[m_nIndexFirst];
	m_nIndexFirst = (m_nIndexFirst + 1) % MAX_SIZE_OF_ACF_ARRAY;

	UpdateACFArrayDisplay();

	//szMsg.Format("After Insert First :   IndexFirst[%ld] = %ld, IndexLast[%ld] = %ld, ", m_nIndexFirst, (LONG)m_emACFIndexed[m_nIndexFirst]+1,  m_nIndexLast, (LONG)m_emACFIndexed[m_nIndexLast]+1);
	//DisplayMessage(szMsg);

	return TRUE;
}

ACFNum CACF::RemoveACFArray()
{
	CString szMsg = "";

	//szMsg.Format("Before Remove Last :   IndexFirst[%ld] = %ld, IndexLast[%ld] = %ld, ", m_nIndexFirst, (LONG)m_emACFIndexed[m_nIndexFirst]+1,  m_nIndexLast, (LONG)m_emACFIndexed[m_nIndexLast]+1);
	//DisplayMessage(szMsg);

	if (m_emACFIndexed[m_nIndexLast] == NO_ACF)
	{
		szMsg.Format("No Remove Last :   IndexFirst[%ld] = %ld, IndexLast[%ld] = %ld, ", m_nIndexFirst, (LONG)m_emACFIndexed[m_nIndexFirst] + 1, m_nIndexLast, (LONG)m_emACFIndexed[m_nIndexLast] + 1);
		DisplayMessage(szMsg);

		return NO_ACF;
	}

	ACFNum emResult = m_emACFIndexed[m_nIndexLast];
	m_emACFIndexed[m_nIndexLast] = NO_ACF;

	m_nIndexLast = (m_nIndexLast + 1) % MAX_SIZE_OF_ACF_ARRAY;

	UpdateACFArrayDisplay();

	//szMsg.Format("After Remove Last :   IndexFirst[%ld] = %ld, IndexLast[%ld] = %ld, ", m_nIndexFirst, (LONG)m_emACFIndexed[m_nIndexFirst]+1,  m_nIndexLast, (LONG)m_emACFIndexed[m_nIndexLast]+1);
	//DisplayMessage(szMsg);


	return emResult;
}

VOID CACF::UpdateACFArrayDisplay()
{
	INT	nACFArray[MAX_SIZE_OF_ACF_ARRAY];

	for (INT i = 0; i < MAX_SIZE_OF_ACF_ARRAY; i++)
	{
		nACFArray[i] = m_emACFIndexed[(m_nIndexLast + i) % MAX_SIZE_OF_ACF_ARRAY] + 1;
	}

	HMI_szACFArray.Format("|ACF# %d | %d | %d | %d | %d | %d | %d | %d | %d | %d |...................| %d | %d | %d | %d | %d | %d | %d | %d | %d | %d |", 
						  nACFArray[MAX_SIZE_OF_ACF_ARRAY - 1], nACFArray[MAX_SIZE_OF_ACF_ARRAY - 2], nACFArray[MAX_SIZE_OF_ACF_ARRAY - 3], nACFArray[MAX_SIZE_OF_ACF_ARRAY - 4], 
						  nACFArray[MAX_SIZE_OF_ACF_ARRAY - 5], nACFArray[MAX_SIZE_OF_ACF_ARRAY - 6], nACFArray[MAX_SIZE_OF_ACF_ARRAY - 7], nACFArray[MAX_SIZE_OF_ACF_ARRAY - 8],
						  nACFArray[MAX_SIZE_OF_ACF_ARRAY - 9], nACFArray[MAX_SIZE_OF_ACF_ARRAY - 10], 
						  nACFArray[MAX_SIZE_OF_ACF_ARRAY - 26], nACFArray[MAX_SIZE_OF_ACF_ARRAY - 27], nACFArray[MAX_SIZE_OF_ACF_ARRAY - 28], nACFArray[MAX_SIZE_OF_ACF_ARRAY - 29], 
						  nACFArray[MAX_SIZE_OF_ACF_ARRAY - 30], nACFArray[MAX_SIZE_OF_ACF_ARRAY - 31] , nACFArray[MAX_SIZE_OF_ACF_ARRAY - 32], nACFArray[MAX_SIZE_OF_ACF_ARRAY - 33], 
						  nACFArray[MAX_SIZE_OF_ACF_ARRAY - 34], nACFArray[MAX_SIZE_OF_ACF_ARRAY - 35]);
}

ACFNum CACF::GetNextArrayACFNum()
{
	INT			nTempACF	= m_emLastIndexedACF;
	INT			i			= 0;
	ACF_INFO	stTempACF[MAX_NUM_OF_ACF];	

	if (m_emACFIndexed[m_nIndexFirst] != NO_ACF)
	{
		return m_emACFIndexed[m_nIndexFirst];
	}

	if (m_emLastIndexedACF == NO_ACF)
	{
		nTempACF = ACF1;
	}
	else
	{
		// check if next ACF is enabled. If not, index ACF1
		if (m_pCACFWH->m_stACF[(m_emLastIndexedACF + 1) % MAX_NUM_OF_ACF].bEnable)
		{
			nTempACF = (ACFNum)((m_emLastIndexedACF + 1) % MAX_NUM_OF_ACF);
		}
		else
		{
			nTempACF = ACF1;
		}
	}

	return (ACFNum)nTempACF;
}

INT CACF::GetCurrArraySize()
{
	DOUBLE dDistToCutter = 0.0;
	INT	nCurACF = NO_ACF;
	INT nArrarySize = 0;

	// Debug
	//CString szMsg = "";	

	dDistToCutter = m_dCutterToHeadDistance + m_dCutterToHeadOffset + m_pCACFWH->m_stACF[ACF1].dLength;
	nCurACF = m_emACFIndexed[m_nIndexLast];

	//szMsg.Format("dDistToCutter = %.2f, nCurACF = %d", dDistToCutter, nCurACF);
	//DisplayMessage(szMsg);

	nArrarySize = 1;	// count for the one under ACF head
	do
	{
		nCurACF++;
		nCurACF = nCurACF % MAX_NUM_OF_ACF;

		if (m_pCACFWH->m_stACF[nCurACF].bEnable)
		{
			if (dDistToCutter >= m_pCACFWH->m_stACF[nCurACF].dLength)
			{
				dDistToCutter -= m_pCACFWH->m_stACF[nCurACF].dLength;
				nArrarySize++;
			}
			else
			{
				break;
			}
		}

		//szMsg.Format("nCurACF: %d, dDistToCutter = %.2f, Length = %.2f", nCurACF, dDistToCutter, m_pCACFWH->m_stACF[nCurACF].dLength);
		//DisplayMessage(szMsg);

	} while (nArrarySize < MAX_SIZE_OF_ACF_ARRAY);

	return nArrarySize;
}

VOID CACF::CalcIndexedLength()
{
	INT	nCurIndex = 0;
	m_dIndexedLength = 0.0;

	// Debug
	CString szMsg = "";

	//if (HMI_bDebugCal)
	//{
	//	szMsg.Format("m_nIndexFirst = %d, m_nIndexLast = %d", m_nIndexFirst, m_nIndexLast);
	//	DisplayMessage(szMsg);
	//}

	nCurIndex = m_nIndexLast;
	if (m_emACFIndexed[nCurIndex] != NO_ACF)
	{
		m_dIndexedLength += m_pCACFWH->m_stACF[m_emACFIndexed[nCurIndex]].dLength;
	}

	while (nCurIndex != m_nIndexFirst)
	{
		nCurIndex = (nCurIndex + 1) % MAX_SIZE_OF_ACF_ARRAY;
		if (m_emACFIndexed[nCurIndex] != NO_ACF)
		{
			m_dIndexedLength += m_pCACFWH->m_stACF[m_emACFIndexed[nCurIndex]].dLength;
		}

		//if (HMI_bDebugCal)
		//{
		//	szMsg.Format("m_dIndexedLength = %.2f, nCurIndex = %d", m_dIndexedLength, nCurIndex);
		//	DisplayMessage(szMsg);
		//}
	}

	
	m_dIndexedLength += m_dAdvancedLength;

	if (m_emACFIndexed[m_nIndexLast] != NO_ACF)
	{
		m_dRemainingLength = m_dCutterToHeadDistance + m_dCutterToHeadOffset - (m_dIndexedLength - m_pCACFWH->m_stACF[m_emACFIndexed[m_nIndexLast]].dLength);
	}
	else
	{
		m_dRemainingLength = m_dCutterToHeadDistance + m_dCutterToHeadOffset - (m_dIndexedLength - m_pCACFWH->m_stACF[(ACFNum)((m_emLastIndexedACF + 1) % MAX_NUM_OF_ACF)].dLength);	
	}
	
	//if (HMI_bDebugCal)
	//{
	//	szMsg.Format("(FINISHED)m_dIndexedLength = %.2f, m_dRemainingLength = %.2f", m_dIndexedLength, m_dRemainingLength);
	//	DisplayMessage(szMsg);
	//}

	// 0.1um allowance
	if (m_dRemainingLength >= -0.0005 && m_dRemainingLength < 0.0)
	{
		m_dRemainingLength = 0.0;
	}

	if (m_dRemainingLength < 0.0)
	{
		szMsg.Format("CalcIndexedLength: m_dRemainingLength = %.8f", m_dRemainingLength);
		DisplayMessage(szMsg);

		if (RemoveACFArray() != NO_ACF)
		{
			CalcIndexedLength();
			m_bValidIndexedLength = TRUE;
		}
		else
		{
			m_bValidIndexedLength = FALSE;
		}
	}
	else
	{
		m_bValidIndexedLength = TRUE;
	}

	return;
}

BOOL CACF::CutACFOperation(BOOL bReset)
{
	CString szMsg;
	m_nLastError = GMP_SUCCESS;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "CUT ACF OPERATION", szMsg);
		return FALSE;
	}

	if (
		//(m_nLastError = SleepWithReturn(1000)) != GMP_SUCCESS || 
		(m_nLastError = SetHalfCutterOnSol(ON, GMP_WAIT)) != GMP_SUCCESS || 
		(m_nLastError = SetHalfCutterOnSol(OFF, GMP_WAIT)) != GMP_SUCCESS ||
		getKlocworkFalse() //klocwork fix 20121211
	   )
	{
		SetError(m_nLastError);
		return FALSE;
	}

	if (bReset)
	{
		m_dAdvancedLength = 0.0;
	}

	return TRUE;
}

INT CACF::IndexToACFHeadOperation()
{
	// Debug
	CString szMsg = "";
	LONG	lAnswer = rMSG_TIMEOUT;
	ACFNum nNextACF = NO_ACF;

	if (m_fHardware && !m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "INDEX TO ACF HEAD", szMsg);
		return gnAMS_NOTOK;
	}

	// Check ACF Array Not Empty
	BOOL bEmpty = TRUE;

	for (INT i = 0; i < MAX_SIZE_OF_ACF_ARRAY; i++)
	{
		if (m_emACFIndexed[i] != NO_ACF)
		{
			bEmpty = FALSE;
		}
	}

	if (!bEmpty)
	{
		lAnswer = HMIMessageBox(MSG_YES_NO, "INDEX TO ACF HEAD", "ACF array is not empty. Clear ACF?");
		if (lAnswer == rMSG_NO || lAnswer == rMSG_TIMEOUT)		
		{
			HMIMessageBox(MSG_OK, "INDEX TO ACF HEAD", "Operation Abort!");
			return gnAMS_NOTOK;
		}

		if (!CutACFOperation())
		{
			HMIMessageBox(MSG_OK, "INDEX TO ACF HEAD", "Operation Abort!");
			return gnAMS_NOTOK;
		}

		HMIMessageBox(MSG_OK, "INDEX TO ACF HEAD", "Please clean ACF before the cutter.");
	}

	ClearACFArray();

	// Index buffer ACF
	INT nArrarySize = GetCurrArraySize();

	CalcIndexedLength();

	do
	{
		if (
			IndexACFIndexer() != GMP_SUCCESS || 
			MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, GMP_NOWAIT) != GMP_SUCCESS || 
			SyncRoller() != GMP_SUCCESS || 
			!CutACFOperation() 
		   )
		{
			HMIMessageBox(MSG_OK, "INDEX TO ACF HEAD", "Operation Abort!");
			return gnAMS_NOTOK;
		}

		// Debug
		//szMsg.Format("m_dRemainingLength = %.5f, ACF Length Indexed: %.2f", m_dRemainingLength, m_dmtrLength);
		//DisplayMessage(szMsg);

		CalcIndexedLength();

		if (!m_bValidIndexedLength)
		{
			//SetError(IDS_ACF1_INVALID_ACF_INDEXED_LENGTH_ERR + (m_lCurSlave * CONVERT_TO_MAPPING));
			SetError(IDS_ACF1_INVALID_ACF_INDEXED_LENGTH_ERR);
			HMIMessageBox(MSG_OK, "INDEX TO ACF HEAD", "Operation Abort!");
			return gnAMS_NOTOK;
		}

		nNextACF = GetNextArrayACFNum();

	} 	
#if 1 //klocwork fix 20121211
	while (nNextACF < MAX_NUM_OF_ACF && nNextACF > NO_ACF && m_dRemainingLength >= m_pCACFWH->m_stACF[nNextACF].dLength);
#else
	while (m_dRemainingLength >= m_pCACFWH->m_stACF[nNextACF].dLength && nNextACF < MAX_NUM_OF_ACF && nNextACF > NO_ACF);
#endif
	// Index remaining ACF

	// Debug
	//szMsg.Format("m_dRemainingLength = %.5f", m_dRemainingLength);
	//DisplayMessage(szMsg);

	if (MoveRelativeIndexer((LONG)(-1.0 * m_dRemainingLength * 1000 * m_stMotorIndex.stAttrib.dDistPerCount * m_dIndexerFactor * m_dIndexerCustomFactor), GMP_WAIT) != GMP_SUCCESS) //20120706
	{
		HMIMessageBox(MSG_OK, "INDEX TO ACF HEAD", "Operation Abort!");
		return gnAMS_NOTOK;
	}

	m_dRemainingLength = 0.0;

	if (
		MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, GMP_NOWAIT) != GMP_SUCCESS || 
		SyncRoller() != GMP_SUCCESS
	   )
	{
		HMIMessageBox(MSG_OK, "INDEX TO ACF HEAD", "Operation Abort!");
		return gnAMS_NOTOK;
	}

	return GMP_SUCCESS;
}


INT CACF::IndexToACFHeadOperation_Single_ACF()
{
	// Debug
	CString szMsg = "";
	LONG	lAnswer = rMSG_TIMEOUT;
	ACFNum nNextACF = NO_ACF;
	DOUBLE dACFLength = 0.0;
	DOUBLE dRemainingACFLength = 0.0;

	if (m_fHardware && !m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "INDEX TO ACF HEAD", szMsg);
		return gnAMS_NOTOK;
	}

	

	lAnswer = HMIMessageBox(MSG_YES_NO, "INDEX TO ACF HEAD", "Clear ACF?");
	if (lAnswer == rMSG_NO || lAnswer == rMSG_TIMEOUT)		
	{
		HMIMessageBox(MSG_OK, "INDEX TO ACF HEAD", "Operation Abort!");
		return gnAMS_NOTOK;
	}

	if (!CutACFOperation())
	{
		HMIMessageBox(MSG_OK, "INDEX TO ACF HEAD", "Operation Abort!");
		return gnAMS_NOTOK;
	}

	HMIMessageBox(MSG_OK, "INDEX TO ACF HEAD", "Please clean ACF before the cutter.");


	dACFLength = m_pCACFWH->m_stACF[ACF1].dLength;
		
	if (
		MoveRelativeIndexer((LONG)(-1.0 * dACFLength * 1000 * m_stMotorIndex.stAttrib.dDistPerCount * m_dIndexerFactor * m_dIndexerCustomFactor), GMP_WAIT) != GMP_SUCCESS || //20120706
		MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, GMP_NOWAIT) != GMP_SUCCESS || 
		SyncRoller() != GMP_SUCCESS || 
		!CutACFOperation() 
	   )
	{
		HMIMessageBox(MSG_OK, "INDEX TO ACF HEAD", "Operation Abort!");
		return gnAMS_NOTOK;
	}

	// Index remaining ACF

	// Debug
	//szMsg.Format("m_dRemainingLength = %.5f", m_dRemainingLength);
	//DisplayMessage(szMsg);


	dRemainingACFLength = m_dCutterToHeadDistance + m_dCutterToHeadOffset;

	if (MoveRelativeIndexer((LONG)(-1.0 * dRemainingACFLength * 1000 * m_stMotorIndex.stAttrib.dDistPerCount * m_dIndexerFactor * m_dIndexerCustomFactor), GMP_WAIT) != GMP_SUCCESS) //20120706
	{
		HMIMessageBox(MSG_OK, "INDEX TO ACF HEAD", "Operation Abort!");
		return gnAMS_NOTOK;
	}

	m_dRemainingLength = 0.0;

	if (
		MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, GMP_NOWAIT) != GMP_SUCCESS || 
		SyncRoller() != GMP_SUCCESS
	   )
	{
		HMIMessageBox(MSG_OK, "INDEX TO ACF HEAD", "Operation Abort!");
		return gnAMS_NOTOK;
	}

	return GMP_SUCCESS;
}

VOID CACF::RecoverACFArrayOperation()
{
	// Check ACF Array Empty
	BOOL bEmpty = TRUE;
	ACFNum nNextACF = NO_ACF;

	for (INT i = 0; i < MAX_SIZE_OF_ACF_ARRAY; i++)
	{
		if (m_emACFIndexed[i] != NO_ACF)
		{
			bEmpty = FALSE;
		}
	}

	if (!bEmpty)
	{
		return;
	}

	ClearACFArray();

	CalcIndexedLength();

	// Index buffer ACF
	if (m_bValidIndexedLength)
	{
		do
		{
			InsertACFArray();
			CalcIndexedLength();

			if (!m_bValidIndexedLength)
			{
				SetError(IDS_ACF1_INVALID_ACF_INDEXED_LENGTH_ERR);
				return;
			}

			nNextACF = GetNextArrayACFNum();

		} 
#if 1 //klocwork fix 20121211
		while (nNextACF < MAX_NUM_OF_ACF && nNextACF > NO_ACF && m_dRemainingLength >= m_pCACFWH->m_stACF[nNextACF].dLength);
#else
		while (m_dRemainingLength >= m_pCACFWH->m_stACF[nNextACF].dLength && nNextACF < MAX_NUM_OF_ACF && nNextACF > NO_ACF);
#endif
		m_dAdvancedLength = m_dRemainingLength;
		m_dRemainingLength = 0.0;
	}
	else
	{
		SetError(IDS_ACF1_INVALID_ACF_INDEXED_LENGTH_ERR);
		HMIMessageBox(MSG_OK, "INDEX TO ACF HEAD", "Operation Abort!");
		return ;
	}

	return;
}

INT CACF::IndexNextACFOperation()
{
	DOUBLE dAllowedLength = 0.0;
	DOUBLE dTotalLength = 0.0;
	DOUBLE dRelativeLength = 0.0;
	CString szMsg;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "INDEX NEXT ACF", szMsg);
		return gnAMS_NOTOK;
	}

	CalcIndexedLength();

	if (m_bValidIndexedLength)
	{
		if (m_emACFIndexed[m_nIndexLast + 1] != NO_ACF)
		{
			dTotalLength = m_dCutterToHeadDistance + m_dCutterToHeadOffset + m_pCACFWH->m_stACF[m_emACFIndexed[m_nIndexLast]].dLength;
		}
		else
		{
			dTotalLength = m_dCutterToHeadDistance + m_dCutterToHeadOffset + m_pCACFWH->m_stACF[GetNextArrayACFNum()].dLength;
		}

		dRelativeLength = m_pCACFWH->m_stACF[GetNextArrayACFNum()].dLength - m_dAdvancedLength;

		dAllowedLength = dTotalLength - m_dIndexedLength;
		
		// Index + Cut
		if (m_pCACFWH->m_stACF[GetNextArrayACFNum()].dLength - m_dAdvancedLength <= dAllowedLength && dAllowedLength != 0.0)
		{
			if (InsertACFArray())
			{
				//szMsg.Format("dRelativeLength: %.2f", dRelativeLength);
				//DisplayMessage(szMsg);

				if (
					MoveRelativeIndexer((LONG)(-1.0 * dRelativeLength * 1000.0 / m_stMotorIndex.stAttrib.dDistPerCount * m_dIndexerFactor * m_dIndexerCustomFactor), SFM_WAIT) != gnAMS_OK 
				   )
				{
					HMIMessageBox(MSG_OK, "INDEX NEXT ACF", "Operation Abort!");
					return gnAMS_NOTOK;
				}
				Sleep(500);
				if (
					//MoveRelativeIndexer((LONG)(-1.0 * dRelativeLength * 1000.0 / m_stMotorIndex.stAttrib.dDistPerCount * m_dIndexerFactor * m_dIndexerCustomFactor), SFM_WAIT) != gnAMS_OK || //20120706
					!CutACFOperation()
				   )
				{
					HMIMessageBox(MSG_OK, "INDEX NEXT ACF", "Operation Abort!");
					return gnAMS_NOTOK;
				}
			}
			else
			{
				DisplayMessage("ACF Error: InsertACFArray");
			}

			CalcIndexedLength();

			if (m_bValidIndexedLength)
			{
				dTotalLength = m_dCutterToHeadDistance + m_dCutterToHeadOffset + m_pCACFWH->m_stACF[m_emACFIndexed[m_nIndexLast]].dLength;
				dRelativeLength = m_pCACFWH->m_stACF[GetNextArrayACFNum()].dLength - m_dAdvancedLength;

				dAllowedLength = dTotalLength - m_dIndexedLength;

				// Index with Reminding Length?
				if (m_pCACFWH->m_stACF[GetNextArrayACFNum()].dLength - m_dAdvancedLength > dAllowedLength)
				{
					//szMsg.Format("m_dRemainingLength: %.2f", m_dRemainingLength);
					//DisplayMessage(szMsg);

					if (
						MoveRelativeIndexer((LONG)(-1.0 * m_dRemainingLength * 1000.0 / m_stMotorIndex.stAttrib.dDistPerCount * m_dIndexerFactor * m_dIndexerCustomFactor), SFM_WAIT) != gnAMS_OK
					   )
					{
						HMIMessageBox(MSG_OK, "INDEX NEXT ACF", "Operation Abort!");
						return gnAMS_NOTOK;
					}
					Sleep(500);
					if (
						//MoveRelativeIndexer((LONG)(-1.0 * m_dRemainingLength * 1000.0 / m_stMotorIndex.stAttrib.dDistPerCount * m_dIndexerFactor * m_dIndexerCustomFactor), SFM_WAIT) != gnAMS_OK || //20120706
						MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, SFM_NOWAIT) != gnAMS_OK || 
						m_stMotorRoller.Sync() != gnAMS_OK
					   )
					{
						HMIMessageBox(MSG_OK, "INDEX NEXT ACF", "Operation Abort!");
						return gnAMS_NOTOK;
					}

					m_dRemainingLength = 0.0;
				}
				else
				{
					if (
						MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, SFM_NOWAIT) != gnAMS_OK || 
						m_stMotorRoller.Sync() != gnAMS_OK 
					   )
					{
						HMIMessageBox(MSG_OK, "INDEX NEXT ACF", "Operation Abort!");
						return gnAMS_NOTOK;
					}
				}
			}
			else
			{
				SetError(IDS_ACF1_INVALID_ACF_INDEXED_LENGTH_ERR);
				HMIMessageBox(MSG_OK, "INDEX NEXT ACF", "Operation Abort!");
				return gnAMS_NOTOK;
			}
		}
		// Index with Reminding Length
		else
		{
			CalcIndexedLength();

			if (m_bValidIndexedLength)
			{
				//szMsg.Format("m_dRemainingLength: %.2f", m_dRemainingLength);
				//DisplayMessage(szMsg);

				if (
					MoveRelativeIndexer((LONG)(-1.0 * m_dRemainingLength * 1000.0 / m_stMotorIndex.stAttrib.dDistPerCount * m_dIndexerFactor * m_dIndexerCustomFactor), SFM_WAIT) != gnAMS_OK || //20120706
					MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, SFM_NOWAIT) != gnAMS_OK || 
					m_stMotorRoller.Sync() != gnAMS_OK
				   )
				{
					HMIMessageBox(MSG_OK, "INDEX NEXT ACF", "Operation Abort!");
					return gnAMS_NOTOK;
				}

				m_dRemainingLength = 0.0;
			}
			else
			{
				SetError(IDS_ACF1_INVALID_ACF_INDEXED_LENGTH_ERR);
				HMIMessageBox(MSG_OK, "INDEX NEXT ACF", "Operation Abort!");
				return gnAMS_NOTOK;
			}
		}
	}
	else
	{
		SetError(IDS_ACF1_INVALID_ACF_INDEXED_LENGTH_ERR);
		HMIMessageBox(MSG_OK, "INDEX NEXT ACF", "Operation Abort!");
		return gnAMS_NOTOK;
	}
	return gnAMS_OK;
}


INT CACF::IndexNextACFOperation_Single_ACF()
{
	// Debug
	CString szMsg = "";
	LONG	lAnswer = rMSG_TIMEOUT;
	ACFNum nNextACF = NO_ACF;
	DOUBLE dACFLength = 0.0;
	DOUBLE dRemainingACFLength = 0.0;

	if (m_fHardware && !m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "INDEX NEXT ACF", szMsg);
		return gnAMS_NOTOK;
	}
	

	dACFLength = m_pCACFWH->m_stACF[ACF1].dLength - (m_dCutterToHeadDistance + m_dCutterToHeadOffset);
		
	if (
		MoveRelativeIndexer((LONG)(-1.0 * dACFLength * 1000.0 / m_stMotorIndex.stAttrib.dDistPerCount * m_dIndexerFactor * m_dIndexerCustomFactor), SFM_WAIT) != gnAMS_OK || //20120706
		MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, SFM_NOWAIT) != gnAMS_OK || 
		m_stMotorRoller.Sync() != gnAMS_OK || 
		!CutACFOperation() 
	   )
	{
		HMIMessageBox(MSG_OK, "INDEX NEXT ACF", "Operation Abort!");
		return gnAMS_NOTOK;
	}

	// Index remaining ACF

	// Debug
	//szMsg.Format("m_dRemainingLength = %.5f", m_dRemainingLength);
	//DisplayMessage(szMsg);


	dRemainingACFLength = m_dCutterToHeadDistance + m_dCutterToHeadOffset;

	if (MoveRelativeIndexer((LONG)(-1.0 * dRemainingACFLength * 1000.0 / m_stMotorIndex.stAttrib.dDistPerCount * m_dIndexerFactor * m_dIndexerCustomFactor), SFM_WAIT) != gnAMS_OK) //201200706
	{
		HMIMessageBox(MSG_OK, "INDEX NEXT ACF", "Operation Abort!");
		return gnAMS_NOTOK;
	}

	m_dRemainingLength = 0.0;

	if (
		MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, SFM_NOWAIT) != gnAMS_OK || 
		m_stMotorRoller.Sync() != gnAMS_OK
	   )
	{
		HMIMessageBox(MSG_OK, "INDEX NEXT ACF", "Operation Abort!");
		return gnAMS_NOTOK;
	}

	return gnAMS_OK;
}


BOOL CACF::SingleCycleOperation(ACFNum nACF)
{
	DWORD	m_dwACFStartTime = 0;
	DWORD	m_dwACFProcessTime = 0;
	CString szMsg;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "INDEX NEXT ACF", szMsg);
		return gnAMS_NOTOK;
	}

	if (!m_pCACFWH->m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", m_pCACFWH->GetStnName());
		HMIMessageBox(MSG_OK, "INDEX NEXT ACF", szMsg);
		return gnAMS_NOTOK;
	}

	if (!m_pCOtherACFWH->m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", m_pCOtherACFWH->GetStnName());
		HMIMessageBox(MSG_OK, "INDEX NEXT ACF", szMsg);
		return gnAMS_NOTOK;
	}

	m_nLastError = GMP_SUCCESS;
	m_dACFProcessTime = 0.0;

	// process timer starts
	m_dwACFStartTime = GetTickCount();

	if (m_pCACFWH->m_stACF[ACF1].dLength <= (m_dCutterToHeadDistance + m_dCutterToHeadOffset))
	{
		if (
			!PressOperation(m_lPreAttachDelay, m_lAttachACFDelay, m_dPressureCtrl, m_lFinalDACCtrl[nACF], m_dUpPressureCtrl) ||
			(m_pCACFWH->MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS) ||
			(m_pCOtherACFWH->MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS) ||
			IndexNextACFOperation() != GMP_SUCCESS
		   )
		{
			return FALSE;
		}
	}
	else
	{
		if (
			!PressOperation(m_lPreAttachDelay, m_lAttachACFDelay, m_dPressureCtrl, m_lFinalDACCtrl[nACF], m_dUpPressureCtrl) ||
			(m_pCACFWH->MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS) ||
			(m_pCOtherACFWH->MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS) ||
			IndexNextACFOperation_Single_ACF() != GMP_SUCCESS
		   )
		{
			return FALSE;
		}
	}

	// process timer ends
	m_dwACFProcessTime = GetTickCount() - m_dwACFStartTime;
	m_dACFProcessTime = (DOUBLE)(m_dwACFProcessTime / 1000.0);

	return TRUE;
}

BOOL CACF::AttachACFOperation(ACFNum nACF)
{
	//if ((m_nLastError = SetCounterBalanceSol(ON, GMP_WAIT)) != GMP_SUCCESS || getKlocworkFalse()) //klocwork fix 20121211
	//{
	//	SetError(m_nLastError);
	//	return FALSE;
	//}

	if (!PressOperation(m_lPreAttachDelay, m_lAttachACFDelay, m_dPressureCtrl, m_lFinalDACCtrl[nACF], m_dUpPressureCtrl))
	{
		return FALSE;
	}

	return TRUE;

}
/////////////////////////////////////////////////////////////////
//Motor Related
/////////////////////////////////////////////////////////////////
INT CACF::MoveRelativeIndexer(DOUBLE dSteps, BOOL bWait)
{	
	// Debug
	//LONG lStartPosn = 0;
	//LONG lStopPosn = 0;

	INT nResult = GMP_SUCCESS;
	if (!m_bModSelected || m_bIsOfflineMode)
	{
		//szMsg.Format("%ld|	MoveRelativeIndexer: Command|Start|Stop	(%.2f     %.2f     %.2f)", m_lMoveIndex, dSteps, lStartPosn*INDEX_MOTOR_TO_ENC_RATIO, (DOUBLE)lStopPosn*INDEX_MOTOR_TO_ENC_RATIO);
		//DisplayMessage(szMsg);

		m_dAdvancedLength += -1.0 * dSteps / (1000.0 / m_stMotorIndex.stAttrib.dDistPerCount * m_dIndexerFactor * m_dIndexerCustomFactor);	// Debug //20120706
		m_dAdvancedLength = SetDecimalPlace(m_dAdvancedLength);
		return nResult;
	}

	nResult = m_stMotorIndex.MoveRelative((LONG)dSteps, bWait);
	
	if (nResult != gnAMS_NOTOK)
	{
		m_dAdvancedLength += -1.0 * dSteps / (1000.0 / m_stMotorIndex.stAttrib.dDistPerCount * m_dIndexerFactor * m_dIndexerCustomFactor);	// Debug //20120706
		m_dAdvancedLength = SetDecimalPlace(m_dAdvancedLength);
	}

	return nResult;
}

INT CACF::MoveIndexer(ACF_INDEXER_CTRL_SIGNAL ctrl, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CString szMsg = _T("");
	DOUBLE	dmtrLength = 0.0;
	switch (ctrl)
	{
	case ACF_INDEXER_INDEX_TAPE:
		// "Previous first index" is the "current first index" + (Array size-1) mod array size
		dmtrLength = -m_pCACFWH->m_stACF[m_emACFIndexed[(m_nIndexFirst + (MAX_SIZE_OF_ACF_ARRAY - 1)) % MAX_SIZE_OF_ACF_ARRAY]].dLength 
					 / m_stMotorIndex.stAttrib.dDistPerCount * 1000.0 * m_dIndexerFactor * m_dIndexerCustomFactor; //20120706
		break;
	}
	return MoveRelativeIndexer((LONG)dmtrLength, bWait);
}

INT CACF::MoveRoller(ACF_ROLLER_CTRL_SIGNAL ctrl, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;
	SRCH_PROFILE	stSrchProf;

	switch (ctrl)
	{
	case ACF_ROLLER_CTRL_SEARCH_UPPER:
		m_stMotorRoller.SetSrchProfSi(m_stMotorRoller.stUsrDefProfile[ACF_ROLLER_SRCH_FEEDER_BUFFER_SNR_ON_LEVEL].sSrchProfID, m_stSrchFreederBuffer);
		stSrchProf = m_stMotorRoller.stUsrDefProfile[ACF_ROLLER_SRCH_FEEDER_BUFFER_SNR_ON_LEVEL];
		break;

	default:
		stSrchProf = m_stMotorRoller.stUsrDefProfile[ACF_ROLLER_SRCH_FEEDER_BUFFER_SNR_ON_LEVEL];
	}

	nResult = m_stMotorRoller.Search(stSrchProf, NO_PROCESS_LIST, bWait);

	return nResult;
}

INT CACF::MoveStripperX(ACF_STRIPPER_X_CTRL_SIGNAL ctrl, BOOL bWait)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	INT		nResult		= GMP_SUCCESS;

	LONG lTempPosn = 0;

	CString szDebug = _T("");
	switch (ctrl)
	{
	case ACF_STRIPPER_X_CTRL_GO_STANDBY_POSN:
		lTempPosn = m_lStandbyPosn;
		m_stMotorStripperX.SetCustMoveProf(m_dStripperXGoMoveRightSpeedFactor); //20150223
		break;

	case ACF_STRIPPER_X_CTRL_GO_OUT_POSN:
		lTempPosn = m_lOutPosn;
		m_stMotorStripperX.SetCustMoveProf(m_dStripperXGoMoveLeftSpeedFactor);
		break;

	case ACF_STRIPPER_X_CTRL_GO_OUT_OFFSET:
		lTempPosn = m_lOutPosn + m_lOutOffset;
		m_stMotorStripperX.SetCustMoveProf(m_dStripperXGoMoveLeftSpeedFactor);
		break;
	}

	if (HMI_bDebugCal)	// 20140801 Yip: Only Show Message When Debug Calculation
	{
		szDebug.Format("MotorStripperX Posn: %d", lTempPosn);
		DisplayMessage(szDebug);
	}
	return m_stMotorStripperX.MoveAbsolute(lTempPosn, GMP_WAIT);
}
/////////////////////////////////////////////////////////////////
//I/O Related
/////////////////////////////////////////////////////////////////

BOOL CACF::IsTurnBufferLmtSnrOn()
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
		bResult = m_stACFFeederBufferSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}


BOOL CACF::IsCutterDownPosnSnrOn()
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
		bResult = m_stACFCutterDnPosnSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CACF::IsTapeDetectSnrOn()
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
		bResult = m_stACFTapeDetectSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

#ifdef ACF_TAPE_END_SNR
BOOL CACF::IsTapeEndSnrOn() //20121122
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
		bResult = m_stACFTapeEndSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}
#endif

//INT CACF::SetHeadDownSol(BOOL bMode, BOOL bWait)
//{
//	INT nResult = GMP_SUCCESS;
//	LONG lTimer = 0;
//	CSoInfo *stTempSo = &m_stHeadDownSol;
//
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
//			stTempSo->GetGmpPort().On();
//			stTempSo->bHMI_Status = TRUE;
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
//			else
//			{
//				TC_DelJob(stTempSo->GetTCObjectID());
//			}
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
//
//INT CACF::SetCounterBalanceSol(BOOL bMode, BOOL bWait)
//{
//	INT nResult = GMP_SUCCESS;
//
//	CSoInfo *stTempSo = &m_stCounterBalanceSol;
//
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
//			stTempSo->GetGmpPort().On();
//			stTempSo->bHMI_Status = TRUE;
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

INT CACF::SetHalfCutterOnSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stACFCutterOnSol;

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
			
			if (bWait)
			{
				Sleep(stTempSo->GetOnDelay());
				//Now the cutter should be at top level. The CutterDownSnr should be off. Check the sensor
				if (IsCutterDownPosnSnrOn()) //20130322
				{ 
				//error
					return IDS_ACF1_CUTTER_NOT_ON_ERR;
				}
			}

			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;

			if (bWait)
			{
				Sleep(stTempSo->GetOffDelay());
				//Now the cutter should be at Lower level. The CutterDownSnr should be on. Check the sensor
				if (!IsCutterDownPosnSnrOn()) //20130322
				{ 
				//error
					return IDS_ACF1_CUTTER_NOT_OFF_ERR;
				}
			}

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

//INT CACF::SetHeadUpSol(BOOL bMode, BOOL bWait)
//{
//	INT nResult = GMP_SUCCESS;
//	LONG lTimer = 0;
//
//	CSoInfo *stTempSo = &m_stHeadUpSol;
//
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
//			stTempSo->GetGmpPort().On();
//			stTempSo->bHMI_Status = TRUE;
//			
//			if (bWait)
//			{
//				do
//				{
//					Sleep(10);
//					lTimer = lTimer + 10;
//				} while (lTimer < stTempSo->GetOnDelay() && !IsHeadUpSnrOn());
//
//				TC_DelJob(stTempSo->GetTCObjectID());
//
//				if (lTimer >= stTempSo->GetOnDelay())
//				{
//					return (IDS_ACF1_HEAD_NOT_UP_ERR + (m_lCurSlave * CONVERT_TO_MAPPING));
//				}
//			}
//			else
//			{
//				TC_DelJob(stTempSo->GetTCObjectID());
//			}
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

//INT CACF::SetStripperSol(BOOL bMode, BOOL bWait)
//{
//	INT nResult = GMP_SUCCESS;
//	LONG lTimer = 0;
//
//	CSoInfo *stTempSo = &m_stStripperSol;
//
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
//			stTempSo->GetGmpPort().On();
//			stTempSo->bHMI_Status = TRUE;
//			
//			if (bWait)
//			{
//				do
//				{
//					Sleep(10);
//					lTimer = lTimer + 10;
//				} while (lTimer < stTempSo->GetOnDelay() && !IsStripperOnSnrOn());
//
//				TC_DelJob(stTempSo->GetTCObjectID());
//
//				if (lTimer >= stTempSo->GetOnDelay())
//				{
//					return (IDS_ACF1_STRIPPER_NOT_ON_ERR + (m_lCurSlave * CONVERT_TO_MAPPING));
//				}
//			}
//			else
//			{
//				TC_DelJob(stTempSo->GetTCObjectID());
//			}
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
//				do
//				{
//					Sleep(10);
//					lTimer = lTimer + 10;
//				} while (lTimer < stTempSo->GetOffDelay() && !IsStripperOffSnrOn());
//
//				TC_DelJob(stTempSo->GetTCObjectID());
//
//				if (lTimer >= stTempSo->GetOffDelay())
//				{
//					return (IDS_ACF1_STRIPPER_NOT_OFF_ERR + (m_lCurSlave * CONVERT_TO_MAPPING));
//				}
//			}
//			else
//			{
//				TC_DelJob(stTempSo->GetTCObjectID());
//			}
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

INT CACF::SetIonizerPowerEnable(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stACFIonizerPowerEnSol;

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

INT CACF::IndexACFIndexer(BOOL bAll)
{
	if (InsertACFArray())
	{
		return MoveIndexer(ACF_INDEXER_INDEX_TAPE, GMP_WAIT);
	}
	else
	{
		return gnAMS_NOTOK;
	}
}


INT CACF::SetDACValue(DOUBLE dPressure)
{	
	return m_stDACPort.SetDACValue(dPressure);
}

INT CACF::CheckACFReady()
{
	INT nResult = GMP_SUCCESS;

	return nResult;
}

/////////////////////////////////////////////////////////////////
//SETUP Related
/////////////////////////////////////////////////////////////////
VOID CACF::SetDiagnSteps(ULONG ulSteps)
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

VOID CACF::ModuleSetupPreTask()
{
	m_bCheckMotor = TRUE;
	m_bCheckIO = TRUE;

	CheckModSelected(TRUE);

	UpdateModuleSetupPara();
	UpdateModuleSetupPosn();
}

VOID CACF::ModuleSetupPostTask()
{
	m_bCheckMotor = FALSE;
	m_bCheckIO = FALSE;
	MoveStripperX(ACF_STRIPPER_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
}

BOOL CACF::ModuleSetupSetPara(PARA stPara)
{

	switch (stPara.lSetupTitle)
	{
	//case 0:
	//	m_stHeadDownSol.SetOnDelay(stPara.lPara);
	//	break;

	//case 1:
	//	m_stHeadDownSol.SetOffDelay(stPara.lPara);
	//	break;

	//case 2:
	//	m_stCounterBalanceSol.SetOnDelay(stPara.lPara);
	//	break;

	//case 3:
	//	m_stCounterBalanceSol.SetOffDelay(stPara.lPara);
	//	break;

	case 0:
		m_stACFCutterOnSol.SetOnDelay(stPara.lPara);
		break;

	case 1:
		m_stACFCutterOnSol.SetOffDelay(stPara.lPara);
		break;

	//case 2:
	//	m_stHeadUpSol.SetOnDelay(stPara.lPara);
	//	break;

	//case 3:
	//	m_stHeadUpSol.SetOffDelay(stPara.lPara);
	//	break;

	//case 8:
	//	m_stStripperSol.SetOnDelay(stPara.lPara);
	//	break;

	//case 9:
	//	m_stStripperSol.SetOffDelay(stPara.lPara);
	//	break;

	case 2:
		m_lPreAttachDelay = stPara.lPara;
		break;

	case 3:
		m_lAttachACFDelay = stPara.lPara;
		break;

	case 10: //20150223
		if (stPara.dPara > 3.0 || stPara.dPara < 0.01)
		{
			HMIMessageBox(MSG_OK, "WARNING", "Valid Range is 0.01 to 3. Operation Abort!");
		}
		else
		{
			m_dStripperXGoMoveLeftSpeedFactor = stPara.dPara;
		}
		break;

	case 11:
		if (stPara.dPara > 3.0 || stPara.dPara < 0.01)
		{
			HMIMessageBox(MSG_OK, "WARNING", "Valid Range is 0.01 to 3. Operation Abort!");
		}
		else
		{
			m_dStripperXGoMoveRightSpeedFactor = stPara.dPara;
		}
		break;
	}

	return CAC9000Stn::ModuleSetupSetPara(stPara);
}

VOID CACF::UpdateModuleSetupPara()
{	
	INT i = 0;
	INT j = 0;

	 //Para Tab
	 //Delay
	i = 0;
	//g_stSetupPara[i].szPara.Format("Head Down Sol On Delay (ms)");		//p20121017
	//g_stSetupPara[i].bEnable		= TRUE;
	//g_stSetupPara[i].bVisible		= TRUE;
	//g_stSetupPara[i].lType			= MACHINE_PARA;
	//g_stSetupPara[i].lPara			= m_stHeadDownSol.GetOnDelay();
	//g_stSetupPara[i].dPara			= 0.0;

	//i++;
	//g_stSetupPara[i].szPara.Format("Head Down Sol Off Delay (ms)");		//p20121017
	//g_stSetupPara[i].bEnable		= TRUE;
	//g_stSetupPara[i].bVisible		= TRUE;
	//g_stSetupPara[i].lType			= MACHINE_PARA;
	//g_stSetupPara[i].lPara			= m_stHeadDownSol.GetOffDelay();
	//g_stSetupPara[i].dPara			= 0.0;

	//i++;
	//g_stSetupPara[i].szPara.Format("Counter Balance On Delay (ms)");
	//g_stSetupPara[i].bEnable		= TRUE;
	//g_stSetupPara[i].bVisible		= TRUE;
	//g_stSetupPara[i].lType			= MACHINE_PARA;
	//g_stSetupPara[i].lPara			= m_stCounterBalanceSol.GetOnDelay();
	//g_stSetupPara[i].dPara			= 0.0;

	//i++;
	//g_stSetupPara[i].szPara.Format("Counter Balance Off Delay (ms)");
	//g_stSetupPara[i].bEnable		= TRUE;
	//g_stSetupPara[i].bVisible		= TRUE;
	//g_stSetupPara[i].lType			= MACHINE_PARA;
	//g_stSetupPara[i].lPara			= m_stCounterBalanceSol.GetOffDelay();
	//g_stSetupPara[i].dPara			= 0.0;

	//i++;
	g_stSetupPara[i].szPara.Format("Halfcut On Delay (ms)");
	g_stSetupPara[i].bEnable		= TRUE;
	g_stSetupPara[i].bVisible		= TRUE;
	g_stSetupPara[i].lType			= MACHINE_PARA;
	g_stSetupPara[i].lPara			= m_stACFCutterOnSol.GetOnDelay();
	g_stSetupPara[i].dPara			= 0.0;

	i++;
	g_stSetupPara[i].szPara.Format("Halfcut Off Delay (ms)");
	g_stSetupPara[i].bEnable		= TRUE;
	g_stSetupPara[i].bVisible		= TRUE;
	g_stSetupPara[i].lType			= MACHINE_PARA;
	g_stSetupPara[i].lPara			= m_stACFCutterOnSol.GetOffDelay();
	g_stSetupPara[i].dPara			= 0.0;

	//i++;
	//g_stSetupPara[i].szPara.Format("Head Up On Delay (ms)");
	//g_stSetupPara[i].bEnable		= TRUE;
	//g_stSetupPara[i].bVisible		= TRUE;
	//g_stSetupPara[i].lType			= MACHINE_PARA;
	//g_stSetupPara[i].lPara			= m_stHeadUpSol.GetOnDelay();
	//g_stSetupPara[i].dPara			= 0.0;

	//i++;
	//g_stSetupPara[i].szPara.Format("Head Up Off Delay (ms)");
	//g_stSetupPara[i].bEnable		= TRUE;
	//g_stSetupPara[i].bVisible		= TRUE;
	//g_stSetupPara[i].lType			= MACHINE_PARA;
	//g_stSetupPara[i].lPara			= m_stHeadUpSol.GetOffDelay();
	//g_stSetupPara[i].dPara			= 0.0;

	//i++;
	//g_stSetupPara[i].szPara.Format("Stripper On Delay (ms)");
	//g_stSetupPara[i].bEnable		= TRUE;
	//g_stSetupPara[i].bVisible		= TRUE;
	//g_stSetupPara[i].lType			= MACHINE_PARA;
	//g_stSetupPara[i].lPara			= m_stStripperSol.GetOnDelay();
	//g_stSetupPara[i].dPara			= 0.0;

	//i++;
	//g_stSetupPara[i].szPara.Format("Stripper Vac Off Delay (ms)");
	//g_stSetupPara[i].bEnable		= TRUE;
	//g_stSetupPara[i].bVisible		= TRUE;
	//g_stSetupPara[i].lType			= MACHINE_PARA;
	//g_stSetupPara[i].lPara			= m_stStripperSol.GetOffDelay();
	//g_stSetupPara[i].dPara			= 0.0;
	
	//i++;	// 2
	//g_stSetupPara[i].szPara.Format("Pre-Attach Delay (ms)");
	//g_stSetupPara[i].bEnable		= TRUE;
	//g_stSetupPara[i].bVisible		= TRUE;
	//g_stSetupPara[i].lType			= DEVICE_PARA;
	//g_stSetupPara[i].lPara			= m_lPreAttachDelay;
	//g_stSetupPara[i].dPara			= 0.0;

	//i++;	// 3
	//g_stSetupPara[i].szPara.Format("Attach ACF Delay (ms)");
	//g_stSetupPara[i].bEnable		= TRUE;
	//g_stSetupPara[i].bVisible		= TRUE;
	//g_stSetupPara[i].lType			= DEVICE_PARA;
	//g_stSetupPara[i].lPara			= m_lAttachACFDelay;
	//g_stSetupPara[i].dPara			= 0.0;
	
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

	i = 10;	// 10
	g_stSetupPara[i].szPara.Format("Stripper Move Left Speed Factor");
	g_stSetupPara[i].bEnable		= TRUE;
	g_stSetupPara[i].bVisible		= TRUE;
	g_stSetupPara[i].lType			= MACHINE_PARA;
	g_stSetupPara[i].lPara			= 0;
	g_stSetupPara[i].dPara			= m_dStripperXGoMoveLeftSpeedFactor;

	i++;	// 11
	g_stSetupPara[i].szPara.Format("Stripper Move Right Speed Factor");
	g_stSetupPara[i].bEnable		= TRUE;
	g_stSetupPara[i].bVisible		= TRUE;
	g_stSetupPara[i].lType			= MACHINE_PARA;
	g_stSetupPara[i].lPara			= 0;
	g_stSetupPara[i].dPara			= m_dStripperXGoMoveRightSpeedFactor;

}

VOID CACF::UpdateModuleSetupPosn()
{	
	INT i = 0;
	INT j = 0;

	// Assign Module Setup Posn/Level/Para
	// Position Tab
	i = 0;
	g_stSetupPosn[i].szPosn.Format("Stripper Standby Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lStandbyPosn;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 1
	g_stSetupPosn[i].szPosn.Format("Stripper Out Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lOutPosn;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 2
	g_stSetupPosn[i].szPosn.Format("Stripper Out Posn Offset");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= DEVICE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lOutOffset;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 3
	g_stSetupPosn[i].szPosn.Format("Standby Level");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lStandbyLevel;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 4
	g_stSetupPosn[i].szPosn.Format("Calib Level");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lCalibLevel;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 5
	g_stSetupPosn[i].szPosn.Format("Bond Level");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= DEVICE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lBondLevel;
	g_stSetupPosn[i].lPosnY			= 0;

	i++;	// 6
	g_stSetupPosn[i].szPosn.Format("BH Standby Posn");
	g_stSetupPosn[i].bEnable		= TRUE;
	g_stSetupPosn[i].bVisible		= TRUE;
	g_stSetupPosn[i].lType			= MACHINE_PARA;
	g_stSetupPosn[i].lPosnX			= m_lStandbyPosnACF;
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

BOOL CACF::ModuleSetupSetPosn(LONG lSetupTitle)
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

	TempPosn.x = m_stMotorStripperX.GetEncPosn();

	switch (lSetupTitle)
	{
	case 0:
		mtrOld.x			= m_lStandbyPosn;

		m_lStandbyPosn		= TempPosn.x;
		break;

	case 1:
		mtrOld.x			= m_lOutPosn;

		m_lOutPosn			= TempPosn.x;
		break;

	case 2:
		mtrOld.x			= m_lOutOffset;

		m_lOutOffset			= TempPosn.x - m_lOutPosn;

	case 3:
		TempPosn.x = m_stMotorZ.GetEncPosn();
		mtrOld.x			= m_lStandbyLevel;

		m_lStandbyLevel		= TempPosn.x;
		break;

	case 4:
		TempPosn.x = m_stMotorZ.GetEncPosn();
		mtrOld.x			= m_lCalibLevel;

		m_lCalibLevel		= TempPosn.x;
		break;

	case 5:
		TempPosn.x = m_stMotorZ.GetEncPosn();
		mtrOld.x			= m_lBondLevel;

		m_lBondLevel		= TempPosn.x;
		break;

	case 6:
		TempPosn.x = m_stMotorX.GetEncPosn();
		mtrOld.x			= m_lStandbyPosnACF;

		m_lStandbyPosnACF		= TempPosn.x;
		break;
	}

	CAC9000Stn::ModuleSetupSetPosn(lSetupTitle, "X", mtrOld.x, TempPosn.x);

	return TRUE;
}

INT CACF::SetupGoStandbyLevel()	// 20150205
{
	INT nResult = GMP_SUCCESS;

	nResult = MoveZ(ACF_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT);	

	return nResult;
}

INT CACF::SetupGoCalibLevel()	// 20150205
{
	INT nResult = GMP_SUCCESS;

	nResult = MoveZ(ACF_Z_CTRL_GO_CALIB_LEVEL, GMP_WAIT);	

	return nResult;
}

INT CACF::SetupGoBondLevel()	// 20150205
{
	INT nResult = GMP_SUCCESS;

	nResult = MoveZ(ACF_Z_CTRL_GO_BOND_LEVEL, GMP_WAIT);	

	return nResult;
}

INT CACF::SetupGoStandbyPosnACF()	// 20150205
{
	INT nResult = GMP_SUCCESS;

	nResult = MoveX(ACF_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);	

	return nResult;
}

INT CACF::SetupGoStandbyPosn()	// 20150205
{
	INT nResult = GMP_SUCCESS;

	nResult = MoveStripperX(ACF_STRIPPER_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);	

	return nResult;
}

INT CACF::SetupGoOutPosn()	// 20150205
{
	INT nResult = GMP_SUCCESS;

	nResult = MoveStripperX(ACF_STRIPPER_X_CTRL_GO_OUT_POSN	, GMP_WAIT);	

	return nResult;
}

INT CACF::SetupGoOutOffset()	// 20150205
{
	INT nResult = GMP_SUCCESS;

	nResult = MoveStripperX(ACF_STRIPPER_X_CTRL_GO_OUT_OFFSET, GMP_WAIT);	

	return nResult;
}

BOOL CACF::ModuleSetupGoPosn(LONG lSetupTitle)
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
		nResult = SetupGoStandbyPosn();
		break;

	case 1:
		nResult = SetupGoOutPosn();
		break;

	case 2:
		nResult = SetupGoOutOffset();
		break;

	case 3:
		nResult = SetupGoStandbyLevel();
		break;

	case 4:
		nResult = SetupGoCalibLevel();
		break;

	case 5:
		nResult = SetupGoBondLevel();
		break;

	case 6:
		nResult = SetupGoStandbyPosnACF();
		break;
	}

	return nResult;
}

VOID CACF::PrintMachineSetup(FILE *fp)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString str = "";

	if (fp != NULL)
	{
		fprintf(fp, "\n<<<<%s>>>>\n", (const char*)GetStnName());
		fprintf(fp, "======================================================\n");
		fprintf(fp, "<<Para>>\n");
		//fprintf(fp, "%30s\t : %ld\n", "Head Down Sol On Delay",		//p20121017
		//		(LONG)pAppMod->m_smfMachine[GetStnName()]["Delays"][m_stHeadDownSol.GetName()]["lOnDelay"]);
		//fprintf(fp, "%30s\t : %ld\n", "Head Down Sol Off Delay",		//p20121017
		//		(LONG)pAppMod->m_smfMachine[GetStnName()]["Delays"][m_stHeadDownSol.GetName()]["lOffDelay"]);
		//fprintf(fp, "%30s\t : %ld\n", "CounterBalance On Delay",
		//		(LONG)pAppMod->m_smfMachine[GetStnName()]["Delays"][m_stCounterBalanceSol.GetName()]["lOnDelay"]);
		//fprintf(fp, "%30s\t : %ld\n", "CounterBalance Off Delay",
		//		(LONG)pAppMod->m_smfMachine[GetStnName()]["Delays"][m_stCounterBalanceSol.GetName()]["lOffDelay"]);
		//fprintf(fp, "%30s\t : %ld\n", "Head Up On Delay",
		//		(LONG)pAppMod->m_smfMachine[GetStnName()]["Delays"][m_stHeadUpSol.GetName()]["lOnDelay"]);
		//fprintf(fp, "%30s\t : %ld\n", "Head Up Off Delay",
		//		(LONG)pAppMod->m_smfMachine[GetStnName()]["Delays"][m_stHeadUpSol.GetName()]["lOffDelay"]);
//		fprintf(fp, "%30s\t : %ld\n", "Stripper On Delay",
//				(LONG)pAppMod->m_smfMachine[GetStnName()]["Delays"][m_stStripperSol.GetName()]["lOnDelay"]);
//		fprintf(fp, "%30s\t : %ld\n", "Stripper Off Delay",
//				(LONG)pAppMod->m_smfMachine[GetStnName()]["Delays"][m_stStripperSol.GetName()]["lOffDelay"]);
		fprintf(fp, "%30s\t : %.2f\n", "Cutter To Head Distance",
				(DOUBLE)pAppMod->m_smfMachine[GetStnName()]["Calib"]["m_dCutterToHeadDistance"]);
		fprintf(fp, "%30s\t : %.2f\n", "Cutter To Head Offset",
				(DOUBLE)pAppMod->m_smfMachine[GetStnName()]["Calib"]["m_dCutterToHeadOffset"]);
		fprintf(fp, "%30s\t : %.2f\n", "ACF Indexer length Factor",
				(DOUBLE)pAppMod->m_smfMachine[GetStnName()]["Calib"]["m_dIndexerCustomFactor"]); //20120706
	}

	CAC9000Stn::PrintMachineSetup(fp);
}

VOID CACF::PrintDeviceSetup(FILE *fp)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString str, str1;

	if (fp != NULL)
	{
		fprintf(fp, "\n<<<<%s>>>>\n", (const char*)GetStnName());
		fprintf(fp, "======================================================\n");
		fprintf(fp, "<<Para>>\n");
		fprintf(fp, "%30s\t : %ld\n", "Half Cut On Delay",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Delays"][m_stACFCutterOnSol.GetName()]["lOnDelay"]);
		fprintf(fp, "%30s\t : %ld\n", "Half Cut Off Delay",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Delays"][m_stACFCutterOnSol.GetName()]["lOffDelay"]);
		fprintf(fp, "%30s\t : %ld\n", "Pre-Attach Delay",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Delays"]["m_lPreAttachDelay"]);
		fprintf(fp, "%30s\t : %ld\n", "Attach ACF Delay",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["Delays"]["m_lAttachACFDelay"]);

		fprintf(fp, "EP Regulator\n");
		fprintf(fp, "%30s\t : %ld\n", "Inital Pressure",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dPressureCtrl"]);
		fprintf(fp, "%30s\t : %ld\n", "Up Pressure",
				(LONG)pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dUpPressureCtrl"]);
		for (INT i = ACF1; i < MAX_NUM_OF_ACF; i++)
		{
			if (m_pCACFWH->m_stACF[i].bEnable)
			{
				str.Format("ACF%ld Final Pressure", i + 1);
				str1.Format("m_lFinalDACCtrl[ACF%ld]", i + 1);
				fprintf(fp, "%30s\t : %ld\n", "Final Pressure",
						(LONG)pAppMod->m_smfDevice[GetStnName()]["EPRegulator"][str1]);
			}
		}
	}

	CAC9000Stn::PrintDeviceSetup(fp);
}

VOID CACF::PrintForceCalibData(FILE *fp)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString str = "";

	if (fp != NULL)
	{
		fprintf(fp, "\n%s Force Calibration Data\n", (const char*)GetStnName());
		fprintf(fp, "======================================================\n");
		fprintf(fp, "<<Para>>\n");
		fprintf(fp, "%30s\t : %ld\n", "Calib Pre-Attach Delay",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Delays"]["m_lCalibPreAttachDelay"]);
		fprintf(fp, "%30s\t : %ld\n", "Calib Attach ACF Delay",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Delays"]["m_lCalibAttachDelay"]);
		fprintf(fp, "%30s\t : %ld\n", "Calib Press Test Delay",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Delays"]["m_lPressTestDelay"]);
		fprintf(fp, "%30s\t : %ld\n", "Force Calib Press Cycle",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Para"]["m_lForceCalibPressCycle"]);
		fprintf(fp, "%30s\t : %ld\n", "Calib Inital Pressure",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Para"]["m_dCalibPressureCtrl"]);
		fprintf(fp, "%30s\t : %ld\n", "Calib Up Pressure",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Para"]["m_dCalibUpPressureCtrl"]);
		fprintf(fp, "%30s\t : %ld\n", "Calib Final Pressure",
				(LONG)pAppMod->m_smfMachine[GetStnName()]["Para"]["m_lCalibFinalDACCtrl"]);

		fprintf(fp, "\n<<Calibration Result>>\n");
		fprintf(fp, "Pressure (bar)	: Force Calibration (kgf)\n");
		for (INT j = 0; j < MAX_BOND_FORCE_MODE; j++)
		{
			for (INT i = 0; i < MAX_FORCE_DATA; i++)
			{
				str.Format("m_stForceCalib[%ld][%ld]", j, i);
				fprintf(fp, "%.5f\t : %.5f\n",	(DOUBLE)pAppMod->m_smfMachine[GetStnName()]["ForceCalibration"][str]["Force_kgf"],
						(DOUBLE)pAppMod->m_smfMachine[GetStnName()]["ForceCalibration"][str]["Pressure_bar"]);
			}
		}
	}
}

VOID CACF::PrintCalibDataToHMI()
{
	CString szFilePath;

	szFilePath.Format("D:\\sw\\AC9000\\Param\\%sForceData.csv", GetStation());
	// Set data to file
	FILE *fp = fopen(szFilePath, "wt");
	if (fp != NULL)
	{
		for (INT i = 0; i < MAX_FORCE_DATA; i++)
		{
			fprintf(fp, "%.5f,%.5f, %.5f\n", -(DOUBLE)m_stForceCalib[m_lCurrBondForceMode][i].OpenDacValue, m_stForceCalib[m_lCurrBondForceMode][i].Force_kgf, (DOUBLE)i * 1.0);
		}

		fclose(fp);
	}
	
	szFilePath.Format("D:\\sw\\AC9000\\Param\\%sForceData(Force).csv", GetStation());
	fp = fopen(szFilePath, "wt");
	if (fp != NULL)
	{
		if (g_bShowForceInNewton)
		{
			fprintf(fp, "Force (N),");
		}
		else
		{
			fprintf(fp, "Force (Kgf),");
		}

		for (INT i = 0; i < MAX_FORCE_DATA; i++)
		{
			if (i < MAX_FORCE_DATA - 1)
			{
				fprintf(fp, "%.5f,", m_stForceCalib[m_lCurrBondForceMode][i].Force_kgf * (g_bShowForceInNewton * KGF_TO_NEWTON + (1 - (LONG)g_bShowForceInNewton)));
			}
			else
			{
				fprintf(fp, "%.5f", m_stForceCalib[m_lCurrBondForceMode][i].Force_kgf * (g_bShowForceInNewton * KGF_TO_NEWTON + (1 - (LONG)g_bShowForceInNewton)));
			}
		}

		fclose(fp);
	}
	
	szFilePath.Format("D:\\sw\\AC9000\\Param\\%sForceData(Pressure).csv", GetStation());
	fp = fopen(szFilePath, "wt");
	if (fp != NULL)
	{
		fprintf(fp, "DAC,");

		for (INT i = 0; i < MAX_FORCE_DATA; i++)
		{
			if (i < MAX_FORCE_DATA - 1)
			{
				fprintf(fp, "%.5f,", -(DOUBLE)m_stForceCalib[m_lCurrBondForceMode][i].OpenDacValue);
			}
			else
			{
				fprintf(fp, "%.5f", -(DOUBLE)m_stForceCalib[m_lCurrBondForceMode][i].OpenDacValue);
			}
		}

		fclose(fp);
	}
	
}

#if 1 //20150512
LONG CACF::HMI_InsertForceCalibrationData(IPC_CServiceMessage &svMsg)
{
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");

	INT i = 0;

	BOOL bFull = TRUE;
	BOOL bOpDone = TRUE;

	for (i = 0; i < MAX_FORCE_DATA; i++)
	{
		if (m_stForceCalib[m_lCurrBondForceMode][i].Force_kgf == 0.0)
		{
			bFull = FALSE;
			break;
		}
	}

	if (bFull)
	{
		HMIMessageBox(MSG_OK, "WARNING", "Data Set is Full.");

		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
		return 1;
	}

	// get data
	LONG lDAC;
	DOUBLE dForce;

	if (!HMINumericKeys("DAC", 20000, -20000, &lDAC) || !HMINumericKeys("Force (Kgf)", 40.0, 0.1, &dForce))
	{
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
		return 1;
	}

	//if (lDAC == 0)
	//{
	//	lDAC = 0.000001;
	//}

	m_stForceCalib[m_lCurrBondForceMode][i].OpenDacValue	= -lDAC;
	m_stForceCalib[m_lCurrBondForceMode][i].Force_kgf		= dForce;

	// sort data
	SortForceCalibData(m_stForceCalib[m_lCurrBondForceMode]);

	// Set data to file
	PrintCalibDataToHMI();

	//Converse the current DAC to new scale
	DacToForce(m_lCalibFinalDACCtrl, m_stForceCalib[m_lCurrBondForceMode], &m_dCalibForceCtrl);
	HMI_dCalibForceCtrl			= m_dCalibForceCtrl * (g_bShowForceInNewton * KGF_TO_NEWTON + (1 - (LONG)g_bShowForceInNewton));
	UpdateCurPressureValue();
	
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}

LONG CACF::HMI_ResetForceCalibrationData(IPC_CServiceMessage &svMsg)
{
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");

	LONG lChannel = 0;

	svMsg.GetMsg(sizeof(LONG), &lChannel);


	if (lChannel >= MAX_FORCE_DATA)
	{
		HMIMessageBox(MSG_OK, "WARNING", "Invalid Entry.");
		return 0;
	}

	m_stForceCalib[m_lCurrBondForceMode][lChannel].OpenDacValue	= 0;
	m_stForceCalib[m_lCurrBondForceMode][lChannel].Force_kgf		= 0.0;

	// sort data
	SortForceCalibData(m_stForceCalib[m_lCurrBondForceMode]);

	// Set data to file
	PrintCalibDataToHMI();

	DacToForce(m_lCalibFinalDACCtrl, m_stForceCalib[m_lCurrBondForceMode], &m_dCalibForceCtrl);
	HMI_dCalibForceCtrl			= m_dCalibForceCtrl * (g_bShowForceInNewton * KGF_TO_NEWTON + (1 - (LONG)g_bShowForceInNewton));
	UpdateCurPressureValue();

	return 0;
}
#endif

#ifdef EXCEL_MACHINE_DEVICE_INFO
BOOL CACF::printMachineDeviceInfo()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bResult = TRUE;
	CString szBlank("--");
	CString szModule = GetStnName();
	LONG lValue = 0;
	DOUBLE dValue = 0.0;
	CString strTitle;

	for (INT j = 0; j < MAX_BOND_FORCE_MODE; j++)
	{
		for (INT i = 0; i < MAX_FORCE_DATA; i++)
		{
			strTitle.Format("m_stForceCalib[%ld][%ld]", j, i);

			dValue = (DOUBLE) pAppMod->m_smfMachine[GetStnName()]["ForceCalibration"][strTitle]["Force_kgf"];
			bResult = printMachineInfoRow(szModule, CString("ForceCalibration"), strTitle, CString("Force_kgf"), szBlank,
										szBlank, szBlank, szBlank, szBlank, szBlank, dValue);

			dValue = (DOUBLE) pAppMod->m_smfMachine[GetStnName()]["ForceCalibration"][strTitle]["Pressure_bar"];
			bResult = printMachineInfoRow(szModule, CString("ForceCalibration"), strTitle, CString("Pressure_bar"), szBlank,
										szBlank, szBlank, szBlank, szBlank, szBlank, dValue);

			lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["ForceCalibration"][strTitle]["OpenDacValue"];
			bResult = printMachineInfoRow(szModule, CString("ForceCalibration"), strTitle, CString("OpenDacValue"), szBlank,
										szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
		}
	}
	if (bResult)
	{
		dValue = (DOUBLE) pAppMod->m_smfMachine[GetStnName()]["Calib"]["m_dCutterToHeadDistance"];
		bResult = printMachineInfoRow(szModule, CString("Calib"), CString("CutterToHeadDistance"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, dValue);
	}
	if (bResult)
	{
		dValue = (DOUBLE) pAppMod->m_smfMachine[GetStnName()]["Calib"]["m_dIndexerCustomFactor"];
		bResult = printMachineInfoRow(szModule, CString("Calib"), CString("IndexerCustomFactor"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, dValue);
	}
	if (bResult)
	{
		dValue = (DOUBLE) pAppMod->m_smfMachine[GetStnName()]["Calib"]["m_dIndexerCustomFactor"];
		bResult = printMachineInfoRow(szModule, CString("Calib"), CString("IndexerCustomFactor"), szBlank, szBlank, 
									  szBlank, szBlank, szBlank, szBlank, szBlank, dValue);
	}

	//Force Calib Para
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Delays"]["m_lPressTestDelay"];
		bResult = printMachineInfoRow(szModule, CString("Delays"), CString("PressTestDelay"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Delays"]["m_lCalibPreAttachDelay"];
		bResult = printMachineInfoRow(szModule, CString("Delays"), CString("CalibPreAttachDelay"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Delays"]["m_lCalibAttachDelay"];
		bResult = printMachineInfoRow(szModule, CString("Delays"), CString("CalibAttachDelay"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Para"]["m_lForceCalibPressCycle"];
		bResult = printMachineInfoRow(szModule, CString("Para"), CString("ForceCalibPressCycle"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Para"]["m_lCalibFinalDACCtrl"];
		bResult = printMachineInfoRow(szModule, CString("Para"), CString("CalibFinalDACCtrl"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		dValue = (DOUBLE) pAppMod->m_smfMachine[GetStnName()]["Para"]["m_dCalibUpPressureCtrl"];
		bResult = printMachineInfoRow(szModule, CString("Para"), CString("CalibUpPressureCtrl"), szBlank, szBlank, 
									  szBlank, szBlank, szBlank, szBlank, szBlank, dValue);
	}
	if (bResult)
	{
		dValue = (DOUBLE) pAppMod->m_smfMachine[GetStnName()]["Para"]["m_dCalibPressureCtrl"];
		bResult = printMachineInfoRow(szModule, CString("Para"), CString("CalibPressureCtrl"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, dValue);
	}

	if (bResult)
	{
		bResult = CAC9000Stn::printMachineDeviceInfo();
	}
	return bResult;
}
#endif

//-----------------------------------------------------------------------------
// HMI command implementation
//-----------------------------------------------------------------------------
//
LONG CACF::HMI_ToggleModSelected(IPC_CServiceMessage &svMsg) //20121112
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	SetModSelected(bMode);
	m_stMotorIndex.PowerOn(bMode);
	m_stMotorRoller.PowerOn(bMode);
	m_stMotorStripperX.PowerOn(bMode);	//20150205

	bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CACF::HMI_PowerOnIndex(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	LogAction(__FUNCTION__);

	m_stMotorIndex.PowerOn(bMode);

	bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CACF::HMI_PowerOnRoller(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	LogAction(__FUNCTION__);

	m_stMotorRoller.PowerOn(bMode);

	bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CACF::HMI_PowerOnStripperX(IPC_CServiceMessage &svMsg)		//20150205
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	LogAction(__FUNCTION__);

	if(!bMode && (MoveStripperX(ACF_STRIPPER_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) == GMP_SUCCESS))
	{
		m_stMotorStripperX.PowerOn(bMode);
	}
	else if(bMode)
	{
		m_stMotorStripperX.PowerOn(bMode);
	}

	bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CACF::HMI_PowerOnX(IPC_CServiceMessage &svMsg)		//20150205
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	LogAction(__FUNCTION__);

	m_stMotorX.PowerOn(bMode);

	bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CACF::HMI_PowerOnZ(IPC_CServiceMessage &svMsg)		//20150205
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	LogAction(__FUNCTION__);

	if(!bMode && (MoveStripperX(ACF_STRIPPER_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) == GMP_SUCCESS))
	{
		m_stMotorZ.PowerOn(bMode);
	}
	else if(bMode)
	{
		m_stMotorStripperX.PowerOn(bMode);
	}

	bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

LONG CACF::HMI_PowerOnForceZ(IPC_CServiceMessage &svMsg)		//20150205
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	LogAction(__FUNCTION__);

	m_stMotorBF.PowerOn(bMode);
	//m_stMotorBF.OpenDAC(1000, -m_lFinalDACCtrl[ACF1], 4000, 0, 0);
	m_stMotorBF.OpenDAC(1000, 0, 4000, 0, 0);
	bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

//LONG CACF::HMI_ToggleHeadDownSol(IPC_CServiceMessage &svMsg)
//{
	//if (m_stHeadDownSol.bHMI_Status)
	//{
	//	if ((m_nLastError = SetHeadDownSol(OFF, GMP_WAIT)) != GMP_SUCCESS || getKlocworkFalse()) //klocwork fix 20121211
	//	{
	//		SetError(m_nLastError);
	//	}
	//}
	//else
	//{
	//	if ((m_nLastError = SetHeadDownSol(ON, GMP_WAIT)) != GMP_SUCCESS || getKlocworkFalse()) //klocwork fix 20121211
	//	{
	//		SetError(m_nLastError);
	//	}
	//}

//	return 0;
//}

//LONG CACF::HMI_ToggleCounterBalanceSol(IPC_CServiceMessage &svMsg)
//{
	//if (m_stCounterBalanceSol.bHMI_Status)
	//{
	//	if ((m_nLastError = SetCounterBalanceSol(OFF, GMP_WAIT)) != GMP_SUCCESS || getKlocworkFalse()) //klocwork fix 20121211
	//	{
	//		SetError(m_nLastError);
	//	}
	//}
	//else
	//{
	//	if ((m_nLastError = SetCounterBalanceSol(ON, GMP_WAIT)) != GMP_SUCCESS || getKlocworkFalse()) //klocwork fix 20121211
	//	{
	//		SetError(m_nLastError);
	//	}
	//}

//	return 0;
//}

LONG CACF::HMI_ToggleCutterOnSol(IPC_CServiceMessage &svMsg)
{
	if (m_stACFCutterOnSol.bHMI_Status)
	{
		if ((m_nLastError = SetHalfCutterOnSol(OFF, GMP_WAIT)) != GMP_SUCCESS || getKlocworkFalse()) //klocwork fix 20121211
		{
			SetError(m_nLastError);
		}
	}
	else
	{
		if ((m_nLastError = SetHalfCutterOnSol(ON, GMP_WAIT)) != GMP_SUCCESS || getKlocworkFalse()) //klocwork fix 20121211
		{
			SetError(m_nLastError);
		}
	}

	return 0;
}

//LONG CACF::HMI_ToggleHeadUpSol(IPC_CServiceMessage &svMsg)
//{
	//if (m_stHeadUpSol.bHMI_Status)
	//{
	//	if ((m_nLastError = SetHeadUpSol(OFF, GMP_WAIT)) != GMP_SUCCESS || getKlocworkFalse()) //klocwork fix 20121211
	//	{
	//		SetError(m_nLastError);
	//	}
	//}
	//else
	//{
	//	if ((m_nLastError = SetHeadUpSol(ON, GMP_WAIT)) != GMP_SUCCESS || getKlocworkFalse()) //klocwork fix 20121211
	//	{
	//		SetError(m_nLastError);
	//	}
	//}

//	return 0;
//}

//LONG CACF::HMI_ToggleStripperSol(IPC_CServiceMessage &svMsg)
//{
//	if (m_stStripperSol.bHMI_Status)
//	{
//		if ((m_nLastError = SetStripperSol(OFF, GMP_WAIT)) != GMP_SUCCESS)
//		{
//			SetError(m_nLastError);
//		}
//	}
//	else
//	{
//		if ((m_nLastError = SetStripperSol(ON, GMP_WAIT)) != GMP_SUCCESS)
//		{
//			SetError(m_nLastError);
//		}
//	}
//
//	return 0;
//}// 20150204
INT CACF::SetForceChargeAmpRangeSol(BOOL bMode, BOOL bWait)
{
	//Turn on this SOL. Range II is selected
	INT nResult = GMP_SUCCESS;
	CSoInfo *stTempSo = &m_stACFForceAmpRangellSol;
#ifdef OFFLINE
	stTempSo->bHMI_Status = bMode;
	return nResult;
#endif
#ifdef _DEBUG
	
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
			if (bWait)
			{
				Sleep(stTempSo->GetOnDelay());
			}
			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());
			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;
			if (bWait)
			{
				Sleep(stTempSo->GetOffDelay());
			}
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

INT CACF::SetForceChargeAmpOperateSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;
	CSoInfo *stTempSo = &m_stACFForceAmpOpSol;
#ifdef OFFLINE
	stTempSo->bHMI_Status = bMode;
	return nResult;
#endif
#ifdef _DEBUG
	
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
			if (bWait)
			{
				Sleep(stTempSo->GetOnDelay());
			}
			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());
			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;
			if (bWait)
			{
				Sleep(stTempSo->GetOffDelay());
			}
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

LONG CACF::HMI_ToggleForceChargeAmpOperateSol(IPC_CServiceMessage &svMsg)
{
	SetForceChargeAmpOperateSol(!m_stACFForceAmpOpSol.bHMI_Status);
	return 0;
}
//AutoGen
LONG CACF::HMI_ToggleForceChargeAmpRangeSol(IPC_CServiceMessage &svMsg)
{
	SetForceChargeAmpRangeSol(!m_stACFForceAmpRangellSol.bHMI_Status);
	return 0;
}

LONG CACF::HMI_ToggleIonizerPowerEnable(IPC_CServiceMessage &svMsg)
{
	if (m_stACFIonizerPowerEnSol.bHMI_Status)
	{
		if ((m_nLastError = SetIonizerPowerEnable(OFF, GMP_WAIT)) != GMP_SUCCESS || getKlocworkFalse()) //klocwork fix 20121211
		{
			SetError(m_nLastError);
		}
	}
	else
	{
		if ((m_nLastError = SetIonizerPowerEnable(ON, GMP_WAIT)) != GMP_SUCCESS || getKlocworkFalse()) //klocwork fix 20121211
		{
			SetError(m_nLastError);
		}
	}

	return 0;
}

LONG CACF::HMI_ResetNumOfPressCount(IPC_CServiceMessage &svMsg)		//20150416
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	LogAction(__FUNCTION__);

	m_lCurNumOfPressCount = 0;
	pAppMod->m_smfMachine[GetStnName()]["Para"]["m_lCurNumOfPressCount"]			= m_lCurNumOfPressCount;
	pAppMod->m_smfMachine.Update();

	return 0;
}

LONG CACF::HMI_SetDiagnSteps(IPC_CServiceMessage &svMsg)
{
	ULONG ulSteps;
	svMsg.GetMsg(sizeof(ULONG), &ulSteps);

	g_lDiagnSteps = ulSteps;

	SetDiagnSteps(ulSteps);	
	return 0;
}

LONG CACF::HMI_IndexCCW(IPC_CServiceMessage &svMsg)
{
	MoveRelativeIndexer(-m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CACF::HMI_IndexCW(IPC_CServiceMessage &svMsg)
{
	MoveRelativeIndexer(m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CACF::HMI_RollerCCW(IPC_CServiceMessage &svMsg)
{
	m_stMotorRoller.MoveRelative(m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CACF::HMI_IndexZNeg(IPC_CServiceMessage &svMsg)	// 20150205
{
	m_stMotorZ.MoveRelative(-m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CACF::HMI_IndexZPos(IPC_CServiceMessage &svMsg)	// 20150205
{
	m_stMotorZ.MoveRelative(m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CACF::HMI_IndexXNeg(IPC_CServiceMessage &svMsg)	// 20150205
{
	m_stMotorX.MoveRelative(-m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CACF::HMI_IndexXPos(IPC_CServiceMessage &svMsg)	// 20150205
{
	m_stMotorX.MoveRelative(m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CACF::HMI_IndexStripperXNeg(IPC_CServiceMessage &svMsg)	// 20150205
{
	m_stMotorStripperX.SetCustMoveProf(m_dStripperXGoMoveLeftSpeedFactor);
	m_stMotorStripperX.MoveRelative(-m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CACF::HMI_IndexStripperXPos(IPC_CServiceMessage &svMsg)	// 20150205
{
	m_stMotorStripperX.SetCustMoveProf(m_dStripperXGoMoveRightSpeedFactor);
	m_stMotorStripperX.MoveRelative(m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CACF::HMI_HomeX(IPC_CServiceMessage &svMsg)	// 20150205
{
	m_stMotorX.Home(GMP_WAIT);

	return 0;
}

LONG CACF::HMI_HomeZ(IPC_CServiceMessage &svMsg)	// 20150205
{
	m_stMotorZ.Home(GMP_WAIT);

	return 0;
}

LONG CACF::HMI_HomeStripperX(IPC_CServiceMessage &svMsg)	// 20150205
{
	m_stMotorStripperX.Home(GMP_WAIT);

	return 0;
}

LONG CACF::HMI_CommX(IPC_CServiceMessage &svMsg)	// 20150205
{
	m_stMotorX.Commutate();

	return 0;
}

LONG CACF::HMI_CommZ(IPC_CServiceMessage &svMsg)	// 20150205
{
	m_stMotorZ.Commutate();

	return 0;
}

LONG CACF::HMI_CommForceZ(IPC_CServiceMessage &svMsg)	// 20150205
{
	m_stMotorBF.Commutate();

	return 0;
}

LONG CACF::HMI_CommStripperX(IPC_CServiceMessage &svMsg)	// 20150205
{
	m_stMotorStripperX.Commutate();

	return 0;
}

LONG CACF::HMI_RollerCW(IPC_CServiceMessage &svMsg)
{
	m_stMotorRoller.MoveRelative(-m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CACF::HMI_RollerGoSearchUpper(IPC_CServiceMessage &svMsg)
{
	MoveRoller(ACF_ROLLER_CTRL_SEARCH_UPPER, GMP_WAIT);

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

//Force
LONG CACF::HMI_SetDAC(IPC_CServiceMessage &svMsg)
{
	DOUBLE dValue;
	svMsg.GetMsg(sizeof(DOUBLE), &dValue);

	m_dPressureCtrl = dValue;

	SetDACValue(m_dPressureCtrl);
	return 0;
}

LONG CACF::HMI_SetFinalDAC(IPC_CServiceMessage &svMsg)
{
	LONG lAnswer = rMSG_TIMEOUT;
	LONG lValue;
	DOUBLE dForce;
	svMsg.GetMsg(sizeof(LONG), &lValue);

	BOOL bValid = DacToForce(lValue, m_stForceCalib[m_lCurrBondForceMode], &dForce);

	if (!bValid)
	{
		lAnswer = HMIMessageBox(MSG_YES_NO, "WARNING", "Desire Pressure Value Is Out Of Calibration Range. Ignore warning?");
		if (lAnswer == rMSG_NO || lAnswer == rMSG_TIMEOUT)
		{
			return 0;
		}

		m_bDACValid = FALSE;
	}

	if (abs(lValue) > ACF_MAX_FORCE)
	{
		lAnswer = HMIMessageBox(MSG_YES_NO, "WARNING", "Force exceeds recommended limit. Ignore warning?");
		if (lAnswer == rMSG_NO || lAnswer == rMSG_TIMEOUT)
		{
			return 0;
		}

		m_bDACValid = FALSE;
	}
	//else
	//{
	//	m_bDACValid = TRUE;
	//}

	if (bValid)
	{
		m_bDACValid = TRUE;
	}

	//m_dFinalPressureCtrl[m_pCACFWH->m_lCurACF] = dValue;
	m_lFinalDACCtrl[ACF_1] = lValue; //20130702

	UpdateCurPressureValue();

	return 0;
}

LONG CACF::HMI_SetUpDAC(IPC_CServiceMessage &svMsg)
{
	DOUBLE dValue;
	svMsg.GetMsg(sizeof(DOUBLE), &dValue);

	m_dUpPressureCtrl = dValue;

	return 0;
}

LONG CACF::HMI_SetForce(IPC_CServiceMessage &svMsg)
{
	LONG lAnswer = rMSG_TIMEOUT;
	LONG lValue = 0;
	DOUBLE dForce = 0.0;
	svMsg.GetMsg(sizeof(DOUBLE), &dForce);

	BOOL bValid = ForceToDac(dForce, m_stForceCalib[m_lCurrBondForceMode], &lValue);

	if (!bValid)
	{
		lAnswer = HMIMessageBox(MSG_YES_NO, "WARNING", "Desire Force Value Is Out Of Calibration Range. Ignore warning?");
		if (lAnswer == rMSG_NO || lAnswer == rMSG_TIMEOUT)
		{
			return 0;
		}

		m_bDACValid = FALSE;
	}

	if (dForce > ACF_MAX_FORCE)
	{
		lAnswer = HMIMessageBox(MSG_YES_NO, "WARNING", "Force exceeds recommended limit. Ignore warning?");
		if (lAnswer == rMSG_NO || lAnswer == rMSG_TIMEOUT)
		{
			return 0;
		}

		m_bDACValid = FALSE;
	}

	if (bValid)
	{
		m_bDACValid = TRUE;
	}

	//m_dFinalPressureCtrl[m_pCACFWH->m_lCurACF] = dValue;
	m_lFinalDACCtrl[ACF_1] = -lValue; //20130702

	UpdateCurPressureValue();

	return 0;
}


LONG CACF::HMI_SetCutterToHeadOffset(IPC_CServiceMessage &svMsg)
{
	LONG lAnswer = rMSG_TIMEOUT;

	DOUBLE dOffset;
	svMsg.GetMsg(sizeof(DOUBLE), &dOffset);

	if (m_dCutterToHeadOffset == dOffset)
	{
		return 0;
	}

	lAnswer = HMIMessageBox(MSG_YES_NO, "SET CUTTER TO HEAD OFFSET", "Auto adjust ACF to Head?");
	if (lAnswer == rMSG_TIMEOUT)
	{
		return 0;
	}
	else if (lAnswer == rMSG_YES)
	{
		if (dOffset < m_dCutterToHeadOffset)
		{
			RemoveACFArray();
		}

		m_dCutterToHeadOffset = dOffset;

		if (m_pCACFWH->m_stACF[ACF1].dLength <= (m_dCutterToHeadDistance + m_dCutterToHeadOffset))
		{
			IndexNextACFOperation();
		}
		else
		{
			IndexNextACFOperation_Single_ACF();
		}

	}
	else
	{
		m_dCutterToHeadOffset = dOffset;
	}

	return 0;
}

LONG CACF::HMI_SetCutterToHeadDistance(IPC_CServiceMessage &svMsg)
{
	LONG lAnswer = rMSG_TIMEOUT;

	DOUBLE dOffset;
	svMsg.GetMsg(sizeof(DOUBLE), &dOffset);

	m_dCutterToHeadDistance = dOffset;

	m_dCutterToHeadOffset = 0.0;

	HMIMessageBox(MSG_OK, "SET CUTTER TO HEAD", "Please re-adjust ACF manually");

	HMIMessageBox(MSG_OK, "SET CUTTER TO HEAD", "Auto Reset Cutter To Head");

	return 0;
}

LONG CACF::HMI_SetNumOfPressBeforeAlertLimit(IPC_CServiceMessage &svMsg) //20150416
{
	LONG lValue = 3000;
	svMsg.GetMsg(sizeof(LONG), &lValue);

	m_lNumOfPressBeforeAlert = lValue;

	return 0;
}

LONG CACF::HMI_SetPreAttachDelay(IPC_CServiceMessage &svMsg)
{
	LONG lDelay;
	svMsg.GetMsg(sizeof(LONG), &lDelay);

	m_lPreAttachDelay = lDelay;

	return 0;
}

LONG CACF::HMI_SetAttachACFDelay(IPC_CServiceMessage &svMsg)
{
	LONG lDelay;
	svMsg.GetMsg(sizeof(LONG), &lDelay);

	m_lAttachACFDelay = lDelay;

	return 0;
}


LONG CACF::HMI_SetPressTestDelay(IPC_CServiceMessage &svMsg)
{
	LONG lDelay;
	svMsg.GetMsg(sizeof(LONG), &lDelay);

	m_lPressTestDelay = lDelay;

	return 0;
}

// Force Calibrated
LONG CACF::HMI_SetCalibPreAttachDelay(IPC_CServiceMessage &svMsg)
{
	LONG lDelay;
	svMsg.GetMsg(sizeof(LONG), &lDelay);

	m_lCalibPreAttachDelay = lDelay;

	return 0;
}

LONG CACF::HMI_SetCalibAttachDelay(IPC_CServiceMessage &svMsg)
{
	LONG lDelay;
	svMsg.GetMsg(sizeof(LONG), &lDelay);

	m_lCalibAttachDelay = lDelay;

	return 0;
}

LONG CACF::HMI_SetCalibFinalPressureCtrl(IPC_CServiceMessage &svMsg)
{
	LONG lAnswer = rMSG_TIMEOUT;
	DOUBLE dForce;
	LONG lValue;
	svMsg.GetMsg(sizeof(LONG), &lValue);

	BOOL bValid = DacToForce(lValue, m_stForceCalib[m_lCurrBondForceMode], &dForce);

	if (!bValid)
	{
		lAnswer = HMIMessageBox(MSG_YES_NO, "WARNING", "Desire Pressure Value Is Out Of Calibration Range. Ignore warning?");
		if (lAnswer == rMSG_NO || lAnswer == rMSG_TIMEOUT)
		{
			return 0;
		}

		m_bCalibDACValid = FALSE;
	}

	if (abs(lValue) > ACF_MAX_FORCE)
	{
		lAnswer = HMIMessageBox(MSG_YES_NO, "WARNING", "Force exceeds recommended limit. Ignore warning?");
		if (lAnswer == rMSG_NO || lAnswer == rMSG_TIMEOUT)
		{
			return 0;
		}

		m_bDACValid = FALSE;
	}
	//else
	//{
	//	m_bDACValid = TRUE;
	//}

	if (bValid)
	{
		m_bCalibDACValid = TRUE;
	}


	m_lCalibFinalDACCtrl	= lValue;
	m_dCalibForceCtrl			= dForce;
	HMI_dCalibForceCtrl			= m_dCalibForceCtrl * (g_bShowForceInNewton * KGF_TO_NEWTON + (1 - (LONG)g_bShowForceInNewton));

	return 0;

}

LONG CACF::HMI_SetCalibUpPressureCtrl(IPC_CServiceMessage &svMsg)
{
	DOUBLE dValue;
	svMsg.GetMsg(sizeof(DOUBLE), &dValue);

	m_dCalibUpPressureCtrl = dValue;

	return 0;
}


LONG CACF::HMI_SetCalibPressureCtrl(IPC_CServiceMessage &svMsg)
{
	DOUBLE dValue;
	svMsg.GetMsg(sizeof(DOUBLE), &dValue);

	if (dValue < 0.01) //20150304
	{
		dValue = 0.01;
	}
	m_dCalibPressureCtrl = dValue;

	SetDACValue(m_dCalibPressureCtrl);

	return 0;
}

LONG CACF::HMI_SetCalibForceCtrl(IPC_CServiceMessage &svMsg)
{
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");

	LONG lAnswer = rMSG_TIMEOUT;
	LONG lValue = 0;
	DOUBLE dForce = 0;
	svMsg.GetMsg(sizeof(DOUBLE), &dForce);

	dForce = dForce / (g_bShowForceInNewton * KGF_TO_NEWTON + (1 - (LONG)g_bShowForceInNewton)); //20130816

	BOOL bValid = ForceToDac(dForce, m_stForceCalib[m_lCurrBondForceMode], &lValue);

	if (!bValid)
	{
		lAnswer = HMIMessageBox(MSG_YES_NO, "WARNING", "Desire Force Value Is Out Of Calibration Range. Ignore warning?");
		if (lAnswer == rMSG_NO || lAnswer == rMSG_TIMEOUT)
		{
			return 0;
		}

		m_bCalibDACValid = FALSE;
	}

	if (dForce > ACF_MAX_FORCE)
	{
		lAnswer = HMIMessageBox(MSG_YES_NO, "WARNING", "Force exceeds recommended limit. Ignore warning?");
		if (lAnswer == rMSG_NO || lAnswer == rMSG_TIMEOUT)
		{
			return 0;
		}

		m_bCalibDACValid = FALSE;
	}

	if (bValid)
	{
		m_bCalibDACValid = TRUE;
	}

	m_lCalibFinalDACCtrl		= -lValue;
	m_dCalibForceCtrl			= dForce;
	HMI_dCalibForceCtrl			= m_dCalibForceCtrl * (g_bShowForceInNewton * KGF_TO_NEWTON + (1 - (LONG)g_bShowForceInNewton));

	return 0;
}

LONG CACF::HMI_SetForceCalibPressCycle(IPC_CServiceMessage &svMsg)
{
	LONG lLimit;
	svMsg.GetMsg(sizeof(LONG), &lLimit);

	m_lForceCalibPressCycle = lLimit;

	return 0;
}

// Indexing ACF Related
LONG CACF::HMI_InsertACFArray(IPC_CServiceMessage &svMsg)
{
	InsertACFArray();
	return 0;
}

LONG CACF::HMI_RemoveACFArray(IPC_CServiceMessage &svMsg)
{
	RemoveACFArray();
	return 0;
}

LONG CACF::HMI_ClearACFArray(IPC_CServiceMessage &svMsg)
{
	ClearACFArray();
	return 0;
}

LONG CACF::HMI_SetForceSensorFactor(IPC_CServiceMessage &svMsg)
{
	DOUBLE dValue = 1.0;
	svMsg.GetMsg(sizeof(DOUBLE), &dValue);

	m_dForceSensorFactor = dValue;

	return 0;
}

LONG CACF::HMI_SetLogForceSensor(IPC_CServiceMessage &svMsg)
{
	BOOL bSelected;
	svMsg.GetMsg(sizeof(BOOL), &bSelected);

	LogAction(__FUNCTION__);
	LogAction("Value = %ld", m_bLogForceSensor);

	m_bLogForceSensor = bSelected;
	
	LogAction("Value = %ld", m_bLogForceSensor);

	return 0;
}

LONG CACF::HMI_SetLogForceSensorEndOfBond(IPC_CServiceMessage &svMsg) //20161117
{
	BOOL bSelected;
	svMsg.GetMsg(sizeof(BOOL), &bSelected);

	LogAction(__FUNCTION__);
	LogAction("Value = %ld", m_bLogForceSensorEndOfBond);

	m_bLogForceSensorEndOfBond = bSelected;
	
	LogAction("Value = %ld", m_bLogForceSensorEndOfBond);

	return 0;
}

LONG CACF::HMI_SetACFIndexerFactor(IPC_CServiceMessage &svMsg) //20120706
{
	DOUBLE dValue;
	svMsg.GetMsg(sizeof(DOUBLE), &dValue);
	
	if (dValue >= 0.2 && dValue <= 3.0)
	{
		m_dIndexerCustomFactor = dValue;
	}

	return 0;
}

//Action Related
LONG CACF::HMI_CutACFOperation(IPC_CServiceMessage &svMsg)
{
	LONG lAnswer = rMSG_TIMEOUT;

	CutACFOperation(FALSE);

	lAnswer = HMIMessageBox(MSG_YES_NO, "CUT ACF OPERATION", "Reset ACF Array?");

	if (lAnswer == rMSG_YES)		
	{
		HMIMessageBox(MSG_OK, "CUT ACF OPERATION", "Please clean ACF before the cutter.");
		ClearACFArray();
	}

	BOOL bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);

	return 1;
}


LONG CACF::HMI_IndexToACFHeadOperation(IPC_CServiceMessage &svMsg)
{

	if (m_pCACFWH->m_stACF[ACF1].dLength <= (m_dCutterToHeadDistance + m_dCutterToHeadOffset))
	{
		IndexToACFHeadOperation();
	}
	else
	{
		IndexToACFHeadOperation_Single_ACF();
	}

	BOOL bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);

	return 1;
}

LONG CACF::HMI_RecoverACFArrayOperation(IPC_CServiceMessage &svMsg)
{
	RecoverACFArrayOperation();

	BOOL bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);

	return 1;
}

LONG CACF::HMI_IndexNextACFOperation(IPC_CServiceMessage &svMsg)
{

	if (m_pCACFWH->m_stACF[ACF1].dLength <= (m_dCutterToHeadDistance + m_dCutterToHeadOffset))
	{
		CalcIndexedLength();

		if (m_bValidIndexedLength)
		{
			if (m_dRemainingLength <= 0.0005)
			{			
				DisplayMessage("ACF exists under the HEAD, Remove ACF.");
				RemoveACFArray();
			}

			IndexNextACFOperation();
		}
		else
		{
			//SetError(IDS_ACF1_INVALID_ACF_INDEXED_LENGTH_ERR + (m_lCurSlave * CONVERT_TO_MAPPING));
			SetError(IDS_ACF1_INVALID_ACF_INDEXED_LENGTH_ERR);
			HMIMessageBox(MSG_OK, "INDEX NEXT ACF", "Operation Abort!");
		}


		BOOL bOpDone = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	}
	else
	{
		IndexNextACFOperation_Single_ACF();

		BOOL bOpDone = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	}

	return 1;
}

LONG CACF::HMI_SingleCycleOperation(IPC_CServiceMessage &svMsg)
{
	SingleCycleOperation();

	BOOL bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);

	return 1;
}

LONG CACF::HMI_ForceCalibration(IPC_CServiceMessage &svMsg)
{
	BOOL bOpDone = TRUE;
	INT i = 0;

	if (ForceCalibration())
	{
		HMIMessageBox(MSG_OK, "ACF FORCE CALIBRATION", "Calibration Success!");
	}
	else
	{
		HMIMessageBox(MSG_OK, "ACF FORCE CALIBRATION", "Calibration Abort!");
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
		return 1;
	}

	PrintCalibDataToHMI();

	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}


LONG CACF::HMI_ForceCalibPressTest(IPC_CServiceMessage &svMsg)
{
	BOOL bOpDone = TRUE;
	CString szMsg;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "ACF FORCE SINGLE PRESS", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
		return 1;
	}

	if (!m_pCACFWH->m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "ACF FORCE SINGLE PRESS", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
		return 1;
	}


	if (rMSG_CONTINUE != HMIMessageBox(MSG_CONTINUE_CANCEL, "ACF FORCE CALIBRATION", "Please place force calibrator onto ACF base heater then press Continue."))
	{
		HMIMessageBox(MSG_OK, "ACF FORCE SINGLE PRESS", "Operation Abort!");
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
		return 1;
	}

	m_bPressCycle = TRUE;

	ForceCalibPressOperation();

//	SetHeadDownSol(OFF, GMP_WAIT);
//	SetCounterBalanceSol(ON, GMP_WAIT);

	m_bPressCycle = FALSE;

	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}

LONG CACF::HMI_SinglePressTest(IPC_CServiceMessage &svMsg)
{
	BOOL bOpDone = TRUE;
	CString szMsg;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "ACF SINGLE PRESS TEST", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
		return 1;
	}

	m_bPressCycle = TRUE;

	SinglePressOperation();

//	SetHeadDownSol(OFF, GMP_WAIT);
//	SetCounterBalanceSol(ON, GMP_WAIT);

	m_bPressCycle = FALSE;

	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}

LONG CACF::HMI_StartPressTest(IPC_CServiceMessage &svMsg)
{
	BOOL bOpDone = TRUE;
	CString szMsg;

	if (!m_bModSelected)
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "ACF PRESS TEST", szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
		return 1;
	}

	m_bPressCycle = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bOpDone);

	return 1;
}


LONG CACF::IPC_SaveMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString strTitle;

	//Posn
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lStandbyPosn"] = m_lStandbyPosn;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lOutPosn"] = m_lOutPosn;

	// saving force calibration
	//Force Data
	for (INT j = 0; j < MAX_BOND_FORCE_MODE; j++)
	{
		for (INT i = 0; i < MAX_FORCE_DATA; i++)
		{
			strTitle.Format("m_stForceCalib[m_lCurrBondForceMode][%ld]", i);
			pAppMod->m_smfMachine[GetStnName()]["ForceCalibration"][strTitle]["Force_kgf"]		= m_stForceCalib[m_lCurrBondForceMode][i].Force_kgf;
			pAppMod->m_smfMachine[GetStnName()]["ForceCalibration"][strTitle]["Pressure_bar"]	= m_stForceCalib[m_lCurrBondForceMode][i].Pressure_bar;
			pAppMod->m_smfMachine[GetStnName()]["ForceCalibration"][strTitle]["OpenDacValue"]	= m_stForceCalib[m_lCurrBondForceMode][i].OpenDacValue;
		}
	}

	pAppMod->m_smfMachine[GetStnName()]["Calib"]["m_dCutterToHeadDistance"]			= m_dCutterToHeadDistance;
	pAppMod->m_smfMachine[GetStnName()]["Calib"]["m_dCutterToHeadOffset"]			= m_dCutterToHeadOffset;
	pAppMod->m_smfMachine[GetStnName()]["Calib"]["m_dIndexerCustomFactor"]			= m_dIndexerCustomFactor; //20120706
	
	//Force Calib Para
	pAppMod->m_smfMachine[GetStnName()]["Delays"]["m_lPressTestDelay"]			= m_lPressTestDelay;
	pAppMod->m_smfMachine[GetStnName()]["Delays"]["m_lCalibPreAttachDelay"]		= m_lCalibPreAttachDelay;
	pAppMod->m_smfMachine[GetStnName()]["Delays"]["m_lCalibAttachDelay"]		= m_lCalibAttachDelay;
	pAppMod->m_smfMachine[GetStnName()]["Para"]["m_lForceCalibPressCycle"]		= m_lForceCalibPressCycle;
	pAppMod->m_smfMachine[GetStnName()]["Para"]["m_dCalibFinalPressureCtrl"]	= m_dCalibFinalPressureCtrl;
	pAppMod->m_smfMachine[GetStnName()]["Para"]["m_dCalibUpPressureCtrl"]		= m_dCalibUpPressureCtrl;
	pAppMod->m_smfMachine[GetStnName()]["Para"]["m_dCalibPressureCtrl"]			= m_dCalibPressureCtrl;
	pAppMod->m_smfMachine[GetStnName()]["Para"]["m_lCalibFinalDACCtrl"]			= m_lCalibFinalDACCtrl;
	
	pAppMod->m_smfMachine[GetStnName()]["Para"]["m_dStripperXGoMoveLeftSpeedFactor"]			= m_dStripperXGoMoveLeftSpeedFactor;
	pAppMod->m_smfMachine[GetStnName()]["Para"]["m_dStripperXGoMoveRightSpeedFactor"]			= m_dStripperXGoMoveRightSpeedFactor;

	pAppMod->m_smfMachine[GetStnName()]["Para"]["m_lNumOfPressBeforeAlert"]			= m_lNumOfPressBeforeAlert; //20150416
	pAppMod->m_smfMachine[GetStnName()]["Para"]["m_lCurNumOfPressCount"]			= m_lCurNumOfPressCount;

	pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lStandbyPosnACF"]			= m_lStandbyPosnACF; //20150416

	pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lStandbyLevel"]			= m_lStandbyLevel; //20150416
	pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lCalibLevel"]			= m_lCalibLevel;

	pAppMod->m_smfMachine[GetStnName()]["Para"]["m_dForceSensorFactor"]			= m_dForceSensorFactor;
	pAppMod->m_smfMachine[GetStnName()]["ForceCalibration"]["m_lCurrBondForceMode"] = m_lCurrBondForceMode;
	return CAC9000Stn::IPC_SaveMachineParam();
}

LONG CACF::IPC_LoadMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString strTitle;
	//Posn
	m_lStandbyPosn	= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lStandbyPosn"];
	m_lOutPosn		= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lOutPosn"];

	for (INT j = 0; j < MAX_BOND_FORCE_MODE; j++)
	{
		for (INT i = 0; i < MAX_FORCE_DATA; i++)
		{
			strTitle.Format("m_stForceCalib[m_lCurrBondForceMode][%ld]", i);
			m_stForceCalib[m_lCurrBondForceMode][i].Force_kgf		= pAppMod->m_smfMachine[GetStnName()]["ForceCalibration"][strTitle]["Force_kgf"];
			m_stForceCalib[m_lCurrBondForceMode][i].Pressure_bar	= pAppMod->m_smfMachine[GetStnName()]["ForceCalibration"][strTitle]["Pressure_bar"];
			m_stForceCalib[m_lCurrBondForceMode][i].OpenDacValue	= pAppMod->m_smfMachine[GetStnName()]["ForceCalibration"][strTitle]["OpenDacValue"];
		}
	}

	m_dCutterToHeadDistance		= pAppMod->m_smfMachine[GetStnName()]["Calib"]["m_dCutterToHeadDistance"];
	m_dCutterToHeadOffset		= pAppMod->m_smfMachine[GetStnName()]["Calib"]["m_dCutterToHeadOffset"];
	m_dIndexerCustomFactor		= pAppMod->m_smfMachine[GetStnName()]["Calib"]["m_dIndexerCustomFactor"]; //20120706
	if (m_dIndexerCustomFactor <= 0.0)
	{
		m_dIndexerCustomFactor = 1.0;
	}
	//Force Calib Para
	m_lPressTestDelay			= pAppMod->m_smfMachine[GetStnName()]["Delays"]["m_lPressTestDelay"];
	m_lCalibPreAttachDelay		= pAppMod->m_smfMachine[GetStnName()]["Delays"]["m_lCalibPreAttachDelay"];
	m_lCalibAttachDelay			= pAppMod->m_smfMachine[GetStnName()]["Delays"]["m_lCalibAttachDelay"];
	m_lForceCalibPressCycle		= pAppMod->m_smfMachine[GetStnName()]["Para"]["m_lForceCalibPressCycle"];
	m_dCalibFinalPressureCtrl	= pAppMod->m_smfMachine[GetStnName()]["Para"]["m_dCalibFinalPressureCtrl"];
	m_dCalibUpPressureCtrl		= pAppMod->m_smfMachine[GetStnName()]["Para"]["m_dCalibUpPressureCtrl"];
	m_dCalibPressureCtrl		= pAppMod->m_smfMachine[GetStnName()]["Para"]["m_dCalibPressureCtrl"];
	m_lCalibFinalDACCtrl		= pAppMod->m_smfMachine[GetStnName()]["Para"]["m_lCalibFinalDACCtrl"];
	m_lCurrBondForceMode = pAppMod->m_smfMachine[GetStnName()]["ForceCalibration"]["m_lCurrBondForceMode"];

	m_lNumOfPressBeforeAlert	= pAppMod->m_smfMachine[GetStnName()]["Para"]["m_lNumOfPressBeforeAlert"]; //20150416
	m_lCurNumOfPressCount		= pAppMod->m_smfMachine[GetStnName()]["Para"]["m_lCurNumOfPressCount"];

	m_dForceSensorFactor		= pAppMod->m_smfMachine[GetStnName()]["Para"]["m_dForceSensorFactor"];

	m_lStandbyLevel	= pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lStandbyLevel"]; //20150416
	m_lCalibLevel		= pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lCalibLevel"];

	m_lStandbyPosnACF	= pAppMod->m_smfMachine[GetStnName()]["Level"]["m_lStandbyPosnACF"]; //20150416

	m_dStripperXGoMoveLeftSpeedFactor = pAppMod->m_smfMachine[GetStnName()]["Para"]["m_dStripperXGoMoveLeftSpeedFactor"];
	if (m_dStripperXGoMoveLeftSpeedFactor < 0.01 || m_dStripperXGoMoveLeftSpeedFactor > 3.0)
	{
		m_dStripperXGoMoveLeftSpeedFactor = 1.0;
	}

	m_dStripperXGoMoveRightSpeedFactor = pAppMod->m_smfMachine[GetStnName()]["Para"]["m_dStripperXGoMoveRightSpeedFactor"];
	if (m_dStripperXGoMoveRightSpeedFactor < 0.01 || m_dStripperXGoMoveRightSpeedFactor > 3.0)
	{
		m_dStripperXGoMoveRightSpeedFactor = 1.0;
	}

	DacToForce(m_lCalibFinalDACCtrl, m_stForceCalib[m_lCurrBondForceMode], &m_dCalibForceCtrl);
	HMI_dCalibForceCtrl			= m_dCalibForceCtrl * (g_bShowForceInNewton * KGF_TO_NEWTON + (1 - (LONG)g_bShowForceInNewton));
	UpdateCurPressureValue();

	PrintCalibDataToHMI();

	return CAC9000Stn::IPC_LoadMachineParam();
}

LONG CACF::IPC_SaveDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	//if (m_lCurSlave == ACF_NONE)
	//{
	//	return 1;
	//}
	// Posn
	pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lOutOffset"] = m_lOutOffset;
	pAppMod->m_smfDevice[GetStnName()]["Level"]["m_lBondLevel"]			= m_lBondLevel;
	
	pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dPressureCtrl"]				= m_dPressureCtrl;
	//pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dFinalPressureCtrl[ACF1]"]		= m_dFinalPressureCtrl[ACF1];
	//pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dFinalPressureCtrl[ACF2]"]		= m_dFinalPressureCtrl[ACF2];
	//pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dFinalPressureCtrl[ACF3]"]		= m_dFinalPressureCtrl[ACF3];
	//pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dFinalPressureCtrl[ACF4]"]		= m_dFinalPressureCtrl[ACF4];
	//pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dFinalPressureCtrl[ACF5]"]		= m_dFinalPressureCtrl[ACF5];
	//pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dFinalPressureCtrl[ACF6]"]		= m_dFinalPressureCtrl[ACF6];
	//pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dFinalPressureCtrl[ACF7]"]		= m_dFinalPressureCtrl[ACF7];
	//pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dFinalPressureCtrl[ACF8]"]		= m_dFinalPressureCtrl[ACF8];
	pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dUpPressureCtrl"]			= m_dUpPressureCtrl;

	// saving delays
	pAppMod->m_smfDevice[GetStnName()]["Delays"]["m_lPreAttachDelay"]				= m_lPreAttachDelay;
	pAppMod->m_smfDevice[GetStnName()]["Delays"]["m_lAttachACFDelay"]				= m_lAttachACFDelay;

	return CAC9000Stn::IPC_SaveDeviceParam();
}

LONG CACF::IPC_LoadDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	
	//if (m_lCurSlave == ACF_NONE)
	//{
	//	return 1;
	//}

	// Posn
	m_lOutOffset = pAppMod->m_smfDevice[GetStnName()]["Posn"]["m_lOutOffset"];
	m_lBondLevel		= pAppMod->m_smfDevice[GetStnName()]["Level"]["m_lBondLevel"];

	m_dPressureCtrl				= pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dPressureCtrl"];
	m_dFinalPressureCtrl[ACF1]	= pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dFinalPressureCtrl[ACF1]"];
	//m_dFinalPressureCtrl[ACF2]	= pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dFinalPressureCtrl[ACF2]"];
	//m_dFinalPressureCtrl[ACF3]	= pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dFinalPressureCtrl[ACF3]"];
	//m_dFinalPressureCtrl[ACF4]	= pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dFinalPressureCtrl[ACF4]"];
	//m_dFinalPressureCtrl[ACF5]	= pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dFinalPressureCtrl[ACF5]"];
	//m_dFinalPressureCtrl[ACF6] = pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dFinalPressureCtrl[ACF6]"];
	//m_dFinalPressureCtrl[ACF7]	= pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dFinalPressureCtrl[ACF7]"];
	//m_dFinalPressureCtrl[ACF8]	= pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dFinalPressureCtrl[ACF8]"];
	m_dUpPressureCtrl			= pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_dUpPressureCtrl"];
	
	// saving delays
	m_lPreAttachDelay			= pAppMod->m_smfDevice[GetStnName()]["Delays"]["m_lPreAttachDelay"];
	m_lAttachACFDelay			= pAppMod->m_smfDevice[GetStnName()]["Delays"]["m_lAttachACFDelay"];

	m_lFinalDACCtrl[ACF1]	= pAppMod->m_smfDevice[GetStnName()]["EPRegulator"]["m_lFinalDACCtrl[ACF1]"];
	
	if (!DacToForce(m_lFinalDACCtrl[ACF1], m_stForceCalib[m_lCurrBondForceMode], &HMI_dFinalForceCtrl))
	{
		m_bDACValid = FALSE;
	}
	else
	{
		m_bDACValid = TRUE;
	}

	HMI_dFinalForceCtrl = HMI_dFinalForceCtrl * (g_bShowForceInNewton * KGF_TO_NEWTON + (1 - (LONG)g_bShowForceInNewton));
	HMI_lFinalDACCtrl = m_lFinalDACCtrl[ACF1]; //20120703
//	UpdateCurPressureValue(); //20120703

	return CAC9000Stn::IPC_LoadDeviceParam();
}

LONG CACF::HMI_SetBondForceMode(IPC_CServiceMessage &svMsg)
{
	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");

	LONG lValue = 0;
	svMsg.GetMsg(sizeof(LONG), &lValue);

	if (lValue < LOW_BOND_FORCE_MODE || lValue >= MAX_BOND_FORCE_MODE)
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		return 0;
	}
	m_lCurrBondForceMode = lValue;

	pCACF1->PrintCalibDataToHMI();
	pCACF1->UpdateCurPressureValue();
	return 0;
}

BOOL CACF::GetADCValue(LONG &lForce)
{
	LONG lResult = 0;

	lResult = m_stADCPort.GetDACValue();
	if (lResult != 0)
	{
		lForce = lResult;
		return GMP_SUCCESS;
	}
	else
	{
		return GMP_FAIL;
	}
}

DOUBLE CACF::RawADCToForce(LONG lRawData, DOUBLE dCperLbf)
{
	//Assume that the range sol has not change since the ADC is read.
	//We found that the largest possible value return NU-Motion is 32*1024. So the range return by ADC is +- 32k. But the resolution of ADC is 12bit.
	//DOUBLE dRangeFactor = 1.0;
	INT nRange = 2;

	if (dCperLbf == 0.0)
	{
		return 0.0;
	}

	if (m_stACFForceAmpRangellSol.bHMI_Status)
	{
		//range II
		nRange = 2;
		//dRangeFactor = 1.0;
	}
	else
	{
		//Range I
		nRange = 1;
		//dRangeFactor = 4.0;
	}
	
	//return ((DOUBLE)lRawData / 2048.0 * FORCE_AMP_RANGE_II / dCperLbf / 250.0 * 15.0 * dRangeFactor); //return force unit in lbf
	return ((DOUBLE)lRawData / (32.0 * 1024.0) * (nRange == 1 ? FORCE_AMP_RANGE_I : FORCE_AMP_RANGE_II ) / dCperLbf); //return force unit in lbf
}


LONG CACF::HMI_MBReadADCForce(IPC_CServiceMessage &svMsg)
{
	LONG lForce = 0;

	//SetMBForceChargeAmpRangeSol(ON, GMP_WAIT);
	//SetMBForceChargeAmpOperateSol(ON, GMP_WAIT);

	if (GetADCValue(lForce) == GMP_SUCCESS)
	{
		m_lForceADCValue = lForce;
	}

	//SetMBForceChargeAmpOperateSol(OFF, GMP_WAIT);
	return 0;
}

INT CACF::LogForceEndOfBond()
{
	CTempDisplay *pCTempDisplay = (CTempDisplay*)m_pModule->GetStation("TempDisplay");
	CString szFile("");
	CString szTime("");
	FILE *fp = NULL;
	SYSTEMTIME ltime;		// New Command
	LONG lForce = 0;

	m_csForceSensor.Lock();

	if (GetADCValue(lForce) == GMP_SUCCESS)
	{
		m_lForceADCEndOfBonding = lForce;
		m_dForceKgEndOfBonding = RawADCToForce(m_lForceADCEndOfBonding, m_dForceSensorFactor) / 2.2;
	}

	m_csForceSensor.Unlock();

	GetLocalTime(&ltime);
	szFile.Format("%s%s_%04u%02u%02u.csv", 
		FORCE_SENSOR_ENDOFBOND_LOG_PATH, 
		FORCE_SENSOR_ENDOFBOND_LOG_FILENAME, 
		ltime.wYear, ltime.wMonth, ltime.wDay);

	fp = fopen(szFile.GetBuffer(0), "a+");
	if (fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		if (ftell(fp) < 10)
		{
			fprintf(fp, "Date, ADC_RAW, Force_kgf, Temperature(degree), WH1 Temperature(degree), WH2 Temperature(degree)\n");
		}

		//GetLocalTime(&ltime);
		szTime.Format("%u-%u-%u %u:%u:%u", 
			ltime.wYear, ltime.wMonth, ltime.wDay, 
			ltime.wHour, ltime.wMinute, ltime.wSecond);

		fprintf(fp, "%s, %ld, %.2f, %.2f, %.2f, %.2f\n", 
			(const char*)szTime, 
			m_lForceADCEndOfBonding, m_dForceKgEndOfBonding, pCTempDisplay->m_dHeaterTemp[0], pCTempDisplay->m_dHeaterTemp[2], pCTempDisplay->m_dHeaterTemp[3]);
		fclose(fp);
	}

	return GMP_SUCCESS;
}

INT CACF::StartLogForceThread()
{
	DWORD dwThreadId = 0;
	//HANDLE hThread = NULL;

	m_hThreadForceADC = ::CreateThread( 
		NULL,						// no security attributes 
		0,							// use default stack size  
		LogADCForce,				// thread function 
		this/*&lWhichMBHead*/,				// argument to thread function 
		0,							// use default creation flags 
		&dwThreadId);				// returns the thread identifier 
	// Check the return value for success. 

	if (m_hThreadForceADC == NULL)
	{
#if 1
		LPVOID lpMsgBuf;
		CString szTmp;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);
		szTmp.Format("StartLogForceThread: CreateThread fail GetLastError:%s", lpMsgBuf);
		DisplayMessage(szTmp);
		LocalFree( lpMsgBuf );
#else
		DisplayMessage("StartLogForceThread: CreateThread failed.");
#endif
		return GMP_FAIL;
	}
	//else 
	//{
	//	CloseHandle(m_hThreadForceADC);
	//}
	return GMP_SUCCESS;
}

//INT CACF::StartDummpThread() //20171116
//{
//	DWORD dwThreadId = 0;
//	HANDLE hThread = NULL;
//
//	hThread = ::CreateThread( 
//		NULL,						// no security attributes 
//		0,							// use default stack size  
//		DummyThread,				// thread function 
//		this/*&lWhichMBHead*/,				// argument to thread function 
//		0,							// use default creation flags 
//		&dwThreadId);				// returns the thread identifier 
//	// Check the return value for success. 
//
//	if (hThread == NULL)
//	{
//#if 1
//		LPVOID lpMsgBuf;
//		CString szTmp;
//		FormatMessage( 
//			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
//			FORMAT_MESSAGE_FROM_SYSTEM | 
//			FORMAT_MESSAGE_IGNORE_INSERTS,
//			NULL,
//			GetLastError(),
//			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
//			(LPTSTR) &lpMsgBuf,
//			0,
//			NULL 
//		);
//		szTmp.Format("StartLogForceThread: CreateThread fail GetLastError:%s", lpMsgBuf);
//		DisplayMessage(szTmp);
//		LocalFree( lpMsgBuf );
//#else
//		DisplayMessage("StartDummpThread: CreateThread failed.");
//#endif
//		return GMP_FAIL;
//	}
//	else 
//	{
//		DisplayMessage("StartDummpThread: Done.");
//		CloseHandle(hThread);
//	}
//	return GMP_SUCCESS;
//}

DWORD WINAPI LogADCForce(LPVOID lpParam)
{
	CACF *pCACFx = NULL;
	CString szMsg(" "), szFileName;
	BOOL *pbRunThread = NULL;
	//BOOL bContact = FALSE; //20161116
	//const DOUBLE dContactForce = 0.95;
	FILE *fp = NULL;
	pCACFx = (CACF *)lpParam;
	
	szFileName.Format(FORCE_SENSOR_LOG_FILENAME);
	
	szMsg = CString(FORCE_SENSOR_LOG_PATH) + szFileName;

	fp = fopen(szMsg.GetBuffer(0), "a+");
	if (fp != NULL)
	{
		pbRunThread = &m_bForceLogRun;

		fseek(fp, 0, SEEK_END);
		if (ftell(fp) < 10)
		{
			fprintf(fp, "Date, Label, ADC_RAW, Force_kgf\n");
		}

		while (*pbRunThread)
		{
			//__time64_t ltime;
			CString szTime = "";

			//_time64(&ltime);
			//szTime = _ctime64(&ltime);
			//szTime.TrimRight('\n');
			SYSTEMTIME ltime;		// New Command

			// Get time as 64-bit integer.
			GetLocalTime(&ltime);
			szTime.Format("%u-%u-%u %u:%u:%u", 
				ltime.wYear, ltime.wMonth, ltime.wDay, 
				ltime.wHour, ltime.wMinute, ltime.wSecond);
			LONG lForce = 0;
			LONG lMotorZEnc = 0;

			m_csForceSensor.Lock();

			if (pCACFx->GetADCValue(lForce) == GMP_SUCCESS)
			{
				pCACFx->m_lForceADCValue = lForce;
				pCACFx->m_dForceBondWeight = pCACFx->RawADCToForce(pCACFx->m_lForceADCValue, pCACFx->m_dForceSensorFactor) / 2.2;
			}

			m_csForceSensor.Unlock();

			fprintf(fp, "%s, %s, %ld, %.2f\n", 
				(const char*)szTime, pCACFx->m_szLogLabel, pCACFx->m_lForceADCValue, pCACFx->m_dForceBondWeight);

			if (*pbRunThread)
			{
				Sleep(10);
			}
		}
		fprintf(fp, "--end--\n");
		fclose(fp);
	}

	return 0;
}

//DWORD WINAPI DummyThread(LPVOID lpParam) //20171116
//{
//	CString szMsg(" "), szFileName;
//	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");
//
//	pCACF1->m_lForceADCValue = 1;
//
//	//while (pCMBx->m_bGlass1Exist)
//	{
//		//__time64_t ltime;
//		CString szTime = "";
//
//		//_time64(&ltime);
//		//szTime = _ctime64(&ltime);
//		//szTime.TrimRight('\n');
//		SYSTEMTIME ltime;		// New Command
//
//		// Get time as 64-bit integer.
//		GetLocalTime(&ltime);
//		szTime.Format("%u-%u-%u %u:%u:%u", 
//			ltime.wYear, ltime.wMonth, ltime.wDay, 
//			ltime.wHour, ltime.wMinute, ltime.wSecond);
//
//		Sleep(300);
//	}
//	pCACF1->m_lForceADCValue = 0;
//	
//	return 0;
//}