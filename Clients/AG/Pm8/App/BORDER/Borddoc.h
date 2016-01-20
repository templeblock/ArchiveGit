#if !defined(AFX_BORDDOC_H__E12ED161_85E2_11D1_8680_006008661BA9__INCLUDED_)
#define AFX_BORDDOC_H__E12ED161_85E2_11D1_8680_006008661BA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Borddoc.h : header file
//

#include "borddef.h"
#include "beobjd.h"
#include "command.h"
#include "SettingsDlg.h"
#include "popup.h"

class CBorddoc;
class CBordview;

/////////////////////////////////////////////////////////////////////////////
// CBrowserDataExchange

struct CBrowserDataExchange
{
	DWORD m_dwFilePathOffset;		// (Relative to start of structure)
	DWORD m_dwFriendlyNameOffset;	// (Relative to start of structure)
	DWORD m_dwCropCopyrightOffset;
};

/////////////////////////////////////////////////////////////////////////////
// Command classes derived from CCommand

// Abstract helper class
class CBECommand : public CCommand
{
protected:
	CBorddoc* m_pDoc;
	CBEObjectD* m_pBEObj;

	// Whether or not this command is happening for the first time
	BOOL m_bNewCommand;
	BOOL m_bForceUpdate;	// Always update view

public:
	CBECommand(CBorddoc* pDoc, CBEObjectD* pBEObject) :
	  CCommand()
	  {
		  m_pDoc = pDoc;
		  m_pBEObj = pBEObject;
		  m_bNewCommand = TRUE;
	  }
};

// Slide bars causing grid to change
class CCmdSetGridCoords : public CBECommand
{
	// Grid indeces and data
	int m_nXIndex[2], m_nYIndex[2];
	LONG m_nNewXValues[2], m_nNewYValues[2];
	LONG m_nOldXValues[2], m_nOldYValues[2];

public:
	CCmdSetGridCoords(CBorddoc* pDoc, CBEObjectD* pBEObject);
	void SetGridCoords(int nXIndex1, int nYIndex1, LONG nNewXValue1, LONG nNewYValue1, int nXIndex2, int nYIndex2, LONG nNewXValue2, LONG nNewYValue2);

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

// Rotate a region 90 degrees
class CCmdRotateRegion : public CBECommand
{
	// Rotation angle
	double m_dAngle;
	
	// regions selected at time of operation
	BOOL m_aSelectedRegions[N_EDIT_REGIONS];

public:
	CCmdRotateRegion(CBorddoc* pDoc, CBEObjectD* pBEObject);
	void SetMembers(double dAngle = RAD_90);

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

// Flip a region horizontally or vertically
class CCmdFlipRegion : public CBECommand
{
	// Axis flip
	enum FlipTypesE m_Flip;
	
	// regions selected at time of operation
	BOOL m_aSelectedRegions[N_EDIT_REGIONS];

public:
	CCmdFlipRegion(CBorddoc* pDoc, CBEObjectD* pBEObject);
	void SetType(enum FlipTypesE flip)
	{
		m_pBEObj->CopySelectionStates(m_aSelectedRegions);
		m_Flip = flip;
	}

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

// Changes region settings
class CCmdChangeSettings : public CBECommand
{
	// Saved data
	double m_aXOffsets[3], m_aYOffsets[3];
	BOOL m_bForceTiles[4];
	int m_nTiles[4];
	int m_nMaxTiles[4];

	// Changes
	CSettings m_Settings;

	// regions selected at time of operation
	BOOL m_aSelectedRegions[N_EDIT_REGIONS];

public:
	CCmdChangeSettings(CBorddoc* pDoc, CBEObjectD* pBEObject);
	void ChangeSettings(CSettingsDlg& SettingsDlg);

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

// Changes background color
class CCmdChangeBGColor : public CBECommand
{
	// Saved data
	BOOL m_bOldUseBGColor;			
	COLORREF m_colOldBGColor;
	
	// New data
	BOOL m_bNewUseBGColor;
	COLORREF m_colNewBGColor;

public:
	CCmdChangeBGColor(CBorddoc* pDoc, CBEObjectD* pBEObject);
	void ChangeBGColor(BOOL bUseBGColor, COLORREF colBGColor);

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

// Choose a graphic
class CCmdChooseGraphic : public CBECommand
{
protected:
	// Saved IDs
	DWORD m_dwIDs[N_EDIT_REGIONS];

	// New ID
	DWORD m_dwNewID;

	// regions selected at time of operation & transforms
	BOOL m_aSelectedRegions[N_EDIT_REGIONS];
	CEditRegion::TransformsT m_aTransforms[N_EDIT_REGIONS];

public:
	CCmdChooseGraphic(CBorddoc* pDoc, CBEObjectD* pBEObject);
	void ChooseGraphic(DWORD dwID);

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

// Abstract copy region class
class CCmdCopyRegion : public CBECommand
{
protected:
	// Save source region index and graphic ID
	int m_nSrcRegion;
	DWORD m_dwSrcID;

	// Save destination region index and graphic ID
	BOOL m_aDestRegions[N_EDIT_REGIONS];
	DWORD m_dwIDs[N_EDIT_REGIONS];
	CEditRegion::TransformsT m_aTransforms[N_EDIT_REGIONS];

public:
	CCmdCopyRegion(CBorddoc* pDoc, CBEObjectD* pBEObject);
	void SaveStates();

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

// Copy an edge vertically/horizontal
class CCmdCopyOpposite : public CCmdCopyRegion
{
	// Override source regions
	BOOL m_aSrcRegions[N_EDIT_REGIONS];

public:
	CCmdCopyOpposite(CBorddoc* pDoc, CBEObjectD* pBEObject);
	void CopyOpposite();

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

// Copy an edge to all edges
class CCmdCopyEdges : public CCmdCopyRegion
{

public:
	CCmdCopyEdges(CBorddoc* pDoc, CBEObjectD* pBEObject);
	void CopyEdges();

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

// Copy a corner to all corners
class CCmdCopyCorners : public CCmdCopyRegion
{

public:
	CCmdCopyCorners(CBorddoc* pDoc, CBEObjectD* pBEObject);
	void CopyCorners();

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

// Cut/copy/paste graphics
class CCmdCutGraphic : public CCmdChooseGraphic
{
public:
	CCmdCutGraphic(CBorddoc* pDoc, CBEObjectD* pBEObject);
	void CutGraphic();

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

class CCmdCopyGraphic : public CCmdChooseGraphic
{
public:
	CCmdCopyGraphic(CBorddoc* pDoc, CBEObjectD* pBEObject);

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

class CCmdPasteGraphic : public CCmdChooseGraphic
{
public:
	CCmdPasteGraphic(CBorddoc* pDoc, CBEObjectD* pBEObject);
	void InitPasteGraphic(COleDataObject* pDataObject);

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

// Zoom operation
class CCmdZoom : public CBECommand
{
	enum CBEObjectD::ZoomActionsE m_zoom;

public:
	CCmdZoom(CBorddoc* pDoc, CBEObjectD* pBEObject);
	void SetZoom(enum CBEObjectD::ZoomActionsE zoom)
	{
		m_zoom = zoom;
	}

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

/////////////////////////////////////////////////////////////////////////////
// CBorddoc document

class CBorddoc : public CDocument
{
// Definitions
	// Undo structures
	struct CopyEdgesT {
		int nSourceEdge;
		DWORD dwID[3];
	};
	struct CopyCornersT {
		int nSourceCorner;
		DWORD dwID[3];
	};
	struct EdgeSettingsT {
		BOOL bForceTile;
		int nTiles;
		double dEdgeInchSize;
	};

	struct UndoBufferT {
		size_t szStructSize;
		int redraw_lHint;
		CObject* redraw_pHint;
		void (*pfunc)(CBorddoc*, void*);
		char* buffer;		// allocated when doc is created
	};

	CBECallbackInterface* m_pCallbackInterface;	// pointer to the interface
	CBEObjectD* m_pCurrBEObj;					// points to current border editor object
	
	// Undo, redo
	CCommandList m_CommandList;

	// Drawing
	BOOL m_bFirstDraw;

	// Selection states
	BOOL m_bSelectAll;
	BOOL m_bSelectCorners;
	BOOL m_bSelectEdges;
	BOOL m_bSelectHEdges;
	BOOL m_bSelectVEdges;

protected:
	CBorddoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBorddoc)

// Attributes
public:
	void SetCallbackInterface(CBECallbackInterface* pCallbackInterface)
	{
		m_pCallbackInterface = pCallbackInterface;
	}
	CBECallbackInterface* GetCallbackInterface()
	{
		return (m_pCallbackInterface);
	}
	void SetCurrBEObj(CBEObjectD* pBEObject);
	void GetBEGrid(LONG aXValues[], LONG aYValues[])
	{
		m_pCurrBEObj->GetXCoords(aXValues);
		m_pCurrBEObj->GetYCoords(aYValues);
	}
	void GetBELimits(RECTAliasT& rctLimits)
	{
		m_pCurrBEObj->GetLimits(rctLimits);
	}
	BOOL AddCommand(CBECommand* pCmd, BOOL bDoIt = TRUE);

// Operations
public:
	void Resize(RECT& rNewSpaceBound, SIZE& sMinBorder);
	BOOL DrawBEGrid(CDC* pDC, CDC* pMemDC, CWnd* pDrawWnd, BOOL bDrawBackground);
	BOOL DrawMinBEGrid(CDC* pDC);
	BOOL DrawBESelections(CDC* pDC, BOOL bErase)
	{
		m_pCurrBEObj->DrawSelections(pDC, bErase);
		return (TRUE);
	}
	BOOL SelectBEEditRegion(BOOL bKeep, BOOL bToggle, CPoint point);
	BOOL SelectBEDragRegion(CPoint* point);
	BOOL PermSelectBEDragRegion();
	void ClearBEDrawing()
	{
		m_bFirstDraw = TRUE;
	}
	void SetGridCoords(int nXIndex1, int nYIndex1, LONG nNewXValue1, LONG nNewYValue1, int nXIndex2, int nYIndex2, LONG nNewXValue2, LONG nNewYValue2);
	void FlipRegion(enum FlipTypesE flip);
	void InitBEDragDrop()
	{
		m_pCurrBEObj->InitDragDrop();
	}
	void ChooseGraphicWArtGallery();
	void ChooseGraphic(DWORD dwID);
	void PasteGraphic(COleDataObject* pDataObject);
	int PasteFromOleDataObject(UINT uFormat, COleDataObject* pDataObject, RefDataT* pRefData);
	void CopyGraphic();
	int CopyOleDataObject(COleDataSource* pData);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBorddoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBorddoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	BOOL OnUpdateSelectHelper(CCmdUI* pCmdUI, enum RegionGroupsE group, UINT nOnID, UINT nOffID);
	void OnModifyPalette(UINT uID, UINT uPaletteID, CPopupPalette& Window);

	//{{AFX_MSG(CBorddoc)
	afx_msg void OnFileSaveAs();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnUpdateZoomIn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomOut(CCmdUI* pCmdUI);
	afx_msg void OnRotate90();
	afx_msg void OnUpdateTransforms(CCmdUI* pCmdUI);
	afx_msg void OnFlipHorizontal();
	afx_msg void OnFlipVertical();
	afx_msg void OnSettings();
	afx_msg void OnUpdateSettings(CCmdUI* pCmdUI);
	afx_msg void OnChooseGraphic();
	afx_msg void OnUpdateChooseGraphic(CCmdUI* pCmdUI);
	afx_msg void OnSelectCorners();
	afx_msg void OnSelectEdges();
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnCopyEdges();
	afx_msg void OnUpdateCopyEdges(CCmdUI* pCmdUI);
	afx_msg void OnCopyCorners();
	afx_msg void OnUpdateCopyCorners(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCutCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnSelectHEdges();
	afx_msg void OnSelectVEdges();
	afx_msg void OnUpdateSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectCorners(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectEdges(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectHEdges(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectVEdges(CCmdUI* pCmdUI);
	afx_msg void OnChooseBgColor();
	afx_msg void OnUpdateChooseBgColor(CCmdUI* pCmdUI);
	afx_msg void OnBackgroundColorPalette();
	afx_msg void OnCopyAcross();
	afx_msg void OnUpdateCopyAcross(CCmdUI* pCmdUI);
	afx_msg void OnRevert();
	afx_msg void OnUpdateRevert(CCmdUI* pCmdUI);
	afx_msg void OnSelectAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BORDDOC_H__E12ED161_85E2_11D1_8680_006008661BA9__INCLUDED_)
