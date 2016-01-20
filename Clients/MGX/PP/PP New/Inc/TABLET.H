//  (c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
//  This material is confidential and a trade secret.
//  Permission to use this work for any purpose must be obtained
//  in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#ifndef TABLET_H
#define TABLET_H

extern "C"
{
#include "wintab.h"
#include "wintabx.h"
}
#define PACKETDATA  ( PK_CURSOR | PK_X | PK_Y | PK_BUTTONS | PK_NORMAL_PRESSURE )
#define PACKETMODE  0
extern "C"
{
#include "pktdef.h"
}

//////////////////////////////////////////////////////////////////////////////
//                                  Macros                                  //
//////////////////////////////////////////////////////////////////////////////
#define NPACKETQSIZE    250 

///////////////////////////////// Local data /////////////////////////////////
typedef struct tabletdata 
{
	WORD  Pressure;
    int   x;
    int   y;
}   TABLETDATA, FAR *LPTABLETDATA;

//////////////////////////////////////////////////////////////////////////////
//                                 Functions                                //
//////////////////////////////////////////////////////////////////////////////
BOOL TabletStart( void );
BOOL TabletInit( void );
LPTABLETDATA GetNextPacket( BOOL fRemove );
int SetTabletQueueSize( int NewSize );
int GetTabletQueueSize( void );
HCTX GetTabletContextHandle( void );
BOOL GetPacketList( UINT *, UINT * ); 
void FlushTabletQueue( UINT );
BOOL IsPenPressed( void );
BOOL TabletEnd( void );

#endif  // TABLET_H
