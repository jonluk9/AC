/////////////////////////////////////////////////////////////////
//	AdamCtrl.cpp : interface of the CAdamCtrl class
//
//	Description:
//		AC9000 Host Software
//
//	Date:		Tue Aug 1 2006
//	Revision:	0.25
//
//	By:			AC9000
//				AAA CSP Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2006.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once


#include "AC9000Stn.h"


#define NUM_OF_ADAM			1
//#define NUM_OF_ADAM_CH		6 // 6 channels for each Adam, total = 2 adam * 6 channels = 12 channels
#define NUM_OF_ADAM_CH		48 // 48 channels, some are TC exist, assign TC channel to each ADAM....


#ifdef ADAMCTRL_EXTERN
extern BOOL				g_bStopCycle;
#endif


class CAdamCtrl : public CAC9000Stn
{
	DECLARE_DYNCREATE(CAdamCtrl)
	friend class CAC9000App;
	friend class CTempDisplay;
	friend class CLitecCtrl;

protected:
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();

	virtual VOID RegisterVariables();

public:
	CAdamCtrl();
	virtual	~CAdamCtrl();

	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID Operation();


protected:
	// ------------------------------------------------------------------------
	// Variable delcaration
	// ------------------------------------------------------------------------
	// Monitor info
	typedef struct
	{
		MonUnitNum		Mon;			// ADAM1, ADAM2....
		MonChNum		Ch;					// CH0, CH1, CH2.......

	} MON_INFO;


	LONG		m_lAdam1Handle;
	LONG		m_lAdam2Handle;
	BOOL		m_bAdam1Init;
	BOOL		m_bAdam2Init;

	//BOOL		m_bAdam1Failed[NUM_OF_ADAM_CH];
	//BOOL		m_bAdam2Failed[NUM_OF_ADAM_CH];
	BOOL		m_bAdamFailed[NUM_OF_ADAM_CH];

	//DOUBLE		m_dAdam1Temp[NUM_OF_ADAM_CH];
	//DOUBLE		m_dAdam2Temp[NUM_OF_ADAM_CH];
	DOUBLE		m_dAdamTemp[NUM_OF_ADAM_CH];

	MON_INFO	m_stAdamInfo[NUM_OF_ADAM_CH]; // holding Adam info
	INT			m_nCurIndex;

	// ------------------------------------------------------------------------
	// Function delcaration
	// ------------------------------------------------------------------------

	BOOL InitADAM();
	BOOL ADAMChecker();

	VOID ResetChannelStatus();
	VOID GetTempValue(CTempDisplay *pStation);


	BOOL IsAdam1MonValid(INT nChNum);
	BOOL IsAdam2MonValid(INT nChNum);
	BOOL IsLitecMonValid(INT nChNum);

protected:
	// ------------------------------------------------------------------------
	// HMI Variable delcaration
	// ------------------------------------------------------------------------

	// Values are for HMI display only! Users should not use these in coding.


	// ------------------------------------------------------------------------
	// IPC Command delcaration
	// ------------------------------------------------------------------------
};
