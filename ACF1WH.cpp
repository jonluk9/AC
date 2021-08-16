/////////////////////////////////////////////////////////////////
//	ACF1WH.cpp : interface of the CACF1WH class
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

#include "ACF1WH.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CACF1WH, CACFWH)

CACF1WH::CACF1WH()
{
	INT i = 0;
	m_lStnErrorCode		= IDS_ACF1WH_NOT_SELECTED_ERR;
	m_szPRTaskStnName	= GetStation().Trim('_');
	
	m_lAddJobPR_GRAB	= ACF1WH_PR_GRAB;
	m_lAddJobPR_PROCESS = ACF1WH_PR_PROCESS;
	
	m_lPRStnErrorCode	= IDS_ACF1WH_PR_MOD_NOT_SELECTED;
	m_lPRModSelectMode	= GLASS_MOD;

	// Motor Related
	m_stMotorY.InitMotorInfo("ACF1WH_Y", &m_bModSelected);
	m_stMotorT.InitMotorInfo("ACF1WH_T", &m_bModSelected);
	//m_stMotorZ[WHZ_1].InitMotorInfo("ACF1WH_Z", &m_bModSelected);

#ifdef SRCH_EMBEDDED_SENSOR //20170810
	m_stMotorT.SetMtrSnrPortName(CString("ACF1WH_T_MTRSNR_PORT"), TRUE, TRUE); //channel has embeded limit snr and indexer inside the encoder plug. An external limit sensor as home sensor.
#endif

	m_myMotorList.Add((CMotorInfo*)&m_stMotorY);
	m_myMotorList.Add((CMotorInfo*)&m_stMotorT);
	//m_myMotorList.Add((CMotorInfo*)&m_stMotorZ[WHZ_1]);


	// Si Related
	m_stYHomeSnr	= CSiInfo("ACF1WHYHomeSnr",					"IOMODULE_IN_PORT_2", BIT_0, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stYHomeSnr);
	m_stTHomeSnr	= CSiInfo("ACF1WHTHomeSnr",					"IOMODULE_IN_PORT_2", BIT_1, ACTIVE_HIGH);
	m_mySiList.Add((CSiInfo*)&m_stTHomeSnr);
	m_stAnvil1VacSnr	= CSiInfo("ACF1WHAnvllLVacSnr",		"IOMODULE_IN_PORT_2", BIT_2, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stAnvil1VacSnr);
	m_stAnvil2VacSnr	= CSiInfo("ACF1WHAnvllRVacSnr",		"IOMODULE_IN_PORT_2", BIT_3, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stAnvil2VacSnr);
	m_stGlass1VacSnr	= CSiInfo("ACF1WHVacSnr",				"IOMODULE_IN_PORT_2", BIT_4, ACTIVE_LOW);
	m_mySiList.Add((CSiInfo*)&m_stGlass1VacSnr);

	// So Related

	m_stAnvilWBSol		= CSoInfo("ACF1WHAnvllLWBSol",		"IOMODULE_OUT_PORT_1", BIT_0, ACF1WH_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stAnvilWBSol);
	m_stAnvil1VacSol		= CSoInfo("ACF1WHAnvllLVacSol",		"IOMODULE_OUT_PORT_1", BIT_1, ACF1WH_SOL, ACTIVE_LOW);
	m_mySoList.Add((CSoInfo*)&m_stAnvil1VacSol);
	m_stAnvil2WeakBlowSol		= CSoInfo("ACF1WHAnvllRWBSol",		"IOMODULE_OUT_PORT_1", BIT_2, ACF1WH_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stAnvil2WeakBlowSol);
	m_stAnvil2VacSol		= CSoInfo("ACF1WHAnvllRVacSol",		"IOMODULE_OUT_PORT_1", BIT_3, ACF1WH_SOL, ACTIVE_LOW);
	m_mySoList.Add((CSoInfo*)&m_stAnvil2VacSol);
	m_stWeakBlowSol				= CSoInfo("ACF1WHAnvllTopWBSol",	"IOMODULE_OUT_PORT_1", BIT_4, ACF1WH_SOL, ACTIVE_HIGH);
	m_mySoList.Add((CSoInfo*)&m_stWeakBlowSol);
	m_stGlass1VacSol			= CSoInfo("ACF1WHVacSol",			"IOMODULE_OUT_PORT_1", BIT_5, ACF1WH_SOL, ACTIVE_LOW);
	m_mySoList.Add((CSoInfo*)&m_stGlass1VacSol);

	m_lCurSlave = ACFWH_1;

	for (i = ACF1; i < MAX_NUM_OF_ACF; i++)
	{
	m_stACF[i].bAlignPtSet		= FALSE;
	m_stACF[i].bBonded			= FALSE;
	m_stACF[i].bEnable			= TRUE;
	m_stACF[i].bValid			= FALSE;
	m_stACF[i].dmtrAlignPt1.x	= 0.0;
	m_stACF[i].dmtrAlignPt1.y	= 0.0;
	m_stACF[i].dmtrAlignPt2.x	= 0.0;
	m_stACF[i].dmtrAlignPt2.y	= 0.0;
	}

	m_dAlignAngle				= 0.0;
	m_mtrAlignCentre.x			= 0;
	m_mtrAlignCentre.y			= 0;
	m_mtrCurrAlignDelta.x		= 0;
	m_mtrCurrAlignDelta.y		= 0;
	m_dGlassRefAng				= 0.0;
	m_dAlignRefAngle			= 0.0;
	m_dmtrAlignRef.x			= 0.0;
	m_dmtrAlignRef.y			= 0.0;

}

CACF1WH::~CACF1WH()
{
}

BOOL CACF1WH::InitInstance()
{
	CACFWH::InitInstance();

	return TRUE;
}

INT CACF1WH::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CACFWH::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CACF1WH::Operation()
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
VOID CACF1WH::RegisterVariables()
{
	try
	{
		// -------------------------------------------------------
		// AC9000Stn Variable and Function
		// -------------------------------------------------------
		RegVariable(GetStation() + _T("bCheckMotor"), &m_bCheckMotor);
		RegVariable(GetStation() + _T("bCheckIO"), &m_bCheckIO);

		RegVariable(GetStation() + _T("bModSelected"), &m_bModSelected);
		RegVariable(GetStation() + _T("bShowModSelected"), &m_bShowModSelected);
		RegVariable(GetStation() + _T("bModError"), &m_bModError);
		RegVariable(GetStation() + _T("bPBEnable"), &m_bPBEnable);

		RegVariable(GetStation() + _T("bGlass1Exist"), &m_bGlass1Exist);

		RegVariable(GetStation() + _T("bDebugSeq"), &HMI_bDebugSeq);
		RegVariable(GetStation() + _T("bLogMotion"), &HMI_bLogMotion);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetModSelected"), HMI_SetModSelected);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeAll"), HMI_HomeAll);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDebugSeq"), HMI_SetDebugSeq);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetLogMotion"), HMI_SetLogMotion);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPBEnable"), HMI_PBEnable);

		// -------------------------------------------------------
		// WorkHolder Variable and Function
		// -------------------------------------------------------		
		RegVariable(_T("HMI_bACF1WHYOn"), &m_stMotorY.bHMI_On);
		RegVariable(_T("HMI_bACF1WHTOn"), &m_stMotorT.bHMI_On);
		//RegVariable(_T("HMI_bACF1WHZOn"), &m_stMotorZ[WHZ_1].bHMI_On);

		RegVariable(_T("HMI_lACF1WHYEncoder"), &m_stMotorY.lHMI_CurrPosn);
		RegVariable(_T("HMI_lACF1WHTEncoder"), &m_stMotorT.lHMI_CurrPosn);
		//RegVariable(_T("HMI_lACF1WHZEncoder"), &m_stMotorZ[WHZ_1].lHMI_CurrPosn);

		//RegVariable(GetStation() + _T("dXDistPerCount"), &m_stMotorX.stAttrib.dDistPerCount);
		//RegVariable(GetStation() + _T("dZDistPerCount"), &m_stMotorZ[WHZ_1].stAttrib.dDistPerCount);
		RegVariable(GetStation() + _T("dTDistPerCount"), &m_stMotorT.stAttrib.dDistPerCount);
		RegVariable(GetStation() + _T("dYDistPerCount"), &m_stMotorY.stAttrib.dDistPerCount);

		//Snr
		RegVariable(_T("HMI_bACF1WHZHomeSnr"), &m_stZ1HomeSnr.bHMI_Status);
		RegVariable(_T("HMI_bACF1WHTHomeSnr"), &m_stTHomeSnr.bHMI_Status);

		RegVariable(_T("HMI_bACF1WHG1VacSnr"), &m_stGlass1VacSnr.bHMI_Status);
		RegVariable(_T("HMI_bACF1WHAnvilLVacSnr"), &m_stAnvil1VacSnr.bHMI_Status);
		RegVariable(_T("HMI_bACF1WHAnvilRVacSnr"), &m_stAnvil2VacSnr.bHMI_Status);

		RegVariable(_T("HMI_bACF1WHWeakBlowSol"), &m_stWeakBlowSol.bHMI_Status);
		RegVariable(_T("HMI_bACF1WHG1VacSol"), &m_stGlass1VacSol.bHMI_Status);
		RegVariable(_T("HMI_bACF1WHAnvilWBSol"), &m_stAnvilWBSol.bHMI_Status);
		RegVariable(_T("HMI_bACF1WHAnvilVacSol"), &m_stAnvil1VacSol.bHMI_Status);
		RegVariable(_T("HMI_bACF1WHAnvil2WBSol"), &m_stAnvil2WeakBlowSol.bHMI_Status);
		RegVariable(_T("HMI_bACF1WHAnvil2VacSol"), &m_stAnvil2VacSol.bHMI_Status);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnY"), HMI_PowerOnY);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("PowerOnT"), HMI_PowerOnT);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("CommY"), HMI_CommY);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("CommT"), HMI_CommT);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeY"), HMI_HomeY);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("HomeT"), HMI_HomeT);
	
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleVacSol"), HMI_ToggleG1VacSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleAnvil2VacSol"), HMI_ToggleAnvil2VacSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleAnvilVacSol"), HMI_ToggleAnvil1VacSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleWBSol"), HMI_ToggleWeakBlowSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleAnvilWB2Sol"), HMI_ToggleAnvilWeakBlow2Sol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleAnvilWBSol"), HMI_ToggleAnvilWeakBlowSol);

		// Setup Page
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDiagnSteps"), HMI_SetDiagnSteps);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexYPos"), HMI_IndexYPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexYNeg"), HMI_IndexYNeg);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexTPos"), HMI_IndexTPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IndexTNeg"), HMI_IndexTNeg);

		// -------------------------------------------------------
		// CPRTaskStn Variable and Function
		// -------------------------------------------------------
		RegVariable(GetStation() + _T("bPRSelected"), &m_bPRSelected);	

		RegVariable(GetStation() + _T("lPRDelay"), &m_lPRDelay);
		RegVariable(GetStation() + _T("lPBPRDelay"), &m_lPRDelay2);
		RegVariable(GetStation() + _T("lCalibPRDelay"), &m_lCalibPRDelay);
		RegVariable(GetStation() + _T("lPRRetryLimit"), &m_lPRRetryLimit);
		RegVariable(GetStation() + _T("lRejectGlassLimit"), &m_lRejectGlassLimit);
		RegVariable(GetStation() + _T("dAlignDist"), &HMI_dAlignDist);	// 20160513 Add Align Distance

		RegVariable(GetStation() + _T("ulCurPRU"), &HMI_ulCurPRU);
		RegVariable(GetStation() + _T("ulPRID"), &HMI_ulPRID);
		RegVariable(GetStation() + _T("bShowPRSearch"), &HMI_bShowPRSearch);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SrchPR"), HMI_SrchPR);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ModifyPRCriteria"), HMI_ModifyPRCriteria);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRRetryLimit"), HMI_SetPRRetryLimit);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetRejectGlassLimit"), HMI_SetRejectGlassLimit);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRDelay"), HMI_SetPRDelay);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetCalibPRDelay"), HMI_SetCalibPRDelay);

		RegVariable(GetStation() + _T("bPR1Loaded"),	&pruACF1.bLoaded);
		RegVariable(GetStation() + _T("bPR2Loaded"),	&pruACF2.bLoaded);
		RegVariable(GetStation() + _T("bAlignPtSet"),	&m_stACF[ACF1].bAlignPtSet);
		RegVariable(GetStation() + _T("bValid"),	&m_stACF[ACF1].bValid);

		RegVariable(GetStation() + _T("dAlignDistTol"), &m_dAlignDistTol);
		RegVariable(GetStation() + _T("dAlignAngleTol"), &m_dAlignAngleTol);
			
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetAlignDistTol"), HMI_SetAlignDistTol);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetAlignAngleTol"), HMI_SetAlignAngleTol);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRSelected"), HMI_SetPRSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("UpdateSetupStatus"), HMI_UpdateSetupStatus);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SelectPRU"), HMI_SelectPRU);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPR1Posn"), HMI_SetPR1Posn);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPR2Posn"), HMI_SetPR2Posn);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRLevel"), HMI_SetPRLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SearchAp1"), HMI_SearchAp1);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SearchAp2"), HMI_SearchAp2);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SearchCentre"), HMI_SearchCentre);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("DisplayVideo"), HMI_DisplayVideo);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetCurrentSetupGlass"), HMI_SetCurrentSetupGlass);

		RegVariable(GetStation() + _T("ulCurPBPRU"), &HMI_ulCurPBPRU);
		RegVariable(GetStation() + _T("bPBPR1Loaded"),	&pruACFPB1[ACF1][OBJECT].bLoaded);
		RegVariable(GetStation() + _T("bPBPR2Loaded"),	&pruACFPB2[ACF1][OBJECT].bLoaded);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SelectPBPRU"), HMI_SelectPBPRU);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPBPR1Posn"), HMI_SetPBPR1Posn);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPBPR2Posn"), HMI_SetPBPR2Posn);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SrchPBPR"), HMI_SrchPBPR);

		RegVariable(GetStation() + _T("lSegThreshold"), &HMI_lSegThreshold);
		RegVariable(GetStation() + _T("lIsAlign"), &m_lIsAlign);
		RegVariable(GetStation() + _T("lAlignAlg"), &m_lAlignAlg);

		RegVariable(GetStation() + _T("bShowSegThreshold"), &HMI_bShowSegThreshold);
		RegVariable(GetStation() + _T("bShowPBMethod"), &HMI_bShowPBMethod);
		RegVariable(GetStation() + _T("bShowBasicAlg"), &HMI_bShowBasicAlg);
		RegVariable(GetStation() + _T("bShowAdvanceAlg"), &HMI_bShowAdvanceAlg);

		RegVariable(GetStation() + _T("dPRMatchScore"), &HMI_dPRMatchScore);
		RegVariable(GetStation() + _T("lDefectThreshold"), &HMI_lDefectThreshold);
		RegVariable(GetStation() + _T("ulPRPostBdAttrib"), &HMI_ulPRPostBdAttrib);
		RegVariable(GetStation() + _T("dPostBdRejectTol"), &HMI_dPostBdRejectTol);	

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetSegThreshold"), HMI_SetSegThreshold);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IncrSegThresholdLevel10"), HMI_IncrSegThresholdLevel10);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("IncrSegThresholdLevel"), HMI_IncrSegThresholdLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("DecrSegThresholdLevel10"), HMI_DecrSegThresholdLevel10);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("DecrSegThresholdLevel"), HMI_DecrSegThresholdLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetIsAlign"), HMI_SetIsAlign);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetAlignAlg"), HMI_SetAlignAlg);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPRMatchScore"), HMI_SetPRMatchScore);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDefectThreshold"), HMI_SetDefectThreshold);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetDefectAttribute"), HMI_SetDefectAttribute);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetPostBdRejectTol"), HMI_SetPostBdRejectTol);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("DisplaySrchArea"), HMI_DisplaySrchArea);
		// -------------------------------------------------------
		// varibales
		// -------------------------------------------------------
		RegVariable(GetStation() + _T("dProcessTime"), &m_dProcessTime);

		RegVariable(GetStation() + _T("lBondOffsetY"), &HMI_lBondOffsetY);

		RegVariable(GetStation() + _T("lLoadXOffset"), &m_lLoadXOffset);
		RegVariable(GetStation() + _T("lLoadYOffset"), &m_lLoadYOffset);
		RegVariable(GetStation() + _T("lLoadTOffset"), &m_lLoadTOffset);
		RegVariable(GetStation() + _T("dLoadTOffset"), &m_dLoadTOffset);
		RegVariable(GetStation() + _T("dLoadAdjXOffset"), &m_dLoadAdjXOffset); //20121005

		RegVariable(GetStation() + _T("lUnloadYOffset"), &m_lUnloadYOffset);
		//RegVariable(GetStation() + _T("dUnloadTOffset"), &m_dUnloadTOffset);
		RegVariable(GetStation() + _T("ulCurACF"), &m_lCurACF);

#ifdef ACF_POSN_CALIB //20120829
		RegVariable(GetStation() + _T("dBondOffsetAdjY"), &HMI_dBondOffsetAdjY);
		RegVariable(GetStation() + _T("bYZControl"), &HMI_bYZControl);
#endif
	
		// -------------------------------------------------------
		// commands
		// -------------------------------------------------------
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SelectACFNum"), HMI_SelectACFNum);

#ifdef ACF_POSN_CALIB //20120829
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetBondOffsetAdjY"), HMI_SetBondOffsetAdjY);
#endif
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetBondOffsetY"), HMI_SetBondOffsetY);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetAdjXLoadOffset"), HMI_SetAdjXLoadOffset); //20121005
		
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ResetLoadOffset"), HMI_ResetLoadOffset); //20140507
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetXLoadOffset"), HMI_SetXLoadOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetYLoadOffset"), HMI_SetYLoadOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetTLoadOffset"), HMI_SetTLoadOffset);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetYUnloadOffset"), HMI_SetYUnloadOffset);
		//m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SetTUnloadOffset"), HMI_SetTUnloadOffset);

		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ACFOffsetTest"), HMI_ACFOffsetTest);

		RegVariable(GetStation() + _T("dMeasuredDistance"), &m_dMeasuredDistance);
		RegVariable(GetStation() + _T("dMeasuredAngle"), &m_dMeasuredAngle);
		RegVariable(GetStation() + _T("bEnableMeasure"), &m_bEnableMeasure);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("MeasureSetZero"), HMI_MeasureSetZero);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("MeasureEnable"), HMI_MeasureEnable);

		RegVariable(GetStation() + _T("bNotBondACF"), &m_bNotBondACF);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("ToggleNotBondACF"), HMI_ToggleNotBondACF);

		RegVariable(GetStation() + _T("lLoopCount"), &HMI_lLoopCount);
		m_comServer.IPC_REG_SERVICE_COMMAND(GetStation() + _T("SearchAndLogPR"), HMI_SearchAndLogPR);
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
}
