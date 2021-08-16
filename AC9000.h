/////////////////////////////////////////////////////////////////
//	AC9000.h : main header file for the AC9000 application
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

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "tlhelp32.h"
#include "AC9000Stn.h"
#include "resource.h"
#include "InitOperation.h"
#include "StringMapFile.h"
#include "Package_Util.h"
#include "MessageDlg.h" //20121231
#include "PR_Util.h"
#include "PerformanceReport.h"
#include <map> //20141124

#ifdef AC9000_EXTERN
extern	CCriticalSection	m_csMsgLock; 
extern	CStringArray		m_astrMsgQueue;

// Get All Status
extern CCriticalSection		m_csGetAllStatusLock;	// 20140728 Yip

// Parameter Log
extern CCriticalSection		m_csParameterLogLock;	// 20140729 Yip

// Update Statistic
extern CCriticalSection		m_csUpdateStatisticLock;	// 20140903 Yip

// Log ErrorAlert
extern CCriticalSection		m_csLogErrorAlertLock;	// 20141124

// Heater Control
extern CCriticalSection		m_csHeaterCtrlLock; //20150714

extern	BOOL				g_bIsVisionNTInited;

extern AUTOMODE				AutoMode;
extern BOOL					g_bDiagMode;

extern BOOL					g_bFlushMode;
extern LSITypeInUse			g_lLSITypeInUse;
extern LSI_TYPE_INFO		g_stLSIType[MAX_NUM_OF_LSI_TYPE];
extern LSI_ON_GLASS_INFO	g_stLSI[MAX_NUM_OF_LSI];
extern ULONG				g_ulUserGroup;

//extern FPC_TYPE_INFO	g_stFPCType[MAX_NUM_OF_FPC_TYPE];

// Station status
//extern GATEWAY_STATUS_TYPE GC902_CWH_Status; //edge cleaner
extern STATUS_TYPE TA1_Status;
extern STATUS_TYPE TA2_Status;
extern STATUS_TYPE ACF_WH_Status[MAX_NUM_OF_ACFWH];
extern STATUS_TYPE ACF_Status[MAX_NUM_OF_ACF_UNIT];
extern STATUS_TYPE IN_PICKARM_Status;
extern STATUS_TYPE INSPOPT_Status;
extern STATUS_TYPE INSP_Status[MAX_NUM_OF_INSP];
extern STATUS_TYPE GLASSPR_Status[MAX_NUM_OF_PREBOND];
extern STATUS_TYPE TA_MANAGER_Status;

extern STATUS_TYPE	ACF_GLASS_Status[MAX_NUM_OF_ACFWH][MAX_NUM_OF_GLASS];
extern STATUS_TYPE	IN_PICKARM_GLASS_Status;
extern STATUS_TYPE	TA1_GLASS_Status;
extern STATUS_TYPE  TA2_GLASS_Status;

extern STATUS_TYPE	WINEAGLE_Status;

extern INT g_nCPAStandby;
extern INT g_nFlipperStandby;

extern CPerformanceReport	*g_pPerformance_report;		//Philip Add 2012/12/7
extern LONG		g_ulDailyPerformanceReportPage;
extern EXCEL_OFFLINE_INFO		g_stOfflineInfo; //20150812

extern BOOL g_bPRDebugInfo; //20150423
extern BOOL GlassOnOutBelt; //20151029
extern BOOL g_bEnableAnvilVac; //20170816
extern BOOL	g_bUplookPREnable;
#endif

enum  	//20130712 //for _getdiskfree() function
{
	DRIVE_NONE = 0, 
	DRIVE_A, 
	DRIVE_B, 
	DRIVE_C, 
	DRIVE_D, 
	DRIVE_E
};

class CPRTaskStn;

class CAC9000App : public SFM_CModule
{
//Attribute
	friend class CHouseKeeping;
public:
	//BOOL CalcWHZLinearEncoder(CAC9000Stn* pStation, MOTOR_PROTECTION stProtect, WHZ_DATA* pResult); //whz rotary linear calib
	CString	m_CurrMotor;


protected:
	//Cal WHZ

	BOOL	HMI_bTA1Z_Selected;
	
	BOOL	HMI_bACF1WHZ_Selected;
	BOOL	HMI_bACF2WHZ_Selected;

	STARTUPINFO			startupInfo;
	PROCESS_INFORMATION	processInfo;

	enum {NULL_OP, AUTO_OP, MANUAL_OP, CYCLE_OP, DEMO_OP};
	enum {MOTION_RUNNING, MOTION_COMPLETE, MOTION_ERROR};

	CString	m_szGroupID;
	CString	m_szDataPath;

	// Message Log variables
	CString	m_szLogFile;
	CString	m_szLogFilePath;
	BOOL	m_fLogFileCreated;
	BOOL	m_fLogMessage;
	BOOL	m_fCanDisplay;
	BOOL	m_fStopMsg;
	CMutex	m_csDispMsg;

	//State
	CString	m_szState;	// Main state name
	INT		m_qState;	// Main state
	INT		m_nOperation;
	DWORD	m_dwStopTime;
	BOOL	m_bStatusLogged;

	LONG	HMI_lSWLimitEncoder; //whz rotary linear calib
	LONG	m_lDiagnSteps; //whz rotary linear calib

	//Flag
	BOOL	m_fDeleteMarkIDE;
	BOOL	m_fExecuteHmi;
	BOOL	m_fRestartHmi;
	BOOL	m_bRunState;
	BOOL	m_fAutoStart;

	BOOL	m_fEnableInitInstanceError;	// Even Init Instance Have Error, still Continous Work

	BOOL	m_fSelectVer1;
	BOOL	m_fSelectVer2;
	BOOL	m_fSelectVer3;
	BOOL	m_fSelectLitec;
	BOOL	m_fSelectADAM;
	BOOL	m_fSelectHeaterOn;	// 20140905 Yip
	//BOOL	m_fSelectEdgeCleaner;
	BOOL	m_fSelectCOG902; //m_fSelectCOG902

	BOOL	m_fEnableHardware;
	BOOL	m_fEnablePRS;
	BOOL	m_fEnablePreBondMach;
	BOOL	m_fEnableMainBondMach;
	BOOL	m_fEnableTempCtrl;
	BOOL	m_fEnableACF1WH;
	BOOL	m_fEnableACF2WH;
	BOOL	m_fEnableACF1;
	//BOOL	m_fEnableACF2;
	BOOL	m_fEnableTA1;
	BOOL	m_fEnableTA2;
	BOOL	m_fEnableInPickArm;
	BOOL	m_fEnableInspOpt;
	BOOL	m_fEnableDiagnMode;

	CMotorInfo *m_pStMotorSWLimit;
	//Software/ Machine info
	CString m_szMachineModel;
	CString m_szMachineID;
	CString m_szOSImageNumber;
	CString m_szSoftwareNumber;
	CString m_szMotionLibVersion; //20121210

	CString m_szVisionOSImageNumber;

	//Lot Info
	CString m_szLotNumber;
	CString m_szLotStartTime;
	CString m_szLotEndTime;

	BOOL m_bPerformanceReportEnable;
	LONG HMI_ulSettingReportManagerStartTime;
	CString HMI_szSettingReportManagerShift1;
	CString HMI_szSettingReportManagerShift2;
	CString HMI_szSettingReportManagerShift3;
	CString HMI_szSettingReportManagerShift4;
	CString HMI_szSettingReportManagerShift5;
	CString HMI_szSettingReportManagerShift6;
	CString HMI_szSettingReportManagerShift7;
	CString HMI_szSettingReportManagerShift8;
	CString HMI_szSettingReportManagerShift9;
	CString HMI_szSettingReportManagerShift10;
	CString HMI_szSettingReportManagerShift11;
	CString HMI_szSettingReportManagerShift12;
	CString HMI_szSettingReportManagerShift13;
	CString HMI_szSettingReportManagerShift14;
	CString HMI_szSettingReportManagerShift15;
	CString HMI_szSettingReportManagerShift16;
	CString HMI_szSettingReportManagerShift17;
	CString HMI_szSettingReportManagerShift18;
	CString HMI_szSettingReportManagerShift19;
	CString HMI_szSettingReportManagerShift20;
	CString HMI_szSettingReportManagerShift21;
	CString HMI_szSettingReportManagerShift22;
	CString HMI_szSettingReportManagerShift23;
	CString HMI_szSettingReportManagerShift24;

//Attribute
public:
	CInitOperation		*m_pInitOperation; //InitOperation Object - Created by Mark Wizard
	CGmpSystem			*m_pCGmpSystem;

	//Machine File and Device File
	CStringMapFile		m_smfMachine;
	CStringMapFile		m_smfDevice;
	CCriticalSection	m_csMachine;
	CCriticalSection	m_csDevice;
	CString				m_szMachineFile;
	CString				m_szDeviceFile;
	CString				m_szMachinePath;
	CString				m_szDevicePath;
	CString				m_szSelectedFile;
#ifdef EXCEL_OFFLINE_SETUP
	CString				m_szOfflineSetupPath;
	CString				m_szOfflineSetupFile;
#endif

	DOUBLE				m_dPrevSWVersion;
	DOUBLE				m_dCurSWVersion;
	
	CString				m_szSoftVersion;
	CString				m_szReleaseDate;

	CString				m_szLitecVersion1;
	CString				m_szLitecVersion2;
	CString				m_szLitecVersion3;
	CString				m_szLitecVersion4;

	BOOL				m_isAppRunning; //20111214
	std::map<CString, CString> m_mapErrorAlertTable; //20141124
	clock_t				m_clkBondStartTime;
	clock_t				m_clkLastErrorTime; //20150105
	PROCESS_INFORMATION		m_stGatewayProcessInfo;
	ULONG				m_ulFreeSpaceMB_C; //20150810
	ULONG				m_ulFreeSpaceMB_D;

//Operation
protected:
	//Initialisation
	VOID LoadData();
	BOOL InitHardware();
	VOID SetupIPC();
	DWORD RunLongTimeFunction(AFX_THREADPROC pfnThreadProc, LPVOID pParam, const CString &szDesc);	// 20140724 Yip

	//Check for Motion Completion
	INT CheckStationMotion();

	//Scan Input & Update Output
	VOID UpdateOutput();
	VOID ScanInput();

	//Operation
	VOID UnInitialOperation();
	VOID IdleOperation();
	VOID DiagOperation();
	VOID SystemInitialOperation();
	VOID PreStartOperation();
	VOID AutoOperation();
	VOID DemoOperation();
	VOID CycleOperation();
	VOID ManualOperation();
	VOID StopOperation();
	VOID DeInitialOperation();

	// IPC Registered Services
	LONG SrvInitialize(IPC_CServiceMessage &svMsg);
	LONG SrvSystemInitialize(IPC_CServiceMessage &svMsg);
	LONG SrvDiagCommand(IPC_CServiceMessage &svMsg);
	LONG SrvAutoCommand(IPC_CServiceMessage &svMsg);
	LONG SrvManualCommand(IPC_CServiceMessage &svMsg);
	LONG SrvCycleCommand(IPC_CServiceMessage &svMsg);
	LONG SrvDemoCommand(IPC_CServiceMessage &svMsg);
	LONG SrvStopCommand(IPC_CServiceMessage &svMsg);
	LONG SrvResetCommand(IPC_CServiceMessage &svMsg);
	LONG SrvDeInitialCommand(IPC_CServiceMessage &svMsg);
	LONG SrvJogCommand(IPC_CServiceMessage &svMsg);
	LONG SrvUpdateMachine(IPC_CServiceMessage &svMsg);
	LONG SrvUpdateDevice(IPC_CServiceMessage &svMsg);
	LONG SrvChangeDevice(IPC_CServiceMessage &svMsg);
	//LONG SrvSaveAsDevice(IPC_CServiceMessage& svMsg);
	LONG SrvNewDevice(IPC_CServiceMessage &svMsg);
	LONG SrvDeleteDevice(IPC_CServiceMessage &svMsg);
	LONG SrvRestoreMachine(IPC_CServiceMessage &svMsg);
#ifdef EXCEL_OFFLINE_SETUP
	LONG HMI_LoadOfflineSetupFile(IPC_CServiceMessage &svMsg);
#endif	
//p20120829
	LONG HMI_StatisticsLogFile(IPC_CServiceMessage &svMsg);
//p20120829:end

	LONG SrvPrintMachine(IPC_CServiceMessage &svMsg);
	LONG SrvPrintDevice(IPC_CServiceMessage &svMsg);

	//Load Machine and Device File
	BOOL LoadMachineFile(BOOL bLoad = TRUE);
	BOOL LoadDeviceFile(BOOL bLoad = TRUE, BOOL bDeletePR = TRUE);

	LONG LoadSoftwareVersion();
	LONG SaveSoftwareVersion();


	//Register Hmi Variables
	VOID RegisterVariables();

	VOID setKlocworkTrue(BOOL bValue); //klocwork fix 20121211

public:
	BOOL getKlocworkTrue();
	BOOL getKlocworkFalse();
	BOOL CheckStopSuccess(); //20150819
	BOOL CheckFileSize(CString &szInFilePath, CString &szInFileName, LONG lFileSizeLimitInKb);

	BOOL LoadSettingReportConfig();
	BOOL SaveSettingReportConfig(CString inReportSettingConfig);
	LONG HMI_SettingReportManagerSaveShiftSchedule(IPC_CServiceMessage &svMsg);
	LONG HMI_ShowPerformanceReportPage(IPC_CServiceMessage &svMsg);

	LONG HMI_CheckStartBondingAllowed(IPC_CServiceMessage &svMsg); //20150610
	LONG HMI_SetPRDebugInfo(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleEnableAnvilVac(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleUplookEnable(IPC_CServiceMessage &svMsg);

#ifdef EXCEL_MACHINE_DEVICE_INFO
	VOID *GetStnObjPointerByStnName(const CString &szStnName);
	LONG HMI_PrintMachineDeviceInfoExcel(IPC_CServiceMessage &svMsg);
#endif

//Operation
public:
	CAC9000App();
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	INT State();
	VOID Operation();

	//Group ID and ModuleID
	CString GetModuleID();
	CString	GetGroupID();
	VOID SaveGroupId(const CString &szGroup);
	VOID SetGroupId(const CString &szGroup);

	//Start Execution
	BOOL AutoStart();
	BOOL SetRun();
	VOID SaveAutoStart(BOOL bOption);

	//Hardware & Option Selection
	VOID SelectHardware(BOOL isOKEnable); //20111214
	VOID SetOptions(BOOL isOKEnable);

	//Msg Log
	BOOL LogMessage();
	VOID SetLogMessage(BOOL bEnable);
	VOID SetTotLogBackupFiles(LONG lUnit);
	VOID SetTotLogFileLine(LONG lUnit);

	//Msg Display
	VOID DisplayException(CAsmException &e);
	VOID DisplayMessage(const CString &szMessage);
	BOOL KeepResponse();	// 20140714 Yip

	VOID ExecuteCreateErrorLog();	// 20140714 Yip
	VOID ExecuteGateway();
	//Execute HMI
	VOID ExecuteHmi();
	VOID KillHmi();
	VOID SetExecuteHmiFlag(BOOL bExecute = TRUE);
	BOOL IsExecuteHmi();
	
	BOOL ScanVisionNT();

	VOID SetAlert(UINT unCode, BOOL bLightAndSound = TRUE);
	VOID SetError(UINT unCode);

	//Log error info // 20141124
	VOID InitErrorAlertTable();
	VOID LogErrorAlarm(const CString &szError, BOOL bIsError = TRUE);

	//Log StationStatus
	VOID GetAllStatus(const CString &szError = "");	// 20140729 Yip: Add Error Description In Status Log

	BOOL MotionLog(const CString &szFunction, DOUBLE dVar1 = 0.0, DOUBLE dVar2 = 0.0, BOOL bWait = GMP_WAIT);
	VOID LogParameter(const CString &szStation, const CString &szFunction, const CString &szParameter, const CString &szOldValue, const CString &szNewValue);	// 20140729 Yip
	VOID LogParameter(const CString &szFunction, const CString &szParameter, const CString &szOldValue, const CString &szNewValue);	// 20140812 Yip

private:
	BOOL	m_bKlocworkTrue; //klocwork fix 20121211

// ------------------------------------------------------------------------
// My addition
// ------------------------------------------------------------------------
public:
	INT		MachineType;

	LONG	m_lAction;

	BOOL	m_bHWInitCheckingPressure; //20130710
	BOOL	m_bHWInitFatalError; //20130226
	BOOL	m_bHWInitFatalErrorUI; //20130710
	BOOL	m_bHWInitError;
	BOOL	m_bHWInited;

	BOOL	m_bAllLitecInited;
	BOOL	m_bAllAdamInited;

	BOOL	m_bInvalidInitState;
	BOOL	m_bInitInstanceError;

	BOOL	m_bAdamExit;

	// Station commutated and homed flags

	BOOL	m_bTA1Comm;
	BOOL	m_bTA1Home;
	BOOL	m_bTA2Comm;
	BOOL	m_bTA2Home;
	BOOL	m_bACF1Comm;
	BOOL	m_bACF1Home;
	//BOOL	m_bACF2Comm;
	//BOOL	m_bACF2Home;
	BOOL	m_bACF1WHComm;
	BOOL	m_bACF1WHHome;
	BOOL	m_bACF2WHComm;
	BOOL	m_bACF2WHHome;
	BOOL	m_bInPickArmComm;
	BOOL	m_bInPickArmHome;
	BOOL	m_bInspOptComm;
	BOOL	m_bInspOptHome;

	BOOL	m_bDiagnMode;

	// Diagnostic testing
	BOOL	bPRRepeatTest;

	// statistics
	ULONG	m_ulGlassPicked;
	ULONG	m_ulTotalGlassPicked;

	ULONG	m_ulGlassRejected;
	ULONG	m_ulTotalGlassRejected;
	ULONG	m_ulInvalidGlass;
	ULONG	m_ulTotalInvalidGlass;
	ULONG	m_ulACFWHPRError;
	ULONG	m_ulTotalACFWHPRError;
	ULONG	m_ulACFWHPBPRError;
	ULONG	m_ulTotalACFWHPBPRError;
	ULONG	m_ulPBWHPRError;
	ULONG	m_ulTotalPBWHPRError;

	ULONG	m_ulUserInputPRError;
	ULONG	m_ulTotalUserInputPRError;


	ULONG	m_ulGlassTolError;
	ULONG	m_ulTotalGlassTolError;
	ULONG	m_ulAngCorrError;
	ULONG	m_ulTotalAngCorrError;

	ULONG	m_ulTrayHolderPRError;
	ULONG	m_ulTotalTrayHolderPRError;

	ULONG	m_ulCPAPickError;
	ULONG	m_ulTotalCPAPickError;

	// 20141003 Yip: Add Statistics For Shuttle Collect FPC Error

	ULONG	m_ulTotalPreBondPRError;

	ULONG	m_ulUnitsBonded;
	ULONG	m_ulTotalUnitsBonded;

	ULONG	m_ulUnitsToBond;					// User Insert number of unit to bond in Test Bond Mode;
	ULONG	m_ulUnitsToBondCounter;			

	ULONG	m_ulUnitsPickedToClean;				// User Insert number of unit bonded then alarm to Clean table
	ULONG	m_ulUnitsPickedToCleanCounter;

//p20120829
	ULONG	m_lStatisticsLogCount;
//p20120829:end

	// Message windows acknowledge flag
	CString				m_szHMIMess;
	BOOL				m_bHMIMess;

	DOUBLE	m_dUPH;

	DWORD	m_dwOpStartTime;
	DWORD	m_dwOpStopTime;

	ULONG	m_ulLangageCode;

	DOUBLE	dDummy;
	CString	stDummy;

	CString HMI_szMotorDirectionPng;
	LONG	HMI_lMotorPosLmt;
	LONG	HMI_lMotorNegLmt;
	//BOOL	HMI_bIsEdgeCleaner;
	BOOL	HMI_bIsCOG902;
	LONG	HMI_lGatewayStatus;
	LONG	m_lThreadID; //20150706
	BOOL m_bSaveLoadDeviceMachine; //20150824 TRUE during device/machine save/loading. Sleep more during save/load, so hope that vision could run faster.

	// ------------------------------------------------------------------------
	// Start Up Sequence
	// ------------------------------------------------------------------------
	VOID PrintMachineInfo();

	INT	 MachineVersion();

	VOID TC_AddJob(INT emTCObjectID);
	VOID TC_DelJob(INT emTCObjectID);

	BOOL IsInitNuDrive();
	BOOL IsTempCtrlInited();
	BOOL IsAnyModuleBusy();
	BOOL IsAllServoCommutated();
	BOOL IsAllMotorHomed();

	BOOL IsSelectLitec();
	BOOL IsSelectADAM();
	BOOL IsSelectHeaterOn();	// 20140905 Yip
	//BOOL IsSelectEdgeCleaner();
	BOOL IsSelectCOG902();
	BOOL IsSelectTA4Camera();
	BOOL IsSelectInConvWH(); //20170724
	BOOL IsAdamExit();

	INT	 StartDataLog(CStringList *pszListPort, GMP_DATALOG_MODE enDatalogMode = GMP_CONTINUOUS, GMP_LOGIC enStopLogic = GMP_NE, GMP_U32 u32StopPattern = 0);	// 20140922 Yip: Add Datalog Mode And Stop Condition
	INT	 StopDataLog(CString szFileName);



	// ------------------------------------------------------------------------
	//Message Windows
	// ------------------------------------------------------------------------
	VOID WriteHMIMess(const CString &szMess, BOOL bLog = FALSE, BOOL bLogTime = TRUE);
	
	// ------------------------------------------------------------------------
	//HMI Windows
	// ------------------------------------------------------------------------
	LONG HMIMessageBox(const INT nBoxType, const CString &szTitle, const CString &szText);
	LONG HMISelectionBox(const CString &szTitle, const CString &szText, const CString &szItemText1 = NULL, const CString &szItemText2 = NULL, const CString &szItemText3 = NULL, const CString &szItemText4 = NULL, const CString &szItemText5 = NULL, const CString &szItemText6 = NULL, const CString &szItemText7 = NULL, const CString &szItemText8 = NULL, const CString &szItemText9 = NULL, const CString &szItemText10 = NULL);
	LONG HMIConfirmWindow(const CString &szFirstbtn, const CString &szSecondbtn, const CString &szThirdbtn, const CString &szTitle, const CString &szText);
	BOOL HMIGetPassword(const CString &szTitle, const CString &szPassword);
	BOOL HMIAlphaKeys(const CString &szPrompt, CString *pszKeyIn, BOOL bKeyInPassword = FALSE);
	BOOL HMINumericKeys(const CString &szPrompt, const DOUBLE dMaxValue, const DOUBLE dMinValue, DOUBLE *pdValue);
	BOOL HMINumericKeys(const CString &szPrompt, const DOUBLE dMaxValue, const DOUBLE dMinValue, LONG *plValue);

	BOOL HMILangSwitching();
//p20120918
	BOOL HMILangTraditionalChinese();
//p20120918:end

	VOID ProcessHMIRequest();

	// ------------------------------------------------------------------------
	// Function delcaration
	// ------------------------------------------------------------------------
	BOOL CheckAutoBondSelected(AutoBondRequirment lMode);
	BOOL CheckPackageStatus(AutoBondRequirment lMode);
	BOOL CheckHeaterOnOff(); //20130424

	//BOOL CheckAutoBondSelected();
	//BOOL CheckReworkPreBondSelected();
	//BOOL CheckReworkMainBond1Selected();
	//BOOL CheckReworkMainBond2Selected();

	BOOL CheckGlassIndexingPath(CString StationName, GlassNum enGlassNum);

	BOOL CheckUnitToBondLmt();		// TRUE when Picked Glass = Unit To Bond
	BOOL CheckToCleanLmt();			// TRUE when UnitsBonded%m_ulUnitsPickedToClean == 0;

	// ------------------------------------------------------------------------
	// HMI Command delcaration
	// ------------------------------------------------------------------------

	LONG SrvCloseApp(IPC_CServiceMessage &svMsg);
	LONG SrvRestartHMI(IPC_CServiceMessage &svMsg);
	LONG SetMachineID(IPC_CServiceMessage &svMsg);
	VOID PrintThreadID(); //20150706

	LONG HMI_GetAllStatus(IPC_CServiceMessage &svMsg);
	LONG HMI_ResetStat(IPC_CServiceMessage &svMsg);

	LONG HMI_SetAllModSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_CheckAllModSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetAllPRSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetAllDebugSeq(IPC_CServiceMessage &svMsg);
	LONG HMI_SetAllLogMotion(IPC_CServiceMessage &svMsg); //20130422

	LONG HMI_PickGlassOperation(IPC_CServiceMessage &svMsg);
	LONG HMI_CleanOperation(IPC_CServiceMessage &svMsg);
	LONG HMI_FlushMode(IPC_CServiceMessage &svMsg);
	LONG HMI_CheckGatewayConnection(IPC_CServiceMessage &svMsgIn); //20130619
	LONG HMI_SetUnitsToBond(IPC_CServiceMessage &svMsg);

	LONG GatewayCmdAskForGlass(GlassNum lGlassNum); //20130725
	LONG GatewayCmdUnloadGlass(GlassNum lGlassNum); //20130725
	
	// ---------------------------------------------------------------
	// Misc
	// ---------------------------------------------------------------
	INT UnselectModuleForIndexingMode(); //20130710
	BOOL ShowPRUploadErrorMessage(PRU *pPRU, CString &szErrorMsg); //20130713
	BOOL ShowPRDownloadErrorMessage(PRU *pPRU, CString &szErrMsg);

	// ---------------------------------------------------------------
	// Software Protection
	// ---------------------------------------------------------------
	LONG HMI_SelectStation(IPC_CServiceMessage &svMsg);
	VOID getStationPicFilename(CString &szStationName, CString &szPicFileName); //20120620
	LONG HMI_SelectStationMotor(IPC_CServiceMessage &svMsg);
	LONG HMI_SetMotorProtectPosLmt(IPC_CServiceMessage &svMsg);
	LONG HMI_SetMotorProtectNegLmt(IPC_CServiceMessage &svMsg);

	LONG HMI_EnterSoftwareLimitPage(IPC_CServiceMessage &svMsg);
	LONG HMI_LeaveSoftwareLimitPage(IPC_CServiceMessage &svMsg);
	LONG HMI_SetDiagnSteps(IPC_CServiceMessage &svMsg);
	LONG HMI_SWLimitIndexMotorPos(IPC_CServiceMessage &svMsg);
	LONG HMI_SWLimitIndexMotorNeg(IPC_CServiceMessage &svMsg);
	VOID SetDiagnSteps(ULONG ulSteps);

	LONG HMI_DataLog(IPC_CServiceMessage &svMsg);
	LONG HMI_MoveAllWHtoBondPosn(IPC_CServiceMessage &svMsg); //20130318
	LONG HMI_KeepQuiet(IPC_CServiceMessage &svMsg);

	LONG LoadGlassReqGatewayOperation(GlassNum lGlass);
	LONG LoadGlassGatewayOperation(GlassNum lGlass);
	DWORD DeleteOldFiles(CString &szInFilePath, CString &szWildCard, LONG lDay); //20141210
	BOOL GetHDDFreeSpace(ULONG &ulFreeSpaceMB_C, ULONG &ulFreeSpaceMB_D); //20150810

#ifdef EXCEL_OFFLINE_SETUP
	INT LoadExcelSetupInfo(CString &szExcelPath);
	INT SetOfflineSetupGeneral();
	INT SetOfflineSetupINWH();
	INT SetOfflineSetupPBWH();
#endif
	BOOL SetHeapInfo(); //20151005
	INT LogMemoryStatus();
	INT LogSystemMemoryStatus();
	ULONGLONG SubtractTimes(const FILETIME& ftA, const FILETIME& ftB);
	BOOL GetCpuUsage(HANDLE hProcess, DOUBLE &dCpuUsage, FILETIME &ftPrevSysTime, FILETIME &ftPrevProcKernel, FILETIME &ftPrevProcUser);

	// --------------------------------------------------------
	// GetAllStation
	// --------------------------------------------------------
public:
	CArray<CAC9000Stn*> &GetStationList() 
	{ 
		return m_Stations; 
	}
	CArray<CPRTaskStn*> &GetPRStationList() 
	{ 
		return m_PRStations; 
	}

private:
	CArray<CAC9000Stn*> m_Stations;
	CArray<CPRTaskStn*> m_PRStations; // for that station have PRU Only



	DECLARE_MESSAGE_MAP()
};

extern CAC9000App theApp;
