/////////////////////////////////////////////////////////////////
//	ACF1.cpp : interface of the CACF1 class
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

#include "ACF.h"


class CACF1 : public CACF
{
	DECLARE_DYNCREATE(CACF1)

protected:
	

protected:
	//Update
	VOID UpdateProfile();
	VOID UpdatePosition();

	virtual VOID RegisterVariables();

private:
	virtual CString GetStation() 
	{
		return "ACF1_";
	}

public:
	CACF1();
	virtual	~CACF1();

	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID Operation();
};
