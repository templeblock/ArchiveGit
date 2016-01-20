/* 		process_states.h

		Processing states
		
		(c) Copyright 1997, Monaco Systems Inc.
		by James Vogh
		
*/

#ifndef IN_PROCESS_STATES
#define IN_PROCESS_STATES

typedef enum {
	Process_Start = 0,
	Process_GammutSurface,
	Process_GammutCompress,
	Process_BuildingTable,
	Process_End
	} ProcessingStates;
	
#endif