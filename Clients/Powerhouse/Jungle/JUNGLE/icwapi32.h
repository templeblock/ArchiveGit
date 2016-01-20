// Library was built for "C"
extern "C"
{

/***************************************************************************/
/*                   IBM Continuous Speech Series  1.1                     */
/***************************************************************************/
/*                                                                         */
/*    Licensed Materials - Property of IBM                                 */
/*    IBM Continuous Speech Series.                                        */
/*    (C) Copyright IBM Corporation 1992, 1994. All rights reserved.       */
/*                                                                         */
/*    US Government Users Restricted Rights -                              */
/*    Use, duplication or disclosure restricted by GSA ADP Schedule        */
/*    Contract with IBM Corporation.                                       */
/*                                                                         */
/*=========================================================================*/
/*                                                                         */
/* Program:           IBM Continuous Speech System (ICSS)                  */
/*                                                                         */
/* Component:         Common                                               */
/*                                                                         */
/* Source File Name:  icsswapi.h                                           */
/*                                                                         */
/* Description:                                                            */
/* This header file is for use by application programs interfacing         */
/* with the Application Programming Interface of the IBM Continuous        */
/* Speech Series. It includes a series of data definitions, data           */
/* types, and function prototypes. It must be "included" in those          */
/* programs using the API.                                                 */
/*                                                                         */
/* Functions:                                                              */
/*                                                                         */
/* Comments:                                                               */
/*                                                                         */
/* Modification History:                                                   */
/* Level     Date      Programmer  Reason                                  */
/* --------  --------  ----------  --------------------------------------- */
/* 000.000   03/11/93  bjtb        Initial version containing header.      */
/* 000.000   04/15/93  SLC         Add new return code for ISTG Start and  */
/*                                 End conversation errors.                */
/* 000.00    05/10/93  SLC         Reorganize return codes and provide     */
/*                                 return code cross-ref. to API calls.    */
/* 000.000   05/18/93  SLC         Fix PTR 484 and 481.                    */
/* 000.000   06/04/93  SLC         Fix PTR616.                             */
/*                                                                         */
/* 000.000   06/30/93  BJTB        modified for windows use                */
/* 000.000   09/23/93  SLC         Updated Windows routines linkage types  */
/* 000.000   11/07/93  SLC         Added Windows ICSS_ERR_WIN_NO_CLIENT    */
/* 000.000   04/19/94  SLC         Added support for OEM runtime.          */
/* 000.000   04/19/94  bjtb        altered loadcontext to be sync.,        */
/*                                 playback and GSW to remove parms. added */
/*                                 flags to GSW return structure.          */
/*           4/94-5/94 bjtb        notification support.                   */
/*           05/24/94  bjtb        Added Noise Sensitivity.                */
/*           06/14/94  SLC         Added AllowableNoiseCount, ECR????      */
/*           06/14/94  SLC         Added error code for no WIN32s support  */
/*                                 for PTR1248.                            */
/* 000.000   06/16/94  SLC         Added error code 76 for ECR1264, and    */
/*                                 changed error code 65 for PTR1249.      */
/*           06/23/94  bjtb        Took out error code 76 for ECR1346      */
/*                                 (was called ICSS_ERR_ALL_NOISE_DETECTED)*/
/* 000.000   07/01/94  OW          Revised error strings for better        */
/*                                 readability. PTR1224                    */
/*           01/10/95  bjtb        Move pieces to vmapi.h                  */
/*                                                                         */
/*=========================================================================*/

//#include <vmapi.h>

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

  ICSS_FIRST_HANDLE = 300,

  ICSS_CHDL_TALK_START = 310,
  ICSS_CHDL_TALK_END,
  ICSS_CHDL_SPOKE_TOO_SOON,
  ICSS_CHDL_UTT_TOO_SHORT,
  ICSS_CHDL_UTT_TOO_LONG,
  ICSS_CHDL_AUDIO_GAIN,

  ICSS_LAST_ID
} ICSS_PARAMETERIDS;


/*********************************************************************/
/*                                                                   */
/* This section defines a set of symbolic values to use when         */
/* invoking the IBM Continuous Speech Series API. These are used     */
/* by the application to indicate certain conditions or options.     */
/* Please refer to Application Programmer's Guide for more detailed  */
/* information on the meaning of each symbolic value.                */
/*                                                                   */
/*********************************************************************/

#define ICSS_DIRECT           2
#define ICSS_RPC              3
#define ICSS_LIVE_MIKE        5
#define ICSS_LIVE_LINE        6
#define ICSS_RECORDED_SPEECH  7
#define ICSS_MAX_ACTIVE_CTX   42


#ifndef TRUE
#define TRUE                  1
#define FALSE                 0
#endif

#define ICSS_CURRENT_VERSION  "V1R1M1 - ZERO"

/*********************************************************************/
/*                                                                   */
/* This section is Windows specific support.                         */
/*                                                                   */
/*********************************************************************/

#define ICSS_USER_MSG_STRING      "ICSS_USER_MSG_STRING"
#define ICSS_NOTIFICATION_STRING  "ICSS_NOTIFICATION_STRING"

#define REPLY_START_CONVERSATION  3
#define REPLY_GET_SPOKEN_WORDS    9
#define REPLY_PLAYBACK            8

typedef struct  _ICSS_START_DATA_
{
   char FAR *RequiredVersion;
   char FAR *ICSSFuture1;
   char FAR *ICSSFuture2;
   char FAR *ICSSFuture3;
   char FAR *ICSSFuture4;
} ICSS_START_DATA;


/*********************************************************************/
/*                                                                   */
/* This section defines the function prototypes for all functions in */
/* the IBM Continuous Speech Series API. The conditional compile     */
/* logic is to ensure that the correct linkage conventions are used  */
/* when operating under the appropriate operating system.            */
/*                                                                   */
/*********************************************************************/

#ifdef __OS2__
  #define ICSSAPILinkType _Optlink
#else
  #define ICSSAPILinkType
#endif


long ICSSAPILinkType ICSSEnd                 (void);

long ICSSAPILinkType ICSSEndConversation     (void);

long ICSSAPILinkType ICSSGetSpokenWords      (HWND ReplyWindow);

long ICSSAPILinkType ICSSGetValue            (long                ParameterID,
                                              ICSS_PARMTYPE FAR * ParameterValue);

long ICSSAPILinkType ICSSInterruptListenState(void);

long ICSSAPILinkType ICSSListen              (long       ContextHandle,
                                              long       SpeechInputSource,
                                              char FAR * SpeechInputFileName);

long ICSSAPILinkType ICSSListenMultCtx       (long FAR  *ContextHandle,
                                              long       NumContext,
                                              long       SpeechInputSource,
                                              char FAR  *SpeechInputFileName);

long ICSSAPILinkType ICSSLoadContext         (long FAR * ContextHandle,
                                              char FAR * ContextName,
                                              float      InsertionPenalty,
                                              float      LanguageWeight,
                                              float      GrammarWeight,
                                              float      NoiseSensitivity);

long ICSSAPILinkType ICSSPlayback            (char FAR * AudioOutputFileName,
                                              HWND       ReplyWindow);

long ICSSAPILinkType ICSSRemoveContext       (long       ContextHandle);

long ICSSAPILinkType ICSSSetValue            (long          ParameterID,
                                              ICSS_PARMTYPE ParameterValue);

long ICSSAPILinkType ICSSStart               (ICSS_START_DATA FAR *ICSSStartData);

long ICSSAPILinkType ICSSStartConversation   (char FAR * ADCName,
                                              long       ADCIFNumber,
                                              char FAR * RPSystemName,
                                              long       RPLinkType,
                                              HWND       ReplyWindow);

char FAR * ICSSAPILinkType ICSSErrorText     (long       ErrorNumber);

long ICSSAPILinkType ICSSDecodeReturnedWords (int                shmem_id,
                                              ICSS_RETURN_WORDS *return_words);

long ICSSAPILinkType ICSSRecognizeAgain      (long *ContextHandle,
                                              long  NumContext,
                                              long  UtteranceID,
                                              ICSS_RETURN_WORDS * WordsPtr);
}	// end of extern "C"
