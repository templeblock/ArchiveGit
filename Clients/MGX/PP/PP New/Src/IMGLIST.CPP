//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"

void CImage::ChangeSelection(LPOBJECT lpObject, BOOL fSelect, BOOL fExtendedSel)
{
	ASSERT(lpObject != NULL);
	if (!fExtendedSel && fSelect)
    	ObjDeselectAll(&ObjList);
	ObjSelectObject((LPPRIMOBJECT)lpObject, fSelect);
	AdjustGroupSelection(fSelect);
	SetupMiniViews(NULL, YES);
}

BOOL CImage::SelObject(LPOBJECT lpObject, LPRECT lpRect,
                    BOOL fExtendedSel, BOOL fExcludeBase)
{
	BOOL fSelected, fSelect;

	// deselect anything currently selected if not an extended selection
	if (!fExtendedSel)
    	ObjDeselectAll(&ObjList);

	// Try to select a new object(s)
	fSelected = SelectObjects(lpObject, lpRect, YES, NO, fExcludeBase);

	// nothing selected and not extended select mode, we're done
	if (!fSelected && !fExtendedSel)
    	return(FALSE);

	// if we couldn't select something, try to deselect something instead,
	// but only if we are in extended select mode
	if (fSelected)
    	fSelect = YES;
	else
    	{
    	fSelected = SelectObjects(lpObject, lpRect, NO, NO,
                                	fExcludeBase);
    	fSelect = NO;
    	}

	// nothing selected or deselected, we're done
	if (!fSelected)
    	return(FALSE);

	// if we are selecting/unselecting we need to make sure all
	// grouped objects have the same select/unselect state
	AdjustGroupSelection(fSelect);

	return(fSelected);
}

void CImage::AdjustGroupSelection(BOOL fSelect)
{
	LPOBJECT lpObject, lpNextObject;

	// if we are selecting/unselecting we need to make sure all
	// grouped objects have the same select/unselect state
	lpNextObject = NULL;
	while (lpNextObject = GetNextObject(lpNextObject, YES, NO))
   	{
    	if (lpNextObject->fSelected != fSelect)
        	continue;
    	if (!lpNextObject->wGroupID)
        	continue;
    	lpObject = NULL;
    	while (lpObject = GetNextObject(lpObject, YES, NO))
       	{
        	if (lpObject->wGroupID == lpNextObject->wGroupID)
            	ObjSelectObject((LPPRIMOBJECT)lpObject, lpNextObject->fSelected);
       	}
   	}
	// Now just make sure all grouped objects have the same select state
	lpNextObject = NULL;
	while (lpNextObject = GetNextObject(lpNextObject, YES, NO))
   	{
    	if (!lpNextObject->wGroupID)
        	continue;
    	lpObject = lpNextObject;
    	while (lpObject = GetNextObject(lpObject, YES, NO))
       	{
        	if (lpObject->wGroupID == lpNextObject->wGroupID)
            	ObjSelectObject((LPPRIMOBJECT)lpObject, lpNextObject->fSelected);
       	}
   	}
}

BOOL CImage::SelectObjects(LPOBJECT lpStartObject, LPRECT lpRect,
                BOOL fSelect, BOOL fBottomUp, BOOL fExcludeBase)
{
	BOOL        fHaveRect, fSelected;
	BOOL        fLoop;
	LPOBJECT    lpObject, lpBase;

	if (lpRect)
    	fHaveRect = lpRect->left < lpRect->right;
	fLoop = lpStartObject != NULL;
	lpObject = lpStartObject;
	fSelected = FALSE;
	lpBase = GetBase();
	while (lpObject = GetNextObject(lpObject, fBottomUp, fLoop))
    {
    	if (!fExcludeBase || lpObject != lpBase)
       	{
        	if (!lpRect)
           	{
            	if (fSelect != lpObject->fSelected)
               	{
                	ObjSelectObject((LPPRIMOBJECT)lpObject, fSelect);
                	fSelected = TRUE;
               	}
           	}
        	else if (fHaveRect)
           	{
             	if (lpObject->rObject.left >= lpRect->left &&
                	lpObject->rObject.right <= lpRect->right &&
                	lpObject->rObject.top >= lpRect->top &&
                	lpObject->rObject.bottom <= lpRect->bottom)
               	{
                	if (fSelect != lpObject->fSelected)
                   	{
                    	ObjSelectObject((LPPRIMOBJECT)lpObject, fSelect);
                    	fSelected = TRUE;
                   	}
               	}
           	}
        	else
           	{
            	if (AstralPtInRect(&lpObject->rObject, *((LPPOINT)lpRect)))
               	{
                	if (lpObject == lpBase || !lpObject->lpAlpha ||
                    	IsMasked(lpObject->lpAlpha,
                            	lpRect->left-lpObject->rObject.left,
                            	lpRect->top-lpObject->rObject.top))
                   	{
                    	if (fSelect != lpObject->fSelected)
                       	{
                        	ObjSelectObject((LPPRIMOBJECT)lpObject, fSelect);
                        	fSelected = TRUE;
                       	}
                    	break;
                   	}
               	}
           	}
       	}
    	if (lpObject == lpStartObject)
        	break;
    }
	return(fSelected);
}

BOOL CImage::MultipleObjects()
{
	LPOBJECT lpObject = NULL;
	int nCount = 0;

	while (lpObject = GetNextObject(lpObject, YES, NO))
   	{
    	++nCount;
    	if (nCount > 1)
        	return(TRUE);
   	}
	return(FALSE);
}

int CImage::CountSelObjects(LPOBJECT lpObject)
{			  
	int nCount = 0;

    while (lpObject = GetSelObject(lpObject))
        ++nCount;
	return(nCount);
}

int CImage::CountObjects()
{
	LPOBJECT lpObject = NULL;
	int nCount = 0;

   	while (lpObject = GetNextObject(lpObject, YES, NO))
       	++nCount;
	return(nCount);
}

LPOBJECT CImage::GetSelObject(LPOBJECT lpObject, BOOL fBottomUp)
{
	LPOBJECT lpSelObject = NULL;

	lpSelObject = GetNextObject(lpObject, fBottomUp, NO);
	if (Control.UseMaskAndObjects || !GetMask())
   	{
    	while (lpSelObject && !lpSelObject->fSelected)
        	lpSelObject = GetNextObject(lpSelObject, fBottomUp, NO);
    	if (!lpSelObject && !lpObject)
        	lpSelObject = GetBase();
   	}
	return(lpSelObject);
}

LPOBJECT CImage::GetNextObject(LPOBJECT lpObject,
                        BOOL fBottomUp,
						BOOL fLoop,
						BOOL fGetDeletedObjects)
{
	LPOBJECT lpNextObject = NULL;

	lpNextObject = (LPOBJECT)ObjGetNextObject(&ObjList,
                	(LPPRIMOBJECT)lpObject, fBottomUp);
	while (lpNextObject && (lpNextObject->ObjectType != OT_FRAME ||
        	(lpNextObject->fDeleted && !fGetDeletedObjects)))
    	lpNextObject = (LPOBJECT)ObjGetNextObject(&ObjList,
                    	(LPPRIMOBJECT)lpNextObject, fBottomUp);
	if (fLoop && !lpNextObject)
   	{
    	lpNextObject = (LPOBJECT)ObjGetNextObject(&ObjList,
                        	NULL, fBottomUp);
    	while (lpNextObject && (lpNextObject->ObjectType != OT_FRAME ||
        	(lpNextObject->fDeleted && !fGetDeletedObjects)))
        	lpNextObject = (LPOBJECT)ObjGetNextObject(&ObjList,
                    	(LPPRIMOBJECT)lpNextObject, fBottomUp);
   	}
	return(lpNextObject);
}

void CImage::InvertSelection()
{
	LPOBJECT lpObject;

	lpObject = GetBase();
	while (lpObject = GetNextObject(lpObject, YES, NO))
    	ObjSelectObject((LPPRIMOBJECT)lpObject, !lpObject->fSelected);
	SetupMiniViews(NULL, YES);          
}

BOOL CImage::IsSelectedObject(LPOBJECT lpObject)
{
	LPOBJECT lpNext = NULL;

	while (lpNext = GetSelObject(lpNext))
   	{
    	if (lpNext == lpObject)
        	return(TRUE);
   	}
	return(FALSE);
}

LPOBJECT CImage::FindSelObject(int x, int y, BOOL fBottomUp)
{
	POINT pt;
	LPOBJECT lpObject = NULL;
	LPOBJECT lpBase;

	pt.x = x;
	pt.y = y;
	lpBase = GetBase();
	while (lpObject = GetSelObject(lpObject, fBottomUp))
   	{
    	if (AstralPtInRect(&lpObject->rObject, pt))
       	{
        	if (lpObject == lpBase)
            	return(lpObject);
        	if (!lpObject->lpAlpha)
            	return(lpObject);
        	if (IsMasked(lpObject->lpAlpha,
                    	x-lpObject->rObject.left,
                    	y-lpObject->rObject.top))
            	return(lpObject);
       	}
   	}
	return(NULL);
}

LPOBJECT CImage::FindObject(int x, int y, BOOL fBottomUp, BOOL fExcludeBase)
{
	POINT pt;
	LPOBJECT lpObject = NULL;
	LPOBJECT lpBase;

	pt.x = x;
	pt.y = y;
	lpBase = GetBase();
	while (lpObject = GetNextObject(lpObject, fBottomUp, NO))
   	{
    	if (fExcludeBase && lpObject == lpBase)
        	continue;
    	if (AstralPtInRect(&lpObject->rObject, pt))
       	{
        	if (lpObject == lpBase)
            	return(lpObject);
        	if (!lpObject->lpAlpha)
            	return(lpObject);
        	if (IsMasked(lpObject->lpAlpha,
                    	x-lpObject->rObject.left,
                    	y-lpObject->rObject.top))
            	return(lpObject);
       	}
   	}
	return(NULL);
}

void CImage::AdjustLayerSelObj(BOOL fToTop, BOOL fOneLevel)
{
	LPOBJECT lpObject, lpFirstObject, lpNext, lpBase, lpLinkObject;

	lpBase = GetBase();
	if (fToTop)
    	lpObject = GetNextObject(lpBase, YES, NO);
	else
    	lpObject = GetNextObject(NULL, NO, NO);
	if (fOneLevel)
   	{
    	while (lpObject && lpObject != lpBase)
       	{
        	// find first selected object
        	if (!lpObject->fSelected)
           	{
            	lpObject = GetNextObject(lpObject, fToTop, NO);
            	continue;
           	}
        	// got a selected object, now find next unselected object to link after
        	lpLinkObject = GetNextObject(lpObject, fToTop, NO);
        	while (lpLinkObject && lpLinkObject->fSelected)
            	lpLinkObject = GetNextObject(lpLinkObject, fToTop, NO);
        	// if no unselected objects to link before or after, nothing to do
        	if (!lpLinkObject)
            	break;
        	// can't move objects below base
        	if (lpLinkObject == lpBase && !fToTop)
            	break;
        	// now move all consecutive selected objects up/down one level
        	while (lpObject && lpObject->fSelected)
           	{
            	lpNext = GetNextObject(lpObject, fToTop, NO);
            	ObjUnlinkObject(&ObjList, (LPPRIMOBJECT)lpObject);
            	ObjLinkObject(&ObjList, (LPPRIMOBJECT)lpLinkObject,
                        	(LPPRIMOBJECT)lpObject, fToTop);
            	lpLinkObject = lpObject;
            	lpObject = lpNext;
           	}
        	lpObject = GetNextObject(lpLinkObject, fToTop, NO);
       	}
   	}
	else
   	{
        	// find first selected object
    	while (lpObject)
       	{
        	if (lpObject->fSelected)
            	break;
        	lpObject = GetNextObject(lpObject, fToTop, NO);
       	}
    	lpFirstObject = lpObject;
    	while (lpObject)
       	{
        	lpNext = GetNextObject(lpObject, fToTop, NO);
        	if (lpObject->fSelected && (lpObject != lpBase))
           	{
            	ObjUnlinkObject(&ObjList, (LPPRIMOBJECT)lpObject);
            	if (fToTop)
                	ObjLinkObject(&ObjList,
                            	(LPPRIMOBJECT)ObjList.lpTail,
                            	(LPPRIMOBJECT)lpObject, YES);
            	else
                	ObjLinkObject(&ObjList,
                            	(LPPRIMOBJECT)ObjList.lpHead,
                            	(LPPRIMOBJECT)lpObject, YES);
           	}
        	lpObject = lpNext;
        	if (lpObject == lpFirstObject)
            	break;
       	}
   	}
}

void CImage::AdjustLayerObject(LPOBJECT lpObject,
                        LPOBJECT lpToObject, BOOL fAfter)
{
	LPOBJECT lpBase;

	if (!lpObject || !lpToObject)
    	return;
	if (lpObject == lpToObject)
    	return;
	lpBase = GetBase();
	if (lpObject == lpBase || lpToObject == lpBase)
    	return;
	ObjUnlinkObject(&ObjList, (LPPRIMOBJECT)lpObject);
	ObjLinkObject(&ObjList, (LPPRIMOBJECT)lpToObject,
            	(LPPRIMOBJECT)lpObject, fAfter);
	SetupMiniViews(NULL, NO);
}


