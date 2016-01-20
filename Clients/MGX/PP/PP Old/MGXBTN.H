/****************************************************************************
  
         (C) Copyright 1990 by Micrografx, Inc.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
            from Micrografx, 1303 Arapaho, Richardson, TX. 75081.

 ****************************************************************************

                                   mgxbtn.h

 ****************************************************************************/


/******************************** Contsants *********************************/

/*******************************
      Window extra offsets 
********************************/
 #define GWW_STATE  0
 #define GWW_FONT   2
 #define BC_EXTRA   (GWW_STATE + (2*sizeof (WORD)))

/*******************************
  These values are assumed 
  for bit shifting operations 
********************************/
 #define BFCHECK     0x0003
 #define BFHILIGHT   0x0004
 #define BFFOCUS     0x0008
 #define BFINCLICK   0x0010   /* inside click code */
 #define BFCAPTURED  0x0020   /* we have mouse capture */
 #define BFMOUSE     0x0040   /* mouse-initiated */
 #define BFDONTCLICK 0x0080   /* don't check on get focus */

 #define CH_PREFIX  '&'
 #define BM_CLICK   WM_USER+99

/*******************************
   Additional button styles
********************************/
 #define BS_RED       0x0100L
 #define BS_YELLOW    0x0200L
 #define BS_GREEN     0x0400L
 #define BS_CYAN      0x0800L
 #define BS_BLUE      0x1000L
 #define BS_MAGENTA   0x2000L
 #define BS_DIMMED    0x4000L
 #define BS_COLORMASK 0x7F00L

/********************************** Types ***********************************/
/**************************** External Variables ****************************/
/**************************** External Functions ****************************/

extern BOOL FAR PASCAL register_mgxbtn_class (HANDLE,BOOL);
extern LONG FAR PASCAL MgxBtnWndFn (HWND,WORD,WORD,LONG);
