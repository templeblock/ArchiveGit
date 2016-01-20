/***************************************************************************/
/*					 IBM Continuous Speech Series  1.1					   */
/***************************************************************************/
/*																		   */
/*	  Licensed Materials - Property of IBM								   */
/*	  IBM Continuous Speech Series for Windows							   */
/*	  (C) Copyright IBM Corporation 1992, 1994. All rights reserved.	   */
/*																		   */
/*	  US Government Users Restricted Rights -							   */
/*	  Use, duplication or disclosure restricted by GSA ADP Schedule		   */
/*	  Contract with IBM Corporation.									   */
/*																		   */
/*=========================================================================*/
/*																		   */
/* Program:			  IBM Continuous Speech System (ICSS)				   */
/*																		   */
/* Component:		  Common											   */
/*																		   */
/* Source File Name:  icsswapi.h										   */
/*																		   */
/* Description:															   */
/* This header file is for use by application programs interfacing		   */
/* with the Application Programming Interface of the IBM Continuous		   */
/* Speech Series. It includes a series of data definitions, data		   */
/* types, and function prototypes. It must be "included" in those		   */
/* programs using the API.												   */
/*																		   */
/* Functions:															   */
/*																		   */
/* Comments:															   */
/*																		   */
/* Modification History:												   */
/* Level	 Date	   Programmer  Reason								   */
/* --------	 --------  ----------  --------------------------------------- */
/* 000.000	 03/11/93  bjtb		   Initial version containing header.	   */
/* 000.000	 04/15/93  SLC		   Add new return code for ISTG Start and  */
/*								   End conversation errors.				   */
/* 000.00	 05/10/93  SLC		   Reorganize return codes and provide	   */
/*								   return code cross-ref. to API calls.	   */
/* 000.000	 05/18/93  SLC		   Fix PTR 484 and 481.					   */
/* 000.000	 06/04/93  SLC		   Fix PTR616.							   */
/*																		   */
/* 000.000	 06/30/93  BJTB		   modified for windows use				   */
/* 000.000	 09/23/93  SLC		   Updated Windows routines linkage types  */
/* 000.000	 11/07/93  SLC		   Added Windows ICSS_ERR_WIN_NO_CLIENT	   */
/* 000.000	 04/19/94  SLC		   Added support for OEM runtime.		   */
/* 000.000	 04/19/94  bjtb		   altered loadcontext to be sync.,		   */
/*								   playback and GSW to remove parms. added */
/*								   flags to GSW return structure.		   */
/*			 4/94-5/94 bjtb		   notification support.				   */
/*			 05/24/94  bjtb		   Added Noise Sensitivity.				   */
/*			 06/14/94  SLC		   Added AllowableNoiseCount, ECR????	   */
/*			 06/14/94  SLC		   Added error code for no WIN32s support  */
/*								   for PTR1248.							   */
/* 000.000	 06/16/94  SLC		   Added error code 76 for ECR1264, and	   */
/*								   changed error code 65 for PTR1249.	   */
/*			 06/23/94  bjtb		   Took out error code 76 for ECR1346	   */
/*								   (was called ICSS_ERR_ALL_NOISE_DETECTED)*/
/* 000.000	 07/01/94  OW		   Revised error strings for better		   */
/*								   readability. PTR1224					   */
/* 000.000	 10/07/94  SLC		   Revised calling sequence options for	   */
/*								   ICSSStart to fix PTR1629.			   */
/* 000.000	 10/17/94  RDV		   Updated comments.  ptf 1250			   */
/* 000.000	 11/29/94  SLC		   Added new Get/Set Value for context	   */
/*								   path.   PTR1704						   */
/*=========================================================================*/

/*********************************************************************/
/*																	 */
/* This section defines all valid return codes from the IBM			 */
/* Continuous Speech Series API.									 */
/*																	 */
/* In the following section of return codes, the comment section to	 */
/* the right of each return code contains two pieces of important	 */
/* information. To the left of the ":" is a number indicating the	 */
/* decimal value of this symbolic return code. To the right of the	 */
/* colon is a series of numbers that indicate which API calls		 */
/* issue this return code. These numbers on the right correspond to	 */
/* the numbers to the left of each API call in the following list:	 */
/*																	 */
/*	 1)	 ICSSStart													 */
/*	 2)	 ICSSStartConversation										 */
/*	 3)	 ICSSGetValue												 */
/*	 4)	 ICSSSetValue												 */
/*	 5)	 ICSSLoadContext											 */
/*	 6)	 ICSSRemoveContext											 */
/*	 7)	 ICSSListen													 */
/*	 8)	 ICSSInterruptListenState									 */
/*	 9)	 ICSSGetSpokenWords											 */
/*	10)	 ICSSPlayback												 */
/*	11)	 ICSSEndConversation										 */
/*	12)	 ICSSEnd													 */
/*	13)	 ICSSThresholdRecordInit									 */
/*	14)	 ICSSThresholdRecordTerm									 */
/*	15)	 ICSSThresholdRecordSpeech									 */
/*	16)	 ICSSThresholdDisplayWave									 */
/*	17)	 ICSSThresholdWindowAnnotation								 */
/*	18)	 ICSSThresholdDrawFrame										 */
/*	19)	 ICSSThresholdCalculate										 */
/*	20)	 ICSSThresholdDialog										 */
/*	21)	 ICSSWordInit												 */
/*	22)	 ICSSWordRecord												 */
/*	23)	 ICSSWordAdd												 */
/*	24)	 ICSSWordCompile											 */
/*	25)	 ICSSWordTerm												 */
/*	26)	 ICSSListenMultCtx											 */
/*																	 */
/*********************************************************************/

// Library was built for "C"
extern "C"
{

typedef enum {
  ICSS_SUCCESS,									 /*	 0 : 1 2 3 4 5 6 7 8 9 10 11 12 26	 */
  ICSS_ERR_ICSS_NOT_ACTIVE,						 /*	 1 :   2 3 4 5 6 7 8 9 10 11 12 26	 */
  ICSS_ERR_ICSS_ALREADY_INIT,					 /*	 2 : 1								 */
  ICSS_ERR_INITIALIZED,							 /*	 3 :	 3 4 5 6 7	 9 10 11	26	 */
  ICSS_ERR_CONV_ACTIVE,							 /*	 4 : 1				 9		 12		 */
  ICSS_ERR_CONV_ALREADY_STARTED,				 /*	 5 :   2							 */
  ICSS_ERR_LISTENING,							 /*	 6 : 1 2 3 4 5 6 7	   10 11 12 26	 */
  ICSS_ERR_INIT_MSG,							 /*	 7 : 1								 */
  ICSS_ERR_INIT_GLOBAL,							 /*	 8 : 1								 */
  ICSS_ERR_INIT_SEMAPHORE,						 /*	 9 : 1								 */

  ICSS_ERR_INIT_GLOBAL_DATA,					 /* 10 : 1								 */
  ICSS_ERR_INIT_ISTG_FAILED,					 /* 11 : 1								 */
  ICSS_ERR_INIT_POOL_SERVICES,					 /* 12 : 1								 */
  ICSS_ERR_TERM_ISTG_FAILED,					 /* 13 :						 12		 */
  ICSS_ERR_LOAD_ADC_CODE,						 /* 14 : 1 2 (1 for AIX, 2 for OS/2)	 */
  ICSS_ERR_INVALID_ADCNUMBER,					 /* 15 :   2							 */
  ICSS_ERR_INVALID_RPLINKTYPE,					 /* 16 :   2							 */
  ICSS_ERR_CONTEXT_NOT_LOADED,					 /* 17 :		 5						 */
  ICSS_ERR_CODEBOOKS_NOT_READ,					 /* 18 :		 5						 */
  ICSS_ERR_MAX_CTX_REACHED,						 /* 19 :		 5						 */

  ICSS_ERR_INVALID_PARAMETER_ID,				 /* 20 :	 3 4						 */
  ICSS_ERR_INVALID_PARM_VALUE,					 /* 21 :	   4						 */
  ICSS_ERR_CONTEXT_NOT_AVAILABLE,				 /* 22 :		   6 7				 26	 */
  ICSS_ERR_INVALID_INSERTION_PENALTY,			 /* 23 :		 5						 */
  ICSS_ERR_INVALID_LANGUAGE_WEIGHT,				 /* 24 :		 5						 */
  ICSS_ERR_INVALID_GRAMMAR_WEIGHT,				 /* 25 :		 5						 */
  ICSS_ERR_INVALID_SPEECH_INPUT_SOURCE,			 /* 26 :			 7				 26	 */
  ICSS_ERR_INVALID_SPEECH_INPUT_FILENAME,		 /* 27 :			 7				 26	 */
  ICSS_ERR_SPEECH_INPUT_SOURCE,					 /* 28 :			 7				 26	 */
  ICSS_ERR_INVALID_SAMPLE_RATE,					 /* 29 :				   10			 */

  ICSS_ERR_PLAYBACK_FAILED,						 /* 30 :				   10			 */
  ICSS_ERR_EMPTY_RESPONSE,						 /* 31 :				 9				 */
  ICSS_ERR_TRUNCATED_RESPONSE,					 /* 32 :				 9				 */
  ICSS_ERR_EXCEEDED_MAX_SIL,					 /* 33 :				 9				 */
  ICSS_ERR_EXCEEDED_MAX_TALK,					 /* 34 :				 9				 */
  ICSS_ERR_LISTEN_STATE_INTERRUPTED,			 /* 35 :				 9				 */
  ICSS_ERR_UNLOAD_ADC_CODE,						 /* 36 :(11 for OS2, 12 - AIX)11 12		 */
  ICSS_ERR_ISTG_START_CONVERSATION,				 /* 37 :   2							 */
  ICSS_ERR_ISTG_END_CONVERSATION,				 /* 38 :					  11		 */
  ICSS_ERR_ISTG_FAILED,							 /* 39 :			 7	 9			 27	 */

  ICSS_ERR_FBS_SEMAPHORE,						 /* 40 :   2	 5 6	 9	  11		 */
  ICSS_ERR_FBS_SHARED_MEMORY,					 /* 41 :   2	 5 6	 9	  11		 */
  ICSS_ERR_FBS_MEMORY,							 /* 42 :   2	 5		 9				 */
  ICSS_ERR_FBS_UNKNOWN_CONTEXT,					 /* 43 :		 5 6					 */
  ICSS_ERR_FBS_NOT_SETUP,						 /* 44 :		 5 6	 9	  11		 */
  ICSS_ERR_FBS_NULL_REQUEST,					 /* 45 :		 5		 9				 */
  ICSS_ERR_FBS_MAX_CONTEXTS,					 /* 46 :		 5						 */
  ICSS_ERR_FBS_OPEN_CONTEXT,					 /* 47 :		 5						 */
  ICSS_ERR_FBS_READ_CONTEXT,					 /* 48 :		 5						 */
  ICSS_ERR_FBS_OPEN_CPDFILE,					 /* 49 :		 5						 */

  ICSS_ERR_FBS_INCOMPATIBLE_CPDFILE,			 /* 50 :		 5						 */
  ICSS_ERR_FBS_INVALID_CONTEXT,					 /* 51 :		 5						 */
  ICSS_ERR_FBS_TOO_MANY_CPDFILES,				 /* 52 :		 5						 */
  ICSS_ERR_FBS_READ_CPDFILE,					 /* 53 :		 5						 */
  ICSS_ERR_FBS_BUFFER_OVERFLOW,					 /* 54 :				 9				 */
  ICSS_ERR_FBS_RPC_FAILURE,						 /* 55 :   2	 5 6	 9	  11		 */
  ICSS_ERR_FBS_RPC_INVALID_HOST,				 /* 56 :   2							 */
  ICSS_ERR_FBS_RPC_SERVER_UNAVAIL,				 /* 57 :   2							 */
  ICSS_ERR_FBS,									 /* 58 :   2	 5 6	 9	  11		 */
  ICSS_ERR_INVALID_ICSSDIR,						 /* 59 : 1								 */

  ICSS_ERR_INVALID_RPSYSTEMNAME,				 /* 60 :   2							 */
  ICSS_ERR_SHOULD_NOT_OCCUR,					 /* 61 :								 */
  ICSS_ERR_WIN_SERVER_START,					 /* 62 : 1 2 3 4 5 6 7 8 9 10 11 12 26	 */
  ICSS_ERR_WIN_API_MEMORY,						 /* 63 : 1 2 3 4 5 6 7 8 9 10 11 12 26	 */
  ICSS_ERR_ASYNC_IN_PROGRESS,					 /* 64 : 1 2 3 4 5 6 7	 9 10 11 12 26	 */
  ICSS_ERR_WIN_NO_SEARCH,						 /* 65 :   2	 5 6	 9	  11		 */
  ICSS_ERR_ADC_OPEN,							 /* 66 :			 7				26	 */
  ICSS_ERR_ADC_CLOSE,							 /* 67 :				 9				 */
  ICSS_ERR_ADC_BUSY,							 /* 68 :			 7				26	 */
  ICSS_ERR_ADC_READ,							 /* 69 :				 9				 */

  ICSS_ERR_ADC_FILE_IO,							 /* 70 :				 9				 */
  ICSS_ERR_UNSUPPORTED_VERSION,					 /* 71 : 1								 */
  ICSS_ERR_UNLICENSED_VERSION,					 /* 72 :   2							 */
  ICSS_ERR_RECO_CHECK_FLAGS,					 /* 73 :				 9				 */
  ICSS_ERR_INVALID_NOISE_SENSITIVITY,			 /* 74 :		 5						 */
  ICSS_ERR_WIN32S_NOT_AVAILABLE,				 /* 75 : 1								 */
  ICSS_TS_ERR_ALLOC,							 /* 76 : 13 15 16 19 */
  ICSS_TS_ERR_INIT,								 /* 77 : 13			 */
  ICSS_TS_ERR_FREE,								 /* 78 : 14			 */
  ICSS_TS_ERR_CLEANUP,							 /* 79 : 14			 */

  ICSS_TS_ERR_OPEN,								 /* 80 : 15			 */
  ICSS_TS_ERR_READ,								 /* 81 : 15			 */
  ICSS_TS_ERR_STOP,								 /* 82 : 15			 */
  ICSS_TS_ERR_CLOSE,							 /* 83 : 15			 */
  ICSS_TS_ERR_OPEN_FILE,						 /* 84 : 15			 */
  ICSS_TS_ERR_CLOSE_FILE,						 /* 85 : 15			 */
  ICSS_TS_ERR_READ_FILE,						 /* 86 : 15			 */
  ICSS_TS_ERR_WRITE_FILE,						 /* 87 : 15			 */
  ICSS_TS_ERR_READ_WAVE1,						 /* 88 : 19			 */
  ICSS_TS_ERR_READ_WAVE2,						 /* 89 : 19			 */

  ICSS_TS_ERR_DATA_SIZE,						 /* 90 : 19			 */
  ICSS_TS_ERR_LOCK,								 /* 91 : 19			 */
  ICSS_TS_ERR_POOR_SIGNAL_NOISE,				 /* 92 : 19			 */
  ICSS_TS_ERR_POSSIBLE_CLIPPING,				 /* 93 : 19			 */
  ICSS_TS_ERR_SIGNAL_CLIPPING,					 /* 94 : 19			 */

  ICSS_WORD_ERR_NO_SERVER,						 /* 95 : 21 22 23 24 25 */
  ICSS_WORD_ERR_MEM_ALLOC,						 /* 96 : 21	   23		*/
  ICSS_WORD_ERR_OPEN_BIGRAM,					 /* 97 :	   23		*/
  ICSS_WORD_ERR_OPEN_DICT,						 /* 98 : 21	   23		*/
  ICSS_WORD_ERR_MEM_LOCK,						 /* 99 :	   23 24	*/
  ICSS_WORD_ERR_COMPILER,						 /* 100: 21	   23 24	*/
  ICSS_WORD_ERR_COMPILE_FAILED,					 /* 101:	   23 24	*/
  ICSS_WORD_ERR_SPOKE_TOO_SOON,					 /* 102:	   23		*/
  ICSS_WORD_ERR_LOAD_RULES,						 /* 103: 21				*/
  ICSS_WORD_ERR_NO_SPEECH,						 /* 104:	22			*/
  ICSS_WORD_ERR_RECORD_FAILED,					 /* 105:	22			*/
  ICSS_WORD_ERR_UNKNOWN_EXCEP,					 /* 106: 21				*/
  ICSS_WORD_ERR_NOT_INIT,						 /* 107:	22 23 24 25 */
  ICSS_WORD_ERR_NO_DEFINITION,					 /* 108:	   23		*/
  ICSS_WORD_ERR_INVALID_CHAR,					 /* 109:	   23		*/
  ICSS_WORD_ERR_TOO_LONG,						 /* 110:	   23		*/
  ICSS_WORD_ERR_BAD_PARM_ONE,					 /* 111: 21 22 23 24	*/
  ICSS_WORD_ERR_BAD_PARM_TWO,					 /* 112: 21 22 23 24	*/
  ICSS_WORD_ERR_BAD_PARM_THREE,					 /* 113: 21 22 23 24	*/
  ICSS_WORD_ERR_BAD_PARM_FOUR,					 /* 114: 21 22 23 24	*/
  ICSS_WORD_ERR_BAD_PARM_FIVE,					 /* 115:	22	  24	*/
  ICSS_WORD_ERR_BAD_PARM_SIX,					 /* 116:		  24	*/
  ICSS_WORD_ERR_BAD_PARM_SEVEN,					 /* 117:		  24	*/
  ICSS_WORD_ERR_BAD_PARM_EIGHT,					 /* 118:		  24	*/
  ICSS_WORD_ERR_BAD_PARM_NINE,					 /* 119:		  24	*/

  ICSS_MAXIMUM_ERROR_NUMBER

} ICSS_RETURNCODES;


#define ICSS_WORD_SUCCESS	ICSS_SUCCESS

/*********************************************************************/
/*																	 */
/* This section will, be default, define an array of character		 */
/* pointers to strings that identify each of the return codes		 */
/* defined in the previous section. This is very useful for programs */
/* using the API to print the associated errors, rather than just a	 */
/* number that has to be looked up. Space may be conserved by		 */
/* turning off the definition of these items by defining a macro	 */
/* by the name of _NO_ICSS_ERROR_STRINGS_.							 */
/*																	 */
/*********************************************************************/

#if defined(INCLUDE_ICSS_ERROR_STRINGS)

char	  *ICSSErrorString[] = {
/*	0 */	"Function completed successfully.",
/*	1 */	"The speech recognition engine has not been started.",
/*	2 */	"The speech recognition engine is already running.",
/*	3 */	"The speech recognition engine is already running, but no conversation is active.",
/*	4 */	"A conversation is still active between the program and the speech engine.",
/*	5 */	"A conversation is already active between the program and the speech engine.",
/*	6 */	"The system is still listening for speech - speech function disregarded.",
/*	7 */	"The speech engine cannot start message services and cannot continue.",
/*	8 */	"The speech engine cannot acquire its shared memory data.",
/*	9 */	"The speech engine cannot start its services for synchronizing processes.",
/* 10 */	"The speech engine cannot initialize its global data and cannot start.",

/* 11 */	"The speech engine cannot start its multitasking services.",
/* 12 */	"The speech engine cannot allocate its data buffers.",
/* 13 */	"The speech engine cannot stop its multitasking services while ending the program.",
/* 14 */	"The speech engine cannot load audio support - ensure that the audio drivers are loaded.",
/* 15 */	"An invalid sound card number has been specified - check the Configuration Profile.",
/* 16 */	"The RPC connection type, specified by ICSSStartConversaton, is not 'direct' or 'RPC'.",
/* 17 */	"The requested context cannot be loaded into memory - its phrases will not be active.",
/* 18 */	"The files supporting the context sample rate cannot be found or are corrupted.",
/* 19 */	"Not enough memory available to load another context. Remove unnecessary contexts.",

/* 20 */	"A system value name in ICSSGetValue or ICSSSetValue is not valid.",
/* 21 */	"The value specified in ICSSSetValue is not valid.",
/* 22 */	"The specified speech context is not loaded in memory.",
/* 23 */	"The Insertion Penalty specified in ICSSLoadContext is not valid.",
/* 24 */	"The Language Weight specified in ICSSLoadContext is not valid.",
/* 25 */	"The Grammar Weight specified in ICSSLoadContext is not valid.",
/* 26 */	"The Speech Input Source specified in ICSSListen is not valid.",
/* 27 */	"The Speech Input Filename specified in ICSSListen is not valid.",
/* 28 */	"The Speech Input device specified in ICSSListen cannot be opened.",
/* 29 */	"The Sample Rate specified in ICSSPlayback is not compatible with the sample rate of the sound file.",

/* 30 */	"Unable to play the specified sound file.",
/* 31 */	"The speech engine heard no words - empty response.",
/* 32 */	"The phrase which the speech engine recognized is too long - the response is truncated.",
/* 33 */	"The maximum allowable silence time has been exceeded - empty response.",
/* 34 */	"The maximum allowable talk time has been exceeded - empty response.",
/* 35 */	"The speech engine has been interrupted from listening by ICSSInterruptListenState.",
/* 36 */	"Cannot unload the audio support module from memory.",
/* 37 */	"ICSSStartConversation failed to establish a conversation.",
/* 38 */	"ICSSStartConversation failed to end a conversation.",
/* 39 */	"The speech engine's multitasking services failed and are attempting to restart.",

/* 40 */	"A synchronization error occurred while attempting to recognize speech.",
/* 41 */	"A shared memory error occurred while attempting to recognize speech.",
/* 42 */	"A memory management error occurred while attempting to recognize speech.",
/* 43 */	"The context represented by the context handle cannot be found.",
/* 44 */	"Recognition cannot occur until the speech engine is initialized.",
/* 45 */	"The speech engine cannot understand the ICSSLoadContext request.",
/* 46 */	"The speech engine cannot load another context - maximum number reached.",
/* 47 */	"The speech engine cannot open the requested context.",
/* 48 */	"The speech engine cannot read the requested context.",
/* 49 */	"The speech engine cannot open the speech models. They may be corrupted or missing.",

/* 50 */	"The requested context is not compatible with the speech models.",
/* 51 */	"The requested context is not valid.",
/* 52 */	"The speech engine detected too many speech model files.",
/* 53 */	"The speech engine cannot read the speech models.  They may be corrupted or missing.",
/* 54 */	"The speech engine detected a speech buffer overflow.",
/* 55 */	"The remote speech server session has been severed.",
/* 56 */	"The remote speech server system cannot not be found.",
/* 57 */	"The remote speech server is not available on the specified system.",
/* 58 */	"An error occurred during speech recognition processing.",
/* 59 */	"The speech directory location specified by ICSSDIR is not valid.",

/* 60 */	"The specified remote system name specified in ICSSStartConversation is not valid.",
/* 61 */	"The speech engine encountered an error that should not occur.",
/* 62 */	"The speech recognition server cannot be started or found.",
/* 63 */	"A speech engine API memory error occurred.",
/* 64 */	"A speech engine asynchronous function is already running.",
/* 65 */	"The speech search engine cannot be started/found.",
/* 66 */	"The speech engine cannot open the audio device.",
/* 67 */	"The speech engine cannot close the audio device.",
/* 68 */	"The audio device is busy.",
/* 69 */	"The speech engine cannot read data from the audio device.",

/* 70 */	"The speech engine cannot perform audio file I/O.",
/* 71 */	"Cannot start the speech engine. Application requires a different version of the speech engine.",
/* 72 */	"Application requires a licensed OEM or unrestricted runtime version of the speech engine.",
/* 73 */	"Recognized speech may not be valid - check flags set by ICSSGetSpokenWords.",
/* 74 */	"The Noise Sensitivity specified in ICSSLoadContext is not valid.",
/* 75 */	"Required Win32S support, V1.1.88.0 or later, is unavailable.",

/* 76 */	"Threshold API memory allocation error.",
/* 77 */	"Threshold API record initialization error.",
/* 78 */	"Threshold API memory free error.",
/* 79 */	"Threshold API record cleanup error.",

/* 80 */	"Threshold API record open error.",
/* 81 */	"Threshold API record read error.",
/* 82 */	"Threshold API record stop error.",
/* 83 */	"Threshold API record close error.",
/* 84 */	"Threshold API file open error.",
/* 85 */	"Threshold API file close error.",
/* 86 */	"Threshold API file read error.",
/* 87 */	"Threshold API file write error.",
/* 88 */	"Threshold API file format error.",
/* 89 */	"Threshold API file format error.",

/* 90 */	"Threshold API file format error.",
/* 91 */	"Threshold API memory lock error.",
/* 92 */	"Threshold API poor signal to noise ratio.",
/* 93 */	"Threshold API possible clipping.",
/* 94 */	"Threshold API possible clipping and bad signal/noise ratio.",
/* 95 */	"The Addword Server is not running.",
/* 96 */	"The Addword Server is unable to allocate memory for internal buffers.",
/* 97 */	"The Addword Server is unable to create its internal bigram file.",
/* 98 */	"The Addword Server is unable to open the standard dictionary, or an addendum dictionary.",
/* 99 */	"The Addword Server is unable to lock memory required for internal buffers.",
/*100 */	"The Addword Grammar Compiler is not running.",
/*101 */	"The Addword Grammar Compiler encountered an error while compiling a bigram or BNF file.",
/*102 */	"The user began speaking before the system was ready for input.",
/*103 */	"The Addword Server is unable to load its word-parsing rules.",
/*104 */	"The user did not begin speaking within the allotted time.",
/*105 */	"The Addword Server is unable to record the file.",
/*106 */	"An unidentified program exception occurred.",
/*107 */	"The Addword Server is not been initialized.",
/*108 */	"The Addword Server is unable to generate a phonetic definition for the word.",
/*109 */	"The input word contains characters which are not valid.",
/*110 */	"The input word is too long.",
/*111 */	"The first parameter for the API function is not valid.",
/*112 */	"The second parameter for the API function is not valid.",
/*113 */	"The third parameter for the API function is not valid.",
/*114 */	"The fourth parameter for the API function is not valid.",
/*115 */	"The fifth parameter for the API function is not valid.",
/*116 */	"The sixth parameter for the API function is not valid.",
/*117 */	"The seventh parameter for the API function is not valid.",
/*118 */	"The eighth parameter for the API function is not valid.",
/*119 */	"The ninth parameter for the API function is not valid.",

			"Maximum Error Number"
		  };
#endif

/*********************************************************************/
/*																	 */
/* This section defines all valid parameter ids used by the IBM		 */
/* Continuous Speech Series API functions ICSSGetValue and			 */
/* ICSSSetValue. These parameter types are used to identify the		 */
/* parameter to be retrieved/affected by the function invocation.	 */
/* Please refer to ICSS for Windows Technical Reference for more	 */
/* detailed information on the meaning of each parameter id.		 */
/*																	 */
/*********************************************************************/

typedef enum {
  ICSS_FIRST_INT,

  ICSS_CINT_VERBOSE_LEVEL = 10,
  ICSS_CINT_ALLOWABLE_NOISE_COUNT,

  ICSS_CINT_UTT_START_THRESHOLD = 20,
  ICSS_CINT_UTT_END_THRESHOLD,

  ICSS_CINT_ATTN_START_THRESHOLD = 30,
  ICSS_CINT_ATTN_END_THRESHOLD,

  ICSS_FIRST_FLOAT =100,

  ICSS_CFLT_BEAM_WIDTH = 110,

  ICSS_CFLT_UTT_MIN_DURATION = 120,
  ICSS_CFLT_UTT_MAX_DURATION,
  ICSS_CFLT_UTT_MAX_SIL_DURATION,
  ICSS_CFLT_UTT_END_SIL_DURATION,

  ICSS_CFLT_ATTN_MIN_DURATION = 130,
  ICSS_CFLT_ATTN_MAX_DURATION,
  ICSS_CFLT_ATTN_INTERUTT_DURATION,

  ICSS_FIRST_STRING = 200,

  ICSS_CSTR_RECORD_SESSION_NAME = 210,
  ICSS_CSTR_RECORD_SPEECH_NAME,
  ICSS_CSTR_FUTURE,
  ICSS_CSTR_RECORD_VQ_NAME,
  ICSS_CSTR_CONTEXT_PATH,

  ICSS_FIRST_HANDLE = 300,

  ICSS_CHDL_TALK_START = 310,
  ICSS_CHDL_TALK_END,
  ICSS_CHDL_SPOKE_TOO_SOON,
  ICSS_CHDL_UTT_TOO_SHORT,
  ICSS_CHDL_UTT_TOO_LONG,

  ICSS_LAST_ID
} ICSS_PARAMETERIDS;

/*********************************************************************/
/*																	 */
/* This section defines a union to be used when receiving data from/ */
/* providing data to the IBM Continuous Speech Series API functions	 */
/* ICSSGetValue or ICSSSetValue.									 */
/*																	 */
/*********************************************************************/

typedef union {
  unsigned long ICSS_HANDLEParameterData;
  long			ICSS_INTParameterData;
  float			ICSS_FLOATParameterData;
  char			ICSS_STRINGParameterData[256];
} ICSS_PARMTYPE;

/*********************************************************************/
/*																	 */
/* This section defines all default value indicators from the IBM	 */
/* Continuous Speech Series API function ICSSSetValue. These are used*/
/* by the application to inform ICSS to reset the specified value	 */
/* back to its current default.										 */
/*																	 */
/*********************************************************************/

#define ICSS_DEFAULT_HANDLE	 0
#define ICSS_DEFAULT_INT	 (-1)
#define ICSS_DEFAULT_FLOAT	 ((float) -2.0)
#define ICSS_DEFAULT_STRING	 ""

/*********************************************************************/
/*																	 */
/* This section defines a set of symbolic values to use when		 */
/* invoking the IBM Continuous Speech Series API. These are used	 */
/* by the application to indicate certain conditions or options.	 */
/* Please refer to ICSS for Windows Technical Reference for more	 */
/* detailed information on the meaning of each symbolic value.		 */
/*																	 */
/*********************************************************************/

#define ICSS_DIRECT			  2
#define ICSS_RPC			  3
#define ICSS_LIVE_MIKE		  5
#define ICSS_LIVE_LINE		  6
#define ICSS_RECORDED_SPEECH  7
#define ICSS_MAX_ACTIVE_CTX	  42


#ifndef TRUE
#define TRUE				  1
#define FALSE				  0
#endif

#define ICSS_CURRENT_VERSION  "V1R1M1 - ZERO"

#define ICSS_USER_MSG_STRING	  "ICSS_USER_MSG_STRING"
#define ICSS_NOTIFICATION_STRING  "ICSS_NOTIFICATION_STRING"

#define REPLY_START_CONVERSATION  3
#define REPLY_GET_SPOKEN_WORDS	  9
#define REPLY_PLAYBACK			  8

#define MAXIMUM_TEXT_LENGTH		  1024

#define SPOKE_TOO_SOON			  0x00000001
#define OUT_OF_GRAMMAR			  0x00000002
#define HAS_NOISE_PHONES		  0x00000004

#define SPOKE_TOO_SOON_SET(flags)		 ((flags & SPOKE_TOO_SOON)	 > 0)
#define OUT_OF_GRAMMAR_SET(flags)		 ((flags & OUT_OF_GRAMMAR)	 > 0)
#define HAS_NOISE_PHONES_SET(flags)		 ((flags & HAS_NOISE_PHONES) > 0)

#define BAD_RECO_TEXT(flags)			 (flags > 0)

typedef struct	_ICSS_START_DATA_
{
   char FAR *RequiredVersion;
   char FAR *ICSSFuture1;
   char FAR *ICSSFuture2;
   char FAR *ICSSFuture3;
   char FAR *ICSSFuture4;
} ICSS_START_DATA;

typedef struct	_ICSS_RETURN_WORDS_
{
   long			  return_code;
   unsigned long  flags;
   float		  utterance_length;
   float		  acquistion_time;
   float		  recognition_time;
   char			  spoken_words[MAXIMUM_TEXT_LENGTH];
   long			  result_context_index;		   /* index in ctx array for final result	 */
} ICSS_RETURN_WORDS;

#define MAX_TITLE 128

typedef struct _THRESHOLD_DLG_PARMS
{
		char szDlgTitle[MAX_TITLE];								// Dialog title text ("" for default)
		long nUttStartThreshold;								// Start talking threshold
		long nUttEndThreshold;									// End talking threshold
} THRESHOLDDLG_PARMS;


/*************************************************************************/
/*																		 */
/* This section covers defines and data structures for the ICSSWord API. */
/*																		 */
/*************************************************************************/

#define AW_BUFFER_SIZE 256

#define MEDIUM_BANDWIDTH 1
#define HIGH_BANDWIDTH	 2

#define ICSS_WORD_RETURN_MESSAGE (WM_USER+500) /* msg: server to client */

#define UNKNOWN_OPCODE	  0	  /* server operation codes */
#define INITIALIZE_OPCODE 1
#define ADDWORD_OPCODE	  2
#define COMPILE_OPCODE	  3
#define RECORD_OPCODE	  4
#define SHUTDOWN_OPCODE	  5	  /* Internal use only */

typedef struct _ICSS_WORD_RETURN_STRUCT_
{
   char			 word		[AW_BUFFER_SIZE]; /* revised version of word   */
   char			 definition [AW_BUFFER_SIZE]; /* Phoneme definition		   */
   long			 return_code;				  /* return code			   */

} ICSS_WORD_RETURN_STRUCT;

/*********************************************************************/
/*																	 */
/* This section defines the function prototypes for all functions in */
/* the IBM Continuous Speech Series API. The conditional compile	 */
/* logic is to ensure that the correct linkage conventions are used	 */
/* when operating under the appropriate operating system.			 */
/*																	 */
/*********************************************************************/

#ifdef WIN32
	#define LOADDS
#else
	#define LOADDS __loadds
#endif

#ifdef __OS2__
  #define ICSSAPILinkType _Optlink
#elif (defined(_INC_WINDOWS) || defined(WIN31))
  #define ICSSAPILinkType  FAR PASCAL LOADDS
#elif (defined(_INC_WINDOWSX) || defined(WINNT))
  #define ICSSAPILinkType  PASCAL
#else
  #define ICSSAPILinkType
#endif

long ICSSAPILinkType ICSSEnd				 (void);

long ICSSAPILinkType ICSSEndConversation	 (void);

long ICSSAPILinkType ICSSGetSpokenWords		 (HWND ReplyWindow);

long ICSSAPILinkType ICSSGetValue			 (long				  ParameterID,
											  ICSS_PARMTYPE FAR * ParameterValue);

long ICSSAPILinkType ICSSInterruptListenState(void);

long ICSSAPILinkType ICSSListen				 (long		 ContextHandle,
											  long		 SpeechInputSource,
											  char FAR * SpeechInputFileName);

long ICSSAPILinkType ICSSListenMultCtx		 (long FAR	*ContextHandle,
											  long		 NumContext,
											  long		 SpeechInputSource,
											  char FAR	*SpeechInputFileName);

long ICSSAPILinkType ICSSLoadContext		 (long FAR * ContextHandle,
											  char FAR * ContextName,
											  float		 InsertionPenalty,
											  float		 LanguageWeight,
											  float		 GrammarWeight,
											  float		 NoiseSensitivity);

long ICSSAPILinkType ICSSPlayback			 (char FAR * AudioOutputFileName,
											  HWND		 ReplyWindow);

long ICSSAPILinkType ICSSRemoveContext		 (long		 ContextHandle);

long ICSSAPILinkType ICSSSetValue			 (long			ParameterID,
											  ICSS_PARMTYPE ParameterValue);

long ICSSAPILinkType ICSSStart				 (ICSS_START_DATA FAR *ICSSStartData);

long ICSSAPILinkType ICSSStartConversation	 (char FAR * ADCName,
											  long		 AudioCardNumber,
											  char FAR * RPSystemName,
											  long		 RPLinkType,
											  HWND		 ReplyWindow);

char FAR * ICSSAPILinkType ICSSErrorText	 (long		 ErrorNumber);

int ICSSAPILinkType ICSSThresholdRecordInit
										(HANDLE FAR *phMem,
										 int		 VerboseLevel,
										 int		 AudioCardNumber);

int ICSSAPILinkType ICSSThresholdRecordTerm(HANDLE hMem);

int ICSSAPILinkType ICSSThresholdRecordSpeech
										(HANDLE		hMem,
										 char  FAR *SpeechInputFileName,
										 int		RecordTime);

int ICSSAPILinkType ICSSThresholdDisplayWave
										(HWND		hWnd,
										 HDC		hDC,
										 POINT FAR *PointArray,
										 int		PixelCount,
										 RECT		rect,
										 short int	MaxSample);

int ICSSAPILinkType ICSSThresholdWindowAnnotation
									(HDC		hDC,
									 RECT		rect,
									 short int	MaxSample,
									 short int	Threshold,
									 int		PixelCount,
									 int		RecordTime,
									 char  FAR *ExtraText);

void ICSSAPILinkType ICSSThresholdDrawFrame (RECT rect, HDC	 hDC);

int ICSSAPILinkType ICSSThresholdCalculate
										 (POINT		far *PointArray,
										  int			 PixelCount,
										  long			 RecordTime,
										  char		far *SpeechInputFileName,
										  short int far *pMaxSample,
										  short int far *pThreshold);

#if defined(_WIN32)
		#define _export
#endif

LRESULT _export CALLBACK ICSSThresholdDialog(HWND hwndActive,
											 HINSTANCE hAppInstance,
											 THRESHOLDDLG_PARMS FAR *pInitData);

long ICSSAPILinkType ICSSWordInit (long StartTalkingThreshold,
								   long StopTalkingThreshold,
								   long AudioCardNumber,
								   HWND ReplyWindow);

long ICSSAPILinkType ICSSWordTerm (void);

long ICSSAPILinkType ICSSWordAdd  (LPSTR Word,
								   LPSTR Dictionary,
								   LPSTR SpeechInputFileName,
								   HWND	 ReplyWindow);

long ICSSAPILinkType ICSSWordCompile (LPSTR GrammarFile,
									  LPSTR ContextFile,
									  LPSTR AddendumDictionary,
									  LPSTR WorkDirectory,
									  long	FiniteState,
									  long	VerboseLevel,
									  long	DetectNoisePhones,
									  long	DetectOutOfGrammar,
									  HWND	ReplyWindow);

long ICSSAPILinkType ICSSWordRecord (LPSTR SpeechInputFileName,
										   float EndSilenceDuration,
										   float StartSilenceDuration,
										   HWND	 ReplyWindow);

/*===========================================================================*/
/* Bottom of File															 */
/*===========================================================================*/

}	// end of extern "C"
