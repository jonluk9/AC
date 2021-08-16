/////////////////////////////////////////////////////////////////
//	PR_Util.cpp : interface of the PR_Util
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
#include "math.h"
#include "prheader.h"

#include "PR_Util.h"
#include "AC9000.h" //20151007

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern LONG g_lHMIIndectViewHeight; //20150917
extern LONG g_lHMIIndectViewWidth;
extern CAC9000App theApp; //20151007

PRU		pruMenu;

PRU		pruIN1;									
PRU		pruACF1;
PRU		pruACF2;
PRU		pruACFPB1[MAX_NUM_OF_ACF][MAX_NUM_OF_SURFACE]; // 0 :WITHACFImage; 1:Background
PRU		pruACFPB2[MAX_NUM_OF_ACF][MAX_NUM_OF_SURFACE];	
PRU		pruACFPB_MP1[MAX_NUM_OF_ACF][MAX_NUM_OF_SURFACE];
PRU		pruACFPB_MP2[MAX_NUM_OF_ACF][MAX_NUM_OF_SURFACE];

PRU		pruACF_WHCalib; // 20141229
PRU		pruACFUL_Calib; 
PRU		pruDummy;

INT		nCamera;
INT		nCurrentCameraID;
INT		nAutoModeCam;
BOOL	bPRUseMouse;
BOOL	bPRUpdateLight;

LONG	g_lCurTA4Glass;
LONG	g_lSearchRange;

// for HMI indentifcation
INT		nCurCalPt;

BOOL	bDirtyRow[PR_MAX_ROW_IN_TEXT_COORD + 1];

BOOL	bChannelInUse[NO_OF_INPUT_CHANNEL] = {FALSE, };

PR_UBYTE	ubSenderID;
PR_UBYTE	ubReceiverID;

//Zoom
PR_CURSOR			g_stCursor;

// refer to page 35
// PR Error Array
static PR_UWORD uwPR_Error[] =
{
	PR_ERR_NO_DIE,					// 0
	PR_ERR_PID_NOT_LD, 
	PR_ERR_SR_AREA_SZ, 
	PR_ERR_DEFECTIVE_DIE, 
	PR_ERR_ALG_NOT_LRN, 
	PR_WARN_TOO_MANY_DIE,			// 5
	PR_WARN_SMALL_SRCH_WIN, 
	PR_WARN_TOO_DARK, 
	PR_WARN_TOO_BRIGHT, 
	PR_ERR_ROTATION, 
	PR_ERR_DIE_TOO_SMALL,			// 10
	PR_ERR_FAIL_SPATIAL, 
	PR_ERR_FAIL_UNIQUE, 
	PR_ERR_INK_DIE, 
	PR_ERR_CHIP_DIE, 
	PR_ERR_CRACK_DIE,				// 15
	PR_ERR_SCRATCH_DIE, 
	PR_ERR_INSUFF_EPOXY_COVERAGE, 
	PR_ERR_PAR, 
	PR_ERR_NOREF, 
	PR_ERR_CAM_NOT_LNK,				// 20
	PR_ERR_UID_NOT_MAT, 
	PR_ERR_NO_PIDREC, 
	PR_ERR_NO_PAD, 
	PR_ERR_LGT_BGT, 
	PR_ERR_LGT_DIM,					// 25
	PR_ERR_LD_LEAD, 
	PR_ERR_SRCH_LEAD_FAIL, 
	PR_ERR_LEAD_TIP_FAIL, 
	PR_ERR_LD_ANG_TOO_LARGE, 
	PR_ERR_NO_EDGE_FIND,			// 30
	PR_ERR_SRC_WIDTH_INCONSIST, 
	PR_ERR_CHK_CLRNCE_FAIL, 
	PR_ERR_LD_TMPL_OUT_RANGE, 
	PR_ERR_INPUT_FILENAME_NOT_EXISTED,
	PR_ERR_CFM_LRN_FAIL,			// 35
	PR_ERR_CFM_SRCH_FAIL,
	PR_WARN_SMALL_SRCH_WIN,
	PR_ERR_SHI_CONFIRM_FAILED,
};

static char *cPR_ErrorMsg[] =
{
	"Pattern Not Found",			// 0
	"Pattern not loaded", 
	"Invalid search area", 
	"Defective die", 
	"Algorithm not learnt", 
	"Too many die",					// 5
	"Search window too small", 
	"Too dark", 
	"Too bright", 
	"Rotation limit exceeded", 
	"Die too small",				// 10
	"Fail spatial", 
	"Fail uniqueness", 
	"Ink die", 
	"Chip die", 
	"Crack die",					// 15
	"Scratch die", 
	"Bad cut die", 
	"Parameter error", 
	"No PR reference memory", 
	"Camera not linked",			// 20
	"UID not match", 
	"No PR pattern record", 
	"Pad centre not found",
	"Lighting too bright", 
	"Lighting too dim",				// 25
	"Load lead fail", 
	"Search lead fail", 
	"Determine lead tip failed", 
	"Load angle range too large",
	"Cannot find edges",			// 30
	"Inconsistence lead width", 
	"Check clearance fail", 
	"Load template out range", 
	"File Already Existed", 
	"Learn Cmf Patttern Fail",		//35
	"Search Cmf Pattern Fail",
	"Search Window too small",
	"Confrim Pattern Fail"
};

PR_ZOOM_FACTOR  g_emZoomFactor[PR_CAMERA_END] = {PR_ZOOM_FACTOR_1X, }; //20150304

VOID PRS_DisplayError(PRU *pPRU, PR_ERROR_TYPE emErrorType, PR_WORD uwErrorCode)
{
	PR_GET_ERROR_STR_CMD     stGetErrorStrCmd;
	PR_GET_ERROR_STR_RPY     stGetErrorStrRpy;

	CString szDisplay = _T("");

	INT nChar = 0;
	INT nRow = 1;
	INT nStart = 0;
	INT nEnd = 0;

	CHAR	cMessage[4][PR_MAX_COL_IN_TEXT_COORD];
	CHAR	cErrorMsg[100];

	for (INT i = 0; i < 100; i++)
	{
		cErrorMsg[i] = 0;
	}

	for (INT i = 0; i < PR_MAX_COL_IN_TEXT_COORD; i++)
	{
		cMessage[0][i] = 0;
		cMessage[1][i] = 0;
		cMessage[2][i] = 0;
		cMessage[3][i] = 0;
	}

	stGetErrorStrCmd.emErrorType = emErrorType;
	stGetErrorStrCmd.uwErrorCode = uwErrorCode;

	PR_GetErrorStrCmd(&stGetErrorStrCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stGetErrorStrRpy);


	if (
		PR_ERROR_STATUS(stGetErrorStrRpy.stStatus.uwPRStatus) || 
		stGetErrorStrRpy.stStatus.uwCommunStatus != PR_COMM_NOERR
	   )
	{
		return;
	}
	else
	{
		// Clear All Text First
		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW4);
		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW3);
		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW2);
		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);

		strcpy(cErrorMsg, (char*)stGetErrorStrRpy.aubErrorStr);

		// Check for END of Line
		nChar = PR_MAX_COL_IN_TEXT_COORD;
		nStart = 0;
		nEnd = 0;

		// Search for ""
		for (; nChar > 0; nChar--)
		{
			if ((cErrorMsg[nChar + nEnd + 1] == 32 || cErrorMsg[nChar + nEnd + 1] == 0) && nChar == PR_MAX_COL_IN_TEXT_COORD)
			{
				if (nRow >= 1 && nRow < 5)
				{
					strncpy(cMessage[(nRow - 1) % 4], cErrorMsg + nEnd, nChar);
					PRS_DisplayText(pPRU->nCamera, 1, nRow - 1 + PRS_MSG_ROW4, cMessage[nRow - 1]);
					
					// Next Row
					nRow++;
					//nStart = nChar+1;
					nEnd = nEnd + nChar + 1;
					nChar = PR_MAX_COL_IN_TEXT_COORD;
				}
			}
			else if (cErrorMsg[nChar + nEnd] == 32)		// 32 is the " "
			{
				if (nRow >= 1 && nRow < 5)
				{
					// 20140804 Yip: Klocwork Fix - Change &cMessage[0][0] + (PR_MAX_COL_IN_TEXT_COORD * (nRow - 1)) To cMessage[nRow - 1]
					// Print Error
					strncpy(cMessage[(nRow - 1) % 4], cErrorMsg + nEnd, nChar);
					PRS_DisplayText(pPRU->nCamera, 1, nRow - 1 + PRS_MSG_ROW4, cMessage[nRow - 1]);
				}
				
				// Next Row
				nRow++;
				//nStart = nChar+1;
				nEnd = nEnd + nChar + 1;
				nChar = PR_MAX_COL_IN_TEXT_COORD;
				i = PR_MAX_COL_IN_TEXT_COORD;
			}
			else if (cErrorMsg[nChar + nEnd] == 0)			// Check for END of Line
			{
				if (nRow >= 1 && nRow < 5)
				{
					// 20140804 Yip: Klocwork Fix - Change &cMessage[0][0] + (PR_MAX_COL_IN_TEXT_COORD * (nRow - 1)) To cMessage[nRow - 1]
					// Print Error
					strncpy(cMessage[(nRow - 1) % 4], cErrorMsg + nEnd, nChar);
					PRS_DisplayText(pPRU->nCamera, 1, nRow - 1 + PRS_MSG_ROW4, cMessage[nRow - 1]);
				}
				break;
			}
		}
	}
}

VOID PRS_InitPRU(PRU *pPRU, LONG lCam, LONG emID, CString szStationName, CString szFileRecordName, LONG lCamMoveGroup1, LONG lCamMoveGroup2, LONG lCamMoveGroup3, LONG lCamMoveGroup4)
{
	switch (lCam)
	{
	case NO_CAM:
		pPRU->nCamera					= NO_CAM;
		pPRU->emCameraNo				= PR_CAMERA_1;
		pPRU->emPurpose[PURPOSE0]		= PR_PURPOSE_CAM_A0;
		pPRU->emPurpose[PURPOSE1]		= PR_PURPOSE_USER_DEFINED_1;
		pPRU->ubSenderID				= PRS_MENU_SID;
		pPRU->ubReceiverID				= PRS_MENU_RID;
		pPRU->emRecordType				= PRS_DIE_TYPE;
		pPRU->dFOV						= 1.0;			//20121102
		pPRU->bIs5MBCam = FALSE;
		break;

	case ACF_CAM:
		pPRU->nCamera					= ACF_CAM;
		pPRU->emCameraNo				= PR_CAMERA_1;
		pPRU->emPurpose[PURPOSE0]		= PR_PURPOSE_CAM_A0;
		pPRU->emPurpose[PURPOSE1]		= PR_PURPOSE_USER_DEFINED_1;
		pPRU->ubSenderID				= PRS_ACF_SID;
		pPRU->ubReceiverID				= PRS_ACF_RID;
		pPRU->emRecordType				= PRS_DIE_TYPE;
		pPRU->dFOV						= 5.1;			//20121102
		pPRU->bIs5MBCam = FALSE;
		break;

	case ACFUL_CAM:
		pPRU->nCamera					= ACFUL_CAM;
		pPRU->emCameraNo				= PR_CAMERA_2;
		pPRU->emPurpose[PURPOSE0]		= PR_PURPOSE_CAM_A1;
		pPRU->emPurpose[PURPOSE1]		= PR_PURPOSE_USER_DEFINED_2;
		pPRU->ubSenderID				= PRS_ACFUL_SID;
		pPRU->ubReceiverID				= PRS_ACFUL_RID;
		pPRU->emRecordType				= PRS_DIE_TYPE;
		pPRU->dFOV						= 83.0;			//20121102
		pPRU->bIs5MBCam = FALSE;
		break;
	
	default:
		ASSERT(0); //error must not run here
		break;
	}

	pPRU->emID				= (PRU_ID)emID;

	pPRU->bCalibrated		= FALSE;
	pPRU->m2p_xform.a		= 0.0;
	pPRU->m2p_xform.b		= 0.0;
	pPRU->m2p_xform.c		= 0.0;
	pPRU->m2p_xform.d		= 0.0;
	pPRU->p2m_xform.a		= 0.0;
	pPRU->p2m_xform.b		= 0.0;
	pPRU->p2m_xform.c		= 0.0;
	pPRU->p2m_xform.d		= 0.0;

#if 1 //20140416
	pPRU->bCalibrated_2		= FALSE;
	pPRU->m2p_xform_2.a		= 0.0;
	pPRU->m2p_xform_2.b		= 0.0;
	pPRU->m2p_xform_2.c		= 0.0;
	pPRU->m2p_xform_2.d		= 0.0;
	pPRU->p2m_xform_2.a		= 0.0;
	pPRU->p2m_xform_2.b		= 0.0;
	pPRU->p2m_xform_2.c		= 0.0;
	pPRU->p2m_xform_2.d		= 0.0;
#endif

	pPRU->bCalibrated_3		= FALSE;
	pPRU->m2p_xform_3.a		= 0.0;
	pPRU->m2p_xform_3.b		= 0.0;
	pPRU->m2p_xform_3.c		= 0.0;
	pPRU->m2p_xform_3.d		= 0.0;
	pPRU->p2m_xform_3.a		= 0.0;
	pPRU->p2m_xform_3.b		= 0.0;
	pPRU->p2m_xform_3.c		= 0.0;
	pPRU->p2m_xform_3.d		= 0.0;

	pPRU->bCalibrated_4		= FALSE;
	pPRU->m2p_xform_4.a		= 0.0;
	pPRU->m2p_xform_4.b		= 0.0;
	pPRU->m2p_xform_4.c		= 0.0;
	pPRU->m2p_xform_4.d		= 0.0;
	pPRU->p2m_xform_4.a		= 0.0;
	pPRU->p2m_xform_4.b		= 0.0;
	pPRU->p2m_xform_4.c		= 0.0;
	pPRU->p2m_xform_4.d		= 0.0;

	pPRU->eCamMoveGroupCurrent = (CAM_MOVEMENT_GROUP_NUM) lCamMoveGroup1;		//current camera movement group for xy calib, search, ... etc
	pPRU->eCamMoveGroup1 = (CAM_MOVEMENT_GROUP_NUM) lCamMoveGroup1; //20150213 Camera movement group for first ABCD
	pPRU->eCamMoveGroup2 = (CAM_MOVEMENT_GROUP_NUM) lCamMoveGroup2; //20150213 Camera movement group for second ABCD
	pPRU->eCamMoveGroup3 = (CAM_MOVEMENT_GROUP_NUM) lCamMoveGroup3; //20170228
	pPRU->eCamMoveGroup4 = (CAM_MOVEMENT_GROUP_NUM) lCamMoveGroup4;

	pPRU->uwCoaxialLevel	= 0;
	pPRU->uwCoaxial1Level	= 0;
	pPRU->uwCoaxial2Level	= 0;
	pPRU->uwRingLevel		= 0;
	pPRU->uwSideLevel		= 0;
	pPRU->uwSide1Level		= 0;	// 20141023
	pPRU->uwBackLevel		= 0;	// 20141023
	pPRU->uwBack1Level		= 0;	// 20141023

	pPRU->ulExposureTime = (PR_EXPOSURE_TIME) 0;

	pPRU->bLoaded			= FALSE;

	pPRU->lPBMethod			= 0;

	pPRU->szStnName			= szStationName;
	pPRU->szFileName		= szFileRecordName;


	pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].x	= PR_DEF_CENTRE_X;
	pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].y	= PR_DEF_CENTRE_Y;
	pPRU->arcoLrnDieCorners[PR_UPPER_LEFT].x		= PR_DEF_CENTRE_X;
	pPRU->arcoLrnDieCorners[PR_UPPER_LEFT].y		= PR_DEF_CENTRE_Y;
	pPRU->arcoLrnDieCorners[PR_LOWER_LEFT].x		= PR_DEF_CENTRE_X;
	pPRU->arcoLrnDieCorners[PR_LOWER_LEFT].y		= PR_DEF_CENTRE_Y;
	pPRU->arcoLrnDieCorners[PR_LOWER_RIGHT].x	= PR_DEF_CENTRE_X;
	pPRU->arcoLrnDieCorners[PR_LOWER_RIGHT].y	= PR_DEF_CENTRE_Y;

	pPRU->arcoDieCorners[PR_UPPER_RIGHT].x	= PR_DEF_CENTRE_X;
	pPRU->arcoDieCorners[PR_UPPER_RIGHT].y	= PR_DEF_CENTRE_Y;
	pPRU->arcoDieCorners[PR_UPPER_LEFT].x	= PR_DEF_CENTRE_X;
	pPRU->arcoDieCorners[PR_UPPER_LEFT].y	= PR_DEF_CENTRE_Y;
	pPRU->arcoDieCorners[PR_LOWER_LEFT].x	= PR_DEF_CENTRE_X;
	pPRU->arcoDieCorners[PR_LOWER_LEFT].y	= PR_DEF_CENTRE_Y;
	pPRU->arcoDieCorners[PR_LOWER_RIGHT].x	= PR_DEF_CENTRE_X;
	pPRU->arcoDieCorners[PR_LOWER_RIGHT].y	= PR_DEF_CENTRE_Y;

	pPRU->rcoDieCentre.x		= PR_DEF_CENTRE_X;
	pPRU->rcoDieCentre.y		= PR_DEF_CENTRE_Y;
	pPRU->rcoDieCentre.x	= PR_DEF_CENTRE_X; //20150728 float PR
	pPRU->rcoDieCentre.y	= PR_DEF_CENTRE_Y;
	pPRU->bUseFloatingPointResult = FALSE;
	pPRU->fAng				= 0.0;
	pPRU->fScore			= 0.0;
	pPRU->fScale			= 0.0;	// 20151201

	pPRU->bStatLoaded		= FALSE;
	pPRU->fStatMean			= 0.0;
	pPRU->fStatStdev		= 0.0;
	
	pPRU->dPostBdRejectTol	= 50.0;
	pPRU->dTotalDefectArea	= 0.0;

	pPRU->lStationID		= 0;
	pPRU->lImageID			= 0;
	pPRU->lACFResult		= PR_ACF_RESULT_TYPE_GOOD;
	pPRU->bUseFirstABCD		= TRUE; //20140416

	PRS_InitLrnCmd(pPRU); //vision UI

	PRS_InitManualLrnDieCmd(pPRU);
	PRS_InitSrchDieCmd(pPRU);

	PRS_InitLrnShapeCmd(pPRU);
	PRS_InitDetectShapeCmd(pPRU);

	PRS_InitRGBCenterCmd(pPRU);		//20150108
	PRS_InitChangeRecordCmd(pPRU);

	PRS_InitLoadTmplCmd(pPRU);
	PRS_InitSrchTmplCmd(pPRU);

	// 20141023 Yip: Add Kerf Fitting Functions
	PRS_InitLrnKerfCmd(pPRU);
	PRS_InitInspKerfCmd(pPRU);
	
	//20150106	Add Extract Shape Function
	PRS_InitExpShapeCmd(pPRU);
	//20150109
	PRS_InitExpObjectCmd(pPRU);

	PRS_InitImageStatInfoCmd(pPRU);

	//Sub
	PRS_InitGrabShareImgCmd(pPRU);
	PRS_InitGrabDispImgCmd(pPRU);
	PRS_InitLrnACFCmd(pPRU);
	PRS_InitSrchACFCmd(pPRU);


	PRS_InitUploadRecordProcessCmd(pPRU);
	PRS_InitDownloadRecordProcessCmd(pPRU);
}




// Text Screen and Graphic Functions
//

VOID PRS_SelectCameraChannel(INT const iCam, PR_UBYTE &ubSenderIDOutput, PR_UBYTE &ubReceiverIDOutput)
{
	ubSenderIDOutput		= PRS_MENU_SID;
	ubReceiverIDOutput		= PRS_MENU_RID;

	switch (iCam)
	{
	case PR_CAMERA_1:
		ubSenderIDOutput		= PRS_ACF_SID;
		ubReceiverIDOutput		= PRS_ACF_RID;

	case PR_CAMERA_2:
		ubSenderIDOutput		= PRS_ACFUL_SID;
		ubReceiverIDOutput		= PRS_ACFUL_RID;
		break;
	}
	ubSenderID = ubSenderIDOutput;
	ubReceiverID = ubReceiverIDOutput;
}

PRU &PRS_SetCameraPRU_FOR_ZOOM(ULONG lCam) 
{
	switch (lCam)
	{
	case ACF_CAM: 
		return pruACF_WHCalib;
		
	case ACFUL_CAM: 
		return pruACFUL_Calib;

	default:
		//DisplayMessage("PRS_SetCameraPRU_FOR_ZOOM Error!");
		return pruACF_WHCalib;
	}
}

PRU &PRS_SetCameraPRU(ULONG lCam) 
{
	switch (lCam)
	{
	case ACF_CAM: 
		return pruACF_WHCalib;

	case ACFUL_CAM: 
		return pruACFUL_Calib;

	default:
		//DisplayMessage("PRS_SetCameraPRU Error!");
		return pruACF_WHCalib;
	}
}

VOID PRS_ClearScreen(INT const iCam)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;
	PR_CLEAR_SCREEN_RPY	stRpy;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PR_ClearScreenCmd(ubCurSenderID, ubCurReceiverID, &stRpy);
}

// note: can support a max of ? characters
VOID PRS_MenuDisplayText(PR_WORD uwCol, PR_WORD uwRow, char const *Str, ...)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PRS_MenuClearTextRow(uwRow);

	PR_DISPLAY_CHAR_CMD	stCmd;
	PR_DISPLAY_CHAR_RPY	stRpy;

	va_list arglist;
	char buf[28];
	va_start(arglist, Str);
	vsprintf(buf, Str, arglist);
	va_end(arglist);

	stCmd.tcoPosition.x = uwCol;
	stCmd.tcoPosition.y = uwRow;
	stCmd.emColor = PR_COLOR_GREEN;
	stCmd.uwNChar = strlen(buf);
	stCmd.pubChar = (unsigned char*)buf;

	bDirtyRow[uwRow] = TRUE;
	PR_DisplayCharCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

VOID PRS_MenuDisplayTextExt(PR_WORD uwCol, PR_WORD uwRow, PRS_CHAR_SIZE emCharSize, char const *Str, ...)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PRS_MenuClearTextRow(uwRow);

	PR_DISPLAY_CHAR_EXT_CMD stCmd;
	PR_DISPLAY_CHAR_RPY	stRpy;

	va_list arglist;
	char buf[28];
	va_start(arglist, Str);
	vsprintf(buf, Str, arglist);
	va_end(arglist);

	stCmd.emCharSize = (PR_CHAR_SIZE) emCharSize; //new ext option
	stCmd.emBackgroundColor = PR_COLOR_TRANSPARENT; //new ext option

	stCmd.tcoPosition.x = uwCol;
	stCmd.tcoPosition.y = uwRow;
	stCmd.emColor = PR_COLOR_GREEN;
	stCmd.uwNChar = strlen(buf);
	stCmd.pubChar = (unsigned char*)buf;
	

	bDirtyRow[uwRow] = TRUE;
	PR_DisplayCharExtCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

VOID PRS_MenuClearTextRow(PR_WORD uwRow)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	if (!bDirtyRow[uwRow])
	{
		return;
	}

	PR_CLEAR_TEXT_ROW_CMD	stCmd;
	PR_CLEAR_TEXT_ROW_RPY	stRpy;

	stCmd.wTextRow = uwRow;

	bDirtyRow[uwRow] = FALSE;
	PR_ClearTextRowCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

// note: can support a max of ? characters
VOID PRS_DisplayTextOrg(INT const iCam, PR_WORD uwCol, PR_WORD uwRow, char const *Str, ...)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	if (uwRow >= PR_MAX_ROW_IN_TEXT_COORD + 1)
	{
		return;
	}

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PRS_ClearTextRow(iCam, uwRow);

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);

	PR_DISPLAY_CHAR_CMD	stCmd;
	PR_DISPLAY_CHAR_RPY	stRpy;

	va_list arglist;
	char buf[28];
	va_start(arglist, Str);
	vsprintf(buf, Str, arglist);
	va_end(arglist);

	stCmd.tcoPosition.x = uwCol;
	stCmd.tcoPosition.y = uwRow;
	stCmd.emColor = PR_COLOR_GREEN;
	stCmd.uwNChar = strlen(buf);
	stCmd.pubChar = (unsigned char*)buf;

	bDirtyRow[uwRow] = TRUE;
	PR_DisplayCharCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

VOID PRS_DisplayTextExt(INT const iCam, PR_WORD uwCol, PR_WORD uwRow, PRS_CHAR_SIZE emCharSize, char *pBuf) //20151204
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	if (uwRow >= PR_MAX_ROW_IN_TEXT_COORD + 1)
	{
		return;
	}

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PRS_ClearTextRow(iCam, uwRow);

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);

	PR_DISPLAY_CHAR_EXT_CMD stCmd;
	PR_DISPLAY_CHAR_RPY	stRpy;

	//va_list arglist;
	//char buf[28];
	//va_start(arglist, Str);
	//vsprintf(buf, Str, arglist);
	//va_end(arglist);

	stCmd.emCharSize = (PR_CHAR_SIZE) emCharSize; //new ext option
	stCmd.emBackgroundColor = PR_COLOR_TRANSPARENT; //new ext option

	stCmd.tcoPosition.x = uwCol;
	stCmd.tcoPosition.y = uwRow;
	stCmd.emColor = PR_COLOR_GREEN;
	stCmd.uwNChar = strlen(pBuf);
	stCmd.pubChar = (unsigned char*)pBuf;

	bDirtyRow[uwRow] = TRUE;
	PR_DisplayCharExtCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

VOID PRS_DisplayText(INT const iCam, PR_WORD uwCol, PR_WORD uwRow, char const *Str, ...)  //20151204
{
	PRU *pPRU = NULL;

	pPRU = &PRS_SetCameraPRU(iCam);

	va_list arglist;
	char buf[32];
	va_start(arglist, Str);
	vsprintf(buf, Str, arglist);
	va_end(arglist);

	if (pPRU->stZoomSensorCmd.emZoomMode == PR_ZOOM_SENSOR_MODE_1X_ZOOM_NORMAL)		//VGA 512 x 512)
	{
		PRS_DisplayTextExt(iCam, uwCol, uwRow, CHAR_SIZE_SMALL, &buf[0]);
	}
	else
	{
		PRS_DisplayTextExt(iCam, uwCol, uwRow, CHAR_SIZE_MEDIUM, &buf[0]);
	}
}

VOID PRS_ClearTextRow(INT const iCam, PR_WORD uwRow)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	if (uwRow >= PR_MAX_ROW_IN_TEXT_COORD + 1)
	{
		return;
	}

	if (!bDirtyRow[uwRow])
	{
		return;
	}

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);

	PR_CLEAR_TEXT_ROW_CMD	stCmd;
	PR_CLEAR_TEXT_ROW_RPY	stRpy;

	stCmd.wTextRow = uwRow;

	bDirtyRow[uwRow] = FALSE;
	PR_ClearTextRowCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

VOID PRS_SetReverseMode(INT const iCam, PR_BOOLEAN bMode)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;
	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);

	PR_SET_REVERSE_MODE_CMD	stCmd;
	PR_SET_REVERSE_MODE_RPY	stRpy;

	stCmd.emReverseMode = bMode;

	PR_SetReverseModeCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

#ifdef NO_INDIRECTVIEW_TXT
VOID PRS_GetPrintPRError(PR_UWORD uwPRStatus, CString &szString) //20151007
{
	CString szDisplay = _T("");

	// find and print known error
	for (INT i = 0; i < (sizeof(uwPR_Error) / sizeof(uwPR_Error[0])); i++)
	{
		if (uwPR_Error[i] == uwPRStatus)
		{
			//PRS_DisplayText(iCam, 1, PRS_MSG_ROW1, cPR_ErrorMsg[i]);
			szString.Format("%s, ", cPR_ErrorMsg[i]);
			return;
		}
	}
	// unknown error
	//PRS_DisplayText(iCam, 1, PRS_MSG_ROW1, "Unknown PRS Error: %x", uwPRStatus);
	szString.Format("Unknown PRS Error: %x", uwPRStatus);
	return;
}
#endif

VOID PRS_PrintPRError(INT const iCam, PR_WORD uwRow, PR_UWORD uwPRStatus)
{
	CString szDisplay = _T("");

	PRS_ClearTextRow(iCam, uwRow);

	// find and print known error
	for (INT i = 0; i < (sizeof(uwPR_Error) / sizeof(uwPR_Error[0])); i++)
	{
		if (uwPR_Error[i] == uwPRStatus)
		{
			PRS_DisplayText(iCam, 1, PRS_MSG_ROW1, cPR_ErrorMsg[i]);
			return;
		}
	}
	// unknown error
	PRS_DisplayText(iCam, 1, PRS_MSG_ROW1, "Unknown PRS Error: %x", uwPRStatus);
}

VOID PRS_PrintACFResult(INT const iCam, PR_WORD uwRow, PR_ACF_RESULT_TYPE uwACFResult)
{
	CString szDisplay = _T("");

	PRS_ClearTextRow(iCam, uwRow);

	// find and print known error
	for (INT i = 0; i < (sizeof(emPR_ACFResult) / sizeof(emPR_ACFResult[0])); i++)
	{
		if (emPR_ACFResult[i] == uwACFResult)
		{
			PRS_DisplayText(iCam, 2, PRS_MSG_ROW1, cPR_ACFResultMsg[i]);
			return;
		}
	}
}

#ifdef NO_INDIRECTVIEW_TXT
VOID PRS_GetPrintACFResult(INT const iCam, PR_WORD uwRow, PR_ACF_RESULT_TYPE uwACFResult, CString &szString) //20151007
{
	CString szDisplay = _T("");

	// find and print known error
	for (INT i = 0; i < (sizeof(emPR_ACFResult) / sizeof(emPR_ACFResult[0])); i++)
	{
		if (emPR_ACFResult[i] == uwACFResult)
		{
			PRS_DisplayText(iCam, 2, PRS_MSG_ROW1, cPR_ACFResultMsg[i]);
			szString.Format("%s, ", cPR_ACFResultMsg[i]);
			return;
		}
	}
}
#endif

VOID PRS_DrawRect(INT const iCam, PR_COORD const coCorner1, PR_COORD const coCorner2, PR_COLOR const emColor)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);

	PR_DRAW_RECT_CMD	stCmd;
	PR_DRAW_RECT_RPY	stRpy;

	stCmd.stWin.coCorner1 = coCorner1;
	stCmd.stWin.coCorner2 = coCorner2;
	stCmd.emColor = emColor;

	PR_DrawRectCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

VOID PRS_EraseRect(INT const iCam, PR_COORD const coCorner1, PR_COORD const coCorner2)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);
	
	PR_DRAW_RECT_CMD	stCmd;
	PR_DRAW_RECT_RPY	stRpy;

	stCmd.stWin.coCorner1 = coCorner1;
	stCmd.stWin.coCorner2 = coCorner2;

	PR_EraseRectCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

VOID PRS_DrawLine(INT const iCam, PR_COORD const coPoint1, PR_COORD const coPoint2, PR_COLOR const emColor)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;
	
	PR_DRAW_LINE_CMD	stCmd;
	PR_DRAW_LINE_RPY	stRpy;

	PR_InitDrawLineCmd(&stCmd);

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);

	stCmd.stLine.coPoint1	= coPoint1;
	stCmd.stLine.coPoint2	= coPoint2;
	stCmd.emColor			= emColor;
	stCmd.emLineType		= PR_LINE_NORMAL;

	PR_DrawLineCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

VOID PRS_EraseLine(INT const iCam, PR_COORD const coPoint1, PR_COORD const coPoint2)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);

	PR_DRAW_LINE_CMD	stCmd;
	PR_DRAW_LINE_RPY	stRpy;

	stCmd.stLine.coPoint1	= coPoint1;
	stCmd.stLine.coPoint2	= coPoint2;
	stCmd.emLineType		= PR_LINE_NORMAL;

	PR_EraseLineCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

VOID PRS_MenuDrawCross(PR_COORD const coCrossCentre, INT const iCrossSize, PR_COLOR const emColor)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;
	
	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PR_DRAW_CROSS_CMD	stCmd;
	PR_DRAW_CROSS_RPY	stRpy;

	PR_SIZE szCrossSize = {iCrossSize, iCrossSize};

	stCmd.coCrossCentre		= coCrossCentre;
	stCmd.szCrossSize		= szCrossSize;
	stCmd.emColor			= emColor;

	PR_DrawCrossCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

VOID PRS_DrawCross(INT const iCam, PR_COORD const coCrossCentre, INT const iCrossSize, PR_COLOR const emColor)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);

	PR_DRAW_CROSS_CMD	stCmd;
	PR_DRAW_CROSS_RPY	stRpy;

	PR_SIZE szCrossSize = {iCrossSize, iCrossSize};

	stCmd.coCrossCentre = coCrossCentre;
	stCmd.szCrossSize = szCrossSize;
	stCmd.emColor = emColor;

	PR_DrawCrossCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

VOID PRS_DrawCross(INT const iCam, PR_RCOORD const rcoCrossCentre, INT const iCrossSize, PR_COLOR const emColor) //20150728 float PR
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);

	PR_DRAW_CROSS_CMD	stCmd;
	PR_DRAW_CROSS_RPY	stRpy;

	PR_SIZE szCrossSize = {iCrossSize, iCrossSize};

	stCmd.coCrossCentre.x = (PR_WORD)rcoCrossCentre.x;
	stCmd.coCrossCentre.y = (PR_WORD)rcoCrossCentre.y;
	stCmd.szCrossSize = szCrossSize;
	stCmd.emColor = emColor;

	PR_DrawCrossCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

VOID PRS_EraseCross(INT const iCam, PR_COORD const coCrossCentre, INT const iCrossSize)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);

	PR_DRAW_CROSS_CMD	stCmd;
	PR_DRAW_CROSS_RPY	stRpy;

	PR_SIZE szCrossSize = {iCrossSize, iCrossSize};

	stCmd.coCrossCentre = coCrossCentre;
	stCmd.szCrossSize = szCrossSize;

	PR_EraseCrossCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

VOID PRS_DrawHomeCursor(INT const iCam, BOOL bShowTmpl)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);

	PR_DRAW_HOME_CURSOR_CMD	stCmd;
	PR_DRAW_HOME_CURSOR_RPY	stRpy;

	if (bShowTmpl)
	{
		stCmd.emTmplSize = PR_TMPL_SIZE_3;
	}
	else
	{
		stCmd.emTmplSize = PR_TMPL_SIZE_DISABLE;
	}

	/*if (iCam == INWH_CAM)
	{
		PR_COORD coCorner1, coCorner2;
		coCorner1.x = PR_MIN_COORD;
		coCorner1.y = PR_DEF_CENTRE_Y - 2;
		coCorner2.x = PR_MAX_COORD;
		coCorner2.y = PR_DEF_CENTRE_Y + 1;
		PRS_DrawRect(iCam, coCorner1, coCorner2, PR_COLOR_GREEN);
		coCorner1.x = PR_MIN_COORD;
		coCorner1.y = PR_DEF_CENTRE_Y - 1;
		coCorner2.x = PR_MAX_COORD;
		coCorner2.y = PR_DEF_CENTRE_Y;
		PRS_DrawRect(iCam, coCorner1, coCorner2, PR_COLOR_GREEN);
		coCorner1.x = PR_DEF_CENTRE_X - 2;
		coCorner1.y = PR_MIN_COORD;
		coCorner2.x = PR_DEF_CENTRE_X + 1;
		coCorner2.y = PR_MAX_COORD;
		PRS_DrawRect(iCam, coCorner1, coCorner2, PR_COLOR_GREEN);
		coCorner1.x = PR_DEF_CENTRE_X - 1;
		coCorner1.y = PR_MIN_COORD;
		coCorner2.x = PR_DEF_CENTRE_X;
		coCorner2.y = PR_MAX_COORD;
		PRS_DrawRect(iCam, coCorner1, coCorner2, PR_COLOR_GREEN);

		coCorner1.x = PR_MIN_COORD;
		coCorner1.y = PR_DEF_CENTRE_Y - 3;
		coCorner2.x = PR_MAX_COORD;
		coCorner2.y = PR_DEF_CENTRE_Y + 2;
		PRS_DrawRect(iCam, coCorner1, coCorner2, PR_COLOR_GREEN);
		coCorner1.x = PR_MIN_COORD;
		coCorner1.y = PR_DEF_CENTRE_Y - 4;
		coCorner2.x = PR_MAX_COORD;
		coCorner2.y = PR_DEF_CENTRE_Y + 3;
		PRS_DrawRect(iCam, coCorner1, coCorner2, PR_COLOR_GREEN);
		coCorner1.x = PR_DEF_CENTRE_X - 3;
		coCorner1.y = PR_MIN_COORD;
		coCorner2.x = PR_DEF_CENTRE_X + 2;
		coCorner2.y = PR_MAX_COORD;
		PRS_DrawRect(iCam, coCorner1, coCorner2, PR_COLOR_GREEN);
		coCorner1.x = PR_DEF_CENTRE_X - 4;
		coCorner1.y = PR_MIN_COORD;
		coCorner2.x = PR_DEF_CENTRE_X + 3;
		coCorner2.y = PR_MAX_COORD;
		PRS_DrawRect(iCam, coCorner1, coCorner2, PR_COLOR_GREEN);
	}
	else
	{*/
		stCmd.coPosition.x = PR_DEF_CENTRE_X;
		stCmd.coPosition.y = PR_DEF_CENTRE_Y;

		PR_DrawHomeCursorCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
	//}
}

VOID PRS_DrawSmallCursor(INT const iCam, PR_COORD const coPosition)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);

	PR_DRAW_SMALL_CURSOR_CMD	stCmd;
	PR_DRAW_SMALL_CURSOR_RPY	stRpy;

	stCmd.coPosition = coPosition;

	PR_DrawSmallCursorCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

PR_COORD PRS_ReadSmallCursor(INT const iCam)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);

	PR_READ_SMALL_CURSOR_RPY	stRpy;

	PR_ReadSmallCursorCmd(ubCurSenderID, ubCurReceiverID, &stRpy);

	return stRpy.coPosition;
}

VOID PRS_DrawSrchArea(INT const iCam, PR_WIN const stWin)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);

	PR_DRAW_SRCH_AREA_CMD	stCmd;
	PR_DRAW_SRCH_AREA_RPY	stRpy;

	stCmd.stWin = stWin;

	PR_DrawSrchAreaCmd(&stCmd, ubCurSenderID, ubCurReceiverID, &stRpy);
}

VOID PRS_DrawAndEraseCursor(INT const iCam, PR_COORD stUpperLeftCorner, PR_COORD stLowerRightCorner, BOOL bDraw)
{
	PR_COLOR	emColor;
	PR_COORD	stStartPos;
	PR_COORD	stEndPos;

	if (bDraw)
	{
		emColor = PR_COLOR_GREEN;
	}
	else
	{
		emColor = PR_COLOR_TRANSPARENT;
	}

	// draw || erase upper right
	stStartPos.x = stLowerRightCorner.x;
	stStartPos.y = stUpperLeftCorner.y;

	stEndPos.x = stStartPos.x - (25 * PR_SCALE_FACTOR);
	stEndPos.y = stStartPos.y;
	PRS_DrawLine(iCam, stStartPos, stEndPos, emColor);

	stEndPos.x = stStartPos.x;
	stEndPos.y = stStartPos.y + (25 * PR_SCALE_FACTOR);
	PRS_DrawLine(iCam, stStartPos, stEndPos, emColor);

	// draw || erase upper left
	stStartPos.x = stUpperLeftCorner.x;
	stStartPos.y = stUpperLeftCorner.y;

	stEndPos.x = stStartPos.x + (25 * PR_SCALE_FACTOR);
	stEndPos.y = stStartPos.y;
	PRS_DrawLine(iCam, stStartPos, stEndPos, emColor);

	stEndPos.x = stStartPos.x;
	stEndPos.y = stStartPos.y + (25 * PR_SCALE_FACTOR);
	PRS_DrawLine(iCam, stStartPos, stEndPos, emColor);

	// draw || erase lower left
	stStartPos.x = stUpperLeftCorner.x;
	stStartPos.y = stLowerRightCorner.y;

	stEndPos.x = stStartPos.x + (25 * PR_SCALE_FACTOR);
	stEndPos.y = stStartPos.y;
	PRS_DrawLine(iCam, stStartPos, stEndPos, emColor);

	stEndPos.x = stStartPos.x;
	stEndPos.y = stStartPos.y - (25 * PR_SCALE_FACTOR);
	PRS_DrawLine(iCam, stStartPos, stEndPos, emColor);

	// draw || erase lower right
	stStartPos.x = stLowerRightCorner.x;
	stStartPos.y = stLowerRightCorner.y;
	stEndPos.x = stStartPos.x - (25 * PR_SCALE_FACTOR);
	stEndPos.y = stStartPos.y;
	PRS_DrawLine(iCam, stStartPos, stEndPos, emColor);

	stEndPos.x = stStartPos.x;
	stEndPos.y = stStartPos.y - (25 * PR_SCALE_FACTOR);
	PRS_DrawLine(iCam, stStartPos, stEndPos, emColor);
}

PR_COORD PRS_GetGraphicsStepSize()
{
	PR_COORD coGraphicStep;

	PR_GetGraphicsStepSize(&coGraphicStep);

	return coGraphicStep;
}

// Mouse Control
//
VOID PRS_UseMouseWindow(PRU *pPRU, PR_COORD stUpperLeftCornerPos, PR_COORD stLowerRightCornerPos)
{
	PR_DEFINE_MOUSE_CTRL_OBJ_CMD	stDefineCmd;
	PR_DEFINE_MOUSE_CTRL_OBJ_RPY	stDefineRpy;

	PR_SET_MOUSE_CTRL_OBJ_CMD		stSetCmd;
	PR_SET_MOUSE_CTRL_OBJ_RPY		stSetRpy;

	if (!bPRUseMouse)
	{
		// Erase original region
		PRS_DrawAndEraseCursor(pPRU->nCamera, stUpperLeftCornerPos, stLowerRightCornerPos, FALSE);

		// Define mouse ctrl
		PR_InitDefineMouseCtrlObjCmd(&stDefineCmd);
		stDefineCmd.emShape = PR_SHAPE_TYPE_RECTANGLE;
		stDefineCmd.emColor = PR_COLOR_CYAN;
		stDefineCmd.ulObjectId = 1;
		PR_DefineMouseCtrlObjCmd(&stDefineCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stDefineRpy);

		// Set mouse ctrl
		PR_InitSetMouseCtrlObjCmd(&stSetCmd);
		stSetCmd.ulObjectId = 1;
		stSetCmd.emShape = PR_SHAPE_TYPE_RECTANGLE;
		stSetCmd.emColor = PR_COLOR_CYAN;
		stSetCmd.uwNumOfCorners = 2;
		if (stUpperLeftCornerPos.x == stLowerRightCornerPos.x || 
			stUpperLeftCornerPos.y == stLowerRightCornerPos.y)
		{
			stSetCmd.acoObjCorner[0].x = PR_DEF_WIN_ULC_X;
			stSetCmd.acoObjCorner[0].y = PR_DEF_WIN_ULC_Y;
			stSetCmd.acoObjCorner[1].x = PR_DEF_WIN_LRC_X;
			stSetCmd.acoObjCorner[1].y = PR_DEF_WIN_LRC_Y;
		}
		else
		{
			stSetCmd.acoObjCorner[0] = stUpperLeftCornerPos;
			stSetCmd.acoObjCorner[1] = stLowerRightCornerPos;
		}
		PR_SetMouseCtrlObjCmd(&stSetCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stSetRpy);

		bPRUseMouse = TRUE;
	}
}


VOID PRS_GetMouseWindow(PRU *pPRU, PR_COORD *pstUpperLeftCornerPos, PR_COORD *pstLowerRightCornerPos)
{
	PR_GET_MOUSE_CTRL_OBJ_CMD	stGetCmd;
	PR_GET_MOUSE_CTRL_OBJ_RPY	stGetRpy;
	PR_ERASE_MOUSE_CTRL_OBJ_CMD	stEraseCmd;
	PR_ERASE_MOUSE_CTRL_OBJ_RPY	stEraseRpy;

	if (bPRUseMouse)
	{
		// Get mouse ctrl
		PR_InitGetMouseCtrlObjCmd(&stGetCmd);
		stGetCmd.ulObjectId = 1;
		PR_GetMouseCtrlObjCmd(&stGetCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stGetRpy);

		if (stGetRpy.uwNumOfCorners == 2)
		{
			pstUpperLeftCornerPos->x = stGetRpy.acoObjCorner[0].x;
			pstUpperLeftCornerPos->y = stGetRpy.acoObjCorner[0].y;
			pstLowerRightCornerPos->x = stGetRpy.acoObjCorner[1].x;
			pstLowerRightCornerPos->y = stGetRpy.acoObjCorner[1].y;
		}

		// Erase the mouse object
		PR_InitEraseMouseCtrlObjCmd(&stEraseCmd);
		stEraseCmd.ulObjectId = 1;
		PR_EraseMouseCtrlObjCmd(&stEraseCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stEraseRpy);

		// Draw the input region
		PRS_DrawAndEraseCursor(pPRU->nCamera, stGetRpy.acoObjCorner[0], stGetRpy.acoObjCorner[1], TRUE);

		bPRUseMouse = FALSE;
	}
}

VOID PRS_CancelMouseWindow(PRU *pPRU)
{
	PR_ERASE_MOUSE_CTRL_OBJ_CMD	stEraseCmd;
	PR_ERASE_MOUSE_CTRL_OBJ_RPY	stEraseRpy;

	if (bPRUseMouse)
	{
		// Erase the mouse object
		PR_InitEraseMouseCtrlObjCmd(&stEraseCmd);
		stEraseCmd.ulObjectId = 1;
		PR_EraseMouseCtrlObjCmd(&stEraseCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stEraseRpy);

		bPRUseMouse = FALSE;
	}
}

PR_VVOID PRS_MouseClick(PR_RFUNC_ON_MOUSE_CLICK_IN const *pstInPar, PR_RFUNC_ON_MOUSE_CLICK_OUT *pstOutPar)
{
	PRU *pPRU;

	if (nCamera == NO_CAM)
	{
		return;
	}

	pPRU = &PRS_SetCameraPRU(nCamera);

	PRS_ClearScreen(pPRU->nCamera);

	CString szMsg = "";
	szMsg.Format("Mouse Posn:%.0f, %.0f\n", pstInPar->rTargetX, pstInPar->rTargetY);
	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, szMsg);

	g_stCursor.prCoord.x = (PR_WORD)pstInPar->rTargetX;
	g_stCursor.prCoord.y = (PR_WORD)pstInPar->rTargetY;
	PRS_DrawCross(pPRU->nCamera, g_stCursor.prCoord, PRS_LARGE_SIZE, PR_COLOR_YELLOW);
}

PR_VVOID PRS_MouseDrag(PR_RFUNC_ON_MOUSE_DRAG_IN const *pstInPar, PR_RFUNC_ON_MOUSE_DRAG_OUT *pstOutPar)
{
	CString szMsg = "";
	CString szSourceMsg = "";

	PRU *pPRU;

	if (nCamera == NO_CAM)
	{
		return;
	}

	pPRU = &PRS_SetCameraPRU(nCamera);

	PRS_ClearScreen(pPRU->nCamera);

	switch (pstInPar->emMouseState)
	{
	case PR_MOUSE_EVENT_STATE_DOWN:
		szMsg.Format("DOWN | Posn:%.0f, %.0f", pstInPar->rcoDestination.x, pstInPar->rcoDestination.y);
		break;

	case PR_MOUSE_EVENT_STATE_DRAG:
		szMsg.Format("DRAG | Posn:%.0f, %.0f", pstInPar->rcoDestination.x, pstInPar->rcoDestination.y);
		break;

	case PR_MOUSE_EVENT_STATE_UP:
		szMsg.Format("UP | Posn:%.0f, %.0f", pstInPar->rcoDestination.x, pstInPar->rcoDestination.y);
		break;

	case PR_MOUSE_EVENT_STATE_END:
		szMsg.Format("END | Posn:%.0f, %.0f", pstInPar->rcoDestination.x, pstInPar->rcoDestination.y);
		break;
	}

	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, szMsg);
	PR_COORD prCoord;
	g_stCursor.prCoord.x = (PR_WORD)pstInPar->rcoDestination.x;
	g_stCursor.prCoord.y = (PR_WORD)pstInPar->rcoDestination.y;
	PRS_DrawCross(pPRU->nCamera, g_stCursor.prCoord, PRS_LARGE_SIZE, PR_COLOR_YELLOW);

	szMsg.Format("Distance:%.2f, Ang:%.2f", pstInPar->rDistance, pstInPar->rAngle);
	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, szMsg);

	prCoord.x = (PR_WORD)pstInPar->rcoSource.x;
	prCoord.y = (PR_WORD)pstInPar->rcoSource.y;
	szSourceMsg.Format("START Posn:%.0f, %.0f", pstInPar->rcoSource.x, pstInPar->rcoSource.y);
	PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW3, szSourceMsg);
	PRS_DrawCross(pPRU->nCamera, prCoord, PRS_LARGE_SIZE, PR_COLOR_GREY);

}

PR_VVOID PRS_InitHookRFuncOnMouseClick()
{
	PR_HOOK_RFUNC_ON_MOUSE_CLICK_CMD	stCmd;
	PR_COMMON_RPY						stCmdRpy;

	PRU *pPRU;

	if (nCamera == NO_CAM)
	{
		return;
	}

	pPRU = &PRS_SetCameraPRU(nCamera);

	PR_InitHookRFuncOnMouseClickCmd(&stCmd);

	stCmd.pFunc = PRS_MouseClick;
	stCmd.ulTimeout = 1000;
	
	PR_HookRFuncOnMouseClickCmd(&stCmd, PRS_MENU_SID, PRS_MENU_RID, &stCmdRpy);

	if (stCmdRpy.uwCommunStatus != PR_COMM_NOERR || stCmdRpy.uwPRStatus != PR_ERR_NOERR)
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stCmdRpy.uwCommunStatus);
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "PR Status: %x", stCmdRpy.uwPRStatus);
	} 
}

PR_VVOID PRS_InitHookRFuncOnMouseDrag()
{
	PR_HOOK_RFUNC_ON_MOUSE_DRAG_CMD		stCmd;
	PR_COMMON_RPY						stCmdRpy;

	PRU *pPRU;

	if (nCamera == NO_CAM)
	{
		return;
	}

	pPRU = &PRS_SetCameraPRU(nCamera);

	PR_InitHookRFuncOnMouseDragCmd(&stCmd);

	stCmd.pFunc = PRS_MouseDrag;
	stCmd.ulTimeout = 1000;
	
	PR_HookRFuncOnMouseDragCmd(&stCmd, PRS_MENU_SID, PRS_MENU_RID, &stCmdRpy);

	if (stCmdRpy.uwCommunStatus != PR_COMM_NOERR || stCmdRpy.uwPRStatus != PR_ERR_NOERR)
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stCmdRpy.uwCommunStatus);
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW2, "PR Status: %x", stCmdRpy.uwPRStatus);
	} 
}

PR_VVOID PRS_SetOnMouseClickCmd(BOOL bMode)
{
	PR_SET_ON_MOUSE_CLICK_CMD	stCmd;
	PR_SET_ON_MOUSE_CLICK_RPY	stCmdRpy;

	PR_InitSetOnMouseClickCmd(&stCmd);

	stCmd.emEnable = (PR_BOOLEAN)bMode;
	PR_SetOnMouseClickCmd(&stCmd, PRS_MENU_SID, PRS_MENU_RID, &stCmdRpy);
}


// Video Functions
//
VOID PRS_DisplayVideo(PRU *pPRU)
{
	BOOL bUpdate = FALSE;		// update lighting in HMI?
	BOOL bUpdateUplookConfig = FALSE;

	PR_DISPLAY_VIDEO_CMD	stCmd;
	PR_DISPLAY_VIDEO_RPY	stRpy;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PR_InitDisplayVideoCmd(&stCmd);
	stCmd.emEnableVideo = PR_TRUE;
	stCmd.emDisplayCursor = PR_TRUE;

	if (pPRU->emCameraNo != nCurrentCameraID)
	{
#if 1 //testing remove me 20111230
		bUpdate = TRUE;	
		//	nCurrentCameraID = pPRU->emCameraNo;
		nCurrentCameraID	= pPRU->nCamera;
		nCamera	= pPRU->nCamera;
#endif
	}

	PRS_ClearScreen(pPRU->nCamera);

	switch ((LONG)pPRU->emCameraNo)
	{

	case PR_CAMERA_1: //INWH_CAM
		stCmd.emCameraNo	= PR_CAMERA_1;
		stCmd.emPurpose		= PR_PURPOSE_CAM_A0;
		g_lHMIIndectViewHeight					= 480; //20150917
		g_lHMIIndectViewWidth					= 512;
		break;

	case PR_CAMERA_2: //ACF1 Cam
		stCmd.emCameraNo	= PR_CAMERA_2;
		stCmd.emPurpose		= PR_PURPOSE_CAM_A1;
		g_lHMIIndectViewHeight					= 480; //20150917
		g_lHMIIndectViewWidth					= 512;
		break;

	case PR_CAMERA_3: //ACF2 Cam
		stCmd.emCameraNo	= PR_CAMERA_3;
		stCmd.emPurpose		= PR_PURPOSE_CAM_A2;
		g_lHMIIndectViewHeight					= 480; //20150917
		g_lHMIIndectViewWidth					= 512;
		break;
	
	case PR_CAMERA_4: //PostACF Insp Downlook
		stCmd.emCameraNo	= PR_CAMERA_4;
		stCmd.emPurpose		= PR_PURPOSE_CAM_A3;
		g_lHMIIndectViewHeight					= 480; //20150917
		g_lHMIIndectViewWidth					= 512;
		break;

	case PR_CAMERA_5: //PB1 UL
		stCmd.emCameraNo	= PR_CAMERA_5;	
		stCmd.emPurpose		= PR_PURPOSE_CAM_A4;
		g_lHMIIndectViewHeight					= 480; //20150917
		g_lHMIIndectViewWidth					= 512;
		break;
		
	case PR_CAMERA_6: //PB1 DnLook
		stCmd.emCameraNo	= PR_CAMERA_6;	
		stCmd.emPurpose		= PR_PURPOSE_CAM_A5;
		g_lHMIIndectViewHeight					= 480; //20150917
		g_lHMIIndectViewWidth					= 512;
		break;
	
	case PR_CAMERA_7: //PB2 UpLook 1
		stCmd.emCameraNo	= PR_CAMERA_7;
		stCmd.emPurpose		= PR_PURPOSE_CAM_A6;
		g_lHMIIndectViewHeight					= 480; //20150917
		g_lHMIIndectViewWidth					= 512;
		break;

	case PR_CAMERA_8: //PB2 Dnlook
		stCmd.emCameraNo	= PR_CAMERA_8;
		stCmd.emPurpose		= PR_PURPOSE_CAM_A7;
		g_lHMIIndectViewHeight					= 480; //20150917
		g_lHMIIndectViewWidth					= 512;
		break;

	case PR_CAMERA_9: //SH1
		stCmd.emCameraNo	= PR_CAMERA_9;
		stCmd.emPurpose		= PR_PURPOSE_CAM_A8;
		g_lHMIIndectViewHeight					= 480; //20150917
		g_lHMIIndectViewWidth					= 512;
		break;

	case PR_CAMERA_10: //SH2
		stCmd.emCameraNo	= PR_CAMERA_10;
		stCmd.emPurpose		= PR_PURPOSE_CAM_A9;
		g_lHMIIndectViewHeight					= 480; //20150917
		g_lHMIIndectViewWidth					= 512;
		break;

	case PR_CAMERA_11: //FPC
		stCmd.emCameraNo	= PR_CAMERA_11;
		stCmd.emPurpose		= PR_PURPOSE_CAM_A10;
		g_lHMIIndectViewHeight					= 480; //20150917
		g_lHMIIndectViewWidth					= 512;
		break;

	case PR_CAMERA_12: //TA4
		stCmd.emCameraNo	= PR_CAMERA_12;
		stCmd.emPurpose		= PR_PURPOSE_CAM_A11;
		g_lHMIIndectViewHeight					= 480; //20150917
		g_lHMIIndectViewWidth					= 512;
		break;

	case PR_CAMERA_13: //MidFlip
		stCmd.emCameraNo	= PR_CAMERA_13;
		stCmd.emPurpose		= PR_PURPOSE_CAM_A12;
		g_lHMIIndectViewHeight					= 480; //20150917
		g_lHMIIndectViewWidth					= 512;
		break;

	default:
		stCmd.emEnableVideo		= PR_FALSE;
		stCmd.emDisplayCursor	= PR_FALSE;
		g_lHMIIndectViewHeight					= 480; //20150917
		g_lHMIIndectViewWidth					= 512;
		break;
	}

	PR_DisplayVideoCmd(&stCmd, ubSenderID, ubReceiverID, &stRpy);

	PRS_CHAR_SIZE emCharSize = CHAR_SIZE_MEDIUM;
	//switch(pPRU->stZoomSensorCmd.emZoomMode) //20151204
	//{
	//	case PR_ZOOM_SENSOR_MODE_1X_ZOOM_NORMAL:
	//		emCharSize = CHAR_SIZE_SMALL;
	//		break;
	//	default:
	//		emCharSize = CHAR_SIZE_MEDIUM;
	//		break;
	//}
	switch ((LONG)pPRU->emCameraNo)
	{

	case PR_CAMERA_1: //INWH_CAM
		PRS_MenuDisplayTextExt(2, 0, emCharSize, "Cam: ACFDL");
		break;

	case PR_CAMERA_2: //ACF1
		PRS_MenuDisplayTextExt(2, 0, emCharSize, "Cam: ACFUL");
		break;

	case PR_CAMERA_3: //ACF2 cam
		//PRS_MenuDisplayTextExt(2, 0, emCharSize, "Cam: ACF2");
		break;
	
	case PR_CAMERA_4: //PostACF
		//PRS_MenuDisplayTextExt(2, 0, emCharSize, "Cam: PostACF");
		break;

	case PR_CAMERA_5: //PB1 Uplook
		//PRS_MenuDisplayTextExt(2, 0, emCharSize, "Cam: PB1 UpLook");
		break;
		
	case PR_CAMERA_6: //PB1 DnLook
		//PRS_MenuDisplayTextExt(2, 0, emCharSize, "Cam: PB1 DnLook");
		break;
	
	case PR_CAMERA_7: //PB2 UpLook
		//PRS_MenuDisplayTextExt(2, 0, emCharSize, "Cam: PB2 UpLook");
		break;

	case PR_CAMERA_8: //PB2 DnLook
		//PRS_MenuDisplayTextExt(2, 0, emCharSize, "Cam: PB1 DnLook");
		break;

	case PR_CAMERA_9: //SH1
		//PRS_MenuDisplayTextExt(2, 0, emCharSize, "Cam: SH1");
		break;

	case PR_CAMERA_10: //SH2
		//PRS_MenuDisplayTextExt(2, 0, emCharSize, "Cam: SH2");
		break;

	case PR_CAMERA_11: //FPCPickArm
		//PRS_MenuDisplayTextExt(2, 0, emCharSize, "Cam: FPCPickArm DL");
		break;

	case PR_CAMERA_12: //TA4
		//PRS_MenuDisplayTextExt(2, 0, emCharSize, "Cam: TA4");
		break;

	case PR_CAMERA_13: //MidFlip
		//PRS_MenuDisplayTextExt(2, 0, emCharSize, "Cam: Post Flipper");
		break;

	default:
		PRS_MenuDisplayTextExt(2, 0, emCharSize, "Cam: NONE");
		break;
	}

	if (bUpdate)
	{
		bPRUpdateLight = TRUE;
	}
}


VOID PRS_DisplayBinImg(INT const iCam, BOOL bMode, ULONG Threshold)
{
	BOOL bUpdate = FALSE;		// update lighting in HMI?

	PR_SELECT_VIDEO_SOURCE_CMD	stCmd;
	//PR_SELECT_VIDEO_SOURCE_RPY	stRpy;

	PR_DISP_BIN_IMG_CMD		stBinCmd;
	PR_DISP_BIN_IMG_EXT_RPY	stBinRpy;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PRU *pPRU;

	pPRU = &PRS_SetCameraPRU(iCam);

	if (iCam != nCamera)
	{
		bUpdate = TRUE;
	}


	switch (iCam)
	{
	case NO_CAM:
		//PRS_MenuDisplayText(2, 0, "Cam: NONE   ");
		//stCmd.emEnableVideo		= PR_FALSE;
		//stCmd.emDisplayCursor	= PR_FALSE;
		return;

	case ACF_CAM:
		stBinCmd.emCameraNo		= PR_CAMERA_1;
		stBinCmd.emPurpose		= PR_PURPOSE_CAM_A0;
		stCmd.emCameraNo		= PR_CAMERA_1;
		stCmd.emPurpose			= PR_PURPOSE_CAM_A0;
		break;

	case ACFUL_CAM:
		stBinCmd.emCameraNo		= PR_CAMERA_2;
		stBinCmd.emPurpose		= PR_PURPOSE_CAM_A1;
		stCmd.emCameraNo		= PR_CAMERA_2;
		stCmd.emPurpose			= PR_PURPOSE_CAM_A1;
		break;

	default:
		stBinCmd.emCameraNo		= PR_CAMERA_1;
		stBinCmd.emPurpose		= PR_PURPOSE_CAM_A0;
		stCmd.emCameraNo		= PR_CAMERA_1;
		stCmd.emPurpose			= PR_PURPOSE_CAM_A0;
		break;
	}

	PR_InitSelectVideoSourceCmd(&stCmd);		//init CmdVideo

	stBinCmd.ubThresholdValue	= (PR_UBYTE)Threshold;				//set threshold value = 100
	stBinCmd.emGrab				= (PR_BOOLEAN) 1 ;

	if (bMode)
	{
		PR_DispBinImgExtCmd(&stBinCmd, ubSenderID, ubReceiverID, &stBinRpy); // turn on threshold display
	}
	else
	{
		PRS_DisplayVideo(pPRU); // recover the live video 
		PRS_DrawHomeCursor(pPRU->nCamera, FALSE);
	}
}

BOOL PRS_CopyUserPurpose(PR_PURPOSE emUserPurpose, PR_PURPOSE emCamPurpose)	// For New PR Auto-Bond Mode Display
{
	PR_COMMON_RPY	stRpy;
	PR_CopyPurposeFromRef(emUserPurpose, emCamPurpose, PRS_MENU_SID, PRS_MENU_RID, &stRpy);
	if (stRpy.uwPRStatus == PR_ERR_NOERR && stRpy.uwCommunStatus == PR_COMM_NOERR)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

VOID PRS_AutoBondMode(BOOL bMode, BOOL bToggle)
{
	PR_SCREEN		pstScreen;
	PR_COMMON_RPY	stRpy;

	PR_PURPOSE		tmpBack/*, tmpFore1, tmpFore2, tmpFore3, tmpFore4, tmpFore5, tmpFore6, tmpFore7*/;
	PR_InitPRScreen(&pstScreen);

	PR_ResetAutoBondCmd(PRS_MENU_SID, PRS_MENU_RID, &stRpy);
	
	if (bMode)
	{
		if (bToggle)
		{
			nAutoModeCam++;

			if (nAutoModeCam > MAX_CAM)
			{
				nAutoModeCam = 1;
			}
		}

		switch (nAutoModeCam)
		{
		case ACF_CAM:
			tmpBack		= PR_PURPOSE_CAM_A0;
			break;

		case ACFUL_CAM:
			tmpBack		= PR_PURPOSE_CAM_A1;
			break;

		default:
			tmpBack		= PR_PURPOSE_CAM_A0;
		}

		// set background
		pstScreen.aemPurpose[0] = tmpBack;
		pstScreen.wNumOfPurpose = 1;
		PR_SetAutoBondBackgroundCmd(PR_AUTO_BOND_SCREEN_MODE_16, &pstScreen, PRS_MENU_SID, PRS_MENU_RID, &stRpy);

		//// set foreground1
		//pstScreen.aemPurpose[0] = tmpFore1;
		//pstScreen.wNumOfPurpose = 1;
		//PR_SetAutoBondForegroundCmd(PR_AUTO_BOND_SCREEN_MODE_16, 10, &pstScreen, PRS_MENU_SID, PRS_MENU_RID, &stRpy);

		//// set foreground2
		//pstScreen.aemPurpose[0] = tmpFore2;
		//pstScreen.wNumOfPurpose = 1;
		//PR_SetAutoBondForegroundCmd(PR_AUTO_BOND_SCREEN_MODE_16, 11, &pstScreen, PRS_MENU_SID, PRS_MENU_RID, &stRpy);

		//// set foreground3
		//pstScreen.aemPurpose[0] = tmpFore3;
		//pstScreen.wNumOfPurpose = 1;
		//PR_SetAutoBondForegroundCmd(PR_AUTO_BOND_SCREEN_MODE_16, 12, &pstScreen, PRS_MENU_SID, PRS_MENU_RID, &stRpy);

		//// set foreground4
		//pstScreen.aemPurpose[0] = tmpFore4;
		//pstScreen.wNumOfPurpose = 1;
		//PR_SetAutoBondForegroundCmd(PR_AUTO_BOND_SCREEN_MODE_16, 13, &pstScreen, PRS_MENU_SID, PRS_MENU_RID, &stRpy);

		//// set foreground5
		//pstScreen.aemPurpose[0] = tmpFore5;
		//pstScreen.wNumOfPurpose = 1;
		//PR_SetAutoBondForegroundCmd(PR_AUTO_BOND_SCREEN_MODE_16, 14, &pstScreen, PRS_MENU_SID, PRS_MENU_RID, &stRpy);

		//// set foreground6
		//pstScreen.aemPurpose[0] = tmpFore6;
		//pstScreen.wNumOfPurpose = 1;
		//PR_SetAutoBondForegroundCmd(PR_AUTO_BOND_SCREEN_MODE_16, 15, &pstScreen, PRS_MENU_SID, PRS_MENU_RID, &stRpy);
		//
		//// set foreground7
		//pstScreen.aemPurpose[0] = tmpFore7;
		//pstScreen.wNumOfPurpose = 1;
		//PR_SetAutoBondForegroundCmd(PR_AUTO_BOND_SCREEN_MODE_16, 16, &pstScreen, PRS_MENU_SID, PRS_MENU_RID, &stRpy);


		PR_SetAutoBondModeCmd(PR_AUTO_BOND_SCREEN_MODE_16, PRS_MENU_SID, PRS_MENU_RID, &stRpy);
	}
	else
	{
		PR_SetAutoBondModeCmd(PR_AUTO_BOND_SCREEN_MODE_DISABLE, PRS_MENU_SID, PRS_MENU_RID, &stRpy);
		//PRS_DisplayVideo(pPRU);
	}
}

VOID PRS_DisableDigitalZoom(INT const iCam)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;
	PRU *pPRU;
	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PR_COMMON_RPY	stRpy;

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);
#if 1 //20120830
	pPRU = &PRS_SetCameraPRU_FOR_ZOOM(iCam);
#else
	pPRU = &PRS_SetCameraPRU(iCam);
#endif

	PR_SetDigitalZoomModeCmd(PR_FALSE, ubSenderID, ubReceiverID, &stRpy);

	if (PR_ERROR_STATUS(stRpy.uwPRStatus) || stRpy.uwCommunStatus != PR_COMM_NOERR)
	{
		PRS_DisplayText(iCam, 1, PRS_MSG_ROW2, "Disable Zoom Error");
		return;
	}

//	PRS_SetOnMouseClickCmd(OFF);	// 20140819 Yip: Do Not Turn Off Mouse Click When Disable Digital Zoom

	PRS_DisplayVideo(pPRU);
}

VOID PRS_DigitalZoom(INT const iCam, PR_ZOOM_FACTOR const emZoomFactor, PR_COORD const coZoomRegionCentre)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;
	CString szMsg = _T("");

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PR_GRAB_DISP_IMG_CMD		stGrabDispImgCmd;
	PR_GRAB_DISP_IMG_RPY		stGrabDispImgRpy;
	PR_COMMON_RPY				stRpy;

	PRU *pPRU;

	PR_InitGrabDispImgCmd(&stGrabDispImgCmd);

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);
#if 1 //20120830
	pPRU = &PRS_SetCameraPRU_FOR_ZOOM(iCam);
#else
	pPRU = &PRS_SetCameraPRU(iCam);
#endif
	stGrabDispImgCmd.emPurpose = pPRU->emPurpose[PURPOSE0];

	PRS_ClearScreen(iCam);

	//Display the image (Grab Image)
	PR_GrabDispImgCmd(&stGrabDispImgCmd, ubCurSenderID, ubCurReceiverID, &stGrabDispImgRpy.uwCommunStatus);
	PR_GrabDispImgRpy(ubCurSenderID, &stGrabDispImgRpy);

	if (PR_ERROR_STATUS(stGrabDispImgRpy.uwPRStatus) || stGrabDispImgRpy.uwCommunStatus != PR_COMM_NOERR)
	{
		PRS_DisplayText(iCam, 1, PRS_MSG_ROW2, "Display Image Error: %d, %d", stGrabDispImgRpy.uwCommunStatus, stGrabDispImgRpy.uwPRStatus);
		return;
	}

	PR_SetDigitalZoomModeCmd(PR_TRUE, ubCurSenderID, ubCurReceiverID, &stRpy);

	if (PR_ERROR_STATUS(stRpy.uwPRStatus) || stRpy.uwCommunStatus != PR_COMM_NOERR)
	{
		PRS_DisplayText(iCam, 1, PRS_MSG_ROW2, "Zoom Error 1");
		return;
	}
	else
	{
		if (emZoomFactor > PR_ZOOM_FACTOR_1X)
		{
			PR_SetDigitalZoomInfoCmd(coZoomRegionCentre, emZoomFactor, ubCurSenderID, ubCurReceiverID, &stRpy);
		
			if (PR_ERROR_STATUS(stRpy.uwPRStatus) || stRpy.uwCommunStatus != PR_COMM_NOERR)
			{
				PRS_DisplayText(iCam, 1, PRS_MSG_ROW2, "Zoom Error 2");
				return;
			}
		}
		else
		{
			PRS_DisableDigitalZoom(iCam);
		}
	}
}

VOID PRS_GetPointFromZoomedImg(PR_ZOOM_FACTOR const emZoomCurrFactor, PR_COORD const coCurrCentreIn1xImg, PR_COORD const coExpectPtInCurrZoomImg, PR_COORD *pcoExpectPtIn1xImg)
{
	PR_GetPointIn1xImgFromZoomedImg(emZoomCurrFactor, coCurrCentreIn1xImg, coExpectPtInCurrZoomImg, pcoExpectPtIn1xImg);
}

VOID PRS_GetPointInZoomImgFrom1xImg(PR_ZOOM_FACTOR const emExpectZoomFactor, PR_COORD const coExpectZoomCentre, PR_COORD const coExpectZoomPoint, PR_COORD *pcoResultantPointInZoomImg, PR_BOOLEAN *pemIsWithinRange)
{
	PR_GetPointInZoomImgFrom1xImg(emExpectZoomFactor, coExpectZoomCentre, coExpectZoomPoint, pcoResultantPointInZoomImg, pemIsWithinRange);
}

// 20141030 Yip: Add Function For Setting Display Magnify Factor
VOID PRS_SetDisplayMagnifyFactor(INT const iCam, PR_ZOOM_MODE const emZoomMode, PR_ZOOM_FACTOR const emZoomFactor, PR_COORD const coZoomRegionCenter)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);

	PR_SET_DISPLAY_MAGNIFY_FACTOR_CMD	stSetDisplayMagnifyFactorCmd;
	PR_SET_DISPLAY_MAGNIFY_FACTOR_RPY	stSetDisplayMagnifyFactorRpy;

	PR_InitSetDisplayMagnifyFactorCmd(&stSetDisplayMagnifyFactorCmd);

	stSetDisplayMagnifyFactorCmd.emZoomMode			= emZoomMode;
	stSetDisplayMagnifyFactorCmd.emZoomFactor		= emZoomFactor;
	stSetDisplayMagnifyFactorCmd.coZoomRegionCenter	= coZoomRegionCenter;

	PR_SetDisplayMagnifyFactorCmd(&stSetDisplayMagnifyFactorCmd, ubCurSenderID, ubCurReceiverID, &stSetDisplayMagnifyFactorRpy);

	if (PR_ERROR_STATUS(stSetDisplayMagnifyFactorRpy.stStatus.uwPRStatus) || stSetDisplayMagnifyFactorRpy.stStatus.uwCommunStatus != PR_COMM_NOERR)
	{
		PRS_DisplayText(iCam, 1, PRS_MSG_ROW2, "Set Display Magnify Factor Error");
	}
}

// 20141030 Yip: Add Function For Setting Display Magnify Factor
VOID PRS_SetDisplayMagnifyFactor(INT const iCam, PR_ZOOM_MODE const emZoomMode, PR_ZOOM_FACTOR const emZoomFactor)
{
	PR_COORD coZoomRegionCenter = {PR_DEF_CENTRE_X, PR_DEF_CENTRE_Y};
	PRS_SetDisplayMagnifyFactor(iCam, emZoomMode, emZoomFactor, coZoomRegionCenter);
}

// Optics Functions
//
VOID PRS_GetOptic(PRU *pPRU)
{
	PR_COMMON_RPY	stRpy;

	PR_GetOptic(pPRU->emPurpose[PURPOSE0], pPRU->ubSenderID, pPRU->ubReceiverID, &pPRU->stOptic, &stRpy);
}

VOID PRS_GetCamLighting(PRU *pPRU)
{
	PR_COMMON_RPY	stRpy;

	PR_UWORD		uwSourceNo;
	PR_SOURCE		emSource;
	PR_UWORD		uwLevel;

	//PRS_GetOptic(pPRU); //20150429
	PR_GetSourceNo(&pPRU->stOptic, pPRU->ubSenderID, pPRU->ubReceiverID, &uwSourceNo, &stRpy);

	for (INT i = 0; i < uwSourceNo; i++)
	{
		PR_GetLightSource(&pPRU->stOptic, i, pPRU->ubSenderID, pPRU->ubReceiverID, &emSource, &stRpy);
		PR_GetLighting(&pPRU->stOptic, emSource, pPRU->ubSenderID, pPRU->ubReceiverID, &uwLevel, &stRpy);
		switch (emSource)
		{
		case PR_COAXIAL_LIGHT:
			pPRU->uwCoaxialLevel 	= uwLevel;
			break;
		case PR_COAXIAL_LIGHT_1:
			pPRU->uwCoaxial1Level 	= uwLevel;
			break;
		case PR_COAXIAL_LIGHT_2:
			pPRU->uwCoaxial2Level 	= uwLevel;
			break;
		case PR_RING_LIGHT:
			pPRU->uwRingLevel 		= uwLevel;
			break;
		case PR_SIDE_LIGHT:
			pPRU->uwSideLevel 		= uwLevel;
			break;
		case PR_SIDE_LIGHT_1:
			pPRU->uwSide1Level 		= uwLevel;
			break;
		case PR_BACK_LIGHT:
			pPRU->uwBackLevel 		= uwLevel;
			break;
		case PR_BACK_LIGHT_1:
			pPRU->uwBack1Level 		= uwLevel;
			break;
			
		}
	}
}

PR_UWORD PRS_GetLighting(PRU *pPRU, PR_SOURCE const emSource)
{
	PR_COMMON_RPY	stRpy;
	PR_UWORD		uwLevel;

	//PRS_GetOptic(pPRU); //20150429
	PR_GetLighting(&pPRU->stOptic, emSource, pPRU->ubSenderID, pPRU->ubReceiverID, &uwLevel, &stRpy);

	return uwLevel;
}

VOID PRS_SetLighting(PRU *pPRU, PR_SOURCE const emSource, PR_UWORD const uwLevel, BOOL bIsSleep)
{
	PR_COMMON_RPY	stRpy;

	//PRS_GetOptic(pPRU); //20150429
	PR_SetLighting(emSource, uwLevel, pPRU->ubSenderID, pPRU->ubReceiverID, &pPRU->stOptic, &stRpy);
	bPRUpdateLight = TRUE;
	if (bIsSleep) //20120710
	{
		Sleep(100);
	}
}

INT	PRS_GetExposureTime(PRU *pPRU)
{
	PR_COMMON_RPY		stRpy;
	PR_EXPOSURE_TIME	iExposureTime;

	PRS_GetOptic(pPRU); //20150429
	PR_GetExposureTime(&pPRU->stOptic, pPRU->ubSenderID, pPRU->ubReceiverID, &iExposureTime, &stRpy);

	return iExposureTime;
}

BOOL PRS_SetExposureTime(PRU *pPRU, INT iExposureTime)
{
	PR_COMMON_RPY		stRpy = {0, };

	PRS_GetOptic(pPRU);
	PR_SetExposureTime((PR_EXPOSURE_TIME)iExposureTime, pPRU->ubSenderID, pPRU->ubReceiverID, &pPRU->stOptic, &stRpy);

	if (stRpy.uwPRStatus == PR_ERR_NOERR && stRpy.uwCommunStatus == PR_COMM_NOERR)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

VOID PRS_EnableHWTrigLighting(INT const iCam, BOOL bMode)
{
	PR_UBYTE	ubCurSenderID = PRS_MENU_SID;
	PR_UBYTE	ubCurReceiverID = PRS_MENU_RID;

	ubSenderID		= PRS_MENU_SID;
	ubReceiverID	= PRS_MENU_RID;

	PR_COMMON_RPY      stCommRpy;

	PRS_ClearTextRow(iCam, PRS_MSG_ROW1);
	PRS_SelectCameraChannel(iCam, ubCurSenderID, ubCurReceiverID);

	if (bMode)
	{
		PR_EnableHwTrigLighting(ubCurSenderID, ubCurReceiverID, &stCommRpy);

		if (stCommRpy.uwCommunStatus != PR_COMM_NOERR || stCommRpy.uwPRStatus != PR_ERR_NOERR)
		{
			PRS_DisplayText(iCam, 1, PRS_MSG_ROW1, "Comm Error: %x", stCommRpy.uwCommunStatus);
			PRS_DisplayText(iCam, 1, PRS_MSG_ROW2, "PR Status: %x", stCommRpy.uwPRStatus);
		} 
	}
	else
	{
		PR_DisableHwTrigLighting(ubCurSenderID, ubCurReceiverID, &stCommRpy);

		if (stCommRpy.uwCommunStatus != PR_COMM_NOERR || stCommRpy.uwPRStatus != PR_ERR_NOERR)
		{
			PRS_DisplayText(iCam, 1, PRS_MSG_ROW1, "Comm Error: %x", stCommRpy.uwCommunStatus);
			PRS_DisplayText(iCam, 1, PRS_MSG_ROW2, "PR Status: %x", stCommRpy.uwPRStatus);
		} 
	}
}


VOID PRS_SetChannelInUse(PRU *pPRU, BOOL bInUse)
{
	INT i = 0;

	switch (pPRU->ubSenderID)
	{
	case PRS_ACF_SID:
		i = 1;
		break;
	}

	bChannelInUse[i] = bInUse;
}

BOOL PRS_GetChannelInUse(PRU *pPRU)
{
	INT i = 0;

	switch (pPRU->ubSenderID)
	{
	case PRS_ACF_SID:
		i = 1;
		break;


	}

	return bChannelInUse[i];
}


// Learn with Vision UI
VOID PRS_InitLrnCmd(PRU *pPRU)
{
	PR_InitLrnCmd(&pPRU->stLrnCmd);

	pPRU->stLrnCmd.emLrnModule						= PR_LRN_MODULE_ALL;
	pPRU->stLrnCmd.emVisualTask						= PR_VISUAL_TASK_SRCH_DIE;
	pPRU->stLrnCmd.stDBLrnPar.aemPurpose[0]			= pPRU->emPurpose[PURPOSE0];

	PR_InitCreateCriteriaCmd(&pPRU->stCreateCritCmd);
	PR_InitModifyCriteriaCmd(&pPRU->stModifyCritCmd);
	PR_InitInspCmd(&pPRU->stSrchCmd);
	pPRU->stSrchCmd.emInspect = PR_FALSE;
}

BOOL PRS_ManualLrnCmd(PRU *pPRU)	// Vision UI
{
	PR_LRN_RPY1	stRpy1;
	PR_LRN_RPY2	stRpy2;
	if (pPRU->emRecordType == PRS_DIE_TYPE)
	{
		pPRU->stLrnCmd.emVisualTask = PR_VISUAL_TASK_SRCH_DIE;
	}
	else if (pPRU->emRecordType == PRS_SHAPE_TYPE)
	{
		pPRU->stLrnCmd.emVisualTask = PR_VISUAL_TASK_DETECT_SHAPE;
	}
	pPRU->stLrnCmd.stDBLrnPar.aemPurpose[0]			= pPRU->emPurpose[PURPOSE0];

	PR_LrnCmd(&pPRU->stLrnCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_LrnRpy(pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy2);
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy1.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy1.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		pPRU->stCreateCritCmd.uwRecordID = stRpy2.uwRecordID;
		pPRU->stCreateCritCmd.emVisualTask = pPRU->stLrnCmd.emVisualTask;

		pPRU->stSrchCmd.uwRecordID = stRpy2.uwRecordID;
		pPRU->stSrchCmd.emVisualTask = pPRU->stLrnCmd.emVisualTask;

		// save the 4 points for learn record
		pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].x	= stRpy2.arcoCorners[0].x;
		pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].y	= stRpy2.arcoCorners[0].y;
		pPRU->arcoLrnDieCorners[PR_UPPER_LEFT].x		= stRpy2.arcoCorners[1].x;
		pPRU->arcoLrnDieCorners[PR_UPPER_LEFT].y		= stRpy2.arcoCorners[1].y;
		pPRU->arcoLrnDieCorners[PR_LOWER_LEFT].x		= stRpy2.arcoCorners[2].x;
		pPRU->arcoLrnDieCorners[PR_LOWER_LEFT].y		= stRpy2.arcoCorners[2].y;
		pPRU->arcoLrnDieCorners[PR_LOWER_RIGHT].x	= stRpy2.arcoCorners[3].x;
		pPRU->arcoLrnDieCorners[PR_LOWER_RIGHT].y	= stRpy2.arcoCorners[3].y;

		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy2.stStatus.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy2.stStatus.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

BOOL PRS_CreateCriteriaCmd(PRU *pPRU)	// Vision UI
{
	PR_CREATE_CRITERIA_RPY	stRpy;

	PR_CreateCriteriaCmd(&pPRU->stCreateCritCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy);

	// reply  OK?
	if (stRpy.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy.stStatus.uwPRStatus))
	{

		pPRU->stModifyCritCmd.uwRecordID = pPRU->stCreateCritCmd.uwRecordID;
		pPRU->stModifyCritCmd.emVisualTask = pPRU->stCreateCritCmd.emVisualTask;
		return TRUE;
	}
	else
	{
		if (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy.stStatus.uwPRStatus);
		}
		return FALSE;
	}

}


BOOL PRS_ModifyCriteriaCmd(PRU *pPRU)	// Vision UI
{
	PR_MODIFY_CRITERIA_RPY	stRpy;

	//pPRU->stModifyCritCmd.uwRecordID = pPRU->stCreateCritCmd.uwRecordID;
	//pPRU->stModifyCritCmd.emVisualTask = pPRU->stLrnCmd.emVisualTask;

	PR_ModifyCriteriaCmd(&pPRU->stModifyCritCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy);

	// reply  OK?
	if (stRpy.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy.stStatus.uwPRStatus))
	{

		return TRUE;
	}
	else
	{
		if (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy.stStatus.uwPRStatus);
		}
		return FALSE;
	}

}

BOOL PRS_ManualSrchCmd(PRU *pPRU)	// Vision UI
{
	PR_INSP_RPY1			stRpy1;
	PR_INSP_RPY2			stRpy2;

	//pPRU->stSrchCmd.emVisualTask	= pPRU->stLrnCmd.emVisualTask;
	pPRU->stSrchCmd.emLatchMode		= PR_LATCH_FROM_CAMERA;
	pPRU->stSrchCmd.emInspect = PR_FALSE;

	PR_InspCmd(&pPRU->stSrchCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1.stStatus.uwCommunStatus);

	if (stRpy1.stStatus.uwCommunStatus == PR_COMM_NOERR)
	{
		PR_InspRpy1(pPRU->ubSenderID, &stRpy1);
	}
	else
	{
		PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy1.stStatus.uwCommunStatus);
		return FALSE;
	}

	if (stRpy1.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.stStatus.uwPRStatus))
	{
		if (stRpy1.emReturnRpy2)
		{
			PR_InspRpy2(pPRU->ubSenderID, &stRpy2);
		}
		else
		{
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Search Reply2 not required. Please check Srch Type and Lrn Type.");
			return FALSE;
		}
	}
	else
	{
		if (stRpy1.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy1.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy1.stStatus.uwPRStatus);
		}
		return FALSE;
	}

	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		pPRU->arcoDieCorners[PR_UPPER_RIGHT].x	= stRpy2.arcoCorners[PR_UPPER_RIGHT].x;
		pPRU->arcoDieCorners[PR_UPPER_RIGHT].y	= stRpy2.arcoCorners[PR_UPPER_RIGHT].y;
		pPRU->arcoDieCorners[PR_UPPER_LEFT].x	= stRpy2.arcoCorners[PR_UPPER_LEFT].x;
		pPRU->arcoDieCorners[PR_UPPER_LEFT].y	= stRpy2.arcoCorners[PR_UPPER_LEFT].y;
		pPRU->arcoDieCorners[PR_LOWER_LEFT].x	= stRpy2.arcoCorners[PR_LOWER_LEFT].x;
		pPRU->arcoDieCorners[PR_LOWER_LEFT].y	= stRpy2.arcoCorners[PR_LOWER_LEFT].y;
		pPRU->arcoDieCorners[PR_LOWER_RIGHT].x	= stRpy2.arcoCorners[PR_LOWER_RIGHT].x;
		pPRU->arcoDieCorners[PR_LOWER_RIGHT].y	= stRpy2.arcoCorners[PR_LOWER_RIGHT].y;

		pPRU->rcoDieCentre.x		= stRpy2.rcoCentre.x;
		pPRU->rcoDieCentre.y		= stRpy2.rcoCentre.y;

		pPRU->fAng					= stRpy2.rRotation;
		pPRU->fScore				= stRpy2.rScore;
		pPRU->rszScale.x			= stRpy2.rszScale.x;
		pPRU->rszScale.y			= stRpy2.rszScale.y;
		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

BOOL PRS_ManualSrchGrabOnly(PRU *pPRU)	// Vision UI
{
	PR_INSP_RPY1			stRpy1;
//	PR_INSP_RPY2			stRpy2;

	
	pPRU->stSrchCmd.emLatchMode		= PR_LATCH_FROM_CAMERA;
	pPRU->stSrchCmd.emInspect = PR_FALSE;

	PR_InspCmd(&pPRU->stSrchCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1.stStatus.uwCommunStatus);

	if (stRpy1.stStatus.uwCommunStatus == PR_COMM_NOERR)
	{
		PR_InspRpy1(pPRU->ubSenderID, &stRpy1);
	}
	else
	{
		PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy1.stStatus.uwCommunStatus);
		return FALSE;
	}

	if (stRpy1.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.stStatus.uwPRStatus))
	{
		return TRUE;
	}
	else
	{
		if (stRpy1.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy1.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy1.stStatus.uwPRStatus);
		}
		return FALSE;
	}

}

BOOL PRS_ManualSrchProcessOnly(PRU *pPRU)	// Vision UI
{
	PR_INSP_RPY2			stRpy2;

	PR_InspRpy2(pPRU->ubSenderID, &stRpy2);

	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		pPRU->arcoDieCorners[PR_UPPER_RIGHT].x	= stRpy2.arcoCorners[PR_UPPER_RIGHT].x;
		pPRU->arcoDieCorners[PR_UPPER_RIGHT].y	= stRpy2.arcoCorners[PR_UPPER_RIGHT].y;
		pPRU->arcoDieCorners[PR_UPPER_LEFT].x	= stRpy2.arcoCorners[PR_UPPER_LEFT].x;
		pPRU->arcoDieCorners[PR_UPPER_LEFT].y	= stRpy2.arcoCorners[PR_UPPER_LEFT].y;
		pPRU->arcoDieCorners[PR_LOWER_LEFT].x	= stRpy2.arcoCorners[PR_LOWER_LEFT].x;
		pPRU->arcoDieCorners[PR_LOWER_LEFT].y	= stRpy2.arcoCorners[PR_LOWER_LEFT].y;
		pPRU->arcoDieCorners[PR_LOWER_RIGHT].x	= stRpy2.arcoCorners[PR_LOWER_RIGHT].x;
		pPRU->arcoDieCorners[PR_LOWER_RIGHT].y	= stRpy2.arcoCorners[PR_LOWER_RIGHT].y;

		pPRU->rcoDieCentre.x		= stRpy2.rcoCentre.x;
		pPRU->rcoDieCentre.y		= stRpy2.rcoCentre.y;

		pPRU->fAng					= stRpy2.rRotation;
		pPRU->fScore				= stRpy2.rScore;
		pPRU->rszScale.x			= stRpy2.rszScale.x;
		pPRU->rszScale.y			= stRpy2.rszScale.y;
		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}


// Die Alignment Functions
//
VOID PRS_InitManualLrnDieCmd(PRU *pPRU)
{
	PR_InitManualLrnDieCmd(&pPRU->stManualLrnDieCmd);

	pPRU->stManualLrnDieCmd.emCameraNo						= pPRU->emCameraNo;
	pPRU->stManualLrnDieCmd.emBackground					= PR_NO_BACKGROUND;
	pPRU->stManualLrnDieCmd.emBackgroundInfo			= PR_BACKGROUND_INFO_DARK;

	pPRU->stManualLrnDieCmd.emObjectType				= PR_OBJ_TYPE_FIDUCIAL_MARK;
	pPRU->stManualLrnDieCmd.uwNumOfDieCorners				= PR_NO_OF_CORNERS;
	pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_RIGHT].x	= PR_DEF_CENTRE_X;
	pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_RIGHT].y	= PR_DEF_CENTRE_Y;
	pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].x	= PR_DEF_CENTRE_X;
	pPRU->stManualLrnDieCmd.acoDieCorners[PR_UPPER_LEFT].y	= PR_DEF_CENTRE_Y;
	pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_LEFT].x	= PR_DEF_CENTRE_X;
	pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_LEFT].y	= PR_DEF_CENTRE_Y;
	pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].x	= PR_DEF_CENTRE_X;
	pPRU->stManualLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT].y	= PR_DEF_CENTRE_Y;
	pPRU->stManualLrnDieCmd.rExpectedAngle					= PR_DEF_ANGLE;
	pPRU->stManualLrnDieCmd.rAngleRange						= PR_DEF_DIE_ANGLE_RANGE;
	pPRU->stManualLrnDieCmd.rAngleStep						= PR_DEF_DIE_ANGLE_STEP;
	//pPRU->stManualLrnDieCmd.stDiePad
	//pPRU->stManualLrnDieCmd.coRefPoint
	//pPRU->stManualLrnDieCmd.rRefAngle
	pPRU->stManualLrnDieCmd.stDefectWin.uwNDetectWin		= 0;	// not in use for now
	pPRU->stManualLrnDieCmd.stDefectWin.uwNIgnoreWin		= 0;	// not in use for now
	//pPRU->stManualLrnDieCmd.emPreRot
	pPRU->stManualLrnDieCmd.emLrnDie						= PR_DEF_LRN_DIE;

	pPRU->stManualLrnDieCmd.emAlignAlg					= PR_EDGE_POINTS_MATCHING_ON_DIE_PATTERN;
	

	pPRU->stManualLrnDieCmd.emInspAlg					= PR_GOLDEN_DIE_TMPL; //include postbond
	pPRU->stManualLrnDieCmd.emCoarseSrchResolution		= PR_RESOLUTION_MEDIUM;
	pPRU->stManualLrnDieCmd.emFineSrchResolution		= PR_RESOLUTION_HIGH;

	pPRU->stManualLrnDieCmd.emInspResolution			= PR_RESOLUTION_DEFAULT;

	pPRU->stManualLrnDieCmd.emIsLrnBackupAlign				= PR_FALSE;
	pPRU->stManualLrnDieCmd.emIsGrabFromPurpose				= PR_TRUE;
	pPRU->stManualLrnDieCmd.aemPurpose[0]					= pPRU->emPurpose[PURPOSE0];
	pPRU->stManualLrnDieCmd.emIsAngUniqueCheck				= PR_FALSE;
	pPRU->stManualLrnDieCmd.emIsRectObj						= PR_TRUE;

	// Fiducial fitting
	pPRU->stManualLrnDieCmd.emIsAutoLearnFMarkCenterAngle	= PR_FALSE;
	pPRU->stManualLrnDieCmd.emFiducialMarkType				= PR_FIDUCIAL_MARK_TYPE_CROSS;
	pPRU->stManualLrnDieCmd.acoFMarkInputCorners[0].x		= PR_DEF_CENTRE_X;
	pPRU->stManualLrnDieCmd.acoFMarkInputCorners[0].y		= PR_DEF_CENTRE_Y;
	pPRU->stManualLrnDieCmd.acoFMarkInputCorners[1].x		= PR_DEF_CENTRE_X;
	pPRU->stManualLrnDieCmd.acoFMarkInputCorners[1].y		= PR_DEF_CENTRE_Y;

	// Learn Confirmation Pattern
	pPRU->stManualLrnDieCmd.emIsLrnCfmPattern				= PR_FALSE;
	pPRU->stManualLrnDieCmd.emIsUseDefaultShapeWin			= PR_FALSE;
	pPRU->stManualLrnDieCmd.stCfmPatternWindow.coCorner1.x	= 0;
	pPRU->stManualLrnDieCmd.stCfmPatternWindow.coCorner1.y	= 0;
	pPRU->stManualLrnDieCmd.stCfmPatternWindow.coCorner2.x	= 0;
	pPRU->stManualLrnDieCmd.stCfmPatternWindow.coCorner2.y	= 0;
	
	pPRU->stManualLrnDieCmd.stAlign.stStreet.ubThreshold 	= 100;

	// 20140815 Yip: Add Manual Learn Die Ignore Region
	pPRU->stManualLrnDieCmd.stAlign.stStreet.stDetectIgnoreWin.uwNIgnoreWin	= 0;
}

VOID PRS_InitSrchDieCmd(PRU *pPRU)
{
	PR_InitSrchDieCmd(&pPRU->stSrchDieCmd);

	pPRU->stSrchDieCmd.emLatch								= PR_TRUE;
	pPRU->stSrchDieCmd.emRetainBuffer						= PR_FALSE;
	pPRU->stSrchDieCmd.emAlign								= PR_TRUE;

	pPRU->stSrchDieCmd.emDefectInsp						= PR_FALSE;

	pPRU->stSrchDieCmd.emPostBondInsp						= PR_FALSE;
	pPRU->stSrchDieCmd.emCameraNo							= pPRU->emCameraNo;
	//pPRU->stSrchDieCmd.emVideoSource
	pPRU->stSrchDieCmd.uwNRecordID							= 1;
	pPRU->stSrchDieCmd.auwRecordID[0]						= 0;
	pPRU->stSrchDieCmd.coProbableDieCentre.x				= PR_DEF_CENTRE_X;
	pPRU->stSrchDieCmd.coProbableDieCentre.y				= PR_DEF_CENTRE_Y;
	//pPRU->stSrchDieCmd.ulRpyControlCode
	pPRU->stSrchDieCmd.emAlignAlg							= pPRU->stManualLrnDieCmd.emAlignAlg;

	// For new PR 3_13ACF
	pPRU->stSrchDieCmd.stDieAlign.stPaLn.emIsCoarseSrch		= PR_TRUE; // PR_TRUE means the original version
	pPRU->stSrchDieCmd.stDieAlign.stPaLn.stLargeSrchWin.coCorner1.x = PR_DEF_WIN_ULC_X;
	pPRU->stSrchDieCmd.stDieAlign.stPaLn.stLargeSrchWin.coCorner1.y = PR_DEF_WIN_ULC_Y;
	pPRU->stSrchDieCmd.stDieAlign.stPaLn.stLargeSrchWin.coCorner2.x = PR_DEF_WIN_LRC_X;
	pPRU->stSrchDieCmd.stDieAlign.stPaLn.stLargeSrchWin.coCorner2.y = PR_DEF_WIN_LRC_Y;

	pPRU->stSrchDieCmd.stDieAlign.stStreet.emDieRotChk		= PR_FALSE;
	pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.x	= PR_DEF_WIN_ULC_X;
	pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.y	= PR_DEF_WIN_ULC_Y;
	pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.x	= PR_DEF_WIN_LRC_X;
	pPRU->stSrchDieCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.y	= PR_DEF_WIN_LRC_Y;
	//pPRU->stSrchDieCmd.stDieAlignPar.rAngleRange
	pPRU->stSrchDieCmd.stDieAlignPar.rMatchScore			= PR_DEF_DIE_MATCH_SCORE;
	pPRU->stSrchDieCmd.stDieAlignPar.emEnableBackupAlign	= PR_FALSE;
	pPRU->stSrchDieCmd.stDieAlignPar.emIsDefaultMatchScore	= PR_FALSE;
	
	pPRU->stSrchDieCmd.stDieAlignPar.rStartAngle			= -PR_DEF_DIE_ANGLE_RANGE;
	pPRU->stSrchDieCmd.stDieAlignPar.rEndAngle				= PR_DEF_DIE_ANGLE_RANGE;

	pPRU->stSrchDieCmd.stDieAlignPar.emIsUseDefaultScale	= PR_FALSE;
	pPRU->stSrchDieCmd.stDieAlignPar.ubMaxScale				= 110;
	pPRU->stSrchDieCmd.stDieAlignPar.ubMinScale				= 90;

	pPRU->stSrchDieCmd.emGraphicInfo						= PR_DISPLAY_CORNERS;
}

BOOL PRS_ManualLrnDie(PRU *pPRU)
{
	PR_MANUAL_LRN_DIE_RPY1	stRpy1;
	PR_MANUAL_LRN_DIE_RPY2	stRpy2;

	// modification on align cmd
	switch (pPRU->stManualLrnDieCmd.emAlignAlg)
	{
	case PRS_ALG_EDGE:
		break;

	case PRS_ALG_INTR:
		break;
	}

	//PRS_DisplayVideo(pPRU);

	//PRS_GetOptic(pPRU);
	PR_ManualLrnDieCmd(&pPRU->stManualLrnDieCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_ManualLrnDieRpy(pPRU->ubSenderID, &stRpy2);
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy1.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy1.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		pPRU->stSrchDieCmd.auwRecordID[0] = stRpy2.uwRecordID;
		pPRU->stChangeRecordCmd.uwRecordID = stRpy2.uwRecordID;	//20150109 FOR RGB Pattern Center
		pPRU->stSrchDieCmd.uwNRecordID = 1;

		// save the 4 points for learn record
		pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].x	= stRpy2.acoDieCorners[0].x;
		pPRU->arcoLrnDieCorners[PR_UPPER_RIGHT].y	= stRpy2.acoDieCorners[0].y;
		pPRU->arcoLrnDieCorners[PR_UPPER_LEFT].x		= stRpy2.acoDieCorners[1].x;
		pPRU->arcoLrnDieCorners[PR_UPPER_LEFT].y		= stRpy2.acoDieCorners[1].y;
		pPRU->arcoLrnDieCorners[PR_LOWER_LEFT].x		= stRpy2.acoDieCorners[2].x;
		pPRU->arcoLrnDieCorners[PR_LOWER_LEFT].y		= stRpy2.acoDieCorners[2].y;
		pPRU->arcoLrnDieCorners[PR_LOWER_RIGHT].x	= stRpy2.acoDieCorners[3].x;
		pPRU->arcoLrnDieCorners[PR_LOWER_RIGHT].y	= stRpy2.acoDieCorners[3].y;

		pPRU->stSrchDieCmd.coProbableDieCentre.x	= stRpy2.coDieCentre.x;
		pPRU->stSrchDieCmd.coProbableDieCentre.y	= stRpy2.coDieCentre.y;

		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy2.stStatus.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy2.stStatus.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

BOOL PRS_SrchDie(PRU *pPRU)
{
	PR_SRCH_DIE_RPY1	stRpy1;
	PR_SRCH_DIE_RPY2	stRpy2;
	PR_SRCH_DIE_RPY3	stRpy3;
	PR_SRCH_DIE_RPY4	stRpy4;

	PR_UWORD	uwComStatus = PR_COMM_NOERR;

	//PRS_GetOptic(pPRU);
	PR_SrchDieCmd(&pPRU->stSrchDieCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &uwComStatus);

	if (uwComStatus == PR_COMM_NOERR)
	{
		PR_SrchDieRpy1(pPRU->ubSenderID, &stRpy1);
	}
	else
	{
		PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, uwComStatus);
		//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", uwComStatus);
		return FALSE;
	}

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_SrchDieRpy2(pPRU->ubSenderID, &stRpy2);

	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy1.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy1.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		pPRU->arcoDieCorners[PR_UPPER_RIGHT].x	= stRpy2.stDieAlign[0].stStreet.arcoDieCorners[PR_UPPER_RIGHT].x;
		pPRU->arcoDieCorners[PR_UPPER_RIGHT].y	= stRpy2.stDieAlign[0].stStreet.arcoDieCorners[PR_UPPER_RIGHT].y;
		pPRU->arcoDieCorners[PR_UPPER_LEFT].x	= stRpy2.stDieAlign[0].stStreet.arcoDieCorners[PR_UPPER_LEFT].x;
		pPRU->arcoDieCorners[PR_UPPER_LEFT].y	= stRpy2.stDieAlign[0].stStreet.arcoDieCorners[PR_UPPER_LEFT].y;
		pPRU->arcoDieCorners[PR_LOWER_LEFT].x	= stRpy2.stDieAlign[0].stStreet.arcoDieCorners[PR_LOWER_LEFT].x;
		pPRU->arcoDieCorners[PR_LOWER_LEFT].y	= stRpy2.stDieAlign[0].stStreet.arcoDieCorners[PR_LOWER_LEFT].y;
		pPRU->arcoDieCorners[PR_LOWER_RIGHT].x	= stRpy2.stDieAlign[0].stStreet.arcoDieCorners[PR_LOWER_RIGHT].x;
		pPRU->arcoDieCorners[PR_LOWER_RIGHT].y	= stRpy2.stDieAlign[0].stStreet.arcoDieCorners[PR_LOWER_RIGHT].y;

		pPRU->rcoDieCentre.x		= stRpy2.stDieAlign[0].stStreet.rcoDieCentre.x;
		pPRU->rcoDieCentre.y		= stRpy2.stDieAlign[0].stStreet.rcoDieCentre.y;
		pPRU->rcoDieCentre.x	= stRpy2.stDieAlign[0].stStreet.rcoDieCentre.x; //20150728 float PR
		pPRU->rcoDieCentre.y	= stRpy2.stDieAlign[0].stStreet.rcoDieCentre.y;
		pPRU->bUseFloatingPointResult = TRUE;
		pPRU->fAng				= stRpy2.stDieAlign[0].stGen.rDieRot;
		pPRU->fScore			= stRpy2.stDieAlign[0].stStreet.rMatchScore;
		pPRU->fScale			= stRpy2.stDieAlign[0].stStreet.rScale;	// 20151201

		//For RGB Pattern
		if(pPRU->stManualLrnDieCmd.emObjectType == PR_OBJ_TYPE_DISPLAY_PANEL)
		{
			if (
				pPRU->stSrchDieCmd.stDieAlign.stStreet.coSelectPoint.x == PR_DEF_WIN_ULC_X &&
				pPRU->stSrchDieCmd.stDieAlign.stStreet.coSelectPoint.y == PR_DEF_WIN_ULC_Y
				)     //Upper Left
			{
				//pPRU->rcoDieCentre.x           = pPRU->arcoDieCorners[PR_UPPER_LEFT].x;
				//pPRU->rcoDieCentre.y           = pPRU->arcoDieCorners[PR_UPPER_LEFT].y;
			}
			else if (
				pPRU->stSrchDieCmd.stDieAlign.stStreet.coSelectPoint.x == PR_DEF_WIN_LRC_X &&
				pPRU->stSrchDieCmd.stDieAlign.stStreet.coSelectPoint.y == PR_DEF_WIN_ULC_Y
				)     //Upper Right
			{
				//pPRU->rcoDieCentre.x           = pPRU->arcoDieCorners[PR_UPPER_RIGHT].x;
				//pPRU->rcoDieCentre.y           = pPRU->arcoDieCorners[PR_UPPER_RIGHT].y;
			}
		}

		if (!pPRU->stSrchDieCmd.emDefectInsp)
		{
			PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
			return TRUE;
		}
		PR_SrchDieRpy3(pPRU->ubSenderID, &stRpy3);
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy2.stStatus.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy2.stStatus.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}

	// reply 3 OK?
	if (stRpy3.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy3.stStatus.uwPRStatus))
	{
		pPRU->dTotalDefectArea		= stRpy3.stDieInspExt[0].stTmplExt.aeChipArea;
#ifdef CHECK_COLLET_DIRT
		pPRU->ulDefectPRStatusResult = stRpy3.stStatus.uwPRStatus;	// PR_ERR_NOERR / PR_ERR_DEFECTIVE_DIE
		pPRU->ulLargestDefectAreaResult = stRpy3.stDieInsp[0].stTmpl.aeLargestDefectArea; // PR_AREA (PR_ULWORD), area represented by number of subpixels
		pPRU->ulTotalDefectAreaResult = stRpy3.stDieInsp[0].stTmpl.aeTotalDefectArea; // PR_AREA (PR_ULWORD), area represented by number of subpixels
#endif

		if (!pPRU->stSrchDieCmd.emPostBondInsp)
		{
			PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
			return TRUE;
		}

		PR_SrchDieRpy4(pPRU->ubSenderID, &stRpy4);
	}
	else
	{
		pPRU->dTotalDefectArea		= stRpy3.stDieInspExt[0].stTmplExt.aeChipArea;

		if (stRpy3.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy3.stStatus.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy3.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy3.stStatus.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy3.stStatus.uwPRStatus);
		}
		return FALSE;
	}

	// reply 4 OK?
	if (stRpy4.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy4.stStatus.uwPRStatus))
	{
		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
		return TRUE;
	}
	else
	{
		if (stRpy4.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy4.stStatus.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy4.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy4.stStatus.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy4.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

BOOL PRS_SrchDieGrabOnly(PRU *pPRU)
{
	CString str;
	PR_SRCH_DIE_RPY1	stRpy1;

	PR_UWORD	uwComStatus = PR_COMM_NOERR;

	//PRS_GetOptic(pPRU);
	PR_SrchDieCmd(&pPRU->stSrchDieCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &uwComStatus);

	if (uwComStatus == PR_COMM_NOERR)
	{
		PR_SrchDieRpy1(pPRU->ubSenderID, &stRpy1);
		
	}
	else
	{
#ifdef NO_INDIRECTVIEW_TXT
		str.Format("PRS Comm Error: %x", uwComStatus);
		theApp.WriteHMIMess(str, TRUE);
#else
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", uwComStatus);
#endif
		return FALSE;
	}

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PRS_SetChannelInUse(pPRU, TRUE);
		return TRUE;
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
#ifdef NO_INDIRECTVIEW_TXT
			str.Format("PRS Comm Error: %x", stRpy1.uwCommunStatus);
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
#endif
		}
		else
		{
#ifdef NO_INDIRECTVIEW_TXT
			PRS_GetPrintPRError(uwComStatus, str); //20151007
			str =  CString("Cam: ") + pPRU->szStnName + CString(". ") + str;
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
#endif
		}
		return FALSE;
	}
}

BOOL PRS_SrchDieProcessOnly(PRU *pPRU)
{
	CString str;
	PR_SRCH_DIE_RPY2	stRpy2;
	PR_SRCH_DIE_RPY3	stRpy3;
	PR_SRCH_DIE_RPY4	stRpy4;

	PR_UWORD	uwComStatus = PR_COMM_NOERR;

	//PRS_GetOptic(pPRU);
	PR_SrchDieRpy2(pPRU->ubSenderID, &stRpy2);

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		pPRU->arcoDieCorners[PR_UPPER_RIGHT].x	= stRpy2.stDieAlign[0].stStreet.arcoDieCorners[PR_UPPER_RIGHT].x;
		pPRU->arcoDieCorners[PR_UPPER_RIGHT].y	= stRpy2.stDieAlign[0].stStreet.arcoDieCorners[PR_UPPER_RIGHT].y;
		pPRU->arcoDieCorners[PR_UPPER_LEFT].x	= stRpy2.stDieAlign[0].stStreet.arcoDieCorners[PR_UPPER_LEFT].x;
		pPRU->arcoDieCorners[PR_UPPER_LEFT].y	= stRpy2.stDieAlign[0].stStreet.arcoDieCorners[PR_UPPER_LEFT].y;
		pPRU->arcoDieCorners[PR_LOWER_LEFT].x	= stRpy2.stDieAlign[0].stStreet.arcoDieCorners[PR_LOWER_LEFT].x;
		pPRU->arcoDieCorners[PR_LOWER_LEFT].y	= stRpy2.stDieAlign[0].stStreet.arcoDieCorners[PR_LOWER_LEFT].y;
		pPRU->arcoDieCorners[PR_LOWER_RIGHT].x	= stRpy2.stDieAlign[0].stStreet.arcoDieCorners[PR_LOWER_RIGHT].x;
		pPRU->arcoDieCorners[PR_LOWER_RIGHT].y	= stRpy2.stDieAlign[0].stStreet.arcoDieCorners[PR_LOWER_RIGHT].y;

		pPRU->rcoDieCentre.x		= stRpy2.stDieAlign[0].stStreet.rcoDieCentre.x;
		pPRU->rcoDieCentre.y		= stRpy2.stDieAlign[0].stStreet.rcoDieCentre.y;
		pPRU->rcoDieCentre.x	= stRpy2.stDieAlign[0].stStreet.rcoDieCentre.x; //20150728 float PR
		pPRU->rcoDieCentre.y	= stRpy2.stDieAlign[0].stStreet.rcoDieCentre.y;
		pPRU->bUseFloatingPointResult = TRUE;
		pPRU->fAng				= stRpy2.stDieAlign[0].stGen.rDieRot;
		pPRU->fScore			= stRpy2.stDieAlign[0].stStreet.rMatchScore;
		pPRU->fScale			= stRpy2.stDieAlign[0].stStreet.rScale;	// 20151201

		if (!pPRU->stSrchDieCmd.emDefectInsp)
		{
#ifndef NO_INDIRECTVIEW_TXT
			PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
#endif
			PRS_SetChannelInUse(pPRU, FALSE);
			return TRUE;
		}
		PR_SrchDieRpy3(pPRU->ubSenderID, &stRpy3);
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
#ifdef NO_INDIRECTVIEW_TXT
			str.Format("Cam: %s. PRS Comm Error: %x", pPRU->szStnName, stRpy2.stStatus.uwCommunStatus);
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
#endif
		}
		else
		{
#ifdef NO_INDIRECTVIEW_TXT
			PRS_GetPrintPRError(stRpy2.stStatus.uwPRStatus, str); //20151007
			str =  CString("Cam: ") + pPRU->szStnName + CString(". ") + str;
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
#endif
		}
		PRS_SetChannelInUse(pPRU, FALSE);
		return FALSE;
	}

	// reply 3 OK?
	if (stRpy3.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy3.stStatus.uwPRStatus))
	{
		pPRU->dTotalDefectArea		= stRpy3.stDieInsp[0].stTmpl.aeTotalDefectArea;

		if (!pPRU->stSrchDieCmd.emPostBondInsp)
		{
#ifndef NO_INDIRECTVIEW_TXT
			PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
#endif
			PRS_SetChannelInUse(pPRU, FALSE);
			return TRUE;
		}
		PR_SrchDieRpy4(pPRU->ubSenderID, &stRpy4);
	}
	else
	{
		pPRU->dTotalDefectArea		= stRpy3.stDieInsp[0].stTmpl.aeTotalDefectArea;

		if (stRpy3.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
#ifdef NO_INDIRECTVIEW_TXT
			str.Format("Cam: %s, PRS Comm Error: %x", pPRU->szStnName, stRpy2.stStatus.uwCommunStatus);
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy3.stStatus.uwCommunStatus);
#endif
		}
		else
		{
#ifdef NO_INDIRECTVIEW_TXT
			PRS_GetPrintPRError(stRpy3.stStatus.uwPRStatus, str); //20151007
			str =  CString("Cam: ") + pPRU->szStnName + CString(". ") + str;
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy3.stStatus.uwPRStatus);
#endif
		}
		PRS_SetChannelInUse(pPRU, FALSE);
		return FALSE;
	}

	// reply 4 OK?
	if (stRpy4.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy4.stStatus.uwPRStatus))
	{
#ifndef NO_INDIRECTVIEW_TXT
		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
#endif
		PRS_SetChannelInUse(pPRU, FALSE);
		return TRUE;
	}
	else
	{
		if (stRpy4.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
#ifdef NO_INDIRECTVIEW_TXT
			str.Format("Cam: %s, PRS Comm Error: %x", pPRU->szStnName, stRpy4.stStatus.uwCommunStatus);
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy4.stStatus.uwCommunStatus);
#endif
		}
		else
		{
#ifdef NO_INDIRECTVIEW_TXT
			PRS_GetPrintPRError(stRpy3.stStatus.uwPRStatus, str); //20151007
			str =  CString("Cam: ") + pPRU->szStnName + CString(". ") + str;
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy4.stStatus.uwPRStatus);
#endif
		}
		PRS_SetChannelInUse(pPRU, FALSE);
		return FALSE;
	}
}
//20150108 RGB Pattern Modify Chenter
VOID PRS_InitChangeRecordCmd(PRU *pPRU)
{
	PR_InitChangeRecordCmd(&pPRU->stChangeRecordCmd);
	pPRU->stChangeRecordCmd.uwRecordID = 0;

}
VOID PRS_InitRGBCenterCmd(PRU *pPRU)
{
	PR_InitLrnShapeCmd(&pPRU->stLrnRGBCenterCmd);

	pPRU->stLrnRGBCenterCmd.emPurpose				= pPRU->emPurpose[PURPOSE0];
	pPRU->stLrnRGBCenterCmd.emShapeType				= PR_SHAPE_TYPE_CORNER;
	pPRU->stLrnRGBCenterCmd.emDetectAlg				= PR_SHAPE_DETECT_ALG_SHA_SYNTHETIC_LRN;
	pPRU->stLrnRGBCenterCmd.emAccuracy				= PR_ACCURACY_HIGH;
	pPRU->stLrnRGBCenterCmd.emObjAttribute			= PR_OBJ_ATTRIBUTE_BRIGHT;
	pPRU->stLrnRGBCenterCmd.coRefPoint.x				= PR_DEF_CENTRE_X;
	pPRU->stLrnRGBCenterCmd.coRefPoint.y				= PR_DEF_CENTRE_Y;
}

BOOL	PRS_LrnRGBCenter(PRU *pPRU)
{
	PR_LRN_SHAPE_RPY1	stRpy1;
	PR_LRN_SHAPE_RPY2	stRpy2;
	
	PR_CHANGE_RECORD_RPY    stRpy3;
                                                                                                                                                    
	if (
		pPRU->stSrchDieCmd.stDieAlign.stStreet.coSelectPoint.x == PR_DEF_WIN_ULC_X &&
		pPRU->stSrchDieCmd.stDieAlign.stStreet.coSelectPoint.y == PR_DEF_WIN_ULC_Y
		)     //Upper Left
	{
		pPRU->stLrnRGBCenterCmd.coRefPoint.x          = pPRU->stLrnRGBCenterCmd.stEncRect.coCorner1.x;
		pPRU->stLrnRGBCenterCmd.coRefPoint.y          = pPRU->stLrnRGBCenterCmd.stEncRect.coCorner1.y;
	}
	else if (
		pPRU->stSrchDieCmd.stDieAlign.stStreet.coSelectPoint.x == PR_DEF_WIN_LRC_X &&
		pPRU->stSrchDieCmd.stDieAlign.stStreet.coSelectPoint.y == PR_DEF_WIN_ULC_Y
		)     //Upper Right
	{
		pPRU->stLrnRGBCenterCmd.coRefPoint.x          = pPRU->stLrnRGBCenterCmd.stEncRect.coCorner2.x;
		pPRU->stLrnRGBCenterCmd.coRefPoint.y           = pPRU->stLrnRGBCenterCmd.stEncRect.coCorner1.y;
	}
	pPRU->stLrnRGBCenterCmd.wEdgeMag = 15; //20150124
	PR_LrnShapeCmd(&pPRU->stLrnRGBCenterCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);
		// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_LrnShapeRpy(pPRU->ubSenderID, &stRpy2);
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy1.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy1.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}
		// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		if (PR_SHAPE_FOUND_CORNER == stRpy2.emShapeFound)
		{
			pPRU->stChangeRecordCmd.coRefPoint.x = pPRU->stSrchDieCmd.coProbableDieCentre.x;
			pPRU->stChangeRecordCmd.coRefPoint.y = pPRU->stSrchDieCmd.coProbableDieCentre.y;
			pPRU->stChangeRecordCmd.coNewPoint.x = (PR_WORD)stRpy2.stShapeRes.rcoCentre.x;
			pPRU->stChangeRecordCmd.coNewPoint.y = (PR_WORD)stRpy2.stShapeRes.rcoCentre.y;
			pPRU->stChangeRecordCmd.rRefAngle = 0;
			pPRU->stChangeRecordCmd.rNewAngle = stRpy2.stShapeRes.rAngle;
			PR_ChangeAlignReturnPointCmd(&pPRU->stChangeRecordCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy3);

			if (stRpy3.stStatus.uwCommunStatus == PR_COMM_NOERR)
			{
				return TRUE;
			}
			else
			{
				PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy3.stStatus.uwCommunStatus);
				return FALSE;
			}

		}
		else
		{
			//PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy2.emShapeFound);
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Error: Unknown Shape %d", stRpy2.emShapeFound);
			return FALSE;
		}
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy2.stStatus.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy2.stStatus.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}


}
// Shape Detection Functions
//
VOID PRS_InitLrnShapeCmd(PRU *pPRU)
{
	PR_InitLrnShapeCmd(&pPRU->stLrnShapeCmd);

	pPRU->stLrnShapeCmd.emPurpose				= pPRU->emPurpose[PURPOSE0];
	//pPRU->stLrnShapeCmd.coRefPoint
	//pPRU->stLrnShapeCmd.rRefAngle
	//pPRU->stLrnShapeCmd.emIsAligned
	pPRU->stLrnShapeCmd.emShapeType				= PR_SHAPE_TYPE_CIRCLE;
	pPRU->stLrnShapeCmd.stEncRect.coCorner1.x	= PR_DEF_WIN_ULC_X;
	pPRU->stLrnShapeCmd.stEncRect.coCorner1.y	= PR_DEF_WIN_ULC_Y;
	pPRU->stLrnShapeCmd.stEncRect.coCorner2.x	= PR_DEF_WIN_LRC_X;
	pPRU->stLrnShapeCmd.stEncRect.coCorner2.y	= PR_DEF_WIN_LRC_Y;
	pPRU->stLrnShapeCmd.emDetectAlg				= PR_SHAPE_DETECT_ALG_EDGE_PTS;
	pPRU->stLrnShapeCmd.emAccuracy				= PR_ACCURACY_HIGH;
	pPRU->stLrnShapeCmd.emObjAttribute			= PR_OBJ_ATTRIBUTE_BRIGHT;
	//pPRU->stLrnShapeCmd.ubThreshold
	pPRU->stLrnShapeCmd.wEdgeMag				= 15/*PR_DEF_SHAPE_EDGE_MAG*/;
	pPRU->stLrnShapeCmd.uwRoughness				= PR_DEF_SHAPE_DETECT_ROUGHNESS;
	//pPRU->stLrnShapeCmd.emIsSaveRecord
	//pPRU->stLrnShapeCmd.uwMaskID
	//pPRU->stLrnShapeCmd.rPassScore
	//pPRU->stLrnShapeCmd.uwNumOfLine
	//pPRU->stLrnShapeCmd.astLineCoord
	pPRU->stLrnShapeCmd.emCentreDefinition		= PR_CENTRE_DEFINITION_BY_CORNER;
	//pPRU->stLrnShapeCmd.uwNConfirmWin
	//pPRU->stLrnShapeCmd.astConfirmWin
	pPRU->stLrnShapeCmd.emExtMethod				= PR_EXT_LINE;
	pPRU->stLrnShapeCmd.uwNIgnoreWin			= 0;
	//pPRU->stLrnShapeCmd.astIgnoreWin
}

VOID PRS_InitDetectShapeCmd(PRU *pPRU)
{
	PR_InitDetectShapeCmd(&pPRU->stDetectShapeCmd);

	pPRU->stDetectShapeCmd.emSameView				= PR_FALSE;
	pPRU->stDetectShapeCmd.uwRecordID				= 0;
	//pPRU->stDetectShapeCmd.coRefPoint
	//pPRU->stDetectShapeCmd.rRefAngle
	pPRU->stDetectShapeCmd.stInspWin.coCorner1.x	= PR_DEF_WIN_ULC_X;
	pPRU->stDetectShapeCmd.stInspWin.coCorner1.y	= PR_DEF_WIN_ULC_Y;
	pPRU->stDetectShapeCmd.stInspWin.coCorner2.x	= PR_DEF_WIN_LRC_X;
	pPRU->stDetectShapeCmd.stInspWin.coCorner2.y	= PR_DEF_WIN_LRC_Y;
	pPRU->stDetectShapeCmd.emIsComplete				= PR_TRUE;
	pPRU->stDetectShapeCmd.rDetectScore				= PR_DEF_SHAPE_DETECT_SCORE;
	pPRU->stDetectShapeCmd.emSelectMode				= PR_SHAPE_SELECT_MODE_RADIUS;
	//pPRU->stDetectShapeCmd.coSelectPt
	pPRU->stDetectShapeCmd.rSelectRadius			= 0;
	pPRU->stDetectShapeCmd.coExpectCentre.x			= PR_DEF_CENTRE_X;
	pPRU->stDetectShapeCmd.coExpectCentre.y			= PR_DEF_CENTRE_Y;
	//pPRU->stDetectShapeCmd.coPositionVar
	pPRU->stDetectShapeCmd.rSizePercentVar			= PR_DEF_SHAPE_SIZE_VAR;
	pPRU->stDetectShapeCmd.emGraphicInfo			= PR_DISPLAY_CENTRE;
	pPRU->stDetectShapeCmd.emPriority				= PR_DEF_PRIORITY;
}

BOOL PRS_LrnShape(PRU *pPRU)
{
	PR_LRN_SHAPE_RPY1	stRpy1;
	PR_LRN_SHAPE_RPY2	stRpy2;

	//PRS_GetOptic(pPRU);
	pPRU->stLrnShapeCmd.wEdgeMag = 15; //20150124
	PR_LrnShapeCmd(&pPRU->stLrnShapeCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_LrnShapeRpy(pPRU->ubSenderID, &stRpy2);
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy1.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy1.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		if (PR_SHAPE_FOUND_CIRCLE == stRpy2.emShapeFound)
		{
			pPRU->stDetectShapeCmd.uwRecordID = stRpy2.uwRecordID;

			if (stRpy2.stCircleRes.rScore >= 50.0) // Double Check the Learn Score Request by Tony 20100902
			{										
			// Recommended to changed to 50.0 by Wong Chi Shing - HK Vision
				return TRUE;
			}
			else 
			{
				PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Learn Fail by low score");
				return FALSE;
			}
		}
		else
		{
			//PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy2.emShapeFound);
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Error: Unknown Shape %d", stRpy2.emShapeFound);
			return FALSE;
		}
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy2.stStatus.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy2.stStatus.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

BOOL PRS_DetectShape(PRU *pPRU)
{
	PR_DETECT_SHAPE_RPY1	stRpy1;
	PR_DETECT_SHAPE_RPY2	stRpy2;

	PR_UWORD	uwComStatus = PR_COMM_NOERR;

	//PRS_GetOptic(pPRU);
	PR_DetectShapeCmd(&pPRU->stDetectShapeCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_DetectShapeRpy(pPRU->ubSenderID, &stRpy2);
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy1.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy1.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		if (PR_SHAPE_FOUND_CIRCLE == stRpy2.emShapeFound)
		{
			pPRU->rcoDieCentre.x	= stRpy2.stCircleRes.coCentre.x;
			pPRU->rcoDieCentre.y	= stRpy2.stCircleRes.coCentre.y;
			pPRU->bUseFloatingPointResult = FALSE; //20150728 float PR

			pPRU->fAng			= 0;
			pPRU->fScore		= (FLOAT)stRpy2.stCircleRes.rScore;

			PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
			return TRUE;
		}
		else
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Error: Unknown Shape %d", stRpy2.emShapeFound);
			return FALSE;
		}
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy2.stStatus.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy2.stStatus.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

BOOL PRS_DetectShapeGrabOnly(PRU *pPRU)
{
	CString str;
	PR_DETECT_SHAPE_RPY1	stRpy1;

	PR_UWORD	uwComStatus = PR_COMM_NOERR;

	//PRS_GetOptic(pPRU);
	PR_DetectShapeCmd(&pPRU->stDetectShapeCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PRS_SetChannelInUse(pPRU, TRUE);
		return TRUE;
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
#ifdef NO_INDIRECTVIEW_TXT
			str.Format("PRS Comm Error: %x", stRpy1.uwCommunStatus);
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
#endif
		}
		else
		{
#ifdef NO_INDIRECTVIEW_TXT
			str.Format("PRS Comm Error: %x", stRpy1.uwCommunStatus);
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
#endif
		}
		return FALSE;
	}
}

BOOL PRS_DetectShapeProcessOnly(PRU *pPRU)
{
	CString str;
	PR_DETECT_SHAPE_RPY2	stRpy2;

	PR_UWORD	uwComStatus = PR_COMM_NOERR;

	//PRS_GetOptic(pPRU);
	PR_DetectShapeRpy(pPRU->ubSenderID, &stRpy2);

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		if (PR_SHAPE_FOUND_CIRCLE == stRpy2.emShapeFound)
		{
			pPRU->rcoDieCentre.x	= stRpy2.stCircleRes.coCentre.x;
			pPRU->rcoDieCentre.y	= stRpy2.stCircleRes.coCentre.y;
			pPRU->bUseFloatingPointResult = FALSE; //20150728 float PR
			pPRU->fAng			= 0;
			pPRU->fScore		= (FLOAT)stRpy2.stCircleRes.rScore;

#ifndef NO_INDIRECTVIEW_TXT
			PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
#endif
			PRS_SetChannelInUse(pPRU, FALSE);
			return TRUE;
		}
		else
		{
#ifdef NO_INDIRECTVIEW_TXT
			str.Format("Cam: %s. Error: Unknow Shape %d", pPRU->szStnName, stRpy2.emShapeFound);
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Error: Unknown Shape %d", stRpy2.emShapeFound);
#endif
			PRS_SetChannelInUse(pPRU, FALSE);
			return FALSE;
		}
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
#ifdef NO_INDIRECTVIEW_TXT
			str.Format("Cam: %s. Comm Error: %x", pPRU->szStnName, stRpy2.stStatus.uwCommunStatus);
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
#endif
		}
		else
		{
#ifdef NO_INDIRECTVIEW_TXT
			PRS_GetPrintPRError(stRpy2.stStatus.uwPRStatus, str); //20151007
			str =  CString("Cam: ") + pPRU->szStnName + CString(". ") + str;
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
#endif
		}
		PRS_SetChannelInUse(pPRU, FALSE);
		return FALSE;
	}
}

// Template Matching Functions
//
VOID PRS_InitLoadTmplCmd(PRU *pPRU)
{
	PR_InitLoadTmplCmd(&pPRU->stLoadTmplCmd);

	//pPRU->stLoadTmplCmd.emCameraNo
	pPRU->stLoadTmplCmd.emTmplType			= PR_TMPL_TYPE_DIE;
	pPRU->stLoadTmplCmd.emTmplSize			= PR_TMPL_SIZE_3;
	pPRU->stLoadTmplCmd.coTmplCentre.x		= PR_DEF_CENTRE_X;
	pPRU->stLoadTmplCmd.coTmplCentre.y		= PR_DEF_CENTRE_Y;
	pPRU->stLoadTmplCmd.rAngleRange			= 5.0;
	pPRU->stLoadTmplCmd.rAngleStep			= 1.0;
	pPRU->stLoadTmplCmd.emAccuracyCode		= PR_ACCURACY_HIGH;
	//pPRU->stLoadTmplCmd.ubBinaryThres
	pPRU->stLoadTmplCmd.emIsTmplCodeUsed	= PR_FALSE;
	pPRU->stLoadTmplCmd.szTmplSize.x		= (PR_DEF_WIN_LRC_X - PR_DEF_WIN_ULC_X) / 2;
	pPRU->stLoadTmplCmd.szTmplSize.y		= (PR_DEF_WIN_LRC_Y - PR_DEF_WIN_ULC_Y) / 2;
	pPRU->stLoadTmplCmd.emPurpose			= pPRU->emPurpose[PURPOSE0];
	// pPRU->emIsReturnFeaturePt
	// pPRU->coFeaturePt
}

VOID PRS_InitSrchTmplCmd(PRU *pPRU)
{
	PR_InitSrchTmplCmd(&pPRU->stSrchTmplCmd);

	pPRU->stSrchTmplCmd.emSameView				= PR_FALSE;
	pPRU->stSrchTmplCmd.emCameraNo				= pPRU->emCameraNo;
	pPRU->stSrchTmplCmd.emRotFlag				= PR_TRUE;
	pPRU->stSrchTmplCmd.emSrchBestMatch			= PR_TRUE;
	pPRU->stSrchTmplCmd.emSpeed					= PR_DEF_SPEED;
	pPRU->stSrchTmplCmd.uwRecordID				= 0;
	pPRU->stSrchTmplCmd.ubPassScore				= PR_DEF_TMPL_PASS_SCORE;
	pPRU->stSrchTmplCmd.stSrchWin.coCorner1.x	= PR_DEF_WIN_ULC_X;
	pPRU->stSrchTmplCmd.stSrchWin.coCorner1.y	= PR_DEF_WIN_ULC_Y;
	pPRU->stSrchTmplCmd.stSrchWin.coCorner2.x	= PR_DEF_WIN_LRC_X;
	pPRU->stSrchTmplCmd.stSrchWin.coCorner2.y	= PR_DEF_WIN_LRC_Y;
	pPRU->stSrchTmplCmd.coSelectPoint.x			= PR_DEF_CENTRE_X;
	pPRU->stSrchTmplCmd.coSelectPoint.y			= PR_DEF_CENTRE_Y;
	pPRU->stSrchTmplCmd.emGraphicInfo			= PR_DISPLAY_CORNERS;
}

BOOL PRS_LoadTmpl(PRU *pPRU)
{
	PR_LOAD_TMPL_RPY1	stRpy1;
	PR_LOAD_TMPL_RPY2	stRpy2;

	//PRS_GetOptic(pPRU);
	PR_LoadTmplCmd(&pPRU->stLoadTmplCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_LoadTmplRpy(pPRU->ubSenderID, &stRpy2);
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		pPRU->stSrchTmplCmd.uwRecordID = stRpy2.uwRecordID;
		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

BOOL PRS_SrchTmpl(PRU *pPRU)
{
	PR_SRCH_TMPL_RPY1	stRpy1;
	PR_SRCH_TMPL_RPY2	stRpy2;

	PR_UWORD	uwComStatus = PR_COMM_NOERR;

	//PRS_GetOptic(pPRU);
	PR_SrchTmplCmd(&pPRU->stSrchTmplCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &uwComStatus);

	if (uwComStatus == PR_COMM_NOERR)
	{
		PR_SrchTmplRpy1(pPRU->ubSenderID, &stRpy1);
	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", uwComStatus);
		return FALSE;
	}

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_SrchTmplRpy2(pPRU->ubSenderID, &stRpy2);
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		pPRU->rcoDieCentre.x	= stRpy2.coTmplCentre.x;
		pPRU->rcoDieCentre.y	= stRpy2.coTmplCentre.y;
		pPRU->bUseFloatingPointResult = FALSE; //20150728 float PR
		pPRU->fAng			= (FLOAT)stRpy2.rTmplAngle;
		pPRU->fScore		= (FLOAT)stRpy2.ubMatchScore;

		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

BOOL PRS_SrchTmplGrabOnly(PRU *pPRU)
{
	PR_SRCH_TMPL_RPY1	stRpy1;

	PR_UWORD	uwComStatus = PR_COMM_NOERR;

	//PRS_GetOptic(pPRU);
	PR_SrchTmplCmd(&pPRU->stSrchTmplCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &uwComStatus);

	if (uwComStatus == PR_COMM_NOERR)
	{
		PR_SrchTmplRpy1(pPRU->ubSenderID, &stRpy1);
	}
	else
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", uwComStatus);
		return FALSE;
	}

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PRS_SetChannelInUse(pPRU, TRUE);
		return TRUE;
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}
}

BOOL PRS_SrchTmplProcessOnly(PRU *pPRU)
{
	CString str("");
	PR_SRCH_TMPL_RPY2	stRpy2;

	PR_UWORD	uwComStatus = PR_COMM_NOERR;

	//PRS_GetOptic(pPRU);
	PR_SrchTmplRpy2(pPRU->ubSenderID, &stRpy2);

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		pPRU->rcoDieCentre.x	= stRpy2.coTmplCentre.x;
		pPRU->rcoDieCentre.y	= stRpy2.coTmplCentre.y;
		pPRU->bUseFloatingPointResult = FALSE; //20150728 float PR
		pPRU->fAng			= (FLOAT)stRpy2.rTmplAngle;
		pPRU->fScore		= (FLOAT)stRpy2.ubMatchScore;

#ifndef NO_INDIRECTVIEW_TXT
		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
#endif
		PRS_SetChannelInUse(pPRU, FALSE);
		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
#ifdef NO_INDIRECTVIEW_TXT
			str.Format("Cam: %s. PRS Comm Error: %x", pPRU->szStnName, stRpy2.stStatus.uwCommunStatus);
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
#endif
		}
		else
		{
#ifdef NO_INDIRECTVIEW_TXT
			PRS_GetPrintPRError(stRpy2.stStatus.uwPRStatus, str); //20151007
			str =  CString("Cam: ") + pPRU->szStnName + CString(". ") + str;
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
#endif
		}
		PRS_SetChannelInUse(pPRU, FALSE);
		return FALSE;
	}
}

// Kerf Fitting Functions
//
VOID PRS_InitLrnKerfCmd(PRU *pPRU)	// 20141023 Yip: Add Kerf Fitting Functions
{
	PR_InitLrnKerfCmd(&pPRU->stLrnKerfCmd);


	//printf( "Purpose : " );
	pPRU->stLrnKerfCmd.emPurpose				= pPRU->emPurpose[PURPOSE0];
	//printf( "Record ID (0 for none) : " );
	pPRU->stLrnKerfCmd.uwRecordID				= 0;
	//printf( "GUI Mode : " );
	pPRU->stLrnKerfCmd.wGUIMode					= 0;	
	//printf( "Kerf Search Mode? (0 = Line Search, 1 = Segmentation)"); //input = 0
	pPRU->stLrnKerfCmd.emKerfSrchMode			= PR_KERF_LINE_SRCH;
	//printf( "Kerf Input Mode? (0 = Horizontal, 1 = Vertical)");
	pPRU->stLrnKerfCmd.emKerfInputMode			= PR_KERF_INPUT_HORIZONTAL;
	//printf( "Obj Type? "); //input PR_OBJ_IC_DIE (3)
	pPRU->stLrnKerfCmd.emObjType				= PR_OBJ_TYPE_IC_DIE;
	//printf("Select PR_OBJ_ATTRIBUTE (1=bright, 2=dark): "); //input 1 or 2
	pPRU->stLrnKerfCmd.emKerfBrightness			= PR_OBJ_ATTRIBUTE_BRIGHT;
	//printf( "Draw Detection Region: " );
	pPRU->stLrnKerfCmd.stROI.coCorner1.x		= PR_DEF_WIN_ULC_X;
	pPRU->stLrnKerfCmd.stROI.coCorner1.y		= PR_DEF_WIN_ULC_Y;
	pPRU->stLrnKerfCmd.stROI.coCorner2.x		= PR_DEF_WIN_LRC_X;
	pPRU->stLrnKerfCmd.stROI.coCorner2.y		= PR_DEF_WIN_LRC_Y;
	//printf( "Edge Mag : " );		//input 50-100
	pPRU->stLrnKerfCmd.ubThreshold				= 50;
	//printf( "Lower Bound Search Angle : " ); //in deg
	pPRU->stLrnKerfCmd.rLowerBoundSearchAngle	= 3.0f;
	//printf( "Upper Bound Search Angle : " ); //in deg
	pPRU->stLrnKerfCmd.rUpperBoundSearchAngle	= 3.0f;
	//printf( "Fitting Passing Score : " ); //input 70-80
	pPRU->stLrnKerfCmd.uwFittingPassingScore	= 75;
	//printf( "Expect Width : " ); //in pixel
	pPRU->stLrnKerfCmd.rExpectWidth				= 10.0f;
	//printf( "Max Width Deviation : " ); //input 48
	pPRU->stLrnKerfCmd.rMaxWidthDev				= 48.0f;
}

VOID PRS_InitInspKerfCmd(PRU *pPRU)	// 20141023 Yip: Add Kerf Fitting Functions
{
	PR_InitInspKerfCmd(&pPRU->stInspKerfCmd);

	//printf( "Latch Mode (0 = same view; 1 = camera; 2 = gallery): " );
	pPRU->stInspKerfCmd.emLatchMode				= PR_LATCH_FROM_CAMERA;
	//printf( "Record ID : " );
	pPRU->stInspKerfCmd.uwRecordID				= 0;
	//printf( "Kerf Insp Mode (0 = Insp with Cmd Parameter, 1 = Insp with Rec Parameter) : " );  //Usually input 1
	pPRU->stInspKerfCmd.emKerfInspMode			= PR_KERF_INSP_W_REC_PAR;
}

BOOL PRS_LrnKerf(PRU *pPRU)	// 20141023 Yip: Add Kerf Fitting Functions
{
	PR_LRN_KERF_RPY1	stRpy1;
	PR_LRN_KERF_RPY2	stRpy2;

	//PRS_GetOptic(pPRU);
	PR_LrnKerfCmd(&pPRU->stLrnKerfCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);

	// reply 1 OK?
	if (stRpy1.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.stStatus.uwPRStatus))
	{
		PR_LrnKerfRpy(pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy2);
	}
	else
	{
		if (stRpy1.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy1.stStatus.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy1.stStatus.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.stStatus.uwPRStatus);
		}
		return FALSE;
	}

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		pPRU->stInspKerfCmd.uwRecordID = stRpy2.uwRecordID;
		if (stRpy2.rFittingScore >= pPRU->stLrnKerfCmd.uwFittingPassingScore) // Double Check the Learn Score Request
		{
			return TRUE;
		}
		else 
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Learn Fail by low score");
			return FALSE;
		}
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy2.stStatus.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy2.stStatus.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

BOOL PRS_InspKerf(PRU *pPRU)	// 20141023 Yip: Add Kerf Fitting Functions
{
	PR_INSP_KERF_RPY1	stRpy1;
	PR_INSP_KERF_RPY2	stRpy2;

	PR_UWORD	uwComStatus = PR_COMM_NOERR;

	//PRS_GetOptic(pPRU);
	PR_InspKerfCmd(&pPRU->stInspKerfCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);

	// reply 1 OK?
	if (stRpy1.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.stStatus.uwPRStatus))
	{
		PR_InspKerfRpy(pPRU->ubSenderID, &stRpy2);
	}
	else
	{
		if (stRpy1.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy1.stStatus.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy1.stStatus.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.stStatus.uwPRStatus);
		}
		return FALSE;
	}

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		//printf( "PR_InspKerfRpy is finished with status %hu, %hu!\n",
		//	stRpy2.stStatus.uwCommunStatus, stRpy2.stStatus.uwPRStatus );
		//printf( "Kerf center line = ( %f, %f ) -> ( %f, %f )\n",
		//	stRpy2.rcoCenterLineEndPt1.x, stRpy2.rcoCenterLineEndPt1.y,
		//	stRpy2.rcoCenterLineEndPt2.x, stRpy2.rcoCenterLineEndPt2.y );
		//printf( "Kerf center line offset = %f\n", stRpy2.rCenterLineOffset );
		//printf( "Kerf width excluding chip = %f\n", stRpy2.rKerfWidthExcludingChip );
		//printf( "Kerf width including chip = %f\n", stRpy2.rKerfWidthIncludingChip );

		pPRU->rcoDieCentre.x	= (PR_WORD)((stRpy2.rcoCenterLineEndPt1.x + stRpy2.rcoCenterLineEndPt2.x) / 2.0f);
		pPRU->rcoDieCentre.y	= (PR_WORD)((stRpy2.rcoCenterLineEndPt1.y + stRpy2.rcoCenterLineEndPt2.y) / 2.0f);

		pPRU->rcoDieCentre.x	= (PR_REAL)((stRpy2.rcoCenterLineEndPt1.x + stRpy2.rcoCenterLineEndPt2.x) / 2.0f); //20150728 float PR
		pPRU->rcoDieCentre.y	= (PR_REAL)((stRpy2.rcoCenterLineEndPt1.y + stRpy2.rcoCenterLineEndPt2.y) / 2.0f);
		pPRU->bUseFloatingPointResult = TRUE; //20150728 float PR

		pPRU->fAng			= (float)(atan2(stRpy2.rcoCenterLineEndPt2.y - stRpy2.rcoCenterLineEndPt1.y, stRpy2.rcoCenterLineEndPt2.x - stRpy2.rcoCenterLineEndPt1.x) / PI * 180.0);
		pPRU->fScore		= (float)stRpy2.rFittingScore;

		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy2.stStatus.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy2.stStatus.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

BOOL PRS_InspKerfGrabOnly(PRU *pPRU)	// 20141023 Yip: Add Kerf Fitting Functions
{
	PR_INSP_KERF_RPY1	stRpy1;

	PR_UWORD	uwComStatus = PR_COMM_NOERR;

	//PRS_GetOptic(pPRU);
	PR_InspKerfCmd(&pPRU->stInspKerfCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);

	// reply 1 OK?
	if (stRpy1.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.stStatus.uwPRStatus))
	{
		PRS_SetChannelInUse(pPRU, TRUE);
		return TRUE;
	}
	else
	{
		if (stRpy1.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

BOOL PRS_InspKerfProcessOnly(PRU *pPRU)	// 20141023 Yip: Add Kerf Fitting Functions
{
	CString str("");
	PR_INSP_KERF_RPY2	stRpy2;

	PR_UWORD	uwComStatus = PR_COMM_NOERR;

	//PRS_GetOptic(pPRU);
	PR_InspKerfRpy(pPRU->ubSenderID, &stRpy2);

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		//printf( "PR_InspKerfRpy is finished with status %hu, %hu!\n",
		//	stRpy2.stStatus.uwCommunStatus, stRpy2.stStatus.uwPRStatus );
		//printf( "Kerf center line = ( %f, %f ) -> ( %f, %f )\n",
		//	stRpy2.rcoCenterLineEndPt1.x, stRpy2.rcoCenterLineEndPt1.y,
		//	stRpy2.rcoCenterLineEndPt2.x, stRpy2.rcoCenterLineEndPt2.y );
		//printf( "Kerf center line offset = %f\n", stRpy2.rCenterLineOffset );
		//printf( "Kerf width excluding chip = %f\n", stRpy2.rKerfWidthExcludingChip );
		//printf( "Kerf width including chip = %f\n", stRpy2.rKerfWidthIncludingChip );

		pPRU->rcoDieCentre.x	= (PR_WORD)((stRpy2.rcoCenterLineEndPt1.x + stRpy2.rcoCenterLineEndPt2.x) / 2.0f);
		pPRU->rcoDieCentre.y	= (PR_WORD)((stRpy2.rcoCenterLineEndPt1.y + stRpy2.rcoCenterLineEndPt2.y) / 2.0f);

		pPRU->rcoDieCentre.x	= (PR_REAL)((stRpy2.rcoCenterLineEndPt1.x + stRpy2.rcoCenterLineEndPt2.x) / 2.0f); //20150728 float PR
		pPRU->rcoDieCentre.y	= (PR_REAL)((stRpy2.rcoCenterLineEndPt1.y + stRpy2.rcoCenterLineEndPt2.y) / 2.0f);
		pPRU->bUseFloatingPointResult = TRUE;

		pPRU->fAng			= (float)(atan2(stRpy2.rcoCenterLineEndPt2.y - stRpy2.rcoCenterLineEndPt1.y, stRpy2.rcoCenterLineEndPt2.x - stRpy2.rcoCenterLineEndPt1.x) / PI * 180.0);
		pPRU->fScore		= (float)stRpy2.rFittingScore;

#ifndef NO_INDIRECTVIEW_TXT
		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
#endif
		PRS_SetChannelInUse(pPRU, FALSE);
		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
#ifdef NO_INDIRECTVIEW_TXT
			str.Format("Cam: %s, PRS Comm Error: %x", pPRU->szStnName, stRpy2.stStatus.uwCommunStatus);
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
#endif
		}
		else
		{
#ifdef NO_INDIRECTVIEW_TXT
			PRS_GetPrintPRError(stRpy2.stStatus.uwPRStatus, str); //20151007
			str =  CString("Cam: ") + pPRU->szStnName + CString(". ") + str;
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
#endif
		}
		PRS_SetChannelInUse(pPRU, FALSE);
		return FALSE;
	}

}
// Exp Object Functions
//
VOID PRS_InitExpObjectCmd(PRU *pPRU)	//20150106 Add Exp Object Functions
{
	PR_InitExtractObjCmd(&pPRU->stExtObjectCmd);

	pPRU->stExtObjectCmd.stLatchPar.emPurpose 											= pPRU->emPurpose[PURPOSE0];
	pPRU->stExtObjectCmd.emShapeType											= PR_OBJSHAPE_TYPE_MULTILINE;   //line detection
	pPRU->stExtObjectCmd.stExtractObjPar.stMultiLineInput.aemLinePolarity[0]	= PR_OBJ_ATTRIBUTE_BRIGHT;
	pPRU->stExtObjectCmd.stExtractObjPar.stMultiLineInput.uwNumOfLine = 1;
	pPRU->stExtObjectCmd.stExtractObjPar.stMultiLineInput.astLine[0].coPoint1.x = (PR_MAX_COORD + 1) / 8;        //start point
	pPRU->stExtObjectCmd.stExtractObjPar.stMultiLineInput.astLine[0].coPoint1.y = PR_DEF_CENTRE_Y;        //start point
	pPRU->stExtObjectCmd.stExtractObjPar.stMultiLineInput.astLine[0].coPoint2.x = (PR_MAX_COORD + 1) / 8 * 7;        //end point
	pPRU->stExtObjectCmd.stExtractObjPar.stMultiLineInput.astLine[0].coPoint2.x = PR_DEF_CENTRE_Y;        //end point
    pPRU->stExtObjectCmd.stExtractObjPar.stMultiLineInput.coAlignRetPt.x = PR_DEF_CENTRE_X;
	pPRU->stExtObjectCmd.stExtractObjPar.stMultiLineInput.coAlignRetPt.y = PR_DEF_CENTRE_Y;
	pPRU->stExtObjectCmd.stExtractObjPar.stMultiLineInput.rMaxScale = 101;
    pPRU->stExtObjectCmd.stExtractObjPar.stMultiLineInput.rMinScale = 99;
	pPRU->stExtObjectCmd.stExtractObjPar.stMultiLineInput.wEdgeMag = 20;
	pPRU->stExtObjectCmd.emObjAttribute = PR_OBJ_ATTRIBUTE_BRIGHT;
	pPRU->stExtObjectCmd.rPassScore = 60;
	pPRU->stExtObjectCmd.stOpRegion.uwNumOfCorners = PR_NO_OF_CORNERS;
	pPRU->stExtObjectCmd.stOpRegion.acoCorners[PR_UPPER_LEFT].x = PR_MIN_COORD + 10;    //stWin is search ROI, expected line should be within the region
	pPRU->stExtObjectCmd.stOpRegion.acoCorners[PR_UPPER_LEFT].y = PR_MIN_COORD + 10;
	pPRU->stExtObjectCmd.stOpRegion.acoCorners[PR_LOWER_RIGHT].x = PR_MAX_COORD - 10;
	pPRU->stExtObjectCmd.stOpRegion.acoCorners[PR_LOWER_RIGHT].y = PR_MAX_COORD - 10;
    pPRU->stExtObjectCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].x = PR_MAX_COORD - 10;
    pPRU->stExtObjectCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].y = PR_MIN_COORD + 10;
    pPRU->stExtObjectCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].x = PR_MIN_COORD + 10;
	pPRU->stExtObjectCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].y = PR_MAX_COORD - 10;
}

BOOL PRS_DetectExpObjectCmd(PRU *pPRU)
{	
	PR_EXTRACT_OBJ_RPY1 stRpy1;
    PR_EXTRACT_OBJ_RPY2 stRpy2;

	PR_UWORD	uwComStatus = PR_COMM_NOERR;

	pPRU->stExtObjectCmd.stExtractObjPar.stMultiLineInput.uwNumOfLine = 1;
	pPRU->stExtObjectCmd.stExtractObjPar.stMultiLineInput.astLine[0].coPoint1.x = (PR_MAX_COORD + 1) / 8;        //start point
	pPRU->stExtObjectCmd.stExtractObjPar.stMultiLineInput.astLine[0].coPoint1.y = PR_DEF_CENTRE_Y;        //start point
	pPRU->stExtObjectCmd.stExtractObjPar.stMultiLineInput.astLine[0].coPoint2.x = (PR_MAX_COORD + 1) / 8 * 7;        //end point
	pPRU->stExtObjectCmd.stExtractObjPar.stMultiLineInput.astLine[0].coPoint2.y = PR_DEF_CENTRE_Y;        //end point

	pPRU->stExtObjectCmd.stOpRegion.acoCorners[PR_UPPER_LEFT].x = PR_MIN_COORD + 100;    //stWin is search ROI, expected line should be within the region
	pPRU->stExtObjectCmd.stOpRegion.acoCorners[PR_UPPER_LEFT].y = (PR_WORD)(PR_DEF_CENTRE_Y - g_lSearchRange);
	pPRU->stExtObjectCmd.stOpRegion.acoCorners[PR_LOWER_RIGHT].x = PR_MAX_COORD - 100;
	pPRU->stExtObjectCmd.stOpRegion.acoCorners[PR_LOWER_RIGHT].y = (PR_WORD)(PR_DEF_CENTRE_Y + g_lSearchRange);
    pPRU->stExtObjectCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].x = PR_MAX_COORD - 100;
    pPRU->stExtObjectCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].y = (PR_WORD)(PR_DEF_CENTRE_Y - g_lSearchRange);
    pPRU->stExtObjectCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].x = PR_MIN_COORD + 100;
	pPRU->stExtObjectCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].y = (PR_WORD)(PR_DEF_CENTRE_Y + g_lSearchRange);

	PR_ExtractObjCmd(&pPRU->stExtObjectCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &uwComStatus);
	PR_ExtractObjRpy1(pPRU->ubSenderID, &stRpy1);
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_ExtractObjRpy2(pPRU->ubSenderID, &stRpy2);
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}

	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		if (stRpy2.emShapeFound == PR_OBJSHAPE_FOUND_MULTILINE)
		pPRU->rcoDieCentre.x	= (PR_WORD)stRpy2.stResult.astMultiLineResult[0].rcoPos.x;	//(stRpy2.stResult.astMultiLineResult[0].astLine[0].coPoint1.x + stRpy2.stResult.astMultiLineResult[0].astLine[0].coPoint2.x) / 2 ;
		pPRU->rcoDieCentre.y	= (PR_WORD)stRpy2.stResult.astMultiLineResult[0].rcoPos.y; //(stRpy2.stResult.astMultiLineResult[0].astLine[0].coPoint1.y + stRpy2.stResult.astMultiLineResult[0].astLine[0].coPoint2.y) / 2 ;

		pPRU->rcoDieCentre.x	= (PR_REAL)stRpy2.stResult.astMultiLineResult[0].rcoPos.x; //20150728 float PR
		pPRU->rcoDieCentre.y	= (PR_REAL)stRpy2.stResult.astMultiLineResult[0].rcoPos.y;
		pPRU->bUseFloatingPointResult = TRUE;

		pPRU->fAng			= (float)(stRpy2.stResult.astMultiLineResult[0].rAngle / PI * 180.0);
		pPRU->fScore		= (float)stRpy2.stResult.astMultiLineResult[0].rScore;

		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
		PRS_SetChannelInUse(pPRU, FALSE);
		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		PRS_SetChannelInUse(pPRU, FALSE);
		return FALSE;
	}
}
// Exp Shape Functions
//
VOID PRS_InitExpShapeCmd(PRU *pPRU)	//20150106 Add Exp Shape Functions
{
	PR_InitExtShapeCmd(&pPRU->stExtShapeCmd);

	pPRU->stExtShapeCmd.emPurpose				= pPRU->emPurpose[PURPOSE0];
	pPRU->stExtShapeCmd.wEdgeMag				= 25/*PR_DEF_SHAPE_EDGE_MAG*/;
	pPRU->stExtShapeCmd.rPassScore				= PR_DEF_EXT_OBJ_PASS_SCORE;            //line score
	pPRU->stExtShapeCmd.emShapeType				= PR_SHAPE_TYPE_LINE;   //line detection
	pPRU->stExtShapeCmd.uwNumOfLine				= 1;                    //single line detection
	pPRU->stExtShapeCmd.uwSrchRange				= 100;
	pPRU->stExtShapeCmd.astLineCoord[0].coPoint1.x = (PR_MAX_COORD + 1) / 8; 
	pPRU->stExtShapeCmd.astLineCoord[0].coPoint1.y = PR_DEF_CENTRE_Y;
	pPRU->stExtShapeCmd.astLineCoord[0].coPoint2.x = (PR_MAX_COORD + 1) / 8 * 7; 
	pPRU->stExtShapeCmd.astLineCoord[0].coPoint2.y = PR_DEF_CENTRE_Y;

}

BOOL PRS_DetectExpShapeCmd(PRU *pPRU)
{	
	PR_EXT_SHAPE_RPY1		stRpy1;
	PR_EXT_SHAPE_RPY2		stRpy2;

	PR_UWORD	uwComStatus = PR_COMM_NOERR;

	pPRU->stExtShapeCmd.astLineCoord[0].coPoint1.x = (PR_MAX_COORD + 1) / 8; 
	pPRU->stExtShapeCmd.astLineCoord[0].coPoint1.y = PR_DEF_CENTRE_Y;
	pPRU->stExtShapeCmd.astLineCoord[0].coPoint2.x = (PR_MAX_COORD + 1) / 8 * 7; 
	pPRU->stExtShapeCmd.astLineCoord[0].coPoint2.y = PR_DEF_CENTRE_Y;

	PR_ExtShapeCmd(&pPRU->stExtShapeCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_ExtShapeRpy(pPRU->ubSenderID, &stRpy2);
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}

	

	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		pPRU->rcoDieCentre.x	= (PR_WORD)stRpy2.stShapeRes.rcoCentre.x;
		pPRU->rcoDieCentre.y	= (PR_WORD)stRpy2.stShapeRes.rcoCentre.y;

		pPRU->rcoDieCentre.x	= (PR_REAL)stRpy2.stShapeRes.rcoCentre.x; //20150728 float PR
		pPRU->rcoDieCentre.y	= (PR_REAL)stRpy2.stShapeRes.rcoCentre.y;
		pPRU->bUseFloatingPointResult = TRUE;

		pPRU->fAng			= (float)(stRpy2.stShapeRes.rAngle/ PI * 180.0);
		pPRU->fScore		= (float)stRpy2.stShapeRes.rScore;

		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
		PRS_SetChannelInUse(pPRU, FALSE);
		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		PRS_SetChannelInUse(pPRU, FALSE);
		return FALSE;
	}
}

VOID PRS_InitImageStatInfoCmd(PRU *pPRU)
{
	PR_InitImageStatInfoCmd(&pPRU->stImageStatInfoCmd);

	pPRU->stImageStatInfoCmd.emSameView		= PR_FALSE;
	pPRU->stImageStatInfoCmd.emPurpose		= pPRU->emPurpose[PURPOSE0];

	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.uwNDetectRegion					= 1;	// number of detect regions
	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].uwNumOfCorners	= 4;	// number of corners
	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[0].x = 5000;
	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[0].y = 1000;
	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[1].x = 1000;
	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[1].y = 1000;
	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[2].x = 1000;
	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[2].y = 5000;
	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[3].x = 5000;
	pPRU->stImageStatInfoCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[3].y = 5000;
}

BOOL PRS_LoadImageStatInfo(PRU *pPRU)
{
	PR_IMAGE_STAT_INFO_RPY1		stRpy1;
	PR_IMAGE_STAT_INFO_RPY2		stRpy2;

	PR_UWORD	uwComStatus = PR_COMM_NOERR;

	pPRU->bStatLoaded = FALSE;

	//PRS_GetOptic(pPRU); //20150429
	PR_ImageStatInfoCmd(&pPRU->stImageStatInfoCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_ImageStatInfoRpy(pPRU->ubSenderID, &stRpy2);
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		pPRU->bStatLoaded	= TRUE;
		pPRU->fStatMean		= (FLOAT)stRpy2.stMeanStdRes.rMean;
		pPRU->fStatStdev	= (FLOAT)stRpy2.stMeanStdRes.rStdev;

		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

BOOL PRS_GetImageStatInfo(PRU *pPRU)
{
	PR_IMAGE_STAT_INFO_RPY1		stRpy1;
	PR_IMAGE_STAT_INFO_RPY2		stRpy2;

	PR_UWORD	uwComStatus = PR_COMM_NOERR;

	//PRS_GetOptic(pPRU); //20150429
	PR_ImageStatInfoCmd(&pPRU->stImageStatInfoCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_ImageStatInfoRpy(pPRU->ubSenderID, &stRpy2);
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		pPRU->fStatMean	= (FLOAT)stRpy2.stMeanStdRes.rMean;
		pPRU->fStatStdev	= (FLOAT)stRpy2.stMeanStdRes.rStdev;

		PRS_ClearTextRow(pPRU->nCamera, PRS_MSG_ROW1);
		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

//Sub BackGround Only
VOID PRS_InitGrabShareImgCmd(PRU *pPRU)
{
	PR_InitGrabShareImgCmd(&pPRU->stShareImgCmd);
	pPRU->stShareImgCmd.uwNumOfPurpose		= 1;
	pPRU->stShareImgCmd.emPurpose			= pPRU->emPurpose[PURPOSE0];
	pPRU->stShareImgCmd.aemPurpose[0]		= pPRU->emPurpose[PURPOSE0];
	pPRU->stShareImgCmd.aemPurpose[1]		= pPRU->emPurpose[PURPOSE1];
	pPRU->stShareImgCmd.emGrabFromRecord	= PR_FALSE;
	pPRU->stShareImgCmd.emOnTheFlyGrab		= PR_FALSE;

}

BOOL PRS_GrabShareImgCmd(PRU *pPRU) //learn Background
{
	PR_GRAB_SHARE_IMAGE_RPY		stRpy1;
	PR_GRAB_SHARE_IMAGE_RPY2	stRpy2;

	//PRS_GetOptic(pPRU);
	PR_GrabShareImgCmd(&pPRU->stShareImgCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_GrabShareImgRpy(pPRU->ubSenderID, &stRpy2);
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		pPRU->lStationID		= stRpy2.ulStationID;
		pPRU->lImageID			= stRpy2.ulImageID;
			

		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

VOID PRS_InitGrabDispImgCmd(PRU *pPRU)
{
	PR_InitGrabDispImgCmd(&pPRU->stDispImgCmd);
	pPRU->stDispImgCmd.emPurpose			= pPRU->emPurpose[PURPOSE0]; // same camera
}

BOOL PRS_GrabDispImgCmd(PRU *pPRU) //learn Background
{
	IMG_UWORD				uwCommunStatus;
	//PR_GRAB_DISP_IMG_RPY	stRpy1;

	//PRS_GetOptic(pPRU);
	PR_GrabDispImgCmd(&pPRU->stDispImgCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &uwCommunStatus);

	// reply 1 OK?
	/*if (uwCommunStatus == PR_COMM_NOERR)
	{
		PR_GrabDispImgRpy(pPRU->ubSenderID, &stRpy1);
	}
	else
	{
		if (uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}*/

	return TRUE;
}

//Sub ACF
VOID PRS_InitLrnACFCmd(PRU *pPRU)
{
	PR_InitLrnACFCmd(&pPRU->stLrnACFCmd);
	pPRU->stLrnACFCmd.uwNumOfPurpose		= 1; // same camera //sharedImg.umNumOfPurpose
	pPRU->stLrnACFCmd.aemPurpose[0] 		= pPRU->emPurpose[PURPOSE0];
	pPRU->stLrnACFCmd.aemPurpose[1]			= pPRU->emPurpose[PURPOSE1];
	pPRU->stLrnACFCmd.emACFSegmentAlg		= PR_ACF_SEGMENT_ALG_SUBTRACTION;
	pPRU->stLrnACFCmd.emACFAlignAlg			= PR_ACF_ALIGN_ALG_PARALLEL_LINE_POINT_FITTING;
	pPRU->stLrnACFCmd.emACFAttribute		= PR_OBJ_ATTRIBUTE_BRIGHT;
	pPRU->stLrnACFCmd.emACFPosition			= PR_OBJ_POSITION_LEFT;
	pPRU->stLrnACFCmd.emBackgroundType		= PR_GLASS_PANEL;
	pPRU->stLrnACFCmd.ubSegThreshold			= 100;
	pPRU->stLrnACFCmd.uwPreSubtractStationID	= 0;
	pPRU->stLrnACFCmd.ulPreSubtractImageID		= 0;
	pPRU->stLrnACFCmd.emACFInspAlg			= PR_ACF_INSP_ALG_TMPL;
	pPRU->stLrnACFCmd.uwIsAlign				= TRUE;

//learn Region
	pPRU->stLrnACFCmd.stACFRegion.uwNumOfCorners	= 4;
	pPRU->stLrnACFCmd.stACFRegion.acoCorners[0].x = PR_DEF_CENTRE_X;
	pPRU->stLrnACFCmd.stACFRegion.acoCorners[0].y = PR_DEF_CENTRE_Y;
	pPRU->stLrnACFCmd.stACFRegion.acoCorners[1].x = PR_DEF_CENTRE_X;
	pPRU->stLrnACFCmd.stACFRegion.acoCorners[1].y = PR_DEF_CENTRE_Y;
	pPRU->stLrnACFCmd.stACFRegion.acoCorners[2].x = PR_DEF_CENTRE_X;
	pPRU->stLrnACFCmd.stACFRegion.acoCorners[2].y = PR_DEF_CENTRE_Y;
	pPRU->stLrnACFCmd.stACFRegion.acoCorners[3].x = PR_DEF_CENTRE_X;
	pPRU->stLrnACFCmd.stACFRegion.acoCorners[3].y = PR_DEF_CENTRE_Y;

	//Search Region
	pPRU->stLrnACFCmd.stSrchRegion.uwNumOfCorners		= 4;
	pPRU->stLrnACFCmd.stSrchRegion.acoCorners[0].x		= PR_DEF_CENTRE_X;
	pPRU->stLrnACFCmd.stSrchRegion.acoCorners[0].y		= PR_DEF_CENTRE_Y;
	pPRU->stLrnACFCmd.stSrchRegion.acoCorners[1].x		= PR_DEF_CENTRE_X;
	pPRU->stLrnACFCmd.stSrchRegion.acoCorners[1].y		= PR_DEF_CENTRE_Y;
	pPRU->stLrnACFCmd.stSrchRegion.acoCorners[2].x		= PR_DEF_CENTRE_X;
	pPRU->stLrnACFCmd.stSrchRegion.acoCorners[2].y		= PR_DEF_CENTRE_Y;
	pPRU->stLrnACFCmd.stSrchRegion.acoCorners[3].x		= PR_DEF_CENTRE_X;
	pPRU->stLrnACFCmd.stSrchRegion.acoCorners[3].y		= PR_DEF_CENTRE_Y;

	//Detect Region
	pPRU->stLrnACFCmd.stDefectMultRegion.uwNMultRegion = 1;
	for (INT i = 0; i < 10; i++)
	{
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[i].uwNumOfCorners		= 4;
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[i].acoCorners[0].x		= PR_DEF_CENTRE_X;
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[i].acoCorners[0].y		= PR_DEF_CENTRE_Y;
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[i].acoCorners[1].x		= PR_DEF_CENTRE_X;
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[i].acoCorners[1].y		= PR_DEF_CENTRE_Y;
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[i].acoCorners[2].x		= PR_DEF_CENTRE_X;
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[i].acoCorners[2].y		= PR_DEF_CENTRE_Y;
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[i].acoCorners[3].x		= PR_DEF_CENTRE_X;
		pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[i].acoCorners[3].y		= PR_DEF_CENTRE_Y;
	}
	//ChipArea
	pPRU->stLrnACFCmd.stChipMultRegion.uwNMultRegion = 2;
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0].uwNumOfCorners	= pPRU->stLrnACFCmd.stACFRegion.uwNumOfCorners;
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[0]					= pPRU->stLrnACFCmd.stACFRegion;
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1].uwNumOfCorners = pPRU->stLrnACFCmd.stACFRegion.uwNumOfCorners;
	pPRU->stLrnACFCmd.stChipMultRegion.astRegion[1]					= pPRU->stLrnACFCmd.stDefectMultRegion.astRegion[0];
	
	pPRU->stLrnACFCmd.emDisplayInfo										= PR_ACF_DISPLAY_CORNERS_AND_REGIONS;


}


VOID PRS_InitSrchACFCmd(PRU *pPRU)
{
	PR_InitSrchACFCmd(&pPRU->stSrchACFCmd);
	pPRU->stSrchACFCmd.stACFInsp.aeMinChipArea						= 0;
	for (INT i = 0; i < PR_MAX_REGION; i++)
	{
		pPRU->stSrchACFCmd.stACFInsp.aeMinSingleDefectArea[i]		= 0;
		pPRU->stSrchACFCmd.stACFInsp.aeMinTotalDefectArea[i]		= 0;
	}
	pPRU->stSrchACFCmd.stACFInsp.emDefectAttribute					= PR_DEFECT_ATTRIBUTE_BRIGHT;
	pPRU->stSrchACFCmd.stACFInsp.ubDefectThreshold					= 30;
	pPRU->stSrchACFCmd.rMatchScore									= 75; //201412170.75;


}

BOOL PRS_LrnACFCmd(PRU *pPRU)
{
	PR_LRN_ACF_RPY1	stRpy1;
	PR_LRN_ACF_RPY2	stRpy2;

	//Init
	stRpy2.emACFResultType = (PR_ACF_RESULT_TYPE)PR_ACF_RESULT_TYPE_CHIP_FAIL;

	//PRS_GetOptic(pPRU);
	PR_LrnACFCmd(&pPRU->stLrnACFCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_LrnACFRpy(pPRU->ubSenderID, &stRpy2);
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		pPRU->lACFResult	= stRpy2.emACFResultType;
		if (stRpy2.emACFResultType != PR_ACF_RESULT_TYPE_GOOD)
		{
			PRS_PrintACFResult(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.emACFResultType);
			return FALSE;
		}
		else
		{
			pPRU->stSrchACFCmd.uwRecordID = stRpy2.uwRecordID;
			// Add me later
			return TRUE;
		}
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}


BOOL PRS_SrchACFCmd(PRU *pPRU)
{
	PR_SRCH_ACF_RPY1		stRpy1;
	PR_SRCH_ACF_RPY2		stRpy2;

	stRpy2.emACFResultType = (PR_ACF_RESULT_TYPE)PR_ACF_RESULT_TYPE_CHIP_FAIL;

	//PRS_GetOptic(pPRU);
	PR_SrchACFCmd(&pPRU->stSrchACFCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_SrchACFRpy(pPRU->ubSenderID, &stRpy2);
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy1.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy1.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		//Align
		pPRU->rcoDieCentre.x				= /*(PR_WORD)*/stRpy2.stACFAlignResult.rcoACFCentre.x;	
		pPRU->rcoDieCentre.y				= /*(PR_WORD)*/stRpy2.stACFAlignResult.rcoACFCentre.y;

		pPRU->rcoDieCentre.x			= (PR_REAL)stRpy2.stACFAlignResult.rcoACFCentre.x;	//20150728 float PR
		pPRU->rcoDieCentre.y			= (PR_REAL)stRpy2.stACFAlignResult.rcoACFCentre.y;
		pPRU->bUseFloatingPointResult = TRUE;

		pPRU->fAng						= stRpy2.stACFAlignResult.rACFRotation;
		pPRU->fScore					= stRpy2.stACFAlignResult.rMatchScore;// 20141217 * 100;

		pPRU->arcoDieCorners[PR_UPPER_RIGHT].x	= /*(PR_WORD)*/stRpy2.stACFAlignResult.arcoACFCorners[PR_UPPER_RIGHT].x;
		pPRU->arcoDieCorners[PR_UPPER_RIGHT].y	= /*(PR_WORD)*/stRpy2.stACFAlignResult.arcoACFCorners[PR_UPPER_RIGHT].y;
		pPRU->arcoDieCorners[PR_UPPER_LEFT].x	= /*(PR_WORD)*/stRpy2.stACFAlignResult.arcoACFCorners[PR_UPPER_LEFT].x;
		pPRU->arcoDieCorners[PR_UPPER_LEFT].y	= /*(PR_WORD)*/stRpy2.stACFAlignResult.arcoACFCorners[PR_UPPER_LEFT].y;
		pPRU->arcoDieCorners[PR_LOWER_LEFT].x	= /*(PR_WORD)*/stRpy2.stACFAlignResult.arcoACFCorners[PR_LOWER_LEFT].x;
		pPRU->arcoDieCorners[PR_LOWER_LEFT].y	= /*(PR_WORD)*/stRpy2.stACFAlignResult.arcoACFCorners[PR_LOWER_LEFT].y;
		pPRU->arcoDieCorners[PR_LOWER_RIGHT].x	= /*(PR_WORD)*/stRpy2.stACFAlignResult.arcoACFCorners[PR_LOWER_RIGHT].x;
		pPRU->arcoDieCorners[PR_LOWER_RIGHT].y	= /*(PR_WORD)*/stRpy2.stACFAlignResult.arcoACFCorners[PR_LOWER_RIGHT].y;

		// Inspect
		pPRU->dTotalDefectArea			= stRpy2.stACFInspResult.aeTotalDefectArea[0]; // For each Defect Are.
		pPRU->lACFResult				= stRpy2.emACFResultType;

		if (stRpy2.emACFResultType != PR_ACF_RESULT_TYPE_GOOD)
		{
			PRS_PrintACFResult(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.emACFResultType);
			return FALSE;
		}

		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy2.stStatus.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy2.stStatus.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

BOOL PRS_SrchACFCmdGrabOnly(PRU *pPRU)
{
	PR_SRCH_ACF_RPY1		stRpy1;
	CString str;

	//PRS_GetOptic(pPRU);
	PR_SrchACFCmd(&pPRU->stSrchACFCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PRS_SetChannelInUse(pPRU, TRUE);
		return TRUE;
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
#ifdef NO_INDIRECTVIEW_TXT
			str.Format("Cam: %s. PRS Comm Error: %x", pPRU->szStnName, stRpy1.uwCommunStatus);
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
#endif
		}
		else
		{
#ifdef NO_INDIRECTVIEW_TXT
			PRS_GetPrintPRError(stRpy1.uwPRStatus, str); //20151007
			str =  CString("Cam: ") + pPRU->szStnName + CString(". ") + str;
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
#endif
		}
		return FALSE;
	}
}

BOOL PRS_SrchACFCmdProcessOnly(PRU *pPRU)
{
	CString str("");
	PR_SRCH_ACF_RPY2		stRpy2;
	stRpy2.emACFResultType = (PR_ACF_RESULT_TYPE)PR_ACF_RESULT_TYPE_CHIP_FAIL;

	PR_SrchACFRpy(pPRU->ubSenderID, &stRpy2);

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		//Align
		pPRU->rcoDieCentre.x				= /*(PR_WORD)*/stRpy2.stACFAlignResult.rcoACFCentre.x;	
		pPRU->rcoDieCentre.y				= /*(PR_WORD)*/stRpy2.stACFAlignResult.rcoACFCentre.y;

		pPRU->rcoDieCentre.x			= (PR_REAL)stRpy2.stACFAlignResult.rcoACFCentre.x;	//20150728 float PR
		pPRU->rcoDieCentre.y			= (PR_REAL)stRpy2.stACFAlignResult.rcoACFCentre.y;
		pPRU->bUseFloatingPointResult = TRUE;

		pPRU->fAng						= stRpy2.stACFAlignResult.rACFRotation;
		pPRU->fScore					= stRpy2.stACFAlignResult.rMatchScore; //20141217 * 100;
		DOUBLE dTotalArea = 0.0;
		for (INT i = 0; i < PR_MAX_REGION; i++)
		{
			dTotalArea += stRpy2.stACFInspResult.aeTotalDefectArea[i]; // For each Defect Are.
		}
		pPRU->dTotalDefectArea			= dTotalArea;
		pPRU->lACFResult				= stRpy2.emACFResultType;

		if (stRpy2.emACFResultType != PR_ACF_RESULT_TYPE_GOOD)
		{
#ifdef NO_INDIRECTVIEW_TXT
			PRS_GetPrintACFResult(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.emACFResultType, str); //20151007
			str =  CString("Cam: ") + pPRU->szStnName + CString(". ") + str;
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_PrintACFResult(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.emACFResultType);
#endif
			PRS_SetChannelInUse(pPRU, FALSE);
			return FALSE;
		}

		PRS_SetChannelInUse(pPRU, FALSE);
		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
#ifdef NO_INDIRECTVIEW_TXT
			str.Format("Cam: %s, PRS Comm Error: %x", pPRU->szStnName, stRpy2.stStatus.uwCommunStatus);
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
#endif
		}
		else
		{
#ifdef NO_INDIRECTVIEW_TXT
			PRS_GetPrintPRError(stRpy2.stStatus.uwPRStatus, str); //20151007
			str =  CString("Cam: ") + pPRU->szStnName + CString(". ") + str;
			theApp.WriteHMIMess(str, TRUE);
#else
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
#endif
		}

		PRS_SetChannelInUse(pPRU, FALSE);
		return FALSE;
	}
}

// General Record & Parameter Functions
//
BOOL PRS_FreeRecordID(PRU *pPRU, PR_RECORD_TYPE emRecordType, PR_UWORD &uwRecordID)	// 20141024 Yip
{
	PR_FREE_RECORD_ID_CMD	stCmd;
	PR_FREE_RECORD_ID_RPY	stRpy;
	
	PR_InitFreeRecordIDCmd(&stCmd);
	if (uwRecordID != 0)
	{
		stCmd.emRecordType = emRecordType;
		stCmd.uwRecordID = uwRecordID;

		PR_FreeRecordIDCmd(&stCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy);

		if (stRpy.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stRpy.uwPRStatus))
		{
			if (stRpy.uwCommunStatus != PR_COMM_NOERR)
			{
				PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy.uwCommunStatus);
			}
			else
			{
				PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy.uwPRStatus);
			}
			return FALSE;
		}
		uwRecordID = 0;
	}
	return TRUE;
}

BOOL PRS_FreeRecordID(PRU *pPRU)
{
#if 1 //visionProc 20180208
	if (pPRU->stSrchCmd.uwRecordID != 0)
	{
		if (!PRS_FreeRecordID(pPRU, PRS_GROUP_TYPE, pPRU->stSrchCmd.uwRecordID)) //visionProc use PRS_GROUP_TYPE
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
#else
	//if (!pPRU->bLoaded)
	//{
	//	return TRUE;
	//}
	//// free pattern record ID
	//if (!PRS_FreeRecordID(pPRU, PRS_DIE_TYPE, pPRU->stSrchCmd.uwRecordID))
	//{
	//	return FALSE;
	//}

	//// free pattern record ID
	//if (!PRS_FreeRecordID(pPRU, PRS_DIE_TYPE, pPRU->stSrchDieCmd.auwRecordID[0]))
	//{
	//	return FALSE;
	//}

	//// free shape record ID
	//if (!PRS_FreeRecordID(pPRU, PRS_SHAPE_TYPE, pPRU->stDetectShapeCmd.uwRecordID))
	//{
	//	return FALSE;
	//}

	//// free template record ID
	//if (!PRS_FreeRecordID(pPRU, PRS_TMPL_TYPE, pPRU->stSrchTmplCmd.uwRecordID))
	//{
	//	return FALSE;
	//}

	//// 20141023 Yip: Add Kerf Fitting Functions
	//// free Kerf record ID
	//if (!PRS_FreeRecordID(pPRU, PRS_KERF_TYPE, pPRU->stInspKerfCmd.uwRecordID))
	//{
	//	return FALSE;
	//}
	//
	//// free ACF record ID
	//if (!PRS_FreeRecordID(pPRU, PRS_ACF_TYPE, pPRU->stSrchACFCmd.uwRecordID))
	//{
	//	return FALSE;
	//}
#endif
	pPRU->bLoaded = FALSE;

	return TRUE;
}

VOID PRS_FreeAllRecords()
{
	INT i = 0;
	PR_FREE_ALL_RECORDS_RPY	stRpy;

	PR_FreeAllRecordsCmd(PRS_MENU_SID, PRS_MENU_RID, &stRpy);

}

VOID PRS_FreeAllRecordsAllHost()
{
	INT i = 0;
	PR_FREE_ALL_RECORDS_RPY	stRpy;

	PR_FreeAllRecordsAllHostsCmd(PRS_MENU_SID, PRS_MENU_RID, &stRpy);

}

BOOL PRS_DispLrnInfo(PRU *pPRU)
{
	PR_DISP_LRN_INFO_CMD	stCmd;
	PR_DISP_LRN_INFO_RPY	stRpy;

	if (!pPRU->bLoaded)
	{
		return FALSE;
	}

	PR_InitDispLrnInfoCmd(&stCmd);

	if(pPRU->emRecordType == PRS_ACF_TYPE)
	{
		stCmd.uwRecordID = pPRU->stSrchACFCmd.uwRecordID;
	}
	else
	{
		stCmd.uwRecordID = pPRU->stSrchCmd.uwRecordID;
	}
	//switch (pPRU->emRecordType)
	//{
	//// Pattern Matching or Edge Matching
	//case PRS_DIE_TYPE:
	//	stCmd.uwRecordID = pPRU->stSrchDieCmd.auwRecordID[0];
	//	break;

	//// Shape Fitting
	//case PRS_SHAPE_TYPE:
	//	stCmd.uwRecordID = pPRU->stDetectShapeCmd.uwRecordID;
	//	break;

	//// Tmpl Fitting
	//case PRS_TMPL_TYPE:
	//	stCmd.uwRecordID = pPRU->stSrchTmplCmd.uwRecordID;
	//	break;

	//// Kerf Fitting
	//case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
	//	stCmd.uwRecordID = pPRU->stInspKerfCmd.uwRecordID;
	//	break;

	//case PRS_ACF_TYPE:
	//	stCmd.uwRecordID = pPRU->stSrchACFCmd.uwRecordID;
	//	break;
	//}

	stCmd.emDispLrnInfoLevel	= PR_DISP_LRN_INFO_ZOOMOUT_256;		// zoom out (256 x 256)
	//stCmd.coRefPoint
	//stCmd.rRefAngle
	stCmd.emIsRefPointAligned	= PR_FALSE;
	stCmd.emIsOverlayLrn		= PR_TRUE;

	PR_DispLrnInfoCmd(&stCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy);

	return TRUE;
}

BOOL PRS_GetRecordInfo(PRU *pPRU, PR_GET_RECORD_INFO_RPY *pstRpy)
{
	PR_GET_RECORD_INFO_CMD	stCmd;
	PR_GET_RECORD_INFO_RPY	stRpy;

	if(pPRU->emRecordType == PRS_ACF_TYPE)
	{
		stCmd.uwRecordID = pPRU->stSrchACFCmd.uwRecordID;
	}
	else
	{
		stCmd.uwRecordID = pPRU->stSrchCmd.uwRecordID;
	}
	//switch (pPRU->emRecordType)
	//{
	//// Pattern Matching or Edge Matching
	//case PRS_DIE_TYPE:
	//	stCmd.uwRecordID = pPRU->stSrchDieCmd.auwRecordID[0];
	//	break;

	//// Shape Fitting
	//case PRS_SHAPE_TYPE:
	//	stCmd.uwRecordID = pPRU->stDetectShapeCmd.uwRecordID;
	//	break;

	//// Tmpl Fitting
	//case PRS_TMPL_TYPE:
	//	stCmd.uwRecordID = pPRU->stSrchTmplCmd.uwRecordID;
	//	break;

	//// Kerf Fitting
	//case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
	//	stCmd.uwRecordID = pPRU->stInspKerfCmd.uwRecordID;
	//	break;

	//case PRS_ACF_TYPE:
	//	stCmd.uwRecordID = pPRU->stSrchACFCmd.uwRecordID;
	//	break;
	//}

	stCmd.emIsReturnRecordStatistics = PR_FALSE;

	PR_GetRecordInfoCmd(&stCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy);

	if (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stRpy.stStatus.uwPRStatus))
	{
		if (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy.stStatus.uwPRStatus);
		}

		return FALSE;
	}

	*pstRpy = stRpy;

	return TRUE;
}

VOID PRS_InitUploadRecordProcessCmd(PRU *pPRU)
{
	PR_InitUploadRecordProcessCmd(&pPRU->stUploadRecordCmd);

	//pPRU->stUploadRecordCmd.ulRecordID		= *;
	pPRU->stUploadRecordCmd.emIsOverwrite		= PR_TRUE;
	//pPRU->stUploadRecordCmd.acFilename		= "d:\\sw\\AC9000\\record\\*\\*.zip";
}

VOID PRS_InitDownloadRecordProcessCmd(PRU *pPRU)
{
	PR_InitDownloadRecordProcessCmd(&pPRU->stDownloadRecordCmd);

	//pPRU->stDownloadRecordCmd.ulRecordID		= *;
	pPRU->stDownloadRecordCmd.emIsOverwrite		= PR_TRUE;
	//pPRU->stDownloadRecordCmd.acFilename		= "d:\\sw\\AC9000\\record\\*\\*.zip";
}

BOOL PRS_UploadRecordProcess(PRU *pPRU, CString *pszErrorMsg)
{ 
//save zip file to .\record
	PR_UPLOAD_RECORD_PROCESS_RPY	stRpy;

	if (!pPRU->bLoaded)
	{
		if (pszErrorMsg != NULL)
		{
			*pszErrorMsg = _T("");    //20121105
		} 
		return TRUE;
	}

	if(pPRU->emRecordType == PRS_ACF_TYPE)
	{
		pPRU->stUploadRecordCmd.ulRecordID = pPRU->stSrchACFCmd.uwRecordID;
	}
	else
	{
		pPRU->stUploadRecordCmd.ulRecordID = pPRU->stSrchCmd.uwRecordID;
	}
	//switch (pPRU->emRecordType)
	//{
	//// Pattern Matching or Edge Matching
	//case PRS_DIE_TYPE:
	//	pPRU->stUploadRecordCmd.ulRecordID = pPRU->stSrchDieCmd.auwRecordID[0];
	//	break;

	//// Shape Fitting
	//case PRS_SHAPE_TYPE:
	//	pPRU->stUploadRecordCmd.ulRecordID = pPRU->stDetectShapeCmd.uwRecordID;
	//	break;

	//// Tmpl Fitting
	//case PRS_TMPL_TYPE:
	//	pPRU->stUploadRecordCmd.ulRecordID = pPRU->stSrchTmplCmd.uwRecordID;
	//	break;

	//// Kerf Fitting
	//case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
	//	pPRU->stUploadRecordCmd.ulRecordID = pPRU->stInspKerfCmd.uwRecordID;
	//	break;

	//case PRS_ACF_TYPE:
	//	pPRU->stUploadRecordCmd.ulRecordID = pPRU->stSrchACFCmd.uwRecordID;
	//	break;
	//}

	PR_UploadRecordProcessCmd(&pPRU->stUploadRecordCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy);

	if (stRpy.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stRpy.uwPRStatus))
	{
		if (stRpy.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy.uwCommunStatus);
			if (pszErrorMsg != NULL)
			{
				pszErrorMsg->Format("Comm Error: %x", stRpy.uwCommunStatus);    //20121105
			} 
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy.uwPRStatus);
			if (pszErrorMsg != NULL)
			{
				pszErrorMsg->Format("PRS Error: %x", stRpy.uwPRStatus);    //20121105
			} 
		}

		return FALSE;
	}

	if (pszErrorMsg != NULL)
	{
		*pszErrorMsg = _T("");    //20121105
	} 
	return TRUE;
}

BOOL PRS_DownloadRecordProcess(PRU *pPRU, BOOL bIsOverwriteRecord, LONG &lRetRecordID, CString *pszErrorMsg)
{ 
//load zip file from .\record
	PR_DOWNLOAD_RECORD_PROCESS_RPY	stRpy;

#if 1 //20130713
	if (!CFileFind().FindFile((char*) & (pPRU->stDownloadRecordCmd.acFilename[0])))
	{
		//do not call pr download function if there is no record zip file
		if (pszErrorMsg != NULL)
		{
			pszErrorMsg->Format("File not find: %s", pPRU->stDownloadRecordCmd.acFilename);
		}
		pPRU->bLoaded = FALSE;
		return TRUE;
	}
	if (pszErrorMsg != NULL)
	{
		*pszErrorMsg = _T("");
	}
#endif
	pPRU->stDownloadRecordCmd.ulRecordID = 0;
	pPRU->stDownloadRecordCmd.emIsOverwrite = bIsOverwriteRecord ? PR_TRUE : PR_FALSE;

	//switch (pPRU->emRecordType)
	//{
	//// Pattern Matching or Edge Matching
	//case PRS_DIE_TYPE:
	//	pPRU->stDownloadRecordCmd.ulRecordID = pPRU->stSrchDieCmd.auwRecordID[0];
	//	break;

	//// Shape Fitting
	//case PRS_SHAPE_TYPE:
	//	pPRU->stDownloadRecordCmd.ulRecordID = pPRU->stDetectShapeCmd.uwRecordID;
	//	break;

	//// Tmpl Fitting
	//case PRS_TMPL_TYPE:
	//	pPRU->stDownloadRecordCmd.ulRecordID = pPRU->stSrchTmplCmd.uwRecordID;
	//	break;

	//// Kerf Fitting
	//case PRS_KERF_TYPE:	// 20141023 Yip: Add Kerf Fitting Functions
	//	pPRU->stDownloadRecordCmd.ulRecordID = pPRU->stInspKerfCmd.uwRecordID;
	//	break;

	//// ACF 
	//case PRS_ACF_TYPE:
	//	pPRU->stDownloadRecordCmd.ulRecordID = pPRU->stSrchACFCmd.uwRecordID;
	//	break;
	//}

	PR_DownloadRecordProcessCmd(&pPRU->stDownloadRecordCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy);

	if (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stRpy.stStatus.uwPRStatus))
	{
		if (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy.stStatus.uwCommunStatus);
			if (pszErrorMsg != NULL)
			{
				pszErrorMsg->Format("Comm Error: %x", stRpy.stStatus.uwCommunStatus);    //20130713
			} 
		}
		else
		{
			if (pPRU->bLoaded)
			{
				PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy.stStatus.uwPRStatus);
				if (pszErrorMsg != NULL)
				{
					pszErrorMsg->Format("PRS Error: %x", stRpy.stStatus.uwPRStatus);    //20130713
				} 
			}
		}

		pPRU->bLoaded = FALSE;
		return FALSE;
	}

	lRetRecordID = (LONG)stRpy.ulRetRecordID;
	pPRU->bLoaded = TRUE;
	return TRUE;
}


// Log Functions
//
VOID PRS_SetDebugFlag(PR_DEBUG emDebugFlag)
{
	PR_SetDebugFlag(emDebugFlag);
}

VOID PRS_SetComLogFlag(PR_COM_LOG emComLogFlag)
{
	PR_SetComLogFlag(emComLogFlag);
}

VOID PRS_SetComLogTimeLimit(PR_ULWORD ulTimeLimit)
{
	//PR_SetComLogTimeLimit(ulTimeLimit);
}

VOID PRS_EnableLog()
{
	PR_ENABLE_LOG_CMD	stCmd;
	PR_ENABLE_LOG_RPY	stRpy;

	PR_InitEnableLogCmd(&stCmd);
	stCmd.ubHostID = PRS_HOST_SID;
	
	PR_EnableLogCmd(&stCmd, PRS_MENU_SID, PRS_MENU_RID, &stRpy);
}

VOID PRS_DisableLog()
{
	PR_DISABLE_LOG_CMD	stCmd;
	PR_DISABLE_LOG_RPY	stRpy;

	PR_InitDisableLogCmd(&stCmd);
	stCmd.ubHostID = PRS_HOST_SID;
	
	PR_DisableLogCmd(&stCmd, PRS_MENU_SID, PRS_MENU_RID, &stRpy);
}


// Miscellaneous
//
VOID PRS_GetHostInterfaceVersionNo(PR_UBYTE	*pubHostITFVersionNo)  //20120106 get version no of interface
{
	PR_GetHostITFVersionNo(pubHostITFVersionNo);
}

BOOL PRS_GetVersionNo(PR_GET_VERSION_NO_RPY *pstRpy)
{
	PR_GET_VERSION_NO_RPY	stRpy;

	PRU *pPRU;

	if (nCamera == NO_CAM)
	{
		return 0;
	}

	pPRU = &PRS_SetCameraPRU(nCamera);

	PR_GetVersionNoCmd(PRS_MENU_SID, PRS_MENU_RID, &stRpy);

	if (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stRpy.stStatus.uwPRStatus))
	{
		if (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy.stStatus.uwPRStatus);
		}

		return FALSE;
	}

	*pstRpy = stRpy;

	return TRUE;
}

VOID PRS_SaveDispImg(PRU *pPRU, CString szPath)
{
	PR_SAVE_DISP_IMG_CMD	stCmd;
	PR_SAVE_DISP_IMG_RPY	stRpy;

	PR_InitSaveDispImgCmd(&stCmd);

	strcpy((char*) & (stCmd.aubSourceLogFileNameWithPath[0]), szPath);

	PR_SaveDispImgCmd(&stCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy);

	if (PR_ERROR_STATUS(stRpy.stStatus.uwPRStatus) || stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR)
	{
		if (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy.stStatus.uwPRStatus);
		}
	}
}

BOOL PRS_GrabDispImage(PRU *pPRU)
{
	PR_GRAB_DISP_IMG_CMD	stCmd;
	PR_GRAB_DISP_IMG_RPY	stRpy;
	PR_UWORD				uwCommunStatus;

	PR_InitGrabDispImgCmd(&stCmd);

	stCmd.emCameraNo	= pPRU->emCameraNo;
	stCmd.emDisplay		= PR_TRUE;
	stCmd.emGrab		= PR_TRUE;
	stCmd.emPurpose		= pPRU->emPurpose[0];

	PR_GrabDispImgCmd(&stCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &uwCommunStatus);

	if (uwCommunStatus != PR_COMM_NOERR)
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", uwCommunStatus);
		return FALSE;
	}
	else
	{
		PR_GrabDispImgRpy(pPRU->ubSenderID, &stRpy);

		if (stRpy.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy.uwPRStatus))
		{
			return TRUE;
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy.uwPRStatus);
			return FALSE;
		}
	}
}

VOID PRS_VisionNtDlg(PR_VISIONNT_DLG emDlg)
{
	PR_VISIONNT_DLG_CMD	stCmd;
	PR_VISIONNT_DLG_RPY	stRpy;

	PR_InitVisionNtDlgCmd(&stCmd);
	stCmd.emDlg = emDlg;

	PR_VisionNtDlgCmd(&stCmd, PRS_MENU_SID, PRS_MENU_RID, &stRpy);
}

BOOL PRS_ProcImgSimpleCmd(PRU *pPRU)
{
	PR_PROC_IMG_SIMPLE_CMD		stCmd;
	PR_UWORD					uwCommunStatus = PR_COMM_ERR;
	PR_PROC_IMG_SIMPLE_RPY1		stRpy1;
	PR_PROC_IMG_SIMPLE_RPY2		stRpy2;
	
	PR_InitProcImgSimpleCmd(&stCmd);
	stCmd.stImg.emGrab = PR_GRAB_FROM_RECORD_ID;
	stCmd.stImg.uwRecordID = pPRU->emID;
	
	PR_ProcImgSimpleCmd(&stCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &uwCommunStatus);

	if (uwCommunStatus != PR_COMM_NOERR)
	{
		PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", uwCommunStatus);
		return FALSE;
	}

	PR_ProcImgSimpleRpy1(pPRU->ubSenderID, &stRpy1);

	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_ProcImgSimpleRpy2(pPRU->ubSenderID, &stRpy2);
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy1.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy1.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_COMM, stRpy2.stStatus.uwCommunStatus);
			//PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_DisplayError(pPRU, PR_ERROR_TYPE_PR, stRpy2.stStatus.uwPRStatus);
			//PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

// Calibrations

static INT DoubleToLong(DOUBLE dInput)
{
	INT iOutput;

	if (dInput < 0)
	{
		iOutput = (INT)(dInput - 0.5);
	}
	else
	{
		iOutput = (INT)(dInput + 0.5);
	}

	return iOutput;
}

VOID PRS_PRPosToMtrOffset(PRU *pPRU, PR_COORD const coPRPos, MTR_POSN *pMtrOffset)
{
	DOUBLE a_term, b_term, c_term, d_term;

	if (!pPRU->bCalibrated)
	{
		pMtrOffset->x = 0;
		pMtrOffset->y = 0;
	}
	
	//if (pPRU->bUseFirstABCD)
	if (pPRU->eCamMoveGroup1 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform.a * (DOUBLE)(coPRPos.x - PR_DEF_CENTRE_X);
		b_term = (DOUBLE)pPRU->p2m_xform.b * (DOUBLE)(coPRPos.y - PR_DEF_CENTRE_Y);
		c_term = (DOUBLE)pPRU->p2m_xform.c * (DOUBLE)(coPRPos.x - PR_DEF_CENTRE_X);
		d_term = (DOUBLE)pPRU->p2m_xform.d * (DOUBLE)(coPRPos.y - PR_DEF_CENTRE_Y);
	}
	else if (pPRU->eCamMoveGroup2 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_2.a * (DOUBLE)(coPRPos.x - PR_DEF_CENTRE_X);
		b_term = (DOUBLE)pPRU->p2m_xform_2.b * (DOUBLE)(coPRPos.y - PR_DEF_CENTRE_Y);
		c_term = (DOUBLE)pPRU->p2m_xform_2.c * (DOUBLE)(coPRPos.x - PR_DEF_CENTRE_X);
		d_term = (DOUBLE)pPRU->p2m_xform_2.d * (DOUBLE)(coPRPos.y - PR_DEF_CENTRE_Y);
	}
	else if (pPRU->eCamMoveGroup3 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_3.a * (DOUBLE)(coPRPos.x - PR_DEF_CENTRE_X);
		b_term = (DOUBLE)pPRU->p2m_xform_3.b * (DOUBLE)(coPRPos.y - PR_DEF_CENTRE_Y);
		c_term = (DOUBLE)pPRU->p2m_xform_3.c * (DOUBLE)(coPRPos.x - PR_DEF_CENTRE_X);
		d_term = (DOUBLE)pPRU->p2m_xform_3.d * (DOUBLE)(coPRPos.y - PR_DEF_CENTRE_Y);
	}
	else if (pPRU->eCamMoveGroup4 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_4.a * (DOUBLE)(coPRPos.x - PR_DEF_CENTRE_X);
		b_term = (DOUBLE)pPRU->p2m_xform_4.b * (DOUBLE)(coPRPos.y - PR_DEF_CENTRE_Y);
		c_term = (DOUBLE)pPRU->p2m_xform_4.c * (DOUBLE)(coPRPos.x - PR_DEF_CENTRE_X);
		d_term = (DOUBLE)pPRU->p2m_xform_4.d * (DOUBLE)(coPRPos.y - PR_DEF_CENTRE_Y);
	}

	pMtrOffset->x = DoubleToLong(a_term + b_term);
	pMtrOffset->y = DoubleToLong(c_term + d_term);
}

VOID PRS_PRPosToMtrOffset(PRU *pPRU, PR_RCOORD const rcoPRPos, MTR_POSN *pMtrOffset) //20150728 float PR
{
	DOUBLE a_term = 0.0, b_term = 0.0, c_term = 0.0, d_term = 0.0;

	if (!pPRU->bCalibrated)
	{
		pMtrOffset->x = 0;
		pMtrOffset->y = 0;
	}
	
	if (pPRU->eCamMoveGroup1 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform.a * (DOUBLE)(rcoPRPos.x - (DOUBLE)PR_DEF_CENTRE_X);
		b_term = (DOUBLE)pPRU->p2m_xform.b * (DOUBLE)(rcoPRPos.y - (DOUBLE)PR_DEF_CENTRE_Y);
		c_term = (DOUBLE)pPRU->p2m_xform.c * (DOUBLE)(rcoPRPos.x - (DOUBLE)PR_DEF_CENTRE_X);
		d_term = (DOUBLE)pPRU->p2m_xform.d * (DOUBLE)(rcoPRPos.y - (DOUBLE)PR_DEF_CENTRE_Y);
	}
	else if (pPRU->eCamMoveGroup2 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_2.a * (DOUBLE)(rcoPRPos.x - (DOUBLE)PR_DEF_CENTRE_X);
		b_term = (DOUBLE)pPRU->p2m_xform_2.b * (DOUBLE)(rcoPRPos.y - (DOUBLE)PR_DEF_CENTRE_Y);
		c_term = (DOUBLE)pPRU->p2m_xform_2.c * (DOUBLE)(rcoPRPos.x - (DOUBLE)PR_DEF_CENTRE_X);
		d_term = (DOUBLE)pPRU->p2m_xform_2.d * (DOUBLE)(rcoPRPos.y - (DOUBLE)PR_DEF_CENTRE_Y);
	}
	else if (pPRU->eCamMoveGroup3 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_3.a * (DOUBLE)(rcoPRPos.x - (DOUBLE)PR_DEF_CENTRE_X);
		b_term = (DOUBLE)pPRU->p2m_xform_3.b * (DOUBLE)(rcoPRPos.y - (DOUBLE)PR_DEF_CENTRE_Y);
		c_term = (DOUBLE)pPRU->p2m_xform_3.c * (DOUBLE)(rcoPRPos.x - (DOUBLE)PR_DEF_CENTRE_X);
		d_term = (DOUBLE)pPRU->p2m_xform_3.d * (DOUBLE)(rcoPRPos.y - (DOUBLE)PR_DEF_CENTRE_Y);
	}
	else if (pPRU->eCamMoveGroup4 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_4.a * (DOUBLE)(rcoPRPos.x - (DOUBLE)PR_DEF_CENTRE_X);
		b_term = (DOUBLE)pPRU->p2m_xform_4.b * (DOUBLE)(rcoPRPos.y - (DOUBLE)PR_DEF_CENTRE_Y);
		c_term = (DOUBLE)pPRU->p2m_xform_4.c * (DOUBLE)(rcoPRPos.x - (DOUBLE)PR_DEF_CENTRE_X);
		d_term = (DOUBLE)pPRU->p2m_xform_4.d * (DOUBLE)(rcoPRPos.y - (DOUBLE)PR_DEF_CENTRE_Y);
	}

	pMtrOffset->x = DoubleToLong(a_term + b_term);
	pMtrOffset->y = DoubleToLong(c_term + d_term);
}

VOID PRS_PRPosToDMtrOffset(PRU *pPRU, PR_COORD const coPRPos, D_MTR_POSN *pdMtrOffset)
{
	DOUBLE a_term, b_term, c_term, d_term;

	if (!pPRU->bCalibrated)
	{
		pdMtrOffset->x = 0.0;
		pdMtrOffset->y = 0.0;
	}

	//if (pPRU->bUseFirstABCD)
	if (pPRU->eCamMoveGroup1 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform.a * (DOUBLE)(coPRPos.x - PR_DEF_CENTRE_X);
		b_term = (DOUBLE)pPRU->p2m_xform.b * (DOUBLE)(coPRPos.y - PR_DEF_CENTRE_Y);
		c_term = (DOUBLE)pPRU->p2m_xform.c * (DOUBLE)(coPRPos.x - PR_DEF_CENTRE_X);
		d_term = (DOUBLE)pPRU->p2m_xform.d * (DOUBLE)(coPRPos.y - PR_DEF_CENTRE_Y);
	}
	else if (pPRU->eCamMoveGroup2 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_2.a * (DOUBLE)(coPRPos.x - PR_DEF_CENTRE_X);
		b_term = (DOUBLE)pPRU->p2m_xform_2.b * (DOUBLE)(coPRPos.y - PR_DEF_CENTRE_Y);
		c_term = (DOUBLE)pPRU->p2m_xform_2.c * (DOUBLE)(coPRPos.x - PR_DEF_CENTRE_X);
		d_term = (DOUBLE)pPRU->p2m_xform_2.d * (DOUBLE)(coPRPos.y - PR_DEF_CENTRE_Y);
	}
	else if (pPRU->eCamMoveGroup3 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_3.a * (DOUBLE)(coPRPos.x - PR_DEF_CENTRE_X);
		b_term = (DOUBLE)pPRU->p2m_xform_3.b * (DOUBLE)(coPRPos.y - PR_DEF_CENTRE_Y);
		c_term = (DOUBLE)pPRU->p2m_xform_3.c * (DOUBLE)(coPRPos.x - PR_DEF_CENTRE_X);
		d_term = (DOUBLE)pPRU->p2m_xform_3.d * (DOUBLE)(coPRPos.y - PR_DEF_CENTRE_Y);
	}
	else if (pPRU->eCamMoveGroup4 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_4.a * (DOUBLE)(coPRPos.x - PR_DEF_CENTRE_X);
		b_term = (DOUBLE)pPRU->p2m_xform_4.b * (DOUBLE)(coPRPos.y - PR_DEF_CENTRE_Y);
		c_term = (DOUBLE)pPRU->p2m_xform_4.c * (DOUBLE)(coPRPos.x - PR_DEF_CENTRE_X);
		d_term = (DOUBLE)pPRU->p2m_xform_4.d * (DOUBLE)(coPRPos.y - PR_DEF_CENTRE_Y);
	}

	pdMtrOffset->x = a_term + b_term;
	pdMtrOffset->y = c_term + d_term;
}

VOID PRS_PRPosToDMtrOffset(PRU *pPRU, PR_RCOORD const rcoPRPos, D_MTR_POSN *pdMtrOffset) //20150728 float PR
{

	DOUBLE a_term = 0.0, b_term = 0.0, c_term = 0.0, d_term = 0.0;

	if (!pPRU->bCalibrated)
	{
		pdMtrOffset->x = 0.0;
		pdMtrOffset->y = 0.0;
	}

	//if (pPRU->bUseFirstABCD)
	if (pPRU->eCamMoveGroup1 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform.a * (DOUBLE)(rcoPRPos.x - (DOUBLE)PR_DEF_CENTRE_X);
		b_term = (DOUBLE)pPRU->p2m_xform.b * (DOUBLE)(rcoPRPos.y - (DOUBLE)PR_DEF_CENTRE_Y);
		c_term = (DOUBLE)pPRU->p2m_xform.c * (DOUBLE)(rcoPRPos.x - (DOUBLE)PR_DEF_CENTRE_X);
		d_term = (DOUBLE)pPRU->p2m_xform.d * (DOUBLE)(rcoPRPos.y - (DOUBLE)PR_DEF_CENTRE_Y);
	}
	else if (pPRU->eCamMoveGroup2 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_2.a * (DOUBLE)(rcoPRPos.x - (DOUBLE)PR_DEF_CENTRE_X);
		b_term = (DOUBLE)pPRU->p2m_xform_2.b * (DOUBLE)(rcoPRPos.y - (DOUBLE)PR_DEF_CENTRE_Y);
		c_term = (DOUBLE)pPRU->p2m_xform_2.c * (DOUBLE)(rcoPRPos.x - (DOUBLE)PR_DEF_CENTRE_X);
		d_term = (DOUBLE)pPRU->p2m_xform_2.d * (DOUBLE)(rcoPRPos.y - (DOUBLE)PR_DEF_CENTRE_Y);
	}
	else if (pPRU->eCamMoveGroup3 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_3.a * (DOUBLE)(rcoPRPos.x - (DOUBLE)PR_DEF_CENTRE_X);
		b_term = (DOUBLE)pPRU->p2m_xform_3.b * (DOUBLE)(rcoPRPos.y - (DOUBLE)PR_DEF_CENTRE_Y);
		c_term = (DOUBLE)pPRU->p2m_xform_3.c * (DOUBLE)(rcoPRPos.x - (DOUBLE)PR_DEF_CENTRE_X);
		d_term = (DOUBLE)pPRU->p2m_xform_3.d * (DOUBLE)(rcoPRPos.y - (DOUBLE)PR_DEF_CENTRE_Y);
	}
	else if (pPRU->eCamMoveGroup4 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_4.a * (DOUBLE)(rcoPRPos.x - (DOUBLE)PR_DEF_CENTRE_X);
		b_term = (DOUBLE)pPRU->p2m_xform_4.b * (DOUBLE)(rcoPRPos.y - (DOUBLE)PR_DEF_CENTRE_Y);
		c_term = (DOUBLE)pPRU->p2m_xform_4.c * (DOUBLE)(rcoPRPos.x - (DOUBLE)PR_DEF_CENTRE_X);
		d_term = (DOUBLE)pPRU->p2m_xform_4.d * (DOUBLE)(rcoPRPos.y - (DOUBLE)PR_DEF_CENTRE_Y);
	}

	pdMtrOffset->x = a_term + b_term;
	pdMtrOffset->y = c_term + d_term;
}

VOID PRS_PROffsetToDMtrOffset(PRU *pPRU, PR_COORD const coPROffset, D_MTR_POSN *pdMtrOffset) //20121105
{
	DOUBLE a_term, b_term, c_term, d_term;

	if (!pPRU->bCalibrated)
	{
		pdMtrOffset->x = 0.0;
		pdMtrOffset->y = 0.0;
	}

	//if (pPRU->bUseFirstABCD)
	if (pPRU->eCamMoveGroup1 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform.a * (DOUBLE) coPROffset.x;
		b_term = (DOUBLE)pPRU->p2m_xform.b * (DOUBLE) coPROffset.y;
		c_term = (DOUBLE)pPRU->p2m_xform.c * (DOUBLE) coPROffset.x;
		d_term = (DOUBLE)pPRU->p2m_xform.d * (DOUBLE) coPROffset.y;
	}
	else if (pPRU->eCamMoveGroup2 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_2.a * (DOUBLE) coPROffset.x;
		b_term = (DOUBLE)pPRU->p2m_xform_2.b * (DOUBLE) coPROffset.y;
		c_term = (DOUBLE)pPRU->p2m_xform_2.c * (DOUBLE) coPROffset.x;
		d_term = (DOUBLE)pPRU->p2m_xform_2.d * (DOUBLE) coPROffset.y;
	}
	else if (pPRU->eCamMoveGroup3 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_3.a * (DOUBLE) coPROffset.x;
		b_term = (DOUBLE)pPRU->p2m_xform_3.b * (DOUBLE) coPROffset.y;
		c_term = (DOUBLE)pPRU->p2m_xform_3.c * (DOUBLE) coPROffset.x;
		d_term = (DOUBLE)pPRU->p2m_xform_3.d * (DOUBLE) coPROffset.y;
	}
	else if (pPRU->eCamMoveGroup4 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_4.a * (DOUBLE) coPROffset.x;
		b_term = (DOUBLE)pPRU->p2m_xform_4.b * (DOUBLE) coPROffset.y;
		c_term = (DOUBLE)pPRU->p2m_xform_4.c * (DOUBLE) coPROffset.x;
		d_term = (DOUBLE)pPRU->p2m_xform_4.d * (DOUBLE) coPROffset.y;
	}

	pdMtrOffset->x = a_term + b_term;
	pdMtrOffset->y = c_term + d_term;
}

VOID PRS_PROffsetToDMtrOffset(PRU *pPRU, PR_RCOORD const rcoPROffset, D_MTR_POSN *pdMtrOffset) //20150728 float PR
{
	DOUBLE a_term = 0.0, b_term = 0.0, c_term = 0.0, d_term = 0.0;

	if (!pPRU->bCalibrated)
	{
		pdMtrOffset->x = 0.0;
		pdMtrOffset->y = 0.0;
	}

	//if (pPRU->bUseFirstABCD)
	if (pPRU->eCamMoveGroup1 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform.a * (DOUBLE) rcoPROffset.x;
		b_term = (DOUBLE)pPRU->p2m_xform.b * (DOUBLE) rcoPROffset.y;
		c_term = (DOUBLE)pPRU->p2m_xform.c * (DOUBLE) rcoPROffset.x;
		d_term = (DOUBLE)pPRU->p2m_xform.d * (DOUBLE) rcoPROffset.y;
	}
	else if (pPRU->eCamMoveGroup2 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_2.a * (DOUBLE) rcoPROffset.x;
		b_term = (DOUBLE)pPRU->p2m_xform_2.b * (DOUBLE) rcoPROffset.y;
		c_term = (DOUBLE)pPRU->p2m_xform_2.c * (DOUBLE) rcoPROffset.x;
		d_term = (DOUBLE)pPRU->p2m_xform_2.d * (DOUBLE) rcoPROffset.y;
	}
	else if (pPRU->eCamMoveGroup3 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_3.a * (DOUBLE) rcoPROffset.x;
		b_term = (DOUBLE)pPRU->p2m_xform_3.b * (DOUBLE) rcoPROffset.y;
		c_term = (DOUBLE)pPRU->p2m_xform_3.c * (DOUBLE) rcoPROffset.x;
		d_term = (DOUBLE)pPRU->p2m_xform_3.d * (DOUBLE) rcoPROffset.y;
	}
	else if (pPRU->eCamMoveGroup4 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->p2m_xform_4.a * (DOUBLE) rcoPROffset.x;
		b_term = (DOUBLE)pPRU->p2m_xform_4.b * (DOUBLE) rcoPROffset.y;
		c_term = (DOUBLE)pPRU->p2m_xform_4.c * (DOUBLE) rcoPROffset.x;
		d_term = (DOUBLE)pPRU->p2m_xform_4.d * (DOUBLE) rcoPROffset.y;
	}

	pdMtrOffset->x = a_term + b_term;
	pdMtrOffset->y = c_term + d_term;
}

VOID PRS_MtrOffsetToPRPos(PRU *pPRU, MTR_POSN const coMtrOffset, PR_COORD *pPRPos)
{
	DOUBLE a_term, b_term, c_term, d_term;

	//if (pPRU->bUseFirstABCD)
	if (pPRU->eCamMoveGroup1 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->m2p_xform.a * (DOUBLE)coMtrOffset.x;
		b_term = (DOUBLE)pPRU->m2p_xform.b * (DOUBLE)coMtrOffset.y;
		c_term = (DOUBLE)pPRU->m2p_xform.c * (DOUBLE)coMtrOffset.x;
		d_term = (DOUBLE)pPRU->m2p_xform.d * (DOUBLE)coMtrOffset.y;
	}
	else if (pPRU->eCamMoveGroup2 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->m2p_xform_2.a * (DOUBLE)coMtrOffset.x;
		b_term = (DOUBLE)pPRU->m2p_xform_2.b * (DOUBLE)coMtrOffset.y;
		c_term = (DOUBLE)pPRU->m2p_xform_2.c * (DOUBLE)coMtrOffset.x;
		d_term = (DOUBLE)pPRU->m2p_xform_2.d * (DOUBLE)coMtrOffset.y;
	}
	else if (pPRU->eCamMoveGroup3 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->m2p_xform_3.a * (DOUBLE)coMtrOffset.x;
		b_term = (DOUBLE)pPRU->m2p_xform_3.b * (DOUBLE)coMtrOffset.y;
		c_term = (DOUBLE)pPRU->m2p_xform_3.c * (DOUBLE)coMtrOffset.x;
		d_term = (DOUBLE)pPRU->m2p_xform_3.d * (DOUBLE)coMtrOffset.y;
	}
	else if (pPRU->eCamMoveGroup4 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->m2p_xform_4.a * (DOUBLE)coMtrOffset.x;
		b_term = (DOUBLE)pPRU->m2p_xform_4.b * (DOUBLE)coMtrOffset.y;
		c_term = (DOUBLE)pPRU->m2p_xform_4.c * (DOUBLE)coMtrOffset.x;
		d_term = (DOUBLE)pPRU->m2p_xform_4.d * (DOUBLE)coMtrOffset.y;
	}

	pPRPos->x = DoubleToLong(a_term + b_term + (DOUBLE)PR_DEF_CENTRE_X);
	pPRPos->y = DoubleToLong(c_term + d_term + (DOUBLE)PR_DEF_CENTRE_Y);
}

VOID PRS_MtrOffsetToPRPos(PRU *pPRU, MTR_POSN const coMtrOffset, PR_RCOORD *prPRPos) //20150728 float PR
{
	DOUBLE a_term = 0.0, b_term = 0.0, c_term = 0.0, d_term = 0.0;

	//if (pPRU->bUseFirstABCD)
	if (pPRU->eCamMoveGroup1 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->m2p_xform.a * (DOUBLE)coMtrOffset.x;
		b_term = (DOUBLE)pPRU->m2p_xform.b * (DOUBLE)coMtrOffset.y;
		c_term = (DOUBLE)pPRU->m2p_xform.c * (DOUBLE)coMtrOffset.x;
		d_term = (DOUBLE)pPRU->m2p_xform.d * (DOUBLE)coMtrOffset.y;
	}
	else if (pPRU->eCamMoveGroup2 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->m2p_xform_2.a * (DOUBLE)coMtrOffset.x;
		b_term = (DOUBLE)pPRU->m2p_xform_2.b * (DOUBLE)coMtrOffset.y;
		c_term = (DOUBLE)pPRU->m2p_xform_2.c * (DOUBLE)coMtrOffset.x;
		d_term = (DOUBLE)pPRU->m2p_xform_2.d * (DOUBLE)coMtrOffset.y;
	}
	else if (pPRU->eCamMoveGroup3 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->m2p_xform_3.a * (DOUBLE)coMtrOffset.x;
		b_term = (DOUBLE)pPRU->m2p_xform_3.b * (DOUBLE)coMtrOffset.y;
		c_term = (DOUBLE)pPRU->m2p_xform_3.c * (DOUBLE)coMtrOffset.x;
		d_term = (DOUBLE)pPRU->m2p_xform_3.d * (DOUBLE)coMtrOffset.y;
	}
	else if (pPRU->eCamMoveGroup4 == pPRU->eCamMoveGroupCurrent)
	{
		a_term = (DOUBLE)pPRU->m2p_xform_4.a * (DOUBLE)coMtrOffset.x;
		b_term = (DOUBLE)pPRU->m2p_xform_4.b * (DOUBLE)coMtrOffset.y;
		c_term = (DOUBLE)pPRU->m2p_xform_4.c * (DOUBLE)coMtrOffset.x;
		d_term = (DOUBLE)pPRU->m2p_xform_4.d * (DOUBLE)coMtrOffset.y;
	}

	prPRPos->x = (PR_REAL)(a_term + b_term + (DOUBLE)PR_DEF_CENTRE_X);
	prPRPos->y = (PR_REAL)(c_term + d_term + (DOUBLE)PR_DEF_CENTRE_Y);
}

VOID PRS_DistanceToPROffset(PRU *pPRU, D_MTR_POSN const dDistance, PR_COORD *pPROffset) //20121102
{ 
//distance in um
  //dFOV in mm
	pPROffset->x = (PR_WORD)(dDistance.x / pPRU->dFOV / 1000.0 * (PR_MAX_COORD + 1));
	pPROffset->y = (PR_WORD)(dDistance.y / pPRU->dFOV / 1000.0 * (PR_MAX_COORD + 1));
}

VOID PRS_CopyPurpose(PRU *pPRU)
{
	PR_COMMON_RPY	stRpy;

	//PR_UWORD		uwSourceNo;
	//PR_SOURCE		emSource;
	//PR_UWORD		uwLevel;
	//PR_PURPOSE		emPurpose;

	//PRS_GetOptic(pPRU);
	//PR_GetSourceNo(&pPRU->stOptic, pPRU->ubSenderID, pPRU->ubReceiverID, &uwSourceNo, &stRpy);
	//for (INT i = 0; i < uwSourceNo; i++)
	//{
	//	PR_GetLightSource(&pPRU->stOptic, i, pPRU->ubSenderID, pPRU->ubReceiverID, &emSource, &stRpy);
	//}
	PR_CopyPurposeFromRef(pPRU->emPurpose[PURPOSE1], pPRU->stOptic.emPurpose, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy);
	//PR_GetGeneralPurposeByPurpose(pPRU->stOptic.emPurpose, pPRU->ubSenderID, pPRU->ubReceiverID, &pPRU->stOptic2.emPurpose, &stRpy);
}

BOOL PRS_DisplayACFInfo(PRU *pPRU, PRU *pPRUBackground, PR_ACF_DISPLAY lInfo)
{
	PR_LRN_ACF_CMD	stCmd;
	PR_LRN_ACF_RPY1	stRpy1;
	PR_LRN_ACF_RPY2	stRpy2;
	
	PR_InitLrnACFCmd(&stCmd);

	stCmd.uwNumOfPurpose				= pPRU->stLrnACFCmd.uwNumOfPurpose; // same camera //sharedImg.umNumOfPurpose
	stCmd.aemPurpose[0] 				= pPRU->emPurpose[PURPOSE0];
	stCmd.aemPurpose[1]					= pPRU->emPurpose[PURPOSE1];
	stCmd.emDisplayInfo					= lInfo;
	stCmd.emACFSegmentAlg				= pPRU->stLrnACFCmd.emACFSegmentAlg;
	stCmd.ubSegThreshold				= pPRU->stLrnACFCmd.ubSegThreshold;
	stCmd.stSrchRegion.uwNumOfCorners	= 4;
	stCmd.stSrchRegion.acoCorners[0].x	= PR_DEF_WIN_LRC_X;
	stCmd.stSrchRegion.acoCorners[0].y	= PR_DEF_WIN_ULC_Y;
	stCmd.stSrchRegion.acoCorners[1].x	= PR_DEF_WIN_ULC_X;
	stCmd.stSrchRegion.acoCorners[1].y	= PR_DEF_WIN_ULC_Y;
	stCmd.stSrchRegion.acoCorners[2].x	= PR_DEF_WIN_ULC_X;
	stCmd.stSrchRegion.acoCorners[2].y	= PR_DEF_WIN_LRC_Y;
	stCmd.stSrchRegion.acoCorners[3].x	= PR_DEF_WIN_LRC_X;
	stCmd.stSrchRegion.acoCorners[3].y	= PR_DEF_WIN_LRC_Y;
	stCmd.emACFAttribute				= pPRU->stLrnACFCmd.emACFAttribute; // support Dark ACF Only
	stCmd.uwPreSubtractStationID		= (PR_UWORD)pPRUBackground->lStationID;
	stCmd.ulPreSubtractImageID			= pPRUBackground->lImageID;
	stCmd.emACFPosition					= pPRU->stLrnACFCmd.emACFPosition;
	stCmd.emBackgroundType				= pPRU->stLrnACFCmd.emBackgroundType;
	stCmd.emACFAlignAlg					= pPRU->stLrnACFCmd.emACFAlignAlg;
	stCmd.emACFInspAlg					= pPRU->stLrnACFCmd.emACFInspAlg;
	stCmd.stACFRegion.uwNumOfCorners	= 4;
	stCmd.stACFRegion					= pPRU->stLrnACFCmd.stACFRegion;
	stCmd.uwIsAlign						= pPRU->stLrnACFCmd.uwIsAlign;

	PR_LrnACFCmd(&stCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRpy1);
	
	// reply 1 OK?
	if (stRpy1.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		PR_LrnACFRpy(pPRU->ubSenderID, &stRpy2);
	}
	else
	{
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy1.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy1.uwPRStatus);
		}
		return FALSE;
	}

	// reply 2 OK?
	if (stRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR && !PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		//pPRU->stSrchACFCmd.uwRecordID = stRpy2.uwRecordID;

		// Add me later
		return TRUE;
	}
	else
	{
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stRpy2.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stRpy2.stStatus.uwPRStatus);
		}
		return FALSE;
	}
}

VOID PRS_RemoveShareImg(PRU *pPRUBackground)
{
	PR_REMOVE_SHARE_IMAGE_CMD	stCmd;
	PR_REMOVE_SHARE_IMAGE_RPY	stRpy;

	PR_InitRemoveShareImgCmd(&stCmd);
	stCmd.ulImageID	= pPRUBackground->lImageID;

	PR_RemoveShareImgCmd(&stCmd, pPRUBackground->ubSenderID, pPRUBackground->ubReceiverID, &stRpy);
}

/////////////////////////
// PR Log
/////////////////////////
BOOL PRS_EnableLogCmd(PRU *pPRU, BOOL bMode, LOG_CONDITION LogCondition)
{
	PR_ENABLE_LOG_CMD	stEnableLogCmd;
	PR_ENABLE_LOG_RPY	stEnableLogRpy;
	PR_DISABLE_LOG_CMD	stDisableLogCmd;
	PR_DISABLE_LOG_RPY	stDisableLogRpy;

	if (bMode)
	{
		PR_InitEnableLogCmd(&stEnableLogCmd);
		stEnableLogCmd.ubHostID		= pPRU->ubSenderID;

		//switch(LogCondition)
		//{
		//case LOG_ALWAYS:
		//	//PR_SetAllCondition(&stEnableLogCmd);
		//	break;
		//case LOG_FAIL:
		//	//PR_SetAllFailureCondition(&stEnableLogCmd);
		//	break;
		//case LOG_ALIGN_FAIL:
		//	//PR_SetAlignFailureCondition(&stEnableLogCmd);
		//	break;
		//case LOG_INSP_FAILE:
		//	//PR_SetInspFailureCondition(&stEnableLogCmd);
		//	break;
		//case LOG_MEASURE_FAILE:
		//	//PR_SetMeasureFailureCondition(&stEnableLogCmd);
		//	break;

		//default:
		//	//PR_SetAllCondition(&stEnableLogCmd);
		//}

		PR_EnableLogCmd(&stEnableLogCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stEnableLogRpy);

		if (stEnableLogRpy.stStatus.uwCommunStatus != PR_COMM_NOERR || stEnableLogRpy.stStatus.uwPRStatus != PR_ERR_NOERR)
		{
			return FALSE;
		}
		else 
		{
			return TRUE;
		}
	}
	else
	{
		PR_InitDisableLogCmd(&stDisableLogCmd);
		stDisableLogCmd.ubHostID		= pPRU->ubSenderID;
		
		PR_DisableLogCmd(&stDisableLogCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stDisableLogRpy);
		
		if (stDisableLogRpy.stStatus.uwCommunStatus != PR_COMM_NOERR || stDisableLogRpy.stStatus.uwPRStatus != PR_ERR_NOERR)
		{
			return FALSE;
		}
		else 
		{
			return TRUE;
		}
	}

	return FALSE;

}

#if 1 //20150722
#define	CLIU_NUM_OF_GRAB						20
BOOL PRS_GrabImageCmd(PRU *pPRU)
{
	PR_GRAB_SHARE_IMAGE_CMD		stImageCmd;
	PR_GRAB_SHARE_IMAGE_RPY		stImageRpy;
	PR_GRAB_SHARE_IMAGE_RPY2	stImageRpy2;

	PR_UWORD					uwCommunStatus = PR_COMM_ERR;
	PR_PROC_IMG_SIMPLE_CMD		stCmd;
	PR_PROC_IMG_SIMPLE_RPY1		stRpy1;
	PR_PROC_IMG_SIMPLE_RPY2		stRpy2;

	PR_REMOVE_SHARE_IMAGE_CMD	stRemoveCmd;
	PR_REMOVE_SHARE_IMAGE_RPY	stRemoveRpy;

	PR_ULWORD				aulStationID[CLIU_NUM_OF_GRAB], aulImageID[CLIU_NUM_OF_GRAB];

	PR_UWORD	i =0;
	PR_PURPOSE	astPurpose[CLIU_NUM_OF_GRAB];
	CString szTmp01("");
	std::deque <CString> m_deqTimeInfoSeq; //20131010

	PR_InitRemoveShareImgCmd( &stRemoveCmd );
	PR_InitGrabShareImgCmd(&stImageCmd);
	PR_InitProcImgSimpleCmd(&stCmd);

	for (i=0; i<CLIU_NUM_OF_GRAB; ++i)
	{
		aulStationID[i] = 0;
		aulImageID[i] = 0;
		//astPurpose[i] = PR_PURPOSE_CAM_A2; //my
		astPurpose[i] = pPRU->emPurpose[PURPOSE0];
	}
	m_deqTimeInfoSeq.clear();

	//grab image
	stImageCmd.emGrabFromRecord = PR_FALSE;		
	for (i=0; i<CLIU_NUM_OF_GRAB; ++i)
	{
		stImageCmd.emPurpose = astPurpose[i];
		//PR_GrabShareImgCmd ( &stImageCmd, ubSenderID, ubReceiverID, &stImageRpy ); //my
		PR_GrabShareImgCmd ( &stImageCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stImageRpy );

		//printf("\tPR_GrabShareImgCmd: comm %x, pr %x\n", 
		//	stImageRpy.uwCommunStatus,
		//	stImageRpy.uwPRStatus);
		szTmp01.Format("\tPR_GrabShareImgCmd: comm %x, pr %x\n", stImageRpy.uwCommunStatus, stImageRpy.uwPRStatus);
		TRACE0(szTmp01);

		//PR_GrabShareImgRpy ( ubSenderID,  &stImageRpy2 );
		PR_GrabShareImgRpy ( pPRU->ubSenderID,  &stImageRpy2 );

		if ( stImageRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR || 
			PR_ERROR_STATUS( stImageRpy2.stStatus.uwPRStatus ) ||
			PR_WARN_STATUS( stImageRpy2.stStatus.uwPRStatus ))
		{
			//printf("\tGrabShare RPY: comm %x, pr %x\n", 
			//	stImageRpy2.stStatus.uwCommunStatus,
			//	stImageRpy2.stStatus.uwPRStatus);
			szTmp01.Format("\tGrabShare RPY: comm %x, pr %x\n", stImageRpy2.stStatus.uwCommunStatus, stImageRpy2.stStatus.uwPRStatus);
			TRACE0(szTmp01);
		}
		else
		{
			//printf("PR_GrabShareImgCmd OK, Station ID: %lu, Image ID: %lu!\n",
			//	stImageRpy2.ulStationID, stImageRpy2.ulImageID);
			szTmp01.Format("PR_GrabShareImgCmd OK, Station ID: %lu, Image ID: %lu!\n", stImageRpy2.ulStationID, stImageRpy2.ulImageID);
			TRACE0(szTmp01);
		}

		aulStationID[i] = stImageRpy2.ulStationID;
		aulImageID[i] = stImageRpy2.ulImageID;
		if (1)
		{
			SYSTEMTIME ltime;
			GetLocalTime(&ltime);

			szTmp01.Format("%04u-%02u-%02u %02u:%02u:%02u.%03u\n", ltime.wYear, ltime.wMonth, ltime.wDay,
					ltime.wHour, ltime.wMinute, ltime.wSecond,  ltime.wMilliseconds);

			m_deqTimeInfoSeq.push_back(szTmp01);
		}

	}

	if (1)
	{
		FILE	*logFp = NULL;
		CString szFilePath("");
		SYSTEMTIME ltime;
		GetLocalTime(&ltime);

		szFilePath.Format("d:\\sw\\AC9000\\Data\\PR_GrabImage_%04u-%02u-%02u.txt", ltime.wYear, ltime.wMonth, ltime.wDay);
		logFp = fopen(szFilePath, "a+");
		if (logFp != NULL)
		{
			std::deque <LONG>::size_type size;
			size = m_deqTimeInfoSeq.size();
			if (size > 0)
			{
				for (INT ii = 0; ii < (INT)size; ii++)
				{
					fprintf(logFp, "\t\t%s\n", (char*) m_deqTimeInfoSeq[ii].GetBuffer(0));
				}
				fprintf(logFp, "\n");
			}
			fclose(logFp);
		}
	}
	m_deqTimeInfoSeq.clear();
	//system("pause");

	//save image, need to turn on log always to log images
	stCmd.stImg.emLatch = PR_LATCH_FROM_GALLERY;
	stCmd.stImg.emGrab = PR_GRAB_FROM_PURPOSE;
	stCmd.emIsDisplay = PR_FALSE;
    for (i=0; i<CLIU_NUM_OF_GRAB; ++i)
	{
		stCmd.stImg.uwStationID = (PR_UWORD)aulStationID[i];
		stCmd.stImg.ulGalleryImageID = aulImageID[i];
		stCmd.stImg.emPurpose = astPurpose[i];

		PR_ProcImgSimpleCmd(&stCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &uwCommunStatus);
		if (uwCommunStatus != PR_COMM_NOERR)
		{
			//printf("PR_ProcImgSimpleCmd is finished with commun status %u!\n", uwCommunStatus);
			szTmp01.Format("PR_ProcImgSimpleCmd is finished with commun status %u!\n", uwCommunStatus);
			TRACE0(szTmp01);
			continue;
		}

		PR_ProcImgSimpleRpy1(pPRU->ubSenderID, &stRpy1);
		if (stRpy1.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stRpy1.uwPRStatus))
		{
			//printf("PR_ProcImgSimpleRpy1 is finished with status %u, %u!\n",
			//	stRpy1.uwCommunStatus, stRpy1.uwPRStatus);
			szTmp01.Format("PR_ProcImgSimpleRpy1 is finished with status %u, %u!\n", stRpy1.uwCommunStatus, stRpy1.uwPRStatus);
			TRACE0(szTmp01);
			continue;
		}

		PR_ProcImgSimpleRpy2(pPRU->ubSenderID, &stRpy2);
		if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
		{
			//printf("PR_ProcImgSimpleRpy2 is finished with status %u, %u!\n",
			//	stRpy2.stStatus.uwCommunStatus, stRpy2.stStatus.uwPRStatus);
			szTmp01.Format("PR_ProcImgSimpleRpy2 is finished with status %u, %u!\n", stRpy2.stStatus.uwCommunStatus, stRpy2.stStatus.uwPRStatus);
			TRACE0(szTmp01);
			continue;
		}

		//printf("PR_ProcImgSimpleCmd OK, Station ID: %lu, Image ID: %lu!\n", stRpy2.ulStationID, stRpy2.ulImageID);
		szTmp01.Format("PR_ProcImgSimpleCmd OK, Station ID: %lu, Image ID: %lu!\n", stRpy2.ulStationID, stRpy2.ulImageID);
		TRACE0(szTmp01);

		//if (0)
		//{
		//	SYSTEMTIME ltime;
		//	GetLocalTime(&ltime);

		//	szTmp01.Format("%04u-%02u-%02u %02u:%02u:%02u.%u\n", ltime.wYear, ltime.wMonth, ltime.wDay,
		//			ltime.wHour, ltime.wMinute, ltime.wSecond,  ltime.wMilliseconds);
		//	TRACE0(szTmp01);
		//
		//	FILE	*logFp = NULL;
		//	CString szFilePath("");
		//	szFilePath.Format("d:\\sw\\AC9000\\Data\\PR_GrabImage_%04u-%02u-%02u", ltime.wYear, ltime.wMonth, ltime.wDay);
		//	logFp = fopen(szFilePath, "a+");
		//	if (logFp != NULL)
		//	{
		//		szTmp01.Format("%04u-%02u-%02u %02u:%02u:%02u.%u  Image:%d\n", ltime.wYear, ltime.wMonth, ltime.wDay,
		//				ltime.wHour, ltime.wMinute, ltime.wSecond,  ltime.wMilliseconds, (INT) i);
		//		fprintf(logFp, szTmp01);
		//		fclose(logFp);
		//	}
		//}
	}

	//remove image
	if (1)
	{
		for (i=0; i<CLIU_NUM_OF_GRAB; ++i)
		{
			stRemoveCmd.ulStationID = aulStationID[i];
			stRemoveCmd.ulImageID = aulImageID[i];

			PR_RemoveShareImgCmd(&stRemoveCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stRemoveRpy);
			if ( stRemoveRpy.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stRemoveRpy.uwPRStatus) )
			{
				//printf("PR_RemoveShareImgCmd() is finished with status %u, %u!\n",
				//	stRemoveRpy.uwCommunStatus, stRemoveRpy.uwPRStatus);
				szTmp01.Format("PR_RemoveShareImgCmd() is finished with status %u, %u!\n", stRemoveRpy.uwCommunStatus, stRemoveRpy.uwPRStatus);
				TRACE0(szTmp01);
			}
		}
	}
	return TRUE;
}
#endif


BOOL PRS_DistortionCalibration(PRU *pPRU) 
{
	PR_RSIZE rSize;
	PR_CALIBRATION_CMD		stCalibCmd;
	PR_CALIBRATION_RPY		stCalibRpy;
	rSize.x = 10.0;
	rSize.y = 10.0;

	PR_InitCalibrationCmd(&stCalibCmd);
	stCalibCmd.emCalMethod	= PR_CAL_METHOD_CHESSBOARD_DISTORTION;
	stCalibCmd.emEnableAutolighting = PR_FALSE;
	stCalibCmd.emPurpose	= pPRU->emPurpose[PURPOSE0];	//input purpose name that needs to be calibrated
	stCalibCmd.rszPhysicalSize = rSize;				//physical size of small square on calibration glass in mm

	stCalibCmd.emCalStage	= PR_CAL_STAGE_1;
	PR_CalibrationCmd( &stCalibCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stCalibRpy);
	if (stCalibRpy.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stCalibRpy.stStatus.uwPRStatus))
	{
		if (stCalibRpy.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stCalibRpy.stStatus.uwCommunStatus);
		}
		else
		{
			PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stCalibRpy.stStatus.uwPRStatus);
		}
		return FALSE;
	}
	else
	{
		stCalibCmd.emCalStage	= PR_CAL_STAGE_SAVE;
		PR_CalibrationCmd( &stCalibCmd, pPRU->ubSenderID, pPRU->ubReceiverID, &stCalibRpy );
		if (stCalibRpy.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stCalibRpy.stStatus.uwPRStatus))
		{
			if (stCalibRpy.stStatus.uwCommunStatus != PR_COMM_NOERR)
			{
				PRS_DisplayText(pPRU->nCamera, 1, PRS_MSG_ROW1, "Comm Error: %x", stCalibRpy.stStatus.uwCommunStatus);
			}
			else
			{
				PRS_PrintPRError(pPRU->nCamera, PRS_MSG_ROW1, stCalibRpy.stStatus.uwPRStatus);
			}
			return FALSE;
		}
	}
	return TRUE;
}
