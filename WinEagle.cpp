/////////////////////////////////////////////////////////////////
//	WinEagle.cpp : interface of the CWinEagle class
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
#include "AC9000.h"
#include "AC9000_Constant.h"
#include "PR_Util.h"
#include "Cal_Util.h"

#define WINEAGLE_EXTERN
#include "WinEagle.h"

#include "TA1.h"
#include "TA2.h"
#include "InspOpt.h"
#include "ACF1WH.h"
#include "ACF2WH.h"
#include "SettingFile.h"
#include "AC9000Stn.h"
#include <io.h>
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CWinEagle, CAC9000Stn)

CWinEagle::CWinEagle()
{ 
	//PR Util
	nCamera						= ACF_CAM;
	nAutoModeCam				= ACF_CAM;
	bPRUseMouse					= FALSE;
	bPRUpdateLight				= TRUE;
	nCurCalPt					= CAL_LEFT;

	for (INT i = 0; i < NO_OF_INPUT_CHANNEL; i++)
	{
		bChannelInUse[i] = FALSE;
	}
	
	for (INT i = 0; i < PR_MAX_ROW_IN_TEXT_COORD + 1; i++)
	{
		bDirtyRow[i]			= FALSE;
	}

	// WinEagle
	m_bPRFailed					= FALSE;
	m_bPRDebug					= FALSE;

	m_szVisionNumber			= "---";
	m_szVisionVersion			= "---";
//	m_szVisionLibVersion		= "3.32.01";
	m_szVisionLibVersion		= "xx.xx.xxxx"; //20120106 get version no of interface

	m_bSwitchAutoMode			= FALSE;
	m_lAutoBondMode				= 0;

	m_bMoveToPosn				= FALSE;
	
	ulShowPRID					= 0;
	ulShowValidID				= 0;
	ulShowRecordType			= 0;
	ulShowOwnerID				= 0;
	
	ulTestCounter				= 0;
	ulTestDelay					= 0;

	ulShowExposureTime			= 0;
	
	bShowPRCoaxialCtrl			= FALSE;
	bShowPRCoaxial1Ctrl			= FALSE;
	bShowPRCoaxial2Ctrl			= FALSE;
	bShowPRRingCtrl				= FALSE;
	bShowPRRing1Ctrl			= FALSE;
	bShowPRSideCtrl				= FALSE;
	bShowPRSide1Ctrl			= FALSE;	// 20141023
	bShowPRBackCtrl				= FALSE;	// 20141023
	bShowPRBack1Ctrl			= FALSE;	// 20141023

	ulHMICoaxialLevel			= 0;
	ulHMICoaxial1Level			= 0;
	ulHMICoaxial2Level			= 0;
	ulHMIRingLevel				= 0;
	ulHMIRing1Level				= 0;
	ulHMISideLevel				= 0;

	m_prZoomCentre.x			= PR_DEF_CENTRE_X;
	m_prZoomCentre.y			= PR_DEF_CENTRE_Y;
	g_stCursor.prCoord.x		= PR_DEF_CENTRE_X;
	g_stCursor.prCoord.y		= PR_DEF_CENTRE_Y;
	g_stCursor.lSize			= PRS_XLARGE_SIZE;	//PRS_LARGE_SIZE;
	g_stCursor.lColor			= PR_COLOR_GREEN;
	m_stCentreCursor.prCoord.x	= PR_DEF_CENTRE_X;
	m_stCentreCursor.prCoord.y	= PR_DEF_CENTRE_Y;
	m_stCentreCursor.lSize		= PRS_XLARGE_SIZE; //PRS_MEDIUM_SIZE;
	m_stCentreCursor.lColor		= PR_COLOR_RED;
	m_lCursorDiagnSteps			= 500;
	m_lCurrZoomFactor			= PR_ZOOM_FACTOR_1X;
	m_lCurrZoomPanFactor		= PR_ZOOM_FACTOR_END; //20150324

	m_PRZoomedPRPt_a.x			= PR_DEF_CENTRE_X;
	m_PRZoomedPRPt_a.y			= PR_DEF_CENTRE_Y;
	ZoomCentre.x				= PR_DEF_CENTRE_X;
	ZoomCentre.y				= PR_DEF_CENTRE_Y;

#if 1 //20130906
	m_lIncludeAnglePosDeltaX	= 0;
	m_lIncludeAnglePosDeltaY	= 0;
	m_lIncludeAngleNegDeltaX	= 0;
	m_lIncludeAngleNegDeltaY	= 0;
#endif
	
	HMI_bCalibPRModSelected		= FALSE;
	HMI_bShowCalibPRModSelected	= FALSE;
	HMI_bCalibModSelected		= FALSE;
	HMI_bShowCalibModSelected	= FALSE;

	HMI_bPRUCalibrated			= FALSE;
	HMI_bCORCalibrated			= FALSE;

	HMI_dIncludedAnglePos		= 0.0; //20130530
	HMI_dIncludedAngleNeg		= 0.0;
	HMI_bShowIncludeAngle		= FALSE;

	HMI_lCalibPRDelay			= 0;
	HMI_dCalibCORRange			= 0.0;
	HMI_dTestCORAng				= 0.0;
	HMI_bShowAutoCal			= FALSE;
	HMI_bShowCORCal				= FALSE;
	HMI_bShowThermalRef			= FALSE; //20140627 PBDL calib

	HMI_bLSITableTab			= FALSE;
	HMI_bINWHTableTab			= FALSE;
	HMI_bUplookXYTab			= FALSE;
	HMI_bPreBondWHTab			= FALSE;
	HMI_bShuttle1Tab			= FALSE;
	HMI_bShuttle2Tab			= FALSE;
	HMI_bTA4Tab					= FALSE;
	HMI_bMBDLTab				= FALSE; // 20141030
	HMI_bACFULTab				= FALSE; // 20141223
	HMI_bVACSnr					= FALSE; //20130318

	HMI_ulPRAlg					= 0;
	HMI_ulPRFMarkType			= 0;

	HMI_bShowPRAlg				= FALSE;
	HMI_bShowFiducial			= FALSE;
	HMI_bShowCursorControl		= FALSE;
	HMI_bShowDigitalZoom		= FALSE;

	HMI_szCurPRStation			= "";
	HMI_lCurRecord				= 0;

	HMI_lErrorLimit				= 10; //20140923

	PRS_InitPRUs();				// must add here
}

CWinEagle::~CWinEagle()
{
}

BOOL CWinEagle::InitInstance()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	CAC9000Stn::InitInstance();

	CString szMsg = _T("");
	INT nCount = 0;
	PRU *pPRU = NULL;

	//20120106 get version no of interface
	PR_UBYTE tmpVer[40];
	PRS_GetHostInterfaceVersionNo(&tmpVer[0]);
	m_szVisionLibVersion.Format("%s", tmpVer);
	szMsg.Format("vision interface version=%s", m_szVisionLibVersion);
	DisplayMessage(szMsg);

	// PR not initalized
	if (!g_bIsVisionNTInited && !m_bPRFailed)
	{
		BOOL bStatus = FALSE;

		if (!m_fHardware)
		{
			g_bIsVisionNTInited = TRUE;
			szMsg = _T("   ") + GetName() + " --- Not Enabled";
			DisplayMessage(szMsg);
		}
		else //if (pAppMod->m_bHWInited && pAppMod->m_bPRInit)
		{
			do 
			{
				Sleep(1000);
				nCount++;

			} while (!pAppMod->ScanVisionNT());

			if (g_bIsVisionNTInited || getKlocworkFalse())	// 20140722 Yip: Show Download PR Record Message After Vision Inited
			{
				SetAlert(IDS_WAIT_PR_DOWNLOAD_RECORD);
			}

#ifdef _DEBUG	// 20140714 Yip: Close "No grabber board detected" Dialog Automatically
			HWND hHandle = NULL;
			INT nTime = 0;
			do 
			{
				Sleep(500);
				hHandle = FindWindow("#32770", "ASM Vision Group");
				nTime += 500;
			} while ((hHandle == NULL) && (nTime < 30000));
			if (hHandle != NULL)
			{
				SendMessage(hHandle, WM_SYSCOMMAND, SC_CLOSE, 0);
			}
#endif

			szMsg.Format("Time Used for HMI Initialization = %ds", nCount);
			DisplayMessage(szMsg);

			//bStatus = PRS_Init();
			bStatus = PRS_ConnectToVision();
			m_bPRFailed = !bStatus;

			if (!m_bPRFailed)
			{
				CString szMsg = "";
				//DOUBLE dProcessTime = 0.0;
				//DWORD dwStartTime = GetTickCount();

				PRS_InitOptics();

				//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
				//szMsg.Format("PRS_InitOptics: %.5f", dProcessTime);
				//DisplayMessage(szMsg);

				PRS_DisplayVideo(&pruACF_WHCalib );
				szMsg = _T("   ") + GetName() + " --- PR Init Completed.";
				DisplayMessage(szMsg);

				PR_GET_VERSION_NO_RPY stRpy;

				if (PRS_GetVersionNo(&stRpy))
				{
					m_szVisionNumber = stRpy.aubSoftwareNo;
					m_szVisionVersion = stRpy.aubVersionNo;
				}

				//dProcessTime = 0.0;
				//dwStartTime = GetTickCount();
				FreeAllRecords();
				PRS_InitHookRFuncOnMouseClick();
				
				// debug time
				//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
				//szMsg.Format("Free All Record: %.5f", dProcessTime);
				//DisplayMessage(szMsg);
				
				//dProcessTime = 0.0;
				//dwStartTime = GetTickCount();

				for (INT i = 1; i < GetmyPRUList().GetSize(); i++)
				{
					LONG lRetRecordID = 0;
					pPRU = GetmyPRUList().GetAt(i);
					//////////
					if (::strlen((char*)&pPRU->stDownloadRecordCmd.acFilename[0]) == 0)
					{
						szMsg.Format("PR Load nothing. i=%d", i);
						DisplayMessage(szMsg);
					}
					else
					{
						PRS_DownloadRecordProcess(pPRU, TRUE, lRetRecordID);
						if (pPRU->bLoaded && lRetRecordID != 0)
						{
							szMsg.Format("PRFile %s, RecID:%ld", pPRU->stDownloadRecordCmd.acFilename, lRetRecordID);
							DisplayMessage(szMsg);
							SetRecordIDToPRUCmd(pPRU, (PRU_ID)lRetRecordID);
						}
						else
						{
							pPRU->bLoaded = FALSE;
							szMsg.Format("PRFile ERROR %s", pPRU->stDownloadRecordCmd.acFilename);
							DisplayMessage(szMsg);
						}
					}
					PRS_EnableHWTrigLighting(pPRU->nCamera, g_bEnableTrigLighting); //20141201
				}
				for (INT i = 0; i < GetmyCalibPRUList().GetSize(); i++)
				{
					LONG lRetRecordID = 0;
					pPRU = GetmyCalibPRUList().GetAt(i);
					if (::strlen((char*)&pPRU->stDownloadRecordCmd.acFilename[0]) == 0)
					{
						szMsg.Format("PR Calib Load nothing. i=%d", i);
						DisplayMessage(szMsg);
					}
					else
					{
						PRS_DownloadRecordProcess(pPRU, TRUE, lRetRecordID);
						if (pPRU->bLoaded)
						{
							szMsg.Format("CalPR File %s, RecID:%ld", pPRU->stDownloadRecordCmd.acFilename, lRetRecordID);
							DisplayMessage(szMsg);
							SetRecordIDToPRUCmd(pPRU, (PRU_ID)lRetRecordID);
						}
					}
				}

				CloseAlarm();		//If crash here. Please make sure that Group ID is correct.
				g_bIsVisionNTInited = bStatus;
				szMsg = _T("   ") + GetName() + " --- PR Records Downloaded.";
				DisplayMessage(szMsg);
			}
		}
	}

	return TRUE;
}


INT CWinEagle::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CAC9000Stn::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CWinEagle::Operation()
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

	Sleep(10);
}


/////////////////////////////////////////////////////////////////
//Update Functions
/////////////////////////////////////////////////////////////////
VOID CWinEagle::UpdateLighting(PRU *pPRU)
{
	if (pPRU == NULL)
	{
		return;
	}

	switch (pPRU->emCameraNo)
	{
	case PR_CAMERA_1:	//INWH
		bShowPRCoaxialCtrl 	= TRUE;
		bShowPRCoaxial1Ctrl = FALSE;
		bShowPRCoaxial2Ctrl = FALSE;
		bShowPRRingCtrl 	= TRUE;
		bShowPRRing1Ctrl 	= FALSE;
		bShowPRSideCtrl		= FALSE;
		bShowPRSide1Ctrl	= FALSE;		//20141023
		bShowPRBackCtrl		= FALSE;
		bShowPRBack1Ctrl	= FALSE;
		break;

	case PR_CAMERA_2:	//INSP1
		bShowPRCoaxialCtrl 	= FALSE;
		bShowPRCoaxial1Ctrl = FALSE;
		bShowPRCoaxial2Ctrl = FALSE;
		bShowPRRingCtrl 	= FALSE;
		bShowPRRing1Ctrl 	= FALSE;
		bShowPRSideCtrl		= TRUE;
		bShowPRSide1Ctrl	= FALSE;		//20141023
		bShowPRBackCtrl		= TRUE;
		bShowPRBack1Ctrl	= FALSE;
		break;

	case PR_CAMERA_3:	//INSP2
		bShowPRCoaxialCtrl 	= TRUE;
		bShowPRCoaxial1Ctrl = FALSE;
		bShowPRCoaxial2Ctrl = FALSE;
		bShowPRRingCtrl 	= FALSE;
		bShowPRRing1Ctrl 	= FALSE;
		bShowPRSideCtrl		= FALSE;
		bShowPRSide1Ctrl	= FALSE;		//20141023
		bShowPRBackCtrl		= FALSE;
		bShowPRBack1Ctrl	= FALSE;
		break;

	case PR_CAMERA_4:	// PBDL1
		bShowPRCoaxialCtrl 	= TRUE;
		bShowPRCoaxial1Ctrl = FALSE;
		bShowPRCoaxial2Ctrl = FALSE;
		bShowPRRingCtrl 	= TRUE;
		bShowPRRing1Ctrl 	= TRUE;
		bShowPRSideCtrl		= FALSE;
		bShowPRSide1Ctrl	= FALSE;		//20141023
		bShowPRBackCtrl		= TRUE;
		bShowPRBack1Ctrl	= FALSE;
		break;

	case PR_CAMERA_5:	// PBDL2
		bShowPRCoaxialCtrl 	= TRUE;
		bShowPRCoaxial1Ctrl = FALSE;
		bShowPRCoaxial1Ctrl = FALSE;
		bShowPRCoaxial2Ctrl = FALSE;
		bShowPRRingCtrl 	= TRUE;
		bShowPRRing1Ctrl 	= TRUE;
		bShowPRSideCtrl		= FALSE;
		bShowPRSide1Ctrl	= FALSE;		//20141023
		bShowPRBackCtrl		= TRUE;
		bShowPRBack1Ctrl	= FALSE;
		break;

	case PR_CAMERA_6:	// PBUL1
		bShowPRCoaxialCtrl 	= TRUE;
		bShowPRCoaxial1Ctrl = FALSE;
		bShowPRCoaxial1Ctrl = FALSE;
		bShowPRCoaxial2Ctrl = FALSE;
		bShowPRRingCtrl 	= TRUE;
		bShowPRRing1Ctrl 	= TRUE;
		bShowPRSideCtrl		= FALSE;
		bShowPRSide1Ctrl	= FALSE;		//20141023
		bShowPRBackCtrl		= FALSE;
		bShowPRBack1Ctrl	= FALSE;
		break;
		
	case PR_CAMERA_7:	// PBUL2
		bShowPRCoaxialCtrl 	= TRUE;
		bShowPRCoaxial1Ctrl = FALSE;
		bShowPRCoaxial2Ctrl = FALSE;
		bShowPRRingCtrl 	= TRUE;
		bShowPRRing1Ctrl 	= TRUE;
		bShowPRSideCtrl		= FALSE;
		bShowPRSide1Ctrl	= FALSE;		//20141023
		bShowPRBackCtrl		= FALSE;
		bShowPRBack1Ctrl	= FALSE;
		break;

	case PR_CAMERA_8:	// SH1
		bShowPRCoaxialCtrl 	= TRUE;
		bShowPRCoaxial1Ctrl = TRUE;
		bShowPRCoaxial2Ctrl = FALSE;
		bShowPRRingCtrl 	= TRUE;
		bShowPRRing1Ctrl 	= FALSE;
		bShowPRSideCtrl		= FALSE;
		bShowPRSide1Ctrl	= FALSE;		//20141023
		bShowPRBackCtrl		= TRUE;
		bShowPRBack1Ctrl	= FALSE;
		break;

	case PR_CAMERA_9:	// SH2
		bShowPRCoaxialCtrl 	= TRUE;
		bShowPRCoaxial1Ctrl = TRUE;
		bShowPRCoaxial2Ctrl = FALSE;
		bShowPRRingCtrl 	= TRUE;
		bShowPRRing1Ctrl 	= FALSE;
		bShowPRSideCtrl		= FALSE;
		bShowPRSide1Ctrl	= FALSE;		//20141023
		bShowPRBackCtrl		= TRUE;
		bShowPRBack1Ctrl	= FALSE;
		break;

	case PR_CAMERA_10:	// LSI
		bShowPRCoaxialCtrl 	= TRUE;
		bShowPRCoaxial1Ctrl = FALSE;
		bShowPRCoaxial2Ctrl = FALSE;
		bShowPRRingCtrl 	= TRUE;
		bShowPRRing1Ctrl 	= FALSE;
		bShowPRSideCtrl		= TRUE;
		bShowPRSide1Ctrl	= FALSE;		//20141023
		bShowPRBackCtrl		= FALSE;
		bShowPRBack1Ctrl	= FALSE;
		break;

	case PR_CAMERA_11:	// TA4
		bShowPRCoaxialCtrl 	= TRUE;
		bShowPRCoaxial1Ctrl = FALSE;
		bShowPRCoaxial2Ctrl = FALSE;
		bShowPRRingCtrl 	= FALSE;
		bShowPRRing1Ctrl 	= FALSE;
		bShowPRSideCtrl		= FALSE;
		bShowPRSide1Ctrl	= FALSE;		//20141023
		bShowPRBackCtrl		= FALSE;
		bShowPRBack1Ctrl	= FALSE;
		break;

	case PR_CAMERA_12:	// MBDL		// 20141023
		bShowPRCoaxialCtrl 	= TRUE;
		bShowPRCoaxial1Ctrl = FALSE;
		bShowPRCoaxial2Ctrl = FALSE;
		bShowPRRingCtrl 	= FALSE;
		bShowPRRing1Ctrl 	= FALSE;
		bShowPRSideCtrl		= FALSE;
		bShowPRSide1Ctrl	= FALSE;		//20141023
		bShowPRBackCtrl		= TRUE;
		bShowPRBack1Ctrl	= TRUE;
		break;

	case PR_CAMERA_13:	// ACFUL		// 20141223
		bShowPRCoaxialCtrl 	= TRUE;
		bShowPRCoaxial1Ctrl = FALSE;
		bShowPRCoaxial2Ctrl = FALSE;
		bShowPRRingCtrl 	= TRUE;
		bShowPRRing1Ctrl 	= FALSE;
		bShowPRSideCtrl		= TRUE;
		bShowPRSide1Ctrl	= TRUE;		//20141023
		bShowPRBackCtrl		= FALSE;
		bShowPRBack1Ctrl	= FALSE;
		break;
	}
}

VOID CWinEagle::UpdateOutput()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	if ((m_qState == UN_INITIALIZE_Q) || (m_qState == DE_INITIAL_Q))
	{
		return;
	}

	if (pAppMod->m_bHWInitError)
	{
		return;
	}

	if (!g_bIsVisionNTInited)
	{
		return;
	}

	PRU *pPRU = NULL;

	try
	{
		// update lighting
		if (bPRUpdateLight)
		{
			bPRUpdateLight = FALSE;

			if (nCamera != NO_CAM)
			{
				pPRU = &PRS_SetCameraPRU(nCamera);
		
				if (m_fHardware)
				{
					ulHMICoaxialLevel	= (ULONG)PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT);
					ulHMICoaxial1Level	= (ULONG)PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT_1);
					ulHMICoaxial2Level	= (ULONG)PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT_2);
					ulHMIRingLevel		= (ULONG)PRS_GetLighting(pPRU, PR_RING_LIGHT);
					ulHMIRing1Level		= (ULONG)PRS_GetLighting(pPRU, PR_RING_LIGHT_1);
					ulHMISideLevel		= (ULONG)PRS_GetLighting(pPRU, PR_SIDE_LIGHT);
					ulHMISide1Level		= (ULONG)PRS_GetLighting(pPRU, PR_SIDE_LIGHT_1);	//20141023
					ulHMIBackLevel		= (ULONG)PRS_GetLighting(pPRU, PR_BACK_LIGHT);		//20141023
					ulHMIBack1Level		= (ULONG)PRS_GetLighting(pPRU, PR_BACK_LIGHT_1);	//20141023
				}
				else
				{
					ulHMICoaxialLevel	= 50;
					ulHMICoaxial1Level	= 60;
					ulHMICoaxial2Level	= 70;
					ulHMIRingLevel		= 80;
					ulHMIRing1Level		= 90;
					ulHMISideLevel		= 100;
					ulHMISide1Level		= 100;
					ulHMIBackLevel		= 100;
					ulHMIBack1Level		= 100;
				}

			}
			else
			{
				ulHMICoaxialLevel	= 0;
				ulHMICoaxial1Level	= 0;
				ulHMICoaxial2Level	= 0;
				ulHMIRingLevel		= 0;
				ulHMIRing1Level		= 0;
				ulHMISideLevel		= 0;
				ulHMISide1Level		= 0;		// 20141023
				ulHMIBackLevel		= 0;
				ulHMIBack1Level		= 0;
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
}

VOID CWinEagle::RegisterVariables()
{
	try
	{

		// -------------------------------------------------------
		// CPRTaskStn Variable and Function
		// -------------------------------------------------------
		RegVariable(_T("PR_ulCurPRU"), &HMI_ulCurPRU);
		RegVariable(_T("PR_ulPRID"), &HMI_ulPRID);
		RegVariable(_T("PR_dPRMatchScore"), &HMI_dPRMatchScore);
		RegVariable(_T("PR_dPRAngleRange"), &HMI_dPRAngleRange);
		RegVariable(_T("PR_dPRPercentVar"), &HMI_dPRPercentVar);
		RegVariable(_T("PR_bShowPRSearch"), &HMI_bShowPRSearch);

		RegVariable(_T("PR_lThermalRefPosnX"), &m_mtrThermalCalibRefPosn.x); //20140627 PBDL calib
		RegVariable(_T("PR_lThermalRefPosnY"), &m_mtrThermalCalibRefPosn.y);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PR_SetPRMatchScore"), HMI_SetPRMatchScore);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PR_SetPRAngRange"), HMI_SetPRAngRange);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PR_SetPRPercentVar"), HMI_SetPRPercentVar);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PR_SrchPR"), HMI_SrchPR);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PR_ModifyPRCriteria"), HMI_ModifyPRCriteria);
		// -------------------------------------------------------
		// varibales
		// -------------------------------------------------------
		RegVariable(_T("PR_bSwitchAutoMode"), &m_bSwitchAutoMode);
		RegVariable(_T("PR_lAutoBondMode"), &m_lAutoBondMode);
		
		RegVariable(_T("VisionNumber"), &m_szVisionNumber);
		RegVariable(_T("VisionVersion"), &m_szVisionVersion);
		RegVariable(_T("VisionLibVersion"), &m_szVisionLibVersion);
		
		RegVariable(_T("PR_ulShowPRID"), &ulShowPRID);
		RegVariable(_T("PR_ulShowValidID"), &ulShowValidID);
		RegVariable(_T("PR_ulShowRecordType"), &ulShowRecordType);
		RegVariable(_T("PR_ulShowOwnerID"), &ulShowOwnerID);

		RegVariable(_T("PR_ulTestCounter"), &ulTestCounter);
		RegVariable(_T("PR_ulTestDelay"), &ulTestDelay);

		// Control Lighting
		RegVariable(_T("PR_ulShowExposureTime"), &ulShowExposureTime);

		RegVariable(_T("PR_bShowPRCoaxialCtrl"), &bShowPRCoaxialCtrl);
		RegVariable(_T("PR_bShowPRCoaxial1Ctrl"), &bShowPRCoaxial1Ctrl);
		RegVariable(_T("PR_bShowPRCoaxial2Ctrl"), &bShowPRCoaxial2Ctrl);
		RegVariable(_T("PR_bShowPRRingCtrl"), &bShowPRRingCtrl);
		RegVariable(_T("PR_bShowPRRing1Ctrl"), &bShowPRRing1Ctrl);
		RegVariable(_T("PR_bShowPRSideCtrl"), &bShowPRSideCtrl);
		RegVariable(_T("PR_bShowPRSide1Ctrl"), &bShowPRSide1Ctrl);		// 20141023
		RegVariable(_T("PR_bShowPRBackCtrl"), &bShowPRBackCtrl);		// 20141023
		RegVariable(_T("PR_bShowPRBack1Ctrl"), &bShowPRBack1Ctrl);		// 20141023

		RegVariable(_T("PR_ulHMICoaxialLevel"), &ulHMICoaxialLevel);
		RegVariable(_T("PR_ulHMICoaxial1Level"), &ulHMICoaxial1Level);
		RegVariable(_T("PR_ulHMICoaxial2Level"), &ulHMICoaxial2Level);
		RegVariable(_T("PR_ulHMIRingLevel"), &ulHMIRingLevel);
		RegVariable(_T("PR_ulHMIRing1Level"), &ulHMIRing1Level);
		RegVariable(_T("PR_ulHMISideLevel"), &ulHMISideLevel);
		RegVariable(_T("PR_ulHMISide1Level"), &ulHMISide1Level);		// 20141023
		RegVariable(_T("PR_ulHMIBackLevel"), &ulHMIBackLevel);			// 20141023
		RegVariable(_T("PR_ulHMIBack1Level"), &ulHMIBack1Level);		// 20141023

		//Calib Setting
		RegVariable(_T("PR_bCalibPRModSelected"), &HMI_bCalibPRModSelected);
		RegVariable(_T("PR_bShowCalibPRModSelected"), &HMI_bShowCalibPRModSelected);
		RegVariable(_T("PR_bCalibModSelected"), &HMI_bCalibModSelected);
		RegVariable(_T("PR_bShowCalibModSelected"), &HMI_bShowCalibModSelected);
		RegVariable(_T("PR_bShowThermalRef"), &HMI_bShowThermalRef); //20140627 PBDL calib

		RegVariable(_T("PR_bPRUCalibrated"), &HMI_bPRUCalibrated);
		RegVariable(_T("PR_bCORCalibrated"), &HMI_bCORCalibrated);
		RegVariable(_T("PR_lCalibPRDelay"), &HMI_lCalibPRDelay);
		RegVariable(_T("PR_dCalibCORRange"), &HMI_dCalibCORRange);
		RegVariable(_T("PR_dTestCORAng"), &HMI_dTestCORAng);
		RegVariable(_T("PR_bShowAutoCal"), &HMI_bShowAutoCal);
		RegVariable(_T("PR_bShowCORCal"), &HMI_bShowCORCal);

		RegVariable(_T("PR_dIncludedAnglePos"), &HMI_dIncludedAnglePos); //20130530
		RegVariable(_T("PR_dIncludedAngleNeg"), &HMI_dIncludedAngleNeg);
		RegVariable(_T("PR_bShowIncludeAngle"), &HMI_bShowIncludeAngle);

		RegVariable(_T("PR_bLSITableTab"), &HMI_bLSITableTab);
		RegVariable(_T("PR_bINWHTableTab"), &HMI_bINWHTableTab);	
		RegVariable(_T("PR_bPreBondWHTab"), &HMI_bPreBondWHTab);
		RegVariable(_T("PR_bUplookXYTab"), &HMI_bUplookXYTab);
		RegVariable(_T("PR_bShuttle1Tab"), &HMI_bShuttle1Tab);
		RegVariable(_T("PR_bShuttle2Tab"), &HMI_bShuttle2Tab);
		RegVariable(_T("PR_bTA4Tab"), &HMI_bTA4Tab);
		RegVariable(_T("PR_bMBDLTab"), &HMI_bMBDLTab);
		RegVariable(_T("PR_bACFULTab"), &HMI_bACFULTab);

		// Setup Learn
		RegVariable(_T("PR_ulPRAlg"), &HMI_ulPRAlg);
		RegVariable(_T("PR_ulPRFMarkType"), &HMI_ulPRFMarkType);

		RegVariable(_T("PR_bShowPRAlg"), &HMI_bShowPRAlg);
		RegVariable(_T("PR_bShowFiducial"), &HMI_bShowFiducial);
		RegVariable(_T("PR_bShowCursorControl"), &HMI_bShowCursorControl);
		RegVariable(_T("PR_bShowDigitalZoom"), &HMI_bShowDigitalZoom);

		RegVariable(_T("PR_bShowDigitalZoomOnly"), &HMI_bShowDigitalZoomOnly); //20150315

		RegVariable(_T("PR_bVACSnr"), &HMI_bVACSnr); //20130318
	
		RegVariable(_T("PR_lCurRecord"), &HMI_lCurRecord);

		RegVariable(_T("PR_lErrorLimit"), &HMI_lErrorLimit); //20140923
	
		// commands
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMISwitchCam", HMISwitchCam);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_SelectCurStation", HMI_SelectCurStation);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_SelectCurRecord", HMI_SelectCurRecord);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIUpdateDiagnPRScreen", HMIUpdateDiagnPRScreen);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIUpdateServicePRScreen", HMIUpdateServicePRScreen);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIClearPRRecord", HMIClearPRRecord);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIStartRepeatTest", HMIStartRepeatTest);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMISetTestCounter", HMISetTestCounter);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMISetTestDelay", HMISetTestDelay);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrExposureTime", HMIIncrExposureTime);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrExposureTime", HMIDecrExposureTime);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrCoaxialLevel", HMIIncrCoaxialLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrCoaxialLevel", HMIDecrCoaxialLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrCoaxialLevel10", HMIIncrCoaxialLevel10);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrCoaxialLevel10", HMIDecrCoaxialLevel10);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrCoaxial1Level", HMIIncrCoaxial1Level);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrCoaxial1Level", HMIDecrCoaxial1Level);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrCoaxial1Level10", HMIIncrCoaxial1Level10);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrCoaxial1Level10", HMIDecrCoaxial1Level10);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrCoaxial2Level", HMIIncrCoaxial2Level);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrCoaxial2Level", HMIDecrCoaxial2Level);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrCoaxial2Level10", HMIIncrCoaxial2Level10);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrCoaxial2Level10", HMIDecrCoaxial2Level10);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrRingLevel", HMIIncrRingLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrRingLevel", HMIDecrRingLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrRingLevel10", HMIIncrRingLevel10);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrRingLevel10", HMIDecrRingLevel10);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrRing1Level", HMIIncrRing1Level);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrRing1Level", HMIDecrRing1Level);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrRing1Level10", HMIIncrRing1Level10);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrRing1Level10", HMIDecrRing1Level10);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrSideLevel", HMIIncrSideLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrSideLevel", HMIDecrSideLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrSideLevel10", HMIIncrSideLevel10);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrSideLevel10", HMIDecrSideLevel10);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrSide1Level", HMIIncrSide1Level);		// 20141023
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrSide1Level", HMIDecrSide1Level);		// 20141023
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrSide1Level10", HMIIncrSide1Level10);	// 20141023
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrSide1Level10", HMIDecrSide1Level10);	// 20141023

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrBackLevel", HMIIncrBackLevel);		// 20141023
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrBackLevel", HMIDecrBackLevel);		// 20141023
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrBackLevel10", HMIIncrBackLevel10);	// 20141023
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrBackLevel10", HMIDecrBackLevel10);	// 20141023

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrBack1Level", HMIIncrBack1Level);		// 20141023
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrBack1Level", HMIDecrBack1Level);		// 20141023
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIIncrBack1Level10", HMIIncrBack1Level10);	// 20141023
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIDecrBack1Level10", HMIDecrBack1Level10);	// 20141023

		// Cursor Related
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_SetCursorDiagnSteps", HMI_SetCursorDiagnSteps);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_IndexCursorXPos", HMI_IndexCursorXPos);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_IndexCursorXNeg", HMI_IndexCursorXNeg);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_IndexCursorYPos", HMI_IndexCursorYPos);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_IndexCursorYNeg", HMI_IndexCursorYNeg);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_ZoomIn", HMI_ZoomIn);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_ZoomOut", HMI_ZoomOut);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_ZoomInOnly", HMI_ZoomInOnly); //20150314 for zoom and pan
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_ZoomOutOnly", HMI_ZoomOutOnly);
		
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIFreeAllRecords", HMIFreeAllRecords);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMIOnOffLogMode", HMIOnOffLogMode);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMISetDebugFlag", HMISetDebugFlag);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMISetComLogFlag", HMISetComLogFlag);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMISetComLogTimeLimit", HMISetComLogTimeLimit);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMISetVisionNtDlgToService", HMISetVisionNtDlgToService);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_HMISetVisionNtDlgToVision", HMISetVisionNtDlgToVision);

		// Caibration Setup Page
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_SetCalibPRSelected", HMI_SetCalibPRSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_SetCalibModSelected", HMI_SetCalibModSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_SelectCalibPRU", HMI_SelectCalibPRU);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_SelectPRAlg", HMI_SelectPRAlg);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_SelectFMarkType", HMI_SelectFMarkType);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_SetCalibPRDelay", HMI_SetCalibPRDelay);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_SetCalibCORRange", HMI_SetCalibCORRange);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_SetTestCORAng", HMI_SetTestCORAng);
		//m_comServer.IPC_REG_SERVICE_COMMAND("SetThermalCalibRefPosn", HMI_SetThermalCalibRefPosn); //20140627 PBDL calib
		//m_comServer.IPC_REG_SERVICE_COMMAND("GoThermalCalibRefPosn", HMI_GoThermalCalibRefPosn);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_AutoCal", HMI_AutoCal);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_CORCal", HMI_CORCal);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_DistortionCal", HMI_DistortionCal);
		//m_comServer.IPC_REG_SERVICE_COMMAND("PR_CORCalLeastSqr", HMI_CORCalLeastSqr); //20160530
		//m_comServer.IPC_REG_SERVICE_COMMAND("PR_CORCalNew", HMI_CORCalNew); //20160525

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_AutoTest", HMI_AutoTest);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_CORAutoTest", HMI_CORAutoTest);//201490923

		//m_comServer.IPC_REG_SERVICE_COMMAND("PR_TA4CORTest", HMI_TA4CORTest);//20141120

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_SetPRErrorLimit", HMI_SetPRErrorLimit);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_CheckPRCentre", HMI_CheckPRCentre);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_SetCORCentre", HMI_SetCORCentre);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_CheckCORCentre", HMI_CheckCORCentre);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_GetPRCentreMap", HMI_GetPRCentreMap);

		m_comServer.IPC_REG_SERVICE_COMMAND("PR_ToggleVAC", HMI_ToggleVAC); //20130318
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_ResetIncludedAngle", HMI_ResetIncludedAngle); //20130530

		// 20140815 Yip
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_SetUserSearchPRContinue", HMI_SetUserSearchPRContinue);
		m_comServer.IPC_REG_SERVICE_COMMAND("PR_SetUserRejectPRTask", HMI_SetUserRejectPRTask);

		//m_comServer.IPC_REG_SERVICE_COMMAND("PR_VerifyPreBondCalibrationPositions", HMI_VerifyPreBondCalibrationPositions);	// 20140915 Yip
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
VOID CWinEagle::PreStartOperation()
{
	if (m_fHardware && m_lAutoBondMode)
	{
		m_bSwitchAutoMode = FALSE;
		PRS_AutoBondMode(TRUE, FALSE);
	}

#ifdef MOUSE_CLICK_PR
	m_bUserRejectPRTask = FALSE;
	m_bUserSearchPRDone = FALSE;

	WINEAGLE_Status = ST_IDLE;
	m_qSubOperation = WINEAGLE_IDLE_Q;
	m_bStopCycle = FALSE;
	g_bShowUserSearchPRDone = FALSE;
#endif
	Motion(FALSE);
	State(IDLE_Q);
#ifdef MOUSE_CLICK_PR
	m_deqDebugSeq.clear(); //20131010
	m_qPreviousSubOperation = m_qSubOperation;
	UpdateDebugSeq(GetSequenceString(), WINEAGLE_Status);
	
#endif
	CAC9000Stn::PreStartOperation();
}

VOID CWinEagle::AutoOperation()
{
	if (m_fHardware && m_bSwitchAutoMode && m_lAutoBondMode)
	{
		m_bSwitchAutoMode = FALSE;
		PRS_AutoBondMode(TRUE, TRUE);
	}

	if (m_fHardware && m_bSwitchAutoMode && !m_lAutoBondMode)
	{
		m_bSwitchAutoMode = FALSE;
		PRS_AutoBondMode(FALSE, FALSE);
	}

#ifdef MOUSE_CLICK_PR
	OperationSequence(); //20150813
#endif

	// stop command received: end operation after cycle completed
	if (Command() == glSTOP_COMMAND || IsAbort() || g_bStopCycle)
	{
		Command(glSTOP_COMMAND);
		State(STOPPING_Q);

#ifdef MOUSE_CLICK_PR
		WINEAGLE_Status = ST_STOP;
		m_bStopCycle = FALSE;	
		CAC9000Stn::StopOperation();
#endif
	}


}

CString CWinEagle::GetSequenceString()	// 20140815 Yip
{
	switch (m_qSubOperation)
	{
	case WINEAGLE_IDLE_Q:
		return "WINEAGLE_IDLE_Q";

	case WINEAGLE_WAIT_REQUEST_Q:
		return "WINEAGLE_WAIT_REQUEST_Q";

	case WINEAGLE_WAIT_USER_CLICK_PR_Q:
		return "WINEAGLE_WAIT_USER_CLICK_PR_Q";

	case WINEAGLE_WAIT_STATION_RELEASE_Q:
		return "WINEAGLE_WAIT_STATION_RELEASE_Q";

	case WINEAGLE_STOP_Q:
		return "WINEAGLE_STOP_Q";
	}

	return "Hi";
}

VOID CWinEagle::OperationSequence()	// 20140815 Yip: Add WinEagle Auto Bond Operation Sequence
{
	switch (m_qSubOperation)
	{
	case WINEAGLE_IDLE_Q:
		m_qSubOperation = WINEAGLE_WAIT_REQUEST_Q;
		break;

	case WINEAGLE_WAIT_REQUEST_Q:
		if (m_bStopCycle)
		{
			m_qSubOperation = WINEAGLE_STOP_Q;
		}
		//else if (INWH_Status == ST_REQ_CLICK_PR)
		//{
		//	// Turn Off AutoBond First
		//	if (m_lAutoBondMode) // Reset Display Video
		//	{
		//		PRS_AutoBondMode(FALSE, FALSE);
		//	}

		//	INWH_Status = ST_ACK_CLICK_PR;
		//	WINEAGLE_Status = ST_BUSY;

		//	m_qSubOperation = WINEAGLE_WAIT_USER_CLICK_PR_Q;
		//}
		//else if (SHUTTLE_Status == ST_REQ_CLICK_PR)
		//{
		//	// Turn Off AutoBond First
		//	if (m_lAutoBondMode) // Reset Display Video
		//	{
		//		PRS_AutoBondMode(FALSE, FALSE);
		//	}

		//	SHUTTLE_Status = ST_ACK_CLICK_PR;
		//	WINEAGLE_Status = ST_BUSY;

		//	m_qSubOperation = WINEAGLE_WAIT_USER_CLICK_PR_Q;
		//}
		//else if (PREBOND_HEAD_Status[PREBOND_1] == ST_REQ_CLICK_PR)
		//{
		//	// Turn Off AutoBond First
		//	if (m_lAutoBondMode) // Reset Display Video
		//	{
		//		PRS_AutoBondMode(FALSE, FALSE);
		//	}

		//	PREBOND_HEAD_Status[PREBOND_1] = ST_ACK_CLICK_PR;
		//	WINEAGLE_Status = ST_BUSY;

		//	m_qSubOperation = WINEAGLE_WAIT_USER_CLICK_PR_Q;
		//}
		//else if (PREBOND_HEAD_Status[PREBOND_2] == ST_REQ_CLICK_PR)
		//{
		//	// Turn Off AutoBond First
		//	if (m_lAutoBondMode) // Reset Display Video
		//	{
		//		PRS_AutoBondMode(FALSE, FALSE);
		//	}

		//	PREBOND_HEAD_Status[PREBOND_2] = ST_ACK_CLICK_PR;
		//	WINEAGLE_Status = ST_BUSY;

		//	m_qSubOperation = WINEAGLE_WAIT_USER_CLICK_PR_Q;
		//}
		break;

	case WINEAGLE_WAIT_USER_CLICK_PR_Q:
		if (m_bStopCycle)	// User Cancel Operation
		{
			m_qSubOperation = WINEAGLE_STOP_Q;
		}
		//else if (m_bUserSearchPRDone) // Waiting Finish user clicked 'DONE' in HMI
		//{
		//	// NOT AutoBond Mode
		//	if (!m_lAutoBondMode) // Reset Display Video
		//	{
		//		PRU *pPRU;
		//		pPRU = &PRS_SetCameraPRU(nCamera);
		//		PRS_DisplayVideo(pPRU);
		//	}
		//	else
		//	{
		//		PRS_AutoBondMode(TRUE, FALSE);
		//	}

		//	WINEAGLE_Status = ST_DONE;

		//	m_bUserSearchPRDone = FALSE;
		//	m_qSubOperation = WINEAGLE_WAIT_STATION_RELEASE_Q;
		//}
		//else if (m_bUserRejectPRTask)
		//{
		//	if (!m_lAutoBondMode)
		//	{
		//		 // NOT AutoBond Mode
		//		// Reset Display Video
		//		PRU *pPRU;
		//		pPRU = &PRS_SetCameraPRU(nCamera);
		//		PRS_DisplayVideo(pPRU);
		//	}
		//	else
		//	{
		//		PRS_AutoBondMode(TRUE, FALSE);
		//	}

		//	WINEAGLE_Status = ST_USER_REJECT;

		//	m_bUserRejectPRTask = FALSE;
		//	m_qSubOperation = WINEAGLE_WAIT_STATION_RELEASE_Q;

		//}
		break;

	case WINEAGLE_WAIT_STATION_RELEASE_Q:
		if (WINEAGLE_Status == ST_RELEASE)
		{
			WINEAGLE_Status = ST_IDLE;
			m_qSubOperation = WINEAGLE_WAIT_REQUEST_Q;
		}
		break;

	case WINEAGLE_STOP_Q:
		break;
	}

	if (m_qPreviousSubOperation != m_qSubOperation)
	{
		if (HMI_bDebugSeq)
		{
			CString szMsg;
			if (WINEAGLE_STOP_Q != m_qSubOperation)
			{
				szMsg.Format("%s Go State: %ld", GetStnName(), m_qSubOperation);
			}
			else
			{
				szMsg.Format("%s Go Stop", GetStnName());
			}
			DisplayMessage(szMsg);

		}
		UpdateDebugSeq(GetSequenceString(), WINEAGLE_Status);
		m_qPreviousSubOperation = m_qSubOperation;
	}
}


// My derivation
BOOL CWinEagle::PRS_Init()
{
	unsigned short		Channel[TOTAL_NO_OF_CHANNEL];
	//unsigned short		Channel[TOTAL_NO_OF_CHANNEL];
	PR_WARM_START_RPY	stCommRpy;
	PR_UWORD			uwStatus = PR_COMM_NOERR;

	CString szMsg = _T("");
	BOOL bResult = FALSE;

	//INT	i;
	INT	j;

//	do //klocwork fix 20121211
	while (TRUE)
	{
		open_EmuSI_drvr(TRUE);

		for (INT i = 0; i < TOTAL_NO_OF_CHANNEL; ++i)
		{
			Channel[i] = 0x200;
		}

		//Channel[0] = 0x200;
		//Channel[1] = 0x200;
		//Channel[2] = 0x200;
		//Channel[3] = 0x80;
		//Channel[4] = 0x80;
		//Channel[5] = 0x80;
		//Channel[6] = 0x80;
		//Channel[7] = 0x80;

		//Channel[8] = 0x80;
		//Channel[9] = 0x80;
		//Channel[10] = 0x80;
		//Channel[11] = 0x80;

		//Channel[0] = 0x200;
		//Channel[1] = 0x200;
		//Channel[2] = 0x200;
		//Channel[3] = 0x200;
		//Channel[4] = 0x200;
		//Channel[5] = 0x200;
		//Channel[6] = 0x200;
		//Channel[7] = 0x80;

		//Channel[8] = 0x80;
		//Channel[9] = 0x80;
		//Channel[10] = 0x80;
		//Channel[11] = 0x80;

		//Channel[12] = 0x80;
		//Channel[13] = 0x80;
		//Channel[14] = 0x80;
		//Channel[15] = 0x80;
		//Channel[16] = 0x80;
		//Channel[17] = 0x80;
		//Channel[18] = 0x80;
		//Channel[19] = 0x80;
		//Channel[20] = 0x80;
		//Channel[21] = 0x80;
		//Channel[22] = 0x80;
		//Channel[23] = 0x80;
		//Channel[24] = 0x80;
		//Channel[25] = 0x80;

		// Initialize the SI commnication
		m_pDPR = Dpr_Master_Init_Ext(DPR_PORT, NO_OF_INPUT_CHANNEL, NO_OF_OUTPUT_CHANNEL, 
									 HOST_SYSTEM_ID, Channel, "AC9000 Host", DPR_EMULATED_MODE);

		if (NULL == m_pDPR)
		{
			SetError(IDS_DPR_MASTER_INIT_ERR);
			DisplayMessage("ERROR: DPR Master Init failed!");
			break;
		}
		szMsg = _T("   ") + GetName() + " --- DPR Master Init success.";
		DisplayMessage(szMsg);

		// Wait for Slave to be initialized (300 sec timeout)
		j = 0;
		do
		{
			Sleep(1000);
			j++;
		} while (1 == Dpr_Slave_Initialized(m_pDPR) && j < 300);

		//szMsg.Format("Time Used for DPR Slave Initialization = %ds", j);
		//DisplayMessage(szMsg);

		if (j >= 300)
		{
			CloseAlarm();
			SetError(IDS_DPR_SLAVE_INIT_ERR);
			DisplayMessage("ERROR: DPR Slave Init timeout. Process terminated!");
			break;
		}
		szMsg = _T("   ") + GetName() + " --- DPR Slave Init success.";
		DisplayMessage(szMsg);

		// Install the interupt callback function
		Dpr_Master_InstallInterrupt(m_pDPR, PR_SetSICallBack, NULL);

		if (Dpr_IsInterruptEnabled(m_pDPR))
		{
			szMsg = _T("   ") + GetName() + " --- Master with interrupt ready.";
			DisplayMessage(szMsg);
		}
		else
		{
			szMsg = _T("   ") + GetName() + " --- Master without interrupt ready.";
			DisplayMessage(szMsg);
		}

		Sleep(1000);	// Ensure interrupt handler is installed before sending command

		// Initialize the communication in vision host interface
		PR_InitMultiSI(m_pDPR, HOST_SYSTEM_ID, VISION_SYSTEM_ID);

		// Call warm start to initalize the vision system
		PR_WarmStartCmd(PRS_ACF_SID, PRS_ACF_RID, &stCommRpy);

		if (PR_COMM_NOERR != stCommRpy.uwCommunStatus)
		{
			uwStatus = stCommRpy.uwCommunStatus;
		}
		else if (PR_ERR_NOERR != stCommRpy.uwPRStatus)
		{
			uwStatus = stCommRpy.uwPRStatus;
		}

		if (PR_ERR_NOERR != uwStatus)
		{
			SetError(IDS_PR_WARM_START_ERR);
			DisplayMessage("ERROR: PR warm start failed!");
			break;
		}

		// Initalized sucessfully
		bResult = TRUE;
		break;
	}
	//} while(TRUE); //klocwork fix 20121211

	return bResult;
}

BOOL CWinEagle::PRS_ConnectToVision()
{
#ifdef FAKE_VISION
	return TRUE;
#else
	PR_CONNECT_TO_VISION_STRUCT	stConnectToVisionStruct;
	PR_COMMON_RPY				stCommRpy;
	PR_UWORD					uwStatus = PR_COMM_NOERR;
	INT i = 0;


	PR_InitConnectToVisionStruct(&stConnectToVisionStruct);

	stConnectToVisionStruct.emIsMasterInit = PR_TRUE;
	stConnectToVisionStruct.emCommMedia = PR_COMM_MEDIA_EMSI;
	stConnectToVisionStruct.ubHostID = HOST_SYSTEM_ID;
	stConnectToVisionStruct.ubPRID = VISION_SYSTEM_ID;

	stConnectToVisionStruct.ubNumOfHostInCh = NO_OF_INPUT_CHANNEL;
	stConnectToVisionStruct.ubNumOfHostOutCh = NO_OF_OUTPUT_CHANNEL;

	for (i = 0; i < NO_OF_INPUT_CHANNEL; i++)
	{
		stConnectToVisionStruct.auwHostInChSize[i] = 0x200; 
	}

	for (i = 0; i < NO_OF_OUTPUT_CHANNEL; i++)
	{
		stConnectToVisionStruct.auwHostOutChSize[i] = 0x200;
	}

	stConnectToVisionStruct.ubNumOfRPCInCh	= 1;
	stConnectToVisionStruct.ubNumOfRPCOutCh = 1;

	stConnectToVisionStruct.ulTimeout = 300000;		// 5 Mins
	DisplayMessage("Connecting to Vision...");
	PR_ConnectToVision(&stConnectToVisionStruct, &stCommRpy);
	DisplayMessage("Connect to Vision Done.");

	if (PR_COMM_NOERR != stCommRpy.uwCommunStatus)
	{
		uwStatus = stCommRpy.uwCommunStatus;
	}
	else if (PR_ERR_NOERR != stCommRpy.uwPRStatus)
	{
		uwStatus = stCommRpy.uwPRStatus;
	}

	if (PR_ERR_NOERR != uwStatus)
	{
		SetError(IDS_PR_WARM_START_ERR);
		DisplayMessage("ERROR: PR start failed!");
		return FALSE;
	}
	else
	{
		// Initalized sucessfully
		return TRUE;
	}
#endif
}	



VOID CWinEagle::PRS_InitOptics()
{
	INT		i = 0;
	PRU	*pPRU = NULL;

	for (i = 0; i < GetmyPRUList().GetSize(); i++)
	{
		pPRU = GetmyPRUList().GetAt(i);
		PRS_GetOptic(pPRU);
	}

	for (i = 0; i < GetmyCalibPRUList().GetSize(); i++)
	{
		pPRU = GetmyCalibPRUList().GetAt(i);
		PRS_GetOptic(pPRU);
	}
	PRS_GetOptic(&pruACF_WHCalib ); //20150625
}

VOID CWinEagle::PRS_InitPRUs()
{
	INT		i = 0;
	CString szFileName;

	m_myPRUList.RemoveAll();
	m_myCalibPRUList.RemoveAll();

	// Menu PRU
	pruMenu.emID			= ID_NONE;
	PRS_InitPRU(&pruMenu, NO_CAM, ID_NONE , "", "", CMG_NONE, CMG_NONE, CMG_NONE, CMG_NONE);
	m_myPRUList.Add((PRU*)&pruMenu);

#ifdef TA1_Y
	PRS_InitPRU(&pruIN1, ACFUL_CAM, ID_ACFUL_PR, "ACFUL", "ACFUL_PR", CMG_ACFUL_XTA1_YTA1, CMG_NONE, CMG_NONE, CMG_NONE);
	m_myPRUList.Add((PRU*)&pruIN1);
#endif

	PRS_InitPRU(&pruACF1, ACF_CAM, ID_ACF_PR1_TYPE1, "ACFDL", "ACF_PR1", CMG_ACFDL_XACFDL_YACF1WH, CMG_ACFDL_XACFDL_YACF2WH, CMG_NONE, CMG_NONE);
	m_myPRUList.Add((PRU*)&pruACF1);

	PRS_InitPRU(&pruACF2, ACF_CAM, ID_ACF_PR2_TYPE1, "ACFDL", "ACF_PR2", CMG_ACFDL_XACFDL_YACF1WH, CMG_ACFDL_XACFDL_YACF2WH, CMG_NONE, CMG_NONE);
	m_myPRUList.Add((PRU*)&pruACF2);
	//ACF PostBd PRU For Insp
	for (i = 0; i < MAX_NUM_OF_ACF; i++)
	{
		szFileName.Format("Insp_ACF%ld_PR1", i + 1);
		PRS_InitPRU(&pruACFPB1[i][OBJECT], ACF_CAM, ID_INSP1_ACFPB1_TYPE1 + i * 4, "Insp1", szFileName, CMG_INSP1_XTA3_YNIL, CMG_NONE, CMG_NONE, CMG_NONE);
		m_myPRUList.Add((PRU*)&pruACFPB1[i][OBJECT]);

		szFileName.Format("Insp_ACF%ld_PR2", i + 1);
		PRS_InitPRU(&pruACFPB2[i][OBJECT], ACF_CAM, ID_INSP1_ACFPB2_TYPE1 + i * 4, "Insp1", szFileName, CMG_INSP1_XTA3_YNIL, CMG_NONE, CMG_NONE, CMG_NONE);
		m_myPRUList.Add((PRU*)&pruACFPB2[i][OBJECT]);

		szFileName.Format("Insp_ACF%ld_PR1_BackGround", i + 1);
		PRS_InitPRU(&pruACFPB1[i][BACKGROUND], ACF_CAM, ID_INSP1_ACFPB1_BACKGROUND_TYPE1 + i * 4, "NONE", szFileName, CMG_INSP1_XTA3_YNIL, CMG_NONE, CMG_NONE, CMG_NONE); // if that record need to display at PR Service, please change the Station Name
		m_myPRUList.Add((PRU*)&pruACFPB1[i][BACKGROUND]);

		szFileName.Format("Insp_ACF%ld_PR2_BackGround", i + 1);
		PRS_InitPRU(&pruACFPB2[i][BACKGROUND], ACF_CAM, ID_INSP1_ACFPB2_BACKGROUND_TYPE1 + i * 4, "NONE", szFileName, CMG_INSP1_XTA3_YNIL, CMG_NONE, CMG_NONE, CMG_NONE);
		m_myPRUList.Add((PRU*)&pruACFPB2[i][BACKGROUND]);
	}

	///////////////////////
	// Calib PRU

	// ACF Cam with ACF WH Calibration PRU
	PRS_InitPRU(&pruACF_WHCalib, ACF_CAM, ID_ACF_WH_CALIB, "ACF_WH", "ACF_Calib", CMG_ACFDL_XACFDL_YACF1WH, CMG_ACFDL_XACFDL_YACF2WH, CMG_NONE, CMG_NONE);
	m_myCalibPRUList.Add(&pruACF_WHCalib);

#ifdef TA1_Y
	PRS_InitPRU(&pruACFUL_Calib, ACFUL_CAM, ID_ACF_UL_CALIB, "ACF_UL", "ACFUL_Calib", CMG_ACFUL_XTA1_YTA1, CMG_NONE, CMG_NONE, CMG_NONE);
	m_myCalibPRUList.Add(&pruACFUL_Calib);
#endif

	//LONG ltemp;
	//ltemp = GetmyPRUList().GetSize();
	//ltemp = GetmyCalibPRUList().GetSize();
	//protection the total num of myPRUList must equal to the last ID...
	if (GetmyPRUList().GetSize() + GetmyCalibPRUList().GetSize() > ID_MAX_NUM/*ID_SHUTTLE2_CALIB+1*/) // Must Use Afx to Pump Our Error
	{
		CString szError;
		AfxMessageBox("Error:: InitPRU");
		szError.Format("myPRUList.GetSize() = %ld", GetmyPRUList().GetSize());
		AfxMessageBox(szError);
		szError.Format("myCalibPRUList.GetSize() = %ld", GetmyCalibPRUList().GetSize());
		AfxMessageBox(szError);
	}

	// Dummy PRU
	//pruDummy.emID						= ID_NONE;
	//PRS_InitPRU(&pruDummy, NO_CAM);
	
}

VOID CWinEagle::FreeAllRecords()
{
	PRU *pPRU = NULL;
	PRS_FreeAllRecordsAllHost();

	for (INT i = 1; i < GetmyPRUList().GetSize(); i++)
	{
		pPRU = GetmyPRUList().GetAt(i);
		pPRU->bLoaded						= FALSE;
		pPRU->stSrchCmd.uwRecordID			= 0;//pPRU->emID;
		pPRU->stSrchDieCmd.auwRecordID[0]	= 0;//pPRU->emID;
		pPRU->stChangeRecordCmd.uwRecordID	= 0;//pPRU->emID;	//20150109
		pPRU->stDetectShapeCmd.uwRecordID	= 0;//pPRU->emID;
		pPRU->stSrchTmplCmd.uwRecordID		= 0;//pPRU->emID;
		pPRU->stSrchACFCmd.uwRecordID		= 0;//pPRU->emID;
		//pPRU->stModifyCritCmd.uwRecordID	= pPRU->emID; //20180208
		pPRU->stLrnCmd.uwRecordID			= 0; //pPRU->emID; //20180208 Let vision assign Record ID
	}

	for (INT i = 0; i < GetmyCalibPRUList().GetSize(); i++)
	{
		pPRU = GetmyCalibPRUList().GetAt(i);
		pPRU->bLoaded						= FALSE;
		pPRU->stSrchCmd.uwRecordID			= 0;//pPRU->emID;
		pPRU->stSrchDieCmd.auwRecordID[0]	= 0;//pPRU->emID;
		pPRU->stChangeRecordCmd.uwRecordID	= 0;//pPRU->emID;	//20150109
		pPRU->stDetectShapeCmd.uwRecordID	= 0;//pPRU->emID;
		pPRU->stSrchTmplCmd.uwRecordID		= 0;//pPRU->emID;
		pPRU->stSrchACFCmd.uwRecordID		= 0;//pPRU->emID;
		//pPRU->stModifyCritCmd.uwRecordID	= pPRU->emID; //20180208
		pPRU->stLrnCmd.uwRecordID			= 0;//pPRU->emID; //20180208 Let vision assign Record ID
	}
}

VOID CWinEagle::SetRecordIDToPRUCmd(PRU *pPRU, PRU_ID emID)
{
	if (pPRU->bLoaded)
	{
		pPRU->stSrchCmd.uwRecordID			= emID;
		pPRU->stSrchDieCmd.auwRecordID[0]	= emID;
		pPRU->stChangeRecordCmd.uwRecordID	= emID;
		pPRU->stDetectShapeCmd.uwRecordID	= emID;
		pPRU->stSrchTmplCmd.uwRecordID		= emID;
		pPRU->stSrchACFCmd.uwRecordID		= emID;
		pPRU->stModifyCritCmd.uwRecordID = emID; //20180208
		pPRU->stLrnCmd.uwRecordID			= 0;
	}
}

PRU &CWinEagle::SelectCurPRU(ULONG ulPRU)
{
	switch (ulPRU)
	{
	
	case ACF1WH_CALIB_PRU:
		return pruACF_WHCalib;

	case ACF2WH_CALIB_PRU:
		return pruACF_WHCalib;

	case ACFUL_CALIB_PRU:
		return pruACFUL_Calib;

	default:
		DisplayMessage("Kill Natalie:: SelectCurPRU Error");
		return pruACF_WHCalib;
	}
}

VOID CWinEagle::StartRepeatTest(PRU *pPRU)
{
	BOOL	bSrch = FALSE;
	CString szMsg = _T("");
	CString szPrintTitle = _T("PR_Test");
	ULONG		nCnt = ulTestCounter;

	if (!pPRU->bLoaded)
	{
		HMIMessageBox(MSG_OK, "PR REPEATABILITY TEST", "PR Pattern Not Loaded!");
		WriteHMIMess("Test Failed: Please re-learn pattern");
		return;
	}

	WriteHMIMess("PR Test in progress...");

	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	// start test	
	FILE *fp = fopen("D:\\sw\\AC9000\\Data\\PR_Test.txt", "wt");
	if (fp != NULL)
	{
		fprintf(fp, "N:\tX:\tY:\n");
		fclose(fp);
	}
	//else //klocwork fix 20121211
	//{
	//	if (fp != NULL)
	//	{
	//		fprintf(fp, "Record %d, PR Test", pPRU->emID);
	//	}
	//}

	while (pAppMod->bPRRepeatTest && nCnt > 0)
	{
	//		bSrch = PRS_InspCmd(pPRU);

		if(pPRU->emRecordType == PRS_ACF_TYPE)
		{
			bSrch = PRS_SrchACFCmd(pPRU);
		}
		else
		{
			bSrch = PRS_ManualSrchCmd(pPRU);
		}

		//switch (pPRU->emRecordType)
		//{
		//// Pattern Matching or Edge Matching
		//case PRS_DIE_TYPE:
		//	bSrch = PRS_SrchDie(pPRU);
		//	break;

		//// Shape Fitting
		//case PRS_SHAPE_TYPE:
		//	bSrch = PRS_DetectShape(pPRU);
		//	break;

		//// Tmpl Fitting
		//case PRS_TMPL_TYPE:
		//	bSrch = PRS_SrchTmpl(pPRU);
		//	break;

		//// Kerf Fitting
		//case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
		//	bSrch = PRS_InspKerf(pPRU);
		//	break;

		//// ACF 
		//case PRS_ACF_TYPE:
		//	bSrch = PRS_SrchACFCmd(pPRU);
		//	break;
		//}

		if (bSrch)
		{
		fp = fopen("D:\\sw\\AC9000\\Data\\PR_Test.txt", "a");

			if (pPRU->bIs5MBCam && pPRU->bUseFloatingPointResult) //20150728 float PR
			{
				szMsg.Format("Cycle %lu XY: (%.2f,%.2f), Ang: %.2f, Score: %.2f", 
						 nCnt, pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, pPRU->fAng, pPRU->fScore);
			}
			else
			{
				szMsg.Format("Cycle %lu XY: (%.2f,%.2f), Ang: %.2f, Score: %.2f", 
						 nCnt, pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, pPRU->fAng, pPRU->fScore);
			}

			if (fp != NULL)
			{

				if (pPRU->bIs5MBCam && pPRU->bUseFloatingPointResult) //20150728 float PR
				{
					fprintf(fp, "%lu\t%.2f\t%.2f\n", nCnt, pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y);
				}
				else
				{
					fprintf(fp, "%lu\t%.2f\t%.2f\n", nCnt, pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y);
				}
				fclose(fp);
			}
		}
		else
		{
			szMsg.Format("Cycle %ld Search Failed", ulTestCounter);
			if (fp != NULL)
			{
				fprintf(fp, "SEARCH FAIL\n");
				fclose(fp);
			}
		}

		nCnt--;

		WriteHMIMess(szMsg);
		Sleep(ulTestDelay);
	}

	if (!pAppMod->bPRRepeatTest)
	{
		WriteHMIMess("PR Test Cancelled");
	}
	else
	{
		WriteHMIMess("PR Test Completed");
		pAppMod->bPRRepeatTest = FALSE;
	}
}

///////////////////////////////////////////////////////
// Updated Page
//////////////////////////////////////////////////////
VOID CWinEagle::UpdateHMIDiagnPRScreen(BOOL bOperation)
{
	PRU	*pPRU = NULL;
		
	pPRU	= GetmyPRUList().GetAt(HMI_ulCurPRU);
	
	PRS_DisplayVideo(pPRU);

	if (!PRS_DispLrnInfo(pPRU))
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "No Pattern Learnt");
	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Displaying Learnt Pattern");
	}

	ulShowExposureTime	= (ULONG)PRS_GetExposureTime(pPRU);

	// pattern loaded: get record info
	if (pPRU->bLoaded)
	{
		if(pPRU->emRecordType == PRS_ACF_TYPE)
		{
			ulShowPRID = pPRU->stSrchACFCmd.uwRecordID;
		}
		else
		{
			ulShowPRID = pPRU->stSrchCmd.uwRecordID;
		}
		//switch (pPRU->emRecordType)
		//{
		//// Pattern Matching or Edge Matching
		//case PRS_DIE_TYPE:
		//	ulShowPRID = pPRU->stSrchDieCmd.auwRecordID[0];
		//	break;

		//// Shape Fitting
		//case PRS_SHAPE_TYPE:
		//	ulShowPRID = pPRU->stDetectShapeCmd.uwRecordID;
		//	break;

		//// Tmpl Fitting
		//case PRS_TMPL_TYPE:
		//	ulShowPRID = pPRU->stSrchTmplCmd.uwRecordID;
		//	break;

		//// Kerf Fitting
		//case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
		//	ulShowPRID = pPRU->stInspKerfCmd.uwRecordID;
		//	break;

		//case PRS_ACF_TYPE:
		//	ulShowPRID = pPRU->stSrchACFCmd.uwRecordID;
		//	break;

		//}

		PR_GET_RECORD_INFO_RPY	stRpy;
		if (PRS_GetRecordInfo(pPRU, &stRpy))
		{
			ulShowValidID		= stRpy.emValidID;
			ulShowOwnerID		= stRpy.ulOwnerID;

			switch (stRpy.emRecordType)
			{
			case PRS_DIE_TYPE:
				ulShowRecordType = 0;
				break;

			case PRS_SHAPE_TYPE:
				ulShowRecordType = 1;
				break;

			case PRS_TMPL_TYPE:
				ulShowRecordType = 2;
				break;
			
			case PRS_ACF_TYPE:
				ulShowRecordType = 3;
				break;
			
			case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
				ulShowRecordType = 4;
				break;
			}
		}
		else
		{
			ulShowValidID		= 0;		// no valid!
			ulShowRecordType	= 5;
			ulShowOwnerID		= 0;
		}
	}

	else
	{
		ulShowPRID			= 0;
		ulShowValidID		= 2;
		ulShowRecordType	= 5;
		ulShowOwnerID		= 0;
	}

	// set lighting control
	UpdateLighting(pPRU);
}

VOID CWinEagle::UpdateHMICalibPRButton(BOOL bOperation)
{
	PRU *pPRU;

	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");

	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	pPRU = &SelectCurPRU(HMI_ulCurPRU);

	switch (HMI_ulCurPRU)
	{

	case ACF1WH_CALIB_PRU:
		HMI_bCalibPRModSelected		= pCACF1WH->m_bPRSelected;
		HMI_bShowCalibPRModSelected	= TRUE;
		//HMI_bCalibModSelected		= pCINWH->m_bModSelected;
		HMI_bCalibModSelected		= pCACF1WH->m_bModSelected && pCInspOpt->m_bModSelected; //20120830
		HMI_bShowCalibModSelected	= pCACF1WH->m_bShowModSelected;
		HMI_lCalibPRDelay			= pCACF1WH->m_lCalibPRDelay;
		HMI_lErrorLimit				= pCACF1WH->m_lPRErrorLimit;
		HMI_bCORCalibrated			= pCACF1WH->m_bCORCalibrated[pCACF1WH->m_lCORCamNum];
		HMI_dCalibCORRange			= pCACF1WH->m_dCalibCORRange;
		HMI_dIncludedAnglePos		= 0.0; //20130530
		HMI_dIncludedAngleNeg		= 0.0; //20130530
		HMI_bShowIncludeAngle		= FALSE;

		HMI_bLSITableTab			= FALSE;
		HMI_bINWHTableTab			= TRUE;
		HMI_bPreBondWHTab			= FALSE;
		HMI_bUplookXYTab			= FALSE;
		HMI_bShuttle1Tab			= FALSE;
		HMI_bShuttle2Tab			= FALSE;
		HMI_bTA4Tab					= FALSE;
		HMI_bVACSnr					= FALSE; //20130318
		HMI_bMBDLTab				= FALSE;
		HMI_bACFULTab				= FALSE;	//20141230

		HMI_bShowThermalRef			= FALSE; //20140627 PBDL calib
		break;
		
	case ACF2WH_CALIB_PRU:
		HMI_bCalibPRModSelected		= pCACF2WH->m_bPRSelected;
		HMI_bShowCalibPRModSelected	= TRUE;
		//HMI_bCalibModSelected		= pCINWH->m_bModSelected;
		HMI_bCalibModSelected		= pCACF2WH->m_bModSelected && pCInspOpt->m_bModSelected; //20120830
		HMI_bShowCalibModSelected	= pCACF2WH->m_bShowModSelected;
		HMI_lCalibPRDelay			= pCACF2WH->m_lCalibPRDelay;
		HMI_lErrorLimit				= pCACF2WH->m_lPRErrorLimit;
		HMI_bCORCalibrated			= pCACF2WH->m_bCORCalibrated[pCACF2WH->m_lCORCamNum];
		HMI_dCalibCORRange			= pCACF2WH->m_dCalibCORRange;
		HMI_dIncludedAnglePos		= 0.0; //20130530
		HMI_dIncludedAngleNeg		= 0.0; //20130530
		HMI_bShowIncludeAngle		= FALSE;

		HMI_bLSITableTab			= FALSE;
		HMI_bINWHTableTab			= TRUE;
		HMI_bPreBondWHTab			= FALSE;
		HMI_bUplookXYTab			= FALSE;
		HMI_bShuttle1Tab			= FALSE;
		HMI_bShuttle2Tab			= FALSE;
		HMI_bTA4Tab					= FALSE;
		HMI_bVACSnr					= FALSE; //20130318
		HMI_bMBDLTab				= FALSE;
		HMI_bACFULTab				= FALSE;	//20141230

		HMI_bShowThermalRef			= FALSE; //20140627 PBDL calib
		break;
		
	case ACFUL_CALIB_PRU:
		HMI_bCalibPRModSelected		= pCTA1->m_bPRSelected;
		HMI_bShowCalibPRModSelected	= TRUE;
		//HMI_bCalibModSelected		= pCINWH->m_bModSelected;
		HMI_bCalibModSelected		= pCTA1->m_bModSelected; //20120830
		HMI_bShowCalibModSelected	= pCTA1->m_bShowModSelected;
		HMI_lCalibPRDelay			= pCTA1->m_lCalibPRDelay;
		HMI_lErrorLimit				= pCTA1->m_lPRErrorLimit;
		HMI_bCORCalibrated			= pCTA1->m_bCORCalibrated[pCTA1->m_lCORCamNum];
		HMI_dCalibCORRange			= pCTA1->m_dCalibCORRange;
		HMI_dIncludedAnglePos		= 0.0; //20130530
		HMI_dIncludedAngleNeg		= 0.0; //20130530
		HMI_bShowIncludeAngle		= FALSE;

		HMI_bLSITableTab			= FALSE;
		HMI_bINWHTableTab			= TRUE;
		HMI_bPreBondWHTab			= FALSE;
		HMI_bUplookXYTab			= FALSE;
		HMI_bShuttle1Tab			= FALSE;
		HMI_bShuttle2Tab			= FALSE;
		HMI_bTA4Tab					= FALSE;
		HMI_bVACSnr					= FALSE; //20130318
		HMI_bMBDLTab				= FALSE;
		HMI_bACFULTab				= FALSE;	//20141230

		HMI_bShowThermalRef			= FALSE; //20140627 PBDL calib
		break;
	}

	if (HMI_bVACSnr) //20130327
	{
		DisplayMessage("VAC Sol On");
	}
	else
	{
		DisplayMessage("VAC Sol Off");
	}

	SetHmiVariable("PR_bVACSnr"); //20130318

	HMI_bPRUCalibrated 			= pPRU->bCalibrated;
	HMI_bShowAutoCal			= pPRU->bLoaded;
	
	if (
		HMI_ulCurPRU == ACF1WH_CALIB_PRU ||
		//HMI_ulCurPRU == ACFUL_CALIB_PRU ||
		HMI_ulCurPRU == ACF2WH_CALIB_PRU
	   )
	{
		HMI_bShowCORCal				= pPRU->bLoaded && pPRU->bCalibrated;
	}
	else
	{
		HMI_bShowCORCal				= FALSE;
	}

	CPRTaskStn::UpdateHMIPRButton();

	// set lighting control and Display Video
	UpdateLighting(pPRU);
	PRS_DisplayVideo(pPRU);
	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);

}

LONG CWinEagle::SelectCalibPRU(ULONG ulPRU, BOOL bCheckModSelected)	// 20140915 Yip: Add bCheckModSelected
{
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH	*pCACF2WH	= (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");

	CString szMsg(" ");
	PRU *pPRU;
	LONG lAnswer = rMSG_TIMEOUT;

	switch (HMI_ulOldCurPRU)
	{
	case ACF1WH_CALIB_PRU:
		pCACF1WH->MoveTToStandby(GMP_WAIT);
		break;
	case ACF2WH_CALIB_PRU:
		pCACF2WH->MoveTToStandby(GMP_WAIT);
		break;
	case ACFUL_CALIB_PRU:
		pCTA1->MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT);
		break;
	}

	// protection
	if (HMI_ulCurPRU >= MAX_NUM_OF_CALIB_PRU)
	{
		HMI_ulCurPRU = ACF1WH_CALIB_PRU;
	}

	pPRU = &SelectCurPRU(HMI_ulCurPRU);

	switch (HMI_ulCurPRU)
	{

	case ACF1WH_CALIB_PRU:
		pruACF_WHCalib.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF1WH;
		if (m_bMoveToPosn)
		{
			pCACF1WH->MoveTToStandby(GMP_WAIT);
			pCACF1WH->OpticMoveAbsoluteXY(pCACF1WH->m_mtrPRCalibPosn.x, pCACF1WH->m_mtrPRCalibPosn.y, GMP_WAIT);
		}
		break;
	case ACF2WH_CALIB_PRU:
		pruACF_WHCalib.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF2WH;
		if (m_bMoveToPosn)
		{
			pCACF2WH->MoveTToStandby(GMP_WAIT);
			pCACF2WH->OpticMoveAbsoluteXY(pCACF2WH->m_mtrPRCalibPosn.x, pCACF2WH->m_mtrPRCalibPosn.y, GMP_WAIT);
		}
		break;
	case ACFUL_CALIB_PRU:
		if (m_bMoveToPosn)
		{
			pCTA1->MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
			pCTA1->MoveTPlacePosn(GMP_WAIT);
			pCTA1->MoveAbsoluteXY(pCTA1->m_mtrPRCalibPosn.x, pCTA1->m_mtrPRCalibPosn.y, GMP_WAIT);
			pCTA1->MoveZ(TA1_Z_CTRL_GO_PR_LEVEL, GMP_WAIT);
		}
		break;
	}
	
	PRS_DisplayVideo(pPRU);
	PRS_DrawHomeCursor(pPRU->emCameraNo, FALSE);

	return TRUE;
}


BOOL CWinEagle::CalcCentreCursorPt(PRU *pPRU, BOOL bDraw, PR_COORD const PreCursor, PR_COORD *pprCentreCursor)
{
	PR_COORD	prTempPt;
	PR_BOOLEAN	TempResultantCoordStatus;

	if (bDraw)
	{
		// draw PRTempAlignPt_a cursor
		PRS_GetPointInZoomImgFrom1xImg(m_lCurrZoomFactor, m_prZoomCentre, PreCursor, &prTempPt, &TempResultantCoordStatus);
		PRS_DrawCross(pPRU->emCameraNo, prTempPt, g_stCursor.lSize, g_stCursor.lColor);
	}
	
	PRS_GetPointFromZoomedImg(m_lCurrZoomFactor, m_prZoomCentre, g_stCursor.prCoord, &prTempPt);

	PR_COORD prTempCentre;

	prTempCentre.x = (PreCursor.x + prTempPt.x) / 2;
	prTempCentre.y = (PreCursor.y + prTempPt.y) / 2;

	PRS_GetPointInZoomImgFrom1xImg(m_lCurrZoomFactor, m_prZoomCentre, prTempCentre, pprCentreCursor, &TempResultantCoordStatus);


	return TempResultantCoordStatus;
}

/////////////////////////////////////////////////////////////////
//General PR Setup Related 
/////////////////////////////////////////////////////////////////
#ifdef PRS_SET_IGNORE_WINDOW_FIRST //20150324
VOID CWinEagle::SetPRIgnoreWindow(PRU *pPRU)
{
	PR_COORD	coUpperLeft;
	PR_COORD	coLowerRight;
	CString		szMsg = _T(" ");

	szMsg.Format("PR ignore windows Setup...");
	DisplayMessage(szMsg);

	PRS_CancelMouseWindow(pPRU);

	coUpperLeft.x = 8192 * 2 / 5;
	coUpperLeft.y = 8192 * 2 / 5;
	coLowerRight.x = 8192 * 3 / 5;
	coLowerRight.y = 8192 * 3 / 5;

	PRS_ClearScreen(pPRU->emCameraNo);
	PRS_DisplayText(pPRU->emCameraNo, 1, PRS_MSG_ROW1, "Set Ignore Window");
	//PRS_CancelMouseWindow(pPRU);
	PRS_UseMouseWindow(pPRU, coUpperLeft, coLowerRight);
	PRS_DrawHomeCursor(pPRU->emCameraNo, FALSE);
	return;
}

VOID CWinEagle::SetPRIgnoreWindowFinish(PRU *pPRU)
{
	PR_COORD	coUpperLeft;
	PR_COORD	coLowerRight;
	CString		szMsg = _T(" ");
	//stCmd.stAlign.stStreet.stDetectIgnoreWin.uwNIgnoreWin
	if (g_ulWizardIgnoreWindowStep >= MAX_PR_IGNORE_WINDOWS)
	{
		szMsg.Format("Error: Total ignore windows exceed %d. Operation Abort!", MAX_PR_IGNORE_WINDOWS);
		DisplayMessage(szMsg);
		return;
	}
	pPRU->stManualLrnDieCmd.stAlign.stStreet.stDetectIgnoreWin.uwNIgnoreWin = (PR_UWORD) g_ulWizardIgnoreWindowStep;
	szMsg.Format("Number of ignore windows: %ld.", g_ulWizardIgnoreWindowStep);
	HMIMessageBox(MSG_OK, "IGNORE WINDOW", szMsg);
	szMsg.Format("Number of ignore windows is %ld.", g_ulWizardIgnoreWindowStep);
	DisplayMessage(szMsg);

	//restore mouse rectangle selection
	switch (HMI_ulPRAlg)
	{
	case PATTERN_MATCHING:
	case EDGE_MATCHING:
	case RECTANGLE_MATCHING:
		coUpperLeft.x = pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].x;
		coUpperLeft.y = pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].y;
		coLowerRight.x = pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].x;
		coLowerRight.y = pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].y;
		break;

	case SHAPE_FITTING:
		coUpperLeft.x = pPRU->stLrnShapeCmd.stEncRect.coCorner1.x;
		coUpperLeft.y = pPRU->stLrnShapeCmd.stEncRect.coCorner1.y;
		coLowerRight.x = pPRU->stLrnShapeCmd.stEncRect.coCorner2.x;
		coLowerRight.y = pPRU->stLrnShapeCmd.stEncRect.coCorner2.y;
		break;

	case TMPL_FITTING:
		coUpperLeft.x = pPRU->stLoadTmplCmd.coTmplCentre.x - pPRU->stLoadTmplCmd.szTmplSize.x;
		coUpperLeft.y = pPRU->stLoadTmplCmd.coTmplCentre.y - pPRU->stLoadTmplCmd.szTmplSize.y;
		coLowerRight.x = pPRU->stLoadTmplCmd.coTmplCentre.x + pPRU->stLoadTmplCmd.szTmplSize.x;
		coLowerRight.y = pPRU->stLoadTmplCmd.coTmplCentre.y + pPRU->stLoadTmplCmd.szTmplSize.y;
		break;

	case FIDUCIAL_FITTING:
		coUpperLeft.x = pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].x;
		coUpperLeft.y = pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].y;
		coLowerRight.x = pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].x;
		coLowerRight.y = pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].y;
		break;
	}
	PRS_ClearTextRow(pPRU->emCameraNo, PRS_MSG_ROW1); //20150716
	PRS_CancelMouseWindow(pPRU);
	//PRS_ClearScreen(pPRU->emCameraNo);
	PRS_DisplayText(pPRU->emCameraNo, 1, PRS_MSG_ROW1, "Ready to learn pattern");

	PRS_UseMouseWindow(pPRU, coUpperLeft, coLowerRight);
	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	
	return;
}

VOID CWinEagle::SetPRIgnoreWindowNext(PRU *pPRU)
{
	//stCmd.stAlign.stStreet.stDetectIgnoreWin.astIgnoreWin[i]
	//stCmd.stAlign.stStreet.stDetectIgnoreWin.uwNIgnoreWin
	//PR_COORD        coCorner1;      /* Upper-left corner */
	//PR_COORD        coCorner2;      /* Lower-right corner */
/*
typedef enum {	PR_COLOR_TRANSPARENT = 0, PR_COLOR_RED = 1, PR_COLOR_YELLOW = 2,
				PR_COLOR_BLUE = 3, PR_COLOR_GREEN = 4,
				PR_COLOR_CYAN = 5, PR_COLOR_MAGENTA = 6,
				PR_COLOR_GREY = 7, PR_COLOR_BLACK = 8, PR_COLOR_FOREGROUND, 
				PR_COLOR_RESTORE_FOREGROUND,
				PR_COLOR_END = 15} PR_COLOR;
*/
	PR_COORD	coUpperLeft;
	PR_COORD	coLowerRight;
	ULONG		ulIndex = 0;
	CString		szMsg = _T(" ");

	if (g_ulWizardIgnoreWindowStep > MAX_PR_IGNORE_WINDOWS)
	{
		szMsg.Format("Error: Number of Ignore Windows exceed %d. Operation Abort!", MAX_PR_IGNORE_WINDOWS);
		DisplayMessage(szMsg);
		szMsg.Format("Number of Ignore Windows exceed %d. Operation Abort!", MAX_PR_IGNORE_WINDOWS);
		HMIMessageBox(MSG_OK, "WARNING", szMsg);
		return;
	}
	szMsg.Format("Current ignore windows %ld.", g_ulWizardIgnoreWindowStep);
	DisplayMessage(szMsg);

	ulIndex = (g_ulWizardIgnoreWindowStep - 1) % MAX_PR_IGNORE_WINDOWS;
	PRS_GetMouseWindow(pPRU, &coUpperLeft, &coLowerRight);
	PRS_CancelMouseWindow(pPRU);
	
	PRS_DrawRect(pPRU->nCamera, coUpperLeft, coLowerRight, PR_COLOR_RED/*(PR_COLOR)(PR_COLOR_RED + ulIndex)*/);

	pPRU->stManualLrnDieCmd.stAlign.stStreet.stDetectIgnoreWin.astIgnoreWin[ulIndex].coCorner1.x = coUpperLeft.x;
	pPRU->stManualLrnDieCmd.stAlign.stStreet.stDetectIgnoreWin.astIgnoreWin[ulIndex].coCorner1.y = coUpperLeft.y;
	pPRU->stManualLrnDieCmd.stAlign.stStreet.stDetectIgnoreWin.astIgnoreWin[ulIndex].coCorner2.x = coLowerRight.x;
	pPRU->stManualLrnDieCmd.stAlign.stStreet.stDetectIgnoreWin.astIgnoreWin[ulIndex].coCorner2.y = coLowerRight.y;

	szMsg.Format("coUpperLeft=(%ld, %ld) coLowerRight=(%ld, %ld).", (LONG)coUpperLeft.x, (LONG)coUpperLeft.y, (LONG)coLowerRight.x, (LONG)coLowerRight.y);
	DisplayMessage(szMsg);

	coUpperLeft.x = 8192 * 2 / 5;
	coUpperLeft.y = 8192 * 2 / 5;
	coLowerRight.x = 8192 * 3 / 5;
	coLowerRight.y = 8192 * 3 / 5;
	//PRS_CancelMouseWindow(pPRU);
	PRS_UseMouseWindow(pPRU, coUpperLeft, coLowerRight);
	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	return;
}
#endif

VOID CWinEagle::SetupPRSrchArea(PRU *pPRU)
{
	BOOL bResult = TRUE;
	PR_COORD	coUpperLeft;
	PR_COORD	coLowerRight;

	if (g_ulWizardStep > 1 && g_ulWizardStep != 99)
	{
		g_ulWizardStep = 1;
	}

	HMI_bShowPRAlg			= FALSE;
	HMI_bShowFiducial		= FALSE;

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

	SetHmiVariable("PR_bShowPRAlg");
	SetHmiVariable("PR_bShowFiducial");

//	PRS_DisplayVideo(pPRU);

	// Perform Operations
	switch (g_ulWizardStep)
	{
	case 99:
		PRS_DisplayVideo(pPRU);
		PRS_CancelMouseWindow(pPRU);

		g_szWizardStep = "";
		g_szWizardMess = "";
		g_szWizardBMPath = "";

		g_bWizardEnableBack		= FALSE;
		g_bWizardEnableNext		= FALSE;
		g_bWizardEnableExit		= TRUE;

		break;

	case 0:
		PRS_DisplayVideo(pPRU);
		PRS_DrawHomeCursor(pPRU->nCamera, FALSE);

		switch (pPRU->emRecordType)
		{
		// Pattern Matching or Edge Matching
		case PRS_DIE_TYPE:
			PRS_UseMouseWindow(pPRU, 
							   pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1, 
							   pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2);
			break;

		// Shape Fitting
		case PRS_SHAPE_TYPE:
			PRS_UseMouseWindow(pPRU, 
							   pPRU->stDetectShapeCmd.stInspWin.coCorner1, 
							   pPRU->stDetectShapeCmd.stInspWin.coCorner2);
			break;

		// Tmpl Fitting
		case PRS_TMPL_TYPE:
			PRS_UseMouseWindow(pPRU, 
							   pPRU->stSrchTmplCmd.stSrchWin.coCorner1, 
							   pPRU->stSrchTmplCmd.stSrchWin.coCorner2);
			break;

		// Kerf Fitting
		case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
			PRS_UseMouseWindow(pPRU, 
							   pPRU->stLrnKerfCmd.stROI.coCorner1, 
							   pPRU->stLrnKerfCmd.stROI.coCorner2);
			break;
		}

		g_szWizardStep = "Step 1/1";
		g_szWizardMessPrev = "";
		g_szWizardMess = "1. Define Pattern Search Area with Mouse...";
		g_szWizardMessNext = "";
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;

		break;

	case 1:
		PRS_GetMouseWindow(pPRU, &coUpperLeft, &coLowerRight);

		switch (pPRU->emRecordType)
		{
		// Pattern Matching or Edge Matching
		case PRS_DIE_TYPE:
			SetPRDieSrchArea(pPRU, coUpperLeft, coLowerRight);
			break;

		// Shape Fitting
		case PRS_SHAPE_TYPE:
			SetPRShapeSrchArea(pPRU, coUpperLeft, coLowerRight);
			break;

		// Tmpl Fitting
		case PRS_TMPL_TYPE:
			SetPRTmplSrchArea(pPRU, coUpperLeft, coLowerRight);
			break;

		// Kerf Fitting
		case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
			SetPRKerfSrchArea(pPRU, coUpperLeft, coLowerRight);
			break;
		}

		PRS_CancelMouseWindow(pPRU);

		g_szWizardStep = "DONE";
		g_szWizardMess = "DONE";
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = FALSE;
		g_bWizardEnableExit = TRUE;

		break;
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

VOID CWinEagle::SetupPRPattern(PRU *pPRU)
{
	BOOL		bResult = TRUE;
	PR_COORD	coUpperLeft;
	PR_COORD	coLowerRight;

	PR_COORD	arcoDieCorners[4];
	PR_SIZE		szTmplSize;
	PR_COORD	coTmplCentre;

	CString		str = "";

	// g_ulWizardStep =98 // skip the follow step
	if (g_ulWizardStep > 6 && g_ulWizardStep != 99 && g_ulWizardStep != 98)
	{
		g_ulWizardStep = 6;
	}

	HMI_bShowPRAlg			= FALSE;
	HMI_bShowFiducial		= FALSE;
	HMI_bShowCursorControl	= FALSE;
	HMI_bShowDigitalZoom	= FALSE;

	SetHmiVariable("SF_bWizardEnable");
	SetHmiVariable("SF_szWizardStep");
	SetHmiVariable("SF_szWizardMess");
	SetHmiVariable("SF_szWizardMessPrev");
	SetHmiVariable("SF_szWizardMessNext");
	SetHmiVariable("SF_szWizardBMPath");
	SetHmiVariable("SF_bWizardEnableBack");
	SetHmiVariable("SF_bWizardEnableNext");
	SetHmiVariable("SF_bWizardEnableExit");

	SetHmiVariable("PR_bShowPRAlg");
	SetHmiVariable("PR_bShowFiducial");
	SetHmiVariable("PR_bShowCursorControl");
	SetHmiVariable("PR_bShowDigitalZoom");

	// 20140815 Yip: Add Manual Learn Die Ignore Region
	PR_DETECT_IGNORE_WIN *pstDetectIgnoreWin = &pPRU->stManualLrnDieCmd.stAlign.stStreet.stDetectIgnoreWin;

	switch (g_ulWizardStep)
	{
	case 99:
		// Reset Digital Zoom
		m_lCurrZoomFactor = PR_ZOOM_FACTOR_1X;
		PRS_DisableDigitalZoom(pPRU->nCamera);
		g_stCursor.prCoord.x = PR_DEF_CENTRE_X;
		g_stCursor.prCoord.y = PR_DEF_CENTRE_Y;
		PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	
		PRS_DisplayVideo(pPRU);
		PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
		PRS_CancelMouseWindow(pPRU);
		PRS_SetOnMouseClickCmd(OFF);
		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);

		g_szWizardStep			= "";
		g_szWizardMess			= "";
		g_szWizardBMPath		= "";

		g_bWizardEnableBack		= FALSE;
		g_bWizardEnableNext		= FALSE;
		g_bWizardEnableExit		= TRUE;

		HMI_bShowPRAlg			= FALSE;
		HMI_bShowFiducial		= FALSE;
		HMI_bShowCursorControl	= FALSE;
		HMI_bShowDigitalZoom	= FALSE;

		break;
	
	case 0:
		PRS_DisplayVideo(pPRU);
		//if (PRS_CopyUserPurpose(pPRU->emPurpose[PURPOSE0], pPRU->emPurpose[PURPOSE1]))
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Ready to learn pattern");
			g_szWizardStep = "Step 1/2";
			g_szWizardMess.Format("1. Align Pattern with camera centre");
			g_szWizardMessNext.Format("2. Learn PR");
			g_szWizardBMPath = "";
		}
		//else
		//{
		//	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Copy user purpose fail!");
		//	bResult = FALSE;
		//	break;
		//}
		
		switch (HMI_ulPRAlg)
		{
		case PATTERN_MATCHING:
		case EDGE_MATCHING:
		case RECTANGLE_MATCHING:
			HMI_bShowFiducial = FALSE;
			coUpperLeft.x = pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].x;
			coUpperLeft.y = pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].y;
			coLowerRight.x = pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].x;
			coLowerRight.y = pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].y;
			break;

		case SHAPE_FITTING:
			HMI_bShowFiducial = FALSE;
			coUpperLeft.x = pPRU->stLrnShapeCmd.stEncRect.coCorner1.x;
			coUpperLeft.y = pPRU->stLrnShapeCmd.stEncRect.coCorner1.y;
			coLowerRight.x = pPRU->stLrnShapeCmd.stEncRect.coCorner2.x;
			coLowerRight.y = pPRU->stLrnShapeCmd.stEncRect.coCorner2.y;
			break;

		case TMPL_FITTING:
			HMI_bShowFiducial = FALSE;
			coUpperLeft.x = pPRU->stLoadTmplCmd.coTmplCentre.x - pPRU->stLoadTmplCmd.szTmplSize.x;
			coUpperLeft.y = pPRU->stLoadTmplCmd.coTmplCentre.y - pPRU->stLoadTmplCmd.szTmplSize.y;
			coLowerRight.x = pPRU->stLoadTmplCmd.coTmplCentre.x + pPRU->stLoadTmplCmd.szTmplSize.x;
			coLowerRight.y = pPRU->stLoadTmplCmd.coTmplCentre.y + pPRU->stLoadTmplCmd.szTmplSize.y;
			break;

		case FIDUCIAL_FITTING:
			HMI_bShowFiducial = TRUE;
			HMI_ulPRFMarkType = pPRU->stManualLrnDieCmd.emFiducialMarkType;
			coUpperLeft.x = pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].x;
			coUpperLeft.y = pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].y;
			coLowerRight.x = pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].x;
			coLowerRight.y = pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].y;
			switch (HMI_ulPRFMarkType)
			{
			case 0:
				g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\FMarkType1.png";
				SetHmiVariable("SF_szWizardBMPath");

				break;
			case 1:
				g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\FMarkType2.png";
				SetHmiVariable("SF_szWizardBMPath");

				break;
			case 2:
				g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\FMarkType3.png";
				SetHmiVariable("SF_szWizardBMPath");

				break;
			}
			break;
		}

		// 20140815 Yip: Add Manual Learn Die Ignore Region
		pstDetectIgnoreWin->uwNIgnoreWin = 0;

		HMI_bShowPRAlg	= TRUE;
		SetHmiVariable("PR_bShowPRAlg");
		SetHmiVariable("PR_bShowFiducial");

		PRS_CancelMouseWindow(pPRU);
		PRS_UseMouseWindow(pPRU, coUpperLeft, coLowerRight);
		PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
		
		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;
		break;

	case 1:
		PRS_GetMouseWindow(pPRU, &coUpperLeft, &coLowerRight);
		PRS_CancelMouseWindow(pPRU);

		arcoDieCorners[PR_UPPER_RIGHT].x	= coLowerRight.x;
		arcoDieCorners[PR_UPPER_RIGHT].y	= coUpperLeft.y;
		arcoDieCorners[PR_UPPER_LEFT].x	= coUpperLeft.x;
		arcoDieCorners[PR_UPPER_LEFT].y	= coUpperLeft.y;
		arcoDieCorners[PR_LOWER_LEFT].x	= coUpperLeft.x;
		arcoDieCorners[PR_LOWER_LEFT].y	= coLowerRight.y;
		arcoDieCorners[PR_LOWER_RIGHT].x	= coLowerRight.x;
		arcoDieCorners[PR_LOWER_RIGHT].y	= coLowerRight.y;

		szTmplSize.x = (coLowerRight.x - coUpperLeft.x) / 2;
		szTmplSize.y = (coLowerRight.y - coUpperLeft.y) / 2;
		coTmplCentre.x = (coLowerRight.x + coUpperLeft.x) / 2;
		coTmplCentre.y = (coLowerRight.y + coUpperLeft.y) / 2;

		if (HMI_ulPRAlg == RECTANGLE_MATCHING)
		{
			PRS_SetOnMouseClickCmd(ON);
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Upper Left Corner");
			
			PRS_DrawCross(pPRU->nCamera, g_stCursor.prCoord, g_stCursor.lSize, g_stCursor.lColor);

			g_szWizardStep = "Step 1/2";
			g_szWizardMess.Format("1. Locate Upper Left Corner");
			g_szWizardMessNext.Format("2. Locate Upper Right Corner");
			g_szWizardBMPath = "";

			HMI_bShowCursorControl			= TRUE;
			SetHmiVariable("PR_bShowCursorControl");


			g_bWizardEnableBack = FALSE;
			g_bWizardEnableNext = TRUE;
			g_bWizardEnableExit = TRUE;
			break;
		}
		//else if (HMI_ulPRAlg == FIDUCIAL_FITTING)
		//{
		//	pPRU->stManualLrnDieCmd.emIsAutoLearnFMarkCenterAngle = PR_TRUE;
		//	pPRU->stManualLrnDieCmd.emFiducialMarkType = (PR_FIDUCIAL_MARK_TYPE)HMI_ulPRFMarkType;

		//	for (INT i = PR_UPPER_RIGHT; i <= PR_LOWER_RIGHT; i++)
		//	{
		//		pPRU->stManualLrnDieCmd.acoDieCorners[i] = arcoDieCorners[i];
		//	}

		//	pPRU->stManualLrnDieCmd.emIsLrnCfmPattern = PR_FALSE;

		//	PRS_DrawCross(pPRU->nCamera, g_stCursor.prCoord, g_stCursor.lSize, g_stCursor.lColor);
		//	PRS_SetOnMouseClickCmd(ON);

		//	g_szWizardStep = "Step 2/3";
		//	g_szWizardMessPrev = "1. Align Glass Pattern centre with camera centre";
		//	g_szWizardMess = "2. Locate Corner1";
		//	g_szWizardMessNext = "3. Locate Corner2";	
		//	g_szWizardBMPath = "";
		//	
		//	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Please Set Pt 1");

		//	HMI_bShowCursorControl			= TRUE;
		//	HMI_bShowDigitalZoom			= TRUE;

		//	SetHmiVariable("PR_bShowCursorControl");
		//	SetHmiVariable("PR_bShowDigitalZoom");

		//	g_bWizardEnableBack = FALSE;
		//	g_bWizardEnableNext = TRUE;
		//	g_bWizardEnableExit = TRUE;
		//}
		else  // Other
		{
			LONG lResponse;

			switch (HMI_ulPRAlg)
			{
			case PATTERN_MATCHING:
			case FIDUCIAL_FITTING:
				pPRU->emRecordType = PRS_DIE_TYPE;
				pPRU->stSrchDieCmd.emAlignAlg = PRS_ALG_INTR;
				break;

			case EDGE_MATCHING:
				lResponse = HMISelectionBox("LEARN PR", "Please Select EDGE:matching algorithm:", "4-Edge", "Left-Edge", "Right-Edge");

				if (lResponse == -1 || lResponse == 11)
				{
					bResult = FALSE;
				}
				if (lResponse == 0)
				{
					pPRU->stManualLrnDieCmd.emAlignAlg = PRS_ALG_EDGE;
					pPRU->stSrchDieCmd.emAlignAlg = PRS_ALG_EDGE;
				}
				else if (lResponse == 1)
				{
					pPRU->stManualLrnDieCmd.emAlignAlg = PRS_ALG_L_EDGE;
					pPRU->stSrchDieCmd.emAlignAlg = PRS_ALG_L_EDGE;
				}
				else if (lResponse == 2)
				{
					pPRU->stManualLrnDieCmd.emAlignAlg = PRS_ALG_R_EDGE;
					pPRU->stSrchDieCmd.emAlignAlg = PRS_ALG_R_EDGE;
				}

				pPRU->emRecordType = PRS_DIE_TYPE;
				pPRU->stManualLrnDieCmd.emIsAutoLearnFMarkCenterAngle = PR_FALSE;
				pPRU->stManualLrnDieCmd.emIsLrnCfmPattern = PR_FALSE;
				pPRU->stManualLrnDieCmd.emObjectType = PR_OBJ_TYPE_IC_DIE;
				break;

			case SHAPE_FITTING:
				//lResponse = HMISelectionBox("LEARN PR", "Please select circle attribute:", "Bright", "Dark");

				//if (lResponse == -1 || lResponse == 11)
				//{
				//	bResult = FALSE;
				//}
				//if (lResponse == 0)
				//{
				//	pPRU->stLrnShapeCmd.emObjAttribute = PR_OBJ_ATTRIBUTE_BRIGHT;
				//}
				//else if (lResponse == 1)
				//{
				//	pPRU->stLrnShapeCmd.emObjAttribute = PR_OBJ_ATTRIBUTE_DARK;
				//}

				pPRU->stLrnShapeCmd.emShapeType				= PR_SHAPE_TYPE_CIRCLE;
				pPRU->stLrnShapeCmd.emDetectAlg				= PR_SHAPE_DETECT_ALG_EDGE_PTS;

				pPRU->stLrnShapeCmd.stEncRect.coCorner1.x = coUpperLeft.x;
				pPRU->stLrnShapeCmd.stEncRect.coCorner1.y = coUpperLeft.y;
				pPRU->stLrnShapeCmd.stEncRect.coCorner2.x = coLowerRight.x;
				pPRU->stLrnShapeCmd.stEncRect.coCorner2.y = coLowerRight.y;
				
				pPRU->emRecordType = PRS_SHAPE_TYPE;
				pPRU->stManualLrnDieCmd.emIsLrnCfmPattern = PR_FALSE;
				break;

			case TMPL_FITTING:
				pPRU->emRecordType = PRS_TMPL_TYPE;
				pPRU->stManualLrnDieCmd.emIsLrnCfmPattern = PR_FALSE;
				break;
			
			}
			switch (pPRU->emRecordType)
			{
			// Fiducial Fitting or Pattern Matching or Edge Matching
			case PRS_DIE_TYPE:
				if (HMI_ulPRAlg == EDGE_MATCHING)
				{
					for (INT i = PR_UPPER_RIGHT; i <= PR_LOWER_RIGHT; i++)
					{
						pPRU->stManualLrnDieCmd.acoDieCorners[i] = arcoDieCorners[i];
					}
					bResult = ManualLrnDie(pPRU);
				}
				else
				{
					PRS_FreeRecordID(pPRU); //20180208
					bResult = ManualLrnDie_New(pPRU);	// Vision UI
				}
				break;

			// Shape Fitting
			case PRS_SHAPE_TYPE:
				//bResult = LearnPRShape(pPRU);
				PRS_FreeRecordID(pPRU); //20180208
				bResult = ManualLrnDie_New(pPRU);	// Vision UI
				break;

			// Tmpl Fitting
			case PRS_TMPL_TYPE:
				pPRU->stLoadTmplCmd.szTmplSize = szTmplSize;
				pPRU->stLoadTmplCmd.coTmplCentre = coTmplCentre;
				bResult = LoadPRTmpl(pPRU);
				break;
			}

			if (bResult)
			{
				// Sucess help to set Pre-Reuqired for search
				pPRU->stSrchDieCmd.stDieAlignPar.rMatchScore	= (PR_REAL)85.0;
				pPRU->stSrchDieCmd.rCfmPatternMatchScore		= (PR_REAL)85.0; // Comfirmation Pattern 	
				pPRU->stDetectShapeCmd.rDetectScore				= (PR_REAL)85.0;
				pPRU->stSrchTmplCmd.ubPassScore					= (PR_UBYTE)85.0;
				
				pPRU->stSrchDieCmd.stDieAlignPar.rStartAngle	= -(PR_REAL)3.0;
				pPRU->stSrchDieCmd.stDieAlignPar.rEndAngle		= (PR_REAL)3.0;

				pPRU->stSrchDieCmd.stDieAlignPar.ubMaxScale	= (PR_UBYTE)105.0;
				pPRU->stSrchDieCmd.stDieAlignPar.ubMinScale	= (PR_UBYTE)95.0;
				pPRU->stDetectShapeCmd.rSizePercentVar		= (PR_REAL)5.0;

				g_szWizardStep = "2/2";
				g_szWizardMessPrev = "2. Learn PR";
				g_szWizardMess = "DONE";
				g_szWizardMessNext = "";
				g_szWizardBMPath = "";

				g_bWizardEnableBack = FALSE;
				g_bWizardEnableNext = FALSE;
				g_bWizardEnableExit = TRUE;
			}
		}
		break;

	case 2: //Get point 1
		if (HMI_ulPRAlg == RECTANGLE_MATCHING)
		{
			if (m_lCurrZoomFactor == PR_ZOOM_FACTOR_1X)
			{
				m_PRZoomedPRPt_a.x = g_stCursor.prCoord.x;
				m_PRZoomedPRPt_a.y = g_stCursor.prCoord.y;
			}
			else
			{
				PRS_GetPointFromZoomedImg(m_lCurrZoomFactor, ZoomCentre, g_stCursor.prCoord, &m_PRZoomedPRPt_a);
			}
			
			pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].x	= m_PRZoomedPRPt_a.x;
			pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].y	= m_PRZoomedPRPt_a.y;

			PRS_EraseCross(pPRU->nCamera, g_stCursor.prCoord, g_stCursor.lSize);
			PRS_DrawCross(pPRU->nCamera, g_stCursor.prCoord, g_stCursor.lSize, PR_COLOR_RED);
					
			g_szWizardStep = "2/2";
			g_szWizardMessPrev = "1. Locate Upper Left Corner";
			g_szWizardMess = "2. Locate Lower Right Corner";
			g_szWizardMessNext = "DONE";
			g_szWizardBMPath = "";
			
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Lower Right Corner");

			HMI_bShowCursorControl			= TRUE;
			SetHmiVariable("PR_bShowCursorControl");

			g_bWizardEnableBack = FALSE;
			g_bWizardEnableNext = TRUE;
			g_bWizardEnableExit = TRUE;
		}
		else
		{
			g_szWizardStep = "DONE";
			g_szWizardMess = "DONE";
			g_szWizardBMPath = "";

			g_bWizardEnableBack = FALSE;
			g_bWizardEnableNext = FALSE;
			g_bWizardEnableExit = TRUE;
		}
		break;

	case 3:
		if (HMI_ulPRAlg == RECTANGLE_MATCHING)
		{
			if (m_lCurrZoomFactor == PR_ZOOM_FACTOR_1X)
			{
				m_PRZoomedPRPt_a.x = g_stCursor.prCoord.x;
				m_PRZoomedPRPt_a.y = g_stCursor.prCoord.y;
			}
			else
			{
				PRS_GetPointFromZoomedImg(m_lCurrZoomFactor, ZoomCentre, g_stCursor.prCoord, &m_PRZoomedPRPt_a);
			}
			
			pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].x	= m_PRZoomedPRPt_a.x;
			pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].y	= m_PRZoomedPRPt_a.y;

			PRS_EraseCross(pPRU->nCamera, g_stCursor.prCoord, g_stCursor.lSize);
			PRS_DrawCross(pPRU->nCamera, g_stCursor.prCoord, g_stCursor.lSize, PR_COLOR_RED);
		

			pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_RIGHT].x		= pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].x;
			pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_RIGHT].y		= pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].y;
			pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].x		= pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].x;
			pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].y		= pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].y;
			pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_LEFT].x		= pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].x;
			pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_LEFT].y		= pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].y;
			pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].x		= pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].x;
			pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].y		= pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].y;

			pPRU->emRecordType = PRS_DIE_TYPE;
			pPRU->stManualLrnDieCmd.emAlignAlg = PR_EDGE_POINTS_MATCHING_ON_DIE_EDGES_WO_PATTERN_GUIDED;
			pPRU->stSrchDieCmd.emAlignAlg = PR_EDGE_POINTS_MATCHING_ON_DIE_EDGES_WO_PATTERN_GUIDED;
			pPRU->stManualLrnDieCmd.emIsAutoLearnFMarkCenterAngle = PR_FALSE;
			pPRU->stManualLrnDieCmd.emIsLrnCfmPattern = PR_FALSE;
			pPRU->stManualLrnDieCmd.emObjectType = PR_OBJ_TYPE_IC_DIE;

			g_szWizardStep = "3/4";
			g_szWizardMessPrev = "2. Locate Lower Right Corner";
			g_szWizardMess = "DONE";
			g_szWizardMessNext = "DONE";
			g_szWizardBMPath = "";
			
			bResult = ManualLrnDie(pPRU);
		}

		if (bResult)
		{
			PRS_SetOnMouseClickCmd(OFF);

			// Sucess help to set Pre-Reuqired for search
			pPRU->stSrchDieCmd.stDieAlignPar.rMatchScore	= (PR_REAL)85.0;
			pPRU->stSrchDieCmd.rCfmPatternMatchScore		= (PR_REAL)85.0; // Comfirmation Pattern 	
			pPRU->stDetectShapeCmd.rDetectScore				= (PR_REAL)85.0;
			pPRU->stSrchTmplCmd.ubPassScore					= (PR_UBYTE)85.0;
			
			pPRU->stSrchDieCmd.stDieAlignPar.rStartAngle	= -(PR_REAL)3.0;
			pPRU->stSrchDieCmd.stDieAlignPar.rEndAngle		= (PR_REAL)3.0;
			pPRU->stSrchDieCmd.stDieAlignPar.ubMaxScale	= (PR_UBYTE)105.0;
			pPRU->stSrchDieCmd.stDieAlignPar.ubMinScale	= (PR_UBYTE)95.0;
			pPRU->stDetectShapeCmd.rSizePercentVar		= (PR_REAL)5.0;

			// 20140815 Yip: Add Manual Learn Die Ignore Region
			if (rMSG_YES != HMIMessageBox(MSG_YES_NO, "Learn PR", "Do all lines indicate edges?\nYes - Done\nNo - Add Ignore Region"))
			{
				coUpperLeft = pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT];
				coLowerRight = pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT];
				for (int i = 0; i < PR_MAX_DETECT_IGNORE_WIN; ++i)
				{
					pstDetectIgnoreWin->astIgnoreWin[i].coCorner1 = coUpperLeft;
					pstDetectIgnoreWin->astIgnoreWin[i].coCorner2 = coLowerRight;
				}
				pstDetectIgnoreWin->uwNIgnoreWin = 0;

				PRS_ClearScreen(pPRU->nCamera);
				PRS_CancelMouseWindow(pPRU);
				PRS_UseMouseWindow(pPRU, coUpperLeft, coLowerRight);
				PRS_DrawHomeCursor(pPRU->nCamera, FALSE);

				g_szWizardStep = "4/4";
				g_szWizardMessPrev = "3. Locate Corner2";
				g_szWizardMess.Format("4.1. Add Ignore Region %d", pstDetectIgnoreWin->uwNIgnoreWin + 1);
				g_szWizardMessNext = "4.2. Adjust Upper Left Corner";
				g_szWizardBMPath = "";

				g_bWizardEnableBack = FALSE;
				g_bWizardEnableNext = TRUE;
				g_bWizardEnableExit = TRUE;
			}
			else
			{
				g_szWizardStep = "4/4";
				g_szWizardMessPrev = "3. Locate Corner2";
				g_szWizardMess = "DONE";
				g_szWizardMessNext = "";
				g_szWizardBMPath = "";

				g_bWizardEnableBack = FALSE;
				g_bWizardEnableNext = FALSE;
				g_bWizardEnableExit = TRUE;
			}
		}
		break;

	case 4:	// 20140815 Yip: Add Manual Learn Die Ignore Region
		PRS_GetMouseWindow(pPRU, &coUpperLeft, &coLowerRight);
		PRS_CancelMouseWindow(pPRU);

		pstDetectIgnoreWin->astIgnoreWin[pstDetectIgnoreWin->uwNIgnoreWin].coCorner1 = coUpperLeft;
		pstDetectIgnoreWin->astIgnoreWin[pstDetectIgnoreWin->uwNIgnoreWin].coCorner2 = coLowerRight;

		m_lCurrZoomFactor = PR_ZOOM_FACTOR_2X;

		ZoomCentre = pstDetectIgnoreWin->astIgnoreWin[pstDetectIgnoreWin->uwNIgnoreWin].coCorner1;
		g_stCursor.prCoord.x = PR_DEF_CENTRE_X;
		g_stCursor.prCoord.y = PR_DEF_CENTRE_Y;
		PRS_DigitalZoom(nCurrentCameraID, m_lCurrZoomFactor, ZoomCentre);
		PRS_DrawCross(nCurrentCameraID, g_stCursor.prCoord, g_stCursor.lSize, g_stCursor.lColor);
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Adjust Upper Left Corner");

		HMI_bShowCursorControl = TRUE;
		HMI_bShowDigitalZoom = TRUE;

		SetHmiVariable("PR_bShowCursorControl");
		SetHmiVariable("PR_bShowDigitalZoom");

		PRS_SetOnMouseClickCmd(ON);

		g_szWizardStep = "4/4";
		g_szWizardMessPrev.Format("4.1. Add Ignore Region %d", pstDetectIgnoreWin->uwNIgnoreWin + 1);
		g_szWizardMess = "4.2. Adjust Upper Left Corner";
		g_szWizardMessNext = "4.3. Adjust Lower Right Corner";
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;

		break;

	case 5:	// 20140815 Yip: Add Manual Learn Die Ignore Region
		if (m_lCurrZoomFactor == PR_ZOOM_FACTOR_1X)
		{
			coUpperLeft = g_stCursor.prCoord;
		}
		else
		{
			PRS_GetPointFromZoomedImg(m_lCurrZoomFactor, ZoomCentre, g_stCursor.prCoord, &coUpperLeft);
		}
		pstDetectIgnoreWin->astIgnoreWin[pstDetectIgnoreWin->uwNIgnoreWin].coCorner1 = coUpperLeft;

		PRS_DisableDigitalZoom(pPRU->nCamera);

		ZoomCentre = pstDetectIgnoreWin->astIgnoreWin[pstDetectIgnoreWin->uwNIgnoreWin].coCorner2;
		g_stCursor.prCoord.x = PR_DEF_CENTRE_X;
		g_stCursor.prCoord.y = PR_DEF_CENTRE_Y;
		PRS_DigitalZoom(nCurrentCameraID, m_lCurrZoomFactor, ZoomCentre);
		PRS_DrawCross(nCurrentCameraID, g_stCursor.prCoord, g_stCursor.lSize, g_stCursor.lColor);
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Adjust Lower Right Corner");

		HMI_bShowCursorControl = TRUE;
		HMI_bShowDigitalZoom = TRUE;

		SetHmiVariable("PR_bShowCursorControl");
		SetHmiVariable("PR_bShowDigitalZoom");

		PRS_SetOnMouseClickCmd(ON);

		g_szWizardStep = "4/4";
		g_szWizardMessPrev = "4.2. Adjust Upper Left Corner";
		g_szWizardMess = "4.3. Adjust Lower Right Corner";
		g_szWizardMessNext = "DONE";
		g_szWizardBMPath = "";

		g_bWizardEnableBack = FALSE;
		g_bWizardEnableNext = TRUE;
		g_bWizardEnableExit = TRUE;
		break;

	case 6:	// 20140815 Yip: Add Manual Learn Die Ignore Region
		if (m_lCurrZoomFactor == PR_ZOOM_FACTOR_1X)
		{
			coLowerRight = g_stCursor.prCoord;
		}
		else
		{
			PRS_GetPointFromZoomedImg(m_lCurrZoomFactor, ZoomCentre, g_stCursor.prCoord, &coLowerRight);
			m_lCurrZoomFactor = PR_ZOOM_FACTOR_1X;
			PRS_DisableDigitalZoom(pPRU->nCamera);
		}
		pstDetectIgnoreWin->astIgnoreWin[pstDetectIgnoreWin->uwNIgnoreWin].coCorner2 = coLowerRight;

		PRS_SetOnMouseClickCmd(OFF);
		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);

		HMI_bShowCursorControl	= FALSE;
		HMI_bShowDigitalZoom	= FALSE;

		SetHmiVariable("PR_bShowCursorControl");
		SetHmiVariable("PR_bShowDigitalZoom");

		++pstDetectIgnoreWin->uwNIgnoreWin;

		for (int i = 0; i < pstDetectIgnoreWin->uwNIgnoreWin; ++i)
		{
			PRS_DrawRect(pPRU->nCamera, pstDetectIgnoreWin->astIgnoreWin[i].coCorner1, pstDetectIgnoreWin->astIgnoreWin[i].coCorner2, (PR_COLOR)(i + 1));
		}

		if (
			(pstDetectIgnoreWin->uwNIgnoreWin < PR_MAX_DETECT_IGNORE_WIN) &&
			(rMSG_YES == HMIMessageBox(MSG_YES_NO, "Learn PR", "Add another ignore region?"))
		   )
		{
			coUpperLeft = pstDetectIgnoreWin->astIgnoreWin[pstDetectIgnoreWin->uwNIgnoreWin].coCorner1;
			coLowerRight = pstDetectIgnoreWin->astIgnoreWin[pstDetectIgnoreWin->uwNIgnoreWin].coCorner2;

			PRS_CancelMouseWindow(pPRU);
			PRS_UseMouseWindow(pPRU, coUpperLeft, coLowerRight);
			PRS_DrawHomeCursor(pPRU->nCamera, FALSE);

			g_szWizardStep = "4/4";
			g_szWizardMessPrev = "3. Locate Corner2";
			g_szWizardMess.Format("4.1. Add Ignore Region %d", pstDetectIgnoreWin->uwNIgnoreWin + 1);
			g_szWizardMessNext = "4.2. Adjust Upper Left Corner";
			g_szWizardBMPath = "";

			g_bWizardEnableBack = FALSE;
			g_bWizardEnableNext = TRUE;
			g_bWizardEnableExit = TRUE;
		}
		else
		{
			bResult = ManualLrnDie(pPRU);

			if (bResult)
			{
				if (rMSG_YES != HMIMessageBox(MSG_YES_NO, "Learn PR", "Do all lines indicate edges?\nYes - Done\nNo - Modify Ignore Region"))
				{
					pstDetectIgnoreWin->uwNIgnoreWin = 0;
					coUpperLeft = pstDetectIgnoreWin->astIgnoreWin[pstDetectIgnoreWin->uwNIgnoreWin].coCorner1;
					coLowerRight = pstDetectIgnoreWin->astIgnoreWin[pstDetectIgnoreWin->uwNIgnoreWin].coCorner2;

					PRS_ClearScreen(pPRU->nCamera);
					PRS_CancelMouseWindow(pPRU);
					PRS_UseMouseWindow(pPRU, coUpperLeft, coLowerRight);
					PRS_DrawHomeCursor(pPRU->nCamera, FALSE);

					g_szWizardStep = "4/4";
					g_szWizardMessPrev = "3. Locate Corner2";
					g_szWizardMess.Format("4.1. Add Ignore Region %d", pstDetectIgnoreWin->uwNIgnoreWin + 1);
					g_szWizardMessNext = "4.2. Adjust Upper Left Corner";
					g_szWizardBMPath = "";

					g_bWizardEnableBack = FALSE;
					g_bWizardEnableNext = TRUE;
					g_bWizardEnableExit = TRUE;
				}
				else
				{
					g_szWizardStep = "4/4";
					g_szWizardMessPrev = "3. Locate Corner2";
					g_szWizardMess = "DONE";
					g_szWizardMessNext = "";
					g_szWizardBMPath = "";

					g_bWizardEnableBack = FALSE;
					g_bWizardEnableNext = FALSE;
					g_bWizardEnableExit = TRUE;
				}
			}
		}
		g_ulWizardStep = 3;
		break;
	}

	if (!bResult)
	{
		PRS_CancelMouseWindow(pPRU);
		PRS_SetOnMouseClickCmd(OFF);
		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);

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

/////////////////////////////////////////////////////////////////
//Setup Related
/////////////////////////////////////////////////////////////////
VOID CWinEagle::SetupWizard()
{
	CSettingFile *pCSettingFile = (CSettingFile*)m_pModule->GetStation("SettingFile");
	PRU *pPRU;

	switch (g_ulWizardSubTitle)
	{
	case 0:
		LogAction("WinEagle: Camera Calibration Learn PR pattern");
		pPRU = &SelectCurPRU(HMI_ulCurPRU);
		SetupPRPatternPreTask();
		SetupPRPattern(pPRU);
		SetupPRPatternPostTask();
		break;

	case 1:
		LogAction("WinEagle: Camera Calibration PR Search Area");
		pPRU = &SelectCurPRU(HMI_ulCurPRU);
		SetupPRSrchArea(pPRU);
		break;

	}
}

VOID CWinEagle::SetupPRPatternPreTask()
{
	//Do Every thing before each Step 
}


VOID CWinEagle::SetupPRPatternPostTask()
{
	//Do Every thing after each Step
	switch (g_ulWizardStep)
	{
	case 99:
		UpdateHMICalibPRButton(FALSE);
		break;
	}
}

VOID CWinEagle::ModulePRSetupPreTask()
{
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH	= (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CInspOpt *pCInspOpt	= (CInspOpt*)m_pModule->GetStation("InspOpt");
		
	// Release me
	//pCTrayHolder->SetDiagnSteps(g_lDiagnSteps);
	//pCTA1->SetDiagnSteps(g_lDiagnSteps);
	//pCINWH->SetDiagnSteps(g_lDiagnSteps);
	//pCPB1->SetDiagnSteps(g_lDiagnSteps);
	//pCPB2->SetDiagnSteps(g_lDiagnSteps);
	//pCPreBondWH->SetDiagnSteps(g_lDiagnSteps);
	//pCShuttle->SetDiagnSteps(g_lDiagnSteps);

	pCACF1WH->m_bCheckIO = TRUE; //20130318
	pCACF2WH->m_bCheckIO = TRUE;
	pCInspOpt->m_bCheckIO = TRUE;

	//Protection for PreBond
	//switch (HMI_ulCurPRU)
	//{
	////case ACF1WH_CALIB_PRU:
	////	pCACF1WH->CheckModSelected(TRUE);
	////	pCInspOpt->CheckModSelected(TRUE);

	////	if (pCACF1WH->m_bModSelected && pCInspOpt->m_bModSelected)
	////	{
	////		m_bMoveToPosn = FALSE; //TRUE;
	////	}
	////	break;

	//}

	m_bMoveToPosn = FALSE; //20130405
	//if (m_bMoveToPosn)
	//{
	//	SelectCalibPRU(HMI_ulCurPRU);
	//}

	UpdateHMICalibPRButton(HMI_ulCurPRU);

}

// Add me later
VOID CWinEagle::ModulePRSetupPostTask()
{
	CACF1WH *pCACF1WH	= (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH	= (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CInspOpt *pCInspOpt	= (CInspOpt*)m_pModule->GetStation("InspOpt");

	pCACF1WH->m_bCheckIO = TRUE; //20130318
	pCACF2WH->m_bCheckIO = TRUE;
	pCInspOpt->m_bCheckIO = TRUE;

	switch (HMI_ulCurPRU)
	{
	case ACF1WH_CALIB_PRU:
		//pCINWH->MoveTToStandby(GMP_WAIT);
		//pCINWH->MoveY(INWH_Y_CTRL_GO_STANDBY_POSN, GMP_WAIT);
		//pCTA1->MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT);
		break;
	}

	//pCPB1->MoveZ(PREBOND_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT); //20140702
	//pCPB2->MoveZ(PREBOND_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT);

}

LONG CWinEagle::SwitchAllCam()	// 20140623 Yip
{
//	LONG lLoopCnt = 0;
//	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");
//
//	PRU *pPRU = NULL;
//	DisplayMessage(CString(__FUNCTION__) + CString(" Begin..."));
//
//	CPB1 *pCPB1 = (CPB1*)m_pModule->GetStation("PB1");
//	CPB2 *pCPB2 = (CPB2*)m_pModule->GetStation("PB2");
//	CInsp1 *pCInsp1 = (CInsp1*)m_pModule->GetStation("Insp1");
//	CInsp2 *pCInsp2 = (CInsp2*)m_pModule->GetStation("Insp2");
//
////Prebond UL 1
//	if (pCPB1 != NULL)
//	{
//		pPRU = &pruPBLSI1[PREBOND_1][LSI_TYPE1];
//		if (pPRU->bLoaded)
//		{
////			PRS_ProcImgSimpleCmd(pPRU);	// 20140709 Yip: Pure Grab Instead Of Search Image When Machine Startup
//			pCPB1->MachineStartup_SrchPR(pPRU);
//		}
//		else
//		{
//			DisplayMessage("PB1 PR1 Type1 PR not loaded");
//		}
//
//		pPRU = &pruPBLSI2[PREBOND_1][LSI_TYPE1];
//		if (pPRU->bLoaded)
//		{
////			PRS_ProcImgSimpleCmd(pPRU);	// 20140709 Yip: Pure Grab Instead Of Search Image When Machine Startup
//			pCPB1->MachineStartup_SrchPR(pPRU);
//		}
//		else
//		{
//			DisplayMessage("PB1 PR2 Type1 PR not loaded");
//		}
//	}
//	else
//	{
//		DisplayMessage("Prebond_1 pointer not ready error");
//	}
////Prebond UL 2
//	if (pCPB2 != NULL)
//	{
//		pPRU = &pruPBLSI1[PREBOND_2][LSI_TYPE1];
//		if (pPRU->bLoaded)
//		{
////			PRS_ProcImgSimpleCmd(pPRU);	// 20140709 Yip: Pure Grab Instead Of Search Image When Machine Startup
//			pCPB2->MachineStartup_SrchPR(pPRU);
//		}
//		else
//		{
//			DisplayMessage("PB2 PR1 Type1 PR not loaded");
//		}
//
//		pPRU = &pruPBLSI2[PREBOND_2][LSI_TYPE1];
//		if (pPRU->bLoaded)
//		{
////			PRS_ProcImgSimpleCmd(pPRU);	// 20140709 Yip: Pure Grab Instead Of Search Image When Machine Startup
//			pCPB2->MachineStartup_SrchPR(pPRU);
//		}
//		else
//		{
//			DisplayMessage("PB2 PR2 Type1 PR not loaded");
//		}
//	}
//	else
//	{
//		DisplayMessage("Prebond_2 pointer not ready error");
//	}
////Insp1 ACF Post Bond	// 20140707 Yip
//	if (pCInsp1 != NULL)
//	{
//		pPRU = &pruACFPB1[INSP_1][ACF1][OBJECT];
//		if (pPRU->bLoaded)
//		{
////			PRS_ProcImgSimpleCmd(pPRU);	// 20140709 Yip: Pure Grab Instead Of Search Image When Machine Startup
//			pCInsp1->MachineStartup_SrchPR(pPRU);
//		}
//		else
//		{
//			DisplayMessage("Insp1 PR1 Type1 PR not loaded");
//		}
//
//		pPRU = &pruACFPB2[INSP_1][ACF1][OBJECT];
//		if (pPRU->bLoaded)
//		{
////			PRS_ProcImgSimpleCmd(pPRU);	// 20140709 Yip: Pure Grab Instead Of Search Image When Machine Startup
//			pCInsp1->MachineStartup_SrchPR(pPRU);
//		}
//		else
//		{
//			DisplayMessage("Insp1 PR2 Type1 PR not loaded");
//		}
//	}
//	else
//	{
//		DisplayMessage("Insp1 pointer not ready error");
//	}
////Insp2 ACF Post Bond	// 20140707 Yip
//	if (pCInsp2 != NULL)
//	{
//		pPRU = &pruACFPB1[INSP_2][ACF1][OBJECT];
//		if (pPRU->bLoaded)
//		{
////			PRS_ProcImgSimpleCmd(pPRU);	// 20140709 Yip: Pure Grab Instead Of Search Image When Machine Startup
//			pCInsp2->MachineStartup_SrchPR(pPRU);
//		}
//		else
//		{
//			DisplayMessage("Insp2 PR1 Type1 PR not loaded");
//		}
//
//		pPRU = &pruACFPB2[INSP_2][ACF1][OBJECT];
//		if (pPRU->bLoaded)
//		{
////			PRS_ProcImgSimpleCmd(pPRU);	// 20140709 Yip: Pure Grab Instead Of Search Image When Machine Startup
//			pCInsp2->MachineStartup_SrchPR(pPRU);
//		}
//		else
//		{
//			DisplayMessage("Insp2 PR2 Type1 PR not loaded");
//		}
//	}
//	else
//	{
//		DisplayMessage("Insp2 pointer not ready error");
//	}
//	PRS_ClearScreen(PBUL1_CAM);
//	PRS_ClearScreen(PBUL2_CAM);
//	PRS_ClearScreen(INSP1_CAM);
//	PRS_ClearScreen(INSP2_CAM);

	DisplayMessage(CString(__FUNCTION__) + CString(" Finished."));
	return 0;
}

//-----------------------------------------------------------------------------
// HMI command implementation
//-----------------------------------------------------------------------------
//

LONG CWinEagle::HMISwitchCam(IPC_CServiceMessage &svMsg)
{
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");

	PRU *pPRU = NULL;
	PR_COORD coCentre;
	coCentre.x = PR_DEF_CENTRE_X;
	coCentre.y = PR_DEF_CENTRE_Y;

	if (!m_lAutoBondMode || State() != AUTO_Q)
	{
		pPRU = &PRS_SetCameraPRU(nCamera);

		if (g_bEnableTrigLighting)
		{
			PRS_EnableHWTrigLighting(pPRU->nCamera, FALSE);
		}

		if (nCamera >= MAX_CAM)
		{
			nCamera = ACF_CAM;
		}
		else
		{
			nCamera++;
		}

		pPRU = &PRS_SetCameraPRU(nCamera);

		if (g_bEnableTrigLighting)
		{
			PRS_EnableHWTrigLighting(pPRU->nCamera, TRUE);
		}

		PRS_DisplayVideo(pPRU);

		if (nCamera != NO_CAM)
		{
			PRS_DrawHomeCursor(nCurrentCameraID, FALSE);
		}

		bPRUpdateLight = TRUE;

		// updated lighting
		UpdateLighting(pPRU);
	}
	return 0;
}
//{
//	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pModule->GetStation("HouseKeeping");
//
//	PRU *pPRU = NULL;
//	PR_COORD coCentre;
//	coCentre.x = PR_DEF_CENTRE_X;
//	coCentre.y = PR_DEF_CENTRE_Y;
//
//	pPRU = &PRS_SetCameraPRU(nCamera);
//
//	if (!g_bEnableTrigLighting)
//	{
//		PRS_EnableHWTrigLighting(pPRU->nCamera, FALSE);
//	}
//	LONG lResponse = -1;
//
//	do
//	{
//		lResponse = HMISelectionBox("LEARN PR", "Please Select Camera", "INWH", "INSP1", "INSP2", "PBDL1", "PBDL2", "PBUL1", "PBUL2", "SHUTTLE1", "SHUTTLE2", "More...");
//		if (lResponse == -1 || lResponse == 11)
//		{
//			bPRUpdateLight = TRUE;
//			// updated lighting
//			UpdateLighting(pPRU);
//			return 0;
//		}
//		else if (lResponse == 9) //More...
//		{
//			lResponse = HMISelectionBox("LEARN PR", "Please Select Camera", "COF TRAY", "TA4", "MBDL", "ACFUL", "BACK...");
//			if (lResponse == -1 || lResponse == 11)
//			{
//				bPRUpdateLight = TRUE;
//				// updated lighting
//				UpdateLighting(pPRU);
//				return 0;
//			}
//			lResponse += 9;
//		}
//	}
//	while (lResponse < FIRST_CAM || lResponse > MAX_CAM);
//
//	switch (lResponse)
//	{
//	case ACF_CAM:
//		nCamera = ACF_CAM;
//		break;
//	case ACFUL_CAM:
//		nCamera = ACFUL_CAM;
//		break;
//	default:
//		nCamera = ACF_CAM;
//		break;
//	}
//
//	if (nCamera >= MAX_CAM)
//	{
//		nCamera = ACF_CAM;
//	}
//	else
//	{
//		nCamera++;
//	}
//
//	pPRU = &PRS_SetCameraPRU(nCamera);
//
//	if (g_bEnableTrigLighting)
//	{
//		PRS_EnableHWTrigLighting(pPRU->nCamera, TRUE);
//	}
//
//	PRS_DisplayVideo(pPRU);
//	
//	if (nCamera != NO_CAM)
//	{
//		PRS_DrawHomeCursor(nCurrentCameraID, FALSE);
//	}
//
//	bPRUpdateLight = TRUE;
//
//	// updated lighting
//	UpdateLighting(pPRU);
//	return 0;
//}

LONG CWinEagle::HMI_SelectCurStation(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	// Remarks for the HMI Display PRStaion seq must equal to the add m_PRStations at AC9000.h
	LONG	lMsgLen = svMsg.GetMsgLen();
	unsigned char cArray[10] = " ";
	UCHAR	*cName = &cArray[0];
	CString temp;

	if (lMsgLen > 0)
	{
		cName = new UCHAR[lMsgLen];
		if (cName == NULL) //klocwork fix 20121211
		{
			return 0;
		}
		svMsg.GetMsg(cName, lMsgLen);
	}

	HMI_szCurPRStation = cName;
	HMI_lCurRecord	= 0;

	//To find the record
	LONG lTemp = 0;
	CString szCurStaionName;
	szCurStaionName = HMI_szCurPRStation;

	
	for (INT i = 0; i < GetmyPRUList().GetSize(); i++)
	{
		if (GetmyPRUList().GetAt(i)->szStnName == szCurStaionName)
		{
			if (lTemp == HMI_lCurRecord)
			{
				HMI_ulCurPRU = GetmyPRUList().GetAt(i)->emID;
				break;
			}
			lTemp++;
		}
	}

	//HMI_ulCurPRU = 0;

	UpdateHMIDiagnPRScreen(FALSE);
	if (cName != NULL) //klocwork fix 20121211
	{
		delete[] cName;
	}
		//delete cName;

	return 0;
}

LONG CWinEagle::HMI_SelectCurRecord(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	LONG lRecord;
	svMsg.GetMsg(sizeof(LONG), &lRecord);

	HMI_lCurRecord = lRecord;

	//To find the record
	LONG lTemp = 0;
	CString szCurStaionName;
	szCurStaionName = HMI_szCurPRStation;

	for (INT i = 0; i < GetmyPRUList().GetSize(); i++)
	{
		if (GetmyPRUList().GetAt(i)->szStnName == szCurStaionName)
		{
			if (lTemp == lRecord)
			{
				HMI_ulCurPRU = GetmyPRUList().GetAt(i)->emID;
				break;
			}
			lTemp++;
		}
	}

	//HMI_ulCurPRU = 0;

	UpdateHMIDiagnPRScreen(FALSE);
	return 0;
}

LONG CWinEagle::HMIUpdateDiagnPRScreen(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	// Reset all Data
	//HMI_szCurPRStation = "TrayHolder";
	HMI_lCurRecord	= 0;

	//To find the record
	LONG lTemp = 0;
	CString szCurStaionName;
	szCurStaionName = HMI_szCurPRStation;

	
	for (INT i = 0; i < GetmyPRUList().GetSize(); i++)
	{
		if (GetmyPRUList().GetAt(i)->szStnName == szCurStaionName)
		{
			if (lTemp == HMI_lCurRecord)
			{
				HMI_ulCurPRU = GetmyPRUList().GetAt(i)->emID;
				break;
			}
			lTemp++;
		}
	}

	UpdateHMIDiagnPRScreen(FALSE);
	return 0;
}

LONG CWinEagle::HMIUpdateServicePRScreen(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	// Reset all Data
	//HMI_szCurPRStation = "TrayHolder";
	HMI_lCurRecord	= 0;

	//To find the record
	LONG lTemp = 0;
	CString szCurStaionName;
	szCurStaionName = HMI_szCurPRStation;

	
	for (INT i = 0; i < GetmyPRUList().GetSize(); i++)
	{
		if (GetmyPRUList().GetAt(i)->szStnName == szCurStaionName)
		{
			if (lTemp == HMI_lCurRecord)
			{
				HMI_ulCurPRU = GetmyPRUList().GetAt(i)->emID;
				break;
			}
			lTemp++;
		}
	}

	UpdateHMIDiagnPRScreen(FALSE);
	return 0;
}


LONG CWinEagle::HMIClearPRRecord(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	LONG lAnswer = rMSG_TIMEOUT;

	lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Operation is irreversible. Continue?");
	if (lAnswer == rMSG_TIMEOUT)
	{
		HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
	}
	
	else if (lAnswer == rMSG_CONTINUE)
	{
		pPRU	= GetmyPRUList().GetAt(HMI_ulCurPRU);
	
		if (PRS_FreeRecordID(pPRU))
		{
			WriteHMIMess("Free PR Record ID Completed");
		}
		else
		{
			WriteHMIMess("Free PR Record ID Failed!");
		}
	}
	else
	{
		WriteHMIMess("Operation Cancelled!");
	}

	UpdateHMIDiagnPRScreen(FALSE);
	return 0;
}

LONG CWinEagle::HMIStartRepeatTest(IPC_CServiceMessage &svMsg)
{

	PRU *pPRU;
	CString szMsg = _T("");
	BOOL	bSrch = FALSE;

	pPRU	= GetmyPRUList().GetAt(HMI_ulCurPRU);
	
	StartRepeatTest(pPRU);

	return 0;
	
}

LONG CWinEagle::HMISetTestCounter(IPC_CServiceMessage &svMsg)
{
	ULONG ulCounter;
	svMsg.GetMsg(sizeof(ULONG), &ulCounter);

	ulTestCounter = ulCounter;
	return 0;
}

LONG CWinEagle::HMISetTestDelay(IPC_CServiceMessage &svMsg)
{
	ULONG ulDelay;
	svMsg.GetMsg(sizeof(ULONG), &ulDelay);

	ulTestDelay = ulDelay;
	return 0;
}


LONG CWinEagle::HMIIncrExposureTime(IPC_CServiceMessage &svMsg)
{
	//CInspWH *pCInspWH = (CInspWH*)m_pModule->GetStation("InspWH");
	//CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");

	INT		i = 0;
	ULONG	ulExposureTime;
	PRU	*pPRU;

	pPRU	= GetmyPRUList().GetAt(HMI_ulCurPRU);

	ulExposureTime = (ULONG)PRS_GetExposureTime(pPRU);
	
	ulExposureTime++;
	if (ulExposureTime > 10)
	{
		ulExposureTime = 10;
	}

	ulShowExposureTime = ulExposureTime;

	PRS_SetExposureTime(pPRU, (INT)ulExposureTime);

	//if(pPRU == &pruINSP_DL_Calib || pPRU == &pruDLGlass1 || pPRU == &pruDLGlass2)
	//{
	//	pCInspWH->m_lINSP_DL_ExposureTimeValue = (LONG)ulExposureTime;
	//}
	//else
	//{
	//	pCInspOpt->m_lINSP_UL_ExposureTimeValue = (LONG)ulExposureTime;
	//}

	return 0;
}

LONG CWinEagle::HMIDecrExposureTime(IPC_CServiceMessage &svMsg)
{
	//CInspWH *pCInspWH = (CInspWH*)m_pModule->GetStation("InspWH");
	//CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");

	INT		i = 0;
	ULONG	ulExposureTime;
	PRU	*pPRU;
	
	pPRU	= GetmyPRUList().GetAt(HMI_ulCurPRU);

	ulExposureTime = (ULONG)PRS_GetExposureTime(pPRU);

	if (ulExposureTime > 0)
	{
		ulExposureTime--;
	}
	ulShowExposureTime = ulExposureTime;

	PRS_SetExposureTime(pPRU, (INT)ulExposureTime);

	//if(pPRU == &pruINSP_DL_Calib || pPRU == &pruDLGlass1 || pPRU == &pruDLGlass2)
	//{
	//	pCInspWH->m_lINSP_DL_ExposureTimeValue = (LONG)ulExposureTime;
	//}
	//else
	//{
	//	pCInspOpt->m_lINSP_UL_ExposureTimeValue = (LONG)ulExposureTime;
	//}

	return 0;
}

LONG CWinEagle::HMIIncrCoaxialLevel(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT);
	
	ulLightLevel++;
	if (ulLightLevel > 100)
	{
		ulLightLevel = 100;
	}

	PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrCoaxialLevel(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT);

	if (ulLightLevel > 0)
	{
		ulLightLevel--;
	}

	PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIIncrCoaxialLevel10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT);
	
	ulLightLevel += 10;
	if (ulLightLevel > 100)
	{
		ulLightLevel = 100;
	}

	PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrCoaxialLevel10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT);

	if (ulLightLevel < 10)
	{
		ulLightLevel = 0;
	}
	else
	{
		ulLightLevel -= 10;
	}

	PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIIncrCoaxial1Level(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT_1);
	
	ulLightLevel++;
	if (ulLightLevel > 100)
	{
		ulLightLevel = 100;
	}

	PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT_1, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrCoaxial1Level(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT_1);

	if (ulLightLevel > 0)
	{
		ulLightLevel--;
	}

	PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT_1, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIIncrCoaxial1Level10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT_1);
	
	ulLightLevel += 10;
	if (ulLightLevel > 100)
	{
		ulLightLevel = 100;
	}

	PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT_1, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrCoaxial1Level10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT_1);

	if (ulLightLevel < 10)
	{
		ulLightLevel = 0;
	}
	else
	{
		ulLightLevel -= 10;
	}

	PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT_1, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIIncrCoaxial2Level(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT_2);
	
	ulLightLevel++;
	if (ulLightLevel > 100)
	{
		ulLightLevel = 100;
	}

	PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT_2, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrCoaxial2Level(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT_2);

	if (ulLightLevel > 0)
	{
		ulLightLevel--;
	}

	PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT_2, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIIncrCoaxial2Level10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT_2);
	
	ulLightLevel += 10;
	if (ulLightLevel > 100)
	{
		ulLightLevel = 100;
	}

	PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT_2, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrCoaxial2Level10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_COAXIAL_LIGHT_2);

	if (ulLightLevel < 10)
	{
		ulLightLevel = 0;
	}
	else
	{
		ulLightLevel -= 10;
	}

	PRS_SetLighting(pPRU, PR_COAXIAL_LIGHT_2, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIIncrRingLevel(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_RING_LIGHT);
	

	ulLightLevel++;
	if (ulLightLevel > 100)
	{
		ulLightLevel = 100;
	}

	PRS_SetLighting(pPRU, PR_RING_LIGHT, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrRingLevel(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_RING_LIGHT);

	if (ulLightLevel > 0)
	{
		ulLightLevel--;
	}

	PRS_SetLighting(pPRU, PR_RING_LIGHT, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIIncrRingLevel10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_RING_LIGHT);
	
	ulLightLevel += 10;
	if (ulLightLevel > 100)
	{
		ulLightLevel = 100;
	}

	PRS_SetLighting(pPRU, PR_RING_LIGHT, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrRingLevel10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_RING_LIGHT);

	if (ulLightLevel < 10)
	{
		ulLightLevel = 0;
	}
	else
	{
		ulLightLevel -= 10;
	}

	PRS_SetLighting(pPRU, PR_RING_LIGHT, (PR_UWORD)ulLightLevel);

	return 0;
}

LONG CWinEagle::HMIIncrRing1Level(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_RING_LIGHT_1);
	
	ulLightLevel++;
	if (ulLightLevel > 100)
	{
		ulLightLevel = 100;
	}
	PRS_SetLighting(pPRU, PR_RING_LIGHT_1, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrRing1Level(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_RING_LIGHT_1);

	if (ulLightLevel > 0)
	{
		ulLightLevel--;
	}

	PRS_SetLighting(pPRU, PR_RING_LIGHT_1, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIIncrRing1Level10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_RING_LIGHT_1);
	
	ulLightLevel += 10;
	if (ulLightLevel > 100)
	{
		ulLightLevel = 100;
	}

	PRS_SetLighting(pPRU, PR_RING_LIGHT_1, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrRing1Level10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_RING_LIGHT_1);

	if (ulLightLevel < 10)
	{
		ulLightLevel = 0;
	}
	else
	{
		ulLightLevel -= 10;
	}

	PRS_SetLighting(pPRU, PR_RING_LIGHT_1, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrSideLevel10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_SIDE_LIGHT);

	if (ulLightLevel < 10)
	{
		ulLightLevel = 0;
	}
	else
	{
		ulLightLevel -= 10;
	}

	PRS_SetLighting(pPRU, PR_SIDE_LIGHT, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrSideLevel(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_SIDE_LIGHT);

	if (ulLightLevel < 1)
	{
		ulLightLevel = 0;
	}
	else
	{
		ulLightLevel--;
	}

	PRS_SetLighting(pPRU, PR_SIDE_LIGHT, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIIncrSideLevel(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_SIDE_LIGHT);

	if (ulLightLevel > 100)
	{
		ulLightLevel = 100;
	}
	else
	{
		ulLightLevel++;
	}

	PRS_SetLighting(pPRU, PR_SIDE_LIGHT, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIIncrSideLevel10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_SIDE_LIGHT);

	if (ulLightLevel > 90)
	{
		ulLightLevel = 100;
	}
	else
	{
		ulLightLevel += 10;
	}

	PRS_SetLighting(pPRU, PR_SIDE_LIGHT, (PR_UWORD)ulLightLevel);
	return 0;
}

// 20141023
LONG CWinEagle::HMIDecrSide1Level10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_SIDE_LIGHT_1);

	if (ulLightLevel < 10)
	{
		ulLightLevel = 0;
	}
	else
	{
		ulLightLevel -= 10;
	}

	PRS_SetLighting(pPRU, PR_SIDE_LIGHT_1, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrSide1Level(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_SIDE_LIGHT_1);

	if (ulLightLevel < 1)
	{
		ulLightLevel = 0;
	}
	else
	{
		ulLightLevel--;
	}

	PRS_SetLighting(pPRU, PR_SIDE_LIGHT_1, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIIncrSide1Level(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_SIDE_LIGHT_1);

	if (ulLightLevel > 100)
	{
		ulLightLevel = 100;
	}
	else
	{
		ulLightLevel++;
	}

	PRS_SetLighting(pPRU, PR_SIDE_LIGHT_1, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIIncrSide1Level10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_SIDE_LIGHT_1);

	if (ulLightLevel > 90)
	{
		ulLightLevel = 100;
	}
	else
	{
		ulLightLevel += 10;
	}

	PRS_SetLighting(pPRU, PR_SIDE_LIGHT_1, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrBackLevel10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_BACK_LIGHT);

	if (ulLightLevel < 10)
	{
		ulLightLevel = 0;
	}
	else
	{
		ulLightLevel -= 10;
	}

	PRS_SetLighting(pPRU, PR_BACK_LIGHT, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrBackLevel(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_BACK_LIGHT);

	if (ulLightLevel < 1)
	{
		ulLightLevel = 0;
	}
	else
	{
		ulLightLevel--;
	}

	PRS_SetLighting(pPRU, PR_BACK_LIGHT, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIIncrBackLevel(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_BACK_LIGHT);

	if (ulLightLevel > 100)
	{
		ulLightLevel = 100;
	}
	else
	{
		ulLightLevel++;
	}

	PRS_SetLighting(pPRU, PR_BACK_LIGHT, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIIncrBackLevel10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_BACK_LIGHT);

	if (ulLightLevel > 90)
	{
		ulLightLevel = 100;
	}
	else
	{
		ulLightLevel += 10;
	}

	PRS_SetLighting(pPRU, PR_BACK_LIGHT, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrBack1Level10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_BACK_LIGHT_1);

	if (ulLightLevel < 10)
	{
		ulLightLevel = 0;
	}
	else
	{
		ulLightLevel -= 10;
	}

	PRS_SetLighting(pPRU, PR_BACK_LIGHT_1, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIDecrBack1Level(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_BACK_LIGHT_1);

	if (ulLightLevel < 1)
	{
		ulLightLevel = 0;
	}
	else
	{
		ulLightLevel--;
	}

	PRS_SetLighting(pPRU, PR_BACK_LIGHT_1, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIIncrBack1Level(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_BACK_LIGHT_1);

	if (ulLightLevel > 100)
	{
		ulLightLevel = 100;
	}
	else
	{
		ulLightLevel++;
	}

	PRS_SetLighting(pPRU, PR_BACK_LIGHT_1, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMIIncrBack1Level10(IPC_CServiceMessage &svMsg)
{
	ULONG	ulLightLevel;
	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	// fix me! issue with PRU lighting (use HMI_ulCurPRU?)
	pPRU = &PRS_SetCameraPRU(nCamera);

	ulLightLevel = (ULONG)PRS_GetLighting(pPRU, PR_BACK_LIGHT_1);

	if (ulLightLevel > 90)
	{
		ulLightLevel = 100;
	}
	else
	{
		ulLightLevel += 10;
	}

	PRS_SetLighting(pPRU, PR_BACK_LIGHT_1, (PR_UWORD)ulLightLevel);
	return 0;
}

LONG CWinEagle::HMI_SetCursorDiagnSteps(IPC_CServiceMessage &svMsg)
{
	ULONG ulSteps;
	svMsg.GetMsg(sizeof(ULONG), &ulSteps);

	switch (ulSteps)
	{
	case 0:
		m_lCursorDiagnSteps = 5;
		break;
	case 1:
		m_lCursorDiagnSteps = 10;
		break;
	case 2:
		m_lCursorDiagnSteps = 50;
		break;
	case 3:
		m_lCursorDiagnSteps = 100;
		break;
	case 4:
		m_lCursorDiagnSteps = 500;
		break;
	case 5:
		m_lCursorDiagnSteps = 1000;
		break;
	default:
		m_lCursorDiagnSteps = 1000;
	}
	return 0;
}

// Add me later
LONG CWinEagle::HMI_IndexCursorXPos(IPC_CServiceMessage &svMsg)
{
	CPreBond *pCPreBond = (CPreBond*)m_pModule->GetStation("PreBond");
	CPreBondWH *pCPreBondWH = (CPreBondWH*)m_pModule->GetStation("PreBondWH");

	PR_BOOLEAN	TempResultantCoordStatus	= PR_FALSE;

	PR_COORD	prTempAlignPt_a;
	//PR_COORD	prTempCentre;
	//BOOL		bDrawCentreCursor			= FALSE;

	prTempAlignPt_a.x	= PR_DEF_CENTRE_X;
	prTempAlignPt_a.y	= PR_DEF_CENTRE_Y;

	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	pPRU = &PRS_SetCameraPRU(nCamera);
	

	//Check drawing centre cursor is required (Set PreBond and PreBond WH alignment pt)
	//if (
	//	(g_lWizardInProgress == SF_PREBOND_WH && g_ulWizardSubTitle == 0 && (g_ulWizardStep == 2 || g_ulWizardStep == 4))||
	//	(g_lWizardInProgress == SF_PREBOND && g_ulWizardSubTitle == 0 && (g_ulWizardStep == 3 || g_ulWizardStep == 5))
	//	)
	//{
	//	if (g_lWizardInProgress == SF_PREBOND)
	//	{
	//		prTempAlignPt_a.x = pCPreBond->m_PRZoomedAlignPt_a.x;
	//		prTempAlignPt_a.y = pCPreBond->m_PRZoomedAlignPt_a.y;
	//	}
	//	else if (g_lWizardInProgress == SF_PREBOND_WH)
	//	{
	//		prTempAlignPt_a.x = pCPreBondWH->m_PRZoomedAlignPt_a.x;
	//		prTempAlignPt_a.y = pCPreBondWH->m_PRZoomedAlignPt_a.y;
	//	}

	//	bDrawCentreCursor = TRUE;
	//}

	// Check cursor is out of boundary
	if (g_stCursor.prCoord.x + (PR_WORD)m_lCursorDiagnSteps > PR_MAX_COORD)
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Out of Boundary");
	}
	else
	{
		// Erase old cursors
		PRS_EraseCross(pPRU->nCamera, g_stCursor.prCoord, g_stCursor.lSize);

		//if (bDrawCentreCursor)
		//{
		//	CalcCentreCursorPt(pPRU, FALSE, prTempAlignPt_a, &prTempCentre);
		//	PRS_EraseCross(pPRU->nCamera, prTempCentre, m_stCentreCursor.lSize);
		//}

		// draw new cursors
		g_stCursor.prCoord.x += (PR_WORD)m_lCursorDiagnSteps;
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Cursor Coord: (%ld,%ld)", g_stCursor.prCoord.x, g_stCursor.prCoord.y);
		PRS_DrawCross(pPRU->nCamera, g_stCursor.prCoord, g_stCursor.lSize, g_stCursor.lColor);

		//if (bDrawCentreCursor)
		//{
		//	PR_COORD prTempCentre;
		//	
		//	if (!CalcCentreCursorPt(pPRU, TRUE, prTempAlignPt_a, &prTempCentre))
		//	{
		//		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "out of zoomed region");
		//	}
		//	else
		//	{
		//		PRS_DrawCross(pPRU->nCamera, prTempCentre, m_stCentreCursor.lSize, m_stCentreCursor.lColor);
		//	}
		//}
	}

	return 0;
}

// Add me later
LONG CWinEagle::HMI_IndexCursorXNeg(IPC_CServiceMessage &svMsg)
{
	CPreBond *pCPreBond = (CPreBond*)m_pModule->GetStation("PreBond");
	CPreBondWH *pCPreBondWH = (CPreBondWH*)m_pModule->GetStation("PreBondWH");

	PR_BOOLEAN	TempResultantCoordStatus	= PR_FALSE;

	PR_COORD	prTempAlignPt_a;
	//PR_COORD	prTempCentre;
	//BOOL		bDrawCentreCursor	= FALSE;

	prTempAlignPt_a.x			= PR_DEF_CENTRE_X;
	prTempAlignPt_a.y			= PR_DEF_CENTRE_Y;

	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	pPRU = &PRS_SetCameraPRU(nCamera);

	//if (
	//	(g_lWizardInProgress == SF_PREBOND_WH && g_ulWizardSubTitle == 0 && (g_ulWizardStep == 2 || g_ulWizardStep == 4))||
	//	(g_lWizardInProgress == SF_PREBOND && g_ulWizardSubTitle == 0 && (g_ulWizardStep == 3 || g_ulWizardStep == 5))
	//	)
	//{
	//	if (g_lWizardInProgress == SF_PREBOND)
	//	{
	//		prTempAlignPt_a.x = pCPreBond->m_PRZoomedAlignPt_a.x;
	//		prTempAlignPt_a.y = pCPreBond->m_PRZoomedAlignPt_a.y;
	//	}
	//	else if (g_lWizardInProgress == SF_PREBOND_WH)
	//	{
	//		prTempAlignPt_a.x = pCPreBondWH->m_PRZoomedAlignPt_a.x;
	//		prTempAlignPt_a.y = pCPreBondWH->m_PRZoomedAlignPt_a.y;
	//	}

	//	bDrawCentreCursor = TRUE;
	//}

	// Check cursor is out of boundary
	if (g_stCursor.prCoord.x - (PR_WORD)m_lCursorDiagnSteps < 0)
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Out of Boundary");
	}
	else
	{
		// Erase old cursors
		PRS_EraseCross(pPRU->nCamera, g_stCursor.prCoord, g_stCursor.lSize);

		//if (bDrawCentreCursor)
		//{
		//	CalcCentreCursorPt(pPRU, FALSE, prTempAlignPt_a, &prTempCentre);
		//	PRS_EraseCross(pPRU->nCamera, prTempCentre, m_stCentreCursor.lSize);
		//}

		// draw new cursors
		g_stCursor.prCoord.x -= (PR_WORD)m_lCursorDiagnSteps;
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Cursor Coord: (%ld,%ld)", g_stCursor.prCoord.x, g_stCursor.prCoord.y);
		PRS_DrawCross(pPRU->nCamera, g_stCursor.prCoord, g_stCursor.lSize, g_stCursor.lColor);

		//if (bDrawCentreCursor)
		//{
		//	if (!CalcCentreCursorPt(pPRU, TRUE, prTempAlignPt_a, &prTempCentre))
		//	{
		//		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "out of zoomed region");
		//	}
		//	else
		//	{
		//		PRS_DrawCross(pPRU->nCamera, prTempCentre, m_stCentreCursor.lSize, m_stCentreCursor.lColor);
		//	}
		//}
	}

	return 0;
}

// Add me later
LONG CWinEagle::HMI_IndexCursorYPos(IPC_CServiceMessage &svMsg)
{
	CPreBond *pCPreBond = (CPreBond*)m_pModule->GetStation("PreBond");
	CPreBondWH *pCPreBondWH = (CPreBondWH*)m_pModule->GetStation("PreBondWH");

	PR_BOOLEAN	TempResultantCoordStatus	= PR_FALSE;

	PR_COORD	prTempAlignPt_a;
	//PR_COORD	prTempCentre;
	//BOOL		bDrawCentreCursor	= FALSE;

	prTempAlignPt_a.x			= PR_DEF_CENTRE_X;
	prTempAlignPt_a.y			= PR_DEF_CENTRE_Y;

	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	pPRU = &PRS_SetCameraPRU(nCamera);

	//Check drawing centre cursor is required (Set PreBond and PreBond WH alignment pt)
	//if (
	//	(g_lWizardInProgress == SF_PREBOND_WH && g_ulWizardSubTitle == 0 && (g_ulWizardStep == 2 || g_ulWizardStep == 4))||
	//	(g_lWizardInProgress == SF_PREBOND && g_ulWizardSubTitle == 0 && (g_ulWizardStep == 3 || g_ulWizardStep == 5))
	//	)
	//{
	//	if (g_lWizardInProgress == SF_PREBOND)
	//	{
	//		prTempAlignPt_a.x = pCPreBond->m_PRZoomedAlignPt_a.x;
	//		prTempAlignPt_a.y = pCPreBond->m_PRZoomedAlignPt_a.y;
	//	}
	//	else if (g_lWizardInProgress == SF_PREBOND_WH)
	//	{
	//		prTempAlignPt_a.x = pCPreBondWH->m_PRZoomedAlignPt_a.x;
	//		prTempAlignPt_a.y = pCPreBondWH->m_PRZoomedAlignPt_a.y;
	//	}
	//	bDrawCentreCursor = TRUE;
	//}

	// Check cursor is out of boundary
	if (g_stCursor.prCoord.y + (PR_WORD)m_lCursorDiagnSteps > PR_MAX_COORD)
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Out of Boundary");
	}
	else
	{
		// Erase old cursors
		PRS_EraseCross(pPRU->nCamera, g_stCursor.prCoord, g_stCursor.lSize);

		//if (bDrawCentreCursor)
		//{
		//	CalcCentreCursorPt(pPRU, FALSE, prTempAlignPt_a, &prTempCentre);
		//	PRS_EraseCross(pPRU->nCamera, prTempCentre, m_stCentreCursor.lSize);
		//}

		// draw new cursors
		g_stCursor.prCoord.y += (PR_WORD)m_lCursorDiagnSteps;
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Cursor Coord: (%ld,%ld)", g_stCursor.prCoord.x, g_stCursor.prCoord.y);
		PRS_DrawCross(pPRU->nCamera, g_stCursor.prCoord, g_stCursor.lSize, g_stCursor.lColor);

		//if (bDrawCentreCursor)
		//{
		//	if (!CalcCentreCursorPt(pPRU, TRUE, prTempAlignPt_a, &prTempCentre))
		//	{
		//		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "out of zoomed region");
		//	}
		//	else
		//	{
		//		PRS_DrawCross(pPRU->nCamera, prTempCentre, m_stCentreCursor.lSize, m_stCentreCursor.lColor);
		//	}
		//}
	}

	return 0;
}

// Add me later
LONG CWinEagle::HMI_IndexCursorYNeg(IPC_CServiceMessage &svMsg)
{
	CPreBond *pCPreBond = (CPreBond*)m_pModule->GetStation("PreBond");
	CPreBondWH *pCPreBondWH = (CPreBondWH*)m_pModule->GetStation("PreBondWH");

	PR_BOOLEAN	TempResultantCoordStatus	= PR_FALSE;

	PR_COORD	prTempAlignPt_a;
	//PR_COORD	prTempCentre;
	//BOOL		bDrawCentreCursor	= FALSE;

	prTempAlignPt_a.x			= PR_DEF_CENTRE_X;
	prTempAlignPt_a.y			= PR_DEF_CENTRE_Y;

	PRU	*pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	pPRU = &PRS_SetCameraPRU(nCamera);

	//Check drawing centre cursor is required (Set PreBond and PreBond WH alignment pt)
	//if (
	//	(g_lWizardInProgress == SF_PREBOND_WH && g_ulWizardSubTitle == 0 && (g_ulWizardStep == 2 || g_ulWizardStep == 4))||
	//	(g_lWizardInProgress == SF_PREBOND && g_ulWizardSubTitle == 0 && (g_ulWizardStep == 3 || g_ulWizardStep == 5))
	//	)
	//{
	//	if (g_lWizardInProgress == SF_PREBOND)
	//	{
	//		prTempAlignPt_a.x = pCPreBond->m_PRZoomedAlignPt_a.x;
	//		prTempAlignPt_a.y = pCPreBond->m_PRZoomedAlignPt_a.y;
	//	}
	//	else if (g_lWizardInProgress == SF_PREBOND_WH)
	//	{
	//		prTempAlignPt_a.x = pCPreBondWH->m_PRZoomedAlignPt_a.x;
	//		prTempAlignPt_a.y = pCPreBondWH->m_PRZoomedAlignPt_a.y;
	//	}
	//	bDrawCentreCursor = TRUE;
	//}

	// Check cursor is out of boundary
	if (g_stCursor.prCoord.y - (PR_WORD)m_lCursorDiagnSteps < 0)
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Out of Boundary");
	}
	else
	{
		// Erase old cursors
		PRS_EraseCross(pPRU->nCamera, g_stCursor.prCoord, g_stCursor.lSize);

		//if (bDrawCentreCursor)
		//{
		//	CalcCentreCursorPt(pPRU, FALSE, prTempAlignPt_a, &prTempCentre);
		//	PRS_EraseCross(pPRU->nCamera, prTempCentre, m_stCentreCursor.lSize);
		//}

		// draw new cursors
		g_stCursor.prCoord.y -= (PR_WORD)m_lCursorDiagnSteps;
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Cursor Coord: (%ld,%ld)", g_stCursor.prCoord.x, g_stCursor.prCoord.y);
		PRS_DrawCross(pPRU->nCamera, g_stCursor.prCoord, g_stCursor.lSize, g_stCursor.lColor);

		//if (bDrawCentreCursor)
		//{
		//	if (!CalcCentreCursorPt(pPRU, TRUE, prTempAlignPt_a, &prTempCentre))
		//	{
		//		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "out of zoomed region");
		//	}
		//	else
		//	{
		//		PRS_DrawCross(pPRU->nCamera, prTempCentre, m_stCentreCursor.lSize, m_stCentreCursor.lColor);
		//	}
		//}
	}
	return 0;
}

LONG CWinEagle::HMIFreeAllRecords(IPC_CServiceMessage &svMsg)
{
	LONG lAnswer = rMSG_TIMEOUT;

	lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Operation is irreversible. Continue?");

	if (lAnswer == rMSG_TIMEOUT)
	{
		HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
	}
	else if (lAnswer == rMSG_CONTINUE)
	{
		FreeAllRecords();
		PRS_InitPRUs();
		WriteHMIMess("*** All PR Record Freed ***");
	}
	else
	{
		HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
	}

	return 0;
}

////////////////////////////////////////////
// Advance Log Function
////////////////////////////////////////////
LONG CWinEagle::HMIOnOffLogMode(IPC_CServiceMessage &svMsg)
{
	if (!m_bPRDebug)
	{
		PRS_EnableLog();
		WriteHMIMess("*** PR Log Enabled ***");
		m_bPRDebug = TRUE;
	}
	else
	{
		PRS_DisableLog();
		WriteHMIMess("*** PR Log Disabled ***");
		m_bPRDebug = FALSE;
	}

	return 0;
}

LONG CWinEagle::HMISetDebugFlag(IPC_CServiceMessage &svMsg)
{
	PR_DEBUG ulDebugFlag;

	CString sDebugFlag;
	LPTSTR lpsz = NULL;
	INT nMsgLen = 0;

	nMsgLen = svMsg.GetMsgLen();
	lpsz = new TCHAR[nMsgLen];
	svMsg.GetMsg(lpsz, nMsgLen);
	sDebugFlag = ToString(lpsz); // convert string char to CString
	delete [] lpsz; //20150615

	if ("Disable" == sDebugFlag)
	{
		ulDebugFlag = PR_DEBUG_DISABLE;
	}
	else if ("Show Data" == sDebugFlag)
	{
		ulDebugFlag = PR_DEBUG_SHOW_DATA;
	}
	else if ("Show Time" == sDebugFlag)
	{
		ulDebugFlag = PR_DEBUG_SHOW_TIME;
	}
	else if ("2 Points" == sDebugFlag)
	{
		ulDebugFlag = PR_DEBUG_2_POINTS;
	}
	else if ("Golden Template" == sDebugFlag)
	{
		ulDebugFlag = PR_DEBUG_GOLDEN_TMPL;
	}
	else if ("Show Permanent Mem" == sDebugFlag)
	{
		ulDebugFlag = PR_DEBUG_PERMANENT_MEM;
	}
	else if ("Log Data" == sDebugFlag)
	{
		ulDebugFlag = PR_DEBUG_LOG_DATA;
	}
	else if ("Log Result" == sDebugFlag)
	{
		ulDebugFlag = PR_DEBUG_LOG_RESULT;
	}
	else if ("Log Auto Test" == sDebugFlag)
	{
		ulDebugFlag = PR_DEBUG_LOG_AUTO_TEST;
	}
	else if ("Log Valid Parameters" == sDebugFlag)
	{
		ulDebugFlag = PR_DEBUG_VALID_PAR;
	}
	else if ("BGA" == sDebugFlag)
	{
		ulDebugFlag = PR_DEBUG_BGA;
	}
	else if ("Log Failure" == sDebugFlag)
	{
		ulDebugFlag = PR_DEBUG_LOG_FAILURE;
	}
	else if ("Log Min Failure" == sDebugFlag)
	{
		ulDebugFlag = PR_DEBUG_LOG_MIN_FAILURE;
	}
	else if ("Log Alg Time" == sDebugFlag)
	{
		ulDebugFlag = PR_DEBUG_LOG_ALG_TIME;
	}
	else
	{
		ulDebugFlag = PR_DEBUG_DISABLE;
	}

	PRS_SetDebugFlag(ulDebugFlag);

	CString szMsg = "*** PR Debug Mode Set To " + sDebugFlag + " ***";
	WriteHMIMess(szMsg);

	return 0;
}

LONG CWinEagle::HMISetComLogFlag(IPC_CServiceMessage &svMsg)
{
	PR_COM_LOG ulComLogFlag = PR_COM_LOG_DISABLE;

	CString sComLogFlag;
	LPTSTR lpsz = NULL;
	INT nMsgLen = 0;

	nMsgLen = svMsg.GetMsgLen();
	lpsz = new TCHAR[nMsgLen];
	svMsg.GetMsg(lpsz, nMsgLen);
	sComLogFlag = ToString(lpsz); // convert string char to CString
	delete [] lpsz; //20150615

	if ("Disable" == sComLogFlag)
	{
		ulComLogFlag = PR_COM_LOG_DISABLE;
	}
	else if ("Log All" == sComLogFlag)
	{
		ulComLogFlag = PR_COM_LOG_ALL;
	}
	else if ("Log Long Com Time" == sComLogFlag)
	{
		ulComLogFlag = PR_COM_LOG_LONG_COM_TIME;
	}

	PRS_SetComLogFlag(ulComLogFlag);

	CString szMsg = "*** PR Comm Log Flag Set To " + sComLogFlag + " ***";
	WriteHMIMess(szMsg);

	return 0;
}

LONG CWinEagle::HMISetComLogTimeLimit(IPC_CServiceMessage &svMsg)
{
	CString szMsg = _T("");

	ULONG ulComLogTimeLimit;
	svMsg.GetMsg(sizeof(ULONG), &ulComLogTimeLimit);

	PRS_SetComLogTimeLimit((PR_ULWORD)ulComLogTimeLimit);

	szMsg.Format("*** PR comm log time limit set to %ld ***", ulComLogTimeLimit);
	WriteHMIMess(szMsg);

	return 0;
}

LONG CWinEagle::HMISetVisionNtDlgToService(IPC_CServiceMessage &svMsg)
{
	PRS_VisionNtDlg(PR_VISIONNT_DLG_SERVICE);

	return 0;
}

LONG CWinEagle::HMISetVisionNtDlgToVision(IPC_CServiceMessage &svMsg)
{
	PRS_VisionNtDlg(PR_VISIONNT_DLG_DIAGNOSTIC);

	return 0;
}

/////////////////////////////////////
// Caibration Setup Page
/////////////////////////////////////
LONG CWinEagle::HMI_SetCalibPRSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");

	switch (HMI_ulCurPRU)
	{
		case ACF1WH_CALIB_PRU:
		pCACF1WH->SetPRSelected(bMode);
		break;
		case ACF2WH_CALIB_PRU:
		pCACF2WH->SetPRSelected(bMode);
		break;
	}

	UpdateHMICalibPRButton(FALSE);

	return 0;
}

LONG CWinEagle::HMI_SetCalibModSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CInspOpt *pCInspOpt = (CInspOpt*)m_pModule->GetStation("InspOpt");
	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");

	switch (HMI_ulCurPRU)
	{
		case ACF1WH_CALIB_PRU:
		pCACF1WH->SetModSelected(bMode);
		pCInspOpt->SetModSelected(bMode);
		break;

		case ACF2WH_CALIB_PRU:
		pCACF2WH->SetModSelected(bMode);
		pCInspOpt->SetModSelected(bMode);
		break;

		case ACFUL_CALIB_PRU:
		pCTA1->SetModSelected(bMode);
		break;
	}

	UpdateHMICalibPRButton(FALSE);

	return 0;
}

LONG CWinEagle::HMI_SelectCalibPRU(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	ULONG ulPRU;
	svMsg.GetMsg(sizeof(ULONG), &ulPRU);
	
	LONG lAnswer = rMSG_TIMEOUT;
	HMI_ulOldCurPRU = HMI_ulCurPRU;
	HMI_ulCurPRU = ulPRU;
	
	lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Move Modules to Calibration Posns?");

	if (lAnswer == rMSG_TIMEOUT)
	{
		HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
		return 0;
	}		
	else if (lAnswer == rMSG_CONTINUE)
	{
		m_bMoveToPosn = TRUE;
	}
	else
	{
		m_bMoveToPosn = FALSE;
	}

	SelectCalibPRU(ulPRU);

	UpdateHMICalibPRButton(FALSE);

	return 0;
}

LONG CWinEagle::HMI_SelectPRAlg(IPC_CServiceMessage &svMsg)
{
	CSettingFile *pCSettingFile = (CSettingFile*)m_pModule->GetStation("SettingFile");
	BOOL bOpDone = FALSE;

	ULONG ulAlg;
	svMsg.GetMsg(sizeof(ULONG), &ulAlg);

	HMI_ulPRAlg = ulAlg;

	switch (HMI_ulPRAlg) //Update HMI only
	{
	case FIDUCIAL_FITTING:
		HMI_bShowFiducial = TRUE;
		SetHmiVariable("PR_bShowFiducial");
		switch (HMI_ulPRFMarkType)
		{
		case 0:
			g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\FMarkType1.png";
			SetHmiVariable("SF_szWizardBMPath");

			break;
		case 1:
			g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\FMarkType2.png";
			SetHmiVariable("SF_szWizardBMPath");

			break;
		case 2:
			g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\FMarkType3.png";
			SetHmiVariable("SF_szWizardBMPath");

			break;
		}
		break;

	default:
		HMI_bShowFiducial = FALSE;
		SetHmiVariable("PR_bShowFiducial");
		g_szWizardBMPath = "";
		SetHmiVariable("SF_szWizardBMPath");	
		break;
	}

#ifdef PRS_SET_IGNORE_WINDOW_FIRST //20150324
	switch (HMI_ulPRAlg) //Update HMI only
	{
	case PATTERN_MATCHING:
	case EDGE_MATCHING:
	case FIDUCIAL_FITTING:
	case RECTANGLE_MATCHING:
		pCSettingFile->HMI_bShowIgnoreWindowBtn = TRUE;
		break;
	case SHAPE_FITTING:
	case TMPL_FITTING:
		pCSettingFile->HMI_bShowIgnoreWindowBtn = FALSE;
		break;
	}
	SetHmiVariable("SF_bShowIgnoreWindowBtn");
#endif
	bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}

LONG CWinEagle::HMI_SelectFMarkType(IPC_CServiceMessage &svMsg)
{
	ULONG ulType;
	svMsg.GetMsg(sizeof(ULONG), &ulType);

	HMI_ulPRFMarkType = ulType;
	switch (HMI_ulPRFMarkType)
	{
	case 0:
		g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\FMarkType1.png";
		SetHmiVariable("SF_szWizardBMPath");

		break;
	case 1:
		g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\FMarkType2.png";
		SetHmiVariable("SF_szWizardBMPath");

		break;
	case 2:
		g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\FMarkType3.png";
		SetHmiVariable("SF_szWizardBMPath");

		break;
	}
	return 0;
}

LONG CWinEagle::HMI_SetCalibPRDelay(IPC_CServiceMessage &svMsg)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");

	LONG lDelay = 0;
	svMsg.GetMsg(sizeof(LONG), &lDelay);

	HMI_lCalibPRDelay = lDelay;

	switch (HMI_ulCurPRU)
	{
		case ACF1WH_CALIB_PRU:
			pCACF1WH->m_lCalibPRDelay = lDelay;
		break;
		case ACF2WH_CALIB_PRU:
			pCACF2WH->m_lCalibPRDelay = lDelay;
		break;
		case ACFUL_CALIB_PRU:
			pCTA1->m_lCalibPRDelay = lDelay;
		break;

	}

	UpdateHMICalibPRButton(FALSE);
	return 0;
}

LONG CWinEagle::HMI_SetCalibCORRange(IPC_CServiceMessage &svMsg)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	DOUBLE dRange;
	svMsg.GetMsg(sizeof(DOUBLE), &dRange);

	if (dRange == 0)
	{
		HMIMessageBox(MSG_OK, "SET COR CALIBRATION RANGE", "Parameter Error! Calibration Range must not be 0");
		return 0;
	}

	switch (HMI_ulCurPRU)
	{

		case ACF1WH_CALIB_PRU:		
		pCACF1WH->m_dCalibCORRange	= dRange;
		break;
		case ACF2WH_CALIB_PRU:		
		pCACF2WH->m_dCalibCORRange	= dRange;
		break;
	}

	HMI_dCalibCORRange = dRange;

	return 0;
}

LONG CWinEagle::HMI_SetTestCORAng(IPC_CServiceMessage &svMsg)
{
	DOUBLE dAng;
	svMsg.GetMsg(sizeof(DOUBLE), &dAng);

	HMI_dTestCORAng = dAng;

	return 0;
}

#if 0 //20140627 PBDL calib
//LONG CWinEagle::HMI_SetThermalCalibRefPosn(IPC_CServiceMessage &svMsg)
//{
//	CTI1 *pCTI1 = (CTI1*)m_pModule->GetStation("TI1");
//	CTI2 *pCTI2 = (CTI2*)m_pModule->GetStation("TI2");
//	CINWH *pCINWH = (CINWH*)m_pModule->GetStation("INWH");
//	CPreBondWH *pCPreBondWH = (CPreBondWH*)m_pModule->GetStation("PreBondWH");
//	CShuttle *pCShuttle = (CShuttle*)m_pModule->GetStation("Shuttle");
//	CPB1 *pCPB1 = (CPB1*)m_pModule->GetStation("PB1");
//	CPB2 *pCPB2 = (CPB2*)m_pModule->GetStation("PB2");
//	CPBGlass1PR *pCPBGlass1PR = (CPBGlass1PR*)m_pModule->GetStation("PBGlass1PR");
//	CPBGlass2PR *pCPBGlass2PR = (CPBGlass2PR*)m_pModule->GetStation("PBGlass2PR");
//	CTA4 *pCTA4	= (CTA4*)m_pModule->GetStation("TA4");
//	CMB1WH *pCMB1WH	= (CMB1WH*)m_pModule->GetStation("MB1WH");
//	CMB2WH *pCMB2WH	= (CMB2WH*)m_pModule->GetStation("MB2WH");
//
//	BOOL bResult = TRUE;
//	PRU *pPRU;
//	CString strMsg;
//	LONG lAnswer = -1;
//
	//pPRU = &SelectCurPRU(HMI_ulCurPRU);
//
//	switch (HMI_ulCurPRU)
//	{
//	case PBDL1_CALIB_PRU:
//		strMsg.Format("Please set this posn and other PBWH calib glass posn under the same thermal condition");
//		HMIMessageBox(MSG_OK, "WARNING", strMsg);
//
//		if (!pCPBGlass1PR->m_bModSelected)
//		{
//			strMsg.Format("%s Module Not Selected. Operation Abort", pCPBGlass1PR->GetStnName());
//			HMIMessageBox(MSG_OK, "WARNING", strMsg);
//			bResult = FALSE;
//		}
//		else
//		{
//			if (!pCPBGlass1PR->SetThermalCalibRefPosn(pPRU))
//			{
//				bResult = FALSE;
//			}
//		}
//		break;
//
//	case PBDL2_CALIB_PRU:
//		strMsg.Format("Please set this posn and other PBWH calib glass posn under the same thermal condition");
//		HMIMessageBox(MSG_OK, "WARNING", strMsg);
//
//		if (!pCPBGlass2PR->m_bModSelected)
//		{
//			strMsg.Format("%s Module Not Selected. Operation Abort", pCPBGlass2PR->GetStnName());
//			HMIMessageBox(MSG_OK, "WARNING", strMsg);
//			bResult = FALSE;
//		}
//		else
//		{
//			if (!pCPBGlass2PR->SetThermalCalibRefPosn(pPRU))
//			{
//				bResult = FALSE;
//			}
//		}
//		break;
//
//	case INWH_CALIB_PRU:
//	case UPLOOK1_CALIB_PRU:
//	case UPLOOK2_CALIB_PRU:
//	case SHUTTLE_CAM1_CALIB_PRU:
//	case SHUTTLE_CAM2_CALIB_PRU:
//	case LSI_CALIB_PRU:
//	case TA4_CALIB_PRU:
//	case UPLOOK1_GLASS_CALIB_PRU:
//	case UPLOOK2_GLASS_CALIB_PRU:
//	case MBDL1_CALIB_PRU:
//	case MBDL2_CALIB_PRU:
//	case ACFUL_CALIB_PRU:
//		strMsg.Format("For PB Glass Only. Operation Abort");
//		HMIMessageBox(MSG_OK, "WARNING", strMsg);
//		bResult = FALSE;
//		break;
//
//	}
//
//	if (bResult)
//	{
//		HMIMessageBox(MSG_OK, "SET THERMAL CAL REF", "OPERATION SUCCESS");
//	}
//	else
//	{
//		HMIMessageBox(MSG_OK, "SET THERMAL CAL REF", "OPERATION FAIL");
//	}
//
	//UpdateHMICalibPRButton(FALSE);
//
//	BOOL bCalInProgress = TRUE;
//	svMsg.InitMessage(sizeof(BOOL), &bCalInProgress);
//
//	return 1;
//}
//
//LONG CWinEagle::HMI_GoThermalCalibRefPosn(IPC_CServiceMessage &svMsg)
//{
//	CTI1 *pCTI1 = (CTI1*)m_pModule->GetStation("TI1");
//	CTI2 *pCTI2 = (CTI2*)m_pModule->GetStation("TI2");
//	CINWH *pCINWH = (CINWH*)m_pModule->GetStation("INWH");
//	CPreBondWH *pCPreBondWH = (CPreBondWH*)m_pModule->GetStation("PreBondWH");
//	CShuttle *pCShuttle = (CShuttle*)m_pModule->GetStation("Shuttle");
//	CPB1 *pCPB1 = (CPB1*)m_pModule->GetStation("PB1");
//	CPB2 *pCPB2 = (CPB2*)m_pModule->GetStation("PB2");
//	CPBGlass1PR *pCPBGlass1PR = (CPBGlass1PR*)m_pModule->GetStation("PBGlass1PR");
//	CPBGlass2PR *pCPBGlass2PR = (CPBGlass2PR*)m_pModule->GetStation("PBGlass2PR");
//	CTA4 *pCTA4	= (CTA4*)m_pModule->GetStation("TA4");
//	CMB1WH *pCMB1WH	= (CMB1WH*)m_pModule->GetStation("MB1WH");
//	CMB2WH *pCMB2WH	= (CMB2WH*)m_pModule->GetStation("MB2WH");
//
//	BOOL bResult = TRUE;
//	PRU *pPRU = NULL;
//	CString strMsg;
//	LONG lAnswer = -1;
//
//	//pPRU = &SelectCurPRU(HMI_ulCurPRU);
//
//	switch (HMI_ulCurPRU)
//	{
//	case PBDL1_CALIB_PRU:
//		if (!pCPreBondWH->m_bModSelected)
//		{
//			strMsg.Format("%s Module Not Selected. Operation Abort", pCPreBondWH->GetStnName());
//			HMIMessageBox(MSG_OK, "WARNING", strMsg);
//			bResult = FALSE;
//		}
//		else
//		{
//			if (!pCPBGlass1PR->GoThermalCalibRefPosn())
//			{
//				bResult = FALSE;
//			}
//		}
//		break;
//
//	case PBDL2_CALIB_PRU:
//		if (!pCPreBondWH->m_bModSelected)
//		{
//			strMsg.Format("%s Module Not Selected. Operation Abort", pCPreBondWH->GetStnName());
//			HMIMessageBox(MSG_OK, "WARNING", strMsg);
//			bResult = FALSE;
//		}
//		else
//		{
//			if (!pCPBGlass2PR->GoThermalCalibRefPosn())
//			{
//				bResult = FALSE;
//			}
//		}
//		break;
//
//	case INWH_CALIB_PRU:
//	case UPLOOK1_CALIB_PRU:
//	case UPLOOK2_CALIB_PRU:
//	case SHUTTLE_CAM1_CALIB_PRU:
//	case SHUTTLE_CAM2_CALIB_PRU:
//	case LSI_CALIB_PRU:
//	case TA4_CALIB_PRU:
//	case UPLOOK1_GLASS_CALIB_PRU:
//	case UPLOOK2_GLASS_CALIB_PRU:
//	case MBDL1_CALIB_PRU:
//	case MBDL2_CALIB_PRU:
//	case ACFUL_CALIB_PRU:
//		strMsg.Format("For PB Glass Only. Operation Abort");
//		HMIMessageBox(MSG_OK, "WARNING", strMsg);
//		bResult = FALSE;
//		break;
//
//	}
//
//	if (!bResult)
//	{
//		HMIMessageBox(MSG_OK, "GO THERMAL CAL REF", "OPERATION FAIL");
//	}
//
//	//UpdateHMICalibPRButton(FALSE);
//
//	BOOL bUICompleted = TRUE;
//	svMsg.InitMessage(sizeof(BOOL), &bUICompleted);
//
//	return 1;
//}
#endif

LONG CWinEagle::HMI_AutoCal(IPC_CServiceMessage &svMsg)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");

	BOOL bResult = TRUE;
	PRU *pPRU;
	CString strMsg;
	LONG lAnswer = -1;

	pPRU = &SelectCurPRU(HMI_ulCurPRU);

	switch (HMI_ulCurPRU)
	{
		case ACF1WH_CALIB_PRU:
			if (!pCACF1WH->m_bModSelected)
			{
				strMsg.Format("%s Module Not Selected. Operation Abort", pCACF1WH->GetStnName());
				HMIMessageBox(MSG_OK, "WARNING", strMsg);
				bResult = FALSE;
			}
			else
			{
				pruACF_WHCalib.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF1WH;
				if (!pCACF1WH->AutoCalibration(pPRU))
				{
					bResult = FALSE;
				}	
			}
			break;

		case ACF2WH_CALIB_PRU:
			if (!pCACF2WH->m_bModSelected)
			{
				strMsg.Format("%s Module Not Selected. Operation Abort", pCACF2WH->GetStnName());
				HMIMessageBox(MSG_OK, "WARNING", strMsg);
				bResult = FALSE;
			}
			else
			{
				pruACF_WHCalib.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF2WH;
				if (!pCACF2WH->AutoCalibration(pPRU))
				{
					bResult = FALSE;
				}	
			}
			break;

		case ACFUL_CALIB_PRU:
			if (!pCTA1->m_bModSelected)
			{
				strMsg.Format("%s Module Not Selected. Operation Abort", pCTA1->GetStnName());
				HMIMessageBox(MSG_OK, "WARNING", strMsg);
				bResult = FALSE;
			}
			else
			{
				if (!pCTA1->AutoCalibration(pPRU))
				{
					bResult = FALSE;
				}	
			}
			break;
	}
	// Set Calib PRU First
	if (!bResult)
	{
		pPRU->bCalibrated = FALSE;
	}
	else
	{
		pPRU->bCalibrated = TRUE;
	}

	//Set bCalibrated for all relative this pPRU
	for (INT i = 1; i < GetmyPRUList().GetSize(); i++)
	{
		PRU *pTempPRU = GetmyPRUList().GetAt(i);  //normal PRU
		if (pTempPRU->eCamMoveGroup1 == pPRU->eCamMoveGroupCurrent) //first abcd
		{
			if (!bResult)
			{
				pTempPRU->bCalibrated	= FALSE;
			}
			else
			{
				pTempPRU->bCalibrated	= TRUE;

				pTempPRU->m2p_xform.a	= pPRU->m2p_xform.a;
				pTempPRU->m2p_xform.b	= pPRU->m2p_xform.b;
				pTempPRU->m2p_xform.c	= pPRU->m2p_xform.c;
				pTempPRU->m2p_xform.d	= pPRU->m2p_xform.d;

				pTempPRU->p2m_xform.a	= pPRU->p2m_xform.a;
				pTempPRU->p2m_xform.b	= pPRU->p2m_xform.b;
				pTempPRU->p2m_xform.c	= pPRU->p2m_xform.c;
				pTempPRU->p2m_xform.d	= pPRU->p2m_xform.d;
			}
		}

		if (pTempPRU->eCamMoveGroup2 == pPRU->eCamMoveGroupCurrent) //Second abcd
		{
			if (!bResult)
			{
				pTempPRU->bCalibrated_2	= FALSE;
			}
			else
			{
				pTempPRU->bCalibrated_2	= TRUE;

				pTempPRU->m2p_xform_2.a	= pPRU->m2p_xform_2.a;
				pTempPRU->m2p_xform_2.b	= pPRU->m2p_xform_2.b;
				pTempPRU->m2p_xform_2.c	= pPRU->m2p_xform_2.c;
				pTempPRU->m2p_xform_2.d	= pPRU->m2p_xform_2.d;

				pTempPRU->p2m_xform_2.a	= pPRU->p2m_xform_2.a;
				pTempPRU->p2m_xform_2.b	= pPRU->p2m_xform_2.b;
				pTempPRU->p2m_xform_2.c	= pPRU->p2m_xform_2.c;
				pTempPRU->p2m_xform_2.d	= pPRU->p2m_xform_2.d;
			}
		}

		if (pTempPRU->eCamMoveGroup3 == pPRU->eCamMoveGroupCurrent)
		{
			if (!bResult)
			{
				pTempPRU->bCalibrated_3	= FALSE;
			}
			else
			{
				pTempPRU->bCalibrated_3	= TRUE;

				pTempPRU->m2p_xform_3.a	= pPRU->m2p_xform_3.a;
				pTempPRU->m2p_xform_3.b	= pPRU->m2p_xform_3.b;
				pTempPRU->m2p_xform_3.c	= pPRU->m2p_xform_3.c;
				pTempPRU->m2p_xform_3.d	= pPRU->m2p_xform_3.d;

				pTempPRU->p2m_xform_3.a	= pPRU->p2m_xform_3.a;
				pTempPRU->p2m_xform_3.b	= pPRU->p2m_xform_3.b;
				pTempPRU->p2m_xform_3.c	= pPRU->p2m_xform_3.c;
				pTempPRU->p2m_xform_3.d	= pPRU->p2m_xform_3.d;
			}
		}

		if (pTempPRU->eCamMoveGroup4 == pPRU->eCamMoveGroupCurrent)
		{
			if (!bResult)
			{
				pTempPRU->bCalibrated_4	= FALSE;
			}
			else
			{
				pTempPRU->bCalibrated_4	= TRUE;

				pTempPRU->m2p_xform_4.a	= pPRU->m2p_xform_4.a;
				pTempPRU->m2p_xform_4.b	= pPRU->m2p_xform_4.b;
				pTempPRU->m2p_xform_4.c	= pPRU->m2p_xform_4.c;
				pTempPRU->m2p_xform_4.d	= pPRU->m2p_xform_4.d;

				pTempPRU->p2m_xform_4.a	= pPRU->p2m_xform_4.a;
				pTempPRU->p2m_xform_4.b	= pPRU->p2m_xform_4.b;
				pTempPRU->p2m_xform_4.c	= pPRU->p2m_xform_4.c;
				pTempPRU->p2m_xform_4.d	= pPRU->p2m_xform_4.d;
			}
		}
	}

	for (INT i = 0; i < GetmyCalibPRUList().GetSize(); i++)
	{
		PRU *pTempPRU = GetmyCalibPRUList().GetAt(i); //calib pru
		if (pTempPRU->eCamMoveGroup1 == pPRU->eCamMoveGroupCurrent) //first abcd
		{
			if (!bResult)
			{
				pTempPRU->bCalibrated	= FALSE;
			}
			else
			{
				pTempPRU->bCalibrated	= TRUE;

				pTempPRU->m2p_xform.a	= pPRU->m2p_xform.a;
				pTempPRU->m2p_xform.b	= pPRU->m2p_xform.b;
				pTempPRU->m2p_xform.c	= pPRU->m2p_xform.c;
				pTempPRU->m2p_xform.d	= pPRU->m2p_xform.d;

				pTempPRU->p2m_xform.a	= pPRU->p2m_xform.a;
				pTempPRU->p2m_xform.b	= pPRU->p2m_xform.b;
				pTempPRU->p2m_xform.c	= pPRU->p2m_xform.c;
				pTempPRU->p2m_xform.d	= pPRU->p2m_xform.d;
			}
		}
		if (pTempPRU->eCamMoveGroup2 == pPRU->eCamMoveGroupCurrent) //second abcd
		{
			if (!bResult)
			{
				pTempPRU->bCalibrated_2	= FALSE;
			}
			else
			{
				pTempPRU->bCalibrated_2	= TRUE;

				pTempPRU->m2p_xform_2.a	= pPRU->m2p_xform_2.a;
				pTempPRU->m2p_xform_2.b	= pPRU->m2p_xform_2.b;
				pTempPRU->m2p_xform_2.c	= pPRU->m2p_xform_2.c;
				pTempPRU->m2p_xform_2.d	= pPRU->m2p_xform_2.d;

				pTempPRU->p2m_xform_2.a	= pPRU->p2m_xform_2.a;
				pTempPRU->p2m_xform_2.b	= pPRU->p2m_xform_2.b;
				pTempPRU->p2m_xform_2.c	= pPRU->p2m_xform_2.c;
				pTempPRU->p2m_xform_2.d	= pPRU->p2m_xform_2.d;
			}
		}
		if (pTempPRU->eCamMoveGroup3 == pPRU->eCamMoveGroupCurrent)
		{
			if (!bResult)
			{
				pTempPRU->bCalibrated_3	= FALSE;
			}
			else
			{
				pTempPRU->bCalibrated_3	= TRUE;

				pTempPRU->m2p_xform_3.a	= pPRU->m2p_xform_3.a;
				pTempPRU->m2p_xform_3.b	= pPRU->m2p_xform_3.b;
				pTempPRU->m2p_xform_3.c	= pPRU->m2p_xform_3.c;
				pTempPRU->m2p_xform_3.d	= pPRU->m2p_xform_3.d;

				pTempPRU->p2m_xform_3.a	= pPRU->p2m_xform_3.a;
				pTempPRU->p2m_xform_3.b	= pPRU->p2m_xform_3.b;
				pTempPRU->p2m_xform_3.c	= pPRU->p2m_xform_3.c;
				pTempPRU->p2m_xform_3.d	= pPRU->p2m_xform_3.d;
			}
		}
		if (pTempPRU->eCamMoveGroup4 == pPRU->eCamMoveGroupCurrent)
		{
			if (!bResult)
			{
				pTempPRU->bCalibrated_4	= FALSE;
			}
			else
			{
				pTempPRU->bCalibrated_4	= TRUE;

				pTempPRU->m2p_xform_4.a	= pPRU->m2p_xform_4.a;
				pTempPRU->m2p_xform_4.b	= pPRU->m2p_xform_4.b;
				pTempPRU->m2p_xform_4.c	= pPRU->m2p_xform_4.c;
				pTempPRU->m2p_xform_4.d	= pPRU->m2p_xform_4.d;

				pTempPRU->p2m_xform_4.a	= pPRU->p2m_xform_4.a;
				pTempPRU->p2m_xform_4.b	= pPRU->p2m_xform_4.b;
				pTempPRU->p2m_xform_4.c	= pPRU->p2m_xform_4.c;
				pTempPRU->p2m_xform_4.d	= pPRU->p2m_xform_4.d;
			}
		}
	}

	if (bResult)
	{
		HMIMessageBox(MSG_OK, "AUTO CAL", "OPERATION SUCCESS");
	}
	else
	{
		HMIMessageBox(MSG_OK, "AUTO CAL", "OPERATION FAIL");
	}

	UpdateHMICalibPRButton(FALSE);

	BOOL bCalInProgress = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bCalInProgress);


	return 1;
}

LONG CWinEagle::HMI_CORCal(IPC_CServiceMessage &svMsg)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	BOOL bCalInProgress = TRUE;
	LONG lAnswer = rMSG_TIMEOUT;
	BOOL bResult = TRUE;
	CString strMsg;
	lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Operation is irreversible. Continue?");

	if (lAnswer == rMSG_CANCEL || lAnswer == rMSG_TIMEOUT)
	{
		svMsg.InitMessage(sizeof(BOOL), &bCalInProgress);
		bCalInProgress = FALSE;
		UpdateHMICalibPRButton(FALSE);
		return 1;
	}

	switch (HMI_ulCurPRU)
	{
	case ACF1WH_CALIB_PRU:
		if (!pCACF1WH->m_bModSelected)
		{
			strMsg.Format("%s Module Not Selected. Operation Abort", pCACF1WH->GetStnName());
			HMIMessageBox(MSG_OK, "WARNING", strMsg);
			bResult = FALSE;
		}
		else
		{
			pruACF_WHCalib.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF1WH;
			if (!pCACF1WH->CORCalibration(&pruACF_WHCalib, 0.2, FALSE, TRUE))
			{
				bResult = FALSE;
			}
			//else
			//{
			//	DOUBLE dMaxX = 0.0, dMaxY = 0.0, dMinX = 0.0, dMinY = 0.0;
			//	pCINWH->FindCORResultRange(&dMaxX, &dMinX, &dMaxY, &dMinY);
			//	strMsg.Format("COR X Max:%.2f Min:%.2f Range:%.2f", dMaxX, dMinX, fabs(dMaxX - dMinX));
			//	WriteHMIMess(strMsg);
			//	strMsg.Format("COR Y Max:%.2f Min:%.2f Range:%.2f", dMaxY, dMinY, fabs(dMaxY - dMinY));
			//	WriteHMIMess(strMsg);
			//}
		}
		break;

	case ACF2WH_CALIB_PRU:
		if (!pCACF2WH->m_bModSelected)
		{
			strMsg.Format("%s Module Not Selected. Operation Abort", pCACF2WH->GetStnName());
			HMIMessageBox(MSG_OK, "WARNING", strMsg);
			bResult = FALSE;
		}
		else
		{
			pruACF_WHCalib.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF2WH;
			if (!pCACF2WH->CORCalibration(&pruACF_WHCalib, 0.2, FALSE, TRUE))
			{
				bResult = FALSE;
			}
			//else
			//{
			//	DOUBLE dMaxX = 0.0, dMaxY = 0.0, dMinX = 0.0, dMinY = 0.0;
			//	pCINWH->FindCORResultRange(&dMaxX, &dMinX, &dMaxY, &dMinY);
			//	strMsg.Format("COR X Max:%.2f Min:%.2f Range:%.2f", dMaxX, dMinX, fabs(dMaxX - dMinX));
			//	WriteHMIMess(strMsg);
			//	strMsg.Format("COR Y Max:%.2f Min:%.2f Range:%.2f", dMaxY, dMinY, fabs(dMaxY - dMinY));
			//	WriteHMIMess(strMsg);
			//}
		}
		break;

	}

	if (bResult)
	{
		HMIMessageBox(MSG_OK, "AUTO CAL", "OPERATION SUCCESS");
	}
	else
	{
		HMIMessageBox(MSG_OK, "AUTO CAL", "OPERATION FAIL");
	}
	svMsg.InitMessage(sizeof(BOOL), &bCalInProgress);

	UpdateHMICalibPRButton(FALSE);

	return 1;
}

LONG CWinEagle::HMI_CheckPRCentre(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");

	INT nConvID = 0;
	IPC_CServiceMessage stMsg;
	LONG lTimeCount = 0;
	LONG lAnswer = -1;

	switch (HMI_ulCurPRU)
	{
	case ACF1WH_CALIB_PRU:
		pCACF1WH->CheckPRCentre(&pruACF_WHCalib);
		break;
	case ACF2WH_CALIB_PRU:
		pCACF2WH->CheckPRCentre(&pruACF_WHCalib);
		break;
	case ACFUL_CALIB_PRU:
		pCTA1->CheckPRCentre(&pruACFUL_Calib);
		break;

	}

	return 0;
}

LONG CWinEagle::HMI_SetCORCentre(IPC_CServiceMessage &svMsg)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	INT nConvID = 0;
	IPC_CServiceMessage stMsg;
	LONG lTimeCount = 0;
	CString strMsg(" ");

	switch (HMI_ulCurPRU)
	{
	case ACF1WH_CALIB_PRU:
		break;
	case ACF2WH_CALIB_PRU:
		break;
	}

	return 0;
}

LONG CWinEagle::HMI_CheckCORCentre(IPC_CServiceMessage &svMsg)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	INT nConvID = 0;
	IPC_CServiceMessage stMsg;
	LONG lTimeCount = 0;
	CString strMsg(" ");

	switch (HMI_ulCurPRU)
	{
	case ACF1WH_CALIB_PRU :
		pCACF1WH->CheckCORCentre(&pruACF_WHCalib , HMI_dTestCORAng, FALSE, TRUE);
		//{
		//	DOUBLE dMaxX = 0.0, dMaxY = 0.0, dMinX = 0.0, dMinY = 0.0;
		//	pCINWH->FindCORResultRange(&dMaxX, &dMinX, &dMaxY, &dMinY);
		//	strMsg.Format("COR X Max:%.2f Min:%.2f Range:%.2f", dMaxX, dMinX, fabs(dMaxX - dMinX));
		//	WriteHMIMess(strMsg);
		//	strMsg.Format("COR Y Max:%.2f Min:%.2f Range:%.2f", dMaxY, dMinY, fabs(dMaxY - dMinY));
		//	WriteHMIMess(strMsg);
		//}
		break;
	case ACF2WH_CALIB_PRU :
		pCACF2WH->CheckCORCentre(&pruACF_WHCalib , HMI_dTestCORAng, FALSE, TRUE);
		//{
		//	DOUBLE dMaxX = 0.0, dMaxY = 0.0, dMinX = 0.0, dMinY = 0.0;
		//	pCINWH->FindCORResultRange(&dMaxX, &dMinX, &dMaxY, &dMinY);
		//	strMsg.Format("COR X Max:%.2f Min:%.2f Range:%.2f", dMaxX, dMinX, fabs(dMaxX - dMinX));
		//	WriteHMIMess(strMsg);
		//	strMsg.Format("COR Y Max:%.2f Min:%.2f Range:%.2f", dMaxY, dMinY, fabs(dMaxY - dMinY));
		//	WriteHMIMess(strMsg);
		//}
		break;
	}

	return 0;
}

LONG CWinEagle::HMI_GetPRCentreMap(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");

	INT nConvID = 0;
	IPC_CServiceMessage stMsg;
	LONG lTimeCount = 0;

	if (g_ulUserGroup < SERVICE)
	{
		HMIMessageBox(MSG_OK, "WARNING", "YOU ARE NOT AUTHORIZED TO PERFORM THIS OPERATION!");	
	}
	else
	{
		switch (HMI_ulCurPRU)
		{
		case ACF1WH_CALIB_PRU:
			break;
		case ACF2WH_CALIB_PRU:
			break;
		}
	}

	HMIMessageBox(MSG_OK, "WARNING", "OPERATION TERMINATED!");

	return 0;
}

#if 1 //20150314
LONG CWinEagle::HMI_ZoomInOnly(IPC_CServiceMessage &svMsg) //for zoom and pan
{
	CWinEagle *pCWinEagle = (CWinEagle*)m_pModule->GetStation("WinEagle");

	g_stCursor.prCoord.x = PR_DEF_CENTRE_X;
	g_stCursor.prCoord.y = PR_DEF_CENTRE_Y;

	ZoomCentre.x = PR_DEF_CENTRE_X;
	ZoomCentre.y = PR_DEF_CENTRE_Y;

	if (pCWinEagle->m_lCurrZoomPanFactor != PR_ZOOM_FACTOR_8X)
	{
		if (pCWinEagle->m_lCurrZoomPanFactor == PR_ZOOM_FACTOR_END) //no zoomPan
		{
			pCWinEagle->m_lCurrZoomPanFactor = PR_ZOOM_FACTOR_1X;
		}
		else if (pCWinEagle->m_lCurrZoomPanFactor == PR_ZOOM_FACTOR_1X)
		{
			pCWinEagle->m_lCurrZoomPanFactor = PR_ZOOM_FACTOR_2X;
		}
		else if (pCWinEagle->m_lCurrZoomPanFactor == PR_ZOOM_FACTOR_2X)
		{
			pCWinEagle->m_lCurrZoomPanFactor = PR_ZOOM_FACTOR_4X;
		}
		else if (pCWinEagle->m_lCurrZoomPanFactor == PR_ZOOM_FACTOR_4X)
		{
			pCWinEagle->m_lCurrZoomPanFactor = PR_ZOOM_FACTOR_8X;
		}
		else
		{
			pCWinEagle->m_lCurrZoomPanFactor = PR_ZOOM_FACTOR_1X;
		}

		//PRS_DigitalZoom(nCurrentCameraID, pCWinEagle->m_lCurrZoomPanFactor, ZoomCentre);
		/*
		typedef enum {	PR_ZOOM_FACTOR_1X,
		PR_ZOOM_FACTOR_2X,
		PR_ZOOM_FACTOR_4X,
		PR_ZOOM_FACTOR_8X,
		PR_ZOOM_FACTOR_END	} PR_ZOOM_FACTOR;
		*/
		PRS_SetDisplayMagnifyFactor(nCurrentCameraID, PR_ZOOM_MODE_IN, pCWinEagle->m_lCurrZoomPanFactor, ZoomCentre);
		PRS_DrawCross(nCurrentCameraID, g_stCursor.prCoord, g_stCursor.lSize, g_stCursor.lColor);

		if (pCWinEagle->m_lCurrZoomPanFactor == PR_ZOOM_FACTOR_1X)
		{
			PRS_DisplayText(nCurrentCameraID, 2, 0, "Cam: 2X Image");
		}
		else if (pCWinEagle->m_lCurrZoomPanFactor == PR_ZOOM_FACTOR_2X)
		{
			PRS_DisplayText(nCurrentCameraID, 2, 0, "Cam: 4X Image");
		}
		else if (pCWinEagle->m_lCurrZoomPanFactor == PR_ZOOM_FACTOR_4X)
		{
			PRS_DisplayText(nCurrentCameraID, 2, 0, "Cam: 8X Image");
		}
		else if (pCWinEagle->m_lCurrZoomPanFactor == PR_ZOOM_FACTOR_8X)
		{
			PRS_DisplayText(nCurrentCameraID, 2, 0, "Cam: 16X Image");
		}

		//HMI_bShowCursorControl = TRUE;

	}

	//switch (g_ulWizardStep)
	//{
	//case 2:
	//	PRS_DisplayText(nCurrentCameraID, 1, PRS_MSG_ROW1, "Please Set Align Pt 1");
	//	break;

	//case 3:
	//	PRS_DisplayText(nCurrentCameraID, 1, PRS_MSG_ROW1, "Please Set Align Pt 2");
	//	break;
	//}

	return 0;
}

LONG CWinEagle::HMI_ZoomOutOnly(IPC_CServiceMessage &svMsg) //for zoom and pan
{
	CWinEagle *pCWinEagle = (CWinEagle*)m_pModule->GetStation("WinEagle");

	g_stCursor.prCoord.x = PR_DEF_CENTRE_X;
	g_stCursor.prCoord.y = PR_DEF_CENTRE_Y;
	//
	ZoomCentre.x = PR_DEF_CENTRE_X;
	ZoomCentre.y = PR_DEF_CENTRE_Y;

	if (pCWinEagle->m_lCurrZoomPanFactor != PR_ZOOM_FACTOR_1X && pCWinEagle->m_lCurrZoomPanFactor != PR_ZOOM_FACTOR_END)
	{
		if (pCWinEagle->m_lCurrZoomPanFactor == PR_ZOOM_FACTOR_8X)
		{
			pCWinEagle->m_lCurrZoomPanFactor = PR_ZOOM_FACTOR_4X;
		}
		else if (pCWinEagle->m_lCurrZoomPanFactor == PR_ZOOM_FACTOR_4X)
		{
			pCWinEagle->m_lCurrZoomPanFactor = PR_ZOOM_FACTOR_2X;
		}
		else if (pCWinEagle->m_lCurrZoomPanFactor == PR_ZOOM_FACTOR_2X)
		{
			pCWinEagle->m_lCurrZoomPanFactor = PR_ZOOM_FACTOR_1X;
		}

		//PRS_DigitalZoom(nCurrentCameraID, pCWinEagle->m_lCurrZoomFactor, ZoomCentre);
		//PRS_DrawCross(nCurrentCameraID, g_stCursor.prCoord, g_stCursor.lSize, g_stCursor.lColor);
		/*
		typedef enum {	PR_ZOOM_FACTOR_1X,
		PR_ZOOM_FACTOR_2X,
		PR_ZOOM_FACTOR_4X,
		PR_ZOOM_FACTOR_8X,
		PR_ZOOM_FACTOR_END	} PR_ZOOM_FACTOR;
		*/
		PRS_SetDisplayMagnifyFactor(nCurrentCameraID, PR_ZOOM_MODE_OUT, pCWinEagle->m_lCurrZoomPanFactor, ZoomCentre);

		if (pCWinEagle->m_lCurrZoomPanFactor == PR_ZOOM_FACTOR_4X)
		{
			PRS_DisplayText(nCurrentCameraID, 2, 0, "Cam: 8X Image");
		}
		else if (pCWinEagle->m_lCurrZoomPanFactor == PR_ZOOM_FACTOR_2X)
		{
			PRS_DisplayText(nCurrentCameraID, 2, 0, "Cam: 4X Image");
		}
		else if (pCWinEagle->m_lCurrZoomPanFactor == PR_ZOOM_FACTOR_1X)
		{
			PRS_DisplayText(nCurrentCameraID, 2, 0, "Cam: 2X Image");
		}
	}
	else
	{
		if (pCWinEagle->m_lCurrZoomPanFactor != PR_ZOOM_FACTOR_END)
		{
			pCWinEagle->m_lCurrZoomPanFactor = PR_ZOOM_FACTOR_END;
			PRS_DisableDigitalZoom(nCurrentCameraID);
			PRS_DrawHomeCursor(nCurrentCameraID, FALSE);

			g_stCursor.prCoord.x = PR_DEF_CENTRE_X;
			g_stCursor.prCoord.y = PR_DEF_CENTRE_Y;
		}
	}

	return 0;
}
#endif

//Digital Zoom
LONG CWinEagle::HMI_ZoomIn(IPC_CServiceMessage &svMsg)
{
	CWinEagle *pCWinEagle = (CWinEagle*)m_pModule->GetStation("WinEagle");

	g_stCursor.prCoord.x = PR_DEF_CENTRE_X;
	g_stCursor.prCoord.y = PR_DEF_CENTRE_Y;

	ZoomCentre.x = PR_DEF_CENTRE_X;
	ZoomCentre.y = PR_DEF_CENTRE_Y;

	if (pCWinEagle->m_lCurrZoomFactor != PR_ZOOM_FACTOR_8X)
	{
		if (pCWinEagle->m_lCurrZoomFactor == PR_ZOOM_FACTOR_1X)
		{
			pCWinEagle->m_lCurrZoomFactor = PR_ZOOM_FACTOR_2X;
		}
		else if (pCWinEagle->m_lCurrZoomFactor == PR_ZOOM_FACTOR_2X)
		{
			pCWinEagle->m_lCurrZoomFactor = PR_ZOOM_FACTOR_4X;
		}
		else if (pCWinEagle->m_lCurrZoomFactor == PR_ZOOM_FACTOR_4X)
		{
			pCWinEagle->m_lCurrZoomFactor = PR_ZOOM_FACTOR_8X;
		}

		PRS_DigitalZoom(nCurrentCameraID, pCWinEagle->m_lCurrZoomFactor, ZoomCentre);
		PRS_DrawCross(nCurrentCameraID, g_stCursor.prCoord, g_stCursor.lSize, g_stCursor.lColor);
	
		if (pCWinEagle->m_lCurrZoomFactor == PR_ZOOM_FACTOR_2X)
		{
			PRS_DisplayText(nCurrentCameraID, 2, 0, "Cam: 2X Image");
		}
		else if (pCWinEagle->m_lCurrZoomFactor == PR_ZOOM_FACTOR_4X)
		{
			PRS_DisplayText(nCurrentCameraID, 2, 0, "Cam: 4X Image");
		}
		else if (pCWinEagle->m_lCurrZoomFactor == PR_ZOOM_FACTOR_8X)
		{
			PRS_DisplayText(nCurrentCameraID, 2, 0, "Cam: 8X Image");
		}

		HMI_bShowCursorControl = TRUE;

	}
	
	switch (g_ulWizardStep)
	{
	case 2:
		PRS_DisplayText(nCurrentCameraID, 1, PRS_MSG_ROW1, "Please Set Align Pt 1");
		break;

	case 3:
		PRS_DisplayText(nCurrentCameraID, 1, PRS_MSG_ROW1, "Please Set Align Pt 2");
		break;
	}

	return 0;
}

LONG CWinEagle::HMI_ZoomOut(IPC_CServiceMessage &svMsg)
{
	CWinEagle *pCWinEagle = (CWinEagle*)m_pModule->GetStation("WinEagle");

	g_stCursor.prCoord.x = PR_DEF_CENTRE_X;
	g_stCursor.prCoord.y = PR_DEF_CENTRE_Y;
	
	ZoomCentre.x = PR_DEF_CENTRE_X;
	ZoomCentre.y = PR_DEF_CENTRE_Y;

	if (pCWinEagle->m_lCurrZoomFactor != PR_ZOOM_FACTOR_2X && pCWinEagle->m_lCurrZoomFactor != PR_ZOOM_FACTOR_1X)
	{
		if (pCWinEagle->m_lCurrZoomFactor == PR_ZOOM_FACTOR_8X)
		{
			pCWinEagle->m_lCurrZoomFactor = PR_ZOOM_FACTOR_4X;
		}
		else if (pCWinEagle->m_lCurrZoomFactor == PR_ZOOM_FACTOR_4X)
		{
			pCWinEagle->m_lCurrZoomFactor = PR_ZOOM_FACTOR_2X;
		}
	
		PRS_DigitalZoom(nCurrentCameraID, pCWinEagle->m_lCurrZoomFactor, ZoomCentre);
		PRS_DrawCross(nCurrentCameraID, g_stCursor.prCoord, g_stCursor.lSize, g_stCursor.lColor);

		if (pCWinEagle->m_lCurrZoomFactor == PR_ZOOM_FACTOR_4X)
		{
			PRS_DisplayText(nCurrentCameraID, 2, 0, "Cam: 4X Image");
		}
		else if (pCWinEagle->m_lCurrZoomFactor == PR_ZOOM_FACTOR_2X)
		{
			PRS_DisplayText(nCurrentCameraID, 2, 0, "Cam: 2X Image");
		}
	}
	else
	{
		if (pCWinEagle->m_lCurrZoomFactor != PR_ZOOM_FACTOR_1X)
		{
			pCWinEagle->m_lCurrZoomFactor = PR_ZOOM_FACTOR_1X;
			PRS_DisableDigitalZoom(nCurrentCameraID);
			PRS_DrawHomeCursor(nCurrentCameraID, FALSE);

			g_stCursor.prCoord.x = PR_DEF_CENTRE_X;
			g_stCursor.prCoord.y = PR_DEF_CENTRE_Y;
		}
	}

	
	switch (g_ulWizardStep)
	{
	case 2:
		PRS_DisplayText(nCurrentCameraID, 1, PRS_MSG_ROW1, "Please Set Align Pt 1");
		break;

	case 3:
		PRS_DisplayText(nCurrentCameraID, 1, PRS_MSG_ROW1, "Please Set Align Pt 2");
		break;
	}

	return 0;
}

LONG CWinEagle::HMI_ToggleVAC(IPC_CServiceMessage &svMsg)
{

	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");

	// protection
	if (HMI_ulCurPRU >= MAX_NUM_OF_CALIB_PRU)
	{
		HMI_ulCurPRU = ACF1WH_CALIB_PRU ;
	}

	switch (HMI_ulCurPRU)
	{
	case ACF1WH_CALIB_PRU :
		//pCINWH->MoveZ(WHZ_1, INWH_Z_CTRL_GO_PR_CALIB_LEVEL,GMP_WAIT);
		break;
	}

	UpdateHMICalibPRButton(FALSE);
	return 0;
}

LONG CWinEagle::HMI_ResetIncludedAngle(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");

	pPRU = &SelectCurPRU(HMI_ulCurPRU);


	switch (HMI_ulCurPRU)
	{
	case ACF1WH_CALIB_PRU :
		break;
	}
	UpdateHMICalibPRButton(FALSE);
	return 0;
}

LONG CWinEagle::HMI_SetUserSearchPRContinue(IPC_CServiceMessage &svMsg)	// 20140815 Yip
{
	m_bUserSearchPRDone = TRUE;
	g_bShowUserSearchPRDone = FALSE;
	return 0;
}

LONG CWinEagle::HMI_SetUserRejectPRTask(IPC_CServiceMessage &svMsg)	// 20140815 Yip
{
	m_bUserRejectPRTask = TRUE;
	g_bShowUserSearchPRDone = FALSE;

	BOOL bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}

VOID CWinEagle::PrintPRCalibSetup(FILE *fp)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString str = "";
	
	if (fp != NULL)
	{
		fprintf(fp, "\n<<X-Y Calibration>>\n");

		for (INT i = 0; i < GetmyCalibPRUList().GetSize(); i++) //20130109 CArray is start from zero. myCalibPRUList is start from zero. But myPRUList is start from 1, because there is a dummy item at index zero (pruMenu)
		{
			if ((BOOL)(LONG)pAppMod->m_smfMachine[GetmyCalibPRUList().GetAt(i)->szStnName]["PRU"][GetmyCalibPRUList().GetAt(i)->szFileName]["bCalibrated"])
			{
				fprintf(fp, "\n%s CALIBRATION\n", (const char*)GetmyCalibPRUList().GetAt(i)->szFileName);
			}
			else
			{
				fprintf(fp, "\n%s NOT CALIBRATION\n", (const char*)GetmyCalibPRUList().GetAt(i)->szFileName);
			}
			fprintf(fp, "======================================================\n");
		}
	}
}

VOID CWinEagle::PrintPRSetup(FILE *fp)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CString str = "";
	
	if (fp != NULL)
	{
		fprintf(fp, "\n<<PR Record>>\n");

		for (INT i = 1; i < GetmyPRUList().GetSize(); i++)
		{
			if ((BOOL)(LONG)pAppMod->m_smfDevice[GetmyPRUList().GetAt(i)->szStnName]["PRU"][GetmyPRUList().GetAt(i)->szFileName]["bLoaded"])
			{
				fprintf(fp, "\n%s\n", (const char*)GetmyPRUList().GetAt(i)->szFileName);
				fprintf(fp, "======================================================\n");
				switch ((LONG)pAppMod->m_smfDevice[GetmyPRUList().GetAt(i)->szStnName]["PRU"][GetmyPRUList().GetAt(i)->szFileName]["emRecordType"])
				{
				case 0:
					fprintf(fp, "Alignment Algorithm		: Pattern Matching");
					break;
				case 1:
					fprintf(fp, "Alignment Algorithm		: Edge Matching");
					break;
				case 2:
					fprintf(fp, "Alignment Algorithm		: Circle(Shape) Fitting");
					break;
				case 3:
					fprintf(fp, "Alignment Algorithm		:Template Fitting");
					break;
				case 4:
					fprintf(fp, "Alignment Algorithm		:Fidiciual Mark");
					break;
				case 5:
					fprintf(fp, "Alignment Algorithm		:Rectangle");
					break;
				}

				// Postbond Only
				if (GetmyPRUList().GetAt(i)->emID >= ID_INSP1_ACFPB1_TYPE1 && GetmyPRUList().GetAt(i)->emID <= ID_INSP2_ACFPB2_BACKGROUND_TYPE8)
				{
					fprintf(fp, "%30s\t : %.2f\n", "Defect Tolerance",
							(DOUBLE)pAppMod->m_smfDevice[GetmyPRUList().GetAt(i)->szStnName]["PRU"][GetmyPRUList().GetAt(i)->szFileName]["dPostBdRejectTol"]);
					fprintf(fp, "%30s\t : %.2f\n", "Defect Threshold",
							(DOUBLE)pAppMod->m_smfDevice[GetmyPRUList().GetAt(i)->szStnName]["PRU"][GetmyPRUList().GetAt(i)->szFileName]["stSrchACFCmd"]["stACFInsp"]["ubDefectThreshold"]);

					switch ((LONG)pAppMod->m_smfDevice[GetmyPRUList().GetAt(i)->szStnName]["PRU"][GetmyPRUList().GetAt(i)->szFileName]["stSrchACFCmd"]["stACFInsp"]["emDefectAttribute"])
					{
					case 0:
						fprintf(fp, "Defect Attribute		: Bright\n");
						break;
					case 1:
						fprintf(fp, "Defect Attribute		: Dark\n");
						break;
					case 2:
						fprintf(fp, "Defect Attribute		: Both\n");
						break;
					}
				}

				fprintf(fp, "%30s\t : %.2f\n",	"Match Score",
						(DOUBLE)pAppMod->m_smfDevice[GetmyPRUList().GetAt(i)->szStnName]["PRU"][GetmyPRUList().GetAt(i)->szFileName]["stSrchDieCmd"]["rMatchScore"]);
				fprintf(fp, "%30s\t : %.2f\n",	"Angle Range",
						(DOUBLE)pAppMod->m_smfDevice[GetmyPRUList().GetAt(i)->szStnName]["PRU"][GetmyPRUList().GetAt(i)->szFileName]["stSrchDieCmd"]["rEndAngle"]);
			}
			else
			{
				fprintf(fp, "\n%s NOT LOADED\n", (LPCTSTR)GetmyPRUList().GetAt(i)->szFileName); //klocwork fix 20121211
			}

		}
	}
}

#ifdef EXCEL_MACHINE_DEVICE_INFO
BOOL CWinEagle::printMachineDeviceInfo()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bResult = TRUE;
	CString szBlank("--");
	CString szModule = GetStnName();
	LONG lValue = 0;
	DOUBLE dValue = 0.0;

	for (INT i = 0; i < GetmyCalibPRUList().GetSize(); i++) //20130109 CArray is start from zero. myCalibPRUList is start from zero. But myPRUList is start from 1, because there is a dummy item at index zero (pruMenu)
	{
		printPRPara(GetmyCalibPRUList().GetAt(i)->szStnName, GetmyCalibPRUList().GetAt(i), GetmyCalibPRUList().GetAt(i)->szFileName, TRUE);
	}

	//for(INT i = 1; i<GetmyPRUList().GetSize(); i++) //device
	//{
	//	printPRPara(GetmyPRUList().GetAt(i)->szStnName, GetmyPRUList().GetAt(i), GetmyPRUList().GetAt(i)->szFileName, FALSE);
	//}

	if (bResult)
	{
		bResult = CAC9000Stn::printMachineDeviceInfo();
	}
	return bResult;
}

LONG CWinEagle::printPRPara(CString szStation, PRU *pPRU, CString szPRU, BOOL IsMachineData) // The print excel
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CStringMapFile *pMap;

	BOOL bResult = TRUE;
	CString szBlank("--");
	CString szModule = GetStnName();
	LONG lValue = 0;
	DOUBLE dValue = 0.0;

	if (IsMachineData)
	{
		pMap = &pAppMod->m_smfMachine;
	}
	else
	{
		pMap = &pAppMod->m_smfDevice;
	}

	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("uwCoaxialLevel"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["uwCoaxialLevel"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("uwCoaxial1Level"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["uwCoaxial1Level"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("uwCoaxial2Level"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["uwCoaxial2Level"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("uwRingLevel"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["uwRingLevel"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("uwRing1Level"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["uwRing1Level"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("uwSideLevel"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["uwSideLevel"]
					   );
   // 20141023
   printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("uwSide1Level"),       
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["uwSide1Level"]
					   );
   printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("uwBackLevel"),       
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["uwBackLevel"]
					   );
   printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("uwBack1Level"),       
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["uwBack1Level"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("bLoaded"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["bLoaded"]	
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("bStatLoaded"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["bStatLoaded"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("lPBMethod"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["lPBMethod"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("dPostBdRejectTol"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["dPostBdRejectTol"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("emRecordType"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["emRecordType"]
					   );

	// calibration PRU
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("arcoLrnDieCorners[PR_UPPER_RIGHT].x"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_RIGHT].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("arcoLrnDieCorners[PR_UPPER_RIGHT].y"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_RIGHT].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("arcoLrnDieCorners[PR_UPPER_LEFT].x"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_LEFT].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("arcoLrnDieCorners[PR_UPPER_LEFT].y"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_LEFT].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("arcoLrnDieCorners[PR_LOWER_RIGHT].x"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_RIGHT].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("arcoLrnDieCorners[PR_LOWER_RIGHT].y"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_RIGHT].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("arcoLrnDieCorners[PR_LOWER_LEFT].x"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_LEFT].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("arcoLrnDieCorners[PR_LOWER_LEFT].y"),
						szBlank, szBlank, szBlank, szBlank, szBlank, (LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_LEFT].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("arcoDieCorners[PR_UPPER_LEFT].x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["arcoDieCorners[PR_UPPER_LEFT].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("arcoDieCorners[PR_UPPER_LEFT].Y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["arcoDieCorners[PR_UPPER_LEFT].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("arcoDieCorners[PR_LOWER_RIGHT].x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["arcoDieCorners[PR_LOWER_RIGHT].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("arcoDieCorners[PR_LOWER_RIGHT].y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["arcoDieCorners[PR_LOWER_RIGHT].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("emAlignAlg"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["emAlignAlg"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("astChipWindows[0].coCorner1.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[0].coCorner1.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("astChipWindows[0].coCorner1.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[0].coCorner1.y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("astChipWindows[0].coCorner2.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[0].coCorner2.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("astChipWindows[0].coCorner2.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[0].coCorner2.y"]
					   );

	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("astChipWindows[1].coCorner1.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[1].coCorner1.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("astChipWindows[1].coCorner1.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[1].coCorner1.y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("astChipWindows[1].coCorner2.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[1].coCorner2.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("astChipWindows[1].coCorner2.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[1].coCorner2.y"]
					   );

	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("leChipIntrusiveLength"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["leChipIntrusiveLength"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("emIsAutoLearnFMarkCenterAngle"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["emIsAutoLearnFMarkCenterAngle"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("emFiducialMarkType"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["emFiducialMarkType"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stManualLrnDieCmd"),
						CString("ubThreshold"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["ubThreshold"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("emAlignAlg"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["emAlignAlg"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("stLargeSrchWin.coCorner1.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stLargeSrchWin.coCorner1.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("stLargeSrchWin.coCorner1.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stLargeSrchWin.coCorner1.y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("stLargeSrchWin.coCorner2.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stLargeSrchWin.coCorner2.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("stLargeSrchWin.coCorner2.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stLargeSrchWin.coCorner2.y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("rMatchScore"), szBlank, szBlank, szBlank, szBlank, 
						(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["rMatchScore"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("rStartAngle"), szBlank, szBlank, szBlank, szBlank, 
						(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["rStartAngle"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("rEndAngle"), szBlank, szBlank, szBlank, szBlank, 
						(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["rEndAngle"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("stDieInsp"), CString("stTmpl"), CString("emDefectAttribute"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieInsp"]["stTmpl"]["emDefectAttribute"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("stDieInsp"), CString("stTmpl"), CString("ubDefectThreshold"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieInsp"]["stTmpl"]["ubDefectThreshold"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("coProbableDieCentre.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["coProbableDieCentre.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("coProbableDieCentre.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["coProbableDieCentre.y"]
					   );

	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("stDieAlignPar"), CString("ubMaxScale"), szBlank, szBlank, szBlank, 
						(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieAlignPar"]["ubMaxScale"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("stDieAlignPar"), CString("ubMinScale"), szBlank, szBlank, szBlank, 
						(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieAlignPar"]["ubMinScale"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("stPaLn.emIsCoarseSrch"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stPaLn.emIsCoarseSrch"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("stPaLn.stLargeSrchWin.coCorner1.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stPaLn.stLargeSrchWin.coCorner1.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("stPaLn.stLargeSrchWin.coCorner1.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stPaLn.stLargeSrchWin.coCorner1.y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("stPaLn.stLargeSrchWin.coCorner2.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stPaLn.stLargeSrchWin.coCorner2.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("stPaLn.stLargeSrchWin.coCorner2.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stPaLn.stLargeSrchWin.coCorner2.y"]
					   );

	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchDieCmd"),
						CString("rCfmPatternMatchScore"), szBlank, szBlank, szBlank, szBlank, 
						(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["rCfmPatternMatchScore"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnShapeCmd"),
						CString("coCorner1.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnShapeCmd"]["coCorner1.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnShapeCmd"),
						CString("coCorner1.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnShapeCmd"]["coCorner1.y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnShapeCmd"),
						CString("coCorner2.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnShapeCmd"]["coCorner2.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnShapeCmd"),
						CString("coCorner2.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnShapeCmd"]["coCorner2.y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnShapeCmd"),
						CString("emObjAttribute"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnShapeCmd"]["emObjAttribute"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stDetectShapeCmd"),
						CString("stInspWin.coCorner1.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["stInspWin.coCorner1.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stDetectShapeCmd"),
						CString("stInspWin.coCorner1.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["stInspWin.coCorner1.y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stDetectShapeCmd"),
						CString("stInspWin.coCorner2.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["stInspWin.coCorner2.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stDetectShapeCmd"),
						CString("stInspWin.coCorner2.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["stInspWin.coCorner2.y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stDetectShapeCmd"),
						CString("rDetectScore"), szBlank, szBlank, szBlank, szBlank, 
						(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["rDetectScore"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stDetectShapeCmd"),
						CString("rSizePercentVar"), szBlank, szBlank, szBlank, szBlank, 
						(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["rSizePercentVar"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stDetectShapeCmd"),
						CString("rStartAngle"), szBlank, szBlank, szBlank, szBlank, 
						(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["rStartAngle"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stDetectShapeCmd"),
						CString("rEndAngle"), szBlank, szBlank, szBlank, szBlank, 
						(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["rEndAngle"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLoadTmplCmd"),
						CString("coTmplCentre.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLoadTmplCmd"]["coTmplCentre.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLoadTmplCmd"),
						CString("coTmplCentre.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLoadTmplCmd"]["coTmplCentre.y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLoadTmplCmd"),
						CString("szTmplSize.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLoadTmplCmd"]["szTmplSize.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLoadTmplCmd"),
						CString("szTmplSize.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLoadTmplCmd"]["szTmplSize.y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLoadTmplCmd"),
						CString("ubPassScore"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["ubPassScore"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchTmplCmd"),
						CString("stSrchWin.coCorner1.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["stSrchWin.coCorner1.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchTmplCmd"),
						CString("stSrchWin.coCorner1.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["stSrchWin.coCorner1.y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchTmplCmd"),
						CString("stSrchWin.coCorner2.x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["stSrchWin.coCorner2.x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchTmplCmd"),
						CString("stSrchWin.coCorner2.y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["stSrchWin.coCorner2.y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stImageStatInfoCmd"),
						CString("acoCorners[0].x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[0].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stImageStatInfoCmd"),
						CString("acoCorners[0].y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[0].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stImageStatInfoCmd"),
						CString("acoCorners[1].x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[1].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stImageStatInfoCmd"),
						CString("acoCorners[1].y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[1].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stImageStatInfoCmd"),
						CString("acoCorners[2].x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[2].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stImageStatInfoCmd"),
						CString("acoCorners[2].y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[2].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stImageStatInfoCmd"),
						CString("acoCorners[3].x"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[3].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stImageStatInfoCmd"),
						CString("acoCorners[3].y"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[3].y"]
					   );

	//New For Substraction
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("emACFSegmentAlg"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emACFSegmentAlg"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("emACFAlignAlg"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emACFAlignAlg"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("emACFAttribute"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emACFAttribute"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("emACFPosition"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emACFPosition"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("emBackgroundType"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emBackgroundType"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("ubSegThreshold"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["ubSegThreshold"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("emACFInspAlg"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emACFInspAlg"]
					   );
//learn Region
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stACFRegion"), CString("uwNumOfCorners"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["uwNumOfCorners"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stACFRegion"), CString("acoCorners[0].x"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[0].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stACFRegion"), CString("acoCorners[0].y"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[0].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stACFRegion"), CString("acoCorners[1].x"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[1].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stACFRegion"), CString("acoCorners[1].y"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[1].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stACFRegion"), CString("acoCorners[2].x"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[2].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stACFRegion"), CString("acoCorners[2].y"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[2].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stACFRegion"), CString("acoCorners[3].x"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[3].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stACFRegion"), CString("acoCorners[3].y"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[3].y"]
					   );

	//Search Region
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stSrchRegion"), CString("uwNumOfCorners"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["uwNumOfCorners"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stSrchRegion"), CString("acoCorners[0].x"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[0].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stSrchRegion"), CString("acoCorners[0].y"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[0].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stSrchRegion"), CString("acoCorners[1].x"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[1].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stSrchRegion"), CString("acoCorners[1].y"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[1].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stSrchRegion"), CString("acoCorners[2].x"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[2].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stSrchRegion"), CString("acoCorners[2].y"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[2].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stSrchRegion"), CString("acoCorners[3].x"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[3].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stSrchRegion"), CString("acoCorners[3].y"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[3].y"]
					   );

	//Detect Region
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stDefectMultRegion"), CString("uwNMultRegion"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"]["uwNMultRegion"]
					   );

	LONG lTotalDefectRegion = pPRU->stLrnACFCmd.stDefectMultRegion.uwNMultRegion;
	for (INT i = 0; i < lTotalDefectRegion; i++)
	{
		CString szRegion;
		szRegion.Format("%ld", lTotalDefectRegion - 1);
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
							CString("stDefectMultRegion"), szRegion, CString("uwNumOfCorners"), szBlank, szBlank, 
							(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["uwNumOfCorners"]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
							CString("stDefectMultRegion"), szRegion, CString("acoCorners[0].x"), szBlank, szBlank, 
							(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[0].x"]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
							CString("stDefectMultRegion"), szRegion, CString("acoCorners[0].y"), szBlank, szBlank, 
							(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[0].y"]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
							CString("stDefectMultRegion"), szRegion, CString("acoCorners[1].x"), szBlank, szBlank, 
							(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[1].x"]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
							CString("stDefectMultRegion"), szRegion, CString("acoCorners[1].y"), szBlank, szBlank, 
							(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[1].y"]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
							CString("stDefectMultRegion"), szRegion, CString("acoCorners[2].x"), szBlank, szBlank, 
							(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[2].x"]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
							CString("stDefectMultRegion"), szRegion, CString("acoCorners[2].y"), szBlank, szBlank, 
							(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[2].y"]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
							CString("stDefectMultRegion"), szRegion, CString("acoCorners[3].x"), szBlank, szBlank, 
							(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[3].x"]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
							CString("stDefectMultRegion"), szRegion, CString("acoCorners[3].y"), szBlank, szBlank, 
							(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[3].y"]
						   );
	}	

	//ChipArea
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stChipMultRegion"), CString("astRegion[0]"), CString("acoCorners[0].x"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[0].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stChipMultRegion"), CString("astRegion[0]"), CString("acoCorners[0].y"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[0].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stChipMultRegion"), CString("astRegion[0]"), CString("acoCorners[1].x"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[1].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stChipMultRegion"), CString("astRegion[0]"), CString("acoCorners[1].y"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[1].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stChipMultRegion"), CString("astRegion[0]"), CString("acoCorners[2].x"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[2].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stChipMultRegion"), CString("astRegion[0]"), CString("acoCorners[2].y"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[2].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stChipMultRegion"), CString("astRegion[0]"), CString("acoCorners[3].x"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[3].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stChipMultRegion"), CString("astRegion[0]"), CString("acoCorners[3].y"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[3].y"]
					   );

	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stChipMultRegion"), CString("astRegion[1]"), CString("acoCorners[0].x"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[0].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stChipMultRegion"), CString("astRegion[1]"), CString("acoCorners[0].y"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[0].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stChipMultRegion"), CString("astRegion[1]"), CString("acoCorners[1].x"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[1].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stChipMultRegion"), CString("astRegion[1]"), CString("acoCorners[1].y"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[1].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stChipMultRegion"), CString("astRegion[1]"), CString("acoCorners[2].x"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[2].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stChipMultRegion"), CString("astRegion[1]"), CString("acoCorners[2].y"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[2].y"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stChipMultRegion"), CString("astRegion[1]"), CString("acoCorners[3].x"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[3].x"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stLrnACFCmd"),
						CString("stChipMultRegion"), CString("astRegion[1]"), CString("acoCorners[3].y"), szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[3].y"]
					   );

	//Srch ACF
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchACFCmd"),
						CString("stACFInsp"), CString("aeMinChipArea"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["stACFInsp"]["aeMinChipArea"]
					   );

	for (INT i = 0; i < PR_MAX_REGION; i++)
	{
		CString szStr;
		szStr.Format("%ld", i);

		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchACFCmd"),
							CString("stACFInsp"), CString("aeMinSingleDefectArea"), szStr, szBlank, szBlank, 
							(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["stACFInsp"]["aeMinSingleDefectArea"][i]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchACFCmd"),
							CString("stACFInsp"), CString("aeMinTotalDefectArea"), szStr, szBlank, szBlank, 
							(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["stACFInsp"]["aeMinTotalDefectArea"][i]
						   );
	}
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchACFCmd"),
						CString("stACFInsp"), CString("emDefectAttribute"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["stACFInsp"]["emDefectAttribute"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchACFCmd"),
						CString("stACFInsp"), CString("ubDefectThreshold"), szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["stACFInsp"]["ubDefectThreshold"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchACFCmd"),
						CString("rMatchScore"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["rMatchScore"]
					   );
	printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("stSrchACFCmd"),
						CString("rAngleRange"), szBlank, szBlank, szBlank, szBlank, 
						(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["rAngleRange"]
					   );
	if (IsMachineData)
	{
		// calibration data
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("bCalibrated"),
							szBlank, szBlank, szBlank, szBlank, szBlank, 
							(LONG)(*pMap)[szStation]["PRU"][szPRU]["bCalibrated"]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("m2p_xform.a"),
							szBlank, szBlank, szBlank, szBlank, szBlank, 
							(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform.a"]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("m2p_xform.b"),
							szBlank, szBlank, szBlank, szBlank, szBlank, 
							(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform.b"]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("m2p_xform.c"),
							szBlank, szBlank, szBlank, szBlank, szBlank, 
							(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform.c"]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("m2p_xform.d"),
							szBlank, szBlank, szBlank, szBlank, szBlank, 
							(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform.d"]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("p2m_xform.a"),
							szBlank, szBlank, szBlank, szBlank, szBlank, 
							(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform.a"]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("p2m_xform.b"),
							szBlank, szBlank, szBlank, szBlank, szBlank, 
							(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform.b"]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("p2m_xform.c"),
							szBlank, szBlank, szBlank, szBlank, szBlank, 
							(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform.c"]
						   );
		printMachineInfoRow(szModule, szStation, CString("PRU"), szPRU, CString("p2m_xform.d"),
							szBlank, szBlank, szBlank, szBlank, szBlank, 
							(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform.d"]
						   );
	}
	return 1;
}
#endif

LONG CWinEagle::SavePRPara(CString szStation, PRU *pPRU, CString szPRU, BOOL IsMachineData) // The save file Name
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	
	CStringMapFile *pMap;
	if (IsMachineData)
	{
		pMap = &pAppMod->m_smfMachine;
	}
	else
	{
		pMap = &pAppMod->m_smfDevice;
	}

	// calibration PRU
	(*pMap)[szStation]["PRU"][szPRU]["uwCoaxialLevel"]	= (LONG)pPRU->uwCoaxialLevel;
	(*pMap)[szStation]["PRU"][szPRU]["uwCoaxial1Level"]	= (LONG)pPRU->uwCoaxial1Level;
	(*pMap)[szStation]["PRU"][szPRU]["uwCoaxial2Level"]	= (LONG)pPRU->uwCoaxial2Level;
	(*pMap)[szStation]["PRU"][szPRU]["uwRingLevel"]		= (LONG)pPRU->uwRingLevel;
	(*pMap)[szStation]["PRU"][szPRU]["uwRing1Level"]	= (LONG)pPRU->uwRing1Level;
	(*pMap)[szStation]["PRU"][szPRU]["uwSideLevel"]		= (LONG)pPRU->uwSideLevel;
	(*pMap)[szStation]["PRU"][szPRU]["uwSide1Level"]	= (LONG)pPRU->uwSide1Level;		// 20141023
	(*pMap)[szStation]["PRU"][szPRU]["uwBackLevel"]		= (LONG)pPRU->uwBackLevel;		// 20141023
	(*pMap)[szStation]["PRU"][szPRU]["uwBack1Level"]	= (LONG)pPRU->uwBack1Level;		// 20141023
	(*pMap)[szStation]["PRU"][szPRU]["bLoaded"]			= (BOOL)pPRU->bLoaded;
	(*pMap)[szStation]["PRU"][szPRU]["bStatLoaded"]		= (BOOL)pPRU->bStatLoaded;
	(*pMap)[szStation]["PRU"][szPRU]["lPBMethod"]		= (LONG)pPRU->lPBMethod;

	(*pMap)[szStation]["PRU"][szPRU]["dPostBdRejectTol"] = (DOUBLE)pPRU->dPostBdRejectTol;
	(*pMap)[szStation]["PRU"][szPRU]["emRecordType"]	= (LONG)pPRU->emRecordType;

	(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_RIGHT].x"]	= pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].x;
	(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_RIGHT].y"]	= pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].y;
	(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_LEFT].x"]	= pPRU->arcoLrnDieCorners[PR_UPPER_LEFT].x;
	(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_LEFT].y"]	= pPRU->arcoLrnDieCorners[PR_UPPER_LEFT].y;
	(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_RIGHT].x"]	= pPRU->arcoLrnDieCorners[PR_LOWER_RIGHT].x;
	(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_RIGHT].y"]	= pPRU->arcoLrnDieCorners[PR_LOWER_RIGHT].y;
	(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_LEFT].x"]	= pPRU->arcoLrnDieCorners[PR_LOWER_LEFT].x;
	(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_LEFT].y"]	= pPRU->arcoLrnDieCorners[PR_LOWER_LEFT].y;

	(*pMap)[szStation]["PRU"][szPRU]["stSrchCmd"]["uwRecordID"] 			= pPRU->stSrchCmd.uwRecordID;	// Vision UI
	(*pMap)[szStation]["PRU"][szPRU]["stSrchCmd"]["emVisualTask"] 			= pPRU->stSrchCmd.emVisualTask;
	(*pMap)[szStation]["PRU"][szPRU]["stModifyCritCmd"]["uwRecordID"] 		= pPRU->stModifyCritCmd.uwRecordID;
	(*pMap)[szStation]["PRU"][szPRU]["stModifyCritCmd"]["emVisualTask"] 		= pPRU->stModifyCritCmd.emVisualTask;

	(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_RIGHT].x"]	= (LONG)pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].x;
	(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_RIGHT].y"]	= (LONG)pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].y;
	(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_LEFT].x"]	= (LONG)pPRU->arcoLrnDieCorners[PR_UPPER_LEFT].x;
	(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_LEFT].y"]	= (LONG)pPRU->arcoLrnDieCorners[PR_UPPER_LEFT].y;
	(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_RIGHT].x"]	= (LONG)pPRU->arcoLrnDieCorners[PR_LOWER_RIGHT].x;
	(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_RIGHT].y"]	= (LONG)pPRU->arcoLrnDieCorners[PR_LOWER_RIGHT].y;
	(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_LEFT].x"]	= (LONG)pPRU->arcoLrnDieCorners[PR_LOWER_LEFT].x;
	(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_LEFT].y"]	= (LONG)pPRU->arcoLrnDieCorners[PR_LOWER_LEFT].y;

	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["arcoDieCorners[PR_UPPER_LEFT].x"]	= (LONG)pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].x;
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["arcoDieCorners[PR_UPPER_LEFT].y"]	= (LONG)pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].y;
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["arcoDieCorners[PR_LOWER_RIGHT].x"]	= (LONG)pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].x;
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["arcoDieCorners[PR_LOWER_RIGHT].y"]	= (LONG)pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].y;
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["emAlignAlg"]						= (LONG)pPRU->stManualLrnDieCmd.emAlignAlg;
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[0].coCorner1.x"]	= (LONG)pPRU->stManualLrnDieCmd.astChipWindows[0].coCorner1.x;
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[0].coCorner1.y"]	= (LONG)pPRU->stManualLrnDieCmd.astChipWindows[0].coCorner1.y;
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[0].coCorner2.x"]	= (LONG)pPRU->stManualLrnDieCmd.astChipWindows[0].coCorner2.x;
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[0].coCorner2.y"]	= (LONG)pPRU->stManualLrnDieCmd.astChipWindows[0].coCorner2.y;
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[1].coCorner1.x"]	= (LONG)pPRU->stManualLrnDieCmd.astChipWindows[1].coCorner1.x;
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[1].coCorner1.y"]	= (LONG)pPRU->stManualLrnDieCmd.astChipWindows[1].coCorner1.y;
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[1].coCorner2.x"]	= (LONG)pPRU->stManualLrnDieCmd.astChipWindows[1].coCorner2.x;
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[1].coCorner2.y"]	= (LONG)pPRU->stManualLrnDieCmd.astChipWindows[1].coCorner2.y;
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["leChipIntrusiveLength"]				= (LONG)pPRU->stManualLrnDieCmd.leChipIntrusiveLength;
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["emIsAutoLearnFMarkCenterAngle"]	= (BOOL)pPRU->stManualLrnDieCmd.emIsAutoLearnFMarkCenterAngle;
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["emFiducialMarkType"]				= (LONG)pPRU->stManualLrnDieCmd.emFiducialMarkType;
	//(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["auwRecordID[0]"]						= (LONG)pPRU->stSrchDieCmd.auwRecordID[0];
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["ubThreshold"] = (LONG)pPRU->stManualLrnDieCmd.stAlign.stStreet.ubThreshold;

	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["emAlignAlg"]				= (LONG)pPRU->stSrchDieCmd.emAlignAlg;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stLargeSrchWin.coCorner1.x"] = (LONG)pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.x;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stLargeSrchWin.coCorner1.y"] = (LONG)pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.y;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stLargeSrchWin.coCorner2.x"] = (LONG)pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.x;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stLargeSrchWin.coCorner2.y"] = (LONG)pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.y;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["rMatchScore"]				= (DOUBLE)pPRU->stSrchDieCmd.stDieAlignPar.rMatchScore;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["rStartAngle"]				= (DOUBLE)pPRU->stSrchDieCmd.stDieAlignPar.rStartAngle;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["rEndAngle"]					= (DOUBLE)pPRU->stSrchDieCmd.stDieAlignPar.rEndAngle;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieInsp"]["stTmpl"]["emDefectAttribute"]	= (LONG)pPRU->stSrchDieCmd.stDieInsp.stTmpl.emDefectAttribute;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieInsp"]["stTmpl"]["ubDefectThreshold"]	= (LONG)pPRU->stSrchDieCmd.stDieInsp.stTmpl.ubDefectThreshold;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["coProbableDieCentre.x"]		= (LONG)pPRU->stSrchDieCmd.coProbableDieCentre.x;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["coProbableDieCentre.y"]		= (LONG)pPRU->stSrchDieCmd.coProbableDieCentre.y;
#ifdef CHECK_COLLET_DIRT
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["emPostBondInsp"]	= (LONG)pPRU->stSrchDieCmd.emPostBondInsp; //20151201
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["emPostBondInspMode"]	= (LONG)pPRU->stSrchDieCmd.emPostBondInspMode; //20151201
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieInsp"]["stTmpl"]["aeMinSingleDefectArea"]	= (LONG)pPRU->stSrchDieCmd.stDieInsp.stTmpl.aeMinSingleDefectArea;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieInsp"]["stTmpl"]["aeMinTotalDefectArea"]	= (LONG)pPRU->stSrchDieCmd.stDieInsp.stTmpl.aeMinTotalDefectArea;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieInsp"]["stTmpl"]["emChipDieChk"]	= (LONG)pPRU->stSrchDieCmd.stDieInsp.stTmpl.emChipDieChk;
#endif
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["coSelectPoint.x"]				= (LONG)pPRU->stSrchDieCmd.stDieAlign.stStreet.coSelectPoint.x;	//20150107
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["coSelectPoint.y"]				= (LONG)pPRU->stSrchDieCmd.stDieAlign.stStreet.coSelectPoint.y;	//20150107 for RGB Pattern
	(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["emObjectType"]				= (LONG)pPRU->stManualLrnDieCmd.emObjectType;	//20150107 for RGB Pattern
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieAlignPar"]["ubMaxScale"]	= (DOUBLE)pPRU->stSrchDieCmd.stDieAlignPar.ubMaxScale; //20130103
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieAlignPar"]["ubMinScale"] = (DOUBLE)pPRU->stSrchDieCmd.stDieAlignPar.ubMinScale;

	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stPaLn.emIsCoarseSrch"]	= (BOOL)(LONG)pPRU->stSrchDieCmd.stDieAlign.stPaLn.emIsCoarseSrch;	 // new
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stPaLn.stLargeSrchWin.coCorner1.x"]	= (LONG)pPRU->stSrchDieCmd.stDieAlign.stPaLn.stLargeSrchWin.coCorner1.x;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stPaLn.stLargeSrchWin.coCorner1.y"]	= (LONG)pPRU->stSrchDieCmd.stDieAlign.stPaLn.stLargeSrchWin.coCorner1.y;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stPaLn.stLargeSrchWin.coCorner2.x"]	= (LONG)pPRU->stSrchDieCmd.stDieAlign.stPaLn.stLargeSrchWin.coCorner2.x;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stPaLn.stLargeSrchWin.coCorner2.y"]	= (LONG)pPRU->stSrchDieCmd.stDieAlign.stPaLn.stLargeSrchWin.coCorner2.y;
	//Confimation Pattern
	(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["rCfmPatternMatchScore"]		= (DOUBLE)pPRU->stSrchDieCmd.rCfmPatternMatchScore;


	(*pMap)[szStation]["PRU"][szPRU]["stLrnShapeCmd"]["coCorner1.x"]				= (LONG)pPRU->stLrnShapeCmd.stEncRect.coCorner1.x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnShapeCmd"]["coCorner1.y"]				= (LONG)pPRU->stLrnShapeCmd.stEncRect.coCorner1.y;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnShapeCmd"]["coCorner2.x"]				= (LONG)pPRU->stLrnShapeCmd.stEncRect.coCorner2.x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnShapeCmd"]["coCorner2.y"]				= (LONG)pPRU->stLrnShapeCmd.stEncRect.coCorner2.y;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnShapeCmd"]["emObjAttribute"]			= (LONG)pPRU->stLrnShapeCmd.emObjAttribute;
	//(*pMap)[szStation]["PRU"][szPRU][szStation"]["stDetectShapeCmd"]["uwRecordID"]			= (LONG)pPRU->stDetectShapeCmd.uwRecordID;
	(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["stInspWin.coCorner1.x"]	= (LONG)pPRU->stDetectShapeCmd.stInspWin.coCorner1.x;
	(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["stInspWin.coCorner1.y"]	= (LONG)pPRU->stDetectShapeCmd.stInspWin.coCorner1.y;
	(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["stInspWin.coCorner2.x"]	= (LONG)pPRU->stDetectShapeCmd.stInspWin.coCorner2.x;
	(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["stInspWin.coCorner2.y"]	= (LONG)pPRU->stDetectShapeCmd.stInspWin.coCorner2.y;
	(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["rDetectScore"]			= (DOUBLE)pPRU->stDetectShapeCmd.rDetectScore;
	(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["rSizePercentVar"]			= (DOUBLE)pPRU->stDetectShapeCmd.rSizePercentVar;

	(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["rStartAngle"]			= (DOUBLE)pPRU->stDetectShapeCmd.rStartAngle;
	(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["rEndAngle"]			= (DOUBLE)pPRU->stDetectShapeCmd.rEndAngle;

	(*pMap)[szStation]["PRU"][szPRU]["stLoadTmplCmd"]["coTmplCentre.x"]			= (LONG)pPRU->stLoadTmplCmd.coTmplCentre.x;
	(*pMap)[szStation]["PRU"][szPRU]["stLoadTmplCmd"]["coTmplCentre.y"]			= (LONG)pPRU->stLoadTmplCmd.coTmplCentre.y;
	(*pMap)[szStation]["PRU"][szPRU]["stLoadTmplCmd"]["szTmplSize.x"]				= (LONG)pPRU->stLoadTmplCmd.szTmplSize.x;
	(*pMap)[szStation]["PRU"][szPRU]["stLoadTmplCmd"]["szTmplSize.y"]				= (LONG)pPRU->stLoadTmplCmd.szTmplSize.y;
	//(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["uwRecordID"]				= (LONG)pPRU->stSrchTmplCmd.uwRecordID;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["ubPassScore"]				= (LONG)pPRU->stSrchTmplCmd.ubPassScore;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["stSrchWin.coCorner1.x"]	= (LONG)pPRU->stSrchTmplCmd.stSrchWin.coCorner1.x;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["stSrchWin.coCorner1.y"]	= (LONG)pPRU->stSrchTmplCmd.stSrchWin.coCorner1.y;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["stSrchWin.coCorner2.x"]	= (LONG)pPRU->stSrchTmplCmd.stSrchWin.coCorner2.x;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["stSrchWin.coCorner2.y"]	= (LONG)pPRU->stSrchTmplCmd.stSrchWin.coCorner2.y;

	// 20141023 Yip: Add Kerf Fitting Functions
	(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["emKerfBrightness"]		= (LONG)pPRU->stLrnKerfCmd.emKerfBrightness;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["coCorner1.x"]				= (LONG)pPRU->stLrnKerfCmd.stROI.coCorner1.x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["coCorner1.y"]				= (LONG)pPRU->stLrnKerfCmd.stROI.coCorner1.y;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["coCorner2.x"]				= (LONG)pPRU->stLrnKerfCmd.stROI.coCorner2.x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["coCorner2.y"]				= (LONG)pPRU->stLrnKerfCmd.stROI.coCorner2.y;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["rLowerBoundSearchAngle"]	= (DOUBLE)pPRU->stLrnKerfCmd.rLowerBoundSearchAngle;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["rUpperBoundSearchAngle"]	= (DOUBLE)pPRU->stLrnKerfCmd.rUpperBoundSearchAngle;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["uwFittingPassingScore"]	= (LONG)pPRU->stLrnKerfCmd.uwFittingPassingScore;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["rMaxWidthDev"]			= (DOUBLE)pPRU->stLrnKerfCmd.rMaxWidthDev;

	(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[0].x"]	= (LONG)pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[0].x;
	(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[0].y"]	= (LONG)pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[0].y;
	(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[1].x"]	= (LONG)pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[1].x;
	(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[1].y"]	= (LONG)pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[1].y;
	(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[2].x"]	= (LONG)pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[2].x;
	(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[2].y"]	= (LONG)pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[2].y;
	(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[3].x"]	= (LONG)pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[3].x;
	(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[3].y"]	= (LONG)pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[3].y;


	//New For Substraction

	//(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["uwNumOfPurpose"]		= pPRU->stLrnACFCmd.uwNumOfPurpose;
	//(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["aemPurpose[0]"]		= pPRU->stLrnACFCmd.aemPurpose[0];
	//(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["aemPurpose[1]"]		= pPRU->stLrnACFCmd.aemPurpose[1];
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emACFSegmentAlg"]		= pPRU->stLrnACFCmd.emACFSegmentAlg;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emACFAlignAlg"]		= pPRU->stLrnACFCmd.emACFAlignAlg;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emACFAttribute"]		= pPRU->stLrnACFCmd.emACFAttribute;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emACFPosition"]		= pPRU->stLrnACFCmd.emACFPosition;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emBackgroundType"]		= pPRU->stLrnACFCmd.emBackgroundType;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["ubSegThreshold"]		= pPRU->stLrnACFCmd.ubSegThreshold;
	//pPRU->stLrnACFCmd.uwPreSubtractStationID	= 0;
	//pPRU->stLrnACFCmd.ulPreSubtractImageID		= 0;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emACFInspAlg"]			= pPRU->stLrnACFCmd.emACFInspAlg;

//learn Region
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["uwNumOfCorners"]	= pPRU->stLrnACFCmd.stACFRegion.uwNumOfCorners;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[0].x"]	= pPRU->stLrnACFCmd.stACFRegion.acoCorners[0].x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[0].y"]	= pPRU->stLrnACFCmd.stACFRegion.acoCorners[0].y;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[1].x"]	= pPRU->stLrnACFCmd.stACFRegion.acoCorners[1].x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[1].y"]	= pPRU->stLrnACFCmd.stACFRegion.acoCorners[1].y;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[2].x"]	= pPRU->stLrnACFCmd.stACFRegion.acoCorners[2].x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[2].y"]	= pPRU->stLrnACFCmd.stACFRegion.acoCorners[2].y;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[3].x"]	= pPRU->stLrnACFCmd.stACFRegion.acoCorners[3].x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[3].y"]	= pPRU->stLrnACFCmd.stACFRegion.acoCorners[3].y;

	//Search Region
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["uwNumOfCorners"]	= pPRU->stLrnACFCmd.stSrchRegion.uwNumOfCorners;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[0].x"]	= pPRU->stLrnACFCmd.stSrchRegion.acoCorners[0].x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[0].y"]	= pPRU->stLrnACFCmd.stSrchRegion.acoCorners[0].y;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[1].x"]	= pPRU->stLrnACFCmd.stSrchRegion.acoCorners[1].x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[1].y"]	= pPRU->stLrnACFCmd.stSrchRegion.acoCorners[1].y;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[2].x"]	= pPRU->stLrnACFCmd.stSrchRegion.acoCorners[2].x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[2].y"]	= pPRU->stLrnACFCmd.stSrchRegion.acoCorners[2].y;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[3].x"]	= pPRU->stLrnACFCmd.stSrchRegion.acoCorners[3].x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[3].y"]	= pPRU->stLrnACFCmd.stSrchRegion.acoCorners[3].y;

	//Detect Region
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"]["uwNMultRegion"]	= pPRU->stLrnACFCmd.stDefectMultRegion.uwNMultRegion;
	LONG lTotalDefectRegion = pPRU->stLrnACFCmd.stDefectMultRegion.uwNMultRegion;
	for (INT i = 0; i < lTotalDefectRegion; i++)
	{
		(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["uwNumOfCorners"]	= pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].uwNumOfCorners;
		(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[0].x"]	= pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].acoCorners[0].x;
		(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[0].y"]	= pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].acoCorners[0].y;
		(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[1].x"]	= pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].acoCorners[1].x;
		(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[1].y"]	= pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].acoCorners[1].y;
		(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[2].x"]	= pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].acoCorners[2].x;
		(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[2].y"]	= pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].acoCorners[2].y;
		(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[3].x"]	= pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].acoCorners[3].x;
		(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[3].y"]	= pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].acoCorners[3].y;
	}	
	//ChipArea
	//pPRU->stLrnACFCmd.stChipMultRegion.uwNMultRegion = 2;
	//(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["uwNumOfCorners"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].uwNumOfCorners;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[0].x"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[0].x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[0].y"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[0].y;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[1].x"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[1].x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[1].y"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[1].y;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[2].x"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[2].x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[2].y"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[2].y;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[3].x"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[3].x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[3].y"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[3].y	;
	
	//(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["uwNumOfCorners"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].uwNumOfCorners;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[0].x"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[0].x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[0].y"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[0].y;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[1].x"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[1].x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[1].y"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[1].y;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[2].x"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[2].x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[2].y"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[2].y;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[3].x"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[3].x;
	(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[3].y"]	= pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[3].y	;
	


	//Srch ACF
	(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["stACFInsp"]["aeMinChipArea"]				= pPRU->stSrchACFCmd.stACFInsp.aeMinChipArea;
	for (INT i = 0; i < PR_MAX_REGION; i++)
	{
		(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["stACFInsp"]["aeMinSingleDefectArea"][i]	= pPRU->stSrchACFCmd.stACFInsp.aeMinSingleDefectArea[i];
		(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["stACFInsp"]["aeMinTotalDefectArea"][i]	= pPRU->stSrchACFCmd.stACFInsp.aeMinTotalDefectArea[i];
	}
	(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["stACFInsp"]["emDefectAttribute"]			= pPRU->stSrchACFCmd.stACFInsp.emDefectAttribute;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["stACFInsp"]["ubDefectThreshold"]			= pPRU->stSrchACFCmd.stACFInsp.ubDefectThreshold;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["rMatchScore"]								= pPRU->stSrchACFCmd.rMatchScore;
	(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["rAngleRange"]								= pPRU->stSrchACFCmd.rAngleRange;
	
	if (IsMachineData)
	{
		// calibration data
		(*pMap)[szStation]["PRU"][szPRU]["bCalibrated"]	= pPRU->bCalibrated;
		(*pMap)[szStation]["PRU"][szPRU]["m2p_xform.a"]	= pPRU->m2p_xform.a;
		(*pMap)[szStation]["PRU"][szPRU]["m2p_xform.b"]	= pPRU->m2p_xform.b;
		(*pMap)[szStation]["PRU"][szPRU]["m2p_xform.c"]	= pPRU->m2p_xform.c;
		(*pMap)[szStation]["PRU"][szPRU]["m2p_xform.d"]	= pPRU->m2p_xform.d;
		(*pMap)[szStation]["PRU"][szPRU]["p2m_xform.a"]	= pPRU->p2m_xform.a;
		(*pMap)[szStation]["PRU"][szPRU]["p2m_xform.b"]	= pPRU->p2m_xform.b;
		(*pMap)[szStation]["PRU"][szPRU]["p2m_xform.c"]	= pPRU->p2m_xform.c;
		(*pMap)[szStation]["PRU"][szPRU]["p2m_xform.d"]	= pPRU->p2m_xform.d;
#if 1 //20140416
		(*pMap)[szStation]["PRU"][szPRU]["bCalibrated_2"]	= pPRU->bCalibrated_2;
		(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_2.a"]	= pPRU->m2p_xform_2.a;
		(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_2.b"]	= pPRU->m2p_xform_2.b;
		(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_2.c"]	= pPRU->m2p_xform_2.c;
		(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_2.d"]	= pPRU->m2p_xform_2.d;
		(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_2.a"]	= pPRU->p2m_xform_2.a;
		(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_2.b"]	= pPRU->p2m_xform_2.b;
		(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_2.c"]	= pPRU->p2m_xform_2.c;
		(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_2.d"]	= pPRU->p2m_xform_2.d;
#endif
#if 1 //20170313
		(*pMap)[szStation]["PRU"][szPRU]["bCalibrated_3"]	= pPRU->bCalibrated_3;
		(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_3.a"]	= pPRU->m2p_xform_3.a;
		(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_3.b"]	= pPRU->m2p_xform_3.b;
		(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_3.c"]	= pPRU->m2p_xform_3.c;
		(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_3.d"]	= pPRU->m2p_xform_3.d;
		(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_3.a"]	= pPRU->p2m_xform_3.a;
		(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_3.b"]	= pPRU->p2m_xform_3.b;
		(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_3.c"]	= pPRU->p2m_xform_3.c;
		(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_3.d"]	= pPRU->p2m_xform_3.d;

		(*pMap)[szStation]["PRU"][szPRU]["bCalibrated_4"]	= pPRU->bCalibrated_4;
		(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_4.a"]	= pPRU->m2p_xform_4.a;
		(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_4.b"]	= pPRU->m2p_xform_4.b;
		(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_4.c"]	= pPRU->m2p_xform_4.c;
		(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_4.d"]	= pPRU->m2p_xform_4.d;
		(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_4.a"]	= pPRU->p2m_xform_4.a;
		(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_4.b"]	= pPRU->p2m_xform_4.b;
		(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_4.c"]	= pPRU->p2m_xform_4.c;
		(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_4.d"]	= pPRU->p2m_xform_4.d;
#endif
	}

	return 1;
}

LONG CWinEagle::LoadPRPara(CString szStation, PRU *pPRU, CString szPRU, BOOL IsMachineData)  // using the wizard station
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
		
	CStringMapFile *pMap;
	if (IsMachineData)
	{
		pMap = &pAppMod->m_smfMachine;
	}
	else
	{
		pMap = &pAppMod->m_smfDevice;
	}

	// calibration PRU
	pPRU->uwCoaxialLevel		= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["uwCoaxialLevel"];
	pPRU->uwCoaxial1Level		= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["uwCoaxial1Level"];
	pPRU->uwCoaxial2Level		= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["uwCoaxial2Level"];
	pPRU->uwRingLevel			= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["uwRingLevel"];
	pPRU->uwRing1Level			= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["uwRing1Level"];
	pPRU->uwSideLevel			= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["uwSideLevel"];
	pPRU->uwSide1Level			= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["uwSide1Level"];		//20141023
	pPRU->uwBackLevel			= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["uwBackLevel"];		//20141023
	pPRU->uwBack1Level			= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["uwBack1Level"];

	pPRU->bLoaded				= (BOOL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["bLoaded"];
	pPRU->bStatLoaded			= (BOOL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["bStatLoaded"];
	pPRU->emRecordType			= (PR_RECORD_TYPE)(LONG)(*pMap)[szStation]["PRU"][szPRU]["emRecordType"];
#ifdef CHECK_COLLET_DIRT
	if ((pPRU->szStnName == "PB1ColletDirt") || (pPRU->szStnName == "PB2ColletDirt"))
	{
		pPRU->emRecordType			= PRS_DIE_TYPE;
	}
#endif
	pPRU->dPostBdRejectTol		= (DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["dPostBdRejectTol"];
	pPRU->lPBMethod				= (LONG)(*pMap)[szStation]["PRU"][szPRU]["lPBMethod"];

	pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].x	= (PR_REAL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_RIGHT].x"];
	pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].y	= (PR_REAL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_RIGHT].y"];
	pPRU->arcoLrnDieCorners[PR_UPPER_LEFT].x		= (PR_REAL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_LEFT].x"];
	pPRU->arcoLrnDieCorners[PR_UPPER_LEFT].y		= (PR_REAL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_LEFT].y"];
	pPRU->arcoLrnDieCorners[PR_LOWER_RIGHT].x	= (PR_REAL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_RIGHT].x"];
	pPRU->arcoLrnDieCorners[PR_LOWER_RIGHT].y	= (PR_REAL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_RIGHT].y"];
	pPRU->arcoLrnDieCorners[PR_LOWER_LEFT].x		= (PR_REAL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_LEFT].x"];
	pPRU->arcoLrnDieCorners[PR_LOWER_LEFT].y		= (PR_REAL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_LEFT].y"];

	pPRU->stSrchCmd.uwRecordID				= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchCmd"]["uwRecordID"];	// Vision UI
	pPRU->stSrchCmd.emVisualTask			= (PR_VISUAL_TASK)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchCmd"]["emVisualTask"];
	pPRU->stModifyCritCmd.uwRecordID		= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stModifyCritCmd"]["uwRecordID"];	// Vision UI
	pPRU->stModifyCritCmd.emVisualTask      = (PR_VISUAL_TASK)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stModifyCritCmd"]["emVisualTask"];

	pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_RIGHT].x"];
	pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_RIGHT].y"];
	pPRU->arcoLrnDieCorners[PR_UPPER_LEFT].x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_LEFT].x"];
	pPRU->arcoLrnDieCorners[PR_UPPER_LEFT].y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_UPPER_LEFT].y"];
	pPRU->arcoLrnDieCorners[PR_LOWER_RIGHT].x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_RIGHT].x"];
	pPRU->arcoLrnDieCorners[PR_LOWER_RIGHT].y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_RIGHT].y"];
	pPRU->arcoLrnDieCorners[PR_LOWER_LEFT].x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_LEFT].x"];
	pPRU->arcoLrnDieCorners[PR_LOWER_LEFT].y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["arcoLrnDieCorners[PR_LOWER_LEFT].y"];


	pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["arcoDieCorners[PR_UPPER_LEFT].x"];
	pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["arcoDieCorners[PR_UPPER_LEFT].y"];
	pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["arcoDieCorners[PR_LOWER_RIGHT].x"];
	pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["arcoDieCorners[PR_LOWER_RIGHT].y"];
	pPRU->stManualLrnDieCmd.emAlignAlg						= (PR_DIE_ALIGN_ALG)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["emAlignAlg"];
	pPRU->stManualLrnDieCmd.astChipWindows[0].coCorner1.x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[0].coCorner1.x"];
	pPRU->stManualLrnDieCmd.astChipWindows[0].coCorner1.y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[0].coCorner1.y"];
	pPRU->stManualLrnDieCmd.astChipWindows[0].coCorner2.x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[0].coCorner2.x"];
	pPRU->stManualLrnDieCmd.astChipWindows[0].coCorner2.y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[0].coCorner2.y"];
	pPRU->stManualLrnDieCmd.astChipWindows[1].coCorner1.x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[1].coCorner1.x"];
	pPRU->stManualLrnDieCmd.astChipWindows[1].coCorner1.y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[1].coCorner1.y"];
	pPRU->stManualLrnDieCmd.astChipWindows[1].coCorner2.x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[1].coCorner2.x"];
	pPRU->stManualLrnDieCmd.astChipWindows[1].coCorner2.y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["astChipWindows[1].coCorner2.y"];
	pPRU->stManualLrnDieCmd.leChipIntrusiveLength			= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["leChipIntrusiveLength"];
	pPRU->stManualLrnDieCmd.emIsAutoLearnFMarkCenterAngle	= (PR_BOOLEAN)(BOOL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["emIsAutoLearnFMarkCenterAngle"];
	pPRU->stManualLrnDieCmd.emFiducialMarkType				= (PR_FIDUCIAL_MARK_TYPE)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["emFiducialMarkType"];
	//pPRU->stSrchDieCmd.auwRecordID[0]						= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["auwRecordID[0]"];
	pPRU->stManualLrnDieCmd.stAlign.stStreet.ubThreshold	= (PR_UBYTE)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["ubThreshold"];

	pPRU->stSrchDieCmd.emAlignAlg										= (PR_DIE_ALIGN_ALG)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["emAlignAlg"];
	pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stLargeSrchWin.coCorner1.x"];
	pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stLargeSrchWin.coCorner1.y"];
	pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stLargeSrchWin.coCorner2.x"];
	pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stLargeSrchWin.coCorner2.y"];
	pPRU->stSrchDieCmd.stDieAlignPar.rMatchScore						= (PR_REAL)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["rMatchScore"];
	pPRU->stSrchDieCmd.stDieAlignPar.rStartAngle						= (PR_REAL)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["rStartAngle"];
	pPRU->stSrchDieCmd.stDieAlignPar.rEndAngle							= (PR_REAL)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["rEndAngle"];
	pPRU->stSrchDieCmd.stDieInsp.stTmpl.emDefectAttribute				= (PR_DEFECT_ATTRIBUTE)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieInsp"]["stTmpl"]["emDefectAttribute"];
	pPRU->stSrchDieCmd.stDieInsp.stTmpl.ubDefectThreshold				= (PR_UBYTE)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieInsp"]["stTmpl"]["ubDefectThreshold"];
	pPRU->stSrchDieCmd.coProbableDieCentre.x							= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["coProbableDieCentre.x"];
	pPRU->stSrchDieCmd.coProbableDieCentre.y							= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["coProbableDieCentre.y"];
	pPRU->stSrchDieCmd.stDieAlign.stStreet.coSelectPoint.x				= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["coSelectPoint.x"];	//20150107
	pPRU->stSrchDieCmd.stDieAlign.stStreet.coSelectPoint.y				= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["coSelectPoint.y"];	//20150107 for RGB Pattern
	pPRU->stManualLrnDieCmd.emObjectType								= (PR_OBJ_TYPE)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stManualLrnDieCmd"]["emObjectType"];	//20150107 for RGB Pattern
#ifdef CHECK_COLLET_DIRT
	pPRU->stSrchDieCmd.emPostBondInsp				= (PR_BOOLEAN)(BOOL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["emPostBondInsp"]; //20151201
	pPRU->stSrchDieCmd.emPostBondInspMode		= (PR_GRAB_PROCESS_MODE)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["emPostBondInspMode"]; //20151201
	pPRU->stSrchDieCmd.stDieInsp.stTmpl.aeMinSingleDefectArea	= (PR_AREA)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieInsp"]["stTmpl"]["aeMinSingleDefectArea"];
	pPRU->stSrchDieCmd.stDieInsp.stTmpl.aeMinTotalDefectArea		= (PR_AREA)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieInsp"]["stTmpl"]["aeMinTotalDefectArea"];
	pPRU->stSrchDieCmd.stDieInsp.stTmpl.emChipDieChk						=  (PR_BOOLEAN)(BOOL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieInsp"]["stTmpl"]["emChipDieChk"];
#endif

	pPRU->stSrchDieCmd.stDieAlignPar.ubMaxScale							= (PR_UBYTE)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieAlignPar"]["ubMaxScale"];
	pPRU->stSrchDieCmd.stDieAlignPar.ubMinScale							= (PR_UBYTE)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stDieAlignPar"]["ubMinScale"];

	pPRU->stSrchDieCmd.stDieAlign.stPaLn.emIsCoarseSrch					= (PR_BOOLEAN)(BOOL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stPaLn.emIsCoarseSrch"];
	pPRU->stSrchDieCmd.stDieAlign.stPaLn.stLargeSrchWin.coCorner1.x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stPaLn.stLargeSrchWin.coCorner1.x"];
	pPRU->stSrchDieCmd.stDieAlign.stPaLn.stLargeSrchWin.coCorner1.y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stPaLn.stLargeSrchWin.coCorner1.y"];
	pPRU->stSrchDieCmd.stDieAlign.stPaLn.stLargeSrchWin.coCorner2.x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stPaLn.stLargeSrchWin.coCorner2.x"];
	pPRU->stSrchDieCmd.stDieAlign.stPaLn.stLargeSrchWin.coCorner2.y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["stPaLn.stLargeSrchWin.coCorner2.y"];
	//Confimation Pattern
	pPRU->stSrchDieCmd.rCfmPatternMatchScore							= (PR_REAL)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stSrchDieCmd"]["rCfmPatternMatchScore"];


	pPRU->stLrnShapeCmd.stEncRect.coCorner1.x							= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnShapeCmd"]["coCorner1.x"];
	pPRU->stLrnShapeCmd.stEncRect.coCorner1.y							= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnShapeCmd"]["coCorner1.y"];
	pPRU->stLrnShapeCmd.stEncRect.coCorner2.x							= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnShapeCmd"]["coCorner2.x"];
	pPRU->stLrnShapeCmd.stEncRect.coCorner2.y							= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnShapeCmd"]["coCorner2.y"];
	pPRU->stLrnShapeCmd.emObjAttribute									= (PR_OBJ_ATTRIBUTE)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnShapeCmd"]["emObjAttribute"];
	//pPRU->stDetectShapeCmd.uwRecordID									= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["uwRecordID"];
	pPRU->stDetectShapeCmd.stInspWin.coCorner1.x						= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["stInspWin.coCorner1.x"];
	pPRU->stDetectShapeCmd.stInspWin.coCorner1.y						= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["stInspWin.coCorner1.y"];
	pPRU->stDetectShapeCmd.stInspWin.coCorner2.x						= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["stInspWin.coCorner2.x"];
	pPRU->stDetectShapeCmd.stInspWin.coCorner2.y						= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["stInspWin.coCorner2.y"];
	pPRU->stDetectShapeCmd.rDetectScore									= (PR_REAL)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["rDetectScore"];
	pPRU->stDetectShapeCmd.rSizePercentVar								= (PR_REAL)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["rSizePercentVar"];
#if 1 //20130103
	PR_REAL rTmpStartAngle, rTmpEndAngle;
	rTmpStartAngle = pPRU->stDetectShapeCmd.rStartAngle;
	rTmpEndAngle = pPRU->stDetectShapeCmd.rEndAngle;
	pPRU->stDetectShapeCmd.rStartAngle							= (PR_REAL)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["rStartAngle"];
	pPRU->stDetectShapeCmd.rEndAngle							= (PR_REAL)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stDetectShapeCmd"]["rEndAngle"];
	if (pPRU->stDetectShapeCmd.rStartAngle == 0.0 && pPRU->stDetectShapeCmd.rEndAngle == 0.0)
	{
		pPRU->stDetectShapeCmd.rStartAngle					= rTmpStartAngle;
		pPRU->stDetectShapeCmd.rEndAngle					= rTmpEndAngle;
	}
#endif

	pPRU->stLoadTmplCmd.coTmplCentre.x									= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLoadTmplCmd"]["coTmplCentre.x"];
	pPRU->stLoadTmplCmd.coTmplCentre.y									= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLoadTmplCmd"]["coTmplCentre.y"];
	pPRU->stLoadTmplCmd.szTmplSize.x									= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLoadTmplCmd"]["szTmplSize.x"];
	pPRU->stLoadTmplCmd.szTmplSize.y									= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLoadTmplCmd"]["szTmplSize.y"];
	//pPRU->stSrchTmplCmd.uwRecordID									= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["uwRecordID"];
	pPRU->stSrchTmplCmd.ubPassScore										= (PR_UBYTE)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["ubPassScore"];
	pPRU->stSrchTmplCmd.stSrchWin.coCorner1.x							= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["stSrchWin.coCorner1.x"];
	pPRU->stSrchTmplCmd.stSrchWin.coCorner1.y							= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["stSrchWin.coCorner1.y"];
	pPRU->stSrchTmplCmd.stSrchWin.coCorner2.x							= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["stSrchWin.coCorner2.x"];
	pPRU->stSrchTmplCmd.stSrchWin.coCorner2.y							= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchTmplCmd"]["stSrchWin.coCorner2.y"];

	// 20141023 Yip: Add Kerf Fitting Functions
	pPRU->stLrnKerfCmd.emKerfBrightness			= (PR_OBJ_ATTRIBUTE)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["emKerfBrightness"];
	pPRU->stLrnKerfCmd.stROI.coCorner1.x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["coCorner1.x"];
	pPRU->stLrnKerfCmd.stROI.coCorner1.y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["coCorner1.y"];
	pPRU->stLrnKerfCmd.stROI.coCorner2.x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["coCorner2.x"];
	pPRU->stLrnKerfCmd.stROI.coCorner2.y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["coCorner2.y"];
	pPRU->stLrnKerfCmd.rLowerBoundSearchAngle	= (PR_REAL)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["rLowerBoundSearchAngle"];
	pPRU->stLrnKerfCmd.rUpperBoundSearchAngle	= (PR_REAL)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["rUpperBoundSearchAngle"];
	pPRU->stLrnKerfCmd.uwFittingPassingScore	= (PR_UWORD)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["uwFittingPassingScore"];
	pPRU->stLrnKerfCmd.rMaxWidthDev				= (PR_REAL)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stLrnKerfCmd"]["rMaxWidthDev"];

	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[0].x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[0].x"];
	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[0].y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[0].y"];
	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[1].x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[1].x"];
	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[1].y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[1].y"];
	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[2].x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[2].x"];
	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[2].y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[2].y"];
	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[3].x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[3].x"];
	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[3].y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stImageStatInfoCmd"]["acoCorners[3].y"];


	//New For Substraction

	//pPRU->stLrnACFCmd.uwNumOfPurpose			= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["uwNumOfPurpose"];
	//pPRU->stLrnACFCmd.aemPurpose[0] 			= (PR_PURPOSE)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["aemPurpose[0]"];
	//pPRU->stLrnACFCmd.aemPurpose[1]				= (PR_PURPOSE)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["aemPurpose[1]"];
	pPRU->stLrnACFCmd.emACFSegmentAlg			= (PR_ACF_SEGMENT_ALG)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emACFSegmentAlg"];
	pPRU->stLrnACFCmd.emACFAlignAlg				= (PR_ACF_ALIGN_ALG)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emACFAlignAlg"];
	pPRU->stLrnACFCmd.emACFAttribute			= (PR_OBJ_ATTRIBUTE)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emACFAttribute"];
	pPRU->stLrnACFCmd.emACFPosition				= (PR_OBJ_POSITION)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emACFPosition"];
	pPRU->stLrnACFCmd.emBackgroundType			= (PR_BACKGROUND)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emBackgroundType"];
	pPRU->stLrnACFCmd.ubSegThreshold			= (PR_UBYTE)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["ubSegThreshold"];
	//pPRU->stLrnACFCmd.uwPreSubtractStationID	= 0;
	//pPRU->stLrnACFCmd.ulPreSubtractImageID		= 0;
	pPRU->stLrnACFCmd.emACFInspAlg				= (PR_ACF_INSP_ALG)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["emACFInspAlg"];

//learn Region
	//pPRU->stLrnACFCmd.stACFRegion.uwNumOfCorners	= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["uwNumOfCorners"];
	pPRU->stLrnACFCmd.stACFRegion.acoCorners[0].x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[0].x"];
	pPRU->stLrnACFCmd.stACFRegion.acoCorners[0].y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[0].y"];
	pPRU->stLrnACFCmd.stACFRegion.acoCorners[1].x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[1].x"];
	pPRU->stLrnACFCmd.stACFRegion.acoCorners[1].y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[1].y"];
	pPRU->stLrnACFCmd.stACFRegion.acoCorners[2].x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[2].x"];
	pPRU->stLrnACFCmd.stACFRegion.acoCorners[2].y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[2].y"];
	pPRU->stLrnACFCmd.stACFRegion.acoCorners[3].x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[3].x"];
	pPRU->stLrnACFCmd.stACFRegion.acoCorners[3].y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stACFRegion"]["acoCorners[3].y"];

	//Search Region
	//pPRU->stLrnACFCmd.stSrchRegion.uwNumOfCorners		= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["uwNumOfCorners"];
	pPRU->stLrnACFCmd.stSrchRegion.acoCorners[0].x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[0].x"];
	pPRU->stLrnACFCmd.stSrchRegion.acoCorners[0].y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[0].y"];
	pPRU->stLrnACFCmd.stSrchRegion.acoCorners[1].x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[1].x"];
	pPRU->stLrnACFCmd.stSrchRegion.acoCorners[1].y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[1].y"];
	pPRU->stLrnACFCmd.stSrchRegion.acoCorners[2].x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[2].x"];
	pPRU->stLrnACFCmd.stSrchRegion.acoCorners[2].y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[2].y"];
	pPRU->stLrnACFCmd.stSrchRegion.acoCorners[3].x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[3].x"];
	pPRU->stLrnACFCmd.stSrchRegion.acoCorners[3].y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stSrchRegion"]["acoCorners[3].y"];

	//Detect Region
	pPRU->stLrnACFCmd.stDefectMultRegion.uwNMultRegion = (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"]["uwNMultRegion"];
	LONG lTotalDefectRegion = pPRU->stLrnACFCmd.stDefectMultRegion.uwNMultRegion;
	for (INT i = 0; i < lTotalDefectRegion; i++)
	{
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].uwNumOfCorners			= (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["uwNumOfCorners"];
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].acoCorners[0].x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[0].x"];
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].acoCorners[0].y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[0].y"];
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].acoCorners[1].x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[1].x"];
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].acoCorners[1].y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[1].y"];
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].acoCorners[2].x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[2].x"];
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].acoCorners[2].y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[2].y"];
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].acoCorners[3].x		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[3].x"];
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[lTotalDefectRegion - 1].acoCorners[3].y		= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stDefectMultRegion"][lTotalDefectRegion - 1]["acoCorners[3].y"];
	}
	//ChipArea
	//pPRU->stLrnACFCmd.stChipMultRegion.uwNMultRegion = 2;
	//pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].uwNumOfCorners	  = (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["uwNumOfCorners"];
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[0].x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[0].x"];
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[0].y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[0].y"];
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[1].x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[1].x"];
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[1].y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[1].y"];
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[2].x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[2].x"];
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[2].y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[2].y"];
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[3].x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[3].x"];
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[3].y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[0]"]["acoCorners[3].y"];
	
	//pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].uwNumOfCorners	  = (PR_UWORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["uwNumOfCorners"];
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[0].x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[0].x"];
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[0].y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[0].y"];
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[1].x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[1].x"];
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[1].y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[1].y"];
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[2].x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[2].x"];
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[2].y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[2].y"];
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[3].x	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[3].x"];
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[3].y	= (PR_WORD)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stLrnACFCmd"]["stChipMultRegion"]["astRegion[1]"]["acoCorners[3].y"];


	//Srch ACF
	pPRU->stSrchACFCmd.stACFInsp.aeMinChipArea					= (PR_AREA)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["stACFInsp"]["aeMinChipArea"];
	for (INT i = 0; i < PR_MAX_REGION; i++)
	{
		pPRU->stSrchACFCmd.stACFInsp.aeMinSingleDefectArea[i]	= (PR_AREA)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["stACFInsp"]["aeMinSingleDefectArea"][i];
		pPRU->stSrchACFCmd.stACFInsp.aeMinTotalDefectArea[i]	= (PR_AREA)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["stACFInsp"]["aeMinTotalDefectArea"][i];
	}
	pPRU->stSrchACFCmd.stACFInsp.emDefectAttribute				= (PR_DEFECT_ATTRIBUTE)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["stACFInsp"]["emDefectAttribute"];
	pPRU->stSrchACFCmd.stACFInsp.ubDefectThreshold				= (PR_UBYTE)(PR_AREA)(LONG)(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["stACFInsp"]["ubDefectThreshold"];
	pPRU->stSrchACFCmd.rMatchScore								= (PR_REAL)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["rMatchScore"];
	pPRU->stSrchACFCmd.rAngleRange								= (PR_REAL)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["stSrchACFCmd"]["rAngleRange"];

	if (IsMachineData)
	{
		// calibration data
		pPRU->bCalibrated		= (BOOL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["bCalibrated"];
		pPRU->m2p_xform.a		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform.a"];
		pPRU->m2p_xform.b		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform.b"];
		pPRU->m2p_xform.c		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform.c"];
		pPRU->m2p_xform.d		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform.d"];
		pPRU->p2m_xform.a		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform.a"];
		pPRU->p2m_xform.b		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform.b"];
		pPRU->p2m_xform.c		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform.c"];
		pPRU->p2m_xform.d		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform.d"];
#if 1 //20140416
		pPRU->bCalibrated_2		= (BOOL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["bCalibrated_2"];
		pPRU->m2p_xform_2.a		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_2.a"];
		pPRU->m2p_xform_2.b		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_2.b"];
		pPRU->m2p_xform_2.c		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_2.c"];
		pPRU->m2p_xform_2.d		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_2.d"];
		pPRU->p2m_xform_2.a		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_2.a"];
		pPRU->p2m_xform_2.b		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_2.b"];
		pPRU->p2m_xform_2.c		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_2.c"];
		pPRU->p2m_xform_2.d		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_2.d"];
#endif
#if 1 //20170313
		pPRU->bCalibrated_3		= (BOOL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["bCalibrated_3"];
		pPRU->m2p_xform_3.a		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_3.a"];
		pPRU->m2p_xform_3.b		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_3.b"];
		pPRU->m2p_xform_3.c		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_3.c"];
		pPRU->m2p_xform_3.d		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_3.d"];
		pPRU->p2m_xform_3.a		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_3.a"];
		pPRU->p2m_xform_3.b		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_3.b"];
		pPRU->p2m_xform_3.c		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_3.c"];
		pPRU->p2m_xform_3.d		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_3.d"];

		pPRU->bCalibrated_4		= (BOOL)(LONG)(*pMap)[szStation]["PRU"][szPRU]["bCalibrated_4"];
		pPRU->m2p_xform_4.a		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_4.a"];
		pPRU->m2p_xform_4.b		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_4.b"];
		pPRU->m2p_xform_4.c		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_4.c"];
		pPRU->m2p_xform_4.d		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["m2p_xform_4.d"];
		pPRU->p2m_xform_4.a		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_4.a"];
		pPRU->p2m_xform_4.b		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_4.b"];
		pPRU->p2m_xform_4.c		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_4.c"];
		pPRU->p2m_xform_4.d		= (FLOAT)(DOUBLE)(*pMap)[szStation]["PRU"][szPRU]["p2m_xform_4.d"];
#endif
	}

	return 1;
}

LONG CWinEagle::IPC_SaveMachineParam()
{
	//for(INT i = 1; i<GetmyCalibPRUList().GetSize(); i++)
	for (INT i = 0; i < GetmyCalibPRUList().GetSize(); i++) //20130109 CArray is start from zero. myCalibPRUList is start from zero. But myPRUList is start from 1, because there is a dummy item at index zero (pruMenu)
	{
		SavePRPara(GetmyCalibPRUList().GetAt(i)->szStnName, GetmyCalibPRUList().GetAt(i), GetmyCalibPRUList().GetAt(i)->szFileName, TRUE);
	}

	return CAC9000Stn::IPC_SaveMachineParam();
}

LONG CWinEagle::IPC_LoadMachineParam()
{
	//for(INT i = 1; i<GetmyCalibPRUList().GetSize(); i++)
	for (INT i = 0; i < GetmyCalibPRUList().GetSize(); i++) //20130109 CArray is start from zero. myCalibPRUList is start from zero. But myPRUList is start from 1, because there is a dummy item at index zero (pruMenu)
	{
		LoadPRPara(GetmyCalibPRUList().GetAt(i)->szStnName, GetmyCalibPRUList().GetAt(i), GetmyCalibPRUList().GetAt(i)->szFileName, TRUE);

		// Reset all calib Data;
		for (INT j = 1; j < GetmyPRUList().GetSize(); j++)
		{
			//First ABCD
			if (	(GetmyPRUList().GetAt(j)->eCamMoveGroup1 == GetmyCalibPRUList().GetAt(i)->eCamMoveGroup1) &&
				GetmyCalibPRUList().GetAt(i)->eCamMoveGroup1 != CMG_NONE)
			{
				GetmyPRUList().GetAt(j)->bCalibrated	= GetmyCalibPRUList().GetAt(i)->bCalibrated;
				
				GetmyPRUList().GetAt(j)->m2p_xform.a	= GetmyCalibPRUList().GetAt(i)->m2p_xform.a;
				GetmyPRUList().GetAt(j)->m2p_xform.b	= GetmyCalibPRUList().GetAt(i)->m2p_xform.b;
				GetmyPRUList().GetAt(j)->m2p_xform.c	= GetmyCalibPRUList().GetAt(i)->m2p_xform.c;
				GetmyPRUList().GetAt(j)->m2p_xform.d	= GetmyCalibPRUList().GetAt(i)->m2p_xform.d;

				GetmyPRUList().GetAt(j)->p2m_xform.a	= GetmyCalibPRUList().GetAt(i)->p2m_xform.a;
				GetmyPRUList().GetAt(j)->p2m_xform.b	= GetmyCalibPRUList().GetAt(i)->p2m_xform.b;
				GetmyPRUList().GetAt(j)->p2m_xform.c	= GetmyCalibPRUList().GetAt(i)->p2m_xform.c;
				GetmyPRUList().GetAt(j)->p2m_xform.d	= GetmyCalibPRUList().GetAt(i)->p2m_xform.d;
			}

			//Second ABCD
			if ((GetmyPRUList().GetAt(j)->eCamMoveGroup2 == GetmyCalibPRUList().GetAt(i)->eCamMoveGroup2) &&
				GetmyCalibPRUList().GetAt(i)->eCamMoveGroup2 != CMG_NONE)
			{
				GetmyPRUList().GetAt(j)->bCalibrated_2	= GetmyCalibPRUList().GetAt(i)->bCalibrated_2;
				
				GetmyPRUList().GetAt(j)->m2p_xform_2.a	= GetmyCalibPRUList().GetAt(i)->m2p_xform_2.a;
				GetmyPRUList().GetAt(j)->m2p_xform_2.b	= GetmyCalibPRUList().GetAt(i)->m2p_xform_2.b;
				GetmyPRUList().GetAt(j)->m2p_xform_2.c	= GetmyCalibPRUList().GetAt(i)->m2p_xform_2.c;
				GetmyPRUList().GetAt(j)->m2p_xform_2.d	= GetmyCalibPRUList().GetAt(i)->m2p_xform_2.d;

				GetmyPRUList().GetAt(j)->p2m_xform_2.a	= GetmyCalibPRUList().GetAt(i)->p2m_xform_2.a;
				GetmyPRUList().GetAt(j)->p2m_xform_2.b	= GetmyCalibPRUList().GetAt(i)->p2m_xform_2.b;
				GetmyPRUList().GetAt(j)->p2m_xform_2.c	= GetmyCalibPRUList().GetAt(i)->p2m_xform_2.c;
				GetmyPRUList().GetAt(j)->p2m_xform_2.d	= GetmyCalibPRUList().GetAt(i)->p2m_xform_2.d;
			}

#if 1 //20170313
			//Third ABCD
			if ((GetmyPRUList().GetAt(j)->eCamMoveGroup3 == GetmyCalibPRUList().GetAt(i)->eCamMoveGroup3) &&
				GetmyCalibPRUList().GetAt(i)->eCamMoveGroup3 != CMG_NONE)
			{
				GetmyPRUList().GetAt(j)->bCalibrated_3	= GetmyCalibPRUList().GetAt(i)->bCalibrated_3;
				
				GetmyPRUList().GetAt(j)->m2p_xform_3.a	= GetmyCalibPRUList().GetAt(i)->m2p_xform_3.a;
				GetmyPRUList().GetAt(j)->m2p_xform_3.b	= GetmyCalibPRUList().GetAt(i)->m2p_xform_3.b;
				GetmyPRUList().GetAt(j)->m2p_xform_3.c	= GetmyCalibPRUList().GetAt(i)->m2p_xform_3.c;
				GetmyPRUList().GetAt(j)->m2p_xform_3.d	= GetmyCalibPRUList().GetAt(i)->m2p_xform_3.d;

				GetmyPRUList().GetAt(j)->p2m_xform_3.a	= GetmyCalibPRUList().GetAt(i)->p2m_xform_3.a;
				GetmyPRUList().GetAt(j)->p2m_xform_3.b	= GetmyCalibPRUList().GetAt(i)->p2m_xform_3.b;
				GetmyPRUList().GetAt(j)->p2m_xform_3.c	= GetmyCalibPRUList().GetAt(i)->p2m_xform_3.c;
				GetmyPRUList().GetAt(j)->p2m_xform_3.d	= GetmyCalibPRUList().GetAt(i)->p2m_xform_3.d;
			}

			//Fourth ABCD
			if ((GetmyPRUList().GetAt(j)->eCamMoveGroup4 == GetmyCalibPRUList().GetAt(i)->eCamMoveGroup4) &&
				GetmyCalibPRUList().GetAt(i)->eCamMoveGroup4 != CMG_NONE)
			{
				GetmyPRUList().GetAt(j)->bCalibrated_4	= GetmyCalibPRUList().GetAt(i)->bCalibrated_4;
				
				GetmyPRUList().GetAt(j)->m2p_xform_4.a	= GetmyCalibPRUList().GetAt(i)->m2p_xform_4.a;
				GetmyPRUList().GetAt(j)->m2p_xform_4.b	= GetmyCalibPRUList().GetAt(i)->m2p_xform_4.b;
				GetmyPRUList().GetAt(j)->m2p_xform_4.c	= GetmyCalibPRUList().GetAt(i)->m2p_xform_4.c;
				GetmyPRUList().GetAt(j)->m2p_xform_4.d	= GetmyCalibPRUList().GetAt(i)->m2p_xform_4.d;

				GetmyPRUList().GetAt(j)->p2m_xform_4.a	= GetmyCalibPRUList().GetAt(i)->p2m_xform_4.a;
				GetmyPRUList().GetAt(j)->p2m_xform_4.b	= GetmyCalibPRUList().GetAt(i)->p2m_xform_4.b;
				GetmyPRUList().GetAt(j)->p2m_xform_4.c	= GetmyCalibPRUList().GetAt(i)->p2m_xform_4.c;
				GetmyPRUList().GetAt(j)->p2m_xform_4.d	= GetmyCalibPRUList().GetAt(i)->p2m_xform_4.d;
			}
#endif
		}
	}

	return CAC9000Stn::IPC_LoadMachineParam();
}

LONG CWinEagle::IPC_SaveDeviceParam()
{
	for (INT i = 1; i < GetmyPRUList().GetSize(); i++)
	{
		SavePRPara(GetmyPRUList().GetAt(i)->szStnName, GetmyPRUList().GetAt(i), GetmyPRUList().GetAt(i)->szFileName);
	}

	return CAC9000Stn::IPC_SaveDeviceParam();
}

LONG CWinEagle::IPC_LoadDeviceParam()
{
	for (INT i = 1; i < GetmyPRUList().GetSize(); i++)
	{
		LoadPRPara(GetmyPRUList().GetAt(i)->szStnName, GetmyPRUList().GetAt(i), GetmyPRUList().GetAt(i)->szFileName);
	}

	return CAC9000Stn::IPC_LoadDeviceParam();
}

#if 1 //20121024 debug only
BOOL CWinEagle::LogCalbIncludeAngData(CString &szLabel, LONG lGlass, LONG lXOffset, LONG lYOffset, DOUBLE dErrDistance, LONG lErrXOffset, LONG lErrYOffset)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	FILE *fp = NULL;

	//if (AutoMode != AUTO_BOND && AutoMode != TEST_BOND)
	//	return TRUE;

	fp = fopen("d:\\sw\\AC9000\\data\\PBULCalibTest_Log.txt", "a+");
	if (fp != NULL)
	{
		__time64_t ltime;
		CString szTime = "";

		_time64(&ltime);
		szTime = _ctime64(&ltime);
		szTime.TrimRight('\n');

		fseek(fp, 0, SEEK_END);
		if (ftell(fp) < 10)
		{
			fprintf(fp, "Date: " + szTime + " PBUL included angle Calib \tcalbXoffset \tcalbYoffset \tErrorDistance(um) \tErrXoffset \tErrYoffset\n");
		}
		switch (lGlass)
		{
		case GLASS1:
			fprintf(fp, "%s Glass1 %s \t%ld\t%ld\t%.4f\t%ld\t%ld\n", (const char*)szLabel, (const char*)szTime, lXOffset, lYOffset, dErrDistance, lErrXOffset, lErrYOffset);
			break;
		//case GLASS2:
		//	fprintf(fp, "%s Glass2 %s \t%ld\t%ld\t%.4f\t%ld\t%ld\n", (const char*)szLabel, (const char*)szTime, lXOffset, lYOffset, dErrDistance, lErrXOffset, lErrYOffset);
		//	break;
		default:
			fprintf(fp, "%s %s \t%ld\t%ld\t%.4f\t%ld\t%ld\n", (const char*)szLabel, (const char*)szTime, lXOffset, lYOffset, dErrDistance, lErrXOffset, lErrYOffset);
			break;
		}
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}
#endif


//LONG CWinEagle::HMI_VerifyPreBondCalibrationPositions(IPC_CServiceMessage &svMsg)	// 20140915 Yip
//{
//	CString szTitle("Verify Pre Bond Calib. Posn");
//	CString szMsg(" ");
//
//	const int NO_OF_STATIONS = 7;
//	CString szStationList[NO_OF_STATIONS] = {"PB1", "PB2", "PreBondWH", "PBUplook", "Shuttle", "TA3", "TA4"};
//
//	for (int i = 0; i < NO_OF_STATIONS; ++i)
//	{
//		CAC9000Stn *pCStation = (CAC9000Stn*)m_pModule->GetStation(szStationList[i]);
//		if (pCStation->m_fHardware)
//		{
//			pCStation->CheckModSelected(TRUE);
//		}
//		if (!pCStation->m_bModSelected)
//		{
//			szMsg.Format("%s Module Not Selected. Operation Abort", pCStation->GetStnName());
//			HMIMessageBox(MSG_OK, szTitle, szMsg);
//			return 0;
//		}
//	}
//
//	CTA3 *pCTA3 = (CTA3*)m_pModule->GetStation("TA3");
//	if (pCTA3->MoveX(TA3_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS)
//	{
//		szMsg.Format("%s Move To Standby Position Error. Operation Abort", pCTA3->GetStnName());
//		HMIMessageBox(MSG_OK, szTitle, szMsg);
//		return 0;
//	}
//
//	CTA4 *pCTA4 = (CTA4*)m_pModule->GetStation("TA4");
//	if (pCTA4->MoveX(TA4_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS)
//	{
//		szMsg.Format("%s Move To Standby Position Error. Operation Abort", pCTA4->GetStnName());
//		HMIMessageBox(MSG_OK, szTitle, szMsg);
//		return 0;
//	}
//
//	const unsigned long MAX_PRU = 4;
//	unsigned long ulPRU[MAX_PRU] =	{/*PBDL1_CALIB_PRU,
//									PBDL2_CALIB_PRU, */
//									UPLOOK1_GLASS_CALIB_PRU,
//									UPLOOK2_GLASS_CALIB_PRU};
//	PRU *pPRU;
//
//	for (unsigned long i = 0; i < MAX_PRU; ++i)
//	{
//		pPRU = &SelectCurPRU(ulPRU[i]);
//		if (!pPRU->bLoaded)
//		{
//			szMsg.Format("%s: %s PR Pattern Not Loaded. Operation Abort!", pPRU->szStnName, pPRU->szFileName);
//			HMIMessageBox(MSG_OK, szTitle, szMsg);
//			return 0;
//		}
//	}
//
//	// Get Date and Time
//	SYSTEMTIME ltime;
//	GetLocalTime(&ltime);
//	CString szDateTime;
//	szDateTime.Format("%04u%02u%02u%02u%02u%02u", ltime.wYear, ltime.wMonth, ltime.wDay, ltime.wHour, ltime.wMinute, ltime.wSecond);
//
//	CString szPath = "D:\\sw\\AC9000\\Data\\PRLog";
//	if (_access(szPath, 0) != 0)	
//	{
//		_mkdir(szPath);
//	}
//
//	szPath += "\\" + szDateTime;
//	if (_access(szPath, 0) != 0)	
//	{
//		_mkdir(szPath);
//	}
//
//	CString szFile = szPath + "\\PRLog_" + szDateTime + ".csv";
//	CStdioFile LogFile;
//	if (!LogFile.Open(szFile, CFile::modeCreate | CFile::modeWrite))
//	{
//		HMIMessageBox(MSG_OK, szTitle, "Cannot Create Log File. Operation Abort!");
//		return 0;
//	}
//	LogFile.SeekToEnd();
//	szMsg = "PRU,";
//	szMsg += "PBWH X,PBWH Y,";
//	szMsg += "PBWH Mini X,PBWH Mini Y,";
//	szMsg += "PBWH Z1,PBWH Z2";
//	szMsg += "PBUplook X,PBUplook Y,";
//	szMsg += "PR X,PR Y,PR Angle,PR Score\n";
//	LogFile.WriteString(szMsg);
//
//	CPreBondWH *pCPreBondWH = (CPreBondWH*)m_pModule->GetStation("PreBondWH");
//	CPBUplook *pCPBUplook = (CPBUplook*)m_pModule->GetStation("PBUplook");
//
//	m_bMoveToPosn = TRUE;
//	for (unsigned long i = 0; i < MAX_PRU; ++i)
//	{
//		// Select PRU
//		HMI_ulCurPRU = ulPRU[i];
//		SelectCalibPRU(HMI_ulCurPRU, FALSE);
//		pPRU = &SelectCurPRU(HMI_ulCurPRU);
//
//		pPRU->coDieCentre.x	= PR_DEF_CENTRE_X;
//		pPRU->coDieCentre.y	= PR_DEF_CENTRE_Y;
//		pPRU->fAng			= 0.0;
//		pPRU->fScore		= 0.0;
//
//		// Search and Log PR
//		PR_SetDebugFlagPerChannel(PR_DEBUG_LOG_APP_LOG, pPRU->ubSenderID);
//		HMI_SrchPR(svMsg);
//		PR_SetDebugFlagPerChannel(PR_DEBUG_DISABLE, pPRU->ubSenderID);
//
//		// Upload PR to Specific Folder
//		PR_UPLOAD_LAST_LOG_CASE_PER_CHANNEL_CMD stCmd;
//		PR_UPLOAD_LAST_LOG_CASE_PER_CHANNEL_RPY stRpy;
//		PR_InitUploadLastLogCasePerChannel(&stCmd);
//		sprintf((char*)stCmd.aubDestinFilePath, szPath.GetString());
//		PR_UploadLastLogCasePerChannel(&stCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy);
//
//		// Log Motor Counts and PR Data to CSV File
//		szMsg.Format("%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%.2f,%.2f\n",
//					 pPRU->szFileName,
//					 pCPreBondWH->m_stMotorX.GetEncPosn(), pCPreBondWH->m_stMotorY.GetEncPosn(),
//					 pCPreBondWH->m_stMotorX2.GetEncPosn(), pCPreBondWH->m_stMotorY2.GetEncPosn(),
//					 pCPreBondWH->m_stMotorZ[WHZ_1].GetEncPosn(), pCPreBondWH->m_stMotorZ[WHZ_2].GetEncPosn(),
////					 pCPreBondWH->m_stMotorT.GetEncPosn(),
//					 pCPBUplook->m_stMotorX.GetEncPosn(), pCPBUplook->m_stMotorY.GetEncPosn(),
//					 pPRU->coDieCentre.x, pPRU->coDieCentre.y, pPRU->fAng, pPRU->fScore
//					);
//		LogFile.WriteString(szMsg);
//	}
//
//	LogFile.Close();
//
//	return 0;
//}

LONG CWinEagle::HMI_AutoTest(IPC_CServiceMessage &svMsg)
{	

	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");
	CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");


	BOOL bResult = TRUE;
	PRU *pPRU;
	CString strMsg;
	LONG lAnswer = -1;

	pPRU = &SelectCurPRU(HMI_ulCurPRU);

	switch (HMI_ulCurPRU)
	{
	case ACF1WH_CALIB_PRU :
		if (!pCACF1WH->m_bModSelected)
		{
			strMsg.Format("%s Module Not Selected. Operation Abort", pCACF1WH->GetStnName());
			HMIMessageBox(MSG_OK, "WARNING", strMsg);
			bResult = FALSE;
		}
		else
		{
			pruACF_WHCalib.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF1WH;
			if (!pCACF1WH->AutoTest(pPRU))
			{
				bResult = FALSE;
			}	
		}
		break;

	case ACF2WH_CALIB_PRU :
		if (!pCACF2WH->m_bModSelected)
		{
			strMsg.Format("%s Module Not Selected. Operation Abort", pCACF2WH->GetStnName());
			HMIMessageBox(MSG_OK, "WARNING", strMsg);
			bResult = FALSE;
		}
		else
		{
			pruACF_WHCalib.eCamMoveGroupCurrent = CMG_ACFDL_XACFDL_YACF2WH;
			if (!pCACF2WH->AutoTest(pPRU))
			{
				bResult = FALSE;
			}	
		}
		break;

	case ACFUL_CALIB_PRU :
		if (!pCTA1->m_bModSelected)
		{
			strMsg.Format("%s Module Not Selected. Operation Abort", pCTA1->GetStnName());
			HMIMessageBox(MSG_OK, "WARNING", strMsg);
			bResult = FALSE;
		}
		else
		{
			if (!pCTA1->AutoTest(pPRU))
			{
				bResult = FALSE;
			}	
		}
		break;
	}

	return 1;
}

LONG CWinEagle::HMI_CORAutoTest(IPC_CServiceMessage &svMsg)
{

	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");


	BOOL bOpDone = TRUE;
	INT nConvID = 0;
	IPC_CServiceMessage stMsg;
	LONG lTimeCount = 0;
	
	LONG lAnswer = rMSG_TIMEOUT;
	LONG lAutoTestCount = 0;
	LONG lResponse = 0;
	LONG lCount = 0;

	BOOL bResult = TRUE;
	CString strMsg;

	do
	{
		lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "OPTION", "Auto Test with COR Angle: -5, -3, -1, -0.5, -0.1, 0.1, 0.5, 1, 3, 5?");

		if (lAnswer == rMSG_CONTINUE)
		{
			if (!HMINumericKeys("Set Test Repeat Time", 30000, 0, &lAutoTestCount))
			{
				HMIMessageBox(MSG_OK, "COR AUTO TEST OPERATION", "OPERATION STOP");
				svMsg.InitMessage(sizeof(BOOL), &bOpDone);
				return 1;
				break;
			}
			if (lAutoTestCount < 0 || lAutoTestCount == 0 || lAutoTestCount > 30000)
			{
				HMIMessageBox(MSG_OK, "COR AUTO TEST OPERATION", "OPERATION STOP");
				svMsg.InitMessage(sizeof(BOOL), &bOpDone);
				return 1;
				break;
			}
			if (lAutoTestCount >= 30)
			{
				lCount = lAutoTestCount / 10;
				lAutoTestCount = lAutoTestCount / lCount;
			}

			switch (HMI_ulCurPRU)
			{
			case ACF1WH_CALIB_PRU :
				if (!pCACF1WH->m_bModSelected)
				{
					strMsg.Format("%s Module Not Selected. Operation Abort", pCACF1WH->GetStnName());
					HMIMessageBox(MSG_OK, "WARNING", strMsg);
					bResult = FALSE;
				}
				else
				{
					if (!pCACF1WH->CORAutoTest(&pruACF_WHCalib, lAutoTestCount, FALSE, TRUE))
					{
						bResult = FALSE;
					}

				}
				break;
			case ACF2WH_CALIB_PRU :
				if (!pCACF2WH->m_bModSelected)
				{
					strMsg.Format("%s Module Not Selected. Operation Abort", pCACF2WH->GetStnName());
					HMIMessageBox(MSG_OK, "WARNING", strMsg);
					bResult = FALSE;
				}
				else
				{
					if (!pCACF2WH->CORAutoTest(&pruACF_WHCalib, lAutoTestCount, FALSE, TRUE))
					{
						bResult = FALSE;
					}

				}
				break;
			}
		}
		else if (lAnswer == rMSG_CANCEL || lAnswer == rMSG_TIMEOUT)
		{
			return 1;
			break;
		}
	} while (0);

	if (bResult)
	{
		HMIMessageBox(MSG_OK, "COR AUTO TEST", "OPERATION SUCCESS");
	}
	else
	{
		HMIMessageBox(MSG_OK, "COR AUTO TEST", "OPERATION FAIL");
	}

	return 1;
}

LONG CWinEagle::HMI_SetPRErrorLimit(IPC_CServiceMessage &svMsg)
{
	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
	CACF2WH *pCACF2WH = (CACF2WH*)m_pModule->GetStation("ACF2WH");



	LONG lLimit;

	svMsg.GetMsg(sizeof(LONG), &lLimit);

	HMI_lErrorLimit = lLimit;

	switch (HMI_ulCurPRU)
	{
	case ACF1WH_CALIB_PRU :
		//pCACF1WH->m_lPRErrorLimit = lLimit;
	case ACF2WH_CALIB_PRU :
		//pCACF2WH->m_lPRErrorLimit = lLimit;
		break;
	}


	return 0;
}

//LONG CWinEagle::HMI_TA4CORTest(IPC_CServiceMessage &svMsg)
//{
//
//	CACF1WH *pCACF1WH = (CACF1WH*)m_pModule->GetStation("ACF1WH");
//
//	BOOL bOpDone = TRUE;
//	INT nConvID = 0;
//	IPC_CServiceMessage stMsg;
//	LONG lTimeCount = 0;
//	
//	LONG lAnswer = rMSG_TIMEOUT;
//	LONG lAutoTestCount = 0;
//	LONG lResponse = 0;
//	LONG lCount = 0;
//
//	BOOL bResult = TRUE;
//	CString strMsg;
//
//	do
//	{
//		lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "OPTION", "Auto Test with COR Angle: -3, -2, -1, -0.5, 0.5, 1, 2, 3?");
//
//
//		if (lAnswer == rMSG_CONTINUE)
//		{
//			if (!HMINumericKeys("Set Test Repeat Time", 30000, 0, &lAutoTestCount))
//			{
//				HMIMessageBox(MSG_OK, "COR AUTO TEST OPERATION", "OPERATION STOP");
//				svMsg.InitMessage(sizeof(BOOL), &bOpDone);
//				return 1;
//				break;
//			}
//			if (lAutoTestCount < 0 || lAutoTestCount == 0 || lAutoTestCount > 30000)
//			{
//				HMIMessageBox(MSG_OK, "COR AUTO TEST OPERATION", "OPERATION STOP");
//				svMsg.InitMessage(sizeof(BOOL), &bOpDone);
//				return 1;
//				break;
//			}
//			if (lAutoTestCount >= 100)
//			{
//				lCount = lAutoTestCount / 100;
//				lAutoTestCount = lAutoTestCount / lCount;
//			}
//
//		do 
//		{
//				
//		if (!pCTA4->m_bModSelected )
//		{
//			strMsg.Format("%s Module Not Selected. Operation Abort", pCTA4->GetStnName());
//			HMIMessageBox(MSG_OK, "WARNING", strMsg);
//			bResult = FALSE;
//		}
//		else
//		{
//			for(INT i = 0; i <= lCount; i++)
//			{
//				pCTA4->m_lCurDisplayPRU = MBDL_PRU;
//				pCTA4->m_lCurGlass = GLASS1;
//				pCTA4->m_lCORCamNum = MBDL1_COR;
//				if (!pCTA4->TA4CORTest(&pruMBDL_Calib[GLASS1] , lAutoTestCount, TRUE))
//				{
//					bResult = FALSE;
//					break;
//				}	
//			}
//
//			if(pCHouseKeeping->HMI_bAutoTestStop)
//			{
//				break;
//			}
//
//	
//			for(INT i = 0; i <= lCount; i++)
//			{
//				pCTA4->m_lCurDisplayPRU = MBDL_PRU;
//				pCTA4->m_lCurGlass = GLASS2;
//				pCTA4->m_lCORCamNum = MBDL2_COR;
//				if (!pCTA4->TA4CORTest(&pruMBDL_Calib[GLASS2] , lAutoTestCount, TRUE))
//				{
//					bResult = FALSE;
//					break;
//				}
//			}
//		}
//		}while(!pCHouseKeeping->HMI_bAutoTestStop);
//			if (pCHouseKeeping->HMI_bAutoTestStop)
//				{
//					pCHouseKeeping->HMI_bAutoTestStop = FALSE;
//				}
//		}
//		
//		else if (lAnswer == rMSG_CANCEL || lAnswer == rMSG_TIMEOUT)
//		{
//			return 1;
//			break;
//		}
//	} while (0);
//
//	if (bResult)
//	{
//		HMIMessageBox(MSG_OK, "TA4 COR TEST", "OPERATION SUCCESS");
//	}
//	else
//	{
//		HMIMessageBox(MSG_OK, "TA4 COR TEST", "OPERATION FAIL");
//	}
//
//	return 1;
//}

LONG CWinEagle::HMI_DistortionCal(IPC_CServiceMessage &svMsg)
{
	CTA1 *pCTA1	= (CTA1*)m_pModule->GetStation("TA1");

	BOOL bResult = TRUE;
	PRU *pPRU;
	CString strMsg;
	LONG lAnswer = -1;

	pPRU = &SelectCurPRU(HMI_ulCurPRU);

	switch (HMI_ulCurPRU)
	{
	case ACF1WH_CALIB_PRU:
		break;
	
	case ACF2WH_CALIB_PRU:
		break;

	case ACFUL_CALIB_PRU:
		bResult = pCTA1->DistortionCalibration(pPRU);
		break;

	}
	if (bResult)
	{
		HMIMessageBox(MSG_OK, "DISTORTION CAL", "OPERATION SUCCESS");
	}
	else
	{
		HMIMessageBox(MSG_OK, "DISTORTION CAL", "OPERATION FAIL");
	}

	UpdateHMICalibPRButton(FALSE);

	BOOL bCalInProgress = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bCalInProgress);


	return 1;
}