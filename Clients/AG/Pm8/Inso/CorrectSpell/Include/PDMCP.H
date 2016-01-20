/***************************************************************************
*                   Copyright 1995 INSO Corporation                        *
*                Personal Dictionary Component Software                    *
*                                                                          *
* This work is protected by United States and international copyright laws *
* and contains certain trade secret information.  All use of this work is  *
* governed by the terms of agreements between INSO Corporation and its     *
* licensees or potential licensees.  Disclosure or disassembly of the      *
* contents of embodied programs or algorithms is prohibited.  All rights   *
* are reserved worldwide.                                                  *
****************************************************************************
 
*-----------------------------------------------------------------------*
*                                                                       *
*        PDmscp.h - Master Control Pointers                             *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
        Description:

		  This header contains the static structure definition for
		  the PDmcp structure


        Cautions:
        None.

        Creation Date:
        01-Jul-93   DLB     Ver. 1.0.0

*-----------------------------------------------------------------------*/

typedef struct PDmcpbuff
{
	PFPDIBUFF FAR *PDibuff;    /* an array of Open Dictionary instances  */

	UINT2B FAR *numDicts;  	/*pointer to number of dictionaries instances */
	INT2B FAR *dictStat;      /* Status of current Dictioanry */

	UINT2B FAR *numProcs; /* Pointer to processes currently running */
	INT2B FAR *procStat; /* Status of Process Pointer  */

	PFPDWRDBUFF FAR *pdWrdbuffs; /* Pointer array of process instances */	

} PDmcpbuff,
FAR *PFPDMCPBUFF;



/* End of File - PDmcp.h
*/
