/*********************** ( First 3D Development ) ***********************/
/*	(c) 1994/5  John Lundy															     						*/
/************************************************************************/
// fddoc.cpp : implementation of the CFdDoc class
//

#include "stdafx.h"
#include "fd.h"

#include "fddoc.h"
#include "fdview.h"
#include "fdstatus.h"
#include "fdmap.h"
#include "ack\ack3d.h"
#include "ack\ackeng.h"
#include "ack\ackext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

int		DocAction=0;

extern int				ViewLowRes, ViewUpDown;
extern CFdView		*OurView;
extern ACKENG far	*ae;
extern CFdStatus	*fds;
extern CFdMap			*fdm;

extern CTime			StartTime;
extern CTimeSpan	ElapsedTime;

extern unsigned long	NumberOfPaints;

/////////////////////////////////////////////////////////////////////////////
// CFdDoc

IMPLEMENT_DYNCREATE(CFdDoc, CDocument)

BEGIN_MESSAGE_MAP(CFdDoc, CDocument)
	//{{AFX_MSG_MAP(CFdDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFdDoc construction/destruction

CFdDoc::CFdDoc()
{
	// TODO: add one-time construction code here
}

CFdDoc::~CFdDoc()
{
}

BOOL CFdDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	OurView->AppAckInitialize();
	SetModifiedFlag(TRUE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CFdDoc serialization

void CFdDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		ElapsedTime = CTime::GetCurrentTime() - StartTime;
		ae->ElapsedTime += ElapsedTime.GetTotalSeconds();
		ae->NumberOfPaints += NumberOfPaints;
		StartTime = CTime::GetCurrentTime();
		NumberOfPaints = 0L;
		ar.Write((void far *)ae, sizeof(ACKENG));
		TRACE("...Game Saved...\n");
		if (fds) fds->TextMsg("Game Saved");
		DocAction = -1;
	}
	else
	{
		// TODO: add loading code here
HGLOBAL		haet;
ACKENG		*aet;
		haet = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(ACKENG));
		if (haet == NULL)
		{
			TRACE("GlobalAlloc in CFdDoc of temporary ACKENG struct failed\n");
			return;
		}
		aet = (ACKENG far *)GlobalLock(haet);
		if (ar.GetFile()->GetLength() == sizeof(ACKENG))
		{
			ar.Read((void far *)aet, sizeof(ACKENG));
			if (aet->Version == AE_VERSION && lstrcmp(aet->Descriptor,AE_DESCRIPTOR) == 0)
			{
				memmove((void far *)ae, (void far *)aet, sizeof(ACKENG));
				//	Initialize weapons ammo pointers
				ae->WeaponsCarried[0].Ammo = &ae->Fists;
				ae->WeaponsCarried[1].Ammo = &ae->Bullets;
				ae->WeaponsCarried[2].Ammo = &ae->Shells;
				ae->WeaponsCarried[3].Ammo = &ae->Bullets;
				ae->WeaponsCarried[4].Ammo = &ae->Rockets;
				if (!ae->ObjList[PLAYER_OBJECT].Dead)
						ViewUpDown = 32 << (8 - ViewLowRes);
				TRACE("...Game Loaded...\n");
				if (fds)
				{
					fds->TextHealth();
					fds->TextAmmo();
					fds->TextMsg("Game Loaded");
					fds->DrawKeys();
				}
				StartTime = CTime::GetCurrentTime();
				NumberOfPaints = 0L;
				OurView->GetParentFrame()->SetWindowText("WinAdv");
				if (fdm) fdm->InvalidateRect(NULL,FALSE);
				DocAction = 1;
			}
			else
			{
				AfxMessageBox("Saved file format does not match\n");
				TRACE("Saved file format does not match\n");
			}
		}
		else
		{
			AfxMessageBox("Saved file size is not compatible\n");
			TRACE("Saved file size is not compatible\n");
		}
		GlobalUnlock(haet);
		GlobalFree(haet);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFdDoc diagnostics

#ifdef _DEBUG
void CFdDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFdDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFdDoc commands

BOOL CFdDoc::OnOpenDocument(const char* pszPathName)
{
BOOL	stat;
	stat = CDocument::OnOpenDocument(pszPathName);
	if (stat) SetModifiedFlag(TRUE);
	return stat;
}

BOOL CFdDoc::OnSaveDocument(const char* pszPathName)
{
BOOL	stat;
	stat = CDocument::OnSaveDocument(pszPathName);
	if (stat) SetModifiedFlag(TRUE);
	return stat;
}
