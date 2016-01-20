/************************************************************************************/
/* monacoprint.h																	*/
/* parameters used throughout monaco print											*/
/************************************************************************************/



#ifndef MONACO_PRINT_H
#define MONACO_PRINT_H

//#define IN_POLAROID
//#define IN_MONACOP_30
//#define IN_EPSON
//#define IN_TECHKON

#ifdef IN_MONACOP_30
#define POLAROID_OR_30
#endif

#ifndef IN_POLAROID  // not polaroid
#define MONACO_FOLDER "\pMonacoPRINT"
#define MONACO_DFAULT_PROFILES "\pColorSyncª Profiles"
#define MONACO_SIG	'MSpt'
#define DEFAULT_PATCH "\pMonaco 1384"
#define MONACO_DEFAULT "\pMonacoPRINT Parameters"
#define MONACO_DEFAULT_SETTINGS "\pMonacoPRINT Settings"
	
#else				// is polaroid
#define MONACO_FOLDER "\pDryJet"
#define MONACO_DFAULT_PROFILES "\pDryJet:Color Profiles"
#define MONACO_SIG	'CLGE'
#define DEFAULT_PATCH "\pPressto! Proof Layout"
#define POLAROID_OR_30
#define MONACO_DEFAULT "\pPressto!Proof Parameters"
#define MONACO_DEFAULT_SETTINGS "\pPressto!Proof Settings"
#endif

#define MONACO_PATCHES_HEAD "Monaco Patches\n"

#define MONACO_INK_SIM "\pInk-Simulation"

#define MONACO_TECHON_DRYJET "\pTechkondryjet"
#define MONACO_TECHON_GENERIC "\pTechkongeneric"

#define LAB_PROFILE_NAME "\pLab Color Space Profile"
#define SWOP_PROFILE_NAME "\pMonaco Swop ICC"

#define MONACO_PATCHES "\pPatchFormats"

#define MONACO_PRINTERS "\pPrinterTypes"



#define MONACO_TEMP_PATCHES "\pTempPatches"

#define MONACO_DEVICES "\pCompensation Tables"
#define MONACO_DTP51 "\pXrite DTP51"
#define MONACO_TECHKON_CP300 "\pTechkon CP300"


#define PROMPT_STRINGS 2004

#define STANDARD_PROMPT 	1
#define SETTINGS_PROMPT 	2
#define PHOTOSHOP_PROMPT 	3
#define ICC_PROMPT 			4
#define CRD_PROMPT			5
#define LINK_PROMPT			6
#define SAVE_DATA_FILE		7


#define	SCRAMBLE_SEED 1234
#define MaxProfiles 200

#define BT_DIM	17


// The dimensions of the black cubes used to calculate gammut surfaces
#define GAM_DIM 10
#define GAM_DIM_K 15
#define GAM_DIM_K_VALS { 0.0, 8.333333, 16.666666, 25.0, 33.333333, 41.66666, 50.0, 58.33333, 66.66666, 75.0, 83.33333, 87.5, 91.66666, 95.833333, 100.0 }
//#define GAM_DIM_K 25
//#define GAM_DIM_K_VALS { 0.0, 4.1666, 8.333333, 12.5,16.666666, 20.83333,25.0, 29.16666,33.333333, 37.5, 41.66666, 45.833333,50.0, 54.1666666,58.33333, 62.5,66.66666, 70.833333,75.0, 79.166666,83.33333, 87.5, 91.66666, 95.833333, 100.0 }

#define GAM_DIM_CUBE (GAM_DIM*GAM_DIM*GAM_DIM)

typedef enum {
	No_Input = 0,						
	Getting_Input = 1,						// getting input from the Xrite or tiff file
	Processing_Input1 = 2,					// calibrate dialog is open
	Processing_Input2 = 4,					// option or similar dialogs are open
	Tweaking_Input = 8,					// tweaking window is open
	Building_Table = 16} InputStatus;		// the table is being built

#define SMALL_CMYK_PATCH

//how many entries in table
#define TABLE_ENTRIES	(35937)

// added by James for cmyk to lab table
#define CMYK_TABLE_ENTRIES (33*33*33 + 256)

#define MAX_NUM_PATCHES 3000

//how many entries in one dimension in reverse table CMYK case
#define DEV_TO_CON_ONE_CMYK	(17)

//how many entries in the reverse table
//#define DEV_TO_CON_TABLE_ENTRIES	(6561)
#define DEV_TO_CON_TABLE_ENTRIES	(DEV_TO_CON_ONE_CMYK*DEV_TO_CON_ONE_CMYK*DEV_TO_CON_ONE_CMYK*DEV_TO_CON_ONE_CMYK)


#define PATCH_729

#define MAX_PRINT_PATCH_ENTRIES	(1000)

#define PHASE_ONE	(1)
#define PHASE_TWO	(2)

//this 729 patches format
#ifdef PATCH_729

#define ONE_D_PATCHES	(9)
#define TWO_D_PATCHES	(ONE_D_PATCHES*ONE_D_PATCHES)
//how many sample patches
#define PRINT_PATCHES	(ONE_D_PATCHES*ONE_D_PATCHES*ONE_D_PATCHES)

#else
//this is 243(currently swop) patches format, added on 8/24
#define ONE_D_PATCHES	(6)
#define TWO_D_PATCHES	(ONE_D_PATCHES*ONE_D_PATCHES)
//how many sample patches
#define PRINT_PATCHES	(ONE_D_PATCHES*ONE_D_PATCHES*ONE_D_PATCHES+ 27)

#endif

// the number of patches used for gammut compresssion
#define NUM_STD_PATCHES	PRINT_PATCHES

// The number of additional patches that are near neutral
#define NUM_NEUTRAL_PATCHES 0//(24)

#ifdef SMALL_CMYK_PATCH

// The number of black patches
#define NUM_BLACK_PATCHES  6//(9)

// The number of patches with both CMY and K
#define NUM_CMYK_PATCHES 108

#endif

// The number of patches used for calibration
#define NUM_CALIB_PATCHES (NUM_STD_PATCHES+NUM_NEUTRAL_PATCHES)

// The number of patches minus the black only patches, these are present in all files
// = 837
#define NUM_OLD_TOTAL_PATCHES (NUM_STD_PATCHES+NUM_NEUTRAL_PATCHES+NUM_CMYK_PATCHES)


// The toatl number of patches  = 843
#define NUM_TOTAL_PATCHES (NUM_STD_PATCHES+NUM_NEUTRAL_PATCHES+NUM_BLACK_PATCHES+NUM_CMYK_PATCHES)

// The toatl number of patches  = 729
#define NUM_TOTAL_RGB_PATCHES (NUM_STD_PATCHES+NUM_NEUTRAL_PATCHES)


// The following are used for reading in strips

#define PATCH_PER_STRIP_LETTER 	(17)
#define PATCH_PER_STRIP_TABLOID 	(28)

#define STRIPS_PER_SHEET		(6)

#define STRIPS_PER_SHEET_TAB	(6)

#define BLACK_PATCHES {0,0.20,0.50,0.70,0.90}
#define BLACK_PATCHES_I {0,20,50,70,90}

//#define BLACK_PATCHES {0,0.20,0.40,0.60,0.80}
//#define BLACK_PATCHES_I {0,20,40,60,80}

#endif	//MONACO_PRINT_H
