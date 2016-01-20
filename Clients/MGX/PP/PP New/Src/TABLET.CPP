//////////////////////////////////////////////////////////////////////////////
//  (c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.               //
//  This material is confidential and a trade secret.                       //
//  Permission to use this work for any purpose must be obtained            //
//  in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081      //
//////////////////////////////////////////////////////////////////////////////
#include "pp.h"
#include "id.h"
#include "tablet.h"

///////////////////////////////// Global Data ////////////////////////////////
static HCTX hContext    = NULL;
TABLETDATA TabletData;

/////////////////////////////// Local Functions //////////////////////////////
LOCAL void PrsInit( void );
LOCAL LPTABLETDATA PrsAdjust( PACKET );

//////////////////////////////////////////////////////////////////////////////
BOOL TabletStart()
//  This function must be called before any mouse activity to stop the Wintab
//  default context from preempting packet activity.
//////////////////////////////////////////////////////////////////////////////
{
    LOGCONTEXT lcPictPub;
    BOOL bSuccess = FALSE;

    if( Control.UseTablet && ( hContext == NULL ))  // context not created yet
    {
        WORD errormode = SetErrorMode( SEM_NOOPENFILEERRORBOX );

	    // check if WinTab available
	    bSuccess = WTInfo( 0, 0, NULL );
        if( !bSuccess )
            return( FALSE ); 

        SetErrorMode( errormode );

    	// get default region
	    bSuccess = WTInfo( WTI_DEFSYSCTX, 0, &lcPictPub );
        if( !bSuccess )
            return( FALSE ); 

	    // modify the digitizing region
	    lstrcpy( lcPictPub.lcName, "PPLogContext" );
	    lcPictPub.lcOptions     |= CXO_MESSAGES;
	    lcPictPub.lcPktData     = PACKETDATA;
	    lcPictPub.lcPktMode     = PACKETMODE;
	    lcPictPub.lcMoveMask    = PACKETDATA;
	    lcPictPub.lcBtnUpMask   = lcPictPub.lcBtnDnMask;
	    lcPictPub.lcOutOrgX     = lcPictPub.lcOutOrgY = 0;

        // X axis origin is the same as in Windows.
        // Y axis is reversed because WinTab origin is at lower-left
	    lcPictPub.lcOutExtX     =  GetSystemMetrics( SM_CXSCREEN );
	    lcPictPub.lcOutExtY     = -GetSystemMetrics( SM_CYSCREEN );

	    // open the region
	    hContext = WTOpen( AfxGetMainWnd()->GetSafeHwnd(), &lcPictPub, TRUE );

        if( !hContext || !( SetTabletQueueSize( NPACKETQSIZE )))
            return( FALSE );
    }
    return( Control.UseTablet );
}

//////////////////////////////////////////////////////////////////////////////
BOOL TabletInit()
//  Returns TRUE if a tablet context exists.
//////////////////////////////////////////////////////////////////////////////
{
    if( hContext )
    {
        UINT Old = 0, New = 0;
        int nResult;

        // get serial numbers of packets in the queue
        nResult = WTQueuePacketsEx( hContext, &Old, &New );

        if( !nResult )
            return FALSE;   // queue is empty for some reason
    }
	return( hContext != NULL );
}

//////////////////////////////////////////////////////////////////////////////
int SetTabletQueueSize( int NewSize )
//////////////////////////////////////////////////////////////////////////////
{
    BOOL bSuccess = FALSE;

    if( !hContext )
        return 0;

    // Set queue size if you can.
    // Start with NewSize and reduce as necessary to get a fit.
    while( NewSize )
    {
       	bSuccess = WTQueueSizeSet( hContext, NewSize );

        if( bSuccess )              
            break;                  // quit if successful

        NewSize--;                  // else try smaller size
    }
    return NewSize;
}

//////////////////////////////////////////////////////////////////////////////
int GetTabletQueueSize()
//////////////////////////////////////////////////////////////////////////////
{
    if( hContext )
        return( WTQueueSizeGet( hContext ));
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
HCTX GetTabletContextHandle()
//////////////////////////////////////////////////////////////////////////////
{
    return hContext;
}

//////////////////////////////////////////////////////////////////////////////
BOOL IsPenPressed()
// this doesn't work 
//////////////////////////////////////////////////////////////////////////////
{
    return NO;
}

//////////////////////////////////////////////////////////////////////////////
void FlushTabletQueue( UINT Keep )
// removes all packets from the queue except the last "Left" packets
//////////////////////////////////////////////////////////////////////////////
{
    if( hContext )
    {
        UINT Old = 0, New = 0;
        int nNum;

        // get serial numbers of packets in the queue
        if( WTQueuePacketsEx( hContext, &Old, &New ))
            WTDataGet( hContext, Old, New - Keep, NPACKETQSIZE, NULL, &nNum );
    }
}

//////////////////////////////////////////////////////////////////////////////
BOOL TabletEnd()
//  Tablet context should remain active until the application is closed.
//////////////////////////////////////////////////////////////////////////////
{
    if( hContext )
    {
        WTClose( hContext );
        hContext = NULL;

#ifndef WIN32
        if( Control.bUnlinkWintab )
            _UnlinkWintab();   // free wintab.dll and reset auto-linking
                               // capabilities in Wintabx/Wintab32x.lib
#endif
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
LPTABLETDATA GetNextPacket( BOOL fRemove )
//  Gets next packet from the queue.
//  If fRemove == TRUE, the packet is removed from the queue.
//  Otherwise, it is only copied from the queue.  
//////////////////////////////////////////////////////////////////////////////
{
    PACKET Packet;
    int numpkts;

    memset( &TabletData, 0, sizeof( TABLETDATA ));

    // bring context to front
    WTOverlap( hContext, TRUE );    

    if( fRemove )
        numpkts = WTPacketsGet( hContext, 1, &Packet );
    else
        numpkts = WTPacketsPeek( hContext, 1, &Packet );

    if( numpkts )
        return( PrsAdjust( Packet ));   // return new pressure and position
    else
        return( &TabletData );
}

//  pressure adjuster local state
static UINT wActiveCsr = 0, wOldCsr = -1;
static UINT wPrsBtn;
static UINT prsYesBtnOrg, prsYesBtnExt, prsNoBtnOrg, prsNoBtnExt;

//////////////////////////////////////////////////////////////////////////////
//  The functions PrsInit and PrsAdjust make sure that our pressure out can //
//  always reach the full 0-100 range we desire, regardless of the button   //
//  pressed or the "pressure button marks" settings.                        //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
LOCAL LPTABLETDATA PrsAdjust( PACKET p )
//////////////////////////////////////////////////////////////////////////////
{
	wActiveCsr = p.pkCursor;

	if( wActiveCsr != wOldCsr ) 
    {
		// re-init on cursor change. 
		PrsInit();
		wOldCsr = wActiveCsr;
	}

    TabletData.x = ( int )p.pkX;
    TabletData.y = ( int )p.pkY;

	// scaling output range is 0-100 
	if( p.pkButtons & ( 1 << wPrsBtn )) 
    {
		// if pressure-activated button is down, scale pressure output to 
        // compensate btn marks.
		TabletData.Pressure = p.pkNormalPressure - prsYesBtnOrg;
		TabletData.Pressure = MulDiv( TabletData.Pressure, 100, prsYesBtnExt );
	}
	else 
    {
		// if pressure-activated button is not down, scale pressure output 
        // directly.
		TabletData.Pressure = p.pkNormalPressure - prsNoBtnOrg;
		TabletData.Pressure = MulDiv( TabletData.Pressure, 100, prsNoBtnExt );
	}
    TabletData.Pressure = bound( TabletData.Pressure, 0, 100 );

	return( &TabletData );
}

//////////////////////////////////////////////////////////////////////////////
LOCAL void PrsInit()
//////////////////////////////////////////////////////////////////////////////
{
	AXIS np;
	LOGCONTEXT lc;
	BYTE logBtns[32];
	UINT btnMarks[2];
	UINT size;

	// discover the LOGICAL button generated by the pressure channel
	// get the PHYSICAL button from the cursor category and run it 
	// through that cursor's button map ( usually the identity map )
	wPrsBtn = ( UINT ) - 1;
	WTInfo( WTI_CURSORS + wActiveCsr, CSR_NPBUTTON, &wPrsBtn );
	size = WTInfo( WTI_CURSORS + wActiveCsr, CSR_BUTTONMAP, &logBtns );
	if( wPrsBtn < size )
		wPrsBtn = logBtns[ wPrsBtn ];

	// get the current context for its device variable
	WTGet( hContext, &lc );

	// get the size of the pressure axis
	WTInfo( WTI_DEVICES + lc.lcDevice, DVC_NPRESSURE, &np );
	prsNoBtnOrg = ( UINT )np.axMin;
	prsNoBtnExt = ( UINT )np.axMax - ( UINT )np.axMin;

	// get the button marks ( up & down generation thresholds ) 
	// and calculate what pressure range we get when pressure-button is down.
	WTInfo( WTI_CURSORS + wActiveCsr, CSR_NPBTNMARKS, btnMarks );
	prsYesBtnOrg = btnMarks[ 1 ];
	prsYesBtnExt = ( UINT )np.axMax - btnMarks[ 1 ];
}
