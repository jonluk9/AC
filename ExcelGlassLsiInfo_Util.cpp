/////////////////////////////////////////////////////////////////
//	ExcelGlassLsiINFO_Util.cpp : interface of the ExcelGlassLsiINFO
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
#include "ExcelGlassLsiInfo_Util.h"
#include "ExcelMOTORINFO_Util.h"
#include "AC9000.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern EXCEL_OFFLINE_INFO		g_stOfflineInfo; //20150812
extern CAC9000App theApp;
extern OFFLINE_INPUT_DATA gOfflineInputData[OFFLINE_MAX];

INT ExcelLoadGeneralSheet(BasicExcel &excelDB)
{
	BasicExcelWorksheet	*pCurSheet = NULL;
	CString szCol0Label(_T(""));
	//CString szMsg(_T(""));
	UINT totalRow = 0, curRow = 0;
	UINT totalCol = 0, curCol = 0;
	BOOL bError = FALSE;

	pCurSheet = excelDB.GetWorksheet("General");
	if (pCurSheet != NULL)
	{
		totalRow = pCurSheet->GetTotalRows();
		totalCol = pCurSheet->GetTotalCols();
		if (totalCol > 0 && totalRow > 0)
		{
			for (curRow =0; curRow < totalRow; curRow++)
			{
				szCol0Label = _T("");
				ExGetCell(*pCurSheet, curRow, 0, szCol0Label);

				if (szCol0Label == "")
				{
					; //skip blink line
				}

				for (INT ii = OFFLINE_GLASS_WIDTH; ii <= OFFLINE_ACF_INSP_REJECT_LMT; ii++) //all item is within OFFLINE_GLASS_WIDTH to OFFLINE_TA4_REJECT_LMT
				{
					if (szCol0Label == CString(gOfflineInputData[ii].pExcelName))
					{
						if (ExGetCell(*pCurSheet, curRow, 1, *gOfflineInputData[ii].pdValue) != gnAMS_OK)
						{
							CString szMsg("");
							szMsg.Format("Load Genaral Info: %s Error", gOfflineInputData[ii].pName);
							theApp.DisplayMessage(szMsg);
							bError = TRUE;
						}
					}
				}

			}
		}
	}
	else if (pCurSheet == NULL)
	{
		theApp.DisplayMessage("Load General Sheet Error");
		bError = TRUE;
	}

	if (bError)
	{
		return gnAMS_NOTOK;
	}
	else
	{
		return gnAMS_OK;
	}
}

INT ExcelLoadINWHSheet(BasicExcel &excelDB)
{
	BasicExcelWorksheet	*pCurSheet = NULL;
	CString szCol0Label(_T(""));
	//CString szMsg(_T(""));
	UINT totalRow = 0, curRow = 0;
	UINT totalCol = 0, curCol = 0;
	BOOL bError = FALSE;

	//g_stOfflineInfo.stINWHInfo.dXDir = -1.0;
	//g_stOfflineInfo.stINWHInfo.dYDir = -1.0;

	pCurSheet = excelDB.GetWorksheet("INWH");
	if (pCurSheet != NULL)
	{
		totalRow = pCurSheet->GetTotalRows();
		totalCol = pCurSheet->GetTotalCols();
		if (totalCol > 0 && totalRow > 0)
		{
			for (curRow =0; curRow < totalRow; curRow++)
			{
				szCol0Label = _T("");
				ExGetCell(*pCurSheet, curRow, 0, szCol0Label);

				if (szCol0Label == "")
				{
					; //skip blink line
				}
				else if (szCol0Label == _T("PR1(um)"))
				{
					if (ExGetCell(*pCurSheet, curRow, 1, g_stOfflineInfo.stINWHInfo.dPR1PosnX) != gnAMS_OK ||
						ExGetCell(*pCurSheet, curRow, 2, g_stOfflineInfo.stINWHInfo.dPR1PosnY) != gnAMS_OK	)
					{
						theApp.DisplayMessage("Load INWH Info: PR1 Error");
						bError = TRUE;
					}
				}
				else if (szCol0Label == _T("PR2(um)"))
				{
					if (ExGetCell(*pCurSheet, curRow, 1, g_stOfflineInfo.stINWHInfo.dPR2PosnX) != gnAMS_OK ||
						ExGetCell(*pCurSheet, curRow, 2, g_stOfflineInfo.stINWHInfo.dPR2PosnY) != gnAMS_OK	)
					{
						theApp.DisplayMessage("Load INWH Info: PR2 Error");
						bError = TRUE;
					}
				}
				else if (szCol0Label == _T("AP1(um)"))
				{
					if (ExGetCell(*pCurSheet, curRow, 1, g_stOfflineInfo.stINWHInfo.dAP1PosnX) != gnAMS_OK ||
						ExGetCell(*pCurSheet, curRow, 2, g_stOfflineInfo.stINWHInfo.dAP1PosnY) != gnAMS_OK	)
					{
						theApp.DisplayMessage("Load INWH Info: AP1 Error");
						bError = TRUE;
					}
				}
				else if (szCol0Label == _T("AP2(um)"))
				{
					if (ExGetCell(*pCurSheet, curRow, 1, g_stOfflineInfo.stINWHInfo.dAP2PosnX) != gnAMS_OK ||
						ExGetCell(*pCurSheet, curRow, 2, g_stOfflineInfo.stINWHInfo.dAP2PosnY) != gnAMS_OK	)
					{
						theApp.DisplayMessage("Load INWH Info: AP2 Error");
						bError = TRUE;
					}
				}
				else
				{
					theApp.DisplayMessage("Load INWH Info: Unknown Row Error");
					bError = TRUE;
				}
			}
		}
	}
	else if (pCurSheet == NULL)
	{
		theApp.DisplayMessage("Load INWH Sheet Error");
		bError = TRUE;
	}

	if (bError)
	{
		return gnAMS_NOTOK;
	}
	else
	{
		return gnAMS_OK;
	}
}

INT ExcelLoadPBWHSheet(BasicExcel &excelDB)
{
	BasicExcelWorksheet	*pCurSheet = NULL;
	CString szCol0Label(_T(""));
	//CString szMsg(_T(""));
	UINT totalRow = 0, curRow = 0;
	UINT totalCol = 0, curCol = 0;
	BOOL bError = FALSE;

	//g_stOfflineInfo.stPBWHInfo.dXDir = -1.0;
	//g_stOfflineInfo.stPBWHInfo.dYDir = 1.0;

	pCurSheet = excelDB.GetWorksheet("PBWH");
	if (pCurSheet != NULL)
	{
		totalRow = pCurSheet->GetTotalRows();
		totalCol = pCurSheet->GetTotalCols();
		if (totalCol > 0 && totalRow > 0)
		{
			for (curRow =0; curRow < totalRow; curRow++)
			{
				szCol0Label = _T("");
				ExGetCell(*pCurSheet, curRow, 0, szCol0Label);

				if (szCol0Label == "")
				{
					; //skip blink line
				}
				else if (szCol0Label == _T("PR1(um)"))
				{
					if (
						ExGetCell(*pCurSheet, curRow, 1, g_stOfflineInfo.stPBWHInfo.dPR1PosnX) != gnAMS_OK ||
						ExGetCell(*pCurSheet, curRow, 2, g_stOfflineInfo.stPBWHInfo.dPR1PosnY) != gnAMS_OK	
						)
					{
						theApp.DisplayMessage("Load PBWH Info: PR1 Error");
						bError = TRUE;
					}
				}
				else if (szCol0Label == _T("PR2(um)"))
				{
					if (
						ExGetCell(*pCurSheet, curRow, 1, g_stOfflineInfo.stPBWHInfo.dPR2PosnX) != gnAMS_OK ||
						ExGetCell(*pCurSheet, curRow, 2, g_stOfflineInfo.stPBWHInfo.dPR2PosnY) != gnAMS_OK	
						)
					{
						theApp.DisplayMessage("Load PBWH Info: PR2 Error");
						bError = TRUE;
					}
				}
				else if (szCol0Label == _T("AP1(um)"))
				{
					if (
						ExGetCell(*pCurSheet, curRow, 1, g_stOfflineInfo.stPBWHInfo.dAP1PosnX) != gnAMS_OK ||
						ExGetCell(*pCurSheet, curRow, 2, g_stOfflineInfo.stPBWHInfo.dAP1PosnY) != gnAMS_OK	
						)
					{
						theApp.DisplayMessage("Load PBWH Info: AP1 Error");
						bError = TRUE;
					}
				}
				else if (szCol0Label == _T("AP2(um)"))
				{
					if (
						ExGetCell(*pCurSheet, curRow, 1, g_stOfflineInfo.stPBWHInfo.dAP2PosnX) != gnAMS_OK ||
						ExGetCell(*pCurSheet, curRow, 2, g_stOfflineInfo.stPBWHInfo.dAP2PosnY) != gnAMS_OK	
						)
					{
						theApp.DisplayMessage("Load PBWH Info: AP2 Error");
						bError = TRUE;
					}
				}
				else
				{
					theApp.DisplayMessage("Load PBWH Info: Unknown Row Error");
					bError = TRUE;
				}
			}
		}
	}
	else if (pCurSheet == NULL)
	{
		theApp.DisplayMessage("Load PBWH Sheet Error");
		bError = TRUE;
	}

	if (bError)
	{
		return gnAMS_NOTOK;
	}
	else
	{
		return gnAMS_OK;
	}
}

INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, DOUBLE &dResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		dResult = 0.0;
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::INT:
		dResult = (DOUBLE)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		dResult = excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		dResult = 0.0;
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::WSTRING:
		dResult = 0.0;
		return gnAMS_NOTOK;
		break;

	default:
		return gnAMS_NOTOK;
		break;
	}
	return gnAMS_OK;
}

INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, LONG	&nResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		nResult = 0;
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::INT:
		nResult = (LONG)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		nResult = (LONG)excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		nResult = 0;
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::WSTRING:
		nResult = 0;
		return gnAMS_NOTOK;
		break;

	default:
		return gnAMS_NOTOK;
		break;
	}
	return gnAMS_OK;
}


INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, SHORT &sResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		sResult = 0;
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::INT:
		sResult = (SHORT)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		sResult = (SHORT)excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		sResult = 0;
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::WSTRING:
		sResult = 0;
		return gnAMS_NOTOK;
		break;

	default:
		return gnAMS_NOTOK;
		break;
	}
	return gnAMS_OK;
}

INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, INT &nResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		nResult = 0;
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::INT:
		nResult = (INT)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		nResult = (INT)excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		nResult = 0;
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::WSTRING:
		nResult = 0;
		return gnAMS_NOTOK;
		break;

	default:
		return gnAMS_NOTOK;
		break;
	}
	return gnAMS_OK;
}

INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, UINT &unResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		unResult = 0;
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::INT:
		unResult = (INT)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		unResult = (INT)excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		unResult = 0;
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::WSTRING:
		unResult = 0;
		return gnAMS_NOTOK;
		break;

	default:
		unResult = 0;
		return gnAMS_NOTOK;
		break;
	}
	return gnAMS_OK;
}

INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, UCHAR *ucResult)
{
	CString szTemp(_T(""));

	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		szTemp = "0";
		strcpy((char*)ucResult, szTemp);
		*ucResult -= 48;
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::INT:
		szTemp.Format("%d", excelSheet.Cell(row, col)->GetInteger());
		strcpy((char*)ucResult, szTemp);
		*ucResult -= 48;
		break;

	case BasicExcelCell::DOUBLE:
		szTemp.Format("%.f", excelSheet.Cell(row, col)->GetDouble());
		strcpy((char*)ucResult, szTemp);
		*ucResult -= 48;
		break;

	case BasicExcelCell::STRING:
		szTemp = excelSheet.Cell(row, col)->GetString();
		strcpy((char*)ucResult, szTemp);
		*ucResult -= 48;
		break;

	case BasicExcelCell::WSTRING:
		szTemp = excelSheet.Cell(row, col)->GetWString();
		strcpy((char*)ucResult, szTemp);
		*ucResult -= 48;
		break;

	default:
		szTemp = "0";
		strcpy((char*)ucResult, szTemp);
		*ucResult -= 48;
		return gnAMS_NOTOK;
		break;
	}
	return gnAMS_OK;
}

INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, CHAR *cResult)
{
	CString szTemp;
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		szTemp = "0";
		strcpy((char*)cResult, szTemp);
		*cResult -= 48;
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::INT:
		szTemp.Format("%d", excelSheet.Cell(row, col)->GetInteger());
		strcpy((char*)cResult, szTemp);
		*cResult -= 48;
		break;

	case BasicExcelCell::DOUBLE:
		szTemp.Format("%.f", excelSheet.Cell(row, col)->GetDouble());
		strcpy((char*)cResult, szTemp);
		*cResult -= 48;
		break;

	case BasicExcelCell::STRING:
		szTemp = excelSheet.Cell(row, col)->GetString();
		strcpy(cResult, szTemp);
		break;

	case BasicExcelCell::WSTRING:
		szTemp = excelSheet.Cell(row, col)->GetWString();
		strcpy((char*)cResult, szTemp);
		*cResult -= 48;
		break;

	default:
		szTemp = "0";
		strcpy((char*)cResult, szTemp);
		*cResult -= 48;
		return gnAMS_NOTOK;
		break;
	}
	return gnAMS_OK;
}

INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, USHORT	&usResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		usResult = 0;
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::INT:
		usResult = (USHORT)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		usResult = (USHORT)excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		usResult = 0;
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::WSTRING:
		usResult = 0;
		return gnAMS_NOTOK;
		break;
	}
	return gnAMS_OK;
}

INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, ULONG	&ulResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		ulResult = 0;
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::INT:
		ulResult = (ULONG)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		ulResult = (ULONG)excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		ulResult = 0;
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::WSTRING:
		ulResult = 0;
		return gnAMS_NOTOK;
		break;
	}
	return gnAMS_OK;
}

INT ExGetCell(BasicExcelWorksheet &excelSheet, const UINT row, const UINT col, CString &szResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		szResult.Format("");
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::INT:
		szResult.Format("");
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::DOUBLE:
		szResult.Format("");
		return gnAMS_NOTOK;
		break;

	case BasicExcelCell::STRING:
		szResult.Format("%s", excelSheet.Cell(row, col)->GetString());
		break;

	case BasicExcelCell::WSTRING:
		szResult.Format("%s", excelSheet.Cell(row, col)->GetWString());
		break;

	default:
		szResult.Format("");
		return gnAMS_NOTOK;
		break;
	}
	return gnAMS_OK;
}
