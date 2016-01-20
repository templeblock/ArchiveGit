/*****************************************************************************

         (C) Copyright 1994 by Micrografx, Inc.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
            from Micrografx, 1303 Arapaho, Richardson, TX. 75081.

 ****************************************************************************

                                    BOMB.H

*****************************************************************************/

#ifndef __BOMB_H_
#define __BOMB_H_

/******************************** Constants *********************************/
/********************************** Types ***********************************/
/********************************* Classes **********************************/

extern void CleanLocks();

extern BOOL FreeTrialStartup( WORD &wDaysLeft, WORD &wTrialsLeft, 
                              BOOL &bIntruder, BOOL &bFirstTime );

extern BOOL FreeTrialFailSafe();

//extern BOOL RunSlideShow();

#endif // __BOMB_H_
