// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"
#include "gdevice.h"

char palette1[512];

DEVINFO DevInfo;
DEVMSG DevMsg;
DEVGET DevData;

VOID back_to_windows()
{
 PicwinOff((LPSTR)palette1,0,NULL,NULL,NULL); /* restore windows disp */
 /* We're coming back from the picture window */
 ReleaseCapture( );
 /* Force a windows redisplay */
 AstralDlgHide( IDD_GRAB );
 ShowCursor( TRUE );
}

void grabber(port, hInstance, hWindow)
int port;
HANDLE hInstance;
HWND hWindow;
{
BYTE devreq; 	/* device requirements */
		/* 1XXXXXXXb = putting image into cache */
		/* 0XXXXXXXb = getting image from cache */
		/* XXXXXXX1b =  need to take over display */
		/* XXXXXXX0b =  no need to take over display */
		/* XXXXXX1Xb =  special willow gs read */
		/* XXXXXX0Xb =  no special read */
		/* XXXXX1XXb =  color image */
		/* XXXXX0XXb =  no color image */
		/* XXXX1XXXb =  if color, 24-bit color image */
		/* XXXX0XXXb =  if color, palette colored image */
LPFRAME lpFrame, lpOldFrame;
BYTE depth;
int lncnt;

 /* get device's intentions -- is this a put to or read from cache */
 DevInfo.hInst = hInstance;  /* set the data structures */
 DevInfo.hPw = hWindow;
 DevInfo.bImg_type = (lpImage->DataType==IDC_SAVECT ? TRUE : FALSE);
 DevInfo.bFile_is_open = ((frame_set( NULL ) == NULL) ? FALSE : TRUE);
 if(DevInfo.bFile_is_open) 
    lstrcpy(DevInfo.cfname, lpImage->CurFile); /* pass the current filename */
 if(DevOpen(&DevInfo))
   {
    DevCall(DEV_MSG, (LPTR)&DevMsg);
    if(DevMsg.MsgNo) /* if MsgNo == 0, then user cancelled */
      {
       if(DevMsg.MsgNo > 0)
	 Message( DevMsg.MsgNo );
       else
	 Print("%ls", (LPTR)DevMsg.szMsg); /* print the driver's message */
      }
    return; 
   }
 devreq = DevInfo.device_caps;
 if(!(devreq & 0x80))  /* doing a put cache to an external device */
   {		       /* fill up the device info data structure */
   DevInfo.npix = lpImage->npix;
   DevInfo.nlin = lpImage->nlin;
   DevInfo.bpl = lpImage->npix;
   DevInfo.xres = lpImage->xres;
   DevInfo.yres = lpImage->yres;
   DevInfo.bpp = 8;
   }
 else
   {
    DevInfo.port = port; /* set the port for devices which require it */
    DevInfo.vgaaddr = GetA000H(); /* set the VGA buffer address */
   }

 if(devreq & 1) /* see if we need to take over display */
  {
   ShowCursor( FALSE );
   /* Create the acquire window: it doesn't have the visable bit set */
   AstralDlg( YES, hInstance, hWindow, IDD_GRAB, AcquireProc);
   AstralDlgShow( IDD_GRAB );
   SetCapture( AstralDlgGet( IDD_GRAB ) );
   /* Enter the picture window command processor */
   DevInfo.hInst = hInstance;  /* set the data structures */
   DevInfo.hPw = AstralDlgGet( IDD_GRAB );
   PicwinOn((LPSTR)palette1);  /* call display driver disable */
  }

 lstrcpy(DevInfo.cproghome, Control.ProgHome); /* home dir for ventek */
 lstrcpy(DevInfo.cramdisk, Control.RamDisk); /* scratch for ventek */
 if(DevCall(DEV_START, (LPTR)&DevInfo)) /* if get, fill struct */
   {
    if(devreq & 1) /* see if we need to restore display */
      back_to_windows();
    DevCall(DEV_MSG, (LPTR)&DevMsg);
    if(DevMsg.MsgNo) /* if MsgNo == 0, then user cancelled */
      {
       if(DevMsg.MsgNo > 0)
	 Message( DevMsg.MsgNo );
       else
	 Print("%ls", (LPTR)DevMsg.szMsg); /* print the driver's message */
      }
    return;
   }

 /* DEV_START returned ok , now get or put image data */

if(devreq & 0x80)  /* putting data into the cache */
  {
   if(devreq & 4)
     depth = 2;
   else
     depth = 1;
   lpOldFrame = frame_set( NULL );
   if (!(lpFrame = frame_open(depth,DevInfo.npix,DevInfo.nlin,DevInfo.xres)) )
	{
         if(devreq & 1) /* see if we need to restore display */
	   back_to_windows();
	 Message( IDS_ESCROPEN, (LPTR)Control.RamDisk );
	 return;
	}
   frame_set( lpFrame );

   if(!(devreq & 2)) /* standard read */
	{
	 for ( lncnt=0; lncnt<DevInfo.nlin; lncnt++ )
	   {
	    AstralClockCursor( lncnt, DevInfo.nlin );
	    DevData.ImageAddress = frame_ptr(0, 0, lncnt, YES);
	    if(DevCall(DEV_READ, (LPTR)&DevData))
	      {
               if(devreq & 1) /* see if we need to restore display */
	         back_to_windows();
	       Print("Unable to get data from device");
	       return;
	      }
	   }
	}
     else /* special willow gs reader */
	{
	  /* read even bytes, even lines */
	 for ( lncnt=0; lncnt<DevInfo.nlin; lncnt+=2 )
	   {
	    DevData.ImageAddress = frame_ptr(0, 0, lncnt, YES);
	    if(DevCall(DEV_READ0, (LPTR)&DevData))
	      {
               if(devreq & 1) /* see if we need to restore display */
	         back_to_windows();
	       Print("Unable to get data from device");
	       return;
	      }
	   }
	  /* read odd bytes, even lines */
	 for ( lncnt=0; lncnt<DevInfo.nlin; lncnt+=2 )
	   {
	    DevData.ImageAddress = frame_ptr(0,1, lncnt, YES);
	    if(DevCall(DEV_READ1, (LPTR)&DevData))
	      {
               if(devreq & 1) /* see if we need to restore display */
	         back_to_windows();
	       Print("Unable to get data from device");
	       return;
	      }
	   }
	  /* read even bytes, odd lines */
	 for ( lncnt=1; lncnt<DevInfo.nlin; lncnt+=2 )
	   {
	    DevData.ImageAddress = frame_ptr(0, 0, lncnt, YES);
	    if(DevCall(DEV_READ2, (LPTR)&DevData))
	      {
               if(devreq & 1) /* see if we need to restore display */
	         back_to_windows();
	       Print("Unable to get data from device");
	       return;
	      }
	   }
	  /* read odd bytes, odd lines */
	 for ( lncnt=1; lncnt<DevInfo.nlin; lncnt+=2 )
	   {
	    DevData.ImageAddress = frame_ptr(0,1, lncnt, YES);
	    if(DevCall(DEV_READ3, (LPTR)&DevData))
	      {
               if(devreq & 1) /* see if we need to restore display */
	         back_to_windows();
	       Print("Unable to get data from device");
	       return;
	      }
	   }
	}

   frame_close( lpOldFrame );

   if(devreq & 1) /* see if we need to restore display */
     back_to_windows();

   /* Setup the new image and bring up the new image window */
   NewImageWindow( NULL, NULL, lpFrame, IDC_SAVETIFF, (DevInfo.bpp == 1), FALSE, IMG_DOCUMENT, NULL );

   lpImage->fChanged = YES; /* only version of image is in the cache */
			 /* so insure user is asked about saving when done */
   DevClose(); /* close the device */
  }
else /* putting data to external device */
  for ( lncnt=0; lncnt<DevInfo.nlin; lncnt++ )
   {
    AstralClockCursor( lncnt, DevInfo.nlin );
    DevData.ImageAddress = frame_ptr(0, 0, lncnt, NO);
    if(DevCall(DEV_WRITE, (LPTR)&DevData))
      {
	Print("Unable to put data to device");
	return;
      }
  }
}


/************************************************************************/
WORD GetA000H()
/************************************************************************/
{
HANDLE hKernel;
LPTR lpVar;
static WORD _A000H;

//if ( _A000H )
//	return( _A000H );

if ( !(hKernel = LoadLibrary( "KERNEL.EXE" )) )
	return( _A000H = 0xA000 );

if ( !(lpVar = (LPTR)GetProcAddress( hKernel, "__C000H" )) )
	_A000H = 0xA000;
else	_A000H = LOWORD( lpVar );
//Print("C000 %x", _A000H );

if ( !(lpVar = (LPTR)GetProcAddress( hKernel, "__A000H" )) )
	_A000H = 0xA000;
else	_A000H = LOWORD( lpVar );
//Print("A000 %x", _A000H );

FreeLibrary( hKernel );
return( _A000H );
}
