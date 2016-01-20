
#ifndef _HOUSE_H_
#define _HOUSE_H_

#include "lgbook.h"
#include "houseid.h"


#define MUSIC_CHANNEL		0
#define NORMAL_CHANNEL		1
#define HINT_CHANNEL		2
#define MAX_SUCCESSWAVES	4			// Max waves for successwaves= key entry for the
#define CLICK_WAVE			"click.wav" // Click wave for button click sound


enum SoundPlaying
{ 
	NotPlaying,
	IntroPlaying,
	CluePlaying,
	SuccessPlaying
};


#ifdef MAIN
	#define DECLARE
#else
	#define DECLARE extern
#endif

class CHouseApp : public CLGBApp
{
public:
	// Implementation
	LPSCENE CreateScene( int iScene, int iNextScene );
	int GetSceneID(int id);
	int GetFirstSceneNo() {	return(IDC_FIRSTSCENENO); }
	int GetFinalSceneNo() { return(IDC_LASTSCENENO); }
	int GetFinalSceneID() { return (IDC_LASTSCENEID); }
};

typedef CHouseApp FAR *LPHOUSEAPP;
DECLARE CHouseApp App;

#endif // _HOUSE_H_  
                          
