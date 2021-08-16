#include "stdafx.h"
#include "AC9000Stn.h"
#include "PerformanceReport.h"
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>



CPerformanceReportContainer::CPerformanceReportContainer(CString inDataTime, LONG inTime, MACHINE_PERFORMANCE_REPORT_TYPE inType, MACHINE_PERFORMANCE_REPORT_MODE inMode)
{
	m_lTime = inTime;
	m_lType = inType;
	m_lMode = inMode;
	m_szDateTime = inDataTime;
}

CPerformanceReport::CPerformanceReport(CString szInShiftConfigPath)
{
	CString shiftDefault[] = {"AAA", "BBB", "CCC", "DDD", "EEE", "FFF", "GGG", "HHH", "III", "JJJ", "KKK", "LLL", "MMM", "NNN", "OOO", "PPP", "QQQ", "RRR", "SSS", "TTT", "UUU", "VVV", "WWW", "XXX"}; 
	m_bIsRunning = false;
	m_bHasRefillRequest = false;
	m_bHasError = false;
	m_bRestart = false;
	m_bIsIdle = false;
	m_bResetHasTimeGap = false;
	m_nNumberOfReport = 0;
	m_lCurrentMode = MPRM_NONE;
	m_lLastUnitBondedTime = clock();
	m_lUnitBondedCounter = 0;
	m_lBondingStart = 0;
	m_lBondingEnd = 0;
	m_szShiftConfigPath = szInShiftConfigPath;
	if (!ReadShiftScheduleConfig(m_szShiftConfigPath))
	{
		for (int i = 0; i < MACHINE_PERFORMANCE_REPORT_CYCLE; i++)
		{
			m_szTeamShift[i] = shiftDefault[i];
		}
	}
	CHAR tmpbuf[128];
	_strtime(tmpbuf);
	m_lReportStartTime = ScheduleStartingPosition(tmpbuf);		//for release testing
	m_lCurrentTimeHr = ScheduleStartingPosition(tmpbuf);
	//m_lShiftResetTime = 14;		//for testing only
	m_pPerformanceReportScreenAr = NULL;
#if 1 //20130312 klocwork fix
	m_nMrCounter = 0;
	m_pPerformanceReportAr = NULL;
	initialzation();
#else
	CFile screenDisplayFile;
	if (screenDisplayFile.Open(MACHINE_PERFORMANCE_HMI_REPORT_CACHE, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
	{
		m_pPerformanceReportAr = new CArchive(&screenDisplayFile, CArchive::store);
		m_nMrCounter = 0;

		m_szReportHeader.Format("Index,Shift,%s/%s/%s,Device File, Run Time, Test Time, Idle Time, Up Time, Down Time, Material Change Time, Abnormal Time,Unit Bonded,Stop Count,MTBA,Error Count,MTBF,\n", m_szReportYear, m_szReportMonth, m_szReportDay);
		m_pPerformanceReportAr->WriteString(m_szReportHeader);		//write the header to the new file
		m_pPerformanceReportAr->Flush();
		m_pPerformanceReportAr->Close();

		delete m_pPerformanceReportAr;
		screenDisplayFile.Close();
	}
//	m_bShiftResetTime = true;	// 20141028 Yip: Correct Reset Time For New Report
	SetEventTime(MPRT_STOP);
#endif
}

CPerformanceReport::~CPerformanceReport()	// 20140819 Yip: Fix Memory Leak Issue
{
	if ((m_fpPerformanceReportFile != NULL) && (m_pPerformanceReportAr != NULL))
	{
		m_fpPerformanceReportFile->Close();
		m_pPerformanceReportAr->Close();
		delete m_fpPerformanceReportFile;
		delete m_pPerformanceReportAr;
	}
}

#if 1 //20130307 klocwork fix
VOID CPerformanceReport::initialzation()
{
	CFile screenDisplayFile;
	if (screenDisplayFile.Open(MACHINE_PERFORMANCE_HMI_REPORT_CACHE, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
	{
		m_pPerformanceReportScreenAr = new CArchive(&screenDisplayFile, CArchive::store);
		m_nMrCounter = 0;

		m_szReportHeader.Format("Index,Shift,%s/%s/%s,Device File, Run Time, Test Time, Idle Time, Up Time, Down Time, Material Change Time, Abnormal Time,Unit Bonded,Stop Count,MTBA,Error Count,MTBF,\n", m_szReportYear, m_szReportMonth, m_szReportDay);
		m_pPerformanceReportScreenAr->WriteString(m_szReportHeader);		//write the header to the new file
		m_pPerformanceReportScreenAr->Flush();
		m_pPerformanceReportScreenAr->Close();

		delete m_pPerformanceReportScreenAr;
		screenDisplayFile.Close();
	}
//	m_bShiftResetTime = true;	// 20141028 Yip: Correct Reset Time For New Report
	SetEventTime(MPRT_STOP);
	m_bFirstTime = TRUE; // benny test
}
#endif

VOID CPerformanceReport::StartNewReport()
{
	CString shiftDefault[] = {"AAA", "BBB", "CCC", "DDD", "EEE", "FFF", "GGG", "HHH", "III", "JJJ", "KKK", "LLL", "MMM", "NNN", "OOO", "PPP", "QQQ", "RRR", "SSS", "TTT", "UUU", "VVV", "WWW", "XXX"}; 

	_critSect.Lock();
	m_lStartReportTime = clock();
	m_lLastUpdateTime = m_lStartReportTime;
		
	CHAR tmpbuf[128];
	CString reportFileName = "";

	_strdate(tmpbuf);

	FormatDate(tmpbuf);

	TCHAR currentDirectory[101];
	GetCurrentDirectory(100, currentDirectory);
	
	m_szReportPath.Format("D:\\\\sw\\\\AC9000\\\\Data\\\\%s", MACHINE_PERFORMANCE_REPORT_FOLDER);
	_mkdir(m_szReportPath);														//create the report directory
	m_szReportPath += "\\\\" + m_szReportYear;
	_mkdir(m_szReportPath);														//create the year directory, if it does not exists

	INT monthInt = 0;
	sscanf(m_szReportMonth, "%d", &monthInt);

	m_szReportPath += "\\\\" + convertIntToMonth(monthInt);
	_mkdir(m_szReportPath);														//create the month directory, if it does not exists

	CString testPath = "";
	do		//test if the file already exists
	{
		testPath = m_szReportPath;
		m_nNumberOfReport++;
		reportFileName.Format("\\\\%s_%d.csv", m_szReportDay, m_nNumberOfReport);
		testPath += reportFileName;
	} while (IsFileExist(testPath));

	//reportFileName.Format("\\\\%s.csv", m_szReportDay);
	m_szReportPath += reportFileName;

	reportFileName += ".csv";

	CString oldData = "";
	if (!ReadShiftScheduleConfig(m_szShiftConfigPath))						//read the shift config file defined by the user
	{
		for (int i = 0; i < MACHINE_PERFORMANCE_REPORT_CYCLE; i++)			//if the file is not defined, use the default setting
		{
			m_szTeamShift[i] = shiftDefault[i];
		}
	}

	//file does not exists create a new file
	m_fpPerformanceReportFile = new CFile();
	if (m_fpPerformanceReportFile->Open(m_szReportPath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
	{
		m_pPerformanceReportAr = new CArchive(m_fpPerformanceReportFile, CArchive::store);
		m_nMrCounter = 0;

		m_pPerformanceReportAr->WriteString("AC9000,\n");
		m_pPerformanceReportAr->Flush();
		m_szReportHeader.Format("Index,Shift,%s/%s/%s,Device File, Run Time, Test Time, Idle Time, Up Time, Down Time, Material Change Time, Abnormal Time,Unit Bonded,Stop Count,MTBA,Error Count,MTBF,\n", m_szReportYear, m_szReportMonth, m_szReportDay);
		m_pPerformanceReportAr->WriteString(m_szReportHeader);		//write the header to the new file
		m_pPerformanceReportAr->Flush();
	}

	_strtime(tmpbuf);
	m_lReportStartTime = ScheduleStartingPosition(tmpbuf);		////for release testing, set the report starting point at the current HR

	/* 20141028 Yip: Correct Reset Time For New Report
	if (m_bShiftResetTime)
	{
		m_bShiftResetTime = false;
	}
	*/

	_critSect.Unlock();
}

VOID CPerformanceReport::UpdateReport(CString deviceFileName)
{
	_critSect.Lock();

	CHAR tmpbuf[128];

	LONG current_time = clock();
	__time64_t ltime;
	_time64(&ltime);

	_strtime(tmpbuf);

	if (m_bIsRunning)
	{
		SetEventTime(MPRT_STOP);
	}
	else
	{
		SetEventTime(MPRT_START);
	}

	if (m_bIsIdle)
	{
		m_AllEvents.push_back(CPerformanceReportContainer(_ctime64(&ltime), current_time, MPRT_IDLE_END, m_lCurrentMode));
	}

	//Search pattern for calculation below
	LONG autoRunTimeTotal = CalcRunTimeTotal(MPRM_AUTO) / CLOCKS_PER_SEC;
	LONG testRunTimeTotal = CalcRunTimeTotal(MPRM_TEST) / CLOCKS_PER_SEC;
	LONG runTimeTotal = autoRunTimeTotal + testRunTimeTotal;
//	LONG downTimeTotal = CalcDownTimeTotal() / CLOCKS_PER_SEC;
	LONG downTimeTotal = 3600 - runTimeTotal;	// 20141027 Yip: Calculate Down Time As The Time Machine Without Running Within Current Hour
	if (downTimeTotal < 0)
	{
		downTimeTotal = CalcDownTimeTotal() / CLOCKS_PER_SEC;
	}
	LONG idleTimeTotal = CalcIdleTimeTotal() / CLOCKS_PER_SEC;
	LONG upTimeTotal = runTimeTotal - idleTimeTotal;
	LONG materialChangeTimeTotal = CalcMaterialChangeTimeTotal() / CLOCKS_PER_SEC;
	LONG abnormalTimeTotal = downTimeTotal - materialChangeTimeTotal; //CalcAbnormalTimeTotal() / CLOCKS_PER_SEC;

	// 20140903 Yip: Calculate Stop Count, Error Count, MTBA, MTBF In Performance Report
	LONG lStopCount = CalcStopCount(MPRM_AUTO) + CalcStopCount(MPRM_TEST);
	LONG lErrorCount = CalcErrorCount(MPRM_AUTO) + CalcErrorCount(MPRM_TEST);
	CString szMTBA("N/A");
	CString szMTBF("N/A");
	if (lStopCount > 0)	// 20140918 Yip
	{
		szMTBA.Format("%.2lf", ((double)upTimeTotal / (double)lStopCount));
	}
	if (lErrorCount > 0)	// 20140918 Yip
	{
		szMTBF.Format("%.2lf", ((double)upTimeTotal / (double)lErrorCount));
	}

	if (m_fpPerformanceReportFile != NULL && m_pPerformanceReportAr != NULL)
	{
		//Write result to the .csv output file
		LONG lCurrentHour = ((m_nMrCounter + m_lReportStartTime) % 24);
		LONG shiftTeamIndex = (24 + lCurrentHour - m_lShiftResetTime) % 24;
		CString content;
		content.Format("%ld,%s,%02ld:00,%s,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%s,%ld,%s,\n", m_nMrCounter + 1, m_szTeamShift[shiftTeamIndex], lCurrentHour, deviceFileName, autoRunTimeTotal, testRunTimeTotal, idleTimeTotal, upTimeTotal, downTimeTotal, materialChangeTimeTotal, abnormalTimeTotal, (m_lBondingEnd - m_lBondingStart), lStopCount, szMTBA, lErrorCount, szMTBF);
		m_pPerformanceReportAr->WriteString(content);
		m_pPerformanceReportAr->Flush();
	}

	m_lLastUpdateTime = clock();

	m_AllEvents.clear(); //clear all the events for next hr restart
	m_nMrCounter++;

	if (m_bIsRunning)
	{
		SetEventTime(MPRT_START);
		if (m_bHasRefillRequest)
		{
			SetEventTime(MPRT_REFILL_ERROR);
		}
	}
	else
	{
		if (m_bHasRefillRequest)
		{
			SetEventTime(MPRT_REFILL_ERROR);
		}
		SetEventTime(MPRT_STOP);
	}

	if (m_bIsIdle)
	{
		m_lLastUnitBondedTime = current_time;
		m_AllEvents.push_back(CPerformanceReportContainer(_ctime64(&ltime), current_time, MPRT_IDLE_START, m_lCurrentMode));
	}
	m_bRestart = true;		// not fist time running

	/* 20141028 Yip: Correct Reset Time For New Report
	if (ScheduleStartingPosition(tmpbuf) != m_lShiftResetTime)
	{
		m_bShiftResetTime = true;
	}
	*/

	_critSect.Unlock();

}

VOID CPerformanceReport::CheckIdleUpdate(LONG inUnitBondedCounter)
{
	LONG current_time = clock();
	__time64_t ltime;
	_time64(&ltime);

	LONG lIdleTimeLimitInMs = 10000;	// 20141103 Yip: Change Idle Time Limit From 60000 ms To 5000 ms To 10000 ms

	if ((m_lUnitBondedCounter == inUnitBondedCounter) && (current_time - m_lLastUnitBondedTime > lIdleTimeLimitInMs) && !m_bIsIdle)	//there is no output for over 1 minute
	{
		m_AllEvents.push_back(CPerformanceReportContainer(_ctime64(&ltime), clock(), MPRT_IDLE_START, m_lCurrentMode)); // the m_lLastUnitBondedTime should be using, but the error rate is too large for testing
		m_bIsIdle = true;
	}
	else if (m_bIsIdle && (m_lUnitBondedCounter != inUnitBondedCounter))	//there is output from idle
	{
		m_AllEvents.push_back(CPerformanceReportContainer(_ctime64(&ltime), clock(), MPRT_IDLE_END, m_lCurrentMode));
		m_bIsIdle = false;
		m_lLastUnitBondedTime = current_time;
		m_lUnitBondedCounter = inUnitBondedCounter;
	}
	else if ((m_lUnitBondedCounter != inUnitBondedCounter))	//there is output
	{
		m_lLastUnitBondedTime = current_time;
		m_lUnitBondedCounter = inUnitBondedCounter;
	}
}

LONG CPerformanceReport::CalcIdleTimeTotal()
{
	LONG result = 0;
	BOOL endIdle = false;
	LONG start_cursor = 0;
	LONG end_cursor = 0;
	std::vector<CPerformanceReportContainer>::iterator it;
	for (it = m_AllEvents.begin(); it != m_AllEvents.end(); it++)
	{
		if (it->m_lType == MPRT_IDLE_START || it->m_lType == MPRT_IDLE_END)
		{
			if (!endIdle && it->m_lType == MPRT_IDLE_START)
			{
				start_cursor = it->m_lTime;
				endIdle = true;
			}
			else if (endIdle && it->m_lType == MPRT_IDLE_END)
			{
				end_cursor = it->m_lTime;
				result += (end_cursor - start_cursor);
				endIdle = false;
			}
		}
	}
	return result;
}

LONG CPerformanceReport::CalcRunTimeTotal(MACHINE_PERFORMANCE_REPORT_MODE inMode)
{
	LONG result = 0;
	BOOL startRunning = false;
	LONG start_cursor = 0;
	LONG stop_cursor = 0;
	std::vector<CPerformanceReportContainer>::iterator it;
	for (it = m_AllEvents.begin(); it != m_AllEvents.end(); it++)
	{
		if ((it->m_lType == MPRT_START || it->m_lType == MPRT_STOP) && it->m_lMode == inMode)
		{
			if (!startRunning && it->m_lType == MPRT_START)
			{
				start_cursor = it->m_lTime;
				startRunning = true;
			}
			else if (startRunning && it->m_lType == MPRT_STOP)
			{
				stop_cursor = it->m_lTime;
				result += (stop_cursor - start_cursor);
				startRunning = false;
			}
		}
	}
	return result;
}

LONG CPerformanceReport::CalcMaterialChangeTimeTotal()
{
	BOOL hasRefillRequest = false;
	LONG result = 0;
	BOOL stopRunning = false;
	LONG start_cursor = 0;
	LONG stop_cursor = 0;
	std::vector<CPerformanceReportContainer>::iterator it;
	for (it = m_AllEvents.begin(); it != m_AllEvents.end(); it++)
	{
		if (it->m_lType == MPRT_START || it->m_lType == MPRT_STOP || it->m_lType == MPRT_REFILL_ERROR)
		{
			if (it->m_lType == MPRT_REFILL_ERROR)
			{
				hasRefillRequest = true;
			}
			
			else if (!stopRunning && it->m_lType == MPRT_STOP)
			{
				if (hasRefillRequest)
				{
					start_cursor = it->m_lTime;
				}
				stopRunning = true;
			}
			else if (stopRunning && it->m_lType == MPRT_START)
			{
				if (hasRefillRequest)
				{
					stop_cursor = it->m_lTime;
					result += (stop_cursor - start_cursor);
					hasRefillRequest = false;
				}
				stopRunning = false;
			}
		}
	}
	return result;
}


/*LONG CPerformanceReport::CalcAbnormalTimeTotal()//downtime - material changing time
{
	BOOL hasError = false;
	LONG result = 0;
	BOOL stopRunning = false;
	LONG start_cursor = 0;
	LONG stop_cursor = 0;
	std::vector<CPerformanceReportContainer>::iterator it;
	for (it = m_AllEvents.begin(); it != m_AllEvents.end(); it++)
	{
		if(it->m_lType == MPRT_START || it->m_lType == MPRT_STOP || it->m_lType == MPRT_ERROR)
		{
			if(it->m_lType == MPRT_ERROR)//20130307
				hasError = true;
			
			else if(!stopRunning && it->m_lType == MPRT_STOP)
			{
				if(hasError)
					start_cursor = it->m_lTime;
				stopRunning = true;
			}
			else if(stopRunning && it->m_lType == MPRT_START)
			{
				if(hasError)
				{
					stop_cursor = it->m_lTime;
					result+= (stop_cursor - start_cursor);
					hasError = false;
				}
				stopRunning = false;
			}
		}
	}
	return result;
}*/


LONG CPerformanceReport::CalcDownTimeTotal()
{
	LONG result = 0;
	BOOL stopRunning = false;
	LONG start_cursor = 0;
	LONG stop_cursor = 0;
	std::vector<CPerformanceReportContainer>::iterator it;
	for (it = m_AllEvents.begin(); it != m_AllEvents.end(); it++)
	{
		if (m_bFirstTime == TRUE)
		{
			continue;
		}
		if (it->m_lType == MPRT_START || it->m_lType == MPRT_STOP)
		{
			if (!stopRunning && it->m_lType == MPRT_STOP)
			{
				start_cursor = it->m_lTime;
				stopRunning = true;
			}
			else if (stopRunning && it->m_lType == MPRT_START)
			{
				stop_cursor = it->m_lTime;
				result += (stop_cursor - start_cursor);
				stopRunning = false;
			}
		}
	}
	return result;
}

// 20140903 Yip: Calculate Stop Count, Error Count, MTBA, MTBF In Performance Report
LONG CPerformanceReport::CalcStopCount(MACHINE_PERFORMANCE_REPORT_MODE inMode)
{
	LONG result = 0;
	BOOL startRunning = FALSE;
	std::vector<CPerformanceReportContainer>::iterator it;
	for (it = m_AllEvents.begin(); it != m_AllEvents.end(); it++)
	{
		if ((it->m_lType == MPRT_START || it->m_lType == MPRT_STOP) && it->m_lMode == inMode)
		{
			if (!startRunning && it->m_lType == MPRT_START)
			{
				startRunning = TRUE;
			}
			else if (startRunning && it->m_lType == MPRT_STOP)
			{
				++result;
				startRunning = FALSE;
			}
		}
	}
	return result;
}

// 20140903 Yip: Calculate Stop Count, Error Count, MTBA, MTBF In Performance Report
LONG CPerformanceReport::CalcErrorCount(MACHINE_PERFORMANCE_REPORT_MODE inMode)
{
	BOOL hasError = FALSE;
	LONG result = 0;
	BOOL startRunning = FALSE;
	std::vector<CPerformanceReportContainer>::iterator it;
	for (it = m_AllEvents.begin(); it != m_AllEvents.end(); it++)
	{
		// 20140929 Yip: Correct Calculation Of Error Count For Performance Report
		if ((it->m_lType == MPRT_START || it->m_lType == MPRT_STOP || it->m_lType == MPRT_ERROR) && it->m_lMode == inMode)
		{
			if (!startRunning)
			{
				if (it->m_lType == MPRT_START)
				{
					startRunning = TRUE;
				}
			}
			else
			{
				if (it->m_lType == MPRT_ERROR)
				{
					hasError = TRUE;
				}
				else if (it->m_lType == MPRT_STOP)
				{
					if (hasError)
					{
						++result;
						hasError = FALSE;
					}
					startRunning = FALSE;
				}
			}
		}
	}
	return result;
}

VOID CPerformanceReport::EndReport()		//this function also calls StartNewReport
{
	_critSect.Lock();
	if (m_fpPerformanceReportFile != NULL && m_pPerformanceReportAr != NULL)
	{
		m_fpPerformanceReportFile->Close();
		m_pPerformanceReportAr->Close();
		delete m_fpPerformanceReportFile;
		delete m_pPerformanceReportAr; //20130307 klocwork fix
	}
	
	//clear all data

	_critSect.Unlock();
	m_bRestart = true;		//copy over for next day
	StartNewReport();
}

VOID CPerformanceReport::ReportEventTime(MACHINE_PERFORMANCE_REPORT_TYPE inType, MACHINE_PERFORMANCE_REPORT_MODE inMode) //auto or test mode is one of the input
{
	__time64_t ltime;
	_time64(&ltime);
	m_lCurrentMode = inMode;
	LONG current_time = clock();
	m_bFirstTime = FALSE; // benny test

	m_AllEvents.push_back(CPerformanceReportContainer(_ctime64(&ltime), clock(), inType, inMode));

	switch (inType)			//report status for copy over between hr
	{
	case MPRT_START:
		m_bIsRunning = true;
		m_bHasRefillRequest = false;
		break;
	case MPRT_STOP:
		if (m_bIsIdle)	//stop action also stop idle
		{
			m_AllEvents.push_back(CPerformanceReportContainer(_ctime64(&ltime), current_time, MPRT_IDLE_END, m_lCurrentMode));
			m_bIsIdle = false;
			m_lLastUnitBondedTime = current_time;
		}
		m_bIsRunning = false;
		break;
	case MPRT_REFILL_ERROR:
		m_bHasRefillRequest = true;
		break;
	case MPRT_ERROR:
		m_bHasError = true;
		break;
	case MPRT_IDLE_START:
		m_bIsIdle = true;
		break;
	case MPRT_IDLE_END:
		m_bIsIdle = false;
		break;
	}
}

VOID CPerformanceReport::ReportEventTime(MACHINE_PERFORMANCE_REPORT_TYPE inType)	//mode is using m_lCurrentMode with this function
{
	__time64_t ltime;
	_time64(&ltime);
	LONG current_time = clock();
	m_bFirstTime = FALSE; //benny test

	switch (inType)		//report status for copy over between hr
	{
	case MPRT_START:
		m_bIsRunning = true;
		m_bHasRefillRequest = false;
		break;
	case MPRT_STOP:
		if (m_bIsIdle)	//stop action also idle
		{
			m_AllEvents.push_back(CPerformanceReportContainer(_ctime64(&ltime), current_time, MPRT_IDLE_END, m_lCurrentMode));
			m_bIsIdle = false;
			m_lLastUnitBondedTime = current_time;
		}
		m_bIsRunning = false;
		break;
	case MPRT_REFILL_ERROR:
		m_bHasRefillRequest = true;
		break;
	case MPRT_ERROR:
		m_bHasError = true;
		break;
	case MPRT_IDLE_START:
		m_bIsIdle = true;
		break;
	case MPRT_IDLE_END:
		m_bIsIdle = false;
		break;
	}
	m_AllEvents.push_back(CPerformanceReportContainer(_ctime64(&ltime), clock(), inType, m_lCurrentMode));
}

VOID CPerformanceReport::SetEventTime(MACHINE_PERFORMANCE_REPORT_TYPE inType)
{
	__time64_t ltime;
	_time64(&ltime);

	m_AllEvents.push_back(CPerformanceReportContainer(_ctime64(&ltime), clock(), inType, m_lCurrentMode));
}

//Counter for hourly report
BOOL CPerformanceReport::TimeToUpdate()
{
	CHAR tmpbuf[128];
	_strtime(tmpbuf);
	LONG lTmpTime = ScheduleStartingPosition(tmpbuf);

	
	//Condition setup
	//lTmpTime:				current time
	//m_lCurrentTimeHr:		Last report time
	//m_lShiftResetTime:	First shift starting time from the HMI interface input
	if (lTmpTime != m_lCurrentTimeHr/* && m_lShiftResetTime != ScheduleStartingPosition(tmpbuf)*/)	// 20141028 Yip: Correct Reset Time For New Report
	{
		m_lCurrentTimeHr = lTmpTime;
		return true;
	}
	return false;
}

//Counter for daily report
BOOL CPerformanceReport::TimeForNextReport()		//time = HMI preset time
{
	if (m_nMrCounter == 0)	// 20141028 Yip: Correct Reset Time For New Report
	{
		return false;
	}
//	if (m_bShiftResetTime)	// 20141028 Yip: Correct Reset Time For New Report
//	{
	CHAR tmpbuf[128];
	_strtime(tmpbuf);

	// 20141028 Yip: Correct Reset Time For New Report (Change From (MACHINE_PERFORMANCE_REPORT_CYCLE - 1) To MACHINE_PERFORMANCE_REPORT_CYCLE)
	//Condition setup: 
	if (m_lShiftResetTime == ScheduleStartingPosition(tmpbuf) || m_nMrCounter >= MACHINE_PERFORMANCE_REPORT_CYCLE)
	{
		return true;
	}
//	}
	return false;
}

//Reformat input from 11/30/12 to 11/30/2012 as output
//Assign m_szReportYear, m_szReportMonth, m_szReportDay 
//(this function both set and get for convenience only)
CString CPerformanceReport::FormatDate(CHAR *inDate)
{
	CString result = "";
	CHAR *token;
	INT iii = 0;
	CString reportDate = "";
	CString tmpStr = "";

	token = strtok(inDate, "/");
	while (token != NULL)
	{
		if (iii == 0)
		{
			result.Append(token);
			result.Append("/");
			m_szReportMonth.Format("%s", token);
		}
		else if (iii == 1)
		{
			result.Append(token);
			result.Append("/");
			m_szReportDay.Format("%s", token);
		}
		else if (iii == 2)
		{
			result.Append("20");	//year 2012, the "20"
			result.Append(token);
			m_szReportYear.Format("20%s", token);
		}
		iii++;
		token = strtok(NULL, "/");
	}
	delete token;
	return result;
}


LONG CPerformanceReport::ScheduleStartingPosition(CHAR *inTime)
{
	LONG result = 0;
	CHAR *token;
	INT iii = 0;
	if ((token = strtok(inTime, ":")) != NULL)
	{
		sscanf(token, "%ld", &result);
	}
	return result;
}

INT CPerformanceReport::SearchUpdateLocation(INT inUpdatePosition)
{
	INT scheuduleMap[24];
	for (int i = 0; i < 23; i++)
	{
		scheuduleMap[i] = ((m_lReportStartTime + i) % 24);
	}
	for (int i = 0; i < 23; i++) //24 hrs start counting from 0
	{
		if (inUpdatePosition == scheuduleMap[i])
		{
			return i;
		}
	}
	return -1;
}

CString CPerformanceReport::convertIntToMonth(INT inMonth)
{
	switch (inMonth)
	{
	case 1:
		return "Jan";
	case 2:
		return "Feb";
	case 3:
		return "Mar";
	case 4:
		return "Apr";
	case 5:
		return "May";
	case 6:
		return "Jun";
	case 7:
		return "Jul";
	case 8:
		return "Aug";
	case 9:
		return "Sep";
	case 10:
		return "Oct";
	case 11:
		return "Nov";
	case 12:
		return "Dec";
	}
	return "NA";
}

BOOL CPerformanceReport::IsFileExist(CString sPathName)
{
	HANDLE hFile;
	hFile = CreateFile(sPathName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) 
	{
		return false;
	}
	CloseHandle(hFile);
	return true;
}


BOOL CPerformanceReport::ShowPerformanceReport(INT inYear, INT inMonth, INT inDay)
{
	//check if file exists
	TCHAR currentDirectory[101];
	GetCurrentDirectory(100, currentDirectory);

	CString reportPath = "";
	reportPath.Format("D:\\\\sw\\\\AC9000\\\\Data\\\\%s\\\\%d\\\\%s\\\\%d_0.csv", MACHINE_PERFORMANCE_REPORT_FOLDER, inYear, convertIntToMonth(inMonth), inDay);
	if (IsFileExist(reportPath))
	{
		MessageBox(NULL, reportPath, "Testing", MB_OK);
		return true;
	}
	return false;
}


CString CPerformanceReport::LoadExistReport(CString inPath)
{
	CString result = "";
	CString tmpstr = "";
	CFile myFile;
	if (myFile.Open(inPath, CFile::modeRead))
	{
		myFile.SeekToBegin();
		CArchive arLoad(&myFile, CArchive::load);
		int counter = 0;

		while (arLoad.ReadString(tmpstr))
		{
			result += tmpstr + "\n";
			counter++;
		}
		m_nMrCounter = counter - 2;
		arLoad.Close();
		myFile.Close();
	}
	return result;
}


BOOL CPerformanceReport::CopyCurrentReportToHMI(LONG inPage)
{
	CFileException ex;

	CString result = "";
	CString tmpStr = "";
	CFile currentReport;

	if (currentReport.Open(m_szReportPath, CFile::modeRead | CFile::shareDenyNone))
	{
		currentReport.SeekToBegin();
		CArchive arLoad(&currentReport, CArchive::load);
		int counter = 0;

		while (arLoad.ReadString(tmpStr))
		{
			if (counter > 0)
			{
				result += tmpStr + "\n";
			}
			counter++;
		}
		arLoad.Close();
		currentReport.Close();
		
		CFile displayFile;
		if (displayFile.Open(MACHINE_PERFORMANCE_HMI_REPORT_CACHE, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone, &ex))
		{
			CArchive arDisplay(&displayFile, CArchive::store);
			
			arDisplay.WriteString(result);
			arDisplay.Flush();

			arDisplay.Close();
			displayFile.Close();
			return true;
		}
	}
	return false;
}

BOOL CPerformanceReport::ReadShiftScheduleConfig(CString szInPath)
{
	CFileException ex;

	CString result = "";
	CString tmpStr = "";
	CFile reportSettingConfigFile;
	LONG ulTmp = 0;
	try
	{
		if (reportSettingConfigFile.Open("D:\\sw\\AC9000\\Release\\performance_report_config.csv", CFile::modeRead | CFile::shareDenyNone))
		{
			reportSettingConfigFile.SeekToBegin();
			CArchive reportSettingConfigAr(&reportSettingConfigFile, CArchive::load);
			BOOL isReportStartTime = true;
			CString szCurrentShift = "";
			LONG ulCurrentShiftStart = 0;
			LONG ulCurrentShiftEnd = 0;
			int counter = 0;
			int nTeamShiftIndex = 0;
			int nShiftRange = 0;
			int nTmpIndex = 0;

			while (reportSettingConfigAr.ReadString(tmpStr))
			{
				tmpStr.Replace(",", "");
				if (counter == 0)
				{
					sscanf(tmpStr, "%ld", &m_lShiftResetTime);
				}
				else
				{
					m_szTeamShift[nTeamShiftIndex].Format("%s", tmpStr);
					nTeamShiftIndex++;
				}
				counter++;
			}
			return true;
		}
	} 
	catch (CFileException ex)
	{
		return false;
	}
	return true;
}