/*******************************************************************************
*
*  (C) Copyright 1994 Rainbow Technologies, Inc. All rights subject to the
*	Developer's agreement.
*
*
*	Description: This file contains definitions of prototypes in 
* 			    eveitf.o
*************************************************************/
extern "C" short int EVEReset(void);
extern "C" short int EVEStatus(void);
extern "C" short int EVEEnable (char *);
extern "C" short int EVEReadCTR(void);
extern "C" short int EVEChallenge ( short int ,  short int);
extern "C" short int EVEReadGPR( short int);
extern "C" short int EVEWriteGPR( short int,  short int);
extern "C" short int EVESetLock ( short int ,  short int,  short int );
