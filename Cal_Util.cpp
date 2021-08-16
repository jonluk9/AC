/////////////////////////////////////////////////////////////////
//	Cal_Util.cpp : interface of the Cal_Util
//
//	Description:
//		
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
#include "math.h"
#include "prheader.h"

#include "Cal_Util.h"

#include "FileOperations.h"

#include "AC9000.h"

#define NUM_OF_BACKUP 5

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LONG DoubleToLong(DOUBLE dInput)
{
	LONG lOutput;

	if (dInput < 0)
	{
		lOutput = (LONG)(dInput - 0.5);
	}
	else
	{
		lOutput = (LONG)(dInput + 0.5);
	}

	return lOutput;
}

DOUBLE SetDecimalPlace(DOUBLE dInput)
{
	LONG	lTempOutput;
	LONG	lTest;
	DOUBLE dOutput;

	dInput *= 1000.0;
	lTempOutput = (LONG)dInput;
	lTest = lTempOutput;
	dOutput = (DOUBLE)lTempOutput / 1000.0;


	return dOutput;
}

BOOL FindManualCalibMatrix(PRU *pPRU, MTR_POSN MtrPts[], PR_COORD PRPts[])
{
	BOOL bResult = TRUE;
	//if (pPRU->bUseFirstABCD)
	if (pPRU->eCamMoveGroup1 == pPRU->eCamMoveGroupCurrent)
	{
		// calculate p2m transform (0 = Left, 1 = Right, 2 = Up, 3 = Down)
		pPRU->p2m_xform.a = -(FLOAT)(MtrPts[1].x - MtrPts[0].x) / (FLOAT)(PRPts[1].x - PRPts[0].x);
		pPRU->p2m_xform.b = -(FLOAT)(MtrPts[3].x - MtrPts[2].x) / (FLOAT)(PRPts[3].y - PRPts[2].y);
		pPRU->p2m_xform.c = -(FLOAT)(MtrPts[1].y - MtrPts[0].y) / (FLOAT)(PRPts[1].x - PRPts[0].x);
		pPRU->p2m_xform.d = -(FLOAT)(MtrPts[3].y - MtrPts[2].y) / (FLOAT)(PRPts[3].y - PRPts[2].y);

		// calculate m2p transform
		FLOAT fDet;
		fDet = (FLOAT)((-1.0) * (pPRU->p2m_xform.b) * (pPRU->p2m_xform.c) + (pPRU->p2m_xform.a) * (pPRU->p2m_xform.d));

		if (fDet == 0.0)
		{
			pPRU->bCalibrated = FALSE;
			return FALSE;
		}

		pPRU->m2p_xform.a = (FLOAT)pPRU->p2m_xform.d / fDet;
		pPRU->m2p_xform.b = -(FLOAT)pPRU->p2m_xform.b / fDet;
		pPRU->m2p_xform.c = -(FLOAT)pPRU->p2m_xform.c / fDet;
		pPRU->m2p_xform.d = (FLOAT)pPRU->p2m_xform.a / fDet;

		pPRU->bCalibrated = TRUE;
	}
	else if (pPRU->eCamMoveGroup2 == pPRU->eCamMoveGroupCurrent)
	{
		// calculate p2m transform (0 = Left, 1 = Right, 2 = Up, 3 = Down)
		pPRU->p2m_xform_2.a = -(FLOAT)(MtrPts[1].x - MtrPts[0].x) / (FLOAT)(PRPts[1].x - PRPts[0].x);
		pPRU->p2m_xform_2.b = -(FLOAT)(MtrPts[3].x - MtrPts[2].x) / (FLOAT)(PRPts[3].y - PRPts[2].y);
		pPRU->p2m_xform_2.c = -(FLOAT)(MtrPts[1].y - MtrPts[0].y) / (FLOAT)(PRPts[1].x - PRPts[0].x);
		pPRU->p2m_xform_2.d = -(FLOAT)(MtrPts[3].y - MtrPts[2].y) / (FLOAT)(PRPts[3].y - PRPts[2].y);

		// calculate m2p transform
		FLOAT fDet;
		fDet = (FLOAT)((-1.0) * (pPRU->p2m_xform_2.b) * (pPRU->p2m_xform_2.c) + (pPRU->p2m_xform_2.a) * (pPRU->p2m_xform_2.d));

		if (fDet == 0.0)
		{
			pPRU->bCalibrated_2 = FALSE;
			return FALSE;
		}

		pPRU->m2p_xform_2.a = (FLOAT)pPRU->p2m_xform_2.d / fDet;
		pPRU->m2p_xform_2.b = -(FLOAT)pPRU->p2m_xform_2.b / fDet;
		pPRU->m2p_xform_2.c = -(FLOAT)pPRU->p2m_xform_2.c / fDet;
		pPRU->m2p_xform_2.d = (FLOAT)pPRU->p2m_xform_2.a / fDet;

		pPRU->bCalibrated_2 = TRUE;
	}
	else if (pPRU->eCamMoveGroup3 == pPRU->eCamMoveGroupCurrent)
	{
		// calculate p2m transform (0 = Left, 1 = Right, 2 = Up, 3 = Down)
		pPRU->p2m_xform_3.a = -(FLOAT)(MtrPts[1].x - MtrPts[0].x) / (FLOAT)(PRPts[1].x - PRPts[0].x);
		pPRU->p2m_xform_3.b = -(FLOAT)(MtrPts[3].x - MtrPts[2].x) / (FLOAT)(PRPts[3].y - PRPts[2].y);
		pPRU->p2m_xform_3.c = -(FLOAT)(MtrPts[1].y - MtrPts[0].y) / (FLOAT)(PRPts[1].x - PRPts[0].x);
		pPRU->p2m_xform_3.d = -(FLOAT)(MtrPts[3].y - MtrPts[2].y) / (FLOAT)(PRPts[3].y - PRPts[2].y);

		// calculate m2p transform
		FLOAT fDet;
		fDet = (FLOAT)((-1.0) * (pPRU->p2m_xform_3.b) * (pPRU->p2m_xform_3.c) + (pPRU->p2m_xform_3.a) * (pPRU->p2m_xform_3.d));

		if (fDet == 0.0)
		{
			pPRU->bCalibrated_3 = FALSE;
			return FALSE;
		}

		pPRU->m2p_xform_3.a = (FLOAT)pPRU->p2m_xform_3.d / fDet;
		pPRU->m2p_xform_3.b = -(FLOAT)pPRU->p2m_xform_3.b / fDet;
		pPRU->m2p_xform_3.c = -(FLOAT)pPRU->p2m_xform_3.c / fDet;
		pPRU->m2p_xform_3.d = (FLOAT)pPRU->p2m_xform_3.a / fDet;

		pPRU->bCalibrated_3 = TRUE;
	}
	else if (pPRU->eCamMoveGroup4 == pPRU->eCamMoveGroupCurrent)
	{
		// calculate p2m transform (0 = Left, 1 = Right, 2 = Up, 3 = Down)
		pPRU->p2m_xform_4.a = -(FLOAT)(MtrPts[1].x - MtrPts[0].x) / (FLOAT)(PRPts[1].x - PRPts[0].x);
		pPRU->p2m_xform_4.b = -(FLOAT)(MtrPts[3].x - MtrPts[2].x) / (FLOAT)(PRPts[3].y - PRPts[2].y);
		pPRU->p2m_xform_4.c = -(FLOAT)(MtrPts[1].y - MtrPts[0].y) / (FLOAT)(PRPts[1].x - PRPts[0].x);
		pPRU->p2m_xform_4.d = -(FLOAT)(MtrPts[3].y - MtrPts[2].y) / (FLOAT)(PRPts[3].y - PRPts[2].y);

		// calculate m2p transform
		FLOAT fDet;
		fDet = (FLOAT)((-1.0) * (pPRU->p2m_xform_4.b) * (pPRU->p2m_xform_4.c) + (pPRU->p2m_xform_4.a) * (pPRU->p2m_xform_4.d));

		if (fDet == 0.0)
		{
			pPRU->bCalibrated_4 = FALSE;
			return FALSE;
		}

		pPRU->m2p_xform_4.a = (FLOAT)pPRU->p2m_xform_4.d / fDet;
		pPRU->m2p_xform_4.b = -(FLOAT)pPRU->p2m_xform_4.b / fDet;
		pPRU->m2p_xform_4.c = -(FLOAT)pPRU->p2m_xform_4.c / fDet;
		pPRU->m2p_xform_4.d = (FLOAT)pPRU->p2m_xform_4.a / fDet;

		pPRU->bCalibrated_4 = TRUE;
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL FindAutoCalibMatrix(PRU *pPRU, MTR_POSN MtrPts[], PR_COORD PRPts[])
{
	BOOL bResult = TRUE;
	//if (pPRU->bUseFirstABCD)
	if (pPRU->eCamMoveGroup1 == pPRU->eCamMoveGroupCurrent)
	{
		// calculate p2m transform (0 = Left, 1 = Right, 2 = Up, 3 = Down)
		pPRU->m2p_xform.a = -(FLOAT)(PRPts[1].x - PRPts[0].x) / (FLOAT)(MtrPts[1].x - MtrPts[0].x);
		pPRU->m2p_xform.b = -(FLOAT)(PRPts[3].x - PRPts[2].x) / (FLOAT)(MtrPts[3].y - MtrPts[2].y);
		pPRU->m2p_xform.c = -(FLOAT)(PRPts[1].y - PRPts[0].y) / (FLOAT)(MtrPts[1].x - MtrPts[0].x);
		pPRU->m2p_xform.d = -(FLOAT)(PRPts[3].y - PRPts[2].y) / (FLOAT)(MtrPts[3].y - MtrPts[2].y);

		// calculate m2p transform
		FLOAT fDet;
		fDet = (FLOAT)((-1.0) * (pPRU->m2p_xform.b) * (pPRU->m2p_xform.c) + (pPRU->m2p_xform.a) * (pPRU->m2p_xform.d));

		if (fDet == 0.0)
		{
			pPRU->bCalibrated = FALSE;
			return FALSE;
		}

		pPRU->p2m_xform.a = (FLOAT)pPRU->m2p_xform.d / fDet;
		pPRU->p2m_xform.b = -(FLOAT)pPRU->m2p_xform.b / fDet;
		pPRU->p2m_xform.c = -(FLOAT)pPRU->m2p_xform.c / fDet;
		pPRU->p2m_xform.d = (FLOAT)pPRU->m2p_xform.a / fDet;

		pPRU->bCalibrated = TRUE;
	}
	else if (pPRU->eCamMoveGroup2 == pPRU->eCamMoveGroupCurrent)
	{
		// calculate p2m transform (0 = Left, 1 = Right, 2 = Up, 3 = Down)
		pPRU->m2p_xform_2.a = -(FLOAT)(PRPts[1].x - PRPts[0].x) / (FLOAT)(MtrPts[1].x - MtrPts[0].x);
		pPRU->m2p_xform_2.b = -(FLOAT)(PRPts[3].x - PRPts[2].x) / (FLOAT)(MtrPts[3].y - MtrPts[2].y);
		pPRU->m2p_xform_2.c = -(FLOAT)(PRPts[1].y - PRPts[0].y) / (FLOAT)(MtrPts[1].x - MtrPts[0].x);
		pPRU->m2p_xform_2.d = -(FLOAT)(PRPts[3].y - PRPts[2].y) / (FLOAT)(MtrPts[3].y - MtrPts[2].y);

		// calculate m2p transform
		FLOAT fDet;
		fDet = (FLOAT)((-1.0) * (pPRU->m2p_xform_2.b) * (pPRU->m2p_xform_2.c) + (pPRU->m2p_xform_2.a) * (pPRU->m2p_xform_2.d));

		if (fDet == 0.0)
		{
			pPRU->bCalibrated_2 = FALSE;
			return FALSE;
		}

		pPRU->p2m_xform_2.a = (FLOAT)pPRU->m2p_xform_2.d / fDet;
		pPRU->p2m_xform_2.b = -(FLOAT)pPRU->m2p_xform_2.b / fDet;
		pPRU->p2m_xform_2.c = -(FLOAT)pPRU->m2p_xform_2.c / fDet;
		pPRU->p2m_xform_2.d = (FLOAT)pPRU->m2p_xform_2.a / fDet;

		pPRU->bCalibrated_2 = TRUE;
	}
	else if (pPRU->eCamMoveGroup3 == pPRU->eCamMoveGroupCurrent)
	{
		// calculate p2m transform (0 = Left, 1 = Right, 2 = Up, 3 = Down)
		pPRU->m2p_xform_3.a = -(FLOAT)(PRPts[1].x - PRPts[0].x) / (FLOAT)(MtrPts[1].x - MtrPts[0].x);
		pPRU->m2p_xform_3.b = -(FLOAT)(PRPts[3].x - PRPts[2].x) / (FLOAT)(MtrPts[3].y - MtrPts[2].y);
		pPRU->m2p_xform_3.c = -(FLOAT)(PRPts[1].y - PRPts[0].y) / (FLOAT)(MtrPts[1].x - MtrPts[0].x);
		pPRU->m2p_xform_3.d = -(FLOAT)(PRPts[3].y - PRPts[2].y) / (FLOAT)(MtrPts[3].y - MtrPts[2].y);

		// calculate m2p transform
		FLOAT fDet;
		fDet = (FLOAT)((-1.0) * (pPRU->m2p_xform_3.b) * (pPRU->m2p_xform_3.c) + (pPRU->m2p_xform_3.a) * (pPRU->m2p_xform_3.d));

		if (fDet == 0.0)
		{
			pPRU->bCalibrated_3 = FALSE;
			return FALSE;
		}

		pPRU->p2m_xform_3.a = (FLOAT)pPRU->m2p_xform_3.d / fDet;
		pPRU->p2m_xform_3.b = -(FLOAT)pPRU->m2p_xform_3.b / fDet;
		pPRU->p2m_xform_3.c = -(FLOAT)pPRU->m2p_xform_3.c / fDet;
		pPRU->p2m_xform_3.d = (FLOAT)pPRU->m2p_xform_3.a / fDet;

		pPRU->bCalibrated_3 = TRUE;
	}
	else if (pPRU->eCamMoveGroup4 == pPRU->eCamMoveGroupCurrent)
	{
		// calculate p2m transform (0 = Left, 1 = Right, 2 = Up, 3 = Down)
		pPRU->m2p_xform_4.a = -(FLOAT)(PRPts[1].x - PRPts[0].x) / (FLOAT)(MtrPts[1].x - MtrPts[0].x);
		pPRU->m2p_xform_4.b = -(FLOAT)(PRPts[3].x - PRPts[2].x) / (FLOAT)(MtrPts[3].y - MtrPts[2].y);
		pPRU->m2p_xform_4.c = -(FLOAT)(PRPts[1].y - PRPts[0].y) / (FLOAT)(MtrPts[1].x - MtrPts[0].x);
		pPRU->m2p_xform_4.d = -(FLOAT)(PRPts[3].y - PRPts[2].y) / (FLOAT)(MtrPts[3].y - MtrPts[2].y);

		// calculate m2p transform
		FLOAT fDet;
		fDet = (FLOAT)((-1.0) * (pPRU->m2p_xform_4.b) * (pPRU->m2p_xform_4.c) + (pPRU->m2p_xform_4.a) * (pPRU->m2p_xform_4.d));

		if (fDet == 0.0)
		{
			pPRU->bCalibrated_4 = FALSE;
			return FALSE;
		}

		pPRU->p2m_xform_4.a = (FLOAT)pPRU->m2p_xform_4.d / fDet;
		pPRU->p2m_xform_4.b = -(FLOAT)pPRU->m2p_xform_4.b / fDet;
		pPRU->p2m_xform_4.c = -(FLOAT)pPRU->m2p_xform_4.c / fDet;
		pPRU->p2m_xform_4.d = (FLOAT)pPRU->m2p_xform_4.a / fDet;

		pPRU->bCalibrated_4 = TRUE;
	}
	else
	{
		bResult = FALSE;
	}


	return bResult;
}

BOOL FindAutoCalibMatrix(PRU *pPRU, MTR_POSN MtrPts[], PR_RCOORD PRPts[]) //20150728 float PR
{
	BOOL bResult = TRUE;
	//if (pPRU->bUseFirstABCD)
	if (pPRU->eCamMoveGroup1 == pPRU->eCamMoveGroupCurrent)
	{
		// calculate p2m transform (0 = Left, 1 = Right, 2 = Up, 3 = Down)
		pPRU->m2p_xform.a = -(FLOAT)(PRPts[1].x - PRPts[0].x) / (FLOAT)(MtrPts[1].x - MtrPts[0].x);
		pPRU->m2p_xform.b = -(FLOAT)(PRPts[3].x - PRPts[2].x) / (FLOAT)(MtrPts[3].y - MtrPts[2].y);
		pPRU->m2p_xform.c = -(FLOAT)(PRPts[1].y - PRPts[0].y) / (FLOAT)(MtrPts[1].x - MtrPts[0].x);
		pPRU->m2p_xform.d = -(FLOAT)(PRPts[3].y - PRPts[2].y) / (FLOAT)(MtrPts[3].y - MtrPts[2].y);

		// calculate m2p transform
		FLOAT fDet;
		fDet = (FLOAT)((-1.0) * (pPRU->m2p_xform.b) * (pPRU->m2p_xform.c) + (pPRU->m2p_xform.a) * (pPRU->m2p_xform.d));

		if (fDet == 0.0)
		{
			pPRU->bCalibrated = FALSE;
			return FALSE;
		}

		pPRU->p2m_xform.a = (FLOAT)pPRU->m2p_xform.d / fDet;
		pPRU->p2m_xform.b = -(FLOAT)pPRU->m2p_xform.b / fDet;
		pPRU->p2m_xform.c = -(FLOAT)pPRU->m2p_xform.c / fDet;
		pPRU->p2m_xform.d = (FLOAT)pPRU->m2p_xform.a / fDet;

		pPRU->bCalibrated = TRUE;
	}
	else if (pPRU->eCamMoveGroup2 == pPRU->eCamMoveGroupCurrent)
	{
		// calculate p2m transform (0 = Left, 1 = Right, 2 = Up, 3 = Down)
		pPRU->m2p_xform_2.a = -(FLOAT)(PRPts[1].x - PRPts[0].x) / (FLOAT)(MtrPts[1].x - MtrPts[0].x);
		pPRU->m2p_xform_2.b = -(FLOAT)(PRPts[3].x - PRPts[2].x) / (FLOAT)(MtrPts[3].y - MtrPts[2].y);
		pPRU->m2p_xform_2.c = -(FLOAT)(PRPts[1].y - PRPts[0].y) / (FLOAT)(MtrPts[1].x - MtrPts[0].x);
		pPRU->m2p_xform_2.d = -(FLOAT)(PRPts[3].y - PRPts[2].y) / (FLOAT)(MtrPts[3].y - MtrPts[2].y);

		// calculate m2p transform
		FLOAT fDet;
		fDet = (FLOAT)((-1.0) * (pPRU->m2p_xform_2.b) * (pPRU->m2p_xform_2.c) + (pPRU->m2p_xform_2.a) * (pPRU->m2p_xform_2.d));

		if (fDet == 0.0)
		{
			pPRU->bCalibrated_2 = FALSE;
			return FALSE;
		}

		pPRU->p2m_xform_2.a = (FLOAT)pPRU->m2p_xform_2.d / fDet;
		pPRU->p2m_xform_2.b = -(FLOAT)pPRU->m2p_xform_2.b / fDet;
		pPRU->p2m_xform_2.c = -(FLOAT)pPRU->m2p_xform_2.c / fDet;
		pPRU->p2m_xform_2.d = (FLOAT)pPRU->m2p_xform_2.a / fDet;

		pPRU->bCalibrated_2 = TRUE;
	}
	else if (pPRU->eCamMoveGroup3 == pPRU->eCamMoveGroupCurrent)
	{
		// calculate p2m transform (0 = Left, 1 = Right, 2 = Up, 3 = Down)
		pPRU->m2p_xform_3.a = -(FLOAT)(PRPts[1].x - PRPts[0].x) / (FLOAT)(MtrPts[1].x - MtrPts[0].x);
		pPRU->m2p_xform_3.b = -(FLOAT)(PRPts[3].x - PRPts[2].x) / (FLOAT)(MtrPts[3].y - MtrPts[2].y);
		pPRU->m2p_xform_3.c = -(FLOAT)(PRPts[1].y - PRPts[0].y) / (FLOAT)(MtrPts[1].x - MtrPts[0].x);
		pPRU->m2p_xform_3.d = -(FLOAT)(PRPts[3].y - PRPts[2].y) / (FLOAT)(MtrPts[3].y - MtrPts[2].y);

		// calculate m2p transform
		FLOAT fDet;
		fDet = (FLOAT)((-1.0) * (pPRU->m2p_xform_3.b) * (pPRU->m2p_xform_3.c) + (pPRU->m2p_xform_3.a) * (pPRU->m2p_xform_3.d));

		if (fDet == 0.0)
		{
			pPRU->bCalibrated_3 = FALSE;
			return FALSE;
		}

		pPRU->p2m_xform_3.a = (FLOAT)pPRU->m2p_xform_3.d / fDet;
		pPRU->p2m_xform_3.b = -(FLOAT)pPRU->m2p_xform_3.b / fDet;
		pPRU->p2m_xform_3.c = -(FLOAT)pPRU->m2p_xform_3.c / fDet;
		pPRU->p2m_xform_3.d = (FLOAT)pPRU->m2p_xform_3.a / fDet;

		pPRU->bCalibrated_3 = TRUE;
	}
	else if (pPRU->eCamMoveGroup4 == pPRU->eCamMoveGroupCurrent)
	{
		// calculate p2m transform (0 = Left, 1 = Right, 2 = Up, 3 = Down)
		pPRU->m2p_xform_4.a = -(FLOAT)(PRPts[1].x - PRPts[0].x) / (FLOAT)(MtrPts[1].x - MtrPts[0].x);
		pPRU->m2p_xform_4.b = -(FLOAT)(PRPts[3].x - PRPts[2].x) / (FLOAT)(MtrPts[3].y - MtrPts[2].y);
		pPRU->m2p_xform_4.c = -(FLOAT)(PRPts[1].y - PRPts[0].y) / (FLOAT)(MtrPts[1].x - MtrPts[0].x);
		pPRU->m2p_xform_4.d = -(FLOAT)(PRPts[3].y - PRPts[2].y) / (FLOAT)(MtrPts[3].y - MtrPts[2].y);

		// calculate m2p transform
		FLOAT fDet;
		fDet = (FLOAT)((-1.0) * (pPRU->m2p_xform_4.b) * (pPRU->m2p_xform_4.c) + (pPRU->m2p_xform_4.a) * (pPRU->m2p_xform_4.d));

		if (fDet == 0.0)
		{
			pPRU->bCalibrated_4 = FALSE;
			return FALSE;
		}

		pPRU->p2m_xform_4.a = (FLOAT)pPRU->m2p_xform_4.d / fDet;
		pPRU->p2m_xform_4.b = -(FLOAT)pPRU->m2p_xform_4.b / fDet;
		pPRU->p2m_xform_4.c = -(FLOAT)pPRU->m2p_xform_4.c / fDet;
		pPRU->p2m_xform_4.d = (FLOAT)pPRU->m2p_xform_4.a / fDet;

		pPRU->bCalibrated_4 = TRUE;
	}
	else
	{
		bResult = FALSE;
	}
	return bResult;
}

BOOL FindCORCentre(INT nSample, MTR_POSN MtrPts[], D_MTR_POSN *p_dmtrCORPosn)
{
	// calculate the COR with modified least squares methods
	DOUBLE a, b, c, d, e;

	DOUBLE dTmp1, dTmp2, dTmp3, dTmp4, dTmp5;
	INT i;

	// calculate a
	dTmp1 = 0.0;
	dTmp2 = 0.0;

	for (i = 0; i < nSample; i++)
	{
		dTmp1 += (DOUBLE) MtrPts[i].x * (DOUBLE) MtrPts[i].x;
		dTmp2 += (DOUBLE) MtrPts[i].x;
	}

	a = (nSample * dTmp1) - (dTmp2 * dTmp2);

	// calculate b
	dTmp1 = 0.0;
	dTmp2 = 0.0;
	dTmp3 = 0.0;

	for (i = 0; i < nSample; i++)
	{
		dTmp1 += (DOUBLE) MtrPts[i].x * (DOUBLE) MtrPts[i].y;
		dTmp2 += (DOUBLE) MtrPts[i].x;
		dTmp3 += (DOUBLE) MtrPts[i].y;
	}

	b = (nSample * dTmp1) - (dTmp2 * dTmp3);

	// calculate c
	dTmp1 = 0.0;
	dTmp2 = 0.0;

	for (i = 0; i < nSample; i++)
	{
		dTmp1 += (DOUBLE) MtrPts[i].y * (DOUBLE) MtrPts[i].y;
		dTmp2 += (DOUBLE) MtrPts[i].y;
	}

	c = (nSample * dTmp1) - (dTmp2 * dTmp2);

	// calculate d
	dTmp1 = 0.0;
	dTmp2 = 0.0;
	dTmp3 = 0.0;
	dTmp4 = 0.0;
	dTmp5 = 0.0;

	for (i = 0; i < nSample; i++)
	{
		dTmp1 += (DOUBLE) MtrPts[i].x * (DOUBLE) MtrPts[i].y * (DOUBLE) MtrPts[i].y;
		dTmp2 += (DOUBLE) MtrPts[i].x;
		dTmp3 += (DOUBLE) MtrPts[i].y * (DOUBLE) MtrPts[i].y;
		dTmp4 += (DOUBLE) MtrPts[i].x * (DOUBLE) MtrPts[i].x * (DOUBLE) MtrPts[i].x;
		dTmp5 += (DOUBLE) MtrPts[i].x * (DOUBLE) MtrPts[i].x;
	}

	d = ((nSample * dTmp1) - (dTmp2 * dTmp3) + (nSample * dTmp4) - (dTmp2 * dTmp5)) / 2.0;

	// calculate e
	dTmp1 = 0.0;
	dTmp2 = 0.0;
	dTmp3 = 0.0;
	dTmp4 = 0.0;
	dTmp5 = 0.0;

	for (i = 0; i < nSample; i++)
	{
		dTmp1 += (DOUBLE) MtrPts[i].y * (DOUBLE) MtrPts[i].x * (DOUBLE) MtrPts[i].x;
		dTmp2 += (DOUBLE) MtrPts[i].y;
		dTmp3 += (DOUBLE) MtrPts[i].x * (DOUBLE) MtrPts[i].x;
		dTmp4 += (DOUBLE) MtrPts[i].y * (DOUBLE) MtrPts[i].y * (DOUBLE) MtrPts[i].y;
		dTmp5 += (DOUBLE) MtrPts[i].y * (DOUBLE) MtrPts[i].y;
	}

	e = ((nSample * dTmp1) - (dTmp2 * dTmp3) + (nSample * dTmp4) - (dTmp2 * dTmp5)) / 2.0;

	// calculate the centre point
	DOUBLE dDen;
	dDen = (a * c) - (b * b);

	if (dDen == 0.0)
	{
		p_dmtrCORPosn->x = 0.0;
		p_dmtrCORPosn->y = 0.0;

		return FALSE;
	}

	p_dmtrCORPosn->x = ((d * c) - (b * e)) / dDen;
	p_dmtrCORPosn->y = ((a * e) - (b * d)) / dDen;

	return TRUE;
}

BOOL RefineCORCentre(MTR_POSN mtrCurPosn, DOUBLE dAngle, D_MTR_POSN dmtrResultPosn, D_MTR_POSN *p_dmtrCORPosn)
{
	DOUBLE dAngleRad;

	dAngleRad = (PI / 180.0) * dAngle;

	if (dAngleRad == 0.0)
	{
		return FALSE;
	}

	DOUBLE dDeterminant;

	dDeterminant = 1.0 / ((cos(dAngleRad) - 1.0) * (cos(dAngleRad) - 1.0) + sin(dAngleRad) * sin(dAngleRad));

	p_dmtrCORPosn->x = dDeterminant * (dmtrResultPosn.x * (1.0 - cos(dAngleRad)) - dmtrResultPosn.y * sin(dAngleRad) 
									   + mtrCurPosn.x * (1.0 - cos(dAngleRad)) + mtrCurPosn.y * sin(dAngleRad));

	p_dmtrCORPosn->y = dDeterminant * (dmtrResultPosn.x * sin(dAngleRad) + dmtrResultPosn.y * (1.0 - cos(dAngleRad))
									   - mtrCurPosn.x * sin(dAngleRad) + mtrCurPosn.y * (1.0 - cos(dAngleRad)));

	return TRUE;
}

BOOL FindLeastSquareCOR(std::vector <D_MTR_POSN> &vecPoint, D_MTR_POSN &dmtrCORPosn, DOUBLE &dRadius) //20160530
{
	// Matrix 3x3
	//	A11, A12, A13
	//	A21, A22, A23
	//	A31, A32, A33

	// Matrix 3x3
	//	A[0][0], A[0][1] A[0][2]
	//	A[1][0], A[1][1] A[1][2]
	//	A[2][0], A[2][1] A[2][2]

	LONG ii = 0, jj = 0;
	DOUBLE dA11 = 0.0, dA12 = 0.0, dA13 = 0.0, dA21 = 0.0, dA22 = 0.0, dA23 = 0.0, dA31 = 0.0, dA32 = 0.0, dA33 = 0.0;
	DOUBLE matrixA[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
	DOUBLE matrixB[3] = {0.0, 0.0, 0.0};
	DOUBLE dDetMatrixA = 0.0, dResultA = 0.0, dResultB = 0.0, dResultC = 0.0;
	DOUBLE dOriginX =0.0, dOriginY =0.0;

	if (vecPoint.size() <= 0)
	{
		return FALSE;
	}
	dOriginX = vecPoint[0].x;
	dOriginY = vecPoint[0].y;

	for (ii = 0; ii < (LONG)vecPoint.size(); ii++)
	{
		vecPoint[ii].x -= dOriginX;
		vecPoint[ii].y -= dOriginY;
	}

	for (ii = 0; ii < (LONG)vecPoint.size(); ii++)
	{
		matrixA[0][0] += 2* vecPoint[ii].x * vecPoint[ii].x;
		matrixA[0][1] += 2 * vecPoint[ii].x * vecPoint[ii].y;
		matrixA[0][2] += vecPoint[ii].x;

		matrixA[1][0] += 2 * vecPoint[ii].x * vecPoint[ii].y;
		matrixA[1][1] += 2 * vecPoint[ii].y * vecPoint[ii].y;
		matrixA[1][2] += vecPoint[ii].y;

		matrixA[2][0] += 2 * vecPoint[ii].x;
		matrixA[2][1] += 2 * vecPoint[ii].y;
		matrixA[2][2] ++;

		matrixB[0] -= vecPoint[ii].x * ((vecPoint[ii].x * vecPoint[ii].x) + (vecPoint[ii].y * vecPoint[ii].y));
		matrixB[1] -= vecPoint[ii].y * ((vecPoint[ii].x * vecPoint[ii].x) + (vecPoint[ii].y * vecPoint[ii].y));
		matrixB[2] -= (vecPoint[ii].x * vecPoint[ii].x) + (vecPoint[ii].y * vecPoint[ii].y);
	}

	//determinant of A
	dDetMatrixA = (matrixA[0][0] * matrixA[1][1] * matrixA[2][2])
							+ (matrixA[0][1] * matrixA[1][2] * matrixA[2][0])
							+ (matrixA[0][2] * matrixA[1][0] * matrixA[2][1])
							- (matrixA[0][2] * matrixA[1][1] * matrixA[2][0])
							- (matrixA[0][1] * matrixA[1][0] * matrixA[2][2])
							- (matrixA[0][0] * matrixA[1][2] * matrixA[2][1]);

	if (fabs(dDetMatrixA) < 0.0000000001)
	{
		return FALSE;
	}
	//Find inverse of A
	//	A11, A12, A13
	//	A21, A22, A23
	//	A31, A32, A33

	//	A[0][0], A[0][1] A[0][2]
	//	A[1][0], A[1][1] A[1][2]
	//	A[2][0], A[2][1] A[2][2]

	dA11 = (matrixA[1][1] * matrixA[2][2]) - (matrixA[1][2] * matrixA[2][1]);
	dA12 = (matrixA[1][0] * matrixA[2][2]) - (matrixA[1][2] * matrixA[2][0]);
	dA13 = (matrixA[1][0] * matrixA[2][1]) - (matrixA[1][1] * matrixA[2][0]);

	dA21 = (matrixA[0][1] * matrixA[2][2]) - (matrixA[0][2] * matrixA[2][1]);
	dA22 = (matrixA[0][0] * matrixA[2][2]) - (matrixA[0][2] * matrixA[2][0]);
	dA23 = (matrixA[0][0] * matrixA[2][1]) - (matrixA[0][1] * matrixA[2][0]);

	dA31 = (matrixA[0][1] * matrixA[1][2]) - (matrixA[0][2] * matrixA[1][1]);
	dA32 = (matrixA[0][0] * matrixA[1][2]) - (matrixA[0][2] * matrixA[1][0]);
	dA33 = (matrixA[0][0] * matrixA[1][1]) - (matrixA[0][1] * matrixA[1][0]);

////
	matrixA[0][0] = dA11;
	//matrixA[0][1] =dlA12;
	//matrixA[0][2] = dA13;

	//matrixA[1][0] = dA21;
	matrixA[1][1] = dA22;
	//matrixA[1][2] = dA23;

	//matrixA[2][0] = dA31;
	//matrixA[2][1] = dA32;
	matrixA[2][2] = dA33;
	
//Transpose of A
	matrixA[0][1] = -dA21;
	matrixA[0][2] = dA31;

	matrixA[1][0] = -dA12;
	matrixA[1][2] = -dA32;

	matrixA[2][0] = dA13;
	matrixA[2][1] = -dA23;

	for (ii = 0; ii < 3; ii++)
	{
		for (jj = 0; jj < 3; jj++)
		{
			matrixA[ii][jj] = matrixA[ii][jj] / dDetMatrixA;
		}
	}

	//	A[0][0], A[0][1] A[0][2]
	//	A[1][0], A[1][1] A[1][2]
	//	A[2][0], A[2][1] A[2][2]
	dResultA = matrixA[0][0] * matrixB[0] + matrixA[0][1] * matrixB[1] + matrixA[0][2] * matrixB[2];
	dResultB = matrixA[1][0] * matrixB[0] + matrixA[1][1] * matrixB[1] + matrixA[1][2] * matrixB[2];
	dResultC = matrixA[2][0] * matrixB[0] + matrixA[2][1] * matrixB[1] + matrixA[2][2] * matrixB[2];

	dmtrCORPosn.x = -dResultA;
	dmtrCORPosn.y = -dResultB;
	dRadius = sqrt((dResultA * dResultA) + (dResultB * dResultB) - dResultC);

	dmtrCORPosn.x += dOriginX;
	dmtrCORPosn.y += dOriginY;

	return TRUE;
}

BOOL GetRandomCirclePoint(DOUBLE dPointX, DOUBLE dCenterX, DOUBLE dCenterY, DOUBLE dRadius, DOUBLE dNoiseLmt, DOUBLE &dOutX, DOUBLE &dOutY)
{
	DOUBLE dTmp1 = 0.0, dTmp2 = 0.0, dTmpY = 0.0;
	INT ii = 0, jj = 0;
	
	if (GetCirclePoint(dPointX, dCenterX, dCenterY, dRadius, dTmpY))
	{
		//srand((unsigned) time(NULL));
		ii = rand();
		jj = rand();

		dTmp1 = ((DOUBLE) ii / (DOUBLE) RAND_MAX) * dNoiseLmt;
		dTmp2 = ((DOUBLE) jj / (DOUBLE) RAND_MAX) * dNoiseLmt;
		if (ii > (RAND_MAX / 2))
		{
			dOutX = dPointX + dTmp1;
		}
		else
		{
			dOutX = dPointX - dTmp1;
		}

		if (jj > (RAND_MAX / 2))
		{
			dOutY = dTmpY + dTmp2;
		}
		else
		{
			dOutY = dTmpY - dTmp2;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL GetCirclePoint(DOUBLE dPointX, DOUBLE dCenterX, DOUBLE dCenterY, DOUBLE dRadius, DOUBLE &dPointY)
{
	DOUBLE dTmp = 0.0;

	if ((dRadius * dRadius) - ((dPointX - dCenterX) * (dPointX - dCenterX)) < 0)
	{
		return FALSE;
	}

	dPointY = sqrt((dRadius * dRadius) - ((dPointX - dCenterX) * (dPointX - dCenterX))) + dCenterY;

	return TRUE;
}

BOOL FindCORWithAngle2Point(const D_MTR_POSN dmtrP1, const D_MTR_POSN dmtrP2, const DOUBLE dAngle, D_MTR_POSN &dmtrCOR1) //20160608
{
	DOUBLE dM0 = 1.0, dM1 = 1.0, dM2 = 1.0, dC1 = 0.0, dC2 = 0.0;
	DOUBLE dAngleRad = 0.0, dTmp;

	dAngleRad = (PI / 180.0) * dAngle;
	dTmp = tan(PI / 2.0 - dAngleRad / 2.0);

	if (fabs(dmtrP2.x - dmtrP1.x) < 0.000000001)
	{
		return FALSE;
	}
	dM0 = (dmtrP2.y - dmtrP1.y) / (dmtrP2.x - dmtrP1.x);

	if (fabs(1 - dM0 * dTmp) < 0.000000001)
	{
		return FALSE;
	}
	if (fabs(1 + dM0 * dTmp) < 0.000000001)
	{
		return FALSE;
	}

	dM1 = (dM0 + dTmp) / (1 - (dM0 * dTmp));
	dM2 = (dM0 - dTmp) / (1 + (dM0 * dTmp));
	dC1 = dmtrP1.y - (dM1 * dmtrP1.x);
	dC2 = dmtrP2.y - (dM2 * dmtrP2.x);

	if (fabs(dM1 - dM2) < 0.000000001)
	{
		return FALSE;
	}

	dmtrCOR1.x = (dC2 - dC1) / (dM1 - dM2);
	dmtrCOR1.y = dM1 * dmtrCOR1.x + dC1;
	return TRUE;
}

BOOL VectorRotate(D_MTR_POSN dmtrIn, DOUBLE dAngleRad, D_MTR_POSN *p_dmtrOut) //20120810
{
	D_MTR_POSN dmtrTmp;

	dmtrTmp.x = dmtrIn.x;
	dmtrTmp.y = dmtrIn.y;

	p_dmtrOut->x = (dmtrTmp.x * cos(dAngleRad)) - (dmtrTmp.y * sin(dAngleRad));
	p_dmtrOut->y = (dmtrTmp.x * sin(dAngleRad)) + (dmtrTmp.y * cos(dAngleRad));
	return TRUE;
}

BOOL VectorRotate(MTR_POSN mtrIn, DOUBLE dAngleRad, MTR_POSN *p_mtrOut) //20120810
{
	D_MTR_POSN dmtrTmp;

	dmtrTmp.x = mtrIn.x;
	dmtrTmp.y = mtrIn.y;

	p_mtrOut->x = DoubleToLong((dmtrTmp.x * cos(dAngleRad)) - (dmtrTmp.y * sin(dAngleRad)));
	p_mtrOut->y = DoubleToLong((dmtrTmp.x * sin(dAngleRad)) + (dmtrTmp.y * cos(dAngleRad)));
	return TRUE;
}

DOUBLE calcAngle(D_MTR_POSN dmtrP1, D_MTR_POSN dmtrP2) //20120924
{
	DOUBLE dx, dy;

	dx = 1.0 * (dmtrP2.x - dmtrP1.x);
	dy = 1.0 * (dmtrP2.y - dmtrP1.y);

	if (dx != 0)
	{
		return atan(dy / dx);
	}
	else
	{
		return 0.0;
	}
}

DOUBLE calcAngleDegree(D_MTR_POSN dmtrP1, D_MTR_POSN dmtrP2)
{
	DOUBLE dx, dy;

	dx = 1.0 * (dmtrP2.x - dmtrP1.x);
	dy = 1.0 * (dmtrP2.y - dmtrP1.y);

	if (dx != 0)
	{
		return (180.0 / PI) * atan(dy / dx);
	}
	else
	{
		return 0.0;
	}
}

DOUBLE calcAngle(MTR_POSN mtrP1, MTR_POSN mtrP2)
{
	DOUBLE dx, dy;

	dx = 1.0 * (mtrP2.x - mtrP1.x);
	dy = 1.0 * (mtrP2.y - mtrP1.y);

	if (dx != 0)
	{
		return atan(dy / dx);
	}
	else
	{
		return 0.0;
	}
}

DOUBLE calcAngleDegree(MTR_POSN mtrP1, MTR_POSN mtrP2)
{
	DOUBLE dx, dy;

	dx = 1.0 * (mtrP2.x - mtrP1.x);
	dy = 1.0 * (mtrP2.y - mtrP1.y);

	if (dx != 0)
	{
		return (180.0 / PI) * atan(dy / dx);
	}
	else
	{
		return 0.0;
	}
}

DOUBLE calcDisplayAngleDegree(DOUBLE dy, DOUBLE dx)
{ 
//range -180 to 180 degree
	DOUBLE dRtn;
	if (dx != 0.0)
	{
		if (dy == 0.0)
		{
			if (dx > 0)
			{
				dRtn = 0.0;
			}
			else
			{
				dRtn = 180.0;
			}
		}
		else if (dy > 0 && dx < 0)
		{
			dRtn = 180.0 + (atan(dy / dx) * 180.0 / PI);
		}
		else if (dy < 0 && dx < 0)
		{
			dRtn = 180.0 - (atan(dy / dx) * 180.0 / PI);
		}
		else
		{
			dRtn = atan(dy / dx) * 180.0 / PI;
		}
	}
	else if (dy == 0.0)
	{
		dRtn = 0.0;
	}
	else if (dy > 0.0)
	{
		dRtn = 90.0;
	}
	else
	{
		dRtn = -90.0;
	}
	return dRtn;
}

BOOL ForceToDac(DOUBLE dTargetForce, FORCE_DATA stForceCalib[], LONG *p_lOpenDacValue)
{
	BOOL bStatus = FALSE;

	// min force, dead weight of the bond head
	if (dTargetForce < stForceCalib[0].Force_kgf)
	{
		*p_lOpenDacValue = stForceCalib[0].OpenDacValue;
		bStatus = FALSE;
	}
	else
	{
		INT i = 0;

		for (i = 0; i < MAX_FORCE_DATA - 1; i++)
		{
			if ((dTargetForce == stForceCalib[i].Force_kgf) /*&& (stForceCalib[i].Force_kgf == stForceCalib[i+1].Force_kgf)*/)
			{
				*p_lOpenDacValue = stForceCalib[i].OpenDacValue;
				bStatus = TRUE;
				break;
			}

			if ((dTargetForce >= stForceCalib[i].Force_kgf) && (dTargetForce < stForceCalib[i + 1].Force_kgf))
			{
				*p_lOpenDacValue = DoubleToLong((DOUBLE)(stForceCalib[i + 1].OpenDacValue)
												- (stForceCalib[i + 1].Force_kgf - dTargetForce) 
												* (DOUBLE)(stForceCalib[i + 1].OpenDacValue - stForceCalib[i].OpenDacValue) 
												/ (stForceCalib[i + 1].Force_kgf - stForceCalib[i].Force_kgf));
				bStatus = TRUE;
				break;
			}

			if (dTargetForce == stForceCalib[i + 1].Force_kgf)
			{
				*p_lOpenDacValue = stForceCalib[i + 1].OpenDacValue;
				bStatus = TRUE;
				break;
			}
		}
	}

	// max force, projection
	if (!bStatus)
	{
		if (stForceCalib[MAX_FORCE_DATA - 1].Force_kgf - stForceCalib[MAX_FORCE_DATA - 2].Force_kgf == 0)
		{
			*p_lOpenDacValue = 0;
		}
		else
		{
			*p_lOpenDacValue = -DoubleToLong((DOUBLE)(stForceCalib[MAX_FORCE_DATA - 1].OpenDacValue)
											- (stForceCalib[MAX_FORCE_DATA - 1].Force_kgf - dTargetForce) 
											* (DOUBLE)(stForceCalib[MAX_FORCE_DATA - 1].OpenDacValue - stForceCalib[MAX_FORCE_DATA - 2].OpenDacValue) 
											/ (stForceCalib[MAX_FORCE_DATA - 1].Force_kgf - stForceCalib[MAX_FORCE_DATA - 2].Force_kgf));
		}
	}

	//if (*p_lOpenDacValue < 0.0)
	//{
	//	*p_lOpenDacValue = 0;
	//}
	return bStatus;
}

BOOL DacToForce(LONG lTargetDacValue, FORCE_DATA stForceCalib[], DOUBLE *p_dForce)
{
	BOOL bStatus = FALSE;
	lTargetDacValue = -lTargetDacValue;

	// min pressure
	if (lTargetDacValue < stForceCalib[0].OpenDacValue)
	{
		*p_dForce = stForceCalib[0].Force_kgf;
		bStatus = FALSE;
	}
	else
	{
		INT i = 0;

		for (i = 0; i < MAX_FORCE_DATA - 1; i++)
		{
			if ((lTargetDacValue == stForceCalib[i].OpenDacValue) /*&& (stForceCalib[i].OpenDacValue == stForceCalib[i+1].OpenDacValue)*/)
			{
				*p_dForce = stForceCalib[i].Force_kgf;
				bStatus = TRUE;
				break;
			}

			if ((lTargetDacValue >= stForceCalib[i].OpenDacValue) && (lTargetDacValue < stForceCalib[i + 1].OpenDacValue))
			{
				*p_dForce = stForceCalib[i + 1].Force_kgf 
							- (stForceCalib[i + 1].OpenDacValue - lTargetDacValue) 
							* (stForceCalib[i + 1].Force_kgf - stForceCalib[i].Force_kgf) 
							/ (stForceCalib[i + 1].OpenDacValue - stForceCalib[i].OpenDacValue);
				bStatus = TRUE;
				break;
			}

			if (lTargetDacValue == stForceCalib[i + 1].OpenDacValue)
			{
				*p_dForce = stForceCalib[i + 1].Force_kgf;
				bStatus = TRUE;
				break;
			}

		}
	}

	// max force, projection
	if (!bStatus)
	{
		if (stForceCalib[MAX_FORCE_DATA - 1].OpenDacValue - stForceCalib[MAX_FORCE_DATA - 2].OpenDacValue == 0)
		{
			*p_dForce = 0;
		}
		else
		{
			*p_dForce = stForceCalib[MAX_FORCE_DATA - 1].Force_kgf 
						- (stForceCalib[MAX_FORCE_DATA - 1].OpenDacValue - lTargetDacValue) 
						* (stForceCalib[MAX_FORCE_DATA - 1].Force_kgf - stForceCalib[MAX_FORCE_DATA - 2].Force_kgf) 
						/ (stForceCalib[MAX_FORCE_DATA - 1].OpenDacValue - stForceCalib[MAX_FORCE_DATA - 2].OpenDacValue);
		}
	}

	if (*p_dForce < 0.0)
	{
		*p_dForce = 0.0;
	}
	return bStatus;
}

BOOL ForceToPressure(DOUBLE dTargetForce, FORCE_DATA stForceCalib[], DOUBLE *p_dPressure)
{
	BOOL bStatus = FALSE;

	// min force, dead weight of the bond head
	if (dTargetForce < stForceCalib[0].Force_kgf)
	{
		*p_dPressure = stForceCalib[0].Pressure_bar;
		bStatus = FALSE;
	}
	else
	{
		INT i = 0;

		for (i = 0; i < MAX_FORCE_DATA - 1; i++)
		{
			if ((dTargetForce == stForceCalib[i].Force_kgf) && (stForceCalib[i].Force_kgf == stForceCalib[i + 1].Force_kgf))
			{
				*p_dPressure = stForceCalib[i].Pressure_bar;
				bStatus = TRUE;
				break;
			}

			if ((dTargetForce >= stForceCalib[i].Force_kgf) && (dTargetForce < stForceCalib[i + 1].Force_kgf))
			{
				*p_dPressure = stForceCalib[i + 1].Pressure_bar 
							   - (stForceCalib[i + 1].Force_kgf - dTargetForce) 
							   * (stForceCalib[i + 1].Pressure_bar - stForceCalib[i].Pressure_bar) 
							   / (stForceCalib[i + 1].Force_kgf - stForceCalib[i].Force_kgf);
				bStatus = TRUE;
				break;
			}
		}
	}

	// max force, projection
	if (!bStatus)
	{
		if (stForceCalib[MAX_FORCE_DATA - 1].Force_kgf - stForceCalib[MAX_FORCE_DATA - 2].Force_kgf == 0)
		{
			*p_dPressure = 0;
		}
		else
		{
			*p_dPressure = stForceCalib[MAX_FORCE_DATA - 1].Pressure_bar 
						   - (stForceCalib[MAX_FORCE_DATA - 1].Force_kgf - dTargetForce) 
						   * (stForceCalib[MAX_FORCE_DATA - 1].Pressure_bar - stForceCalib[MAX_FORCE_DATA - 2].Pressure_bar) 
						   / (stForceCalib[MAX_FORCE_DATA - 1].Force_kgf - stForceCalib[MAX_FORCE_DATA - 2].Force_kgf);
		}
	}

	if (*p_dPressure < 0.0)
	{
		*p_dPressure = 0.0;
	}
	return bStatus;
}

BOOL PressureToForce(DOUBLE dTargetPressure, FORCE_DATA stForceCalib[], DOUBLE *p_dForce)
{
	BOOL bStatus = FALSE;

	// min pressure
	if (dTargetPressure < stForceCalib[0].Pressure_bar)
	{
		*p_dForce = stForceCalib[0].Force_kgf;
		bStatus = FALSE;
	}
	else
	{
		INT i = 0;

		for (i = 0; i < MAX_FORCE_DATA - 1; i++)
		{
			if ((dTargetPressure == stForceCalib[i].Pressure_bar) && (stForceCalib[i].Pressure_bar == stForceCalib[i + 1].Pressure_bar))
			{
				*p_dForce = stForceCalib[i].Force_kgf;
				bStatus = TRUE;
				break;
			}

			if ((dTargetPressure >= stForceCalib[i].Pressure_bar) && (dTargetPressure < stForceCalib[i + 1].Pressure_bar))
			{
				*p_dForce = stForceCalib[i + 1].Force_kgf 
							- (stForceCalib[i + 1].Pressure_bar - dTargetPressure) 
							* (stForceCalib[i + 1].Force_kgf - stForceCalib[i].Force_kgf) 
							/ (stForceCalib[i + 1].Pressure_bar - stForceCalib[i].Pressure_bar);
				bStatus = TRUE;
				break;
			}
		}
	}

	// max force, projection
	if (!bStatus)
	{
		if (stForceCalib[MAX_FORCE_DATA - 1].Pressure_bar - stForceCalib[MAX_FORCE_DATA - 2].Pressure_bar == 0)
		{
			*p_dForce = 0;
		}
		else
		{
			*p_dForce = stForceCalib[MAX_FORCE_DATA - 1].Force_kgf 
						- (stForceCalib[MAX_FORCE_DATA - 1].Pressure_bar - dTargetPressure) 
						* (stForceCalib[MAX_FORCE_DATA - 1].Force_kgf - stForceCalib[MAX_FORCE_DATA - 2].Force_kgf) 
						/ (stForceCalib[MAX_FORCE_DATA - 1].Pressure_bar - stForceCalib[MAX_FORCE_DATA - 2].Pressure_bar);
		}
	}

	if (*p_dForce < 0.0)
	{
		*p_dForce = 0.0;
	}
	return bStatus;
}


BOOL RotaryToLinearEnc(LONG lTargetRotaryEnc, WHZ_DATA stWhZ_q[], LONG *p_lLinearEnc)
{
	BOOL bStatus = FALSE;
	LONG lResult;

	// WHZ lowest Calib posn
	if (lTargetRotaryEnc > stWhZ_q[0].RotaryPosn)
	{
		*p_lLinearEnc = stWhZ_q[0].LinearPosn;
		bStatus = FALSE;
	}
	else if (lTargetRotaryEnc < stWhZ_q[MAX_NUM_WHZ_CAL_STEP - 1].RotaryPosn) // over highest calib posn
	{
		*p_lLinearEnc = stWhZ_q[MAX_NUM_WHZ_CAL_STEP - 1].LinearPosn;
		bStatus = FALSE;
	}
	else if (lTargetRotaryEnc == stWhZ_q[MAX_NUM_WHZ_CAL_STEP - 1].RotaryPosn) // highest calib posn
	{
		*p_lLinearEnc = stWhZ_q[MAX_NUM_WHZ_CAL_STEP - 1].LinearPosn;
		bStatus = TRUE;
	}
	else // search Enc 
	{
		INT i = 0;

		for (i = 0; i < MAX_NUM_WHZ_CAL_STEP - 1; i++)
		{
			if (lTargetRotaryEnc == stWhZ_q[i].RotaryPosn) 
			{
				*p_lLinearEnc = stWhZ_q[i].LinearPosn;
				bStatus = TRUE;
				break;
			}

			if ((lTargetRotaryEnc < stWhZ_q[i].RotaryPosn) && (lTargetRotaryEnc > stWhZ_q[i + 1].RotaryPosn))
			{
				lResult = stWhZ_q[i + 1].LinearPosn 
						  - ((stWhZ_q[i + 1].RotaryPosn - lTargetRotaryEnc) 
							 * (stWhZ_q[i + 1].LinearPosn - stWhZ_q[i].LinearPosn) 
							 / (stWhZ_q[i + 1].RotaryPosn - stWhZ_q[i].RotaryPosn));	//debug

				*p_lLinearEnc = stWhZ_q[i + 1].LinearPosn 
								- ((stWhZ_q[i + 1].RotaryPosn - lTargetRotaryEnc) 
								   * (stWhZ_q[i + 1].LinearPosn - stWhZ_q[i].LinearPosn) 
								   / (stWhZ_q[i + 1].RotaryPosn - stWhZ_q[i].RotaryPosn));
				bStatus = TRUE;
				break;
			}
		}
	}

	return bStatus;
}


BOOL LinearToRotaryEnc(LONG lTargetLinearEnc, WHZ_DATA stWhZ_q[], LONG *p_lRotaryEnc)
{
	BOOL bStatus = FALSE;

	// WHZ lowest Calib posn
	if (lTargetLinearEnc > stWhZ_q[0].LinearPosn)
	{
		*p_lRotaryEnc = stWhZ_q[0].RotaryPosn;
		bStatus = FALSE;
	}
	else if (lTargetLinearEnc < stWhZ_q[MAX_NUM_WHZ_CAL_STEP - 1].LinearPosn) // over highest calib posn
	{
		*p_lRotaryEnc = stWhZ_q[MAX_NUM_WHZ_CAL_STEP - 1].RotaryPosn;
		bStatus = FALSE;
	}
	else if (lTargetLinearEnc == stWhZ_q[MAX_NUM_WHZ_CAL_STEP - 1].LinearPosn) // highest calib posn
	{
		*p_lRotaryEnc = stWhZ_q[MAX_NUM_WHZ_CAL_STEP - 1].RotaryPosn;
		bStatus = TRUE;
	}
	else // search Enc 
	{
		INT i = 0;

		for (i = 0; i < MAX_NUM_WHZ_CAL_STEP - 1; i++)
		{
			if (lTargetLinearEnc == stWhZ_q[i].LinearPosn) 
			{
				*p_lRotaryEnc = stWhZ_q[i].RotaryPosn;
				bStatus = TRUE;
				break;
			}

			if ((lTargetLinearEnc < stWhZ_q[i].LinearPosn) && (lTargetLinearEnc > stWhZ_q[i + 1].LinearPosn))
			{
				*p_lRotaryEnc = stWhZ_q[i + 1].RotaryPosn 
								- ((stWhZ_q[i + 1].LinearPosn - lTargetLinearEnc) 
								   * (stWhZ_q[i + 1].RotaryPosn - stWhZ_q[i].RotaryPosn) 
								   / (stWhZ_q[i + 1].LinearPosn - stWhZ_q[i].LinearPosn));
				bStatus = TRUE;
				break;
			}
		}
	}

	return bStatus;
}

BOOL RotaryToLinearEnc(LONG lTargetRotaryEnc, ENC_CALIB_DATA stWhZ_q[], LONG *p_lLinearEnc)
{
	BOOL bStatus = FALSE;

	// WHZ lowest Calib posn
	if (lTargetRotaryEnc > stWhZ_q[0].RotaryPosn)
	{
		*p_lLinearEnc = stWhZ_q[0].LinearPosn;
		bStatus = FALSE;
	}
	else if (lTargetRotaryEnc < stWhZ_q[PREBOND_T_CALIB_DATA - 1].RotaryPosn) // over highest calib posn
	{
		*p_lLinearEnc = stWhZ_q[PREBOND_T_CALIB_DATA - 1].LinearPosn;
		bStatus = FALSE;
	}
	else if (lTargetRotaryEnc == stWhZ_q[PREBOND_T_CALIB_DATA - 1].RotaryPosn) // highest calib posn
	{
		*p_lLinearEnc = stWhZ_q[PREBOND_T_CALIB_DATA - 1].LinearPosn;
		bStatus = TRUE;
	}
	else // search Enc 
	{
		INT i = 0;

		for (i = 0; i < PREBOND_T_CALIB_DATA - 1; i++)
		{
			if (lTargetRotaryEnc == stWhZ_q[i].RotaryPosn) 
			{
				*p_lLinearEnc = stWhZ_q[i].LinearPosn;
				bStatus = TRUE;
				break;
			}

			if ((lTargetRotaryEnc < stWhZ_q[i].RotaryPosn) && (lTargetRotaryEnc > stWhZ_q[i + 1].RotaryPosn))
			{
				*p_lLinearEnc = stWhZ_q[i + 1].LinearPosn
								- ((stWhZ_q[i + 1].RotaryPosn - lTargetRotaryEnc) 
								   * (stWhZ_q[i + 1].LinearPosn - stWhZ_q[i].LinearPosn) 
								   / (stWhZ_q[i + 1].RotaryPosn - stWhZ_q[i].RotaryPosn));
				bStatus = TRUE;
				break;
			}
		}
	}

	return bStatus;
}


BOOL LinearToRotaryEnc(LONG lTargetLinearEnc, ENC_CALIB_DATA stWhZ_q[], LONG *p_lRotaryEnc)
{
	BOOL bStatus = FALSE;

	// WHZ lowest Calib posn
	if (lTargetLinearEnc > stWhZ_q[0].LinearPosn)
	{
		*p_lRotaryEnc = stWhZ_q[0].RotaryPosn;
		bStatus = FALSE;
	}
	else if (lTargetLinearEnc < stWhZ_q[PREBOND_T_CALIB_DATA - 1].LinearPosn) // over highest calib posn
	{
		*p_lRotaryEnc = stWhZ_q[PREBOND_T_CALIB_DATA - 1].RotaryPosn;
		bStatus = FALSE;
	}
	else if (lTargetLinearEnc == stWhZ_q[PREBOND_T_CALIB_DATA - 1].LinearPosn) // highest calib posn
	{
		*p_lRotaryEnc = stWhZ_q[PREBOND_T_CALIB_DATA - 1].RotaryPosn;
		bStatus = TRUE;
	}
	else // search Enc 
	{
		INT i = 0;

		for (i = 0; i < PREBOND_T_CALIB_DATA - 1; i++)
		{
			if (lTargetLinearEnc == stWhZ_q[i].LinearPosn) 
			{
				*p_lRotaryEnc = stWhZ_q[i].RotaryPosn;
				bStatus = TRUE;
				break;
			}

			if ((lTargetLinearEnc < stWhZ_q[i].LinearPosn) && (lTargetLinearEnc > stWhZ_q[i + 1].LinearPosn))
			{
				*p_lRotaryEnc = stWhZ_q[i + 1].RotaryPosn 
								- ((stWhZ_q[i + 1].LinearPosn - lTargetLinearEnc) 
								   * (stWhZ_q[i + 1].RotaryPosn - stWhZ_q[i].RotaryPosn) 
								   / (stWhZ_q[i + 1].LinearPosn - stWhZ_q[i].LinearPosn));
				bStatus = TRUE;
				break;
			}
		}
	}

	return bStatus;
}

//BOOL CalcWHAbsoluteLevel(LONG lRefLevel, LONG lOffset, WHZ_DATA stData[MAX_NUM_OF_WHZ][MAX_NUM_WHZ_CAL_STEP], LONG *p_lAbsoluteLevel, LONG WHZ)
//{
//	//CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
//	LONG lLevelInLinear = 0;
//	LONG lLevelInRotary	= 0;
//	BOOL bResult = TRUE;
//	if (
//		WHZ == WHZ_2
//	   )
//	{
//		// Transform Reference Level from Motor Count to Linear
//		if (!RotaryToLinearEnc(lRefLevel, stData[WHZ_2], &lLevelInLinear))
//		{
//			lLevelInRotary = lRefLevel;
//			bResult = FALSE;
//		}
//		else
//		{
//			lLevelInLinear += DistanceToCount(lOffset, WHZ_CAL_DISTPERCNT);
//
//			if (!LinearToRotaryEnc(lLevelInLinear, stData[WHZ_2], &lLevelInRotary))
//			{
//				lLevelInRotary = lRefLevel;
//				bResult = FALSE;
//			}
//		}
//	}
//	else
//	{
//	// Transform Reference Level from Motor Count to Linear
//		if (!RotaryToLinearEnc(lRefLevel, stData[WHZ_1], &lLevelInLinear))
//		{
//			lLevelInRotary = lRefLevel;
//			bResult = FALSE;
//		}
//		else
//		{
//			lLevelInLinear += DistanceToCount(lOffset, WHZ_CAL_DISTPERCNT);
//
//			if (!LinearToRotaryEnc(lLevelInLinear, stData[WHZ_1], &lLevelInRotary))
//			{
//				lLevelInRotary = lRefLevel;
//				bResult = FALSE;
//			}
//		}
//	}
//	*p_lAbsoluteLevel = lLevelInRotary;
//
//	return bResult;
//}

//BOOL CalcTAbsoluteLevel(LONG lRefLevel, LONG lOffset, ENC_CALIB_DATA stData[PREBOND_T_CALIB_DATA], LONG *p_lAbsoluteLevel)
//{
//	//CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
//	LONG lLevelInLinear = 0;
//	LONG lLevelInRotary	= 0;
//	BOOL bResult = TRUE;
//	{
//		// Transform Reference Level from Motor Count to Linear
//		if (!RotaryToLinearEnc(lRefLevel, stData, &lLevelInLinear))
//		{
//			lLevelInRotary = lRefLevel;
//			bResult = FALSE;
//		}
//		else
//		{
//			lLevelInLinear += DistanceToCount(lOffset, WHZ_CAL_DISTPERCNT);
//
//			if (!LinearToRotaryEnc(lLevelInLinear, stData[WHZ_2], &lLevelInRotary))
//			{
//				lLevelInRotary = lRefLevel;
//				bResult = FALSE;
//			}
//		}
//	}
//	*p_lAbsoluteLevel = lLevelInRotary;
//
//	return bResult;
//}

BOOL CalcWHLinearOffset(LONG lRefLevel, LONG lCurrLevel , WHZ_DATA stData[], LONG *p_lLinearOffset)
{
	LONG lRefLevelInLinear	= 0;
	LONG lCurrLevelInLinear	= 0;
	BOOL bResult = TRUE;

	// Transform Reference Level from Motor Count to Linear
	if (!RotaryToLinearEnc(lRefLevel, stData, &lRefLevelInLinear))
	{
		*p_lLinearOffset = 0;
		return FALSE;
	}
	if (!RotaryToLinearEnc(lCurrLevel, stData, &lCurrLevelInLinear))
	{
		*p_lLinearOffset = 0;
		return FALSE;
	}

	*p_lLinearOffset = DoubleToLong(CountToDistance((DOUBLE)lCurrLevelInLinear - (DOUBLE)lRefLevelInLinear, WHZ_CAL_DISTPERCNT));

	return TRUE;
}


LONG DistanceToCount(DOUBLE dDistance, DOUBLE dDistPerCount) //From um to count
{
	if (dDistPerCount != 0.0)
	{
		return DoubleToLong(dDistance / (dDistPerCount));
	}
	else
	{
		return 0;
	}
}

DOUBLE CountToDistance(DOUBLE dMotorCount, DOUBLE dDistPerCount)
{
	return dMotorCount * (dDistPerCount);
}

LONG AngleToCount(DOUBLE dAngle, DOUBLE dDistPerCount)
{
	if (dDistPerCount != 0.0)
	{
		return DoubleToLong(dAngle / dDistPerCount);
	}
	else
	{
		return 0;
	}
}

DOUBLE	CountToAngle(DOUBLE dMotorCount, DOUBLE dDistPerCount)
{
	if (dDistPerCount != 0.0)
	{
		return dMotorCount * dDistPerCount;
	}
	else
	{
		return 0.0;
	}
}

DOUBLE DistanceInHipec(LONG lDistance, DOUBLE dDistPerCount)
{
	DOUBLE dTemp = (DOUBLE)lDistance * (dDistPerCount / 1000.0);

	return dTemp;
}

DOUBLE VelocityInHipec(LONG lVelocity, DOUBLE dDistPerCount)
{
	DOUBLE dTemp = lVelocity * (dDistPerCount / 1000.0) * HIPEC_FREQ / 64.0;

	return dTemp;
}

LONG HipecInDistance(DOUBLE dHipecDist, DOUBLE dDistPerCount)
{
	LONG lTemp = DoubleToLong(dHipecDist / (dDistPerCount / 1000.0));

	return lTemp;
}

LONG HipecInVelocity(DOUBLE dVelocity, DOUBLE dDistPerCount)
{
	LONG lTemp = DoubleToLong(dVelocity / ((dDistPerCount / 1000.0) * HIPEC_FREQ) * 64.0);

	return lTemp;
}

VOID CalcPosnAfterTransform(D_MTR_POSN dmtrCurrOffset, DOUBLE dCurrDistPerCnt, DOUBLE dResultDistPerCnt, DOUBLE dAngleBtnAxis, D_MTR_POSN *p_dmtrResultOffset)
{
	D_MTR_POSN dmtrTempPosn;
		
	dmtrTempPosn.x = dmtrCurrOffset.x * (dCurrDistPerCnt / dResultDistPerCnt);
	dmtrTempPosn.y = dmtrCurrOffset.y * (dCurrDistPerCnt / dResultDistPerCnt);

	dAngleBtnAxis = (PI / 180.0) * dAngleBtnAxis;

	p_dmtrResultOffset->x = (+dmtrTempPosn.x * cos(dAngleBtnAxis) - dmtrTempPosn.y * sin(dAngleBtnAxis));
	p_dmtrResultOffset->y = (+dmtrTempPosn.x * sin(dAngleBtnAxis) + dmtrTempPosn.y * cos(dAngleBtnAxis));
}

VOID SortTempCalibData(TEMP_DATA stTempCalib[])
{
	INT i, j;
	INT min;

	TEMP_DATA stTmpData[MAX_TEMP_DATA];

	for (i = 0; i < MAX_TEMP_DATA; i++)
	{
		stTmpData[i].SetTemp = stTempCalib[i].SetTemp;
		stTmpData[i].BondTemp = stTempCalib[i].BondTemp;

		stTempCalib[i].SetTemp = 0.0;
		stTempCalib[i].BondTemp = 0.0;
	}

	for (i = 0; i < MAX_TEMP_DATA; i++)
	{
		min = 0;

		for (j = 0; j < MAX_TEMP_DATA; j++)
		{
			if (stTmpData[j].SetTemp != 0)
			{
				if (stTmpData[min].SetTemp == 0.0 || stTmpData[j].SetTemp < stTmpData[min].SetTemp)
				{
					min = j;
				}
			}
		}

		stTempCalib[i].SetTemp = stTmpData[min].SetTemp;
		stTempCalib[i].BondTemp = stTmpData[min].BondTemp;

		stTmpData[min].SetTemp = 0.0;
		stTmpData[min].BondTemp = 0.0;
	}
}

VOID SortForceCalibData(FORCE_DATA stForceCalib[]) //20130416
{
	INT i, j;
	INT min;

	FORCE_DATA stTmpData[MAX_FORCE_DATA];

	for (i = 0; i < MAX_FORCE_DATA; i++)
	{
		stTmpData[i].Force_kgf		= stForceCalib[i].Force_kgf;
		stTmpData[i].Pressure_bar	= stForceCalib[i].Pressure_bar;		
		stTmpData[i].OpenDacValue	= stForceCalib[i].OpenDacValue;

		stForceCalib[i].Pressure_bar	= 0.0;
		stForceCalib[i].Force_kgf		= 0.0;
		stForceCalib[i].OpenDacValue	= 0;
	}

	for (i = 0; i < MAX_FORCE_DATA; i++)
	{
		min = 0;

		for (j = 0; j < MAX_FORCE_DATA; j++)
		{
			if (stTmpData[j].Force_kgf != 0.0)
			{
				if (stTmpData[min].Force_kgf == 0.0 || stTmpData[j].Force_kgf < stTmpData[min].Force_kgf)
				{
					min = j;
				}
			}
		}

		stForceCalib[i].OpenDacValue	= stTmpData[min].OpenDacValue;
		stForceCalib[i].Force_kgf		= stTmpData[min].Force_kgf;

		stTmpData[min].OpenDacValue		= 0;
		stTmpData[min].Force_kgf		= 0.0;
	}
}

BOOL SetTempToBondTemp(DOUBLE dSetTemp, TEMP_DATA stTempCalib[], DOUBLE *p_dBondTemp)
{
	BOOL bStatus = FALSE;

	// stTempCalib[] is empty
	if (stTempCalib[0].SetTemp == 0.0)
	{
		*p_dBondTemp = dSetTemp;
		bStatus = FALSE;
	}
	// stTempCalib[] is not empty
	else
	{
		// min temp
		if (dSetTemp < stTempCalib[0].SetTemp)
		{
			//if (dSetTemp < stTempCalib[0].BondTemp)
			//{
			//	*p_dBondTemp = dSetTemp;
			//}
			//else
			//{
			//	*p_dBondTemp = stTempCalib[0].BondTemp;
			//}
			if (stTempCalib[0].SetTemp != 0)
			{
				*p_dBondTemp = stTempCalib[0].BondTemp 
							   - (stTempCalib[0].SetTemp - dSetTemp) 
							   * (stTempCalib[0].BondTemp - 0.0) 
							   / (stTempCalib[0].SetTemp - 0.0);
			}
			else
			{
				*p_dBondTemp = dSetTemp;
			}
			bStatus = FALSE;
		}
		else
		{
			INT i = 0;

			for (i = 0; i < MAX_TEMP_DATA - 1; i++)
			{
				if (dSetTemp == stTempCalib[i].SetTemp)
				{
					*p_dBondTemp = stTempCalib[i].BondTemp;
					bStatus = TRUE;
					return bStatus;
				}

				if ((dSetTemp > stTempCalib[i].SetTemp) && (dSetTemp < stTempCalib[i + 1].SetTemp))
				{
					*p_dBondTemp = stTempCalib[i + 1].BondTemp 
								   - (stTempCalib[i + 1].SetTemp - dSetTemp) 
								   * (stTempCalib[i + 1].BondTemp - stTempCalib[i].BondTemp) 
								   / (stTempCalib[i + 1].SetTemp - stTempCalib[i].SetTemp);
					bStatus = TRUE;
					return bStatus;
				}

				if ((dSetTemp > stTempCalib[i].SetTemp) && (stTempCalib[i + 1].SetTemp == 0))
				{
					if (i == 0)
					{
						*p_dBondTemp = stTempCalib[i].BondTemp 
									   - (stTempCalib[i].SetTemp - dSetTemp) 
									   * (stTempCalib[i].BondTemp - 0.0) 
									   / (stTempCalib[i].SetTemp - 0.0);
						bStatus = FALSE;
						return bStatus;
					}
					else
					{
						*p_dBondTemp = stTempCalib[i].BondTemp 
									   - (stTempCalib[i].SetTemp - dSetTemp) 
									   * (stTempCalib[i].BondTemp - stTempCalib[i - 1].BondTemp) 
									   / (stTempCalib[i].SetTemp - stTempCalib[i - 1].SetTemp);
						bStatus = FALSE;
						return bStatus;
					}
				}

				if (dSetTemp == stTempCalib[i + 1].SetTemp)
				{
					*p_dBondTemp = stTempCalib[i + 1].BondTemp;
					bStatus = TRUE;
					return bStatus;
				}
			}
			// max Temp and project compare with all 5 Data 
			if (dSetTemp > stTempCalib[MAX_TEMP_DATA - 1].SetTemp)
			{
				*p_dBondTemp = stTempCalib[MAX_TEMP_DATA - 1].BondTemp 
							   - (stTempCalib[MAX_TEMP_DATA - 1].SetTemp - dSetTemp) 
							   * (stTempCalib[MAX_TEMP_DATA - 1].BondTemp - stTempCalib[MAX_TEMP_DATA - 2].BondTemp) 
							   / (stTempCalib[MAX_TEMP_DATA - 1].SetTemp - stTempCalib[MAX_TEMP_DATA - 2].SetTemp);
				bStatus = FALSE;
				return bStatus;
			}
		}
	}

	return bStatus;
}

BOOL BondTempToSetTemp(DOUBLE dBondTemp, TEMP_DATA stTempCalib[], DOUBLE *p_dSetTemp)
{
	BOOL bStatus = FALSE;

	// stTempCalib[] is empty
	if (stTempCalib[0].SetTemp == 0.0)
	{
		*p_dSetTemp = dBondTemp;
		bStatus = FALSE;
	}
	// stTempCalib[] is not empty
	else
	{
		// min temp
		if (dBondTemp < stTempCalib[0].BondTemp)
		{
			//if (dBondTemp < stTempCalib[0].SetTemp)
			//{
			//	*p_dSetTemp = dBondTemp;
			//}
			//else
			//{
			//	*p_dSetTemp = stTempCalib[0].SetTemp;
			//}

			if (stTempCalib[0].BondTemp != 0)
			{
				*p_dSetTemp = stTempCalib[0].SetTemp 
							  - (stTempCalib[0].BondTemp - dBondTemp) 
							  * (stTempCalib[0].SetTemp - 0.0) 
							  / (stTempCalib[0].BondTemp - 0.0);
			}
			else
			{
				*p_dSetTemp = dBondTemp;
			}
			bStatus = FALSE;
		}
		else
		{
			INT i = 0;

			for (i = 0; i < MAX_TEMP_DATA - 1; i++)
			{
				if (dBondTemp == stTempCalib[i].BondTemp)
				{
					*p_dSetTemp = stTempCalib[i].SetTemp;
					bStatus = TRUE;
					break;
				}

				if ((dBondTemp > stTempCalib[i].BondTemp) && (dBondTemp < stTempCalib[i + 1].BondTemp))
				{
					*p_dSetTemp = stTempCalib[i + 1].SetTemp 
								  - (stTempCalib[i + 1].BondTemp - dBondTemp) 
								  * (stTempCalib[i + 1].SetTemp - stTempCalib[i].SetTemp) 
								  / (stTempCalib[i + 1].BondTemp - stTempCalib[i].BondTemp);
					bStatus = TRUE;
					break;
				}

				if ((dBondTemp > stTempCalib[i].BondTemp) && (stTempCalib[i + 1].BondTemp == 0))
				{
					if (i == 0)
					{
						*p_dSetTemp = stTempCalib[i].SetTemp 
									  - (stTempCalib[i].BondTemp - dBondTemp) 
									  * (stTempCalib[i].SetTemp - 0.0) 
									  / (stTempCalib[i].BondTemp - 0.0);
						bStatus = FALSE;
						break;
					}
					else
					{
						*p_dSetTemp = stTempCalib[i].SetTemp 
									  - (stTempCalib[i].BondTemp - dBondTemp) 
									  * (stTempCalib[i].SetTemp - stTempCalib[i - 1].SetTemp) 
									  / (stTempCalib[i].BondTemp - stTempCalib[i - 1].BondTemp);
						bStatus = FALSE;
						break;
					}
				}

				if (dBondTemp == stTempCalib[i + 1].BondTemp)
				{
					*p_dSetTemp = stTempCalib[i + 1].SetTemp;
					bStatus = TRUE;
					break;
				}
			}
			// max Temp and project compare with all 5 Data 
			if (dBondTemp > stTempCalib[MAX_TEMP_DATA - 1].BondTemp)
			{
				*p_dSetTemp = stTempCalib[MAX_TEMP_DATA - 1].SetTemp 
							  - (stTempCalib[MAX_TEMP_DATA - 1].BondTemp - dBondTemp) 
							  * (stTempCalib[MAX_TEMP_DATA - 1].SetTemp - stTempCalib[MAX_TEMP_DATA - 2].SetTemp) 
							  / (stTempCalib[MAX_TEMP_DATA - 1].BondTemp - stTempCalib[MAX_TEMP_DATA - 2].BondTemp);
				bStatus = FALSE;
				return bStatus;
			}

		}
	}

	return bStatus;
}

LSITypeNum GetLSITypeNum(HandOverStatusNum lHandOverStatusNum)
{
	switch (lHandOverStatusNum)
	{
	case HANDOVER_LSI_TYPE1_SH1:
	case HANDOVER_LSI_TYPE1_SH2:
		return LSI_TYPE1;

	case HANDOVER_LSI_TYPE2_SH1:
	case HANDOVER_LSI_TYPE2_SH2:
		return LSI_TYPE2;

	default:
		return NO_LSI_TYPE;
	}
}

ShuttleUnitNum GetShuttleUnit(HandOverStatusNum lHandOverStatusNum)
{
	switch (lHandOverStatusNum)
	{
	case HANDOVER_LSI_TYPE1_SH1:
	case HANDOVER_LSI_TYPE2_SH1:
		return SHUTTLE_1;

	case HANDOVER_LSI_TYPE1_SH2:
	case HANDOVER_LSI_TYPE2_SH2:
		return SHUTTLE_2;

	default:
		return SHUTTLE_NONE;
	}
}

BOOL SetHandOverStatus(LSITypeNum lLSIType, ShuttleUnitNum lShuttleUnit, HandOverStatusNum *lHandOverStatus) 
{
	LONG  lTemp;
	lTemp = lShuttleUnit + lLSIType * (MAX_NUM_OF_LSI_TYPE);

	if (
		lTemp >= MAX_NUM_OF_HANDOVER || 
		lTemp <= NO_HANDOVER
	   )
	{
		return FALSE;
	}
	else
	{
		*lHandOverStatus = (HandOverStatusNum)lTemp;
		return TRUE;
	}
}

STATUS_TYPE SetPickLSIStatus(HandOverStatusNum lHandOverStatus)
{
	if (lHandOverStatus == HANDOVER_LSI_TYPE1_SH1)
	{
		return ST_REQ_PICK_TYPE1_SH1;
	}
	else if (lHandOverStatus == HANDOVER_LSI_TYPE1_SH2)
	{
		return ST_REQ_PICK_TYPE1_SH2;
	}
	else if (lHandOverStatus == HANDOVER_LSI_TYPE2_SH1)
	{
		return ST_REQ_PICK_TYPE2_SH1;
	}
	else if (lHandOverStatus == HANDOVER_LSI_TYPE2_SH2)
	{
		return ST_REQ_PICK_TYPE2_SH2;
	}

	return ST_ERROR;
}

STATUS_TYPE SetPlaceLSIStatus(HandOverStatusNum lHandOverStatus)
{
	if (lHandOverStatus == HANDOVER_LSI_TYPE1_SH1)
	{
		return ST_REQ_PLACE_TYPE1_SH1;
	}
	else if (lHandOverStatus == HANDOVER_LSI_TYPE1_SH2)
	{
		return ST_REQ_PLACE_TYPE1_SH2;
	}
	else if (lHandOverStatus == HANDOVER_LSI_TYPE2_SH1)
	{
		return ST_REQ_PLACE_TYPE2_SH1;
	}
	else if (lHandOverStatus == HANDOVER_LSI_TYPE2_SH2)
	{
		return ST_REQ_PLACE_TYPE2_SH2;
	}

	return ST_ERROR;
}

HandOverStatusNum GetHandOverStatus(STATUS_TYPE stStationStatus)
{
	if (stStationStatus == ST_REQ_PLACE_TYPE1_SH1)
	{
		return HANDOVER_LSI_TYPE1_SH1;
	}
	else if (stStationStatus == ST_REQ_PLACE_TYPE1_SH2)
	{
		return HANDOVER_LSI_TYPE1_SH2;
	}
	else if (stStationStatus == ST_REQ_PLACE_TYPE2_SH1)
	{
		return HANDOVER_LSI_TYPE2_SH1;
	}	
	else if (stStationStatus == ST_REQ_PLACE_TYPE2_SH2)
	{
		return HANDOVER_LSI_TYPE2_SH2;
	}

	return NO_HANDOVER;
}

LONG GetMaxValue(LONG lValue1, LONG lValue2, LONG lValue3, LONG lValue4)
{
	LONG lMaxValue = lValue1;

	if (lValue2 > lMaxValue)
	{
		lMaxValue = lValue2;
	}

	if (lValue3 > lMaxValue)
	{
		lMaxValue = lValue3;
	}

	if (lValue4 > lMaxValue)
	{
		lMaxValue = lValue4;
	}

	return lMaxValue;
}


INT GetNextLSI(LONG lCurrLSI, LSI_ON_GLASS_INFO	stLSI[])
{
	INT i = lCurrLSI;

	for (i = lCurrLSI; i < MAX_NUM_OF_LSI; i++)
	{
		if (stLSI[i].bEnable)
		{
			if (!stLSI[i].bBonded)
			{
				// Got the m_lCurLSI to Bond
				break;
			}
			else
			{
				// Find the Next LSI
				i = (i + 1) % MAX_NUM_OF_LSI;
			}
		}
		else
		{
			break;
		}
	}

	if (!stLSI[i].bEnable)
	{
		// Error
		return NO_LSI;
	}
	else
	{
		return i;
	}
}

INT	SrchNextEdge(LONG lCurrEdge, EdgeStatus stEdgeStatus[])
{
	INT nResult			= EDGE_NONE;

	INT nIndex			= 0;
	INT nCW_Neighbour	= (lCurrEdge + nIndex)%MAX_EDGE_OF_GLASS;
	INT	nCCW_Neighbour	= (lCurrEdge + (MAX_EDGE_OF_GLASS - nIndex))%MAX_EDGE_OF_GLASS;
	BOOL bCheckCCW		= FALSE;

	do
	{
		// Set the Edge to Search
		nCW_Neighbour	= (lCurrEdge + nIndex)%MAX_EDGE_OF_GLASS;
		nCCW_Neighbour	= (lCurrEdge + (MAX_EDGE_OF_GLASS - nIndex))%MAX_EDGE_OF_GLASS;

		// Check Next Edge in Clockwise
		if (stEdgeStatus[nCW_Neighbour].bUsed)
		{
			if (!stEdgeStatus[nCW_Neighbour].bBonded)
			{
				// Got the Next Edge
				nResult = nCW_Neighbour;
				break;
			}
			else
			{				
				bCheckCCW = TRUE;
			}
		}
		else
		{
			bCheckCCW = TRUE;
		}

		// Check Next Edge in CounterClockwise
		if (bCheckCCW)
		{
			if (stEdgeStatus[nCCW_Neighbour].bUsed)
			{
				if (!stEdgeStatus[nCCW_Neighbour].bBonded)
				{
					// Got the Next Edge
					nResult = nCCW_Neighbour;
					break;
				}
			}
		}
		
		nIndex++;

	}	while (nIndex <= DoubleToLong(MAX_EDGE_OF_GLASS/2.0));	// Prevent Dead Looping

	if (nIndex > DoubleToLong(MAX_EDGE_OF_GLASS/2.0))
	{
		return EDGE_NONE;
	}

	return nResult;
}

INT	GetNextEdge(LONG lCurrEdge, ACF_INFO stACF[])
{
	// Define Clockwise & Anti Clockwise Neighbour Edge
	// Make Decision on Closest Next Edge (Cloclwise Neighbour for first Priority)

	INT nResult			= EDGE_NONE;
	INT nNumOfEdge		= 0;

	INT	i = 0;
	EdgeStatus stEdgeStatus[MAX_EDGE_OF_GLASS];

	for (i = 0; i < MAX_EDGE_OF_GLASS; i++)
	{
		stEdgeStatus[i].bUsed	= FALSE;
		stEdgeStatus[i].bBonded	= FALSE;
	}

	// Set the Edge Status 
	// Assume the Bonding ACF/LSI order is Ascending
	for (i = ACF1; i < MAX_NUM_OF_ACF; i++)
	{
		if (stACF[i].bEnable)
		{
			switch (stACF[i].lEdgeInfo)
			{
			case EDGE_A:
				stEdgeStatus[EDGE_A].bUsed	= TRUE;
				stEdgeStatus[EDGE_A].bBonded = stACF[i].bBonded;
				break;

			//case EDGE_B:
			//	stEdgeStatus[EDGE_B].bUsed	= TRUE;
			//	stEdgeStatus[EDGE_B].bBonded = stACF[i].bBonded;
			//	break;

			//case EDGE_C:
			//	stEdgeStatus[EDGE_C].bUsed	= TRUE;
			//	stEdgeStatus[EDGE_C].bBonded = stACF[i].bBonded;
			//	break;

			//case EDGE_D:
			//	stEdgeStatus[EDGE_D].bUsed	= TRUE;
			//	stEdgeStatus[EDGE_D].bBonded = stACF[i].bBonded;
			//	break;
			}
		}
	}

	return SrchNextEdge(lCurrEdge, stEdgeStatus);
}

/*
INT	GetNextEdge(LONG lCurrEdge, LSI_ON_GLASS_INFO stLSI[])
{
	// Define Clockwise & Anti Clockwise Neighbour Edge
	// Make Decision on Closest Next Edge (Cloclwise Neighbour for first Priority)

	INT nResult			= EDGE_NONE;

	INT	i = 0;
	EdgeStatus stEdgeStatus[MAX_EDGE_OF_GLASS];

	for (i = 0; i < MAX_EDGE_OF_GLASS; i++)
	{
		stEdgeStatus[i].bUsed	= FALSE;
		stEdgeStatus[i].bBonded	= FALSE;
	}

	// Set the Edge Status 
	// Assume the Bonding ACF/LSI order is Ascending
	for (i = LSI1; i < MAX_NUM_OF_LSI; i++)
	{
		if (stLSI[i].bEnable)
		{
			switch (stLSI[i].lGlassInfo)
			{
			case EDGE_A:
				stEdgeStatus[EDGE_A].bUsed	= TRUE;
				stEdgeStatus[EDGE_A].bBonded = stLSI[i].bBonded;
				break;

			case EDGE_B:
				stEdgeStatus[EDGE_B].bUsed	= TRUE;
				stEdgeStatus[EDGE_B].bBonded = stLSI[i].bBonded;
				break;

			case EDGE_C:
				stEdgeStatus[EDGE_C].bUsed	= TRUE;
				stEdgeStatus[EDGE_C].bBonded = stLSI[i].bBonded;
				break;

			case EDGE_D:
				stEdgeStatus[EDGE_D].bUsed	= TRUE;
				stEdgeStatus[EDGE_D].bBonded = stLSI[i].bBonded;
				break;
			}
		}
		else
		{
			break;
		}
	}

	return SrchNextEdge(lCurrEdge, stEdgeStatus);
}

INT	GetNextEdge(LONG lCurrEdge, MB_INFO	stMB[])
{
	// Define Clockwise & Anti Clockwise Neighbour Edge
	// Make Decision on Closest Next Edge (Cloclwise Neighbour for first Priority)

	INT nResult			= EDGE_NONE;

	INT	i = 0;
	EdgeStatus stEdgeStatus[MAX_EDGE_OF_GLASS];

	for (i = 0; i < MAX_EDGE_OF_GLASS; i++)
	{
		stEdgeStatus[i].bUsed	= FALSE;
		stEdgeStatus[i].bBonded	= FALSE;
	}

	// Set the Edge Status 
	// Assume the Bonding ACF/LSI order is Ascending
	for (i = LSI1; i < MAX_NUM_OF_LSI; i++)
	{
		if (stMB[i].bEnable)
		{
			switch (stMB[i].lGlassInfo)
			{
			case EDGE_A:
				stEdgeStatus[EDGE_A].bUsed	= TRUE;
				stEdgeStatus[EDGE_A].bBonded = stMB[i].bBonded;
				break;

			case EDGE_B:
				stEdgeStatus[EDGE_B].bUsed	= TRUE;
				stEdgeStatus[EDGE_B].bBonded = stMB[i].bBonded;
				break;

			case EDGE_C:
				stEdgeStatus[EDGE_C].bUsed	= TRUE;
				stEdgeStatus[EDGE_C].bBonded = stMB[i].bBonded;
				break;

			case EDGE_D:
				stEdgeStatus[EDGE_D].bUsed	= TRUE;
				stEdgeStatus[EDGE_D].bBonded = stMB[i].bBonded;
				break;
			}
		}
		else
		{
			break;
		}
	}

	return SrchNextEdge(lCurrEdge, stEdgeStatus);
}

INT	GetNextEdge(LONG lCurrEdge, PRESS_INFO	stPRESS[])
{
	// Define Clockwise & Anti Clockwise Neighbour Edge
	// Make Decision on Closest Next Edge (Cloclwise Neighbour for first Priority)

	INT nResult			= EDGE_NONE;

	INT	i = 0;
	EdgeStatus stEdgeStatus[MAX_EDGE_OF_GLASS];

	for (i = 0; i < MAX_EDGE_OF_GLASS; i++)
	{
		stEdgeStatus[i].bUsed	= FALSE;
		stEdgeStatus[i].bBonded	= FALSE;
	}

	// Set the Edge Status 
	// Assume the Bonding ACF/LSI order is Ascending
	for (i = LSI1; i < MAX_NUM_OF_LSI; i++)
	{
		if (stPRESS[i].bEnable)
		{
			switch (stPRESS[i].lGlassInfo)
			{
			case EDGE_A:
				stEdgeStatus[EDGE_A].bUsed	= TRUE;
				stEdgeStatus[EDGE_A].bBonded = stPRESS[i].bBonded;
				break;

			case EDGE_B:
				stEdgeStatus[EDGE_B].bUsed	= TRUE;
				stEdgeStatus[EDGE_B].bBonded = stPRESS[i].bBonded;
				break;

			case EDGE_C:
				stEdgeStatus[EDGE_C].bUsed	= TRUE;
				stEdgeStatus[EDGE_C].bBonded = stPRESS[i].bBonded;
				break;

			case EDGE_D:
				stEdgeStatus[EDGE_D].bUsed	= TRUE;
				stEdgeStatus[EDGE_D].bBonded = stPRESS[i].bBonded;
				break;
			}
		}
		else
		{
			break;
		}
	}

	return SrchNextEdge(lCurrEdge, stEdgeStatus);
}

INT SrchLastEdge(LONG lFirstEdge, EdgeStatus stEdgeStatus[])
{
	INT nResult			= EDGE_NONE;
	LONG lCurrEdge		= lFirstEdge;

	INT i = 0;

	//for (i = 0; i < MAX_EDGE_OF_GLASS; i++)
	//{
	//	stEdgeStatus[i].bUsed	= FALSE;
	//	stEdgeStatus[i].bBonded	= FALSE;
	//}

	stEdgeStatus[lFirstEdge].bBonded = TRUE;

	if (lFirstEdge == SrchNextEdge(lFirstEdge, stEdgeStatus))
	{
		nResult = lFirstEdge;
	}
	else
	{
		do
		{
			nResult		= lCurrEdge;
			stEdgeStatus[lCurrEdge].bBonded = TRUE;
			lCurrEdge	= SrchNextEdge(lCurrEdge, stEdgeStatus);

			// End of Looping
			if (lCurrEdge == EDGE_NONE)
			{
				break;
			}
			
		} while (lCurrEdge != lFirstEdge);
	}

	return nResult;
}

INT GetLastEdge(LONG lFirstEdge, ACF_INFO stACF[])
{
	INT nResult			= EDGE_NONE;

	LONG lCurrEdge		= lFirstEdge;

	EdgeStatus stEdgeStatus[MAX_EDGE_OF_GLASS];

	INT i = 0;

	for (i = 0; i < MAX_EDGE_OF_GLASS; i++)
	{
		stEdgeStatus[i].bUsed	= FALSE;
		stEdgeStatus[i].bBonded	= FALSE;
	}

	// Set the Edge Status 
	// Assume the Bonding ACF/LSI order is Ascending
	for (i = ACF1; i < MAX_NUM_OF_ACF; i++)
	{
		if (stACF[i].bEnable)
		{
			switch (stACF[i].lEdgeInfo)
			{
			case EDGE_A:
				stEdgeStatus[EDGE_A].bUsed	= TRUE;
				break;

			case EDGE_B:
				stEdgeStatus[EDGE_B].bUsed	= TRUE;
				break;

			case EDGE_C:
				stEdgeStatus[EDGE_C].bUsed	= TRUE;
				break;

			case EDGE_D:
				stEdgeStatus[EDGE_D].bUsed	= TRUE;
				break;
			}
		}
	}

	return SrchLastEdge(lFirstEdge, stEdgeStatus);
}

INT GetLastEdge(LONG lFirstEdge, LSI_ON_GLASS_INFO stLSI[])
{
	INT nResult			= EDGE_NONE;

	LONG lCurrEdge		= lFirstEdge;

	EdgeStatus stEdgeStatus[MAX_EDGE_OF_GLASS];

	INT i = 0;

	for (i = 0; i < MAX_EDGE_OF_GLASS; i++)
	{
		stEdgeStatus[i].bUsed	= FALSE;
		stEdgeStatus[i].bBonded	= FALSE;
	}

	// Set the Edge Status 
	// Assume the Bonding ACF/LSI order is Ascending
	for (i = LSI1; i < MAX_NUM_OF_LSI; i++)
	{
		if (stLSI[i].bEnable)
		{
			switch (stLSI[i].lGlassInfo)
			{
			case EDGE_A:
				stEdgeStatus[EDGE_A].bUsed	= TRUE;
				break;

			case EDGE_B:
				stEdgeStatus[EDGE_B].bUsed	= TRUE;
				break;

			case EDGE_C:
				stEdgeStatus[EDGE_C].bUsed	= TRUE;
				break;

			case EDGE_D:
				stEdgeStatus[EDGE_D].bUsed	= TRUE;
				break;
			}
		}
	}
	
	return SrchLastEdge(lFirstEdge, stEdgeStatus);
}
*/

INT GetFirstUnit(LONG lCurrEdge, ACF_INFO	stACF[])
{
	INT i;

	for( i = 0; i< MAX_NUM_OF_ACF; i++)
	{
		if(stACF[i].bEnable && stACF[i].lEdgeInfo == lCurrEdge) // To find the first unit on this edge
		{
			return i;
		}
	}
	
	return NO_ACF;
}

/*
INT GetFirstUnit(LONG lEdge, LSI_ON_GLASS_INFO	stLSI[])
{
	INT i;

	for( i = 0; i< MAX_NUM_OF_LSI; i++)
	{
		if(stLSI[i].bEnable && stLSI[i].lGlassInfo == lEdge) // To find the first unit on this edge
		{
			return i;
		}
	}
	
	return NO_LSI;
}

INT GetFirstUnit(LONG lEdge, MB_INFO	stMB[])
{
	INT i;

	for( i = 0; i< MAX_NUM_OF_LSI; i++)
	{
		if(stMB[i].bEnable && stMB[i].lGlassInfo == lEdge) // To find the first unit on this edge
		{
			return i;
		}
	}
	
	return NO_LSI;
}

INT GetFirstPress(LONG lEdge, PRESS_INFO	stPRESS[])
{
	INT i;

	for( i = 0; i< MAX_NUM_OF_LSI; i++)
	{
		if(stPRESS[i].bEnable && stPRESS[i].lGlassInfo == lEdge) // To find the first unit on this edge
		{
			return i;
		}
	}
	
	return NO_LSI;
}

*/

INT GetNextACF(LONG lCurrACF, ACF_INFO	stACF[])
{
	INT i = 0;
	LONG lTempEdge = GetNextEdge(stACF[lCurrACF].lEdgeInfo, stACF);

	if (stACF[lCurrACF].lEdgeInfo != lTempEdge)
	{
		// Assign the m_lCurLSI starting from the first unit on Edge
		if ((i = GetFirstUnit((EdgeNum)lTempEdge, stACF)) == NO_ACF)
		{
			// Errorr
			return NO_ACF;
		}
	}

	do
	{
		if (stACF[i].bEnable)
		{
			if (!stACF[i].bBonded)
			{
				// Got the m_lCurLSI to Bond
				break;
			}
			else
			{
				// Find the Next LSI
				i = (i + 1)%MAX_NUM_OF_ACF;
			}
		}
		else
		{
			break;
		}
	
	} while(lTempEdge == stACF[i].lEdgeInfo);

	if (
		!stACF[i].bEnable ||
		lTempEdge != stACF[i].lEdgeInfo
		)
	{
		// Error
		return NO_ACF;
	}
	else
	{
		return i;
	}
}

/*
INT GetNextLSI(LONG lCurrLSI, LSI_ON_GLASS_INFO	stLSI[])
{
	LONG lTempEdge = GetNextEdge(stLSI[lCurrLSI].lGlassInfo, stLSI);
	INT i = lCurrLSI;

	if (stLSI[lCurrLSI].lGlassInfo != lTempEdge)
	{
		// Assign the m_lCurLSI starting from the first unit on Edge
		if ((i = GetFirstUnit((EdgeNum)lTempEdge, stLSI)) == NO_LSI)
		{
			// Errorr
			return NO_LSI;
		}
	}

	do
	{
		if (stLSI[i].bEnable)
		{
			if (!stLSI[i].bBonded)
			{
				// Got the m_lCurLSI to Bond
				break;
			}
			else
			{
				// Find the Next LSI
				i = (i + 1)%MAX_NUM_OF_LSI;
			}
		}
		else
		{
			break;
		}
	
	} while(lTempEdge == stLSI[i].lGlassInfo);

	if (
		!stLSI[i].bEnable ||
		lTempEdge != stLSI[i].lGlassInfo
		)
	{
		// Error
		return NO_LSI;
	}
	else
	{
		return i;
	}
}

*/


VOID SetMBPRESSInfo(STATUS_TYPE GLASS_Status[], PRESS_INFO *stMBPRESS)
{
	//for (INT i = 0; i < MAX_NUM_OF_LSI; i++)
	//{
	//	stMBPRESS[i].bEnable = g_stLSI[i].bEnable; //20120807
	//	if (GLASS_Status[GLASS1] == ST_READY && GLASS_Status[GLASS2] == ST_READY)
	//	{
	//		stMBPRESS[i].lPressMode						= DUAL_PRESS;
	//	}
	//	else if (GLASS_Status[GLASS1] == ST_READY && (GLASS_Status[GLASS2] == ST_ERROR || GLASS_Status[GLASS2] == ST_EMPTY))
	//	{
	//		stMBPRESS[i].lPressMode						= SINGLE_LEFT_PRESS;
	//	}
	//	else if ((GLASS_Status[GLASS1] == ST_ERROR || GLASS_Status[GLASS1] == ST_EMPTY) && GLASS_Status[GLASS2] == ST_READY)
	//	{
	//		stMBPRESS[i].lPressMode						= SINGLE_RIGHT_PRESS;
	//	}
	//	else  // Both Error Glass
	//	{	
	//		stMBPRESS[i].bEnable						= FALSE;	
	//		stMBPRESS[i].lPressMode						= DUAL_PRESS;
	//	}
	//}
}


LONG SetGlassStatus(STATUS_TYPE StationStatus, STATUS_TYPE *pGLASS_Status)
{
	switch (StationStatus)
	{
	case ST_PLACE_READY1_EMPTY2:
		pGLASS_Status[GLASS1] = ST_READY;
	//	pGLASS_Status[GLASS2] = ST_EMPTY;
		break;

	case ST_PLACE_READY1_ERR2:
		pGLASS_Status[GLASS1] = ST_READY;
	//	pGLASS_Status[GLASS2] = ST_ERROR;
		break;

	case ST_PLACE_EMPTY1_READY2:
		pGLASS_Status[GLASS1] = ST_EMPTY;
	//	pGLASS_Status[GLASS2] = ST_READY;
		break;

	case ST_PLACE_EMPTY1_ERR2:
		pGLASS_Status[GLASS1] = ST_EMPTY;
	//	pGLASS_Status[GLASS2] = ST_ERROR;
		break;

	case ST_PLACE_ERR1_READY2:
		pGLASS_Status[GLASS1] = ST_ERROR;
	//	pGLASS_Status[GLASS2] = ST_READY;
		break;

	case ST_PLACE_ERR1_ERR2:
		pGLASS_Status[GLASS1] = ST_ERROR;
	//	pGLASS_Status[GLASS2] = ST_ERROR;
		break;

	case ST_PLACE_ERR1_EMPTY2:
		pGLASS_Status[GLASS1] = ST_ERROR;
	//	pGLASS_Status[GLASS2] = ST_EMPTY;
		break;

	case ST_PLACE_READY1_READY2:
		pGLASS_Status[GLASS1] = ST_READY;
	//	pGLASS_Status[GLASS2] = ST_READY;
		break;

	default:
		return GMP_FAIL;
	}

	return GMP_SUCCESS;
}

LONG SetStationStatus(STATUS_TYPE GLASS_Status[])
{
	LONG lTempStatus = ST_PICK_EMPTY1_EMPTY2;

	for (INT i = GLASS1; i < MAX_NUM_OF_GLASS; i++)
	{
		switch (GLASS_Status[i])
		{
		case ST_READY:
		case ST_READY_1_LEFT:
		case ST_READY_1_RIGHT:
		case ST_READY_2_LEFT:
		case ST_READY_2_RIGHT:
		case ST_READY_1:
		case ST_READY_2:
			if (i == GLASS1)
			{
				lTempStatus = ST_PICK_READY1_READY2;
			}
			else
			{
				lTempStatus += 0;
			}
			break;

		case ST_ERROR:
			if (i == GLASS1)
			{
				lTempStatus = ST_PICK_ERR1_READY2;
			}
			else
			{
				lTempStatus += 1;
			}
			break;

		case ST_EMPTY:
			if (i == GLASS1)
			{
				lTempStatus = ST_PICK_EMPTY1_READY2;
			}
			else
			{
				lTempStatus += 2;
			}
			break;

		default:
			return ST_ERROR;
		}
	}

	return lTempStatus;

}

/*

CString GetStatusString(STATUS_TYPE stStatus)
{
	switch (stStatus)
	{
	case ST_IDLE:
		return "ST_IDLE";

	case ST_READY:
		return "ST_READY";

	case ST_READY_1:
		return "ST_READY_1";

	case ST_READY_2:
		return "ST_READY_2";

	case ST_BUSY:
		return "ST_BUSY";

	case ST_ACK:
		return "ST_ACK";

	case ST_READY_LOAD:
		return "ST_READY_LOAD";

	case ST_REQ_ACF:
		return "ST_REQ_ACF";

	case ST_DONE:
		return "ST_DONE";

	case ST_EMPTY:
		return "ST_EMPTY";

	case ST_ACK_ACF_WH:
		return "ST_ACK_ACF_WH";

	case ST_REQ_TO_BOND_POSN:
		return "ST_REQ_TO_BOND_POSN";

	case ST_ACK_TO_BOND_POSN:
		return "ST_ACK_TO_BOND_POSN";

	case ST_REQ_BONDING:
		return "ST_REQ_BONDING";

	case ST_REQ_BONDING1:
		return "ST_REQ_BONDING1";

	case ST_REQ_BONDING2:
		return "ST_REQ_BONDING2";

	case ST_BONDING:
		return "ST_BONDING";

	case ST_BOND_COMPLETED:
		return "ST_BOND_COMPLETED";

	case ST_REQ_COLLECT_LSI:
		return "ST_REQ_COLLECT_LSI";

	case ST_ACK_COLLECT_LSI:
		return "ST_ACK_COLLECT_LSI";

	case ST_REQ_QUARTZ:
		return "ST_REQ_QUARTZ";

	case ST_ACK_REQ_QUARTZ_1:
		return "ST_ACK_REQ_QUARTZ_1";

	case ST_ACK_REQ_QUARTZ_2:
		return "ST_ACK_REQ_QUARTZ_2";

	case ST_REQ_LSI:
		return "ST_REQ_LSI";

	case ST_REQ_TYPE2:
		return "ST_REQ_TYPE2";

	case ST_ACK_REQ_LSI:
		return "ST_ACK_REQ_LSI";

	case ST_ACK_REQ_TYPE1:
		return "ST_ACK_REQ_TYPE1";

	case ST_ACK_REQ_TYPE2:
		return "ST_ACK_REQ_TYPE2";

	case ST_PR_IN_PROCESS:
		return "ST_PR_IN_PROCESS";

	case ST_VAC_ERROR:
		return "ST_VAC_ERROR";

	case ST_PR_ERROR:
		return "ST_PR_ERROR";

	case ST_ERROR:
		return "ST_ERROR";

	case ST_ACK_SHUTTLE:
		return "ST_ACK_SHUTTLE";

	case ST_REQ_PLACE_LSI:
		return "ST_REQ_PLACE_LSI";

	case ST_REQ_PLACE_TYPE1:
		return "ST_REQ_PLACE_TYPE1";

	case ST_REQ_PLACE_TYPE2:
		return "ST_REQ_PLACE_TYPE2";

	case ST_ACK_CHIPPICKARM:
		return "ST_ACK_CHIPPICKARM";

	case ST_PICK_GLASS:
		return "ST_PICK_GLASS";

	case ST_RELEASE_GLASS:
		return "ST_RELEASE_GLASS";

	case ST_REQ_BLOW_LSI:
		return "ST_REQ_BLOW_LSI";

	case ST_ACK_BLOW_LSI:
		return "ST_ACK_BLOW_LSI";

	case ST_ACK_PICK_TYPE1:
		return "ST_ACK_PICK_TYPE1";

	case ST_ACK_PICK_TYPE2:
		return "ST_ACK_PICK_TYPE2";

	case ST_PICK_ERROR:
		return "ST_PICK_ERROR";

	case ST_HOLD_GLASS:
		return "ST_HOLD_GLASS";

	case ST_CHECK_GLASS:
		return "ST_CHECK_GLASS";

	case ST_PR_ERROR_TYPE1:
		return "ST_PR_ERROR_TYPE1";

	case ST_PR_ERROR_TYPE2:
		return "ST_PR_ERROR_TYPE2";

	case ST_READY_1_LEFT:
		return "ST_READY_1_LEFT";

	case ST_READY_1_RIGHT:
		return "ST_READY_1_RIGHT";

	case ST_READY_2_LEFT:
		return "ST_READY_2_LEFT";

	case ST_READY_2_RIGHT:
		return "ST_READY_2_RIGHT";

	case ST_CHECK_1:
		return "ST_CHECK_1";

	case ST_CHECK_2:
		return "ST_CHECK_2";

	case ST_READY_1_READY2:
		return "ST_READY_1_READY2";

	case ST_READY_1_ERR2:
		return "ST_READY_1_ERR2";

	case ST_READY_1_EMPTY2:
		return "ST_READY_1_EMPTY2";

	case ST_ERR1_READY2:
		return "ST_ERR1_READY2";

	case ST_ERR1_ERR2:
		return "ST_ERR1_ERR2";

	case ST_ERR1_EMPTY2:
		return "ST_ERR1_EMPTY2";

	case ST_EMPTY1_READY2:
		return "ST_EMPTY1_READY2";

	case ST_EMPTY1_ERR2:
		return "ST_EMPTY1_ERR2";

	case ST_EMPTY1_EMPTY2:
		return "ST_EMPTY1_EMPTY2";

	case ST_CHECK1_CHECK2:
		return "ST_CHECK1_CHECK2";

	case ST_CHECK1_ERR2:
		return "ST_CHECK1_ERR2";

	case ST_CHECK1_EMPTY2:
		return "ST_CHECK1_EMPTY2";

	case ST_ERR1_CHECK2:
		return "ST_ERR1_CHECK2";

	case ST_EMPTY1_CHECK2:
		return "ST_EMPTY1_CHECK2";

	case ST_STOP:
		return "ST_STOP";

	case ST_MOD_NOT_SELECTED:
		return "ST_MOD_NOT_SELECTED";
	}

	return "ASSASSINATE JASON";
}
*/

VOID CalcDataMean(DATA_SET *p_stDataSet)
{
	INT		i = 0;
	DOUBLE	dDataSum = 0.0;

	for (i = 0; i < MAX_RECORD_DATA; i++)
	{
		dDataSum += p_stDataSet->dDataSet[i];
	}

	p_stDataSet->dMean = dDataSum / MAX_RECORD_DATA;
}

VOID CalcDataRange(DATA_SET *p_stDataSet)
{
	INT i = 0;

	for (i = 0; i < MAX_RECORD_DATA; i++)
	{
		if (p_stDataSet->dDataSet[i] < p_stDataSet->dMin)
		{
			p_stDataSet->dMin = p_stDataSet->dDataSet[i];
		}
		else if (p_stDataSet->dDataSet[i] > p_stDataSet->dMax)
		{
			p_stDataSet->dMax = p_stDataSet->dDataSet[i];
		}
	}
}


BOOL CopyDir(const CString szSource, const CString szDest)
{
	CFileOperation fo;
	
	fo.SetOverwriteMode(TRUE);

	if (!fo.CheckPath(szSource))
	{
		//fo.ShowError();
		return FALSE;
	}
	
	//if (!fo.CheckPath(szDest))
	//{
	//	CreateDirectory(szDest, NULL);
	//	return FALSE;
	//}

	if (!fo.Copy(szSource, szDest))
	{
		fo.ShowError();
		return FALSE;
	}

	return TRUE;
}

BOOL DeleteDir(const CString szDest)
{
	CFileOperation fo;
	
	fo.SetOverwriteMode(TRUE);

	if (!fo.Delete(szDest))
	{
		//fo.ShowError();
		return FALSE;
	}

	return TRUE;
}

BOOL CheckPath(const CString szDest)
{
	CFileOperation fo;
	
	if (!fo.CheckPath(szDest))
	{
		//fo.ShowError();
		return FALSE;
	}

	return TRUE;
	
}

VOID CreatePath(const CString szPath)
{
	if (CheckPath(szPath) == PATH_NOT_FOUND) 
	{
		CreateDirectory(szPath, NULL);
	}
}

BOOL RenameDir(const CString szSource, const CString szDest)
{
	CFileOperation fo;
	
	fo.SetOverwriteMode(TRUE);

	if (!fo.Rename(szSource, szDest))
	{
		//fo.ShowError();
		return FALSE;
	}

	return TRUE;
}


BOOL Test(PRESS_INFO *PRESSValue)
{
	PRESS_INFO stTest[MAX_NUM_OF_LSI];

	for (INT i = 0; i < MAX_NUM_OF_LSI; i++)
	{
		stTest[i] = PRESSValue[i];
	}
	return TRUE;
}

BOOL IsSplitFile(CString szFileName, ULONG ulSizeInByte)
{
	CFileFind	FileFind;

	ULONG ulFileSize = szFileName.GetLength();

	BOOL bWorking = FileFind.FindFile(szFileName);

	if (!bWorking)
	{
		return TRUE;
	}

	while (bWorking)
	{
		bWorking = FileFind.FindNextFile();
		ulFileSize = (ULONG)FileFind.GetLength();
	}

	if (ulFileSize >= ulSizeInByte)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL BackupFile(CString szFileName)
{
	CFileFind	FileFind;
	CString		szBackupFile[NUM_OF_BACKUP];
	CString		szExtension;
	BOOL		bResult = TRUE;

	INT			i = 0;


	if (szFileName == "")
	{
		return FALSE;
	}

	szBackupFile[0] = szFileName;
	szFileName.TrimRight(".");
	for (i = 1; i < NUM_OF_BACKUP; i++)
	{
		szExtension.Format("00%ld", i + 1);
		szBackupFile[i] = szFileName.TrimRight(".") + szExtension;
	}
	

	for (i = NUM_OF_BACKUP - 1; i >= 0; i--)
	{
		// Delete the oldest File
		if ((i == NUM_OF_BACKUP - 1))
		{
			if (FileFind.FindFile(szBackupFile[i]))
			{
				DeleteDir(szBackupFile[i]);
			}
		}
		else
		{
			if (FileFind.FindFile(szBackupFile[i]))
			{
				bResult = RenameDir(szBackupFile[i], szBackupFile[i + 1]);
			}
		}
	}

	return bResult;
}

DOUBLE CalDequeAverage(std::deque <LONG> &stlDeque) //20130425
{
	DOUBLE dReturn = 0.0;
	std::deque <LONG>::size_type size;

	size = stlDeque.size();
	if (size == 0)
	{
		return 0.0;
	}

	for (INT ii = 0; ii < (INT)size; ii++)
	{
		dReturn += stlDeque[ii];
	}
	dReturn /= size;
	return dReturn;
}

DOUBLE CalDequeAverage(std::deque <DOUBLE> &stdDeque) //20130425
{
	DOUBLE dReturn = 0.0;
	std::deque <LONG>::size_type size;

	size = stdDeque.size();
	if (size == 0)
	{
		return 0.0;
	}

	for (INT ii = 0; ii < (INT)size; ii++)
	{
		dReturn += stdDeque[ii];
	}
	dReturn /= (DOUBLE) size;
	return dReturn;
}

BOOL CalVectorSD(std::vector <D_MTR_POSN> &stlVector, DOUBLE &dSDx, DOUBLE &dSDy) //20160610
{
	//Standard deviation of vector
	DOUBLE dValueX = 0.0, dValueY = 0.0;
	DOUBLE dAverageX = 0.0, dAverageY = 0.0;
	DOUBLE dErrX = 0.0, dErrY = 0.0;
	std::vector <D_MTR_POSN>::size_type size;

	size = stlVector.size();
	if (size == 0)
	{
		return FALSE;
	}

	for (INT ii = 0; ii < (INT)size; ii++)
	{
		dValueX += stlVector[ii].x;
		dValueY += stlVector[ii].y;
	}
	
	dAverageX = dValueX / (DOUBLE) size;
	dAverageY = dValueY / (DOUBLE) size;
	
	for (INT ii = 0; ii < (INT)size; ii++)
	{
		dErrX += (dAverageX - stlVector[ii].x) * (dAverageX - stlVector[ii].x);
		dErrY += (dAverageY - stlVector[ii].y) * (dAverageY - stlVector[ii].y);
	}

	dSDx = dErrX / (DOUBLE) size;
	dSDy = dErrY / (DOUBLE) size;
	return TRUE;
}

//INT	SrchNextEdge(LONG lCurrEdge, EdgeStatus stEdgeStatus[])
//{
//	LONG lResult			= EDGE_NONE;
//
//	INT nIndex			= 0;
//	INT nCW_Neighbour	= (lCurrEdge + nIndex) % MAX_EDGE_OF_GLASS;
//	INT	nCCW_Neighbour	= (lCurrEdge + (MAX_EDGE_OF_GLASS - nIndex)) % MAX_EDGE_OF_GLASS;
//	BOOL bCheckCCW		= FALSE;
//
//	do
//	{
//		// Set the Edge to Search
//		nCW_Neighbour	= (lCurrEdge + nIndex) % MAX_EDGE_OF_GLASS;
//		nCCW_Neighbour	= (lCurrEdge + (MAX_EDGE_OF_GLASS - nIndex)) % MAX_EDGE_OF_GLASS;
//
//		// Check Next Edge in Clockwise
//		if (stEdgeStatus[nCW_Neighbour].bUsed)
//		{
//			if (!stEdgeStatus[nCW_Neighbour].bBonded)
//			{
//				// Got the Next Edge
//				lResult = nCW_Neighbour;
//				break;
//			}
//			else
//			{				
//				bCheckCCW = TRUE;
//			}
//		}
//		else
//		{
//			bCheckCCW = TRUE;
//		}
//
//		// Check Next Edge in CounterClockwise
//		if (bCheckCCW)
//		{
//			if (stEdgeStatus[nCCW_Neighbour].bUsed)
//			{
//				if (!stEdgeStatus[nCCW_Neighbour].bBonded)
//				{
//					// Got the Next Edge
//					lResult = nCCW_Neighbour;
//					break;
//				}
//			}
//		}
//
//		nIndex++;												//INDEX increments!!!
//
//	}	 while (nIndex <= DoubleToLong(MAX_EDGE_OF_GLASS));	// Prevent Dead Looping
//
//	if (nIndex > DoubleToLong(MAX_EDGE_OF_GLASS))
//	{
////#if 1 //20140804
////		int ii = 0;
////		CString szMsg;
////		for (ii = 0; ii < MAX_EDGE_OF_GLASS; ii++)
////		{
////			g_pAppMod->DisplayMessage("NO LSI Trial Number 3"); //Chong Li Prebond Error
////			szMsg.Format("stEdgeStatus bUsed=%d, bBonded=%d", stEdgeStatus[ii].bUsed ? 1 : 0, stEdgeStatus[ii].bBonded ? 1 : 0);
////			g_pAppMod->DisplayMessage(szMsg);
////		}
////#endif
//		return EDGE_NONE;
//	}
//
//	return lResult;
//}

//INT	GetNextEdge(LONG lCurrEdge, ACF_INFO stACF[])
//{
//	// Define Clockwise & Anti Clockwise Neighbour Edge
//	// Make Decision on Closest Next Edge (Cloclwise Neighbour for first Priority)
//
//	INT nResult			= EDGE_NONE;
//	INT nNumOfEdge		= 0;
//
//	INT	i = 0;
//	EdgeStatus stEdgeStatus[MAX_EDGE_OF_GLASS];
//
//	for (i = 0; i < MAX_EDGE_OF_GLASS; i++)
//	{
//		stEdgeStatus[i].bUsed	= FALSE;
//		stEdgeStatus[i].bBonded	= FALSE;
//	}
//
//	// Set the Edge Status 
//	// Assume the Bonding ACF/LSI order is Ascending
//	for (i = ACF1; i < MAX_NUM_OF_ACF; i++)
//	{
//		if (stACF[i].bEnable)
//		{
//			switch (stACF[i].lEdgeInfo)
//			{
//			case EDGE_A:
//				stEdgeStatus[EDGE_A].bUsed	= TRUE;
//				stEdgeStatus[EDGE_A].bBonded = stACF[i].bBonded;
//				break;/*
//
//			case EDGE_B:
//				stEdgeStatus[EDGE_B].bUsed	= TRUE;
//				stEdgeStatus[EDGE_B].bBonded = stACF[i].bBonded;
//				break;
//
//			case EDGE_C:
//				stEdgeStatus[EDGE_C].bUsed	= TRUE;
//				stEdgeStatus[EDGE_C].bBonded = stACF[i].bBonded;
//				break;
//
//			case EDGE_D:
//				stEdgeStatus[EDGE_D].bUsed	= TRUE;
//				stEdgeStatus[EDGE_D].bBonded = stACF[i].bBonded;
//				break;*/
//			}
//		}
//	}
//
//	return SrchNextEdge(lCurrEdge, stEdgeStatus);
//}

VOID GenGlassID(CString &szRtn) //20161207
{
	static UINT nNumber = 0;
	SYSTEMTIME ltime;

	GetLocalTime(&ltime);
	szRtn.Format("G%02u%02u%02u%02u%02u%02u", ltime.wMonth, ltime.wDay, ltime.wHour, ltime.wMinute, ltime.wSecond, nNumber % 100);

	if (++nNumber >= 100)
	{
		nNumber = 0;
	}
   return;
}

VOID GenCOFID(CString &szRtn) //20161207
{
	static UINT nNumber = 0;
	SYSTEMTIME ltime;

	GetLocalTime(&ltime);
	szRtn.Format("C%02u%02u%02u%02u%02u%02u", ltime.wMonth, ltime.wDay, ltime.wHour, ltime.wMinute, ltime.wSecond, nNumber % 100);

	if (++nNumber >= 100)
	{
		nNumber = 0;
	}
   return;
}