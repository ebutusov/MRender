#pragma once
#include "Vectors.h"
#include "Quaternion.h"
#include "Matrix.h"

class CTrackball
{
private:
  CQuaternionf m_quat;
  CQuaternionf m_rotation_delta;
  CQuaternionf m_rotation;
  Vector2df m_start;
  CMatrix m_matRotation;
  CMatrix m_matTranslation;
  GLfloat m_matResult[16];
  bool m_tracking;
public:
  CTrackball(void);

  void SetZoom(GLfloat zoom);
  void StartTracking(int mouse_x, int mouse_y);
  GLfloat* GetMatrix();
  void DoZoom(float delta);
  BOOL DoTracking(int x, int y);
  void EndTracking();
};
