/////////////////////////////////////////////////////////////////
//	AC9000Doc.cpp : implementation of the CAC9000Doc class
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
#include "AC9000.h"

#include "AC9000Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////
// CAC9000Doc
/////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CAC9000Doc, CDocument)


BEGIN_MESSAGE_MAP(CAC9000Doc, CDocument)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////
// CAC9000Doc construction/destruction
/////////////////////////////////////////////////////////////////

CAC9000Doc::CAC9000Doc()
{
	// TODO: add one-time construction code here

}

CAC9000Doc::~CAC9000Doc()
{
}

BOOL CAC9000Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
	{
		return FALSE;
	}

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}


/////////////////////////////////////////////////////////////////
// CAC9000Doc serialization
/////////////////////////////////////////////////////////////////

void CAC9000Doc::Serialize(CArchive &ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


/////////////////////////////////////////////////////////////////
// CAC9000Doc diagnostics
/////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void CAC9000Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAC9000Doc::Dump(CDumpContext &dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////
// CAC9000Doc commands
/////////////////////////////////////////////////////////////////
