// editrgn.h : Header for the class hierarchy of 'CEditRegion'
//

#ifndef __EDITRGN_H__
#define __EDITRGN_H__

#include "borddef.h"

#include <afxtempl.h>
#include <math.h>

// defines
#define ROUND(d, p) \
	(floor((d)*p + 0.5) / p)

const int ROUND_PLACE = 10000;
const double PI = 3.14159265359;
const double RAD_90 = ROUND(PI / 2., ROUND_PLACE);
const double RAD_270 = ROUND(RAD_90 * 3., ROUND_PLACE);
const double _2PI = ROUND(2. * PI, ROUND_PLACE);
const double DTHRESH = 0.001;

#define IS_90_ROT(d) \
	(((d) > RAD_90-DTHRESH && (d) < RAD_90+DTHRESH) || ((d) > RAD_270-DTHRESH && (d) < RAD_270+DTHRESH))

// Types
struct RECTAliasT {
	LONG* left;
	LONG* top;
	LONG* right;
	LONG* bottom;

	void SetRECTAlias(LONG* nleft, LONG* ntop, LONG *nright, LONG *nbottom)
	{
		left = nleft;
		top = ntop;
		right = nright;
		bottom = nbottom;
	}
	void GetRECTAlias(CRect& rctBounds)
	{
		rctBounds.SetRect(
			*left, 
			*top,
			*right,
			*bottom);
	}
};

struct RefDataT {
	DWORD dwID;			// Graphic ID
	void* pDatabase;	// Associated database
};

enum SideTypeE {
	LEFT_SIDE, TOP_SIDE, RIGHT_SIDE, BOTTOM_SIDE
};

enum FlipTypesE {
	NO_FLIP = -1,
	H_FLIP = 0, V_FLIP, HV_FLIP
};

/////////////////////////////////////////////////////////////////////////////
// CEditRegion - base class which contains generic edit region members.

class CEditRegion
{
// Definitions
public:
	struct TransformsT {
		BOOL bXFlipped;		// TRUE when flipped over X
		BOOL bYFlipped;		// TRUE when flipped over Y
		double dAngle;		// Rotation angle in radians
	};

// Data
protected:
	DWORD m_dwID;			// Graphic ID
	void* m_pDatabase;		// Associated database
	BOOL m_bSelected;		// TRUE if currently selected
	BOOL m_bSelectBoxDrawState;	// Determines when to draw XOR selection box
	RECTAliasT m_rctBounds;	// Rectangle bounds of the region

	// Transformations
	TransformsT m_Transforms;

	// Release list
	static int m_nRefLimit;						// Max # of references in a list
	CList<RefDataT, RefDataT&> m_ReleaseList;	// List of references to be released

// Construction/destruction
public:
	CEditRegion(RECTAliasT& rctBounds, void* pDatabase);

// Attributes
public:
	static void SetRefLimit(int nRefLimit)
	{
		m_nRefLimit = nRefLimit;
	}
	DWORD GetGraphicID()
	{
		return (m_dwID);
	}
	void DuplicateGraphicID(CBECallbackInterface* pCBECallbacks, void* pDupDatabase, CEditRegion* pRegion);
	void FlipOverX()
	{
		m_Transforms.bXFlipped = !m_Transforms.bXFlipped;
	}
	void FlipOverY()
	{
		m_Transforms.bYFlipped = !m_Transforms.bYFlipped;
	}
	void SetXFlip(BOOL bFlip)
	{
		m_Transforms.bXFlipped = bFlip;
	}
	BOOL GetXFlip()
	{
		return (m_Transforms.bXFlipped);
	}
	void SetYFlip(BOOL bFlip)
	{
		m_Transforms.bYFlipped = bFlip;
	}
	BOOL GetYFlip()
	{
		return (m_Transforms.bYFlipped);
	}
	void SetRotation(double dAngle)
	{
		m_Transforms.dAngle = dAngle;
	}
	void Rotate(double dAngle);
	double GetRotation()
	{
		return (m_Transforms.dAngle);
	}
	void SetTransforms(TransformsT& transforms)
	{
		m_Transforms = transforms;
	}
	void GetTransforms(TransformsT& transforms)
	{
		transforms = m_Transforms;
	}
	BOOL SetSelection(BOOL bState, BOOL bToggle = FALSE)
	{
		if (bToggle)
		{
			m_bSelected = !m_bSelected;
		}
		else
		{
			m_bSelected = bState;
		}
		
		return (m_bSelected);
	}
	BOOL GetSelection()
	{
		return (m_bSelected);
	}
	void GetBounds(CRect& rctBounds)
	{
		m_rctBounds.GetRECTAlias(rctBounds);
	}
	void SetGraphicID(CBECallbackInterface* pCBECallbacks, DWORD dwID, BOOL bUpdateList = TRUE);
	virtual void IntelligentRotate(CBECallbackInterface* pCBECallbacks) = 0;

// Ref list operators
	void AddReleaseRef(CBECallbackInterface* pCBECallbacks, DWORD dwID, void* pDatabase);
	BOOL AdjustHeadRef(DWORD dwID);
	void AddRefHead(DWORD dwID, void* pDatabase);
	void RemoveRefList(CBECallbackInterface* pCBECallbacks);

// Implememtation
public:
	void SetDrawInfo(CBEDrawInfo* pDrawInfo);
	void RestoreDrawInfo(CBEDrawInfo* pDrawInfo);
	void RotateBounds(CBEDrawInfo* pDrawInfo, CPoint& ptOrigin);
	void ApplyLocalRotation(CBEDrawInfo* pDrawInfo);
	BOOL InSelectRegion(CPoint& point);
	void DrawSelectionBox(CDC* pDC, CPoint ptOffset, BOOL bDrawOver = TRUE);
	void MoveRectToOrigin(RECT* rct);
	virtual int DrawRegion(CBECallbackInterface* pCBECallbacks, CBEDrawInfo* pDrawInfo, CPoint* ptOrigin, BOOL bRelative = FALSE) = 0;
};

/////////////////////////////////////////////////////////////////////////////
// CCornerRegion - specifically handles corner selection regions

class CCornerRegion : public CEditRegion
{
// Construction/destruction
public:
	CCornerRegion(RECTAliasT& rctBounds, void* pDatabase);

// Attributes
public:
	virtual void IntelligentRotate(CBECallbackInterface* pCBECallbacks) {}

// Interface
public:
	virtual int DrawRegion(CBECallbackInterface* pCBECallbacks, CBEDrawInfo* pDrawInfo, CPoint* ptOrigin, BOOL bRelative = FALSE);
};

/////////////////////////////////////////////////////////////////////////////
// CEdgeRegion - specifically handles edge selection regions

class CEdgeRegion : public CEditRegion
{
// Data
	BOOL m_bForceTile;		// TRUE to force tiling to a specific number
	BOOL m_bUseLastTiles;	// TRUE to use the last tile count
	int m_nTiles;			// # of tiles to force
	int m_nTilesDrawn;		// # of tiles drawn on last execution
	int m_nMaxTiles;		// Max. # of tiles to use in intelligent redraw
	enum SideTypeE m_Side;	// side the edge is on

// Implememtation
public:
	CEdgeRegion(RECTAliasT& rctBounds, enum SideTypeE side, void* pDatabase);

// Attributes
public:
	void SetTiling(BOOL bForce, int nTiles, int nMaxTiles)
	{
		m_bForceTile = bForce;
		m_nTiles = nTiles;
		m_nMaxTiles = nMaxTiles;
	}
	void GetTiling(BOOL& bForce, int& nTiles, int& nMaxTiles)
	{
		bForce = m_bForceTile;
		nTiles = m_nTiles;
		nMaxTiles = m_nMaxTiles;
	}
	void UseLastTiles(BOOL bState)
	{
		if (!(m_bUseLastTiles = bState))
		{
			m_nTilesDrawn = -1;
		}
	}
	void SetSizing(double dSize, double aXOffsets[], double aYOffsets[]);
	void GetSizing(double& dSize, double aXOffsets[], double aYOffsets[]);
	enum SideTypeE GetSide()
	{
		return (m_Side);
	}
	virtual void IntelligentRotate(CBECallbackInterface* pCBECallbacks);

// Interface
public:
	virtual int DrawRegion(CBECallbackInterface* pCBECallbacks, CBEDrawInfo* pDrawInfo, CPoint* ptOrigin, BOOL bRelative = FALSE);
};

#endif
