/* fddll.c - Contains the low-level MIDI output callback function for
 *      MIDIMon.  This module also contains the LibMain() and WEP() 
 *      DLL routines, and other functions accessed by the callback.
 *
 *      Because this module contains a low-level callback function,
 *      this entire module must reside in a FIXED code segment in a DLL.
 *      The data segment must be FIXED as well, since it is accessed by
 *      the callback.
 */

#include <windows.h>
#include <mmsystem.h>
#include "fddll.h"

long readinum(char far* *midiPtr);
UINT to16bits(unsigned char c1, unsigned char c2);
long to32bits(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4);

#pragma alloc_text(FAR_BSS,MidiOutputHandler,readinum,to16bits,to32bits,MidiInitialize,MidiEnable)

MIDI_CHANNEL	midiChannels[16];
BOOL					MusicWanted;
long					division, rational;
UINT					format, tracks;
int						midiHoldOff=60;
char					far *midiOut;
MIDIHDR				far *mhOut;

/*
 *  LibMain - Generic for a DLL.  Just initializes a little local memory.
 */
int FAR PASCAL LibMain (hInstance, wDataSeg, wHeapSize, lpCmdLine)
HANDLE hInstance ;
WORD   wDataSeg, wHeapSize ;
LPSTR  lpCmdLine ;
{
    // Nothing to do - SDK Libentry does the LocalInit

    return TRUE;    
}

/*
 *  WEP - Generic for a DLL.  Doesn't do a whole lot.
 */
void FAR PASCAL _WEP(wParam)
WORD wParam;
{
}

long readinum(char far**);

/* midiOutputHandler - Low-level callback function to handle MIDI output.
 *      Installed by midiInOpen().  The output handler takes incoming
 *      MIDI events and places them in the circular output buffer.  It then
 *      notifies the application by posting a MM_MIDIOUTPUT message.
 *
 *      This function is accessed at interrupt time, so it should be as 
 *      fast and efficient as possible.  You can't make any
 *      Windows calls here, except PostMessage().  The only Multimedia
 *      Windows call you can make are timeGetSystemTime(), midiOutShortMsg().
 *      
 *
 * Param:   hMidiOut - Handle for the associated output device.
 *          wMsg - One of the MOM_***** messages.
 *          hmOut - HMIDIOUT data.
 *
 * Return:  void
 */     

void CALLBACK MidiOutputHandler(
UINT wTimerID,
UINT wMsg,
DWORD hmOut,
DWORD dw1,
DWORD dw2)
{
char						mtype;
unsigned char		status, cmd1, cmd2, m[5];
int							index, expect, chans;
int							i;
UINT						mmstat;
long						tdelay, cnt;
DWORD						msg;
static char far	*midiPtr;
static UINT			sequence;
static long			timer, tempo;
//	static long			keysig, smpte, timesig;
/* This array is indexed by the high half of a status byte.  It's */
/* value is either the number of bytes needed (1 or 2) for a channel */
/* message, or 0 (meaning it's not  a channel message). */
static int chantype[]={	0, 0, 0, 0, 0, 0, 0, 0,		/* 0x00 through 0x70 */
												2, 2, 2, 2, 1, 1, 2, 0};	/* 0x80 through 0xf0 */

	if (!MusicWanted) return;

	if (midiHoldOff)
	{
		midiHoldOff--;
		if (midiHoldOff) return;
		for (index=0 ; index < 16 ; index++)
		{
			midiChannels[index].ptr = midiChannels[index].StartPtr;
			midiChannels[index].cmd = 0;
			midiChannels[index].delay = 0;
			//midiChannels[index].fudge = 0;
		}
		timer = 0;
	}
	else
		timer += rational;
	
	for (index=0,chans=16 ; index < 16 ; index++)
	{
		//	Make sure we are working with an active channel
		midiPtr = midiChannels[index].ptr;
		if (midiPtr == NULL) continue;

		do
		{
			//	Make sure we have data left to use
			if (midiPtr >= midiChannels[index].EndPtr) break;
			chans--;	//	activity indicator

			//	Wait through the present delay
			if (timer < midiChannels[index].delay)
			{
				break;
			}

			if (!midiChannels[index].cmd)
			{
				//	Check if we need to delay
				tdelay = readinum((char far **)&midiPtr);
				midiChannels[index].ptr = midiPtr;
				if (tdelay)
				{
					midiChannels[index].delay = timer + tdelay - (timer - midiChannels[index].delay);
					midiChannels[index].cmd = 1;
					break;
				}
			}
			else
				midiChannels[index].cmd = 0;
				

			status = (unsigned char)*midiPtr;
			if (status & 0x80)
			{
				midiChannels[index].status = status;
				midiPtr++;
			}
			else
			{	//	Running status
				status = midiChannels[index].status;
			}

			if (status < MIDI_SYSEX)		//	channel command
			{
				expect = chantype[status >> 4];
				cmd1 = (unsigned char)*midiPtr++;
				cmd2 = (expect == 2) ? (unsigned char)*midiPtr++ : 0;
        	
				msg = (DWORD) status | ((DWORD)cmd1 << 8) | ((DWORD)cmd2 << 16);
        	
//				execute the command
				mmstat = midiOutShortMsg((HMIDIOUT)(WORD)hmOut, msg);
        	
				midiChannels[index].ptr = midiPtr;
				continue;
			}
			//	Handle all the SYSEX messages
			if (status >= MIDI_SYSEX)			//	0xF0	System exclusive message
			{
				if (status <= MIDI_EOX)
				{
					cnt = readinum((char far **)&midiPtr);
					mhOut->lpData = midiChannels[index].ptr;
					mhOut->dwFlags = 0;
					midiPtr += cnt;
					mhOut->dwBufferLength = midiPtr - midiChannels[index].ptr;
					mmstat = midiOutPrepareHeader((HMIDIOUT)(WORD)hmOut, mhOut, sizeof(MIDIHDR));
					mmstat = midiOutLongMsg((HMIDIOUT)(WORD)hmOut, mhOut, sizeof(MIDIHDR));
					mmstat = midiOutUnprepareHeader((HMIDIOUT)(WORD)hmOut, mhOut, sizeof(MIDIHDR));
					midiChannels[index].ptr = midiPtr;
					continue;
				}
				if (status == MIDI_SYSTEM_RESET)	//	Meta Event
				{
					mtype = *midiPtr++;
					cnt = readinum((char far **)&midiPtr);
					for (i=0 ; i < cnt ; i++, midiPtr++) if (i < 5) m[i] = *midiPtr;
					midiChannels[index].ptr = midiPtr;
					switch (mtype)
					{
        	case 0x00:			/* Sequence number */
        		sequence = to16bits(m[0],m[1]);
          	break;
        	case 0x01:      /* Text event */
        	case 0x02:      /* Copyright notice */
        	case 0x03:      /* Sequence/Track name */
        	case 0x04:      /* Instrument name */
        	case 0x05:      /* Lyric */
        	case 0x06:      /* Marker */
        	case 0x07:      /* Cue point */
        	case 0x08:
        	case 0x09:
        	case 0x0a:
        	case 0x0b:
        	case 0x0c:
        	case 0x0d:
        	case 0x0e:
        	case 0x0f:			/* These are all text events */
          	break;
        	case 0x2f:      /* End of Track */
                break;
        	case 0x51:      /* Set tempo */
                tempo = to32bits(0,m[0],m[1],m[2]);
                rational = ((division * 25000L) / tempo);
                if (!rational) rational = 1L;
                break;
        	case 0x54:			/* SMPTE */
                //	smpte(m[0],m[1],m[2],m[3],m[4]);
                break;
        	case 0x58:			/* Time Sig */
                //	timesig(m[0],m[1],m[2],m[3]);
                break;
        	case 0x59:			/* Key Sig */
                //	keysig(m[0],m[1]);
                break;
        	case 0x7f:			/* Sequence specific */
                break;
					default:
						break;
					}
					continue;
				}
			}
		}
		while(1);
	}
	if (chans == 16) midiHoldOff = 60;	//	wait before restarting
}

/* readinum - read a varying-length midi number, and return the number. */
long readinum(char far* *midiPtr)
{
char far	*ptr;
int			c;
long		value;

	ptr = *midiPtr;
  c = *ptr++;;
  value = c & 0x7f;
  if (c & 0x80)
  {
   	do
   	{
     	c = *ptr++;
      value = (value << 7) + (c & 0x7f);
    }
    while (c & 0x80);
  }
  *midiPtr = ptr;
  return (value);
}

UINT to16bits(unsigned char c1, unsigned char c2)
{
UINT		val;
	val = (UINT) c1;
	val <<= 8;
	val |= (UINT) c2;
	return(val);
}

long to32bits(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4)
{
long		val;
	val = (long) c1;
	val <<= 8;
	val |= (long) c2;
	val <<= 8;
	val |= (long) c3;
	val <<= 8;
	val |= (long) c4;
	return(val);
}

void CALLBACK MidiInitialize(
MIDIHDR far	*lpmhOut,
char far 		*lpmidiOut,
long				mmfnum)
{
char far	*midiPtr,far *midiEnd;
int				index;
long			mtype, len;

	mhOut = lpmhOut;
	midiOut = lpmidiOut;

	midiPtr = midiOut;
	midiEnd = midiOut + mmfnum;
	for (index=0 ; index < 16 ; index++)	//	Find all the channels
	{
		if (midiPtr >= midiEnd) break;
		mtype = *((unsigned long far *)midiPtr);
		midiPtr += 4;
		len = *((unsigned long far *)midiPtr);
		mmfnum = (long)(unsigned char) *midiPtr++;
		mmfnum <<= 8;
		mmfnum |= (long)(unsigned char) *midiPtr++;
		mmfnum <<= 8;
		mmfnum |= (long)(unsigned char) *midiPtr++;
		mmfnum <<= 8;
		mmfnum |= (long)(unsigned char) *midiPtr++;
		switch (mtype)
		{
		case 0x6468544d:	//	'MThd'
			//	Basic info
			format = (UINT)*midiPtr++ << 8;
			format |= (UINT)*midiPtr++;
			tracks = (UINT)*midiPtr++ << 8;
			tracks |= (UINT)*midiPtr++;
			division = (long)*midiPtr++ << 8;
			division |= (long)*midiPtr++;
      if (!division) division = 1L;
      rational = 0L;
      rational = ((division * 25000L) / 50000L);	//	default tempo = 50000
			index--;		//	No channel used
			break;
		case 0x6b72544d:	//	'MTrk'
			midiChannels[index].StartPtr = midiPtr;
			midiChannels[index].EndPtr = midiPtr + mmfnum;
			midiChannels[index].ptr = midiPtr;
			midiChannels[index].delay = 0;
			//midiChannels[index].fudge = 0;
			midiPtr += mmfnum;
			break;
		case 0x50534944:	//	'DISP'
		case 0x5453494c:	//	'LIST'
		default:
			index--;		//	No channel used
			midiPtr += len;
			break;
		}
	}
	for ( ; index < 16 ; index++)
	{
		midiChannels[index].StartPtr = NULL;
		midiChannels[index].ptr = NULL;
	}

	return;
}

void CALLBACK MidiEnable(BOOL Active, HMIDIOUT hmOut)
{
	MusicWanted = Active;
	if (Active)
		midiHoldOff = 60;
	else
	{
		if (hmOut)	//	Only if midi is active, shut it down
			midiOutReset(hmOut);
	}
}
