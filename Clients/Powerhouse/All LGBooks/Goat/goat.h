
#ifndef _GOAT_H_
#define _GOAT_H_

#include "lgbook.h"
#include "goatid.h"

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

class FAR CGoatApp : public CLGBApp
{
public:
	// Implementation
	LPSCENE CreateScene( int iScene, int iNextScene );
	int GetSceneID(int id);
	int GetFirstSceneNo() {	return(IDC_FIRSTSCENENO); }
	int GetFinalSceneNo() { return(IDC_LASTSCENENO); }
	int GetFinalSceneID() { return (IDC_LASTSCENEID); }
};

typedef CGoatApp FAR *LPGOATAPP;
DECLARE CGoatApp App;

#endif // _GOAT_H_  
                          