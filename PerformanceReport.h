#pragma once
#include "AC9000Stn.h"
#include <vector>
#include <iostream>
#include "stdafx.h"




typedef enum
{
	MPRT_START = 0,
	MPRT_STOP,
	MPRT_REFILL_ERROR,	//for material change this and MPRT_ERROR will both be set, but refill has priority over other error.
	MPRT_ERROR,
	MPRT_IDLE_START,
	MPRT_IDLE_END,
} MACHINE_PERFORMANCE_REPORT_TYPE;

typedef enum
{
	MPRM_NONE = 0,
	MPRM_AUTO,
	MPRM_TEST,
} MACHINE_PERFORMANCE_REPORT_MODE;


#define MACHINE_PERFORMANCE_REPORT_FOLDER "PerformanceReport"
#define MACHINE_PERFORMANCE_HMI_REPORT_CACHE "D:\\sw\\AC9000\\Release\\mpr_hmi_screen_cache.csv"
#define MACHINE_PERFORMANCE_REPORT_CYCLE 24		//24 hours a day, one day one cycle

class CPerformanceReportContainer
{
public:
	CString m_szDateTime;
	LONG m_lTime;
	MACHINE_PERFORMANCE_REPORT_TYPE m_lType;
	MACHINE_PERFORMANCE_REPORT_MODE m_lMode;

	CPerformanceReportContainer(CString inDataTime, LONG inTime, MACHINE_PERFORMANCE_REPORT_TYPE inType, MACHINE_PERFORMANCE_REPORT_MODE inMode);
};

//Report cycle is every 24 hrs a new report. 
class CPerformanceReport
{
	CCriticalSection _critSect;
public:
	CFile *m_fpPerformanceReportFile;
	CArchive *m_pPerformanceReportScreenAr; //klocwork fix 20130307
	CArchive *m_pPerformanceReportAr;
	CString m_szReportPath;
	CString m_szReportHeader;

	LONG m_lReportStartTime;
	LONG m_lShiftResetTime;
//	BOOL m_bShiftResetTime;	// 20141028 Yip: Correct Reset Time For New Report
	BOOL m_bResetHasTimeGap;

	CString m_szReportYear;
	CString m_szReportMonth;
	CString m_szReportDay;
	CString m_szTeamShift[MACHINE_PERFORMANCE_REPORT_CYCLE];
	CString m_szShiftConfigPath;

	LONG m_lCurrentTimeHr;


		//for switching over between hr
	BOOL m_bIsRunning;
	BOOL m_bIsIdle;
	BOOL m_bHasRefillRequest;		//means refilling
	BOOL m_bHasError;
	BOOL m_bRestart;
	BOOL m_bFirstTime; //Benny test

	INT m_nMrCounter;
	MACHINE_PERFORMANCE_REPORT_MODE m_lCurrentMode;


	LONG m_lStartReportTime;
	LONG m_lLastUpdateTime;

	LONG m_lLastUnitBondedTime;
	LONG m_lUnitBondedCounter;

	LONG m_lBondingStart;
	LONG m_lBondingEnd;

	std::vector <CPerformanceReportContainer> m_AllEvents;		//hold all events within 1 hr only
																	
	CPerformanceReport(CString szInShiftConfigPath);
	virtual ~CPerformanceReport();	// 20140819 Yip: Fix Memory Leak Issue
	VOID initialzation(); //20130307 klocwork fix
	VOID StartNewReport();
	VOID EndReport();
	VOID UpdateReport(CString deviceFileName);
		

	INT m_nNumberOfReport;
		
	BOOL TimeToUpdate();
	BOOL TimeForNextReport(); 

	VOID ReportEventTime(MACHINE_PERFORMANCE_REPORT_TYPE inType, MACHINE_PERFORMANCE_REPORT_MODE inMode);		//for start only
	VOID ReportEventTime(MACHINE_PERFORMANCE_REPORT_TYPE inType);
	VOID SetEventTime(MACHINE_PERFORMANCE_REPORT_TYPE inType);
		
	VOID CheckIdleUpdate(LONG inUnitBondedCounter);

	BOOL ShowPerformanceReport(INT inYear, INT inMonth, INT inDay);
	BOOL CopyCurrentReportToHMI(LONG inPage);
	BOOL Testing()
	{
		return true;
	}

	BOOL renewReportSettingConfig();
	BOOL ReadShiftScheduleConfig(CString szInPath);


private:
	CPerformanceReport(const CPerformanceReport &src) {}	// 20140926 Yip: Add Copy Constructor For Klocwork Fix
	CPerformanceReport& operator=(const CPerformanceReport &src)	// 20140926 Yip: Add Assignment Operator For Klocwork Fix
	{
		return *this;
	}
	CString CPerformanceReport::FormatDate(CHAR *inDate);
	LONG ScheduleStartingPosition(CHAR *inTime);
	INT SearchUpdateLocation(INT inUpdatePosition);

	LONG CalcRunTimeTotal(MACHINE_PERFORMANCE_REPORT_MODE inMode);
	LONG CalcDownTimeTotal();
	LONG CalcIdleTimeTotal();
	LONG CalcMaterialChangeTimeTotal();
		//LONG CalcAbnormalTimeTotal();

	// 20140903 Yip: Calculate Stop Count, Error Count, MTBA, MTBF In Performance Report
	LONG CalcStopCount(MACHINE_PERFORMANCE_REPORT_MODE inMode);
	LONG CalcErrorCount(MACHINE_PERFORMANCE_REPORT_MODE inMode);

	CString convertIntToMonth(INT inMonth);
	BOOL IsFileExist(CString sPathName);
	CString LoadExistReport(CString inPath);
};