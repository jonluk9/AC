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

#pragma once

#include "AC9000Stn.h"
#include "math.h"
#include "prheader.h"
#include "PR_Util.h"
#include "Cal_Util.h"
#include "HouseKeeping.h"

#ifdef EXCEL_MACHINE_DEVICE_INFO
#include "ExcelMachineInfo_Util.h"
#endif

#ifdef PRTASKSTN_EXTERN
extern CString	g_szWizardBMPath;
extern BOOL	g_bIsVisionNTInited;
#endif

extern BOOL g_bPRDebugInfo; //20150423

#define NON_CALIB_PR_DELAY	-1		//20150310

typedef enum
{
	BASIC = 0,
	ADVANCE,
	MAX_NUM_OF_PB_METHOD,

} PB_Method;

typedef enum
{
	PR1,
	PR2,
	MAX_NUM_OF_PR,

} PRNum;


class CPRTaskStn
{
	//DECLARE_DYNCREATE(CPRTaskStn)
public:
	CString		m_szPRTaskStnName;
#if 1 //20170328
	HANDLE m_hPRGrabThread[MAX_NUM_OF_GLASS][MAX_NUM_OF_LSI][MAX_NUM_OF_PR];
	HANDLE m_hPRProcThread[MAX_NUM_OF_GLASS][MAX_NUM_OF_LSI][MAX_NUM_OF_PR];
	HANDLE m_hPRSrchThread[MAX_NUM_OF_GLASS][MAX_NUM_OF_LSI][MAX_NUM_OF_PR]; //combine grab and proc

	BOOL m_bPRGrabThreadRunning[MAX_NUM_OF_GLASS][MAX_NUM_OF_LSI][MAX_NUM_OF_PR];
	BOOL m_bPRProcThreadRunning[MAX_NUM_OF_GLASS][MAX_NUM_OF_LSI][MAX_NUM_OF_PR];
	BOOL m_bPRSrchThreadRunning[MAX_NUM_OF_GLASS][MAX_NUM_OF_LSI][MAX_NUM_OF_PR]; //combine grab and proc

	BOOL m_bPRGrabThreadResult[MAX_NUM_OF_GLASS][MAX_NUM_OF_LSI][MAX_NUM_OF_PR];
	BOOL m_bPRProcThreadResult[MAX_NUM_OF_GLASS][MAX_NUM_OF_LSI][MAX_NUM_OF_PR];
	BOOL m_bPRSrchThreadResult[MAX_NUM_OF_GLASS][MAX_NUM_OF_LSI][MAX_NUM_OF_PR]; //combine grab and proc

	LONG m_lThreadGlassx;
	LONG m_lThreadLSIx;
	LONG m_lThreadPRx;
	PRU	*m_pThreadPRU;
#endif
protected:
	CAC9000App *m_pAppMod;

	LONG		m_lPRStnErrorCode;		// IDS_xxxxx_NOT_SELECTED_ERR

	LONG		m_lPRModSelectMode;		

	BOOL		m_bPRSelected;
	BOOL		m_bPRError;

	BOOL		m_bPRCycle;
	ULONG		m_ulPRCycle;

	LONG		m_lAddJobPR_GRAB;
	LONG		m_lAddJobPR_PROCESS;

	LONG		m_lPRRetryCounter;
	LONG		m_lPRRetryLimit;
	LONG		m_lRejectGlassCounter;
	LONG		m_lRejectGlassLimit;
	LONG		m_lPRDelay;
	LONG		m_lPRDelay2; //20150323
	LONG		m_lCalibPRDelay;
	LONG		m_lPRErrorLimit;//20141003

	D_MTR_POSN	m_dmtrCurrPR1Posn;
	D_MTR_POSN	m_dmtrCurrPR2Posn;
#ifdef ENCODER_BASED //use encoder value instead of command motor count
	D_MTR_POSN	m_dmtrCurrPRxAfterPRPosn;
	D_MTR_POSN	m_dmtrCurrPR1BeforePRPosn;
	D_MTR_POSN	m_dmtrCurrPR1AfterPRPosn;
	D_MTR_POSN	m_dmtrCurrPR2BeforePRPosn;
	D_MTR_POSN	m_dmtrCurrPR2AfterPRPosn;
#endif
	D_MTR_POSN	m_dmtrCurrPR1PosnOffset;	// 20150119 For TA4 DL check the posn of glass on MBWH 
	D_MTR_POSN	m_dmtrCurrPR2PosnOffset;
	D_MTR_POSN	m_dmtrCurrCenterPRPosn;	//INWH Glass Center Posn
	
	D_MTR_POSN	m_dmtrCurrPR3Posn;			// 20150109
	D_MTR_POSN	m_dmtrCurrPR4Posn;			// 20150109
	D_MTR_POSN	m_dmtrCurrAlign1Offset;						// current align1 offsets
	D_MTR_POSN	m_dmtrCurrAlign2Offset;						// current align2 offsets
	D_MTR_POSN	m_dmtrCurrAlignCentre;						// current final die centre
	DOUBLE		m_dCurrAlignAngle;							// current final die angle	
	DOUBLE		m_dCurrPreciseAlginAngle;					// Record the Corrected angle in precise mode
	DOUBLE		m_dCurrAlignDist;							// distance btw align pts
	DOUBLE		m_dAlignDistTol;							// alignment Distance tolerance (in mm)
	DOUBLE		m_dAlignAngleTol;							// alignment angle tolerance 

	MTR_POSN	m_mtrCurrPRCalibPosn;
	LONG		m_lCurrPRCalibLevel;

	INT			m_nPRInProgress;

	//PostBond Only
	PR_RCOORD	m_PRCurrPBPR1Corner[PR_NO_OF_CORNERS];
	PR_RCOORD	m_PRCurrPBPR2Corner[PR_NO_OF_CORNERS];

	//Auto Calib parameters
	PR_RCOORD	m_stPRCalibPts[NO_OF_CAL_PT];
	PR_RCOORD	m_rstPRCalibPts[NO_OF_CAL_PT]; //20150728 float PR
	MTR_POSN	m_stMtrCalibPts[NO_OF_CAL_PT];
	MTR_POSN	m_stCORMtrCalibPts[MAX_NUM_OF_TA4_COR][COR_ARRAY_SIZE];
	MTR_POSN	m_mtrThermalCalibRefPosn; //20140627 PBDL calib
	MTR_POSN	m_mtrThermalCalibOffset; //20140707 thermal compensation

	//// COR parameters
	D_MTR_POSN	m_dmtrTableCOR_Pos[MAX_NUM_OF_TA4_COR][COR_ARRAY_SIZE];
	D_MTR_POSN	m_dmtrTableCOR_Neg[MAX_NUM_OF_TA4_COR][COR_ARRAY_SIZE];
	DOUBLE		m_dCORLUT[MAX_NUM_OF_TA4_COR][COR_ARRAY_SIZE];
	BOOL		m_bCORCalibrated[MAX_NUM_OF_TA4_COR];
	DOUBLE		m_dCalibCORRange;
	LONG		m_lCORCamNum;

	ULONG		HMI_ulOldCurPRU;
	ULONG		HMI_ulCurPRU;
	ULONG		HMI_ulCurPBPRU;
	ULONG		HMI_ulPRID;
	DOUBLE		HMI_dPRMatchScore;
	DOUBLE		HMI_dPRAngleRange;
	DOUBLE		HMI_dPRPercentVar;
	BOOL		HMI_bShowPRSearch;
	DOUBLE		HMI_dPostBdRejectTol;
	ULONG		HMI_ulPRPostBdAttrib;
	LONG		HMI_lSegThreshold;
	LONG		HMI_lDefectThreshold;
	LONG		HMI_lPRSearchRange;		//20150107 Polarizer Detect

	// Advance
	LONG		HMI_lACFAlignAlg;		
	LONG		HMI_lACFSegmentAlg;
	LONG		HMI_lACFIsAlign;
	LONG		HMI_lPBMethod;		// 0: Basic; 1: Advance

	// Basic
	LONG		m_lIsAlign;			// 0: NO Align
	LONG		m_lAlignAlg;		// 0:Parallel; 1: Three Line

	BOOL		HMI_bShowSegThreshold;
	BOOL		HMI_bShowPBMethod;
	BOOL		HMI_bShowBasicAlg;
	BOOL		HMI_bShowAdvanceAlg;
	BOOL		HMI_bShowACFAlignAlg;	//IsAlign or not

	CMotorInfo	m_motorDummy; //klocwork fix 20121211
protected:


public:
	CPRTaskStn();
	virtual	~CPRTaskStn();

public:
	VOID SetPRSelected(BOOL bMode);
	BOOL IsPRSelected();
	VOID CheckPRModSelected(BOOL bMode);
	LONG GetPRModSelectMode();

	LONG SleepWithCalibDelay();
private:
	LONG GetPRDelay();
	LONG GetPRDelay2();
	LONG GetPRCalibDelay();

protected:
	BOOL ManualLrnDie_New(PRU *pPRU);	// Vision UI
	VOID SrchPR_New(PRU *pPRU);			// Vision UI
	VOID ModifyPRCriteria(PRU *pPRU);	
	BOOL PRModifyCenter(PRU *pPRU);	//20150108 RGB Pattern Center
	BOOL ManualLrnDie(PRU *pPRU);
	VOID SrchPRDie(PRU *pPRU);
	VOID SetPRDieSrchArea(PRU *pPRU, PR_COORD coCorner1, PR_COORD coCorner2);
	BOOL LearnPRShape(PRU *pPRU);
	VOID DetectPRShape(PRU *pPRU);
	VOID SetPRShapeSrchArea(PRU *pPRU, PR_COORD coCorner1, PR_COORD coCorner2);
	BOOL LoadPRTmpl(PRU *pPRU);
	VOID SrchPRTmpl(PRU *pPRU);
	VOID SetPRTmplSrchArea(PRU *pPRU, PR_COORD coCorner1, PR_COORD coCorner2);
	BOOL LearnPRKerf(PRU *pPRU);	// 20141023 Yip: Add Kerf Fitting Functions
	VOID InspPRKerf(PRU *pPRU);	// 20141023 Yip: Add Kerf Fitting Functions
	VOID SetPRKerfSrchArea(PRU *pPRU, PR_COORD coCorner1, PR_COORD coCorner2);	// 20141023 Yip: Add Kerf Fitting Functions
	BOOL LrnACFCmd(PRU *pPRU, PRU *pPRUBackground);
	BOOL LrnBackgroundCmd(PRU *pPRU, PRU *pPRUBackground);
	VOID SrchACFCmd(PRU *pPRU);

	BOOL SearchPR(PRU *pPRU, BOOL bIsWait = TRUE, LONG lPRDelay = NON_CALIB_PR_DELAY, CAC9000Stn *pStation = NULL);
	BOOL SearchPRGrabOnly(PRU *pPRU);
	BOOL SearchPRProcessOnly(PRU *pPRU);

public:
	BOOL SearchPRCentre(PRU *pPRU, BOOL bIsInvert = FALSE, LONG lPRDelay = NON_CALIB_PR_DELAY);
	VOID SinglePRCycleOperation(PRU *pPRU, DOUBLE dTestAng, DOUBLE dTestX, DOUBLE dTestY, PRINT_DATA *stPrintData);

protected:
	// Calc Related
	VOID CalDPosnAfterRotate(D_MTR_POSN dmtrTableCOR, MTR_POSN mtrCurPosn, DOUBLE dAngle, D_MTR_POSN *p_dmtrResultPosn);
	VOID CalPosnAfterRotate(MTR_POSN mtrCurPosn, DOUBLE dAngle, MTR_POSN *p_mtrResultPosn);

	BOOL CheckPRCentre(PRU *pPRU, BOOL bIsInvert = FALSE); //20120613
	BOOL AutoCalibration(PRU *pPRU);
	BOOL SetThermalCalibRefPosn(PRU *pPRU); //20140627 PBDL calib
	BOOL DistortionCalibration(PRU *pPRU);
#if 1 //20170328
	INT PRSearchThread(PRU *pPRU, LONG lGlassx, LONG lLSIx, LONG lPRx, LONG lTimeOut, CString &szErrMsg, CAC9000Stn *pStation = NULL);
	INT StartPRGrabThread(PRU *pPRU, LONG lGlassx, LONG lLSIx, LONG lPRx, CAC9000Stn *pStation = NULL);
	INT StartPRProcThread(PRU *pPRU, LONG lGlassx, LONG lLSIx, LONG lPRx, CAC9000Stn *pStation = NULL);
	INT StartPRSrchThread(PRU *pPRU, LONG lGlassx, LONG lLSIx, LONG lPRx, CAC9000Stn *pStation = NULL);
#endif
	
	BOOL AutoTest(PRU *pPRU);  //20140923
	BOOL CORAutoTest(PRU *pPRU, LONG lAutoTestCount, BOOL bUseRotaryEnc = FALSE, BOOL bInvertAngle = FALSE);
	BOOL TA4CORTest(PRU *pPRU, LONG lAutoTestCount,  BOOL bUseRotaryEnc = FALSE); //20141120
	BOOL PRGrabImage(PRU *pPRU); //20150722
#if 1 //20160610
	INT FindCORByAngle2Point(PRU *pPRU, DOUBLE dAngleRange, BOOL bUseRotaryEnc = FALSE);
	INT GetStandbyPoint(PRU *pPRU, LONG lBaseT, BOOL bUseRotaryEnc, D_MTR_POSN &dmtrResultPoint);
	INT GetRotatedPoint(PRU *pPRU, LONG lBaseT, LONG lRotatedT, BOOL bUseRotaryEnc, D_MTR_POSN &dmtrResultPoint);
#endif
#if 1 //20160530
	INT FindCORByLeaseSqr(PRU *pPRU, DOUBLE dAngleRange, D_MTR_POSN &dmtrCOR_Result, DOUBLE &dRadius, BOOL bUseRotaryEnc = FALSE);
	INT Get3LeastSquarePoint(PRU *pPRU, LONG lBaseT, DOUBLE dAngleRange, std::vector <D_MTR_POSN> &vecPoint, BOOL bUseRotaryEnc);
#endif
#if 1 //20160525
	INT FindCOR(PRU *pPRU, DOUBLE dAngleRange, D_MTR_POSN &dmtrCOR_Result, DOUBLE &dSDx, DOUBLE &dSDy, BOOL bUseRotaryEnc = FALSE);
	INT FindOneCOR(PRU *pPRU, LONG lBaseT, DOUBLE dAngleRange, D_MTR_POSN &dmtrCOR_Result, BOOL bUseRotaryEnc);
	INT CalcCOR(const D_MTR_POSN &dmtrSamplePt1, const D_MTR_POSN &dmtrSamplePt2, const D_MTR_POSN &dmtrSamplePt3, D_MTR_POSN &dmtrCOR_Result);
	VOID FindCORData(D_MTR_POSN dmtrPt1, D_MTR_POSN dmtrPt2, DOUBLE &dC, DOUBLE &dM, D_MTR_POSN &dmtrMid);
#endif

	BOOL CORCalibration(PRU *pPRU, DOUBLE dInitalStepAngle = 0.5, BOOL bUseRotaryEnc = FALSE, BOOL bInvertAngle = FALSE);
	BOOL FindCORResultRange(DOUBLE *pdMaxX, DOUBLE *pdMinX, DOUBLE *pdMaxY, DOUBLE *pdMinY);
	BOOL SetAllCORData(DOUBLE dXPosn, DOUBLE dYPosn);
	BOOL CheckCORCentre(PRU *pPRU, DOUBLE dTestAng, BOOL bUseRotaryEnc = FALSE, BOOL bInvertAngle = FALSE, PRINT_DATA *stPrintData = NULL);
	BOOL GetPRCentreMap(PRU *pPRU);

	virtual BOOL AutoSearchPR1(PRU *pPRU, PRMultiLevel emPRMultiLevel = PR_LEVEL_NORMAL, CString *pszMsgOut = NULL, BOOL bMoveLighting = FALSE);
	BOOL AutoSearchPR1GrabOnly(PRU *pPRU, CAC9000Stn *pStation = NULL);
	BOOL AutoSearchPR1ProcessOnly(PRU *pPRU, CString *pszMsgOut = NULL, CAC9000Stn *pStation = NULL);
	virtual BOOL AutoSearchPR2(PRU *pPRU, PRMultiLevel emPRMultiLevel = PR_LEVEL_NORMAL, CString *pszMsgOut = NULL, BOOL bMoveLighting = FALSE); //20140414
	BOOL AutoSearchPR2GrabOnly(PRU *pPRU, CAC9000Stn *pStation = NULL);
	BOOL AutoSearchPR2ProcessOnly(PRU *pPRU, CString *pszMsgOut = NULL, CAC9000Stn *pStation = NULL);

	BOOL AutoSearchPBPR1(PRU *pPRU);
	BOOL AutoSearchPBPR1GrabOnly(PRU *pPRU);
	BOOL AutoSearchPBPR1ProcessOnly(PRU *pPRU);
	BOOL AutoSearchPBPR2(PRU *pPRU);
	BOOL AutoSearchPBPR2GrabOnly(PRU *pPRU);
	BOOL AutoSearchPBPR2ProcessOnly(PRU *pPRU);

public:
	virtual CMotorInfo &GetMotorX();
	virtual CMotorInfo &GetMotorY();
	//virtual CMotorInfo &GetMotorZ(WHZNum lWHZ = WHZ_NONE);
	virtual CMotorInfo &GetMotorT();

	virtual INT		SyncX();
	virtual INT		SyncY();
	virtual INT		SyncZ();
	virtual INT		SyncT();

	virtual INT MoveZToPRCalibLevel(BOOL bWait = GMP_WAIT); //Only FOR COR Calib and COR Check
	virtual INT MoveXYToPRCalibPosn(BOOL bWait = GMP_WAIT); //Only FOR COR Calib and COR Check
	virtual INT MoveTToStandby(BOOL bWait = GMP_WAIT);

	virtual INT MoveZToPRLevel(BOOL bWait = GMP_WAIT, PRMultiLevel emPRMultiLevel = PR_LEVEL_NORMAL);
	virtual INT MoveLighting(BOOL bWait = GMP_WAIT);  //20150218 for prebond wh uplook light

	virtual INT MoveXYToCurrPR1Posn(BOOL bWait = GMP_WAIT);
	virtual INT MoveXYToCurrPR2Posn(BOOL bWait = GMP_WAIT);
	
	virtual INT MoveAbsoluteXY(LONG lCntX, LONG lCntY, BOOL bWait);
	virtual INT MoveRelativeXY(LONG lCntX, LONG lCntY, BOOL bWait);

	virtual INT MoveAbsoluteXY(LONG lCntX, LONG lCntY, BOOL bWait, BOOL bMoveToSaveX);
	virtual INT MoveRelativeXY(LONG lCntX, LONG lCntY, BOOL bWait, BOOL bMoveToSaveX);

	virtual INT MoveAbsoluteT(LONG lMtrCnt, BOOL bWait);
	virtual INT MoveRelativeT(LONG lMtrCnt, BOOL bWait);

	virtual PRU &SelectCurPRU(ULONG ulPRU);
	virtual PRU &SelectCurPBPRU(ULONG ulPRU);
	virtual PRU &SelectCurPBBackground(ULONG ulPRU);

	virtual LONG GetStandbyTPosn(); //20160525

protected:
	VOID UpdateHMIPRButton();
	VOID UpdateHMIPBPRButton();
	VOID DisplaySrchArea(PRU *pPRU);
	VOID DisplayPBNonAlignSrchArea(PRU *pPRU);
	VOID DisplayPBSrchArea(PRU *pPRU, LONG lSearchPixel = 20);
	VOID DisplayNormalSrchArea(PRU *pPRU);

	// 20140729 Yip
	VOID SetAndLogPRParameter(const CString &szFunction, const CString &szParameter, DOUBLE &dParameter, DOUBLE dNewValue);
	VOID SetAndLogPRParameter(const CString &szFunction, const CString &szParameter, LONG &lParameter, LONG lNewValue);
	VOID LogPRParameter(const CString &szStation, const CString &szFunction, const CString &szParameter, const CString &szOldValue, const CString &szNewValue);
	VOID LogPRParameter(const CString &szFunction, const CString &szParameter, const CString &szOldValue, const CString &szNewValue);

public:
	VOID PrintCalibSetup(FILE *fp);

protected:
	LONG HMI_SetPRMatchScore(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPRAngRange(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPRPercentVar(IPC_CServiceMessage &svMsg);

	LONG HMI_SetPRSearchRange(IPC_CServiceMessage &svMsg);		//20150107 Polarizer Detect

	// Only For PostBond
	LONG HMI_IncrSegThresholdLevel10(IPC_CServiceMessage &svMsg);
	LONG HMI_IncrSegThresholdLevel(IPC_CServiceMessage &svMsg);
	LONG HMI_DecrSegThresholdLevel10(IPC_CServiceMessage &svMsg);
	LONG HMI_DecrSegThresholdLevel(IPC_CServiceMessage &svMsg);
	LONG HMI_SetSegThreshold(IPC_CServiceMessage &svMsg);
	LONG HMI_SetDefectThreshold(IPC_CServiceMessage &svMsg);
	LONG HMI_SetDefectAttribute(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPostBdRejectTol(IPC_CServiceMessage &svMsg);
	LONG HMI_SetACFPostBondMethod(IPC_CServiceMessage &svMsg);
	
	LONG HMI_SetIsAlign(IPC_CServiceMessage &svMsg);
	LONG HMI_SetAlignAlg(IPC_CServiceMessage &svMsg);

	LONG HMI_SetIsACFAlign(IPC_CServiceMessage &svMsg);
	LONG HMI_SetACFSegmentMethod(IPC_CServiceMessage &svMsg);
	LONG HMI_SetACFAlignMethod(IPC_CServiceMessage &svMsg);

	LONG HMI_SetAlignDistTol(IPC_CServiceMessage &svMsg);	// 20160513
	LONG HMI_SetAlignAngleTol(IPC_CServiceMessage &svMsg);

	LONG HMI_ModifyPRCriteria(IPC_CServiceMessage &svMsg);	// Vision UI
	VOID MachineStartup_SrchPR(PRU *pPRU);	// 20140623 Yip
	LONG HMI_SrchPR(IPC_CServiceMessage &svMsg);
	LONG HMI_DisplaySrchArea(IPC_CServiceMessage &svMsg);

	LONG HMI_SetPRRetryLimit(IPC_CServiceMessage &svMsg);
	LONG HMI_SetRejectGlassLimit(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPRDelay(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPRDelay2(IPC_CServiceMessage &svMsg); //20150323
	LONG HMI_SetCalibPRDelay(IPC_CServiceMessage &svMsg);
	
	LONG HMI_DetectLine(IPC_CServiceMessage &svMsg);	//20150107 Polarizer Detection
	BOOL DetectLine(PRU *pPRU);


public:
	LONG IPC_SavePRTaskStnMachinePara();
	LONG IPC_LoadPRTaskStnMachinePara();
	LONG IPC_SavePRTaskStnDevicePara();
	LONG IPC_LoadPRTaskStnDevicePara();
	
	virtual LONG IPC_SaveMachineRecord();
	virtual LONG IPC_LoadMachineRecord();
	virtual LONG IPC_SaveDeviceRecord();
	virtual LONG IPC_LoadDeviceRecord();
#ifdef EXCEL_MACHINE_DEVICE_INFO
	virtual LONG printPRTaskStnMachinePara();
#endif


};