#include "StdAfx.h"
#include "TreeViewM.h"

CTreeViewM::CTreeViewM(void)
{
}

CTreeViewM::~CTreeViewM(void)
{
}

void
CTreeViewM::FillList()
{
	this->InsertItem("tapir", NULL, NULL);
	this->InsertItem("shrimp", NULL, NULL);
}