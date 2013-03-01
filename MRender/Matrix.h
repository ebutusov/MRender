#pragma once
#include <math.h>

/* GL Matrix layout:

			[  0 R] [  4 R] [  8 R] [  12 T]
			[  1 R] [  5 R] [  9 R] [  13 T]
			[  2 R] [  6 R] [ 10 R] [  14 T]
			[  3  ] [  7  ] [ 11  ] [  15  ]
*/


class CMatrix
{
private:
  GLfloat m_matrix[16];
public:
	CMatrix()
	{
		SetIdentity();
	}

	CMatrix(bool load_identity)
	{
		if (load_identity)
			SetIdentity();
		else
			memset(&m_matrix, 0, sizeof(m_matrix));
	}

	inline void SetIdentity()
	{
		// clear and set identity matrix
		memset(&m_matrix, 0, sizeof(m_matrix));
		m_matrix[0] = m_matrix[5] = m_matrix[10] = 1.0f; // rotations
		m_matrix[15] = 1.0f; // scale
	}

	CMatrix(const CMatrix &other)
	{
		other.GetMatrix(m_matrix);
	}

	// basic operations
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

  inline void SetTranslationX(GLfloat x)
  {
    m_matrix[12] = x;
  }

  inline void SetTranslationY(GLfloat y)
  {
    m_matrix[13] = y;
  }

  inline void SetTranslationZ(GLfloat z)
  {
    m_matrix[14] = z;
  }

	inline void GetMatrix(GLfloat *matrix) const
	{
		if (matrix)
			memcpy(matrix, m_matrix, sizeof(m_matrix));
	}

	// operators

	const CMatrix operator*(const CMatrix &other)
	{
		CMatrix n(*this);
		n *= other;
		return n;
	}

	CMatrix& operator*=(const CMatrix &other)
	{
		GLfloat m2[16], res[16];
		other.GetMatrix(m2);
		memset(res, 0, sizeof(res));

		for (int i=0;i<4;++i)
		{
			for (int j=0;j<16;j+=4)
			{
				res[i + j] =	m_matrix[i] * m2[j] + 
											m_matrix[i+4] * m2[j+1] +
											m_matrix[i+8] * m2[j+2] + 
											m_matrix[i+12] * m2[j+3];
			}
		}
		memcpy(m_matrix, res, sizeof(m_matrix));
		return *this;
	}

	CMatrix& operator=(const CMatrix& other)
	{
		other.GetMatrix(m_matrix);
		return *this;
	}

	CMatrix& operator=(GLfloat *array16)
	{
		memcpy(m_matrix, array16, sizeof(m_matrix));
		return *this;
	}  
};