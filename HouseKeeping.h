/////////////////////////////////////////////////////////////////
//	CHouseKeeping.cpp : interface of the CHouseKeeping class
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
#include "Package_Util.h"

#ifdef EXCEL_MACHINE_DEVICE_INFO
#include "ExcelMachineInfo_Util.h"
#endif

#ifdef HOUSEKEEPING_EXTERN
extern BOOL					g_bStopCycle;

extern AUTOMODE				AutoMode;
extern BOOL					g_bDiagMode;
extern LONG					g_lDiagnSteps;
extern BOOL					g_bSelectedReworkMode;

extern BOOL					g_bShowForceInNewton;
extern DOUBLE				g_dModuleOffset;

extern GLASS_INFO			g_stGlass;
extern LSI_ON_GLASS_INFO	g_stLSI[MAX_NUM_OF_LSI];
extern LSI_TYPE_INFO		g_stLSIType[MAX_NUM_OF_LSI_TYPE];

extern BOOL					g_bSafeIndexing;
extern BOOL					g_bSlowMotionIndexing;
extern ULONG				g_ulSlowMotionFactor;
extern FORCE_CALIBRATOR		g_stForceCalibrator;
		
extern BOOL					g_bEnableTrigLighting;

extern ULONG				g_ulUserGroup;
extern LONG					g_lGlassInputMode; //20120412
extern LONG					g_lErrorCounter;

// 20140815 Yip
extern BOOL					g_bShowUserSearchPRDone;
extern CString				g_szBondingMsg;

//extern GATEWAY_STATUS_TYPE	GC902_CWH_Status;

extern LONG					g_lGatewayTA1Status; //20130919

extern GATEWAY_STATUS_TYPE	COG902_TA5_Status;
extern COG902_Glass_Info	g_COG902GlassInfo[2];

extern GATEWAY_STATUS_TYPE AC9000_Machine_Status;
extern GATEWAY_STATUS_TYPE COG902_Machine_Status;
#endif


typedef struct
{
	CString			szName;

	BOOL			bValid;				// TRUE when PreBond PR learnt && alignment pt calibrated
	DOUBLE			dLength;
	DOUBLE			dWidth;
	DOUBLE			dThickness;
	
	// Bond Pt Related
	D_MTR_POSN		dmtrAlignPt1;
	D_MTR_POSN		dmtrAlignPt2;
	BOOL			bAlignPtSet;		// TRUE when FPC Alignment pts are set
	PRU				*pPBPRU1;
	PRU				*pPBPRU2;

	// Pick Pt Related
	D_MTR_POSN		dmtrAlignPickPt1;
	D_MTR_POSN		dmtrAlignPickPt2;
	BOOL			bAlignPickPtSet;

	OFFSET			stPickOffset;
	PRU				*pFPCPRU1;
	PRU				*pFPCPRU2;

	// ChipPickArm
	D_MTR_POSN		dmtrAlignFPCPt1;
	D_MTR_POSN		dmtrAlignFPCPt2;
	BOOL			bAlignFPCPtSet;

	PRU				*pFPCUPLOOKPRU1;
	PRU				*pFPCUPLOOKPRU2;


} FPC_TYPE_INFO;


class CHouseKeeping : public CAC9000Stn
{
	DECLARE_DYNCREATE(CHouseKeeping)
	friend class CAC9000App;
	friend class CAC9000Stn; //20130121
//	friend class CMainBond;
//#ifdef MB_FIND_CONTACT_LEVEL_DIFF //20130615
//	friend class CMB1;
//	friend class CMB2;
//#endif

	friend class CTA1;
	friend class CTA2;
	friend class CTAManager;
	friend class CInPickArm;
	friend class CWorkHolder;
	friend class CACFWH;
	friend class CPRTaskStn;
	friend class CWinEagle;

protected:
	

protected:
	//Update
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();

	virtual VOID RegisterVariables();
	virtual VOID PreStartOperation();
	virtual VOID AutoOperation();
	virtual VOID StopOperation();

public:
	CHouseKeeping();
	virtual	~CHouseKeeping();

	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID Operation();

	//BOOL m_bHouekeepingDebug; //20150714
protected:
	// ------------------------------------------------------------------------
	// Variable delcaration
	// ------------------------------------------------------------------------
	DOUBLE	m_dPrevSWVersion;
	DOUBLE  m_dCurSWVersion;

	// Add NuMotion 
	// 
	//CSiInfo		m_stHTPwrOnSnr;
	CSiInfo		m_stSMPS200VSnr;
	CSiInfo		m_stPwrOFF;
	CSiInfo		m_stAirSupplySnr;	//20120703
	CSiInfo		m_stEMODetectSnr;				// Hit EMO and disconnect with HiPEC
	CSiInfo		m_stMainPowerFaultSnr;			// 3 Phase Power Fault
	//CSiInfo		m_stSafetyLightCurtainSnr;		//20121204

	CSiInfo		m_stACF1IonizerCheckSnr;
	CSiInfo		m_stACF1IonizerErrorSnr;



	CSiInfo		m_stACFFrontDoorLockSnr;
	CSiInfo		m_stACFBackDoorLockSnr;

	CSoInfo		m_stACFFrontDoorLockSol;
	CSoInfo		m_stACFBackDoorLockSol;

	CSiInfo		m_stFH1Snr;
	CSiInfo		m_stFH2Snr;
	CSiInfo		m_stFL1Snr;
	CSiInfo		m_stFL2Snr;
	CSiInfo		m_stSPSnr;
	CSiInfo		m_stAirSupplyCleanerSnr;
	CSiInfo		m_stCEBypassKeyOnSnr;

	CSiInfo		m_stCleanVacSnr;
	CSiInfo		m_stCleanPreSnr;

	CSoInfo		m_stRedLight;
	CSoInfo		m_stOrangeLight;
	CSoInfo		m_stGreenLight;
	CSoInfo		m_stBuzzer;

	CSoInfo		m_stCleanVacSol;	
	CSoInfo		m_stCleanPreSol;
	
	//CSoInfo		m_stPBHTPwrOnSol;
	//CSoInfo		m_stMBOddIndexRollSol;
	//CSoInfo		m_stMBEvenIndexRollSol;

	///////////////////////////////////////////////
	//Start, STOP, Reset Buttons
	CSiInfo		m_stStartButtonSnr;
	CSiInfo		m_stStopButtonSnr;
	CSiInfo		m_stResetButtonSnr;

	CSoInfo		m_stStartButtonSol;
	CSoInfo		m_stResetButtonSol;	




	// ------------------------------------------------------------------------
	// End of Motor SI/SO
	// ------------------------------------------------------------------------

	BOOL	m_bErrorExist;
	BOOL	m_bAlertExist;
	BOOL	m_bBeepSound; //20140704
	CString	m_szLastError;	// 20140814 Yip

	ULONG	m_ulDetectCover;
	BOOL	m_bDetectCoverSlowMotion;
	BOOL	m_bDetectCoverStopOp;
	BOOL	m_bDetectAirSupply;
	BOOL	m_bDetectLightCurtainSnr;
	BOOL	m_bDetectEMO;
	BOOL	m_bDetectMainPowerFault;
	BOOL	m_bDetectSafetyLightCurtainSnr; //20121204

	BOOL	m_bTrayReadySwitchEnable; //20131004
		
	LONG	m_lAirErrorCounter;

#if 1 //20121009 pin detection spring test
	BOOL			m_bPinTest;
#endif
#ifdef PB_LSI_EXPANDSION //20150303
	BOOL			m_bPRLoopTest;
#endif

	BOOL HMI_bStopProcess; //20121225
	BOOL HMI_bChangeACF;	// 20140911 Yip
	
	BOOL	m_bEnableStartBtn;
	BOOL	m_bEnableResetBtn;
	BOOL	m_bEnableStopBtn;
	BOOL	m_bPressBtnToStop;

	LONG	m_lACFBondedAlarmCount; //20130709
	LONG	m_lACFBondedAlarmLimit;

	// Pakage Information
	
	// ------------------------------------------------------------------------
	// Function delcaration
	// ------------------------------------------------------------------------
	VOID OperationSequence();

	VOID SetMotionProfileMode(ULONG ulMode);

	// I/O
	//BOOL IsPushBtnX1On();
	//BOOL IsPushBtnX2On();
	
	BOOL IsAirSupplyOn();

	BOOL IsEMODetectSnrOn();
	BOOL IsMainPowerFaultSnrOn();
	//BOOL IsSafetyLightCurtainSnrOn(); //20121204
	BOOL IsStartBtnOn();
	BOOL IsResetBtnOn();
	BOOL IsStopBtnOn();

	BOOL IsACFFrontCoverOn(); 
	BOOL IsACFBackCoverOn(); 
	
	INT SetRedSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetOrangeSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetGreenSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetBuzzerSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetStartButtonLampSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetResetButtonLampSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);

	INT SetACFFrontDoorLockSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetACFBackDoorLockSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);

	INT SetCleanVacSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetCleanPreSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);

	VOID ResetSystem();
	VOID StartBonding();
	VOID StopBonding();

	DOUBLE GetSoftwareVersion(CString Number);
	
	//Print Data
	BOOL	PrintHeader(CString szStation, CString szEvent);
	BOOL	PrintTitle(CString szEvent, CString szTitle[]);
	BOOL	PrintData(CString szEvent, PRINT_DATA stPrintData);
	BOOL	IsDataFileExist(CString szEvent);	// 20140619 Yip

	virtual VOID	PrintMachineSetup(FILE *fp);
	virtual VOID	PrintDeviceSetup(FILE *fp);
#ifdef EXCEL_MACHINE_DEVICE_INFO
	virtual BOOL printMachineDeviceInfo();
#endif

protected:
	CMotorInfo	m_stMotorEncoderOnlyTmp1; //whz rotary linear calib
	LONG	m_lHMI_EncoderOnlyTmp1CurrPosn; //whz rotary linear calib
	// ------------------------------------------------------------------------
	// HMI Variable delcaration
	// ------------------------------------------------------------------------
	CString szUserPassword;
	CString szEngPassword;
	CString szSvrPassword;

	LONG	HMI_lAutoMode;

	BOOL	HMI_bAutoTestStop;//20140923
	
	//Package Info
	CString	HMI_szACFName;
	DOUBLE	HMI_dACFWidth;
	DOUBLE	HMI_dACFLength;

	BOOL	HMI_bIndexTeflonTest;

	// ------------------------------------------------------------------------
	// HMI Command delcaration
	// ------------------------------------------------------------------------

	LONG HMI_PRAutoTestStop(IPC_CServiceMessage &svMsg); //20140923
#ifdef PB_LSI_EXPANDSION //20150303
	LONG HMI_StopPRLoopTest(IPC_CServiceMessage &svMsg);
#endif
	LONG HMI_TA1ZMoveLoopStop(IPC_CServiceMessage &svMsg); //20150615
	//LONG HMI_SetDebugVar(IPC_CServiceMessage &svMsg); //20150714

	//SI/SO
	LONG HMI_ToggleRedSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleOrangeSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleGreenSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleBuzzerSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleStartBtnSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleResetBtnSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleACFFrontDoorLockSol(IPC_CServiceMessage &svMsg);	// 20150204
	LONG HMI_ToggleACFBackDoorLockSol(IPC_CServiceMessage &svMsg);	// 20150204
	
	LONG HMI_ToggleCleanVacSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleCleanPreSol(IPC_CServiceMessage &svMsg);


	//new sol (total 4)
	//INT SetHTPwrOnSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetMBBarIonPwrEnableSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);  // 20141029
	INT SetPBBarIonPwrEnableSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);  // 20141029
	INT SetMBFrontDoorLockSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);		// 20150204
	
	//INT SetPBHTPwrOnSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	//INT SetMBOddIndexRollSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	//INT SetMBEvenIndexRollSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);

	
	//LONG HMI_ToggleHTPwrOnSol(IPC_CServiceMessage &svMsg);

	//LONG HMI_ToggleMBOddIndexRollSol(IPC_CServiceMessage &svMsg);
	//LONG HMI_ToggleMBEvenIndexRollSol(IPC_CServiceMessage &svMsg);

#if 1 //20121009 pin detection spring test
	LONG HMI_testPinDetectSpringStop(IPC_CServiceMessage &svMsg);
#endif
	VOID LockAllCovers();
	VOID UnLockAllCovers();
	// ------------------------------------------------------------------------
	// Setting
	// ------------------------------------------------------------------------
	LONG HMI_BackUpPackages(IPC_CServiceMessage &svMsg);
	LONG HMI_ChangePassword(IPC_CServiceMessage &svMsg);
	LONG HMI_LangSwitching(IPC_CServiceMessage &svMsg);

	LONG HMI_SetAutoMode(IPC_CServiceMessage &svMsg);

	LONG HMI_SetForceCalibHeight(IPC_CServiceMessage &svMsg);
	LONG HMI_SelectShowForceMode(IPC_CServiceMessage &svMsg);

	LONG HMI_SetModuleOffset(IPC_CServiceMessage &svMsg);


	LONG HMI_SetDetectCover(IPC_CServiceMessage &svMsg);
	LONG HMI_SetDetectLightCurtainSnr(IPC_CServiceMessage &svMsg);
	LONG HMI_SetDetectAirSupply(IPC_CServiceMessage &svMsg);

	LONG HMI_SetSafeIndexing(IPC_CServiceMessage &svMsg);
	LONG HMI_SetSlowMotionIndexing(IPC_CServiceMessage &svMsg);
	LONG HMI_SetSlowMotionFactor(IPC_CServiceMessage &svMsg);
	LONG HMI_SetEnableTrigLighting(IPC_CServiceMessage &svMsg);

	LONG HMI_CheckUserGroup(IPC_CServiceMessage &svMsg);

	// ------------------------------------------------------------------------
	// Package Information
	// ------------------------------------------------------------------------
	// package information
	// Glass Info. Related
	LONG HMI_SetGlassName(IPC_CServiceMessage &svMsg);
	LONG HMI_SetGlassLength(IPC_CServiceMessage &svMsg);
	LONG HMI_SetGlassWidth(IPC_CServiceMessage &svMsg);
	LONG HMI_SetReflectorThickness(IPC_CServiceMessage &svMsg);
	LONG HMI_SetUpperGlassThickness(IPC_CServiceMessage &svMsg);
	LONG HMI_SetLowerGlassThickness(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPolarizerThickness(IPC_CServiceMessage &svMsg);
	LONG HMI_SetGlassItoWidth(IPC_CServiceMessage &svMsg);

	// ACF Info. Related
	LONG HMI_SetACFName(IPC_CServiceMessage &svMsg);
	LONG HMI_SetACFWidth(IPC_CServiceMessage &svMsg);
	LONG HMI_SetACFLength(IPC_CServiceMessage &svMsg);
	LONG HMI_SetACFWasteCollectionLimit(IPC_CServiceMessage &svMsg); //20130709
#ifdef EXCEL_OFFLINE_SETUP
	INT OfflineSetupCheckInputValid(OFFLINE_INPUT_ITEM emItem, DOUBLE dValue);
#endif
	// LSI Type Info. Related
	LONG HMI_SetLSIType1Name(IPC_CServiceMessage &svMsg);
	LONG HMI_SetLSIType1Length(IPC_CServiceMessage &svMsg);
	LONG HMI_SetLSIType1Width(IPC_CServiceMessage &svMsg);
	LONG HMI_SetLSIType1Thickness(IPC_CServiceMessage &svMsg);

	// LSI On Glass Info. Related

	// MB Press Info

	// Rework Mode
	LONG HMI_SelectedReworkMode(IPC_CServiceMessage &svMsg);

	//Glass Input Mode 20120412
	LONG HMI_SetGlassInputMode(IPC_CServiceMessage &svMsg);

	LONG HMI_LightAndBuzzerInIdle(IPC_CServiceMessage &svMsg); //20130115

//p20120918
	// Traditional Chinese Switching
	LONG HMILangTraditionalChinese(IPC_CServiceMessage &svMsg);
//p20120918:end
	BOOL checkReceivedStatus(GATEWAY_STATUS_TYPE lStatus);
	LONG GatewayUpdateGC902CWHStatus(IPC_CServiceMessage &svMsg);
	LONG CheckGatewayConnection(IPC_CServiceMessage &svMsg);
	LONG GetGatewayTA5Status(IPC_CServiceMessage &svMsg);
	LONG GatewayUpdateCOG902TA5Status(IPC_CServiceMessage &svMsg);
	LONG GatewayUpdateCOG902MachineStatus(IPC_CServiceMessage &svMsg);
	INT getGatewayString(IPC_CServiceMessage &svMsg, LONG &lMsgLength, CString &szResult, LONG lMaxMsgLength = 100);
	INT getGatewayLong(IPC_CServiceMessage &svMsg, LONG &lMsgLength, LONG &lResult);

	LONG ProcessGatewayCommand(IPC_CServiceMessage &svMsg);

	LONG HMI_SetTrayReadySwitchEnable(IPC_CServiceMessage &svMsg);	// 20140630 Yip

	LONG HMI_SetChangeACF(IPC_CServiceMessage &svMsg);	// 20140917 Yip

	// Saving / loading parameters
	virtual LONG IPC_SaveMachineParam();
	virtual LONG IPC_LoadMachineParam();
	virtual LONG IPC_SaveDeviceParam();
	virtual LONG IPC_LoadDeviceParam();

};
