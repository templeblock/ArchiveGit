// agcvtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcvtDlg.h"
#include "pagedefs.h"
#include "project.h"
#include "paper.h"
#include "pmwtempl.h"
#include "Pmwdoc.h"
#include "pmwview.h"
#include "bmp.h"
#include "clip.h"
#include "Carddoc.h"
#include "grafobj.h"
#include "pmgfont.h"
#include "utils.h"
#include "rectobj.h"
#include "labels.h"
#include "lbldoc.h"
#include "agcnvtdefs.h"
#include "labeldlg.h"
#include "textrec.h"

static CFile sErrorLog("c:\\AgcnvtErrorLog.txt", CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate);

extern Clipboard near clipboard;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//sdDPI is a guesstimate of the resoulution of the metafile


CMyMetaFile::CMyMetaFile()
{
}

CMyMetaFile::~CMyMetaFile()
{
	POSITION pos = m_RotatedTextInfoList.GetHeadPosition();
	while(pos)
	{
		delete m_RotatedTextInfoList.GetNext(pos);
	}
	
	pos = m_TextInfoList.GetHeadPosition();
	while(pos)
	{
		delete m_TextInfoList.GetNext(pos);
	}
	//clean up
	for(int i = 0; i < m_BmpDataArray.GetSize(); i++)
	{
		delete m_BmpDataArray[i].pBmpInfo;
		delete m_BmpDataArray[i].pBits;
	}
	for(i = 0; i < m_PolyDataArray.GetSize(); i++)
	{
		delete m_PolyDataArray[i].pDC;
	}
}

void CMyMetaFile::ProcessMetafile(CDC* pDC, BOOL fMask /*=FALSE*/, LPARAM lParam /*=0*/)
{
	//make first pass through metafile to collect data
	m_bGetData = TRUE;
	m_bGetTextInfo = TRUE;
	Play(pDC, fMask, lParam);

	m_bGetTextInfo = FALSE;
	//bail if error occured
	if(m_bBail)
		return;
	//set to bogus value
	m_nCurBmpIndex = -1;

	//there are no bitmaps so we need to run through and get the information again.  Normally 
	//anything appearing after a bitmap is valid but since there are no bitmaps we have to assume
	//everything is valid.
	if(m_BmpDataArray.GetSize() == 0 && m_PolyDataArray.GetSize() == 0 )
	{
		if( !m_bProjectCreated )
		{
			if(!CreateNewProject(""))
			{
				CString strWarning;
				strWarning.Format("%s project creation failed.%c%c",m_pszFileName, 13, 10);
				sErrorLog.Write(strWarning, strWarning.GetLength());
				m_bBail = TRUE;
				return;
			}
		}
		Play(pDC, fMask, lParam);
	}
	//set variables and make next pass which actually creates the objects based on the data obtained from
	//the first pass.
	m_bGetData = FALSE;
	m_nCurPolyIndex = 0;

	Play(pDC, fMask, lParam);
	//if for any reason the metafiles didn't get added to the project add them now.
	for(int i = 0; i < m_PolyDataArray.GetSize(); i++)
	{
		if(m_PolyDataArray[i].pDC->m_hDC != NULL)
			AddMetafile(&m_PolyDataArray[i]);
	}

	AddRotatedText();
	AddText();


	//setup the save path
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( m_pszFileName, drive, dir, fname, ext );
	CString strDir(dir);
	strDir = strDir.Left(strDir.ReverseFind('\\') );
	strDir = strDir.Left(strDir.ReverseFind('\\') + 1);
	strDir += "PMW";
	_makepath( path_buffer, drive, strDir, fname, m_strSaveExt );
	
	CString strDirPath = drive + strDir;
	CreateDirectory(strDirPath, NULL);
	//if we have a document save and close the project
	if(m_pDoc)
	{
		m_pDoc->SaveToFile(path_buffer, TRUE, TRUE );
		m_pDoc->OnCloseDocument();
	}
	else
	{
		AfxMessageBox("Null Document");
	}


}
//take the input rect in the metafiles cord and compute the output rect in printmaster panel coords
PBOX CMyMetaFile::ComputeOutputRect(CRect rcInput, int nPS4Panel)
{
	PBOX pbResult;
	CSize szPmwPanel;

	PBOX rcPanelWorld;
	//offset to the panel world
	m_pDoc->get_panel_world(&rcPanelWorld);
	//these variable are for fudge factors.  for example for a 1/2 horiz fold Card we need to add a 
	//quarter inch because of a differnce in how we handle panels.  we also don't distinguish between\
	//inside front and inside back panels but the ds4 file does.  we also have to handle where the Pmw
	//panel is in world coords.

	int nAddX = rcPanelWorld.x0;
	int nAddY = rcPanelWorld.y0;

	switch( m_pInfo->type )
	{
		case PROJECT_TYPE_HalfCard:
		{
			if(m_pInfo->subtype == CARD_SideFold)
			{
				szPmwPanel.cx = 5*1800;
				szPmwPanel.cy = 8*1800;
				if(nPS4Panel == 0)
					nAddX += (int)(.25*1800);
				if(nPS4Panel == 2)
					nAddX += (int)(5.25*1800);
			}
			else
			{
				szPmwPanel.cx = 8*1800;
				szPmwPanel.cy = 5*1800;
				if(nPS4Panel != 1)
					nAddY += (int)(.25*1800);
				if(nPS4Panel == 2)
					nAddY += 5*1800;
			}
			break;		
		}
		case PROJECT_TYPE_Card:
		{
			if(m_pInfo->subtype == CARD_SideFold)
			{
				szPmwPanel.cx = (int)(3.75*1800);
				szPmwPanel.cy = 5*1800;
				if(nPS4Panel == 2)
					nAddX += (int)(4.25*1800);
			}
			else
			{
				szPmwPanel.cx = 5*1800;
				szPmwPanel.cy = (int)(3.75*1800);
				if(nPS4Panel == 2)
					nAddY += (int)(4.25*1800);
			}
			break;		
		}
		case PROJECT_TYPE_Poster:
		{
			if(m_pInfo->orientation == LANDSCAPE)
			{
				szPmwPanel.cx = (int)(10.5*1800);
				szPmwPanel.cy = (int)(8*1800);	
			}
			else
			{
				szPmwPanel.cx = (int)(8*1800);
				szPmwPanel.cy = (int)(10.5*1800);			
			}
			break;
		}
		case PROJECT_TYPE_Envelope:
		{
			szPmwPanel.cx = 9*1800;
			szPmwPanel.cy = (int)(3.875*1800);			
			nAddY += 112;
			nAddX += 112;
			break;
		}
		case PROJECT_TYPE_Label:
		{
			if(m_szWindowExt.cx > 24000 && m_szWindowExt.cy > 24000)
			{
				PPNT pbDims = m_pInfo->m_pPaperInfo->GetProjectDims();
				szPmwPanel.cx = pbDims.x;
				szPmwPanel.cy = pbDims.y;		
			}
			else if(m_szWindowExt.cx > m_szWindowExt.cy)
			{
				if(m_szWindowExt.cx > 24000 && m_szWindowExt.cy > 12000)
				{
					PPNT pbDims = m_pInfo->m_pPaperInfo->GetProjectDims();
					szPmwPanel.cx = pbDims.x;
					szPmwPanel.cy = pbDims.y;		
				}
				else if(m_szWindowExt.cx > 24000 && m_szWindowExt.cy < 9000)
				{
					PPNT pbDims = m_pInfo->m_pPaperInfo->GetProjectDims();
					szPmwPanel.cx = pbDims.x;
					szPmwPanel.cy = pbDims.y;		
				}
			}
			else if(m_szWindowExt.cx < m_szWindowExt.cy)
			{
				PPNT pbDims = m_pInfo->m_pPaperInfo->GetProjectDims();
				szPmwPanel.cx = pbDims.x;
				szPmwPanel.cy = pbDims.y;		
			}
			break;
		}
		case PROJECT_TYPE_PostCard:
		{
			szPmwPanel.cx = (int)(5.75*1800);
			szPmwPanel.cy = (int)(3.75*1800);			
			break;
		}
	}
	//the panels do not come in at a 0 origin so we need to offset the input rect to make
	//it relative to a 0 origin
	CRect rcPS4Panel = m_PanelArray[nPS4Panel];
	rcInput.OffsetRect(-rcPS4Panel.left, -rcPS4Panel.top);


	//calculate the percentage of the input panel compared to the pmw panel
	//we need this to convert the input rect to pmw coords.
	double nXOffset = (double)szPmwPanel.cx/rcPS4Panel.Width();
	double nYOffset = (double)szPmwPanel.cy/rcPS4Panel.Height();

	pbResult.x0 = (long)(rcInput.left * nXOffset + nAddX);
	pbResult.y0 = (long)(rcInput.top * nYOffset + nAddY);
	pbResult.x1 = (long)((rcInput.left + rcInput.Width()) * nXOffset + nAddX);
	pbResult.y1 = (long)((rcInput.top + rcInput.Height()) * nYOffset + nAddY);

	return pbResult;
}

//Return the .ds4 panel based on a point in metafile coords
int CMyMetaFile::GetPanel( CPoint ptLoc )
{
	BYTE nPS4Panel = 0;
	//loop the panel rects looking for one that the point is in
	for(nPS4Panel = 0; nPS4Panel < m_PanelArray.GetSize(); nPS4Panel++)
	{
		if( m_PanelArray[nPS4Panel].PtInRect(ptLoc) )
		{
			break;
		}
	}
	//if one isn't found assume it is the first panel
	if(nPS4Panel >= m_PanelArray.GetSize())
	{
		nPS4Panel = 0;
	}
	return nPS4Panel;
}
//sets the printmaster panel from the specified point and returns the .ds4 panel
int CMyMetaFile::SetPanel( CPoint ptLoc )
{
	short nPmwPanel = 0;
	BYTE nPS4Panel = GetPanel(ptLoc);
	nPmwPanel = nPS4Panel;
	switch( m_pInfo->type )
	{
		case PROJECT_TYPE_HalfCard:
		case PROJECT_TYPE_Card:
		case PROJECT_TYPE_PostCard:
		{
			//need to subtract one because both the inside front and inside back is the same panel in printmaster
			if(nPmwPanel > 1)
				nPmwPanel--;

			if(m_pInfo->type !=  PROJECT_TYPE_PostCard )
				((CCardDoc*)m_pDoc)->set_panel(nPmwPanel);
			else
				m_pDoc->GotoPage(nPmwPanel);
		}
	}
	return nPS4Panel;
}
//attempt to create the project based on the metafiles window ext
BOOL CMyMetaFile::SetupProjectFromWindowExt()
{
	//square or circle stickers
	if(m_szWindowExt.cx > 24000 && m_szWindowExt.cy > 24000 )
	{
		m_pInfo->pTemplate = GET_PMWAPP()->m_pLabelTemplate;
		m_pInfo->type = PROJECT_TYPE_Label;
		m_pInfo->orientation = PORTRAIT;
		m_PanelArray.Add(rcSquareSticker);
		
		//get the paper info
		CLabelList list;
		list.ReadList("LABELS.DAT");
		CPaperInfo* pPaperInfo = (CPaperInfo*)list.Label(13);
		*(m_pInfo->m_pPaperInfo) = *pPaperInfo;
		sdDPI = 10099;		
		m_strSaveExt = ".lbl";
		CString strWarning;
		strWarning.Format("%s is a label project.%c%c",m_pszFileName, 13, 10);
		sErrorLog.Write(strWarning, strWarning.GetLength());
	}
	else if(m_szWindowExt.cx < m_szWindowExt.cy )
	{
		if(m_szWindowExt.cx > 15000)
		{
			m_pInfo->pTemplate = GET_PMWAPP()->m_pPosterTemplate;
			m_pInfo->type = PROJECT_TYPE_Poster;
			m_pInfo->orientation = PORTRAIT;
			m_PanelArray.Add(rcPosterV);
			sdDPI = 2267;
			m_strSaveExt = ".sig";
		}
		else
		{
			//veritacal sticker projects are not supported in printmaster
			CString strWarning;
			strWarning.Format("%s is a vertical label project and is unsupported.%c%c",m_pszFileName, 13, 10);
			sErrorLog.Write(strWarning, strWarning.GetLength());
			return FALSE;
		}
	}
	else
	{
		if(m_szWindowExt.cy > 16000)
		{
			m_pInfo->pTemplate = GET_PMWAPP()->m_pPosterTemplate;
			m_pInfo->type = PROJECT_TYPE_Poster;
			m_pInfo->orientation = LANDSCAPE;
			m_PanelArray.Add(rcPosterH);
			m_strSaveExt = ".sig";
			sdDPI = 2267;
		}
		else if( m_szWindowExt.cx > 20000 &&  m_szWindowExt.cy < 9000 )
		{
			m_pInfo->pTemplate = GET_PMWAPP()->m_pLabelTemplate;
			m_pInfo->type = PROJECT_TYPE_Label;
			m_pInfo->orientation = PORTRAIT;
			CRect rcLabel(0,0,m_szWindowExt.cx, m_szWindowExt.cy);
			m_PanelArray.Add(rcLabel);
			//bring up dialog and let user choose label type
            CLabelTypeDialog dlg(m_pInfo->m_pPaperInfo, NULL, WIZBUT_BACK | WIZBUT_CANCEL | WIZBUT_FINISH, BRAND_NEW_COLOR);
			dlg.DoModal();

			CPaperInfo* pPaperInfo = (CPaperInfo*)dlg.GetChosenLabel();
			*(m_pInfo->m_pPaperInfo) = *pPaperInfo;
			PBOX pbMargins = pPaperInfo->GetPageMargins();
			PPNT pbDims = pPaperInfo->GetProjectDims();
			sdDPI = rcLabel.Width() * 1800 /  (pbDims.x + pbMargins.x0 + pbMargins.x1);

			CString strWarning;
			strWarning.Format("%s is a label project.%c%c",m_pszFileName, 13, 10);
			sErrorLog.Write(strWarning, strWarning.GetLength());
			m_strSaveExt = ".lbl";
		}
		else if( m_szWindowExt.cx > 20000 &&  m_szWindowExt.cy < 12000 )
		{
			m_pInfo->pTemplate = GET_PMWAPP()->m_pEnvelopeTemplate;
			m_pInfo->type = PROJECT_TYPE_Envelope;
			m_pInfo->orientation = LANDSCAPE;
			CRect rcEnvelope(0,0, m_szWindowExt.cx, m_szWindowExt.cy);
			m_PanelArray.Add(rcEnvelope);

			CLabelList list;
			list.ReadList("ENVELOPE.DAT");
			CPaperInfo* pPaperInfo = (CPaperInfo*)list.Label(4);
			*(m_pInfo->m_pPaperInfo) = *pPaperInfo;
			sdDPI = (double)m_szWindowExt.cy / 3.6;
			m_strSaveExt = ".env";
		}
		else if( m_szWindowExt.cx > 24000 && m_szWindowExt.cy > 12000 && m_szWindowExt.cy < 14000)
		{
			m_pInfo->pTemplate = GET_PMWAPP()->m_pLabelTemplate;
			m_pInfo->type = PROJECT_TYPE_Label;
			m_pInfo->orientation = PORTRAIT;
			m_PanelArray.Add(rcRectStickerH);

			CLabelList list;
			list.ReadList("LABELS.DAT");
			CPaperInfo* pPaperInfo = (CPaperInfo*)list.Label(12);
			*(m_pInfo->m_pPaperInfo) = *pPaperInfo;
			sdDPI = 10615;		
			CString strWarning;
			strWarning.Format("%s is a label project.%c%c",m_pszFileName, 13, 10);
			sErrorLog.Write(strWarning, strWarning.GetLength());
			m_strSaveExt = ".lbl";
		}
		else 
			return FALSE;
	}
	return TRUE;

}

//creates a new project and adds the rect panels to the panel array
//returns false if it can't find a project
BOOL CMyMetaFile::CreateNewProject(CString strText)
{
	CString strType;
   //setup the document information
	if(strText.CompareNoCase("French Fold Vertical 1 Format Template") == 0)
	{
		m_pInfo->pTemplate = GET_PMWAPP()->m_pCardTemplate;
		m_pInfo->type = PROJECT_TYPE_Card;
		m_pInfo->subtype = CARD_SideFold;   
		m_pInfo->orientation = PORTRAIT;
		m_PanelArray.Add(rcQuarterVFFront);
		m_PanelArray.Add(rcQuarterVFIFront);
		m_PanelArray.Add(rcQuarterVFIBack);
		m_PanelArray.Add(rcQuarterVFBack);
		m_strSaveExt = ".car";
		sdDPI = 1136;
	}
	else if(strText.CompareNoCase("French Fold Horizontal 1 Format Template") == 0)
	{
		m_pInfo->pTemplate = GET_PMWAPP()->m_pCardTemplate;
		m_pInfo->type = PROJECT_TYPE_Card;
		m_pInfo->subtype = CARD_TopFold;   
		m_pInfo->orientation = LANDSCAPE;
		m_PanelArray.Add(rcQuarterHFFront);
		m_PanelArray.Add(rcQuarterHFIFront);
		m_PanelArray.Add(rcQuarterHFIBack);
		m_PanelArray.Add(rcQuarterHFBack);
		m_strSaveExt = ".car";
		sdDPI = 1470;
	}
	else if(strText.CompareNoCase("Half Fold Horizontal 1 Format Template") == 0)
	{
		m_pInfo->pTemplate = GET_PMWAPP()->m_pHalfCardTemplate;
		m_pInfo->type = PROJECT_TYPE_HalfCard;
		m_pInfo->subtype = CARD_TopFold;         // As a default.
		m_pInfo->orientation = PORTRAIT;
		m_PanelArray.Add(rcHalfHFFront);
		m_PanelArray.Add(rcHalfHFIFront);
		m_PanelArray.Add(rcHalfHFIBack);
		m_PanelArray.Add(rcHalfHFBack);
		m_strSaveExt = ".hcr";
		sdDPI = 1250;
	}
	else if(strText.CompareNoCase("Half Fold Vertical 1 Format Template") == 0)
	{
		m_pInfo->pTemplate = GET_PMWAPP()->m_pHalfCardTemplate;
		m_pInfo->type = PROJECT_TYPE_HalfCard;
		m_pInfo->subtype = CARD_SideFold;         // As a default.
		m_pInfo->orientation = LANDSCAPE;
		m_PanelArray.Add(rcHalfVFFront);
		m_PanelArray.Add(rcHalfVFIFront);
		m_PanelArray.Add(rcHalfVFIBack);
		m_PanelArray.Add(rcHalfVFBack);
		m_strSaveExt = ".hcr";
		sdDPI = 862;
	}
	else if(strText.CompareNoCase("Post Card Horizontal Format Template") == 0)
	{
		m_pInfo->pTemplate = GET_PMWAPP()->m_pPostCardTemplate;
		m_pInfo->type = PROJECT_TYPE_PostCard;
		m_pInfo->orientation = PORTRAIT;
		m_PanelArray.Add(rcPostCardHF);
		m_PanelArray.Add(rcPostCardHB);

		CLabelList list;
		list.ReadList("POSTCARD.DAT");
		CPaperInfo* pPaperInfo = (CPaperInfo*)list.Label(0);
		*(m_pInfo->m_pPaperInfo) = *pPaperInfo;
		m_strSaveExt = ".pcr";

		sdDPI = 1470;
	}
	else if(strText.CompareNoCase("Post Card Vertical Format Template") == 0)
	{
		CString strWarning;
		strWarning.Format("%s is Postcard vertical project.  Unsupported.%c%c",m_pszFileName, 13, 10);
		sErrorLog.Write(strWarning, strWarning.GetLength());

		return FALSE;

		m_pInfo->pTemplate = GET_PMWAPP()->m_pPostCardTemplate;
		m_pInfo->type = PROJECT_TYPE_PostCard;
		m_pInfo->orientation = PORTRAIT;
		m_PanelArray.Add(rcPostCardVF);
		m_PanelArray.Add(rcPostCardVB);

		CLabelList list;
		list.ReadList("POSTCARD.DAT");
		CPaperInfo* pPaperInfo = (CPaperInfo*)list.Label(0);
		*(m_pInfo->m_pPaperInfo) = *pPaperInfo;
		m_strSaveExt = ".pcr";
		
		sdDPI = 1470;
	}
	else if(!SetupProjectFromWindowExt())
	{
		CString strWarning;
		strWarning.Format("%s project doesn't exist.%c%c",m_pszFileName, 13, 10);
		sErrorLog.Write(strWarning, strWarning.GetLength());
		return FALSE;
	}
	//setup the type and the paper info
	m_pInfo->type = ((CPmwDocTemplate*)m_pInfo->pTemplate)->ProjectType();

	if( m_pInfo->type != PROJECT_TYPE_Envelope && m_pInfo->type != PROJECT_TYPE_Label && 
		m_pInfo->type != PROJECT_TYPE_PostCard )
	{
		*(m_pInfo->m_pPaperInfo) = *(GET_PMWAPP()->GetPaperInfo(m_pInfo->orientation));
	}

	//set the doc info in the app
	GET_PMWAPP()->last_new_info = *m_pInfo;
	m_pDoc = (CPmwDoc*)m_pInfo->pTemplate->OpenDocumentFile(NULL);

	BOOL bResult = (m_pDoc != NULL);
	m_bProjectCreated = TRUE;

	return bResult;
}

//enumerate the metafile records
BOOL CMyMetaFile::EnumMetaFileProc(HANDLETABLE FAR* pObject, METARECORD FAR* pMetaRecord, int nObject, EnumData* pData)
{
	//init the brush and pen arrays.  make them the size of the number of objects. THen we can use the
	//index to make sure we get the righ brush from the select object record
	if(m_brushArray.GetSize() == 0)
	{
		for(int i = 0; i < nObject; i++)
		{
			LOGBRUSH lgBr;
			lgBr.lbColor = (COLORREF)-1;//bogus value
			m_brushArray.Add(lgBr);
			LOGPEN lgPen;
			lgPen.lopnColor = (COLORREF)-1;//bogus value
			m_penArray.Add(lgPen);
		}
	}
	if(m_bBail)
		return FALSE;

	switch (pMetaRecord->rdFunction)
	{
		case META_SETWINDOWEXT:
		{
			//save the window ext
			recordMetaSETWINDOWEXT *pRecord = (recordMetaSETWINDOWEXT*)pMetaRecord;
			m_szWindowExt.cx = pRecord->nX;
			m_szWindowExt.cy = pRecord->nY;

			if(!m_bGetData )
			{
				//set the window ext for the polygons
				for(int i = 0; i < m_PolyDataArray.GetSize(); i++)
				{
					CSize szGraphic = m_PolyDataArray[i].ptExt - m_PolyDataArray[i].ptOrigin;
					m_PolyDataArray[i].pDC->SetWindowExt(szGraphic);
				}
			}
			break;
		}
		case META_SELECTOBJECT:
		{
			if(!m_bGetData)
			{
				int nCurObj = pMetaRecord->rdParm[0];
				//was a pen or brush created right before this.  If so then we are selecting a brush or pen
				//into the dc so we want to add it to the brush or pen array
				if(m_bPenCreated)
				{
					m_penArray[nCurObj] = m_lgPen;
				}
				else if(m_bBrushCreated)
				{
					m_brushArray[nCurObj] = m_lgBrush;
				}
				m_bPenCreated = FALSE;
				m_bBrushCreated = FALSE;
				//we are selecting an object that may or may not be a valid brush or pen.  the only way to 
				//know is to see if the brush and pen arrays have a valid entry for that index.  this 
				//is needed because there is no gaurentee that the object being selected was created
				//directly before this.
				if(m_penArray[nCurObj].lopnColor != -1)
				{
					CPen pen;
					pen.CreatePenIndirect(&(m_penArray[nCurObj]));
						
					if(m_nCurPolyIndex < m_PolyDataArray.GetSize())
					{
						if(m_PolyDataArray[m_nCurPolyIndex].pDC->m_hDC != NULL)
						{
							CPen* pOldPen = m_PolyDataArray[m_nCurPolyIndex].pDC->SelectObject(&pen);
							pen.Detach();//detach the object
							if(pOldPen)
								pOldPen->DeleteObject();
						}
					}
					//store the index of the currently selected pen
					m_nCurPenObjSel = nCurObj;
				}
				else if(m_brushArray[nCurObj].lbColor != -1)
				{
					CBrush brush;
					brush.CreateBrushIndirect(&(m_brushArray[nCurObj]));
					if(m_nCurPolyIndex < m_PolyDataArray.GetSize())
					{
						if(m_PolyDataArray[m_nCurPolyIndex].pDC->m_hDC != NULL)
						{
							CBrush* pOldBrush = m_PolyDataArray[m_nCurPolyIndex].pDC->SelectObject(&brush);
							brush.Detach();
							if(pOldBrush)
								pOldBrush->DeleteObject();
						}
					}
					//store the index of the currently selected pen
					m_nCurBrushObjSel = nCurObj;
				}
			}
			break;
		}
		case META_DELETEOBJECT:
		{
			//jsut clear the entries in both the pen and brush arrays.
			int nCurObj = pMetaRecord->rdParm[0];
			m_penArray[nCurObj].lopnColor = -1;
			m_brushArray[nCurObj].lbColor = -1;
			break;
		}
		case META_SETTEXTCOLOR:
		{
			SetTextColorRecord(pMetaRecord);
			break;
		}
		case META_SETTEXTALIGN:
		{
			SetTextAlignRecord(pMetaRecord);
			break;
		}
		case META_CREATEFONTINDIRECT:
		{
				CreateFontRecord(pMetaRecord);
			break;
		}
		case META_TEXTOUT:
		{
			TextOutRecord(pMetaRecord);
			break;
		}
		case META_SAVEDC:
		{
			if(!m_bGetData )
			{
				for(int i = 0; i < m_PolyDataArray.GetSize(); i++)
					PlayMetaFileRecord(m_PolyDataArray[i].pDC->m_hDC, pObject, pMetaRecord, nObject);
			}
			break;
		}
		case META_SETWINDOWORG:
		{
			if(!m_bGetData )
			{
				for(int i = 0; i < m_PolyDataArray.GetSize(); i++)
				{
					CSize szGraphic = m_PolyDataArray[i].ptExt - m_PolyDataArray[i].ptOrigin;
					m_PolyDataArray[i].pDC->SetWindowOrg(m_PolyDataArray[i].ptOrigin);
				}
			}
			break;
		}
		case META_RESTOREDC:
		{
			if(!m_bGetData )
			{
				for(int i = 0; i < m_PolyDataArray.GetSize(); i++)
				{
					PlayMetaFileRecord(m_PolyDataArray[i].pDC->m_hDC, pObject, pMetaRecord, nObject);
				}	
			}
			break;
		}
		case META_CREATEPENINDIRECT:
		{
			if(!m_bGetData )
				CreatePenRecord(pMetaRecord);
			break;
		}
		case META_CREATEBRUSHINDIRECT:
		{
			if(!m_bGetData )
				CreateBrushRecord(pMetaRecord);
			break;
		}
		case META_POLYGON:
		{
			PolyRecord(pObject, pMetaRecord, nObject);
			break;
		}
		case META_POLYPOLYGON:
		{
			PolyRecord(pObject, pMetaRecord, nObject, TRUE);
			break;
		}
		case META_POLYLINE:
		{
			PolyRecord(pObject, pMetaRecord, nObject);
			break;
		}
		case META_SETPOLYFILLMODE:
		{
			if(!m_bGetData )
			{
				int nIndex = GetPolyIndex(pMetaRecord);
				/*for(int i = 0; i < m_PolyDataArray.GetSize(); i++)//*/if(nIndex != -1 )
				{
					ASSERT(nIndex < m_PolyDataArray.GetSize());
					PlayMetaFileRecord(m_PolyDataArray[nIndex].pDC->m_hDC, pObject, pMetaRecord, nObject);
				}
			}

			break;
		}
		case META_STRETCHDIB:
		{
			StretchDIBRecord(pMetaRecord);
			break;
		}
	}

	return TRUE;
}

//creates and adds the metafile to the project
void CMyMetaFile::AddMetafile(PolyData* pPolyData)
{
	//setup a METAFILEPICT struct
	METAFILEPICT metaPict;
	metaPict.mm = MM_ANISOTROPIC;
	metaPict.xExt = pPolyData->ptExt.x - pPolyData->ptOrigin.x;
	metaPict.yExt = pPolyData->ptExt.y - pPolyData->ptOrigin.y;
	metaPict.hMF = pPolyData->pDC->Close();

	//we have all the data for the bmp let's create a graphic object and add it to the doc
	GraphicObject* pObject;
	//create a unique name
	char cbName[30];
	clipboard.SetUpdateTime();
	clipboard.BuildPasteName(".WMF", cbName);
	//set the panel in the doc.  This must be done before the object is created
	int nPanel = SetPanel(pPolyData->ptOrigin);

	//create the graphic object
	CreateWMFFromMetaFilePict(cbName, m_pDoc->get_database(), &metaPict, pObject);

	CRect rcPoly(pPolyData->ptOrigin, pPolyData->ptExt - pPolyData->ptOrigin); 
	PBOX rcBounds = ComputeOutputRect(rcPoly, nPanel);
	//set the objects panel
	pObject->set_panel(m_pDoc->get_current_panel());
	//set the bounds 
	pObject->set_bound(rcBounds);
	pObject->calc();
	m_pDoc->append_object(pObject);
	m_pDoc->add_refresh(&rcBounds);
	//clean up
	DeleteMetaFile(metaPict.hMF);

}

//handles the polygon, polyline and polypolygon records
void CMyMetaFile::PolyRecord(HANDLETABLE* pObject, METARECORD* pMetaRecord, int nObject, BOOL bPolyPoly /* = FALSE*/)
{
	//first pass
	if(m_bGetData)
	{
		GetPolyData(pMetaRecord, bPolyPoly);
		return;
	}
	//make sure the polygon is within the panel
	if( PolyWithinPanel(pMetaRecord, bPolyPoly))
	{
		//check to see if it is a rectangle
		if(	!PolygonToRect(pMetaRecord) )
		{
			//get the right inedex
			int nIndex = GetPolyIndex(pMetaRecord, bPolyPoly);
			if(nIndex != -1 )
			{
				//decrement the count
				m_PolyDataArray[nIndex].nCount--;
				m_nCurPolyIndex = nIndex;
				PlayMetaFileRecord(m_PolyDataArray[nIndex].pDC->m_hDC, pObject, pMetaRecord, nObject);
				//if that was the last polygon for this vector graphic than lets add it to the project
				//this helps retain proper z-depth
				if(m_PolyDataArray[nIndex].nCount == 0)
					AddMetafile(&(m_PolyDataArray[m_nCurPolyIndex]));
			}
		}
	}
}

//returns true if the polygon is withing the bounds of the panel
BOOL CMyMetaFile::PolyWithinPanel(METARECORD* pMetaRecord, BOOL bPolyPoly /*= FALSE*/)
{
	//if there is only one panel than everything is on it.
	if(m_PanelArray.GetSize() == 1)
		return TRUE;

	recordMetaPOLYGON *pRecord = (recordMetaPOLYGON*)pMetaRecord;
	WordPoint* pPoints = NULL;
	//get a pointer to the actual points
	if(bPolyPoly)
	{
		pPoints = (WordPoint*)((BYTE*)pRecord->Point + pRecord->nCount * 2);
	}
	else
	{
		pPoints = pRecord->Point;
	}
	//check to see if the first point is insied the panel.  This is good enough for us
	for(int i = 0; i < m_PanelArray.GetSize(); i++)
	{
		CRect rcPanel = m_PanelArray[i];
		rcPanel.InflateRect(-1,-1);
		if(rcPanel.PtInRect(CPoint(pPoints[0].x, pPoints[0].y)))
			return TRUE;
	}
	return FALSE;
}

//get the info about the polys.  this is only called during the first pass
void CMyMetaFile::GetPolyData(METARECORD* pMetaRecord, BOOL bPolyPoly/* = FALSE*/)
{
	//if the poly isn't within the panel who cares
	if(!PolyWithinPanel(pMetaRecord, bPolyPoly))
		return;
	
	recordMetaPOLYGON *pRecord = (recordMetaPOLYGON*)pMetaRecord;
	WordPoint* pPoints = NULL;
	int nCount = 0;
	int nPolyCount = 1;
	WORD* pPointCount = NULL;
	//get a pointer to the points and a count
	if(bPolyPoly)
	{
		pPoints = (WordPoint*)((BYTE*)pRecord->Point + pRecord->nCount * 2);
		nPolyCount = pRecord->nCount;
		pPointCount = (WORD*)((BYTE*)pRecord->Point);
	}
	else
	{
		pPoints = pRecord->Point;
		nCount = pRecord->nCount;
	}
	


	//if it is a rectangle we don't want it as a vector graphic.  we want it as a rectangle object
	if(nCount == 5 && pPoints[0].y == pPoints[1].y && 
		pPoints[1].x == pPoints[2].x && pPoints[2].y == pPoints[3].y)
	{
		return;
	}
	//do we need to add another polydata struct to the array
	//we have one metafile per panel
	BOOL bAddNewData = FALSE;
	if(m_PolyDataArray.GetSize() == 0 )
		bAddNewData = TRUE;
	else 
	{
		ASSERT(m_nCurPolyIndex < m_PolyDataArray.GetSize());
		if(m_PanelArray.GetSize() > 1)
		{

			int nPanel = GetPanel(m_PolyDataArray[m_nCurPolyIndex].ptOrigin);
			CRect rcCur = m_PanelArray[nPanel];
			rcCur.InflateRect(-1,-1);
			CPoint ptTemp(pPoints[0].x, pPoints[0].y);
			if(!rcCur.PtInRect( ptTemp) )
				bAddNewData = TRUE;
		}
	}
	//add the struct
	if(bAddNewData)
	{
		PolyData data;
		data.ptOrigin = CPoint(pPoints[0].x,pPoints[0].y);
		data.ptExt = CPoint(pPoints[0].x,pPoints[0].y);
		data.nCount = 0;
		data.pDC = new CMetaFileDC;
		data.pDC->Create(NULL);
		m_PolyDataArray.Add(data);
		m_nCurPolyIndex = m_PolyDataArray.GetSize() - 1;
	}
	//determine the bounds of the vector graphic
	for(int i = 0; i < nPolyCount; i++)
	{
		if(pPointCount != NULL)
			nCount = pPointCount[i];

		for(int j = 0; j < nCount; j++)
		{
			m_PolyDataArray[m_nCurPolyIndex].ptOrigin.x = min(m_PolyDataArray[m_nCurPolyIndex].ptOrigin.x, pPoints[0].x);
			m_PolyDataArray[m_nCurPolyIndex].ptOrigin.y = min(m_PolyDataArray[m_nCurPolyIndex].ptOrigin.y, pPoints[0].y);
			m_PolyDataArray[m_nCurPolyIndex].ptExt.x = max(m_PolyDataArray[m_nCurPolyIndex].ptExt.x ,pPoints[0].x);
			m_PolyDataArray[m_nCurPolyIndex].ptExt.y = max(m_PolyDataArray[m_nCurPolyIndex].ptExt.y, pPoints[0].y);
			pPoints++;
		}
	}
	//increment the poly count
	m_PolyDataArray[m_nCurPolyIndex].nCount++;
}

//return the index into the poly array for this polygon
int CMyMetaFile::GetPolyIndex(METARECORD* pMetaRecord, BOOL bPolyPoly /* = FALSE*/)
{
	WordPoint* pPoints = NULL;
	//if there is only one we know the index is 0
	if(m_PolyDataArray.GetSize() == 1)
		return 0;

	//get a pointer to the points
	if(bPolyPoly)
	{
		recordMetaPOLYGON *pRecord = (recordMetaPOLYGON*)pMetaRecord;
		pPoints = (WordPoint*)((BYTE*)pRecord->Point + pRecord->nCount * 2);	
	}
	else
	{
		recordMetaPOLYGON *pRecord = (recordMetaPOLYGON*)pMetaRecord;
		pPoints = pRecord->Point;
	}
	//check to see if the first point is within the metafiles bounds
	for( int i = 0; i < m_PolyDataArray.GetSize(); i++)
	{
		CRect rcCur(m_PolyDataArray[i].ptOrigin, m_PolyDataArray[i].ptExt - m_PolyDataArray[i].ptOrigin);
		rcCur.InflateRect(1,1);
		if(rcCur.PtInRect(CPoint(pPoints[0].x, pPoints[0].y) ))
			return i;
	
	}
	//polygon is not within bounds
	return -1;
}

//handle the text alignment record
void CMyMetaFile::SetTextAlignRecord(METARECORD* pMetaRecord)
{
	recordMetaTEXTALIGN *pRecord = (recordMetaTEXTALIGN*)pMetaRecord;

	//horizontal alignment is useless to us.  It doesn't match.  We assume that it is always centered
	//unless the text is rotated
	switch(pRecord->nHorizFlags)
	{
		case TA_CENTER:
			m_nHorizTextAlign = ALIGN_center;
			break;
		case TA_LEFT:
			m_nHorizTextAlign = ALIGN_left;
			break;
		case TA_RIGHT:
			m_nHorizTextAlign = ALIGN_right;
			break;
	}
	
	switch(pRecord->nVertFlags)
	{
		case TA_BASELINE:
			m_nVertTextAlign = TA_BASELINE;
			break;
		case TA_BOTTOM:
			m_nVertTextAlign = TA_BOTTOM;
			break;
		case TA_TOP:
			m_nVertTextAlign = TA_TOP;
			break;
	}
	m_nVertTextAlign = pRecord->nVertFlags;
}

//handles the text color record
void CMyMetaFile::SetTextColorRecord(METARECORD* pMetaRecord)
{
	recordMetaFONTCOLOR *pRecord = (recordMetaFONTCOLOR*)pMetaRecord;

	m_crTextColor = pRecord->crFontColor;
}

//handles the create pen record
void CMyMetaFile::CreatePenRecord(METARECORD* pMetaRecord)
{
	recordMetaCREATEPENINDIRECT *pRecord = (recordMetaCREATEPENINDIRECT*)pMetaRecord;
	
	m_nPenWidth = pRecord->nX;
	//store the logpen data
	if(!m_bGetData )
	{
		m_bPenCreated = TRUE;
		m_lgPen.lopnStyle = pRecord->nStyle;
		m_lgPen.lopnWidth = CPoint(pRecord->nX, pRecord->nY);
		m_lgPen.lopnColor = pRecord->crColor;

	}
}

//handles the create brush record
void CMyMetaFile::CreateBrushRecord(METARECORD* pMetaRecord)
{
	recordMetaCREATEBRUSHINDIRECT *pRecord = (recordMetaCREATEBRUSHINDIRECT*)pMetaRecord;
	//store the logbrush data
	if(!m_bGetData  )
	{
		m_bBrushCreated = TRUE;
		m_lgBrush.lbStyle = pRecord->nStyle;
		m_lgBrush.lbHatch = pRecord->nHatch;
		m_lgBrush.lbColor = pRecord->crColor;

	}
}

//checks to see if the polygon  is a rectangle and if it is add it to the project as a rectangle object
BOOL CMyMetaFile::PolygonToRect(METARECORD* pMetaRecord)
{
	recordMetaPOLYGON *pRecord = (recordMetaPOLYGON*)pMetaRecord;

	if(m_bOkToAddPoly || PolyWithinPanel(pMetaRecord))
	{
		if(pRecord->nCount == 5 && pRecord->Point[0].y == pRecord->Point[1].y && 
			pRecord->Point[1].x == pRecord->Point[2].x && pRecord->Point[2].y == pRecord->Point[3].y)
		{
			WordPoint ptTopLeft = pRecord->Point[0];
			WordPoint ptBottomRight = pRecord->Point[2];
			//it is  a rect but it isn't within bounds
			if(abs(ptBottomRight.x - ptTopLeft.x) > m_PanelArray[0].Width() - 100 && 
				abs(ptBottomRight.y - ptTopLeft.y) > m_PanelArray[0].Height() - 100)
			{
				return TRUE;
			}

			//calculate the bounds.
			CRect rcPoly(ptTopLeft.x, ptTopLeft.y, ptBottomRight.x, ptBottomRight.y);
			int nPanel = SetPanel(CPoint(rcPoly.left, rcPoly.top));
			PBOX rcBounds = ComputeOutputRect(rcPoly, nPanel);
			PMGDatabase* pDatabase = m_pDoc->get_database();
			//create a rect object
			RectangleObject* pObject = pDatabase->create_rectangle_object(NULL);

			CObjectProperties properties(pDatabase);
			//set the fill and line color
			COLOR ForeColor = TRANSPARENT_COLOR;
			if(m_brushArray[m_nCurBrushObjSel].lbStyle != BS_HOLLOW && m_brushArray[m_nCurBrushObjSel].lbStyle != BS_NULL)
				ForeColor = COLOR_FROM_COLORREF(m_brushArray[m_nCurBrushObjSel].lbColor);

			properties.SetFillForegroundColor(ForeColor, FALSE);

			COLOR PenColor = TRANSPARENT_COLOR;
			if(m_penArray[m_nCurPenObjSel].lopnStyle != PS_NULL)
				PenColor = COLOR_FROM_COLORREF(m_penArray[m_nCurPenObjSel].lopnColor);

			properties.SetOutlineForegroundColor(PenColor, FALSE);
			properties.SetOutlineStyle(-1, FALSE);
			properties.SetOutlineWidthType(0, FALSE);
			properties.SetOutlineWidth(MakeFixed((double)m_nPenWidth * 72 / sdDPI ), FALSE);
			
			pObject->SetObjectProperties(properties);
			//add to the doc
			pObject->set_bound(rcBounds);
			pObject->calc();
			pObject->set_panel(m_pDoc->get_current_panel());
			m_pDoc->append_object(pObject);
			m_pDoc->add_refresh(&rcBounds);
			
			return TRUE;


		}
	}
	return FALSE;

}

//handles the create font record
void CMyMetaFile::CreateFontRecord(METARECORD* pMetaRecord)
{
	recordMetaCREATEFONTINDIRECT *pRecord = (recordMetaCREATEFONTINDIRECT*)pMetaRecord;

	// store the logfont data

	BYTE* pCopy = (BYTE*)&m_lf;
	memcpy(pCopy + 10, &(pRecord->lfFont), sizeof(LOGFONT) - 10);
	m_lf.lfHeight = pRecord->lfFont.lfHeight;
	m_lf.lfWidth = pRecord->lfFont.lfWidth;
	m_lf.lfEscapement = pRecord->lfFont.lfEscapement;
	m_lf.lfOrientation = pRecord->lfFont.lfOrientation;
	m_lf.lfWeight = pRecord->lfFont.lfWeight;
}

//handles the text out record
void CMyMetaFile::TextOutRecord(METARECORD* pMetaRecord)
{
	recordMetaTEXTOUT *pRecord = (recordMetaTEXTOUT*)pMetaRecord;

	char* pString = new char[pRecord->wCount+1];
	memcpy(pString, &(pRecord->String[0]), pRecord->wCount);
	pString[pRecord->wCount] = '\0';
	//if the string is less than 5 chars starting with a space.
	//this gets rid of unwanted empty text frames
	if(strlen(pString) < 5 && *pString == ' ' )
	{
		delete [] pString;
		return;
	}
	//create the prject.  The first string if valid contains info about which project
	if( !m_bProjectCreated )
	{
		if(!CreateNewProject(pString))
		{
			CString strWarning;
			strWarning.Format("%s project creation failed.%c%c",m_pszFileName, 13, 10);
			sErrorLog.Write(strWarning, strWarning.GetLength());
			m_bBail = TRUE;
			return;
		}
	}
	//get the output location
	WORD* pCoord = (WORD*)pRecord + (pMetaRecord->rdSize - 2 );
	int nY = *pCoord++;
	int nX = *pCoord;

	BOOL bAddString = FALSE;
	for(int i = 0; i < m_PanelArray.GetSize(); i++)
	{
		if(m_PanelArray[i].PtInRect(CPoint(nX, nY)))
			bAddString = TRUE;
	}
	if(bAddString && m_bGetTextInfo)
	{
		CTextInfo* pInfo = new CTextInfo;
		pInfo->m_ptLoc = CPoint(nX, nY);
		pInfo->m_lf = m_lf;
		pInfo->m_strText = pString;
		pInfo->m_crTextColor = m_crTextColor;
		int nPanel = GetPanel(pInfo->m_ptLoc);
		pInfo->m_nPS4Panel = nPanel;
		BOOL bItemAdded = FALSE;

		CTypedPtrList<CObList, CTextInfo*>* pList = &m_TextInfoList;
		if(m_lf.lfEscapement != 0)
			pList = &m_RotatedTextInfoList;

		POSITION pos = pList->GetHeadPosition();
		POSITION lastPos = pos;
		while(pos)
		{
			CTextInfo* pTextInfo = pList->GetNext(pos);
			if(nPanel < pTextInfo->m_nPS4Panel)
			{
				pList->InsertBefore(lastPos, pInfo);
				bItemAdded = TRUE;
				break;
			}
			else if(m_lf.lfEscapement == 0 && nPanel == pTextInfo->m_nPS4Panel)
			{
				if(nY < pTextInfo->m_ptLoc.y)
				{
					pList->InsertBefore(lastPos, pInfo);
					bItemAdded = TRUE;
					break;
				}
			}
			lastPos = pos;
		}

		if(!bItemAdded)
			pList->AddTail(pInfo);

	}
	//cleanuup
	delete [] pString;

}

int CMyMetaFile::GetFontSize(LOGFONT& lf)
{
	//this is a hack to determine the font size.  I was unable to find any other way to determine it.
	//the hack is based on the fact that each font is consitenly off a constant percent.
	char percent[24];

	int nConv = GetPrivateProfileString("Font", lf.lfFaceName, "", percent, 24, strIniPath); 
	int nPercent = atoi(percent);
	if(!nConv)
	{
		CString strWarning;
		strWarning.Format("%s missing font entry in .ini file  Substituting CAC Futura Casual.%c%c",m_pszFileName, 13, 10);
		sErrorLog.Write(strWarning, strWarning.GetLength());
		strcpy(lf.lfFaceName, "CAC Futura Casual" );
		nPercent = -12;
	}

	int nRes = (int)(sdDPI * ((double)nPercent / 100) + sdDPI);

	int nSize = (int)((double)-lf.lfHeight * 72 / nRes + .5 );
	
	return nSize;
}

void CMyMetaFile::AddRotatedText()
{
	PMGDatabase* pDatabase = m_pDoc->get_database();
	CTxp Txp(pDatabase);
	int nParaNum = 0;
	int nRotation = 0;

	CString strWarning;
	strWarning.Format("%s has rotated text.%c%c",m_pszFileName, 13, 10);
	sErrorLog.Write(strWarning, strWarning.GetLength());

	POSITION pos = m_RotatedTextInfoList.GetHeadPosition();
	while(pos)
	{
		CTextInfo* pTextInfo = m_RotatedTextInfoList.GetNext(pos);

		//set the panel
		int nPS4Panel = SetPanel(pTextInfo->m_ptLoc);
		int nPanel = m_pDoc->get_current_panel();
		
		PMGFontServer* pFontServer = (PMGFontServer*)m_pDoc->get_font_server();

		if(strcmp(pTextInfo->m_lf.lfFaceName, "Arial") == 0)
		{
			CString strWarning;
			strWarning.Format("%s has imported arial text.%c%c",m_pszFileName, 13, 10);
			sErrorLog.Write(strWarning, strWarning.GetLength());
			strcpy(pTextInfo->m_lf.lfFaceName, m_strDefaultFontName );
		}

		//create the font
		CTextStyle Style(pDatabase);
		Style.SetDefault();
		int nFace = typeface_server.find_face(pTextInfo->m_lf.lfFaceName, FALSE);
		if (nFace != -1)
		{
			DB_RECORD_NUMBER lFace = pFontServer->font_face_to_record(nFace);
			if (lFace != 0)
			{
				Style.Font(lFace);
				pDatabase->free_font_record(lFace, TRUE);
			}
		}

		int nSize = GetFontSize(pTextInfo->m_lf);

		//determine text bounds.
		CDC* pDC = AfxGetMainWnd()->GetDC();
		int nOldMode = pDC->SetMapMode(MM_LOENGLISH);
		CFont font;
		LOGFONT lf;
		memcpy(&lf, &(pTextInfo->m_lf), sizeof(LOGFONT));
		lf.lfHeight = nSize * 10;
		font.CreatePointFontIndirect(&lf, pDC );
		CFont* pOldFont = pDC->SelectObject(&font);
		CSize szText = pDC->GetTextExtent(pTextInfo->m_strText);

		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);

		pDC->SelectObject(pOldFont);
		pDC->SetMapMode(nOldMode);
		AfxGetMainWnd()->ReleaseDC(pDC);
		CRect rcIn(pTextInfo->m_ptLoc, pTextInfo->m_ptLoc);
		PBOX rcBounds = ComputeOutputRect(rcIn, nPS4Panel );
		
		//convert the size of the text to our resolution. and fudge it a little.  This is also a necessary 
		//hack
		rcBounds.x0 -= 75;
		rcBounds.x1 = rcBounds.x0 + szText.cx * 18 + 150;
		rcBounds.y1 = rcBounds.y0 + szText.cy * 18;
		int nYOffset;
		if(m_nVertTextAlign == TA_BASELINE)
		{
			nYOffset = tm.tmAscent * 18;
			rcBounds.y0 -= nYOffset;
			rcBounds.y1 -= nYOffset;
		}

		//set the spacing
		Style.Size(MakeFixed(nSize));
		Style.BaseSize(MakeFixed(nSize));
		Style.UpdateFontMetrics();
		Style.LeadingType(1);
		Style.Leading(MakeFixed((rcBounds.y1 - rcBounds.y0)*72/1800));

		//should we append the text or create a new text slab
		int nAdd = 2 * (rcBounds.x1 - rcBounds.x0) / pTextInfo->m_strText.GetLength();
		if( m_nLastPS4Panel == nPS4Panel && nRotation == pTextInfo->m_lf.lfEscapement &&
			m_ptLastTextLine.x + nAdd > pTextInfo->m_ptLoc.x)
		{
			{
				Txp.InsertString("\n");

				int nHeight = (pTextInfo->m_ptLoc.y - m_ptLastTextLine.y) * 72 / sdDPI;
				Style.Leading(MakeFixed(nHeight));
				nParaNum++;
			}

			//set the unrotated bounds
			m_pbTextBounds.x0 = min(m_pbTextBounds.x0, rcBounds.x0);
			m_pbTextBounds.x1 = max(m_pbTextBounds.x1, rcBounds.x1);
			m_pbTextBounds.y1 = max(m_pbTextBounds.y1, rcBounds.y1);
			m_pCurTextFrame->set_unrotated_bound(m_pbTextBounds);
		}
		else //create a new frame obj
		{
			FRAMEOBJ_CREATE_STRUCT fcs;
			memset(&fcs, 0, sizeof(fcs));
			fcs.bound = rcBounds;
			fcs.vert_alignment = ALIGN_top;
			fcs.alignment = ALIGN_center;
			fcs.m_Fill.SetColor(FillFormatV1::Black, TRANSPARENT_COLOR);

			if ((m_pCurTextFrame = pDatabase->create_frame_object(&fcs)) != NULL)
			{
				m_pDoc->append_object(m_pCurTextFrame);
				m_pCurTextFrame->set_panel(m_pDoc->get_current_panel());
				Txp.Init(m_pCurTextFrame, 0);
			}

			m_pbTextBounds = rcBounds;
			m_nLastRightBoundary = 0;
			nParaNum = 0;
		}
		m_pCurTextFrame->set_rotation((double)pTextInfo->m_lf.lfEscapement / 1800 * 3.14159 );

		CTextRecord* pRecord = Txp.LockText();

		Style.Alignment(ALIGN_center);

		Style.FillSetColor(0, COLOR_FROM_COLORREF(pTextInfo->m_crTextColor));

		pRecord->ChangeParagraphStyle(nParaNum, nParaNum, &Style);
		pRecord->release();
		// Put in the first line of text.
		Txp.Style(Style);
		Txp.InsertString(pTextInfo->m_strText);

		m_pCurTextFrame->calc();

		//store the last text out pos and panel
		if(m_ptLastTextLine.y != pTextInfo->m_ptLoc.y || m_nLastPS4Panel != nPS4Panel )
			m_ptLastTextLine = pTextInfo->m_ptLoc;

		m_nLastRightBoundary = rcBounds.x1;

		m_nLastPS4Panel = nPS4Panel;
		nRotation = pTextInfo->m_lf.lfEscapement;
	}


/*	//undo the offset that was added for the rotation
	if(m_lf.lfEscapement != 0)
	{
		int nXOffset = (m_pbTextBounds.x1 - m_pbTextBounds.x0) / 2 - (m_pbTextBounds.y1 - m_pbTextBounds.y0) ;
		int nYOffset = (m_pbTextBounds.x1 - m_pbTextBounds.x0) / 2 ;
		m_pbTextBounds.x0 += nXOffset;
		m_pbTextBounds.y0 += nYOffset;
		m_pbTextBounds.x1 += nXOffset;
		m_pbTextBounds.y1 += nYOffset;

		int nOffset = rcBounds.x0 - m_pbTextBounds.x0;
		rcBounds.y0 += nOffset;
		rcBounds.y1 += nOffset;
		rcBounds.x0 -= nOffset;
		rcBounds.x1 -= nOffset;
	}
		if(m_lf.lfEscapement != 0)
		{
			CString strWarning;
			strWarning.Format("%s has rotated text.%c%c",m_pszFileName, 13, 10);
			sErrorLog.Write(strWarning, strWarning.GetLength());
			if(strlen(pString) == 1)
			{
				CString strWarning;
				strWarning.Format("%s has text along path.%c%c",m_pszFileName, 13, 10);
				sErrorLog.Write(strWarning, strWarning.GetLength());
			
			}
			Style.Alignment(ALIGN_left);
		}
			//rotate and offset the frame obj
			if(m_lf.lfEscapement != 0)
			{
				PBOX pbUnrotated = m_pCurTextFrame->get_unrotated_bound();
				int nXOffset = (pbUnrotated.x1 - pbUnrotated.x0) / 2 - (pbUnrotated.y1 - pbUnrotated.y0) / 2 ;
				int nYOffset = (pbUnrotated.x1 - pbUnrotated.x0) / 2 ;
				pbUnrotated.x0 -= nXOffset;
				pbUnrotated.y0 -= nXOffset;
				pbUnrotated.x1 -= nXOffset;
				pbUnrotated.y1 -= nXOffset;
				m_pCurTextFrame->set_unrotated_bound(pbUnrotated);
				m_pCurTextFrame->set_rotation((double)m_lf.lfEscapement / 1800 * 3.14159 );
			}
			*/

}

void CMyMetaFile::AddText()
{
	PMGDatabase* pDatabase = m_pDoc->get_database();
	CTxp Txp(pDatabase);
	int nParaNum = 0;

	POSITION pos = m_TextInfoList.GetHeadPosition();
	while(pos)
	{
		CTextInfo* pTextInfo = m_TextInfoList.GetNext(pos);

		//set the panel
		int nPS4Panel = SetPanel(pTextInfo->m_ptLoc);
		int nPanel = m_pDoc->get_current_panel();
		
		PMGFontServer* pFontServer = (PMGFontServer*)m_pDoc->get_font_server();

		if(strcmp(pTextInfo->m_lf.lfFaceName, "Arial") == 0)
		{
			CString strWarning;
			strWarning.Format("%s has imported arial text.%c%c",m_pszFileName, 13, 10);
			sErrorLog.Write(strWarning, strWarning.GetLength());
			strcpy(pTextInfo->m_lf.lfFaceName, m_strDefaultFontName );
		}

		//create the font
		CTextStyle Style(pDatabase);
		Style.SetDefault();
		int nFace = typeface_server.find_face(pTextInfo->m_lf.lfFaceName, FALSE);
		if (nFace != -1)
		{
			DB_RECORD_NUMBER lFace = pFontServer->font_face_to_record(nFace);
			if (lFace != 0)
			{
				Style.Font(lFace);
				pDatabase->free_font_record(lFace, TRUE);
			}
		}

		int nSize = GetFontSize(pTextInfo->m_lf);

		//determine text bounds.
		CDC* pDC = AfxGetMainWnd()->GetDC();
		int nOldMode = pDC->SetMapMode(MM_LOENGLISH);
		CFont font;
		LOGFONT lf;
		memcpy(&lf, &(pTextInfo->m_lf), sizeof(LOGFONT));
		lf.lfHeight = nSize * 10;
		font.CreatePointFontIndirect(&lf, pDC );
		CFont* pOldFont = pDC->SelectObject(&font);
		CSize szText = pDC->GetTextExtent(pTextInfo->m_strText);

		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);

		pDC->SelectObject(pOldFont);
		pDC->SetMapMode(nOldMode);
		AfxGetMainWnd()->ReleaseDC(pDC);
		CRect rcIn(pTextInfo->m_ptLoc, pTextInfo->m_ptLoc);
		PBOX rcBounds = ComputeOutputRect(rcIn, nPS4Panel );
		
		//convert the size of the text to our resolution. and fudge it a little.  This is also a necessary 
		//hack
		rcBounds.x0 -= 75;
		rcBounds.x1 = rcBounds.x0 + szText.cx * 18 + 150;
		rcBounds.y1 = rcBounds.y0 + szText.cy * 18;
		int nYOffset;
		if(m_nVertTextAlign == TA_BASELINE)
		{
			nYOffset = tm.tmAscent * 18;
			rcBounds.y0 -= nYOffset;
			rcBounds.y1 -= nYOffset;
		}

		//set the spacing
		Style.Size(MakeFixed(nSize));
		Style.BaseSize(MakeFixed(nSize));
		Style.UpdateFontMetrics();
		Style.LeadingType(1);
		Style.Leading(MakeFixed((rcBounds.y1 - rcBounds.y0)*72/1800));

		//should we append the text or create a new text slab
		if( m_nLastPS4Panel == nPS4Panel && (m_pbTextBounds.y1 + 1.1 * nSize * 1800 / 72 > rcBounds.y0 && 
			rcBounds.y0 >= m_pbTextBounds.y0) && (rcBounds.x0 < m_pbTextBounds.x1 && rcBounds.x1 > m_pbTextBounds.x0) )
		{
			if(m_ptLastTextLine.y != pTextInfo->m_ptLoc.y)
			{
				Txp.InsertString("\n");

				int nHeight = (pTextInfo->m_ptLoc.y - m_ptLastTextLine.y) * 72 / sdDPI;
				Style.Leading(MakeFixed(nHeight));
				nParaNum++;
			}

			//set the unrotated bounds
			m_pbTextBounds.x0 = min(m_pbTextBounds.x0, rcBounds.x0);
			m_pbTextBounds.x1 = max(m_pbTextBounds.x1, rcBounds.x1);
			m_pbTextBounds.y1 = max(m_pbTextBounds.y1, rcBounds.y1);
			m_pCurTextFrame->set_unrotated_bound(m_pbTextBounds);
		}
		else //create a new frame obj
		{
			FRAMEOBJ_CREATE_STRUCT fcs;
			memset(&fcs, 0, sizeof(fcs));
			fcs.bound = rcBounds;
			fcs.vert_alignment = ALIGN_top;
			fcs.alignment = ALIGN_center;
			fcs.m_Fill.SetColor(FillFormatV1::Black, TRANSPARENT_COLOR);

			if ((m_pCurTextFrame = pDatabase->create_frame_object(&fcs)) != NULL)
			{
				m_pDoc->append_object(m_pCurTextFrame);
				m_pCurTextFrame->set_panel(m_pDoc->get_current_panel());
				Txp.Init(m_pCurTextFrame, 0);
			}

			m_pbTextBounds = rcBounds;
			m_nLastRightBoundary = 0;
			nParaNum = 0;
		}

		CTextRecord* pRecord = Txp.LockText();

		if(pTextInfo->m_ptLoc.x == m_ptLastTextLine.x)
		{
			CTextStyle lastStyle;
			pRecord->GetParagraphStyle(nParaNum - 1, lastStyle);
			if(lastStyle.Alignment() != ALIGN_left)
			{
				lastStyle.Alignment(ALIGN_left);
				pRecord->ChangeParagraphStyle(nParaNum-1, nParaNum - 1, &lastStyle);
			}

			Style.Alignment(ALIGN_left);
		}
		else if (rcBounds.x1 == m_nLastRightBoundary)
		{
			CTextStyle lastStyle;
			pRecord->GetParagraphStyle(nParaNum - 1, lastStyle);
			if(lastStyle.Alignment() != ALIGN_right)
			{
				lastStyle.Alignment(ALIGN_right);
				pRecord->ChangeParagraphStyle(nParaNum-1, nParaNum - 1, &lastStyle);
			}

			Style.Alignment(ALIGN_right);
		}
		else
			Style.Alignment(ALIGN_center);

		Style.FillSetColor(0, COLOR_FROM_COLORREF(pTextInfo->m_crTextColor));

		pRecord->ChangeParagraphStyle(nParaNum, nParaNum, &Style);
		pRecord->release();
		// Put in the first line of text.
		Txp.Style(Style);
		Txp.InsertString(pTextInfo->m_strText);

		m_pCurTextFrame->calc();

		//store the last text out pos and panel
		if(m_ptLastTextLine.y != pTextInfo->m_ptLoc.y || m_nLastPS4Panel != nPS4Panel )
			m_ptLastTextLine = pTextInfo->m_ptLoc;

		m_nLastRightBoundary = rcBounds.x1;

		m_nLastPS4Panel = nPS4Panel;
	}
}

//handles the stretch dib record
void CMyMetaFile::StretchDIBRecord(METARECORD* pMetaRecord)
{
	recordMetaSTRETCHDIB *pRecord = (recordMetaSTRETCHDIB*)pMetaRecord;
	//first pass
	if(m_bGetData)
	{	//create prject if not already created
		if( !m_bProjectCreated )
		{
			if(!CreateNewProject(""))
			{
				CString strWarning;
				strWarning.Format("%s project creation failed.%c%c",m_pszFileName, 13, 10);
				sErrorLog.Write(strWarning, strWarning.GetLength());
				m_bBail = TRUE;
				return;
			}
		}
		//is it a new bmp add new info to array
		if( IsNewBmp(pMetaRecord) )
		{
			BmpData newBmpData;
			newBmpData.pBmpInfo = new BITMAPINFO;
			newBmpData.pBits = NULL;
			newBmpData.rcBmpDst.SetRect(pRecord->wDstX,pRecord->wDstY, pRecord->wDstX + pRecord->wDstXExt, pRecord->wDstY + pRecord->wDstYExt);
			memcpy(newBmpData.pBmpInfo, &(pRecord->BitmapInfo), sizeof(BITMAPINFO));
			m_nCurBmpIndex = m_BmpDataArray.Add(newBmpData);
		}
		else
		{
			//fill in the info
			m_BmpDataArray[m_nCurBmpIndex].rcBmpDst.bottom = pRecord->wDstY + pRecord->wDstYExt;
			m_BmpDataArray[m_nCurBmpIndex].pBmpInfo->bmiHeader.biHeight++;
		}
		return;
	}
	//second pass

	DWORD nScanLine = (pRecord->wSrcXExt*pRecord->BitmapInfo.bmiHeader.biBitCount+7)/8;
	nScanLine = (nScanLine + 3) & ~3;

	int m_nCurBmpIndex = GetBmpDataIndex(pMetaRecord );
	ASSERT(m_nCurBmpIndex < m_BmpDataArray.GetSize());

	//have the memory for the data been allocated
	if(m_BmpDataArray[m_nCurBmpIndex].pBits == NULL )
	{
		m_BmpDataArray[m_nCurBmpIndex].pBits = new BYTE[sizeof(BITMAPINFOHEADER) + 
								nScanLine * m_BmpDataArray[m_nCurBmpIndex].pBmpInfo->bmiHeader.biHeight];
		//copy the bitmapinfo data
		memcpy(m_BmpDataArray[m_nCurBmpIndex].pBits, m_BmpDataArray[m_nCurBmpIndex].pBmpInfo, sizeof(BITMAPINFOHEADER));
		//move pointer to the end because it is a bottom up bitmap
		m_pTempBits = m_BmpDataArray[m_nCurBmpIndex].pBits + 
						sizeof(BITMAPINFOHEADER) + 
						nScanLine * m_BmpDataArray[m_nCurBmpIndex].pBmpInfo->bmiHeader.biHeight;
	}
	//copy the bmp data
	m_pTempBits -= nScanLine;
	BYTE* pBmpBits = (BYTE*)pRecord->BitmapInfo.bmiColors;
	memcpy(m_pTempBits, pBmpBits, nScanLine);

	if(pRecord->wDstY > 9000)
		int z = 0;
	//is this the last line of data for the bmp. 
	if( pRecord->wDstY + pRecord->wDstYExt == m_BmpDataArray[m_nCurBmpIndex].rcBmpDst.bottom )
	{
		//we have all the data for the bmp let's create a graphic object and add it to the doc
		GraphicObject* pObject;
		//create a unique name
		char cbName[30];
		clipboard.SetUpdateTime();
		clipboard.BuildPasteName(".BMP", cbName);

		int nPS4Panel = SetPanel( m_BmpDataArray[m_nCurBmpIndex].rcBmpDst.TopLeft() );
		//create the graphic object
		CreateBMPFromDIB(cbName, m_pDoc->get_database(),(BITMAPINFO*)m_BmpDataArray[m_nCurBmpIndex].pBits, pObject);

		PBOX rcBounds = ComputeOutputRect(m_BmpDataArray[m_nCurBmpIndex].rcBmpDst, nPS4Panel );
		pObject->set_bound(rcBounds);
		pObject->calc();
		m_pDoc->append_object(pObject);
		pObject->set_panel(m_pDoc->get_current_panel());
		m_pDoc->add_refresh(&rcBounds);
		m_bOkToAddPoly = TRUE;
	}
}
//true if starting a new bmp
BOOL CMyMetaFile::IsNewBmp(METARECORD* pMetaRecord)
{
	recordMetaSTRETCHDIB *pRecord = (recordMetaSTRETCHDIB*)pMetaRecord;

	int nLast = m_BmpDataArray.GetSize() - 1;
	if(nLast >= 0)
	{
		BITMAPINFO* pInfo = m_BmpDataArray[nLast].pBmpInfo;
		if(pInfo->bmiHeader.biWidth == pRecord->BitmapInfo.bmiHeader.biWidth && 
			m_BmpDataArray[nLast].rcBmpDst.left == pRecord->wDstX)
		{
			return FALSE;
		}
	}

	return TRUE;
}
//returns the index into the array for this bmp
int CMyMetaFile::GetBmpDataIndex(METARECORD* pMetaRecord)
{
	recordMetaSTRETCHDIB *pRecord = (recordMetaSTRETCHDIB*)pMetaRecord;

	for(int i = 0; i < m_BmpDataArray.GetSize(); i++)
	{
		if(m_BmpDataArray[i].pBmpInfo->bmiHeader.biWidth == pRecord->BitmapInfo.bmiHeader.biWidth && 
			m_BmpDataArray[i].rcBmpDst.left == pRecord->wDstX )
		{
			return i;
		}
	}
	return -1;
}

BOOL CMyMetaFile::Init(LPCSTR pszFileName, CString strDefaultFontName)
{
	Reset();
	///set the file name and the defualt font name
	m_pszFileName = pszFileName;
	m_strDefaultFontName = strDefaultFontName;
	m_bGetTextInfo = FALSE;
	
	//set up the info for a default project
	m_pInfo = &(GET_PMWAPP()->last_new_info);
	m_pInfo->type = PROJECT_TYPE_Poster;
	m_pInfo->subtype = 0;
	m_pInfo->orientation = PORTRAIT;
	m_pInfo->pTemplate = NULL;
	m_pInfo->m_pPaperInfo = new CPaperInfo;
	m_pInfo->m_pCalendarInfo = NULL;
	//init variables
	m_bProjectCreated = FALSE;
	m_bGetData = FALSE;
	m_pTempBits = NULL;
	m_pDoc = NULL;
	//set to bogus values
	m_ptLastTextLine.x = -32767;
	m_ptLastTextLine.y = -32767;
	m_bOkToAddPoly = FALSE;
	m_bBail = FALSE;
	m_nCurPolyIndex = 0;
	m_strSaveExt = ".hcr";
	m_bBrushCreated = FALSE;
	m_bPenCreated = FALSE;
	//set to bogus values
	m_nCurBrushObjSel = -1;
	m_nCurPenObjSel = -1;

	return TRUE;
}

BOOL CMyMetaFile::GetMetaFile(void)
{
	BOOL fSuccess = FALSE;
	ASSERT(m_pszFileName != NULL);
	if(m_hMetaFile != NULL)
		return m_fMetaFileValid;
	
	CFile File;
	HMETAFILE* phMetaFile = &m_hMetaFile;
	if (File.Open(m_pszFileName, CFile::modeRead | CFile::shareDenyWrite))
	{
		// Read the metafile header.
		METAHEADER Header;
		DWORD dwPosition = File.GetPosition();
		if (File.Read(&Header, sizeof(Header)) == sizeof(Header))
		{
			*phMetaFile = NULL;
			// Get the size from the metafile header.
			DWORD dwSize = Header.mtSize*2;		
			if (dwSize != 0)
			{
				// Allocate the memory to hold the metafile data.
				HGLOBAL hMetaFileData;					
				hMetaFileData = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, dwSize);					
				if (hMetaFileData != NULL)
				{
					LPVOID pMetaFileData;						
					pMetaFileData = GlobalLock(hMetaFileData);						
					if (pMetaFileData != NULL)
					{
						// Seek back to the start of the metafile header.						
						File.Seek(dwPosition, CFile::begin);
						// Read the metafile data.						
						if (File.ReadHuge(pMetaFileData, dwSize) == dwSize)
						{
							// We have now read the data, make a metafile from it.

#if defined (_MAC)
                 // TODO:MAC -- SetMetaFileBitsEx not implemented
#elif defined(WIN32)
							*phMetaFile = ::SetMetaFileBitsEx(dwSize, (LPBYTE)pMetaFileData);
#else
							*phMetaFile = ::SetMetaFileBits(hMetaFileData);
#endif
							fSuccess = *phMetaFile != NULL;
						}

#ifdef WIN32
						GlobalUnlock(hMetaFileData);
						pMetaFileData = NULL;
#else
						if (*phMetaFile == NULL)
						{
							GlobalUnlock(hMetaFileData);
							pMetaFileData = NULL;
						}
#endif
					}
#ifdef WIN32
					GlobalFree(hMetaFileData);
					hMetaFileData = NULL;
#else
					if (*phMetaFile == NULL)
					{
						GlobalFree(hMetaFileData);
						hMetaFileData = NULL;
					}
#endif
				}
			}
		}
	}

	m_fMetaFileValid = TRUE;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CAgcvtDlg dialog

CAgcvtDlg::CAgcvtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAgcvtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAgcvtDlg)
	m_nFontNorm = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
}

void CAgcvtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAgcvtDlg)
	DDX_Control(pDX, IDC_FILE, m_editFile);
	DDX_Radio(pDX, IDC_FONT_NORM, m_nFontNorm);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAgcvtDlg, CDialog)
	//{{AFX_MSG_MAP(CAgcvtDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgcvtDlg message handlers

BOOL CAgcvtDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CAgcvtDlg::OnBrowse() 
{
	CFileDialog	FileDialog(TRUE, ".WMF", NULL, OFN_ALLOWMULTISELECT, "WMF Files (*.wmf)|*.wmf|All Files (*.*)|*.*||", this);
	char path[8192];
	FileDialog.m_ofn.lpstrFile = path;
	FileDialog.m_ofn.nMaxFile = 8192;
	
	if (FileDialog.DoModal() == IDOK)
	{
		POSITION pos = FileDialog.GetStartPosition();
		while(pos)
		{
			CString csPath = FileDialog.GetNextPathName(pos);
			if (!csPath.IsEmpty())
			{
				m_editFile.SetWindowText(csPath);
				UpdateWindow();
				ParseWmf(csPath);
			}
		}
	}
}

void CAgcvtDlg::ParseWmf(const CString& csPathName)
{
	

	CMyMetaFile MetaFile;
	CString strDefaultFontName = "CAC Norm Heavy";
	UpdateData();
	if(m_nFontNorm)
		strDefaultFontName = "CAC Saxon Bold";

	if (MetaFile.Init(csPathName, strDefaultFontName))
	{
		CDC* pDC = GetDC();
		MetaFile.ProcessMetafile(pDC, FALSE, (LPARAM)this);
		ReleaseDC(pDC);
	}
}
