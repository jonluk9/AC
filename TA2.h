/////////////////////////////////////////////////////////////////
//	TA2.cpp : interface of the CTA2 class
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

#define TRANSFERARM_EXTERN
#include "TransferArm.h"

#ifdef EXCEL_MACHINE_DEVICE_INFO
#include "ExcelMachineInfo_Util.h"
#endif

#include "PRTaskStn.h"

#ifdef TA2_EXTERN
extern BOOL					g_bStopCycle;

extern DOUBLE				g_dModuleOffset;

extern STATUS_TYPE			TA2_Status;
extern STATUS_TYPE			TA1_Status;
extern STATUS_TYPE			TA_MANAGER_Status;
extern ACFWHUnitNum			TA2_POSN;

extern STATUS_TYPE			ACF_WH_Status[MAX_NUM_OF_ACFWH];

extern STATUS_TYPE			TA2_GLASS_Status;

extern STATUS_TYPE			ACF_GLASS_Status[MAX_NUM_OF_ACFWH][MAX_NUM_OF_GLASS]; //20140529

extern LONG					g_lGlassInputMode; //20120412

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

#endif

typedef enum
{

	TA2_X_CTRL_NOTHING = 0, 
	//TA2_X_CTRL_GO_HOME, 
	TA2_X_CTRL_GO_STANDBY_POSN, 
	TA2_X_CTRL_GO_PLACE_GLASS_POSN,
	TA2_X_CTRL_GO_PLACE_GLASS_OFFSET,
	TA2_X_CTRL_GO_PICK_ACF1WH_POSN,
	TA2_X_CTRL_GO_PICK_ACF1WH_OFFSET, 
	TA2_X_CTRL_GO_PICK_ACF2WH_POSN, 
	TA2_X_CTRL_GO_PICK_ACF2WH_OFFSET,

} TA2_X_CTRL_SIGNAL;

typedef enum
{
	TA2_Z_CTRL_NOTHING = 0,
	TA2_Z_CTRL_GO_HOME,
	TA2_Z_CTRL_GO_STANDBY_LEVEL,
	TA2_Z_CTRL_GO_PICK_LEVEL,
	TA2_Z_CTRL_GO_LOWER_LEVEL,

} TA2_Z_CTRL_SIGNAL;


// States
//
typedef enum
{
	TA2_IDLE_Q = 0, 

	TA2_WAIT_TA_MANAGER_REQUEST_Q,
	TA2_PICK_COF_Q,
	TA2_CHECK_PICK_ERROR_Q,
	TA2_CHECK_COF_ERROR_Q,
	TA2_REJECT_COF_Q,			//5

	TA2_WAIT_PICKARM_ACK_Q,
	TA2_COF_VAC_OFF_REQUEST_Q,
	TA2_WAIT_PICK_COF_COMPLETE_Q,


	TA2_STOP_Q = 99,

} TA2_STATE;


class CTA2 : public CTransferArm
{
	DECLARE_DYNCREATE(CTA2)
	friend class CAC9000App;
	friend class CSettingFile;
	friend class CACFWH;
	friend class CInspection;
	friend class CInPickArm;
	friend class CTA1;
	friend class CWinEagle;

protected:
	

protected:
	virtual VOID RegisterVariables();
	virtual VOID PreStartOperation();
	virtual VOID AutoOperation();
	
	virtual BOOL GetGlass1Status() 
	{
		return m_bGlass1Exist;
	}
	//virtual BOOL GetGlass2Status() 
	//{
	//	return m_bGlass2Exist;
	//}

private:
	virtual CString		GetStation() 
	{
		return "TA2_";
	}

public:
	CTA2();
	virtual	~CTA2();

	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID Operation();

protected:

	// ------------------------------------------------------------------------
	// Variable delcaration
	// ------------------------------------------------------------------------
	ACFWHUnitNum m_lCurrCheck;


	//////////////////////////////////////////////
	// Handshaking bits to Fx9000	Description in (USL <-> COF902); USL is TA2, FX9000 is COF
									//Fx9000
	CSiInfo		m_stFOUT_IN1;		//(COF_MC_READY)
	CSiInfo		m_stFOUT_IN2;		//(COF_PICK_ACTION)
	CSiInfo		m_stFOUT_IN3;		//(COF_VAC_OFF_REQUEST)
	CSiInfo		m_stFOUT_IN4;		//(COF_PICK_ERROR)
	CSiInfo		m_stFOUT_IN5;		//Reserve
	CSiInfo		m_stFOUT_IN6;		//Reserve

									//TA2
	CSoInfo		m_stFOUT_OUT1;		//(USL_MC_READY)
	CSoInfo		m_stFOUT_OUT2;		//(USL_OK_SEND_READY)
	CSoInfo		m_stFOUT_OUT3;		//(USL_NG_SEND_READY)
	CSoInfo		m_stFOUT_OUT4;		//(USL_VAC_OFF_ACK)
	CSoInfo		m_stFOUT_OUT5;		//Reserve
	CSoInfo		m_stFOUT_OUT6;		//Reserve

	///////////////////////////////////////


//	LONG	m_lPolarizerThickness[MAX_NUM_OF_GLASS]; //20120927 pin detect polarizer


	// ------------------------------------------------------------------------
	// End of Motor SI/SO
	// ------------------------------------------------------------------------
	
	// cycle status

	LONG		m_lZMoveLoopLimit; 
	LONG		m_lZMoveLoopCount;
	BOOL		m_bZMoveLoopEnable;

	BOOL		HMI_bStandAlone;
	BOOL		HMI_bManualPickCOF;
	// ------------------------------------------------------------------------
	// Function delcaration
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	// Virtual Transfer Function delcaration
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	// END Virtual WorkHolder Function delcaration
	// ------------------------------------------------------------------------
	VOID UpdateOutput();
	CString GetSequenceString();
	VOID OperationSequence();

	GlassNum AssignPickFromWhere();
	BOOL CheckPickStatus();
	BOOL SetPickStatus();
	BOOL IsACFWHReadyLoad();
	BOOL SetPlaceStatus();
	BOOL CheckPlaceStatus();
	BOOL CheckReplaceStatus();
	BOOL ResetPlaceStatus();
	BOOL IsInStandbyPosn(); //20121227
	BOOL IsErrorCOF();
	INT	RejectCOF();


	BOOL HS_IsFxMachineReady();
	BOOL HS_IsFxPickAction();
	BOOL HS_IsFxVacOffRequest();
	BOOL HS_IsFxPickError();

	VOID HS_TA2SetMachineReady(BOOL bState);
	VOID HS_TA2SetOKSendReady(BOOL bState);
	VOID HS_TA2SetNGSendReady(BOOL bState);
	VOID HS_TA2SetVacOffACK(BOOL bState);

	INT PickGlass(ACFWHUnitNum lACFWHnum);
//	INT PlaceGlass(GlassNum lGlassNum);
	
	BOOL PickGlassOperation(ACFWHUnitNum lACFWHnum);
	BOOL PlaceGlassOperation();

	// Special Used For TA2 Place Glass To ACF1WH and ACF2WH
	BOOL IsPlaceToWHVacSensorOn(LONG lGlassNum);
	INT SetPlaceToWHVacSol(LONG lGlassNum, BOOL bMode, BOOL bWait = GMP_NOWAIT);
	LONG CalcPlaceToWHDelay(BOOL bMode, LONG lGlassNum);

	//Motor Related
	INT MoveX(TA2_X_CTRL_SIGNAL ctrl, BOOL bWait);
	INT MoveZ(TA2_Z_CTRL_SIGNAL ctrl, BOOL bWait);
	INT MovePlaceToWHToStandbyLevel(LONG lGlass, BOOL bWait);
	INT MovePlaceToWHToLoadLevel(LONG lGlass, BOOL bWait);
	INT MovePlaceToWHToLoadPosn(LONG lGlass, BOOL bWait);

	INT MoveToPickPosn(LONG lGlass, BOOL bWait);

	LONG HMI_SetStandAlone(IPC_CServiceMessage &svMsg);
	LONG HMI_SetManualPickCOF(IPC_CServiceMessage &svMsg);

	
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
	INT SetupGoPickGlassPosn();
	INT SetupGoPickGlassOffset();
	INT SetupGoPickGlass2Posn();
	INT SetupGoPickGlass2Offset();
	INT SetupGoPlaceGlassPosn();
	INT SetupGoPlaceGlassOffset();

	INT SetupGoStandbyLevel();
	INT SetupGoPickLevel();
	INT SetupGoPlaceLevel();
	INT SetupGoLowerLevel();

	LONG HMI_SetZMoveLoopLimit(IPC_CServiceMessage &svMsg); //20150615
	LONG HMI_LoopTestZMove(IPC_CServiceMessage &svMsg);
	INT TestZRepeatMove();

	INT KeepQuiet();

	virtual VOID PrintMachineSetup(FILE *fp);
	virtual VOID PrintDeviceSetup(FILE *fp);
#ifdef EXCEL_MACHINE_DEVICE_INFO
	virtual BOOL printMachineDeviceInfo();
#endif

	// PR Relative
	virtual CMotorInfo &GetMotorX(); 
	virtual CMotorInfo &GetMotorY();
	virtual INT SyncX();
	virtual INT SyncY();

	virtual INT MoveAbsoluteXY(LONG lCntX, LONG lCntY, BOOL bWait);
	virtual INT MoveRelativeXY(LONG lCntX, LONG lCntY, BOOL bWait);

	INT MoveTPickPosn(BOOL bWait);
	INT MoveTPlacePosn(BOOL bWait);

	INT IndexGlassToTA2(GlassNum lGlassNum);
	BOOL IndexPickGlassOperation(GlassNum lGlassNum);

	LONG HMI_SelectGlassNum(IPC_CServiceMessage &svMsg);
	LONG HMI_UpdateSetupStatus(IPC_CServiceMessage &svMsg);

	LONG HMI_MeasureSetZero(IPC_CServiceMessage &svMsg); //20150115
	LONG HMI_MeasureEnable(IPC_CServiceMessage &svMsg);

protected:
	// ------------------------------------------------------------------------
	// HMI Variable delcaration
	// ------------------------------------------------------------------------
	
	// ------------------------------------------------------------------------
	// HMI Command delcaration
	// ------------------------------------------------------------------------
	LONG HMI_HomeAll(IPC_CServiceMessage &svMsg);
	LONG HMI_PickGlassOperation(IPC_CServiceMessage &svMsg);
	//LONG HMI_PlaceGlassOperation(IPC_CServiceMessage &svMsg);

	LONG HMI_ToggleUseFPCVacuum(IPC_CServiceMessage &svMsg); //20141112

	LONG HMI_ToggleFOutOut1(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleFOutOut2(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleFOutOut3(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleFOutOut4(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleFOutOut5(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleFOutOut6(IPC_CServiceMessage &svMsg);
	
	// Saving / loading parameters
	virtual LONG IPC_SaveMachineParam();
	virtual LONG IPC_LoadMachineParam();
	virtual LONG IPC_SaveDeviceParam();
	virtual LONG IPC_LoadDeviceParam();
};
