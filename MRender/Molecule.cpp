#include "StdAfx.h"
#include "GLDrawHelper.h"
#include "Molecule.h"


void
DrawSphereM(GLfloat x, GLfloat y, GLfloat z, GLfloat diameter, BOOL wireframe, GLfloat scale)
{
	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(diameter, diameter, diameter);
	// 12, 24
	CGLDrawHelper::DrawSphere(12*scale,24*scale, wireframe);
	glPopMatrix();
}

void
DrawTubeM(GLfloat x1, GLfloat y1, GLfloat z1, 
				 GLfloat x2, GLfloat y2, GLfloat z2,
				 GLfloat diameter, bool wire, GLfloat scale)
{
	CGLDrawHelper::DrawTube(x1, y1, z1, x2, y2, z2, diameter*scale, 0.0f, 30, TRUE, FALSE, wire);
}

////////////////////////////////////////////////////////////////////////////////////////
// MOLECULE IMPLEMENTATION
////////////////////////////////////////////////////////////////////////////////////////


#define FOREACH_ATOM() \
	POSITION pos = m_Atoms.GetStartPosition(); \
	int num; \
	AtomPtr atom; \
	while(pos != NULL) \
	{

#define END_FOREACH_ATOM() }


GLfloat CMolecule::LINKCOLOR[4] = { 0.3f, 0.3f, 0.2f, 1.0f };

CMolecule::CMolecule(void)
{
	m_bDrawLinks = FALSE;
	m_Description = NULL;
}

CMolecule::~CMolecule(void)
{
	POSITION pos = m_Atoms.GetStartPosition();
	int num;
	AtomPtr atom;
	while(pos != NULL)
	{
		num = m_Atoms.GetKeyAt(pos);
		atom = m_Atoms.GetNextValue(pos);
		delete atom;
	}
	m_Atoms.RemoveAll();
	
	for(unsigned int i=0;i<m_AtomLinks.GetCount();i++)
	{
		ATOMLINKPTR link = m_AtomLinks.GetAt(i);
		delete link;
	}
	m_AtomLinks.RemoveAll();
	if(m_Description)
		delete m_Description;
}

void
CMolecule::SetDescription(LPTSTR desc)
{
	m_Description = new TCHAR[_tcslen(desc)+1];
	_tcsncpy_s(m_Description, _tcslen(desc)+1, desc, _tcslen(desc));
}

LPTSTR
CMolecule::GetDescription()
{
	if(m_Description)
		return m_Description;
	else return "UNKNOWN";
}

int
CMolecule::GetAtomsCount()
{
	return m_Atoms.GetCount();
}

GLfloat
CMolecule::GetMaxDimension()
{
	return m_Scale;
}

void
CMolecule::EnableLinks(BOOL enable)
{
	m_bDrawLinks = enable;
	if(enable)
		RescaleAtoms();
}

void
CMolecule::PutLink(int from, int to)
{
	if(m_AtomLinks.GetCount() == 0)
	{
		ATOMLINKPTR alink = new ATOMLINK();
		alink->from = from;
		alink->to = to;
		alink->strength = 1;	// one link, normal strength
													// if there will be more links between those two atoms,
													// strenght will be increased
		m_AtomLinks.Add(alink);
	}
	else
	{
		// we need to check for duplicates before inserting this link
		BOOL already_defined = FALSE;
		for(int i=0; i<m_AtomLinks.GetCount(); i++)
		{
			ATOMLINKPTR link = m_AtomLinks.GetAt(i);
			if((link->from == from && link->to == to))
			{
				link->strength++;
				already_defined = TRUE;
				break; // multi link between two atoms
			}
			if(link->to == from && link->from == to)
				already_defined = TRUE;	// reverse link already exists, don't add
		}
		if(!already_defined)
		{
			ATOMLINKPTR alink = new ATOMLINK();
			alink->from = from;
			alink->to = to;
			alink->strength = 1;
			m_AtomLinks.Add(alink);
		}
	}
}

void
CMolecule::RescaleAtoms()
{
	FOREACH_ATOM()
		//num = m_Atoms.GetKeyAt(pos);
		atom = m_Atoms.GetNextValue(pos);
		GLfloat bot = 0.4;
    GLfloat top = 0.6;
    GLfloat min = 1.17;
    GLfloat max = 1.80;
    GLfloat ratio = (atom->GetSize() - min) / (max - min);
    atom->SetScaledSize(bot + (ratio * (top - bot)));
	END_FOREACH_ATOM()
}

/// Large molecules are not always initally visible beacuse
/// of their size. This function calculates minimal bounding
/// box, which would be a base for rescaling and translating.
/// Molecule is centered in the middle of the box.
void
CMolecule::CalculateBoundingBox()
{
	if(m_Atoms.GetCount() == 0)
	{
		for(int i=0;i<3;i++)
			TRANSLATIONS[i] = 0.0f;
		m_Depth = 0.0f;
		m_Width = 0.0f;
		m_Height = 0.0f;
		return;
	}		

	GLfloat x1, y1, z1, x2, y2, z2;
	POSITION p = m_Atoms.GetStartPosition();
	AtomPtr firstAtom = m_Atoms.GetValueAt(p);
	x1 = x2 = firstAtom->GetX();
	y1 = y2 = firstAtom->GetY();
	z1 = z2 = firstAtom->GetZ();

	FOREACH_ATOM()
		atom = m_Atoms.GetNextValue(pos);
		if(atom->GetX() < x1) x1 = atom->GetX();
		if(atom->GetY() < y1) y1 = atom->GetY();
		if(atom->GetZ() < z1) z1 = atom->GetZ();
		
		if(atom->GetX() > x2) x2 = atom->GetX();
		if(atom->GetY() > y2) y2 = atom->GetY();
		if(atom->GetZ() > z2) z2 = atom->GetZ();
	END_FOREACH_ATOM()

	m_Width = x2-x1;
	m_Height = y2-y1;
	m_Depth = z2-z1;

	GLfloat size = m_Width > m_Height ? m_Width : m_Height;
	size = size > m_Depth ? size : m_Depth;
	m_Scale = size;		// 7 is empiric ;-)

	TRANSLATIONS[0] = -(x1+m_Width/2);	// X translation
	TRANSLATIONS[1] = -(y1+m_Height/2);	// Y translation
	TRANSLATIONS[2] = -(z1+m_Depth/2);	// Z translation

	const static int scale_after_that = 100;
	const static int max_atoms = 1000;
	
	int cnt = m_Atoms.GetCount();
	if(cnt >= max_atoms)
	{
		m_ElementScale = 0.2f; // exception
		return;
	}

	if(cnt > scale_after_that)
	{
		// max scale is 1.0f, min scale is 0.2f
		GLfloat ratio = (GLfloat)(cnt-scale_after_that)/(max_atoms-scale_after_that);
		m_ElementScale = 1.0f - (ratio * (1.0f-0.2f));
	}
	else
		m_ElementScale = 1.0f;
}

void
CMolecule::CreateAtom(GLfloat x, GLfloat y, GLfloat z, TCHAR *atomName, int number)
{
	CAtom *a = new CAtom(atomName);
	a->SetCoords(x, y, z);
	m_Atoms.SetAt(number, a);
}

void
CMolecule::DrawYourself()
{
	if(m_Atoms.GetCount() == 0)
		return;

	// apply scaling and translations, if needed
	//glScalef(m_Scale, m_Scale, m_Scale);
	glTranslatef(TRANSLATIONS[0], TRANSLATIONS[1], TRANSLATIONS[2]);
	

	FOREACH_ATOM()
		num = m_Atoms.GetKeyAt(pos);
		atom = m_Atoms.GetNextValue(pos);
		GLfloat color[4];
		atom->GetColor(color);
		color[3] = 1.0f;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
		GLfloat size = m_bDrawLinks ? atom->GetScaledSize() : atom->GetSize();
		DrawSphereM(atom->GetX(), atom->GetY(), atom->GetZ(), size, FALSE, m_ElementScale);
	END_FOREACH_ATOM()

	if(m_bDrawLinks)
	{
		// set default link color
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, LINKCOLOR);
		for(int i=0;i<m_AtomLinks.GetCount(); i++)
		{
			ATOMLINKPTR link = m_AtomLinks.GetAt(i);
			CAtom *fromAtom;
			m_Atoms.Lookup(link->from, fromAtom);
			CAtom *toAtom;
			m_Atoms.Lookup(link->to, toAtom);
			if(toAtom && fromAtom)
			{
				DrawTubeM(fromAtom->GetX(), fromAtom->GetY(), fromAtom->GetZ(),
					toAtom->GetX(), toAtom->GetY(), toAtom->GetZ(), link->strength*0.1f, FALSE, m_ElementScale);
			}
		}
	}
}

