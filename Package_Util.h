/////////////////////////////////////////////////////////////////
//	Package_Util.cpp : interface of the Package_Util
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


typedef struct
{
	// Dimension Related (all in mm)
	DOUBLE			dLength;
	DOUBLE			dWidth;

	DOUBLE			dThickness1;
	DOUBLE			dThickness2;
	DOUBLE			dThickness3;
	DOUBLE			dThickness4;

	LONG			lSealantPosn;		// 0 - Left, 1 - Right, 2 - Bottom

	DOUBLE			dItoWidth;
	DOUBLE			dItoToDieEdge;
	DOUBLE			dWHOffset;		// Remove Me (May not necessary in New System Design)

	CString			szName;
} GLASS_INFO;

typedef struct
{
	BOOL			bEnable;
	BOOL			bValid;				// TRUE when PR learnt && alignment pt calibrated && PostBond learnt && PostBond pt set
	BOOL			bBonded;			// TRUE when the INWH's PR Finish

	// Bond Pt Related
#ifdef ACF_POSN_CALIB //20120829
	D_MTR_POSN		dmtrACFAlignPt1;
	D_MTR_POSN		dmtrACFAlignPt2;
	D_MTR_POSN		dmtrACFCenterFromGlassCenterOffset;
	LONG			lACFWHLoadXRefOffset;
	LONG			lACFWHBondYRefOffset[MAX_NUM_OF_ACF];
#endif
	D_MTR_POSN		dmtrAlignPt1;
	D_MTR_POSN		dmtrAlignPt2;
	BOOL			bAlignPtSet;		// TRUE when Alignment pts are set
}
INWH_INFO;

#ifdef TA4_PR
typedef struct
{
	BOOL			bEnable;
	BOOL			bValid;				// TRUE when PR learnt && alignment pt calibrated && PostBond learnt && PostBond pt set
	BOOL			bBonded;			// TRUE when the TA4's PR Finish

	D_MTR_POSN		dmtrAlignPt1;
	D_MTR_POSN		dmtrAlignPt2;
	BOOL			bAlignPtSet;		// TRUE when Alignment pts are set
}
TA4_PR_INFO;
#endif

typedef struct
{
	BOOL			bEnable;
	BOOL			bValid;				// TRUE when PR learnt && alignment pt calibrated && PostBond learnt && PostBond pt set
	BOOL			bBonded;			// TRUE when the TA4's PR Finish

	D_MTR_POSN		dmtrAlignPt1;
	D_MTR_POSN		dmtrAlignPt2;
	D_MTR_POSN		dmtrPolarizerPt1;
	D_MTR_POSN		dmtrPolarizerPt2;
	BOOL			bAlignPtSet;		// TRUE when Alignment pts are set
}
TA2_PR_INFO;

typedef struct  // 20141103
{
	BOOL			bEnable;
	BOOL			bValid;				// TRUE when PR learnt && alignment pt calibrated && PostBond learnt && PostBond pt set
	BOOL			bBonded;			// TRUE when the TA4's PR Finish

	D_MTR_POSN		dmtrGlassFromWHOffset[MAX_NUM_OF_MAINBOND]; //20141202
	D_MTR_POSN		dmtrGlassFromWHDistance[MAX_NUM_OF_MAINBOND]; 
	D_MTR_POSN		dmtrGlassPt1[MAX_NUM_OF_MAINBOND];
	D_MTR_POSN		dmtrGlassPt2[MAX_NUM_OF_MAINBOND];
	D_MTR_POSN		dmtrGlassCenter[MAX_NUM_OF_MAINBOND];
	D_MTR_POSN		dmtrWHPt1[MAX_NUM_OF_MAINBOND];
	D_MTR_POSN		dmtrWHPt2[MAX_NUM_OF_MAINBOND];
	D_MTR_POSN		dmtrWHCenter[MAX_NUM_OF_MAINBOND];


	D_MTR_POSN		dmtrAlignPt1;
	D_MTR_POSN		dmtrAlignPt2;
	D_MTR_POSN		dmtrAlignPtCenter;
	BOOL			bAlignPtSet;		// TRUE when Alignment pts are set
}
MBDL_PR_INFO;

typedef struct
{
	//CString			szName;				// suppose all name are the same for
	//BOOL			bEnable;
	////LONG			lLSIType;			// NO_LSI_TYPE, LSI_TYPE1, LSI_TYPE2
	//DOUBLE			dLength;
	//DOUBLE			dWidth;

	//BOOL			bBonded;			// TRUE when the ACF is attached onto the glass (used in Automode)
	//OFFSET			stBondOffset;

	CString			szName;				// suppose all name are the same for
	BOOL			bEnable;
	BOOL			bValid;				// TRUE when PR learnt && alignment pt calibrated && PostBond learnt && PostBond pt set
	//LONG			lLSIType;			// NO_LSI_TYPE, LSI_TYPE1, LSI_TYPE2
	DOUBLE			dLength;
	DOUBLE			dWidth;
	LONG			lEdgeInfo;

	// Bond Pt Related
	D_MTR_POSN		dmtrAlignPt1;
	D_MTR_POSN		dmtrAlignPt2;
	BOOL			bAlignPtSet;		// TRUE when Alignment pts are set

	OFFSET			stBondOffset;
	BOOL			bBonded;			// TRUE when the ACF is attached onto the glass (used in Automode)

	// PostBond Related	
	D_MTR_POSN		dmtrPostBondPt1;
	D_MTR_POSN		dmtrPostBondPt2;
	BOOL			bPostBondPtSet;		// TRUE when PostBond Pts are set


} ACF_INFO;

typedef struct
{
	BOOL			bEnable;
	BOOL			bValid;				// TRUE when PR learnt && alignment pt calibrated 
	BOOL			bBonded;			// TRUE when the ACFUL's PR Finish

	D_MTR_POSN		dmtrAlignPt1;
	D_MTR_POSN		dmtrAlignPt2;
	BOOL			bAlignPtSet;		// TRUE when Alignment pts are set
}
ACFUL_INFO;

typedef struct
{
	CString			szName;

	BOOL			bValid[MAX_NUM_OF_PREBOND];			// TRUE when PreBond PR learnt && alignment pt calibrated
	DOUBLE			dLength;
	DOUBLE			dWidth;
	DOUBLE			dThickness;
	
	// Bond Pt Related
	D_MTR_POSN		dmtrAlignPt1[MAX_NUM_OF_PREBOND];
	D_MTR_POSN		dmtrAlignPt2[MAX_NUM_OF_PREBOND];
	BOOL			bAlignPtSet[MAX_NUM_OF_PREBOND];		// TRUE when LSI Alignment pts are set

	// Pick Pt Related TrayHolder
	D_MTR_POSN		dmtrAlignPickPt1;
	D_MTR_POSN		dmtrAlignPickPt2;
	BOOL			bAlignPickPtSet;

	OFFSET			stPickOffset;
	
	// Preciser
	D_MTR_POSN		dmtrAlignLSIPt1[MAX_NUM_OF_PREBOND];
	D_MTR_POSN		dmtrAlignLSIPt2[MAX_NUM_OF_PREBOND];
	BOOL			bAlignLSIPtSet[MAX_NUM_OF_PREBOND];

	OFFSET			stPreBondPickOffset[MAX_NUM_OF_PREBOND];

} LSI_TYPE_INFO;

typedef struct
{
	BOOL			bEnable;
	BOOL			bValid[MAX_NUM_OF_GLASS];				// TRUE when PreBond WH PR learnt && alignment pt calibrated
	LONG			lLSIType;			// NO_LSI_TYPE, LSI_TYPE1, LSI_TYPE2
	LONG			lGlassInfo;

	// PreBond Related
	D_MTR_POSN		dmtrAlignPt1[MAX_NUM_OF_PREBOND];
	D_MTR_POSN		dmtrAlignPt2[MAX_NUM_OF_PREBOND];
	BOOL			bAlignPtSet[MAX_NUM_OF_PREBOND];		// TRUE when Alignment pts are set
	OFFSET			stBondOffset[MAX_NUM_OF_PREBOND][MAX_NUM_OF_UNIT];
	//OFFSET			stBondMachOffset[MAX_NUM_OF_PREBOND][MAX_NUM_OF_UNIT]; //20130726
	BOOL			bBonded[MAX_NUM_OF_PREBOND];			// TRUE when the LSI is Bonded onto the glass (used in Automode)

} LSI_ON_GLASS_INFO;

typedef struct
{
	BOOL			bEnable;
	//LSINum			lPressLSI[MAX_NUM_OF_MB_UNIT];	// To know which LSI do we bonded
	LONG			bBonded;
	LONG			lPressMode;
	LONG			lLSIType;			// NO_LSI_TYPE, LSI_TYPE1, LSI_TYPE2

} PRESS_INFO;							// Operation seq used.