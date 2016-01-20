#include <windows.h>
#include <mmsystem.h>
#include "proto.h"
#include "midinote.p"

#ifndef WIN32
#define MAKEWORD(bLow, bHigh)   ((WORD)((bHigh<<8)|bLow))
#endif

static HMIDIOUT hMidiOut;

//***********************************************************************
void MidiNoteOpen( void )
//***********************************************************************
{
	if ( hMidiOut )
		return;

//	UINT wNumDevs = midiOutGetNumDevs();
//	MIDIOUTCAPS moc;
//	UINT wError = midiOutGetDevCaps( MIDIMAPPER/*nDevice*/, &moc, sizeof(moc) );
	UINT wError = midiOutOpen( &hMidiOut, MIDIMAPPER/*nDevice*/,
		NULL/*dwCallback*/, 0L/*dwInst*/, 0L/*dwFlags*/ );
}

//***********************************************************************
void MidiNoteClose( void )
//***********************************************************************
{
	if ( !hMidiOut )
		return;

	UINT wError;
	wError = midiOutReset( hMidiOut );
	wError = midiOutClose( hMidiOut );
	hMidiOut = NULL;
}

//***********************************************************************
void MidiNotePlay( 	BYTE bChannel, BYTE bInstrumentPatch, BYTE bNote,
					BYTE bVolume )
//***********************************************************************
{
	if ( !hMidiOut )
		return;

	UINT wError;
	wError = midiOutShortMsg( hMidiOut,
		MAKELONG( MAKEWORD( 0xC0 + bChannel/*0-16*/, bInstrumentPatch/*0-127*/),
				  MAKEWORD( 0, 0 ) ) );
	// Turn the note on
	wError = midiOutShortMsg( hMidiOut,
		MAKELONG( MAKEWORD( 0x90 + bChannel/*0-16*/, bNote/*0-127*/ ),
				  MAKEWORD( bVolume, 0 ) ) );
}

//***********************************************************************
void MidiNoteStop( 	BYTE bChannel, BYTE bInstrumentPatch, BYTE bNote )
//***********************************************************************
{
	if ( !hMidiOut )
		return;

	UINT wError;
	wError = midiOutShortMsg( hMidiOut,
		MAKELONG( MAKEWORD( 0xC0 + bChannel/*0-16*/, bInstrumentPatch/*0-127*/),
				  MAKEWORD( 0, 0 ) ) );
	// Turn the note off
	wError = midiOutShortMsg( hMidiOut,
		MAKELONG( MAKEWORD( 0x80 + bChannel/*0-16*/, bNote/*0-127*/ ),
				  MAKEWORD( 0, 0 ) ) );
}

//***********************************************************************
void MidiNoteStopAll( void )
//***********************************************************************
{
	if ( !hMidiOut )
		return;

	midiOutReset( hMidiOut );
}
