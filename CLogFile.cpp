#include "StdAfx.h"
#include <stdarg.h>
#include <stdio.h>
#include "CLogFile.h"

CLogFile::CLogFile(CString filename, unsigned int mode, CString prefix, int size)
{
	if (!prefix.IsEmpty())
	{
		// Multiple Object Access 
		// Must be Save Always
		mode = mode | SAVE_ALWAYS;
		this->prefix = prefix;
	}
	this->mode = ACCESS_MODE(mode);
	this->filename = filename;
	this->size = size;
	bLogEnable = true;

	if ((this->mode & NEW_ALWAYS) != 0)
	{
		// Crate New File
		try
		{ 
			CFile::Remove(filename); 
		} 
		catch (...) {}
	}

	if ((this->mode & SAVE_ALWAYS) == 0)
	{
		// Open File Until object destruct
		outfile.Open(filename, CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate);
		outfile.SeekToEnd();
	} 
	else
	{
		if (outfile.Open(filename, CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate))
		{
			this->filename = outfile.GetFilePath();			
			outfile.Close();
		}
	}
	LogHeader();
}

CLogFile::~CLogFile()
{
	LogFooter();
	if ((mode & SAVE_ALWAYS) == 0)
	{
		try
		{ 
			outfile.Close(); 
		} 
		catch (...) {}
	}
}

void CLogFile::SetLogEnable(bool bEna)
{
	bLogEnable = bEna;
}
void CLogFile::LogFooter()
{
	if ((mode & WRITE_TIME) == 0)
	{
		SYSTEMTIME SysTime;
		GetLocalTime(&SysTime);
		log("Log File Destructed![%2d:%02d:%02d:%03d]",
			SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds);
	}
	else
	{
		log("Log File Destructed!");
	}

}
void CLogFile::LogHeader()
{
	if ((mode & WRITE_TIME) == 0)
	{
		SYSTEMTIME SysTime;
		GetLocalTime(&SysTime);
		log("Log File Initialized![%2d:%02d:%02d:%03d]",
			SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds);
	}
	else
	{
		log("Log File Initialized!");
	}
}

void CLogFile::flush()
{
	if ((mode & SAVE_ALWAYS) == 0)
	{
		// Open File Until object destruct
		try
		{ 
			outfile.Flush(); 
		} 
		catch (...) {}
	}
}

void CLogFile::CreateBakLog()
{
	if (size > 0)
	{
		try 
		{
			if ((mode & SAVE_ALWAYS) != 0)
			{
				outfile.Open(filename, CFile::modeRead);
			}
			bool bExceedSize = (outfile.GetLength() > (size * 1024));	// size in KB
			if ((mode & SAVE_ALWAYS) != 0)
			{
				outfile.Close();
			}

			if (bExceedSize)
			{
				CString bakFilename = this->filename + ".bak";
				try 
				{ 
					CFile::Remove(bakFilename); 
				} 
				catch (...) {};
				if ((mode & SAVE_ALWAYS) == 0) 
				{
					outfile.Close();
				}
				CFile::Rename(filename, bakFilename);
				if ((mode & SAVE_ALWAYS) == 0) 
				{
					outfile.Open(filename, CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate);
				}
			}
		}
		catch (...) {}
	}
}

void CLogFile::log(const char *fmt, ...)
{
	if (!bLogEnable) 
	{
		return;
	}
	va_list arg_list;
	char temp[1024];
	va_start(arg_list, fmt);
	vsprintf(temp, fmt, arg_list);
	va_end(arg_list);
	CString szTemp = temp;
	log(szTemp);
}

void CLogFile::log(BYTE *message, int size)
{
	CString byteData(""), byteDataArray("");
	int j = 0;
	for (int i = 0; i < size; i++)
	{
		if (j == 0) 
		{
			byteDataArray.Format("Data:::");
		}
		byteData.Format("%02x", message[i]);
		byteDataArray.Append(byteData);
		if ((j == 50) || (i == size - 1))		// Display 50 bytes in each line
		{
			j = 0;
			log(byteDataArray);
		}
		else
		{
			j++;
		}
	}
}

void CLogFile::log(CString &message)
{
	CSingleLock lock(&m_Mutex);

	if (!bLogEnable) 
	{
		return;
	}

	if (lock.Lock(INFINITE))
	{
		CreateBakLog();

		try
		{
			if ((mode & SAVE_ALWAYS) != 0)
			{
				CFileException exception;
				if (!outfile.Open(filename, CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate, &exception))
				{
					return; 
				}				
				try 
				{
					outfile.SeekToEnd();
				} 
				catch (...) {}
			}
			CString logMessage;

			if ((mode & WRITE_TIME) != 0)
			{
				SYSTEMTIME SysTime;
				GetLocalTime(&SysTime);
				logMessage.Format("%2d:%02d:%02d:%03d\t", 
								  SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds);
			}
			else
			{
				logMessage.Format("");
			}

			if (!prefix.IsEmpty())
			{
				logMessage.Append(prefix + "\t");
			}

			logMessage.Append(message + "\r\n");

			outfile.Write(logMessage, logMessage.GetLength());
			if ((mode & SAVE_ALWAYS) != 0)
			{
				outfile.Close();
			}
		} 
		catch (...) {}
		lock.Unlock();
	}
}

void CLogFile::log(CString szTitle[])
{
	CString	szPrintTitle;
	CSingleLock lock(&m_Mutex);

	if (!bLogEnable) 
	{
		return;
	}

	if (lock.Lock(INFINITE))
	{
		CreateBakLog();

		try
		{
			if ((mode & SAVE_ALWAYS) != 0)
			{
				CFileException exception;
				if (!outfile.Open(filename, CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate, &exception))
				{
					return; 
				}				
				try 
				{
					outfile.SeekToEnd();
				} 
				catch (...) {}
			}
			CString logMessage;

			//if ((mode & WRITE_TIME) != 0)
			//{
			//	SYSTEMTIME SysTime;
			//	GetLocalTime(&SysTime);
			//	logMessage.Format("%2d:%02d:%02d:%03d\t", 
			//		SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds);
			//}
			//else
			//	logMessage.Format("");

			if (!prefix.IsEmpty())
			{
				logMessage.Append(prefix + "\t");
			}

			for (INT i = 0; i < MAX_PRINT_DATA - 1; i++)
			{
				//if (szTitle[i] == "")
				//{
				//	break;
				//}

				szPrintTitle += szTitle[i] + _T("\t");
			}

			logMessage.Append(szPrintTitle + "\r\n");

			outfile.Write(logMessage, logMessage.GetLength());
			if ((mode & SAVE_ALWAYS) != 0)
			{
				outfile.Close();
			}
		} 
		catch (...) {}
		lock.Unlock();
	}
}
