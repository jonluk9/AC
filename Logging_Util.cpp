#include "stdafx.h"


#include "Logging_Util.h"
#include "Cal_Util.h"

#define DATALOG_DIR ".\\Data"

BOOL	bEnableLogging	= TRUE;

static CString			g_strFileName = "D:\\sw\\AC9000\\Data\\AC9000Log.000";
static CCriticalSection g_CriticalSection;

#if 1 //20150615
typedef CMap<CString, LPCTSTR, FileLock, FileLock&> CFileMap;
static CFileMap gm_FileMap;
#else
static CMap<CString, LPCTSTR, FileLock, FileLock&> gm_FileMap;
#endif

CString GetTime()
{
	CString strTime;
	SYSTEMTIME ltime;		// New Command

	// Get time as 64-bit integer.
	GetLocalTime(&ltime);

	// Convert to local time.
	strTime.Format("%04u-%02u-%02u %02u:%02u:%02u\t", 
				   ltime.wYear, ltime.wMonth, ltime.wDay, 
				   ltime.wHour, ltime.wMinute, ltime.wSecond);

	return strTime;
}

void LogAction(LPCTSTR fmt, ...)
{
	CString msg;
	va_list args;
	BOOL	bIsSplitFile;

	va_start(args, fmt);
	msg.FormatV(fmt, args);
	va_end(args);

	// Lock
	g_CriticalSection.Lock();
	
	//Backup
	if ((bIsSplitFile = IsSplitFile(g_strFileName, 1000000)) == TRUE)
	{
		if (!BackupFile(g_strFileName))
		{
			//DisplayMessage("ERROR: ProductionDataLog Backup Data FAIL");
		}
	}
	
	//Write
	if (bEnableLogging)
	{
		WriteToFile(g_strFileName, msg);
	}

	//Unlock
	g_CriticalSection.Unlock();

}

void WriteToFile(LPCTSTR strFileName, LPCTSTR fmt, ...)
{
	CStdioFile	File;
	CFileFind	fileFind;
	va_list		args;
	CString		msg;

	FileLock &FileMap = gm_FileMap[strFileName];

	if (FileMap.pLockFile == NULL)
	{
		::CreateDirectory(DATALOG_DIR, NULL);
		FileMap.pLockFile = new CCriticalSection;
	}

	FileMap.pLockFile->Lock();

	if (fmt)
	{
		va_start(args, fmt);
		msg.FormatV(fmt, args);
		va_end(args);
	}
		
	//Find the file, if not exist,then create
	if (!fileFind.FindFile(strFileName))
	{
		if (File.Open(strFileName, CFile::modeCreate))
		{
			File.Close();
		}
	}

	if (File.Open(strFileName, CFile::modeWrite))
	{
		File.SeekToEnd();
		File.WriteString(GetTime() + msg + "\n");
		File.Close();
	}

	FileMap.pLockFile->Unlock();
}

INT FreeAllFileLock() //20150615
{
	INT nReturn = 0;
	CFileMap::CPair* pFileMap;

	UINT uTableSize = gm_FileMap.GetSize();

	pFileMap= gm_FileMap.PGetFirstAssoc( );
	while (pFileMap != NULL)
	{
		if (pFileMap->value.pLockFile != NULL)
		{
			nReturn++;
		}
		delete pFileMap->value.pLockFile;
		pFileMap= gm_FileMap.PGetNextAssoc(pFileMap);
	}
	return nReturn;
}
