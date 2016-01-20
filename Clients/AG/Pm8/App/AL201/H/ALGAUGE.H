/*
 * ALGAUGE.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 * This file and ALGauge.cpp are derived from Microsoft's zYzGauge
 * file posted on CompuServe.
 *
 * You have to include this header file if you are going to use the
 * percentage gauge file.  The percentage gauge is not a C++ class, it
 * is some straight C Windows code that Microsoft posted on Compuserve.
 * The end user has to create a control and assign it class "ALGauge"
 * in Resource Workshop or App Studio.  The initialization function
 * defined in this header file then has to be called to set up the
 * control.  After that, it is simply a matter of sending a number
 * between 0 and 100 to the control, using the ALGaugeSetPosition
 * command. The actual format used for this control is:
 *
 *  SendMessage( window_handle, ALGaugeSetPosition, ratio, 0 )
 * 
 * PROTOTYPES:
 *
 *   ALGaugeInit()   : Public initialization for the entire class
 *                     of gauge objects.
 *
 * ENUMERATED TYPES:
 *
 *  typedef ALGaugeMessages       : Messages that you can send to a
 *                                  gauge control.
 *
 *  typedef ALGaugeOrientation    : The physical orientation of the
 *                                  gauge on your screen.  I like left
 *                                  to right best.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0A : New release
 */

#ifndef _ALGAUGE_H
#define _ALGAUGE_H

/*
 * These are all the different messages you can send to
 * an ALGauge object.  Our demo programs don't use any of these
 * except ALGaugeSetPosition, so most of these aren't even tested.
 */
typedef enum _ALGaugeMessages {  /* Tag public type */
    ALGaugeSetRange       = WM_USER,
    ALGaugeGetRange,
    ALGaugeSetPosition,
    ALGaugeGetPosition,
    ALGaugeSetOrientation,
    ALGaugeGetOrientation,
    ALGaugeSetForegroundColor,
    ALGaugeGetForegroundColor,
    ALGaugeSetBackgroundColor,
    ALGaugeGetBackgroundColor,
    ALGaugeSetDeltaPosition
} ALGaugeMessages;

/*
 * Send one of these with ALGaugeSetOrientation, to determined which
 * direction the flood fill progresses.
 */

typedef enum _ALGaugeOrientation { /* Tag public type */
    ALGaugeOrientLeftToRight,
    ALGaugeOrientRightToLeft,
    ALGaugeOrientBottomToTop,
    ALGaugeOrientTopToBottom
} ALGaugeOrientation;


/*
 * public function prototypes 
 */

#ifdef __cplusplus
extern "C"
#endif
AL_LINKAGE int AL_FUNCTION ALGaugeInit( HINSTANCE hInstance,
                                        HINSTANCE hPrevInstance );

#endif /* #ifdef _ALGAUGE_H */

