#include "stdafx.h"
#include "Matrix.h"

CMatrix::CMatrix()
{
  // clear and set identity matrix
  memset(&m_matrix, 0, sizeof(m_matrix));
  m_matrix[0] = m_matrix[5] = m_matrix[10] = 1.0f; // rotations
  m_matrix[15] = 1.0f; // scale
}

inline void CMatrix::GetMatrix(GLfloat *matrix)
{
  if (matrix)
    memcpy(matrix, m_matrix, sizeof(m_matrix));
}

CMatrix& CMatrix::operator*=(CMatrix &other)
{
  GLfloat m2[16], res[16];
  other.GetMatrix(m2);
  int x = sizeof(res);
  memset(res, 0, sizeof(res));

  for (int i=0;i<16;i+=4)
  {
    for (int j=0;j<4;j++)
    {
      res[i + j] = m_matrix[i] * m2[j] + m_matrix[i+1] * m2[j+4] +
                        m_matrix[i+2] * m2[j+8] + m_matrix[i+3] * m2[j + 12];
    }
  }
  memcpy(m_matrix, res, sizeof(m_matrix));
  return *this;
}

CMatrix& CMatrix::operator=(CMatrix& other)
{
  GLfloat m2[16];
  other.GetMatrix(m2);
  
  memcpy(m_matrix, m2, sizeof(m2));
  return *this;
}

CMatrix& CMatrix::operator=(GLfloat *array16)
{
  memcpy(m_matrix, array16, sizeof(m_matrix));
  return *this;
}