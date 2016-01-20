/*=========================================================================*/
/*                                                                         */
/*                         IBM Confidential                                */
/*        (IBM Confidential-Restricted when combined with                  */
/*        the aggregated source modules for this program.)                 */
/*                                                                         */
/*                        OCO Source Materials                             */
/*                                                                         */
/*   IBM Continuous Speech Series                                          */
/*   (C) Copyright IBM Corporation 1992, 1993                              */
/*                                                                         */
/*=========================================================================*/
/*                                                                         */
/* Program:           IBM Continuous Speech System (ICSS)                  */
/*                                                                         */
/* Component:         Common                                               */
/*                                                                         */
/* Source File Name:  vmapi.h                                              */
/*                                                                         */
/* Description:                                                            */
/*    VoiceManager external api header.                                    */
/*                                                                         */
/* Functions:                                                              */
/*                                                                         */
/* Comments:                                                               */
/*                                                                         */
/* Modification History:                                                   */
/* Level     Date      Programmer  Reason                                  */
/* --------  --------  ----------  --------------------------------------- */
/* 000.000   01/09/95  bjtb        Initial version.                        */
/* ecr2438   04/12/95  bjtb        add wcis api calls.                     */
/* ptf2565   05/15/95  bjtb        new return code                         */
/* ptf2569   06/07/95  rdv         delete of active ctx.                   */
/*=========================================================================*/

#ifndef _VMAPI_INCLUDE_
#define _VMAPI_INCLUDE_

/*********************************************************************/
/*                                                                   */
/* This section defines all valid return codes from the IBM          */
/* Continuous Speech Series API.                                     */
/*                                                                   */
/* In the following section of return codes, the comment section to  */
/* the right of each return code contains two pieces of important    */
/* information. To the left of the ":" is a number indicating the    */
/* decimal value of this symbolic return code. To the right of the   */
/* colon is a series of numbers that indicate which API calls        */
/* issue this return code. These numbers on the right correspond to  */
/* the numbers to the left of each API call in the following list:   */
/*                                                                   */
/*   1)  ICSSStart                                                   */
/*   2)  ICSSStartConversation                                       */
/*   3)  ICSSGetValue                                                */
/*   4)  ICSSSetValue                                                */
/*   5)  ICSSLoadContext                                             */
/*   6)  ICSSRemoveContext                                           */
/*   7)  ICSSListen                                                  */
/*   8)  ICSSInterruptListenState                                    */
/*   9)  ICSSGetSpokenWords                                          */
/*  10)  ICSSPlayback                                                */
/*  11)  ICSSEndConversation                                         */
/*  12)  ICSSEnd                                                     */
/*                                                                   */
/*                                                                   */
/*********************************************************************/

typedef enum {
  ICSS_SUCCESS,                                  /*  0 : 1 2 3 4 5 6 7 8 9 10 11 12      */
  ICSS_ERR_ICSS_NOT_ACTIVE,                      /*  1 :   2 3 4 5 6 7 8 9 10 11 12      */
  ICSS_ERR_ICSS_ALREADY_INIT,                    /*  2 : 1                               */
  ICSS_ERR_INITIALIZED,                          /*  3 :     3 4 5 6 7   9 10 11         */
  ICSS_ERR_CONV_ACTIVE,                          /*  4 : 1               9       12      */
  ICSS_ERR_CONV_ALREADY_STARTED,                 /*  5 :   2                             */
  ICSS_ERR_LISTENING,                            /*  6 : 1 2 3 4 5 6 7     10 11 12      */
  ICSS_ERR_INIT_MSG,                             /*  7 : 1                               */
  ICSS_ERR_INIT_GLOBAL,                          /*  8 : 1                               */
  ICSS_ERR_INIT_SEMAPHORE,                       /*  9 : 1                               */

  ICSS_ERR_INIT_GLOBAL_DATA,                     /* 10 : 1                               */
  ICSS_ERR_INIT_ISTG_FAILED,                     /* 11 : 1                               */
  ICSS_ERR_INIT_POOL_SERVICES,                   /* 12 : 1                               */
  ICSS_ERR_TERM_ISTG_FAILED,                     /* 13 :                         12      */
  ICSS_ERR_LOAD_ADC_CODE,                        /* 14 : 1 2 (1 for AIX, 2 for OS/2)     */
  ICSS_ERR_INVALID_ADCNUMBER,                    /* 15 :   2                             */
  ICSS_ERR_INVALID_RPLINKTYPE,                   /* 16 :   2                             */
  ICSS_ERR_CONTEXT_NOT_LOADED,                   /* 17 :         5                       */
  ICSS_ERR_CODEBOOKS_NOT_READ,                   /* 18 :         5                       */
  ICSS_ERR_MAX_CTX_REACHED,                      /* 19 :         5                       */

  ICSS_ERR_INVALID_PARAMETER_ID,                 /* 20 :     3 4                         */
  ICSS_ERR_INVALID_PARM_VALUE,                   /* 21 :       4                         */
  ICSS_ERR_CONTEXT_NOT_AVAILABLE,                /* 22 :           6 7                   */
  ICSS_ERR_INVALID_INSERTION_PENALTY,            /* 23 :         5                       */
  ICSS_ERR_INVALID_LANGUAGE_WEIGHT,              /* 24 :         5                       */
  ICSS_ERR_INVALID_GRAMMAR_WEIGHT,               /* 25 :         5                       */
  ICSS_ERR_INVALID_SPEECH_INPUT_SOURCE,          /* 26 :             7                   */
  ICSS_ERR_INVALID_SPEECH_INPUT_FILENAME,        /* 27 :             7                   */
  ICSS_ERR_SPEECH_INPUT_SOURCE,                  /* 28 :             7                   */
  ICSS_ERR_INVALID_SAMPLE_RATE,                  /* 29 :                   10            */

  ICSS_ERR_PLAYBACK_FAILED,                      /* 30 :                   10            */
  ICSS_ERR_EMPTY_RESPONSE,                       /* 31 :                 9               */
  ICSS_ERR_TRUNCATED_RESPONSE,                   /* 32 :                 9               */
  ICSS_ERR_EXCEEDED_MAX_SIL,                     /* 33 :                 9               */
  ICSS_ERR_EXCEEDED_MAX_TALK,                    /* 34 :                 9               */
  ICSS_ERR_LISTEN_STATE_INTERRUPTED,             /* 35 :                 9               */
  ICSS_ERR_UNLOAD_ADC_CODE,                      /* 36 :(11 for OS2, 12 - AIX)11 12      */
  ICSS_ERR_ISTG_START_CONVERSATION,              /* 37 :   2                             */
  ICSS_ERR_ISTG_END_CONVERSATION,                /* 38 :                      11         */
  ICSS_ERR_ISTG_FAILED,                          /* 39 :             7   9               */

  ICSS_ERR_FBS_SEMAPHORE,                        /* 40 :   2     5 6     9    11         */
  ICSS_ERR_FBS_SHARED_MEMORY,                    /* 41 :   2     5 6     9    11         */
  ICSS_ERR_FBS_MEMORY,                           /* 42 :   2     5       9               */
  ICSS_ERR_FBS_UNKNOWN_CONTEXT,                  /* 43 :         5 6                     */
  ICSS_ERR_FBS_NOT_SETUP,                        /* 44 :         5 6     9    11         */
  ICSS_ERR_FBS_NULL_REQUEST,                     /* 45 :         5       9               */
  ICSS_ERR_FBS_MAX_CONTEXTS,                     /* 46 :         5                       */
  ICSS_ERR_FBS_OPEN_CONTEXT,                     /* 47 :         5                       */
  ICSS_ERR_FBS_READ_CONTEXT,                     /* 48 :         5                       */
  ICSS_ERR_FBS_OPEN_CPDFILE,                     /* 49 :         5                       */

  ICSS_ERR_FBS_INCOMPATIBLE_CPDFILE,             /* 50 :         5                       */
  ICSS_ERR_FBS_INVALID_CONTEXT,                  /* 51 :         5                       */
  ICSS_ERR_FBS_TOO_MANY_CPDFILES,                /* 52 :         5                       */
  ICSS_ERR_FBS_READ_CPDFILE,                     /* 53 :         5                       */
  ICSS_ERR_FBS_BUFFER_OVERFLOW,                  /* 54 :                 9               */
  ICSS_ERR_FBS_RPC_FAILURE,                      /* 55 :   2     5 6     9    11         */
  ICSS_ERR_FBS_RPC_INVALID_HOST,                 /* 56 :   2                             */
  ICSS_ERR_FBS_RPC_SERVER_UNAVAIL,               /* 57 :   2                             */
  ICSS_ERR_FBS,                                  /* 58 :   2     5 6     9    11         */
  ICSS_ERR_INVALID_ICSSDIR,                      /* 59 : 1                               */

  ICSS_ERR_INVALID_RPSYSTEMNAME,                 /* 60 :   2                             */
  ICSS_ERR_SHOULD_NOT_OCCUR,                     /* 61 :                                 */
  ICSS_ERR_WIN_SERVER_START,                     /* 62 : 1 2 3 4 5 6 7 8 9 10 11 12      */
  ICSS_ERR_WIN_API_MEMORY,                       /* 63 : 1 2 3 4 5 6 7 8 9 10 11 12      */
  ICSS_ERR_ASYNC_IN_PROGRESS,                    /* 64 : 1 2 3 4 5 6 7   9 10 11 12      */
  ICSS_ERR_WIN_NO_SEARCH,                        /* 65 :   2     5 6     9    11         */
  ICSS_ERR_ADC_OPEN,                             /* 66 :             7                   */
  ICSS_ERR_ADC_CLOSE,                            /* 67 :                 9               */
  ICSS_ERR_ADC_BUSY,                             /* 68 :             7                   */
  ICSS_ERR_ADC_READ,                             /* 69 :                 9               */

  ICSS_ERR_ADC_FILE_IO,                          /* 70 :                 9               */
  ICSS_ERR_UNSUPPORTED_VERSION,                  /* 71 : 1                               */
  ICSS_ERR_UNLICENSED_VERSION,                   /* 72 :   2                             */
  ICSS_ERR_RECO_CHECK_FLAGS,                     /* 73 :                 9               */
  ICSS_ERR_INVALID_NOISE_SENSITIVITY,            /* 74 :         5                       */
  ICSS_ERR_WIN32S_NOT_AVAILABLE,                 /* 75 : 1                               */
  ICSS_ERR_INVALID_CONVERSATION_ID,              /* 76 */
  ICSS_ERR_INVALID_UTTERANCE_COUNT,              /* 77 */
  ICSS_ERR_INVALID_RECORD_MODE,                  /* 78 */
  ICSS_ERR_ADC_END_OF_FILE,                      /* 79 ptf2565*/
  ICSS_ERR_FBS_DELETE_ACTIVE_CONTEXT,            /* 80 :           6                     */

  ICSS_MAXIMUM_ERROR_NUMBER
} ICSS_RETURNCODES;

/*********************************************************************/
/*                                                                   */
/* This section will, be default, define an array of character       */
/* pointers to strings that identify each of the return codes        */
/* defined in the previous section. This is very useful for programs */
/* using the API to print the associated errors, rather than just a  */
/* number that has to be looked up. Space may be conserved by        */
/* turning off the definition of these items by defining a macro     */
/* by the name of _NO_ICSS_ERROR_STRINGS_.                           */
/*                                                                   */
/*********************************************************************/

#if defined(INCLUDE_ICSS_ERROR_STRINGS)

char      *ICSSErrorString[] = {
/*  0 */    "Speech function successfully completed.",
/*  1 */    "The speech recognition engine has not been started.",
/*  2 */    "The speech recognition engine is already running.",
/*  3 */    "The speech recognition engine is already running, but no conversation is active.",
/*  4 */    "A conversation is still active between the program and the speech engine.",
/*  5 */    "A conversation is already active between the program and the speech engine.",
/*  6 */    "The system is still listening for speech - speech function disregarded.",
/*  7 */    "The speech engine cannot start message services and cannot continue.",
/*  8 */    "The speech engine cannot acquire its shared memory data.",
/*  9 */    "The speech engine cannot start its services for synchronizing processes.",
/* 10 */    "The speech engine cannot initialize its global data and cannot start.",

/* 11 */    "The speech engine cannot start its multitasking services.",
/* 12 */    "The speech engine cannot allocate its data buffers.",
/* 13 */    "The speech engine cannot stop its multitasking services while ending the program.",
/* 14 */    "The speech engine could not load audio support - ensure audio drivers are loaded.",
/* 15 */    "An invalid sound card number has been specified - check the User Configuration Profile.",
/* 16 */    "The RPC connection type, specified by ICSSStartConversaton, is not 'direct' or 'RPC'.",
/* 17 */    "The requested context has not been loaded into memory - its phrases will not be active.",
/* 18 */    "The files supporting the context sample rate cannot be found or are corrupted.",
/* 19 */    "Not enough memory to load another context - maximum number reached. Remove unnecessary contexts.",

/* 20 */    "A system value name in ICSSGetValue or ICSSSetValue is invalid.",
/* 21 */    "The value specified in ICSSSetValue is invalid.",
/* 22 */    "The specified speech context is not loaded in memory.",
/* 23 */    "The Insertion Penalty specified in ICSSLoadContext is invalid.",
/* 24 */    "The Language Weight specified in ICSSLoadContext is invalid.",
/* 25 */    "The Grammar Weight specified in ICSSLoadContext is invalid.",
/* 26 */    "The Speech Input Source specified in ICSSListen is invalid.",
/* 27 */    "The Speech Input Filename specified in ICSSListen is invalid.",
/* 28 */    "The Speech Input device specified in ICSSListen cannot be opened.",
/* 29 */    "The specified Sample Rate is invalid.",

/* 30 */    "Unable to playback the specified sound file.",
/* 31 */    "The speech engine heard no words - empty response.",
/* 32 */    "The phrase which the speech engine recognized was too long - the response has been truncated.",
/* 33 */    "The maximum allowable silence time was exceeded - empty response.",
/* 34 */    "The maximum allowable talk time was exceeded - empty response.",
/* 35 */    "The speech engine has been interrupted from listening by ICSSInterruptListenState.",
/* 36 */    "Cannot unload the audio support module from memory.",
/* 37 */    "ICSSStartConversation failed to establish a conversation.",
/* 38 */    "ICSSStartConversation failed to end a conversation.",
/* 39 */    "The speech engine's multitasking services has failed and is attempting to restart.",

/* 40 */    "A synchronization error occurred while attempting to recognize speech.",
/* 41 */    "A shared memory error occurred while attempting to recognize speech.",
/* 42 */    "A memory management error occurred while attempting to recognize speech.",
/* 43 */    "The context represented by the context handle could not be found.",
/* 44 */    "Recognition cannot occur until the speech engine is initialized.",
/* 45 */    "The speech engine cannot understand the ICSSLoadContext request.",
/* 46 */    "ICSSLoadContext cannot load another context - maximum number reached.",
/* 47 */    "ICSSLoadContext could not open the requested context.",
/* 48 */    "ICSSLoadContext could not read the requested context.",
/* 49 */    "ICSSLoadContext could not open the speech models - could be corrupted or missing.",

/* 50 */    "ICSSLoadContext cannot load a context incompatible with the speech models.",
/* 51 */    "ICSSLoadContext cannot load an invalid context.",
/* 52 */    "ICSSLoadContext failed upon detecting too many speech model files.",
/* 53 */    "ICSSLoadContext could not read the speech models - could be corrupted or missing.",
/* 54 */    "ICSSGetSpokenWords failed upon detecting a speech buffer overflow.",
/* 55 */    "The remote speech server session has been severed.",
/* 56 */    "The remote speech server system cannot not be found.",
/* 57 */    "The remote speech server is unavailable on the specified system.",
/* 58 */    "An error occurred during speech recognition processing.",
/* 59 */    "The speech directory location specified by ICSSDIR is invalid.",

/* 60 */    "The remote system name specified in ICSSStartConversation is invalid.",
/* 61 */    "The speech engine encountered an error that should not occur.",
/* 62 */    "The speech recognition server cannot be started/found.",
/* 63 */    "A speech engine API memory error occurred.",
/* 64 */    "A speech engine asynchronous function is currently executing.",
/* 65 */    "The speech search engine cannot be started/found.",
/* 66 */    "ICSSListen failed - cannot open the audio device.",
/* 67 */    "ICSSGetSpokenWords failed - cannot close the audio device.",
/* 68 */    "ICSSListen failed - the audio device is busy.",
/* 69 */    "ICSSGetSpokenWords failed - cannot read data from the audio device.",

/* 70 */    "ICSSGetSpokenWords failed - cannot perform audio file I/O.",
/* 71 */    "Cannot start the speech engine. Application requires a different version of the speech engine.",
/* 72 */    "Application requires a licensed OEM or unrestricted runtime version of the speech engine.",
/* 73 */    "Recognized speech may be invalid - check flags set by ICSSGetSpokenWords.",
/* 74 */    "The Noise Sensitivity specified in ICSSLoadContext is invalid.",
/* 75 */    "Required Win32S support, V1.1.88.0 or later, is unavailable.",

/* 76 */    "The Conversation ID provided is not known to ICSS.",
/* 77 */    "The requested number of utterances is not allowed.",
/* 78 */    "The recording mode value is incorrect.",
/* 79 */    "End of File was reached when reading audio data from a file.",
/* 80 */    "Delete failed.  Context was still active",
            "Maximum Error Number"
          };
#endif

/*********************************************************************/
/*                                                                   */
/* This section defines all valid parameter ids used by the IBM      */
/* Continuous Speech Series API functions ICSSGetValue and           */
/* ICSSSetValue. These parameter types are used to identify the      */
/* parameter to be retrieved/affected by the function invocation.    */
/* Please refer to Application Programmer's Guide for more detailed  */
/* information on the meaning of each parameter id.                  */
/*                                                                   */
/*********************************************************************/

typedef enum {
  VMAPI_FIRST_INT = 100,

  VMAPI_FIRST_FLOAT = 200,

  VMAPI_FIRST_STRING = 300,

  VMAPI_CSTR_RECORD_SESSION_NAME,
  VMAPI_CSTR_RECORD_SPEECH_NAME,

  VMAPI_FIRST_HANDLE = 400,

  VMAPI_CHDL_TALK_START,
  VMAPI_CHDL_TALK_END,
  VMAPI_CHDL_SPOKE_TOO_SOON,
  VMAPI_CHDL_UTT_TOO_SHORT,
  VMAPI_CHDL_UTT_TOO_LONG,
  VMAPI_CHDL_AUDIO_GAIN,

  VMAPI_LAST_ID

} VMAPI_PARAMETERIDS;

/*********************************************************************/
/*                                                                   */
/* This section defines a union to be used when receiving data from/ */
/* providing data to the IBM Continuous Speech Series API functions  */
/* ICSSGetValue or ICSSSetValue.                                     */
/*                                                                   */
/*********************************************************************/

typedef union {
  unsigned long ICSS_HANDLEParameterData;
  long          ICSS_INTParameterData;
  float         ICSS_FLOATParameterData;
  char          ICSS_STRINGParameterData[256];
} ICSS_PARMTYPE;


/*********************************************************************/
/*                                                                   */
/* This section defines all default value indicators from the IBM    */
/* Continuous Speech Series API function ICSSSetValue. These are used*/
/* by the application to inform ICSS to reset the specified value    */
/* back to its current default.                                      */
/*                                                                   */
/*********************************************************************/

#define ICSS_DEFAULT_HANDLE  0
#define ICSS_DEFAULT_INT     (-1)
#define ICSS_DEFAULT_FLOAT   ((float) -2.0)
#define ICSS_DEFAULT_STRING  ""

/*============================================================*/
/*  Defines                                                   */
/*============================================================*/

#define VMAPILinkType

#define VM_USER_MSG_STRING               "VM_USER_MSG_STRING"
#define VM_NOTIFICATION_STRING           "VM_NOTIFICATION_STRING"

#define MAXIMUM_TEXT_LENGTH              1024

#define SPOKE_TOO_SOON                   0x00000001
#define OUT_OF_GRAMMAR                   0x00000002
#define HAS_NOISE_PHONES                 0x00000004

#define SPOKE_TOO_SOON_SET(flags)        ((flags & SPOKE_TOO_SOON)   > 0)
#define OUT_OF_GRAMMAR_SET(flags)        ((flags & OUT_OF_GRAMMAR)   > 0)
#define HAS_NOISE_PHONES_SET(flags)      ((flags & HAS_NOISE_PHONES) > 0)

#define BAD_RECO_TEXT(flags)             (flags > 0)

#define VMAPI_REPLY_RECO_TEXT            1 /* sent to client with recoed text */
#define VMAPI_PLAYBACK_DONE              2 /* sent to client when playback done */
#define VMAPI_LOSING_SPEECH_FOCUS        3 /* sent to client when it loses speech focus */
#define VMAPI_VOICEMANAGER_CLOSING       4 /* send to client when voice manager closes */

typedef enum
{
   FIRST_RECORD_MODE,
   TIME_RECORD,
   THRESHOLD_RECORD,
   START_RECORD,
   STOP_RECORD,
   LAST_RECORD_MODE
} RECORD_MODES;

/*============================================================*/
/*  Structures                                                */
/*============================================================*/

typedef struct  _ICSS_RETURN_WORDS_
{
   long           return_code;
   unsigned long  flags;
   float          utterance_length;
   float          acquistion_time;
   float          recognition_time;
   char           spoken_words[MAXIMUM_TEXT_LENGTH];
   long           result_context_index;   /* idx in ctx array */
   long           utterance_id;
   unsigned long  reserved1;
   unsigned long  reserved2;
} ICSS_RETURN_WORDS;

typedef struct _RECORD_PARMS_
{
  /* Time Record */
  int record_seconds;

  /* Threshold Record */
  int start_threshold;
  int end_threshold;
  float end_silence_duration;
  float maximum_silence_duration;
  float maximum_utterance_duration;
} RECORD_PARMS;

/*============================================================*/
/*  Prototypes                                                */
/*============================================================*/

int    VMAPILinkType VMStartConversation   (int   *ConversationID,
                                            HWND   ReplyWindow,
                                            char  *AgentName);

int    VMAPILinkType VMEndConversation     (int    ConversationID);

int    VMAPILinkType VMLoadContext         (int    ConversationID,
                                            long  *ContextHandle,
                                            char  *ContextName,
                                            float  InsertionPenalty,
                                            float  LanguageWeight,
                                            float  GrammarWeight,
                                            float  NoiseSensitivity);

int    VMAPILinkType VMRemoveContext       (int    ConversationID,
                                            int    ContextHandle);

int    VMAPILinkType VMSetActiveContext    (int    ConversationID,
                                            int   *ContextHandles,
                                            int    ContextCount );

int    VMAPILinkType VMDecodeReturnedWords (int    shmem_id,
                                            ICSS_RETURN_WORDS *user_words_ptr);

char * VMAPILinkType VMErrorText           (long   ErrorNumber);


int VMAPILinkType  VMGetValue              (int    ConversationID,
                                            int    ParameterID,
                                            ICSS_PARMTYPE *ParameterValue);

int VMAPILinkType  VMSetValue              (int    ConversationID,
                                            int    ParameterID,
                                            ICSS_PARMTYPE *ParameterValue);

int VMAPILinkType VMRecord                 (int    ConversationID,
                                            char  *AudioName,
                                            int    RecordMode,
                                            RECORD_PARMS *RecordParms);

int VMAPILinkType VMPlayback               (int    ConversationID,
                                            char  *AudioName);

int VMAPILinkType VMRequestSpeechFocus     (int    ConversationID);

int VMAPILinkType VMReleaseSpeechFocus     (int    ConversationID);

int VMAPILinkType VMRequestAudio           (int    ConversationID);

int VMAPILinkType VMReleaseAudio           (int    ConversationID);

int VMAPILinkType VMRecognizeAgain (int ConversationID,
                                    int UtteranceID,
                                    int *ContextHandles,
                                    int ContextCount);

int VMAPILinkType VMSetActiveWindow (int ConversationID, HWND NewWindow);

int VMAPILinkType VMSetMapFile (int ConversationID, char *Filename);

int VMAPILinkType VMExpandBranch (int ConversationID, char *OperationText);

#endif  /* _VMAPI_INCLUDE_ */

/*============================================================*/
/*  End of File                                               */
/*============================================================*/
