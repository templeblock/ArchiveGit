#include <windows.h>
#include <stdlib.h>
#include "proto.h"
#include "control.h"
#include "worlds.h"

int COpponent::iNumOpponents = 0;

//************************************************************************
COpponent::COpponent()
//************************************************************************
{
	iNumOpponents++; 
	iDirection = 0; 
	iHealth = 0; 
	ptCell.x = 0;
	ptCell.y = 0;
	lpPrevOpp = NULL; 
	lpNextOpp = NULL; 
}

//************************************************************************
COpponent::~COpponent()
//************************************************************************
{
	iNumOpponents--;
	Delete();
}

//********************************************************************************************
BOOL COpponent::Init( int iInitDir, POINT pntInitCell, int iBehave )
//********************************************************************************************
{ 
	iDirection = iInitDir;
	iBehavior = iBehave;
	ptCell = pntInitCell;
	iHealth = 100;
	lpPrevOpp = NULL;
	lpNextOpp = NULL;
	return TRUE;
}

//************************************************************************
BOOL COpponent::Delete(void)
//************************************************************************
{ 
	if ( lpPrevOpp )
		 lpPrevOpp->lpNextOpp =  lpNextOpp;
	if ( lpNextOpp )
		 lpNextOpp->lpPrevOpp =  lpPrevOpp;

	return( TRUE );
}
