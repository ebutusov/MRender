#pragma once

#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <math.h>

class CMatrix
{
private:
  GLfloat m_matrix[16];
public:
  CMatrix();

  inline void GetTranslations(GLfloat &x, GLfloat &y, GLfloat &z)
  {
    x = m_matrix[12];
    y = m_matrix[13];
    z = m_matrix[14];
  }
  inline void SetTranslation(GLfloat x, GLfloat y, GLfloat z)
  {
    m_matrix[12] = x;
    m_matrix[13] = y;
    m_matrix[14] = z;
  }

  inline void CMatrix::SetTranslationX(GLfloat x)
  {
    m_matrix[12] = x;
  }

  inline void CMatrix::SetTranslationY(GLfloat y)
  {
    m_matrix[13] = y;
  }

  inline void CMatrix::SetTranslationZ(GLfloat z)
  {
    m_matrix[14] = z;
  }

  void GetMatrix(GLfloat *matrix);

  CMatrix& operator=(CMatrix& other);
  CMatrix& operator=(GLfloat *array16);
  CMatrix& operator*=(CMatrix& other);
  
};