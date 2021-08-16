/////////////////////////////////////////////////////////////////
//	TimeChart.cpp : interface of the CTimeChart class
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

#ifdef TIMECHART_EXTERN
extern BOOL					g_bStopCycle;

#endif


typedef struct
{
	BOOL	bSelected;		// selected for logging
	CString	stObjName;
	INT		nObjID;

	INT		nWeight;
	BOOL	bStatus;
	INT		nJobCount;

} TimeObject;

class CTimeChart : public CAC9000Stn
{
	DECLARE_DYNCREATE(CTimeChart)

	
	friend class CPreBond; //20120831
	friend class CINWH;
	friend class CTA2;
	friend class CMainBondWH;

protected:
	//Update
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();

	virtual VOID RegisterVariables();
	virtual VOID PreStartOperation();
	virtual VOID AutoOperation();

private:
	virtual CString		GetStation() 
	{
		return "TC_";
	};

public:
	CTimeChart();
	virtual	~CTimeChart();

	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID Operation();

protected:
	// ------------------------------------------------------------------------
	// Variable delcaration
	// ------------------------------------------------------------------------
	INT		m_nStateDiff;			// difference in value between on/off state
	INT		m_nObjDiff;				// difference in value between timechart objects
	CString	m_szFileName;

	FILE	*m_logfp;
	DWORD	m_dwStartTime;			// start time of each cycle

	TimeObject	stTimeObj[NUM_OF_TC_OBJECT];

	BOOL	m_bLogging;

	// ------------------------------------------------------------------------
	// Function delcaration
	// ------------------------------------------------------------------------
	VOID	TC_RegisterObj();

	VOID	TC_AddCmd(INT emTCObjectID);
	VOID	TC_DelCmd(INT emTCObjectID);

	VOID	TC_ResetTimer();

protected:
	// ------------------------------------------------------------------------
	// HMI Variable delcaration
	// ------------------------------------------------------------------------
	BOOL	HMI_bSolSelected;

	// ------------------------------------------------------------------------
	// HMI Command delcaration
	// ------------------------------------------------------------------------

	LONG HMI_SetModSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetInConvSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetINSHSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetINWHSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetACF1WHSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetACF2WHSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPBWHSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetMB1WHSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetMB2WHSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetTA1Selected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetTA2Selected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetTA3Selected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetTA4Selected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetTA5Selected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetACF1Selected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetACF2Selected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPBUplookSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPB1Selected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPB2Selected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetMB1Selected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetMB2Selected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetTrayHolderSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetSHSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_SetOutConvSelected(IPC_CServiceMessage &svMsg);

	LONG HMI_SetSOLSelected(IPC_CServiceMessage &svMsg);

	LONG HMI_SelectAll(IPC_CServiceMessage &svMsg);

	LONG IPC_AddCmd(IPC_CServiceMessage &svMsg);
	LONG IPC_DelCmd(IPC_CServiceMessage &svMsg);
	LONG IPC_ResetTimer(IPC_CServiceMessage &svMsg);
};
