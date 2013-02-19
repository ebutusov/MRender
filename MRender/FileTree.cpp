#include "StdAfx.h"
#include "FileTree.h"
#include "resource.h"

CFileTree::CFileTree(void)
{
}

CFileTree::~CFileTree(void)
{
}

/// Callback for tree items sorting.
template<class T>
int CALLBACK
_TVICompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CString text1, text2;
	T* tvc = (T*)lParamSort;
	HTREEITEM first = (HTREEITEM)lParam1;
	HTREEITEM second = (HTREEITEM)lParam2;
	tvc->GetItemText(first, text1);
	tvc->GetItemText(second, text2);
	if(tvc->ItemHasChildren(first) && !tvc->ItemHasChildren(second))
		return -1;
	if(!tvc->ItemHasChildren(first) && tvc->ItemHasChildren(second))
		return 1;
	return _tcscmp(text1, text2);
}

LRESULT
CFileTree::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	m_ImageList.Create(IDB_FILELIST, 18, 0, RGB(255,0,255));
	bHandled = FALSE;		// base class will do the rest
	return 0;
}

/// Sets images for collapsed and expanded item.
/// Reflect notifications involved here.
LRESULT CFileTree::OnItemExpanding(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
	NMTREEVIEW *nmt = (LPNMTREEVIEW)pnmh;
	if(nmt->action & TVE_EXPAND)
		this->SetItemImage(nmt->itemNew.hItem, 1, 1);
	else
		this->SetItemImage(nmt->itemNew.hItem, 0, 0);
	bHandled = TRUE;
	return TRUE;
}

void
CFileTree::FillTree(LPCTSTR startDir)
{
	this->SetImageList(m_ImageList.m_hImageList);
	CFindFile finder;
	ProcessDir(startDir, TVI_ROOT);
	TVSORTCB tvs;
	tvs.hParent = NULL;
	tvs.lParam = (LPARAM)static_cast<CFileTree *>(this);		// 5#337
	tvs.lpfnCompare = ::_TVICompareProc<CFileTree>;
	this->SortChildrenCB(&tvs, FALSE);
	//this->SortChildren(TVI_ROOT, TRUE);
}

/// Recursively process directory.
void
CFileTree::ProcessDir(LPCTSTR dir, HTREEITEM item)
{
	CFindFile finder;
	CString pattern;
	pattern.Format("%s\\*.*", dir);
	if(finder.FindFile(pattern))
	{
		do
		{
			if(finder.IsDots())
				continue;
			HTREEITEM it = this->InsertItem(finder.GetFileTitle(), item, NULL);
			if(finder.IsDirectory())
			{
				this->SetItemImage(it, 0, 0);		// we'll handle expanding and collapsing to set proper image
				this->SetItemData(it, (DWORD_PTR)it);	// store item's handle as the data
				ProcessDir((LPTSTR)(LPCSTR)finder.GetFilePath(), it);
			}
			else
			{
				this->SetItemImage(it, 2, 2);
				this->SetItemData(it, (DWORD_PTR)it);	// store item's handle as the data
			}
		} while(finder.FindNextFileA());
	}
	finder.Close();
}
