// wait until the application is the foreground application and also notify the user that
// the application needs attention

#include "waitforegrd.h"

#define DropSound 6

McoStatus waitTillForeground(void)
{
ProcessSerialNumber PSN1,PSN2;
OSErr iErr;
struct NMRec  myNote;	//declare your NMRec
Handle ManDoneS;	//declare a handle for the sound
EventRecord 	event;
Boolean		result;

iErr = GetCurrentProcess(&PSN1);
iErr = GetFrontProcess(&PSN2);

iErr = SameProcess(&PSN1,&PSN2,&result);
if (!result)
	{

	myNote.qType = nmType;	//queue type -- nmType = 8
	myNote.nmMark = 1;	//get mark in Apple menu
	myNote.nmIcon = nil;	//no flashing Icon

	//get the sound you want out of your resources
	ManDoneS = GetResource('snd ', DropSound);

	myNote.nmSound = ManDoneS;	//set the sound to be played
	myNote.nmStr = nil;	//no alert box
	myNote.nmResp = nil;	//no response procedure
	myNote.nmRefCon = nil;	//nil since don't need my A5

	NMInstall(&myNote);

	do	{
		GetNextEvent(0xFFFF,&event);
		iErr = GetCurrentProcess(&PSN1);
		iErr = GetFrontProcess(&PSN2);
		iErr = SameProcess(&PSN1,&PSN2,&result);
		} while (!result);
		


	NMRemove(&myNote);
	}
	
return MCO_SUCCESS;
	
}
