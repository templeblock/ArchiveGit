// ( c ) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// needed for windows definitions
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "bsrutil.h"
#include "calib.h"
#include "twain.h"                  // needed for TWAIN definitions
#include "dca_glue.h"               // for function prototypes of glue code
#include "dca_acq.h"                // contain TWAIN sample support code

// Constants for images that have unknown dimensions 
#define HS_MAXWIDTH  11             // Maximum width of an image.
#define HS_MAXLENGTH 11             // Maximum length of an image.

// Globals from other modules that are needed here
extern DSMENTRYPROC lpDSM_Entry;    // function pointer to Source Mang. entry
extern TW_IDENTITY  appID, dsID;    // access to identity structs from glue

extern void twainError( int id );
extern int twainGetFileFormat();
extern int twainNegotiatePixelTypes();
extern int twainGetTransferType();
extern int twainImageLoad();

static unsigned int  twainTransferType;
static unsigned int  twainFileFormat;
static unsigned int  twainPixelFlavor;
static pTW_PALETTE8  lpTwainPalette = ( pTW_PALETTE8 )( NULL );
static BOOL fInvert;

extern BOOL bTwainUIEnabled;



/**************************************************************************
 * FUNCTION: DCAcquire
 *
 * ARGS:    None
 * RETURNS: None
 *
 * NOTES:   - opendsm, open the Source Manager
 *          - opends, open the Source
 *          - enable Source
 *          - wait for a message from Source ( usually XFERREADY )
*************************************************************************/
VOID DCAcquire()
{
    if( bTwainUIEnabled ) 
    {
	    DCDisableDS();
		if( DCEnableDS() == FALSE )
		{
			DCCloseDS();
		}
		return;
	}

	if( DCOpenDSM() == TRUE )
    {
		// Please note that another Source may change the system default while
		// your not looking and simply getting the default will not guarentee
		// you get what you want. Suggest saving the dsID structure privately
		// after you open it to assure subsequent connections are to your intended
		// Source -- ed
		// Also note the the DSM will maintain a static list of all Sources in
		// the system at the moment it is opened.  Changes in available Sources
		// while the DSM is open may cause unpredictable results. -- ed
		//
		// force the opends to give me the system default

		dsID.Id = 0;
		dsID.ProductName[0] = 0;

		if( DCOpenDS() == TRUE ) 
        {
			if( !twainGetTransferType()) 
            {
				DCTerminate();
				return;
			}

			// An EnableDS error should close the DS
			if( DCEnableDS() == FALSE )
            {
				DCCloseDS();
			}
		}
	}
	return;
}

/**************************************************************************
	Function:       getEnumerationValue
**************************************************************************/
long getEnumerationValue( pTW_ENUMERATION pEnumeration, int j )
{
	LPVOID pItemList;
	long returnValue = 0;

	if( pEnumeration != NULL )
    {
		// assign base address of ItemList array to 'generic' pointer
		pItemList = ( LPVOID )pEnumeration->ItemList;

		// CAST to type of var caller wants
		switch( pEnumeration->ItemType )
        {
			case TWTY_INT8   : 
                returnValue = *(( pTW_INT8 ) pItemList+j ); 
                break;

			case TWTY_UINT8  : 
                returnValue = *(( pTW_UINT8 ) pItemList+j ); 
                break;

			case TWTY_INT16  : 
                returnValue = *(( pTW_INT16 ) pItemList+j ); 
                break;

			case TWTY_UINT16 : 
                returnValue = *(( pTW_UINT16 )pItemList+j ); 
                break;

			case TWTY_INT32  : 
                    returnValue = *(( pTW_INT32 ) pItemList+j ); 
                    break;

			case TWTY_UINT32 : 
                returnValue = *(( pTW_UINT32 )pItemList+j ); 
                break;

			case TWTY_BOOL   : 
                returnValue = *(( pTW_BOOL )  pItemList+j ); 
                break;

			default:        
                break;
		}
	}
	return( returnValue );
}

/**************************************************************************
	Function:       twainGetTransferType
**************************************************************************/
int twainGetTransferType()
{
	TW_CAPABILITY Capability;   // For capability negotiation
	pTW_ENUMERATION pEnumeration;
	pTW_ONEVALUE pOneValue;
	long value;
	int ret_val;
	int i, accept;

	// Find out what transfer types are supported
	Capability.Cap = ICAP_XFERMECH;
	accept = 0;

	if(( *lpDSM_Entry )( &appID, &dsID,DG_CONTROL, DAT_CAPABILITY, MSG_GET,
		( TW_MEMREF )&Capability ) != TWRC_SUCCESS )
    {
		twainError( IDS_CAPGETFAILED );
		return( -1 );
	}

	if( Capability.ConType == TWON_ONEVALUE ) 
    {
		if(( pOneValue = ( pTW_ONEVALUE )GlobalLock( Capability.hContainer )) == NULL ) 
        {
			GlobalFree( Capability.hContainer );
			return( FALSE );
		}

		switch( pOneValue->Item ) 
        {
			case TWSX_NATIVE : accept |= 1; break;
			case TWSX_FILE   : accept |= 2; break;
			case TWSX_MEMORY : accept |= 4; break;
		}
	} 
    else 
    {
		if(( pEnumeration = ( pTW_ENUMERATION )GlobalLock( Capability.hContainer )) == NULL ) 
        {
			GlobalFree( Capability.hContainer );
			return( FALSE );
		}

		for( i=0;i<pEnumeration->NumItems;i++ ) 
        {
			value = getEnumerationValue( pEnumeration, i );

			switch( value ) 
            {
				case TWSX_NATIVE : accept |= 1; break;
				case TWSX_FILE   : accept |= 2; break;
				case TWSX_MEMORY : accept |= 4; break;
			}
		}
	}

	GlobalUnlock( Capability.hContainer );
	GlobalFree( Capability.hContainer );

	ret_val = TRUE;

	// If in memory is not supported 
	if( accept&4 ) 
    {
		twainTransferType = TWSX_MEMORY;
		ret_val = twainNegotiatePixelTypes();
	} 
    else 
    {
		if( accept&2 ) 
        {
			twainTransferType = TWSX_FILE;
			ret_val = twainGetFileFormat();
		} 
        else 
        {
			twainTransferType = TWSX_NATIVE;
		}
	}

	if( ret_val ) 
    {
		Capability.Cap        = ICAP_XFERMECH;
		Capability.ConType    = TWON_ONEVALUE;
		Capability.hContainer = GlobalAlloc( GMEM_MOVEABLE, sizeof( TW_ONEVALUE ));

		pOneValue = ( pTW_ONEVALUE )GlobalLock( Capability.hContainer );
		pOneValue->ItemType   = TWTY_UINT16;
		pOneValue->Item       = twainTransferType;

		GlobalUnlock( Capability.hContainer );

		if(( *lpDSM_Entry )( &appID, &dsID, DG_CONTROL, DAT_CAPABILITY, MSG_SET,
			( TW_MEMREF )&Capability ) != TWRC_SUCCESS )
        {
			ret_val = FALSE;
		}
	}
	return( ret_val );
}

/**************************************************************************
	Function:       twainGetFileFormat
**************************************************************************/
int twainGetFileFormat()
{
	TW_CAPABILITY Capability;       // For capability negotiation
	pTW_ENUMERATION pEnumeration;
	long value;
	int i, accept;

	// Find out what file format types are supported 
	Capability.Cap = ICAP_IMAGEFILEFORMAT;
	accept = 0;

	if(( *lpDSM_Entry )( &appID, &dsID, DG_CONTROL, DAT_CAPABILITY, MSG_GET,
		( TW_MEMREF )&Capability ) != TWRC_SUCCESS )
    {
		twainError( IDS_CAPGETFAILED );
		return( FALSE );
	}
	
	if( Capability.ConType == TWON_ONEVALUE ) 
    {
		pTW_ONEVALUE pOneValue;

		if(( pOneValue = ( pTW_ONEVALUE )GlobalLock( Capability.hContainer )) == NULL ) 
        {
			twainError( IDS_CAPGETFAILED );
			GlobalFree( Capability.hContainer );
			return( FALSE );
		}

		switch( pOneValue->Item ) 
        {
			case TWFF_TIFF   : accept |= 1; break;
			case TWFF_BMP    : accept |= 2; break;
		}
	} 
    else 
    {
		if(( pEnumeration = ( pTW_ENUMERATION )GlobalLock( Capability.hContainer )) == NULL ) 
        {
			twainError( IDS_CAPGETFAILED );
			GlobalFree( Capability.hContainer );
			return( FALSE );
		}

		for( i=0;i<pEnumeration->NumItems;i++ ) 
        {
			value = getEnumerationValue( pEnumeration, i );

			switch( value ) 
            {
				case TWFF_TIFF   : accept |= 1; break;
				case TWFF_BMP    : accept |= 2; break;
			}
		}
	}

	GlobalUnlock( Capability.hContainer );
	GlobalFree( Capability.hContainer );
								
	if( accept & 1 ) 
    {
		twainFileFormat = TWFF_TIFF;
	} 
    else 
    {
		if( accept & 2 ) 
        {
			twainFileFormat = TWFF_BMP;
		} 
        else 
        {
			// Iff neither of our file formats is supported
			twainTransferType = TWSX_NATIVE;
		}
	}
	return( TRUE );
}

/**************************************************************************
	Function:       twainNegotiatePixelTypes
**************************************************************************/
int twainNegotiatePixelTypes()
{
	TW_CAPABILITY Capability;       // For capability negotiation 
	pTW_ENUMERATION pEnumeration;
	pTW_ONEVALUE pOneValue;
	pTW_UINT16 pItemList;
	long value;
	int i, accept;

	// Find out what pixel types are supported 
	Capability.Cap = ICAP_PIXELTYPE;
	accept = 0;

	if(( *lpDSM_Entry )( &appID, &dsID, DG_CONTROL, DAT_CAPABILITY, MSG_GET,
		( TW_MEMREF )&Capability ) != TWRC_SUCCESS )
    {
		twainError( IDS_CAPGETFAILED );
		return( FALSE );
	}

	if( Capability.ConType == TWON_ONEVALUE ) 
    {
		pTW_ONEVALUE pOneValue;

		if(( pOneValue = ( pTW_ONEVALUE )GlobalLock( Capability.hContainer )) == NULL ) 
        {
			twainError( IDS_CAPGETFAILED );
			GlobalFree( Capability.hContainer );
			return( FALSE );
		}

		switch( pOneValue->Item ) 
        {
			case TWPT_BW      : accept |= 0x001; break;
			case TWPT_GRAY    : accept |= 0x002; break;
			case TWPT_RGB     : accept |= 0x004; break;
			case TWPT_PALETTE : accept |= 0x008; break;
			case TWPT_CMY     : accept |= 0x010; break;
			case TWPT_CMYK    : accept |= 0x020; break;
			case TWPT_YUV     : accept |= 0x040; break;
			case TWPT_CIEABC  : accept |= 0x080; break;
			case TWPT_CIEXYZ  : accept |= 0x100; break;
		}
	} 
    else 
    {
		if(( pEnumeration = ( pTW_ENUMERATION )GlobalLock( Capability.hContainer )) == NULL ) 
        {
			twainError( IDS_CAPGETFAILED );
			GlobalFree( Capability.hContainer );
			return( FALSE );
		}

		for( i=0;i<pEnumeration->NumItems;i++ ) 
        {
			value = getEnumerationValue( pEnumeration, i );

			switch( value ) 
            {
				case TWPT_BW      : accept |= 0x001; break;
				case TWPT_GRAY    : accept |= 0x002; break;
				case TWPT_RGB     : accept |= 0x004; break;
				case TWPT_PALETTE : accept |= 0x008; break;
				case TWPT_CMY     : accept |= 0x010; break;
				case TWPT_CMYK    : accept |= 0x020; break;
				case TWPT_YUV     : accept |= 0x040; break;
				case TWPT_CIEABC  : accept |= 0x080; break;
				case TWPT_CIEXYZ  : accept |= 0x100; break;
			}
		}
	}

	GlobalUnlock( Capability.hContainer );
	GlobalFree( Capability.hContainer );

	// Check to see if there are any unsupported types.
	if( accept & 0xFFF0 ) 
    {
		// Make sure that we can accept at least one of the types.
		if( !( accept & 0x000F )) 
        {
			twainError( IDS_CAPSETFAILED );
			GlobalUnlock( Capability.hContainer );
			GlobalFree( Capability.hContainer );
			return( FALSE );
		}

		Capability.Cap     = ICAP_PIXELTYPE;
		Capability.ConType = TWON_ENUMERATION;

		// Alloc enough memory for our max number of pixel types.( 4 ) + 1
		Capability.hContainer = GlobalAlloc( GMEM_MOVEABLE,  
			sizeof( TW_ENUMERATION )- sizeof( TW_UINT8 ) + ( sizeof( TW_UINT16 ) * 5 ));
					
		pEnumeration = ( pTW_ENUMERATION )GlobalLock( Capability.hContainer );
		pItemList = ( pTW_UINT16 )pEnumeration->ItemList;

		pEnumeration->ItemType     = TWTY_UINT16;
		pEnumeration->CurrentIndex = 0;
		pEnumeration->DefaultIndex = 0;
		pEnumeration->NumItems     = 0;

		// Only allow our accepted pixel types.

		if( accept & 1 ) pItemList[pEnumeration->NumItems++] = TWPT_BW;
		if( accept & 2 ) pItemList[pEnumeration->NumItems++] = TWPT_GRAY;
		if( accept & 4 ) pItemList[pEnumeration->NumItems++] = TWPT_RGB;
		if( accept & 8 ) pItemList[pEnumeration->NumItems++] = TWPT_PALETTE;
									
		if(( *lpDSM_Entry )( &appID, &dsID, DG_CONTROL, DAT_CAPABILITY, MSG_SET,
			( TW_MEMREF )&Capability ) != TWRC_SUCCESS )
        {
			twainError( IDS_CAPSETFAILED );
			GlobalUnlock( Capability.hContainer );
			GlobalFree( Capability.hContainer );
			return( FALSE );
		}
	}

	// Find out the device's pixel flavor 
	Capability.Cap = ICAP_PIXELFLAVOR;

	if(( *lpDSM_Entry )( &appID, &dsID, DG_CONTROL, DAT_CAPABILITY, MSG_GET,
		( TW_MEMREF )&Capability ) != TWRC_SUCCESS )
    {
		twainPixelFlavor = TWPF_CHOCOLATE;
		return( TRUE );
	} 

    BOOL bEnumerated = FALSE;
	if( Capability.ConType == TWON_ONEVALUE ) 
    {
		if(( pOneValue = ( pTW_ONEVALUE )GlobalLock( Capability.hContainer )) == NULL )
        {
			twainError( IDS_CAPGETFAILED );
			GlobalFree( Capability.hContainer );
			return( FALSE );
	    }
		twainPixelFlavor = pOneValue->Item;
	} 
    else 
    {
        bEnumerated = TRUE;
		if(( pEnumeration = ( pTW_ENUMERATION )GlobalLock( Capability.hContainer )) == NULL ) 
        {
			twainError( IDS_CAPGETFAILED );
			GlobalFree( Capability.hContainer );
			return( FALSE );
	    }

		accept = 0;

	    for( i=0;i<pEnumeration->NumItems;i++ ) 
        {
		    value = getEnumerationValue( pEnumeration, i );

		    switch( value ) 
            {
			    case TWPF_CHOCOLATE : accept |= 0x01; break;
			    case TWPF_VANILLA   : accept |= 0x02; break;
		    }
	    }
	    if( accept & 1 ) 
		    twainPixelFlavor = TWPF_CHOCOLATE;
        else
	    if( accept & 2 ) 
		    twainPixelFlavor = TWPF_VANILLA;
        else 
		    twainPixelFlavor = TWPF_CHOCOLATE;
    }

	GlobalUnlock( Capability.hContainer );
	GlobalFree( Capability.hContainer );

	Capability.Cap        = ICAP_PIXELFLAVOR;
	Capability.ConType    = TWON_ONEVALUE;
	Capability.hContainer = GlobalAlloc( GMEM_MOVEABLE, sizeof( TW_ONEVALUE ));

	pOneValue = ( pTW_ONEVALUE )GlobalLock( Capability.hContainer );
	pOneValue->ItemType   = TWTY_BOOL;
	pOneValue->Item       = twainPixelFlavor;

	GlobalUnlock( Capability.hContainer );

    if((( *lpDSM_Entry )( &appID, &dsID, DG_CONTROL, DAT_CAPABILITY, MSG_SET, 
        ( TW_MEMREF )&Capability ) != TWRC_SUCCESS ) && bEnumerated )
    {
        // Complain that you can't set the pixel flavor unless the device 
        // doesn't allow enumerated flavors. Earlier versions of the Twain 
        // spec didn't require or allow you to negotiate pixel flavors.
		twainError( IDS_CAPSETFAILED );
		GlobalFree( Capability.hContainer );
		return( FALSE );

	}
	fInvert = twainPixelFlavor == TWPF_VANILLA;

    // MGX twain driver tells you falsely that it is sending inverted data 
    // for gray scale, color scans. Change INI setting to reset invert flag.
	if( Scans.ScanLAInvert )     
    	fInvert = !fInvert;     

	return( TRUE );
}

/**************************************************************************
 *
 * FUNCTION: ProcessTWMessage
 *
 * ARGS:    lpMsg  Pointer to Windows msg retrieved by GetMessage
 *          hWnd   Application's main window handle
 *
 * RETURNS: TRUE  if application should process message as usual
 *          FALSE if application should skip processing of this message
 *
 * NOTES:   1 ). be sure both Source Manager and Source are open
 *          2 ). two way message traffic:
 *              - relay windows messages down to Source's modeless dialog
 *              - retrieve TWAIN messages from the Source
 *
 * COMMENT: ProcessSourceMessage is designed for applications that can only
 * have one Source open at a time.  If you wish your application to have more
 * than one Source open at a time please consult the TWAIN ToolKit for
 * event handling instructions.
 *
**************************************************************************/
BOOL ProcessTWMessage( LPMSG lpMsg, HWND hWnd )
{
	TW_UINT16  dcRC=TWRC_NOTDSEVENT;
	TW_EVENT   dcEvent;

	// Only ask Source Manager to process event if there is a Source connected.

	if(( DCIsDSMOpen()) &&( DCIsDSOpen()))
    {
	    // A Source provides a modeless dialog box as its user interface.
		// The following call relays Windows messages down to the Source's
		// UI that were intended for its dialog box.  It also retrieves TWAIN
		// messages sent from the Source to our  Application.

		dcEvent.pEvent = ( TW_MEMREF )lpMsg;
		dcRC = ( *lpDSM_Entry )( &appID, &dsID, DG_CONTROL, DAT_EVENT, 
                              MSG_PROCESSEVENT,( TW_MEMREF )&dcEvent );

		switch( dcEvent.TWMessage )
        {
			case MSG_XFERREADY:  
                DCTransferImage( hWnd ); 
                break;

			case MSG_CLOSEDSREQ: 
                DCTerminate(); 
                break;

			case MSG_NULL:      // No message from the Source to the App break;
			default:            // possible new message
                break;      
		}
	}

	// tell the caller what happened
	return( dcRC==TWRC_DSEVENT );
}

/**************************************************************************
 *
 * FUNCTION: DCTransferImage
 *
 * ARGS:    hWnd
 *
 * RETURNS: none
 *
 * NOTES:   1 ). delete any bit maps laying around
 *          2 ). mention those who do not want Native need CAP nego. ICAP_XFERMECH
 *          3 ). get a little information about image, for form, I do not use it
 *          4 ). set up a for form loop to pull image( s ) from the Source
 *          5 ). call for GetCompleteImage from Source
 *          6 ). be sure to send a MSG_ENDXFER as a seperator between images
 *          7 ). after the images are transfered I like to shut down the Source
 *              DCTerminate
 *
 * COMMENTS: Setup for a transfer in the routine called as a response to
 * XFERREADY.  Then has a nested loop do/while on the routine which
 * actually pulls in the image or GetCompleteImage.  The GetCompleteImage
 * routine also deals with the cancel, xferdone, success messages from
 * Source.
 *
**************************************************************************/
VOID DCTransferImage( HWND hWnd )
{
	TW_PENDINGXFERS dcPendingXfer;

	int image_count = 0;
	int transfer_ok;

	// explicitly initialize the our flags
	dcPendingXfer.Count = 0;

	// Ask Source if more images are coming.  I only support one image per transfer
	// in this example but do/while loop is included to help? those who wish to
	// support more than one.

	do 
    {
		// Tell the Source what type of transfer you want.
		switch( twainTransferType ) 
        {
			case TWSX_NATIVE : 
				transfer_ok = GetCompleteNativeImage( hWnd ); 
    			break;

			case TWSX_FILE   : 
				TW_SETUPFILEXFER  dcFileSetup;
				char number[32];

				wsprintf( number, "%05d", image_count );
				dcFileSetup.Format = twainFileFormat;

				switch( twainFileFormat ) 
                {
					case TWFF_TIFF : 
						GetFileLocation( IDN_TIFF, dcFileSetup.FileName );
						stripfile( dcFileSetup.FileName );
						lstrcat( dcFileSetup.FileName, "TWN" );
						lstrcat( dcFileSetup.FileName, number );
						lstrcat( dcFileSetup.FileName, ".TIF" );
					    break;

					case TWFF_BMP  : 
						GetFileLocation( IDN_BMP, dcFileSetup.FileName );
						stripfile( dcFileSetup.FileName );
						lstrcat( dcFileSetup.FileName, "TWN" );
						lstrcat( dcFileSetup.FileName, number );
						lstrcat( dcFileSetup.FileName, ".BMP" );
					    break;

					default : 
						twainError( IDS_MEMXFERFAILED );
						DCTerminate();
						return;
					    break;
				}
				image_count++;

				if(( *lpDSM_Entry )( &appID, &dsID, DG_CONTROL, DAT_SETUPFILEXFER, 
                                    MSG_SET,( TW_MEMREF )&dcFileSetup ) != TWRC_SUCCESS )
                {
					twainError( IDS_MEMXFERFAILED );
					return;
				}
				transfer_ok = GetCompleteFileImage( hWnd, dcFileSetup ); 
			    break;

			case TWSX_MEMORY : 
				TW_SETUPMEMXFER dcMemSetup;

				if(( *lpDSM_Entry )( &appID, &dsID,DG_CONTROL, DAT_SETUPMEMXFER, 
                                    MSG_GET,( TW_MEMREF )&dcMemSetup ) != TWRC_SUCCESS )
                {
					twainError( IDS_MEMXFERFAILED );
					return;
				}

				// Since we will return a length of the preferred size,
				// make sure that it is within our limits.
				if( dcMemSetup.Preferred > 65536L ) 
                    dcMemSetup.Preferred = 65536L;

				transfer_ok = GetCompleteMemoryImage( hWnd, dcMemSetup ); 
			    break;
		}

		if( transfer_ok ) 
        {
			// Required for proper 6<->7 state transitions
			if(( *lpDSM_Entry )( &appID, &dsID, DG_CONTROL, DAT_PENDINGXFERS, 
                               MSG_ENDXFER,( TW_MEMREF )&dcPendingXfer ) != TWRC_SUCCESS )
            {
				dcPendingXfer.Count = 0;  // trash remaining images
			}
		}

	} while( transfer_ok && ( dcPendingXfer.Count != 0 ));

	if( lpTwainPalette != ( pTW_PALETTE8 )NULL )
	{
		FreeUp(( LPTR )lpTwainPalette );
		lpTwainPalette = ( pTW_PALETTE8 )NULL;
	}

    // source could be shut down here after each image, but dialog is left up
    // until shut down by the user or app is terminated
    // DCTerminate();

	return;
}

/**************************************************************************
	Function:       GetCompleteNativeImage
**************************************************************************/
int GetCompleteNativeImage( HWND hWnd )
{
	TW_UINT32 hBitMap;
	TW_UINT16 dcRC;
	
	dcRC = TWRC_SUCCESS;
	while( dcRC != TWRC_XFERDONE ) 
    {
		dcRC = ( *lpDSM_Entry )( &appID, &dsID,
				DG_IMAGE, DAT_IMAGENATIVEXFER, MSG_GET,( TW_MEMREF )&hBitMap );

		switch( dcRC ) 
        {
			case TWRC_SUCCESS:
			case TWRC_XFERDONE: break;

			case TWRC_CANCEL:
			case TWRC_FAILURE:

			default: 
				// something wrong, abort the transfer and delete the image
				if( dcRC == TWRC_FAILURE ) 
                {
					twainError( IDS_TWAINFAILURE );
				}
				return( FALSE );
			    break;
		}
	}
	SendMessage( hWnd, PM_XFERDONE, IDC_SAVETWAINDIB,( unsigned long )hBitMap );

	return( TRUE );
}

/**************************************************************************
	Function:       GetCompleteFileImage
**************************************************************************/
int GetCompleteFileImage( HWND hWnd, TW_SETUPFILEXFER dcFileSetup )
{
	TW_UINT16 dcRC;

	dcRC = TWRC_SUCCESS;
	while( dcRC != TWRC_XFERDONE ) 
    {
		dcRC = ( *lpDSM_Entry )( &appID, &dsID,
				DG_IMAGE, DAT_IMAGEFILEXFER, MSG_GET, NULL );

		switch( dcRC ) 
        {
			case TWRC_SUCCESS:
			case TWRC_XFERDONE: break;

			case TWRC_CANCEL:
			case TWRC_FAILURE:

			default: 
				// something wrong, abort the transfer and delete the image
				if( dcRC == TWRC_FAILURE ) 
                {
					twainError( IDS_TWAINFAILURE );
				}
				return( FALSE );
			    break;
		}
	}
	SendMessage( hWnd, PM_XFERDONE, IDC_SAVETWAINFILE, ( long )( LPTR )&dcFileSetup );

	return( TRUE );
}

/**************************************************************************
	Function:       GetCompleteMemoryImage
**************************************************************************/
int ImagePlane;

int GetCompleteMemoryImage( HWND hWnd, TW_SETUPMEMXFER dcMemSetup )
{
	TW_IMAGEINFO   dcImageInfo;
	TW_IMAGEMEMXFER dcImageData;
	TW_UINT16 dcRC;
	LPTR lpBuffer = NULL;
	LPFRAME lpFrame;
	int imageWidth;
	int need_new;
	int save_type, expand;
	FRMDATATYPE Type;

	dcImageData.Memory.Flags  = TWMF_APPOWNS | TWMF_POINTER;
	dcImageData.Memory.Length = dcMemSetup.Preferred;
	dcImageData.Memory.TheMem = LineBuffer[0];

	// Get the image information, nice to know a little about the 
    // image the Source will be sending
	if( !( *lpDSM_Entry )( &appID, &dsID, DG_IMAGE, DAT_IMAGEINFO, MSG_GET,
			( TW_MEMREF )&dcImageInfo ) == TWRC_SUCCESS )
    {
		return( FALSE );
	}

	// We have the information about the type/size/depth of the image
	// Stored in dcImageInfo.
	imageWidth = dcImageInfo.ImageWidth;
	need_new = FALSE;   // If we have to shrink the image.( hand scanners )

	if( dcImageInfo.ImageLength == -1 ) 
    {
		// Unknown length - hand scanner 
		dcImageInfo.ImageLength = HS_MAXLENGTH * dcImageInfo.YResolution.Whole;
		need_new = TRUE;
	}

	if( dcImageInfo.ImageWidth == -1 ) 
    {
		// Unknown width - hand scanner 
		dcImageInfo.ImageWidth = HS_MAXWIDTH * dcImageInfo.XResolution.Whole;
		need_new = TRUE;
	}

	expand = 0;

	lpTwainPalette = ( pTW_PALETTE8 )NULL;

	// Find out the proper file save type for this image.
	switch( dcImageInfo.PixelType ) 
    {
		case TWPT_BW      : 
			if( Control.LineArtAsGray ) 
            {
				// Create Continuous tone
				Type = FDT_GRAYSCALE;
				save_type = IDC_SAVECT;
				expand = 1;
			} 
            else 
            {
				// Line Art
				Type = FDT_LINEART;
				save_type = IDC_SAVELA;
			}
		    break;

		case TWPT_GRAY    : 
			Type = FDT_GRAYSCALE;
			save_type = IDC_SAVECT;
		    break;

		case TWPT_RGB     : 
			Type = FDT_RGBCOLOR;
			save_type = IDC_SAVE24BITCOLOR;
		    break;

		case TWPT_PALETTE :
			lpTwainPalette = ( pTW_PALETTE8 )Alloc( sizeof( TW_PALETTE8 ) +
										( 256 * sizeof( TW_ELEMENT8 )));

			if( lpTwainPalette == ( pTW_PALETTE8 )NULL )
				return( FALSE );

			if( !( *lpDSM_Entry )( &appID, &dsID, DG_IMAGE, DAT_PALETTE8, 
                MSG_GET,( TW_MEMREF )lpTwainPalette ) == TWRC_SUCCESS )
            {
			
				twainError( IDS_UNSUPPORTEDPIXTYPE );
				return( FALSE );
			}

			if( lpTwainPalette->PaletteType != TWPA_RGB ) 
            {
				twainError( IDS_UNSUPPORTEDPIXTYPE );
				return( FALSE );
			}

			Type = FDT_PALETTECOLOR;
			save_type = IDC_SAVE8BITCOLOR;
		    break;

		default : 
			twainError( IDS_INVALIDPIXTYPE );
			return( FALSE );
		    break;
	}

	// Open the new frame.          
	if( !( lpFrame = FrameOpen( 
			Type,
			( WORD )imageWidth,
			( WORD )dcImageInfo.ImageLength,
			( WORD )dcImageInfo.XResolution.Whole ))) 
    {
		// Issue the error message 
		Message( IDS_ESCROPEN,( LPTR )Control.RamDisk );
		return( FALSE );
	}

	if( Type == FDT_PALETTECOLOR )
	{
		int i;
		LPCOLORMAP lpColorMap = FrameGetColorMap( lpFrame );

		int NumColors = lpTwainPalette->NumColors;
		if( NumColors > 256 )
			NumColors = 256;

		for( i = 0; i < NumColors; ++i )
		{
			lpColorMap->RGBData[i].red = lpTwainPalette->Colors[i].Channel1;			
			lpColorMap->RGBData[i].green = lpTwainPalette->Colors[i].Channel2;			
			lpColorMap->RGBData[i].blue = lpTwainPalette->Colors[i].Channel3;			

            if( fInvert )
            {
			    lpColorMap->RGBData[i].red   ^= 0xFF;
			    lpColorMap->RGBData[i].green ^= 0xFF;
			    lpColorMap->RGBData[i].blue  ^= 0xFF;
            }
		}
        lpColorMap->NumEntries = NumColors;
	}

	if( dcImageInfo.Planar ) 
        ImagePlane = 0;

	while( dcRC != TWRC_XFERDONE ) 
    {
		dcRC = ( *lpDSM_Entry )( &appID, &dsID,
				DG_IMAGE, DAT_IMAGEMEMXFER, MSG_GET,( TW_MEMREF )&dcImageData );

		switch( dcRC ) 
        {
			case TWRC_SUCCESS:
			case TWRC_XFERDONE: 
				if( dcImageData.YOffset == 0 ) 
                    ImagePlane++;

				StoreImageRect( lpFrame, dcImageInfo, dcImageData, Type, expand );
			    break;

			case TWRC_CANCEL:
			case TWRC_FAILURE:

			default: 
				// something wrong, abort the transfer and delete the image
				if( dcRC == TWRC_FAILURE ) 
                {
					twainError( IDS_TWAINFAILURE );
				}

				// Close the garbage frame 
				FrameClose( lpFrame );

				return( FALSE );
			    break;
		}
	}

	// Bring up the new image window.
	SendMessage( hWnd, PM_XFERDONE, 
               ( unsigned short )save_type,( unsigned long )lpFrame );

	return( TRUE );
}

/**************************************************************************
	Function:       StoreImageRect
**************************************************************************/
void StoreImageRect( LPFRAME lpFrame, TW_IMAGEINFO imageInfo, 
                     TW_IMAGEMEMXFER imageData, FRMDATATYPE Type, int expand )
{
	register int i, j;
	BYTE Lut4to8[] = { 0,17,34,51,68,85,102,119,136,153,170,187,204,221,238,255 };

	LPTR pSrc, pDst;

	pSrc = ( LPTR )imageData.Memory.TheMem;

	if( imageData.BytesWritten <= 0 ) 
        return;

	for( i = 0; i < imageData.Rows; i++ ) 
    {
		pDst = FramePointerRaw( lpFrame, ( int )( 0 ), ( int )( i+imageData.YOffset ), ( BOOL )( YES ));

		switch( Type ) 
        {
			case FDT_LINEART :      // Bi-Level Data 
				pDst += ( imageData.XOffset + 7 ) / 8;
	
				j = imageData.BytesPerRow;

				if( fInvert ) 
                {
					while( j-- ) 
                    {
						*pDst++ = ( *pSrc++ ) ^ 0xFF;
					}
				} 
                else 
                {
					copy( pSrc, pDst,( int )( imageData.BytesPerRow ));
					pSrc += imageData.BytesPerRow;
				}
			    break;

			case FDT_PALETTECOLOR : // Palette Color Data 
			case FDT_GRAYSCALE :    // Gray Scale Data
				if( expand ) 
                {
					pDst += imageData.XOffset;
/*
					la2con( pSrc,( int )( imageData.Columns ), pDst, 
						 ( BOOL )(( twainPixelFlavor == TWPF_CHOCOLATE ) ? YES : NO ));
*/
					la2con( pSrc,( int )( imageData.Columns ), pDst, 
						 ( BOOL )( fInvert ? NO : YES ));

                    pSrc += imageData.BytesPerRow;
				} 
                else 
                {
					switch( imageInfo.BitsPerSample[ 0 ] ) 
                    {
						case 4 : 
							pDst += ( imageData.XOffset );
							j = imageData.BytesPerRow;

							if( Type == FDT_PALETTECOLOR )
							{
                				// if the image is inverted, color tables are
                                // inverted. The image data is NOT inverted.
                                while( j-- )
								{
									*pDst++ = (( *pSrc ) & 0xF0 ) >> 4;
									*pDst++ = ( *pSrc++ ) & 0x0F;
								}
							}
							else
							{
                                if( fInvert )
                                {
                                    while( j-- )
								    {
									    *pDst++ = ( Lut4to8[(( *pSrc ) & 0xF0 ) >> 4 ] ) ^ 0xFF;
									    *pDst++ = ( Lut4to8[( *pSrc++ ) & 0x0F ] ) ^ 0xFF;
								    }
                                }
                                else
                                {
								    while( j-- )
								    {
									    *pDst++ = Lut4to8[(( *pSrc ) & 0xF0 ) >> 4 ];
									    *pDst++ = Lut4to8[( *pSrc++ ) & 0x0F ];
								    }
                                }
							}
						    break;

						case 8 : 
							pDst += ( imageData.XOffset );

                			// if the image is inverted, color tables are
                            // inverted. The image data is NOT inverted.
							if(( Type == FDT_GRAYSCALE ) && fInvert )
       							not( pSrc, pDst,( int )( imageData.BytesPerRow ));
                            else
       							copy( pSrc, pDst,( int )( imageData.BytesPerRow ));

							pSrc += imageData.BytesPerRow;
						    break;
					}
				}
			    break;

			case FDT_RGBCOLOR : 
				pDst += ( imageData.XOffset * 3 );

				if( imageInfo.Planar ) 
                {
					j = imageData.BytesPerRow;

					switch( ImagePlane ) 
                    {
						case 1 :        // Red   Plane
                            break;      
						case 2 :        // Green Plane
                            pDst++;  
                            break;      
						case 3 :        // Blue  Plane
                            pDst += 2; 
                            break; 
					}

                    if( fInvert )
                    {
					    while( j-- ) 
                        {
						    *pDst = *pSrc ^ 0xFF;
                            *pSrc++;
						    pDst += 3;
					    }
                    }
                    else
                    {
					    while( j-- ) 
                        {
						    *pDst = *pSrc++;
						    pDst += 3;
					    }
                    }
				} 
                else 
                {
                    if( fInvert )
                    {
					    not( pSrc, pDst,( int )( imageData.BytesPerRow ));
					    pSrc += imageData.BytesPerRow;  
                    }
                    else
                    {
					    copy( pSrc, pDst,( int )( imageData.BytesPerRow ));
					    pSrc += imageData.BytesPerRow;  
                    }
				}
			    break;
		}
	}
}

/**************************************************************************
	Function:       ApplyCalMapToFrame
**************************************************************************/
static LPFRAME ApplyCalMapToFrame( LPFRAME lpFrame )
{
	LPTR  lpData;
	LPRGB lpRGB;
	LPTR  lpLut1, lpLut2, lpLut3, lpSrc, lpDst;
	int   iCount;
	int   y;
	LPFRAME		lpDstFrame, lpRtnFrame = lpFrame;
	FRMDATATYPE Type;
	FRMTYPEINFO	inType, outType;
	CFrameTypeConvert	Convert;

	inType  = ColorManager.Scan.src;
	outType = ColorManager.Scan.dst;

	Type = FrameType( lpFrame );

	if( Type != FDT_GRAYSCALE && Type != FDT_RGBCOLOR )
		return( lpFrame );

	if( !Scans.fScanUseCMS && !LoadScanCalibrate( Names.ScanMap ))
		return( lpFrame );

	if( Control.CMSEnabled &&
		Scans.fScanUseCMS &&
		!IsSrcPTSelected( &inType.ptInfo ) &&
		!IsDstPTSelected( &outType.ptInfo ))
		return( lpFrame );

	if( FrameType( lpFrame ) == FDT_GRAYSCALE )
	{
		lpLut1 = Scans.CalMap[CALSCAN_GRAY].Lut;

		ProgressBegin( 1, 0 );
		for( y=0;y<FrameYSize( lpFrame );y++ )
		{
			if( AstralClockCursor( y, FrameYSize( lpFrame ), TRUE ))
			{
				FrameClose( lpFrame );
				return( NULL );
			}
			lpData = FramePointer( lpFrame, 0, y, TRUE );
			if( !lpData )
				continue;

			iCount = FrameXSize( lpFrame );

			while( iCount-- > 0 )
			{
				*lpData++ = lpLut1[*lpData];
			}
		}
		ProgressEnd();
		lpRtnFrame = lpFrame;
	}
	else
	{
		if( Control.CMSEnabled && Scans.fScanUseCMS )
		{
			lpDstFrame = FrameOpen( 
				outType,
				( WORD )FrameXSize( lpFrame ),
				( WORD )FrameYSize( lpFrame ),
				( WORD )FrameResolution( lpFrame ));
			if( !lpDstFrame )
			{
				Message( IDS_ESCROPEN,( LPTR )Control.RamDisk );
				return( lpFrame );
			}

			iCount = FrameXSize( lpFrame );

			Convert.Init( inType, outType, iCount );

			ProgressBegin( 1, 0 );

			for( y=0;y<FrameYSize( lpFrame );y++ )
			{
				if( AstralClockCursor( y, FrameYSize( lpFrame ), TRUE ))
				{
					FrameClose( lpFrame );
					FrameClose( lpDstFrame );
					return( NULL );
				}
				lpSrc = ( LPTR )FramePointer( lpFrame, 0, y, FALSE );
				if( !lpSrc )
					continue;

				lpDst = ( LPTR )FramePointer( lpDstFrame, 0, y, TRUE );
				if( !lpDst )
					continue;

				Convert.ConvertData( lpSrc, lpDst, y, iCount );
			}
			ProgressEnd();
			FrameClose( lpFrame );
			lpRtnFrame = lpDstFrame;
		}
		else
		{
			lpLut1 = Scans.CalMap[CALSCAN_RED].Lut;
			lpLut2 = Scans.CalMap[CALSCAN_GREEN].Lut;
			lpLut3 = Scans.CalMap[CALSCAN_BLUE].Lut;

			ProgressBegin( 1, 0 );
			for( y = 0; y < FrameYSize( lpFrame ); y++ )
			{
				if( AstralClockCursor( y, FrameYSize( lpFrame ), TRUE ))
				{
					FrameClose( lpFrame );
					return( NULL );
				}
				lpRGB = ( LPRGB )FramePointer( lpFrame, 0, y, TRUE );
				if( !lpRGB )
					continue;

				iCount = FrameXSize( lpFrame );
				while( iCount-- > 0 )
				{
					lpRGB->red   = lpLut1[lpRGB->red];
					lpRGB->green = lpLut2[lpRGB->green];
					lpRGB->blue  = lpLut3[lpRGB->blue];
					lpRGB++;
				}
			}
			ProgressEnd();
			lpRtnFrame = lpFrame;
		}
	}
return( lpRtnFrame );
}

/**************************************************************************
	Function:       OpenImageWindow
**************************************************************************/
void OpenImageWindow( HINSTANCE hInstance, HWND hWindow, LPVOID lpVoid,
                      int DataType )
{
	LPFRAME lpFrame;

	if( lpVoid != NULL ) 
    {
		switch( DataType ) 
        {
			case IDC_SAVETWAINDIB : 
				HANDLE hBitMap;
				LPTR   lpBitMap;

				hBitMap  = ( HANDLE )(( long )lpVoid );
				lpBitMap = ( LPTR )GlobalLock( hBitMap );

				lpFrame = DIBToFrame( lpBitMap, TRUE );

				GlobalUnlock( hBitMap );
				GlobalFree( hBitMap );
			    break;

			case IDC_SAVETWAINFILE : 
				pTW_SETUPFILEXFER pDCFileSetup;

				pDCFileSetup = ( pTW_SETUPFILEXFER )lpVoid;
				
				switch( pDCFileSetup->Format ) 
                {
					case TWFF_TIFF : 
						twainImageLoad( IDN_TIFF, pDCFileSetup->FileName );
						return;
					    break;

					case TWFF_BMP : 
						twainImageLoad( IDN_BMP, pDCFileSetup->FileName );
						return;
					    break;
				}
			    break;

			case IDC_SAVECT :
			case IDC_SAVELA :
			case IDC_SAVE8BITCOLOR :
			case IDC_SAVE24BITCOLOR : 
				lpFrame = ( LPFRAME )lpVoid;
			    break;

			default : 
				return;
			    break;
		}
	} 
    else 
    {
		// User Must have cancelled or encountered an error.
		return;
	}

	if( lpFrame == NULL )
	{
		return;
	}

	// Make sure we apply the scanner calibration map 
	if( !( lpFrame = ApplyCalMapToFrame( lpFrame )))
		return;

	// Setup the new image and bring up the new image window 
    LPIMAGE lpNewImage = CreateImage( NULL, lpFrame, NULL, 
        NULL, Control.DefaultFileType, DataType, IMG_DOCUMENT, NULL );

    if( lpNewImage )
    {
        if( PictPubApp.OpenDocumentFile(( LPSTR )lpNewImage->CurFile,
           lpNewImage ))         
        {
           	// only version of image is in the cache
        	// so insure user is asked about saving when done
            lpNewImage->fChanged = TRUE;
			PictPubApp.DocumentChanged( lpNewImage );
        }
        else
            DestroyImage( lpNewImage );
    }
}

/**************************************************************************
	Function:       twainError
**************************************************************************/
void twainError( int id )
{
	char twainErrorString[256];	
	char errorString[256];	

	AstralStrEx( IDS_TWAINERROR,( char far * )twainErrorString, 255 );
	AstralStrEx( id,( char far * )errorString, 255 );

	MessageBox( PictPubApp.Get_hWndAstral(), errorString, twainErrorString, MB_OK | MB_ICONEXCLAMATION );
}

/**************************************************************************
    Function:   twainImageLoad
**************************************************************************/
int twainImageLoad( ITEMID idFileType, LPSTR szName )
{
	LPSTR 		lpName;
	LPFRAME     lpFrame;
	STRING  	locName;
	FRMTYPEINFO	Type;

	lstrcpy( locName, szName );

	if( !OKToOpen( szName, &lpName ))
		return( FALSE );

	CReadImage read( szName );
	lpFrame = read.ReadFrame();

	if( !lpFrame ) 
    {
		return( FALSE );
	}

	idFileType = read.GetFileType();

	// Make sure we apply the scanner calibration map 
	// and do CMS if needed.
	if( !( lpFrame = ApplyCalMapToFrame( lpFrame )))
		return( FALSE );

	FrameGetTypeInfo( lpFrame, &Type );

	// Setup the new image and bring up the new image window 
    LPIMAGE lpNewImage = CreateImage( NULL, lpFrame, NULL, 
        NULL, idFileType, GetSaveDataType( Type ), IMG_DOCUMENT, NULL );

    if( lpNewImage )
    {
        if( PictPubApp.OpenDocumentFile(( LPSTR )lpNewImage->CurFile,
           lpNewImage ))         
        {
           	// only version of image is in the cache 
        	// so insure user is asked about saving when done
            lpNewImage->fChanged = TRUE;
			PictPubApp.DocumentChanged( lpNewImage );
        }
        else
            DestroyImage( lpNewImage );
    }
	// Now we can get rid of the file.
	FileDelete( locName );

	return( TRUE );
}

/**************************************************************************/
BOOL CheckTwainOK()
/**************************************************************************/
{
	#define       WINDIRPATHSIZE 160
	char          WinDir[WINDIRPATHSIZE];
	BOOL bReturn = FALSE;
	FNAME szTwainDir;
	FNAME szCurrentDir;

	GetWindowsDirectory (WinDir, WINDIRPATHSIZE);
	lstrcat (WinDir, "\\");
	lstrcpy(szTwainDir, WinDir);
#ifdef WIN32
	lstrcat (szTwainDir, "TWAIN32.DLL");
#else
	lstrcat (szTwainDir, "TWAIN.DLL");
#endif
	if (!FileExists(szTwainDir))
		return(FALSE);

	lstrcpy(szTwainDir, WinDir);
#ifdef WIN32
	lstrcat(szTwainDir, "TWAIN32");
#else
	lstrcat(szTwainDir, "TWAIN");
#endif

	GetCurrentDir(szCurrentDir, sizeof(FNAME));

	bReturn = SetCurrentDir(szTwainDir);

	SetCurrentDir( szCurrentDir );

	return(bReturn);
}


