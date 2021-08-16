//////////////////////////////////////////////////////////////////////////////
//	AC9000_Constant.h : Defines the Constant header
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

//////////////////////////////////////////////////////////////////////////////
//	State Definitions
//////////////////////////////////////////////////////////////////////////////

enum {UN_INITIALIZE_Q, IDLE_Q, SYSTEM_INITIAL_Q, PRESTART_Q, DIAGNOSTICS_Q,
	  AUTO_Q, DEMO_Q, MANUAL_Q, STOPPING_Q, ERROR_Q, DE_INITIAL_Q, CYCLE_Q, LAST_Q};

const CString gszMODULE_STATE[]		= {_T("Un-Initialize"),
									   _T("Idle"),
									   _T("System-Init"),
									   _T("Pre-Start"),
									   _T("Diagnostics"),
									   _T("Auto-Run"),
									   _T("Demonstration"),
									   _T("Manual"),
									   _T("Stopping"),
									   _T("Error"),
									   _T("De-Initialize"),
									   _T("Cycle")};

const DOUBLE gdGlassOnWHDistance = (10.0 * 1000.0); //20120918 10mm

#define MAX_PR_IGNORE_WINDOWS	8
