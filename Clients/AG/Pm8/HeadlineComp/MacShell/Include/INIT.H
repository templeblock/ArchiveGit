/* ===========================================================================
   Module   :	init.h
   Project  :	Renaissance graphic headline test utility.
   Abstract :	Application initialization routines.
   Status   :	Under Development
  
   Copyright © 1996, Turning Point Software.  All Rights Reserved.
  
   To Do:
  
   =========================================================================== */


/**  define menu IDs  **/
#define           kAppleMENU         1
#define           kFileMENU          2
#define           kEditMENU          3
#define           kScaleMENU         4
#define           kFlipMENU          5
#define           kRotateMENU        6
#define           kShapeMENU         7
#define           kPathMENU         11
#define           kEffectsMENU       8
#define           kOutlineMENU      12
#define           kWackyMENU         9
#define           kFontMENU         10

/***  prototypes  ***/
#ifdef __cplusplus
extern "C" {
#endif
         BOOLEAN        fValidateConfiguration (void);
         void           InitApplication (void);
 	     void           SetUpMenus (void);
         void           ShutdownApplication (void);
         BOOLEAN        fSystem7Available (void);
         BOOLEAN        fGestaltAvailable (void);
         BOOLEAN        fColorQuickdrawAvailable (BOOLEAN f32Bit);
         BOOLEAN        fTrapAvailable (short trap);
         short          sTrapType (short trap);
         short          sToolboxTraps (void);
		Boolean  		tsysMacHasGWorlds (void);
#ifdef __cplusplus
}
#endif
