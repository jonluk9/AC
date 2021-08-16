#pragma once
#include "afxmt.h"
#define MAX_PRINT_DATA 200

class CLogFile: public CObject
{
public:
	enum ACCESS_MODE {START_MODE = 0, NEW_ALWAYS = 1, SAVE_ALWAYS = 2, WRITE_TIME = 4};
	CLogFile(CString filename, unsigned int mode, CString prefix = "", int size = 500/*in KB*/);
	~CLogFile();
	bool bLogEnable;
	void SetLogEnable(bool bEna);
	void flush();
	void log(char const*, ...);
	void log(CString &message);
	void log(BYTE *message, int size);
	void log(CString szTitle[]);

protected:
	int size;
	CMutex m_Mutex;
	static CMutex m_ClassMutex;
	CString filename;
	ACCESS_MODE mode;
	CFile outfile;
	CString prefix;
	void LogHeader();
	void LogFooter();
	void CreateBakLog();
};
