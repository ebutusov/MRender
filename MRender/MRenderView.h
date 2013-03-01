// MRenderView.h : interface of the CMRenderView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atlopengl.h>
#include "Molecule.h"
#include "Trackball.h"

class CMRenderView : public CWindowImpl<CMRenderView>, COpenGL<CMRenderView>, CUpdateUI<CMRenderView>
{
private:
	GLuint m_font_base;
	LONG m_lTextHeight;
	BOOL m_Keys[256];
	CMolecule *m_pMolecule;
	BOOL m_bShowLinks, m_bShowLabels;
  CTrackball m_track;
	bool m_bPickMode;
	GLfloat m_fAspect;

	void DoSelect(int x, int y);

public:
	DECLARE_WND_CLASS(NULL)

	CMRenderView();

	BOOL PreTranslateMessage(MSG* pMsg);
	BOOL OnIdle();
	void OnInit();
	void OnRender();
	void OnResize(int cx, int cy);
	void LoadMolecule(LPCTSTR filename);

	void SetShowLabels(BOOL enable);
	BOOL GetShowLabels() { return m_bShowLabels; }

	void SetShowLinks(BOOL enable);
	BOOL GetShowLinks() { return m_bShowLinks; }

	BOOL m_bNeedsRedraw;

	void Clear();

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

  LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BOOL GetUpdateFlag();

	BEGIN_MSG_MAP(CMRenderView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
    MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
    MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		CHAIN_MSG_MAP(COpenGL<CMRenderView>)
	END_MSG_MAP()

	BEGIN_UPDATE_UI_MAP(CMainFrame)
	END_UPDATE_UI_MAP()


// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};
