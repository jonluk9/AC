/////////////////////////////////////////////////////////////////
//	TAManager.cpp : interface of the CTAManager class
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


#ifdef TAManager_EXTERN
extern BOOL					g_bStopCycle;

extern STATUS_TYPE			TA_MANAGER_Status;
extern STATUS_TYPE			TA1_Status;
extern STATUS_TYPE			TA2_Status;
//extern STATUS_TYPE			MAINBOND_WH_Status[MAX_NUM_WITH_MAINBOND_3][MAX_NUM_OF_GLASS];
//extern STATUS_TYPE			ACF_GLASS_Status[MAX_NUM_OF_ACFWH][MAX_NUM_OF_GLASS];
extern STATUS_TYPE			ACF_WH_Status[MAX_NUM_OF_ACFWH];

extern ACFWHUnitNum			TA1_POSN;		//TA1 Cur POSN
extern ACFWHUnitNum			TA2_POSN;		//TA2 Cur POSN

#endif

// States
//
typedef enum
{
	TA_MANAGER_IDLE_Q = 0, 

	TA_MANAGER_CHECK_ACFWH_REQUEST_LOAD_Q,
	TA_MANAGER_CHECK_TA1_SAFE_TO_MOVE_Q,
	TA_MANAGER_WAIT_TA1_ACK_Q,
	TA_MANAGER_CHECK_ACFWH_REQUEST_UNLOAD_Q,
	TA_MANAGER_CHECK_TA2_SAFE_TO_MOVE_Q,		//5

	TA_MANAGER_WAIT_TA2_ACK_Q,

	TA_MANAGER_STOP_Q = 99,

} TA_MANAGER_STATE;


class CTAManager : public CAC9000Stn
{
	DECLARE_DYNCREATE(CTAManager)

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
		return "TAManager_";
	};

public:
	CTAManager();
	virtual	~CTAManager();

	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID Operation();

protected:
	// ------------------------------------------------------------------------
	// Variable delcaration
	// ------------------------------------------------------------------------
	ACFWHUnitNum m_lCurrCheck;
	ACFWHUnitNum m_lCurrNotCheck;
	// ------------------------------------------------------------------------
	// Function delcaration
	// ------------------------------------------------------------------------
	CString GetSequenceString();
	VOID OperationSequence();


	// ------------------------------------------------------------------------
	// HMI Variable delcaration
	// ------------------------------------------------------------------------
	// Values are for HMI display only! Users should not use these in coding.	

	// ------------------------------------------------------------------------
	// HMI Command delcaration
	// ------------------------------------------------------------------------

};
