#pragma once
#include "afxmt.h"
#include "afxtempl.h"

#include "AC9000Stn.h"

extern BOOL	bEnableLogging;

typedef struct FileLock 
{
	FileLock() 
		: pLockFile(NULL), bNewFile(true) {}
	//FileLock& operator=(const FileLock& src) 
	//	{ pLockFile = src.pLockFile; bNewFile = src.bNewFile; return *this;}
	CCriticalSection *pLockFile;
	bool bNewFile;
} FileLock;

CString GetTime();
void LogAction(LPCTSTR fmt, ...);
void WriteToFile(LPCTSTR strFileName, LPCTSTR fmt, ...);
INT FreeAllFileLock(); //20150615

