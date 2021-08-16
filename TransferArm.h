/////////////////////////////////////////////////////////////////
//	TransferArm.cpp : interface of the CTransferArm class
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

#ifdef TRANSFERARM_EXTERN
extern BOOL					g_bStopCycle;

extern AUTOMODE				AutoMode;
extern BOOL					g_bDiagMode;
extern LONG					g_lDiagnSteps;

extern DOUBLE				g_dGToGOffset; // Glass To Glass Offset

extern ULONG				g_ulUserGroup;

extern SETUP_POSN_XY		g_stSetupPosn[NUM_OF_POSN_XY];
extern SETUP_LEVEL_Z		g_stSetupLevel[NUM_OF_LEVEL_Z];
extern SETUP_PARA			g_stSetupPara[NUM_OF_PARA];

#endif

#include "WorkHolder.h"

#ifdef EXCEL_MACHINE_DEVICE_INFO
#include "ExcelMachineInfo_Util.h"
#endif


class CTransferArm : public CAC9000Stn
{
	DECLARE_DYNCREATE(CTransferArm)
	friend class CINWH;

protected:
	

protected:
	//Update
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();

	virtual VOID RegisterVariables();

public:
	CTransferArm();
	virtual	~CTransferArm();

	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID Operation();

protected:

	// ------------------------------------------------------------------------
	// Variable delcaration
	// ------------------------------------------------------------------------
	CMotorInfo	m_stMotorX;
	CMotorInfo	m_stMotorZ;
	CMotorInfo	m_stMotorY;
	
	CSiInfo		m_stXLmtSnr;
	CSiInfo		m_stZLmtSnr;
	CSiInfo		m_stYLmtSnr;
	CSiInfo		m_stZHomeSnr;
	CSiInfo		m_stT1Snr;	//Upper Snr
	CSiInfo		m_stT2Snr;	//lower Snr
	CSiInfo		m_stGlass1VacSnr;
	CSiInfo		m_stGlass2VacSnr;


	CSiInfo		m_stExtendSnr;
	CSiInfo		m_stRetractSnr;
	CSiInfo		m_stLFPCVacSnr;
	CSiInfo		m_stRFPCVacSnr;

	CSoInfo		m_stLFPCVacSol;
	CSoInfo		m_stRFPCVacSol;
	CSoInfo		m_stRotarySol;
	CSoInfo		m_stZBrakeSol;

	CSiInfo		m_stLRejectVacSnr;
	CSiInfo		m_stRRejectVacSnr;
	CSiInfo		m_stRGlassDetectSnr;
	CSiInfo		m_stLGlassDetectSnr;

	CSoInfo		m_stGlass1VacSol;
	CSoInfo		m_stGlass2VacSol;
	CSoInfo		m_stWeakBlowSol;
	CSoInfo		m_stGlass2WeakBlowSol;

	// ------------------------------------------------------------------------
	// End of Motor SI/SO
	// ------------------------------------------------------------------------
	
	// ------------------------------------------------------------------------
	// End of CHECK Who Am I
	// ------------------------------------------------------------------------
	
	// Cycle Status
	LONG	m_lPickPlaceRetryCounter;
	LONG	m_lPickPlaceRetryLimit;
	DWORD	m_dwMachineTime;
	DWORD	m_dwStartTime;
	INT		m_nCycleCounter;
	DOUBLE	m_dCycleTime;
	DOUBLE	m_dTempCycleTime[10];
	GlassNum PPAction;

		// Measurement Routine
	MTR_POSN	m_mtrMeasureSetZeroPt;
	DOUBLE		m_dMeasuredDistance;
	DOUBLE		m_dMeasuredAngle;
	BOOL		m_bEnableMeasure;

public:
	BOOL	m_bIsVacError; //20121224 vac error
	BOOL	m_bUseFPCVacuum;	// 20140619 Yip
protected:

	// positions
	LONG	m_lStandbyPosn;
	LONG	m_lLeftSafetyPosn;
	LONG	m_lRightSafetyPosn;
	LONG	m_lPickGlassPosn;
	LONG	m_lPrePlaceGlassPosn;
	LONG	m_lPlaceGlassPosn;
	LONG	m_lRejectBinPosn;
	LONG	m_lPickGlassOffset;		
	LONG	m_lPlaceGlassOffset;	
	LONG	m_lPickGlass1Offset;
	LONG	m_lPickGlass2Offset;
	LONG	m_lPlaceGlass1Offset;
	LONG	m_lPlaceGlass2Offset;

	//Levels
	LONG	m_lStandbyLevel;
	LONG	m_lPickLevel;
	LONG	m_lPlaceLevel;
	LONG	m_lLowerLevel;
	LONG	m_lPRLevel;
	LONG	m_lPRCalibLevel;

	// Y Positions
	LONG	m_lYStandbyPosn;




	// ------------------------------------------------------------------------
	// Function delcaration
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	// Virtual WorkHolder Function delcaration
	// ------------------------------------------------------------------------
	// All Virtual Function from WorkHolder.cpp

	// ------------------------------------------------------------------------
	// END Virtual WorkHolder Function delcaration
	// ------------------------------------------------------------------------
	// I/Os
	INT SetHandShakeBit(CSoInfo *stTempSo, BOOL bMode);
	BOOL IsHandShakeBitOn(CSiInfo stTempSi);

	BOOL IsVacSensorOn(GlassNum lGlassNum);
	INT SetVacSol(GlassNum lGlassNum, BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT	SetWeakBlowSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetG2WeakBlowSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);

	BOOL IsLFPC_VacSensorOn();	// 20140619 Yip
	BOOL IsRFPC_VacSensorOn();	// 20140619 Yip
	INT SetLFPCVacSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetRFPCVacSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);

	BOOL IsT1SnrOn();
	BOOL IsT2SnrOn();
	INT SetRotarySol(BOOL bMode, BOOL bWait = GMP_NOWAIT);

	INT MoveTPickPosn(BOOL bWait);
	INT MoveTPlacePosn(BOOL bWait);
	INT SyncZ();	
	
	BOOL IsGLASS1_VacSensorOn();
	BOOL IsGLASS2_VacSensorOn();
	INT SetGLASS1_VacSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetGLASS2_VacSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);

	INT SetZBrakeSol(BOOL bMode, BOOL bWait);

	// 20140619 Yip
	BOOL IsVacStateOn(GLASS_FPC_VAC_STATE stVacState);
	BOOL IsVacStateOff(GLASS_FPC_VAC_STATE stVacState);
	BOOL IsVacStateMatched(GLASS_FPC_VAC_STATE stVacState);
	GLASS_FPC_VAC_STATE GetGlass1_VacSensorOnState();
	GLASS_FPC_VAC_STATE GetGlass2_VacSensorOnState();
	GLASS_FPC_VAC_STATE GetGlassFPC1_VacSensorOnState();
	GLASS_FPC_VAC_STATE GetGlassFPC2_VacSensorOnState();
	INT SetGlassFPC1_VacSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);
	INT SetGlassFPC2_VacSol(BOOL bMode, BOOL bWait = GMP_NOWAIT);

	VOID SetDiagnSteps(ULONG ulSteps);

	GlassNum CalcPickStatus(BOOL bWHStatus[], BOOL bArmStatus[]);
	GlassNum CalcPlaceStatus(BOOL bWHStatus[], BOOL bArmStatus[]);
	INT CheckModulePower(); //20121219 arm crash

	// Add me for setup wizard later
	// ------------------------------------------------------------------------
	// HMI Variable delcaration
	// ------------------------------------------------------------------------
	// Values are for HMI display only! Users should not use these in coding.

	// ------------------------------------------------------------------------
	// HMI Command delcaration
	// ------------------------------------------------------------------------
	// Motor
	LONG HMI_PowerOnX(IPC_CServiceMessage &svMsg);

	LONG HMI_CommX(IPC_CServiceMessage &svMsg);
	
	LONG HMI_HomeX(IPC_CServiceMessage &svMsg);

	LONG HMI_PowerOnZ(IPC_CServiceMessage &svMsg);

	LONG HMI_CommZ(IPC_CServiceMessage &svMsg);
	
	LONG HMI_HomeZ(IPC_CServiceMessage &svMsg);

	LONG HMI_PowerOnY(IPC_CServiceMessage &svMsg);

	LONG HMI_CommY(IPC_CServiceMessage &svMsg);
	
	LONG HMI_HomeY(IPC_CServiceMessage &svMsg);

	LONG HMI_ToggleG1VacSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleG2VacSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleWeakBlowSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleG2WeakBlowSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleLFPCVacSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleRFPCVacSol(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleTSol(IPC_CServiceMessage &svMsg);

	// Setup Page Related
	LONG HMI_SetDiagnSteps(IPC_CServiceMessage &svMsg);
	LONG HMI_IndexXPos(IPC_CServiceMessage &svMsg);
	LONG HMI_IndexXNeg(IPC_CServiceMessage &svMsg);
	LONG HMI_IndexZPos(IPC_CServiceMessage &svMsg);
	LONG HMI_IndexZNeg(IPC_CServiceMessage &svMsg);
	LONG HMI_IndexYPos(IPC_CServiceMessage &svMsg);
	LONG HMI_IndexYNeg(IPC_CServiceMessage &svMsg);

	LONG HMI_SetPickPlaceRetryLimit(IPC_CServiceMessage &svMsg);
	
	virtual LONG IPC_SaveMachineParam();
	virtual LONG IPC_LoadMachineParam();
	virtual LONG IPC_SaveDeviceParam();
	virtual LONG IPC_LoadDeviceParam();
#ifdef EXCEL_MACHINE_DEVICE_INFO
	virtual BOOL printMachineDeviceInfo();
#endif


};
