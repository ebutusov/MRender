#pragma once

#include <atlmisc.h>

class CFileTree : public CWindowImpl<CFileTree, CTreeViewCtrl>
{
	//DECLARE_WND_SUPERCLASS(NULL, CTreeViewCtrl::GetWndClassName()
public:
	void FillTree(LPCTSTR startDir);

private:
	CImageList m_ImageList;
	void ProcessDir(LPCTSTR dir, HTREEITEM it);

public:
	//DECLARE_EMPTY_MSG_MAP()
	BEGIN_MSG_MAP(CFileTree)
		//CHAIN_MSG_MAP(CWindowImpl<CFileTree, CTreeViewCtrl>)
		MESSAGE_HANDLER(WM_CREATE, OnCreate);
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_ITEMEXPANDED, OnItemExpanding)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

private:
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnItemExpanding(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
};
