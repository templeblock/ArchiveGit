/* ************************************************************ */

    VBMIX.EXE - Visual Basic Wavemix Sample Application

    by Chris Grinstead
    Microsoft Corporation
    
    This sample application was written in Microsoft Visual 
    Basic 3.0 and shows how WAVEMIX.DLL might be called from 
    a Visual Basic program. 
    See WAVEMIX.TXT for information on the WaveMix API itself.

    NOTES:
    
    Running more than one WaveMix.DLL app at a time:
    Since Visual Basic 3.0 does not expose certain Windows
    messages for processing, the custom control MSGBLAST.VBX 
    is used to provide a means processing Windows messages such 
    as WM_ACTIVATE. This allows VBMIX to release the wave device
    when you switch to another application.

    Debugging in the VB environment:
    Always use the System Menu or File Exit, etc.  to shut 
    down your app while debugging in the VB environment. Since 
    WaveMix.DLL is not re-entrant, pressing the Stop button in 
    debugger will not clean up WaveMix stuff, leaving the WaveMixSession
    open and all of the wave files open as well. 

/* ************************************************************* */
    


    
