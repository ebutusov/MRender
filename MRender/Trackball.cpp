#include "StdAfx.h"
#include "Trackball.h"

CTrackball::CTrackball(void) :
  m_quat(0.0f, 0.0f, 0.0f, 1.0f)
{
  m_quat.CreateFromAxisAngle(0.0f, 0.0f, 0.0f, 0.0f);
  m_rotation.CreateFromAxisAngle(0.0f, 0.0f, 0.0f, 0.0f);
  m_tracking = false;
}

void CTrackball::SetZoom(GLfloat zoom)
{
  m_matTranslation.SetTranslationZ(zoom);
}
void CTrackball::StartTracking(int mx, int my)
{
  m_start.x = static_cast<float>(mx);
  m_start.y = static_cast<float>(my);
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
	// first rotation, then translation (matrix multiplication applies these in reversed order,
	// so this is translation * rotation)
	// if we reverse order, i.e. res = rotation * translation, we will be rotating _translated_
	// object around the origin
	
	// ok, this needs little more explaining:
	// when we think of global fixed coordinate system, the transformations are applied
	// in reversed order (here: rotation, then translation). the effect is that our object
	// gets rotated (around origin), and then translated (zoom/unzoom) into -Z
	//
	// when we think of object's local coordinate system, the translations seems to apply
	// in order as they appear (here: translation, then rotation). so our object gets translated
	// to the desired point in space, and then it's rotated around it's local origin.
	// code is the same, just a matter of how we think of it.

	CMatrix ret = m_matTranslation * m_matRotation;
	ret.GetMatrix(m_matResult);
  return (GLfloat *)&m_matResult;
}

bool CTrackball::DoTracking(int x, int y)
{
  if (!m_tracking)
    return false;

  Vector2df delta = m_start - Vector2df((float)x, (float)y);
  delta /= 2;

  Vector3df mouse(delta.x, -delta.y, 0.0f);
  Vector3df dep(0.0f, 0.0f, 1.0f);
  Vector3df axis(0.0f, 0.0f, 0.0f);

	// axis will be perpendicular to mouse and dep vectors
	// this is our rotation axis
  axis = mouse ^ dep;
	axis.Normalize();

	// every 100 pixels is 360 degress or rotation
  //GLfloat len = axis.Magnitude() / 100.0f;
  GLfloat len = fabs(delta.Magnitude());
  
	while (len > 360.0f)
		len -= 360.0f;

  ATLTRACE("delta_x : %f, delta_y: %f\n", mouse.x, mouse.y);
  ATLTRACE("axis: x:%f  y:%f  z:%f\n", axis.x, axis.y, axis.z, len);
  ATLTRACE("angle: %f\n", len);
  
  m_rotation_delta.CreateFromAxisAngle(axis.x, axis.y, axis.z, len);
  
  m_quat = m_rotation_delta * m_rotation;
  return true; // still tracking
}

void CTrackball::EndTracking()
{
	// store current rotation state
  m_rotation = m_quat; // m_rotation_delta * m_rotation;
  m_tracking = false;
}
