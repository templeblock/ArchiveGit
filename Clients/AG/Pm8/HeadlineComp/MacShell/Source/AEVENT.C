/* ===========================================================================
   Module   :	aevent.c
   Project  :	Renaissance graphic headline test utility.
   Abstract :	Apple Event handler.
   Status   :	Under Development
  
   Copyright © 1996, Turning Point Software.  All Rights Reserved.
  
   To Do:
  
  
   =========================================================================== */
   

#include      "HeadlinerIncludes.h"
#include      "aevent.h"

ASSERTNAME


/**  globals  **/
extern  Boolean              gQuit;


/* ----------------------------------------------------------------------------
   handleAECore 
   Handles core application Apple Events.  
   Returns an apple event error code.                                        
   ---------------------------------------------------------------------------- */
pascal  OSErr  handleAECore (AppleEvent theEvent, AppleEvent theReply, long refCon)
{
#pragma unused(theReply)
#pragma unused(refCon)
OSErr                osErr = noErr;
Size                 rSize;
DescType             rType;
unsigned long        eventID;

/**  get event ID of apple event  **/
    if ((osErr = AEGetAttributePtr(&theEvent, keyEventIDAttr, typeType, &rType, (Ptr)&eventID, sizeof(eventID), &rSize)) != noErr)
       return(osErr);

/**  process apple event  **/
    switch (eventID) 
    {
       case kAEOpenApplication:
          osErr = noErr;
          break;
       case kAEOpenDocuments:
          TpsAssert(false, "We have no Documents!");
          osErr = errAEEventNotHandled;
          break;
       case kAEPrintDocuments:
          TpsAssert(false, "We print no Documents!");
          osErr = errAEEventNotHandled;
          break;
       case kAEQuitApplication:
          gQuit = true;														// exit event loop
          break;
       case kAEApplicationDied:
          TpsAssert(false, "A close relative to this application just Died!");
          osErr = errAEEventNotHandled;
          break;
       case kAEAnswer:
          TpsAssert(false, "We have no Answers!");
          osErr = errAEEventNotHandled;
          break;
      default:
          TpsAssert(false, "We are very confused over this turn of Events!");
          osErr = errAEEventNotHandled;
          break;
    }

    return(osErr);
}



