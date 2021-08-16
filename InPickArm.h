/////////////////////////////////////////////////////////////////
//	InPickArm.cpp : interface of the CInPickArm class
//
//	Description:
//		Motion Application
//
//	Date:		Friday, November 20, 2015
//	Revision:	1.00
//
//	By:			Author Unknown
//				Department
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2015.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once
#define TRANSFERARM_EXTERN
#include "TransferArm.h"

#include "AC9000Stn.h"

extern STATUS_TYPE TA1_Status;
extern STATUS_TYPE IN_PICKARM_Status;
extern STATUS_TYPE IN_PICKARM_GLASS_Status;


typedef enum
{
	IN_PICKARM_CTRL_NOTHING = 0, 
	IN_PICKARM_X_CTRL_GO_STANDBY_POSN, 
	IN_PICKARM_X_CTRL_GO_PICK_GLASS_POSN,
	IN_PICKARM_X_CTRL_GO_PICK_GLASS_OFFSET,
	IN_PICKARM_X_CTRL_GO_PREPLACE_POSN,
	IN_PICKARM_X_CTRL_GO_PLACE_GLASS_POSN,
	IN_PICKARM_X_CTRL_GO_PLACE_GLASS_OFFSET,
	IN_PICKARM_X_CTRL_GO_REJECT_BIN_POSN,

} IN_PICKARM_X_CTRL_SIGNAL;

typedef enum
{
	IN_PICKARM_Z_CTRL_NOTHING = 0,
	IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL,
	//IN_PICKARM_Z_CTRL_GO_SAFETY_LEVEL,
	IN_PICKARM_Z_CTRL_GO_PICK_LEVEL,
	IN_PICKARM_Z_CTRL_GO_PLACE_LEVEL,		

} IN_PICKARM_Z_CTRL_SIGNAL;

typedef enum
{
	IN_PICKARM_IDLE_Q = 0,

	IN_PICKARM_CHECK_EXIST_Q,
	IN_PICKARM_WAIT_FOR_PICK_Q,
	IN_PICKARM_WAIT_FOR_PLACE_Q,
	IN_PICKARM_WAIT_TA1_ACK_Q,
	IN_PICKARM_PICK_COF_Q,			//5

	IN_PICKARM_WAIT_USL_VAC_OFF_Q,
	IN_PICKARM_CHECK_PICK_COF_EXIST_Q,
	IN_PICKARM_CHECK_USL_RESET_SEND_READY_Q,
	IN_PICKARM_CHECK_COF_STATUS_Q,

	IN_PICKARM_STOP_Q = 99,

} IN_PICKARM_STATE;

//class CInPickArm : public CAC9000Stn
class CInPickArm : public CTransferArm
{
	DECLARE_DYNCREATE(CInPickArm)
	friend class CAC9000App;
	friend class CSettingFile;
	friend class CTA1;
	friend class CTA2;
	friend class CACFWH;

private:
	virtual CString GetStation() 
	{
		return "InPickArm_";
	}

protected:
	//CSiInfo		m_stPressureSnr;
	
	CSiInfo		m_stUSLBackSideDoorSnr;
	CSiInfo		m_stFInIn1;
	CSiInfo		m_stFInIn2;
	CSiInfo		m_stFInIn3;
	CSiInfo		m_stFInIn4;
	CSiInfo		m_stFInIn5;
	CSiInfo		m_stFInIn6;


	CSoInfo		m_stFInOut1;
	CSoInfo		m_stFInOut2;
	CSoInfo		m_stFInOut3;
	CSoInfo		m_stFInOut4;
	CSoInfo		m_stFInOut5;
	CSoInfo		m_stFInOut6;



	BOOL	HMI_bStandAlone;

protected:
	//Update
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();

	virtual VOID RegisterVariables();
	virtual VOID PreStartOperation();
	virtual VOID AutoOperation();

	CString GetSequenceString();
	VOID OperationSequence();

	BOOL CheckPickStatus();
	BOOL SetPickStatus();
	BOOL SetPlaceStatus();
	
	INT MoveX(IN_PICKARM_X_CTRL_SIGNAL ctrl, BOOL bWait = GMP_WAIT);
	INT MoveZ(IN_PICKARM_Z_CTRL_SIGNAL ctrl, BOOL bWait = GMP_WAIT);


	BOOL IsUSLBackSideDoorSnrOn(); 
	BOOL IsUSLMcReadyOn(); //20160511
	BOOL IsUSLOkSendReadyOn();
	BOOL IsUSLNgSendReadyOn();
	BOOL IsUSLVacOffAckOn();

	INT SetCOFMcReady(BOOL bMode, BOOL bWait = GMP_NOWAIT); //20160511
	INT SetCOFPickAction(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetCOFVacOffReq(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetCOFPickError(BOOL bMode, BOOL bWait = GMP_NOWAIT);

	INT SetFInOut1(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetFInOut2(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetFInOut3(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetFInOut4(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetFInOut5(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetFInOut6(BOOL bMode, BOOL bWait = GMP_NOWAIT);

	BOOL PickGlassOperation(GlassNum lGlassNum);
	BOOL PlaceGlassOperation(GlassNum lGlassNum);

	//INT PickGlass_Down(GlassNum lGlassNum);
	//INT PickGlass_Up(GlassNum lGlassNum);
	INT PickGlass(GlassNum lGlassNum);
	INT PlaceGlass(GlassNum lGlassNum);

	LONG HMI_HomeAll(IPC_CServiceMessage &svMsg);
	
	LONG HMI_PickGlassOperation(IPC_CServiceMessage &svMsg);
	LONG HMI_PlaceGlassOperation(IPC_CServiceMessage &svMsg);

	LONG HMI_ToggleFInOut1(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleFInOut2(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleFInOut3(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleFInOut4(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleFInOut5(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleFInOut6(IPC_CServiceMessage &svMsg);

	LONG HMI_SetStandAlone(IPC_CServiceMessage &svMsg);

	INT GetPickAction();
	INT RejectCOF();

	// Module Setup Related
	VOID ModuleSetupPreTask();
	VOID ModuleSetupPostTask();
	
	BOOL ModuleSetupSetPosn(LONG lSetupTitle = 0);
	BOOL ModuleSetupSetLevel(LONG lSetupTitle = 0);
	BOOL ModuleSetupSetPara(PARA stPara);
	BOOL ModuleSetupGoPosn(LONG lSetupTitle = 0);
	BOOL ModuleSetupGoLevel(LONG lSetupTitle);

	VOID UpdateModuleSetupPosn();
	VOID UpdateModuleSetupLevel();
	VOID UpdateModuleSetupPara();

	INT SetupGoStandbyPosn();
	INT SetupGoPickGlassPosn();
	INT SetupGoPlaceGlassPosn();
	INT SetupGoPickGlassOffset();
	INT SetupGoPlaceGlassOffset();
	INT SetupGoStandbyLevel();
	INT SetupGoPickGlassLevel();
	INT SetupGoPlaceGlassLevel();
	INT SetupGoPrePlaceGlassPosn();
	INT SetupGoRejectBinPosn();

	// Saving / loading parameters
	virtual LONG IPC_SaveMachineParam();
	virtual LONG IPC_LoadMachineParam();
	virtual LONG IPC_SaveDeviceParam();
	virtual LONG IPC_LoadDeviceParam();

public:
	CInPickArm();
	virtual	~CInPickArm();

	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID Operation();
};
