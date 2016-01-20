
#ifndef _SAILOR_H_
#define _SAILOR_H_

#include "lgbook.h"
#include "sailorid.h"

#ifdef MAIN
	#define DECLARE
#else
	#define DECLARE extern
#endif

class CSailorApp : public CLGBApp
{
public:
	// Implementation
	LPSCENE CreateScene( int iScene, int iNextScene );
	int GetSceneID(int id);
	int GetFirstSceneNo() {	return(IDC_FIRSTSCENENO); }
	int GetFinalSceneNo() { return(IDC_LASTSCENENO); }
	int GetFinalSceneID() { return (IDC_LASTSCENEID); }
};

typedef CSailorApp FAR *LPSAILORAPP;
DECLARE CSailorApp App;

#endif // _SAILOR_H_  
                          