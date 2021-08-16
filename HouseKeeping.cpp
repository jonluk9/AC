/////////////////////////////////////////////////////////////////
//	CHouseKeeping.cpp : interface of the CHouseKeeping class
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

#define HOUSEKEEPING_EXTERN	 // must pust here 
#include "HouseKeeping.h"

#include "ACF1.h"

#include "ACF1WH.h"
#include "ACF2WH.h"

#include "Cal_Util.h"
#include "FileOperations.h"

#include "TempDisplay.h" //20120305

#include "TA1.h"
#include "TA2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern STATUS_TYPE		TRAY_IN_ELEVATOR_Status;
extern BOOL				g_bIsVisionNTInited;
extern OFFLINE_INPUT_DATA gOfflineInputData[OFFLINE_MAX];

IMPLEMENT_DYNCREATE(CHouseKeeping, CAC9000Stn)

CHouseKeeping::CHouseKeeping()
{
	// Si Related

	//dummy Motor for WH-Z rotary-linear calibration
	//m_stMotorEncoderOnlyTmp1.InitMotorInfo("HOUSEKEEPING_ENC_ONLY" , &m_bModSelected); //whz rotary linear calib

	//m_stACF1IonizerCheckSnr		= CSiInfo("ACF1IonizerCheckSnr",	"IOMODULE_IN_PORT_24", BIT_16, ACTIVE_HIGH);//<- Not OK!!
	//m_mySiList.Add((CSiInfo*)&m_stACF1IonizerCheckSnr);	
	//m_stACF1IonizerErrorSnr		= CSiInfo("ACF1IonizerErrorSnr",	"IOMODULE_IN_PORT_24", BIT_17, ACTIVE_HIGH);//<- Not OK!!
	//m_mySiList.Add((CSiInfo*)&m_stACF1IonizerErrorSnr);
	//m_stACF2IonizerCheckSnr		= CSiInfo("ACF2IonizerCheckSnr",	"IOMODULE_IN_PORT_24", BIT_18, ACTIVE_HIGH);//<- Not OK!!
	//m_mySiList.Add((CSiInfo*)&m_stACF2IonizerCheckSnr);	
	//m_stACF2IonizerErrorSnr		= CSiInfo("ACF2IonizerErrorSnr",	"IOMODULE_IN_PORT_24", BIT_19, ACTIVE_HIGH);//<- Not OK!!
	//m_mySiList.Add((CSiInfo*)&m_stACF2IonizerErrorSnr);	
	//m_stHTPwrOnSnr			= CSiInfo("HTPwrOnSnr",			"IOMODULE_IN_PORT_24", BIT_7, ACTIVE_LOW);
	//m_mySiList.Add((CSiInfo*)&m_stHTPwrOnSnr); //ok	

	m_stSMPS200VSnr			= CSiInfo("SMPS200VStatus",			"IOMODULE_IN_PORT_2", BIT_16, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stSMPS200VSnr); //ok	
	m_stPwrOFF				= CSiInfo("PwrOFF",					"IOMODULE_IN_PORT_2", BIT_17, ACTIVE_LOW); //20150706 ACTIVE_HIGH
	m_mySiList.Add((CSiInfo*)&m_stPwrOFF); //ok	
	m_stEMODetectSnr		= CSiInfo("EMODetectSnr",			"IOMODULE_IN_PORT_2", BIT_18, ACTIVE_HIGH); //20150706 ACTIVE_LOW
	m_mySiList.Add((CSiInfo*)&m_stEMODetectSnr); //ok	
	m_stMainPowerFaultSnr	= CSiInfo("MainPowerFaultSnr",		"IOMODULE_IN_PORT_2", BIT_19, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stMainPowerFaultSnr); //ok	

	m_stFH1Snr	= CSiInfo("FH1",	"IOMODULE_IN_PORT_2", BIT_20, ACTIVE_HIGH);  //20150706 ACTIVE_LOW
	m_mySiList.Add((CSiInfo*)&m_stFH1Snr); //ok

	m_stFH2Snr	= CSiInfo("FH2",	"IOMODULE_IN_PORT_2", BIT_21, ACTIVE_HIGH);  //20150706 ACTIVE_LOW
	m_mySiList.Add((CSiInfo*)&m_stFH2Snr); //ok

	m_stFL1Snr	= CSiInfo("FL1",	"IOMODULE_IN_PORT_2", BIT_22, ACTIVE_HIGH);  //20150706 ACTIVE_LOW
	m_mySiList.Add((CSiInfo*)&m_stFL1Snr); //ok

	m_stFL2Snr	= CSiInfo("FL2",	"IOMODULE_IN_PORT_2", BIT_23, ACTIVE_HIGH);  //20150706 ACTIVE_LOW
	m_mySiList.Add((CSiInfo*)&m_stFL2Snr); //ok



	m_stSPSnr	= CSiInfo("SP",										"IOMODULE_IN_PORT_5", BIT_0, ACTIVE_HIGH);  //20150706 ACTIVE_LOW
	m_mySiList.Add((CSiInfo*)&m_stSPSnr); //ok
	
	m_stAirSupplyCleanerSnr	= CSiInfo("AirSupplyCleanerSnr",		"IOMODULE_IN_PORT_5", BIT_1, ACTIVE_LOW);  
	m_mySiList.Add((CSiInfo*)&m_stAirSupplyCleanerSnr); //ok

	m_stAirSupplySnr			= CSiInfo("AirSupplySnr",			"IOMODULE_IN_PORT_5", BIT_2, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stAirSupplySnr);	//ok

	m_stCEBypassKeyOnSnr			= CSiInfo("CEBypassKeyOnSnr",	"IOMODULE_IN_PORT_5", BIT_3, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stCEBypassKeyOnSnr);	//ok

	//m_stSafetyLightCurtainSnr	= CSiInfo("SafetyLightCurtainSnr",	"IOMODULE_IN_PORT_16", BIT_3, ACTIVE_LOW);	// 20140908 Yip: Change From ACTIVE_HIGH To ACTIVE_LOW
	//m_mySiList.Add((CSiInfo*)&m_stSafetyLightCurtainSnr); //ok	
	//m_stPBFrontCoverSnr			= CSiInfo("PBFrontCoverSnr",		"IOMODULE_IN_PORT_16", BIT_4, ACTIVE_LOW);
	//m_mySiList.Add((CSiInfo*)&m_stPBFrontCoverSnr);	//ok




	m_stStartButtonSnr			= CSiInfo("StartButtonSnr",		"IOMODULE_IN_PORT_6", BIT_21, ACTIVE_LOW);  //acf chassis
	m_mySiList.Add((CSiInfo*)&m_stStartButtonSnr);

	m_stStopButtonSnr			= CSiInfo("StopButtonSnr",		"IOMODULE_IN_PORT_6", BIT_22, ACTIVE_LOW); 
	m_mySiList.Add((CSiInfo*)&m_stStopButtonSnr);

	m_stResetButtonSnr			= CSiInfo("ResetButtonSnr",		"IOMODULE_IN_PORT_6", BIT_23, ACTIVE_LOW); 
	m_mySiList.Add((CSiInfo*)&m_stResetButtonSnr);




	m_stACFFrontDoorLockSnr		= CSiInfo("ACFFrontDoorLockSnr",	"IOMODULE_IN_PORT_5", BIT_6, ACTIVE_LOW); 
	m_mySiList.Add((CSiInfo*)&m_stACFFrontDoorLockSnr);

	m_stACFBackDoorLockSnr		= CSiInfo("ACFBackDoorLockSnr",		"IOMODULE_IN_PORT_5", BIT_7, ACTIVE_LOW); 
	m_mySiList.Add((CSiInfo*)&m_stACFBackDoorLockSnr);

	m_stCleanVacSnr	= CSiInfo("CleanVacSnr",		"IOMODULE_IN_PORT_2", BIT_14, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stCleanVacSnr);
	m_stCleanPreSnr	= CSiInfo("CleanPreSnr",		"IOMODULE_IN_PORT_2", BIT_15, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stCleanPreSnr);


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// So Related
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_stStartButtonSol			= CSoInfo("StartButtonSol",		"IOMODULE_OUT_PORT_6", BIT_21, OTHER_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stStartButtonSol);	

	m_stResetButtonSol			= CSoInfo("ResetButtonSol",		"IOMODULE_OUT_PORT_6", BIT_23, OTHER_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stResetButtonSol);

	m_stACFFrontDoorLockSol		= CSoInfo("ACFFrontDoorLockSol",	"IOMODULE_OUT_PORT_6", BIT_6, OTHER_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stACFFrontDoorLockSol);	

	m_stACFBackDoorLockSol		= CSoInfo("ACFBackDoorLockSol",		"IOMODULE_OUT_PORT_6", BIT_7, OTHER_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stACFBackDoorLockSol);

	m_stCleanVacSol		= CSoInfo("CleanVacSol",	"IOMODULE_OUT_PORT_1", BIT_14, OTHER_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stCleanVacSol);
	m_stCleanPreSol		= CSoInfo("CleanPreSol",	"IOMODULE_OUT_PORT_1", BIT_15, OTHER_SOL, ACTIVE_LOW);
	m_mySoList.Add((CSoInfo*)&m_stCleanPreSol);



	//m_stMBBarIonPwrEnableSol = CSoInfo("MBBarIonPwrEnableSol", "IOMODULE_OUT_PORT_25", BIT_7, OTHER_SOL, ACTIVE_HIGH);
	//m_mySoList.Add((CSoInfo*)&m_stMBBarIonPwrEnableSol); //ok	// 20141029
	
	//m_stPBBarIonPwrEnableSol	= CSoInfo("PBBarIonPwrEnableSol",	"IOMODULE_OUT_PORT_13", BIT_9, OTHER_SOL, ACTIVE_HIGH);
	//m_mySoList.Add((CSoInfo*)&m_stPBBarIonPwrEnableSol);	//ok // 20141029
	
	m_stRedLight				= CSoInfo("RedLight",				"IOMODULE_OUT_PORT_6", BIT_17, OTHER_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stRedLight);	//ok
	m_stOrangeLight				= CSoInfo("OrangeLight",			"IOMODULE_OUT_PORT_6", BIT_18, OTHER_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stOrangeLight);	//ok
	m_stGreenLight				= CSoInfo("GreenLight",				"IOMODULE_OUT_PORT_6", BIT_19, OTHER_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stGreenLight);	//ok
	m_stBuzzer					= CSoInfo("Buzzer",					"IOMODULE_OUT_PORT_6", BIT_20, OTHER_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stBuzzer);	//ok

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	HMI_bIndexTeflonTest = FALSE;

	m_bDetectAirSupply				= TRUE; //20120305 problem 13
	m_lAirErrorCounter				= 0;
	HMI_bStopProcess				= FALSE; //20121225
	HMI_bChangeACF					= FALSE;	// 20140911 Yip

	m_lACFBondedAlarmCount		= 1000;	//20130709
	m_lACFBondedAlarmLimit		= 0;

	m_lHMI_EncoderOnlyTmp1CurrPosn	= 0; //whz rotary linear calib

	m_bTrayReadySwitchEnable	= FALSE;	// 20140630 Yip
	m_bBeepSound = TRUE; //20140704

	HMI_bAutoTestStop						= FALSE; //20140923
#ifdef PB_LSI_EXPANDSION //20150303
	m_bPRLoopTest = FALSE;
#endif
	m_szLastError = "";
	//m_bHouekeepingDebug = FALSE; //20150714
	
	m_bEnableStartBtn	= FALSE;
	m_bEnableResetBtn	= TRUE;
	m_bEnableStopBtn	= FALSE;
	m_bPressBtnToStop	= FALSE;
	
	SetResetButtonLampSol(ON);
}

CHouseKeeping::~CHouseKeeping()
{
}

BOOL CHouseKeeping::InitInstance()
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

#if 0  //whz rotary linear calib
		m_stMotorEncoderOnlyTmp1.SetAppStation(pAppMod);
		m_stMotorEncoderOnlyTmp1.SetStnName(GetStnName());
		if (pAppMod->IsInitNuDrive())
		{
			m_stMotorEncoderOnlyTmp1.SetGmpChannel(pGmpSys->AddChannel(m_stMotorEncoderOnlyTmp1.stAttrib.szAxis, m_stMotorEncoderOnlyTmp1.stAttrib.dDistPerCount));
			m_stMotorEncoderOnlyTmp1.InitMtrAxis();
			m_stMotorEncoderOnlyTmp1.SetPosition(0);
		}
#endif

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

			//SetMBBarIonPwrEnableSol(ON);
			//SetPBBarIonPwrEnableSol(ON);	// 20141029
			////Heat power always on
			//SetHTPwrOnSol(ON);
			//SetPBHTPwrOnSol(ON);
			

			SetOrangeSol(ON);	// 20140630 Yip: Yellow Light On After Machine Start Up
		}
#if 1 //20120305 problem 13
		if (pAppMod->IsInitNuDrive() && !IsAirSupplyOn())
		{
			pAppMod->m_bHWInitFatalError = TRUE;
			if (pAppMod->m_bDiagnMode)
			{
				m_bDetectAirSupply = FALSE;
				SetError(IDS_HK_PREBOND_AIR_SUPPLY_ERR);

				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				DisplayMessage("WARNING: PreBond Air Supply Error");
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			}
			else
			{
				pAppMod->m_bHWInitError = TRUE;

				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				DisplayMessage("ERROR: PreBond Air Supply Error");
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			}
		}
#endif

	}
	catch (CAsmException e)
	{
		pAppMod->m_bHWInitError = TRUE;

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}


	pAppMod->m_bHWInitCheckingPressure = FALSE;
	return TRUE;
}

INT CHouseKeeping::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CAC9000Stn::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CHouseKeeping::Operation()
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

VOID CHouseKeeping::UpdateOutput()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CGmpSystem *pGmpSys = pAppMod->m_pCGmpSystem;
	static clock_t clkLastESIOQuery = 0;

	INT i = 0;

	BOOL bStopOperation = FALSE;
	LONG lAnswer		= rMSG_TIMEOUT;
	BOOL bShowAirSupplyMessage = FALSE; //20120210

#if 1 //20130410
	if ((m_qState == UN_INITIALIZE_Q) || (m_qState == DE_INITIAL_Q))
#else
	if ((m_qState == UN_INITIALIZE_Q) || (m_qState == DE_INITIAL_Q) || (m_qState == AUTO_Q) || (m_qState == DEMO_Q))
#endif
	{
		return;
	}

	if (pAppMod->m_bHWInitError)
	{
		return;
	}

	if (IsResetBtnOn() && m_bEnableResetBtn)
	{
		m_bEnableStartBtn	= TRUE;
		//m_bEnableResetBtn	= FALSE;
		m_bEnableStopBtn	= FALSE;
		ResetSystem();
	}
 	if (IsStartBtnOn() && m_bEnableStartBtn)
	{
		m_bEnableStartBtn	= FALSE;
		m_bEnableResetBtn	= FALSE;
		m_bEnableStopBtn	= FALSE;
		StartBonding();
		SetResetButtonLampSol(OFF);
	}

	try
	{
		BOOL bPBError = FALSE; //20130412
		BOOL bAirSupplyError = FALSE; 
#if 1 //20171123
		if (m_fHardware && pAppMod->IsInitNuDrive() && 
			((DOUBLE)(labs(clock() - clkLastESIOQuery)) / CLOCKS_PER_SEC > 5.0))
		{
			CString szTmpMsg;
			int nNumOfFaultyIO = 0;
			clkLastESIOQuery = clock();
			if (pGmpSys->QueryFaultyIO(nNumOfFaultyIO))
			{
				int ii = 0, nErrorCode = 0;
				char cPortName[256] = {0,};

				szTmpMsg.Format("FaultyIO: nNumOfFaultyIO = %d", nNumOfFaultyIO);
				DisplayMessage(szTmpMsg);
				for (ii = 0; ii < nNumOfFaultyIO; ii++)
				{
					if (pGmpSys->GetFaultyIOInfo(ii, &cPortName[0], &nErrorCode))
					{
						szTmpMsg.Format("FaultyIO: \nPortName=%s\n ErrCode=%d", &cPortName[0], nErrorCode);
						DisplayMessage(szTmpMsg);
						szTmpMsg.Format("ESIO Error: PortName=%s : Bit-map=0x%x", &cPortName[0], nErrorCode);
						SetError(IDS_HK_ESIO_BOARD_HW_ERR, szTmpMsg);
					}
				}
				pGmpSys->ResetFaultyIO();
			}
		}
#endif
		if (m_fHardware && m_bCheckMotor)
		{
			for (i = 0; i < m_myMotorList.GetSize(); i++)
			{
				GetmyMotorList().GetAt(i)->bHMI_On			= GetmyMotorList().GetAt(i)->IsPowerOn();
				GetmyMotorList().GetAt(i)->lHMI_CurrPosn	= GetmyMotorList().GetAt(i)->GetEncPosn();
			}

#if 1 //whz rotary linear calib
			//if (pAppMod->IsInitNuDrive() && m_bCheckIO)
			{
				m_lHMI_EncoderOnlyTmp1CurrPosn = -1 * m_stMotorEncoderOnlyTmp1.GetEncPosn();
			}
#endif

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
#if 1 //20120305 problem 13 detect air supply
		if (m_fHardware)
		{
			static BOOL bEMODetected = FALSE;
			if (IsEMODetectSnrOn()) //20130829
			{
				if (!bEMODetected)
				{
					// 20140910 Yip: Stop Operation If EMO Is Pressed
					if (State() == AUTO_Q || State() == DEMO_Q || State() == CYCLE_Q)
					{
						bStopOperation = TRUE;
						if (theApp.IsSelectCOG902())
						{
							AC9000_Machine_Status = ST_GW_STOP;
							//CInSHWH *pCInSHWH = (CInSHWH*)m_pModule->GetStation("InSHWH");
							//pCInSHWH->UpdateAC9000MachineStatus();
						}
					}
					DisplayMessage("*** EMO is Pressed ***");
					SetError(IDS_HK_EMO_PRESSED_ERR);	// 20140903 Yip
				}
				bEMODetected = TRUE;
			}
			else
			{
				bEMODetected = FALSE;
			}
		}
		// detect air supply
		if (m_bDetectAirSupply && m_fHardware)
		{
			BOOL bONError = FALSE;
			BOOL bOFFError = FALSE;

			bPBError = !IsAirSupplyOn();
			//on/off Loader
			//if(pAppMod->IsSelectONLoader())
			//{
			//	bONError = !IsONLoaderAirSupplyOn();
			//}
			//if(pAppMod->IsSelectOFFLoader())
			//{
			//	bOFFError = !IsOFFLoaderAirSupplyOn();
			//}

			if (bPBError ||  bONError || bOFFError)
			{
				m_lAirErrorCounter++;
			}
			else
			{
				m_lAirErrorCounter = 0;
			}

			// Error?
			if (m_lAirErrorCounter > 5)		
			{
				CWinEagle *pCWinEagle = (CWinEagle*)m_pModule->GetStation("WinEagle");

				if (bPBError)
				{
					SetError(IDS_HK_PREBOND_AIR_SUPPLY_ERR);
				}

				if (State() == AUTO_Q || State() == DEMO_Q || State() == CYCLE_Q)
				{
					bStopOperation = TRUE;
				}

				//if (pCWinEagle->m_bPRInit)	// Set Error can only be sent when HMI is initialized
				{
					if (!(State() == AUTO_Q || State() == DEMO_Q || State() == CYCLE_Q)) //20120210
					{
#if 1 //20130226
						CTempDisplay *pCTempDisplay = (CTempDisplay*)m_pModule->GetStation("TempDisplay");
						if (bPBError)
						{ 
							//turn off PB heater
							pCTempDisplay->PowerOnHeater(ACF1_CH1, OFF);
							pCTempDisplay->PowerOnHeater(ACF1WH_CH3, OFF);
							pCTempDisplay->PowerOnHeater(ACF2WH_CH4, OFF);
							DisplayMessage("Power Off All ACF Heater. Since Air Supply Error");
						}
#endif
						lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Continue to Monitor air supply?");
						if (lAnswer == rMSG_CANCEL)
						{
							m_bDetectAirSupply = FALSE;
							SetHmiVariable("Housekeeping_bDetectAirSupply");
							HMIMessageBox(MSG_OK, "WARNING", "Please enable air supply detection manually.");
						}
						else if (lAnswer == rMSG_TIMEOUT)
						{
							m_bDetectAirSupply = FALSE;
							SetHmiVariable("Housekeeping_bDetectAirSupply");
						}
#if 1 //20130226
						if (bPBError)
						{
							HMIMessageBox(MSG_OK, "WARNING", "All Heaters are powered down.");
						}
#endif
					}
					else
					{
						bShowAirSupplyMessage = TRUE; //20120210
						//m_bDetectAirSupply = FALSE;
					}
				}
				//else
				//{
				//	m_bDetectAirSupply = FALSE;
				//}
			}
		}

		if (bStopOperation)
		{
			INT nConvID;
			IPC_CServiceMessage stMsg;
			LONG lTimeCount = 0;

			typedef struct
			{
				LONG lAxis;
				LONG lAction;
				LONG lUnit;
				BOOL bReply;
			} PARA;

			PARA stPara;

			stPara.lAxis = 0;
			stPara.lAction = 0;
			stPara.lUnit = 0;
			stPara.bReply = FALSE;

			Command(glSTOP_COMMAND);
			State(STOPPING_Q);

			stMsg.InitMessage(sizeof(PARA), &stPara);
			nConvID = m_comClient.SendRequest("AC9000", "Stop", stMsg);
			while (!m_comClient.ScanReplyForConvID(nConvID, 1) && (lTimeCount++ < 8640000))
			{
				Sleep(1);
			}
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			
			if (bShowAirSupplyMessage) //20120210
			{
				CTempDisplay *pCTempDisplay = (CTempDisplay*)m_pModule->GetStation("TempDisplay");

				if (bPBError)
				{ 
				//turn off PB heater
					pCTempDisplay->PowerOnHeater(ACF1_CH1, OFF);
					pCTempDisplay->PowerOnHeater(ACF1WH_CH3, OFF);
					pCTempDisplay->PowerOnHeater(ACF2WH_CH4, OFF);
					DisplayMessage("Power Off All Heater. Since Air Supply Error");
					HMIMessageBox(MSG_OK, "WARNING", "All Heaters are powered down.");
				}

				//HMIMessageBox(MSG_OK, "WARNING", "Please enable air supply detection manually.");
				lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Continue to Monitor air supply?");
				if (lAnswer == rMSG_CANCEL)
				{
					m_bDetectAirSupply = FALSE;
					SetHmiVariable("Housekeeping_bDetectAirSupply");
					HMIMessageBox(MSG_OK, "WARNING", "Please enable air supply detection manually.");
				}
				else if (lAnswer == rMSG_TIMEOUT)
				{
					m_bDetectAirSupply = FALSE;
					SetHmiVariable("Housekeeping_bDetectAirSupply");
				}
			}
		}

#endif
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	Sleep(UPDATEOUTPUT_SLEEP);
}

VOID CHouseKeeping::RegisterVariables()
{
	try
	{
		RegVariable(_T("HMI_lEncoderOnlyTmp1CurrPosn"), &m_lHMI_EncoderOnlyTmp1CurrPosn); //whz rotary linear calib
		// -------------------------------------------------------
		// AC9000Stn Variable and Function
		// -------------------------------------------------------		
		RegVariable(_T("HouseKeeping_bCheckIO"), &m_bCheckIO);
		RegVariable(_T("HouseKeeping_bCheckMotor"), &m_bCheckMotor);

		// -------------------------------------------------------
		// varibales
		// -------------------------------------------------------
		RegVariable(_T("ulUserGroup"), &g_ulUserGroup);
		RegVariable(_T("szPassword"), &szUserPassword);
		RegVariable(_T("szEngPassword"), &szEngPassword);
		RegVariable(_T("szSvrPassword"), &szSvrPassword);
		
		RegVariable(_T("Housekeeping_ulDetectCover"), &m_ulDetectCover);
		RegVariable(_T("HouseKeeping_bSlowMotionIndexing"), &g_bSlowMotionIndexing);
		RegVariable(_T("HouseKeeping_bDetectAirSupply"), &m_bDetectAirSupply);
		RegVariable(_T("Housekeeping_bDetectLightCurtainSnr"), &m_bDetectLightCurtainSnr);
		RegVariable(_T("Housekeeping_bShowForceInNewton"), &g_bShowForceInNewton);
		RegVariable(_T("HouseKeeping_ulSlowMotionFactor"), &g_ulSlowMotionFactor);

		RegVariable(_T("Housekeeping_bEnableTrigLighting"), &g_bEnableTrigLighting);

		RegVariable(_T("HouseKeeping_dForceCalibratorHeight"), &g_stForceCalibrator.dHeight);

		RegVariable(_T("Housekeeping_dModuleOffset"), &g_dModuleOffset);
		
		RegVariable(_T("Housekeeping_lACFBondedAlarmCount"), &m_lACFBondedAlarmCount); //20130709
		RegVariable(_T("Housekeeping_lACFBondedAlarmLimit"), &m_lACFBondedAlarmLimit);

		RegVariable(_T("HouseKeeping_bTrayReadySwitchEnable"), &m_bTrayReadySwitchEnable);	// 20140630 Yip

		RegVariable(_T("HMI_bAirSupplySnr"), &m_stAirSupplySnr.bHMI_Status); //20120703
		RegVariable(_T("HMI_bByPassOnSnr"), &m_stCEBypassKeyOnSnr.bHMI_Status); //20120703
		RegVariable(_T("HMI_bAirSupplyCleanerSnr"), &m_stAirSupplyCleanerSnr.bHMI_Status); //20120703
		//RegVariable(_T("HMI_bMBAirSupplySnr"), &m_stMBAirSupplySnr.bHMI_Status);
		RegVariable(_T("HMI_bEMODetectSnr"), &m_stEMODetectSnr.bHMI_Status);
		RegVariable(_T("HMI_bMainPowerFaultSnr"), &m_stMainPowerFaultSnr.bHMI_Status);
		//RegVariable(_T("HMI_bSafetyLightCurtainSnr"), &m_stSafetyLightCurtainSnr.bHMI_Status); //20121204

		RegVariable(_T("HMI_bACF1IonizerCheckSnr"), &m_stACF1IonizerCheckSnr.bHMI_Status);
		RegVariable(_T("HMI_bACF1IonizerErrorSnr"), &m_stACF1IonizerErrorSnr.bHMI_Status);

		//RegVariable(_T("HMI_bHTPwrOnSnr"), &m_stHTPwrOnSnr.bHMI_Status);
		RegVariable(_T("HMI_bPowerOff"), &m_stPwrOFF.bHMI_Status);


		RegVariable(_T("HMI_bFH1Snr"), &m_stFH1Snr.bHMI_Status);
		RegVariable(_T("HMI_bFH2Snr"), &m_stFH2Snr.bHMI_Status);
		RegVariable(_T("HMI_bFL1Snr"), &m_stFL1Snr.bHMI_Status);
		RegVariable(_T("HMI_bFL2Snr"), &m_stFL2Snr.bHMI_Status);
		RegVariable(_T("HMI_bSPSnr"), &m_stSPSnr.bHMI_Status);
		RegVariable(_T("HMI_bSMPS200VSnr"), &m_stSMPS200VSnr.bHMI_Status);
			

		RegVariable(_T("HMI_bStartButtonSnr"), &m_stStartButtonSnr.bHMI_Status);
		RegVariable(_T("HMI_bStopButtonSnr"), &m_stStopButtonSnr.bHMI_Status);
		RegVariable(_T("HMI_bResetButtonSnr"), &m_stResetButtonSnr.bHMI_Status);
		
		RegVariable(_T("HMI_bStartButtonSol"), &m_stStartButtonSol.bHMI_Status);
		RegVariable(_T("HMI_bResetButtonSol"), &m_stResetButtonSol.bHMI_Status);

		RegVariable(_T("HMI_bACFFrontDoorLockSol"), &m_stACFFrontDoorLockSol.bHMI_Status);
		RegVariable(_T("HMI_bACFBackDoorLockSol"), &m_stACFBackDoorLockSol.bHMI_Status);

		RegVariable(_T("HMI_bACFFrontDoorLockSnr"), &m_stACFFrontDoorLockSnr.bHMI_Status);
		RegVariable(_T("HMI_bACFBackDoorLockSnr"), &m_stACFBackDoorLockSnr.bHMI_Status);

		RegVariable(_T("HMI_bRedLight"), &m_stRedLight.bHMI_Status);
		RegVariable(_T("HMI_bOrangeLight"), &m_stOrangeLight.bHMI_Status);
		RegVariable(_T("HMI_bGreenLight"), &m_stGreenLight.bHMI_Status);
		RegVariable(_T("HMI_bBuzzer"), &m_stBuzzer.bHMI_Status);

		RegVariable(_T("HMI_bCleanVacSnr"), &m_stCleanVacSnr.bHMI_Status);
		RegVariable(_T("HMI_bCleanPreSnr"), &m_stCleanPreSnr.bHMI_Status);
		RegVariable(_T("HMI_bCleanVacSol"), &m_stCleanVacSol.bHMI_Status);
		RegVariable(_T("HMI_bCleanPreSol"), &m_stCleanPreSol.bHMI_Status);
		

		//new sol (total: 4)
		//m_comServer.IPC_REG_SERVICE_COMMAND("HK_ToggleMBBarIonPwrEnableSol", HMI_ToggleMBBarIonPwrEnableSol);  //20141029
		//m_comServer.IPC_REG_SERVICE_COMMAND("HK_TogglePBBarIonPwrEnableSol", HMI_TogglePBBarIonPwrEnableSol);  //20141029

		//m_comServer.IPC_REG_SERVICE_COMMAND("HK_TogglePBHTPwrOnSol", HMI_TogglePBHTPwrOnSol);
		//m_comServer.IPC_REG_SERVICE_COMMAND("HK_ToggleMBOddIndexRollSol", HMI_ToggleMBOddIndexRollSol);
		//m_comServer.IPC_REG_SERVICE_COMMAND("HK_ToggleMBEvenIndexRollSol", HMI_ToggleMBEvenIndexRollSol);

		// 20140815 Yip
		// New User enter PR center
		RegVariable(_T("HK_bShowUserSearchPRDone"), &g_bShowUserSearchPRDone);
		// Display the message when bonding
		RegVariable(_T("HK_szBondingMsg"), &g_szBondingMsg);

		// -------------------------------------------------------
		// package information
		// -------------------------------------------------------

		RegVariable(_T("PKG_Glass.szName"), &g_stGlass.szName);
		RegVariable(_T("PKG_Glass.dLength"), &g_stGlass.dLength);
		RegVariable(_T("PKG_Glass.dWidth"), &g_stGlass.dWidth);
		RegVariable(_T("PKG_Glass.dThickness1"), &g_stGlass.dThickness1);
		RegVariable(_T("PKG_Glass.dThickness2"), &g_stGlass.dThickness2);
		RegVariable(_T("PKG_Glass.dThickness3"), &g_stGlass.dThickness3);
		RegVariable(_T("PKG_Glass.dThickness4"), &g_stGlass.dThickness4);

		RegVariable(_T("PKG_Glass.dItoWidth"), &g_stGlass.dItoWidth);
		
		//ACF Related
		RegVariable(_T("PKG_ACF1.szName"), &HMI_szACFName);
		RegVariable(_T("PKG_ACF1.dWidth"), &HMI_dACFWidth);	
		RegVariable(_T("PKG_ACF1.dLength"), &HMI_dACFLength);

		// LSI Type Related
		RegVariable(_T("PKG_LSIType1.szName"), &g_stLSIType[LSI_TYPE1].szName);
		RegVariable(_T("PKG_LSIType1.dLength"), &g_stLSIType[LSI_TYPE1].dLength);
		RegVariable(_T("PKG_LSIType1.dWidth"), &g_stLSIType[LSI_TYPE1].dWidth);
		RegVariable(_T("PKG_LSIType1.dThickness"), &g_stLSIType[LSI_TYPE1].dThickness);

		// Rework Mode
		RegVariable(_T("HK_bSelectedReworkMode"), &g_bSelectedReworkMode);
		RegVariable(_T("HK_bIndexTeflonTest"), &HMI_bIndexTeflonTest);

		//Glass input Mode 20120412
		RegVariable(_T("Housekeeping_lGlassInputMode"), &g_lGlassInputMode);
		// LST On Glass Related
		
		// MB Press Shift 

		RegVariable(_T("HMI_bStopProcess"), &HMI_bStopProcess); //20121225
		RegVariable(_T("HMI_bChangeACF"), &HMI_bChangeACF);	// 20140911 Yip

		//RegVariable(_T("HMI_bHouseKeepDebug"), &m_bHouekeepingDebug);	// 20150714
		// -------------------------------------------------------
		// commands
		// -------------------------------------------------------
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_AutoTestStop", HMI_PRAutoTestStop); //20140923
		//PreBond I/O
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_ToggleStartBtnSol", HMI_ToggleStartBtnSol);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_ToggleResetBtnSol", HMI_ToggleResetBtnSol);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_ToggleRedSol", HMI_ToggleRedSol);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_ToggleOrangeSol", HMI_ToggleOrangeSol);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_ToggleGreenSol", HMI_ToggleGreenSol);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_ToggleBuzzerSol", HMI_ToggleBuzzerSol);

		m_comServer.IPC_REG_SERVICE_COMMAND("HK_ToggleACFFrontDoorLockSol", HMI_ToggleACFFrontDoorLockSol);	// 20150204
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_ToggleACFBackDoorLockSol", HMI_ToggleACFBackDoorLockSol);	// 20150204

		m_comServer.IPC_REG_SERVICE_COMMAND("HK_ToggleCleanVacSol", HMI_ToggleCleanVacSol);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_ToggleCleanPreSol", HMI_ToggleCleanPreSol);

		//m_comServer.IPC_REG_SERVICE_COMMAND("HK_ToggleMBFrontDoorLockSol", HMI_ToggleMBFrontDoorLockSol);	// 20150204
		
#ifdef PB_LSI_EXPANDSION //20150303
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_StopPRLoopTest", HMI_StopPRLoopTest);	// 20150204
#endif
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_TA1ZMoveLoopStop", HMI_TA1ZMoveLoopStop);	// 20150615

		//m_comServer.IPC_REG_SERVICE_COMMAND("HK_SetDebugVar", HMI_SetDebugVar);	// 20150714

		// -------------------------------------------------------
		// Setting
		// -------------------------------------------------------
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_BackUpPackages", HMI_BackUpPackages);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_ChangePassword", HMI_ChangePassword);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_LangSwitching", HMI_LangSwitching);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_SetAutoMode", HMI_SetAutoMode);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_SetForceCalibHeight", HMI_SetForceCalibHeight);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_SelectShowForceMode", HMI_SelectShowForceMode);

		m_comServer.IPC_REG_SERVICE_COMMAND("HK_SetModuleOffset", HMI_SetModuleOffset);

		m_comServer.IPC_REG_SERVICE_COMMAND("HK_SetDetectCover", HMI_SetDetectCover);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_SetDetectLightCurtainSnr", HMI_SetDetectLightCurtainSnr);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_SetDetectAirSupply", HMI_SetDetectAirSupply);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_SetSlowMotionIndexing", HMI_SetSlowMotionIndexing);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_SetSlowMotionFactor", HMI_SetSlowMotionFactor);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_SetEnableTrigLighting", HMI_SetEnableTrigLighting);

		m_comServer.IPC_REG_SERVICE_COMMAND("HK_CheckUserGroup", HMI_CheckUserGroup);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_testPinDetectSpringStop", HMI_testPinDetectSpringStop); //20121009 pin detection spring test

		m_comServer.IPC_REG_SERVICE_COMMAND("HK_SetTrayReadySwitchEnable", HMI_SetTrayReadySwitchEnable);	// 20140630 Yip

		m_comServer.IPC_REG_SERVICE_COMMAND("HK_SetChangeACF", HMI_SetChangeACF);	// 20140917 Yip

		// -------------------------------------------------------
		// package information
		// -------------------------------------------------------
		// Glass Related
		m_comServer.IPC_REG_SERVICE_COMMAND("PKG_SetGlassName", HMI_SetGlassName);
		m_comServer.IPC_REG_SERVICE_COMMAND("PKG_SetGlassLength", HMI_SetGlassLength);
		m_comServer.IPC_REG_SERVICE_COMMAND("PKG_SetGlassWidth", HMI_SetGlassWidth);
		m_comServer.IPC_REG_SERVICE_COMMAND("PKG_SetReflectorThickness", HMI_SetReflectorThickness);
		m_comServer.IPC_REG_SERVICE_COMMAND("PKG_SetUpperGlassThickness", HMI_SetUpperGlassThickness);
		m_comServer.IPC_REG_SERVICE_COMMAND("PKG_SetLowerGlassThickness", HMI_SetLowerGlassThickness);
		m_comServer.IPC_REG_SERVICE_COMMAND("PKG_SetPolarizerThickness", HMI_SetPolarizerThickness);

		m_comServer.IPC_REG_SERVICE_COMMAND("PKG_SetGlassItoWidth", HMI_SetGlassItoWidth);

		// ACF
		m_comServer.IPC_REG_SERVICE_COMMAND("PKG_SetACFName", HMI_SetACFName);
		m_comServer.IPC_REG_SERVICE_COMMAND("PKG_SetACFWidth", HMI_SetACFWidth);
		m_comServer.IPC_REG_SERVICE_COMMAND("PKG_SetACFLength", HMI_SetACFLength);
		m_comServer.IPC_REG_SERVICE_COMMAND("HMI_SetACFWasteCollectionLimit", HMI_SetACFWasteCollectionLimit); //20130709

		// LSI Type Related
		m_comServer.IPC_REG_SERVICE_COMMAND("PKG_SetLSIType1Name", HMI_SetLSIType1Name);
		m_comServer.IPC_REG_SERVICE_COMMAND("PKG_SetLSIType1Length", HMI_SetLSIType1Length);
		m_comServer.IPC_REG_SERVICE_COMMAND("PKG_SetLSIType1Width", HMI_SetLSIType1Width);
		m_comServer.IPC_REG_SERVICE_COMMAND("PKG_SetLSIType1Thickness", HMI_SetLSIType1Thickness);
		
		//Rework
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_SelectedReworkMode", HMI_SelectedReworkMode);
		
		//Glass input mode 20120412
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_SetGlassInputMode", HMI_SetGlassInputMode);
		
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_LightAndBuzzerInIdle", HMI_LightAndBuzzerInIdle); //20130115

//p20120918
		//Traditional Chinese Language Setting
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_HMILangTraditionalChinese", HMILangTraditionalChinese);
//p20120918:end
		//m_comServer.IPC_REG_SERVICE_COMMAND("HK_UpdateCleanerWhStatus", GatewayUpdateGC902CWHStatus);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_CheckGatewayConnection", CheckGatewayConnection);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_GetGatewayTA1Status", GetGatewayTA5Status);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_ProcessGatewayCommand", ProcessGatewayCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_UpdateTA5Status", GatewayUpdateCOG902TA5Status);
		m_comServer.IPC_REG_SERVICE_COMMAND("HK_UpdateMachineStatus", GatewayUpdateCOG902MachineStatus);

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
VOID CHouseKeeping::PreStartOperation()
{
	m_bErrorExist = FALSE; //20130115
	m_bAlertExist = FALSE;
	m_szLastError = "";	// 20140814 Yip
	SetRedSol(OFF);
	SetOrangeSol(OFF);
	SetGreenSol(ON);
	SetBuzzerSol(OFF);
	//m_bHouekeepingDebug = FALSE; //20150714

	g_bStopCycle = FALSE; //20130412
	
	m_bEnableStartBtn	= FALSE;
	m_bEnableResetBtn	= FALSE;
	m_bEnableStopBtn	= TRUE;
	m_bPressBtnToStop   = FALSE;
	
	SetResetButtonLampSol(OFF);

	Motion(FALSE);
	State(IDLE_Q);
	CAC9000Stn::PreStartOperation();
}

VOID CHouseKeeping::AutoOperation()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	static BOOL bPrevAlertState = FALSE;

	if (Command() == glSTOP_COMMAND || g_bStopCycle)
	{
		m_bEnableStartBtn	= TRUE;
		m_bEnableResetBtn	= TRUE;
		m_bEnableStopBtn	= FALSE;
		State(STOPPING_Q);
		SetResetButtonLampSol(ON);
	}
	else
	{
		OperationSequence();

		// Handle error
		if (m_bErrorExist)
		{
			bPrevAlertState = FALSE; //20140704
			SetRedSol(ON);
			SetOrangeSol(OFF);
			SetGreenSol(OFF);
			SetBuzzerSol(ON);
		}
		else if (m_bAlertExist)
		{
			if (!bPrevAlertState) //20140704
			{
				bPrevAlertState = TRUE;
				m_bBeepSound = TRUE;
			}
			SetRedSol(ON);
			SetOrangeSol(OFF);
			SetGreenSol(OFF);
			if (m_bBeepSound)
			{
				SetBuzzerSol(ON);
			}
			Sleep(300);

			SetRedSol(OFF);
			SetOrangeSol(OFF);
			SetGreenSol(OFF);
			SetBuzzerSol(OFF);
			Sleep(100);
		}
		else
		{
			bPrevAlertState = FALSE; //20140704
			SetRedSol(OFF);
			SetOrangeSol(OFF);
			SetGreenSol(ON);
			SetBuzzerSol(OFF);
		}

		Sleep(100);

	}
}

VOID CHouseKeeping::StopOperation()
{
#if 1 //20130115
	BOOL bErrorExit = FALSE;
	//Sleep(100);

	bErrorExit = m_bErrorExist;
	m_bErrorExist = FALSE;
	m_bAlertExist = FALSE;
	//m_szLastError = "";	// 20140814 Yip

	if (bErrorExit)
	{
		SetRedSol(ON);
		SetOrangeSol(OFF);
		SetGreenSol(OFF);
		SetBuzzerSol(OFF);
	}
	else
	{
		SetRedSol(OFF);
		SetOrangeSol(ON);
		SetGreenSol(OFF);
		SetBuzzerSol(OFF);
	}
#else
	//Sleep(100);

	m_bErrorExist = FALSE;
	m_bAlertExist = FALSE;
	//m_szLastError = "";	// 20140814 Yip

	SetRedSol(OFF);
	SetOrangeSol(ON);
	SetGreenSol(OFF);
	SetBuzzerSol(OFF);
#endif
	CAC9000Stn::StopOperation();
}

/////////////////////////////////////////////////////////////////
//Operational Sequences
/////////////////////////////////////////////////////////////////
VOID CHouseKeeping::OperationSequence()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	if (m_bDetectCoverStopOp)
	{
		if (!IsACFFrontCoverOn())
		{
			m_bErrorExist = TRUE;
			SetError(IDS_HK_FRONT_COVER_NOT_DETECTED_ERR);
			Command(glSTOP_COMMAND, TRUE);
			g_bStopCycle = TRUE;
			DisplayMessage("FrontCover");
		}
		if (!IsACFBackCoverOn())
		{
			m_bErrorExist = TRUE;
			SetError(IDS_HK_BACK_COVER_NOT_DETECTED_ERR);
			Command(glSTOP_COMMAND, TRUE);
			g_bStopCycle = TRUE;
			DisplayMessage("FrontCover");
		}
	}

	if (m_bDetectCoverSlowMotion)
	{
		if (!IsACFFrontCoverOn() || !IsACFBackCoverOn())
		{
			SetMotionProfileMode(SLOW_MOT_PROF);
		}
		else
		{
			SetMotionProfileMode(NORMAL_MOT_PROF);
		}
	}
	if (IsStopBtnOn() && m_bEnableStopBtn)
	{
		m_bEnableStartBtn	= FALSE;
		m_bEnableResetBtn	= FALSE;
		m_bEnableStopBtn	= FALSE;
		m_bPressBtnToStop	= TRUE;
		StopBonding();
	}
}

VOID CHouseKeeping::SetMotionProfileMode(ULONG ulMode)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	INT i = 0;
	INT j = 0;
	
	for (i = 0; i < pAppMod->GetStationList().GetSize(); i++)
	{
		for (j = 0; j < pAppMod->GetStationList().GetAt(i)->GetmyMotorList().GetSize(); j++)
		{
			pAppMod->GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->stMotionProfile.sMotionProfID = (SHORT)ulMode;
		}
	}
}

/////////////////////////////////////////////////////////////////
//I/O Related
/////////////////////////////////////////////////////////////////
//BOOL CHouseKeeping::IsPushBtnX1On()
//{
//	BOOL bResult = FALSE;
//
//#ifdef OFFLINE
//	return bResult;
//#endif 
//
//#ifdef _DEBUG	// 20140917 Yip
//	if (!theApp.IsInitNuDrive())
//	{
//		return bResult;
//	}
//#endif
//
//	try
//	{
//		bResult = m_stX1PushBtnSnr.GetGmpPort().IsOn();
//		if (bResult)	// 20140630 Yip
//		{
//			Sleep(10);
//			bResult = m_stX1PushBtnSnr.GetGmpPort().IsOn();
//		}
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
//
//BOOL CHouseKeeping::IsPushBtnX2On()
//{
//	BOOL bResult = FALSE;
//
//#ifdef OFFLINE
//	return bResult;
//#endif 
//
//#ifdef _DEBUG	// 20140917 Yip
//	if (!theApp.IsInitNuDrive())
//	{
//		return bResult;
//	}
//#endif
//
//	try
//	{
//		bResult = m_stX2PushBtnSnr.GetGmpPort().IsOn();
//		if (bResult)	// 20140630 Yip
//		{
//			Sleep(10);
//			bResult = m_stX2PushBtnSnr.GetGmpPort().IsOn();
//		}
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


BOOL CHouseKeeping::IsAirSupplyOn() //20120703
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
		bResult = m_stAirSupplySnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CHouseKeeping::IsEMODetectSnrOn()
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
//return FALSE;  //20140406 debug only
	try
	{
		bResult = m_stEMODetectSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		bResult = TRUE;	// 20140910 Yip: Assume EMO Is Pressed If Read I/O Failed
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CHouseKeeping::IsMainPowerFaultSnrOn()
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
		bResult = m_stMainPowerFaultSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

//BOOL CHouseKeeping::IsSafetyLightCurtainSnrOn() //20121204
//{
//	BOOL bResult = FALSE;
//
//#ifdef OFFLINE
//	return bResult;
//#endif 
//
//#ifdef _DEBUG	// 20140917 Yip
//	if (!theApp.IsInitNuDrive())
//	{
//		return bResult;
//	}
//#endif
//
//	try
//	{
//		bResult = m_stSafetyLightCurtainSnr.GetGmpPort().IsOn();
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

////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////


BOOL CHouseKeeping::IsACFFrontCoverOn() 
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
		bResult = m_stACFFrontDoorLockSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}


BOOL CHouseKeeping::IsACFBackCoverOn()
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
		bResult = m_stACFBackDoorLockSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


BOOL CHouseKeeping::IsStartBtnOn()
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
		bResult = m_stStartButtonSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CHouseKeeping::IsResetBtnOn()
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
		bResult = m_stResetButtonSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CHouseKeeping::IsStopBtnOn()
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
		bResult = m_stStopButtonSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

INT CHouseKeeping::SetRedSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;
	CSoInfo *stTempSo = &m_stRedLight;

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
			stTempSo->GetGmpPort().On();
			stTempSo->bHMI_Status = TRUE;
		}
		else
		{
			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;
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

INT CHouseKeeping::SetOrangeSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;
	CSoInfo *stTempSo = &m_stOrangeLight;

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
			stTempSo->GetGmpPort().On();
			stTempSo->bHMI_Status = TRUE;
		}
		else
		{
			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;
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

INT CHouseKeeping::SetGreenSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;
	CSoInfo *stTempSo = &m_stGreenLight;

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
			stTempSo->GetGmpPort().On();
			stTempSo->bHMI_Status = TRUE;
		}
		else
		{
			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;
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

INT CHouseKeeping::SetBuzzerSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;
	CSoInfo *stTempSo = &m_stBuzzer;

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
			stTempSo->GetGmpPort().On();
			stTempSo->bHMI_Status = TRUE;
		}
		else
		{
			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;
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


INT CHouseKeeping::SetACFFrontDoorLockSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;
	CSoInfo *stTempSo = &m_stACFFrontDoorLockSol;

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
			stTempSo->GetGmpPort().On();
			stTempSo->bHMI_Status = TRUE;
		}
		else
		{
			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;
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

INT CHouseKeeping::SetACFBackDoorLockSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;
	CSoInfo *stTempSo = &m_stACFBackDoorLockSol;

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
			stTempSo->GetGmpPort().On();
			stTempSo->bHMI_Status = TRUE;
		}
		else
		{
			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;
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


INT CHouseKeeping::SetStartButtonLampSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;
	CSoInfo *stTempSo = &m_stStartButtonSol;

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
			stTempSo->GetGmpPort().On();
			stTempSo->bHMI_Status = TRUE;
		}
		else
		{
			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;
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

INT CHouseKeeping::SetResetButtonLampSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;
	CSoInfo *stTempSo = &m_stResetButtonSol;

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
			stTempSo->GetGmpPort().On();
			stTempSo->bHMI_Status = TRUE;
		}
		else
		{
			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;
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



DOUBLE CHouseKeeping::GetSoftwareVersion(CString Number)
{
	double result;

	result = (double)atof(Number.Right(5));
	
	return result;
}

//////////////////////////////////////////////////
// Print Data
//////////////////////////////////////////////////
BOOL CHouseKeeping::PrintHeader(CString szStation, CString szEvent)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bResult = TRUE;

	FILE *fp = fopen(_T("D:\\sw\\AC9000\\Data\\") + szEvent + _T(".txt"), "a+");
	if (fp != NULL)
	{
		fprintf(fp, "\nSoftware Version\tFile Name\tStation\tEvent\n"); 
		fprintf(fp, "%s\t%s\t%s\t%s\n", (const char*)pAppMod->m_szSoftVersion, (const char*)pAppMod->m_szDeviceFile, (const char*)szStation, (const char*)szEvent);
		fclose(fp);
	}

	return bResult;
}

BOOL CHouseKeeping::PrintTitle(CString szEvent, CString szTitle[])
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bResult = TRUE;

	CString szPrintTitle = _T("");

	szPrintTitle = _T("Time\t");

	for (INT i = 0; i < MAX_PRINT_DATA; i++)
	{
		if (szTitle[i] == "")
		{
			break;
		}

		szPrintTitle += szTitle[i] + _T("\t");
	}

	FILE *fp = fopen(_T("D:\\sw\\AC9000\\Data\\") + szEvent + _T(".txt"), "a+");
	if (fp != NULL)
	{
		fprintf(fp, szPrintTitle + _T("\n"));
		fclose(fp);
	}

	return bResult;
}

BOOL CHouseKeeping::PrintData(CString szEvent, PRINT_DATA stPrintData)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bResult = TRUE;
	CString szMsg = _T("");
	CString szPrintData = _T("");
	CString szTempData = _T("");

	SYSTEMTIME ltime;		// New Command

	// Get time as 64-bit integer.
	GetLocalTime(&ltime);

	// Convert to local time.
	szPrintData.Format("%04u-%02u-%02u %02u:%02u:%02u\t", 
					   ltime.wYear, ltime.wMonth, ltime.wDay, 
					   ltime.wHour, ltime.wMinute, ltime.wSecond);

	for (INT i = 0; i < MAX_PRINT_DATA; i++)
	{
		if (stPrintData.szTitle[i] == "")
		{
			break;
		}

		szTempData.Format("%.3f\t", stPrintData.dData[i]);

		szPrintData += szTempData;
	}

	FILE *fp = fopen(_T("D:\\sw\\AC9000\\Data\\") + szEvent + _T(".txt"), "a+");
	if (fp != NULL)
	{
#if 1 //20130425
		//szMsg.Format("PrintData:\\sw\\AC9000\\Data\\%s", szEvent);
		//DisplayMessage(szMsg);

		fseek(fp, 0, SEEK_END);
		if (ftell(fp) < 10)
		{
			for (INT i = 0; i < MAX_PRINT_DATA; i++)
			{
				fprintf(fp, stPrintData.szTitle[i] + "\t");
				if (stPrintData.szTitle[i] == "")
				{
					break;
				}
			}
			fprintf(fp, "\n");
		}
#endif
		fprintf(fp, szPrintData + _T("\n"));
		fclose(fp);
	}

	return bResult;
}

BOOL CHouseKeeping::IsDataFileExist(CString szEvent)	// 20140619 Yip
{
	return (CheckPath(_T("D:\\sw\\AC9000\\Data\\") + szEvent + _T(".txt")) != PATH_NOT_FOUND);
}

VOID CHouseKeeping::PrintMachineSetup(FILE *fp)
{
	CAC9000Stn::PrintMachineSetup(fp);
}

VOID CHouseKeeping::PrintDeviceSetup(FILE *fp)
{
	CAC9000Stn::PrintDeviceSetup(fp);
}

#ifdef EXCEL_MACHINE_DEVICE_INFO
BOOL CHouseKeeping::printMachineDeviceInfo()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bResult = TRUE;
	CString szBlank("--");
	CString szModule = GetStnName();
	LONG lValue = 0;
	DOUBLE dValue = 0.0;
	CString szStr = _T("");

	if (bResult)
	{
		dValue = (DOUBLE) pAppMod->m_smfMachine[GetStnName()]["Tools"]["g_stForceCalibrator.dHeight"];
		bResult = printMachineInfoRow(szModule, CString("Tools"), CString("ForceCalibrator.Height"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, dValue);
	}
	if (bResult)
	{
		dValue = (DOUBLE) pAppMod->m_smfMachine[GetStnName()]["MechanicalDesignError"]["g_dModuleOffset"];
		bResult = printMachineInfoRow(szModule, CString("MechanicalDesignError"), CString("ModuleOffset"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, dValue);
	}
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Settings"]["g_bShowForceInNewton"];
		bResult = printMachineInfoRow(szModule, CString("Settings"), CString("ShowForceInNewton"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Settings"]["m_ulDetectCover"];
		bResult = printMachineInfoRow(szModule, CString("Settings"), CString("DetectCover"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Settings"]["m_bDetectLightCurtainSnr"];
		bResult = printMachineInfoRow(szModule, CString("Settings"), CString("DetectLightCurtainSnr"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Settings"]["g_lGlassInputMode"];
		bResult = printMachineInfoRow(szModule, CString("Settings"), CString("GlassInputMode"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Settings"]["m_lACFBondedAlarmLimit"]; //ACF collection bin
		bResult = printMachineInfoRow(szModule, CString("Settings"), CString("ACFBondedAlarmLimit"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Settings"]["m_lACFBondedAlarmCount"]; //ACF collection bin
		bResult = printMachineInfoRow(szModule, CString("Settings"), CString("ACFBondedAlarmCount"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
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
//SI/SO
LONG CHouseKeeping::HMI_ToggleResetBtnSol(IPC_CServiceMessage &svMsg)
{
	if (m_stResetButtonSol.bHMI_Status)
	{
		SetResetButtonLampSol(OFF);
	}
	else
	{
		SetResetButtonLampSol(ON);
	}

	return 0;
}

LONG CHouseKeeping::HMI_ToggleStartBtnSol(IPC_CServiceMessage &svMsg)
{
	if (m_stStartButtonSol.bHMI_Status)
	{
		SetStartButtonLampSol(OFF);
	}
	else
	{
		SetStartButtonLampSol(ON);
	}

	return 0;
}

LONG CHouseKeeping::HMI_ToggleRedSol(IPC_CServiceMessage &svMsg)
{
	if (m_stRedLight.bHMI_Status)
	{
		SetRedSol(OFF);
	}
	else
	{
		SetRedSol(ON);
	}

	return 0;
}

LONG CHouseKeeping::HMI_ToggleOrangeSol(IPC_CServiceMessage &svMsg)
{
	if (m_stOrangeLight.bHMI_Status)
	{
		SetOrangeSol(OFF);
	}
	else
	{
		SetOrangeSol(ON);
	}

	return 0;
}

LONG CHouseKeeping::HMI_ToggleGreenSol(IPC_CServiceMessage &svMsg)
{
	if (m_stGreenLight.bHMI_Status)
	{
		SetGreenSol(OFF);
	}
	else
	{
		SetGreenSol(ON);
	}

	return 0;
}

LONG CHouseKeeping::HMI_ToggleBuzzerSol(IPC_CServiceMessage &svMsg)
{
	if (m_stBuzzer.bHMI_Status)
	{
		SetBuzzerSol(OFF);
	}
	else
	{
		SetBuzzerSol(ON);
	}

	return 0;
}
// 20150204

LONG CHouseKeeping::HMI_ToggleACFFrontDoorLockSol(IPC_CServiceMessage &svMsg)
{
	if (m_stACFFrontDoorLockSol.bHMI_Status)
	{
		SetACFFrontDoorLockSol(OFF);
	}
	else
	{
		SetACFFrontDoorLockSol(ON);
	}

	return 0;
}

LONG CHouseKeeping::HMI_ToggleACFBackDoorLockSol(IPC_CServiceMessage &svMsg)
{
	if (m_stACFBackDoorLockSol.bHMI_Status)
	{
		SetACFBackDoorLockSol(OFF);
	}
	else
	{
		SetACFBackDoorLockSol(ON);
	}

	return 0;
}



#if 1 //20121009 pin detection spring test
LONG CHouseKeeping::HMI_testPinDetectSpringStop(IPC_CServiceMessage &svMsg)
{
	BOOL bValue;
	svMsg.GetMsg(sizeof(BOOL), &bValue);

	m_bPinTest = bValue;
	return 0;
}
#endif

// ------------------------------------------------------------------------
// Setting
// ------------------------------------------------------------------------
LONG CHouseKeeping::HMI_BackUpPackages(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	//CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("EcoreCtrl");

	CString szpassword = szEngPassword;

	if (HMIGetPassword("Please enter password", szpassword))
	{
		CreateDirectory("D:\\sw\\AC9000\\Backup", 0);
		CopyDir("D:\\sw\\AC9000\\Device", "D:\\sw\\AC9000\\Backup");
		CopyDir("D:\\sw\\AC9000\\MarkData", "D:\\sw\\AC9000\\Backup");
		CopyDir("D:\\sw\\AC9000\\Param", "D:\\sw\\AC9000\\Backup");
		CopyDir("D:\\sw\\AC9000\\Record", "D:\\sw\\AC9000\\Backup");
		
		// Release me later
		//pAppMod->PrintMachineInfo();
		//pCLitecCtrl->PrintEcoreInfomation();
		CopyDir("D:\\sw\\AC9000\\Data", "D:\\sw\\AC9000\\Backup");
	}

	BOOL bDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bDone);
	return 1;
}

LONG CHouseKeeping::HMI_ChangePassword(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	CString	szPassword = "";
	LONG	lResponse;

	lResponse = HMISelectionBox("CHANGE PASSWORD", "Please select user group", "Engineer", "Service Eng");

	if (lResponse == -1 || lResponse == 11)
	{
		HMIMessageBox(MSG_OK, "CHANGE PASSWORD", "Operation Abort!");
		return 0;
	}

	switch (lResponse)
	{
	case 0:
		szPassword = szEngPassword;
		break;

	case 1:
		szPassword = szSvrPassword;
		break;
	}

	if (HMIGetPassword("Please enter the current password", szPassword))
	{
		if (HMIAlphaKeys("Please enter the New Password", &szPassword, TRUE))
		{
			if (szPassword == "")
			{
				HMIMessageBox(MSG_OK, "CHANGE PASSWORD", "Password must contain characters. Please enter again!");
				if (HMIAlphaKeys("Please enter the New Password", &szPassword, TRUE))
				{
					if (szPassword == "")
					{
						HMIMessageBox(MSG_OK, "CHANGE PASSWORD", "Operation Terminated.");
						return 0;
					}
				}
				else
				{
					HMIMessageBox(MSG_OK, "CHANGE PASSWORD", "Operation Abort!");
				}
			}
			if (HMIGetPassword("Please confirm the New Password", szPassword))
			{
				DisplayMessage("Saving Machine file...");

				switch (lResponse)
				{
				case 0:
					szEngPassword = szPassword;
					pAppMod->m_smfMachine["HouseKeeping"]["Password"]["szEngPassword"] = szEngPassword;
					HMIMessageBox(MSG_OK, "CHANGE PASSWORD", "*** Engineer usergroup password changed ***");
					break;

				case 1:
					szSvrPassword = szPassword;
					pAppMod->m_smfMachine["HouseKeeping"]["Password"]["szSvrPassword"] = szSvrPassword;
					HMIMessageBox(MSG_OK, "CHANGE PASSWORD", "*** Service Engineer usergroup password changed ***");
					break;
				}

				pAppMod->m_smfMachine.Update(); //20150128
			}
			else
			{
				HMIMessageBox(MSG_OK, "CHANGE PASSWORD", "Operation Abort!");
			}
		}
		else
		{
			HMIMessageBox(MSG_OK, "CHANGE PASSWORD", "Operation Abort!");
		}
	}
	return 0;

}

LONG CHouseKeeping::HMI_LangSwitching(IPC_CServiceMessage &svMsg)
{
	BOOL bResult;

	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	bResult = pAppMod->HMILangSwitching();

	return 0;
}

LONG CHouseKeeping::HMI_SetAutoMode(IPC_CServiceMessage &svMsg)
{
	LONG lMode;
	svMsg.GetMsg(sizeof(LONG), &lMode);

	HMI_lAutoMode = lMode;
	AutoMode = (AUTOMODE)HMI_lAutoMode;

#if 1 //20130710
	if (AutoMode == BURN_IN)
	{
		CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
		for (int i = 0; i < pAppMod->GetPRStationList().GetSize(); i++)
		{
			pAppMod->GetPRStationList().GetAt(i)->SetPRSelected(FALSE);
		}
	}
	if (AutoMode == INDEXING)
	{
		CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
		if (pAppMod != NULL)
		{
			pAppMod->UnselectModuleForIndexingMode();
		}
	}
#endif
	return 0;
}

LONG CHouseKeeping::HMI_SetForceCalibHeight(IPC_CServiceMessage &svMsg)
{
	DOUBLE dHeight;
	svMsg.GetMsg(sizeof(DOUBLE), &dHeight);

	g_stForceCalibrator.dHeight = dHeight;

	return 0;
}

LONG CHouseKeeping::HMI_SetModuleOffset(IPC_CServiceMessage &svMsg)
{
	DOUBLE dHeight;
	svMsg.GetMsg(sizeof(DOUBLE), &dHeight);

	g_dModuleOffset = dHeight;

	return 0;
}

LONG CHouseKeeping::HMI_SelectShowForceMode(IPC_CServiceMessage &svMsg)
{
	// Reset me later
	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");
	//CACF2 *pCACF2 = (CACF2*)m_pModule->GetStation("ACF2");

	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	g_bShowForceInNewton = bMode;

	pCACF1->HMI_dCalibForceCtrl			= pCACF1->m_dCalibForceCtrl * (g_bShowForceInNewton * KGF_TO_NEWTON + (1 - (LONG)g_bShowForceInNewton));
	pCACF1->HMI_dFinalForceCtrl			= pCACF1->HMI_dFinalForceCtrl * (g_bShowForceInNewton * KGF_TO_NEWTON + (1 - (LONG)g_bShowForceInNewton));
	
	//pCACF2->HMI_dCalibForceCtrl			= pCACF2->m_dCalibForceCtrl * (g_bShowForceInNewton * KGF_TO_NEWTON + (1 - (LONG)g_bShowForceInNewton));
	//pCACF2->HMI_dFinalForceCtrl			= pCACF2->HMI_dFinalForceCtrl * (g_bShowForceInNewton * KGF_TO_NEWTON + (1 - (LONG)g_bShowForceInNewton));

	pCACF1->PrintCalibDataToHMI();
	//pCACF2->PrintCalibDataToHMI();
	return 0;
}

LONG CHouseKeeping::HMI_SetDetectCover(IPC_CServiceMessage &svMsg)
{
	ULONG ulMode;
	svMsg.GetMsg(sizeof(ULONG), &ulMode);

	m_ulDetectCover = ulMode;

	switch (m_ulDetectCover)
	{
	case 0: //slow
		m_bDetectCoverSlowMotion = TRUE;
		m_bDetectCoverStopOp = FALSE;
		break;
	case 1: //stop
		m_bDetectCoverSlowMotion = FALSE;
		m_bDetectCoverStopOp = TRUE;
		break;
	case 2: //disable
		m_bDetectCoverSlowMotion = FALSE;
		m_bDetectCoverStopOp = FALSE;
		break;
	}

	return 0;
}

LONG CHouseKeeping::HMI_SetDetectLightCurtainSnr(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	m_bDetectLightCurtainSnr = bMode;

	return 0;
}

LONG CHouseKeeping::HMI_SetDetectAirSupply(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	m_bDetectAirSupply = bMode;

	return 0;
}

LONG CHouseKeeping::HMI_SetSlowMotionIndexing(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	INT i = 0;
	INT j = 0;

	g_bSlowMotionIndexing = bMode;

	if (g_bSlowMotionIndexing)
	{
		SetMotionProfileMode(g_ulSlowMotionFactor);
	}
	else
	{
		SetMotionProfileMode(NORMAL_MOT_PROF);
	}

	return 0;
}

LONG CHouseKeeping::HMI_SetSlowMotionFactor(IPC_CServiceMessage &svMsg)
{

	ULONG ulFactor;
	svMsg.GetMsg(sizeof(ULONG), &ulFactor);

	if (ulFactor < MAX_NUM_OF_MOT_PROF)
	{
		g_ulSlowMotionFactor = ulFactor;

		if (g_bSlowMotionIndexing)
		{
			SetMotionProfileMode(g_ulSlowMotionFactor);
		}
	}

	return 0;
}

LONG CHouseKeeping::HMI_SetEnableTrigLighting(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	g_bEnableTrigLighting = bMode;

	return 0;
}

LONG CHouseKeeping::HMI_CheckUserGroup(IPC_CServiceMessage &svMsg)
{
	BOOL	bStatus = FALSE;

	if (
		(g_ulUserGroup == ENGINEER) ||
		(g_ulUserGroup == SERVICE) ||
		(g_ulUserGroup == DEVELOPER)
	   )
	{
		bStatus = TRUE;
	}
	else
	{
		HMIMessageBox(MSG_OK, "EDIT DEVICE INFORMATION", "Current User Login Group does not have right to edit Device Information!");
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bStatus);
	return 1;
}

// ------------------------------------------------------------------------
// Package Information
// ------------------------------------------------------------------------
LONG CHouseKeeping::HMI_SetGlassName(IPC_CServiceMessage &svMsg)
{
#if 1 //20150615
	LONG	lMsgLen = svMsg.GetMsgLen();
	UCHAR cLine[_MAX_PATH];
	UCHAR	*cName = &cLine[0];

	memset(&cLine[0], 0, _MAX_PATH);
	if (lMsgLen + 1 > _MAX_PATH)
	{
		CString szMsgTmp("");
		HMIMessageBox(MSG_OK, "WARNING", "Software Handling Error. Operation Abort!");
		szMsgTmp.Format("%s: Message Length Error", __FUNCTION__);
		DisplayMessage(szMsgTmp);
		return 0;
	}
	else if (lMsgLen > 0)
	{
		svMsg.GetMsg(cName, lMsgLen);
		g_stGlass.szName.Format("%s", cName);
	}
#else
	LONG	lMsgLen = svMsg.GetMsgLen();
	UCHAR	*cName;

	if (lMsgLen > 0)
	{
		cName = new UCHAR[lMsgLen];
		svMsg.GetMsg(cName, lMsgLen);
		g_stGlass.szName = (CString)cName;
	}
#endif
	return 0;
}

LONG CHouseKeeping::HMI_SetGlassLength(IPC_CServiceMessage &svMsg)
{

	DOUBLE dLength;
	svMsg.GetMsg(sizeof(DOUBLE), &dLength);

	g_stGlass.dLength = dLength;

	return 0;
}

LONG CHouseKeeping::HMI_SetGlassWidth(IPC_CServiceMessage &svMsg)
{
	DOUBLE dWidth;
	svMsg.GetMsg(sizeof(DOUBLE), &dWidth);

	g_stGlass.dWidth = dWidth;

	return 0;
}

LONG CHouseKeeping::HMI_SetReflectorThickness(IPC_CServiceMessage &svMsg)
{
	DOUBLE dThickness;
	svMsg.GetMsg(sizeof(DOUBLE), &dThickness);

	g_stGlass.dThickness1 = dThickness;

	return 0;
}

LONG CHouseKeeping::HMI_SetUpperGlassThickness(IPC_CServiceMessage &svMsg)
{
	DOUBLE dThickness;
	svMsg.GetMsg(sizeof(DOUBLE), &dThickness);

	g_stGlass.dThickness2 = dThickness;

	return 0;
}

LONG CHouseKeeping::HMI_SetLowerGlassThickness(IPC_CServiceMessage &svMsg)
{
	DOUBLE dThickness;
	svMsg.GetMsg(sizeof(DOUBLE), &dThickness);

	g_stGlass.dThickness3 = dThickness;

	return 0;
}

LONG CHouseKeeping::HMI_SetPolarizerThickness(IPC_CServiceMessage &svMsg)
{
	DOUBLE dThickness;
	svMsg.GetMsg(sizeof(DOUBLE), &dThickness);

	g_stGlass.dThickness4 = dThickness;

	return 0;
}

LONG CHouseKeeping::HMI_SetGlassItoWidth(IPC_CServiceMessage &svMsg)
{
	DOUBLE dWidth = 0.0;

	svMsg.GetMsg(sizeof(DOUBLE), &dWidth);

	g_stGlass.dItoWidth = dWidth;

	return 0;
}

////////////////////////////////////////////////////////
// ACF Info. Related
////////////////////////////////////////////////////////
LONG CHouseKeeping::HMI_SetACFName(IPC_CServiceMessage &svMsg)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

#if 1 //20150615
	LONG	lMsgLen = svMsg.GetMsgLen();
	UCHAR cLine[_MAX_PATH];
	UCHAR	*cName = &cLine[0];

	memset(&cLine[0], 0, _MAX_PATH);

	if (lMsgLen + 1 > _MAX_PATH)
	{
		CString szMsgTmp("");
		HMIMessageBox(MSG_OK, "WARNING", "Software Handling Error. Operation Abort!");
		szMsgTmp.Format("%s: Message Length Error", __FUNCTION__);
		DisplayMessage(szMsgTmp);
		return 0;
	}
	else if (lMsgLen > 0)
	{
		svMsg.GetMsg(cName, lMsgLen);

		for (INT i = 0; i < MAX_NUM_OF_ACF; i++)
		{
			pCACF1WH->m_stACF[i].szName.Format("%s", cName);
			pCACF2WH->m_stACF[i].szName.Format("%s", cName);
		}

		HMI_szACFName = pCACF1WH->m_stACF[ACF1].szName;
	}
#else
	LONG	lMsgLen = svMsg.GetMsgLen();
	UCHAR	*cName;

	if (lMsgLen > 0)
	{
		cName = new UCHAR[lMsgLen];
		svMsg.GetMsg(cName, lMsgLen);

		for (INT i = 0; i < MAX_NUM_OF_ACF; i++)
		{
			pCACF1WH->m_stACF[i].szName = (CString)cName;
			pCACF2WH->m_stACF[i].szName = (CString)cName;
		}

		HMI_szACFName = pCACF1WH->m_stACF[ACF1].szName;
	}
#endif
	return 0;
}

LONG CHouseKeeping::HMI_SetACFWidth(IPC_CServiceMessage &svMsg)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	DOUBLE dWidth;
	svMsg.GetMsg(sizeof(DOUBLE), &dWidth);

#ifdef ACF_POSN_CALIB //20120829
	DOUBLE dWidthDiff;

	dWidthDiff = dWidth - pCACF1WH->m_stACF[ACF1].dWidth;
	//shift y-offset

	for (INT i = ACF1; i < MAX_NUM_OF_ACF; i++)
	{
		pCACF1WH->m_lBondOffsetY[i] -= DoubleToLong(dWidthDiff / 2.0 * 1000.0);
		pCACF2WH->m_lBondOffsetY[i] -= DoubleToLong(dWidthDiff / 2.0 * 1000.0);
	}
	pCACF1WH->HMI_lBondOffsetY = pCACF1WH->m_lBondOffsetY[ACF1];
	pCACF2WH->HMI_lBondOffsetY = pCACF2WH->m_lBondOffsetY[ACF1];
#endif	
	
	for (INT i = ACF1; i < MAX_NUM_OF_ACF; i++)
	{
		pCACF1WH->m_stACF[i].dWidth = dWidth;
		pCACF2WH->m_stACF[i].dWidth = dWidth;
	}

	HMI_dACFWidth = pCACF1WH->m_stACF[ACF1].dWidth;

	return 0;
}

LONG CHouseKeeping::HMI_SetACFLength(IPC_CServiceMessage &svMsg)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	DOUBLE dLength;
	svMsg.GetMsg(sizeof(DOUBLE), &dLength);

#ifdef ACF_POSN_CALIB //20120829
	//acf length increase, m_lLoadXOffset decrease
	DOUBLE dLengthDiff;

	dLengthDiff = dLength - pCACF1WH->m_stACF[ACF1].dLength;

	//shift xoffset
	//pCACF1WH->m_lLoadXOffset -= DoubleToLong(dLengthDiff / 2.0 * 1000.0);
	//pCACF2WH->m_lLoadXOffset -= DoubleToLong(dLengthDiff / 2.0 * 1000.0);

#endif	

	for (INT i = ACF1; i < MAX_NUM_OF_ACF; i++)
	{
		pCACF1WH->m_stACF[i].dLength = dLength;
		pCACF2WH->m_stACF[i].dLength = dLength;
	}

	HMI_dACFLength = pCACF1WH->m_stACF[ACF1].dLength;

	return 0;
}

LONG CHouseKeeping::HMI_SetACFWasteCollectionLimit(IPC_CServiceMessage &svMsg) //20130709
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	LONG lValue = 0;
	svMsg.GetMsg(sizeof(LONG), &lValue);

	m_lACFBondedAlarmLimit = lValue;
	m_lACFBondedAlarmCount = m_lACFBondedAlarmLimit;

	pAppMod->m_smfMachine["HouseKeeping"]["Settings"]["m_lACFBondedAlarmCount"] = m_lACFBondedAlarmCount;
	//if (m_lACFBondedAlarmCount > m_lACFBondedAlarmLimit)
	//{
	//	m_lACFBondedAlarmCount = m_lACFBondedAlarmLimit;
	//}
	return 0;
}

#ifdef EXCEL_OFFLINE_SETUP
INT CHouseKeeping::OfflineSetupCheckInputValid(OFFLINE_INPUT_ITEM emItem, DOUBLE dValue)
{
	if (emItem <= OFFLINE_MIN || emItem >= OFFLINE_MAX)
	{
		return gnAMS_NOTOK;
	}

	OFFLINE_INPUT_DATA stInputData = gOfflineInputData[emItem];
	
	if (dValue < stInputData.dMin || dValue > stInputData.dMax)
	{
		CString szMsg(_T(""));
		if (stInputData.pName != NULL)
		{
			szMsg.Format("%s Value Invalid. Please Check Input File. Input:%.2f Range(%.2f, %.2f)", stInputData.pName, dValue, stInputData.dMin, stInputData.dMax);
			HMIMessageBox(MSG_OK, "OFFLINE SETUP FILE", szMsg); //translate
		}
		return gnAMS_NOTOK;
	}
	return gnAMS_OK;
}
#endif

////////////////////////////////////////////////////////
// LSI Type Info. Related
////////////////////////////////////////////////////////
LONG CHouseKeeping::HMI_SetLSIType1Name(IPC_CServiceMessage &svMsg)
{
#if 1 //20150615
	LONG	lMsgLen = svMsg.GetMsgLen();
	UCHAR cLine[_MAX_PATH];
	UCHAR	*cName = &cLine[0];

	memset(&cLine[0], 0, _MAX_PATH);

	if (lMsgLen + 1 > _MAX_PATH)
	{
		CString szMsgTmp("");
		HMIMessageBox(MSG_OK, "WARNING", "Software Handling Error. Operation Abort!");
		szMsgTmp.Format("%s: Message Length Error", __FUNCTION__);
		DisplayMessage(szMsgTmp);
		return 0;
	}
	else if (lMsgLen > 0)
	{
		svMsg.GetMsg(cName, lMsgLen);
		g_stLSIType[LSI_TYPE1].szName.Format("%s", cName);
	}
#else
	LONG	lMsgLen = svMsg.GetMsgLen();
	UCHAR	*cName;

	if (lMsgLen > 0)
	{
		cName = new UCHAR[lMsgLen];
		svMsg.GetMsg(cName, lMsgLen);
		g_stLSIType[LSI_TYPE1].szName = (CString)cName;
	}
#endif
	return 0;
}

LONG CHouseKeeping::HMI_SetLSIType1Length(IPC_CServiceMessage &svMsg)
{
	DOUBLE dLength;
	svMsg.GetMsg(sizeof(DOUBLE), &dLength);

	g_stLSIType[LSI_TYPE1].dLength = dLength;

	return 0;
}

LONG CHouseKeeping::HMI_SetLSIType1Width(IPC_CServiceMessage &svMsg)
{
	DOUBLE dWidth;
	svMsg.GetMsg(sizeof(DOUBLE), &dWidth);

	g_stLSIType[LSI_TYPE1].dWidth = dWidth;

	return 0;
}

LONG CHouseKeeping::HMI_SetLSIType1Thickness(IPC_CServiceMessage &svMsg)
{
	DOUBLE dThickness;
	svMsg.GetMsg(sizeof(DOUBLE), &dThickness);

	g_stLSIType[LSI_TYPE1].dThickness = dThickness;

	return 0;
}

///////////////////////////////////////
// LSI On Glass Info. Related
///////////////////////////////////////


////////////////////////////////////////////////////////
// MB Press Seq.
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
// Rework Mode
////////////////////////////////////////////////////////

LONG CHouseKeeping::HMI_SelectedReworkMode(IPC_CServiceMessage &svMsg)
{
	BOOL bResult;
	svMsg.GetMsg(sizeof(BOOL), &bResult);

	g_bSelectedReworkMode = bResult;

	return 0;
}

////////////////////////////////////////////////////////
// Glass Input Mode
//////////////////////////////////////////////////////// 20120412
LONG CHouseKeeping::HMI_SetGlassInputMode(IPC_CServiceMessage &svMsg)
{
	LONG lMode;
	svMsg.GetMsg(sizeof(LONG), &lMode);

	switch ((GLASS_INPUT_MODE) lMode)
	{
	case GLASS_1_INPUT:
		g_lGlassInputMode = GLASS_1_INPUT;
		break;
	//case GLASS_2_INPUT:
	//	g_lGlassInputMode = GLASS_2_INPUT;
	//	break;
	//case GLASS_1_2_INPUT:
	default:
		g_lGlassInputMode = GLASS_1_INPUT;
		break;
	}
	return 0;
}

LONG CHouseKeeping::HMI_LightAndBuzzerInIdle(IPC_CServiceMessage &svMsg) //20130115
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	m_bErrorExist = FALSE;
	m_bAlertExist = FALSE;
	//m_szLastError = "";	// 20140814 Yip

	if (pAppMod->IsInitNuDrive())
	{
		SetRedSol(OFF);
		SetOrangeSol(ON);
		SetGreenSol(OFF);
		SetBuzzerSol(OFF);
	}
	if (g_bIsVisionNTInited)
	{
		PRS_DisplayVideo(&pruACF_WHCalib ); //20150522
	}
	return 0;
}
//p20120918
////////////////////////////////////////////////////////
// Traditional Chinese Switching
////////////////////////////////////////////////////////
LONG CHouseKeeping::HMILangTraditionalChinese(IPC_CServiceMessage &svMsg)
{
	BOOL bResult;

	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	bResult = pAppMod->HMILangTraditionalChinese();

	return 0;
}
//p20120918:end

LONG CHouseKeeping::HMI_SetTrayReadySwitchEnable(IPC_CServiceMessage &svMsg)	// 20140630 Yip
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	m_bTrayReadySwitchEnable = bMode;

	return 0;
}

////////////////////////////////////////////////////////
// Load/Save
////////////////////////////////////////////////////////
// Saving / loading parameters
LONG CHouseKeeping::IPC_SaveMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	pAppMod->m_smfMachine[GetStnName()]["Password"]["szEngPassword"]	= szEngPassword;
	pAppMod->m_smfMachine[GetStnName()]["Password"]["szSvrPassword"]	= szSvrPassword;

	pAppMod->m_smfMachine[GetStnName()]["Tools"]["g_stForceCalibrator.dHeight"]		= g_stForceCalibrator.dHeight;

	pAppMod->m_smfMachine[GetStnName()]["MechanicalDesignError"]["g_dModuleOffset"]	= g_dModuleOffset;

	pAppMod->m_smfMachine[GetStnName()]["Settings"]["g_bShowForceInNewton"]			= g_bShowForceInNewton;

	pAppMod->m_smfMachine[GetStnName()]["Settings"]["m_ulDetectCover"]				= m_ulDetectCover;
	//pAppMod->m_smfMachine[GetStnName()]["Settings"]["m_bDetectAirSupply"]			= m_bDetectAirSupply;
	pAppMod->m_smfMachine[GetStnName()]["Settings"]["m_bDetectLightCurtainSnr"]		= m_bDetectLightCurtainSnr;

	pAppMod->m_smfMachine[GetStnName()]["Settings"]["g_lGlassInputMode"]			= g_lGlassInputMode;
	pAppMod->m_smfMachine[GetStnName()]["Settings"]["m_lACFBondedAlarmLimit"]		= m_lACFBondedAlarmLimit; //20130709

	pAppMod->m_smfMachine["HouseKeeping"]["Settings"]["m_bTrayReadySwitchEnable"]	= m_bTrayReadySwitchEnable;	// 20140630 Yip

	return CAC9000Stn::IPC_SaveMachineParam();
}

LONG CHouseKeeping::IPC_LoadMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	//CPB1 *pCPB1 = (CPB1*)m_pModule->GetStation("PB1");
	//CPB2 *pCPB2 = (CPB2*)m_pModule->GetStation("PB2");

	szEngPassword				= pAppMod->m_smfMachine[GetStnName()]["Password"]["szEngPassword"];
	szSvrPassword				= pAppMod->m_smfMachine[GetStnName()]["Password"]["szSvrPassword"];

	g_stForceCalibrator.dHeight = pAppMod->m_smfMachine[GetStnName()]["Tools"]["g_stForceCalibrator.dHeight"];
	
	g_dModuleOffset				= pAppMod->m_smfMachine[GetStnName()]["MechanicalDesignError"]["g_dModuleOffset"];

	g_bShowForceInNewton		= (BOOL)(LONG)pAppMod->m_smfMachine[GetStnName()]["Settings"]["g_bShowForceInNewton"];
	
	m_ulDetectCover				= (LONG)pAppMod->m_smfMachine[GetStnName()]["Settings"]["m_ulDetectCover"];
	//m_bDetectAirSupply			= (BOOL)(LONG)pAppMod->m_smfMachine[GetStnName()]["Settings"]["m_bDetectAirSupply"];
	m_bDetectLightCurtainSnr	= (BOOL)(LONG)pAppMod->m_smfMachine[GetStnName()]["Settings"]["m_bDetectLightCurtainSnr"];

	g_lGlassInputMode			= pAppMod->m_smfMachine[GetStnName()]["Settings"]["g_lGlassInputMode"];

	m_lACFBondedAlarmCount		= pAppMod->m_smfMachine[GetStnName()]["Settings"]["m_lACFBondedAlarmCount"];	//20130709
	m_lACFBondedAlarmLimit		= pAppMod->m_smfMachine[GetStnName()]["Settings"]["m_lACFBondedAlarmLimit"];
	
	if (m_lACFBondedAlarmLimit <= 0)
	{
		m_lACFBondedAlarmLimit = 5000;
	}
	if (m_lACFBondedAlarmCount > m_lACFBondedAlarmLimit || m_lACFBondedAlarmCount <= 0)
	{
		m_lACFBondedAlarmCount = m_lACFBondedAlarmLimit;
	}

	m_bTrayReadySwitchEnable	= (BOOL)(LONG)pAppMod->m_smfMachine["HouseKeeping"]["Settings"]["m_bTrayReadySwitchEnable"];	// 20140630 Yip

	SetMotionProfileMode(g_ulSlowMotionFactor);

	//stuff
	switch (m_ulDetectCover)
	{
	case 0:
		m_bDetectCoverSlowMotion = TRUE;
		m_bDetectCoverStopOp = FALSE;
		break;
	case 1:
		m_bDetectCoverSlowMotion = FALSE;
		m_bDetectCoverStopOp = TRUE;
		break;
	case 2:
		m_bDetectCoverSlowMotion = FALSE;
		m_bDetectCoverStopOp = FALSE;
		break;
	}

	if (szEngPassword == "")
	{
		szEngPassword = "AC9000";	//should be cog902
	}

	if (szSvrPassword == "")
	{
		szSvrPassword = "a5s2m2";
	}

	return CAC9000Stn::IPC_LoadMachineParam();
}

LONG CHouseKeeping::IPC_SaveDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	//CINWH *pCINWH = (CINWH*)m_pModule->GetStation("INWH");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	//CMB1WH *pCMB1WH	= (CMB1WH*)m_pModule->GetStation("MB1WH");
	//CMB2WH	*pCMB2WH	= (CMB2WH*)m_pModule->GetStation("MB2WH");
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1");

	CString str;

	pAppMod->m_smfDevice["Package"]["g_stGlass.szName"]			= g_stGlass.szName;

	pAppMod->m_smfDevice["Package"]["g_stGlass.dLength"]		= g_stGlass.dLength;
	pAppMod->m_smfDevice["Package"]["g_stGlass.dWidth"]			= g_stGlass.dWidth;
	pAppMod->m_smfDevice["Package"]["g_stGlass.dThickness1"]	= g_stGlass.dThickness1;
	pAppMod->m_smfDevice["Package"]["g_stGlass.dThickness2"]	= g_stGlass.dThickness2;
	pAppMod->m_smfDevice["Package"]["g_stGlass.dThickness3"]	= g_stGlass.dThickness3;
	pAppMod->m_smfDevice["Package"]["g_stGlass.dThickness4"]	= g_stGlass.dThickness4;

	pAppMod->m_smfDevice["Package"]["g_stGlass.dItoWidth"]		= g_stGlass.dItoWidth;

	pAppMod->m_smfDevice["Lighting"]["g_bEnableTrigLighting"]	= g_bEnableTrigLighting; //20141201

	pAppMod->m_smfDevice["Package"]["m_stACFUL"]["bAlignPtSet"]						= pCTA1->m_stACFUL.bAlignPtSet;
	pAppMod->m_smfDevice["Package"]["m_stACFUL"]["bValid"]							= pCTA1->m_stACFUL.bValid;
	pAppMod->m_smfDevice["Package"]["m_stACFUL"]["dmtrAlignPt1.x"]					= pCTA1->m_stACFUL.dmtrAlignPt1.x;
	pAppMod->m_smfDevice["Package"]["m_stACFUL"]["dmtrAlignPt1.y"]					= pCTA1->m_stACFUL.dmtrAlignPt1.y;
	pAppMod->m_smfDevice["Package"]["m_stACFUL"]["dmtrAlignPt2.x"]					= pCTA1->m_stACFUL.dmtrAlignPt2.x;
	pAppMod->m_smfDevice["Package"]["m_stACFUL"]["dmtrAlignPt2.y"]					= pCTA1->m_stACFUL.dmtrAlignPt2.y;

	// ACF Info. Related
	pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF1WH"]["bAlignPtSet"]						= pCACF1WH->m_stACF[ACF1].bAlignPtSet;
	pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF1WH"]["bValid"]							= pCACF1WH->m_stACF[ACF1].bValid;
	pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF1WH"]["dmtrAlignPt1.x"]					= pCACF1WH->m_stACF[ACF1].dmtrAlignPt1.x;
	pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF1WH"]["dmtrAlignPt1.y"]					= pCACF1WH->m_stACF[ACF1].dmtrAlignPt1.y;
	pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF1WH"]["dmtrAlignPt2.x"]					= pCACF1WH->m_stACF[ACF1].dmtrAlignPt2.x;
	pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF1WH"]["dmtrAlignPt2.y"]					= pCACF1WH->m_stACF[ACF1].dmtrAlignPt2.y;

	pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF2WH"]["bAlignPtSet"]						= pCACF2WH->m_stACF[ACF1].bAlignPtSet;
	pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF2WH"]["bValid"]							= pCACF2WH->m_stACF[ACF1].bValid;
	pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF2WH"]["dmtrAlignPt1.x"]					= pCACF2WH->m_stACF[ACF1].dmtrAlignPt1.x;
	pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF2WH"]["dmtrAlignPt1.y"]					= pCACF2WH->m_stACF[ACF1].dmtrAlignPt1.y;
	pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF2WH"]["dmtrAlignPt2.x"]					= pCACF2WH->m_stACF[ACF1].dmtrAlignPt2.x;
	pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF2WH"]["dmtrAlignPt2.y"]					= pCACF2WH->m_stACF[ACF1].dmtrAlignPt2.y;

	pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF1WH"]["szName"]					= pCACF1WH->m_stACF[ACF1].szName;
	pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF1WH"]["bEnable"]					= pCACF1WH->m_stACF[ACF1].bEnable;
	pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF1WH"]["dLength"]					= pCACF1WH->m_stACF[ACF1].dLength;
	pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF1WH"]["dWidth"]					= pCACF1WH->m_stACF[ACF1].dWidth;

	pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF1WH"]["stBondOffset.x"]			= pCACF1WH->m_stACF[ACF1].stBondOffset.x;
	pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF1WH"]["stBondOffset.y"]			= pCACF1WH->m_stACF[ACF1].stBondOffset.y;
	pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF1WH"]["stBondOffset.Theta"]		= pCACF1WH->m_stACF[ACF1].stBondOffset.Theta;

	pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF2WH"]["szName"]					= pCACF2WH->m_stACF[ACF1].szName;
	pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF2WH"]["bEnable"]					= pCACF2WH->m_stACF[ACF1].bEnable;
	pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF2WH"]["dLength"]					= pCACF2WH->m_stACF[ACF1].dLength;
	pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF2WH"]["dWidth"]					= pCACF2WH->m_stACF[ACF1].dWidth;

	pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF2WH"]["stBondOffset.x"]			= pCACF2WH->m_stACF[ACF1].stBondOffset.x;
	pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF2WH"]["stBondOffset.y"]			= pCACF2WH->m_stACF[ACF1].stBondOffset.y;
	pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF2WH"]["stBondOffset.Theta"]		= pCACF2WH->m_stACF[ACF1].stBondOffset.Theta;

	// LSI Info. Related
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["szName"]				= g_stLSIType[LSI_TYPE1].szName;

	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["dLength"]				= g_stLSIType[LSI_TYPE1].dLength;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["dWidth"]				= g_stLSIType[LSI_TYPE1].dWidth;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["dThickness"]			= g_stLSIType[LSI_TYPE1].dThickness;

	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB1"]["dmtrAlignPt1.x"]		= g_stLSIType[LSI_TYPE1].dmtrAlignPt1[PREBOND_1].x;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB1"]["dmtrAlignPt1.y"]		= g_stLSIType[LSI_TYPE1].dmtrAlignPt1[PREBOND_1].y;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB1"]["dmtrAlignPt2.x"]		= g_stLSIType[LSI_TYPE1].dmtrAlignPt2[PREBOND_1].x;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB1"]["dmtrAlignPt2.y"]		= g_stLSIType[LSI_TYPE1].dmtrAlignPt2[PREBOND_1].y;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB1"]["bAlignPtSet"]			= g_stLSIType[LSI_TYPE1].bAlignPtSet[PREBOND_1];

	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB2"]["dmtrAlignPt1.x"]		= g_stLSIType[LSI_TYPE1].dmtrAlignPt1[PREBOND_2].x;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB2"]["dmtrAlignPt1.y"]		= g_stLSIType[LSI_TYPE1].dmtrAlignPt1[PREBOND_2].y;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB2"]["dmtrAlignPt2.x"]		= g_stLSIType[LSI_TYPE1].dmtrAlignPt2[PREBOND_2].x;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB2"]["dmtrAlignPt2.y"]		= g_stLSIType[LSI_TYPE1].dmtrAlignPt2[PREBOND_2].y;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB2"]["bAlignPtSet"]			= g_stLSIType[LSI_TYPE1].bAlignPtSet[PREBOND_2];

	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["dmtrAlignPickPt1.x"]	= g_stLSIType[LSI_TYPE1].dmtrAlignPickPt1.x;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["dmtrAlignPickPt1.y"]	= g_stLSIType[LSI_TYPE1].dmtrAlignPickPt1.y;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["dmtrAlignPickPt2.x"]	= g_stLSIType[LSI_TYPE1].dmtrAlignPickPt2.x;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["dmtrAlignPickPt2.y"]	= g_stLSIType[LSI_TYPE1].dmtrAlignPickPt2.y;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["bAlignPickPtSet"]		= g_stLSIType[LSI_TYPE1].bAlignPickPtSet;

	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["stPickOffset.x"]		= g_stLSIType[LSI_TYPE1].stPickOffset.x;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["stPickOffset.y"]		= g_stLSIType[LSI_TYPE1].stPickOffset.y;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["stPickOffset.Theta"]	= g_stLSIType[LSI_TYPE1].stPickOffset.Theta;

	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH1"]["dmtrAlignLSIPt1.x"]		= g_stLSIType[LSI_TYPE1].dmtrAlignLSIPt1[SHUTTLE_1].x;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH1"]["dmtrAlignLSIPt1.y"]		= g_stLSIType[LSI_TYPE1].dmtrAlignLSIPt1[SHUTTLE_1].y;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH1"]["dmtrAlignLSIPt2.x"]		= g_stLSIType[LSI_TYPE1].dmtrAlignLSIPt2[SHUTTLE_1].x;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH1"]["dmtrAlignLSIPt2.y"]		= g_stLSIType[LSI_TYPE1].dmtrAlignLSIPt2[SHUTTLE_1].y;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH1"]["bAlignLSIPtSet"]		= g_stLSIType[LSI_TYPE1].bAlignLSIPtSet[SHUTTLE_1];

	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH2"]["dmtrAlignLSIPt1.x"]		= g_stLSIType[LSI_TYPE1].dmtrAlignLSIPt1[SHUTTLE_2].x;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH2"]["dmtrAlignLSIPt1.y"]		= g_stLSIType[LSI_TYPE1].dmtrAlignLSIPt1[SHUTTLE_2].y;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH2"]["dmtrAlignLSIPt2.x"]		= g_stLSIType[LSI_TYPE1].dmtrAlignLSIPt2[SHUTTLE_2].x;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH2"]["dmtrAlignLSIPt2.y"]		= g_stLSIType[LSI_TYPE1].dmtrAlignLSIPt2[SHUTTLE_2].y;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH2"]["bAlignLSIPtSet"]		= g_stLSIType[LSI_TYPE1].bAlignLSIPtSet[SHUTTLE_2];

	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH1"]["stPreBondPickOffset.x"]		= g_stLSIType[LSI_TYPE1].stPreBondPickOffset[SHUTTLE_1].x;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH1"]["stPreBondPickOffset.y"]		= g_stLSIType[LSI_TYPE1].stPreBondPickOffset[SHUTTLE_1].y;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH1"]["stPreBondPickOffset.Theta"]	= g_stLSIType[LSI_TYPE1].stPreBondPickOffset[SHUTTLE_1].Theta;

	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH2"]["stPreBondPickOffset.x"]		= g_stLSIType[LSI_TYPE1].stPreBondPickOffset[SHUTTLE_2].x;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH2"]["stPreBondPickOffset.y"]		= g_stLSIType[LSI_TYPE1].stPreBondPickOffset[SHUTTLE_2].y;
	pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH2"]["stPreBondPickOffset.Theta"]	= g_stLSIType[LSI_TYPE1].stPreBondPickOffset[SHUTTLE_2].Theta;

	// LSI Info. On Glass Related
	pAppMod->m_smfDevice["Package"]["g_stLSI"]["LSI1"]["bEnable"]							= g_stLSI[LSI1].bEnable;
	pAppMod->m_smfDevice["Package"]["g_stLSI"]["LSI1"]["lLSIType"]							= g_stLSI[LSI1].lLSIType;

	pAppMod->m_smfDevice["Package"]["g_stLSI"]["LSI1"]["PB1"]["dmtrAlignPt1.x"]				= g_stLSI[LSI1].dmtrAlignPt1[PREBOND_1].x;
	pAppMod->m_smfDevice["Package"]["g_stLSI"]["LSI1"]["PB1"]["dmtrAlignPt1.y"]				= g_stLSI[LSI1].dmtrAlignPt1[PREBOND_1].y;
	pAppMod->m_smfDevice["Package"]["g_stLSI"]["LSI1"]["PB1"]["dmtrAlignPt2.x"]				= g_stLSI[LSI1].dmtrAlignPt2[PREBOND_1].x;
	pAppMod->m_smfDevice["Package"]["g_stLSI"]["LSI1"]["PB1"]["dmtrAlignPt2.y"]				= g_stLSI[LSI1].dmtrAlignPt2[PREBOND_1].y;
	pAppMod->m_smfDevice["Package"]["g_stLSI"]["LSI1"]["PB1"]["bAlignPtSet"]				= g_stLSI[LSI1].bAlignPtSet[PREBOND_1];

	pAppMod->m_smfDevice["Package"]["g_stLSI"]["LSI1"]["PB2"]["dmtrAlignPt1.x"]				= g_stLSI[LSI1].dmtrAlignPt1[PREBOND_2].x;
	pAppMod->m_smfDevice["Package"]["g_stLSI"]["LSI1"]["PB2"]["dmtrAlignPt1.y"]				= g_stLSI[LSI1].dmtrAlignPt1[PREBOND_2].y;
	pAppMod->m_smfDevice["Package"]["g_stLSI"]["LSI1"]["PB2"]["dmtrAlignPt2.x"]				= g_stLSI[LSI1].dmtrAlignPt2[PREBOND_2].x;
	pAppMod->m_smfDevice["Package"]["g_stLSI"]["LSI1"]["PB2"]["dmtrAlignPt2.y"]				= g_stLSI[LSI1].dmtrAlignPt2[PREBOND_2].y;
	pAppMod->m_smfDevice["Package"]["g_stLSI"]["LSI1"]["PB2"]["bAlignPtSet"]				= g_stLSI[LSI1].bAlignPtSet[PREBOND_2];

	CString szName1;
	CString szName2;

	//Setting
	pAppMod->m_smfDevice[GetStnName()]["Settings"]["g_bSafeIndexing"]		= g_bSafeIndexing;
	pAppMod->m_smfDevice[GetStnName()]["Settings"]["g_bSlowMotionIndexing"]	= g_bSlowMotionIndexing;
	pAppMod->m_smfDevice[GetStnName()]["Settings"]["g_ulSlowMotionFactor"]	= g_ulSlowMotionFactor;
	
	return CAC9000Stn::IPC_SaveDeviceParam();
}

LONG CHouseKeeping::IPC_LoadDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	//CINWH *pCINWH = (CINWH*)m_pModule->GetStation("INWH");
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	//CMB1WH *pCMB1WH	= (CMB1WH*)m_pModule->GetStation("MB1WH");
	//CMB2WH	*pCMB2WH	= (CMB2WH*)m_pModule->GetStation("MB2WH");
	//CPB1 *pCPB1 = (CPB1*)m_pModule->GetStation("PB1");
	//CPB2 *pCPB2 = (CPB2*)m_pModule->GetStation("PB2");
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1");

	CString str;

	g_stGlass.szName		= pAppMod->m_smfDevice["Package"]["g_stGlass.szName"];

	g_stGlass.dLength		= pAppMod->m_smfDevice["Package"]["g_stGlass.dLength"];
	g_stGlass.dWidth		= pAppMod->m_smfDevice["Package"]["g_stGlass.dWidth"];
	g_stGlass.dThickness1	= pAppMod->m_smfDevice["Package"]["g_stGlass.dThickness1"];
	g_stGlass.dThickness2	= pAppMod->m_smfDevice["Package"]["g_stGlass.dThickness2"];
	g_stGlass.dThickness3	= pAppMod->m_smfDevice["Package"]["g_stGlass.dThickness3"];
	g_stGlass.dThickness4	= pAppMod->m_smfDevice["Package"]["g_stGlass.dThickness4"];

	g_stGlass.dItoWidth		= pAppMod->m_smfDevice["Package"]["g_stGlass.dItoWidth"];

	g_bEnableTrigLighting	= (BOOL)(LONG)pAppMod->m_smfDevice["Lighting"]["g_bEnableTrigLighting"];  //20141201

	pCTA1->m_stACFUL.bAlignPtSet						= (BOOL)(LONG)pAppMod->m_smfDevice["Package"]["m_stACFUL"]["bAlignPtSet"];
	pCTA1->m_stACFUL.bValid							= (BOOL)(LONG)pAppMod->m_smfDevice["Package"]["m_stACFUL"]["bValid"];
	pCTA1->m_stACFUL.dmtrAlignPt1.x					= pAppMod->m_smfDevice["Package"]["m_stACFUL"]["dmtrAlignPt1.x"];
	pCTA1->m_stACFUL.dmtrAlignPt1.y					= pAppMod->m_smfDevice["Package"]["m_stACFUL"]["dmtrAlignPt1.y"];
	pCTA1->m_stACFUL.dmtrAlignPt2.x					= pAppMod->m_smfDevice["Package"]["m_stACFUL"]["dmtrAlignPt2.x"];
	pCTA1->m_stACFUL.dmtrAlignPt2.y					= pAppMod->m_smfDevice["Package"]["m_stACFUL"]["dmtrAlignPt2.y"];

	// ACF Info. Related
	pCACF1WH->m_stACF[ACF1].bAlignPtSet						= (BOOL)(LONG)pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF1WH"]["bAlignPtSet"];
	pCACF1WH->m_stACF[ACF1].bValid							= (BOOL)(LONG)pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF1WH"]["bValid"];
	pCACF1WH->m_stACF[ACF1].dmtrAlignPt1.x					= pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF1WH"]["dmtrAlignPt1.x"];
	pCACF1WH->m_stACF[ACF1].dmtrAlignPt1.y					= pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF1WH"]["dmtrAlignPt1.y"];
	pCACF1WH->m_stACF[ACF1].dmtrAlignPt2.x					= pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF1WH"]["dmtrAlignPt2.x"];
	pCACF1WH->m_stACF[ACF1].dmtrAlignPt2.y					= pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF1WH"]["dmtrAlignPt2.y"];

	pCACF2WH->m_stACF[ACF1].bAlignPtSet						= (BOOL)(LONG)pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF2WH"]["bAlignPtSet"];
	pCACF2WH->m_stACF[ACF1].bValid							= (BOOL)(LONG)pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF2WH"]["bValid"];
	pCACF2WH->m_stACF[ACF1].dmtrAlignPt1.x					= pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF2WH"]["dmtrAlignPt1.x"];
	pCACF2WH->m_stACF[ACF1].dmtrAlignPt1.y					= pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF2WH"]["dmtrAlignPt1.y"];
	pCACF2WH->m_stACF[ACF1].dmtrAlignPt2.x					= pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF2WH"]["dmtrAlignPt2.x"];
	pCACF2WH->m_stACF[ACF1].dmtrAlignPt2.y					= pAppMod->m_smfDevice["Package"]["m_stACF"][ACF1]["ACF2WH"]["dmtrAlignPt2.y"];

	pCACF1WH->m_stACF[ACF1].szName				= pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF1WH"]["szName"];
	pCACF1WH->m_stACF[ACF1].bEnable				= (BOOL)(LONG)pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF1WH"]["bEnable"];
	pCACF1WH->m_stACF[ACF1].dLength				= pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF1WH"]["dLength"];
	pCACF1WH->m_stACF[ACF1].dWidth				= pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF1WH"]["dWidth"];

	pCACF1WH->m_stACF[ACF1].stBondOffset.x		= pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF1WH"]["stBondOffset.x"];
	pCACF1WH->m_stACF[ACF1].stBondOffset.y		= pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF1WH"]["stBondOffset.y"];
	pCACF1WH->m_stACF[ACF1].stBondOffset.Theta	= pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF1WH"]["stBondOffset.Theta"];

	pCACF2WH->m_stACF[ACF1].szName				= pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF2WH"]["szName"];
	pCACF2WH->m_stACF[ACF1].bEnable				= (BOOL)(LONG)pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF2WH"]["bEnable"];
	pCACF2WH->m_stACF[ACF1].dLength				= pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF2WH"]["dLength"];
	pCACF2WH->m_stACF[ACF1].dWidth				= pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF2WH"]["dWidth"];

	pCACF2WH->m_stACF[ACF1].stBondOffset.x		= pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF2WH"]["stBondOffset.x"];
	pCACF2WH->m_stACF[ACF1].stBondOffset.y		= pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF2WH"]["stBondOffset.y"];
	pCACF2WH->m_stACF[ACF1].stBondOffset.Theta	= pAppMod->m_smfDevice["Package"]["m_stACF"]["ACF1"]["ACF2WH"]["stBondOffset.Theta"];

	// LSI Info. Related
	g_stLSIType[LSI_TYPE1].szName				= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["szName"];

	g_stLSIType[LSI_TYPE1].dLength				= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["dLength"];
	g_stLSIType[LSI_TYPE1].dWidth				= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["dWidth"];
	g_stLSIType[LSI_TYPE1].dThickness			= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["dThickness"]	;

	g_stLSIType[LSI_TYPE1].dmtrAlignPt1[PREBOND_1].x	= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB1"]["dmtrAlignPt1.x"];
	g_stLSIType[LSI_TYPE1].dmtrAlignPt1[PREBOND_1].y	= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB1"]["dmtrAlignPt1.y"];
	g_stLSIType[LSI_TYPE1].dmtrAlignPt2[PREBOND_1].x	= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB1"]["dmtrAlignPt2.x"];
	g_stLSIType[LSI_TYPE1].dmtrAlignPt2[PREBOND_1].y	= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB1"]["dmtrAlignPt2.y"];
	g_stLSIType[LSI_TYPE1].bAlignPtSet[PREBOND_1]		= (BOOL)(LONG)pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB1"]["bAlignPtSet"];

	g_stLSIType[LSI_TYPE1].dmtrAlignPt1[PREBOND_2].x	= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB2"]["dmtrAlignPt1.x"];
	g_stLSIType[LSI_TYPE1].dmtrAlignPt1[PREBOND_2].y	= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB2"]["dmtrAlignPt1.y"];
	g_stLSIType[LSI_TYPE1].dmtrAlignPt2[PREBOND_2].x	= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB2"]["dmtrAlignPt2.x"];
	g_stLSIType[LSI_TYPE1].dmtrAlignPt2[PREBOND_2].y	= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB2"]["dmtrAlignPt2.y"];
	g_stLSIType[LSI_TYPE1].bAlignPtSet[PREBOND_2]		= (BOOL)(LONG)pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["PB2"]["bAlignPtSet"];

	g_stLSIType[LSI_TYPE1].dmtrAlignPickPt1.x			= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["dmtrAlignPickPt1.x"];
	g_stLSIType[LSI_TYPE1].dmtrAlignPickPt1.y			= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["dmtrAlignPickPt1.y"];
	g_stLSIType[LSI_TYPE1].dmtrAlignPickPt2.x			= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["dmtrAlignPickPt2.x"];
	g_stLSIType[LSI_TYPE1].dmtrAlignPickPt2.y			= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["dmtrAlignPickPt2.y"];
	g_stLSIType[LSI_TYPE1].bAlignPickPtSet				= (BOOL)(LONG)pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["bAlignPickPtSet"];

	g_stLSIType[LSI_TYPE1].stPickOffset.x				= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["stPickOffset.x"];
	g_stLSIType[LSI_TYPE1].stPickOffset.y				= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["stPickOffset.y"];
	g_stLSIType[LSI_TYPE1].stPickOffset.Theta			= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["stPickOffset.Theta"];

	g_stLSIType[LSI_TYPE1].dmtrAlignLSIPt1[SHUTTLE_1].x	= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH1"]["dmtrAlignLSIPt1.x"];
	g_stLSIType[LSI_TYPE1].dmtrAlignLSIPt1[SHUTTLE_1].y	= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH1"]["dmtrAlignLSIPt1.y"];
	g_stLSIType[LSI_TYPE1].dmtrAlignLSIPt2[SHUTTLE_1].x = pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH1"]["dmtrAlignLSIPt2.x"];
	g_stLSIType[LSI_TYPE1].dmtrAlignLSIPt2[SHUTTLE_1].y = pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH1"]["dmtrAlignLSIPt2.y"];
	g_stLSIType[LSI_TYPE1].bAlignLSIPtSet[SHUTTLE_1]	= (BOOL)(LONG)pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH1"]["bAlignLSIPtSet"];

	g_stLSIType[LSI_TYPE1].dmtrAlignLSIPt1[SHUTTLE_2].x	= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH2"]["dmtrAlignLSIPt1.x"];
	g_stLSIType[LSI_TYPE1].dmtrAlignLSIPt1[SHUTTLE_2].y	= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH2"]["dmtrAlignLSIPt1.y"];
	g_stLSIType[LSI_TYPE1].dmtrAlignLSIPt2[SHUTTLE_2].x = pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH2"]["dmtrAlignLSIPt2.x"];
	g_stLSIType[LSI_TYPE1].dmtrAlignLSIPt2[SHUTTLE_2].y = pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH2"]["dmtrAlignLSIPt2.y"];
	g_stLSIType[LSI_TYPE1].bAlignLSIPtSet[SHUTTLE_2]	= (BOOL)(LONG)pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH2"]["bAlignLSIPtSet"];

	g_stLSIType[LSI_TYPE1].stPreBondPickOffset[SHUTTLE_1].x		= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH1"]["stPreBondPickOffset.x"];
	g_stLSIType[LSI_TYPE1].stPreBondPickOffset[SHUTTLE_1].y		= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH1"]["stPreBondPickOffset.y"];
	g_stLSIType[LSI_TYPE1].stPreBondPickOffset[SHUTTLE_1].Theta = pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH1"]["stPreBondPickOffset.Theta"];

	g_stLSIType[LSI_TYPE1].stPreBondPickOffset[SHUTTLE_2].x		= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH2"]["stPreBondPickOffset.x"];
	g_stLSIType[LSI_TYPE1].stPreBondPickOffset[SHUTTLE_2].y		= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH2"]["stPreBondPickOffset.y"];
	g_stLSIType[LSI_TYPE1].stPreBondPickOffset[SHUTTLE_2].Theta	= pAppMod->m_smfDevice["Package"]["g_stLSIType"]["LSI_TYPE1"]["SH2"]["stPreBondPickOffset.Theta"];

	// LSI Info. On Glass Related
	g_stLSI[LSI1].bEnable		= (BOOL)(LONG)pAppMod->m_smfDevice["Package"]["g_stLSI"]["LSI1"]["bEnable"];
	g_stLSI[LSI1].lLSIType		= pAppMod->m_smfDevice["Package"]["g_stLSI"]["LSI1"]["lLSIType"];

	HMI_dACFLength	= pCACF1WH->m_stACF[ACF1].dLength;
	HMI_dACFWidth	= pCACF1WH->m_stACF[ACF1].dWidth;

	//Setting
	g_bSafeIndexing			= (BOOL)(LONG)pAppMod->m_smfDevice[GetStnName()]["Settings"]["g_bSafeIndexing"];
	g_bSlowMotionIndexing	= (BOOL)(LONG)pAppMod->m_smfDevice[GetStnName()]["Settings"]["g_bSlowMotionIndexing"];
	g_ulSlowMotionFactor	= pAppMod->m_smfDevice[GetStnName()]["Settings"]["g_ulSlowMotionFactor"];
	
#if 1 //20130423
	if (g_bSlowMotionIndexing)
	{
		SetMotionProfileMode(g_ulSlowMotionFactor);
	}
	else
	{
		SetMotionProfileMode(NORMAL_MOT_PROF);
	}
#endif
	return CAC9000Stn::IPC_LoadDeviceParam();
}

BOOL CHouseKeeping::checkReceivedStatus(GATEWAY_STATUS_TYPE lStatus)
{
	if (lStatus >= ST_GW_TA5_MAX || lStatus < ST_GW_INSHWH_IDLE)
	{
		return gnAMS_NOTOK;
	}
	else
	{
		return gnAMS_OK;
	}
}

//LONG CHouseKeeping::GatewayUpdateGC902CWHStatus(IPC_CServiceMessage &svMsg)
//{
//	CString szMsg = _T("");
//	INT nMsgLen = 0;
//	IPC_CServiceMessage svRpyMsg;
//	//LONG lRpyNum = m_lGatewayReplayNo;
//	
//	//DisplayMessage("xxxx GatewayUpdateAC9000TA1Status xxxx");
//	nMsgLen = svMsg.GetMsgLen();
//
//	if (nMsgLen != sizeof(LONG))
//	{
//		DisplayMessage("GatewayUpdateGC902CWHStatus, MsgLen error");
//	}
//	else
//	{
//		GATEWAY_STATUS_TYPE emOldStatus = GC902_CWH_Status;
//		svMsg.GetMsg(nMsgLen, &GC902_CWH_Status);
//		if (GC902_CWH_Status != emOldStatus)	// 20140728 Yip: Show GC902 CWH Status Only When Change Occurred
//		{
//			if (HMI_bDebugSeq)	// 20140801 Yip: Only Show Message When Debug Sequence
//			{
//				szMsg.Format("GC902_CWH_Status change= %ld", GC902_CWH_Status);
//				DisplayMessage(szMsg);
//			}
//		}
//		if (checkReceivedStatus(GC902_CWH_Status) != gnAMS_OK)
//		{
//			DisplayMessage("CWH State Received is invalid");
//		}
//	}
//
//	//virtual INT WriteReply(const CString&	rCommand,
//	//		   IPC_CServiceMessage&		rReplyMsg,
//	//		   INT						nReplyMode = 1,
//	//		   INT						nReplyNum = 1);
//
//	//svRpyMsg.InitMessage(sizeof(LONG), &lRpyNum);
//
//	//m_comServer.WriteReply(CString("GatewayCmd"), svRpyMsg);
//
//	//if (++m_lGatewayReplayNo > 10000)
//	//	m_lGatewayReplayNo = 0;
//
//	return 1;	
//}

LONG CHouseKeeping::CheckGatewayConnection(IPC_CServiceMessage &svMsg)
{
#if 1
	LONG lMsgLength = 0;
	CString szResult = _T(" "), szMsg = _T(" ");
	IPC_CServiceMessage svRpyMsg;
	LONG lRpyNum = gnAMS_OK;

	if (getGatewayString(svMsg, lMsgLength, szResult) == gnAMS_OK)
	{
		szMsg.Format("CheckGatewayConnection: Received Msg=%s", szResult);
		DisplayMessage(szMsg);

		//Reply integer
		svRpyMsg.InitMessage(sizeof(LONG), &lRpyNum);
		m_comServer.WriteReply(CString("HK_CheckGatewayConnection"), svRpyMsg);
		return 1;
	}
	return 0;
#else
	CString szMsg = _T("");
	INT nMsgLen = 0;
	IPC_CServiceMessage svRpyMsg;
	LONG lRpyNum = gnAMS_OK;
	char *pBuffer = NULL;
	
	//DisplayMessage("xxxx GatewayUpdateAC9000TA1Status xxxx");
	nMsgLen = svMsg.GetMsgLen();

	if (nMsgLen > 200 || nMsgLen == 0)
	{
		DisplayMessage("CheckGatewayConnection, MsgLen error");
	}
	else
	{
		pBuffer = new char[nMsgLen + 1];

		if (pBuffer != NULL)
		{
			memset(pBuffer, 0, nMsgLen + 1);
			*(pBuffer + nMsgLen) = '\0';
			svMsg.GetMsg(nMsgLen, pBuffer);
			
			szMsg.Format("AC9000_CheckConnection String= %s", pBuffer);
			DisplayMessage(szMsg);

			svRpyMsg.InitMessage(sizeof(LONG), &lRpyNum);
			m_comServer.WriteReply(CString("HK_UpdateAC9000TA1Status"), svRpyMsg);

			delete[] pBuffer;
		}
	}

	return 1;	
#endif
}

LONG CHouseKeeping::GetGatewayTA5Status(IPC_CServiceMessage &svMsg)
{
	LONG lMsgLength = 0;
	CString szResult = _T(" "), szMsg = _T(" ");
	IPC_CServiceMessage svRpyMsg;
	LONG lRpyNum = 0;

	if (getGatewayString(svMsg, lMsgLength, szResult) == gnAMS_OK)
	{
		szMsg.Format("GetGatewayTA5Status: Received Msg=%s", szResult);
		DisplayMessage(szMsg);

		//Reply integer
		lRpyNum = g_lGatewayTA1Status;
		svRpyMsg.InitMessage(sizeof(LONG), &lRpyNum);
		m_comServer.WriteReply(CString("HK_GetGatewayTA5Status"), svRpyMsg);
		return 1;
	}
	return 0;
}

LONG CHouseKeeping::GatewayUpdateCOG902TA5Status(IPC_CServiceMessage &svMsg)
{
	CString szMsg = _T("");
	INT nMsgLen = 0;
	IPC_CServiceMessage svRpyMsg;
	//LONG lRpyNum = m_lGatewayReplayNo;
	
	//DisplayMessage("xxxx GatewayUpdateCOG902TA1Status xxxx");
	nMsgLen = svMsg.GetMsgLen();

	if (nMsgLen != sizeof(LONG))
	{
		DisplayMessage("GatewayUpdateCOG902TA5Status, MsgLen error");
	}
	else
	{
		GATEWAY_STATUS_TYPE emOldStatus = COG902_TA5_Status;
		svMsg.GetMsg(nMsgLen, &COG902_TA5_Status);
		if (COG902_TA5_Status != emOldStatus)	// 20140728 Yip: Show COG902 TA5 Status Only When Change Occurred
		{
			if (HMI_bDebugSeq)	// 20140801 Yip: Only Show Message When Debug Sequence
			{
				szMsg.Format("COG902_TA5_Status change= %ld", COG902_TA5_Status);
				DisplayMessage(szMsg);
			}
		}
		if (checkReceivedStatus(COG902_TA5_Status) != gnAMS_OK)
		{
			DisplayMessage("TA5 State Received is invalid");
		}
	}

	switch (COG902_TA5_Status)
	{
	//update COG902 input glass info
	case ST_GW_TA5_G1_G2_NOR:
		g_COG902GlassInfo[L_GLASS].bIsExist = TRUE;
		g_COG902GlassInfo[L_GLASS].bToRemove = FALSE;
		g_COG902GlassInfo[R_GLASS].bIsExist = TRUE;
		g_COG902GlassInfo[R_GLASS].bToRemove = FALSE;
		break;
	case ST_GW_TA5_G1_NOR_G2_EMP:
		g_COG902GlassInfo[L_GLASS].bIsExist = TRUE;
		g_COG902GlassInfo[L_GLASS].bToRemove = FALSE;
		g_COG902GlassInfo[R_GLASS].bIsExist = FALSE;
		g_COG902GlassInfo[R_GLASS].bToRemove = FALSE;
		break;
	case ST_GW_TA5_G1_NOR_G2_ERR:
		g_COG902GlassInfo[L_GLASS].bIsExist = TRUE;
		g_COG902GlassInfo[L_GLASS].bToRemove = FALSE;
		g_COG902GlassInfo[R_GLASS].bIsExist = TRUE;
		g_COG902GlassInfo[R_GLASS].bToRemove = TRUE;
		break;

	case ST_GW_TA5_G1_G2_EMP:
		g_COG902GlassInfo[L_GLASS].bIsExist = FALSE;
		g_COG902GlassInfo[L_GLASS].bToRemove = FALSE;
		g_COG902GlassInfo[R_GLASS].bIsExist = FALSE;
		g_COG902GlassInfo[R_GLASS].bToRemove = FALSE;
		break;
	case ST_GW_TA5_G1_EMP_G2_NOR:
		g_COG902GlassInfo[L_GLASS].bIsExist = FALSE;
		g_COG902GlassInfo[L_GLASS].bToRemove = FALSE;
		g_COG902GlassInfo[R_GLASS].bIsExist = TRUE;
		g_COG902GlassInfo[R_GLASS].bToRemove = FALSE;
		break;
	case ST_GW_TA5_G1_EMP_G2_ERR:
		g_COG902GlassInfo[L_GLASS].bIsExist = FALSE;
		g_COG902GlassInfo[L_GLASS].bToRemove = FALSE;
		g_COG902GlassInfo[R_GLASS].bIsExist = TRUE;
		g_COG902GlassInfo[R_GLASS].bToRemove = TRUE;
		break;

	case ST_GW_TA5_G1_G2_ERR:
		g_COG902GlassInfo[L_GLASS].bIsExist = TRUE;
		g_COG902GlassInfo[L_GLASS].bToRemove = TRUE;
		g_COG902GlassInfo[R_GLASS].bIsExist = TRUE;
		g_COG902GlassInfo[R_GLASS].bToRemove = TRUE;
		break;
	case ST_GW_TA5_G1_ERR_G2_NOR:
		g_COG902GlassInfo[L_GLASS].bIsExist = TRUE;
		g_COG902GlassInfo[L_GLASS].bToRemove = TRUE;
		g_COG902GlassInfo[R_GLASS].bIsExist = TRUE;
		g_COG902GlassInfo[R_GLASS].bToRemove = FALSE;
		break;
	case ST_GW_TA5_G1_ERR_G2_EMP:
		g_COG902GlassInfo[L_GLASS].bIsExist = TRUE;
		g_COG902GlassInfo[L_GLASS].bToRemove = TRUE;
		g_COG902GlassInfo[R_GLASS].bIsExist = FALSE;
		g_COG902GlassInfo[R_GLASS].bToRemove = FALSE;
		break;
	//default:
	//	//ST_GW_TA5_G1_G2_NOR:
	//	g_COG902GlassInfo[L_GLASS].bIsExist = TRUE;
	//	g_COG902GlassInfo[L_GLASS].bToRemove = FALSE;
	//	g_COG902GlassInfo[R_GLASS].bIsExist = TRUE;
	//	g_COG902GlassInfo[R_GLASS].bToRemove = FALSE;
	//	break;

	}

	//virtual INT WriteReply(const CString&	rCommand,
	//		   IPC_CServiceMessage&		rReplyMsg,
	//		   INT						nReplyMode = 1,
	//		   INT						nReplyNum = 1);

	//svRpyMsg.InitMessage(sizeof(LONG), &lRpyNum);

	//m_comServer.WriteReply(CString("GatewayCmd"), svRpyMsg);

	//if (++m_lGatewayReplayNo > 10000)
	//	m_lGatewayReplayNo = 0;

	return 1;	
}

LONG CHouseKeeping::GatewayUpdateCOG902MachineStatus(IPC_CServiceMessage &svMsg)
{
	CString szMsg = _T("");
	INT nMsgLen = 0;
	IPC_CServiceMessage svRpyMsg;
	//LONG lRpyNum = m_lGatewayReplayNo;
	
	//DisplayMessage("xxxx GatewayUpdateCOG902TA1Status xxxx");
	nMsgLen = svMsg.GetMsgLen();

	if (nMsgLen != sizeof(LONG))
	{
		DisplayMessage("GatewayUpdateCOG902MachineStatus, MsgLen error");
	}
	else
	{
		svMsg.GetMsg(nMsgLen, &COG902_Machine_Status);

		if (COG902_Machine_Status == ST_GW_READY)
		{
			szMsg.Format("COG902_Machine_Status = ST_GW_READY");
		}
		else if (COG902_Machine_Status == ST_GW_STOP)
		{
			szMsg.Format("COG902_Machine_Status = ST_GW_STOP");
		}
		else
		{
			szMsg.Format("COG902_Machine_Status = ERROR!!");
		}

		DisplayMessage(szMsg);
		if (checkReceivedStatus(COG902_Machine_Status) != gnAMS_OK)
		{
			DisplayMessage("CWH State Received is invalid");
		}
	}

	return 1;
}

INT CHouseKeeping::getGatewayString(IPC_CServiceMessage &svMsg, LONG &lMsgLength, CString &szResult, LONG lMaxMsgLength)
{
	CString szMsg = _T("");
	INT nMsgLen = 0;
	char *pBuffer = NULL;
	
	nMsgLen = svMsg.GetMsgLen();

	if (nMsgLen > lMaxMsgLength || nMsgLen == 0)
	{
		DisplayMessage("getGatewayString, MsgLen error");
		return gnAMS_NOTOK;
	}
	else
	{
		pBuffer = new char[nMsgLen + 1];

		if (pBuffer != NULL)
		{
			memset(pBuffer, 0, nMsgLen + 1);
			*(pBuffer + nMsgLen) = '\0';
			svMsg.GetMsg(nMsgLen, pBuffer);
			
			//szMsg.Format("getGatewayString String= %s", pBuffer);
			//DisplayMessage(szMsg);
			szResult.Format("%s", pBuffer);

			delete[] pBuffer;
			return gnAMS_OK;
		}
	}
	return gnAMS_NOTOK;
}

INT CHouseKeeping::getGatewayLong(IPC_CServiceMessage &svMsg, LONG &lMsgLength, LONG &lResult)
{
	//CString szMsg = _T("");
	INT nMsgLen = 0;
	
	nMsgLen = svMsg.GetMsgLen();

	if (nMsgLen != sizeof(LONG) || nMsgLen == 0)
	{
		DisplayMessage("getGatewayLong, MsgLen error");
		return gnAMS_NOTOK;
	}
	else
	{
		svMsg.GetMsg(nMsgLen, &lResult);
		//szMsg.Format("getGatewayString String= %s", pBuffer);
		//DisplayMessage(szMsg);
		return gnAMS_OK;
	}
	return gnAMS_NOTOK;
}


// 20141022
//INT CHouseKeeping::SetMBOddIndexRollSol(BOOL bMode, BOOL bWait)
//{
//	INT nResult = GMP_SUCCESS;
//
//	CSoInfo *stTempSo = &m_stMBOddIndexRollSol;
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

//INT CHouseKeeping::SetMBEvenIndexRollSol(BOOL bMode, BOOL bWait)
//{
//	INT nResult = GMP_SUCCESS;
//
//	CSoInfo *stTempSo = &m_stMBEvenIndexRollSol;
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

//LONG CHouseKeeping::HMI_ToggleHTPwrOnSol(IPC_CServiceMessage &svMsg)
//{
//	if (m_stHTPwrOnSol.bHMI_Status)
//	{
//		SetHTPwrOnSol(OFF);
//	}
//	else
//	{
//		SetHTPwrOnSol(ON);
//	}
//
//	return 0;
//}




//LONG CHouseKeeping::HMI_ToggleMBOddIndexRollSol(IPC_CServiceMessage &svMsg)
//{
//	if (m_stMBOddIndexRollSol.bHMI_Status)
//	{
//		SetMBOddIndexRollSol(OFF);
//	}
//	else
//	{
//		SetMBOddIndexRollSol(ON);
//	}
//
//	return 0;
//}
//
//LONG CHouseKeeping::HMI_ToggleMBEvenIndexRollSol(IPC_CServiceMessage &svMsg)
//{
//	if (m_stMBEvenIndexRollSol.bHMI_Status)
//	{
//		SetMBEvenIndexRollSol(OFF);
//	}
//	else
//	{
//		SetMBEvenIndexRollSol(ON);
//	}
//
//	return 0;
//}


LONG CHouseKeeping::ProcessGatewayCommand(IPC_CServiceMessage &svMsg)
{
	//CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	////CInSHWH *pCInSHWH = (CInSHWH*)m_pModule->GetStation("InSHWH");
	//LONG lMsgLength = 0;
	//CString szResult = _T(" "), szMsg = _T(" ");
	//IPC_CServiceMessage svRpyMsg;
	//LONG lRpyNum = gnAMS_NOTOK;

	//if (getGatewayString(svMsg, lMsgLength, szResult) == gnAMS_OK)
	//{
	//	szMsg.Format("ProcessGatewayCommand: Received Msg=%s", szResult);
	//	DisplayMessage(szMsg);

	//	if (/*pCInSHWH->HMI_bStandAlone*/)
	//	{
	//		HMIMessageBox(MSG_OK, "Gateway Command Received", "Gateway Command From Inline, Please UnselectStand Alone Mode");
	//		lRpyNum = gnAMS_NOTOK;
	//		DisplayMessage("***********************************");
	//		DisplayMessage("Stand Alone Mode: Receive Command From Inline");
	//		DisplayMessage("***********************************");

	//		svRpyMsg.InitMessage(sizeof(LONG), &lRpyNum);
	//		m_comServer.WriteReply(CString("HK_ProcessGatewayCommand"), svRpyMsg);
	//		return 1;

	//	}

	//	if (szResult.GetLength() > 0)
	//	{
	//		DisplayMessage("szResult");
	//		////Pick glass command
	//		if (szResult == CString("AC9000_LoadGlass1Req"))
	//		{
	//			//load req glass 1
	//			lRpyNum = pAppMod->LoadGlassReqGatewayOperation(GLASS1);
	//		}
	//		else if (szResult == CString("AC9000_LoadGlass2Req"))
	//		{
	//			//load req glass 2
	//			lRpyNum = pAppMod->LoadGlassReqGatewayOperation(GLASS2);
	//		}
	//		else if (szResult == CString("AC9000_LoadGlass1_2Req"))
	//		{
	//			//load req glass 1 & 2
	//			lRpyNum = pAppMod->LoadGlassReqGatewayOperation(MAX_NUM_OF_GLASS);
	//		}
	//		///////////////////////////////////////////////
	//		else if (szResult == CString("AC9000_LoadGlass1"))
	//		{
	//			//Load glass 1
	//			lRpyNum = pAppMod->LoadGlassGatewayOperation(GLASS1);
	//		}
	//		else if (szResult == CString("AC9000_LoadGlass2"))
	//		{
	//			//Load glass 2
	//			lRpyNum = pAppMod->LoadGlassGatewayOperation(GLASS2);
	//		}
	//		else if (szResult == CString("AC9000_LoadGlass1_2"))
	//		{
	//			//Load glass 1 & 2
	//			lRpyNum = pAppMod->LoadGlassGatewayOperation(MAX_NUM_OF_GLASS);
	//		}

	//		else
	//		{
	//			lRpyNum = gnAMS_NOTOK;
	//			DisplayMessage("***********************************");
	//			DisplayMessage("Unknown Remote Command :" + szResult);
	//			DisplayMessage("***********************************");
	//		}
	//	}

	//	//Reply integer
	//	svRpyMsg.InitMessage(sizeof(LONG), &lRpyNum);
	//	m_comServer.WriteReply(CString("HK_ProcessGatewayCommand"), svRpyMsg);
	//	return 1;
	//}
	return 0;
}

LONG CHouseKeeping::HMI_SetChangeACF(IPC_CServiceMessage &svMsg)	// 20140917 Yip
{
	BOOL bResult = TRUE;
	HMI_bChangeACF = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CHouseKeeping::HMI_PRAutoTestStop(IPC_CServiceMessage &svMsg)
{
	CAC9000App *m_pAppMod = dynamic_cast<CAC9000App*> (m_pModule);
	if(!HMI_bAutoTestStop)
	{
		HMI_bAutoTestStop = TRUE;
	}
	m_pAppMod->HMIMessageBox(MSG_OK, "AUTO TEST OPERATION", "Auto Test Stopped.");

	return 0;

}

#ifdef PB_LSI_EXPANDSION //20150303
LONG CHouseKeeping::HMI_StopPRLoopTest(IPC_CServiceMessage &svMsg)
{
	m_bPRLoopTest = FALSE;
	return 0;
}
#endif

LONG CHouseKeeping::HMI_TA1ZMoveLoopStop(IPC_CServiceMessage &svMsg)
{
	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");

	pCTA1->m_bZMoveLoopEnable = FALSE;
	return 0;
}

//LONG CHouseKeeping::HMI_SetDebugVar(IPC_CServiceMessage &svMsg) //20150714
//{
//	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");
//
//	m_bHouekeepingDebug = !m_bHouekeepingDebug;
//	return 0;
//}

VOID CHouseKeeping::LockAllCovers()
{
		SetACFFrontDoorLockSol(ON);
		SetACFBackDoorLockSol(ON);
		Sleep(500);
}

VOID CHouseKeeping::UnLockAllCovers()
{
		SetACFFrontDoorLockSol(OFF);
		SetACFBackDoorLockSol(OFF);
		Sleep(500);
}

INT CHouseKeeping::SetCleanVacSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stCleanVacSol;

	if(IsBurnInDiagBond())
	{
		Sleep(100);
		stTempSo->bHMI_Status = bMode;
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

			if (!g_bDiagMode)
			{
				stTempSo->GetGmpPort().On();
				stTempSo->bHMI_Status = TRUE;
			}
			else
			{
				stTempSo->GetGmpPort().Off();
				stTempSo->bHMI_Status = FALSE;
			}

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

INT CHouseKeeping::SetCleanPreSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stCleanPreSol;

	if(IsBurnInDiagBond())
	{
		Sleep(100);
		stTempSo->bHMI_Status = bMode;
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

			if (!g_bDiagMode)
			{
				stTempSo->GetGmpPort().On();
				stTempSo->bHMI_Status = TRUE;
			}
			else
			{
				stTempSo->GetGmpPort().Off();
				stTempSo->bHMI_Status = FALSE;
			}

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

LONG CHouseKeeping::HMI_ToggleCleanVacSol(IPC_CServiceMessage &svMsg)
{
	SetCleanVacSol(!m_stCleanVacSol.bHMI_Status);

	return 0;
}

LONG CHouseKeeping::HMI_ToggleCleanPreSol(IPC_CServiceMessage &svMsg)
{
	SetCleanPreSol(!m_stCleanPreSol.bHMI_Status);

	return 0;
}

VOID CHouseKeeping::ResetSystem()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	IPC_CServiceMessage svMsg;
	CString szScreen = "Bonding";
	for (INT i = 0; i < g_lErrorCounter; i++)
	{
		CloseAlarm();
	}
	g_lErrorCounter = 0;
	if (AutoMode == AUTO_BOND)
	{
		pAppMod->HMI_SetAllModSelected(svMsg);
	}
	svMsg.InitMessage(szScreen.GetLength() + 1, szScreen.GetBuffer());
	szScreen.ReleaseBuffer();
	m_comClient.SendRequest("HmiUserService", "HmiSetScreen", svMsg, 0);
}

VOID CHouseKeeping::StartBonding()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	IPC_CServiceMessage svMsg;
	CString szScreen = "Operation";
	//pAppMod->HMI_CheckStartBondingAllowed(svMsg);
	svMsg.InitMessage(szScreen.GetLength() + 1, szScreen.GetBuffer());
	szScreen.ReleaseBuffer();
	m_comClient.SendRequest("HmiUserService", "HmiSetScreen", svMsg, 0);
	pAppMod->SrvAutoCommand(svMsg);
}

VOID CHouseKeeping::StopBonding()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	HMI_bStopProcess = FALSE;
	SetHmiVariable("HMI_bStopProcess");
	IPC_CServiceMessage svMsg;
	//CString szScreen = "Bonding";
	pAppMod->SrvStopCommand(svMsg);
	pAppMod->HMI_StatisticsLogFile(svMsg);
	//do
	//{
	//	Sleep(50);

	//} while (pAppMod->m_szState != "Idle");
	//HMI_LightAndBuzzerInIdle(svMsg);
	//svMsg.InitMessage(szScreen.GetLength() + 1, szScreen.GetBuffer());
	//szScreen.ReleaseBuffer();
	//m_comClient.SendRequest("HmiUserService", "HmiSetScreen", svMsg, 0);
}
