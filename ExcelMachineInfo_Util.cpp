/////////////////////////////////////////////////////////////////
//	ExcelMachineInfo_Util.cpp
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


#include "stdafx.h"
#ifdef EXCEL_MACHINE_DEVICE_INFO
#include "BasicExcel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// must declara before using excelOperation
using namespace YExcel;
using namespace YCompoundFiles;

extern BasicExcelWorksheet	*g_pExcelMachineInfoSheet;
extern BasicExcelWorksheet	*g_pExcelDeviceInfoSheet;
extern LONG					g_lMachineInfoRowNum;
extern LONG					g_lDeviceInfoRowNum;

#include "ExcelMachineInfo_Util.h"

BOOL printMachineInfoRow(CString &szTitle1, CString &szTitle2, CString &szTitle3, CString &szTitle4, CString &szTitle5,
						 CString &szTitle6, CString &szTitle7, CString &szTitle8, CString &szTitle9, CString &szTitle10,
						 DOUBLE dValue)
{
	BOOL bResult = TRUE;
	LONG lCol	= 0;

	if (g_pExcelMachineInfoSheet == NULL)
	{
		return FALSE;
	}

	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle1);
	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle2);
	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle3);
	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle4);
	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle5);

	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle6);
	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle7);
	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle8);
	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle9);
	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle10);

	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetDouble(dValue);

	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString("DOUBLE");

	g_lMachineInfoRowNum++;
	return bResult;
}


BOOL printMachineInfoRow(CString &szTitle1, CString &szTitle2, CString &szTitle3, CString &szTitle4, CString &szTitle5,
						 CString &szTitle6, CString &szTitle7, CString &szTitle8, CString &szTitle9, CString &szTitle10,
						 INT nValue)
{
	BOOL bResult = TRUE;
	LONG lCol	= 0;

	if (g_pExcelMachineInfoSheet == NULL)
	{
		return FALSE;
	}

	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle1);
	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle2);
	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle3);
	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle4);
	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle5);

	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle6);
	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle7);
	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle8);
	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle9);
	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString(szTitle10);

	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetInteger(nValue);

	g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, lCol++)->SetString("INTEGER");

	g_lMachineInfoRowNum++;
	return bResult;
}

BOOL printDeviceInfoRow(CString &szTitle1, CString &szTitle2, CString &szTitle3, CString &szTitle4, CString &szTitle5,
						CString &szTitle6, CString &szTitle7, CString &szTitle8, CString &szTitle9, CString &szTitle10,
						DOUBLE dValue)
{
	BOOL bResult = TRUE;
	LONG lCol	= 0;

	if (g_pExcelDeviceInfoSheet == NULL)
	{
		return FALSE;
	}

	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle1);
	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle2);
	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle3);
	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle4);
	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle5);

	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle6);
	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle7);
	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle8);
	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle9);
	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle10);

	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetDouble(dValue);

	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString("DOUBLE");

	g_lDeviceInfoRowNum++;
	return bResult;
}


BOOL printDeviceInfoRow(CString &szTitle1, CString &szTitle2, CString &szTitle3, CString &szTitle4, CString &szTitle5,
						CString &szTitle6, CString &szTitle7, CString &szTitle8, CString &szTitle9, CString &szTitle10,
						INT nValue)
{
	BOOL bResult = TRUE;
	LONG lCol	= 0;

	if (g_pExcelDeviceInfoSheet == NULL)
	{
		return FALSE;
	}

	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle1);
	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle2);
	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle3);
	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle4);
	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle5);

	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle6);
	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle7);
	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle8);
	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle9);
	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString(szTitle10);

	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetInteger(nValue);

	g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, lCol++)->SetString("INTEGER");

	g_lDeviceInfoRowNum++;
	return bResult;
}

#endif
