#include <windowsx.h>
#include "proto.h"
#include "Cells.h"

//***********************************************************************
CCell::CCell()
//***********************************************************************
{
	// Initialize cell
	x = 0;
	y = 0;
	m_nVisible = NO;
	m_On = YES;

	cType = NULL;
	cLook = NULL;

	for(int i=0; i<MAX_GATE; i++)
	{
		cPortal[i] = NULL;
		cOpp[i]    = NULL;
	}
}

//***********************************************************************
CCell::~CCell()
//***********************************************************************
{
}

//***********************************************************************
char CCell::PortalDir(int iDir)
//***********************************************************************
{
	// Adjust direction to the clock
	while (iDir < 1)
		iDir += 12;
	while (iDir > 12)
		iDir -= 12;

	if (iDir & 1)
		return 0;

	int iPortal = (iDir/2) - 1;
	return cPortal[iPortal];
}

//***********************************************************************
void CCell::SetPortalDir(int iDir, char cNum)
//***********************************************************************
{
	// Adjust direction to the clock
	while (iDir < 1)
		iDir += 12;
	while (iDir > 12)
		iDir -= 12;

	if (iDir & 1)
		return;

	int iPortal = (iDir/2) - 1;
	cPortal[iPortal] = cNum;
}

//***********************************************************************
char CCell::PortalType(int iPortal)
//***********************************************************************
{
	// Get the portal char
	if (iPortal < 0 || iPortal >= MAX_GATE)
		return 0;

	return cPortal[iPortal];
}

//***********************************************************************
void CCell::SetPortalType(int iPortal, char cNum)
//***********************************************************************
{
	if (iPortal < 0 || iPortal >= MAX_GATE)
		return;

	cPortal[iPortal] = cNum;
}

//***********************************************************************
void CCell::ZapPortal(int iDir)
//***********************************************************************
{
	// convert the direction to portal
	iDir = iDir / 2 - 1;

	if (iDir < 0 && iDir > MAX_GATE)
		return;

	cPortal[iDir] = 'M';
}				  

//***********************************************************************
char CCell::OppDir(int iDir)
//***********************************************************************
{
	// Adjust direction to the clock
	while (iDir < 1)
		iDir += 12;
	while (iDir > 12)
		iDir -= 12;

	if (iDir & 1)
		return 0;

	int iPortal = (iDir/2) - 1;
	return cOpp[iPortal];
}

//***********************************************************************
void CCell::SetOppDir(int iDir, char cNum)
//***********************************************************************
{
	// Adjust direction to the clock
	while (iDir < 1)
		iDir += 12;
	while (iDir > 12)
		iDir -= 12;

	int iPortal = (iDir/2) - 1;
	cOpp[iPortal] = cNum;
}

//***********************************************************************
char CCell::OppType(int iPortal)
//***********************************************************************
{
	// Get the opponent type code
	if (iPortal < 0 || iPortal >= MAX_GATE)
		return 0;

	return cOpp[iPortal];
}

//***********************************************************************
void CCell::SetOppType(int iPortal, char cNum)
//***********************************************************************
{
	if (iPortal < 0 || iPortal >= MAX_GATE)
		return;

	cOpp[iPortal] = cNum;
}

//***********************************************************************
void CCell::ClearOpps(void)
//***********************************************************************
{
	for(int i=0; i<MAX_GATE; i++)
		cOpp[i] = 0;
}

