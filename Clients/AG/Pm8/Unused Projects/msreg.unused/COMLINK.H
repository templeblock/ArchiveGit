/*
// $Workfile: COMLINK.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/COMLINK.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.2   28 Apr 1997 15:48:52   Fred
// Changed include to not reference V drive
// 
//    Rev 1.1   11 Dec 1995 16:30:58   JAY
// Win32s stuff
// 
//    Rev 1.0   22 Nov 1995 08:51:54   FRED
// Initial revision.
// 
//    Rev 1.1   30 Oct 1995 13:07:30   FRED
// 
//    Rev 1.0   27 Oct 1995 16:34:44   JAY
// Initial revision.
*/ 

#ifndef __COMLINK_H__
#define __COMLINK_H__

#include ".\gcpp300\h\compplib.hpp"

/*
// Our wrapper for the GFComLink-derived class.
// This allows us to customize the GFModem class as necessary.
*/

#ifdef WIN32
class CComLink : public GFComWin32
#else
class CComLink : public GFComWin3
#endif
{
public:
	CComLink(int nPort);
	BOOL Valid(void)
		{ return handle >= 0; }
	int LastError(void)
		{ return lasterror; }
protected:
	virtual int SystemIdle(void);
};

#ifdef WIN32
//
// Our local copy of this with the "GCPP_CLASS_TYPE" taken off.
// We don't want to require linking with this, we just want to allow access
// to the data.
//

class GFComWin32s : public GFComWin32
{
  friend class GCPP_CLASS_TYPE GFSerial;
  friend class GCPP_CLASS_TYPE GFDataFormat;
  friend class GCPP_CLASS_TYPE GFLineStatus;

  public:
    GCPP_PROTO GFComWin32s( int com = COM1,
                          unsigned rxsize = GCPP_RXSIZE,
                          unsigned txsize = GCPP_TXSIZE );
    GCPP_PROTO GFComWin32s( GFComWin32s GCPP_DLL_FAR &prev );
    GCPP_PROTO ~GFComWin32s( void );
    void GCPP_PROTO SetEventCharacter( char event_character );
    int GCPP_PROTO GetEventMask( unsigned long &event_signal_mask );
    int GCPP_PROTO SetEventMask( unsigned long event_signal_mask );
    int GCPP_PROTO WaitForEvent( unsigned long &event_signal_mask );

  protected:
    void GCPP_PROTO Flush( void );
    int  GCPP_PROTO GetLine( enum Lines line );
    void GCPP_PROTO Initialize( int com );
    int GCPP_PROTO Open( void );
    int GCPP_PROTO Close( void );
    void GCPP_PROTO GetBufferInfo( GFLinkInfo &sbc,
              int status_flag = GCPP_RXINFO | GCPP_TXINFO | GCPP_FLOWINFO );
    int GCPP_PROTO NewLinkSize( unsigned insize,
                                unsigned inroom,
                                unsigned outsize = 0 );
    int GCPP_PROTO SetHandShake( int inhand, int outhand );
    int GCPP_PROTO GetHandShake( int GCPP_DLL_FAR &inhand,
                                 int GCPP_DLL_FAR &outhand );
    int GCPP_PROTO LineBreak( int state );
    int GCPP_PROTO LineControl( int line, int state = -1 );
    int GCPP_PROTO SetDataFormat( long bd, char pr, int dl, int st );
    void GCPP_PROTO GetDataFormat( long GCPP_DLL_FAR &bd,
                                   char GCPP_DLL_FAR &pr,
                                   int GCPP_DLL_FAR &dl,
                                   int GCPP_DLL_FAR &st );
    void GCPP_PROTO UpdateLineStatus( GFlinestat GCPP_DLL_FAR &line );
    int GCPP_PROTO GetChar( void );
    unsigned GCPP_PROTO GetBuffer( unsigned char GCPP_DLL_FAR *buffer,
                                   unsigned count );
    int GCPP_PROTO PutChar( unsigned char b );
    unsigned GCPP_PROTO PutString( const unsigned char GCPP_DLL_FAR *string );
    unsigned GCPP_PROTO PutBuffer( const unsigned char GCPP_DLL_FAR *buffer,
                                   unsigned count );
    int GCPP_PROTO PutImm( unsigned char b );
    int GCPP_PROTO ClearLink( int dir, unsigned numbytes = 0 );
    int GCPP_PROTO TranslateWin32ErrorCode( void );
    void GCPP_PROTO ReportLinkStatus( VIEWER_OUTPUT ViewerOutput );
    virtual int GCPP_PROTO SystemIdle( void );

public:
    long CommHandle;
};
#endif

#endif
