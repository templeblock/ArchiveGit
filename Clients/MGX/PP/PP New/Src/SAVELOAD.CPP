//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "svrdoc.h"
#include "id.h"
#include "bsrid.h"  
#include "browser.h"
#include "framelib.h"
#include <sys/types.h>
#include <sys/stat.h>		  

extern BOOL SetupImportOptions( int );

// Static prototypes	 
LOCAL void OldToNewMap( LPOLDMAP lpOldMap, LPMAP lpNewMap);
LOCAL BOOL Load31Map(LPMAP lpMap0, LPMAP lpMap1, LPMAP lpMap2, LPMAP lpMap3, LPTSTR lpFileName );
LOCAL HWND LoadImageWindow( LPCMDLIST lpCmdList, ITEMID idFileType, LPTSTR lpFileName, int iDoZoom );
LOCAL BOOL Read31Map(FILE_HANDLE fh, LPMAP lpMap);
LOCAL BOOL Read40Map(FILE_HANDLE fh, LPMAP lpMap);
LOCAL BOOL Read50Map(FILE_HANDLE fh, LPMAP lpMap);
LOCAL BOOL Write40Map(FILE_HANDLE fh, LPMAP lpMap);
LOCAL BOOL Write50Map(FILE_HANDLE fh, LPMAP lpMap);
LOCAL BOOL LoadMap( LPMAP lpMap0, LPMAP lpMap1, LPMAP lpMap2, LPMAP lpMap3, LPMAP lpMap4,
               LPTSTR lpFileName, LPBOOL fIsPP5Map );
LOCAL BOOL HandleImageSave(LPIMAGE lpImage, LPTSTR lpFileName, ITEMID idFileType,
					ITEMID idDataType, LPVOID lpOptions);

// for Shape reading
LOCAL int get_values(LPTSTR lpStr, LPPOINT lpPoints, int nMax, int res);
LOCAL BOOL shape_read_poly(CStdioFile *lpFile, LPCPolyList lpList, int res);
LOCAL BOOL shape_write_header(CStdioFile *lpFile, LPCPolyList lpPolyList, int res);
LOCAL BOOL shape_write_trailer(CStdioFile *lpFile);
LOCAL BOOL shape_write_poly(LPCPoly lpPoly, CStdioFile *lpFile,  LPRECT lpBounds, int res);

/***********************************************************************/
HWND AstralImageLoad( ITEMID idFileType, LPTSTR lpFileName, int iDoZoom,
                       BOOL bCheckDuplicates )
/***********************************************************************/
{
HWND hWnd;
LPTSTR lpNewName;

if ( bCheckDuplicates )
	{
	if ( !OKToOpen( lpFileName, &lpNewName ) )
		return(NULL);
	}
hWnd = LoadImageWindow(NULL, idFileType, lpFileName, iDoZoom);
if (hWnd)
	{
    // Add the file name to our recall list
    AddRecallImageName( lpFileName );
	}
else
	{
	// if we can't read it, nuke it from the recall list
   	if (!FileExists(lpFileName))
    	DelRecallImageName(lpFileName);
	}
return(hWnd);
}

/***********************************************************************/
CDocument * AstralImageLoadBG( ITEMID idFileType, LPTSTR lpFileName,
                        BOOL bCheckDuplicates, LPCMSINFO pcmsInfo )
/***********************************************************************/
{
	OPENINFO OpenInfo;
	LPLOADFILE_PARMS lpParms;
	FNAME szFileName;

	lpParms = (LPLOADFILE_PARMS)Alloc(sizeof(LOADFILE_PARMS));
	if (!lpParms)
	{
		Message(IDS_EMEMALLOC);
		return(NULL);
	}

	if (!CreateLoadFileParms(	idFileType,
								lpFileName,
								TRUE,
								pcmsInfo,
								&OpenInfo.PhotoCDResOverride,
								lpParms))
	{
		FreeUp(lpParms);
		return(NULL);
	}

	OpenInfo.idCommand = IDS_CMD_LOADFILE;
	OpenInfo.lpParms = (LPCMD_PARMS)lpParms;

	if (AlreadyOpen(lpFileName) ||
    	(lpParms->idFileType >= IDN_FIRSTIMPORTFILETYPE && 
         lpParms->idFileType <= IDN_LASTIMPORTFILETYPE) )
		GetUntitledName(szFileName);
	else
		lstrcpy(szFileName, lpFileName);
	CDocument *pDoc = PictPubApp.OpenDocumentFile(szFileName, &OpenInfo);
	if (!pDoc)
		FreeUp(lpParms);

	return(pDoc);
}

/***********************************************************************/
BOOL CreateLoadFileParms( ITEMID idFileType, LPTSTR lpFileName,
                        BOOL bCheckDuplicates, LPCMSINFO pcmsInfo,
						LPINT lpPhotoCDResOverride,
						LPLOADFILE_PARMS lpParms)
/***********************************************************************/
{
    LPTSTR 			lpNewName;
	LPPCDOPTIONS	lpOptions;
    ITEMID          FileType;
	CMSINFO			cmsInfo;

    if( bCheckDuplicates )
    {
	    if( !OKToOpen( lpFileName, &lpNewName ))
		    return( FALSE );
    }

    FileType = GetPPFileType( lpFileName );
    if( !FileType )
        FileType = idFileType;
    
	PPFFOpenOptions.PhotoCDResOverride = 0;

	lpParms->lpOptions = NULL;

    if( FileType >= IDN_FIRSTIMPORTFILETYPE && 
                FileType <= IDN_LASTIMPORTFILETYPE )
    {
        if( !SetupImportOptions( FileType ))
            return(FALSE);
    }
	else
	{
		CReadImage read(lpFileName, FileType);
		if (!read.PromptForOptions(&lpParms->lpOptions))
			return(FALSE);
		if (FileType == IDN_PHOTOCD && lpParms->lpOptions)
		{
			lpOptions = ( LPPCDOPTIONS )lpParms->lpOptions;
			if ( !pcmsInfo )
				pcmsInfo = &cmsInfo;
			*pcmsInfo = lpOptions->cmsInfo;
		}
	}

	*lpPhotoCDResOverride = PPFFOpenOptions.PhotoCDResOverride;

    lstrcpy( lpParms->szFileName, lpFileName );
	GetVolumnID(lpParms->szFileName, lpParms->szVolumeID);
   lpParms->idFileType = FileType;

   // when all of CMS is hooked this can be removed
   if( pcmsInfo )
	   lpParms->cmsInfo = *pcmsInfo;
   else
	   cmsInfoInit( &lpParms->cmsInfo );

	if( !IsSrcPTSelected( &lpParms->cmsInfo.src.ptInfo ) )
		lpParms->cmsInfo.dst = lpParms->cmsInfo.src;

	return(TRUE);
}

/***********************************************************************/
BOOL LoadFile( LPIMAGE lpImage, LPLOADFILE_PARMS lpParms )
/***********************************************************************/
{
LPNEWIMAGEINFO lpInfo;

lpInfo = (LPNEWIMAGEINFO)Alloc(sizeof(NEWIMAGEINFO));
if (!lpInfo)
	{
	Message(IDS_EMEMALLOC);
	return(FALSE);
	}
clr((LPTR)lpInfo, sizeof(NEWIMAGEINFO));

CReadImage read(lpParms->szFileName, lpParms->idFileType, &lpParms->cmsInfo, lpParms->szVolumeID );
read.SetOptions(lpParms->lpOptions);
lpImage = read.ReadImage(lpParms->lpCmdList);
if (lpImage)
	{
	FRMTYPEINFO TypeInfo;

	ImgGetTypeInfo(lpImage, &TypeInfo);
	if (IsDstPTSelected(&TypeInfo.ptInfo))
		lpParms->cmsInfo.dst = TypeInfo;
	read.GetCMSSrcTypeInfo(&lpParms->cmsInfo.src);

    // Add the file name to our recall list
	if (!lpImage->lpCmdList->fCmdListEdit)
	    AddRecallImageName( lpParms->szFileName );

	lpInfo->lpImage = lpImage;
	ListAddTail(&lpParms->Common.NewImageList, lpInfo);
	lpParms->Common.UpdateType = UT_NEWIMAGE;
	return(TRUE);
	}
else
	{
	// if we can't read it, nuke it from the recall list
   	if (!FileExists(lpParms->szFileName))
    	DelRecallImageName(lpParms->szFileName);
	}
FreeUp(lpInfo);
return(FALSE);
}

/***********************************************************************/
BOOL AstralImageSave( LPIMAGE lpImage, ITEMID idFileType,
                        ITEMID idDataType, LPTSTR lpFileName )
/***********************************************************************/
{
	BOOL fRet;
	LPVOID lpOptions;

	if ( !lpImage )
		return( FALSE );

	if ( idFileType == IDN_AVI)
		return( DoAviSave(lpImage, idDataType, lpFileName ) );

	if ( lpImage->DocumentType == IMG_DOCPIECE &&
		StringsEqual( lpImage->CurFile, lpFileName ) )
		return( DoFastbitsSave( idFileType, idDataType, lpFileName ) );

	if (!SetupSaveOptions(idFileType, NO, lpImage))
		return(FALSE);

	if (!GetSaveOptions(idFileType, &lpOptions))
		return(FALSE);
	fRet = HandleImageSave(lpImage, lpFileName, idFileType, idDataType,
							lpOptions);
	if (lpOptions)
		FreeUp(lpOptions);
	return(fRet);
}

/***********************************************************************/
BOOL AstralImageSaveBG( LPIMAGE lpImage, ITEMID idFileType,
                           ITEMID idDataType, LPTSTR lpFileName,
									LPCMSINFO pCmsInfo )
/***********************************************************************/
{
	SAVEFILE_PARMS parms;
	FNAME szFileName;

	if ( !lpImage )
		return( FALSE );

	if ( Control.Crippled )
	{
		Message( Control.Crippled );
		return( FALSE );
	}

	// do fast bits save now
	if ( lpImage->DocumentType == IMG_DOCPIECE &&
		StringsEqual( lpImage->CurFile, lpFileName ) )
		return( DoFastbitsSave( idFileType, idDataType, lpFileName ) );
		
	if (!SetupSaveOptions(idFileType, NO, lpImage))
		return(FALSE);

	lstrcpy(szFileName, filename(lpImage->CurFile));
	stripext(szFileName);
	lstrcpy(parms.szFileName, filename(lpFileName));
	stripext(parms.szFileName);
	if (StringsEqual(szFileName, parms.szFileName))
	{
		lstrcpy(parms.szFileName, lpFileName);
		stripfile(parms.szFileName);
		lstrcat(parms.szFileName, "*");
		lstrcat(parms.szFileName, extension(lpFileName));
	}
	else
		lstrcpy(parms.szFileName, lpFileName);
	parms.idFileType = 	idFileType;
	parms.idDataType = idDataType;
	parms.EPSOptions = EPSOptions;
	parms.TIFFOptions = TIFFOptions;
	parms.TGAOptions = TGAOptions;
	parms.JPEGOptions = JPEGOptions;
	parms.PPFFOptions = PPFFOptions;
	parms.AVIOptions = AVIOptions;
	cmsInfoInit( &parms.CmsInfo );

	return(ProcessCommand(lpImage->lpCmdList, IDS_CMD_SAVEFILE, &parms));
}

/***********************************************************************/
BOOL SaveFile( LPIMAGE lpImage, LPSAVEFILE_PARMS lpParms )
/***********************************************************************/
{
	BOOL fRet;
	LPTSTR lp;
	FNAME szFileName;
	LPVOID lpOptions = NULL;

	switch (lpParms->idFileType)
	{
		case IDN_EPS:
			lpOptions = &lpParms->EPSOptions;
			break;
		case IDN_TIFF:
			lpOptions = &lpParms->TIFFOptions;
			break;
		case IDN_TARGA:
			lpOptions = &lpParms->TGAOptions;
			break;
		case IDN_PP:
        case IDN_PPCOMPOUND:
		case IDN_PP4:
			lpOptions = &lpParms->PPFFOptions;
            lpParms->PPFFOptions.bSaveAsCompoundStg = 
                (lpParms->idFileType == IDN_PPCOMPOUND) ? TRUE : FALSE;
			break;
		case IDN_JPEG:
			lpOptions = &lpParms->JPEGOptions;
			break;
		case IDN_AVI:
			lpOptions = &lpParms->AVIOptions;
			break;
		default:
			break;

	}
	lstrcpy(szFileName, lpParms->szFileName);
	lp = filename(lpParms->szFileName);
	if (*lp == '*')
	{
		stripfile(szFileName);
		lstrcat(szFileName, filename(lpImage->CurFile));
		stripext(szFileName);
		lstrcat(szFileName, extension(lpParms->szFileName));
	}
	fRet = HandleImageSave(lpImage, szFileName, lpParms->idFileType,
							lpParms->idDataType, lpOptions);

	if (fRet)
		lpParms->Common.UpdateType = UT_SAVE;
	return(fRet);
}

/***********************************************************************/
LOCAL BOOL HandleImageSave(LPIMAGE lpImage, LPTSTR lpFileName, ITEMID idFileType,
					ITEMID idDataType, LPVOID lpOptions) 
/***********************************************************************/
{
	LOADFILE_PARMS parms;
	LPCMDPKT lpCmdPkt;
	int PhotoCDResOverride, iCount;

	CWriteImage write(lpFileName, idFileType);
	write.SetOptions(lpOptions);
	write.SetUpdateThumb(Browser.CreateThumbnailOnSave);
	if (write.WriteImage(lpImage, idDataType))
	{
		iCount = ListGetCount(&lpImage->lpCmdList->UndoPacketList);
		LPCMDPKT lpLoadPkt = (LPCMDPKT)ListGetHead(&lpImage->lpCmdList->UndoPacketList);
		LPTSTR lpLoadFile = GetLoadFileName(lpLoadPkt);
		// if we have a load packet, see if we just wrote over
		// the file we loaded from, if so, then the command list
		// we have is bogus.  We need to start again with a clean
		// slate.
		if (lpLoadFile)
		{
			// see if we overwrote the file we loaded from
			BOOL fWackCmdList = StringsEqual(lpLoadFile, lpFileName);
			// special handling for PP5's
			if (idFileType == IDN_PP || idFileType == IDN_PPCOMPOUND)
			{
				// if we overwrote our PP5, it's ok if we saved the command
				// list, otherwise we must still wack it
				if (fWackCmdList)
				{
					if (SavingPP5CmdList(&lpImage->lpCmdList->UndoPacketList, lpOptions))
						fWackCmdList = FALSE;
				}
				// if we saved to a new PP5, then check to seed if we
				// saved the command list, if we didn't then wack it
				else
				{
					if (!SavingPP5CmdList(&lpImage->lpCmdList->UndoPacketList, lpOptions))
						fWackCmdList = TRUE;
				}
			}
			if (fWackCmdList)
			{
   				ListUnlink(&lpImage->lpCmdList->UndoPacketList, lpLoadPkt);
   				DestroyPacketList(&lpImage->lpCmdList->UndoPacketList);
   				DestroyPacketList(&lpImage->lpCmdList->RedoPacketList);
   				SetLoadFileName(lpLoadPkt, lpFileName, idFileType);
   				ListAddTail(&lpImage->lpCmdList->UndoPacketList, lpLoadPkt);
				// Auto Apply so the user cant do and undo.
				ImgEditApplyAll(lpImage, YES /*fApplyData*/, YES /*fApplyAlpha*/ );
				
			}

		}
		// if we never had a load packet, then we need to create one
		// so that the user can do command list stuff
		else
		{
            parms.lpOptions = NULL;
            lstrcpy( parms.szFileName, lpFileName );
            GetVolumnID(parms.szFileName, parms.szVolumeID);
            parms.idFileType = GetPPFileType( lpFileName );
            // when all of CMS is hooked this can be removed
            cmsInfoInit( &parms.cmsInfo );
            if( !IsSrcPTSelected( &parms.cmsInfo.src.ptInfo ))
            	parms.cmsInfo.dst = parms.cmsInfo.src;
			if (lpCmdPkt = CreatePacket(IDS_CMD_LOADFILE, &parms, TRUE))
				ListAddTail(&lpImage->lpCmdList->UndoPacketList, lpCmdPkt);
			// Auto Apply so the user cant do and undo.
			ImgEditApplyAll(lpImage, YES /*fApplyData*/, YES /*fApplyAlpha*/ );
		}

    	// Add the file name to our recall list
		AddRecallImageName( lpFileName );
		return(TRUE);
	}
	return(FALSE);
}

/***********************************************************************/
BOOL SavingPP5CmdList(LPLIST lpPacketList, LPVOID lpOptions)
/***********************************************************************/
{
	LPPPFFOPTIONS lpPPFFOptions = (LPPPFFOPTIONS)lpOptions;
	if (!lpOptions)
		return(FALSE);
	int iCount = ListGetCount(lpPacketList);
	if (iCount == 1)
	{
		if (lpPPFFOptions->bSaveLink)
			return(TRUE);
	}
	else
	if (iCount)
	{
		if (lpPPFFOptions->bSaveCmdList)
			return(TRUE);
	}
	return(FALSE);
}

/***********************************************************************/
BOOL SaveToAlbum( LPIMAGE lpImage, LPSAVETOALBUM_PARMS lpParms )
/***********************************************************************/
{
BOOL fRet;
LPVOID lpOptions = NULL;

switch (lpParms->idFileType)
	{
	case IDN_EPS:
		lpOptions = &lpParms->EPSOptions;
		break;
	case IDN_TIFF:
		lpOptions = &lpParms->TIFFOptions;
		break;
	case IDN_TARGA:
		lpOptions = &lpParms->TGAOptions;
		break;
	case IDN_PP:
    case IDN_PPCOMPOUND:
	case IDN_PP4:
		lpOptions = &lpParms->PPFFOptions;
		break;
	case IDN_JPEG:
		lpOptions = &lpParms->JPEGOptions;
		break;
	case IDN_AVI:
		lpOptions = &lpParms->AVIOptions;
		break;
	default:
		break;

	}
fRet = HandleImageSave(lpImage, lpParms->szFileName, lpParms->idFileType,
  						lpParms->idDataType, lpOptions);

// add to the album and update the thumbnail if necessary
NonBSRAddToAlbum(lpParms->szFileName, lpParms->szAlbum);

return(fRet);
}

/***********************************************************************/
LOCAL HWND LoadImageWindow( LPCMDLIST lpCmdList, ITEMID idFileType,
                              LPTSTR lpFileName, int iDoZoom )
/***********************************************************************/
{
LPIMAGE lpImage;
HWND hWnd=NULL;

CReadImage read(lpFileName, idFileType);
lpImage = read.ReadImage(lpCmdList);
if (!lpImage)
	return(NULL);

if (AlreadyOpen(lpFileName))
{
	GetUntitledName(lpImage->CurFile);
	lpImage->fUntitled = lpImage->fChanged = TRUE;
}
// Setup the new image and bring up the new image window
if (PictPubApp.OpenDocumentFile((LPTSTR)lpImage->CurFile,
   lpImage))         
{
    //Locate the document that owns the lpImage and retrieve the Views hWnd
	POSITION TemplatePos = PictPubApp.GetFirstDocTemplate();
	while (TemplatePos)
		{
		CDocTemplate * pTemplate = PictPubApp.GetNextDocTemplate(TemplatePos);
	    POSITION DocPos = pTemplate->GetFirstDocPosition();
	    while (DocPos)
	        {
	        CServerDoc *pDoc = (CServerDoc*)pTemplate->GetNextDoc (DocPos);    
	        if (pDoc && (pDoc->m_lpImage == lpImage))
	            {
	            POSITION ViewPos = pDoc->GetFirstViewPosition();
	            if (ViewPos)
	                {
	                CView* pView = pDoc->GetNextView( ViewPos );
	                if (pView)
	                {
	                   hWnd = pView->GetSafeHwnd();
	                   break;
	                }
	            }
    	    }
		}
    }
}

return( hWnd );
}

/***********************************************************************/
BOOL OKToOpen( LPTSTR lpFile, LPPTSTR lppName )
/***********************************************************************/
{
int ret;

*lppName = lpFile;

ret = YES;
if (AlreadyOpen(lpFile))
	{
	*lppName = NULL;
	ret = (AstralOKCancel( IDS_OKTOOPEN, (LPTSTR)lpFile ) == IDOK );
	}

return( ret );
}

/***********************************************************************/
BOOL AlreadyOpen( LPTSTR lpFile )
/***********************************************************************/
{
	return( (PictPubApp.GetDocument(lpFile) != NULL));
}


/***********************************************************************/
void NewAddRecallImageName( LPTSTR lpFileName )
/***********************************************************************/
{
 	PictPubApp.AddToRecentFileList(lpFileName);
}

/***********************************************************************/
void AddRecallImageName( LPTSTR lpFileName )
/***********************************************************************/
{
FORWARD_WM_ADDRECALL(PictPubApp.Get_hWndAstral(FALSE), lpFileName, SendMessage);
}

/***********************************************************************/
void DelRecallImageName( LPTSTR lpFileName )
/***********************************************************************/
{
}

/***********************************************************************/
BOOL LoadCalMap(LPMAP lpMasterMap, LPMAP lpCMap, LPMAP lpMMap, LPMAP lpYMap, LPMAP lpKMap,
				LPTSTR lpFileName)
/***********************************************************************/
{
	BOOL fIsPP5Map;

	if (LoadMap(lpMasterMap, lpCMap, lpMMap, lpYMap, lpKMap, lpFileName,
				&fIsPP5Map))
	{
		if (!fIsPP5Map)
		{
			if (lpKMap && lpMasterMap)
				*lpKMap = *lpMasterMap;
			if (lpMasterMap)
				ResetMap(lpMasterMap, Mapping.Map_nPoints, NO);

			// to fix bug where 3.1 cal maps are save with a bogus
			// reverse flag
			if (lpCMap && lpCMap->Reverse)
			{
				lpCMap->Reverse = NO;
				MakeMap(lpCMap);
			}
			if (lpMMap && lpMMap->Reverse)
			{
				lpMMap->Reverse = NO;
				MakeMap(lpMMap);
			}
			if (lpYMap && lpYMap->Reverse)
			{
				lpYMap->Reverse = NO;
				MakeMap(lpYMap);
			}
			if (lpKMap && lpKMap->Reverse)
			{
				lpKMap->Reverse = NO;
				MakeMap(lpKMap);
			}
		}

		return(TRUE);
	}
	return(FALSE);
}

/***********************************************************************/
BOOL LoadMap( LPMAP lpMap0, LPMAP lpMap1, LPMAP lpMap2, LPMAP lpMap3,
               LPTSTR lpFileName )
/***********************************************************************/
{
	return(LoadMap(lpMap0,lpMap1,lpMap2,lpMap3,NULL,lpFileName));
}

/***********************************************************************/
BOOL LoadMap( LPMAP lpMap0, LPMAP lpMap1, LPMAP lpMap2, LPMAP lpMap3, LPMAP lpMap4,
               LPTSTR lpFileName )
/***********************************************************************/
{
BOOL fIsPP5Map;

return(LoadMap(lpMap0, lpMap1, lpMap2, lpMap3, lpMap4, lpFileName, &fIsPP5Map));
}

/***********************************************************************/
LOCAL BOOL LoadMap( LPMAP lpMap0, LPMAP lpMap1, LPMAP lpMap2, LPMAP lpMap3, LPMAP lpMap4,
               LPTSTR lpFileName, LPBOOL fIsPP5Map )
/***********************************************************************/
{
#define MAX_MAP_SIZE ((4*sizeof(MAP))+3+10)
FILE_HANDLE fp;
MAP map;
int len;
TCHAR buffer[10];

*fIsPP5Map = FALSE;
if ( ( fp = FileOpen( lpFileName, FO_READ ) ) == FILE_HANDLE_INVALID )
	{
	Message( IDS_EOPEN, Lowercase(lpFileName) );
	return( FALSE );
	}

len = FileRead( fp, buffer, 3 );
if (len != 3)
	goto ReadError;

if (StringsEqualN( "MAP", buffer, 3 ) )
	{
	FileClose( fp );
	if (lpMap4)
		ResetMap(lpMap4, MAXPOINTS, NO);
	return(Load31Map(lpMap0, lpMap1, lpMap2, lpMap3, lpFileName));
	}
else if (StringsEqualN( "MP4", buffer, 3 ) )
	{
	ResetMap(&map, MAXPOINTS, NO);
	if (!Read40Map(fp, &map))
		goto ReadError;
	if ( lpMap0 )
		copy((LPTR)&map, (LPTR)lpMap0, sizeof(MAP));
	if (!Read40Map(fp, &map))
		goto ReadError;
	if ( lpMap1 )
		copy((LPTR)&map, (LPTR)lpMap1, sizeof(MAP));
	if (!Read40Map(fp, &map))
		goto ReadError;
	if ( lpMap2 )
		copy((LPTR)&map, (LPTR)lpMap2, sizeof(MAP));
	if (!Read40Map(fp, &map))
		goto ReadError;
	if ( lpMap3 )
		copy((LPTR)&map, (LPTR)lpMap3, sizeof(MAP));
	// last map is optional
	if (lpMap4)
	{
		if (Read40Map(fp, &map))
			copy((LPTR)&map, (LPTR)lpMap4, sizeof(MAP));
		else
			ResetMap(lpMap4, MAXPOINTS, NO);
	}
	FileClose( fp );
	}
else if (StringsEqualN( "MP5", buffer, 3 ) )
	{
	*fIsPP5Map = TRUE;
	ResetMap(&map, MAXPOINTS, NO);
	if (!Read50Map(fp, &map))
		goto ReadError;
	if ( lpMap0 )
		copy((LPTR)&map, (LPTR)lpMap0, sizeof(MAP));
	if (!Read50Map(fp, &map))
		goto ReadError;
	if ( lpMap1 )
		copy((LPTR)&map, (LPTR)lpMap1, sizeof(MAP));
	if (!Read50Map(fp, &map))
		goto ReadError;
	if ( lpMap2 )
		copy((LPTR)&map, (LPTR)lpMap2, sizeof(MAP));
	if (!Read50Map(fp, &map))
		goto ReadError;
	if ( lpMap3 )
		copy((LPTR)&map, (LPTR)lpMap3, sizeof(MAP));
	if (!Read50Map(fp, &map))
		goto ReadError;
	if ( lpMap4 )
		copy((LPTR)&map, (LPTR)lpMap4, sizeof(MAP));
	FileClose( fp );
	}
else
	goto ReadError;

return(TRUE);

ReadError:
FileClose( fp );
Message( IDS_EREAD, Lowercase(lpFileName) );
return( FALSE );
}

/***********************************************************************/
static void OldToNewMap( LPOLDMAP lpOldMap, LPMAP lpNewMap)
/***********************************************************************/
{
	lpNewMap->Reverse		= lpOldMap->Reverse;
	lpNewMap->Levels		= lpOldMap->Levels;
	lpNewMap->Negate		= lpOldMap->Negate;
	lpNewMap->Threshold		= lpOldMap->Threshold;
	lpNewMap->Points		= lpOldMap->Points;
	lpNewMap->Contrast		= lpOldMap->Contrast;
	lpNewMap->Brightness[0]	= lpOldMap->Brightness;
	lpNewMap->Brightness[1] = 0;
	lpNewMap->Brightness[2] = 0;
	lpNewMap->Brightness[3] = 0;
	lpNewMap->bStretch		= lpOldMap->bStretch;
	lpNewMap->bModified		= lpOldMap->bModified;
	copy((LPTR)lpOldMap->Pnt, (LPTR)lpNewMap->Pnt, MAXPOINTS*sizeof(POINT));
	copy(lpOldMap->Lut, lpNewMap->Lut, LUTSIZE);
	lpNewMap->bReverseHMS 	= FALSE;
}


/***********************************************************************/
static BOOL Load31Map( LPMAP lpMap0, LPMAP lpMap1, LPMAP lpMap2, LPMAP lpMap3,
                        LPTSTR lpFileName )
/***********************************************************************/
{
DWORD len;
FILE_HANDLE fp;
MAP map;
TCHAR buffer[10];

if ( ( fp = FileOpen( lpFileName, FO_READ ) ) == FILE_HANDLE_INVALID )
	{
	Message( IDS_EOPEN, Lowercase(lpFileName) );
	return( FALSE );
	}

len = FileRead( fp, buffer, 3 );
if (len != 3 || !StringsEqualN( "MAP", buffer, 3 ) )
	goto ReadError;

ResetMap(&map, MAXPOINTS, NO);
if (!Read31Map(fp, &map))
	goto ReadError;
if ( lpMap0 )
	copy((LPTR)&map, (LPTR)lpMap0, sizeof(MAP));
if (!Read31Map(fp, &map))
	goto ReadError;
if ( lpMap1 )
	copy((LPTR)&map, (LPTR)lpMap1, sizeof(MAP));
if (!Read31Map(fp, &map))
	goto ReadError;
if ( lpMap2 )
	copy((LPTR)&map, (LPTR)lpMap2, sizeof(MAP));
if (!Read31Map(fp, &map))
	goto ReadError;
if ( lpMap3 )
	copy((LPTR)&map, (LPTR)lpMap3, sizeof(MAP));
FileClose( fp );
return( TRUE );

ReadError:
FileClose( fp );
Message( IDS_EREAD, Lowercase(lpFileName) );
return( FALSE );
}

/***********************************************************************/
LOCAL BOOL Read31Map(FILE_HANDLE fh, LPMAP lpMap)
/***********************************************************************/
{
int i;
INT16 v16;

if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Reverse = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Levels = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Negate = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Threshold = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Points = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Contrast = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Brightness[0] = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->bStretch = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->bModified = v16;
for (i = 0; i < 12; ++i)
	{
	if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
		return(FALSE);
	lpMap->Pnt[i].x = v16;
	if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
		return(FALSE);
	lpMap->Pnt[i].y = v16;
	}
if (FileRead(fh, lpMap->Lut, LUTSIZE) != LUTSIZE)
	return(FALSE);
return(TRUE);
}

/***********************************************************************/
LOCAL BOOL Read40Map(FILE_HANDLE fh, LPMAP lpMap)
/***********************************************************************/
{
int i;
INT16 v16;

if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Reverse = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Levels = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Negate = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Threshold = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Points = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Contrast = v16;
for (i = 0; i < NUM_BANDS; ++i)
	{
	if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
		return(FALSE);
	lpMap->Brightness[i] = v16;
	}
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->bStretch = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->bModified = v16;
for (i = 0; i < 12; ++i)
	{
	if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
		return(FALSE);
	lpMap->Pnt[i].x = v16;
	if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
		return(FALSE);
	lpMap->Pnt[i].y = v16;
	}
if (FileRead(fh, lpMap->Lut, LUTSIZE) != LUTSIZE)
	return(FALSE);
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->bReverseHMS = v16;
return(TRUE);
}

/***********************************************************************/
LOCAL BOOL Read50Map(FILE_HANDLE fh, LPMAP lpMap)
/***********************************************************************/
{
int i;
INT16 v16;
INT32 w, f;

if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Reverse = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Levels = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Negate = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Threshold = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Points = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->Contrast = v16;
for (i = 0; i < NUM_BANDS; ++i)
	{
	if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
		return(FALSE);
	lpMap->Brightness[i] = v16;
	}
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->bStretch = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->bModified = v16;
for (i = 0; i < MAXPOINTS; ++i)
	{
	if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
		return(FALSE);
	lpMap->Pnt[i].x = v16;
	if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
		return(FALSE);
	lpMap->Pnt[i].y = v16;
	}
if (FileRead(fh, lpMap->Lut, LUTSIZE) != LUTSIZE)
	return(FALSE);
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->bReverseHMS = v16;
if (FileRead(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
lpMap->iCurves = v16;
if (FileRead(fh, &w, sizeof(INT32)) != sizeof(INT32))
	return(FALSE);
if (FileRead(fh, &f, sizeof(INT32)) != sizeof(INT32))
	return(FALSE);
lpMap->gamma = DBLMAKEWHOLE(w);
lpMap->gamma += DBLMAKEFRACTION(f);

return(TRUE);
}

/***********************************************************************/
LOCAL BOOL Write40Map(FILE_HANDLE fh, LPMAP lpMap)
/***********************************************************************/
{
int i;
INT16 v16;

v16 = lpMap->Reverse;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
v16 = lpMap->Levels;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
v16 = lpMap->Negate;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
v16 = lpMap->Threshold;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
v16 = lpMap->Points;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
v16 = lpMap->Contrast;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
for (i = 0; i < NUM_BANDS; ++i)
	{
	v16 = lpMap->Brightness[i];
	if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
		return(FALSE);
	}
v16 = lpMap->bStretch;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
v16 = lpMap->bModified;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
for (i = 0; i < MAXPOINTS; ++i)
	{
	v16 = lpMap->Pnt[i].x;
	if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
		return(FALSE);
	v16 = lpMap->Pnt[i].y;
	if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
		return(FALSE);
	}
if (FileWrite(fh, lpMap->Lut, LUTSIZE) != LUTSIZE)
	return(FALSE);
v16 = lpMap->bReverseHMS;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
return(TRUE);
}

/***********************************************************************/
LOCAL BOOL Write50Map(FILE_HANDLE fh, LPMAP lpMap)
/***********************************************************************/
{
int i;
INT16 v16;
INT32 v32;

v16 = lpMap->Reverse;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
v16 = lpMap->Levels;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
v16 = lpMap->Negate;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
v16 = lpMap->Threshold;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
v16 = lpMap->Points;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
v16 = lpMap->Contrast;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
for (i = 0; i < NUM_BANDS; ++i)
	{
	v16 = lpMap->Brightness[i];
	if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
		return(FALSE);
	}
v16 = lpMap->bStretch;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
v16 = lpMap->bModified;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
for (i = 0; i < MAXPOINTS; ++i)
	{
	v16 = lpMap->Pnt[i].x;
	if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
		return(FALSE);
	v16 = lpMap->Pnt[i].y;
	if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
		return(FALSE);
	}
if (FileWrite(fh, lpMap->Lut, LUTSIZE) != LUTSIZE)
	return(FALSE);
v16 = lpMap->bReverseHMS;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
v16 = lpMap->iCurves;
if (FileWrite(fh, &v16, sizeof(INT16)) != sizeof(INT16))
	return(FALSE);
v32 = DBLWHOLE(lpMap->gamma);
if (FileWrite(fh, &v32, sizeof(INT32)) != sizeof(INT32))
	return(FALSE);
v32 = DBLFRACTION32(lpMap->gamma);
if (FileWrite(fh, &v32, sizeof(INT32)) != sizeof(INT32))
	return(FALSE);
return(TRUE);
}

/***********************************************************************/
BOOL SaveMap( LPMAP lpMap0, LPMAP lpMap1, LPMAP lpMap2, LPMAP lpMap3,
               LPTSTR lpFileName )
/***********************************************************************/
{
	return (SaveMap(lpMap0,lpMap1,lpMap2,lpMap3,NULL,lpFileName));
}

/***********************************************************************/
BOOL SaveMap( LPMAP lpMap0, LPMAP lpMap1, LPMAP lpMap2, LPMAP lpMap3, LPMAP lpMap4,
               LPTSTR lpFileName )
/***********************************************************************/
{
FILE_HANDLE fp;
MAP NullMap;

if ( ( fp = FileOpen( lpFileName, FO_CREATE|FO_WRITE ) ) == FILE_HANDLE_INVALID )
	{
	Message( IDS_ECREATE, Lowercase(lpFileName) );
	return( FALSE );
	}

ResetMap( &NullMap, MAPPOINTS, NO );
if ( lpMap0 )
	MakeMap( lpMap0 );
else	{
	lpMap0 = &NullMap;
	}
if ( lpMap1 )
	MakeMap( lpMap1 );
else	{
	lpMap1 = &NullMap;
	}
if ( lpMap2 )
	MakeMap( lpMap2 );
else	{
	lpMap2 = &NullMap;
	}
if ( lpMap3 )
	MakeMap( lpMap3 );
else	{
	lpMap3 = &NullMap;
	}
if ( lpMap4 )
	MakeMap( lpMap4 );
else	{
	lpMap4 = &NullMap;
	}
	
if (FileWrite( fp, "MP5", 3 ) != 3)
	goto WriteError;
if (!Write50Map(fp, lpMap0))
	goto WriteError;
if (!Write50Map(fp, lpMap1))
	goto WriteError;
if (!Write50Map(fp, lpMap2))
	goto WriteError;
if (!Write50Map(fp, lpMap3))
	goto WriteError;
if (!Write50Map(fp, lpMap4))
	goto WriteError;
FileClose( fp );
return( TRUE );


WriteError:
FileClose(fp);
Message( IDS_EWRITE, Lowercase(lpFileName) );
return( FALSE );
}


/***********************************************************************/
ITEMID GetPPFileType( LPTSTR lpFileName )
/***********************************************************************/
{
ITEMID Type;
LPTSTR lp;
TCHAR tmp[4];

tmp[0] = tmp[1] = tmp[2] = tmp[3] = '\0';
lp = stripdir(lpFileName);
while ( *lp )
	{ // check for an extension
	if ( *lp++ != '.' )
		continue;
	// put extension into local buffer, can make lower case
	if ( *lp )
		tmp[0] = tolower(*lp++);
	if ( *lp )
		tmp[1] = tolower(*lp++);
	if ( *lp )
		tmp[2] = tolower(*lp++);
	break;
	}

// Force the file type if the extension is a known one
Type = 0;
if ( StringsEqual( "sct", tmp ) && Control.Retail ) Type = IDN_SCITEX;	else
if ( StringsEqual( "eps", tmp ) ) Type = IDN_EPS;	else    
if ( StringsEqual( "tif", tmp ) ) Type = IDN_TIFF;	else
if ( StringsEqual( "bmp", tmp ) ) Type = IDN_BMP;	else
if ( StringsEqual( "dib", tmp ) ) Type = IDN_DIB;	else
if ( StringsEqual( "gif", tmp ) ) Type = IDN_GIF;	else
if ( StringsEqual( "pcx", tmp ) ) Type = IDN_PCX;	else
if ( StringsEqual( "tga", tmp ) ) Type = IDN_TARGA;	else
if ( StringsEqual( "jpg", tmp ) ) Type = IDN_JPEG;	else
if ( StringsEqual( "jif", tmp ) ) Type = IDN_JPEG;	else
if ( StringsEqual( "avi", tmp ) ) Type = IDN_AVI;	else
if ( StringsEqual( "pcd", tmp ) ) Type = IDN_PHOTOCD; else
if ( StringsEqual( "dcs", tmp ) ) Type = IDN_DCS; 	else
if ( StringsEqual( "pp5", tmp ) ) Type = IDN_PP; 	else
if ( StringsEqual( "ppf", tmp ) ) Type = IDN_PPCOMPOUND; 	else
if ( StringsEqual( "pp4", tmp ) ) Type = IDN_PP4;   else
if ( StringsEqual( "ai", tmp ) )  Type = IDN_AI;	else    
if ( StringsEqual( "cdr", tmp ) ) Type = IDN_CDR;	else
if ( StringsEqual( "cgm", tmp ) ) Type = IDN_CGM;	else
if ( StringsEqual( "drw", tmp ) ) Type = IDN_DRW;   else
if ( StringsEqual( "wmf", tmp ) ) Type = IDN_WMF;

return(Type);
}

/***********************************************************************/

ITEMID GetFrameFileDataType(LPFRAME lpFrame)
{
	ITEMID idDataType;

	switch(FrameType(lpFrame))
	{
		case FDT_LINEART :
			idDataType = IDC_SAVELA;
		break;

		case FDT_GRAYSCALE :
			idDataType = IDC_SAVECT;
		break;

		case FDT_PALETTECOLOR :
			idDataType = IDC_SAVE8BITCOLOR;
		break;

		case FDT_RGBCOLOR :
			idDataType = IDC_SAVE24BITCOLOR;
		break;

		case FDT_CMYKCOLOR :
			idDataType = IDC_SAVE32BITCOLOR;
		break;
	}

	return(idDataType);
}

/***********************************************************************/
//	makes sure the volume is in the drive for the given file.  If the 
//	file is there, but the volume is wrong, the user may still choose to open.
//	Returns TRUE if the open/save should continue.  
//	Returns FALSE if the operation was canceled.
/***********************************************************************/
BOOL PromptForVolume(LPTSTR lpFileName, LPVOLUMEID lpVolume, BOOL bLoad)
/***********************************************************************/
{
	BOOL bRemovable;
	int iRet;
	
	while (TRUE)
	{
		if (NonBSRVolumeCheck(lpFileName, lpVolume, &bRemovable))
		{
		 	if (SafeFileExists(lpFileName))
			{
				// check for read only 
				if (bLoad && !FilePermission(lpFileName, 2L/*Write*/))
				{
					if (GetPPFileType(lpFileName) != IDN_PHOTOCD)
					{
                        if( Browser.WarnReadOnly )
                        {
						    if (AstralOKCancel(IDS_WARN_READONLY, lpFileName) != IDOK)
							    return(FALSE);
                        }
					}
				}
				return(TRUE);
			}
			if (!bRemovable)
				return(TRUE);   // will give file not found later in load case
			if (AstralOKCancel(IDS_EVOLUME, lpFileName) !=IDOK)
				return(FALSE);
		}
		else //Wrong volume
		{
		 	if (SafeFileExists(lpFileName))
			{
				//Wrong volume correct file
				iRet = AstralConfirm(IDS_EFILE_VOLUME, lpFileName);
				if (iRet == IDNO)
					continue;
				else if (iRet == IDYES)
				{
					// check for read only 
					if (bLoad && !FilePermission(lpFileName, 2L/*Write*/))
					{
						if (GetPPFileType(lpFileName) != IDN_PHOTOCD)
						{
                            if( Browser.WarnReadOnly )
                            {
							    if (AstralOKCancel(IDS_WARN_READONLY, lpFileName) != IDOK)
							    	return(FALSE);
                            }
						}
					}
					return(TRUE);
				}
				else
					return(FALSE);
			}
			else if (AstralOKCancel(IDS_EVOLUME, lpFileName) !=IDOK)
				return(FALSE);
		}
	}
	return(FALSE);	
}

//************************************************************************
// used in ForEach1
//************************************************************************
LOCAL BOOL polylist_flip(LPCPoly lpPoly, LONG lDelta)
{
	POINT pt;
	
	if (lpPoly->NumPoints() <= 0)
		return(TRUE);
	lpPoly->GetPoint(-1);
	while(lpPoly->ListNextPoint(&pt))
	{
		pt.y = lDelta-pt.y;
		lpPoly->SetCurrentPoint(&pt);
	}
	return(TRUE);
}

//************************************************************************
//			LoadShape
// PARAMETERS:
//				LPTSTR lpFileName- Name of file to load.
//				int res- the resolution of the points.
// DESCRIPTION:
//			Loads a Poly list from the given file.  
//			We can read a subset of AI files.
//			The caller should delete the LPCPolyList and all its Polys.
// RETURNS:
//			NULL for failure.
//************************************************************************
LPCPolyList LoadShape(LPTSTR lpFileName, int res)
//************************************************************************
{
	LPCPolyList	lpList = NULL;
	CStdioFile File;
	CFileException e;
	RECT rBounds;
	
	// gets closed at destructor	
	if( !File.Open( lpFileName, CFile::modeRead | CFile::typeBinary /*CFile::shareDenyWrite*/, &e ) )
	{
		Message( IDS_EOPEN, Lowercase(lpFileName) );
		return(NULL);
	}

	TRY_BLOCK
	// find beginning
//	if (!shape_find_begin(&File))
//		goto ReadError;
	if (!(lpList = new CPolyList))
		goto ReadError;
	// keep reading 'till we fail.
	while(shape_read_poly(&File, lpList, res));
	if (lpList->IsEmpty())
	{
		FreePolyList(lpList);
		lpList = NULL;	
	}
	else
	{
		// now flip the shape
		PolyListGetBounds(lpList, &rBounds);
		lpList->ForEach1((LPLISTPROC1)polylist_flip, (long)(rBounds.bottom+rBounds.top)); 
	}
	return(lpList);
ReadError:
   	if (lpList)
   		FreePolyList(lpList);
   	Message( IDS_EREAD, Lowercase(lpFileName) );
   	return(NULL);

	CATCH_BLOCK
	goto ReadError; 
	CATCH_END
}

//************************************************************************
//		shape_read_poly
// DESCRIPTION:
//		Reads from the current position to the end of the current group.
// RETURNS:
//		FALSE if EOF was reached.
// EXCEPTIONS:
//		May throw a CFileException.
//************************************************************************
LOCAL BOOL shape_read_poly(CStdioFile *lpFile, LPCPolyList lpList, int res)
//************************************************************************
{
	LPCPoly lpPoly;
	TCHAR String[100];
	TCHAR c;
	int num;
	POINT pts[3];
	BOOL bClose = FALSE;
	BOOL bDone = FALSE;
	BOOL bEnd = FALSE;
	LPTSTR lpStr = String;
	int count = 0;
	
	// create polygon
	if (!(lpPoly = new CPoly))
		return(FALSE);
	// read chars
	while (!bDone && lpFile->Read(&c, 1))
	{
		// are we lloking for the end?
		if (bEnd && (c != '\r' && c != '\n'))
			continue;
		// force an end of line if too large
		if (++count >= 100)
			c = '\n';
		switch (c)
		{
			case '%':
			case '//':
				// comment
				bEnd = TRUE;
			break;
			case 'm':
			case 'l':
			case 'L':
				// single point
				num = get_values(String, pts, 1, res);
				if (num >= 1)
					lpPoly->AddPoint(&pts[0]);
				bEnd = TRUE;
			break;
			case 'v':
			case 'V':
			case 'y':
			case 'Y':
				// not sure if this is right
				num = get_values(String, pts, 2, res);
				if (num >= 2)
					lpPoly->AddBezierLine(&pts[0], &pts[0], &pts[1]);
				bEnd = TRUE;
			break;
			case 'c':
			case 'C':
				// curve
				num = get_values(String, pts, 3, res);
				if (num >= 3)
					lpPoly->AddBezierLine(&pts[0], &pts[1], &pts[2]);
				bEnd = TRUE;
			break;
			case 'n':
			case 's':
			case 'b':
			case 'f':
			case 'q':
			case 'h':
				// its a closed end
				bDone = TRUE;
				bClose = TRUE;
			break;
			
			case 'H':
			case 'N':
			case 'S':
			case 'B':
			case 'F':
			case 'Q':
				// its an opened end
				bDone = TRUE;
				bClose = FALSE;
			break;
			
			case '\n':
			case '\r':
				// end of line
				lpStr = String;
				count = 0;
				bEnd = FALSE;
			break;
			default:
				if (!WITHIN(c,'0','9') && c != '.' && c !=' ') 
					bEnd = TRUE;
				*lpStr++ = c;
			break;	
		}
	}
	if (lpPoly->NumPoints())
	{
		lpPoly->ClosePoly(bClose);
		lpList->AddTail(lpPoly);
	}
	else
		delete lpPoly;
	return(bDone);	
}


//************************************************************************
//		get_values
// PARAMETERS:
//	  			LPTSTR lpStr- The string to read. 
//	  			LPPOINT lpPoints- The array to put values into.
//	  			int nMax- The size of lpValues.
//				int res- to convert to file coords.
// DESCRIPTION:
//		Parses the values from the given sting into lpPoints.
//		For reading from ai files.  Converts to file units.
// RETURNS:
//		The number of entries filled
//************************************************************************
LOCAL int get_values(LPTSTR lpStr, LPPOINT lpPoints, int nMax, int res)
//************************************************************************
{
	int num;
	double value;
	BOOL bTranslated;
	
	num = 0;
	lpStr = SkipSpaces(lpStr);
	while(lpStr && *lpStr != '\0' && num < nMax)
	{
		// get x
		value = AsciiDouble(lpStr, &bTranslated);
		if (!bTranslated)
			return(num);
		lpPoints[num].x = (int)(res*value/72.0);
		if (!(lpStr = lStrChr(lpStr, ' ')))
			break;
		lpStr = SkipSpaces(lpStr);
		value = AsciiDouble(lpStr, &bTranslated);
		if (!bTranslated)
			return(num);
		// get y
		lpPoints[num].y = (int)(res*value/72.0);
		if (!(lpStr = lStrChr(lpStr, ' ')))
			break;
		lpStr = SkipSpaces(lpStr);
		num++;
	}
	return(num);
}


//************************************************************************
//			SaveShape
// PARAMETERS:
//				LPCPolyList lpPolyList- The poly list to save.
//	 			LPTSTR lpFileName- The File to save it to.
//				int res- the resolution of the points.
// DESCRIPTION:
//			Saves the given poly list as a simple AI file.
// RETURNS:
//			FALSE for failure.
//************************************************************************
BOOL SaveShape(LPCPolyList lpPolyList,  LPTSTR lpFileName, int res)
//************************************************************************
{
	CStdioFile File;
	CFileException e;
	POSITION pos;
	RECT rBounds;
	LPCPoly lpPoly;
	
	if (!lpPolyList || lpPolyList->IsEmpty())
		return(FALSE);
	TRY_BLOCK
	// gets closed at destructor	
	if( !File.Open( lpFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText, &e ) )
		goto WriteError;

	
	// write header info (including u)
	if (!shape_write_header(&File, lpPolyList, res))
		goto WriteError;

	PolyListGetBounds(lpPolyList, &rBounds);

	// write out polys
	pos = lpPolyList->GetHeadPosition();
	while (pos)
	{
		lpPoly = (LPCPoly)lpPolyList->GetNextPtr(pos);
		if (!lpPoly)
			break;
		if (!shape_write_poly(lpPoly, &File, &rBounds, res))
			goto WriteError;
	}
	// write out trailer info (including U)
	if (!shape_write_trailer(&File))
		goto WriteError;
	return(TRUE);
WriteError:
   	Message( IDS_EWRITE, Lowercase(lpFileName) );
   	return(NULL);
	
	CATCH_BLOCK
	goto WriteError;
	CATCH_END
}


//************************************************************************
//			shape_write_poly
// DESCRIPTION:
//		Writes the given polygon to the given file.
//		Called from ForEach1 call.
// EXCEPTIONS:
//		May throw a CFileException.
//************************************************************************
LOCAL BOOL shape_write_poly(LPCPoly lpPoly, CStdioFile *lpFile, LPRECT lpBounds, int res)
//************************************************************************
{
	BOOL bAbort;
	POINT pts[3];
	TCHAR String[100];
	TCHAR szNums[6][25];
	int iBottom;
	
	iBottom = lpBounds->bottom + lpBounds->top;
		
	if (lpPoly->NumPoints() <= 0)
		return(TRUE);
		
	// write out begin point
	lpPoly->GetPoint(0, &pts[0]);
	pts[0].y = iBottom - pts[0].y;
	point_to_strings(pts[0].x, pts[0].y, szNums[0], szNums[1], res); 
	wsprintf(String, "%s %s m\n", szNums[0], szNums[1]);
	lpFile->WriteString(String);
	// write out points and beziers
	bAbort = FALSE;
	while(!bAbort && lpPoly->ListNextPoint(&pts[0]))
	{
		pts[0].y = iBottom - pts[0].y;
		if (lpPoly->IsHandle())
		{
			// get other handle
			lpPoly->ListNextPoint(&pts[1]);
			pts[1].y = iBottom - pts[1].y;
			// get endpoint
			if (!lpPoly->ListNextPoint(&pts[2]))
			{
				lpPoly->NextPoint(&pts[2]);
				bAbort = TRUE;
			}
			pts[2].y = iBottom - pts[2].y;
			// convert
			point_to_strings(pts[0].x, pts[0].y, szNums[0], szNums[1], res); 
			point_to_strings(pts[1].x, pts[1].y, szNums[2], szNums[3], res); 
			point_to_strings(pts[2].x, pts[2].y, szNums[4], szNums[5], res); 
			// just write out curve
			wsprintf(String, "%s %s %s %s %s %s C\n", szNums[0], szNums[1],
													  szNums[2], szNums[3],
													  szNums[4], szNums[5]);
			
			lpFile->WriteString(String);
		}
		else
		{
			// just write out point
			point_to_strings(pts[0].x, pts[0].y, szNums[0], szNums[1], res); 
			wsprintf(String, "%s %s l\n", szNums[0], szNums[1]);
			lpFile->WriteString(String);
		}
	}
	if (lpPoly->IsClosed())
		lstrcpy(String, (LPTSTR)"s\n");
	else
		lstrcpy(String, (LPTSTR)"S\n");
	lpFile->WriteString(String);
	return(TRUE);
}


//************************************************************************
//		shape_write_header
// DESCRIPTION:
//		Writes standard AI header.
//		Begings group and sets up drawing mode.
// RETURNS:
//		FALSE for error.
// EXCEPTIONS:
//		May throw a CFileException.
//************************************************************************
LOCAL BOOL shape_write_header(CStdioFile *lpFile, LPCPolyList lpPolyList, int res)
//************************************************************************
{
	#define AI_STRING_SIZE 150
	TCHAR Format[AI_STRING_SIZE];
	TCHAR String[AI_STRING_SIZE];
	RECT rect;
	TCHAR szNums[4][25];

	if (!AstralStrEx(IDS_AI_HEADER_BEGIN, String, AI_STRING_SIZE))
		return(FALSE);
	lpFile->WriteString(String);
	
	// bounding box
	if (!AstralStrEx(IDS_AI_HEADER, Format, AI_STRING_SIZE))
		return(FALSE);
	PolyListGetBounds(lpPolyList, &rect);
	point_to_strings(rect.left, rect.top, szNums[0], szNums[1], res); 
	point_to_strings(rect.right, rect.bottom, szNums[2], szNums[3], res); 
	wsprintf(String, Format, szNums[0], szNums[1], szNums[2], szNums[3]);
	lpFile->WriteString(String);
	
	if (!AstralStrEx(IDS_AI_HEADER_END, String, AI_STRING_SIZE))
		return(FALSE);
	lpFile->WriteString(String);
	if (!AstralStrEx(IDS_AI_SETUP, String, AI_STRING_SIZE))
		return(FALSE);
	lpFile->WriteString(String);
	if (!AstralStrEx(IDS_AI_GROUP_BEGIN, String, AI_STRING_SIZE))
		return(FALSE);
	lpFile->WriteString(String);
	return(TRUE);
}

//************************************************************************
//		shape_write_trailer
// DESCRIPTION:
//		Writes standard AI trailer.
//		Ends group and ai file.
// RETURNS:
//		FALSE for error.
// EXCEPTIONS:
//		May throw a CFileException.
//************************************************************************
LOCAL BOOL shape_write_trailer(CStdioFile *lpFile)
//************************************************************************
{
	TCHAR String[AI_STRING_SIZE];
	
	if (!AstralStrEx(IDS_AI_GROUP_END, String, AI_STRING_SIZE))
		return(FALSE);
	lpFile->WriteString(String);
	if (!AstralStrEx(IDS_AI_END, String, AI_STRING_SIZE))
		return(FALSE);
	lpFile->WriteString(String);
	return(TRUE);
}


//************************************************************************
// DESCRIPTION:
//		For AI writer.
//		converts the given points to ai resolution (1/72 inch)
//************************************************************************
void point_to_strings(int x, int y, LPTSTR lpStrX, LPTSTR lpStrY, int res)
//************************************************************************
{
	double fx, fy;
	
	fx = (double)x*72.0/res;
	fy = (double)y*72.0/res;
	DoubleAscii(fx, lpStrX);
	DoubleAscii(fy, lpStrY);
}

//////////////////////////////////////////////////////////////////////////
BOOL SetPPFileExt( LPTSTR lpFileName, ITEMID idFileType )
//  truncates the existing file extension and replaces it with the correct 
//  extension based on the specified file type.
//
//  If file type passed is not in the list, file name extension is set to .TIF 
//  and the caller is warned in the return value to set new file type.
//////////////////////////////////////////////////////////////////////////
{
    BOOL bValidID = TRUE;

    // remove the extension, if it exists
    TCHAR *pDot = strrchr( lpFileName, '.' );
    if( pDot )
        *pDot = '\0';        

    // Force the extension based on the file type
    if ( idFileType == IDN_SCITEX )   lstrcat( lpFileName, ".sct" ); else
    if ( idFileType == IDN_EPS )      lstrcat( lpFileName, ".eps" ); else   
    if ( idFileType == IDN_TIFF )     lstrcat( lpFileName, ".tif" ); else
    if ( idFileType == IDN_BMP )      lstrcat( lpFileName, ".bmp" ); else
    if ( idFileType == IDN_DIB )      lstrcat( lpFileName, ".dib" ); else
    if ( idFileType == IDN_GIF )      lstrcat( lpFileName, ".gif" ); else
    if ( idFileType == IDN_PCX )      lstrcat( lpFileName, ".pcx" ); else
    if ( idFileType == IDN_TARGA )    lstrcat( lpFileName, ".tga" ); else
    if ( idFileType == IDN_JPEG )     lstrcat( lpFileName, ".jpg" ); else
    if ( idFileType == IDN_JPEG )     lstrcat( lpFileName, ".jif" ); else
    if ( idFileType == IDN_AVI )      lstrcat( lpFileName, ".avi" ); else
    if ( idFileType == IDN_PHOTOCD )  lstrcat( lpFileName, ".pcd" ); else
    if ( idFileType == IDN_DCS ) 	  lstrcat( lpFileName, ".dcs" ); else
    if ( idFileType == IDN_PP ) 	  lstrcat( lpFileName, ".pp5" ); else
    if ( idFileType == IDN_PPCOMPOUND )   lstrcat( lpFileName, ".ppf" ); else
    if ( idFileType == IDN_PP4 )      lstrcat( lpFileName, ".pp4" ); else
    if ( idFileType == IDN_AI )	      lstrcat( lpFileName, ".ai" );  else 
    if ( idFileType == IDN_CDR )	  lstrcat( lpFileName, ".cdr" ); else
    if ( idFileType == IDN_CGM )	  lstrcat( lpFileName, ".cgm" ); else
    if ( idFileType == IDN_DRW )      lstrcat( lpFileName, ".drw" ); else
    if ( idFileType == IDN_WMF )      lstrcat( lpFileName, ".wmf" ); else
    {
        // if file type not in the list, set extension to TIF and inform 
        // caller that file type is not in the list.
        lstrcat( lpFileName, ".tif" );
        bValidID = FALSE;
    }
    return( bValidID );
}


