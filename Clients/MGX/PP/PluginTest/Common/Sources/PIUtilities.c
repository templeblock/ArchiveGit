//-------------------------------------------------------------------------------
//
//	File:
//		PIUtilities.c
//
//	Copyright 1993-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This library contains sources and functions
//		to simplify the use of suites and also some helpful
//		common plug-in functions. 
//
//	Use:
//		PIUtilities is intended to group common functions
//		into higher-level macros and routines to simplify
//		plug-in programming.
//
//		Most expect A4-globals to be set-up already, for
//		gStuff to be a valid pointer to your global structure's
//		parameter block, and, on Windows, for hDllInstance to
//		be a global handle reference to your plug-in DLL.
//
//	Version history:
//		Version 1.0.0	3/1/1993	Ace		Created for Photoshop 2.5.
//			Written by Mark Hamburg.
//
//		Version 1.0.1	4/8/1997	Ace		Updated for Photoshop 4.0.1.
//			Comments fattened out.  Reorganized for clarity.
//
//		Version 1.0.2	8/24/1997	Ace		Updated for Photoshop 5.0.
//			Added SPBasicSuiteAvailable functions.
//
//-------------------------------------------------------------------------------

#include "PIUtilities.h"

//-------------------------------------------------------------------------------
//
//	PIResetString
//	
//	Clears a string by zeroing its first two bytes (takes care of Pascal and
//	C strings.)
//
//	Note: This will automatically get inlined on non-debug builds.
//
//	Inputs:
//		unsigned char *string		Any string, at least two bytes.
//
//	Outputs:
//		unsigned char *string		String reset to 0.
//
//-------------------------------------------------------------------------------

void PIResetString(unsigned char *string)
{
	string[0] = '\0';
	string[1] = '\0';
}

//-------------------------------------------------------------------------------
//
//	HostGetPlatformWindowPtr
//	
//	Returns the window pointer from an AboutRecord.  Windows only.
//	returns NULL on Macintosh (AboutRecord is not provided on Mac platform.)
//
//	Inputs:
//		AboutRecordPtr aboutPtr			Pointer to AboutRecord.
//
//	Outputs:
//		returns Handle platform->hwnd	Or NULL if Macintosh/not available.
//
//-------------------------------------------------------------------------------

Handle HostGetPlatformWindowPtr (AboutRecordPtr aboutPtr)
{
	// make platform a pointer to the platform data (we only need this for
	// Windows, but we'll at least initialize it so Macintosh won't
	// complain):
	PlatformData *platform = (PlatformData *) (aboutPtr->platformData);

	#ifdef __PIWin__
	
		return (Handle)platform->hwnd; // return the handle to the window

	#else // Macintosh

		return NULL;

	#endif
	
} // end HostGetPlatformWindowPtr

//-------------------------------------------------------------------------------
//
//	HostDisplayPixelsAvailable
//	
//	Determines whether the DisplayPixels callback is available.
//
//	Inputs:
//		DisplayPixelsProc proc		Pointer to DisplayPixelsProc.
//
//	Outputs:
//		returns TRUE				If the DisplayPixels callback is available.
//		returns FALSE				If the DisplayPixels callback is absent.
//
//-------------------------------------------------------------------------------

Boolean HostDisplayPixelsAvailable (DisplayPixelsProc proc)
{
	Boolean	available = TRUE;
	
	if (proc == NULL)
		available = FALSE;
	
	return available;

} // end HostDisplayPixelsAvailable

//-------------------------------------------------------------------------------
//
//	WarnHostDisplayPixelsAvailable
//	
//	Shows an alert if the DisplayPixels callback is not available.
//
//	Inputs:
//		DisplayPixelsProc proc		Pointer to DisplayPixelsProc.
//
//		Handle hDllInstance			Windows DLLInstance global handle.
//
//		AlertID						#define with the ID of the "Need host"
//									alert.  See header file.
//
//		kNeedVers					#define with the ID of the "Need host ^0"
//									string.  See header file.
//
//	Outputs:
//		Pops a dialog with the warning "Need host version 2.5.2." if the
//		proc is not available.
//
//		returns TRUE				If the DisplayPixels callback is available.
//		returns FALSE				If the DisplayPixels callback is absent.
//
//-------------------------------------------------------------------------------

Boolean WarnHostDisplayPixelsAvailable (DisplayPixelsProc proc,
										Handle hDllInstance)
{
	Boolean available = HostDisplayPixelsAvailable(proc);
	
	if (!available)
	{
		Str255 minVersion = "";
		
		// Converts a double/float value to a string, with 3 digits of
		// decimal precision:
		DoubleToString(minVersion, 2.502, 3);
		minVersion[ minVersion[0]-1 ] = '.'; // replace the 0 with a "." to make 2.5.2
		
		// Use our routine to pop a dialog saying "Sorry, need host
		// version ^0 or better" where ^0 will be replaced by the
		// string we just created:
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  kNeedVers,
						  minVersion,
						  NULL);
	}
	
	return available;

} // end WarnHostDisplayPixelsAvailable
	
//-------------------------------------------------------------------------------
//
//	HostAdvanceStateAvailable
//	
//	Determines whether the AdvanceState callback is available.
//
//	Inputs:
//		AdvanceStateProc proc		Pointer to AdvanceStateProc.
//
//	Outputs:
//		returns TRUE				If the AdvanceState callback is available.
//		returns FALSE				If the AdvanceState callback is absent.
//
//-------------------------------------------------------------------------------

Boolean HostAdvanceStateAvailable (AdvanceStateProc proc)
{
	Boolean	available = TRUE;
	
	if (proc == NULL)
		available = FALSE;
	
	return available;
	
} // end HostAdvanceStateAvailable

//-------------------------------------------------------------------------------
//
//	WarnHostAdvanceStateAvailable
//	
//	Shows an alert if the AdvanceState callback is not available.
//
//	Inputs:
//		AdvanceStateProc proc		Pointer to AdvanceStateProc.
//
//		Handle hDllInstance			Windows DLLInstance global handle.
//
//		AlertID						#define with the ID of the "Need host"
//									alert.  See header file.
//
//		kNeedVers					#define with the ID of the "Need host ^0"
//									string.  See header file.
//
//	Outputs:
//		Pops a dialog with the warning "Need host version 3.0" if the
//		proc is not available.
//
//		returns TRUE				If the AdvanceState callback is available.
//		returns FALSE				If the AdvanceState callback is absent.
//
//-------------------------------------------------------------------------------

Boolean WarnHostAdvanceStateAvailable (AdvanceStateProc proc,
									   Handle hDllInstance)
{
	Boolean available = HostAdvanceStateAvailable(proc);
	
	if (!available)
	{
		Str255	minVersion = "";
		
		// Converts a double/float value to a string, with 1 digit of
		// decimal precision:
		DoubleToString(minVersion, 3.0, 1);
	
		// Use our routine to pop a dialog saying "Sorry, need host
		// version ^0 or better" where ^0 will be replaced by the
		// string we just created:
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  kNeedVers,
						  minVersion,
						  NULL);
	}
	
	return available;

} // end WarnHostAdvanceStateAvailable

//-------------------------------------------------------------------------------
//
//	HostColorServicesAvailable
//	
//	Determines whether the ColorServices callback is available.
//
//	Inputs:
//		ColorServicesProc proc		Pointer to ColorServicesProc.
//
//	Outputs:
//		returns TRUE				If the ColorServices callback is available.
//		returns FALSE				If the ColorServices callback is absent.
//
//-------------------------------------------------------------------------------

Boolean HostColorServicesAvailable (ColorServicesProc proc)
{
	Boolean	available = TRUE;
	
	if (proc == NULL)
		available = FALSE;
	
	return available;
	
} // end HostColorServicesAvailable

//-------------------------------------------------------------------------------
//
//	WarnHostColorServicesAvailable
//	
//	Shows an alert if the ColorServices callback is not available.
//
//	Inputs:
//		ColorServicesProc proc		Pointer to ColorServicesProc.
//
//		Handle hDllInstance			Windows DLLInstance global handle.
//
//		AlertID						#define with the ID of the "Need host"
//									alert.  See header file.
//
//		kNeedVers					#define with the ID of the "Need host ^0"
//									string.  See header file.
//
//	Outputs:
//		Pops a dialog with the warning "Need host 2.5" if the proc is
//		not available.
//
//		returns TRUE				If the ColorServices callback is available.
//		returns FALSE				If the ColorServices callback is absent.
//
//-------------------------------------------------------------------------------

Boolean WarnHostColorServicesAvailable (ColorServicesProc proc,
									    Handle hDllInstance)
{
	Boolean available = HostColorServicesAvailable(proc);
	
	if (!available)
	{
		Str255 minVersion = "";
		
		// Converts a double/float value to a string, with 1 digit of
		// decimal precision:
		DoubleToString(minVersion, 2.5, 1);
	
		// Use our routine to pop a dialog saying "Sorry, need host
		// version ^0 or better" where ^0 will be replaced by the
		// string we just created:
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  kNeedVers,
						  minVersion,
						  NULL);
	}

	return available;

} // end WarnHostColorServicesAvailable

//-------------------------------------------------------------------------------
//
//	CSSetColor
//	
//	Sets an array of 4 int16 colors to individual color values.
//
//	Inputs:
//		int16 *outColor				Pointer to array of 4 int16s to set.
//
//		const int16 inColor1		Colors to set array to.
//		const int16 inColor2
//		const int16 inColor3
//		const int16 inColor4
//
//	Outputs:
//		returns noErr				If copy went okay.
//		returns OSErr				Probably missing a parameter (such
//									as a bad pointer.)
//
//		int16 outColor				Array of 4 int16s set to inColors.
//
//-------------------------------------------------------------------------------

OSErr CSSetColor (int16 *outColor,
	 			  const int16 inColor1,
				  const int16 inColor2,
				  const int16 inColor3,
				  const int16 inColor4)
{
	OSErr err = noErr;
	
	if (outColor != NULL)
	{
		outColor[0] = inColor1;
		outColor[1] = inColor2;
		outColor[2] = inColor3;
		outColor[3] = inColor4;
	}
	else
	{
		err = errMissingParameter;
	}
	
	return err;
	
} // end CSSetColor

//-------------------------------------------------------------------------------
//
//	CSCopyColor
//	
//	Copys an array of 4 int16 colors to another array.
//
//	Inputs:
//		int16 *outColor				Pointer to array of 4 int16s to set.
//
//		const int16 *inColor		Pointer to source array of 4 int16s.
//
//	Outputs:
//		returns noErr				If copy went okay.
//		returns OSErr				Probably missing a parameter (such
//									as a bad pointer.)
//
//		int16 *outColor				Array of 4 int16s set to inColor.
//
//-------------------------------------------------------------------------------

OSErr CSCopyColor (int16 *outColor, const int16 *inColor)
{
	short loop;
	OSErr err = noErr;
	
	if (outColor != NULL && inColor != NULL)
	{
		for (loop = 0; loop < 4; loop++)
		{
			outColor[loop] = inColor[loop];
		}
	}
	else
	{
		err = errMissingParameter;
	}

	return err;

} // end CSCopyColor

//-------------------------------------------------------------------------------
//
//	CSToPicker
//	
//	Converts colors from ColorServices space to Color Picker components
//	space.
//
//	Inputs:
//		const int16 *inColor		Pointer to ColorServices source array of 4 int16s.
//
//	Outputs:
//		unsigned16 *outColor		Pointer to PickParms array of 4 uint16s to set.
//
//		returns noErr				If copy went okay.
//		returns OSErr				Probably missing a parameter (such
//									as a bad pointer.)
//
//	Note:
//		Color Services space is basically an unsigned8 space in an int16.
//		Color Picker space is an 8.8 fixed integer space.
//
//-------------------------------------------------------------------------------

OSErr CSToPicker (unsigned16 *outColor, const int16 *inColor)
{
	short loop;
	OSErr err = noErr;
	
	if (outColor != NULL && inColor != NULL)
	{
		for (loop = 0; loop < 4; loop++)
		{
			outColor[loop] = (inColor[loop] << 8);
		}
	}
	else
	{
		err = errMissingParameter;
	}

	return err;

} // end CSToPicker

//-------------------------------------------------------------------------------
//
//	CSFromPicker
//	
//	Converts colors from Color Picker components space to ColorServices space.
//
//	Inputs:
//		unsigned16 *outColor		Pointer to PickParms array of 4 uint16s.
//
//	Outputs:
//		const int16 *inColor		Pointer to ColorServices source array of 4 int16s.
//
//		returns noErr				If copy went okay.
//		returns OSErr				Probably missing a parameter (such
//
//	Note:
//		Color Services space is basically an unsigned8 space in an int16.
//		Color Picker space is an 8.8 fixed integer space.
//
//		WARNING: You lose precision with this routine because 8.8 space is
//		labotomized into unsigned8 space, discarding the lower bits.  If
//		you want that resolution, convert each unit separately to a double
//		or use them as 8.8 fixed numbers and/or do not use ColorServices.
//
//-------------------------------------------------------------------------------

OSErr CSFromPicker (int16 *outColor, const unsigned16 *inColor)
{
	short loop;
	OSErr err = noErr;
	
	if (outColor != NULL && inColor != NULL)
	{
		for (loop = 0; loop < 4; loop++)
		{
			outColor[loop] = (inColor[loop] >> 8);
		}
	}
	else
	{
		err = errMissingParameter;
	}

	return err;

} // end CSToPicker

//-------------------------------------------------------------------------------
//
//	CSPlanesFromMode
//	
//	Returns the number of expected planes for any valid image mode.
//
//	Inputs:
//		const int16 imageMode		Any valid image mode.  See PIGeneral.h.
//
//		const int16 currPlanes		Number of current planes.
//
//	Outputs:
//		returns int16				Number of expected planes for that
//									color mode, or currPlanes if in
//									multiChannel or an unsupported mode.
//
//-------------------------------------------------------------------------------

int16 CSPlanesFromMode (const int16 imageMode,
						const int16 currPlanes)
{
	int16 planes = currPlanes;
	// Default to current planes if we can't find right set.
	
	// If we're in multichannel mode, the planes value is
	// dynamic, so we'll just return the current number of
	// planes for that.  (What else can we do?)
	
	if (imageMode >= plugInModeBitmap && imageMode <= plugInModeRGB48 &&
		imageMode != plugInModeMultichannel)
	{ // are we within bounds?
	
		/* static */ const int16 planesPerMode [] =
		{ // Set up a small little array to return the values
		  // we need.  If you have A4-globals set up
		  // (Macintosh) you can make this static to
		  // make its access faster on subsequent
		  // lookups.  If you don't have A4-globals set up
		  // and you need them (Macintosh 68k) then you
		  // will have garbage next time it's used:
		  
			/* plugInModeBitmap */			1,
			/* plugInModeGrayScale */		1,
			/* plugInModeIndexedColor */	3,
			/* plugInModeRGBColor */		3,
			/* plugInModeCMYKColor */		4,
			/* plugInModeHSLColor */		3,
			/* plugInModeHSBColor */		3,
			/* plugInModeMultichannel */	0, // special case
			/* plugInModeDuotone */			1,
			/* plugInModeLabColor */		3,
			/* plugInModeGray16 */			1,
			/* plugInModeRGB48 */			3
		};
		
		planes = planesPerMode[imageMode];
	}
	
	return planes;

} // end CSPlanesFromMode

//-------------------------------------------------------------------------------
//
//	CSModeToSpace
//	
//	Returns the number of expected planes for any valid image mode.
//
//	Inputs:
//		const int16 imageMode		Any valid image mode.  See PIGeneral.h.
//
//	Outputs:
//		returns int16				plugIncolorServices space, or
//									plugIncolorServicesChosenSpace, if
//									invalid/out of range.
//
//-------------------------------------------------------------------------------

int16 CSModeToSpace (const int16 imageMode)
{
	int16 space = plugIncolorServicesChosenSpace;
	// Default to same space.

	if (imageMode >= plugInModeBitmap && imageMode <= plugInModeRGB48)
	{
		/* static */ const int16 modePerSpace [] =
		{ // little array to map modes to color space
		  // values.  Make this static if you have
		  // A4-global space set up (Macintosh 68k) and this
		  // will run faster.  If you need A4-space and
		  // don't have it set up, and this is made
		  // static, you'll have garbage next time
		  // it's used:
		  
			/* plugInModeBitmap */			plugIncolorServicesChosenSpace,
			/* plugInModeGrayScale */		plugIncolorServicesGraySpace,
			/* plugInModeIndexedColor */	plugIncolorServicesChosenSpace,
			/* plugInModeRGBColor */		plugIncolorServicesRGBSpace,
			/* plugInModeCMYKColor */		plugIncolorServicesCMYKSpace,
			/* plugInModeHSLColor */		plugIncolorServicesHSLSpace,
			/* plugInModeHSBColor */		plugIncolorServicesHSBSpace,
			/* plugInModeMultichannel */	plugIncolorServicesChosenSpace,
			/* plugInModeDuotone */			plugIncolorServicesGraySpace,
			/* plugInModeLabColor */		plugIncolorServicesLabSpace,
			/* plugInModeGray16 */			plugIncolorServicesGraySpace,
			/* plugInModeRGB48 */			plugIncolorServicesRGBSpace
		};
		
		space = modePerSpace[imageMode];
	
	} // If unsupported mode, will return current space.
	
	return space;
	
} // end CSModeToSpace

//-------------------------------------------------------------------------------
//
//	HostCSConvertColor
//	
//	Converts a color one color space to another.
//
//	Inputs:
//		ColorServicesProc proc		Pointer to the Color Services callback.
//
//		const int16 sourceSpace		Any valid color services color space.
//									See PIGeneral.h.
//
//		const int16 resultSpace		Any valid color services target space.
//									See PIGeneral.h.
//
//		int16 *ioColor				Four int16s for input and converted color.
//
//	Outputs:
//		returns OSErr				Returns noErr and converts the color,
//									or an error (if one occurs) and leaves the
//									original color.
//
//									Returns plugInHostInsufficient if
//									the suite was unavailable.
//
//									You should call WarnColorServicesAvailable()
//									before you use this routine, anyway.
//									
//		int16 *ioColor				Original color components converted
//									to target space.
//
//-------------------------------------------------------------------------------

OSErr HostCSConvertColor (ColorServicesProc proc,
						 const int16 sourceSpace,
						 const int16 resultSpace,
						 int16 *ioColor)
{
	OSErr err = noErr;
	
	if (HostColorServicesAvailable(proc))
	{ // now populate color services info with stuff
		if (ioColor != NULL)
		{ // Parameter good.

			ColorServicesInfo	csinfo;
	
			// Initialize our info structure with default
			// values:
			err = CSInitInfo(&csinfo);
			
			if (err == noErr)
			{ // Init'ed okay.				
			
				// Now override default values with our own.
				
				// (1) Copy original to colorComponents space then
				// set up space to convert from source to result:
				err = CSCopyColor(&csinfo.colorComponents[0], ioColor);
		
				if (err == noErr)
				{ // Copied okay.
					// (2) Set the source and result space:
					csinfo.sourceSpace = sourceSpace;
					csinfo.resultSpace = resultSpace;
					
					// (3) Call the convert routine with this info:
					err = (*proc)(&csinfo);
			
					// (4) If no error, copy the converted colors:
					if (err == noErr)
						CSCopyColor (ioColor, csinfo.colorComponents);
			
				} // copy
				
			} // initinfo

		} // ioColor

		else
		{ // ioColor pointer bad.
			err = errMissingParameter;
		}
	}
	else
	{ // color services suite was not available
		err = errPlugInHostInsufficient;
	}
	
	return err;

} // end HostCSConvertColor

//-------------------------------------------------------------------------------
//
//	HostCSPickColor
//	
//	Shows the color picker and returns the user selected color.
//
//	Inputs:
//		ColorServicesProc proc		Pointer to the Color Services callback.
//
//		const Str255 inPrompt		Prompt string.
//
//		int16 *ioSpace				Source (initial) space.
//
//		int16 *ioColor				Source (initial) color.
//
//	Outputs:
//		returns OSErr				Returns noErr and converts the color,
//									or an error (if one occurs) and leaves the
//									original color.
//
//									Returns plugInHostInsufficient if
//									the suite was unavailable.
//
//									You should call WarnColorServicesAvailable()
//									before you use this routine, anyway.
//									
//		int16 *ioSpace				Target space of picked color.
//
//		int16 *ioColor				Color components converted
//									to target space.
//
//-------------------------------------------------------------------------------

OSErr HostCSPickColor (ColorServicesProc proc,
					   const Str255 inPrompt,
					   int16 *ioSpace,
					   int16 *ioColor)
{
	OSErr err = noErr;
	
	if (HostColorServicesAvailable(proc))
	{ // now populate color services info with stuff
		if (ioColor != NULL && ioSpace != NULL)
		{ // Parameters good.

			ColorServicesInfo	csinfo;
	
			// Initialize our info structure with default
			// values:
			err = CSInitInfo(&csinfo);
			
			if (err == noErr)
			{ // Init'ed okay.				
			
				// Now override default values with our own.
				
				// (1) Set the selector to choose color:
				csinfo.selector = plugIncolorServicesChooseColor;
				
				// (2) Set the prompt string:
				csinfo.selectorParameter.pickerPrompt = (Str255 *)inPrompt;
				
				// (3) Set the source components:
				csinfo.sourceSpace = *ioSpace;
				CSCopyColor (csinfo.colorComponents, ioColor);
				
				// (4) Call the convert routine with this info:
				err = (*proc)(&csinfo);
			
				// (5) If no error, copy the converted colors
				// and user-picked result space:
				if (err == noErr)
				{
					CSCopyColor (ioColor, csinfo.colorComponents);
					*ioSpace = csinfo.resultSpace;
				} // copy
				
			} // initinfo

		} // outColor and outSpace

		else
		{ // outColor or outSpace pointer bad.
			err = errMissingParameter;
		}
	}
	else
	{ // color services suite was not available
		err = errPlugInHostInsufficient;
	}
	
	return err;

} // end HostCSPickColor

//-------------------------------------------------------------------------------
//
//	CSInitInfo
//	
//	Initializes a ColorServicesInfo space with default parameters.
//
//	Inputs:
//		ColorServicesInfo *ioCSinfo	ColorServicesInfo block to reset.
//
//	Outputs:
//		returns noErr				If reset and cleared successfully.
//		returns OSErr				Any errors.
//
//		ColorServicesInfo *ioCSinfo	Set to default parameters.
//
//-------------------------------------------------------------------------------

OSErr CSInitInfo (ColorServicesInfo *ioCSinfo)
{
	OSErr err = noErr;
	
	if (ioCSinfo != NULL)
	{
		CSSetColor(ioCSinfo->colorComponents, 0, 0, 0, 0);
	
		ioCSinfo->selector = plugIncolorServicesConvertColor;
		ioCSinfo->sourceSpace = plugIncolorServicesRGBSpace;
		ioCSinfo->resultSpace = plugIncolorServicesChosenSpace;
		ioCSinfo->reservedSourceSpaceInfo = NULL; // must be null
		ioCSinfo->reservedResultSpaceInfo = NULL; // must be null
		ioCSinfo->reserved = NULL; // must be null
		ioCSinfo->selectorParameter.pickerPrompt = NULL;
		ioCSinfo->infoSize = sizeof(*ioCSinfo);
	}
	else
	{
		err = errMissingParameter;
	}
	
	return err;

} // end CSInitInfo

//-------------------------------------------------------------------------------
//
//	HostBufferProcsAvailable
//	
//	Determines whether the BufferProcs callback is available.
//
//	Inputs:
//		BufferProcs *proc			Pointer to BufferProcs callback.
//
//	Outputs:
//		Boolean *outNewerVersion	TRUE if the host has a newer version
//									of the procs than the plug-in.
//									FALSE if the host has the same version
//									of the procs as the plug-in.
//
//		returns TRUE				If the BufferProcs callback is available.
//		returns FALSE				If the BufferProcs callback is absent.
//
//-------------------------------------------------------------------------------

Boolean HostBufferProcsAvailable (BufferProcs *procs, Boolean *outNewerVersion)
{
	
	Boolean available = TRUE;		// assume procs are available
	Boolean newerVersion = FALSE;	// assume we're running under correct version
	
	
	// We want to check for this stuff in a logical order, going from things
	// that should always be present to things that "may" be present.  It's
	// always a danger checking things that "may" be present because some
	// hosts may not leave them NULL if unavailable, instead pointing to
	// other structures to save space.  So first we'll check the main
	// proc pointer, then the version, the number of routines, then some
	// of the actual routines:
	
	if (procs == NULL)
	{
		available = FALSE;
	}	
	else if (procs->bufferProcsVersion < kCurrentBufferProcsVersion)
	{
		available = FALSE;
	}	
	else if (procs->bufferProcsVersion > kCurrentBufferProcsVersion)
	{	
		available = FALSE;
		newerVersion = TRUE;	
	}	
	else if (procs->numBufferProcs < kCurrentBufferProcsCount)
	{
		available = FALSE;
	}
	else if (procs->allocateProc == NULL ||
			 procs->lockProc == NULL ||
			 procs->unlockProc == NULL ||
			 procs->freeProc == NULL ||
			 procs->spaceProc == NULL)
	{
		available = FALSE;
	}
		
	if (newerVersion && (outNewerVersion != NULL))
		*outNewerVersion = newerVersion;
		
	return available;
	
} // end HostBufferProcsAvailable

//-------------------------------------------------------------------------------
//
//	WarnHostBufferProcsAvailable
//	
//	Shows an alert if the BufferProcs callback is not available.
//
//	Inputs:
//		BufferProcs *procs			Pointer to BufferProcs callback.
//
//		Handle hDllInstance			Windows DLLInstance global handle.
//
//		AlertID						#define with the ID of the "Need host"
//									alert.  See header file.
//
//		kNeedVers					#define with the ID of the "Need host ^0"
//									string.  See header file.
//
//		kWrongHost					#define with the ID of the "Wrong host"
//									string.  See header file.
//
//	Outputs:
//		Pops a dialog with the warning "Need at least version 2.5" or
//		the warning "This is the wrong host" if the proc is not available
//		and/or the needed version is newer than the host's version.
//
//		returns TRUE				If the BufferProcs callback is available.
//		returns FALSE				If the BufferProcs callback is absent.
//
//-------------------------------------------------------------------------------

Boolean WarnHostBufferProcsAvailable (BufferProcs *procs,
									  Handle hDllInstance)
{
	
	Boolean newerVersion = FALSE;
	Boolean available = HostBufferProcsAvailable (procs, &newerVersion);
	
	if (!available)
	{
		Str255 	minVersion = "";
		short	errStringID = kNeedVers;
		
		// If we need a later version of the suite, pop a dialog
		// saying we have the wrong host (who *knows* what version):
		if (newerVersion)
			errStringID = kWrongHost;

		// Converts a double/float value to a string, with 1 digit of
		// decimal precision:
		DoubleToString(minVersion, 2.5, 1);
		
		// Use our routine to pop a dialog saying "Sorry, need host
		// version ^0 or better" where ^0 will be replaced by the
		// string we just created, or one saying "This is the
		// wrong host":
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  errStringID,
						  minVersion,
						  NULL);
	}
			
	return available;
	
} // end WarnHostBufferProcsAvailable

//-------------------------------------------------------------------------------
//
//	HostBufferSpace
//	
//	Returns the amount of space available in the host buffer.
//
//	Inputs:
//		BufferProcs *procs			Pointer to ColorServicesProc.
//
//	Outputs:
//		returns int32				Number of bytes free in buffer or 0 if
//									none free or no procs available.
//
//-------------------------------------------------------------------------------

int32 HostBufferSpace (BufferProcs *procs)
{
	
	if (HostBufferProcsAvailable (procs, NULL))
		return (*procs->spaceProc) ();	
	else
		return 0;
	
} // end HostBufferSpace

//-------------------------------------------------------------------------------
//
//	HostAllocateBuffer
//	
//	Allocates a buffer of a specified amount.
//
//	Inputs:
//		BufferProcs *procs			Pointer to BufferProcs callback.
//
//		const int32 inSize			Size of requested buffer.
//
//	Outputs:
//		returns OSErr				noErr if successful, or error code.
//
//		BufferID *outBufferID		Will be a valid ID to a buffer if noErr,
//									0 if an error occurred.
//
//-------------------------------------------------------------------------------

OSErr HostAllocateBuffer (BufferProcs *procs, const int32 inSize, BufferID *outBufferID)
{
	
	*outBufferID = 0;
	
	if (HostBufferProcsAvailable (procs, NULL))
		return (*procs->allocateProc) (inSize, outBufferID);
	else
		return memFullErr;
	
} // end HostAllocateBuffer

//-------------------------------------------------------------------------------
//
//	HostFreeBuffer
//	
//	Releases a buffer.
//
//	Inputs:
//		BufferProcs *procs			Pointer to BufferProcs callback.
//
//		const BufferID inBufferID	ID of buffer to release.
//
//	Outputs:
//		returns OSErr				noErr if successful, or error code.
//
//		BufferID *outBufferID		Will be a valid ID to a buffer if noErr,
//									0 if an error occurred.
//
//-------------------------------------------------------------------------------

void HostFreeBuffer (BufferProcs *procs, const BufferID inBufferID)
{
	
	if (HostBufferProcsAvailable (procs, NULL))
		(*procs->freeProc) (inBufferID);
	
} // end HostFreeBuffer

//-------------------------------------------------------------------------------
//
//	HostLockBuffer
//	
//	Locks a buffer and returns a pointer to it.
//
//	Lock and unlock calls manipulate a counter, so they must match exactly.
//
//	Inputs:
//		BufferProcs *procs			Pointer to BufferProcs callback.
//
//		const BufferID inBufferID	ID of buffer to lock.
//
//		Boolean inMoveHigh			Move the buffer high in memory?
//
//	Outputs:
//		returns Ptr					Locks the buffer and returns a pointer to
//									it, if successful; NULL if not.
//
//-------------------------------------------------------------------------------

Ptr HostLockBuffer (BufferProcs *procs, 
					const BufferID inBufferID, 
					Boolean inMoveHigh)
{
	
	if (HostBufferProcsAvailable (procs, NULL))
		return (*procs->lockProc) (inBufferID, inMoveHigh);
	else
		return NULL;
	
} // end HostLockBuffer

//-------------------------------------------------------------------------------
//
//	HostUnlockBuffer
//	
//	Unlocks a buffer.
//
//	Lock and unlock calls manipulate a counter, so they must match exactly.
//
//	Inputs:
//		BufferProcs *procs			Pointer to BufferProcs callback.
//
//		const BufferID inBufferID	ID of buffer to lock.
//
//		Boolean inMoveHigh			Move the buffer high in memory?
//
//	Outputs:
//		returns Ptr					Locks the buffer and returns a pointer to
//									it, if successful; NULL if not.
//
//-------------------------------------------------------------------------------

void HostUnlockBuffer (BufferProcs *procs,
					   const BufferID inBufferID)
{
	
	if (HostBufferProcsAvailable (procs, NULL))
		(*procs->unlockProc) (inBufferID);
	
} // end HostUnlockBuffer

//-------------------------------------------------------------------------------
//
//	HostAllocateStripBuffer
//	
//	Allocates a buffer which is as tall as possible.  It sets the actual
//	height and bufferID parameters if successful.
//
//	Inputs:
//		BufferProcs *procs			Pointer to BufferProcs callback.
//
//		const int32 inRowBytes		The desired rowBytes.
//
//		const int16 inMinHeight		The minimum height.
//
//		const int16 inMaxHeight		The maximum height.
//
//		const int16 inNumBuffers	The fraction of the available buffer
//									space to use expressed as 1/inNumBuffers.
//
//	Outputs:
//		returns OSErr				noErr and actual height and bufferID
//									if successful, error if not.
//
//		int16 *outActualHeight		Actual height of buffer allocated,
//									if successful.
//
//		BufferID *outBufferID		ID of buffer allocated, if successful.
//
//-------------------------------------------------------------------------------

OSErr HostAllocateStripBuffer (BufferProcs *procs,
							   const int32 inRowBytes,
							   const int16 inMinHeight,
							   const int16 inMaxHeight,
							   const int16 inNumBuffers,
							   int16 *outActualHeight,
							   BufferID *outBufferID)
{

	//---------------------------------------------------------------------------
	//	(1) Allocate and set-up our parameters
	//---------------------------------------------------------------------------

	OSErr err = noErr;

	int32 size;

	const int32 availableSpace = HostBufferSpace (procs) / inNumBuffers;	
	const int32 availableHeight = availableSpace / inRowBytes;
	
	int32 height = (int32)inMaxHeight;
	
	// If we don't have enough space for the maximum height,
	// use what is available:
	if (availableHeight < inMaxHeight)
		height = availableHeight;
		
	height++; // we'll need one extra iteration
	
	*outBufferID = NULL;

	size = inRowBytes * height;
	
	
	//---------------------------------------------------------------------------
	//	(2) Iterate down from maximum height to minimum height,
	//		attempting to allocate a buffer each time until successful.
	//		(Hopefully should happen on the very first iteration.)
	//---------------------------------------------------------------------------
	
	while (!*outBufferID && height > inMinHeight)
	{
		
		height--;
		size -= inRowBytes;
		
		err = HostAllocateBuffer (procs, size, outBufferID);
		
	}
		
	if (*outBufferID != NULL)
		*outActualHeight = (int16)height; // report height we ended up with
		
	return err;
	
} // end HostAllocateStripBuffer

//-------------------------------------------------------------------------------
//
//	HostResourceProcsAvailable
//	
//	Determines whether the ResourceProcs (Pseudo-Resources) callback is available.
//
//	A pseudo-resource is a cross-platform resource that is stored as
//	an Image Resource block in the data fork of the document with the
//	other document information.
//
//	Inputs:
//		ResourceProcs *proc			Pointer to ResourceProcs callback.
//
//	Outputs:
//		Boolean *outNewerVersion	TRUE if the host has a newer version
//									of the procs than the plug-in.
//									FALSE if the host has the same version
//									of the procs as the plug-in.
//
//		returns TRUE				If the ResourceProcs callback is available.
//		returns FALSE				If the ResourceProcs callback is absent.
//
//-------------------------------------------------------------------------------

Boolean HostResourceProcsAvailable (ResourceProcs *procs, Boolean *outNewerVersion)
{
	
	Boolean available = TRUE;		// assume procs are available
	Boolean newerVersion = FALSE;	// assume we're running under correct version
	
	// We want to check for this stuff in a logical order, going from things
	// that should always be present to things that "may" be present.  It's
	// always a danger checking things that "may" be present because some
	// hosts may not leave them NULL if unavailable, instead pointing to
	// other structures to save space.  So first we'll check the main
	// proc pointer, then the version, the number of routines, then some
	// of the actual routines:

	if (procs == NULL)
	{
		available = FALSE;
	}	
	else if (procs->resourceProcsVersion < kCurrentResourceProcsVersion)
	{
		available = FALSE;
	}	
	else if (procs->resourceProcsVersion > kCurrentResourceProcsVersion)
	{	
		available = FALSE;	
		newerVersion = TRUE;
	}
	else if (procs->numResourceProcs < kCurrentResourceProcsCount)
	{
		available = FALSE;
	}	
	else if (procs->countProc == NULL ||
			 procs->getProc == NULL ||
			 procs->deleteProc == NULL ||
			 procs->addProc == NULL)
	{
		available = FALSE;
	}
		
	if (newerVersion && (outNewerVersion != NULL))
		*outNewerVersion = newerVersion;
		
	return available;
	
} // end HostResourceProcsAvailable

//-------------------------------------------------------------------------------
//
//	WarnHostResourceProcsAvailable
//	
//	Shows an alert if the ResourceProcs (Pseudo-Resources) callback
//	is not available.
//
//	A pseudo-resource is a cross-platform resource that is stored as
//	an Image Resource block in the data fork of the document with the
//	other document information.
//
//	Inputs:
//		ResourceProcs *procs		Pointer to ResourceProcs callback.
//
//		Handle hDllInstance			Windows DLLInstance global handle.
//
//		AlertID						#define with the ID of the "Need host"
//									alert.  See header file.
//
//		kNeedVers					#define with the ID of the "Need host ^0"
//									string.  See header file.
//
//		kWrongHost					#define with the ID of the "Wrong host"
//									string.  See header file.
//
//	Outputs:
//		Pops a dialog with the warning "Need at least version 2.5" or
//		the warning "This is the wrong host" if the proc is not available
//		and/or the needed version is newer than the host's version.
//
//		returns TRUE				If the ResourceProcs callback is available.
//		returns FALSE				If the ResourceProcs callback is absent.
//
//-------------------------------------------------------------------------------

Boolean WarnHostResourceProcsAvailable (ResourceProcs *procs,
										Handle hDllInstance)
{
	
	Boolean newerVersion = FALSE;
	Boolean available = HostResourceProcsAvailable (procs, &newerVersion);
	
	if (!available)
	{
		Str255 	minVersion = "";
		short	errStringID = kNeedVers;
		
		// If we need a later version of the suite, pop a dialog
		// saying we have the wrong host (who *knows* what version):
		if (newerVersion)
			errStringID = kWrongHost;

		// Converts a double/float value to a string, with 1 digit of
		// decimal precision:
		DoubleToString(minVersion, 3.0, 1);
		
		// Use our routine to pop a dialog saying "Sorry, need host
		// version ^0 or better" where ^0 will be replaced by the
		// string we just created, or one saying "This is the
		// wrong host":
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  errStringID,
						  minVersion,
						  NULL);
	}
			
	return available;
	
} // end WarnHostResourceProcsAvailable

//-------------------------------------------------------------------------------
//
//	HostCountPIResources
//	
//	Returns the number of pseudo-resources in a document.
//
//	A pseudo-resource is a cross-platform resource that is stored as
//	an Image Resource block in the data fork of the document with the
//	other document information.
//
//	Inputs:
//		ResourceProcs *proc			Pointer to ResourceProcs callback.
//
//		const ResType inType		Resource type to count.  You can have
//									multiple resources in a document of
//									different types; this routine will
//									only count one specific type at a time.
//
//	Outputs:
//		returns int16				Number of resources found, 0 if none or
//									ResourceProcs not present.
//
//-------------------------------------------------------------------------------

int16 HostCountPIResources (ResourceProcs *procs,
							const ResType inType)
{
	if (HostResourceProcsAvailable (procs, NULL))
		return (*procs->countProc) (inType);
	else
		return 0;
	
} // end HostCountPIResources
	
//-------------------------------------------------------------------------------
//
//	HostGetPIResource
//	
//	Returns a handle to a specific pseudo-resource.
//
//	WARNING: This handle is NOT a copy of the resource, but a master handle
//			 right to the resource.  If you dispose it, you will dispose the
//			 the resource from the document and it will no longer be available.
//			 Use DeletePIResource to delete resources.
//
//	A pseudo-resource is a cross-platform resource that is stored as
//	an Image Resource block in the data fork of the document with the
//	other document information.
//
//	Inputs:
//		ResourceProcs *proc			Pointer to ResourceProcs callback.
//
//		const ResType inType		Resource type.
//
//		const int16 inIndex			Index of resource to retrieve.
//
//	Outputs:
//		returns Handle				Handle to resource found, or
//									NULL if not found or procs unavailable.
//
//-------------------------------------------------------------------------------

Handle HostGetPIResource (ResourceProcs *procs,
						  const ResType inType,
						  const int16 inIndex)
{
	if (HostResourceProcsAvailable (procs, NULL))
		return (*procs->getProc) (inType, inIndex);		
	else
		return NULL;

} // end HostGetPIResource

//-------------------------------------------------------------------------------
//
//	HostDeletePIResource
//	
//	Deletes a specific pseudo-resource.
//
//	A pseudo-resource is a cross-platform resource that is stored as
//	an Image Resource block in the data fork of the document with the
//	other document information.
//
//	Inputs:
//		ResourceProcs *proc			Pointer to ResourceProcs callback.
//
//		const ResType inType		Resource type.
//
//		const int16 inIndex			Index of resource to delete.
//
//	Outputs:
//		Deletes resource of type inType of index inIndex.
//		The resource will no longer be in the document.
//
//		NOTE: Subsequent Resource counts and indexes will be -1.
//			  If you're working with a record AFTER the one you
//			  deleted, everything will have moved up by one index.
//
//-------------------------------------------------------------------------------

void HostDeletePIResource (ResourceProcs *procs,
						   const ResType inType,
						   const int16 inIndex)
{	
	if (HostResourceProcsAvailable (procs, NULL))
		(*procs->deleteProc) (inType, inIndex);

} // end HostDeletePIResource

//-------------------------------------------------------------------------------
//
//	HostAddPIResource
//	
//	Adds a pseudo-resource to the current document.
//
//	A pseudo-resource is a cross-platform resource that is stored as
//	an Image Resource block in the data fork of the document with the
//	other document information.
//
//	Inputs:
//		ResourceProcs *proc			Pointer to ResourceProcs callback.
//
//		const ResType inType		Resource type.
//
//		const Handle inDataHandle	Handle to data to store.
//
//	Outputs:
//		Adds the passed resource to the document pseudo-resources.
//
//		NOTE: Subsequent Resource counts and indexes will be +1.
//			  It will always add to the end of the list.
//
//-------------------------------------------------------------------------------

OSErr HostAddPIResource (ResourceProcs *procs,
						 const ResType inType,
						 const Handle inDataHandle)
{
	
	if (HostResourceProcsAvailable (procs, NULL))
		return (*procs->addProc) (inType, inDataHandle);
	else
		return memFullErr;
			
} // end HostAddPIResource

//-------------------------------------------------------------------------------
//
//	HostHandleProcsAvailable
//	
//	Determines whether the HandleProcs callback is available.
//
//	The HandleProcs are cross-platform master pointers that point to
//	pointers that point to data that is allocated in the Photoshop
//	virtual memory structure.  They're reference counted and
//	managed more efficiently than the operating system calls.
//
//	WARNING:  Do not mix operating system handle creation, deletion,
//			  and sizing routines with these callback routines.  They
//			  operate differently, allocate memory differently, and,
//			  while you won't crash, you can cause memory to be
//			  allocated on the global heap and never deallocated.
//
//	Inputs:
//		HandleProcs *proc			Pointer to HandleProcs callback.
//
//	Outputs:
//		Boolean *outNewerVersion	TRUE if the host has a newer version
//									of the procs than the plug-in.
//									FALSE if the host has the same version
//									of the procs as the plug-in.
//
//		returns TRUE				If the HandleProcs callback is available.
//		returns FALSE				If the HandleProcs callback is absent.
//
//-------------------------------------------------------------------------------

Boolean HostHandleProcsAvailable (HandleProcs *procs,
								  Boolean *outNewerVersion)

{ // We only require the new, dispose, set size and get size procs.  You can
  // use the OS lock and unlock routines, if necessary.  Even with that,
  // however, lets just check for everything.
	
	Boolean available = TRUE;		// assume procs are available
	Boolean newerVersion = FALSE;	// assume we're running under correct version
	
	if (procs == NULL)
	{
		available = FALSE;
	}	
	else if (procs->handleProcsVersion < kCurrentHandleProcsVersion)
	{
		available = FALSE;
	}	
	else if (procs->handleProcsVersion > kCurrentHandleProcsVersion)
	{	
		available = FALSE;	
		newerVersion = TRUE;
	}	
	else if (procs->numHandleProcs < kCurrentHandleProcsCount)
	{
		available = FALSE;
	}
	else if (procs->newProc == NULL ||
			 procs->disposeProc == NULL ||
			 procs->getSizeProc == NULL ||
			 procs->setSizeProc == NULL)
	{
		available = FALSE;
	}
		
	if (newerVersion && (outNewerVersion != NULL))
		*outNewerVersion = newerVersion;
		
	return available;
	
} // end HostHandleProcsAvailable

//-------------------------------------------------------------------------------
//
//	WarnHostHandleProcsAvailable
//	
//	Determines whether the HandleProcs callback is available.
//
//	The HandleProcs are cross-platform master pointers that point to
//	pointers that point to data that is allocated in the Photoshop
//	virtual memory structure.  They're reference counted and
//	managed more efficiently than the operating system calls.
//
//	WARNING:  Do not mix operating system handle creation, deletion,
//			  and sizing routines with these callback routines.  They
//			  operate differently, allocate memory differently, and,
//			  while you won't crash, you can cause memory to be
//			  allocated on the global heap and never deallocated.
//
//	Inputs:
//		HandleProcs *procs			Pointer to HandleProcs callback.
//
//		Handle hDllInstance			Windows DLLInstance global handle.
//
//		AlertID						#define with the ID of the "Need host"
//									alert.  See header file.
//
//		kNeedVers					#define with the ID of the "Need host ^0"
//									string.  See header file.
//
//		kWrongHost					#define with the ID of the "Wrong host"
//									string.  See header file.
//
//	Outputs:
//		Pops a dialog with the warning "Need at least version 2.5" or
//		the warning "This is the wrong host" if the proc is not available
//		and/or the needed version is newer than the host's version.
//
//		returns TRUE				If the HandleProcs callback is available.
//		returns FALSE				If the HandleProcs callback is absent.
//
//-------------------------------------------------------------------------------

Boolean WarnHostHandleProcsAvailable (HandleProcs *procs,
									  Handle hDllInstance)
{
	Boolean newerVersion = FALSE;
	Boolean available = HostHandleProcsAvailable (procs, &newerVersion);
	
	if (!available)
	{
		Str255 	minVersion = "";
		short	errStringID = kNeedVers;
		
		// If we need a later version of the suite, pop a dialog
		// saying we have the wrong host (who *knows* what version):
		if (newerVersion)
			errStringID = kWrongHost;

		// Converts a double/float value to a string, with 1 digit of
		// decimal precision:
		DoubleToString(minVersion, 3.0, 1);
		
		// Use our routine to pop a dialog saying "Sorry, need host
		// version ^0 or better" where ^0 will be replaced by the
		// string we just created, or one saying "This is the
		// wrong host":
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  errStringID,
						  minVersion,
						  NULL);
	}
			
	return available;
	
} // end WarnHostHandleProcsAvailable

//-------------------------------------------------------------------------------
//
//	HostNewHandle
//	
//	Creates a new handle and returns its address.  If the HandleProcs are
//	unavailable, it will try the OS call to create the handle.
//
//	The HandleProcs are cross-platform master pointers that point to
//	pointers that point to data that is allocated in the Photoshop
//	virtual memory structure.  They're reference counted and
//	managed more efficiently than the operating system calls.
//
//	WARNING:  Do not mix operating system handle creation, deletion,
//			  and sizing routines with these callback routines.  They
//			  operate differently, allocate memory differently, and,
//			  while you won't crash, you can cause memory to be
//			  allocated on the global heap and never deallocated.
//
//	Inputs:
//		HandleProcs *proc			Pointer to HandleProcs callback.
//
//		const int32 inSize			Size of handle to allocate.
//
//	Outputs:
//		returns Handle				The handle will be valid if it was
//									able to create it, or NULL if not or
//									the HandleProcs were unavailable.
//
//-------------------------------------------------------------------------------

Handle HostNewHandle (HandleProcs *procs, const int32 inSize)
{
	
	if (HostHandleProcsAvailable (procs, NULL))
		return (*procs->newProc) (inSize);
	else
		return (Handle)(NewHandle (inSize)); // try OS routine

} // end HostNewHandle

//-------------------------------------------------------------------------------
//
//	HostDisposeHandle
//	
//	Disposes a handle allocated via the Handle suite. 
//
//	The HandleProcs are cross-platform master pointers that point to
//	pointers that point to data that is allocated in the Photoshop
//	virtual memory structure.  They're reference counted and
//	managed more efficiently than the operating system calls.
//
//	WARNING:  Do not mix operating system handle creation, deletion,
//			  and sizing routines with these callback routines.  They
//			  operate differently, allocate memory differently, and,
//			  while you won't crash, you can cause memory to be
//			  allocated on the global heap and never deallocated.
//
//	Inputs:
//		HandleProcs *proc			Pointer to HandleProcs callback.
//
//		Handle h					Handle to dispose.
//
//	Outputs:
//		Disposes the handle.  If the HandleProcs are unavailable,
//		it will try the OS routine.
//
//-------------------------------------------------------------------------------

void HostDisposeHandle (HandleProcs *procs, Handle h)
{
	
	if (HostHandleProcsAvailable (procs, NULL))
		(*procs->disposeProc) (h);
	else
		DisposeHandle (h); // try OS routine
	
}

//-------------------------------------------------------------------------------
//
//	HostGetHandleSize
//	
//	Returns the size of a handle. 
//
//	The HandleProcs are cross-platform master pointers that point to
//	pointers that point to data that is allocated in the Photoshop
//	virtual memory structure.  They're reference counted and
//	managed more efficiently than the operating system calls.
//
//	WARNING:  Do not mix operating system handle creation, deletion,
//			  and sizing routines with these callback routines.  They
//			  operate differently, allocate memory differently, and,
//			  while you won't crash, you can cause memory to be
//			  allocated on the global heap and never deallocated.
//
//	Inputs:
//		HandleProcs *proc			Pointer to HandleProcs callback.
//
//		Handle h					Handle to query.
//
//	Outputs:
//		returns int32				Size of handle.  If procs are not
//									available, will try to return it
//									with OS routine.
//
//-------------------------------------------------------------------------------

int32 HostGetHandleSize (HandleProcs *procs, Handle h)
{
	
	if (HostHandleProcsAvailable (procs, NULL))
		return (*procs->getSizeProc) (h);
	else
		return GetHandleSize (h); // try OS routine
	
} // end HostGetHandleSize

//-------------------------------------------------------------------------------
//
//	HostSetHandleSize
//	
//	Resizes a handle. 
//
//	The HandleProcs are cross-platform master pointers that point to
//	pointers that point to data that is allocated in the Photoshop
//	virtual memory structure.  They're reference counted and
//	managed more efficiently than the operating system calls.
//
//	WARNING:  Do not mix operating system handle creation, deletion,
//			  and sizing routines with these callback routines.  They
//			  operate differently, allocate memory differently, and,
//			  while you won't crash, you can cause memory to be
//			  allocated on the global heap and never deallocated.
//
//	Inputs:
//		HandleProcs *proc			Pointer to HandleProcs callback.
//
//		Handle h					Handle to resize.
//
//		const int32 inNewSize		New size of handle.
//
//	Outputs:
//		returns OSErr				noErr if able to resize, otherwise
//									whatever error occurred (if any).
//
//-------------------------------------------------------------------------------

OSErr HostSetHandleSize (HandleProcs *procs,
						 Handle h,
						 const int32 inNewSize)
{
	
	if (HostHandleProcsAvailable (procs, NULL))
	{	
		return (*procs->setSizeProc) (h, inNewSize);
	}
	else
	{ // try OS routine:
	
		SetHandleSize (h, inNewSize);
	
		#ifdef __PIMac__
			return MemError();
		#else
			return noErr;
		#endif
	}	
	
} // end HostSetHandleSize

//-------------------------------------------------------------------------------
//
//	HostLockHandle
//	
//	Locks a handle, returning a pointer to the data. 
//
//	The HandleProcs are cross-platform master pointers that point to
//	pointers that point to data that is allocated in the Photoshop
//	virtual memory structure.  They're reference counted and
//	managed more efficiently than the operating system calls.
//
//	WARNING:  Do not mix operating system handle creation, deletion,
//			  and sizing routines with these callback routines.  They
//			  operate differently, allocate memory differently, and,
//			  while you won't crash, you can cause memory to be
//			  allocated on the global heap and never deallocated.
//
//			  You probably can use the OS calls HLock and HUnlock
//			  with HandleProcs handles without a problem, but try
//			  not to if you can help it.
//
//	Inputs:
//		HandleProcs *proc			Pointer to HandleProcs callback.
//
//		Handle h					Handle to lock.
//
//		const Boolean inMoveHigh	Move high in memory before locking?
//
//	Outputs:
//		returns Ptr					Locks handle and returns a pointer
//									to its data, or NULL if unsuccessful.
//									It will drop to OS routines if
//									procs are unavailable.
//
//-------------------------------------------------------------------------------

Ptr HostLockHandle (HandleProcs *procs, Handle h, const Boolean inMoveHigh)
{
	if (HostHandleProcsAvailable (procs, NULL))
	{
		return (*procs->lockProc) (h, inMoveHigh);	
	}		
	else
	{ // Use OS routines:
	
		#ifdef __PIMac__
			if (inMoveHigh)
				MoveHHi (h);		
			HLock (h);
					
			return *h; // dereference and return pointer
		
		#else // WIndows
		
			return GlobalLock (h);

		#endif
	}

} // end HostLockHandle

//-------------------------------------------------------------------------------
//
//	HostUnlockHandle
//	
//	Unlocks a handle.
//
//	The HandleProcs are cross-platform master pointers that point to
//	pointers that point to data that is allocated in the Photoshop
//	virtual memory structure.  They're reference counted and
//	managed more efficiently than the operating system calls.
//
//	WARNING:  Do not mix operating system handle creation, deletion,
//			  and sizing routines with these callback routines.  They
//			  operate differently, allocate memory differently, and,
//			  while you won't crash, you can cause memory to be
//			  allocated on the global heap and never deallocated.
//
//			  You probably can use the OS calls HLock and HUnlock
//			  with HandleProcs handles without a problem, but try
//			  not to if you can help it.
//
//	Inputs:
//		HandleProcs *proc			Pointer to HandleProcs callback.
//
//		Handle h					Handle to unlock.
//
//	Outputs:
//		Unlocks the handle so it will be moved at next compaction.
//
//-------------------------------------------------------------------------------

void HostUnlockHandle (HandleProcs *procs, Handle h)
{
	if (HostHandleProcsAvailable (procs, NULL))
	{
		(*procs->unlockProc) (h);	
	}		
	else
	{ // Use OS routines:
		#ifdef __PIMac__
		
			HUnlock (h);
	
		#else // Windows

			GlobalUnlock (h);

		#endif
	}
	
} // end HostUnlockHandle

//-------------------------------------------------------------------------------
//
//	HostHandle2CString
//	
//	Copies a handle's contents to a C string.
//
//	The HandleProcs are cross-platform master pointers that point to
//	pointers that point to data that is allocated in the Photoshop
//	virtual memory structure.  They're reference counted and
//	managed more efficiently than the operating system calls.
//
//	Inputs:
//		HandleProcs *proc			Pointer to HandleProcs callback.
//
//		Handle inDataHandle			Handle to copy.
//
//		const size_t outMaxSize		Maximum size of buffer.
//
//	Outputs:
//		char *outString				Null terminated C string
//									with copy of handle.
//
//-------------------------------------------------------------------------------

void HostHandle2CString(HandleProcs *procs,
					   Handle inDataHandle, 
					   char *outString,
					   const size_t outMaxSize)
{
	PIResetString((unsigned char *)outString); // macro to clear a string. See header file.
	
	if (inDataHandle != NULL)
	{ // inDataHandle is valid.
	
		Ptr p = HostLockHandle(procs, inDataHandle, FALSE);
		
		if (p != NULL)
		{ // Got a valid pointer.
		
			size_t size = HostGetHandleSize(procs, inDataHandle);
			
			if (size >= outMaxSize-1)
				size = outMaxSize-1; // Have to pin to maximum string length.
			
			// Append handle to string:
			PICopy(outString, p, size);
			outString[size+1] = '\0';

		} // p
		
		HostUnlockHandle(procs, inDataHandle); // Done copying.  Let it move.
		
	} // inDataHandle
	
} // end HostHandle2CString

//-------------------------------------------------------------------------------
//
//	HostHandle2PString
//	
//	Copies a handle's contents to a pascal string with a maximum length of 255.
//
//	The HandleProcs are cross-platform master pointers that point to
//	pointers that point to data that is allocated in the Photoshop
//	virtual memory structure.  They're reference counted and
//	managed more efficiently than the operating system calls.
//
//	Inputs:
//		HandleProcs *proc			Pointer to HandleProcs callback.
//
//		Handle inDataHandle			Handle to copy.
//
//	Outputs:
//		Str255 outString			Pascal string to receive data.
//
//-------------------------------------------------------------------------------

void HostHandle2PString(HandleProcs *procs,
					    Handle inDataHandle, 
					    Str255 outString)
{
	PIResetString(outString); // macro to clear a string. See header file.
	
	if (inDataHandle != NULL)
	{ // inDataHandle is valid.
	
		Ptr p = HostLockHandle(procs, inDataHandle, FALSE);
		
		if (p != NULL)
		{ // Got a valid pointer.
		
			size_t size = HostGetHandleSize(procs, inDataHandle);
			
			if (size > 254)
				size = 254; // Have to pin to maximum string length.
			
			// Append handle to string:
			AppendString(outString, (unsigned char *)p, 0, (short)size);
		
		} // p
		
		HostUnlockHandle(procs, inDataHandle); // Done copying.  Let it move.
		
	} // inDataHandle
	
} // end HostHandle2PString
	
//-------------------------------------------------------------------------------
//
//	HostCString2Handle
//	
//	Copies a null-terminated C string's contents to a new handle.
//
//	The HandleProcs are cross-platform master pointers that point to
//	pointers that point to data that is allocated in the Photoshop
//	virtual memory structure.  They're reference counted and
//	managed more efficiently than the operating system calls.
//
//	Inputs:
//		HandleProcs *proc			Pointer to HandleProcs callback.
//
//		const char *inString		C string with source data.
//
//	Outputs:
//		returns Handle 				New handle with copy of string,
//									NULL if unable to create.
//
//-------------------------------------------------------------------------------

Handle HostCString2Handle(HandleProcs *procs, const char *inString)
{
	const size_t len = PIstrlen(inString);
	Handle	h = HostNewHandle(procs, len);
	
	if (h != NULL)
	{ // Succeeded in creating the handle.  Now copy the data:

		Ptr p = HostLockHandle(procs, h, FALSE);

		// If we created a valid pointer, use our copy routine
		// to transfer data of a specified length (s[0]):
		if (p != NULL)
		{
			HostCopy(p, inString, len);
			
			HostUnlockHandle(procs, h); // Release so it can move.
		}
	}

	return h; // Handle we created or NULL

} // end HostCString2Handle

//-------------------------------------------------------------------------------
//
//	HostPString2Handle
//	
//	Copies a pascal string's contents to a new handle.
//
//	The HandleProcs are cross-platform master pointers that point to
//	pointers that point to data that is allocated in the Photoshop
//	virtual memory structure.  They're reference counted and
//	managed more efficiently than the operating system calls.
//
//	Inputs:
//		HandleProcs *proc			Pointer to HandleProcs callback.
//
//		const Str255 inString		Pascal string with source data.
//
//	Outputs:
//		returns Handle 				New handle with copy of string,
//									NULL if unable to create.
//
//-------------------------------------------------------------------------------

Handle HostPString2Handle(HandleProcs *procs, const Str255 inString)
{
	Handle	h = HostNewHandle(procs, inString[0]); // [0] is length
	
	if (h != NULL)
	{ // Succeeded in creating the handle.  Now copy the data:

		Ptr p = HostLockHandle(procs, h, FALSE);

		// If we created a valid pointer, use our copy routine
		// to transfer data of a specified length (s[0]):
		if (p != NULL)
		{
			HostCopy(p, &inString[1], inString[0]);
			
			HostUnlockHandle(procs, h); // Release so it can move.
		}
	}

	return h; // Handle we created or NULL

} // end HostPString2Handle

//-------------------------------------------------------------------------------
//
//	HostHandleCat
//	
//	Concatinates two handles, creating a copy, if necessary.
//
//	The HandleProcs are cross-platform master pointers that point to
//	pointers that point to data that is allocated in the Photoshop
//	virtual memory structure.  They're reference counted and
//	managed more efficiently than the operating system calls.
//
//	Inputs:
//		HandleProcs *proc			Pointer to HandleProcs callback.
//
//		Handle *outHandle			Handle to concatenate inHandle
//									to.  If this is NULL, the routine
//									will create a new handle.
//
//		const Handle inHandle		Data to concatenate onto outHandle.
//		
//	Outputs:
//		returns OSErr				Error checking is pretty loose
//									on this one, since it will create
//									the handle, if necessary.
//
//-------------------------------------------------------------------------------

OSErr HostHandleCat(HandleProcs *procs,
					 Handle *outHandle,
					 const Handle inHandle)
{
	OSErr err = noErr;
	
	const int32	inHandleSize = HostGetHandleSize(procs, inHandle);
	
	if (inHandleSize > 0)
	{ // Don't bother doing this if there's no data in there.
		int32 outHandleSize = 0;
	
		if (outHandle == NULL)
		{ // We're expecting a valid handle, and we didn't even get
		  // that.  Return an error.
		  err = paramErr;
		}
		else
		{
			if (*outHandle == NULL)
			{ // outHandle needs to be created:
				*outHandle = HostNewHandle(procs, inHandleSize);
			}
			else
			{ // outHandle exists.  Grow handle:
				outHandleSize = HostGetHandleSize(procs, *outHandle);
				HostSetHandleSize(procs, *outHandle, outHandleSize + inHandleSize);
			}
		
			if (*outHandle != NULL)
			{ // We either just created a valid handle or already had one:
					
				Ptr outHandlePtr = HostLockHandle(procs, *outHandle, true);
				const Ptr inHandlePtr = HostLockHandle(procs, inHandle, true);
				
				if (outHandlePtr != NULL && inHandlePtr != NULL)
				{ // Have valid pointers.  Copy to end:
					HostCopy(&outHandlePtr[outHandleSize],
							 inHandlePtr,
							 inHandleSize);
				}
				
				// Unlock the handles so they can move:
				HostUnlockHandle(procs, *outHandle);
				HostUnlockHandle(procs, inHandle);

			} // *outHandle

		} // outHandle

	} // inHandleSize

	return err;

} // end HostHandleCat

//-------------------------------------------------------------------------------
//
//	HostHandleCopy
//	
//	Copies a handle.
//
//	The HandleProcs are cross-platform master pointers that point to
//	pointers that point to data that is allocated in the Photoshop
//	virtual memory structure.  They're reference counted and
//	managed more efficiently than the operating system calls.
//
//	Inputs:
//		HandleProcs *proc			Pointer to HandleProcs callback.
//
//		Handle *outHandle			Handle to copy inHandle
//									to.  If this is NULL, the routine
//									will create a new handle.
//
//		const Handle inHandle		Data to concatenate onto outHandle.
//		
//	Outputs:
//		returns OSErr				Error checking is pretty loose
//									on this one, since it will create
//									the handle, if necessary.
//
//-------------------------------------------------------------------------------

OSErr HostHandleCopy(HandleProcs *procs,
					 Handle *outHandle,
					 const Handle inHandle)
{
	OSErr err = noErr;
	
	// We're expecting outHandle to point to a valid Handle variable.
	if (outHandle == NULL)
	{
		err = paramErr;
	}	
	else
	{
		// We're expecting a NULL outHandle that we can fill.  If it's not null,
		// there's a danger in leaking memory so we'll just return an error:
		if (*outHandle != NULL)
		{
			err = paramErr;
		}
		else
		{
			err = HostHandleCat(procs, outHandle, inHandle);
		}
	}
	
	return err;
	
} // end HostHandleCopy

//-------------------------------------------------------------------------------
//
//	HostGetString
//	
//	Uses the generic PIGetResource to grab a resource and stuff it
//  into a pascal string.  The resource will be truncated if over
//  255 characters.  Cross-platform as long as the same String
//	resource exists with the same Resource ID on both platforms'
//	versions.
//
//	Inputs:
//		Handle hDllinstance			Windows DLL instance pointer.
//									NULL on Macintosh.
//
//		const int32 inResourceID	String resource ID to grab data from.
//
//		StringResource				#define for resource type 'STR '.
//
//	Outputs:
//		Str255 outString			Pascal string copy.
//
//-------------------------------------------------------------------------------
 
void HostGetString (HandleProcs *procs,
					Handle hDllInstance,
					const int32 inResourceID,
					Str255 outString)
{
	int32	size = 0;
	Handle	h = HostGetResource(procs, hDllInstance, StringResource, inResourceID, &size);
	
	PIResetString(outString); // macro to clear string

	if (h != NULL)
	{ // Was able to retrieve resource.  The pointer points to a pascal
	  // string, so skip the first length byte and copy the rest:
		
		if (size > 1)
		{
			Ptr p = HostLockHandle(procs, h, TRUE); // move high
			if (p != NULL)
			{
				HostCopy(outString, p, size);
			}

			HostUnlockHandle(procs, h);
			p = NULL; // just in case

		}
		
		HostDisposeHandle(procs, h);
		h = NULL;
			
	} // h

} // end HostGetString

//-------------------------------------------------------------------------------
//
//	PIType2Char
//	
//	Converts an uint32 ResType to its corresponding 4 characters.
//
//	Inputs:
//		const ResType inType		uint32 ResType to convert.
//
//	Outputs:
//		char *outChars				4 characters representing the ResType.
//									Make sure your pointer points to
//									an array of at least 4 characters or
//									this will stomp a little memory (4 bytes).
//
//-------------------------------------------------------------------------------

void PIType2Char (const ResType inType, char *outChars)
{
	int8 loopType, loopChar;
	
	// Figure out which byte we're dealing with and move it
	// to the highest character, then mask out everything but
	// the lowest byte and assign that as the character:
	for (loopType = 0, loopChar = 3; loopType < 4; loopType++, loopChar--)
		outChars[loopChar] = (char) ( (inType >> (8*loopType)) & 0xFF );
	
	outChars[loopType] = '\0';

} // end PIType2Char

//-------------------------------------------------------------------------------
//
//	PIChar2Type
//	
//	Convert 4 characters to a corresponding uint32 ResType.
//
//	Inputs:
//		const char *inChars			Pointer to 4 characters to convert.
//
//	Outputs:
//		returns ResType				Unsigned int32 (long) corresponding
//									to the 4 characters.
//
//-------------------------------------------------------------------------------

ResType PIChar2Type (const char *inChars)
{
	unsigned short loop;
	ResType outType = 0;
	
	for (loop = 0; loop < 4; loop++)
	{
		outType <<= 8; // bits per char
		outType |= inChars[loop];
	}

	return outType;

} // end PIChar2Type

//-------------------------------------------------------------------------------
//
//	HostGetResource
//	
//	Reads a resource from a Windows or Macintosh plug-in.
//
//	Inputs:
//		Handle hDllInstance			Windows DLL handle.
//									NULL on Macintosh.
//
//		const ResType inType		4-character resource type.
//
//		const int32 inRezID			ID of resource.
//
//	Outputs:
//		int32 *outSize				Set to size of resource read,
//									0 if not found or empty.
//
//		returns Ptr					Returns a pointer to the locked memory.
//
//-------------------------------------------------------------------------------

Handle	HostGetResource (HandleProcs *procs,
						 Handle hDllInstance,
					 	 const ResType inType, 
					 	 const int32 inRezID,
					 	 int32 *outSize)
{
	Handle			h2 = NULL; // HGLOBAL Handle h;
	Handle			h1 = NULL;	// HRSRC Handle hr;
	Handle			h = NULL;
	short			x = 3;
	Ptr				p = NULL, p1 = NULL, p2 = NULL; // LPVOID Ptr hPtr;
	char			name[5] = "\0\0\0\0";
	
	*outSize = 0;
#ifdef __PIWin__
	if (inType == StringResource)
	{
		size_t size = 0;
		char buff[kStr255InitLen] = "";
		
		size = LoadString(hDllInstance, inRezID, buff, kStr255MaxLen);
		if (size > 0)
		{ // got something
			h = HostNewHandle(procs, size+2);
			// Add one for pascal length byte,
			// add another for trailing null.
			if (h != NULL)
			{
				p = HostLockHandle(procs, h, TRUE); // moveHigh
				if (p != NULL)
				{
					HostCopy(p+1, buff, size);
					p[0] = (char)(size); // pascal length

					size++;
					p[size] = '\0'; // trailing null
					size++; // add one to include length

					HostUnlockHandle(procs, h);
				} // p
			} // h
		} // *outSize
		*outSize = size;
	}
	else
	{
		// Windows requires us to pass a string for the id, but the string
		// can be a number if it starts with "#". So we create a
		// string with only "#", convert the int32 to a string,
		// and concatinate that string onto our "#" string:
		char winID [kStr255InitLen] = "#\0";
		char buff[33];
		
		ltoa (inRezID, buff, 10);
		HostStringCat(winID, buff);
		PIType2Char(inType, name);
		
		h2 = FindResource(hDllInstance, winID, name);
		if (h2 != NULL)
		{
			size_t size = 0;
			h1 = LoadResource(hDllInstance, h2);
			size = SizeofResource(hDllInstance, h2);
			if (size > 0)
			{ // got something
				h = HostNewHandle(procs, size);
				if (h != NULL)
				{
					p = HostLockHandle(procs, h, TRUE); // moveHigh
					if (p != NULL)
					{
						p1 = LockResource(h1);
						if (p1 != NULL)
						{
							HostCopy(p, p1, size);
							UnlockResource(h1);
							p1 = NULL;
						} // p1
						HostUnlockHandle(procs, h);
						p = NULL;
					} // p
				} // h
			} // *outSize
			
			FreeResource(h1);
			h1 = NULL;
			h2 = NULL; // done
			*outSize = size;
		} // h2
	}
#elif defined(__PIMac__)
	#ifdef __PIMWCW__
		#pragma unused (hDllInstance)
	#endif
	
	h2 = GetResource(inType, inRezID);
	if (h2 != NULL)
	{
		*outSize = GetHandleSize(h2);
		
		if (*outSize > 0)
		{ // got something.  Do something with it.
			HLock(h2); // lock
			p2 = *h2; // pointer
		
			h = HostNewHandle(procs, *outSize);
			if (h != NULL)
			{
				p = HostLockHandle(procs, h, TRUE);
				HostCopy(p, p2, *outSize);
				HostUnlockHandle(procs, h);
			} // h
			HUnlock(h2);
			p2 = NULL;
		} // *outSize
		HPurge(h2);
		ReleaseResource(h2);
		h2 = NULL;
	} // h2
#endif
	return h;
} // end HostGetResource

//-------------------------------------------------------------------------------
//
//	HostPropertyAvailable
//	
//	Determines whether the Property suite of callbacks is available.
//
//	The Property suite callbacks are two callbacks, GetProperty and
//	SetProperty, that manage a list of different data elements.  See
//	PIProperties.h.
//
//	Inputs:
//		PropertyProcs *procs		Pointer to Property suite callbacks.
//
//	Outputs:
//		Boolean *outNewerVersion	TRUE if the host has a newer version
//									of the procs than the plug-in.
//									FALSE if the host has the same version
//									of the procs as the plug-in.
//
//		returns TRUE				If the Property suite is available.
//		returns FALSE				If the Property suite is absent.
//
//-------------------------------------------------------------------------------

Boolean HostPropertyAvailable (PropertyProcs *procs, Boolean *outNewerVersion)
{
	
	Boolean available = TRUE;		// assume procs are available
	Boolean newerVersion = FALSE;	// assume we're running under correct version
	
	// We want to check for this stuff in a logical order, going from things
	// that should always be present to things that "may" be present.  It's
	// always a danger checking things that "may" be present because some
	// hosts may not leave them NULL if unavailable, instead pointing to
	// other structures to save space.  So first we'll check the main
	// proc pointer, then the version, the number of routines, then some
	// of the actual routines:
	
	if (procs == NULL)
	{
		available = FALSE;
	}	
	else if (procs->propertyProcsVersion < kCurrentPropertyProcsVersion)
	{
		available = FALSE;
	}	
	else if (procs->propertyProcsVersion > kCurrentPropertyProcsVersion)
	{
		available = FALSE;
		newerVersion = TRUE;
	}
	else if (procs->numPropertyProcs < kCurrentPropertyProcsCount)
	{
		available = FALSE;
	}	
	else if (procs->getPropertyProc == NULL ||
			 procs->setPropertyProc == NULL)
	{
		available = FALSE;
	}
		
	if (newerVersion && (outNewerVersion != NULL))
		*outNewerVersion = newerVersion;
		
	return available;
	
} // end HostPropertyAvailable

//-------------------------------------------------------------------------------
//
//	WarnHostPropertyAvailable
//	
//	Shows an alert if the PropertyProcs callback is not available.
//
//	The Property suite callbacks are two callbacks, GetProperty and
//	SetProperty, that manage a list of different data elements.  See
//	PIProperties.h.
//
//	Inputs:
//		PropertyProcs *procs		Pointer to PropertyProcs callback.
//
//		Handle hDllInstance			Windows DLLInstance global handle.
//
//		AlertID						#define with the ID of the "Need host"
//									alert.  See header file.
//
//		kNeedVers					#define with the ID of the "Need host ^0"
//									string.  See header file.
//
//		kWrongHost					#define with the ID of the "Wrong host"
//									string.  See header file.
//
//	Outputs:
//		Pops a dialog with the warning "Need at least version 2.5" or
//		the warning "This is the wrong host" if the proc is not available
//		and/or the needed version is newer than the host's version.
//
//		returns TRUE				If the PropertyProcs callback is available.
//		returns FALSE				If the PropertyProcs callback is absent.
//
//-------------------------------------------------------------------------------

Boolean WarnHostPropertyAvailable (PropertyProcs *procs,
								   Handle hDllInstance)
{
	Boolean newerVersion = FALSE;
	Boolean available = HostPropertyAvailable (procs, &newerVersion);
	
	if (!available)
	{
		Str255 	minVersion = "";
		short	errStringID = kNeedVers;
		
		// If we need a later version of the suite, pop a dialog
		// saying we have the wrong host (who *knows* what version):
		if (newerVersion)
			errStringID = kWrongHost;

		// Converts a double/float value to a string, with 1 digit of
		// decimal precision:
		DoubleToString(minVersion, 2.5, 1);
		
		// Use our routine to pop a dialog saying "Sorry, need host
		// version ^0 or better" where ^0 will be replaced by the
		// string we just created, or one saying "This is the
		// wrong host":
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  errStringID,
						  minVersion,
						  NULL);
	}
			
	return available;
	
} // end WarnHostPropertyProcsAvailable

//-------------------------------------------------------------------------------
//
//	HostChannelPortAvailable
//	
//	Determines whether the ChannelPortProcs callback is available.
//
//	The Channel Port Procs are callbacks designed to simplify
//	merged image and target access, with built in scaling.
//	They're used extensively by Selection modules.
//
//	Inputs:
//		ChannelPortProcs *procs		Pointer to ChannelPort callbacks.
//
//	Outputs:
//		Boolean *outNewerVersion	TRUE if the host has a newer version
//									of the procs than the plug-in.
//									FALSE if the host has the same version
//									of the procs as the plug-in.
//
//		returns TRUE				If ChannelPortProcs is available.
//		returns FALSE				If ChannelPortProcs is absent.
//
//-------------------------------------------------------------------------------

Boolean HostChannelPortAvailable (ChannelPortProcs *procs, Boolean *outNewerVersion)
{
	
	Boolean available = TRUE;		// assume procs are available
	Boolean newerVersion = FALSE;	// assume we're running under correct version
	
	// We want to check for this stuff in a logical order, going from things
	// that should always be present to things that "may" be present.  It's
	// always a danger checking things that "may" be present because some
	// hosts may not leave them NULL if unavailable, instead pointing to
	// other structures to save space.  So first we'll check the main
	// proc pointer, then the version, the number of routines, then some
	// of the actual routines:

	if (procs == NULL)
	{
		available = FALSE;
	}	
	else if (procs->channelPortProcsVersion < kCurrentChannelPortProcsVersion)
	{
		available = FALSE;
	}	
	else if (procs->channelPortProcsVersion > kCurrentChannelPortProcsVersion)
	{	
		available = FALSE;	
		newerVersion = TRUE;
	}
		
	else if (procs->numChannelPortProcs < kCurrentChannelPortProcsCount)
	{
		available = FALSE;
	}
	else if (procs->readPixelsProc == NULL ||
			 procs->writeBasePixelsProc == NULL ||
			 procs->readPortForWritePortProc == NULL)
	{
		available = FALSE;
	}	
	
	if (newerVersion && outNewerVersion != NULL)
		*outNewerVersion = newerVersion;
		
	return available;
	
} // end HostChannelPortAvailable

//-------------------------------------------------------------------------------
//
//	WarnHostChannelPortAvailable
//	
//	Shows an alert if the ChannelPortProcs callback
//	is not available.
//
//	The Channel Port Procs are callbacks designed to simplify
//	merged image and target access, with built in scaling.
//	They're used extensively by Selection modules.
//
//	Inputs:
//		ChannelPortProcs *procs		Pointer to ChannelPort callback.
//
//		Handle hDllInstance			Windows DLLInstance global handle.
//
//		AlertID						#define with the ID of the "Need host"
//									alert.  See header file.
//
//		kNeedVers					#define with the ID of the "Need host ^0"
//									string.  See header file.
//
//		kWrongHost					#define with the ID of the "Wrong host"
//									string.  See header file.
//
//	Outputs:
//		Pops a dialog with the warning "Need at least version 2.5" or
//		the warning "This is the wrong host" if the proc is not available
//		and/or the needed version is newer than the host's version.
//
//		returns TRUE				If the ResourceProcs callback is available.
//		returns FALSE				If the ResourceProcs callback is absent.
//
//-------------------------------------------------------------------------------

Boolean WarnHostChannelPortAvailable (ChannelPortProcs *procs,
									  Handle hDllInstance)
{
	
	Boolean newerVersion = FALSE;
	Boolean available = HostChannelPortAvailable (procs, &newerVersion);
	
	if (!available)
	{
		Str255 	minVersion = "";
		short	errStringID = kNeedVers;
		
		// If we need a later version of the suite, pop a dialog
		// saying we have the wrong host (who *knows* what version):
		if (newerVersion)
			errStringID = kWrongHost;

		// Converts a double/float value to a string, with 1 digit of
		// decimal precision:
		DoubleToString(minVersion, 4.0, 1);
		
		// Use our routine to pop a dialog saying "Sorry, need host
		// version ^0 or better" where ^0 will be replaced by the
		// string we just created, or one saying "This is the
		// wrong host":
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  errStringID,
						  minVersion,
						  NULL);
	}
			
	return available;
	
} // end WarnHostChannelPortAvailable

//-------------------------------------------------------------------------------
//
//	HostDescriptorAvailable
//	
//	Determines whether the PIDescriptorParameters callback is available.
//
//	The Descriptor Parameters suite are callbacks designed for
//	scripting and automation.  See PIActions.h.
//
//	Inputs:
//		PIDescriptorParameters *procs	Pointer to Descriptor Parameters suite.
//
//	Outputs:
//		Boolean *outNewerVersion		TRUE if the host has a newer version
//										of the procs than the plug-in.
//										FALSE if the host has the same version
//										of the procs as the plug-in.
//
//		returns TRUE					If PIDescriptorParameters is available.
//		returns FALSE					If PIDescriptorParameters is absent.
//
//-------------------------------------------------------------------------------

Boolean HostDescriptorAvailable (PIDescriptorParameters *procs,
								 Boolean *outNewerVersion)
{
	
	Boolean available = TRUE;		// assume procs are available
	Boolean newerVersion = FALSE;	// assume we're running under correct version
	
	// We want to check for this stuff in a logical order, going from things
	// that should always be present to things that "may" be present.  It's
	// always a danger checking things that "may" be present because some
	// hosts may not leave them NULL if unavailable, instead pointing to
	// other structures to save space.  So first we'll check the main
	// proc pointer, then the version, the number of routines, then some
	// of the actual routines:
		
	if (procs == NULL)
	{
		available = FALSE;
	}	
	else if (procs->descriptorParametersVersion < 
			 kCurrentDescriptorParametersVersion)
	{
		available = FALSE;
	}	
	else if (procs->descriptorParametersVersion >
			 kCurrentDescriptorParametersVersion)
	{	
		available = FALSE;	
		newerVersion = TRUE;
	}	
	else if (procs->readDescriptorProcs == NULL || 
			 procs->writeDescriptorProcs == NULL)
	{
		available = FALSE;
	}
	else if (procs->readDescriptorProcs->readDescriptorProcsVersion
			 < kCurrentReadDescriptorProcsVersion)
	{
		available = FALSE;
	}
	else if (procs->readDescriptorProcs->readDescriptorProcsVersion
			 > kCurrentReadDescriptorProcsVersion)
	{
		available = FALSE;
		newerVersion = TRUE;
	}
	else if (procs->readDescriptorProcs->numReadDescriptorProcs
			 < kCurrentReadDescriptorProcsCount)
	{
		available = FALSE;
	}
	else if (procs->writeDescriptorProcs->writeDescriptorProcsVersion
			 < kCurrentWriteDescriptorProcsVersion)
	{
		available = FALSE;
	}
	else if (procs->writeDescriptorProcs->writeDescriptorProcsVersion
			 > kCurrentWriteDescriptorProcsVersion)
	{
		available = FALSE;
		newerVersion = TRUE;
	}
	else if (procs->writeDescriptorProcs->numWriteDescriptorProcs
			 < kCurrentWriteDescriptorProcsCount)
	{
		available = FALSE;
	}
	else if (procs->readDescriptorProcs->openReadDescriptorProc == NULL ||
			 procs->readDescriptorProcs->closeReadDescriptorProc == NULL ||
			 procs->readDescriptorProcs->getKeyProc == NULL ||
			 procs->readDescriptorProcs->getIntegerProc == NULL ||
			 procs->readDescriptorProcs->getFloatProc == NULL ||
			 procs->readDescriptorProcs->getUnitFloatProc == NULL ||
			 procs->readDescriptorProcs->getBooleanProc == NULL ||
			 procs->readDescriptorProcs->getTextProc == NULL ||
			 procs->readDescriptorProcs->getAliasProc == NULL ||
			 procs->readDescriptorProcs->getEnumeratedProc == NULL ||
			 procs->readDescriptorProcs->getClassProc == NULL ||
			 procs->readDescriptorProcs->getSimpleReferenceProc == NULL ||	
			 procs->readDescriptorProcs->getObjectProc == NULL ||
			 procs->readDescriptorProcs->getCountProc == NULL ||
			 procs->readDescriptorProcs->getStringProc == NULL ||
			 procs->readDescriptorProcs->getPinnedIntegerProc == NULL ||
			 procs->readDescriptorProcs->getPinnedFloatProc == NULL ||
			 procs->readDescriptorProcs->getPinnedUnitFloatProc == NULL)
	{
		available = FALSE;
	}	
	else if (procs->writeDescriptorProcs->openWriteDescriptorProc == NULL ||
			 procs->writeDescriptorProcs->closeWriteDescriptorProc == NULL ||
			 procs->writeDescriptorProcs->putIntegerProc == NULL ||
			 procs->writeDescriptorProcs->putFloatProc == NULL ||
			 procs->writeDescriptorProcs->putUnitFloatProc == NULL ||
			 procs->writeDescriptorProcs->putBooleanProc == NULL ||
			 procs->writeDescriptorProcs->putTextProc == NULL ||
			 procs->writeDescriptorProcs->putAliasProc == NULL ||
			 procs->writeDescriptorProcs->putEnumeratedProc == NULL ||
			 procs->writeDescriptorProcs->putClassProc == NULL ||
			 procs->writeDescriptorProcs->putSimpleReferenceProc == NULL ||	
			 procs->writeDescriptorProcs->putObjectProc == NULL ||
			 procs->writeDescriptorProcs->putCountProc == NULL ||
			 procs->writeDescriptorProcs->putStringProc == NULL ||
			 procs->writeDescriptorProcs->putScopedClassProc == NULL ||
			 procs->writeDescriptorProcs->putScopedObjectProc == NULL)

	if (newerVersion && (outNewerVersion != NULL))
		*outNewerVersion = newerVersion;
		
	return available;
	
} // end HostDescriptorAvailable

//-------------------------------------------------------------------------------
//
//	WarnHostDescriptorAvailable
//	
//	Shows an alert if the PIDescriptorParameters callback
//	is not available.
//
//	The Descriptor Parameters suite are callbacks designed for
//	scripting and automation.  See PIActions.h.
//
//	Inputs:
//		PIDescriptorParameters *procs	Pointer to Descriptor Parameters suite.
//
//		Handle hDllInstance				Windows DLLInstance global handle.
//
//		AlertID							#define with the ID of the "Need host"
//										alert.  See header file.
//
//		kNeedVers						#define with the ID of the "Need host ^0"
//										string.  See header file.
//
//		kWrongHost						#define with the ID of the "Wrong host"
//										string.  See header file.
//
//	Outputs:
//		Pops a dialog with the warning "Need at least version 2.5" or
//		the warning "This is the wrong host" if the proc is not available
//		and/or the needed version is newer than the host's version.
//
//		returns TRUE					If PIDescriptorParameters is available.
//		returns FALSE					If PIDescriptorParameters is absent.
//
//-------------------------------------------------------------------------------

Boolean WarnHostDescriptorAvailable (PIDescriptorParameters *procs,
									 Handle hDllInstance)
{
	Boolean newerVersion = FALSE;
	Boolean available = HostDescriptorAvailable (procs, &newerVersion);
	
	if (!available)
	{
		Str255 	minVersion = "";
		short	errStringID = kNeedVers;
		
		// If we need a later version of the suite, pop a dialog
		// saying we have the wrong host (who *knows* what version):
		if (newerVersion)
			errStringID = kWrongHost;

		// Converts a double/float value to a string, with 1 digit of
		// decimal precision:
		DoubleToString(minVersion, 4.0, 1);
		
		// Use our routine to pop a dialog saying "Sorry, need host
		// version ^0 or better" where ^0 will be replaced by the
		// string we just created, or one saying "This is the
		// wrong host":
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  errStringID,
						  minVersion,
						  NULL);
	}
			
	return available;
	
} // end WarnHostDescriptorAvailable

//-------------------------------------------------------------------------------
//
//	HostCloseReader
//	
//	Closes a read token, disposes its handle, sets the token to NULL, and
//	sets the parameter blocks' descriptor to NULL.
//
//	The Descriptor Parameters suite are callbacks designed for
//	scripting and automation.  See PIActions.h.
//
//	Inputs:
//		PIDescriptorParameters *procs	Pointer to Descriptor Parameters suite.
//
//		HandleProcs *hProcs				Pointer to HandleProcs callback.
//
//		PIReadDescriptor *token			Pointer to token to close.
//
//		procs->descriptor				Pointer to original read handle.
//
//	Outputs:
//		PIReadDescriptor *token			Set to NULL.
//
//		procs->descriptor				Disposed then set to NULL.
//
//		returns OSErr					noErr or error if one occurred.
//
//-------------------------------------------------------------------------------

OSErr HostCloseReader (PIDescriptorParameters *procs,
					   HandleProcs *hProcs,
					   PIReadDescriptor *token)
{
	// Close token:
	OSErr		err = procs->readDescriptorProcs->closeReadDescriptorProc(*token);
	
	// Dispose the parameter block descriptor:
	HostDisposeHandle(hProcs, procs->descriptor);
	
	// Set the descriptor and the read token to NULL:
	procs->descriptor = NULL;
	*token = NULL;
	
	return err;

} // end HostCloseReader

//-------------------------------------------------------------------------------
//
//	HostCloseWriter
//	
//	Closes a write token, stores its handle in the global parameter block for
//	the host to use, sets the token to NULL, and sets the recordInfo to 
//	plugInDialogOptional (the default).
//
//	The Descriptor Parameters suite are callbacks designed for
//	scripting and automation.  See PIActions.h.
//
//	Inputs:
//		PIDescriptorParameters *procs	Pointer to Descriptor Parameters suite.
//
//		HandleProcs *hProcs				Pointer to HandleProcs callback.
//
//		PIWriteDescriptor *token		Pointer to token to close and pass on.
//
//		procs->descriptor				Should be NULL.  If not, its contents
//										will be disposed and replaced.
//
//	Outputs:
//		PIWriteDescriptor *token		Set to NULL.
//
//		procs->descriptor				Set to descriptor handle.
//
//		returns OSErr					noErr or error if one occurred.
//
//-------------------------------------------------------------------------------

OSErr	HostCloseWriter(PIDescriptorParameters *procs,
						HandleProcs *hProcs,
						PIWriteDescriptor *token)
{
	OSErr				err = noErr; // assume no error
	PIDescriptorHandle	h = NULL;
	
	if (procs->descriptor != NULL) // don't need descriptor passed to us
		HostDisposeHandle(hProcs, procs->descriptor); // dispose.
	procs->writeDescriptorProcs->closeWriteDescriptorProc(*token, &h);
	procs->descriptor = h;
	
	// Set recordInfo to default.  Options are: plugInDialogOptional,
	// plugInDialogRequire, plugInDialogNone:
	procs->recordInfo = plugInDialogOptional;

	*token = NULL;
	
	return err;
	
} // end HostCloseWriter

//-------------------------------------------------------------------------------
//
//	HostPutObj
//	
//	Adds an object into an open descriptor, disposes its handle and sets the
//	handle to NULL.
//
//	The Descriptor Parameters suite are callbacks designed for
//	scripting and automation.  See PIActions.h.
//
//	Inputs:
//		PIDescriptorParameters *procs	Pointer to Descriptor Parameters suite.
//
//		HandleProcs *hProcs				Pointer to HandleProcs callback.
//
//		PIWriteDescriptor token			Pointer to token to write object to.
//
//		DescriptorKeyID	key				Unsigned int32 key for object.
//
//		DescriptorTypeID type			Unsigned int32 type for object.
//
//		PIDescriptorHandle *h			Handle to descriptor object that
//										will be writen into the open token.
//
//	Outputs:
//		PIWriteDescriptor *token		Has the object *h added to it.
//
//		PIDescriptorHandle *h			Written, then, if noErr,
//										disposed and set to NULL.
//
//		returns OSErr					noErr or error if one occurred.
//
//-------------------------------------------------------------------------------

OSErr HostPutObj (PIDescriptorParameters *procs,
				  HandleProcs *hProcs, 
				  PIWriteDescriptor token,
				  DescriptorKeyID key,
				  DescriptorTypeID type,
				  PIDescriptorHandle *h)
{
	OSErr		err = noErr;
	
	err = procs->writeDescriptorProcs->putObjectProc(token, key, type, *h);

	if (err == noErr)
	{
		HostDisposeHandle(hProcs, *h);
		*h = NULL;
	}
	
	return err;
	
} // end HostPutObj

//-------------------------------------------------------------------------------
//
//	HostPlayDialog
//	
//	Interrogates playInfo and returns whether host has told plug-in to pop
//	its dialog.
//
//	The Descriptor Parameters suite are callbacks designed for
//	scripting and automation.  See PIActions.h.
//
//	Inputs:
//		PIDescriptorParameters *procs	Pointer to Descriptor Parameters suite.
//
//	Outputs:
//		returns Boolean					TRUE if you should pop your dialog.
//										FALSE if not.
//
//-------------------------------------------------------------------------------

Boolean	HostPlayDialog(PIDescriptorParameters *procs)
{
	return procs->playInfo == plugInDialogDisplay;

	// Other valid values for playInfo:
	// plugInDialogDontDisplay = display only if necessary,
	// plugInDialogDisplay = display your dialog,
	// plugInDialogSilent = never display your dialog

} // end HostPlayDialog


//-------------------------------------------------------------------------------
//
//	DivideAtDecimal
//	
//	Finds the '.' or ',' in a string and catinates it at that point,
//	puts all the data after the mark in a second string.
//
//	Inputs:
//		Str255 ioWhole					Pascal string to parse.
//
//		Str255 outDecimal				Pascal string to receive decimal
//										portion.
//
//	Outputs:
//		Str255 ioWhole					Length set to whole portion
//										(left side of decimal).
//
//		Str255 outDecimal				Decimal portion of string
//										(right side of decimal).
//
//-------------------------------------------------------------------------------

void DivideAtDecimal(Str255 ioWhole, Str255 outDecimal)
{ 
	short	loop = ioWhole[0];
	
	do
	{
		// Some countries use ',' as decimal, so check for either:
		
		if (ioWhole[loop] == '.' || ioWhole[loop] == ',')
		{ // found the decimal point.  Now right side data to out string:
		
			AppendString(outDecimal,
						 ioWhole, 
						 (short)(loop+1), 
						 (short)(ioWhole[0] - loop));

			if ( outDecimal[0] >= kStr255MaxLen ) 
				outDecimal[0]--;

			outDecimal[ 1+outDecimal[0] ] = 0; // null terminate
			
			ioWhole[0] = loop-1; // shorten string
			ioWhole[ loop ] = 0; // null terminate
			
			loop = 1; // exit loop
		
		} // ioWhole
		
		loop--;
	
	} while (loop > 0);
	
} // end DivideAtDecimal
	
//-------------------------------------------------------------------------------
//
//	AppendString
//	
//	Appends one pascal string to another.
//
//	Inputs:
//		Str255 outTarget				Pascal string to concatinate onto.
//
//		const Str255 inSource			Pascal source string.
//
//		short inSourceStart				First char to start copying from
//										source string.
//
//		short inLength					Number of chars to copy.
//
//	Outputs:
//		Str255 outTarget				Has inSource from char
//										inSourceStart to
//										inSourceStart+inLength appended.
//
//-------------------------------------------------------------------------------

void AppendString (Str255 outTarget,
				   const Str255 inSource,
				   short inSourceStart,
				   short inLength)
{
	
	// If we're going to go over our 255 pascal string length, then
	// back up inLength bytes and start there:
	if ( (outTarget[0] + (inLength - inSourceStart)) > kStr255MaxLen)
		inLength = (kStr255MaxLen + inSourceStart) - outTarget[0];

	for ( ; inLength > 0; inLength--)
		outTarget[ ++outTarget[0] ] = inSource[inSourceStart++];

	if (outTarget[0] >= kStr255MaxLen)
		outTarget[0]--; // Back off one char for null:

	outTarget[ (outTarget[0]+1) ] = 0; // null terminate

} // end AppendString

//-------------------------------------------------------------------------------
//
//	AppendCharToString
//	
//	Appends one character to the end of a pascal string.
//
//	Inputs:
//		Str255 outString				Pascal string to concatinate char onto.
//
//		const unsigned char c			Character to add.
//
//	Outputs:
//		Str255 outString				If under 256 characters, has char
//										appended and is null terminated.
//
//-------------------------------------------------------------------------------

void AppendCharToString (Str255 outString, const unsigned char c)
{
	if (outString[0] < kStr255MaxLen)
		outString[ ++outString[0] ] = c;

	if (outString[0] >= kStr255MaxLen)
		outString[0]--;	// Blast over last character

	outString[ 1+outString[0] ] = 0; // null terminate

} // end AppendCharToString

//-------------------------------------------------------------------------------
//
//	HostBlockMove
//	
//	Copies a chunk of memory. This is by no means the most effective way
//	to do this, but it's simple and its cross-platform and can be bound
//	and error checked in a later version.
//
//	Inputs:
//		Ptr outTarget					Pointer to target memory.
//
//		const Ptr inSource				Pointer to source block.
//
//		const size_t inLength			Number of bytes to copy.
//
//	Outputs:
//		Ptr outTarget					Points to the copy of the
//										chunk of memory.  Only
//										valid if both pointers were
//										valid.
//
//-------------------------------------------------------------------------------

void HostBlockMove (Ptr outTarget,
					const Ptr inSource,
					const size_t inLength)
{
	size_t loop;
	
	if (outTarget != NULL && inSource != NULL)
	{
		for (loop = 0; loop < inLength; loop++)
			*(outTarget++) = *(inSource+loop);
	}
	
} // end HostBlockMove

//-------------------------------------------------------------------------------
//
//	HostCopy
//	
//	Copies a chunk of memory. This is by no means the most effective way
//	to do this, but it's simple and its cross-platform and can be bound
//	and error checked in a later version.
//
//	Inputs:
//		void *outTarget					Pointer to target memory.
//
//		const void *inSource			Pointer to source block.
//
//		const size_t inLength			Number of bytes to copy.
//
//	Outputs:
//		void *outTarget					Points to the copy of the
//										chunk of memory.  Only
//										valid if both pointers were
//										valid.
//
//-------------------------------------------------------------------------------

void HostCopy (void *outTarget, const void *inSource, const size_t inLength)
{
	// Recast and use the existing block move routine:
	HostBlockMove((Ptr)outTarget, (Ptr)inSource, inLength);

} // end HostCopy

//-------------------------------------------------------------------------------
//
//	PIstrlen
//	
//	Returns the length of a null-terminated string.  There are many functions
//	like this one present that are duplicates of the ANSI string library so that
//	most basic functionality is present without including the library.
//
//	Inputs:
//		const char *inString			Pointer to string to check length.
//
//	Outputs:
//		returns int16					Length of string up to NULL byte,
//										or 32767 (MAX_INT).
//
//-------------------------------------------------------------------------------

int16 PIstrlen (const char *inString)
{
		int16	outLength = 0;
		
		while (inString[outLength] != 0 && outLength < 32767)
			outLength++;
		
		return outLength;
		
} // end PIstrlen

//-------------------------------------------------------------------------------
//
//	HostStringCat
//	
//	Concatinates a source string to the end of a target string.
//
//	Inputs:
//		char *outTarget					C string to append to.
//
//		const char *inSource			C string to append to other string.
//
//	Outputs:
//		char *outTarget					Original string with new string.
//
//		returns int16					Length of new string.
//
//-------------------------------------------------------------------------------

int16 HostStringCat (char *outTarget, const char *inSource)
{
	int16	outLength = PIstrlen(outTarget);
	int16	loop = 0;
	
	while (outLength < 32767 && inSource[loop] != 0)
		outTarget[outLength++] = inSource[loop++];
		
	outTarget[outLength] = 0; // null terminate
	
	return outLength;
	
} // end HostStringCat

//-------------------------------------------------------------------------------
//
//	UpperToLower
//	
//	Changes an uppercase character to lowercase.
//
//	Inputs:
//		const char inChar			Character to convert.
//
//	Outputs:
//		char						If between A and Z, the lowercase
//									equivalent.  Otherwise, the original
//									character.
//
//-------------------------------------------------------------------------------

char UpperToLower(const char inChar)
{
	char	outChar = inChar;
	
	if (inChar >= 'A' && inChar <= 'Z')
		outChar = (inChar - 'A') + 'a';

	return outChar;
	
} // end UpperToLower
	
//-------------------------------------------------------------------------------
//
//	HostMatch
//	
//	Finds an occurrence of a target string in a source string.  Note that
//	the target has to match exactly with the source string.  If the target
//	starts in the middle of the source string, HostMatch will return the
//	character position.  Otherwise it will return the constant noMatch
//	or gotMatch.
//
//	Inputs:
//		const char *inSearch		Source C-string to search.
//
//		const char *inTarget		Target C-string to search for.
//
//		const Boolean ignoreCase	TRUE if you want a case-insensitive
//									search; FALSE if you want exact.
//
//		const size_t inLength		Maximum number of chars to check.
//									Should not be over 256.
//
//	Outputs:
//		returns noMatch				If could not find exact match.
//
//		returns gotMatch			If an exact match was found.
//
//		returns uint16				Exact match was found, from char
//									outMatch to end of string.
//
//-------------------------------------------------------------------------------

int16 HostMatch(const char *inSearch,
				const char *inTarget,
				const Boolean ignoreCase,
				const size_t inLength)
{
	const uint16	targetSize = PIstrlen(inTarget);
	
	uint16			loop = 0;
	uint16			start = 0;

	
	int16	outMatch = noMatch; // -1 = no match

	Boolean	done = false;

	while (!done)
	{
		if (inSearch[loop] == inTarget[start] || 
		   (ignoreCase &&
		   (UpperToLower(inSearch[loop]) == UpperToLower(inTarget[start]))))
		{ // we found a match, this is the character it started at:
			if (outMatch < 0)
				outMatch = loop;

			start++;
		}
		else
		{ // reset:
		
			outMatch = noMatch;
			start = 0;
		}

		loop++;

		// we're done if we've hit a null and we've found a match,
		// or if we've hit a null and we're under 256 chars (standard)
		// or we're over the length:
		done = (inSearch[loop] == 0 && start >= targetSize) ||
			   (inSearch[loop] == 0 && inLength <= kStr255MaxLen) ||
			   (loop > inLength);

	} // while !done

	if (inSearch[loop] > 0)
		outMatch = noMatch; // didn't match exactly because the NULL
							// terminator would have matched up, too.

	return outMatch;
	
} // end HostMatch	

//-------------------------------------------------------------------------------
//
//	HostStringMatch
//	
//	Finds an occurrence of a target string in a source string.  Note that
//	the target has to match exactly with the source string.  If the target
//	starts in the middle of the source string, HostMatch will return the
//	character position.  Otherwise it will return the constant noMatch
//	or gotMatch.
//
//	This routine is the same as HostMatch, except the search is done
//	on a pascal string.  The pascal string must have a char free so
//	it can be NULL terminated, otherwise this routine will always
//	return noMatch.
//
//	Inputs:
//		const char *inSearch		Source pascal string to search.
//
//		const char *inTarget		Target C-string to search for.
//
//		const Boolean ignoreCase	TRUE if you want a case-insensitive
//									search; FALSE if you want exact.
//
//	Outputs:
//		returns noMatch				If could not find exact match.
//
//		returns gotMatch			If an exact match was found.
//
//		returns int16				Exact match was found, from char
//									outMatch to end of string.
//
//-------------------------------------------------------------------------------

int16 HostStringMatch(Str255 inSearch, 
					  const char *inTarget,
					  const Boolean ignoreCase)
{
	if (inSearch[0] >= kStr255MaxLen)
		inSearch[0]--; // Blast over last character

	inSearch[ inSearch[0]+1 ] = 0;

	return HostMatch((char *)&inSearch[1], inTarget, ignoreCase, inSearch[0]);

} // end HostStringMatch

//-------------------------------------------------------------------------------
//
//	power
//	
//	Raises a given base to a certain power.
//
//	Inputs:
//		const uint32 inBase			Base to multiply.
//
//		const uint16 inRaise		Exponent to raise base to.
//
//	Outputs:
//		double outPower				Result of doing base^raise.
//
//-------------------------------------------------------------------------------

double power (const uint32 inBase, const uint16 inRaise)
{
	double outPower = 1;
	short loop;
	
	for (loop = 0; loop < inRaise; loop++)
		outPower *= inBase;

	return outPower;
}

//-------------------------------------------------------------------------------
//
//	DoubleToString
//	
//	Takes a double value and creates a string with the number in it, rounding
//	for a given precision.
//
//	Inputs:
//		double inValue				Floating point value to convert.
//
//		short inPrecision			Number of decimal points of precision.
//
//	Outputs:
//		Str255 outString			Pascal string representing number.
//
//-------------------------------------------------------------------------------

void DoubleToString (Str255 outString,
					 double inValue,
					 short inPrecision)
{
	unsigned32	x = 0;
	double		y = 0;
	Boolean		negative = (inValue < 0);
	
	PIResetString(outString);
	
	if (negative)
		inValue = -inValue; // get rid of negative number
	
	// determine if we need to round:
	y = inValue * power(10, inPrecision);
	
	x = (unsigned32)y; // whole portion;
	y -= x; // subtract whole portion
	y *= 10; // move over one more place
	
	// Round:
	if (y >= 5)
		inValue += (5.0 / (double)power(10, (short)(inPrecision+1)));
	
	if (inPrecision > 1)
	{ // now check to see if we can lose a decimal place

		y = inValue * power(10, (short)(inPrecision-1));

		x = (unsigned32)y; // whole portion
		y -= x; // subtract whole portion
		y *= 10; // number of interest

		// our number of interest is zero, so lose a point of precision:
		if (y < 1)
			inPrecision--;
			
	} // inPrecision
	
	y = 0; // done with this
	
	if (negative)
		x = (unsigned long)-inValue; // whole number portion
	else
		x = (unsigned long)inValue;

	// Convert basic number:
	
	NumToString((long)x, outString);
	
	outString[++outString[0]] = '.'; // add decimal point
	
	inValue -= (unsigned32)inValue; // get rid of whole-number portion

	do
	{ // add each character for the decimal values:
	
		inValue *= 10;
		x = (unsigned32)inValue; // whole number
		outString[++outString[0]] = '0' + (char)(x);
		inValue -= x;

	} while (inValue > 0 && --inPrecision > 0);

} // end DoubleToString

//-------------------------------------------------------------------------------
//
//	StringToNumber
//	
//	Converts a pascal string to a numeric (long) value, returning whether
//	it found a valid number or not.
//
//	Inputs:
//		Str255 s					Source string to parse.
//
//	Outputs:
//		long *value					The value found in the string.  If no
//									value is found, this is left untouched.
//
//		returns Boolean				TRUE if a valid number was found,
//									FALSE if no number could be parsed.
//
//-------------------------------------------------------------------------------

StringToNumberResult_t StringToNumber (Str255 s, long *value)
{
	
	short i;
	short j;
	long x;
	Boolean negative = FALSE;
	Boolean isNumber = TRUE;
	Boolean trailingBlanks = FALSE;
	StringToNumberResult_t result = kStringToNumberInvalid;
	
	for (i = 1, j = 0; i <= s[0] && isNumber; ++i)
	{
		
		if (j == 0 && s [i] == ' ')
			; /* Do nothing: Leading blanks */
			
		else if (j > 0 && s [i] == ' ')
			trailingBlanks = TRUE;
			
		else if (trailingBlanks && s [i] != ' ')
			isNumber = FALSE;
			
		else if (j == 0 && !negative && s [i] == '-')
			negative = TRUE;
			
		else if (s [i] < '0' || s [i] > '9')
			isNumber = FALSE;
			
		else
			s [++j] = s [i];
		
	} // for i
		
	if (j == 0 && !negative)
		isNumber = FALSE;
	else
		s[ (s[0] = (char) j)+1] = 0;
	
	if (isNumber)
	{
		
		if (j <= 9)
		{
			#ifdef __PIWin__
			
				x = atol(&s[1]);
			
			#else // Mac
			
				StringToNum (s, &x);
			
			#endif // MSWindows
		}
		else
		{ // j > 9
		
			x = 0x7FFFFFFF;
		}
		
		if (negative)
			x = -x;
			
		*value = x; // return value found
			
	} // isNumber
		
	if (isNumber)
		{
		if (negative)
			result = kStringToNumberNegative;
		else
			result = kStringToNumberValid;
		}
	
	return result;
	
} // end StringToNumber
	
//-------------------------------------------------------------------------------
//
//	StringToDouble
//	
//	Converts a pascal string to a numeric (double) value, returning whether
//	it found a valid number or not.
//
//	Inputs:
//		Str255 s					Source string to parse.
//
//	Outputs:
//		double *value				The value found in the string.  If no
//									value is found, this is left untouched.
//
//		returns Boolean				TRUE if a valid number was found,
//									FALSE if no number could be parsed.
//
//-------------------------------------------------------------------------------

StringToNumberResult_t StringToDouble (Str255 s1, double *value)
	{
	Str255 s2 = ""; 		// string to receive decimal number portion
		
	long x1 = 0;			// initial whole number
	long x2 = 0;			// initial decimal portion
	short precision = 0;	// initial precision to no decimal points
	
	double x = 0;			// variable to work with while coercing	
	
	StringToNumberResult_t result = kStringToNumberInvalid;

	// A flag set to track whether whole, decimal, or all of the
	// text returned was a number or not:
	StringToNumberResult_t wholeNumberResult = kStringToNumberInvalid;
	StringToNumberResult_t decimalNumberResult = kStringToNumberInvalid;
	Boolean isANumber = false;
		
	// Our routine to find the decimal point (period ".") and divide
	// from that on into a separate string (truncating string1):
	DivideAtDecimal(s1, s2);

	// Convert the whole portion. Will return TRUE if successful:
	wholeNumberResult = StringToNumber (s1, &x1);
	
	// Convert the decimal portion. Will return TRUE if successful:
	decimalNumberResult = StringToNumber (s2, &x2);
	
	// The precision for this conversion should be no more
	// than the length of the decimal string (this could get
	// pretty long!):
	precision = s2[0]; // pascal string length is first byte in string
	
	// Only if both error flags are set to do we need to
	// return that no valid number was found:
	isANumber = 
		(
		wholeNumberResult != kStringToNumberInvalid && 
		decimalNumberResult != kStringToNumberInvalid
		);
	
	
	if (isANumber)
	{	// We did get a valid number (or part of one).  Continue.
		// Here we do a little bit of swanky math to turn the decimal
		// portion into its decimal representation (by dividing it
		// by a power of ten equal to its length) and then add
		// that to the whole portion.  If x1 is less than zero,
		// subtract from the whole portion:
		
		if (
			x1 < 0 || 
			wholeNumberResult == kStringToNumberNegative ||
			decimalNumberResult == kStringToNumberNegative
			)
			{
			result = kStringToNumberNegative;
			*value = (double)x1 - ((double)x2 / (double)power(10, s2[0]));
			}
		else
			{
			result = kStringToNumberValid;
			*value = (double)x1 + ((double)x2 / (double)power(10, s2[0]));
			}

		return result;
	}
	else
	{ // wasn't a number, don't set value!

		return result;
	} // end notANumber
}

//-------------------------------------------------------------------------------
//
//	PIParamText
//	
//	Replaces occurrences of "^0", "^1", and "^2" in a source string with
//	subsequent strings.
//
//	Inputs:
//		unsigned char *s			Source search string.
//
//		unsigned char *r0			Replacement string for "^0".
//
//		unsigned char *r1			Replacement string for "^1".
//
//		unsigned char *r2			Replacement string for "^2".
//
//	Outputs:
//		unsigned char *s			Has replacement strings in place of
//									"^0", "^1", and/or "^2".
//
//-------------------------------------------------------------------------------

void PIParamText(unsigned char *s,
				 unsigned char *r0,
				 unsigned char *r1, 
				 unsigned char *r2)
{
	short	loop = 0;
	Boolean	flag = false;
	Str255 s1 = "";
	Str255 rx = "";
	
	while (loop < s[0])
	{
		loop++;
		
		if (s[loop] == '^') flag = true;
		else if (s[loop] >= '0' && s[loop] <= '2' && flag)
		{
			flag = false;
			switch (s[loop])
			{
				case '0':	// Replace occurrences of "^0":
					if (r0[0] > 0) // If provided, copy to rx:
						AppendString(rx, r0, 1, (short)(r0[0]));
					else 
						PIResetString(rx); // Nope, reset rx.
					break;
				case '1':	// Replace occurrences of "^1":
					if (r1[0] > 0) // If provided, copy to rx:
						AppendString(rx, r1, 1, (short)(r1[0]));
					else 
						PIResetString(rx); // Nope, reset rx.
					break;
				case '2':	// Replace occurrences of "^2":
					if (r2[0] > 0) // If provided, copy to rx:
						AppendString(rx, r2, 1, (short)(r2[0]));
					else 
						PIResetString(rx); // Nope, reset rx.
					break;
			
			} // switch
			
			// Make a copy of everything before the "^n":
			AppendString(s1, s, 1, (short)(loop-2));
			
			if (rx[0] > 0) // We do have to replace, so copy that:
				AppendString(s1, rx, 1, (short)rx[0]);
			
			// Now copy the remainder of the string to the end:
			AppendString(s1, s, (short)(loop+1), (short)(s[0]-loop));
			
			// Reset then copy to the original string:
			PIResetString(s);
			AppendString(s, s1, 1, s1[0]);
			
			// Now reset our work strings:
			PIResetString(s1);
			PIResetString(rx);
			
			loop--; // stay here to check for other ^N's.
		
		} // else if
		
		else flag = false;
		
	} // end while
	
} // end PIParamText

//-------------------------------------------------------------------------------
//
//	HostReportError
//	
//	Returns an error string and the error code required to tell the
//	host to display that string.
//
//	The most useful way to use this routine is to point inSource at
//	the host errString (gStuff->errString), and inTarget at your
//	string that describes the error.  Then set the global return
//	result (gResult) to the result of this routine (errReportString).
//
//	For example:
//		gResult = HostReportError(gStuff->errString, "this plug-in crashed");
//
//	Once you return to the host, will display the error dialog:
//		"Could not complete your request because this plug-in crashed."
//
//	Inputs:
//		Str255 inSource					Pascal source string.
//
//		Str255 inTarget					Pascal target string.
//
//	Outputs:
//		Str255 inSource					Will be a copy of inTarget.
//
//		returns short errReportString	Host error code to display a string.
//
//-------------------------------------------------------------------------------

short HostReportError(Str255 inSource, Str255 inTarget)
{
	const short outError = errReportString;
	
	if (inSource)
	{ // have to clear and copy:
	
		PIResetString(inSource);
		AppendString(inSource, inTarget, 1, inTarget[0]);
	}
	else
	{ // nothing there.  Copy the pointer:
	
		inSource = inTarget;
	}

	return outError;
	
} // end HostReportError

//-------------------------------------------------------------------------------
//
//	ClearOSInfo
//	
//	An OSInfo structure is a simple structure created in the PIUtilities world
//	to retrieve gestalt-like information about the current host and the
//	operating system.  See PIUtilities.h.
//
//	Inputs:
//		none
//
//	Outputs:
//		OSInfo *osInfo					Structure will be completely cleared.
//
//-------------------------------------------------------------------------------

void ClearOSInfo(OSInfo *osInfo)

{
	osInfo->signature = NULLID;
	osInfo->majorVersion = 
	osInfo->minorVersion =
	osInfo->subVersion =
	osInfo->stage =
	osInfo->stageVersion =
	osInfo->os = 0;
}

//-------------------------------------------------------------------------------
//
//	GetOSInfo
//	
//	Retrieves information about the host and the operating system, populating
//	an OSInfo structure.
//
//	An OSInfo structure is a simple structure created in the PIUtilities world
//	to retrieve gestalt-like information about the current host and the
//	operating system.  See PIUtilities.h.
//
//	Inputs:
//		HandleProcs *procs		Pointer to the handle procs callbacks.
//								NOTE: These are no longer used because
//								many third-party hosts were bombing when
//								we tried to make these calls, which sometimes
//								come very early in the plug-in process.
//
//	Outputs:
//		OSInfo *osInfo			Structure will be filled with
//								all obtainable information, including:
//
//								signature of host (Windows hosts are
//								mapped to their Mac unsigned32 4-byte
//								ResType counterparts);
//								majorVersion, minorVersion, subVersion,
//								of host;
//								stage (alpha, beta, development, release),
//								and stageVersion of host;
//								OS (Win95, WinNT, WinNT4, MacOS6, MacOS7,
//								MacOS8).
//								See PIUtilities.h.
//
//		returns OSErr			noErr or an error if one occurred.
//
//-------------------------------------------------------------------------------

OSErr GetOSInfo(HandleProcs* procs, OSInfo *osInfo)
{
	OSErr				err = noErr;

#ifdef __PIWin__

	DWORD				versionInfoHandle = 0; 	// pointer to variable to receive zero
	int16				versionInfoSize = 0;
	Handle				h = NULL;
    Ptr					versionInfoData = NULL;
	char				filename[kStr255InitLen] = "";
	VS_FIXEDFILEINFO *	ffi = NULL;
	char *				p = NULL;
	UINT				len = 0, slen = 0;
	unsigned32			ux = 0;
	int32				x = 0;
	Str255				s1 = "", s2 = "";
	OSErr				winErr = noErr;
	
	//---------------------------------------------------------------------------
	//	(1) Clear out structures, allocate memory, load basic hunka data
	//---------------------------------------------------------------------------
	
	ClearOSInfo(osInfo); // clears out struct

	GetModuleFileName(NULL /* hDllInstance */, filename, kStr255MaxLen);
	versionInfoSize = (int16)GetFileVersionInfoSize(filename, &versionInfoHandle);

	// Unfortunately, not all hosts seem to support the handle suite correctly,
	// so we're going to do this the old fashioned way:
	/* Old routine:
	h = HostNewHandle(procs, 1+versionInfoSize);
	if (h == NULL)
		return memFullErr; // couldn't allocate buffer
	
	versionInfoData = HostLockHandle(procs, h, true);
	*/

	versionInfoData = malloc(1+versionInfoSize);
	if (versionInfoData == NULL)
			return memFullErr; // couldn't allocate buffer

	// If this function fails, the return value is 0:
	winErr = 
		GetFileVersionInfo
		(
		filename, 
		versionInfoHandle, 
		versionInfoSize, 
		versionInfoData
		);

	if (winErr != errFunctionFailed)
	{ // no error.  Continue.
	
		//-----------------------------------------------------------------------
		//	(2) We were able to allocate everything.  Go get OS data.
		//-----------------------------------------------------------------------
		
		winErr = VerQueryValue
			(
			versionInfoData, 
			"\\", 
			&p, 
			&len
			); // ask for basic info
		
		if (winErr != errFunctionFailed)
		{ // no error.  Map basic info into os versions.
		  // MapSystem is a very simple and basic OS defining routine.
		  // You may want to create your own, more
		  // detailed OS interrogator.
		  
			ffi = (VS_FIXEDFILEINFO *)p;
			osInfo->stage = MapStage(ffi->dwFileFlagsMask, ffi->dwFileFlags); // release, prerelease, etc.
			osInfo->stageVersion = 0;
			osInfo->os = MapSystem(ffi->dwFileOS);
		}
		
		//-----------------------------------------------------------------------
		//	(3) Now try to get some strings with the file information we need.
		//		We'll often error out right here because the information can
		// 		appear with different specific translation tags.  So
		// 		we'll really try an exhaustive approach of first
		// 		attempting to get the first standard Translation, then
		// 		the first generic Translation, and if that doesn't work,
		// 		we'll do the search ourselves through the hunka data:
		//-----------------------------------------------------------------------
		
		winErr = VerQueryValue
			(
			versionInfoData, 
			"\\VarFileInfo\\Translation", 
			&p, 
			&len
			); // get the translation table

		if (winErr == errFunctionFailed)
		{ // Error.  Try generic value:
			ux = 0x04090000L;
		}
		else
		{ // No error.  Get first entry in translation table.
		
			for (x = 0; x < 2; x++)
			{
				ux |= (unsigned8)(*p++);
				ux |= ((unsigned8)(*p++)) << 8;
				if (x == 0)
					ux <<= 16; // move high
			}
		}

		_ultoa( ux, s1, 16 ); // convert unsigned long to string

		// Now build string we're going to use:
		HostStringCat(s2, "\\StringFileInfo\\"); // basic prompt
		
		if ((ux >> 24) < 0x10) 
			HostStringCat(s2, "0"); // add leading zero
		
		HostStringCat(s2, s1); // add translation table
		HostStringCat(s2, "\\"); // trailing backslash
		
		slen = PIstrlen(s2)+1; // string length and trailing null
		
		HostCopy(s1, s2, slen); // copy s2 to s1
		HostStringCat(s1, "InternalName"); // getting Internal Name next

		winErr = VerQueryValue
			(
			versionInfoData, 
			s1,
			&p,
			&len
			);

		if (winErr == errFunctionFailed)
		{ // Error.
		
			//-------------------------------------------------------------------
			//	(4) The generic translation didn't work.  Search ourselves.
			//-------------------------------------------------------------------
				
			len = HostMatch
				(
				versionInfoData,
				"InternalName",
				true,
				versionInfoSize
				);

			// Return value is error or correct length:
			if (len != noMatch)
			{ // found the string
		
				p = versionInfoData + len + 14; // length of "InternalName" + pad

				// If next two bytes are a single character then a null, skip them
				// (Language identifier?)
				if (p[1] == 0) p += 2;
				
				// We've got a Windows name.  Do the compare and find the
				// equivalent Mac OS 4-byte ResType signature:
				osInfo->signature = MapSignature(p);
			}

			len = HostMatch
				(
				versionInfoData,
				"FileVersion",
				true,
				versionInfoSize
				);

			// Return value is error or correct length:
			if (len != noMatch)
			{ // now get the File version ourselves
		
				p = versionInfoData + len + 12; // length of "FileVersion" + pad

				// If next two bytes are a single character then a null, skip them
				// (Language identifier?)
				if (p[1] == 0) p += 2;
				
				PIResetString(s1);
				PIResetString(s2);
				AppendString(s1, (unsigned char *)p, 0, (short)strlen(p)); // copy string to pascal
				DivideAtDecimal(s1, s2);

				x = 0; // just in case
				StringToNumber(s1, &x);
				osInfo->majorVersion = (unsigned8)x;
				osInfo->subVersion = osInfo->minorVersion = 0;

				x = 0; // just in case
				StringToNumber(s2, &x);
				
				if (x > 9)
				{ // put tens place in minor, hundreds place in sub
				
					osInfo->minorVersion = (unsigned8)(x / 10);
					osInfo->subVersion = (unsigned8)(x - (osInfo->minorVersion*10));
					// removes tenths digit
				}
				else if (s2[0] > 1) 
				{ // was "x.0y" so put hundreds place in sub, instead of minor
				
					osInfo->subVersion = (unsigned8)x;
				}
				else
				{
					osInfo->minorVersion = (unsigned8)x;
				}
			}
		}
		else
		{
			//-------------------------------------------------------------------
			//	(5) Try to map the Windows signature to the Mac ResType.
			//-------------------------------------------------------------------
			
			osInfo->signature = MapSignature(p);

			HostCopy(s1, s2, slen); // copy s2 to s1
			HostStringCat(s1, "FileVersion"); // getting File Version next

			winErr = VerQueryValue
				(
				versionInfoData, 
				s1,
				&p,
				&len
				);

			if (winErr != errFunctionFailed)
			{ // No error.  Continue.
	
				//---------------------------------------------------------------
				//	(6) Convert a string version ("X.Y.Z" or "X.YZ") to
				//		major, minor, and subVersions.
				//---------------------------------------------------------------
		
				PIResetString(s1);
				PIResetString(s2);
				AppendString(s1, (unsigned char *)p, 0, (short)strlen(p)); // copy string to pascal
				DivideAtDecimal(s1, s2);

				x = 0; // just in case
				StringToNumber(s1, &x);
				osInfo->majorVersion = (unsigned8)x;

				x = 0; // just in case
				StringToNumber(s2, &x);
	
				if (x > 10)
				{ // put tens place in minor, hundreds place in sub
	
					osInfo->minorVersion = (unsigned8)(x / 10);
					osInfo->subVersion = (unsigned8)(x - (osInfo->minorVersion*10));
					// removes tenths digit
				}
				else if (s2[0] > 1)
				{
					// was "x.0y" so put hundreds place in sub, instead of minor
					osInfo->subVersion = (unsigned8)x;
				}
				else 
				{
					osInfo->minorVersion = (unsigned8)x;
				}

			} // Error.

		} // Couldn't get translation table info.

		// Unfortunately all the Handle stuff may not be available in some hosts.
		if (versionInfoData != NULL)
		{
			free(versionInfoData);
			versionInfoData = NULL;
		}
	} // Couldn't get the version info struct.

	#elif defined(__PIMac__)

	// NOTE:
	// We're not going to mix Macintosh calls with Photoshop handle suite calls.  It
	// worked at one point, but different hosts get weird about it...
	
	short	rID;
	Handle	h = NULL;
	Ptr		p = NULL;
	long	x;
		
	#ifdef __PIMWCW__
		#pragma unused (procs)
	#endif
	
	//---------------------------------------------------------------------------
	//	(1) Clear out structures, allocate memory, store resource pointers
	//---------------------------------------------------------------------------
	
	ClearOSInfo(osInfo); // clears out struct

	rID = CurResFile(); // get the current resource file
	
	UseResFile( LMGetCurApRefNum() ); // sets the res file to the application (not the plug-in)
	
	h = Get1IndResource('BNDL', 1); // want the 1st bndl resource of the app
	
	if (!ResError())
	{ // work with bndl

		//-----------------------------------------------------------------------
		//	(2) Was able to read BNDL resource.  Get the app signature from it.
		//-----------------------------------------------------------------------
		
		// Mac lock the handle:
		HLock(h);
		p = *h;
		if (p != NULL)
		{
			osInfo->signature = PIChar2Type(p);
		}
		
		HUnlock(h);
		ReleaseResource(h);
		h = NULL;
		p = NULL;
	}
		
	h = Get1IndResource('vers', 1); // want the 1st vers resource of the app
		
	if (!ResError())
	{ // No error.
		
		//-----------------------------------------------------------------------
		//	(3) Was able to read vers resource.  Get the version info from it.
		//-----------------------------------------------------------------------

		HLock(h);
		p = *h;
		if (p != NULL)
		{
			osInfo->majorVersion = (unsigned8)*p++;
			osInfo->minorVersion = osInfo->subVersion = (unsigned8)*p++;
			osInfo->minorVersion >>= 4; // top 4 bits = vers2
			osInfo->subVersion &= 0x0F; // bottom 4 bits = vers3
			osInfo->stage = MapStage(NULLID,(unsigned32)*p++); // 0x20 = dev,
							  // 0x40 = alpha,
							  // 0x60 = beta,
							  // 0x80 = release
			osInfo->stageVersion = (unsigned8)*p;
		}
		HUnlock(h);
		ReleaseResource(h);
		h = NULL;
		p = NULL;
	}
	
	if (Gestalt(gestaltSystemVersion, &x) == noErr)
	{ // got the system version.

		//-----------------------------------------------------------------------
		//	(4) Was able to read OS info from Gestalt call.  Map and return
		//		an enumeration to tell what system this is (very simple and
		//		basic OS defining -- you may want to create your own, more
		//		detailed OS interrogator.)
		//-----------------------------------------------------------------------

		osInfo->os = MapSystem((unsigned32)(x >> 8));

	}

	//---------------------------------------------------------------------------
	//	(5) Restore resource pointer and exit.
	//---------------------------------------------------------------------------
	
	UseResFile (rID); // restore

#endif // Mac/Unix

	return err;

} // end GetOSInfo

//-------------------------------------------------------------------------------
//
//	AppMatch
//	
//	Retrives the OSInfo structure for a host, then matches that signature
//	against either a list or a single resource type.  If it finds it, it
//	returns an unsigned long with compressed information.
//
//	Inputs:
//		HandleProcs *procs				Pointer to the Handle procs callbacks.
//
//		const ResType *signatures		Pointer to one or more 4-byte resource
//										names to compare signature to.  Must
//										be NULLID terminated.
//
//	Outputs:
//		unsigned32 outCompressedInfo	outCompressedInfo is in the form:
//										AAbbCCdd, where AA is the high
//										order byte, and dd is the low, and:
//
//										AA = majorVersion,
//										bb = minorVersion,
//										CC = subVersion,
//										dd = os.
//
//										If something was found, this
//										structure will not be empty.  If
//										no match was made, this will be 0.
//
//-------------------------------------------------------------------------------

unsigned32 AppMatch (HandleProcs *procs, const ResType *signatures)
{
	ResType			*p = (ResType *)signatures;
	Boolean			gotit = false;
	unsigned32 		outCompressedInfo = 0;
	static Boolean	initedOSInfo = false;
	static OSInfo	osInfo;
	
	if (!initedOSInfo)
	{ // Only initialize this structure once.
		OSErr	err = GetOSInfo(procs, &osInfo);

		if (err != noErr)
			ClearOSInfo(&osInfo);
			
		initedOSInfo = true;
	}
	
	if (ValidOSInfo(osInfo))
	{ // Macro to check for valid os info structure says its fine.
	
		while (*p != NULLID && gotit == false)
			gotit = (*p++ == osInfo.signature); // search for match
			
		if (gotit)
		{
			outCompressedInfo = (unsigned32)osInfo.majorVersion << 24 |
				   				(unsigned32)osInfo.minorVersion << 20 |
				   				(unsigned32)osInfo.subVersion << 16 |
				  				osInfo.os;
		}
	}

	return outCompressedInfo;

} // end AppMatch

//-------------------------------------------------------------------------------
//
//	CheckForHost
//	
//	Retrives the OSInfo structure for a host, then matches that signature
//	against a list of known hosts.
//
//	These signatures are specifically grouped so that if a different
//	version of the same app is found (such as Illustrator 1.1, which
//	has a different signature than Illustrator 6), the routine will
//	return the first signature with the correct version info.
//
//	Inputs:
//		HandleProcs *procs				Pointer to the Handle procs callbacks.
//
//		const ResType signature			Specific 4-byte signature to search for.
//
//	Outputs:
//		unsigned32 outCompressedInfo	outCompressedInfo is in the form:
//										AAbbCCdd, where AA is the high
//										order byte, and dd is the low, and:
//
//										AA = majorVersion,
//										bb = minorVersion,
//										CC = subVersion,
//										dd = os.
//
//										If something was found, this
//										structure will not be empty.  If
//										no match was made, this will be 0.
//
//-------------------------------------------------------------------------------

unsigned32 CheckForHost (HandleProcs *procs, const ResType signature)
{
	static const ResType signatures[] =
	{
		rtCorelDraw6, NULLID,
		rtCorelArtPaint6, NULLID,
		rtDeBabelizer, NULLID,
		rtAfterEffects, NULLID,
		rtIllustrator3, rtIllustrator88,
		rtIllustrator11, rtIllustrator6, NULLID,
		rtPageMaker5, rtPageMaker6, rtPageMaker65, NULLID,
		rtPhotoDeluxe, NULLID,
		rtPhotoshop, NULLID,
		rtPremiere, NULLID
	};
							  	   
	static const short numResTypes = sizeof(signatures) / sizeof(ResType);

	short loop = 0;
	unsigned32	outCompressedInfo = 0;
	
	do
	{
		if (signatures[loop] == signature && signatures[loop] != NULLID)
		{ // got a match
			outCompressedInfo = AppMatch(procs, &signatures[loop]);
			loop = numResTypes; // done looping
		}
	} while (++loop < numResTypes);
	
	return outCompressedInfo;

} // end CheckForHost

//-------------------------------------------------------------------------------
//
//	MapStage
//	
//	Given a mask and stage value, return whether a host is development, beta,
//	alpha, or release.
//
//	Inputs:
//		const unsigned32 stageMask		Mask value.  Windows only.
//
//		const unsigned32 stage			Stage value.  Generally equates
//										to development, alpha, beta, or release.
//
//	Outputs:
//		returns unsigned8				With enumerations for either
//										Windows or Macintosh.  See PIUtilities.h.
//
//-------------------------------------------------------------------------------

unsigned8	MapStage (const unsigned32 stageMask, const unsigned32 stage)

{
	#ifdef __PIWin__

		// These "magic" numbers are taken from the online reference:
		
		if ((stageMask & 0x01L) && (stage & 0x01L))
			return OSStageDevelopment; // 0x01L = VS_FF_DEBUG
		
		else if ((stageMask & 0x02L) && (stage & 0x02L))
			return OSStageBeta; // 0x02L = VS_FF_PRERELEASE
		
		else 
			return OSStageRelease;

	#else // Mac/Unix

		#if __MWERKS__
			#pragma unused (stageMask)
		#endif // MWERKS

		// The "magic" numbers are documented in Inside Macintosh:

		switch (stage)
		{
			case 0x20:
				return OSStageDevelopment;
				break;
			case 0x40:
				return OSStageAlpha;
				break;
			case 0x60:
				return OSStageBeta;
				break;
			case 0x80:
			default:
				return OSStageRelease;
				break;
				
		} // switch stage
		
	#endif // Mac/Unix
	
} // end MapStage

//-------------------------------------------------------------------------------
//
//	MapSystem
//	
//	Given a value, return whether a host is WinNT, Win16, Win32, MacOS6, MacOS7,
//	etc.
//
//	Inputs:
//		const unsigned32 os				Long with OS magic number.
//
//	Outputs:
//		returns unsigned8				With enumerations for either
//										Windows or Macintosh.  See PIUtilities.h.
//
//-------------------------------------------------------------------------------

unsigned8	MapSystem (const unsigned32 os)
{
	#ifdef __PIWin__

		// These "magic" numbers are taken from the online reference:

		switch (os)
		{
			case 0x00040000L:
				return OSWinNT;
				break;
			case 0x01L:
				return OSWin16;
				break;
			case 0x04L:
			default:
				return OSWin32;
				break;
		}

	#else // Mac/Unix

		// The "magic" numbers are documented in Inside Macintosh:

		switch (os)
		{
			case 6:
				return OSMacOS6;
				break;
			case 8:
				return OSMacOS8;
				break;
			case 7:
			default:
				return OSMacOS7;
				break;
		}

	#endif // MSWindows
	
} // end MapSystem

//-------------------------------------------------------------------------------
//
//	MapSignature
//	
//	Given the entry for a Windows original filename, find the corresponding
//	Macintosh 4-byte unsigned32 ResType signature.
//
//	Inputs:
//		const char *inName			Windows original filename.
//
//	Outputs:
//		returns unsigned32			Correlating ResType 4-byte signature.
//									0 if not found.
//
//-------------------------------------------------------------------------------

unsigned32	MapSignature (const char *inName)
{
	static const char *fileNames[] =
	{
		fnCorelDraw6,
		fnCorelArtPaint6,
		fnDeBabelizer,
		fnAfterEffects,
		fnIllustrator6,
		fnPageMaker5,
		fnPageMaker6,
		fnPageMaker65,
		fnPhotoDeluxe,
		fnPhotoshop,
		fnPremiere
	};
	
	static const ResType fileTypes[] =
	{
		rtCorelDraw6,
		rtCorelArtPaint6,
		rtDeBabelizer,
		rtAfterEffects,
		rtIllustrator6,
		rtPageMaker5,
		rtPageMaker6,
		rtPageMaker65,
		rtPhotoDeluxe,
		rtPhotoshop,
		rtPremiere
	};

	static const short numResTypes = sizeof(fileTypes) / sizeof(ResType);

	unsigned32 outSignature = NULLID;
	short loop = 0;
	
	do
	{
		if (HostMatch(inName, fileNames[loop], true, kStr255MaxLen) == gotMatch)
		{ // got a match

			outSignature = fileTypes[loop];
			loop = numResTypes;
		}

	} while (++loop < numResTypes);

	return outSignature;

} // end MapSignature

//-------------------------------------------------------------------------------
//
//	IsWindows
//	
//	Check to see if any of the Windows OS enums are present in a
//	compressedInfo long.
//
//	This is a simple way to check to see if a host is present in a certain
//	operating system.  For example:
//		bHostIsPremiereWin = IsWindows(HostIsPremiere());
//
//	Inputs:
//		const unsigned32 flag			CompressedInfo.  See AppMatch() and
//										CheckForHost().
//	Outputs:
//		returns Boolean					TRUE if any Windows OS enums are
//										present;
//										FALSE if not.
//
//-------------------------------------------------------------------------------

Boolean		IsWindows (const unsigned32 flag)
{
	unsigned8 smallFlag = (unsigned8)(flag & 0xFF); // take lowest byte

	return (smallFlag >= OSWinStart && smallFlag <= OSWinEnd);

} // end IsWindows

//-------------------------------------------------------------------------------
//
//	IsMacOS
//	
//	Check to see if any of the Macintosh OS enums are present in a
//	compressedInfo long.
//
//	This is a simple way to check to see if a host is present in a certain
//	operating system.  For example:
//		bHostIsPremiereWin = IsWindows(HostIsPremiere());
//
//	Inputs:
//		const unsigned32 flag			CompressedInfo.  See AppMatch() and
//										CheckForHost().
//	Outputs:
//		returns Boolean					TRUE if any Macintosh OS enums are
//										present;
//										FALSE if not.
//
//-------------------------------------------------------------------------------

Boolean		IsMacOS (const unsigned32 flag)
{
	unsigned8 smallFlag = (unsigned8)(flag & 0xFF); // take lowest byte

	return (smallFlag >= OSMacOSStart && smallFlag <= OSMacOSEnd);

} // end IsMacOs

//-------------------------------------------------------------------------------
//
//	IsValidHost
//	
//	Check to see if the host returned is valid.  (Mac or Win).
//
//	This is a simple way to check to see if a host is present.  For example:
//		bHostIsPremiere = IsValidHost(HostIsPremiere());
//
//	Inputs:
//		const unsigned32 flag			CompressedInfo.  See AppMatch() and
//										CheckForHost().
//	Outputs:
//		returns Boolean					TRUE if valid;
//										FALSE if not.
//
//-------------------------------------------------------------------------------

Boolean		IsValidHost (const unsigned32 flag)
{
	// Any non-zero value indicates a match:
	return (flag != kHostNoMatch);

} // end IsValidHost

//-------------------------------------------------------------------------------
//
//	AllocateGlobals
//	
//	Allocates globals using the handle suite, calls the Initialization routine
//	if necessary, and locks and returns a pointer for their use.
//
//	This is the first routine you'll want to use coming in and out of your
//	plug-in.  It was designed to make plug-in setup and reentry more
//	bulletproof and simpler.
//
//	Inputs:
//		const uint32 resultAddr			The address of the gResult variable.
//
//		const uint32 paramBlockAddr		The address of the gStuff paramBlock.
//
//		HandleProcs *procs				Pointer to the HandleProcs callbacks.
//
//		const size_t size				Size of the entire global structure,
//										including the parameter block.
//
//		long *data						Pointer to the *data long which is
//										passed in and stored by the host.
//
//		FProc InitGlobals				Function pointer to your InitGlobals
//										routine to be called if we're
//										allocating them for the first time.
//										The declaration of the routine must
//										be:
//											void InitGlobals(Ptr globalPtr);
//
//	Outputs:
//		long *data						Will be set to the handle to the
//										global memory for reentry.  The host
//										automatically holds this data and
//										passes it back on subsequent iterations
//										(unless the plug-in had been unloaded).
//
//		returns Ptr						NULL if unsuccessful allocating memory,
//										otherwise a pointer to the locked
//										global structure.
//
//-------------------------------------------------------------------------------

Ptr AllocateGlobals(const uint32 resultAddr,
					const uint32 paramBlockAddr,
					HandleProcs *procs,
					const size_t size,
					long *data, 
					FProc InitGlobals)

{
	Ptr globalPtr = NULL; // initial no error return value
	
	if (!*data)
	{ // Data is empty, so initialize our globals
		
		// Create a chunk of memory to put our globals.
		// Have to call HostNewHandle directly, since gStuff (in
		// the PINewHandle macro) hasn't been defined yet
		Handle h = HostNewHandle(procs, size);
		
		if (h != NULL)
		{ // We created a valid handle. Use it.
		
			// lock the handle and move it high
			// (we'll unlock it after we're done):
			globalPtr = HostLockHandle(procs, h, TRUE);
			
			if (globalPtr != NULL)
			{ // was able to create global pointer.
			
				// the result is always the first two bytes of
				// the global structure, always followed by a
				// pointer to the param block.  Set result
				// to zero and set the pointer to the
				// param block:
				uint32 *result = (uint32 *)globalPtr;
				uint32 *address = (uint32 *)(globalPtr + sizeof(short *));
				
				*result = resultAddr;
				*((short *)resultAddr) = noErr;
				
				*address = paramBlockAddr;
				
				// now initialize any globals that need starting values:
				InitGlobals (globalPtr);
				
				// store the handle in the passed in long *data:
				*data = (long)h;
				h = NULL; // clear the handle, just in case
			
			}
			else
			{ // There was an error creating the pointer.  Back out
			  // of all of this.

				HostDisposeHandle(procs, h);
				h = NULL; // just in case
			}
		}		
	}
	else
	{
		// we've already got a valid structure pointed to by *data
		// lock it, cast the returned pointer to a global pointer
		// and point globals at it:
		globalPtr = HostLockHandle(procs, (Handle)*data, TRUE);

		if (globalPtr != NULL)
		{ // able to create global pointer.
		
			// the result is always the first two bytes of
			// the global structure, always followed by a
			// pointer to the param block.  Set result
			// to zero and set the pointer to the
			// param block:
			uint32 *result = (uint32 *)globalPtr;
			uint32 *address = (uint32 *)(globalPtr + sizeof(short *));
			
			*result = resultAddr;
			*((short *)resultAddr) = noErr;
			
			*address = paramBlockAddr;
		}
	}
	
	return globalPtr;
}

//-------------------------------------------------------------------------------

// end PIUtilities.c
