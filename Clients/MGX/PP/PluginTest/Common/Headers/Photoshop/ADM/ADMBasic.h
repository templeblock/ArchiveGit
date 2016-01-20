/*
 * Name:
 *	ADMBasic.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	ADM Basic Suite.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 3/9/1996	DL	First version.
 *		Created by Dave Lazarony.
 */

#ifndef __ADMBasic__
#define __ADMBasic__

/*
 * Includes
 */
 
#ifndef __ADMTypes__
#include "ADMTypes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma PRAGMA_ALIGN_BEGIN
#pragma PRAGMA_IMPORT_BEGIN

/*
 * Constants
 */

#define kADMBasicSuite			"ADM Basic Suite"

#define kADMBasicSuiteVersion2	2
#define kADMBasicSuiteVersion3	3
#define kADMBasicSuiteVersion	kADMBasicSuiteVersion2
#define kADMBasicVersion		kADMBasicSuiteVersion

/*
 * Types
 */

typedef enum
{
	kADMNoAnswer = 0,
	kADMYesAnswer,
	kADMCancelAnswer,

	kADMDummyAnswer = 0xFFFFFFFF
}
ADMAnswer;


// ADMAnswer equivalents
typedef enum
{
	kADMLeftAnswer = 0,
	kADMRightAnswer,
	kADMMiddleAnswer,
	
	kADMDummyChoice = 0xFFFFFFFF
}
ADMChoice;


#ifdef MAC_ENV
typedef struct
{
	unsigned long *types;
	short numTypes;
} ADMPlatformFileTypesSpecification;
typedef struct
{
	unsigned long *types;
	short numTypes;
	char filter[256];
} ADMPlatformFileTypesSpecification3;
#else
typedef struct
{
	char filter[256];
} ADMPlatformFileTypesSpecification, ADMPlatformFileTypesSpecification3;
#endif

/*
 * Basic Suite
 */
typedef struct ADMBasicSuite2
{
	int ASAPI (*GetIndexString)(SPPluginRef pluginRef, int stringID, int stringIndex, char *string, int maxLen);

	void ASAPI (*Beep)(void);
	
	void ASAPI (*ErrorAlert)(char *errorString);
	void ASAPI (*MessageAlert)(char *messageString);
	ADMAnswer ASAPI (*QuestionAlert)(char *questionString);

	ASBoolean ASAPI (*GetScreenDimensions)(ASPoint *point, ASRect *dimensions);

	void ASAPI (*EnableToolTips)(ASBoolean enable);
	ASBoolean ASAPI (*AreToolTipsEnabled)(void);
	
	void ASAPI (*ValueToString)(float value, char *text, int maxLen, ADMUnits units, int precision, ASBoolean alwaysAppendUnits);
	ASBoolean ASAPI (*StringToValue)(const char *text, float *value, ADMUnits units);

	ASBoolean ASAPI (*StandardGetFileDialog)(char *message, ADMPlatformFileTypesSpecification *filter, SPPlatformFileSpecification *startingDir, char *startingFile, SPPlatformFileSpecification *result);
	ASBoolean ASAPI (*StandardGetDirectoryDialog)(char *message, SPPlatformFileSpecification *startingDir, SPPlatformFileSpecification *result);
	ASBoolean ASAPI (*StandardPutFileDialog)(char *message, SPPlatformFileSpecification *startingDir, char *startingFile, SPPlatformFileSpecification *result);

	ASBoolean ASAPI (*ChooseColor)( ASPoint where, const ASRGBColor *inColor, ASRGBColor *outColor );	

	void ASAPI (*SetAppUnits)(ADMUnits appUnits, ADMUnits actualUnits);
	ADMUnits ASAPI (*GetAppUnits)(ADMUnits appUnits);
	void ASAPI (*SetNumbersArePoints)(ASBoolean points);
	ASBoolean ASAPI (*GetNumbersArePoints)(void);

	void ASAPI (*SetDefaultIncrements)(ADMUnits units, float smallIncr, float largeIncr);
	void ASAPI (*GetDefaultIncrements)(ADMUnits units, float *smallIncr, float *largeIncr);

	//This probably should move up with the other dialog calls (eg ErrorAlert)
	void ASAPI (*AboutBox)(SPPluginRef plugin, char *text1, char *text2);

	// move up to by other tool tips functions
	ASBoolean ASAPI (*AreToolTipsSticky)(ADMDialogRef dialog);
	void ASAPI (*GetToolTipDelays)(int *popupDelay, int *popdownDelay);

	// move up by GetScreenDimensions()
	ASBoolean ASAPI (*GetWorkspaceBounds)(ASRect *dimensions);		// main screen less system menus, task bars, etc
	ASBoolean ASAPI (*GetPaletteLayoutBounds)(ASRect *dimensions);	// inset of standard application window
	
} ADMBasicSuite2;	


typedef struct ADMBasicSuite3
{
	int ASAPI (*GetIndexString)(SPPluginRef pluginRef, int stringID, int stringIndex, char *string, int maxLen);

	void ASAPI (*Beep)(void);
	
	void ASAPI (*ErrorAlert)(char *errorString);
	void ASAPI (*MessageAlert)(char *messageString);
	ADMAnswer ASAPI (*QuestionAlert)(char *questionString);

	ASBoolean ASAPI (*GetScreenDimensions)(ASPoint *point, ASRect *dimensions);

	void ASAPI (*EnableToolTips)(ASBoolean enable);
	ASBoolean ASAPI (*AreToolTipsEnabled)(void);
	
	void ASAPI (*ValueToString)(float value, char *text, int maxLen, ADMUnits units, int precision, ASBoolean alwaysAppendUnits);
	ASBoolean ASAPI (*StringToValue)(const char *text, float *value, ADMUnits units);

	ASBoolean ASAPI (*StandardGetFileDialog)(char *message, ADMPlatformFileTypesSpecification3 *filter, SPPlatformFileSpecification *startingDir, char *startingFile, SPPlatformFileSpecification *result);
	ASBoolean ASAPI (*StandardGetDirectoryDialog)(char *message, SPPlatformFileSpecification *startingDir, SPPlatformFileSpecification *result);
	ASBoolean ASAPI (*StandardPutFileDialog)(char *message, SPPlatformFileSpecification *startingDir, char *startingFile, SPPlatformFileSpecification *result);

	ASBoolean ASAPI (*ChooseColor)( ASPoint where, const ASRGBColor *inColor, ASRGBColor *outColor );	

	void ASAPI (*SetAppUnits)(ADMUnits appUnits, ADMUnits actualUnits);
	ADMUnits ASAPI (*GetAppUnits)(ADMUnits appUnits);
	void ASAPI (*SetNumbersArePoints)(ASBoolean points);
	ASBoolean ASAPI (*GetNumbersArePoints)(void);

	void ASAPI (*SetDefaultIncrements)(ADMUnits units, float smallIncr, float largeIncr);
	void ASAPI (*GetDefaultIncrements)(ADMUnits units, float *smallIncr, float *largeIncr);

	//This probably should move up with the other dialog calls (eg ErrorAlert)
	void ASAPI (*AboutBox)(SPPluginRef plugin, char *text1, char *text2);

	// move up to by other tool tips functions
	ASBoolean ASAPI (*AreToolTipsSticky)(ADMDialogRef dialog);
	void ASAPI (*GetToolTipDelays)(int *popupDelay, int *popdownDelay);

	// move up by GetScreenDimensions()
	ASBoolean ASAPI (*GetWorkspaceBounds)(ASRect *dimensions);		// main screen less system menus, task bars, etc
	ASBoolean ASAPI (*GetPaletteLayoutBounds)(ASRect *dimensions);	// inset of standard application window

	void ASAPI (*SetPlatformCursor)(SPPluginRef pluginRef, int nCursorID);
	void ASAPI (*PluginAboutBox)(char *title, char *text);
	
	// call before an alert, defaults are reset automatically
	void ASAPI (*SetAlertButtonText)(char *leftChoice, char* middleChoice, char*rightChoice);	
	
	void ASAPI (*ShowToolTip)(ASPoint *where, char *tip);
	void ASAPI (*HideToolTip)(void);
	
} ADMBasicSuite3;

typedef ADMBasicSuite2 ADMBasicSuite;

#pragma PRAGMA_IMPORT_END
#pragma PRAGMA_ALIGN_END

#ifdef __cplusplus
}
#endif


#endif








