/////////////////////////////////////////////////////////////////
//	TimeChart.cpp : interface of the CTimeChart class
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

#define TIMECHART_EXTERN
#include "TimeChart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const CString gszTCHART_DATA_PATH	= _T("D:\\sw\\AC9000\\DATA\\");

IMPLEMENT_DYNCREATE(CTimeChart, CAC9000Stn)

CTimeChart::CTimeChart()
{
	m_nStateDiff					= 4;
	m_nObjDiff						= 10;
	m_szFileName					= _T("001.tic");

	m_logfp							= NULL;
	m_dwStartTime					= 0;

	// register new time chart objects
	TC_RegisterObj();

	m_bLogging						= FALSE;

	// -------------------------------------------------------
	// HMI varibales
	// -------------------------------------------------------
	HMI_bSolSelected				= TRUE;
}

CTimeChart::~CTimeChart()
{
}

BOOL CTimeChart::InitInstance()
{
	CAC9000Stn::InitInstance();
	
	m_bModSelected = FALSE;

	return TRUE;
}

INT CTimeChart::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CAC9000Stn::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CTimeChart::Operation()
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

VOID CTimeChart::UpdateOutput()
{
	if (!m_fHardware)
	{
		return;
	}

	if ((m_qState == UN_INITIALIZE_Q) || (m_qState == DE_INITIAL_Q))
	{
		return;
	}

	try
	{
		//Update sensor here
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
}

VOID CTimeChart::RegisterVariables()
{
	try
	{
		// -------------------------------------------------------
		// AC9000Stn Variable and Function
		// -------------------------------------------------------
		RegVariable(GetStation() + _T("bModSelected"), &m_bModSelected);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetModSelected"), HMI_SetModSelected);

		// -------------------------------------------------------
		// varibales
		// -------------------------------------------------------
		RegVariable(GetStation() + _T("InConv.bSelected"), &stTimeObj[IN_CONV].bSelected);
		RegVariable(GetStation() + _T("InConv_SOL.bSelected"), &stTimeObj[IN_CONV_SOL].bSelected);

		RegVariable(GetStation() + _T("INSH.bSelected"), &stTimeObj[INSH_X].bSelected);
		RegVariable(GetStation() + _T("INSH_SOL.bSelected"), &stTimeObj[INSH_SOL].bSelected);

		RegVariable(GetStation() + _T("INWH.bSelected"), &stTimeObj[INWH_Y].bSelected);
		RegVariable(GetStation() + _T("INWH_SOL.bSelected"), &stTimeObj[INWH_SOL].bSelected);

		RegVariable(GetStation() + _T("ACF1WH.bSelected"), &stTimeObj[ACF1WH_Y].bSelected);
		RegVariable(GetStation() + _T("ACF1WH_SOL.bSelected"), &stTimeObj[ACF1WH_SOL].bSelected);
		RegVariable(GetStation() + _T("ACF2WH.bSelected"), &stTimeObj[ACF2WH_Y].bSelected);
		RegVariable(GetStation() + _T("ACF2WH_SOL.bSelected"), &stTimeObj[ACF2WH_SOL].bSelected);

		RegVariable(GetStation() + _T("PBWH.bSelected"), &stTimeObj[PBWH_X1].bSelected);
		RegVariable(GetStation() + _T("PBWH_SOL.bSelected"), &stTimeObj[PBWH1_SOL].bSelected);

		RegVariable(GetStation() + _T("MB1WH.bSelected"), &stTimeObj[MB1WH_Y].bSelected);
		RegVariable(GetStation() + _T("MB1WH_SOL.bSelected"), &stTimeObj[MB1WH_SOL].bSelected);
		RegVariable(GetStation() + _T("MB2WH.bSelected"), &stTimeObj[MB2WH_Y].bSelected);
		RegVariable(GetStation() + _T("MB2WH_SOL.bSelected"), &stTimeObj[MB2WH_SOL].bSelected);

		RegVariable(GetStation() + _T("MB3WH.bSelected"), &stTimeObj[MB3WH_Y].bSelected);
		RegVariable(GetStation() + _T("MB3WH_SOL.bSelected"), &stTimeObj[MB3WH_SOL].bSelected);
		RegVariable(GetStation() + _T("MB4WH.bSelected"), &stTimeObj[MB4WH_Y].bSelected);
		RegVariable(GetStation() + _T("MB4WH_SOL.bSelected"), &stTimeObj[MB4WH_SOL].bSelected);

		RegVariable(GetStation() + _T("MB5WH.bSelected"), &stTimeObj[MB5WH_Y].bSelected);
		RegVariable(GetStation() + _T("MB5WH_SOL.bSelected"), &stTimeObj[MB5WH_SOL].bSelected);
		RegVariable(GetStation() + _T("MB6WH.bSelected"), &stTimeObj[MB6WH_Y].bSelected);
		RegVariable(GetStation() + _T("MB6WH_SOL.bSelected"), &stTimeObj[MB6WH_SOL].bSelected);

		RegVariable(GetStation() + _T("TA1.bSelected"), &stTimeObj[TA1_X].bSelected);
		RegVariable(GetStation() + _T("TA1_SOL.bSelected"), &stTimeObj[TA1_SOL].bSelected);
		RegVariable(GetStation() + _T("TA2.bSelected"), &stTimeObj[TA2_X].bSelected);
		RegVariable(GetStation() + _T("TA2_SOL.bSelected"), &stTimeObj[TA2_SOL].bSelected);
		RegVariable(GetStation() + _T("TA3.bSelected"), &stTimeObj[TA3_X].bSelected);
		RegVariable(GetStation() + _T("TA3_SOL.bSelected"), &stTimeObj[TA3_SOL].bSelected);
		RegVariable(GetStation() + _T("TA4.bSelected"), &stTimeObj[TA4_X].bSelected);
		RegVariable(GetStation() + _T("TA4_SOL.bSelected"), &stTimeObj[TA4_SOL].bSelected);
		RegVariable(GetStation() + _T("TA5.bSelected"), &stTimeObj[TA5_X].bSelected);
		RegVariable(GetStation() + _T("TA5_SOL.bSelected"), &stTimeObj[TA5_SOL].bSelected);

		RegVariable(GetStation() + _T("ACF1.bSelected"), &stTimeObj[ACF1_INDEX].bSelected);
		RegVariable(GetStation() + _T("ACF1_SOL.bSelected"), &stTimeObj[ACF1_SOL].bSelected);
		RegVariable(GetStation() + _T("ACF2.bSelected"), &stTimeObj[ACF2_INDEX].bSelected);
		RegVariable(GetStation() + _T("ACF2_SOL.bSelected"), &stTimeObj[ACF2_SOL].bSelected);

		RegVariable(GetStation() + _T("PBUplook.bSelected"), &stTimeObj[PB_UPLOOK_X].bSelected);

		RegVariable(GetStation() + _T("PB1.bSelected"), &stTimeObj[PB1_Z].bSelected);
		RegVariable(GetStation() + _T("PB1_SOL.bSelected"), &stTimeObj[PB1_SOL].bSelected);
		RegVariable(GetStation() + _T("PB2.bSelected"), &stTimeObj[PB2_Z].bSelected);
		RegVariable(GetStation() + _T("PB2_SOL.bSelected"), &stTimeObj[PB2_SOL].bSelected);

		RegVariable(GetStation() + _T("MB1.bSelected"), &stTimeObj[MB1_Z].bSelected);
		RegVariable(GetStation() + _T("MB1_SOL.bSelected"), &stTimeObj[MB1_SOL].bSelected);
		RegVariable(GetStation() + _T("MB2.bSelected"), &stTimeObj[MB2_Z].bSelected);
		RegVariable(GetStation() + _T("MB2_SOL.bSelected"), &stTimeObj[MB2_SOL].bSelected);

		RegVariable(GetStation() + _T("TrayHolder.bSelected"), &stTimeObj[FPC_TL_X1].bSelected);
		RegVariable(GetStation() + _T("TrayHolder_SOL.bSelected"), &stTimeObj[TRAY1_DOWN_SOL].bSelected);

		RegVariable(GetStation() + _T("SH.bSelected"), &stTimeObj[SH_X].bSelected);
		RegVariable(GetStation() + _T("SH_SOL.bSelected"), &stTimeObj[SH_SOL].bSelected);

		RegVariable(GetStation() + _T("OutConv_SOL.bSelected"), &stTimeObj[OUT_CONV_SOL].bSelected);

		RegVariable(GetStation() + _T("SOL.bSelected"), &HMI_bSolSelected);

		// -------------------------------------------------------
		// commands
		// -------------------------------------------------------
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetInConvSelected"), HMI_SetInConvSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetINSHSelected"), HMI_SetINSHSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetINWHSelected"), HMI_SetINWHSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetACF1WHSelected"), HMI_SetACF1WHSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetACF2WHSelected"), HMI_SetACF2WHSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPBWHSelected"), HMI_SetPBWHSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetMB1WHSelected"), HMI_SetMB1WHSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetMB2WHSelected"), HMI_SetMB2WHSelected);	
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetMB3WHSelected"), HMI_SetMB3WHSelected);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetMB4WHSelected"), HMI_SetMB4WHSelected);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetMB5WHSelected"), HMI_SetMB5WHSelected);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetMB6WHSelected"), HMI_SetMB6WHSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetTA1Selected"), HMI_SetTA1Selected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetTA2Selected"), HMI_SetTA2Selected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetTA3Selected"), HMI_SetTA3Selected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetTA4Selected"), HMI_SetTA4Selected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetTA5Selected"), HMI_SetTA5Selected);		
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetACF1Selected"), HMI_SetACF1Selected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetACF2Selected"), HMI_SetACF2Selected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPBUplookSelected"), HMI_SetPBUplookSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPB1Selected"), HMI_SetPB1Selected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPB2Selected"), HMI_SetPB2Selected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetMB1Selected"), HMI_SetMB1Selected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetMB2Selected"), HMI_SetMB2Selected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetTrayHolderSelected"), HMI_SetTrayHolderSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetSHSelected"), HMI_SetSHSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetOutConvSelected"), HMI_SetOutConvSelected);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetSOLSelected"), HMI_SetSOLSelected);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SelectAll"), HMI_SelectAll);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("AddCmd"), IPC_AddCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("DelCmd"), IPC_DelCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ResetTimer"), IPC_ResetTimer);
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

VOID CTimeChart::PreStartOperation()
{
	INT i = 0;
	INT nObjCounter = 0;

	if (!m_bModSelected)
	{
		m_bLogging = FALSE;
	}
	else
	{
		DisplayMessage("*******************************");
		DisplayMessage("TimeChart Module is enabled."); //20130513
		DisplayMessage("*******************************");
		
		// open file
		m_logfp = fopen(gszTCHART_DATA_PATH + m_szFileName, "a+");

		if (m_logfp != NULL)
		{
			__time64_t ltime;
			CString szTime = "";

			_time64(&ltime);
			szTime = _ctime64(&ltime);
			fprintf(m_logfp, "Start time: " + szTime);
			fprintf(m_logfp, "Time\t");

			// assign weights to objects
			for (i = 0; i < NUM_OF_TC_OBJECT; i++)
			{
				if (stTimeObj[i].bSelected)
				{
					nObjCounter++;
				}
			}

			for (i = 0; i < NUM_OF_TC_OBJECT; i++)
			{
				if (stTimeObj[i].bSelected)
				{
					nObjCounter--;
					stTimeObj[i].bStatus = FALSE;
					stTimeObj[i].nWeight = (nObjCounter) * m_nObjDiff;
					fprintf(m_logfp, "%s\t", (const char*)stTimeObj[i].stObjName);
				}
			}
			fprintf(m_logfp, "\n");

			m_dwStartTime = GetTickCount();
			m_bLogging = TRUE;
		}
		else
		{
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			DisplayMessage("ERROR: Unable to create Time Chart Log File");
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		}
	}

	Motion(FALSE);
	State(IDLE_Q);
}

VOID CTimeChart::AutoOperation()
{
	// stop command received: end operation after cycle completed
	if (Command() == glSTOP_COMMAND || g_bStopCycle)
	{
		m_bLogging = FALSE;
	}

	if (!m_bLogging)
	{
		// close file
		if (m_logfp != NULL)
		{
			fprintf(m_logfp, "\nEnd of Time Chart Log File\n\n");
			fclose(m_logfp);
		}

		m_comClient.PurgeReplyQueue();

		State(STOPPING_Q);
	}
}


/////////////////////////////////////////////////////////////////
//Function delcaration
/////////////////////////////////////////////////////////////////

VOID CTimeChart::TC_RegisterObj()
{
	INT i = 0;

	// 20140523 Yip: TC entry re-order
	stTimeObj[IN_CONV].stObjName = "InConv Belt";
	stTimeObj[IN_CONV_SOL].stObjName = "InConv Sol";
	stTimeObj[IN_CONV_X].stObjName = "InConv X";
	stTimeObj[INTRAY_OPEN_SOL].stObjName = "Tray In Elev Open Sol";
	stTimeObj[INTRAY_SPLIT_SOL].stObjName = "Tray In Elev Split Sol";
	stTimeObj[TRAY1_DOWN_SOL].stObjName = "Tray Indexer 1 Down Sol";
	stTimeObj[TRAY2_DOWN_SOL].stObjName = "Tray Indexer 2 Down Sol";
	stTimeObj[FPC_TL_X1].stObjName = "Tray Indexer 1 X";
	stTimeObj[FPC_TL_X2].stObjName = "Tray Indexer 2 X";
	stTimeObj[FPC_TL_Z1].stObjName = "Tray In Elev Z";
	stTimeObj[FPC_TL_Z2].stObjName = "Tray Out Elev Z";
	stTimeObj[OUTTRAY_OPEN_SOL].stObjName = "Tray Out Elev Open Sol";
	stTimeObj[FPC_PA_X].stObjName = "FPC Pick Arm X";
	stTimeObj[FPC_PA_Y].stObjName = "FPC Pick Arm Y";
	stTimeObj[FPC_PA_Z1].stObjName = "FPC Pick Arm Z1";
	stTimeObj[FPC_PA_T1].stObjName = "FPC Pick Arm T1";
	stTimeObj[FPC_PA_Z2].stObjName = "FPC Pick Arm Z2";
	stTimeObj[FPC_PA_T2].stObjName = "FPC Pick Arm T2";
	stTimeObj[SH_X].stObjName = "Shuttle X";
	stTimeObj[SH_Y].stObjName = "Shuttle Y";
	stTimeObj[SH_REJECT_BIN_SOL].stObjName = "Shuttle Reject Bin Sol";
	stTimeObj[SH_FPC_PRESSER_SOL].stObjName = "Shuttle FPC Presser Sol";
	stTimeObj[SH_IONIZER_AIR_SOL].stObjName = "Shuttle Ionizer Air Sol";
	stTimeObj[SH_IONIZER_PWR_ENABLE].stObjName = "Shuttle Ionizer Power Enable Sol";
	stTimeObj[SH_SOL].stObjName = "Shuttle Sol";
	stTimeObj[FPC_RA_X_SOL].stObjName = "FPC Reject Arm X Sol";
	stTimeObj[FPC_RA_Z_SOL].stObjName = "FPC Reject Arm Z Sol";
	stTimeObj[FPC_PRESS1_SOL].stObjName = "FPC Pressor 1 Sol";
	stTimeObj[FPC_PRESS2_SOL].stObjName = "FPC Pressor 2 Sol";
	stTimeObj[INSH_X].stObjName = "INSH X";
	stTimeObj[INSH_Y].stObjName = "INSH Y";
	stTimeObj[INSH_Z1].stObjName = "INSH Z1";
	stTimeObj[INSH_Z2].stObjName = "INSH Z2";
	stTimeObj[INSH_EXTEND_SOL].stObjName = "INSH Extend Sol";
	stTimeObj[INSH_SOL].stObjName = "INSH Sol";
	stTimeObj[REJECT_BELT].stObjName = "Reject Belt";
	stTimeObj[IN_RA_GO_FRONT_SOL].stObjName = "Reject Arm Go Front Sol";
	stTimeObj[TA1_X].stObjName = "TA1 X";
	stTimeObj[TA1_Z].stObjName = "TA1 Z";
	stTimeObj[TA1_SOL].stObjName = "TA1 Sol";
	stTimeObj[INWH_Y].stObjName = "INWH Y";
	stTimeObj[INWH_Z1].stObjName = "INWH Z1";
	stTimeObj[INWH_Z2].stObjName = "INWH Z2";
	stTimeObj[INWH_T].stObjName = "INWH T";
	stTimeObj[INWH_SOL].stObjName = "INWH Sol";
	stTimeObj[TA2_X].stObjName = "TA2 X";
	stTimeObj[TA2_SOL].stObjName = "TA2 Sol";
	stTimeObj[ACF1WH_Y].stObjName = "ACF1WH Y";
	stTimeObj[ACF1WH_Z].stObjName = "ACF1WH Z";
	stTimeObj[ACF1WH_SOL].stObjName = "ACF1WH Sol";
	stTimeObj[ACF1_INDEX].stObjName = "ACF1 Index";
	stTimeObj[ACF1_ROLLER].stObjName = "ACF1 Roller";
	stTimeObj[ACF1_SOL].stObjName = "ACF1 Sol";
	stTimeObj[ACF1_STRIPPER_X].stObjName = "ACF1 STRIPPER X";
	stTimeObj[ACF2WH_Y].stObjName = "ACF2WH Y";
	stTimeObj[ACF2WH_Z].stObjName = "ACF2WH Z";
	stTimeObj[ACF2WH_SOL].stObjName = "ACF2WH Sol";
	stTimeObj[ACF2_INDEX].stObjName = "ACF2 Index";
	stTimeObj[ACF2_ROLLER].stObjName = "ACF2 Roller";
	stTimeObj[ACF2_SOL].stObjName = "ACF2 Sol";
	stTimeObj[ACF2_STRIPPER_X].stObjName = "ACF2 STRIPPER X";
	stTimeObj[TA3_X].stObjName = "TA3 X";
	stTimeObj[TA3_SOL].stObjName = "TA3 Sol";
	stTimeObj[PBWH_X1].stObjName = "PBWH X1";
	stTimeObj[PBWH_Y1].stObjName = "PBWH Y1";
	stTimeObj[PBWH_Z1].stObjName = "PBWH Z1";
	stTimeObj[PBWH1_SOL].stObjName = "PBWH1 Sol";
	stTimeObj[PBWH_X2].stObjName = "PBWH X2";
	stTimeObj[PBWH_Y2].stObjName = "PBWH Y2";
	stTimeObj[PBWH_Z2].stObjName = "PBWH Z2";
	stTimeObj[PBWH_T].stObjName = "PBWH T";
	stTimeObj[PBWH2_SOL].stObjName = "PBWH2 Sol";
	stTimeObj[PB1_Z].stObjName = "PB1 Z";
	stTimeObj[PB1_T].stObjName = "PB1 T";
	stTimeObj[PB1_FPC_REJECT_X].stObjName = "FPC REJECT X"; //20150228
	stTimeObj[PB1_SOL].stObjName = "PB1 Sol";
	stTimeObj[PB2_Z].stObjName = "PB2 Z";
	stTimeObj[PB2_T].stObjName = "PB2 T";
	stTimeObj[PB2_SOL].stObjName = "PB2 Sol";
	stTimeObj[PB_UPLOOK_X].stObjName = "PB Uplook X";
	stTimeObj[PB_UPLOOK_Y].stObjName = "PB Uplook Y";
	stTimeObj[TA4_X].stObjName = "TA4 X";
	stTimeObj[TA4_UPDNLK_Y].stObjName = "TA4 Up/Down Look Y";
	stTimeObj[TA4_T1].stObjName = "TA4 T1";
	stTimeObj[TA4_T2].stObjName = "TA4 T2";
	stTimeObj[TA4_EXTEND_SOL].stObjName = "TA4 Extend Sol";
	stTimeObj[TA4_SOL].stObjName = "TA4 Sol";
	stTimeObj[MB1WH_Y].stObjName = "MB1WH Y";
	stTimeObj[MB1WH_Z].stObjName = "MB1WH Z";
	stTimeObj[MB1WH_SOL].stObjName = "MB1WH Sol";
	stTimeObj[MB2WH_Y].stObjName = "MB2WH Y";
	stTimeObj[MB2WH_Z].stObjName = "MB2WH Z";
	stTimeObj[MB2WH_SOL].stObjName = "MB2WH Sol";
	stTimeObj[MB1_Z].stObjName = "MB1 Z";
	stTimeObj[MB1_INDEX].stObjName = "MB1 Index";
	stTimeObj[MB1_REWINDER1].stObjName = "MB1 Rewinder1";
	stTimeObj[MB1_REWINDER2].stObjName = "MB1 Rewinder2";
	stTimeObj[MB1_SOL].stObjName = "MB1 Sol";
	stTimeObj[MB3WH_Y].stObjName = "MB3WH Y";
	stTimeObj[MB3WH_Z].stObjName = "MB3WH Z";
	stTimeObj[MB3WH_SOL].stObjName = "MB3WH Sol";
	stTimeObj[MB4WH_Y].stObjName = "MB4WH Y";
	stTimeObj[MB4WH_Z].stObjName = "MB4WH Z";
	stTimeObj[MB4WH_SOL].stObjName = "MB4WH Sol";
	stTimeObj[MB2_Z].stObjName = "MB2 Z";
	stTimeObj[MB2_INDEX].stObjName = "MB2 Index";
	stTimeObj[MB2_REWINDER1].stObjName = "MB2 Rewinder1";
	stTimeObj[MB2_REWINDER2].stObjName = "MB2 Rewinder2";
	stTimeObj[MB2_SOL].stObjName = "MB2 Sol";
	stTimeObj[MB5WH_Y].stObjName = "MB5WH Y";
	stTimeObj[MB5WH_Z].stObjName = "MB5WH Z";
	stTimeObj[MB5WH_SOL].stObjName = "MB5WH Sol";
	stTimeObj[MB6WH_Y].stObjName = "MB6WH Y";
	stTimeObj[MB6WH_Z].stObjName = "MB6WH Z";
	stTimeObj[MB6WH_SOL].stObjName = "MB6WH Sol";
	stTimeObj[MB3_Z].stObjName = "MB3 Z";
	stTimeObj[MB3_INDEX].stObjName = "MB3 Index";
	stTimeObj[MB3_REWINDER1].stObjName = "MB3 Rewinder1";
	stTimeObj[MB3_REWINDER2].stObjName = "MB3 Rewinder2";
	stTimeObj[MB3_SOL].stObjName = "MB3 Sol";
	stTimeObj[TA5_X].stObjName = "TA5 X";
	stTimeObj[TA5_SOL].stObjName = "TA5 Sol";
	stTimeObj[OUTWH_Y].stObjName = "OUTWH Y";
	stTimeObj[OUTWH_Z1].stObjName = "OUTWH Z1";
	stTimeObj[OUTWH_Z2].stObjName = "OUTWH Z2";
	stTimeObj[OUTWH_SOL].stObjName = "OUTWH Sol";
	stTimeObj[OUT_CONV_SOL].stObjName = "OutConv Sol";
	stTimeObj[OTHER_SOL].stObjName = "Other Sol";


	// 20140523 Yip: TC entry re-order
	stTimeObj[IN_WH_PR_GRAB].stObjName = "INWH Grab";
	stTimeObj[IN_WH_PR_PROCESS].stObjName = "INWH Process";
	stTimeObj[ACF1WH_PR_GRAB].stObjName = "ACF1WH Grab";
	stTimeObj[ACF1WH_PR_PROCESS].stObjName = "ACF1WH Process";
	stTimeObj[ACF2WH_PR_GRAB].stObjName = "ACF2WH Grab";
	stTimeObj[ACF2WH_PR_PROCESS].stObjName = "ACF2WH Process";
	stTimeObj[PB_GLASS_PR_GRAB].stObjName = "PB Glass Grab";
	stTimeObj[PB_GLASS_PR_PROCESS].stObjName = "PB Glass Process";
	stTimeObj[PB_LSI_PR_GRAB].stObjName = "PB FPC Grab";
	stTimeObj[PB_LSI_PR_PROCESS].stObjName = "PB FPC Process";
	stTimeObj[SH_PR_GRAB].stObjName = "Shuttle Grab";
	stTimeObj[SH_PR_PROCESS].stObjName = "Shuttle Process";

	for (i = 0; i < NUM_OF_TC_OBJECT; i++)
	{
		stTimeObj[i].bSelected	= TRUE;
		stTimeObj[i].nObjID		= i;

		stTimeObj[i].nWeight	= 0;
		stTimeObj[i].bStatus	= FALSE;
		stTimeObj[i].nJobCount	= 0;
	}

	if (MAX_NUM_OF_MAINBOND < 3)	// 20140828 Yip: Disable Time Chart For Main Bond 3 If Not Exist
	{
		for (i = MB5WH_Y; i <= MB3_SOL; i++)
		{
			stTimeObj[i].bSelected	= FALSE;
		}
	}
}

VOID CTimeChart::TC_AddCmd(INT emTCObjectID)
{
	if ((emTCObjectID < 0) || (emTCObjectID >= NUM_OF_TC_OBJECT))	// 20140717 Yip
	{
		CString szMsg;
		szMsg.Format("TimeChart AddCmd with Invalid ID %d", emTCObjectID);
		DisplayMessage(szMsg);
		return;
	}

	if (!m_bModSelected || !m_bLogging || !stTimeObj[emTCObjectID].bSelected)
	{
		return;
	}

	// print to file
	if (m_logfp == NULL)
	{
		return;
	}

	DWORD dwTime = GetTickCount() - m_dwStartTime;

	// log before
	fprintf(m_logfp, "%ld\t", dwTime);

	for (INT i = 0; i < NUM_OF_TC_OBJECT; i++)
	{
		if (stTimeObj[i].bSelected)
		{
			if (stTimeObj[i].bStatus)
			{
				fprintf(m_logfp, "%d\t", stTimeObj[i].nWeight + m_nStateDiff);
			}
			else
			{
				fprintf(m_logfp, "%d\t", stTimeObj[i].nWeight);
			}
		}
	}
	fprintf(m_logfp, "\n");

	stTimeObj[emTCObjectID].bStatus = TRUE;

	// log after
	fprintf(m_logfp, "%ld\t", dwTime);

	for (INT i = 0; i < NUM_OF_TC_OBJECT; i++)
	{
		if (stTimeObj[i].bSelected)
		{
			if (stTimeObj[i].bStatus)
			{
				fprintf(m_logfp, "%d\t", stTimeObj[i].nWeight + m_nStateDiff);
			}
			else
			{
				fprintf(m_logfp, "%d\t", stTimeObj[i].nWeight);
			}
		}
	}
	fprintf(m_logfp, "\n");
}

VOID CTimeChart::TC_DelCmd(INT emTCObjectID)
{
	if ((emTCObjectID < 0) || (emTCObjectID >= NUM_OF_TC_OBJECT))	// 20140717 Yip
	{
		CString szMsg;
		szMsg.Format("TimeChart DelCmd with Invalid ID %d", emTCObjectID);
		DisplayMessage(szMsg);
		return;
	}

	if (!m_bModSelected || !m_bLogging || !stTimeObj[emTCObjectID].bSelected)
	{
		return;
	}

	// print to file
	if (m_logfp == NULL)
	{
		return;
	}

	DWORD dwTime = GetTickCount() - m_dwStartTime;

	// log before
	fprintf(m_logfp, "%ld\t", dwTime);

	for (INT i = 0; i < NUM_OF_TC_OBJECT; i++)
	{
		if (stTimeObj[i].bSelected)
		{
			if (stTimeObj[i].bStatus)
			{
				fprintf(m_logfp, "%d\t", stTimeObj[i].nWeight + m_nStateDiff);
			}
			else
			{
				fprintf(m_logfp, "%d\t", stTimeObj[i].nWeight);
			}
		}
	}
	fprintf(m_logfp, "\n");

	stTimeObj[emTCObjectID].bStatus = FALSE;

	// log after
	fprintf(m_logfp, "%ld\t", dwTime);

	for (INT i = 0; i < NUM_OF_TC_OBJECT; i++)
	{
		if (stTimeObj[i].bSelected)
		{
			if (stTimeObj[i].bStatus)
			{
				fprintf(m_logfp, "%d\t", stTimeObj[i].nWeight + m_nStateDiff);
			}
			else
			{
				fprintf(m_logfp, "%d\t", stTimeObj[i].nWeight);
			}
		}
	}
	fprintf(m_logfp, "\n");
}

VOID CTimeChart::TC_ResetTimer()
{
	if (!m_bModSelected || !m_bLogging)
	{
		return;
	}

	// print to file
	if (m_logfp == NULL)
	{
		return;
	}

	fprintf(m_logfp, "Unit Bonded. Cycle Time: %ld\n\n", GetTickCount() - m_dwStartTime);

	fprintf(m_logfp, "Time\t");

	for (INT i = 0; i < NUM_OF_TC_OBJECT; i++)
	{
		if (stTimeObj[i].bSelected)
		{
			fprintf(m_logfp, "%s\t", (const char*)stTimeObj[i].stObjName);
		}
	}
	fprintf(m_logfp, "\n");

	m_dwStartTime = GetTickCount();
}


//-----------------------------------------------------------------------------
// HMI command implementation
//-----------------------------------------------------------------------------
//

LONG CTimeChart::HMI_SetModSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		m_bModSelected = TRUE;
	}
	else
	{
		m_bModSelected = FALSE;
	}

	return 0;
}

LONG CTimeChart::HMI_SetInConvSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[IN_CONV].bSelected = TRUE;
		stTimeObj[IN_CONV_X].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[IN_CONV_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[IN_CONV].bSelected = FALSE;
		stTimeObj[IN_CONV_X].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[IN_CONV_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetINSHSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[INSH_X].bSelected = TRUE;
		stTimeObj[INSH_Y].bSelected = TRUE;
		stTimeObj[INSH_Z1].bSelected = TRUE;
		stTimeObj[INSH_Z2].bSelected = TRUE;
		stTimeObj[REJECT_BELT].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[INSH_EXTEND_SOL].bSelected = TRUE;
			stTimeObj[IN_RA_GO_FRONT_SOL].bSelected = TRUE;
			stTimeObj[INSH_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[INSH_X].bSelected = FALSE;
		stTimeObj[INSH_Y].bSelected = FALSE;
		stTimeObj[INSH_Z1].bSelected = FALSE;
		stTimeObj[INSH_Z2].bSelected = FALSE;
		stTimeObj[REJECT_BELT].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[INSH_EXTEND_SOL].bSelected = FALSE;
			stTimeObj[IN_RA_GO_FRONT_SOL].bSelected = FALSE;
			stTimeObj[INSH_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetINWHSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[INWH_Y].bSelected = TRUE;
		stTimeObj[INWH_Z1].bSelected = TRUE;
		stTimeObj[INWH_Z2].bSelected = TRUE;
		stTimeObj[INWH_T].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[INWH_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[INWH_Y].bSelected = FALSE;
		stTimeObj[INWH_Z1].bSelected = FALSE;
		stTimeObj[INWH_Z2].bSelected = FALSE;
		stTimeObj[INWH_T].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[INWH_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetACF1WHSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[ACF1WH_Y].bSelected = TRUE;
		stTimeObj[ACF1WH_Z].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[ACF1WH_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[ACF1WH_Y].bSelected = FALSE;
		stTimeObj[ACF1WH_Z].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[ACF1WH_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetACF2WHSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[ACF2WH_Y].bSelected = TRUE;
		stTimeObj[ACF2WH_Z].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[ACF2WH_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[ACF2WH_Y].bSelected = FALSE;
		stTimeObj[ACF2WH_Z].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[ACF2WH_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetPBWHSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[PBWH_X1].bSelected = TRUE;
		stTimeObj[PBWH_Y1].bSelected = TRUE;
		stTimeObj[PBWH_Z1].bSelected = TRUE;
		stTimeObj[PBWH_X2].bSelected = TRUE;
		stTimeObj[PBWH_Y2].bSelected = TRUE;
		stTimeObj[PBWH_Z2].bSelected = TRUE;
		stTimeObj[PBWH_T].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[PBWH1_SOL].bSelected = TRUE;
			stTimeObj[PBWH2_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[PBWH_X1].bSelected = FALSE;
		stTimeObj[PBWH_Y1].bSelected = FALSE;
		stTimeObj[PBWH_Z1].bSelected = FALSE;
		stTimeObj[PBWH_X2].bSelected = FALSE;
		stTimeObj[PBWH_Y2].bSelected = FALSE;
		stTimeObj[PBWH_Z2].bSelected = FALSE;
		stTimeObj[PBWH_T].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[PBWH1_SOL].bSelected = FALSE;
			stTimeObj[PBWH2_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetMB1WHSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[MB1WH_Y].bSelected = TRUE;
		stTimeObj[MB1WH_Z].bSelected = TRUE;
		stTimeObj[MB2WH_Y].bSelected = TRUE;
		stTimeObj[MB2WH_Z].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[MB1WH_SOL].bSelected = TRUE;
			stTimeObj[MB2WH_SOL].bSelected = TRUE;

		}
	}
	else
	{
		stTimeObj[MB1WH_Y].bSelected = FALSE;
		stTimeObj[MB1WH_Z].bSelected = FALSE;
		stTimeObj[MB2WH_Y].bSelected = FALSE;
		stTimeObj[MB2WH_Z].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[MB1WH_SOL].bSelected = FALSE;
			stTimeObj[MB2WH_SOL].bSelected = FALSE;

		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetMB2WHSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[MB3WH_Y].bSelected = TRUE;
		stTimeObj[MB3WH_Z].bSelected = TRUE;
		stTimeObj[MB4WH_Y].bSelected = TRUE;
		stTimeObj[MB4WH_Z].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[MB3WH_SOL].bSelected = TRUE;
			stTimeObj[MB4WH_SOL].bSelected = TRUE;

		}
	}
	else
	{
		stTimeObj[MB3WH_Y].bSelected = FALSE;
		stTimeObj[MB3WH_Z].bSelected = FALSE;
		stTimeObj[MB4WH_Y].bSelected = FALSE;
		stTimeObj[MB4WH_Z].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[MB3WH_SOL].bSelected = FALSE;
			stTimeObj[MB4WH_SOL].bSelected = FALSE;

		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetTA1Selected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[TA1_X].bSelected = TRUE;
		stTimeObj[TA1_Z].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[TA1_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[TA1_X].bSelected = FALSE;
		stTimeObj[TA1_Z].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[TA1_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetTA2Selected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[TA2_X].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[TA2_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[TA2_X].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[TA2_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetTA3Selected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[TA3_X].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[TA3_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[TA3_X].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[TA3_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetTA4Selected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[TA4_X].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[TA4_UPDNLK_Y].bSelected = TRUE;
			stTimeObj[TA4_T1].bSelected = TRUE;
			stTimeObj[TA4_T2].bSelected = TRUE;
			stTimeObj[TA4_EXTEND_SOL].bSelected = TRUE;
			stTimeObj[TA4_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[TA4_X].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[TA4_UPDNLK_Y].bSelected = FALSE;
			stTimeObj[TA4_T1].bSelected = FALSE;
			stTimeObj[TA4_T2].bSelected = FALSE;
			stTimeObj[TA4_EXTEND_SOL].bSelected = FALSE;
			stTimeObj[TA4_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetTA5Selected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[TA5_X].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[TA5_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[TA5_X].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[TA5_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetACF1Selected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[ACF1_INDEX].bSelected = TRUE;
		stTimeObj[ACF1_ROLLER].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[ACF1_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[ACF1_INDEX].bSelected = FALSE;
		stTimeObj[ACF1_ROLLER].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[ACF1_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetACF2Selected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[ACF2_INDEX].bSelected = TRUE;
		stTimeObj[ACF2_ROLLER].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[ACF2_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[ACF2_INDEX].bSelected = FALSE;
		stTimeObj[ACF2_ROLLER].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[ACF2_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetPBUplookSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[PB_UPLOOK_X].bSelected = TRUE;
		stTimeObj[PB_UPLOOK_Y].bSelected = TRUE;
	}
	else
	{
		stTimeObj[PB_UPLOOK_X].bSelected = FALSE;
		stTimeObj[PB_UPLOOK_Y].bSelected = FALSE;
	}
	return 0;
}

LONG CTimeChart::HMI_SetPB1Selected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[PB1_Z].bSelected = TRUE;
		stTimeObj[PB1_T].bSelected = TRUE;
		stTimeObj[PB1_FPC_REJECT_X].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[PB1_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[PB1_Z].bSelected = FALSE;
		stTimeObj[PB1_T].bSelected = FALSE;
		stTimeObj[PB1_FPC_REJECT_X].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[PB1_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetPB2Selected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[PB2_Z].bSelected = TRUE;
		stTimeObj[PB2_T].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[PB2_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[PB2_Z].bSelected = FALSE;
		stTimeObj[PB2_T].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[PB2_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetMB1Selected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[MB1_Z].bSelected = TRUE;
		stTimeObj[MB1_INDEX].bSelected = TRUE;
		stTimeObj[MB1_REWINDER1].bSelected = TRUE;
		stTimeObj[MB1_REWINDER2].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[MB1_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[MB1_Z].bSelected = FALSE;
		stTimeObj[MB1_INDEX].bSelected = FALSE;
		stTimeObj[MB1_REWINDER1].bSelected = FALSE;
		stTimeObj[MB1_REWINDER2].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[MB1_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetMB2Selected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[MB2_Z].bSelected = TRUE;
		stTimeObj[MB2_INDEX].bSelected = TRUE;
		stTimeObj[MB2_REWINDER1].bSelected = TRUE;
		stTimeObj[MB2_REWINDER2].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[MB2_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[MB2_Z].bSelected = FALSE;
		stTimeObj[MB2_INDEX].bSelected = FALSE;
		stTimeObj[MB2_REWINDER1].bSelected = FALSE;
		stTimeObj[MB2_REWINDER2].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[MB2_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetTrayHolderSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[FPC_TL_Z1].bSelected = TRUE;
		stTimeObj[FPC_TL_X1].bSelected = TRUE;
		stTimeObj[FPC_TL_X2].bSelected = TRUE;
		stTimeObj[FPC_TL_Z2].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[INTRAY_OPEN_SOL].bSelected = TRUE;
			stTimeObj[INTRAY_SPLIT_SOL].bSelected = TRUE;
			stTimeObj[TRAY1_DOWN_SOL].bSelected = TRUE;
			stTimeObj[TRAY2_DOWN_SOL].bSelected = TRUE;
			stTimeObj[OUTTRAY_OPEN_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[FPC_TL_Z1].bSelected = FALSE;
		stTimeObj[FPC_TL_X1].bSelected = FALSE;
		stTimeObj[FPC_TL_X2].bSelected = FALSE;
		stTimeObj[FPC_TL_Z2].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[INTRAY_OPEN_SOL].bSelected = FALSE;
			stTimeObj[INTRAY_SPLIT_SOL].bSelected = FALSE;
			stTimeObj[TRAY1_DOWN_SOL].bSelected = FALSE;
			stTimeObj[TRAY2_DOWN_SOL].bSelected = FALSE;
			stTimeObj[OUTTRAY_OPEN_SOL].bSelected = FALSE;
		}
	}
	return 0;
}

LONG CTimeChart::HMI_SetSHSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[SH_X].bSelected = TRUE;
		stTimeObj[SH_Y].bSelected = TRUE;
		stTimeObj[FPC_PA_X].bSelected = TRUE;
		stTimeObj[FPC_PA_Y].bSelected = TRUE;
		stTimeObj[FPC_PA_T1].bSelected = TRUE;
		stTimeObj[FPC_PA_T2].bSelected = TRUE;
		stTimeObj[FPC_PA_Z1].bSelected = TRUE;
		stTimeObj[FPC_PA_Z2].bSelected = TRUE;
		if (HMI_bSolSelected)
		{
			stTimeObj[FPC_RA_X_SOL].bSelected = TRUE;
			stTimeObj[FPC_RA_Z_SOL].bSelected = TRUE;
			stTimeObj[FPC_PRESS1_SOL].bSelected = TRUE;
			stTimeObj[FPC_PRESS2_SOL].bSelected = TRUE;
			stTimeObj[SH_REJECT_BIN_SOL].bSelected = TRUE;
			stTimeObj[SH_SOL].bSelected = TRUE;
		}
	}
	else
	{
		stTimeObj[SH_X].bSelected = FALSE;
		stTimeObj[SH_Y].bSelected = FALSE;
		stTimeObj[FPC_PA_X].bSelected = FALSE;
		stTimeObj[FPC_PA_Y].bSelected = FALSE;
		stTimeObj[FPC_PA_T1].bSelected = FALSE;
		stTimeObj[FPC_PA_T2].bSelected = FALSE;
		stTimeObj[FPC_PA_Z1].bSelected = FALSE;
		stTimeObj[FPC_PA_Z2].bSelected = FALSE;
		if (HMI_bSolSelected)
		{
			stTimeObj[FPC_RA_X_SOL].bSelected = FALSE;
			stTimeObj[FPC_RA_Z_SOL].bSelected = FALSE;
			stTimeObj[FPC_PRESS1_SOL].bSelected = FALSE;
			stTimeObj[FPC_PRESS2_SOL].bSelected = FALSE;
			stTimeObj[SH_REJECT_BIN_SOL].bSelected = FALSE;
			stTimeObj[SH_SOL].bSelected = FALSE;
		}
	}
	return 0;
}


LONG CTimeChart::HMI_SetOutConvSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		stTimeObj[OUT_CONV_SOL].bSelected = TRUE;
	}
	else
	{
		stTimeObj[OUT_CONV_SOL].bSelected = FALSE;
	}
	return 0;
}

LONG CTimeChart::HMI_SetSOLSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		HMI_bSolSelected = TRUE;

		// InConv
		if (stTimeObj[IN_CONV].bSelected)
		{
			stTimeObj[IN_CONV_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[IN_CONV_SOL].bSelected = FALSE;
		}

		// INSH
		if (stTimeObj[INSH_X].bSelected)
		{
			stTimeObj[INSH_EXTEND_SOL].bSelected = TRUE;
			stTimeObj[IN_RA_GO_FRONT_SOL].bSelected = TRUE;
			stTimeObj[INSH_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[INSH_EXTEND_SOL].bSelected = FALSE;
			stTimeObj[IN_RA_GO_FRONT_SOL].bSelected = FALSE;
			stTimeObj[INSH_SOL].bSelected = FALSE;
		}

		// INWH
		if (stTimeObj[INWH_Y].bSelected)
		{
			stTimeObj[INWH_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[INWH_SOL].bSelected = FALSE;
		}

		// ACF1 WH
		if (stTimeObj[ACF1WH_Y].bSelected)
		{
			stTimeObj[ACF1WH_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[ACF1WH_SOL].bSelected = FALSE;
		}

		// ACF2 WH
		if (stTimeObj[ACF2WH_Y].bSelected)
		{
			stTimeObj[ACF2WH_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[ACF2WH_SOL].bSelected = FALSE;
		}

		// PBWH
		if (stTimeObj[PBWH_X1].bSelected)
		{
			stTimeObj[PBWH1_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[PBWH1_SOL].bSelected = FALSE;
		}
		
		// MB1WH
		if (stTimeObj[MB1WH_Y].bSelected)
		{
			stTimeObj[MB1WH_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[MB1WH_SOL].bSelected = FALSE;
		}

		// MB2WH
		if (stTimeObj[MB2WH_Y].bSelected)
		{
			stTimeObj[MB2WH_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[MB2WH_SOL].bSelected = FALSE;
		}

		// TA1
		if (stTimeObj[TA1_X].bSelected)
		{
			stTimeObj[TA1_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[TA1_SOL].bSelected = FALSE;
		}

		// TA2
		if (stTimeObj[TA2_X].bSelected)
		{
			stTimeObj[TA2_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[TA2_SOL].bSelected = FALSE;
		}

		// TA3
		if (stTimeObj[TA3_X].bSelected)
		{
			stTimeObj[TA3_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[TA3_SOL].bSelected = FALSE;
		}

		// TA4
		if (stTimeObj[TA4_X].bSelected)
		{
			stTimeObj[TA4_UPDNLK_Y].bSelected = TRUE;
			stTimeObj[TA4_T1].bSelected = TRUE;
			stTimeObj[TA4_T2].bSelected = TRUE;
			stTimeObj[TA4_EXTEND_SOL].bSelected = TRUE;
			stTimeObj[TA4_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[TA4_UPDNLK_Y].bSelected = FALSE;
			stTimeObj[TA4_T1].bSelected = FALSE;
			stTimeObj[TA4_T2].bSelected = FALSE;
			stTimeObj[TA4_EXTEND_SOL].bSelected = FALSE;
			stTimeObj[TA4_SOL].bSelected = FALSE;
		}

		// TA5
		if (stTimeObj[TA5_X].bSelected)
		{
			stTimeObj[TA5_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[TA5_SOL].bSelected = FALSE;
		}
		
		// ACF1
		if (stTimeObj[ACF1_INDEX].bSelected)
		{
			stTimeObj[ACF1_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[ACF1_SOL].bSelected = FALSE;
		}

		// ACF1
		if (stTimeObj[ACF2_INDEX].bSelected)
		{
			stTimeObj[ACF2_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[ACF2_SOL].bSelected = FALSE;
		}

		// PB1
		if (stTimeObj[PB1_Z].bSelected)
		{
			stTimeObj[PB1_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[PB1_SOL].bSelected = FALSE;
		}

		// PB2
		if (stTimeObj[PB2_Z].bSelected)
		{
			stTimeObj[PB2_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[PB2_SOL].bSelected = FALSE;
		}

		// MB1
		if (stTimeObj[MB1_Z].bSelected)
		{
			stTimeObj[MB1_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[MB1_SOL].bSelected = FALSE;
		}

		// MB2
		if (stTimeObj[MB2_Z].bSelected)
		{
			stTimeObj[MB2_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[MB2_SOL].bSelected = FALSE;
		}

		// TH
		if (stTimeObj[FPC_TL_X1].bSelected)
		{
			stTimeObj[INTRAY_OPEN_SOL].bSelected = TRUE;
			stTimeObj[INTRAY_SPLIT_SOL].bSelected = TRUE;
			stTimeObj[TRAY1_DOWN_SOL].bSelected = TRUE;
			stTimeObj[TRAY2_DOWN_SOL].bSelected = TRUE;
			stTimeObj[OUTTRAY_OPEN_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[INTRAY_OPEN_SOL].bSelected = FALSE;
			stTimeObj[INTRAY_SPLIT_SOL].bSelected = FALSE;
			stTimeObj[TRAY1_DOWN_SOL].bSelected = FALSE;
			stTimeObj[TRAY2_DOWN_SOL].bSelected = FALSE;
			stTimeObj[OUTTRAY_OPEN_SOL].bSelected = FALSE;
		}

		// SH
		if (stTimeObj[SH_X].bSelected)
		{
			stTimeObj[FPC_RA_X_SOL].bSelected = TRUE;
			stTimeObj[FPC_RA_Z_SOL].bSelected = TRUE;
			stTimeObj[FPC_PRESS1_SOL].bSelected = TRUE;
			stTimeObj[FPC_PRESS2_SOL].bSelected = TRUE;
			stTimeObj[SH_REJECT_BIN_SOL].bSelected = TRUE;
			stTimeObj[SH_SOL].bSelected = TRUE;
		}
		else
		{
			stTimeObj[FPC_RA_X_SOL].bSelected = FALSE;
			stTimeObj[FPC_RA_Z_SOL].bSelected = FALSE;
			stTimeObj[FPC_PRESS1_SOL].bSelected = FALSE;
			stTimeObj[FPC_PRESS2_SOL].bSelected = FALSE;
			stTimeObj[SH_REJECT_BIN_SOL].bSelected = FALSE;
			stTimeObj[SH_SOL].bSelected = FALSE;
		}
	}
	else
	{
		HMI_bSolSelected = FALSE;

		stTimeObj[IN_CONV_SOL].bSelected = FALSE;
		stTimeObj[INWH_SOL].bSelected = FALSE;
		stTimeObj[ACF1WH_SOL].bSelected = FALSE;
		stTimeObj[ACF2WH_SOL].bSelected = FALSE;
		stTimeObj[PBWH1_SOL].bSelected = FALSE;
		stTimeObj[MB1WH_SOL].bSelected = FALSE;
		stTimeObj[MB2WH_SOL].bSelected = FALSE;
		stTimeObj[TA1_SOL].bSelected = FALSE;
		stTimeObj[TA2_SOL].bSelected = FALSE;
		stTimeObj[TA3_SOL].bSelected = FALSE;
		stTimeObj[TA4_EXTEND_SOL].bSelected = FALSE;
		stTimeObj[TA4_SOL].bSelected = FALSE;
		stTimeObj[TA5_SOL].bSelected = FALSE;
		stTimeObj[ACF1_SOL].bSelected = FALSE;
		stTimeObj[ACF2_SOL].bSelected = FALSE;
		stTimeObj[PB1_SOL].bSelected = FALSE;
		stTimeObj[PB2_SOL].bSelected = FALSE;
		stTimeObj[MB1_SOL].bSelected = FALSE;
		stTimeObj[MB2_SOL].bSelected = FALSE;
		stTimeObj[INTRAY_OPEN_SOL].bSelected = FALSE;
		stTimeObj[INTRAY_SPLIT_SOL].bSelected = FALSE;
		stTimeObj[TRAY1_DOWN_SOL].bSelected = FALSE;
		stTimeObj[TRAY2_DOWN_SOL].bSelected = FALSE;
		stTimeObj[OUTTRAY_OPEN_SOL].bSelected = FALSE;
		stTimeObj[SH_SOL].bSelected = FALSE;
	}

	return 0;
}

LONG CTimeChart::HMI_SelectAll(IPC_CServiceMessage &svMsg)
{
	INT i;
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);
	LogAction("%s: %s", __FUNCTION__, bMode ? "ON" : "OFF"); //20130510

	if (bMode)
	{
		for (i = 0; i < NUM_OF_TC_OBJECT; i++)
		{
			stTimeObj[i].bSelected = TRUE;
		}
	}
	else
	{
		for (i = 0; i < NUM_OF_TC_OBJECT; i++)
		{
			stTimeObj[i].bSelected = FALSE;
		}
	}

	if (MAX_NUM_OF_MAINBOND < 3)	// 20140828 Yip: Disable Time Chart For Main Bond 3 If Not Exist
	{
		for (i = MB5WH_Y; i <= MB3_SOL; i++)
		{
			stTimeObj[i].bSelected	= FALSE;
		}
	}

	return 0;
}

LONG CTimeChart::IPC_AddCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	INT nObjectID;
	svMsg.GetMsg(sizeof(INT), &nObjectID);

	TC_AddCmd(nObjectID);

	return 0;
}

LONG CTimeChart::IPC_DelCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	INT nObjectID;
	svMsg.GetMsg(sizeof(INT), &nObjectID);

	TC_DelCmd(nObjectID);

	return 0;
}

LONG CTimeChart::IPC_ResetTimer(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	TC_ResetTimer();

	return 0;
}
