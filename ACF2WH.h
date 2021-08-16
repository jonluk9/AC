/////////////////////////////////////////////////////////////////
//	ACF2WH.cpp : interface of the CACF2WH class
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

#include "ACFWH.h"


class CACF2WH : public CACFWH
{
	DECLARE_DYNCREATE(CACF2WH)

protected:
	

protected:
	virtual VOID RegisterVariables();

private:
	virtual CString GetStation() 
	{
		return "ACF2WH_";
	}

public:
	CACF2WH();
	virtual	~CACF2WH();

	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID Operation();
};
