#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#define MAIN
#include "pain.h"	
#include "boat.h"
#include "nutdrop.h"
#include "strmbstr.h"
#include "maze.h"

//***********************************************************************
LPSCENE CcabApp::CreateScene( int iScene, int iNextScene )
//***********************************************************************
{    
	LPSCENE lpScene;

	if (iScene == IDD_BOATI || iScene == IDD_BOAT1 || iScene == IDD_BOAT2 || iScene == IDD_BOAT3)
	    lpScene = new CBoatScene(iNextScene);
	else
	if (iScene == IDD_NUTDROPI || iScene == IDD_NUTDROP1 || iScene == IDD_NUTDROP2 || iScene == IDD_NUTDROP3)
		lpScene = new CNutDropScene (iNextScene);
	else
	if (iScene == IDD_STRMBSTRI || iScene == IDD_STRMBSTR1 || iScene == IDD_STRMBSTR2 || iScene == IDD_STRMBSTR3)
		lpScene = new CStormbusterScene (iNextScene);
	else
	if (iScene == IDD_MAZEI || iScene == IDD_MAZE1 || iScene == IDD_MAZE2 || iScene == IDD_MAZE3)
		lpScene = new CMazeScene (iNextScene);
	else
	    lpScene = CLGBApp::CreateScene(iScene, iNextScene);
	return(lpScene);
}

//***********************************************************************
int CcabApp::GetSceneID(int id)
//***********************************************************************
{
	if (id == IDC_ACTIVITY1)
		id = IDD_BOAT1;
	else
	if (id == IDC_ACTIVITY2)
		id = IDD_NUTDROP1;
	else
	if (id == IDC_ACTIVITY3)
		id = IDD_STRMBSTR1;
	else
	if (id == IDC_ACTIVITY4)
		id = IDD_MAZE1;
	else
		id = CLGBApp::GetSceneID(id);
	return(id);
}

