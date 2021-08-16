/////////////////////////////////////////////////////////////////
//	ExcelMOTORINFO.cpp : interface of the ExcelMOTOR_INFO
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
#include "ExcelMOTORINFO_Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL LoadMotorInfo(BasicExcelWorksheet excelSheet, CMotorInfo *stMotor)
{
	UINT curRow;
	UINT curCol;
	UINT totalRow;
	UINT totalCol;
	CString szExcelData;
	LONG lIsExist;
	BOOL bFound = FALSE;

	totalRow = excelSheet.GetTotalRows();
	totalCol = excelSheet.GetTotalCols();
	

	//Found the corresponsing Col.
	for (curCol = 0; curCol < totalCol; curCol++)
	{
		GetCell(excelSheet, 0, curCol, &szExcelData);
		LONG lLength = szExcelData.GetLength();
		if (szExcelData.GetLength() == 0)
		{
			lLength = 1;
		}
		CString szMsg = stMotor->stAttrib.szAxis.Right(lLength);
		
		if (stMotor->stAttrib.szAxis.Right(lLength) == szExcelData)
		{
			bFound = TRUE;
			break;
		}
		else if (curCol == totalCol - 1) // cann't find the tagert after all valid Col return FALSE
		{
			bFound = FALSE;
			return FALSE;
		}
	}
	
	if (bFound) //Double Check the Data
	{
		GetCell(excelSheet, 1, curCol, &lIsExist);
		if (lIsExist == 0)
		{
			bFound = FALSE;
			return FALSE;
		}

		GetCell(excelSheet, 2, curCol, &szExcelData);
		if (szExcelData != stMotor->stAttrib.szAxis)
		{
			bFound = FALSE;
			return FALSE;
		}
	}

	if (bFound)
	{
		curRow = 2;
		GetCell(excelSheet, curRow++, curCol, &stMotor->stAttrib.szAxis);
#if 1 //20121127
		GetCell(excelSheet, curRow++, curCol, &stMotor->stAttrib.dEncoderScale);
#else
		GetCell(excelSheet, curRow++, curCol, &stMotor->stAttrib.dDistPerCount);
#endif
		GetCell(excelSheet, curRow++, curCol, &stMotor->stAttrib.dDistPerCount);
		GetCell(excelSheet, curRow++, curCol, &stMotor->lErrorCode);
		GetCell(excelSheet, curRow++, curCol, &stMotor->nTCObjectID);

		GetCell(excelSheet, curRow++, curCol, stMotor->stAttrib.stSensorMapping.cHomePortAddrss);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stAttrib.stSensorMapping.nHomeMask);
		GetCell(excelSheet, curRow++, curCol, stMotor->stAttrib.stSensorMapping.cNegLmtPortAddrss);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stAttrib.stSensorMapping.nNegLmtMask);
		GetCell(excelSheet, curRow++, curCol, stMotor->stAttrib.stSensorMapping.cPosLmtPortAddrss);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stAttrib.stSensorMapping.nPosLmtMask);

		GetCell(excelSheet, curRow++, curCol, &stMotor->stAttrib.sPositionLimit);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stAttrib.sDacLimit);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stAttrib.sTimeLimit);

		GetCell(excelSheet, curRow++, curCol, &stMotor->stAttrib.bLmtSnrState);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stAttrib.bEncoderExist);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stAttrib.bLmtSnrExist);

		GetCell(excelSheet, curRow++, curCol, &stMotor->stComm.enCommOption);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stComm.sNumberOfPoles);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stComm.ulCountPerRev);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stComm.bCommDir);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stComm.unSlowDownFactor);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stComm.dAcceptance);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stComm.dInitialDAC);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stComm.dIncreamentDAC);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stComm.dMaximumDAC);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stComm.dFitError);

		GetCell(excelSheet, curRow++, curCol, &stMotor->stMotionProfile.sMotionProfID);
		GetCell(excelSheet, curRow++, curCol, stMotor->stMotionProfile.cMotionCtrlType);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stMotionProfile.dVel);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stMotionProfile.dAcc);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stMotionProfile.dDec);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stMotionProfile.dJerk);

		GetCell(excelSheet, curRow++, curCol, stMotor->stCtrlProf.cSettlingCtrlType);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stCtrlProf.sSettliingTime);

		GetCell(excelSheet, curRow++, curCol, &stMotor->stHomeProfile.bEnable);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stHomeProfile.bCheckError);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stHomeProfile.sSrchProfID);
		GetCell(excelSheet, curRow++, curCol, stMotor->stHomeProfile.cSrchCtrlType);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stHomeProfile.sSrchDirection);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stHomeProfile.bSetPosn);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stHomeProfile.lDesiredPosn);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stHomeProfile.enSrchType);
		GetCell(excelSheet, curRow++, curCol, stMotor->stHomeProfile.cPort);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stHomeProfile.unSensorMask);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stHomeProfile.bAstate);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stHomeProfile.enLimitType);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stHomeProfile.stProf.dSrchVel);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stHomeProfile.stProf.dSrchLimit);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stHomeProfile.stProf.dDriveInVel);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stHomeProfile.stProf.dDriveIn);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stHomeProfile.stProf.sDebounce);

		GetCell(excelSheet, curRow++, curCol, &stMotor->stIndexProfile.bEnable);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stIndexProfile.sSrchProfID);
		GetCell(excelSheet, curRow++, curCol, stMotor->stIndexProfile.cSrchCtrlType);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stIndexProfile.sSrchDirection);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stIndexProfile.bSetPosn);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stIndexProfile.lDesiredPosn);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stIndexProfile.enSrchType);
		GetCell(excelSheet, curRow++, curCol, stMotor->stIndexProfile.cPort);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stIndexProfile.unSensorMask);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stIndexProfile.bAstate);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stIndexProfile.enLimitType);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stIndexProfile.stProf.dSrchVel);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stIndexProfile.stProf.dSrchLimit);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stIndexProfile.stProf.dDriveInVel);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stIndexProfile.stProf.dDriveIn);
		GetCell(excelSheet, curRow++, curCol, &stMotor->stIndexProfile.stProf.sDebounce);

		GetCell(excelSheet, curRow++, curCol, &stMotor->stAttrib.dDistPerCountAux); //20140322
	}
	return TRUE;
}


VOID GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, DOUBLE	*dResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		*dResult = 0.0;
		break;

	case BasicExcelCell::INT:
		*dResult = (DOUBLE)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		*dResult = excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		*dResult = 0.0;
		break;

	case BasicExcelCell::WSTRING:
		*dResult = 0.0;
		break;
	}
}

VOID GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, LONG	*nResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		*nResult = 0;
		break;

	case BasicExcelCell::INT:
		*nResult = (LONG)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		*nResult = (LONG)excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		*nResult = 0;
		break;

	case BasicExcelCell::WSTRING:
		*nResult = 0;
		break;
	}
}


VOID GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, SHORT	*sResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		*sResult = 0;
		break;

	case BasicExcelCell::INT:
		*sResult = (SHORT)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		*sResult = (SHORT)excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		*sResult = 0;
		break;

	case BasicExcelCell::WSTRING:
		*sResult = 0;
		break;
	}
}

VOID GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, INT	*nResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		*nResult = 0;
		break;

	case BasicExcelCell::INT:
		*nResult = (INT)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		*nResult = (INT)excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		*nResult = 0;
		break;

	case BasicExcelCell::WSTRING:
		*nResult = 0;
		break;
	}
}

VOID GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, UINT	*unResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		*unResult = 0;
		break;

	case BasicExcelCell::INT:
		*unResult = (INT)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		*unResult = (INT)excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		*unResult = 0;
		break;

	case BasicExcelCell::WSTRING:
		*unResult = 0;
		break;
	}
}

VOID GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, UCHAR *ucResult)
{
	CString szTemp;
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		szTemp = "0";
		strcpy((char*)ucResult, szTemp);
		*ucResult -= 48;
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
	}
}

VOID GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, CHAR *cResult)
{
	CString szTemp;
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		szTemp = "0";
		strcpy((char*)cResult, szTemp);
		*cResult -= 48;
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
	}
}

VOID GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, USHORT	*usResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		*usResult = 0;
		break;

	case BasicExcelCell::INT:
		*usResult = (USHORT)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		*usResult = (USHORT)excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		*usResult = 0;
		break;

	case BasicExcelCell::WSTRING:
		*usResult = 0;
		break;
	}
}

VOID GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, ULONG	*ulResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		*ulResult = 0;
		break;

	case BasicExcelCell::INT:
		*ulResult = (ULONG)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		*ulResult = (ULONG)excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		*ulResult = 0;
		break;

	case BasicExcelCell::WSTRING:
		*ulResult = 0;
		break;
	}
}

VOID GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, CString	*szResult)
{
	
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		szResult->Format("");
		break;

	case BasicExcelCell::INT:
		szResult->Format("");
		break;

	case BasicExcelCell::DOUBLE:
		szResult->Format("");
		break;

	case BasicExcelCell::STRING:
		szResult->Format("%s", excelSheet.Cell(row, col)->GetString());
		break;

	case BasicExcelCell::WSTRING:
		szResult->Format("%s", excelSheet.Cell(row, col)->GetWString());
		break;
	}
}

VOID GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, GMP_COMMUTATION_OPTION	*enResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		*enResult = (GMP_COMMUTATION_OPTION)0;
		break;

	case BasicExcelCell::INT:
		*enResult = (GMP_COMMUTATION_OPTION)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		*enResult = (GMP_COMMUTATION_OPTION)(LONG)excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		*enResult = (GMP_COMMUTATION_OPTION)0;
		break;

	case BasicExcelCell::WSTRING:
		*enResult = (GMP_COMMUTATION_OPTION)0;
		break;
	}
}

VOID GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, GMP_EVT_TYPE	*enResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		*enResult = (GMP_EVT_TYPE)0;
		break;

	case BasicExcelCell::INT:
		*enResult = (GMP_EVT_TYPE)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		*enResult = (GMP_EVT_TYPE)(LONG)excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		*enResult = (GMP_EVT_TYPE)0;
		break;

	case BasicExcelCell::WSTRING:
		*enResult = (GMP_EVT_TYPE)0;
		break;
	}
}

VOID GetCell(BasicExcelWorksheet excelSheet, UINT row, UINT col, GMP_EVT_LMT_TYPE	*enResult)
{
	switch (excelSheet.Cell(row, col)->Type())
	{
	case BasicExcelCell::UNDEFINED:
		*enResult = (GMP_EVT_LMT_TYPE)0;
		break;

	case BasicExcelCell::INT:
		*enResult = (GMP_EVT_LMT_TYPE)excelSheet.Cell(row, col)->GetInteger();
		break;

	case BasicExcelCell::DOUBLE:
		*enResult = (GMP_EVT_LMT_TYPE)(LONG)excelSheet.Cell(row, col)->GetDouble();
		break;

	case BasicExcelCell::STRING:
		*enResult = (GMP_EVT_LMT_TYPE)0;
		break;

	case BasicExcelCell::WSTRING:
		*enResult = (GMP_EVT_LMT_TYPE)0;
		break;
	}
}
