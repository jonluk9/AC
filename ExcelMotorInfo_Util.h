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

BOOL	LoadMotorInfo(BasicExcelWorksheet excelSheet, CMotorInfo *stMotor);
VOID	GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, DOUBLE	*dResult);
VOID	GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, LONG	*lResult);
VOID	GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, SHORT	*sResult);
VOID	GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, INT		*nResult);
VOID	GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, UINT	*unResult);
VOID	GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, UCHAR	*uchResult);
VOID	GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, CHAR	*cResult);
VOID	GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, ULONG	*ulResult);
VOID	GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, USHORT	*usResult);

VOID	GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, GMP_COMMUTATION_OPTION	*enResult);
VOID	GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, GMP_EVT_TYPE	*enResult);
VOID	GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, GMP_EVT_LMT_TYPE	*enResult);
//VOID	GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, DWORD	*dwordResult);

VOID	GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, CString	*szResult);


