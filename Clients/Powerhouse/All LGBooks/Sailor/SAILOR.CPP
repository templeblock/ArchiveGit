#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#define MAIN
#include "sailor.h"	
#include "frogger.h"
#include "shelter.h"
#include "connect.h"
#include "mixup.h"

//***********************************************************************
LPSCENE CSailorApp::CreateScene( int iScene, int iNextScene )
//***********************************************************************
{    
	LPSCENE lpScene;

	if (iScene == IDD_FROGGERI || iScene == IDD_FROGGER)
	    lpScene = new CFroggerScene(iNextScene);
	else
	if (iScene == IDD_DRESSI || iScene == IDD_DRESS)
	    lpScene = new CMixupScene(iNextScene);
	else
	if (iScene == IDD_HOUSEI || iScene == IDD_HOUSE1 || iScene == IDD_HOUSE2 || iScene == IDD_HOUSE3)
	    lpScene = new CShelterScene(iNextScene);
	else
	if (iScene == IDD_CONNECTI || iScene == IDD_CONNECT)
	    lpScene = new CConnectScene(iNextScene);
	else
	    lpScene = CLGBApp::CreateScene(iScene, iNextScene);

    return( lpScene );
}

//***********************************************************************
int CSailorApp::GetSceneID(int id)
//***********************************************************************
{
	if (id == IDC_ACTIVITY1)
		id = IDD_FROGGER;
	else
	if (id == IDC_ACTIVITY2)
		id = IDD_HOUSE1;
	else
	if (id == IDC_ACTIVITY3)
		id = IDD_DRESS;
	else
	if (id == IDC_ACTIVITY4)
		id = IDD_CONNECT;
	else
		id = CLGBApp::GetSceneID(id);
	return(id);
}

