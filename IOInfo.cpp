#include "stdafx.h"

#include "AC9000.h"

// Si Related
CSiInfo::CSiInfo()
{
	pGmpPort		= NULL;
	szPortName		= "DEFAULT_PORT_NAME";
	unMask			= BIT_0;
	bActiveState	= ACTIVE_HIGH;

	bHMI_Status		= OFF;
}

CSiInfo::CSiInfo(CString szName, CString szPortName, unsigned int unMask, BOOL bActiveState)
{
	this->pGmpPort		= NULL;	// 20140711 Yip
	this->szName		= szName;
	this->szPortName	= szPortName;
	this->unMask		= unMask;
	this->bActiveState	= bActiveState;

	bHMI_Status			= OFF;
}

CSiInfo::~CSiInfo()
{

}

VOID CSiInfo::SetGmpPort(CGmpSiPort *pGmpPort)
{
	this->pGmpPort		= pGmpPort;
}

CGmpSiPort &CSiInfo::GetGmpPort()
{
	return *pGmpPort;
}

CString CSiInfo::GetPortName()
{
	return szPortName;
}

CString CSiInfo::GetName()
{
	return szName;
}

unsigned int CSiInfo::GetMask()
{
	return unMask;
}

BOOL CSiInfo::GetActiveState()
{
	return bActiveState;
}

BOOL CSiInfo::IsOn()
{
	BOOL bResult = FALSE;

	if (!IsHardwareSelected())
	{
		return TRUE; //bResult;
	}

	try
	{
		bResult = GetGmpPort().IsOn();
	}
	catch (CAsmException e)
	{
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return bResult;
}

BOOL CSiInfo::IsHardwareSelected()
{
	CAppStation *pStation		= NULL;

	if (pAppStation == NULL)
	{
		return FALSE;
	}

	if (szChStnName == "")
	{
		pAppStation->DisplayMessage("szChStnName cannot be empty");
		return FALSE;
	}

	if (
		pAppStation != NULL &&
		(pStation = dynamic_cast<CAppStation*>(pAppStation->GetStation(szChStnName))) != NULL
	   )
	{
		return pStation->IsHardware();
	}
	else
	{
		if (pAppStation != NULL)
		{
			CString szMsg = "";
			pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			pAppStation->SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
			pAppStation->DisplayMessage("Fn: IsHardwareSelect()");
			szMsg.Format("Err: GetStation(%s)", szChStnName);
			pAppStation->DisplayMessage(szMsg);
			pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		}
		return FALSE;
	}
}

BOOL CSoInfo::IsHardwareSelected()
{
	CAppStation *pStation		= NULL;

	if (pAppStation == NULL)
	{
		return FALSE;
	}

	if (szChStnName == "")
	{
		pAppStation->DisplayMessage("szChStnName cannot be empty");
		return FALSE;
	}

	if (
		pAppStation != NULL &&
		(pStation = dynamic_cast<CAppStation*>(pAppStation->GetStation(szChStnName))) != NULL
	   )
	{
		return pStation->IsHardware();
	}
	else
	{
		if (pAppStation != NULL)
		{
			CString szMsg = "";
			pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			pAppStation->SetError(IDS_HK_SOFTWARE_HANDLING_ERR);
			pAppStation->DisplayMessage("Fn: IsHardwareSelect()");
			szMsg.Format("Err: GetStation(%s)", szChStnName);
			pAppStation->DisplayMessage(szMsg);
			pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		}
		return FALSE;
	}
}


// So Related
CSoInfo::CSoInfo()
{
	pGmpPort		= NULL;

	szPortName		= "DEFAULT_PORT_NAME";
	unMask			= BIT_0;

	nTCObjectID		= -1;	// 20140718 Yip

	bLogMotion		= FALSE;

	lOnDelay		= 500;
	lOffDelay		= 500;

	bHMI_Status		= OFF;
}

CSoInfo::CSoInfo(CString szName, CString szPortName, unsigned int unMask, INT nTCObjectID, BOOL bActiveState, BOOL bIsMachineData)
{
	this->szName			= szName;
	this->szPortName		= szPortName;
	this->unMask			= unMask;
	this->nTCObjectID		= nTCObjectID;
	this->bActiveState		= bActiveState;
	this->bIsMachineData	= bIsMachineData;

	pGmpPort		= NULL;	// 20140711 Yip
	bLogMotion		= FALSE;

	lOnDelay		= 500;
	lOffDelay		= 500;

	bHMI_Status		= OFF;
}

CSoInfo::CSoInfo(CString szName, CString szPortName, unsigned int unMask, INT nTCObjectID, BOOL bActiveState)
{
	this->szName			= szName;
	this->szPortName		= szPortName;
	this->unMask			= unMask;
	this->nTCObjectID		= nTCObjectID;
	this->bActiveState		= bActiveState;

	bIsMachineData	= FALSE;

	pGmpPort		= NULL;	// 20140711 Yip
	bLogMotion		= FALSE;

	lOnDelay		= 500;
	lOffDelay		= 500;

	bHMI_Status		= OFF;
}

CSoInfo::~CSoInfo()
{

}

VOID CSoInfo::SetGmpPort(CGmpSoPort *pGmpPort)
{
	this->pGmpPort = pGmpPort;
}

CGmpSoPort &CSoInfo::GetGmpPort()
{
	return *pGmpPort;
}

CString CSoInfo::GetName()
{
	return szName;
}

CString CSoInfo::GetPortName()
{
	return szPortName;
}

unsigned int CSoInfo::GetMask()
{
	return unMask;
}

BOOL CSoInfo::GetActiveState()
{
	return bActiveState;
}

INT CSoInfo::GetTCObjectID()
{
	return nTCObjectID;
}

BOOL CSoInfo::IsMachineData()
{
	return bIsMachineData;
}

VOID CSoInfo::SetOnDelay(LONG lDelay)
{
	lOnDelay		= lDelay;
}

VOID CSoInfo::SetOffDelay(LONG lDelay)
{
	lOffDelay		= lDelay;
}

LONG CSoInfo::GetOnDelay()
{
	return lOnDelay;
}

LONG CSoInfo::GetOffDelay()
{
	return lOffDelay;
}

INT CSoInfo::SetSol(BOOL bMode, BOOL bWait, BOOL bDiagMode)
{
	INT nResult = GMP_SUCCESS;
	LONG lDelay = 0;

	if (!IsHardwareSelected())
	{
		////For diagnoistic
		//bHMI_Status = bMode;
		return nResult;
	}

	try
	{
		if (bMode)
		{
			pAppStation->TC_AddJob(GetTCObjectID());

			if (GetOnDelay() > 0)
			{
				GetGmpPort().On();
				bHMI_Status = TRUE;
			}
			else
			{
				if (pAppStation->State() == 1 && bDiagMode)
				{
					pAppStation->HMIMessageBox(MSG_OK, "SetSol", "To switch on a solenoid, On Delay should be larger than zero");
				}
			}


			if (bWait)
			{
				Sleep(GetOnDelay());
			}

			pAppStation->TC_DelJob(GetTCObjectID());
		}
		else
		{
			pAppStation->TC_AddJob(GetTCObjectID());

			GetGmpPort().Off();
			bHMI_Status = FALSE;

			lDelay = GetOffDelay();
			if (bWait)
			{
				//CString szDebug = "";
				//szDebug.Format("%s Sleep Off Delay %d",szName, lDelay);
				//pAppStation->DisplayMessage(szDebug);
				Sleep(lDelay);
			}

			pAppStation->TC_DelJob(GetTCObjectID());
		}
	}
	catch (CAsmException e)
	{
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return GMP_FAIL;
	}

	return nResult;
}

// PortInfo Related
CPortInfo::CPortInfo()
{
	pGmpPort		= NULL;

	szName			= "DEFAULT_PORT_NAME";
	szPortName		= "DEFAULT_PORT_NAME";
	unMask			= BIT_0;
}

CPortInfo::CPortInfo(CString szName, CString szPortName, unsigned int unMask)
{
	this->pGmpPort		= NULL;	// 20140711 Yip
	this->szName		= szName;
	this->szPortName	= szPortName;
	this->unMask		= unMask;
}

CPortInfo::~CPortInfo()
{

}

VOID CPortInfo::SetGmpPort(CGmpDACPort *pGmpPort)
{
	this->pGmpPort		= pGmpPort;
}

VOID CPortInfo::SetAppStation(CAC9000App *pAppStation)
{
	this->pAppStation		= pAppStation;
}

CString CPortInfo::GetName()
{
	return szName;
}

CString CPortInfo::GetPortName()
{
	return szPortName;
}

unsigned int CPortInfo::GetMask()
{
	return unMask;
}

INT CPortInfo::SetDACValue(DOUBLE dValue)
{
	INT nResult = GMP_SUCCESS;
	LONG lValue = 0;
	lValue = (LONG)(dValue / 5.0 * 32767.0);

#ifdef OFFLINE
	return nResult;
#endif

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		return nResult;
	}
#endif

	if (pGmpPort == NULL)	// 20140711 Yip
	{
		return nResult;
	}

	try
	{
		pGmpPort->SetPattern(lValue);
	}
	catch (CAsmException e)
	{
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return nResult;
}

ULONG CPortInfo::GetDACValue()
{
	ULONG ulResult = 0;

#ifdef OFFLINE
	return ulResult;
#endif

#ifdef _DEBUG	// 20140917 Yip
	if (!theApp.IsInitNuDrive())
	{
		return ulResult;
	}
#endif

	if (pGmpPort == NULL)	// 20140711 Yip
	{
		return ulResult;
	}

	try
	{
		ulResult = pGmpPort->GetDACPattern();
	}
	catch (CAsmException e)
	{
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		pAppStation->DisplayException(e);
		pAppStation->DisplayMessage("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	return ulResult;
}

