/////////////////////////////////////////////////////////////////
// AC9000Stn.h : interface of the AC9000Stn class
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


#include "AppStation.h"
#include "StringMapFile.h"
#include "Logging_Util.h"
#include "CLogFile.h"
#include "gmp_macro.h"

#include "GI_Channel.h"
#include "GI_SiSo.h"
#include "GI_System.h"
#include "GI_DAC.h"

#include <deque> //20131010

#ifdef EXCEL_MACHINE_DEVICE_INFO
#include "ExcelMachineInfo_Util.h"
#endif

#define MAX_NUM_WHZ_CAL_STEP	(90) //whz rotary linear calib
#define NUM_OF_SAMPLE_DATA		(5)
#define WHZ_CAL_DISTPERCNT		(0.5)

#define HIPEC_FREQ		(2000.0)

#define OFF				(0)
#define ON				(1)

#ifndef PI
#define PI				(3.14159265358979)
#endif

#define SEQ_DEQUE_LENGTH	20 //20131010
#define KGF_TO_NEWTON	(9.8)

#define UPDATEOUTPUT_SLEEP	(100)

#define MACHINE_PB		(0)
#define MACHINE_MB		(1)
#define MACHINE_PBMB	(2)

#define MAX_SIZE_OF_ACF_ARRAY	(35)

#define COR_ARRAY_SIZE	(10)

#define MAX_FORCE_DATA	(11)
#define MAX_TEMP_DATA	(5)
#define MAX_RECORD_DATA	(50)
#define MAX_STATION_SEQ_DATA (4)

#define NUM_OF_POSN_XY		(20)
#define NUM_OF_LEVEL_Z		(20)
#define NUM_OF_PARA			(40)

#define PREBOND_T_CALIB_DATA (5000) //300)		// +/- 15 degree, each data stands for 0.1 degree

#define CONVERT_TO_MAPPING  1000

// Message Box
#define MSG_OK						(1)
#define MSG_CLOSE					(2)
#define MSG_CONTINUE				(3)

#define MSG_OK_RETRY				(101)
#define MSG_OK_CANCEL				(102)
#define MSG_YES_NO					(103)
#define MSG_YES_RETRY				(104)
#define MSG_YES_CANCEL				(105)
#define MSG_RETRY_CANCEL			(106)
#define MSG_CONTINUE_STOP			(107)
#define MSG_CONTINUE_RETRY			(108)
#define MSG_CONTINUE_CANCEL			(109)

#define MSG_OK_RETRY_CANCEL			(201)
#define MSG_OK_RETRY_STOP			(202)
#define MSG_YES_NO_RETRY			(203)
#define MSG_YES_RETRY_CANCEL		(204)
#define MSG_ABORT_RETRY_CANCEL		(205)
#define MSG_STOP_RETRY_CANCEL		(206)
#define MSG_CONTINUE_RETRY_STOP		(207)
#define MSG_CONTINUE_RETRY_CANCEL	(208)

#define MAX_MAINBOND	(3)

#define rMSG_ERROR			(0)
#define rMSG_OK				(1)
#define rMSG_CANCEL			(2)
#define rMSG_YES			(3)
#define rMSG_NO				(4)
#define rMSG_RETRY			(5)
#define rMSG_CLOSE			(6)
#define rMSG_CONTINUE		(7)
#define rMSG_STOP			(8)
#define rMSG_ABORT			(9)
#define rMSG_TIMEOUT		(11)

#define NO_PROCESS_LIST		(-1)

#define MAX_NUM_OF_FPC_LAYER	(6)	// 20140325 Yip

#define FPC_GAP				(5.0)
#define GAP_THICKNESS		(1.0)

///////////////////////////////////
#define MAX_LTC_CH		(12)
#define NUM_OF_LITEC	(1)//(4)

#define SERVO_MOTOR					TRUE
#define STEPPER_MOTOR				FALSE

typedef enum
{
	LOW_BOND_FORCE_MODE = 0,
	NORMAL_BOND_FORCE_MODE,
	MAX_BOND_FORCE_MODE,
} ACF_BOND_FORCE_MODE;

typedef enum //20141120
{
	TA4_COR = 0,
	MBDL1_COR,
	MBDL2_COR,
	MAX_NUM_OF_TA4_COR,

}TA4_COR_NUM;


typedef enum //20140122
{
	ACF1_CH1 = 0,
	Litec1_CH2,
	ACF1WH_CH3,
	ACF2WH_CH4,
	Litec1_CH5,
	Litec1_CH6,			//5
	Litec1_CH7,
	Litec1_CH8,
	Litec1_CH9,
	Litec1_CH10,
	Litec1_CH11,		//10
	Litec1_CH12,
	
	Litec2_CH1,	
	Litec2_CH2,
	Litec2_CH3, 
	Litec2_CH4,	//15	
	Litec2_CH5,
	Litec2_CH6,
	Litec2_CH7,	
	Litec2_CH8,
	Litec2_CH9,			//20
	Litec2_CH10,
	Litec2_CH11,
	Litec2_CH12,

	Litec3_CH1,
	Litec3_CH2,		//25
	Litec3_CH3,
	Litec3_CH4,
	Litec3_CH5,
	Litec3_CH6,
	Litec3_CH7,		//30
	Litec3_CH8,
	Litec3_CH9,
	Litec3_CH10,	
	Litec3_CH11,
	Litec3_CH12,	//35

	Litec4_CH1,
	Litec4_CH2,
	Litec4_CH3,
	Litec4_CH4,
	Litec4_CH5,		//40
	Litec4_CH6,
	Litec4_CH7,
	Litec4_CH8,
	Litec4_CH9,
	Litec4_CH10,			//45
	Litec4_CH11,
	Litec4_CH12,

} LITEC_CHANNEL_NUM;


/////////////////////////////////////////////////////
typedef struct
{
	LONG RotaryPosn;
	LONG LinearPosn;

} WHZ_DATA; //whz rotary linear calib

typedef enum
{
	VERSION_1 = 0,
	VERSION_2,
	VERSION_3,
	MAX_NUM_OF_MACHINE_VERSION, 

} MachineVersion;

typedef enum	// 20140523 Yip: TC entry re-order
{
	// Motor / Solenoid
	FPC_TL_Z1 = 0,
	INTRAY_OPEN_SOL,
	INTRAY_SPLIT_SOL,
	FPC_TL_X1,
	TRAY1_DOWN_SOL,
	FPC_TL_X2,	// 5
	TRAY2_DOWN_SOL,
	FPC_TL_Z2,
	OUTTRAY_OPEN_SOL,
	FPC_PA_X,
	FPC_PA_Y,	// 10
	FPC_PA_Z1,
	FPC_PA_T1,
	FPC_PA_Z2,
	FPC_PA_T2,
	SH_X,	// 15
	SH_Y,
	SH_REJECT_BIN_SOL,
	SH_FPC_PRESSER_SOL,
	SH_IONIZER_AIR_SOL,
	SH_IONIZER_PWR_ENABLE, //20
	SH_SOL,
	FPC_RA_X_SOL,
	FPC_RA_Z_SOL,
	FPC_PRESS1_SOL,
	FPC_PRESS2_SOL, //25
	INSH_X,
	INSH_Y,
	INSH_Z1,
	INSH_Z2,
	INSH_EXTEND_SOL,	//30
	REJECT_BELT,
	IN_RA_GO_FRONT_SOL,
	INSH_SOL,
	IN_PICKARM_X,
	IN_PICKARM_Z,		//35
	IN_PICKARM_SOL,
	TA1_X,
	TA1_Z,		//35
	TA1_SOL,
	INWH_Y,
	INWH_Z1,
	INWH_Z2,
	INWH_T,		//40
	INWH_SOL,
	TA2_X,
	TA2_SOL,
	ACF1WH_Y,
	ACF1WH_Z,	//45
	ACF1_STRIPPER_X,	//46
	ACF1WH_SOL,
	ACF1_INDEX,
	ACF1_ROLLER,
	ACF1_SOL,	//50
	ACF2WH_Y,	//51
	ACF2WH_Z,
	ACF2_STRIPPER_X,
	ACF2WH_SOL,
	ACF2_INDEX,	//55
	ACF2_ROLLER,
	ACF2_SOL,	//57
	ACF_WH_PR_GRAB,
	ACF_WH_PR_PROCESS,
	TA3_X,
	TA3_SOL,
	PBWH_X1,	//60
	PBWH_Y1,
	PBWH_Z1,	//62
	PBWH1_SOL,
	PB1_Z,
	PB1_T,	// 65
	PB1_FPC_REJECT_X,
	PB1_SOL,
	PBWH_X2,	//68
	PBWH_Y2,
	PBWH_Z2,	//70
	PBWH_T,
	PBWH2_SOL,
	PB2_Z,		//73
	PB2_T,
	PB2_SOL,	//75
	PB_UPLOOK_X,
	PB_UPLOOK_Y,
	TA4_X,			//78
	TA4_UPDNLK_Y,	
	TA4_T1,		//80
	TA4_T2,	// 81
	TA4_EXTEND_SOL,
	TA4_SOL,
	MB1WH_Y,
	MB1WH_Z,	//85
	MB1WH_SOL,		//86
	MB2WH_Y,
	MB2WH_Z,
	MB2WH_SOL,	// 89
	MB1_Z,		//90
	MB1_INDEX,		//91
	MB1_REWINDER1,
	MB1_REWINDER2,
	MB1_SOL,
	MB3WH_Y,	//95
	MB3WH_Z,		//96
	MB3WH_SOL,
	MB4WH_Y,
	MB4WH_Z,	// 99
	MB4WH_SOL,	//100
	MB2_Z,			//101
	MB2_INDEX,
	MB2_REWINDER1,
	MB2_REWINDER2,	//104
	MB2_SOL,		//105
	MB5WH_Y,	
	MB5WH_Z,
	MB5WH_SOL,
	MB6WH_Y,	// 109
	MB6WH_Z,	//110
	MB6WH_SOL,		//111
	MB3_Z,
	MB3_INDEX,
	MB3_REWINDER1,
	MB3_REWINDER2,	//115
	MB3_SOL,		//116
	TA5_X,
	TA5_SOL,
	OUTWH_Y,
	OUTWH_Z1,	//120
	OUTWH_Z2,		//121
	OUTWH_SOL,
	IN_CONV,
	IN_CONV_SOL,	//124
	OUT_CONV_SOL,	//125
	OTHER_SOL,
	IN_CONV_X,

	// PR
	IN_WH_PR_GRAB,		//128
	IN_WH_PR_PROCESS,
	ACF1WH_PR_GRAB,		
	ACF1WH_PR_PROCESS,
	ACF2WH_PR_GRAB,		
	ACF2WH_PR_PROCESS,
	PB_GLASS_PR_GRAB,	// 132
	PB_GLASS_PR_PROCESS,
	PB_LSI_PR_GRAB,
	PB_LSI_PR_PROCESS,		//135
	SH_PR_GRAB,
	SH_PR_PROCESS,	// 137
	ACFUL_PR_GRAB,		
	ACFUL_PR_PROCESS,


	NUM_OF_TC_OBJECT,	// 138

} TC_OBJECT_ID;

/*
typedef enum //20120903 TC entry re-order
{
	IN_CONV = 0, 
	IN_CONV_SOL, 
	TA1_X,
	TA1_Z,
	TA1_SOL,
	TA2_X,		//5
	TA2_SOL,
	TA3_X,
	TA3_SOL,
	TA4_X,
	TA4_SOL,	//10
	TA5_X,
	TA5_SOL,	
	INWH_Y,
	INWH_Z1,
	INWH_Z2,	//15
	INWH_T,
	INWH_SOL,
	ACF1WH_Y, 
	ACF1WH_Z, 
	ACF1WH_SOL,		//20
	ACF2WH_Y, 
	ACF2WH_Z, 
	ACF2WH_SOL,
	ACF1_INDEX,
	ACF1_ROLLER,	//25
	ACF1_SOL,
	ACF2_INDEX,
	ACF2_ROLLER,
	ACF2_SOL,
	PBWH_X,			//30
	PBWH_Y,
	PBWH_Z1,
	PBWH_Z2,
	PBWH_SOL,
	PB_UPLOOK_X,	//35
	PB_UPLOOK_Y,
	PB1_Z,
	PB1_T,
	PB1_SOL,
	PB2_Z,			//40
	PB2_T,
	PB2_SOL,
	TH_X1,
	TH_X2,
	TH_SOL,			//45
	CPA_Y,
	CPA_Z,
	CPA_T,
	CPA_SOL,
	FLIPPER_Z,			//50
	FLIPPER_SWING,
	FLIPPER_FLIP,
	FLIPPER_SOL,
	SH1_X,
	SH1_Y,				//55
	SH1_SOL,
	SH2_X,
	SH2_Y,
	SH2_SOL,
	MB1WH_Y,			//60
	MB1WH_Z1,
	MB1WH_Z2,
	MB1WH_SOL,
	MB2WH_Y,
	MB2WH_Z1,			//65
	MB2WH_Z2,
	MB2WH_SOL,

	MB3WH_Y,
	MB3WH_Z1,			//65
	MB3WH_Z2,
	MB3WH_SOL,

	MB4WH_Y,
	MB4WH_Z1,			//65
	MB4WH_Z2,
	MB4WH_SOL,

	MB5WH_Y,
	MB5WH_Z1,			//65
	MB5WH_Z2,
	MB5WH_SOL,

	MB6WH_Y,
	MB6WH_Z1,			//65
	MB6WH_Z2,
	MB6WH_SOL,
	MB1_Z,
	MB1_INDEX,
	MB1_SOL,			//70
	MB2_Z,
	MB2_INDEX,
	MB2_SOL,
	MB3_Z,
	MB3_INDEX,
	MB3_SOL,
	OUT_CONV_SOL,
	IN_WH_PR_GRAB,		//75
	IN_WH_PR_PROCESS,
	ACF_PR_GRAB,
	ACF_PR_PROCESS,
	PB_GLASS_PR_GRAB,
	PB_GLASS_PR_PROCESS,	//80
	PB_LSI_PR_GRAB,
	PB_LSI_PR_PROCESS,
	SH1_PR_GRAB,
	SH1_PR_PROCESS,
	SH2_PR_GRAB,			//85
	SH2_PR_PROCESS,
	TH_PR_GRAB,
	TH_PR_PROCESS,
	MB1_REWINDER,
	MB2_REWINDER,			//90
	OTHER_SOL,
	INSH_SOL,
	OUTWH_SOL,

	NUM_OF_TC_OBJECT,

} TC_OBJECT_ID;
*/

//typedef enum
//{
//	IN_CONV = 0, 
//	IN_CONV_SOL, 
//	INWH_Y,
//	INWH_Z1,
//	INWH_Z2,
//	INWH_T,			//5
//	INWH_SOL,
//	ACF1WH_Y, 
//	ACF1WH_Z, 
//	ACF1WH_SOL, 
//	ACF2WH_Y,		//10
//	ACF2WH_Z, 
//	ACF2WH_SOL,
//	PBWH_X,
//	PBWH_Y,
//	PBWH_Z1,		//15
//	PBWH_Z2,
//	PBWH_SOL,
//	MB1WH_Y,
//	MB1WH_Z1,
//	MB1WH_Z2,		//20
//	MB1WH_SOL,
//	MB2WH_Y,
//	MB2WH_Z1,
//	MB2WH_Z2,
//	MB2WH_SOL,		//25
//	TA1_X,
//	TA1_Z,
//	TA1_SOL,
//	TA2_X,
//	TA2_SOL,		//30
//	TA3_X,
//	TA3_SOL,
//	TA4_X,
//	TA4_SOL,
//	TA5_X,			//35
//	TA5_SOL,	
//	ACF1_INDEX,
//	ACF1_ROLLER,
//	ACF1_SOL,
//	ACF2_INDEX,		//40
//	ACF2_ROLLER,
//	ACF2_SOL,
//	PB_UPLOOK_X,
//	PB_UPLOOK_Y,
//	PB1_Z,			//45
//	PB1_T,
//	PB1_SOL,
//	PB2_Z,
//	PB2_T,
//	PB2_SOL,		//50
//	MB1_Z,
//	MB1_INDEX,
//	MB1_SOL,
//	MB2_Z,
//	MB2_INDEX,		//55
//	MB2_SOL,
//	TH_X1,
//	TH_X2,
//	TH_SOL,
//	CPA_Y,			//60
//	CPA_Z,
//	CPA_T,
//	CPA_SOL,
//	FLIPPER_Z,
//	FLIPPER_SWING,	//65
//	FLIPPER_FLIP,
//	FLIPPER_SOL,
//	SH1_X,
//	SH1_Y,
//	SH1_SOL,		//70
//	SH2_X,
//	SH2_Y,
//	SH2_SOL,
//	OUT_CONV_SOL,
//	IN_WH_PR_GRAB,	//75
//	IN_WH_PR_PROCESS,
//	ACF_PR_GRAB,
//	ACF_PR_PROCESS,
//	PB_GLASS_PR_GRAB,
//	PB_GLASS_PR_PROCESS,	//80
//	PB_LSI_PR_GRAB,
//	PB_LSI_PR_PROCESS,
//	SH1_PR_GRAB,
//	SH1_PR_PROCESS,
//	SH2_PR_GRAB,			//85
//	SH2_PR_PROCESS,
//	TH_PR_GRAB,
//	TH_PR_PROCESS,
//	OTHER_SOL,
//
//	NUM_OF_TC_OBJECT,		//90
//
//} TC_OBJECT_ID;

typedef enum
{
	SF_WIZARD_IDLE = -1,
	SF_WINEAGLE,
	SF_TA1,
	SF_TA2,
	SF_ACF1,
	//SF_ACF2,
	SF_ACF1WH,
	SF_ACF2WH,
	SF_INPICKARM,
	SF_INSP1,
	SF_INSP2,

}	SETTING_FILE_OPTION;

typedef enum
{
	NO_FPC_TYPE = -1, 
	FPC_TYPE1 = 0, 
	FPC_TYPE2, 
	MAX_NUM_OF_FPC_TYPE, 

} FPCTypeNum;

typedef enum
{
	SCREEN_IDLE = -1,
	SCREEN_WINEAGLE,
	SCREEN_TA1,
	SCREEN_TA2,
	SCREEN_ACF1,
	//SCREEN_ACF2,
	SCREEN_ACF1WH,		//4
	SCREEN_ACF2WH,		//5
	SCREEN_INPA,
	SCREEN_INSP1,
	SCREEN_INSP2,
	SCREEN_ACF_DOWNLOOK,
	SCREEN_INSP_DOWNLOOK,	//10

	// PR Screen
	// Add me later
	
	//DEVICE
	SCREEN_DEVICE_INFO, 
	SCREEN_FORCE_CALIBATION, 
	SCREEN_ACF_UPLOOK,

}	MODULE_SETUP_SCREEN;

typedef enum
{
	HEATER_OP_OK	= 0, 
	HEATER_NOT_INIT, 
	HEATER_FAILED

} HEATER_ERR;

typedef enum
{
	BURN_IN = 0, 
	INDEXING, 
	DIAG_BOND, 
	TEST_BOND, 
	AUTO_BOND, 

} AUTOMODE;

typedef struct 
{
	// all in mm
	DOUBLE dHeight;

} FORCE_CALIBRATOR;

typedef struct
{
	LONG RotaryPosn;
	LONG LinearPosn;
	
} ENC_CALIB_DATA;

typedef enum
{
	FLIP_MODE = 0,
	NON_FLIP_MODE,

} FLIPMODE;

typedef enum
{
	KI_0_MODE,
	OFF_MOTOR_MODE,
	NORMAL_MODE,

}	WH_BONDING_CTRL_MODE;

typedef enum
{
	NO_UNIT		= -1,
	LEFT_UNIT	= 0, 
	RIGHT_UNIT,
	MAX_NUM_OF_MB_UNIT,

} MB_UNIT;

typedef enum
{
	MAINBOND_NONE	= -1,		// Seuqence: PreBondWH Posn
	MAINBOND_1		= 0, 
	MAINBOND_2,
	//MAINBOND_3,
	MAX_NUM_OF_MAINBOND,		// Seuqence: Out Conveyor Posn
	MAX_NUM_WITH_MAINBOND_3,	// 20140708 Yip: Assume Main Bond 3 Exists For Handling MB5WH And MB6WH

} MainbondType;

typedef enum
{
	NO_MON = -1,
	ADAM1,
	//ADAM2,
	LITECMon,

} MonUnitNum;

//typedef enum  , NOT USED, Error in handshaking TA4, MBWHs, TA5, TAManager
//{
//	MBWH_NONE	= -1,		
//	MBWH_1		= 0, 
//	MBWH_2,
//	MBWH_3,
//	MBWH_4,
//	MBWH_5,
//	MBWH_6,
//	MAX_NUM_OF_MBWH, 
//
//} MBWHType;

typedef enum
{
	MB_NONE		= -1,
	MB1_LEFT,
	MB1_RIGHT,
	MB2_LEFT,
	MB2_RIGHT,
	MAX_NUM_OF_UNIT,

} MainbondUnitNum;

typedef enum
{
	ACF_NONE = -1,
	ACF_1,
//	ACF_2,
	MAX_NUM_OF_ACF_UNIT,

} ACFUnitNum;

typedef enum
{
	ACFWH_NONE = -1,
	ACFWH_1,
	ACFWH_2,
	MAX_NUM_OF_ACFWH,

} ACFWHUnitNum;

typedef enum
{
	PREBOND_NONE	= -1,
	PREBOND_1,
	PREBOND_2,
	MAX_NUM_OF_PREBOND,

} PreBondUnitNum;

typedef enum
{
	SHUTTLE_NONE	= -1,
	SHUTTLE_1,
	SHUTTLE_2,
	MAX_NUM_OF_SHUTTLE,

} ShuttleUnitNum;

typedef enum
{
	INSP_NONE	= -1,
	INSP_1,
	INSP_2,
	MAX_NUM_OF_INSP,

} InspUnitNum;

typedef enum
{
	FAST_MODE = 0,
	PRECISE_MODE,

} PR_BondMode;

typedef enum
{
	BUTTON_1, 
	BUTTON_2, 
	BUTTON_3, 
	BUTTON_TIMEOUT,

} Response;

typedef enum
{

	NO_TI = -1,
	TI_1 = 0, 
	TI_2, 
	MAX_NUM_OF_TI, 

} TrayIndexerType;

typedef enum
{
	NEW_TRAY = 0,
	READY_TRAY, 
	SAFE_TO_LOAD_TRAY,
	EMPTY_TRAY, 

} CUR_TRAY_TYPE;

typedef struct
{
	// all in mm
	DOUBLE dLength;
	DOUBLE dWidth;

	DOUBLE dThickness1;
	DOUBLE dThickness2;
	DOUBLE dThickness3;
	DOUBLE dThickness4;

	LONG lSealantPos;

	DOUBLE dItoWidth;
	DOUBLE dPBWHOffset;
	DOUBLE dACFWHOffset;
	DOUBLE dInConOffset;
	DOUBLE dACFWHUnloadOffset;

	CString szName;
	LONG	lType; // 0: Glass, 1: PCB

} GLASS;

typedef enum
{
	TWO_COL = 0,
	THREE_COL,
	FOUR_COL, 
	MAX_NUM_OF_COL, 

} ColumnSize;

typedef enum
{
	NO_TRAYHOLDER = -1,
	TRAYHOLDER_1 = 0, 
	TRAYHOLDER_2, 
	MAX_NUM_OF_TRAYHOLDER, 

} TrayHolderNum;

typedef enum
{
	USE_NONE = -1, 
	USE_TYPE1 = 0, 
	USE_TYPE2, 
	USE_BOTH,

} LSITypeInUse;


//must same as FnDeclare.h
typedef enum 
{
	USER		= 0,
	OPERATOR,
	ENGINEER,
	SERVICE,
	DEVELOPER,

} USER_LOGIN;

typedef enum
{
	DUAL_PRESS = 0,
	SINGLE_LEFT_PRESS,
	SINGLE_RIGHT_PRESS,

} PRESS_MODE;

typedef enum
{
	LOW_BOND_FORCE = 0,
	NORMAL_BOND_FORCE,
	NUM_OF_BOND_FORCE_MODE,

} BOND_FORCE_MODE;

typedef enum
{
	NO_HANDOVER = -1,
	HANDOVER_LSI_TYPE1_SH1,
	HANDOVER_LSI_TYPE1_SH2,
	HANDOVER_LSI_TYPE2_SH1,
	HANDOVER_LSI_TYPE2_SH2,
	MAX_NUM_OF_HANDOVER,

} HandOverStatusNum;

typedef enum
{
	NO_LSI_TYPE = -1, 
	LSI_TYPE1 = 0, 
	LSI_TYPE2, 
	MAX_NUM_OF_LSI_TYPE, 

} LSITypeNum;

typedef enum
{
	NO_LSI = -1,	// exceptional case
	LSI1 = 0, 
	LSI2,
	LSI3,
	LSI4,
	LSI5,
	LSI6,
	LSI7,
	LSI8,
	MAX_NUM_OF_LSI,	// on Glass

} LSINum;

#ifdef PR_MULTI_IMAGE //20150821
typedef enum
{
	NO_IMAGE_NUM = -1, 
	IMAGE_NUM_0 = 0, 
	IMAGE_NUM_1, 
	IMAGE_NUM_2, 
	IMAGE_NUM_3, 
	MAX_NUM_OF_IMAGE, 
} PRImageNum;
#endif

typedef enum
{
	NO_CH = -1,
	CH0 = 0, 
	CH1,
	CH2,
	CH3,
	CH4,
	CH5,
	CH6,
	CH7,

} MonChNum;

typedef enum
{
	NO_PRESS = -1,	// exceptional case
	PRESS1 = 0, 
	PRESS2,
	PRESS3,
	PRESS4,
	PRESS5,
	PRESS6,
	PRESS7,
	PRESS8,
	MAX_NUM_OF_PRESS,	// on Glass

} PRESSNum;

typedef enum
{
	PB_INSP_BOTH = 0, 
	PB_INSP_PT1, 
	PB_INSP_PT2,

} PBInspectionPt;


typedef enum
{
	PR_LEVEL_NORMAL = 0,
	PR_LEVEL_IR,
	MAX_NUM_OF_PR_LEVEL,

} PRMultiLevel; //20150106

//typedef enum
//{
//	WHZ_NONE = -1,
//	WHZ_1 = 0,
//	WHZ_2,
//	MAX_NUM_OF_WHZ,
//
//} WHZNum;

typedef enum
{
	NO_GLASS = -1,
	GLASS1 = 0,
	//GLASS2,
	MAX_NUM_OF_GLASS,

} GlassNum;

typedef enum
{
	NO_ACF = -1,	// exceptional case
	ACF1 = 0, 
	//ACF2,
	//ACF3,
	//ACF4,
	//ACF5,
	//ACF6,
	//ACF7,
	//ACF8,
	MAX_NUM_OF_ACF,	

} ACFNum;

typedef enum
{
	COL_1 = 0,
	COL_2,
	COL_3,
	COL_4,
	MAX_NUM_OF_TRAY_IN_COL,	

} TrayInColNum;

typedef enum
{
	UPPER_LEFT = 0,
	UPPER_RIGHT,
	LOWER_LEFT,
	
	MAX_NUM_OF_CORNER,	

} CornerNum;

typedef enum
{
	EDGE_NONE = -1,
	EDGE_A	= 0, // Upper
	//EDGE_B,		// Right
	//EDGE_C,		// Bottom
	//EDGE_D,		// LEft
	MAX_EDGE_OF_GLASS,
} EdgeNum;

typedef enum
{
	ST_PR_IDLE = 0,
	ST_PR1_IN_PROGRESS,
	ST_PR2_IN_PROGRESS,
	ST_PB_PR1_IN_PROGRESS,
	ST_PB_PR2_IN_PROGRESS,

	MAX_OF_PR_STATUS,	

} PR_Status;

typedef struct 
{
	DOUBLE	Force_kgf;
	DOUBLE	Pressure_bar;
	LONG	OpenDacValue;

} FORCE_DATA;

typedef struct
{
	DOUBLE SetTemp;
	DOUBLE BondTemp;

} TEMP_DATA;

typedef struct
{
	DOUBLE	dDataSet[MAX_RECORD_DATA];
	DOUBLE	dMean;
	DOUBLE	dMax;
	DOUBLE	dMin;

} DATA_SET;

typedef struct
{
	CString szTitle[MAX_PRINT_DATA];
	DOUBLE 	dData[MAX_PRINT_DATA];

} PRINT_DATA;

typedef struct 
{
	LONG x;
	LONG y;

} MTR_POSN;

typedef struct 
{
	DOUBLE x;
	DOUBLE y;

} D_MTR_POSN;


typedef struct 
{
	LONG Col;
	LONG Row;
	LONG Layer;	// 20140324 Yip

} WAFFLE_POSN;

typedef struct 
{
	LONG x;
	LONG y;

} VIR_COORD;

typedef struct 
{
	LONG	x;
	LONG	y;
	DOUBLE	Theta;

} OFFSET;

typedef enum
{
	INWH = 0, 
	ACF_WH, 
	PREBONDWH, 
	MAINBONDWH,
	MAINBONDWH2,

} Indexing_Mod;

typedef enum
{
	DEG_ZERO = 0, 
	DEG_POS_90, 
	DEG_NEG_90, 
	DEG_180,

} FPC_ANGLE;

typedef enum
{
	GLASS_OFF_FPC_OFF = 0,
	GLASS_OFF_FPC_ON,
	GLASS_ON_FPC_OFF,
	GLASS_ON_FPC_ON,

} GLASS_FPC_VAC_STATE;	// 20140619 Yip


typedef struct
{
	LONG	lCoaxialLevel;
	LONG	lRingLevel;
	LONG	lCoaxial1Level;
	LONG	lSideLevel;
	LONG	lBackLevel; // 20141023

	DOUBLE	dObjectMean;
	DOUBLE	dObjectStDev;
	DOUBLE	dBackgroundMean;
	DOUBLE	dBackgroundStDev;

	DOUBLE	dSDThreshold;
	DOUBLE	dMeanThreshold;
	DOUBLE	dThreshold;


} IMAGE_STAT;

typedef struct
{
	FPC_ANGLE	Rotate;
	MTR_POSN	mtrLayerOffset;
	MTR_POSN	mtrLayerAlignPoint1;
	MTR_POSN	mtrLayerAlignPoint2;
}	LAYER_INFO;

typedef struct
{
	// Tray Holder Related	
	BOOL		bPtSet[MAX_NUM_OF_TI];
	MTR_POSN	mtrTrayUpperLeftPosn;
	MTR_POSN	mtrTrayUpperRightPosn;
	MTR_POSN	mtrTrayLowerLeftPosn;
	
	LONG		lTrayInRow;
	LONG		lTrayInCol;

	D_MTR_POSN	dmtrTrayRowPitch;
	D_MTR_POSN	dmtrTrayColPitch;

	// ChipTray Related
	LONG		lTrayTypeInCol[MAX_NUM_OF_TRAY_IN_COL];

	MTR_POSN	mtrPocketUpperLeftOffset[MAX_NUM_OF_TI];
	MTR_POSN	mtrPocketUpperLeftOffset2[MAX_NUM_OF_TI];
	MTR_POSN	mtrPocketUpperRightOffset[MAX_NUM_OF_TI];
	MTR_POSN	mtrPocketLowerLeftOffset[MAX_NUM_OF_TI];

	LONG		lPocketInRow[MAX_NUM_OF_TI];
	LONG		lPocketInCol[MAX_NUM_OF_TI];
	LONG		lPocketInLayer[MAX_NUM_OF_TI];	// 20140324 Yip
	
	D_MTR_POSN	dmtrPocketRowPitch[MAX_NUM_OF_TI];
	D_MTR_POSN	dmtrPocketColPitch[MAX_NUM_OF_TI];
	LAYER_INFO	stPocketLayerInfo[MAX_NUM_OF_TI][MAX_NUM_OF_FPC_LAYER];	// 20140325 Yip
	
}	TRAYHOLDER_INFO;


typedef struct
{
	// ON/OFF Loader Related
	BOOL		bPtSet;

	MTR_POSN	mtrPocketUpperLeftPosn;
	MTR_POSN	mtrPocketUpperLeftPosn2;
	MTR_POSN	mtrPocketUpperRightPosn;
	MTR_POSN	mtrPocketLowerLeftPosn;

	LONG		lPocketInRow;
	LONG		lPocketInCol;

	D_MTR_POSN	dmtrPocketRowPitch;
	D_MTR_POSN	dmtrPocketColPitch;

}	GLASSHOLDER_INFO;

typedef enum
{
	VERT_LEFT = 0,
	HORIZ_LEFT,

} TRAY_DIRECTION;

typedef enum
{
	FIRST_TO_LAST = 0,
	LAST_TO_FIRST,

} TRAY_COL_INDEX_MODE;

typedef struct
{
	CString	szPosn;
	BOOL	bEnable;
	BOOL	bVisible;
	LONG	lType;
	LONG	lPosnX;
	LONG	lPosnY;

}	SETUP_POSN_XY;

typedef struct
{
	CString	szLevel;
	BOOL	bEnable;
	BOOL	bVisible;
	LONG	lType;
	LONG	lLevelZ;

}	SETUP_LEVEL_Z;

typedef struct
{
	CString	szPara;
	BOOL	bEnable;
	BOOL	bVisible;
	LONG	lType;
	LONG	lPara;
	DOUBLE	dPara;

}	SETUP_PARA;

typedef struct
{
	LONG	lSetupTitle;
	LONG	lPara;
	DOUBLE	dPara;

} PARA;

#define MAX_NUM_DEBUG_STATUS	7
#define	EMPTY_MARK				0xdeadbeaf

typedef struct //20140425
{
	CString szLabel;
	LONG lStatus;
} GLASS_STATUS_ENTRY;

typedef struct //20140425
{
	CString szState;
	clock_t clkState;
	LONG lStatus[MAX_NUM_DEBUG_STATUS];							//number
	GLASS_STATUS_ENTRY stGlassStatus[MAX_NUM_DEBUG_STATUS];		//label and number
} STATE_ENTRY;

typedef struct
{
	long	lSetupTitle;
	double	dPara;

} PARA_D;

typedef enum
{
	MACHINE_PARA = 0,
	DEVICE_PARA,

} PARA_TYPE;

typedef enum
{
	GLASS_MOD	= 0,
	MB1_MOD,
	MB2_MOD,
	LSI_MOD,
	ACF_MOD,

} MinModuleRequest;

typedef enum
{	
	REWORK_MB1 = 0,
	REWORK_MB2,
	REWORK_PB,
	NORMAL_BONDING,

} AutoBondRequirment;

typedef enum
{
	ST_IDLE = 0, 
	ST_READY, 
	ST_BUSY,
	ST_ACK,			
	ST_ERROR,
	ST_ACK_CHIPPICKARM,	//5

	ST_REQ_PICK_TYPE1_SH1,
	ST_REQ_PICK_TYPE1_SH2,
	ST_REQ_PICK_TYPE2_SH1,
	ST_REQ_PICK_TYPE2_SH2,	//9

	ST_REQ_PLACE_TYPE1_SH1,
	ST_REQ_PLACE_TYPE1_SH2,
	ST_REQ_PLACE_TYPE2_SH1,
	ST_REQ_PLACE_TYPE2_SH2,	//13

	ST_REQ_TYPE1,
	ST_REQ_TYPE2,
	ST_REQ_LSI,	//16

	ST_ACK_BLOW_LSI,
	ST_ACK_REQ_TYPE1,
	ST_ACK_REQ_TYPE2,
	ST_PR_ERROR_TYPE1,
	ST_PR_ERROR_TYPE2,
	ST_PR_IN_PROCESS,
	ST_REQ_BLOW_LSI,	//23
	//ST_REQ_QUARTZ,

	ST_HOLD_GLASS,
	ST_PICK_ERROR,
	ST_PICK_GLASS,
	ST_PLACE_GLASS,
	ST_RELEASE_GLASS,
	ST_PR_DONE,
	ST_EMPTY,
	ST_READY_LOAD,
	ST_VAC_ERROR,
	ST_DONE,
	ST_REQ_ACF,
	ST_REQ_BONDING,	//35

	ST_READY_1_LEFT,
	ST_READY_1_RIGHT,
	ST_READY_2_LEFT,
	ST_READY_2_RIGHT,
	ST_READY_1,
	ST_READY_2,	//41

	ST_MB1_REQ_LOAD,
	ST_MB2_REQ_LOAD,
	ST_MB1_ACK_LOAD,
	ST_MB2_ACK_LOAD,
	ST_MB1_REQ_UNLOAD,
	ST_MB2_REQ_UNLOAD,
	ST_MB1_ACK_UNLOAD,
	ST_MB2_ACK_UNLOAD,	//49

	ST_PB1_PR_IN_PROCESS,
	ST_PB2_PR_IN_PROCESS,
	ST_PBWH_PR_IN_PROCESS,
	ST_REQUEST_UPLOOK,
	ST_ACK_PB1_BONDING,
	ST_ACK_PB2_BONDING,
	ST_ACK_PB1_PB2_BONDING,
	ST_ACK_PR1_GRAB,
	ST_ACK_PR2_GRAB,
	ST_ACK_TO_BOND_POSN,
	ST_BOND_COMPLETED,
	ST_PR_READY,		//60
	ST_REQ_PB1_BONDING,
	ST_REQ_PB2_BONDING,
	ST_REQ_WH_TO_BOND_POSN,
	ST_REQUEST_PR1_GRAB,
	ST_REQUEST_PR1_PROCESS,	//65
	ST_REQUEST_PR2_GRAB,
	ST_REQUEST_PR2_PROCESS,
	ST_REQUEST_PR1,
	ST_REQUEST_PR2,
	ST_READY_PICK,		//70


	ST_RETRY,
	ST_HOLD,	// ie Holder by other station
	
	ST_PLACE_READY1_READY2,
	ST_PLACE_READY1_ERR2,	
	ST_PLACE_READY1_EMPTY2,	//75

	ST_PLACE_EMPTY1_READY2,
	ST_PLACE_EMPTY1_ERR2,
	ST_PLACE_EMPTY1_EMPTY2,	//78

	ST_PLACE_ERR1_READY2,
	ST_PLACE_ERR1_ERR2,
	ST_PLACE_ERR1_EMPTY2,	//81

	ST_PICK_READY1_READY2,
	ST_PICK_READY1_ERR2,	
	ST_PICK_READY1_EMPTY2,	//84

	ST_PICK_EMPTY1_READY2,
	ST_PICK_EMPTY1_ERR2,	
	ST_PICK_EMPTY1_EMPTY2,	//87

	ST_PICK_ERR1_READY2,
	ST_PICK_ERR1_ERR2,
	ST_PICK_ERR1_EMPTY2,	//90

#ifdef SHUTTLE_REJECT_VAC //20130424
	ST_REQ_SHUTTLE_MOVE_REJ_DIE_POSN,
	ST_ACK_PICK_LSI_FROM_SHUTTLE_TO_FLIPPER,
	ST_LSI_TO_BIN_DONE,
	ST_REQ_PICK_REJ_DIE,
#endif
#ifdef SHUTTLE_PR_ERR_FLIPPER_PICK //20130424 flipper pick PR error die
	ST_PICK_PR_ERR_LSI,
	ST_REQ_PICK_PR_ERR_DIE,
	ST_PICK_FROM_SHUTTLE1,
	ST_PICK_FROM_SHUTTLE2,
	ST_PICK_REJ_FROM_SHUTTLE1,
	ST_PICK_REJ_FROM_SHUTTLE2,
	ST_SEL_TARGET,
#endif
#if 1//def PB_1122
	ST_PR1_READY,
	ST_PR2_READY,
	ST_PR1_ERROR,
	ST_PR2_ERROR,
	ST_REQUEST_UPLOOK_PR1,
	ST_REQUEST_UPLOOK_PR2,
	ST_PB1_PR1_IN_PROCESS,
	ST_PB1_PR2_IN_PROCESS,
	ST_PB2_PR1_IN_PROCESS,
	ST_PB2_PR2_IN_PROCESS,
	ST_PR1_IN_PROCESS,
	ST_PR2_IN_PROCESS,
#endif
	ST_WAIT_PBWH_UPLOOK, //cof mod
	ST_PBWH_AT_PBUPLOOK1_PR1,
	ST_PBWH_AT_PBUPLOOK1_PR2,
	ST_PBWH_AT_PBUPLOOK2_PR1,
	ST_PBWH_AT_PBUPLOOK2_PR2,
	ST_PR_BEGIN,
	ST_BLOW_READY,
	ST_BLOW_COMPLETE,
	ST_PR_FINISH_WITH_ERR,
	ST_PB_BIN_AT_STANDBY, //124
	ST_STOP,				// 125

	ST_BONDING,
	ST_MOD_NOT_SELECTED,	

	ST_TA1_PICK_REQ = 500, //edge cleaner
	ST_TA1_VAC_ON,
	ST_TA1_PICK_GLASS_DONE,
	ST_TA1_PICK_GLASS_ERROR,
	ST_TA1_PICK_GLASS_RETRY, //20130805
	ST_TA1_STOP,

	ST_USER_REJECT,
	ST_TRAY_LOAD_REQUEST,
	ST_TRAY1_LOAD_ACK,
	ST_TRAY2_LOAD_ACK,
	ST_TRAY_TAKE_AWAY_REQUEST,		//510

	ST_TRAY_TAKE_AWAY_ACK,
	ST_TRAY_UNLOAD_REQUEST,
	ST_TRAY1_UNLOAD_ACK,
	ST_TRAY2_UNLOAD_ACK,

	//InSHWH begin
	ST_UNLOAD_READY,		//515
	ST_ACK_REQ_PLACE,
	ST_LOAD_READY,
	ST_REQ_PLACE,
	ST_INSHWH_READY_REMOVE_GLASS,
	ST_INSHWH_READY_TA1_PICK_GLASS,
	ST_INSHWH_PICK_GLASS_DONE,
	//InSHWH end

	//Reject ARM
	ST_REJARM_BUSY,
	ST_REJARM_REMOVE_GLASS_READY,
	ST_REJARM_REMOVE_GLASS_DONE,
	ST_REJARM_REMOVE_GLASS_FAIL,

	ST_READY_3,

	// 20140318 Yip
	ST_WAIT_PB_REJECT_1,
	ST_WAIT_PB_REJECT_2,
	ST_WAIT_PB_REJECT_1_2,
	ST_REQ_REJECT_PB_1,
	ST_REQ_REJECT_PB_2,
	ST_REQ_REJECT_PB_1_2,
	ST_REJECT_PB_1,
	ST_REJECT_PB_2,
	ST_REJECT_PB_1_2,
	ST_REQ_REJECT_1,
	ST_REQ_REJECT_2,
	ST_REQ_REJECT_1_2,
	ST_REJECT,
	ST_REJECT_1,
	ST_REJECT_2,
	ST_REJECT_1_2,
	ST_REQ_COLLECT_1,
	ST_REQ_COLLECT_2,
	ST_REQ_COLLECT_1_2,
	ST_COLLECT_1,
	ST_COLLECT_2,
	ST_COLLECT_1_2,
	ST_COLLECTED_1,
	ST_COLLECTED_2,
	ST_COLLECTED_1_2,
	ST_BUSY_1,
	ST_BUSY_2,
	ST_BUSY_1_2,
	ST_PLACED,
	ST_PREBOND_1,
	ST_PREBOND_2,
	ST_PICKED,

	// 20140815 Yip
	ST_REQ_CLICK_PR,
	ST_ACK_CLICK_PR,
	ST_RELEASE,

	ST_ACK_TA4_PR,	// 20141031 Yip
	ST_UNLOAD_REQUEST,

	ST_ACFWH1_REQ_LOAD,
	ST_ACFWH2_REQ_LOAD,
	ST_ACFWH1_ACK_LOAD,
	ST_ACFWH2_ACK_LOAD,
	ST_ACFWH1_REQ_UNLOAD,
	ST_ACFWH2_REQ_UNLOAD,
	ST_ACFWH1_ACK_UNLOAD,
	ST_ACFWH2_ACK_UNLOAD,	

	ST_REQ_PR,
	ST_ACFWH1_REQ_PR_ACK,
	ST_ACFWH2_REQ_PR_ACK,
	ST_PR_ERROR,
	ST_CHECK_GLASS,
	ST_ACFWH1_REQ_ACF_ACK,
	ST_ACFWH2_REQ_ACF_ACK,
	ST_ACFWH1_DONE,
	ST_ACFWH2_DONE,
	ST_PB_PR_IN_PROCESS


} STATUS_TYPE;


////// Please copy status from GC902
typedef enum
{
	ST_GW_CWH_IDLE = 0, 

	ST_GW_REQ_TA1_VAC_CHECK = 500,
	ST_GW_REQ_TA1_VAC1_VAC2_ON,
	ST_GW_REQ_TA1_VAC1_ON,
	ST_GW_REQ_TA1_VAC2_ON,

	ST_GW_CWH_STOP, //504

	ST_GW_CWH_MOD_NOT_SELECTED,	
///////////////////////////////////////////////////////////////////
	ST_GW_NEXT_MACHINE_READY = 600, // Check Next machine
	ST_GW_NEXT_MACHINE_NOT_READY,
///////////////////////////////////////////////////////////////////

	ST_GW_CWH_MAX,


	ST_GW_INSHWH_IDLE = 700,

	ST_GW_TA5_PLACE_REQ = 800,
	ST_GW_TA5_VAC_OFF,
	ST_GW_INSHWH_VAC_ON,
	ST_GW_INSHWH_PLACE_GLASS_DONE,
	ST_GW_INSHWH_PLACE_GLASS_ERROR,
	ST_GW_INSHWH_PLACE_GLASS_RETRY, //20130805
	ST_GW_INSHWH_STOP,				

	ST_GW_INSHWH_MOD_NOT_SELECTED,	

	ST_GW_INCONV_IDLE = 900,
	ST_GW_INCONV_BUSY,
	ST_GW_INCONV_STOP,

	ST_GW_TA5_G1_G2_NOR = 1000, //NOR=Normal, EMP = Empty, ERR = Error
	ST_GW_TA5_G1_NOR_G2_EMP,
	ST_GW_TA5_G1_NOR_G2_ERR,

	ST_GW_TA5_G1_G2_EMP,
	ST_GW_TA5_G1_EMP_G2_NOR,
	ST_GW_TA5_G1_EMP_G2_ERR,

	ST_GW_TA5_G1_G2_ERR,
	ST_GW_TA5_G1_ERR_G2_NOR,
	ST_GW_TA5_G1_ERR_G2_EMP,

	

	ST_GW_READY,
	ST_GW_STOP,

	ST_GW_TA5_MAX,

} GATEWAY_STATUS_TYPE;
//typedef enum
//{
//	ST_GW_INSHWH_IDLE = 499,
//
//	ST_GW_TA5_PLACE_REQ = 500,
//	ST_GW_INSHWH_VAC_ON,
//	ST_GW_INSHWH_PLACE_GLASS_DONE,
//	ST_GW_INSHWH_PLACE_GLASS_ERROR,
//	ST_GW_INSHWH_PLACE_GLASS_RETRY, //20130805
//	ST_GW_INSHWH_STOP,				
//
//	ST_GW_INSHWH_MOD_NOT_SELECTED,	
/////////////////////////////////////////////////////////////////////
//	ST_GW_NEXT_MACHINE_READY = 600, // Check Next machine
//	ST_GW_NEXT_MACHINE_NOT_READY,
/////////////////////////////////////////////////////////////////////
//	ST_GW_INCONV_IDLE = 900,
//	ST_GW_INCONV_BUSY,
//	ST_GW_INCONV_STOP,
//
//	ST_GW_TA5_G1_G2_NOR = 1000,//NOR=Normal, EMP = Empty, ERR = Error
//	ST_GW_TA5_G1_NOR_G2_EMP,
//	ST_GW_TA5_G1_NOR_G2_ERR,
//
//	ST_GW_TA5_G1_G2_EMP,
//	ST_GW_TA5_G1_EMP_G2_NOR,
//	ST_GW_TA5_G1_EMP_G2_ERR,
//
//	ST_GW_TA5_G1_G2_ERR,
//	ST_GW_TA5_G1_ERR_G2_NOR,
//	ST_GW_TA5_G1_ERR_G2_EMP,
//
//	ST_GW_TA5_MAX,
//} GATEWAY_STATUS_TYPE;

typedef struct
{
	BOOL bIsExist;
	BOOL bToRemove;
} COG902_Glass_Info;

typedef enum
{
	L_GLASS = 0,
	R_GLASS,
	MAX_GLASS,
} COG902_Num_Glass;


typedef enum
{
	NETWORK_GOOD,
	NETWORK_NIL,
	NETWORK_CONNECTING,
	NETWORK_MAX_LIMIT,

} GATEWAY_CHECK_CONNECTION;

////// Please copy status from GC902

typedef enum
{
	SS_ERROR = -1, 
	SS_NOT_READY = 0, 
	SS_READY = 1, 

} STANDBY_SIGNAL;

typedef enum
{
	GLASS_INPUT_MIN = -1, 
	GLASS_1_2_INPUT = 0,
	GLASS_1_INPUT, 
	//GLASS_2_INPUT,
	GLASS_INPUT_MAX,

} GLASS_INPUT_MODE;

typedef enum //20150302
{
	MB_SELECT_AUTO = 0, 
	MB_SELECT_MB12,
	MB_SELECT_MB34,
	MB_SELECT_MAX,

} MB_SELECT_MODE;

//Motor Info. Related
typedef struct
{
	char		*cHomePortAddrss;
	INT			nHomeMask;
	char		*cNegLmtPortAddrss;
	INT			nNegLmtMask;
	char		*cPosLmtPortAddrss;
	INT			nPosLmtMask;

}	MOTOR_SENSOR;

typedef struct
{
	char				*cSettlingCtrlType;
	short				sSettliingTime;

}	CTRL_PROFILE;

typedef struct
{
	//Profile String
	SHORT				sMotionProfID;
	char				*cMotionCtrlType;

	DOUBLE				dJerk;
	DOUBLE				dAcc;
	DOUBLE				dDec;
	DOUBLE				dVel;

}	MOTION_PROFILE;

typedef struct
{
	DOUBLE				dSrchVel;		// mm/s		(The value would be transformed in Get/Set Search Profile Functions)
	DOUBLE				dSrchLimit;		// mm		(The value would be transformed in Get/Set Search Profile Functions)
	DOUBLE				dDriveInVel;	// mm/s		(The value would be transformed in Get/Set Search Profile Functions)
	DOUBLE				dDriveIn;		// mm		(The value would be transformed in Get/Set Search Profile Functions)
	short				sDebounce;		// confirm the state after number of continuous samples when snr is cut

} SRCH_PROF_PARA;

typedef struct
{
	BOOL				bEnable;
	SHORT				sSrchProfID;
	char				*cSrchCtrlType;

	// Srch Input Related
	SHORT				sSrchDirection;
	BOOL				bSetPosn;
	LONG				lDesiredPosn;
	BOOL				bCheckError;

	// Srch Profile Related
	GMP_EVT_TYPE		enSrchType;		// Setup SrchType, e.g. HomeSnr, Indexor, SW Port, e.t.c
	char				*cPort;
	unsigned int		unSensorMask;
	BOOL				bAstate;
	GMP_EVT_LMT_TYPE	enLimitType;	// Srch by distance lmt or Time Lmt

	// Srch Move Related
	SRCH_PROF_PARA		stProf;
	
}	SRCH_PROFILE;

typedef struct
{
	CString				szAxis;
	DOUBLE				dEncoderScale;			// count / mm
	DOUBLE				dDistPerCount;			// um / count
	MOTOR_SENSOR		stSensorMapping;		// ?

	SHORT				sPositionLimit;
	SHORT				sDacLimit;
	SHORT				sTimeLimit;

	BOOL				bLmtSnrState;			// Define Limit Snr Active State
	BOOL				bEncoderExist;			// Enable Encoder Fault and SW Position Limit Protection
	BOOL				bLmtSnrExist;			// Enable Limit Snr Protection

	DOUBLE				dDistPerCountAux;			// um / count //20140322
}	MOTOR_ATTRIB;

typedef struct
{
	// Open Comm. Info.
	GMP_COMMUTATION_OPTION	enCommOption;

	SHORT					sNumberOfPoles;
	ULONG					ulCountPerRev;
	// For DirectionalComm
	BOOL					bCommDir;

	// For OpenStationaryComm
	INT						unSlowDownFactor;
	DOUBLE					dAcceptance;
	DOUBLE					dInitialDAC;
	DOUBLE					dIncreamentDAC;
	DOUBLE					dMaximumDAC;
	DOUBLE					dFitError;

	INT						nCommStatus;

}	MOTOR_COMM;

typedef struct
{
	LONG			lPosLmt;			// Positive Limit Posn
	LONG			lNegLmt;			// Negative Limit Posn

}	MOTOR_PROTECTION;



class CMotorInfo
{
private:
	class CAC9000App	*pAppStation;
	BOOL				*pModSelected;
	CString				szChStnName;
	
public:
	CGmpChannel			*pGmpChannel;
	BOOL				bLogMotion;
	LONG				lErrorCode;			// (New) Define Mtr Error Code
	INT					nTCObjectID;		// (New) Define Mtr Error Code
	BOOL				bHomeStatus;		// Check Is Home or not

	LONG				lHMI_CurrPosn;		// Record the current Posn
	BOOL				bHMI_On;			// Record the current Posn
	BOOL				m_bEnableMotorMove; //20150907
	BOOL				bIsServoMotor;


#ifdef SRCH_EMBEDDED_SENSOR //20170810
	CString				szMtrSnrPortName; //20170810
	BOOL				bUseMtrSnrPort; //20170810
	BOOL				bEnableSrchEmbeddedHome;
	BOOL				bEnableSrchEmbeddedIndexer;
#endif	

	MOTOR_PROTECTION	stProtect;
	MOTOR_ATTRIB		stAttrib;
	MOTOR_COMM			stComm;

	MOTION_PROFILE		stMotionProfile;
	CTRL_PROFILE		stCtrlProf;	

	SRCH_PROFILE		stHomeProfile;
	SRCH_PROFILE		stIndexProfile;

	SRCH_PROFILE		stUsrDefProfile[MAX_NUM_OF_SRCH_PROF];

	CMotorInfo();
	CMotorInfo(CGmpChannel *pGmpChannel, CAC9000App *pAppStation, BOOL *pModSelected);
	VOID InitMotorInfo(CString szAxis, BOOL *pModSelected, BOOL bIsServoMotor = TRUE);
	virtual ~CMotorInfo();

#ifdef SRCH_EMBEDDED_SENSOR //20170810
	VOID SetMtrSnrPortName(CString &szInMtrSnrPortName, BOOL bEnableEmbeddedHome, BOOL bEnableEmbeddedIndexer);  //20170810
#endif
	VOID SetGmpChannel(CGmpChannel *pGmpChannel);
	VOID SetAppStation(CAC9000App *pAppStation);
	VOID SetStnName(CString szStnName);
	BOOL IsHardwareSelected();

	VOID InitDefaultMotorInfo();
	// Add me
	CString GetName();
	CString GetChStnName();
	CString GetErrPortID();
	CString GetDACPortID();
	CString GetDrvPortID();
	CString GetEncPortID();
	CString GetCmdPortID();
	CString GetMotorSnsrPortID();
	CString GetChModePortID();

	INT InitMtrAxis();
	
	// Set parameters to CGmpChannel
	INT SelectCtrlPara();
	INT SelectStaticCtrlPara(CString szStaticCtrlPara = "PL_Static");
	INT SetMoveInput(short sMotionProfID = NORMAL_MOT_PROF, short sProcListSaveID = -1);
	INT SetMoveProf();
	INT SetCustMoveProf(DOUBLE dFactor = 1.0); //20150223
	INT SetCustMoveProf(DOUBLE dAccFactor, DOUBLE dDecFactor, DOUBLE dVelFactor, DOUBLE dJerkFactor); //20150223

	INT SetSoftwareLmt();
	INT SetPosition(LONG lPosn);

	INT InitHomeProf();
	INT InitSrchProf(SRCH_PROFILE stSrchProf);
	INT SetSrchProfSi(SHORT	sSrchProfID, SRCH_PROF_PARA stSrchPara);
	INT SetSrchInput(short sMotionProfID = NORMAL_MOT_PROF, short sProcListSaveID = -1);

	INT SetProcessBlk(short				sProcListSaveID, 
					  unsigned short		usBlkNum, 
					  unsigned short		usNextBlk, 
					  GMP_LOGIC			enLogic, 
					  const char			*szInputPort, 
					  unsigned int		unInputPattern, 
					  unsigned int		unInputMask,
					  const char			*szOutputPort, 
					  unsigned int		unOutputPattern, 
					  unsigned int		unOutputMask);

	INT SetProcessList(short				sProcListSaveID, 
					   byte				ucSize,
					   unsigned short		*p_usStartProcBlkSaveIDList);

	INT EnableProtection();
	INT EnableProtection(GMP_PROT_TYPE enProtectionType, BOOL bTriggerState);
	INT DisableProtection(GMP_PROT_TYPE enProtectionType);

	//INT PowerOn(BOOL bMode);
	INT PowerOn(BOOL bMode, BOOL bCheckAlreadyCommHome = TRUE);
	BOOL IsPowerOn();

	INT Commutate(GMP_COMMUTATION_OPTION enCommMode = GMP_COMMUTATION_OPTION_NONE);

	INT ClearError();

	INT Sync();
	INT SetTCState(BOOL isOn); //20120829
	BOOL IsMotionCompleted(BOOL bIsForceTCLog = FALSE); //20120901

	INT MoveRelative(LONG lMtrCnt, BOOL bWait, BOOL bCheckModSelect = TRUE);
	INT MoveAbsolute(LONG lMtrCnt, BOOL bWait);
	INT OpenDAC(LONG lHoldDAC, LONG lEndDAC, LONG lHoldTime, LONG lRampUpTime, BOOL bWait);

	INT Search(SRCH_PROFILE stSrchProf, short sProcListSaveID = NO_PROCESS_LIST, BOOL bWait = GMP_WAIT, BOOL bIsCheckHomed = TRUE);
#ifdef SRCH_EMBEDDED_SENSOR //20170810
	INT SearchEmbededSnr(EMBEDED_SRCH_ENUM emSrchSnr, SRCH_PROFILE stSrchProf, BOOL bWait = GMP_WAIT);
#endif

#if 1 //20120927
	INT GetIOPortValue(CString szPortName);
#endif
	INT GetEncPosn();
	INT GetCmdPosn();

#ifdef SRCH_EMBEDDED_SENSOR //20170810
	INT GetHomeSnrState(GMP_BOOLEAN *pbState); //20170810 get home sensor state inside Encoder plug
	INT GetLmtSnrState(GMP_BOOLEAN *pbState);	 // get Limit sensor state inside Encoder plug
	INT GetIndexSnrState(GMP_BOOLEAN *pbState); // get Indexer state inside Encoder plug
#endif

	// Operation
	INT Home(BOOL bWait);

	INT MTRTest();
	INT	ModifySWProtection(BOOL bEnable); //20140129
	INT MotorAbort();
	INT MotorStop();
};

class CSiInfo
{
private:
	class CAC9000App	*pAppStation;
	CString				szChStnName;

protected:
	CString			szName;
	CGmpSiPort		*pGmpPort;
	CString			szPortName;
	unsigned int	unMask;
	BOOL			bActiveState;

public:
	BOOL			bHMI_Status;

	CSiInfo();
	CSiInfo(CString szName, CString szPortName, unsigned int unMask, BOOL bActiveState);
	virtual ~CSiInfo();

	VOID			SetGmpPort(CGmpSiPort *pGmpPort);
	CGmpSiPort		&GetGmpPort();
	CString			GetName();
	CString			GetPortName();
	unsigned int	GetMask();
	BOOL			GetActiveState();
	BOOL			IsOn();
	BOOL			IsHardwareSelected();
};

class CSoInfo
{
private:
	class CAC9000App	*pAppStation;
	CString				szChStnName;

protected:
	CGmpSoPort		*pGmpPort;
	CString			szName;
	CString			szPortName;
	unsigned int	unMask;
	BOOL			bActiveState;

	BOOL			bLogMotion;
	INT				nTCObjectID;

	LONG			lOnDelay;
	LONG			lOffDelay;

	BOOL			bIsMachineData;

public:
	BOOL			bHMI_Status;

	CSoInfo();
	CSoInfo(CString szName, CString	szPortName, unsigned int unMask, INT nTCObjectID, BOOL bActiveState, BOOL bIsMachineData);
	CSoInfo(CString szName, CString	szPortName, unsigned int unMask, INT nTCObjectID, BOOL bActiveState);	
	virtual ~CSoInfo();

	VOID SetGmpPort(CGmpSoPort *pGmpPort);

	CGmpSoPort		&GetGmpPort();
	CString			GetName();
	CString			GetPortName();
	unsigned int	GetMask();
	BOOL			GetActiveState();

	BOOL			IsHardwareSelected();

	INT GetTCObjectID();
	BOOL IsMachineData();


	VOID SetOnDelay(LONG lDelay);
	VOID SetOffDelay(LONG lDelay);

	LONG GetOnDelay();
	LONG GetOffDelay();

	INT SetSol(BOOL bMode, BOOL bWait, BOOL bDiagMode = FALSE);
};

#if 1 //20150423
typedef struct
{
	CString szPURName;
	DWORD dwCmdStartTime;
	DWORD dwCmdFinishTime;

	DWORD dwRpy1StartTime;
	DWORD dwRpy1FinishTime;

	DWORD dwRpy2StartTime;
	DWORD dwRpy2FinishTime;

	DWORD dwRpy3StartTime;
	DWORD dwRpy3FinishTime;

	DWORD dwOverallTime;
}
PRDebugInfo;
#endif

#ifdef EXCEL_OFFLINE_SETUP
typedef struct
{
	//glass
	DOUBLE dGlassWidth; //mm
	DOUBLE dGlassLength; //mm
	DOUBLE dITOWidth; //mm

	//Thickness
	DOUBLE dReflectorThickness; //mm
	DOUBLE dUpperGlassThickness; //mm
	DOUBLE dLowerGlassThickness; //mm
	DOUBLE dPolarizerThickness; //mm

	DOUBLE dACFLength; //mm
	DOUBLE dACFWidth; //mm

	DOUBLE dCOFLength; //mm
	DOUBLE dCOFWidth; //mm
	DOUBLE dCOFThickness; //mm

	DOUBLE dTA1PickPlaceRetryLimit;
	DOUBLE dTA2PickPlaceRetryLimit;
	DOUBLE dTA3PickPlaceRetryLimit;
	DOUBLE dTA4PickPlaceRetryLimit;
	DOUBLE dTA5PickPlaceRetryLimit;

	DOUBLE dTA4PRRetryLimit;
	DOUBLE dTA4RejectLimit;

	DOUBLE dINWHPRRetryLimit;
	DOUBLE dINWHRejectLimit;

	DOUBLE dACFInspRetryLimit;
	DOUBLE dACFInspRejectLimit;

} EXCEL_OFFLINE_GENERAL_INFO;

typedef struct
{
	BOOL bDataValid;
	DOUBLE dXDir; //Direction sign of Excel file and PR -1.0 or 1.0. 1.0. When they are same direction, value is 1.0.
	DOUBLE dYDir;
	DOUBLE dPR1PosnX;
	DOUBLE dPR1PosnY;
	DOUBLE dPR2PosnX;
	DOUBLE dPR2PosnY;

	DOUBLE dAP1PosnX;
	DOUBLE dAP1PosnY;
	DOUBLE dAP2PosnX;
	DOUBLE dAP2PosnY;
} EXCEL_OFFLINE_INWH_INFO;

typedef struct
{
	BOOL bDataValid;
	DOUBLE dXDir; //Direction sign of Excel file and PR -1.0 or 1.0. 1.0. When they are same direction, value is 1.0.
	DOUBLE dYDir;

	DOUBLE dPR1PosnX;
	DOUBLE dPR1PosnY;
	DOUBLE dPR2PosnX;
	DOUBLE dPR2PosnY;

	DOUBLE dAP1PosnX;
	DOUBLE dAP1PosnY;
	DOUBLE dAP2PosnX;
	DOUBLE dAP2PosnY;
} EXCEL_OFFLINE_PBWH_INFO;

typedef struct
{
	EXCEL_OFFLINE_GENERAL_INFO stGeneralInfo;
	EXCEL_OFFLINE_INWH_INFO stINWHInfo;
	EXCEL_OFFLINE_PBWH_INFO stPBWHInfo;
} EXCEL_OFFLINE_INFO;

typedef enum
{
	OFFLINE_MIN = -1,
	//General
	OFFLINE_GLASS_WIDTH,
	OFFLINE_GLASS_LENGTH,
	OFFLINE_GLASS_ITOWIDTH,

	OFFLINE_GLASS_REFLECTOR_THICKNESS_MM,
	OFFLINE_GLASS_UPPER_GLASS_THICKNESS_MM,
	OFFLINE_GLASS_LOWER_GLASS_THICKNESS_MM,
	OFFLINE_GLASS_POLARIZER_THICKNESS_MM,

	OFFLINE_ACF_LENGTH_MM,
	OFFLINE_ACF_WIDTH_MM,

	OFFLINE_COF_LENGTH_MM,
	OFFLINE_COF_WIDTH_MM,
	OFFLINE_COF_THICKNESS_MM,

	OFFLINE_TA1_PICKPLACE_RETRY_LMT,
	OFFLINE_TA2_PICKPLACE_RETRY_LMT,
	OFFLINE_TA3_PICKPLACE_RETRY_LMT,
	OFFLINE_TA4_PICKPLACE_RETRY_LMT,
	OFFLINE_TA5_PICKPLACE_RETRY_LMT,
	OFFLINE_TA4_RR_RETRY_LMT,
	OFFLINE_TA4_REJECT_LMT,
	OFFLINE_INWH_PR_RETRY_LMT,
	OFFLINE_INWH_REJECT_LMT,
	OFFLINE_ACF_INSP_RETRY_LMT,
	OFFLINE_ACF_INSP_REJECT_LMT,
	//ACF
	//INWH
	OFFLINE_INWH_PR1_X_POSN_UM,
	OFFLINE_INWH_PR1_Y_POSN_UM,
	OFFLINE_INWH_PR2_X_POSN_UM,
	OFFLINE_INWH_PR2_Y_POSN_UM,
	OFFLINE_INWH_AP1_X_POSN_UM,
	OFFLINE_INWH_AP1_Y_POSN_UM,
	OFFLINE_INWH_AP2_X_POSN_UM,
	OFFLINE_INWH_AP2_Y_POSN_UM,

	//PBWH
	OFFLINE_PBWH_PR1_X_POSN_UM,
	OFFLINE_PBWH_PR1_Y_POSN_UM,
	OFFLINE_PBWH_PR2_X_POSN_UM,
	OFFLINE_PBWH_PR2_Y_POSN_UM,
	OFFLINE_PBWH_AP1_X_POSN_UM,
	OFFLINE_PBWH_AP1_Y_POSN_UM,
	OFFLINE_PBWH_AP2_X_POSN_UM,
	OFFLINE_PBWH_AP2_Y_POSN_UM,

	OFFLINE_MAX
} OFFLINE_INPUT_ITEM;

typedef struct
{
	OFFLINE_INPUT_ITEM emIdx;
	char *pName;				//error name
	char *pExcelName;		//excel item name
	DOUBLE dMax;
	DOUBLE dMin;
	DOUBLE *pdValue;
} OFFLINE_INPUT_DATA;
#endif

class CPortInfo
{
protected:
	CGmpDACPort			*pGmpPort;
	class CAC9000App	*pAppStation;

	CString			szName;
	CString			szPortName;
	unsigned int	unMask;

public:
	CPortInfo();
	CPortInfo(CString szName, CString	szPortName, unsigned int unMask);
	~CPortInfo();

	VOID SetGmpPort(CGmpDACPort *pGmpPort);
	VOID SetAppStation(CAC9000App *pAppStation);

	CGmpDACPort &GetGmpPort();
	CString	GetName();
	CString GetPortName();
	unsigned int GetMask();

	INT SetDACValue(DOUBLE dValue);
	ULONG GetDACValue();
};

class CAC9000Stn : public CAppStation
{
	DECLARE_DYNCREATE(CAC9000Stn)
	friend class CAC9000App;
	friend class CWinEagle;
//	friend class CACFWH;

private: //klocwork fix 20120222
	BOOL	m_bKlocworkTrue;

protected:
	CStringMapFile	*m_psmfMachine;
	CStringMapFile	*m_psmfDevice;

protected:
	//State Operation
	virtual	VOID	IdleOperation();
	virtual	VOID	DiagOperation();
	virtual	VOID	InitOperation();
	virtual	VOID	PreStartOperation();
	virtual	VOID	AutoOperation();
	virtual	VOID	DemoOperation();
	virtual VOID	CycleOperation();
	virtual	VOID	ManualOperation();
	virtual	VOID	ErrorOperation();
	virtual	VOID	StopOperation();
	virtual VOID	DeInitialOperation();

	//Error
	virtual BOOL	SetAlert(UINT unCode, const CString &szMessage = "", BOOL bLightAndSound = TRUE);
	virtual BOOL	SetError(UINT unCode, const CString &szMessage = "");
	virtual BOOL	SetSolError(const CString &szMessage);	// 20140707 Yip
	virtual BOOL	SetMotorError(const CString &szMessage);	// 20140707 Yip
	virtual BOOL	CloseDisplayAlert(UINT unCode);


	//Signalling
	virtual BOOL	Signal(INT nSignal);

	//Update Profile
	VOID UpdateSearchProfile(const CString &szAxis, const CString &szProfile);
	VOID UpdateMotionProfile(const CString &szAxis, const CString &szProfile);
	VOID UpdatePosUnit(const CString &szAxis, const CString &szPos);

	//Register Variables
	virtual VOID RegisterVariables();

protected: // Can by changed by friend only
	LONG	m_lStnErrorCode;			// IDS_xxxxx_NOT_SELECTED_ERR
	LONG	m_lModSelectMode;				// For Check rework station Level;

	BOOL	m_bInitInstanceError;		// Only used at InitInstance

	BOOL	m_bCheckMotor;
	BOOL	m_bCheckIO;

	BOOL	m_bModSelected;
	BOOL	m_bShowModSelected;
	//BOOL	m_bPRSelected;
	//BOOL	m_bCORCalibrated[MAX_EDGE_OF_GLASS];
	BOOL	m_bModError;
	BOOL	m_bModuleNotMoveDuringAutoBurnIn; //20150907 effective in burnIn and diagnose bond
	//BOOL	m_bPRError;
	BOOL	m_bType1Exist;
	BOOL	m_bType2Exist;
	BOOL	m_bGlass1Exist;
	BOOL	m_bGlass2Exist;
	BOOL	m_bIsOfflineMode;
	BOOL	m_bIsBusy;
	
	//// Delays
	//LONG	m_lPRDelay;
	//LONG	m_lCalibPRDelay;
	//LONG	m_lPRErrorLimit; //20141003

	LONG	m_lDiagnSteps;

	//Seq
	BOOL	m_bPressCycle;
	BOOL	m_bStopCycle;

	CArray<CMotorInfo*> m_myMotorList;

	CArray<CSiInfo*> m_mySiList;

	CArray<CSoInfo*> m_mySoList;

	std::deque <CString> m_deqDebugSeq; //20131010
	LONG	m_lThreadID;
	BOOL	HMI_bDebugSeq;
	BOOL	HMI_bDebugCal;
	BOOL	HMI_bLogBondLevel;
	BOOL	HMI_bLogMotion;
	BOOL	HMI_bLogPRAction;

private:
	LONG	m_lStationID;

protected:
	CLogFile *LogFile;

public:
	CAC9000Stn();
	virtual	~CAC9000Stn();
	virtual	BOOL	InitInstance();
	virtual	INT		ExitInstance();

protected:
	// ------------------------------------------------------------------------
	// Variable delcaration
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	// Function delcaration
	// ------------------------------------------------------------------------
	INT	SleepWithReturn(DWORD dwMilliseconds);

	//TimeChart Logging
	VOID TC_AddJob(INT emTCObjectID);
	VOID TC_DelJob(INT emTCObjectID);
	VOID TC_ResetTime();

	//Message Windows and Logging
	VOID WriteHMIMess(const CString &szMess, BOOL bLog = FALSE, BOOL bLogTime = TRUE);
	LONG HMIMessageBox(const INT nBoxType, const CString &szTitle, const CString &szText);
	LONG HMISelectionBox(const CString &szTitle, const CString &szText, const CString &szItemText1 = NULL, const CString &szItemText2 = NULL, const CString &szItemText3 = NULL, const CString &szItemText4 = NULL, const CString &szItemText5 = NULL, const CString &szItemText6 = NULL, const CString &szItemText7 = NULL, const CString &szItemText8 = NULL, const CString &szItemText9 = NULL, const CString &szItemText10 = NULL);
	LONG HMIConfirmWindow(const CString &szFirstbtn, const CString &szSecondbtn, const CString &szThirdbtn, const CString &szTitle, const CString &szText);
	BOOL HMIGetPassword(const CString &szTitle, const CString &szPassword);
	BOOL HMIAlphaKeys(const CString &szPrompt, CString *pszKeyIn, BOOL bKeyInPassword = FALSE);
	BOOL HMINumericKeys(const CString &szPrompt, const DOUBLE dMaxValue, const DOUBLE dMinValue, DOUBLE *pdValue);
	BOOL HMINumericKeys(const CString &szPrompt, const DOUBLE lMaxValue, const DOUBLE lMinValue, LONG *plValue);

	// 20140619 Yip
	VOID UpdateDebugSeq(const CString &szSeq);
	VOID UpdateDebugSeq(const CString &szSeq, const STATUS_TYPE &stStatus);
	VOID UpdateDebugSeq(const CString &szSeq, const STATUS_TYPE &stStatus, const CString &szRelatedStatus, const LONG &lRelatedStatus);
	VOID UpdateDebugSeq(const CString &szSeq, const STATUS_TYPE &stStatus, const STATUS_TYPE &stGlassStatus);
	VOID UpdateDebugSeq(const CString &szSeq, const STATUS_TYPE &stStatus, const STATUS_TYPE stGlassStatus[]);
	VOID UpdateDebugSeq(const CString &szSeq, const STATUS_TYPE &stStatus, BOOL bExist);
	VOID UpdateDebugSeq(const CString &szSeq, const STATUS_TYPE &stStatus, BOOL bExist1, BOOL bExist2);

public:
		
	BOOL	IsInitInstanceError();
	BOOL	IsBusy();
	BOOL	IsModSelected();
	BOOL	IsPRSelected();
	BOOL	IsOfflineMode();
	BOOL	IsPressCycle();
	BOOL	IsBurnInDiagBond();


	const CString &GetStnName() const 
	{ 
		return m_szStationName; 
	}
#if 0 //klocwork fix 20121211
	CMapElement &GetDeviceMap() 
	{ 
		return (*m_psmfDevice)[m_szStationName]; 
	}
	CMapElement &GetMachineMap() 
	{ 
		return (*m_psmfMachine)[m_szStationName]; 
	}
#endif
	CArray<CMotorInfo*> &GetmyMotorList()	
	{
		return m_myMotorList; 
	}
	CArray<CSiInfo*> &GetmySiList()	
	{ 
		return m_mySiList; 
	}
	CArray<CSoInfo*> &GetmySoList()	
	{ 
		return m_mySoList; 
	}

	virtual BOOL GetGlass1Status() 
	{
		return m_bGlass1Exist;
	}
	virtual BOOL GetGlass2Status() 
	{
		return m_bGlass2Exist;
	}

	virtual VOID PrintMachineSetup(FILE *fpf);
	virtual VOID PrintCalibSetup(FILE *fp);
	virtual VOID PrintForceCalibData(FILE *fp);
	virtual VOID PrintDeviceSetup(FILE *fp);		
	virtual VOID PrintPRSetup(FILE *fp);
	virtual VOID PrintPRCalibSetup(FILE *fp);
	VOID PrintDebugSeq(FILE *fp);
#ifdef EXCEL_MACHINE_DEVICE_INFO
	virtual BOOL printMachineDeviceInfo();
#endif

	// Saving / loading parameters
	virtual LONG IPC_SaveMachineParam();
	virtual LONG IPC_LoadMachineParam();
	virtual LONG IPC_SaveDeviceParam();
	virtual LONG IPC_LoadDeviceParam();


protected:
	VOID	SetBusy(BOOL bMode);
	VOID	SetOfflineMode(BOOL bMode);
	virtual VOID SetModSelected(BOOL bMode);
	VOID	CheckModSelected(BOOL bMode);
	LONG	GetModSelectMode();
	VOID	SetStationID(LONG lStationID);
	LONG	GetStationID();

	//HMI Function
	LONG HMI_SetModSelected(IPC_CServiceMessage &svMsg);
	LONG HMI_HomeAll(IPC_CServiceMessage &svMSg);

	LONG HMI_SetDebugSeq(IPC_CServiceMessage &svMsg);
	LONG HMI_SetDebugCal(IPC_CServiceMessage &svMsg);
	LONG HMI_SetLogMotion(IPC_CServiceMessage &svMsg);

	VOID	setKlocworkTrue(BOOL bValue); //klocwork fix 20121211
	BOOL	getKlocworkTrue();
	BOOL	getKlocworkFalse();
	//virtual CMotorInfo &GetMotorZ(WHZNum lWHZ = WHZ_NONE); //whz rotary linear calib

	// 20140729 Yip
	virtual VOID UpdateModuleSetupPosn();
	virtual VOID UpdateModuleSetupLevel();
	virtual VOID UpdateModuleSetupPara();
	VOID ModuleSetupSetPosn(LONG lSetupTitle, const CString &szAxis, LONG lOldPosn, LONG lNewPosn);
	VOID ModuleSetupSetPosn(LONG lSetupTitle, MTR_POSN mtrOldPosn, MTR_POSN mtrNewPosn);
	VOID ModuleSetupSetLevel(LONG lSetupTitle, LONG lOldLevel, LONG lNewLevel);
	BOOL ModuleSetupSetPara(PARA stPara);

	// 20140729
	VOID SetAndLogParameter(const CString &szFunction, const CString &szParameter, LONG &lParameter, LONG lNewValue);
	VOID SetAndLogParameter(const CString &szFunction, const CString &szParameter, ULONG &ulParameter, ULONG ulNewValue);
	VOID SetAndLogParameter(const CString &szFunction, const CString &szParameter, DOUBLE &dParameter, DOUBLE dNewValue);
	VOID SetAndLogParameter(const CString &szFunction, const CString &szParameter, BOOL &bParameter, BOOL bNewValue);
	VOID LogParameter(const CString &szFunction, const CString &szParameter, const CString &szOldValue, const CString &szNewValue);
};
