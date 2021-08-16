/////////////////////////////////////////////////////////////////
//	AdamCtrl.cpp : interface of the CAdamCtrl class
//
//	Description:
//		AC9000 Host Software
//
//	Date:		Tue Aug 1 2006
//	Revision:	0.25
//
//	By:			AC9000
//				AAA CSP Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2006.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
//trash
#include "stdafx.h"
#include "math.h"
#include "MarkConstant.h"
#include "driver.h"

#include "AC9000.h"
#include "AC9000_Constant.h"


#include "TempDisplay.h"

#define ADAMCTRL_EXTERN
#include "AdamCtrl.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ADAM global variable
DEVLIST DeviceList[100];
DEVLIST SubDeviceList[100];
LRESULT ErrCde;
static PT_DeviceGetFeatures ptDevFeatures;
static PT_TCMuxRead ptTCMuxRead;
static DEVFEATURES DevFeatures;
static FLOAT fTemp;


IMPLEMENT_DYNCREATE(CAdamCtrl, CAC9000Stn)

CAdamCtrl::CAdamCtrl()
{
	INT i;

	m_lAdam1Handle					= 0;
	m_lAdam2Handle					= 0;
	m_bAdam1Init					= FALSE;
	m_bAdam2Init					= FALSE;
	
	for (i = 0; i < NUM_OF_ADAM_CH; i++)
	{
		m_bAdamFailed[i]			= FALSE;
		m_dAdamTemp[i]				= 0.0;

	}

	m_nCurIndex						= 0;

	//Reset All Mon info
	for (i = 0; i < NUM_OF_ADAM_CH; i++)
	{
		m_stAdamInfo[i].Mon			= NO_MON;
		m_stAdamInfo[i].Ch			= NO_CH;

	}

	//Mon Unit & Mon TC ch assignment
	m_stAdamInfo[ACF1_CH1].Mon			= ADAM1;
	m_stAdamInfo[ACF1_CH1].Ch			= CH0;

	m_stAdamInfo[ACF1WH_CH3].Mon		= ADAM1;
	m_stAdamInfo[ACF1WH_CH3].Ch			= CH2;

	m_stAdamInfo[ACF2WH_CH4].Mon		= ADAM1;
	m_stAdamInfo[ACF2WH_CH4].Ch			= CH3;


	// -------------------------------------------------------
	// HMI varibales
	// -------------------------------------------------------

}

CAdamCtrl::~CAdamCtrl()
{
}

BOOL CAdamCtrl::InitInstance()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	CAC9000Stn::InitInstance();

	if (!InitADAM())
	{
		pAppMod->m_bHWInitError = TRUE;
	}
	else
	{
		pAppMod->m_bAllAdamInited = TRUE;
	}

	return TRUE;
}

INT CAdamCtrl::ExitInstance()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	if (m_bAdam1Init)
	{
		DRV_DeviceClose((LONG far*)&m_lAdam1Handle);
	}
	if (m_bAdam2Init)
	{
		DRV_DeviceClose((LONG far*)&m_lAdam2Handle);
	}

	pAppMod->m_bAdamExit = TRUE;

	return CAC9000Stn::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CAdamCtrl::Operation()
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

	if (m_bUpdateProfile)
	{
		UpdateProfile();
	}

	if (m_bUpdatePosition)
	{
		UpdatePosition();
	}

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
BOOL CAdamCtrl::IsAdam1MonValid(INT nChNum)
{
	BOOL bResult = FALSE;

	if (m_stAdamInfo[nChNum].Mon == ADAM1 && m_stAdamInfo[nChNum].Ch != NO_CH)
	{
		bResult = TRUE;
	}

	return bResult;
}

BOOL CAdamCtrl::IsAdam2MonValid(INT nChNum)
{
	BOOL bResult = FALSE;

	//if (m_stAdamInfo[nChNum].Mon == ADAM2 && m_stAdamInfo[nChNum].Ch != NO_CH)
	{
		bResult = TRUE;
	}

	return bResult;
}

BOOL CAdamCtrl::IsLitecMonValid(INT nChNum)
{
	BOOL bResult = FALSE;

	if (m_stAdamInfo[nChNum].Mon == LITECMon)
	{
		bResult = TRUE;
	}

	return bResult;
}

VOID CAdamCtrl::UpdateOutput()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CLitecCtrl *pCLitecCtrl = (CLitecCtrl*)m_pModule->GetStation("LitecCtrl");

	CString szMsg = _T("");

	LONG lTimer = 0;

	if ((m_qState == UN_INITIALIZE_Q) || (m_qState == DE_INITIAL_Q))
	{
		return;
	}

	if (pAppMod->m_bHWInitError)
	{
		return;
	}

	if (!m_fHardware)
	{
		return;
	}

	//Update sensor 
	SFM_CHipecInPort *pInPort = NULL;
	SFM_CHipecSiPort *pSiPort = NULL;
	USHORT usTemp = 0;

	try
	{
		// PreBond ADAM
		if (m_bAdam1Init && IsAdam1MonValid(m_nCurIndex))
		{
			ptTCMuxRead.DasChan = 0;
			ptTCMuxRead.DasGain = 0;
			ptTCMuxRead.ExpChan = 0;
			ptTCMuxRead.TCType = KTC;
			ptTCMuxRead.TempScale = 0;
			ptTCMuxRead.temp = (FLOAT far*)&fTemp;

			// read temperature
			if (m_nCurIndex < NUM_OF_ADAM_CH)
			{
				if (Command() == glPRESTART_COMMAND || Command() == glSTOP_COMMAND || g_bStopCycle)
				{
					return;
				}

				//ptTCMuxRead.DasChan = m_nCurIndex;
				ptTCMuxRead.DasChan = m_stAdamInfo[m_nCurIndex].Ch;

				if ((ErrCde = DRV_TCMuxRead(m_lAdam1Handle, (LPT_TCMuxRead)&ptTCMuxRead)) != SUCCESS)
				{
					if (!m_bAdamFailed[m_nCurIndex])
					{
						szMsg.Format("ADAM1 Ch%ld Error(%ld): Unable to Read ADAM 1", m_nCurIndex, ErrCde);
						DisplayMessage(szMsg);
						szMsg.Format("ADAM1 Ch%ld Error(%ld): Unable to Read ADAM 1", m_stAdamInfo[m_nCurIndex].Ch, ErrCde);
						DisplayMessage(szMsg);

						//SetError(IDS_ADAM_ERR);
						//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + m_nCurIndex);

						//SetError(IDS_ADAM_HEATER1_ERR + m_nCurIndex); //20130222
						SetError(IDS_ADAM_HEATER1_ERR + m_stAdamInfo[m_nCurIndex].Ch); 
						m_bAdamFailed[m_nCurIndex] = TRUE;
					}
				}
				else
				{
					m_dAdamTemp[m_nCurIndex] = fTemp;
				}
			}
		}
		// MainBond ADAM
		//else if (m_bAdam2Init && IsAdam2MonValid(m_nCurIndex))
		//{
		//	ptTCMuxRead.DasChan = 0;
		//	ptTCMuxRead.DasGain = 0;
		//	ptTCMuxRead.ExpChan = 0;
		//	ptTCMuxRead.TCType = KTC;
		//	ptTCMuxRead.TempScale = 0;
		//	ptTCMuxRead.temp = (FLOAT far*)&fTemp;

		//	// read temperature
		//	if (m_nCurIndex < NUM_OF_ADAM_CH)
		//	{
		//		if (Command() == glPRESTART_COMMAND || Command() == glSTOP_COMMAND || g_bStopCycle)
		//		{
		//			return;
		//		}

		//		//ptTCMuxRead.DasChan = m_nCurIndex;
		//		ptTCMuxRead.DasChan = m_stAdamInfo[m_nCurIndex].Ch;

		//		if ((ErrCde = DRV_TCMuxRead(m_lAdam2Handle, (LPT_TCMuxRead)&ptTCMuxRead)) != SUCCESS)
		//		{
		//			if (!m_bAdamFailed[m_nCurIndex])
		//			{
		//				szMsg.Format("ADAM2 Ch%ld Error(%ld): Unable to Read ADAM 2", m_nCurIndex, ErrCde);
		//				DisplayMessage(szMsg);
		//				szMsg.Format("ADAM2 Ch%ld Error(%ld): Unable to Read ADAM 2", m_stAdamInfo[m_nCurIndex].Ch, ErrCde);
		//				DisplayMessage(szMsg);

		//				//SetError(IDS_ADAM_ERR);
		//				//SetError(IDS_TEMP_CTRL_HEATER1_CTRL_ERR + m_nCurIndex + NUM_OF_ADAM_CH);
		//				
		//				//SetError(IDS_ADAM_HEATER1_ERR + m_nCurIndex + NUM_OF_ADAM_CH); //20130222
		//				SetError(IDS_ADAM_HEATER1_ERR + m_stAdamInfo[m_nCurIndex].Ch); //alfred???
		//				m_bAdamFailed[m_nCurIndex] = TRUE;
		//			}
		//		}
		//		else
		//		{
		//			m_dAdamTemp[m_nCurIndex] = fTemp;
		//		}
		//	}
		//}

		// Litec Monitor no need to do it here, Updated in Litec UpdateOutput()

	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	m_nCurIndex++;
	if (m_nCurIndex > NUM_OF_ADAM_CH)
	{
		m_nCurIndex = 0;
	}

	Sleep(100);
}

VOID CAdamCtrl::UpdateProfile()
{
	//Add Your Code Here
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachine);

	m_bUpdateProfile = FALSE;

	try
	{
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	slLock.Unlock();
}

VOID CAdamCtrl::UpdatePosition()
{
	//Add Your Code Here
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CSingleLock slLock(&pAppMod->m_csDevice);
	
	m_bUpdatePosition = FALSE;

	try
	{
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	slLock.Unlock();
}

VOID CAdamCtrl::RegisterVariables()
{
	try
	{
		// variables
		//

		// commands
		//
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
}


/////////////////////////////////////////////////////////////////
//Modified Virtual Operational Functions
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//Wrapped Functions
/////////////////////////////////////////////////////////////////

BOOL CAdamCtrl::InitADAM()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString szMsg = _T("");

	BOOL bResult = FALSE;

	m_bAdam1Init = FALSE;
	m_bAdam2Init = FALSE;

	INT i;

	//Check Adam when hardware exists.
	if (m_fHardware)
	{
		if ((bResult = ADAMChecker()) != TRUE)
		{
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szMsg.Format("ERROR: ADAMChecker Error!");
			DisplayMessage(szMsg);
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

			return FALSE;
		}
	}

	// Init PreBond Adam
	if (!m_fHardware)
	{
		szMsg = _T("   ") + GetName() + " --- ADAM 1 Not Enabled";
		DisplayMessage(szMsg);
	}
	else
	{
		if ((ErrCde = DRV_DeviceOpen(SubDeviceList[0].dwDeviceNum, (LONG far*)&m_lAdam1Handle)) != SUCCESS)
		{
			char cMsg[80];
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szMsg.Format("initADAM ERROR(%ld): ADAM 1 Open Device Error!", ErrCde);
			DisplayMessage(szMsg);
			DRV_GetErrorMessage((ULONG)ErrCde, (LPSTR)&cMsg);
			DisplayMessage(cMsg);
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

			return FALSE;
		}

		ptDevFeatures.buffer = (LPDEVFEATURES)&DevFeatures;
		ptDevFeatures.size = sizeof(DEVFEATURES);

		if ((ErrCde = DRV_DeviceGetFeatures(m_lAdam1Handle, (LPT_DeviceGetFeatures)&ptDevFeatures)) != SUCCESS)
		{
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			szMsg.Format("initADAM ERROR(%ld): ADAM 1 Get Feature Error!", ErrCde);
			DisplayMessage(szMsg);
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

			DRV_DeviceClose((LONG far*)&m_lAdam1Handle);

			return FALSE;
		}

		ptTCMuxRead.DasChan = 0;
		ptTCMuxRead.DasGain = 0;
		ptTCMuxRead.ExpChan = 0;
		ptTCMuxRead.TCType = KTC;
		ptTCMuxRead.TempScale = 0;
		ptTCMuxRead.temp = (FLOAT far*)&fTemp;

		// read temperature
		for (i = 0; i < NUM_OF_ADAM_CH; i++)
		{
			if (IsAdam1MonValid(i))
			{
				//ptTCMuxRead.DasChan = i;
				ptTCMuxRead.DasChan = m_stAdamInfo[i].Ch;

				//ptTCMuxRead.DasChan = m_nCurIndex;

				if ((ErrCde = DRV_TCMuxRead(m_lAdam1Handle, (LPT_TCMuxRead)&ptTCMuxRead)) != SUCCESS)
				{
					DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
					szMsg.Format("ADAM1[%d] Ch%d Error(%ld): Unable to Read ADAM 1", i, m_stAdamInfo[i].Ch, ErrCde);
					DisplayMessage(szMsg);
					DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

					DRV_DeviceClose((LONG far*)&m_lAdam1Handle);

					return FALSE;
				}
			}
		}

		m_bAdam1Init = TRUE;

		szMsg = _T("   ") + GetName() + " --- ADAM 1 Initialized";
		DisplayMessage(szMsg);
	}

	//// Init MainBond Adam
	//if (!m_fHardware)
	//{
	//	szMsg = _T("   ") + GetName() + " --- ADAM 2 Not Enabled";
	//	DisplayMessage(szMsg);
	//}
	//else
	//{
	//	if ((ErrCde = DRV_DeviceOpen(SubDeviceList[1].dwDeviceNum, (LONG far*)&m_lAdam2Handle)) != SUCCESS)
	//	{
	//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	//		szMsg.Format("ERROR(%ld): ADAM 2 Open Device Error!", ErrCde);
	//		DisplayMessage(szMsg);
	//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

	//		return FALSE;
	//	}

	//	ptDevFeatures.buffer = (LPDEVFEATURES)&DevFeatures;
	//	ptDevFeatures.size = sizeof(DEVFEATURES);

	//	if ((ErrCde = DRV_DeviceGetFeatures(m_lAdam2Handle, (LPT_DeviceGetFeatures)&ptDevFeatures)) != SUCCESS)
	//	{
	//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	//		szMsg.Format("ERROR(%ld): ADAM 2 Get Feature Error!", ErrCde);
	//		DisplayMessage(szMsg);
	//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

	//		DRV_DeviceClose((LONG far*)&m_lAdam2Handle);

	//		return FALSE;
	//	}

	//	ptTCMuxRead.DasChan = 0;
	//	ptTCMuxRead.DasGain = 0;
	//	ptTCMuxRead.ExpChan = 0;
	//	ptTCMuxRead.TCType = KTC;
	//	ptTCMuxRead.TempScale = 0;
	//	ptTCMuxRead.temp = (FLOAT far*)&fTemp;

	//	// read temperature
	//	for (i = 0; i < NUM_OF_ADAM_CH; i++)
	//	{

	//		if (IsAdam2MonValid(i))
	//		{
	//			
	//			//ptTCMuxRead.DasChan = i;
	//			ptTCMuxRead.DasChan = m_stAdamInfo[i].Ch;

	//			if ((ErrCde = DRV_TCMuxRead(m_lAdam2Handle, (LPT_TCMuxRead)&ptTCMuxRead)) != SUCCESS)
	//			{
	//				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	//				szMsg.Format("ADAM2[%d] Ch%d Error(%ld): Unable to Read ADAM 2", i, m_stAdamInfo[i].Ch, ErrCde);
	//				DisplayMessage(szMsg);
	//				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

	//				DRV_DeviceClose((LONG far*)&m_lAdam2Handle);

	//				return FALSE;
	//			}
	//		}
	//	}

		m_bAdam2Init = TRUE;

	//	szMsg = _T("   ") + GetName() + " --- ADAM 2 Initialized";
	//	DisplayMessage(szMsg);
	//}

	return TRUE;
}

BOOL CAdamCtrl::ADAMChecker()
{
	SHORT nNumOfDevices;
	SHORT nOutEntries;
	SHORT nNumOfSubdevices;

	USHORT gwDevice;
	char cMsg[80];
	BOOL bStatus = FALSE;

	if ((ErrCde = DRV_DeviceGetNumOfList((SHORT far*)&nNumOfDevices)) != SUCCESS)
	{
		DRV_GetErrorMessage((ULONG)ErrCde, (LPSTR)&cMsg);
		DisplayMessage(cMsg);

		return FALSE;
	}

	if (nNumOfDevices > 100)
	{
		nNumOfDevices = 100;
	}

	// retrieve the information of all installed devices
	if ((ErrCde = DRV_DeviceGetList((DEVLIST far*)&DeviceList[0], (SHORT)nNumOfDevices, (SHORT far*)&nOutEntries)) != (LONG)SUCCESS)
	{
		DRV_GetErrorMessage((ULONG)ErrCde, (LPSTR)&cMsg);
		DisplayMessage(cMsg);
		return FALSE;
	}

	gwDevice = 1;  // preset for safety ...

	for (INT i = 0; i < nNumOfDevices; i++)
	{
		if (
			(DeviceList[i].szDeviceName[0] == 'C') &&
			(DeviceList[i].szDeviceName[1] == 'O') &&
			(DeviceList[i].szDeviceName[2] == 'M') 
		   )
		{
			gwDevice = i;
		}
	}

	nNumOfSubdevices = DeviceList[gwDevice].nNumOfSubdevices;

	if (nNumOfSubdevices > 100)
	{
		nNumOfSubdevices = 100;
	}

	CString str = "";
	str.Format("nNumOfSubdevices = %ld", nNumOfSubdevices);
	DisplayMessage(str);

	if (nNumOfSubdevices != 0)
	{
		if (
			(ErrCde = DRV_DeviceGetSubList(
						  (DWORD)DeviceList[gwDevice].dwDeviceNum, 
						  (DEVLIST far*)&SubDeviceList[0], 
						  (SHORT)nNumOfSubdevices, 
						  (SHORT far*)&nOutEntries)
			) != (LONG)SUCCESS
		   )
		{
			return FALSE;
		}
	}
	str.Format("Name = %s", SubDeviceList[0].szDeviceName);
	DisplayMessage(str);
	str.Format("Name = %s", SubDeviceList[1].szDeviceName);
	DisplayMessage(str);


	return TRUE;
}

VOID CAdamCtrl::ResetChannelStatus()
{
	CTempDisplay *pCTempDisplay = (CTempDisplay*)m_pModule->GetStation("TempDisplay");

	INT i;
	for (i = 0; i < NUM_OF_ADAM_CH; i++)
	{
		pCTempDisplay->m_bMonFailed[i]					= m_bAdamFailed[i]		= FALSE;
		//pCTempDisplay->m_bMonFailed[i + NUM_OF_ADAM_CH] = m_bAdam2Failed[i]		= FALSE;
	}
}

VOID CAdamCtrl::GetTempValue(CTempDisplay *pStation)
{
	CTempDisplay *pCTempDisplay = (CTempDisplay*)m_pModule->GetStation("TempDisplay");
	
	INT i;

	for (i = 0; i < NUM_OF_ADAM_CH; i++) 
	{
		if (pCTempDisplay->m_bHeaterExist[i])  // Valid Heater Channels will have MonTemp[]
		{
			if (m_bAdamFailed[i])
			{
				pStation->m_bMonFailed[i] = m_bAdamFailed[i];
			}

			pStation->m_dMonTemp[i] = m_dAdamTemp[i] + pStation->m_dMonOffset[i];
		}

			//if (m_bAdam1Failed[i])
			//{
			//	pStation->m_bMonFailed[i] = m_bAdam1Failed[i];
			//}

			//pStation->m_dMonTemp[i] = m_dAdam1Temp[i] + pStation->m_dMonOffset[i];

			//if (m_bAdam2Failed[i])
			//{
			//	pStation->m_bMonFailed[i + NUM_OF_ADAM_CH] = m_bAdam2Failed[i];
			//}

			//pStation->m_dMonTemp[i + NUM_OF_ADAM_CH] = m_dAdam2Temp[i] + pStation->m_dMonOffset[i + NUM_OF_ADAM_CH];
	}
}

//-----------------------------------------------------------------------------
// IPC command implementation
//-----------------------------------------------------------------------------
//