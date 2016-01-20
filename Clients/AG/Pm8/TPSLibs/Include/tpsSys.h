/*
	File: tpsSys.h
	
	interface to system-level utility code.
	
	Client:	Turning Point Software, Inc.
	Copyright ©1995 Turning Point Software, Inc. All rights reserved.
	
	History:
	
		2/14/95	Rich Siegel		Creation.
		--	additional revision comments go here --
*/

#ifndef __TPS_SYS__
#define __TPS_SYS__

/*	macro definitions follow	*/

/*	Data types follow	*/

/*	public globals follow	*/

/*	Prototypes for public functions */

#ifdef MAC
Boolean tsysHasMacTrap(short);
Boolean tsysMacMonitorIsActive(GDHandle);
Boolean tsysMacMonitorHasDepth(GDHandle, short);
GDHandle tsysGetMacMonitorOfDepth(short);
Boolean tsysMacHasGestalt(void);
Boolean tsysMacHasCQD(Boolean);
Boolean tsysMacHasGWorlds(void);
Boolean tsysMacHasSys7(void);
Boolean tsysMacHasSndMgr3(void);
Boolean tsysIsPowerMac(void);
long SetMinFastMachine(long);
Boolean fCPUSucksMolasses(void);
Boolean tsysMacHasQT(long *);
#endif

#ifdef _WINDOWS

#endif

#endif	/* __TPS_SYS__ */

