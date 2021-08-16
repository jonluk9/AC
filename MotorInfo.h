#pragma once

#include "GI_Channel.h"
#include "GI_System.h"
#include "AC9000.h"

////Motor Info. Related
//typedef struct
//{
//	char		*cHomePortAddrss;
//	INT			nHomeMask;
//	char		*nNegLmtPortAddrss;
//	INT			nNegLmtMask;
//    char		*nPosLmtPortAddrss;
//	INT			nPosLmtMask;
//
//}	MOTOR_SENSOR;
//
//typedef struct
//{
//	char				*cSettlingCtrlType;
//	short				sSettliingTime;
//
//}	CTRL_PROFILE;
//
//typedef struct
//{
//	//Profile String
//	SHORT				sMotionProfID;
//	char				*cMotionCtrlType;
//
//	DOUBLE				dJerk;
//	DOUBLE				dAcc;
//	DOUBLE				dDec;
//	DOUBLE				dVel;
//
//}	MOTION_PROFILE;
//
//typedef struct
//{
//	BOOL				bEnable;
//	SHORT				sSrchProfID;
//	char				*cSrchCtrlType;
//
//	// Srch Input Related
//	SHORT				sSrchDirection;
//	BOOL				bSetPosn;
//	LONG				lDesiredPosn;
//	BOOL				bCheckError;
//
//	// Srch Profile Related
//	GMP_EVT_TYPE		enSrchType;		// Setup SrchType, e.g. HomeSnr, Indexor, SW Port, e.t.c
//	char				*szPort;
//	unsigned int		unSensorMask;
//	BOOL				bAstate;
//	GMP_EVT_LMT_TYPE	enLimitType;	// Srch by distance lmt or Time Lmt
//
//	// Srch Move Related
//	DOUBLE				dSrchVel;		// mm/s		(The value would be transformed in Get/Set Search Profile Functions)
//	DOUBLE				dSrchLimit;		// mm		(The value would be transformed in Get/Set Search Profile Functions)
//	DOUBLE				dDriveInVel;	// mm/s		(The value would be transformed in Get/Set Search Profile Functions)
//	DOUBLE				dDriveIn;		// mm		(The value would be transformed in Get/Set Search Profile Functions)
//	short				sDebounce;		// confirm the state after number of continuous samples when snr is cut
//	
//}	SRCH_PROFILE;
//
//typedef struct
//{
//	CString				szAxis;
//	DOUBLE				dEncoderScale;			// count / mm
//	DOUBLE				dDistPerCount;			// um / count
//	MOTOR_SENSOR		stSensorMapping;		// ?
//
//	SHORT				sPositionLimit;
//	SHORT				sDacLimit;
//	SHORT				sTimeLimit;
//
//	BOOL				bLmtSnrState;			// Define Limit Snr Active State
//	BOOL				bEncoderExist;			// Enable Encoder Fault and SW Position Limit Protection
//	BOOL				bLmtSnrExist;			// Enable Limit Snr Protection
//
//}	MOTOR_ATTRIB;
//
//typedef struct
//{
//	// Open Comm. Info.
//	GMP_COMMUTATION_OPTION	enCommOption;
//
//	SHORT					sNumberOfPoles;
//	ULONG					ulCountPerRev;
//	// For DirectionalComm
//	BOOL					bCommDir;
//
//	// For OpenStationaryComm
//	INT						unSlowDownFactor;
//	DOUBLE					dAcceptance;
//	DOUBLE					dInitialDAC;
//	DOUBLE					dIncreamentDAC;
//	DOUBLE					dMaximumDAC;
//	DOUBLE					dFitError;
//
//	INT						nCommStatus;
//
//}	MOTOR_COMM;
//
//typedef struct
//{
//	LONG			lPosLmt;			// Positive Limit Posn
//	LONG			lNegLmt;			// Negative Limit Posn
//
//}	MOTOR_PROTECTION;
//
//class CMotorInfo
//{
//private:
//	CGmpChannel			*pGmpChannel;
//	class CAC9000App	*pAppStation;
//	BOOL				*pModSelected;
//	
//public:
//	LONG				lErrorCode;			// (New) Define Mtr Error Code
//	INT					nTCObjectID;		// (New) Define Mtr Error Code
//	BOOL				bHomeStatus;			// Check Is Home or not
//	LONG				lCurrPosn;			// Record the current Posn
//
//	MOTOR_PROTECTION	stProtect;
//	MOTOR_ATTRIB		stAttrib;
//	MOTOR_COMM			stComm;
//
//	MOTION_PROFILE		stMotionProfile;
//	CTRL_PROFILE		stCtrlProf;	
//
//	SRCH_PROFILE		stHomeProfile;
//	SRCH_PROFILE		stIndexProfile;
//
//	SRCH_PROFILE		stUsrDefProfile[MAX_NUM_OF_SRCH_PROF];
//
//	CMotorInfo();
//	CMotorInfo(CGmpChannel* pGmpChannel, CAC9000App *pAppStation, BOOL *pModSelected);
//	~CMotorInfo();
//
//
//	VOID InitDefaultMotorInfo();
//	// Add me
//	CString GetName();
//
//	INT InitMtrAxis();
//	
//	// Set parameters to CGmpChannel
//	INT SelectCtrlPara();
//	INT SetMoveInput();
//	INT SetMoveProf();
//
//	INT SetSoftwareLmt();
//
//	INT InitHomeProf();
//	INT InitSrchProf(CString szAxis, SRCH_PROFILE stSrchProf);
//
//	INT EnableProtection();
//	INT EnableProtection(GMP_PROT_TYPE enProtectionType, BOOL bTriggerState);
//	INT DisableProtection(GMP_PROT_TYPE enProtectionType);
//
//	INT PowerOn(BOOL bMode);
//	BOOL IsPowerOn();
//
//	INT Commutate(GMP_COMMUTATION_OPTION enCommMode = GMP_COMMUTATION_OPTION_NONE);
//
//	INT ClearError();
//
//	INT Sync();
//
//	INT MoveRelative(LONG lMtrCnt, BOOL bWait);
//	INT MoveAbsolute(LONG lMtrCnt, BOOL bWait);
//
//	INT Search(SRCH_PROFILE stSrchProf, BOOL bWait);
//
//	INT GetEncPosn();
//	INT GetCmdPosn();
//
//	// Operation
//	INT Home(BOOL bWait);
//};