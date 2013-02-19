#pragma once

#include <atlopengl.h>

class CAtom
{
public:
	CAtom(void);
	CAtom(TCHAR * symbol);
	void SetCoords(GLfloat x, GLfloat y, GLfloat z);
	void SetColor(GLfloat r, GLfloat g, GLfloat b);
	void GetColor(GLfloat *rColor);
	BOOL SetName(TCHAR *name);
	void SetScaledSize(GLfloat size);
	GLfloat GetScaledSize();
	GLfloat GetX() { return m_XCoord; }
	GLfloat GetY() { return m_YCoord; }
	GLfloat GetZ() { return m_ZCoord; }
	GLfloat GetSize() { return m_Size; } 
public:
	virtual ~CAtom(void);
private:

	TCHAR m_Name[4];
	TCHAR m_FullName[20];

	GLfloat m_XCoord;
	GLfloat m_YCoord;
	GLfloat m_ZCoord;

	GLfloat m_Size;
	GLfloat m_ScaledSize;

	GLfloat m_ColorR;
	GLfloat m_ColorG;
	GLfloat m_ColorB;

	typedef struct _ATOMDEFAULTS
	{
		TCHAR name[4];
		TCHAR full_name[20];
		GLfloat size;
		GLfloat color[4];
	} atom_defaults;

	static atom_defaults m_AtomDefaults[];

	BOOL LoadDefaults();

};
