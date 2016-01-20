/*
	File: macSys.c
	
	Mac implementation of TPS system utilities
	
	Client:	Turning Point Software, Inc.
	Copyright ©1995 Turning Point Software, Inc. All rights reserved.
	
	History:
	
		2/14/95	Rich Siegel			Creation.
		--	additional revision comments go here --
*/

#pragma mark TPS Libs
/*	TPS library includes follow	*/

#include "tpsTypes.h"
#include "tpsDebug.h"
#include "tErrDefs.h"
#include "tpsSys.h"

#if (! defined(MAC)) || defined(_WINDOWS)
#error This is a Mac-only source file.
#endif

#ifdef ASSERTNAME
ASSERTNAME
#endif

#pragma mark Private Macros
/*	private macros follow	*/

#define MINIMUM_FAST_MACHINE	0x138C	// the value of TimeDBRA on a baseline
										//	machine (in this case a 20Mhz 68040

#pragma mark Private Types
/*	private types follow	*/
//	-- define module-private data types here --

#pragma mark Private Globals
/*	private globals follow	*/

static long	_msMinTimeDBRA = MINIMUM_FAST_MACHINE;	//	default baseline machine

#pragma mark -
#pragma mark Private Functions
/*	private functions follow */

/*
	Function Name:	_sTrapType
	
	returns whether the input trap number is a toolbox or an OS trap
	
	On Entry:
	
		trap	->	Toolbox trap word
	
	Returns:
	
		predefined value "ToolTrap" if trap is a Toolbox trap;
		predefined value "OSTrap" if trap is an OS trap
		
*/
static short  _sTrapType (short trap)
{
	if (trap & 0x0800)
		return ToolTrap;
	else
		return OSTrap;
}
	

/*
	Function Name:	
	
		determines the size of the Toolbox trap table on the current machine
	
	Returns:
	
		size of Toolbox trap table
		
*/
static short  _sToolboxTraps (void)
{
	if (NGetTrapAddress(_InitGraf, ToolTrap) == NGetTrapAddress(0xAA6E, ToolTrap))
		return 0x200;
	else
		return 0x400;
}
	

#pragma mark -
#pragma mark Public Globals
/*	public globals follow	*/
//	-- define public globals here --

#pragma mark -
#pragma mark Public Functions
/*	public functions follow */
//	-- define public functions here --

/*
	Function Name:	tsysHasMacTrap
	
	determines whether a specific Macintosh trap is implemented
	
	On Entry:
	
		trap	->	Toolbox/OS trap word
			
	Returns:
	
		TRUE if trap word "trap" is implemented, FALSE if not
		
*/

Boolean  tsysHasMacTrap (short trap)
{
short		type;

	type = _sTrapType(trap);
	if (type == ToolTrap)
		if ((trap &= 0x07FF) >= _sToolboxTraps())
			trap = _Unimplemented;

	return(NGetTrapAddress(trap, type) != NGetTrapAddress(_Unimplemented, ToolTrap));
}
	
/*
	Function Name:	tsysMacMonitorIsActive
	
	determines whether a specified graphics device is an active display
	
	On Entry:
	
		device	->	handle to Color Quickdraw graphics device
	
	Returns:
	
		TRUE if "device" is an active display, FALSE if not
		
*/
Boolean  tsysMacMonitorIsActive (GDHandle device)
{
    if (device == nil)  
       return false;
    if (device == GetMainDevice())                         // main device always active
       return true;
    if (TestDeviceAttribute(device, screenDevice) && 
        TestDeviceAttribute(device, screenActive))
       return true;
    return false;
}  


/*
	Function Name:	tsysMacMonitorHasDepth
	
	determines whether a given screen device supports a required bit depth
	
	On Entry:
	
		device	->	handle to Color Quickdraw graphics device
		depth	->	bit depth (bits per pixel) needed
		
	Returns:
	
		TRUE if the given device is an active screen -and- it supports
		the requested bit depth; FALSE if the device is not an active
		display -or- it does not support the required bit depth.
		
*/
Boolean  tsysMacMonitorHasDepth (GDHandle device, short depth)
{
    if (tsysMacMonitorIsActive(device) && HasDepth(device, depth, 0, 0))
       return true;
    return false;
}  

/*
	Function Name:	tsysGetMacMonitorOfDepth
	
	If possible, finds a graphics device capable of supporting a
	desired bit depth.
		
	On Entry:
	
		depth	->	required bit depth
	
	Returns:
	
		Handle to a graphics device that supports the required
		bit depth, or NIL if no such device exists.
		
*/
GDHandle  tsysGetMacMonitorOfDepth (short depth)
{
GDHandle             screen;

	if ((! tsysMacHasSys7()) || (! tsysMacHasCQD(false))) 
		return nil;
    screen = GetMainDevice();	// check main device first
    if (tsysMacMonitorHasDepth(screen, depth)) 
		return screen;
	screen = GetDeviceList();
    while (screen != nil)
    {
		if (tsysMacMonitorHasDepth(screen, depth)) 
			return screen;
		screen = GetNextDevice(screen);  
    }
	return nil;
}  


/*
	Function Name:	tsysMacHasGestalt
	
	Determines whether the _Gestalt trap is available on the current machine.
	
	Returns:
	
	TRUE if _Gestalt is implemented, FALSE if not.
		
*/
Boolean  tsysMacHasGestalt (void)
{
	return tsysHasMacTrap(_Gestalt);
}


/*
	Function Name:	tsysMacHasCQD
	
	Determines whether Color Quickdraw is running on this machine.
		
	On Entry:
	
		f32Bit	->	TRUE if 32-bit Quickdraw is required, FALSE if not.
	
	Returns:
	
		TRUE if the desired version of Color Quickdraw is running,
		FALSE if not.
			
*/
Boolean  tsysMacHasCQD (Boolean f32Bit)
{
long			response;

    if (Gestalt(gestaltQuickdrawVersion, &response) != noErr)
		return false;
    if ((response & gestalt32BitQD) != 0)
		return true;
    if (((response & gestalt8BitQD) != 0) && !f32Bit)
		return true;
	return false;
}


/*
	Function Name:	tsysMacHasGWorlds
	
	Determines whether the machine supports color GWorlds.
	
	
	Returns:
	
		TRUE if color GWorlds are supported on this machine,
		FALSE if not.
*/
Boolean  tsysMacHasGWorlds (void)
{
long			response;

    if (Gestalt(gestaltQuickdrawFeatures, &response) != noErr)
		return false;
    if (response >= gestaltHasDeepGWorlds)
		return true;
	return false;
}


/*
	Function Name:	tsysMacHasSys7
		
	Determines whether System 7.0 or later is running.
	
	Returns:
	
		TRUE if System 7.0 or later is running, FALSE if not.
		
*/
Boolean  tsysMacHasSys7 (void)
{
long			result = 0;

	if (GetToolTrapAddress(_Gestalt) == GetToolTrapAddress(_Unimplemented) || 
		Gestalt(gestaltSystemVersion, &result) != noErr)
		result = 0;

	return(result >= 0x0700);
}

/*
	Function Name:	tsysMacHasSndMgr3
		
	Determines whether Sound Manager 3.0 or later is running.
	
	Returns:
	
		TRUE if Sound Manager 3.0 or later is running, FALSE if not.
		
*/
Boolean tsysMacHasSndMgr3(void)
{
	if (! tsysHasMacTrap(_SoundDispatch))
		return FALSE;
		
	{
		NumVersion	snd_vers = SndSoundManagerVersion();
		
		return (snd_vers.majorRev >= 3);
	}
}

/*
	Function Name:	tsysIsPowerMac
	
	Determines whether the current native CPU is a PowerPC 601 or better.
	
	Returns:
	
		TRUE if the hardware CPU is a PowerPC 601 or better, FALSE if not.
*/
Boolean  tsysIsPowerMac (void)
{
long	result = 0;

	if (Gestalt(gestaltNativeCPUtype, &result) != noErr)
		result = 0;

	return (result >= gestaltCPU601);
}

/*
	Function Name:	SetMinFastMachine
	
		Establishes a new baseline value (used by fCPUSucksMolasses())
	
	On Entry:
	
		NewTimeDBRA	->	new minimum machine value
	
	Returns:
	
		old minimum machine value
		
	Side Effects:
	
		changes the _msMinTimeDBRA static global.
*/

long	SetMinFastMachine(long NewTimeDBRA)
{
	long oldTimeDBRA = _msMinTimeDBRA;

	_msMinTimeDBRA = NewTimeDBRA;

	return(oldTimeDBRA);
}

/*
	Function Name:	fCPUSucksMolasses
	
		Determines whether the current machine is slower than a
		minimum requirement.
	
	Returns:
	
		TRUE if the machine is slower than the minimum given in
		_msMinTimeDBRA.
*/

Boolean  fCPUSucksMolasses (void)
{
	return (_msMinTimeDBRA >= LMGetTimeDBRA());
}


/*
	Function Name:	tsysMacHasQT
	
		Determines whether QuickTime is running on this machine.
	
	On Entry:
	
		version	->	pointer to value to hold current QuickTime version.
	
	On Exit:
	
		*version	->	set to current QuickTime version.
		
	Returns:
	
		TRUE if QuickTime is running, FALSE if not.
*/
Boolean  tsysMacHasQT (long *version)
{
    if (Gestalt(gestaltQuickTime, version) != noErr)
		return false;
    if (*version == 0)
		return false;
	return true;
}

