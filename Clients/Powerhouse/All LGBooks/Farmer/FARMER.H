
#ifndef _FARMER_H_
#define _FARMER_H_

#include "lgbook.h"
#include "farmerid.h"

#define HINT_CHANNEL		2
#define MAX_SUCCESSWAVES	4		// Max waves for successwaves= key entry for the
									// Cleanup and Hide & Seek Activities.

#ifdef MAIN
	#define DECLARE
#else
	#define DECLARE extern
#endif

class CFarmerApp : public CLGBApp
{
public:
	// Implementation
	LPSCENE CreateScene( int iScene, int iNextScene );
	int GetSceneID(int id);
	int GetFirstSceneNo() {	return(IDC_FIRSTSCENENO); }
	int GetFinalSceneNo() { return(IDC_LASTSCENENO); }
	int GetFinalSceneID() { return (IDC_LASTSCENEID); }
};

typedef CFarmerApp FAR *LPFARMERAPP;
DECLARE CFarmerApp App;

#endif // _FARMER_H_  
                          