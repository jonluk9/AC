/////////////////////////////////////////////////////////////////
//	ACF1.cpp : interface of the CACF1 class
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
#include "ACF1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
/// <summary>
/// 
/// </summary>
/// <returns>
/// 
/// </returns>
IMPLEMENT_DYNCREATE(CACF1, CACF)

CACF1::CACF1()
{
	// CAC9000Srn
	m_lStnErrorCode		= IDS_ACF1_NOT_SELECTED_ERR;
	m_lModSelectMode	= ACF_MOD;

	m_lCurSlave	= ACF1;

	// Motor Related
	m_stMotorIndex.InitMotorInfo("ACF1_INDEXOR", &m_bModSelected, STEPPER_MOTOR);
	m_stMotorRoller.InitMotorInfo("ACF1_ROLLER", &m_bModSelected, STEPPER_MOTOR);
	m_stMotorZ.InitMotorInfo("ACF1_Z", &m_bModSelected);
	m_stMotorStripperX.InitMotorInfo("ACF1_STRIPPER_X", &m_bModSelected);
	m_stMotorX.InitMotorInfo("ACF1_X", &m_bModSelected);
	m_stMotorBF.InitMotorInfo("ACF1_FORCE_Z", &m_bModSelected);

	m_myMotorList.Add((CMotorInfo*)&m_stMotorIndex);
	m_myMotorList.Add((CMotorInfo*)&m_stMotorRoller);
	m_myMotorList.Add((CMotorInfo*)&m_stMotorZ);
	m_myMotorList.Add((CMotorInfo*)&m_stMotorStripperX);	
	m_myMotorList.Add((CMotorInfo*)&m_stMotorX);	
	m_myMotorList.Add((CMotorInfo*)&m_stMotorBF);


	// Si Related
	m_stACFFeederBufferSnr		= CSiInfo("ACFFeederBufferSnr",			"IOMODULE_IN_PORT_6", BIT_0, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stACFFeederBufferSnr);
	m_stACFStripperHomeSnr		= CSiInfo("ACFStripperHomeSnr",			"IOMODULE_IN_PORT_6", BIT_1, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stACFStripperHomeSnr);	
	m_stACFCutterDnPosnSnr		= CSiInfo("ACFCutterDnPosnSnr",			"IOMODULE_IN_PORT_6", BIT_2, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stACFCutterDnPosnSnr);	
	m_stACFCutterPressureSnr	= CSiInfo("ACFCutterPressureSnr",		"IOMODULE_IN_PORT_6", BIT_3, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stACFCutterPressureSnr);
	m_stACFTapeEndSnr			= CSiInfo("ACFTapeEndSnr",				"IOMODULE_IN_PORT_6", BIT_4, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stACFTapeEndSnr);	
	m_stACFIonizerCheckSnr		= CSiInfo("ACFIonizerCheckSnr",			"IOMODULE_IN_PORT_6", BIT_5, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stACFIonizerCheckSnr);	
	m_stACFIonizerErrorSnr		= CSiInfo("ACFIonizerErrorSnr",			"IOMODULE_IN_PORT_6", BIT_6, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stACFIonizerErrorSnr);	
	m_stACFTapeDetectSnr		= CSiInfo("ACFTapeDetectSnr",			"IOMODULE_IN_PORT_6", BIT_7, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stACFTapeDetectSnr);	
	m_stACFZHomeSnr				= CSiInfo("ACFZHomeSnr",				"IOMODULE_IN_PORT_6", BIT_8, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stACFZHomeSnr);	
	m_stACFXHomeSnr				= CSiInfo("ACFXHomeSnr",				"IOMODULE_IN_PORT_6", BIT_9, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stACFXHomeSnr);	
	






	//m_stHeadUpSnr				= CSiInfo("ACF1HeadUpSnr",			"IOMODULE_IN_PORT_6", BIT_0, ACTIVE_HIGH);
	//m_mySiList.Add((CSiInfo*)&m_stHeadUpSnr);	
	//m_stFeederBufferSnr			= CSiInfo("ACF1FeederBufferSnr",	"IOMODULE_IN_PORT_6", BIT_1, ACTIVE_HIGH);
	//m_mySiList.Add((CSiInfo*)&m_stFeederBufferSnr);	
	//m_stStripperHomeSnr			= CSiInfo("ACF1StripperHomeSnr",		"IOMODULE_IN_PORT_6", BIT_2, ACTIVE_LOW);
	//m_mySiList.Add((CSiInfo*)&m_stStripperHomeSnr);	// 20150204
	////m_stStripperOnSnr			= CSiInfo("ACF1StripperOnSnr",		"IOMODULE_IN_PORT_6", BIT_2, ACTIVE_LOW);
	////m_mySiList.Add((CSiInfo*)&m_stStripperOnSnr);	
	////m_stStripperOffSnr			= CSiInfo("ACF1StripperOffSnr",		"IOMODULE_IN_PORT_6", BIT_3, ACTIVE_LOW);
	////m_mySiList.Add((CSiInfo*)&m_stStripperOffSnr);	
	//m_stCutterDownPosnSnr		= CSiInfo("ACF1CutterDownPosnSnr",	"IOMODULE_IN_PORT_6", BIT_4, ACTIVE_HIGH);
	//m_mySiList.Add((CSiInfo*)&m_stCutterDownPosnSnr);	
	//m_stPressureSnr				= CSiInfo("ACF1PressureSnr",		"IOMODULE_IN_PORT_6", BIT_5, ACTIVE_HIGH);
	//m_mySiList.Add((CSiInfo*)&m_stPressureSnr);	
	//m_stCutterPressureSnr		= CSiInfo("ACF1CutterPressureSnr",	"IOMODULE_IN_PORT_6", BIT_6, ACTIVE_HIGH);
	//m_mySiList.Add((CSiInfo*)&m_stPressureSnr);	//???
	//m_stTapeEndSnr				= CSiInfo("ACF1TapeEndSnr",			"IOMODULE_IN_PORT_6", BIT_7, ACTIVE_HIGH); //20121122
	//m_mySiList.Add((CSiInfo*)&m_stTapeEndSnr);	
	//m_stIonizerCheckSnr				= CSiInfo("ACF1IonizerCheckOutputSnr",			"IOMODULE_IN_PORT_6", BIT_8, ACTIVE_HIGH); //20121122
	//m_mySiList.Add((CSiInfo*)&m_stIonizerCheckSnr);
	//m_stIonizerErrorSnr				= CSiInfo("ACF1IonizerErrorOutputSnr",			"IOMODULE_IN_PORT_6", BIT_9, ACTIVE_HIGH); //20121122
	//m_mySiList.Add((CSiInfo*)&m_stIonizerErrorSnr);



	// So Related
	m_stACFForceAmpOpSol		= CSoInfo("ACFForceAmpOpSol",		"IOMODULE_OUT_PORT_3", BIT_12, ACF1_SOL, ACTIVE_HIGH, TRUE);
	m_mySoList.Add((CSoInfo*)&m_stACFForceAmpOpSol);
	m_stACFForceAmpRangellSol	= CSoInfo("ACFForceAmpRangellSol",	"IOMODULE_OUT_PORT_3", BIT_13, ACF1_SOL, ACTIVE_HIGH, TRUE);
	m_mySoList.Add((CSoInfo*)&m_stACFForceAmpRangellSol);
	m_stACFCutterOnSol			= CSoInfo("ACFCutterOnSol",			"IOMODULE_OUT_PORT_3", BIT_14, ACF1_SOL, ACTIVE_HIGH, TRUE);
	m_mySoList.Add((CSoInfo*)&m_stACFCutterOnSol);
	m_stACFIonizerPowerEnSol	= CSoInfo("ACFIonizerPowerEnSol",	"IOMODULE_OUT_PORT_3", BIT_15, ACF1_SOL, ACTIVE_HIGH );
	m_mySoList.Add((CSoInfo*)&m_stACFIonizerPowerEnSol);


	//m_stHeadDownSol				= CSoInfo("ACF1HeadDownSol",		"IOMODULE_OUT_PORT_7", BIT_0, ACF1_SOL, ACTIVE_HIGH, TRUE);
	//m_mySoList.Add((CSoInfo*)&m_stHeadDownSol);
	//m_stCounterBalanceSol		= CSoInfo("ACF1CounterBalanceSol",	"IOMODULE_OUT_PORT_7", BIT_1, ACF1_SOL, ACTIVE_LOW, TRUE);
	//m_mySoList.Add((CSoInfo*)&m_stCounterBalanceSol);
	//m_stCutterOnSol				= CSoInfo("ACF1CutterOnSol",		"IOMODULE_OUT_PORT_7", BIT_2, ACF1_SOL, ACTIVE_HIGH, TRUE);
	//m_mySoList.Add((CSoInfo*)&m_stCutterOnSol);
	//m_stIonizerPwrEnableSol		= CSoInfo("ACF1IonizerPwrEnableSol", "IOMODULE_OUT_PORT_7", BIT_3, ACF1_SOL, ACTIVE_HIGH);
	//m_mySoList.Add((CSoInfo*)&m_stIonizerPwrEnableSol);
	////m_stStripperSol				= CSoInfo("ACF1StripperSol",		"IOMODULE_OUT_PORT_7", BIT_4, ACF1_SOL, ACTIVE_HIGH, TRUE);
	////m_mySoList.Add((CSoInfo*)&m_stStripperSol);	// 20150204
	//m_stHeadUpSol				= CSoInfo("ACF1HeadUpSol",			"IOMODULE_OUT_PORT_7", BIT_5, ACF1_SOL, ACTIVE_LOW, TRUE);
	//m_mySoList.Add((CSoInfo*)&m_stHeadUpSol);

	//m_stDACPort					= CPortInfo("ACF1DACPort",			"IOMODULE_HWDAC_DAC16A", BIT_0);

	m_stADCPort					= CPortInfo("ACF1ADCPort",			"IOMODULE_ADC_PORT_0", BIT_0);

	// Srch Prof
	m_stMotorRoller.stUsrDefProfile[ACF_ROLLER_SRCH_FEEDER_BUFFER_SNR_ON_LEVEL].bEnable						= GMP_ENABLE;
	m_stMotorRoller.stUsrDefProfile[ACF_ROLLER_SRCH_FEEDER_BUFFER_SNR_ON_LEVEL].sSrchProfID					= USR_DEF_SRCH_PROF1;
	m_stMotorRoller.stUsrDefProfile[ACF_ROLLER_SRCH_FEEDER_BUFFER_SNR_ON_LEVEL].sSrchDirection				= NEGATIVE_DIR;
	strcpy((char*)m_stMotorRoller.stUsrDefProfile[ACF_ROLLER_SRCH_FEEDER_BUFFER_SNR_ON_LEVEL].cPort, m_stACFFeederBufferSnr.GetPortName());
	m_stMotorRoller.stUsrDefProfile[ACF_ROLLER_SRCH_FEEDER_BUFFER_SNR_ON_LEVEL].unSensorMask				= m_stACFFeederBufferSnr.GetMask();
	m_stMotorRoller.stUsrDefProfile[ACF_ROLLER_SRCH_FEEDER_BUFFER_SNR_ON_LEVEL].bAstate						= m_stACFFeederBufferSnr.GetActiveState();

	m_stSrchFreederBuffer.dSrchVel		= 0.5;
	m_stSrchFreederBuffer.dSrchLimit	= 1000;
	m_stSrchFreederBuffer.dDriveInVel	= 5.0;
	m_stSrchFreederBuffer.dDriveInVel	= 500;
	m_stSrchFreederBuffer.sDebounce		= 2;

}

CACF1::~CACF1()
{
}

BOOL CACF1::InitInstance()
{
	CACF::InitInstance();

	return TRUE;
}

INT CACF1::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CACF::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CACF1::Operation()
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
VOID CACF1::RegisterVariables()
{
	try
	{
		// -------------------------------------------------------
		// AC9000Stn Variable and Function
		// -------------------------------------------------------
		// -------------------------------------------------------
		RegVariable(GetStation() + _T("bCheckMotor"), &m_bCheckMotor);
		RegVariable(GetStation() + _T("bCheckIO"), &m_bCheckIO);

		RegVariable(GetStation() + _T("bModSelected"), &m_bModSelected);
		RegVariable(GetStation() + _T("bShowModSelected"), &m_bShowModSelected);
		RegVariable(GetStation() + _T("bModError"), &m_bModError);
		
		RegVariable(GetStation() + _T("bGlass1Exist"), &m_bGlass1Exist);

		RegVariable(GetStation() + _T("bDebugSeq"), &HMI_bDebugSeq);
		RegVariable(GetStation() + _T("bLogMotion"), &HMI_bLogMotion);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetModSelected"), HMI_SetModSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleModSelected"), HMI_ToggleModSelected); //20121112
		
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeAll"), HMI_HomeAll);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDebugSeq"), HMI_SetDebugSeq);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDebugCal"), HMI_SetDebugCal);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetLogMotion"), HMI_SetLogMotion);

		// -------------------------------------------------------
		// varibales
		// -------------------------------------------------------
		// hardware
		RegVariable(_T("HMI_bACF1IndexOn"), &m_stMotorIndex.bHMI_On);
		RegVariable(_T("HMI_bACF1RollerOn"), &m_stMotorRoller.bHMI_On);
		RegVariable(_T("HMI_bACF1XOn"), &m_stMotorX.bHMI_On);	// 20150205
		RegVariable(_T("HMI_bACF1ZOn"), &m_stMotorZ.bHMI_On);	// 20150205
		RegVariable(_T("HMI_bACF1ForceZOn"), &m_stMotorBF.bHMI_On);	// 20150205
		RegVariable(_T("HMI_bACF1StripperXOn"), &m_stMotorStripperX.bHMI_On);	// 20150205
		RegVariable(_T("HMI_lACF1XEncoder"), &m_stMotorX.lHMI_CurrPosn);	// 20150205
		RegVariable(_T("HMI_lACF1ZEncoder"), &m_stMotorZ.lHMI_CurrPosn);	// 20150205
		RegVariable(_T("HMI_lACF1StripperXEncoder"), &m_stMotorStripperX.lHMI_CurrPosn);	// 20150205
		RegVariable(_T("HMI_lACF1ForceZEncoder"), &m_stMotorBF.lHMI_CurrPosn);	// 20150205
		RegVariable(GetStation() + _T("dStripperXDistPerCount"), &m_stMotorStripperX.stAttrib.dDistPerCount);	// 20150205
		RegVariable(GetStation() + _T("dXDistPerCount"), &m_stMotorX.stAttrib.dDistPerCount);	// 20150205
		RegVariable(GetStation() + _T("dZDistPerCount"), &m_stMotorZ.stAttrib.dDistPerCount);	// 20150205
		
		// Motor Snr

		// Solenoid Snr
		//RegVariable(_T("HMI_bACF1HeadUpSnr"), &m_stHeadUpSnr.bHMI_Status);
		RegVariable(_T("HMI_bACF1FeederBufferSnr"), &m_stACFFeederBufferSnr.bHMI_Status);
		RegVariable(_T("HMI_bACF1StripperHomeSnr"), &m_stACFStripperHomeSnr.bHMI_Status);		// 20150204
		RegVariable(_T("HMI_bACF1XHomeSnr"), &m_stACFXHomeSnr.bHMI_Status);		// 20150204
		RegVariable(_T("HMI_bACF1ZHomeSnr"), &m_stACFZHomeSnr.bHMI_Status);		// 20150204
		//RegVariable(_T("HMI_bACF1StripperOnSnr"), &m_stStripperOnSnr.bHMI_Status);
		//RegVariable(_T("HMI_bACF1StripperOffSnr"), &m_stStripperOffSnr.bHMI_Status);
		RegVariable(_T("HMI_bACF1CutterDownPosnSnr"), &m_stACFCutterDnPosnSnr.bHMI_Status);
		//RegVariable(_T("HMI_bACF1PressureSnr"), &m_stPressureSnr.bHMI_Status);
		RegVariable(_T("HMI_bACF1CutterPressureSnr"), &m_stACFCutterPressureSnr.bHMI_Status);
		RegVariable(_T("HMI_bACF1TapeEndSnr"), &m_stACFTapeEndSnr.bHMI_Status); //20121122
		RegVariable(_T("HMI_bACF1TapeDetectSnr"), &m_stACFTapeDetectSnr.bHMI_Status); //20121122
		RegVariable(_T("HMI_bACF1IonizerCheckSnr"), &m_stACFIonizerCheckSnr.bHMI_Status);
		RegVariable(_T("HMI_bACF1IonizerErrorSnr"), &m_stACFIonizerErrorSnr.bHMI_Status);

		// Solenoid
		RegVariable(_T("HMI_bACF1ForceAmpOpSol"), &m_stACFForceAmpOpSol.bHMI_Status);
		RegVariable(_T("HMI_bACF1ForceAmpRangellSol"), &m_stACFForceAmpRangellSol.bHMI_Status);
		RegVariable(_T("HMI_bACF1CutterOnSol"), &m_stACFCutterOnSol.bHMI_Status);
		RegVariable(_T("HMI_bACF1IonizerPowerEnSol"), &m_stACFIonizerPowerEnSol.bHMI_Status);


		RegVariable(GetStation() + _T("dACFProcessTime"), &m_dACFProcessTime);

		//Force
		RegVariable(GetStation() + _T("bDACValid"), &m_bDACValid);
		RegVariable(GetStation() + _T("dPressureCtrl"), &m_dPressureCtrl);
		RegVariable(GetStation() + _T("dForceCtrl"), &HMI_dFinalForceCtrl);
		RegVariable(GetStation() + _T("dFinalPressureCtrl"), &HMI_dFinalPressureCtrl);
		RegVariable(GetStation() + _T("dUpPressureCtrl"), &m_dUpPressureCtrl);
		RegVariable(GetStation() + _T("lFinalDACCtrl"), &HMI_lFinalDACCtrl);
		
		// ACF cutter to head offset
		RegVariable(GetStation() + _T("dCutterToHeadDistance"), &m_dCutterToHeadDistance);
		RegVariable(GetStation() + _T("dCutterToHeadOffset"), &m_dCutterToHeadOffset);

		//Para
		RegVariable(GetStation() + _T("lPreAttachDelay"), &m_lPreAttachDelay);
		RegVariable(GetStation() + _T("lAttachACFDelay"), &m_lAttachACFDelay);
		RegVariable(GetStation() + _T("lPressTestDelay"), &m_lPressTestDelay);

		// ACF Indexing Related
		RegVariable(GetStation() + _T("szACFArray"), &HMI_szACFArray);
		
		// 
		RegVariable(GetStation() + _T("lCalibPreAttachDelay"), &m_lCalibPreAttachDelay);
		RegVariable(GetStation() + _T("lCalibAttachDelay"), &m_lCalibAttachDelay);
		RegVariable(GetStation() + _T("bCalibDACValid"), &m_bCalibDACValid);
		RegVariable(GetStation() + _T("dCalibFinalPressureCtrl"), &m_dCalibFinalPressureCtrl);
		RegVariable(GetStation() + _T("dCalibUpPressureCtrl"), &m_dCalibUpPressureCtrl);
		RegVariable(GetStation() + _T("dCalibPressureCtrl"), &m_dCalibPressureCtrl);
		RegVariable(GetStation() + _T("dCalibForceCtrl"), &HMI_dCalibForceCtrl);
		RegVariable(GetStation() + _T("lForceCalibPressCycle"), &m_lForceCalibPressCycle);
		RegVariable(GetStation() + _T("dIndexerCustomFactor"), &m_dIndexerCustomFactor); //20120706
		RegVariable(GetStation() + _T("lCalibFinalDACCtrl"), &m_lCalibFinalDACCtrl);

		RegVariable(GetStation() + _T("lCurNumOfPressCount"), &m_lCurNumOfPressCount); //20150416
		RegVariable(GetStation() + _T("lNumOfPressBeforeAlert"), &m_lNumOfPressBeforeAlert);

		RegVariable(_T("HMI_lACF1ADCForce"), &m_lForceADCValue);

		RegVariable(_T("HMI_dACF1ADCForceWeight"), &m_dForceBondWeight);

		RegVariable(_T("HMI_dACF1ADCForceSnrFactor"), &m_dForceSensorFactor);

		RegVariable(GetStation() + _T("bLogForceSensor"), &m_bLogForceSensor);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetLogForceSensor"), HMI_SetLogForceSensor); //20150315

		RegVariable(GetStation() + _T("bLogForceSensorEndOfBond"), &m_bLogForceSensorEndOfBond); //20161117
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetLogForceSensorEndOfBond"), HMI_SetLogForceSensorEndOfBond); 

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ACFReadADCForce"), HMI_MBReadADCForce);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetForceSensorFactor"), HMI_SetForceSensorFactor);
		// -------------------------------------------------------
		// commands
		// -------------------------------------------------------
		// hardware
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnIndex"), HMI_PowerOnIndex);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnRoller"), HMI_PowerOnRoller);
	
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleHeadDownSol"), HMI_ToggleHeadDownSol);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleCounterBalanceSol"), HMI_ToggleCounterBalanceSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleCutterOnSol"), HMI_ToggleCutterOnSol);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleHeadUpSol"), HMI_ToggleHeadUpSol);
// 20150204		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleStripperSol"), HMI_ToggleStripperSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleForceChargeAmpOperateSol"), HMI_ToggleForceChargeAmpOperateSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleForceChargeAmpRangeSol"), HMI_ToggleForceChargeAmpRangeSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleIonizerPowerEnable"), HMI_ToggleIonizerPowerEnable);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ResetNumOfPressCount"), HMI_ResetNumOfPressCount); //20150416

		// Setup Page
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDiagnSteps"), HMI_SetDiagnSteps);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexCCW"), HMI_IndexCCW);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexCW"), HMI_IndexCW);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("RollerCCW"), HMI_RollerCCW);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("RollerCW"), HMI_RollerCW);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("RollerGoSearchUpper"), HMI_RollerGoSearchUpper);		// New
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnForceZ"), HMI_PowerOnForceZ);		// 20150205
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("CommForceZ"), HMI_CommForceZ);		// 20150205
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexZPos"), HMI_IndexZPos);		// 20150205
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexZNeg"), HMI_IndexZNeg);		// 20150205
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnZ"), HMI_PowerOnZ);		// 20150205
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeZ"), HMI_HomeZ);		// 20150205
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("CommZ"), HMI_CommZ);		// 20150205
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexXPos"), HMI_IndexXPos);		// 20150205
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexXNeg"), HMI_IndexXNeg);		// 20150205
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnX"), HMI_PowerOnX);		// 20150205
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeX"), HMI_HomeX);		// 20150205
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("CommX"), HMI_CommX);		// 20150205
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexStripperXPos"), HMI_IndexStripperXPos);		// 20150205
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexStripperXNeg"), HMI_IndexStripperXNeg);		// 20150205
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnStripperX"), HMI_PowerOnStripperX);		// 20150205
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeStripperX"), HMI_HomeStripperX);		// 20150205
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("CommStripperX"), HMI_CommStripperX);		// 20150205

		//Force
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDAC"), HMI_SetDAC);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetFinalDAC"), HMI_SetFinalDAC);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetForce"), HMI_SetForce);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetUpDAC"), HMI_SetUpDAC);

		//Para
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetCutterToHeadOffset"), HMI_SetCutterToHeadOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetCutterToHeadDistance"), HMI_SetCutterToHeadDistance);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPreAttachDelay"), HMI_SetPreAttachDelay);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetAttachACFDelay"), HMI_SetAttachACFDelay);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPressTestDelay"), HMI_SetPressTestDelay);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetNumOfPressBeforeAlertLimit"), HMI_SetNumOfPressBeforeAlertLimit); //20150416

		//Force Calib
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetCalibPreAttachDelay"), HMI_SetCalibPreAttachDelay);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetCalibAttachDelay"), HMI_SetCalibAttachDelay);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetCalibFinalPressureCtrl"), HMI_SetCalibFinalPressureCtrl);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetCalibUpPressureCtrl"), HMI_SetCalibUpPressureCtrl);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetCalibPressureCtrl"), HMI_SetCalibPressureCtrl);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetCalibForceCtrl"), HMI_SetCalibForceCtrl);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetForceCalibPressCycle"), HMI_SetForceCalibPressCycle);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("InsertForceCalibrationData"), HMI_InsertForceCalibrationData); //20150512
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ResetForceCalibrationData"), HMI_ResetForceCalibrationData);

		// Indexing ACF Related
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("InsertACFArray"), HMI_InsertACFArray);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("RemoveACFArray"), HMI_RemoveACFArray);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ClearACFArray"), HMI_ClearACFArray);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetACFIndexerFactor"), HMI_SetACFIndexerFactor); //20120706

		//Action Related
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("CutACFOperation"), HMI_CutACFOperation);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexToACFHeadOperation"), HMI_IndexToACFHeadOperation);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("RecoverACFArrayOperation"), HMI_RecoverACFArrayOperation);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexNextACFOperation"), HMI_IndexNextACFOperation);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SingleCycleOperation"), HMI_SingleCycleOperation);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ForceCalibration"), HMI_ForceCalibration);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ForceCalibPressTest"), HMI_ForceCalibPressTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SinglePressTest"), HMI_SinglePressTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("StartPressTest"), HMI_StartPressTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetBondForceMode"), HMI_SetBondForceMode);
		
		RegVariable(GetStation() + _T("lCurrBondForceMode"), &m_lCurrBondForceMode);

	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
}
