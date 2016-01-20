#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#define MAIN
#include "house.h"	
#include "chase.h"
#include "maze.h"
#include "putaway.h"

//***********************************************************************
LPSCENE CHouseApp::CreateScene( int iScene, int iNextScene )
//***********************************************************************
{    
	LPSCENE lpScene;

	if (iScene == IDD_CHASEI || iScene == IDD_CHASE1 || iScene == IDD_CHASE2 || iScene == IDD_CHASE3)
	    lpScene = new CChaseScene(iNextScene);
	else
	if (iScene == IDD_MAZEI || iScene == IDD_MAZE1 || iScene == IDD_MAZE2 || iScene == IDD_MAZE3)
	    lpScene = new CMazeScene(iNextScene);
	else
	if (iScene == IDD_PUTAWAYI || iScene == IDD_PUTAWAY1 || iScene == IDD_PUTAWAY2 || iScene == IDD_PUTAWAY3)
	    lpScene = new CPutAwayScene(iNextScene);
	else
	    lpScene = CLGBApp::CreateScene(iScene, iNextScene);
	return(lpScene);
}

//***********************************************************************
int CHouseApp::GetSceneID(int id)
//***********************************************************************
{
	if (id == IDC_ACTIVITY1)
		id = IDD_CHASE1;
	else
	if (id == IDC_ACTIVITY2)
		id = IDD_MAZE1;
	else
	if (id == IDC_ACTIVITY3)
		id = IDD_PUTAWAY1;
	else
		id = CLGBApp::GetSceneID(id);
	return(id);
}

