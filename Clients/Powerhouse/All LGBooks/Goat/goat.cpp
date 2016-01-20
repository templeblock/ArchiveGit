#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#define MAIN
#include "goat.h"
#include "dance.h"	
#include "shop.h"
#include "soccer.h"
#include "balloon.h"

//***********************************************************************
LPSCENE CGoatApp::CreateScene( int iScene, int iNextScene )
//***********************************************************************
{    
	LPSCENE lpScene;

	switch (iScene)
	{
		case IDD_DANCEI:
		case IDD_DANCE1:
		case IDD_DANCE2:
		case IDD_DANCE3:
		{
			lpScene = new CDanceScene(iNextScene);
			break;	
		}
		case IDD_SHOPPINGI:
		case IDD_SHOPPING1:
		case IDD_SHOPPING2:
		case IDD_SHOPPING3:
		{
			lpScene = new CShopScene(iNextScene);
			break;	
		}
		case IDD_SOCCERI:
		case IDD_SOCCER1:
		case IDD_SOCCER2:
		case IDD_SOCCER3:
		{
			lpScene = new CSoccerScene(iNextScene);
			break;	
		}
		case IDD_BALLOONI:
		case IDD_BALLOON1:
		case IDD_BALLOON2:
		case IDD_BALLOON3:
		{
			lpScene = new CBalloonScene(iNextScene);
			break;	
		}
		default:
		{
		    lpScene = CLGBApp::CreateScene(iScene, iNextScene);
			break;
		}
	}


    return( lpScene );
}

//***********************************************************************
int CGoatApp::GetSceneID(int id)
//***********************************************************************
{
	if (id == IDC_ACTIVITY1)
		id = IDD_SHOPPING1;
	else
	if (id == IDC_ACTIVITY2)
		id = IDD_DANCE1;
	else
	if (id == IDC_ACTIVITY3)
		id = IDD_SOCCER1;
	else
	if (id == IDC_ACTIVITY4)
		id = IDD_BALLOON1;
	else
		id = CLGBApp::GetSceneID(id);
	return(id);
}

