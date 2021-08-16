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

#include "AC9000Stn.h"

#include "BasicExcel.h"

// must declara before using excelOperation
using namespace YExcel;
using namespace YCompoundFiles;

INT ExcelLoadGeneralSheet(BasicExcel &excelDB);
INT ExcelLoadINWHSheet(BasicExcel &excelDB);
INT ExcelLoadPBWHSheet(BasicExcel &excelDB);

INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, DOUBLE &dResult);
INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, LONG	&nResult);
INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, SHORT &sResult);
INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, INT &nResult);
INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, UINT &unResult);
INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, UCHAR *ucResult);
INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, CHAR *cResult);
INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, USHORT	&usResult);
INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, ULONG	&ulResult);
INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, CString &szResult);
