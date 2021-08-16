/////////////////////////////////////////////////////////////////
//	LitecCtrl.cpp : interface of the CLitecCtrl class
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
#include "Cal_Util.h"

#define LITECCTRL_EXTERN
#include "LitecCtrl.h"
#include "TempDisplay.h"
#include "AdamCtrl.h"

#define LOBIT(w)           ((BYTE)((DWORD_PTR)(w) & 0xf))
#define HIBIT(w)           ((BYTE)((DWORD_PTR)(w) >> 4))

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CLitecCtrl, CAC9000Stn)

CLitecCtrl::CLitecCtrl()
{
	INT i;
	INT k;	
	
	for (k = 0; k < NUM_OF_LITEC; k++)
	{
		m_hLitecPort[k]					= NULL;
		m_hLitecMCB[k]					= NULL;

		for (i = 0; i < MAX_LTC_CH; i++)
		{
			m_hLitecChn[((k * MAX_LTC_CH) + i)]	= NULL;
		}

		m_stLitecVersion[k]				= "---";
		m_bLitecInit[k]					= FALSE;
	}

	//m_stLitecVersion[0]				= "---";
	//m_stLitecVersion[1]				= "---";
	/*m_bLitecInit					= FALSE;*/

	for (i = 0; i < NUM_OF_LITEC * MAX_LTC_CH; i++)
	{
		m_bLitecOn[i]				= FALSE;
		m_dLitecTemp[i]				= 0.0;
		m_dLitecSetTemp[i]			= 0.0;
		m_bLitecATOn[i]				= FALSE;
		m_bLitecFailed[i]			= FALSE;
	}

	//AutoTune
	for (i = 0; i < NUM_OF_LITEC * MAX_LTC_CH; i++)
	{
		m_dwATTimer[i]				= 0;
		m_ulATCounter[i]			= 0;
		m_bIsLearningAT[i]			= FALSE;

		for (INT j = 0; j < AUTOTUNE_BUFFER_SIZE; j++)
		{
			m_fATBuffer[i][j]		= 0;
		}		
		
		m_bLearningHOD[i]			= FALSE;
		m_bLearningRMS[i]			= FALSE;
	}
	//m_bLearningHOD					= FALSE;
	//m_bLearningRMS					= FALSE;

	m_stTuningSetting.ucMethod		= EBTC_TNMTHD_RELAY;	//EBTC_TNMTHD_STEP
	m_stTuningSetting.dSetPoint		= 0.0;
	m_stTuningSetting.dErrBound		= 0.05f;
	m_stTuningSetting.ucInChn		= 0x1;
	m_stTuningSetting.fStepTarget	= 0;

	m_nCurIndex						= 0;

	m_stLitec[0].nComPortNum		= COM1;		//Litec1
	m_stLitec[0].nLitecType			= CONSTANT_HEAT;

	//m_stLitec[1].nComPortNum		= COM3;		//Litec2
	//m_stLitec[1].nLitecType			= CONSTANT_HEAT;	//PULSE_HEAT

	//m_stLitec[2].nComPortNum		= COM4;		//Litec3
	//m_stLitec[2].nLitecType			= CONSTANT_HEAT;	//PULSE_HEAT

	//m_stLitec[3].nComPortNum		= COM5;		//Litec4
	//m_stLitec[3].nLitecType			= CONSTANT_HEAT;


}

CLitecCtrl::~CLitecCtrl()
{
}

BOOL CLitecCtrl::InitInstance()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	CAC9000Stn::InitInstance();
	if (!InitLitec())
	{
		pAppMod->m_bHWInitError = TRUE;
		pAppMod->m_bAllLitecInited = FALSE;
	}
	else
	{
		pAppMod->m_bAllLitecInited = TRUE;

		// updated Litec Version
		pAppMod->m_szLitecVersion1		= m_stLitecVersion[0];
		//pAppMod->m_szLitecVersion2		= m_stLitecVersion[1];
		//pAppMod->m_szLitecVersion3		= m_stLitecVersion[2];
		//pAppMod->m_szLitecVersion4		= m_stLitecVersion[3];
	}

	return TRUE;
}

INT CLitecCtrl::ExitInstance()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	//do //20121231 the Adam only calls device close in their exitInstance() funcation. Therefore litec control is not required to wait for the Adam
	//{
	//	Sleep(100);
	//} while (!pAppMod->IsAdamExit());



	//if (m_bLitecInit)
	
	if (pAppMod->m_bAllLitecInited) 
	{
		DisplayMessage("Exit Trun off All heater");

		for (INT i = 0; i < NUM_OF_LITEC * MAX_LTC_CH; i++)
		{
			if (
				i == ACF1_CH1 		//Litec Valid Channels

				/*		nChNum == MB5_CTRL_CH5 ||
				nChNum == MB6_CTRL_CH6 ||
				nChNum == MB5_WH_CH7 ||
				nChNum == MB6_WH_CH8*/ 

			   )
			{
				PowerOnLitec(i, FALSE);
			}
		}


		for (INT i = 0; i < NUM_OF_LITEC; i++)
		{
			if (m_hLitecMCB[i] != NULL)	// 20140520 Yip: Check NULL For Litec Handles
			{
				ec_release_mcb(m_hLitecMCB[i]);
			}
			if (m_hLitecPort[i] != NULL)	// 20140520 Yip: Check NULL For Litec Handles
			{
				ec_close(m_hLitecPort[i]);
			}
		}
	}
	return CAC9000Stn::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CLitecCtrl::Operation()
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
		Sleep(100);
	}
	else
	{
		Sleep(5*100);
	}
}	


/////////////////////////////////////////////////////////////////
//Update Functions
/////////////////////////////////////////////////////////////////
BOOL CLitecCtrl::IsLitecValidCh(INT nChNum)
{
	//return TRUE;
	BOOL bResult = FALSE;
	if (
		nChNum == ACF1_CH1 ||		//Litec Valid Channels
		nChNum == ACF1WH_CH3 ||
		nChNum == ACF2WH_CH4

/*		nChNum == MB5_CTRL_CH5 ||
		nChNum == MB6_CTRL_CH6 ||
		nChNum == MB5_WH_CH7 ||
		nChNum == MB6_WH_CH8*/ 

	   )
	{
		bResult = TRUE;
	}
	return bResult;
}

VOID CLitecCtrl::UpdateOutput()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CTempDisplay *pCTempDisplay = (CTempDisplay*)m_pModule->GetStation("TempDisplay");
	CAdamCtrl *pCAdamCtrl = (CAdamCtrl*)m_pModule->GetStation("AdamCtrl");

	if ((m_qState == UN_INITIALIZE_Q) || (m_qState == DE_INITIAL_Q))
	{
		return;
	}

	if (pAppMod->m_bHWInitError)
	{
		return;
	}

	if (pAppMod->m_lAction == glAMS_SYSTEM_EXIT)
	{
		return;
	}

	if (!m_fHardware)
	{
		return;
	}

	try
	{
		//if (m_bLitecInit && (m_nCurIndex < MAX_LTC_CH))
		if (pAppMod->m_bAllLitecInited && IsLitecValidCh(m_nCurIndex) && (m_nCurIndex < NUM_OF_LITEC * MAX_LTC_CH))	
		{
			if (Command() == glPRESTART_COMMAND || Command() == glSTOP_COMMAND || g_bStopCycle)
			{
				return;
			}

			//for (INT i = 0; i < MAX_LTC_CH; i++) //alfred ???
			//{
			//	if (m_bIsLearningAT[(m_nCurIndex + i) % MAX_LTC_CH])
			//	{
			//		m_bLitecATOn[(m_nCurIndex + i) % MAX_LTC_CH] = IsLitecLearningAT((m_nCurIndex + i) % MAX_LTC_CH);
			//	}
			//}


			if (m_bIsLearningAT[m_nCurIndex] && pCTempDisplay->m_bHeaterExist[m_nCurIndex] == TRUE)
			{
				m_bLitecATOn[m_nCurIndex] = IsLitecLearningAT(m_nCurIndex);
			}

				//UpdateLitecStatus(m_nCurIndex);	//debug

			if (!m_bIsLearningAT[m_nCurIndex])
			{
				//if(m_bLearningRMS)
				//{
				//	IsLitecLearningRMS(&m_bLearningRMS);
				//}

				//Litec Temp, Status
				if (!m_bLitecFailed[m_nCurIndex])
				{
					UpdateLitecStatus(m_nCurIndex);				
				}
				else
				{
					m_bLitecOn[m_nCurIndex] = IsLitecOn(m_nCurIndex);
				}			

			}
			else
			{
				m_bLitecOn[m_nCurIndex] = TRUE;	//Since Litec would not set heater to be ON during AutoTune, set it manually.
			}

			GetLitecTemp(m_nCurIndex, &m_dLitecTemp[m_nCurIndex]);
			
			//Sleep(500);//Milton

			//if (   //Litec Monitor CH Update; m_dLitecTemp[m_nCurIndex] >> AdamTemp[Heater Ctrl Ch] 
			//	m_nCurIndex == MB1_MON_CH3 ||
			//	m_nCurIndex == MB2_MON_CH4 ||
			//	m_nCurIndex == MB3_MON_CH3 ||
			//	m_nCurIndex == MB4_MON_CH4 
			//)
			//{
			//	if (m_nCurIndex == MB1_MON_CH3)
			//	{
			//		pCAdamCtrl->m_dAdamTemp[MB1_CTRL_CH1] = m_dLitecTemp[MB1_MON_CH3];
			//	}
			//	else if (m_nCurIndex == MB2_MON_CH4)
			//	{
			//		pCAdamCtrl->m_dAdamTemp[MB2_CTRL_CH2] = m_dLitecTemp[MB2_MON_CH4];
			//	}
			//	else if (m_nCurIndex == MB3_MON_CH3)
			//	{
			//		pCAdamCtrl->m_dAdamTemp[MB3_CTRL_CH1] = m_dLitecTemp[MB3_MON_CH3];
			//	}
			//	else if (m_nCurIndex == MB4_MON_CH4)
			//	{
			//		pCAdamCtrl->m_dAdamTemp[MB4_CTRL_CH2] = m_dLitecTemp[MB4_MON_CH4];
			//	}
			//}
		}		

		m_nCurIndex++;
		if (m_nCurIndex >= MAX_LTC_CH * NUM_OF_LITEC)
		{
			m_nCurIndex = 0;
		}

		//Sleep(100);
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

}

VOID CLitecCtrl::RegisterVariables()
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
//Wrapped Functions
/////////////////////////////////////////////////////////////////

BOOL CLitecCtrl::InitLitec()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString szMsg = _T("");
	CString szDisplay = _T("");

	//	m_bLitecInit = FALSE;

	INT		i = 0;
	INT		j = 0;
	EBTC_INFO stEbtcInfo = {0, 0, 0, 0, 0, 0, 0, 0};
	CString szChannel = "";


	for (i = 0; i < NUM_OF_LITEC; i++)
	{
		m_bLitecInit[i] = FALSE;
	}

	// Init Litec
	if (!m_fHardware)
	{
		szMsg = _T("   ") + GetName() + " --- Not Enabled";
		DisplayMessage(szMsg);

		return TRUE;
	}
	else
	{	
		// Litec at com port 1,3,4,5
		for (INT nLitecNum = 0; nLitecNum < NUM_OF_LITEC; nLitecNum++)
			//INT nLitecNum = 0;
		{

			// open comm port connection
			m_hLitecPort[nLitecNum] = ec_open((INT)(m_stLitec[nLitecNum].nComPortNum), 115200); 
			Sleep(500);
			if (m_hLitecPort[nLitecNum] == NULL || getKlocworkFalse()) //klocwork fix 20121211
			{
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				szMsg.Format("ERROR: Litec%d ComPort%d Error!", nLitecNum + 1, (INT)(m_stLitec[nLitecNum].nComPortNum));
				DisplayMessage(szMsg);
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				return FALSE;
			}

			// create mcb
			m_hLitecMCB[nLitecNum] = ec_create_mcb(m_hLitecPort[nLitecNum], 0, "Litec");	// mcmid = 0;


			if (m_hLitecMCB[nLitecNum] == NULL)
			{
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				szMsg.Format("ERROR: Litec%d Init MCB Error!", nLitecNum + 1);
				DisplayMessage(szMsg);
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				for (j = nLitecNum; j >= 0; j--)
				{
					ec_close(m_hLitecPort[j]);
				}
				return FALSE;
			}

			// create channel
			//if (nLitecNum == 0 || nLitecNum == 3)
			//{
				for (i = 0; i < MAX_LTC_CH; i++)
				{	
					m_hLitecChn[(i + (nLitecNum * MAX_LTC_CH))] = ec_create_channel(m_hLitecMCB[nLitecNum], i + 1, szChannel);

					if (m_hLitecChn[(i + (nLitecNum * MAX_LTC_CH))] == NULL)
					{
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						szMsg.Format("ERROR: Litec%d Init Channel %d Error!", nLitecNum + 1, i + 1);
						DisplayMessage(szMsg);
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						for (j = nLitecNum; j >= 0; j--)
						{
							ec_release_mcb(m_hLitecMCB[j]);
							ec_close(m_hLitecPort[j]);
						}				
						return FALSE;
					}
				}
			//}
			//else
			//{
			////2, 3
			//	for (i = 0; i < 4; i++)
			//	{	
			//		m_hLitecChn[(i + (nLitecNum * MAX_LTC_CH))] = ec_create_channel(m_hLitecMCB[nLitecNum], i + 1, szChannel);

			//		if (m_hLitecChn[(i + (nLitecNum * MAX_LTC_CH))] == NULL)
			//		{
			//			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			//			szMsg.Format("ERROR: Litec%d Init Channel %d Error!", nLitecNum + 1, i + 1);
			//			DisplayMessage(szMsg);
			//			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			//			for (j = nLitecNum; j >= 0; j--)
			//			{
			//				ec_release_mcb(m_hLitecMCB[j]);
			//				ec_close(m_hLitecPort[j]);
			//			}				
			//			return FALSE;
			//		}
			//	}
			//}

			// get version
			DWORD dwLitecVersion = 0;
			dwLitecVersion = litec_lib_version();
			m_stLitecLibVersion = FormatBCDVersion(dwLitecVersion);
			szMsg = "--- Litec Library Version: " + m_stLitecLibVersion;
			DisplayMessage(szMsg);
			Sleep(500); //Milton
			//Get Litec model part no.
			char szModel[50] = {0};
			if (0 != litec_get_model_partno(m_hLitecMCB[nLitecNum], &szModel[0]))
			{
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				szDisplay.Format(_T("Litec%d get model part no. Error! %s"), nLitecNum + 1, ec_report_last_error());
				DisplayMessage(szDisplay);
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

				for (j = nLitecNum; j >= 0; j--)
				{
					//		if( j != 1)			//alfred???
					//		{
					ec_release_mcb(m_hLitecMCB[j]);
					ec_close(m_hLitecPort[j]);
					//		}
				}				
				return FALSE;
			}

			szMsg.Format("--- Litec%d Model Part no.: %s", nLitecNum + 1, szModel);
			DisplayMessage(szMsg);

			if (0 != ebtc_get_information(m_hLitecMCB[nLitecNum], &stEbtcInfo))
			{
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				szDisplay.Format(_T("Litec%d Init Error! %s"), nLitecNum + 1, ec_report_last_error());
				DisplayMessage(szDisplay);
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

				for (j = nLitecNum; j >= 0; j--)
				{
					ec_release_mcb(m_hLitecMCB[j]);
					ec_close(m_hLitecPort[j]);
				}				
				return FALSE;
			}
			m_stLitecVersion[nLitecNum] = FormatBCDVersion(stEbtcInfo.vfirmware);	
			szMsg.Format("--- Litec%d Firmware Version: %s", nLitecNum + 1, m_stLitecVersion[nLitecNum]);
			DisplayMessage(szMsg);			
			szMsg.Format("		status of connection %ld", stEbtcInfo.bConnected);
			DisplayMessage(szMsg);
		}

		for (INT nLitecNum = 0; nLitecNum < NUM_OF_LITEC; nLitecNum++)
			//nLitecNum = 0;
		{
			////Test AlarmBand
			////Disable alarmBand (temp out of range alarm)
			//if (0 != ebtc_mask_enable_alarm(m_hLitecMCB[nLitecNum], MaskHi, MaskLo, DISABLE))
			//{
			//	DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			//	DisplayMessage("ERROR: Litec Init Error! Disable AlarmBand Failed.");
			//	DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

			//	for (j = nLitecNum; j >= 0; j--)
			//	{
			//		ec_release_mcb(m_hLitecMCB[j]);
			//		ec_close(m_hLitecPort[j]);
			//	}				
			//	return FALSE;
			//}			

		//	if (nLitecNum == 0 || nLitecNum == 3)
			{
				for (i = 0; i < MAX_LTC_CH; i++)
				{
					if (0 != ebtc_enable(m_hLitecChn[(i + (nLitecNum * MAX_LTC_CH))], DISABLE))
					{
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						DisplayMessage("ERROR: Litec Init Error! Power Off Litec Failed.");
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

						for (j = nLitecNum; j >= 0; j--)
						{
							ec_release_mcb(m_hLitecMCB[j]);
							ec_close(m_hLitecPort[j]);
						}				
						return FALSE;
					}

					if (0 != ebtc_set_alarmband(m_hLitecChn[(i + (nLitecNum * MAX_LTC_CH))], LITEC_ALARM_BAND_TEMPERATURE))
					{
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						DisplayMessage("ERROR: Litec Init Error! Set AlarmBand Failed.");
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

						for (j = nLitecNum; j >= 0; j--)
						{
							ec_release_mcb(m_hLitecMCB[j]);
							ec_close(m_hLitecPort[j]);
						}				
						return FALSE;
					}

					if (HEATER_FAILED == SetHODThreshold(i, 0xA))
					{
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						DisplayMessage("ERROR: Litec Init Error! Set HOD Threshold Failed.");
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

						for (j = nLitecNum; j >= 0; j--)
						{
							ec_release_mcb(m_hLitecMCB[j]);
							ec_close(m_hLitecPort[j]);
						}				
						return FALSE;
					}
#if 1 //20120308 problem 15
					if (0 != ebtc_hod_enable(m_hLitecChn[(i + (nLitecNum * MAX_LTC_CH))], FALSE))
					{
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						DisplayMessage("ERROR: Litec Init Error! Disable HOD Failed.");
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

						for (j = nLitecNum; j >= 0; j--)
						{
							ec_release_mcb(m_hLitecMCB[j]);
							ec_close(m_hLitecPort[j]);
						}				
						return FALSE;
					}
#endif

					//Control method of Chn 6 and 12 are default ON_OFF_CTRL
					if (i == 5 || i == 11)
					{
						if (0 != litec_set_control_method(m_hLitecChn[(i + (nLitecNum * MAX_LTC_CH))], LITEC_CTRL_PID))
						{
							DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
							szDisplay.Format(_T("Set Control Method Error! %s"), ec_report_last_error());
							DisplayMessage(szDisplay);
							DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

							for (j = nLitecNum; j >= 0; j--)
							{
								ec_release_mcb(m_hLitecMCB[j]);
								ec_close(m_hLitecPort[j]);
							}				
							return FALSE;
						}
					}

					//#if 0 //20130911 for truly dark mark issue. We have to lower the dynamic output for testing
					//				if (
					//					i == MB1_LEFT_HEAD_CH ||
					//					i == MB1_RIGHT_HEAD_CH ||
					//					i == MB1_LEFT_BASE_CH ||
					//					i == MB1_RIGHT_BASE_CH ||
					//					i == MB2_LEFT_HEAD_CH ||
					//					i == MB2_RIGHT_HEAD_CH ||
					//					i == MB2_LEFT_BASE_CH ||
					//					i == MB2_RIGHT_BASE_CH 
					//				)
					//				{
					//					float fLimit = 0.9f;
					//					//LITEC_API int     litec_get_dynamic_output_limit(HANDLE hChn, float *fLimit);
					//					//LITEC_API int     litec_set_dynamic_output_limit(HANDLE hChn, float fLimit);
					//					if (0 != litec_set_dynamic_output_limit(m_hLitecChn[(i + (nLitecNum * MAX_LTC_CH))], fLimit))
					//					{
					//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
					//						szDisplay.Format(_T("litec_set_dynamic_output_limit Error! %s"), ec_report_last_error());
					//						DisplayMessage(szDisplay);
					//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
					//						
					//						for (j = nLitecNum; j >= 0; j--)
					//						{
					//							ec_release_mcb(m_hLitecMCB[j]);
					//							ec_close(m_hLitecPort[j]);
					//						}				
					//						return FALSE;
					//					}
					//
					//					fLimit = 0.0;
					//					if (0 != litec_get_dynamic_output_limit(m_hLitecChn[(i + (nLitecNum * MAX_LTC_CH))], &fLimit))
					//					{
					//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
					//						szDisplay.Format(_T("litec_get_dynamic_output_limit Error! %s"), ec_report_last_error());
					//						DisplayMessage(szDisplay);
					//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
					//						
					//						for (j = nLitecNum; j >= 0; j--)
					//						{
					//							ec_release_mcb(m_hLitecMCB[j]);
					//							ec_close(m_hLitecPort[j]);
					//						}				
					//						return FALSE;
					//					}
					//					else
					//					{
					//						szDisplay.Format("litec_get_dynamic_output_limit ch:%d, fLimit:%f", i, fLimit);
					//						DisplayMessage(szDisplay);
					//					}
					//
					//				}
					//#endif
					// EBTC_SRCTYPE_TC_K -> K-type thermocouple
					// Save physical input channel configuration like T.C. type. Parameter
					if (
						0 != ebtc_set_source_type(m_hLitecMCB[nLitecNum], 1, i + 1, EBTC_SRCTYPE_TC_K) //|| 
						//0 != litec_save_phin_eedata_all(m_hLitecMCB[nLitecNum], i+1) 
					   )
					{
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						DisplayMessage("ERROR: Litec Init Error! Set Source Type Failed.");
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

						for (j = nLitecNum; j >= 0; j--)
						{
							ec_release_mcb(m_hLitecMCB[j]);
							ec_close(m_hLitecPort[j]);
						}				
						return FALSE;
					}		
				}

				if (0 != litec_set_valid_channels(m_hLitecMCB[nLitecNum], MaskLo))
				{
					DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
					DisplayMessage("ERROR: Litec Init Error! Set Valid Channels Failed.");
					DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

					for (j = nLitecNum; j >= 0; j--)
					{
						ec_release_mcb(m_hLitecMCB[j]);
						ec_close(m_hLitecPort[j]);
					}				
					return FALSE;
				}

				m_bLitecInit[nLitecNum] = TRUE;
				DisplayMessage("Litech Init Success (Debug)");
			}

// Pulse Heat Controller
//			else
//			{
//				for (i = 0; i < 4; i++)
//				{
//					if (0 != ebtc_enable(m_hLitecChn[(i + (nLitecNum * MAX_LTC_CH))], DISABLE))
//					{
//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//						DisplayMessage("ERROR: Litec Init Error! Power Off Litec Failed.");
//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//
//						for (j = nLitecNum; j >= 0; j--)
//						{
//							ec_release_mcb(m_hLitecMCB[j]);
//							ec_close(m_hLitecPort[j]);
//						}				
//						return FALSE;
//					}
//
//					if (0 != ebtc_set_alarmband(m_hLitecChn[(i + (nLitecNum * MAX_LTC_CH))], 400))
//					{
//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//						DisplayMessage("ERROR: Litec Init Error! Set AlarmBand Failed.");
//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//
//						for (j = nLitecNum; j >= 0; j--)
//						{
//							ec_release_mcb(m_hLitecMCB[j]);
//							ec_close(m_hLitecPort[j]);
//						}				
//						return FALSE;
//					}
//
//					if (HEATER_FAILED == SetHODThreshold(i, 0xA))
//					{
//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//						DisplayMessage("ERROR: Litec Init Error! Set HOD Threshold Failed.");
//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//
//						for (j = nLitecNum; j >= 0; j--)
//						{
//							ec_release_mcb(m_hLitecMCB[j]);
//							ec_close(m_hLitecPort[j]);
//						}				
//						return FALSE;
//					}
//#if 1 //20120308 problem 15
//					if (0 != ebtc_hod_enable(m_hLitecChn[(i + (nLitecNum * MAX_LTC_CH))], FALSE))
//					{
//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//						DisplayMessage("ERROR: Litec Init Error! Disable HOD Failed.");
//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//
//						for (j = nLitecNum; j >= 0; j--)
//						{
//							ec_release_mcb(m_hLitecMCB[j]);
//							ec_close(m_hLitecPort[j]);
//						}				
//						return FALSE;
//					}
//#endif
//
//					//Control method of Chn 6 and 12 are default ON_OFF_CTRL
//					//if (i == 5 || i == 11)
//					//{
//					//	if (0 != litec_set_control_method(m_hLitecChn[(i + (nLitecNum * MAX_LTC_CH))], LITEC_CTRL_PID))
//					//	{
//					//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//					//		szDisplay.Format(_T("Set Control Method Error! %s"), ec_report_last_error());
//					//		DisplayMessage(szDisplay);
//					//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//					//		
//					//		for (j = nLitecNum; j >= 0; j--)
//					//		{
//					//			ec_release_mcb(m_hLitecMCB[j]);
//					//			ec_close(m_hLitecPort[j]);
//					//		}				
//					//		return FALSE;
//					//	}
//					//}
//
//					//#if 0 //20130911 for truly dark mark issue. We have to lower the dynamic output for testing
//					//				if (
//					//					i == MB1_LEFT_HEAD_CH ||
//					//					i == MB1_RIGHT_HEAD_CH ||
//					//					i == MB1_LEFT_BASE_CH ||
//					//					i == MB1_RIGHT_BASE_CH ||
//					//					i == MB2_LEFT_HEAD_CH ||
//					//					i == MB2_RIGHT_HEAD_CH ||
//					//					i == MB2_LEFT_BASE_CH ||
//					//					i == MB2_RIGHT_BASE_CH 
//					//				)
//					//				{
//					//					float fLimit = 0.9f;
//					//					//LITEC_API int     litec_get_dynamic_output_limit(HANDLE hChn, float *fLimit);
//					//					//LITEC_API int     litec_set_dynamic_output_limit(HANDLE hChn, float fLimit);
//					//					if (0 != litec_set_dynamic_output_limit(m_hLitecChn[(i + (nLitecNum * MAX_LTC_CH))], fLimit))
//					//					{
//					//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//					//						szDisplay.Format(_T("litec_set_dynamic_output_limit Error! %s"), ec_report_last_error());
//					//						DisplayMessage(szDisplay);
//					//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//					//						
//					//						for (j = nLitecNum; j >= 0; j--)
//					//						{
//					//							ec_release_mcb(m_hLitecMCB[j]);
//					//							ec_close(m_hLitecPort[j]);
//					//						}				
//					//						return FALSE;
//					//					}
//					//
//					//					fLimit = 0.0;
//					//					if (0 != litec_get_dynamic_output_limit(m_hLitecChn[(i + (nLitecNum * MAX_LTC_CH))], &fLimit))
//					//					{
//					//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//					//						szDisplay.Format(_T("litec_get_dynamic_output_limit Error! %s"), ec_report_last_error());
//					//						DisplayMessage(szDisplay);
//					//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//					//						
//					//						for (j = nLitecNum; j >= 0; j--)
//					//						{
//					//							ec_release_mcb(m_hLitecMCB[j]);
//					//							ec_close(m_hLitecPort[j]);
//					//						}				
//					//						return FALSE;
//					//					}
//					//					else
//					//					{
//					//						szDisplay.Format("litec_get_dynamic_output_limit ch:%d, fLimit:%f", i, fLimit);
//					//						DisplayMessage(szDisplay);
//					//					}
//					//
//					//				}
//					//#endif
//					// EBTC_SRCTYPE_TC_K -> K-type thermocouple
//					// Save physical input channel configuration like T.C. type. Parameter
//					if (
//						0 != ebtc_set_source_type(m_hLitecMCB[nLitecNum], 1, i + 1, EBTC_SRCTYPE_TC_K) //|| 
//						//0 != litec_save_phin_eedata_all(m_hLitecMCB[nLitecNum], i+1) 
//					   )
//					{
//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//						DisplayMessage("ERROR: Litec Init Error! Set Source Type Failed.");
//						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//
//						for (j = nLitecNum; j >= 0; j--)
//						{
//							ec_release_mcb(m_hLitecMCB[j]);
//							ec_close(m_hLitecPort[j]);
//						}				
//						return FALSE;
//					}		
//				}
//
//				if (0 != litec_set_valid_channels(m_hLitecMCB[nLitecNum], MaskLo))
//				{
//					DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//					DisplayMessage("ERROR: Litec Init Error! Set Valid Channels Failed.");
//					DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//
//					for (j = nLitecNum; j >= 0; j--)
//					{
//						ec_release_mcb(m_hLitecMCB[j]);
//						ec_close(m_hLitecPort[j]);
//					}				
//					return FALSE;
//				}
//
//				m_bLitecInit[nLitecNum] = TRUE;
//				DisplayMessage("Litech Init Success (Debug)");
//			}

			//m_bLitecInit = TRUE;

			
			//if( m_bLitecInit[1] == TRUE || m_bLitecInit[2] == TRUE)
			//{

			//}



		}

	}

	//DWORD dwTickMax = 500;
	//float fLimit = 1;
//#if 0 , Pulse Heat
//	ebtc_ramping_enable(m_hLitecChn[MB1_CTRL_CH1], DISABLE);
//	ebtc_ramping_enable(m_hLitecChn[MB2_CTRL_CH2], DISABLE);	
//	ebtc_ramping_enable(m_hLitecChn[MB3_CTRL_CH1], DISABLE);
//	ebtc_ramping_enable(m_hLitecChn[MB4_CTRL_CH2], DISABLE);
//	ebtc_set_temperature_limit(m_hLitecChn[MB1_CTRL_CH1], 400.0);
//	ebtc_set_temperature_limit(m_hLitecChn[MB2_CTRL_CH2], 400.0);
//	ebtc_set_temperature_limit(m_hLitecChn[MB3_CTRL_CH1], 400.0);
//	ebtc_set_temperature_limit(m_hLitecChn[MB4_CTRL_CH2], 400.0);
//
	EBTC_CTRLPARAM_PID stTunedPID = {0};	// 20150310
	stTunedPID.fKp_d = 2700;
	stTunedPID.fKi_d = 0;
	stTunedPID.fKd_d = 0;
	stTunedPID.fKp_s = 2700;
	stTunedPID.fKi_s = 60;
	stTunedPID.fKd_s = 600;
	stTunedPID.fCBND = 10;
//
//	LTC_FLT_BLK stTunedFfcFilter = {0};	// 20140508 Yip
//	stTunedFfcFilter.wOrder = 1;
//	stTunedFfcFilter.f_a[1] = 0;
//	stTunedFfcFilter.f_b[0] = 92;
//	stTunedFfcFilter.f_b[1] = 0;
//
//	if (!SetCtrlPara(MB1_CTRL_CH1, stTunedPID/*, stTunedFfcFilter*/))	// 20140508 Yip
//	{
////		return FALSE;
//	}
//
//	if (!SetCtrlPara(MB2_CTRL_CH2, stTunedPID/*, stTunedFfcFilter*/))	// 20140508 Yip
//	{
////		return FALSE;
//	}
//
//	if (!SetCtrlPara(MB3_CTRL_CH1, stTunedPID/*, stTunedFfcFilter*/))	// 20140508 Yip
//	{
////		return FALSE;
//	}
//
//	if (!SetCtrlPara(MB4_CTRL_CH2, stTunedPID/*, stTunedFfcFilter*/))	// 20140508 Yip
//	{
////		return FALSE;
//	}
//
//	//SetRampingProtection(MB1_CTRL_CH1); // ,  dwTickMax,  fLimit);
//	//SetRampingProtection(MB2_CTRL_CH2); //,  dwTickMax,  fLimit);
//
//
//	//SetRampingProtection(MB3_CTRL_CH1); //,  dwTickMax,  fLimit);
//	//SetRampingProtection(MB4_CTRL_CH2); //,  dwTickMax,  fLimit);
//
//	ebtc_enable(m_hLitecChn[MB1_CTRL_CH1], DISABLE);
//	ebtc_enable(m_hLitecChn[MB2_CTRL_CH2], DISABLE);
//	ebtc_enable(m_hLitecChn[MB3_CTRL_CH1], DISABLE);
//	ebtc_enable(m_hLitecChn[MB4_CTRL_CH2], DISABLE);
//#endif
	return TRUE;
}

INT CLitecCtrl::UpdateLitecStatus(INT num)
{
	INT		nResult			= HEATER_OP_OK;
	LONG	lStatus			= 0;
	BOOL	bBinStatus[13];
	WORD	wStatus ;

	CString	szDisplay = "";

	INT		j = 0;
	for (INT i = 0; i < 13; i++)
	{
		bBinStatus[i] = FALSE;
	}

	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	if (!m_bLitecFailed[num])
	{
		return HEATER_OP_OK;
	}
	//if (/*num != MB1_MON_CH3 &&
	//	num != MB2_MON_CH4 &&
	//	num != MB3_MON_CH3 &&
	//	num != MB4_MON_CH4 */
	//   )
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
		if (0 != ebtc_get_status(m_hLitecChn[num], &wStatus))
		{
#ifdef HEATER_CONTROL_LOCK //20150714
			m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
			m_bLitecFailed[num] = TRUE;

			CString szDisplay = _T("");
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("Litec%d Ch#%d Litec Get Status Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
			DisplayMessage(szDisplay);
			//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
			//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
			SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20140401
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			nResult = HEATER_FAILED;
		}
		else
		{
#ifdef HEATER_CONTROL_LOCK //20150714
			m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
			lStatus = (LONG)wStatus;
			//std::hex<<lStatus;
			szDisplay.Format(_T("Litec%d Ch#%d Recived Bit Stream %ld"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), lStatus);
			DisplayMessage(szDisplay);

			if (lStatus >= 0x8000)	//max 15 bits status
			{
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				DisplayMessage("Litec Get Status over flow Error!");
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				return HEATER_FAILED;
			}

			// translate the returned value to binary array
			j = 13;
			for (i = 0x2000; i >= 1; j--)
			{
				if (lStatus - i >= 0)
				{
					bBinStatus[j % 13] = TRUE;
					lStatus -= i;
				}

				if (lStatus == 0)
				{
					break;
				}

				i = i / 2;
			}

			// check Litec error exist
			for (i = 0; i < 13; i++)
			{
				switch (i)
				{
				case 0:		//0x0001
					if (bBinStatus[i])
					{
						//channel enabled
						m_bLitecOn[num] = TRUE;
					}
					else
					{
						m_bLitecOn[num] = FALSE;
					}
					break;

				case 1:		//0x0002
					if (bBinStatus[i])
					{
						PowerOnLitec(num, FALSE);
						m_bLitecFailed[num] = TRUE;
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						szDisplay.Format("Litec%d Ch#%d Thermocouple Connection Error!", ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1));
						DisplayMessage(szDisplay);
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						nResult = IDS_TEMP_CTRL_TEMP_SNR_ERR;
					}
					break;

				case 2:		//0x0004
					if (bBinStatus[i])
					{
						PowerOnLitec(num, FALSE);
						m_bLitecFailed[num] = TRUE;
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						szDisplay.Format("Litec%d Ch#%d Cold Junction Sensor Connection Error!", ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1));
						DisplayMessage(szDisplay);
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						nResult = IDS_TEMP_CTRL_COMP_SNR_ERR;
					}
					break;

				case 3:		//0x0008
					if (bBinStatus[i])
					{
						m_bLitecFailed[num] = TRUE;
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						szDisplay.Format("Litec%d Ch#%d Temperature Out of Alarm Band!", ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1));
						DisplayMessage(szDisplay);
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						nResult = IDS_TEMP_CTRL_TEMP_OUT_OF_RANGE_ERR;

					}
					break;

				case 4:		//0x0010
					if (bBinStatus[i])
					{					
						PowerOnLitec(num, FALSE);
						m_bLitecFailed[num] = TRUE;
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						szDisplay.Format("Litec%d Ch#%d Temperature Hit Limit!", ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1));
						DisplayMessage(szDisplay);
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						nResult = IDS_TEMP_CTRL_SET_TEMP_LMT_HIT_ERR;
					}
					break;
#if 1 //20120308 problem 15
				case 5:		//0x0020
					if (bBinStatus[i])
					{
						PowerOnLitec(num, FALSE);
						m_bLitecFailed[num] = TRUE;
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						szDisplay.Format("Litec%d Ch#%d Heater Open Error!", ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1));
						DisplayMessage(szDisplay);
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						nResult = IDS_TEMP_CTRL_BREAKAGE_DETECTED_ERR;
					}
					break;
#endif
				case 6:		//0x0040
					if (bBinStatus[i])
					{
						m_bLitecFailed[num] = TRUE;
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						szDisplay.Format("Litec%d Ch#%d Protection Trigger!", ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1));
						DisplayMessage(szDisplay);
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						nResult = IDS_TEMP_CTRL_PROTECTION_TRIGGERED_ERR;
					}
					break;

				case 7:		//0x0080 
					if (bBinStatus[i])
					{
						//Reserved
					}
					break;

				case 8:		//0x0100
					if (bBinStatus[i])
					{
						m_bLitecFailed[num] = TRUE;
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						szDisplay.Format("Litec%d Ch#%d Noise Protection Triggered!", ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1));
						DisplayMessage(szDisplay);
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						nResult = IDS_TEMP_CTRL_NOISE_PROTECTION_TRIGGERED_ERR;
					}
					break;

				case 9:		//0x0200
					if (bBinStatus[i])
					{
						m_bLitecFailed[num] = TRUE;
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						szDisplay.Format("Litec%d Ch#%d Dynamic Protection Triggered!", ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1));
						DisplayMessage(szDisplay);
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						nResult = IDS_TEMP_CTRL_DYN_PROTECTION_TRIGGERED_ERR;

						// If the channel is enabled but stays in dynamic control phase for more than fTimeSpan
						// seconds, dynamic protection will be triggered: control output will be cleared to protect
						// the heater element from being damaged.
					}
					break;

				case 10:	//0x0400
					if (bBinStatus[i])
					{
						m_bLitecFailed[num] = TRUE;
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						szDisplay.Format("Litec%d Ch#%d Static Protection Triggered!", ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1));
						DisplayMessage(szDisplay);
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						nResult = IDS_TEMP_CTRL_STATIC_PROTECTION_TRIGGERED_ERR;
					}
					break;

				case 11:	//0x0800
					if (bBinStatus[i])
					{
						PowerOnLitec(num, FALSE);
						m_bLitecFailed[num] = TRUE;
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						szDisplay.Format("Litec%d Ch#%d Ramping Protection Triggered!", ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1));
						DisplayMessage(szDisplay);
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						nResult = IDS_TEMP_CTRL_TEMP_RAMP_UP_ERR;
					}
					break;

				case 12:	//0x1000
					if (bBinStatus[i])
					{
						PowerOnLitec(num, FALSE);
						m_bLitecFailed[num] = TRUE;
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						szDisplay.Format("Litec%d Ch#%d Wrong AC Line Freq", ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1));
						DisplayMessage(szDisplay);
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						//nResult = IDS_TEMP_CTRL_TEMP_RAMP_UP_ERR;
					}
					break;
				}
			}


			if (nResult != HEATER_OP_OK)
			{
				SetError(nResult);
				SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
				SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
				szDisplay.Format("Litec%d Ch#%d ", ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1));
				DisplayMessage(__FUNCTION__);
				nResult = HEATER_FAILED;
			}

		}
	}

	return nResult;
}


INT CLitecCtrl::GetAlarmBand(INT num, DOUBLE *pdAlarmBand)
{
	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	if (m_bLitecFailed[num])
	{
		return HEATER_FAILED;
	}

	FLOAT fAlarmBand = 0.0;

#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_get_alarmband(m_hLitecChn[num], &fAlarmBand))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		m_bLitecFailed[num] = TRUE;

		CString szDisplay = _T("");

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("GetAlarmBand Error! %s"), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); 
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return HEATER_FAILED;
	}
	else
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		m_bLitecFailed[num] = FALSE;
	}

	*pdAlarmBand = (DOUBLE)fAlarmBand;

	return HEATER_OP_OK;
}

INT CLitecCtrl::SetAlarmBand(INT num, DOUBLE dAlarmBand)
{
	CString szDisplay = _T("");

	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	if (dAlarmBand > 200.0)
	{
		m_bLitecFailed[num] = TRUE;

		szDisplay.Format("Litec%d Invalid Alarm Band on Ch#%d: %.5f", ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), dAlarmBand);
		DisplayMessage(szDisplay);
	}
	else
	{
		m_bLitecFailed[num] = FALSE;

		FLOAT fAlarmBand;
		fAlarmBand = (FLOAT)dAlarmBand;

#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
		if (0 != ebtc_set_alarmband(m_hLitecChn[num], fAlarmBand))
		{
#ifdef HEATER_CONTROL_LOCK //20150714
			m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("SetAlarmBand Error! %s"), ec_report_last_error());
			DisplayMessage(szDisplay);
			//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
			//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
			SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			return HEATER_FAILED;
		}
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

		// Check whether the Value is set
		DOUBLE dCurrAlarmBand = 0.0;
		GetAlarmBand(num, &dCurrAlarmBand);

		if (dCurrAlarmBand != dAlarmBand)
		{
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("SetAlarmBand Failed! %s"), ec_report_last_error());
			DisplayMessage(szDisplay);
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			return HEATER_FAILED;
		}

		if (0 != litec_save_configuration_all(m_hLitecChn[num]))
		{
			m_bLitecFailed[num] = TRUE;

			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("Set Usr Config Error! %s"), ec_report_last_error());
			DisplayMessage(szDisplay);
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			return HEATER_FAILED;
		}
	}

	if (m_bLitecFailed[num])
	{
		//SetError(IDS_TEMP_CTRL_CTRL_ERR);		//20130222
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		DisplayMessage(__FUNCTION__);
		return HEATER_FAILED;
	}

	return HEATER_OP_OK;
}

INT CLitecCtrl::GetTempLmt(INT num, DOUBLE *pdTempLmt)
{
	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	//if (m_bLitecFailed[num])
	//{
	//	return HEATER_FAILED;
	//}

	FLOAT fTempLmt = 0.0;

#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_get_temperature_limit(m_hLitecChn[num], &fTempLmt))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		m_bLitecFailed[num] = TRUE;

		CString szDisplay = _T("");

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("GetTempLmt Error! %s"), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return HEATER_FAILED;
	}
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
	//else
	//{
	//	m_bLitecFailed[num] = FALSE;
	//}

	*pdTempLmt = (DOUBLE)fTempLmt;

	return HEATER_OP_OK;
}

INT CLitecCtrl::SetTempLmt(INT num, DOUBLE dTempLmt)
{
	CString szDisplay = _T("");

	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	if (dTempLmt > 450.0)
	{
		m_bLitecFailed[num] = TRUE;

		szDisplay.Format("Litec%d Invalid Temperature Limit on Ch#%d: %.5f", ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), dTempLmt);
		DisplayMessage(szDisplay);
	}
	else
	{
		m_bLitecFailed[num] = FALSE;

		FLOAT fTempLmt;
		fTempLmt = (FLOAT)dTempLmt;

#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
		if (0 != ebtc_set_temperature_limit(m_hLitecChn[num], fTempLmt))
		{
#ifdef HEATER_CONTROL_LOCK //20150714
			m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("SetTempLmt Error! %s"), ec_report_last_error());
			DisplayMessage(szDisplay);
			//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
			//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
			SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			return HEATER_FAILED;
		}
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

		Sleep(1000);

		// Check whether the Value is set
		DOUBLE dCurrTempLmt = 0.0;
		GetTempLmt(num, &dCurrTempLmt);

		if (dCurrTempLmt != dTempLmt)
		{
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("SetTempLmt Failed! %s"), ec_report_last_error());
			DisplayMessage(szDisplay);
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			return HEATER_FAILED;
		}

		if (0 != litec_save_configuration_all(m_hLitecChn[num]))
		{
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("SaveUsrConfig. Error! %s"), ec_report_last_error());
			DisplayMessage(szDisplay);
			//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
			//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
			SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			return FALSE;
		}
	}

	if (m_bLitecFailed[num])
	{
		//SetError(IDS_TEMP_CTRL_CTRL_ERR); //20130222
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		DisplayMessage(__FUNCTION__);
		return HEATER_FAILED;
	}

	return HEATER_OP_OK;
}


INT CLitecCtrl::GetRampingProtection(INT num, LONG *plTimeLmt, DOUBLE *pdTempLmt)
{
	CString szDisplay = _T("");

	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	FLOAT	fTempLmt = 0.0;
	DWORD	dwTickMax = 0;

#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_get_ramping_protection(m_hLitecChn[num], &dwTickMax, &fTempLmt))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("GetRampingProtection Error! %s"), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return HEATER_FAILED;
	}
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

	*plTimeLmt = (LONG)dwTickMax;
	*pdTempLmt = (DOUBLE)fTempLmt;

	return HEATER_OP_OK;
}


INT CLitecCtrl::SetRampingProtection(INT num, LONG lTimeLmt, DOUBLE dTempLmt)
{
	CString szDisplay = _T("");

	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_set_ramping_protection(m_hLitecChn[num], (DWORD)lTimeLmt, (FLOAT)dTempLmt))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Set Ramping Protection Error! %s"), ec_report_last_error());
		DisplayMessage(szDisplay);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return HEATER_FAILED;
	}
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

	if (0 != litec_save_configuration_all(m_hLitecChn[num]))
	{
		m_bLitecFailed[num] = TRUE;

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Set Usr Config Error! %s"), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return HEATER_FAILED;
	}

	return HEATER_OP_OK;
}

INT CLitecCtrl::PowerOnLitec(INT num, BOOL bMode)
{
	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	////Debug
	//CString szDebug = "";
	//if(bMode)
	//{
	//	szDebug.Format("Ch %ld Enabled",num);
	//}
	//else
	//{
	//	szDebug.Format("Ch %ld Disabled",num);
	//}
	//DisplayMessage(szDebug);

#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_enable(m_hLitecChn[num], bMode))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		m_bLitecFailed[num] = TRUE;

		CString szDisplay = _T("");

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("PowerOn Litec%d Ch#%d Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
	else
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		m_bLitecFailed[num]	= FALSE;
		m_bLitecOn[num]		= bMode;
	}

	if (m_bLitecFailed[num])
	{
		//SetError(IDS_TEMP_CTRL_CTRL_ERR); //20130222
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num)); //20130110
		DisplayMessage(__FUNCTION__);
		return HEATER_FAILED;
	}
	else if (bMode)
	{
		UpdateLitecStatus(num);
	}
	

	return HEATER_OP_OK;
}


BOOL CLitecCtrl::IsLitecOn(INT num)
{
	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return FALSE;
	}

	//if (m_bLitecFailed[num])
	//{
	//	return FALSE;
	//}

	WORD wStatus;

#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	ebtc_get_status(m_hLitecChn[num], &wStatus);
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

	if (0x0001 == wStatus)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

INT CLitecCtrl::GetLitecTemp(INT num, DOUBLE *p_dTemp)
{
	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		*p_dTemp = 0.0;
		return HEATER_NOT_INIT;
	}

	FLOAT fCurTemp = 0.0;
	
	// Pulse Controller Monitor Ch get temp
	//if (num == MB1_MON_CH3 || num == MB3_MON_CH3) //|| num == MB2_MON_CH4|| num == MB3_MON_CH3|| num ==MB4_MON_CH4)
	//{
	//	ebtc_get_ADCi_data(m_hLitecChn[num], 1, 3, &fCurTemp); //(HANDLE hCB, BYTE ucLCM, BYTE ucCHN, float *fData)
	//}

	//else if (num == MB2_MON_CH4 || num == MB4_MON_CH4)
	//{
	//	ebtc_get_ADCi_data(m_hLitecChn[num], 1, 4, &fCurTemp); //(HANDLE hCB, BYTE ucLCM, BYTE ucCHN, float *fData)
	//}

	//else if (0 != ebtc_get_temperature(m_hLitecChn[num], &fCurTemp))	//ebtc_get_temperature(HANDLE hChn, float *fT);
	
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_get_temperature(m_hLitecChn[num], &fCurTemp))	//ebtc_get_temperature(HANDLE hChn, float *fT);
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		m_bLitecFailed[num] = TRUE;

		CString szDisplay = _T("");

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Litec%d Ch#%d Get Litec Temp Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
#ifdef HEATER_CONTROL_LOCK //20150714
	else
	{
		m_csHeaterCtrlLock.Unlock(); //20150714
	}
#endif  //20150714

	*p_dTemp = (DOUBLE)fCurTemp;

	return HEATER_OP_OK;
}

INT CLitecCtrl::GetLitecSetTemp(INT num, DOUBLE *p_dTemp)
{
	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		*p_dTemp = 0.0;
		return HEATER_NOT_INIT;
	}

	FLOAT fCurTemp = 0.0;

#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_get_setpoint(m_hLitecChn[num], &fCurTemp))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		m_bLitecFailed[num] = TRUE;

		CString szDisplay = _T("");

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Litec%d Ch#%d Get Litec SetTemp Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
	else
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		//m_bLitecFailed[num] = FALSE;
	}

	*p_dTemp = (DOUBLE)fCurTemp;

	if (m_bLitecFailed[num])
	{
		//SetError(IDS_TEMP_CTRL_CTRL_ERR); //20130222
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		DisplayMessage(__FUNCTION__);
		return HEATER_FAILED;
	}

	return HEATER_OP_OK;
}

INT CLitecCtrl::SetLitecTemp(INT num, DOUBLE dTemp)
{
	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	if (dTemp > 500.0)
	{
		m_bLitecFailed[num] = TRUE;

		CString szDisplay = _T("");
		szDisplay.Format("Invalid SetTemp:%.5f", dTemp);
		DisplayMessage(szDisplay);
	}
	else
	{
		m_bLitecFailed[num] = FALSE;

		FLOAT fSetTemp;
		fSetTemp = (FLOAT)dTemp;

#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
		if (0 != ebtc_set_temperature(m_hLitecChn[num], fSetTemp))
		{
#ifdef HEATER_CONTROL_LOCK //20150714
			m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
			m_bLitecFailed[num] = TRUE;

			CString szDisplay = _T("");
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("Litec%d Ch#%d Set Temperature Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
			DisplayMessage(szDisplay);
			//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
			//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
			SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			return HEATER_FAILED;
		}
		else
		{
#ifdef HEATER_CONTROL_LOCK //20150714
			m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
			m_dLitecSetTemp[num] = dTemp;
		}
	}

	if (m_bLitecFailed[num])
	{
		//SetError(IDS_TEMP_CTRL_CTRL_ERR); //20130222
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		DisplayMessage(__FUNCTION__);
		return HEATER_FAILED;
	}

	return HEATER_OP_OK;
}


INT CLitecCtrl::SetTunePara(INT num)
{
	CTempDisplay *pCTempDisplay = (CTempDisplay*)m_pModule->GetStation("TempDisplay");

	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	if (0.0 == m_dLitecSetTemp[num])
	{
		CString szDebug = "";
		szDebug.Format("Litec%d Ch#%d SetTemp is zero", ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1));
		DisplayMessage(szDebug);
		//m_dLitecSetTemp[num] = pCTempDisplay->m_dHeaterSetTemp[num];

		//SetError(IDS_TEMP_CTRL_CTRL_ERR); //20130222
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		DisplayMessage(__FUNCTION__);
		return HEATER_FAILED;
	}

	m_stTuningSetting.ucMethod		= EBTC_TNMTHD_RELAY; 
	m_stTuningSetting.dSetPoint		= m_dLitecSetTemp[num];
	m_stTuningSetting.dErrBound		= 0.05f;
	//m_stTuningSetting.ucInChn		= num + 1;

	/*switch (num)
	{
	case 0:
		m_stTuningSetting.ucInChn	= 0x01;
		break;
	case 1:
		m_stTuningSetting.ucInChn	= 0x02;
		break;
	case 2:
		m_stTuningSetting.ucInChn	= 0x03;
		break;
	case 3:
		m_stTuningSetting.ucInChn	= 0x04;
		break;
	case 4:
		m_stTuningSetting.ucInChn	= 0x05;
		break;
	case 5:
		m_stTuningSetting.ucInChn	= 0x06;
		break;
	case 6:
		m_stTuningSetting.ucInChn	= 0x07;
		break;
	case 7:
		m_stTuningSetting.ucInChn	= 0x08;
		break;
	case 8:
		m_stTuningSetting.ucInChn	= 0x09;
		break;
	case 9:
		m_stTuningSetting.ucInChn	= 0x0A;
		break;
	case 10:
		m_stTuningSetting.ucInChn	= 0x0B;
		break;
	case 11:
		m_stTuningSetting.ucInChn	= 0x0C;
		break;
	}*/


	switch (num) //Channel num 0x01-0x0C ONLY , channel mapping to real channel connection in Litec
	{
	case  ACF1_CH1:
		m_stTuningSetting.ucInChn	= 0x01;
		break;

	case  ACF1WH_CH3:
		m_stTuningSetting.ucInChn	= 0x03;
		break;

	case  ACF2WH_CH4:
		m_stTuningSetting.ucInChn	= 0x04;
		break;

	default:
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayMessage("		AutoTune Invalid Channel number, SetTunePara(num)	");
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return HEATER_FAILED;

		break;
	}

	EBTC_TNSET stTuning;

	stTuning.ucMethod		= m_stTuningSetting.ucMethod;
	stTuning.fSetPoint		= (FLOAT)m_stTuningSetting.dSetPoint;
	stTuning.dwClkPrescaler = 0;
	stTuning.fErrBound		= (FLOAT)m_stTuningSetting.dErrBound;
	stTuning.ucInChn		= m_stTuningSetting.ucInChn;
	stTuning.fStepTarget	= (FLOAT)100.0;

#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_tune_reset(m_hLitecChn[num]))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		m_bLitecFailed[num] = TRUE;

		CString szDisplay = _T("");
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Litec%d Ch#%d Reset Tune Para Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return HEATER_FAILED;
	}
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

	Sleep(1000);

#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_tune_configure(m_hLitecChn[num], &stTuning)) 
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		m_bLitecFailed[num] = TRUE;

		CString szDisplay = _T("");

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Set Tune Para Error! %s"), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_CTRL_ERR); //20130222
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		
		return HEATER_FAILED;
	}
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

	return HEATER_OP_OK;
}

INT CLitecCtrl::TurnOnLitecAT(INT num, BOOL bMode)
{
	CTempDisplay *pCTempDisplay = (CTempDisplay*)m_pModule->GetStation("TempDisplay");

	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	////Debug
	//if(bMode)
	//{
	//	DisplayMessage("Turn On Litec AT");
	//}
	//else
	//{
	//	DisplayMessage("Turn Off Litec AT");
	//}

	if (bMode)
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
		if (0 != ebtc_tune_start(m_hLitecChn[num]))
		{
#ifdef HEATER_CONTROL_LOCK //20150714
			m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
			m_bLitecFailed[num] = TRUE;

			CString szDisplay = _T("");			
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("Litec%d Ch#%d Start Tune Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
			DisplayMessage(szDisplay);
			//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
			//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
			SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			return HEATER_FAILED;
		}
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		m_bLitecATOn[num] = TRUE;
		m_bIsLearningAT[num] = TRUE;
		m_ulATCounter[num]	= 0;
	}
	else
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
		if (0 != ebtc_tune_abort(m_hLitecChn[num]))
		{
#ifdef HEATER_CONTROL_LOCK //20150714
			m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
			m_bLitecFailed[num] = TRUE;

			CString szDisplay = _T("");
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("Litec%d Ch#%d Stop AutoTune Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
			DisplayMessage(szDisplay);
			//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
			SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			return HEATER_FAILED;
		}
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

		//m_bLitecATOn[num] = FALSE;
		//m_bIsLearningAT[num] = FALSE;
		m_ulATCounter[num]	= 0;
		Sleep(1000);

#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
		if (0 != ebtc_tune_reset(m_hLitecChn[num]))
		{
#ifdef HEATER_CONTROL_LOCK //20150714
			m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
			m_bLitecFailed[num] = TRUE;

			CString szDisplay = _T("");
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("Litec%d Ch#%d Reset Tune Para Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
			DisplayMessage(szDisplay);
			//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
			//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
			SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			return HEATER_FAILED;
		}		
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
	}
	m_bLitecFailed[num] = FALSE;

	return HEATER_OP_OK;
}

BOOL CLitecCtrl::IsLitecLearningAT(INT num)
{
	CTempDisplay *pCTempDisplay = (CTempDisplay*)m_pModule->GetStation("TempDisplay");
	CString szDisplay = _T("");

	EBTC_CTRLPARAM_PID	stTunedPID;
	EBTC_CTRLPARAM_PID	stOldPID;
	EBTC_TNSTAT			stTuningStat = {0, 0.0, 0.0, 0};
	LTC_TNLOG			TnLog = {0};
	BOOL				bDone = FALSE;
	DWORD				dwCurrentTime = 0;

	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return FALSE;
	}

	if (m_bLitecFailed[num])
	{
		return FALSE;
	}	
	
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_tune_get_status(m_hLitecChn[num], &stTuningStat))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		m_bLitecFailed[num] = TRUE;

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Litec%d Ch#%d Get Tune status Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

		TurnOnLitecAT(num, OFF);
		m_bIsLearningAT[num] = FALSE;
		return FALSE;
	}
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

	if (EBTC_TNSTS_IDLE == stTuningStat.ucTnStat)
	{
		DisplayMessage("Auto Tuning Idle");
		TurnOnLitecAT(num, OFF);
		m_bIsLearningAT[num] = FALSE;
		return FALSE;
	}
	else if (EBTC_TNSTS_ABORT == stTuningStat.ucTnStat)
	{
		DisplayMessage("Auto Tuning Abort");
		TurnOnLitecAT(num, OFF);
		m_bIsLearningAT[num] = FALSE;
		return FALSE;
	}
	else if (EBTC_TNSTS_START == stTuningStat.ucTnStat)
	{
		bDone = FALSE;
		m_bIsLearningAT[num] = TRUE;
	}
	else if (EBTC_TNSTS_FINISH == stTuningStat.ucTnStat)
	{
		bDone = TRUE;
		m_bIsLearningAT[num] = FALSE;

		/* backup the old control parameters */
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
		if (0 != ebtc_ctrl_get_pid(m_hLitecChn[num], &stOldPID))
		{
#ifdef HEATER_CONTROL_LOCK //20150714
			m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("Litec%d Ch#%d Get PID Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
			DisplayMessage(szDisplay);
			//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
			//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
			SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			return FALSE;
		}
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
	}

	if (!bDone)
	{
		//Ensure the sampling rate is at max 1 Hz
		if ((dwCurrentTime = (GetTickCount() - m_dwATTimer[num])) < 1000)
		{
			//szDisplay.Format("Time for each update: %d", dwCurrentTime);
			//DisplayMessage(szDisplay);
			return TRUE;
		}

		if (0 != litec_tune_logging(m_hLitecChn[num], &TnLog))
		{
			m_bLitecFailed[num] = TRUE;

			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("Litec%d Ch#%d Tune Logging Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
			DisplayMessage(szDisplay);
			//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
			//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
			SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

			TurnOnLitecAT(num, OFF);
			m_bIsLearningAT[num] = FALSE;
			return FALSE;
		}

		m_dwATTimer[num] = GetTickCount();

		for (LONG i = 0; i < TnLog.ucTempCnt; i++)
		{
			m_fATBuffer[num][m_ulATCounter[num]++] = TnLog.fTs[i];

			////Debug
			//szDisplay.Format("m_fATBuffer[%d][%ld] = %.4f, TempCnt = %ld", num, m_ulATCounter[num] - 1, m_fATBuffer[num][m_ulATCounter[num]-1], TnLog.ucTempCnt);
			//DisplayMessage(szDisplay);
		}
	
		return TRUE;
	}

	if (bDone)
	{	
		if (0 != litec_tune_get_parameter_relay(m_hLitecChn[num], float(m_dLitecSetTemp[num]),
												float(1), &m_fATBuffer[num][0], (WORD)m_ulATCounter[num],
												&stTunedPID)) 
		{	
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("Litec%d Ch#%d Get Parameter Relay Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
			DisplayMessage(szDisplay);
			//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
			//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
			SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			return FALSE;
		}

		//Debug 
		DisplayMessage("=================================");
		DisplayMessage("Obtained PID Control Parameters\n");
		DisplayMessage("#Dynamic\n");
		szDisplay.Format(" Kp = %.5f, Ki = %.5f, Kd = %.5f\n", stTunedPID.fKp_d, stTunedPID.fKi_d, stTunedPID.fKd_d);
		DisplayMessage(szDisplay);
		DisplayMessage("#Static\n");
		szDisplay.Format(" Kp = %.5f, Ki = %.5f, Kd = %.5f\n", stTunedPID.fKp_s, stTunedPID.fKi_s, stTunedPID.fKd_s);
		DisplayMessage(szDisplay);
		DisplayMessage("Others\n");
		szDisplay.Format(" A = %.5f, RT = %.5f, FT = %.5f, DT = %.5f\n", stTunedPID.fA, stTunedPID.fRT, stTunedPID.fFT, stTunedPID.fDT);
		DisplayMessage(szDisplay);
		szDisplay.Format(" ESO = %.5f, SPT = %.5f, CBND = %.5f\n\n", stTunedPID.fESO, stTunedPID.fSPT, stTunedPID.fCBND);
		DisplayMessage(szDisplay);
		DisplayMessage("=================================");

		if (HEATER_OP_OK != SetPIDPara(num, &stTunedPID))
		{
			//Restore PID parameters
#ifdef HEATER_CONTROL_LOCK //20150714
			m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
			if (0 != ebtc_ctrl_set_pid(m_hLitecChn[num], &stOldPID))
			{
#ifdef HEATER_CONTROL_LOCK //20150714
				m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				szDisplay.Format(_T("Litec%d Ch#%d Restore PID Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
				DisplayMessage(szDisplay);
				//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
				//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
				SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
				SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				return FALSE;
			}
#ifdef HEATER_CONTROL_LOCK //20150714
			m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		}

		//Learning HOD Mask should have finished up till this point
		UpdateHODMask(num);
		m_bLearningHOD[num] = FALSE;

	}	
	
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_tune_reset(m_hLitecChn[num]))
	{
		DisplayMessage("Reset tune status failed after AutoTune");
	}
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

	m_ulATCounter[num]	= 0;
	m_bLitecATOn[num] = FALSE;
	m_bIsLearningAT[num] = FALSE;

	//Finish learning AutoTune
	//Keep the Power On after autotune requested by Wai
	PowerOnLitec(num, ON);

	return FALSE;
}

INT CLitecCtrl::LearnLitecHODMask(INT num)
{
	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	//for (INT i = 0; i < MAX_LTC_CH; i++)
	//{
	//	if (m_bLitecFailed[i])
	//	{
	//		return HEATER_FAILED;
	//	}
	//}

	if (m_bLitecFailed[num])
	{
		return HEATER_FAILED;
	}

#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_hod_mask_learning_start(m_hLitecMCB[(INT)(num / MAX_LTC_CH)], MaskHi, MaskLo))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		CString szDisplay = _T("");

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Start Learning HOD Mask Error! %s"), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return HEATER_FAILED;
	}
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

	m_bLearningHOD[num] = TRUE;
	return HEATER_OP_OK;
}


BOOL CLitecCtrl::IsLitecLearningHOD(INT num)
{
	CString szDisplay = _T("");

	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	for (INT i = 0; i < MAX_LTC_CH; i++) //alfred???
	{
		if (m_bLitecFailed[i])
		{
			return HEATER_FAILED;
		}
	}

	BYTE LearningStatus = EBTC_FSM_HOD_IDLE;

#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_hod_mask_learning_get_status(m_hLitecMCB[(INT)(num / MAX_LTC_CH)], MaskHi, MaskLo, &LearningStatus))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Learning HOD Mask Get Status Error! %s"), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return HEATER_FAILED;
	}
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

	//Debug
	if (LearningStatus == EBTC_FSM_HOD_IDLE)
	{
		DisplayMessage("HOD Learning Status: Idle");
		return TRUE;
	}
	else if (LearningStatus == EBTC_FSM_HOD_MONITORING)
	{
		DisplayMessage("HOD Learning Status: Monitoring");
		return TRUE;
	}
	else if (LearningStatus == EBTC_FSM_HOD_ALARM)
	{
		DisplayMessage("HOD Learning Status: Alarm");
		return TRUE;
	}
	else if (LearningStatus == EBTC_FSM_HOD_LEARNING)
	{
		DisplayMessage("HOD Learning Status: Learning");
		return TRUE;
	}
	else if (LearningStatus == EBTC_FSM_HOD_LEARNED)
	{
		DisplayMessage("HOD Learning Status: Learned");
		UpdateHODMask(num);
		return FALSE;
	}

	return TRUE;
}


INT CLitecCtrl::UpdateHODMask(INT num)
{
	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	//for (INT i = 0; i < MAX_LTC_CH; i++)
	//{
	//	if (m_bLitecFailed[i])
	//	{
	//		return HEATER_FAILED;
	//	}
	//}

	if (m_bLitecFailed[num])
	{
		return HEATER_FAILED;
	}

#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_hod_mask_update(m_hLitecChn[num], MaskHi, MaskLo)) //  m_hLitecMCB
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		//m_bLitecFailed[num] = TRUE;

		CString szDisplay = _T("");

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Update HOD Mask Error! %s"), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return HEATER_FAILED;
	}
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

	return HEATER_OP_OK;
}

INT CLitecCtrl::LearnLitecRMSMask(INT num, BOOL bMode, INT nOpMode)
{
	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	if (bMode)
	{
		//if (0 != ebtc_rms_mask_learning_start(m_hLitecMCB[HEATER], MaskHi, MaskLo, (BYTE)nOpMode)) //0 = standy mode, 1 = working mode
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
		if (0 != ebtc_rms_mask_learning_start(m_hLitecMCB[(INT)(num / MAX_LTC_CH)], MaskHi, MaskLo, (BYTE)nOpMode)) //0 = standy mode, 1 = working mode
		{
#ifdef HEATER_CONTROL_LOCK //20150714
			m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
			CString szDisplay = _T("");

			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("Start Learning RMS Mask Error! %s"), ec_report_last_error());
			DisplayMessage(szDisplay);
			SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
			//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			return HEATER_FAILED;
		}
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

		m_bLearningRMS[num] = TRUE;
	}
	else
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
		//if (0 != ebtc_rms_mask_learning_stop(m_hLitecMCB[HEATER], MaskHi, MaskLo))
		if (0 != ebtc_rms_mask_learning_stop(m_hLitecMCB[(INT)(num / MAX_LTC_CH)], MaskHi, MaskLo))
		{
#ifdef HEATER_CONTROL_LOCK //20150714
			m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
			CString szDisplay = _T("");

			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("Stop Learning RMS Mask Error! %s"), ec_report_last_error());
			DisplayMessage(szDisplay);
			SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
			//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			return HEATER_FAILED;
		}
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
	}

	return HEATER_OP_OK;
}


INT CLitecCtrl::IsLitecLearningRMS(INT num, BOOL *pStatus)
{
	CString szDisplay = _T("");

	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	EBTC_PROT_RMS RMSs;

	//if (0 != ebtc_rms_mask_learning_read_data(m_hLitecMCB[HEATER], MaskHi, MaskLo, &RMSs))
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_rms_mask_learning_read_data(m_hLitecMCB[(INT)(num / MAX_LTC_CH)], MaskHi, MaskLo, &RMSs))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		*pStatus = FALSE;

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Learning RMS Mask Get Status Error! %s"), ec_report_last_error());
		DisplayMessage(szDisplay);
		SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

		//PEBTC_PROT_RMS pRMS = 0;
		//if(0 != ebtc_rms_get(m_hLitecChn[4], 0, pRMS))
		//{
		//	szDisplay.Format(_T("Get ebtc rms error! %s"), ec_report_last_error());
		//	DisplayMessage(szDisplay);
		//}
		//else
		//{
		//	szDisplay.Format(_T("fAvgPower = %.4f, dwCycleTime = %ld, ucFSM = %d"), pRMS->fAvgPower, pRMS->dwCycleTime, pRMS->ucFSM);
		//	DisplayMessage(szDisplay);
		//}

		return HEATER_FAILED;
	}
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

	if (RMSs.ucFSM == (BYTE)EBTC_FSM_RMS_IDLE)
	{
		DisplayMessage("RMS IDLE");
		*pStatus = FALSE;
	}
	if (RMSs.ucFSM == (BYTE)EBTC_FSM_RMS_WAITING)
	{
		DisplayMessage("RMS Waiting");
		*pStatus = TRUE;
	}
	if (RMSs.ucFSM == (BYTE)EBTC_FSM_RMS_LEARNING)
	{
		DisplayMessage("RMS Learning");
		*pStatus = TRUE;
	}
	if (RMSs.ucFSM == (BYTE)EBTC_FSM_RMS_FAILED)
	{
		DisplayMessage("RMS Failed");
		*pStatus = FALSE;
		LearnLitecRMSMask(num, OFF);		
	}
	if (RMSs.ucFSM == (BYTE)EBTC_FSM_RMS_WORKING)
	{
		DisplayMessage("RMS Working");
		*pStatus = TRUE;
	}
	if (RMSs.ucFSM == (BYTE)EBTC_FSM_RMS_LEARNED)
	{
		DisplayMessage("RMS learned");
		*pStatus = FALSE;
		LearnLitecRMSMask(num, OFF);	

		LTC_PROTRMS_INPUT RMSIn;

		RMSIn.fRMS	= RMSs.fAvgPower;
		RMSIn.dwCycle = RMSs.dwCycleTime / 1000;
		RMSIn.wDlyCnt = 120;
		RMSIn.wHitCnt = 3;

		if (0 != litec_set_RMS_ex(m_hLitecChn[4], &RMSIn)) //alfred???
		{
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("Litec set RMS ex Error! %s"), ec_report_last_error());
			DisplayMessage(szDisplay);
			SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
			//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			return HEATER_FAILED;
		}
	}

	return HEATER_OP_OK;
}

INT CLitecCtrl::PrintPIDPara(FILE *fp, INT num)
{
	CString szDisplay = "";
	EBTC_CTRLPARAM_PID	stTunedPID;

	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		m_bLitecATOn[num] = FALSE;
		return HEATER_NOT_INIT;
	}

#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_ctrl_get_pid(m_hLitecChn[num], &stTunedPID))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Litec%d Ch#%d PRINT PID Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
		DisplayMessage(szDisplay);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		fprintf(fp, "\n# PRINT Heater CH PID Failed!\n");
		return HEATER_FAILED;
	}
	else //Print CH PID
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		DisplayMessage("=================================");
		DisplayMessage("Obtained PID Control Parameters\n");
		
		szDisplay.Format(_T("Litec%d Ch#%d %S PRINT PID :- \n"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), GetLitecChName(num));
		DisplayMessage(szDisplay);

		DisplayMessage(CString(_T("#Dynamic\n")));
		szDisplay.Format(" Kp = %.5f, Ki = %.5f, Kd = %.5f\n", stTunedPID.fKp_d, stTunedPID.fKi_d, stTunedPID.fKd_d);
		DisplayMessage(szDisplay);
		DisplayMessage(CString(_T("#Static\n")));
		szDisplay.Format(" Kp = %.5f, Ki = %.5f, Kd = %.5f\n", stTunedPID.fKp_s, stTunedPID.fKi_s, stTunedPID.fKd_s);
		DisplayMessage(szDisplay);
		DisplayMessage(CString(_T("Others\n")));
		szDisplay.Format(" A = %.5f, RT = %.5f, FT = %.5f, DT = %.5f\n", stTunedPID.fA, stTunedPID.fRT, stTunedPID.fFT, stTunedPID.fDT);
		DisplayMessage(szDisplay);
		szDisplay.Format(" ESO = %.5f, SPT = %.5f, CBND = %.5f\n\n", stTunedPID.fESO, stTunedPID.fSPT, stTunedPID.fCBND);
		DisplayMessage(szDisplay);
		DisplayMessage(CString(_T("=================================")));

		fprintf(fp, "\n %s #Dynamic\n", GetLitecChName(num).GetBuffer(0));
		fprintf(fp, " Kp = %.5f, Ki = %.5f, Kd = %.5f\n", stTunedPID.fKp_d, stTunedPID.fKi_d, stTunedPID.fKd_d);
		fprintf(fp, "#Static\n");
		fprintf(fp, " Kp = %.5f, Ki = %.5f, Kd = %.5f\n", stTunedPID.fKp_s, stTunedPID.fKi_s, stTunedPID.fKd_s);
		fprintf(fp, "Others\n");
		fprintf(fp, " A = %.5f, RT = %.5f, FT = %.5f, DT = %.5f\n", stTunedPID.fA, stTunedPID.fRT, stTunedPID.fFT, stTunedPID.fDT);
		fprintf(fp, " ESO = %.5f, SPT = %.5f, CBND = %.5f\n\n", stTunedPID.fESO, stTunedPID.fSPT, stTunedPID.fCBND);
	}

	return HEATER_OP_OK;
}

INT CLitecCtrl::GetPIDPara(INT num, PEBTC_CTRLPARAM_PID pstTunedPID)
{
	CString szDisplay = "";

	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		m_bLitecATOn[num] = FALSE;
		return HEATER_NOT_INIT;
	}

#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_ctrl_get_pid(m_hLitecChn[num], pstTunedPID))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Litec%d Ch#%d Get PID Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return HEATER_FAILED;
	}
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

	return HEATER_OP_OK;
}

INT CLitecCtrl::SetPIDPara(INT num, PEBTC_CTRLPARAM_PID pstTunedPID)
{
	CString szDisplay = "";
	EBTC_CTRLPARAM_PID	stGetPID = {0};

	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		m_bLitecATOn[num] = FALSE;
		return HEATER_NOT_INIT;
	}	

#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_ctrl_set_pid(m_hLitecChn[num], pstTunedPID))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Litec%d Ch#%d Set PID Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return HEATER_FAILED;
	}
	else
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		Sleep(1000);

		if (HEATER_OP_OK == GetPIDPara(num, &stGetPID))
		{
			////Debug
			//DisplayMessage("=================================");
			//DisplayMessage("Obtained PID Control Parameters\n");
			//DisplayMessage("#Dynamic\n");
			//szDisplay.Format(" Kp = %.5f, Ki = %.5f, Kd = %.5f\n", stGetPID.fKp_d, stGetPID.fKi_d, stGetPID.fKd_d);
			//DisplayMessage(szDisplay);
			//DisplayMessage("#Static\n");
			//szDisplay.Format(" Kp = %.5f, Ki = %.5f, Kd = %.5f\n", stGetPID.fKp_s, stGetPID.fKi_s, stGetPID.fKd_s);
			//DisplayMessage(szDisplay);
			//DisplayMessage("Others\n");
			//szDisplay.Format(" A = %.5f, RT = %.5f, FT = %.5f, DT = %.5f\n", stGetPID.fA, stGetPID.fRT, stGetPID.fFT, stGetPID.fDT);
			//DisplayMessage(szDisplay);
			//szDisplay.Format(" ESO = %.5f, SPT = %.5f, CBND = %.5f\n\n", stGetPID.fESO, stGetPID.fSPT, stGetPID.fCBND);
			//DisplayMessage(szDisplay);

			//DisplayMessage("Set PID Control Parameters\n");
			//DisplayMessage("#Dynamic\n");
			//szDisplay.Format(" Kp = %.5f, Ki = %.5f, Kd = %.5f\n", pstTunedPID->fKp_d, pstTunedPID->fKi_d, pstTunedPID->fKd_d);
			//DisplayMessage(szDisplay);
			//DisplayMessage("#Static\n");
			//szDisplay.Format(" Kp = %.5f, Ki = %.5f, Kd = %.5f\n", pstTunedPID->fKp_s, pstTunedPID->fKi_s, pstTunedPID->fKd_s);
			//DisplayMessage(szDisplay);
			//DisplayMessage("Others\n");
			//szDisplay.Format(" A = %.5f, RT = %.5f, FT = %.5f, DT = %.5f\n", pstTunedPID->fA, pstTunedPID->fRT, pstTunedPID->fFT, pstTunedPID->fDT);
			//DisplayMessage(szDisplay);
			//szDisplay.Format(" ESO = %.5f, SPT = %.5f, CBND = %.5f\n\n", pstTunedPID->fESO, pstTunedPID->fSPT, pstTunedPID->fCBND);
			//DisplayMessage(szDisplay);
			//DisplayMessage("=================================");

			if (
				(pstTunedPID->fKp_d != stGetPID.fKp_d) ||
				(pstTunedPID->fKi_d != stGetPID.fKi_d) ||
				(pstTunedPID->fKd_d != stGetPID.fKd_d) ||

				(pstTunedPID->fKp_s != stGetPID.fKp_s) ||
				(pstTunedPID->fKi_s != stGetPID.fKi_s) ||
				(pstTunedPID->fKd_s != stGetPID.fKd_s) ||

				(pstTunedPID->fA != stGetPID.fA)	||
				(pstTunedPID->fRT != stGetPID.fRT) ||
				(pstTunedPID->fFT != stGetPID.fFT) ||
				(pstTunedPID->fDT != stGetPID.fDT) ||

				(pstTunedPID->fESO != stGetPID.fESO) ||
				(pstTunedPID->fSPT != stGetPID.fSPT) ||
				(pstTunedPID->fCBND != stGetPID.fCBND)
			   )
			{
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				szDisplay.Format(_T("Litec%d Ch#%d Set PID Failed! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());

				//szDisplay.Format(_T("SetPID Failed! %s"), ec_report_last_error());
				DisplayMessage(szDisplay);
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				return HEATER_FAILED;
			}
			else
			{
				Sleep(1000);
				if (0 != litec_save_configuration_pid(m_hLitecChn[num]))
				{
					DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
					szDisplay.Format(_T("Litec%d Ch#%d SavePID Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
					DisplayMessage(szDisplay);
					//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
					//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
					SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
					SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
					DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
					return HEATER_FAILED;
				}
			}
		}
		else
		{
			return HEATER_FAILED;
		}		
	}

	return HEATER_OP_OK;
}



INT CLitecCtrl::SetHODThreshold(INT num, LONG lThreshold)
{
	CString szDisplay = _T("");

	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	if (m_bLitecFailed[num])
	{
		return HEATER_FAILED;
	}

	//Once the output current is less than the threshold, controller will claim
	//the current sensor feedback error
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (HEATER_OP_OK != ebtc_set_ADCp_threshold(m_hLitecMCB[(INT)(num / MAX_LTC_CH)], 1, ((INT)(num % MAX_LTC_CH) + 1), (WORD)lThreshold))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		m_bLitecFailed[num] = TRUE;

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Set ADCp Threshold Error! %s"), ec_report_last_error());
		DisplayMessage(szDisplay);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return HEATER_FAILED;
	}
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

	if (0 != litec_save_configuration_all(m_hLitecChn[num]))
	{
		m_bLitecFailed[num] = TRUE;

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Litec%d Ch#%d Save HOD Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return HEATER_FAILED;
	}

	return HEATER_OP_OK;
}

INT CLitecCtrl::GetHODThreshold(INT num, LONG *lThreshold)
{
	if (!m_bLitecInit[(INT)(num / MAX_LTC_CH)])
	{
		return HEATER_NOT_INIT;
	}

	if (m_bLitecFailed[num])
	{
		return HEATER_FAILED;
	}

	WORD wThreshold = 0;

#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (HEATER_OP_OK != ebtc_get_ADCp_threshold(m_hLitecMCB[(INT)(num / MAX_LTC_CH)], 1, ((INT)(num % MAX_LTC_CH) + 1), &wThreshold))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		m_bLitecFailed[num] = TRUE;

		CString szDisplay = _T("");

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Get ADCp Threshold Error! %s"), ec_report_last_error());
		DisplayMessage(szDisplay);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return HEATER_FAILED;
	}
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
	
	*lThreshold = (LONG)wThreshold;
	return HEATER_OP_OK;
}

VOID CLitecCtrl::GetTempValue(CTempDisplay *pStation)
{
//	CTempDisplay *pCTempDisplay = (CTempDisplay*)m_pModule->GetStation("TempDisplay");

	INT i;

	for (i = 0; i < NUM_OF_LITEC * NUM_OF_HEATER_CHANNEL; i++)
	{
		if (pStation->IsLitecHeaterExist(i))
		{
			pStation->m_bHeaterOn[i]		= m_bLitecOn[i];
			pStation->m_dHeaterTemp[i]		= m_dLitecTemp[i];
			pStation->m_bHeaterATOn[i]		= m_bLitecATOn[i];
			pStation->m_bHeaterFailed[i]	= m_bLitecFailed[i];		
		}
	}

}

#if 1 //20130911 for truly dark mark issue. We have to lower the dynamic output for testing
BOOL CLitecCtrl::SetDynamicOutputLimit(float fLimit)
{

	CString szDisplay = _T("");
	INT i;
	for (i = 0; i < NUM_OF_LITEC * MAX_LTC_CH; i++) 
	{
		float fGetLimit = 0.0;
		//if (
		//	//i == MB1_CTRL_CH1 ||
		//	//i == MB2_CTRL_CH2 ||
		//	//i == MB1_WH_CH1 ||
		//	//i == MB2_WH_CH2 ||
		//	//i == MB3_CTRL_CH1 ||
		//	//i == MB4_CTRL_CH2 ||
		//	//i == MB3_WH_CH3 ||
		//	//i == MB4_WH_CH4 
		//   )
		//{
		//	//LITEC_API int     litec_get_dynamic_output_limit(HANDLE hChn, float *fLimit);
		//	//LITEC_API int     litec_set_dynamic_output_limit(HANDLE hChn, float fLimit);
		//	if (0 != litec_set_dynamic_output_limit(m_hLitecChn[i], fLimit))
		//	{
		//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		//		szDisplay.Format(_T("Litec#d Ch%d litec_set_dynamic_output_limit Error! %s"), (INT)(i / MAX_LTC_CH) + 1 , (INT)(i % MAX_LTC_CH) + 1, ec_report_last_error());
		//		DisplayMessage(szDisplay);
		//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

		//		return FALSE;
		//	}

		//	if (0 != litec_get_dynamic_output_limit(m_hLitecChn[i], &fGetLimit))
		//	{
		//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		//		szDisplay.Format(_T("Litec#d Ch%d litec_get_dynamic_output_limit Error! %s"), (INT)(i / MAX_LTC_CH) + 1 , (INT)(i % MAX_LTC_CH) + 1, ec_report_last_error());
		//		DisplayMessage(szDisplay);
		//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

		//		return FALSE;
		//	}
		//	else
		//	{
		//		szDisplay.Format(_T("litec_get_dynamic_output_limit Litec%d ch:%d, fLimit:%f"), (INT)(i / MAX_LTC_CH) + 1 , (INT)(i % MAX_LTC_CH) + 1, fGetLimit);
		//		DisplayMessage(szDisplay);
		//	}

		//}
	}
	return TRUE;
}
#endif

CString CLitecCtrl::FormatBCDVersion(DWORD dwVerNum)
{
	CString szBCDVersion = "";

	szBCDVersion.Format("%u%u.%u%u.%u%u%u%u",
						HIBIT(HIBYTE(HIWORD(dwVerNum))), 
						LOBIT(HIBYTE(HIWORD(dwVerNum))), 
						HIBIT(LOBYTE(HIWORD(dwVerNum))), 
						LOBIT(LOBYTE(HIWORD(dwVerNum))), 
						HIBIT(HIBYTE(LOWORD(dwVerNum))), 
						LOBIT(HIBYTE(LOWORD(dwVerNum))), 
						HIBIT(LOBYTE(LOWORD(dwVerNum))), 
						LOBIT(LOBYTE(LOWORD(dwVerNum))));
	return szBCDVersion;
}

VOID CLitecCtrl::PrintLitecInfomation()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	INT i = 0;
	INT k = 0;

	CString szMsg = "";

	EBTC_INFO stEbtcInfo;

	DOUBLE				dAlarmBand[MAX_LTC_CH];
	DOUBLE				dSetTempLmt[MAX_LTC_CH];
	LONG				lHODThreshold[MAX_LTC_CH];
	EBTC_CTRLPARAM_PID	st_PIDCtrl[MAX_LTC_CH];


	__time64_t ltime;
	_time64(&ltime);
	CString szTime = _ctime64(&ltime);

	FILE *fp = fopen("D:\\sw\\AC9000\\Data\\Litec Information.log", "a+");
	if (fp != NULL)
	{
		fprintf(fp, "***********************************************************************************************\n");
		fprintf(fp, "*                                                                                             *\n");
		fprintf(fp, "*                             Litec Information                                               *\n");
		fprintf(fp, "*                                                                                             *\n");
		fprintf(fp, "***********************************************************************************************\n");
		fprintf(fp, "Date: " + szTime + "\n");
	}

	// Get Data
	if (!pAppMod->m_bAllLitecInited)
	{
		if (fp != NULL)
		{
			fprintf(fp, "Litec Not Initilized\n");
		}
	}
	else
	{
		for (k = 0; k < NUM_OF_LITEC; k++)
		{
			if (m_bLitecInit[k])
			{
				if (fp != NULL)
				{			
				// get version
					DWORD dwLitecVersion = 0;

					dwLitecVersion = litec_lib_version();
					m_stLitecLibVersion = FormatBCDVersion(dwLitecVersion);

					fprintf(fp, "Litec Library Version: " + m_stLitecLibVersion + "\n");

				//Get Litec model part no.
					char szModel[50] = {0};
					if (0 != litec_get_model_partno(m_hLitecMCB[k], &szModel[0]))
					{
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						szMsg.Format(_T("Litec Get Litec Infomation Error! %s"), ec_report_last_error());
						DisplayMessage(szMsg);
						SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
						DisplayMessage(__FUNCTION__);
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						if (fp != NULL)
						{
							fclose(fp);
						}
						return;
					}
				
					szMsg.Format("Litec Model Part no.: %s \n", szModel);
					fprintf(fp, szMsg);

					for (INT nLitecNum = 0; nLitecNum < NUM_OF_LITEC; nLitecNum++)
				//INT nLitecNum = 0;
					{
#ifdef HEATER_CONTROL_LOCK //20150714
						m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
						if (0 != ebtc_get_information(m_hLitecMCB[nLitecNum], &stEbtcInfo))
						{
#ifdef HEATER_CONTROL_LOCK //20150714
							m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
							DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
							szMsg.Format(_T("Litec Get Litec Infomation Error! %s"), ec_report_last_error());
							DisplayMessage(szMsg);
							SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
							DisplayMessage(__FUNCTION__);
							DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
							if (fp != NULL)
							{
								fclose(fp);
							}
							return;
						}
#ifdef HEATER_CONTROL_LOCK //20150714
						m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714

						m_stLitecVersion[nLitecNum] = FormatBCDVersion(stEbtcInfo.vfirmware);
			
						szMsg.Format("Litec %d Firmware Version: %s \n", nLitecNum, m_stLitecVersion[nLitecNum]);
						fprintf(fp, szMsg);					
						szMsg.Format("		status of connection: %ld \n", stEbtcInfo.bConnected);
						fprintf(fp, szMsg);
						szMsg.Format("		The number of control channels: %ld \n", stEbtcInfo.nCHNs);
						fprintf(fp, szMsg);
						szMsg.Format("		The number of LCM board: %ld \n\n", stEbtcInfo.nLCMs);
						fprintf(fp, szMsg);	
					}
				}

				for (i = 0; i < MAX_LTC_CH; i++)
				{
					if (HEATER_OP_OK == GetAlarmBand(i, &dAlarmBand[i]))
					{       
						if (fp != NULL)
						{
							fprintf(fp, "Ch#%d AlarmBand = %.2f\n", i + 1, dAlarmBand[i]);
						}
					}

					if (HEATER_OP_OK == GetTempLmt(i, &dSetTempLmt[i]))
					{
						if (fp != NULL)
						{
							fprintf(fp, "Ch#%d Temperature Limit = %.2f\n", i + 1, dSetTempLmt[i]);
						}
					}

					if (HEATER_OP_OK == GetHODThreshold(i, &lHODThreshold[i]))
					{
						if (fp != NULL)
						{
							fprintf(fp, "Ch#%d Heater HOD Threshold = %ld\n", i + 1, lHODThreshold[i]);
						}
					}
				
					if (HEATER_OP_OK == GetPIDPara(i, &st_PIDCtrl[i]))
					{
						if (fp != NULL)
						{
							fprintf(fp, "Ch#%d PID Control Parameters\n", i + 1);
							fprintf(fp, "#Dynamic\n");
							fprintf(fp, " Kp = %.5f, Ki = %.5f, Kd = %.5f\n", st_PIDCtrl[i].fKp_d, st_PIDCtrl[i].fKi_d, st_PIDCtrl[i].fKd_d);

							fprintf(fp, "#Static\n");
							fprintf(fp, " Kp = %.5f, Ki = %.5f, Kd = %.5f\n", st_PIDCtrl[i].fKp_s, st_PIDCtrl[i].fKi_s, st_PIDCtrl[i].fKd_s);

							fprintf(fp, "Others\n");
							fprintf(fp, " A = %.5f, RT = %.5f, FT = %.5f, DT = %.5f\n", st_PIDCtrl[i].fA, st_PIDCtrl[i].fRT, st_PIDCtrl[i].fFT, st_PIDCtrl[i].fDT);
							fprintf(fp, " ESO = %.5f, SPT = %.5f, CBND = %.5f\n\n", st_PIDCtrl[i].fESO, st_PIDCtrl[i].fSPT, st_PIDCtrl[i].fCBND);
						}
					}				
				}
			}		
		} //for
	}
	if (fp != NULL)
	{
		fclose(fp);
	}
	return;
}

INT CLitecCtrl::GetLitecChNum(int nCh)
{
	return (nCh % MAX_LTC_CH);
}

INT CLitecCtrl::GetLitecUnitNum(int nCh)
{
	return (nCh / MAX_LTC_CH);
}

INT CLitecCtrl::GetLitecErrNum(int nCh)	// 20140708 Yip: Use Error Number Instead Of Channel Number For Litec Errors
{
	switch (nCh)
	{
	case ACF1_CH1:
		return 0;
	case ACF1WH_CH3:
		return 2;
	case ACF2WH_CH4:
		return 3;
	}
	return 0;
}

CString CLitecCtrl::GetLitecChName(int nCh)	// 20140804 Yip: Get Litec Channel Name From Channel Number
{
	switch (nCh)
	{
	case ACF1_CH1:
		return "ACF1";
	case ACF1WH_CH3:
		return "ACF1WH";
	case ACF2WH_CH4:
		return "ACF2WH";
	}
	return "Dummy";
}

BOOL CLitecCtrl::SetCtrlPara(INT num, EBTC_CTRLPARAM_PID stTunedPID/*, LTC_FLT_BLK stTunedFfcFilter*/)	// 20140508 Yip
{
	CString szDisplay = _T("");

	EBTC_CTRLPARAM_PID	stOldPID;
//	LTC_FLT_BLK			stOldFfcFilter;

	INT nLitecNum = (INT)(num / MAX_LTC_CH);

	if (!m_bLitecInit[nLitecNum])
	{
		return FALSE;
	}

	if (m_bLitecFailed[num])
	{
		return FALSE;
	}

	if (m_stLitecVersion[nLitecNum].Compare("02.12.1174") < 0)	// 20140509 Yip: set and store FFC setting for pulse heater firmware v2.12.1174
	{
		return FALSE;
	}
	
	/* backup the old control parameters */
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
	if (0 != ebtc_ctrl_get_pid(m_hLitecChn[num], &stOldPID))
	{
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Litec%d Ch#%d Get PID Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return FALSE;
	}
#ifdef HEATER_CONTROL_LOCK //20150714
	m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
	/*
	if (0 != litec_get_ffc_filter(m_hLitecChn[num], &stOldFfcFilter))
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szDisplay.Format(_T("Litec%d Ch#%d Get FFC Filter Error! %s"), ((INT)(num/MAX_LTC_CH) + 1), ((INT)(num%MAX_LTC_CH) + 1), ec_report_last_error());
		DisplayMessage(szDisplay);
		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return FALSE;
	}
	*/

	//Debug 
	DisplayMessage("=================================");
	DisplayMessage("Obtained PID Control Parameters\n");
	DisplayMessage("#Dynamic\n");
	szDisplay.Format(" Kp = %.5f, Ki = %.5f, Kd = %.5f\n", stTunedPID.fKp_d, stTunedPID.fKi_d, stTunedPID.fKd_d);
	DisplayMessage(szDisplay);
	DisplayMessage("#Static\n");
	szDisplay.Format(" Kp = %.5f, Ki = %.5f, Kd = %.5f\n", stTunedPID.fKp_s, stTunedPID.fKi_s, stTunedPID.fKd_s);
	DisplayMessage(szDisplay);
	DisplayMessage("Others\n");
	szDisplay.Format(" A = %.5f, RT = %.5f, FT = %.5f, DT = %.5f\n", stTunedPID.fA, stTunedPID.fRT, stTunedPID.fFT, stTunedPID.fDT);
	DisplayMessage(szDisplay);
	szDisplay.Format(" ESO = %.5f, SPT = %.5f, CBND = %.5f\n\n", stTunedPID.fESO, stTunedPID.fSPT, stTunedPID.fCBND);
	DisplayMessage(szDisplay);
	/*
	DisplayMessage("\n");
	DisplayMessage("Obtained FFC Filter Parameters\n");
	for (int i = 0; i <= stTunedFfcFilter.wOrder; ++i)
	{
		szDisplay.Format("A[%ld] = %.5f, B[%ld] = %.5f\n", stTunedFfcFilter.wOrder, stTunedFfcFilter.f_a[i], stTunedFfcFilter.wOrder, stTunedFfcFilter.f_b[i]);
	}
	*/
	DisplayMessage("=================================");

	if (HEATER_OP_OK != SetPIDPara(num, &stTunedPID))
	{
		//Restore PID parameters
#ifdef HEATER_CONTROL_LOCK //20150714
		m_csHeaterCtrlLock.Lock(); //20150714
#endif  //20150714
		if (0 != ebtc_ctrl_set_pid(m_hLitecChn[num], &stOldPID))
		{
#ifdef HEATER_CONTROL_LOCK //20150714
			m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szDisplay.Format(_T("Litec%d Ch#%d Restore PID Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
			DisplayMessage(szDisplay);
			//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
			//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
			SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
			SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		}
		else
		{
#ifdef HEATER_CONTROL_LOCK //20150714
			m_csHeaterCtrlLock.Unlock(); //20150714
#endif  //20150714
		}
		return FALSE;
	}
	//if (0 != litec_set_ffc_filter(m_hLitecChn[num], &stTunedFfcFilter))
	//{
	//	DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	//	szDisplay.Format(_T("Litec%d Ch#%d Set FFC Filter Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
	//	DisplayMessage(szDisplay);
	//	//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
	//	//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
	//	SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
	//	SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
	//	DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

	//	/*
	//	//Restore FFC Filter parameters
	//	if (0 != litec_set_ffc_filter(m_hLitecChn[num], &stOldFfcFilter))
	//	{
	//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	//		szDisplay.Format(_T("Litec%d Ch#%d Restore FFC Filter Error! %s"), ((INT)(num/MAX_LTC_CH) + 1), ((INT)(num%MAX_LTC_CH) + 1), ec_report_last_error());
	//		DisplayMessage(szDisplay);
	//		//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
	//		//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
	//		SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
	//		SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
	//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	//	}
	//	*/
	//	return FALSE;
	//}

	//if (0 != litec_save_ctrl_ffc(m_hLitecChn[num]))
	//{
	//	DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	//	szDisplay.Format(_T("Litec%d Ch#%d Save FFC Filter Error! %s"), ((INT)(num / MAX_LTC_CH) + 1), ((INT)(num % MAX_LTC_CH) + 1), ec_report_last_error());
	//	DisplayMessage(szDisplay);
	//	//SetError(IDS_TEMP_CTRL_COMMUNICATION_ERROR);
	//	//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + GetLitecErrNum(num));
	//	SetError(IDS_TEMP_CTRL_UNIT1_ERROR + GetLitecUnitNum(num)); //20140401
	//	SetError(IDS_TEMP_CTRL_HEATER1_COMMUNICATION_ERROR + GetLitecErrNum(num)); //20130222
	//	DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	//	return FALSE;
	//}

	return TRUE;
}
