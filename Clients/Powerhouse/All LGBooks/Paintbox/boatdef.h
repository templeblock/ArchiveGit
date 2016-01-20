#ifndef _BOATDEF_H_
#define _BOATDEF_H_

// The following describes how cars perform in each lane
// 1. Cars will be selected randomly from the left to right "car pool"
//	  or the right to left "car pool".
// 2. The top and bottom settings define the vertical boundaries of the lane.
// 3. The left to right setting governs the direction of traffic.
// 4. The number of cars in a lane as well as the filename of the file
//    to be used for the sprite cell.
// 5. The minimum and maximum times are the times used to start
//    cars in each lane at random intervals.  The minimum
//	  time should be setup based on the lane speed so that you can't
//    possibly run out of cars.  There are different settings
//    for each difficulty level.
// 6. The speed is the pixels/second that a car will travel.  All
//    cars in a lane travel at the same speed.  There are different
//    settings for each difficulty level.
// 7. All other structure entries should be set to the default.

#define DOGGER_CAR		"boat.bmp"
#define TRAFFIC_WAVE 	"TRAFFIC.WAV"
#define CRASH_WAVE		"CRASH.WAV"
#define BUMP_WAVE		"BUMP.WAV"

#define CAR_X			101
#define CAR_Y			365
#define CAR_MOVE_XINC	20
#define CAR_MOVE_YINC	14
#define NUM_LANES 		5
#define NUM_NOZONES		7
#define FROGGER_TIMER 	10
#define FROGGER_DELAY	50
#define TRAFFIC_CHANNEL	1 // 0 is always reserved for transition sound
#define CRASH_CHANNEL	2
#define BUMP_CHANNEL	3

#undef SUCCESS_CHANNEL
#define SUCCESS_CHANNEL	4
#undef INTRO_CHANNEL
#define INTRO_CHANNEL	4
//#define HINT_CHANNEL	5

typedef enum
{
	RIGHT_TO_LEFT,
	LEFT_TO_RIGHT,
	RANDOM
} DIRECTION;


// The following is a list of rectangles where the car is not allowed to go
// just add more rectangles to the list to restrict more areas
static RECT NoZones[NUM_LEVELS][NUM_NOZONES] = {
	
	// Level 1
	0, 229, 242, 301, 	// left side of island
	406, 227, 640, 301,	// right side of island 
	0, 0, 275, 127,		// Top Left of Game
	363, 0, 640, 127,	// Top Right of Game
	0, 431, 640, 480,   // Bottom of Game
	0, 0, 47, 480,		// left side of far side
	594, 0, 640, 480,	// right side of far side  

	// Level 2          
	0, 0, 0, 0, 		// left side of island
	0, 0, 0, 0,			// right side of island
	0, 0, 275, 127,		// Top Left of Game
	363, 0, 640, 127,	// Top Right of Game
	0, 431, 640, 480,   // Bottom of Game  
	0, 0, 47, 480,		// left side of far side
	594, 0, 640, 480, 	// right side of far side  		
	
	
	// Level 3
	0, 229, 242, 301, 	// left side of island
	406, 227, 640, 301,	// right side of island 
	0, 0, 275, 127,		// Top Left of Game
	363, 0, 640, 127,	// Top Right of Game
	0, 431, 640, 480,   // Bottom of Game
	0, 0, 47, 480,		// left side of far side
	594, 0, 640, 480	// right side of far side 
	
	};

static RECT WinZones[NUM_LEVELS] = {
	// Level 1
	342, 30, 552, 108,
	// Level 2
	342, 30, 552, 108,
	// Level 3
	342, 30, 552, 108};

// clipping rectangle for display of sprites
// this enables us to have a frame around the background bitmap
static RECT rGameArea = {47, 30, 592, 433};
                                   
// These sprites are in a static position on the screen                                   
static STILLSPRITE Stills[] = { 
	"oct01.bmp",    9, 180,  52, 10, LEVEL1_BIT|LEVEL2_BIT|LEVEL3_BIT, 	NULL,
	"oct01.bmp",    9, 340, 354,  5, LEVEL1_BIT|LEVEL2_BIT|LEVEL3_BIT, 	NULL,
	"drag01.bmp",  11, 70,  233, 20, LEVEL3_BIT, 						NULL,
	"drag01.bmp",  11, 340, 233, 15, LEVEL3_BIT, 						NULL,
	"coral01.bmp", 10, 204, 233, 20, LEVEL3_BIT,						NULL,
	"coral01.bmp", 10, 481, 233, 15, LEVEL3_BIT,						NULL
};
    
static MOTORPOOL L2RMotorPool[] = {
	"shark01.bmp", 10,
	FALSE, NULL,
	"whirl01.bmp", 7,
	FALSE, NULL,
	"spout01.bmp", 5,
	FALSE, NULL};           
	
static MOTORPOOL R2LMotorPool[] = {
	"dolph01.BMP", 10,
	FALSE, NULL,
	"shark01.bmp", 10,
	FALSE, NULL,
	"whirl01.bmp", 7,
	FALSE, NULL,
	"spout01.bmp", 5,
	FALSE, NULL};           

typedef struct _froglane
{
	int		iTop;						// top of lane in pixels
	int		iBottom;					// bottom of lane in pixels
	DIRECTION Direction;				// Direction of cars
	DWORD	dwMinimumTime[NUM_LEVELS];	// minimum time for random interval (milliseconds)
	DWORD	dwMaximumTime[NUM_LEVELS];	// maximum time for random interval (milliseconds)
	int		iSpeed[NUM_LEVELS];			// speed of cars in pixels/second
	BOOL	fWaitingForCar;				// Waiting for a car to be free?
	DWORD	dwWaitStart;				// Time when started waiting
	DWORD	dwWaitTime;					// The wait time  
	BOOL    fUse[NUM_LEVELS];			// Use this level ?
} FROGLANE, FAR *LPFROGLANE;

static FROGLANE Lanes[NUM_LANES] = {
	101, 167, RIGHT_TO_LEFT,			// iTop iBottom Direction
// Level 1 Level 2 Level 3
	3000, 	1000,	500,				// dwMinimumTime
	6000, 	3500,	1000,				// dwMaximumTime
	85,		85,		65,  				// iSpeed
	FALSE, 	0, 		0,	 				// fWaitingForCar dwWaitStart dwWaitTime  
	FALSE,  TRUE,  TRUE,				// fUse
                     
	167, 233, LEFT_TO_RIGHT,			// iTop iBottom Direction
// Level 1 Level 2 Level 3
	3000, 	1000,	1000,				// dwMinimumTime
	6000, 	3000,	3000,				// dwMaximumTime
	75,		75,		55,	  				// iSpeed
	FALSE, 	0, 	   	0,	   				// fWaitingForCar dwWaitStart dwWaitTime
	TRUE,  TRUE,   TRUE,               // fUse

	233, 299, RANDOM,					// iTop iBottom Direction
// Level 1 Level 2 Level 3
	3000, 	1000,	1000,				// dwMinimumTime
	6000, 	4000,	3500,				// dwMaximumTime
	100, 	100,	80,				// iSpeed	
	FALSE, 	0, 		0,					// fWaitingForCar dwWaitStart dwWaitTime
	FALSE,  TRUE,  FALSE,               // fUse

	299, 365, RIGHT_TO_LEFT,			// iTop iBottom Direction
// Level 1 Level 2  Level3
	3000, 	1000,	1000,				// dwMinimumTime
	6000, 	4000,	3000,				// dwMaximumTime
	100, 	100,	70,		    		// iSpeed	
	FALSE, 	0, 		0,	 				// fWaitingForCar dwWaitStart dwWaitTime
	TRUE,  TRUE,   TRUE,               // fUse

	365, 433, LEFT_TO_RIGHT,			// iTop iBottom Direction
// Level 1 Level 2  Level 3
	3000, 	1000,	950,				// dwMinimumTime
	6000, 	2000,	1500,				// dwMaximumTime
	75, 	75,		75,					// iSpeed	
	FALSE, 	0, 		0,	  				// fWaitingForCar dwWaitStart dwWaitTime
	FALSE,  FALSE,  FALSE};             // fUse

#endif // _BOATDEF_H_
