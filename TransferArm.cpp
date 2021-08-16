/////////////////////////////////////////////////////////////////
//	TransferArm.cpp : interface of the CTransferArm class
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
#include "MarkConstant.h"
#include "AC9000.h"
#include "AC9000_Constant.h"

#define TRANSFERARM_EXTERN
#include "TransferArm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CTransferArm, CAC9000Stn)

CTransferArm::CTransferArm()
{
	m_lPickPlaceRetryCounter	= 0;
	m_lPickPlaceRetryLimit		= 0;
	
	m_dwStartTime				= 0;
	m_nCycleCounter				= 0;
	m_dCycleTime				= 0.0;
	for (INT i = 0; i < 10; i++)
	{
		m_dTempCycleTime[i] = 0.0;
	}

	m_lStandbyPosn				= 0;
	m_lLeftSafetyPosn			= 0;
	m_lRightSafetyPosn			= 0;
	m_lPickGlassPosn			= 0;
	m_lPlaceGlassPosn			= 0;

	m_bUseFPCVacuum = FALSE;	// 20140925 Yip
}

CTransferArm::~CTransferArm()
{
}

BOOL CTransferArm::InitInstance()
{
	CAC9000Stn::InitInstance();

	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	CGmpSystem *pGmpSys = pAppMod->m_pCGmpSystem;
	CString		szMsg	= _T("");

	INT i = 0;

	try
	{
		// Acutators
		//if (pAppMod->IsInitNuDrive())
		//{
		for (i = 0; i < m_myMotorList.GetSize(); i++)
		{
				// Initi Motor Configuration
			GetmyMotorList().GetAt(i)->SetAppStation(pAppMod);
			GetmyMotorList().GetAt(i)->SetStnName(GetStnName());
			if (pAppMod->IsInitNuDrive())
			{
				GetmyMotorList().GetAt(i)->SetGmpChannel(pGmpSys->AddChannel(m_myMotorList.GetAt(i)->stAttrib.szAxis, m_myMotorList.GetAt(i)->stAttrib.dDistPerCount));
				GetmyMotorList().GetAt(i)->InitMtrAxis();
			}
		}
		//}

		if (pAppMod->IsInitNuDrive())
		{
			// SI
			for (i = 0; i < m_mySiList.GetSize(); i++)
			{
				GetmySiList().GetAt(i)->SetGmpPort(
					pGmpSys->AddSiPort(m_mySiList.GetAt(i)->GetPortName(), 
									   m_mySiList.GetAt(i)->GetMask(), 
									   m_mySiList.GetAt(i)->GetActiveState()));
			}

			// SO
			for (i = 0; i < m_mySoList.GetSize(); i++)
			{
				GetmySoList().GetAt(i)->SetGmpPort(
					pGmpSys->AddSoPort(m_mySoList.GetAt(i)->GetPortName(), 
									   m_mySoList.GetAt(i)->GetMask(), 
									   m_mySoList.GetAt(i)->GetActiveState()));

				if( 
					GetmySoList().GetAt(i)->GetName() != "InPickArmRotarySol" ||
					GetmySoList().GetAt(i)->GetName() != "TA1TSol" ||
					GetmySoList().GetAt(i)->GetName() != "TA2TSol" 
					)
				{
					GetmySoList().GetAt(i)->GetGmpPort().Off();
				}
				GetmySoList().GetAt(i)->bHMI_Status = GetmySoList().GetAt(i)->GetGmpPort().IsOn();				
			}
		}
	}
	catch (CAsmException e)
	{
		pAppMod->m_bHWInitError = TRUE;

		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	while (pAppMod->m_bHWInitCheckingPressure) //20130711
	{
		Sleep(100);
	}

	if (m_fHardware && !pAppMod->m_bHWInitError && !pAppMod->m_bDiagnMode)
	{
		szMsg = _T("	") + GetName() + " --- commutating servos";
		DisplayMessage(szMsg);

		// Commutate Motor
		for (i = 0; i < m_myMotorList.GetSize(); i++)
		{
			if (GetmyMotorList().GetAt(i)->Commutate() != GMP_SUCCESS)
			{
				m_bInitInstanceError			= TRUE;
				pAppMod->m_bHWInitError			= TRUE;
			}
		}

		if (GetStnName() == "TA1")	
		{	
			pAppMod->m_bTA1Comm	= TRUE;	
		}
		else if (GetStnName() == "TA2")	
		{	
			pAppMod->m_bTA2Comm	= TRUE;	
		}
		else if (GetStnName() == "InPickArm")	
		{	
			pAppMod->m_bInPickArmComm	= TRUE;	
		}

		// wait for all servo commutated (even with error)
		do
		{
			Sleep(100);
		} while (!pAppMod->IsAllServoCommutated());

		if (!m_bInitInstanceError)
		{
			szMsg = _T("	") + GetName() + " --- homing motors";
			DisplayMessage(szMsg);

			// Home Motor
			for (i = 0; i < m_myMotorList.GetSize(); i++)
			{
				if (GetmyMotorList().GetAt(i)->Home(GMP_NOWAIT) != GMP_SUCCESS)
				{
					m_bInitInstanceError	= TRUE;
					pAppMod->m_bHWInitError = TRUE;
				}
			}

			// Sync & Enable Protection
			for (i = 0; i < m_myMotorList.GetSize(); i++)
			{
				if (
					GetmyMotorList().GetAt(i)->Sync() != GMP_SUCCESS ||
					GetmyMotorList().GetAt(i)->EnableProtection() != GMP_SUCCESS
				   )
				{
					m_bInitInstanceError	= TRUE;
					pAppMod->m_bHWInitError = TRUE;
				}
				else
				{
					GetmyMotorList().GetAt(i)->bHomeStatus = TRUE;
				}
			}

			if (!pAppMod->m_bHWInitError)
			{
				// Everythings OK
				m_bModSelected			= TRUE;
			}

			if (GetStnName() == "TA1")	
			{	
				pAppMod->m_bTA1Home	= TRUE;	
			}
			else if (GetStnName() == "TA2")	
			{	
				pAppMod->m_bTA2Home	= TRUE;	
			}
			//else if (GetStnName() == "TA3")	
			//{	
			//	pAppMod->m_bTA3Home	= TRUE;	
			//}
			//else if (GetStnName() == "TA4")	
			//{	
			//	pAppMod->m_bTA4Home	= TRUE;	
			//}
			//else if (GetStnName() == "TA5")	
			//{	
			//	pAppMod->m_bTA5Home	= TRUE;	
			//}
			else if (GetStnName() == "InPickArm")	
			{	
				pAppMod->m_bInPickArmHome	= TRUE;	
			}
		}
	}
	else
	{
		if (GetStnName() == "TA1")
		{	
			pAppMod->m_bTA1Home	= TRUE;
			pAppMod->m_bTA1Comm	= TRUE;
		}
		else if (GetStnName() == "TA2")	
		{	
			pAppMod->m_bTA2Home	= TRUE;	
			pAppMod->m_bTA2Comm	= TRUE;	
		}
		else if (GetStnName() == "InPickArm")	
		{	
			pAppMod->m_bInPickArmHome	= TRUE;	
			pAppMod->m_bInPickArmComm	= TRUE;	
		}
	}

	return TRUE;
}

INT CTransferArm::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CAC9000Stn::ExitInstance();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CTransferArm::Operation()
{
	switch (State())
	{		
	case IDLE_Q:
		IdleOperation();
		break;

	case DIAGNOSTICS_Q:
		DiagOperation();
		break;

	case SYSTEM_INITIAL_Q:
		InitOperation();
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

	case ERROR_Q:
		ErrorOperation();
		break;

	case STOPPING_Q:
		StopOperation();
		break;

	case DE_INITIAL_Q:
		DeInitialOperation();
		break;

	default:
		State(IDLE_Q);
		Motion(FALSE);
		break;
	}

	UpdateOutput();

	if (!theApp.m_bSaveLoadDeviceMachine) //20150824
	{
		Sleep(10);
	}
	else
	{
		Sleep(10*10);
	}
}


/////////////////////////////////////////////////////////////////
//Update Functions
/////////////////////////////////////////////////////////////////

VOID CTransferArm::UpdateOutput()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	INT i = 0;

	if ((m_qState == UN_INITIALIZE_Q) || (m_qState == DE_INITIAL_Q) || (m_qState == AUTO_Q) || (m_qState == DEMO_Q))
	{
		return;
	}

	if (pAppMod->m_bHWInitError)
	{
		return;
	}

	try
	{
		if (m_fHardware && m_bCheckMotor)
		{
			for (i = 0; i < m_myMotorList.GetSize(); i++)
			{
				GetmyMotorList().GetAt(i)->bHMI_On			= GetmyMotorList().GetAt(i)->IsPowerOn();
				GetmyMotorList().GetAt(i)->lHMI_CurrPosn	= GetmyMotorList().GetAt(i)->GetEncPosn();
			}

			if (m_bModSelected)
			{
				CheckModSelected(TRUE);
			}
		}

		// I/Os
		if (pAppMod->IsInitNuDrive() && m_bCheckIO)
		{
			for (i = 0; i < m_mySiList.GetSize(); i++)
			{
				GetmySiList().GetAt(i)->bHMI_Status			= GetmySiList().GetAt(i)->GetGmpPort().IsOn();
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	Sleep(UPDATEOUTPUT_SLEEP);
}

VOID CTransferArm::RegisterVariables()
{
	try
	{
		//Register Sensors here 
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
}

// ------------------------------------------------------------------------
// Virtual TransferArm Function delcaration
// ------------------------------------------------------------------------



// ------------------------------------------------------------------------
// END Virtual TransferArm Function delcaration
// ------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////
//Modified Virtual Operational Functions
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//Operational Sequences
/////////////////////////////////////////////////////////////////
VOID CTransferArm::SetDiagnSteps(ULONG ulSteps)
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

/////////////////////////////////////////////////////////////////
//I/O Related
/////////////////////////////////////////////////////////////////
BOOL CTransferArm::IsVacSensorOn(GlassNum lGlassNum)
{
	if (lGlassNum == GLASS1)
	{
		return IsGLASS1_VacSensorOn();
	}
	//else if (lGlassNum == GLASS2)
	//{
	//	return IsGLASS2_VacSensorOn();
	//}
	else
	{
		SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
		return FALSE;
	}
}

INT CTransferArm::SetVacSol(GlassNum lGlassNum, BOOL bMode, BOOL bWait)
{
	if (lGlassNum == GLASS1)
	{
		return SetGlassFPC1_VacSol(bMode, bWait);
	}
	//else if (lGlassNum == GLASS2)
	//{
	//	return SetGlassFPC2_VacSol(bMode, bWait);
	//}
	else
	{
		if (
			//(SetGlassFPC2_VacSol(bMode, GMP_NOWAIT) == GMP_SUCCESS) &&
			(SetGlassFPC1_VacSol(bMode, bWait) == GMP_SUCCESS)
		   )
		{
			return GMP_SUCCESS;
		}
	}
	return GMP_FAIL;
}	

INT CTransferArm::SetWeakBlowSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stWeakBlowSol;

	if(IsBurnInDiagBond())
	{
		Sleep(100);
		stTempSo->bHMI_Status = bMode;
		return GMP_SUCCESS;
	} 

#ifdef OFFLINE
	stTempSo->bHMI_Status = bMode;
	return nResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}
#endif
	if ((State() == AUTO_Q || State() == DEMO_Q || State() == PRESTART_Q || State() == STOPPING_Q) && (AutoMode == BURN_IN || AutoMode == DIAG_BOND))//20150630
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}

	try
	{
		if (bMode)
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			if (!g_bDiagMode)
			{
				stTempSo->GetGmpPort().On();
				stTempSo->bHMI_Status = TRUE;
			}
			else
			{
				stTempSo->GetGmpPort().Off();
				stTempSo->bHMI_Status = FALSE;
			}

			if (bWait)
			{
				Sleep(stTempSo->GetOnDelay());
			}

			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;

			if (bWait)
			{
				Sleep(stTempSo->GetOffDelay());
			}

			TC_DelJob(stTempSo->GetTCObjectID());
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}	

//INT CTransferArm::SetG2WeakBlowSol(BOOL bMode, BOOL bWait)
//{
//	INT nResult = GMP_SUCCESS;
//
//	CSoInfo *stTempSo = &m_stGlass2WeakBlowSol;
//
//	if(IsBurnInDiagBond())
//	{
//		Sleep(100);
//		stTempSo->bHMI_Status = bMode;
//		return GMP_SUCCESS;
//	} 
//
//#ifdef OFFLINE
//	stTempSo->bHMI_Status = bMode;
//	return nResult;
//#endif 
//
//#ifdef _DEBUG	// 20140917 Yip
//	if (!theApp.IsInitNuDrive())
//	{
//		stTempSo->bHMI_Status = bMode;
//		return nResult;
//	}
//#endif
//	if ((State() == AUTO_Q || State() == DEMO_Q || State() == PRESTART_Q || State() == STOPPING_Q) && (AutoMode == BURN_IN || AutoMode == DIAG_BOND))//20150630
//	{
//		stTempSo->bHMI_Status = bMode;
//		return nResult;
//	}
//
//	try
//	{
//		if (bMode)
//		{
//			TC_AddJob(stTempSo->GetTCObjectID());
//
//			if (!g_bDiagMode)
//			{
//				stTempSo->GetGmpPort().On();
//				stTempSo->bHMI_Status = TRUE;
//			}
//			else
//			{
//				stTempSo->GetGmpPort().Off();
//				stTempSo->bHMI_Status = FALSE;
//			}
//
//			if (bWait)
//			{
//				Sleep(stTempSo->GetOnDelay());
//			}
//
//			TC_DelJob(stTempSo->GetTCObjectID());
//		}
//		else
//		{
//			TC_AddJob(stTempSo->GetTCObjectID());
//
//			stTempSo->GetGmpPort().Off();
//			stTempSo->bHMI_Status = FALSE;
//
//			if (bWait)
//			{
//				Sleep(stTempSo->GetOffDelay());
//			}
//
//			TC_DelJob(stTempSo->GetTCObjectID());
//		}
//	}
//	catch (CAsmException e)
//	{
//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//		DisplayException(e);
//		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//	}
//
//	return nResult;
//}	

BOOL CTransferArm::IsLFPC_VacSensorOn()	// 20140619 Yip
{
	BOOL bResult = FALSE;

#ifdef OFFLINE
	return GetGlass1Status();
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		return GetGlass1Status();
	}
#endif

	try
	{
		bResult = m_stLFPCVacSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CTransferArm::IsRFPC_VacSensorOn()	// 20140619 Yip
{
	BOOL bResult = FALSE;

#ifdef OFFLINE
	return GetGlass2Status();
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		return GetGlass2Status();
	}
#endif

	try
	{
		bResult = m_stRFPCVacSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

INT CTransferArm::SetLFPCVacSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stLFPCVacSol;

	if(IsBurnInDiagBond())
	{
		Sleep(100);
		stTempSo->bHMI_Status = bMode;
		return GMP_SUCCESS;
	} 

#ifdef OFFLINE
	stTempSo->bHMI_Status = bMode;
	return nResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}
#endif
	if ((State() == AUTO_Q || State() == DEMO_Q || State() == PRESTART_Q || State() == STOPPING_Q) && (AutoMode == BURN_IN || AutoMode == DIAG_BOND))//20150630
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}

	try
	{
		if (bMode)
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			if (!g_bDiagMode)
			{
				stTempSo->GetGmpPort().On();
				stTempSo->bHMI_Status = TRUE;
			}

			if (bWait)
			{
				Sleep(stTempSo->GetOnDelay());
			}

			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			if (!g_bDiagMode)
			{
				stTempSo->GetGmpPort().Off();
				stTempSo->bHMI_Status = FALSE;
			}

			if (bWait)
			{
				Sleep(stTempSo->GetOffDelay());
			}

			TC_DelJob(stTempSo->GetTCObjectID());
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CTransferArm::SetRFPCVacSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stRFPCVacSol;

	if(IsBurnInDiagBond())
	{
		Sleep(100);
		stTempSo->bHMI_Status = bMode;
		return GMP_SUCCESS;
	} 

#ifdef OFFLINE
	stTempSo->bHMI_Status = bMode;
	return nResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}
#endif
	if ((State() == AUTO_Q || State() == DEMO_Q || State() == PRESTART_Q || State() == STOPPING_Q) && (AutoMode == BURN_IN || AutoMode == DIAG_BOND))//20150630
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}

	try
	{
		if (bMode)
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			if (!g_bDiagMode)
			{
				stTempSo->GetGmpPort().On();
				stTempSo->bHMI_Status = TRUE;
			}

			if (bWait)
			{
				Sleep(stTempSo->GetOnDelay());
			}

			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			if (!g_bDiagMode)
			{
				stTempSo->GetGmpPort().Off();
				stTempSo->bHMI_Status = FALSE;
			}

			if (bWait)
			{
				Sleep(stTempSo->GetOffDelay());
			}

			TC_DelJob(stTempSo->GetTCObjectID());
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CTransferArm::SetRotarySol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stRotarySol;
	LONG lWait = 1000 * 10;

	if (!m_fHardware) //20160128
	{
		return GMP_SUCCESS;
	}

#ifdef OFFLINE
	stTempSo->bHMI_Status = bMode;
	return nResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}
#endif

	try
	{
		if (bMode)
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().On();
			stTempSo->bHMI_Status = TRUE;

			if (bWait)
			{
				lWait = max(stTempSo->GetOnDelay(), 10);
				while (lWait >= 0 && !IsT1SnrOn())
				{
					lWait -= 10;
					Sleep(10);
				}
				if (lWait < 0)
				{
					nResult = GMP_FAIL;
				}
			}
			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;

			if (bWait)
			{
				lWait = max(stTempSo->GetOffDelay(), 100);
				while (lWait >= 0 && !IsT2SnrOn())
				{
					lWait -= 10;
					Sleep(10);
				}
				if (lWait < 0)
				{
					nResult = GMP_FAIL;
				}
			}

			TC_DelJob(stTempSo->GetTCObjectID());
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

INT CTransferArm::SetZBrakeSol(BOOL bMode, BOOL bWait)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stZBrakeSol;


#ifdef OFFLINE
	stTempSo->bHMI_Status = bMode;
	return nResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}
#endif

	if (!m_fHardware) //20121127
	{
		return GMP_SUCCESS;
	}

	try
	{
		if (bMode)
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().On();
			stTempSo->bHMI_Status = TRUE;

			if (bWait)
			{
				Sleep(stTempSo->GetOnDelay());
			}

			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;

			if (bWait)
			{
				Sleep(stTempSo->GetOffDelay());
			}

			TC_DelJob(stTempSo->GetTCObjectID());
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
	
	return nResult;
}

INT CTransferArm::MoveTPickPosn(BOOL bWait)
{
	return SetRotarySol(OFF, bWait);		//ON = Pick, LOAD,  OFF = Place, Unload
}

INT CTransferArm::MoveTPlacePosn(BOOL bWait)
{
	return SetRotarySol(ON, bWait);	//ON = Pick, LOAD,  OFF = Place, Unload
}

INT CTransferArm::SyncZ()	
{
	return m_stMotorZ.Sync();
}

BOOL CTransferArm::IsT1SnrOn()	//Upper Snr
{
	BOOL bResult = FALSE;

#ifdef OFFLINE
	return bResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		return bResult;
	}
#endif

	try
	{
		bResult = m_stT1Snr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CTransferArm::IsT2SnrOn()		//Lower snr
{
	BOOL bResult = FALSE;

#ifdef OFFLINE
	return bResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		return bResult;
	}
#endif

	try
	{
		bResult = m_stT2Snr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CTransferArm::IsGLASS1_VacSensorOn()
{
	BOOL bResult = FALSE;

#ifdef OFFLINE
	return GetGlass1Status();
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		return GetGlass1Status();
	}
#endif

	try
	{
		bResult = m_stGlass1VacSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CTransferArm::IsGLASS2_VacSensorOn()
{
	BOOL bResult = FALSE;

#ifdef OFFLINE
	return GetGlass2Status();
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		return GetGlass2Status();
	}
#endif

	try
	{
		bResult = m_stGlass2VacSnr.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

INT CTransferArm::SetGLASS1_VacSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stGlass1VacSol;

	if(IsBurnInDiagBond())
	{
		Sleep(100);
		stTempSo->bHMI_Status = bMode;
		return GMP_SUCCESS;
	} 

#ifdef OFFLINE
	stTempSo->bHMI_Status = bMode;
	return nResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}
#endif
	if ((State() == AUTO_Q || State() == DEMO_Q || State() == PRESTART_Q || State() == STOPPING_Q) && (AutoMode == BURN_IN || AutoMode == DIAG_BOND))//20150630
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}

	try
	{
		if (bMode)
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			if (!g_bDiagMode)
			{
				stTempSo->GetGmpPort().On();
				stTempSo->bHMI_Status = TRUE;
			}

			if (bWait)
			{
				Sleep(stTempSo->GetOnDelay());
			}

			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			if (!g_bDiagMode)
			{
				stTempSo->GetGmpPort().Off();
				stTempSo->bHMI_Status = FALSE;
			}

			if (bWait)
			{
				Sleep(stTempSo->GetOffDelay());
			}

			TC_DelJob(stTempSo->GetTCObjectID());
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
	
	return nResult;
}

INT CTransferArm::SetGLASS2_VacSol(BOOL bMode, BOOL bWait)
{
	INT nResult = GMP_SUCCESS;

	CSoInfo *stTempSo = &m_stGlass2VacSol;

	if(IsBurnInDiagBond())
	{
		Sleep(100);
		stTempSo->bHMI_Status = bMode;
		return GMP_SUCCESS;
	} 

#ifdef OFFLINE
	stTempSo->bHMI_Status = bMode;
	return nResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}
#endif
	if ((State() == AUTO_Q || State() == DEMO_Q || State() == PRESTART_Q || State() == STOPPING_Q) && (AutoMode == BURN_IN || AutoMode == DIAG_BOND))//20150630
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}

	try
	{
		if (bMode)
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			if (!g_bDiagMode)
			{
				stTempSo->GetGmpPort().On();
				stTempSo->bHMI_Status = TRUE;
			}

			if (bWait)
			{
				Sleep(stTempSo->GetOnDelay());
			}

			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			if (!g_bDiagMode)
			{
				stTempSo->GetGmpPort().Off();
				stTempSo->bHMI_Status = FALSE;
			}

			if (bWait)
			{
				Sleep(stTempSo->GetOffDelay());
			}

			TC_DelJob(stTempSo->GetTCObjectID());
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}
	
	return nResult;
}

BOOL CTransferArm::IsVacStateOn(GLASS_FPC_VAC_STATE stVacState)	// 20140619 Yip
{
	return (stVacState == GLASS_ON_FPC_ON);
}

BOOL CTransferArm::IsVacStateOff(GLASS_FPC_VAC_STATE stVacState)	// 20140619 Yip
{
	return (stVacState == GLASS_OFF_FPC_OFF);
}

BOOL CTransferArm::IsVacStateMatched(GLASS_FPC_VAC_STATE stVacState)	// 20140619 Yip
{
	return ((stVacState == GLASS_OFF_FPC_OFF) || (stVacState == GLASS_ON_FPC_ON));
}

GLASS_FPC_VAC_STATE CTransferArm::GetGlass1_VacSensorOnState()	// 20140619 Yip
{
	return (IsGLASS1_VacSensorOn() ? GLASS_ON_FPC_ON : GLASS_OFF_FPC_OFF);
}

GLASS_FPC_VAC_STATE CTransferArm::GetGlass2_VacSensorOnState()	// 20140619 Yip
{
	return (IsGLASS2_VacSensorOn() ? GLASS_ON_FPC_ON : GLASS_OFF_FPC_OFF);
}

GLASS_FPC_VAC_STATE CTransferArm::GetGlassFPC1_VacSensorOnState()	// 20140619 Yip
{
	if (m_bUseFPCVacuum)
	{
		return (GLASS_FPC_VAC_STATE)((IsGLASS1_VacSensorOn() ? GLASS_ON_FPC_OFF : GLASS_OFF_FPC_OFF) + (IsLFPC_VacSensorOn() ? GLASS_OFF_FPC_ON : GLASS_OFF_FPC_OFF));
	}
	return GetGlass1_VacSensorOnState();
}

GLASS_FPC_VAC_STATE CTransferArm::GetGlassFPC2_VacSensorOnState()	// 20140619 Yip
{
	if (m_bUseFPCVacuum)
	{
		return (GLASS_FPC_VAC_STATE)((IsGLASS2_VacSensorOn() ? GLASS_ON_FPC_OFF : GLASS_OFF_FPC_OFF) + (IsRFPC_VacSensorOn() ? GLASS_OFF_FPC_ON : GLASS_OFF_FPC_OFF));
	}
	return GetGlass2_VacSensorOnState();
}

INT CTransferArm::SetGlassFPC1_VacSol(BOOL bMode, BOOL bWait)	// 20140619 Yip
{
	if (SetGLASS1_VacSol(bMode, GMP_NOWAIT) != GMP_SUCCESS)
	{
		return GMP_FAIL;
	}

	if (m_bUseFPCVacuum)
	{
		if (SetLFPCVacSol(bMode, GMP_NOWAIT) != GMP_SUCCESS)
		{
			return GMP_FAIL;
		}
	}

	if (bWait)
	{
		if (m_bUseFPCVacuum)
		{
			Sleep(bMode ? m_stGlass1VacSol.GetOnDelay() : m_stGlass1VacSol.GetOffDelay());
		}
		else
		{
			if (bMode)
			{
				Sleep(max(m_stGlass1VacSol.GetOnDelay(), m_stLFPCVacSol.GetOnDelay()));
			}
			else
			{
				Sleep(max(m_stGlass1VacSol.GetOffDelay(), m_stLFPCVacSol.GetOffDelay()));
			}
		}
	}

	return GMP_SUCCESS;
}

INT CTransferArm::SetGlassFPC2_VacSol(BOOL bMode, BOOL bWait)	// 20140619 Yip
{
	if (SetGLASS2_VacSol(bMode, GMP_NOWAIT) != GMP_SUCCESS)
	{
		return GMP_FAIL;
	}

	if (m_bUseFPCVacuum)
	{
		if (SetRFPCVacSol(bMode, GMP_NOWAIT) != GMP_SUCCESS)
		{
			return GMP_FAIL;
		}
	}

	if (bWait)
	{
		if (m_bUseFPCVacuum)
		{
			Sleep(bMode ? m_stGlass2VacSol.GetOnDelay() : m_stGlass2VacSol.GetOffDelay());
		}
		else
		{
			if (bMode)
			{
				Sleep(max(m_stGlass2VacSol.GetOnDelay(), m_stRFPCVacSol.GetOnDelay()));
			}
			else
			{
				Sleep(max(m_stGlass2VacSol.GetOffDelay(), m_stRFPCVacSol.GetOffDelay()));
			}
		}
	}

	return GMP_SUCCESS;
}


INT CTransferArm::SetHandShakeBit(CSoInfo *stTempSo, BOOL bMode)
{
	INT nResult = GMP_SUCCESS;

//	CSoInfo *stTempSo = &m_stWeakBlowSol;

	if (IsBurnInDiagBond())
	{
		//Sleep(100);
		stTempSo->bHMI_Status = bMode;
		return GMP_SUCCESS;
	}

#ifdef OFFLINE
	stTempSo->bHMI_Status = bMode;
	return nResult;
#endif 

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		stTempSo->bHMI_Status = bMode;
		return nResult;
	}
#endif

	try
	{
		if (bMode)
		{
			TC_AddJob(stTempSo->GetTCObjectID());

				stTempSo->GetGmpPort().On();
				stTempSo->bHMI_Status = TRUE;

			TC_DelJob(stTempSo->GetTCObjectID());
		}
		else
		{
			TC_AddJob(stTempSo->GetTCObjectID());

			stTempSo->GetGmpPort().Off();
			stTempSo->bHMI_Status = FALSE;

			TC_DelJob(stTempSo->GetTCObjectID());
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}	

BOOL CTransferArm::IsHandShakeBitOn(CSiInfo stTempSi)
{
	BOOL bResult = FALSE;

	try
	{
		bResult = stTempSi.GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		DisplayException(e);
		DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

GlassNum CTransferArm::CalcPickStatus(BOOL bWHStatus[], BOOL bArmStatus[])
{
	//LONG lResult[MAX_NUM_OF_GLASS] = {FALSE, FALSE};

	//for (INT i = GLASS1; i < MAX_NUM_OF_GLASS; i++)
	//{
	//	lResult[i] = bWHStatus[i] * 10 + bArmStatus[i];
	//}

	//// WH have glass, and Arm not glass = 10
	//if (lResult[GLASS1] == 10 && lResult[GLASS2] == 10)
	//{
	//	return MAX_NUM_OF_GLASS;
	//}
	//else if (lResult[GLASS1] == 10)
	//{
	//	return GLASS1;
	//}
	//else if (lResult[GLASS2] == 10)
	//{
	//	return GLASS2;
	//}
	//else
	//{
	//	return NO_GLASS;
	//}	

	return GLASS1;
}

GlassNum CTransferArm::CalcPlaceStatus(BOOL bWHStatus[], BOOL bArmStatus[])
{
	//LONG lResult[MAX_NUM_OF_GLASS] = {FALSE, FALSE};

	//for (INT i = GLASS1; i < MAX_NUM_OF_GLASS; i++)
	//{
	//	lResult[i] = bArmStatus[i] * 10 + bWHStatus[i];
	//}

	//// WH have glass, and Arm not glass = 10
	//if (lResult[GLASS1] == 10 && lResult[GLASS2] == 10)
	//{
	//	return MAX_NUM_OF_GLASS;
	//}
	//else if (lResult[GLASS1] == 10)
	//{
	//	return GLASS1;
	//}
	//else if (lResult[GLASS2] == 10)
	//{
	//	return GLASS2;
	//}
	//else
	//{
	//	return NO_GLASS;
	//}

	return GLASS1;
}

INT CTransferArm::CheckModulePower() //20121219 arm crash
{
	//LONG lAnswer = rMSG_TIMEOUT;
	CString szMsg = _T("                         ");

	if (m_bModSelected)
	{
		for (INT i = 0 ; i < GetmyMotorList().GetSize(); i++)
		{
			//szMsg = GetmyMotorList().GetAt(i)->GetName(); //test
			if (!GetmyMotorList().GetAt(i)->IsPowerOn())
			{
				szMsg.Format("%s is Power Off. Operation Abort!", GetStnName());

				HMIMessageBox(MSG_OK, "WARNING", szMsg);
				return GMP_FAIL;

				//szMsg.Format("%s is Power Off. Continue?", GetStnName());
				//lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", szMsg);
				//if (lAnswer == rMSG_TIMEOUT || lAnswer == rMSG_CANCEL)
				//{
				//	HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
				//	return GMP_FAIL;
				//}
			}
		}
	}
	else
	{
		szMsg.Format("%s Module Not Selected. Operation Abort!", GetStnName());
		HMIMessageBox(MSG_OK, "WARNING", szMsg);
		return GMP_FAIL;

		//szMsg.Format("%s Module Not Selected. Continue?", GetStnName());
		//lAnswer = HMIMessageBox(MSG_CONTINUE_CANCEL, "WARNING", szMsg);
		//if (lAnswer == rMSG_TIMEOUT || lAnswer == rMSG_CANCEL)
		//{
		//	HMIMessageBox(MSG_OK, "WARNING", "Operation Abort!");
		//	return GMP_FAIL;
		//}
	}
	return GMP_SUCCESS;
}

// ------------------------------------------------------------------------
// HMI Command delcaration
// ------------------------------------------------------------------------

LONG CTransferArm::HMI_PowerOnX(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	m_stMotorX.PowerOn(bMode);

	bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

// Commutate
LONG CTransferArm::HMI_CommX(IPC_CServiceMessage &svMsg)
{
	m_stMotorX.Commutate();

	return 0;
}

// Home
LONG CTransferArm::HMI_HomeX(IPC_CServiceMessage &svMsg)
{
	m_stMotorX.Home(GMP_WAIT);

	return 0;
}

LONG CTransferArm::HMI_PowerOnZ(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	m_stMotorZ.PowerOn(bMode);

	bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

// Commutate
LONG CTransferArm::HMI_CommZ(IPC_CServiceMessage &svMsg)
{
	m_stMotorZ.Commutate();

	return 0;
}

// Home
LONG CTransferArm::HMI_HomeZ(IPC_CServiceMessage &svMsg)
{
	m_stMotorZ.Home(GMP_WAIT);

	return 0;
}

LONG CTransferArm::HMI_PowerOnY(IPC_CServiceMessage &svMsg)
{
	BOOL bMode;
	svMsg.GetMsg(sizeof(BOOL), &bMode);

	m_stMotorY.PowerOn(bMode);

	bMode = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bMode);
	return 1;
}

// Commutate
LONG CTransferArm::HMI_CommY(IPC_CServiceMessage &svMsg)
{
	m_stMotorY.Commutate();

	return 0;
}

// Home
LONG CTransferArm::HMI_HomeY(IPC_CServiceMessage &svMsg)
{
	m_stMotorY.Home(GMP_WAIT);

	return 0;
}

LONG CTransferArm::HMI_ToggleG1VacSol(IPC_CServiceMessage &svMsg)
{
	SetGLASS1_VacSol(!m_stGlass1VacSol.bHMI_Status);
	return 0;
}

//LONG CTransferArm::HMI_ToggleG2VacSol(IPC_CServiceMessage &svMsg)
//{
//	SetGLASS2_VacSol(!m_stGlass2VacSol.bHMI_Status);
//	return 0;
//}

LONG CTransferArm::HMI_ToggleWeakBlowSol(IPC_CServiceMessage &svMsg)
{
	SetWeakBlowSol(!m_stWeakBlowSol.bHMI_Status);
	return 0;
}

//LONG CTransferArm::HMI_ToggleG2WeakBlowSol(IPC_CServiceMessage &svMsg)
//{
//	SetG2WeakBlowSol(!m_stGlass2WeakBlowSol.bHMI_Status);
//	return 0;
//}


LONG CTransferArm::HMI_ToggleLFPCVacSol(IPC_CServiceMessage &svMsg)
{
	SetLFPCVacSol(!m_stLFPCVacSol.bHMI_Status);
	return 0;
}


LONG CTransferArm::HMI_ToggleRFPCVacSol(IPC_CServiceMessage &svMsg)
{
	SetRFPCVacSol(!m_stRFPCVacSol.bHMI_Status);
	return 0;
}

LONG CTransferArm::HMI_ToggleTSol(IPC_CServiceMessage &svMsg)
{
	SetRotarySol(!m_stRotarySol.bHMI_Status, GMP_WAIT);
	return 0;
}



// Setup Page Related
LONG CTransferArm::HMI_SetDiagnSteps(IPC_CServiceMessage &svMsg)
{
	ULONG ulSteps;
	svMsg.GetMsg(sizeof(ULONG), &ulSteps);

	g_lDiagnSteps = ulSteps;
	SetDiagnSteps(ulSteps);
	return 0;
}

LONG CTransferArm::HMI_IndexXPos(IPC_CServiceMessage &svMsg)
{
	m_stMotorX.MoveRelative(m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CTransferArm::HMI_IndexXNeg(IPC_CServiceMessage &svMsg)
{
	m_stMotorX.MoveRelative(-m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CTransferArm::HMI_IndexZPos(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	m_stMotorZ.MoveRelative(m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CTransferArm::HMI_IndexZNeg(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	m_stMotorZ.MoveRelative(-m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CTransferArm::HMI_IndexYPos(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	m_stMotorY.MoveRelative(m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CTransferArm::HMI_IndexYNeg(IPC_CServiceMessage &svMsg)
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	m_stMotorY.MoveRelative(-m_lDiagnSteps, GMP_WAIT);
	return 0;
}

LONG CTransferArm::HMI_SetPickPlaceRetryLimit(IPC_CServiceMessage &svMsg)
{
	LONG lLimit;
	svMsg.GetMsg(sizeof(LONG), &lLimit);

	m_lPickPlaceRetryLimit = lLimit;

	return 0;
}

LONG CTransferArm::IPC_SaveMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lStandbyPosn"]		= m_lStandbyPosn;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lLeftSafetyPosn"]	= m_lLeftSafetyPosn;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lRightSafetyPosn"]	= m_lRightSafetyPosn;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPickGlassPosn"]		= m_lPickGlassPosn;
	pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPlaceGlassPosn"]	= m_lPlaceGlassPosn;

	return CAC9000Stn::IPC_SaveMachineParam();
}

LONG CTransferArm::IPC_LoadMachineParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	m_lStandbyPosn			= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lStandbyPosn"];
	m_lLeftSafetyPosn		= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lLeftSafetyPosn"];
	m_lRightSafetyPosn		= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lRightSafetyPosn"];
	m_lPickGlassPosn		= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPickGlassPosn"];
	m_lPlaceGlassPosn		= pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPlaceGlassPosn"];

	return CAC9000Stn::IPC_LoadMachineParam();
}

LONG CTransferArm::IPC_SaveDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	pAppMod->m_smfDevice[GetStnName()]["Para"]["m_lPickPlaceRetryLimit"]	= m_lPickPlaceRetryLimit;

	return CAC9000Stn::IPC_SaveDeviceParam();
}

LONG CTransferArm::IPC_LoadDeviceParam()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);

	m_lPickPlaceRetryLimit			= pAppMod->m_smfDevice[GetStnName()]["Para"]["m_lPickPlaceRetryLimit"];

	return CAC9000Stn::IPC_LoadDeviceParam();
}

#ifdef EXCEL_MACHINE_DEVICE_INFO
BOOL CTransferArm::printMachineDeviceInfo()
{
	CAC9000App *pAppMod = dynamic_cast<CAC9000App*>(m_pModule);
	BOOL bResult = TRUE;
	CString szBlank("--");
	CString szModule = GetStnName();
	LONG lValue = 0;
	DOUBLE dValue = 0.0;

	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lStandbyPosn"];
		bResult = printMachineInfoRow(szModule, CString("Posn"), CString("StandbyPosn"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lLeftSafetyPosn"];
		bResult = printMachineInfoRow(szModule, CString("Posn"), CString("LeftSafetyPosn"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lRightSafetyPosn"];
		bResult = printMachineInfoRow(szModule, CString("Posn"), CString("RightSafetyPosn"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPickGlassPosn"];
		bResult = printMachineInfoRow(szModule, CString("Posn"), CString("PickGlassPosn"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}
	if (bResult)
	{
		lValue = (LONG) pAppMod->m_smfMachine[GetStnName()]["Posn"]["m_lPlaceGlassPosn"];
		bResult = printMachineInfoRow(szModule, CString("Posn"), CString("PlaceGlassPosn"), szBlank, szBlank,
									  szBlank, szBlank, szBlank, szBlank, szBlank, lValue);
	}

	if (bResult)
	{
		bResult = CAC9000Stn::printMachineDeviceInfo();
	}
	return bResult;
}
#endif

