/////////////////////////////////////////////////////////////////
//	AC9000.cpp : Defines the class behaviors for the application.
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

#include "stdafx.h"
#include "direct.h"
#define AC9000_EXTERN
#include "AC9000.h"
#include "MainFrm.h"
#include "AC9000Stn.h"
#include "AC9000Doc.h"
#include "AC9000View.h"
#include "MarkConstant.h"
#include "AC9000_Constant.h"
#include "HardwareDlg.h"
#include "OptionDlg.h"
#include "OptionPasswordDlg.h" //20130916
#include "AppStation.h"
#include "WorkHolder.h"
#include "TransferArm.h"
#include "InPickArm.h"
#include "TA1.h"
#include "TA2.h"
#include "ACF.h"
#include "ACFWH.h"
#include "ACF1.h"
#include "ACF1WH.h"
#include "ACF2WH.h"
//#include "Inspection.h"
//#include "Insp1.h"
//#include "Insp2.h"
#include "TAManager.h"
#include "HouseKeeping.h"
#include "WinEagle.h"
#include "TempDisplay.h"
#include "SettingFile.h"
#include "TimeChart.h"
#include "LitecCtrl.h"
#include "AdamCtrl.h"

#include "InPickArm.h"
#include "InspOpt.h"

#include "AC9000Stn.h"
#include "GI_System.h"

#include "PRTaskStn.h"

#include "PR_Util.h"
//#include "Cal_Util.h"
#include "Logging_Util.h"
#include "ExcelMotorInfo_Util.h"
#include "BasicExcel.h"
#include "FileOperations.h"
#include "ExcelGlassLsiInfo_Util.h"

#include "Psapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////
//	Constant Definitions
/////////////////////////////////////////////////////////////////



const CString gszCOMPANY_NAME			= _T("ASM Assembly Automation Ltd");
const CString gszSOFTWARE_VERSION		= _T("1.00.01");
const CString gszMACHINE_MODEL			= _T("AOC");
const CString gszSOFTWARE_NUMBER		= _T("10-301538");
const CString gszRELEASE_DATE			= _T("Wed, Aug 16, 2017");

// Define the Registry's Sections
const CString gszPROFILE_SETTING		= _T("Settings");
const CString gszPROFILE_GROUP_ID		= _T("Group Id");
const CString gszPROFILE_AUTO_START		= _T("Auto Start");
const CString gszPROFILE_LOG_MSG		= _T("Log Message");
const CString gszPROFILE_DIR_PATH		= _T("Directory Path");
const CString gszPROFILE_HW_CONFIG		= _T("Hardware Config");
const CString gszPROFILE_EXECUTE_HMI	= _T("Execute HMI");
const CString gszLOG_PATH_SETTING		= _T("Msg Log Path");
const CString gszLOG_FILE_SETTING		= _T("Msg Log File");
const CString gszLOG_BACKUP_SETTING		= _T("Total Backup Files");
const CString gszLOG_FILE_LINE			= _T("Total Number of Lines");
const CString gszSELECT_MC_VER1			= _T("M/C Ver1");
const CString gszSELECT_MC_VER2			= _T("M/C Ver2");
const CString gszSELECT_MC_VER3			= _T("M/C Ver3");
const CString gszSELECT_LITEC			= _T("Select LITEC");
const CString gszSELECT_ADAM			= _T("Select ADAM");
const CString gszSELECT_HEATER_ON		= _T("Select Heater On");	// 20140905 Yip

const CString gszENABLE_ALL_HARDWARE	= _T("Enable All Hardware");
const CString gszENABLE_PRS				= _T("Enable PRS");
const CString gszENABLE_PREBOND_MACH	= _T("Enable PreBond Machine");
const CString gszENABLE_MAINBOND_MACH	= _T("Enable MainBond Machine");
const CString gszENABLE_TEMP_CTRL		= _T("Enable TempCtrl");
const CString gszENABLE_ACF1WH			= _T("Enable ACF1WH");
const CString gszENABLE_ACF2WH			= _T("Enable ACF2WH");
const CString gszENABLE_ACF1			= _T("Enable ACF1");
const CString gszENABLE_TA1				= _T("Enable TA1");
const CString gszENABLE_TA2				= _T("Enable TA2");
const CString gszENABLE_InPickArm		= _T("Enable InPickArm");
const CString gszENABLE_InspOpt			= _T("Enable InspOpt");
const CString gszENABLE_DIAGN_MODE		= _T("Enable DiagnMode");

const CString gszMACHINE_PATH_SETTING	= _T("Machine File Path");
const CString gszDEVICE_PATH_SETTING	= _T("Device File Path");
const CString gszMACHINE_FILE_SETTING	= _T("Machine File");
const CString gszDEVICE_FILE_SETTING	= _T("Device File");

const CString gszMACHINE_ID				= _T("Machine ID");
const CString gszOS_IMAGE				= _T("OS Image Number");

const CString gszVISION_OS_IMAGE		= _T("Vision OS Image Number");

const CString gszCONFIG_EXT				= _T(".cfg");
const CString gszCONFIG_BACK_EXT		= _T(".bcf");

// Define default Registry settings' value
const CString gszMODULE_HW_FILE			= _T("AC9000Hardware");
const CString gszDEFAULT_GROUP_ID		= _T("AC9000");

//Log File
const CString gszLOG_FILE_PATH			= _T("D:\\sw\\AC9000\\LogFileDir");
const CString gszLOG_FILE_NAME			= _T("AC9000MsgLog");

//Application Name
const CString gszAC9000	= _T("AC9000");

//Mark IDE Data File Path
const CString gszMARK_DATA_PATH			= _T("D:\\sw\\AC9000\\MarkData\\");
/*const*/ 
CString gszMARK_DATA_FILE_NAME	= _T("AC9000.dat");

//SCF Files
const CString gszSCF_FLIE_PATH			= _T("D:\\sw\\AC9000\\MarkData\\SCFFile\\AC9000.ini");
const CString gszSCF_ACTI_PATH			= _T("D:\\sw\\AC9000\\MarkData\\SCFFile\\Activation.ini");

const CString gszSCF_FLIE_PATH_2			= _T("D:\\sw\\AC9000\\MarkData\\SCFFile\\AC9000_ver2.ini");
const CString gszSCF_ACTI_PATH_2			= _T("D:\\sw\\AC9000\\MarkData\\SCFFile\\Activation_ver2.ini");

const CString gszSCF_FLIE_PATH_3			= _T("D:\\sw\\AC9000\\MarkData\\SCFFile\\AC9000_ver3.ini");
const CString gszSCF_ACTI_PATH_3			= _T("D:\\sw\\AC9000\\MarkData\\SCFFile\\Activation_ver3.ini");

//Machine and Device File
const CString gszMACHINE_FILE_PATH		= _T("D:\\sw\\AC9000\\Param");
const CString gszDEVICE_FILE_PATH		= _T("D:\\sw\\AC9000\\Device");
const CString gszMACHINE_FILE_NAME		= _T("MachineParam.prm");
const CString gszDEVICE_FILE_NAME		= _T("DeviceParam.prm");

#ifdef EXCEL_OFFLINE_SETUP
const CString gszOFFLINE_SETUP_FILE_PATH		= _T("D:\\sw\\AC9000\\OfflineSetup");
extern GLASS_INFO			g_stGlass;
#endif

const CString gszMACHINE_ID_NO			= _T("---");
const CString gszOS_IMAGE_NO			= _T("---");

const CString gszOS_VISION_IMAGE_NO		= _T("---");

// Define the IPC Service Commands
const CString gszHARDWARE_INITIAL		= _T("Initialize");
const CString gszSYSTEM_INITIAL			= _T("SystemInit");
const CString gszDIAG_COMMAND			= _T("Diagnostic");
const CString gszAUTO_COMMAND			= _T("AutoCycle");
const CString gszMANUAL_COMMAND			= _T("Manual");
const CString gszCYCLE_COMMAND			= _T("CycleTest");
const CString gszDEMO_COMMAND			= _T("Demonstration");
const CString gszSTOP_COMMAND			= _T("Stop");
const CString gszRESET_COMMAND			= _T("Reset");
const CString gszDE_INITIAL_COMMAND		= _T("DeInitialize");
const CString gszJOG_COMMAND			= _T("Jog");
const CString gszGET_DEVICE				= _T("GetDevice");
const CString gszUPDATE_DEVICE			= _T("UpdateDevice");
const CString gszCHANGE_DEVICE			= _T("ChangeDevice");
const CString gszDELETE_DEVICE			= _T("DeleteDevice"); //20120703

const CString gszNEW_DEVICE				= _T("NewDevice");
const CString gszSAVE_AS_DEVICE			= _T("SaveAsDevice");
const CString gszPRINT_DEVICE			= _T("PrintDevice");


const CString gszGET_MACHINE			= _T("GetMachine");
const CString gszUPDATE_MACHINE			= _T("UpdateMachine");
const CString gszPRINT_MACHINE			= _T("PrintMachine");

BEGIN_MESSAGE_MAP(CAC9000App, SFM_CModule)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////
//Construction
/////////////////////////////////////////////////////////////////

CAC9000App::CAC9000App()
{

	HMI_lSWLimitEncoder			= 0; //whz rotary linear calib

	m_fExecuteHmi				= FALSE;
	m_szState					= gszMODULE_STATE[UN_INITIALIZE_Q];
	m_qState					= UN_INITIALIZE_Q;
	m_nOperation				= NULL_OP;
	m_lAction					= glNULL_COMMAND;
	m_dwStopTime				= 0;
	m_bStatusLogged				= TRUE; //20150819
	m_bRunState					= FALSE;
	m_fDeleteMarkIDE			= FALSE;

	m_fEnableInitInstanceError	= TRUE;
	m_fSelectVer1				= FALSE;
	m_fSelectVer2				= FALSE;
	m_fSelectVer3				= FALSE;

	m_fSelectLitec				= TRUE;
	m_fSelectADAM				= TRUE;
	m_fSelectHeaterOn			= FALSE;	// 20140905 Yip
	m_fSelectCOG902				= TRUE;
	m_bAdamExit					= FALSE;

	m_fEnableHardware			= TRUE;
	m_fEnablePRS				= TRUE;


	m_szMachineModel			= gszMACHINE_MODEL;
	m_szMachineID				= "---";
	m_szOSImageNumber			= "10-M00207V1-00-00";
	m_szVisionOSImageNumber		= "---";
	m_szLitecVersion1			= "---";
	m_szLitecVersion2			= "---";
	m_szLitecVersion3			= "---";
	m_szLitecVersion4			= "---";
	m_szSoftwareNumber			= gszSOFTWARE_NUMBER;
	m_szSoftVersion				= gszSOFTWARE_VERSION;
	m_szReleaseDate				= gszRELEASE_DATE;

	m_bHWInitCheckingPressure	= TRUE;
	m_bHWInitFatalError			= FALSE; //20130226
	m_bHWInitFatalErrorUI		= FALSE;

	m_isAppRunning				= FALSE; //20111214
	m_szMotionLibVersion		= "1.0a"; //20121210
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_bPerformanceReportEnable	= TRUE;	// 20140730 Yip: Enable Performance Report
	m_bSaveLoadDeviceMachine = FALSE; //20150824

	m_ulFreeSpaceMB_C = 0; //20150810
	m_ulFreeSpaceMB_D = 0;
#ifdef EXCEL_OFFLINE_SETUP
	m_szOfflineSetupPath = _T("D:\\sw\\AC9000\\OfflineSetup\\");
	m_szOfflineSetupFile = _T("test.xls");

	g_stOfflineInfo.stINWHInfo.bDataValid = FALSE;
	g_stOfflineInfo.stINWHInfo.dXDir = -1.0;
	g_stOfflineInfo.stINWHInfo.dYDir = -1.0;

	g_stOfflineInfo.stPBWHInfo.bDataValid = FALSE;
	g_stOfflineInfo.stPBWHInfo.dXDir = -1.0;
	g_stOfflineInfo.stPBWHInfo.dYDir = 1.0;
#endif

	//HMI_bIsEdgeCleaner			= FALSE;
	HMI_bIsCOG902				= FALSE;
	HMI_lGatewayStatus			= NETWORK_NIL; //0=gnAMS_OK, 1=gnAMS_NOTOK, 2=connecting

	ZeroMemory(&m_stGatewayProcessInfo, sizeof(m_stGatewayProcessInfo));

	setKlocworkTrue(TRUE); //klocwork fix 20121211
	InitErrorAlertTable(); //20141124

	//cal whz
	HMI_bTA1Z_Selected = FALSE;

	HMI_bACF1WHZ_Selected = FALSE;
	HMI_bACF2WHZ_Selected = FALSE;

	SetHeapInfo(); //20151005
}


/////////////////////////////////////////////////////////////////
// The one and only CAC9000App object
/////////////////////////////////////////////////////////////////

CAC9000App theApp;

/////////////////////////////////////////////////////////////////
//	Station Threads
/////////////////////////////////////////////////////////////////
//CInConv				*pCInConv;
CTA1				*pCTA1;
CTA2				*pCTA2;
CACF1				*pCACF1;
CACF1WH				*pCACF1WH;
CACF2WH				*pCACF2WH;
//CInsp1				*pCInsp1;
//CInsp2				*pCInsp2;
CTAManager			*pCTAManager;
CHouseKeeping		*pCHouseKeeping;
CWinEagle			*pCWinEagle;
CTempDisplay		*pCTempDisplay;
CSettingFile		*pCSettingFile;
CTimeChart			*pCTimeChart;
CLitecCtrl			*pCLitecCtrl;
CAdamCtrl			*pCAdamCtrl;
CInPickArm			*pCInPickArm;
CInspOpt			*pCInspOpt;

#ifdef EXCEL_MACHINE_DEVICE_INFO
BasicExcelWorksheet		*g_pExcelMachineInfoSheet = NULL;
BasicExcelWorksheet		*g_pExcelDeviceInfoSheet = NULL;
LONG					g_lMachineInfoRowNum = 0;
LONG					g_lDeviceInfoRowNum = 0;
#endif

/////////////////////////////////////////////////////////////////
// CAC9000App initialization
/////////////////////////////////////////////////////////////////

BOOL CAC9000App::InitInstance()
{
	HANDLE hMutex; //20120614
	// Create mutex, because there cannot be 2 instances of the same application
	hMutex = CreateMutex(NULL, FALSE, gszAC9000); 

	// Check if mutex is created succesfully
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//Mutex already exists so there is a running instance of our app.
		MessageDlg dlg;
		//TRACE("### Mutex already exists so there is a running instance of our app.\n");
		dlg.DoModal();
		return false;
	}

#if 1 //20130712
	{
		ULONG ulFreeSpaceMB = 0;
		struct _diskfree_t df = {0};
		if (_getdiskfree((INT)DRIVE_D, &df) == 0)
		{
			ulFreeSpaceMB = df.avail_clusters * df.sectors_per_cluster / 1024 * df.bytes_per_sector / 1024;
			m_ulFreeSpaceMB_D = ulFreeSpaceMB;

			if (ulFreeSpaceMB < 500)
			{
				CString szMsg(" ");
				szMsg.Format("D-Drive free space is %luMB. Free Space is less than 500MB. Please Free More Disk Space for the D-Drive.", ulFreeSpaceMB);
				//HMIMessageBox(MSG_OK, "D-Drive Free Space", szMsg);
				AfxMessageBox((char*)szMsg.GetBuffer(0));
			}
		}

		if (_getdiskfree((INT)DRIVE_C, &df) == 0)
		{
			ulFreeSpaceMB = df.avail_clusters * df.sectors_per_cluster / 1024 * df.bytes_per_sector / 1024;
			m_ulFreeSpaceMB_C = ulFreeSpaceMB;

			if (ulFreeSpaceMB < 500)
			{
				CString szMsg(" ");
				szMsg.Format("C-Drive free space is %luMB. Free Space is less than 500MB. Please Free More Disk Space for the C-Drive.", ulFreeSpaceMB);
				//HMIMessageBox(MSG_OK, "D-Drive Free Space", szMsg);
				AfxMessageBox((char*)szMsg.GetBuffer(0));
			}
		}
	}
#endif


	InitCommonControls();

	SFM_CModule::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		CloseHandle(hMutex);	// 20140801 Yip: Klocwork Fix
		return FALSE;
	}
	AfxEnableControlContainer();

#if 1
	if (m_bPerformanceReportEnable)
	{
		HANDLE hFile;
		hFile = CreateFile("D:\\sw\\AC9000\\Release\\performance_report_config.csv", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL); //20130109

		if (hFile == INVALID_HANDLE_VALUE)		//Philip add
		{
			//use default setting
			SaveSettingReportConfig("8,\nAAA,\nBBB,\nCCC,\nDDD,\nEEE,\nFFF,\nGGG,\nHHH,\nIII,\nJJJ,\nKKK,\nLLL,\nMMM,\nNNN,\nOOO,\nPPP,\nQQQ,\nRRR,\nSSS,\nTTT,\nUUU,\nVVV,\nWWW,\nXXX,");
			LoadSettingReportConfig();
		}
		else
		{
			LoadSettingReportConfig();
		}
		CloseHandle(hFile);


		g_pPerformance_report = new CPerformanceReport("D:\\sw\\AC9000\\Release\\performance_report_config.csv");	//input param: shift config file path
		if (g_pPerformance_report != NULL)
		{
			g_pPerformance_report->StartNewReport();    //create a new report
		}		
	}
#endif

	INT nTemp;
	SetRegistryKey(gszCOMPANY_NAME);

	//AutoStart
	nTemp = GetProfileInt(gszPROFILE_SETTING, gszPROFILE_AUTO_START, 0);
	nTemp ? m_fAutoStart = TRUE : m_fAutoStart = FALSE;

	// Initialize the hardware if auto-start is enabled
	if (m_fAutoStart)
	{
		m_bRunState = TRUE;
	}

	m_qState = UN_INITIALIZE_Q;

	//Log File
	nTemp = GetProfileInt(gszPROFILE_SETTING, gszPROFILE_LOG_MSG, 1);
	nTemp = 1; //20150818 force ON
	nTemp ? m_fLogMessage = TRUE : m_fLogMessage = FALSE;

	//Execute Hmi
	nTemp = GetProfileInt(gszPROFILE_SETTING, gszPROFILE_EXECUTE_HMI, 0);
	nTemp = 1; //20150818 force ON
	SetExecuteHmiFlag(nTemp);

	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate *pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAC9000Doc),
		RUNTIME_CLASS(CMainFrame), // main SDI frame window
		RUNTIME_CLASS(CAC9000View));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
	{
		return FALSE;
	}

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

	//Create InitOperation
	m_pInitOperation	= new CInitOperation;

	//Event
	m_evStart.ResetEvent();

	//Group ID
	m_szGroupID	= GetProfileString(gszPROFILE_SETTING, gszPROFILE_GROUP_ID, gszDEFAULT_GROUP_ID);

	//Retrieve Tot Log File Backup
	nTemp = GetProfileInt(gszPROFILE_SETTING, gszLOG_BACKUP_SETTING, 50); //Default values, 5 Backup files
	if (nTemp <= 5) //20131004
	{
		nTemp = 50;
	}
	SetTotLogBackupFiles(nTemp); 

	//Total Number of line for Log File 
	nTemp = GetProfileInt(gszPROFILE_SETTING, gszLOG_FILE_LINE, 50000); //Default values, 50000 lines
	SetTotLogFileLine(nTemp); 

	// Retrieve Message Log File Path
	if ((m_szLogFilePath = GetProfileString(gszPROFILE_SETTING, gszLOG_PATH_SETTING, _T(""))) == _T(""))
	{
		m_szLogFilePath = gszLOG_FILE_PATH;
		WriteProfileString(gszPROFILE_SETTING, gszLOG_PATH_SETTING, gszLOG_FILE_PATH);
	}

	if ((m_szLogFile = GetProfileString(gszPROFILE_SETTING, gszLOG_FILE_SETTING, _T(""))) == _T(""))
	{
		m_szLogFile = gszLOG_FILE_NAME;
		WriteProfileString(gszPROFILE_SETTING, gszLOG_FILE_SETTING, gszLOG_FILE_NAME);
	}

	m_pInitOperation->SaveLogFilePath(m_szLogFilePath);
	m_pInitOperation->SaveLogFile(m_szLogFile);

	// Retrieve Machine path
	if ((m_szMachinePath = GetProfileString(gszPROFILE_SETTING, gszMACHINE_PATH_SETTING, _T(""))) == _T(""))
	{
		m_szMachinePath = gszMACHINE_FILE_PATH;
		WriteProfileString(gszPROFILE_SETTING, gszMACHINE_PATH_SETTING, gszMACHINE_FILE_PATH);
	}

	// Retrieve Device path
	if ((m_szDevicePath = GetProfileString(gszPROFILE_SETTING, gszDEVICE_PATH_SETTING, _T(""))) == _T(""))
	{
		m_szDevicePath = gszDEVICE_FILE_PATH;
		WriteProfileString(gszPROFILE_SETTING, gszDEVICE_PATH_SETTING, gszDEVICE_FILE_PATH);
	}
	
	// Retrieve Machine File
	if ((m_szMachineFile = GetProfileString(gszPROFILE_SETTING, gszMACHINE_FILE_SETTING, _T(""))) == _T(""))
	{
		m_szMachineFile = gszMACHINE_FILE_NAME;
		WriteProfileString(gszPROFILE_SETTING, gszMACHINE_FILE_SETTING, gszMACHINE_FILE_NAME);
	}

	// Retrieve Default Device File
	if ((m_szDeviceFile = GetProfileString(gszPROFILE_SETTING, gszDEVICE_FILE_SETTING, _T(""))) == _T(""))
	{
		m_szDeviceFile = gszDEVICE_FILE_NAME;
		WriteProfileString(gszPROFILE_SETTING, gszDEVICE_FILE_SETTING, gszDEVICE_FILE_NAME);
	}

	// Retrieve Machine ID
	if ((m_szMachineID = GetProfileString(gszPROFILE_SETTING, gszMACHINE_ID, _T(""))) == _T(""))
	{
		m_szMachineID = gszMACHINE_ID_NO;
		WriteProfileString(gszPROFILE_SETTING, gszMACHINE_ID, gszMACHINE_ID_NO);
	}

	// Retrieve OS Image Number
	if ((m_szOSImageNumber = GetProfileString(gszPROFILE_SETTING, gszOS_IMAGE, _T(""))) == _T(""))
	{
		m_szOSImageNumber = gszOS_IMAGE_NO;
		WriteProfileString(gszPROFILE_SETTING, gszOS_IMAGE, gszOS_IMAGE_NO);
	}

	// Retrieve OS Image Number
	if ((m_szVisionOSImageNumber = GetProfileString(gszPROFILE_SETTING, gszVISION_OS_IMAGE, _T(""))) == _T(""))
	{
		m_szVisionOSImageNumber = gszOS_VISION_IMAGE_NO;
		WriteProfileString(gszPROFILE_SETTING, gszVISION_OS_IMAGE, gszOS_VISION_IMAGE_NO);
	}

	CSingleLock slLock(&m_csDispMsg);
	slLock.Lock();
	m_fCanDisplay = TRUE;
	slLock.Unlock();

	// Backup the Message Log (if program crashes in last run)
	m_pInitOperation->RenameLogFile();
	// Create the Message Log file used for debugging
	m_fLogFileCreated = m_pInitOperation->CreateLogFile();

	m_fSelectVer1				= GetProfileInt(gszPROFILE_HW_CONFIG, gszSELECT_MC_VER1, 1);
	m_fSelectVer2				= GetProfileInt(gszPROFILE_HW_CONFIG, gszSELECT_MC_VER2, 0);
	m_fSelectVer3				= GetProfileInt(gszPROFILE_HW_CONFIG, gszSELECT_MC_VER3, 0);
	m_fSelectLitec				= GetProfileInt(gszPROFILE_HW_CONFIG, gszSELECT_LITEC, 1);
	m_fSelectADAM				= GetProfileInt(gszPROFILE_HW_CONFIG, gszSELECT_ADAM, 1);
	m_fSelectHeaterOn			= GetProfileInt(gszPROFILE_HW_CONFIG, gszSELECT_HEATER_ON, 0);	// 20140905 Yip
	//m_fSelectCOG902				= GetProfileInt(gszPROFILE_HW_CONFIG, gszSELECT_COG902, 1);

	//Mark IDE Data Path
	LoadData();

	//Retrieve EnableHardware Option
	//Note that EnableAllHardware() should be called after LoadData();
	m_fEnableHardware			= GetProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_ALL_HARDWARE, 1);
	m_fEnablePRS				= GetProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_PRS, 1);
	m_fEnablePreBondMach		= GetProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_PREBOND_MACH, 1);
	m_fEnableMainBondMach		= GetProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_MAINBOND_MACH, 1);
	m_fEnableTempCtrl			= GetProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_TEMP_CTRL, 1);
	m_fEnableTA1				= GetProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_TA1, 1);
	m_fEnableTA2				= GetProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_TA2, 1);
	m_fEnableACF1WH				= GetProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_ACF1WH, 1);
	m_fEnableACF2WH				= GetProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_ACF2WH, 1);
	m_fEnableACF1				= GetProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_ACF1, 1);
	m_fEnableInPickArm			= GetProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_InPickArm, 1);
	m_fEnableInspOpt			= GetProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_InspOpt, 1);
	m_fEnableDiagnMode			= GetProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_DIAGN_MODE, 1);

	m_bHWInitError				= FALSE;
	m_bHWInitFatalError			= FALSE; //20130226
	m_bHWInitFatalErrorUI		= FALSE;
	m_bHWInited					= FALSE;

	m_bAllLitecInited			= FALSE;
	m_bAllAdamInited			= FALSE;

	m_bInvalidInitState			= FALSE;
	m_bInitInstanceError		= FALSE;

	// Station commutated flags
	m_bTA1Comm					= FALSE;	
	m_bTA1Home					= FALSE;
	m_bTA2Comm					= FALSE;
	m_bTA2Home					= FALSE;
	m_bACF1Comm					= FALSE;
	m_bACF1Home					= FALSE;
	m_bACF1WHComm				= FALSE;
	m_bACF1WHHome				= FALSE;
	m_bACF2WHComm				= FALSE;
	m_bACF2WHHome				= FALSE;
	m_bInPickArmComm			= FALSE;
	m_bInPickArmHome			= FALSE;
	m_bInspOptComm				= FALSE;
	m_bInspOptHome				= FALSE;
	
	m_bDiagnMode				= FALSE;

	// Diagnostic testing
	bPRRepeatTest				= FALSE;

	// Message windows
	m_szHMIMess					= "The Machine is initialing";
	m_bHMIMess					= FALSE;

	// statistics
	m_ulGlassPicked				= 0;
	m_ulTotalGlassPicked		= 0;

	m_ulGlassRejected						= 0;
	m_ulTotalGlassRejected					= 0;
	m_ulInvalidGlass						= 0;
	m_ulTotalInvalidGlass					= 0;
	m_ulACFWHPBPRError						= 0;
	m_ulTotalACFWHPBPRError					= 0;

	m_ulGlassTolError			= 0;
	m_ulTotalGlassTolError		= 0;
	m_ulAngCorrError			= 0;
	m_ulTotalAngCorrError		= 0;

	m_ulCPAPickError			= 0;
	m_ulTotalCPAPickError		= 0;

	m_ulUnitsBonded				= 0;
	m_ulTotalUnitsBonded		= 0;

//p20120829
	m_lStatisticsLogCount		= 0;
//p20120829:end

	m_ulUnitsToBond				= 0;					// User Insert number of unit to bond in Test Bond Mode;
	m_ulUnitsToBondCounter		= 0;	

	m_dUPH						= 0.0;

	m_ulLangageCode				= 0;		// English

	dDummy						= 0.0;
	stDummy						= "";

	HMI_szMotorDirectionPng		= "";
	HMI_lMotorPosLmt			= 0;
	HMI_lMotorNegLmt			= 0;

#if 1 //20130916
	m_smfMachine.Close(); //Clear File buffer
	if (!m_smfMachine.Open(m_szMachinePath + "\\" + m_szMachineFile, FALSE, TRUE))
	{
		AfxMessageBox("Can not load File \"" + m_szMachinePath + "\\" + m_szMachineFile + "\"");
	}
#endif
	m_lThreadID = GetCurrentThreadId(); //20150706

	return TRUE;
}

int CAC9000App::ExitInstance() 
{
	CString szMsg("");

	if (m_pInitOperation)
	{
		if (m_fLogFileCreated)
		{
			m_pInitOperation->RenameLogFile();
		}

		if (IsInitNuDrive())
		{
			m_pCGmpSystem->FreeSystem();
			DisplayMessage("Nu Drive is uninitialized ...");
		}
		
		delete m_pInitOperation;
		delete m_pCGmpSystem;
	}

	if (g_pPerformance_report != NULL)	// 20140819 Yip: Fix Memory Leak Issue
	{
		delete g_pPerformance_report;
	}

	INT nFileLockFree = 0;
	nFileLockFree = FreeAllFileLock(); //20150615
	szMsg.Format("File Lock Free = %d", nFileLockFree);
	DisplayMessage(szMsg);
	
	m_smfDevice.Close();
	m_smfMachine.Close();

   // Close process and thread handles.
	if (m_stGatewayProcessInfo.hProcess != NULL)
	{
		TerminateProcess(m_stGatewayProcessInfo.hProcess, 0);
		CloseHandle(m_stGatewayProcessInfo.hProcess);
		CloseHandle(m_stGatewayProcessInfo.hThread);
	}

	return SFM_CModule::ExitInstance();
}


/////////////////////////////////////////////////////////////////
// CAC9000App message handlers
/////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////
//Group ID
/////////////////////////////////////////////////////////////////

CString	CAC9000App::GetModuleID()
{
	return m_szModuleName;
}

CString	CAC9000App::GetGroupID()
{
	return m_szGroupID;
}


/////////////////////////////////////////////////////////////////
//Mark IDE Data Loading
/////////////////////////////////////////////////////////////////

VOID CAC9000App::LoadData()
{
	try
	{
		switch (MachineVersion())
		{
		case VERSION_1:
			gszMARK_DATA_FILE_NAME	= _T("AC9000-1.dat");
			DisplayMessage("AC9000-1 MACHINE");
			break;
			
		case VERSION_2:
			gszMARK_DATA_FILE_NAME	= _T("AC9000-1.dat"); //use the same mark file for another machine, because we are using SCF.ini.
			DisplayMessage("AC9000-2 MACHINE");
			break;	

		case VERSION_3:
			gszMARK_DATA_FILE_NAME	= _T("AC9000-1.dat"); //use the same mark file for another machine, because we are using SCF.ini.
			DisplayMessage("AC9000-3 MACHINE");
			break;	
		}

		//Load data
		m_szDataPath	= gszMARK_DATA_PATH + gszMARK_DATA_FILE_NAME;
		m_pInitOperation->LoadData(m_szDataPath);

		m_szModuleName	= m_pInitOperation->GetModuleID();

		if ((m_szGroupID == gszDEFAULT_GROUP_ID)	&& 
			(m_pInitOperation->GetGroupID() != _T("")))
		{
			m_szGroupID	= m_pInitOperation->GetGroupID();
			SaveGroupId(m_szGroupID);
		}
		else if (m_szGroupID == "")
		{
			(m_pInitOperation->GetGroupID() == _T("")) ? m_szGroupID = gszDEFAULT_GROUP_ID :
																	   m_szGroupID	= m_pInitOperation->GetGroupID();

			SaveGroupId(m_szGroupID);
		}

		CAC9000View *pView = (CAC9000View*)((CMainFrame*) m_pMainWnd)->GetActiveView();
		pView->SetGroupID(m_szGroupID);
		pView->SetModule(m_szModuleName);
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
}

DWORD CAC9000App::RunLongTimeFunction(AFX_THREADPROC pfnThreadProc, LPVOID pParam, const CString &szDesc)	// 20140724 Yip
{
	DWORD dwExitCode;

	CWinThread *pThread = AfxBeginThread(pfnThreadProc, pParam, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	pThread->m_bAutoDelete = FALSE;
	pThread->ResumeThread();

	do
	{
		Sleep(100);
		if (GetExitCodeThread(pThread->m_hThread, &dwExitCode) == 0)
		{
			dwExitCode = 0;
			CString szMsg;
			szMsg.Format("%s Error: %ld", szDesc, GetLastError());
			DisplayMessage(szMsg);
		}
		KeepResponse();
	} while (dwExitCode == STILL_ACTIVE);
	delete pThread;

	return dwExitCode;
}

UINT _LoadMotorInfoParam(LPVOID lpVoid)	// 20140715 Yip
{
	BasicExcel *pExcelDB = static_cast<BasicExcel*>(lpVoid);
	CString szPath = _T("D:\\sw\\AC9000\\Param\\AC9000_MotorInfoParam.xls");
	if (theApp.MachineVersion() >= VERSION_2 && theApp.MachineVersion() < VERSION_3)
	{
		szPath = _T("D:\\sw\\AC9000\\Param\\AC9000_MotorInfoParam_ver2.xls");
	}
	else if (theApp.MachineVersion() >= VERSION_3)
	{
		szPath = _T("D:\\sw\\AC9000\\Param\\AC9000_MotorInfoParam_ver3.xls");
	}
	if (pExcelDB->Load(szPath))
	{
		return 1;
	}
	return 0;
}

typedef struct
{
	CGmpSystem *pCGmpSystem;
	char *cFilePath;
	char *cActiPath;
} GMP_INIT;

UINT _InitGmpSystem(LPVOID lpVoid)	// 20140715 Yip
{
	GMP_INIT *pGmpInit = static_cast<GMP_INIT*>(lpVoid);
	try
	{
		pGmpInit->pCGmpSystem->InitSystem(pGmpInit->cFilePath, pGmpInit->cActiPath, 8000);
		return 1;
	}
	catch (CAsmException e)	// 20141111 Yip: Catch Init GMP System Exception
	{
		theApp.DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		theApp.DisplayException(e);
		theApp.DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
	return 0;
}

UINT _SwitchAllCam(LPVOID lpVoid)	// 20140722 Yip
{
	CWinEagle *pCWinEagle = static_cast<CWinEagle*>(lpVoid);
	pCWinEagle->SwitchAllCam();
	return 0;
}

/////////////////////////////////////////////////////////////////
//Hardware Init
/////////////////////////////////////////////////////////////////

BOOL CAC9000App::InitHardware()
{
	BOOL			bInit = FALSE;
	CString			szStn;
	SFM_CStation	*pStn = NULL;
	CAppStation	*pAppStn;

	CString szMsg = "";
	__time64_t ltime;

	try
	{
		//Display Software Ver, Machine model and Release Date
		DisplayMessage("Machine Model: " + m_szMachineModel);
		DisplayMessage("Machine ID: " + m_szMachineID);
		DisplayMessage("Software Version: " + m_szSoftVersion);
		DisplayMessage("Release Date: " + m_szReleaseDate);

		//Setup the IPC comms and Register Variables
		SetupIPC();
		RegisterVariables();

		//Enable/ Disable controller here
		if (!m_fEnableHardware)
		{
			m_pInitOperation->EnableAllHardware(FALSE);
		}

		//Init NuMotion hardware
		//m_pInitOperation->InitializeHipec();
		DisplayMessage("NuMotion Network initialized ...");
		KeepResponse();	// 20140714 Yip

		//Register Station Class
		//ADD_STATION_HERE, Do not remove or modify this comment, it will be used by MARK-ADD-IN to add a Station
		m_pInitOperation->RegisterStnClass("CTA1", RUNTIME_CLASS(CTA1));
		m_pInitOperation->RegisterStnClass("CTA2", RUNTIME_CLASS(CTA2));
		m_pInitOperation->RegisterStnClass("CACF1", RUNTIME_CLASS(CACF1));
		m_pInitOperation->RegisterStnClass("CACF1WH", RUNTIME_CLASS(CACF1WH));
		m_pInitOperation->RegisterStnClass("CACF2WH", RUNTIME_CLASS(CACF2WH));
		m_pInitOperation->RegisterStnClass("CInspOpt", RUNTIME_CLASS(CInspOpt));
		m_pInitOperation->RegisterStnClass("CTAManager", RUNTIME_CLASS(CTAManager));
		m_pInitOperation->RegisterStnClass("CHouseKeeping", RUNTIME_CLASS(CHouseKeeping));
		m_pInitOperation->RegisterStnClass("CWinEagle", RUNTIME_CLASS(CWinEagle));
		m_pInitOperation->RegisterStnClass("CTempDisplay", RUNTIME_CLASS(CTempDisplay));
		m_pInitOperation->RegisterStnClass("CSettingFile", RUNTIME_CLASS(CSettingFile));
		m_pInitOperation->RegisterStnClass("CTimeChart", RUNTIME_CLASS(CTimeChart));
		m_pInitOperation->RegisterStnClass("CLitecCtrl", RUNTIME_CLASS(CLitecCtrl));
		m_pInitOperation->RegisterStnClass("CAdamCtrl", RUNTIME_CLASS(CAdamCtrl));
		m_pInitOperation->RegisterStnClass("CInPickArm", RUNTIME_CLASS(CInPickArm));
_ASSERTE(_CrtCheckMemory());

		//Create Station and Sub Station
		m_pInitOperation->CreateStation();
_ASSERTE(_CrtCheckMemory());
		DisplayMessage("Completed Station setup ...");
		KeepResponse();	// 20140714 Yip
_ASSERTE(_CrtCheckMemory());

		m_Stations.RemoveAll();
		m_PRStations.RemoveAll();

		//Relay to Stn & Sub Stn: Update Pos flag, Profile flag, and m_fHardware
		for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
		{
			m_pStationMap.GetNextAssoc(pos, szStn, pStn);
			pAppStn = dynamic_cast<CAppStation*>(pStn);

			if (pAppStn)
			{
				pAppStn->SetUpdatePosition(m_fEnableHardware, TRUE);
				pAppStn->SetUpdateProfile(m_fEnableHardware, TRUE);
				pAppStn->SetHardware(m_fEnableHardware, TRUE);
				m_Stations.Add((CAC9000Stn*)pAppStn);
			}
		}
_ASSERTE(_CrtCheckMemory());

		//Assign All Station ID 
		for (INT i = 0; i < GetStationList().GetSize(); i++)
		{
			GetStationList().GetAt(i)->SetStationID(i);
		}

		// Assign threads pointers
		pCWinEagle			= dynamic_cast<CWinEagle*>(GetStation("WinEagle"));
		pCTA1				= dynamic_cast<CTA1*>(GetStation("TA1")); 
		pCTA2				= dynamic_cast<CTA2*>(GetStation("TA2")); 
		pCACF1				= dynamic_cast<CACF1*>(GetStation("ACF1"));
		pCACF1WH			= dynamic_cast<CACF1WH*>(GetStation("ACF1WH"));
		pCACF2WH			= dynamic_cast<CACF2WH*>(GetStation("ACF2WH"));
		pCInspOpt			= dynamic_cast<CInspOpt*>(GetStation("InspOpt"));
		pCTAManager			= dynamic_cast<CTAManager*>(GetStation("TAManager"));

		pCSettingFile		= dynamic_cast<CSettingFile*>(GetStation("SettingFile"));
		pCHouseKeeping		= dynamic_cast<CHouseKeeping*>(GetStation("HouseKeeping"));
		pCTimeChart			= dynamic_cast<CTimeChart*>(GetStation("TimeChart"));
		pCTempDisplay		= dynamic_cast<CTempDisplay*>(GetStation("TempDisplay"));
		pCLitecCtrl			= dynamic_cast<CLitecCtrl*>(GetStation("LitecCtrl"));
		pCAdamCtrl			= dynamic_cast<CAdamCtrl*>(GetStation("AdamCtrl"));

		pCInPickArm		= dynamic_cast<CInPickArm*>(GetStation("InPickArm")); 

		// For PRTaskStn
		pCACF1WH->m_pAppMod		= this;
		pCACF2WH->m_pAppMod		= this;
		pCWinEagle->m_pAppMod	= this;
		pCTA1->m_pAppMod	= this;
_ASSERTE(_CrtCheckMemory());

		// For Forward Extern

		pCACF1->m_pCACFWH		= pCACF1WH;
		pCACF1->m_pCOtherACFWH	= pCACF2WH;
		pCACF1WH->m_pCOtherACFWH= pCACF2WH;
		pCACF2WH->m_pCOtherACFWH= pCACF1WH;
		pCACF1WH->m_pCACF		= pCACF1;
		pCACF2WH->m_pCACF		= pCACF1;

		//Add all station that required PR System
		//m_PRStations.Add((CPRTaskStn*)pCInsp1);
		//m_PRStations.Add((CPRTaskStn*)pCInsp2);
		m_PRStations.Add((CPRTaskStn*)pCACF1WH);
		m_PRStations.Add((CPRTaskStn*)pCACF2WH);
		m_PRStations.Add((CPRTaskStn*)pCTA1);
		// set hardware enable flag accordingly
		// Use as Init Temp Control
		if (m_fEnablePreBondMach && m_fEnableMainBondMach)
		{
			MachineType = MACHINE_PBMB;
		}
		else if (!m_fEnablePreBondMach && !m_fEnableMainBondMach)
		{
			MachineType = MACHINE_PBMB;
		}
		else if (m_fEnablePreBondMach)
		{
			MachineType = MACHINE_PB;
		}
		else
		{
			MachineType = MACHINE_MB;
		}
_ASSERTE(_CrtCheckMemory());

		//if (IsInitNuDrive())
		//{			
		pCWinEagle->SetHardware(m_fEnablePRS);
		pCTA1->SetHardware(m_fEnableTA1);
		pCTA2->SetHardware(m_fEnableTA2);
		pCACF1->SetHardware(m_fEnableACF1);
		pCACF1WH->SetHardware(m_fEnableACF1WH);
		pCACF2WH->SetHardware(m_fEnableACF2WH);
		pCInPickArm->SetHardware(m_fEnableInPickArm);

		pCInspOpt->SetHardware(m_fEnableInspOpt);
		pCTempDisplay->SetHardware(m_fEnableTempCtrl);
		pCLitecCtrl->SetHardware(m_fEnableTempCtrl);

		if (IsSelectADAM())
		{
			pCAdamCtrl->SetHardware(m_fEnableTempCtrl);
		}
		KeepResponse();	// 20140714 Yip
_ASSERTE(_CrtCheckMemory());

		if (IsSelectCOG902())
		{
			HMI_bIsCOG902 = TRUE;
		}
		else
		{
			HMI_bIsCOG902 = FALSE;
		}

		m_bDiagnMode = m_fEnableDiagnMode;

		BasicExcel				excelDB;
		BasicExcelWorksheet	*curSheet;
		CString str;
		CString szPath = _T("D:\\sw\\AC9000\\Param\\AC9000_MotorInfoParam.xls");

		DisplayMessage("Loading Motor Info...");
_ASSERTE(_CrtCheckMemory());

		// 20140715 Yip: Create Thread For Loading Motor Info Parameters
		if (RunLongTimeFunction(_LoadMotorInfoParam, static_cast<LPVOID>(&excelDB), "Motor Info Load") == 1)
		{
			for (INT i = 0; i < GetStationList().GetSize(); i++)
			{
				curSheet = excelDB.GetWorksheet(GetStationList().GetAt(i)->GetStnName());
				//only print error when that station do not have motors
				if (curSheet != NULL && GetStationList().GetAt(i)->GetmyMotorList().GetSize() != 0)
				{
					for (INT j = 0; j < GetStationList().GetAt(i)->GetmyMotorList().GetSize(); j++)
					{
						if (!LoadMotorInfo(*curSheet, GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)))
						{
							str.Format("Loading Motor Info Error! %s", GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->stAttrib.szAxis);

							DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
							DisplayMessage(str);
							DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						}
					}
				}
				else if (curSheet == NULL && GetStationList().GetAt(i)->GetmyMotorList().GetSize() != 0)
				{
					str.Format("Loading Motor Info Error! %s", GetStationList().GetAt(i)->GetStnName());
					DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
					DisplayMessage(str);
					DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				}
				KeepResponse();	// 20140714 Yip
			}
			excelDB.Close();
			DisplayMessage("Motor Info Loaded");
		}
		else
		{
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			DisplayMessage("Loading Motor Info Error!");
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		}
		KeepResponse();	// 20140714 Yip
_ASSERTE(_CrtCheckMemory());

		// Initi Nu Drive Here
		m_pCGmpSystem = new CGmpSystem();

		char cFilePath[50];
		char cActiPath[50];

		switch (MachineVersion())
		{
		case VERSION_1:
			strcpy(cFilePath, gszSCF_FLIE_PATH);
			strcpy(cActiPath, gszSCF_ACTI_PATH);
			DisplayMessage("AC9000-1 MACHINE");
			break;
			
		case VERSION_2:
			strcpy(cFilePath, gszSCF_FLIE_PATH_2);
			strcpy(cActiPath, gszSCF_ACTI_PATH_2);
			DisplayMessage("AC9000-2 MACHINE");
			break;

		case VERSION_3:
			strcpy(cFilePath, gszSCF_FLIE_PATH_3);
			strcpy(cActiPath, gszSCF_ACTI_PATH_3);
			DisplayMessage("AC9000-3 MACHINE");
			break;
		}

		{
		//20120612
			UINT uRev = 0, uBuildNum = 0;
			unsigned char cSubRev[10];
			memset(&cSubRev[0], 0, sizeof(unsigned char) * 10);
			m_pCGmpSystem->GetMotionLibRevision(&uRev, &cSubRev[0], &uBuildNum);
			DisplayMessage("**** Gmp Motion library Revision ****");
			str.Format("Revision: %d.%02d %s BuildNum = %d.%02d", uRev / 100, uRev % 100, &cSubRev[0], uBuildNum / 100, uBuildNum % 100);
			DisplayMessage(str);
			DisplayMessage("****************************************");
			m_szMotionLibVersion.Format("%d.%02d %s %d.%02d", uRev / 100, uRev % 100, &cSubRev[0], uBuildNum / 100, uBuildNum % 100); //20121210
		}
_ASSERTE(_CrtCheckMemory());

		if (IsInitNuDrive())
		{
			if (CheckPath(cFilePath)  != PATH_IS_FILE) //20151029
			{
				CString szError("");
				szError.Format("File not Found:%s", cFilePath);
				AfxMessageBox(szError.GetString());
				return FALSE;
			}
			if (CheckPath(cActiPath)  != PATH_IS_FILE) //20151029
			{
				CString szError("");
				szError.Format("File not Found:%s", cFilePath);
				AfxMessageBox(szError.GetString());
				return FALSE;
			}
//			m_pCGmpSystem->InitSystem(cFilePath, cActiPath, 8000);
			DisplayMessage("Initializing Nu System...");
			// 20140715 Yip: Create Thread For Initializing Nu System
			GMP_INIT GmpInit = {m_pCGmpSystem, cFilePath, cActiPath};
			if (RunLongTimeFunction(_InitGmpSystem, static_cast<LPVOID>(&GmpInit), "Init GMP System") != 1)
			{
				return FALSE;	// 20141111 Yip: Catch Init GMP System Exception
			}
		}

		DisplayMessage("Initialize Nu system completed");
		KeepResponse();	// 20140714 Yip
_ASSERTE(_CrtCheckMemory());

		//Load Machine and Device
		LoadSoftwareVersion();  

		BOOL	bLoadFileError = FALSE;

#if 1 //20150224
		CString szPathRecord = _T("");
		szPathRecord = "D:\\sw\\AC9000\\Record\\";
		CreatePath(szPathRecord);
#endif
#ifdef EXCEL_OFFLINE_SETUP
		CreatePath("D:\\sw\\AC9000\\OfflineSetup");
#endif

		//Load Machine and Device File
		if (LoadMachineFile())
		{
			_time64(&ltime);
			szMsg = _ctime64(&ltime);

			szMsg += _T(": Machine file Loaded");
		}
		else
		{
			bLoadFileError = TRUE;
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			DisplayMessage("ERROR: Unable to load Machine file");
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		}
		KeepResponse();	// 20140714 Yip

		if (LoadDeviceFile())
		{
			_time64(&ltime);
			szMsg = _ctime64(&ltime);

			szMsg += _T(": Device file Loaded");
		}
		else
		{
			bLoadFileError = TRUE;
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			DisplayMessage("ERROR: Unable to load Device file");
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		}
		KeepResponse();	// 20140714 Yip

		if (!bLoadFileError)
		{
		
			if (pCHouseKeeping->m_dPrevSWVersion < pCHouseKeeping->m_dCurSWVersion)
			{
				//Load Machine and Device File
				if (LoadMachineFile(FALSE))
				{
					DisplayMessage("Machine Data Convertion Finished");
				}
				else
				{
					bLoadFileError = TRUE;
					DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
					DisplayMessage("ERROR: Unable to save Machine file");
					DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				}
				KeepResponse();	// 20140714 Yip

				if (LoadDeviceFile(FALSE))
				{
					DisplayMessage("Device Data Convertion Finished");
				}
				else
				{
					bLoadFileError = TRUE;
					DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
					DisplayMessage("ERROR: Unable to save Device file");
					DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				}
				KeepResponse();	// 20140714 Yip
			}
		}

		//Save Software
		SaveSoftwareVersion();
		KeepResponse();	// 20140714 Yip

		// Init HMI
		ExecuteHmi();

		m_evStart.SetEvent();
		bInit = TRUE;
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bInit;
}

VOID CAC9000App::SetupIPC()
{
	if (m_fCreated)
	{
		return;
	}

	try
	{
		Create(m_szModuleName, m_szGroupID);
		DisplayMessage("Communication initialized ...");

		// Register Service Commands
		m_comServer.IPC_REG_SERVICE_COMMAND(gszHARDWARE_INITIAL, SrvInitialize);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszSYSTEM_INITIAL, SrvSystemInitialize);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszDIAG_COMMAND, SrvDiagCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszAUTO_COMMAND, SrvAutoCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszMANUAL_COMMAND, SrvManualCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszCYCLE_COMMAND, SrvCycleCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszDEMO_COMMAND, SrvDemoCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszSTOP_COMMAND, SrvStopCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszRESET_COMMAND, SrvResetCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszDE_INITIAL_COMMAND, SrvDeInitialCommand);

		//Jog Mode
		m_comServer.IPC_REG_SERVICE_COMMAND(gszJOG_COMMAND, SrvJogCommand);
		
		//Device
		m_comServer.IPC_REG_SERVICE_COMMAND(gszUPDATE_DEVICE, SrvUpdateDevice);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszCHANGE_DEVICE, SrvChangeDevice);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszNEW_DEVICE, SrvNewDevice);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszPRINT_DEVICE, SrvPrintDevice);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszDELETE_DEVICE, SrvDeleteDevice); //20120703

		//Machine
		m_comServer.IPC_REG_SERVICE_COMMAND(gszUPDATE_MACHINE, SrvUpdateMachine);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszPRINT_MACHINE, SrvPrintMachine);

#ifdef EXCEL_OFFLINE_SETUP
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadOfflineSetupFile"), HMI_LoadOfflineSetupFile);
#endif
		// Additional Service Commands registration here

		DisplayMessage("IPC Services registered ...");
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
}


/////////////////////////////////////////////////////////////////
// Display Msg & Exception
/////////////////////////////////////////////////////////////////

VOID CAC9000App::DisplayException(CAsmException &e)
{
	CString	szException, szMsg, szCode;
	CString	szMessage = "	EXCEPTION ENCOUNTERED\n";

	//e.What(szException);

	szMsg = m_szModuleName + _T(" - EXCEPTION ENCOUNTER!");
	DisplayMessage(szMsg);

	e.What(szException);
	szMsg.Format("  FUNCTION:   %s", e.Function());
	DisplayMessage(szMsg);

	szMsg.Format("  PARAMETER:  Error Code Number = %d", e.ErrorID());
	DisplayMessage(szMsg);

	if (e.Parameter() != "")
	{
		szMsg.Format("  ERROR TYPE: %s", e.Parameter());
		DisplayMessage(szMsg);
	}
}

VOID CAC9000App::DisplayMessage(const CString &szMessage)
{
	CSingleLock slLock(&m_csDispMsg);

	if (slLock.Lock(300))
	{
		if (m_fLogMessage && m_fCanDisplay)
		{
			if (m_pMainWnd != NULL)
			{
				CAC9000View *pView = (CAC9000View*)((CMainFrame*) m_pMainWnd)->GetActiveView();
			
				if (pView != NULL)
				{
					pView->DisplayMessage(szMessage);

					if (m_fLogFileCreated)
					{
						m_pInitOperation->WriteLogFile(szMessage);
					}
				}
			}
		}
	}

	slLock.Unlock();
}


/////////////////////////////////////////////////////////////////
// CAC9000App Override of SFM_CModule class functions
/////////////////////////////////////////////////////////////////

VOID CAC9000App::Operation()
{
	static INT	nScanCount = 0;

	// Scan Input Variable at about 100 ms interval
	if ((m_qState != UN_INITIALIZE_Q)	&&
		(m_qState != DE_INITIAL_Q)		&&
		++nScanCount >= 10)
	{
		nScanCount = 0;
		ScanInput();
	}

	switch (m_qState)
	{
	case UN_INITIALIZE_Q:
		UnInitialOperation();
		if (g_pPerformance_report != NULL)
		{
			g_pPerformance_report->m_lBondingStart = m_ulTotalUnitsBonded;    //Philip performance report for initializing the total bond
		} 
		break;

	case IDLE_Q:
		IdleOperation();
		break;

	case DIAGNOSTICS_Q:
		DiagOperation();
		break;

	case SYSTEM_INITIAL_Q:
		SystemInitialOperation();
		break;

	case PRESTART_Q:
		PreStartOperation();
		break;

	case CYCLE_Q:
		CycleOperation();
		break;

	case AUTO_Q:
		AutoOperation();
		break;

	case DEMO_Q:
		DemoOperation();
		break;

	case MANUAL_Q:
		ManualOperation();
		break;

	case STOPPING_Q:
		StopOperation();
		break;

	case DE_INITIAL_Q:
		DeInitialOperation();
		break;
	}

	Sleep(10);

	UpdateOutput();
}

VOID CAC9000App::UpdateOutput()
{
	static DWORD dwLastMemoryLogTime = 0;

	m_szState = gszMODULE_STATE[m_qState];

	if ((m_qState == UN_INITIALIZE_Q) ||
		(m_qState == DE_INITIAL_Q))
	{
		return;
	}

	if (
		m_qState != SYSTEM_INITIAL_Q &&
		m_qState != PRESTART_Q
	   )
	{ 
	//20121225
		pCHouseKeeping->HMI_bStopProcess = TRUE;
	}

	if (g_pPerformance_report != NULL)
	{
		if (g_pPerformance_report->m_bIsRunning)											// Machine is running at auto or test mode, 
		{
			g_pPerformance_report->CheckIdleUpdate(m_ulTotalUnitsBonded);    //check if the machine is idle, Idle Definition: no output for over 1 minute
		}						
		else
		{
			g_pPerformance_report->m_lLastUnitBondedTime = clock();    //Reset the clock to current time to avoid idle update 
		}						

		if (g_pPerformance_report->TimeForNextReport())									//check if it is over 24 for next day, next report
		{
			g_pPerformance_report->m_lBondingEnd = m_ulTotalUnitsBonded;								//Record the number of output for the current report (the end point) update for the UPH
			g_pPerformance_report->UpdateReport(m_szDeviceFile);								//Compute and report all the events from last hr
			g_pPerformance_report->m_lBondingStart = m_ulTotalUnitsBonded;							//Record the number of output for the next report (the start point) update for the UPH
			g_pPerformance_report->CopyCurrentReportToHMI(g_ulDailyPerformanceReportPage);		//refresh the report display for user at HMI
			g_pPerformance_report->EndReport();												//close up the current report and open another new report
		}
		else if (g_pPerformance_report->TimeToUpdate())									//check if it is time for reporting the last hr
		{
			g_pPerformance_report->m_lBondingEnd = m_ulTotalUnitsBonded;								//Record the number of output for the current report (the end point) update for the UPH
			g_pPerformance_report->UpdateReport(m_szDeviceFile);								//Compute and report all the events from last hr
			g_pPerformance_report->m_lBondingStart = m_ulTotalUnitsBonded;							//Record the number of output for the next report (the start point) update for the UPH
			g_pPerformance_report->CopyCurrentReportToHMI(g_ulDailyPerformanceReportPage);		//refresh the report display for user at HMI
		}
	}

	if (/*m_bHMIMess && */m_astrMsgQueue.GetSize() > 0) 
	{
		m_csMsgLock.Lock();
		Sleep(200);
		m_szHMIMess = m_astrMsgQueue[0];
		m_astrMsgQueue.RemoveAt(0);
		m_bHMIMess = FALSE;
		m_csMsgLock.Unlock();
		SetHmiVariable("szHMIMess"); //20150618
	}

	if ( !m_bStatusLogged && !CheckStopSuccess())
	{
		DWORD dwTime = GetTickCount() - m_dwStopTime;

		if (dwTime > 30 * 1000) //30 sec
		{
			GetAllStatus(CString(_T("STOP_TIME_OUT after 30Sec")));
			m_bStatusLogged = TRUE;
		}
	}
	
	if (
		(m_qState == IDLE_Q || m_qState == DEMO_Q || m_qState == AUTO_Q || m_qState == CYCLE_Q) &&
		(labs(GetTickCount() - dwLastMemoryLogTime) / CLOCKS_PER_SEC > 1 * 60)
		)
	{
		LogSystemMemoryStatus();
		LogMemoryStatus();
		dwLastMemoryLogTime = GetTickCount();
	}

	// TODO: Add your variables update here

	// restart HMI?
	if (m_fRestartHmi)
	{
		Sleep(5000);
		DisplayMessage("Restarting HMI...");
		ExecuteHmi();
	}
	//read encoder for motor Software Limit page
	if (m_pStMotorSWLimit != NULL)
	{
		HMI_lSWLimitEncoder = m_pStMotorSWLimit->GetEncPosn();
	}
}

VOID CAC9000App::ScanInput()
{
	if ((m_qState == UN_INITIALIZE_Q) ||
		(m_qState == DE_INITIAL_Q))
	{
		return;
	}

	// TODO: Add your own variables scanning here
}

INT CAC9000App::State()
{
	return m_qState;
}


/////////////////////////////////////////////////////////////////
//	State Operation Functions
/////////////////////////////////////////////////////////////////

VOID CAC9000App::UnInitialOperation()
{
	if (m_bRunState)
	{
		//ExecuteHmi();

#ifndef _DEBUG	// 20140722 Yip: Do Not Create Error Log When Start Up For Debug Mode To Save Time
		// Logging
		if (CheckPath("D:\\CreateErrorLog.exe"))
		{
//			system("D:\\CreateErrorLog.exe");
			ExecuteCreateErrorLog();	// 20140714 Yip: Do Not Show Up Command Prompt For CreateErrorLog.exe
			Sleep(500);
		}
#endif
		
		// Check if visionNT.exe is running
		HANDLE hProcessSnap = NULL;
		PROCESSENTRY32 pe32 = {0};
		INT nVisionNTCount	= 0;
		INT nAsmHmiCount	= 0;
		INT nGatewayCount	= 0;

		// Take a snapshot of all processes in the system. 
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (hProcessSnap != INVALID_HANDLE_VALUE)
		{
			// Fill in the size of the structure before using it. 
			pe32.dwSize = sizeof(PROCESSENTRY32);

			// Walk the snapshot of the processes, and for each process, display information.
			if (Process32First(hProcessSnap, &pe32))
			{
				do
				{
					CString strName = pe32.szExeFile;
					strName = strName.MakeUpper();

					if (strName.Find("VISIONNT.EXE") >= 0)
					{
						nVisionNTCount++;
					}

					if (strName.Find("ASMHMI.EXE") >= 0)
					{
						nAsmHmiCount++;
					}

					if (strName.Find("SOCKETGATEWAY.EXE") >= 0)
					{
						nGatewayCount++;
					}

				} while (Process32Next(hProcessSnap, &pe32));
			}
		}
		CloseHandle(hProcessSnap);

		if (nGatewayCount <= 0 && nAsmHmiCount == 0 && nVisionNTCount == 0)
		{
			ExecuteGateway();
		}
		
		if (nVisionNTCount != 0 || nAsmHmiCount != 0 || nGatewayCount != 0)
		{
			if (nVisionNTCount != 0)
			{
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				DisplayMessage("ERROR: VisionNT.exe is running!");
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				DisplayMessage("*** Please terminate VisionNT.exe and restart program ***");
			}

			if (nAsmHmiCount != 0)
			{
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				DisplayMessage("ERROR: AsmHmi.exe is running!");
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				DisplayMessage("*** Please terminate AsmHmi.exe and restart program ***");
			}
			
			if (nGatewayCount != 0)
			{
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				DisplayMessage("ERROR: SocketGateway.exe is running!");
				DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				DisplayMessage("*** Please terminate SocketGateway.exe and restart program ***");
			}
		}
		else if (InitHardware())
		{
			m_qState		= IDLE_Q;
			m_fDeleteMarkIDE = TRUE;
			DisplayMessage(_T("Hardware Initialized - System in IDLE state"));
		}
		else
		{
			DisplayMessage(_T("Hardware fail to Initialize!!!"));
			DisplayMessage(_T("*** Please restart program ***"));
		}

		m_bRunState = FALSE;
	}
}

VOID CAC9000App::IdleOperation()
{
	CString strInitMessage;
	CString szMsg("");

	if (m_fDeleteMarkIDE)
	{
		//Purpose: To Delete all Data Created by MarkIDE
		INT	nCount = 0;

		for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
		{
			CString	szName;
			SFM_CStation *pStation = NULL;
			m_pStationMap.GetNextAssoc(pos, szName, pStation);

			if (!pStation->IsInitialized())
			{
				static CString	szNamePrevStation(" ");
				if (szNamePrevStation != szName)
				{
					DisplayMessage("Waiting for initialization of:" + szName);
					szNamePrevStation = szName;
				}
				return;
			}

			nCount++;
		}

		if (nCount == m_pStationMap.GetCount())
		{
			m_fDeleteMarkIDE = FALSE;
			DisplayMessage(_T("Stations Initialized - Stations in IDLE state"));
			//delete and create a null file
			if (CheckPath("D:\\FRAME IV\\Bin\\MarkIDE.exe"))
			{
				// Delete all the MarkIDE application sw
				RenameDir("D:\\FRAME IV\\Bin\\MarkIDE.exe", "D:\\FRAME IV\\Bin\\ClickMe.bak");
			}
			
			HMILangTraditionalChinese(); //20130227

			if (m_bHWInitFatalError) //20130226
			{
				m_bHWInitFatalErrorUI = TRUE; //20130710
				strInitMessage.Format("Initialization with Fatal Error. Please Restart Program");
				WriteHMIMess(strInitMessage);
				DisplayMessage(_T("*** Initialization with Fatal Error. Please Restart Program***"));

				if (IsInitNuDrive() && !pCHouseKeeping->IsAirSupplyOn())
				{
					SetError(IDS_HK_AIR_SUPPLY_ERR);
				}
				//SetError(IDS_HK_AIR_SUPPLY_ERR);
				return; //No "OK" button at program start. Force user exit program.
			}

			if (m_bHWInitError)
			{
				if (m_bInvalidInitState)
				{
					m_bHWInitFatalErrorUI = TRUE; //20130710
					strInitMessage.Format("*** Please Remove Glass in the Machine and Restart Program***");
					DisplayMessage(_T("*** Please Remove Glass in the Machine and Restart Program***"));
					return;
				}

				strInitMessage.Format("TEST__INIT ERROR!");
				WriteHMIMess(strInitMessage);
				m_bInitInstanceError = TRUE;
				m_bHWInitError = FALSE; // Must Set, otherwise cannot enter UpdateOutput

				if (m_fEnableInitInstanceError)
				{
					for (int i = 0; i < GetStationList().GetSize(); i++)
					{
						if (GetStationList().GetAt(i)->IsInitInstanceError())
						{
							strInitMessage.Format(GetStationList().GetAt(i)->GetStnName() + "__INIT ERROR!");
							WriteHMIMess(strInitMessage);
						}
					}
					
					WriteHMIMess("Please RE-INIT");
					m_bInitInstanceError = TRUE;
					m_bHWInitError = FALSE; // Must Set, otherwise cannot enter UpdateOutput
				}
				else
				{
					m_bHWInitFatalErrorUI = TRUE; //20130710
					// add turn off all motors
					WriteHMIMess("*** Please restart program ***");
					DisplayMessage(_T("*** Please restart program ***"));
					return;
				}
			}

			// move motors to standby
			if (m_bDiagnMode)
			{
				//// Set E/P regulators
				//if (IsInitHipec())
				//{
				//	pCACF->SetDACValue(pCACF->m_dPressureCtrl);
				//}
		
				strInitMessage = "Initalization DONE";
				m_bHWInited = TRUE;
			}
			else
			{
				if (!m_bInitInstanceError)
				{
					strInitMessage = "Initalization DONE";

					if (
						pCInPickArm->MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_NOWAIT) != GMP_SUCCESS ||
						pCTA1->MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_NOWAIT) != GMP_SUCCESS ||
						pCTA2->MoveZ(TA2_Z_CTRL_GO_STANDBY_LEVEL, GMP_NOWAIT) != GMP_SUCCESS ||
						pCInPickArm->m_stMotorZ.Sync() != GMP_SUCCESS ||
						pCTA1->m_stMotorZ.Sync() != GMP_SUCCESS ||
						pCTA2->m_stMotorZ.Sync() != GMP_SUCCESS ||

						pCInPickArm->MoveX(IN_PICKARM_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT) != GMP_SUCCESS ||
						pCTA1->MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT) != GMP_SUCCESS ||
						pCTA2->MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT) != GMP_SUCCESS ||
						pCInPickArm->m_stMotorX.Sync() != GMP_SUCCESS ||
						pCTA1->m_stMotorX.Sync() != GMP_SUCCESS ||
						pCTA2->m_stMotorX.Sync() != GMP_SUCCESS ||

						pCACF1->MoveZ(ACF_Z_CTRL_GO_STANDBY_LEVEL, GMP_NOWAIT) != GMP_SUCCESS ||
						pCACF1->MoveX(ACF_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT) != GMP_SUCCESS ||
						pCACF1->m_stMotorZ.Sync() != GMP_SUCCESS ||
						pCACF1->m_stMotorX.Sync() != GMP_SUCCESS ||

						pCACF1WH->MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_NOWAIT) != GMP_SUCCESS ||
						pCACF2WH->MoveY(ACF_WH_Y_CTRL_GO_STANDBY_POSN, GMP_NOWAIT) != GMP_SUCCESS ||
						pCACF1WH->m_stMotorY.Sync() != GMP_SUCCESS ||
						pCACF2WH->m_stMotorY.Sync() != GMP_SUCCESS ||

						pCACF1WH->MoveT(ACF_WH_T_CTRL_GO_STANDBY, GMP_NOWAIT) != GMP_SUCCESS ||
						pCACF2WH->MoveT(ACF_WH_T_CTRL_GO_STANDBY, GMP_NOWAIT) != GMP_SUCCESS ||
						pCACF1WH->m_stMotorT.Sync() != GMP_SUCCESS ||
						pCACF2WH->m_stMotorT.Sync() != GMP_SUCCESS ||

						pCInspOpt->MoveX(INSPOPT_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != GMP_SUCCESS 

					   )
					{
						strInitMessage = "Initalization DONE (Move Motor To Standby Error)";
					}
				}

				//if (IsInitNuDrive())	//offline 20120302
				//{
				//	pCACF1->SetHeadUpSol(ON, GMP_NOWAIT);
				//}

				// Add me later
				// Motor Move to Standby Posn
				// Move Teflon
				//if (IsInitHipec())
				//{
				//	pCACF->SetDACValue(pCACF->m_dPressureCtrl);
				//}
#if 1 //20141210
				CString szInFilePath("D:\\sw\\AC9000\\LogFileDir\\");
				//DeleteOldFiles(szInFilePath, CString("AC9000MsgLog*.mlg"), 14);	//20141208
				DeleteOldFiles(szInFilePath, CString("AC9000MsgLog*.txt"), 14);
				DeleteOldFiles(szInFilePath, CString("ErrorAlarm_*.csv"), 14);
				DeleteOldFiles(szInFilePath, CString("StationStatus*.log"), 14);

				szInFilePath = "D:\\sw\\AC9000\\Data\\StatisticsLogFile\\";
				DeleteOldFiles(szInFilePath, CString("StatisticsLogFile_*.txt"), 14);

				szInFilePath = "D:\\sw\\AC9000\\Data\\ParameterLog\\";
				DeleteOldFiles(szInFilePath, CString("*.log"), 14);

				szInFilePath = "D:\\sw\\AC9000\\Data\\Calculation Log\\";
				DeleteOldFiles(szInFilePath, CString("*.log"), 14);
				DeleteOldFiles(szInFilePath, CString("*.txt"), 14);

				szInFilePath = "D:\\sw\\AC9000\\Data\\";
				DeleteOldFiles(szInFilePath, CString("Temperature_*.csv"), 14);
				DeleteOldFiles(szInFilePath, CString("Temperature_*.log"), 14);
				DeleteOldFiles(szInFilePath, CString("MemoryUseageLog*.csv"), 14);
#endif

				m_bHWInited = TRUE;
			}

			PrintThreadID(); //20150706

			szMsg.Format("HDD Free Space C: %ldMB, D:%ldMB", m_ulFreeSpaceMB_C, m_ulFreeSpaceMB_D);
			DisplayMessage(szMsg);
#if 1 //20150915
			m_csLogErrorAlertLock.Lock();
			CString szInFilePath2("D:\\sw\\AC9000\\LogFileDir\\");
			CString szInFileName2("ErrorAlarm.csv");
			CheckFileSize(szInFilePath2, szInFileName2, 512); //512KB
			LogErrorAlarm(CString("HOST_PROGRAM_START"), FALSE);
			m_csLogErrorAlertLock.Unlock();
#endif

//			if (g_bIsVisionNTInited && IsInitNuDrive())	// 20140623 Yip
//			{
////				pCWinEagle->SwitchAllCam();
//				// 20140715 Yip: Create Thread For Switching All Cameras
//				RunLongTimeFunction(_SwitchAllCam, static_cast<LPVOID>(pCWinEagle), "Switch All Cameras");
//			}
		}
	}
}

VOID CAC9000App::DiagOperation()
{
	if (CheckStationMotion() == MOTION_COMPLETE)
	{
		DisplayMessage(_T("Diagnostic operation completed"));
		m_qState = IDLE_Q;
	}
}

VOID CAC9000App::SystemInitialOperation()
{
	// Signal all stations to perform prestart operation
	SFM_CStation *pStation = NULL;
	CAppStation  *pAppStation;
	POSITION	  pos;
	CString		  szKey;
	BOOL		  bError = FALSE;

	if (CheckStationMotion() == MOTION_COMPLETE)
	{
		DisplayMessage(_T("Initialization completed"));

		//check for error during Initialization
		for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
		{
			m_pStationMap.GetNextAssoc(pos, szKey, pStation);
			pAppStation = dynamic_cast<CAppStation*>(pStation);

			if (pAppStation->Result() == gnNOTOK)
			{
				bError = TRUE;
				break;
			}
		}

		if ((m_nOperation != NULL_OP) && (bError != TRUE))
		{
			// Signal all stations to perform prestart operation
			for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
			{
				m_pStationMap.GetNextAssoc(pos, szKey, pStation);

				if ((pAppStation = dynamic_cast<CAppStation*>(pStation)) != NULL)
				{
					pAppStation->Motion();
					pAppStation->Command(glPRESTART_COMMAND);
					pAppStation->Result(gnOK);
				}
			}

			DisplayMessage(_T("Perform PreStart Command"));
			m_qState = PRESTART_Q;
		}
		else
		{
			m_qState	= IDLE_Q;
			m_nOperation = NULL_OP;
			m_lAction	= glNULL_COMMAND;
		}
	}
}

VOID CAC9000App::PreStartOperation()
{
	//If PreStart Operation Completed, Signal All station to perform: Auto ot Demo
	//Operation (depending on the Command)
	SFM_CStation	*pStation = NULL;
	CAppStation	*pAppStation = NULL;
	POSITION		pos;
	CString			szKey;

	if (CheckStationMotion() == MOTION_COMPLETE)
	{
		__time64_t ltime;
		CString szTime = "";

		_time64(&ltime);
		szTime = _ctime64(&ltime);
		szTime.TrimRight('\n');


		DisplayMessage(_T("PreStart operation completed"));

		if ((m_nOperation == AUTO_OP) || (m_nOperation == DEMO_OP) || (m_nOperation == CYCLE_OP))
		{
			for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
			{
				m_pStationMap.GetNextAssoc(pos, szKey, pStation);

				if ((pAppStation = dynamic_cast<CAppStation*>(pStation)) != NULL)
				{
					pAppStation->Motion();
					pAppStation->SetAxisAction(0, m_lAction, 0);
					pAppStation->Result(gnOK);

					switch (m_nOperation)
					{
					case AUTO_OP:
						pAppStation->Command(glAUTO_COMMAND);
						break;

					case DEMO_OP:
						pAppStation->Command(glDEMO_COMMAND);
						break;

					case CYCLE_OP:
						pAppStation->Command(glCYCLE_COMMAND);
						break;
					}
				}
			}

			switch (m_nOperation)
			{
			case AUTO_OP:
				if (AutoMode == DIAG_BOND)
				{
					g_bDiagMode = TRUE;
				}
				DisplayMessage(_T("Perform Auto Operation at ") + szTime);
				WriteHMIMess(_T("*** Perform Auto Operation ***") + szTime);
				m_qState = AUTO_Q;
				break;

			case DEMO_OP:
				if (AutoMode == DIAG_BOND)
				{
					g_bDiagMode = TRUE;
				}
				DisplayMessage(_T("Perform Demo Operation at ") + szTime);
				WriteHMIMess(_T("*** Perform Demo Operation at ") + szTime + _T("***"));
				m_qState = DEMO_Q;
				break;

			case CYCLE_OP:
				DisplayMessage(_T("Perform Cycle Test"));
				WriteHMIMess(_T("*** Perform Cycle Test ***"));
				m_qState = CYCLE_Q;
				break;

			default:
				DisplayMessage(_T("Invalid Operation Encountered!"));
				m_qState = IDLE_Q;
				break;
			}
		}
		else
		{
			m_nOperation	= NULL_OP;
			m_lAction		= glNULL_COMMAND;
			m_qState		= IDLE_Q;
			DisplayMessage(_T("System Initialization completed"));
		}
	}
}

VOID CAC9000App::AutoOperation()
{
	if (CheckStationMotion() == MOTION_COMPLETE)
	{
		DisplayMessage(_T("Auto Operation completed"));

		m_qState	= IDLE_Q;
		m_nOperation = NULL_OP;
		m_lAction	= glNULL_COMMAND;
	}
}

VOID CAC9000App::DemoOperation()
{
	if (CheckStationMotion() == MOTION_COMPLETE)
	{
		DisplayMessage(_T("Demonstration Operation completed"));

		m_qState	= IDLE_Q;
		m_nOperation = NULL_OP;
		m_lAction	= glNULL_COMMAND;
	}
}

VOID CAC9000App::CycleOperation()
{
	if (CheckStationMotion() == MOTION_COMPLETE)
	{
		DisplayMessage(_T("Cycle Test completed"));

		m_qState	= IDLE_Q;
		m_nOperation = NULL_OP;
		m_lAction	= glNULL_COMMAND;
	}
}

VOID CAC9000App::ManualOperation()
{
	if (CheckStationMotion() == MOTION_COMPLETE)
	{
		m_qState	= IDLE_Q;
		m_nOperation = NULL_OP;
		m_lAction	= glNULL_COMMAND;

		DisplayMessage(_T("Manual operation completed"));
	}
}

VOID CAC9000App::StopOperation()
{
	if (CheckStationMotion() == MOTION_COMPLETE)
	{
		if (m_lAction == glSYSTEM_EXIT)
		{
			((CFrameWnd*) m_pMainWnd)->PostMessage(WM_CLOSE);
		}

		if (m_fStopMsg)
		{
			g_bDiagMode = FALSE;

			try
			{
				// 20140728 Yip: Display Time When Operation Stopped
				__time64_t ltime;
				_time64(&ltime);
				CString szTime = _ctime64(&ltime);
				DisplayMessage(_T("[") + szTime + _T("]:") + _T("Operation Stopped!"));

				if (1)
				{
					pCHouseKeeping->UnLockAllCovers();
					DisplayMessage("UnLock all door");
				}

				// 20140801 Yip: Display Bonding Statistics When Operation Stopped
				DisplayMessage("***********************************************");
				DisplayMessage("Bonding Statistics:");
				CString szMsg;
				szMsg.Format("UPH: %.2lf", m_dUPH);
				DisplayMessage(szMsg);
				szMsg.Format("Unit Bonded: %ld", m_ulUnitsBonded);
				DisplayMessage(szMsg);
				szMsg.Format("Glass Picked: %ld", m_ulGlassPicked);
				DisplayMessage(szMsg);
				szMsg.Format("Glass Rejected: %ld", m_ulGlassRejected);
				DisplayMessage(szMsg);
				DisplayMessage("***********************************************");

				TRACE0("\n****** Stop Operation ******\n");
			}
			catch (CAsmException e)
			{
				DisplayException(e);
			}
		}

		m_qState	= IDLE_Q;
		m_nOperation = NULL_OP;
		m_lAction	= glNULL_COMMAND;


		PRS_AutoBondMode(FALSE, FALSE);

		// Add me later
		// save statistics
		m_smfMachine["Stats"]["m_ulTotalGlassPicked"]				= m_ulTotalGlassPicked;

		m_smfMachine["Stats"]["m_ulTotalGlassRejected"]				= m_ulTotalGlassRejected;
		m_smfMachine["Stats"]["m_ulTotalInvalidGlass"]				= m_ulTotalInvalidGlass;
		m_smfMachine["Stats"]["m_ulTotalACFWHPBPRError"]			= m_ulTotalACFWHPBPRError;

		m_smfMachine["Stats"]["m_ulTotalGlassTolError"]				= m_ulTotalGlassTolError;
		m_smfMachine["Stats"]["m_ulTotalAngCorrError"]				= m_ulTotalAngCorrError;

		m_smfMachine["Stats"]["m_ulTotalUnitsBonded"]				= m_ulTotalUnitsBonded;

		m_smfMachine["HouseKeeping"]["Settings"]["m_lACFBondedAlarmCount"] = pCHouseKeeping->m_lACFBondedAlarmCount;	//20130709

		m_smfMachine.Update();
	}
}

VOID CAC9000App::DeInitialOperation()
{
	SFM_CStation *pStation = NULL;
	CAppStation  *pAppStation;
	POSITION	  pos;
	CString		  szKey;
	CString		  szName;
	INT			  nCount = 0;

	for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
	{
		m_pStationMap.GetNextAssoc(pos, szKey, pStation);
		pAppStation = dynamic_cast<CAppStation*>(pStation);

		if (pAppStation->IsReInitialized())
		{
			nCount++;
		}
		else
		{
			break;
		}
	}

	if (nCount == m_pStationMap.GetCount())
	{
		m_qState = IDLE_Q;
		DisplayMessage("De-Initialize/Re-Initialize Operation Completed, Systems in IDLE state");
	}
}


/////////////////////////////////////////////////////////////////
//	Service Command Functions
/////////////////////////////////////////////////////////////////

LONG CAC9000App::SrvInitialize(IPC_CServiceMessage &svMsg)
{
	SMotCommand	smCommand;
	CString		szDisplay;
	BOOL		bResult = FALSE;

	try
	{
		// Decipher the receive request command
		svMsg.GetMsg(sizeof(SMotCommand), &smCommand);

		// TODO: Process the received command and perform the initialization
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CAC9000App::SrvSystemInitialize(IPC_CServiceMessage &svMsg)
{
	SMotCommand		smCommand;
	CString			szDisplay;
	SFM_CStation	*pStation = NULL;
	CAppStation	*pAppStation;
	POSITION		pos;
	CString			szKey;
	BOOL			bResult = FALSE;

	try
	{
		// Decipher the receive request command
		svMsg.GetMsg(sizeof(smCommand), &smCommand);

		szDisplay.Format(_T("%s - Receive System Initialize command: %d"), m_szModuleName, smCommand.lAction);
		DisplayMessage(szDisplay);

		if (m_qState == IDLE_Q)
		{
			m_lAction = smCommand.lAction;
			m_nOperation = NULL_OP;

			for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
			{
				m_pStationMap.GetNextAssoc(pos, szKey, pStation);
				pAppStation = dynamic_cast<CAppStation*>(pStation);

				pAppStation->Motion(TRUE);
				pAppStation->Result(gnOK);
				pAppStation->SetAxisAction(smCommand.lAxis, smCommand.lAction, smCommand.lUnit);

				pAppStation->Command(glINITIAL_COMMAND);
			}

			m_qState = SYSTEM_INITIAL_Q;
			bResult = TRUE;
		}
		else
		{
			szDisplay.Format(_T("%s - Initialize command canceled, Systems not in IDLE state"), m_szModuleName);
			DisplayMessage(szDisplay);

			bResult = FALSE;
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CAC9000App::SrvDiagCommand(IPC_CServiceMessage &svMsg)
{
	struct
	{
		LONG			lStation;
		SMotCommand		smCommand;
	} smDiagCommand;

	CString	szDisplay;
	BOOL	bResult = FALSE;

	try
	{
		// Decipher the receive request command
		svMsg.GetMsg(sizeof(smDiagCommand), &smDiagCommand);

		szDisplay.Format(_T("%s - Receive Diagnostic command"), m_szModuleName);
		DisplayMessage(szDisplay);

		szDisplay.Format(_T("   Station: %d, Axis: %d, Action: %d, Unit: %d"),
						 smDiagCommand.lStation, smDiagCommand.smCommand.lAxis,
						 smDiagCommand.smCommand.lAction, smDiagCommand.smCommand.lUnit);
		DisplayMessage(szDisplay);

		if (m_qState == IDLE_Q)
		{
			CAppStation *pStation		= NULL;
			CString		 szStationName	= m_pInitOperation->GetStationName(smDiagCommand.lStation);

			if ((pStation = dynamic_cast<CAppStation*>(GetStation(szStationName))) != NULL)
			{
				pStation->SetAxisAction(smDiagCommand.smCommand.lAxis,
										smDiagCommand.smCommand.lAction, 
										smDiagCommand.smCommand.lUnit);

				pStation->Motion(TRUE);
				pStation->Command(glDIAGNOSTIC_COMMAND);

				m_qState = DIAGNOSTICS_Q;
				bResult = TRUE;
			}
			else
			{
				szDisplay.Format(_T("%s - Diagnostic command canceled, Station Name not found"), m_szModuleName);
				DisplayMessage(szDisplay);
			}

		}
		else
		{
			szDisplay.Format(_T("%s - Diagnostic command canceled, Systems not in IDLE state"), m_szModuleName);
			DisplayMessage(szDisplay);

			bResult = FALSE;
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CAC9000App::SrvAutoCommand(IPC_CServiceMessage &svMsg)
{
	SMotCommand		smCommand;
	POSITION		pos;
	CString			szDisplay	= _T("");
	CString			szKey		= _T("");
	CString			szMsg		= _T("");
	SFM_CStation	*pStation	= NULL;
	CAppStation	*pAppStation	= NULL;
	BOOL			bResult		= FALSE;
	BOOL			bError		= FALSE;
	LONG			lCurMode	= NORMAL_BONDING;

	pCHouseKeeping->HMI_bStopProcess = FALSE; //20121225
	pCHouseKeeping->HMI_bChangeACF = FALSE;	// 20140911 Yip
	m_bStatusLogged = TRUE; //20150819

	try
	{
		if (m_qState == IDLE_Q)
		{

			pCHouseKeeping->SetResetButtonLampSol(OFF);
			pCHouseKeeping->m_bEnableStartBtn	= FALSE;
			pCHouseKeeping->m_bEnableResetBtn	= FALSE;
			pCHouseKeeping->m_bEnableStopBtn	= TRUE;
			pCHouseKeeping->m_bPressBtnToStop   = FALSE;
			pCHouseKeeping->SetResetButtonLampSol(OFF);
			// Decipher the receive request command
			svMsg.GetMsg(sizeof(SMotCommand), &smCommand);

			__time64_t ltime;
			_time64(&ltime);
			CString szTime = _ctime64(&ltime);
			szDisplay = _T("[") + szTime + _T("]:") + _T("Receive Auto command");

			m_dwOpStartTime = GetTickCount();
			m_clkBondStartTime = clock();

			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			DisplayMessage(szDisplay);
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		

			// Check Heater Ready
			if (!pCTempDisplay->CheckHeaterTempReady())
			{
				bError = TRUE;
			}
			
			// Check All Heater On for auto bond
			if (!CheckHeaterOnOff())
			{
				bError = TRUE;
			}
			//if (AutoMode == AUTO_BOND)
			//{
			//	if ( !pCTempDisplay->CheckAllHeaterOn()) //20130227
			//	{
			//		bError = TRUE;
			//	}
			//}

			// Check Air supply mode
			if (!pCHouseKeeping->m_bDetectAirSupply) //20130412
			{
				pCHouseKeeping->SetError(IDS_HK_AIR_SUPPLY_DETECTION_ENABLE_ERR);
				bError = TRUE;
			}
			//check air supply
			if (AutoMode != BURN_IN && !pCHouseKeeping->IsAirSupplyOn())
			{
				pCHouseKeeping->SetError(IDS_HK_AIR_SUPPLY_ERR);
				bError = TRUE;
			}
			
			g_bFlushMode = FALSE;

			if (!bError)   //20130227
			{
				// Autobond mode
				if (AutoMode == AUTO_BOND)
				{
					bError = TRUE;

					if (CheckAutoBondSelected(NORMAL_BONDING))
					{
						lCurMode = NORMAL_BONDING;
						bError = FALSE;
					}
					//else if (CheckAutoBondSelected(REWORK_PB))
					//{
					//	lCurMode = REWORK_PB;
					//	bError = FALSE;
					//}
					//else if (CheckAutoBondSelected(REWORK_MB1))
					//{
					//	lCurMode = REWORK_MB1;
					//	bError = FALSE;
					//}				
					//else if (CheckAutoBondSelected(REWORK_MB2))
					//{
					//	lCurMode = REWORK_MB2;
					//	bError = FALSE;
					//}	

					if (bError)
					{
						// Display Error at here
						for (INT i = 0; i < GetStationList().GetSize(); i++)
						{
							GetStationList().GetAt(i)->CheckModSelected(TRUE);
						}

						for (INT i = 0; i < GetPRStationList().GetSize(); i++)
						{
							GetPRStationList().GetAt(i)->CheckPRModSelected(TRUE);
						}
					}
					else 
					{
						switch (lCurMode)
						{
						case REWORK_MB1:
							DisplayMessage("Select Rework MB1");
							break;
						case REWORK_MB2:
							DisplayMessage("Select Rework MB2");
							break;
						case REWORK_PB:
							DisplayMessage("Select Rework PB");
							break;
						}

						if (!CheckPackageStatus((AutoBondRequirment)lCurMode))
						{
							bError = TRUE;
						}
					}
				}
				else
				{
					if (AutoMode == INDEXING && g_ulUserGroup < SERVICE) //20130823
					{
						UnselectModuleForIndexingMode();
					}

					if (pCACF1->m_bModSelected)
					{
						if (pCACF1->CheckACFReady() != gnAMS_OK)
						{
							bError = TRUE;
						}
					}

#if 1 //20130726
					//check all TAx
					if (AutoMode == TEST_BOND || AutoMode == BURN_IN)
					{
						CString szPromptStr = _T("");
						BOOL bIsPrompt = FALSE;
						if (!pCInPickArm->m_bModSelected)
						{
							szPromptStr.Format("%s Module Not Selected. Operation Abort!", pCInPickArm->GetStnName());
							bIsPrompt = TRUE;
						}
						if (!pCTA1->m_bModSelected)
						{
							szPromptStr.Format("%s Module Not Selected. Operation Abort!", pCTA1->GetStnName());
							bIsPrompt = TRUE;
						}
						if (!pCTA2->m_bModSelected)
						{
							szPromptStr.Format("%s Module Not Selected. Operation Abort!", pCTA2->GetStnName());
							bIsPrompt = TRUE;
						}
						
						if (bIsPrompt)
						{
							if (AutoMode == TEST_BOND)
							{
								HMIMessageBox(MSG_OK, "WARNING", szPromptStr);
								bError = TRUE;
							}
							if (AutoMode == BURN_IN)
							{
								LONG lAnswer = 0;
								lAnswer = HMIMessageBox(MSG_YES_NO, "BURN-IN", "Not All Transfer Arm Module are Selected, Continue?");
								if (lAnswer == rMSG_TIMEOUT || lAnswer == rMSG_NO)
								{
									bError = TRUE;
								}
							}
						}
					}
#endif
#if 1 //20121226
					if (AutoMode == INDEXING)
					{
						CString szPromptStr = _T("       ");
						BOOL bIsPrompt = FALSE;
						if (!pCInPickArm->m_bModSelected)
						{
							szPromptStr.Format("%s Module Not Selected. Operation Abort!", pCInPickArm->GetStnName());
							bIsPrompt = TRUE;
						}
						if (!pCTA1->m_bModSelected)
						{
							szPromptStr.Format("%s Module Not Selected. Operation Abort!", pCTA1->GetStnName());
							bIsPrompt = TRUE;
						}
						if (!pCTA2->m_bModSelected)
						{
							szPromptStr.Format("%s Module Not Selected. Operation Abort!", pCTA2->GetStnName());
							bIsPrompt = TRUE;
						}
						if (!pCACF1WH->m_bModSelected)
						{
							szPromptStr.Format("%s Module Not Selected. Operation Abort!", pCACF1WH->GetStnName());
							bIsPrompt = TRUE;
						}
						if (!pCACF2WH->m_bModSelected)
						{
							szPromptStr.Format("%s Module Not Selected. Operation Abort!", pCACF2WH->GetStnName());
							bIsPrompt = TRUE;
						}
						if (bIsPrompt)
						{
							HMIMessageBox(MSG_OK, "WARNING", szPromptStr);
							bError = TRUE;
						}
					}
#endif
				}
			}
			else //20130425
			{
				DisplayMessage("Auto command abort! Since Heater/AirSupply problem");
			}

			if (!bError && m_qState == IDLE_Q && !m_bDiagnMode)
			{
				pCHouseKeeping->LockAllCovers();
				DisplayMessage("Lock all door");
			}
			if (!bError && m_qState == IDLE_Q && !m_bDiagnMode)
			{
#if 1 //20141124
				m_csLogErrorAlertLock.Lock();
				CString szInFilePath2("D:\\sw\\AC9000\\LogFileDir\\");
				CString szInFileName2("ErrorAlarm.csv");
				CheckFileSize(szInFilePath2, szInFileName2, 512); //512KB
				LogErrorAlarm(CString("START_BONDING"));
				m_csLogErrorAlertLock.Unlock();
#endif
				if (GetHDDFreeSpace(m_ulFreeSpaceMB_C, m_ulFreeSpaceMB_D)) //20150810
				{
					szMsg.Format("HDD Free Space C: %ldMB, D:%ldMB", m_ulFreeSpaceMB_C, m_ulFreeSpaceMB_D);
					DisplayMessage(szMsg);
				}
				PRS_ClearScreen(nCamera);
				// reset stats
				m_ulGlassPicked = 0;

				m_ulGlassRejected = 0;
				m_ulInvalidGlass = 0;
				m_ulACFWHPBPRError = 0;

				m_ulGlassTolError = 0;
				m_ulAngCorrError = 0;

				m_ulCPAPickError = 0;
				
				m_ulUnitsBonded = 0;
				m_dUPH = 0.0;

				m_ulUnitsToBondCounter = m_ulUnitsToBond;

				m_ulTotalGlassRejected = m_ulTotalInvalidGlass + m_ulTotalACFWHPBPRError + m_ulTotalGlassTolError + m_ulTotalAngCorrError;

				m_lAction		= smCommand.lAction;
				m_nOperation	= AUTO_OP;
				m_qState		= SYSTEM_INITIAL_Q;

				// Signal all stations to perform system initialization
				for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
				{
					m_pStationMap.GetNextAssoc(pos, szKey, pStation);

					if ((pAppStation = dynamic_cast<CAppStation*>(pStation)) != NULL)
					{
						pAppStation->Motion();
						pAppStation->Command(glINITIAL_COMMAND);
						pAppStation->Result(gnOK);
					}
				}

				bResult	= TRUE;
			}
		}
		else
		{
			szDisplay.Format(_T("%s - Auto command canceled, Systems not in IDLE state"), m_szModuleName);
			DisplayMessage(szDisplay);

			bResult = FALSE;
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		bResult = FALSE;
	}

	if (g_pPerformance_report != NULL)
	{
		if (AutoMode == AUTO_BOND)
		{
			g_pPerformance_report->ReportEventTime(MPRT_START, MPRM_AUTO);    //report the event to the temporary memory, a vector
		}		
		else if (AutoMode == TEST_BOND)
		{
			g_pPerformance_report->ReportEventTime(MPRT_START, MPRM_TEST);    //report the event to the temporary memory, a vector
		}		
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CAC9000App::SrvManualCommand(IPC_CServiceMessage &svMsg)
{
	SMotCommand			smCommand;
	CString				szDisplay	= _T("");
	BOOL				bResult		= FALSE;

	try
	{
		// Decipher the receive request command
		svMsg.GetMsg(sizeof(SMotCommand), &smCommand);

		szDisplay.Format(_T("%s - Receive Manual command"), m_szModuleName);
		DisplayMessage(szDisplay);
		
		if (m_qState == IDLE_Q)
		{
			// TODO: Process the received command and perform the Manual operation
			bResult = TRUE;
		}
		else
		{
			szDisplay.Format(_T("%s - Manual command canceled, Systems not in IDLE state"), m_szModuleName);
			DisplayMessage(szDisplay);

			bResult = FALSE;
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CAC9000App::SrvCycleCommand(IPC_CServiceMessage &svMsg)
{
	SMotCommand		smCommand;
	POSITION		pos;
	CString			szDisplay	= _T("");
	CString			szKey		= _T("");
	SFM_CStation	*pStation	= NULL;
	CAppStation	*pAppStation	= NULL;
	BOOL			bResult		= FALSE;

	try
	{
		// Decipher the receive request command
		svMsg.GetMsg(sizeof(SMotCommand), &smCommand);

		__time64_t ltime;
		_time64(&ltime);
		CString szTime = _ctime64(&ltime);
		CString szMsg = "";
		szMsg.Format(_T("%s - Receive Cycle command"), m_szModuleName);
		szDisplay = _T("[") + szTime + _T("]:") + szMsg; //20130411
		DisplayMessage(szDisplay);
		
		if (m_qState == IDLE_Q && !m_bDiagnMode)
		{
			m_lAction		= smCommand.lAction;
			m_nOperation	= CYCLE_OP;
			m_qState		= SYSTEM_INITIAL_Q;

			// Signal all stations to perform Cycle Test operation
			for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
			{
				m_pStationMap.GetNextAssoc(pos, szKey, pStation);

				if ((pAppStation = dynamic_cast<CAppStation*>(pStation)) != NULL)
				{
					pAppStation->Motion();
					pAppStation->Result(gnOK);
					pAppStation->SetAxisAction(0, m_lAction, 0);
					pAppStation->Command(glINITIAL_COMMAND);
				}
			}

			bResult	= TRUE;
		}
		else
		{
			if (m_qState != IDLE_Q)
			{
				szDisplay.Format(_T("%s - Cycle command canceled, Systems not in IDLE state"), m_szModuleName);
			}
			else
			{
				HMIMessageBox(MSG_OK, "ERROR", "Systems initialized in DIAGN mode");
				szDisplay.Format(_T("%s - Cycle command canceled, Systems in DIAGN mode"), m_szModuleName);
			}
			DisplayMessage(szDisplay);

			bResult = FALSE;
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CAC9000App::SrvDemoCommand(IPC_CServiceMessage &svMsg)
{
	SMotCommand		smCommand;
	POSITION		pos;
	CString			szDisplay	= _T("");
	CString			szKey		= _T("");
	SFM_CStation	*pStation	= NULL;
	CAppStation	*pAppStation	= NULL;
	BOOL			bResult		= FALSE;

	pCHouseKeeping->HMI_bStopProcess = FALSE; //20121225
	pCHouseKeeping->HMI_bChangeACF = FALSE;	// 20140911 Yip
	m_bStatusLogged = TRUE; //20150819
//#ifdef MOUSE_CLICK_PR
//	m_ulINWHUserInputPRError = 0;
//#endif

	try
	{
		// Decipher the receive request command
		svMsg.GetMsg(sizeof(SMotCommand), &smCommand);

		szDisplay.Format(_T("%s - Receive Demostration command"), m_szModuleName);
		DisplayMessage(szDisplay);
		
		if (m_qState == IDLE_Q) 
		{
			m_lAction		= smCommand.lAction;
			m_nOperation	= DEMO_OP;
			m_qState		= SYSTEM_INITIAL_Q;

			// Signal all stations to perform Demo Cycle operation
			for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
			{
				m_pStationMap.GetNextAssoc(pos, szKey, pStation);

				if ((pAppStation = dynamic_cast<CAppStation*>(pStation)) != NULL)
				{
					pAppStation->Motion();
					pAppStation->Result(gnOK);
					pAppStation->SetAxisAction(0, m_lAction, 0);
					pAppStation->Command(glINITIAL_COMMAND);
				}
			}

			bResult			= TRUE;
		}
		else
		{
			szDisplay.Format(_T("%s - Demostration command canceled, Systems not in IDLE state"), m_szModuleName);
			DisplayMessage(szDisplay);

			bResult = FALSE;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CAC9000App::SrvStopCommand(IPC_CServiceMessage &svMsg)
{
	SMotCommand		smCommand;
	POSITION		pos;
	CString			szDisplay	= _T("");
	CString			szKey		= _T("");
	CString			szMsg		= _T("");
	SFM_CStation	*pStation	= NULL;
	CAppStation	*pAppStation	= NULL;
	BOOL			bResult		= FALSE;

	pCHouseKeeping->m_bEnableStartBtn	= TRUE;
	pCHouseKeeping->m_bEnableResetBtn	= TRUE;
	pCHouseKeeping->m_bEnableStopBtn	= FALSE;

	try
	{
		if (g_pPerformance_report != NULL)
		{
			g_pPerformance_report->ReportEventTime(MPRT_STOP);    //report the event to the temporary memory, a vector
		}			

		// Decipher the receive request command
		svMsg.GetMsg(sizeof(SMotCommand), &smCommand);
		m_lAction = smCommand.lAction;

		__time64_t ltime;
		_time64(&ltime);
		CString szTime = _ctime64(&ltime);
		CString szMsg = "";
		szMsg.Format(_T("%s - Receive Stop command"), m_szModuleName);
		szDisplay = _T("[") + szTime + _T("]:") + szMsg; //20130411
		//szDisplay.Format(_T("%s - Receive Stop command"), m_szModuleName);
		DisplayMessage(szDisplay);

		if (m_lAction == glCOMPLETE_STOP)
		{
			for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
			{
				m_pStationMap.GetNextAssoc(pos, szKey, pStation);
				pAppStation = dynamic_cast<CAppStation*>(pStation);

				if (pAppStation->State() == ERROR_Q)
				{
					m_lAction = glCYCLE_STOP;
					DisplayMessage("Auto convert COMPLETE_STOP to CYCLE_STOP");
					break;
				}
			}
		}

		if (m_lAction == glSYSTEM_EXIT)
		{
			SendAlarm(_T("OS011"));
			TRACE0("\n****** Exit System ******\n");

			CSingleLock slLock(&m_csDispMsg);
			slLock.Lock();
			m_fCanDisplay = FALSE;
			slLock.Unlock();
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	try
	{
		// Signal all stations to perform Stop operation
		for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
		{
			m_pStationMap.GetNextAssoc(pos, szKey, pStation);

			if ((pAppStation = dynamic_cast<CAppStation*>(pStation)) != NULL)
			{
				if ((m_lAction == glSYSTEM_EXIT) ||
					(m_lAction == glEMERGENCY_STOP))
				{
					pAppStation->SetAxisAction(0, glABORT_STOP, 0);
				}
				else
				{
					pAppStation->SetAxisAction(0, m_lAction, 0);
				}

				pAppStation->Command(glSTOP_COMMAND);
				pAppStation->Result(gnOK);
			}
		}

		if (m_qState != IDLE_Q)
		{
			m_fStopMsg = TRUE;
		}
		else
		{
			m_fStopMsg = FALSE;
		}

		m_qState = STOPPING_Q;
		m_dwStopTime = GetTickCount();
		m_bStatusLogged = FALSE;
		bResult	= TRUE;
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		bResult = FALSE;
	}

#if 1 //20141210
	CString szInFilePath("D:\\sw\\AC9000\\LogFileDir\\");
	DeleteOldFiles(szInFilePath, CString("AC9000MsgLog*.txt"), 14);
	DeleteOldFiles(szInFilePath, CString("ErrorAlarm_*.csv"), 14);
	DeleteOldFiles(szInFilePath, CString("StationStatus*.log"), 14);

	szInFilePath = CString("D:\\sw\\AC9000\\Data\\StatisticsLogFile\\");
	DeleteOldFiles(szInFilePath, CString("StatisticsLogFile_*.txt"), 14);

	szInFilePath = "D:\\sw\\AC9000\\Data\\ParameterLog\\";
	DeleteOldFiles(szInFilePath, CString("*.log"), 14);

	szInFilePath = "D:\\sw\\AC9000\\Data\\Calculation Log\\";
	DeleteOldFiles(szInFilePath, CString("*.log"), 14);
	DeleteOldFiles(szInFilePath, CString("*.txt"), 14);

	szInFilePath = "D:\\sw\\AC9000\\Data\\";
	DeleteOldFiles(szInFilePath, CString("Temperature_*.csv"), 14);
	DeleteOldFiles(szInFilePath, CString("MemoryUseageLog*.csv"), 14);
#endif

	if (GetHDDFreeSpace(m_ulFreeSpaceMB_C, m_ulFreeSpaceMB_D)) //20150810
	{
		szMsg.Format("HDD Free Space C: %ldMB, D:%ldMB", m_ulFreeSpaceMB_C, m_ulFreeSpaceMB_D);
		DisplayMessage(szMsg);
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

//p20120829: log result onto a text file after pressing stop button
LONG CAC9000App::HMI_StatisticsLogFile(IPC_CServiceMessage &svMsg)
{
	UINT		MAX_NUM_OF_STAT_FILES	= 10;
	BOOL		bOpDone					= TRUE;
	CString		szStatFilePath			= "";

	// 20140814 Yip: Log Statistic Into File By Date Instead Of By File Count
	SYSTEMTIME ltime;
	GetLocalTime(&ltime);
	szStatFilePath.Format("D:\\sw\\AC9000\\Data\\StatisticsLogFile\\StatisticsLogFile_%04u%02u%02u.txt", ltime.wYear, ltime.wMonth, ltime.wDay);
//	szStatFilePath.Format("D:\\sw\\AC9000\\Data\\StatisticsLogFile\\StatisticsLogFile%ld.txt", m_lStatisticsLogCount);
	
	//Create Directory if not exist
	CFileFind fileFind;
	INT nFind = m_szDeviceFile.ReverseFind('.');
	if (!fileFind.FindFile("D:\\sw\\AC9000\\Data\\StatisticsLogFile\\"))
	{
		m_pInitOperation->CreateFileDirectory("D:\\sw\\AC9000\\Data\\StatisticsLogFile\\");
	}

	// Open file to record UpLook PR Position
	FILE *fp = fopen(szStatFilePath, "at");	// 20140814 Yip: Use "at" Instead Of "wt"
	if (fp != NULL)
	{
		clock_t clkNow;
		__time64_t ltime;
		CString szTime = "";

		clkNow = clock();
		_time64(&ltime);
		szTime = _ctime64(&ltime);
		szTime.TrimRight('\n');

		fprintf(fp, "%s\nOperation Time in sec = %.3f\n", (const char*)szTime, (DOUBLE)(clkNow - m_clkBondStartTime) / CLOCKS_PER_SEC);

		fprintf(fp, "******Statistic Log of Most Recent Trial******\n");
		fprintf(fp, "Units Bonded = %lu\n", m_ulUnitsBonded);

		fprintf(fp, "Glass Picked = %lu\n", m_ulGlassPicked);
		fprintf(fp, "Glass Rejected = %lu\n", m_ulGlassRejected);
		fprintf(fp, "Invalid Glass = %lu\n", m_ulInvalidGlass);

		fprintf(fp, "Glass Tol. Error = %lu\n", m_ulGlassTolError);
		//fprintf(fp, "FPC Tol. Error = %lu\n", m_ulLSITolError);
		fprintf(fp, "Angle Corr. Error = %lu\n", m_ulAngCorrError);

		fprintf(fp, "ACFWH PostBond PR Error = %lu\n", m_ulACFWHPBPRError);
		fprintf(fp, "FPC Pick Arm Pick Error = %lu\n", m_ulCPAPickError);

		fprintf(fp, "Last Error = %s\n", (pCHouseKeeping->m_bErrorExist ? pCHouseKeeping->m_szLastError : "No Error"));	// 20140814 Yip

		fprintf(fp, "\n");
		fprintf(fp, "**********Statistic Log of All Trials**********\n");		
		fprintf(fp, "Total Units Bonded = %lu\n", m_ulTotalUnitsBonded);

		fprintf(fp, "Total Glass Picked = %lu\n", m_ulTotalGlassPicked);
		fprintf(fp, "Total Glass Rejected = %lu\n", m_ulTotalGlassRejected);
		fprintf(fp, "Total Invalid Glass = %lu\n", m_ulTotalInvalidGlass);

		fprintf(fp, "Total Glass Tol. Error = %lu\n", m_ulTotalGlassTolError);
		fprintf(fp, "Total Angle Corr. Error = %lu\n", m_ulTotalAngCorrError);

		fprintf(fp, "Total ACFWH PostBond PR Error = %lu\n", m_ulTotalACFWHPBPRError);
		fprintf(fp, "Total FPC Pick Arm Pick Error = %lu\n", m_ulTotalCPAPickError);

		fprintf(fp, "\n\n");
		fprintf(fp, "***********************************************\n");
		fprintf(fp, "\n\n");

		fclose(fp);
		m_lStatisticsLogCount = (m_lStatisticsLogCount + 1) % MAX_NUM_OF_STAT_FILES;
	}

	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}
//p20120829:end****************************************************

LONG CAC9000App::SrvResetCommand(IPC_CServiceMessage &svMsg)
{
	struct
	{
		LONG			lStation;
		SMotCommand		smCommand;
	} smResetCommand;

	POSITION	  pos;
	CString		  szDisplay	= _T("");
	CString		  szKey		= _T("");
	SFM_CStation *pStation	= NULL;
	CAppStation  *pAppStation = NULL;
	BOOL		  bResult	= FALSE;

	try
	{
		// Decipher the receive request command
		svMsg.GetMsg(sizeof(smResetCommand), &smResetCommand);

		szDisplay.Format(_T("%s - Receive Reset command"), m_szModuleName);
		DisplayMessage(szDisplay);
		
		if (smResetCommand.smCommand.lAction == glRESET_SINGLE)
		{
			CString szStationName = m_pInitOperation->GetStationName(smResetCommand.lStation);

			if ((pAppStation = dynamic_cast<CAppStation*>(GetStation(szStationName))) != NULL)
			{
				if (pAppStation->State() == ERROR_Q)
				{
					pAppStation->Command(glRESET_COMMAND);
					bResult = TRUE;
				}
			}
			else
			{
				szDisplay.Format(_T("%s - Single Reset command canceled, Station Name not found"), m_szModuleName);
				DisplayMessage(szDisplay);
			}
		}
		else
		{
			for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
			{
				m_pStationMap.GetNextAssoc(pos, szKey, pStation);
				pAppStation = dynamic_cast<CAppStation*>(pStation);

				if (pAppStation->State() == ERROR_Q)
				{
					pAppStation->Command(glRESET_COMMAND);
				}

				pAppStation->Abort(FALSE);
			}

			bResult = TRUE;
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CAC9000App::SrvDeInitialCommand(IPC_CServiceMessage &svMsg)
{
	CString			szDisplay, szKey;
	BOOL			bResult;
	SFM_CStation	*pStation = NULL;
	CAppStation	*pAppStation;

	try
	{
		szDisplay.Format(_T("%s - Receive hardware de-initialize command"), m_szModuleName);
		DisplayMessage(szDisplay);
		
		// Decipher the receive request command
		//	svMsg.GetMsg(sizeof(SCommand), &smCommand);

		if (m_qState == IDLE_Q)
		{
			//Hipec hardware DeInitialize
			m_pInitOperation->DeleteContents();
			DisplayMessage("Reset Hipec Ring Network ...");

			//Re-Load Mark IDE Data
			LoadData();

			//Set Controller Type
			m_pInitOperation->SetControllerType(OTHER_CONTROLLER);

			// Signal all stations to ReInitialize
			for (POSITION pos = m_pStationMap.GetStartPosition(); pos != NULL;)
			{
				m_pStationMap.GetNextAssoc(pos, szKey, pStation);

				if ((pAppStation = dynamic_cast<CAppStation*>(pStation)) != NULL)
				{
					pAppStation->Command(glDEINITIAL_COMMAND);
					pAppStation->ReInitialized(FALSE);
					pAppStation->Result(gnOK);
				}
			}

			m_qState = DE_INITIAL_Q;
			bResult = TRUE;
		}
		else
		{
			szDisplay.Format(_T("%s - De-Initialize command canceled, Systems not in IDLE state"), m_szModuleName);
			DisplayMessage(szDisplay);
		
			bResult = FALSE;
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CAC9000App::SrvJogCommand(IPC_CServiceMessage &svMsg)
{
	SMotCommand		smCommand;
	CString			szKey		= _T("");
	SFM_CStation	*pStn		= NULL;
	CAppStation	*pAppStn		= NULL;
	BOOL			bResult		= FALSE;

	try
	{
		// Decipher the receive request command
		svMsg.GetMsg(sizeof(smCommand), &smCommand);
		m_lAction = smCommand.lAction;

		for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
		{
			m_pStationMap.GetNextAssoc(pos, szKey, pStn);
			pAppStn = dynamic_cast<CAppStation*>(pStn);
			
			switch (m_lAction)
			{
			case glMRK_JOG_ON:
				pAppStn->SetJogMode(TRUE);
				break;

			case glMRK_JOG_OFF:
				pAppStn->SetJogMode(FALSE);
				break;

			case glMRK_STEP:
				pAppStn->Step(TRUE);
				break;
			}

			bResult = TRUE;
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}


/////////////////////////////////////////////////////////////////
// Start execution utilities
/////////////////////////////////////////////////////////////////

BOOL CAC9000App::AutoStart()
{
	return m_fAutoStart;
}

BOOL CAC9000App::SetRun()
{
	BOOL bResult = FALSE;

	if (m_qState == UN_INITIALIZE_Q)
	{
		m_bRunState = TRUE;
		bResult = TRUE;
	}
	else
	{
		DisplayMessage("Hardware already been initialized!");
	}

	return bResult;
}

VOID CAC9000App::SaveAutoStart(BOOL bOption)
{
	INT nTemp = 0;

	if (bOption)
	{
		nTemp = 1;
	}

	WriteProfileInt(gszPROFILE_SETTING, gszPROFILE_AUTO_START, nTemp);
	m_fAutoStart = bOption;
}


/////////////////////////////////////////////////////////////////
// Option & Hardware Selection
/////////////////////////////////////////////////////////////////

VOID CAC9000App::SetOptions(BOOL isOKEnable)
{
	COptionPasswordDlg dlgPw;
	BOOL bPasswordValid = FALSE;
	CString szSvrPasswrod;

	if (isOKEnable && dlgPw.DoModal() == IDOK)
	{
		szSvrPasswrod = m_smfMachine["HouseKeeping"]["Password"]["szSvrPassword"];
		if (szSvrPasswrod.GetLength() <= 1)
		{
			szSvrPasswrod.Format("a5s2m2");
		}

		if (dlgPw.m_szPassword == szSvrPasswrod)
		{
			bPasswordValid = TRUE;
		}
		else
		{
			bPasswordValid = FALSE;
			DisplayMessage("Option Modification Password Incorrect");
		}
	}
	
	if (bPasswordValid || !isOKEnable)
	{
		COptionDlg	dlg;

		dlg.m_bSelectVer1			= m_fSelectVer1;
		dlg.m_bSelectVer2			= m_fSelectVer2;
		dlg.m_bSelectVer3			= m_fSelectVer3;

		dlg.m_bSelectLitec			= m_fSelectLitec;
		dlg.m_bSelectADAM			= m_fSelectADAM;
		dlg.m_bSelectHeaterOn		= m_fSelectHeaterOn;	// 20140905 Yip
		//dlg.m_bSelectEdgeCleaner	= m_fSelectEdgeCleaner;
		//dlg.m_bSelectCOG902			= m_fSelectCOG902;

		if (!isOKEnable) //20120402
		{
			dlg.m_bIsAppRunning = TRUE;
		}

		// Initialize the Option dialog attributes
		if (dlg.DoModal() == IDOK && isOKEnable) //20111214
		{
			m_fSelectVer1			= dlg.m_bSelectVer1;
			m_fSelectVer2			= dlg.m_bSelectVer2;
			m_fSelectVer3			= dlg.m_bSelectVer3;
			m_fSelectLitec			= dlg.m_bSelectLitec;
			m_fSelectADAM			= dlg.m_bSelectADAM;
			m_fSelectHeaterOn		= dlg.m_bSelectHeaterOn;	// 20140905 Yip
			//m_fSelectEdgeCleaner	= dlg.m_bSelectEdgeCleaner;
			//m_fSelectCOG902			= dlg.m_bSelectCOG902;
			// Obtain the selected options and save them

			//Mark IDE Data Path
			LoadData();

			// Retrieve Options
			WriteProfileInt(gszPROFILE_HW_CONFIG, gszSELECT_MC_VER1, m_fSelectVer1);
			WriteProfileInt(gszPROFILE_HW_CONFIG, gszSELECT_MC_VER2, m_fSelectVer2);
			WriteProfileInt(gszPROFILE_HW_CONFIG, gszSELECT_MC_VER3, m_fSelectVer3);
			WriteProfileInt(gszPROFILE_HW_CONFIG, gszSELECT_LITEC, m_fSelectLitec);
			WriteProfileInt(gszPROFILE_HW_CONFIG, gszSELECT_ADAM, m_fSelectADAM);
			WriteProfileInt(gszPROFILE_HW_CONFIG, gszSELECT_HEATER_ON, m_fSelectHeaterOn);	// 20140905 Yip
			//WriteProfileInt(gszPROFILE_HW_CONFIG, gszSELECT_COG902, m_fSelectCOG902);
		}
	}
}

VOID CAC9000App::SelectHardware(BOOL isOKEnable)
{
	COptionPasswordDlg dlgPw;
	BOOL bPasswordValid = FALSE;
	CString szSvrPasswrod;

	if (isOKEnable && dlgPw.DoModal() == IDOK)
	{
		szSvrPasswrod = m_smfMachine["HouseKeeping"]["Password"]["szSvrPassword"];
		if (szSvrPasswrod.GetLength() <= 1)
		{
			szSvrPasswrod.Format("a5s2m2");
		}

		if (dlgPw.m_szPassword == szSvrPasswrod)
		{
			bPasswordValid = TRUE;
		}
		else
		{
			bPasswordValid = FALSE;
			DisplayMessage("Hardware Modification Password Incorrect");
		}
	}
	
	if (bPasswordValid || !isOKEnable)
	{
		CHardwareDlg hwDlg;

		hwDlg.m_bHardware		= m_fEnableHardware;
		hwDlg.m_bPRS			= m_fEnablePRS;
		hwDlg.m_bTempCtrl		= m_fEnableTempCtrl;
		hwDlg.m_bDiagnMode		= m_fEnableDiagnMode;

		//hwDlg.m_bInConveyor		= m_fEnableInConveyor;
		hwDlg.m_bTA1			= m_fEnableTA1;
		hwDlg.m_bTA2			= m_fEnableTA2;
		hwDlg.m_bACF1WH			= m_fEnableACF1WH;
		hwDlg.m_bACF2WH			= m_fEnableACF2WH;
		hwDlg.m_bACF1			= m_fEnableACF1;
		hwDlg.m_bInPickArm		= m_fEnableInPickArm;
		hwDlg.m_bInspOpt		= m_fEnableInspOpt;

		if (!isOKEnable) //20120402
		{
			hwDlg.m_bIsAppRunning = TRUE;
		}

		if (hwDlg.DoModal() == IDOK && isOKEnable) //20111214
		{
			m_fEnableHardware		= hwDlg.m_bHardware;
			m_fEnablePRS			= hwDlg.m_bPRS;
			m_fEnableTempCtrl		= hwDlg.m_bTempCtrl;
			m_fEnableDiagnMode		= hwDlg.m_bDiagnMode;

			//m_fEnableInConveyor		= hwDlg.m_bInConveyor;
			m_fEnableTA1			= hwDlg.m_bTA1;
			m_fEnableTA2			= hwDlg.m_bTA2;
			m_fEnableACF1WH			= hwDlg.m_bACF1WH;
			m_fEnableACF2WH			= hwDlg.m_bACF2WH;
			m_fEnableACF1			= hwDlg.m_bACF1;
			m_fEnableInPickArm		= hwDlg.m_bInPickArm;
			m_fEnableInspOpt		= hwDlg.m_bInspOpt;

			WriteProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_ALL_HARDWARE, m_fEnableHardware);
			WriteProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_PRS, m_fEnablePRS);
			WriteProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_TEMP_CTRL, m_fEnableTempCtrl);
			WriteProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_DIAGN_MODE, m_fEnableDiagnMode);

			WriteProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_TA1, m_fEnableTA1);
			WriteProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_TA2, m_fEnableTA2);
			WriteProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_ACF1WH, m_fEnableACF1WH);
			WriteProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_ACF2WH, m_fEnableACF2WH);
			WriteProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_ACF1, m_fEnableACF1);
			WriteProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_InPickArm, m_fEnableInPickArm);
			WriteProfileInt(gszPROFILE_HW_CONFIG, gszENABLE_InspOpt, m_fEnableInspOpt);
		}
	}
}


/////////////////////////////////////////////////////////////////
// Log Message
/////////////////////////////////////////////////////////////////

BOOL CAC9000App::LogMessage()
{
	return m_fLogMessage;
}

VOID CAC9000App::SetLogMessage(BOOL bEnable)
{
	INT nTemp = 0;

	if (bEnable)
	{
		nTemp = 1;
	}

	WriteProfileInt(gszPROFILE_SETTING, gszPROFILE_LOG_MSG, nTemp);
	m_fLogMessage = bEnable;
}

VOID CAC9000App::SetTotLogBackupFiles(LONG lUnit)
{
	WriteProfileInt(gszPROFILE_SETTING, gszLOG_BACKUP_SETTING, lUnit);
	m_pInitOperation->SetTotLogBackupFiles(lUnit);
}

VOID CAC9000App::SetTotLogFileLine(LONG lUnit)
{
	WriteProfileInt(gszPROFILE_SETTING, gszLOG_FILE_LINE, m_pInitOperation->SetTotLogFileLine(lUnit));
}


/////////////////////////////////////////////////////////////////
// Group ID
/////////////////////////////////////////////////////////////////

VOID CAC9000App::SetGroupId(const CString &szGroup)
{
	m_szGroupID = szGroup;
	WriteProfileString(gszPROFILE_SETTING, gszPROFILE_GROUP_ID, szGroup);
}

VOID CAC9000App::SaveGroupId(const CString &szGroup)
{
	WriteProfileString(gszPROFILE_SETTING, gszPROFILE_GROUP_ID, szGroup);
}


/////////////////////////////////////////////////////////////////
// Check for motion completion
/////////////////////////////////////////////////////////////////

INT CAC9000App::CheckStationMotion()
{
	SFM_CStation *pStation = NULL;
	CAppStation  *pAppStation;
	POSITION	  pos;
	CString		  szKey;
	INT			  nResult = MOTION_COMPLETE;

	for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
	{
		m_pStationMap.GetNextAssoc(pos, szKey, pStation);
		pAppStation = dynamic_cast<CAppStation*>(pStation);

		if (pAppStation->InMotion())
		{
			nResult = MOTION_RUNNING;
			break;
		}
	}

	return nResult;
}


/////////////////////////////////////////////////////////////////
// Execute HMI
/////////////////////////////////////////////////////////////////

VOID CAC9000App::SetExecuteHmiFlag(BOOL bExecute)
{
	INT nTemp = 0;
	
	if (bExecute)
	{
		nTemp = 1;
	}

	WriteProfileInt(gszPROFILE_SETTING, gszPROFILE_EXECUTE_HMI, nTemp);
	m_fExecuteHmi = bExecute;
}

BOOL CAC9000App::IsExecuteHmi()
{
	return m_fExecuteHmi;
}

VOID CAC9000App::ExecuteHmi()
{
	if (m_fExecuteHmi)
	{
		STARTUPINFO startupInfo;
		PROCESS_INFORMATION	processInfo;
	
		memset(&startupInfo, 0, sizeof(STARTUPINFO)); //set memory to 0
		startupInfo.cb = sizeof(STARTUPINFO);
	
		CString strCurDir = "D:\\sw\\AC9000\\AsmHmi";
		CString strHmiExe = "D:\\sw\\AC9000\\AsmHmi\\AsmHmi.exe";
		CString strHmiFile = "D:\\sw\\AC9000\\Hmi\\AC9000.hcf";
		CString strCommandLine = strHmiExe + " " + strHmiFile;
	
		bool bDebug = true;
		char szCommandLine[1024];
		strcpy(szCommandLine, (LPCTSTR)strCommandLine);
	
		CreateProcess(
			/*Application Name*/NULL, 
			/*Command Line*/szCommandLine,
			NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, 
			/*Current Directory*/strCurDir, 
			&startupInfo, &processInfo);
			
		m_fRestartHmi = FALSE;
	}
}

VOID CAC9000App::ExecuteGateway()
{
	STARTUPINFO startupInfo;
	//PROCESS_INFORMATION	processInfo;

	ZeroMemory(&m_stGatewayProcessInfo, sizeof(m_stGatewayProcessInfo));
	memset(&startupInfo, 0, sizeof(STARTUPINFO)); //set memory to 0
	startupInfo.cb = sizeof(STARTUPINFO);

	CString strCurDir = "D:\\sw\\AC9000\\Gateway";
	CString strGatewayExe = "D:\\sw\\AC9000\\Gateway\\SocketGateway.exe";
	//CString strGatewayFile = "D:\\sw\\AC9000\\Gateway\\IPC_Gateway.cfg";
	CString strCommandLine = strGatewayExe; // + " " + strGatewayFile;

	//bool bDebug = true;
	char szCommandLine[1024];
	ZeroMemory(&szCommandLine[0], 1024);
	strcpy(szCommandLine, (LPCTSTR)strCommandLine);

	CreateProcess(
		/*Application Name*/NULL, 
		/*Command Line*/szCommandLine,
		NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, 
		/*Current Directory*/strCurDir, 
		&startupInfo, &m_stGatewayProcessInfo);
}

BOOL CAC9000App::KeepResponse()	// 20140714 Yip
{
	MSG msgCur;
	while (PeekMessage(&msgCur, NULL, NULL, NULL, PM_NOREMOVE))
	{
		// pump message, but quit on WM_QUIT
		if (!PumpMessage())
		{
			return FALSE;
		}
	}
	return TRUE;
}

VOID CAC9000App::ExecuteCreateErrorLog()	// 20140714 Yip
{
	DisplayMessage("Creating Error Log...");

	STARTUPINFO startupInfo;
	PROCESS_INFORMATION	processInfo;

	memset(&startupInfo, 0, sizeof(STARTUPINFO)); //set memory to 0
	startupInfo.cb = sizeof(STARTUPINFO);

	CString strCurDir = "D:";
	CString strCommandLine = "D:\\CreateErrorLog.exe";

	char szCommandLine[1024];
	strcpy(szCommandLine, (LPCTSTR)strCommandLine);

	BOOL bResult = CreateProcess(
					   /*Application Name*/NULL, 
					   /*Command Line*/szCommandLine,
					   NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, 
					   /*Current Directory*/strCurDir, 
					   &startupInfo, &processInfo);

	if (bResult)
	{
		DWORD dwResult;
		do
		{
			KeepResponse();
			Sleep(100);
			dwResult = WaitForSingleObject(processInfo.hProcess, 100);
		} while (dwResult == WAIT_TIMEOUT);
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	}

	DisplayMessage("Error Log Created");
}

BOOL CAC9000App::ScanVisionNT()
{
	// Check if visionNT.exe is running
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = {0};
	INT nCount = 0;

	// Take a snapshot of all processes in the system. 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		// Fill in the size of the structure before using it. 
		pe32.dwSize = sizeof(PROCESSENTRY32);

		// Walk the snapshot of the processes, and for each process, display information.
		if (Process32First(hProcessSnap, &pe32))
		{
			do
			{
				CString strName = pe32.szExeFile;
				strName = strName.MakeUpper();

				if (strName.Find("VISIONNT.EXE") >= 0)
				{
					nCount++;
				}
			} while (Process32Next(hProcessSnap, &pe32));
		}
	}
	CloseHandle(hProcessSnap);

	if (nCount != 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

#if 1 //20141124
VOID CAC9000App::InitErrorAlertTable()
{
	//auto generate by pyton script. Pls don't input manually
	// For details. Pls refer \\cog900-ws1\SoftwareBackup\Development\Developer\python\Portable Python 2.7.6.1\App\test
	m_mapErrorAlertTable["ACF1000"] = "ACF1 Not Selected Error";
	m_mapErrorAlertTable["ACF1001"] = "ACF1 Indexer Motor Error";
	m_mapErrorAlertTable["ACF1002"] = "ACF1 Roller Motor Error";
	m_mapErrorAlertTable["ACF1021"] = "ACF1 Head Not Up Error";
	m_mapErrorAlertTable["ACF1022"] = "ACF1 Unit Not Up Error";
	m_mapErrorAlertTable["ACF1023"] = "ACF1 Counter Balance Sol Not On Error";
	m_mapErrorAlertTable["ACF1024"] = "ACF1 Counter Balance Sol Not Off Error";
	m_mapErrorAlertTable["ACF1025"] = "ACF1 Stripper Not On Error";
	m_mapErrorAlertTable["ACF1026"] = "ACF1 Stripper Not Off Error";
	m_mapErrorAlertTable["ACF1027"] = "ACF1 Cutter Not On Error";
	m_mapErrorAlertTable["ACF1028"] = "ACF1 Cutter Not Off Error";
	m_mapErrorAlertTable["ACF1029"] = "ACF1 Tape End Warning";
	m_mapErrorAlertTable["ACF1030"] = "ACF1 Tape End Error";
	m_mapErrorAlertTable["ACF1031"] = "ACF1 Invalid ACF Error";
	m_mapErrorAlertTable["ACF1032"] = "ACF1 Invalid ACF Indexed Length Error";
	m_mapErrorAlertTable["ACF1033"] = "ACF1 Stripper Motor X Error";
	m_mapErrorAlertTable["ACF1034"] = "ACF1 Renew Rubber Band Reminder";
	m_mapErrorAlertTable["ACF1WH000"] = "ACF1WH Not Selected Error";
	m_mapErrorAlertTable["ACF1WH001"] = "ACF1WH Motor Y Error";
	m_mapErrorAlertTable["ACF1WH002"] = "ACF1WH Motor Z Error";
	m_mapErrorAlertTable["ACF1WH021"] = "ACF1WH Glass Vac Error";
	m_mapErrorAlertTable["ACF1WH022"] = "ACF1WH Reject Glass Limit Hit";
	m_mapErrorAlertTable["ACF2000"] = "ACF2 Not Selected Error";
	m_mapErrorAlertTable["ACF2001"] = "ACF2 Indexer Motor Error";
	m_mapErrorAlertTable["ACF2002"] = "ACF2 Roller Motor Error";
	m_mapErrorAlertTable["ACF2021"] = "ACF2 Head Not Up Error";
	m_mapErrorAlertTable["ACF2022"] = "ACF2 Unit Not Up Error";
	m_mapErrorAlertTable["ACF2023"] = "ACF2 Counter Balance Sol Not On Error";
	m_mapErrorAlertTable["ACF2024"] = "ACF2 Counter Balance Sol Not Off Error";
	m_mapErrorAlertTable["ACF2025"] = "ACF2 Stripper Not On Error";
	m_mapErrorAlertTable["ACF2026"] = "ACF2 Stripper Not Off Error";
	m_mapErrorAlertTable["ACF2027"] = "ACF2 Cutter Not On Error";
	m_mapErrorAlertTable["ACF2028"] = "ACF2 Cutter Not Off Error";
	m_mapErrorAlertTable["ACF2029"] = "ACF2 Tape End Warning";
	m_mapErrorAlertTable["ACF2030"] = "ACF2 Tape End Error";
	m_mapErrorAlertTable["ACF2031"] = "ACF2 Invalid ACF Error";
	m_mapErrorAlertTable["ACF2032"] = "ACF2 Invalid ACF Indexed Length Error";
	m_mapErrorAlertTable["ACF2033"] = "ACF2 Stripper Motor X Error";
	m_mapErrorAlertTable["ACF2034"] = "ACF2 Renew Rubber Band Reminder";
	m_mapErrorAlertTable["ACF2WH000"] = "ACF2WH Not Selected Error";
	m_mapErrorAlertTable["ACF2WH001"] = "ACF2WH Motor Y Error";
	m_mapErrorAlertTable["ACF2WH002"] = "ACF2WH Motor Z Error";
	m_mapErrorAlertTable["ACF2WH021"] = "ACF2WH Glass Vac Error";
	m_mapErrorAlertTable["ACF2WH022"] = "ACF2WH Reject Glass Limit Hit";
	m_mapErrorAlertTable["ADAM000"] = "ADAM Read Error";
	m_mapErrorAlertTable["ADAM001"] = "ADAM MB1 Head Read Error";
	m_mapErrorAlertTable["ADAM002"] = "ADAM MB2 Head Read Error";
	m_mapErrorAlertTable["ADAM003"] = "ADAM MB1 Base Read Error";
	m_mapErrorAlertTable["ADAM004"] = "ADAM MB2 Base Read Error";
	m_mapErrorAlertTable["ADAM005"] = "ADAM ACF1 Read Error";
	m_mapErrorAlertTable["ADAM006"] = "ADAM PB1 Read Error";
	m_mapErrorAlertTable["ADAM007"] = "ADAM MB3 Head Read Error";
	m_mapErrorAlertTable["ADAM008"] = "ADAM MB4 Head Read Error";
	m_mapErrorAlertTable["ADAM009"] = "ADAM MB3 Base Read Error";
	m_mapErrorAlertTable["ADAM010"] = "ADAM MB4 Base Read Error";
	m_mapErrorAlertTable["ADAM011"] = "ADAM ACF2 Read Error";
	m_mapErrorAlertTable["ADAM012"] = "ADAM PB2 Read Error";
	m_mapErrorAlertTable["CPA000"] = "ChipPickArm Not Selected Error";
	m_mapErrorAlertTable["CPA021"] = "ChipPickArm Pick Error";
	m_mapErrorAlertTable["CPA022"] = "ChipPickArm Unknown Die Error";
	m_mapErrorAlertTable["CPA023"] = "ChipPickArm Collet Life Cycle Limit Hit";
	m_mapErrorAlertTable["CPA024"] = "ChipPickArm Contact Sensor Error";
	m_mapErrorAlertTable["FLIP000"] = "Flipper Not Selected Error";
	m_mapErrorAlertTable["FLIP001"] = "Flipper Motor Flip T Error";
	m_mapErrorAlertTable["FLIP002"] = "Flipper Motor Swing T Error";
	m_mapErrorAlertTable["FLIP003"] = "Flipper Motor Z Error";
	m_mapErrorAlertTable["FLIP021"] = "Flipper Get Die Error";
	m_mapErrorAlertTable["FLIP022"] = "Flipper Unknown Die Error";
	m_mapErrorAlertTable["FLIP023"] = "Flipper Collet Life Cycle Limit Hit";
	m_mapErrorAlertTable["FLIP024"] = "Flipper Get Type1 Vac Error";
	m_mapErrorAlertTable["FLIP025"] = "Flipper Get Type2 Vac Error";
	m_mapErrorAlertTable["FLIP026"] = "Flipper Shuttle LSI Type Not Match Error";
	m_mapErrorAlertTable["FLIP027"] = "Flipper Pick LSI Error";
	m_mapErrorAlertTable["FLIP028"] = "Flipper Pick Reject LSI Error";
	m_mapErrorAlertTable["FPCPICKARM001"] = "FPC PickArm X Motor Lmt Snr Hit";
	m_mapErrorAlertTable["FPCPICKARM002"] = "FPC Pick Arm LZ Sol Error";
	m_mapErrorAlertTable["FPCPICKARM003"] = "FPC Pick Arm RZ Sol Error";
	m_mapErrorAlertTable["FPCPICKARM004"] = "FPC Pick Arm Pre Pick Error";
	m_mapErrorAlertTable["FPCPICKARM005"] = "FPC Pick Arm Place Position Error";
	m_mapErrorAlertTable["FPCPICKARM006"] = "FPC Pick Arm Place FPC Error";
	m_mapErrorAlertTable["FPCPICKARM007"] = "FPC Pick Arm Pick Position Error";
	m_mapErrorAlertTable["FPCPICKARM008"] = "FPC Pick Arm Pick FPC Retry Limit Hit";
	m_mapErrorAlertTable["FPCPICKARM009"] = "FPC Pick Arm Motor Y Error";
	m_mapErrorAlertTable["FPCPICKARM010"] = "FPC Pick Arm Motor T1 Error";
	m_mapErrorAlertTable["FPCPICKARM011"] = "FPC Pick Arm Motor X Error";
	m_mapErrorAlertTable["FPCPICKARM012"] = "FPC Pick Arm Motor T2 Error";
	m_mapErrorAlertTable["FPCPICKARM013"] = "FPC Pick Arm Z1 Home Snr Not Cut Error";
	m_mapErrorAlertTable["FPCPICKARM014"] = "FPC Pick Arm Z2 Home Snr Not Cut Error";
	m_mapErrorAlertTable["FPCPICKARM015"] = "FPC Pick Arm Motor Z1 Error";
	m_mapErrorAlertTable["FPCPICKARM016"] = "FPC Pick Arm Motor Z2 Error";
	m_mapErrorAlertTable["FPCPICKARM017"] = "FPC Pick Arm Z1 Safety Level Error";
	m_mapErrorAlertTable["FPCPICKARM018"] = "FPC Pick Arm Z2 Safety Level Error";
	m_mapErrorAlertTable["FPCPICKARM019"] = "FPC Pick Arm Z1 / Z2 Safety Level Error";
	m_mapErrorAlertTable["FPCREJARM001"] = "FPC Reject Arm Vac Off Error";
	m_mapErrorAlertTable["FPCREJARM002"] = "FPC Reject Arm Pick Error";
	m_mapErrorAlertTable["FPCREJARM003"] = "FPC Reject Arm Place Error";
	m_mapErrorAlertTable["FPCREJARM004"] = "FPC Reject Arm Skip FPC Limit Hit";
	m_mapErrorAlertTable["HK000"] = "HouseKeeping Air Supply Error";
	m_mapErrorAlertTable["HK001"] = "HouseKeeping Operation TimeOut Error";
	m_mapErrorAlertTable["HK002"] = "HouseKeeping Software Handling Error";
	m_mapErrorAlertTable["HK003"] = "HouseKeeping Prebond Air Supply Error";
	m_mapErrorAlertTable["HK004"] = "HouseKeeping Mainbond Air Supply Error";
	m_mapErrorAlertTable["HK005"] = "HouseKeeping Prebond Front Cover Open Error";
	m_mapErrorAlertTable["HK006"] = "HouseKeeping Prebond Back Cover Open Error";
	m_mapErrorAlertTable["HK007"] = "HouseKeeping Mainbond Front Cover Open Error";
	m_mapErrorAlertTable["HK008"] = "HouseKeeping Mainbond Back Cover Open Error";
	m_mapErrorAlertTable["HK009"] = "HouseKeeping Light Curtain Blocked Error";
	m_mapErrorAlertTable["HK010"] = "HouseKeeping Air Supply Detection Enable Error";
	m_mapErrorAlertTable["HK011"] = "Please Clear the ACF Collection Box";
	m_mapErrorAlertTable["HK012"] = "HouseKeeping EMO Pressed";
	m_mapErrorAlertTable["HK013"] = "HouseKeeping Off Lader InElev Upper Cover Not  Detected Error";
	m_mapErrorAlertTable["HK014"] = "HoouseKeeping In Tray Elev Door Not Detected Error";
	m_mapErrorAlertTable["HK015"] = "HoouseKeeping Out Tray Door Not Selected Error";
	m_mapErrorAlertTable["HK016"] = "HoouseKeeping In Cell Front Cover Not Sekected Error";
	m_mapErrorAlertTable["HK017"] = "HouseKeeping Pickarm door Not selected Error";
	m_mapErrorAlertTable["HK018"] = "HoouseKeeping ICH back Cover not Selection Error;r";
	m_mapErrorAlertTable["HP001"] = "Motor Not On Error";
	m_mapErrorAlertTable["HP002"] = "JoyStick Not Off Error";
	m_mapErrorAlertTable["HP003"] = "Limit Sensor Hit";
	m_mapErrorAlertTable["HP004"] = "Search Fail";
	m_mapErrorAlertTable["HP005"] = "DAC Limit Hit";
	m_mapErrorAlertTable["HP006"] = "Software Position Limit Hit";
	m_mapErrorAlertTable["HP007"] = "Driver Fault";
	m_mapErrorAlertTable["HP008"] = "Encoder Fault";
	m_mapErrorAlertTable["HP009"] = "Power Fault";
	m_mapErrorAlertTable["HP010"] = "Position Error Limit Hit";
	m_mapErrorAlertTable["HP011"] = "Temperature Power Fault";
	m_mapErrorAlertTable["HP012"] = "Software Commutation Initialization Fault";
	m_mapErrorAlertTable["HP013"] = "Unknown Channel Fault";
	m_mapErrorAlertTable["HP014"] = "Unknown Fault";
	m_mapErrorAlertTable["HP015"] = "Motor Commutation Error";
	m_mapErrorAlertTable["HP016"] = "Motor Not Home";
	m_mapErrorAlertTable["HP017"] = "Isolation Power Fault";
	m_mapErrorAlertTable["HP018"] = "Motion Abort";
	m_mapErrorAlertTable["HP019"] = "Other Channel Fault";
	m_mapErrorAlertTable["HP020"] = "Event Driven Error";
	m_mapErrorAlertTable["HP021"] = "Motion TimeOut";
	m_mapErrorAlertTable["HP022"] = "FlexiControl Error";
	m_mapErrorAlertTable["HP031"] = "Search Timeout Error";
	m_mapErrorAlertTable["HP032"] = "Exceed Software Limit Error";
	m_mapErrorAlertTable["HP033"] = "Hit Limit Sensor Error";
	m_mapErrorAlertTable["HP034"] = "Encoder Fault Error";
	m_mapErrorAlertTable["HP035"] = "Driver Fault Error";
	m_mapErrorAlertTable["HP036"] = "Motion Abort Error";
	m_mapErrorAlertTable["HP037"] = "Dependent Channel Error";
	m_mapErrorAlertTable["HP038"] = "Motion Control Error";
	m_mapErrorAlertTable["HP039"] = "Brake Lock Error";
	m_mapErrorAlertTable["HP040"] = "Open Wire Fault Error";
	m_mapErrorAlertTable["HP041"] = "Velocity Jump Error";
	m_mapErrorAlertTable["HP042"] = "Emergency Stop Error";
	m_mapErrorAlertTable["HP043"] = "Hit Projected Software Limit Error";
	m_mapErrorAlertTable["INCON000"] = "In Conveyor Not Selected Error";
	m_mapErrorAlertTable["INCON001"] = "In Conveyor Belt Error";
	m_mapErrorAlertTable["INCON021"] = "In Conveyor Glass Not Exist";
	m_mapErrorAlertTable["INCON022"] = "In Conveyor Motor X Error";
	m_mapErrorAlertTable["INCON023"] = "In Conveyor Light Curtain Blocked Alert";
	m_mapErrorAlertTable["INSHWH000"] = "INSHWH Not Selected Error";
	m_mapErrorAlertTable["INSHWH001"] = "INSHWH Motor X Error";
	m_mapErrorAlertTable["INSHWH002"] = "INSHWH Motor Y Error";
	m_mapErrorAlertTable["INSHWH003"] = "INSHWH Motor Z1 Error";
	m_mapErrorAlertTable["INSHWH004"] = "INSHWH Motor Z2 Error";
	m_mapErrorAlertTable["INSHWH005"] = "INSHWH Pick Glass Error";
	m_mapErrorAlertTable["INSHWH006"] = "INSHWH Reject Glass Error";
	m_mapErrorAlertTable["INSP1000"] = "Glass1 ACF PR Not Selected Error";
	m_mapErrorAlertTable["INSP1010"] = "Glass1 ACF PR Not Loaded Error";
	m_mapErrorAlertTable["INSP1011"] = "Glass1 ACF PR1 Error";
	m_mapErrorAlertTable["INSP1012"] = "Glass1 ACF PR2 Error";
	m_mapErrorAlertTable["INSP1013"] = "Glass1 ACF Length Tolerance Error";
	m_mapErrorAlertTable["INSP2000"] = "Glass2 ACF PR Not Selected Error";
	m_mapErrorAlertTable["INSP2010"] = "Glass2 ACF PR Not Loaded Error";
	m_mapErrorAlertTable["INSP2011"] = "Glass2 ACF PR1 Error";
	m_mapErrorAlertTable["INSP2012"] = "Glass2 ACF PR2 Error";
	m_mapErrorAlertTable["INSP2013"] = "Glass2 ACF Length Tolerance Error";
	m_mapErrorAlertTable["INWH000"] = "INWH Not Selected Error";
	m_mapErrorAlertTable["INWH001"] = "INWH PR Not Selected Error";
	m_mapErrorAlertTable["INWH002"] = "INWH Motor Y Error";
	m_mapErrorAlertTable["INWH003"] = "INWH Motor T Error";
	m_mapErrorAlertTable["INWH004"] = "INWH Motor Z1 Error";
	m_mapErrorAlertTable["INWH005"] = "INWH Motor Z2 Error";
	m_mapErrorAlertTable["INWH006"] = "INWH Calibration Not Selected";
	m_mapErrorAlertTable["INWH010"] = "INWH PR Not Loaded";
	m_mapErrorAlertTable["INWH011"] = "INWH PR1 Error";
	m_mapErrorAlertTable["INWH012"] = "INWH PR2 Error";
	m_mapErrorAlertTable["INWH013"] = "INWH Alignment Not Loaded";
	m_mapErrorAlertTable["INWH021"] = "INWH Glass1 Vac Error";
	m_mapErrorAlertTable["INWH022"] = "INWH Glass2 Vac Error";
	m_mapErrorAlertTable["INWH023"] = "INWH Glass Vac Error";
	m_mapErrorAlertTable["INWH024"] = "INWH Reject Glass Limit Hit";
	m_mapErrorAlertTable["INWH025"] = "INWH Glass1 PR1 Manual Click PR Alert";
	m_mapErrorAlertTable["INWH026"] = "INWH Glass1 PR2 Manual Click PR Alert";
	m_mapErrorAlertTable["INWH027"] = "INWH Glass2 PR1 Manual Click PR Alert";
	m_mapErrorAlertTable["INWH028"] = "INWH Glass2 PR2 Manual Click PR Alert";
	m_mapErrorAlertTable["MB1000"] = "MainBond1 Not Selected Error";
	m_mapErrorAlertTable["MB1001"] = "MainBond1 Motor Z Error";
	m_mapErrorAlertTable["MB1002"] = "MainBond1 Indexer Error";
	m_mapErrorAlertTable["MB1003"] = "MainBond1 Rewinder Error";
	m_mapErrorAlertTable["MB1021"] = "MainBond1 Left Teflon Empty Error";
	m_mapErrorAlertTable["MB1022"] = "MainBond1 Right Teflon Empty Error";
	m_mapErrorAlertTable["MB1023"] = "MainBond1 Left Contact Error";
	m_mapErrorAlertTable["MB1024"] = "MainBond1 Right Contact Error";
	m_mapErrorAlertTable["MB1025"] = "MainBond1 Left Teflon Tape Error";
	m_mapErrorAlertTable["MB1026"] = "MainBond1 Right Teflon Tape Error";
	m_mapErrorAlertTable["MB1027"] = "MainBond1 Contact Level Out Of Tolerance Error";
	m_mapErrorAlertTable["MB1028"] = "MainBond1 Left Pre Contact Error";
	m_mapErrorAlertTable["MB1029"] = "MainBond1 Right Pre Contact Error";
	m_mapErrorAlertTable["MB1030"] = "MainBond1 Left Counter Balance Sol Not On Error";
	m_mapErrorAlertTable["MB1031"] = "MainBond1 Right Counter Balance Sol Not On Error";
	m_mapErrorAlertTable["MB1032"] = "MainBond1 Post Contact Error";
	m_mapErrorAlertTable["MB1033"] = "MainBond1 Pre Contact Error";
	m_mapErrorAlertTable["MB1034"] = "MainBond1 Left Teflon Jam Error";
	m_mapErrorAlertTable["MB1035"] = "MainBond1 Right Teflon Jam Error";
	m_mapErrorAlertTable["MB1036"] = "MainBond1 Left-Right Contact Level Difference Tolerance Error";
	m_mapErrorAlertTable["MB1037"] = "MainBond1 Home Sensor Cut Error";
	m_mapErrorAlertTable["MB1038"] = "MainBond1 Contact Level Variation Tolerance Error";
	m_mapErrorAlertTable["MB1039"] = "MainBond1 Left Tape Empty Error";
	m_mapErrorAlertTable["MB1040"] = "MainBond1 Right Tape Empty Error";
	m_mapErrorAlertTable["MB1WH000"] = "MainBond1 WH Not Selected  Error";
	m_mapErrorAlertTable["MB1WH001"] = "MainBond1 WH Motor Y  Error";
	m_mapErrorAlertTable["MB1WH002"] = "MainBond1 WH Motor Z1  Error";
	m_mapErrorAlertTable["MB1WH003"] = "MainBond1 WH Motor Z2  Error";
	m_mapErrorAlertTable["MB1WH021"] = "MainBond1 WH Glass1 Vac  Error";
	m_mapErrorAlertTable["MB1WH022"] = "MainBond1 WH Glass2 Vac  Error";
	m_mapErrorAlertTable["MB1WH023"] = "MainBond1 WH Glass Vac  Error";
	m_mapErrorAlertTable["MB1WH024"] = "MainBond1 WH Reject Glass Limit Hit";
	m_mapErrorAlertTable["MB2000"] = "MainBond2 Not Selected Error";
	m_mapErrorAlertTable["MB2001"] = "MainBond2 Motor Z Error";
	m_mapErrorAlertTable["MB2002"] = "MainBond2 Indexer Error";
	m_mapErrorAlertTable["MB2003"] = "MainBond2 Rewinder Error";
	m_mapErrorAlertTable["MB2021"] = "MainBond2 Left Teflon Empty Error";
	m_mapErrorAlertTable["MB2022"] = "MainBond2 Right Teflon Empty Error";
	m_mapErrorAlertTable["MB2023"] = "MainBond2 Left Contact Error";
	m_mapErrorAlertTable["MB2024"] = "MainBond2 Right Contact Error";
	m_mapErrorAlertTable["MB2025"] = "MainBond2 Left Teflon Tape Error";
	m_mapErrorAlertTable["MB2026"] = "MainBond2 Right Teflon Tape Error";
	m_mapErrorAlertTable["MB2027"] = "MainBond2 Contact Level Out Of Tolerance Error";
	m_mapErrorAlertTable["MB2028"] = "MainBond2 Left Pre-Contact Error";
	m_mapErrorAlertTable["MB2029"] = "MainBond2 Right Pre-Contact Error";
	m_mapErrorAlertTable["MB2030"] = "MainBond2 Left Counter Balance Sol Not On Error";
	m_mapErrorAlertTable["MB2031"] = "MainBond2 Right Counter Balance Sol Not On Error";
	m_mapErrorAlertTable["MB2032"] = "MainBond2 Post Contact Error";
	m_mapErrorAlertTable["MB2033"] = "MainBond2 Pre Contact Error";
	m_mapErrorAlertTable["MB2034"] = "MainBond2 Left Teflon Jam Error";
	m_mapErrorAlertTable["MB2035"] = "MainBond2 Right Teflon Jam Error";
	m_mapErrorAlertTable["MB2036"] = "MainBond2 Left-Right Contact Level Difference Tolerance Error";
	m_mapErrorAlertTable["MB2037"] = "MainBond2 Home Sensor Cut Error";
	m_mapErrorAlertTable["MB2038"] = "MainBond2 Contact Level Variation Tolerance Error";
	m_mapErrorAlertTable["MB2039"] = "MainBond2 Left Tape Empty Error";
	m_mapErrorAlertTable["MB2040"] = "MainBond2 Right Tape Empty Error";
	m_mapErrorAlertTable["MB2WH000"] = "MainBond2 WH Not Selected Error";
	m_mapErrorAlertTable["MB2WH001"] = "MainBond2 WH Motor Y Error";
	m_mapErrorAlertTable["MB2WH002"] = "MainBond2 WH Motor Z1 Error";
	m_mapErrorAlertTable["MB2WH003"] = "MainBond2 WH Motor Z2 Error";
	m_mapErrorAlertTable["MB2WH021"] = "MainBond2 WH Glass1 Vac Error";
	m_mapErrorAlertTable["MB2WH022"] = "MainBond2 WH Glass2 Vac Error";
	m_mapErrorAlertTable["MB2WH023"] = "MainBond2 WH Glass Vac Error";
	m_mapErrorAlertTable["MB2WH024"] = "MainBond2 WH Reject Glass Limit Hit";
	m_mapErrorAlertTable["MTR000"] = "Motor Error";
	m_mapErrorAlertTable["OUTCON000"] = "Out Conveyor Not Selected Error";
	m_mapErrorAlertTable["OUTCON001"] = "Out Conveyor Full Error";
	m_mapErrorAlertTable["OUTCON002"] = "Out Conveyor Reject Glass  Error";
	m_mapErrorAlertTable["PB1000"] = "PreBond1 Not Selected Error";
	m_mapErrorAlertTable["PB1001"] = "PreBond1 PR Not Selected Error";
	m_mapErrorAlertTable["PB1002"] = "PreBond1 Motor T Error";
	m_mapErrorAlertTable["PB1003"] = "PreBond1 Motor Z Error";
	m_mapErrorAlertTable["PB1004"] = "PreBond1 Calibration Not Selected  Error";
	m_mapErrorAlertTable["PB1010"] = "PreBond1 PR Not Loaded  Error";
	m_mapErrorAlertTable["PB1011"] = "PreBond1 PR1 Error";
	m_mapErrorAlertTable["PB1012"] = "PreBond1 PR2 Error";
	m_mapErrorAlertTable["PB1013"] = "PreBond1 Alignment Not Loaded  Error";
	m_mapErrorAlertTable["PB1021"] = "PreBond1 Collet Vac  Error";
	m_mapErrorAlertTable["PB1022"] = "PreBond1 Pick Contact  Error";
	m_mapErrorAlertTable["PB1023"] = "PreBond1 Pick Die Error";
	m_mapErrorAlertTable["PB1024"] = "PreBond1 Pre Contact Error";
	m_mapErrorAlertTable["PB1025"] = "PreBond1 Bond Contact Error";
	m_mapErrorAlertTable["PB1026"] = "PreBond1 Blow Die Limit Hit";
	m_mapErrorAlertTable["PB1027"] = "PreBond1 Distance Out Of Tolerance Error";
	m_mapErrorAlertTable["PB1028"] = "PreBond1 Angle Tolerance Error";
	m_mapErrorAlertTable["PB1029"] = "PreBond1 LSI Not Found After Contact Error";
	m_mapErrorAlertTable["PB1030"] = "PreBond1 LSI Position Out Of Tolerance Error";
	m_mapErrorAlertTable["PB1031"] = "PreBond1 SH1 Contact  Error";
	m_mapErrorAlertTable["PB1032"] = "PreBond1 Contact Level Out Of Tolerance  Error";
	m_mapErrorAlertTable["PB1033"] = "PreBond1 Post Contact  Error";
	m_mapErrorAlertTable["PB1034"] = "PreBond1 Invalid LSI Type Error";
	m_mapErrorAlertTable["PB1035"] = "PreBond1 Pick LSI Error";
	m_mapErrorAlertTable["PB1036"] = "PreBond1 Home Sensor Cut Error";
	m_mapErrorAlertTable["PB1037"] = "PreBond1 Blow Die Error";
	m_mapErrorAlertTable["PB1038"] = "PreBond1 Theta Calibration Table Limit Hit Error";
	m_mapErrorAlertTable["PB1039"] = "PreBond1 Reject Bin Motor X Error";
	m_mapErrorAlertTable["PB1040"] = "PreBond1 Theta Calibration In Progess";
	m_mapErrorAlertTable["PB1041"] = "PreBond1 PR1 Manual Click PR Error";
	m_mapErrorAlertTable["PB1042"] = "PreBond1 PR2 Manual Click PR Error";
	m_mapErrorAlertTable["PB2000"] = "PreBond2 Not Selected Error";
	m_mapErrorAlertTable["PB2001"] = "PreBond2 PR Not Selected Error";
	m_mapErrorAlertTable["PB2002"] = "PreBond2 Motor T Error";
	m_mapErrorAlertTable["PB2003"] = "PreBond2 Motor Z Error";
	m_mapErrorAlertTable["PB2004"] = "PreBond2 Calibration Not Selected Error";
	m_mapErrorAlertTable["PB2010"] = "PreBond2 PR Not Loaded Error";
	m_mapErrorAlertTable["PB2011"] = "PreBond2 PR1 Error";
	m_mapErrorAlertTable["PB2012"] = "PreBond2 PR2 Error";
	m_mapErrorAlertTable["PB2013"] = "PreBond2 Alignment Not Loaded Error";
	m_mapErrorAlertTable["PB2021"] = "PreBond2 Collet Vac Error";
	m_mapErrorAlertTable["PB2022"] = "PreBond2 Pick Contact Error";
	m_mapErrorAlertTable["PB2023"] = "PreBond2 Pick Die Error";
	m_mapErrorAlertTable["PB2024"] = "PreBond2 Pre Contact Error";
	m_mapErrorAlertTable["PB2025"] = "PreBond2 Bond Contact Error";
	m_mapErrorAlertTable["PB2026"] = "PreBond2 Blow Die Limit Hit";
	m_mapErrorAlertTable["PB2027"] = "PreBond2 Distance Out Of Tolerance Error";
	m_mapErrorAlertTable["PB2028"] = "PreBond2 Angle Tolerance Error";
	m_mapErrorAlertTable["PB2029"] = "PreBond2 LSI Not Found After Contact Error";
	m_mapErrorAlertTable["PB2030"] = "PreBond2 LSI Position Out Of Tolerance Error";
	m_mapErrorAlertTable["PB2031"] = "PreBond2 SH2 Contact Error";
	m_mapErrorAlertTable["PB2032"] = "PreBond2 Contact Level Out Of Tolerance Error";
	m_mapErrorAlertTable["PB2033"] = "PreBond2 Post Contact Error";
	m_mapErrorAlertTable["PB2034"] = "PreBond2 Invalid LSI Type Error";
	m_mapErrorAlertTable["PB2035"] = "PreBond2 Pick LSI Error";
	m_mapErrorAlertTable["PB2036"] = "PreBond2 Home Sensor Cut Error";
	m_mapErrorAlertTable["PB2037"] = "PreBond2 Blow Die Error";
	m_mapErrorAlertTable["PB2038"] = "PreBond2 Theta Calibration Table Limit Hit Error";
	m_mapErrorAlertTable["PB2039"] = "PreBond2 Reject Bin Motor X Dummy Error";
	m_mapErrorAlertTable["PB2040"] = "PreBond2 Theta Calibration In Progess";
	m_mapErrorAlertTable["PB2041"] = "PreBond2 PR1 Manual Click PR Error";
	m_mapErrorAlertTable["PB2042"] = "PreBond2 PR2 Manual Click PR Error";
	m_mapErrorAlertTable["PBGLASS1000"] = "PBGlass1 PR Not Selected Error";
	m_mapErrorAlertTable["PBGLASS1010"] = "PBGlass1 PR Not Loaded Error";
	m_mapErrorAlertTable["PBGLASS1011"] = "PBGlass1  PR1 Error";
	m_mapErrorAlertTable["PBGLASS1012"] = "PBGlass1 PR2 Error";
	m_mapErrorAlertTable["PBGLASS1013"] = "PBGlass1 Angle Out Of Tolerance Error";
	m_mapErrorAlertTable["PBGLASS1014"] = "PBGlass1  Distance Out Of Tolerance Error";
	m_mapErrorAlertTable["PBGLASS2000"] = "PBGlass2 PR Not Selected Error";
	m_mapErrorAlertTable["PBGLASS2010"] = "PBGlass2 PR Not Loaded Error";
	m_mapErrorAlertTable["PBGLASS2011"] = "PBGlass2 PR1 Error";
	m_mapErrorAlertTable["PBGLASS2012"] = "PBGlass2 PR2 Error";
	m_mapErrorAlertTable["PBGLASS2013"] = "PBGlass2 Angle Out Of Tolerance Error";
	m_mapErrorAlertTable["PBGLASS2014"] = "PBGlass2 Distance Out Of Tolerance Error";
	m_mapErrorAlertTable["PBUPLOOK000"] = "PBUplook Not Selected Error";
	m_mapErrorAlertTable["PBUPLOOK001"] = "PBUplook Motor X Error";
	m_mapErrorAlertTable["PBUPLOOK002"] = "PBUplook Motor Y Error";
	m_mapErrorAlertTable["PBWH000"] = "PreBond WH Not Selected Error";
	m_mapErrorAlertTable["PBWH001"] = "PreBond WH Motor X Error";
	m_mapErrorAlertTable["PBWH002"] = "PreBond WH Motor Y Error";
	m_mapErrorAlertTable["PBWH003"] = "PreBond WH Motor Z1 Error";
	m_mapErrorAlertTable["PBWH004"] = "PreBond WH Motor Z2 Error";
	m_mapErrorAlertTable["PBWH005"] = "PreBond WH Calibration Not Loaded Error";
	m_mapErrorAlertTable["PBWH006"] = "PreBond WH Motor X2 Error";
	m_mapErrorAlertTable["PBWH007"] = "PreBond WH Motor Y2 Error";
	m_mapErrorAlertTable["PBWH008"] = "PreBond WH Motor T Error";
	m_mapErrorAlertTable["PBWH011"] = "PreBond WH Alignment Not Loaded Error";
	m_mapErrorAlertTable["PBWH021"] = "PreBond WH Glass 1 Vac Error";
	m_mapErrorAlertTable["PBWH022"] = "PreBond WH Glass 2 Vac Error";
	m_mapErrorAlertTable["PBWH023"] = "PreBond WH Glass Vac Error";
	m_mapErrorAlertTable["PBWH024"] = "PreBond WH Reject Glass Limit Hit";
	m_mapErrorAlertTable["PR000"] = "PR Not Selected Error";
	m_mapErrorAlertTable["PR001"] = "DPR Master Initialise Error";
	m_mapErrorAlertTable["PR002"] = "DPR Slave Initialise Error";
	m_mapErrorAlertTable["PR003"] = "DPR Warm Start Error";
	m_mapErrorAlertTable["PR004"] = "Loading PR records from archive...";
	m_mapErrorAlertTable["REJARM000"] = "RejectArm Not Selected Error";
	m_mapErrorAlertTable["REJARM001"] = "RejectArm Error";
	m_mapErrorAlertTable["REJARM002"] = "Belt Full";
	m_mapErrorAlertTable["SH001"] = "Shuttle Reject Pre Bond FPC Error";
	m_mapErrorAlertTable["SH002"] = "Shuttle Collect FPC Error";
	m_mapErrorAlertTable["SH003"] = "Shuttle Roll FPC Error";
	m_mapErrorAlertTable["SH004"] = "Shuttle Move to PR1 Error";
	m_mapErrorAlertTable["SH005"] = "Shuttle Move to PR2 Error";
	m_mapErrorAlertTable["SH006"] = "Shuttle Move to Pre Pick Position Error";
	m_mapErrorAlertTable["SH007"] = "Shuttle Move to Prebond1 Error";
	m_mapErrorAlertTable["SH008"] = "Shuttle Move to Prebond2 Error";
	m_mapErrorAlertTable["SH009"] = "Shuttle Move to Place Position Error";
	m_mapErrorAlertTable["SH010"] = "Shuttle Move to Reject Position Error";
	m_mapErrorAlertTable["SH011"] = "Shuttle Retract Reject Bin Error";
	m_mapErrorAlertTable["SH012"] = "Shuttle Lose FPC1 Error";
	m_mapErrorAlertTable["SH013"] = "Shuttle Lose FPC2 Error";
	m_mapErrorAlertTable["SH014"] = "Shuttle Lose FPC1 & FPC2 Error";
	m_mapErrorAlertTable["SH1000"] = "SH1 Not Selected Error";
	m_mapErrorAlertTable["SH1001"] = "SH1 PR Not Selected Error";
	m_mapErrorAlertTable["SH1002"] = "SH1 Motor X Error";
	m_mapErrorAlertTable["SH1003"] = "SH1 Motor Y Error";
	m_mapErrorAlertTable["SH1004"] = "SH1 Calibration Not Loaded Error";
	m_mapErrorAlertTable["SH1010"] = "SH1 PR Not Loaded Error";
	m_mapErrorAlertTable["SH1011"] = "SH1 PR1 Error";
	m_mapErrorAlertTable["SH1012"] = "SH1 PR2 Error";
	m_mapErrorAlertTable["SH1013"] = "SH1 Alignment Not Loaded Error";
	m_mapErrorAlertTable["SH1021"] = "SH1 Get FPC Error";
	m_mapErrorAlertTable["SH1022"] = "SH1 Get Reject FPC Error";
	m_mapErrorAlertTable["SH1023"] = "SH1 PR1 In Danger Zone Error";
	m_mapErrorAlertTable["SH1024"] = "SH1 Reject FPC Limit Hit";
	m_mapErrorAlertTable["SH1025"] = "SH1 PR1 Mouse Click PR Alert";
	m_mapErrorAlertTable["SH1026"] = "SH1 PR2 Mouse Click PR Alert";
	m_mapErrorAlertTable["SH2000"] = "SH2 Not Selected Error";
	m_mapErrorAlertTable["SH2001"] = "SH2 PR Not Selected Error";
	m_mapErrorAlertTable["SH2002"] = "SH2 Motor X Error";
	m_mapErrorAlertTable["SH2003"] = "SH2 Motor Y Error";
	m_mapErrorAlertTable["SH2004"] = "SH2 Calibration Not Loaded Error";
	m_mapErrorAlertTable["SH2010"] = "SH2 PR Not Loaded Error";
	m_mapErrorAlertTable["SH2011"] = "SH2 PR1 Error";
	m_mapErrorAlertTable["SH2012"] = "SH2 PR2 Error";
	m_mapErrorAlertTable["SH2013"] = "SH2 Alignment Not Loaded Error";
	m_mapErrorAlertTable["SH2021"] = "SH2 Get FPC Error";
	m_mapErrorAlertTable["SH2022"] = "SH2 Get Reject FPC Error";
	m_mapErrorAlertTable["SH2023"] = "SH2 PR2 In Danger Zone Error";
	m_mapErrorAlertTable["SH2024"] = "SH2 Reject FPC Limit Hit";
	m_mapErrorAlertTable["SH2025"] = "SH2 PR1 Mouse Click PR Alert";
	m_mapErrorAlertTable["SH2026"] = "SH2 PR2 Mouse Click PR Alert";
	m_mapErrorAlertTable["SOL000"] = "Solenoid Error";
	m_mapErrorAlertTable["T1I000"] = "TrayIndexer Motor X1 Error";
	m_mapErrorAlertTable["T1I001"] = "TrayIndexer 1 Not Selected Error";
	m_mapErrorAlertTable["T1I002"] = "TrayIndexer 1 Invalid Tray Number Error";
	m_mapErrorAlertTable["T1I003"] = "TrayIndexer 1 Next Tray On Track Error";
	m_mapErrorAlertTable["T1I004"] = "TrayIndexer 1 Current Tray Exist Error";
	m_mapErrorAlertTable["T1I005"] = "TrayIndexer 1 Load Tray Error";
	m_mapErrorAlertTable["T1I006"] = "TrayIndexer 1 Tray Not Exist Error";
	m_mapErrorAlertTable["T1I007"] = "TrayIndexer 1 Down Sensor Not On  Error";
	m_mapErrorAlertTable["T1I008"] = "TrayIndexer 1 Down Sensor Still On  Error";
	m_mapErrorAlertTable["T2I000"] = "TrayIndexer Motor X2 Error";
	m_mapErrorAlertTable["T2I001"] = "TrayIndexer 2 Not Selected Error";
	m_mapErrorAlertTable["T2I002"] = "TrayIndexer 2 Invalid Tray Number Error";
	m_mapErrorAlertTable["T2I003"] = "TrayIndexer 2 Next Tray On Track Error";
	m_mapErrorAlertTable["T2I004"] = "TrayIndexer 2 Current Tray Exist Error";
	m_mapErrorAlertTable["T2I005"] = "TrayIndexer 2 Load Tray Error";
	m_mapErrorAlertTable["T2I006"] = "TrayIndexer 2 Tray Not Exist Error";
	m_mapErrorAlertTable["T2I007"] = "TrayIndexer 2 Down Sensor Not On  Error";
	m_mapErrorAlertTable["T2I008"] = "TrayIndexer 2 Down Sensor Still On  Error";
	m_mapErrorAlertTable["TA1000"] = "TA1 Not Selected Error";
	m_mapErrorAlertTable["TA1001"] = "TA1 Motor X Error";
	m_mapErrorAlertTable["TA1002"] = "TA1 Motor Z Error";
	m_mapErrorAlertTable["TA1003"] = "TA1 Motor Z In Danger Zone";
	m_mapErrorAlertTable["TA1010"] = "TA1 Pick Error";
	m_mapErrorAlertTable["TA1011"] = "TA1 Place Error";
	m_mapErrorAlertTable["TA2000"] = "TA2 Not Selected Error";
	m_mapErrorAlertTable["TA2001"] = "TA2 Motor X Error";
	m_mapErrorAlertTable["TA2002"] = "TA2 Motor X In Danger Zone";
	m_mapErrorAlertTable["TA2010"] = "TA2 Pick Glass1 Error";
	m_mapErrorAlertTable["TA2011"] = "TA2 Pick Glass2 Error";
	m_mapErrorAlertTable["TA2012"] = "TA2 Pick Glass Error";
	m_mapErrorAlertTable["TA2013"] = "TA2 Place Glass1 Error";
	m_mapErrorAlertTable["TA2014"] = "TA2 Place Glass2 Error";
	m_mapErrorAlertTable["TA2015"] = "TA2 Place Glass Error";
	m_mapErrorAlertTable["TA2016"] = "TA2 PR Module Not_Selected";
	m_mapErrorAlertTable["TA3000"] = "TA3 Not Selected Error";
	m_mapErrorAlertTable["TA3001"] = "TA3 Motor X Error";
	m_mapErrorAlertTable["TA3002"] = "TA3 Motor X In Danger Zone";
	m_mapErrorAlertTable["TA3010"] = "TA3Pick Glass1 Error";
	m_mapErrorAlertTable["TA3011"] = "TA3 Pick Glass2 Error";
	m_mapErrorAlertTable["TA3012"] = "TA3 Pick Glass Error";
	m_mapErrorAlertTable["TA3013"] = "TA3 Place Glass1 Error";
	m_mapErrorAlertTable["TA3014"] = "TA3 Place Glass2 Error";
	m_mapErrorAlertTable["TA3015"] = "TA3 Place Glass Error";
	m_mapErrorAlertTable["TA3016"] = "TA3 Reject Glass Limit Hit";
	m_mapErrorAlertTable["TA3017"] = "TA3 Glass1 PR1 Error";
	m_mapErrorAlertTable["TA3018"] = "TA3 Glass1 PR2 Error";
	m_mapErrorAlertTable["TA3019"] = "TA3 Glass2 PR1 Error";
	m_mapErrorAlertTable["TA3020"] = "TA3 Glass2 PR2 Error";
	m_mapErrorAlertTable["TA3021"] = "TA3 Glass1 PR1 Glass2 PR1 Error";
	m_mapErrorAlertTable["TA3022"] = "TA3 Glass1 PR1 Glass2 PR2 Error";
	m_mapErrorAlertTable["TA3023"] = "TA3 Glass1 PR2 Glass2 PR1 Error";
	m_mapErrorAlertTable["TA3024"] = "TA3 Glass1 PR2 Glass2 PR2 Error";
	m_mapErrorAlertTable["TA4001"] = "TA4 Not Selected Error";
	m_mapErrorAlertTable["TA4002"] = "TA4 Motor X Error";
	m_mapErrorAlertTable["TA4003"] = "TA4 Motor X In Danger Zone";
	m_mapErrorAlertTable["TA4010"] = "TA4 Pick Glass1 Error";
	m_mapErrorAlertTable["TA4011"] = "TA4 Pick Glass2 Error";
	m_mapErrorAlertTable["TA4012"] = "TA4 Pick Glass Error";
	m_mapErrorAlertTable["TA4013"] = "TA4 Place Glass1 Error";
	m_mapErrorAlertTable["TA4014"] = "TA4 Place Glass2 Error";
	m_mapErrorAlertTable["TA4015"] = "TA4 Place Glass Error";
	m_mapErrorAlertTable["TA4016"] = "TA4 Glass1 Vacuum Not On Error";
	m_mapErrorAlertTable["TA4017"] = "TA4 Glass2 Vacuum Not On Error";
	m_mapErrorAlertTable["TA4018"] = "TA4 FPC1 Vacuum Not On Error";
	m_mapErrorAlertTable["TA4019"] = "TA4 FPC2 Vacuum Not On Error";
	m_mapErrorAlertTable["TA4021"] = "TA4 External Arm Sol Not On Error";
	m_mapErrorAlertTable["TA4022"] = "TA4 External Arm Sol Not Off Error";
	m_mapErrorAlertTable["TA4023"] = "TA4 PR Module Not Selected Error";
	m_mapErrorAlertTable["TA4024"] = "TA4 Glass & FPC Vacuum State Not Match Error";
	m_mapErrorAlertTable["TA4025"] = "TA4 Reject Glass Limit Hit Error";
	m_mapErrorAlertTable["TA4026"] = "TA4 MainBond1 Glass1 PR1 Error";
	m_mapErrorAlertTable["TA4027"] = "TA4 MainBond1 Glass2 PR1 Error";
	m_mapErrorAlertTable["TA4028"] = "TA4 MainBond2 Glass1 PR1 Error";
	m_mapErrorAlertTable["TA4029"] = "TA4 MainBond2 Glass2 PR1 Error";
	m_mapErrorAlertTable["TA4030"] = "TA4 MainBond1 Glass1 PR2 Error";
	m_mapErrorAlertTable["TA4031"] = "TA4 MainBond1 Glass2 PR2 Error";
	m_mapErrorAlertTable["TA4032"] = "TA4 MainBond2 Glass1 PR2 Error";
	m_mapErrorAlertTable["TA4033"] = "TA4 MainBond2 Glass2 PR2 Error";
	m_mapErrorAlertTable["TA4034"] = "TA4 DownLook Reject Glass1 Limit Hit Error";
	m_mapErrorAlertTable["TA4035"] = "TA4 DownLook Reject Glass2 Limit Hit Error";
	m_mapErrorAlertTable["TA4036"] = "TA4 DownLook Glass1 PR1 Error";
	m_mapErrorAlertTable["TA4037"] = "TA4 DownLook Glass1 PR2 Error";
	m_mapErrorAlertTable["TA4038"] = "TA4 DownLook Glass2 PR1 Error";
	m_mapErrorAlertTable["TA4039"] = "TA4 DownLook Glass2 PR2 Error";
	m_mapErrorAlertTable["TA4040"] = "TA4 Motor T1 Error";
	m_mapErrorAlertTable["TA4041"] = "TA4 Motor T2 Error";
	m_mapErrorAlertTable["TA5000"] = "TA5 Not Selected Error";
	m_mapErrorAlertTable["TA5001"] = "TA5Motor X Error";
	m_mapErrorAlertTable["TA5002"] = "TA5Motor X In Danger Zone";
	m_mapErrorAlertTable["TA5010"] = "TA5 Pick Glass1 Error";
	m_mapErrorAlertTable["TA5011"] = "TA5 Pick Glass2 Error";
	m_mapErrorAlertTable["TA5012"] = "TA5 Pick Glass Error";
	m_mapErrorAlertTable["TA5013"] = "TA5 Place Glass1 Error";
	m_mapErrorAlertTable["TA5014"] = "TA5 Place Glass2 Error";
	m_mapErrorAlertTable["TA5015"] = "TA5 Place Glass Error";
	m_mapErrorAlertTable["TA5016"] = "TA5 Glass Exist Error";
	m_mapErrorAlertTable["TA5017"] = "TA5 Glass Exist Or OutBelt Not Move Error";
	m_mapErrorAlertTable["TA5018"] = "TA5 Glass & FPC Vacuum State Not Match Error";
	m_mapErrorAlertTable["TA5019"] = "TA5 Glass1 Vacuum Not On Error";
	m_mapErrorAlertTable["TA5020"] = "TA5 Glass2 Vacuum Not On Error";
	m_mapErrorAlertTable["TA5021"] = "TA5 FPC1 Vacuum Not On Error";
	m_mapErrorAlertTable["TA5022"] = "TA5 FPC2 Vacuum Not On Error";
	m_mapErrorAlertTable["TEMP0000"] = "MB1 Head Heater Error";
	m_mapErrorAlertTable["TEMP0001"] = "MB2 Head Heater Error";
	m_mapErrorAlertTable["TEMP0002"] = "MB1 Base Heater Error";
	m_mapErrorAlertTable["TEMP0003"] = "MB2 Base Heater Error";
	m_mapErrorAlertTable["TEMP0004"] = "ACF1 Heater Error";
	m_mapErrorAlertTable["TEMP0005"] = "PB1 Heater Error";
	m_mapErrorAlertTable["TEMP0006"] = "MB3 Head Heater Error";
	m_mapErrorAlertTable["TEMP0007"] = "MB4 Head Heater Error";
	m_mapErrorAlertTable["TEMP0008"] = "MB3 Base Heater Error";
	m_mapErrorAlertTable["TEMP0009"] = "MB4 Base Heater Error";
	m_mapErrorAlertTable["TEMP0010"] = "ACF2 Heater Error";
	m_mapErrorAlertTable["TEMP0011"] = "PB2 Heater Error";
	m_mapErrorAlertTable["TEMP0050"] = "Temperature Controller Control Error";
	m_mapErrorAlertTable["TEMP0060"] = "Temperature Monitor Error";
	m_mapErrorAlertTable["TEMP0080"] = "Heater Communication Error";
	m_mapErrorAlertTable["TEMP0100"] = "MB1 Head Heater Not On Error";
	m_mapErrorAlertTable["TEMP0101"] = "MB2 Head Heater Not On Error";
	m_mapErrorAlertTable["TEMP0102"] = "MB1 Base Heater Not On Error";
	m_mapErrorAlertTable["TEMP0103"] = "MB2 Base Heater Not On Error";
	m_mapErrorAlertTable["TEMP0104"] = "ACF1 Heater Not On Error";
	m_mapErrorAlertTable["TEMP0105"] = "PB1 Heater Not On Error";
	m_mapErrorAlertTable["TEMP0106"] = "MB3 Head Heater Not On Error";
	m_mapErrorAlertTable["TEMP0107"] = "MB4 Head Heater Not On Error";
	m_mapErrorAlertTable["TEMP0108"] = "MB3 Base Heater Not On Error";
	m_mapErrorAlertTable["TEMP0109"] = "MB4 Base Heater Not On Error";
	m_mapErrorAlertTable["TEMP0110"] = "ACF2 Heater Not On Error";
	m_mapErrorAlertTable["TEMP0111"] = "PB2 Heater Not On Error";
	m_mapErrorAlertTable["TEMP0200"] = "MB1 Head Heater Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0201"] = "MB2 Head Heater Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0202"] = "MB1 Base Heater Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0203"] = "MB2 Base Heater Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0204"] = "ACF1 Heater Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0205"] = "PB1 Heater Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0206"] = "MB3 Head Heater Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0207"] = "MB4 Head Heater Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0208"] = "MB3 Base Heater Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0209"] = "MB4 Base Heater Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0210"] = "ACF2 Heater Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0211"] = "PB2 Heater Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0300"] = "MB1 Head Temperature Not Ready";
	m_mapErrorAlertTable["TEMP0301"] = "MB2 Head Temperature Not Ready";
	m_mapErrorAlertTable["TEMP0302"] = "MB1 Base Temperature Not Ready";
	m_mapErrorAlertTable["TEMP0303"] = "MB2 Base Temperature Not Ready";
	m_mapErrorAlertTable["TEMP0304"] = "ACF1 Temperature Not Ready";
	m_mapErrorAlertTable["TEMP0305"] = "PB1 Temperature Not Ready";
	m_mapErrorAlertTable["TEMP0306"] = "MB3 Head Temperature Not Ready";
	m_mapErrorAlertTable["TEMP0307"] = "MB4 Head Temperature Not Ready";
	m_mapErrorAlertTable["TEMP0308"] = "MB3 Base Temperature Not Ready";
	m_mapErrorAlertTable["TEMP0309"] = "MB4 Base Temperature Not Ready";
	m_mapErrorAlertTable["TEMP0310"] = "ACF2 Temperature Not Ready";
	m_mapErrorAlertTable["TEMP0311"] = "PB2 Temperature Not Ready";
	m_mapErrorAlertTable["TEMP0400"] = "MB1 Head Monitor Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0401"] = "MB2 Head Monitor Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0402"] = "MB1 Base Monitor Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0403"] = "MB2 Base Monitor Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0404"] = "ACF1 Monitor Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0405"] = "PB1 Monitor Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0406"] = "MB3 Head Monitor Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0407"] = "MB4 Head Monitor Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0408"] = "MB3 Base Monitor Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0409"] = "MB4 Base Monitor Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0410"] = "ACF2 Monitor Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0411"] = "PB2 Monitor Temperature Out of Range Error";
	m_mapErrorAlertTable["TEMP0500"] = "ACF1 Head Heater Ctrl Error";
	m_mapErrorAlertTable["TEMP0501"] = "ACF2 Head Heater Ctrl Error";
	m_mapErrorAlertTable["TEMP0502"] = "PB1 Base Heater Ctrl Error";
	m_mapErrorAlertTable["TEMP0503"] = "PB2 Base Heater Ctrl Error";
	m_mapErrorAlertTable["TEMP0504"] = "MB1 Heater Ctrl Error";
	m_mapErrorAlertTable["TEMP0505"] = "MB2 Heater Ctrl Error";
	m_mapErrorAlertTable["TEMP0506"] = "MB3 Head Heater Ctrl Error";
	m_mapErrorAlertTable["TEMP0507"] = "MB4 Head Heater Ctrl Error";
	m_mapErrorAlertTable["TEMP0508"] = "MB1 Base Heater Ctrl Error";
	m_mapErrorAlertTable["TEMP0509"] = "MB2 Base Heater Ctrl Error";
	m_mapErrorAlertTable["TEMP0510"] = "MB3 Base Heater Ctrl Error";
	m_mapErrorAlertTable["TEMP0511"] = "MB4 Base Heater Ctrl Error";
	m_mapErrorAlertTable["TEMP0600"] = "MB1 Head Heater Mon Error";
	m_mapErrorAlertTable["TEMP0601"] = "MB2 Head Heater Mon Error";
	m_mapErrorAlertTable["TEMP0602"] = "MB1 Base Heater Mon Error";
	m_mapErrorAlertTable["TEMP0603"] = "MB2 Base Heater Mon Error";
	m_mapErrorAlertTable["TEMP0604"] = "ACF1 Heater Mon Error";
	m_mapErrorAlertTable["TEMP0605"] = "PB1 Heater Mon Error";
	m_mapErrorAlertTable["TEMP0606"] = "MB3 Head Heater Mon Error";
	m_mapErrorAlertTable["TEMP0607"] = "MB4 Head Heater Mon Error";
	m_mapErrorAlertTable["TEMP0608"] = "MB3 Base Heater Mon Error";
	m_mapErrorAlertTable["TEMP0609"] = "MB4 Base Heater Mon Error";
	m_mapErrorAlertTable["TEMP0610"] = "ACF2 Heater Mon Error";
	m_mapErrorAlertTable["TEMP0611"] = "PB2 Heater Mon Error";
	m_mapErrorAlertTable["TEMP0700"] = "Litec Thermocouple Connection Error";
	m_mapErrorAlertTable["TEMP0701"] = "Litec Connection Sensor Connection Error";
	m_mapErrorAlertTable["TEMP0702"] = "Litec Temperature Out of Alarm Band Error";
	m_mapErrorAlertTable["TEMP0703"] = "Litec Temperature Hit Limit";
	m_mapErrorAlertTable["TEMP0704"] = "Litec Heater Open Error";
	m_mapErrorAlertTable["TEMP0705"] = "Litec Protection Triggered!";
	m_mapErrorAlertTable["TEMP0706"] = "Litec Chip ADC Recovery Fail";
	m_mapErrorAlertTable["TEMP0707"] = "Litec Control Noise Protection Triggered!";
	m_mapErrorAlertTable["TEMP0708"] = "Litec Dynamic Protection Triggered!";
	m_mapErrorAlertTable["TEMP0709"] = "Litec Control Static Protection Triggered!";
	m_mapErrorAlertTable["TEMP0710"] = "Litec Control Ramping Protection Triggered!";
	m_mapErrorAlertTable["TEMP0800"] = "MB1 Head Heater Communication Error";
	m_mapErrorAlertTable["TEMP0801"] = "MB2 Head Heater Communication Error";
	m_mapErrorAlertTable["TEMP0802"] = "MB1 Base Heater Communication Error";
	m_mapErrorAlertTable["TEMP0803"] = "MB2 Base Heater Communication Error";
	m_mapErrorAlertTable["TEMP0804"] = "ACF1 Heater Communication Error";
	m_mapErrorAlertTable["TEMP0805"] = "PB1 Heater Communication Error";
	m_mapErrorAlertTable["TEMP0806"] = "MB3 Head Heater Communication Error";
	m_mapErrorAlertTable["TEMP0807"] = "MB4 Head Heater Communication Error";
	m_mapErrorAlertTable["TEMP0808"] = "MB3 Base Heater Communication Error";
	m_mapErrorAlertTable["TEMP0809"] = "MB4 Base Heater Communication Error";
	m_mapErrorAlertTable["TEMP0810"] = "ACF2 Heater Communication Error";
	m_mapErrorAlertTable["TEMP0811"] = "PB2 Heater Communication Error";
	m_mapErrorAlertTable["TEMP0812"] = "Temperture Controller Unit 1 Error";
	m_mapErrorAlertTable["TEMP0813"] = "Temperture Controller Unit 2 Error";
	m_mapErrorAlertTable["TEMP0814"] = "Temperture Controller Unit 3 Error";
	m_mapErrorAlertTable["TEMP0815"] = "Temperture Controller Unit 4 Error";
	m_mapErrorAlertTable["TEMP0816"] = "Temperture Controller Unit 5 Error";
	m_mapErrorAlertTable["TEMP0817"] = "Temperture Controller Unit 6 Error";
	m_mapErrorAlertTable["TEMP0818"] = "Temperture Controller Unit 7 Error";
	m_mapErrorAlertTable["TEMP0819"] = "Temperture Controller Unit 8 Error";
	m_mapErrorAlertTable["TEMP0820"] = "Temperture Controller Unit 9 Error";
	m_mapErrorAlertTable["TEMP0821"] = "Temperture Controller Unit 10 Error";
	m_mapErrorAlertTable["TEMP0822"] = "Temperture Controller Unit 11 Error";
	m_mapErrorAlertTable["TEMP0823"] = "Temperture Controller Unit 12 Error";
	m_mapErrorAlertTable["TH000"] = "TrayHolder Not Selected Error";
	m_mapErrorAlertTable["TH001"] = "TrayHolder PR Not Selected Error";
	m_mapErrorAlertTable["TH002"] = "TrayHolder Motor X1  Error";
	m_mapErrorAlertTable["TH003"] = "TrayHolder Motor X2  Error";
	m_mapErrorAlertTable["TH010"] = "TrayHolder PR Not Loaded  Error";
	m_mapErrorAlertTable["TH011"] = "TrayHolder PR1  Error";
	m_mapErrorAlertTable["TH012"] = "TrayHolder PR2  Error";
	m_mapErrorAlertTable["TH013"] = "TrayHolder Skip PR Limit Hit  Error";
	m_mapErrorAlertTable["TH014"] = "TrayHolder Tray Exist PR Not Loaded  Error";
	m_mapErrorAlertTable["TH015"] = "TrayHolder Skip Tray Limit Hit  Error";
	m_mapErrorAlertTable["TH016"] = "TrayHolder Alignment Not Loaded  Error";
	m_mapErrorAlertTable["TH017"] = "TrayHolder Information Not Set  Error";
	m_mapErrorAlertTable["TH020"] = "TrayHolder No Match LSI Type Tray  Error";
	m_mapErrorAlertTable["TIE001"] = "TrayInElevator Motor Z Error";
	m_mapErrorAlertTable["TIE002"] = "TrayInElevator Not Selected Error";
	m_mapErrorAlertTable["TIE003"] = "TrayInElevator Open Sensor Not On Error";
	m_mapErrorAlertTable["TIE004"] = "TrayInElevator No Tray Error";
	m_mapErrorAlertTable["TIE005"] = "TrayInElevator No COF Error";
	m_mapErrorAlertTable["TOE001"] = "TrayOutElevator Motor Z Error";
	m_mapErrorAlertTable["TOE002"] = "TrayOutElevator Not Selected Error";
	m_mapErrorAlertTable["TOE003"] = "TrayOutElevator Open Sensor Not On Error";
	return;
}

VOID CAC9000App::LogErrorAlarm(const CString &szError, BOOL bIsError)
{
	// open debugging file
	FILE *fp = fopen("D:\\sw\\AC9000\\LogFileDir\\ErrorAlarm.csv", "a+");

	if (fp != NULL)
	{
		CString szCurrError = szError;
		CString szTime(" "), szDescription(" ");
		char tmpbuf[129];
		struct tm *today;
		__time64_t ltime;

		_time64(&ltime);
		today = _localtime64(&ltime);
		memset(&tmpbuf[0], 0 , 129);
		strftime(tmpbuf, 128, "%Y-%m-%d %H:%M:%S", today);
		szTime.Format("%s", tmpbuf);

		
		LONG lfpPos = ftell(fp);
		fseek(fp, 0, SEEK_END);
		if (ftell(fp) < 10)
		{
			fprintf(fp, "Machine Model: %s\n", m_szMachineModel.GetString()); //20150124
			fprintf(fp, "Machine ID: %s\n", m_szMachineID.GetString());
			fprintf(fp, "SW Version: %s\n", m_szSoftVersion.GetString());
			fprintf(fp, "Release Date: %s\n", m_szReleaseDate.GetString());
			//fprintf(fp, "Date_Time,Error_ID,Error_Description,Message_Type,Error-To-Start_Duration(Sec),Last_Error, Error-To-Error_Duration(Sec)\n");
			fprintf(fp, "Date_Time,Error_ID,Error_Description,Message_Type,Error-To-Start_Duration(Sec)\n");
		}
		fseek(fp, lfpPos, SEEK_SET);

		if (szCurrError == "")
		{
			szCurrError = "NIL";
		}

		szDescription = m_mapErrorAlertTable[szError];
		if (szDescription == "")
		{
			szDescription = "Nil";
		}

		if (szCurrError == "START_BONDING")
		{
			szDescription = "START BONDING";
		}

		if (szCurrError == "HOST_PROGRAM_START")
		{
			szDescription = "HOST PROGRAM START";
		}

		if (szCurrError == "START_BONDING")
		{
			if (pCHouseKeeping->m_szLastError.GetLength() > 0 &&
				(DOUBLE)(m_clkBondStartTime - m_clkLastErrorTime) > 0.0)	//20150105
			{
				fprintf(fp, "%s,%s,%s,START_BONDING,%.2f,%s\n", 
					(char*)szTime.GetBuffer(0), 
					(char*)szCurrError.GetBuffer(0),
					(char*)szDescription.GetBuffer(0), 
					(DOUBLE)(m_clkBondStartTime - m_clkLastErrorTime) / CLOCKS_PER_SEC, 
					(char*)m_mapErrorAlertTable[pCHouseKeeping->m_szLastError].GetBuffer(0)/*, (DOUBLE)(clock() - m_clkLastErrorTime) / CLOCKS_PER_SEC*/);
			}
			else
			{
				fprintf(fp, "%s,%s,%s,START_BONDING,--,--\n", 
					(char*)szTime.GetBuffer(0), 
					(char*)szCurrError.GetBuffer(0),
					(char*)szDescription.GetBuffer(0)/*, (DOUBLE)(clock() - m_clkLastErrorTime) / CLOCKS_PER_SEC*/);
			}
		}
		else if (szCurrError == "HOST_PROGRAM_START")
		{
				fprintf(fp, "%s,%s,%s,HOST_PROGRAM_START,--,--\n", 
					(char*)szTime.GetBuffer(0), 
					(char*)szCurrError.GetBuffer(0),
					(char*)szDescription.GetBuffer(0));
		}
		else
		{
			fprintf(fp, "%s,%s,%s,%s,--,--\n", 
				(char*)szTime.GetBuffer(0), 
				(char*)szCurrError.GetBuffer(0), 
				(char*)szDescription.GetBuffer(0), 
				bIsError ? "Error" : "Alert"/*, (DOUBLE)(clock() - m_clkLastErrorTime) / CLOCKS_PER_SEC*/);
		}

		fclose(fp);
	}

}
#endif

VOID CAC9000App::GetAllStatus(const CString &szError)	// 20140729 Yip: Add Error Description In Status Log
{
	m_csGetAllStatusLock.Lock();	// 20140728 Yip

	SYSTEMTIME ltime;
	GetLocalTime(&ltime);

	CString szFileName;
	szFileName.Format("D:\\sw\\AC9000\\LogFileDir\\StationStatus_%04u%02u%02u.log", ltime.wYear, ltime.wMonth, ltime.wDay);

	// open debugging file
//	FILE *fp = fopen("D:\\sw\\AC9000\\LogFileDir\\StationStatus.log", "a+");
	FILE *fp = fopen(szFileName.GetString(), "a+");	// 20140903 Yip: Separate Station Status Log By Date

	if (fp != NULL)
	{
		__time64_t ltime;
		_time64(&ltime);
		CString szTime = _ctime64(&ltime);
		fprintf(fp, "\nAC9000 Station Status: %s", szTime.GetString());
		fprintf(fp, "Machine ID: %s\n", m_szMachineID.GetString());
		fprintf(fp, "SW Version: %s\n", m_szSoftVersion.GetString());

		if (!szError.IsEmpty())	// 20140729 Yip: Add Error Description In Status Log
		{
			fprintf(fp, "Error: %s Desp:%s\n", szError.GetString(), m_mapErrorAlertTable[szError].GetString());
		}
		else
		{
			fprintf(fp, "Error: NIL\n");
		}

		if (m_qState == STOPPING_Q)
		{
			fprintf(fp, "***************************************************************\n");
			fprintf(fp, "AUTOMATIC LOG STATION STATUS\n");
			fprintf(fp, "***************************************************************\n");
		}

		// station states
		fprintf(fp, "Machine State: %d\tBonding Mode: %d\n", State(), AutoMode);

#if 1 //20140220
		SFM_CStation	*pStation = NULL;
		CAppStation	*pAppStation = NULL;
		CString			szKey = _T("");
		POSITION		pos;
		fprintf(fp, "***********************State Map********************************\n");
		for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
		{
			m_pStationMap.GetNextAssoc(pos, szKey, pStation);
			pAppStation = dynamic_cast<CAppStation*>(pStation);
			fprintf(fp, "AppName:%s, State:%s\n", (char*)pAppStation->GetName().GetBuffer(0), (char*)(LPCTSTR)gszMODULE_STATE[pAppStation->State()]);
		}
		fprintf(fp, "***************************************************************\n");
#endif
		fprintf(fp, "TA1 Go State: %d\tTA1_Status: %ld\n", pCTA1->m_qSubOperation, (LONG)TA1_Status);
		pCTA1->PrintDebugSeq(fp); //20131010
		fprintf(fp, "TA2 Go State: %d\tTA2_Status: %ld\n", pCTA2->m_qSubOperation, (LONG)TA2_Status);
		pCTA2->PrintDebugSeq(fp); //20131010
		fprintf(fp, "TAManager Go State: %d\tTA_MANAGER_Status: %ld\n", pCTAManager->m_qSubOperation, (LONG)TA_MANAGER_Status);
		pCTAManager->PrintDebugSeq(fp);

		fprintf(fp, "InPickArm Go State: %d\tInPickArm_Status: %ld\n", pCInPickArm->m_qSubOperation, (LONG)IN_PICKARM_Status);
		pCInPickArm->PrintDebugSeq(fp);

		fprintf(fp, "WinEagle Go State: %d\tWinEagle_Status: %ld\n", pCWinEagle->m_qSubOperation, (LONG)WINEAGLE_Status);
		pCWinEagle->PrintDebugSeq(fp); //20150813

		fprintf(fp, "ACF1WH Go State: %d\tACF_WH_Status[ACFWH_1]: %ld\n", pCACF1WH->m_qSubOperation, (LONG)ACF_WH_Status[ACFWH_1]);
		pCACF1WH->PrintDebugSeq(fp); //20131010
		fprintf(fp, "ACF2WH Go State: %d\tACF_WH_Status[ACFWH_2]: %ld\n", pCACF2WH->m_qSubOperation, (LONG)ACF_WH_Status[ACFWH_2]);
		pCACF2WH->PrintDebugSeq(fp); //20131010
		fprintf(fp, "ACF Go State: %d\tACF_Status[ACF_1]: %ld\n", pCACF1->m_qSubOperation, (LONG)ACF_Status[ACF_1]);
		pCACF1->PrintDebugSeq(fp); //20131010
		
		fprintf(fp, "InspOpt Go State: %d\tINSPOPT_Status: %ld\n", pCInspOpt->m_qSubOperation, (LONG)INSPOPT_Status);
		pCInspOpt->PrintDebugSeq(fp); //20131010

		// autobond stats
		fprintf(fp, "\nMachine Statistics:\n");
		fprintf(fp, "Unit Bonded:\t%ld\n", m_ulUnitsBonded);
		fprintf(fp, "Unit Per Hr:\t%.2f\n", m_dUPH);

		fprintf(fp, "\t\t\tCPA Pick Error:\t%ld\n", m_ulCPAPickError);
		fprintf(fp, "\tGlass Picked:\t%ld\n", m_ulGlassPicked);

		fprintf(fp, "\t\tGlass Rejected:\t%ld\n", m_ulGlassRejected);
		fprintf(fp, "\t\t\tACF PB PR Error:\t%ld\n", m_ulACFWHPBPRError);
		fprintf(fp, "\t\t\tGlass Tol Error:\t%ld\n", m_ulGlassTolError);
		fprintf(fp, "\t\t\tAngle Tol Error:\t%ld\n", m_ulAngCorrError);
		
		fprintf(fp, "\nAll Glass Status:\n");
		//fprintf(fp, "TA1 Glass Exist: %ld\t %ld\n", (LONG)pCTA1->m_bGlass1Exist, (LONG)pCTA1->m_bGlass2Exist);
		//fprintf(fp, "TA2 Glass Status: %ld\t %ld\n", (LONG)TA2_GLASS_Status[GLASS1], (LONG)TA2_GLASS_Status[GLASS2]);
		
		fprintf(fp, "InPickArm Glass Exist: %ld\t \n", (LONG)IN_PICKARM_GLASS_Status);
		fprintf(fp, "TA1 Glass Exist: %ld\t \n", (LONG)TA1_GLASS_Status);
		fprintf(fp, "TA2 Glass Status: %ld\t \n", (LONG)TA2_GLASS_Status);
		fprintf(fp, "ACFWH Glass Status: %ld\t %ld\n", (LONG)ACF_GLASS_Status[ACFWH_1][GLASS1], (LONG)ACF_GLASS_Status[ACFWH_2][GLASS1]);
	

		//fprintf(fp, "\nHeater PID:\n");
		//pCLitecCtrl->PrintPIDPara(fp, ACF1_CH1);
		//pCLitecCtrl->PrintPIDPara(fp, ACF2_CH2);
		//pCLitecCtrl->PrintPIDPara(fp, PB1_CH3);
		//pCLitecCtrl->PrintPIDPara(fp, PB2_CH4);
		//pCLitecCtrl->PrintPIDPara(fp, MB1_WH_CH1);
		//pCLitecCtrl->PrintPIDPara(fp, MB2_WH_CH2);
		//pCLitecCtrl->PrintPIDPara(fp, MB3_WH_CH3);
		//pCLitecCtrl->PrintPIDPara(fp, MB4_WH_CH4);

		fprintf(fp, "\nEnd of Report\n");
		fprintf(fp, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");

		fclose(fp);
	}

	m_csGetAllStatusLock.Unlock();	// 20140728 Yip
}

BOOL CALLBACK EnumWndFunc(HWND hWnd, LPARAM lParam)
{
	RECT Rect;
	::GetWindowRect(hWnd, &Rect);

	if (Rect.right - Rect.left == 1286)
	{
		::PostMessage(hWnd, WM_CLOSE, 0, NULL);
	}

	return TRUE;
}

VOID CAC9000App::KillHmi()
{
	DisplayMessage("Killing HMI...");
	EnumThreadWindows(processInfo.dwThreadId, EnumWndFunc, NULL);

	// Check if AsmHmi.exe is running
	INT nCount = 0;

	do
	{
		Sleep(10);

		HANDLE hProcessSnap = NULL;
		PROCESSENTRY32 pe32 = {0};
		nCount = 0;

		// Take a snapshot of all processes in the system. 
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (hProcessSnap != INVALID_HANDLE_VALUE)
		{
			// Fill in the size of the structure before using it. 
			pe32.dwSize = sizeof(PROCESSENTRY32);

			// Walk the snapshot of the processes, and for each process, display information.
			if (Process32First(hProcessSnap, &pe32))
			{
				do
				{
					CString strName = pe32.szExeFile;
					strName = strName.MakeUpper();

					if (strName.Find("ASMHMI.EXE") >= 0)
					{
						nCount++;
					}
				} while (Process32Next(hProcessSnap, &pe32));
			}
		}
		CloseHandle(hProcessSnap);

	} while (nCount != 0);

	m_fRestartHmi = TRUE;
}

VOID CAC9000App::SetAlert(UINT unCode, BOOL bLightAndSound)
{
	CString		szCode(" ");
	INT			bResult;

	if ((unCode == gnOK) || (unCode == IDS_SYS_ABORT))
	{
		return;
	}

	pCHouseKeeping->m_bAlertExist = bLightAndSound; //20130115

	if ((bResult = szCode.LoadString(unCode)) != FALSE)
	{
		try
		{
			SendAlarm(szCode, ALM_DONT_LOG, 3000, gszALM_CLASS_ALERT);
#if 1 //20141124
			if (bLightAndSound && ((m_qState == AUTO_Q) || (m_qState == DEMO_Q) || (m_qState == STOPPING_Q)))
			{
				m_csLogErrorAlertLock.Lock();
				CString szInFilePath2("D:\\sw\\AC9000\\LogFileDir\\");
				CString szInFileName2("ErrorAlarm.csv");
				CheckFileSize(szInFilePath2, szInFileName2, 512); //512KB
				LogErrorAlarm(szCode, FALSE);
				m_csLogErrorAlertLock.Unlock();
			}
#endif
		}
		catch (CAsmException e)
		{
			TRACE1(_T("SendError Fail - %s\n"), szCode);
		}
	}
	else
	{
		szCode.Format(_T("%u: Alarm Code Not Found! From %s station"), unCode, m_szModuleName);
	}

	szCode = "Error: " + szCode  + " Desp: " + m_mapErrorAlertTable[szCode];
	DisplayMessage(szCode);

	return;
}

VOID CAC9000App::SetError(UINT unCode)
{
	CString		szCode;
	BOOL		bResult;
	LONG		lResponse = 0;

	if (unCode == gnOK)
	{
		return;
	}
	
	pCHouseKeeping->m_bErrorExist = TRUE; //20130115
		
	if ((bResult = szCode.LoadString(unCode)) != FALSE)
	{
		try
		{
			SendAlarm(szCode, ALM_FORCE_LOG, 3000, gszALM_CLASS_ERROR);
		}
		catch (CAsmException e)
		{
			TRACE1(_T("SendError Fail - %s\n"), szCode);
		}
	}
	else
	{
		szCode.Format(_T("%u: Alarm Code Not Found! From %s station"), unCode, m_szModuleName);
	}


	if ((m_qState == AUTO_Q) || (m_qState == DEMO_Q) || (m_qState == STOPPING_Q))
	{
		GetAllStatus(szCode);	// 20140728 Yip
#if 1 //20141124
		m_csLogErrorAlertLock.Lock();
		CString szInFilePath2("D:\\sw\\AC9000\\LogFileDir\\");
		CString szInFileName2("ErrorAlarm.csv");
		CheckFileSize(szInFilePath2, szInFileName2, 512); //512KB
		LogErrorAlarm(szCode);
		m_csLogErrorAlertLock.Unlock();
		m_clkLastErrorTime = clock(); //20150105
		pCHouseKeeping->m_szLastError = szCode;
#endif
	}

	__time64_t ltime;
	_time64(&ltime);
	CString szTime = _ctime64(&ltime);

	szCode = _T("[") + szTime + _T("]: ") + m_szModuleName + " Error: " + szCode + " Desp: " + m_mapErrorAlertTable[szCode];
	DisplayMessage(szCode);
}

////////////////////////////////////////////////////////////////////////////////////
//Machine - Load machine File
////////////////////////////////////////////////////////////////////////////////////


BOOL CAC9000App::LoadMachineFile(BOOL bLoad)
{
	CSingleLock slLock(&m_csMachine);
	slLock.Lock();

	CFileFind fileFind;

	CString szStationName;
	CString szCommand;
	CString szErrorMsg;

	CString szMsg = "";
	__time64_t ltime;

	//Create Param Directory if not exist
	if (!fileFind.FindFile(m_szMachinePath))
	{
		if (!m_pInitOperation->CreateFileDirectory(m_szMachinePath))
		{
			AfxMessageBox("Can not create Directory");
			slLock.Unlock();

			return FALSE;
		}
	}

	//Open Machine File (Create a new one if not found)
	m_smfMachine.Close(); //Clear File buffer

	if (!m_smfMachine.Open(m_szMachinePath + "\\" + m_szMachineFile, FALSE, TRUE))
	{
		AfxMessageBox("Can not load File \"" + m_szMachinePath + "\\" + m_szMachineFile + "\"");
		slLock.Unlock();

		return FALSE;
	}

	if (bLoad)
	{
		_time64(&ltime);
		szMsg = _ctime64(&ltime);

		szMsg += _T(": Loading Machine file...");
		DisplayMessage(szMsg);
		
		for (int i = 0; i < GetStationList().GetSize(); i++)
		{
			szStationName = GetStationList().GetAt(i)->GetStnName();
			GetStationList().GetAt(i)->IPC_LoadMachineParam();
			ProcessHMIRequest();
			KeepResponse();	// 20140714 Yip
		}

		for (int i = 0; i < GetPRStationList().GetSize(); i++)
		{
			GetPRStationList().GetAt(i)->IPC_LoadPRTaskStnMachinePara();
			ProcessHMIRequest();
			KeepResponse();	// 20140714 Yip
		}

		for (int i = 0; i < GetPRStationList().GetSize(); i++)
		{
			GetPRStationList().GetAt(i)->IPC_LoadMachineRecord();
			ProcessHMIRequest();
			KeepResponse();	// 20140714 Yip
		}

		if (g_bIsVisionNTInited)
		{
			CString szErrMsg = _T("");
			PRU *pPRU = NULL;
			for (INT i = 0; i < pCWinEagle->GetmyCalibPRUList().GetSize(); i++)
			{
				LONG lRetRecordID = 0;
				pPRU = pCWinEagle->GetmyCalibPRUList().GetAt(i);
				if (!PRS_DownloadRecordProcess(pPRU, TRUE, lRetRecordID, &szErrMsg))
				{
					ShowPRDownloadErrorMessage(pPRU, szErrMsg);
				}
				else if (pPRU->bLoaded)
				{
					szMsg.Format("Load Mcahine PR File %s, RetRecordID:%ld", pPRU->stDownloadRecordCmd.acFilename, lRetRecordID);
					DisplayMessage(szMsg);
					pCWinEagle->SetRecordIDToPRUCmd(pPRU, (PRU_ID)lRetRecordID);
				}
				KeepResponse();	// 20140714 Yip
			}
		}
		
		// Add me later 
		// loading statistics
		m_ulTotalGlassPicked					= m_smfMachine["Stats"]["m_ulTotalGlassPicked"];

		m_ulTotalGlassRejected					= m_smfMachine["Stats"]["m_ulTotalGlassRejected"];
		m_ulTotalInvalidGlass						= m_smfMachine["Stats"]["m_ulTotalInvalidGlass"];
		m_ulTotalACFWHPBPRError			= m_smfMachine["Stats"]["m_ulTotalACFWHPBPRError"];
		m_ulTotalGlassTolError					= m_smfMachine["Stats"]["m_ulTotalGlassTolError"];
		m_ulTotalAngCorrError					= m_smfMachine["Stats"]["m_ulTotalAngCorrError"];

		m_ulTotalCPAPickError					= m_smfMachine["Stats"]["m_ulTotalCPAPickError"];
		
		m_ulTotalUnitsBonded					= m_smfMachine["Stats"]["m_ulTotalUnitsBonded"];
	}
	else
	{
		_time64(&ltime);
		szMsg = _ctime64(&ltime);

		szMsg += _T(": Saving Machine file...");

		for (int i = 0; i < GetStationList().GetSize(); i++)
		{
			szStationName = GetStationList().GetAt(i)->GetStnName();
			GetStationList().GetAt(i)->IPC_SaveMachineParam();
			ProcessHMIRequest();
			KeepResponse();	// 20140714 Yip
		}

		for (int i = 0; i < GetPRStationList().GetSize(); i++)
		{
			GetPRStationList().GetAt(i)->IPC_SavePRTaskStnMachinePara();
			ProcessHMIRequest();
			KeepResponse();	// 20140714 Yip
		}

		for (int i = 0; i < GetPRStationList().GetSize(); i++)
		{
			GetPRStationList().GetAt(i)->IPC_SaveMachineRecord();
			ProcessHMIRequest();
			KeepResponse();	// 20140714 Yip
		}

		if (g_bIsVisionNTInited)
		{
			PRU *pPRU = NULL;
			CString szErrMsg = _T(" ");
			for (INT i = 0; i < pCWinEagle->GetmyCalibPRUList().GetSize(); i++)
			{
				pPRU = pCWinEagle->GetmyCalibPRUList().GetAt(i);
				if (!PRS_UploadRecordProcess(pPRU, &szErrMsg))
				{
					ShowPRUploadErrorMessage(pPRU, szErrMsg); //20130713
				}
				KeepResponse();	// 20140714 Yip
			}
		}
	}

	m_smfMachine.Update();
	slLock.Unlock();

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////
//IPC Service,  Request machine
////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////
//IPC Service,  Update machine
////////////////////////////////////////////////////////////////////////////////////

LONG CAC9000App::SrvUpdateMachine(IPC_CServiceMessage &svMsg)
{
	CSingleLock slLock(&m_csMachine);
	slLock.Lock();

	BOOL  bResult = TRUE;
	LONG  lStation = -1;
	LONG *pBuffer = NULL, *pData = NULL;

	BOOL bOpDone = TRUE;
	CString szStationName;
	CString szCommand;
	CString szErrorMsg;
	
	CString szMsg = "";
	__time64_t ltime;

	CString szpassword = pCHouseKeeping->szEngPassword;

	if (!HMIGetPassword("Please enter password", szpassword))
	{
		m_bSaveLoadDeviceMachine = FALSE; //20150824
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
		return 1;
	}

	try
	{
		INT	nMsgLen	= svMsg.GetMsgLen();		//in Byte
		pData		= new LONG[nMsgLen];	//The first index, in the message is lStation

		svMsg.GetMsg(nMsgLen, pData); //Decipher message
		pBuffer = pData;
		lStation = *pBuffer++; //pBuffer[0]

		LogAction(__FUNCTION__);
		LogParameter(__FUNCTION__, "Machine File", "Previous File = " + m_szMachineFile, "Updated File = " + m_szMachineFile);

		_time64(&ltime);
		szMsg = _ctime64(&ltime);

		szMsg += _T(": Saving Machine file...");
		DisplayMessage(szMsg);
		m_bSaveLoadDeviceMachine = TRUE; //20150824

		for (int i = 0; i < GetStationList().GetSize(); i++)
		{
			GetStationList().GetAt(i)->IPC_SaveMachineParam();
			ProcessHMIRequest();
		}
		//pCTrayIndexer->IPC_SaveMachineParam();

		for (int i = 0; i < GetPRStationList().GetSize(); i++)
		{
			GetPRStationList().GetAt(i)->IPC_SavePRTaskStnMachinePara();
			ProcessHMIRequest();
		}
		// PRUs
#if 1 //20141114 create path if it is not already exist
			CString szPath = _T(" ");
			CString szDevice = m_szDeviceFile;
			INT nFind = szDevice.ReverseFind('.');

			szPath = "d:\\sw\\AC9000\\Record\\" + szDevice.Left(nFind) + "\\";
			CreatePath(szPath);
#endif
		for (int i = 0; i < GetPRStationList().GetSize(); i++)
		{
			GetPRStationList().GetAt(i)->IPC_SaveMachineRecord();
			ProcessHMIRequest();
		}
	
		if (g_bIsVisionNTInited)
		{
			CString szErrMsg = _T(" ");
			PRU *pPRU = NULL;
			for (INT i = 0; i < pCWinEagle->GetmyCalibPRUList().GetSize(); i++)
			{
				pPRU = pCWinEagle->GetmyCalibPRUList().GetAt(i);
				if (!PRS_UploadRecordProcess(pPRU, &szErrMsg))
				{
					ShowPRUploadErrorMessage(pPRU, szErrMsg); //20130713
				}
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		bResult = FALSE;
	}

	if (bResult)
	{
		//Inform Stn to update profile
		//CAppStation* pAppStn = dynamic_cast<CAppStation*> (GetStation(m_pInitOperation->GetStationName(lStation)));
		//pAppStn->SetUpdateProfile(TRUE, FALSE);
		m_smfMachine.Update();
		
		HMIMessageBox(MSG_OK, "UPDATE MACHINE", "Machine File Saved");
	}
	else
	{
		HMIMessageBox(MSG_OK, "UPDATE MACHINE", "Machine File Save Error");
	}
	svMsg.InitMessage(sizeof(bResult), &bResult); //Return Values

	if (pData)
	{
		delete[] pData;  //Free up memory //klocwork fix 20121211
	}

	m_bSaveLoadDeviceMachine = FALSE; //20150824
	slLock.Unlock();
	return 1;
}

////////////////////////////////////////////////////////////////////////////////////
//Device File
////////////////////////////////////////////////////////////////////////////////////
BOOL CAC9000App::LoadDeviceFile(BOOL bLoad, BOOL bDeletePR)
{
	CSingleLock slLock(&m_csDevice);
	slLock.Lock();

	CFileFind			fileFind;
	CPositionAxisObj	*pPos = NULL;
	
	CString szStationName;
	CString szCommand;
	CString szErrorMsg;
	
	CString szMsg = "";
	__time64_t ltime;

	//Create Param Directory if not exist
	if (!fileFind.FindFile(m_szDevicePath))
	{
		if (!m_pInitOperation->CreateFileDirectory(m_szDevicePath))
		{
			AfxMessageBox("Can not create Directory");
			slLock.Unlock();

			return FALSE;
		}
	}

	//Open Device File (Create a new one if not found)
	m_smfDevice.Close(); //Clear File buffer

	if (!m_smfDevice.Open(m_szDevicePath + "\\" + m_szDeviceFile, FALSE, TRUE))
	{
		m_szDeviceFile = "CG144.prm";

		if (!m_smfDevice.Open(m_szDevicePath + "\\" + m_szDeviceFile, FALSE, TRUE))
		{
			AfxMessageBox("Cannot load File \"" + m_szDevicePath + "\\" + m_szDeviceFile + "\"");
			slLock.Unlock();
			return FALSE;
		}
	}

	if (!bLoad)
	{

		_time64(&ltime);
		szMsg = _ctime64(&ltime);

		szMsg += _T(": Creating Device file...");
		DisplayMessage(szMsg);

		for (int i = 0; i < GetStationList().GetSize(); i++)
		{
			GetStationList().GetAt(i)->IPC_SaveDeviceParam();
			ProcessHMIRequest();
			KeepResponse();	// 20140714 Yip
		}
	
		for (int i = 0; i < GetPRStationList().GetSize(); i++)
		{
			GetPRStationList().GetAt(i)->IPC_SavePRTaskStnDevicePara();
			ProcessHMIRequest();
			KeepResponse();	// 20140714 Yip
		}

		// PRUs
		for (int i = 0; i < GetPRStationList().GetSize(); i++)
		{
			GetPRStationList().GetAt(i)->IPC_SaveDeviceRecord();
			ProcessHMIRequest();
			KeepResponse();	// 20140714 Yip
		}
	
		if (g_bIsVisionNTInited)
		{
			CString szErrMsg = _T(" ");
			PRU *pPRU = NULL;
			for (INT i = 1; i < pCWinEagle->GetmyPRUList().GetSize(); i++)
			{
				pPRU = pCWinEagle->GetmyPRUList().GetAt(i);
				if (!PRS_UploadRecordProcess(pPRU, &szErrMsg))
				{
					ShowPRUploadErrorMessage(pPRU, szErrMsg); //20130713
				}
				KeepResponse();	// 20140714 Yip
			}
		}

		slLock.Unlock();
		return TRUE;
	}
	else
	{
		_time64(&ltime);
		szMsg = _ctime64(&ltime);

		szMsg += _T(": Loading Device file: [") + m_szDeviceFile + _T("]");
		DisplayMessage(szMsg);

		// Must Load HouseKeeping before other
		pCHouseKeeping->IPC_LoadDeviceParam();

		for (int i = 0; i < GetStationList().GetSize(); i++)
		{
			GetStationList().GetAt(i)->IPC_LoadDeviceParam();
			ProcessHMIRequest();
			KeepResponse();	// 20140714 Yip
		}

		for (int i = 0; i < GetPRStationList().GetSize(); i++)
		{
			GetPRStationList().GetAt(i)->IPC_LoadPRTaskStnDevicePara();
			ProcessHMIRequest();
			KeepResponse();	// 20140714 Yip
		}
		// PRUs
		for (int i = 0; i < GetPRStationList().GetSize(); i++)
		{
			GetPRStationList().GetAt(i)->IPC_LoadDeviceRecord();
			ProcessHMIRequest();
			KeepResponse();	// 20140714 Yip
		}

		if (g_bIsVisionNTInited)
		{
			CString szErrMsg = _T(" ");
			PRU *pPRU = NULL;
			for (INT i = 1; i < pCWinEagle->GetmyPRUList().GetSize(); i++)
			{
				LONG lRetRecordID = 0;
				pPRU = pCWinEagle->GetmyPRUList().GetAt(i);
				PRS_FreeRecordID(pPRU); //20180208
				if (!PRS_DownloadRecordProcess(pPRU, TRUE, lRetRecordID, &szErrMsg))
				{
					ShowPRDownloadErrorMessage(pPRU, szErrMsg);
				}
				else if(pPRU->bLoaded)
				{
					szMsg.Format("Load Device PR File %s, RetRecordID:%ld", pPRU->stDownloadRecordCmd.acFilename, lRetRecordID);
					DisplayMessage(szMsg);
					pCWinEagle->SetRecordIDToPRUCmd(pPRU, (PRU_ID)lRetRecordID);
				}

				ProcessHMIRequest();
				KeepResponse();	// 20140714 Yip
			}
		}

		// Set Temp 
		if (HEATER_FAILED == pCTempDisplay->SetHeaterTempFromData())
		{
			DisplayMessage("Set Heater Error");
		}

		//UpdateSetupStatus()
		//pCInsp1->UpdateSetupStatus();
		//pCInsp2->UpdateSetupStatus();
	}



	m_smfDevice.Update();
	slLock.Unlock();

	return TRUE;
}



LONG CAC9000App::SrvUpdateDevice(IPC_CServiceMessage &svMsg)
{
	CSingleLock slLock(&m_csDevice);
	slLock.Lock();

	BOOL  bResult = TRUE;
	
	CString szMsg = "";	
	__time64_t ltime;

	//LONG  lStation = -1;
	//LONG* pBuffer = NULL, *pData = NULL;

	try
	{
		//INT nMsgLen	= svMsg.GetMsgLen();		//in Byte
		//pData		= new LONG[nMsgLen];  //The first index, in the message is lStation

		//svMsg.GetMsg(nMsgLen, pData); //Decipher message
		//pBuffer  = pData;
		//lStation = *pBuffer++; //pBuffer[0]

		//switch (lStation)
		//{
		//}

		//Open Device File
		m_smfDevice.Close(); //Clear File buffer

		if (!m_smfDevice.Open(m_szDevicePath + "\\" + m_szDeviceFile, FALSE, FALSE))
		{
			AfxMessageBox("Device file missing!");
			bResult = FALSE;
		}
		else
		{
			LogAction(__FUNCTION__ + m_szDeviceFile);
			LogParameter(__FUNCTION__, "Device File", "Previous File = " + m_szDeviceFile, "Updated File = " + m_szDeviceFile);

			_time64(&ltime);
			szMsg = _ctime64(&ltime);

			szMsg += _T(": Saving Device file : [") + m_szDeviceFile + _T("]");
			DisplayMessage(szMsg);
			m_bSaveLoadDeviceMachine = TRUE; //20150824
						
			for (int i = 0; i < GetStationList().GetSize(); i++)
			{
				GetStationList().GetAt(i)->IPC_SaveDeviceParam();
				ProcessHMIRequest();
			}
			
			for (int i = 0; i < GetPRStationList().GetSize(); i++)
			{
				GetPRStationList().GetAt(i)->IPC_SavePRTaskStnDevicePara();
				ProcessHMIRequest();
			}
			// PRUs
#if 1 //20130812 create path if it is not already exist
			CString szPath = _T(" ");
			CString szDevice = m_szDeviceFile;
			INT nFind = szDevice.ReverseFind('.');

			szPath = "d:\\sw\\AC9000\\Record\\" + szDevice.Left(nFind) + "\\";
			CreatePath(szPath);
#endif
			for (int i = 0; i < GetPRStationList().GetSize(); i++)
			{
				GetPRStationList().GetAt(i)->IPC_SaveDeviceRecord();
				ProcessHMIRequest();
			}

			if (g_bIsVisionNTInited)
			{
				CString szErrMsg = _T(" ");
				PRU *pPRU = NULL;
				for (INT i = 1; i < pCWinEagle->GetmyPRUList().GetSize(); i++)
				{
					pPRU = pCWinEagle->GetmyPRUList().GetAt(i);
					
					szMsg.Format("saveRecord:%s", pPRU->stUploadRecordCmd.acFilename);
					DisplayMessage(szMsg);
					if (!PRS_UploadRecordProcess(pPRU, &szErrMsg))
					{
						ShowPRUploadErrorMessage(pPRU, szErrMsg); //20130713
					}
					ProcessHMIRequest();
				}
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		bResult = FALSE;
	}

	BOOL bOpDone = TRUE;

	if (bResult)
	{
		_time64(&ltime);
		szMsg = _ctime64(&ltime);
		szMsg += _T(": Device file Saved");
		DisplayMessage(szMsg);

		//Inform Stn to update position
		//CAppStation* pAppStn = dynamic_cast<CAppStation*> (GetStation(m_pInitOperation->GetStationName(lStation)));
		//pAppStn->SetUpdatePosition(TRUE, FALSE);
		m_smfDevice.Update();

		// 20140813 Yip: Update Device File Path In Registry After Updated Device
		WriteProfileString(gszPROFILE_SETTING, gszDEVICE_FILE_SETTING, m_szDeviceFile);
		WriteProfileString(gszPROFILE_SETTING, gszDEVICE_PATH_SETTING, m_szDevicePath);

		HMIMessageBox(MSG_OK, "UPDATE DEVICE", "Device File Saved");

	}
	else
	{
		_time64(&ltime);
		szMsg = _ctime64(&ltime);
		szMsg += _T(": Save Device file fail");
		DisplayMessage(szMsg);

		HMIMessageBox(MSG_OK, "UPDATE DEVICE", "Device File Save Error");

	}
	//if (pData)
	//{
	//	delete pData;
	//}
	m_bSaveLoadDeviceMachine = FALSE; //20150824

	slLock.Unlock();
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);

	return 1;
}

#ifdef EXCEL_OFFLINE_SETUP
LONG CAC9000App::HMI_LoadOfflineSetupFile(IPC_CServiceMessage &svMsg)
{
	BOOL	bResult = TRUE;
	LPTSTR	lpsz	= NULL;
	CString szOfflineFile, szPath, szFile, szExt;
	INT nFind = 0, nMsgLen = 0;

	try
	{
		nMsgLen	= svMsg.GetMsgLen();
		lpsz	= new TCHAR[nMsgLen];
		svMsg.GetMsg(lpsz, nMsgLen);	//Decipher message
		
		//Get New File Path and Name
		szOfflineFile = ToString(lpsz); //Convert string char to CString	
		nFind	= szOfflineFile.ReverseFind('\\');
		
		if (nFind == -1)
		{
			//Keyed in file name without directory info (e.g: "Abc")
			//Append current directory with file name
			szPath		= m_szOfflineSetupPath;
			szFile		= szOfflineFile;
			szOfflineFile	= szPath + "\\" + szFile;
		}
		else
		{
			//Keyed in file name with directory info (e.g: "C:\Xyz\Abc.prp")
			szPath	= szOfflineFile.Left(nFind);
			szFile = szOfflineFile.Right(szOfflineFile.GetLength() - nFind - 1);
		}

		//Check if device exist
		CFileFind fileFind;
		if (!fileFind.FindFile(szOfflineFile) || szPath.IsEmpty() || szFile.IsEmpty())
		{
			HmiMessage("File does not exist \"" + szOfflineFile + "\"", "Select proper File", glHMI_MBX_OK);
			bResult = FALSE;
		}

		if (bResult)
		{
			//Save old device file name and path
			CString szOldPath = m_szDevicePath;
			CString szOldFile = m_szDeviceFile;
			
			if (
				LoadExcelSetupInfo(szOfflineFile) == gnAMS_OK &&
				SetOfflineSetupGeneral() == gnAMS_OK &&
				SetOfflineSetupINWH() == gnAMS_OK &&
				SetOfflineSetupPBWH() == gnAMS_OK
				)
			{			
				HMIMessageBox(MSG_OK, "LOAD OFFLINE FILE", "Offline Setup File Loaded");
			}
			else
			{
				
				HMIMessageBox(MSG_OK, "LOAD OFFLINE FILE", "Offline Setup File Load Error");
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	if (lpsz)
	{
		//delete lpsz;
		delete[] lpsz;  //klocwork fix 20121211
	}

	BOOL bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}
#endif

LONG CAC9000App::SrvChangeDevice(IPC_CServiceMessage &svMsg)
{
	BOOL	bResult = TRUE;
	LPTSTR	lpsz	= NULL;
	CString szDevice, szPath, szFile, szExt;
	INT		nFind = 0, nMsgLen = 0;

	// Add me later
	// Check Heater Auto-tune in progress
	
	try
	{
		//Message Structure: char* strNewDeviceFileName  //E.g: C:\abc\yy\NewDevice.dev
		//Get New device file name
		nMsgLen	= svMsg.GetMsgLen();
		lpsz	= new TCHAR[nMsgLen];
		svMsg.GetMsg(lpsz, nMsgLen);	//Decipher message
		
		m_bSaveLoadDeviceMachine = TRUE; //20150824
		//Get New File Path and Name
		szDevice = ToString(lpsz); //Convert string char to CString	
		nFind	= szDevice.ReverseFind('\\');
		
		if (nFind == -1)
		{
			//Keyed in file name without directory info (e.g: "Abc")
			//Append current directory with file name
			szPath		= m_szDevicePath;
			szFile		= szDevice;
			szDevice	= szPath + "\\" + szFile;
		}
		else
		{
			//Keyed in file name with directory info (e.g: "C:\Xyz\Abc.prp")
			szPath	= szDevice.Left(nFind);
			szFile = szDevice.Right(szDevice.GetLength() - nFind - 1);
		}

		//Check if device exist
		CFileFind fileFind;
		if (!fileFind.FindFile(szDevice) || szPath.IsEmpty() || szFile.IsEmpty())
		{
			HmiMessage("File does not exist \"" + szDevice + "\"", "CHANGE DEVICE", glHMI_MBX_OK);
			bResult = FALSE;
		}

		if (bResult)
		{
			//Save old device file name and path
			CString szOldPath = m_szDevicePath;
			CString szOldFile = m_szDeviceFile;
			
			//Save new device file name and path
			m_szDeviceFile	= szFile;
			m_szDevicePath	= szPath;

			if (bResult = LoadDeviceFile()) //Load new device
			{
				LogParameter(__FUNCTION__, "Device File", "Previous File = " + szOldFile, "Loaded File = " + m_szDeviceFile);

				//Save Machine and Device and software version
				if (pCHouseKeeping->m_dPrevSWVersion < pCHouseKeeping->m_dCurSWVersion)
				{
					if (LoadDeviceFile(FALSE))
					{
						DisplayMessage("Device Data Convertion Finished");
					}
					else
					{
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						DisplayMessage("ERROR: Unable to save Device file");
						DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
						HMIMessageBox(MSG_OK, "LOAD DEVICE", "Device File Load Error");
						
						m_bSaveLoadDeviceMachine = FALSE; //20150824
						BOOL bOpDone = TRUE;
						svMsg.InitMessage(sizeof(BOOL), &bOpDone);
						return 1;
					}
				}

				SaveSoftwareVersion();
				
				// Reset all Data
				//pCTrayHolder->m_lCurTrayHolder		= TRAYHOLDER_1;
				INT i = 0;
				INT j = 0;
				//for (i = LSI_TYPE1; i < MAX_NUM_OF_LSI_TYPE; i++)
				//{
				//	//for (j = COL_1; j < pCTrayHolder->m_stTrayHolder[TRAYHOLDER_1].lTrayInCol; j++)
				//	//{
				//	//	if ((LSITypeNum)pCTrayHolder->m_stTrayHolder[TRAYHOLDER_1].lTrayTypeInCol[j] == i)
				//	//	{
				//	//		pCTrayHolder->m_WhCurTray[i].Col	= j + 1;
				//	//		pCTrayHolder->m_WhCurTray[i].Row	= 1;
				//	//		break;
				//	//	}
				//	//}

				//	//if (j == pCTrayHolder->m_stTrayHolder[TRAYHOLDER_1].lTrayInCol)
				//	//{
				//	//	pCTrayHolder->m_WhCurTray[i].Col	= 1;
				//	//	pCTrayHolder->m_WhCurTray[i].Row	= 1;
				//	//}

				//	//pCTrayHolder->m_WhCurPocket[i].Col	= 1;
				//	//pCTrayHolder->m_WhCurPocket[i].Row	= 1;
				//}

				WriteProfileString(gszPROFILE_SETTING, gszDEVICE_FILE_SETTING, m_szDeviceFile);
				WriteProfileString(gszPROFILE_SETTING, gszDEVICE_PATH_SETTING, m_szDevicePath);

				HMIMessageBox(MSG_OK, "LOAD DEVICE", "Device File Loaded");

			}
			else
			{
				//If can not load new device, Revert back to old one
				m_szDevicePath = szOldPath;
				m_szDeviceFile = szOldFile;
				//LoadDeviceFile();
				
				HMIMessageBox(MSG_OK, "LOAD DEVICE", "Device File Load Error");

			}
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	if (lpsz)
	{
		//delete lpsz;
		delete[] lpsz;  //klocwork fix 20121211
	}

	//if (bResult)
	//{
	//	////Inform all stations (and sub stations) to update device
	//	//CString			szStn;
	//	//SFM_CStation*	pStn;
	//	//CAppStation*		pAppStn;

	//	//for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
	//	//{
	//	//	m_pStationMap.GetNextAssoc(pos, szStn, pStn);
	//	//	pAppStn = dynamic_cast<CAppStation*> (pStn);			
	//	//	(pAppStn) ? pAppStn->SetUpdatePosition(TRUE) : 0;  //Inform all stns & sub stns
	//	//}
	//}

	m_bSaveLoadDeviceMachine = FALSE; //20150824
	BOOL bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}


LONG CAC9000App::SrvNewDevice(IPC_CServiceMessage &svMsg)
{
	BOOL bResult	= TRUE;
	LPTSTR lpsz		= NULL;
	CString			szDevice;
	CString			szPath;
	CString			szFile;
	CString			szExt;
	INT nFind		= 0;
	INT nFindExt	= 0;
	INT nMsgLen		= 0;
	BOOL bOpDone	= TRUE;

	LONG			lAnswer = rMSG_TIMEOUT;
	
	CString szMsg = "";
	__time64_t ltime;

	LogAction(__FUNCTION__ + m_szDeviceFile);

	try
	{
		//Message Structure: char* strDestFileName  //E.g: C:\abc\yy\NewDevice.dev
		//Get New device file name
		nMsgLen	= svMsg.GetMsgLen();
		lpsz	= new TCHAR[nMsgLen];
		svMsg.GetMsg(lpsz, nMsgLen);	//Decipher message
		
		//Get new File Path and Name
		szDevice = ToString(lpsz); //Convert string char to CString	
#if 0 //20121018
		//nFind	= szDevice.ReverseFind('\\');

		//if (nFind == -1)
		//{
#endif
			//Keyed in file name without directory info (e.g: "ABC.prm")
			nFindExt = szDevice.Find('.');
			if (nFindExt == -1)
			{
				//Keyed in file name without extension
				// Avoid space bar at the end
				//if (szDevice.GetAt(szDevice.GetLength()-1) == ' ')
				//{
				//	szDevice = szDevice.Left(szDevice.GetLength() - 1);
				//}
				szDevice += ".prm";
			}
			else // avoid user enter abc.prm.prm.....
			{
				//Keyed in file name with extension
				szDevice = szDevice.Left(nFindExt);
				//if (szDevice.GetAt(szDevice.GetLength()-1) == ' ')
				//{
				//	szDevice = szDevice.Left(szDevice.GetLength() - 1);
				//}
				szDevice += ".prm";
			}

			//Append current directory with file name
			szPath		= m_szDevicePath;
			szFile		= szDevice;
			szDevice	= szPath + "\\" + szFile;
#if 0 //20121018
		//}
		//else
		//{
		//	//Keyed in file name with directory info (e.g: "D:\SW\AC9000\Device\ABC.prm")
		//	szPath		= szDevice.Left(nFind);
		//	szFile		= szDevice.Right(szDevice.GetLength() - nFind - 1);
		//}
#endif
		// All that variabel can;t exist as the folder name 
		if (
			szFile.Find('\\') != -1 || szFile.Find('/') != -1 || szFile.Find(':') != -1 || 
			szFile.Find('*') != -1 || szFile.Find('?') != -1 || szFile.Find('"') != -1 ||
			szFile.Find('<') != -1 || szFile.Find('>') != -1 || szFile.Find('|') != -1 ||
			szFile.Find('!') != -1 || szFile.Find('@') != -1 || szFile.Find('#') != -1 || 
			szFile.Find('$') != -1 || szFile.Find('%') != -1 || szFile.Find('^') != -1 ||
			szFile.Find('&') != -1 || szFile.Find('*') != -1 || szFile.Find('(') != -1 ||
			szFile.Find(')') != -1 ||
			szFile.Find('[') != -1 || szFile.Find(']') != -1 || szFile.Find('+') != -1 || //20120104
			szFile.Find('=') != -1 || szFile.Find(' ') != -1
		   )
		{
			HMIMessageBox(MSG_OK, "NEW DEVICE", "File Name cannot include \\//:*?'<>|!@#$%^&*()[]+= characters and [WHITE SPACE]");
			bResult = FALSE;
		}

		if (szFile.GetLength() > 25)
		{
			HMIMessageBox(MSG_OK, "NEW DEVICE", "Length of filename must be less than 20 characters");
			bResult = FALSE;
		}

		
		//Save old device file path and name
		CString szOldPath = m_szDevicePath;
		CString szOldFile = m_szDeviceFile;

		//Save new device file path and name
		m_szDevicePath	= szPath;
		m_szDeviceFile	= szFile;

		if (bResult)
		{
			//Create Directory if not exist
			CFileFind fileFind;
			nFind = m_szDeviceFile.ReverseFind('.');
			if (!fileFind.FindFile("d:\\sw\\AC9000\\Record\\" + m_szDeviceFile.Left(nFind) + "\\"))
			{
				m_pInitOperation->CreateFileDirectory("d:\\sw\\AC9000\\Record\\" + m_szDeviceFile.Left(nFind) + "\\");
			}

			//check if path is valid
			if (szPath != szOldPath)
			{
				HMIMessageBox(MSG_OK, "NEW DEVICE", "Invalid Directory \"" + szPath + "\\\"");
				bResult = FALSE;
			}

			if (bResult)
			{
				//Copy current device file to new device file
				if (!m_smfDevice.SaveAs(szDevice, CStringMapFile::frmBINARY, FALSE))
				{
					//Device name exists!
					HMIMessageBox(MSG_OK, "NEW DEVICE", "Device Name Already Exist");
					bResult = FALSE;
				}
			}
			
			if (bResult)
			{
				//Close current file and open new device
				m_smfDevice.Close();

				if (!m_smfDevice.Open(szDevice, FALSE, FALSE))
				{
					HMIMessageBox(MSG_OK, "NEW DEVICE", "Cannot Load File \"" + szDevice + "\\");
					bResult = FALSE;
				}
			}

			if (bResult)
			{
				//Reset some parameters and load the new file
				//
				bResult = LoadDeviceFile(FALSE, FALSE);
				if (bResult)
				{
					LogParameter(__FUNCTION__, "Device File", "Previous File = " + szOldFile, "Created File = " + m_szDeviceFile);

					WriteProfileString(gszPROFILE_SETTING, gszDEVICE_FILE_SETTING, m_szDeviceFile);
					WriteProfileString(gszPROFILE_SETTING, gszDEVICE_PATH_SETTING, m_szDevicePath);

					HMIMessageBox(MSG_OK, "NEW DEVICE", "New Device File Created");

					_time64(&ltime);
					szMsg = _ctime64(&ltime);
					szMsg += _T(": Created Device file :") + m_szDeviceFile;
					DisplayMessage(szMsg);
				}
			}
		}

		if (!bResult)
		{
			//If cannot create a new device, Revert back to old one
			m_szDevicePath = szOldPath;
			m_szDeviceFile = szOldFile;

			HMIMessageBox(MSG_OK, "NEW DEVICE", "Unable to Create New Device File");
		}

	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}


	if (lpsz)
	{
		delete[] lpsz;  //klocwork fix 20121211
	}

	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}

LONG CAC9000App::SrvDeleteDevice(IPC_CServiceMessage &svMsg)
{
	BOOL	bResult = TRUE;
	LPTSTR	lpsz	= NULL;
	CString szDevice, szPath, szFile, szFolder, szTemp, szExt;
	INT		nFind = 0, nMsgLen = 0;

	try
	{
		//Message Structure: char* strNewDeviceFileName (e.g: "D:\SW\AC9000\Device\ABC.prm")
		//Get New device file name
		nMsgLen	= svMsg.GetMsgLen();
		lpsz	= new TCHAR[nMsgLen];
		svMsg.GetMsg(lpsz, nMsgLen);	//Decipher message
		
		//Get New File Path and Name
		szDevice = ToString(lpsz); //Convert string char to CString	
		nFind	= szDevice.ReverseFind('\\');
		
		if (nFind == -1)
		{
			//Keyed in file name without directory info (e.g: "ABC.prm")
			//Append current directory with file name
			szPath		= m_szDevicePath;
			szFile		= szDevice;
			szDevice	= szPath + "\\" + szFile;
		}
		else
		{
			//Keyed in file name with directory info (e.g: "D:\SW\AC9000\Device\ABC.prm")
			szPath		= szDevice.Left(nFind);
			szFile		= szDevice.Right(szDevice.GetLength() - nFind - 1);
		}

		nFind = szFile.ReverseFind('.');
		szFolder = "d:\\sw\\AC9000\\Record\\" + szFile.Left(nFind) + "\\";

		BOOL bOpDone = TRUE;

		//Check if device exist
		CFileFind fileFind;
		if (!fileFind.FindFile(szDevice) || szPath.IsEmpty() || szFile.IsEmpty())
		{
			HMIMessageBox(MSG_OK, "DELETE DEVICE", "File does not exist \"" + szDevice + "\"");
			bResult = FALSE;
		}

		if (bResult)
		{
			if (rMSG_CONTINUE == HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Delete file: " + szFile + "?"))
			{
				if (0 != remove(szDevice))
				{
					HMIMessageBox(MSG_OK, "DELETE DEVICE", "ERROR: Unable to Delete File!");
				}

				CFileOperation fo;
				fo.Delete(szFolder);
			}
			else
			{
				WriteHMIMess("Operation Cancelled!");
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	BOOL bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);

	if (lpsz)
	{
		delete[] lpsz;  //klocwork fix 20121211
	}

	if (bResult)
	{
		//Inform all stations (and sub stations) to update device
		CString			szStn;
		SFM_CStation	*pStn = NULL;
		CAppStation	*pAppStn;

		for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
		{
			m_pStationMap.GetNextAssoc(pos, szStn, pStn);
			pAppStn = dynamic_cast<CAppStation*>(pStn);
			(pAppStn) ? pAppStn->SetUpdatePosition(TRUE) : 0;  //Inform all stns & sub stns
		}
	}

	return 1;
}

LONG CAC9000App::SrvRestoreMachine(IPC_CServiceMessage &svMsg)
{
	if (rMSG_CONTINUE == HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "All changes to Machine file will be discarded. Continue?"))
	{
		LogAction(__FUNCTION__);
		LogParameter(__FUNCTION__, "Machine File", "Previous File = " + m_szMachineFile, "Restored File = " + m_szMachineFile);

		try
		{
			if (LoadMachineFile()) //Load machine file
			{
				HMIMessageBox(MSG_OK, "LOAD MACHINE", "Machine File Loaded");

			}
			else
			{
				HMIMessageBox(MSG_OK, "LOAD MACHINE", "Machine File Load Error");
			}
		}
		catch (CAsmException e)
		{
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			DisplayException(e);
			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		}
	}
	else
	{
		HMIMessageBox(MSG_OK, "LOAD MACHINE", "Operation Abort!");
	}

	BOOL bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);

	return 1;
}

LONG CAC9000App::SrvPrintMachine(IPC_CServiceMessage &svMsg)
{
	__time64_t ltime;
	CString szTime = "";

	_time64(&ltime);
	szTime = _ctime64(&ltime);

	FILE *fp = fopen("D:\\sw\\AC9000\\Data\\machine.txt", "w+");
	if (fp != NULL)
	{
		fprintf(fp, "******************************************************\n");
		fprintf(fp, "*                                                    *\n");
		fprintf(fp, "*     AC9000 High Precision Chip On Glass Bonder     *\n");
		fprintf(fp, "*                                                    *\n");
		fprintf(fp, "******************************************************\n\n");

		fprintf(fp, "Date: " + szTime);

		fprintf(fp, "Machine ID: %s\n", (const char*)m_szMachineID);
		
		for (int i = 0; i < GetStationList().GetSize(); i++)
		{
			GetStationList().GetAt(i)->PrintMachineSetup(fp);
			GetStationList().GetAt(i)->PrintCalibSetup(fp);
			GetStationList().GetAt(i)->PrintForceCalibData(fp);
		}

		for (int i = 0; i < GetStationList().GetSize(); i++)
		{
			GetStationList().GetAt(i)->PrintPRCalibSetup(fp);
		}

		fclose(fp);
	}

	HMI_PrintMachineDeviceInfoExcel(svMsg);
	BOOL bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);

	return 1;
}

LONG CAC9000App::SrvPrintDevice(IPC_CServiceMessage &svMsg)
{
	__time64_t ltime;
	CString szTime = "";

	_time64(&ltime);
	szTime = _ctime64(&ltime);

	FILE *fp = fopen("D:\\sw\\AC9000\\Data\\device.txt", "w+");
	if (fp != NULL)
	{
		fprintf(fp, "******************************************************\n");
		fprintf(fp, "*                                                    *\n");
		fprintf(fp, "*     AC9000 High Precision ACF On COF Bonder     *\n");
		fprintf(fp, "*                                                    *\n");
		fprintf(fp, "******************************************************\n\n");

		fprintf(fp, "Date: " + szTime);

		fprintf(fp, "Machine ID:	%s\n", (const char*)m_szMachineID);
		fprintf(fp, "Device Name:	%s\n", (const char*)m_szDeviceFile);
		
		CString szStationName;
		
		for (int i = 0; i < GetStationList().GetSize(); i++)
		{
			szStationName = GetStationList().GetAt(i)->GetStnName(); //20130228
			DisplayMessage("printDeviceSetup:" + szStationName);
			GetStationList().GetAt(i)->PrintDeviceSetup(fp);
		}

		for (int i = 0; i < GetStationList().GetSize(); i++)
		{
			GetStationList().GetAt(i)->PrintPRSetup(fp);
		}
		fclose(fp);
	}

	BOOL bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);

	return 1;

}
////////////////////////////////////////////////////////////////////////////////////
//Register Hmi Variables
////////////////////////////////////////////////////////////////////////////////////

VOID CAC9000App::RegisterVariables()
{
	try
	{
		RegVariable(_T("HMI_bTA1Z_Selected"), &HMI_bTA1Z_Selected);
		//Machine and Device File
		RegVariable(_T("ParamPath"), &m_szMachinePath);
		RegVariable(_T("DevicePath"), &m_szDevicePath);
		RegVariable(_T("DeviceFile"), &m_szDeviceFile);
		RegVariable(_T("MachineFile"), &m_szMachineFile);
		RegVariable(_T("SelectedFile"), &m_szSelectedFile);
#ifdef EXCEL_OFFLINE_SETUP
		RegVariable(_T("OfflineSetupPath"), &m_szOfflineSetupPath); 
		RegVariable(_T("OfflineSetupFile"), &m_szOfflineSetupFile); 
#endif

		//Lot Info
		RegVariable(_T("LotNumber"), &m_szLotNumber);
		RegVariable(_T("LotStartTime"), &m_szLotStartTime);
		RegVariable(_T("LotEndTime"), &m_szLotEndTime);
		
		//Software/ Machine info
		RegVariable(_T("MachineModel"), &m_szMachineModel);
		RegVariable(_T("MachineID"), &m_szMachineID);
		RegVariable(_T("OSImageNumber"), &m_szOSImageNumber);
		RegVariable(_T("SoftwareNumber"), &m_szSoftwareNumber);
		RegVariable(_T("SoftVersion"), &m_szSoftVersion);
		RegVariable(_T("ReleaseDate"), &m_szReleaseDate);

		RegVariable(_T("VisionOSImageNumber"), &m_szOSImageNumber);
		
		RegVariable(_T("LitecVersion1"), &m_szLitecVersion1);
		RegVariable(_T("LitecVersion2"), &m_szLitecVersion2);
		RegVariable(_T("LitecVersion3"), &m_szLitecVersion3);
		RegVariable(_T("LitecVersion4"), &m_szLitecVersion4);

		RegVariable(_T("MotionLibVersion"), &m_szMotionLibVersion); //20121210

		// S/W status
		RegVariable(_T("CurState"), &m_szState);

		// PR init
		RegVariable(_T("PRInit"), &g_bIsVisionNTInited);
		// Hardware init
		RegVariable(_T("HWInited"), &m_bHWInited);
		// Hardware Error
		RegVariable(_T("HWInitError"), &m_bHWInitError);
		RegVariable(_T("HWInitFatalErrorUI"), &m_bHWInitFatalErrorUI); //20130710
		
		// Invalid Init State
		RegVariable(_T("InvalidInitState"), &m_bInvalidInitState);
		// 
		RegVariable(_T("InitInstanceError"), &m_bInitInstanceError);
		
		RegVariable(_T("EcoreInited"), &m_bAllLitecInited); //20121004

		// Message windows
		RegVariable(_T("szHMIMess"), &m_szHMIMess);
		RegVariable(_T("bHMIMess"), &m_bHMIMess);

		// Diagnostic testing
		RegVariable(_T("bPRRepeatTest"), &bPRRepeatTest);

		// statistics
		RegVariable(_T("Stat_ulGlassPicked"), &m_ulGlassPicked);
		RegVariable(_T("Stat_ulTotalGlassPicked"), &m_ulTotalGlassPicked);

		RegVariable(_T("Stat_ulGlassRejected"), &m_ulGlassRejected);
		RegVariable(_T("Stat_ulTotalGlassRejected"), &m_ulTotalGlassRejected);
		RegVariable(_T("Stat_ulInvalidGlass"), &m_ulInvalidGlass);
		RegVariable(_T("Stat_ulTotalInvalidGlass"), &m_ulTotalInvalidGlass);
		RegVariable(_T("Stat_ulACFWHPBPRError"), &m_ulACFWHPBPRError);
		RegVariable(_T("Stat_ulTotalACFWHPBPRError"), &m_ulTotalACFWHPBPRError);

		RegVariable(_T("Stat_ulGlassTolError"), &m_ulGlassTolError);
		RegVariable(_T("Stat_ulTotalGlassTolError"), &m_ulTotalGlassTolError);
		RegVariable(_T("Stat_ulAngCorrError"), &m_ulAngCorrError);
		RegVariable(_T("Stat_ulTotalAngCorrError"), &m_ulTotalAngCorrError);

		RegVariable(_T("Stat_ulCPAPickError"), &m_ulCPAPickError);
		RegVariable(_T("Stat_ulTotalCPAPickError"), &m_ulTotalCPAPickError);

		RegVariable(_T("Stat_ulUnitsBonded"), &m_ulUnitsBonded);
		RegVariable(_T("Stat_ulTotalUnitsBonded"), &m_ulTotalUnitsBonded);

		RegVariable(_T("Stat_dUPH"), &m_dUPH);

		RegVariable(_T("MotorPosLmt"), &HMI_lMotorPosLmt);
		RegVariable(_T("MotorNegLmt"), &HMI_lMotorNegLmt);	
		RegVariable(_T("szMotorDirectionPng"), &HMI_szMotorDirectionPng);
		
		//RegVariable(_T("HMI_bIsEdgeCleaner"), &HMI_bIsEdgeCleaner);
		RegVariable(_T("HMI_bIsCOG902"), &HMI_bIsCOG902); //Milton need to add to HMI
		RegVariable(_T("HMI_lGatewayStatus"), &HMI_lGatewayStatus); //20130619

		RegVariable(_T("ulSettingPerformanceReportStartTime"), &HMI_ulSettingReportManagerStartTime);
		RegVariable(_T("Stat_ulDailyPerformanceReportPage"), &g_ulDailyPerformanceReportPage);	

		RegVariable(_T("szSettingReportManagerShift1"), &HMI_szSettingReportManagerShift1);
		RegVariable(_T("szSettingReportManagerShift2"), &HMI_szSettingReportManagerShift2);
		RegVariable(_T("szSettingReportManagerShift3"), &HMI_szSettingReportManagerShift3);
		RegVariable(_T("szSettingReportManagerShift4"), &HMI_szSettingReportManagerShift4);
		RegVariable(_T("szSettingReportManagerShift5"), &HMI_szSettingReportManagerShift5);
		RegVariable(_T("szSettingReportManagerShift6"), &HMI_szSettingReportManagerShift6);
		RegVariable(_T("szSettingReportManagerShift7"), &HMI_szSettingReportManagerShift7);
		RegVariable(_T("szSettingReportManagerShift8"), &HMI_szSettingReportManagerShift8);
		RegVariable(_T("szSettingReportManagerShift9"), &HMI_szSettingReportManagerShift9);
		RegVariable(_T("szSettingReportManagerShift10"), &HMI_szSettingReportManagerShift10);
		RegVariable(_T("szSettingReportManagerShift11"), &HMI_szSettingReportManagerShift11);
		RegVariable(_T("szSettingReportManagerShift12"), &HMI_szSettingReportManagerShift12);
		RegVariable(_T("szSettingReportManagerShift13"), &HMI_szSettingReportManagerShift13);
		RegVariable(_T("szSettingReportManagerShift14"), &HMI_szSettingReportManagerShift14);
		RegVariable(_T("szSettingReportManagerShift15"), &HMI_szSettingReportManagerShift15);
		RegVariable(_T("szSettingReportManagerShift16"), &HMI_szSettingReportManagerShift16);
		RegVariable(_T("szSettingReportManagerShift17"), &HMI_szSettingReportManagerShift17);
		RegVariable(_T("szSettingReportManagerShift18"), &HMI_szSettingReportManagerShift18);
		RegVariable(_T("szSettingReportManagerShift19"), &HMI_szSettingReportManagerShift19);
		RegVariable(_T("szSettingReportManagerShift20"), &HMI_szSettingReportManagerShift20);
		RegVariable(_T("szSettingReportManagerShift21"), &HMI_szSettingReportManagerShift21);
		RegVariable(_T("szSettingReportManagerShift22"), &HMI_szSettingReportManagerShift22);
		RegVariable(_T("szSettingReportManagerShift23"), &HMI_szSettingReportManagerShift23);
		RegVariable(_T("szSettingReportManagerShift24"), &HMI_szSettingReportManagerShift24);

		RegVariable(_T("HMI_bPRDebugInfo"), &g_bPRDebugInfo); //20150625
		RegVariable(_T("HMI_bEnableAnvilVac"), &g_bEnableAnvilVac); //20170816
		RegVariable(_T("HMI_bUplookPREnable"), &g_bUplookPREnable); //20170816
		DisplayMessage("Hmi Variables Registered...");

		// commands
		m_comServer.IPC_REG_SERVICE_COMMAND("CloseApp", SrvCloseApp);
		m_comServer.IPC_REG_SERVICE_COMMAND("RestartHMI", SrvRestartHMI);
		m_comServer.IPC_REG_SERVICE_COMMAND("SetMachineID", SetMachineID);
		m_comServer.IPC_REG_SERVICE_COMMAND("CleanOperation", HMI_CleanOperation); //20121112

		m_comServer.IPC_REG_SERVICE_COMMAND("Stat_ResetStat", HMI_ResetStat);
		m_comServer.IPC_REG_SERVICE_COMMAND("GetAllStatus", HMI_GetAllStatus);

		m_comServer.IPC_REG_SERVICE_COMMAND("SetAllModSelected", HMI_SetAllModSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND("CheckAllModSelected", HMI_CheckAllModSelected);
		m_comServer.IPC_REG_SERVICE_COMMAND("SetAllPRSelected", HMI_SetAllPRSelected);

		m_comServer.IPC_REG_SERVICE_COMMAND("SetAllDebugSeq", HMI_SetAllDebugSeq);
		m_comServer.IPC_REG_SERVICE_COMMAND("SetAllLogMotion", HMI_SetAllLogMotion); //20130422

		m_comServer.IPC_REG_SERVICE_COMMAND("RestoreMachine", SrvRestoreMachine);
			
//p20120829
		m_comServer.IPC_REG_SERVICE_COMMAND("StatisticsLogFile", HMI_StatisticsLogFile);
//p20120829:end

		// Set Protection
		m_comServer.IPC_REG_SERVICE_COMMAND("SelectStation", HMI_SelectStation);
		m_comServer.IPC_REG_SERVICE_COMMAND("SelectStationMotor", HMI_SelectStationMotor);
		m_comServer.IPC_REG_SERVICE_COMMAND("SetMotorProtectPosLmt", HMI_SetMotorProtectPosLmt);
		m_comServer.IPC_REG_SERVICE_COMMAND("SetMotorProtectNegLmt", HMI_SetMotorProtectNegLmt);
		m_comServer.IPC_REG_SERVICE_COMMAND("EnterSoftwareLimitPage", HMI_EnterSoftwareLimitPage);
		m_comServer.IPC_REG_SERVICE_COMMAND("LeaveSoftwareLimitPage", HMI_LeaveSoftwareLimitPage);
		m_comServer.IPC_REG_SERVICE_COMMAND("AC9000_SetDiagnSteps", HMI_SetDiagnSteps);
		m_comServer.IPC_REG_SERVICE_COMMAND("AC9000_SWLimitIndexMotorPos", HMI_SWLimitIndexMotorPos);
		m_comServer.IPC_REG_SERVICE_COMMAND("AC9000_SWLimitIndexMotorNeg", HMI_SWLimitIndexMotorNeg);

		m_comServer.IPC_REG_SERVICE_COMMAND("PickGlassOperation", HMI_PickGlassOperation);

		m_comServer.IPC_REG_SERVICE_COMMAND("DataLog", HMI_DataLog);
		m_comServer.IPC_REG_SERVICE_COMMAND("MoveAllWHtoBondPosn", HMI_MoveAllWHtoBondPosn); //20130318
		m_comServer.IPC_REG_SERVICE_COMMAND("HMI_KeepQuiet", HMI_KeepQuiet);
		
		m_comServer.IPC_REG_SERVICE_COMMAND("FlushMode", HMI_FlushMode); //20130403
		m_comServer.IPC_REG_SERVICE_COMMAND("HMI_CheckGatewayConnection", HMI_CheckGatewayConnection); //20130619

		m_comServer.IPC_REG_SERVICE_COMMAND("SettingReportManagerSaveShiftSchedule", HMI_SettingReportManagerSaveShiftSchedule);		//Philip

		m_comServer.IPC_REG_SERVICE_COMMAND("CheckStartBondingAllowed", HMI_CheckStartBondingAllowed);		//20150610

		m_comServer.IPC_REG_SERVICE_COMMAND("HMI_SetPRDebugInfo", HMI_SetPRDebugInfo);		//20150625
		m_comServer.IPC_REG_SERVICE_COMMAND("HMI_ToggleEnableAnvilVac", HMI_ToggleEnableAnvilVac);		//20170816
		m_comServer.IPC_REG_SERVICE_COMMAND("HMI_ToggleUplookEnable", HMI_ToggleUplookEnable);		//20170816
		
#ifdef EXCEL_MACHINE_DEVICE_INFO
		m_comServer.IPC_REG_SERVICE_COMMAND("HMI_PrintMachineDeviceInfoExcel", HMI_PrintMachineDeviceInfoExcel);
#endif
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}


}


////////////////////////////////////////////////////////////////////////////////////
//String Map File Save As
////////////////////////////////////////////////////////////////////////////////////

VOID CAC9000App::PrintMachineInfo()
{
	FILE *fp = fopen("D:\\sw\\AC9000\\Backup\\readme.txt", "wt");
	if (fp != NULL)
	{
		fprintf(fp, "Machine ID\t\t: %s\n", (const char*)gszMACHINE_ID_NO);
		fprintf(fp, "OS Image No\t: %s\n", (const char*)gszOS_IMAGE_NO);
		fprintf(fp, "Vision OS Image No\t: %s\n", (const char*)gszMACHINE_ID_NO);
		fprintf(fp, "\n");

		fprintf(fp, "SWR Number\t\t: %s\n", (const char*)gszSOFTWARE_NUMBER);
		fprintf(fp, "SW Version\t\t: %s\n", (const char*)gszSOFTWARE_VERSION);
		fprintf(fp, "SW Release Date\t\t: %s\n", (const char*)gszRELEASE_DATE);

		fclose(fp);
	}
}

/////////////////////////////////////////////////////////////////
//Startup Sequence
/////////////////////////////////////////////////////////////////
INT CAC9000App::MachineVersion()
{
	if (m_fSelectVer1)
	{
		return VERSION_1;
	}
	else if (m_fSelectVer2)
	{
		return VERSION_2;
	}
	else if (m_fSelectVer3)
	{
		return VERSION_3;
	}
	else
	{
		DisplayMessage("SEVEN OUT! Unknown Machine Version Detected");
		return MAX_NUM_OF_MACHINE_VERSION;
	}
}

VOID CAC9000App::TC_AddJob(INT emTCObjectID)
{
	if (!pCTimeChart->m_bModSelected || (State() != AUTO_Q && State() != DEMO_Q))
	{
		return;
	}

	INT nConvID = 0;
	IPC_CServiceMessage stMsg;
	LONG lTimeCount = 0;

	stMsg.InitMessage(sizeof(INT), &emTCObjectID);
	nConvID = m_comClient.SendRequest("TimeChart", "TC_AddCmd", stMsg, 0);
}

VOID CAC9000App::TC_DelJob(INT emTCObjectID)
{
	if (!pCTimeChart->m_bModSelected || (State() != AUTO_Q && State() != DEMO_Q))
	{
		return;
	}

	INT nConvID = 0;
	IPC_CServiceMessage stMsg;
	LONG lTimeCount = 0;

	stMsg.InitMessage(sizeof(INT), &emTCObjectID);
	nConvID = m_comClient.SendRequest("TimeChart", "TC_DelCmd", stMsg, 0);
}

BOOL CAC9000App::IsInitNuDrive()
{
	return (m_fEnableHardware);
}

BOOL CAC9000App::IsTempCtrlInited()
{
	return (m_bAllLitecInited);

}

BOOL CAC9000App::IsAnyModuleBusy()
{
	for (int i = 0; i < GetStationList().GetSize(); i++)
	{
		if (GetStationList().GetAt(i)->IsBusy())
		{
			return TRUE;
		}
	}
	return FALSE;

}

BOOL CAC9000App::IsAllServoCommutated()
{
	return (m_bTA1Comm && m_bTA2Comm &&
			m_bACF1Comm && m_bACF1WHComm && m_bACF2WHComm && m_bInPickArmComm);
}

BOOL CAC9000App::IsAllMotorHomed()
{
	return (m_bTA1Home && m_bTA2Home &&
			m_bACF1Home && m_bACF1WHHome && m_bACF2WHHome && m_bInPickArmHome);
}

BOOL CAC9000App::IsSelectLitec()
{
	return m_fSelectLitec;
}

BOOL CAC9000App::IsSelectADAM()
{
	return m_fSelectADAM;
}

BOOL CAC9000App::IsSelectHeaterOn()	// 20140905 Yip
{
	return m_fSelectHeaterOn;
}

BOOL CAC9000App::IsSelectCOG902()
{
	return m_fSelectCOG902;
}

BOOL CAC9000App::IsAdamExit()
{
	return (m_bAdamExit);
}

// 20140922 Yip: Add Datalog Mode And Stop Condition
INT CAC9000App::StartDataLog(CStringList *pszListPort, GMP_DATALOG_MODE enDatalogMode, GMP_LOGIC enStopLogic, GMP_U32 u32StopPattern)
{
	CStringList szlistPortFinal;

	if (pszListPort)
	{
		szlistPortFinal.AddTail(pszListPort);
	}
	else
	{

		CStdioFile fin;

		if (fin.Open("D:\\sw\\AC9000\\Data\\datalogport.txt", CFile::modeRead))
		{
			CString sMsg;
			while (fin.ReadString(sMsg))
			{
				if (sMsg.GetLength() > 0)
				{
					szlistPortFinal.AddTail(sMsg);
				}
			}
			fin.Close();
		}
		else
		{
			return GMP_FAIL;
		}
	}

	try
	{
		// 20140922 Yip: Add Datalog Mode And Stop Condition
		m_pCGmpSystem->EnableDatalog(&szlistPortFinal, enDatalogMode, enStopLogic, u32StopPattern);
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		SetError(IDS_DATALOG_FAIL);
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return GMP_FAIL;
	}
	
	return GMP_SUCCESS;
}

INT CAC9000App::StopDataLog(CString szFileName)
{
	try
	{
		m_pCGmpSystem->DisableDatalog(szFileName);
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		SetError(IDS_DATALOG_FAIL);
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return GMP_FAIL;
	}

	return GMP_SUCCESS;
}

/////////////////////////////////////////////////////////////////
//Message Windows
/////////////////////////////////////////////////////////////////
VOID CAC9000App::WriteHMIMess(const CString &szMess, BOOL bLog, BOOL bLogTime)
{
	CString	szMessage	= _T("");
	CString szTime = _T("");
	__time64_t ltime = NULL;

	_time64(&ltime);
	if (ltime != NULL)
	{
		szTime = _ctime64(&ltime);
	}

	if (szTime.GetLength() > 0)
	{
		szTime = szTime.Trim('\n');
	}

	if (bLogTime)
	{
		szMessage = CString(_T("[")) + szTime + CString(_T("]   ")) + szMess;
	}
	else
	{
		szMessage = szMess;
	}

	m_csMsgLock.Lock();
	m_astrMsgQueue.Add(szMess);
	m_csMsgLock.Unlock();

	if (bLog)
	{
		DisplayMessage(szMess);
	}
}

/////////////////////////////////////////////////////////////////
//HMI Windows
/////////////////////////////////////////////////////////////////

LONG CAC9000App::HMIMessageBox(const INT nBoxType, const CString &szTitle, const CString &szText)
{
	IPC_CServiceMessage svMsg;
	INT nConvID = 0;
	LONG lReply = 0;
	LONG lTimeCount = 0;

	LONG	lType	= nBoxType;
	LONG	lAlign	= 3;			// align centre
	LONG	lWidth	= 400;
	LONG	lHeight	= 280;

	char *pBuffer = new char[szText.GetLength() + 1 + szTitle.GetLength() + 1 + 4 * sizeof(LONG)];
	
	INT nIndex = 0;
	memcpy(&pBuffer[nIndex], (LPCTSTR)szText, szText.GetLength() + 1);
	nIndex += szText.GetLength() + 1;
	memcpy(&pBuffer[nIndex], (LPCTSTR)szTitle, szTitle.GetLength() + 1);
	nIndex += szTitle.GetLength() + 1;
	memcpy(&pBuffer[nIndex], &lType, sizeof(LONG));
	nIndex += sizeof(LONG);
	memcpy(&pBuffer[nIndex], &lAlign, sizeof(LONG));
	nIndex += sizeof(LONG);
	memcpy(&pBuffer[nIndex], &lWidth, sizeof(LONG));
	nIndex += sizeof(LONG);
	memcpy(&pBuffer[nIndex], &lHeight, sizeof(LONG));
	nIndex += sizeof(LONG);

	svMsg.InitMessage(nIndex, pBuffer);
	delete[] pBuffer;

	nConvID = m_comClient.SendRequest("HmiUserService", "HmiMessageBox", svMsg);
	while (1)
	{
		if (m_comClient.ReadReplyForConvID(nConvID, svMsg, NULL, NULL, 0))
		{
			if (svMsg.GetMsgLen() == sizeof(LONG))
			{
				svMsg.GetMsg(sizeof(LONG), &lReply);
			}
			break;
		}
		else if (lTimeCount++ > 30000)
		{
			lReply		= glHMI_TIMEOUT;
			pCHouseKeeping->SetError(IDS_HK_OPERATION_TIMEOUT);
			break;
		}
		else
		{
			while (m_comServer.ProcessRequest())
			{
				;
			}
			Sleep(10);
		}
	}

	return lReply;
}

LONG CAC9000App::HMISelectionBox(const CString &szTitle, const CString &szText, const CString &szItemText1, const CString &szItemText2, const CString &szItemText3, const CString &szItemText4, 
								 const CString &szItemText5, const CString &szItemText6, const CString &szItemText7, const CString &szItemText8, const CString &szItemText9, const CString &szItemText10)
{
	IPC_CServiceMessage svMsg;
	INT nConvID = 0;
	LONG lReply = 0;
	LONG lTimeCount = 0;
	INT i = 0;
	INT j = 0;


	LONG	lSelection = 0;					//A long variable indicating the default selection to be displayed.
	LONG	lNumSelection = 0;				//A long variable indicating the total number of selections (maximum of 7 items).
	
	CString szItemText[11];

	szItemText[0] = szItemText1;
	szItemText[1] = szItemText2;
	szItemText[2] = szItemText3;
	szItemText[3] = szItemText4;
	szItemText[4] = szItemText5;
	szItemText[5] = szItemText6;
	szItemText[6] = szItemText7;
	szItemText[7] = szItemText8;
	szItemText[8] = szItemText9;
	szItemText[9] = szItemText10;
	szItemText[10] = "";

	while (szItemText[j] != "")
	{
		lNumSelection++;
		j++;
	}

	char *pBuffer = new char[szText.GetLength() + 1 + szTitle.GetLength() + 1 + 2 * sizeof(LONG) + szItemText[0].GetLength() + 1 + szItemText[1].GetLength() + 1 
									+ szItemText[2].GetLength() + 1 + szItemText[3].GetLength() + 1 + szItemText[4].GetLength() + 1 + szItemText[5].GetLength() + 1 + szItemText[6].GetLength() + 1 
									+ szItemText[7].GetLength() + 1 + szItemText[8].GetLength() + 1 + szItemText[9].GetLength() + 1];

	INT nIndex = 0;

	memcpy(&pBuffer[nIndex], (LPCTSTR)szText, szText.GetLength() + 1);
	nIndex += szText.GetLength() + 1;
	memcpy(&pBuffer[nIndex], (LPCTSTR)szTitle, szTitle.GetLength() + 1);
	nIndex += szTitle.GetLength() + 1;
	memcpy(&pBuffer[nIndex], &lSelection, sizeof(LONG));
	nIndex += sizeof(LONG);
	memcpy(&pBuffer[nIndex], &lNumSelection, sizeof(LONG));
	nIndex += sizeof(LONG);

	for (i = 0; i < j; i++)
	{
		memcpy(&pBuffer[nIndex], (LPCTSTR)szItemText[i], szItemText[i].GetLength() + 1);
		nIndex += szItemText[i].GetLength() + 1;
	}

	svMsg.InitMessage(nIndex, pBuffer);
	delete[] pBuffer;

	nConvID = m_comClient.SendRequest("HmiUserService", "HmiSelectionBox", svMsg);
	while (1)
	{
		if (m_comClient.ReadReplyForConvID(nConvID, svMsg, NULL, NULL, 0))
		{
			if (svMsg.GetMsgLen() == sizeof(LONG))
			{
				svMsg.GetMsg(sizeof(LONG), &lReply);
			}
			break;
		}
		else if (lTimeCount++ > 30000)
		{
			lReply		= glHMI_TIMEOUT;
			pCHouseKeeping->SetError(IDS_HK_OPERATION_TIMEOUT);
			break;
		}
		else
		{
			while (m_comServer.ProcessRequest())
			{
				;
			}
			Sleep(10);
		}
	}

	return lReply;
}

LONG CAC9000App::HMIConfirmWindow(const CString &szFirstbtn, const CString &szSecondbtn, const CString &szThirdbtn, const CString &szTitle, const CString &szText)
{
	IPC_CServiceMessage svMsg;
	INT nConvID = 0;
	LONG lReply = 0;
	LONG lTimeCount = 0;

	LONG	lType	= 209;			// 3 buttons
	LONG	lAlign	= 3;			// align centre
	LONG	lWidth	= 400;
	LONG	lHeight	= 280;
	LONG	lTimeout = 0;
	CString szBitmapFile = NULL;

	CString szCustomText1 = szFirstbtn;
	CString szCustomText2 = szSecondbtn;
	CString szCustomText3 = szThirdbtn;

	/*CString szCustomText1 = "Yes";
	CString szCustomText2 = "No";
	CString szCustomText3 = "Cancel";*/

	Response MessageReply = BUTTON_1;
	LONG lResponse	= 0;

	char *pBuffer = new char[szText.GetLength() + 1 + szTitle.GetLength() + 1 + 5 * sizeof(LONG) + szBitmapFile.GetLength() + 1 + szCustomText1.GetLength() + 1 + szCustomText2.GetLength() + 1 + szCustomText3.GetLength() + 1];
	
	INT nIndex = 0;
	memcpy(&pBuffer[nIndex], (LPCTSTR)szText, szText.GetLength() + 1);
	nIndex += szText.GetLength() + 1;
	memcpy(&pBuffer[nIndex], (LPCTSTR)szTitle, szTitle.GetLength() + 1);
	nIndex += szTitle.GetLength() + 1;
	memcpy(&pBuffer[nIndex], &lType, sizeof(LONG));
	nIndex += sizeof(LONG);
	memcpy(&pBuffer[nIndex], &lAlign, sizeof(LONG));
	nIndex += sizeof(LONG);
	memcpy(&pBuffer[nIndex], &lWidth, sizeof(LONG));
	nIndex += sizeof(LONG);
	memcpy(&pBuffer[nIndex], &lHeight, sizeof(LONG));
	nIndex += sizeof(LONG);
	memcpy(&pBuffer[nIndex], &lTimeout, sizeof(LONG));
	nIndex += sizeof(LONG);
	memcpy(&pBuffer[nIndex], (LPCTSTR)szBitmapFile, szBitmapFile.GetLength() + 1);
	nIndex += szBitmapFile.GetLength() + 1;

	memcpy(&pBuffer[nIndex], (LPCTSTR)szCustomText1, szCustomText1.GetLength() + 1);
	nIndex += szCustomText1.GetLength() + 1;
	memcpy(&pBuffer[nIndex], (LPCTSTR)szCustomText2, szCustomText2.GetLength() + 1);
	nIndex += szCustomText2.GetLength() + 1;
	memcpy(&pBuffer[nIndex], (LPCTSTR)szCustomText3, szCustomText3.GetLength() + 1);
	nIndex += szCustomText3.GetLength() + 1;

	svMsg.InitMessage(nIndex, pBuffer);
	delete[] pBuffer;

	nConvID = m_comClient.SendRequest("HmiUserService", "HmiMessageBox", svMsg);
	while (1)
	{
		if (m_comClient.ReadReplyForConvID(nConvID, svMsg, NULL, NULL, 0))
		{
			if (svMsg.GetMsgLen() == sizeof(LONG))
			{
				svMsg.GetMsg(sizeof(LONG), &lResponse);
			}
			break;
		}
		else if (lTimeCount++ > 30000)
		{
			MessageReply		= BUTTON_3; // Alaways at the Cancel
			pCHouseKeeping->SetError(IDS_HK_OPERATION_TIMEOUT);
			break;
		}
		else
		{
			while (m_comServer.ProcessRequest())
			{
				;
			}
			Sleep(10);
		}
	}
	if (lResponse == 1)
	{
		MessageReply = BUTTON_1;
	}
	else if (lResponse == 5)
	{
		MessageReply = BUTTON_2;
	}
	else if (lResponse == 8)
	{
		MessageReply = BUTTON_3;
	}
	return MessageReply;
}

BOOL CAC9000App::HMIGetPassword(const CString &szTitle, const CString &szPassword)
{
	IPC_CServiceMessage svMsg;
	INT nConvID = 0;
	LONG lReply = 0;
	LONG lTimeCount = 0;

	PCHAR	pBuffer		= NULL;
	INT		nLength		= 0;
	INT		nMsgIndex	= 0;
	INT		nTCharSize	= sizeof(TCHAR);
	BOOL	bReply		= FALSE;

	nLength = (szTitle.GetLength() + 1) * nTCharSize;
	nLength += (szPassword.GetLength() + 1) * nTCharSize;
	pBuffer = new CHAR[nLength];

	_tcsncpy(pBuffer, szTitle, szTitle.GetLength() + 1);
	nMsgIndex = szTitle.GetLength() + 1;
	_tcsncpy(pBuffer + nMsgIndex, szPassword, szPassword.GetLength() + 1);

	try
	{
		svMsg.InitMessage(nLength, pBuffer);
		delete[] pBuffer;
		pBuffer = NULL;

		nConvID = m_comClient.SendRequest("HmiUserService", "HmiPassword", svMsg);
		while (1)
		{
			MSG msgCur;
			while (::PeekMessage(&msgCur, NULL, NULL, NULL, PM_NOREMOVE))
			{
				CWinApp *pApp = AfxGetApp();
				// pump message, but quit on WM_QUIT
				if (!pApp->PumpMessage())
				{
					return bReply;
				}
			}

			if (m_comClient.ReadReplyForConvID(nConvID, svMsg, NULL, NULL, 0))
			{
				if (svMsg.GetMsgLen() == sizeof(BOOL))
				{
					svMsg.GetMsg(sizeof(BOOL), &bReply);
				}
				break;
			}
			else if (lTimeCount++ > 30000)
			{
				lReply		= glHMI_TIMEOUT;
				pCHouseKeeping->SetError(IDS_HK_OPERATION_TIMEOUT);
				break;
			}
			else
			{
				while (m_comServer.ProcessRequest())
				{
					;
				}
				Sleep(10);
			}
		}
	}
	catch (CAsmException e)
	{
		if (pBuffer != NULL)
		{
			delete[] pBuffer;
			pBuffer = NULL;
		}
	}

	return bReply;
}

BOOL CAC9000App::HMIAlphaKeys(const CString &szPrompt, CString *pszKeyIn, BOOL bKeyInPassword)
{
	IPC_CServiceMessage svMsg;
	INT nConvID = 0;
	LONG lReply = 0;
	LONG lTimeCount = 0;
	BOOL bTRUE = TRUE;
	BOOL bFALSE = FALSE;

	stDummy = "";
	CString szName = "stDummy";

	CHAR *pBuffer = new CHAR[szName.GetLength() + 1 + szPrompt.GetLength() + 1 + sizeof(BOOL) + sizeof(BOOL)];
	BOOL bReply = FALSE;

	INT nIndex = 0;
	memcpy(&pBuffer[nIndex], (LPCTSTR)szName, szName.GetLength() + 1);
	nIndex += szName.GetLength() + 1;
	memcpy(&pBuffer[nIndex], (LPCTSTR)szPrompt, szPrompt.GetLength() + 1);
	nIndex += szPrompt.GetLength() + 1;
	memcpy(&pBuffer[nIndex], &bKeyInPassword, sizeof(BOOL));
	nIndex += sizeof(BOOL);
	memcpy(&pBuffer[nIndex], &bTRUE, sizeof(BOOL));
	nIndex += sizeof(BOOL);

	svMsg.InitMessage(nIndex, pBuffer);
	delete[] pBuffer;

	nConvID = m_comClient.SendRequest("HmiUserService", "HmiAlphaKeys", svMsg);
	while (1)
	{
		MSG msgCur;
		while (::PeekMessage(&msgCur, NULL, NULL, NULL, PM_NOREMOVE))
		{
			CWinApp *pApp = AfxGetApp();
			// pump message, but quit on WM_QUIT
			if (!pApp->PumpMessage())
			{
				return bReply;
			}
		}

		if (m_comClient.ReadReplyForConvID(nConvID, svMsg, NULL, NULL, 0))
		{
			if (svMsg.GetMsgLen() == sizeof(LONG))
			{
				svMsg.GetMsg(sizeof(LONG), &lReply);
			}
			else
			{
#if 1 //20150615
				LONG	lMsgLen = svMsg.GetMsgLen();
				CHAR cReply[_MAX_PATH];
				CHAR	*pReply = &cReply[0];

				memset(&cReply[0], 0, _MAX_PATH);

				if (lMsgLen + 1 > _MAX_PATH)
				{
					CString szMsgTmp("");
					HMIMessageBox(MSG_OK, "WARNING", "Software Handling Error. Operation Abort!");
					szMsgTmp.Format("%s: Message Length Error", __FUNCTION__);
					DisplayMessage(szMsgTmp);
					return bReply;
				}
				else if (lMsgLen > 0)
				{
					svMsg.GetMsg(lMsgLen, pReply);
					pszKeyIn->Format("%s", &pReply[sizeof(BOOL)]);
				}
#else
				CHAR *pReply = new CHAR[svMsg.GetMsgLen()];
				svMsg.GetMsg(svMsg.GetMsgLen(), pReply);

				CString stReply = &pReply[sizeof(BOOL)];
				*pszKeyIn = stReply;
#endif				
				bReply = TRUE;
			}
			break;
		}
		else if (lTimeCount++ > 30000)
		{
			lReply		= glHMI_TIMEOUT;
			pCHouseKeeping->SetError(IDS_HK_OPERATION_TIMEOUT);
			break;
		}
		else
		{
			while (m_comServer.ProcessRequest())
			{
				;
			}
			Sleep(10);
		}
	}

	return bReply;
}

BOOL CAC9000App::HMINumericKeys(const CString &szPrompt, const DOUBLE dMaxValue, const DOUBLE dMinValue, DOUBLE *pdValue)
{
	IPC_CServiceMessage svMsg;
	INT nConvID = 0;
	LONG lReply = 0;
	LONG lTimeCount = 0;
	BOOL bTRUE = TRUE;

	dDummy = 0.0;
	CString szVariable = "dDummy";

	CHAR *pBuffer = new CHAR[szVariable.GetLength() + 1 + szPrompt.GetLength() + 1 + 2 * sizeof(DOUBLE) + sizeof(BOOL)];
	BOOL bReply = FALSE;

	INT nIndex = 0;
	memcpy(&pBuffer[nIndex], (LPCTSTR)szVariable, szVariable.GetLength() + 1);
	nIndex += szVariable.GetLength() + 1;
	memcpy(&pBuffer[nIndex], (LPCTSTR)szPrompt, szPrompt.GetLength() + 1);
	nIndex += szPrompt.GetLength() + 1;
	memcpy(&pBuffer[nIndex], &dMaxValue, sizeof(DOUBLE));
	nIndex += sizeof(DOUBLE);
	memcpy(&pBuffer[nIndex], &dMinValue, sizeof(DOUBLE));
	nIndex += sizeof(DOUBLE);
	memcpy(&pBuffer[nIndex], &bTRUE, sizeof(BOOL));
	nIndex += sizeof(BOOL);

	svMsg.InitMessage(nIndex, pBuffer);
	delete[] pBuffer;

	nConvID = m_comClient.SendRequest("HmiUserService", "HmiNumericKeys", svMsg);
	while (1)
	{
		MSG msgCur;
		while (::PeekMessage(&msgCur, NULL, NULL, NULL, PM_NOREMOVE))
		{
			CWinApp *pApp = AfxGetApp();
			// pump message, but quit on WM_QUIT
			if (!pApp->PumpMessage())
			{
				return bReply;
			}
		}

		if (m_comClient.ReadReplyForConvID(nConvID, svMsg, NULL, NULL, 0))
		{
			if (svMsg.GetMsgLen() == sizeof(LONG))
			{
				svMsg.GetMsg(sizeof(LONG), &lReply);
			}
			else
			{
#if 1 //20150615
				LONG	lMsgLen = svMsg.GetMsgLen();
				CHAR cReply[_MAX_PATH];
				CHAR *pReply = &cReply[0];

				memset(&cReply[0], 0, _MAX_PATH);

				if (lMsgLen + 1 > _MAX_PATH)
				{
					CString szMsgTmp("");
					HMIMessageBox(MSG_OK, "WARNING", "Software Handling Error. Operation Abort!");
					szMsgTmp.Format("%s: Message Length Error", __FUNCTION__);
					DisplayMessage(szMsgTmp);
					return bReply;
				}
				else if (lMsgLen > 0)
				{
					svMsg.GetMsg(lMsgLen, pReply);
					CString stReply;
					stReply.Format("%s", &pReply[sizeof(BOOL)]);
					*pdValue = atof(stReply);
				}
#else
				CHAR *pReply = new CHAR[svMsg.GetMsgLen()];
				svMsg.GetMsg(svMsg.GetMsgLen(), pReply);

				CString stReply = &pReply[sizeof(BOOL)];
				*pdValue = atof(stReply);				
#endif
				bReply = TRUE;
			}
			break;
		}
		else if (lTimeCount++ > 30000)
		{
			lReply		= glHMI_TIMEOUT;
			pCHouseKeeping->SetError(IDS_HK_OPERATION_TIMEOUT);
			break;
		}
		else
		{
			while (m_comServer.ProcessRequest())
			{
				;
			}
			Sleep(10);
		}
	}

	return bReply;
}

BOOL CAC9000App::HMINumericKeys(const CString &szPrompt, const DOUBLE dMaxValue, const DOUBLE dMinValue, LONG *plValue)
{
	IPC_CServiceMessage svMsg;
	INT nConvID = 0;
	LONG lReply = 0;
	LONG lTimeCount = 0;
	BOOL bTRUE = TRUE;

	dDummy = 0.0;
	CString szVariable = "dDummy";

	CHAR *pBuffer = new CHAR[szVariable.GetLength() + 1 + szPrompt.GetLength() + 1 + 2 * sizeof(DOUBLE) + sizeof(BOOL)];
	BOOL bReply = FALSE;

	INT nIndex = 0;
	memcpy(&pBuffer[nIndex], (LPCTSTR)szVariable, szVariable.GetLength() + 1);
	nIndex += szVariable.GetLength() + 1;
	memcpy(&pBuffer[nIndex], (LPCTSTR)szPrompt, szPrompt.GetLength() + 1);
	nIndex += szPrompt.GetLength() + 1;
	memcpy(&pBuffer[nIndex], &dMaxValue, sizeof(DOUBLE));
	nIndex += sizeof(DOUBLE);
	memcpy(&pBuffer[nIndex], &dMinValue, sizeof(DOUBLE));
	nIndex += sizeof(DOUBLE);
	memcpy(&pBuffer[nIndex], &bTRUE, sizeof(BOOL));
	nIndex += sizeof(BOOL);

	svMsg.InitMessage(nIndex, pBuffer);
	delete[] pBuffer;

	nConvID = m_comClient.SendRequest("HmiUserService", "HmiNumericKeys", svMsg);
	//while (!m_comClient.ScanReplyForConvID(nConvID, 1) && (lTimeCount++ < 8640000))
	//{
	//	MSG msgCur;
	//	while(::PeekMessage(&msgCur, NULL, NULL, NULL, PM_NOREMOVE))
	//	{
	//		CWinApp* pApp = AfxGetApp();
	//		// pump message, but quit on WM_QUIT
	//		if (!pApp->PumpMessage())
	//		{
	//			return bReply;
	//		}
	//	}
	//	Sleep(10);
	//	ProcessHMIRequest();
	//}

	//if (lTimeCount < 8640000)
	//{
	//	CString szMsg = "";

	//	m_comClient.ReadReplyForConvID(nConvID, svMsg);

	//	if (svMsg.GetMsgLen() == sizeof(BOOL))
	//	{
	//		svMsg.GetMsg(sizeof(BOOL), &bReply);
	//	}
	//	else
	//	{
	//		CHAR* pReply = new CHAR[svMsg.GetMsgLen()];
	//		svMsg.GetMsg(svMsg.GetMsgLen(), pReply);

	//		CString stReply = &pReply[sizeof(BOOL)];
	//		*plValue = atol(stReply);

	//		bReply = TRUE;
	//	}
	//}
	while (1)
	{
		MSG msgCur;
		while (::PeekMessage(&msgCur, NULL, NULL, NULL, PM_NOREMOVE))
		{
			CWinApp *pApp = AfxGetApp();
			// pump message, but quit on WM_QUIT
			if (!pApp->PumpMessage())
			{
				return bReply;
			}
		}

		if (m_comClient.ReadReplyForConvID(nConvID, svMsg, NULL, NULL, 0))
		{
			if (svMsg.GetMsgLen() == sizeof(LONG))
			{
				svMsg.GetMsg(sizeof(LONG), &lReply);
			}
			else
			{
#if 1 //20150615
				LONG	lMsgLen = svMsg.GetMsgLen();
				CHAR cReply[_MAX_PATH];
				CHAR *pReply = &cReply[0];

				memset(&cReply[0], 0, _MAX_PATH);

				if (lMsgLen + 1 > _MAX_PATH)
				{
					CString szMsgTmp("");
					HMIMessageBox(MSG_OK, "WARNING", "Software Handling Error. Operation Abort!");
					szMsgTmp.Format("%s: Message Length Error", __FUNCTION__);
					DisplayMessage(szMsgTmp);
					return bReply;
				}
				else if (lMsgLen > 0)
				{
					svMsg.GetMsg(lMsgLen, pReply);
					CString stReply;
					stReply.Format("%s", &pReply[sizeof(BOOL)]);
					*plValue = atol(stReply);
				}
#else
				CHAR *pReply = new CHAR[svMsg.GetMsgLen()];
				svMsg.GetMsg(svMsg.GetMsgLen(), pReply);

				CString stReply = &pReply[sizeof(BOOL)];
				*plValue = atol(stReply);
#endif
				bReply = TRUE;
			}
			break;
		}
		else if (lTimeCount++ > 30000)
		{
			lReply		= glHMI_TIMEOUT;
			pCHouseKeeping->SetError(IDS_HK_OPERATION_TIMEOUT);
			break;
		}
		else
		{
			while (m_comServer.ProcessRequest())
			{
				;
			}
			Sleep(10);
		}
	}
	return bReply;
}

BOOL CAC9000App::HMILangSwitching()
{
	IPC_CServiceMessage svMsg;

	switch (m_ulLangageCode)
	{
	case 0:		// English
		m_ulLangageCode = 2;	// to Trad. Chinese
		break;

	case 1:		// Simplify Chinese
		m_ulLangageCode = 0;	// to English
		break;

	case 2:		// Trad. Chinese
		m_ulLangageCode = 1;	// to Simplify Chinese
		break;

	default:
		m_ulLangageCode = 2;	// to English
		break;
	}

	svMsg.InitMessage(sizeof(ULONG), &m_ulLangageCode);
	m_comClient.SendRequest("HmiUserService", "HmiLangSwitching", svMsg, 0);

	return TRUE;
}

//p20120918
BOOL CAC9000App::HMILangTraditionalChinese()
{
	try
	{
		IPC_CServiceMessage svMsg;
		if (m_ulLangageCode != 2)
		{
			m_ulLangageCode = 2;					// Change Language to Traditional Chinese
			svMsg.InitMessage(sizeof(ULONG), &m_ulLangageCode);
			m_comClient.SendRequest("HmiUserService", "HmiLangSwitching", svMsg, 0);
		}
	}
	catch (CAsmException e)	// 20140403 Yip
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
	return TRUE;
}
//p20120918:end


VOID CAC9000App::ProcessHMIRequest()
{
	try
	{
		// perform server request
		if (m_comServer.ScanRequest())
		{
			//if (g_bEnableLog) 
			//{
			//	m_comServer.ProcessRequest(-2);
			//}
			//else 
			//{
			m_comServer.ProcessRequest();
			//}
		}
	}
	catch (CAsmException e)
	{
		CString szMessage;
		e.What(szMessage);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		TRACE1("SFM_CModule::Run - %s\n", szMessage);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

}
////////////////////////////////////////////////////////////////////////////////////
//Close Operation
////////////////////////////////////////////////////////////////////////////////////
BOOL CAC9000App::CheckAutoBondSelected(AutoBondRequirment lMode)
{
	BOOL bError = FALSE;
	INT	i = 0;
	INT j = 0;
	CString szMsg = "";
	LONG	lAnswer = rMSG_TIMEOUT;

	BOOL bPBContinue = FALSE;	// 20140617 Yip
	BOOL bMBContinue = FALSE;	// 20140617 Yip

	switch (lMode)
	{
	//case REWORK_MB1:
	//	for (i = 0; i < GetStationList().GetSize(); i++)
	//	{
	//		if (
	//			GetStationList().GetAt(i)->GetModSelectMode() == GLASS_MOD ||
	//			GetStationList().GetAt(i)->GetModSelectMode() == MB1_MOD
	//		   )
	//		{
	//			if (!GetStationList().GetAt(i)->IsModSelected())
	//			{
	//				CString str;
	//				str.Format("%s", GetStationList().GetAt(i)->GetStnName());
	//				bError = TRUE;
	//			}
	//		}
	//		else
	//		{	
	//			if (GetStationList().GetAt(i)->IsModSelected())
	//			{
	//				CString str;
	//				str.Format("%s", GetStationList().GetAt(i)->GetStnName());
	//				bError = TRUE;
	//			}
	//		}
	//	}
	//	
	//	// PR Module
	//	for (i = 0; i < GetPRStationList().GetSize(); i++)
	//	{
	//		if (!IsSelectTA4Camera() && GetPRStationList().GetAt(i)->m_szPRTaskStnName == "TA4") //20140527
	//		{
	//			continue;
	//		}
	//		if (
	//			GetPRStationList().GetAt(i)->GetPRModSelectMode() == GLASS_MOD ||
	//			GetPRStationList().GetAt(i)->GetPRModSelectMode() == MB1_MOD
	//		   )
	//		{
	//			if (!GetPRStationList().GetAt(i)->IsPRSelected())
	//			{
	//				bError = TRUE;
	//			}
	//		}
	//		else
	//		{	
	//			if (GetPRStationList().GetAt(i)->IsPRSelected())
	//			{
	//				CString str;
	//				str.Format("%s", GetStationList().GetAt(i)->GetStnName());
	//				bError = TRUE;
	//			}
	//		}
	//	}
	//	break;

	//case REWORK_MB2:
	//	for (i = 0; i < GetStationList().GetSize(); i++)
	//	{
	//		if (
	//			GetStationList().GetAt(i)->GetModSelectMode() == GLASS_MOD ||
	//			GetStationList().GetAt(i)->GetModSelectMode() == MB2_MOD
	//		   )
	//		{
	//			if (!GetStationList().GetAt(i)->IsModSelected())
	//			{
	//				bError = TRUE;
	//			}
	//		}
	//		else
	//		{	
	//			if (GetStationList().GetAt(i)->IsModSelected())
	//			{
	//				CString str;
	//				str.Format("%s", GetStationList().GetAt(i)->GetStnName());
	//				bError = TRUE;
	//			}
	//		}
	//	}

	//	// PR Module
	//	for (i = 0; i < GetPRStationList().GetSize(); i++)
	//	{
	//		if (!IsSelectTA4Camera() && GetPRStationList().GetAt(i)->m_szPRTaskStnName == "TA4") //20140527
	//		{
	//			continue;
	//		}
	//		if (
	//			GetPRStationList().GetAt(i)->GetPRModSelectMode() == GLASS_MOD ||
	//			GetPRStationList().GetAt(i)->GetPRModSelectMode() == MB2_MOD
	//		   )
	//		{
	//			if (!GetPRStationList().GetAt(i)->IsPRSelected())
	//			{
	//				bError = TRUE;
	//			}
	//		}
	//		else
	//		{	
	//			if (GetPRStationList().GetAt(i)->IsPRSelected())
	//			{
	//				CString str;
	//				str.Format("%s", GetStationList().GetAt(i)->GetStnName());
	//				bError = TRUE;
	//			}
	//		}
	//	}
	//	break;

	//case REWORK_PB:
	//	for (i = 0; i < GetStationList().GetSize(); i++)
	//	{
	//		if (
	//			GetStationList().GetAt(i)->GetModSelectMode() == GLASS_MOD ||
	//			GetStationList().GetAt(i)->GetModSelectMode() == MB1_MOD ||
	//			GetStationList().GetAt(i)->GetModSelectMode() == MB2_MOD ||
	//			GetStationList().GetAt(i)->GetModSelectMode() == LSI_MOD
	//		   )
	//		{
	//			if (!GetStationList().GetAt(i)->IsModSelected())
	//			{
	//				CString str;
	//				str.Format("%s", GetStationList().GetAt(i)->GetStnName());
	//				bError = TRUE;
	//			}
	//		}
	//		else
	//		{	
	//			if (GetStationList().GetAt(i)->IsModSelected())
	//			{
	//				CString str;
	//				str.Format("%s", GetStationList().GetAt(i)->GetStnName());
	//				bError = TRUE;
	//			}
	//		}
	//	}

	//	// PR Module
	//	for (i = 0; i < GetPRStationList().GetSize(); i++)
	//	{
	//		if (!IsSelectTA4Camera() && GetPRStationList().GetAt(i)->m_szPRTaskStnName == "TA4") //20140527
	//		{
	//			continue;
	//		}
	//		if (
	//			GetPRStationList().GetAt(i)->GetPRModSelectMode() == GLASS_MOD ||
	//			GetPRStationList().GetAt(i)->GetPRModSelectMode() == MB1_MOD ||
	//			GetPRStationList().GetAt(i)->GetPRModSelectMode() == MB2_MOD ||
	//			GetPRStationList().GetAt(i)->GetPRModSelectMode() == LSI_MOD
	//		   )
	//		{
	//			if (!GetPRStationList().GetAt(i)->IsPRSelected())
	//			{
	//				bError = TRUE;
	//			}
	//		}
	//		else
	//		{	
	//			if (GetPRStationList().GetAt(i)->IsPRSelected())
	//			{
	//				CString str;
	//				str.Format("%s", GetStationList().GetAt(i)->GetStnName());
	//				bError = TRUE;
	//			}
	//		}
	//	}
	//	break;

	case NORMAL_BONDING:
		for (i = 0; i < GetStationList().GetSize(); i++)
		{
			if (!GetStationList().GetAt(i)->IsModSelected())
			{
				CString szStationName = GetStationList().GetAt(i)->GetStnName();


				if (szStationName == "TimeChart" || //20120103 ignore these stations
					szStationName == "ACF1WH" ||
					szStationName == "ACF2WH"
				   )
				{
					LONG lAnswer = rMSG_CANCEL;
					BOOL bContinue = TRUE;
					if (!bPBContinue)	// 20140617 Yip
					{
						if (szStationName == "ACF1WH" ||
							szStationName == "ACF2WH")
						{
							lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "ACFWH Module Not Selected. Continue?");
							if (lAnswer == rMSG_CANCEL || lAnswer == rMSG_TIMEOUT)
							{
								bContinue = FALSE;
							}
							else
							{
								bPBContinue = TRUE;
							}
						}
					}
					if (bContinue)
					{
						continue;
					}
				}
				bError = TRUE;
			}
		}

		// PR Module
		for (i = 0; i < GetPRStationList().GetSize(); i++)
		{
			if (!GetPRStationList().GetAt(i)->IsPRSelected())
			{
				CString szPRTaskName;
				szPRTaskName = GetPRStationList().GetAt(i)->m_szPRTaskStnName;
				if (
					GetPRStationList().GetAt(i)->m_szPRTaskStnName == "Insp1" || 
					GetPRStationList().GetAt(i)->m_szPRTaskStnName == "Insp2"
					) //20140527
				{
					continue;
				}
				bError = TRUE;
			}
		}
		break;
	}

	if (!bError)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CAC9000App::CheckHeaterOnOff() //20130424
{
	BOOL bResult = TRUE;
	INT	i = 0;
	CString szMsg = "";

	switch (AutoMode)
	{
	case AUTO_BOND:
		//for(i = 0; i<GetStationList().GetSize(); i++)
		//{
		//	if (
		//		GetStationList().GetAt(i)->GetStnName() == "ACF1" ||
		//		GetStationList().GetAt(i)->GetStnName() == "ACF2" ||
		//		GetStationList().GetAt(i)->GetStnName() == "PB1" ||
		//		GetStationList().GetAt(i)->GetStnName() == "PB2"
		//		)
		//	{ //Heater on these station must turn on in AutoBond
		//		CString szStationName;
		//		szStationName = GetStationList().GetAt(i)->GetStnName();
		//		if (
		//			!pCTempDisplay->IsAllHeaterOnByStationName(szStationName)
		//			)
		//		{
		//			szMsg.Format("Heater on Station:%s Not On Error", GetStationList().GetAt(i)->GetStnName());
		//			DisplayMessage(szMsg);
		//			bResult = FALSE;
		//		}
		//	}

		//	if (
		//		GetStationList().GetAt(i)->GetStnName() == "MB1" ||
		//		GetStationList().GetAt(i)->GetStnName() == "MB2"
		//		)
		//	{ //Heater on these station must turn on in AutoBond only if it's module is selected.
		//		CString szStationName;
		//		szStationName = GetStationList().GetAt(i)->GetStnName();
		//		if (
		//			GetStationList().GetAt(i)->IsModSelected() &&
		//			!pCTempDisplay->IsAllHeaterOnByStationName(szStationName)
		//			)
		//		{
		//			szMsg.Format("Heater on Station:%s Not On Error", GetStationList().GetAt(i)->GetStnName());
		//			DisplayMessage(szMsg);
		//			bResult = FALSE;
		//		}
		//	}
		//}

		if (!pCTempDisplay->CheckHeaterPowerForAutobond())  //20130430
		{
			szMsg.Format("Heater Not On Error");
			DisplayMessage(szMsg);
			bResult = FALSE;
		}

		//if ( !pCTempDisplay->CheckAllHeaterOn()) //20130227
		//{
		//	szMsg.Format("Heater Not On Error");
		//	DisplayMessage(szMsg);
		//	bResult = FALSE;
		//}
		break;

	case TEST_BOND:
		for (i = 0; i < GetStationList().GetSize(); i++)
		{
			if (
				GetStationList().GetAt(i)->GetStnName() == "ACF1" ||
				GetStationList().GetAt(i)->GetStnName() == "ACF2"
			   )
			{
				CString szStationName;
				szStationName = GetStationList().GetAt(i)->GetStnName();
				if (
					//GetStationList().GetAt(i)->GetStnName() == "ACF1" &&
					GetStationList().GetAt(i)->IsModSelected() &&
					!pCTempDisplay->IsAllHeaterOnByStationName(szStationName)
				   )
				{
					szMsg.Format("Heater on Station:%s Not On Error", GetStationList().GetAt(i)->GetStnName());
					DisplayMessage(szMsg);
					bResult = FALSE;
				}
			}
		}
		break;
	default:
		bResult = TRUE;
		break;
	}
	return bResult;
}

BOOL CAC9000App::CheckPackageStatus(AutoBondRequirment lMode)
{
	BOOL bCheckType1	= FALSE;
	BOOL bCheckType2	= FALSE;
	BOOL bError = FALSE;
	INT	i = 0;
	INT j = 0;
	CString szMsg = "";
	LONG	lAnswer = rMSG_TIMEOUT;

	switch (g_lLSITypeInUse)
	{
	case USE_NONE:
		DisplayMessage("HELL(AC9000):	SrvAutoCommand Invalid g_lLSITypeInUse");
		bError = TRUE;
		break;

	case USE_TYPE1:
		bCheckType1 = TRUE;
		break;

	case USE_TYPE2:
		bCheckType2 = TRUE;
		break;

	case USE_BOTH:
		bCheckType1 = TRUE;
		bCheckType2 = TRUE;
		break;
	}
	
	// Check Setup Status
	//for (i = GLASS1; i < MAX_NUM_OF_GLASS; i++)
	//{
	//	if (pCINWH->m_stINWH[i].bEnable)
	//	{
	//		if (!pCINWH->m_stINWH[i].bAlignPtSet)
	//		{
	//			SetError(IDS_INWH_ALIGN_NOT_LOADED);
	//			bError = TRUE;
	//		}
	//		if (!pruIN1.bLoaded || !pruIN2.bLoaded)
	//		{
	//			SetError(IDS_INWH_PR_NOT_LOADED);
	//			bError = TRUE;
	//		}
	//	}
	//}

//	if (lMode == REWORK_PB || lMode == NORMAL_BONDING) 
//	{
//		for (i = GLASS1; i < MAX_NUM_OF_GLASS; i++)
//		{
//			for (j = LSI1; j < MAX_NUM_OF_LSI; j++)
//			{
//				if (g_stLSI[j].bEnable)
//				{
//#ifdef PBWH_UPLOOK
//					if (!pruPBGlass1[i][j].bLoaded || !pruPBGlass2[i][j].bLoaded)
//#else
//					if (!pruPBGlassDL1[i][j].bLoaded || !pruPBGlassDL2[i][j].bLoaded)
//#endif
//					{
//						SetError(IDS_PBGLASS1_PR_NOT_LOADED + i * CONVERT_TO_MAPPING);
//						bError = TRUE;
//					}
//					else if (!g_stLSI[i].bAlignPtSet || getKlocworkFalse()) //klocwork fix 20121211
//					{
//						SetError(IDS_PREBOND_WH_ALIGN_NOT_LOADED);
//						bError = TRUE;
//					}
//				}
//			}
//		}
//
//		// PreBond
//		for (i = PREBOND_1; i < MAX_NUM_OF_PREBOND; i++)
//		{
//			if (bCheckType1)
//			{
//				if (!pruPBLSI1[i][LSI_TYPE1].bLoaded || !pruPBLSI2[i][LSI_TYPE1].bLoaded)
//				{
//					SetError(IDS_PB1_PR_NOT_LOADED + i * CONVERT_TO_MAPPING);
//					bError = TRUE;
//				}	
//				else if (!g_stLSIType[LSI_TYPE1].bAlignPtSet || getKlocworkFalse()) //klocwork fix 20121211
//				{
//					SetError(IDS_PB1_ALIGN_NOT_LOADED);
//					bError = TRUE;
//				}
//			}
//
//			if (bCheckType2)
//			{
//				if (!pruPBLSI1[i][LSI_TYPE2].bLoaded || !pruPBLSI2[i][LSI_TYPE2].bLoaded)
//				{
//					SetError(IDS_PB1_PR_NOT_LOADED + i * CONVERT_TO_MAPPING);
//					bError = TRUE;
//				}	
//				else if (!g_stLSIType[LSI_TYPE2].bAlignPtSet[i])
//				{
//					SetError(IDS_PB1_ALIGN_NOT_LOADED + i * CONVERT_TO_MAPPING);
//					bError = TRUE;
//				}
//			}
//		}
//		
//		// Shuttle
//		for (i = SHUTTLE_1; i < MAX_NUM_OF_SHUTTLE; i++)
//		{
//			if (bCheckType1)
//			{
//				if (!pruPreciser1[i][LSI_TYPE1].bLoaded || !pruPreciser2[i][LSI_TYPE1].bLoaded)
//				{
//					SetError(IDS_SH1_PR_NOT_LOADED + i * CONVERT_TO_MAPPING);
//					bError = TRUE;
//				}	
//				else if (!g_stLSIType[LSI_TYPE1].bAlignLSIPtSet[i])
//				{
//					SetError(IDS_SH1_ALIGN_NOT_LOADED + i * CONVERT_TO_MAPPING);
//					bError = TRUE;
//				}
//			}
//
//			if (bCheckType2)
//			{
//				if (!pruPreciser1[i][LSI_TYPE2].bLoaded || !pruPreciser2[i][LSI_TYPE2].bLoaded)
//				{
//					SetError(IDS_SH1_PR_NOT_LOADED + i * CONVERT_TO_MAPPING);
//					bError = TRUE;
//				}	
//				else if (!g_stLSIType[LSI_TYPE2].bAlignLSIPtSet[i])
//				{
//					SetError(IDS_SH1_ALIGN_NOT_LOADED + i * CONVERT_TO_MAPPING);
//					bError = TRUE;
//				}
//			}
//		}
//#if 0 //20140418
//		// TrayHolder
//		if (bCheckType1)
//		{
//			if (!pruLSI1[LSI_TYPE1].bLoaded || !pruLSI2[LSI_TYPE1].bLoaded)
//			{
//				SetError(IDS_TRAYHOLDER_PR_NOT_LOADED);
//				bError = TRUE;
//			}	
//			else if (!g_stLSIType[LSI_TYPE1].bAlignPickPtSet)
//			{
//				SetError(IDS_TRAYHOLDER_ALIGN_NOT_LOADED);
//				bError = TRUE;
//			}
//		}
//
//		if (bCheckType2)
//		{
//			if (!pruLSI1[LSI_TYPE2].bLoaded || !pruLSI2[LSI_TYPE2].bLoaded)
//			{
//				SetError(IDS_TRAYHOLDER_PR_NOT_LOADED);
//				bError = TRUE;
//			}	
//			else if (!g_stLSIType[LSI_TYPE2].bAlignPickPtSet)
//			{
//				SetError(IDS_TRAYHOLDER_ALIGN_NOT_LOADED);
//				bError = TRUE;
//			}
//		}
//#endif
		//if (pCTrayHolder->m_bModSelected)
		//{
		//	if (!pCTrayHolder->m_bTrayHolderReady[TRAYHOLDER_1])
		//	{
		//		lAnswer = HMIMessageBox(MSG_YES_NO, "AUTOBOND", "Is TrayHolder 1 Ready?");
		//		// check the TrayHolder is alreday or not
		//		if (lAnswer == rMSG_YES) // Chinese
		//		{
		//			pCTrayHolder->m_bTrayHolderReady[TRAYHOLDER_1] = TRUE;
		//			pCHouseKeeping->SetX1PushBtnLampSol(OFF, SFM_NOWAIT);
		//		}
		//		else if (lAnswer == rMSG_NO)
		//		{
		//			pCTrayHolder->m_bTrayHolderReady[TRAYHOLDER_1] = FALSE;
		//			pCHouseKeeping->SetX1PushBtnLampSol(ON, SFM_NOWAIT);
		//		}
		//		else 
		//		{
		//			bError = TRUE;
		//		}
		//	}
		//	if (!pCTrayHolder->m_bTrayHolderReady[TRAYHOLDER_2])
		//	{
		//		lAnswer = HMIMessageBox(MSG_YES_NO, "AUTOBOND", "Is TrayHolder 2 Ready?");
		//		if (lAnswer == rMSG_YES)
		//		{
		//			pCTrayHolder->m_bTrayHolderReady[TRAYHOLDER_2] = TRUE;
		//			pCHouseKeeping->SetX2PushBtnLampSol(OFF, SFM_NOWAIT);
		//		}
		//		else if (lAnswer == rMSG_NO)
		//		{
		//			pCTrayHolder->m_bTrayHolderReady[TRAYHOLDER_2] = FALSE;
		//			pCHouseKeeping->SetX2PushBtnLampSol(ON, SFM_NOWAIT);
		//		}
		//		else 
		//		{
		//			bError = TRUE;
		//		}
		//	}
		//}
	//}

	if (!bError)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/////////////////////////////////////
//BOOL CAC9000App::CheckAutoBondSelected()
//{
//	return TRUE;
//}

//BOOL CAC9000App::CheckReworkPreBondSelected()
//{
//	return TRUE;
//}
//
//BOOL CAC9000App::CheckReworkMainBond1Selected()
//{
//	return TRUE;
//}
//
//BOOL CAC9000App::CheckReworkMainBond2Selected()
//{
//	return TRUE;
//}

BOOL CAC9000App::CheckGlassIndexingPath(CString StationName, GlassNum lGlassNum)
{

	BOOL bStatus = TRUE;
	CString strMsg;

	if (
		//StationName == pCInPickArm->GetStnName()	|| 
		//StationName == pCTA1->GetStnName()	||  // 20141103
		StationName == pCACF1WH->GetStnName() //||
	//	lGlassNum == GLASS1
		//StationName == pCTA2->GetStnName()
	   )
	{
	//ACF1WH
		if (!pCInPickArm->m_bModSelected)
		{
			strMsg.Format("%s Module Not Selected. Operation Abort!", pCInPickArm->GetStnName());
			HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
			bStatus = FALSE;
		}

		if (!pCTA1->m_bModSelected)
		{
			strMsg.Format("%s Module Not Selected. Operation Abort!", pCTA1->GetStnName());
			HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
			bStatus = FALSE;
		}

		if (!pCACF1WH->m_bModSelected)
		{
			strMsg.Format("%s Module Not Selected. Operation Abort!", pCACF1WH->GetStnName());
			HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
			bStatus = FALSE;
		}
		//if (!pCACF2WH->m_bModSelected)
		//{
		//	strMsg.Format("%s Module Not Selected. Operation Abort!", pCACF2WH->GetStnName());
		//	HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
		//	bStatus = FALSE;
		//}

		//if (pCTA1->IsGLASS1_VacSensorOn())
		//{			
		//	HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Glass exists on TA1 already. Please remove glass and retry again.");
		//	bStatus = FALSE;
		//}

		if (
			((pCACF1WH->SetVacSol(GLASS1, ON, GMP_WAIT) != GMP_SUCCESS || pCACF1WH->IsGLASS1_VacSensorOn()))
			//(lGlassNum == GLASS2 && (pCACF2WH->SetVacSol(GLASS2, ON, GMP_WAIT) != GMP_SUCCESS || pCACF2WH->IsGLASS2_VacSensorOn()))
		   )
		{			
			HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Glass exists on ACF1WH already. Please remove glass and retry again.");
			bStatus = FALSE;
		}
		else
		{
			if (lGlassNum == GLASS1)
			{
				pCACF1WH->SetVacSol(GLASS1, OFF, GMP_WAIT);
			}
			
			//if (lGlassNum == GLASS2)
			//{
			//	pCACF2WH->SetVacSol(GLASS2, OFF, GMP_WAIT);
			//}
		}
	}

	else if (
		//StationName == pCInPickArm->GetStnName()	|| 
		//StationName == pCTA1->GetStnName()	||  // 20141103
		StationName == pCACF2WH->GetStnName() //||
	//	lGlassNum == GLASS2
		//StationName == pCTA2->GetStnName()
	   )
	{
	//ACF2WH
		if (!pCInPickArm->m_bModSelected)
		{
			strMsg.Format("%s Module Not Selected. Operation Abort!", pCInPickArm->GetStnName());
			HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
			bStatus = FALSE;
		}

		if (!pCTA1->m_bModSelected)
		{
			strMsg.Format("%s Module Not Selected. Operation Abort!", pCTA1->GetStnName());
			HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
			bStatus = FALSE;
		}

		if (!pCACF2WH->m_bModSelected)
		{
			strMsg.Format("%s Module Not Selected. Operation Abort!", pCACF2WH->GetStnName());
			HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", strMsg);
			bStatus = FALSE;
		}

		//if (pCTA1->IsGLASS1_VacSensorOn())
		//{			
		//	HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Glass exists on TA1 already. Please remove glass and retry again.");
		//	bStatus = FALSE;
		//}

		if (
			//(lGlassNum == GLASS1 && (pCACF1WH->SetVacSol(GLASS1, ON, GMP_WAIT) != GMP_SUCCESS || pCACF1WH->IsGLASS1_VacSensorOn())) || 
			((pCACF2WH->SetVacSol(GLASS1, ON, GMP_WAIT) != GMP_SUCCESS || pCACF2WH->IsGLASS1_VacSensorOn()))
		   )
		{			
			HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Glass exists on ACF2WH already. Please remove glass and retry again.");
			bStatus = FALSE;
		}
		else
		{
			//if (lGlassNum == GLASS1)
			//{
			//	pCACF1WH->SetVacSol(GLASS1, OFF, GMP_WAIT);
			//}
			
			//if (lGlassNum == GLASS2)
			//{
				pCACF2WH->SetVacSol(GLASS1, OFF, GMP_WAIT);
			//}
		}
	}
	return bStatus;
}

BOOL CAC9000App::CheckUnitToBondLmt()
{
	return TRUE;
}


BOOL CAC9000App::CheckToCleanLmt()
{
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
//Close Operation
////////////////////////////////////////////////////////////////////////////////////
LONG CAC9000App::SrvCloseApp(IPC_CServiceMessage &svMsg)
{	
	SMotCommand		smCommand;
	
	POSITION		pos;
	SFM_CStation	*pStation = NULL;
	CAppStation	*pAppStation = NULL;
	CString			szKey = _T("");

	BOOL			bResult;
	//BOOL			bDelayedCloseApp = FALSE; //20121231
	//CAppStation*	pAppStationLitec = NULL;


	DisplayMessage("Closing application...");
	svMsg.GetMsg(sizeof(SMotCommand), &smCommand);

	m_lAction = glAMS_SYSTEM_EXIT;

	// 20140617 Yip: Power Off All Heaters When System Exit In Main App Directly Instead Of Through HMI Command
	pCTempDisplay->PowerOnAllHeater(FALSE);

	// Signal all stations to perform Stop operation
	for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
	{
		m_pStationMap.GetNextAssoc(pos, szKey, pStation);

		if ((pAppStation = dynamic_cast<CAppStation*>(pStation)) != NULL)
		{
#if 1 //20121231
			TRACE0("SrvCloseApp... " + pAppStation->GetName() + "\n");
#endif
			pAppStation->SetAxisAction(0, m_lAction, 0);
			pAppStation->Command(glAMS_STOP_COMMAND);
			pAppStation->Result(GMP_SUCCESS);
		}
	}
	bResult = TRUE;

	m_qState = STOPPING_Q;

	svMsg.InitMessage(sizeof(BOOL), &bResult);

	return 1;
}

LONG CAC9000App::SrvRestartHMI(IPC_CServiceMessage &svMsg)
{
	CString szpassword = pCHouseKeeping->szEngPassword;

	if (HMIGetPassword("Please enter password", szpassword))
	{
		KillHmi();
	}

	return 0;
}

LONG CAC9000App::SetMachineID(IPC_CServiceMessage &svMsg)
{
	CString szpassword = pCHouseKeeping->szSvrPassword;

	if (HMIGetPassword("Please enter service password", szpassword))
	{
		if (HMIAlphaKeys("Please enter Machine ID", &m_szMachineID))
		{
			WriteProfileString(gszPROFILE_SETTING, gszMACHINE_ID, m_szMachineID);
		}
	}

	return 0;
}

VOID CAC9000App::PrintThreadID()
{
	//ThreadID is log by vision log file. Sometime purge channel happen. So we have to look at this table to find out which thread call vision function that cause the purge channel.
	CString szLine(" ");

	DisplayMessage("--------ThreadID List--------");

	szLine.Format("AC9000App:%ld", m_lThreadID);
	DisplayMessage(szLine);

	for (INT i = 0; i < GetStationList().GetSize(); i++)
	{
		szLine.Format("%s::%ld", GetStationList().GetAt(i)->GetStnName(), GetStationList().GetAt(i)->m_lThreadID);
		DisplayMessage(szLine);
	}
	DisplayMessage("--------------------------");
}

LONG CAC9000App::HMI_GetAllStatus(IPC_CServiceMessage &svMsg)
{
	GetAllStatus();

	BOOL bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);

	return 1;
}

LONG CAC9000App::HMI_ResetStat(IPC_CServiceMessage &svMsg)
{
	m_ulTotalGlassPicked = 0;
	m_ulTotalGlassRejected = 0;
	m_ulTotalInvalidGlass = 0;

	m_ulTotalACFWHPBPRError = 0;
	m_ulTotalGlassTolError = 0;
	m_ulTotalAngCorrError = 0;

	m_ulTotalCPAPickError = 0;

	if (g_pPerformance_report != NULL)	// 20140812 Yip: Include Previous Bonded Units In m_lBondingStart
	{
		g_pPerformance_report->m_lBondingStart -= m_ulTotalUnitsBonded;
	}
	m_ulTotalUnitsBonded = 0;

	return 0;
}

LONG CAC9000App::HMI_SetAllModSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	for (int i = 0; i < GetStationList().GetSize(); i++)
	{
		if (GetStationList().GetAt(i)->GetStnName() != "TimeChart")
		{
			GetStationList().GetAt(i)->SetModSelected(bMode);
		}
	}

	for (int i = 0; i < GetPRStationList().GetSize(); i++)
	{
		GetPRStationList().GetAt(i)->SetPRSelected(bMode);
	}

	return 0;
}

LONG CAC9000App::HMI_CheckAllModSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	for (int i = 0; i < GetStationList().GetSize(); i++)
	{
		GetStationList().GetAt(i)->CheckModSelected(bMode);
	}

	return 0;
}

LONG CAC9000App::HMI_SetAllPRSelected(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	for (int i = 0; i < GetPRStationList().GetSize(); i++)
	{
		GetPRStationList().GetAt(i)->SetPRSelected(bMode);
	}

	return 0;
}

LONG CAC9000App::HMI_SetAllDebugSeq(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	for (int i = 0; i < GetStationList().GetSize(); i++)
	{
		GetStationList().GetAt(i)->HMI_bDebugSeq = bMode;
	}

	return 0;
}

LONG CAC9000App::HMI_SetAllLogMotion(IPC_CServiceMessage &svMsg) //20130422
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	for (int i = 0; i < GetStationList().GetSize(); i++)
	{
		GetStationList().GetAt(i)->HMI_bLogMotion = bMode;
	}

	return 0;
}

LONG CAC9000App::HMI_PickGlassOperation(IPC_CServiceMessage &svMsg)
{

	BOOL bOpDone;
	LONG lResponse;
	LONG lPlaceGlassStation;
	GlassNum lCurPickStatus			= MAX_NUM_OF_GLASS;
	LONG lCurGlass = GLASS1;


	lResponse = HMISelectionBox("PICK GLASS OPERATION", "Please select where to place glass to:", "ACF1WH", "ACF2WH");
	if (lResponse == -1 || lResponse == 11)
	{
		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Operation Abort!");
		bOpDone = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
		return 1;
	}

	lPlaceGlassStation = lResponse;


	/*lResponse = HMISelectionBox("PICK GLASS OPERATION", "Pick Which Glass", "Both Glass", "Glass 1", "Glass 2");
	if (lResponse == -1 || lResponse == 11)
	{
		HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Operation Abort!");
		bOpDone = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
		return 1;
	}

	if (lResponse == 0)
	{
		lCurPickStatus = MAX_NUM_OF_GLASS;
	}
	else if (lResponse == 1)
	{
		lCurPickStatus = GLASS1;
	}
	else if (lResponse == 2)
	{
		lCurPickStatus = GLASS2;
	}*/


	lCurPickStatus = GLASS1;

	switch (lPlaceGlassStation)
	{
	//ACF1WH
	case 0:
		if (
			!CheckGlassIndexingPath(pCACF1WH->GetStnName(), lCurPickStatus) || 
			pCInPickArm->MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != gnAMS_OK ||
			//pCACF1WH->MoveZToStandbyLevel(MAX_NUM_OF_GLASS, GMP_WAIT) != gnAMS_OK ||
			pCTA1->MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != gnAMS_OK ||
			pCTA1->MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != gnAMS_OK ||
			!pCACF1WH->PickGlassOperation(lCurPickStatus)
		   )
		{
			HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Operation Abort!");
			bOpDone = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bOpDone);
			return 1;
		}
		break;

	//ACF2WH // special take care
	case 1:
		if (
			!CheckGlassIndexingPath(pCACF2WH->GetStnName(), lCurPickStatus) || 
			pCInPickArm->MoveZ(IN_PICKARM_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != gnAMS_OK ||
			//pCACF2WH->MoveZToStandbyLevel(MAX_NUM_OF_GLASS, GMP_WAIT) != gnAMS_OK ||
			pCTA1->MoveZ(TA1_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != gnAMS_OK ||
			pCTA1->MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_WAIT) != gnAMS_OK ||
			!pCACF2WH->PickGlassOperation(lCurPickStatus)
		   )
		{
			HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Operation Abort!");
			bOpDone = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bOpDone);
			return 1;
		}
		break;

	}

	bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	
	return 1;
}

LONG CAC9000App::HMI_CleanOperation(IPC_CServiceMessage &svMsg)
{
	LONG lAnswer = 0;
	lAnswer = HMIMessageBox(MSG_OK_CANCEL, "Clean Operation", "Move modules to Clean Posn?");

	if (lAnswer == rMSG_OK)
	{
		pCTA1->m_stMotorX.IsPowerOn();
		pCTA2->m_stMotorX.IsPowerOn();

		pCACF1WH->m_stMotorY.IsPowerOn();


		pCTA1->MoveX(TA1_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT);
		pCTA2->MoveX(TA2_X_CTRL_GO_STANDBY_POSN, GMP_NOWAIT);

		pCACF1WH->m_stMotorY.Sync();

		pCTA1->m_stMotorX.Sync();
		pCTA2->m_stMotorX.Sync();

	//HMIMessageBox(MSG_OK, "CLEAN OPERATION", "Press 'OK' when finished cleaning");

	//pCINWH->SetModSelected(ON);
	//pCACF1WH->SetModSelected(ON);
	//pCACF2WH->SetModSelected(ON);
	//pCPreBondWH->SetModSelected(ON);
	//pCMB1WH->SetModSelected(ON);
	//pCMB2WH->SetModSelected(ON);
	//pCTA1->SetModSelected(ON);
	//pCTA2->SetModSelected(ON);
	//pCTA3->SetModSelected(ON);
	//pCTA4->SetModSelected(ON);
	//pCTA5->SetModSelected(ON);
	}


	BOOL bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}

LONG CAC9000App::HMI_FlushMode(IPC_CServiceMessage &svMsg)
{
	g_bFlushMode = TRUE;
	WriteHMIMess("Flushing in progress...");
	return 0;
}

LONG CAC9000App::HMI_CheckGatewayConnection(IPC_CServiceMessage &svMsgIn) //20130619
{
	IPC_CServiceMessage svMsg;
	INT nConvID = 0;
	LONG lReply = 0;
	LONG lTimeCount = 0;
	const char szTxt[] = _T("Hello_COG902_How_Are_You");
	char *pBuffer = new char[sizeof(szTxt)];

	INT nIndex = 0;
	clock_t clkNow, clkInit;
	CString szMsg = _T(" ");

	clkInit = clock();
	HMI_lGatewayStatus = NETWORK_CONNECTING;
	memset(&pBuffer[0], 0, sizeof(szTxt));
	memcpy(&pBuffer[0], &szTxt[0], sizeof(szTxt));
	nIndex = sizeof(szTxt);

	DisplayMessage("Network CheckConnection via gateway socket and waiting for reply.");
	svMsg.InitMessage(nIndex, pBuffer);

	try
	{
		nConvID = m_comClient.SendRequest("HK_COG902GW", "HK_CheckGatewayConnection", svMsg); //"HK_CheckAC9000GatewayConnection", svMsg);
	}
	catch (CAsmException e) //20140609
	{
		CString szMsg;
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szMsg.Format("Func:%s", __FUNCTION__);
		DisplayMessage(szMsg);
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

		szMsg.Format("Gateway Send Data Fail. Pls check gateway connection Setting.");
		DisplayMessage(szMsg);
		HMIMessageBox(MSG_OK, "Network Connection", szMsg);

		delete[] pBuffer;
		return 0;
	}
	while (1)
	{
		if (m_comClient.ReadReplyForConvID(nConvID, svMsg, NULL, NULL, 0))
		{
			if (svMsg.GetMsgLen() == sizeof(LONG))
			{
				svMsg.GetMsg(sizeof(LONG), &lReply);
				clkNow = clock();
				HMI_lGatewayStatus = lReply;

				szMsg.Format("CheckConnection Reply=0x%x", lReply);
				DisplayMessage(szMsg);

				if (lReply == gnAMS_OK) //0
				{
					HMI_lGatewayStatus = NETWORK_GOOD;
				}
				else
				{
					HMI_lGatewayStatus = NETWORK_NIL;
				}
				
				szMsg.Format("COG902 Replay Time: %.4f sec", (DOUBLE)(clkNow - clkInit) / CLOCKS_PER_SEC);
				DisplayMessage(szMsg);
				HMIMessageBox(MSG_OK, "Network Connection", szMsg);
			}
			break;
		}
		else if (lTimeCount++ > 6000) //6 sec timeout limit
		{
			lReply		= glHMI_TIMEOUT;
			pCHouseKeeping->SetError(IDS_HK_OPERATION_TIMEOUT);
			HMI_lGatewayStatus = NETWORK_NIL;
			DisplayMessage("CheckConnection timeout error");
			break;
		}
		else
		{
			while (m_comServer.ProcessRequest())
			{
				;
			}
			Sleep(1);
		}
	}

	delete[] pBuffer;

	return 0;
}

LONG CAC9000App::GatewayCmdAskForGlass(GlassNum lGlassNum) //20130725
{
//ask GC902 to pick glass and then WH-X move to unload Posn, WH-Z move to standby Level
	IPC_CServiceMessage svMsg;
	LONG lResult = gnAMS_OK;
	INT nConvID = 0;
	LONG lReply = 0;
	LONG lTimeCount = 0;
	INT nIndex = 0;
	CString szMsg = _T(" ");
	const char szTxt1[] = _T("GC902_PickGlass1");
	const char szTxt2[] = _T("GC902_PickGlass2");
	const char szTxtBoth[] = _T("GC902_PickGlass1_2");
	char *pCmd = NULL;

	clock_t clkNow, clkInit;

	clkInit = clock();

	switch (lGlassNum)
	{
	case GLASS1:
		pCmd = (char*)&szTxt1[0];
		DisplayMessage("Network Ask ECleaner Pick Glass1 and waiting for reply.");
		break;
	//case GLASS2:
	//	pCmd = (char*)&szTxt2[0];
	//	DisplayMessage("Network Ask ECleaner Pick Glass2 and waiting for reply.");
	//	break;
	case MAX_NUM_OF_GLASS:
	default:
		pCmd = (char*)&szTxtBoth[0];
		DisplayMessage("Network Ask ECleaner Pick Both Glass and waiting for reply.");
		break;
	}

	//nIndex = sizeof(pCmd);
	nIndex = strlen(pCmd) + 1;
	svMsg.InitMessage(nIndex, pCmd);

	try
	{
		nConvID = m_comClient.SendRequest("HK_COG902GW", "HK_ProcessGatewayCommand", svMsg);
	}
	catch (CAsmException e) //20140609
	{
		CString szMsg;
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szMsg.Format("Func:%s", __FUNCTION__);
		DisplayMessage(szMsg);
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

		szMsg.Format("Gateway Send Data Fail. Pls check gateway connection Setting.");
		DisplayMessage(szMsg);
		HMIMessageBox(MSG_OK, "Network ask for glass", szMsg);

		return gnAMS_NOTOK;
	}
	while (1)
	{
		if (m_comClient.ReadReplyForConvID(nConvID, svMsg, NULL, NULL, 0))
		{
			if (svMsg.GetMsgLen() == sizeof(LONG))
			{
				svMsg.GetMsg(sizeof(LONG), &lReply);
				clkNow = clock();

				szMsg.Format("Pick Glass Gateway Reply=0x%x", lReply);
				DisplayMessage(szMsg);

				if (lReply == gnAMS_OK) //0
				{
					HMI_lGatewayStatus = NETWORK_GOOD;
					lResult = gnAMS_OK;
				}
				else
				{
					HMI_lGatewayStatus = NETWORK_NIL;
					lResult = gnAMS_NOTOK;
				}
				
				szMsg.Format("GC902 Pick Glass Replay Time: %.4f sec", (DOUBLE)(clkNow - clkInit) / CLOCKS_PER_SEC);
				DisplayMessage(szMsg);
				//HMIMessageBox(MSG_OK, "Network Connection", szMsg);
			}
			break;
		}
		else if (lTimeCount++ > 40 * 1000) //40 sec timeout limit
		{
			lReply		= glHMI_TIMEOUT;
			pCHouseKeeping->SetError(IDS_HK_OPERATION_TIMEOUT);
			HMI_lGatewayStatus = NETWORK_NIL;
			DisplayMessage("Pick Glass Connection timeout error");
			lResult = gnAMS_NOTOK;
			break;
		}
		else
		{
			while (m_comServer.ProcessRequest())
			{
				;
			}
			Sleep(1);
		}
	}

	return lResult;
}

LONG CAC9000App::GatewayCmdUnloadGlass(GlassNum lGlassNum) //20130725
{
//ask GC902 WH-Z move to unload Level and then turn off WH-Vac. Then WH-Z move to Standby Level
	IPC_CServiceMessage svMsg;
	LONG lResult = gnAMS_OK;
	INT nConvID = 0;
	LONG lReply = 0;
	LONG lTimeCount = 0;
	INT nIndex = 0;
	CString szMsg = _T(" ");
	const char szTxt1[] = _T("GC902_UnloadGlass1");
	const char szTxt2[] = _T("GC902_UnloadGlass2");
	const char szTxtBoth[] = _T("GC902_UnloadGlass1_2");
	char *pCmd = NULL;

	clock_t clkNow, clkInit;

	clkInit = clock();

	switch (lGlassNum)
	{
	case GLASS1:
		pCmd = (char*)&szTxt1[0];
		DisplayMessage("Network Ask ECleaner Pick Glass1 and waiting for reply.");
		break;
	//case GLASS2:
	//	pCmd = (char*)&szTxt2[0];
	//	DisplayMessage("Network Ask ECleaner Pick Glass2 and waiting for reply.");
	//	break;
	case MAX_NUM_OF_GLASS:
	default:
		pCmd = (char*)&szTxtBoth[0];
		DisplayMessage("Network Ask ECleaner Pick Both Glass and waiting for reply.");
		break;
	}

	//nIndex = sizeof(pCmd);
	nIndex = strlen(pCmd) + 1;
	svMsg.InitMessage(nIndex, pCmd);

	try
	{
		nConvID = m_comClient.SendRequest("HK_COG902GW", "HK_ProcessGatewayCommand", svMsg);
	}
	catch (CAsmException e) //20140609
	{
		CString szMsg;
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		szMsg.Format("Func:%s", __FUNCTION__);
		DisplayMessage(szMsg);
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

		szMsg.Format("Gateway Send Data Fail. Pls check gateway connection Setting.");
		DisplayMessage(szMsg);
		HMIMessageBox(MSG_OK, "Network Unload glass", szMsg);

		return gnAMS_NOTOK;
	}



	while (1)
	{
		if (m_comClient.ReadReplyForConvID(nConvID, svMsg, NULL, NULL, 0))
		{
			if (svMsg.GetMsgLen() == sizeof(LONG))
			{
				svMsg.GetMsg(sizeof(LONG), &lReply);
				clkNow = clock();

				szMsg.Format("Unload Glass Gateway Reply=0x%x", lReply);
				DisplayMessage(szMsg);

				if (lReply == gnAMS_OK) //0
				{
					HMI_lGatewayStatus = NETWORK_GOOD;
					lResult = gnAMS_OK;
				}
				else
				{
					HMI_lGatewayStatus = NETWORK_NIL;
					lResult = gnAMS_NOTOK;
				}
				
				szMsg.Format("GC902 Unload Glass Replay Time: %.4f sec", (DOUBLE)(clkNow - clkInit) / CLOCKS_PER_SEC);
				DisplayMessage(szMsg);
				//HMIMessageBox(MSG_OK, "Network Connection", szMsg);
			}
			break;
		}
		else if (lTimeCount++ > 15 * 1000) //15 sec timeout limit
		{
			lReply		= glHMI_TIMEOUT;
			pCHouseKeeping->SetError(IDS_HK_OPERATION_TIMEOUT);
			HMI_lGatewayStatus = NETWORK_NIL;
			DisplayMessage("Unload Glass Connection timeout error");
			lResult = gnAMS_NOTOK;
			break;
		}
		else
		{
			while (m_comServer.ProcessRequest())
			{
				;
			}
			Sleep(1);
		}
	}

	return lResult;
}


INT CAC9000App::UnselectModuleForIndexingMode()
{
	//for (int i=0; i<GetPRStationList().GetSize(); i++)
	//{
	//	GetPRStationList().GetAt(i)->SetPRSelected(FALSE);
	//}

	// Disable
	pCACF1->SetModSelected(FALSE);

	// Disable PostBond
	//pCInsp1->SetPRSelected(FALSE);
	//pCInsp2->SetPRSelected(FALSE);
	//pCACF1WH->SetPRSelected(FALSE);
	//pCACF2WH->SetPRSelected(FALSE);

	return gnOK;
}

BOOL CAC9000App::ShowPRUploadErrorMessage(PRU *pPRU, CString &szErrorMsg) //20130713
{
	INT nFind = 0;
	CString szTitle = _T("");

	if (pPRU == NULL)
	{
		return FALSE;
	}
	if (CString(pPRU->stUploadRecordCmd.acFilename).GetLength() == 0)
	{
		return FALSE;
	}

	DisplayMessage(CString("PR Upload Record error at file:") + CString(pPRU->stUploadRecordCmd.acFilename) + CString(", Error:") + szErrorMsg);

	szTitle.Format("%s", pPRU->stUploadRecordCmd.acFilename);
	nFind = szTitle.ReverseFind('\\');
	if (nFind != -1)
	{
		szTitle = szTitle.Right(szTitle.GetLength() - nFind - 1);
	}
	HMIMessageBox(MSG_OK, szTitle, 
				  CString("File cannot be saved properly. Please revise and try again. err:") + szErrorMsg);
	
	return TRUE;
}

BOOL CAC9000App::ShowPRDownloadErrorMessage(PRU *pPRU, CString &szErrMsg) //20130713
{
	INT nFind = 0;
	CString szTitle = _T("");

	if (pPRU == NULL)
	{
		return FALSE;
	}

	DisplayMessage(CString("PR Download Record error at file:") + CString(pPRU->stDownloadRecordCmd.acFilename));
	szTitle.Format("%s", pPRU->stDownloadRecordCmd.acFilename);
	nFind = szTitle.ReverseFind('\\');
	if (nFind != -1)
	{
		szTitle = szTitle.Right(szTitle.GetLength() - nFind - 1);
	}
	HMIMessageBox(MSG_OK, szTitle, 
				  CString("File cannot be loaded properly. Please revise and try again. err:") + szErrMsg);

	return TRUE;
}

LONG CAC9000App::LoadSoftwareVersion()
{
	//Load Device File;
	CSingleLock slLock(&m_csDevice);
	slLock.Lock();

	CFileFind			fileFind;
	
	//Create Param Directory if not exist
	if (!fileFind.FindFile(m_szDevicePath))
	{
		if (!m_pInitOperation->CreateFileDirectory(m_szDevicePath))
		{
			AfxMessageBox("Cannot create Directory");
			slLock.Unlock();

			return FALSE;
		}
	}

	//Open Device File
	m_smfDevice.Close(); //Clear File buffer

	// File not find. Create a new one!
	if (!m_smfDevice.Open(m_szDevicePath + "\\" + m_szDeviceFile, FALSE, FALSE))
	{
		m_szDeviceFile = "CG144.prm";

		if (!m_smfDevice.Open(m_szDevicePath + "\\" + m_szDeviceFile, FALSE, TRUE))
		{
			AfxMessageBox("Cannot load File \"" + m_szDevicePath + "\\" + m_szDeviceFile + "\"");
			slLock.Unlock();
			return FALSE;
		}
	}

	//load the sw version
	pCHouseKeeping->m_dPrevSWVersion	= m_smfDevice["HouseKeeping"]["SW Version"];
	pCHouseKeeping->m_dCurSWVersion		= pCHouseKeeping->GetSoftwareVersion(gszSOFTWARE_VERSION);
	
	m_smfDevice.Update();
	slLock.Unlock();

	return 0;
}

LONG CAC9000App::SaveSoftwareVersion()
{
	//Load Device File;
	CSingleLock slLock(&m_csDevice);
	slLock.Lock();

	CFileFind			fileFind;
	
	//Create Param Directory if not exist
	if (!fileFind.FindFile(m_szDevicePath))
	{
		if (!m_pInitOperation->CreateFileDirectory(m_szDevicePath))
		{
			AfxMessageBox("Cannot create Directory");
			slLock.Unlock();

			return FALSE;
		}
	}

	//Open Device File
	m_smfDevice.Close(); //Clear File buffer

	// File not find. Create a new one!
	if (!m_smfDevice.Open(m_szDevicePath + "\\" + m_szDeviceFile, FALSE, FALSE))
	{
		if (!m_smfDevice.Open(m_szDevicePath + "\\" + m_szDeviceFile, FALSE, TRUE))
		{
			AfxMessageBox("Cannot load File \"" + m_szDevicePath + "\\" + m_szDeviceFile + "\"");
			slLock.Unlock();
			return FALSE;
		}
	}

	//load the sw version
	//m_smfDevice["HouseKeeping"]["SW Version"] = pCHouseKeeping->GetSoftwareVersion(gszSOFTWARE_VERSION);
	
	m_smfDevice.Update();
	slLock.Unlock();

	return 0;
}

///////////////////////////
// LOG
///////////////////////////
BOOL CAC9000App::MotionLog(const CString &szFunction, DOUBLE dVar1, DOUBLE dVar2, BOOL bWait)
{
	BOOL bResult = TRUE;

	CString szPrintData = _T("");
	CString szTempData = _T("");

	szPrintData.Format("%ld\t", GetTickCount());

	szPrintData += szFunction + _T("\t");

	szTempData.Format("%.2f\t", dVar1);

	szPrintData += szTempData;

	szTempData.Format("%.2f\t", dVar2);

	szPrintData += szTempData;

	if (bWait)
	{
		szTempData = "WAIT";
	}
	else
	{
		szTempData = "NO_WAIT";
	}

	szPrintData += szTempData;

	FILE *fp = fopen(_T("D:\\sw\\AC9000\\Data\\MotionLog.txt"), "a+");
	if (fp != NULL)
	{
		fprintf(fp, szPrintData + _T("\n"));
		fclose(fp);
	}

	return bResult;
}

// 20140729 Yip
VOID CAC9000App::LogParameter(const CString &szStation, const CString &szFunction, const CString &szParameter, const CString &szOldValue, const CString &szNewValue)
{
	m_csParameterLogLock.Lock();

	if (bEnableLogging && (szOldValue != szNewValue))
	{
		SYSTEMTIME ltime;
		GetLocalTime(&ltime);

		CString szLogDirectory = "D:\\sw\\AC9000\\Data\\ParameterLog";
		CreateDirectory(szLogDirectory.GetString(), NULL);

		CString szFileName;
		szFileName.Format("%s\\%s_%04u%02u%02u.log", szLogDirectory, m_szMachineID, ltime.wYear, ltime.wMonth, ltime.wDay);

		CFileFind fileFind;
		if (!fileFind.FindFile(szFileName))
		{
			CStdioFile File;
			if (File.Open(szFileName, CFile::modeCreate | CFile::modeWrite))
			{
				File.WriteString("Time\tStation\tFunction\tParameter\tOld Value\tNew Value\n");
				File.Close();
			}
		}

		CString szMsg;
		szMsg = szStation + "\t" + szFunction + "\t" + szParameter + "\t" + szOldValue + "\t" + szNewValue;
		WriteToFile(szFileName, szMsg);
	}

	m_csParameterLogLock.Unlock();
}

// 20140812 Yip
VOID CAC9000App::LogParameter(const CString &szFunction, const CString &szParameter, const CString &szOldValue, const CString &szNewValue)
{
	LogParameter("AC9000App", szFunction, szParameter, szOldValue, szNewValue);
}

LONG CAC9000App::HMI_SelectStation(IPC_CServiceMessage &svMsg)
{
	LONG	lMsgLen = svMsg.GetMsgLen();
	CString strCurrentStation;
#if 1 //20150615
	UCHAR cLine[_MAX_PATH] = {0, };
	UCHAR	*pcName = &cLine[0];

	memset(&cLine[0], 0, _MAX_PATH);

	if (lMsgLen + 1 > _MAX_PATH)
	{
		CString szMsgTmp("");
		HMIMessageBox(MSG_OK, "WARNING", "Software Handling Error. Operation Abort!");
		szMsgTmp.Format("%s: Message Length Error", __FUNCTION__);
		DisplayMessage(szMsgTmp);
		return 0;
	}
	else if (lMsgLen > 0)
	{
		svMsg.GetMsg(pcName, lMsgLen);
		strCurrentStation.Format("%s", pcName);
	}
#else
	UCHAR	*cName;

	if (lMsgLen > 0)
	{
		cName = new UCHAR[lMsgLen];
		svMsg.GetMsg(cName, lMsgLen);
		strCurrentStation = (CString)cName;
	}
#endif		
	CString szFileName;
	szFileName = "D:\\sw\\AC9000\\Param\\CurrentModuleMotorList.csv";

	if (strCurrentStation == "")
	{
		// Set data to file
		FILE *fp = fopen(szFileName, "wt");
		
		if (fp != NULL)
		{
			fclose(fp);
		}
	}

	// Print The Motor List Name:
	for (INT i = 0; i < GetStationList().GetSize(); i++)
	{
		if (
			GetStationList().GetAt(i)->GetStnName() == strCurrentStation && 
			GetStationList().GetAt(i)->GetmyMotorList().GetSize() != 0
		   )
		{
			
			// Set data to file
			FILE *fp = fopen(szFileName, "wt");

			for (INT j = 0; j < GetStationList().GetAt(i)->GetmyMotorList().GetSize(); j++)	
			{
				if (GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->stAttrib.bEncoderExist)
				{
					if (fp != NULL)
					{
						fprintf(fp, "%s,\n", (const char*)GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->GetName());
					}
				}
			}

			if (fp != NULL)
			{
				fclose(fp);
			}
		}
	}
	
	// Reset Display Value
	getStationPicFilename(strCurrentStation, HMI_szMotorDirectionPng); //20120620
	//HMI_szMotorDirectionPng	= "D:\\sw\\AC9000\\Hmi\\images\\modules (png)\\" +  strCurrentStation + ".png";
	HMI_lMotorPosLmt		= 0;
	HMI_lMotorNegLmt		= 0;

	return 0;
}


VOID CAC9000App::getStationPicFilename(CString &szStationName, CString &szPicFileName) //20120620
{
	CString szTmpFileName;
	if (szStationName == "TA2" || szStationName == "TA3")
	{
		szTmpFileName	= "PTA";
	}
	else if (szStationName == "TA4" || szStationName == "TA5")
	{
		szTmpFileName	= "MTA";
	}
	else if (szStationName == "ACF1" || szStationName == "ACF2")
	{
		szTmpFileName	= "ACF";
	}
	else if (szStationName == "PB1" || szStationName == "PB2")
	{
		szTmpFileName	= "PB";
	}
	else if (szStationName == "PreBondWH")
	{
		szTmpFileName	= "PBWH";
	}
	else if (szStationName == "InConv")
	{
		szTmpFileName	= "InCell(Rotated)";
	}
	else if (szStationName == "ACF1WH" || szStationName == "ACF2WH")
	{
		szTmpFileName	= "ACFWH";
	}
	else if (szStationName == "MB1" || szStationName == "MB2")
	{
		szTmpFileName	= "MB";
	}
	else if (szStationName == "MB1WH" || szStationName == "MB2WH")
	{
		szTmpFileName	= "MBWH";
	}
	else
	{
		szTmpFileName	= szStationName;
	}

	szPicFileName	= "D:\\sw\\AC9000\\Hmi\\images\\modules (png)\\" + szTmpFileName + ".png";
	return;
}
LONG CAC9000App::HMI_SelectStationMotor(IPC_CServiceMessage &svMsg)
{
	LONG	lMsgLen = svMsg.GetMsgLen();
	UCHAR	*cName = NULL;
	CString temp;
	CString	HMI_Name;
	
	HMI_bTA1Z_Selected	= FALSE;


	if (lMsgLen > 0)
	{
#if 1 //20150615
		UCHAR cLine[_MAX_PATH] = {0, };

		cName = &cLine[0];
		memset(&cLine[0], 0, _MAX_PATH);

		if (lMsgLen + 1 > _MAX_PATH)
		{
			CString szMsgTmp("");
			HMIMessageBox(MSG_OK, "WARNING", "Software Handling Error. Operation Abort!");
			szMsgTmp.Format("%s: Message Length Error", __FUNCTION__);
			DisplayMessage(szMsgTmp);
			return 0;
		}
		else if (lMsgLen > 0)
		{
			svMsg.GetMsg(cName, lMsgLen);
		}
#else
		cName = new UCHAR[lMsgLen];
		svMsg.GetMsg(cName, lMsgLen);
#endif
		// Find the Motor from the actuator name
		for (INT i = 0; i < GetStationList().GetSize(); i++)
		{
			for (INT j = 0; j < GetStationList().GetAt(i)->GetmyMotorList().GetSize(); j++)	
			{
				if ((CString)cName == GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->GetName())
				{	
					m_pStMotorSWLimit = GetStationList().GetAt(i)->GetmyMotorList().GetAt(j);
					m_CurrMotor = GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->GetName();
					HMI_lMotorPosLmt = GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->stProtect.lPosLmt;
					HMI_lMotorNegLmt = GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->stProtect.lNegLmt;
					break;
				}
			}
		}
	}
	if (m_CurrMotor == "TA1_Z")
		{
			HMI_bTA1Z_Selected = TRUE;
		}

	return 0;
}

LONG CAC9000App::HMI_SetMotorProtectPosLmt(IPC_CServiceMessage &svMsg)
{
	LONG	lMsgLen = svMsg.GetMsgLen();
	UCHAR	*cName = NULL;
	CString tempMotorName;

	if (lMsgLen > 0)
	{
#if 1 //20150615
		UCHAR cLine[_MAX_PATH] = {0, };

		cName = &cLine[0];
		memset(&cLine[0], 0, _MAX_PATH);

		if (lMsgLen + 1 > _MAX_PATH)
		{
			CString szMsgTmp("");
			HMIMessageBox(MSG_OK, "WARNING", "Software Handling Error. Operation Abort!");
			szMsgTmp.Format("%s: Message Length Error", __FUNCTION__);
			DisplayMessage(szMsgTmp);
			return 0;
		}
		else if (lMsgLen > 0)
		{
			svMsg.GetMsg(cName, lMsgLen);
		}
#else
		cName = new UCHAR[lMsgLen];
		svMsg.GetMsg(cName, lMsgLen);
#endif
		// Find the Motor from the actuator name
		for (INT i = 0; i < GetStationList().GetSize(); i++)
		{
			for (INT j = 0; j < GetStationList().GetAt(i)->GetmyMotorList().GetSize(); j++)	
			{
				if ((CString)cName == GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->GetName())
				{
#if 1 //20120620
					BOOL bResult = TRUE;
					if (GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->stProtect.lNegLmt >= GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->GetEncPosn())
					{
						LONG lAnswer = rMSG_TIMEOUT;

						lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Negative Limit is larger than Positive Limit. Continue?");

						if (lAnswer == rMSG_TIMEOUT || lAnswer == rMSG_CANCEL)
						{
							HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
							bResult = FALSE;
						}
					}
					if (bResult)
					{
						GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->stProtect.lPosLmt = GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->GetEncPosn();
						HMI_lMotorPosLmt = GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->stProtect.lPosLmt;
						GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->SetSoftwareLmt();
					}
#else
					//GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->stProtect.lPosLmt = GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->GetEncPosn();
					//HMI_lMotorPosLmt = GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->stProtect.lPosLmt;
					//GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->SetSoftwareLmt();
#endif
					return 0;

				}
			}
		}
	}

	HMIMessageBox(MSG_OK, "WARNING", "Please select module and motor first");
	return 0;
}

LONG CAC9000App::HMI_SetMotorProtectNegLmt(IPC_CServiceMessage &svMsg)
{
	// Print The Motor List Name:
	LONG	lMsgLen = svMsg.GetMsgLen();
	UCHAR	*cName = NULL;
	CString tempMotorName;

	if (lMsgLen > 0)
	{
#if 1 //20150615
		UCHAR cLine[_MAX_PATH] = {0, };

		cName = &cLine[0];
		memset(&cLine[0], 0, _MAX_PATH);

		if (lMsgLen + 1 > _MAX_PATH)
		{
			CString szMsgTmp("");
			HMIMessageBox(MSG_OK, "WARNING", "Software Handling Error. Operation Abort!");
			szMsgTmp.Format("%s: Message Length Error", __FUNCTION__);
			DisplayMessage(szMsgTmp);
			return 0;
		}
		else if (lMsgLen > 0)
		{
			svMsg.GetMsg(cName, lMsgLen);
		}
#else
		cName = new UCHAR[lMsgLen];
		svMsg.GetMsg(cName, lMsgLen);
#endif
		// Find the Motor from the actuator name
		for (INT i = 0; i < GetStationList().GetSize(); i++)
		{
			for (INT j = 0; j < GetStationList().GetAt(i)->GetmyMotorList().GetSize(); j++)	
			{
				if ((CString)cName == GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->GetName())
				{	
#if 1 //20120620
					BOOL bResult = TRUE;
					if (GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->stProtect.lPosLmt <= GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->GetEncPosn())
					{
						LONG lAnswer = rMSG_TIMEOUT;

						lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", "Positive Limit is small than Negative Limit. Continue?");

						if (lAnswer == rMSG_TIMEOUT || lAnswer == rMSG_CANCEL)
						{
							HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
							bResult = FALSE;
						}
					}
					if (bResult)
					{
						GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->stProtect.lNegLmt = GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->GetEncPosn();
						HMI_lMotorNegLmt = GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->stProtect.lNegLmt;
						GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->SetSoftwareLmt();
					}
#else
					GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->stProtect.lNegLmt = GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->GetEncPosn();
					HMI_lMotorNegLmt = GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->stProtect.lNegLmt;
					GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->SetSoftwareLmt();
#endif
					return 0;
				}
			}
		}
	}
	

	HMIMessageBox(MSG_OK, "WARNING", "Please select module and motor first");
	
	return 0;

}

LONG CAC9000App::HMI_EnterSoftwareLimitPage(IPC_CServiceMessage &svMsg)
{
	CString szStation;
	m_pStMotorSWLimit = NULL;

	for (INT i = 0; i < GetStationList().GetSize(); i++)
	{
		szStation = GetStationList().GetAt(i)->GetStnName();
		if (szStation == "PreBondWH" || szStation == "INWH")
		{
			for (INT j = 0; j < GetStationList().GetAt(i)->GetmyMotorList().GetSize(); j++)	
			{
				GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->ModifySWProtection(FALSE);
			}
		}
	}

	return 0;
}

LONG CAC9000App::HMI_LeaveSoftwareLimitPage(IPC_CServiceMessage &svMsg)
{
	CString szStation;
	m_pStMotorSWLimit = NULL;

	for (INT i = 0; i < GetStationList().GetSize(); i++)
	{
		szStation = GetStationList().GetAt(i)->GetStnName();
		if (szStation == "PreBondWH" || szStation == "INWH")
		{
			for (INT j = 0; j < GetStationList().GetAt(i)->GetmyMotorList().GetSize(); j++)	
			{
				//if ((CString)cName == GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->GetName())
				{	
					GetStationList().GetAt(i)->GetmyMotorList().GetAt(j)->ModifySWProtection(TRUE);
				}
			}
		}
	}

	return 0;
}

VOID CAC9000App::SetDiagnSteps(ULONG ulSteps)
{
	switch (ulSteps)
	{
	case 0:
		m_lDiagnSteps = 1;
		break;
	case 1:
		m_lDiagnSteps = 10;
		break;
	case 2:
		m_lDiagnSteps = 100;
		break;
	case 3:
		m_lDiagnSteps = 1000;
		break;
	case 4:
		m_lDiagnSteps = 5000;
		break;
	case 5:
		m_lDiagnSteps = 10000;
		break;
	default:
		m_lDiagnSteps = 5000;
	}
}

LONG CAC9000App::HMI_SetDiagnSteps(IPC_CServiceMessage &svMsg)
{
	ULONG ulSteps;
	svMsg.GetMsg(sizeof(ULONG), &ulSteps);

	SetDiagnSteps(ulSteps);
	return 0;
}

LONG CAC9000App::HMI_SWLimitIndexMotorPos(IPC_CServiceMessage &svMsg)
{
	
	if (m_pStMotorSWLimit != NULL)
	{
		m_pStMotorSWLimit->MoveRelative(m_lDiagnSteps, GMP_WAIT);
	}
	else
	{
		HMIMessageBox(MSG_OK, "Motor Index Pos", "Please Select a Motor");
	}
	return 0;
}

LONG CAC9000App::HMI_SWLimitIndexMotorNeg(IPC_CServiceMessage &svMsg)
{
	if (m_pStMotorSWLimit != NULL)
	{
		m_pStMotorSWLimit->MoveRelative(-m_lDiagnSteps, GMP_WAIT);
	}
	else
	{
		HMIMessageBox(MSG_OK, "Motor Index Neg", "Please Select a Motor");
	}
	return 0;
}

LONG CAC9000App::HMI_DataLog(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	if (bMode)
	{
		// 20140922 Yip: Add Datalog Mode And Stop Condition
		GMP_DATALOG_MODE enDatalogMode = GMP_CONTINUOUS;
		if (rMSG_YES == HMIMessageBox(MSG_YES_NO, "Start Datalog", "Stop datalog when first port not equal to 0?"))
		{
			enDatalogMode = GMP_STOP_EVT;
		}

		HMIMessageBox(MSG_OK, "DEVELOPER", "Start Datalog");
		StartDataLog(NULL, enDatalogMode);
	}
	else
	{
		StopDataLog("D:\\sw\\AC9000\\Data\\NuDataLog.txt");
	}
	return 0;
}

LONG CAC9000App::HMI_MoveAllWHtoBondPosn(IPC_CServiceMessage &svMsg) //20130318
{
	BOOL bStatus = TRUE;
	INT nResult = GMP_SUCCESS;
	BOOL bOpDone = TRUE;
	CString szPromptStr = _T("       ");

	//svMsg.GetMsg(sizeof(BOOL), &bMode);

	if (pCACF1WH == NULL || pCACF2WH == NULL)
	{
		svMsg.InitMessage(sizeof(BOOL), &bOpDone);
		return 1;
	}
	if (!pCACF1WH->m_bModSelected)
	{
		szPromptStr.Format("%s Module Not Selected. Operation Abort!", pCACF1WH->GetStnName());
		HMIMessageBox(MSG_OK, "MOVE ALL WH OPERATION", szPromptStr);
		bStatus = FALSE;
	}
	if (!pCACF2WH->m_bModSelected)
	{
		szPromptStr.Format("%s Module Not Selected. Operation Abort!", pCACF2WH->GetStnName());
		HMIMessageBox(MSG_OK, "MOVE ALL WH OPERATION", szPromptStr);
		bStatus = FALSE;
	}


	if (bStatus)
	{
		LONG lReply = HMIConfirmWindow("Yes", "No", "Cancel", "OPTION", "Move ALL WH to Bond Posn and Offset?");

		if (lReply == BUTTON_1)
		{
			if (
				pCACF1WH->SetupGoBondLevel(ACF1, false) == GMP_SUCCESS &&
				pCACF2WH->SetupGoBondLevel(ACF1, false) == GMP_SUCCESS
			   )
			{
				bStatus = TRUE;
			}
			else
			{
				bStatus = FALSE;
			}
		}
		else if (lReply == BUTTON_2 || lReply == BUTTON_3)
		{
			bStatus = FALSE;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}

LONG CAC9000App::HMI_KeepQuiet(IPC_CServiceMessage &svMsg)
{
	INT nResult = GMP_SUCCESS;
	BOOL bOpDone = TRUE;

	pCTA1->KeepQuiet();
	pCTA2->KeepQuiet();

	pCACF1WH->KeepQuiet();
	pCACF2WH->KeepQuiet();

	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}

LONG CAC9000App::HMI_ShowPerformanceReportPage(IPC_CServiceMessage &svMsg)	//Philip Add
{
	LONG ulPageNum = 0;
	svMsg.GetMsg(sizeof(LONG), &ulPageNum);
	g_ulDailyPerformanceReportPage = ulPageNum;
	return 1;
}

#ifdef EXCEL_MACHINE_DEVICE_INFO
VOID *CAC9000App::GetStnObjPointerByStnName(const CString &szStnName)
{
	//module inside GetStationList
	if (szStnName.GetLength() > 0)
	{
		if (szStnName == "ACF1WH")
		{
			return pCACF1WH;
		}
		else if (szStnName == "TA1")
		{
			return pCTA1;
		}
		else if (szStnName == "SettingFile")
		{
			return pCSettingFile;
		}
		else if (szStnName == "ACF2WH")
		{
			return pCACF2WH;
		}
		else if (szStnName == "TA2")
		{
			return pCTA2;
		}
		else if (szStnName == "TAManager")
		{
			return pCTAManager;
		}
		else if (szStnName == "LitecCtrl")
		{
			return pCLitecCtrl;
		}
		else if (szStnName == "AdamCtrl")
		{
			return pCAdamCtrl;
		}
		else if (szStnName == "HouseKeeping")
		{
			return pCHouseKeeping;
		}
		else if (szStnName == "InPickArm")
		{
			return pCInPickArm;
		}
		else if (szStnName == "TimeChart")
		{
			return pCTimeChart;
		}
		else if (szStnName == "TempDisplay")
		{
			return pCTempDisplay;
		}
		else if (szStnName == "InspOpt")
		{
			return pCInspOpt;
		}
		else if (szStnName == "ACF1")
		{
			return pCACF1;
		}
		else if (szStnName == "WinEagle")
		{
			return pCWinEagle;
		}
		//else if (szStnName == "ACF2")
		//{
		//	return pCACF2;
		//}
		else
		{
			return NULL;
		}
	}
	else
	{ 
	//error
		return NULL;
	}
}

LONG CAC9000App::HMI_PrintMachineDeviceInfoExcel(IPC_CServiceMessage &svMsg)
{
	BOOL bOpDone	= TRUE;
	CString szMsg, szDevice;
	const CString szExcelTemplateSrcPath = _T("D:\\sw\\AC9000\\Param\\"), szExcelTemplateDestPath = _T("D:\\sw\\AC9000\\Data\\"), szExcelTemplateFileName = _T("AC9000_Info_Template.xls");
	CString szTime = _T(""), szFileName = _T("");
	__time64_t ltime;

	_time64(&ltime);
	szTime = _ctime64(&ltime);
	szTime.TrimRight('\n');

	char tmpbuf[129];
	struct tm *today;
	today = _localtime64(&ltime);
	memset(&tmpbuf[0], 0 , 129);
	strftime(tmpbuf, 128, "%d_%b_%H_%M_%S", today);
	szFileName = szExcelTemplateDestPath + CString("AC9000_Info_") + CString(tmpbuf) + CString(".xls");

	CFileOperation fo;
	bOpDone = fo.Copy(szExcelTemplateSrcPath + szExcelTemplateFileName, szExcelTemplateDestPath);
	fo.Rename(szExcelTemplateDestPath + szExcelTemplateFileName, szFileName);

	BasicExcel excelDB((const char*)szFileName);

	szDevice = m_szDeviceFile;

	szDevice.TrimLeft("\\");
	g_lMachineInfoRowNum	= 0;
	g_lDeviceInfoRowNum		= 0;

	if (excelDB.Load(szFileName))
	{
		excelDB.New(2);
		excelDB.RenameWorksheet((size_t)0, (const char*)"AC9000_Machine_Info");		///< Rename an Excel worksheet at the given index to the given ANSI name. Index starts from 0. Returns true if successful, false if otherwise.
		excelDB.RenameWorksheet((size_t)1, (const char*)"AC9000_Device_Info");		///< Rename an Excel worksheet at the given index to the given ANSI name. Index starts from 0. Returns true if successful, false if otherwise.

		g_pExcelMachineInfoSheet = excelDB.GetWorksheet((size_t)0);
		g_pExcelDeviceInfoSheet = excelDB.GetWorksheet((size_t)1);

		if (g_pExcelMachineInfoSheet != NULL)
		{
			INT nCol = 0;
			g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, nCol++)->SetString("AC9000 Machine Data List");
			//g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, nCol++)->SetString(CString("Device:") + szDevice);
			g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, nCol++)->SetString(CString("Machine Model: ") + m_szMachineModel);
			g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, nCol++)->SetString(CString("Machine ID: ") + m_szMachineID);
			g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, nCol++)->SetString(CString("Software Version: ") + m_szSoftVersion);
			g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, nCol++)->SetString(CString("SW Release Date: ") + m_szReleaseDate);
			nCol = 10;
			g_pExcelMachineInfoSheet->Cell(g_lMachineInfoRowNum, nCol)->SetString(CString("Date:") + szTime);
			g_lMachineInfoRowNum++;

			nCol = 0;
			g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, nCol++)->SetString("AC9000 Device Data List");
			g_pExcelDeviceInfoSheet->Cell(g_lDeviceInfoRowNum, nCol++)->SetString(CString("Device:") + szDevice);
			nCol = 10;
			g_pExcelMachineInfoSheet->Cell(g_lDeviceInfoRowNum, nCol)->SetString(CString("Date:") + szTime);
			g_lDeviceInfoRowNum++;


			//INT i= 0;
			for (INT i = 0; i < GetStationList().GetSize(); i++)
			{
				void *pStn = NULL;
				pStn = GetStnObjPointerByStnName(GetStationList().GetAt(i)->GetStnName());
				if (pStn != NULL)
				{
					((CAC9000Stn*)pStn)->printMachineDeviceInfo();
				}
#if 1
				for (int jj = 0; jj < GetPRStationList().GetSize(); jj++)
				{
					if (GetPRStationList().GetAt(jj)->m_szPRTaskStnName == GetStationList().GetAt(i)->GetStnName())
					{
						GetPRStationList().GetAt(jj)->printPRTaskStnMachinePara();
					}
				}
#endif
			}

			if (excelDB.Save())
			{
				DisplayMessage("excel save ok");
				szMsg.Format("Total number of Line:%ld", g_lMachineInfoRowNum);
				DisplayMessage(szMsg);
			}
			excelDB.Close();
			g_pExcelMachineInfoSheet	= NULL;
			g_pExcelDeviceInfoSheet		= NULL;
		}
	}

	bOpDone = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bOpDone);
	return 1;
}
#endif

//Philip add
LONG CAC9000App::HMI_SettingReportManagerSaveShiftSchedule(IPC_CServiceMessage &svMsg)
{
	CString HMISettingReportManagerMessage = "";
	CString szTmpStr = "";
	HMISettingReportManagerMessage.Format("%ld,\n", HMI_ulSettingReportManagerStartTime);
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift1 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift2 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift3 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift4 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift5 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift6 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift7 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift8 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift9 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift10 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift11 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift12 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift13 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift14 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift15 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift16 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift17 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift18 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift19 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift20 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift21 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift22 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift23 + ",\n";
	HMISettingReportManagerMessage += HMI_szSettingReportManagerShift24 + ",\n";

	if (SaveSettingReportConfig(HMISettingReportManagerMessage))
	{
		HMIMessageBox(MSG_OK, "Notice", "New Setting Saved");
	}
	else
	{
		HMIMessageBox(MSG_OK, "Notice", "Cannot Save Setting");
	}

	return 0;
}

LONG CAC9000App::HMI_CheckStartBondingAllowed(IPC_CServiceMessage &svMsg) //20150610
{
	LONG lAnswer = rMSG_CANCEL;
	BOOL bAllow = TRUE, bResult = FALSE;

	//Check non auto-bond is select. Show warning to operator.
	if (AutoMode != AUTO_BOND)
	{
		lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "START BONDING", "Auto bonding is not Selected. Confirm to Continue?");

		if (lAnswer == rMSG_CANCEL || lAnswer == rMSG_TIMEOUT)
		{
			bAllow = FALSE;
		}
	}
	if (!bAllow)
	{
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (bAllow)
	{
		bResult = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}
	else
	{
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}
}

LONG CAC9000App::HMI_SetPRDebugInfo(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	g_bPRDebugInfo = bMode;

	return 0;
}

LONG CAC9000App::HMI_ToggleEnableAnvilVac(IPC_CServiceMessage &svMsg)
{
	g_bEnableAnvilVac = !g_bEnableAnvilVac;
	return 0;
}

LONG CAC9000App::HMI_ToggleUplookEnable(IPC_CServiceMessage &svMsg)
{
	g_bUplookPREnable = !g_bUplookPREnable;
	return 0;
}

BOOL CAC9000App::LoadSettingReportConfig()		//philip add
{
	CFileException ex;

	CString result = "";
	CString tmpStr = "";
	CFile reportSettingConfigFile;
	LONG ulTmp = 0;

	if (reportSettingConfigFile.Open("D:\\sw\\AC9000\\Release\\performance_report_config.csv", CFile::modeRead | CFile::shareDenyNone))
	{
		reportSettingConfigFile.SeekToBegin();
		CArchive reportSettingConfigAr(&reportSettingConfigFile, CArchive::load);
		int counter = 0;

		try
		{
			while (reportSettingConfigAr.ReadString(tmpStr))
			{
				tmpStr.Replace(",", "");
				if (counter == 0)
				{
					sscanf(tmpStr, "%ld", &ulTmp);
					HMI_ulSettingReportManagerStartTime = ulTmp;
				}
				else if (counter == 1)
				{
					HMI_szSettingReportManagerShift1.Format("%s", tmpStr);
				}
				else if (counter == 2)
				{
					HMI_szSettingReportManagerShift2.Format("%s", tmpStr);
				}
				else if (counter == 3)
				{
					HMI_szSettingReportManagerShift3.Format("%s", tmpStr);
				}
				else if (counter == 4)
				{
					HMI_szSettingReportManagerShift4.Format("%s", tmpStr);
				}
				else if (counter == 5)
				{
					HMI_szSettingReportManagerShift5.Format("%s", tmpStr);
				}
				else if (counter == 6)
				{
					HMI_szSettingReportManagerShift6.Format("%s", tmpStr);
				}
				else if (counter == 7)
				{
					HMI_szSettingReportManagerShift7.Format("%s", tmpStr);
				}
				else if (counter == 8)
				{
					HMI_szSettingReportManagerShift8.Format("%s", tmpStr);
				}
				else if (counter == 9)
				{
					HMI_szSettingReportManagerShift9.Format("%s", tmpStr);
				}
				else if (counter == 10)
				{
					HMI_szSettingReportManagerShift10.Format("%s", tmpStr);
				}
				else if (counter == 11)
				{
					HMI_szSettingReportManagerShift11.Format("%s", tmpStr);
				}
				else if (counter == 12)
				{
					HMI_szSettingReportManagerShift12.Format("%s", tmpStr);
				}
				else if (counter == 13)
				{
					HMI_szSettingReportManagerShift13.Format("%s", tmpStr);
				}
				else if (counter == 14)
				{
					HMI_szSettingReportManagerShift14.Format("%s", tmpStr);
				}
				else if (counter == 15)
				{
					HMI_szSettingReportManagerShift15.Format("%s", tmpStr);
				}
				else if (counter == 16)
				{
					HMI_szSettingReportManagerShift16.Format("%s", tmpStr);
				}
				else if (counter == 17)
				{
					HMI_szSettingReportManagerShift17.Format("%s", tmpStr);
				}
				else if (counter == 18)
				{
					HMI_szSettingReportManagerShift18.Format("%s", tmpStr);
				}
				else if (counter == 19)
				{
					HMI_szSettingReportManagerShift19.Format("%s", tmpStr);
				}
				else if (counter == 20)
				{
					HMI_szSettingReportManagerShift20.Format("%s", tmpStr);
				}
				else if (counter == 21)
				{
					HMI_szSettingReportManagerShift21.Format("%s", tmpStr);
				}
				else if (counter == 22)
				{
					HMI_szSettingReportManagerShift22.Format("%s", tmpStr);
				}
				else if (counter == 23)
				{
					HMI_szSettingReportManagerShift23.Format("%s", tmpStr);
				}
				else if (counter == 24)
				{
					HMI_szSettingReportManagerShift24.Format("%s", tmpStr);
				}
				counter++;
			}
		}
		catch (CAsmException e)
		{
			reportSettingConfigAr.Close();
			reportSettingConfigFile.Close();
			return false;
		}
		reportSettingConfigAr.Close();
		reportSettingConfigFile.Close();
		return true;
	}
	return false;
}

BOOL CAC9000App::SaveSettingReportConfig(CString inReportSettingConfig)		//philip add
{
	CFileException ex;
	CFile reportSettingConfigFile;

	if (reportSettingConfigFile.Open("D:\\sw\\AC9000\\Release\\performance_report_config.csv", CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone, &ex))
	{
		CArchive reportSettingConfigAr(&reportSettingConfigFile, CArchive::store);
			
		reportSettingConfigAr.WriteString(inReportSettingConfig);
		reportSettingConfigAr.Flush();

		reportSettingConfigAr.Close();
		reportSettingConfigFile.Close();
		return true;
	}
	return false;
}

VOID CAC9000App::setKlocworkTrue(BOOL bValue) //klocwork fix 20121211
{
	m_bKlocworkTrue = bValue;
	return;
}
BOOL CAC9000App::getKlocworkTrue()
{
	return m_bKlocworkTrue;
}
BOOL CAC9000App::getKlocworkFalse()
{
	return !m_bKlocworkTrue;
}

BOOL CAC9000App::CheckStopSuccess() //20150819
{
	//Check all stations have go to NON-Bonding state
	//If machine stop bonding successfully, all stations should be in IDLE state. Or not in Prestart, Demo, auto, stopping state
	SFM_CStation	*pStation = NULL;
	CAppStation	*pAppStation = NULL;
	CString			szKey = _T("");
	POSITION		pos;
	BOOL bResult = TRUE;

	for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
	{
		m_pStationMap.GetNextAssoc(pos, szKey, pStation);
		pAppStation = dynamic_cast<CAppStation*>(pStation);

		if (
			pAppStation->State() == PRESTART_Q ||
			pAppStation->State() == DEMO_Q ||
			pAppStation->State() == AUTO_Q ||
			pAppStation->State() == STOPPING_Q
			)
		{
			bResult = FALSE;
			break;
		}
	}

	return bResult;
}

BOOL CAC9000App::CheckFileSize(CString &szInFilePath, CString &szInFileName, LONG lFileSizeLimitInKb)
{
	//Rename the file if the file size is larger than the lFileSizeLimit
	//The new filename will append the current date and time at the end.
	BOOL bFileOp = FALSE;
	CFileOperation fo;

	FILE *fp = fopen(szInFilePath + szInFileName, "r");
	if (fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		if (ftell(fp) > lFileSizeLimitInKb * 1024)
		{
			bFileOp = TRUE;
		}
		fclose(fp);
	}

	if (bFileOp)
	{
		INT nPos = 0;
		CString szFileName("FileName"), szExt("ext");
		nPos = szInFileName.Find(".");
		szFileName = szInFileName.Left(nPos);
		szExt = szInFileName.Right(szInFileName.GetLength() - nPos - 1);

		CString szTime = _T(""), szDestFilePath = _T("");
		char tmpbuf[110];
		__time64_t ltime;
		struct tm *today;

		_time64(&ltime);
		today = _localtime64(&ltime);
		memset(&tmpbuf[0], 0 , 110);
		strftime(tmpbuf, 109, "_%Y_%m_%d_%H_%M_%S.", today);
		szDestFilePath = szInFilePath + szFileName + CString(tmpbuf) + szExt;
		szTime = tmpbuf;

		//bOpDone = fo.Copy(szInFilePath + szInFileName, szInFilePath);
		if (fo.Rename(szInFilePath + szInFileName, szDestFilePath))
		{
			return TRUE;
		}
	}

	return FALSE;
}

//BOOL CAC9000App::CalcWHZLinearEncoder(CAC9000Stn* pStation, MOTOR_PROTECTION stProtect, WHZ_DATA* pResult) //whz rotary linear calib
//{	
//
//	CString szMsg = "";
//
//	WHZ_DATA	stWhZ_temp[MAX_NUM_WHZ_CAL_STEP];
//	INT			i, j, k;
//	DOUBLE		WHZ_stepsize;
//	LONG		LinearSample[NUM_OF_SAMPLE_DATA];
//	LONG		RotarySample[NUM_OF_SAMPLE_DATA];	// WHZ motor
//	LONG		SumEncoderData;
//
//	memset(&LinearSample[0], 0, sizeof(LONG) * NUM_OF_SAMPLE_DATA);
//	memset(&RotarySample[0], 0, sizeof(LONG) * NUM_OF_SAMPLE_DATA);
//	//if(lSWLimitLevel[DOWN] < 500)
//	//{
//	//	return FALSE;
//	//}
//
//	if (
//		m_CurrMotor == "INWH_Z1"	||
//		m_CurrMotor == "ACF1WH_Z"	||
//		m_CurrMotor == "ACF2WH_Z"	||
//		m_CurrMotor == "PBWH_Z1"	||
//		m_CurrMotor == "MB1WH_Z"	||
//		m_CurrMotor == "MB2WH_Z"	||
//		m_CurrMotor == "MB3WH_Z"	||
//		m_CurrMotor == "MB4WH_Z"
//	   )
//	{
//		if (pStation->GetMotorZ(WHZ_1).MoveAbsolute(LONG(stProtect.lPosLmt + 500), SFM_WAIT) != gnAMS_OK)
//		{
//			return FALSE;
//		}
//	}
//	else if (
//			 m_CurrMotor == "INWH_Z2"	||
//			 m_CurrMotor == "PBWH_Z2"
//			)
//	{
//		if (pStation->GetMotorZ(WHZ_2).MoveAbsolute(LONG(stProtect.lPosLmt + 500), SFM_WAIT) != gnAMS_OK)
//		{
//			return FALSE;
//		}
//	}
//
//	//else
//	//{
//	//	Sleep(500);
//
//	//	if(!pStation->IsOfflineMode())
//	//	{
//	//		try
//	//		{
//
//	//			// Reset ACF Rollout Encoder zero
//	//			pCACF->m_pRollOut->SetPosition(0);
//	//		}
//	//		catch (CAsmException e)
//	//		{
//	//			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//	//			DisplayException(e);
//	//			szMsg.Format("    AXIS: %s", pCACF->m_stMotorRollOut.stAttrib.szAxis);
//	//			DisplayMessage(szMsg);
//	//			pCACF->CheckAxisError(pCACF->m_stMotorRollOut.stAttrib.szAxis);
//	//			DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//	//		}
//	//	}
//	//}
//
//	for (i = 0; i < MAX_NUM_WHZ_CAL_STEP; i++) // backup data q
//	{
//		stWhZ_temp[i] = pResult[i];
//	}
//
//	WHZ_stepsize = (DOUBLE)((stProtect.lNegLmt - 500) - (stProtect.lPosLmt + 500)) / (DOUBLE)(MAX_NUM_WHZ_CAL_STEP);
//	if (
//		m_CurrMotor == "INWH_Z1"	||
//		m_CurrMotor == "ACF1WH_Z"	||
//		m_CurrMotor == "ACF2WH_Z"	||
//		m_CurrMotor == "PBWH_Z1"	||
//		m_CurrMotor == "MB1WH_Z"	||
//		m_CurrMotor == "MB2WH_Z"	||
//		m_CurrMotor == "MB3WH_Z"	||
//		m_CurrMotor == "MB4WH_Z"
//	   )
//	{
//		pResult[0].RotaryPosn = pStation->GetMotorZ(WHZ_1).GetEncPosn();
//
//	}
//	else if (
//			 m_CurrMotor == "INWH_Z2"	||
//			 m_CurrMotor == "PBWH_Z2"
//			)
//	{
//		pResult[0].RotaryPosn = pStation->GetMotorZ(WHZ_2).GetEncPosn();
//
//	}
//	pResult[0].LinearPosn = -1 * pCHouseKeeping->m_stMotorEncoderOnlyTmp1.GetEncPosn();
//
//	for (i = 1; i < MAX_NUM_WHZ_CAL_STEP; i++) //get linear Encoder data
//	{	
//		if (
//			m_CurrMotor == "INWH_Z1"	||
//			m_CurrMotor == "ACF1WH_Z"	||
//			m_CurrMotor == "ACF2WH_Z"	||
//			m_CurrMotor == "PBWH_Z1"	||
//			m_CurrMotor == "MB1WH_Z"	||
//			m_CurrMotor == "MB2WH_Z"	||
//			m_CurrMotor == "MB3WH_Z"	||
//			m_CurrMotor == "MB4WH_Z"
//		   )
//		{
//			if (pStation->GetMotorZ(WHZ_1).MoveAbsolute(LONG(stProtect.lPosLmt + (WHZ_stepsize * i)), SFM_WAIT) == gnAMS_OK)
//			{
//				Sleep(300);
//			}
//			else // WHZ move error
//			{
//				for (k = 0; k < MAX_NUM_WHZ_CAL_STEP; k++) // restore data q
//				{
//					pResult[k] = stWhZ_temp[k];
//				}
//
//				return FALSE;
//			}
//		}
//		else if (
//				 m_CurrMotor == "INWH_Z2"	||
//				 m_CurrMotor == "PBWH_Z2"
//				)
//		{
//			if (pStation->GetMotorZ(WHZ_2).MoveAbsolute(LONG(stProtect.lPosLmt + (WHZ_stepsize * i)), SFM_WAIT) == gnAMS_OK)
//			{
//				Sleep(300);
//			}
//			else // WHZ move error
//			{
//				for (k = 0; k < MAX_NUM_WHZ_CAL_STEP; k++) // restore data q
//				{
//					pResult[k] = stWhZ_temp[k];
//				}
//
//				return FALSE;
//			}
//		}
//
//		if (
//			m_CurrMotor == "INWH_Z1"	||
//			m_CurrMotor == "ACF1WH_Z"	||
//			m_CurrMotor == "ACF2WH_Z"	||
//			m_CurrMotor == "PBWH_Z1"	||
//			m_CurrMotor == "MB1WH_Z"	||
//			m_CurrMotor == "MB2WH_Z"	||
//			m_CurrMotor == "MB3WH_Z"	||
//			m_CurrMotor == "MB4WH_Z"
//		   )
//		{
//			for (j = 0; j < NUM_OF_SAMPLE_DATA; j++) // sample data
//			{
//				RotarySample[j] = pStation->GetMotorZ(WHZ_1).GetEncPosn();
//				LinearSample[j] = -1 * pCHouseKeeping->m_stMotorEncoderOnlyTmp1.GetEncPosn();
//				Sleep(200);
//			}
//		}
//		else if (
//				 m_CurrMotor == "INWH_Z2"	||
//				 m_CurrMotor == "PBWH_Z2"
//				)
//		{
//			for (j = 0; j < NUM_OF_SAMPLE_DATA; j++) // sample data
//			{
//				RotarySample[j] = pStation->GetMotorZ(WHZ_2).GetEncPosn();
//				LinearSample[j] = -1 * pCHouseKeeping->m_stMotorEncoderOnlyTmp1.GetEncPosn();
//				Sleep(200);
//			}
//		}
//
//
//
//		SumEncoderData = 0; //reset
//
//		for (j = 0; j < NUM_OF_SAMPLE_DATA; j++)
//		{
//			SumEncoderData += RotarySample[j];
//		}
//		pResult[i].RotaryPosn = (LONG)(SumEncoderData / NUM_OF_SAMPLE_DATA);
//
//		SumEncoderData = 0; //reset
//
//		for (j = 0; j < NUM_OF_SAMPLE_DATA; j++)
//		{
//			SumEncoderData += LinearSample[j];
//		}
//		pResult[i].LinearPosn = (LONG)(SumEncoderData / NUM_OF_SAMPLE_DATA);		
//	}
//
//	return TRUE;
//}

LONG CAC9000App::LoadGlassReqGatewayOperation(GlassNum lGlass)
{
	//BOOL nResult = gnAMS_NOTOK;
	//CInSHWH *pCInSHWH = (CInSHWH*)m_pModule->GetStation("InSHWH");

	//if (!pCInSHWH->m_bModSelected)
	//{
	//	DisplayMessage("PICK GLASS OPERATION: InSHWH Module Not Selected. Operation Abort!");
	//	return GMP_FAIL;
	//}
	//if (lGlass == GLASS1 && pCInSHWH->IsLeftVacSnrOn())
	//{
	//	HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Please Remove Glass On InSHWH, and retry again");
	//	return GMP_FAIL;
	//}
	//else if (lGlass == GLASS2 && pCInSHWH->IsRightVacSnrOn())
	//{
	//	HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Please Remove Glass On InSHWH, and retry again");
	//	return GMP_FAIL;
	//}
	//else if (lGlass == MAX_NUM_OF_GLASS && 
	//		 (pCInSHWH->IsLeftVacSnrOn() || pCInSHWH->IsRightVacSnrOn())	
	//		)
	//{
	//	HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Please Remove Glass On InSHWH, and retry again");
	//	return GMP_FAIL;
	//}

	//if (
	//	pCInSHWH->SetPitchExtendSol(OFF, GMP_WAIT) != GMP_SUCCESS ||
	//	pCInSHWH->MoveZ1Z2(INSH_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT) != GMP_SUCCESS ||
	//	pCInSHWH->MoveXY(INSH_WH_XY_CTRL_GO_LOAD_POSN, GMP_WAIT) != GMP_SUCCESS
	//   )
	//{
	//	return GMP_FAIL;
	//}

	//if (lGlass == GLASS1)
	//{
	////Load Req Glass1
	//	// 20140918 Yip: Correct From INSH_WH_Z_CTRL_GO_LOAD_LEVEL To INSH_WH_Z_CTRL_GO_LOAD_LEVEL_WITH_OFFSET
	//	if (
	//		pCInSHWH->MoveZ(INSH_WH_Z_CTRL_GO_LOAD_LEVEL_WITH_OFFSET, WHZ_1, GMP_WAIT) != GMP_SUCCESS	||
	//		pCInSHWH->SetLeftVacSol(ON, GMP_WAIT) != GMP_SUCCESS
	//	   )
	//	{
	//		return GMP_FAIL;
	//	}

	//}
	//else if (lGlass == GLASS2)
	//{
	////Load Req Glass2
	//	// 20140918 Yip: Correct From INSH_WH_Z_CTRL_GO_LOAD_LEVEL To INSH_WH_Z_CTRL_GO_LOAD_LEVEL_WITH_OFFSET
	//	if (
	//		pCInSHWH->MoveZ(INSH_WH_Z_CTRL_GO_LOAD_LEVEL_WITH_OFFSET, WHZ_2, GMP_WAIT) != GMP_SUCCESS	||
	//		pCInSHWH->SetRightVacSol(ON, GMP_WAIT) != GMP_SUCCESS
	//	   )
	//	{
	//		return GMP_FAIL;
	//	}
	//}
	//else if (lGlass == MAX_NUM_OF_GLASS)
	//{
	////Load Req G1G2
	//	// 20140918 Yip: Correct From INSH_WH_Z_CTRL_GO_LOAD_LEVEL To INSH_WH_Z_CTRL_GO_LOAD_LEVEL_WITH_OFFSET
	//	if (
	//		pCInSHWH->MoveZ1Z2(INSH_WH_Z_CTRL_GO_LOAD_LEVEL_WITH_OFFSET, GMP_WAIT) != GMP_SUCCESS ||
	//		pCInSHWH->SetLeftVacSol(ON, GMP_WAIT) != GMP_SUCCESS ||
	//		pCInSHWH->SetRightVacSol(ON, GMP_WAIT) != GMP_SUCCESS
	//	   )
	//	{
	//		return GMP_FAIL;
	//	}
	//}

	return GMP_SUCCESS;
}

LONG CAC9000App::LoadGlassGatewayOperation(GlassNum lGlass)
{
	////CInSHWH *pCInSHWH = (CInSHWH*)m_pModule->GetStation("InSHWH");

	////if (!pCInSHWH->m_bModSelected)
	//{
	//	DisplayMessage("PICK GLASS OPERATION: InSHWH Module Not Selected. Operation Abort!");
	//	return GMP_FAIL;
	//}

	////pCInSHWH->MoveZ1Z2(INSH_WH_Z_CTRL_GO_STANDBY_LEVEL, GMP_WAIT);

	////if (lGlass == GLASS1 && !pCInSHWH->IsLeftVacSnrOn())
	//{
	//	HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Load Glass to InSHWH FAIL!! Retry again");
	//	return GMP_FAIL;
	//}
	////else if (lGlass == GLASS2 && !pCInSHWH->IsRightVacSnrOn())
	//{
	//	HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Load Glass to InSHWH FAIL!! Retry again");
	//	return GMP_FAIL;
	//}
	//else if (lGlass == MAX_NUM_OF_GLASS /*&& */
	//		 //(!pCInSHWH->IsLeftVacSnrOn() || !pCInSHWH->IsRightVacSnrOn())	
	//		)
	//{
	//	HMIMessageBox(MSG_OK, "PICK GLASS OPERATION", "Load Glass to InSHWH FAIL!! Retry again");
	//	return GMP_FAIL;
	//}

	////pCInSHWH->SetPitchExtendSol(OFF);
	////pCInSHWH->MoveXY(INSH_WH_XY_CTRL_GO_STANDBY_POSN, GMP_NOWAIT);

	return GMP_SUCCESS;
}

DWORD CAC9000App::DeleteOldFiles(CString &szInFilePath, CString &szWildCard, LONG lDay) //20141210
{
	//Note: szInFilePath + szWildCard should equal a full path eg D:\sw\COG902\LogFileDir\*.mlg
	//Delete all file pass the file filter and older that the lDay
	const ULONGLONG minutesPerTick = 600000000;
	CString szPath(" "), szMsg(" ");
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH] = "";
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	FILETIME ftWriteTime, ftLimitTime, ftSysTime;
	SYSTEMTIME sysTime;

	if (lDay < 0)
	{
		return -1;
	}

	szPath = szInFilePath + szWildCard;
	strcpy(szDir, szPath.GetString());
	DisplayMessage(szPath);

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		//DisplayMessage("FindFirstFile Error.");
		return -1;
	}

	// List all the files in the directory with some info about them.

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//szMsg.Format("<DIR>:%s\n", ffd.cFileName);
			//DisplayMessage(szMsg);
			;
		}
		else
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			//szMsg.Format("file :%s, Size:%ld bytes\n", ffd.cFileName, filesize.QuadPart);
			//DisplayMessage(szMsg);

			ftWriteTime.dwHighDateTime = ffd.ftLastWriteTime.dwHighDateTime;
			ftWriteTime.dwLowDateTime = ffd.ftLastWriteTime.dwLowDateTime;
			FileTimeToSystemTime(&ftWriteTime, &sysTime);
			//szMsg.Format("WriteDateTime %d-%d-%d %d %02d:%02d:%02d\n", sysTime.wYear, sysTime.wMonth, sysTime.wDay,
			//	sysTime.wDayOfWeek, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
			//DisplayMessage(szMsg);

			ULARGE_INTEGER uli1, uli2;
			GetSystemTimeAsFileTime(&ftSysTime);
			uli1.HighPart = ftSysTime.dwHighDateTime;
			uli1.LowPart = ftSysTime.dwLowDateTime;

			uli2.QuadPart = lDay * 24 * 60 * minutesPerTick;
			uli1.QuadPart -= uli2.QuadPart;

			// Convert back to FILETIME
			ftLimitTime.dwHighDateTime = uli1.HighPart;
			ftLimitTime.dwLowDateTime = uli1.LowPart;

			LONG lResult = 0;
			lResult = CompareFileTime(&ftLimitTime, &ftWriteTime);
			//szMsg.Format("Compare %ld\n", lResult);
			//DisplayMessage(szMsg);
			if (lResult == 1)
			{
				TCHAR pStrDeleteFile[MAX_PATH] = "";
				sprintf(pStrDeleteFile, "%s%s", (char*)szInFilePath.GetBuffer(0), ffd.cFileName);
				szMsg.Format("Delete:%s\n", pStrDeleteFile);
				DisplayMessage(szMsg);
				DeleteFile(pStrDeleteFile);
			}

		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		DisplayMessage("FindFirstFile error");
	}

	FindClose(hFind);
	return dwError;
}

BOOL CAC9000App::GetHDDFreeSpace(ULONG &ulFreeSpaceMB_C, ULONG &ulFreeSpaceMB_D) //20150810
{
	ULONG ulFreeSpaceMB = 0;
	struct _diskfree_t df = {0};
	BOOL bResult = TRUE;

	if (_getdiskfree((INT)DRIVE_D, &df) == 0)
	{
		ulFreeSpaceMB = df.avail_clusters * df.sectors_per_cluster / 1024 * df.bytes_per_sector / 1024;
		ulFreeSpaceMB_D = ulFreeSpaceMB;
	}
	else
	{
		bResult = FALSE;
	}

	if (_getdiskfree((INT)DRIVE_C, &df) == 0)
	{
		ulFreeSpaceMB = df.avail_clusters * df.sectors_per_cluster / 1024 * df.bytes_per_sector / 1024;
		ulFreeSpaceMB_C = ulFreeSpaceMB;
	}
	else
	{
		bResult = FALSE;
	}
	return bResult;
}

#ifdef EXCEL_OFFLINE_SETUP
INT CAC9000App::LoadExcelSetupInfo(CString &szExcelPath)
{
	BasicExcel excelDB;
	//CString szPath = _T("D:\\sw\\AC9000\\GlassInfo\\AC9000_GlassInfo.xls");
	BOOL bError = FALSE;

#ifdef _DEBUG
	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
#endif

	DisplayMessage("Loading Glass Info...");

	if (excelDB.Load(szExcelPath))
	{
		////////////////
		if (ExcelLoadGeneralSheet(excelDB) != gnAMS_OK)
		{
			DisplayMessage("Load General Error");
			bError = TRUE;
		}
		else if (ExcelLoadINWHSheet(excelDB) != gnAMS_OK)
		{
			DisplayMessage("Load INWH Error");
			bError = TRUE;
		}
		else if (ExcelLoadPBWHSheet(excelDB) != gnAMS_OK)
		{
			DisplayMessage("Load PBWH Error");
			bError = TRUE;
		}
		////////////////
		excelDB.Close();
		DisplayMessage("Glass Info Loaded");
	}
	else
	{
		DisplayMessage("Excel Load Fail");
		bError = TRUE;
	}

#ifdef _DEBUG
	newMemState.Checkpoint();
	if( diffMemState.Difference( oldMemState, newMemState ) )
	{
		TRACE( "Memory leaked!\n" );
	}
#endif

	if (bError)
	{
		return gnAMS_NOTOK;
	}
	else
	{
		return gnAMS_OK;
	}
}

INT CAC9000App::SetOfflineSetupGeneral()
{
	BOOL bResult = TRUE;

	if (
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_GLASS_WIDTH, g_stOfflineInfo.stGeneralInfo.dGlassWidth) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_GLASS_LENGTH, g_stOfflineInfo.stGeneralInfo.dGlassLength) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_GLASS_ITOWIDTH, g_stOfflineInfo.stGeneralInfo.dITOWidth) == gnAMS_OK
		)
	{
		g_stGlass.dWidth = g_stOfflineInfo.stGeneralInfo.dGlassWidth;
		g_stGlass.dLength = g_stOfflineInfo.stGeneralInfo.dGlassLength;
		g_stGlass.dItoWidth = g_stOfflineInfo.stGeneralInfo.dITOWidth;
	}
	else
	{
		bResult = FALSE;
	}

	if (
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_GLASS_REFLECTOR_THICKNESS_MM, g_stOfflineInfo.stGeneralInfo.dReflectorThickness) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_GLASS_UPPER_GLASS_THICKNESS_MM, g_stOfflineInfo.stGeneralInfo.dUpperGlassThickness) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_GLASS_LOWER_GLASS_THICKNESS_MM, g_stOfflineInfo.stGeneralInfo.dLowerGlassThickness) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_GLASS_POLARIZER_THICKNESS_MM, g_stOfflineInfo.stGeneralInfo.dPolarizerThickness) == gnAMS_OK
		)
	{
		g_stGlass.dThickness1 = g_stOfflineInfo.stGeneralInfo.dReflectorThickness;
		g_stGlass.dThickness2 = g_stOfflineInfo.stGeneralInfo.dUpperGlassThickness;
		g_stGlass.dThickness3 = g_stOfflineInfo.stGeneralInfo.dLowerGlassThickness;
		g_stGlass.dThickness4 = g_stOfflineInfo.stGeneralInfo.dPolarizerThickness;
	}
	else
	{
		bResult = FALSE;
	}

	if (
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_ACF_LENGTH_MM, g_stOfflineInfo.stGeneralInfo.dACFLength) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_ACF_WIDTH_MM, g_stOfflineInfo.stGeneralInfo.dACFWidth) == gnAMS_OK
		)
	{
		//////////// set acf width
#ifdef ACF_POSN_CALIB //20120829
		DOUBLE dWidthDiff;

		dWidthDiff = g_stOfflineInfo.stGeneralInfo.dACFWidth - pCACF1WH->m_stACF[ACF1].dWidth;
		//shift y-offset

		for (INT i = ACF1; i < MAX_NUM_OF_ACF; i++)
		{
			pCACF1WH->m_lBondOffsetY[i] -= DoubleToLong(dWidthDiff / 2.0 * 1000.0);
			pCACF2WH->m_lBondOffsetY[i] -= DoubleToLong(dWidthDiff / 2.0 * 1000.0);
		}
		pCACF1WH->HMI_lBondOffsetY = pCACF1WH->m_lBondOffsetY[ACF1];
		pCACF2WH->HMI_lBondOffsetY = pCACF2WH->m_lBondOffsetY[ACF1];
#endif	

		for (INT i = ACF1; i < MAX_NUM_OF_ACF; i++)
		{
			pCACF1WH->m_stACF[i].dWidth = g_stOfflineInfo.stGeneralInfo.dACFWidth;
			pCACF2WH->m_stACF[i].dWidth = g_stOfflineInfo.stGeneralInfo.dACFWidth;
		}

		pCHouseKeeping->HMI_dACFWidth = pCACF1WH->m_stACF[ACF1].dWidth;
		//////////// set acf width

		//////////// set acf length start
#ifdef ACF_POSN_CALIB //20120829
		//acf length increase, m_lLoadXOffset decrease
		DOUBLE dLengthDiff;

		dLengthDiff = g_stOfflineInfo.stGeneralInfo.dACFLength - pCACF1WH->m_stACF[ACF1].dLength;

		//shift xoffset
		pCACF1WH->m_lLoadXOffset -= DoubleToLong(dLengthDiff / 2.0 * 1000.0);
		pCACF2WH->m_lLoadXOffset -= DoubleToLong(dLengthDiff / 2.0 * 1000.0);

#endif	

		for (INT i = ACF1; i < MAX_NUM_OF_ACF; i++)
		{
			pCACF1WH->m_stACF[i].dLength = g_stOfflineInfo.stGeneralInfo.dACFLength;
			pCACF2WH->m_stACF[i].dLength = g_stOfflineInfo.stGeneralInfo.dACFLength;
		}

		pCHouseKeeping->HMI_dACFLength = pCACF1WH->m_stACF[ACF1].dLength;
		//////////// set acf length end
	}
	else
	{
		bResult = FALSE;
	}

	if (
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_COF_LENGTH_MM, g_stOfflineInfo.stGeneralInfo.dCOFLength) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_COF_WIDTH_MM, g_stOfflineInfo.stGeneralInfo.dCOFWidth) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_COF_THICKNESS_MM, g_stOfflineInfo.stGeneralInfo.dCOFThickness) == gnAMS_OK
		)
	{
		 g_stLSIType[LSI_TYPE1].dLength = g_stOfflineInfo.stGeneralInfo.dCOFLength;
		 g_stLSIType[LSI_TYPE1].dWidth = g_stOfflineInfo.stGeneralInfo.dCOFWidth;
		 g_stLSIType[LSI_TYPE1].dThickness = g_stOfflineInfo.stGeneralInfo.dCOFThickness;
	}
	else
	{
		bResult = FALSE;
	}

	if (
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_TA1_PICKPLACE_RETRY_LMT, g_stOfflineInfo.stGeneralInfo.dTA1PickPlaceRetryLimit) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_TA2_PICKPLACE_RETRY_LMT, g_stOfflineInfo.stGeneralInfo.dTA2PickPlaceRetryLimit) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_TA3_PICKPLACE_RETRY_LMT, g_stOfflineInfo.stGeneralInfo.dTA3PickPlaceRetryLimit) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_TA4_PICKPLACE_RETRY_LMT, g_stOfflineInfo.stGeneralInfo.dTA4PickPlaceRetryLimit) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_TA5_PICKPLACE_RETRY_LMT, g_stOfflineInfo.stGeneralInfo.dTA5PickPlaceRetryLimit) == gnAMS_OK
		)
	{
		pCTA1->m_lPickPlaceRetryLimit	  = (LONG)g_stOfflineInfo.stGeneralInfo.dTA1PickPlaceRetryLimit;
		pCTA2->m_lPickPlaceRetryLimit	  = (LONG)g_stOfflineInfo.stGeneralInfo.dTA2PickPlaceRetryLimit;
		//pCTA3->m_lPickPlaceRetryLimit	  = (LONG)g_stOfflineInfo.stGeneralInfo.dTA3PickPlaceRetryLimit;
		//pCTA4->m_lPickPlaceRetryLimit	  = (LONG)g_stOfflineInfo.stGeneralInfo.dTA4PickPlaceRetryLimit;
		//pCTA5->m_lPickPlaceRetryLimit	  = (LONG)g_stOfflineInfo.stGeneralInfo.dTA5PickPlaceRetryLimit;
	}
	else
	{
		bResult = FALSE;
	}

	if (
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_TA4_RR_RETRY_LMT, g_stOfflineInfo.stGeneralInfo.dTA4PRRetryLimit) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_TA4_REJECT_LMT, g_stOfflineInfo.stGeneralInfo.dTA4RejectLimit) == gnAMS_OK
		)
	{
		//pCTA4->m_lPRRetryLimit = (LONG)g_stOfflineInfo.stGeneralInfo.dTA4PRRetryLimit;
		//pCTA4->m_lRejectGlassLimit = (LONG)g_stOfflineInfo.stGeneralInfo.dTA4RejectLimit;
	}
	else
	{
		bResult = FALSE;
	}

	if (
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_INWH_PR_RETRY_LMT, g_stOfflineInfo.stGeneralInfo.dINWHPRRetryLimit) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_INWH_REJECT_LMT, g_stOfflineInfo.stGeneralInfo.dINWHRejectLimit) == gnAMS_OK
		)
	{
		//pCINWH->m_lPRRetryLimit = (LONG)g_stOfflineInfo.stGeneralInfo.dINWHPRRetryLimit;
		//pCINWH->m_lRejectGlassLimit = (LONG)g_stOfflineInfo.stGeneralInfo.dINWHRejectLimit;
	}
	else
	{
		bResult = FALSE;
	}

	if (
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_ACF_INSP_RETRY_LMT, g_stOfflineInfo.stGeneralInfo.dACFInspRetryLimit) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_ACF_INSP_REJECT_LMT, g_stOfflineInfo.stGeneralInfo.dACFInspRejectLimit) == gnAMS_OK
		)
	{
		//pCInsp1->m_lPRRetryLimit = (LONG)g_stOfflineInfo.stGeneralInfo.dACFInspRetryLimit;
		//pCInsp2->m_lPRRetryLimit = (LONG)g_stOfflineInfo.stGeneralInfo.dACFInspRetryLimit;

		//pCInsp1->m_lRejectGlassLimit = (LONG)g_stOfflineInfo.stGeneralInfo.dACFInspRejectLimit;
		//pCInsp2->m_lRejectGlassLimit = (LONG)g_stOfflineInfo.stGeneralInfo.dACFInspRejectLimit;
	}
	else
	{
		bResult = FALSE;
	}

	if (bResult)
	{
		return gnAMS_OK;
	}
	else
	{
		return gnAMS_NOTOK;
	}
}

INT CAC9000App::SetOfflineSetupINWH()
{
	BOOL bResult = TRUE;

	if (
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_INWH_PR1_X_POSN_UM, g_stOfflineInfo.stINWHInfo.dPR1PosnX) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_INWH_PR1_Y_POSN_UM, g_stOfflineInfo.stINWHInfo.dPR1PosnY) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_INWH_PR2_X_POSN_UM, g_stOfflineInfo.stINWHInfo.dPR2PosnX) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_INWH_PR2_Y_POSN_UM, g_stOfflineInfo.stINWHInfo.dPR2PosnY) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_INWH_AP1_X_POSN_UM, g_stOfflineInfo.stINWHInfo.dAP1PosnX) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_INWH_AP1_Y_POSN_UM, g_stOfflineInfo.stINWHInfo.dAP1PosnY) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_INWH_AP2_X_POSN_UM, g_stOfflineInfo.stINWHInfo.dAP2PosnX) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_INWH_AP2_Y_POSN_UM, g_stOfflineInfo.stINWHInfo.dAP2PosnY) == gnAMS_OK
		)
	{
		g_stOfflineInfo.stINWHInfo.bDataValid = TRUE;
	}
	else
	{
		g_stOfflineInfo.stINWHInfo.bDataValid = FALSE;
		bResult = FALSE;
	}

	if (bResult)
	{
		return gnAMS_OK;
	}
	else
	{
		return gnAMS_NOTOK;
	}
}

INT CAC9000App::SetOfflineSetupPBWH()
{
	BOOL bResult = TRUE;

	if (
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_PBWH_PR1_X_POSN_UM, g_stOfflineInfo.stPBWHInfo.dPR1PosnX) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_PBWH_PR1_Y_POSN_UM, g_stOfflineInfo.stPBWHInfo.dPR1PosnY) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_PBWH_PR2_X_POSN_UM, g_stOfflineInfo.stPBWHInfo.dPR2PosnX) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_PBWH_PR2_Y_POSN_UM, g_stOfflineInfo.stPBWHInfo.dPR2PosnY) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_PBWH_AP1_X_POSN_UM, g_stOfflineInfo.stPBWHInfo.dAP1PosnX) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_PBWH_AP1_Y_POSN_UM, g_stOfflineInfo.stPBWHInfo.dAP1PosnY) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_PBWH_AP2_X_POSN_UM, g_stOfflineInfo.stPBWHInfo.dAP2PosnX) == gnAMS_OK &&
		pCHouseKeeping->OfflineSetupCheckInputValid(OFFLINE_PBWH_AP2_Y_POSN_UM, g_stOfflineInfo.stPBWHInfo.dAP2PosnY) == gnAMS_OK
		)
	{
		g_stOfflineInfo.stPBWHInfo.bDataValid = TRUE;
	}
	else
	{
		g_stOfflineInfo.stPBWHInfo.bDataValid = FALSE;
		bResult = FALSE;
	}

	if (bResult)
	{
		return gnAMS_OK;
	}
	else
	{
		return gnAMS_NOTOK;
	}
}
#endif

BOOL CAC9000App::SetHeapInfo() //20151005
{
	BOOL bReturn = FALSE;

#if 0
	intptr_t hCrtHeap = _get_heap_handle();
    ULONG ulEnableLFH = 2;
	if (hCrtHeap != NULL)
	{
		bReturn = HeapSetInformation((PVOID)hCrtHeap, HeapCompatibilityInformation, &ulEnableLFH, sizeof(ulEnableLFH));
	}
	return bReturn;

#elif 0
	ULONG HeapFragValue = 2;
	HANDLE hHeap = NULL;
	DWORD lastError = 0;

	hHeap = GetProcessHeap();
	if (hHeap != NULL)
	{
		unsigned char bBuffer[128] = {0, };
		size_t size = 0;
		bReturn = HeapQueryInformation(hHeap, HeapCompatibilityInformation, &bBuffer[0], 128, NULL);

		bReturn = HeapSetInformation(/*_get_heap_handle()*/hHeap, HeapCompatibilityInformation, &HeapFragValue, sizeof(HeapFragValue));
		if (!bReturn)
		{
			LPVOID lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
				);
			// Process any inserts in lpMsgBuf.
			// ...
			// Display the string.
			MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
			// Free the buffer.
		}
	}
	return bReturn;
#else
	size_t size = 0;
	size = _get_sbh_threshold();
	bReturn = _set_sbh_threshold(1016); //1024 - 8
	size = _get_sbh_threshold();
	return bReturn;
#endif
}

INT CAC9000App::LogMemoryStatus()
{
	HANDLE hProcessSnap = NULL;
	HANDLE hProcess = NULL;
	PROCESSENTRY32 pe32 = {0};
	INT nVisionNTCount	= 0;
	INT nAsmHmiCount	= 0;
	INT nGatewayCount	= 0;
	BOOL bResult = TRUE;
	CString szDirectory(_T("D:\\sw\\AC9000\\Data\\"));
	static FILETIME ftPrevSysTime_COF = {0,}, ftPrevProcKernel_COF = {0,}, ftPrevProcUser_COF = {0,};
	static FILETIME ftPrevSysTime_HMI = {0,}, ftPrevProcKernel_HMI = {0,}, ftPrevProcUser_HMI = {0,};
	static FILETIME ftPrevSysTime_VIS = {0,}, ftPrevProcKernel_VIS = {0,}, ftPrevProcUser_VIS = {0,};

	// Take a snapshot of all processes in the system. 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{

		// Fill in the size of the structure before using it. 
		pe32.dwSize = sizeof(PROCESSENTRY32);

		// Walk the snapshot of the processes, and for each process, display information.
		if (Process32First(hProcessSnap, &pe32))
		{
			do
			{
				CString strName = pe32.szExeFile;
				CString szPath;
				CString szFileNamePrefix(_T("MemoryUsageLog"));
				CString szFileNameSuffix(_T(".csv"));

				strName = strName.MakeUpper();
				szPath = szDirectory;
				if (
					strName.Find("VISIONNT.EXE") >= 0 ||
					strName.Find("ASMHMI.EXE") >= 0 ||
					strName.Find("AC9000.EXE") >= 0 ||
					strName.Find("SOCKETGATEWAY.EXE") >= 0
					)
				{
					PROCESS_MEMORY_COUNTERS stProcessMemoryCnt;
					DOUBLE dCpuUsage = 0.0;

					INT nFind = strName.ReverseFind('.');
					szPath = szPath + szFileNamePrefix + strName.Left(nFind) + szFileNameSuffix;

					hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);

					if (hProcess != NULL)
					{
						if (strName.Find("VISIONNT.EXE") >= 0)
						{
							GetCpuUsage(hProcess, dCpuUsage, ftPrevSysTime_VIS, ftPrevProcKernel_VIS, ftPrevProcUser_VIS);
						}
						else if (strName.Find("ASMHMI.EXE") >= 0)
						{
							GetCpuUsage(hProcess, dCpuUsage, ftPrevSysTime_HMI, ftPrevProcKernel_HMI, ftPrevProcUser_HMI);
						}
						else if (strName.Find("AC9000.EXE") >= 0)
						{
							GetCpuUsage(hProcess, dCpuUsage, ftPrevSysTime_COF, ftPrevProcKernel_COF, ftPrevProcUser_COF);
						}

						if (
							GetProcessMemoryInfo(hProcess, &stProcessMemoryCnt, sizeof(stProcessMemoryCnt))
							)
						{
							CString szInFileName2("");
							szInFileName2 = szFileNamePrefix + strName.Left(nFind) + szFileNameSuffix;
							CheckFileSize(szDirectory, szInFileName2, 512); //512KB

							FILE *fp = NULL;
							fp = fopen((char *)szPath.GetBuffer(0), "a+");
							if (fp != NULL)
							{
								CString szTime(" ");
								char tmpbuf[129];
								struct tm *today;
								__time64_t ltime;

								_time64(&ltime);
								today = _localtime64(&ltime);
								memset(&tmpbuf[0], 0 , 129);
								strftime(tmpbuf, 128, "%Y-%m-%d %H:%M:%S", today);
								szTime.Format("%s", tmpbuf);

								fseek(fp, 0, SEEK_END);
								if (ftell(fp) < 10)
								{//print header
									fprintf(fp, "Time, Title, PageFaultCount, PeakWorkingSetSize KB, WorkingSetSize KB, QuotaPeakPagedPoolUsage KB, QuotaPagedPoolUsage KB, QuotaPeakNonPagedPoolUsage KB, QuotaNonPagedPoolUsage KB, PagefileUsage KB, PeakPagefileUsage KB, CPU Percentage\n");
								}
								fprintf(fp, "%s, %s, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %.5f\n",
									(char *)szTime.GetBuffer(0),
									(char *)strName.GetBuffer(0),
									(ULONG)stProcessMemoryCnt.PageFaultCount,
									(ULONG)stProcessMemoryCnt.PeakWorkingSetSize / 1024,
									(ULONG)stProcessMemoryCnt.WorkingSetSize / 1024,
									(ULONG)stProcessMemoryCnt.QuotaPeakPagedPoolUsage / 1024,
									(ULONG)stProcessMemoryCnt.QuotaPagedPoolUsage / 1024,
									(ULONG)stProcessMemoryCnt.QuotaPeakNonPagedPoolUsage / 1024,
									(ULONG)stProcessMemoryCnt.QuotaNonPagedPoolUsage /1024,
									(ULONG)stProcessMemoryCnt.PagefileUsage /1024,
									(ULONG)stProcessMemoryCnt.PeakPagefileUsage / 1024,
									dCpuUsage	);
								fclose(fp);
							}
						}
						CloseHandle(hProcess);
					}
				}
			} while (Process32Next(hProcessSnap, &pe32));
		}
	}
	CloseHandle(hProcessSnap);

	return 0;
}

INT CAC9000App::LogSystemMemoryStatus()
{
	BOOL bResult = TRUE;
	CString szDirectory("D:\\sw\\AC9000\\Data\\");
	MEMORYSTATUSEX statex;

	statex.dwLength = sizeof(statex);

	if (GlobalMemoryStatusEx(&statex))
	{
		CString szPath;
		CString szInFileName2("MemoryUseageLogSystem.csv");
		FILE *fp = NULL;

		CheckFileSize(szDirectory, szInFileName2, 512); //512KB

		szPath = szDirectory + szInFileName2;
		fp = fopen((char *)szPath.GetBuffer(0), "a+");
		if (fp != NULL)
		{
			CString szTime(" ");
			char tmpbuf[129];
			struct tm *today;
			__time64_t ltime;

			_time64(&ltime);
			today = _localtime64(&ltime);
			memset(&tmpbuf[0], 0 , 129);
			strftime(tmpbuf, 128, "%Y-%m-%d %H:%M:%S", today);
			szTime.Format("%s", tmpbuf);

			fseek(fp, 0, SEEK_END);
			if (ftell(fp) < 10)
			{//print header
				fprintf(fp, "Time, Title, 	Percent_Memory_In_Use, Total_Physical_Memory_KB, Free_Physical_Memory_KB, Total_Paging_File_KB, Free_paging_File_KB, Total_Virtual_Memory_KB, Free_Virtual_Memory_KB, Free_Extended_Memory_KB\n");
			}
			fprintf(fp, "%s, System, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu\n",
				(char *)szTime.GetBuffer(0),
				statex.dwMemoryLoad,					//1
				statex.ullTotalPhys / 1024,				//2
				statex.ullAvailPhys / 1024,				//3
				statex.ullTotalPageFile / 1024,
				statex.ullAvailPageFile / 1024,		//5
				statex.ullTotalVirtual / 1024,
				statex.ullAvailVirtual / 1024,
				statex.ullAvailExtendedVirtual / 1024 //8
				);
			fclose(fp);
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ULONGLONG CAC9000App::SubtractTimes(const FILETIME& ftA, const FILETIME& ftB)
{
	LARGE_INTEGER a, b;
	a.LowPart = ftA.dwLowDateTime;
	a.HighPart = ftA.dwHighDateTime;
	b.LowPart = ftB.dwLowDateTime;
	b.HighPart = ftB.dwHighDateTime;
	return a.QuadPart - b.QuadPart;
}

BOOL CAC9000App::GetCpuUsage(HANDLE hProcess, DOUBLE &dCpuUsage, FILETIME &ftPrevSysTime, FILETIME &ftPrevProcKernel, FILETIME &ftPrevProcUser)
{
	FILETIME ftProcCreation = {0,}, ftProcExit = {0,}, ftProcKernel = {0,}, ftProcUser = {0,}, ftSysTime = {0,};
	//ULONGLONG ull1 = 0, ull2 = 0;
	BOOL bResult = TRUE;
	SYSTEM_INFO sysinfo;

	GetSystemInfo( &sysinfo );

	if (
		//!GetSystemTimes(&ftSysIdle, &ftSysKernel, &ftSysUser) ||
		!GetProcessTimes(hProcess, &ftProcCreation,&ftProcExit, &ftProcKernel, &ftProcUser))
	{
		return FALSE;
	}

	if (ftPrevSysTime.dwLowDateTime == 0 && ftPrevSysTime.dwHighDateTime == 0)
	{
		GetSystemTimeAsFileTime(&ftPrevSysTime);
		ftPrevProcKernel = ftProcKernel;
		ftPrevProcUser = ftProcUser;
		return FALSE;
	}
	GetSystemTimeAsFileTime(&ftSysTime);

	ULONGLONG ftSysTimeDiff = SubtractTimes(ftSysTime, ftPrevSysTime);
	ULONGLONG ftProcKernelDiff = SubtractTimes(ftProcKernel, ftPrevProcKernel);
	ULONGLONG ftProcUserDiff = SubtractTimes(ftProcUser, ftPrevProcUser) / (ULONGLONG)sysinfo.dwNumberOfProcessors; //4 cpu cores

	if (ftProcKernelDiff + ftProcUserDiff > 0)
	{
		//dCpuUsage = (DOUBLE)((DOUBLE)(100.0 * ftProcUserDiff) / (DOUBLE)(ftProcKernelDiff + ftProcUserDiff));
		dCpuUsage = (DOUBLE)100.0 * ((DOUBLE)(ftProcUserDiff + ftProcKernelDiff) / (DOUBLE)(ftSysTimeDiff));
	}
	else
	{
		dCpuUsage = 0.0;
		//bResult = FALSE;
	}
	ftPrevProcKernel = ftProcKernel;
	ftPrevProcUser = ftProcUser;
	//dwLastRun = GetTickCount64();
	GetSystemTimeAsFileTime(&ftPrevSysTime);

	return bResult;
}
