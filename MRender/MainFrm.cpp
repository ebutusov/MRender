// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainFrm.h"

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;
	return m_view.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	if(m_view.GetUpdateFlag())
		m_view.OnIdle();
	else
		Sleep(20);
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	CreateSimpleStatusBar();

	m_wndVertSplit.Create( *this, rcDefault, NULL, 0, WS_EX_CLIENTEDGE);
	m_view.Create(m_wndVertSplit, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	m_TreeView.Create(m_wndVertSplit, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	
	// this didn't work
	//m_TreeView.ModifyStyle(NULL, TVS_HASBUTTONS | TVS_HASLINES | TVS_TRACKSELECT | TVS_SINGLEEXPAND |  TVS_SHOWSELALWAYS);	
	
	LONG lStyle;
	lStyle = ::GetWindowLong(m_TreeView.m_hWnd, GWL_STYLE);
	lStyle |= (TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_DISABLEDRAGDROP | TVS_TRACKSELECT);
	::SetWindowLong(m_TreeView.m_hWnd, GWL_STYLE, lStyle); 

	m_wndVertSplit.SetSplitterPanes ( m_TreeView, m_view );
	m_wndVertSplit.SetSplitterExtendedStyle(SPLIT_PROPORTIONAL & SPLIT_RIGHTALIGNED);
	m_hWndClient = m_wndVertSplit;
	m_wndVertSplit.m_cxyMin = 150;
	m_wndVertSplit.m_cxySplitBar = 1;
	m_wndVertSplit.m_nDefActivePane = 0;
	m_wndVertSplit.m_cxyDragOffset = 0;
	UpdateLayout();
	m_wndVertSplit.SetSplitterPos(150);

	//m_TreeView.SetBkColor(RGB(255, 255, 255));
	//m_TreeView.SetLineColor(RGB(0, 0, 255));
	//m_TreeView.SetTextColor(RGB(255, 255, 255));
	//m_TreeView.SetLineColor(RGB(0, 0, 0));
	m_TreeView.FillTree(_T("molecules"));

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);
	UISetCheck(ID_VIEW_LINKS, 1);
	m_view.SetShowLinks(true);
	UISetCheck(ID_VIEW_LABELS, 0);
	m_view.SetShowLabels(false);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);
	bHandled = FALSE;		// pass message to the chain
	return 0;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

#define _APPTITLE _T("MRender")

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	m_view.Clear();
	SetWindowText(_APPTITLE);
	bHandled = TRUE;
	return 0;
}

LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	static TCHAR filter[] = _T("PDB files (*.pdb)\0*.pdb\0All files (*.*)\0*.*\0\0");
	CFileDialog cfd(TRUE, NULL, NULL, 4|2, filter, this->m_hWnd);
	cfd.m_ofn.Flags |= OFN_NOCHANGEDIR;
	if(cfd.DoModal() == IDOK)
	{
		m_view.LoadMolecule(cfd.m_ofn.lpstrFile);
		CString title;
		title.Format(_T("%s: %s"), _APPTITLE, cfd.m_ofn.lpstrFileTitle);
		SetWindowText(title);
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CMainFrame::OnViewShowLabels(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	bool showLabels = m_view.GetShowLabels();
	showLabels = !showLabels;
	m_view.SetShowLabels(showLabels);
	UISetCheck(ID_VIEW_LABELS, showLabels);
	UpdateLayout();
	bHandled = TRUE;
	return 0;
}


LRESULT CMainFrame::OnViewShowLinks(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	bool showLinks = m_view.GetShowLinks();
	showLinks = !showLinks;
	m_view.SetShowLinks(showLinks);
	UISetCheck(ID_VIEW_LINKS, showLinks);
	UpdateLayout();
	bHandled = TRUE;
	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnTreeSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NMTREEVIEW *trv = (NMTREEVIEW*) pnmh;
	TCHAR t[32];
	m_TreeView.GetItemText(trv->itemNew.hItem, t, 31);
	HTREEITEM hti = trv->itemNew.hItem;

	if(m_TreeView.ItemHasChildren(hti))
		return 0; // this is a folder, don't process it

	CString path = _T("molecules\\");
	
	HTREEITEM lxti;
	while((lxti = m_TreeView.GetParentItem(hti)) != NULL)
	{
		hti = lxti;
		TCHAR text[32];
		m_TreeView.GetItemText(lxti, text, 31);
		path = path + text + "\\";
	}
	path = path + t + _T(".pdb");
	m_view.LoadMolecule(path);
	return 0;
}
