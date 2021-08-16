/////////////////////////////////////////////////////////////////
//	ACFWH.cpp : interface of the CACFWH class
//
//	Description:
//		AC9000 Application Software
//
//	Date:		Thursday, September 09, 2010
//	Revision:	1.00
//
//	By:			AC9000
//				CSP
//
//	Copyright @ ASM Assembly Automation Ltd., 2010.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once

#include "AC9000Stn.h"
#define WORKHOLDER_EXTERN
#include "WorkHolder.h"
#include "PRTaskStn.h"
#include "Package_Util.h"

#ifdef EXCEL_MACHINE_DEVICE_INFO
#include "ExcelMachineInfo_Util.h"
#endif

#ifdef ACFWH_EXTERN
extern BOOL					g_bStopCycle;

extern AUTOMODE				AutoMode;
extern BOOL					g_bDiagMode;
extern BOOL					g_bSelectedReworkMode;
extern LONG					g_lDiagnSteps;
extern BOOL					g_bEnableAnvilVac; 
extern BOOL					g_bUplookPREnable;

extern DOUBLE				g_dModuleOffset;
extern BOOL					g_bIsUserSearchPRDone;
extern BOOL					g_bShowUserSearchPRDone;
extern CString				g_szBondingMsg;

extern STATUS_TYPE			ACF_WH_Status[MAX_NUM_OF_ACFWH];
extern STATUS_TYPE			ACF_Status[MAX_NUM_OF_ACF_UNIT];
extern STATUS_TYPE			TA1_Status;
extern STATUS_TYPE			TA2_Status;
extern STATUS_TYPE			INSPOPT_Status;

extern STATUS_TYPE			ACF_GLASS_Status[MAX_NUM_OF_ACFWH][MAX_NUM_OF_GLASS];
extern STATUS_TYPE			WINEAGLE_Status;

extern ACF_INFO				g_stACF[MAX_NUM_OF_ACF];
extern LONG					g_lGlassInputMode;

extern GLASS_INFO			g_stGlass;

	//wizard
extern BOOL					g_bWizardEnable;
extern CString				g_szWizardTitle;
extern ULONG				g_ulWizardSubTitle;
extern CString				g_szWizardStep;
extern CString				g_szWizardMess;
extern CString				g_szWizardMessPrev;
extern CString				g_szWizardMessNext;
extern CString				g_szWizardBMPath;
extern BOOL					g_bWizardEnableBack;
extern BOOL					g_bWizardEnableNext;
extern BOOL					g_bWizardEnableExit;
extern ULONG				g_ulWizardStep;
extern LONG					g_lWizardInProgress;

extern ULONG				g_ulUserGroup;
extern SETUP_POSN_XY		g_stSetupPosn[NUM_OF_POSN_XY];
extern SETUP_LEVEL_Z		g_stSetupLevel[NUM_OF_LEVEL_Z];
extern SETUP_PARA			g_stSetupPara[NUM_OF_PARA];

extern CCriticalSection		m_csGetAllStatusLock;

#endif


typedef enum
{
	SELECT_ALG = 0,
	LEARN_ACF_REGION,
	LEARN_SRCH_REGION,
	ADJUST_THERSHOLD,
	LEARN_DEFECT_REGION,
	LEARN_CHIP_REGION,
	FINISH_LEARN,

} LEARN_ACF_PATTERN_STEP;
// Control Signals
//
typedef enum
{
	ACF_WH_Y_CTRL_NOTHING = 0, 
	ACF_WH_Y_CTRL_GO_HOME, 
	ACF_WH_Y_CTRL_GO_STANDBY_POSN, 
	ACF_WH_Y_CTRL_GO_APPROX_BOND_POSN,
	ACF_WH_Y_CTRL_GO_BASE_HEATER_POSN,
	ACF_WH_Y_CTRL_GO_LOAD_POSN, 
	ACF_WH_Y_CTRL_GO_UNLOAD_POSN, 
	ACF_WH_Y_CTRL_GO_LOAD_POSN_OFFSET,
	ACF_WH_Y_CTRL_GO_UNLOAD_POSN_OFFSET, 

} ACF_WH_Y_CTRL_SIGNAL;

typedef enum
{
	ACF_WH_XY_CTRL_NOTHING = 0,
	ACF_WH_XY_CTRL_GO_PBPR1_POSN, 
	ACF_WH_XY_CTRL_GO_PBPR2_POSN, 
	ACF_WH_XY_CTRL_GO_PBPR_MP1_POSN,
	ACF_WH_XY_CTRL_GO_PBPR_MP2_POSN,

} ACF_WH_XY_CTRL_SIGNAL;

typedef enum
{
	ACF_WH_T_CTRL_NOTHING = 0, 
	ACF_WH_T_CTRL_GO_HOME, 
	ACF_WH_T_CTRL_GO_STANDBY,
	ACF_WH_T_CTRL_GO_LOAD_POSN_OFFSET,

} ACF_WH_T_CTRL_SIGNAL;

//typedef enum
//{
//	ACF_WH_Z_CTRL_NOTHING = 0, 
//	ACF_WH_Z_CTRL_GO_HOME, 
//	ACF_WH_Z_CTRL_GO_STANDBY_LEVEL, 
//	ACF_WH_Z_CTRL_GO_CALIB_STAGE_LEVEL, 
//	ACF_WH_Z_CTRL_GO_CALIB_LOAD_LEVEL, 
//	ACF_WH_Z_CTRL_GO_CALIB_UNLOAD_LEVEL, 
//	ACF_WH_Z_CTRL_GO_APPROX_BOND_LEVEL,
//	ACF_WH_Z_CTRL_GO_PRE_LOAD_LEVEL, 
//	ACF_WH_Z_CTRL_GO_LOAD_LEVEL, 
//	ACF_WH_Z_CTRL_GO_PRE_UNLOAD_LEVEL, 
//	ACF_WH_Z_CTRL_GO_UNLOAD_LEVEL, 
//
//} ACF_WH_Z_CTRL_SIGNAL;

#ifdef ACF_POSN_CALIB //20120917
typedef enum //Divide the operation into parts
{
	ACF_OFFSET_TEST_CTRL_ALL = 0, 
	ACF_OFFSET_TEST_CTRL_BEGIN,
	ACF_OFFSET_TEST_CTRL_END,

} ACF_OFFSET_TEST_CTRL;
#endif

// States
//
typedef enum
{
	ACF_WH_IDLE_Q = 0, 

	ACF_WH_REQUEST_FOR_GLASS_Q, 
	ACF_WH_CHECK_GLASS_EXIST_Q, 
	ACF_WH_MOVE_TO_CURRENT_EDGE_Q, 
	ACF_WH_DO_PR1_Q, 
	ACF_WH_REDO_PR1_Q,					// 5

	ACF_WH_DO_PR2_Q, 
	ACF_WH_REDO_PR2_Q, 
	ACF_WH_CHECK_LAST_PR_COMPLETED_Q,
	ACF_WH_WAIT_TO_BOND_POSN_Q, 
	ACF_WH_WAIT_ACF_COMPLETE_Q,			//10

	ACF_WH_CHECK_LAST_BONDED_ON_EDGE_Q, 
	ACF_WH_POSTBOND_CHECK_Q, 
	ACF_WH_DO_POST_PR1_Q, 
	ACF_WH_REDO_POST_PR1_Q, 
	ACF_WH_DO_POST_PR2_Q,				//15

	ACF_WH_REDO_POST_PR2_Q,				
	ACF_WH_DO_POST_MP1_Q,
	ACF_WH_DO_POST_MP2_Q,
	ACF_WH_CHECK_LAST_POST_PR_ON_EDGE_Q, 
	ACF_WH_CHECK_ACF_LENGTH_TOL_Q,			//20

	ACF_WH_GO_UNLOAD_GLASS_Q,			
	ACF_WH_WAIT_GLASS_PICKED_Q, 
	ACF_WH_WAIT_WINEAGE_ACCEPT_PR1_Q,
	ACF_WH_WAIT_USER_FINISH_PR1_Q,
	ACF_WH_WAIT_WINEAGE_ACCEPT_PR2_Q,		//25

	ACF_WH_WAIT_USER_FINISH_PR2_Q,	
	ACF_WH_WAIT_PLACE_GLASS_Q,
	ACF_WH_WAIT_OPTIC_PERMISSION_Q,
	ACF_WH_WAIT_OPTIC_ACK_Q,
	ACF_WH_WAIT_OPTIC_PB_INSP_PERMISSION_Q,		//30

	ACF_WH_WAIT_OPTIC_PB_INSP_ACK_Q,

	ACF_WH_STOP_Q = 99,


} ACF_WH_STATE;

class CACF;

class CACFWH : public CWorkHolder, public CPRTaskStn
{
	DECLARE_DYNCREATE(CACFWH)
	friend class CAC9000App;
	friend class CSettingFile;
	friend class CHouseKeeping;
	friend class CACF;
	friend class CTA2;
	friend class CTA1;
	friend class CPreBondWH;
	friend class CINWH;
	friend class CWinEagle;

//#ifdef ACF_POSN_CALIB //20120829
//	friend class CINWH;
//#endif
//	

protected:
	

protected:
	//Update
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();

	virtual VOID RegisterVariables();
	virtual VOID PreStartOperation();
	virtual VOID AutoOperation();

	virtual BOOL GetGlass1Status() 
	{
		return m_bGlass1Exist;
	}
	//virtual MOTOR_INFO &GetMotorY()
	//{ 
	//	return m_stMotorY; 
	//}
	//virtual MOTOR_INFO &GetMotorT()
	//{ 
	//	return m_stMotorT; 
	//}
private:
	virtual  CString		GetStation() 
	{
		return "";
	};

public:
	CACFWH();
	virtual	~CACFWH();

	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID Operation();

protected:
	CACF *m_pCACF;
	CACFWH	*m_pCOtherACFWH;
	// ------------------------------------------------------------------------
	// Variable delcaration
	// ------------------------------------------------------------------------
	
	////SI
	//CSiInfo		m_stAnvil1VacSnr;
	//CSiInfo		m_stAnvil2VacSnr;

	////SO
	//CSoInfo		m_stAnvilWBSol;
	//CSoInfo		m_stAnvil1VacSol;
	//CSoInfo		m_stAnvil2WeakBlowSol;
	//CSoInfo		m_stAnvil2VacSol;	

	// Add NuMotion 
	// 
	// ------------------------------------------------------------------------
	// End of Motor SI/SO
	// ------------------------------------------------------------------------

	LONG				m_lCurSlave;
	
	// ------------------------------------------------------------------------
	// End of CHECK Who Am I
	// ------------------------------------------------------------------------
	ACF_INFO	m_stACF[MAX_NUM_OF_ACF];
//	LONG		m_lPolarizerThickness[MAX_NUM_OF_GLASS]; //20120927 pin detect polarizer
	BOOL		m_bNotBondACF;

	// cycle status
	BOOL		m_bPBError;
	BOOL		m_bGlassWithError;
	LONG		m_lCurACF;
	LONG		m_lCurEdge;
	LONG		m_lPrevEdge; //remember the edge direction for T

	LONG		m_lPRRetryCounter;
	LONG		m_lPRRetryLimit;
	LONG		m_lRejectGlassCounter;
	LONG		m_lRejectGlassLimit;
	DWORD		m_dwStartTime;
	
	INT			m_nPRInProgress;
	LONG		m_lLoadGlassEdge;
	LONG		m_lUnloadGlassEdge;

	DOUBLE		m_dProcessTime;

	BOOL		m_bReworkMode;
	BOOL		m_bReworkACFNum[MAX_NUM_OF_ACF];
	BOOL		m_bPostBondChecked[MAX_EDGE_OF_GLASS];

	// Posn 
	MTR_POSN	m_mtrBaseHeaterPosn;
	LONG		m_lBondOffsetY[MAX_NUM_OF_ACF];
	DOUBLE		m_dBondOffsetAdjY[MAX_NUM_OF_ACF]; //20120925
	//LONG		m_lBondLevelOffset[MAX_NUM_OF_WHZ][MAX_NUM_OF_ACF];
	LONG		m_lStandbyPosnT[MAX_EDGE_OF_GLASS];
	MTR_POSN	m_mtrSafetyPosn;
	MTR_POSN	m_mtrBondOffset;		// Dynamic No need Save
	LONG		m_lTOffset;				// Dynamic No need Save
		
	MTR_POSN	m_mtrACFCenterPoint;	// Device
	DOUBLE		m_dBondOffsetT;			// Machine
	MTR_POSN	m_mtrACFLLPoint;
	MTR_POSN	m_mtrACFLRPoint;
	DOUBLE		m_dACFRefWidth; 
	DOUBLE		m_dACFRefLength;

	LONG		m_lLoadXOffset;
	DOUBLE		m_dLoadAdjXOffset; //20121005
	LONG		m_lLoadYOffset;
	LONG		m_lUnloadYOffset;	// ACF1WH and ACF2WH are the same
	MTR_POSN	m_mtrPR1Posn[MAX_EDGE_OF_GLASS];
	MTR_POSN	m_mtrPR2Posn[MAX_EDGE_OF_GLASS];
	MTR_POSN	m_mtrPRCalibPosn;
	MTR_POSN	m_mtrPBPR1Posn;
	MTR_POSN	m_mtrPBPR2Posn;

	LONG		HMI_lLoopCount;

	// Align Glass
	DOUBLE		m_dAlignAngle;			// store the current Glass Rotate Angle		
	MTR_POSN	m_mtrAlignCentre;			// store the current Glass Rotate Angle		
	MTR_POSN	m_mtrCurrAlignDelta;			// store the current Glass Delta x,y
	DOUBLE		m_dGlassRefAng;
	D_MTR_POSN	m_dmtrAlignRef;
	DOUBLE		m_dAlignRefAngle;
	DOUBLE		m_dAlignRefDist;
	DOUBLE		m_dACFAngX;									// angle between ACF tape and WH table along X axis
	BOOL		m_bACFCalibValid;							// To check done with ACF Calib or not
	DOUBLE		m_dBondAngle;								// final bonding angle (also used for updating PRx posns)
	D_MTR_POSN	m_dmtrBondPtOffset;							// used for Bond point offset between ACF and glass centre (current)
	DOUBLE		m_dUnloadGlassCompensationAngle[MAX_EDGE_OF_GLASS];	// 20151023 after DO PR CalcBondPt() record the m_dCurrAlignAngle for that Edge

	BOOL HMI_bCursorControl;		
	
	DOUBLE		HMI_dAlignDist;

	BOOL		m_bPRDone;	// Check DL PR Done.


	BOOL		m_bUnloadReady; //20140627

	////////////////////////////////////////////////////////////////////////
	//From COG930 parameters
	///////////////////////////////////////////////////////////////////////
	// align pt param
	//DOUBLE		m_dACFAngX;									// angle between ACF tape and WH table along X axis
	//BOOL		m_bACFCalibValid;							// To check done with ACF Calib or not
	//DOUBLE		m_dBondAngle;								// final bonding angle (also used for updating PRx posns)
	//D_MTR_POSN	m_dmtrBondPtOffset;							// used for Bond point offset between ACF and glass centre (current)

	//DOUBLE		m_dGlassRefAng[MAX_EDGE_OF_GLASS];			// Glass reference angle
	//D_MTR_POSN	m_dmtrAlignRef[MAX_EDGE_OF_GLASS];			// reference for ACF1 align point centre (only ACF1 is needed)
	//DOUBLE		m_dAlignRefAngle[MAX_EDGE_OF_GLASS];		// reference for ACF1 align point angle (only ACF1 is needed)
	//DOUBLE		m_dUnloadGlassCompensationAngle[MAX_EDGE_OF_GLASS];	// 20151023 after DO PR CalcBondPt() record the m_dCurrAlignAngle for that Edge
	BOOL		m_bACFWHUnloadThetaCompensation; //20151023

	DOUBLE		m_dGlassPostBondRefAng;

	//D_MTR_POSN	m_dmtrCurrPR1Posn[MAX_EDGE_OF_GLASS];		// current PR1 encoder centre (used in calib, calc)
	//D_MTR_POSN	m_dmtrCurrPR2Posn[MAX_EDGE_OF_GLASS];		// current PR2 encoder centre (used in calib, calc)
	//D_MTR_POSN	m_dmtrCurrAlign1Offset;						// current align1 offset (used for search ap1)
	//D_MTR_POSN	m_dmtrCurrAlign2Offset;						// current align2 offset (used for search ap2)
	//D_MTR_POSN	m_dmtrCurrAlignCentre;						// current align centre for ACFx
	//DOUBLE		m_dCurrAlignAngle;							// current align angle for ACFx (calculated from PR angle - Glass ref angle)
	//DOUBLE		m_dCurrAlignDist;

	//PB
	D_MTR_POSN	m_dmtrCurrPBPR1Posn;
	D_MTR_POSN	m_dmtrCurrPBPR2Posn;
	D_MTR_POSN	m_dmtrCurrPBPR_MP1Posn;						// PostBond Mid Point1
	D_MTR_POSN	m_dmtrCurrPBPR_MP2Posn;						// PostBond Mid Point2
	D_MTR_POSN	m_dmtrLastACFPBPR1Posn;						//temprary to remenber the PBP1 centre
	D_MTR_POSN	m_dmtrLastACFPBPR2Posn;						//temprary to remember the PEP2 centre
	PR_RCOORD	m_PRCurrPBPR1Corner[PR_NO_OF_CORNERS];
	PR_RCOORD	m_PRCurrPBPR2Corner[PR_NO_OF_CORNERS];
	DOUBLE		m_dCurrACFLength;
	DOUBLE		m_dACFLengthUpperLmt;
	DOUBLE		m_dACFLengthLowerLmt;

	IMAGE_STAT	m_PBStat1;
	IMAGE_STAT	m_PBStat2;
	LONG		m_lPRPBStatMode;		// 0:SD; 1: Mean
	LONG		m_lPRPBStatRegion;		// 0: >=
	
	//other
	BOOL		m_bPBEnable;
	ULONG		m_ulPBMode;			// 0: Pattern; 1: Statistics
	LONG		m_lPBInspectionPt;
	LONG		m_lNumOfPB_MP;

	LONG		m_lTotalDefectMultRegion;
	LONG		m_lIsAlign;			// 0: NO Align
	LONG		m_lAlignAlg;		// 0:Parallel; 1: Three Line

	////// COR parameters
	//D_MTR_POSN	m_dmtrTableCOR_Pos[MAX_EDGE_OF_GLASS][COR_ARRAY_SIZE];
	//D_MTR_POSN	m_dmtrTableCOR_Neg[MAX_EDGE_OF_GLASS][COR_ARRAY_SIZE];
	//DOUBLE		m_dCORLUT[MAX_EDGE_OF_GLASS][COR_ARRAY_SIZE];
	////BOOL		m_bCORCalibrated[MAX_EDGE_OF_GLASS];
	//DOUBLE		m_dCalibCORRange;

	//// XY Calibration Parameters
	////PR_COORD	m_stPRCalibPts[4];
	////MTR_POSN	m_stMtrCalibPts[4];
	//MTR_POSN	m_stCORMtrCalibPts[COR_ARRAY_SIZE];

	BOOL			m_bACFPlacePosnSetup;	//	ACFUL Setup
	//DOUBLE			m_dACFRefWidth;						//	ACFUL Setup
	//DOUBLE			m_dACFRefLength;					//	ACFUL Setup
	MTR_POSN		m_mtrACFAnvilLLPosn;	//	ACFUL Setup
	MTR_POSN		m_mtrACFAnvilLRPosn;	//	ACFUL Setup
	MTR_POSN		m_mtrAlignRefPosn;	//	ACFUL Setup
	MTR_POSN		m_mtrAlignRefPosnOnWH;
	//DOUBLE			m_dAlignRefAngle;		//	ACFUL Setup
	MTR_POSN		m_mtrRefPlacePosnOffset;	//	ACFUL Setup
	DOUBLE			m_dAnvilBlockAngOffset;		//	ACFUL Setup
	LONG			m_lLoadTOffset;
	DOUBLE			m_dLoadTOffset;

	// ------------------------------------------------------------------------
	// Function delcaration
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	// Virtual WorkHolder Function delcaration
	// ------------------------------------------------------------------------
	// All Virtual Function from PRTaskStn.cpp

	virtual CMotorInfo &GetMotorX(); 
	virtual CMotorInfo &GetMotorY();
	virtual CMotorInfo &GetMotorT();

	virtual INT SyncX();
	virtual INT SyncY();
	virtual INT SyncT();

	virtual INT MoveXYToPRCalibPosn(BOOL bWait = GMP_WAIT);

	virtual BOOL PickGlassOperation(GlassNum lGlassNum);
	
	// ------------------------------------------------------------------------
	// END Virtual WorkHolder Function delcaration
	// ------------------------------------------------------------------------
	CString GetSequenceString();
	VOID OperationSequence();

#ifdef ACF_POSN_CALIB //20120917
	BOOL ACFOffsetTestCalibOperation_RunPart(BOOL bIsPickGlass, ACF_OFFSET_TEST_CTRL stPart);
	BOOL ACFOffsetTestCalibOperation(BOOL bIsPickGlass);
#endif

	BOOL ACFOffsetTestOperation(BOOL bDoACFDL = TRUE);

	LONG FindFirstUnit(EdgeNum lEdge);
	INT GetNextPostBondEdge(LONG lCurrEdge);

	//Motion
	//INT MoveZToStandbyLevel(LONG lGlassNum, BOOL bWait = GMP_WAIT);
	//INT MoveZToLoadLevel(LONG lGlassNum, BOOL bWait = GMP_WAIT);
	//INT MoveZToUnloadLevel(LONG lGlassNum, BOOL bWait = GMP_WAIT);
	//INT MoveZToPreLoadLevel(LONG lGlassNum, BOOL bWait = GMP_WAIT);
	
	INT MoveXYToStandbyPosn(BOOL bWait = GMP_WAIT);
	INT MoveXYToLoadPosn(BOOL bWait = GMP_WAIT);
	INT MoveXYToUnloadPosn(BOOL bWait = GMP_WAIT);
	
	INT MoveXYToBondPosn(LONG lLSINum);
	INT MoveYToBondPosn(LONG lLSINum);
	//INT MoveZToBondLevel(LONG lLSINum);
	INT MoveT(ACF_WH_T_CTRL_SIGNAL ctrl, BOOL bWait);
	INT MoveTToBondPosn(BOOL bWait = GMP_WAIT);
	virtual INT MoveRelativeT(LONG lMtrCnt, BOOL bWait);
	virtual INT MoveTToStandby(BOOL bWait = GMP_WAIT);

	INT MoveY(ACF_WH_Y_CTRL_SIGNAL ctrl, BOOL bWait);
	INT OpticMoveXY(ACF_WH_XY_CTRL_SIGNAL ctrl, BOOL bWait);
	INT PBMoveY(ACF_WH_XY_CTRL_SIGNAL ctrl, BOOL bWait);
	//INT MoveZ(ACF_WH_Z_CTRL_SIGNAL ctrl, BOOL bWait);

	INT MoveYToPR1Posn(LONG lEdge, BOOL bWait);
	INT MoveYToPR2Posn(LONG lEdge, BOOL bWait);

	INT ACFMoveAbsoluteXY(LONG lMtrCntX, LONG lMtrCntY, BOOL bWait);
	INT OpticMoveAbsoluteXY(LONG lMtrCntX, LONG lMtrCntY, BOOL bWait);
	INT MoveAbsoluteXY(LONG lMtrCntX, LONG lMtrCntY, BOOL bWait);

	INT MoveAbsoluteT(LONG lMtrCntT, BOOL bWait);
	INT MoveAbsoluteY(LONG lMtrCntY, BOOL bWait);

	INT MoveXYToCurPRPosn(LONG lPRU);
	INT MoveXYToCurPBPosn(LONG lPRU);
	virtual INT MoveXYToCurrPR1Posn(BOOL bWait = GMP_WAIT);
	virtual INT MoveXYToCurrPR2Posn(BOOL bWait = GMP_WAIT);
	virtual INT MoveXYToCurrPB1Posn(BOOL bWait = GMP_WAIT);
	virtual INT MoveXYToCurrPB2Posn(BOOL bWait = GMP_WAIT);

	INT ACFMoveRelativeXY(LONG lMtrCntX, LONG lMtrCntY, BOOL bWait);
	INT OpticMoveRelativeXY(LONG lMtrCntX, LONG lMtrCntY, BOOL bWait);
	INT MoveRelativeXY(LONG lMtrCntX, LONG lMtrCntY, BOOL bWait);

	INT MoveRelativeY(LONG lMtrCntY, BOOL bWait);

	//Calc Related
	VOID CalDPosnAfterRotate(D_MTR_POSN dmtrTableCOR, MTR_POSN mtrCurPosn, DOUBLE dAngle, D_MTR_POSN *p_dmtrResultPosn);
	VOID CalPosnAfterRotate(LONG lEdge, MTR_POSN mtrCurPosn, DOUBLE dAngle, MTR_POSN *p_mtrResultPosn);
	LONG GetStandbyPosnT(LONG lEdge);
	//VOID SetCurEdge();
	
	
	//PR Related
	//BOOL AutoSearchPR1(EdgeNum lEdge);
	//BOOL AutoSearchPR1GrabOnly(EdgeNum lEdge);
	//BOOL AutoSearchPR1ProcessOnly(EdgeNum lEdge);
	//BOOL AutoSearchPR2(EdgeNum lEdge);
	//BOOL AutoSearchPR2GrabOnly(EdgeNum lEdge);
	//BOOL AutoSearchPR2ProcessOnly(EdgeNum lEdge);

	BOOL AutoSearchPBPR1();
	BOOL AutoSearchPBPR1GrabOnly(LONG lACFNum);
	BOOL AutoSearchPBPR1ProcessOnly(LONG lACFNum);
	BOOL AutoSearchPBPR2();
	BOOL AutoSearchPBPR2GrabOnly(LONG lACFNum);
	BOOL AutoSearchPBPR2ProcessOnly(LONG lACFNum);

	//BOOL SearchPR(PRU *pPRU);
	//BOOL SearchPRGrabOnly(PRU *pPRU);
	//BOOL SearchPRProcessOnly(PRU *pPRU);
	//BOOL SearchPRCentre(PRU *pPRU);
	//BOOL SearchCurrPRPosn(ACFNum lACFNum); // Included rotate the Edge to current Edge
	BOOL CheckPRPBStat(PRU *pPRU);

	// Middle Point
	BOOL AutoSearchPB_MPPR1();
	BOOL AutoSearchPB_MPPR1GrabOnly(LONG lACFNum);
	BOOL AutoSearchPB_MPPR1ProcessOnly(LONG lACFNum);
	BOOL AutoSearchPB_MPPR2();
	BOOL AutoSearchPB_MPPR2GrabOnly(LONG lACFNum);
	BOOL AutoSearchPB_MPPR2ProcessOnly(LONG lACFNum);
	VOID CalcBondPt(ACFNum nACF);
	VOID CalcPB_MP();
	VOID CalcACFAccuracy(D_MTR_POSN dmtrPBPR1Posn, D_MTR_POSN dmtrPBPR2Posn, BOOL bDisplayLength);

	virtual PRU &SelectCurPRU(ULONG ulPRU);
	PRU &SelectCurPBPRU(ULONG ulPBPRU);
	VOID ModulePRSetupPreTask();
	VOID ModulePRSetupPostTask();
	VOID UpdateSetupStatus();
	VOID SelectPRU(ULONG ulPRU, BOOL bIsMove = TRUE);
	VOID SelectPBPRU(ULONG	ulPBPRU, BOOL bIsMove = TRUE);
	LONG HMI_SetPRSelected(IPC_CServiceMessage &svMsg);
	//LONG HMI_SelectCurSlave(IPC_CServiceMessage &svMsg);
	LONG HMI_SelectPRU(IPC_CServiceMessage &svMsg);
	LONG HMI_UpdateSetupStatus(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPR1Posn(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPR2Posn(IPC_CServiceMessage &svMsg);
	//LONG HMI_SetPRLevel(IPC_CServiceMessage &svMsg);
	LONG HMI_SearchAp1(IPC_CServiceMessage &svMsg);
	LONG HMI_SearchAp2(IPC_CServiceMessage &svMsg);
	LONG HMI_SearchCentre(IPC_CServiceMessage &svMsg);
	LONG HMI_ToggleNotBondACF(IPC_CServiceMessage &svMsg);
	LONG HMI_DisplayVideo(IPC_CServiceMessage &svMsg);
	//LONG HMI_SetCurrentSetupGlass(IPC_CServiceMessage &svMsg);
	LONG HMI_SelectPBPRU(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPBPR1Posn(IPC_CServiceMessage &svMsg);
	LONG HMI_SetPBPR2Posn(IPC_CServiceMessage &svMsg);
	LONG HMI_SrchPBPR(IPC_CServiceMessage &svMsg);
	//VOID UpdateHMIPBPRButton();
	VOID SetupACFPattern();
	VOID GetMouseWindow(PRU *pPRU, PR_REGION *Region);
	VOID DrawMouseWindow(PRU *pPRU, PR_REGION Region);

	VOID SetupWizard();
	VOID SetupPRPatternPreTask(PRU *pPRU);
	VOID SetupPRPatternPostTask();
	VOID SetupAlignPt();
	VOID SetupACFOffset();
	BOOL SetGlassRef(BOOL bSetAng);
	VOID CalcAlignPt();
	VOID CalcBondOffset();
	BOOL DoACFDL();
	BOOL IsRejectLimitHit();
	VOID SetupLoadPosnOffset();
	VOID UpDateACFTAPlaceACFWHPosn();

	VOID GetAndLogXYPosn(ACF_WH_XY_CTRL_SIGNAL lCtrl = ACF_WH_XY_CTRL_NOTHING);

	// Module Setup Related
	VOID ModuleSetupPreTask();
	VOID ModuleSetupPostTask();

	BOOL ModuleSetupSetPosn(LONG lSetupTitle = 0);
	BOOL ModuleSetupGoPosn(LONG lSetupTitle = 0);
	BOOL ModuleSetupSetLevel(LONG lSetupTitle = 0);
	BOOL ModuleSetupGoLevel(LONG lSetupTitle = 0);
	BOOL ModuleSetupSetPara(PARA stPara);

	VOID UpdateModuleSetupPosn();
	VOID UpdateModuleSetupLevel();
	VOID UpdateModuleSetupPara();

	INT SetupGoStandbyPosn();
	INT	SetupGoBaseHeaterPosn();
	INT SetupGoLoadGlassPosn();
	INT SetupGoUnloadGlassPosn();
	INT SetupGoApproxBondPosn();	
	INT SetupGoUnloadGlassPosnOffset();
	INT SetupGoBondPosn(ACFNum lACFNum);
	INT SetupGoPRCalibPosn();
	INT SetupGoSafetyPosn();
	INT SetupGoCORPosn();

	INT SetupGoStandbyLevel();
	INT SetupGoCalibStageLevel();
	INT SetupGoCalibLoadLevel();
	INT SetupGoCalibUnloadLevel();
	INT	SetupGoApproxBondLevel();
	INT SetupGoLoadLevel();
	INT SetupGoUnloadLevel();
	//INT SetupGoBondLevel(LONG lLSINum);
	INT SetupGoBondLevel(LONG lLSINum, BOOL bIsInteractive = TRUE); //20130318

	INT SetupGoTStandbyPosn();
	INT SetupGoTLoadOffsetPosn();
	INT SetupGoTLoadPosn();
	INT SetupGoTUnloadPosn();
	INT SetupGoTUnloadOffset();
	INT	SetupGoTApproxBondPosn();
	INT KeepQuiet();

	virtual VOID PrintMachineSetup(FILE *fp);
	virtual VOID PrintDeviceSetup(FILE *fp);

#ifdef EXCEL_MACHINE_DEVICE_INFO
	virtual BOOL printMachineDeviceInfo();
#endif

	// ------------------------------------------------------------------------
	// HMI Variable delcaration
	// ------------------------------------------------------------------------
	// Values are for HMI display only! Users should not use these in coding.	

	LONG	HMI_lBondOffsetY;
#ifdef ACF_POSN_CALIB //20120829
	DOUBLE	HMI_dBondOffsetAdjY;
	BOOL	HMI_bYZControl;
#endif

	BOOL	HMI_bLogACFLength;

	// ------------------------------------------------------------------------
	// HMI Command delcaration
	// ------------------------------------------------------------------------
	// Module Related

	LONG HMI_SelectACFNum(IPC_CServiceMessage &svMsg);
#ifdef ACF_POSN_CALIB //20120829
	LONG HMI_SetBondOffsetAdjY(IPC_CServiceMessage &svMsg);
#endif
	LONG HMI_SetBondOffsetY(IPC_CServiceMessage &svMsg);
	LONG HMI_SetAdjXLoadOffset(IPC_CServiceMessage &svMsg); //20121005
	LONG HMI_ResetLoadOffset(IPC_CServiceMessage &svMsg); //20140507
	LONG HMI_SetXLoadOffset(IPC_CServiceMessage &svMsg);
	LONG HMI_SetYLoadOffset(IPC_CServiceMessage &svMsg);
	LONG HMI_SetTLoadOffset(IPC_CServiceMessage &svMsg);

	LONG HMI_SetYUnloadOffset(IPC_CServiceMessage &svMsg);
	//LONG HMI_SetTUnloadOffset(IPC_CServiceMessage &svMsg);

	LONG HMI_ACFOffsetTest(IPC_CServiceMessage &svMsg);
	LONG HMI_PBEnable(IPC_CServiceMessage &svMsg); 
	LONG HMI_MeasureSetZero(IPC_CServiceMessage &svMsg);
	LONG HMI_MeasureEnable(IPC_CServiceMessage &svMsg); 
	LONG HMI_SearchAndLogPR(IPC_CServiceMessage &svMsg);
	
	virtual LONG IPC_SaveMachineParam();
	virtual LONG IPC_LoadMachineParam();
	virtual LONG IPC_SaveDeviceParam();
	virtual LONG IPC_LoadDeviceParam();
	virtual LONG IPC_SaveMachineRecord();
	virtual LONG IPC_LoadMachineRecord();
	virtual LONG IPC_SaveDeviceRecord();
	virtual LONG IPC_LoadDeviceRecord();

};
