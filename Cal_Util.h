/////////////////////////////////////////////////////////////////
//	Cal_Util.cpp : interface of the Cal_Util
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

#include "prheader.h"

#include "AC9000Stn.h"
#include "PR_Util.h"

#include "Package_Util.h"

#include <deque> //20130425
#include <vector>

//#include <string>
//#include <iostream>

//#include <conio.h>
extern LSI_ON_GLASS_INFO	g_stLSI[MAX_NUM_OF_LSI];

typedef struct
{
	BOOL bUsed;
	BOOL bBonded;

} EdgeStatus;

LONG DoubleToLong(DOUBLE dInput);
DOUBLE SetDecimalPlace(DOUBLE dInput);

BOOL FindManualCalibMatrix(PRU *pPRU, MTR_POSN MtrPts[], PR_COORD PRPts[]);
BOOL FindAutoCalibMatrix(PRU *pPRU, MTR_POSN MtrPts[], PR_COORD PRPts[]);
BOOL FindAutoCalibMatrix(PRU *pPRU, MTR_POSN MtrPts[], PR_RCOORD PRPts[]); //20150728 float PR
BOOL FindCORCentre(INT nSample, MTR_POSN MtrPts[], D_MTR_POSN *p_dmtrCORPosn);
BOOL RefineCORCentre(MTR_POSN mtrCurPosn, DOUBLE dAngle, D_MTR_POSN dmtrResultPosn, D_MTR_POSN *p_dmtrCORPosn);

BOOL FindLeastSquareCOR(std::vector <D_MTR_POSN> &vecPoint, D_MTR_POSN &dmtrCORPosn, DOUBLE &dRadius); //20160530
BOOL GetRandomCirclePoint(DOUBLE dPointX, DOUBLE dCenterX, DOUBLE dCenterY, DOUBLE dRadius, DOUBLE dNoiseLmt, DOUBLE &dOutX, DOUBLE &dOutY);
BOOL GetCirclePoint(DOUBLE dPointX, DOUBLE dCenterX, DOUBLE dCenterY, DOUBLE dRadius, DOUBLE &dPointY);
BOOL FindCORWithAngle2Point(const D_MTR_POSN dmtrP1, const D_MTR_POSN dmtrP2, const DOUBLE dAngle, D_MTR_POSN &dmtrCOR); //20160608

BOOL VectorRotate(D_MTR_POSN dmtrIn, DOUBLE dAngleRad, D_MTR_POSN *p_dmtrOut); //20120810
BOOL VectorRotate(MTR_POSN mtrIn, DOUBLE dAngleRad, MTR_POSN *p_mtrOut);

DOUBLE calcAngle(D_MTR_POSN dmtrP1, D_MTR_POSN dmtrP2); //20120924
DOUBLE calcAngleDegree(D_MTR_POSN dmtrP1, D_MTR_POSN dmtrP2);
DOUBLE calcAngle(MTR_POSN mtrP1, MTR_POSN mtrP2);
DOUBLE calcAngleDegree(MTR_POSN mtrP1, MTR_POSN mtrP2);
DOUBLE calcDisplayAngleDegree(DOUBLE dy, DOUBLE dx);

BOOL ForceToDac(DOUBLE dTargetForce, FORCE_DATA stForceCalib[], LONG *p_lOpenDacValue);
BOOL DacToForce(LONG lTargetDacValue, FORCE_DATA stForceCalib[], DOUBLE *p_dForce);

BOOL ForceToPressure(DOUBLE dTargetForce, FORCE_DATA stForceCalib[], DOUBLE *p_dPressure);
BOOL PressureToForce(DOUBLE dTargetPressure, FORCE_DATA stForceCalib[], DOUBLE *p_dForce);

BOOL RotaryToLinearEnc(LONG lTargetRotaryEnc, WHZ_DATA stWhZ_q[], LONG *p_lLinearEnc);
BOOL LinearToRotaryEnc(LONG lTargetLinearEnc, WHZ_DATA stWhZ_q[], LONG *p_lRotaryEnc);
BOOL RotaryToLinearEnc(LONG lTargetRotaryEnc, ENC_CALIB_DATA stWhZ_q[], LONG *p_lLinearEnc);
BOOL LinearToRotaryEnc(LONG lTargetLinearEnc, ENC_CALIB_DATA stWhZ_q[], LONG *p_lRotaryEnc);

//
//BOOL CalcWHAbsoluteLevel(LONG lRefLevel, LONG lOffset, WHZ_DATA stData[MAX_NUM_OF_WHZ][MAX_NUM_WHZ_CAL_STEP], LONG *p_lAbsoluteLevel, LONG WHZ);	// RefLevel = Reference WH level in Motor Count
																										// lOffset	= Offset distance (in um) from RefLevel
																										// stData	= The calibrated data set between Linear & Rotary

BOOL CalcWHLinearOffset(LONG lRefLevel, LONG lCurrLevel , WHZ_DATA stData[], LONG *p_lLinearOffset);	// RefLevel		= Reference WH level in Motor Count
																										// lCurrLevel	= The Level aims to compare with RefLevel
																										// stData		= The calibrated data set between Linear & Rotary
																										// p_lLinearOffset = The Offset displacement in um

LONG	DistanceToCount(DOUBLE dDistance, DOUBLE dDistPerCount);					// Insert Required Distance(um) and Return Motor Count
DOUBLE	CountToDistance(DOUBLE dMotorCount, DOUBLE dDistPerCount);					// Insert Required Motor Count and Return Distance(um)

LONG	AngleToCount(DOUBLE dAngle, DOUBLE dDistPerCount);							// Insert Required Angle(Degree) and Return Motor Count
DOUBLE	CountToAngle(DOUBLE dMotorCount, DOUBLE dDistPerCount);						// Insert Required Motor Count and Return Angle(Degree)

DOUBLE	DistanceInHipec(LONG lDistance, DOUBLE dDistPerCount);					// Distance in mm
DOUBLE	VelocityInHipec(LONG lVelocity, DOUBLE dDistPerCount);					// Velocity in mm/s

LONG	HipecInDistance(DOUBLE dHipecDist, DOUBLE dDistPerCount);
LONG	HipecInVelocity(DOUBLE dVelocity, DOUBLE dDistPerCount);

VOID	CalcPosnAfterTransform(D_MTR_POSN dmtrCurrOffset, DOUBLE dCurrDistPerCnt, DOUBLE dResultDistPerCnt, DOUBLE dAngleBtnAxis, D_MTR_POSN *p_dmtrResultOffset);		// Transform the offset from Current Axis to Result Axis with Angle

VOID SortTempCalibData(TEMP_DATA stTempCalib[]);
VOID SortForceCalibData(FORCE_DATA stForceCalib[]); //20130416

BOOL SetTempToBondTemp(DOUBLE dSetTemp, TEMP_DATA stTempCalib[], DOUBLE *p_dBondTemp);
BOOL BondTempToSetTemp(DOUBLE dBondTemp, TEMP_DATA stTempCalib[], DOUBLE *p_dSetTemp);

LSITypeNum GetLSITypeNum(HandOverStatusNum lHandOverStatusNum);
ShuttleUnitNum GetShuttleUnit(HandOverStatusNum lHandOverStatusNum);
BOOL SetHandOverStatus(LSITypeNum lLSIType, ShuttleUnitNum lShuttleUnit, HandOverStatusNum *lHandOverStatus);
STATUS_TYPE SetPickLSIStatus(HandOverStatusNum lHandOverStatus);
STATUS_TYPE SetPlaceLSIStatus(HandOverStatusNum lHandOverStatus);
HandOverStatusNum GetHandOverStatus(STATUS_TYPE stStationStatus);


LONG GetMaxValue(LONG lValue1 = 0, LONG lValue2 = 0, LONG lValue3 = 0, LONG lValue4 = 0);



//INT SrchNextEdge(LONG lCurrEdge, EdgeStatus stEdgeStatus[]);					// Return lCurrEdge if all the LSI on the specific Edge is not bonded
																				// Return EDGE_NONE if no suitable Edge is searched
																				// Return the neighbour Edge in Clockwise as first Priority
INT	GetNextEdge(LONG lCurrEdge, ACF_INFO			stACF[]);
//INT	GetNextEdge(LONG lCurrEdge, LSI_ON_GLASS_INFO	stLSI[]);
//INT	GetNextEdge(LONG lCurrEdge, MB_INFO				stMB[]);
//INT	GetNextEdge(LONG lCurrEdge, PRESS_INFO			stPRESS[]);
//
//INT SrchLastEdge(LONG lFirstEdge, EdgeStatus		stEdgeStatus[]);
//INT GetLastEdge(LONG lFirstEdge, ACF_INFO			stACF[]);
//INT GetLastEdge(LONG lFirstEdge, LSI_ON_GLASS_INFO	stLSI[]);


INT GetFirstUnit(LONG lCurrEdge, ACF_INFO			stACF[]);
/*
INT GetFirstUnit(LONG lCurrEdge, LSI_ON_GLASS_INFO	stLSI[]);
INT GetFirstUnit(LONG lCurrEdge, MB_INFO			stMB[]);
INT GetFirstPress(LONG lCurrEdge, PRESS_INFO		stPRESS[]);
*/

INT GetNextACF(LONG lCurrACF, ACF_INFO				stACF[]);
				
INT SrchNextEdge(LONG lCurrEdge, EdgeStatus stEdgeStatus[]);
INT	GetNextEdge(LONG lCurrEdge, ACF_INFO			stACF[]);
INT GetNextLSI(LONG lCurrLSI, LSI_ON_GLASS_INFO		stLSI[]);
/*

VOID SetMBPRESSInfo(LONG lPressType, DOUBLE lMBHeadMin, DOUBLE lMBHeadMax, MTR_POSN mtrPR1[], MTR_POSN mtrPR2[], MB_INFO stMB[], PRESS_INFO* stMBPRESS);
*/
VOID SetMBPRESSInfo(STATUS_TYPE GLASS_Status[], PRESS_INFO *stMBPRESS);

LONG SetGlassStatus(STATUS_TYPE StationStatus, STATUS_TYPE *pGLASS_Status);
LONG SetStationStatus(STATUS_TYPE GLASS_Status[]);
/*
CString GetStatusString(STATUS_TYPE stStatus);
*/

VOID CalcDataMean(DATA_SET *p_stDataSet);
VOID CalcDataRange(DATA_SET *p_stDataSet);

BOOL CopyDir(const CString szSource, const CString szDest);
BOOL DeleteDir(const CString szDest);
BOOL CheckPath(const CString szDest);
VOID CreatePath(const CString szPath);
BOOL RenameDir(const CString szSource, const CString szDest);

BOOL Test(PRESS_INFO *stPRESS);

BOOL IsSplitFile(CString szFileName, ULONG ulSizeInByte);
BOOL BackupFile(CString szFileName);

DOUBLE CalDequeAverage(std::deque <LONG> &stlDeque); //20130425
DOUBLE CalDequeAverage(std::deque <DOUBLE> &stdDeque);
BOOL CalVectorSD(std::vector <D_MTR_POSN> &stlVector, DOUBLE &dSDx, DOUBLE &dSDy); //20160610

VOID GenGlassID(CString &szRtn); //20161207
VOID GenCOFID(CString &szRtn);

