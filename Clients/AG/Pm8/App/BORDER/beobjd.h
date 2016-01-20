// beobjd.h : Header for the derived class 'CBEObjectD'
//

#ifndef __BEOBJD_H__
#define __BEOBJD_H__

#include "borddef.h"
#include "editrgn.h"

#include <afxtempl.h>

const N_EDIT_REGIONS = 8;

enum RegionGroupsE {
	ALL_R, CORNERS_R, EDGES_R, EDGES_H_R, EDGES_V_R, EDGE_R
};

struct SRedrawStateT {
	void* pDrawState;

	int nRegionIndex;	// region we were drawing
	int nTileIndex;		// tile we were drawing
};

/////////////////////////////////////////////////////////////////////////////
// CCntArray - class derived from CArray where you can
//				store the # of valid elements in it.
template<class TYPE, class ARG_TYPE>
class CCntArray : public CArray<TYPE, ARG_TYPE>
{
	int m_nValidItems;	// # of valid items in the array

public:
	int GetNValidItems()
	{
		return (m_nValidItems);
	}
	void SetNValidItems(int nValidItems)
	{
		m_nValidItems = nValidItems;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CBEObjectD - interface definition for border object
//             These objects are created and destroyed by the border DLL in
//             response to requests from the client (via NewObject()).

class CBEObjectD : public CBEObject
{
// Definitions
	struct SRawDataT {
		UINT version;

		double dXSize, dYSize;				// Size of border in inches
		double aXCoords[3], aYCoords[3];	// Positions of grid points, offset from (0,0) in inches

		// Background color
		BOOL bUseBGColor;					// TRUE when BG color is to be displayed
		COLORREF colBGColor;				// Actual RGB of the BG color (-1L for transparent)
	
		// Inner rectangle
		RECT rctInner;

		// Edit region info
		DWORD dwIDs[N_EDIT_REGIONS];							// graphic ID for each edit region
		CEditRegion::TransformsT transforms[N_EDIT_REGIONS];	// transformation data for each graphic
		
		// Side region info
		BOOL bForceTile[4];
		int nTiles[4];
		int nMaxTiles[4];

	};

// Data
	// General
	// The outside and inside corner points are defined by 4 x and 4 y
	// coordinate values. Pointers to these values are passed to the
	// editing regions so that changes are automatically updated.
	//
	//      x0   x1               x2     x3
	//  y0	._____._______________.______.
	//      |     |				  |      |
	//		|     |               |      |
	//	y1	._____._______________.______.
	//		|     |               |      |
	//		|     |               |      |
	//		|     |               |      |
	//		|     |               |      |
	//		|     |               |      |
	//		|     |               |      |
	//		|     |               |      |
	//	y2	._____._______________.______.
	//      |     |				  |      |
	//		|     |               |      |
	//  y3	._____._______________.______.

	double m_dXSize, m_dYSize;					// Size of border in inches
	double m_aXOffsets[3], m_aYOffsets[3];		// Positions of grid points, offset from (0,0) in inches
	double m_aXZmOffsets[3], m_aYZmOffsets[3];	// Positions of grid points, zoom scale included
	LONG m_aXCoords[4], m_aYCoords[4];			// Define the 16 point grid
	int m_nZoomPosition;						// Current zoom position (+1 in, -1 out)
	double m_dZoomScale;						// Zoom multiplier
	void* m_pDatabase;							// Associated database
	BOOL m_bInteriorsInitialized;				// TRUE when interior gird points have been initialized
	RECT m_rctInner;							// inner rectangle of PM screen border

	// Background color
	BOOL m_bUseBGColor;						// TRUE when BG color is to be displayed
	COLORREF m_colBGColor;					// Actual RGB of the BG color (-1L for transparent)
	
	// Ratios for calcualtions
	double m_dDPInchRatio;					// Current ratio of device coordinates to inches
	double m_dAspectRatio;					// dYSize / dXSize

	// Edit regions
	CEditRegion* m_apEditRegions[N_EDIT_REGIONS];		// The 8 edit regions, allocated with derived class
	BOOL m_aSelectedRegions[N_EDIT_REGIONS];			// Corresponding list of selected regions
	BOOL m_bSelected;									// At least 1 region selected
	CCntArray<CRect, CRect&> m_aRectArray;	// Array of rectangles to be operated on
	int m_nDragRegion;						// For drag and drop

	// Normalized changed area
	double m_adChangedRect[4];

	// Saved data
	SRawDataT m_OriginalData;

	// For successful copy construction
	static BOOL m_bCopyCompleted;

// Helpers
	void SetAllRegionGraphics(DWORD dwIDs[], BOOL aRegions[], BOOL bMulti, BOOL bUpdateRef);

// Construction/destruction
public:
	CBEObjectD() {}
	CBEObjectD(void* pDatabase, int nLength, void* pData);
	CBEObjectD(CBEObjectD* pBEObjCopy, void* pDatabase);
	void Init(void* pDatabase);
	void CreateEditRegions();

// Interface
public:
	// Release the memory for this object.
	// The object pointer is no longer valid after this call.
	virtual int Release();

	// Get the "serialization data" for this object.
	virtual int GetData(int& nLength, void* pData = NULL);

	// Set the "serialization data" for this object.
	virtual int SetData(int nLength, void* pData);

	// Destroy the object (remove all graphic references).
	// This implicitly does a Release().
	// The object pointer is no longer valid after this call.
	virtual int Destroy();

	// Duplicate this border object.
	// The new object is returned.
	virtual int Duplicate(CBEObject*& pObject, void* pDatabase = NULL);

	// Set the size (aspect). Units are inches.
	virtual int SetSize(double dXSize, double dYSize);

	// Get inner rectangle dimensions
	virtual void GetInnerRect(RECT* pRect);

	// Draw the border.
	virtual int Draw(CBEDrawInfo* pDrawInfo);

	// Edit the border. This launches the editor interface.
	virtual int Edit(HWND hParent);

// Attributes
	void* GetDatabase()
	{
		return (m_pDatabase);
	}
	void SetXCoord(int nIndex, LONG nValue);
	void SetYCoord(int nIndex, LONG nValue);
	LONG GetXCoord(int nIndex)
	{
		return (m_aXCoords[nIndex]);
	}
	LONG GetYCoord(int nIndex)
	{
		return (m_aYCoords[nIndex]);
	}
	void GetXCoords(LONG aXCoords[]);
	void GetYCoords(LONG aYCoords[]);
	void SetXOffset(int nIndex, double dValue)
	{
		m_aXOffsets[nIndex] = dValue;
	}
	void SetYOffset(int nIndex, double dValue)
	{
		m_aYOffsets[nIndex] = dValue;
	}
	void GetXOffsets(double aXOffsets[]);
	void GetYOffsets(double aYOffsets[]);
	double GetXSize()
	{
		return (m_dXSize);
	}
	double GetYSize()
	{
		return (m_dYSize);
	}
	void GetLimits(RECTAliasT& rctLimits)
	{
		rctLimits.SetRECTAlias(&m_aXCoords[0], &m_aYCoords[0], &m_aXCoords[3], &m_aYCoords[3]);
	}
	void ComputeAspectRatio(double dX, double dY)
	{
		m_dAspectRatio = (dX != 0.) ? dY / dX : 0.;
	}
	CCntArray<CRect,CRect&>* GetRectList()
	{
		return (&m_aRectArray);
	}
	void ClearChangedRect();
	double* GetChangedRect()
	{
		return (m_adChangedRect);
	}
	int GetDragRegion()
	{
		return (m_nDragRegion);
	}
	void SetDragRegion(int nDragRegion)
	{
		m_nDragRegion = nDragRegion;
	}

	enum ZoomActionsE {
		ZoomIn, ZoomOut
	};
	void Zoom(ZoomActionsE zoom);
	BOOL AtZoomLimit(ZoomActionsE zoom);
	BOOL Selected()
	{
		return (m_bSelected);
	}
	BOOL EdgeCopyAllowed(enum FlipTypesE flip = NO_FLIP);
	BOOL CornerCopyAllowed();
	BOOL CanCutCopy();
	BOOL CanDoTransforms();
	void SetBGColor(BOOL bUse, COLORREF colBGColor)
	{
		m_bUseBGColor = bUse;
		m_colBGColor = colBGColor;
	}
	BOOL GetBGColor(COLORREF& colBGColor)
	{
		colBGColor = m_colBGColor;
		return (m_bUseBGColor);
	}
	BOOL IsEmpty();

// Edit Region
	BOOL SelectEditRegion(BOOL bKeep, BOOL bSelect, CPoint point);
	BOOL SelectDragRegion(CPoint* point);
	void PermSelectDragRegion();
	void ReselectRegions();
	void SelectAllRegions(BOOL bState);
	void SelectCornerRegions(BOOL bState);
	void SelectEdgeRegions(BOOL bState, enum FlipTypesE flip = NO_FLIP);
	void CopySelectionStates(BOOL aSelectedRegions[])
	{
		memcpy(aSelectedRegions, m_aSelectedRegions, sizeof(BOOL)*N_EDIT_REGIONS);
	}
	void GetTiling(BOOL& bForce, int& nTiles, int& nMaxTiles);
	void SetTiling(BOOL bForce, int nTiles, int nMaxTiles);
	void GetAllTiling(BOOL bForce[], int nTiles[], int nMaxTiles[]);
	void SetAllTiling(BOOL bForce[], int nTiles[], int nMaxTiles[]);
	BOOL GetSizing(double& dEdgeSize);
	void SetSizing(double dEdgeSize, BOOL aRegions[]);
	BOOL IsEdgeSelected();
	int GetOppositeEdge(int nEdgeIndex);
	enum SideTypeE GetRegionSide(int nEdgeIndex);
	void GetSelectionData(RefDataT* pRefData);
	BOOL AreRegionsSelected(enum RegionGroupsE group);

// Workspace and grid sizing
	void Resize(const RECT& rNewSpaceBound, const SIZE& sMinBorder);
	void ComputeGridCoords(BOOL bUseZoom = TRUE);
	void AdjustOffsets(BOOL bToZoom);
	void ScaleOffsets(double aXDest[], double aYDest[], double aXSrc[], double aYSrc[], double dScale);
	void CheckZoomedGrid();

// Drawing
	int DrawGrid(CBEDrawInfo* pDrawInfo, CDC* pMemDC, BOOL bDrawBackground, BOOL bWipeBackground);
	int DrawGrid(CBEDrawInfo* pDrawInfo);
	void DrawGridRect(CDC* pDC, BOOL bWipeBackground = TRUE, BOOL bWipeInner = TRUE);
	int DrawSelections(CDC* pDC, BOOL bErase);
	void DrawBackground(CBEDrawInfo* pDrawInfo);

// Callback interface wrappers
	DWORD ChooseGraphic(HWND hParent);
	DWORD ChooseGraphic(CString csFilePath, CString csFriendlyName, BOOL fCropCopyright);
	void RemoveRef(DWORD dwID);

// Operations
	void SetRegionGraphics(DWORD dwID, BOOL aRegions[], BOOL bNew);
	void SetRegionGraphics(DWORD dwIDs[], BOOL aRegions[], BOOL bNew);
	void SetRegionGraphic(DWORD dwID, int nRegion, BOOL bUpdateRef);
	void GetRegionGraphics(DWORD dwIDs[]);
	void SetRegionTransforms(CEditRegion::TransformsT transforms[]);
	void SetRegionTransforms(CEditRegion::TransformsT transforms, BOOL aRegions[]);
	void GetRegionTransforms(CEditRegion::TransformsT transforms[]);
	void RotateRegions(double dAngle, BOOL aRegions[]);
	void FlipRegions(enum FlipTypesE flip, BOOL aRegions[]);
	void ApplyRegionFlip(int nSrcRegion, BOOL aChangeRegions[]);
	void ApplyEdgeFlip(int nSrcEdge, BOOL aChangeRegions[]);
	void ApplyCornerFlip(int nSrcCorner, BOOL aChangeRegions[]);
	void ApplyEdgeCopy(int nSrcEdge, BOOL aChangeRegions[]);
	void InitDragDrop()
	{
		m_nDragRegion = -1;
	}
	void SetChangedRects(BOOL nChanged[]);
	void Revert();
};

#endif
