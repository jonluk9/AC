/////////////////////////////////////////////////////////////////
//	TempDisplay.cpp : interface of the CTempDisplay class
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


#ifdef EXCEL_MACHINE_DEVICE_INFO
#include "ExcelMachineInfo_Util.h"
#endif

#define NUM_OF_HEATER_CHANNEL	(12)
#define NUM_OF_MACHINE		(2)

//#define	MB1L_HEATER			(0)
//#define	MB1R_HEATER			(1)
//#define	MBWH1L_HEATER		(2)
//#define	MBWH1R_HEATER		(3)
//#define	ACF1_HEATER			(4)
//#define	PB1_HEATER			(5)
//#define	MB2L_HEATER			(6)
//#define	MB2R_HEATER			(7)
//#define	MBWH2L_HEATER		(8)
//#define	MBWH2R_HEATER		(9)
//#define	ACF2_HEATER			(10)
//#define	PB2_HEATER			(11)


#ifdef TEMPDISPLAY_EXTERN
extern BOOL				g_bStopCycle;
extern BOOL				g_bDiagMode;
extern AUTOMODE			AutoMode;
extern LONG				g_lGlassInputMode;

extern STATUS_TYPE ACF_Status[MAX_NUM_OF_ACF_UNIT];
extern STATUS_TYPE ACF_WH_Status[MAX_NUM_OF_ACFWH];
//extern STATUS_TYPE PREBOND_HEAD_Status[MAX_NUM_OF_PREBOND];
//extern STATUS_TYPE MAINBOND_HEAD_Status[MAX_NUM_WITH_MAINBOND_3];
#endif

class CTempDisplay : public CAC9000Stn
{
	DECLARE_DYNCREATE(CTempDisplay)
	friend class CAC9000App;
	friend class CLitecCtrl;
	friend class CHouseKeeping; //20120305
	friend class CPreBond; //20130702

public:

	BOOL		m_bHeaterOn[NUM_OF_LITEC *NUM_OF_HEATER_CHANNEL];
	DOUBLE		m_dHeaterTemp[NUM_OF_LITEC *NUM_OF_HEATER_CHANNEL];
	DOUBLE		m_dHeaterSetTemp[NUM_OF_LITEC *NUM_OF_HEATER_CHANNEL];
	DOUBLE		m_dHeaterTempTol[NUM_OF_LITEC *NUM_OF_HEATER_CHANNEL];
	DOUBLE		m_dBondPtTemp[NUM_OF_LITEC *NUM_OF_HEATER_CHANNEL];
	BOOL		m_bHeaterATOn[NUM_OF_LITEC *NUM_OF_HEATER_CHANNEL];
	BOOL		m_bHeaterExist[NUM_OF_LITEC *NUM_OF_HEATER_CHANNEL];  //NEW
	clock_t		m_clkHeaterOnTime[NUM_OF_LITEC *NUM_OF_HEATER_CHANNEL]; //20150309

	INT			m_nCurIndex;

	// temp calibration
	TEMP_DATA	m_stTempCalib[NUM_OF_LITEC *NUM_OF_HEATER_CHANNEL][MAX_TEMP_DATA];
	BOOL		m_bValidTemp[NUM_OF_LITEC *NUM_OF_HEATER_CHANNEL];

	//Monitor Related
	BOOL		m_bMonFailed[NUM_OF_LITEC *NUM_OF_HEATER_CHANNEL];
	DOUBLE		m_dMonTemp[NUM_OF_LITEC *NUM_OF_HEATER_CHANNEL];
	
	// Logging
	BOOL		m_bMonTemp;
	DOUBLE		m_dMonOffset[NUM_OF_LITEC *NUM_OF_HEATER_CHANNEL];
	DOUBLE		m_dMonTol[NUM_OF_LITEC *NUM_OF_HEATER_CHANNEL];
	DWORD		m_dwFaultTimerStart[NUM_OF_LITEC *NUM_OF_HEATER_CHANNEL];

	BOOL		m_bHeatersOn;	// 20140918 Yip: Add Flag To Toggle All Heaters Power On/Off

protected:
	

protected:
	//Update
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();

	virtual VOID RegisterVariables();

private:
	CString GetStation() 
	{
		return "TD_";
	}

public:
	CTempDisplay();
	virtual	~CTempDisplay();

	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID Operation();

protected:
	// ------------------------------------------------------------------------
	// Variable delcaration
	// ------------------------------------------------------------------------

	// So Info
	//CSoInfo		m_stPB1HeaterCoolingSol;
	//CSoInfo		m_stPB2HeaterCoolingSol;
	//CSoInfo		m_stMB1HeaterCoolingSol;
	//CSoInfo		m_stMB2HeaterCoolingSol;
	//CSoInfo		m_stMB3HeaterCoolingSol; //Milton
	//CSoInfo		m_stMB4HeaterCoolingSol;

	// ------------------------------------------------------------------------
	// End of SI/SO
	// ------------------------------------------------------------------------

	BOOL		m_bHeaterFailed[NUM_OF_LITEC *NUM_OF_HEATER_CHANNEL];
//p20121004
	DOUBLE		m_dMB1WHLeftAlarmTemp;
//p20121004:end
	// ------------------------------------------------------------------------
	// Function delcaration
	// ------------------------------------------------------------------------
	
	//Heater Cooling
	//INT SetCoolingSol(CSoInfo *stTempSo, BOOL bMode, BOOL bWait = SFM_NOWAIT);

	//Heater
	INT PowerOnHeater(INT num, BOOL bMode);
	BOOL PowerOnAllHeater(BOOL bMode); //20120305 problem 13 air supply error

	INT GetHeaterTemp(INT num, DOUBLE *p_dTemp);
	INT GetHeaterSetTemp(INT num, DOUBLE *p_dTemp);

	INT SetHeaterTemp(INT num, DOUBLE dTemp);
	INT SetHeaterTempFromData();

	//Tuning
	INT TurnOnHeaterAT(INT num, BOOL bMode);	
	INT LearnHODMask(INT num);
	INT LearnRMSMask(INT num);

	//Checking
	BOOL CheckHeaterTempReady();
	BOOL CheckHeaterInit();
	BOOL CheckAllHeaterOn(); //20130227
	BOOL IsAllHeaterOnByStationName(CString &szStationName); //20130424
	BOOL CheckHeaterPowerForAutobond(); //20130430
	BOOL IsLitecHeaterExist(INT nHeaterChNum);
	
	//UpdateOutput Checking
	BOOL CheckHeaterMalfunctionTimeout(INT nCurindex);	//return TRUE if Fault Time exceed Malfunction timeout
	BOOL CheckHeaterCurTemp(INT nCurindex);				//return TRUE if Current Temp has error
	BOOL CheckHeaterFail(INT nCurindex);				//Check heater fail only if module is selected
	VOID CheckHeaterCooling(INT nCurindex);

	VOID ResetAllData(LONG lChannel);
	
	virtual VOID PrintMachineSetup(FILE *fp);
	virtual VOID PrintDeviceSetup(FILE *fp);
#ifdef EXCEL_MACHINE_DEVICE_INFO
	virtual BOOL printMachineDeviceInfo();
#endif

	//For Saving data to File
	VOID ConvertToFileName(INT nChn, CString *pszName, CString *pszSetTemp, CString *pszBondTemp);
	VOID ConvertToModuleName(INT nChn, CString *pszModuleName);
	VOID ConvertToStationName(INT nChn, CString *pszStationName); //20130424

protected:
	// ------------------------------------------------------------------------
	// HMI Variable delcaration
	// ------------------------------------------------------------------------

	// Values are for HMI display only! Users should not use these in coding.


	// ------------------------------------------------------------------------
	// IPC Command delcaration
	// ------------------------------------------------------------------------
	//I/Os
	//LONG HMI_TogglePB1HeaterCoolingSol(IPC_CServiceMessage &svMsg);
	//LONG HMI_TogglePB2HeaterCoolingSol(IPC_CServiceMessage &svMsg);
	//LONG HMI_ToggleMB1HeaterCoolingSol(IPC_CServiceMessage &svMsg);
	//LONG HMI_ToggleMB2HeaterCoolingSol(IPC_CServiceMessage &svMsg);
	//LONG HMI_ToggleMB3HeaterCoolingSol(IPC_CServiceMessage &svMsg);
	//LONG HMI_ToggleMB4HeaterCoolingSol(IPC_CServiceMessage &svMsg);
	
	LONG HMI_PowerOnAllHeater(IPC_CServiceMessage &svMsg);
	LONG HMI_PowerOnHeater(IPC_CServiceMessage &svMsg);

	LONG HMI_SetHeaterTemp(IPC_CServiceMessage &svMsg);
	LONG HMI_SetTempTol(IPC_CServiceMessage &svMsg);
	LONG HMI_SetBondTemp(IPC_CServiceMessage &svMsg);

	//Tuning
	LONG HMI_SetHeaterATPara(IPC_CServiceMessage &svMsg);
	LONG HMI_TurnOnHeaterAT(IPC_CServiceMessage &svMsg);

	//Set Protections
	LONG HMI_SetAlarmBand(IPC_CServiceMessage &svMsg);
	LONG HMI_LearnHODMask(IPC_CServiceMessage &svMsg);
	LONG HMI_SetHODThreshold(IPC_CServiceMessage &svMsg);
	LONG HMI_LearnRMSMask(IPC_CServiceMessage &svMsg);

	// Temp. Monitor related
	LONG HMI_SetMonTemp(IPC_CServiceMessage &svMsg);
	LONG HMI_SetMonOffset(IPC_CServiceMessage &svMsg);
	LONG HMI_SetMonTol(IPC_CServiceMessage &svMsg);	

	// temperature calib.
	LONG HMI_TempInsertData(IPC_CServiceMessage &svMsg);
	LONG HMI_TempResetData(IPC_CServiceMessage &svMsg);
	LONG HMI_TempResetAllData(IPC_CServiceMessage &svMsg);
	
	//reset Temp Lmt
	LONG HMI_SetTempLmt(IPC_CServiceMessage &svMsg);
	
	//Print Litec INFO
	LONG HMI_PrintLitecInformation(IPC_CServiceMessage &svMsg);
	LONG HMI_SetDynOutputLmt(IPC_CServiceMessage &svMsg); //20130912


	LONG HMI_ResetAllHeater(IPC_CServiceMessage &svMsg); //20130413
	//Reset Channal (ON/OFF and AutoTune OFF)
	LONG HMI_ResetHeaterChannel(IPC_CServiceMessage &svMsg);

	BOOL LogTempretureInfo(CString &szLabel); //20150629

	// Saving / loading parameters
	virtual LONG IPC_SaveMachineParam();
	virtual LONG IPC_LoadMachineParam();
	virtual LONG IPC_SaveDeviceParam();
	virtual LONG IPC_LoadDeviceParam();
};
