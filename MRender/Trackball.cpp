#include "StdAfx.h"
#include "Trackball.h"

CTrackball::CTrackball(void) :
  m_quat(0.0f, 0.0f, 0.0f, 1.0f)
{
  m_quat.CreateFromAxisAngle(0.0f, 0.0f, 0.0f, 0.0f);
  m_rotation.CreateFromAxisAngle(0.0f, 0.0f, 0.0f, 0.0f);
  m_tracking = false;
}

CTrackball::~CTrackball(void)
{

}

void CTrackball::SetZoom(GLfloat zoom)
{
  m_matTranslation.SetTranslationZ(zoom);
}
void CTrackball::StartTracking(int mx, int my)
{
  m_start.x = mx;
  m_start.y = my;
  m_tracking = true;
}

void CTrackball::DoZoom(float delta)
{
  GLfloat x, y, z;
  m_matTranslation.GetTranslations(x, y, z);
  m_matTranslation.SetTranslationZ(z + delta);
}

GLfloat* CTrackball::GetMatrix()
{
  ATLTRACE("GetMatrix\n");
  GLfloat m[16];
  m_quat.CreateMatrix(m);
  m_matRotation = m;
  m_matRotation *= m_matTranslation;
  m_matRotation.GetMatrix(m_matResult);
  return (GLfloat *)&m_matResult;
}

BOOL CTrackball::DoTracking(int x, int y)
{
  if (!m_tracking)
    return false;

  Vector2d delta = m_start - Vector2d(x, y);
  delta /= 2;

  Vector3d mouse(delta.x, -delta.y, 0.0f);
  Vector3d dep(0.0f, 0.0f, 1.0f);
  Vector3d axis(0.0f, 0.0f, 0.0f);

  axis = mouse ^ dep;
  
  GLfloat len = axis.Magnitude() / 100.0f;
  
  ATLTRACE("delta_x : %f, delta_y: %f", mouse.x, mouse.y);
  ATLTRACE("axis: x:%f  y:%f  z%f\n", axis.x, axis.y, axis.z);
  
  m_rotation_delta.CreateFromAxisAngle(axis.x, axis.y, axis.z, len);
  
  m_quat = m_rotation_delta * m_rotation;
  return TRUE; // still tracking
}

void CTrackball::EndTracking()
{
  m_rotation = m_rotation_delta * m_rotation;
  m_tracking = false;
}
