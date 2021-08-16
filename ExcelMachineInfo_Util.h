/////////////////////////////////////////////////////////////////
//	ExcelMOTOR_INFO.cpp : interface of the ExcelMOTOR_INFO
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

#ifndef EXCELMACHINEINFO_UTIL_HEADER
#define EXCELMACHINEINFO_UTIL_HEADER
#include "AC9000Stn.h"

BOOL printMachineInfoRow(CString &szTitle1, CString &szTitle2, CString &szTitle3, CString &szTitle4, CString &szTitle5,
						  CString &szTitle6, CString &szTitle7, CString &szTitle8, CString &szTitle9, CString &szTitle10,
						  DOUBLE dValue);
BOOL printMachineInfoRow(CString &szTitle1, CString &szTitle2, CString &szTitle3, CString &szTitle4, CString &szTitle5,
						  CString &szTitle6, CString &szTitle7, CString &szTitle8, CString &szTitle9, CString &szTitle10,
						  INT nValue);

BOOL printDeviceInfoRow(CString &szTitle1, CString &szTitle2, CString &szTitle3, CString &szTitle4, CString &szTitle5,
						  CString &szTitle6, CString &szTitle7, CString &szTitle8, CString &szTitle9, CString &szTitle10,
						  DOUBLE dValue);
BOOL printDeviceInfoRow(CString &szTitle1, CString &szTitle2, CString &szTitle3, CString &szTitle4, CString &szTitle5,
						  CString &szTitle6, CString &szTitle7, CString &szTitle8, CString &szTitle9, CString &szTitle10,
						  INT nValue);
#endif
