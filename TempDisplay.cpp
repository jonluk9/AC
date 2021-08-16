/////////////////////////////////////////////////////////////////
//	TempDisplay.cpp : interface of the CTempDisplay class
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
// Trash
#include "stdafx.h"
#include "MarkConstant.h"
#include "AC9000.h"
#include "AC9000_Constant.h"
#include "ACF1.h"
#include "ACF1WH.h"
#include "ACF2WH.h"
#include "Cal_Util.h"
//#include "PB1.h"
//#include "PB2.h"
//#include "MB1.h"
//#include "MB2.h"
#include "HouseKeeping.h"

#define TEMPDISPLAY_EXTERN
#include "TempDisplay.h"
#include "math.h"

#include "LitecCtrl.h"
#include "AdamCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define HEATER_MALFUNC_ON_TO_CHECK_DELAY		(60 * 10)		/*10 minute*/
#define HEATER_MALFUNC_TIMOUT	10000
#define TEMP_ERROR_TOL	10.0

IMPLEMENT_DYNCREATE(CTempDisplay, CAC9000Stn)

CTempDisplay::CTempDisplay()
{
	INT i;
	INT k;	

	//// So Related
	//m_stPB1HeaterCoolingSol		= CSoInfo("PB1CoolingSol",			"IOMODULE_OUT_PORT_1", BIT_12, PB1_SOL, ACTIVE_HIGH);
	//m_mySoList.Add((CSoInfo*)&m_stPB1HeaterCoolingSol);
	//m_stPB2HeaterCoolingSol		= CSoInfo("PB2CoolingSol",			"IOMODULE_OUT_PORT_3", BIT_6, PB1_SOL, ACTIVE_HIGH);
	//m_mySoList.Add((CSoInfo*)&m_stPB2HeaterCoolingSol);
	//m_stMB1HeaterCoolingSol		= CSoInfo("MB1CoolingSol",	"IOMODULE_OUT_PORT_17", BIT_0, MB1_SOL, ACTIVE_HIGH);
	//m_mySoList.Add((CSoInfo*)&m_stMB1HeaterCoolingSol);
	//m_stMB2HeaterCoolingSol		= CSoInfo("MB2CoolingSol",	"IOMODULE_OUT_PORT_17", BIT_1, MB1_SOL, ACTIVE_HIGH);
	//m_mySoList.Add((CSoInfo*)&m_stMB2HeaterCoolingSol);
	////AC9000
	//m_stMB3HeaterCoolingSol		= CSoInfo("MB3CoolingSol",	"IOMODULE_OUT_PORT_19", BIT_0, MB1_SOL, ACTIVE_HIGH);
	//m_mySoList.Add((CSoInfo*)&m_stMB3HeaterCoolingSol);
	//m_stMB4HeaterCoolingSol		= CSoInfo("MB4CoolingSol",	"IOMODULE_OUT_PORT_19", BIT_1, MB1_SOL, ACTIVE_HIGH);
	//m_mySoList.Add((CSoInfo*)&m_stMB4HeaterCoolingSol);


	for (i = 0; i < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; i++)
	{
		m_bHeaterOn[i]					= FALSE;
		m_dHeaterTemp[i]				= 0.0;
		m_dHeaterSetTemp[i]				= 0.0;
		m_dHeaterTempTol[i]				= 5.0;
		m_bHeaterATOn[i]				= FALSE;
		m_dBondPtTemp[i]				= 0.0;
		m_bHeaterExist[i]				= FALSE; // newly added, alfred
		m_clkHeaterOnTime[i]	=  -1; //20150309

	}

	// Real Heater Exist for Power On check
	m_bHeaterExist[ACF1_CH1]				= TRUE;	//1
	m_bHeaterExist[ACF1WH_CH3]				= TRUE;	//1
	m_bHeaterExist[ACF2WH_CH4]				= TRUE;	//1
	//m_bHeaterExist[ACF2_CH2]				= TRUE;	//2
	//m_bHeaterExist[PB1_CH3]					= TRUE;	//3
	//m_bHeaterExist[PB2_CH4]					= TRUE;	//4
	//m_bHeaterExist[MB1_CTRL_CH1]			= TRUE; //13
	//m_bHeaterExist[MB2_CTRL_CH2]			= TRUE; //16
	//m_bHeaterExist[MB3_CTRL_CH1]			= TRUE; //25
	//m_bHeaterExist[MB4_CTRL_CH2]			= TRUE; //28
	//m_bHeaterExist[MB1_WH_CH1]				= TRUE; //37
	//m_bHeaterExist[MB2_WH_CH2]				= TRUE; //38
	//m_bHeaterExist[MB3_WH_CH3]				= TRUE; //39
	//m_bHeaterExist[MB4_WH_CH4]				= TRUE; //40


	// temp calibration
	for (k = 0; k < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; k++)
	{
		for (i = 0; i < MAX_TEMP_DATA; i++)
		{
			m_stTempCalib[k][i].SetTemp		= 0.0;
			m_stTempCalib[k][i].BondTemp	= 0.0;
		}
	}

	// Temp. Monitor related
	m_bMonTemp				= TRUE; //FALSE; //20130412

	for (i = 0; i < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; i++)
	{
		m_bMonFailed[i]				= FALSE;
		m_dMonTemp[i]				= 0.0;
		m_dMonOffset[i]				= 0.0;		
		m_dMonTol[i]				= 0.0;
	}


	for (i = 0; i < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; i++)
	{
		m_bHeaterFailed[i]			= FALSE;
		m_dwFaultTimerStart[i]		= 0;
	}

	m_nCurIndex							= 0;

	m_bHeatersOn = FALSE;	// 20140918 Yip: Add Flag To Toggle All Heaters Power On/Off
}

CTempDisplay::~CTempDisplay()
{
}

BOOL CTempDisplay::InitInstance()
{
	CAC9000Stn::InitInstance();
	
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CGmpSystem *pGmpSys = pAppMod->m_pCGmpSystem;
	CString		szMsg	= _T("");

	try
	{
		// IOs
		//if (pAppMod->IsInitNuDrive())
		//{
			// SO
			//for (INT i = 0; i < m_mySoList.GetSize(); i++)
			//{
			//	GetmySoList().GetAt(i)->SetGmpPort(
			//		pGmpSys->AddSoPort(m_mySoList.GetAt(i)->GetPortName(), 
			//						   m_mySoList.GetAt(i)->GetMask(), 
			//						   m_mySoList.GetAt(i)->GetActiveState()));

			//	GetmySoList().GetAt(i)->GetGmpPort().Off();
			//	GetmySoList().GetAt(i)->bHMI_Status = GetmySoList().GetAt(i)->GetGmpPort().IsOn();
			//}
		//}

	}
	catch (CAsmException e)
	{
		pAppMod->m_bHWInitError = TRUE;

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
	
	do
	{
		Sleep(100);
	} while (!pAppMod->IsTempCtrlInited());

	//Set Temp
	if (m_fHardware && !pAppMod->m_bHWInitError && !pAppMod->m_bDiagnMode)
	{
		if (
			HEATER_FAILED == SetHeaterTempFromData()
		   )
		{
			DisplayMessage("Set Heater Temp from Data Error");
		}
		else
		{
			// 20140905 Yip: Power On All Heaters When System Initialization To Save Heat Up Time
			if (theApp.IsSelectHeaterOn())
			{
				DisplayMessage("Power On All Heater");
				PowerOnAllHeater(TRUE);
			}
		}
	}

	return TRUE;
}

INT CTempDisplay::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CAC9000Stn::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CTempDisplay::Operation()
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
		Sleep(10 * 10);
	}
}


/////////////////////////////////////////////////////////////////
//Update Functions
/////////////////////////////////////////////////////////////////
BOOL CTempDisplay::IsLitecHeaterExist(INT nHeaterChNum)
{
	BOOL bResult = FALSE;

	if (m_bHeaterExist[nHeaterChNum])
	{
		bResult = TRUE;
	}

	return bResult;
}

VOID CTempDisplay::UpdateOutput()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");
	CAdamCtrl *pCAdamCtrl = (CAdamCtrl*)m_pModule->GetStation("AdamCtrl");

	CString szMsg = _T("");
	BOOL bError	= FALSE;
	static clock_t clkLastTempLogTime = 0; //20150629
	clock_t clkNow = 0;


	if ((m_qState == UN_INITIALIZE_Q) || (m_qState == DE_INITIAL_Q) || (m_qState == CYCLE_Q))
	{
		return;
	}

	if (pAppMod->m_bHWInitError)
	{
		return;
	}

	if ((pAppMod->m_lAction == glAMS_SYSTEM_EXIT) || (m_lAction == glAMS_SYSTEM_EXIT))
	{
		return;
	}

	if (!m_fHardware)
	{
		return;
	}

#if 1 //20150629
	clkNow = clock();
	if (m_qState == AUTO_Q || m_qState == DEMO_Q)
	{
		if (clkNow != -1 && (DOUBLE)labs(clkNow - clkLastTempLogTime) / CLOCKS_PER_SEC > 5.0) //5 second
		{
			//log time and tempreture
			clkLastTempLogTime = clock();
			LogTempretureInfo(CString("LogTemp")); //20150629
		}
	}
#endif

	try
	{
		
		if (pCAdamCtrl != NULL)
		{
			pCAdamCtrl->GetTempValue(this); //alfred???
		}
		if (pCLitecCtrl != NULL)
		{
			pCLitecCtrl->GetTempValue(this);
		}

		//Heaters
		if (CheckHeaterInit() && (m_nCurIndex < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL) && IsLitecHeaterExist(m_nCurIndex))
		{
			if (Command() == glPRESTART_COMMAND || Command() == glSTOP_COMMAND || g_bStopCycle)
			{
				return;
			}

			// Check Heater Malfunction Timeout When m_bMonTemp is enabled
			if (m_bHeaterOn[m_nCurIndex]) //20130412
			{
				if (CheckHeaterMalfunctionTimeout(m_nCurIndex))
				{
					bError = TRUE;
				}
			}			

			// Check Current Temp during AUTOBOND && TESTBOND
			if (
				(State() == AUTO_Q || State() == DEMO_Q || State() == CYCLE_Q) &&
				(AutoMode == AUTO_BOND || AutoMode == TEST_BOND)
			   )
			{
				if (m_bHeaterOn[m_nCurIndex])
				{
					if (CheckHeaterCurTemp(m_nCurIndex))
					{
						bError = TRUE;
					}
				}
			}

			// Heater Failed
			//Check if heater fail only if module is selected
			bError = CheckHeaterFail(m_nCurIndex);
			
			// turn off cooling if Heater under 50 degreeC or not in use
			//CheckHeaterCooling(m_nCurIndex);
		}
		
		if (bError)
		{
#if 1 //20120119 testing
			if ((State() == AUTO_Q || State() == DEMO_Q || State() == CYCLE_Q) && (AutoMode != BURN_IN))
#else
			if (State() == AUTO_Q || State() == DEMO_Q || State() == CYCLE_Q)
#endif
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

				stMsg.InitMessage(sizeof(PARA), &stPara);
				nConvID = m_comClient.SendRequest("AC9000", "Stop", stMsg);
				while (!m_comClient.ScanReplyForConvID(nConvID, 1) && (lTimeCount++ < 8640000))
				{
					Sleep(1);
				}
				m_comClient.ReadReplyForConvID(nConvID, stMsg);

				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				DisplayMessage("Operation Stopped! Temp Ctrl Error!");
				//SetError(IDS_TEMP_CTRL_CTRL_ERR); //20130222
				SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(m_nCurIndex)); //20140401
				SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + pCLitecCtrl->GetLitecErrNum(m_nCurIndex)); //20130110
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			}
		}

		m_nCurIndex++;
		if (m_nCurIndex >= NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL)
		{
			m_nCurIndex = 0;
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

VOID CTempDisplay::RegisterVariables()
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");
	
	
	try
	{
		CString szName = "";
		///////////////////////////////////////////////////
		// variables //////////////////////////////////////
		///////////////////////////////////////////////////
	
		// Heater related
		for (INT i = 0; i < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; i++)
		{
			//if(pCLitecCtrl->IsLitecTempUpdate(i)) //heater & Mon Channels

			if (m_bHeaterExist[i])		//<- Heater Only 
			{
				szName.Format(GetStation() + "bHTR%ldOn", i + 1);			//TD_bHTR1On
				RegVariable(szName, &m_bHeaterOn[i]);
				szName.Format(GetStation() + "dHTR%ldTemp", i + 1);		//TD_dHTR1Temp
				RegVariable(szName, &m_dHeaterTemp[i]);
				szName.Format(GetStation() + "dHTR%ldSetTemp", i + 1);	//TD_dHTR1SetTemp
				RegVariable(szName, &m_dHeaterSetTemp[i]);
				szName.Format(GetStation() + "dHTR%ldSetTempTol", i + 1);	//TD_dHTR1SetTempTol
				RegVariable(szName, &m_dHeaterTempTol[i]);
				szName.Format(GetStation() + "dHTR%ldBondPtTemp", i + 1); //TD_dHTR1BondPtTemp
				RegVariable(szName, &m_dBondPtTemp[i]);
				szName.Format(GetStation() + "bHTR%ldATOn", i + 1);		//TD_bHTR1ATOn
				RegVariable(szName, &m_bHeaterATOn[i]);
				szName.Format(GetStation() + "bHTR%ldValidTemp", i + 1);	//TD_bHTR1ValidTemp
				RegVariable(szName, &m_bValidTemp[i]);
				szName.Format(GetStation() + "dHTR%ldMonTemp", i + 1);	//TD_dHTR1MonTemp
				RegVariable(szName, &m_dMonTemp[i]);
				szName.Format(GetStation() + "dHTR%ldMonOffset", i + 1);	//TD_dHTR1MonOffset
				RegVariable(szName, &m_dMonOffset[i]);
				szName.Format(GetStation() + "dHTR%ldMonTol", i + 1);		//TD_dHTR1MonTol
				RegVariable(szName, &m_dMonTol[i]);
			}
		}
//p20121004
		//RegVariable(GetStation() + "dMB1WHLeftAlarmTemp", &m_dMB1WHLeftAlarmTemp);
//p20121004:end

		// Temp. Monitor related
		RegVariable(GetStation() + "bMonTemp", &m_bMonTemp);

		RegVariable(GetStation() + "bHeatersOn", &m_bHeatersOn);	// 20140918 Yip: Add Flag To Toggle All Heaters Power On/Off

		//Heater Cooling Sol
		//RegVariable(_T("HMI_bPB1HeaterCoolingSol"), &m_stPB1HeaterCoolingSol.bHMI_Status);
		//RegVariable(_T("HMI_bPB2HeaterCoolingSol"), &m_stPB2HeaterCoolingSol.bHMI_Status);
		//RegVariable(_T("HMI_bMB1HeaterCoolingSol"), &m_stMB1HeaterCoolingSol.bHMI_Status);
		//RegVariable(_T("HMI_bMB2HeaterCoolingSol"), &m_stMB2HeaterCoolingSol.bHMI_Status);

		//RegVariable(_T("HMI_bMB3HeaterCoolingSol"), &m_stMB3HeaterCoolingSol.bHMI_Status);
		//RegVariable(_T("HMI_bMB4HeaterCoolingSol"), &m_stMB4HeaterCoolingSol.bHMI_Status);

		///////////////////////////////////////////////////
		// commands ///////////////////////////////////////
		///////////////////////////////////////////////////
		//I/Os
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "TogglePB1HeaterCoolingSol", HMI_TogglePB1HeaterCoolingSol);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "TogglePB2HeaterCoolingSol", HMI_TogglePB2HeaterCoolingSol);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "ToggleMB1HeaterCoolingSol", HMI_ToggleMB1HeaterCoolingSol);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "ToggleMB2HeaterCoolingSol", HMI_ToggleMB2HeaterCoolingSol);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "ToggleMB3HeaterCoolingSol", HMI_ToggleMB3HeaterCoolingSol);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "ToggleMB4HeaterCoolingSol", HMI_ToggleMB4HeaterCoolingSol);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "ToggleMB2HeaterCoolingSol", HMI_ToggleMB2HeaterCoolingSol);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "PowerOnAllHeater", HMI_PowerOnAllHeater);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "PowerOnHeater", HMI_PowerOnHeater);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "SetHeaterTemp", HMI_SetHeaterTemp);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "SetTempTol", HMI_SetTempTol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "SetBondTemp", HMI_SetBondTemp);

		//AutoTune
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "SetHeaterATPara", HMI_SetHeaterATPara);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "TurnOnHeaterAT", HMI_TurnOnHeaterAT);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "LearnHODMask", HMI_LearnHODMask);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "LearnRMSMask", HMI_LearnRMSMask);

		//Temp. Monitor Related
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "SetMonTemp", HMI_SetMonTemp);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "SetMonOffset", HMI_SetMonOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "SetMonTol", HMI_SetMonTol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "SetAlarmBand", HMI_SetAlarmBand);

		// temperature calib.
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "TempInsertData", HMI_TempInsertData);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "TempResetData", HMI_TempResetData);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "TempResetAllData", HMI_TempResetAllData);

		// Set Temp Lmt
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "SetTempLmt", HMI_SetTempLmt);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "SetHODThreshold", HMI_SetHODThreshold);	
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "PrintLitecInformation", HMI_PrintLitecInformation);

		// Set Dynamic Output Limit
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "SetDynOutputLmt", HMI_SetDynOutputLmt); //20130912

		// Reset Channel 
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "ResetAllHeater", HMI_ResetAllHeater);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + "ResetHeaterChannel", HMI_ResetHeaterChannel);
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
}

/////////////////////////////////////////////////////////////////
//Wrapped Functions
/////////////////////////////////////////////////////////////////
//INT CTempDisplay::SetCoolingSol(CSoInfo *stTempSo, BOOL bMode, BOOL bWait)
//{
//	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
//	CPB1 *pCPB1 = (CPB1*)m_pModule->GetStation("PB1");
//	CPB2 *pCPB2 = (CPB2*)m_pModule->GetStation("PB2");
//	CMB1 *pCMB1 = (CMB1*)m_pModule->GetStation("MB1");
//	CMB2 *pCMB2 = (CMB2*)m_pModule->GetStation("MB2");
//
//	INT nResult = GMP_SUCCESS;
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
//	if (
//		!pAppMod->IsInitNuDrive() ||
//		(stTempSo->GetName() == "PB1CoolingSol"		&& !pCPB1->m_fHardware) ||
//		(stTempSo->GetName() == "PB2CoolingSol"		&& !pCPB2->m_fHardware) ||
//		(stTempSo->GetName() == "MBUnit1CoolingSol"	&& !pCMB1->m_fHardware) ||
//		(stTempSo->GetName() == "MBUnit2CoolingSol"	&& !pCMB2->m_fHardware)
//	   )
//	{
//		return GMP_SUCCESS;
//	}
//
//	try
//	{
//		if (bMode)
//		{
//			stTempSo->GetGmpPort().On();
//			stTempSo->bHMI_Status = TRUE;
//		}
//		else
//		{
//			stTempSo->GetGmpPort().Off();
//			stTempSo->bHMI_Status = FALSE;
//		}
//	}
//	catch (CAsmException e)
//	{
//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//		DisplayException(e);
//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//	}
//	
//	return GMP_SUCCESS;
//}

INT CTempDisplay::PowerOnHeater(INT num, BOOL bMode)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	INT nResult = HEATER_OP_OK;

	m_bHeaterFailed[num] = FALSE; //20130627

	if (
		(nResult = pCLitecCtrl->SetLitecTemp(num, m_dHeaterSetTemp[num])) != HEATER_OP_OK ||
		(nResult = pCLitecCtrl->PowerOnLitec(num, bMode) != HEATER_OP_OK)
	   )
	{
		return HEATER_FAILED;
	}
	else
	{
		if (bMode)
		{
			m_clkHeaterOnTime[num] = clock(); //20150309
		}
		else
		{
			m_clkHeaterOnTime[num] = -1;
		}
		return HEATER_OP_OK;
	}

	return nResult;
}

BOOL CTempDisplay::PowerOnAllHeater(BOOL bMode) //20120305 problem 13 air supply error
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	INT i;	
	BOOL bOpDone = TRUE;

	m_bHeatersOn = bMode;	// 20140918 Yip: Add Flag To Toggle All Heaters Power On/Off

	// Avoid Display Error Message when Press EMO
	if (!pCLitecCtrl->m_fHardware)
	{
		return TRUE;
	}

	// Set Heater SetTemp
	// Power On Heater
	for (i = 0; i < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; i++)
	{
		if (bMode && m_bHeaterExist[i])
		{
			SetHeaterTemp(i, m_dHeaterSetTemp[i]);
			PowerOnHeater(i, bMode);		
		}
		//PowerOnHeater(i, bMode);		
	}


	//turn on cooling if turning heater on
	//if (bMode)
	//{
	//	SetCoolingSol(&m_stPB1HeaterCoolingSol, ON);
	//	SetCoolingSol(&m_stPB2HeaterCoolingSol, ON);
	//	SetCoolingSol(&m_stMB1HeaterCoolingSol, ON);
	//	SetCoolingSol(&m_stMB2HeaterCoolingSol, ON);
	//	SetCoolingSol(&m_stMB3HeaterCoolingSol, ON);
	//	SetCoolingSol(&m_stMB4HeaterCoolingSol, ON);
	//}

	
	return TRUE;
}

INT CTempDisplay::GetHeaterTemp(INT num, DOUBLE *p_dTemp)
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	INT nResult = HEATER_OP_OK;

	nResult = pCLitecCtrl->GetLitecTemp(num, p_dTemp);

	return nResult;
}


INT CTempDisplay::SetHeaterTemp(INT num, DOUBLE dTemp)
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");
	INT nResult = HEATER_OP_OK;

	nResult = pCLitecCtrl->SetLitecTemp(num, dTemp);

	return nResult;
}

INT CTempDisplay::SetHeaterTempFromData()
{
	//CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);


	INT i;

	for (i = 0; i < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; i++)
	{
		m_bValidTemp[i] = FALSE;
	}

	//if (pCLitecCtrl->m_bLitecInit) 
	if (pAppMod->m_bAllLitecInited) 
	{
		for (i = 0; i < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; i++)
		{
			if (m_bHeaterExist[i])
			{
				if (SetHeaterTemp(i, m_dHeaterSetTemp[i]) == HEATER_FAILED)
				{
					return HEATER_FAILED;
				}
			}
		}
	}

	for (i = 0; i < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; i++)
	{
		if (m_bHeaterExist[i])
		{
			m_bValidTemp[i] = SetTempToBondTemp(m_dHeaterSetTemp[i], m_stTempCalib[i], &m_dBondPtTemp[i]);
		}
	}

	return HEATER_OP_OK;
}

INT CTempDisplay::TurnOnHeaterAT(INT num, BOOL bMode)
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	INT nResult = HEATER_OP_OK;

	nResult = pCLitecCtrl->TurnOnLitecAT(num, bMode);
		
	if (bMode && m_bHeaterExist[num])
	{
		LearnHODMask(num);
	}

	return nResult;
}

INT CTempDisplay::LearnHODMask(INT num)
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	CString szMsg				= "";
	INT		nResult = HEATER_OP_OK;

	if (HEATER_OP_OK != pCLitecCtrl->LearnLitecHODMask(num))
	{
		szMsg.Format("Channel %d Learn HOD Mark Failed!", num);
		DisplayMessage(szMsg);
		return HEATER_FAILED;
	}

	return HEATER_OP_OK;
}

INT CTempDisplay::LearnRMSMask(INT num)
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	CString szMsg = "";

	if (HEATER_OP_OK != pCLitecCtrl->LearnLitecRMSMask(num, ON))
	{
		szMsg.Format("Channel %d Learn RMS Mark Failed!", num);
		DisplayMessage(szMsg);
		return HEATER_FAILED;
	}

	return HEATER_OP_OK;
}

BOOL CTempDisplay::CheckHeaterTempReady()
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	CACF1 *pCACF1 = (CACF1*)m_pModule->GetStation("ACF1");
	//CACF2 *pCACF2 = (CACF2*)m_pModule->GetStation("ACF2");
	//CPB1 *pCPB1 = (CPB1*)m_pModule->GetStation("PB1");
	//CPB2 *pCPB2 = (CPB2*)m_pModule->GetStation("PB2");
	//CMB1 *pCMB1 = (CMB1*)m_pModule->GetStation("MB1");
	//CMB2 *pCMB2 = (CMB2*)m_pModule->GetStation("MB2");

	BOOL bError = FALSE; //20130411

	if (AutoMode == AUTO_BOND || AutoMode == TEST_BOND)
	{
		DOUBLE dDiff = 0.0;
		CString szMsg = "";

		if (
			!pCACF1->m_bPressCycle && /*!pCACF2->m_bPressCycle && !pCPB1->m_bPressCycle && !pCPB2->m_bPressCycle && 
			!pCMB1->m_bPressCycle && !pCMB2->m_bPressCycle &&*/ pCACF1->m_bModSelected //&&
			//g_lGlassInputMode != GLASS_2_INPUT //20130508
		   )
		{
			if (!m_bHeaterOn[ACF1_CH1])
			{
				DisplayMessage("ACF1_CH1 Not On Error");
				//SetError(IDS_TEMP_CTRL_NOT_ON);
				SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(ACF1_CH1)); //20140401
				SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(ACF1_CH1)); //20130222
				bError = TRUE; //20130411
				//return FALSE;
			}
			else if ((dDiff = fabs(m_dHeaterTemp[ACF1_CH1] - m_dHeaterSetTemp[ACF1_CH1])) > TEMP_ERROR_TOL)
			{
				szMsg.Format("ACF 1 Head dDiff: %.5f (Temp:%.2f, setTemp%.2f)", dDiff, m_dHeaterTemp[ACF1_CH1], 
							 m_dHeaterSetTemp[ACF1_CH1]);
				DisplayMessage(szMsg);
				//SetError(IDS_TEMP_CTRL_CTRL_TEMP_NOT_READY_ERR);
				SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(ACF1_CH1)); //20140401
				SetError(IDS_TEMP_CTRL_HEATER1_CTRL_TEMP_NOT_READY_ERR + pCLitecCtrl->GetLitecErrNum(ACF1_CH1)); //20130222 ACF1
				bError = TRUE; //20130411
				//return FALSE;
			}
		}
		//if (
		//	!pCACF1->m_bPressCycle && !pCACF2->m_bPressCycle && !pCPB1->m_bPressCycle && !pCPB2->m_bPressCycle && 
		//	!pCMB1->m_bPressCycle && !pCMB2->m_bPressCycle && pCACF2->m_bModSelected &&
		//	g_lGlassInputMode != GLASS_1_INPUT //20130508
		//   )
		//{
		//	if (!m_bHeaterOn[ACF2_CH2])
		//	{
		//		DisplayMessage("ACF2_CH2 Not On Error");
		//			//SetError(IDS_TEMP_CTRL_NOT_ON);
		//		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(ACF2_CH2)); //20140401
		//		SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(ACF2_CH2)); //20130222
		//		bError = TRUE; //20130411
		//			//return FALSE;
		//	}
		//	else if ((dDiff = fabs(m_dHeaterTemp[ACF2_CH2] - m_dHeaterSetTemp[ACF2_CH2])) > TEMP_ERROR_TOL)
		//	{
		//		szMsg.Format("ACF 2 Head dDiff: %.5f (Temp:%.2f, setTemp%.2f)", dDiff, m_dHeaterTemp[ACF2_CH2], 
		//					 m_dHeaterSetTemp[ACF2_CH2]);
		//		DisplayMessage(szMsg);
		//			//SetError(IDS_TEMP_CTRL_CTRL_TEMP_NOT_READY_ERR);
		//		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(ACF2_CH2)); //20140401
		//		SetError(IDS_TEMP_CTRL_HEATER1_CTRL_TEMP_NOT_READY_ERR + pCLitecCtrl->GetLitecErrNum(ACF2_CH2)); //20130222 ACF2
		//		bError = TRUE; //20130411
		//			//return FALSE;
		//	}			
		//}
		//if (
		//	!pCACF1->m_bPressCycle && !pCACF2->m_bPressCycle && !pCPB1->m_bPressCycle && !pCPB2->m_bPressCycle && 
		//	!pCMB1->m_bPressCycle && !pCMB2->m_bPressCycle && pCPB1->m_bModSelected &&
		//	g_lGlassInputMode != GLASS_2_INPUT //20130508
		//   )
		//{
		//	if (!m_bHeaterOn[PB1_CH3])
		//	{
		//		DisplayMessage("PB1_CH3 Not On Error");
		//		//SetError(IDS_TEMP_CTRL_NOT_ON);
		//		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(PB1_CH3)); //20140401
		//		SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(PB1_CH3)); //20130222
		//		bError = TRUE; //20130411
		//		//return FALSE;
		//	}
		//	else if ((dDiff = fabs(m_dHeaterTemp[PB1_CH3] - m_dHeaterSetTemp[PB1_CH3])) > TEMP_ERROR_TOL)
		//	{
		//		szMsg.Format("PreBond 1 Head dDiff: %.5f (Temp:%.2f, setTemp%.2f)", dDiff, m_dHeaterTemp[PB1_CH3], 
		//					 m_dHeaterSetTemp[PB1_CH3]);
		//		DisplayMessage(szMsg);
		//		//SetError(IDS_TEMP_CTRL_CTRL_TEMP_NOT_READY_ERR);
		//		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(PB1_CH3)); //20140401
		//		SetError(IDS_TEMP_CTRL_HEATER1_CTRL_TEMP_NOT_READY_ERR + pCLitecCtrl->GetLitecErrNum(PB1_CH3)); //20130222 PB1
		//		bError = TRUE; //20130411
		//		//return FALSE;
		//	}
		//}
		//if (
		//	!pCACF1->m_bPressCycle && !pCACF2->m_bPressCycle && !pCPB1->m_bPressCycle && !pCPB2->m_bPressCycle && 
		//	!pCMB1->m_bPressCycle && !pCMB2->m_bPressCycle && pCPB2->m_bModSelected &&
		//	g_lGlassInputMode != GLASS_1_INPUT //20130508
		//   )
		//{
		//	if (!m_bHeaterOn[PB2_CH4])
		//	{
		//		DisplayMessage("PB2_CH4 Not On Error");
		//		//SetError(IDS_TEMP_CTRL_NOT_ON);
		//		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(PB2_CH4)); //20140401
		//		SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(PB2_CH4)); //20130222
		//		bError = TRUE; //20130411
		//		//return FALSE;
		//	}
		//	else if ((dDiff = fabs(m_dHeaterTemp[PB2_CH4] - m_dHeaterSetTemp[PB2_CH4])) > TEMP_ERROR_TOL)
		//	{
		//		szMsg.Format("PreBond 2 Head dDiff: %.5f (Temp:%.2f, setTemp%.2f)", dDiff, m_dHeaterTemp[PB2_CH4], 
		//					 m_dHeaterSetTemp[PB2_CH4]);
		//		DisplayMessage(szMsg);
		//		//SetError(IDS_TEMP_CTRL_CTRL_TEMP_NOT_READY_ERR);
		//		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(PB2_CH4)); //20140401
		//		SetError(IDS_TEMP_CTRL_HEATER1_CTRL_TEMP_NOT_READY_ERR + pCLitecCtrl->GetLitecErrNum(PB2_CH4)); //20130222 PB2
		//		bError = TRUE; //20130411
		//		//return FALSE;
		//	}
		//}
		//if (
		//	!pCACF1->m_bPressCycle && !pCACF2->m_bPressCycle && !pCPB1->m_bPressCycle && !pCPB2->m_bPressCycle && 
		//	!pCMB1->m_bPressCycle && !pCMB2->m_bPressCycle && pCMB1->m_bModSelected
		//   )
		//{
		//	//LEFT Heater
		//	//MB1 Head
		//	if (g_lGlassInputMode != GLASS_2_INPUT) //20130508
		//	{
		//		if (!m_bHeaterOn[MB1_CTRL_CH1])
		//		{
		//			DisplayMessage("MB1_CTRL_CH1 Not On Error");
		//			//SetError(IDS_TEMP_CTRL_NOT_ON);
		//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB1_CTRL_CH1)); //20140401
		//			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB1_CTRL_CH1)); //20130222
		//			bError = TRUE; //20130411
		//			//return FALSE;
		//		}
		//		else if ((dDiff = fabs(m_dHeaterTemp[MB1_CTRL_CH1] - m_dHeaterSetTemp[MB1_CTRL_CH1])) > TEMP_ERROR_TOL)
		//		{
		//			szMsg.Format("MainBond1 Left Head dDiff: %.5f (Temp:%.2f, setTemp%.2f)", dDiff, m_dHeaterTemp[MB1_CTRL_CH1], 
		//						 m_dHeaterSetTemp[MB1_CTRL_CH1]);
		//			DisplayMessage(szMsg);
		//			//SetError(IDS_TEMP_CTRL_CTRL_TEMP_NOT_READY_ERR);
		//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB1_CTRL_CH1)); //20140401
		//			SetError(IDS_TEMP_CTRL_HEATER1_CTRL_TEMP_NOT_READY_ERR + pCLitecCtrl->GetLitecErrNum(MB1_CTRL_CH1)); //20130222 MB1 head
		//			bError = TRUE; //20130411
		//			//return FALSE;
		//		}

		//		//MB1 WH Base
		//		if (!m_bHeaterOn[MB1_WH_CH1])
		//		{
		//			DisplayMessage("MB1_WH_CH1 Not On Error");
		//			//SetError(IDS_TEMP_CTRL_NOT_ON);
		//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB1_WH_CH1)); //20140401
		//			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB1_WH_CH1)); //20130222
		//			bError = TRUE; //20130411
		//			//return FALSE;
		//		}
		//		else if ((dDiff = fabs(m_dHeaterTemp[MB1_WH_CH1] - m_dHeaterSetTemp[MB1_WH_CH1])) > TEMP_ERROR_TOL)
		//		{
		//			szMsg.Format("MainBond1 Left Base dDiff: %.5f (Temp:%.2f, setTemp%.2f)", dDiff, m_dHeaterTemp[MB1_WH_CH1], 
		//						 m_dHeaterSetTemp[MB1_WH_CH1]);
		//			DisplayMessage(szMsg);
		//			//SetError(IDS_TEMP_CTRL_CTRL_TEMP_NOT_READY_ERR);
		//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB1_WH_CH1)); //20140401
		//			SetError(IDS_TEMP_CTRL_HEATER1_CTRL_TEMP_NOT_READY_ERR + pCLitecCtrl->GetLitecErrNum(MB1_WH_CH1)); //20130222 MB1 base
		//			bError = TRUE; //20130411
		//			//return FALSE;
		//		}
		//	}

		//	//RIGHT Heater, MB2 head
		//	if (g_lGlassInputMode != GLASS_1_INPUT) //20130508
		//	{
		//		if (!m_bHeaterOn[MB2_CTRL_CH2])
		//		{
		//			DisplayMessage("MB2_CTRL_CH2 Not On Error");
		//			//SetError(IDS_TEMP_CTRL_NOT_ON);
		//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB2_CTRL_CH2)); //20140401
		//			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB2_CTRL_CH2)); //20130222
		//			bError = TRUE; //20130411
		//			//return FALSE;
		//		}
		//		else if ((dDiff = fabs(m_dHeaterTemp[MB2_CTRL_CH2] - m_dHeaterSetTemp[MB2_CTRL_CH2])) > TEMP_ERROR_TOL)
		//		{
		//			szMsg.Format("MainBond1 Right Head dDiff: %.5f (Temp:%.2f, setTemp%.2f)", dDiff, m_dHeaterTemp[MB2_CTRL_CH2], 
		//						 m_dHeaterSetTemp[MB2_CTRL_CH2]);
		//			DisplayMessage(szMsg);
		//			//SetError(IDS_TEMP_CTRL_CTRL_TEMP_NOT_READY_ERR);
		//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB2_CTRL_CH2)); //20140401
		//			SetError(IDS_TEMP_CTRL_HEATER1_CTRL_TEMP_NOT_READY_ERR + pCLitecCtrl->GetLitecErrNum(MB2_CTRL_CH2)); //20130222 MB2 head
		//			bError = TRUE; //20130411
		//			//return FALSE;
		//		}

		//		//MB2 WH Base
		//		if (!m_bHeaterOn[MB2_WH_CH2])
		//		{
		//			DisplayMessage("MB2_WH_CH2 Not On Error");
		//			//SetError(IDS_TEMP_CTRL_NOT_ON);
		//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB2_WH_CH2)); //20140401
		//			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB2_WH_CH2)); //20130222
		//			bError = TRUE; //20130411
		//			//return FALSE;
		//		}
		//		else if ((dDiff = fabs(m_dHeaterTemp[MB2_WH_CH2] - m_dHeaterSetTemp[MB2_WH_CH2])) > TEMP_ERROR_TOL)
		//		{
		//			szMsg.Format("MainBond1 Right Base dDiff: %.5f (Temp:%.2f, setTemp%.2f)", dDiff, m_dHeaterTemp[MB2_WH_CH2], 
		//						 m_dHeaterSetTemp[MB2_WH_CH2]);
		//			DisplayMessage(szMsg);
		//			//SetError(IDS_TEMP_CTRL_CTRL_TEMP_NOT_READY_ERR);
		//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB2_WH_CH2)); //20140401
		//			SetError(IDS_TEMP_CTRL_HEATER1_CTRL_TEMP_NOT_READY_ERR + pCLitecCtrl->GetLitecErrNum(MB2_WH_CH2)); //20130222 MB2 base
		//			bError = TRUE; //20130411
		//			//return FALSE;
		//		}
		//	}
		//}
		//if (
		//	!pCACF1->m_bPressCycle && !pCACF2->m_bPressCycle && !pCPB1->m_bPressCycle && !pCPB2->m_bPressCycle && 
		//	!pCMB1->m_bPressCycle && !pCMB2->m_bPressCycle && pCMB2->m_bModSelected
		//   )
		//{
		//	//LEFT Heater
		//	//MB3 Head
		//	if (g_lGlassInputMode != GLASS_2_INPUT) //20130508
		//	{
		//		if (!m_bHeaterOn[MB3_CTRL_CH1])
		//		{
		//			DisplayMessage("MB3_CTRL_CH1 Not On Error");
		//			//SetError(IDS_TEMP_CTRL_NOT_ON);
		//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB3_CTRL_CH1)); //20140401
		//			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB3_CTRL_CH1)); //20130222
		//			bError = TRUE; //20130411
		//			//return FALSE;
		//		}
		//		else if ((dDiff = fabs(m_dHeaterTemp[MB3_CTRL_CH1] - m_dHeaterSetTemp[MB3_CTRL_CH1])) > TEMP_ERROR_TOL)
		//		{
		//			szMsg.Format("MainBond2 Left Head dDiff: %.5f (Temp:%.2f, setTemp%.2f)", dDiff, m_dHeaterTemp[MB3_CTRL_CH1], 
		//						 m_dHeaterSetTemp[MB3_CTRL_CH1]);
		//			DisplayMessage(szMsg);
		//			//SetError(IDS_TEMP_CTRL_CTRL_TEMP_NOT_READY_ERR);
		//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB3_CTRL_CH1)); //20140401
		//			SetError(IDS_TEMP_CTRL_HEATER1_CTRL_TEMP_NOT_READY_ERR + pCLitecCtrl->GetLitecErrNum(MB3_CTRL_CH1)); //20130222 MB3 head
		//			bError = TRUE; //20130411
		//			//return FALSE;
		//		}

		//		//MB3 WH Base
		//		if (!m_bHeaterOn[MB3_WH_CH3])
		//		{
		//			DisplayMessage("MB3_WH_CH3 Not On Error");
		//			//SetError(IDS_TEMP_CTRL_NOT_ON);
		//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB3_WH_CH3)); //20140401
		//			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB3_WH_CH3)); //20130222
		//			bError = TRUE; //20130411
		//			//return FALSE;
		//		}
		//		else if ((dDiff = fabs(m_dHeaterTemp[MB3_WH_CH3] - m_dHeaterSetTemp[MB3_WH_CH3])) > TEMP_ERROR_TOL)
		//		{
		//			szMsg.Format("MainBond2 Left Base dDiff: %.5f (Temp:%.2f, setTemp%.2f)", dDiff, m_dHeaterTemp[MB3_WH_CH3], 
		//						 m_dHeaterSetTemp[MB3_WH_CH3]);
		//			DisplayMessage(szMsg);
		//			//SetError(IDS_TEMP_CTRL_CTRL_TEMP_NOT_READY_ERR);
		//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB3_WH_CH3)); //20140401
		//			SetError(IDS_TEMP_CTRL_HEATER1_CTRL_TEMP_NOT_READY_ERR + pCLitecCtrl->GetLitecErrNum(MB3_WH_CH3)); //20130222 MB3 base
		//			bError = TRUE; //20130411
		//			//return FALSE;
		//		}
		//	}

		//	//RIGHT Heater, MB4 head
		//	if (g_lGlassInputMode != GLASS_1_INPUT) //20130508
		//	{
		//		if (!m_bHeaterOn[MB4_CTRL_CH2])
		//		{
		//			DisplayMessage("MB4_CTRL_CH2 Not On Error");
		//			//SetError(IDS_TEMP_CTRL_NOT_ON);
		//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB4_CTRL_CH2)); //20140401
		//			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB4_CTRL_CH2)); //20130222
		//			bError = TRUE; //20130411
		//			//return FALSE;
		//		}
		//		else if ((dDiff = fabs(m_dHeaterTemp[MB4_CTRL_CH2] - m_dHeaterSetTemp[MB4_CTRL_CH2])) > TEMP_ERROR_TOL)
		//		{
		//			szMsg.Format("MainBond2 Right Head dDiff: %.5f (Temp:%.2f, setTemp%.2f)", dDiff, m_dHeaterTemp[MB4_CTRL_CH2], 
		//						 m_dHeaterSetTemp[MB4_CTRL_CH2]);
		//			DisplayMessage(szMsg);
		//			//SetError(IDS_TEMP_CTRL_CTRL_TEMP_NOT_READY_ERR);
		//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB4_CTRL_CH2)); //20140401
		//			SetError(IDS_TEMP_CTRL_HEATER1_CTRL_TEMP_NOT_READY_ERR + pCLitecCtrl->GetLitecErrNum(MB4_CTRL_CH2)); //20130222 MB4 head
		//			bError = TRUE; //20130411
		//			//return FALSE;
		//		}

		//		//MB4 Wh Base
		//		if (!m_bHeaterOn[MB4_WH_CH4])
		//		{
		//			DisplayMessage("MB4_WH_CH4 Not On Error");
		//			//SetError(IDS_TEMP_CTRL_NOT_ON);
		//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB4_WH_CH4)); //20140401
		//			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB4_WH_CH4)); //20130222
		//			bError = TRUE; //20130411
		//			//return FALSE;
		//		}
		//		else if ((dDiff = fabs(m_dHeaterTemp[MB4_WH_CH4] - m_dHeaterSetTemp[MB4_WH_CH4])) > TEMP_ERROR_TOL)
		//		{
		//			szMsg.Format("MainBond2 Right Base dDiff: %.5f (Temp:%.2f, setTemp%.2f)", dDiff, m_dHeaterTemp[MB4_WH_CH4], 
		//						 m_dHeaterSetTemp[MB4_WH_CH4]);
		//			DisplayMessage(szMsg);
		//			//SetError(IDS_TEMP_CTRL_CTRL_TEMP_NOT_READY_ERR);
		//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB4_WH_CH4)); //20140401
		//			SetError(IDS_TEMP_CTRL_HEATER1_CTRL_TEMP_NOT_READY_ERR + pCLitecCtrl->GetLitecErrNum(MB4_WH_CH4)); //20130222 MB4 base
		//			bError = TRUE; //20130411
		//			//return FALSE;
		//		}
			//}
		//}
	}

	if (bError) //20130411
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CTempDisplay::CheckHeaterInit()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	BOOL bResult = HEATER_OP_OK;

	//bResult = pCLitecCtrl->m_bLitecInit;
	
	for (INT ii = 0; ii < NUM_OF_LITEC; ii++)
	{
		if (pCLitecCtrl->m_bLitecInit[ii] == FALSE)
		{
			bResult = FALSE;
			break;
		}
		else
		{
			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL CTempDisplay::CheckAllHeaterOn() //20130227
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");
	//CString szModuleName = "";
	//CString szMsg = "";

	BOOL bResult = TRUE;

	for (INT ii = 0; ii < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; ii++)
	{
		if (!m_bHeaterOn[ii] && m_bHeaterExist[ii])
		{
			//ConvertToModuleName(ii, &szModuleName);
			//szMsg.Format("%s Heater Not On Error", szModuleName);
			//DisplayMessage(szMsg);
			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(ii)); //20140401
			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(ii));
			bResult = FALSE;
			break;
		}
	}
	if (!bResult)
	{
		DisplayMessage("Not All heater are On Error");
	}
	return bResult;
}

BOOL CTempDisplay::IsAllHeaterOnByStationName(CString &szStationName) //20130424
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	//if (szStationName == "MB1")
	//{ 
	////channel: MB1, MB2, MB1WH, MB2WH
	//	if (m_bHeaterOn[MB1_CTRL_CH1] && 
	//		m_bHeaterOn[MB2_CTRL_CH2] &&
	//		m_bHeaterOn[MB1_WH_CH1] &&
	//		m_bHeaterOn[MB2_WH_CH2])
	//	{
	//		return TRUE;
	//	}
	//	else
	//	{
	//		if (!m_bHeaterOn[MB1_CTRL_CH1])
	//		{
	//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB1_CTRL_CH1)); //20140401
	//			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB1_CTRL_CH1));
	//		}
	//		if (!m_bHeaterOn[MB2_CTRL_CH2])
	//		{
	//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB2_CTRL_CH2)); //20140401
	//			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB2_CTRL_CH2));
	//		}
	//		if (!m_bHeaterOn[MB1_WH_CH1])
	//		{
	//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB1_WH_CH1)); //20140401
	//			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB1_WH_CH1));
	//		}
	//		if (!m_bHeaterOn[MB2_WH_CH2])
	//		{
	//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB2_WH_CH2)); //20140401
	//			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB2_WH_CH2));
	//		}
	//	}
	//}
	//else if (szStationName == "MB2")
	//{ 
	////Channel: MB3, MB4, MB3WH, MB4WH
	//	if (m_bHeaterOn[MB3_CTRL_CH1] && 
	//		m_bHeaterOn[MB4_CTRL_CH2] &&
	//		m_bHeaterOn[MB3_WH_CH3] &&
	//		m_bHeaterOn[MB4_WH_CH4])
	//	{
	//		return TRUE;
	//	}
	//	else
	//	{
	//		if (!m_bHeaterOn[MB3_CTRL_CH1])
	//		{
	//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB3_CTRL_CH1)); //20140401
	//			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB3_CTRL_CH1));
	//		}
	//		if (!m_bHeaterOn[MB4_CTRL_CH2])
	//		{
	//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB4_CTRL_CH2)); //20140401
	//			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB4_CTRL_CH2));
	//		}
	//		if (!m_bHeaterOn[MB3_WH_CH3])
	//		{
	//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB3_WH_CH3)); //20140401
	//			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB3_WH_CH3));
	//		}
	//		if (!m_bHeaterOn[MB4_WH_CH4])
	//		{
	//			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB4_WH_CH4)); //20140401
	//			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB4_WH_CH4));
	//		}
	//	}
	//}
	//else if (szStationName == "PB1")
	//{ 
	////Channel 5
	//	if (m_bHeaterOn[PB1_CH3])
	//	{
	//		return TRUE;
	//	}
	//	else
	//	{
	//		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(PB1_CH3)); //20140401
	//		SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(PB1_CH3));
	//	}
	//}
	//else if (szStationName == "PB2")
	//{ 
	////Channel 11
	//	if (m_bHeaterOn[PB2_CH4])
	//	{
	//		return TRUE;
	//	}
	//	else
	//	{
	//		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(PB2_CH4)); //20140401
	//		SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(PB2_CH4));
	//	}
	//}
	if (szStationName == "ACF1")
	{ 
	//Channel 4
		if (m_bHeaterOn[ACF1_CH1])
		{
			return TRUE;
		}
		else
		{
			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(ACF1_CH1)); //20140401
			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(ACF1_CH1));
		}
	}
	//else if (szStationName == "ACF2")
	//{ 
	////Channel 10
	//	if (m_bHeaterOn[ACF2_CH2])
	//	{
	//		return TRUE;
	//	}
	//	else
	//	{
	//		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(ACF2_CH2)); //20140401
	//		SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(ACF2_CH2));
	//	}
	//}
	return FALSE;
}

BOOL CTempDisplay::CheckHeaterPowerForAutobond() //20130430
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	BOOL bResult = TRUE;

	switch (g_lGlassInputMode)
	{
	case GLASS_1_INPUT: //ACF1, PB1, MB1, MB3
		if (!m_bHeaterOn[ACF1_CH1])
		{
			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(ACF1_CH1)); //20140401
			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(ACF1_CH1));
			bResult = FALSE;
		}
		//if (!m_bHeaterOn[PB1_CH3])
		//{
		//	SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(PB1_CH3)); //20140401
		//	SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(PB1_CH3));
		//	bResult = FALSE;
		//}
		//if (!m_bHeaterOn[MB1_CTRL_CH1]) // MB1 head
		//{
		//	SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB1_CTRL_CH1)); //20140401
		//	SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB1_CTRL_CH1));
		//	bResult = FALSE;
		//}
		//if (!m_bHeaterOn[MB1_WH_CH1]) //MB1 WH
		//{
		//	SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB1_WH_CH1)); //20140401
		//	SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB1_WH_CH1));
		//	bResult = FALSE;
		//}
		//if (!m_bHeaterOn[MB3_CTRL_CH1]) //MB3 head
		//{
		//	SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB3_CTRL_CH1)); //20140401
		//	SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB3_CTRL_CH1));
		//	bResult = FALSE;
		//}
		//if (!m_bHeaterOn[MB3_WH_CH3]) //MB3 WH
		//{
		//	SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB3_WH_CH3)); //20140401
		//	SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB3_WH_CH3));
		//	bResult = FALSE;
		//}
		break;

	//case GLASS_2_INPUT: //ACF2, PB2, MB2, MB4
		if (!m_bHeaterOn[ACF1_CH1])
		{
			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(ACF1_CH1)); //20140401
			SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(ACF1_CH1));
			bResult = FALSE;
		}
	//	if (!m_bHeaterOn[PB2_CH4]) //PB2
	//	{
	//		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(PB2_CH4)); //20140401
	//		SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(PB2_CH4));
	//		bResult = FALSE;
	//	}
	//	if (!m_bHeaterOn[MB2_CTRL_CH2]) //MB2 head
	//	{
	//		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB2_CTRL_CH2)); //20140401
	//		SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB2_CTRL_CH2));
	//		bResult = FALSE;
	//	}
	//	if (!m_bHeaterOn[MB2_WH_CH2]) //MB2 WH
	//	{
	//		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB2_WH_CH2)); //20140401
	//		SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB2_WH_CH2));
	//		bResult = FALSE;
	//	}
	//	if (!m_bHeaterOn[MB4_CTRL_CH2]) //MB4 head
	//	{
	//		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB4_CTRL_CH2)); //20140401
	//		SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB4_CTRL_CH2));
	//		bResult = FALSE;
	//	}
	//	if (!m_bHeaterOn[MB4_WH_CH4]) //MB4 WH
	//	{
	//		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(MB4_WH_CH4)); //20140401
	//		SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(MB4_WH_CH4));
	//		bResult = FALSE;
	//	}
	//	break;

	default:
		for (INT ii = 0; ii < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; ii++)
		{
			if (m_bHeaterExist[ii] && !m_bHeaterOn[ii])
			{
				SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(ii)); //20140401
				SetError(IDS_TEMP_CTRL_HEATER1_NOT_ON + pCLitecCtrl->GetLitecErrNum(ii));
				bResult = FALSE;
				break;
			}
		}
		break;
	}

	//if (!bResult)
	//{
	//	DisplayMessage("Heater Not On Error");
	//}
	return bResult;
}

BOOL CTempDisplay::CheckHeaterMalfunctionTimeout(INT nCurindex)
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	CString szMsg = "";

	// Check Heater Malfunction Timeout
	// When m_bMonTemp is enabled
	if (
		!m_bHeaterFailed[nCurindex] && m_bMonTemp && !m_bMonFailed[nCurindex] && m_dMonTol[nCurindex] != 0.0 && 
#if 1 //20150309
		m_clkHeaterOnTime[nCurindex] != -1 && (clock() - m_clkHeaterOnTime[nCurindex]) / CLOCKS_PER_SEC > HEATER_MALFUNC_ON_TO_CHECK_DELAY &&
#endif
		(
			m_dHeaterTemp[nCurindex] >= (/*0.9*/ 0.95 * m_dHeaterSetTemp[nCurindex]) || //20130413
			m_dMonTemp[nCurindex] >= (/*0.9*/ 0.95 * m_dHeaterSetTemp[nCurindex])
		) 	// start check once temp reaches 90% of set temp
	   )
	{
		if (fabs(m_dMonTemp[nCurindex] - m_dHeaterTemp[nCurindex]) > m_dMonTol[nCurindex])
		{
			if ((LONG)(GetTickCount() - m_dwFaultTimerStart[nCurindex]) > HEATER_MALFUNC_TIMOUT)
			{				
				m_bMonFailed[nCurindex] = TRUE;
				PowerOnHeater(nCurindex, OFF);
				DisplayMessage("****************************************************");
				//SetError(IDS_TEMP_CTRL_MON_TEMP_OUT_OF_RANGE_ERR);
				//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + nCurindex);
				SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(m_nCurIndex)); //20140401
				SetError(IDS_TEMP_CTRL_HEATER1_MON_TEMP_OUT_OF_RANGE_ERR + pCLitecCtrl->GetLitecErrNum(m_nCurIndex)); //20130222
				DisplayMessage("Heater Malfunction Error");
				if (m_dMonTemp[nCurindex] > m_dHeaterTemp[nCurindex]) //20130314
				{
					DisplayMessage("Heater temperture is lower than monitor temperature");
				}
				else
				{
					DisplayMessage("Heater temperture is higher than monitor temperature");
				}
				szMsg.Format("Heater Ch%d Cur Temp = %.2f, | Mon Temp = %.2f", nCurindex + 1, m_dHeaterTemp[nCurindex], m_dMonTemp[nCurindex]);
				DisplayMessage(szMsg);
				DisplayMessage("****************************************************");
				return TRUE;
			}
		}
		else
		{
			m_dwFaultTimerStart[nCurindex] = GetTickCount();
		}
	}
	return FALSE;
}

BOOL CTempDisplay::CheckHeaterCurTemp(INT nCurindex)
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	CString szMsg = "";
	BOOL bCheckCurTemp = TRUE;

	switch (nCurindex)
	{
	case ACF1_CH1:
		if (ACF_Status[ACF_1] == ST_MOD_NOT_SELECTED)
		{
			bCheckCurTemp = FALSE;
		}
		break;

	//case ACF2_CH2:
	//	if (ACF_Status[ACF_2] == ST_MOD_NOT_SELECTED)
	//	{
	//		bCheckCurTemp = FALSE;
	//	}
	//	break;

	//case PB1_CH3:
	//	if (PREBOND_HEAD_Status[PREBOND_1] == ST_MOD_NOT_SELECTED)
	//	{
	//		bCheckCurTemp = FALSE;
	//	}
	//	break;

	//case PB2_CH4:
	//	if (PREBOND_HEAD_Status[PREBOND_2] == ST_MOD_NOT_SELECTED)
	//	{
	//		bCheckCurTemp = FALSE;
	//	}
	//	break;

	//case MB1_CTRL_CH1:
	//case MB2_CTRL_CH2:
	//case MB1_WH_CH1:
	//case MB2_WH_CH2:		
	//	if (MAINBOND_HEAD_Status[MAINBOND_1] == ST_MOD_NOT_SELECTED)
	//	{
	//		bCheckCurTemp = FALSE;
	//	}
	//	break;

	//case MB3_CTRL_CH1:
	//case MB4_CTRL_CH2:
	//case MB3_WH_CH3:
	//case MB4_WH_CH4:
	//	if (MAINBOND_HEAD_Status[MAINBOND_2] == ST_MOD_NOT_SELECTED)
	//	{
	//		bCheckCurTemp = FALSE;
	//	}
	//	break;

	default:
		break;
	}

	if (
		bCheckCurTemp && !m_bHeaterFailed[nCurindex] &&
		!m_bMonFailed[nCurindex] && m_dHeaterTempTol[nCurindex] != 0.0
	   )
	{
		if (fabs(m_dHeaterSetTemp[nCurindex] - m_dHeaterTemp[nCurindex]) > m_dHeaterTempTol[nCurindex])
		{			
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			m_bHeaterFailed[nCurindex] = TRUE;
			//SetError(IDS_TEMP_CTRL_CTRL_TEMP_OUT_OF_RANGE_ERR);
			//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + pCLitecCtrl->GetLitecErrNum(m_nCurIndex));
			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + pCLitecCtrl->GetLitecUnitNum(m_nCurIndex)); //20140401
			SetError(IDS_TEMP_CTRL_HEATER1_CTRL_TEMP_OUT_OF_RANGE_ERR + pCLitecCtrl->GetLitecErrNum(m_nCurIndex)); //20130222
			if (m_dHeaterSetTemp[nCurindex] > m_dHeaterTemp[nCurindex]) //20130314
			{
				DisplayMessage("Set temperature is higher than heater temperature");
			}
			else
			{
				DisplayMessage("Set temperature is lower than heater temperature");
			}
			szMsg.Format("Heater Ch%d Temp Out of Ramge Set Temp = %.2f, | Curr Temp = %.2f", nCurindex + 1, m_dHeaterSetTemp[nCurindex], m_dHeaterTemp[nCurindex]);
			DisplayMessage(szMsg);
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CTempDisplay::CheckHeaterFail(INT nCurindex)
{
	CString szMsg = "";
	BOOL bCheckHeater = TRUE;

	switch (nCurindex)
	{
	case ACF1_CH1:
		if (ACF_Status[ACF_1] == ST_MOD_NOT_SELECTED)
		{
			bCheckHeater = FALSE;
		}
		break;

	case ACF1WH_CH3:
		if (ACF_WH_Status[ACFWH_1] == ST_MOD_NOT_SELECTED)
		{
			bCheckHeater = FALSE;
		}
		break;

	case ACF2WH_CH4:
		if (ACF_WH_Status[ACFWH_2] == ST_MOD_NOT_SELECTED)
		{
			bCheckHeater = FALSE;
		}
		break;

	//case PB2_CH4:
	//	if (PREBOND_HEAD_Status[PREBOND_2] == ST_MOD_NOT_SELECTED)
	//	{
	//		bCheckHeater = FALSE;
	//	}
	//	break;

	//case MB1_CTRL_CH1:
	//case MB2_CTRL_CH2:
	//case MB1_WH_CH1:
	//case MB2_WH_CH2:		
	//	if (MAINBOND_HEAD_Status[MAINBOND_1] == ST_MOD_NOT_SELECTED)
	//	{
	//		bCheckHeater = FALSE;
	//	}
	//	break;

	//case MB3_CTRL_CH1:
	//case MB4_CTRL_CH2:
	//case MB3_WH_CH3:
	//case MB4_WH_CH4:
	//	if (MAINBOND_HEAD_Status[MAINBOND_2] == ST_MOD_NOT_SELECTED)
	//	{
	//		bCheckHeater = FALSE;
	//	}
	//	break;

	default:
		break;
	}

	//if (bCheckHeater && (m_bHeaterFailed[nCurindex] || m_bMonFailed[nCurindex]))
	//{
	//	if (m_bHeaterFailed[nCurindex])
	//	{
	//		DisplayMessage("TempDisplay: m_bHeaterFailed");
	//	}
	//	if (m_bMonFailed[nCurindex])
	//	{
	//		DisplayMessage("TempDisplay: m_bMonFailed");
	//	}
	//	
	//	return TRUE;
	//}
	return FALSE;
}

//VOID CTempDisplay::CheckHeaterCooling(INT nCurindex)
//{
//	switch (nCurindex)
//	{
//	case MB1_CTRL_CH1:
//	case MB2_CTRL_CH2:
//		if (
//			(!m_bHeaterOn[MB1_CTRL_CH1] && m_dHeaterTemp[MB1_CTRL_CH1] < 50.0) && 
//			(!m_bHeaterOn[MB2_CTRL_CH2] && m_dHeaterTemp[MB2_CTRL_CH2] < 50.0) && 
//			!m_stMB1HeaterCoolingSol.bHMI_Status &&
//			!m_stMB2HeaterCoolingSol.bHMI_Status
//		   )
//		{
//			SetCoolingSol(&m_stMB1HeaterCoolingSol, OFF);
//			SetCoolingSol(&m_stMB2HeaterCoolingSol, OFF);
//		}
//		else if (
//				 ((m_bHeaterOn[MB1_CTRL_CH1] && m_dHeaterTemp[MB1_CTRL_CH1] > 50.0)) || 
//				 ((m_bHeaterOn[MB2_CTRL_CH2] && m_dHeaterTemp[MB2_CTRL_CH2] > 50.0))
//				)
//		{
//			SetCoolingSol(&m_stMB1HeaterCoolingSol, ON);
//			SetCoolingSol(&m_stMB2HeaterCoolingSol, ON);
//
//		}
//
//		break;
//
//	case MB3_CTRL_CH1:
//	case MB4_CTRL_CH2:
//		if (
//			(!m_bHeaterOn[MB3_CTRL_CH1] && m_dHeaterTemp[MB3_CTRL_CH1] < 50.0) && 
//			(!m_bHeaterOn[MB4_CTRL_CH2] && m_dHeaterTemp[MB4_CTRL_CH2] < 50.0) &&
//			!m_stMB3HeaterCoolingSol.bHMI_Status &&
//			!m_stMB4HeaterCoolingSol.bHMI_Status
//		   )
//		{
//			SetCoolingSol(&m_stMB3HeaterCoolingSol, OFF);
//			SetCoolingSol(&m_stMB4HeaterCoolingSol, OFF);
//
//		}	
//		else if (
//				 ((m_bHeaterOn[MB3_CTRL_CH1] && m_dHeaterTemp[MB3_CTRL_CH1] > 50.0)) || 
//				 ((m_bHeaterOn[MB4_CTRL_CH2] && m_dHeaterTemp[MB4_CTRL_CH2] > 50.0))
//				)
//		{
//			SetCoolingSol(&m_stMB3HeaterCoolingSol, ON);
//			SetCoolingSol(&m_stMB4HeaterCoolingSol, ON);
//
//		}
//		break;
//
//	case PB1_CH3:
//		if (
//			(!m_bHeaterOn[PB1_CH3] && m_dHeaterTemp[PB1_CH3] < 50.0) && 
//			!m_stPB1HeaterCoolingSol.bHMI_Status
//		   )
//		{
//			SetCoolingSol(&m_stPB1HeaterCoolingSol, OFF);
//		}
//		else if (
//				 ((m_bHeaterOn[PB1_CH3] && m_dHeaterTemp[PB1_CH3] > 50.0))
//				)
//		{
//			SetCoolingSol(&m_stPB1HeaterCoolingSol, ON);
//		}
//		break;
//
//	case PB2_CH4:
//		if (
//			(!m_bHeaterOn[PB2_CH4] && m_dHeaterTemp[PB2_CH4] < 50.0) && 
//			!m_stPB2HeaterCoolingSol.bHMI_Status
//		   )
//		{
//			SetCoolingSol(&m_stPB2HeaterCoolingSol, OFF);
//		}
//		else if (
//				 ((m_bHeaterOn[PB2_CH4] && m_dHeaterTemp[PB2_CH4] > 50.0))
//				)
//		{
//			SetCoolingSol(&m_stPB2HeaterCoolingSol, ON);
//		}
//		break;
//
//	default:
//		return;
//	}
//
//	return;
//}

VOID CTempDisplay::ResetAllData(LONG lChannel)
{
	INT i;

	for (i = 0; i < MAX_TEMP_DATA; i++)
	{
		m_stTempCalib[lChannel][i].SetTemp = 0.0;
		m_stTempCalib[lChannel][i].BondTemp = 0.0;
	}

	// Set data to file
	CString szName = "";
	CString	szSetTemp = "";
	CString	szBondTemp = "";

	ConvertToFileName(lChannel, &szName, &szSetTemp, &szBondTemp);

	FILE *fp = fopen(szName, "wt");
	if (fp != NULL)
	{
		for (INT i = 0; i < MAX_TEMP_DATA; i++)
		{
			fprintf(fp, "%.5f,%.5f\n", m_stTempCalib[lChannel][i].SetTemp, m_stTempCalib[lChannel][i].BondTemp);
		}
		fclose(fp);
	}
	
	fp = fopen(szSetTemp, "wt");
	if (fp != NULL)
	{
		fprintf(fp, "SetTemp (DegC),");

		for (INT i = 0; i < MAX_TEMP_DATA; i++)
		{
			if (i < MAX_TEMP_DATA - 1)
			{
				fprintf(fp, "%.5f,", m_stTempCalib[lChannel][i].SetTemp);
			}
			else
			{
				fprintf(fp, "%.5f", m_stTempCalib[lChannel][i].SetTemp);
			}
		}
		fclose(fp);
	}

	fp = fopen(szBondTemp, "wt");
	if (fp != NULL)
	{
		fprintf(fp, "BondTemp (DegC),");

		for (INT i = 0; i < MAX_TEMP_DATA; i++)
		{
			if (i < MAX_TEMP_DATA - 1)
			{
				fprintf(fp, "%.5f,", m_stTempCalib[lChannel][i].BondTemp);
			}
			else
			{
				fprintf(fp, "%.5f", m_stTempCalib[lChannel][i].BondTemp);
			}
		}
		fclose(fp);
	}

	m_bValidTemp[lChannel] = SetTempToBondTemp(m_dHeaterSetTemp[lChannel], m_stTempCalib[lChannel], &m_dBondPtTemp[lChannel]);

	return;
}

VOID CTempDisplay::PrintMachineSetup(FILE *fp)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString szName = "";
	CString szModuleName = "";

	if (fp != NULL)
	{
		fprintf(fp, "\nLitec Monitor\n");
		fprintf(fp, "======================================================\n");
		fprintf(fp, "<Monitor Offsets>\n");
		
		for (INT i = 0; i < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; i++)
		{
			ConvertToModuleName(i, &szModuleName);
			szName.Format("m_dMonOffset%ld", i + 1);
			fprintf(fp, "%30s	: %ld\n", (const char*)szModuleName, (LONG)pAppMod->m_smfMachine[GetStnName()]["TempMon"][szName]);
		}

		fprintf(fp, "\n<Monitor Tolerence>\n");

		for (INT i = 0; i < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; i++)
		{
			ConvertToModuleName(i, &szModuleName);
			szName.Format("m_dMonTol%ld", i + 1);
			fprintf(fp, "%30s	: %ld\n", (const char*)szModuleName, (LONG)pAppMod->m_smfMachine[GetStnName()]["TempMon"][szName]);
		}
	}
}

VOID CTempDisplay::PrintDeviceSetup(FILE *fp)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString szModuleName = "";
	CString szName = "";
	
	if (fp != NULL)
	{
		fprintf(fp, "\nLitecCtrl\n");
		fprintf(fp, "======================================================\n");
		fprintf(fp, "<Set Temp>\n");

		for (INT k = 0; k < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; k++)
		{
			ConvertToModuleName(k, &szModuleName);
			szName.Format("m_dHeater%ldSetTemp", k);
			fprintf(fp, "%30s	: %ld\n", (const char*)szModuleName, (LONG)pAppMod->m_smfDevice[GetStnName()]["Heater"][szName]);
		}

		fprintf(fp, "\n<Temp Calibration>");
		for (INT k = 0; k < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; k++)
		{
			ConvertToModuleName(k, &szModuleName);
			fprintf(fp, "\n<%s>\n", (const char*)szModuleName);
			fprintf(fp, "Set Temp (degC)			: Bond Temp (degC)\n");
			for (INT i = 0; i < MAX_TEMP_DATA; i++)
			{
				szName.Format("m_stTempCalib[%ld][%ld]", k, i);
				fprintf(fp, "%.4f				: %.4f\n", (DOUBLE)pAppMod->m_smfDevice[GetStnName()]["Calib"][szName]["SetTemp"],  
						(DOUBLE)pAppMod->m_smfDevice[GetStnName()]["Calib"][szName]["BondTemp"]);										
			}
		}
	}
}

#ifdef EXCEL_MACHINE_DEVICE_INFO
BOOL CTempDisplay::printMachineDeviceInfo()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bResult = TRUE;
	CString szBlank("--");
	CString szModule = GetStnName();
	LONG lValue = 0;
	DOUBLE dValue = 0.0;
	CString szName = "";

	for (INT k = 0; k < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; k++)
	{
		szName.Format("m_dMonOffset%ld", k + 1);

		dValue = (DOUBLE) pAppMod->m_smfMachine[GetStnName()]["TempMon"][szName];
		bResult = printMachineInfoRow(szModule, CString("TempMon"), szName, szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, dValue);

		szName.Format("m_dMonTol%ld", k + 1);
		dValue = (DOUBLE) pAppMod->m_smfMachine[GetStnName()]["TempMon"][szName];
		bResult = printMachineInfoRow(szModule, CString("TempMon"), szName, szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, dValue);

		szName.Format("m_dHeater%ldTempTol", k);
		dValue = (DOUBLE) pAppMod->m_smfMachine[GetStnName()]["Heater"][szName];
		bResult = printMachineInfoRow(szModule, CString("Heater"), szName, szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, dValue);

		// loading temp calibration
		for (INT i = 0; i < MAX_TEMP_DATA; i++)
		{
			szName.Format("m_stTempCalib[%ld][%ld]", k, i);

			dValue = (DOUBLE) pAppMod->m_smfMachine[GetStnName()]["Calib"][szName]["SetTemp"];
			bResult = printMachineInfoRow(szModule, CString("Calib"), szName, CString("SetTemp"), szBlank,
										  szBlank, szBlank, szBlank, szBlank, szBlank, dValue);
			if (!bResult)
			{
				break;
			}

			dValue = (DOUBLE) pAppMod->m_smfMachine[GetStnName()]["Calib"][szName]["BondTemp"];
			bResult = printMachineInfoRow(szModule, CString("Calib"), szName, CString("BondTemp"), szBlank,
										  szBlank, szBlank, szBlank, szBlank, szBlank, dValue);
			if (!bResult)
			{
				break;
			}
		}
	}

	if (bResult)
	{
		bResult = CAC9000Stn::printMachineDeviceInfo();
	}
	return bResult;
}
#endif

VOID CTempDisplay::ConvertToFileName(INT nChn, CString *pszName, CString *pszSetTemp, CString *pszBondTemp)
{
	switch (nChn)
	{
	//case MB1_CTRL_CH1:
	//	pszName->Format("D:\\sw\\AC9000\\Device\\MB1L_TempData.csv");
	//	pszSetTemp->Format("D:\\sw\\AC9000\\Device\\MB1L_TempData(SetTemp).csv");
	//	pszBondTemp->Format("D:\\sw\\AC9000\\Device\\MB1L_TempData(BondTemp).csv");
	//	break;

	//case MB2_CTRL_CH2:
	//	pszName->Format("D:\\sw\\AC9000\\Device\\MB1R_TempData.csv");
	//	pszSetTemp->Format("D:\\sw\\AC9000\\Device\\MB1R_TempData(SetTemp).csv");
	//	pszBondTemp->Format("D:\\sw\\AC9000\\Device\\MB1R_TempData(BondTemp).csv");
	//	break;

	//case MB1_WH_CH1:
	//	pszName->Format("D:\\sw\\AC9000\\Device\\MBWH1L_TempData.csv");
	//	pszSetTemp->Format("D:\\sw\\AC9000\\Device\\MBWH1L_TempData(SetTemp).csv");
	//	pszBondTemp->Format("D:\\sw\\AC9000\\Device\\MBWH1L_TempData(BondTemp).csv");
	//	break;

	//case MB2_WH_CH2:
	//	pszName->Format("D:\\sw\\AC9000\\Device\\MBWH1R_TempData.csv");
	//	pszSetTemp->Format("D:\\sw\\AC9000\\Device\\MBWH1R_TempData(SetTemp).csv");
	//	pszBondTemp->Format("D:\\sw\\AC9000\\Device\\MBWH1R_TempData(BondTemp).csv");
	//	break;

	case ACF1_CH1:
		pszName->Format("D:\\sw\\AC9000\\Device\\ACF1_TempData.csv");
		pszSetTemp->Format("D:\\sw\\AC9000\\Device\\ACF1_TempData(SetTemp).csv");
		pszBondTemp->Format("D:\\sw\\AC9000\\Device\\ACF1_TempData(BondTemp).csv");
		break;

	//case PB1_CH3:
	//	pszName->Format("D:\\sw\\AC9000\\Device\\PB1_TempData.csv");
	//	pszSetTemp->Format("D:\\sw\\AC9000\\Device\\PB1_TempData(SetTemp).csv");
	//	pszBondTemp->Format("D:\\sw\\AC9000\\Device\\PB1_TempData(BondTemp).csv");
	//	break;

	//case MB3_CTRL_CH1:
	//	pszName->Format("D:\\sw\\AC9000\\Device\\MB2L_TempData.csv");
	//	pszSetTemp->Format("D:\\sw\\AC9000\\Device\\MB2L_TempData(SetTemp).csv");
	//	pszBondTemp->Format("D:\\sw\\AC9000\\Device\\MB2L_TempData(BondTemp).csv");
	//	break;

	//case MB4_CTRL_CH2:
	//	pszName->Format("D:\\sw\\AC9000\\Device\\MB2R_TempData.csv");
	//	pszSetTemp->Format("D:\\sw\\AC9000\\Device\\MB2R_TempData(SetTemp).csv");
	//	pszBondTemp->Format("D:\\sw\\AC9000\\Device\\MB2R_TempData(BondTemp).csv");
	//	break;

	//case MB3_WH_CH3:
	//	pszName->Format("D:\\sw\\AC9000\\Device\\MBWH2L_TempData.csv");
	//	pszSetTemp->Format("D:\\sw\\AC9000\\Device\\MBWH2L_TempData(SetTemp).csv");
	//	pszBondTemp->Format("D:\\sw\\AC9000\\Device\\MBWH2L_TempData(BondTemp).csv");
	//	break;

	//case MB4_WH_CH4:
	//	pszName->Format("D:\\sw\\AC9000\\Device\\MBWH2R_TempData.csv");
	//	pszSetTemp->Format("D:\\sw\\AC9000\\Device\\MBWH2R_TempData(SetTemp).csv");
	//	pszBondTemp->Format("D:\\sw\\AC9000\\Device\\MBWH2R_TempData(BondTemp).csv");
	//	break;

	//case ACF2_CH2:
	//	pszName->Format("D:\\sw\\AC9000\\Device\\ACF2_TempData.csv");
	//	pszSetTemp->Format("D:\\sw\\AC9000\\Device\\ACF2_TempData(SetTemp).csv");
	//	pszBondTemp->Format("D:\\sw\\AC9000\\Device\\ACF2_TempData(BondTemp).csv");
	//	break;

	//case PB2_CH4:
	//	pszName->Format("D:\\sw\\AC9000\\Device\\PB2_TempData.csv");
	//	pszSetTemp->Format("D:\\sw\\AC9000\\Device\\PB2_TempData(SetTemp).csv");
	//	pszBondTemp->Format("D:\\sw\\AC9000\\Device\\PB2_TempData(BondTemp).csv");
	//	break;
	}

	return;
}

VOID CTempDisplay::ConvertToModuleName(INT nChn, CString *pszModuleName)
{
	switch (nChn)
	{
	//case MB1_CTRL_CH1:
	//	pszModuleName->Format("MainBond Head 1 Left");
	//	break;

	//case MB2_CTRL_CH2:
	//	pszModuleName->Format("MainBond Head 1 Right");
	//	break;

	//case MB1_WH_CH1:
	//	pszModuleName->Format("MainBond WH 1 Left");
	//	break;

	//case MB2_WH_CH2:
	//	pszModuleName->Format("MainBond WH 1 Right");
	//	break;

	case ACF1_CH1:
		pszModuleName->Format("ACF Head 1");
		break;

	//case PB1_CH3:
	//	pszModuleName->Format("PreBond Head 1");
	//	break;

	//case MB3_CTRL_CH1:
	//	pszModuleName->Format("MainBond Head 2 Left");
	//	break;

	//case MB4_CTRL_CH2:
	//	pszModuleName->Format("MainBond Head 2 Right");
	//	break;

	//case MB3_WH_CH3:
	//	pszModuleName->Format("MainBond WH 2 Left");
	//	break;

	//case MB4_WH_CH4:
	//	pszModuleName->Format("MainBond WH 2 Right");
	//	break;

	//case ACF2_CH2:
	//	pszModuleName->Format("ACF Head 2");
	//	break;

	//case PB2_CH4:
	//	pszModuleName->Format("PreBond Head 2");
	//	break;
	}

	return;
}

VOID CTempDisplay::ConvertToStationName(INT nChn, CString *pszStationName) //20130424
{

	switch (nChn)
	{
	//case MB1_CTRL_CH1: //"MainBond Head 1 Left"
	//case MB2_CTRL_CH2: //"MainBond Head 1 Right"
	//case MB1_WH_CH1: //"MainBond Base 1 Left"
	//case MB2_WH_CH2: //"MainBond Base 1 Right"
	//	pszStationName->Format("MB1");
	//	break;

	case ACF1_CH1: //"ACF Head 1"
		pszStationName->Format("ACF1");
		break;

	//case PB1_CH3: //"PreBond Head 1"
	//	pszStationName->Format("PB1");
	//	break;

	//case MB3_CTRL_CH1: //"MainBond Head 2 Left"
	//case MB4_CTRL_CH2: //"MainBond Head 2 Right"
	//case MB3_WH_CH3: //"MainBond Base 2 Left"
	//case MB4_WH_CH4: //"MainBond Base 2 Right"
	//	pszStationName->Format("MB2");
	//	break;

	//case ACF2_CH2: //"ACF Head 2"
	//	pszStationName->Format("ACF2");
	//	break;

	//case PB2_CH4: //"PreBond Head 2"
	//	pszStationName->Format("PB2");
	//	break;
	}

	return;
}
//-----------------------------------------------------------------------------
// IPC command implementation
//-----------------------------------------------------------------------------
//
//LONG CTempDisplay::HMI_TogglePB1HeaterCoolingSol(IPC_CServiceMessage &svMsg)
//{
//	if (m_stPB1HeaterCoolingSol.bHMI_Status)
//	{
//		SetCoolingSol(&m_stPB1HeaterCoolingSol, OFF);
//	}
//	else
//	{
//		SetCoolingSol(&m_stPB1HeaterCoolingSol, ON);
//	}
//
//	return 0;
//}
//
//LONG CTempDisplay::HMI_TogglePB2HeaterCoolingSol(IPC_CServiceMessage &svMsg)
//{
//	if (m_stPB2HeaterCoolingSol.bHMI_Status)
//	{
//		SetCoolingSol(&m_stPB2HeaterCoolingSol, OFF);
//	}
//	else
//	{
//		SetCoolingSol(&m_stPB2HeaterCoolingSol, ON);
//	}
//
//	return 0;
//}
//
//LONG CTempDisplay::HMI_ToggleMB1HeaterCoolingSol(IPC_CServiceMessage &svMsg)
//{
//	if (m_stMB1HeaterCoolingSol.bHMI_Status)
//	{
//		SetCoolingSol(&m_stMB1HeaterCoolingSol, OFF);
//	}
//	else
//	{
//		SetCoolingSol(&m_stMB1HeaterCoolingSol, ON);
//	}
//
//	return 0;
//}
//
//LONG CTempDisplay::HMI_ToggleMB2HeaterCoolingSol(IPC_CServiceMessage &svMsg)
//{
//	if (m_stMB2HeaterCoolingSol.bHMI_Status)
//	{
//		SetCoolingSol(&m_stMB2HeaterCoolingSol, OFF);
//	}
//	else
//	{
//		SetCoolingSol(&m_stMB2HeaterCoolingSol, ON);
//	}
//
//	return 0;
//}
//
//LONG CTempDisplay::HMI_ToggleMB3HeaterCoolingSol(IPC_CServiceMessage &svMsg)
//{
//	if (m_stMB3HeaterCoolingSol.bHMI_Status)
//	{
//		SetCoolingSol(&m_stMB3HeaterCoolingSol, OFF);
//	}
//	else
//	{
//		SetCoolingSol(&m_stMB3HeaterCoolingSol, ON);
//	}
//
//	return 0;
//}
//
//LONG CTempDisplay::HMI_ToggleMB4HeaterCoolingSol(IPC_CServiceMessage &svMsg)
//{
//	if (m_stMB4HeaterCoolingSol.bHMI_Status)
//	{
//		SetCoolingSol(&m_stMB4HeaterCoolingSol, OFF);
//	}
//	else
//	{
//		SetCoolingSol(&m_stMB4HeaterCoolingSol, ON);
//	}
//
//	return 0;
//}

LONG CTempDisplay::HMI_PowerOnAllHeater(IPC_CServiceMessage &svMsg)
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	INT i;	
	BOOL bOpDone = TRUE;

	m_bHeatersOn = bMode;	// 20140918 Yip: Add Flag To Toggle All Heaters Power On/Off

	// Avoid Display Error Message when Press EMO
	if (!pCLitecCtrl->m_fHardware)
	{
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);

		return TRUE;
	}

	// Set Heater SetTemp
	// Power On Heater
	for (i = 0; i < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; i++)
	{
#if 1 //20140520
		if (m_bHeaterExist[i])
		{
			if (bMode)
			{
				SetHeaterTemp(i, m_dHeaterSetTemp[i]);
			}
			PowerOnHeater(i, bMode);		
		}
#else
		if (bMode && m_bHeaterExist[i])
		{
			SetHeaterTemp(i, m_dHeaterSetTemp[i]);
			PowerOnHeater(i, bMode);		
		}
#endif
		//PowerOnHeater(i, bMode);		
	}


	//turn on cooling if turning heater on
	//if (bMode)
	//{
	//	SetCoolingSol(&m_stPB1HeaterCoolingSol, ON);
	//	SetCoolingSol(&m_stPB2HeaterCoolingSol, ON);
	//	SetCoolingSol(&m_stMB1HeaterCoolingSol, ON);
	//	SetCoolingSol(&m_stMB2HeaterCoolingSol, ON);
	//	SetCoolingSol(&m_stMB3HeaterCoolingSol, ON);
	//	SetCoolingSol(&m_stMB4HeaterCoolingSol, ON);
	//}

	
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);

	return TRUE;
}

LONG CTempDisplay::HMI_PowerOnHeater(IPC_CServiceMessage &svMsg)
{
	typedef struct
	{
		BOOL bMode;
		LONG lChannel;
	} PARA;

	PARA stPara;
	svMsg.GetMsg(sizeof(PARA), &stPara);

	PowerOnHeater(stPara.lChannel, stPara.bMode);

	if (stPara.bMode)
	{
		switch (stPara.lChannel)
		{
		//case	MB1_CTRL_CH1:	
		//case	MB2_CTRL_CH2:
		//	SetCoolingSol(&m_stMB1HeaterCoolingSol, ON);
		//	SetCoolingSol(&m_stMB2HeaterCoolingSol, ON);

		//	break;

		//case	MB3_CTRL_CH1:	
		//case	MB4_CTRL_CH2:
		//	SetCoolingSol(&m_stMB3HeaterCoolingSol, ON);
		//	SetCoolingSol(&m_stMB4HeaterCoolingSol, ON);
		//	break;

		//case	PB1_CH3:	
		//	SetCoolingSol(&m_stPB1HeaterCoolingSol, ON);
		//	break;

		//case	PB2_CH4:
		//	SetCoolingSol(&m_stPB2HeaterCoolingSol, ON);
		//	break;	

		//case	MB1_WH_CH1:	
		//case	MB2_WH_CH2:
		//case	MB3_WH_CH3:
		//case	MB4_WH_CH4:	
		case	ACF1_CH1:	
		//case	ACF2_CH2:
			break;
		}
	}

	return 0;
}

// set temp
LONG CTempDisplay::HMI_SetHeaterTemp(IPC_CServiceMessage &svMsg)
{
	BOOL bIsBaseHeater = FALSE;
	//LONG lMBheadHeaterChannel;

	typedef struct
	{				
		DOUBLE dTemp;
		LONG lChannel;
	} PARA;

	PARA stPara;

	stPara.dTemp = 0.0;
	stPara.lChannel = 0;
	svMsg.GetMsg(sizeof(DOUBLE) + sizeof(LONG), &stPara);

	////Debug
	//CString szMsg = "";
	//szMsg.Format("dTemp: %.2f, Channel num: %ld", stPara.dTemp, stPara.lChannel);
	//DisplayMessage(szMsg);

	//if (
	//	stPara.lChannel == MB1_WH_CH1 || stPara.lChannel == MB2_WH_CH2 ||
	//	stPara.lChannel == MB3_WH_CH3 || stPara.lChannel == MB4_WH_CH4
	//   )
	//{
	//	bIsBaseHeater = TRUE;
	//}

	//if (bIsBaseHeater)
	//{
	//	switch (stPara.lChannel)
	//	{
	//	case MB1_WH_CH1:
	//		lMBheadHeaterChannel = MB1_CTRL_CH1;
	//		break;

	//	case MB2_WH_CH2:
	//		lMBheadHeaterChannel = MB2_CTRL_CH2;
	//		break;

	//	case MB3_WH_CH3:
	//		lMBheadHeaterChannel = MB3_CTRL_CH1;
	//		break;

	//	case MB4_WH_CH4:
	//		lMBheadHeaterChannel = MB4_CTRL_CH2;
	//		break;
	//	}

	//	if (rMSG_YES == HMIMessageBox(MSG_YES_NO, "WARNING", "Changing Base Heater Temperature Would Affect Temperature Calibration. Clear calibration data?"))
	//	{
	//		ResetAllData(lMBheadHeaterChannel); //The Corresponding Head Heater of the Selected Base Heater //alfred AC9000
	//	}
	//}
	//else
	{
		BOOL bValid = SetTempToBondTemp(stPara.dTemp, m_stTempCalib[stPara.lChannel], &m_dBondPtTemp[stPara.lChannel]);

		if (!bValid)
		{
			if (rMSG_NO == HMIMessageBox(MSG_YES_NO, "WARNING", "Desire Set Temp Is Out Of Calibration Range. Ignore warning?"))
			{
				return 0;
			}

			m_bValidTemp[stPara.lChannel] = FALSE;
		}
		else
		{
			m_bValidTemp[stPara.lChannel] = TRUE;
		}
	}

	if ((INT)HEATER_OP_OK == SetHeaterTemp(stPara.lChannel, stPara.dTemp))
	{
		CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");
		SetAndLogParameter(__FUNCTION__, pCLitecCtrl->GetLitecChName(stPara.lChannel) + " Heater Set Temperature", m_dHeaterSetTemp[stPara.lChannel], stPara.dTemp);
	}

	return 0;
}

LONG CTempDisplay::HMI_SetTempTol(IPC_CServiceMessage &svMsg)
{
	typedef struct
	{
		DOUBLE dTol;
		LONG lChannel;
	} PARA;

	PARA stPara;
	stPara.dTol = 0.0;
	stPara.lChannel = 0;

	svMsg.GetMsg(sizeof(DOUBLE) + sizeof(LONG), &stPara);

	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");
	SetAndLogParameter(__FUNCTION__, pCLitecCtrl->GetLitecChName(stPara.lChannel) + " Heater Temperature Tolerance", m_dHeaterTempTol[stPara.lChannel], stPara.dTol);

	return 0;
}

LONG CTempDisplay::HMI_SetBondTemp(IPC_CServiceMessage &svMsg)
{
	typedef struct
	{
		DOUBLE dTemp;
		LONG lChannel;
	} PARA;

	PARA stPara;

	stPara.dTemp = 0.0;
	stPara.lChannel = 0;
	svMsg.GetMsg(sizeof(DOUBLE) + sizeof(LONG), &stPara);

	DOUBLE dSetTemp;

	BOOL bValid = BondTempToSetTemp(stPara.dTemp, m_stTempCalib[stPara.lChannel], &dSetTemp);

	if (!bValid)
	{
		if (rMSG_NO == HMIMessageBox(MSG_YES_NO, "WARNING", "Desire BondPt Temp Is Out Of Calibration Range. Ignore warning?"))
		{
			return 0;
		}

		m_bValidTemp[stPara.lChannel] = FALSE;
	}
	else
	{
		m_bValidTemp[stPara.lChannel] = TRUE;
	}

	if (HEATER_OP_OK == SetHeaterTemp(stPara.lChannel, dSetTemp))
	{
		CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");
		SetAndLogParameter(__FUNCTION__, pCLitecCtrl->GetLitecChName(stPara.lChannel) + " Heater Bond Temperature", m_dBondPtTemp[stPara.lChannel], stPara.dTemp);
		SetAndLogParameter(__FUNCTION__, pCLitecCtrl->GetLitecChName(stPara.lChannel) + " Heater Set Temperature", m_dHeaterSetTemp[stPara.lChannel], dSetTemp);
	}

	return 0;
}

// auto tuning
LONG CTempDisplay::HMI_SetHeaterATPara(IPC_CServiceMessage &svMsg)
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	LONG lChannel = 0;
	svMsg.GetMsg(sizeof(LONG), &lChannel);

	if (pCLitecCtrl->SetTunePara(lChannel) != HEATER_OP_OK)
	{
		HMIMessageBox(MSG_OK, "Set Tune Para", "Operation Fail");
	}

	return 0;
}

LONG CTempDisplay::HMI_TurnOnHeaterAT(IPC_CServiceMessage &svMsg)
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	typedef struct
	{
		BOOL bMode;
		LONG lChannel;
	} PARA;

	PARA stPara;

	stPara.bMode = FALSE;
	stPara.lChannel = 0;
	svMsg.GetMsg(sizeof(PARA), &stPara);

	if (ON == stPara.bMode)
	{
		if (pCLitecCtrl->SetTunePara(stPara.lChannel) != HEATER_OP_OK)
		{
			HMIMessageBox(MSG_OK, "Set Tune Para", "Operation Fail");
			return 0;
		}

		switch (stPara.lChannel)
		{
		//case	MB1_CTRL_CH1:	
		//case	MB2_CTRL_CH2:
		//	SetCoolingSol(&m_stMB1HeaterCoolingSol, ON);
		//	break;

		//case	MB3_CTRL_CH1:	
		//case	MB4_CTRL_CH2:
		//	SetCoolingSol(&m_stMB2HeaterCoolingSol, ON);
		//	break;

		//case	PB1_CH3:	
		//	SetCoolingSol(&m_stPB1HeaterCoolingSol, ON);
		//	break;

		//case	PB2_CH4:
		//	SetCoolingSol(&m_stPB2HeaterCoolingSol, ON);
		//	break;	

		//case	MB1_WH_CH1:	
		//case	MB2_WH_CH2:
		//case	MB3_WH_CH3:
		//case	MB4_WH_CH4:	
		case	ACF1_CH1:	
		//case	ACF2_CH2:
			break;
		}
	}


	if (HEATER_OP_OK != TurnOnHeaterAT(stPara.lChannel, stPara.bMode))
	{
		HMIMessageBox(MSG_OK, "Turn On Auto-Tune", "Operation Fail");
	}

	CString szMsg = "";
	szMsg.Format("Ch#%d start Auto-Tune", stPara.lChannel);
	DisplayMessage(szMsg);
	
	return 0;
}

//Set Alarm Band
LONG CTempDisplay::HMI_SetAlarmBand(IPC_CServiceMessage &svMsg)
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	typedef struct
	{
		DOUBLE dAlarmBand;
		LONG lChannel;
	} PARA;

	PARA stPara;

	stPara.dAlarmBand = 0.0;
	stPara.lChannel = 0;
	svMsg.GetMsg(sizeof(DOUBLE) + sizeof(LONG), &stPara);

	if (pCLitecCtrl->SetAlarmBand(stPara.lChannel, stPara.dAlarmBand) != HEATER_OP_OK)
	{
		HMIMessageBox(MSG_OK, "Set AlarmBand", "Operation Fail");
	}

	m_dMB1WHLeftAlarmTemp = stPara.dAlarmBand;

	return 0;
}

LONG CTempDisplay::HMI_LearnHODMask(IPC_CServiceMessage &svMsg)
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	LONG lChannel;
	svMsg.GetMsg(sizeof(LONG), &lChannel);

	if (m_dHeaterTemp[lChannel] > 50.0)
	{
		HMIMessageBox(MSG_OK, "LEARN HOD MASK", "Learning HOD is required to start below 50 degree Celsius. Operation Abort!");
	}
	else
	{
		if (
			(HEATER_OP_OK != SetHeaterTemp(lChannel, m_dHeaterSetTemp[lChannel])) || //100.0
			(HEATER_OP_OK != PowerOnHeater(lChannel, ON)) ||
			(HEATER_OP_OK != LearnHODMask(lChannel))
		   )
		{
			HMIMessageBox(MSG_OK, "LEARN HOD MASK", "Learn HOD Mask Failed!");
		}
		else
		{			
			Sleep(1000);	//wait Litec reaches full PWM
			pCLitecCtrl->UpdateHODMask(lChannel);
			HMIMessageBox(MSG_OK, "LEARN HOD MASK", "Learn HOD Mask Finished!");
		}
	}

	BOOL bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}

LONG CTempDisplay::HMI_SetHODThreshold(IPC_CServiceMessage &svMsg)
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	LONG lChannel;
	svMsg.GetMsg(sizeof(LONG), &lChannel);

	LONG lThreshold = 0;
	CString szMsg = "";

	if (pCLitecCtrl->GetHODThreshold(lChannel, &lThreshold) == HEATER_OP_OK)
	{
		if (HMINumericKeys("Set HOD Threshold", 32767.0, 0.0, &lThreshold))
		{
			if (pCLitecCtrl->SetHODThreshold(lChannel, lThreshold) != HEATER_OP_OK)
			{
				HMIMessageBox(MSG_OK, "SET HOD THRESHOLD", "Operation Fail");
			}
		}
		else
		{
			HMIMessageBox(MSG_OK, "SET HOD THRESHOLD", "Operation Fail");
		}
	}
	return 1;
}

LONG CTempDisplay::HMI_LearnRMSMask(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	LONG lChannel;
	svMsg.GetMsg(sizeof(LONG), &lChannel);

	if (m_dHeaterTemp[lChannel] > 50.0)
	{
		HMIMessageBox(MSG_OK, "LEARN RMS MASK", "Learning RMS is required to start below 50 degree Celsius. Operation Abort!");
	}
	else
	{
		if (
			(HEATER_OP_OK != LearnRMSMask(lChannel))
		   )
		{
			HMIMessageBox(MSG_OK, "LEARN RMS MASK", "Learn RMS Mask Failed!");
		}
		else
		{
			//HMIMessageBox(MSG_OK, "LEARN RMS MASK", "Learn RMS Mask Finished!");
			DisplayMessage("Start Learn RMS Mask");
		}
	}

	BOOL bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}

// Logging
LONG CTempDisplay::HMI_SetMonTemp(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	m_bMonTemp = bMode;

	//if (bMode) //20130627
	{
		for (INT i = 0; i < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; i++)
		{
			m_bMonFailed[i] = FALSE;
		}
	}

	return 0;
}

// Set Offset
LONG CTempDisplay::HMI_SetMonOffset(IPC_CServiceMessage &svMsg)
{
	typedef struct
	{
		DOUBLE dOffset;
		LONG lChannel;
	} PARA;

	PARA stPara;
	stPara.dOffset = 0.0;
	stPara.lChannel = 0;
	svMsg.GetMsg(sizeof(DOUBLE) + sizeof(LONG), &stPara);

	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");
	SetAndLogParameter(__FUNCTION__, pCLitecCtrl->GetLitecChName(stPara.lChannel) + " Temperature Monitor Offset", m_dMonOffset[stPara.lChannel], stPara.dOffset);

	return 0;
}

// Set Tolerance
LONG CTempDisplay::HMI_SetMonTol(IPC_CServiceMessage &svMsg)
{
	typedef struct
	{
		DOUBLE dTol;
		LONG lChannel;
	} PARA;

	PARA stPara;
	stPara.dTol = 0.0;
	stPara.lChannel = 0;
	svMsg.GetMsg(sizeof(DOUBLE) + sizeof(LONG), &stPara);

	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");
	SetAndLogParameter(__FUNCTION__, pCLitecCtrl->GetLitecChName(stPara.lChannel) + " Temperature Monitor Tolerance", m_dMonTol[stPara.lChannel], stPara.dTol);

	return 0;
}

//Temp Calib Related
LONG CTempDisplay::HMI_TempInsertData(IPC_CServiceMessage &svMsg)
{
	LONG lChannel;
	svMsg.GetMsg(sizeof(LONG), &lChannel);

	INT i;
	BOOL bFull = TRUE;
	BOOL bOpDone = TRUE;

	for (i = 0; i < MAX_TEMP_DATA; i++)
	{
		if (m_stTempCalib[lChannel][i].SetTemp == 0.0)
		{
			bFull = FALSE;
			break;
		}
	}

	if (bFull)
	{
		HMIMessageBox(MSG_OK, "TEMP CALIBRATION INSERT DATA", "Data Set is Full.");

		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
		//return 0;
		return 1; //20120314 problem 11
	}

	// get data
	DOUBLE dSetTemp = 0.0;
	DOUBLE dBondTemp = 0.0;

	if (!HMINumericKeys("SetTemp (DegC)", 500.0, 25.0, &dSetTemp) || !HMINumericKeys("BondTemp (DegC)", 500.0, 25.0, &dBondTemp))
	{
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
		//return 0;
		return 1; //20120314 problem 11
	}

	m_stTempCalib[lChannel][i].SetTemp = dSetTemp;
	m_stTempCalib[lChannel][i].BondTemp = dBondTemp;

	// sort data
	SortTempCalibData(m_stTempCalib[lChannel]);

	// Set data to file
	CString szName = "";
	CString	szSetTemp = "";
	CString	szBondTemp = "";

	ConvertToFileName(lChannel, &szName, &szSetTemp, &szBondTemp);
		
	FILE *fp = fopen(szName, "wt");
	if (fp != NULL)
	{
		for (INT i = 0; i < MAX_TEMP_DATA; i++)
		{
			fprintf(fp, "%.5f,%.5f\n", m_stTempCalib[lChannel][i].SetTemp, m_stTempCalib[lChannel][i].BondTemp);
		}
		fclose(fp);
	}
	
	fp = fopen(szSetTemp, "wt");
	if (fp != NULL)
	{
		fprintf(fp, "SetTemp (DegC),");

		for (INT i = 0; i < MAX_TEMP_DATA; i++)
		{
			if (i < MAX_TEMP_DATA - 1)
			{
				fprintf(fp, "%.5f,", m_stTempCalib[lChannel][i].SetTemp);
			}
			else
			{
				fprintf(fp, "%.5f", m_stTempCalib[lChannel][i].SetTemp);
			}
		}
		fclose(fp);
	}
	

	fp = fopen(szBondTemp, "wt");
	if (fp != NULL)
	{
		fprintf(fp, "BondTemp (DegC),");

		for (INT i = 0; i < MAX_TEMP_DATA; i++)
		{
			if (i < MAX_TEMP_DATA - 1)
			{
				fprintf(fp, "%.5f,", m_stTempCalib[lChannel][i].BondTemp);
			}
			else
			{
				fprintf(fp, "%.5f", m_stTempCalib[lChannel][i].BondTemp);
			}
		}
		fclose(fp);
	}	

	m_bValidTemp[lChannel]	= SetTempToBondTemp(m_dHeaterSetTemp[lChannel], m_stTempCalib[lChannel], &m_dBondPtTemp[lChannel]);

	//return 0;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone); //20120314 problem 11
	return 1;
}

LONG CTempDisplay::HMI_TempResetData(IPC_CServiceMessage &svMsg)
{
	typedef struct
	{
		LONG lData;
		LONG lChannel;
	} PARA;

	PARA stPara;
	stPara.lData = 0;
	stPara.lChannel = 0;
	
	svMsg.GetMsg(sizeof(PARA), &stPara);

	if (stPara.lData >= MAX_TEMP_DATA)
	{
		HMIMessageBox(MSG_OK, "TEMP CALIBRATION RESET DATA", "Invalid Entry.");
		return 0;
	}

	m_stTempCalib[stPara.lChannel][stPara.lData].SetTemp = 0.0;
	m_stTempCalib[stPara.lChannel][stPara.lData].BondTemp = 0.0;

	// sort data
	CString szName = "";
	CString	szSetTemp = "";
	CString	szBondTemp = "";

	ConvertToFileName(stPara.lChannel, &szName, &szSetTemp, &szBondTemp);
	SortTempCalibData(m_stTempCalib[stPara.lChannel]);

	// Set data to file
	FILE *fp = fopen(szName, "wt");
	if (fp != NULL)
	{
		for (INT i = 0; i < MAX_TEMP_DATA; i++)
		{
			fprintf(fp, "%.5f,%.5f\n", m_stTempCalib[stPara.lChannel][i].SetTemp, m_stTempCalib[stPara.lChannel][i].BondTemp);
		}
		fclose(fp);
	}

	fp = fopen(szSetTemp, "wt");
	if (fp != NULL)
	{
		fprintf(fp, "SetTemp (DegC),");

		for (INT i = 0; i < MAX_TEMP_DATA; i++)
		{
			if (i < MAX_TEMP_DATA - 1)
			{
				fprintf(fp, "%.5f,", m_stTempCalib[stPara.lChannel][i].SetTemp);
			}
			else
			{
				fprintf(fp, "%.5f", m_stTempCalib[stPara.lChannel][i].SetTemp);
			}
		}
		fclose(fp);
	}	

	fp = fopen(szBondTemp, "wt");
	if (fp != NULL)
	{
		fprintf(fp, "BondTemp (DegC),");

		for (INT i = 0; i < MAX_TEMP_DATA; i++)
		{
			if (i < MAX_TEMP_DATA - 1)
			{
				fprintf(fp, "%.5f,", m_stTempCalib[stPara.lChannel][i].BondTemp);
			}
			else
			{
				fprintf(fp, "%.5f", m_stTempCalib[stPara.lChannel][i].BondTemp);
			}
		}
		fclose(fp);
	}

	m_bValidTemp[stPara.lChannel] = SetTempToBondTemp(m_dHeaterSetTemp[stPara.lChannel], m_stTempCalib[stPara.lChannel], &m_dBondPtTemp[stPara.lChannel]);
	return 0;
}

LONG CTempDisplay::HMI_TempResetAllData(IPC_CServiceMessage &svMsg)
{
	LONG lChannel;
	svMsg.GetMsg(sizeof(LONG), &lChannel);

	ResetAllData(lChannel);
	return 0;
}

LONG CTempDisplay::HMI_SetTempLmt(IPC_CServiceMessage &svMsg)
{
	CLitecCtrl  *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");
	
	DOUBLE dSetTemp = 0;
	DOUBLE dTimeLmt = 30;
	DOUBLE dTempLmt = 0;
	CString str;

	LONG lChannel;
	svMsg.GetMsg(sizeof(LONG), &lChannel);

	if (pCLitecCtrl->GetTempLmt(lChannel, &dSetTemp) != HEATER_OP_OK)
	{
		return 1;
	}
	CString szOldValue;
	szOldValue.Format("Value = %ld", dSetTemp);
	if (HMINumericKeys("Set Temperature Limit(DegC)", 450.0, 25, &dSetTemp))
	{
		if (pCLitecCtrl->SetTempLmt(lChannel, dSetTemp) != HEATER_OP_OK)
		{
			HMIMessageBox(MSG_OK, "Set Temp Limit", "Operation Fail");
		}
		else
		{
			CString szNewValue;
			szNewValue.Format("Value = %ld", dSetTemp);
			LogParameter(__FUNCTION__, pCLitecCtrl->GetLitecChName(lChannel) + " Temperature Limit", szOldValue, szNewValue);
		}
	}

	if (rMSG_CONTINUE == HMIMessageBox(MSG_CONTINUE_CANCEL, "OPTION", "Set Ramping Protection As Well?"))
	{
		if (rMSG_CONTINUE == HMIMessageBox(MSG_CONTINUE_CANCEL, "OPTION", "Use Default Value?"))
		{
			if (pCLitecCtrl->SetRampingProtection(lChannel) != HEATER_OP_OK)
			{
				HMIMessageBox(MSG_OK, "Set Ramping Protection", "Operation Fail");
			}
		}
		else
		{
			if (
				HMINumericKeys("Set Time Limit(s)", 5000.0, 0, &dTimeLmt) &&
				HMINumericKeys("Set Temperature Limit(DegC)", 10.0, 0, &dTempLmt)
			   )
			{
				if (pCLitecCtrl->SetRampingProtection(lChannel, (LONG)dTimeLmt, dTempLmt) != HEATER_OP_OK)
				{
					HMIMessageBox(MSG_OK, "Set Ramping Protection", "Operation Fail");
				}
			}
			else
			{
				HMIMessageBox(MSG_OK, "OPTION", "Ramping Protection remain unchanged.");
			}
		}
	}
	
	return 0;
}

LONG CTempDisplay::HMI_PrintLitecInformation(IPC_CServiceMessage &svMsg)
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	pCLitecCtrl->PrintLitecInfomation();

	HMIMessageBox(MSG_OK, "PRINT INFORMATION", "The file is printed! [D:\\sw\\AC9000\\Data\\Litec Information.log]");

	return 1;	
}

LONG CTempDisplay::HMI_SetDynOutputLmt(IPC_CServiceMessage &svMsg) //20130912
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	//pCLitecCtrl->SetDynamicOutputLimit(0.75);
	pCLitecCtrl->SetDynamicOutputLimit(1.0);

	return 0;
}

LONG CTempDisplay::HMI_ResetAllHeater(IPC_CServiceMessage &svMsg) //20130413
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");
	CAdamCtrl *pCAdamCtrl = (CAdamCtrl*)m_pModule->GetStation("AdamCtrl");
	INT i;	
	BOOL bOpDone = TRUE;

	// Avoid Display Error Message when Press EMO
	if (!pCLitecCtrl->m_fHardware)
	{
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
		return TRUE;
	}

	// Set Heater SetTemp
	// Power On Heater
	for (i = 0; i < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; i++)
	{
		pCLitecCtrl->PowerOnLitec(i, ON);
		pCLitecCtrl->PowerOnLitec(i, OFF);
		
		//in case channel failed during AT
		pCLitecCtrl->TurnOnLitecAT(i, OFF);
	}

	if (m_bMonTemp)
	{
		pCAdamCtrl->ResetChannelStatus();
	}

	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return TRUE;
}

LONG CTempDisplay::HMI_ResetHeaterChannel(IPC_CServiceMessage &svMsg)
{
	BOOL bOpDone = TRUE;
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");
	CAdamCtrl *pCAdamCtrl = (CAdamCtrl*)m_pModule->GetStation("AdamCtrl");

	LONG lChannel;
	svMsg.GetMsg(sizeof(LONG), &lChannel);

	pCLitecCtrl->PowerOnLitec(lChannel, ON);
	pCLitecCtrl->PowerOnLitec(lChannel, OFF);
	
	//in case channel failed during AT
	pCLitecCtrl->TurnOnLitecAT(lChannel, OFF);

	if (m_bMonTemp)
	{
		pCAdamCtrl->ResetChannelStatus();
	}

	svMsg.InitMessage(sizeof(BOOL), &bOpDone); //20130318
	return 1;
}

BOOL CTempDisplay::LogTempretureInfo(CString &szLabel) //20150629
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	FILE *fp = NULL;
	SYSTEMTIME ltime;
	GetLocalTime(&ltime);

	CString szFileName;
	szFileName.Format("D:\\sw\\AC9000\\Data\\Temperature_%04u%02u%02u.csv", ltime.wYear, ltime.wMonth, ltime.wDay);

	fp = fopen(szFileName.GetString(), "a+");
	if (fp != NULL)
	{
		clock_t clkNow;
		__time64_t ltime;
		CString szTime = "";
		clkNow = clock();
		_time64(&ltime);
		szTime = _ctime64(&ltime);
		szTime.TrimRight('\n');
		fseek(fp, 0, SEEK_END);
		if (ftell(fp) < 10)
		{
			fprintf(fp, "Date: " + szTime + "\tTime Temperature: \tACF1\tACF2\tPB1\tPB2\tMB1\tMB2\tMB1_Mon\tMB2_Mon\tMB3\tMB4\tMB3_Mon\tMB4_Mon\tMB1_WH\tMB2_WH\tMB3_WH\tMB4_WH\n");
		}

/*
		m_bHeaterOn[i]
		m_bHeaterATOn[i]
		bHeaterFailed[i]
		m_dHeaterTemp[ACF1_CH1]
*/
		fprintf(fp, "%s %s\t%.3f\t%.2f\t%.2f\t%.2f\t%.2f\t	%.2f\t%.2f\t%.2f\t%.2f\t	%.2f\t%.2f\t%.2f\t%.2f\t	%.2f\t%.2f\t%.2f\t%.2f\n", 
			(const char*)szLabel, 
			(const char*)szTime, 
			(DOUBLE)(clkNow - pAppMod->m_clkBondStartTime) / CLOCKS_PER_SEC,

			m_dHeaterTemp[ACF1_CH1]//,
			//m_dHeaterTemp[ACF2_CH2],
			//m_dHeaterTemp[PB1_CH3],
			//m_dHeaterTemp[PB2_CH4],

			//m_dHeaterTemp[MB1_CTRL_CH1],
			//m_dHeaterTemp[MB2_CTRL_CH2],
			//m_dHeaterTemp[MB1_MON_CH3],
			//m_dHeaterTemp[MB2_MON_CH4],

			//m_dHeaterTemp[MB3_CTRL_CH1],
			//m_dHeaterTemp[MB4_CTRL_CH2],
			//m_dHeaterTemp[MB3_MON_CH3],
			//m_dHeaterTemp[MB4_MON_CH4],

			//m_dHeaterTemp[MB1_WH_CH1],
			//m_dHeaterTemp[MB2_WH_CH2],
			//m_dHeaterTemp[MB3_WH_CH3],
			//m_dHeaterTemp[MB4_WH_CH4]
			);

		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

// Saving / loading parameters
LONG CTempDisplay::IPC_SaveMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString szName = "";

	for (INT k = 0; k < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; k++)
	{
		szName.Format("m_dMonOffset%ld", k + 1);
		pAppMod->m_smfMachine[GetStnName()]["TempMon"][szName]		= m_dMonOffset[k];
		szName.Format("m_dMonTol%ld", k + 1);
		pAppMod->m_smfMachine[GetStnName()]["TempMon"][szName]		= m_dMonTol[k];
		szName.Format("m_dHeater%ldTempTol", k);
		pAppMod->m_smfMachine[GetStnName()]["Heater"][szName]		= m_dHeaterTempTol[k];

		// saving temp calibration
		for (INT i = 0; i < MAX_TEMP_DATA; i++)
		{
			szName.Format("m_stTempCalib[%ld][%ld]", k, i);
			pAppMod->m_smfMachine[GetStnName()]["Calib"][szName]["SetTemp"]		= m_stTempCalib[k][i].SetTemp;
			pAppMod->m_smfMachine[GetStnName()]["Calib"][szName]["BondTemp"]	= m_stTempCalib[k][i].BondTemp;
		}
	}

	return CAC9000Stn::IPC_SaveMachineParam();
}

LONG CTempDisplay::IPC_LoadMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");
	CString szName = "";
	CString szSetTemp = "";
	CString szBondTemp = "";

	for (INT k = 0; k < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; k++)
	{
		szName.Format("m_dMonOffset%ld", k + 1);
		m_dMonOffset[k]		= pAppMod->m_smfMachine[GetStnName()]["TempMon"][szName];
		szName.Format("m_dMonTol%ld", k + 1);
		m_dMonTol[k]		= pAppMod->m_smfMachine[GetStnName()]["TempMon"][szName];
		szName.Format("m_dHeater%ldTempTol", k);
		//m_dHeaterTempTol[k]		= pAppMod->m_smfDevice[GetStnName()]["Heater"][szName];
		m_dHeaterTempTol[k]		= pAppMod->m_smfMachine[GetStnName()]["Heater"][szName]; //20130724

		// loading temp calibration
		for (INT i = 0; i < MAX_TEMP_DATA; i++)
		{
			szName.Format("m_stTempCalib[%ld][%ld]", k, i);
			m_stTempCalib[k][i].SetTemp		= pAppMod->m_smfMachine[GetStnName()]["Calib"][szName]["SetTemp"];
			m_stTempCalib[k][i].BondTemp	= pAppMod->m_smfMachine[GetStnName()]["Calib"][szName]["BondTemp"];
		}
	}
	
	for (INT i = 0; i < NUM_OF_HEATER_CHANNEL; i++)
	{
		if (m_dMonTol[i] < 5.0)
		{
			m_dMonTol[i] = 5.0;
		}
	}

	return CAC9000Stn::IPC_LoadMachineParam();
}

LONG CTempDisplay::IPC_SaveDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString szName = "";

	for (INT k = 0; k < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; k++)
	{
		szName.Format("m_dHeater%ldSetTemp", k);
		pAppMod->m_smfDevice[GetStnName()]["Heater"][szName] = m_dHeaterSetTemp[k];
		//szName.Format("m_dHeater%ldTempTol", k);
		//pAppMod->m_smfDevice[GetStnName()]["Heater"][szName] = m_dHeaterTempTol[k];
	}	

	return CAC9000Stn::IPC_SaveDeviceParam();
}

LONG CTempDisplay::IPC_LoadDeviceParam()
{
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString szName = "";
	CString szSetTemp = "";
	CString szBondTemp = "";

	for (INT k = 0; k < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; k++)
	{
		szName.Format("m_dHeater%ldSetTemp", k);
		m_dHeaterSetTemp[k]		= pAppMod->m_smfDevice[GetStnName()]["Heater"][szName];
		//szName.Format("m_dHeater%ldTempTol", k);
		//m_dHeaterTempTol[k]		= pAppMod->m_smfDevice[GetStnName()]["Heater"][szName];

		//pCLitecCtrl->SetLitecTemp(k, m_dHeaterSetTemp[k]);	//Milton
		pCLitecCtrl->m_dLitecSetTemp[k] = m_dHeaterSetTemp[k];
	}	


	////Set data to files
	FILE *fp = NULL;
	fp = NULL;

	for (k = 0; k < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; k++)
	{
		if (!IsLitecHeaterExist(k)) //20140530
		{
			continue;
		}
		// Set data to file
		ConvertToFileName(k, &szName, &szSetTemp, &szBondTemp);
	
		fp = fopen(szName, "wt");
		if (fp != NULL)
		{
			for (INT i = 0; i < MAX_TEMP_DATA; i++)
			{
				fprintf(fp, "%.5f,%.5f\n", m_stTempCalib[k][i].SetTemp, m_stTempCalib[k][i].BondTemp);
			}
			fclose(fp);
		}

	
		fp = fopen(szSetTemp, "wt");
		if (fp != NULL)
		{
			fprintf(fp, "SetTemp (DegC),");

			for (INT i = 0; i < MAX_TEMP_DATA; i++)
			{
				if (i < MAX_TEMP_DATA - 1)
				{
					fprintf(fp, "%.5f,", m_stTempCalib[k][i].SetTemp);
				}
				else
				{
					fprintf(fp, "%.5f", m_stTempCalib[k][i].SetTemp);
				}
			}
			fclose(fp);
		}

		fp = fopen(szBondTemp, "wt");
		if (fp != NULL)
		{
			fprintf(fp, "BondTemp (DegC),");

			for (INT i = 0; i < MAX_TEMP_DATA; i++)
			{
				if (i < MAX_TEMP_DATA - 1)
				{
					fprintf(fp, "%.5f,", m_stTempCalib[k][i].BondTemp);
				}
				else
				{
					fprintf(fp, "%.5f", m_stTempCalib[k][i].BondTemp);
				}
			}
			fclose(fp);
		}
	}

	for (INT i = 0; i < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; i++)
	{
		if (m_dHeaterTempTol[i] < 5.0)
		{
			m_dHeaterTempTol[i] = 5.0;
		}
	}

	return CAC9000Stn::IPC_LoadDeviceParam();
}
