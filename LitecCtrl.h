/////////////////////////////////////////////////////////////////
//	LitecCtrl.cpp : interface of the CLitecCtrl class
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
// Trash
#pragma once

#include "AC9000Stn.h"
#include "ebtc.h"
#include "litecdefs.h"
#include "litec.h"
#include "litecwin32.h"



#ifdef LITECCTRL_EXTERN
extern BOOL				g_bStopCycle;

// Heater Control
extern CCriticalSection		m_csHeaterCtrlLock; //20150714
#endif

//#define MAX_LTC_CH	(12)		moved to AC9000stn.h
#define MON_INDEX	(MAX_LTC_CH)
//#define NUM_OF_LITEC	(4)			moved to AC9000stn.h
#define AUTOTUNE_BUFFER_SIZE (3600)

#define MaskHi	0
#define MaskLo	4095 //12 channels with 12 bits
#define LITEC_ALARM_BAND_TEMPERATURE		450			//400			//20150910
// Litec MCB
typedef enum
{
	HEATER = 0, 
	//MONITOR, 
} LITEC_MCB_TYPE;

//typedef enum //20130911			moved to AC9000stn.h
//{
//	MB1_LEFT_HEAD_CH = 0,
//	MB1_RIGHT_HEAD_CH,
//	MB1_LEFT_BASE_CH,
//	MB1_RIGHT_BASE_CH,
//	ACF1_CH,
//	PB1_CH,
//	MB2_LEFT_HEAD_CH,
//	MB2_RIGHT_HEAD_CH,
//	MB2_LEFT_BASE_CH,
//	MB2_RIGHT_BASE_CH,
//	ACF2_CH,
//	PB2_CH
//} LITEC_CHANNEL_NUM;

//#define COM1	1
//#define COM3	3
//#define COM4	4
//#define COM5	5

typedef enum //20140122
{
	COM1 = 1,
	COM2 = 2,
	COM3 = 3,
	COM4 = 4,
	COM5 = 5,

} LITEC_COMPORT_NUM;


typedef enum //20140122
{
	CONSTANT_HEAT = 0,
	PULSE_HEAT,

} LITEC_CONTROL_TYPE;


class CLitecCtrl : public CAC9000Stn
{
	DECLARE_DYNCREATE(CLitecCtrl)
	friend class CAC9000App;
	friend class CTempDisplay;
	friend class CHouseKeeping;
	friend class CAdamCtrl;

protected:
	//Update
	VOID UpdateOutput();

	virtual VOID RegisterVariables();

public:
	CLitecCtrl();
	virtual	~CLitecCtrl();

	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID Operation();

protected:
	// ------------------------------------------------------------------------
	// Variable delcaration
	// ------------------------------------------------------------------------

	HANDLE	m_hLitecPort[NUM_OF_LITEC];
	HANDLE	m_hLitecMCB[NUM_OF_LITEC];
	HANDLE	m_hLitecChn[(NUM_OF_LITEC *MAX_LTC_CH) + 11];

	// status
	CString m_stLitecLibVersion;
	CString m_stLitecVersion[NUM_OF_LITEC];
	BOOL	m_bLitecInit[NUM_OF_LITEC];
	BOOL	m_bLitecOn[NUM_OF_LITEC *MAX_LTC_CH];
	DOUBLE	m_dLitecTemp[NUM_OF_LITEC *MAX_LTC_CH];
	DOUBLE	m_dLitecSetTemp[NUM_OF_LITEC *MAX_LTC_CH];
	BOOL	m_bLitecATOn[NUM_OF_LITEC *MAX_LTC_CH];
	BOOL	m_bLitecFailed[NUM_OF_LITEC *MAX_LTC_CH];
	INT		m_nCurIndex;

	//AutoTune Timer
	DWORD	m_dwATTimer[NUM_OF_LITEC *MAX_LTC_CH];
	FLOAT	m_fATBuffer[NUM_OF_LITEC *MAX_LTC_CH][AUTOTUNE_BUFFER_SIZE];
	ULONG	m_ulATCounter[NUM_OF_LITEC *MAX_LTC_CH];
	BOOL	m_bIsLearningAT[NUM_OF_LITEC *MAX_LTC_CH];

	//Learn HOD 
	BOOL	m_bLearningHOD[NUM_OF_LITEC *MAX_LTC_CH];

	//Learn RMS
	BOOL	m_bLearningRMS[NUM_OF_LITEC *MAX_LTC_CH];

	// Litec info
	typedef struct
	{
		LITEC_COMPORT_NUM	nComPortNum;			// ComPort Num {COM1, COM2, COM3 ....}
		LITEC_CONTROL_TYPE	nLitecType;				// PULSE_HEAT, CONSTANT_HEAT

	} LITEC_INFO;

	// tuning setting
	typedef struct
	{
		BYTE	ucMethod;			// tuning method: 0 = relay, 1 = step
		DOUBLE	dSetPoint;			// set point
		DWORD	dwClkPrescaler;		// control frequency during tuning, do not need to set
		DOUBLE	dErrBound;			// error bound (for relay only)
		BYTE	ucInChn;			// input control channel (for relay only)
		FLOAT	fStepTarget;		// step target (for step only)
	} TUNING_SETTING;

	TUNING_SETTING m_stTuningSetting;

	// PID parameters
	typedef struct
	{
		DOUBLE	dKp_d;			// dynamic Kp
		DOUBLE	dKi_d;			// dynamic Ki
		DOUBLE	dKd_d;			// dynamic Kd
		DOUBLE	dKp_s;			// static Kp
		DOUBLE	dKi_s;			// static Ki
		DOUBLE	dKd_s;			// static Kd
		DOUBLE	dA;				// amplitude (degC)
		DOUBLE	dRT;			// rising time (s)
		DOUBLE	dFT;			// falling time (s)
		DOUBLE	dDT;			// delay time (s)
		DOUBLE	dESO;			// estimated static PWM output
		DOUBLE	dSPT;			// tuning point (degC)
		DOUBLE	dCBND;			// control band (degC)
	} PID_CTRL;

	PID_CTRL	m_stPIDCtrl[NUM_OF_LITEC *MAX_LTC_CH];

	// fuzzy control parameters
	typedef struct
	{
		DOUBLE	dUpperBand;			// Upper band to activate fuzzy control (degC)
		DOUBLE	dLowerBand;			// Lower band to activate fuzzy control (degC)
		DOUBLE	dOutputFactor;		// Output factor
	} FUZZY_CTRL;

	FUZZY_CTRL	m_stFuzzyCtrl[NUM_OF_LITEC *MAX_LTC_CH];

	//Litec init info
	LITEC_INFO	m_stLitec[NUM_OF_LITEC];

	// ------------------------------------------------------------------------
	// Function delcaration
	// ------------------------------------------------------------------------
	BOOL InitLitec();

	INT UpdateLitecStatus(INT num);
	INT GetLitecChNum(int nCh);
	INT GetLitecUnitNum(int nCh);
	INT GetLitecErrNum(int nCh);	// 20140708 Yip: Use Error Number Instead Of Channel Number For Litec Errors
	CString GetLitecChName(int nCh);	// 20140804 Yip: Get Litec Channel Name From Channel Number

	//Alarm & Protection Routines
	INT GetAlarmBand(INT num, DOUBLE *pdAlarmBand);
	INT SetAlarmBand(INT num, DOUBLE dAlarmBand);

	INT GetTempLmt(INT num, DOUBLE *pdTempLmt);
	INT SetTempLmt(INT num, DOUBLE dTempLmt);

	INT GetRampingProtection(INT num, LONG *plTimeLmt, DOUBLE *pdTempLmt);
	INT SetRampingProtection(INT num, LONG lTimeLmt = 60, DOUBLE dTempLmt = 1.0);

	// Basic Routines
	INT PowerOnLitec(INT num, BOOL bMode);
	BOOL IsLitecOn(INT num);
	BOOL IsLitecValidCh(INT nChNum);

	INT GetLitecTemp(INT num, DOUBLE *p_dTemp);
	INT GetLitecSetTemp(INT num, DOUBLE *p_dTemp);
	INT SetLitecTemp(INT num, DOUBLE dTemp);

	// Control and Tunning
	//AutoTune
	INT SetTunePara(INT num);
	INT TurnOnLitecAT(INT num, BOOL bMode);
	BOOL IsLitecLearningAT(INT num);

	//HOD
	INT LearnLitecHODMask(INT num);
	BOOL IsLitecLearningHOD(INT num);
	INT UpdateHODMask(INT num);

	//RMS, not in use
	INT LearnLitecRMSMask(INT num, BOOL bMode, INT nOpMode = 0); //0 = standby Mode, 1 = Working Mode
	INT IsLitecLearningRMS(INT num, BOOL *pStatus);

	INT PrintPIDPara(FILE *fp, INT num);
	INT GetPIDPara(INT num, PEBTC_CTRLPARAM_PID pstPID);
	INT SetPIDPara(INT num, PEBTC_CTRLPARAM_PID pstPID);
	BOOL SetCtrlPara(INT num, EBTC_CTRLPARAM_PID stTunedPID/*, LTC_FLT_BLK stTunedFfcFilter*/);	// 20140508 Yip

	INT GetHODThreshold(INT num, LONG *lThreshold);
	INT SetHODThreshold(INT num, LONG lThreshold);
	
	VOID GetTempValue(CTempDisplay *pStation);	
	BOOL SetDynamicOutputLimit(float fLimit = 1.0); //20130912

	CString FormatBCDVersion(DWORD dwVerNum);
	VOID PrintLitecInfomation();
	

};
