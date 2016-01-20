#ifndef _FROGDEF_H_
#define _FROGDEF_H_

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

#define DOGGER_CAR		"DGRCAR.BMP"
#define TRAFFIC_WAVE 	"TRAFFIC.WAV"
#define CRASH_WAVE		"CRASH.WAV"
#define BUMP_WAVE		"BUMP.WAV"
#define CLICK_WAVE		"CLICK.WAV"
#define CAR_X			301
#define CAR_Y			387
#define CAR_MIDX		301
#define CAR_MIDY		247
#define CAR_MOVE_XINC	20
#define CAR_MOVE_YINC	14
#define NUM_LANES 		5
#define NUM_NOZONES		7
#define NUM_WINZONES	1
#define FROGGER_TIMER 	10
#define FROGGER_DELAY	50
#define TRAFFIC_CHANNEL	1 // 0 is always reserved for transition sound
#define CRASH_CHANNEL	2
#define BUMP_CHANNEL	3
#define SUCCESS_CHANNEL	4
#define INTRO_CHANNEL	4
#define HINT_CHANNEL	5

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
	0, 229, 242, 301, 	// left side of island
	406, 227, 640, 301,	// right side of island 
	0, 0, 275, 127,		// Top Left of Game
	363, 0, 640, 127,	// Top Right of Game
	0, 431, 640, 480,   // Bottom of Game
	0, 0, 47, 480,		// left side of far side
	594, 0, 640, 480,	// right side of far side 
	
	// Level 3
	0, 0, 0, 0, 		// left side of island
	0, 0, 0, 0,			// right side of island
	0, 0, 275, 127,		// Top Left of Game
	363, 0, 640, 127,	// Top Right of Game
	0, 431, 640, 480,  // Bottom of Game  
	0, 0, 47, 480,		// left side of far side
	594, 0, 640, 480};	// right side of far side  		

static RECT WinZones[NUM_LEVELS][NUM_WINZONES] = {
	// Level 1
	276, 61, 362, 127, 	// center of far side
	// Level 2
	276, 61, 362, 127, 	// center of far side
	// Level 3
	276, 61, 362, 127}; 	// center of far side

// clipping rectangle for display of sprites
// this enables us to have a frame around the background bitmap
static RECT rGameArea = {47, 28, 594, 432};

static MOTORPOOL L2RMotorPool[] = {
	"BIKER.BMP", 1,
	FALSE, NULL,
	"BLUCARR.BMP", 1,
	FALSE, NULL,
	"BLUTRKR.BMP", 1,
	FALSE, NULL,
	"BUSR.BMP", 1,
	FALSE, NULL,
	"GRNCARR.BMP", 1,
	FALSE, NULL,
	"REDCARR.BMP", 1,
	FALSE, NULL,
	"REDTRKR.BMP", 1,
	FALSE, NULL,
	"WHTTRKR.BMP", 1,
	FALSE, NULL};
	
static MOTORPOOL R2LMotorPool[] = {
	"BIKEL.BMP", 1,
	FALSE, NULL,
	"BLUCARL.BMP", 1,
	FALSE, NULL,
	"BLUTRKL.BMP", 1,
	FALSE, NULL,
	"BUSL.BMP", 1,
	FALSE, NULL,
	"GRNCARL.BMP", 1,
	FALSE, NULL,
	"REDCARL.BMP", 1,
	FALSE, NULL,
	"REDTRKL.BMP", 1,
	FALSE, NULL,
	"WHTTRKL.BMP", 1,
	FALSE, NULL};

LPSTR lpBackgrounds[NUM_LEVELS] = {
	"backgrnd.dib", 
	"backgrnd.dib",
	"backgrn2.dib"};

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
} FROGLANE, FAR *LPFROGLANE;

static FROGLANE Lanes[NUM_LANES] = {
	250, 285, RANDOM,	  				// iTop iBottom Direction
// Level 1 Level 2 Level 3
	0, 		0,		5500,				// dwMinimumTime
	0, 		0,		10000,				// dwMaximumTime
	0,		0,		400,  				// iSpeed
	FALSE, 	0, 		0,	 				// fWaitingForCar dwWaitStart dwWaitTime
                     
	135,	170, RIGHT_TO_LEFT,			// iTop iBottom Direction
// Level 1 Level 2 Level 3
	3000, 	1000,	1000,				// dwMinimumTime
	6000, 	3000,	3000,				// dwMaximumTime
	75,	150,	200,					// iSpeed
	FALSE, 	0, 	   	0,	   				// fWaitingForCar dwWaitStart dwWaitTime

	187, 222, RIGHT_TO_LEFT,			// iTop iBottom Direction
// Level 1 Level 2 Level 3
	3000, 	1000,	1000,				// dwMinimumTime
	6000, 	4000,	3500,				// dwMaximumTime
	100, 	250,	275,				// iSpeed	
	FALSE, 	0, 		0,					// fWaitingForCar dwWaitStart dwWaitTime

	305, 340, LEFT_TO_RIGHT,			// iTop iBottom Direction
// Level 1 Level 2  Level3
	3000, 	1000,	1000,				// dwMinimumTime
	6000, 	4000,	3000,				// dwMaximumTime
	100, 	300,	350,				// iSpeed	
	FALSE, 	0, 		0,	 				// fWaitingForCar dwWaitStart dwWaitTime

	352, 387, LEFT_TO_RIGHT,			// iTop iBottom Direction
// Level 1 Level 2  Level 3
	3000, 	1000,	950,				// dwMinimumTime
	6000, 	2000,	1500,				// dwMaximumTime
	75, 	200,	250,				// iSpeed	
	FALSE, 	0, 		0};	  				// fWaitingForCar dwWaitStart dwWaitTime

#endif // _FROGDEF_H_