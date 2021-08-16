/////////////////////////////////////////////////////////////////
//	ACF.cpp : interface of the CACF class
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

#ifdef EXCEL_MACHINE_DEVICE_INFO
#include "ExcelMachineInfo_Util.h"
#endif

#define FORCE_AMP_RANGE_I		20000.0				//Range I 20000pC
#define FORCE_AMP_RANGE_II		5000.0				//Range II 5000pC

#ifdef ACF_EXTERN
extern BOOL					g_bStopCycle;

extern AUTOMODE				AutoMode;
extern BOOL					g_bDiagMode;
extern LONG					g_lDiagnSteps;
	
extern STATUS_TYPE			ACF_Status[MAX_NUM_OF_ACF_UNIT];
extern STATUS_TYPE			ACF_WH_Status[MAX_NUM_OF_ACFWH];

extern BOOL					g_bShowForceInNewton;

extern ULONG				g_ulUserGroup;
extern SETUP_PARA			g_stSetupPara[NUM_OF_PARA];
extern SETUP_POSN_XY		g_stSetupPosn[NUM_OF_POSN_XY];
extern FORCE_CALIBRATOR		g_stForceCalibrator;

extern CCriticalSection	m_csForceSensor; //20161117
extern BOOL m_bForceLogRun;


#endif

//extern BOOL m_bMBForceLogRun[MAX_NUM_OF_MAINBOND];

#define FORCE_SENSOR_LOG_PATH			"D:\\SW\\AC9000\\Data\\Calculation Log\\"
#define FORCE_SENSOR_LOG_FILENAME		"Force_Sensor_Log_during_bondDelay.csv"

#define FORCE_SENSOR_ENDOFBOND_LOG_PATH			"D:\\SW\\AC9000\\Data\\Calculation Log\\"
#define FORCE_SENSOR_ENDOFBOND_LOG_FILENAME	"Force_Sensor_Log_EndOfBond"

#define	INDEX_MOTOR_TO_ENC_RATIO	(1.6)

// Control Signals
//
typedef enum
{
	ACF_BF_CTRL_NOTHING = 0, 
	ACF_BF_CTRL_OFF, 
	ACF_BF_CTRL_DAC_ON, 

} ACF_BF_CTRL_SIGNAL;


typedef enum
{
	ACF_Z_CTRL_NOTHING = 0, 
	ACF_Z_CTRL_GO_STANDBY_LEVEL,  
	ACF_Z_CTRL_GO_CALIB_LEVEL, 
	ACF_Z_CTRL_GO_BOND_LEVEL, 
	ACF_Z_CTRL_GO_SEARCH_LEVEL, 
	ACF_Z_CTRL_SRCH_CALIB_LEVEL,
	ACF_Z_CTRL_SRCH_CONTACT_LIMIT,
	ACF_Z_CTRL_GO_FORCE_CALIB_BOND_LEVEL,
	ACF_Z_CTRL_GO_SRCH_FORCE_CALIB_BOND_CONTACT_LEVEL,
	ACF_Z_CTRL_GO_SRCH_FORCE_CALIB_BOND_CONTACT_LEVEL_AND_PRESS,
	ACF_Z_CTRL_GO_SRCH_FORCE_CALIB_POSTBOND_CONTACT_LEVEL,
	ACF_Z_CTRL_GO_SRCH_POSTBOND_CONTACT_LEVEL,

} ACF_Z_CTRL_SIGNAL;

typedef enum
{
	ACF_Z_SRCH_CONTACT_SNR_ON_LEVEL,
	ACF_Z_SRCH_CONTACT_SNR_OFF_LEVEL,

	// ALERT! must be less than MAX_NUM_OF_SRCH_PROF

} ACFMotorZSearchProf;

typedef enum
{
	PROCESS_HEAD,

} ACFProcessBlkNum;

typedef enum
{
	ACF_INDEXER_CTRL_NOTHING = 0, 
	ACF_INDEXER_INDEX_TAPE, 

} ACF_INDEXER_CTRL_SIGNAL;

typedef enum
{
	ACF_ROLLER_CTRL_NOTHING = 0, 
	ACF_ROLLER_CTRL_SEARCH_UPPER,
	ACF_ROLLER_CTRL_SEARCH_LOWER,

} ACF_ROLLER_CTRL_SIGNAL;

typedef enum
{
	ACF_ROLLER_SRCH_FEEDER_BUFFER_SNR_ON_LEVEL,

	// ALERT! must be less than MAX_NUM_OF_SRCH_PROF

} ACFMotorRollerSearchProf;

// States
//
typedef enum
{
	ACF_IDLE_Q = 0, 
	ACF_WAIT_REQUEST_Q,
	ACF_BOND_OPERATION_Q,
	ACF_STRIPPER_ON_Q,
	ACF_STRIPPER_OFF_Q,
	ACF_INDEX_OPERATION_Q,
	ACF_INDEX_LAST_OPERATION_Q,

	ACF_STOP_Q = 99, 

} ACF_STATE;

typedef enum
{
	ACF_STRIPPER_X_CTRL_NOTHING = 0,
	ACF_STRIPPER_X_CTRL_GO_HOME_POSN, 
	ACF_STRIPPER_X_CTRL_GO_STANDBY_POSN, 
	ACF_STRIPPER_X_CTRL_GO_OUT_POSN,
	ACF_STRIPPER_X_CTRL_GO_OUT_OFFSET,

} ACF_STRIPPER_X_CTRL_SIGNAL;


typedef enum
{
	ACF_X_CTRL_NOTHING = 0,
	ACF_X_CTRL_GO_HOME_POSN, 
	ACF_X_CTRL_GO_STANDBY_POSN, 

} ACF_X_CTRL_SIGNAL;


class CACFWH;

DWORD WINAPI LogADCForce(LPVOID lpParam);
//DWORD WINAPI DummyThread(LPVOID lpParam);

class CACF : public CAC9000Stn
{
	DECLARE_DYNCREATE(CACF)
	friend class CAC9000App;
	friend class CSettingFile;
	friend class CHouseKeeping;

	friend class CACFWH;
	friend class CTempDisplay;

	friend class CINWH; //20130122

protected:
	

protected:
	//Update
	VOID UpdateOutput();
	VOID UpdateCurPressureValue();

	virtual VOID RegisterVariables();
	virtual VOID PreStartOperation();
	virtual VOID CycleOperation();
	virtual VOID AutoOperation();

private:
	virtual CString		GetStation() 
	{
		return "";
	};

public:
	CACF();
	virtual	~CACF();

	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID Operation();

	BOOL GetADCValue(LONG &lForce);
	DOUBLE RawADCToForce(LONG lRawData, DOUBLE dCperLbf);

protected:
	CACFWH		*m_pCACFWH;
	CACFWH		*m_pCOtherACFWH;
	// ------------------------------------------------------------------------
	// Variable delcaration
	// ------------------------------------------------------------------------
	CMotorInfo	m_stMotorIndex;
	CMotorInfo	m_stMotorRoller;
	CMotorInfo	m_stMotorStripperX;
	CMotorInfo	m_stMotorX;
	CMotorInfo	m_stMotorZ;
	CMotorInfo	m_stMotorBF;

	//SI
	CSiInfo		m_stACFFeederBufferSnr;
	CSiInfo		m_stACFStripperHomeSnr;
	CSiInfo		m_stACFCutterDnPosnSnr;
	CSiInfo		m_stACFCutterPressureSnr;
	CSiInfo		m_stACFTapeEndSnr;
	CSiInfo		m_stACFIonizerCheckSnr;
	CSiInfo		m_stACFIonizerErrorSnr;
	CSiInfo		m_stACFTapeDetectSnr;
	CSiInfo		m_stACFZHomeSnr;
	CSiInfo		m_stACFXHomeSnr;




	//CSiInfo		m_stHeadUpSnr;
	//CSiInfo		m_stFeederBufferSnr;
	//CSiInfo		m_stStripperHomeSnr;	//20150204
	////CSiInfo		m_stStripperOnSnr;
	////CSiInfo		m_stStripperOffSnr;
	//CSiInfo		m_stCutterDownPosnSnr;
	//CSiInfo		m_stPressureSnr;
	//CSiInfo		m_stCutterPressureSnr;
	//CSiInfo		m_stTapeEndSnr; //20121122
	//CSiInfo		m_stIonizerCheckSnr;
	//CSiInfo		m_stIonizerErrorSnr;



	CSoInfo		m_stACFForceAmpOpSol;
	CSoInfo		m_stACFForceAmpRangellSol;
	CSoInfo		m_stACFCutterOnSol;
	CSoInfo		m_stACFIonizerPowerEnSol;


	CPortInfo	m_stDACPort;

	CPortInfo	m_stADCPort; //20151229
	
	// ------------------------------------------------------------------------
	// End of Motor SI/SO
	// ------------------------------------------------------------------------	

	LONG		m_lCurSlave;	//only one ACF; = ACF_1
	LONG		m_lCurrWH;		//ACFWH_1, ACFWH_2

	// ------------------------------------------------------------------------
	// End of CHECK Who Am I
	// ------------------------------------------------------------------------
	SRCH_PROF_PARA		m_stSrchFreederBuffer;
	
	SRCH_PROF_PARA		m_stSrchDefault;
	SRCH_PROF_PARA		m_stSrchForceCalibBondLevel;
	SRCH_PROF_PARA		m_stSrchForceCalibPostBondLevel;
	SRCH_PROF_PARA		m_stSrchBondLevel;
	SRCH_PROF_PARA		m_stSrchPostBondLevel;
	
	// cycle status
	DOUBLE	m_dACFProcessTime; 
	
	//Force
	BOOL	m_bDACValid;
	DOUBLE	m_dPressureCtrl;				//inital
	LONG	m_lFinalDACCtrl[MAX_NUM_OF_ACF];
	DOUBLE	m_dFinalPressureCtrl[MAX_NUM_OF_ACF];
	DOUBLE	m_dUpPressureCtrl;

	// ACF Indexing Related
	BOOL	m_bValidIndexedLength;
	BOOL	m_bACFIndexed[MAX_NUM_OF_ACF];				// for one cycle only
	ACFNum	m_emACFIndexed[MAX_SIZE_OF_ACF_ARRAY];
	ACFNum	m_emLastIndexedACF;
	INT		m_nIndexFirst;
	INT		m_nIndexLast;

	DOUBLE	m_dIndexerFactor;
	DOUBLE	m_dIndexerCustomFactor;	//20120706

	LONG	m_lCurNumOfPressCount; //20150416
	static LONG	m_lNumOfPressBeforeAlert;
	BOOL	m_bChangeRubberBandAlertShown;


	//Level

	LONG				m_lStandbyLevel;
	LONG				m_lCalibLevel;
	LONG				m_lBondLevel;
	LONG				m_lBondLevelOffset;

	// Posn
	LONG	m_lStandbyPosnACF;	// 20150205
	LONG	m_lStandbyPosn;	// 20150205
	LONG	m_lOutPosn;
	LONG	m_lOutOffset;	// 20150212

	// Positions
	// Index ACF Related
	// ACF Cutter to Head Distance
	DOUBLE	m_dCutterToHeadMachineDist;					// ACF Cutter to Head Distance (Hardcode)
	DOUBLE	m_dCutterToHeadDistance;					// ACF Cutter to Head Distance (in mm)
	DOUBLE	m_dCutterToHeadOffset;						// ACF Cutter to Head Offset (in mm)

	DOUBLE	m_dRemainingLength;							// (in mm)
	DOUBLE	m_dAdvancedLength;							// (in mm)
	DOUBLE	m_dIndexedLength;							// (in mm)

	//Delays
	LONG	m_lPreAttachDelay;
	LONG	m_lAttachACFDelay;
	LONG	m_lPressTestDelay;

	// force calibration
	LONG				m_lCalibPreAttachDelay;
	LONG				m_lCalibAttachDelay;
	BOOL				m_bCalibDACValid;
	DOUBLE				m_dCalibFinalPressureCtrl;
	DOUBLE				m_dCalibUpPressureCtrl;
	DOUBLE				m_dCalibPressureCtrl;
	DOUBLE				m_dCalibForceCtrl;
	LONG				m_lForceCalibPressCycle;
	LONG				m_lCalibFinalDACCtrl;

	static LONG				m_lCurrBondForceMode;
	FORCE_DATA			m_stForceCalib[MAX_BOND_FORCE_MODE][MAX_FORCE_DATA];

	DOUBLE			m_dStripperXGoMoveLeftSpeedFactor;
	DOUBLE			m_dStripperXGoMoveRightSpeedFactor;

public:
	static BOOL	m_bLogForceSensor;
	static BOOL	m_bLogForceSensorEndOfBond;; //20161117 log force at the end of bond
	DOUBLE	m_dForceKgEndOfBonding;
	LONG	m_lForceADCEndOfBonding;
	LONG	m_lForceADCValue;		//raw value from NU-Motion
	DOUBLE	m_dForceBondWeight;		//lbf or kgf weight of force sensor
	DOUBLE	m_dForceSensorFactor;	//Force sensor factor (each sensor should has its own value)
	CString m_szLogLabel;
protected:
	HANDLE m_hThreadForceADC;
	// ------------------------------------------------------------------------
	// Function delcaration
	// ------------------------------------------------------------------------
	CString GetSequenceString();
	VOID	OperationSequence();
	VOID	OperationSequence_Single_ACF();

	BOOL	PressOperation(LONG lPreAttachDelay, LONG lBondDelay, DOUBLE dPressureCtrl, LONG lFinalPressure, DOUBLE dUpPressureCtrl, BOOL bIndexACF = TRUE);
	VOID	ForceCalibPressOperation();
	//VOID	SinglePressOperation();
	VOID	SinglePressOperation(BOOL bIsMoveStripper = FALSE); //20130318

	VOID	ClearACFArray();
	BOOL	InsertACFArray();
	ACFNum	RemoveACFArray();
	VOID	UpdateACFArrayDisplay();
	ACFNum	GetNextArrayACFNum();
	INT		GetCurrArraySize();
	VOID	CalcIndexedLength();			// Calculate the length from IndexLast to m_emLastIndexedACF
	
	BOOL	CutACFOperation(BOOL bReset = TRUE);	// Reset m_dAdvancedLength?
	INT		IndexToACFHeadOperation();
	INT		IndexToACFHeadOperation_Single_ACF();
	VOID	RecoverACFArrayOperation();
	INT		IndexNextACFOperation();
	INT		IndexNextACFOperation_Single_ACF();
	BOOL	SingleCycleOperation(ACFNum nACF = ACF1);
	BOOL	AttachACFOperation(ACFNum nACF);
	BOOL	ForceCalibration();

	//Motion
	INT MoveRelativeIndexer(DOUBLE dSteps, BOOL bWait);
	INT MoveIndexer(ACF_INDEXER_CTRL_SIGNAL ctrl, BOOL bWait);
	INT MoveRoller(ACF_ROLLER_CTRL_SIGNAL ctrl, BOOL bWait);
	// 20150205
	INT MoveStripperX(ACF_STRIPPER_X_CTRL_SIGNAL ctrl, BOOL bWait);
	
	// I/Os
	BOOL IsTurnBufferLmtSnrOn();
	BOOL IsCutterDownPosnSnrOn();
	BOOL IsTapeEndSnrOn(); //20121122
	BOOL IsTapeDetectSnrOn();

	//INT SetHeadDownSol(BOOL bMode, BOOL bWait = SFM_NOWAIT);
	//INT SetCounterBalanceSol(BOOL bMode, BOOL bWait = SFM_NOWAIT);
	INT SetHalfCutterOnSol(BOOL bMode, BOOL bWait = SFM_NOWAIT);
	//INT SetHeadUpSol(BOOL bMode, BOOL bWait = SFM_NOWAIT);	
	//INT SetStripperSol(BOOL bMode, BOOL bWait = SFM_NOWAIT);
	INT SetIonizerPowerEnable(BOOL bMode, BOOL bWait = SFM_NOWAIT);
	INT SetForceChargeAmpOperateSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetForceChargeAmpRangeSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	
	//Action Related
	INT IndexACFIndexer(BOOL bAll = FALSE);
	INT SetDACValue(DOUBLE dPressure);
	INT CheckACFReady();

	// Module Setup Related
	VOID SetDiagnSteps(ULONG ulSteps);
	VOID ModuleSetupPreTask();
	VOID ModuleSetupPostTask();
	
	BOOL ModuleSetupSetPosn(LONG lSetupTitle = 0);
	BOOL ModuleSetupGoPosn(LONG lSetupTitle = 0);
	BOOL ModuleSetupSetPara(PARA stPara);
	
	VOID UpdateModuleSetupPara();
	VOID UpdateModuleSetupPosn();	// 20150205

	INT SetupGoStandbyPosnACF();	// 20150205
	INT SetupGoStandbyPosn();	// 20150205
	INT SetupGoOutPosn();		// 20150209
	INT SetupGoOutOffset();		// 20150212

	INT SetupGoStandbyLevel();	// 20150205
	INT SetupGoCalibLevel();		// 20150209
	INT SetupGoBondLevel();		// 20150212

	INT MoveBF(ACF_BF_CTRL_SIGNAL ctrl, BOOL bWait);
	INT MoveZ(ACF_Z_CTRL_SIGNAL ctrl, BOOL bWait);
	INT MoveX(ACF_X_CTRL_SIGNAL ctrl, BOOL bWait);
	INT MoveRelativeX(LONG lCntX, BOOL bWait);
	INT MoveAbsoluteX(LONG lCntX, BOOL bWait);	

	virtual INT SyncX();
	INT SyncStripperX();
	INT SyncRoller();
	INT SyncIndex();

	virtual VOID PrintMachineSetup(FILE *fp);
	virtual VOID PrintDeviceSetup(FILE *fp);
	virtual VOID PrintForceCalibData(FILE *fp);
	virtual VOID PrintCalibDataToHMI();
#ifdef EXCEL_MACHINE_DEVICE_INFO
	virtual BOOL printMachineDeviceInfo();
#endif

	LONG HMI_SetLogForceSensor(IPC_CServiceMessage &svMsg);
	LONG HMI_SetLogForceSensorEndOfBond(IPC_CServiceMessage &svMsg); //20161117
	LONG HMI_SetForceSensorFactor(IPC_CServiceMessage &svMsg);
	LONG HMI_MBReadADCForce(IPC_CServiceMessage &svMsg);
	INT LogForceEndOfBond();
	INT StartLogForceThread();
	INT StartDummpThread(); //20171116

	// ------------------------------------------------------------------------
	// HMI Variable delcaration
	// ------------------------------------------------------------------------
	// Index ACF Related
	CString	HMI_szACFArray;
	
	LONG	HMI_lFinalDACCtrl;
	DOUBLE	HMI_dFinalPressureCtrl;
	DOUBLE	HMI_dFinalForceCtrl;

	DOUBLE	HMI_dCalibForceCtrl;

	// ------------------------------------------------------------------------
	// HMI Command delcaration
	// ------------------------------------------------------------------------

	//Motor
	LONG HMI_ToggleModSelected(IPC_CServiceMessage &svMsg); //20121112
	LONG HMI_PowerOnIndex(IPC_CServiceMessage &svMsg);
	LONG HMI_PowerOnRoller(IPC_CServiceMessage &svMsg);
	LONG HMI_PowerOnStripperX(IPC_CServiceMessage &svMsg);		//20150205
	LONG HMI_PowerOnX(IPC_CServiceMessage &svMsg);		//20150205
	LONG HMI_PowerOnZ(IPC_CServiceMessage &svMsg);		//20150205
	LONG HMI_PowerOnForceZ(IPC_CServiceMessage &svMsg);		//20150205


	//LONG HMI_ToggleHeadDownSol(IPC_CServiceMessage &svMsg);
	//LONG HMI_ToggleCounterBalanceSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleCutterOnSol(IPC_CServiceMessage &svMsg);
	//LONG HMI_ToggleHeadUpSol(IPC_CServiceMessage &svMsg);
	//LONG HMI_ToggleStripperSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleForceChargeAmpOperateSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleForceChargeAmpRangeSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleIonizerPowerEnable(IPC_CServiceMessage &svMsg);
	LONG HMI_ResetNumOfPressCount(IPC_CServiceMessage &svMsg);		//20150416

	LONG HMI_SetDiagnSteps(IPC_CServiceMessage &svMsg);
	LONG HMI_IndexCCW(IPC_CServiceMessage &svMsg);
	LONG HMI_IndexCW(IPC_CServiceMessage &svMsg);
	LONG HMI_RollerCCW(IPC_CServiceMessage &svMsg);
	LONG HMI_RollerCW(IPC_CServiceMessage &svMsg);
	LONG HMI_RollerGoSearchUpper(IPC_CServiceMessage &svMsg);
	LONG HMI_IndexZNeg(IPC_CServiceMessage &svMsg);	// 20150205
	LONG HMI_IndexZPos(IPC_CServiceMessage &svMsg);	// 20150205
	LONG HMI_CommZ(IPC_CServiceMessage &svMsg);	// 20150205
	LONG HMI_CommForceZ(IPC_CServiceMessage &svMsg);	// 20150205
	LONG HMI_HomeZ(IPC_CServiceMessage &svMsg);	// 20150205
	LONG HMI_IndexXNeg(IPC_CServiceMessage &svMsg);	// 20150205
	LONG HMI_IndexXPos(IPC_CServiceMessage &svMsg);	// 20150205
	LONG HMI_CommX(IPC_CServiceMessage &svMsg);	// 20150205
	LONG HMI_HomeX(IPC_CServiceMessage &svMsg);	// 20150205
	LONG HMI_IndexStripperXNeg(IPC_CServiceMessage &svMsg);	// 20150205
	LONG HMI_IndexStripperXPos(IPC_CServiceMessage &svMsg);	// 20150205
	LONG HMI_CommStripperX(IPC_CServiceMessage &svMsg);	// 20150205
	LONG HMI_HomeStripperX(IPC_CServiceMessage &svMsg);	// 20150205




	//Force
	LONG HMI_SetDAC(IPC_CServiceMessage &svMsg);
	LONG HMI_SetFinalDAC(IPC_CServiceMessage &svMsg);
	LONG HMI_SetForce(IPC_CServiceMessage &svMsg);
	LONG HMI_SetUpDAC(IPC_CServiceMessage &svMsg);
	
	LONG HMI_SetBondForceMode(IPC_CServiceMessage &svMsg);

	LONG HMI_SetCutterToHeadOffset(IPC_CServiceMessage &svMsg);
	LONG HMI_SetCutterToHeadDistance(IPC_CServiceMessage &svMsg);

	LONG HMI_SetNumOfPressBeforeAlertLimit(IPC_CServiceMessage &svMsg); //20150416
	LONG HMI_SetPreAttachDelay(IPC_CServiceMessage &svMsg);
	LONG HMI_SetAttachACFDelay(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPressTestDelay(IPC_CServiceMessage &svMsg);

	LONG HMI_InsertForceCalibrationData(IPC_CServiceMessage &svMsg); //20150512
	LONG HMI_ResetForceCalibrationData(IPC_CServiceMessage &svMsg);

	// Force Calibrated
	LONG HMI_SetCalibPreAttachDelay(IPC_CServiceMessage &svMsg);
	LONG HMI_SetCalibAttachDelay(IPC_CServiceMessage &svMsg);
	LONG HMI_SetCalibFinalPressureCtrl(IPC_CServiceMessage &svMsg);
	LONG HMI_SetCalibUpPressureCtrl(IPC_CServiceMessage &svMsg);
	LONG HMI_SetCalibPressureCtrl(IPC_CServiceMessage &svMsg);
	LONG HMI_SetCalibForceCtrl(IPC_CServiceMessage &svMsg);
	LONG HMI_SetForceCalibPressCycle(IPC_CServiceMessage &svMsg);

	// Indexing ACF Related
	LONG HMI_InsertACFArray(IPC_CServiceMessage &svMsg);
	LONG HMI_RemoveACFArray(IPC_CServiceMessage &svMsg);
	LONG HMI_ClearACFArray(IPC_CServiceMessage &svMsg);
	LONG HMI_SetACFIndexerFactor(IPC_CServiceMessage &svMsg); //20120706

	//Action Related
	LONG HMI_CutACFOperation(IPC_CServiceMessage &svMsg);
	LONG HMI_IndexToACFHeadOperation(IPC_CServiceMessage &svMsg);
	LONG HMI_RecoverACFArrayOperation(IPC_CServiceMessage &svMsg);
	LONG HMI_IndexNextACFOperation(IPC_CServiceMessage &svMsg);
	LONG HMI_SingleCycleOperation(IPC_CServiceMessage &svMsg);
	LONG HMI_ForceCalibration(IPC_CServiceMessage &svMsg);
	LONG HMI_ForceCalibPressTest(IPC_CServiceMessage &svMsg);
	LONG HMI_SinglePressTest(IPC_CServiceMessage &svMsg);
	LONG HMI_StartPressTest(IPC_CServiceMessage &svMsg);

	// Saving / loading parameters
	virtual LONG IPC_SaveMachineParam();
	virtual LONG IPC_LoadMachineParam();
	virtual LONG IPC_SaveDeviceParam();
	virtual LONG IPC_LoadDeviceParam();


};
