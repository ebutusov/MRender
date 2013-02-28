// MRenderView.cpp : implementation of the CMRenderView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "GLDrawHelper.h"
#include "MRenderView.h"
#include "MoleculeBuilder.h"

CMRenderView::CMRenderView()
	: m_font_base(0),
	m_pMolecule(NULL),
	m_bShowLinks(false),
	m_bShowLabels(false)
{
}

BOOL CMRenderView::PreTranslateMessage(MSG* pMsg)
{
	bool redrawScene = FALSE;
	switch(pMsg->message)
	{
		case WM_KEYUP:
			{
				this->m_Keys[pMsg->wParam] = FALSE;
				redrawScene = TRUE;
				break;
			}
		case WM_KEYDOWN:
			{
				this->m_Keys[pMsg->wParam] = TRUE;
				redrawScene = TRUE;
				break;
			}
		default:
		{
			break;
		}
	}

	if(redrawScene)
	{
		RedrawWindow();
		return TRUE;
	}

	return FALSE;
}

BOOL
CMRenderView::GetUpdateFlag()
{
	if(m_pMolecule && m_bNeedsRedraw)
		return TRUE;
	return FALSE;
}

void CMRenderView::OnInit()
{
	m_bNeedsRedraw = TRUE;
  m_track.SetZoom(-20.0f);

	// clear key flags
	for(int i=0;i<256;i++)
		this->m_Keys[i] = FALSE;

	glClearColor(0.000f, 0.000f, 0.000f, 1.0f); //Background color

	// Activate lighting and a light source
	glMatrixMode(GL_MODELVIEW);
	glMatrixMode(GL_PROJECTION);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// TODO: dynamic light calculation should be done here
	//static GLfloat pos[4] = {1.0f, 0.4f, 0.9f, 0.0f};
	//static GLfloat amb[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  //static GLfloat dif[4] = {0.8f, 0.8f, 0.8f, 1.0f};
  //static GLfloat spc[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	static GLfloat pos[4] = {-1.0f, 0.4f, 0.9f, 0.0f}; 
	static GLfloat amb[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  static GLfloat dif[4] = {0.5f, 0.5f, 0.5f, 1.0f};
  static GLfloat spc[4] = {0.3f, 0.3f, 0.3f, 1.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  dif);
  //glLightfv(GL_LIGHT0, GL_SPECULAR, spc);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	// font display list
	HFONT newFont = CreateFont(-18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			ANSI_CHARSET, 0, 0,
			ANTIALIASED_QUALITY, 0, _T("Courier New"));
	
	if(!newFont)
		MessageBox(_T("Cannot create font!"), _T("Error"), MB_OK|MB_ICONERROR);
	else
	{
		CDC dc(this->GetDC());
		dc.SelectFont(newFont);
		TEXTMETRIC tm;
		BOOL ok = GetTextMetrics(dc.m_hDC, &tm);
		if(ok)
			m_lTextHeight = tm.tmHeight;
		else
			m_lTextHeight = 0;
		//HFONT oldFont = (HFONT)SelectObject(this->GetDC(), newFont);
		//GLuint base = glGenLists(96);
		m_font_base = 1000;
		wglUseFontBitmaps(dc.m_hDC, 0, 255, m_font_base);
	}
	//SelectObject(this->GetDC(), oldFont);
	//DeleteObject(newFont);

	// Define material parameters
	//static GLfloat glfMatAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f};
	//static GLfloat glfMatDiffuse[] = {0.5f, 0.5f, 0.580f, 1.0f};
	//static GLfloat glfMatSpecular[]= {1.000f, 1.000f, 1.000f, 1.0f};
	//static GLfloat glfMatEmission[]= {0.000f, 0.000f, 0.000f, 1.0f};
	//static GLfloat fShininess = 128.000f;
	//static GLfloat fShininess = 20.0f;
	// Set material parameters
	//glMaterialfv(GL_FRONT, GL_AMBIENT,  glfMatAmbient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE,  glfMatDiffuse);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, glfMatSpecular);
	//glMaterialfv(GL_FRONT, GL_EMISSION, glfMatEmission);
	//glMaterialf(GL_FRONT, GL_SHININESS, fShininess);

	//CMoleculeBuilder builder;
	//m_pMolecule = builder.LoadFromFile("nicotine.pdb");
	//if(m_pMolecule != NULL)
	//	m_pMolecule->EnableLinks(TRUE);
	//else
	//	MessageBox("Failed to load molecule!", "ERROR", MB_OK);
}

BOOL CMRenderView::OnIdle()
{
	static bool reverse_angle = TRUE;
	RedrawWindow();
	return FALSE;
}


LRESULT CMRenderView::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
  SetFocus();
  ::SetCapture(this->m_hWnd);
  int x = GET_X_LPARAM(lParam);
  int y = GET_Y_LPARAM(lParam);
  m_track.StartTracking(x, y);
  bHandled = TRUE;
  return 0;
}
LRESULT CMRenderView::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
  m_track.EndTracking();
  ::ReleaseCapture();
  bHandled = TRUE;
  return 0;
}

LRESULT CMRenderView::OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	int x_pos = GET_X_LPARAM(lParam);
	int y_pos = GET_Y_LPARAM(lParam);
  if (m_track.DoTracking(x_pos, y_pos))
    RedrawWindow();
	bHandled = TRUE;
	return 0;
}

LRESULT CMRenderView::OnMouseWheel(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	m_track.DoZoom(zDelta / 100.0f);
	RedrawWindow();
	bHandled = TRUE;
	return 0;
}

void
CMRenderView::LoadMolecule(LPCTSTR filename)
{
	if(m_pMolecule != NULL)
	{
		delete m_pMolecule;
		m_pMolecule = NULL;
	}
	CMoleculeBuilder builder;
	m_pMolecule = builder.LoadFromFile(filename);
	if(m_pMolecule == NULL)
	{
		MessageBox(_T("Failed to load molecule!"), _T("Error"), MB_OK | MB_ICONERROR);
	}
	else
	{
		m_pMolecule->EnableLinks(m_bShowLinks);
		m_pMolecule->SetFontList(m_font_base);
		m_pMolecule->EnableLabels(m_bShowLabels);
		m_pMolecule->GenerateFormula();
		RedrawWindow();
	}
}

void
CMRenderView::Clear()
{
	if(m_pMolecule != NULL)
	{
		delete m_pMolecule;
		m_pMolecule = NULL;
		RedrawWindow();
	}
}

void CMRenderView::SetShowLinks(BOOL enable)
{
	if(m_pMolecule)
	{
		m_pMolecule->EnableLinks(enable);
		RedrawWindow();
	}
	m_bShowLinks = enable;
}

void CMRenderView::SetShowLabels(BOOL enable)
{
	if(m_pMolecule)
	{
		m_pMolecule->EnableLabels(enable);
		RedrawWindow();
	}
	m_bShowLabels = enable;
}

LRESULT CMRenderView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// register object for message filtering and idle updates
	m_pMolecule = NULL;
	bHandled = FALSE;	// pass msg to the base class (chained)
	return 0;
}

void
DrawSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat diameter, BOOL wireframe)
{
	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(diameter, diameter, diameter);
	CGLDrawHelper::DrawSphere(12,24, wireframe);
	glPopMatrix();
}

void
DrawTube(GLfloat x1, GLfloat y1, GLfloat z1, 
				 GLfloat x2, GLfloat y2, GLfloat z2,
				 GLfloat diameter, bool wire)
{
	CGLDrawHelper::DrawTube(x1, y1, z1, x2, y2, z2, diameter, 0.0f, 30, TRUE, FALSE, wire);
}

void
SetItemColor(GLfloat r, GLfloat g, GLfloat b, GLfloat alpha)
{
	static GLfloat col[4];
	col[0] = r; col[1] = g; col[2] = b; col[3] = alpha;
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, col); 
}

void CMRenderView::OnRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear buffers
  glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(m_pMolecule)
	{
		GLfloat tx,ty,tz;
		m_pMolecule->GetTranslations(tx, ty, tz);
    glLoadMatrixf(m_track.GetMatrix());
    glTranslatef(tx, ty, tz);
		m_pMolecule->Draw();
		RECT rect;
    this->GetClientRect(&rect);
		glColor3f(0.48f, 0.8f, 0.44f);
		CGLDrawHelper::DrawString(m_font_base, rect.right, rect.bottom, 
			0.1f, (rect.bottom-rect.top) - m_lTextHeight,
			m_pMolecule->GetDescription(), m_lTextHeight);
		CGLDrawHelper::DrawString(m_font_base, rect.right, rect.bottom,
			0.1f, 0.1f + m_lTextHeight/2.0f, m_pMolecule->GetFormula(), m_lTextHeight); 
	}
	glFlush();
	m_bNeedsRedraw = FALSE;
}

void CMRenderView::OnResize(int cx, int cy) 
{
	GLfloat fFovy  = 45.0f; // Field-of-view
	GLfloat fZNear = 1.0f;  // Near clipping plane
	GLfloat fZFar = 100.0f;  // Far clipping plane

	// Calculate viewport aspect
	RECT rc;
	GetClientRect(&rc);

	GLfloat fAspect = (GLfloat)(rc.right-rc.left) / (GLfloat)(rc.bottom-rc.top);

	// Define viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fFovy, fAspect, fZNear, fZFar);
	glViewport(rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top);
	glMatrixMode(GL_MODELVIEW);
}

