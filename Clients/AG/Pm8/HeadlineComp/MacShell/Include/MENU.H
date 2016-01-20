/* ===========================================================================
   Module   :	menu.h
   Project  :	Renaissance graphic headline test utility.
   Abstract :	Menu routines of application.
   Status   :	Under Development
  
   Copyright © 1996, Turning Point Software.  All Rights Reserved.
  
   To Do:
  
  
   =========================================================================== */
   


/**  defines  **/
#define           kAboutAMENU        1

#define           kNewFMENU          1
#define           kOpenFMENU         2
#define           kCloseFMENU        3
#define           kPageFMENU         5
#define           kPrintFMENU        6
#define           kQuitFMENU         8

#define           kUndoEMENU         1
#define           kCutEMENU          3
#define           kCopyEMENU         4
#define           kPasteEMENU        5
#define           kClearEMENU        6

#define           k10SMENU         	1
#define           k25SMENU          2
#define           k50SMENU          3
#define           k75SMENU          4
#define           k100SMENU         5
#define           k150SMENU         6
#define           k200SMENU         7
#define           k300SMENU         8
#define           k500SMENU         9

#define           kNonePMENU       	1
#define           kHorizPMENU       2
#define           kVertPMENU        3
#define           kBothPMENU        4

#define           k0RMENU         	1
#define           k90RMENU          2
#define           k180RMENU         3
#define           k270RMENU         4
#define           kOtherRMENU       6

#define           kNoShapeDMENU     1
#define           kShape11DMENU     3
#define           kShape12DMENU     4
#define           kShape13DMENU     5
#define           kShape14DMENU     6
#define           kShape15DMENU     7
#define           kShape16DMENU     8
#define           kShape17DMENU     9
#define           kShape18DMENU    10
#define           kShape21DMENU    11
#define           kShape22DMENU    12
#define           kShape23DMENU    13
#define           kShape24DMENU    14
#define           kShape25DMENU    15
#define           kShape26DMENU    16
#define           kShape27DMENU    17
#define           kShape28DMENU    18
#define           kShape31DMENU    19
#define           kShape32DMENU    20
#define           kShape33DMENU    21
#define           kShape34DMENU    22
#define           kShape35DMENU    23
#define           kShape36DMENU    24
#define           kShape37DMENU    25
#define           kShape38DMENU    26
#define           kShape41DMENU    27
#define           kShape42DMENU    28
#define           kShape43DMENU    29
#define           kShape44DMENU    30
#define           kShape45DMENU    31
#define           kShape46DMENU    32
#define           kShape47DMENU    33
#define           kShape48DMENU    34
#define           kShape51DMENU    35
#define           kShape52DMENU    36
#define           kShape53DMENU    37
#define           kShape54DMENU    38
#define           kShape55DMENU    39
#define           kShape56DMENU    40
#define           kShape57DMENU    41
#define           kShape58DMENU    42
#define           kShape61DMENU    43
#define           kShape62DMENU    44
#define           kShape63DMENU    45
#define           kShape64DMENU    46
#define           kShape65DMENU    47
#define           kShape66DMENU    48
#define           kShape67DMENU    49
#define           kShape68DMENU    50
#define           kShape71DMENU    51
#define           kShape72DMENU    52
#define           kShape73DMENU    53

#define           kNoPathGMENU      1
#define           kPath10GMENU      3
#define           kPath20GMENU      4
#define           kPath30GMENU      5
#define           kPath40GMENU      6
#define           kPath50GMENU      7
#define           kPath60GMENU      8
#define           kPath70GMENU      9
#define           kPath80GMENU     10
#define           kPath90GMENU     11
#define           kPath100GMENU    12
#define           kPath110GMENU    13
#define           kPath120GMENU    14
#define           kPath130GMENU    15

#define           kStrokeOMENU      1
#define           kFillOMENU        2
#define           kStrokeFillOMENU  3
#define           k1OutlineOMENU    5
#define           k2OutlineOMENU    6
#define           kBlurOMENU        7
#define           k3OutlineOMENU    8
#define           kHair2OutlineOMENU  9
#define           k2HairOutlineOMENU 10
#define           kRoundOMENU      11
#define           kGlintOMENU      12
#define           kEmbossOMENU     13

#define           kNoneCMENU        1
#define           kStackCMENU       3
#define           kSpinStackCMENU   4
#define           kPerspStackCMENU  5
#define           kExtruCMENU       7
#define           kPerspExtruCMENU  8
#define           kIllumExtruCMENU  9
#define           kDropShadowCMENU  11
#define           kSoftDropShadowCMENU  12
#define           kCastShadowCMENU  13
#define           kSoftCastShadowCMENU  14
#define           kSilhouetteCMENU 16
#define           kEmbossCMENU     17
#define           kDebossCMENU     18

#define           kShearWMENU       1
#define           kAltShearWMENU    2
#define           kAltBaselineWMENU 3
#define           kAltShear2WMENU   4



enum	EFlipDirection	{ kFlipNone, kFlipHorizontal, kFlipVertical, kFlipHorzAndVert };

/**  prototypes  **/
         void           HandleMenu (long choice, short modifiers);
         void           AdjustMenus (void);


