#include "stdafx.h"

#include "AC9000Stn.h"
#include "AC9000.h"

#define MTR_SW_POS_LIMIT_HIT							2205

// CMotorInfo Related
CMotorInfo::CMotorInfo()
{
	pGmpChannel		= NULL;
	pAppStation		= NULL;
	pModSelected	= NULL;

	// 20140819 Yip: Fix Memory Leak Issue
	stAttrib.stSensorMapping.cHomePortAddrss = NULL;
	stAttrib.stSensorMapping.cNegLmtPortAddrss = NULL;
	stAttrib.stSensorMapping.cPosLmtPortAddrss = NULL;
	stMotionProfile.cMotionCtrlType = NULL;
	stCtrlProf.cSettlingCtrlType = NULL;
	stHomeProfile.cSrchCtrlType = NULL;
	stIndexProfile.cSrchCtrlType = NULL;
	stIndexProfile.cPort = NULL;
	for (INT i = 0; i < MAX_NUM_OF_SRCH_PROF; i++)
	{
		stUsrDefProfile[i].cPort = NULL;
	}
#ifdef SRCH_EMBEDDED_SENSOR //20170810
	bUseMtrSnrPort = FALSE;
	bEnableSrchEmbeddedHome = FALSE;
	bEnableSrchEmbeddedIndexer = FALSE;
	szMtrSnrPortName = "";
#endif
}

CMotorInfo::CMotorInfo(CGmpChannel *pGmpChannel, CAC9000App *pAppStation, BOOL *pModSelected)
{
	this->pGmpChannel		= pGmpChannel;
	this->pAppStation		= pAppStation;
	this->pModSelected		= pModSelected;

#ifdef SRCH_EMBEDDED_SENSOR //20170810
	bUseMtrSnrPort = FALSE;
	bEnableSrchEmbeddedHome = FALSE;
	bEnableSrchEmbeddedIndexer = FALSE;
	szMtrSnrPortName = "";
#endif
	InitDefaultMotorInfo();
}

// 20140825 Yip: Use Init Function Instead Of Constructor To Prevent Memory Leak When Assignment
VOID CMotorInfo::InitMotorInfo(CString szAxis, BOOL *pModSelected, BOOL bIsServo)
{
	InitDefaultMotorInfo();

	stAttrib.szAxis			= szAxis;
	this->pModSelected		= pModSelected;
	bIsServoMotor = bIsServo;
}

CMotorInfo::~CMotorInfo()
{
	// 20140819 Yip: Fix Memory Leak Issue
	if (stAttrib.stSensorMapping.cHomePortAddrss != NULL)
	{
		delete[] stAttrib.stSensorMapping.cHomePortAddrss;
	}
	if (stAttrib.stSensorMapping.cNegLmtPortAddrss != NULL)
	{
		delete[] stAttrib.stSensorMapping.cNegLmtPortAddrss;
	}
	if (stAttrib.stSensorMapping.cPosLmtPortAddrss != NULL)
	{
		delete[] stAttrib.stSensorMapping.cPosLmtPortAddrss;
	}
	if (stMotionProfile.cMotionCtrlType != NULL)
	{
		delete[] stMotionProfile.cMotionCtrlType;
	}
	if (stCtrlProf.cSettlingCtrlType != NULL)
	{
		delete[] stCtrlProf.cSettlingCtrlType;
	}
	if (stHomeProfile.cSrchCtrlType != NULL)
	{
		delete[] stHomeProfile.cSrchCtrlType;
	}
	if (stIndexProfile.cSrchCtrlType != NULL)
	{
		delete[] stIndexProfile.cSrchCtrlType;
	}
	if (stIndexProfile.cPort != NULL)
	{
		delete[] stIndexProfile.cPort;
	}
	for (INT i = 0; i < MAX_NUM_OF_SRCH_PROF; i++)
	{
		if (stUsrDefProfile[i].cPort != NULL)
		{
			delete[] stUsrDefProfile[i].cPort;
		}
	}
}

#ifdef SRCH_EMBEDDED_SENSOR //20170810
VOID CMotorInfo::SetMtrSnrPortName(CString &szInMtrSnrPortName, BOOL bEnableEmbeddedHome, BOOL bEnableEmbeddedIndexer)  //20170810
{
	szMtrSnrPortName = szInMtrSnrPortName;
	bUseMtrSnrPort = TRUE;
	bEnableSrchEmbeddedHome = bEnableEmbeddedHome;
	bEnableSrchEmbeddedIndexer = bEnableEmbeddedIndexer;
}
#endif

VOID CMotorInfo::SetGmpChannel(CGmpChannel *pGmpChannel)
{
	this->pGmpChannel		= pGmpChannel;
}

VOID CMotorInfo::SetAppStation(CAC9000App *pAppStation)
{
	this->pAppStation		= pAppStation;
}

VOID CMotorInfo::SetStnName(CString szStnName)
{
	this->szChStnName		= szStnName;
}

BOOL CMotorInfo::IsHardwareSelected()
{
	CAppStation *pStation		= NULL;

	if (
		pAppStation != NULL &&
		(pStation = dynamic_cast<CAppStation*>(pAppStation->GetStation(szChStnName))) != NULL
	   )
	{
		return pStation->IsHardware();
	}
	else
	{
		CString szMsg = "";
		if (pAppStation != NULL)
		{
			pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			pAppStation->SetError(lErrorCode);
			pAppStation->SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
			pAppStation->DisplayMessage("Fn: IsHardwareSelect()");
			szMsg.Format("Err: GetStation(%s)", szChStnName);
			pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		}
		return FALSE;
	}
}

VOID CMotorInfo::InitDefaultMotorInfo()
{
	szChStnName									= "Dummy_Station";
	bLogMotion									= FALSE;

	bHomeStatus									= FALSE;
	lErrorCode									= 128;
	nTCObjectID									= 0;

	lHMI_CurrPosn								= 0;
	bHMI_On										= OFF;

	stProtect.lPosLmt							= 999999999;
	stProtect.lNegLmt							= -999999999;

	stAttrib.szAxis								= "DEFAULT_MTR";
	stAttrib.dEncoderScale						= 2000.0;
	stAttrib.dDistPerCount						= 0.5;
	stAttrib.dDistPerCountAux					= 0.5;  //20140322

	stAttrib.stSensorMapping.cHomePortAddrss	= new char[IO_PORT_STRING_LENGTH];
	stAttrib.stSensorMapping.nHomeMask			= 0x1;
	stAttrib.stSensorMapping.cNegLmtPortAddrss	= new char[IO_PORT_STRING_LENGTH];
	stAttrib.stSensorMapping.nNegLmtMask		= 0x1;
	stAttrib.stSensorMapping.cPosLmtPortAddrss	= new char[IO_PORT_STRING_LENGTH];
	stAttrib.stSensorMapping.nPosLmtMask		= 0x1;

	stAttrib.sPositionLimit						= 2000;
	stAttrib.sDacLimit							= 8192;
	stAttrib.sTimeLimit							= 1000;

	stAttrib.bLmtSnrState						= ACTIVE_HIGH;
	stAttrib.bEncoderExist						= TRUE;
	stAttrib.bLmtSnrExist						= TRUE;

	stComm.enCommOption							= GMP_COMMUTATION_OPTION_FROM_SCF;
	stComm.sNumberOfPoles						= 2;
	stComm.ulCountPerRev						= 5000;
	stComm.nCommStatus							= GMP_FAIL;

	stComm.bCommDir								= POSITIVE_DIR;
	stComm.unSlowDownFactor						= 2;
	stComm.dAcceptance							= 0;
	stComm.dInitialDAC							= 2000;
	stComm.dIncreamentDAC						= 2000;
	stComm.dMaximumDAC							= 30000;
	stComm.dFitError							= NULL;

	stMotionProfile.sMotionProfID				= NORMAL_MOT_PROF;
	stMotionProfile.cMotionCtrlType				= new char[PROFILE_STRING_LENGTH];
	stMotionProfile.dVel						= 1000.0;
	stMotionProfile.dAcc						= 100.0;
	stMotionProfile.dDec						= 100.0;
	stMotionProfile.dJerk						= 10.0;

	stCtrlProf.cSettlingCtrlType				= new char[PROFILE_STRING_LENGTH];
	stCtrlProf.sSettliingTime					= 30;

	// Home Profile
	stHomeProfile.bEnable						= GMP_ENABLE;
	stHomeProfile.sSrchProfID					= HOME_SRCH_PROF;
	stHomeProfile.cSrchCtrlType					= new char[PROFILE_STRING_LENGTH];
	stHomeProfile.sSrchDirection				= POSITIVE_DIR;
	stHomeProfile.bSetPosn						= GMP_DISABLE;
	stHomeProfile.lDesiredPosn					= 0;
	stHomeProfile.enSrchType					= GMP_EVT_SRH_SENSOR; //GMP_EVT_SRH_HOME;
	stHomeProfile.cPort							= stAttrib.stSensorMapping.cHomePortAddrss;
	stHomeProfile.unSensorMask					= stAttrib.stSensorMapping.nHomeMask;
	stHomeProfile.bAstate						= ACTIVE_HIGH;
	stHomeProfile.enLimitType					= GMP_EVT_LMT_DIST;

	stHomeProfile.stProf.dSrchVel				= 10;
	stHomeProfile.stProf.dSrchLimit				= 1000;
	stHomeProfile.stProf.dDriveInVel			= 5;
	stHomeProfile.stProf.dDriveIn				= 500;
	stHomeProfile.stProf.sDebounce				= 2;

	// Index Profile
	stIndexProfile.bEnable						= GMP_ENABLE;
	stIndexProfile.sSrchProfID					= INDEX_SRCH_PROF;
	stIndexProfile.cSrchCtrlType				= new char[PROFILE_STRING_LENGTH];
	stIndexProfile.sSrchDirection				= !stHomeProfile.sSrchDirection;
	stIndexProfile.bSetPosn						= GMP_ENABLE;
	stIndexProfile.lDesiredPosn					= 0;
	stIndexProfile.bCheckError					= GMP_ENABLE;
	stIndexProfile.enSrchType					= GMP_EVT_SRH_INDEX;
	stIndexProfile.cPort						= new char[IO_PORT_STRING_LENGTH];
	stIndexProfile.unSensorMask					= 0x1;
	stIndexProfile.bAstate						= ACTIVE_HIGH;
	stIndexProfile.enLimitType					= GMP_EVT_LMT_DIST;

	stIndexProfile.stProf.dSrchVel				= 10;
	stIndexProfile.stProf.dSrchLimit			= 1000;
	stIndexProfile.stProf.dDriveInVel			= 5;
	stIndexProfile.stProf.dDriveIn				= 500;
	stIndexProfile.stProf.sDebounce				= 2;

	for (INT i = 0; i < MAX_NUM_OF_SRCH_PROF; i++)
	{
		stUsrDefProfile[i].bEnable				= GMP_DISABLE;
		stUsrDefProfile[i].sSrchProfID			= INDEX_SRCH_PROF + i;
		stUsrDefProfile[i].cSrchCtrlType		= "PL_Static";
		stUsrDefProfile[i].sSrchDirection		= POSITIVE_DIR;
		stUsrDefProfile[i].bSetPosn				= GMP_DISABLE;
		stUsrDefProfile[i].lDesiredPosn			= 0;
		stUsrDefProfile[i].bCheckError			= GMP_ENABLE;
		stUsrDefProfile[i].enSrchType			= GMP_EVT_SRH_PORT_AND;
		stUsrDefProfile[i].cPort				= new char[IO_PORT_STRING_LENGTH];
		stUsrDefProfile[i].unSensorMask			= 0x1;
		stUsrDefProfile[i].bAstate				= ACTIVE_HIGH;
		stUsrDefProfile[i].enLimitType			= GMP_EVT_LMT_DIST;

		stUsrDefProfile[i].stProf.dSrchVel		= 10;
		stUsrDefProfile[i].stProf.dSrchLimit	= 1000;
		stUsrDefProfile[i].stProf.dDriveInVel	= 0;
		stUsrDefProfile[i].stProf.dDriveIn		= 0;
		stUsrDefProfile[i].stProf.sDebounce		= 2;
	}
}

CString CMotorInfo::GetName()
{
	return stAttrib.szAxis;
}

CString CMotorInfo::GetChStnName()
{
	return szChStnName;
}

CString CMotorInfo::GetErrPortID()
{
	CString szResult = "HARDWARE_NOT_SELECTED";

	if (!IsHardwareSelected())
	{
		return szResult;
	}

	try
	{
		szResult = pGmpChannel->GetErrPortID();
	}
	catch (CAsmException e)
	{
		szResult = "HARDWARE_NOT_SELECTED";
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return szResult;
	}

	return szResult;
}

CString CMotorInfo::GetDACPortID()
{
	CString szResult = "HARDWARE_NOT_SELECTED";

	if (!IsHardwareSelected())
	{
		return szResult;
	}

	try
	{
		szResult = pGmpChannel->GetDACPortID();
	}
	catch (CAsmException e)
	{
		szResult = "HARDWARE_NOT_SELECTED";
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return szResult;
	}

	return szResult;
}

CString CMotorInfo::GetDrvPortID()
{
	CString szResult = "HARDWARE_NOT_SELECTED";

	if (!IsHardwareSelected())
	{
		return szResult;
	}

	try
	{
		szResult = pGmpChannel->GetDrvPortID();
	}
	catch (CAsmException e)
	{
		szResult = "HARDWARE_NOT_SELECTED";
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return szResult;
	}

	return szResult;
}

CString CMotorInfo::GetEncPortID()
{
	CString szResult = "HARDWARE_NOT_SELECTED";

	if (!IsHardwareSelected())
	{
		return szResult;
	}

	try
	{
		szResult = pGmpChannel->GetEncPortID();
	}
	catch (CAsmException e)
	{
		szResult = "HARDWARE_NOT_SELECTED";
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return szResult;
	}

	return szResult;
}

CString CMotorInfo::GetCmdPortID()
{
	CString szResult = "HARDWARE_NOT_SELECTED";

	if (!IsHardwareSelected())
	{
		return szResult;
	}

	try
	{
		szResult = pGmpChannel->GetCmdPortID();
	}
	catch (CAsmException e)
	{
		szResult = "HARDWARE_NOT_SELECTED";
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return szResult;
	}

	return szResult;
}

CString CMotorInfo::GetMotorSnsrPortID()
{
	CString szResult = "HARDWARE_NOT_SELECTED";

	if (!IsHardwareSelected())
	{
		return szResult;
	}

	try
	{
		szResult = pGmpChannel->GetMotorSnsrPortID();
	}
	catch (CAsmException e)
	{
		szResult = "HARDWARE_NOT_SELECTED";
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return szResult;
	}

	return szResult;
}

CString CMotorInfo::GetChModePortID()
{
	CString szResult = "HARDWARE_NOT_SELECTED";

	if (!IsHardwareSelected())
	{
		return szResult;
	}

	try
	{
		szResult = pGmpChannel->GetChModePortID();
	}
	catch (CAsmException e)
	{
		szResult = "HARDWARE_NOT_SELECTED";
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return szResult;
	}

	return szResult;
}

INT CMotorInfo::InitMtrAxis()
{
	INT nResult = GMP_SUCCESS;
	CString szMsg = _T("");
	INT i = 0;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	// Initi control parameters and motion profiles
	if (
		SelectCtrlPara() == GMP_SUCCESS &&
		SetMoveProf() == GMP_SUCCESS &&
		InitHomeProf() == GMP_SUCCESS &&
		SetSoftwareLmt() == GMP_SUCCESS
	   )
	{
		for (i = 0; i < MAX_NUM_OF_MOT_PROF; i++)
		{
			SetMoveInput(i);
		}

		// Initi Search Profile
		for (i = 0; i < MAX_NUM_OF_SRCH_PROF; i++)
		{
			if (stUsrDefProfile[i].bEnable)
			{
				if (InitSrchProf(stUsrDefProfile[i]) != GMP_SUCCESS)
				{
					nResult = GMP_FAIL;
					break;
				}
			}
		}
	}

	return nResult;
}

INT CMotorInfo::SelectCtrlPara()
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		pGmpChannel->SelectStCtrl("PL_Static");
		//pGmpChannel->SelectSettlingCtrl(stCtrlProf.cSettlingCtrlType, stCtrlProf.sSettliingTime);
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::SelectStaticCtrlPara(CString szStaticCtrlPara)
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		char cString[40]; //20120618
		memset(&cString[0], 0, sizeof(char) * 40);
		strcpy(&cString[0], szStaticCtrlPara.GetBuffer(0));
		pGmpChannel->SelectStCtrl(&cString[0]);
		//pGmpChannel->SelectStCtrl(szStaticCtrlPara);
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::SetMoveInput(short sMotionProfID, short sProcListSaveID)
{
	INT nResult = GMP_SUCCESS;

	try
	{
		pGmpChannel->SetMoveInput(sMotionProfID, 
								  stMotionProfile.cMotionCtrlType,
								  sProcListSaveID);
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::SetMoveProf()
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		pGmpChannel->SetMoveProfSi(NORMAL_MOT_PROF, 
								   stMotionProfile.dJerk, 
								   stMotionProfile.dAcc, 
								   stMotionProfile.dDec, 
								   stMotionProfile.dVel);

		pGmpChannel->SetMoveProfSi(SLOW_MOT_PROF, 
								   stMotionProfile.dJerk * 0.5, 
								   stMotionProfile.dAcc * 0.5, 
								   stMotionProfile.dDec * 0.5, 
								   stMotionProfile.dVel * 0.5);
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::SetCustMoveProf(DOUBLE dFactor) //20150223
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	if (dFactor <= 0.0)
	{
		return GMP_FAIL;
	}

	try
	{
		pGmpChannel->SetMoveProfSi(NORMAL_MOT_PROF, 
								   stMotionProfile.dJerk * dFactor,
								   stMotionProfile.dAcc * dFactor, 
								   stMotionProfile.dDec * dFactor, 
								   stMotionProfile.dVel * dFactor);

		pGmpChannel->SetMoveProfSi(SLOW_MOT_PROF, 
								   stMotionProfile.dJerk * 0.5 * dFactor, 
								   stMotionProfile.dAcc * 0.5 * dFactor, 
								   stMotionProfile.dDec * 0.5 * dFactor, 
								   stMotionProfile.dVel * 0.5 * dFactor);
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::SetCustMoveProf(DOUBLE dAccFactor, DOUBLE dDecFactor, DOUBLE dVelFactor, DOUBLE dJerkFactor) //20150223
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	if (dAccFactor <= 0.0 || dDecFactor <= 0.0 || dVelFactor <= 0.0 || dJerkFactor <= 0.0)
	{
		return GMP_FAIL;
	}

	try
	{
		pGmpChannel->SetMoveProfSi(NORMAL_MOT_PROF, 
								   stMotionProfile.dJerk * dJerkFactor,
								   stMotionProfile.dAcc * dAccFactor, 
								   stMotionProfile.dDec * dDecFactor, 
								   stMotionProfile.dVel * dVelFactor);

		pGmpChannel->SetMoveProfSi(SLOW_MOT_PROF, 
								   stMotionProfile.dJerk * 0.5 * dJerkFactor, 
								   stMotionProfile.dAcc * 0.5 * dAccFactor, 
								   stMotionProfile.dDec * 0.5 * dDecFactor, 
								   stMotionProfile.dVel * 0.5 * dVelFactor);
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::SetSoftwareLmt()
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		pGmpChannel->SetSoftwareLimitPosn(stMotionProfile.sMotionProfID, stProtect.lPosLmt, stProtect.lNegLmt);
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::SetPosition(LONG lPosn)
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		pGmpChannel->SetPosition(lPosn);
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::InitHomeProf()
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		// Set Srch Home Para
		if (stHomeProfile.bEnable)
		{
			pGmpChannel->SetSrchInput(stHomeProfile.sSrchProfID, stHomeProfile.cSrchCtrlType, GMP_TRUE); 

			pGmpChannel->InitSrchProf(stHomeProfile.sSrchProfID, stHomeProfile.enSrchType, 
									  stHomeProfile.cPort, stHomeProfile.unSensorMask,
									  stHomeProfile.bAstate, stHomeProfile.enLimitType, 
									  stHomeProfile.stProf.sDebounce, stHomeProfile.bCheckError);

			pGmpChannel->SetSrchProfSi(stHomeProfile.sSrchProfID, stHomeProfile.stProf.dSrchVel,
									   stHomeProfile.stProf.dSrchLimit, stHomeProfile.stProf.dDriveInVel,
									   stHomeProfile.stProf.dDriveIn);

#ifdef SRCH_EMBEDDED_SENSOR //20170810
			pGmpChannel->SetSrchEmbeddedSnrInput(SRCH_HOME, stHomeProfile.cSrchCtrlType, GMP_TRUE); 

			pGmpChannel->InitSrchEmbeddedSnr(SRCH_HOME, stHomeProfile.stProf.sDebounce, stHomeProfile.bCheckError);

			pGmpChannel->SetSrchEmbeddedSnrProfSi(SRCH_HOME, stHomeProfile.stProf.dSrchVel,
									   stHomeProfile.stProf.dSrchLimit, stHomeProfile.stProf.dDriveInVel,
									   stHomeProfile.stProf.dDriveIn);
#endif
		}

		if (stIndexProfile.bEnable)
		{
			// Set Srch Indexor Para
			pGmpChannel->SetSrchInput(stIndexProfile.sSrchProfID, stIndexProfile.cSrchCtrlType, GMP_TRUE); 

			pGmpChannel->InitSrchProf(stIndexProfile.sSrchProfID, stIndexProfile.enSrchType, 
									  stIndexProfile.cPort, stIndexProfile.unSensorMask, 
									  stIndexProfile.bAstate, stIndexProfile.enLimitType, 
									  stIndexProfile.stProf.sDebounce, stIndexProfile.bCheckError);

			pGmpChannel->SetSrchProfSi(stIndexProfile.sSrchProfID, stIndexProfile.stProf.dSrchVel,
									   stIndexProfile.stProf.dSrchLimit, stIndexProfile.stProf.dDriveInVel,
									   stIndexProfile.stProf.dDriveIn);
#ifdef SRCH_EMBEDDED_SENSOR //20170810
			pGmpChannel->SetSrchEmbeddedSnrInput(SRCH_INDEX, stIndexProfile.cSrchCtrlType, GMP_TRUE); 

			pGmpChannel->InitSrchEmbeddedSnr(SRCH_INDEX, stIndexProfile.stProf.sDebounce, stIndexProfile.bCheckError);

			pGmpChannel->SetSrchEmbeddedSnrProfSi(SRCH_INDEX, stIndexProfile.stProf.dSrchVel,
									   stIndexProfile.stProf.dSrchLimit, stIndexProfile.stProf.dDriveInVel,
									   stIndexProfile.stProf.dDriveIn);
#endif
		}
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::InitSrchProf(SRCH_PROFILE stSrchProf)
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		pGmpChannel->SetSrchInput(stSrchProf.sSrchProfID, stSrchProf.cSrchCtrlType, stSrchProf.bSetPosn); 

		pGmpChannel->InitSrchProf(stSrchProf.sSrchProfID, stSrchProf.enSrchType, 
								  stSrchProf.cPort, stSrchProf.unSensorMask, 
								  stSrchProf.bAstate, stSrchProf.enLimitType, 
								  stSrchProf.stProf.sDebounce, stSrchProf.bCheckError);

		pGmpChannel->SetSrchProfSi(stSrchProf.sSrchProfID, stSrchProf.stProf.dSrchVel,
								   stSrchProf.stProf.dSrchLimit, stSrchProf.stProf.dDriveInVel,
								   stSrchProf.stProf.dDriveIn);
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::SetSrchProfSi(SHORT	sSrchProfID, SRCH_PROF_PARA stSrchPara)
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		stUsrDefProfile[sSrchProfID].stProf.dSrchVel		= stSrchPara.dSrchVel;
		stUsrDefProfile[sSrchProfID].stProf.dSrchLimit		= stSrchPara.dSrchLimit;
		stUsrDefProfile[sSrchProfID].stProf.dDriveInVel		= stSrchPara.dDriveInVel;
		stUsrDefProfile[sSrchProfID].stProf.dDriveIn		= stSrchPara.dDriveIn;

		pGmpChannel->SetSrchProfSi(sSrchProfID, stUsrDefProfile[sSrchProfID].stProf.dSrchVel,
								   stUsrDefProfile[sSrchProfID].stProf.dSrchLimit, stUsrDefProfile[sSrchProfID].stProf.dDriveInVel,
								   stUsrDefProfile[sSrchProfID].stProf.dDriveIn);
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::SetSrchInput(short sMotionProfID, short sProcListSaveID)
{
	INT nResult = GMP_SUCCESS;

	try
	{
		pGmpChannel->SetSrchInput(sMotionProfID,
								  "PL_Static",
								  FALSE,
								  sProcListSaveID); 
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::SetProcessBlk(short sProcListSaveID, unsigned short usBlkNum, unsigned short usNextBlk, 
							  GMP_LOGIC enLogic, 
							  const char *szInputPort, unsigned int unInputPattern, unsigned int unInputMask,
							  const char *szOutputPort, unsigned int unOutputPattern, unsigned int unOutputMask)
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		pGmpChannel->SetProcessProf_EventTrig(sProcListSaveID, usBlkNum, usNextBlk,
											  enLogic,
											  (char*)szInputPort, unInputPattern, unInputMask, 
											  (char*)szOutputPort, unOutputPattern, unOutputMask);
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::SetProcessList(short sProcListSaveID, byte ucSize, unsigned short *p_usStartProcBlkSaveIDList)
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		pGmpChannel->SetProcessList(sProcListSaveID, ucSize, p_usStartProcBlkSaveIDList);
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::EnableProtection()
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		if (stAttrib.bEncoderExist)
		{
			if (
				GetName() == "IN_PICK_ARM_X" || GetName() == "IN_PICK_ARM_Z" ||
				GetName() == "TA1_X" || GetName() == "TA1_Y" || GetName() == "TA1_Z" ||
				GetName() == "TA2_X" || GetName() == "TA2_Z" || GetName() == "INSPOPT_X" ||
				GetName() == "ACF1WH_Y" || GetName() == "ACF2WH_Y" ||
				GetName() == "ACF1WH_T" || GetName() == "ACF2WH_T" ||
				GetName() == "ACF1_Z" || GetName() == "ACF1_X" || GetName() == "ACF1_STRIPPER_X"
			   )
			{
				pGmpChannel->EnableProtection(GMP_PROT_TYPE_SW_LMT, ACTIVE_HIGH);
			}
			//pGmpChannel->EnableProtection(GMP_PROT_TYPE_SW_LMT, ACTIVE_HIGH);
			pGmpChannel->EnableProtection(GMP_PROT_TYPE_ENCODER_FAULT, ACTIVE_HIGH);
		}

		if (stAttrib.bLmtSnrExist)
		{
			pGmpChannel->EnableProtection(GMP_PROT_TYPE_LMT_SENSOR, (GMP_TRIGGER_STATE)stAttrib.bLmtSnrState);
		}
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::EnableProtection(GMP_PROT_TYPE enProtectionType, BOOL bTriggerState)
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		pGmpChannel->EnableProtection(enProtectionType, (GMP_TRIGGER_STATE)bTriggerState);
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::DisableProtection(GMP_PROT_TYPE enProtectionType)
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		pGmpChannel->DisableProtection(enProtectionType);
	}
	catch (CAsmException e)
	{
		nResult = GMP_FAIL;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

//INT CMotorInfo::PowerOn(BOOL bMode)
INT CMotorInfo::PowerOn(BOOL bMode, BOOL bCheckAlreadyCommHome) //20130418
{
	INT nResult = GMP_SUCCESS;

	//CString szMsg = "";
	//szMsg.Format("%s\tPowerON\t%ld", stAttrib.szAxis, bMode);
	//LogAction(szMsg);

	CString szErrMsg = "";
	CString szDrvMsg = "";

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		pGmpChannel->ClearAllError();
		pGmpChannel->ClearDriverError();

		if (stComm.nCommStatus != GMP_SUCCESS)
		{
			Commutate();
		}

		if (bMode)
		{
			BOOL bIsOn = TRUE;
			//if (bCheckAlreadyCommHome && bIsServoMotor && stComm.nCommStatus != GMP_SUCCESS) //20130418
			//{
			//	CString szTitle = _T(" "), szBoxMsg = _T(" ");
			//	szTitle.Format("%s", stAttrib.szAxis);
			//	szBoxMsg.Format("%s has not been Comm yet! Motor Power Off", stAttrib.szAxis);
			//	pAppStation->HMIMessageBox(MSG_OK, szTitle, szBoxMsg);
			//	bIsOn = FALSE;
			//}
			//if (bCheckAlreadyCommHome && bIsServoMotor && !bHomeStatus) //20130418
			//{
			//	CString szTitle = _T(" "), szBoxMsg = _T(" ");
			//	szTitle.Format("%s", stAttrib.szAxis);
			//	szBoxMsg.Format("%s has not been homed yet! Motor Power Off", stAttrib.szAxis);
			//	pAppStation->HMIMessageBox(MSG_OK, szTitle, szBoxMsg);
			//	bIsOn = FALSE;
			//}

			if (bIsOn) //20130418
			{
				pGmpChannel->ClearAllError(); //20150918
				pGmpChannel->PowerOn();
			}
			else
			{
				pGmpChannel->ClearAllError(); //20150918
				pGmpChannel->PowerOff();
				nResult = GMP_FAIL;
			}
		}
		else
		{
			pGmpChannel->PowerOff();
		}
	}
	catch (CAsmException e)
	{
		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

BOOL CMotorInfo::IsPowerOn()
{
	INT bResult = GMP_ENABLE;

	if (!IsHardwareSelected())
	{
		return bResult;
	}

	try
	{
		bResult = pGmpChannel->IsPowerOn();
	}
	catch (CAsmException e)
	{
		bResult = GMP_DISABLE;
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

INT CMotorInfo::Commutate(GMP_COMMUTATION_OPTION enCommMode)
{
	INT nResult = GMP_SUCCESS;

	GMP_COMMUTATION_OPTION enCommOption = enCommMode;

	CString szErrMsg = "";
	CString szDrvMsg = "";

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	DisableProtection(GMP_PROT_TYPE_SW_LMT);

	pGmpChannel->ClearAllError();
	pGmpChannel->ClearDriverError();

	if (enCommMode == GMP_COMMUTATION_OPTION_NONE)
	{
		enCommOption = stComm.enCommOption;
	}

	stComm.nCommStatus	= GMP_FAIL;
	bHomeStatus			= GMP_FAIL;
	//bHomeStatus			= FALSE; //20150314

	try
	{
		switch (enCommOption)
		{
		case GMP_COMMUTATION_OPTION_FROM_SCF:		///< The commutation method selected in SCF will be used
			pGmpChannel->Commutate();
			break;

		case GMP_COMMUTATION_OPTION_STATIONARY:		///< A typical setting of open stationary commutation: Slow down factor = 1; Init DAC = 2000; Increment DAC = 1000; Max DAC = 30000
			pGmpChannel->StationaryComm();
			break;

		case GMP_COMMUTATION_OPTION_OPEN_STATIONARY:	///< Open stationary commutation
			pGmpChannel->OpenStationaryComm(stComm.unSlowDownFactor,
											stComm.dAcceptance,
											stComm.dInitialDAC,
											stComm.dIncreamentDAC,
											stComm.dMaximumDAC);
			break;

		case GMP_COMMUTATION_OPTION_DIRECTIONAL:		///< Directional commutation
			pGmpChannel->DirectionalComm(stComm.bCommDir);
			break;
		}
	}
	catch (CAsmException e)
	{
		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return nResult;
	}

	stComm.nCommStatus = GMP_SUCCESS;
	return nResult;
}

INT CMotorInfo::ClearError()
{
	INT nResult = GMP_SUCCESS;

	CString szErrMsg = "";
	CString szDrvMsg = "";

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		pGmpChannel->ClearAllError();
	}
	catch (CAsmException e)
	{
		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::Sync()
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

//#ifdef PB1_NO_FPC_REJECT_BIN
//	if (GetName() == "PB1_FPC_REJECT_X")
//	{
//		return GMP_SUCCESS;
//	}
//#endif

	CString szErrMsg = "";
	CString szDrvMsg = "";

	try
	{
		pGmpChannel->Synchronize();

		pAppStation->TC_DelJob(nTCObjectID);
	}
	catch (CAsmException e)
	{
		*pModSelected = FALSE;

		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::SetTCState(BOOL isOn)
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	CString szErrMsg = "";
	CString szDrvMsg = "";

	try
	{
		if (isOn)
		{
			pAppStation->TC_AddJob(nTCObjectID);
		}
		else
		{
			pAppStation->TC_DelJob(nTCObjectID);
		}
	}
	catch (CAsmException e)
	{
		*pModSelected = FALSE;

		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

BOOL CMotorInfo::IsMotionCompleted(BOOL bIsForceTCLog) //20120901
{
	BOOL bResult = TRUE;
	INT	nResult = GMP_SUCCESS;
	static BOOL bIsPreviousComplete = TRUE; //20120901

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	CString szErrMsg = "";
	CString szDrvMsg = "";

	try
	{
		bResult = pGmpChannel->IsComplete();

		if (bResult && (!bIsPreviousComplete || bIsForceTCLog))
		{
			pAppStation->TC_DelJob(nTCObjectID);
		}
		bIsPreviousComplete = bResult;
	}
	catch (CAsmException e)
	{
		*pModSelected = FALSE;

		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

INT CMotorInfo::MoveRelative(LONG lMtrCnt, BOOL bWait , BOOL bCheckModSelect)
{
	INT		nResult		= GMP_SUCCESS;

	CString szResult	= _T("");
	CString szMsg		= _T("");
	CString szErrMsg	= "";
	CString szDrvMsg	= "";

	szMsg.Format("%s\tMoveRelative\t%ld", stAttrib.szAxis, lMtrCnt);

//#ifdef PB1_NO_FPC_REJECT_BIN
//	if (GetName() == "PB1_FPC_REJECT_X")
//	{
//		return GMP_SUCCESS;
//	}
//#endif

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	//Milton for debug 2014/02/06
	if (!*pModSelected && bCheckModSelect)
	{
		szMsg.Format("*** ERROR: %s Move Relative Error (Module Not Selected) ***", stAttrib.szAxis);
		pAppStation->DisplayMessage(szMsg);
		return GMP_FAIL;	// 20140618 Yip: Change From GMP_SUCCESS To GMP_FAIL
	}

	if (!bHomeStatus)
	{
		pAppStation->SetError(lErrorCode);
		pAppStation->SetError(IDS_MOTOR_NOT_HOME);
		return GMP_FAIL;
	}

	try
	{
		pAppStation->TC_AddJob(nTCObjectID);
		//LogAction(szMsg);
		pGmpChannel->MoveRelative(lMtrCnt, stMotionProfile.sMotionProfID);

		if (bWait == GMP_WAIT)
		{
			pGmpChannel->Synchronize();
			pAppStation->TC_DelJob(nTCObjectID);
		}
		else
		{
			nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);

			if (nResult != GMP_SUCCESS)
			{
				*pModSelected = FALSE;

				pAppStation->SetError(lErrorCode);
//				pAppStation->SetError(nResult);
				szMsg.Format("*** ERROR: %s Move Relative Error ***", stAttrib.szAxis);
				//LogAction(szMsg);
				pAppStation->DisplayMessage(szMsg);
			}
		}
	}
	catch (CAsmException e)
	{
		*pModSelected = FALSE;

		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::MoveAbsolute(LONG lMtrCnt, BOOL bWait)
{
	INT		nResult		= GMP_SUCCESS;
	CString szResult	= _T("");
	CString szMsg		= _T("");
	CString szErrMsg	= "";
	CString szDrvMsg	= "";

	szMsg.Format("%s\tMoveAbsolute\t%ld", stAttrib.szAxis, lMtrCnt);

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	if (!*pModSelected)
	{
		szMsg.Format("*** ERROR: %s Move Absolute Error (Module Not Selected) ***", stAttrib.szAxis);
		pAppStation->DisplayMessage(szMsg);
		return GMP_FAIL;	// 20140618 Yip: Change From GMP_SUCCESS To GMP_FAIL
	}

	if (!bHomeStatus)
	{
		pAppStation->SetError(lErrorCode);
		pAppStation->SetError(IDS_MOTOR_NOT_HOME);
		return GMP_FAIL;
	}
	try
	{
		pAppStation->TC_AddJob(nTCObjectID);
		//LogAction(szMsg);
		pGmpChannel->MoveAbsolute(lMtrCnt, stMotionProfile.sMotionProfID);

		if (bWait == GMP_WAIT)
		{
			pGmpChannel->Synchronize();
			pAppStation->TC_DelJob(nTCObjectID);
		}
		else
		{
			nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);

			if (nResult != GMP_SUCCESS)
			{
				*pModSelected = FALSE;

				pAppStation->SetError(lErrorCode);
//				pAppStation->SetError(nResult);
				szMsg.Format("*** ERROR: %s Move Absolute Error ***", stAttrib.szAxis);
				//LogAction(szMsg);
				pAppStation->DisplayMessage(szMsg);
			}
		}


	}
	catch (CAsmException e)
	{
		*pModSelected = FALSE;

		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		if (e.ErrorID() == MTR_SW_POS_LIMIT_HIT)
		{
			pAppStation->SetError(IDS_SW_POS_LIMIT_HIT);
		}
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		if (e.ErrorID() == MTR_SW_POS_LIMIT_HIT) //20130913
		{
			nResult = GMP_FAIL; //seems that when sw limit hit. The nResult is GMP_SUCCESS.
		}
	}

	return nResult;
}

INT CMotorInfo::OpenDAC(LONG lHoldDAC, LONG lEndDAC, LONG lHoldTime, LONG lRampUpTime, BOOL bWait)
{
	INT		nResult		= GMP_SUCCESS;
	CString szResult	= _T("");
	CString szMsg		= _T("");
	CString szErrMsg	= "";
	CString szDrvMsg	= "";
	OPENDAC_PROFILE dacProf;

	szMsg.Format("%s\tOpenDAC\t%ld", stAttrib.szAxis, lEndDAC);

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	//if (!*pModSelected)
	//{
	//	szMsg.Format("*** ERROR: %s Open DAC Error (Module Not Selected) ***", stAttrib.szAxis);
	//	pAppStation->DisplayMessage(szMsg);
	//	return GMP_FAIL;	// 20140618 Yip: Change From GMP_SUCCESS To GMP_FAIL
	//}

	//if (!bHomeStatus)
	//{
	//	pAppStation->SetError(lErrorCode);
	//	pAppStation->SetError(IDS_MOTOR_NOT_HOME);
	//	return GMP_FAIL;
	//}
	try
	{
		dacProf.nRampTime = lRampUpTime;
		dacProf.nHoldTime = lHoldTime;
		dacProf.nHoldValue = lHoldDAC;
		dacProf.nEndValue = lEndDAC;
		pAppStation->TC_AddJob(nTCObjectID);
		//LogAction(szMsg);
		pGmpChannel->OpenDAC(&dacProf, stMotionProfile.sMotionProfID);

		if (bWait == GMP_WAIT)
		{
			pGmpChannel->Synchronize();
			pAppStation->TC_DelJob(nTCObjectID);
		}
		else
		{
			nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);

			if (nResult != GMP_SUCCESS)
			{
				*pModSelected = FALSE;

				pAppStation->SetError(lErrorCode);
//				pAppStation->SetError(nResult);
				szMsg.Format("*** ERROR: %s Open DAC Error ***", stAttrib.szAxis);
				//LogAction(szMsg);
				pAppStation->DisplayMessage(szMsg);
			}
		}


	}
	catch (CAsmException e)
	{
		*pModSelected = FALSE;

		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		if (e.ErrorID() == MTR_SW_POS_LIMIT_HIT)
		{
			pAppStation->SetError(IDS_SW_POS_LIMIT_HIT);
		}
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		if (e.ErrorID() == MTR_SW_POS_LIMIT_HIT) //20130913
		{
			nResult = GMP_FAIL; //seems that when sw limit hit. The nResult is GMP_SUCCESS.
		}
	}

	return nResult;
}

INT CMotorInfo::Search(SRCH_PROFILE stSrchProf, short sProcListSaveID, BOOL bWait, BOOL bIsCheckHomed)
{
	INT		nResult		= GMP_SUCCESS;
	CString szResult	= _T("");
	CString szMsg		= _T("");
	CString szErrMsg	= "";
	CString szDrvMsg	= "";

	if (stSrchProf.sSrchProfID == HOME_SRCH_PROF)
	{
		szMsg.Format("%s\tSearch\tHOME", stAttrib.szAxis);
	}
	else if (stSrchProf.sSrchProfID == INDEX_SRCH_PROF)
	{
		szMsg.Format("%s\tSearch\tINDEXOR", stAttrib.szAxis);
	}

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	if (!*pModSelected)
	{
		szMsg.Format("*** ERROR: %s Search Error (Module Not Selected) ***", stAttrib.szAxis);
		pAppStation->DisplayMessage(szMsg);
		return GMP_FAIL;
	}

	try
	{
		pAppStation->TC_AddJob(nTCObjectID);
		//LogAction(szMsg);
		pGmpChannel->Search(stSrchProf.sSrchProfID, (byte)stSrchProf.sSrchDirection, sProcListSaveID);

		if (bWait == GMP_WAIT)
		{
			pGmpChannel->Synchronize();
			pAppStation->TC_DelJob(nTCObjectID);
		}
		else
		{
			if (stSrchProf.bCheckError)
			{
				nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);	
			}

			if (nResult != GMP_SUCCESS)
			{
				*pModSelected = FALSE;

				pAppStation->SetError(lErrorCode);
//				pAppStation->SetError(nResult);
				szMsg.Format("*** ERROR: %s Srch Error ***", stAttrib.szAxis);
				//LogAction(szMsg);
				pAppStation->DisplayMessage(szMsg);
			}
		}
	}
	catch (CAsmException e)
	{
		*pModSelected = FALSE;
		nResult = GMP_FAIL;

		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		PowerOn(OFF);

		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

#ifdef SRCH_EMBEDDED_SENSOR //20170810
INT CMotorInfo::SearchEmbededSnr(EMBEDED_SRCH_ENUM emSrchSnr, SRCH_PROFILE stSrchProf, BOOL bWait)
{
	INT		nResult		= GMP_SUCCESS;
	CString szResult	= _T("");
	CString szMsg		= _T("");
	CString szErrMsg	= "";
	CString szDrvMsg	= "";

	if (emSrchSnr == SRCH_HOME)
	{
		szMsg.Format("%s\tSearch embeded HOME", stAttrib.szAxis);
	}
	else if (emSrchSnr == SRCH_LMT)
	{
		szMsg.Format("%s\tSearch embeded Limit", stAttrib.szAxis);
	}
	else if (emSrchSnr == SRCH_INDEX)
	{
		szMsg.Format("%s\tSearch embeded INDEXER", stAttrib.szAxis);
	}

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	if (!*pModSelected)
	{
		szMsg.Format("*** ERROR: %s Search Error (Module Not Selected) ***", stAttrib.szAxis);
		pAppStation->DisplayMessage(szMsg);
		return GMP_FAIL;
	}

	try
	{
		pAppStation->TC_AddJob(nTCObjectID);
		//LogAction(szMsg);
		pGmpChannel->SearchEmbeddedSnr(emSrchSnr, (byte)stSrchProf.sSrchDirection);

		if (bWait == GMP_WAIT)
		{
			pGmpChannel->Synchronize();
			pAppStation->TC_DelJob(nTCObjectID);
		}
		else
		{
			if (stSrchProf.bCheckError)
			{
				nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);	
			}

			if (nResult != GMP_SUCCESS)
			{
				*pModSelected = FALSE;

				pAppStation->SetError(lErrorCode);
//				pAppStation->SetError(nResult);
				szMsg.Format("*** ERROR: %s Srch Error ***", stAttrib.szAxis);
				//LogAction(szMsg);
				pAppStation->DisplayMessage(szMsg);
			}
		}
	}
	catch (CAsmException e)
	{
		*pModSelected = FALSE;
		nResult = GMP_FAIL;

		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		PowerOn(OFF);

		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}
#endif

#if 1 //20120927
INT CMotorInfo::GetIOPortValue(CString szPortName)
{
	INT		nResult		= GMP_SUCCESS;
	CString szResult	= _T("");
	CString szErrMsg	= "";
	CString szDrvMsg	= "";

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		//if (stAttrib.bEncoderExist)
		//{
		nResult = pGmpChannel->GetIOPortValue(szPortName);
		//}
		//else
		//{
		//	nResult = 0;
		//}
	}
	catch (CAsmException e)
	{
		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}
#endif

INT CMotorInfo::GetEncPosn()
{
	INT		nResult		= GMP_SUCCESS;
	CString szResult	= _T("");
	CString szErrMsg	= "";
	CString szDrvMsg	= "";

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		if (stAttrib.bEncoderExist)
		{
			nResult = pGmpChannel->GetEncPosn();
		}
		else
		{
			nResult = 0;
		}
	}
	catch (CAsmException e)
	{
		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}
#ifdef SRCH_EMBEDDED_SENSOR //20170810
INT CMotorInfo::GetHomeSnrState(GMP_BOOLEAN *pbState) //20170810
{
	INT		nResult		= GMP_SUCCESS;
	CString szResult	= _T("");
	CString szErrMsg	= "";
	CString szDrvMsg	= "";

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	if (!bUseMtrSnrPort)
	{
		return GMP_FAIL;
	}
	try
	{
		nResult = pGmpChannel->GetHomeSnrStateWithPortName((char *)szMtrSnrPortName.GetBuffer(0), pbState);
	}
	catch (CAsmException e)
	{
		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		nResult = GMP_FAIL;
	}

	return nResult;
}

INT CMotorInfo::GetLmtSnrState(GMP_BOOLEAN *pbState) //20170810
{
	INT		nResult		= GMP_SUCCESS;
	CString szResult	= _T("");
	CString szErrMsg	= "";
	CString szDrvMsg	= "";

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	if (!bUseMtrSnrPort)
	{
		return GMP_FAIL;
	}
	try
	{
		nResult = pGmpChannel->GetLmtSnrStateWithPortName((char *)szMtrSnrPortName.GetBuffer(0), pbState);
	}
	catch (CAsmException e)
	{
		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		nResult = GMP_FAIL;
	}

	return nResult;
}

INT CMotorInfo::GetIndexSnrState(GMP_BOOLEAN *pbState) //20170810
{
	INT		nResult		= GMP_SUCCESS;
	CString szResult	= _T("");
	CString szErrMsg	= "";
	CString szDrvMsg	= "";

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	if (!bUseMtrSnrPort)
	{
		return GMP_FAIL;
	}
	try
	{
		nResult = pGmpChannel->GetIndexSnrStateWithPortName((char *)szMtrSnrPortName.GetBuffer(0), pbState);
	}
	catch (CAsmException e)
	{
		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		nResult = GMP_FAIL;
	}

	return nResult;
}

#endif

INT CMotorInfo::GetCmdPosn()
{
	INT		nResult		= GMP_SUCCESS;
	CString szResult	= _T("");
	CString szErrMsg	= "";
	CString szDrvMsg	= "";

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		nResult = pGmpChannel->GetCmdPosn();
	}
	catch (CAsmException e)
	{
		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::Home(BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	BOOL bTempStatus = *pModSelected;

	bHomeStatus = FALSE;

	DisableProtection(GMP_PROT_TYPE_LMT_SENSOR);
	DisableProtection(GMP_PROT_TYPE_SW_LMT);

	if (!IsPowerOn())
	{

			//if ((nResult = PowerOn(GMP_ENABLE)) != GMP_SUCCESS)
		if ((nResult = PowerOn(ON, FALSE)) != GMP_SUCCESS) //20130418
		{
			return nResult;
		}
	}

	*pModSelected = TRUE;

	InitHomeProf();

	if (stHomeProfile.bEnable)
	{
		//pAppStation->DisplayMessage("Home Seq 1");
		if (
#ifdef SRCH_EMBEDDED_SENSOR //20170810
			((!bUseMtrSnrPort || (bUseMtrSnrPort && !bEnableSrchEmbeddedHome)) && ((nResult = Search(stHomeProfile, NO_PROCESS_LIST, TRUE)) != GMP_SUCCESS)) ||
			(bUseMtrSnrPort && bEnableSrchEmbeddedHome && ((nResult = SearchEmbededSnr(SRCH_HOME, stHomeProfile, TRUE)) != GMP_SUCCESS))
#else
			(nResult = Search(stHomeProfile, NO_PROCESS_LIST, TRUE)) != GMP_SUCCESS
#endif
			)
		{
			return nResult;
		}
		else if (!stHomeProfile.bCheckError)
		{
				  //if ((nResult = PowerOn(GMP_ENABLE)) != GMP_SUCCESS)
			if ((nResult = PowerOn(GMP_ENABLE, FALSE)) != GMP_SUCCESS) //20130418
			{
				return nResult;
			}
		}
		Sleep(1000);
#if 1 //20140115
		stHomeProfile.bAstate = !stHomeProfile.bAstate;
		if (!bUseMtrSnrPort || (bUseMtrSnrPort && !bEnableSrchEmbeddedHome))
		{
			if (stHomeProfile.sSrchDirection == POSITIVE_DIR)
			{
				stHomeProfile.sSrchDirection = NEGATIVE_DIR;
			}
			else
			{
				stHomeProfile.sSrchDirection = POSITIVE_DIR;
			}
		}

		InitSrchProf(stHomeProfile);

		//pAppStation->DisplayMessage("Home Seq 2");

#ifdef SRCH_EMBEDDED_SENSOR //20170810
		if (
			((!bUseMtrSnrPort || (bUseMtrSnrPort && !bEnableSrchEmbeddedHome)) && ((nResult = Search(stHomeProfile, NO_PROCESS_LIST, TRUE)) != GMP_SUCCESS)) ||
			(bUseMtrSnrPort && bEnableSrchEmbeddedHome && ((nResult = SearchEmbededSnr(SRCH_HOME, stHomeProfile, TRUE)) != GMP_SUCCESS))
			)
#else
		if ((nResult = Search(stHomeProfile, NO_PROCESS_LIST, TRUE)) != GMP_SUCCESS)
#endif
		{
			return nResult;
		}
		else if (!stHomeProfile.bCheckError)
		{
				  //if ((nResult = PowerOn(GMP_ENABLE)) != GMP_SUCCESS)
			if ((nResult = PowerOn(GMP_ENABLE, FALSE)) != GMP_SUCCESS) //20130418
			{
				return nResult;
			}
		}

//restore
		stHomeProfile.bAstate = !stHomeProfile.bAstate;
		if (!bUseMtrSnrPort || (bUseMtrSnrPort && !bEnableSrchEmbeddedHome))
		{
			if (stHomeProfile.sSrchDirection == POSITIVE_DIR)
			{
				stHomeProfile.sSrchDirection = NEGATIVE_DIR;
			}
			else
			{
				stHomeProfile.sSrchDirection = POSITIVE_DIR;
			}
		}
		InitHomeProf();
			//InitSrchProf(stHomeProfile);
			//if ((nResult = Search(stInvertHomeProfile, NO_PROCESS_LIST, TRUE)) != GMP_SUCCESS)
			//{
			//    return nResult;
			//}
			//else if (!stInvertHomeProfile.bCheckError)
			//{
			//    //if ((nResult = PowerOn(GMP_ENABLE)) != GMP_SUCCESS)
			//    if ((nResult = PowerOn(GMP_ENABLE, FALSE)) != GMP_SUCCESS) //20130418
			//    {
			//          return nResult;
			//    }
			//}
		Sleep(1000);
#endif
	}

	if (stIndexProfile.bEnable)
	{
#ifdef SRCH_EMBEDDED_SENSOR //20170810
		if (!bUseMtrSnrPort || (bUseMtrSnrPort && !bEnableSrchEmbeddedIndexer))
		{
			nResult = Search(stIndexProfile, NO_PROCESS_LIST, bWait);
		}
		else
		{
			nResult = SearchEmbededSnr(SRCH_INDEX, stIndexProfile, bWait);
		}
#else
		nResult = Search(stIndexProfile, NO_PROCESS_LIST, bWait);
#endif
	}

	if (nResult == GMP_SUCCESS && bWait)
	{
		bHomeStatus       = TRUE;
		EnableProtection();
	}

	if (nResult == GMP_SUCCESS)
	{
		*pModSelected     = bTempStatus;
	}
	else
	{
		*pModSelected = FALSE;
		PowerOn(OFF);
	}

	return nResult;
}


INT CMotorInfo::MTRTest()
{
	INT nResult = GMP_SUCCESS;

	INT nTestProcedure = 0;
	LONG lPosn = 0;
	LONG lStartPosn = 0;

	LogAction("%s MOTOR TEST", GetName());
	LogAction("___________________________________________________");
	LogAction("TEST\tRESULT\tERROR");

	do
	{
		switch (nTestProcedure)
		{
			// Commutation Test
		case 0:
			if ((nResult = Commutate()) == GMP_SUCCESS)
			{
				LogAction("Commutation Test\tSUCCESS");
			}
			else
			{
				LogAction("Commutation Test\tFAILED\t%d", nResult);
			}
			break;

			// Motor ON /Status Test
		case 1:
			if ((nResult = PowerOn(ON)) == GMP_SUCCESS)
			{
				LogAction("Power ON Test\tSUCCESS");

				if (IsPowerOn())
				{
					LogAction("Motor Status Test\tSUCCESS");
				}
				else
				{
					nResult = GMP_FAIL;
					LogAction("Motor Status Test\tFAILED\tDetect ON Fail");
				}
			}
			else
			{
				nResult = GMP_FAIL;
				LogAction("Power ON Test\tFAILED\t%d", nResult);
			}
			break;

			// Motor OFF /Status Test
		case 2:
			if ((nResult = PowerOn(OFF)) == GMP_SUCCESS)
			{
				LogAction("Power OFF Test\tSUCCESS");

				if (!IsPowerOn())
				{
					LogAction("Motor Status Test\tSUCCESS");
				}
				else
				{
					nResult = GMP_FAIL;
					LogAction("Motor Status Test\tFAILED\tDetect OFF Fail");
				}
			}
			else
			{
				nResult = GMP_FAIL;
				LogAction("Power OFF Test\tFAILED\t%d", nResult);
			}
			break;

			// Home Motor Test
		case 3:
			if ((nResult = Home(GMP_WAIT)) == GMP_SUCCESS)
			{
				LogAction("Home Motor Test\tSUCCESS");
			}
			else
			{
				LogAction("Home Motor Test\tFAIL\t%d", nResult);
			}
			break;

			// Protection Enable Test
		case 4:
			if ((nResult = EnableProtection()) == GMP_SUCCESS)
			{
				LogAction("Enable Protection Test\tSUCCESS");
			}
			else
			{
				LogAction("Enable Protection Test\tFAIL\t%d", nResult);
			}
			break;

			// Move Relative Test
		case 5:
			*pModSelected = TRUE;
			Sleep(1000);
			if (stIndexProfile.sSrchDirection == POSITIVE_DIR)
			{
				lPosn = -5000;
			}
			else
			{
				lPosn = 5000;
			}

			lStartPosn = GetEncPosn();

			if ((nResult = MoveRelative(lPosn, GMP_WAIT)) == GMP_SUCCESS)
			{
				if (((lStartPosn + lPosn) - GetEncPosn()) < 5)
				{
					LogAction("MoveRelative Test\tSUCCESS");
				}
				else
				{
					nResult = GMP_FAIL;
					LogAction("MoveRelative Test\tFAIL\t%ld Cnt Error", (lStartPosn + lPosn) - GetEncPosn());
				}
			}
			else
			{
				LogAction("MoveRelative Test\tFAIL\t%d", nResult);
			}
			break;

			// Move Absolute Test
		case 6:
			*pModSelected = TRUE;
			Sleep(1000);
			if (stIndexProfile.sSrchDirection == POSITIVE_DIR)
			{
				lPosn = -5000;
			}
			else
			{
				lPosn = 5000;
			}

			if ((nResult = MoveAbsolute(lPosn, GMP_WAIT)) == GMP_SUCCESS)
			{
				if ((lPosn - GetEncPosn()) < 5)
				{
					LogAction("MoveAbsolute Test\tSUCCESS");
				}
				else
				{
					nResult = GMP_FAIL;
					LogAction("MoveAbsolute Test\tFAIL\t%ld Cnt Error", lPosn - GetEncPosn());
				}
			}
			else
			{
				LogAction("MoveAbsolute Test\tFAIL\t%d", nResult);
			}
			break;

		default:
			nTestProcedure = 99;
		}

		if (nResult != GMP_SUCCESS)
		{
			break;
		}
		else
		{
			nTestProcedure++;
		}

	} while (nTestProcedure < 99);

	return nResult;
}

INT CMotorInfo::ModifySWProtection(BOOL bEnable)
{
	INT nResult = GMP_FAIL;
	CString szName = _T(" ");
	szName = GetName();

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	if (stAttrib.bEncoderExist)
	{
		if (bEnable)
		{
			nResult = EnableProtection();
		}
		else
		{
			if (IsPowerOn())
			{
				nResult = DisableProtection(GMP_PROT_TYPE_SW_LMT);
				pGmpChannel->ClearAllError();
				pGmpChannel->ClearDriverError();
			}
		}
	}
	return nResult;
}


INT CMotorInfo::MotorAbort()
{
	INT		nResult		= GMP_SUCCESS;
	CString szResult	= _T("");
	CString szErrMsg	= "";
	CString szDrvMsg	= "";

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		pGmpChannel->Abort();
	}
	catch (CAsmException e)
	{
		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CMotorInfo::MotorStop()
{
	INT		nResult		= GMP_SUCCESS;
	CString szResult	= _T("");
	CString szErrMsg	= "";
	CString szDrvMsg	= "";

	if (!IsHardwareSelected())
	{
		return nResult;
	}

	try
	{
		pGmpChannel->Stop();
	}
	catch (CAsmException e)
	{
		nResult = pGmpChannel->ReportErrorStatus(&szErrMsg, &szDrvMsg);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->SetError(lErrorCode);
//		pAppStation->SetError(nResult);
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}