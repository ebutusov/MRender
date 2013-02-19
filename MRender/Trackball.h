#pragma once
#include "vect.h"
#include "glQuaternion.h"
#include "Matrix.h"

class CTrackball
{
private:
  glQuaternion m_quat;
  glQuaternion m_rotation_delta;
  glQuaternion m_rotation;
  Vector2d m_start;
  CMatrix m_matRotation;
  CMatrix m_matTranslation;
  GLfloat m_matResult[16];
  bool m_tracking;
  //D3DXMATRIX m_matTranslation;
  //D3DXMATRIX m_matRotation;
public:
  CTrackball(void);
  virtual ~CTrackball(void);

  void SetZoom(GLfloat zoom);
  void StartTracking(int mouse_x, int mouse_y);
  GLfloat* GetMatrix();
  void DoZoom(float delta);
  BOOL DoTracking(int x, int y);
  void EndTracking();
};
