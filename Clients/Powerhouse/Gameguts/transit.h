#ifndef _transit
#define _transit

typedef void (FAR *DIBLTCALLBACK)(DWORD dwData, HDC dcDst, int xDst, int yDst, int xDstExt, int yDstExt, int xSrc, int ySrc, int xSrcExt, int ySrcExt );

#define TRANS_REPLACE				1	// Transitions Effects
#define TRANS_BLINDS_HORIZONTAL		2
#define TRANS_BLINDS_VERTICAL		3
#define TRANS_BOX_IN				4
#define TRANS_BOX_OUT				5
#define TRANS_CHECKERBOARD_LEFT		6
#define TRANS_CHECKERBOARD_RIGHT	7
#define TRANS_CHECKERBOARD_UP		8
#define TRANS_CHECKERBOARD_DOWN		9
#define TRANS_COVER_LEFT			10
#define TRANS_COVER_RIGHT			11
#define TRANS_COVER_UP				12
#define TRANS_COVER_DOWN			13
#define TRANS_COVER_LEFTDOWN		14
#define TRANS_COVER_LEFTUP			15
#define TRANS_COVER_RIGHTDOWN		16
#define TRANS_COVER_RIGHTUP			17
#define TRANS_DISSOLVE				18
#define TRANS_FADETHROUGHBLACK		19
#define TRANS_STRETCH_HORIZONTAL_OUT	20
#define TRANS_STRETCH_VERTICAL_OUT	21
#define TRANS_STRETCH_LEFT			22
#define TRANS_STRETCH_RIGHT			23
#define TRANS_STRETCH_UP			24
#define TRANS_STRETCH_DOWN			25
#define TRANS_RAIN_LEFT				26
#define TRANS_RAIN_RIGHT			27
#define TRANS_RAIN_UP				28
#define TRANS_RAIN_DOWN				29
#define TRANS_RANDOMBARS_VERTICAL	30
#define TRANS_RANDOMBARS_HORIZONTAL 31
#define TRANS_ROTATE_VERTICAL		32
#define TRANS_ROTATE_HORIZONTAL		33
#define TRANS_SPIRAL_CW_OUT			34
#define TRANS_SPIRAL_CCW_IN			35
#define TRANS_SPIRAL_CCW_OUT		36
#define TRANS_SPIRAL_CW_IN			37
#define TRANS_SPLIT_HORIZONTAL_IN	38
#define TRANS_SPLIT_VERTICAL_IN		39
#define TRANS_SPLIT_VERTICAL_OUT	40
#define TRANS_SPLIT_HORIZONTAL_OUT	41
#define TRANS_UNCOVER_LEFT			42
#define TRANS_UNCOVER_RIGHT			43
#define TRANS_UNCOVER_UP			44
#define TRANS_UNCOVER_DOWN			45
#define TRANS_UNCOVER_LEFTDOWN		46
#define TRANS_UNCOVER_LEFTUP		47
#define TRANS_UNCOVER_RIGHTDOWN		48
#define TRANS_UNCOVER_RIGHTUP		49
#define TRANS_WATERFALL				50
#define TRANS_WEAVE_VERTICAL		51
#define TRANS_WEAVE_HORIZONTAL		52
#define TRANS_WIPE_LEFT				53
#define TRANS_WIPE_RIGHT			54
#define TRANS_WIPE_UP				55
#define TRANS_WIPE_DOWN				56
#define TRANS_WIPE_LEFTDOWN			57
#define TRANS_WIPE_LEFTUP			58
#define TRANS_WIPE_RIGHTDOWN		59
#define TRANS_WIPE_RIGHTUP			60
#define TRANS_XRAY					61

#define TRANS_VERYFAST				0x0100	// Transition Speed Modifiers
#define TRANS_FAST					0x0200
#define TRANS_MEDIUM				0x0300
#define TRANS_SLOW					0x0400
#define TRANS_VERYSLOW				0x0500

#define MAKETRANSITION(e,s) ( (e)|(s) )

typedef struct
{
	HDC		dcDst;			// Destination device context
	HDC		dcSrc;			// Source DC (if not NULL)
	PDIB	lpSrcDib;		// Source DIB (if not NULL)
	WORD	Transition;		// Selected transition
	int		xDst, yDst;		// Destination origin
	int		xSrc, ySrc;		// Source origin
	int		xExt, yExt;		// Blt extents
	WORD	nIteration;		// Current iteration
	int		nTicks;			// Countdown timer for transition step
	int		vStep, hStep;	// Vert and horz step counters
	int		xPos, yPos;		// Last partical location
	int		xBox, yBox;		// Extents of particle
	int		xMin, xMax;		// Limits of a spiral
	int		yMin, yMax;		// Limits of a spiral
	HANDLE	hData;			// Handle to user data
	LPBYTE	lpData;			// Pointer to user data
	int		iClock;
	DWORD	dwLastTime;		// last time step
	int		nStepSizeOverride;		// step size override
	int		nTicksOverride;			// speed override
	DWORD	dwTotalTime;	// total time for effect
	DWORD	dwTotalIterations; // total number of iterations
	DWORD	dwStepTime;	   // time this effect was started
						   // must be filled in by setup function
	DWORD	dwTimePerStep; // amount of time per step
	DWORD	dwIteration;
	TRANSCALLBACK 	lpCallback;
	DWORD			dwCallbackData;
} TRANS, FAR *LPTRANS;

#endif
