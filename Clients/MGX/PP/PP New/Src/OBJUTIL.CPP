//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "objutil.h"
#include "ppafx.h"
#include "id.h"

/************************************************************************/
LPOBJECT ObjDuplicateObject(LPOBJECT lpObject)
/************************************************************************/
{
FRMTYPEINFO DstTypeInfo;

ASSERT(lpObject);

FrameGetTypeInfo(ObjGetEditFrame(lpObject), &DstTypeInfo);
return(ObjDuplicateObject(lpObject, DstTypeInfo));
}

/************************************************************************/
LPOBJECT ObjDuplicateObject(LPOBJECT lpObject, FRMTYPEINFO DstTypeInfo,
						BOOL bDoCMS, LPFRMTYPEINFO lpSrcTypeInfo)
/************************************************************************/
{
	LPOBJECT lpDupObject;
	LPFRAME lpDataFrame;
	LPFRAME lpAlphaFrame = NULL;
	LPALPHA lpAlpha;
   FRMTYPEINFO TypeInfo;
   FRMDATATYPE FrameType = FDT_GRAYSCALE;
	CMSINFO		CmsInfo;
	BOOL			bRetry;
	int			DataType;

	ASSERT(lpObject);

   FrameGetTypeInfo(ObjGetEditFrame(lpObject), &TypeInfo);
   if (TypeInfo.DataType == FDT_LINEART && Mask.OneBitMask)
      FrameType = FDT_LINEART;

	  if (lpSrcTypeInfo)
	  	FrameSetTypeInfo(lpSrcTypeInfo, FDT_NONE);
	if( IsDstPTSelected( &DstTypeInfo.ptInfo ) &&
		!IsSrcPTSelected( &TypeInfo.ptInfo )  && bDoCMS )
		{
		DataType = GetSaveDataType( TypeInfo );
		if( DataType == IDC_SAVE24BITCOLOR || DataType == IDC_SAVE32BITCOLOR )
			{
			if( DataType == IDC_SAVE24BITCOLOR )
				CmsInfo.src.DataType = FDT_RGBCOLOR;
			else
				CmsInfo.src.DataType = FDT_CMYKCOLOR;
			do
				{
				CCmsPasteDlg PasteDlg( &CmsInfo,
					CWnd::FromHandle( PictPubApp.Get_hWndAstral() ) );
				
				PasteDlg.DoModal();
				if( !PasteDlg.GetReturn() )
					{
					cmsInfoInit( &CmsInfo );
					bRetry = FALSE;
					}
				else
					{
					if( TypeInfo.DataType != CmsInfo.src.DataType )
						Message( IDS_CMS_EFILEPTMM );
					else
						{
						bRetry = FALSE;
						( ObjGetEditFrame( lpObject )	)->ptInfo = CmsInfo.src.ptInfo;
						if (lpSrcTypeInfo)
							*lpSrcTypeInfo = CmsInfo.src;
						}
					}
				}
			while( bRetry );
			}
		else
			cmsInfoInit( &CmsInfo );
		}

	if (!(lpDataFrame = FrameConvert(ObjGetEditFrame(lpObject), DstTypeInfo, NULL)))
		return(NULL);
	if (lpAlpha = ObjGetAlpha(lpObject))
		if (!(lpAlphaFrame = FrameCopy(lpAlpha->Pixmap.EditFrame, NULL)))
			{
			FrameClose(lpDataFrame);
			return(NULL);
			}
	if (!(lpAlpha = MaskCreate(lpAlphaFrame,
							FrameXSize(lpDataFrame),
							FrameYSize(lpDataFrame),
							ON, Control.NoUndo, FrameType)))
		{
		FrameClose(lpDataFrame);
		return(NULL);
		}
	if (!(lpDupObject = ObjCreateFromFrame(lpObject->StorageType, lpDataFrame,
								lpAlpha, &lpObject->rObject, Control.NoUndo)))
		{
		FrameClose(lpDataFrame);
		MaskClose(lpAlpha);
		return(NULL);
		}
	lpDupObject->Opacity = lpObject->Opacity;
	lpDupObject->MergeMode = lpObject->MergeMode;
	lpDupObject->wGroupID = lpObject->wGroupID;
	lpDupObject->bLocked = lpObject->bLocked;
	// FIX if (lpObject->lpText)
   	//lpDupObject->lpText = new CTextObject(*lpObject->lpText);
	return(lpDupObject);
}




