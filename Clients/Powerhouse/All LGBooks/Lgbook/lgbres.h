#ifndef _LGBRES_H_
#define _LGBRES_H_

// fade flag values
#define	FADE	1
#define NOFADE	0

// clip flag values
#define CLIP	1
#define NOCLIP	0

// defaults transitions
#define TRANS_PREV	NOFADE CLIP TRANS_COVER_LEFT  50 750
#define TRANS_NEXT	NOFADE CLIP TRANS_COVER_RIGHT 50 750
#define TRANS_JUMP	NOFADE CLIP TRANS_DISSOLVE    10 500
#define TRANS_NONE	NOFADE CLIP 0

// default step size - 50 pixels advanced per step
#define STEP_SIZE	50

// default total time - 1 second
#define TOTAL_TIME	1000

// standard clipping rectangle
#define CL	47		// left
#define CT	30		// top
#define CR	592		// right
#define CB	433		// bottom

#endif // _LGBRES_H_
