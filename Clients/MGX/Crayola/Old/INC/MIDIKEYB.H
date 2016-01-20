/*    (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */


/** midikeyb.h                                                           **/

#ifndef _MIDIKEYB_H_
#define _MIDIKEYB_H_


/* MIDI keyboard control messages--class "midiKeyB" */
    #define KEYB_SETLAYOUT      (WM_USER + 0)
    #define KEYB_GETLAYOUT      (WM_USER + 1)
    #define KEYB_SETMSGDOWN     (WM_USER + 2)
    #define KEYB_GETMSGDOWN     (WM_USER + 3)
    #define KEYB_SETMSGUP       (WM_USER + 4)
    #define KEYB_GETMSGUP       (WM_USER + 5)
    #define KEYB_SETCHANNEL     (WM_USER + 6)
    #define KEYB_GETCHANNEL     (WM_USER + 7)
    #define KEYB_SETFGCOLOR     (WM_USER + 8)
    #define KEYB_GETFGCOLOR     (WM_USER + 9)
    #define KEYB_SETBKCOLOR     (WM_USER + 10)
    #define KEYB_GETBKCOLOR     (WM_USER + 11)
    #define KEYB_SETVELOCITY    (WM_USER + 12)
    #define KEYB_GETVELOCITY    (WM_USER + 13)
    #define KEYB_SETHWND        (WM_USER + 14)
    #define KEYB_GETHWND        (WM_USER + 15)
    #define KEYB_SETHMIDIOUT    (WM_USER + 16)
    #define KEYB_GETHMIDIOUT    (WM_USER + 17)

    #define KEYB_MIDISHORTMSG   (WM_USER + 18)
    #define KEYB_RESET          (WM_USER + 19)


/* default message down and message up values */
    #ifndef WM_MIDIKEYBKEYDOWN
        #define WM_MIDIKEYBKEYDOWN  (WM_USER + 1969)
    #endif

    #ifndef WM_MIDIKEYBKEYUP
        #define WM_MIDIKEYBKEYUP    (WM_USER + 1970)
    #endif


/* keyboard styles */
    #define KEYBS_LABELS        0x8000L     /* keys will have labels     */


/* public function prototypes */
    BOOL FAR PASCAL midiKeyBInit( HANDLE hInst );


#endif

/** EOF: midikeyb.h **/
