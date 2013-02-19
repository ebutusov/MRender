#pragma once

#include <atlcoll.h>
#include <atlopengl.h>
#include "Atom.h"

class CMolecule
{
public:
	CMolecule(void);
	void CreateAtom(GLfloat x, GLfloat y, GLfloat z, TCHAR *atomName, int number);
	void DrawYourself();

	void PutLink(int from, int to);
	void EnableLinks(BOOL enable);

	void SetDescription(LPTSTR desc);
	LPTSTR GetDescription();
	GLfloat GetMaxDimension();
	int GetAtomsCount();
	void CalculateBoundingBox();

	virtual ~CMolecule(void);

protected:
	BOOL m_bDrawLinks;
	static GLfloat LINKCOLOR[4];
	LPTSTR m_Description;
	GLfloat TRANSLATIONS[3];
	GLfloat m_Width, m_Height, m_Depth;
	GLfloat m_Scale, m_ElementScale;

	void RescaleAtoms();

	typedef CAtom* AtomPtr;
	CAtlMap<int, AtomPtr> m_Atoms;

	typedef struct _ATOMLINK
	{
		int from;		// source atom's number
		int to;			// destination's number
		int strength; // how many links 
	} ATOMLINK;
	
	typedef ATOMLINK* ATOMLINKPTR;
	CAtlArray<ATOMLINKPTR> m_AtomLinks;
};
