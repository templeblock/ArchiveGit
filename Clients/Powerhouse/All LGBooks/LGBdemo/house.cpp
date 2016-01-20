#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#define MAIN
#include "house.h"	
#include "chase.h"
#include "connect.h"
#include "mixup.h"

//***********************************************************************
LPSCENE CHouseApp::CreateScene( int iScene, int iNextScene )
//***********************************************************************
{    
	LPSCENE lpScene;

	if (iScene == IDD_CHASEI || iScene == IDD_CHASE1 || iScene == IDD_CHASE2 || iScene == IDD_CHASE3)
	    lpScene = new CChaseScene(iNextScene);
	else
	if (iScene == IDD_CONNECTI || iScene == IDD_CONNECT1 || iScene == IDD_CONNECT2 || iScene == IDD_CONNECT3)
	    lpScene = new CConnectScene(iNextScene);
	else
	if (iScene == IDD_MIXUPI || iScene == IDD_MIXUP1 || iScene == IDD_MIXUP2 || iScene == IDD_MIXUP3)
	    lpScene = new CMixupScene(iNextScene);
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
		id = IDD_CONNECT1;
	else
	if (id == IDC_ACTIVITY3)
		id = IDD_MIXUP1;
	else
		id = CLGBApp::GetSceneID(id);
	return(id);
}

