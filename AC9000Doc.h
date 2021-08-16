/////////////////////////////////////////////////////////////////
//	AC9000Doc.h : interface of the CAC9000Doc class
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


class CAC9000Doc : public CDocument
{
protected: // create from serialization only
	CAC9000Doc();
	DECLARE_DYNCREATE(CAC9000Doc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive &ar);

// Implementation
public:
	virtual ~CAC9000Doc();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
