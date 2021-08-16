/////////////////////////////////////////////////////////////////
//	MainFrm.cpp : implementation of the CMainFrame class
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

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)


BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_APP_EXIT, OnClose)
END_MESSAGE_MAP()


static UINT indicators[] =
{
	ID_SEPARATOR, // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};
CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	CMenu CurrentMenu;	// 20140819 Yip: Fix Memory Leak Issue (Use CMenu Instead Of CMenu*)

	CurrentMenu.LoadMenu(IDR_EXIT);
	SetMenu(&CurrentMenu);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
									  sizeof(indicators) / sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT &cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
	{
		return FALSE;
	}

	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style = WS_OVERLAPPED | WS_CAPTION /*| WS_SYSMENU*/ | WS_MINIMIZEBOX | WS_TILED;


	if (cs.hMenu != NULL)
	{
		DestroyMenu(cs.hMenu);
		cs.hMenu = NULL;
	}

	cs.cx = 475;
	cs.cy = 450;

	return TRUE;
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext &dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

void CMainFrame::OnClose() 
{

	CFrameWnd::OnClose();
}
