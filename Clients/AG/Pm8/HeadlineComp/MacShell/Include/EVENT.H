/* ===========================================================================
   Module   :	event.h
   Project  :	Renaissance graphic headline test utility.
   Abstract :	Main event loop of application.
   Status   :	Under Development
  
   Copyright © 1996, Turning Point Software.  All Rights Reserved.
  
   To Do:
  
  
   =========================================================================== */
   


/**  define resource IDs  **/
#define  rErrorSTR					1000 
#define  rSystem7ALRT				11001  
#define  rLowMemoryALRT				11002
#define  rMonitorConfigALRT			11003  
#define  rSystemALRT				11004 
#define  rApplicALRT				11005 
#define  rGenericALRT				11006 
#define  rQuickDrawALRT				11011  
#define  rQuickTimeALRT				11012  
#define  rSoundManagerALRT			11013  
#define  rInstallerALRT				11014 

/**  prototypes  **/
         void           MainEventLoop (void);
         void           AlertOnError (sWORD severity, TERRCODE error, BOOLEAN fatal);


