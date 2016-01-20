// Check ends for all shots
// Check values for all shots
// Check downs for all shots
// Check explosions for all shots
// All downs always take you to an empty
#include <winres.h>
#include "resource.h"
#include "commonid.h"
#include "control.h"
#include "cllsnid.h"

//***********************************************************************
// Title specific scenes (dialogs)  stages, (backgrounds)  and props (controls)
//
// STAGE switches are as follows:
//              /t# = transition #
//              /s  = scaled to fit window
//              /c  = centered
//              /n  = do not use WinG
//              /a  = animation will be used (requires 2 DIB's)
// VIDEO switches are as follows:
// Note: Two videos in one scene may cause a hang
//              /a  = auto play (when scene begins)
//              /f  = full screen playback
//              /z  = zoom factor
// GOTOSCENE switches are as follows:
//              /t# = timed to go of in # seconds even if not clicked

// What about sprites?
// What about wav files?

//***********************************************************************
1 SCENE // The logo scene
//***********************************************************************
STAGE ""
PROPS
{
	VIDEOC( "coll_int.avi/zoom=2/autoplay", IDOK, 320, 240 )
	//GOTOSCENE( "2/timer=1", IDOK, 0, 0, 640, 480 )
}

//***********************************************************************
2 SCENE // The main menu scene
//***********************************************************************
STAGE "main.bmp/transition=7,driver21.wav/loop"
PROPS
{ // 122 x 87
	#ifdef _DEBUG
		GOTOSCENE( "25", -1, 0, 0, 640, 375 )
	#endif
	DDIB( IDC_START,     7, 392 )  // Start a New Game
	DDIB( IDC_RESUME,  137, 392 )  // Resume the last Game
	DDIB( IDC_OPTIONS, 263, 392 )  // Options
	DDIB( IDC_CREDITS, 391, 392 )  // Credits
	DDIB( IDC_QUIT,    518, 392 )  // Quit
}

//***********************************************************************
20 SCENE // The start a new game scene
//***********************************************************************
STAGE "play.bmp/a"
PROPS
{
	// Keep radar on top since it sometimes goes full screen
	RADAR( IDC_RADAR, 177, 308, 287, 172)

	// NOTE: Need +abc to install custom MMIO junk proc
	VIDEOC( "collide%d.avi+abc/zoom=2", IDC_VIDEO, 320, 148 )
//	VIDEOC( "collide1.avi/zoom=2", IDC_VIDEO, 320, 148 )
//	VIDEOC( "collide1.grf/zoom=2", IDC_VIDEO, 320, 148 )

//	DIB( IDC_HUD1,   7, 306 )
//	DIB( IDC_HUD2,  24, 340 )
//	DIB( IDC_HUD3,  41, 306 ) 
//	DIB( IDC_HUD4,  58, 340 )
//	DIB( IDC_HUD5,  75, 306 )

//	DIB( IDC_HUDOPP1,   7, 306 )
//	DIB( IDC_HUDOPP2,  24, 340 )
//	DIB( IDC_HUDOPP3,  41, 306 )
//	DIB( IDC_HUDOPP4,  58, 340 )
//	DIB( IDC_HUDOPP5,  75, 306 )

	TDIB( IDC_HUDOPP1,  12, 317 )
	TDIB( IDC_HUDOPP2,  51, 311 )
	TDIB( IDC_HUDOPP3,  89, 315 )

	DIB( IDC_COLLECT09,  3, 378 )
	DIB( IDC_COLLECT05, 32, 378 )
	DIB( IDC_COLLECT06, 61, 378 )
	DIB( IDC_COLLECT10, 90, 378 )

	DIB( IDC_COLLECT07,  3, 409 )
	DIB( IDC_COLLECT01, 32, 409 )
	DIB( IDC_COLLECT02, 61, 409 )
	DIB( IDC_COLLECT08, 90, 409 )

	DIB( IDC_COLLECT11,  3, 440 )
	DIB( IDC_COLLECT03, 32, 440 )
	DIB( IDC_COLLECT04, 61, 440 )
	DIB( IDC_COLLECT12, 90, 440 )

	DIB( IDC_ENERGYDIGITA, 133, 306)
	DIB( IDC_ENERGYDIGITB, 143, 306)
	DIB( IDC_ENERGYDIGITC, 153, 306)
	METERXV( IDC_ENERGY, 141, 326 )

	DIB( IDC_AMMODIGITA, 501, 399)
	DIB( IDC_AMMODIGITB, 511, 399)
	DIB( IDC_AMMODIGITC, 521, 399)
	DIB( IDC_AMMOTYPE, 535, 398)

	TDIB( IDC_KILL100, 488, 438)
	TDIB( IDC_KILL200, 501, 438)
	TDIB( IDC_KILL300, 488, 456)
	TDIB( IDC_KILL400, 501, 456)

	TDIB( IDC_KILL10, 514, 438)
	TDIB( IDC_KILL20, 527, 438)
	TDIB( IDC_KILL30, 540, 438)
	TDIB( IDC_KILL40, 553, 438)
	TDIB( IDC_KILL50, 566, 438)
	TDIB( IDC_KILL60, 579, 438)
	TDIB( IDC_KILL70, 592, 438)
	TDIB( IDC_KILL80, 605, 438)
	TDIB( IDC_KILL90, 618, 438)

	TDIB( IDC_KILL1, 514, 456)
	TDIB( IDC_KILL2, 527, 456)
	TDIB( IDC_KILL3, 540, 456)
	TDIB( IDC_KILL4, 553, 456)
	TDIB( IDC_KILL5, 566, 456)
	TDIB( IDC_KILL6, 579, 456)
	TDIB( IDC_KILL7, 592, 456)
	TDIB( IDC_KILL8, 605, 456)
	TDIB( IDC_KILL9, 618, 456)

	DIB( IDC_WEAPON, 476, 298 ) 

	//DDIB( IDC_EXIT, 0, 350 )
	//DDIB( IDC_SAVE, 0, 375 )
	//DDIB( IDC_ZOOM, 0, 400 )
	//DIB( IDC_OPTIONS, 8, 418 )
}

#ifdef _DEBUG
//***********************************************************************
25 SCENE // The test and debug scene
//***********************************************************************
STAGE "debug.bmp"
PROPS
{
	// NOTE: Need +abc to install custom MMIO junk proc
	VIDEOC( "collide1.avi+abc/zoom=2", IDC_VIDEO, 320, 148 )

	BUTTON( "Weapons", IDC_WEAPONDEBUG, 530, 310, 120, 25 );
	BUTTON( "Shots",   IDC_SHOTSDEBUG,  530, 340, 120, 25 );

	DDIB( IDC_EXIT, 590, 370 )
}
#endif

//***********************************************************************
30 SCENE // The resume game scene
//***********************************************************************
STAGE "resume.bmp/transition=10,driver11.wav/loop"
PROPS
{
	LTEXT( "", IDC_GAME1_NAME, 254,  85, 195, 18 )
	LTEXT( "", IDC_GAME2_NAME, 254, 121, 195, 18 )
	LTEXT( "", IDC_GAME3_NAME, 254, 157, 195, 18 )
	LTEXT( "", IDC_GAME4_NAME, 254, 192, 195, 18 )
	LTEXT( "", IDC_GAME5_NAME, 254, 229, 195, 18 )
	DDIB( IDC_GAME1, 182,  79 )
	DDIB( IDC_GAME2, 182, 115 )
	DDIB( IDC_GAME3, 182, 151 )
	DDIB( IDC_GAME4, 182, 186 )
	DDIB( IDC_GAME5, 182, 223 )
	DDIB( IDC_GAME0, 182, 259 )
	DDIB( IDC_CANCEL, 269, 370 )
}

//***********************************************************************
40 SCENE // The options scene
//***********************************************************************
STAGE "options.bmp/a/transition=46"
PROPS
{
	DDIB( IDC_OPTION1,   255,  56 )	
	DDIB( IDC_OPTION2,   255,  86 )	
	DDIB( IDC_OPTION3,   255, 116 )	
	DDIB( IDC_OPTION4,   255, 146 )	
	DDIB( IDC_OPTION5,   255, 177 )	
	DDIB( IDC_OPTSTATE1, 489,  56 )	
	DDIB( IDC_OPTSTATE2, 489,  86 )	
	DDIB( IDC_OPTSTATE3, 489, 116 )	
	DDIB( IDC_OPTSTATE4, 489, 146 )	
	DDIB( IDC_OPTSTATE5, 489, 177 )	

	//HSLIDER( IDC_SLIDER_WAVVOL,  10-8, 350-11, 301+16, 25+22 ) // thumb is 16x47 pixels
	//HSLIDER( IDC_SLIDER_MIDIVOL, 10-8, 380-11, 301+16, 25+22 )
	DIB( IDC_VOLUME0, 44, 356 )
	DDIB( IDC_EXIT, 598, 456 )
}

//***********************************************************************
50 SCENE // The credits scene
//***********************************************************************
STAGE "credits.bmp/a/transition=29,driver11.wav/loop"
PROPS
{
	DDIB( IDC_RETURN, 269, 370 )
}

//***********************************************************************
50 RCDATA
//***********************************************************************
{
"
	face1 Arial, size1 18, italic1 0, color1 255   0 0,","
	face2 Arial, size2 14, italic2 0, color2 255 255 0,","
	start 0 300, end=0 0, delay 600, speed 40, type 2;","
	name .;
	name .;
	name Creative Director,				type 1, delay 1500;","
	name  Chris Painter;","			
	name Software Development,			type 1, delay 1500;","
	name  Mike Christian;","
	name  Jim McCurdy;","
	name  Craig Simmons;","
	name Writer,						type 1, delay 1500;","
	name  Adam Dubov;","
	name Editor,						type 1, delay 1500;","
	name  Conrad Smart;","
	name Production Designer,			type 1, delay 1500;","
	name  Jay Hannah;","
	name Line Producer,					type 1, delay 1500;","
	name  Harriet;","
	name 1st Assistant Director,		type 1, delay 1500;","
	name  Marc Oppenheimer;","
	name Post-Production Supervisor,	type 1, delay 1500;","
	name Additional Editing, type 2;","
	name  Rob Claridge;","
	name Composer/Sound Design,			type 1, delay 1500;","
	name  Chuck Barth;","
	name Interface Design,				type 1, delay 1500;","
	name  Lon Richter;","
	name  Kyrsten Johnson;","
	name Producers,						type 1, delay 1500;","
	name  Curt Marvis;","
	name  Dean Mauro;","
	name Executive Producer,			type 1, delay 1500;","
	name  Frank Mazza;","
",0
}

//***********************************************************************
60 SCENE // The quit scene
//***********************************************************************
STAGE "quit.bmp/transition=35,driver11.wav/loop"
PROPS
{
	DDIB( IDC_YES, 200, 400 )
	DDIB( IDC_NOT, 331, 400 )
}

//***********************************************************************
70 SCENE // The save game scene
//***********************************************************************
STAGE "save.bmp/transition=10,driver11.wav/loop"
PROPS
{
	EDITX( IDC_GAME1_NAME, 253, 102, 195, 18 )
	EDITX( IDC_GAME2_NAME, 253, 138, 195, 18 )
	EDITX( IDC_GAME3_NAME, 253, 174, 195, 18 )
	EDITX( IDC_GAME4_NAME, 253, 209, 195, 18 )
	EDITX( IDC_GAME5_NAME, 253, 246, 195, 18 )
	DDIB( IDC_GAME1, 182,  96 )
	DDIB( IDC_GAME2, 182, 132 )
	DDIB( IDC_GAME3, 182, 168 )
	DDIB( IDC_GAME4, 182, 203 )
	DDIB( IDC_GAME5, 182, 240 )
	DDIB( IDC_CANCEL, 265, 370 )
}
