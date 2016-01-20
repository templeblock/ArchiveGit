// ****************************************************************************
//
//  File Name:			PsdClipboardViewerDialog.cpp
//
//  Project:			Renaissance Graphic Imager
//
//  Author:				G. Brown
//
//  Description:		Definition of the CPsdClipboardViewerDialog class
//
//  Portability:		MFC dependent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/GraphicComp/Source/PsdClipboardViewerDialog.cpp           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"GraphicIncludes.h"
ASSERTNAME

#include "PsdClipboardViewerDialog.h"
#include "GraphicCompResource.h"

#include "GraphicApplication.h"
#include "Psd3SingleGraphic.h"
#include "DcDrawingSurface.h"

/////////////////////////////////////////////////////////////////////////////
// CPsdClipboardViewerDialog dialog


CPsdClipboardViewerDialog::CPsdClipboardViewerDialog(CWnd* pParent)
: CDialog(DIALOG_PSD_CLIPBOARD_VIEWER, pParent)
{
	m_pPsd3Graphic = NULL;
}

CPsdClipboardViewerDialog::~CPsdClipboardViewerDialog()
{
	delete m_pPsd3Graphic;
}

BOOL CPsdClipboardViewerDialog::OnInitDialog()
{
	// Call the base method
	CDialog::OnInitDialog();

	// Initialize the radio buttons
	m_cRadioPsd3Graphic.SetCheck(0);
	m_cRadioPsd5Graphic.SetCheck(0);

	delete m_pPsd3Graphic;
	m_pPsd3Graphic = GetGraphicFromClipboard();
	if (m_pPsd3Graphic)
	{
		if (m_pPsd3Graphic->ContainsGradientData())
			m_cRadioPsd5Graphic.SetCheck(1);
		else
			m_cRadioPsd3Graphic.SetCheck(1);
	}
	
	return TRUE;
}

void CPsdClipboardViewerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, CONTROL_STATIC_GRAPHIC_PREVIEW, m_cGraphicPreview);
	DDX_Control(pDX, CONTROL_RADIO_PSD3_GRAPHIC, m_cRadioPsd3Graphic);
	DDX_Control(pDX, CONTROL_RADIO_PSD5_GRAPHIC, m_cRadioPsd5Graphic);
}

RPsd3Graphic* CPsdClipboardViewerDialog::GetGraphicFromClipboard()
{	
	RPsd3Graphic* pPsd3Graphic = new RPsd3SingleGraphic;
	HGLOBAL hClipboard = NULL;
	try
	{
		// Open the clipboard
		if (!::OpenClipboard(m_hWnd)) throw kClipboardError;

		// Get the data from the clipboard
		if (IsClipboardFormatAvailable(kPsd3SquareGraphicFormat))
		{			
			hClipboard = ::GetClipboardData(kPsd3SquareGraphicFormat);
			
		}
		else if (IsClipboardFormatAvailable(kPsd3ColumnGraphicFormat))
		{
			hClipboard = ::GetClipboardData(kPsd3ColumnGraphicFormat);
			m_cRadioPsd3Graphic.SetCheck(1);
		}
		else if (IsClipboardFormatAvailable(kPsd3RowGraphicFormat))
		{
			hClipboard = ::GetClipboardData(kPsd3RowGraphicFormat);
			m_cRadioPsd3Graphic.SetCheck(1);
		}
		else if (IsClipboardFormatAvailable(kPsd5SquareGraphicFormat))
		{
			hClipboard = ::GetClipboardData(kPsd5SquareGraphicFormat);
			m_cRadioPsd5Graphic.SetCheck(1);
		}
		else if (IsClipboardFormatAvailable(kPsd5ColumnGraphicFormat))
		{
			hClipboard = ::GetClipboardData(kPsd5ColumnGraphicFormat);
			m_cRadioPsd5Graphic.SetCheck(1);
		}
		else if (IsClipboardFormatAvailable(kPsd5RowGraphicFormat))
		{
			hClipboard = ::GetClipboardData(kPsd5RowGraphicFormat);
			m_cRadioPsd5Graphic.SetCheck(1);
		}
		else 
		{
			throw ::kDataFormatInvalid;
		}

		// Get a pointer to the clipboard data		
		void* pClipboardData = ::GlobalLock(hClipboard);

		RPsd3GraphicInfo* pGraphicInfo = reinterpret_cast<RPsd3GraphicInfo*>(pClipboardData);
		uBYTE* pGraphicData = reinterpret_cast<uBYTE*>(pClipboardData) + sizeof(RPsd3GraphicInfo);

		// Load the graphic
		if (!pPsd3Graphic->Load(pGraphicData, pGraphicInfo->m_GraphicSize, pGraphicInfo->m_Mono)) throw ::kDataFormatInvalid;

		// Unlock the clipboard's global memory
		if (hClipboard) ::GlobalUnlock(hClipboard);
		
		// Close the clipboard		
		::CloseClipboard();
	}
	catch(YException)
	{
		// Clean up
		delete m_pPsd3Graphic;
		m_pPsd3Graphic = NULL;
		if (hClipboard) ::GlobalUnlock(hClipboard);
		::CloseClipboard();
		
		return NULL;
	}

	return pPsd3Graphic;
}

BEGIN_MESSAGE_MAP(CPsdClipboardViewerDialog, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPsdClipboardViewerDialog message handlers
void CPsdClipboardViewerDialog::OnPaint()
{
	CPaintDC cPaintDC(this); // device context for painting

	// Get the client rect of the preview window
	CRect cPreviewRect;
	m_cGraphicPreview.GetClientRect(&cPreviewRect);
	cPreviewRect.DeflateRect(4, 4);

	// Get a pointer to the RSingleGraphic
	RSingleGraphic* pSingleGraphic = dynamic_cast<RSingleGraphic*>(m_pPsd3Graphic);
	if (pSingleGraphic)
	{
		// Initialize a drawing surface to render the graphic into
		RDcDrawingSurface rDS;
		if (rDS.Initialize(cPaintDC.m_ps.hdc, cPaintDC.m_ps.hdc))
		{
			RRealSize rGraphicSize = pSingleGraphic->GetGraphicSize();
			
			// Map the graphic's size to the size of the preview window
			R2dTransform rTransform;
			rTransform.PreScale((YRealDimension)cPreviewRect.Width() / rGraphicSize.m_dx,
				(YRealDimension)cPreviewRect.Height() / rGraphicSize.m_dy);
			rTransform.PostTranslate(cPreviewRect.left, cPreviewRect.top);
			
			RIntRect rRender(cPreviewRect.left, cPreviewRect.top, cPreviewRect.right, cPreviewRect.bottom);
			pSingleGraphic->Render(rDS, rGraphicSize, rTransform, rRender);
		}
	}
	
	// Do not call CDialog ::OnPaint() for painting messages
}
