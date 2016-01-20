//////////////////////////////////////////////////////////////////////////////
//  (c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.               //
//  This material is confidential and a trade secret.                       //
//  Permission to use this work for any purpose must be obtained            //
//  in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081      //
//////////////////////////////////////////////////////////////////////////////
#include "pp.h"
#include "routines.h"
#include "imprtrdr.h"
#include "id.h"
#include "ruby.h"

LOCAL BOOL RUBYPROC StatusProc (
    BYTE byPercent,        // @parm Current percentage
    RUBYSTATUSCODE rsCode, // @parm Status code.
    LPCSTR pszErrMsg);       // @parm Error string 

//////////////////////////////////////////////////////////////////////////////
BOOL CReadImage::ImportRead()
//////////////////////////////////////////////////////////////////////////////
{
    BOOL fRet = FALSE;
	RUBYIMPORTFILE	Import;
   	FNAME szTempFile;

    ProgressBegin( 2 );

    // create temp file name
    // set import filter profile path
    if( Control.bNetworkInstall )       // use pouch path
		lstrcpy( szTempFile, Control.PouchPath );
    else                               // use filters directory
    	GetExtNamePath( szTempFile, IDN_IMPORTFILTERS );

    lstrcat( szTempFile, "PP" );
    GetTempFilename( szTempFile, ".TIF" );
 
	clr((LPTR)&Import, sizeof(Import));
	Import.uiSize			= sizeof(RUBYIMPORTFILE);
	Import.pszTempFile 		= szTempFile;
	Import.Interface		= RUBY_DT_TIFF_FILE;
	Import.pszSrcFile		= m_lpFileName;
	Import.pszSrcFormat 	= extension(m_lpFileName)+1;
	Import.pStatusProc		= StatusProc;
	Import.hwndParent		= MessageParent();
	Import.bConfigure		= FALSE;

    ProgressSetupPass( 90, 0 );
    ProgressBegin(1);
    BOOL fSuccess = rubyImportFile(&Import);
	ProgressEnd();

	if (fSuccess)
	{
    	CReadImage TiffRead( szTempFile );

    	// get pointer to frame and pass data back up
   		m_lpFrame     = TiffRead.ReadFrame( m_cmsInfo.dst );
		if (m_lpFrame)
		{
			m_iWidth      = TiffRead.GetFileWidth();
			m_iHeight     = TiffRead.GetFileHeight();
			m_iRes        = TiffRead.GetFileResolution();
    		m_FileType    = TiffRead.GetBitmapFileType();
			fRet = TRUE;
		}
	}

   	FileDelete( szTempFile );        // get rid of temp file in case of error
    ProgressEnd();
    return( fRet );
}


//
// This function handles status from Ruby
//
LOCAL BOOL RUBYPROC StatusProc (
    BYTE byPercent,        // @parm Current percentage
    RUBYSTATUSCODE rsCode, // @parm Status code.
    LPCSTR pszErrMsg       // @parm Error string 
)
{
	switch( rsCode )
	{
   		case RUBY_STATUS_PERCENT:
   		{
  	
			if (AstralClockCursor((int)byPercent, 100, TRUE))
				return(FALSE);
      		break;

   		case RUBY_STATUS_WARNING:
   		case RUBY_STATUS_ERROR:
   		case RUBY_STATUS_UNKNOWN:
      		// In general, any message coming through with the
      		// RUBY_STATUS_WARNING flag may be ignored.
      		break;

		}
	}
	return(TRUE);
}





				  
