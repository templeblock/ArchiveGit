
#ifndef _PAINTBOX_H_
#define _PAINTBOX_H_

#include "lgbook.h"
#include "painid.h"

#define SUCCESS_CHANNEL		0
#define MUSIC_CHANNEL		0
#define NORMAL_CHANNEL		1
#define INTRO_CHANNEL		2
#define HINT_CHANNEL		2
#define MAX_SUCCESSWAVES	4			// Max waves for successwaves= key entry for the
#define CLICK_WAVE			"click.wav" // Click wave for button click sound


#ifdef MAIN
	#define DECLARE
#else
	#define DECLARE extern
#endif

class FAR CcabApp : public CLGBApp
{
public:
	// Implementation
	LPSCENE CreateScene( int iScene, int iNextScene );
	int GetSceneID(int id);
	int GetFirstSceneNo() {	return(IDC_FIRSTSCENENO); }
	int GetFinalSceneNo() { return(IDC_LASTSCENENO); }
	int GetFinalSceneID() { return (IDC_LASTSCENEID); }
};

typedef CcabApp FAR *LPcabAPP;
DECLARE CcabApp App;

#endif // _cab_H_  
