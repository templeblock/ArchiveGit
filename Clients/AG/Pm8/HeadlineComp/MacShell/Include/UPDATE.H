/* ===========================================================================
   Module   :	update.h
   Project  :	Renaissance graphic headline test utility.
   Abstract :	Screen update/drawing routines.
   Status   :	Under Development
  
   Copyright © 1996, Turning Point Software.  All Rights Reserved.
  
   To Do:
  
  
   =========================================================================== */
   

/**  prototypes  **/
         void           PaintMainWindow (void);
         void           PrintMainWindow (void);
         void           DrawGraphicToScreen ( WindowPtr pPort, Rect *rect );
         void           DrawGraphicToOffScreen ( void );
         void           DrawGraphicToPict ( WindowPtr pPort );
			RRealSize  		CalcDefaultHeadlineScreenSize ( );


