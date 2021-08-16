/////////////////////////////////////////////////////////////////
//	TA1.cpp : interface of the CTA1 class
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

#include "PRTaskStn.h"

#define TRANSFERARM_EXTERN
#include "TransferArm.h"

#ifdef EXCEL_MACHINE_DEVICE_INFO
#include "ExcelMachineInfo_Util.h"
#endif

#ifdef TA1_EXTERN
extern BOOL					g_bStopCycle;
	
extern BOOL					g_bFlushMode;

extern STATUS_TYPE			TA1_Status;
extern STATUS_TYPE			TA1_GLASS_Status;
extern ACFWHUnitNum			TA1_POSN;
extern STATUS_TYPE			TA_MANAGER_Status;	
extern STATUS_TYPE			IN_PICKARM_Status;
extern STATUS_TYPE			ACF_WH_Status[MAX_NUM_OF_ACFWH];

extern STATUS_TYPE			TA2_Status;


extern GLASS_INFO			g_stGlass;
extern LONG					g_lGlassInputMode;

extern BOOL					GlassOnInBelt;
extern BOOL					g_bUplookPREnable;

	//wizard
extern BOOL					g_bWizardEnable;
extern CString				g_szWizardTitle;
extern ULONG				g_ulWizardSubTitle;
extern CString				g_szWizardStep;
extern CString				g_szWizardMess;
extern CString				g_szWizardMessPrev;
extern CString				g_szWizardMessNext;
extern CString				g_szWizardBMPath;
extern BOOL					g_bWizardEnableBack;
extern BOOL					g_bWizardEnableNext;
extern BOOL					g_bWizardEnableExit;
extern ULONG				g_ulWizardStep;
extern LONG					g_lWizardInProgress;


//extern GATEWAY_STATUS_TYPE	GC902_CWH_Status;
extern LONG					g_lGatewayTA1Status; //20130919

#endif

typedef enum
{
	TA1_X_CTRL_NOTHING = 0, 
	//TA1_X_CTRL_GO_HOME, 
	TA1_X_CTRL_GO_STANDBY_POSN, 
	TA1_X_CTRL_GO_PICK_GLASS_POSN,
	TA1_X_CTRL_GO_PLACE_ACF1WH_POSN,
	TA1_X_CTRL_GO_PLACE_ACF2WH_POSN, 
	TA1_X_CTRL_GO_PLACE_ACF1WH_OFFSET, 
	TA1_X_CTRL_GO_PLACE_ACF2WH_OFFSET,
	TA1_X_CTRL_GO_PLACE_ACF1WH_PR_OFFSET, 
	TA1_X_CTRL_GO_PLACE_ACF2WH_PR_OFFSET,

} TA1_X_CTRL_SIGNAL;

typedef enum
{
	TA1_Z_CTRL_NOTHING = 0,
	TA1_Z_CTRL_GO_HOME,
	TA1_Z_CTRL_GO_STANDBY_LEVEL,
	TA1_Z_CTRL_GO_LOWER_LEVEL,
	TA1_Z_CTRL_GO_PLACE_LEVEL,
	TA1_Z_CTRL_GO_PR_LEVEL, 
	TA1_Z_CTRL_GO_CALIB_PR_LEVEL,

} TA1_Z_CTRL_SIGNAL;

typedef enum
{
	TA1_Y_CTRL_NOTHING = 0,
	TA1_Y_CTRL_GO_STANDBY_POSN,

} TA1_Y_CTRL_SIGNAL;


// States
//
typedef enum
{
	TA1_IDLE_Q = 0, 

	TA1_WAIT_INPICKARM_REQUEST_Q,
	TA1_WAIT_INPICKARM_PLACE_COMPLETE_Q,
	TA1_CHECK_PICK_ERROR_Q,
	TA1_WAIT_TA_MANAGER_REQUEST_Q,		
	TA1_PLACE_COF_Q,					//5

	TA1_CHECK_PLACE_COF_Q,
	TA1_DO_2PT_PR_Q,
	TA1_GRAB_PR2_OK_Q,
	TA1_SEARCH_PR2_OK_Q,
	TA1_GRAB_N_SEARCH_PR1_OK_Q,				//10
	
	TA1_REJECT_COF_Q,
	TA1_DO_BIG_FOV_PR_Q,

	TA1_STOP_Q = 99,

} TA1_STATE;

class CTA1 : public CTransferArm, public CPRTaskStn
{
	DECLARE_DYNCREATE(CTA1)
	friend class CAC9000App;
	friend class CAC9000Stn;
	friend class CSettingFile;
	friend class CWinEagle;
	friend class CACFWH;
	friend class CInPickArm;
	friend class CTA2;
	friend class CHouseKeeping;

protected:
	

protected:
	VOID UpdateOutput();

	virtual VOID RegisterVariables();
	virtual VOID PreStartOperation();
	virtual VOID AutoOperation();

	virtual BOOL GetGlass1Status() 
	{
		return m_bGlass1Exist;
	}


private:
	virtual CString		GetStation() 
	{
		return "TA1_";
	}

public:
	CTA1();
	virtual	~CTA1();

	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID Operation();

protected:
	// ------------------------------------------------------------------------
	// Variable delcaration
	// ------------------------------------------------------------------------
	ACFWHUnitNum m_lCurrCheck;
	// 
	// ------------------------------------------------------------------------
	// End of Motor SI/SO
	// ------------------------------------------------------------------------

	// cycle status
	BOOL		m_bIsDefaultIndex;	// 1: Place Glass2 first 0: Place Glass1 first
	LONG		m_lCurrAssignPlace; // Try to pretend place to which Glass1 or Glass2

	LONG		m_lZMoveLoopLimit; 
	LONG		m_lZMoveLoopCount;
	BOOL		m_bZMoveLoopEnable;

	LONG		m_lXPlaceWhPROffset;	// X Place WH PR Offset
	LONG		m_lYPlaceWhPROffset;	// Y Place WH PR Offset
	LONG		m_lTPlaceWhPROffset;	// T Place WH PR Offset


	//GATEWAY_STATUS_TYPE GC902_CWH_Status_Received;
	BOOL		HMI_bStandAlone;

	//Posn
	MTR_POSN		m_mtrPRCalibPosn;
	MTR_POSN		m_mtrPR1Posn;
	MTR_POSN		m_mtrPR2Posn;
	D_MTR_POSN		m_dmtrPR1Offset;
	D_MTR_POSN		m_dmtrPR2Offset;
	
	MTR_POSN		m_mtrTA1PRPosn;
	MTR_POSN		m_mtrBigFOVPRPosn;
	D_MTR_POSN		m_dmtrBigFOVPROffset;

	//LONG	m_lPlaceLevel[MAX_NUM_OF_ACFWH];
	// ------------------------------------------------------------------------
	// Function delcaration
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	// All Virtual Function from PRTaskStn.cpp

	virtual CMotorInfo &GetMotorX(); 
	virtual CMotorInfo &GetMotorY();

	virtual INT SyncX();
	virtual INT SyncY();
	virtual INT SyncZ();

	virtual INT MoveXYToPRCalibPosn(BOOL bWait = GMP_WAIT);
	virtual INT MoveXYToCurrPR1Posn(BOOL bWait = GMP_WAIT);
	// ------------------------------------------------------------------------
	// END Virtual WorkHolder Function delcaration
	// ------------------------------------------------------------------------
	CString GetSequenceString();
	VOID OperationSequence();

	GlassNum AssignPlaceToWhere();
	VOID SetupWizard();
	VOID SetupPRPatternPreTask(PRU *pPRU);
	VOID SetupPRPatternPostTask();
	VOID SetupAlignPt();
	BOOL SetGlassRef(BOOL bSetAng);
	VOID CalcAlignPt();
	VOID UpdatePlaceWHPosnOffset();
	VOID PlaceOffsetEqualZero();
	BOOL IsRejectLimitHit();

	ACFUL_INFO	m_stACFUL;

	// Align Glass
	DOUBLE		m_dAlignAngle;			// store the current Glass Rotate Angle		
	MTR_POSN	m_mtrAlignCentre;			// store the current Glass Rotate Angle		
	MTR_POSN	m_mtrCurrAlignDelta;			// store the current Glass Delta x,y
	DOUBLE		m_dGlassRefAng;
	DOUBLE		m_dAlignRefAngle;
	D_MTR_POSN	m_dmtrAlignRef;	

	virtual PRU &SelectCurPRU(ULONG ulPRU);
	VOID ModulePRSetupPreTask();
	VOID ModulePRSetupPostTask();
	VOID UpdateSetupStatus();
	VOID SelectPRU(ULONG ulPRU, BOOL bIsMove = TRUE);
	LONG HMI_SetPRSelected(IPC_CServiceMessage &svMsg);
	//LONG HMI_SelectCurSlave(IPC_CServiceMessage &svMsg);
	LONG HMI_SelectPRU(IPC_CServiceMessage &svMsg);
	LONG HMI_UpdateSetupStatus(IPC_CServiceMessage &svMsg);
	LONG HMI_SearchAp1(IPC_CServiceMessage &svMsg);
	LONG HMI_SearchAp2(IPC_CServiceMessage &svMsg);
	LONG HMI_SearchCentre(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPRLevel(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPRPosn(IPC_CServiceMessage &svMsg);
	//LONG HMI_SetPR1Posn(IPC_CServiceMessage &svMsg);
	//LONG HMI_SetPR2Posn(IPC_CServiceMessage &svMsg);

	//INT PickGlass_DOWN();
	//INT PickGlass_UP();
	INT PlaceGlass(ACFWHUnitNum lACFWHnum);
	INT VacuumON(BOOL bWait);	
	INT BlowCOF();

	BOOL CheckPlaceStatus();
	//BOOL PickGlassOperation();
	BOOL PlaceGlassOperation(ACFWHUnitNum lACFWHnum);
	
	BOOL IsBigFOVPROK();
	BOOL IsRotaryUpperSnrOn();
	BOOL IsRotaryLowerSnrOn();
	BOOL IsFlipZHomeSnrOn();

	//Motor Related
	INT HomeZ();

	INT MoveX(TA1_X_CTRL_SIGNAL ctrl, BOOL bWait);
	INT MoveZ(TA1_Z_CTRL_SIGNAL ctrl, BOOL bWait);
	INT MoveY(TA1_Y_CTRL_SIGNAL ctrl, BOOL bWait);

	INT MoveXYToBigFOVPRPosn(BOOL bWait = GMP_WAIT);

	virtual INT MoveZToPRLevel(BOOL bWait);
	virtual INT MoveZToPRCalibLevel(BOOL bWait);
	virtual INT MoveXYToCurPRPosn(LONG lPRU);
	//virtual INT MoveXYToCurrPR1Posn(BOOL bWait = GMP_WAIT);
	virtual INT MoveXYToCurrPR2Posn(BOOL bWait = GMP_WAIT);	

	virtual INT MoveAbsoluteY(LONG lCntY, BOOL bWait);
	virtual INT MoveAbsoluteXY(LONG lCntX, LONG lCntY, BOOL bWait);
	virtual INT MoveRelativeXY(LONG lCntX, LONG lCntY, BOOL bWait);

	//I/O Related

	// Module Setup Related
	VOID ModuleSetupPreTask();
	VOID ModuleSetupPostTask();
	
	BOOL ModuleSetupSetPosn(LONG lSetupTitle = 0);
	BOOL ModuleSetupGoPosn(LONG lSetupTitle = 0);
	BOOL ModuleSetupSetLevel(LONG lSetupTitle = 0);
	BOOL ModuleSetupGoLevel(LONG lSetupTitle = 0);
	BOOL ModuleSetupSetPara(PARA stPara);

	VOID UpdateModuleSetupPosn();
	VOID UpdateModuleSetupLevel();
	VOID UpdateModuleSetupPara();

	INT SetupGoStandbyPosn();
	INT SetupGoPRCalibPosn();
	INT SetupGoPickGlassPosn();
	INT SetupGoPickGlassOffset();
	INT SetupGoPlaceGlassPosn();
	INT SetupGoPlaceGlass1Posn();
	INT SetupGoPlaceGlass2Posn();
	INT SetupGoPlaceGlassOffset(); //20121011
	INT SetupGoPlaceGlass2Offset();

	INT SetupGoStandbyLevel();
	INT SetupGoPickLevel();
	INT SetupGoPlaceLevel();
	INT SetupGoLowerLevel();
	INT SetupGoPRLevel();

	LONG HMI_SetZMoveLoopLimit(IPC_CServiceMessage &svMsg); //20150615
	LONG HMI_LoopTestZMove(IPC_CServiceMessage &svMsg);
	INT TestZRepeatMove();

	INT KeepQuiet();


	virtual VOID PrintMachineSetup(FILE *fp);
	virtual VOID PrintDeviceSetup(FILE *fp);
#ifdef EXCEL_MACHINE_DEVICE_INFO
	virtual BOOL printMachineDeviceInfo();
#endif


protected:
	// ------------------------------------------------------------------------
	// HMI Variable delcaration
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	// HMI Command delcaration
	// ------------------------------------------------------------------------
//p20121104
	LONG HMI_SetModSelected(IPC_CServiceMessage &svMsg);
//p20121104:end
	LONG HMI_HomeAll(IPC_CServiceMessage &svMsg);
	
	//LONG HMI_PowerOnZ(IPC_CServiceMessage &svMsg);
	//LONG HMI_CommZ(IPC_CServiceMessage &svMsg);
	//LONG HMI_HomeZ(IPC_CServiceMessage &svMsg);

	LONG HMI_IndexZPos(IPC_CServiceMessage &svMsg);
	LONG HMI_IndexZNeg(IPC_CServiceMessage &svMsg);

	//LONG HMI_PickGlassOperation(IPC_CServiceMessage &svMsg);
	LONG HMI_PlaceGlassOperation(IPC_CServiceMessage &svMsg);
	
	LONG HMI_SetPlaceGlassPrority(IPC_CServiceMessage &svMsg);

	// Saving / loading parameters
	virtual LONG IPC_SaveMachineParam();
	virtual LONG IPC_LoadMachineParam();
	virtual LONG IPC_SaveDeviceParam();
	virtual LONG IPC_LoadDeviceParam();
	virtual LONG IPC_SaveMachineRecord();
	virtual LONG IPC_LoadMachineRecord();
	virtual LONG IPC_SaveDeviceRecord();
	virtual LONG IPC_LoadDeviceRecord();


};
