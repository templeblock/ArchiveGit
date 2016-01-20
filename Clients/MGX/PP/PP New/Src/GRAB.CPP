// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯

#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "gdevice.h"

/* imports from the VGA driver */
extern "C" {
extern BOOL WINPROC EXPORT PicwinOff( LPSTR, unsigned, LPSTR, LPSTR, LPSTR );
extern BOOL WINPROC EXPORT PicwinOn( LPSTR );
}

// exported functions for grabbers 
int grabber_AstralClockCursor( int part, int whole );
LPFRAME grabber_frame_open( WORD Depth, int x_size, int y_size, int resolution );
void grabber_frame_close( LPFRAME lpFrame );
LPTR grabber_frame_ptr( int Depth, int x, int y, int bModify );
LPTR grabber_frame_set( LPTR lpFrame );

char palette1[512];

static BOOL bKeepDLL;

DEVINFO DevInfo;
DEVMSG DevMsg;
DEVGET DevData;

// dynamic procs of the Grabber dll

DEVNAMEPROC lpfnDevName;
DEVOPENPROC lpfnDevOpen;
DEVCLOSEPROC lpfnDevClose;
DEVCALLPROC lpfnDevCall;
DLGPROC lpfnAcquireProc;

#define DevName     (*lpfnDevName)
#define DevOpen     (*lpfnDevOpen)
#define DevCall     (*lpfnDevCall)
#define DevClose    (*lpfnDevClose)
#define AcquireProc (*lpfnAcquireProc)

/*======================================================================*\
	Function:	IsGrabberOK
\*======================================================================*/

BOOL IsGrabberOK( LPSTR lpExtName, LPSTR lpString )
{
	HMODULE hDLL;

	if ( !(hDLL = (HMODULE)LoadGrabberLibrary(lpExtName) ) )
		return( NO );
	
	if (!(lpfnDevName = (DEVNAMEPROC)GetProcAddress( hDLL, MAKEINTRESOURCE(301) ))){
		return( NO );
	}

	*lpString = 30;	// set version to 3.0

	if (!DevName((LPSTR)lpString ) || *lpString < 'A' || *lpString > 'z' ) {
		// No grabber name
		*lpString = '\0';
		return( NO );
	}

	return( YES );
}

/*======================================================================*\
	Function:	LoadGrabberLibrary
\*======================================================================*/

HANDLE LoadGrabberLibrary( LPSTR lpExtName )
{
	HMODULE hDLL;
	FNAME szDLL;
	static HMODULE hStaticDLL;
	static FNAME szStaticDLL;

	if ( !lpExtName )
		{ // A cleanup call
		if ( hStaticDLL )
			{
		  	FreeLibrary( hStaticDLL );
			hStaticDLL = NULL;
			szStaticDLL[0] = '\0';
			}
		return( NULL );
		}

	if ( !LookupExtFile( lpExtName, szDLL, IDN_SCANNER ) )
		return( NULL );

	if ( hStaticDLL )
		{ // If we have a DLL already loaded
		if ( !bKeepDLL || !StringsEqual( szStaticDLL, szDLL ) )
			{ // If we don't keep DLL's or its a new one...
		  	FreeLibrary( hStaticDLL );
			hStaticDLL = NULL;
			szStaticDLL[0] = '\0';
			}
		}

	if ( hStaticDLL )
		return( hStaticDLL );

	if ( !(hDLL = (HMODULE)AstralLoadLibrary(szDLL))  )
		return( NULL );
	hStaticDLL = hDLL;
	lstrcpy( szStaticDLL, szDLL );
	return( hDLL );
}


/*======================================================================*\
	Function:	back_to_windows
\*======================================================================*/

VOID back_to_windows( int flag )
{
	/* Don't go back to windows if we never left! */
	if (flag) {
		/* We're coming back from the picture window */
#ifndef WIN32
		PicwinOff((LPSTR)palette1,0,NULL,NULL,NULL); /* restore windows disp */
#endif
		ReleaseCapture( );

		AstralDlgHide( IDD_GRAB ); /* Force a windows redisplay */

		ShowCursor( TRUE );

		AstralDlgEnd( AstralDlgGet( IDD_GRAB ), TRUE );
	}
}

/*======================================================================*\
	Function:	grabber
\*======================================================================*/

void grabber( LPSTR lpExtName, int port, HINSTANCE hInstance, HWND hWindow )
{

	HWND hWnd;
/*
	special conditions:
		hand scan interface --	
			with hand scanners the dialog box has a scan
			frame which gets filled with image data.  the
			dialog box must remain up during reads, so we
			use the exported frame_open, cacheptr, and
			frame_close to create the frame instead of 
			device reads.  Devopen just returns devcaps,
			devstart does everything else.
	
		special willow gs --	
			the gs board has 4 different buffers, which
			contain the following: 0-even pix/even lines,
			1-odd pix/even lines, 2-even pix/odd lines,
			3-odd pix/odd lines.
			each call to devread will layer the data so
			that the buffers will be combined correctly.
	
	Device Requirements Byte Decoding:

		XXXXXXX1b =  need to take over display
		XXXXXXX0b =  no need to take over display
		XXXXXX1Xb =  special willow gs read
		XXXXXX0Xb =  no special read 
		XXXX00XXb =  image depth, special case palette color 8-bit
		XXXX01XXb =  depth == 1
		XXXX10XXb =  depth == 2
		XXXX11XXb =  depth == 3
		X1XXXXXXb = special hand scan i/f 
		X0XXXXXXb = standard grab i/f
		XX1XXXXXb = don't free DLL after exit
		XX0XXXXXb = free DLL after exit 
		1XXXXXXXb = putting data from cache to external device
		0XXXXXXXb = putting data from external device into cache
		*/

	BYTE devreq; // device requirements
	LPFRAME lpFrame, lpOldFrame;
	BYTE depth;
	int lncnt,Datatype;
	HMODULE hDLL;
	LPIMAGE lpImage;

	if ( !(hDLL = (HMODULE)LoadGrabberLibrary(lpExtName)) ) {
		Message( IDS_EBADDRIVER, NULL );
		return;
	}

	if ( !(lpImage = GetActiveImage()))
		lpFrame = NULL;
	else
		lpFrame = ImgGetBaseEditFrame(lpImage);

   lpfnDevName     = (DEVNAMEPROC)GetProcAddress(hDLL, MAKEINTRESOURCE(301));
   lpfnDevOpen     = (DEVOPENPROC)GetProcAddress(hDLL, MAKEINTRESOURCE(302));
   lpfnDevCall     = (DEVCALLPROC)GetProcAddress(hDLL, MAKEINTRESOURCE(303));
   lpfnDevClose    = (DEVCLOSEPROC)GetProcAddress(hDLL, MAKEINTRESOURCE(304));
   lpfnAcquireProc = (DLGPROC)GetProcAddress(hDLL, MAKEINTRESOURCE(305));

	/* get device's intentions -- is this a put to or read from cache */
	DevInfo.hInst = hInstance;  /* set the data structures */
	DevInfo.hPw = hWindow;
	DevInfo.bFile_is_open = ( lpFrame != NULL );

	if(DevInfo.bFile_is_open)
		{
		DevInfo.bImg_type = (lpImage->DataType==IDC_SAVECT ? TRUE : FALSE);
		/* pass the current filename */
		lstrcpy(DevInfo.cfname, lpImage->CurFile); 
		}

	if(!DevOpen(&DevInfo)) {
		DevCall(DEV_MSG, (LPTR)&DevMsg);
		
		/* if MsgNo == 0, then user cancelled */
		if(DevMsg.MsgNo) {
			if(DevMsg.MsgNo > 0) {
				Message( DevMsg.MsgNo );
			} else {
				/* print the driver's message */
				Print("%ls", (LPTR)DevMsg.szMsg);
			}
		}
		return; 
	}

	bKeepDLL = (DevInfo.device_caps & 0x20);
	devreq = DevInfo.device_caps;
	
	/* doing a put cache to an external device */
	if(!(devreq & 0x80)) {
		/* fill up the device info data structure */
		DevInfo.npix = FrameXSize( lpFrame );
		DevInfo.nlin = FrameYSize( lpFrame );
		DevInfo.bpl =  FrameXSize( lpFrame );
		DevInfo.xres = DevInfo.yres = FrameResolution( lpFrame );
		DevInfo.bpp = 8;
	} else {
		DevInfo.port = port; /* set the port for devices which require it */
#ifndef WIN32
		DevInfo.vgaaddr = GetA000H(); /* set the VGA buffer address */
#endif
	}

	/* see if we need to take over display */
	if(devreq & 1) {
		/* Hide the Cursor */
		ShowCursor( FALSE );

		/* Create the acquire window: it doesn't have the visable bit set */
		AstralDlg( YES, hInstance, hWindow, IDD_GRAB, AcquireProc);
		AstralDlgShow( IDD_GRAB );

		/* Enter the picture window command processor */
		SetCapture( AstralDlgGet( IDD_GRAB ) );

		/* set the data structures */
		DevInfo.hInst = hInstance;  
		DevInfo.hPw   = AstralDlgGet( IDD_GRAB );

		/* call display driver disable */
#ifndef WIN32
		PicwinOn((LPSTR)palette1);
#endif
	}

	if(devreq & 0x40) { // devstart does everything
		DevInfo.FpPtr = (LPTRPROC)MakeProcInstance(
			(FARPROC)grabber_frame_ptr, DevInfo.hInst);

		DevInfo.FoPtr = (LPTRPROC)MakeProcInstance(
			(FARPROC)grabber_frame_open, DevInfo.hInst);

		DevInfo.FsPtr = (LPTRPROC)MakeProcInstance(
			(FARPROC)grabber_frame_set, DevInfo.hInst);

		DevInfo.FcPtr = (LPROC)MakeProcInstance(
			(FARPROC)grabber_frame_close, DevInfo.hInst);

		DevInfo.CcPtr = (LPROC)MakeProcInstance(
			(FARPROC)grabber_AstralClockCursor, DevInfo.hInst);
	}

	DevInfo.bLineArtAsGray = Control.LineArtAsGray;
 
	if(!DevCall(DEV_START, (LPTR)&DevInfo)) { /* if get, fill struct */
		back_to_windows(devreq & 1); /* restore if true */

		DevCall(DEV_MSG, (LPTR)&DevMsg);

		/* if MsgNo == 0, then user cancelled */
		if(DevMsg.MsgNo) {
			if(DevMsg.MsgNo > 0) {
				Message( DevMsg.MsgNo );
			} else {
				/* print the driver's message */
				Print("%ls", (LPTR)DevMsg.szMsg);
			}
		}

		if(devreq & 0x40) { // devstart does everything
			FreeProcInstance((FARPROC)DevInfo.FpPtr);
			FreeProcInstance((FARPROC)DevInfo.FoPtr);
			FreeProcInstance((FARPROC)DevInfo.FsPtr);
			FreeProcInstance((FARPROC)DevInfo.FcPtr);
			FreeProcInstance((FARPROC)DevInfo.CcPtr);
		}

		DevClose(); /* close the device */
		return;
	}

	/* DEV_START returned ok, now get or put image data if not done */

	if((devreq & 0xc0) == 0x80) { // putting data into the cache with devreads
		if(((devreq & 4) == 4) || ((devreq & 0x0c) == 0)) {
			depth = 1;
		} else {
			if((devreq & 8) == 8) {
				depth = 2;
			} else {
				if((devreq & 0x0c) == 0x0c) {
					depth = 3;
				}
			}
		}

		lpOldFrame = frame_set( NULL );
	
		if(!(lpFrame = FrameOpen(
				(FRMDATATYPE)depth,DevInfo.npix,DevInfo.nlin,DevInfo.xres))) {

			back_to_windows(devreq & 1); /* Restore if true */
	
			FrameError(IDS_EIMAGEOPEN);
			return;
		}
	
		frame_set(lpFrame);
	
		if(!(devreq & 2))	{
			for ( lncnt=0; lncnt<DevInfo.nlin; lncnt++ ) {
				DevData.ImageAddress = FramePointer(lpFrame, 0, lncnt, YES);
				if(!DevCall(DEV_READ, (LPTR)&DevData)) {
		         back_to_windows(devreq & 1); /* Restore if true */
					DevCall(DEV_MSG, (LPTR)&DevMsg);
	
					/* if MsgNo == 0, then user cancelled */
					if(DevMsg.MsgNo) {
						Print("Unable to get data from device");
					} else {
						DevInfo.nlin = lncnt;  // hand scanner, get total lines
					}
					return;
				}
			}
		} else { /* special willow gs reader */
	
			/* read even bytes, even lines */
			for ( lncnt=0; lncnt<DevInfo.nlin; lncnt+=2 ) {
				DevData.ImageAddress = FramePointer(lpFrame, 0, lncnt, YES);
				if(!DevCall(DEV_READ0, (LPTR)&DevData)) {
					back_to_windows(devreq & 1); /* Restore if true */
					Print("Unable to get data from device");
					return;
				}
			}
	
			/* read odd bytes, even lines */
			for ( lncnt=0; lncnt<DevInfo.nlin; lncnt+=2 ) {
				DevData.ImageAddress = FramePointer(lpFrame, 1, lncnt, YES);
				if(!DevCall(DEV_READ1, (LPTR)&DevData)) {
					back_to_windows(devreq & 1); /* Restore if true */
					Print("Unable to get data from device");
					return;
				}
			}
	
			/* read even bytes, odd lines */
			for ( lncnt=1; lncnt<DevInfo.nlin; lncnt+=2 ) {
				DevData.ImageAddress = FramePointer(lpFrame, 0, lncnt, YES);
				if(!DevCall(DEV_READ2, (LPTR)&DevData)) {
					back_to_windows(devreq & 1); /* Restore if true */
					Print("Unable to get data from device");
					return;
				}
			}
	
			/* read odd bytes, odd lines */
			for ( lncnt=1; lncnt<DevInfo.nlin; lncnt+=2 ) {
				DevData.ImageAddress = FramePointer(lpFrame, 1, lncnt, YES);
				if(!DevCall(DEV_READ3, (LPTR)&DevData)) {
					back_to_windows(devreq & 1); /* Restore if true */
					Print("Unable to get data from device");
					return;
				}
			}
		}
		
		FrameClose( lpOldFrame );
	
		back_to_windows(devreq & 1); /* Restore if true */
	
		if(DevInfo.bpp == 1) {
			Datatype = IDC_SAVELA;
		} else {
			if(DevInfo.bpp == 8) {
				Datatype = IDC_SAVECT;
			} else {
				Datatype = IDC_SAVE24BITCOLOR;
			}
		}
	
		/* Setup the new image and bring up the new image window */
        LPIMAGE lpNewImage = CreateImage(NULL, lpFrame, NULL, 
           NULL, Control.DefaultFileType, Datatype, 
           IMG_DOCUMENT, NULL);
        if (lpNewImage)
        {
            if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
               lpNewImage))         
            {
               	/* only version of image is in the cache */
            	/* so insure user is asked about saving when done */
                lpNewImage->fChanged = TRUE;
            }
            else
                DestroyImage(lpNewImage);
        }
	
		DevClose(); /* close the device */
	} else {
        LPIMAGE lpNewImage = NULL;

		/* enable the frame created by DevStart */
		lpFrame = frame_set(NULL);

		/* putting data to external device with DevWrites */
		if((devreq & 0xc0) == 0) {
			for ( lncnt=0; lncnt<DevInfo.nlin; lncnt++ ) {
				AstralClockCursor( lncnt, DevInfo.nlin, NO );
				DevData.ImageAddress = FramePointer(lpFrame, 0, lncnt, NO);
				if(!DevCall(DEV_WRITE, (LPTR)&DevData)) {
					Print("Unable to put data to device");
					return;
				}
			}
		} else {
			/* devstart has created a frame with image data */
			FreeProcInstance((FARPROC)DevInfo.FpPtr);
			FreeProcInstance((FARPROC)DevInfo.FoPtr);
			FreeProcInstance((FARPROC)DevInfo.FsPtr);
			FreeProcInstance((FARPROC)DevInfo.FcPtr);

			if(DevInfo.bpp == 1) {
				Datatype = IDC_SAVELA;
			} else {
				if(DevInfo.bpp == 8) {
					Datatype = IDC_SAVECT;
				} else {
					Datatype = IDC_SAVE24BITCOLOR;
				}
			}

			back_to_windows(devreq & 1); /* Restore if true */

			/* enable the frame created by DevStart */
			lpFrame = frame_set(NULL);

			/* DevInfo.nlin returns the actual line count */
			if(FrameYSize(lpFrame) <= DevInfo.nlin) {
				/* Setup the new image and bring up the new image window */
            {
                lpNewImage = CreateImage(NULL, lpFrame, NULL, 
                   NULL, Control.DefaultFileType, Datatype, 
                   IMG_DOCUMENT, NULL);
                if (lpNewImage)
                {
                    if (!PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
                       lpNewImage))         
                    {
                        DestroyImage(lpNewImage);
                        lpNewImage = NULL;
                    }
                }
            }

			} else {
				/* create a new frame of the right size and copy  */
				/* the right number of lines to it */

				lpOldFrame = FrameOpen(
					FrameType(lpFrame),
					FrameXSize(lpFrame), 
					DevInfo.nlin,
					FrameResolution(lpFrame));

				AstralCursor( IDC_WAIT );

				for(lncnt=0; lncnt<DevInfo.nlin; lncnt++) {
					FrameCopyLine(lpFrame, lpOldFrame, lncnt);
				}

				FrameClose(lpFrame);
				lpFrame = lpOldFrame;
				AstralCursor( NULL );
				/* Setup the new image and bring up the new image window */
                lpNewImage = CreateImage(NULL, lpFrame, NULL, 
                   NULL, Control.DefaultFileType, Datatype, 
                   IMG_DOCUMENT, NULL);
                if (lpNewImage)
                {
                    if (!PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
                       lpNewImage))         
                    {
                        DestroyImage(lpNewImage);
                        lpNewImage = NULL;
                    }
                }

			}
			/* only version of image is in the cache */
			/* so insure user is asked about saving when done */
            if (lpNewImage)    
                lpNewImage->fChanged = TRUE;

			DevClose(); /* close the device */
		}
	}
}

#ifndef WIN32
/************************************************************************/
WORD GetA000H()
/************************************************************************/
{
HMODULE hKernel;
LPTR lpVar;
static WORD _A000H;
BOOL bLibin;

if ( !(hKernel = GetModuleHandle( "KERNEL" )) )  // see if already resident
	{
	if ( (hKernel = LoadLibrary( "KERNEL.EXE" )) < HINSTANCE_ERROR) // not resident
		return( _A000H = 0xA000 );
	else	bLibin = TRUE;
	}
else	bLibin = FALSE;

if ( !(lpVar = (LPTR)GetProcAddress( hKernel, "__A000H" )) )
	_A000H = 0xA000;
else	_A000H = LOWORD( lpVar );

if ( bLibin )  // free up the library if we loaded it
	FreeLibrary( hKernel );
return( _A000H );
}
#endif

/************************************************************************/
int EXPORT grabber_AstralClockCursor( int part, int whole )
/************************************************************************/
{
	return(AstralClockCursor(part, whole, YES));
}

/************************************************************************/
LPTR EXPORT grabber_frame_ptr( int Depth, int x, int y, BOOL bModify )
/************************************************************************/
{
	if (Depth == -1)
		return(FramePointerRaw(frame_set(NULL), x, y, bModify));
	else
		return(FramePointer(frame_set(NULL), x, y, bModify));
}

/************************************************************************/
LPTR EXPORT grabber_frame_set( LPTR lpFrame )
/************************************************************************/
{
return((LPTR)frame_set((LPFRAME)lpFrame));
}

/************************************************************************/
void EXPORT grabber_frame_close( LPFRAME lpFrame )
/************************************************************************/
{
FrameClose(lpFrame);
}

/************************************************************************/
LPFRAME EXPORT grabber_frame_open( WORD Depth, int x_size, int y_size, int Resolution )
/************************************************************************/
{
	LPFRAME lpNewFrame = NULL;

	switch(Depth)
	{
	 	case 0 : lpNewFrame = FrameOpen(FDT_LINEART,   x_size, y_size, Resolution); break;
	 	case 1 : lpNewFrame = FrameOpen(FDT_GRAYSCALE, x_size, y_size, Resolution); break;
	 	case 3 : lpNewFrame = FrameOpen(FDT_RGBCOLOR,  x_size, y_size, Resolution); break;
	 	case 4 : lpNewFrame = FrameOpen(FDT_CMYKCOLOR, x_size, y_size, Resolution); break;
	}

	if (lpNewFrame == NULL)
		FrameError( IDS_EPROCESSOPEN );

	return(lpNewFrame);
}


