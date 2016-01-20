#include "pp.h"
#include <mmsystem.h>
#include "aviintf.h"
#include "aviutil.h"


#define MAXSTREAMS	12

typedef struct _streamlst{
	PAVIFILE	pfile;
	PAVISTREAM Streams[MAXSTREAMS];
	int nStreams;
}STREAMLST, FAR *LPSTREAMLST;

typedef struct _avisave{
	PAVIFILE	pf;
	PAVISTREAM 	pStream;
	int writepos;
}AVISAVE, FAR *LPAVISAVE;


LOCAL LPVOID avi_OpenStreams(LPSTR lpFileName);
LOCAL PAVISTREAM avi_GetEditableStream(LPVOID lpOpenData, DWORD fccType);
LOCAL PAVISTREAM avi_GetStream(LPVOID lpOpenData, DWORD fccType);
LOCAL BOOL avi_SaveStreams(LPVOID lpOpenData, LPSTR lpSaveName);
LOCAL void avi_CloseStreams(LPVOID lpOpenData);
LOCAL PAVISTREAM avi_CreateStream(PAVISTREAM pStream, LPBITMAPINFOHEADER lpbi);
LOCAL LPBITMAPINFOHEADER avi_prepare_frame(LPAVISTREAMINFO pInfo, BOOL bClip, LPFRAME lpFrame, LPOBJECT lpObject);
LPAVISAVE avi_SaveBegin(LPBITMAPINFOHEADER lpbi, PAVISTREAM pBaseStream, LPSTR lpFilename);
BOOL avi_SaveWrite(LPAVISAVE lpAviSave, LPBITMAPINFOHEADER lpbi);
VOID avi_SaveClose(LPAVISAVE lpAviSave);
LOCAL BOOL TEST_VER();


//************************************************************************
//	 aviInstertFrame
// PARAMETERS:
//			LPSTR lpSrcFile- The file to insert into.
//			LPSTR lpDstFile- The file to put the result into.
//			LPFRAME lpFrame- The frame to insert.
//			long lFrame- The frame number to insert before. (-1 for end)
//			BOOL bClip- Clip or Size frame if no correct size.
// DESCRIPTION:
//		Put the given frame into the avi file lpSrcFile, putting the 
//		result into lpDstFile. 
// RETURNS:
//		FALSE for failure.
//************************************************************************
BOOL aviInstertFrame(LPSTR lpSrcFile, LPSTR lpDstFile, int iAVIPosType, long AVIPosition, BOOL bClip, LPFRAME lpFrame, LPOBJECT lpObject)
//************************************************************************
{

	long start,i, nTotal, nLastFrame, ReadFrame, lFrame;
	LPVOID lpData = NULL;
	PAVISTREAM lpNewStream = NULL;
	LPBITMAPINFOHEADER lpDIB;
	LPBITMAPINFOHEADER  lpbi = NULL;
	PAVISTREAM lpEditStream;
	AVISTREAMINFO streaminfo;
	PGETFRAME pgf = NULL;
    PAVIFILE         pf = NULL;
	LPAVISAVE		lpAviSave = NULL;
	
	ProgressBegin(1, 0);
	// open file
	if (!(lpData = avi_OpenStreams(lpSrcFile)))
		goto ExitError;
	lpEditStream = avi_GetStream(lpData, streamtypeVIDEO);
	if (!lpEditStream)
		goto ExitError;
	// get bitmap
	AVIStreamInfo(lpEditStream, &streaminfo, sizeof(streaminfo));
	if (!(lpbi = avi_prepare_frame(&streaminfo, bClip, lpFrame,lpObject)))
		goto ExitError;
	
	// write to new file 
	start = AVIStreamStart(lpEditStream);
	nTotal = AVIStreamLength(lpEditStream);
	nLastFrame = start+nTotal;
	if (start <0 || nTotal<0)
		goto ExitError;
		
	switch(iAVIPosType)
	{
		case 0: 	// AVI_ADD_END
			lFrame = nLastFrame;
			nLastFrame++;
		break;
		case 1:		// IDC_AVI_INSERT
			lFrame = mbound(AVIPosition,0,nLastFrame);
			nLastFrame++;
		break;
		case 2:		// IDC_AVI_REPLACE
			lFrame = mbound(AVIPosition,0,nLastFrame);
			if (AVIOptions.AVIPosition == nLastFrame)
				nLastFrame++;	
		break;
		default:
			goto ExitError;
	}
	pgf = AVIStreamGetFrameOpen(lpEditStream, NULL);
	if (!pgf)
		goto ExitError;
		
	lpAviSave = avi_SaveBegin(lpbi, lpEditStream, lpDstFile);
	if (!lpAviSave)
		goto ExitError;
		
	ReadFrame = start;	
	for ( i=start; i<nLastFrame; i++ )
	{
		AstralClockCursor(i-start, nLastFrame-start, NO );
		// get frame
		if (i==lFrame)
		{
			lpDIB = lpbi;
			
			// skip this frame for insert
			if (iAVIPosType == 2 /*IDC_AVI_REPLACE*/)
				ReadFrame++;
			if (!(lpbi = avi_prepare_frame(&streaminfo, bClip, lpFrame,lpObject)))
				goto ExitError;
		}
		else
			lpDIB = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, ReadFrame++);
		if (!lpDIB)
			goto ExitError;

		// convert 16 bit frame to 24 bit
		if (lpDIB->biBitCount == 16)
		{
			LPFRAME lpTempFrame;
			lpTempFrame = DIBToFrame((LPTR)lpDIB, NO);
			if (!lpTempFrame)
				goto ExitError;
			if (!(lpDIB = avi_prepare_frame(&streaminfo, bClip, lpTempFrame, NULL)))
				goto ExitError;
			if (!avi_SaveWrite(lpAviSave, lpDIB))
				goto ExitError;
			FreeUp((LPTR)lpDIB);
		}		
		else
		{
			// Now write out video frame
			if (!avi_SaveWrite(lpAviSave, lpDIB))
				goto ExitError;
		}
	}
	
	avi_SaveClose(lpAviSave);
	AVIStreamGetFrameClose(pgf);
	FreeUp(lpbi);
	lpbi = NULL;		
		
	avi_CloseStreams(lpData);
	ProgressEnd();
		
	return(TRUE);
ExitError:
	ProgressEnd();
	if (lpData)
		avi_CloseStreams(lpData);
	if (lpNewStream)
		AVIStreamRelease(lpNewStream);
	if (lpAviSave)
		avi_SaveClose(lpAviSave);
	if (lpbi)
		FreeUp(lpbi);
	if (pgf)
		AVIStreamGetFrameClose(pgf);
	return(FALSE);
}



//************************************************************************
//	 aviInstertFrame
// DESCRIPTION:
//		Put the given frame into a new avi file.
// RETURNS:
//		FALSE for failure.
//************************************************************************
BOOL aviNewFile(LPSTR lpFile, LPFRAME lpFrame, LPOBJECT lpObject)
//************************************************************************
{
	LPBITMAPINFOHEADER  lpbi = NULL;
	LPAVISAVE		lpAviSave = NULL;
	
	// get dib
	if (lpObject)
	{
		lpFrame = ImgCopyFrame(NULL,lpObject,NULL,NO,NO);
		lpbi = (LPBITMAPINFOHEADER)FrameToDIB(lpFrame, NULL);
		FrameClose(lpFrame);
	}
	else
		lpbi = (LPBITMAPINFOHEADER)FrameToDIB(lpFrame, NULL);
	if (!lpbi)
		return(FALSE);
	
	// get new file 
	lpAviSave = avi_SaveBegin(lpbi, NULL, lpFile);
	if (!lpAviSave)
		goto ExitError;
		
	// Now write out video frame
	if (!avi_SaveWrite(lpAviSave, lpbi))
	  	goto ExitError;
	
	avi_SaveClose(lpAviSave);
	FreeUp(lpbi);
	return(TRUE);

ExitError:
	if (lpAviSave)
		avi_SaveClose(lpAviSave);
	if (lpbi)
		FreeUp(lpbi);
	return(FALSE);
}

//************************************************************************
//		aviGetFrame
// DESCRIPTION:
//		Gets the lFrame frame out of an avi file.
// RETURNS:
//		The frame of NULL.
//************************************************************************
LPFRAME aviGetFrame(LPSTR lpFileName, LONG lFrame)
//************************************************************************
{

	PAVIFILE	pfile;
	PAVISTREAM 	pstream;
	PGETFRAME	pget;
	HRESULT		hResult;
	LPBITMAPINFOHEADER  lpbi;
	LPFRAME lpFrame = NULL;


	hResult = AVIFileOpen(&pfile, lpFileName, 0, 0L);
	// CHECK hRESULT ***FIX
	hResult = AVIFileGetStream(pfile, &pstream, streamtypeVIDEO, 0);
	// CHECK hRESULT ***FIX
	
	pget = AVIStreamGetFrameOpen(pstream, NULL);
	if(pget) 
	{ 
		// Great, decompressor open

		// Get our own personal copy of the DIB
		lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pget, lFrame);
		
		// Convert to frame
		lpFrame = DIBToFrame((LPTR)lpbi, NO);
		
		// Now release our stuff
		AVIStreamGetFrameClose(pget);
	} 
	else
	{
		//****ERROR ***FIX??
	}
	
	// Close the stream and file
	AVIStreamRelease( pstream );
	AVIFileRelease( pfile );
	return (lpFrame);
}


LOCAL BOOL TEST_VER()
{
	WORD wVer;
	
    wVer = HIWORD(VideoForWindowsVersion());
    if (wVer < 0x010a){
	    /* oops, we are too old, blow out of here */
	    return FALSE;
    }
	return(TRUE);
}


//************************************************************************
//		aviOpenStreams
// DESCRIPTION:
//		Open the given file and all its streams
// RETURNS:
//		The data to be passed to other avi routines.  NULL for failure.
//************************************************************************
LOCAL LPVOID avi_OpenStreams(LPSTR lpFileName)
//************************************************************************
{
	LPSTREAMLST lpStLst;
	AVIFILEINFO	fileinfo;
	int i;

	
	lpStLst = (LPSTREAMLST)Alloc(sizeof(STREAMLST));
	if (!lpStLst)
		return(NULL);
	clr((LPTR)lpStLst, sizeof(STREAMLST));
	
	
	// open file
	if (AVIFileOpen(&lpStLst->pfile, lpFileName, OF_READ, NULL))
		goto ExitError;
	// arbitrary limit on number of streams
	if (AVIFileInfo(lpStLst->pfile, &fileinfo, sizeof(fileinfo)))
		goto ExitError;
	lpStLst->nStreams = min(fileinfo.dwStreams, MAXSTREAMS);
	if (lpStLst->nStreams <= 0)
		goto ExitError;
	// read through streams
	for (i=0;i<lpStLst->nStreams;i++)
		AVIFileGetStream(lpStLst->pfile, &lpStLst->Streams[i], 0L, i);
	
	return((LPVOID)lpStLst);

ExitError:
	if (lpStLst->pfile)
		AVIFileClose(lpStLst->pfile);
	FreeUp(lpStLst);
	return(NULL);	
}

//************************************************************************
//		aviGetEditableStream
// DESCRIPTION:
//		Makes the first stream of the given type into an Editable stream.
// RETURNS:
//		The editable stream, or NULL for failure.
//************************************************************************
LOCAL PAVISTREAM avi_GetEditableStream(LPVOID lpOpenData, DWORD fccType)
//************************************************************************
{
	int i;
	AVISTREAMINFO streaminfo;
	PAVISTREAM editStream;
	LPSTREAMLST lpStLst = (LPSTREAMLST)lpOpenData;
	if (!lpStLst)
		return(NULL);
	
	// find first matching stream
	for (i=0;i<lpStLst->nStreams;i++)
	{
		AVIStreamInfo(lpStLst->Streams[i], &streaminfo, sizeof(streaminfo));
		if (streaminfo.fccType == fccType)
			break;
	}
	if (i>=lpStLst->nStreams)
		return(NULL);

	// prepare video for editing
	CreateEditableStream(&editStream, lpStLst->Streams[i]);
	AVIStreamRelease(lpStLst->Streams[i]);
	lpStLst->Streams[i] = editStream;
	return(lpStLst->Streams[i]);
}



//************************************************************************
//		aviGetStream
// DESCRIPTION:
//		Gets the first stream of the given type.
// RETURNS:
//		NULL for failure.
//************************************************************************
LOCAL PAVISTREAM avi_GetStream(LPVOID lpOpenData, DWORD fccType)
//************************************************************************
{
	int i;
	AVISTREAMINFO streaminfo;
	LPSTREAMLST lpStLst = (LPSTREAMLST)lpOpenData;
	if (!lpStLst)
		return(NULL);
	
	// find first matching stream
	for (i=0;i<lpStLst->nStreams;i++)
	{
		AVIStreamInfo(lpStLst->Streams[i], &streaminfo, sizeof(streaminfo));
		if (streaminfo.fccType == fccType)
			break;
	}
	if (i>=lpStLst->nStreams)
		return(NULL);

	return(lpStLst->Streams[i]);
}


//************************************************************************
//	aviSaveStreams
// DESCRIPTION:
//		Saves all the streams in lpOpenData to the given file
// RETURNS:
//		FALSE for failure.
//************************************************************************
LOCAL BOOL avi_SaveStreams(LPVOID lpOpenData, LPSTR lpSaveName)
//************************************************************************
{
	LONG err;
	LPSTREAMLST lpStLst = (LPSTREAMLST)lpOpenData;
	if (!lpStLst)
		return(FALSE);
	
	err = (long)AVISaveV(lpSaveName,	// file name
			     	NULL,			// handler
			     	(AVISAVECALLBACK) NULL, // callback
			     	lpStLst->nStreams,	// number of streams
			     	lpStLst->Streams,	// source streams
			     	NULL);			// compressores

	return(err==0);
}



//************************************************************************
//		aviCloseStreams
// DESCRIPTION:
//		Call with data from aviOpenStreams.
//************************************************************************
LOCAL void avi_CloseStreams(LPVOID lpOpenData)
//************************************************************************
{
	int i;
	LPSTREAMLST lpStLst = (LPSTREAMLST)lpOpenData;
	if (!lpStLst)
		return;
	
	// close streams
	for (i=0;i<lpStLst->nStreams;i++)
		AVIStreamRelease(lpStLst->Streams[i]);
	// close file
	AVIFileRelease(lpStLst->pfile);
	FreeUp(lpStLst);
}

//************************************************************************
//		avi_prepare_frame
// PARAMETERS:
//		PAVISTREAMINFO pInfo- The info to base new frame on. 
//		BOOL bClip- Clip or size.
//		LPFRAME lpFrame- the frame (only use lpFrame OR lpObject).
//		LPOBJECT lpObject- the object (only use lpFrame OR lpObject).
// DESCRIPTION:
//		Sizes or clips the frame (or object) to fit the stream defined
//		by pInfo.
// RETURNS:
//		A bitmap which fits pInfo or NULL for failure.
//************************************************************************
LOCAL LPBITMAPINFOHEADER avi_prepare_frame(LPAVISTREAMINFO pInfo, BOOL bClip, LPFRAME lpFrame, LPOBJECT lpObject)
//************************************************************************
{
	LPBITMAPINFOHEADER lpDIB;
	LPFRAME lpNewFrame;
	RECT rOut;
	
	rOut = pInfo->rcFrame;
	// convert to pp rect
	rOut.right--;
	rOut.bottom--;
	
	if (lpObject)
		lpNewFrame = ImgCopyFrame(NULL,lpObject,&rOut,bClip,NO);
	else if (RectWidth(&rOut) != FrameXSize(lpFrame) ||	RectHeight(&rOut) != FrameYSize(lpFrame))
	{
		if (!bClip)
			lpNewFrame = SizeFrame(lpFrame, RectWidth(&rOut), RectHeight(&rOut), 100, NO);
		else if (RectWidth(&rOut)>FrameXSize(lpFrame)||RectHeight(&rOut)>FrameYSize(lpFrame))
			lpNewFrame = FrameCopy( lpFrame, &rOut );
	}
	else
		lpNewFrame = lpFrame;
	if (!lpNewFrame)
		return(NULL);
	lpDIB = (LPBITMAPINFOHEADER)FrameToDIB(lpNewFrame, &rOut);
	if (lpNewFrame != lpFrame)
		FrameClose(lpNewFrame);
	return(lpDIB);
}



//************************************************************************
//
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
//
//************************************************************************
LPAVISAVE avi_SaveBegin(LPBITMAPINFOHEADER lpbi, PAVISTREAM pBaseStream, LPSTR lpFilename)
{
    HRESULT          hr;
    AVISTREAMINFO    strhdr;
	LPAVISAVE		lpAviSave;

	lpAviSave = (LPAVISAVE)Alloc(sizeof(AVISAVE));
	if (!lpAviSave)
		return(NULL);
	clr((LPTR)lpAviSave, sizeof(AVISAVE));

    hr = AVIFileOpen(&lpAviSave->pf, 	// returned file pointer
		       lpFilename,		    	// file name
		       OF_WRITE | OF_CREATE,	// mode to open file with
		       NULL);			    	// use handler determined
	if (hr != AVIERR_OK)
		goto ExitError; 

    if (pBaseStream)
	{
    	hr = AVIStreamInfo(pBaseStream, &strhdr, sizeof(strhdr));
		if (hr != AVIERR_OK)
			goto ExitError; 
	}
	else
    {
    	clr((LPTR)&strhdr, sizeof(strhdr));
    	strhdr.fccType                = streamtypeVIDEO;// stream type
    	strhdr.fccHandler             = 0;
    	strhdr.dwScale                = 1;
    	strhdr.dwRate                 = 15;		    // 15 fps
    	strhdr.dwSuggestedBufferSize  = lpbi->biSizeImage;
    	SetRect(&strhdr.rcFrame, 0, 0,		    // rectangle for stream
		    (int) lpbi->biWidth,
		    (int) lpbi->biHeight);
	}

	hr = AVIFileCreateStream(lpAviSave->pf, &lpAviSave->pStream, &strhdr);
	if (hr != AVIERR_OK)
		goto ExitError; 
    hr = AVIStreamSetFormat(lpAviSave->pStream, 0, lpbi,(lpbi->biSize+lpbi->biClrUsed*sizeof(RGBQUAD)));
	if (hr != AVIERR_OK)
		goto ExitError; 
	lpAviSave->writepos = AVIStreamStart(lpAviSave->pStream);
	return(lpAviSave);

ExitError:
	if (lpAviSave->pStream)
		AVIStreamClose(lpAviSave->pStream);
	if (lpAviSave->pf)
	{
		AVIFileClose(lpAviSave->pf);
		FileDelete(lpFilename);
	}
	return(NULL);
}
	
//************************************************************************
//
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
//
//************************************************************************
BOOL avi_SaveWrite(LPAVISAVE lpAviSave, LPBITMAPINFOHEADER lpbi)
{
	LPTR lpData;
    HRESULT          hr;
	
    lpData =  (LPTR)lpbi+lpbi->biSize+lpbi->biClrUsed * sizeof(RGBQUAD);

	hr = AVIStreamWrite(lpAviSave->pStream,	    // stream pointer
			      lpAviSave->writepos++,		    // time of this frame
			      1,		    		// number to write
				  lpData,
			      lpbi->biSizeImage,	// size of this frame
			      AVIIF_KEYFRAME,	    // flags....
			      NULL, NULL);
	return(hr == AVIERR_OK);
}


//************************************************************************
//
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
//
//************************************************************************
VOID avi_SaveClose(LPAVISAVE lpAviSave)
{
     // Close the stream and file
    AVIStreamClose(lpAviSave->pStream);
    AVIFileClose(lpAviSave->pf);
	FreeUp((LPTR)lpAviSave);
}


