/////////////////////////////////////////////////////////////////
//	ACF1WH.cpp : interface of the CACF1WH class
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


class CACF1WH : public CACFWH
{
	DECLARE_DYNCREATE(CACF1WH)

protected:
	

protected:
	//Update
	virtual VOID RegisterVariables();

private:
	virtual CString GetStation() 
	{
		return "ACF1WH_";
	}

public:
	CACF1WH();
	virtual	~CACF1WH();

	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID Operation();
};
