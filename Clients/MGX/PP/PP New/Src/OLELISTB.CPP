// olelistb.cpp : implementation file
//

#include "pp.h"
#include "id.h"
#include "ppafx.h"
#include "dataitem.h"
#include "svritem.h"
#include "olelistb.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COlelistBox

COlelistBox::COlelistBox()
{
}

COlelistBox::~COlelistBox()
{
}

BEGIN_MESSAGE_MAP(COlelistBox, CListBox)
	//{{AFX_MSG_MAP(COlelistBox)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COlelistBox message handlers


void COlelistBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	FNAME       *lpResult;
    CDataItem   *pDataItem;
    LPIMAGE      lpImage = GetActiveImage();
	CServerDoc  *pDoc = PictPubApp.GetDocument(lpImage);
	int          num;
    CRect        r,rr;
    BOOL         bCreateDoc = (pDoc == NULL);
    CPoint       p;

	CListBox::OnLButtonDown(nFlags, point);
    FORWARD_WM_LBUTTONUP(GetSafeHwnd(), point.x, point.y, nFlags, ::SendMessage);

	lpResult = ExtBsr_GetSelection(GetParent()->GetSafeHwnd(), &num);

    if (!lpResult || num != 1)
    	return;
	lstrcpy(Names.Clipboard, lpResult[0]);
	if (LookupExtFileN(Names.Clipboard, Names.PasteImageFile, 
		IDN_CLIPBOARD, NO))
	{
		if (bCreateDoc)
		{
			LPFRAME lpFrame = FrameOpen(FDT_GRAYSCALE, 1, 1, 72);
			if (lpFrame)
			{
	            POSITION    Pos;
				lpImage = CreateImage(NULL,lpFrame,NULL,NULL,
						IDN_PP,IDC_SAVECT,IMG_DOCUMENT,NULL);
				if (!lpImage)
				{
					FrameClose(lpFrame);
					return;
				}
				pDoc = (CServerDoc*)PictPubApp.OpenDocumentFile(
					lpImage->CurFile, lpImage, FALSE);
				if (!pDoc)
				{
					DestroyImage(lpImage);
					return;
				}
		 	}
			else
				return;
		}

		if (pDoc)
		{
		    pDataItem = new CDataItem(pDoc, Names.Clipboard, YES);
		    GetParent()->GetWindowRect(&r);
		    ClientToScreen(&point);
		    GetItemRect(GetCurSel(), &rr);
		    ClientToScreen(&rr);
		    p.x = rr.Width() / 2;
		    p.y = rr.Height() / 2;
		
		    // NOTE!!the pDataItem will be internally destroyed once the drag/drop 
		    // operation is completed therefore we do not need to delete the pDataItem
		    pDataItem->DoDragDrop(&rr, p, FALSE, DROPEFFECT_COPY, &r);
		
		    if (bCreateDoc)
				pDoc->OnCloseDocument();
		}
    }
    else if(Names.Clipboard[0])
    {
  	  	// deleted the file...notify the browser
  		UpdatePasteFromFile(NULL);
	} 
}

