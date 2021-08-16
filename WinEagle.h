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

#pragma once

#include "prheader.h"
#include "PR_Util.h"

#include "AC9000Stn.h"
#include "PRTaskStn.h"

#ifdef EXCEL_MACHINE_DEVICE_INFO
#include "ExcelMachineInfo_Util.h"
#endif

#ifdef WINEAGLE_EXTERN
extern BOOL				g_bStopCycle;

extern AUTOMODE			AutoMode;
extern BOOL				g_bDiagMode;

extern LONG				g_lDiagnSteps;

extern BOOL				g_bEnableTrigLighting;
extern BOOL				g_bIsVisionNTInited;

// 20140815 Yip
extern BOOL			g_bShowUserSearchPRDone;
extern CString			g_szBondingMsg;

// 20140815 Yip
extern STATUS_TYPE		WINEAGLE_Status;
//extern STATUS_TYPE		PREBOND_HEAD_Status[MAX_NUM_OF_PREBOND];
//extern STATUS_TYPE		SHUTTLE_Status;
//extern STATUS_TYPE		INWH_Status;

//wizard
extern BOOL				g_bWizardEnable;
extern CString			g_szWizardTitle;
extern ULONG			g_ulWizardSubTitle;
extern CString			g_szWizardStep;
extern CString			g_szWizardMess;
extern CString			g_szWizardMessPrev;
extern CString			g_szWizardMessNext;
extern CString			g_szWizardBMPath;
extern BOOL				g_bWizardEnableBack;
extern BOOL				g_bWizardEnableNext;
extern BOOL				g_bWizardEnableExit;
extern ULONG			g_ulWizardStep;
extern LONG				g_lWizardInProgress;

#ifdef PRS_SET_IGNORE_WINDOW_FIRST //20150324
extern ULONG			g_ulWizardIgnoreWindowStep; //20130319 PR ignore windows
#endif
#endif

typedef enum
{
	//INWH_CALIB_PRU = 0,
	//DNLOOK1_WH_CALIB_PRU,
	//DNLOOK2_WH_CALIB_PRU,
	//UPLOOK1_WH_CALIB_PRU,
	//UPLOOK2_WH_CALIB_PRU,
	//UPLOOK1_CALIB_PRU,
	//UPLOOK2_CALIB_PRU,
	//SHUTTLE_CAM1_CALIB_PRU,
	//SHUTTLE_CAM2_CALIB_PRU,
	//LSI_CALIB_PRU,
	//TA4_CALIB_PRU,
	ACF1WH_CALIB_PRU = 0,
	ACF2WH_CALIB_PRU,
	ACFUL_CALIB_PRU,
//#ifndef PBWH_UPLOOK
	//PBDL1_CALIB_PRU,
	//PBDL2_CALIB_PRU,
//#endif
	//UPLOOK1_CALIB_PRU,
	//UPLOOK2_CALIB_PRU,
	//SHUTTLE_CAM1_CALIB_PRU,
	//SHUTTLE_CAM2_CALIB_PRU,
	//LSI_CALIB_PRU,
	//TA4_CALIB_PRU,
	//UPLOOK1_GLASS_CALIB_PRU, //20140620
	//UPLOOK2_GLASS_CALIB_PRU,
	//MBDL1_CALIB_PRU,	// 20141023
	//MBDL2_CALIB_PRU,
	//ACFUL_CALIB_PRU,	//20141222
	MAX_NUM_OF_CALIB_PRU,
} NumOfCalibPRU;

// States
//
typedef enum
{
	WINEAGLE_IDLE_Q = 0,
	WINEAGLE_WAIT_REQUEST_Q,
	WINEAGLE_WAIT_USER_CLICK_PR_Q,
	WINEAGLE_WAIT_STATION_RELEASE_Q,
	WINEAGLE_STOP_Q,
} WINEAGLE_STATE;

class CWinEagle : public CAC9000Stn, public CPRTaskStn
{
	DECLARE_DYNCREATE(CWinEagle)
	friend class CAC9000App;
	friend class CSettingFile;
	friend class CHouseKeeping;
	friend class CPBGlassPR;
	friend class CPreBond;
	friend class CPreBondWH;
	friend class CShuttle;
	friend class CINWH;
	friend class CInspection;
	friend class CACFWH;
	friend class CACF1WH;
	friend class CACF2WH;
	friend class CPRTaskStn;
	friend class CTA1;
#ifdef TA4_PR
	friend class CTA4;
#endif
	friend class CTA2;

protected:
	VOID UpdateLighting(PRU *pPRU);

	VOID UpdateOutput();

	virtual VOID RegisterVariables();
	virtual VOID PreStartOperation();
	virtual VOID AutoOperation();


public:
	CWinEagle();
	virtual	~CWinEagle();

	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID Operation();

protected:
	// ------------------------------------------------------------------------
	// Variable delcaration
	// ------------------------------------------------------------------------

	BOOL		m_bPRFailed;
	BOOL		m_bPRDebug;

	CString		m_szVisionNumber;
	CString		m_szVisionVersion;
	CString		m_szVisionLibVersion;
	
	// pointers
	DPR_Struct	*m_pDPR;					// pointer to the DPR structure

	BOOL		m_bSwitchAutoMode;
	LONG		m_lAutoBondMode;

	// Wizard Related
	BOOL		m_bMoveToPosn;

#if 1 //20130906
	LONG		m_lIncludeAnglePosDeltaX;
	LONG		m_lIncludeAnglePosDeltaY;
	LONG		m_lIncludeAngleNegDeltaX;
	LONG		m_lIncludeAngleNegDeltaY;
#endif

	// 20140815 Yip
	BOOL		m_bUserSearchPRDone;
	BOOL		m_bUserRejectPRTask;
	BOOL		m_bShowUserSearchPRDone;

private:
	CArray<PRU*> m_myPRUList;
	CArray<PRU*> m_myCalibPRUList;

protected:
	CArray<PRU*> &GetmyCalibPRUList() 
	{ 
		return m_myCalibPRUList; 
	}
	CArray<PRU*> &GetmyPRUList() 
	{ 
		return m_myPRUList; 
	}

	// ------------------------------------------------------------------------
	// Function delcaration
	// ------------------------------------------------------------------------
	CString GetSequenceString();	// 20140815 Yip
	VOID OperationSequence();	// 20140815 Yip: Add WinEagle Auto Bond Operation Sequence

	BOOL PRS_Init();
	BOOL PRS_ConnectToVision();
	VOID PRS_InitOptics();
	VOID PRS_InitPRUs();

	VOID FreeAllRecords();
	VOID SetRecordIDToPRUCmd(PRU *pPRU, PRU_ID emID);

	virtual PRU &SelectCurPRU(ULONG ulPRU);

	VOID StartRepeatTest(PRU *pPRU);

	// Calibration
	VOID UpdateHMIDiagnPRScreen(BOOL bOperation);
	VOID UpdateHMICalibPRButton(BOOL bOperation);

	BOOL CalcCentreCursorPt(PRU *pPRU, BOOL bDraw, PR_COORD const PreCursor, PR_COORD *pprCentreCursor);
	
	LONG SelectCalibPRU(ULONG ulPRU, BOOL bCheckModSelected = TRUE);	// 20140915 Yip: Add bCheckModSelected
	PRU &SelectCurCalibPRU(LONG lCurPRU);

#ifdef PRS_SET_IGNORE_WINDOW_FIRST //20150324
	VOID SetPRIgnoreWindow(PRU *pPRU); //20130319 PR ignore windows
	VOID SetPRIgnoreWindowFinish(PRU *pPRU);
	VOID SetPRIgnoreWindowNext(PRU *pPRU);
#endif

	VOID SetupPRPattern(PRU *pPRU);
	VOID SetupPRSrchArea(PRU *pPRU);

	VOID SetupWizard();
	
	VOID SetupPRPatternPreTask();
	VOID SetupPRPatternPostTask();

	VOID ModulePRSetupPreTask();
	VOID ModulePRSetupPostTask();

public:
	LONG SwitchAllCam();	// 20140623 Yip

protected:

	// ------------------------------------------------------------------------
	// HMI Variable delcaration
	// ------------------------------------------------------------------------

	// Adavance PR Page
	ULONG	ulShowPRID;
	ULONG	ulShowValidID;			// 0: No, 1: Yes, 2: ------
	ULONG	ulShowRecordType;		// 0: Pattern, 1: Shape, 2: Tmpl, 3: ACF, 4: Kerf
	ULONG	ulShowOwnerID;

	ULONG	ulTestCounter;
	ULONG	ulTestDelay;

	ULONG	ulShowExposureTime;		// 0: Default, ---> 10: Time 10

	BOOL	bShowPRCoaxialCtrl;
	BOOL	bShowPRCoaxial1Ctrl;
	BOOL	bShowPRCoaxial2Ctrl;
	BOOL	bShowPRRingCtrl;
	BOOL	bShowPRRing1Ctrl;
	BOOL	bShowPRSideCtrl;
	BOOL	bShowPRSide1Ctrl; // 20141023
	BOOL	bShowPRBackCtrl; // 20141023
	BOOL	bShowPRBack1Ctrl; // 20141023

	ULONG	ulHMICoaxialLevel;		// Blue
	ULONG	ulHMICoaxial1Level;		// Red
	ULONG	ulHMICoaxial2Level;		// Prism Lighting
	ULONG	ulHMIRingLevel;
	ULONG	ulHMIRing1Level;		// Side Lighting
	ULONG	ulHMISideLevel;
	ULONG	ulHMISide1Level; // 20141023
	ULONG	ulHMIBackLevel; // 20141023
	ULONG	ulHMIBack1Level; // 20141023

	PR_COORD			m_prZoomCentre;  // Zoom IN/OUT
	PR_CURSOR			m_stCentreCursor;
	LONG				m_lCursorDiagnSteps;
	PR_ZOOM_FACTOR		m_lCurrZoomFactor;
	PR_ZOOM_FACTOR		m_lCurrZoomPanFactor;
	//Zoom In
	PR_COORD	m_PRZoomedPRPt_a;
	PR_COORD	ZoomCentre;

	BOOL	HMI_bCalibPRModSelected;
	BOOL	HMI_bShowCalibPRModSelected;
	BOOL	HMI_bCalibModSelected;
	BOOL	HMI_bShowCalibModSelected;
	BOOL	HMI_bShowThermalRef; //20140627 PBDL calib

	BOOL	HMI_bPRUCalibrated;
	BOOL	HMI_bCORCalibrated;
	LONG	HMI_lCalibPRDelay;
	DOUBLE	HMI_dCalibCORRange;
	DOUBLE	HMI_dTestCORAng;
	BOOL	HMI_bShowAutoCal;
	BOOL	HMI_bShowCORCal;

	DOUBLE	HMI_dIncludedAnglePos; //20130530
	DOUBLE	HMI_dIncludedAngleNeg;
	BOOL	HMI_bShowIncludeAngle;

	BOOL	HMI_bLSITableTab;
	BOOL	HMI_bINWHTableTab;
	BOOL	HMI_bUplookXYTab;		// Control Uplook1 and Uplook2 Cam
	BOOL	HMI_bPreBondWHTab;
	BOOL	HMI_bShuttle1Tab;
	BOOL	HMI_bShuttle2Tab;
	BOOL	HMI_bMBDLTab;
	BOOL	HMI_bACFULTab;		//20141222

#ifdef TA4_PR
	BOOL	HMI_bTA4Tab;
#endif
	BOOL	HMI_bVACSnr; //20130318
	//Wizard
	LONG	HMI_ulPRAlg;
	LONG	HMI_ulPRFMarkType;

	BOOL	HMI_bShowPRAlg;
	BOOL	HMI_bShowFiducial;
	BOOL	HMI_bShowCursorControl;
	BOOL	HMI_bShowDigitalZoom;
	BOOL	HMI_bShowDigitalZoomOnly; //20150314 for shape
	
	CString	HMI_szCurPRStation;		// For select PRU at Advanced Page
	LONG	HMI_lCurRecord;

	LONG	HMI_lErrorLimit; //20140923


	// ------------------------------------------------------------------------
	// HMI Command delcaration
	// ------------------------------------------------------------------------

	LONG HMISwitchCam(IPC_CServiceMessage &svMsg);

	LONG HMI_SelectCurStation(IPC_CServiceMessage &svMsg);
	LONG HMI_SelectCurRecord(IPC_CServiceMessage &svMsg);

	LONG HMIUpdateDiagnPRScreen(IPC_CServiceMessage &svMsg);
	LONG HMIUpdateServicePRScreen(IPC_CServiceMessage &svMsg);

	LONG HMIClearPRRecord(IPC_CServiceMessage &svMsg);

	LONG HMIStartRepeatTest(IPC_CServiceMessage &svMsg);
	LONG HMISetTestCounter(IPC_CServiceMessage &svMsg);
	LONG HMISetTestDelay(IPC_CServiceMessage &svMsg);

	LONG HMIIncrExposureTime(IPC_CServiceMessage &svMsg);
	LONG HMIDecrExposureTime(IPC_CServiceMessage &svMsg);

	LONG HMIIncrCoaxialLevel(IPC_CServiceMessage &svMsg);
	LONG HMIDecrCoaxialLevel(IPC_CServiceMessage &svMsg);
	LONG HMIIncrCoaxialLevel10(IPC_CServiceMessage &svMsg);
	LONG HMIDecrCoaxialLevel10(IPC_CServiceMessage &svMsg);
	
	LONG HMIIncrCoaxial1Level(IPC_CServiceMessage &svMsg);
	LONG HMIDecrCoaxial1Level(IPC_CServiceMessage &svMsg);
	LONG HMIIncrCoaxial1Level10(IPC_CServiceMessage &svMsg);
	LONG HMIDecrCoaxial1Level10(IPC_CServiceMessage &svMsg);

	LONG HMIIncrCoaxial2Level(IPC_CServiceMessage &svMsg);
	LONG HMIDecrCoaxial2Level(IPC_CServiceMessage &svMsg);
	LONG HMIIncrCoaxial2Level10(IPC_CServiceMessage &svMsg);
	LONG HMIDecrCoaxial2Level10(IPC_CServiceMessage &svMsg);

	LONG HMIIncrRingLevel(IPC_CServiceMessage &svMsg);
	LONG HMIDecrRingLevel(IPC_CServiceMessage &svMsg);
	LONG HMIIncrRingLevel10(IPC_CServiceMessage &svMsg);
	LONG HMIDecrRingLevel10(IPC_CServiceMessage &svMsg);

	LONG HMIIncrRing1Level(IPC_CServiceMessage &svMsg);
	LONG HMIDecrRing1Level(IPC_CServiceMessage &svMsg);
	LONG HMIIncrRing1Level10(IPC_CServiceMessage &svMsg);
	LONG HMIDecrRing1Level10(IPC_CServiceMessage &svMsg);

	LONG HMIIncrSideLevel(IPC_CServiceMessage &svMsg);
	LONG HMIDecrSideLevel(IPC_CServiceMessage &svMsg);
	LONG HMIIncrSideLevel10(IPC_CServiceMessage &svMsg);
	LONG HMIDecrSideLevel10(IPC_CServiceMessage &svMsg);

	LONG HMIIncrSide1Level(IPC_CServiceMessage &svMsg);		// 20141023
	LONG HMIDecrSide1Level(IPC_CServiceMessage &svMsg);
	LONG HMIIncrSide1Level10(IPC_CServiceMessage &svMsg);
	LONG HMIDecrSide1Level10(IPC_CServiceMessage &svMsg);

	LONG HMIIncrBackLevel(IPC_CServiceMessage &svMsg);		// 20141023
	LONG HMIDecrBackLevel(IPC_CServiceMessage &svMsg);
	LONG HMIIncrBackLevel10(IPC_CServiceMessage &svMsg);
	LONG HMIDecrBackLevel10(IPC_CServiceMessage &svMsg);

	LONG HMIIncrBack1Level(IPC_CServiceMessage &svMsg);		// 20141023
	LONG HMIDecrBack1Level(IPC_CServiceMessage &svMsg);
	LONG HMIIncrBack1Level10(IPC_CServiceMessage &svMsg);
	LONG HMIDecrBack1Level10(IPC_CServiceMessage &svMsg);

	LONG HMI_SetCursorDiagnSteps(IPC_CServiceMessage &svMsg);
	LONG HMI_IndexCursorXPos(IPC_CServiceMessage &svMsg);
	LONG HMI_IndexCursorXNeg(IPC_CServiceMessage &svMsg);
	LONG HMI_IndexCursorYPos(IPC_CServiceMessage &svMsg);
	LONG HMI_IndexCursorYNeg(IPC_CServiceMessage &svMsg);

	LONG HMI_ZoomInOnly(IPC_CServiceMessage &svMsg); //20150314
	LONG HMI_ZoomOutOnly(IPC_CServiceMessage &svMsg);
	LONG HMI_ZoomIn(IPC_CServiceMessage &svMsg);
	LONG HMI_ZoomOut(IPC_CServiceMessage &svMsg);

	LONG HMIFreeAllRecords(IPC_CServiceMessage &svMsg);

	LONG HMIOnOffLogMode(IPC_CServiceMessage &svMsg);
	LONG HMISetDebugFlag(IPC_CServiceMessage &svMsg);
	LONG HMISetComLogFlag(IPC_CServiceMessage &svMsg);
	LONG HMISetComLogTimeLimit(IPC_CServiceMessage &svMsg);

	LONG HMISetVisionNtDlgToService(IPC_CServiceMessage &svMsg);
	LONG HMISetVisionNtDlgToVision(IPC_CServiceMessage &svMsg);

	LONG HMI_SetCalibPRSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetCalibModSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_SelectCalibPRU(IPC_CServiceMessage &svMsg);
	LONG HMI_SelectPRAlg(IPC_CServiceMessage &svMsg);
	LONG HMI_SelectFMarkType(IPC_CServiceMessage &svMsg);

	LONG HMI_SetCalibPRDelay(IPC_CServiceMessage &svMsg);
	LONG HMI_SetCalibCORRange(IPC_CServiceMessage &svMsg);
	LONG HMI_SetTestCORAng(IPC_CServiceMessage &svMsg);
	
	//LONG HMI_GoThermalCalibRefPosn(IPC_CServiceMessage &svMsg); //20140627 PBDL calib
	//LONG HMI_SetThermalCalibRefPosn(IPC_CServiceMessage &svMsg);
	LONG HMI_AutoCal(IPC_CServiceMessage &svMsg);
	LONG HMI_CORCal(IPC_CServiceMessage &svMsg);
	LONG HMI_DistortionCal(IPC_CServiceMessage &svMsg);

	//LONG HMI_CORCalAngle2Point(IPC_CServiceMessage &svMsg); //20160610
	LONG HMI_CORCalLeastSqr(IPC_CServiceMessage &svMsg); //20160530
	LONG HMI_CORCalNew(IPC_CServiceMessage &svMsg); //20160525

	LONG HMI_AutoTest(IPC_CServiceMessage &svMsg); //20140923
	LONG HMI_CORAutoTest(IPC_CServiceMessage &svMsg);
	//LONG HMI_TA4CORTest(IPC_CServiceMessage &svMsg); //20141120

	LONG HMI_SetPRErrorLimit(IPC_CServiceMessage &svMsg); //20140923


	LONG HMI_CheckPRCentre(IPC_CServiceMessage &svMsg);
	LONG HMI_SetCORCentre(IPC_CServiceMessage &svMsg);
	LONG HMI_CheckCORCentre(IPC_CServiceMessage &svMsg);

	LONG HMI_GetPRCentreMap(IPC_CServiceMessage &svMsg);

	LONG HMI_ToggleVAC(IPC_CServiceMessage &svMsg); //20130318

	LONG HMI_ResetIncludedAngle(IPC_CServiceMessage &svMsg); //20130530

	// 20140815 Yip
	LONG HMI_SetUserSearchPRContinue(IPC_CServiceMessage &svMsg);
	LONG HMI_SetUserRejectPRTask(IPC_CServiceMessage &svMsg);

	LONG HMI_VerifyPreBondCalibrationPositions(IPC_CServiceMessage &svMsg);	// 20140915 Yip

#if 1 //20121024 debug only
	BOOL LogCalbIncludeAngData(CString &szLabel, LONG lGlass, LONG lXOffset, LONG lYOffset, DOUBLE dErrDistance, LONG lErrXOffset, LONG lErrYOffset);
#endif
	virtual VOID PrintPRSetup(FILE *fp);
	virtual VOID PrintPRCalibSetup(FILE *fp);
#ifdef EXCEL_MACHINE_DEVICE_INFO
	virtual BOOL printMachineDeviceInfo();
	LONG printPRPara(CString szStation, PRU *pPRU, CString szPRU, BOOL IsMachineData);
#endif


	LONG SavePRPara(CString szStation, PRU *pPRU, CString szPRU, BOOL IsMachineData = FALSE);
	LONG LoadPRPara(CString szStation, PRU *pPRU, CString szPRU, BOOL IsMachineData = FALSE);  // using the wizard station

	// Saving / loading parameters
	virtual LONG IPC_SaveMachineParam();
	virtual LONG IPC_LoadMachineParam();
	virtual LONG IPC_SaveDeviceParam();
	virtual LONG IPC_LoadDeviceParam();


};
