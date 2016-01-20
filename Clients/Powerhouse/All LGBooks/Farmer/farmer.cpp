#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#define MAIN
#include "farmer.h"	
#include "hideseek.h"
#include "soundoff.h"
#include "cleanup.h"
#include "mixup.h"

//***********************************************************************
LPSCENE CFarmerApp::CreateScene( int iScene, int iNextScene )
//***********************************************************************
{    
	LPSCENE lpScene;

	if (iScene == IDD_HIDESEEKI || iScene == IDD_HIDESEEK1 || iScene == IDD_HIDESEEK2 || iScene == IDD_HIDESEEK3)
	    lpScene = new CHideSeekScene(iNextScene);
	else
	if (iScene == IDD_SOUNDOFFI || iScene == IDD_SOUNDOFF1 || iScene == IDD_SOUNDOFF2 || iScene == IDD_SOUNDOFF3)
	    lpScene = new CSoundOffScene(iNextScene);
	else
	if (iScene == IDD_CLEANUPI || iScene == IDD_CLEANUP1 || iScene == IDD_CLEANUP2 || iScene == IDD_CLEANUP3)
	    lpScene = new CCleanupScene(iNextScene);
	else
	if (iScene == IDD_MIXUPI || iScene == IDD_MIXUP1 || iScene == IDD_MIXUP2 || iScene == IDD_MIXUP3)
	    lpScene = new CMixupScene(iNextScene);
	else
	    lpScene = CLGBApp::CreateScene(iScene, iNextScene);

    return( lpScene );
}

//***********************************************************************
int CFarmerApp::GetSceneID(int id)
//***********************************************************************
{
	if (id == IDC_ACTIVITY1)
		id = IDD_HIDESEEK1;
	else
	if (id == IDC_ACTIVITY2)
		id = IDD_SOUNDOFF1;
	else
	if (id == IDC_ACTIVITY3)
		id = IDD_CLEANUP1;
	else
	if (id == IDC_ACTIVITY4)
		id = IDD_MIXUP1;
	else
		id = CLGBApp::GetSceneID(id);
	return(id);
}

