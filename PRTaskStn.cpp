/////////////////////////////////////////////////////////////////
//	PRTaskStn.cpp : interface of the CWorkHolder class
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
#include "AC9000.h"

#define PRTASKSTN_EXTERN
#include "PRTaskStn.h"
#include <typeinfo.h>

#include "FileOperations.h"
#include "ExcelMotorInfo_Util.h"
#include "BasicExcel.h"
#include "WinEagle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI PRGrabThread(LPVOID lpParam); //20170328
DWORD WINAPI PRProcThread(LPVOID lpParam);
DWORD WINAPI PRSrchThread(LPVOID lpParam);

CPRTaskStn::CPRTaskStn()
{
	INT i = 0;

	m_pAppMod						= NULL; //20150106
	m_szPRTaskStnName				= "";
	m_lPRModSelectMode				= GLASS_MOD;

	m_bPRSelected					= TRUE;
	m_bPRError						= FALSE;

	m_bPRCycle						= FALSE;
	m_ulPRCycle						= 0;

	m_lAddJobPR_GRAB				= 0;		
	m_lAddJobPR_PROCESS				= 0;

	m_lPRRetryCounter				= 0;
	m_lPRRetryLimit					= 0;
	m_lRejectGlassCounter			= 0;
	m_lRejectGlassLimit				= 0;
	m_lPRDelay						= 0;
	m_lPRDelay2						= 0; //20150323
	m_lCalibPRDelay					= 0;
	m_lPRErrorLimit					= 10;

	m_dmtrCurrPR1Posn.x				= 0.0;
	m_dmtrCurrPR1Posn.y				= 0.0;
	m_dmtrCurrPR2Posn.x				= 0.0;
	m_dmtrCurrPR2Posn.y				= 0.0;
	m_dmtrCurrAlign1Offset.x		= 0.0;
	m_dmtrCurrAlign1Offset.y		= 0.0;
	m_dmtrCurrAlign2Offset.x		= 0.0;
	m_dmtrCurrAlign2Offset.y		= 0.0;
	m_dmtrCurrAlignCentre.x			= 0.0;
	m_dmtrCurrAlignCentre.y			= 0.0;
	m_dCurrAlignAngle				= 0.0;
	m_dCurrPreciseAlginAngle		= 0.0;
	m_dCurrAlignDist				= 0.0;

	m_nPRInProgress					= ST_PR_IDLE;

	for (INT ii = 0; ii < MAX_NUM_OF_GLASS; ii++) //20170328
	{
		for (INT jj = 0; jj < MAX_NUM_OF_LSI; jj++)
		{
			for (INT kk = 0; kk < MAX_NUM_OF_PR; kk++)
			{
				m_hPRGrabThread[ii][jj][kk] = NULL;
				m_hPRProcThread[ii][jj][kk] = NULL;
				m_hPRSrchThread[ii][jj][kk] = NULL;

				m_bPRGrabThreadRunning[ii][jj][kk] = FALSE;
				m_bPRProcThreadRunning[ii][jj][kk] = FALSE;
				m_bPRSrchThreadRunning[ii][jj][kk] = FALSE;

				m_bPRGrabThreadResult[ii][jj][kk] = FALSE;
				m_bPRProcThreadResult[ii][jj][kk] = FALSE;
				m_bPRSrchThreadResult[ii][jj][kk] = FALSE;
			}
		}
	}
	m_lThreadGlassx = GLASS1;
	m_lThreadLSIx = LSI1;
	m_lThreadPRx = PR1;
	m_pThreadPRU = NULL;

	//PostBond Only
	for (INT i = 0; i < PR_NO_OF_CORNERS; i++)
	{
		m_PRCurrPBPR1Corner[i].x			= 0;
		m_PRCurrPBPR1Corner[i].y			= 0;
		m_PRCurrPBPR2Corner[i].x			= 0;
		m_PRCurrPBPR2Corner[i].y			= 0;
	}

	//Auto Calib parameters
	for (INT i = 0; i < NO_OF_CAL_PT; i++)
	{
		m_stPRCalibPts[i].x					= 0;
		m_stPRCalibPts[i].y					= 0;
		m_stMtrCalibPts[i].x				= 0;
		m_stMtrCalibPts[i].y				= 0;
	}

	//// COR parameters
	for (INT j = 0; j < MAX_NUM_OF_TA4_COR; j++)
	{
		for (INT i = 0; i < COR_ARRAY_SIZE; i++)
		{
			m_stCORMtrCalibPts[j][i].x				= 0;
			m_stCORMtrCalibPts[j][i].y				= 0;
			m_dmtrTableCOR_Pos[j][i].x				= 0;
			m_dmtrTableCOR_Pos[j][i].y				= 0;
			m_dmtrTableCOR_Neg[j][i].x				= 0;
			m_dmtrTableCOR_Neg[j][i].y				= 0;
			m_dCORLUT[j][i]						= 0;
		}
	}
	
	for (INT j = 0; j < MAX_NUM_OF_TA4_COR; j++)
	{
		m_bCORCalibrated[j]					= FALSE;
	}
	m_dCalibCORRange					= 5.0;
	//m_dCalibCORRange					= 20.0; //20130318
	m_lCORCamNum						= 0;

	m_mtrThermalCalibRefPosn.x			= 0; //20140627 PBDL cal
	m_mtrThermalCalibRefPosn.y			= 0;
	m_mtrThermalCalibOffset.x			= 0; //20140707 thermal compensation
	m_mtrThermalCalibOffset.y			= 0;

	HMI_ulCurPRU						= 0;
	HMI_ulPRID							= 0;
	HMI_dPRMatchScore					= 85;
	HMI_dPRAngleRange					= 3.0;
	HMI_dPRPercentVar					= 5.0;
	HMI_bShowPRSearch					= FALSE;
	HMI_dPostBdRejectTol				= 5.0;
	HMI_ulPRPostBdAttrib				= PR_DEFECT_ATTRIBUTE_BRIGHT;
	HMI_lSegThreshold					= 0;
	HMI_lDefectThreshold				= 0;

	// Advance
	HMI_lACFAlignAlg					= 0;		
	HMI_lACFSegmentAlg					= 0;
	HMI_lACFIsAlign						= 0;
	HMI_lPBMethod						= 0;

	// Basic
	m_lIsAlign							= 0;
	m_lAlignAlg							= 0;

	HMI_bShowSegThreshold				= FALSE;
	HMI_bShowPBMethod					= FALSE;
	HMI_bShowBasicAlg					= FALSE;
	HMI_bShowAdvanceAlg					= FALSE;	
	HMI_bShowACFAlignAlg				= FALSE;

	m_motorDummy.stAttrib.szAxis = ""; //klocwork fix 20121211

}

CPRTaskStn::~CPRTaskStn()
{
}
/////////////////////////////////////////////////////////////////
// Virtual Move
/////////////////////////////////////////////////////////////////
// Fix me
CMotorInfo &CPRTaskStn::GetMotorX() // Suppose the child will be used
{
#if 1 //klocwork fix 20121211
	return m_motorDummy;
#else
	//Create A DUMMY CMotorInfo;
	CMotorInfo stDummyMotor;
	stDummyMotor.stAttrib.szAxis = "";
	return stDummyMotor;
#endif
}

CMotorInfo &CPRTaskStn::GetMotorY()
{
#if 1 //klocwork fix 20121211
	return m_motorDummy;
#else
	//Create A DUMMY CMotorInfo;
	CMotorInfo stDummyMotor;
	stDummyMotor.stAttrib.szAxis = "";
	return stDummyMotor;
#endif
}

CMotorInfo &CPRTaskStn::GetMotorT()
{
#if 1 //klocwork fix 20121211
	return m_motorDummy;
#else
	//Create A DUMMY CMotorInfo;
	CMotorInfo stDummyMotor;
	stDummyMotor.stAttrib.szAxis = "";
	return stDummyMotor;
#endif
}

//CMotorInfo &CPRTaskStn::GetMotorZ(WHZNum lWHZ)
//{
//#if 1 //klocwork fix 20121211
//	return m_motorDummy;
//#else
//	//Create A DUMMY CMotorInfo;
//	CMotorInfo stDummyMotor;
//	stDummyMotor.stAttrib.szAxis = "";
//	return stDummyMotor;
//#endif
//}

LONG CPRTaskStn::GetStandbyTPosn() //20160525
{
	return 0;
}

INT CPRTaskStn::SyncX()
{
	return GMP_SUCCESS;
}

INT CPRTaskStn::SyncY()
{

	return GMP_SUCCESS;
}

INT CPRTaskStn::SyncZ()
{

	return GMP_SUCCESS;
}

INT CPRTaskStn::SyncT()
{

	return GMP_SUCCESS;
}

INT CPRTaskStn::MoveZToPRCalibLevel(BOOL bWait)
{
	return GMP_SUCCESS;
}

INT CPRTaskStn::MoveXYToPRCalibPosn(BOOL bWait)
{
	return GMP_SUCCESS;
}

INT CPRTaskStn::MoveTToStandby(BOOL bWait)
{
	return GMP_SUCCESS;
}

INT CPRTaskStn::MoveLighting(BOOL bWait)  //20150218
{
	return GMP_SUCCESS;
}

INT CPRTaskStn::MoveXYToCurrPR1Posn(BOOL bWait)
{
	return GMP_SUCCESS;
}

INT CPRTaskStn::MoveZToPRLevel(BOOL bWait, PRMultiLevel emPRMultiLevel)
{
	return GMP_SUCCESS;
}

INT CPRTaskStn::MoveXYToCurrPR2Posn(BOOL bWait)
{
	return GMP_SUCCESS;
}

INT CPRTaskStn::MoveAbsoluteXY(LONG lCntX, LONG lCntY, BOOL bWait)
{
	return GMP_SUCCESS;
}

INT CPRTaskStn::MoveRelativeXY(LONG lCntX, LONG lCntY, BOOL bWait)
{
	return GMP_SUCCESS;
}

INT CPRTaskStn::MoveAbsoluteXY(LONG lCntX, LONG lCntY, BOOL bWait, BOOL bMoveToSafeX)
{
	return GMP_SUCCESS;
}

INT CPRTaskStn::MoveRelativeXY(LONG lCntX, LONG lCntY, BOOL bWait, BOOL bMoveToSafeX)
{
	return GMP_SUCCESS;
}

INT CPRTaskStn::MoveAbsoluteT(LONG lMtrCnt, BOOL bWait)
{
	return GetMotorT().MoveAbsolute(lMtrCnt, bWait);
}

INT CPRTaskStn::MoveRelativeT(LONG lMtrCnt, BOOL bWait)
{
	return GetMotorT().MoveRelative(lMtrCnt, bWait);
}

PRU &CPRTaskStn::SelectCurPRU(ULONG ulPRU)
{
	return pruDummy;
}

PRU &CPRTaskStn::SelectCurPBPRU(ULONG ulPRU)
{
	return pruDummy;
}

PRU &CPRTaskStn::SelectCurPBBackground(ULONG ulPRU)
{
	return pruDummy;
}

/////////////////////////////////////////////////////////////////
// END Virtual Move
/////////////////////////////////////////////////////////////////

VOID CPRTaskStn::SetPRSelected(BOOL bMode)
{
	if (!g_bIsVisionNTInited)
	{
		m_bPRSelected = FALSE;
		m_pAppMod->HMIMessageBox(MSG_OK, "MODULE NOT SELECTED", "PRS Not Initialized!");
	}
	else
	{
		m_bPRSelected = bMode;
	}
}

BOOL CPRTaskStn::IsPRSelected()
{
	return m_bPRSelected;
}

VOID CPRTaskStn::CheckPRModSelected(BOOL bMode)
{
	if (g_bIsVisionNTInited)
	{
		if (bMode)
		{
			if (!m_bPRSelected)
			{
				m_pAppMod->SetAlert(m_lPRStnErrorCode);
			}
		}
	}
}

LONG CPRTaskStn::GetPRModSelectMode()
{
	return m_lPRModSelectMode;
}

LONG CPRTaskStn::SleepWithCalibDelay()
{
	Sleep(m_lCalibPRDelay);

	return GMP_SUCCESS;
}
LONG CPRTaskStn::GetPRDelay()
{
	return m_lPRDelay;
}

LONG CPRTaskStn::GetPRDelay2()
{
	return m_lPRDelay2;
}

LONG CPRTaskStn::GetPRCalibDelay()
{
	return m_lCalibPRDelay;
}

/////////////////////////////////////////
// Learn Function
////////////////////////////////////////
BOOL CPRTaskStn::ManualLrnDie_New(PRU *pPRU)	// Vision UI Learn
{
	// learn die
	pPRU->bLoaded = FALSE;

	if (PRS_ManualLrnCmd(pPRU))
	{


		if (PRS_CreateCriteriaCmd(pPRU))
		{

			pPRU->bLoaded = TRUE;

			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Pattern Learnt");

			CString szMsg = _T("");
			szMsg.Format("Pattern Learn Succeed [ID: %d]", pPRU->stSrchDieCmd.auwRecordID[0]);
			m_pAppMod->WriteHMIMess(szMsg);

			return TRUE;
		}
		else
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Pattern Learn Failed!");
			m_pAppMod->WriteHMIMess("Pattern Learn Failed!");

			return FALSE;
		}
		
	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Pattern Learn Failed!");
		m_pAppMod->WriteHMIMess("Pattern Learn Failed!");

		return FALSE;
	}
}

VOID CPRTaskStn::ModifyPRCriteria(PRU *pPRU)
{
	if (!pPRU->bLoaded)
	{
		m_pAppMod->WriteHMIMess("Pattern Search Error: Please load pattern first.");
		return;
	}

	if (PRS_ModifyCriteriaCmd(pPRU))
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Criteria Modified");

	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Modify Criteria Failed!");
		m_pAppMod->WriteHMIMess("Modify Criteria Failed!");
	}
}

VOID CPRTaskStn::SrchPR_New(PRU *pPRU)
{
	if (!pPRU->bLoaded)
	{
		m_pAppMod->WriteHMIMess("Pattern Search Error: Please load pattern first.");
		return;
	}

	if (PRS_ManualSrchCmd(pPRU))
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Pattern Found");

		CString szMsg = _T("");
		m_pAppMod->WriteHMIMess("Pattern Search Succeed!");
		szMsg.Format("XY: (%.2f,%.2f), Ang: %.2f, Score: %.2f, Scale: x:%.2f, y: %.2f", 
					 pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, pPRU->fAng, pPRU->fScore, pPRU->rszScale.x, pPRU->rszScale.y);	
		m_pAppMod->WriteHMIMess(szMsg);

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);
	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Pattern Search Failed!");
		m_pAppMod->WriteHMIMess("Pattern Search Failed!");
	}
}

BOOL CPRTaskStn::PRModifyCenter(PRU *pPRU)
{
	// learn die
	pPRU->bLoaded = FALSE;

	if (PRS_LrnRGBCenter(pPRU))
	{
		pPRU->bLoaded = TRUE;

		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Pattern Center Modified");

		CString szMsg = _T("");
		szMsg.Format("Modify Pattern Center Succeed [ID: %d]", pPRU->stChangeRecordCmd.uwRecordID);
		m_pAppMod->WriteHMIMess(szMsg);

		return TRUE;
	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Modify Pattern Center Failed!");
		m_pAppMod->WriteHMIMess("Pattern Learn Failed!");

		return FALSE;
	}
}

BOOL CPRTaskStn::ManualLrnDie(PRU *pPRU)
{
	// learn die
	pPRU->bLoaded = FALSE;

	if (PRS_ManualLrnDie(pPRU))
	{
		pPRU->bLoaded = TRUE;

		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Pattern Learnt");

		CString szMsg = _T("");
		szMsg.Format("Pattern Learn Succeed [ID: %d]", pPRU->stSrchDieCmd.auwRecordID[0]);
		m_pAppMod->WriteHMIMess(szMsg);

		return TRUE;
	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Pattern Learn Failed!");
		m_pAppMod->WriteHMIMess("Pattern Learn Failed!");

		return FALSE;
	}
}

VOID CPRTaskStn::SrchPRDie(PRU *pPRU)
{
	if (!pPRU->bLoaded)
	{
		m_pAppMod->WriteHMIMess("Pattern Search Error: Please load pattern first.");
		return;
	}

	if (PRS_SrchDie(pPRU))
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Pattern Found");

		CString szMsg = _T("");
		m_pAppMod->WriteHMIMess("Pattern Search Succeed!");
		
		if (pPRU->bIs5MBCam && pPRU->bUseFloatingPointResult) //20150728 float PR
		{
			szMsg.Format("XY: (%.2f,%.2f), Ang: %.2f, Score: %.2f", 
						pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, pPRU->fAng, pPRU->fScore);
			m_pAppMod->WriteHMIMess(szMsg);
		}
		else
		{
			szMsg.Format("XY: (%ld,%ld), Ang: %.2f, Score: %.2f", 
						pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, pPRU->fAng, pPRU->fScore);
			m_pAppMod->WriteHMIMess(szMsg);
		}

#ifdef CHECK_COLLET_DIRT
		//if (pPRU->stSrchDieCmd.stDieInsp.stTmpl.emChipDieChk == PR_FALSE)
		if ((pPRU->szStnName == "PB1ColletDirt") || (pPRU->szStnName == "PB2ColletDirt"))
		{
			if (pPRU->ulDefectPRStatusResult == PR_ERR_NOERR)
			{
				szMsg.Format("DefectPRStatus:%ld(NoErr), LargestDefectArea:%ld, TotalDefectArea:%ld ", 
					pPRU->ulDefectPRStatusResult, pPRU->ulLargestDefectAreaResult,  pPRU->ulTotalDefectAreaResult);
			}
			else
			{
				szMsg.Format("DefectPRStatus:%ld(DEFECTIVE_DIE), LargestDefectArea:%ld, TotalDefectArea:%ld ", 
					pPRU->ulDefectPRStatusResult, pPRU->ulLargestDefectAreaResult,  pPRU->ulTotalDefectAreaResult);
			}
			m_pAppMod->WriteHMIMess(szMsg);
		}
#endif
		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);
	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Pattern Search Failed!");
		m_pAppMod->WriteHMIMess("Pattern Search Failed!");
	}
}

VOID CPRTaskStn::SetPRDieSrchArea(PRU *pPRU, PR_COORD coCorner1, PR_COORD coCorner2)
{
	CString szOldValue;
	szOldValue.Format("ID = %ld, Upper Left Corner = (%d, %d), Lower Right Corner = (%d, %d)",
					  pPRU->emID,
					  pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.x,
					  pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.y,
					  pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.x,
					  pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.y);

	pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1 = coCorner1;
	pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2 = coCorner2;

	pPRU->stSrchDieCmd.stDieAlign.stStreet.coProbableDieCentre.x = (coCorner1.x + coCorner2.x) / 2;	// 20150901
	pPRU->stSrchDieCmd.stDieAlign.stStreet.coProbableDieCentre.y = (coCorner1.y + coCorner2.y) / 2;	// 20150901

	CString szNewValue;
	szNewValue.Format("ID = %ld, Upper Left Corner = (%d, %d), Lower Right Corner = (%d, %d)",
					  pPRU->emID,
					  pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.x,
					  pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.y,
					  pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.x,
					  pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.y);

	LogPRParameter("PRTaskStn", __FUNCTION__, "Search Area", szOldValue, szNewValue);

	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Pattern Search Area Updated");
	m_pAppMod->WriteHMIMess("Pattern Search Area Updated");
}

// PR Menu routines: Shape
BOOL CPRTaskStn::LearnPRShape(PRU *pPRU)
{
	// learn shape
	pPRU->bLoaded = FALSE;

	if (PRS_LrnShape(pPRU))
	{
		pPRU->bLoaded = TRUE;

		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Shape Learnt");

		CString szMsg = _T("");
		szMsg.Format("Shape Learn Succeed [ID: %d]", pPRU->stDetectShapeCmd.uwRecordID);
		m_pAppMod->WriteHMIMess(szMsg);
		
		return TRUE;
	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Shape Learn Failed!");
		m_pAppMod->WriteHMIMess("Shape Learn Failed!");

		return FALSE;
	}
}

VOID CPRTaskStn::DetectPRShape(PRU *pPRU)
{
	if (!pPRU->bLoaded)
	{
		m_pAppMod->WriteHMIMess("Shape Search Error: Please load pattern first.");
		return;
	}

	if (PRS_DetectShape(pPRU))
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Shape Found");

		CString szMsg = _T("");
		m_pAppMod->WriteHMIMess("Shape Search Succeed!");
		szMsg.Format("XY: (%ld,%ld), Score: %.2f", 
					 pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, pPRU->fScore);
		m_pAppMod->WriteHMIMess(szMsg);

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);
	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Shape Search Failed!");
		m_pAppMod->WriteHMIMess("Shape Search Failed!");
	}
}

VOID CPRTaskStn::SetPRShapeSrchArea(PRU *pPRU, PR_COORD coCorner1, PR_COORD coCorner2)
{
	CString szOldValue;
	szOldValue.Format("ID = %ld, Upper Left Corner = (%d, %d), Lower Right Corner = (%d, %d)",
					  pPRU->emID,
					  pPRU->stDetectShapeCmd.stInspWin.coCorner1.x,
					  pPRU->stDetectShapeCmd.stInspWin.coCorner1.y,
					  pPRU->stDetectShapeCmd.stInspWin.coCorner2.x,
					  pPRU->stDetectShapeCmd.stInspWin.coCorner2.y);

	pPRU->stDetectShapeCmd.stInspWin.coCorner1	= coCorner1;
	pPRU->stDetectShapeCmd.stInspWin.coCorner2	= coCorner2;

	CString szNewValue;
	szNewValue.Format("ID = %ld, Upper Left Corner = (%d, %d), Lower Right Corner = (%d, %d)",
					  pPRU->emID,
					  pPRU->stDetectShapeCmd.stInspWin.coCorner1.x,
					  pPRU->stDetectShapeCmd.stInspWin.coCorner1.y,
					  pPRU->stDetectShapeCmd.stInspWin.coCorner2.x,
					  pPRU->stDetectShapeCmd.stInspWin.coCorner2.y);

	LogPRParameter("PRTaskStn", __FUNCTION__, "Search Area", szOldValue, szNewValue);

	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Shape Search Area Updated");
	m_pAppMod->WriteHMIMess("Shape Search Area Updated");
}
	

// PR Menu routines: Tmpl
BOOL CPRTaskStn::LoadPRTmpl(PRU *pPRU)
{

	// load template
	pPRU->bLoaded = FALSE;

	if (PRS_LoadTmpl(pPRU))
	{
		pPRU->bLoaded = TRUE;

		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Tmpl Loaded");

		CString szMsg = _T("");
		szMsg.Format("Tmpl Load Succeed [ID: %d]", pPRU->stSrchTmplCmd.uwRecordID);
		m_pAppMod->WriteHMIMess(szMsg);
		
		return TRUE;
	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Tmpl Load Failed!");
		m_pAppMod->WriteHMIMess("Tmpl Load Failed!");

		return FALSE;
	}
}

VOID CPRTaskStn::SrchPRTmpl(PRU *pPRU)
{
	if (!pPRU->bLoaded)
	{
		m_pAppMod->WriteHMIMess("Tmpl Search Error: Please load Tmpl first.");
		return;
	}

	if (PRS_SrchTmpl(pPRU))
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Tmpl Found");

		CString szMsg = _T("");
		m_pAppMod->WriteHMIMess("Tmp Search Succeed!");
		szMsg.Format("XY: (%ld,%ld), Ang: %.2f", 
					 pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, pPRU->fAng);
		m_pAppMod->WriteHMIMess(szMsg);

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);
	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Search Tmpl Failed!");
		m_pAppMod->WriteHMIMess("Tmpl Search Failed!");
	}
}

VOID CPRTaskStn::SetPRTmplSrchArea(PRU *pPRU, PR_COORD coCorner1, PR_COORD coCorner2)
{
	CString szOldValue;
	szOldValue.Format("ID = %ld, Upper Left Corner = (%d, %d), Lower Right Corner = (%d, %d)",
					  pPRU->emID,
					  pPRU->stSrchTmplCmd.stSrchWin.coCorner1.x,
					  pPRU->stSrchTmplCmd.stSrchWin.coCorner1.y,
					  pPRU->stSrchTmplCmd.stSrchWin.coCorner2.x,
					  pPRU->stSrchTmplCmd.stSrchWin.coCorner2.y);

	pPRU->stSrchTmplCmd.stSrchWin.coCorner1 = coCorner1;
	pPRU->stSrchTmplCmd.stSrchWin.coCorner2 = coCorner2;

	CString szNewValue;
	szNewValue.Format("ID = %ld, Upper Left Corner = (%d, %d), Lower Right Corner = (%d, %d)",
					  pPRU->emID,
					  pPRU->stSrchTmplCmd.stSrchWin.coCorner1.x,
					  pPRU->stSrchTmplCmd.stSrchWin.coCorner1.y,
					  pPRU->stSrchTmplCmd.stSrchWin.coCorner2.x,
					  pPRU->stSrchTmplCmd.stSrchWin.coCorner2.y);

	LogPRParameter("PRTaskStn", __FUNCTION__, "Search Area", szOldValue, szNewValue);

	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Tmpl Search Area Updated");
	m_pAppMod->WriteHMIMess("Tmpl Search Area Updated");
}


// PR Menu routines: Kerf Fitting
BOOL CPRTaskStn::LearnPRKerf(PRU *pPRU)	// 20141023 Yip: Add Kerf Fitting Functions
{
	// learn kerf fitting
	pPRU->bLoaded = FALSE;

	if (PRS_LrnKerf(pPRU))
	{
		pPRU->bLoaded = TRUE;

		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Kerf Fitting Learnt");

		CString szMsg = _T("");
		szMsg.Format("Kerf Fitting Learn Succeed [ID: %d]", pPRU->stInspKerfCmd.uwRecordID);
		m_pAppMod->WriteHMIMess(szMsg);
		
		return TRUE;
	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Kerf Fitting Learn Failed!");
		m_pAppMod->WriteHMIMess("Kerf Fitting Learn Failed!");

		return FALSE;
	}
}

VOID CPRTaskStn::InspPRKerf(PRU *pPRU)	// 20141023 Yip: Add Kerf Fitting Functions
{
	if (!pPRU->bLoaded)
	{
		m_pAppMod->WriteHMIMess("Kerf Fitting Search Error: Please load Kerf fitting first.");
		return;
	}

	if (PRS_InspKerf(pPRU))
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Kerf Fitting Found");

		CString szMsg = _T("");
		m_pAppMod->WriteHMIMess("Kerf Fitting Search Succeed!");
		
		if (pPRU->bIs5MBCam && pPRU->bUseFloatingPointResult) //20150728 float PR
		{
			szMsg.Format("XY: (%.2f,%.2f), Angle: %.2f, Score: %.2f", 
					 pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, pPRU->fAng, pPRU->fScore);
		}
		else
		{
			szMsg.Format("XY: (%ld,%ld), Angle: %.2f, Score: %.2f", 
					 pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, pPRU->fAng, pPRU->fScore);
		}
		m_pAppMod->WriteHMIMess(szMsg);

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);
	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Kerf Fitting Search Failed!");
		m_pAppMod->WriteHMIMess("Kerf Fitting Search Failed!");
	}
}

VOID CPRTaskStn::SetPRKerfSrchArea(PRU *pPRU, PR_COORD coCorner1, PR_COORD coCorner2)	// 20141023 Yip: Add Kerf Fitting Functions
{
	CString szOldValue;
	szOldValue.Format("ID = %ld, Upper Left Corner = (%d, %d), Lower Right Corner = (%d, %d)",
					  pPRU->emID,
					  pPRU->stLrnKerfCmd.stROI.coCorner1.x,
					  pPRU->stLrnKerfCmd.stROI.coCorner1.y,
					  pPRU->stLrnKerfCmd.stROI.coCorner2.x,
					  pPRU->stLrnKerfCmd.stROI.coCorner2.y);

	pPRU->stLrnKerfCmd.stROI.coCorner1	= coCorner1;
	pPRU->stLrnKerfCmd.stROI.coCorner2	= coCorner2;

	CString szNewValue;
	szNewValue.Format("ID = %ld, Upper Left Corner = (%d, %d), Lower Right Corner = (%d, %d)",
					  pPRU->emID,
					  pPRU->stLrnKerfCmd.stROI.coCorner1.x,
					  pPRU->stLrnKerfCmd.stROI.coCorner1.y,
					  pPRU->stLrnKerfCmd.stROI.coCorner2.x,
					  pPRU->stLrnKerfCmd.stROI.coCorner2.y);

	LogPRParameter("PRTaskStn", __FUNCTION__, "Search Area", szOldValue, szNewValue);

	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Kerf Fitting Search Area Updated");
	m_pAppMod->WriteHMIMess("Kerf Fitting Search Area Updated");
}


BOOL CPRTaskStn::LrnACFCmd(PRU *pPRU, PRU *pPRUBackground)
{
	CString szMsg = _T("");
	PR_COORD coCentre;
	coCentre.x = PR_DEF_CENTRE_X;
	coCentre.y = PR_DEF_CENTRE_Y;

	// learn die

	pPRU->bLoaded = FALSE;
	pPRUBackground->bLoaded = FALSE;

	if (PRS_LrnACFCmd(pPRU))
	{
		pPRU->bLoaded = TRUE;

		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Pattern Learnt");
		szMsg.Format("Pattern Learn Succeed [ID: %d]", pPRU->stSrchACFCmd.uwRecordID);
		m_pAppMod->WriteHMIMess(szMsg);

		return TRUE;
	}
	else
	{		
		for (INT i = 0; i < (sizeof(emPR_ACFResult) / sizeof(emPR_ACFResult[0])); i++)
		{
			if (emPR_ACFResult[i] == pPRU->lACFResult)
			{
				szMsg.Format("%s", cPR_ACFResultMsg[i]);
				m_pAppMod->WriteHMIMess(szMsg);
			}
		}
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Pattern Learn Failed!");
		m_pAppMod->WriteHMIMess("Pattern Learn Failed!");

		return FALSE;
	}
}

BOOL CPRTaskStn::LrnBackgroundCmd(PRU *pPRU, PRU *pPRUBackground)
{
	PR_COORD coCentre;
	coCentre.x = PR_DEF_CENTRE_X;
	coCentre.y = PR_DEF_CENTRE_Y;

	// learn die
	pPRUBackground->bLoaded	= FALSE;

	if (PRS_GrabShareImgCmd(pPRUBackground))
	{
		pPRUBackground->bLoaded = TRUE;


		PRS_DisplayText(pPRUBackground->nCamera, 1, PRS_MSG_ROW1, "Pattern Learnt");

		return TRUE;
	}
	else
	{
		PRS_DisplayText(pPRUBackground->nCamera, 1, PRS_MSG_ROW1, "Pattern Learn Failed!");
		m_pAppMod->WriteHMIMess("Pattern Learn Failed!");

		return FALSE;
	}
}

VOID CPRTaskStn::SrchACFCmd(PRU *pPRU)
{
	CString szMsg = _T("");

	if (!pPRU->bLoaded)
	{
		m_pAppMod->WriteHMIMess("Pattern Search Error: Please load pattern first.");
		return;
	}

	if (PRS_SrchACFCmd(pPRU))
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Pattern Found");

		m_pAppMod->WriteHMIMess("Pattern Search Succeed!");


		if (pPRU->bIs5MBCam && pPRU->bUseFloatingPointResult) //20150728 float PR
		{
			szMsg.Format("XY: (%.2f,%.2f), Ang: %.2f, Score: %.2f", 
				pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, pPRU->fAng, pPRU->fScore / 100.0); //20130429
			m_pAppMod->WriteHMIMess(szMsg);
			PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);
		}
		else
		{
			szMsg.Format("XY: (%.2f,%.2f), Ang: %.2f, Score: %.2f", 
				pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, pPRU->fAng, pPRU->fScore / 100.0); //20130429
			m_pAppMod->WriteHMIMess(szMsg);
			PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);
		}
	}
	else
	{
		for (INT i = 0; i < (sizeof(emPR_ACFResult) / sizeof(emPR_ACFResult[0])); i++)
		{
			if (emPR_ACFResult[i] == pPRU->lACFResult)
			{
				szMsg.Format("%s", cPR_ACFResultMsg[i]);
				m_pAppMod->WriteHMIMess(szMsg);
			}
		}	
	}
}

/////////////////////////////////////////
// Search Function
////////////////////////////////////////
BOOL CPRTaskStn::SearchPR(PRU *pPRU, BOOL bIsWait, LONG lPRDelay, CAC9000Stn *pStation)
{
	BOOL bResult = FALSE;
	PRDebugInfo stPRDebugInfo; //20150423

	if (bIsWait)
	{
		if (lPRDelay == -1)
		{
			if (pPRU->szStnName == "MB")
			{
				Sleep(m_lPRDelay2);
			}
			else
			{
				Sleep(m_lPRDelay);
			}
		}
		else
		{
			Sleep(lPRDelay);
		}
	}

	if (!m_bPRSelected)
	{
		return TRUE;
	}

	if (!pPRU->bLoaded)
	{
		pPRU->rcoDieCentre.x	= PR_DEF_CENTRE_X;
		pPRU->rcoDieCentre.y	= PR_DEF_CENTRE_Y;
		//pPRU->rcoDieCentre.x = (PR_REAL)PR_DEF_CENTRE_X; //20150728 float PR
		//pPRU->rcoDieCentre.y = (PR_REAL)PR_DEF_CENTRE_Y;
		pPRU->fAng			= 0.0;
		pPRU->fScore		= 0.0;
		pPRU->rszScale.x	= 0.0;
		pPRU->rszScale.y	= 0.0;

		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "PR Pattern Not Loaded");

		return FALSE;
	}

	if  (g_bPRDebugInfo)  //20150423
	{
		stPRDebugInfo.dwCmdStartTime = GetTickCount();
	}

	m_pAppMod->TC_AddJob(m_lAddJobPR_GRAB);
	m_pAppMod->TC_AddJob(m_lAddJobPR_PROCESS);

#if 1	// Vision UI
	if(pPRU->emRecordType == PRS_ACF_TYPE)
	{
		bResult = PRS_SrchACFCmd(pPRU);
	}
	else
	{
		bResult = PRS_ManualSrchCmd(pPRU);
	}
#else
	switch (pPRU->emRecordType)
	{
	// Pattern Matching or Edge Matching
	case PRS_DIE_TYPE:
		bResult = PRS_SrchDie(pPRU);
		break;

	// Shape Fitting
	case PRS_SHAPE_TYPE:
		bResult = PRS_DetectShape(pPRU);
		break;

	// Tmpl Fitting
	case PRS_TMPL_TYPE:
		bResult = PRS_SrchTmpl(pPRU);
		break;

	// Kerf Fitting
	case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
		bResult = PRS_InspKerf(pPRU);
		break;

	case PRS_ACF_TYPE:
		bResult = PRS_SrchACFCmd(pPRU);
		break;
	}
#endif
	
	m_pAppMod->TC_DelJob(m_lAddJobPR_GRAB);
	m_pAppMod->TC_DelJob(m_lAddJobPR_PROCESS);

#if 1 //20150423
	if  (g_bPRDebugInfo)  //20150423
	{
		stPRDebugInfo.dwOverallTime = GetTickCount();
		CString szMsg = "";
		if (pStation != NULL)
		{
			szMsg.Format("Stn:%s PRU:%s Grab and Process Time: %.5f sec", pStation->GetStnName(), pPRU->szStnName, (stPRDebugInfo.dwOverallTime - stPRDebugInfo.dwCmdStartTime) / 1000.0);
		}
		else
		{
			szMsg.Format("PRU:%s Grab and Process Time: %.5f sec", pPRU->szStnName, (stPRDebugInfo.dwOverallTime - stPRDebugInfo.dwCmdStartTime) / 1000.0);
		}
		theApp.DisplayMessage(szMsg);
	}
#endif
	return bResult;
}

BOOL CPRTaskStn::SearchPRGrabOnly(PRU *pPRU)
{
	BOOL bResult = FALSE;

	if (!pPRU->bLoaded)
	{
		pPRU->rcoDieCentre.x	= PR_DEF_CENTRE_X;
		pPRU->rcoDieCentre.y	= PR_DEF_CENTRE_Y;
		pPRU->rcoDieCentre.x = (PR_REAL)PR_DEF_CENTRE_X; //20150728 float PR
		pPRU->rcoDieCentre.y = (PR_REAL)PR_DEF_CENTRE_Y;
		pPRU->fAng			= 0.0;
		pPRU->fScore		= 0.0;

		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "PR Pattern Not Loaded");

		return FALSE;
	}

	m_pAppMod->TC_AddJob(m_lAddJobPR_GRAB);

#if 1
	if(pPRU->emRecordType == PRS_ACF_TYPE)
	{
		bResult = PRS_SrchACFCmdGrabOnly(pPRU);
	}
	else
	{
		bResult = PRS_ManualSrchGrabOnly(pPRU);
	}
#else
	switch (pPRU->emRecordType)
	{
	// Pattern Matching or Edge Matching
	case PRS_DIE_TYPE:
		bResult = PRS_SrchDieGrabOnly(pPRU);
		break;

	// Shape Fitting
	case PRS_SHAPE_TYPE:
		bResult = PRS_DetectShapeGrabOnly(pPRU);
		break;

	// Tmpl Fitting
	case PRS_TMPL_TYPE:
		bResult = PRS_SrchTmplGrabOnly(pPRU);
		break;

	// Kerf Fitting
	case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
		bResult = PRS_InspKerfGrabOnly(pPRU);
		break;
	
	case PRS_ACF_TYPE:
		bResult = PRS_SrchACFCmdGrabOnly(pPRU);
		break;
	}
#endif

	m_pAppMod->TC_DelJob(m_lAddJobPR_GRAB);

	return bResult;
}

BOOL CPRTaskStn::SearchPRProcessOnly(PRU *pPRU)
{
	BOOL bResult = FALSE;

	if (!pPRU->bLoaded)
	{
		pPRU->rcoDieCentre.x	= PR_DEF_CENTRE_X;
		pPRU->rcoDieCentre.y	= PR_DEF_CENTRE_Y;
		pPRU->rcoDieCentre.x = (PR_REAL)PR_DEF_CENTRE_X; //20150728 float PR
		pPRU->rcoDieCentre.y = (PR_REAL)PR_DEF_CENTRE_Y;

		pPRU->fAng			= 0.0;
		pPRU->fScore		= 0.0;

		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "PR Pattern Not Loaded");

		return FALSE;
	}

	m_pAppMod->TC_AddJob(m_lAddJobPR_PROCESS);

#if 1
	if(pPRU->emRecordType == PRS_ACF_TYPE)
	{
		bResult = PRS_SrchACFCmdProcessOnly(pPRU);
	}
	else
	{
		bResult = PRS_ManualSrchProcessOnly(pPRU);
	}
#else
	switch (pPRU->emRecordType)
	{
	// Pattern Matching or Edge Matching
	case PRS_DIE_TYPE:
		bResult = PRS_SrchDieProcessOnly(pPRU);
		break;

	// Shape Fitting
	case PRS_SHAPE_TYPE:
		bResult = PRS_DetectShapeProcessOnly(pPRU);
		break;

	// Tmpl Fitting
	case PRS_TMPL_TYPE:
		bResult = PRS_SrchTmplProcessOnly(pPRU);
		break;

	// Kerf Fitting
	case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
		bResult = PRS_InspKerfProcessOnly(pPRU);
		break;

	case PRS_ACF_TYPE:
		bResult = PRS_SrchACFCmdProcessOnly(pPRU);
		break;
	}
#endif

	m_pAppMod->TC_DelJob(m_lAddJobPR_PROCESS);

	return bResult;
}

BOOL CPRTaskStn::SearchPRCentre(PRU *pPRU, BOOL bIsInvert, LONG lPRDelay) //20120613
{
	MTR_POSN mtr_offset;
	BOOL bResult = FALSE;

	if (!m_bPRSelected)
	{
		return TRUE;
	}

	if (lPRDelay == -1)
	{
		if (pPRU->szStnName == "MB")
		{
			Sleep(m_lPRDelay2);
		}
		else
		{
			Sleep(m_lPRDelay);
		}
	}
	else
	{
		Sleep(lPRDelay);
	}
	
	if (!pPRU->bLoaded)
	{
		pPRU->rcoDieCentre.x	= PR_DEF_CENTRE_X;
		pPRU->rcoDieCentre.y	= PR_DEF_CENTRE_Y;
		pPRU->rcoDieCentre.x = (PR_REAL)PR_DEF_CENTRE_X; //20150728 float PR
		pPRU->rcoDieCentre.y = (PR_REAL)PR_DEF_CENTRE_Y;
		pPRU->fAng			= 0.0;
		pPRU->fScore		= 0.0;

		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "PR Pattern Not Loaded");

		return FALSE;
	}

	m_pAppMod->TC_AddJob(m_lAddJobPR_GRAB);
	m_pAppMod->TC_AddJob(m_lAddJobPR_PROCESS);

#if 1
	if(pPRU->emRecordType == PRS_ACF_TYPE)
	{
		bResult = PRS_SrchACFCmd(pPRU);
	}
	else
	{
		bResult = PRS_ManualSrchCmd(pPRU);
	}
#else
	switch (pPRU->emRecordType)
	{
	// Pattern Matching or Edge Matching
	case PRS_DIE_TYPE:
		bResult = PRS_SrchDie(pPRU);
		break;

	// Shape Fitting
	case PRS_SHAPE_TYPE:
		bResult = PRS_DetectShape(pPRU);
		break;

	// Tmpl Fitting
	case PRS_TMPL_TYPE:
		bResult = PRS_SrchTmpl(pPRU);
		break;

	// Kerf Fitting
	case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
		bResult = PRS_InspKerf(pPRU);
		break;

	case PRS_ACF_TYPE:
		bResult = PRS_SrchACFCmd(pPRU);
		break;
	}
#endif

	m_pAppMod->TC_DelJob(m_lAddJobPR_GRAB);
	m_pAppMod->TC_DelJob(m_lAddJobPR_PROCESS);

	// Die found. Align table to centre
	if (bResult)
	{
		if (pPRU->bIs5MBCam && pPRU->bUseFloatingPointResult) //20150728 float PR
		{
			PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);
		}
		else
		{
			PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);
		}

		if (bIsInvert) //20120613
		{
			//mtr_offset.x = -1* mtr_offset.x;
			mtr_offset.y = -1 * mtr_offset.y;
		}

		if (MoveRelativeXY(mtr_offset.x, mtr_offset.y, GMP_WAIT) != GMP_SUCCESS)
		{
			bResult = FALSE;
		}
	}

	return bResult;
}

VOID CPRTaskStn::SinglePRCycleOperation(PRU *pPRU, DOUBLE dTestAng, DOUBLE dTestX, DOUBLE	dTestY, PRINT_DATA	*stPrintData)
{	
	MTR_POSN	mtrCurPosn;
	INT i = 0;

	//DOUBLE		dData[20];

	if (!m_bPRCycle)
	{
		return;
	}

	do
	{	
		PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
		
		if (
			MoveXYToPRCalibPosn() != gnAMS_OK ||
			MoveZToPRCalibLevel() != gnAMS_OK
		   )
		{
			m_bPRCycle = FALSE;
			break;
		}

		if (pPRU->szStnName == "MB")
		{
			Sleep(m_lPRDelay2);
		}
		else
		{
			Sleep(m_lPRDelay);
		}

		stPrintData->dData[0] = GetMotorX().GetEncPosn();
		stPrintData->dData[1] = GetMotorY().GetEncPosn();
		stPrintData->dData[2] = GetMotorT().GetEncPosn();

		if (!SearchPRCentre(pPRU))
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "PR Failed!");
			m_bPRCycle = FALSE;
			break;
		}

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

		if (pPRU->szStnName == "MB")
		{
			Sleep(m_lPRDelay2);
		}
		else
		{
			Sleep(m_lPRDelay);
		}

		mtrCurPosn.x = GetMotorX().GetEncPosn();
		mtrCurPosn.y = GetMotorY().GetEncPosn();

		stPrintData->dData[3] = GetMotorX().GetEncPosn();
		stPrintData->dData[4] = GetMotorY().GetEncPosn();
		stPrintData->dData[5] = GetMotorT().GetEncPosn();
		stPrintData->dData[6] = pPRU->rcoDieCentre.x;
		stPrintData->dData[7] = pPRU->rcoDieCentre.y;

		LONG lTestAngStep	= AngleToCount(dTestAng, GetMotorT().stAttrib.dDistPerCount);
		LONG lTestXStep		= DistanceToCount(dTestX * 1000 , GetMotorX().stAttrib.dDistPerCount);
		LONG lTestYStep		= DistanceToCount(dTestY * 1000, GetMotorY().stAttrib.dDistPerCount);
	
		// Forward
		if (
			(GetMotorT().MoveRelative(lTestAngStep, GMP_WAIT) != gnAMS_OK) ||
			(GetMotorX().MoveRelative(lTestXStep, GMP_WAIT) != gnAMS_OK) ||
			(GetMotorY().MoveRelative(lTestYStep, GMP_WAIT) != gnAMS_OK)
		   )
		{
			m_bPRCycle = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		stPrintData->dData[8] = GetMotorX().GetEncPosn();
		stPrintData->dData[9] = GetMotorY().GetEncPosn();
		stPrintData->dData[10] = GetMotorT().GetEncPosn();

		// Backward
		if (
			(GetMotorT().MoveRelative(-lTestAngStep, GMP_WAIT) != gnAMS_OK) ||
			(GetMotorX().MoveRelative(-lTestXStep, GMP_WAIT) != gnAMS_OK) ||
			(GetMotorY().MoveRelative(-lTestYStep, GMP_WAIT) != gnAMS_OK)
		   )
		{
			m_bPRCycle = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		stPrintData->dData[11] = GetMotorX().GetEncPosn();
		stPrintData->dData[12] = GetMotorY().GetEncPosn();
		stPrintData->dData[13] = GetMotorT().GetEncPosn();

		if (!SearchPR(pPRU))
		{
			m_bPRCycle = FALSE;
			break;
		}

		if (pPRU->bIs5MBCam && pPRU->bUseFloatingPointResult) //20150728 float PR
		{
			stPrintData->dData[14] = pPRU->rcoDieCentre.x;
			stPrintData->dData[15] = pPRU->rcoDieCentre.y;
		}
		else
		{
			stPrintData->dData[14] = pPRU->rcoDieCentre.x;
			stPrintData->dData[15] = pPRU->rcoDieCentre.y;
		}

		CString szMsg = "";
		m_ulPRCycle++;
		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);
		szMsg.Format("Cycle: %ld", m_ulPRCycle);
		PRS_DisplayText(pPRU->nCamera, 2, 0, szMsg);

	} while (0);
		
}

/////////////////////////////////////////////////////////////////
// COR Calc
/////////////////////////////////////////////////////////////////
VOID CPRTaskStn::CalDPosnAfterRotate(D_MTR_POSN dmtrTableCOR, MTR_POSN mtrCurPosn, DOUBLE dAngle, D_MTR_POSN *p_dmtrResultPosn)
{
	DOUBLE dAngleRad;

	 //COR not calibrated
	if (!m_bCORCalibrated)
	{
		p_dmtrResultPosn->x = mtrCurPosn.x;
		p_dmtrResultPosn->y = mtrCurPosn.y;
		return;
	}

	dAngleRad = (PI / 180.0) * dAngle;

	p_dmtrResultPosn->x = (+((DOUBLE)mtrCurPosn.x - dmtrTableCOR.x) * cos(dAngleRad)
						   - ((DOUBLE)mtrCurPosn.y - dmtrTableCOR.y) * sin(dAngleRad)
						   + dmtrTableCOR.x);

	p_dmtrResultPosn->y = (+((DOUBLE)mtrCurPosn.x - dmtrTableCOR.x) * sin(dAngleRad)
						   + ((DOUBLE)mtrCurPosn.y - dmtrTableCOR.y) * cos(dAngleRad)
						   + dmtrTableCOR.y);

	return;
}

VOID CPRTaskStn::CalPosnAfterRotate(MTR_POSN mtrCurPosn, DOUBLE dAngle, MTR_POSN *p_mtrResultPosn)
{
	DOUBLE dAngleRad;
	D_MTR_POSN dmtrTableCOR;

	if (!m_bCORCalibrated[m_lCORCamNum] || dAngle == 0.0)
	{
		p_mtrResultPosn->x = mtrCurPosn.x;
		p_mtrResultPosn->y = mtrCurPosn.y;
		return;
	}

	dAngleRad = (PI / 180.0) * dAngle;

	INT i = 0;
	INT nIndex = 0;

	if (fabs(dAngle) > m_dCORLUT[m_lCORCamNum][COR_ARRAY_SIZE - 1])
	{
		nIndex = COR_ARRAY_SIZE - 1;
	}
	else
	{
		for (i = 0; i < COR_ARRAY_SIZE; i++)
		{
			if (fabs(dAngle) <= m_dCORLUT[m_lCORCamNum][i])
			{
				nIndex = i;
				break;
			}
		}
	}

	// negative angle
	if (dAngle < 0.0)
	{
		dmtrTableCOR.x = m_dmtrTableCOR_Neg[m_lCORCamNum][nIndex].x;
		dmtrTableCOR.y = m_dmtrTableCOR_Neg[m_lCORCamNum][nIndex].y;
	}
	// positive ange
	else
	{
		dmtrTableCOR.x = m_dmtrTableCOR_Pos[m_lCORCamNum][nIndex].x;
		dmtrTableCOR.y = m_dmtrTableCOR_Pos[m_lCORCamNum][nIndex].y;
	}

	p_mtrResultPosn->x = DoubleToLong(+((DOUBLE)mtrCurPosn.x - dmtrTableCOR.x) * cos(dAngleRad)
									  - ((DOUBLE)mtrCurPosn.y - dmtrTableCOR.y) * sin(dAngleRad)
									  + dmtrTableCOR.x);

	p_mtrResultPosn->y = DoubleToLong(+((DOUBLE)mtrCurPosn.x - dmtrTableCOR.x) * sin(dAngleRad)
									  + ((DOUBLE)mtrCurPosn.y - dmtrTableCOR.y) * cos(dAngleRad)
									  + dmtrTableCOR.y);

	return;
}


#if 1 //20140627 PBDL cal
BOOL CPRTaskStn::SetThermalCalibRefPosn(PRU *pPRU)
{
	CString szTitle		= "SET THERMAL REF POSN";
	CString szText		= "";
	CString szMsg		= "";
	BOOL bResult		= TRUE;

	if (!m_bPRSelected)
	{
		szText.Format("Module PR Not Selected. Operation Abort!");
		m_pAppMod->HMIMessageBox(MSG_OK, szTitle, szText);
		return FALSE;
	}
	if (!pPRU->bCalibrated)
	{
		szText.Format("XY Calibration NOT Done. Operation Abort!");
		m_pAppMod->HMIMessageBox(MSG_OK, szTitle, szText);
		szText.Format("Please Finish XY Calibration First.");
		m_pAppMod->HMIMessageBox(MSG_OK, szTitle, szText);
		return FALSE;
	}

	m_pAppMod->WriteHMIMess("Set PBDL Calibration");

	PRS_ClearScreen(pPRU->nCamera);

	// operation frame (with 0.1m calibration window)
	do
	{
		if (
			MoveZToPRCalibLevel() != GMP_SUCCESS ||
			MoveXYToPRCalibPosn() != GMP_SUCCESS
		   )
		{
			m_pAppMod->WriteHMIMess("Calibration Abort!");
			return FALSE;
		}

		Sleep(m_lCalibPRDelay);

		if (!SearchPRCentre(pPRU))
		{
			m_pAppMod->DisplayMessage("SetThermalCalibRefPosn: PR Search error.");
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		m_mtrThermalCalibRefPosn.x = GetMotorX().GetEncPosn();
		m_mtrThermalCalibRefPosn.y = GetMotorY().GetEncPosn();

		szMsg.Format("SetThermalCalibRefPosn: m_lCalibPRDelay=%ld", m_lCalibPRDelay);
		m_pAppMod->DisplayMessage(szMsg);

		szMsg.Format("m_mtrThermalCalibRefPosn=(%ld,%ld)", m_mtrThermalCalibRefPosn.x, m_mtrThermalCalibRefPosn.y);
		m_pAppMod->DisplayMessage(szMsg);

		//if (
		//	(MoveAbsoluteXY(mtr_oldPosn.x, mtr_oldPosn.y, GMP_WAIT) != GMP_SUCCESS) || 
		//)
		//{
		//	bResult = FALSE;
		//	return FALSE;
		//}
		//Sleep(m_lCalibPRDelay);

		if (!SearchPR(pPRU))
		{
			m_pAppMod->DisplayMessage("SetThermalCalibRefPosn: PR Search error2.");
			bResult = FALSE;
			break;
		}
		
		if (pPRU->bIs5MBCam && pPRU->bUseFloatingPointResult) //20150728 float PR
		{
			szMsg.Format("SetThermalCalibRefPosn: Error Offset=(%.2f,%.2f)", 
						pPRU->rcoDieCentre.x - (PR_REAL)PR_DEF_CENTRE_X, 
						pPRU->rcoDieCentre.y - (PR_REAL)PR_DEF_CENTRE_Y);
		}
		else
		{
			szMsg.Format("SetThermalCalibRefPosn: Error Offset=(%ld,%ld)", 
						pPRU->rcoDieCentre.x - PR_DEF_CENTRE_X, 
						pPRU->rcoDieCentre.y - PR_DEF_CENTRE_Y);
		}
		m_pAppMod->DisplayMessage(szMsg);
		
	} while (0);

	return bResult;
}
#endif

/////////////////////////////////////////////////////////////////
// X-Y Calibration
/////////////////////////////////////////////////////////////////
BOOL CPRTaskStn::AutoCalibration(PRU *pPRU)
{

	CString szTitle		= "AUTO CALIBRATION";
	CString szText		= "";
	CString szMsg		= "";
	BOOL bResult		= TRUE;
	LONG lStepTimes		= 1;	// 20150804 120mm FOV

	MTR_POSN mtr_oldPosn;

	if (!m_bPRSelected)
	{
		szText.Format("Module PR Not Selected. Operation Abort!");
		m_pAppMod->HMIMessageBox(MSG_OK, szTitle, szText);
		return FALSE;
	}

	m_pAppMod->WriteHMIMess("Starting Auto Calibration...");

	PRS_ClearScreen(pPRU->nCamera);

	if (pPRU->emCameraNo == PR_CAMERA_10)
	{
		lStepTimes = 100;
	}
	else
	{
		lStepTimes = 1;
	}

	// operation frame (with 0.1m calibration window)
	do
	{
		if (pPRU->bCalibrated && !SearchPRCentre(pPRU))
		{
			bResult = FALSE;
			break;
		}

		mtr_oldPosn.x = GetMotorX().GetEncPosn();
		mtr_oldPosn.y = GetMotorY().GetEncPosn();

		// left
		if (
			(MoveAbsoluteXY(mtr_oldPosn.x, mtr_oldPosn.y, GMP_WAIT) != GMP_SUCCESS) || 
			SleepWithCalibDelay() != GMP_SUCCESS || 
			(MoveAbsoluteXY(mtr_oldPosn.x - 100 * lStepTimes, mtr_oldPosn.y, GMP_WAIT) != GMP_SUCCESS) 
		   )
		{
			bResult = FALSE;
			return FALSE;
		}
		Sleep(m_lCalibPRDelay);

		if (!SearchPR(pPRU))
		{
			bResult = FALSE;
			break;
		}

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

		m_stPRCalibPts[CAL_LEFT].x		= pPRU->rcoDieCentre.x;
		m_stPRCalibPts[CAL_LEFT].y		= pPRU->rcoDieCentre.y;
		m_stMtrCalibPts[CAL_LEFT].x		= mtr_oldPosn.x - 100 * lStepTimes;
		m_stMtrCalibPts[CAL_LEFT].y		= mtr_oldPosn.y;

		// right
		if (
			(MoveAbsoluteXY(mtr_oldPosn.x, mtr_oldPosn.y, GMP_WAIT) != GMP_SUCCESS) || 
			SleepWithCalibDelay() != GMP_SUCCESS || 
			(MoveAbsoluteXY(mtr_oldPosn.x + 100 * lStepTimes, mtr_oldPosn.y, GMP_WAIT) != GMP_SUCCESS) 
		   )
		{
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		PRS_ClearScreen(pPRU->nCamera);

		if (!SearchPR(pPRU))
		{
			bResult = FALSE;
			break;
		}

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

		m_stPRCalibPts[CAL_RIGHT].x		= pPRU->rcoDieCentre.x;
		m_stPRCalibPts[CAL_RIGHT].y		= pPRU->rcoDieCentre.y;
		m_stMtrCalibPts[CAL_RIGHT].x	= mtr_oldPosn.x + 100 * lStepTimes;
		m_stMtrCalibPts[CAL_RIGHT].y	= mtr_oldPosn.y;

		// up
		if (
			(MoveAbsoluteXY(mtr_oldPosn.x, mtr_oldPosn.y, GMP_WAIT) != GMP_SUCCESS) || 
			SleepWithCalibDelay() != GMP_SUCCESS ||  
			(MoveAbsoluteXY(mtr_oldPosn.x, mtr_oldPosn.y + 100 * lStepTimes, GMP_WAIT) != GMP_SUCCESS) 
		   )
		{
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		PRS_ClearScreen(pPRU->nCamera);

		if (!SearchPR(pPRU))
		{
			bResult = FALSE;
			break;
		}

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

		m_stPRCalibPts[CAL_UP].x		= pPRU->rcoDieCentre.x;
		m_stPRCalibPts[CAL_UP].y		= pPRU->rcoDieCentre.y;
		m_stMtrCalibPts[CAL_UP].x		= mtr_oldPosn.x;
		m_stMtrCalibPts[CAL_UP].y		= mtr_oldPosn.y + 100 * lStepTimes;

		// down
		if (
			(MoveAbsoluteXY(mtr_oldPosn.x, mtr_oldPosn.y, GMP_WAIT) != GMP_SUCCESS) || 
			SleepWithCalibDelay() != GMP_SUCCESS || 
			(MoveAbsoluteXY(mtr_oldPosn.x, mtr_oldPosn.y - 100 * lStepTimes, GMP_WAIT) != GMP_SUCCESS) 
		   )
		{
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		PRS_ClearScreen(pPRU->nCamera);

		if (!SearchPR(pPRU))
		{
			bResult = FALSE;
			break;
		}

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

		m_stPRCalibPts[CAL_DOWN].x		= pPRU->rcoDieCentre.x;
		m_stPRCalibPts[CAL_DOWN].y		= pPRU->rcoDieCentre.y;
		m_stMtrCalibPts[CAL_DOWN].x		= mtr_oldPosn.x;
		m_stMtrCalibPts[CAL_DOWN].y		= mtr_oldPosn.y - 100 * lStepTimes;

		// back to origin
		if (MoveAbsoluteXY(mtr_oldPosn.x, mtr_oldPosn.y, GMP_WAIT) != GMP_SUCCESS)
		{
			bResult = FALSE;
			break;
		}

	} while (0);

	PRS_DisplayVideo(pPRU);

	if (!bResult || !FindAutoCalibMatrix(pPRU, m_stMtrCalibPts, m_stPRCalibPts))
	{

		pPRU->bCalibrated	= FALSE;

		m_pAppMod->HMIMessageBox(MSG_OK, szTitle, "Calibration Failed!");
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Calibration Failed!");
		m_pAppMod->WriteHMIMess("Auto Calibration Failed!");

		return FALSE;
	}

	// operation frame (with PR_DEF_CENTRE_X/2 [2048x2048] calibration window)
	PR_RCOORD pr_diesize;
	PR_RCOORD pr_coord;
	MTR_POSN mtr_offset;
	pr_diesize.x	= 0.0;
	pr_diesize.y	= 0.0;
	pr_coord.x		= 0.0;
	pr_coord.y		= 0.0;
	mtr_offset.x	= 0;
	mtr_offset.y	= 0;

	pr_diesize.x = pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].x - pPRU->arcoLrnDieCorners[PR_UPPER_LEFT].x;
	pr_diesize.y = pPRU->arcoLrnDieCorners[PR_LOWER_RIGHT].y - pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].y;
	//switch (pPRU->emRecordType)
	//{
	//// Pattern Matching or Edge Matching
	//case PRS_DIE_TYPE:
	//	pr_diesize.x = pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].x - pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].x;
	//	pr_diesize.y = pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].y - pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].y;
	//	break;

	//// Shape Fitting
	//case PRS_SHAPE_TYPE:
	//	pr_diesize.x = pPRU->stLrnShapeCmd.stEncRect.coCorner2.x - pPRU->stLrnShapeCmd.stEncRect.coCorner1.x;
	//	pr_diesize.y = pPRU->stLrnShapeCmd.stEncRect.coCorner2.y - pPRU->stLrnShapeCmd.stEncRect.coCorner1.y;
	//	break;

	//// Tmpl Fitting
	//case PRS_TMPL_TYPE:
	//	pr_diesize.x = pPRU->stLoadTmplCmd.szTmplSize.x * 2;
	//	pr_diesize.y = pPRU->stLoadTmplCmd.szTmplSize.y * 2;
	//	break;
	//}

	do
	{
		// back to origin
		if (!SearchPRCentre(pPRU))
		{
			bResult = FALSE;
			break;
		}

		mtr_oldPosn.x = GetMotorX().GetEncPosn();
		mtr_oldPosn.y = GetMotorY().GetEncPosn();

		// left
		if (pPRU->emCameraNo == PR_CAMERA_2)
		{
			pr_coord.x = (PR_MAX_COORD + 1) / 8 * 6 - pr_diesize.x / 2;
		}
		else
		{
			pr_coord.x = (PR_MAX_COORD + 1) / 8 * 7 - pr_diesize.x / 2;
		}
		pr_coord.y = PR_DEF_CENTRE_Y;
		PRS_PRPosToMtrOffset(pPRU, pr_coord, &mtr_offset);
	
		if (
			(MoveAbsoluteXY(mtr_oldPosn.x, mtr_oldPosn.y, GMP_WAIT) != GMP_SUCCESS) || 
			SleepWithCalibDelay() != GMP_SUCCESS || 
			(MoveAbsoluteXY(mtr_oldPosn.x + mtr_offset.x, mtr_oldPosn.y, GMP_WAIT) != GMP_SUCCESS) 
		   )
		{
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		PRS_ClearScreen(pPRU->nCamera);

		if (!SearchPR(pPRU))
		{
			bResult = FALSE;
			break;
		}

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

		m_stPRCalibPts[CAL_LEFT].x		= pPRU->rcoDieCentre.x;
		m_stPRCalibPts[CAL_LEFT].y		= pPRU->rcoDieCentre.y;
		m_stMtrCalibPts[CAL_LEFT].x		= mtr_oldPosn.x + mtr_offset.x;
		m_stMtrCalibPts[CAL_LEFT].y		= mtr_oldPosn.y;

		// right
		if (pPRU->emCameraNo == PR_CAMERA_2)
		{
			pr_coord.x = (PR_MAX_COORD + 1) / 8 * 2 + pr_diesize.x / 2;
		}
		else
		{
			pr_coord.x = (PR_MAX_COORD + 1) / 8 * 1 + pr_diesize.x / 2;
		}
		pr_coord.y = PR_DEF_CENTRE_Y;
		PRS_PRPosToMtrOffset(pPRU, pr_coord, &mtr_offset);
		
		if (
			(MoveAbsoluteXY(mtr_oldPosn.x, mtr_oldPosn.y, GMP_WAIT) != GMP_SUCCESS) || 
			SleepWithCalibDelay() != GMP_SUCCESS || 
			(MoveAbsoluteXY(mtr_oldPosn.x + mtr_offset.x, mtr_oldPosn.y, GMP_WAIT) != GMP_SUCCESS) 
		   )
		{
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		PRS_ClearScreen(pPRU->nCamera);

		if (!SearchPR(pPRU))
		{
			bResult = FALSE;
			break;
		}

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

		m_stPRCalibPts[CAL_RIGHT].x		= pPRU->rcoDieCentre.x;
		m_stPRCalibPts[CAL_RIGHT].y		= pPRU->rcoDieCentre.y;
		m_stMtrCalibPts[CAL_RIGHT].x	= mtr_oldPosn.x + mtr_offset.x;
		m_stMtrCalibPts[CAL_RIGHT].y	= mtr_oldPosn.y;

		// up
		pr_coord.x = PR_DEF_CENTRE_X;
		if (pPRU->emCameraNo == PR_CAMERA_2)
		{
			pr_coord.y = (PR_MAX_COORD + 1) / 8 * 5 - pr_diesize.y / 2;
		}
		else
		{
			pr_coord.y = (PR_MAX_COORD + 1) / 8 * 7 - pr_diesize.y / 2;
		}

		PRS_PRPosToMtrOffset(pPRU, pr_coord, &mtr_offset);

		if (
			(MoveAbsoluteXY(mtr_oldPosn.x, mtr_oldPosn.y, GMP_WAIT) != GMP_SUCCESS) || 
			SleepWithCalibDelay() != GMP_SUCCESS || 
			(MoveAbsoluteXY(mtr_oldPosn.x, mtr_oldPosn.y + mtr_offset.y, GMP_WAIT) != GMP_SUCCESS) 
		   )
		{
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		PRS_ClearScreen(pPRU->nCamera);

		if (!SearchPR(pPRU))
		{
			bResult = FALSE;
			break;
		}

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

		m_stPRCalibPts[CAL_UP].x		= pPRU->rcoDieCentre.x;
		m_stPRCalibPts[CAL_UP].y		= pPRU->rcoDieCentre.y;
		m_stMtrCalibPts[CAL_UP].x		= mtr_oldPosn.x;
		m_stMtrCalibPts[CAL_UP].y		= mtr_oldPosn.y + mtr_offset.y;

		// down
		pr_coord.x = PR_DEF_CENTRE_X;
		if (pPRU->emCameraNo == PR_CAMERA_2)
		{
			pr_coord.y = (PR_MAX_COORD + 1) / 8 * 3 + pr_diesize.y / 2;
		}
		else
		{
			pr_coord.y = (PR_MAX_COORD + 1) / 8 * 1 + pr_diesize.y / 2;
		}
		PRS_PRPosToMtrOffset(pPRU, pr_coord, &mtr_offset);

		if (
			(MoveAbsoluteXY(mtr_oldPosn.x, mtr_oldPosn.y, GMP_WAIT) != GMP_SUCCESS) || 
			SleepWithCalibDelay() != GMP_SUCCESS || 
			(MoveAbsoluteXY(mtr_oldPosn.x, mtr_oldPosn.y + mtr_offset.y, GMP_WAIT) != GMP_SUCCESS) 
		   )
		{
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		PRS_ClearScreen(pPRU->nCamera);

		if (!SearchPR(pPRU))
		{
			bResult = FALSE;
			break;
		}

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

		m_stPRCalibPts[CAL_DOWN].x		= pPRU->rcoDieCentre.x;
		m_stPRCalibPts[CAL_DOWN].y		= pPRU->rcoDieCentre.y;
		m_stMtrCalibPts[CAL_DOWN].x		= mtr_oldPosn.x;
		m_stMtrCalibPts[CAL_DOWN].y		= mtr_oldPosn.y + mtr_offset.y;

		// back to origin
		if (MoveAbsoluteXY(mtr_oldPosn.x, mtr_oldPosn.y, GMP_WAIT) != GMP_SUCCESS)
		{
			bResult = FALSE;
			break;
		}

		FILE *fp = fopen("D:\\sw\\AC9000\\Data\\AUTOCALIBRATION.txt", "a+");

		if (fp != NULL) //klocwork fix 20121211
		{
			for (INT i = 0; i < NO_OF_CAL_PT; i++)
			{
				szMsg.Format("%d:    PR(%ld, %ld), MTR(%ld, %ld)\n", i, m_stPRCalibPts[i].x, m_stPRCalibPts[i].y, m_stMtrCalibPts[i].x, m_stMtrCalibPts[i].y);
				fprintf(fp, szMsg);
			}

			fclose(fp);
		}

	} while (0);

	PRS_DisplayVideo(pPRU);

	// Release Me with ASAP
	if (bResult && FindAutoCalibMatrix(pPRU, m_stMtrCalibPts, m_stPRCalibPts))
	{

		return TRUE;
	}
	else
	{
		return FALSE;
	}
	//return FALSE; //klocwork fix 20121211
}

/////////////////////////////////////////////////////////////////
// COR Calibration
/////////////////////////////////////////////////////////////////
BOOL CPRTaskStn::CheckPRCentre(PRU *pPRU, BOOL bIsInvert) //20120613
{
	CString szMsg;
	MTR_POSN mtr_offset;

	PRS_DisplayVideo(pPRU);

	if (!m_bPRSelected)
	{
		szMsg.Format("PR Module Not Selected. Operation Abort!");
		m_pAppMod->HMIMessageBox(MSG_OK, "CHECK PR CENTRE OPERATION", szMsg);
		return 0;
	}

	Sleep(m_lCalibPRDelay);

	if (
		!SearchPRCentre(pPRU, bIsInvert) ||
		(MoveLighting(GMP_WAIT) != GMP_SUCCESS) //20150218
		) //20120613
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "PR SEARCH", "PR Failed!");
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "PR Failed!");
		return 0;
	}

	szMsg.Format("BEFORE MOVE: PR XY: [%ld,%ld]", pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y);
	m_pAppMod->WriteHMIMess(szMsg);

	PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

	Sleep(m_lCalibPRDelay);
	
	PRS_ClearScreen(pPRU->nCamera);

	if (!SearchPR(pPRU))
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "PR SEARCH", "PR Failed!");
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "PR Failed!");

		return 0;
	}

	PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

	szMsg.Format("AFTER MOVE: PR XY: [%.2f,%.2f]", pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y);
	m_pAppMod->WriteHMIMess(szMsg);

	Sleep(500);

	PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);

	szMsg.Format("Away From Centre (PR): [%.2f, %.2f] (POSN): [%ld,%ld]", 
		pPRU->rcoDieCentre.x - (float)PR_DEF_CENTRE_X, pPRU->rcoDieCentre.y -(float) PR_DEF_CENTRE_Y, 
		mtr_offset.x, mtr_offset.y);

	m_pAppMod->WriteHMIMess(szMsg);

	return 0;
}

BOOL CPRTaskStn::CORCalibration(PRU *pPRU, DOUBLE dInitalStepAngle, BOOL bUseRotaryEnc, BOOL bInvertAngle)
{
	INT i;
	CString szTitle;
	CString szMsg;

	szTitle.Format("COR CALIBRATION");


	if (!m_bPRSelected)
	{
		szMsg.Format("PR Module Not Selected. Operation Abort!") ;
		m_pAppMod->HMIMessageBox(MSG_OK, szTitle, szMsg);
		return FALSE;
	}
	
	m_pAppMod->WriteHMIMess("Starting COR Calibration...");
	PRS_DisplayVideo(pPRU);
	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);

	if (
		MoveZToPRCalibLevel() != GMP_SUCCESS ||
		MoveXYToPRCalibPosn() != GMP_SUCCESS ||
		MoveTToStandby() != GMP_SUCCESS 
	   )
	{
		m_pAppMod->WriteHMIMess("Calibration Abort!");
		return FALSE;
	}

	if (m_dCalibCORRange == 0.0)
	{
		m_pAppMod->HMIMessageBox(MSG_OK, szTitle, "Parameter Error! Calibration Range must not be 0");
		m_pAppMod->WriteHMIMess("Calibration Abort!");

		return FALSE;
	}

	for (i = 0; i < COR_ARRAY_SIZE; i++)
	{
		m_stCORMtrCalibPts[m_lCORCamNum][i].x = 0;
		m_stCORMtrCalibPts[m_lCORCamNum][i].y = 0;
	}

	// pull to centre first
	if (!SearchPRCentre(pPRU))
	{
		m_pAppMod->HMIMessageBox(MSG_OK, szTitle, "PR Search Error!");
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Calibration Failed!");
		m_pAppMod->WriteHMIMess("Calibration Abort!");

		return FALSE;
	}

	D_MTR_POSN dmtr_tempCOR = {0.0, 0.0};

	BOOL bResult = TRUE;
	INT nCounter = 0;
	LONG lStep = 0;

	// main frame 1: Find approximate table COR
	do
	{
		PRS_ClearScreen(pPRU->nCamera);

		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "COR Calibration: %ld/10", nCounter + 1);
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Pattern Angle: %.2f", pPRU->fAng);

		// get initial calibration point
		if (nCounter == 0)
		{
			m_stCORMtrCalibPts[m_lCORCamNum][nCounter].x = GetMotorX().GetEncPosn();
			m_stCORMtrCalibPts[m_lCORCamNum][nCounter].y = GetMotorY().GetEncPosn();
		}
		else
		{
			m_stCORMtrCalibPts[m_lCORCamNum][nCounter].x = m_stCORMtrCalibPts[m_lCORCamNum][nCounter - 1].x;
			m_stCORMtrCalibPts[m_lCORCamNum][nCounter].y = m_stCORMtrCalibPts[m_lCORCamNum][nCounter - 1].y;
		}

		// move theta
		lStep = AngleToCount(dInitalStepAngle/*0.2*//*0.5*/, bUseRotaryEnc ? GetMotorT().stAttrib.dDistPerCountAux : GetMotorT().stAttrib.dDistPerCount);

		if (
			(bUseRotaryEnc && (MoveRelativeT(lStep, GMP_WAIT) != GMP_SUCCESS)) ||
			(!bUseRotaryEnc && (GetMotorT().MoveRelative(lStep, GMP_WAIT) != GMP_SUCCESS))
		   )
		{
			m_pAppMod->DisplayMessage("COR move T fail");
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		// align pattern centre to cam centre after rotation
		if (!SearchPR(pPRU))
		{
			Sleep(m_lCalibPRDelay);

			// retry #1
			if (!SearchPR(pPRU))
			{
				Sleep(m_lCalibPRDelay);

				// retry #2
				if (!SearchPR(pPRU))
				{
					m_pAppMod->HMIMessageBox(MSG_OK, szTitle, "PR Search Error!");
					bResult = FALSE;
					break;
				}
			}
		}

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

		// update calibration data
		MTR_POSN mtrTempOffset;
		PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtrTempOffset);

		m_stCORMtrCalibPts[m_lCORCamNum][nCounter].x += mtrTempOffset.x;
		m_stCORMtrCalibPts[m_lCORCamNum][nCounter].y += mtrTempOffset.y;

		if (MoveAbsoluteXY(m_stCORMtrCalibPts[m_lCORCamNum][nCounter].x, m_stCORMtrCalibPts[m_lCORCamNum][nCounter].y, GMP_WAIT) != GMP_SUCCESS)
		{
			bResult = FALSE;
			break;
		}

		// do PR and check if needed to force table to centre
		do
		{
			Sleep(m_lCalibPRDelay);

			PRS_ClearScreen(pPRU->nCamera);

			if (!SearchPR(pPRU))
			{
				Sleep(m_lCalibPRDelay);

				// retry #1
				if (!SearchPR(pPRU))
				{
					Sleep(m_lCalibPRDelay);

					// retry #2
					if (!SearchPR(pPRU))
					{
						m_pAppMod->HMIMessageBox(MSG_OK, szTitle, "PR Search Error!");
						bResult = FALSE;
						break;
					}
				}
			}

			PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

			if (
				fabs(pPRU->rcoDieCentre.x - PR_DEF_CENTRE_X) > 1 || 
				fabs(pPRU->rcoDieCentre.y - PR_DEF_CENTRE_Y) > 1 
			   )
			{
				PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "fine tuning...(%.2f,%.2f)", 
								pPRU->rcoDieCentre.x - PR_DEF_CENTRE_X, 
								pPRU->rcoDieCentre.y - PR_DEF_CENTRE_Y);

				Sleep(m_lCalibPRDelay);

				// correction
				if (!SearchPR(pPRU))
				{
					Sleep(m_lCalibPRDelay);

					// retry #1
					if (!SearchPR(pPRU))
					{
						Sleep(m_lCalibPRDelay);

						// retry #2
						if (!SearchPR(pPRU))
						{
							m_pAppMod->HMIMessageBox(MSG_OK, szTitle, "PR Search Error!");
							bResult = FALSE;
							break;
						}
					}
				}

				PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

				// update calibration point
				MTR_POSN mtrTempOffset;
				PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtrTempOffset);

				m_stCORMtrCalibPts[m_lCORCamNum][nCounter].x += mtrTempOffset.x;
				m_stCORMtrCalibPts[m_lCORCamNum][nCounter].y += mtrTempOffset.y;

				if (MoveAbsoluteXY(m_stCORMtrCalibPts[m_lCORCamNum][nCounter].x, m_stCORMtrCalibPts[m_lCORCamNum][nCounter].y, GMP_WAIT) != GMP_SUCCESS)
				{
					bResult = FALSE;
					break;
				}
			}
			else
			{
				// result = ok
				break;
			}
		} while (1);

		nCounter++;

	} while (bResult && nCounter < COR_ARRAY_SIZE);

	if (!bResult || m_pAppMod->getKlocworkFalse()) //klocwork fix 20121211
	{
		m_pAppMod->WriteHMIMess("Calibration Abort!");
		return FALSE;
	}

	if (!FindCORCentre(nCounter, m_stCORMtrCalibPts[m_lCORCamNum], &dmtr_tempCOR))
	{
		m_pAppMod->HMIMessageBox(MSG_OK, szTitle, "Calibration Failed!");
		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "COR Calibration Failed!");

		m_pAppMod->WriteHMIMess("Unable to find COR Centre with data. Calibration Abort!");
		return FALSE;
	}
	// Done finding approximate table COR

	m_pAppMod->WriteHMIMess("Starting to fine-tuning COR...");
	PRS_DisplayVideo(pPRU);
	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);

	//if (
	//	MoveXYToPRCalibPosn() != GMP_SUCCESS ||
	//	MoveTToStandby() != GMP_SUCCESS 
	//	)
	//{
	//	m_pAppMod->WriteHMIMess("Calibration Abort!");
	//	return FALSE;
	//}

	MTR_POSN mtr_Temp = {0, 0};
	D_MTR_POSN dmtr_TempResult = {0.0, 0.0};
	D_MTR_POSN dmtr_TempOffset = {0.0, 0.0};
	DOUBLE dRefineAng = 0.0;

	D_MTR_POSN	dmtrTempPos[COR_ARRAY_SIZE];
	D_MTR_POSN	dmtrTempNeg[COR_ARRAY_SIZE];
	DOUBLE		dTempLUT[COR_ARRAY_SIZE];

	bResult = TRUE;
	nCounter = 0;
	lStep = 0;

	for (i = 0; i < COR_ARRAY_SIZE; i++)
	{
		dmtrTempPos[i].x = 0.0;
		dmtrTempPos[i].y = 0.0;
		dmtrTempNeg[i].x = 0.0;
		dmtrTempNeg[i].y = 0.0;

		dTempLUT[i] = (i + 1) * m_dCalibCORRange / (2.0 * 10.0);
	}

	// main frame 2: Fine-tunning table COR (neg array)
	do
	{
		PRS_ClearScreen(pPRU->nCamera);
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Fine-Tuning COR(neg): %ld/%ld", nCounter + 1, 2 * COR_ARRAY_SIZE);

		// get initial approximate COR
		if (nCounter == 0)
		{
			dmtrTempNeg[nCounter].x = dmtr_tempCOR.x;
			dmtrTempNeg[nCounter].y = dmtr_tempCOR.y;
		}
		else
		{
			dmtrTempNeg[nCounter].x = dmtrTempNeg[nCounter - 1].x;
			dmtrTempNeg[nCounter].y = dmtrTempNeg[nCounter - 1].y;
		}

		if (
			MoveXYToPRCalibPosn() != GMP_SUCCESS ||
			MoveTToStandby() != GMP_SUCCESS 
		   )
		{
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		if (!SearchPRCentre(pPRU))
		{
			m_pAppMod->HMIMessageBox(MSG_OK, szTitle, "PR Search Error!");
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Calibration Failed!");
			m_pAppMod->WriteHMIMess("Calibration Abort!");

			bResult = FALSE;
			break;
		}

		mtr_Temp.x = GetMotorX().GetEncPosn();
		mtr_Temp.y = GetMotorY().GetEncPosn();

		// move theta
		if (nCounter == 0)
		{
			dRefineAng = (-1.0) * (dTempLUT[0] / 2.0);
		}
		else
		{
			dRefineAng = (-1.0) * (dTempLUT[nCounter - 1] + (dTempLUT[0] / 2.0));
		}

		lStep = AngleToCount(dRefineAng, bUseRotaryEnc ? GetMotorT().stAttrib.dDistPerCountAux : GetMotorT().stAttrib.dDistPerCount);

		CalDPosnAfterRotate(dmtrTempNeg[nCounter], mtr_Temp, dRefineAng, &dmtr_TempResult);

		if (
			(MoveAbsoluteXY(DoubleToLong(dmtr_TempResult.x), DoubleToLong(dmtr_TempResult.y), GMP_WAIT) != GMP_SUCCESS) || 
			(bUseRotaryEnc && (MoveRelativeT(lStep, GMP_WAIT) != GMP_SUCCESS)) ||
			(!bUseRotaryEnc && (GetMotorT().MoveRelative(lStep, GMP_WAIT) != GMP_SUCCESS))
		   )
		{
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		// align pattern centre to cam centre after rotation
		if (!SearchPR(pPRU))
		{
			Sleep(m_lCalibPRDelay);

			// retry #1
			if (!SearchPR(pPRU))
			{
				Sleep(m_lCalibPRDelay);

				// retry #2
				if (!SearchPR(pPRU))
				{
					m_pAppMod->HMIMessageBox(MSG_OK, szTitle, "PR Search Error!");
					bResult = FALSE;
					break;
				}
			}
		}

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

		PRS_PRPosToDMtrOffset(pPRU, pPRU->rcoDieCentre, &dmtr_TempOffset);

		dmtr_TempResult.x += dmtr_TempOffset.x;
		dmtr_TempResult.y += dmtr_TempOffset.y;
#if 1 //20140702
		RefineCORCentre(mtr_Temp, bInvertAngle ? -dRefineAng : dRefineAng, dmtr_TempResult, &dmtrTempNeg[nCounter]);
#else
		RefineCORCentre(mtr_Temp, dRefineAng, dmtr_TempResult, &dmtrTempNeg[nCounter]);
#endif
		nCounter++;

	} while (bResult && nCounter < COR_ARRAY_SIZE);

	if (!bResult)
	{
		m_pAppMod->WriteHMIMess("Calibration Abort!");
		return FALSE;
	}

	nCounter = COR_ARRAY_SIZE;

	// main frame 3: Fine-tunning table COR (neg array) part 2
	do
	{
		PRS_ClearScreen(pPRU->nCamera);

		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Fine-Tuning COR(neg): %ld/%ld", nCounter + 1, 2 * COR_ARRAY_SIZE);

		// get approximate COR
		if (nCounter > COR_ARRAY_SIZE)
		{
			dmtrTempNeg[2 * COR_ARRAY_SIZE - nCounter - 1].x = dmtrTempNeg[2 * COR_ARRAY_SIZE - nCounter].x;
			dmtrTempNeg[2 * COR_ARRAY_SIZE - nCounter - 1].y = dmtrTempNeg[2 * COR_ARRAY_SIZE - nCounter].y;
		}

		if (
			MoveXYToPRCalibPosn() != GMP_SUCCESS ||
			MoveTToStandby() != GMP_SUCCESS 			
		   )
		{
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		if (!SearchPRCentre(pPRU))
		{
			m_pAppMod->HMIMessageBox(MSG_OK, szTitle, "PR Search Error!");
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Calibration Failed!");
			m_pAppMod->WriteHMIMess("Calibration Abort!");

			bResult = FALSE;
			break;
		}


		mtr_Temp.x = GetMotorX().GetEncPosn();
		mtr_Temp.y = GetMotorY().GetEncPosn();

		// move theta
		dRefineAng = (-1.0) * (dTempLUT[2 * COR_ARRAY_SIZE - nCounter - 1] - (dTempLUT[0] / 2.0));

		lStep = AngleToCount(dRefineAng, bUseRotaryEnc ? GetMotorT().stAttrib.dDistPerCountAux : GetMotorT().stAttrib.dDistPerCount);

		CalDPosnAfterRotate(dmtrTempNeg[2 * COR_ARRAY_SIZE - nCounter - 1], mtr_Temp, dRefineAng, &dmtr_TempResult);

		if (
			(MoveAbsoluteXY(DoubleToLong(dmtr_TempResult.x), DoubleToLong(dmtr_TempResult.y), GMP_WAIT) != GMP_SUCCESS) || 
			(bUseRotaryEnc && (MoveRelativeT(lStep, GMP_WAIT) != GMP_SUCCESS)) ||
			(!bUseRotaryEnc && (GetMotorT().MoveRelative(lStep, GMP_WAIT) != GMP_SUCCESS))
		   )
		{
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		// align pattern centre to cam centre after rotation
		if (!SearchPR(pPRU))
		{
			Sleep(m_lCalibPRDelay);

			// retry #1
			if (!SearchPR(pPRU))
			{
				Sleep(m_lCalibPRDelay);

				// retry #2
				if (!SearchPR(pPRU))
				{
					m_pAppMod->HMIMessageBox(MSG_OK, szTitle, "PR Search Error!");
					bResult = FALSE;
					break;
				}
			}
		}

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

		PRS_PRPosToDMtrOffset(pPRU, pPRU->rcoDieCentre, &dmtr_TempOffset);

		dmtr_TempResult.x += dmtr_TempOffset.x;
		dmtr_TempResult.y += dmtr_TempOffset.y;

#if 1 //20140702
		RefineCORCentre(mtr_Temp, bInvertAngle ? -dRefineAng : dRefineAng, dmtr_TempResult, &dmtrTempNeg[2 * COR_ARRAY_SIZE - nCounter - 1]);
#else
		RefineCORCentre(mtr_Temp, dRefineAng, dmtr_TempResult, &dmtrTempNeg[2 * COR_ARRAY_SIZE - nCounter - 1]);
#endif
		nCounter++;

	} while (bResult && nCounter < 2 * COR_ARRAY_SIZE);

	if (!bResult)
	{
		m_pAppMod->WriteHMIMess("Calibration Abort!");
		return FALSE;
	}

	nCounter = 0;

	// main frame 4: Fine-tunning table COR (pos array)
	do
	{
		PRS_ClearScreen(pPRU->nCamera);

		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Fine-Tuning COR(pos): %ld/%ld", nCounter + 1, 2 * COR_ARRAY_SIZE);

		// get initial approximate COR
		if (nCounter == 0)
		{
			dmtrTempPos[nCounter].x = dmtrTempNeg[nCounter].x;
			dmtrTempPos[nCounter].y = dmtrTempNeg[nCounter].y;
		}
		else
		{
			dmtrTempPos[nCounter].x = dmtrTempPos[nCounter - 1].x;
			dmtrTempPos[nCounter].y = dmtrTempPos[nCounter - 1].y;
		}

		if (
			MoveXYToPRCalibPosn() != GMP_SUCCESS ||
			MoveTToStandby() != GMP_SUCCESS ||
			m_pAppMod->getKlocworkFalse() //klocwork fix 20121211
		   )
		{
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		if (!SearchPRCentre(pPRU) || m_pAppMod->getKlocworkFalse()) //klocwork fix 20121211
		{
			m_pAppMod->HMIMessageBox(MSG_OK, szTitle, "PR Search Error!");
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Calibration Failed!");
			m_pAppMod->WriteHMIMess("Calibration Abort!");

			bResult = FALSE;
			break;
		}

		mtr_Temp.x = GetMotorX().GetEncPosn();
		mtr_Temp.y = GetMotorY().GetEncPosn();

		// move theta
		if (nCounter == 0)
		{
			dRefineAng = (1.0) * (dTempLUT[0] / 2.0);
		}
		else
		{
			dRefineAng = (1.0) * (dTempLUT[nCounter - 1] + (dTempLUT[0] / 2.0));
		}

		lStep = AngleToCount(dRefineAng, bUseRotaryEnc ? GetMotorT().stAttrib.dDistPerCountAux : GetMotorT().stAttrib.dDistPerCount);

		CalDPosnAfterRotate(dmtrTempPos[nCounter], mtr_Temp, dRefineAng, &dmtr_TempResult);

	
		if (
			(MoveAbsoluteXY(DoubleToLong(dmtr_TempResult.x), DoubleToLong(dmtr_TempResult.y), GMP_WAIT) != GMP_SUCCESS) || 
			(bUseRotaryEnc && (MoveRelativeT(lStep, GMP_WAIT) != GMP_SUCCESS)) ||
			(!bUseRotaryEnc && (GetMotorT().MoveRelative(lStep, GMP_WAIT) != GMP_SUCCESS))
			|| m_pAppMod->getKlocworkFalse() //klocwork fix 20121211
		   )
		{
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		// align pattern centre to cam centre after rotation
		if (!SearchPR(pPRU))
		{
			Sleep(m_lCalibPRDelay);

			// retry #1
			if (!SearchPR(pPRU))
			{
				Sleep(m_lCalibPRDelay);

				// retry #2
				if (!SearchPR(pPRU) || m_pAppMod->getKlocworkFalse()) //klocwork fix 20121211
				{
					m_pAppMod->HMIMessageBox(MSG_OK, szTitle, "PR Search Error!");
					bResult = FALSE;
					break;
				}
			}
		}

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

		PRS_PRPosToDMtrOffset(pPRU, pPRU->rcoDieCentre, &dmtr_TempOffset);

		dmtr_TempResult.x += dmtr_TempOffset.x;
		dmtr_TempResult.y += dmtr_TempOffset.y;

#if 1 //20140702
		RefineCORCentre(mtr_Temp, bInvertAngle ? -dRefineAng : dRefineAng, dmtr_TempResult, &dmtrTempPos[nCounter]);
#else
		RefineCORCentre(mtr_Temp, dRefineAng, dmtr_TempResult, &dmtrTempPos[nCounter]);
#endif
		nCounter++;

	} while (bResult && nCounter < COR_ARRAY_SIZE);

	if (!bResult || m_pAppMod->getKlocworkFalse()) //klocwork fix 20121211
	{
		m_pAppMod->WriteHMIMess("Calibration Abort!");
		return FALSE;
	}

	nCounter = COR_ARRAY_SIZE;

	// main frame 5: Fine-tunning table COR (pos array) part 2
	do
	{
		PRS_ClearScreen(pPRU->nCamera);

		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Fine-Tuning COR(pos): %ld/%ld", nCounter + 1, 2 * COR_ARRAY_SIZE);

		// get approximate COR
		if (nCounter > COR_ARRAY_SIZE)
		{
			dmtrTempPos[2 * COR_ARRAY_SIZE - nCounter - 1].x = dmtrTempPos[2 * COR_ARRAY_SIZE - nCounter].x;
			dmtrTempPos[2 * COR_ARRAY_SIZE - nCounter - 1].y = dmtrTempPos[2 * COR_ARRAY_SIZE - nCounter].y;
		}

		if (
			MoveXYToPRCalibPosn() != GMP_SUCCESS ||
			MoveTToStandby() != GMP_SUCCESS 
		   )
		{
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		if (!SearchPRCentre(pPRU))
		{
			m_pAppMod->HMIMessageBox(MSG_OK, szTitle, "PR Search Error!");
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Calibration Failed!");
			m_pAppMod->WriteHMIMess("Calibration Abort!");

			bResult = FALSE;
			break;
		}

		mtr_Temp.x = GetMotorX().GetEncPosn();
		mtr_Temp.y = GetMotorY().GetEncPosn();

		// move theta
		dRefineAng = (1.0) * (dTempLUT[2 * COR_ARRAY_SIZE - nCounter - 1] - (dTempLUT[0] / 2.0));

		lStep = AngleToCount(dRefineAng, bUseRotaryEnc ? GetMotorT().stAttrib.dDistPerCountAux : GetMotorT().stAttrib.dDistPerCount);

		CalDPosnAfterRotate(dmtrTempPos[2 * COR_ARRAY_SIZE - nCounter - 1], mtr_Temp, dRefineAng, &dmtr_TempResult);

		if (
			(MoveAbsoluteXY(DoubleToLong(dmtr_TempResult.x), DoubleToLong(dmtr_TempResult.y), GMP_WAIT) != GMP_SUCCESS) || 
			(bUseRotaryEnc && (MoveRelativeT(lStep, GMP_WAIT) != GMP_SUCCESS)) ||
			(!bUseRotaryEnc && (GetMotorT().MoveRelative(lStep, GMP_WAIT) != GMP_SUCCESS))
		   )
		{
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		// align pattern centre to cam centre after rotation
		if (!SearchPR(pPRU))
		{
			Sleep(m_lCalibPRDelay);

			// retry #1
			if (!SearchPR(pPRU))
			{
				Sleep(m_lCalibPRDelay);

				// retry #2
				if (!SearchPR(pPRU))
				{
					m_pAppMod->HMIMessageBox(MSG_OK, szTitle, "PR Search Error!");
					bResult = FALSE;
					break;
				}
			}
		}

		PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

		PRS_PRPosToDMtrOffset(pPRU, pPRU->rcoDieCentre, &dmtr_TempOffset);

		dmtr_TempResult.x += dmtr_TempOffset.x;
		dmtr_TempResult.y += dmtr_TempOffset.y;

#if 1 //20140702
		RefineCORCentre(mtr_Temp, bInvertAngle ? -dRefineAng : dRefineAng, dmtr_TempResult, &dmtrTempPos[2 * COR_ARRAY_SIZE - nCounter - 1]);
#else
		RefineCORCentre(mtr_Temp, dRefineAng, dmtr_TempResult, &dmtrTempPos[2 * COR_ARRAY_SIZE - nCounter - 1]);
#endif
		nCounter++;

	} while (bResult && nCounter < 2 * COR_ARRAY_SIZE);

	if (!bResult)
	{
		m_pAppMod->WriteHMIMess("Calibration Abort!");
		return FALSE;
	}

	for (i = 0; i < COR_ARRAY_SIZE; i++)
	{
		m_dmtrTableCOR_Pos[m_lCORCamNum][i].x = dmtrTempPos[i].x;
		m_dmtrTableCOR_Pos[m_lCORCamNum][i].y = dmtrTempPos[i].y;
		m_dmtrTableCOR_Neg[m_lCORCamNum][i].x = dmtrTempNeg[i].x;
		m_dmtrTableCOR_Neg[m_lCORCamNum][i].y = dmtrTempNeg[i].y;

		m_dCORLUT[m_lCORCamNum][i] = dTempLUT[i];
	}

	// Adjustment
	D_MTR_POSN dmtrTemp = {0.0, 0.0};
	D_MTR_POSN dmtrDiff = {0.0, 0.0};
	for (i = 1; i < COR_ARRAY_SIZE - 1; i++)
	{
		dmtrTemp.x = fabs(m_dmtrTableCOR_Pos[m_lCORCamNum][i + 1].x - m_dmtrTableCOR_Pos[m_lCORCamNum][i].x);
		if (dmtrTemp.x > dmtrDiff.x)
		{
			dmtrDiff.x = dmtrTemp.x;
		}
		dmtrTemp.y = fabs(m_dmtrTableCOR_Pos[m_lCORCamNum][i + 1].y - m_dmtrTableCOR_Pos[m_lCORCamNum][i].y);
		if (dmtrTemp.y > dmtrDiff.y)
		{
			dmtrDiff.y = dmtrTemp.y;
		}
	}
	dmtrTemp.x = fabs(m_dmtrTableCOR_Pos[m_lCORCamNum][1].x - m_dmtrTableCOR_Pos[m_lCORCamNum][0].x);
	dmtrTemp.y = fabs(m_dmtrTableCOR_Pos[m_lCORCamNum][1].y - m_dmtrTableCOR_Pos[m_lCORCamNum][0].y);
	if (dmtrTemp.x > dmtrDiff.x || dmtrTemp.y > dmtrDiff.y)
	{
		m_dmtrTableCOR_Pos[m_lCORCamNum][0].x = m_dmtrTableCOR_Pos[m_lCORCamNum][1].x;
		m_dmtrTableCOR_Pos[m_lCORCamNum][0].y = m_dmtrTableCOR_Pos[m_lCORCamNum][1].y;
	}

	dmtrTemp.x = 0.0;
	dmtrTemp.y = 0.0;
	dmtrDiff.x = 0.0;
	dmtrDiff.y = 0.0;
	for (i = 1; i < COR_ARRAY_SIZE - 1; i++)
	{
		dmtrTemp.x = fabs(m_dmtrTableCOR_Neg[m_lCORCamNum][i + 1].x - m_dmtrTableCOR_Neg[m_lCORCamNum][i].x);
		if (dmtrTemp.x > dmtrDiff.x)
		{
			dmtrDiff.x = dmtrTemp.x;
		}
		dmtrTemp.y = fabs(m_dmtrTableCOR_Neg[m_lCORCamNum][i + 1].y - m_dmtrTableCOR_Neg[m_lCORCamNum][i].y);
		if (dmtrTemp.y > dmtrDiff.y)
		{
			dmtrDiff.y = dmtrTemp.y;
		}
	}
	dmtrTemp.x = fabs(m_dmtrTableCOR_Neg[m_lCORCamNum][1].x - m_dmtrTableCOR_Neg[m_lCORCamNum][0].x);
	dmtrTemp.y = fabs(m_dmtrTableCOR_Neg[m_lCORCamNum][1].y - m_dmtrTableCOR_Neg[m_lCORCamNum][0].y);
	if (dmtrTemp.x > dmtrDiff.x || dmtrTemp.y > dmtrDiff.y)
	{
		m_dmtrTableCOR_Neg[m_lCORCamNum][0].x = m_dmtrTableCOR_Neg[m_lCORCamNum][1].x;
		m_dmtrTableCOR_Neg[m_lCORCamNum][0].y = m_dmtrTableCOR_Neg[m_lCORCamNum][1].y;
	}

	PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "COR Calibration Succeed!");

	m_bCORCalibrated[m_lCORCamNum] = TRUE;

	MoveXYToPRCalibPosn();
	MoveTToStandby();

	return TRUE;
}

BOOL CPRTaskStn::FindCORResultRange(DOUBLE *pdMaxX, DOUBLE *pdMinX, DOUBLE *pdMaxY, DOUBLE *pdMinY)
{
	INT i;
	DOUBLE dNegXMax = 0.0, dNegXMin = 0.0, dNegYMax = 0.0, dNegYMin = 0.0;
	DOUBLE dXMax = 0.0, dXMin = 0.0;
	DOUBLE dYMax = 0.0, dYMin = 0.0;

	dNegXMax = m_dmtrTableCOR_Neg[m_lCORCamNum][0].x;
	dNegXMin = m_dmtrTableCOR_Neg[m_lCORCamNum][0].x;

	dNegYMax = m_dmtrTableCOR_Neg[m_lCORCamNum][0].y;
	dNegYMin = m_dmtrTableCOR_Neg[m_lCORCamNum][0].y;

	for (i = 1; i < COR_ARRAY_SIZE; i++)
	{
		if (m_dmtrTableCOR_Neg[m_lCORCamNum][i].x > dNegXMax)
		{
			dNegXMax = m_dmtrTableCOR_Neg[m_lCORCamNum][i].x;
		}
		if (m_dmtrTableCOR_Neg[m_lCORCamNum][i].x < dNegXMin)
		{
			dNegXMin = m_dmtrTableCOR_Neg[m_lCORCamNum][i].x;
		}

		if (m_dmtrTableCOR_Neg[m_lCORCamNum][i].y > dNegYMax)
		{
			dNegYMax = m_dmtrTableCOR_Neg[m_lCORCamNum][i].y;
		}
		if (m_dmtrTableCOR_Neg[m_lCORCamNum][i].y < dNegYMin)
		{
			dNegYMin = m_dmtrTableCOR_Neg[m_lCORCamNum][i].y;
		}
	}
	
	//Positive
	dXMax = dNegXMax;
	dXMin = dNegXMin;
	dYMax = dNegYMax;
	dYMin = dNegYMin;
	for (i = 0; i < COR_ARRAY_SIZE; i++)
	{
		if (m_dmtrTableCOR_Pos[m_lCORCamNum][i].x > dXMax)
		{
			dXMax = m_dmtrTableCOR_Pos[m_lCORCamNum][i].x;
		}
		if (m_dmtrTableCOR_Pos[m_lCORCamNum][i].x < dXMin)
		{
			dXMin = m_dmtrTableCOR_Pos[m_lCORCamNum][i].x;
		}

		if (m_dmtrTableCOR_Pos[m_lCORCamNum][i].y > dYMax)
		{
			dYMax = m_dmtrTableCOR_Pos[m_lCORCamNum][i].y;
		}
		if (m_dmtrTableCOR_Pos[m_lCORCamNum][i].y < dYMin)
		{
			dYMin = m_dmtrTableCOR_Pos[m_lCORCamNum][i].y;
		}
	}
	*pdMaxX = dXMax;
	*pdMinX = dXMin;
	*pdMaxY = dYMax;
	*pdMinY = dYMin;

	return TRUE;
}

BOOL CPRTaskStn::SetAllCORData(DOUBLE dXPosn, DOUBLE dYPosn)
{
	INT ii;

	for (ii = 0; ii < COR_ARRAY_SIZE; ii++)
	{
		m_dmtrTableCOR_Neg[m_lCORCamNum][ii].x = dXPosn;
		m_dmtrTableCOR_Pos[m_lCORCamNum][ii].x = dXPosn;

		m_dmtrTableCOR_Neg[m_lCORCamNum][ii].y = dYPosn;
		m_dmtrTableCOR_Pos[m_lCORCamNum][ii].y = dYPosn;
	}
	m_bCORCalibrated[m_lCORCamNum] = TRUE;

	return TRUE;
}

BOOL CPRTaskStn::CheckCORCentre(PRU *pPRU, DOUBLE dTestAng, BOOL bUseRotaryEnc, BOOL bInvertAngle, PRINT_DATA	*stPrintData)
{

	CString szMsg = "";
	MTR_POSN mtrCurPosn;
	MTR_POSN mtrResultPosn;
	MTR_POSN mtrOffset;

	CString str;

	PRS_DisplayVideo(pPRU);

	if (!m_bCORCalibrated)
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "CHECK COR CENTRE OPERATION", "Have not been Calibrated, Please calib first and retry");
		return FALSE;
	}

	//Move to Calib Posn 
	//Move to the Calib Posn with T is standby Posn
	if (
		MoveTToStandby() != GMP_SUCCESS ||
		MoveXYToPRCalibPosn() != GMP_SUCCESS ||
		MoveLighting(GMP_WAIT) != GMP_SUCCESS || //20150218
		MoveZToPRCalibLevel() != GMP_SUCCESS 
	   )
	{
		return FALSE;
	}
	
	Sleep(m_lCalibPRDelay);

	if (!SearchPRCentre(pPRU))
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "CHECK COR CENTRE OPERATION", "COR Test Failed!");
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "PR Failed!");
		m_pAppMod->WriteHMIMess("PreBond PR Failed!");
		return 0;
	}

	if (MoveLighting(GMP_WAIT) != GMP_SUCCESS) //20150218
	{
		return FALSE;
	}

	PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

	mtrCurPosn.x = GetMotorX().GetEncPosn();
	mtrCurPosn.y = GetMotorY().GetEncPosn();

	if (stPrintData != NULL)
	{
		stPrintData->dData[0] = (DOUBLE) mtrCurPosn.x; //"Before Rotate PR1 X(mtr)"
		stPrintData->dData[1] = (DOUBLE) mtrCurPosn.y; //"Before Rotate PR1 Y(mtr)"
		stPrintData->dData[2] = (DOUBLE) GetMotorT().GetEncPosn(); //"Before Rotate PR1 T(mtr)"
	}

	if (!SearchPR(pPRU))
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "CHECK COR CENTRE OPERATION", "COR Test Failed!");
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "PR Failed!");
		m_pAppMod->WriteHMIMess("PreBond PR Failed!");
		return 0;
	}

	if (pPRU->bIs5MBCam && pPRU->bUseFloatingPointResult) //20150728 float PR
	{
		if (stPrintData != NULL)
		{
			stPrintData->dData[3] = (DOUBLE) pPRU->rcoDieCentre.x; //"After Rotate Pixel X"
			stPrintData->dData[4] = (DOUBLE) pPRU->rcoDieCentre.y; //"After Rotate Pixel Y"
		}
	}
	else
	{
		if (stPrintData != NULL)
		{
			stPrintData->dData[3] = (DOUBLE) pPRU->rcoDieCentre.x; //"Before Rotate Pixel X"
			stPrintData->dData[4] = (DOUBLE) pPRU->rcoDieCentre.y; //"Before Rotate Pixel Y"
		}
	}

	LONG lTestAngStep = AngleToCount(dTestAng, bUseRotaryEnc ? GetMotorT().stAttrib.dDistPerCountAux : GetMotorT().stAttrib.dDistPerCount);
	if (
		(bUseRotaryEnc && (MoveRelativeT(lTestAngStep, GMP_WAIT) != GMP_SUCCESS)) ||
		(!bUseRotaryEnc && (GetMotorT().MoveRelative(lTestAngStep, GMP_WAIT) != GMP_SUCCESS))
	   )
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "CHECK COR CENTRE OPERATION", "COR Test Failed!");
		return 0;
	}

	CalPosnAfterRotate(mtrCurPosn, bInvertAngle ? -dTestAng : dTestAng, &mtrResultPosn);

	mtrOffset.x = mtrResultPosn.x - mtrCurPosn.x;
	mtrOffset.y = mtrResultPosn.y - mtrCurPosn.y;

	MoveRelativeXY(mtrOffset.x, mtrOffset.y, GMP_WAIT);
	MoveLighting(GMP_WAIT); //20150218
	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Offset (%ld,%ld)", mtrOffset.x, mtrOffset.y);

	Sleep(m_lCalibPRDelay);

	if (stPrintData != NULL)
	{
		stPrintData->dData[5] = (DOUBLE) GetMotorX().GetEncPosn(); //"After Rotate PR1 X(mtr)"
		stPrintData->dData[6] = (DOUBLE) GetMotorY().GetEncPosn(); //"After Rotate PR1 Y(mtr)"
		stPrintData->dData[7] = (DOUBLE) GetMotorT().GetEncPosn();  //"After Rotate PR1 T(mtr)"
	}

	if (!SearchPR(pPRU))
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "CHECK COR CENTRE OPERATION", "COR Test Failed!");
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "PR Failed!");
		m_pAppMod->WriteHMIMess("PreBond PR Failed!");
		return 0;
	}

	PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

	szMsg.Format("PR XY: [%.2f,%.2f]", pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y);
	m_pAppMod->WriteHMIMess(szMsg);

	PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtrOffset);

	szMsg.Format("Away From Centre (PR): [%.2f, %.2f] (POSN): [%ld,%ld]", 
				(DOUBLE)pPRU->rcoDieCentre.x - (DOUBLE)PR_DEF_CENTRE_X, (DOUBLE)pPRU->rcoDieCentre.y - (DOUBLE)PR_DEF_CENTRE_Y, 
				mtrOffset.x, mtrOffset.y);
	m_pAppMod->WriteHMIMess(szMsg);

	if (stPrintData != NULL)
	{
		stPrintData->dData[8] = (DOUBLE) pPRU->rcoDieCentre.x; //"After Rotate Pixel X"
		stPrintData->dData[9] = (DOUBLE) pPRU->rcoDieCentre.y; //"After Rotate Pixel Y"
	}

	if (m_bPRCycle)
	{
		m_ulPRCycle++;
		szMsg.Format("Cycle: %ld", m_ulPRCycle);
		PRS_DisplayText(pPRU->nCamera, 2, 0, szMsg);
	}

	return TRUE;
}
/*BOOL CPRTaskStn::GetPRCentreMap(PRU *pPRU)
{
	BOOL bResult = TRUE;
	LONG lDiection = 0;		// 0 - X, 1 - Y

	MTR_POSN mtrPosn = {0, 0};
	INT i = 0;
	CString szMsg = "";

	LONG lSelection = m_pAppMod->HMISelectionBox("OPTION", "Please select move direction", "X", "Y");


	if (lSelection == -1 || lSelection == 11)
	{
		bResult = FALSE;
	}
	else
	{
		FILE *fp = fopen("D:\\sw\\AC9000\\Data\\GetPRCentreMap.txt", "a+");
		if (lSelection == 0)
		{
			mtrPosn.x = 10;
		}
		else
		{
			mtrPosn.y = 10;
		}

		while (1)
		{
			if (!SearchPR(pPRU))
			{
				break;
			}

			Sleep(m_lCalibPRDelay);

			if (MoveRelativeXY(mtrPosn.x, mtrPosn.y, GMP_WAIT) != gnAMS_OK)
			{
				break;
			}

			m_pAppMod->ProcessHMIRequest();

			szMsg.Format("Point%d\tENC\t%ld\t%ld\tPR\t%ld\t%ld\n", i, GetMotorX().GetEncPosn(), GetMotorY().GetEncPosn(), pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y);
			fprintf(fp, szMsg);

			i++;
		}
		fclose(fp);
	}



	return bResult;
}*/

BOOL CPRTaskStn::GetPRCentreMap(PRU *pPRU)
{
	BOOL bResult = TRUE;
	LONG lDiection = 0;		// 0 - X, 1 - Y
	LONG lNumOfCntX = 0;
	LONG lNumOfCntY = 0;

	MTR_POSN mtrPosn = {0, 0};
	INT i = 0;
	CString szMsg = "";

	MTR_POSN mtrStartPosn;
	MTR_POSN mtrEndPosn;
	mtrStartPosn.x	= GetMotorX().GetEncPosn();
	mtrStartPosn.y	= GetMotorY().GetEncPosn();
	mtrEndPosn.x	= GetMotorX().GetEncPosn();
	mtrEndPosn.y	= GetMotorY().GetEncPosn();

	//LONG lSelection = m_pAppMod->HMISelectionBox("OPTION", "Please select move direction", "X", "Y", "X&Y");

	//if (lSelection == -1 || lSelection == 11)
	//{
	//	bResult = FALSE;
	//}
	//else
	//{
	FILE *fp = fopen("D:\\sw\\AC9000\\Data\\GetPRCentreMap.txt", "a+");

		//if (lSelection == 0)
		//{
		//	mtrPosn.x = 10;
		//}
		//else if (lSelection == 1)
		//{
		//	mtrPosn.y = 10;
		//}
		//else if (lSelection == 2)
		//{
	if (
		!m_pAppMod->HMINumericKeys("Set X Limit(cnt)", 9999999.0, 0, &lNumOfCntX) ||
		!m_pAppMod->HMINumericKeys("Set Y Limit(cnt)", 9999999.0, 0, &lNumOfCntY)
	   )
	{
		bResult = FALSE;
	}
	else
	{
		mtrEndPosn.x = mtrStartPosn.x + lNumOfCntX;
		mtrEndPosn.y = mtrStartPosn.y + lNumOfCntY;
		mtrPosn.x = 10;
		mtrPosn.y = 10;
	}
		//}

	while (bResult)
	{
		if (fp != NULL) //klocwork fix 20121211
		{
			fprintf(fp, "START\n");
		}
		if (
			GetMotorX().MoveAbsolute(mtrStartPosn.x, GMP_WAIT) != GMP_SUCCESS ||
			GetMotorY().MoveAbsolute(mtrStartPosn.y, GMP_WAIT) != GMP_SUCCESS
		   )
		{
			bResult = FALSE;
			break;
		}

		Sleep(m_lCalibPRDelay);

		while (GetMotorY().GetEncPosn() <= mtrEndPosn.y)
		{
			while (GetMotorX().GetEncPosn() <= mtrEndPosn.x)
			{
				if (!SearchPR(pPRU))
				{
					bResult = FALSE;
					break;
				}
				else
				{
					Sleep(m_lCalibPRDelay);

					if (GetMotorX().MoveRelative(mtrPosn.x, GMP_WAIT) != GMP_SUCCESS)
					{
						bResult = FALSE;
						break;
					}
				}

				szMsg.Format("\tPoint%d\tENC\t%ld\t%ld\tPR\t%.2f\t%.2f\n", i, GetMotorX().GetEncPosn(), GetMotorY().GetEncPosn(), pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y);

				if (fp != NULL) //klocwork fix 20121211
				{
					fprintf(fp, szMsg);
				}
				i++;
					
				m_pAppMod->ProcessHMIRequest();
			}

			if (
				!bResult ||
				GetMotorX().MoveAbsolute(mtrStartPosn.x, GMP_WAIT) != GMP_SUCCESS ||
				GetMotorY().MoveRelative(mtrPosn.y, GMP_WAIT) != GMP_SUCCESS
			   )
			{
				bResult = FALSE;
				break;
			}

			Sleep(m_lCalibPRDelay);
		}
	}
	if (fp != NULL) //klocwork fix 20121211
	{
		fclose(fp);
	}
	//}

	return bResult;
}

/////////////////////////////////////////////////////////////////
//PR Related
/////////////////////////////////////////////////////////////////
BOOL CPRTaskStn::AutoSearchPR1(PRU *pPRU, PRMultiLevel emPRMultiLevel, CString *pszStr, BOOL bMoveLighting)
{
	MTR_POSN mtr_offset;

	if (
		(MoveXYToCurrPR1Posn(bMoveLighting ? GMP_WAIT : GMP_NOWAIT)) != GMP_SUCCESS || 
		(bMoveLighting && (MoveLighting(GMP_WAIT) != GMP_SUCCESS)) ||//20150218
		(MoveZToPRLevel(GMP_NOWAIT, emPRMultiLevel)) != GMP_SUCCESS || 
		(SyncX()) != GMP_SUCCESS || 
		(SyncY()) != GMP_SUCCESS || 
		(SyncZ()) != GMP_SUCCESS
		)
	{
		return FALSE;
	}

	if (!m_bPRSelected)
	{
		if (pPRU->szStnName == "MB")
		{
			Sleep(m_lPRDelay2);
		}
		else
		{
			Sleep(m_lPRDelay);
		}
		return TRUE;
	}

	if (!SearchPR(pPRU))
	{
		return FALSE;
	}

	//Test

	if (pPRU->bIs5MBCam && pPRU->bUseFloatingPointResult) //20150728 float PR
	{
		PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);
	}
	else
	{
		PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);
	}
#if 1 //20140414 debug only
	if (pszStr != NULL)
	{
		pszStr->Format("CurrPR1Posn: %.5f %.5f mtr_offset=%ld %ld", m_dmtrCurrPR1Posn.x, m_dmtrCurrPR1Posn.y, mtr_offset.x, mtr_offset.y);
	}
#endif
	m_dmtrCurrPR1PosnOffset.x = (DOUBLE)mtr_offset.x;		//20150119
	m_dmtrCurrPR1PosnOffset.y = (DOUBLE)mtr_offset.y;		//20150119
	m_dmtrCurrPR1Posn.x += (DOUBLE)mtr_offset.x;
	m_dmtrCurrPR1Posn.y += (DOUBLE)mtr_offset.y;

	return TRUE;
}

BOOL CPRTaskStn::AutoSearchPR1GrabOnly(PRU *pPRU, CAC9000Stn *pStation)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();
	PRDebugInfo stPRDebugInfo; //20150423

	m_nPRInProgress = ST_PR1_IN_PROGRESS;

	if (!m_bPRSelected)
	{
		return TRUE;
	}

	if  (g_bPRDebugInfo)  //20150423
	{
		stPRDebugInfo.dwCmdStartTime = GetTickCount();
	}

	if (!SearchPRGrabOnly(pPRU))
	{
		m_nPRInProgress = ST_PR_IDLE;
		return FALSE;
	}

	if  (g_bPRDebugInfo)  //20150423
	{
		stPRDebugInfo.dwOverallTime = GetTickCount();
		CString szMsg = "";
		if (pStation != NULL)
		{
			szMsg.Format("Stn:%s PRU:%s PR1 Grab Time: %.5f sec", pStation->GetStnName(), pPRU->szStnName, (stPRDebugInfo.dwOverallTime - stPRDebugInfo.dwCmdStartTime) / 1000.0);
		}
		else
		{
			szMsg.Format("PRU:%s PR1 Grab Time: %.5f sec", pPRU->szStnName, (stPRDebugInfo.dwOverallTime - stPRDebugInfo.dwCmdStartTime) / 1000.0);
		}
		theApp.DisplayMessage(szMsg);
	}

	return TRUE;
}

#if 1 //20140414
BOOL CPRTaskStn::AutoSearchPR1ProcessOnly(PRU *pPRU, CString *pszMsgOut, CAC9000Stn *pStation)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();
	PRDebugInfo stPRDebugInfo; //20150423

	MTR_POSN mtr_offset;

	if (!m_bPRSelected)
	{
		m_nPRInProgress = ST_PR_IDLE;
		return TRUE;
	}

	if  (g_bPRDebugInfo)  //20150423
	{
		stPRDebugInfo.dwCmdStartTime = GetTickCount();
	}

	if (!SearchPRProcessOnly(pPRU))
	{
		m_nPRInProgress = ST_PR_IDLE;
		return FALSE;
	}

	PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);

	if  (g_bPRDebugInfo)  //20150423
	{
		stPRDebugInfo.dwOverallTime = GetTickCount();
		CString szMsg = "";
		if (pStation != NULL)
		{
			szMsg.Format("Stn:%s PRU:%s PR1 Process Time: %.5f sec", pStation->GetStnName(), pPRU->szStnName, (stPRDebugInfo.dwOverallTime - stPRDebugInfo.dwCmdStartTime) / 1000.0);
		}
		else
		{
			szMsg.Format("PRU:%s PR1 Process Time: %.5f sec", pPRU->szStnName, (stPRDebugInfo.dwOverallTime - stPRDebugInfo.dwCmdStartTime) / 1000.0);
		}
		theApp.DisplayMessage(szMsg);
	}

	if (pszMsgOut != NULL)
	{
		pszMsgOut->Format("PR1Posn=%.3f %.3f offset=%ld %ld", m_dmtrCurrPR1Posn.x, m_dmtrCurrPR1Posn.y, mtr_offset.x, mtr_offset.y);
	}
	m_dmtrCurrPR1PosnOffset.x = (DOUBLE)mtr_offset.x;		//20150119
	m_dmtrCurrPR1PosnOffset.y = (DOUBLE)mtr_offset.y;		//20150119
	m_dmtrCurrPR1Posn.x += (DOUBLE)mtr_offset.x;
	m_dmtrCurrPR1Posn.y += (DOUBLE)mtr_offset.y;

	m_nPRInProgress = ST_PR_IDLE;

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("%s PR1 Process Time: %.5f", GetStnName(), dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}
#else
BOOL CPRTaskStn::AutoSearchPR1ProcessOnly(PRU *pPRU)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();

	MTR_POSN mtr_offset;

	if (!m_bPRSelected)
	{
		m_nPRInProgress = ST_PR_IDLE;
		return TRUE;
	}

	if (!SearchPRProcessOnly(pPRU))
	{
		m_nPRInProgress = ST_PR_IDLE;
		return FALSE;
	}

	if (pPRU->bIs5MBCam && pPRU->bUseFloatingPointResult) //20150728 float PR
	{
		PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);
	}
	else
	{
		PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);
	}

	m_dmtrCurrPR1Posn.x += (DOUBLE)mtr_offset.x;
	m_dmtrCurrPR1Posn.y += (DOUBLE)mtr_offset.y;

	m_nPRInProgress = ST_PR_IDLE;

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("%s PR1 Process Time: %.5f", GetStnName(), dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}
#endif

BOOL CPRTaskStn::AutoSearchPR2(PRU *pPRU, PRMultiLevel emPRMultiLevel, CString *pszStr, BOOL bMoveLighting)
{
	MTR_POSN mtr_offset;

	if (
		(MoveXYToCurrPR2Posn(bMoveLighting ? GMP_WAIT : GMP_NOWAIT)) != GMP_SUCCESS || 
		(bMoveLighting && (MoveLighting(GMP_WAIT) != GMP_SUCCESS)) ||//20150218
		(MoveZToPRLevel(GMP_NOWAIT, emPRMultiLevel)) != GMP_SUCCESS || 
		(SyncX()) != GMP_SUCCESS || 
		(SyncY()) != GMP_SUCCESS || 
		(SyncZ()) != GMP_SUCCESS
		)
	{
		return FALSE;
	}

	if (!m_bPRSelected)
	{
		if (pPRU->szStnName == "MB")
		{
			Sleep(m_lPRDelay2);
		}
		else
		{
			Sleep(m_lPRDelay);
		}
		return TRUE;
	}

	if (!SearchPR(pPRU))
	{
		return FALSE;
	}

	if (pPRU->bIs5MBCam && pPRU->bUseFloatingPointResult) //20150728 float PR
	{
		PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);
	}
	else
	{
		PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);
	}
#if 1 //20140414 debug only
	if (pszStr != NULL)
	{
		pszStr->Format("CurrPR2Posn: %.5f %.5f mtr_offset=%ld %ld", m_dmtrCurrPR2Posn.x, m_dmtrCurrPR2Posn.y, mtr_offset.x, mtr_offset.y);
	}
#endif

	m_dmtrCurrPR2PosnOffset.x = (DOUBLE)mtr_offset.x;		//20150119
	m_dmtrCurrPR2PosnOffset.y = (DOUBLE)mtr_offset.y;		//20150119
	m_dmtrCurrPR2Posn.x += (DOUBLE)mtr_offset.x;
	m_dmtrCurrPR2Posn.y += (DOUBLE)mtr_offset.y;

	return TRUE;
}

BOOL CPRTaskStn::AutoSearchPR2GrabOnly(PRU *pPRU, CAC9000Stn *pStation)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();
	PRDebugInfo stPRDebugInfo; //20150423

	m_nPRInProgress = ST_PR2_IN_PROGRESS;

	if (!m_bPRSelected)
	{
		return TRUE;
	}

	if  (g_bPRDebugInfo)  //20150423
	{
		stPRDebugInfo.dwCmdStartTime = GetTickCount();
	}

	if (!SearchPRGrabOnly(pPRU))
	{
		m_nPRInProgress = ST_PR_IDLE;
		return FALSE;
	}

	if  (g_bPRDebugInfo)  //20150423
	{
		stPRDebugInfo.dwOverallTime = GetTickCount(); //20150423
		CString szMsg = "";
		if (pStation != NULL)
		{
			szMsg.Format("Stn:%s PRU:%s PR2 Grab Time: %.5f sec", pStation->GetStnName(), pPRU->szStnName, (stPRDebugInfo.dwOverallTime - stPRDebugInfo.dwCmdStartTime) / 1000.0);
		}
		else
		{
			szMsg.Format("PRU:%s PR2 Grab Time: %.5f sec", pPRU->szStnName, (stPRDebugInfo.dwOverallTime - stPRDebugInfo.dwCmdStartTime) / 1000.0);
		}
		theApp.DisplayMessage(szMsg);
	}

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("%s PR2 Grab Time: %.5f", GetStnName(), dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}

#if 1 //20140414
BOOL CPRTaskStn::AutoSearchPR2ProcessOnly(PRU *pPRU, CString *pszMsgOut, CAC9000Stn *pStation)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();
	PRDebugInfo stPRDebugInfo; //20150423

	MTR_POSN mtr_offset;

	if (!m_bPRSelected)
	{
		m_nPRInProgress = ST_PR_IDLE;
		return TRUE;
	}

	if  (g_bPRDebugInfo)  //20150423
	{
		stPRDebugInfo.dwCmdStartTime = GetTickCount();
	}

	if (!SearchPRProcessOnly(pPRU))
	{
		m_nPRInProgress = ST_PR_IDLE;
		return FALSE;
	}

	PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);

	if  (g_bPRDebugInfo)  //20150423
	{
		stPRDebugInfo.dwOverallTime = GetTickCount();
		CString szMsg = "";
		if (pStation != NULL)
		{
			szMsg.Format("Stn:%s PRU:%s PR2 Process Time: %.5f sec", pStation->GetStnName(), pPRU->szStnName, (stPRDebugInfo.dwOverallTime - stPRDebugInfo.dwCmdStartTime) / 1000.0);
		}
		else
		{
			szMsg.Format("PRU:%s PR2 Process Time: %.5f sec", pPRU->szStnName, (stPRDebugInfo.dwOverallTime - stPRDebugInfo.dwCmdStartTime) / 1000.0);
		}
		theApp.DisplayMessage(szMsg);
	}

	if (pszMsgOut != NULL)
	{
		pszMsgOut->Format("PR2Posn=%.3f %.3f offset=%ld %ld", m_dmtrCurrPR2Posn.x, m_dmtrCurrPR2Posn.y, mtr_offset.x, mtr_offset.y);
	}
	
	m_dmtrCurrPR2PosnOffset.x = (DOUBLE)mtr_offset.x;		//20150119
	m_dmtrCurrPR2PosnOffset.y = (DOUBLE)mtr_offset.y;		//20150119
	m_dmtrCurrPR2Posn.x += (DOUBLE)mtr_offset.x;
	m_dmtrCurrPR2Posn.y += (DOUBLE)mtr_offset.y;

	m_nPRInProgress = ST_PR_IDLE;

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("%s PR2 Process Time: %.5f", GetStnName(), dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}
#else
BOOL CPRTaskStn::AutoSearchPR2ProcessOnly(PRU *pPRU)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();

	MTR_POSN mtr_offset;

	if (!m_bPRSelected)
	{
		m_nPRInProgress = ST_PR_IDLE;
		return TRUE;
	}

	if (!SearchPRProcessOnly(pPRU))
	{
		m_nPRInProgress = ST_PR_IDLE;
		return FALSE;
	}

	if (pPRU->bIs5MBCam && pPRU->bUseFloatingPointResult) //20150728 float PR
	{
		PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);
	}
	else
	{
		PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);
	}

	m_dmtrCurrPR2Posn.x += (DOUBLE)mtr_offset.x;
	m_dmtrCurrPR2Posn.y += (DOUBLE)mtr_offset.y;

	m_nPRInProgress = ST_PR_IDLE;

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("%s PR2 Process Time: %.5f", GetStnName(), dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}
#endif

// PostBond
BOOL CPRTaskStn::AutoSearchPBPR1(PRU *pPRU)
{
	if (!m_bPRSelected)
	{
		if (pPRU->szStnName == "MB")
		{
			Sleep(m_lPRDelay2);
		}
		else
		{
			Sleep(m_lPRDelay);
		}
		return TRUE;
	}

	if (!SearchPR(pPRU))
	{
		return FALSE;
	}
	
	m_PRCurrPBPR1Corner[PR_UPPER_LEFT].x	= pPRU->arcoDieCorners[PR_UPPER_LEFT].x;
	m_PRCurrPBPR1Corner[PR_UPPER_LEFT].y	= pPRU->arcoDieCorners[PR_UPPER_LEFT].y;
	m_PRCurrPBPR1Corner[PR_LOWER_LEFT].x	= pPRU->arcoDieCorners[PR_LOWER_LEFT].x;
	m_PRCurrPBPR1Corner[PR_LOWER_LEFT].y	= pPRU->arcoDieCorners[PR_LOWER_LEFT].y;
	m_PRCurrPBPR1Corner[PR_UPPER_RIGHT].x	= pPRU->arcoDieCorners[PR_UPPER_RIGHT].x;
	m_PRCurrPBPR1Corner[PR_UPPER_RIGHT].y	= pPRU->arcoDieCorners[PR_UPPER_RIGHT].y;
	m_PRCurrPBPR1Corner[PR_LOWER_RIGHT].x	= pPRU->arcoDieCorners[PR_LOWER_RIGHT].x;
	m_PRCurrPBPR1Corner[PR_LOWER_RIGHT].y	= pPRU->arcoDieCorners[PR_LOWER_RIGHT].y;

	return TRUE;
}

BOOL CPRTaskStn::AutoSearchPBPR1GrabOnly(PRU *pPRU)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();

	m_nPRInProgress = ST_PB_PR1_IN_PROGRESS;

	if (!m_bPRSelected)
	{
		return TRUE;
	}

	if (!SearchPRGrabOnly(pPRU))
	{
		m_nPRInProgress = ST_PR_IDLE;
		return FALSE;
	}

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("%s PR1 Grab Time: %.5f", GetStnName(), dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}

BOOL CPRTaskStn::AutoSearchPBPR1ProcessOnly(PRU *pPRU)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();

	if (!m_bPRSelected)
	{
		m_nPRInProgress = ST_PR_IDLE;
		return TRUE;
	}

	if (!SearchPRProcessOnly(pPRU))
	{
		m_nPRInProgress = ST_PR_IDLE;
		return FALSE;
	}
	
	m_PRCurrPBPR1Corner[PR_UPPER_LEFT].x	= pPRU->arcoDieCorners[PR_UPPER_LEFT].x;
	m_PRCurrPBPR1Corner[PR_UPPER_LEFT].y	= pPRU->arcoDieCorners[PR_UPPER_LEFT].y;
	m_PRCurrPBPR1Corner[PR_LOWER_LEFT].x	= pPRU->arcoDieCorners[PR_LOWER_LEFT].x;
	m_PRCurrPBPR1Corner[PR_LOWER_LEFT].y	= pPRU->arcoDieCorners[PR_LOWER_LEFT].y;
	m_PRCurrPBPR1Corner[PR_UPPER_RIGHT].x	= pPRU->arcoDieCorners[PR_UPPER_RIGHT].x;
	m_PRCurrPBPR1Corner[PR_UPPER_RIGHT].y	= pPRU->arcoDieCorners[PR_UPPER_RIGHT].y;
	m_PRCurrPBPR1Corner[PR_LOWER_RIGHT].x	= pPRU->arcoDieCorners[PR_LOWER_RIGHT].x;
	m_PRCurrPBPR1Corner[PR_LOWER_RIGHT].y	= pPRU->arcoDieCorners[PR_LOWER_RIGHT].y;

	m_nPRInProgress = ST_PR_IDLE;

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("%s PR1 Process Time: %.5f", GetStnName(), dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}

BOOL CPRTaskStn::AutoSearchPBPR2(PRU *pPRU)
{
	if (!m_bPRSelected)
	{
		if (pPRU->szStnName == "MB")
		{
			Sleep(m_lPRDelay2);
		}
		else
		{
			Sleep(m_lPRDelay);
		}
		return TRUE;
	}

	if (!SearchPR(pPRU))
	{
		return FALSE;
	}
	
	m_PRCurrPBPR2Corner[PR_UPPER_LEFT].x	= pPRU->arcoDieCorners[PR_UPPER_LEFT].x;
	m_PRCurrPBPR2Corner[PR_UPPER_LEFT].y	= pPRU->arcoDieCorners[PR_UPPER_LEFT].y;
	m_PRCurrPBPR2Corner[PR_LOWER_LEFT].x	= pPRU->arcoDieCorners[PR_LOWER_LEFT].x;
	m_PRCurrPBPR2Corner[PR_LOWER_LEFT].y	= pPRU->arcoDieCorners[PR_LOWER_LEFT].y;
	m_PRCurrPBPR2Corner[PR_UPPER_RIGHT].x	= pPRU->arcoDieCorners[PR_UPPER_RIGHT].x;
	m_PRCurrPBPR2Corner[PR_UPPER_RIGHT].y	= pPRU->arcoDieCorners[PR_UPPER_RIGHT].y;
	m_PRCurrPBPR2Corner[PR_LOWER_RIGHT].x	= pPRU->arcoDieCorners[PR_LOWER_RIGHT].x;
	m_PRCurrPBPR2Corner[PR_LOWER_RIGHT].y	= pPRU->arcoDieCorners[PR_LOWER_RIGHT].y;

	return TRUE;
}


BOOL CPRTaskStn::AutoSearchPBPR2GrabOnly(PRU *pPRU)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();

	m_nPRInProgress = ST_PB_PR2_IN_PROGRESS;

	if (!m_bPRSelected)
	{
		return TRUE;
	}

	if (!SearchPRGrabOnly(pPRU))
	{
		m_nPRInProgress = ST_PR_IDLE;
		return FALSE;
	}

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("%s PR2 Grab Time: %.5f", GetStnName(), dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}

BOOL CPRTaskStn::AutoSearchPBPR2ProcessOnly(PRU *pPRU)
{
	// debug time
	//DOUBLE dProcessTime = 0.0;
	//DWORD dwStartTime = GetTickCount();

	if (!m_bPRSelected)
	{
		m_nPRInProgress = ST_PR_IDLE;
		return TRUE;
	}

	if (!SearchPRProcessOnly(pPRU))
	{
		m_nPRInProgress = ST_PR_IDLE;
		return FALSE;
	}
	
	m_PRCurrPBPR2Corner[PR_UPPER_LEFT].x	= pPRU->arcoDieCorners[PR_UPPER_LEFT].x;
	m_PRCurrPBPR2Corner[PR_UPPER_LEFT].y	= pPRU->arcoDieCorners[PR_UPPER_LEFT].y;
	m_PRCurrPBPR2Corner[PR_LOWER_LEFT].x	= pPRU->arcoDieCorners[PR_LOWER_LEFT].x;
	m_PRCurrPBPR2Corner[PR_LOWER_LEFT].y	= pPRU->arcoDieCorners[PR_LOWER_LEFT].y;
	m_PRCurrPBPR2Corner[PR_UPPER_RIGHT].x	= pPRU->arcoDieCorners[PR_UPPER_RIGHT].x;
	m_PRCurrPBPR2Corner[PR_UPPER_RIGHT].y	= pPRU->arcoDieCorners[PR_UPPER_RIGHT].y;
	m_PRCurrPBPR2Corner[PR_LOWER_RIGHT].x	= pPRU->arcoDieCorners[PR_LOWER_RIGHT].x;
	m_PRCurrPBPR2Corner[PR_LOWER_RIGHT].y	= pPRU->arcoDieCorners[PR_LOWER_RIGHT].y;

	m_nPRInProgress = ST_PR_IDLE;

	// debug time
	//dProcessTime = (DOUBLE)(GetTickCount() - dwStartTime);
	//CString szMsg = "";
	//szMsg.Format("%s PR2 Process Time: %.5f", GetStnName(), dProcessTime);
	//DisplayMessage(szMsg);

	return TRUE;
}

VOID CPRTaskStn::UpdateHMIPRButton()
{
	PRU *pPRU;

	//Normal PR
	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPRU(pThis->HMI_ulCurPRU);
	pThis->HMI_lPRSearchRange = g_lSearchRange;	//20150115 pPRU->stExtShapeCmd.uwSrchRange;

	if(pPRU->emRecordType == PRS_ACF_TYPE)
	{
		pThis->HMI_ulPRID = pPRU->stSrchACFCmd.uwRecordID;
		pThis->HMI_dPRMatchScore = pPRU->stSrchACFCmd.rMatchScore; //20141217 * 100 ;
		pThis->HMI_dPRAngleRange = pPRU->stSrchACFCmd.rAngleRange;
		pThis->HMI_dPRPercentVar = 5.0;
		pThis->HMI_lDefectThreshold = pPRU->stSrchACFCmd.stACFInsp.ubDefectThreshold;
		pThis->HMI_dPostBdRejectTol = pPRU->dPostBdRejectTol;
	}
	else
	{
		pThis->HMI_ulPRID = pPRU->stSrchCmd.uwRecordID;
	}

//	switch (pPRU->emRecordType)
//	{
//	// Fiducial Fitting or Pattern Matching or Edge Matching
//	case PRS_DIE_TYPE:
//		pThis->HMI_ulPRID = pPRU->stSrchDieCmd.auwRecordID[0];
//		pThis->HMI_dPRMatchScore = pPRU->stSrchDieCmd.stDieAlignPar.rMatchScore;
//		pThis->HMI_dPRAngleRange = pPRU->stSrchDieCmd.stDieAlignPar.rEndAngle;
//		pThis->HMI_dPRPercentVar = pPRU->stSrchDieCmd.stDieAlignPar.ubMaxScale - 100.0; // Hard-set
//#ifdef CHECK_COLLET_DIRT
//		pThis->HMI_lDefectAttribute = pPRU->stSrchDieCmd.stDieInsp.stTmpl.emDefectAttribute;
//		pThis->HMI_lDefectThreshold = pPRU->stSrchDieCmd.stDieInsp.stTmpl.ubDefectThreshold;
//		pThis->HMI_dMinSingleDefectArea = (DOUBLE) (pPRU->stSrchDieCmd.stDieInsp.stTmpl.aeMinSingleDefectArea) / 8192.0 / 8192.0 * 100.0;
//		pThis->HMI_dMinTotalDefectArea = (DOUBLE) (pPRU->stSrchDieCmd.stDieInsp.stTmpl.aeMinTotalDefectArea) / 8192.0 / 8192.0 * 100.0;
//#endif
//		break;
//
//	// Shape Fitting
//	case PRS_SHAPE_TYPE:
//		pThis->HMI_ulPRID = pPRU->stDetectShapeCmd.uwRecordID;
//		pThis->HMI_dPRMatchScore = pPRU->stDetectShapeCmd.rDetectScore;
//		pThis->HMI_dPRAngleRange = pPRU->stDetectShapeCmd.rEndAngle;
//		pThis->HMI_dPRPercentVar = pPRU->stDetectShapeCmd.rSizePercentVar;
//		break;
//
//	// Tmpl Fitting
//	case PRS_TMPL_TYPE:
//		pThis->HMI_ulPRID = pPRU->stSrchTmplCmd.uwRecordID;
//		pThis->HMI_dPRMatchScore = pPRU->stSrchTmplCmd.ubPassScore;
//		pThis->HMI_dPRAngleRange = 0.0;
//		pThis->HMI_dPRPercentVar = 5.0;
//		break;
//
//	// Kerf Fitting
//	case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
//		pThis->HMI_ulPRID = pPRU->stInspKerfCmd.uwRecordID;
//		pThis->HMI_dPRMatchScore = pPRU->stLrnKerfCmd.uwFittingPassingScore;
//		pThis->HMI_dPRAngleRange = pPRU->stLrnKerfCmd.rLowerBoundSearchAngle + pPRU->stLrnKerfCmd.rUpperBoundSearchAngle;
//		pThis->HMI_dPRPercentVar = pPRU->stLrnKerfCmd.rMaxWidthDev;
//		break;
//
//	case PRS_ACF_TYPE:
//		pThis->HMI_ulPRID = pPRU->stSrchACFCmd.uwRecordID;
//		pThis->HMI_dPRMatchScore = pPRU->stSrchACFCmd.rMatchScore; //20141217 * 100 ;
//		pThis->HMI_dPRAngleRange = pPRU->stSrchACFCmd.rAngleRange;
//		pThis->HMI_dPRPercentVar = 5.0;
//		pThis->HMI_lDefectThreshold = pPRU->stSrchACFCmd.stACFInsp.ubDefectThreshold;
//		pThis->HMI_dPostBdRejectTol = pPRU->dPostBdRejectTol;
//		break;
//
//	}

	pThis->HMI_bShowPRSearch = TRUE; //pPRU->bLoaded;
}

VOID CPRTaskStn::UpdateHMIPBPRButton()
{
	PRU *pPRU;

	//Normal PR
	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);
	pThis->HMI_lPRSearchRange = g_lSearchRange;	//20150115 pPRU->stExtShapeCmd.uwSrchRange;

	if(pPRU->emRecordType == PRS_ACF_TYPE)
	{
		pThis->HMI_ulPRID = pPRU->stSrchACFCmd.uwRecordID;
		pThis->HMI_dPRMatchScore = pPRU->stSrchACFCmd.rMatchScore; //20141217 * 100 ;
		pThis->HMI_dPRAngleRange = pPRU->stSrchACFCmd.rAngleRange;
		pThis->HMI_dPRPercentVar = 5.0;
		pThis->HMI_lDefectThreshold = pPRU->stSrchACFCmd.stACFInsp.ubDefectThreshold;
		pThis->HMI_dPostBdRejectTol = pPRU->dPostBdRejectTol;
	}
	else
	{
		pThis->HMI_ulPRID = pPRU->stSrchCmd.uwRecordID;
	}

	pThis->HMI_bShowPRSearch = TRUE; //pPRU->bLoaded;
}

VOID CPRTaskStn::DisplaySrchArea(PRU *pPRU)// only for PostBond
{
	if (!pPRU->bLoaded)
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "PR Pattern Not Loaded");
		return;
	}

	if (pPRU->stLrnACFCmd.uwIsAlign == FALSE)
	{
		DisplayPBNonAlignSrchArea(pPRU);
	}
	else
	{
		if (pPRU->stLrnACFCmd.emACFSegmentAlg == PR_ACF_SEGMENT_ALG_NO_PREPROCESS)
		{
			DisplayPBSrchArea(pPRU);
		}
		else
		{
			DisplayNormalSrchArea(pPRU);
		}
	}
}

VOID CPRTaskStn::DisplayPBNonAlignSrchArea(PRU *pPRU)
{
	PR_COORD	coUpperLeft;
	PR_COORD	coLowerRight;

	// Draw the Large rectangle
	coUpperLeft.x = (PR_WORD)(pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[1].x);
	coUpperLeft.y = (PR_WORD)(pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[1].y);
	coLowerRight.x = (PR_WORD)(pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[3].x);
	coLowerRight.y = (PR_WORD)(pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].acoCorners[3].y);	
	PRS_DrawRect(pPRU->nCamera, coUpperLeft, coLowerRight, PR_COLOR_RED);
	
	// Draw the small rectangle
	coUpperLeft.x = (PR_WORD)(pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[1].x);
	coUpperLeft.y = (PR_WORD)(pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[1].y);
	coLowerRight.x = (PR_WORD)(pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[3].x);
	coLowerRight.y = (PR_WORD)(pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].acoCorners[3].y);	
	PRS_DrawRect(pPRU->nCamera, coUpperLeft, coLowerRight, PR_COLOR_RED);

}

VOID CPRTaskStn::DisplayPBSrchArea(PRU *pPRU, LONG lSearchPixel)
{
	PR_COORD	coUpperLeft;
	PR_COORD	coLowerRight;

	// LeftSide
	if (pPRU->stSrchDieCmd.emAlignAlg == PRS_ALG_L_EDGE)
	{
		// Draw the left vertical rectangle
		coUpperLeft.x = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_UPPER_LEFT].x - (lSearchPixel * 16));
		coUpperLeft.y = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_UPPER_LEFT].y - (lSearchPixel * 16));
		coLowerRight.x = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_LOWER_LEFT].x + (lSearchPixel * 16));
		coLowerRight.y = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_LOWER_LEFT].y + (lSearchPixel * 16));	
		PRS_DrawRect(pPRU->nCamera, coUpperLeft, coLowerRight, PR_COLOR_RED);

		// Draw the upper horizontal rectangle
		coUpperLeft.x = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_UPPER_LEFT].x - (lSearchPixel * 16));
		coUpperLeft.y = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_UPPER_LEFT].y - (lSearchPixel * 16));
		coLowerRight.x = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_UPPER_RIGHT].x + (lSearchPixel * 16));
		coLowerRight.y = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_UPPER_RIGHT].y + (lSearchPixel * 16));	
		PRS_DrawRect(pPRU->nCamera, coUpperLeft, coLowerRight, PR_COLOR_RED);
		
		// Draw the Lower horizontal rectangle
		coUpperLeft.x = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_LOWER_LEFT].x - (lSearchPixel * 16));
		coUpperLeft.y = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_LOWER_LEFT].y - (lSearchPixel * 16));
		coLowerRight.x = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_LOWER_RIGHT].x + (lSearchPixel * 16));
		coLowerRight.y = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_LOWER_RIGHT].y + (lSearchPixel * 16));	
		PRS_DrawRect(pPRU->nCamera, coUpperLeft, coLowerRight, PR_COLOR_RED);
	}
	else
	{	
		// Draw the left vertical rectangle
		coUpperLeft.x = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_UPPER_RIGHT].x - (lSearchPixel * 16));
		coUpperLeft.y = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_UPPER_RIGHT].y - (lSearchPixel * 16));
		coLowerRight.x = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_LOWER_RIGHT].x + (lSearchPixel * 16));
		coLowerRight.y = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_LOWER_RIGHT].y + (lSearchPixel * 16));	
		PRS_DrawRect(pPRU->nCamera, coUpperLeft, coLowerRight, PR_COLOR_RED);

		// Draw the upper horizontal rectangle
		coUpperLeft.x = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_UPPER_LEFT].x - (lSearchPixel * 16));
		coUpperLeft.y = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_UPPER_LEFT].y - (lSearchPixel * 16));
		coLowerRight.x = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_UPPER_RIGHT].x + (lSearchPixel * 16));
		coLowerRight.y = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_UPPER_RIGHT].y + (lSearchPixel * 16));	
		PRS_DrawRect(pPRU->nCamera, coUpperLeft, coLowerRight, PR_COLOR_RED);
		
		// Draw the Lower horizontal rectangle
		coUpperLeft.x = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_LOWER_LEFT].x - (lSearchPixel * 16));
		coUpperLeft.y = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_LOWER_LEFT].y - (lSearchPixel * 16));
		coLowerRight.x = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_LOWER_RIGHT].x + (lSearchPixel * 16));
		coLowerRight.y = (PR_WORD)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_LOWER_RIGHT].y + (lSearchPixel * 16));	
		PRS_DrawRect(pPRU->nCamera, coUpperLeft, coLowerRight, PR_COLOR_RED);
	}
}

VOID CPRTaskStn::DisplayNormalSrchArea(PRU *pPRU)
{
	PR_COORD	coUpperLeft;
	PR_COORD	coLowerRight;

	// Draw the Large rectangle
	coUpperLeft.x = (PR_WORD)(pPRU->stLrnACFCmd.stSrchRegion.acoCorners[1].x);
	coUpperLeft.y = (PR_WORD)(pPRU->stLrnACFCmd.stSrchRegion.acoCorners[1].y);
	coLowerRight.x = (PR_WORD)(pPRU->stLrnACFCmd.stSrchRegion.acoCorners[3].x);
	coLowerRight.y = (PR_WORD)(pPRU->stLrnACFCmd.stSrchRegion.acoCorners[3].x);
	PRS_DrawRect(pPRU->nCamera, coUpperLeft, coLowerRight, PR_COLOR_RED);
}

VOID CPRTaskStn::PrintCalibSetup(FILE *fp)
{
	CString str, str1, str2 = "";

	if (fp != NULL)
	{
		fprintf(fp, "\n%s COR Calibration\n", (const char*)m_szPRTaskStnName);
		fprintf(fp, "======================================================\n");

		
		
		for (INT j = 0; j < MAX_NUM_OF_TA4_COR; j++)
		{
			str.Format("m_bCORCalibrated[%ld]", j);
			if ((BOOL)(LONG)m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][str])
			{
				fprintf(fp, "COR Calibration			: Calibrated\n");
			}
			else
			{
				fprintf(fp, "COR Calibration			: Not Calibrated\n");
			}
			for (INT i = 0; i < COR_ARRAY_SIZE; i++)
			{
				str.Format("m_dCORLUT[%ld][%ld]", j, i);
				str1.Format("m_dmtrTableCOR_Pos[%ld][%ld].x", j, i);
				str2.Format("m_dmtrTableCOR_Pos[%ld][%ld].y", j, i);
				fprintf(fp, "%30s\t : %.2f, %.2f, %.2f\n", "COR Angle (+)", 
					(DOUBLE)m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][str], 
					(DOUBLE)m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][str1], 
					(DOUBLE)m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][str2]);
			}


			for (INT i = 0; i < COR_ARRAY_SIZE; i++)
			{
				str.Format("m_dCORLUT[%ld][%ld]", j, i);
				str1.Format("m_dmtrTableCOR_Neg[%ld][%ld].x", j, i);
				str2.Format("m_dmtrTableCOR_Neg[%ld][%ld].y", j, i);
				fprintf(fp, "%30s\t : %.2f, %.2f, %.2f\n", "COR Angle (-)", 
					(DOUBLE)m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][str],  
					(DOUBLE)m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][str1], 
					(DOUBLE)m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][str2]);

			}
		}
	}
}
/////////////////////////////////////////////////////////////////
//HMI Function
/////////////////////////////////////////////////////////////////
LONG CPRTaskStn::HMI_SetPRSearchRange(IPC_CServiceMessage &svMsg)
{
	//PRU *pPRU;

	LONG lSearchRange;
	svMsg.GetMsg(sizeof(LONG), &lSearchRange);

	//SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	//CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	//pPRU = &pThis->SelectCurPRU(pThis->HMI_ulCurPRU);

	CString szOldValue;
	szOldValue.Format("Value = %.2f", g_lSearchRange);

	g_lSearchRange	= lSearchRange;

	CString szNewValue;
	szNewValue.Format("Value = %.2f", g_lSearchRange);

	LogPRParameter(__FUNCTION__, "PR Search Range", szOldValue, szNewValue);

	UpdateHMIPRButton();

	return 0;
}

LONG CPRTaskStn::HMI_SetPRMatchScore(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	DOUBLE dMatchScore;
	svMsg.GetMsg(sizeof(DOUBLE), &dMatchScore);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);

	CString szOldValue;
	szOldValue.Format("ID = %ld, Value = %.2f", pPRU->emID, pPRU->stSrchDieCmd.stDieAlignPar.rMatchScore);

	//pPRU->stSrchDieCmd.stDieAlignPar.rMatchScore	= (PR_REAL)dMatchScore;
	//pPRU->stSrchDieCmd.rCfmPatternMatchScore		= (PR_REAL)dMatchScore; // Comfirmation Pattern 	
	//pPRU->stDetectShapeCmd.rDetectScore				= (PR_REAL)dMatchScore;
	//pPRU->stSrchTmplCmd.ubPassScore					= (PR_UBYTE)dMatchScore;
	//pPRU->stLrnKerfCmd.uwFittingPassingScore		= (PR_UWORD)dMatchScore;	// 20141023 Yip: Add Kerf Fitting Functions
	pPRU->stSrchACFCmd.rMatchScore					= (PR_REAL)dMatchScore;//20141217 / 100;

	CString szNewValue;
	szNewValue.Format("ID = %ld, Value = %.2f", pPRU->emID, pPRU->stSrchDieCmd.stDieAlignPar.rMatchScore);

	LogPRParameter(__FUNCTION__, "PR Match Score", szOldValue, szNewValue);

	UpdateHMIPBPRButton();

	return 0;
}

LONG CPRTaskStn::HMI_SetPRAngRange(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	DOUBLE dAngRange;
	svMsg.GetMsg(sizeof(DOUBLE), &dAngRange);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPRU(pThis->HMI_ulCurPRU);

	PR_REAL rOldAngle = (PR_REAL)dAngRange;

	switch (pPRU->emRecordType)
	{
	// Pattern Matching or Edge Matching
	case PRS_DIE_TYPE:
		rOldAngle = pPRU->stSrchDieCmd.stDieAlignPar.rEndAngle;
		pPRU->stSrchDieCmd.stDieAlignPar.rStartAngle = -(PR_REAL)dAngRange;
		pPRU->stSrchDieCmd.stDieAlignPar.rEndAngle = (PR_REAL)dAngRange;
		break;

	// Shape Fitting
	case PRS_SHAPE_TYPE:
		rOldAngle = pPRU->stDetectShapeCmd.rEndAngle;
		pPRU->stDetectShapeCmd.rStartAngle = -(PR_REAL)dAngRange; //20130103
		pPRU->stDetectShapeCmd.rEndAngle = (PR_REAL)dAngRange;
		break;

	// Tmpl Fitting
	case PRS_TMPL_TYPE:
		break;

	// Kerf Fitting
	case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
		rOldAngle = pPRU->stLrnKerfCmd.rUpperBoundSearchAngle;
		pPRU->stLrnKerfCmd.rLowerBoundSearchAngle = (PR_REAL)dAngRange;
		pPRU->stLrnKerfCmd.rUpperBoundSearchAngle = (PR_REAL)dAngRange;
		break;

	case PRS_ACF_TYPE:
		rOldAngle = pPRU->stSrchACFCmd.rAngleRange;
		pPRU->stSrchACFCmd.rAngleRange = (PR_REAL)dAngRange;
		break;
	}

	CString szOldValue;
	szOldValue.Format("ID = %ld, Value = %.2f", pPRU->emID, rOldAngle);
	CString szNewValue;
	szNewValue.Format("ID = %ld, Value = %.2f", pPRU->emID, (PR_REAL)dAngRange);

	LogPRParameter(__FUNCTION__, "PR Angle Range", szOldValue, szNewValue);

	UpdateHMIPRButton();

	return 0;
}

LONG CPRTaskStn::HMI_SetPRPercentVar(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	DOUBLE dVar;
	svMsg.GetMsg(sizeof(DOUBLE), &dVar);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPRU(pThis->HMI_ulCurPRU);

	PR_REAL rOldVar = (PR_REAL)dVar;

	switch (pPRU->emRecordType)
	{
	case PRS_DIE_TYPE:
		rOldVar = (PR_REAL)(pPRU->stSrchDieCmd.stDieAlignPar.ubMaxScale - 100);
		pPRU->stSrchDieCmd.stDieAlignPar.ubMaxScale	= (PR_UBYTE)(100.0 + dVar); //20130103
		pPRU->stSrchDieCmd.stDieAlignPar.ubMinScale	= (PR_UBYTE)(100.0 - dVar);
		break;

	case PRS_SHAPE_TYPE:
		rOldVar = pPRU->stDetectShapeCmd.rSizePercentVar;
		pPRU->stDetectShapeCmd.rSizePercentVar		= (PR_REAL)dVar;
		break;

	case PRS_TMPL_TYPE:
		break;

	case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
		rOldVar = pPRU->stLrnKerfCmd.rMaxWidthDev;
		pPRU->stLrnKerfCmd.rMaxWidthDev		= (PR_REAL)dVar;
		break;

	case PRS_ACF_TYPE:
		break;
	}

	CString szOldValue;
	szOldValue.Format("ID = %ld, Value = %.2f", pPRU->emID, rOldVar);
	CString szNewValue;
	szNewValue.Format("ID = %ld, Value = %.2f", pPRU->emID, (PR_REAL)dVar);

	LogPRParameter(__FUNCTION__, "PR Percentage Variation", szOldValue, szNewValue);

	UpdateHMIPRButton();

	return 0;
}

LONG CPRTaskStn::HMI_IncrSegThresholdLevel10(IPC_CServiceMessage &svMsg)
{
	LONG	lThreshold;
	PRU *pPRU = NULL;
	PRU *pPRUBackground = NULL;

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);
	pPRUBackground = &pThis->SelectCurPBBackground(pThis->HMI_ulCurPBPRU);
	
	lThreshold = (LONG)pPRU->stLrnACFCmd.ubSegThreshold;
	
	lThreshold += 10;
	if (lThreshold > 255)
	{
		lThreshold = 255;
	}

	pThis->HMI_lSegThreshold = lThreshold;
	pThis->HMI_lDefectThreshold	= lThreshold; //20130429 suggest by WAI

	pPRU->stLrnACFCmd.ubSegThreshold	= (PR_UBYTE)lThreshold;
	PRS_DisplayACFInfo(pPRU, pPRUBackground, PR_ACF_DISPLAY_SEGMENT_IMG);

	return 0;
}

LONG CPRTaskStn::HMI_IncrSegThresholdLevel(IPC_CServiceMessage &svMsg)
{
	LONG	lThreshold;
	PRU *pPRU = NULL;
	PRU *pPRUBackground = NULL;

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);
	pPRUBackground = &pThis->SelectCurPBBackground(pThis->HMI_ulCurPBPRU);

	lThreshold = (LONG)pPRU->stLrnACFCmd.ubSegThreshold;
	
	lThreshold += 1;
	if (lThreshold > 255)
	{
		lThreshold = 255;
	}

	pThis->HMI_lSegThreshold = lThreshold;
	pThis->HMI_lDefectThreshold	= lThreshold; //20130429 suggest by WAI

	pPRU->stLrnACFCmd.ubSegThreshold	= (PR_UBYTE)lThreshold;
	PRS_DisplayACFInfo(pPRU, pPRUBackground, PR_ACF_DISPLAY_SEGMENT_IMG);
	
	return 0;
}

LONG CPRTaskStn::HMI_DecrSegThresholdLevel10(IPC_CServiceMessage &svMsg)
{
	LONG	lThreshold;
	PRU *pPRU = NULL;
	PRU *pPRUBackground = NULL;

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);
	pPRUBackground = &pThis->SelectCurPBBackground(pThis->HMI_ulCurPBPRU);

	lThreshold = (LONG)pPRU->stLrnACFCmd.ubSegThreshold;
	
	lThreshold -= 10;
	if (lThreshold > 255)
	{
		lThreshold = 255;
	}

	pThis->HMI_lSegThreshold = lThreshold;
	pThis->HMI_lDefectThreshold	= lThreshold; //20130429 suggest by WAI

	pPRU->stLrnACFCmd.ubSegThreshold	= (PR_UBYTE)lThreshold;
	PRS_DisplayACFInfo(pPRU, pPRUBackground, PR_ACF_DISPLAY_SEGMENT_IMG);

	return 0;

}

LONG CPRTaskStn::HMI_DecrSegThresholdLevel(IPC_CServiceMessage &svMsg)
{
	LONG	lThreshold;
	PRU *pPRU = NULL;
	PRU *pPRUBackground = NULL;

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);
	pPRUBackground = &pThis->SelectCurPBBackground(pThis->HMI_ulCurPBPRU);

	lThreshold = (LONG)pPRU->stLrnACFCmd.ubSegThreshold;
	
	lThreshold -= 1;
	if (lThreshold > 255)
	{
		lThreshold = 255;
	}

	pThis->HMI_lSegThreshold = lThreshold;
	pThis->HMI_lDefectThreshold	= lThreshold; //20130429 suggest by WAI

	pPRU->stLrnACFCmd.ubSegThreshold	= (PR_UBYTE)lThreshold;
	PRS_DisplayACFInfo(pPRU, pPRUBackground, PR_ACF_DISPLAY_SEGMENT_IMG);

	return 0;
}

LONG CPRTaskStn::HMI_SetSegThreshold(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU = NULL;
	PRU *pPRUBackground = NULL;

	LONG lThreshold;
	svMsg.GetMsg(sizeof(LONG), &lThreshold);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);
	pPRUBackground = &pThis->SelectCurPBBackground(pThis->HMI_ulCurPRU);

	pThis->HMI_lSegThreshold = lThreshold;

	pPRU->stLrnACFCmd.ubSegThreshold	= (PR_UBYTE)lThreshold;
	PRS_DisplayACFInfo(pPRU, pPRUBackground, PR_ACF_DISPLAY_SEGMENT_IMG);

	UpdateHMIPBPRButton();

	return TRUE;
}

LONG CPRTaskStn::HMI_SetDefectThreshold(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	LONG lThreshold;
	svMsg.GetMsg(sizeof(LONG), &lThreshold);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);

	CString szOldValue;
	szOldValue.Format("ID = %ld, Value = %ld", pPRU->emID, pPRU->stSrchACFCmd.stACFInsp.ubDefectThreshold);

	pThis->HMI_lDefectThreshold	= lThreshold;

	pPRU->stSrchACFCmd.stACFInsp.ubDefectThreshold			= (PR_UBYTE)lThreshold;

	CString szNewValue;
	szNewValue.Format("ID = %ld, Value = %ld", pPRU->emID, pPRU->stSrchACFCmd.stACFInsp.ubDefectThreshold);

	LogPRParameter(__FUNCTION__, "PR Defect Threshold", szOldValue, szNewValue);

	UpdateHMIPBPRButton();

	return 0;
}

LONG CPRTaskStn::HMI_SetDefectAttribute(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	ULONG ulAttrib;
	svMsg.GetMsg(sizeof(ULONG), &ulAttrib);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);

	CString szOldValue;
	szOldValue.Format("ID = %ld, Value = %ld", pPRU->emID, pPRU->stSrchACFCmd.stACFInsp.emDefectAttribute);

	pThis->HMI_ulPRPostBdAttrib	= ulAttrib;
	switch (ulAttrib)
	{
	case 0:
		pPRU->stSrchACFCmd.stACFInsp.emDefectAttribute = PR_DEFECT_ATTRIBUTE_BRIGHT;
		break;

	case 1:
		pPRU->stSrchACFCmd.stACFInsp.emDefectAttribute = PR_DEFECT_ATTRIBUTE_DARK;
		break;

	case 2:
		pPRU->stSrchACFCmd.stACFInsp.emDefectAttribute = PR_DEFECT_ATTRIBUTE_BOTH;
		break;
	}

	CString szNewValue;
	szNewValue.Format("ID = %ld, Value = %ld", pPRU->emID, pPRU->stSrchACFCmd.stACFInsp.emDefectAttribute);

	LogPRParameter(__FUNCTION__, "PR Defect Attribute", szOldValue, szNewValue);

	UpdateHMIPBPRButton();

	return 0;
}

LONG CPRTaskStn::HMI_SetPostBdRejectTol(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	DOUBLE dTol;
	svMsg.GetMsg(sizeof(DOUBLE), &dTol);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);

	CString szOldValue;
	szOldValue.Format("ID = %ld, Value = %ld", pPRU->emID, pThis->HMI_dPostBdRejectTol);

	pThis->HMI_dPostBdRejectTol = dTol;

	//pPRU->dPostBdRejectTol = HMI_dPostBdRejectTol;
	pPRU->dPostBdRejectTol = pThis->HMI_dPostBdRejectTol; //20121227
		
	pPRU->stSrchACFCmd.stACFInsp.aeMinChipArea = 
		(PR_AREA)pPRU->stSrchACFCmd.stACFInsp.aeMinChipArea = 
			(PR_AREA)(
				(DOUBLE)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_UPPER_RIGHT].x - pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_LOWER_LEFT].x) * 
				(DOUBLE)(pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_LOWER_LEFT].y - pPRU->stLrnACFCmd.stACFRegion.acoCorners[PR_UPPER_RIGHT].y) * 
				pPRU->dPostBdRejectTol / 100.0 
			);
	pPRU->stSrchACFCmd.stACFInsp.aeMinTotalDefectArea[0]	= pPRU->stSrchACFCmd.stACFInsp.aeMinChipArea;
	pPRU->stSrchACFCmd.stACFInsp.aeMinSingleDefectArea[0]	= pPRU->stSrchACFCmd.stACFInsp.aeMinChipArea;

	CString szNewValue;
	szNewValue.Format("ID = %ld, Value = %ld", pPRU->emID, pThis->HMI_dPostBdRejectTol);

	LogPRParameter(__FUNCTION__, "Post Bond Reject Tolerance", szOldValue, szNewValue);

	UpdateHMIPBPRButton();

	return 0;
}

// Basic /Advance
LONG CPRTaskStn::HMI_SetACFPostBondMethod(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	LONG lMethod;
	svMsg.GetMsg(sizeof(LONG), &lMethod);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);

	pPRU->lPBMethod	= lMethod;
	HMI_lPBMethod = lMethod;

	if (pPRU->lPBMethod == BASIC)
	{
		HMI_bShowBasicAlg = TRUE;
		HMI_bShowAdvanceAlg = FALSE;

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
			//SetHmiVariable("ACF_WH_lIsAlign");
			//SetHmiVariable("ACF_WH_lAlignAlg");
			//SetHmiVariable("SF_szWizardBMPath");

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
			//SetHmiVariable("ACF_WH_lIsAlign");
			//SetHmiVariable("ACF_WH_lAlignAlg");
			//SetHmiVariable("SF_szWizardBMPath");
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
			//SetHmiVariable("ACF_WH_lIsAlign");
			//SetHmiVariable("ACF_WH_lAlignAlg");
			//SetHmiVariable("SF_szWizardBMPath");
		}
		else
		{
			m_lIsAlign	= 0;
			m_lAlignAlg = 0;
			pPRU->stLrnACFCmd.uwIsAlign			= PR_FALSE;
			pPRU->stLrnACFCmd.emACFSegmentAlg	= PR_ACF_SEGMENT_ALG_NO_PREPROCESS;
			pPRU->stLrnACFCmd.emACFAlignAlg		= PR_ACF_ALIGN_ALG_PARALLEL_LINE_POINT_FITTING;
			g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\ParallelLine_NonAlign.png";
			//SetHmiVariable("ACF_WH_lIsAlign");
			//SetHmiVariable("ACF_WH_lAlignAlg");
			//SetHmiVariable("SF_szWizardBMPath");
			m_pAppMod->WriteHMIMess("Convert To Non_Align Mode");
		}
	}
	else if (pPRU->lPBMethod == ADVANCE)
	{
		HMI_bShowBasicAlg = FALSE;
		HMI_bShowAdvanceAlg = TRUE;

		HMI_lACFIsAlign		= pPRU->stLrnACFCmd.uwIsAlign;
		HMI_lACFSegmentAlg	= pPRU->stLrnACFCmd.emACFSegmentAlg;
		HMI_lACFAlignAlg	= pPRU->stLrnACFCmd.emACFAlignAlg;

		if (pPRU->stLrnACFCmd.uwIsAlign == PR_TRUE)
		{
			HMI_bShowACFAlignAlg = TRUE;
		}
		else
		{
			HMI_bShowACFAlignAlg = FALSE;
		}
	}

	return 0;
}
// Basic Method // For New Command 
LONG CPRTaskStn::HMI_SetIsAlign(IPC_CServiceMessage &svMsg)
{	
	PRU *pPRU; 

	LONG lMode;
	svMsg.GetMsg(sizeof(BOOL), &lMode);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);

	pThis->m_lIsAlign = lMode; //20120927

	if (!pThis->m_lIsAlign) //Non-Align
	{
		m_lAlignAlg = 0;
		pPRU->stLrnACFCmd.uwIsAlign			= PR_FALSE;
		pPRU->stLrnACFCmd.emACFSegmentAlg	= PR_ACF_SEGMENT_ALG_NO_PREPROCESS;
		pPRU->stLrnACFCmd.emACFAlignAlg		= PR_ACF_ALIGN_ALG_PARALLEL_LINE_POINT_FITTING;

		g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\ParallelLine_NonAlign.png";
		//SetHmiVariable("SF_szWizardBMPath");
		//SetHmiVariable("ACF_WH_lIsAlign");
		//SetHmiVariable("ACF_WH_lAlignAlg");

	}
	else
	{
		m_lAlignAlg = 0;
		pPRU->stLrnACFCmd.uwIsAlign			= PR_TRUE;
		pPRU->stLrnACFCmd.emACFSegmentAlg	= PR_ACF_SEGMENT_ALG_PREPROCESS;
		pPRU->stLrnACFCmd.emACFAlignAlg		= PR_ACF_ALIGN_ALG_PARALLEL_LINE_POINT_FITTING;

		g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\ParallelLine_Align.png";
		//SetHmiVariable("SF_szWizardBMPath");
		//SetHmiVariable("ACF_WH_lIsAlign");
		//SetHmiVariable("ACF_WH_lAlignAlg");

	}

	return 0;
}

LONG CPRTaskStn::HMI_SetAlignAlg(IPC_CServiceMessage &svMsg)
{	
	PRU *pPRU; 

	LONG lMode;
	svMsg.GetMsg(sizeof(BOOL), &lMode);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);

	pThis->m_lAlignAlg = lMode;

	switch (pThis->m_lAlignAlg)
	{
	case 0: //ParallelLineWithAlign
		m_lIsAlign	= 1;
		pThis->m_lAlignAlg	= 0;
		g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\ParallelLine_Align.png";
		//SetHmiVariable("SF_szWizardBMPath");
		//SetHmiVariable("ACF_WH_lIsAlign");
		//SetHmiVariable("ACF_WH_lAlignAlg");

		pPRU->stLrnACFCmd.emACFSegmentAlg	= PR_ACF_SEGMENT_ALG_PREPROCESS;
		pPRU->stLrnACFCmd.emACFAlignAlg		= PR_ACF_ALIGN_ALG_PARALLEL_LINE_POINT_FITTING;
		pPRU->stLrnACFCmd.uwIsAlign			= PR_TRUE;

		break;

	case 1:
		m_lIsAlign	= 1;
		pThis->m_lAlignAlg	= 1;
		g_szWizardBMPath = "d:\\sw\\AC9000\\Hmi\\images\\Wizard\\PR\\LineFitting_Align.png";
		//SetHmiVariable("SF_szWizardBMPath");
		//SetHmiVariable("ACF_WH_lIsAlign");
		//SetHmiVariable("ACF_WH_lAlignAlg");

		pPRU->stLrnACFCmd.emACFSegmentAlg	= PR_ACF_SEGMENT_ALG_NO_PREPROCESS;
		pPRU->stLrnACFCmd.emACFAlignAlg		= PR_ACF_ALIGN_ALG_3LINE_FITTING;
		pPRU->stLrnACFCmd.uwIsAlign			= PR_TRUE;
	
		break;
	}
				
	return 0;
}

// Advance Method // For New Command 
LONG CPRTaskStn::HMI_SetIsACFAlign(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	LONG lMethod;
	svMsg.GetMsg(sizeof(LONG), &lMethod);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);
	
	pPRU->stLrnACFCmd.uwIsAlign = (PR_UWORD)lMethod;

	HMI_lACFIsAlign = pPRU->stLrnACFCmd.uwIsAlign;

	if (pPRU->stLrnACFCmd.uwIsAlign == PR_TRUE)
	{
		HMI_bShowACFAlignAlg = TRUE;
	}
	else
	{
		HMI_bShowACFAlignAlg = FALSE;
	}

	return 0;
}

LONG CPRTaskStn::HMI_SetACFSegmentMethod(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	LONG lMethod;
	svMsg.GetMsg(sizeof(LONG), &lMethod);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);
	
	pPRU->stLrnACFCmd.emACFSegmentAlg = (PR_ACF_SEGMENT_ALG)lMethod;

	HMI_lACFSegmentAlg = pPRU->stLrnACFCmd.emACFSegmentAlg;

	return 0;
}

LONG CPRTaskStn::HMI_SetACFAlignMethod(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	LONG lMethod;
	svMsg.GetMsg(sizeof(LONG), &lMethod);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);
	
	pPRU->stLrnACFCmd.emACFAlignAlg = (PR_ACF_ALIGN_ALG)lMethod;

	HMI_lACFAlignAlg = pPRU->stLrnACFCmd.emACFAlignAlg;
	return 0;
}

VOID CPRTaskStn::MachineStartup_SrchPR(PRU *pPRU)	// 20140623 Yip
{
	if (!pPRU->bLoaded)
	{
		return;
	}

	PRS_DisplayVideo(pPRU);

#if 1 //Vision UI
	if (pPRU->emRecordType == PRS_ACF_TYPE)
	{
		pPRU->stSrchACFCmd.emDisplayInfo				= PR_ACF_DISPLAY_CORNERS_AND_REGIONS;
		PRS_SrchACFCmd(pPRU);
		pPRU->stSrchACFCmd.emDisplayInfo				= PR_ACF_DISPLAY_NONE;
	}
	else
	{
		PRS_ManualSrchCmd(pPRU);
	}
#else
	switch (pPRU->emRecordType)
	{
	// Pattern Matching or Edge Matching
	case PRS_DIE_TYPE:
		PRS_SrchDie(pPRU);
		break;

	// Shape Fitting
	case PRS_SHAPE_TYPE:
		PRS_DetectShape(pPRU);
		break;

	// Tmpl Fitting
	case PRS_TMPL_TYPE:
		PRS_SrchTmpl(pPRU);
		break;

	// Kerf Fitting
	case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
		PRS_InspKerf(pPRU);
		break;

	case PRS_ACF_TYPE:
		pPRU->stSrchACFCmd.emDisplayInfo				= PR_ACF_DISPLAY_CORNERS_AND_REGIONS;
		PRS_SrchACFCmd(pPRU);
		pPRU->stSrchACFCmd.emDisplayInfo				= PR_ACF_DISPLAY_NONE;
		break;
	}
#endif
	return;
}

//PR Action
LONG CPRTaskStn::HMI_ModifyPRCriteria(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPRU(pThis->HMI_ulCurPRU);

	PRS_DisplayVideo(pPRU);

	ModifyPRCriteria(pPRU);	// Vision UI

	UpdateHMIPRButton();

	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CPRTaskStn::HMI_SetAlignDistTol(IPC_CServiceMessage &svMsg)
{
	
	DOUBLE dLimit;
	svMsg.GetMsg(sizeof(DOUBLE), &dLimit);
	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	SetAndLogPRParameter(__FUNCTION__, "Align Distance Tolerance", pThis->m_dAlignDistTol, dLimit);

	return 0;
}


LONG CPRTaskStn::HMI_SetAlignAngleTol(IPC_CServiceMessage &svMsg)
{
	
	DOUBLE dLimit;
	svMsg.GetMsg(sizeof(DOUBLE), &dLimit);
	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	SetAndLogPRParameter(__FUNCTION__, "Align Angle Tolerance", pThis->m_dAlignAngleTol, dLimit);

	return 0;
}

//PR Action
BOOL CPRTaskStn::DetectLine(PRU *pPRU)	//20150114 Polarizer Detection
{
	//PRU *pPRU ;
	BOOL bResult = FALSE;
	MTR_POSN mtr_offset;

	if (!m_bPRSelected)
	{
		return TRUE;
	}

	if (pPRU->szStnName == "MB")
	{
		Sleep(m_lPRDelay2);
	}
	else
	{
		Sleep(m_lPRDelay);
	}
	
	if (!pPRU->bLoaded)
	{
		pPRU->rcoDieCentre.x	= PR_DEF_CENTRE_X;
		pPRU->rcoDieCentre.y	= PR_DEF_CENTRE_Y;
		pPRU->rcoDieCentre.x = (PR_REAL)PR_DEF_CENTRE_X; //20150728 float PR
		pPRU->rcoDieCentre.y = (PR_REAL)PR_DEF_CENTRE_Y;
		pPRU->fAng			= 0.0;
		pPRU->fScore		= 0.0;

		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "PR Pattern Not Loaded");

		return FALSE;
	}

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPRU(pThis->HMI_ulCurPRU);

	PRS_DisplayVideo(pPRU);

	bResult = PRS_DetectExpObjectCmd(pPRU);

	if (bResult)
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Line Found");
		CString szMsg = _T("");
		m_pAppMod->WriteHMIMess("Line Detect Succeed!");

		if (pPRU->bIs5MBCam && pPRU->bUseFloatingPointResult) //20150728 float PR
		{
			szMsg.Format("XY: (%.2f,%.2f), Ang: %.2f, Score: %.2f", 
						pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, pPRU->fAng, pPRU->fScore);
			m_pAppMod->WriteHMIMess(szMsg);
			PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);
			PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);
		}
		else
		{
			szMsg.Format("XY: (%ld,%ld), Ang: %.2f, Score: %.2f", 
						pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, pPRU->fAng, pPRU->fScore);
			m_pAppMod->WriteHMIMess(szMsg);
			PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);
			PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);
		}

		if (MoveRelativeXY(mtr_offset.x, mtr_offset.y, GMP_WAIT) != GMP_SUCCESS)
		{
			bResult = FALSE;
		}
	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Line Detect Failed!");
		m_pAppMod->WriteHMIMess("Line Detect Failed!");
		bResult = FALSE;
	}
	return	bResult;
}

LONG CPRTaskStn::HMI_DetectLine(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPRU(pThis->HMI_ulCurPRU);

	PRS_DisplayVideo(pPRU);

	if (PRS_DetectExpObjectCmd(pPRU))
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Line Found");

		CString szMsg = _T("");
		m_pAppMod->WriteHMIMess("Line Detect Succeed!");
		
		if (pPRU->bIs5MBCam && pPRU->bUseFloatingPointResult) //20150728 float PR
		{
			szMsg.Format("XY: (%.2f,%.2f), Ang: %.2f, Score: %.2f", 
						pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, pPRU->fAng, pPRU->fScore);
			m_pAppMod->WriteHMIMess(szMsg);

			PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);
		}
		else
		{
			szMsg.Format("XY: (%ld,%ld), Ang: %.2f, Score: %.2f", 
						pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, pPRU->fAng, pPRU->fScore);
			m_pAppMod->WriteHMIMess(szMsg);

			PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);
		}
	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Line Detect Failed!");
		m_pAppMod->WriteHMIMess("Line Detect Failed!");
	}
	UpdateHMIPRButton();

	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

#if 1 // Vision UI
LONG CPRTaskStn::HMI_SrchPR(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPRU(pThis->HMI_ulCurPRU);

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

	UpdateHMIPRButton();

	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}
#else
LONG CPRTaskStn::HMI_SrchPR(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;
	CString szMsg(" ");

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPRU(pThis->HMI_ulCurPRU);

	PRS_DisplayVideo(pPRU);

	//szMsg.Format("%s", pPRU->szStnName);
	//m_pAppMod->DisplayMessage(szMsg);


	switch (pPRU->emRecordType)
	{
	// Pattern Matching or Edge Matching
	case PRS_DIE_TYPE:
		SrchPRDie(pPRU);
		break;

	// Shape Fitting
	case PRS_SHAPE_TYPE:
		DetectPRShape(pPRU);
		break;

	// Tmpl Fitting
	case PRS_TMPL_TYPE:
		SrchPRTmpl(pPRU);
		break;

	// Kerf Fitting
	case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
		InspPRKerf(pPRU);
		break;

	case PRS_ACF_TYPE:
		pPRU->stSrchACFCmd.emDisplayInfo				= PR_ACF_DISPLAY_CORNERS_AND_REGIONS;
		SrchACFCmd(pPRU);
		pPRU->stSrchACFCmd.emDisplayInfo				= PR_ACF_DISPLAY_NONE;
		break;
	}

	UpdateHMIPRButton();

	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}
#endif

LONG CPRTaskStn::HMI_DisplaySrchArea(IPC_CServiceMessage &svMsg)
{
	PRU *pPRU;

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);
	pPRU = &pThis->SelectCurPBPRU(pThis->HMI_ulCurPBPRU);

	PRS_DisplayVideo(pPRU);

	DisplaySrchArea(pPRU);
	
	UpdateHMIPBPRButton();

	return 0;
}

LONG CPRTaskStn::HMI_SetPRRetryLimit(IPC_CServiceMessage &svMsg)
{
	LONG lLimit;
	svMsg.GetMsg(sizeof(LONG), &lLimit);
	
	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);

	SetAndLogPRParameter(__FUNCTION__, "PR Retry Limit", pThis->m_lPRRetryLimit, lLimit);

	return 0;
}

LONG CPRTaskStn::HMI_SetRejectGlassLimit(IPC_CServiceMessage &svMsg)
{
	LONG lLimit;
	svMsg.GetMsg(sizeof(LONG), &lLimit);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);

	SetAndLogPRParameter(__FUNCTION__, "Reject Glass Limit", pThis->m_lRejectGlassLimit, lLimit);

	return 0;
}

LONG CPRTaskStn::HMI_SetPRDelay(IPC_CServiceMessage &svMsg)
{
	LONG lDelay;
	svMsg.GetMsg(sizeof(LONG), &lDelay);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);

	SetAndLogPRParameter(__FUNCTION__, "PR Delay", pThis->m_lPRDelay, lDelay);

	return 0;
}

LONG CPRTaskStn::HMI_SetPRDelay2(IPC_CServiceMessage &svMsg) //20150323
{
	LONG lDelay;
	svMsg.GetMsg(sizeof(LONG), &lDelay);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);

	SetAndLogPRParameter(__FUNCTION__, "PR Delay", pThis->m_lPRDelay2, lDelay);

	return 0;
}

LONG CPRTaskStn::HMI_SetCalibPRDelay(IPC_CServiceMessage &svMsg)
{
	LONG lDelay;
	svMsg.GetMsg(sizeof(LONG), &lDelay);

	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	CPRTaskStn *pThis = dynamic_cast<CPRTaskStn*>(pGF);

	SetAndLogPRParameter(__FUNCTION__, "Calib PR Delay", pThis->m_lCalibPRDelay, lDelay);

	return 0;
}

LONG CPRTaskStn::IPC_SavePRTaskStnMachinePara()
{
	CString szParamName;

	m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["m_lCalibPRDelay"]					= m_lCalibPRDelay;
	m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["m_lPRErrorLimit"]					= m_lPRErrorLimit;	//20141003

	// COR
	

	for (INT j = 0; j < MAX_NUM_OF_TA4_COR; j++)
	{
		szParamName.Format("m_bCORCalibrated[%ld]", j);
		m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName]			= m_bCORCalibrated[j];
		for (INT i = 0; i < COR_ARRAY_SIZE; i++)
		{
			szParamName.Format("m_dmtrTableCOR_Pos[%ld][%ld].x", j, i);
			m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName]					= m_dmtrTableCOR_Pos[j][i].x;
			szParamName.Format("m_dmtrTableCOR_Pos[%ld][%ld].y", j, i);
			m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName]				= m_dmtrTableCOR_Pos[j][i].y;
			szParamName.Format("m_dmtrTableCOR_Neg[%ld][%ld].x", j, i);
			m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName]				= m_dmtrTableCOR_Neg[j][i].x;
			szParamName.Format("m_dmtrTableCOR_Neg[%ld][%ld].y", j, i);
			m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName]				= m_dmtrTableCOR_Neg[j][i].y;
			szParamName.Format("m_dCORLUT[%ld][%ld]", j, i);
			m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName]				= m_dCORLUT[j][i];
		}
	}
	return 1;
}

LONG CPRTaskStn::IPC_LoadPRTaskStnMachinePara()
{
	CString szParamName;

	m_lCalibPRDelay			= m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["m_lCalibPRDelay"];
	m_lPRErrorLimit			= m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["m_lPRErrorLimit"];

	// COR

	for (INT j = 0; j < MAX_NUM_OF_TA4_COR; j++)
	{
		szParamName.Format("m_bCORCalibrated[%ld]", j);
		m_bCORCalibrated[j]		= (BOOL)(LONG)m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName];

		for (INT i = 0; i < COR_ARRAY_SIZE; i++)
		{
			szParamName.Format("m_dmtrTableCOR_Pos[%ld][%ld].x", j, i);
			m_dmtrTableCOR_Pos[j][i].x		= m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName];
			szParamName.Format("m_dmtrTableCOR_Pos[%ld][%ld].y", j, i);
			m_dmtrTableCOR_Pos[j][i].y		= m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName];
			szParamName.Format("m_dmtrTableCOR_Neg[%ld][%ld].x", j, i);
			m_dmtrTableCOR_Neg[j][i].x		= m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName];
			szParamName.Format("m_dmtrTableCOR_Neg[%ld][%ld].y", j, i);
			m_dmtrTableCOR_Neg[j][i].y		= m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName];
			szParamName.Format("m_dCORLUT[%ld][%ld]", j, i);
			m_dCORLUT[j][i]				= m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName];
		}
	}
	return 1;
}


LONG CPRTaskStn::IPC_SavePRTaskStnDevicePara()
{
	m_pAppMod->m_smfDevice[m_szPRTaskStnName]["PRTaskStn"]["m_lPRRetryLimit"]			= m_lPRRetryLimit;
	m_pAppMod->m_smfDevice[m_szPRTaskStnName]["PRTaskStn"]["m_lRejectGlassLimit"]		= m_lRejectGlassLimit;

	m_pAppMod->m_smfDevice[m_szPRTaskStnName]["PRTaskStn"]["m_lPRDelay"]		= m_lPRDelay;
	m_pAppMod->m_smfDevice[m_szPRTaskStnName]["PRTaskStn"]["m_lPRDelay2"]	= m_lPRDelay2; //20150323
	
	return 1;
}	

LONG CPRTaskStn::IPC_LoadPRTaskStnDevicePara()
{
	m_lPRRetryLimit		= m_pAppMod->m_smfDevice[m_szPRTaskStnName]["PRTaskStn"]["m_lPRRetryLimit"];
	m_lRejectGlassLimit = m_pAppMod->m_smfDevice[m_szPRTaskStnName]["PRTaskStn"]["m_lRejectGlassLimit"];

	m_lPRDelay			= m_pAppMod->m_smfDevice[m_szPRTaskStnName]["PRTaskStn"]["m_lPRDelay"];
	m_lPRDelay2			= m_pAppMod->m_smfDevice[m_szPRTaskStnName]["PRTaskStn"]["m_lPRDelay2"]; //20150323
	
	return 1;
}	

LONG CPRTaskStn::IPC_SaveMachineRecord()
{

	return 1;
}

LONG CPRTaskStn::IPC_LoadMachineRecord()
{
	return 1;
}

LONG CPRTaskStn::IPC_SaveDeviceRecord()
{
	return 1;
}

LONG CPRTaskStn::IPC_LoadDeviceRecord()
{
	return 1;
}

#ifdef EXCEL_MACHINE_DEVICE_INFO
LONG CPRTaskStn::printPRTaskStnMachinePara()
{
	//CAC9000App *pAppMod = dynamic_cast<CAC9000App*> (m_pModule);
	BOOL bResult = TRUE;
	CString szBlank("--");
	//CString szModule = GetStnName();
	LONG lValue = 0;
	DOUBLE dValue = 0.0;
	CString szParamName;

	if (bResult)
	{
		lValue = (LONG) m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["m_lCalibPRDelay"];
		bResult = printMachineInfoRow(m_szPRTaskStnName, CString("PRTaskStn"), CString("m_lCalibPRDelay"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		lValue = (LONG) m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["m_lPRErrorLimit"];
		bResult = printMachineInfoRow(m_szPRTaskStnName, CString("PRTaskStn"), CString("m_lPRErrorLimit"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	// COR
	for(INT j = 0; j < MAX_NUM_OF_TA4_COR; j++)
	{
		if (bResult)
		{
			szParamName.Format("m_bCORCalibrated[%ld]", j);
			lValue = (LONG) m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName];
			bResult = printMachineInfoRow(m_szPRTaskStnName, CString("PRTaskStn"), CString("COR"), szParamName, szBlank,
				szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
		}

		for (INT i = 0; i < COR_ARRAY_SIZE; i++)
		{
			szParamName.Format("m_dmtrTableCOR_Pos[%ld][%ld].x", j, i);
			dValue = (DOUBLE) m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName];
			bResult = printMachineInfoRow(m_szPRTaskStnName, CString("PRTaskStn"), CString("COR"), szParamName, szBlank,
				szBlank, szBlank, szBlank, szBlank, szBlank, dValue);
			if (!bResult)
			{
				break;
			}

			szParamName.Format("m_dmtrTableCOR_Pos[%ld][%ld].y", j, i);
			dValue = (DOUBLE) m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName];
			bResult = printMachineInfoRow(m_szPRTaskStnName, CString("PRTaskStn"), CString("COR"), szParamName, szBlank,
				szBlank, szBlank, szBlank, szBlank, szBlank, dValue);
			if (!bResult)
			{
				break;
			}

			szParamName.Format("m_dmtrTableCOR_Neg[%ld][%ld].x", j, i);
			dValue = (DOUBLE) m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName];
			bResult = printMachineInfoRow(m_szPRTaskStnName, CString("PRTaskStn"), CString("COR"), szParamName, szBlank,
				szBlank, szBlank, szBlank, szBlank, szBlank, dValue);
			if (!bResult)
			{
				break;
			}

			szParamName.Format("m_dmtrTableCOR_Neg[%ld][%ld].y", j, i);
			dValue = (DOUBLE) m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName];
			bResult = printMachineInfoRow(m_szPRTaskStnName, CString("PRTaskStn"), CString("COR"), szParamName, szBlank,
				szBlank, szBlank, szBlank, szBlank, szBlank, dValue);
			if (!bResult)
			{
				break;
			}

			szParamName.Format("m_dCORLUT[%ld][%ld]", j, i);
			dValue = (DOUBLE) m_pAppMod->m_smfMachine[m_szPRTaskStnName]["PRTaskStn"]["COR"][szParamName];
			bResult = printMachineInfoRow(m_szPRTaskStnName, CString("PRTaskStn"), CString("COR"), szParamName, szBlank,
				szBlank, szBlank, szBlank, szBlank, szBlank, dValue);
			if (!bResult)
			{
				break;
			}
		}
	}
	return bResult;
}
#endif
VOID CPRTaskStn::SetAndLogPRParameter(const CString &szFunction, const CString &szParameter, DOUBLE &dParameter, DOUBLE dNewValue)	// 20140729 Yip
{
	CString szOldValue;
	szOldValue.Format("Value = %.5f", dParameter);

	dParameter = dNewValue;

	CString szNewValue;
	szNewValue.Format("Value = %.5f", dNewValue);

	LogPRParameter(szFunction, szParameter, szOldValue, szNewValue);
}


VOID CPRTaskStn::SetAndLogPRParameter(const CString &szFunction, const CString &szParameter, LONG &lParameter, LONG lNewValue)	// 20140729 Yip
{
	CString szOldValue;
	szOldValue.Format("Value = %ld", lParameter);

	lParameter = lNewValue;

	CString szNewValue;
	szNewValue.Format("Value = %ld", lNewValue);

	LogPRParameter(szFunction, szParameter, szOldValue, szNewValue);
}

VOID CPRTaskStn::LogPRParameter(const CString &szStation, const CString &szFunction, const CString &szParameter, const CString &szOldValue, const CString &szNewValue)	// 20140729 Yip
{
	LogAction(szFunction);
	LogAction("%s\t%s", szParameter, szOldValue);
	LogAction("%s\t%s", szParameter, szNewValue);
	theApp.LogParameter(szStation, szFunction, szParameter, szOldValue, szNewValue);
}

VOID CPRTaskStn::LogPRParameter(const CString &szFunction, const CString &szParameter, const CString &szOldValue, const CString &szNewValue)	// 20140729 Yip
{
	SFM_CStation *pGF = (SFM_CStation*)(void*)this; // grand-grand-father
	LogPRParameter(pGF->GetName(), szFunction, szParameter, szOldValue, szNewValue);
}

BOOL CPRTaskStn::AutoTest(PRU *pPRU)
{
	CWinEagle *pCWinEagle = (CWinEagle*)m_pAppMod->GetStation("WinEagle");

	CString szMsg;
	MTR_POSN mtr_offset;
	BOOL bResult		= TRUE;
	PRS_DisplayVideo(pPRU);
	int count	= 4;
	int i;
	FLOAT fTargetPointX[4], fTargetPointY[4];
	CString szMoveOut[4], szMoveIn[4];

	if (!m_bPRSelected)
	{
		szMsg.Format("%s PR Module Not Selected. Operation Abort!", m_szPRTaskStnName);
		m_pAppMod->HMIMessageBox(MSG_OK, "PR AUTO TEST OPERATION", szMsg);
		return FALSE;
	}

	m_pAppMod->WriteHMIMess("Starting Auto Test...");
				
	//Movement 
	PRS_ClearScreen(pPRU->nCamera);


	MTR_POSN mtr_oldPosn;
	PR_RCOORD pr_diesize;
	PR_RCOORD pr_coord;
	pr_diesize.x	= 0.0;
	pr_diesize.y	= 0.0;
	pr_coord.x		= 0.0;
	pr_coord.y		= 0.0;
	FLOAT errorX		= 0.0;
	FLOAT errorY		= 0.0; 
	FLOAT	targetX		= 0.0;
	FLOAT	targetY		= 0.0;

	pr_diesize.x = pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].x - pPRU->arcoLrnDieCorners[PR_UPPER_LEFT].x;
	pr_diesize.y = pPRU->arcoLrnDieCorners[PR_LOWER_RIGHT].y - pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].y;
	//switch (pPRU->emRecordType)
	//{
	//// Pattern Matching or Edge Matching
	//case PRS_DIE_TYPE:
	//	pr_diesize.x = pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].x - pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].x;
	//	pr_diesize.y = pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].y - pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].y;
	//	break;

	//// Shape Fitting
	//case PRS_SHAPE_TYPE:
	//	pr_diesize.x = pPRU->stLrnShapeCmd.stEncRect.coCorner2.x - pPRU->stLrnShapeCmd.stEncRect.coCorner1.x;
	//	pr_diesize.y = pPRU->stLrnShapeCmd.stEncRect.coCorner2.y - pPRU->stLrnShapeCmd.stEncRect.coCorner1.y;
	//	break;

	//// Tmpl Fitting
	//case PRS_TMPL_TYPE:
	//	pr_diesize.x = pPRU->stLoadTmplCmd.szTmplSize.x * 2;
	//	pr_diesize.y = pPRU->stLoadTmplCmd.szTmplSize.y * 2;
	//	break;
	//}

	//Excel file 
	BOOL bOpDone	= TRUE;
	LONG m_lCaliInfoRowNum	= 0;
	INT  nCol				= 0;
	const CString  szDestPath = _T("D:\\sw\\AC9000\\Data\\AutoTest\\");
	CString szTime = _T(""), szFileName = _T("");
	
	SYSTEMTIME ltime;		// New Command

	// Get time as 64-bit integer.
	GetLocalTime(&ltime);

	// Convert to local time.
	szTime.Format("%u-%u-%u %u:%u:%u\t", 
				  ltime.wYear, ltime.wMonth, ltime.wDay, 
				  ltime.wHour, ltime.wMinute, ltime.wSecond);

	
	
	szFileName = szDestPath + CString("AC9000_PRCaliInfo_") + m_szPRTaskStnName + CString(".csv");
	FILE *fp = fopen(szFileName, "a+");

	do
	{
		fTargetPointX[0] = ((PR_MAX_COORD + 1) / 8 * 7 - pr_diesize.x / 2);
		fTargetPointX[1] = ((PR_MAX_COORD + 1) / 8 * 1 + pr_diesize.x / 2);
		fTargetPointX[2] = ((PR_MAX_COORD + 1) / 8 * 1 + pr_diesize.x / 2);
		fTargetPointX[3] = ((PR_MAX_COORD + 1) / 8 * 7 - pr_diesize.x / 2);

		fTargetPointY[0] = ((PR_MAX_COORD + 1) / 8 * 7 - pr_diesize.x / 2);
		fTargetPointY[1] = ((PR_MAX_COORD + 1) / 8 * 7 - pr_diesize.x / 2);
		fTargetPointY[2] = ((PR_MAX_COORD + 1) / 8 * 1 + pr_diesize.x / 2);
		fTargetPointY[3] = ((PR_MAX_COORD + 1) / 8 * 1 + pr_diesize.x / 2);

		szMoveOut[0] = "Move to Upper-Left Point";
		szMoveOut[1] = "Move to Upper-Right Point";
		szMoveOut[2] = "Move to Lower-Right Point";
		szMoveOut[3] = "Move to Lower-Left Point";

		szMoveIn[0] = "Move back from Upper-Left Point";
		szMoveIn[1] = "Move back from Upper-Right Point";
		szMoveIn[2] = "Move back from Lower-Right Point";
		szMoveIn[3] = "Move back from Lower-Left Point";
		
		

		DOUBLE thetaX	= 180 * atan(pPRU->p2m_xform.b / pPRU->p2m_xform.a) / 3.1415926;
		DOUBLE thetaY	= 180 * atan(-1 * (pPRU->p2m_xform.c / pPRU->p2m_xform.d)) / 3.1415926;

		if (fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			fprintf(fp, "%s,PR Transform Angle X (degree): %.3f,PR Transform Angle Y (degree): %.3f,Difference (Theta Y-Theta X): %.3f,\n", (const char*)szTime, thetaX, thetaY, (thetaY - thetaX)); 
			fprintf(fp, "%s,Movement,Target Point X:,Target Point Y:,Before Move PR X:,Before Move PR Y:,Before Move Motor Posn X:,Before Move Motor Posn Y:,After Move PR X:,After Move PR Y:,After Move Motor Posn X:,After Move Motor Posn Y:,POSN X:,POSN Y:,Away from Target Point X:,Away from Target Point Y:\n", (const char*)szTime); 
			//fclose(fp);
		}											 
														
// back to origin
		if (!SearchPRCentre(pPRU))
		{
			bResult = FALSE;
			break;
		}
		

		mtr_oldPosn.x = GetMotorX().GetEncPosn();
		mtr_oldPosn.y = GetMotorY().GetEncPosn();

		

		for (i = 0; i < count; i++)
		{
			//Move Out
			PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);
			szMsg.Format("Move to Out Point");
			m_pAppMod->WriteHMIMess(szMsg);
			szMsg.Format("BEFORE MOVE: PR XY: [%.2f,%.2f]", pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y);
			m_pAppMod->WriteHMIMess(szMsg);
			pr_coord.x = fTargetPointX[i];
			pr_coord.y = fTargetPointY[i];
			PRS_PRPosToMtrOffset(pPRU, pr_coord, &mtr_offset);
			targetX	= PR_MAX_COORD + 1 - pr_coord.x;
			targetY	= PR_MAX_COORD + 1 - pr_coord.y;

			if (fp != NULL)
			{
				fseek(fp, 0, SEEK_END);
				fprintf(fp, "%s,%s,%.2f,%.2f,%.2f,%.2f,%ld,%ld,", (const char*)szTime, (const char*)szMoveOut[i], targetX, targetY, pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, mtr_oldPosn.x, mtr_oldPosn.y); 
			}
			//Move
		
			if (
				(MoveAbsoluteXY(mtr_oldPosn.x, mtr_oldPosn.y, GMP_WAIT) != GMP_SUCCESS) || 
				SleepWithCalibDelay() != GMP_SUCCESS || 
				(MoveAbsoluteXY(mtr_oldPosn.x + mtr_offset.x, mtr_oldPosn.y + mtr_offset.y, GMP_WAIT) != GMP_SUCCESS) 
			   )
			{
				bResult = FALSE;
				break;
			}
	
			Sleep(m_lCalibPRDelay);

			PRS_ClearScreen(pPRU->nCamera);

			if (!SearchPR(pPRU))
			{
				bResult = FALSE;
				break;
			}

			PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);


			mtr_oldPosn.x = GetMotorX().GetEncPosn();
			mtr_oldPosn.y = GetMotorY().GetEncPosn();


			szMsg.Format("AFTER MOVE: PR XY: [%.2f,%.2f]", pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y);
			m_pAppMod->WriteHMIMess(szMsg);

			errorX = pPRU->rcoDieCentre.x - targetX;
			errorY = pPRU->rcoDieCentre.y - targetY;

			pr_coord.x = errorX + PR_DEF_CENTRE_X;
			pr_coord.y = errorY + PR_DEF_CENTRE_Y;

			PRS_PRPosToMtrOffset(pPRU, pr_coord, &mtr_offset);

			szMsg.Format("Away From Upper-Left Point (PR) : [%.2f, %.2f], (POSN): [%ld,%ld]", errorX, errorY, mtr_offset.x, mtr_offset.y);
			m_pAppMod->WriteHMIMess(szMsg);

			if (fp != NULL)
			{
				fseek(fp, 0, SEEK_END);
				fprintf(fp, "%.2f,%.2f,%ld,%ld,%ld,%ld,%.2f,%.2f\n", pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, mtr_oldPosn.x, mtr_oldPosn.y, mtr_offset.x, mtr_offset.y, errorX, errorY); 
			}																															

			if (abs((INT)errorX) > pCWinEagle->HMI_lErrorLimit || abs((INT)errorY) > pCWinEagle->HMI_lErrorLimit)
			{
				szMsg.Format("%s, Error Is Out Of Range", szMoveOut[i]);
				m_pAppMod->WriteHMIMess(szMsg);
			}

			// Move In
			// back to origin
			if (!SearchPRCentre(pPRU))
			{
				bResult = FALSE;
				break;
			}


			szMsg.Format("Move Back from Out Point");
			m_pAppMod->WriteHMIMess(szMsg);
			szMsg.Format("BEFORE MOVE: PR XY: [%.2f,%.2f]", pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y);
			m_pAppMod->WriteHMIMess(szMsg);

			if (fp != NULL)
			{
				fseek(fp, 0, SEEK_END);
				fprintf(fp, "%s,%s,%d,%d,%.2f,%.2f,%ld,%ld,", (const char*)szTime, (const char*)szMoveIn[i], PR_DEF_CENTRE_X, PR_DEF_CENTRE_Y, pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, mtr_oldPosn.x, mtr_oldPosn.y); 
			}

			Sleep(m_lCalibPRDelay);

			PRS_ClearScreen(pPRU->nCamera);
			PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);

			if (!SearchPR(pPRU))
			{
				bResult = FALSE;
				break;
			}
			PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

			szMsg.Format("AFTER MOVE: PR XY: [%.2f,%.2f]", pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y);
			m_pAppMod->WriteHMIMess(szMsg);

			PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtr_offset);
			errorX = pPRU->rcoDieCentre.x - PR_DEF_CENTRE_X;
			errorY = pPRU->rcoDieCentre.y - PR_DEF_CENTRE_Y;

			
			mtr_oldPosn.x = GetMotorX().GetEncPosn();
			mtr_oldPosn.y = GetMotorY().GetEncPosn();
			

			szMsg.Format("Away From Center (PR):[%.2f,%.2f], (POSN): [%ld,%ld]", errorX, errorY, mtr_offset.x, mtr_offset.y);
			m_pAppMod->WriteHMIMess(szMsg);

			if (fp != NULL)
			{
				fseek(fp, 0, SEEK_END);
				fprintf(fp, "%.2f,%.2f,%ld,%ld,%ld,%ld,%.2f,%.2f\n", pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y, mtr_oldPosn.x, mtr_oldPosn.y, mtr_offset.x, mtr_offset.y, errorX, errorY); 
			}	

			if (abs((INT)errorX) > pCWinEagle->HMI_lErrorLimit || abs((INT)errorY) > pCWinEagle->HMI_lErrorLimit)
			{
				szMsg.Format("%s, Error Is Out Of Range", szMoveIn[i]);
				m_pAppMod->WriteHMIMess(szMsg);
			}

		}

	} while (0);

	//if (!bOpDone)
	//{
	//	m_pAppMod->HMIMessageBox(MSG_OK, "Calibration file fail", "Calibration file might be opened, please close it and try again.");
	//	//bOpDone = fo.Delete(szExcelTemplateDestPath + szExcelTemplateFileName);
	//	return FALSE;
	//}
	//else
	//{
		if (fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			fprintf(fp, "\n"); 
			fclose(fp);
		}
		return TRUE;
	//}
}

BOOL CPRTaskStn::CORAutoTest(PRU *pPRU, LONG lAutoTestCount, BOOL bUseRotaryEnc, BOOL bInvertAngle)
{
	CWinEagle *pCWinEagle = (CWinEagle*)m_pAppMod->GetStation("WinEagle");
	CHouseKeeping *pCHouseKeeping = (CHouseKeeping*)m_pAppMod->GetStation("HouseKeeping");
	CString szMsg = "";
	MTR_POSN mtrCurPosn;
	MTR_POSN mtrResultPosn;
	MTR_POSN mtrOffset;
	BOOL bResult = TRUE;
	DOUBLE dAng[8] = {-5.0, -3.0, -1.0, -0.5, 0.5, 1.0, 3.0, 5.0};
	DOUBLE dTestAng;
	LONG lErrorX[100] = {0};
	LONG lErrorY[100] = {0};
	LONG  lEminX(0), lEmaxX(0), lEminY(0), lEmaxY(0), lErrorSumX(0), lErrorSumY(0);
	DOUBLE dStDevX(0), dStDevY(0), dAverageX(0), dAverageY(0);
	double sx(0), sy(0);
	LONG lAnswer = rMSG_TIMEOUT;
	//LONG lAutoTestCount = 3;
	LONG lAngCount = 8;
	LONG i , j;

	CString str;

	PRS_DisplayVideo(pPRU);


	if (!m_bPRSelected)
	{
		szMsg.Format("%s PR Module Not Selected. Operation Abort!" , m_szPRTaskStnName);
		m_pAppMod->HMIMessageBox(MSG_OK, "COR AUTO TEST OPERATION", szMsg);
		return FALSE;
	}

	if (!m_bCORCalibrated)
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "COR AUTO TEST OPERATION", "This Module have not been Calibrated, Please calib first and retry");
		return FALSE;
	}

	//if (pPRU == &pruACF_WHCalib )
	//{
	//	m_szPRTaskStnName = "ACF";
	//}
	//else
	//{
	//	m_szPRTaskStnName = "Unknown_PRTask";
	//	return FALSE;
	//}

	//Move to Calib Posn 
	//Move to the Calib Posn with T is standby Posn
	if (
		MoveTToStandby() != GMP_SUCCESS ||
		MoveXYToPRCalibPosn() != GMP_SUCCESS ||
		MoveZToPRCalibLevel() != GMP_SUCCESS
	   )
	{
		return FALSE;
	}
	//Excel file 
	BOOL bOpDone	= TRUE;
	//const CString szExcelTemplateSrcPath = _T("D:\\sw\\AC9000\\Param\\"), szExcelTemplateDestPath = _T("D:\\sw\\FX9000\\Data\\"), szExcelTemplateFileName = _T("FX9000_Info_Template.xls");
	const CString  szDestPath = _T("D:\\sw\\AC9000\\Data\\AutoTest\\");
	CString szTime = _T(""), szFileName = _T("");
	
	SYSTEMTIME ltime;		// New Command

	// Get time as 64-bit integer.
	GetLocalTime(&ltime);

	// Convert to local time.
	szTime.Format("%u-%u-%u %u:%u:%u\t", 
				  ltime.wYear, ltime.wMonth, ltime.wDay, 
				  ltime.wHour, ltime.wMinute, ltime.wSecond);

	
	
	szFileName = szDestPath + CString("AC9000_CORCaliInfo_") + m_szPRTaskStnName + CString(".csv");
	FILE *fp = fopen(szFileName, "a+");
	do
	{

		if (fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			fprintf(fp, "%s,TestAngle,Away from Centre X(pixel):,Away from Centre Y(pixel):,Away from Centre POSN X:,Away from Centre POSN Y:,Error range X (min):,Error range X (max):,Error range Y (min):,Error range Y (max):,Error SD X:,Error SD Y:,\n", (const char*)szTime); 
			//fclose(fp);
		}	

		for (i = 0; i < lAngCount; i++)
		{
			lErrorSumX	= 0;
			lErrorSumY	= 0;
			dStDevX		= 0.0;
			dStDevY		= 0.0;
			dAverageX	= 0.0;
			dAverageY	= 0.0;

			if (pCHouseKeeping->HMI_bAutoTestStop)
			{
				pCHouseKeeping->HMI_bAutoTestStop	= FALSE;
				m_pAppMod->WriteHMIMess("COR Auto Test Stop!");
				bResult = FALSE;
				break;
			}
			
			dTestAng = dAng[i];


			for (j = 0; j < lAutoTestCount; j++)
			{
				if (pCHouseKeeping->HMI_bAutoTestStop || theApp.getKlocworkFalse())
				{
					pCHouseKeeping->HMI_bAutoTestStop = FALSE;
					m_pAppMod->WriteHMIMess("COR Auto Test Stop!");
					bResult = FALSE;
					break;
				}
		
					//Move to Calib Posn 
					//Move to the Calib Posn with T is standby Posn
				if (
					MoveTToStandby() != GMP_SUCCESS ||
					MoveXYToPRCalibPosn() != GMP_SUCCESS ||
					MoveZToPRCalibLevel() != GMP_SUCCESS
				   )
				{
					bResult = FALSE;
					break;
				}

				Sleep(m_lCalibPRDelay);

				if (!SearchPRCentre(pPRU))
				{
					m_pAppMod->HMIMessageBox(MSG_OK, "COR AUTO TEST OPERATION", "COR Auto Test Failed!");
					PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "PR Failed!");
					m_pAppMod->WriteHMIMess("COR Auto Test SearchPRCentre Failed!");
					bResult = FALSE;
					break;

				}

				PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

				mtrCurPosn.x = GetMotorX().GetEncPosn();
				mtrCurPosn.y = GetMotorY().GetEncPosn();

				Sleep(m_lCalibPRDelay);

				if (!SearchPR(pPRU, FALSE))
				{
					m_pAppMod->HMIMessageBox(MSG_OK, "COR AUTO TEST OPERATION", "COR Auto Test Failed!");
					PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "PR Failed!");
					m_pAppMod->WriteHMIMess(" COR Auto Test SearchPR Failed!");
					bResult = FALSE;
					break;
				}

				LONG lTestAngStep = AngleToCount(dTestAng, bUseRotaryEnc ? GetMotorT().stAttrib.dDistPerCountAux : GetMotorT().stAttrib.dDistPerCount);
				if (
					(bUseRotaryEnc && (MoveRelativeT(lTestAngStep, GMP_WAIT) != GMP_SUCCESS)) ||
					(!bUseRotaryEnc && (GetMotorT().MoveRelative(lTestAngStep, GMP_WAIT) != GMP_SUCCESS))
					)
				{
					m_pAppMod->HMIMessageBox(MSG_OK, "COR AUTO TEST OPERATION", "COR Auto Test Failed!");
					m_pAppMod->WriteHMIMess(" COR Auto Test MoveRelative T Failed!");
					bResult = FALSE;
					break;
				}

				CalPosnAfterRotate(mtrCurPosn, bInvertAngle ? -dTestAng : dTestAng, &mtrResultPosn);

				mtrOffset.x = mtrResultPosn.x - mtrCurPosn.x;
				mtrOffset.y = mtrResultPosn.y - mtrCurPosn.y;



				MoveRelativeXY(mtrOffset.x, mtrOffset.y, GMP_WAIT);
				PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Offset (%ld,%ld)", mtrOffset.x, mtrOffset.y);

				Sleep(m_lCalibPRDelay);
				if (!SearchPR(pPRU, FALSE))
				{
					m_pAppMod->HMIMessageBox(MSG_OK, "COR AUTO TEST OPERATION", "COR Auto Test Failed!");
					PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "PR Failed!");
					m_pAppMod->WriteHMIMess("COR Auto Test SearchPR2 Failed!");
					bResult = FALSE;
					break;
				}


				PRS_DrawCross(pPRU->nCamera, pPRU->rcoDieCentre, PRS_MEDIUM_SIZE, PR_COLOR_YELLOW);

				szMsg.Format("PR XY: [%.2f,%.2f]", pPRU->rcoDieCentre.x, pPRU->rcoDieCentre.y);
				m_pAppMod->WriteHMIMess(szMsg);

				PRS_PRPosToMtrOffset(pPRU, pPRU->rcoDieCentre, &mtrOffset);
				lErrorX[j] = (LONG)(pPRU->rcoDieCentre.x - PR_DEF_CENTRE_X);
				lErrorY[j] = (LONG)(pPRU->rcoDieCentre.y - PR_DEF_CENTRE_Y);

				szMsg.Format("Away From Centre (PR): [%ld, %ld] (POSN): [%ld,%ld]", lErrorX[j], lErrorY[j], mtrOffset.x, mtrOffset.y);
				m_pAppMod->WriteHMIMess(szMsg);

				lErrorX[j] = mtrOffset.x;
				lErrorY[j] = mtrOffset.y;

				lErrorSumX += lErrorX[j];
				lErrorSumY += lErrorY[j];

				if (fp != NULL)
				{
					fseek(fp, 0, SEEK_END);
					fprintf(fp, "%s,%.3f,%ld,%ld,%ld,%ld,", (const char*)szTime, dTestAng, lErrorX[j], lErrorY[j], mtrOffset.x, mtrOffset.y); 
					if (j != (lAutoTestCount - 1))
					{
						fseek(fp, 0, SEEK_END);
						fprintf(fp, "\n");
					}
				}

				PRS_ClearScreen(pPRU->nCamera);

				if (abs(lErrorX[j]) > pCWinEagle->HMI_lErrorLimit || abs(lErrorY[j]) > pCWinEagle->HMI_lErrorLimit)
				{
					szMsg.Format(" Test Ang:%.1lf,  Error Is Out Of Range", dTestAng);
					m_pAppMod->WriteHMIMess(szMsg);

				}
			}
			if (!bResult) 
			{
				break;
			}

//			m_lCaliInfoRowNum--;
			dAverageX = ((double)lErrorSumX) / lAutoTestCount;
			dAverageY = ((double)lErrorSumY) / lAutoTestCount;
			lEminX = lErrorX[0];
			lEmaxX = lErrorX[0];
			lEminY = lErrorY[0];
			lEmaxY = lErrorY[0];

			for (j = 0; j < lAutoTestCount; j++)
			{
					
				lEminX = lEminX < lErrorX[j] ? lEminX : lErrorX[j];
				lEminY = lEminY < lErrorY[j] ? lEminY : lErrorY[j];
				lEmaxX = lEmaxX < lErrorX[j] ? lErrorX[j] : lEmaxX ;
				lEmaxY = lEmaxY < lErrorY[j] ? lErrorY[j] : lEmaxY ;

				sx += (lErrorX[j] - dAverageX) * (lErrorX[j] - dAverageX);
				sy += (lErrorY[j] - dAverageY) * (lErrorY[j] - dAverageY);
			}
			dStDevX = sqrt(sx / lAutoTestCount);
			dStDevY = sqrt(sy / lAutoTestCount);

			if (fp != NULL)
			{
				fseek(fp, 0, SEEK_END);
				fprintf(fp, "%ld,%ld,%ld,%ld,%.5f,%.5f,\n", lEminX, lEmaxX, lEminY, lEmaxY, dStDevX, dStDevY); 

			}
		
		}		
	} while (0);

	if (!bResult)
	{
		if (fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			fprintf(fp, "\n"); 
			fclose(fp);
		}
		m_pAppMod->HMIMessageBox(MSG_OK, "Calibration file Save", "Calibration file Save");
		return FALSE;
	}
	else
	{
		if (fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			fprintf(fp, "\n"); 
			fclose(fp);
		}
		return TRUE;
	}

}

BOOL CPRTaskStn::PRGrabImage(PRU *pPRU) //20150722
{
	BOOL bResult = FALSE;

	bResult = PRS_GrabImageCmd(pPRU);

	return bResult;
}

#if 1 //20160610
INT CPRTaskStn::FindCORByAngle2Point(PRU *pPRU, DOUBLE dAngleRange, BOOL bUseRotaryEnc)
{
	LONG ii = 0, lStartTheta = 0, lCurTheta = 0;
	D_MTR_POSN dmtrCOR = {0.0, 0.0}, dmtrOrigin = {0.0, 0.0}, dmtrStby = {0.0, 0.0};
	std::vector <D_MTR_POSN> vecPosPoint1;
	std::vector <D_MTR_POSN> vecPosPoint2;
	std::vector <D_MTR_POSN> vecNegPoint1;
	std::vector <D_MTR_POSN> vecNegPoint2;
	D_MTR_POSN dmtrCORPosn = {0.0, 0.0};
	D_MTR_POSN dmtrPoint = {0.0, 0.0};
	MTR_POSN OrgPosn = {0, 0};
	DOUBLE dAngleStep = 0.0, dSD1x = 0.0, dSD1y = 0.0, dSD2x = 0.0, dSD2y = 0.0;
	BOOL bResult = TRUE;

	CString szMsg("");

	vecPosPoint1.clear();
	vecNegPoint1.clear();
	vecPosPoint2.clear();
	vecNegPoint2.clear();

	PRS_ClearScreen(pPRU->nCamera);
	lStartTheta = GetStandbyTPosn();

	dAngleStep = dAngleRange / 10.0;

	OrgPosn.x = GetMotorX().GetEncPosn();
	OrgPosn.y = GetMotorY().GetEncPosn();

	if (GetStandbyPoint(pPRU, lStartTheta, bUseRotaryEnc, dmtrStby) != GMP_SUCCESS)
	{
		return GMP_FAIL;
	}

	if (MoveAbsoluteXY(OrgPosn.x, OrgPosn.y, GMP_WAIT) != GMP_SUCCESS)
	{
		return GMP_FAIL;
	}

	//Neg angle
	for (ii = 1; ii <= 10; ii++)
	{
		szMsg.Format("Get Neg Ang Point %ld/10", ii + 1);
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, szMsg);

		lCurTheta = lStartTheta - (ii * AngleToCount(dAngleStep, bUseRotaryEnc ? GetMotorT().stAttrib.dDistPerCountAux : GetMotorT().stAttrib.dDistPerCount));

		if (bResult && GetRotatedPoint(pPRU, lStartTheta, lCurTheta, bUseRotaryEnc, dmtrPoint) != GMP_SUCCESS)
		{
			bResult = FALSE;
		}

		if (bResult && MoveAbsoluteXY(OrgPosn.x, OrgPosn.y, GMP_WAIT) != GMP_SUCCESS)
		{
			bResult = FALSE;
		}

		if (bResult && FindCORWithAngle2Point(dmtrStby, dmtrPoint, (dAngleStep * (DOUBLE) ii), dmtrCORPosn))
		{
			bResult = FALSE;
		}
		vecNegPoint1.push_back(dmtrCORPosn);

		if (bResult && FindCORWithAngle2Point(dmtrStby, dmtrPoint,( -dAngleStep * (DOUBLE) ii), dmtrCORPosn))
		{
			bResult = FALSE;
		}
		vecNegPoint2.push_back(dmtrCORPosn);

		if (!bResult)
		{
			vecPosPoint1.clear();
			vecNegPoint1.clear();
			vecPosPoint2.clear();
			vecNegPoint2.clear();
			return GMP_FAIL;
		}
	}

	//Pos Angle
	for (ii = 1; ii <= 10; ii++)
	{
		szMsg.Format("Get Pos Ang Point %ld/10", ii + 1);
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, szMsg);

		lCurTheta = lStartTheta + (ii * AngleToCount(dAngleStep, bUseRotaryEnc ? GetMotorT().stAttrib.dDistPerCountAux : GetMotorT().stAttrib.dDistPerCount));

		if (bResult && GetRotatedPoint(pPRU, lStartTheta, lCurTheta, bUseRotaryEnc, dmtrPoint) != GMP_SUCCESS)
		{
			bResult = FALSE;
		}

		if (bResult && MoveAbsoluteXY(OrgPosn.x, OrgPosn.y, GMP_WAIT) != GMP_SUCCESS)
		{
			bResult = FALSE;
		}

		if (bResult && FindCORWithAngle2Point(dmtrStby, dmtrPoint, dAngleStep * (DOUBLE) ii, dmtrCORPosn))
		{
			bResult = FALSE;
		}
		vecPosPoint1.push_back(dmtrCORPosn);

		if (bResult && FindCORWithAngle2Point(dmtrStby, dmtrPoint, -dAngleStep * (DOUBLE) ii, dmtrCORPosn))
		{
			bResult = FALSE;
		}
		vecPosPoint2.push_back(dmtrCORPosn);

		if (!bResult)
		{
			vecPosPoint1.clear();
			vecNegPoint1.clear();
			vecPosPoint2.clear();
			vecNegPoint2.clear();
			return GMP_FAIL;
		}
	}

	if (
		!CalVectorSD(vecNegPoint1, dSD1x, dSD1y) ||
		!CalVectorSD(vecNegPoint2, dSD2x, dSD2y)
		)
	{
		vecPosPoint1.clear();
		vecNegPoint1.clear();
		vecPosPoint2.clear();
		vecNegPoint2.clear();
		return GMP_FAIL;
	}

	if (dSD1x * dSD1x + dSD1y * dSD1y > dSD2x * dSD2x + dSD2y * dSD2y)
	{
		//vecNegPoint2 is useful
	}
	else
	{
		//vecNegPoint1 is useful
	}
#if 0
		{
			FILE *fp = NULL;
			SYSTEMTIME ltime;
			GetLocalTime(&ltime);
			CString szFileName;
			szFileName.Format("D:\\sw\\AC9000\\Data\\COR_LeastSqr_%s_%04u%02u%02u.csv", pPRU->szStnName, ltime.wYear, ltime.wMonth, ltime.wDay);

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
					fprintf(fp, "Date: " + szTime + ", dRadius, COR_X, COR_Y, Point1_X, Point1_Y, Point2_X, Point2_Y, Point3_X, Point3_Y, Point4_X, Point4_Y\n");
				}

				fprintf(fp, "%s, %.2f, %.2f\n", (const char*)szTime, dmtrCOR.x, dmtrCOR.y);

				for (ii = 0; ii < (LONG)vecPoint.size(); ii++)
				{
					fprintf(fp, ",%.2f, %.2f\n", vecPoint[ii].x + dmtrOrigin.x, vecPoint[ii].y + dmtrOrigin.y);
				}
				fprintf(fp, "\n");

				fclose(fp);
			}
		}
	vecPoint.clear();
#endif

	PRS_ClearScreen(pPRU->nCamera);
	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	return GMP_SUCCESS;
}

INT CPRTaskStn::GetStandbyPoint(PRU *pPRU, LONG lBaseT, BOOL bUseRotaryEnc, D_MTR_POSN &dmtrResultPoint)
{
	INT nResult = GMP_SUCCESS;
	MTR_POSN OrgPosn = {0, 0};
	LONG lStep = 0;
	CString szMsg;

	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Angle 0");
	if (
		(bUseRotaryEnc && MoveAbsoluteT(lBaseT, GMP_WAIT)  != GMP_SUCCESS) ||
		(!bUseRotaryEnc && GetMotorT().MoveAbsolute(lBaseT, GMP_WAIT)  != GMP_SUCCESS)
		)
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "MotorT move base Failed!");
		theApp.DisplayMessage("Move T base Fail");
		return GMP_FAIL;
	}

	Sleep(m_lCalibPRDelay);
	if (!SearchPRCentre(pPRU))
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "Calib PR Search Failed!");
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Search Failed!");
		nResult = GMP_FAIL;
		return nResult;
	}
	MoveLighting(GMP_WAIT);

	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	Sleep(m_lCalibPRDelay);

	dmtrResultPoint.x = (DOUBLE) GetMotorX().GetEncPosn();
	dmtrResultPoint.y = (DOUBLE) GetMotorY().GetEncPosn();

	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	return nResult;
}

INT CPRTaskStn::GetRotatedPoint(PRU *pPRU, LONG lBaseT, LONG lRotatedT, BOOL bUseRotaryEnc, D_MTR_POSN &dmtrResultPoint)
{
	INT nResult = GMP_SUCCESS;
	MTR_POSN OrgPosn = {0, 0};
	LONG lStep = 0;
	CString szMsg;

	if (
		(bUseRotaryEnc && MoveAbsoluteT(lBaseT, GMP_WAIT)  != GMP_SUCCESS) ||
		(!bUseRotaryEnc && GetMotorT().MoveAbsolute(lBaseT, GMP_WAIT)  != GMP_SUCCESS)
		)
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "MotorT move base Failed!");
		theApp.DisplayMessage("Move T base Fail");
		return GMP_FAIL;
	}

	if (
		(bUseRotaryEnc && MoveAbsoluteT(lRotatedT, GMP_WAIT)  != GMP_SUCCESS) ||
		(!bUseRotaryEnc && GetMotorT().MoveAbsolute(lRotatedT, GMP_WAIT)  != GMP_SUCCESS)
		)
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "MotorT move base Failed!");
		theApp.DisplayMessage("Move T base Fail");
		return GMP_FAIL;
	}

	Sleep(m_lCalibPRDelay);
	if (!SearchPRCentre(pPRU))
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "Calib PR Search Failed!");
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Search Failed!");
		nResult = GMP_FAIL;
		return nResult;
	}
	MoveLighting(GMP_WAIT);

	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	Sleep(m_lCalibPRDelay);

	dmtrResultPoint.x = (DOUBLE) GetMotorX().GetEncPosn();
	dmtrResultPoint.y = (DOUBLE) GetMotorY().GetEncPosn();
	
	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	return nResult;
}
#endif

#if 1 //20160530
INT CPRTaskStn::FindCORByLeaseSqr(PRU *pPRU, DOUBLE dAngleRange, D_MTR_POSN &dmtrCOR_Result, DOUBLE &dRadius, BOOL bUseRotaryEnc)
{
	LONG ii = 0, lStartTheta = 0, lCurTheta = 0;
	D_MTR_POSN dmtrCOR, dmtrOrigin;
	std::vector <D_MTR_POSN> vecPoint;
	D_MTR_POSN dmtrCORPosn = {0.0, 0.0};
	D_MTR_POSN dmtrPoint = {0.0, 0.0};
	const LONG lRun = 31;	//must be odd number
	 DOUBLE dAngleStep = 0.0;

	CString szMsg;

	dmtrCOR.x = 0.0;
	dmtrCOR.y = 0.0;
	dmtrOrigin.x = 0.0;
	dmtrOrigin.y = 0.0;

	vecPoint.clear();

	PRS_ClearScreen(pPRU->nCamera);
	lStartTheta = GetStandbyTPosn();

	dAngleStep = dAngleRange / ((lRun-1) / 2);

	for (ii = -((lRun-1) / 2); ii <= (lRun-1) / 2; ii++)
	{
		szMsg.Format("Get 1 Points %ld/%ld", ii + ((lRun-1) / 2), lRun);
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, szMsg);

		lCurTheta = lStartTheta + (ii * AngleToCount(dAngleStep, bUseRotaryEnc ? GetMotorT().stAttrib.dDistPerCountAux : GetMotorT().stAttrib.dDistPerCount));

		if (Get3LeastSquarePoint(pPRU, lCurTheta, dAngleRange, vecPoint, bUseRotaryEnc) != GMP_SUCCESS)
		{
			return GMP_FAIL;
		}
	}

	dmtrOrigin.x = vecPoint[0].x;
	dmtrOrigin.y = vecPoint[0].y;
	if (!FindLeastSquareCOR(vecPoint, dmtrCOR, dRadius)) //20160530
	{
		vecPoint.clear();
		return GMP_FAIL;
	}
	szMsg.Format("COR:(%.4f, %.4f). R=%.4f", dmtrCOR.x, dmtrCOR.y, dRadius);
	theApp.DisplayMessage(szMsg);
	dmtrCOR_Result.x = dmtrCOR.x;
	dmtrCOR_Result.y = dmtrCOR.y;

#if 1
		{
			FILE *fp = NULL;
			SYSTEMTIME ltime;
			GetLocalTime(&ltime);
			CString szFileName;
			szFileName.Format("D:\\sw\\AC9000\\Data\\COR_LeastSqr_%s_%04u%02u%02u.csv", pPRU->szStnName, ltime.wYear, ltime.wMonth, ltime.wDay);


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
					fprintf(fp, "Date: " + szTime + ", dRadius, COR_X, COR_Y, Point1_X, Point1_Y, Point2_X, Point2_Y, Point3_X, Point3_Y, Point4_X, Point4_Y\n");
				}

				fprintf(fp, "%s, %.2f, %.2f, %.2f\n", (const char*)szTime, dRadius, dmtrCOR.x, dmtrCOR.y);

				for (ii = 0; ii < (LONG)vecPoint.size(); ii++)
				{
					fprintf(fp, ",%.2f, %.2f\n", vecPoint[ii].x + dmtrOrigin.x, vecPoint[ii].y + dmtrOrigin.y);
				}
				fprintf(fp, "\n");

				fclose(fp);
			}
		}
#endif
	vecPoint.clear();

	PRS_ClearScreen(pPRU->nCamera);
	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	return GMP_SUCCESS;
}

//INT CPRTaskStn::Get3LeastSquarePoint(PRU *pPRU, LONG lBaseT, DOUBLE dAngleRange, std::vector <D_MTR_POSN> &vecPoint, BOOL bUseRotaryEnc)
//{
//	INT nResult = GMP_SUCCESS;
//	D_MTR_POSN	dmtrSamplePt1, dmtrSamplePt2, dmtrSamplePt3;
//	MTR_POSN OrgPosn;
//	LONG lStep = 0;
//	CString szMsg;
//
//	dmtrSamplePt1.x = 0.0;
//	dmtrSamplePt2.x = 0.0;
//	dmtrSamplePt3.x = 0.0;
//	dmtrSamplePt1.y = 0.0;
//	dmtrSamplePt2.y = 0.0;
//	dmtrSamplePt3.y = 0.0;
//
////// Point 2 No Angle. Standby Theta
//	OrgPosn.x = GetMotorX().GetEncPosn();
//	OrgPosn.y = GetMotorY().GetEncPosn();
//
//	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Angle 0");
//	if (
//		//MoveT(INWH_T_CTRL_GO_STANDBY, GMP_WAIT) != GMP_SUCCESS
//		(bUseRotaryEnc && MoveAbsoluteT(lBaseT, GMP_WAIT)  != GMP_SUCCESS) ||
//		(!bUseRotaryEnc && GetMotorT().MoveAbsolute(lBaseT, GMP_WAIT)  != GMP_SUCCESS)
//		)
//	{
//		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "MotorT move base Failed!");
//		theApp.DisplayMessage("Move T base Fail");
//		return GMP_FAIL;
//	}
//
//	Sleep(m_lCalibPRDelay);
//	if (!SearchPRCentre(pPRU))
//	{
//		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "Calib PR Search Failed!");
//		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Search Failed!");
//		nResult = GMP_FAIL;
//		return nResult;
//	}
//	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
//	Sleep(1000);
//
//	dmtrSamplePt2.x = (DOUBLE) GetMotorX().GetEncPosn();
//	dmtrSamplePt2.y = (DOUBLE) GetMotorY().GetEncPosn();
//	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
//	//HMIMessageBox(MSG_OK, "FIND COR", "This is sample Point2. (Stby T)");
//
////// Point 1 neg angle
//	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Angle -");
//	if (MoveAbsoluteXY(OrgPosn.x, OrgPosn.y, GMP_WAIT) != GMP_SUCCESS)
//	{
//		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "XY move back Failed!");
//		theApp.DisplayMessage("move back Fail");
//		return GMP_FAIL;
//	}
//	if (
//		//MoveT(INWH_T_CTRL_GO_STANDBY, GMP_WAIT) != GMP_SUCCESS
//		(bUseRotaryEnc && MoveAbsoluteT(lBaseT, GMP_WAIT)  != GMP_SUCCESS) ||
//		(!bUseRotaryEnc && GetMotorT().MoveAbsolute(lBaseT, GMP_WAIT)  != GMP_SUCCESS)
//		)
//	{
//		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "MotorT move BASE Failed!");
//		theApp.DisplayMessage("move T base Fail");
//		return GMP_FAIL;
//	}
//	lStep = AngleToCount(-dAngleRange, bUseRotaryEnc ? GetMotorT().stAttrib.dDistPerCountAux : GetMotorT().stAttrib.dDistPerCount);
//	if (
//		(bUseRotaryEnc && (MoveRelativeT(lStep, GMP_WAIT) != GMP_SUCCESS)) ||
//		(!bUseRotaryEnc && (GetMotorT().MoveRelative(lStep, GMP_WAIT) != GMP_SUCCESS))
//		)
//	{
//		m_pAppMod->HMIMessageBox(MSG_OK, "FIND COR ERROR", "Move T Neg Angle Failed!");
//		nResult = GMP_FAIL;
//		return nResult;
//	}
//	Sleep(m_lCalibPRDelay);
//	if (!SearchPRCentre(pPRU))
//	{
//		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "Calib PR Search Failed!");
//		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Search Failed!");
//		nResult = GMP_FAIL;
//		return nResult;
//	}
//	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
//	Sleep(1000);
//
//	dmtrSamplePt1.x = (DOUBLE) GetMotorX().GetEncPosn();
//	dmtrSamplePt1.y = (DOUBLE) GetMotorY().GetEncPosn();
//	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
//	//HMIMessageBox(MSG_OK, "FIND COR", "This is sample Point1 (Neg Angle)");
//
////// Point 3 Positive angle
//	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Angle +");
//	if (MoveAbsoluteXY(OrgPosn.x, OrgPosn.y, GMP_WAIT) != GMP_SUCCESS)
//	{
//		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "Move back Failed!");
//		theApp.DisplayMessage("move back Fail");
//		return GMP_FAIL;
//	}
//
//	if (
//		//MoveT(INWH_T_CTRL_GO_STANDBY, GMP_WAIT) != GMP_SUCCESS
//		(bUseRotaryEnc && MoveAbsoluteT(lBaseT, GMP_WAIT)  != GMP_SUCCESS) ||
//		(!bUseRotaryEnc && GetMotorT().MoveAbsolute(lBaseT, GMP_WAIT)  != GMP_SUCCESS)
//		)
//	{
//		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "MotorT move Stby Failed!");
//		theApp.DisplayMessage("MotorT Move Stby Fail");
//		return GMP_FAIL;
//	}
//	lStep = AngleToCount(dAngleRange, bUseRotaryEnc ? GetMotorT().stAttrib.dDistPerCountAux : GetMotorT().stAttrib.dDistPerCount);
//	if (
//		(bUseRotaryEnc && (MoveRelativeT(lStep, GMP_WAIT) != GMP_SUCCESS)) ||
//		(!bUseRotaryEnc && (GetMotorT().MoveRelative(lStep, GMP_WAIT) != GMP_SUCCESS))
//		)
//	{
//		m_pAppMod->HMIMessageBox(MSG_OK, "FIND COR ERROR", "Move T Neg Angle Failed!");
//		nResult = GMP_FAIL;
//		return nResult;
//	}
//	Sleep(m_lCalibPRDelay);
//	if (!SearchPRCentre(pPRU))
//	{
//		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "Calib PR Search Failed!");
//		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Search Failed!");
//		nResult = GMP_FAIL;
//		return nResult;
//	}
//	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
//	Sleep(1000);
//
//	dmtrSamplePt3.x = (DOUBLE) GetMotorX().GetEncPosn();
//	dmtrSamplePt3.y = (DOUBLE) GetMotorY().GetEncPosn();
//	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
//	//HMIMessageBox(MSG_OK, "FIND COR", "This is sample Point3 (Pos Angle)");
//
//	//szMsg.Format("FindCOR Pt1:%.3f,%.3f", dmtrSamplePt1.x, dmtrSamplePt1.y);
//	//DisplayMessage(szMsg);
//	//szMsg.Format("FindCOR Pt2:%.3f,%.3f", dmtrSamplePt2.x, dmtrSamplePt2.y);
//	//DisplayMessage(szMsg);
//	//szMsg.Format("FindCOR Pt3:%.3f,%.3f", dmtrSamplePt3.x, dmtrSamplePt3.y);
//	//DisplayMessage(szMsg);
//	
//	if (
//		//MoveT(INWH_T_CTRL_GO_STANDBY, GMP_WAIT) != GMP_SUCCESS ||
//		(bUseRotaryEnc && MoveAbsoluteT(lBaseT, GMP_WAIT)  != GMP_SUCCESS) ||
//		(!bUseRotaryEnc && GetMotorT().MoveAbsolute(lBaseT, GMP_WAIT)  != GMP_SUCCESS) ||
//		MoveAbsoluteXY(OrgPosn.x, OrgPosn.y, GMP_WAIT) != GMP_SUCCESS
//		)
//	{
//		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "MotorT move base Failed!");
//		theApp.DisplayMessage("MotorT move base Fail");
//		return GMP_FAIL;
//	}
//	Sleep(300);
//
//
//	vecPoint.push_back(dmtrSamplePt1);
//	vecPoint.push_back(dmtrSamplePt2);
//	vecPoint.push_back(dmtrSamplePt3);
//	return nResult;
//}

INT CPRTaskStn::Get3LeastSquarePoint(PRU *pPRU, LONG lBaseT, DOUBLE dAngleRange, std::vector <D_MTR_POSN> &vecPoint, BOOL bUseRotaryEnc)
{
	INT nResult = GMP_SUCCESS;
	D_MTR_POSN	dmtrSamplePt1, dmtrSamplePt2, dmtrSamplePt3;
	MTR_POSN OrgPosn;
	LONG lStep = 0;
	CString szMsg;

	dmtrSamplePt1.x = 0.0;
	dmtrSamplePt2.x = 0.0;
	dmtrSamplePt3.x = 0.0;
	dmtrSamplePt1.y = 0.0;
	dmtrSamplePt2.y = 0.0;
	dmtrSamplePt3.y = 0.0;

//// Point 2 No Angle. Standby Theta
	OrgPosn.x = GetMotorX().GetEncPosn();
	OrgPosn.y = GetMotorY().GetEncPosn();

	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Angle 0");
	if (
		//MoveT(INWH_T_CTRL_GO_STANDBY, GMP_WAIT) != GMP_SUCCESS
		(bUseRotaryEnc && MoveAbsoluteT(lBaseT, GMP_WAIT)  != GMP_SUCCESS) ||
		(!bUseRotaryEnc && GetMotorT().MoveAbsolute(lBaseT, GMP_WAIT)  != GMP_SUCCESS)
		)
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "MotorT move base Failed!");
		theApp.DisplayMessage("Move T base Fail");
		return GMP_FAIL;
	}

	Sleep(m_lCalibPRDelay);
	if (!SearchPRCentre(pPRU))
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "Calib PR Search Failed!");
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Search Failed!");
		nResult = GMP_FAIL;
		return nResult;
	}
	MoveLighting(GMP_WAIT);

	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	Sleep(m_lCalibPRDelay);

	dmtrSamplePt1.x = (DOUBLE) GetMotorX().GetEncPosn();
	dmtrSamplePt1.y = (DOUBLE) GetMotorY().GetEncPosn();
	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	//HMIMessageBox(MSG_OK, "FIND COR", "This is sample Point2. (Stby T)");

	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	//HMIMessageBox(MSG_OK, "FIND COR", "This is sample Point3 (Pos Angle)");

	//szMsg.Format("FindCOR Pt1:%.3f,%.3f", dmtrSamplePt1.x, dmtrSamplePt1.y);
	//DisplayMessage(szMsg);
	//szMsg.Format("FindCOR Pt2:%.3f,%.3f", dmtrSamplePt2.x, dmtrSamplePt2.y);
	//DisplayMessage(szMsg);
	//szMsg.Format("FindCOR Pt3:%.3f,%.3f", dmtrSamplePt3.x, dmtrSamplePt3.y);
	//DisplayMessage(szMsg);
	
	if (
		//MoveT(INWH_T_CTRL_GO_STANDBY, GMP_WAIT) != GMP_SUCCESS ||
		(bUseRotaryEnc && MoveAbsoluteT(lBaseT, GMP_WAIT)  != GMP_SUCCESS) ||
		(!bUseRotaryEnc && GetMotorT().MoveAbsolute(lBaseT, GMP_WAIT)  != GMP_SUCCESS) ||
		MoveAbsoluteXY(OrgPosn.x, OrgPosn.y, GMP_WAIT) != GMP_SUCCESS
		)
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "MotorT move base Failed!");
		theApp.DisplayMessage("MotorT move base Fail");
		return GMP_FAIL;
	}
	Sleep(300);

	vecPoint.push_back(dmtrSamplePt1);
	return nResult;
}
#endif

#if 1 //20160525
INT CPRTaskStn::FindCOR(PRU *pPRU, DOUBLE dAngleRange, D_MTR_POSN &dmtrCOR_Result, DOUBLE &dSDx, DOUBLE &dSDy, BOOL bUseRotaryEnc)
{
	// using mid-point, perpendicular lines intersection method
	//dSDx, dSDy are standard deviation
	//CTA1 *pCTA1 = (CTA1*)m_pModule->GetStation("TA1");
	LONG ii = 0, lStartTheta = 0, lCurTheta = 0;
	D_MTR_POSN dmtrCOR, dmtrCOR_Average;
	std::vector <D_MTR_POSN> vtCOR;		//hold all events within 1 hr only
	CString szMsg;

	dmtrCOR.x = 0.0;
	dmtrCOR.y = 0.0;

	PRS_ClearScreen(pPRU->nCamera);
	lStartTheta = GetStandbyTPosn();

	for (ii = -2; ii <= 2; ii++)
	{
		szMsg.Format("COR %ld/5", ii + 3);
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, szMsg);

		lCurTheta = lStartTheta + (ii * AngleToCount(0.1, bUseRotaryEnc ? GetMotorT().stAttrib.dDistPerCountAux : GetMotorT().stAttrib.dDistPerCount));
		if (FindOneCOR(pPRU, lCurTheta, dAngleRange, dmtrCOR, bUseRotaryEnc) != GMP_SUCCESS)
		{
			return GMP_FAIL;
		}
		else
		{
			vtCOR.push_back(dmtrCOR);
		}
	}

	dmtrCOR_Average.x = 0.0;
	dmtrCOR_Average.y = 0.0;

	for (ii=0; ii < (LONG)vtCOR.size(); ii++)
	{
		dmtrCOR_Average.x += vtCOR[ii].x;
		dmtrCOR_Average.y += vtCOR[ii].y;
	}

	if (vtCOR.size() > 0)
	{
		DOUBLE dAccX = 0.0, dAccY = 0.0;
		dmtrCOR_Result.x = dmtrCOR_Average.x / vtCOR.size();
		dmtrCOR_Result.y = dmtrCOR_Average.y / vtCOR.size();

		for (ii=0; ii < (LONG)vtCOR.size(); ii++)
		{
			dAccX += (dmtrCOR_Result.x - vtCOR[ii].x) * (dmtrCOR_Result.x - vtCOR[ii].x);
			dAccY += (dmtrCOR_Result.y - vtCOR[ii].y) * (dmtrCOR_Result.y - vtCOR[ii].y);
		}

		dSDx = sqrt(dAccX / (DOUBLE)vtCOR.size());
		dSDy = sqrt(dAccY / (DOUBLE)vtCOR.size());

#if 1
		{
			FILE *fp = NULL;
			SYSTEMTIME ltime;
			GetLocalTime(&ltime);
			CString szFileName;
			szFileName.Format("D:\\sw\\AC9000\\Data\\COR_%s_%04u%02u%02u.csv", pPRU->szStnName, ltime.wYear, ltime.wMonth, ltime.wDay);


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
					fprintf(fp, "Date: " + szTime + ", COR-2_X, COR-2_Y, COR-1_X, COR-1_Y, COR0_X, COR0_Y, COR1_X, COR1_Y, COR2_X, COR2_Y, average_X, average_Y, SD_X, SD_Y\n");
				}

				fprintf(fp, "%s, %.2f, %.2f, %.2f, %.2f, 	%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n", 
					(const char*)szTime, 
					vtCOR[0].x,
					vtCOR[0].y,
					vtCOR[1].x,
					vtCOR[1].y,
					vtCOR[2].x,
					vtCOR[2].y,
					vtCOR[3].x,
					vtCOR[3].y,
					vtCOR[4].x,
					vtCOR[4].y	,
					dmtrCOR_Result.x,
					dmtrCOR_Result.y,
					dSDx,
					dSDy
					);

				fclose(fp);
			}
		}
#endif

		//if (
		//	CountToDistance(dSDx, pCTA1->m_stMotorX.stAttrib.dDistPerCount) > 200.0 ||
		//	CountToDistance(dSDy, m_stMotorY.stAttrib.dDistPerCount) > 200.0
		//	)
		//{
		//	//vtCOR.clear();
		//	return GMP_FAIL; //SD is too large
		//}



		vtCOR.clear();
		return GMP_SUCCESS;
	}

	//vtCOR.clear();

	PRS_ClearScreen(pPRU->nCamera);
	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	return GMP_FAIL;
}

INT CPRTaskStn::FindOneCOR(PRU *pPRU, LONG lBaseT, DOUBLE dAngleRange, D_MTR_POSN &dmtrCOR_Result, BOOL bUseRotaryEnc)
{
	INT nResult = GMP_SUCCESS;
	D_MTR_POSN	dmtrSamplePt1, dmtrSamplePt2, dmtrSamplePt3;
	MTR_POSN OrgPosn;
	LONG lStep = 0;
	CString szMsg;

	dmtrSamplePt1.x = 0.0;
	dmtrSamplePt2.x = 0.0;
	dmtrSamplePt3.x = 0.0;
	dmtrSamplePt1.y = 0.0;
	dmtrSamplePt2.y = 0.0;
	dmtrSamplePt3.y = 0.0;

//// Point 2 No Angle. Standby Theta
	OrgPosn.x = GetMotorX().GetEncPosn();
	OrgPosn.y = GetMotorY().GetEncPosn();

	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Angle 0");
	if (
		//MoveT(INWH_T_CTRL_GO_STANDBY, GMP_WAIT) != GMP_SUCCESS
		(bUseRotaryEnc && MoveAbsoluteT(lBaseT, GMP_WAIT)  != GMP_SUCCESS) ||
		(!bUseRotaryEnc && GetMotorT().MoveAbsolute(lBaseT, GMP_WAIT)  != GMP_SUCCESS)
		)
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "MotorT move base Failed!");
		theApp.DisplayMessage("Move T base Fail");
		return GMP_FAIL;
	}

	Sleep(m_lCalibPRDelay);
	if (!SearchPRCentre(pPRU))
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "Calib PR Search Failed!");
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Search Failed!");
		nResult = GMP_FAIL;
		return nResult;
	}
	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	Sleep(1000);

	dmtrSamplePt2.x = (DOUBLE) GetMotorX().GetEncPosn();
	dmtrSamplePt2.y = (DOUBLE) GetMotorY().GetEncPosn();
	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	//HMIMessageBox(MSG_OK, "FIND COR", "This is sample Point2. (Stby T)");

//// Point 1 neg angle
	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Angle -");
	if (MoveAbsoluteXY(OrgPosn.x, OrgPosn.y, GMP_WAIT) != GMP_SUCCESS)
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "XY move back Failed!");
		theApp.DisplayMessage("move back Fail");
		return GMP_FAIL;
	}
	if (
		//MoveT(INWH_T_CTRL_GO_STANDBY, GMP_WAIT) != GMP_SUCCESS
		(bUseRotaryEnc && MoveAbsoluteT(lBaseT, GMP_WAIT)  != GMP_SUCCESS) ||
		(!bUseRotaryEnc && GetMotorT().MoveAbsolute(lBaseT, GMP_WAIT)  != GMP_SUCCESS)
		)
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "MotorT move BASE Failed!");
		theApp.DisplayMessage("move T base Fail");
		return GMP_FAIL;
	}
	lStep = AngleToCount(-dAngleRange, bUseRotaryEnc ? GetMotorT().stAttrib.dDistPerCountAux : GetMotorT().stAttrib.dDistPerCount);
	if (
		(bUseRotaryEnc && (MoveRelativeT(lStep, GMP_WAIT) != GMP_SUCCESS)) ||
		(!bUseRotaryEnc && (GetMotorT().MoveRelative(lStep, GMP_WAIT) != GMP_SUCCESS))
		)
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "FIND COR ERROR", "Move T Neg Angle Failed!");
		nResult = GMP_FAIL;
		return nResult;
	}
	Sleep(m_lCalibPRDelay);
	if (!SearchPRCentre(pPRU))
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "Calib PR Search Failed!");
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Search Failed!");
		nResult = GMP_FAIL;
		return nResult;
	}
	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	Sleep(1000);

	dmtrSamplePt1.x = (DOUBLE) GetMotorX().GetEncPosn();
	dmtrSamplePt1.y = (DOUBLE) GetMotorY().GetEncPosn();
	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	//HMIMessageBox(MSG_OK, "FIND COR", "This is sample Point1 (Neg Angle)");

//// Point 3 Positive angle
	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "Angle +");
	if (MoveAbsoluteXY(OrgPosn.x, OrgPosn.y, GMP_WAIT) != GMP_SUCCESS)
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "Move back Failed!");
		theApp.DisplayMessage("move back Fail");
		return GMP_FAIL;
	}

	if (
		//MoveT(INWH_T_CTRL_GO_STANDBY, GMP_WAIT) != GMP_SUCCESS
		(bUseRotaryEnc && MoveAbsoluteT(lBaseT, GMP_WAIT)  != GMP_SUCCESS) ||
		(!bUseRotaryEnc && GetMotorT().MoveAbsolute(lBaseT, GMP_WAIT)  != GMP_SUCCESS)
		)
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "MotorT move Stby Failed!");
		theApp.DisplayMessage("MotorT Move Stby Fail");
		return GMP_FAIL;
	}
	lStep = AngleToCount(dAngleRange, bUseRotaryEnc ? GetMotorT().stAttrib.dDistPerCountAux : GetMotorT().stAttrib.dDistPerCount);
	if (
		(bUseRotaryEnc && (MoveRelativeT(lStep, GMP_WAIT) != GMP_SUCCESS)) ||
		(!bUseRotaryEnc && (GetMotorT().MoveRelative(lStep, GMP_WAIT) != GMP_SUCCESS))
		)
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "FIND COR ERROR", "Move T Neg Angle Failed!");
		nResult = GMP_FAIL;
		return nResult;
	}
	Sleep(m_lCalibPRDelay);
	if (!SearchPRCentre(pPRU))
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "Calib PR Search Failed!");
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Search Failed!");
		nResult = GMP_FAIL;
		return nResult;
	}
	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	Sleep(1000);

	dmtrSamplePt3.x = (DOUBLE) GetMotorX().GetEncPosn();
	dmtrSamplePt3.y = (DOUBLE) GetMotorY().GetEncPosn();
	PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	//HMIMessageBox(MSG_OK, "FIND COR", "This is sample Point3 (Pos Angle)");

	//szMsg.Format("FindCOR Pt1:%.3f,%.3f", dmtrSamplePt1.x, dmtrSamplePt1.y);
	//DisplayMessage(szMsg);
	//szMsg.Format("FindCOR Pt2:%.3f,%.3f", dmtrSamplePt2.x, dmtrSamplePt2.y);
	//DisplayMessage(szMsg);
	//szMsg.Format("FindCOR Pt3:%.3f,%.3f", dmtrSamplePt3.x, dmtrSamplePt3.y);
	//DisplayMessage(szMsg);
	
	if (
		//MoveT(INWH_T_CTRL_GO_STANDBY, GMP_WAIT) != GMP_SUCCESS ||
		(bUseRotaryEnc && MoveAbsoluteT(lBaseT, GMP_WAIT)  != GMP_SUCCESS) ||
		(!bUseRotaryEnc && GetMotorT().MoveAbsolute(lBaseT, GMP_WAIT)  != GMP_SUCCESS) ||
		MoveAbsoluteXY(OrgPosn.x, OrgPosn.y, GMP_WAIT) != GMP_SUCCESS
		)
	{
		m_pAppMod->HMIMessageBox(MSG_OK, "WARNING", "MotorT move base Failed!");
		theApp.DisplayMessage("MotorT move base Fail");
		return GMP_FAIL;
	}
	Sleep(300);

	nResult = CalcCOR(dmtrSamplePt1, dmtrSamplePt2, dmtrSamplePt3, dmtrCOR_Result);
	return nResult;
}

INT CPRTaskStn::CalcCOR(const D_MTR_POSN &dmtrSamplePt1, const D_MTR_POSN &dmtrSamplePt2, const D_MTR_POSN &dmtrSamplePt3, D_MTR_POSN &dmtrCOR_Result)
{
	CString szMsg;
	//D_MTR_POSN dmtrSamplePt1, dmtrSamplePt2, dmtrSamplePt3;

	//dmtrSamplePt1.x = 1.0; dmtrSamplePt1.y = 3.0;
	//dmtrSamplePt2.x = 2.0; dmtrSamplePt2.y = 2.0;
	//dmtrSamplePt3.x = 1.0; dmtrSamplePt3.y = 1.0;

	//dmtrSamplePt1.x = 3.0; dmtrSamplePt1.y = 5.0;
	//dmtrSamplePt2.x = 4.0; dmtrSamplePt2.y = 4.0;
	//dmtrSamplePt3.x = 3.0; dmtrSamplePt3.y = 3.0;

	//dmtrSamplePt1.x = 3.8; dmtrSamplePt1.y = 4.6;
	//dmtrSamplePt2.x = 4.0; dmtrSamplePt2.y = 4.0;
	//dmtrSamplePt3.x = 3.8; dmtrSamplePt3.y = 3.4;

	//dmtrSamplePt1.x = 3.98; dmtrSamplePt1.y = 4.198997;
	//dmtrSamplePt2.x = 4.0; dmtrSamplePt2.y = 4.0;
	//dmtrSamplePt3.x = 3.98; dmtrSamplePt3.y = 3.8010025;

	szMsg.Format("FindCOR Pt1:%.3f,%.3f", dmtrSamplePt1.x, dmtrSamplePt1.y);
	theApp.DisplayMessage(szMsg);
	szMsg.Format("FindCOR Pt2:%.3f,%.3f", dmtrSamplePt2.x, dmtrSamplePt2.y);
	theApp.DisplayMessage(szMsg);
	szMsg.Format("FindCOR Pt3:%.3f,%.3f", dmtrSamplePt3.x, dmtrSamplePt3.y);
	theApp.DisplayMessage(szMsg);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	DOUBLE dC12 = 0.0, dC23 = 0.0; 
	DOUBLE dM12 = 0.0, dM23 = 0.0; 
	D_MTR_POSN dmtrMid12, dmtrMid23; 
	D_MTR_POSN dmtrCOR;
	
	dmtrMid12.x = 0.0;
	dmtrMid12.y = 0.0;
	dmtrMid23.x = 0.0;
	dmtrMid23.y = 0.0;
	dmtrCOR.x = 0.0;
	dmtrCOR.y = 0.0;

	FindCORData(dmtrSamplePt1, dmtrSamplePt2, dC12, dM12, dmtrMid12);
	FindCORData(dmtrSamplePt2, dmtrSamplePt3, dC23, dM23, dmtrMid23);

	if (dM12 - dM23 != 0.0)
	{
		dmtrCOR.x = (dC23 - dC12) / (dM12 - dM23);
	}
	else
	{
		dmtrCOR.x = 0.0;
	}
	dmtrCOR.y = (dM12 * dmtrCOR.x) + dC12;

	szMsg.Format("Result COR=%.4f, %.4f", dmtrCOR.x, dmtrCOR.y);
	theApp.DisplayMessage(szMsg);
	dmtrCOR_Result.x = dmtrCOR.x;
	dmtrCOR_Result.y = dmtrCOR.y;

	return GMP_SUCCESS;
}

VOID CPRTaskStn::FindCORData(D_MTR_POSN dmtrPt1, D_MTR_POSN dmtrPt2, DOUBLE &dC, DOUBLE &dM, D_MTR_POSN &dmtrMid)
{
	dmtrMid.x = (dmtrPt1.x + dmtrPt2.x) / 2.0;
	dmtrMid.y = (dmtrPt1.y + dmtrPt2.y) / 2.0;

	dM = -1.0 / ((dmtrPt2.y - dmtrPt1.y) / (dmtrPt2.x - dmtrPt1.x));

	dC = dmtrMid.y - (dM * dmtrMid.x);
	
	return;
}
#endif

BOOL CPRTaskStn::DistortionCalibration(PRU *pPRU)
{
	BOOL bResult = FALSE;

	bResult = PRS_DistortionCalibration(pPRU);

	return bResult;

}

#if 1 //20170328
INT CPRTaskStn::PRSearchThread(PRU *pPRU, LONG lGlassx, LONG lLSIx, LONG lPRx, LONG lTimeOut, CString &szErrMsg, CAC9000Stn *pStation)
{
		LONG lGlass = 0, lLSI = 0, lPR = 0;
		LONG lCount = lTimeOut; //5sec

		lGlass = lGlassx;
		lLSI = lLSIx;
		lPR = lPRx;
//// Grab image
		if (StartPRGrabThread(pPRU, lGlass, lLSI, lPR, pStation) != GMP_SUCCESS)
		{
			szErrMsg = "PR Grab Thread error";
			return GMP_FAIL;
		}
		while (WaitForSingleObject(m_hPRGrabThread[lGlass][lLSI][lPR], 5 * 1000) == WAIT_TIMEOUT)
		{
			szErrMsg.Format("PR Grab Thread Timeout error. Glass%ld, LSI%ld, PR%ld", lGlass + 1 , lLSI + 1, lPR + 1);
		}
		while (m_bPRGrabThreadRunning[lGlass][lLSI][lPR] == TRUE && lCount > 0) //wait until it finished
		{
			Sleep(20);
			lCount -= 20;
		}
		CloseHandle(m_hPRGrabThread[lGlass][lLSI][lPR]); ////

		if (lCount <= 0)
		{
			szErrMsg = "PR Grab Thread Timeout error";
			return GMP_FAIL;
		}
		else if (!m_bPRGrabThreadResult[lGlass][lLSI][lPR])
		{
			szErrMsg = "PR Grab Thread Result error";
			return GMP_FAIL;
		}

//// Process
		lCount = lTimeOut;
		if (StartPRProcThread(pPRU, lGlass, lLSI, lPR, pStation) != GMP_SUCCESS)
		{
			szErrMsg = "PR Proc Thread error";
			return GMP_FAIL;
		}
		while (WaitForSingleObject(m_hPRProcThread[lGlass][lLSI][lPR], 5 * 1000) == WAIT_TIMEOUT)
		{
			szErrMsg.Format("PR Proc Thread Timeout error. Glass%ld, LSI%ld, PR%ld", lGlass + 1 , lLSI + 1, lPR + 1);
		}
		while (m_bPRProcThreadRunning[lGlass][lLSI][lPR] == TRUE && lCount > 0)
		{
			Sleep(10);
			lCount -= 10;
		}

		CloseHandle(m_hPRProcThread[lGlass][lLSI][lPR]); /////
		if (lCount <= 0)
		{
			szErrMsg = "PR Proc Thread Timeout error";
			return GMP_FAIL;
		}
		else
		{
			if (m_bPRProcThreadResult[lGlass][lLSI][lPR])
			{
				return GMP_SUCCESS;
			}
			else
			{
				szErrMsg = "PR Search Fail";
				return GMP_FAIL;
			}
		}
}

INT CPRTaskStn::StartPRGrabThread(PRU *pPRU, LONG lGlassx, LONG lLSIx, LONG lPRx, CAC9000Stn *pStation)
{
	DWORD dwThreadId = 0;

	m_lThreadGlassx = lGlassx;
	m_lThreadLSIx = lLSIx;
	m_lThreadPRx = lPRx;
	m_pThreadPRU = pPRU;

	m_bPRGrabThreadRunning[lGlassx][lLSIx][lPRx] = TRUE;

	m_hPRGrabThread[lGlassx][lLSIx][lPRx] = ::CreateThread( 
		NULL,						// no security attributes 
		0,							// use default stack size  
		PRGrabThread,				// thread function 
		this,				// argument to thread function 
		0,							// use default creation flags 
		&dwThreadId);				// returns the thread identifier 
	// Check the return value for success. 

	if (m_hPRGrabThread[lGlassx][lLSIx][lPRx] == NULL)
	{
		CString szTmp;
		m_bPRGrabThreadRunning[lGlassx][lLSIx][lPRx] = FALSE;
		if (pStation != NULL)
		{
			szTmp.Format("%s StartPRGrabThread: CreateThread failed.", pStation->GetStnName());
		}
		else
		{
			szTmp.Format("StartPRGrabThread: CreateThread failed.");
		}
		theApp.DisplayMessage(szTmp);
		return GMP_FAIL;
	}
	//else 
	//{
		//LONG lCount = 0;
		//while(!m_bPRGrabThreadRunning[lGlassx][lLSIx][lPRx])
		//{
		//	lCount++;
		//	Sleep(1);
		//	if (lCount > 5000 && !m_bPRGrabThreadRunning[lGlassx][lLSIx][lPRx]) //should be less 1ms
		//	{
		//		CString szTmp;
		//		if (pStation != NULL)
		//		{
		//			szTmp.Format("%s StartPRGrabThread: CreateThread Timeout.", pStation->GetStnName());
		//		}
		//		else
		//		{
		//			szTmp.Format("StartPRGrabThread: CreateThread Timeout.");
		//		}
		//		theApp.DisplayMessage(szTmp);
		//		return GMP_FAIL;
		//	}
		//}
		//CloseHandle(hThread);
	//}
	return GMP_SUCCESS;
}

INT CPRTaskStn::StartPRProcThread(PRU *pPRU, LONG lGlassx, LONG lLSIx, LONG lPRx, CAC9000Stn *pStation)
{
	DWORD dwThreadId = 0;

	m_lThreadGlassx = lGlassx;
	m_lThreadLSIx = lLSIx;
	m_lThreadPRx = lPRx;
	m_pThreadPRU = pPRU;

	m_bPRProcThreadRunning[lGlassx][lLSIx][lPRx] = TRUE;

	m_hPRProcThread[lGlassx][lLSIx][lPRx] = ::CreateThread( 
		NULL,						// no security attributes 
		0,							// use default stack size  
		PRProcThread,				// thread function 
		this,				// argument to thread function 
		0,							// use default creation flags 
		&dwThreadId);				// returns the thread identifier 
	// Check the return value for success. 

	if (m_hPRProcThread[lGlassx][lLSIx][lPRx] == NULL)
	{
		CString szTmp;
		m_bPRProcThreadRunning[lGlassx][lLSIx][lPRx] = FALSE;
		if (pStation != NULL)
		{
			szTmp.Format("%s StartPRProcThread: CreateThread failed.", pStation->GetStnName());
		}
		else
		{
			szTmp.Format("StartPRProcThread: CreateThread failed.");
		}
		theApp.DisplayMessage(szTmp);
		return GMP_FAIL;
	}
	//else 
	//{
		//LONG lCount = 0;
		//while(!m_bPRProcThreadRunning[lGlassx][lLSIx][lPRx])
		//{
		//	lCount++;
		//	Sleep(1);
		//	if (lCount > 5000 && !m_bPRProcThreadRunning[lGlassx][lLSIx][lPRx])
		//	{
		//		CString szTmp;
		//		if (pStation != NULL)
		//		{
		//			szTmp.Format("%s StartPRProcThread: CreateThread Timeout", pStation->GetStnName());
		//		}
		//		else
		//		{
		//			szTmp.Format("StartPRProcThread: CreateThread Timeout");
		//		}
		//		theApp.DisplayMessage(szTmp);
		//		return GMP_FAIL;
		//	}
		//}
		//CloseHandle(hThread);
	//}
	return GMP_SUCCESS;
}

INT CPRTaskStn::StartPRSrchThread(PRU *pPRU, LONG lGlassx, LONG lLSIx, LONG lPRx, CAC9000Stn *pStation)
{
	DWORD dwThreadId = 0;

	m_lThreadGlassx = lGlassx;
	m_lThreadLSIx = lLSIx;
	m_lThreadPRx = lPRx;
	m_pThreadPRU = pPRU;

	m_bPRSrchThreadRunning[lGlassx][lLSIx][lPRx] = TRUE;

	m_hPRSrchThread[lGlassx][lLSIx][lPRx] = ::CreateThread( 
		NULL,						// no security attributes 
		0,							// use default stack size  
		PRSrchThread,				// thread function 
		this,				// argument to thread function 
		0,							// use default creation flags 
		&dwThreadId);				// returns the thread identifier 
	// Check the return value for success. 

	if (m_hPRSrchThread[lGlassx][lLSIx][lPRx] == NULL)
	{
		CString szTmp;
		m_bPRSrchThreadRunning[lGlassx][lLSIx][lPRx] = FALSE;
		if (pStation != NULL)
		{
			szTmp.Format("%s StartPRSrchThread: CreateThread failed.", pStation->GetStnName());
		}
		else
		{
			szTmp.Format("StartPRSrchThread: CreateThread failed.");
		}
		theApp.DisplayMessage(szTmp);
		return GMP_FAIL;
	}
	return GMP_SUCCESS;
}

DWORD WINAPI PRGrabThread(LPVOID lpParam)
{
	CPRTaskStn *pPRTaskStn = NULL;
	PRU *pPRU = NULL;
	LONG lGlass = 0, lLSI = 0, lPR = 0;
	//DWORD dwStartTime = 0;

	pPRTaskStn = (CPRTaskStn *)lpParam;

	pPRU = pPRTaskStn->m_pThreadPRU;
	lGlass = pPRTaskStn->m_lThreadGlassx;
	lLSI = pPRTaskStn->m_lThreadLSIx;
	lPR = pPRTaskStn->m_lThreadPRx;

	//pPRTaskStn->m_bPRGrabThreadRunning[lGlass][lLSI][lPR] = TRUE;
	pPRTaskStn->m_bPRGrabThreadResult[lGlass][lLSI][lPR] = FALSE;

	//dwStartTime = GetTickCount();
	pPRTaskStn->m_bPRGrabThreadResult[lGlass][lLSI][lPR] = PRS_ManualSrchGrabOnly(pPRU);
	//if (labs(GetTickCount() - dwStartTime) < 5)
	//{
	//	Sleep(5);
	//}

	pPRTaskStn->m_bPRGrabThreadRunning[lGlass][lLSI][lPR] = FALSE;
	return 0;
}

DWORD WINAPI PRProcThread(LPVOID lpParam)
{
	CPRTaskStn *pPRTaskStn = NULL;
	PRU *pPRU = NULL;
	LONG lGlass = 0, lLSI = 0, lPR = 0;
	//DWORD dwStartTime = 0;

	pPRTaskStn = (CPRTaskStn *)lpParam;

	pPRU = pPRTaskStn->m_pThreadPRU;
	lGlass = pPRTaskStn->m_lThreadGlassx;
	lLSI = pPRTaskStn->m_lThreadLSIx;
	lPR = pPRTaskStn->m_lThreadPRx;


	//pPRTaskStn->m_bPRProcThreadRunning[lGlass][lLSI][lPR] = TRUE;
	pPRTaskStn->m_bPRProcThreadResult[lGlass][lLSI][lPR] = FALSE;
	
	//dwStartTime = GetTickCount();
	pPRTaskStn->m_bPRProcThreadResult[lGlass][lLSI][lPR] = PRS_ManualSrchProcessOnly(pPRU);
	//if (labs(GetTickCount() - dwStartTime) < 5)
	//{
	//	Sleep(5);
	//}

	pPRTaskStn->m_bPRProcThreadRunning[lGlass][lLSI][lPR] = FALSE;
	return 0;
}

DWORD WINAPI PRSrchThread(LPVOID lpParam)
{
	CPRTaskStn *pPRTaskStn = NULL;
	PRU *pPRU = NULL;
	LONG lGlass = 0, lLSI = 0, lPR = 0;
	//DWORD dwStartTime = 0;

	pPRTaskStn = (CPRTaskStn *)lpParam;

	pPRU = pPRTaskStn->m_pThreadPRU;
	lGlass = pPRTaskStn->m_lThreadGlassx;
	lLSI = pPRTaskStn->m_lThreadLSIx;
	lPR = pPRTaskStn->m_lThreadPRx;

	//pPRTaskStn->m_bPRSrchThreadRunning[lGlass][lLSI][lPR] = TRUE;
	pPRTaskStn->m_bPRSrchThreadResult[lGlass][lLSI][lPR] = FALSE;
	
	//dwStartTime = GetTickCount();
	pPRTaskStn->m_bPRSrchThreadResult[lGlass][lLSI][lPR] = PRS_ManualSrchCmd(pPRU);
	//if (labs(GetTickCount() - dwStartTime) < 5)
	//{
	//	Sleep(5);
	//}

	pPRTaskStn->m_bPRSrchThreadRunning[lGlass][lLSI][lPR] = FALSE;
	return 0;
}
#endif