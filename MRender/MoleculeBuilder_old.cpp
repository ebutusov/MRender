#include "StdAfx.h"
#include "MoleculeBuilder.h"
#include "atlwfile.h"
#include "atlmisc.h"

CMoleculeBuilder::CMoleculeBuilder(void)
{
}

CMoleculeBuilder::~CMoleculeBuilder(void)
{
}

CMolecule*
CMoleculeBuilder::LoadFromFile(LPCTSTR filename)
{
	CFile f;
	if(f.Open(filename) != TRUE)
		return NULL;

	DWORD size = f.GetSize();
	m_FileBuff = new TCHAR[size+1];
	f.Read(m_FileBuff, size);
	m_FileBuff[size] = '\0';
	f.Close();
	CMolecule *mol = new CMolecule();
	BuildMolecule(mol);
	mol->CalculateBoundingBox();
	return mol;
}

void
CMoleculeBuilder::BuildMolecule(CMolecule *mol)
{
	TCHAR *wptr = m_FileBuff;
	TCHAR *lptr = m_FileBuff;
	int lineLen = 0;
	BOOL done = FALSE;
	while(!done)
	{
		lineLen = 0;
		while(*wptr != '\n' && *wptr != '\0')
		{
			wptr++;
			lineLen++;
		}
		if(*wptr == '\0')
		{
			done = TRUE;
			break;
		}
		else
		{
			wptr++;
			lineLen++;
		}
		TCHAR *lineBuff = new TCHAR[lineLen+1];
		TCHAR *p = lineBuff;
		while(lptr != wptr)
		{
			*(lineBuff++) = *(lptr++);
		}
		*lineBuff = '\0';
		lineBuff = p;
		
		// parse line buffer

		//MessageBox(NULL, lineBuff, "LINE", MB_OK);
		ParseLine(lineBuff, mol);
		delete [] lineBuff;
	}
}

void
CMoleculeBuilder::ParseLine(TCHAR *line, CMolecule *mol)
{
	int len = _tcslen(line);
	if(len < 6)
		return;

	if(!_tcsncicmp("HETATM", line, 6) || !_tcsncicmp("ATOM", line, 4))
	{
		// got atom entry
		int number;
		TCHAR *name_wrk = new TCHAR[4];
		TCHAR *name = new TCHAR[4];
		if(1 != sscanf (line+7, " %d ", &number))
            MessageBox(NULL, "PARSE ERROR", "ERROR", MB_OK);
		_tcsncpy(name_wrk, line+12, 3);
		name_wrk[3] = '\0';
		TCHAR *safe = name;
		for(int i=0;i<4;i++)
			if(!isspace(name_wrk[i]) && !isdigit(name_wrk[i]))
				*(name++) = name_wrk[i];
		*name = '\0';
		name = safe;
		GLfloat x = -999, y = -999, z = -999;
		if(3 != sscanf (line + 31, " %f %f %f ", &x, &y, &z))
			MessageBox(NULL, "COORD PARSE ERROR", "ERROR", MB_OK);

		mol->CreateAtom(x, y, z, name, number);
		return;
	}
	if(!_tcsncicmp("CONECT", line, 6))
  {
		int atoms[11];
    TCHAR buf[5];
		
		if(len < 12)	// CONECTssdddd (12 chars)
			return;

		line = line+8;
		int i = 0;
		for(i=0;i<11;i++)
		{
			memset(buf, 0, 4);
			_tcsncpy(buf, line, 4);
			buf[4] = '\0';
			int val = atoi(buf);
			if(val > 0)
			{
				atoms[i] = val;
				if(_tcslen(line) >= 9)		// 4 characters were read, 1 is the space
					line += 5;							// and we need at least 4 characters more to continue reading (4+1+4=9)
				else
				{
					i++;
					break;
				}
			}
			else
				break;
		}
		int j;
    for (j = 1; j < i; j++)
		{
      if (atoms[j] > 0)
				mol->PutLink(atoms[0], atoms[j]);
		}    
		return;
	}
	if(!_tcsncicmp("HEADER", line, 6))
	{
		if(_tcslen(line) < 11)
			return;
		mol->SetDescription(line+10);
	}
}