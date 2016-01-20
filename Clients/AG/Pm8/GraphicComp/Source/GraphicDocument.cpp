// ****************************************************************************
//
//  File Name:			GraphicDocument.cpp
//
//  Project:			Graphic Component
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RGraphicDocument class
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/GraphicComp/Source/GraphicDocument.cpp                    $
//   $Author:: Rgrenfel                                                       $
//     $Date:: 3/08/99 1:12p                                                  $
// $Revision:: 2                                                              $
//
// ****************************************************************************

#include	"GraphicIncludes.h"
ASSERTNAME

#include	"GraphicDocument.h"
#include "GraphicCompResource.h"

#include "StandaloneApplication.h"
#include "StandaloneDocument.h"
#include	"GraphicApplication.h"
#include	"GraphicView.h"
#include "LineView.h"
#include	"BorderView.h"
#include "EditGraphicAction.h"
#include "DrawingSurface.h"
#include "ChunkyStorage.h"
#include "ComponentTypes.h"
#include "DataTransfer.h"

// Includes for the graphic types we support
#include "Psd3SingleGraphic.h"
#include "Psd3BorderGraphic.h"
#include "Psd3BannerGraphic.h"
#include "Psd3RuledLineGraphic.h"
#include "Psd3TimepieceGraphic.h"
#include "Psd3BackdropGraphic.h"
#include "CGMSingleGraphic.h"

const YChunkTag	kTimepieceGraphicTimeChunk =	0x00A0F700;
const YChunkTag	kPropertyChunkID           =	'PROP';
const YVersion		kAddedGraphicColorVersion(kDefaultDataVersion + 1);

// ****************************************************************************
//
//  Function Name:	RGraphicDocument::RGraphicDocument()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RGraphicDocument::RGraphicDocument(RApplication* pApp, const RComponentAttributes& componentAttributes, const YComponentType& componentType, BOOLEAN fLoading)
	: RComponentDocument(pApp, componentAttributes, componentType, fLoading)
	, m_pGraphic(NULL)
	, m_rTintColor( kBlack )
	, m_fLinkedData( FALSE )
{
}

// ****************************************************************************
//
//  Function Name:	RGraphicDocument::~RGraphicDocument()
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RGraphicDocument::~RGraphicDocument()
{
	delete m_pGraphic;
}

// ****************************************************************************
//
//  Function Name:	RGraphicDocument::SetGraphic()
//
//  Description:		Set the given graphic into the component
//
//  Returns:			The Old Graphic
//
//  Exceptions:		None
//
// ****************************************************************************
RGraphic* RGraphicDocument::SetGraphic(RGraphic* pGraphic)
{
	RGraphic* pOldGraphic = m_pGraphic;
	m_pGraphic = pGraphic;

	//	Call to reset the runaround path
	static_cast<RComponentView*>(GetActiveView())->UndefineOutlinePath();

	return pOldGraphic;
}

// ****************************************************************************
//
//  Function Name:	RGraphicDocument::GetGraphic( )
//
//  Description:		Gets the the graphic for this component
//
//  Returns:			The graphic
//
//  Exceptions:		None
//
// ****************************************************************************
RGraphic* RGraphicDocument::GetGraphic( ) const
{
	return m_pGraphic;
}

// ****************************************************************************
//
//  Function Name:	RGraphicDocument::Render( )
//
//  Description:		Called to render the documents data into an output target
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RGraphicDocument::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, const RRealSize& size ) const
{	
	// Color tint the graphic
	RSolidColor rOldTintColor = drawingSurface.GetTintColor();
	drawingSurface.SetTintColor( m_rTintColor );

	// Tint the graphic
	drawingSurface.SetTint(GetTint());

	// Get the component type
	YComponentType hType = GetComponentType();
	
	RGraphic* pRenderGraphic = m_pGraphic;
	if (!pRenderGraphic) 
	{
		// Create a placeholder graphic
		YResourceId hId( 0 );
		RPsd3Graphic* pPlaceholderGraphic = NULL;
		if (hType == kSquareGraphicComponent)
		{
			pPlaceholderGraphic = new RPsd3SingleGraphic;
			hId = PLACEHOLDER_SQUARE_GRAPHIC;
		}
		else
		if (hType == kRowGraphicComponent)
		{
			pPlaceholderGraphic = new RPsd3SingleGraphic;
			hId = PLACEHOLDER_ROW_GRAPHIC;
		}
		else
		if (hType == kColumnGraphicComponent)
		{
			pPlaceholderGraphic = new RPsd3SingleGraphic;
			hId = PLACEHOLDER_COLUMN_GRAPHIC;
		}
		else
		if (hType == kHorizontalLineComponent || hType == kVerticalLineComponent)
		{							
			pPlaceholderGraphic = new RPsd3RuledLineGraphic;
			hId = PLACEHOLDER_RULED_LINE_GRAPHIC;
		}
		else
		if(hType == kBorderComponent || hType == kMiniBorderComponent)
		{				
			pPlaceholderGraphic = new RPsd3BorderGraphic;
			hId = PLACEHOLDER_BORDER_GRAPHIC;
		}
		
		if (pPlaceholderGraphic) pPlaceholderGraphic->Load(hId);
		
		pRenderGraphic = dynamic_cast<RGraphic*>(pPlaceholderGraphic);
	}

	if (pRenderGraphic)
	{
		// Set the specific properties of borders and lines
		if (hType == kHorizontalLineComponent)
		{
			RPsd3RuledLineGraphic* pRuledLineGraphic = dynamic_cast<RPsd3RuledLineGraphic*>(pRenderGraphic);	
			pRuledLineGraphic->SetHorizontal();			
		}
		else
		if (hType == kVerticalLineComponent)
		{
			RPsd3RuledLineGraphic* pRuledLineGraphic = dynamic_cast<RPsd3RuledLineGraphic*>(pRenderGraphic);	
			pRuledLineGraphic->SetVertical();
		}
		else
		if (hType == kBorderComponent || hType == kMiniBorderComponent)
		{
			RBorderView* pBorderView = static_cast<RBorderView*>(GetActiveView());
			YComponentBoundingRect rBorderExteriorBoundingRect = pBorderView->GetBoundingRect();
			YComponentBoundingRect rBorderInteriorBoundingRect = pBorderView->GetInteriorBoundingRect();
			YRealDimension nWidthBorderThickness = (rBorderExteriorBoundingRect.Width() - rBorderInteriorBoundingRect.Width()) / 2.0;
			YRealDimension nHeightBorderThickness = (rBorderExteriorBoundingRect.Height() - rBorderInteriorBoundingRect.Height()) / 2.0;

			// Get a pointer to the border graphic
			RPsd3BorderGraphic* pBorderGraphic = dynamic_cast<RPsd3BorderGraphic*>(pRenderGraphic);

			// Set the border thickness
			pBorderGraphic->SetBorderThickness(::Max(nWidthBorderThickness,nHeightBorderThickness));
		}

		// Render the graphic
		pRenderGraphic->Render( drawingSurface, size, transform, rcRender, pRenderGraphic->GetMonochromeColor( ), FALSE );

		// If we rendered a placeholder, we need to delete it
		if (!m_pGraphic) delete pRenderGraphic;
	}

	drawingSurface.SetTintColor( rOldTintColor );
}

// ****************************************************************************
//
//  Function Name:	RDocument::RenderShadow( )
//
//  Description:		Renders this document's shadow
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGraphicDocument::RenderShadow( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, const RRealSize& size, const RSolidColor& shadowColor ) const
	{
	RComponentView* pView = dynamic_cast<RComponentView*>(GetActiveView());
	if (pView && pView->HasBackgroundColor())
		{
		// if we have an active view and it has a behind color,
		// render rectangular shadow as though cast from the behind color
		RColor rOldFill = drawingSurface.GetFillColor( );
		drawingSurface.SetFillColor( shadowColor );
		drawingSurface.FillRectangle( RRealRect( size ), transform );
		drawingSurface.SetFillColor( rOldFill );
		}
	else if( m_pGraphic )
		{
		// If we have a graphic, render it; pass in the shadow color as the monochrome color and TRUE
		//	to indicate mask mode drawing.
		m_pGraphic->Render( drawingSurface, size, transform, rcRender, shadowColor, TRUE );
		}

	if (pView && pView->GetFrameType() != kNoFrame && !pView->HasBackgroundColor())
		{
		// if we have an active view and it has a frame,
		// render shadow as though cast from the frame
		pView->RenderFrame( drawingSurface, transform, shadowColor );
		}
	}

// ****************************************************************************
//
//  Function Name:	RGraphicDocument::CreateView( )
//
//  Description:		Creates a new view on this document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentView* RGraphicDocument::CreateView( const YComponentBoundingRect& boundingRect, RView* pParentView )
	{
	RComponentView* pComponentView = NULL;
	
	const YComponentType& hType = GetComponentType();
	if (hType == kHorizontalLineComponent || hType == kVerticalLineComponent)
		pComponentView = new RLineView( boundingRect, this, pParentView );
	else
	if (hType == kBorderComponent)
		pComponentView = new RBorderView (boundingRect, this, pParentView );
	else
	if (hType == kMiniBorderComponent)
		pComponentView = new RMiniBorderView( boundingRect, this, pParentView );
	else
	if (hType == kBackdropGraphicComponent)
		pComponentView = new RBackdropView( boundingRect, this, pParentView );
	else
		pComponentView = new RGraphicView( boundingRect, this, pParentView );

	return pComponentView;
	}

// ****************************************************************************
//
//  Function Name:	RGraphicDocument::Read()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RGraphicDocument::Read(RChunkStorage& rStorage, EReadReason reason)
{
	// Call the base member
	RComponentDocument::Read(rStorage, reason);

	static YChunkTag aSupportedGraphicTags[] = 
	{
		kPsd3SingleGraphicChunkID,
		kPsd3HorizontalBannerGraphicChunkID,
		kPsd3VerticalBannerGraphicChunkID,
		kPsd3RuledLineGraphicChunkID,
		kPsd3BorderGraphicChunkID,
		kPsd3TimepieceGraphicChunkID,
		kPsd3BackdropGraphicChunkID,
		kPsd3TiledBackropGraphicChunkID,
		kCGMSingleGraphicChunkID
	};

	RPsd3Graphic* pPsd3Graphic = NULL;
	RChunkStorage::RChunkSearcher rSearcher;
	for (int nIdx = 0; nIdx < sizeof(aSupportedGraphicTags) / sizeof(YChunkTag); ++nIdx)
	{
		rSearcher = rStorage.Start(aSupportedGraphicTags[nIdx], !kRecursive);
		if (!rSearcher.End())
		{
			// Create an instance of the appropriate graphic type
			switch(aSupportedGraphicTags[nIdx])
			{
			case kPsd3SingleGraphicChunkID:
				pPsd3Graphic = new RPsd3SingleGraphic;
				break;
			case kPsd3HorizontalBannerGraphicChunkID:
				pPsd3Graphic = new RPsd3HorizontalBannerGraphic;
				break;
			case kPsd3VerticalBannerGraphicChunkID:
				pPsd3Graphic = new RPsd3VerticalBannerGraphic;
				break;
			case kPsd3RuledLineGraphicChunkID:
				pPsd3Graphic = new RPsd3RuledLineGraphic;
				break;
			case kPsd3BorderGraphicChunkID:
				pPsd3Graphic = new RPsd3BorderGraphic;
				break;
			case kPsd3TimepieceGraphicChunkID:
				pPsd3Graphic = new RPsd3TimepieceGraphic;
				break;
			case kPsd3BackdropGraphicChunkID:
				pPsd3Graphic = new RPsd3BackdropGraphic;
				break;
			case kPsd3TiledBackropGraphicChunkID:
				pPsd3Graphic = new RPsd3TiledBackdropGraphic;
				break;
			case kCGMSingleGraphicChunkID:
				pPsd3Graphic = new RCGMSingleGraphic;
				break;
			default:
				TpsAssertAlways("What do you think you're doing?");
				pPsd3Graphic = NULL;
			}

			// Exit the for loop
			break;
		}
	}

	// If we found a valid graphic, read it in and select the parent chunk
	if (pPsd3Graphic)
	{
		// Call RPsd3Graphic::Read()
		pPsd3Graphic->Read(rStorage);

		// REVIEW 5/1/97 GKB	This is where we will determine whether we have a tiled backdrop or not.
		//							We will look at the graphic's size to do this.  If the graphic is square,
		//							we will assume that it is a tile.  If it is not square, we will assume that
		//							it is not tiled.  It is important to note that this may be a faulty 
		//							assumption and therefore should not be relied on.  This code should not
		//							be included in the shipping product, and as such, all projects saved prior
		//							to this date should be re-opened and converted to the proper format by
		//							re-saving.
		//
		RPsd3SingleGraphic* pSingleGraphic = dynamic_cast<RPsd3SingleGraphic*>(pPsd3Graphic);
		RPsd3BackdropGraphic* pPsd3BackdropGraphic = dynamic_cast<RPsd3BackdropGraphic*>(pPsd3Graphic);
		RPsd3TiledBackdropGraphic* pPsd3TiledBackdropGraphic = dynamic_cast<RPsd3TiledBackdropGraphic*>(pPsd3Graphic);
		if (pSingleGraphic && !pPsd3BackdropGraphic && !pPsd3TiledBackdropGraphic && (GetComponentType() == kBackdropGraphicComponent))
		{
			RPsd3Graphic* pPsd3UpdateGraphic = NULL;
			RRealSize rGraphicSize = pSingleGraphic->GetGraphicSize();
			if (::Round(rGraphicSize.m_dx) == ::Round(rGraphicSize.m_dy))
				pPsd3UpdateGraphic = new RPsd3TiledBackdropGraphic;		
			else
				pPsd3UpdateGraphic = new RPsd3BackdropGraphic;

			// Load the new graphic with the internal data of the old graphic
			pPsd3UpdateGraphic->Load(pPsd3Graphic->GetGraphicData(), pPsd3Graphic->GetGraphicDataSize(), pPsd3Graphic->GetMonochrome());

			// Delete the old graphic and change pPsd3Graphic to point to the new one
			delete pPsd3Graphic;
			pPsd3Graphic = pPsd3UpdateGraphic;
		}
		//
		// REVIEW 5/1/97 GKB	END
		
		// Set the color in the graphic
		RGraphic* pGraphic = dynamic_cast<RGraphic*>(pPsd3Graphic);

		// Read in the monochrome color of the RGraphic
		RColor rMonochromeColor;
		rStorage >> rMonochromeColor;
	
		pGraphic->SetMonochromeColor(rMonochromeColor);

		// Set the graphic in the document
		SetGraphic(pGraphic);
		rStorage.SelectParentChunk();
	}

	// Look for a Timepiece time data chunk
	rSearcher = rStorage.Start(kTimepieceGraphicTimeChunk, !kRecursive);
	if (!rSearcher.End())
	{
		RTime rTime;
		rStorage >> rTime;
		RPsd3TimepieceGraphic* pTimepieceGraphic = dynamic_cast<RPsd3TimepieceGraphic*>(m_pGraphic);
		TpsAssert(pTimepieceGraphic, "Graphic component has a Timepiece Time data chunk but no Timepiece graphic.");
		pTimepieceGraphic->SetTime(rTime);
		rStorage.SelectParentChunk();
	}

	// Search for property information	
	//
	rSearcher = rStorage.Start(kPropertyChunkID, !kRecursive);

	if (!rSearcher.End())
	{
		rStorage >> m_rTintColor;
		rStorage.SelectParentChunk();
	}	
}

// ****************************************************************************
//
//  Function Name:	RGraphicDocument::Write()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RGraphicDocument::Write(RChunkStorage& rStorage, EWriteReason reason ) const
{
	// Call the base member
	RComponentDocument::Write(rStorage, reason);

	if (m_pGraphic && !m_fLinkedData)
	{
		// Add a chunk of the type appropriate for the graphic
		RPsd3Graphic* pPsd3Graphic = dynamic_cast<RPsd3Graphic*>(m_pGraphic);
		rStorage.AddChunk(pPsd3Graphic->GetChunkStorageTag());
		{
			// Set the current chunk data version
			rStorage.GetCurrentChunkCurrentDataVersion( ) = kAddedGraphicColorVersion;

			// Call RPsd3Graphic::Write()
			pPsd3Graphic->Write(rStorage);

			// Write the monochrome color of the RGraphic
			RColor rMonochromeColor = m_pGraphic->GetMonochromeColor();
			rStorage << rMonochromeColor;
		}
		rStorage.SelectParentChunk();
	}

	// If we have a timepiece graphic, add a timepiece time chunk here
	RPsd3TimepieceGraphic* pTimepieceGraphic = dynamic_cast<RPsd3TimepieceGraphic*>(m_pGraphic);
	if (pTimepieceGraphic)
	{
		rStorage.AddChunk(kTimepieceGraphicTimeChunk);
		{
			rStorage << pTimepieceGraphic->GetTime();
		}
		rStorage.SelectParentChunk();
	}

	// Write object properties
	//
	rStorage.AddChunk( kPropertyChunkID );
	rStorage << m_rTintColor;
	rStorage.SelectParentChunk();
}

// ****************************************************************************
//
//  Function Name:	RGraphicDocument::GetDataFormatsSupportedForCopy( )
//
//  Description:		Retrieves the data formats that this document can copy to.
//							Formats should be added to the end of the collection in
//							decreasing order of priority.
//
//							Default behavior is to support picture and bitmap formats.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGraphicDocument::GetDataFormatsSupportedForCopy(YDataFormatCollection& collection) const
{	
	// Check to see if the graphic contains gradient data
	RPsd3Graphic* pPsd3Graphic = dynamic_cast<RPsd3Graphic*>(GetGraphic());
	if (pPsd3Graphic)
	{
		if (GetComponentType() == kSquareGraphicComponent)
		{
			if (pPsd3Graphic->ContainsGradientData())
				collection.InsertAtEnd(kPsd5SquareGraphicFormat);
			else
				collection.InsertAtEnd(kPsd3SquareGraphicFormat);
		}
		else if (GetComponentType() == kColumnGraphicComponent)
		{
			if (pPsd3Graphic->ContainsGradientData())
				collection.InsertAtEnd(kPsd5ColumnGraphicFormat);
			else
				collection.InsertAtEnd(kPsd3ColumnGraphicFormat);
		}
		else if (GetComponentType() == kRowGraphicComponent)
		{
			if (pPsd3Graphic->ContainsGradientData())
				collection.InsertAtEnd(kPsd5RowGraphicFormat);
			else
				collection.InsertAtEnd(kPsd3RowGraphicFormat);
		}
	}

	// Call the base member
	RComponentDocument::GetDataFormatsSupportedForCopy(collection);
}

// ****************************************************************************
//
//  Function Name:	RGraphicDocument::Copy( )
//
//  Description:		Requests data in the specified format.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RGraphicDocument::Copy(YDataFormat dataFormat, RDataTransferTarget& dataTransferTarget)
{	
	if (	dataFormat == kPsd3SquareGraphicFormat ||
			dataFormat == kPsd5SquareGraphicFormat ||
			dataFormat == kPsd3ColumnGraphicFormat ||
			dataFormat == kPsd5ColumnGraphicFormat ||
			dataFormat == kPsd3RowGraphicFormat ||
			dataFormat == kPsd5RowGraphicFormat)
	{
		RPsd3Graphic* pPsd3Graphic = dynamic_cast<RPsd3Graphic*>(GetGraphic());
		if (pPsd3Graphic)
		{			
			// Allocate the buffer to contain the graphic info array and the graphic data
			uLONG uBufferSize = sizeof(RPsd3GraphicInfo) + pPsd3Graphic->GetGraphicDataSize();
			uBYTE* pBuffer = new uBYTE[uBufferSize];

			// Fill in the graphic header
			// NOTE We use the m_Unused field to store the fAdjustLineWidth flag
			RPsd3GraphicInfo* pGraphicInfo = reinterpret_cast<RPsd3GraphicInfo*>(pBuffer);
			memset(pGraphicInfo, 0x00, sizeof(RPsd3GraphicInfo));
			pGraphicInfo->m_GraphicSize = pPsd3Graphic->GetGraphicDataSize();
			pGraphicInfo->m_Unused = pPsd3Graphic->GetAdjustLineWidth();
			pGraphicInfo->m_Mono = pPsd3Graphic->GetMonochrome();

			// Copy the graphic data
			uBYTE* pGraphicData = pBuffer + sizeof(RPsd3GraphicInfo);
			memcpy(pGraphicData, pPsd3Graphic->GetGraphicData(), pPsd3Graphic->GetGraphicDataSize());

			// Put the data up on the clipbboard
			dataTransferTarget.SetData(dataFormat, pBuffer, uBufferSize);
			
			// Clean up
			delete [] pBuffer;
		}
		
		else
		{
			TpsAssertAlways("Unsupported format requested in RGraphicDocument::Copy().");
		}
	}
	else
	{
		RComponentDocument::Copy(dataFormat, dataTransferTarget);
	}
}

// ****************************************************************************
//
//  Function Name:	RGraphicDocument::Paste( )
//
//  Description:		Retrieves data in the specified format from the clipboard
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RGraphicDocument::Paste(YDataFormat dataFormat, const RDataTransferSource& dataTransferSource)
{
	if (	dataFormat == kPsd3SquareGraphicFormat ||
			dataFormat == kPsd5SquareGraphicFormat ||
			dataFormat == kPsd3ColumnGraphicFormat ||
			dataFormat == kPsd5ColumnGraphicFormat ||
			dataFormat == kPsd3RowGraphicFormat ||
			dataFormat == kPsd5RowGraphicFormat)
	{
		TpsAssert(dataTransferSource.IsFormatAvailable(dataFormat), "A valid data format was given in RGraphicDocument::Paste(),\nbut the information is not available on the clipboard.");

		// Get the raw graphic data from the clipboard and reconstruct the graphic		
		uBYTE* pBuffer = NULL;
		YDataLength nDataLength;
		try
		{
			dataTransferSource.GetData(dataFormat, pBuffer, nDataLength);
			TpsAssert(pBuffer, "A valid data format was given in RGraphicDocument::Paste(),\nbut the information is not available on the clipboard.");

			// Use the data to initialize the component					
			RPsd3GraphicInfo* pGraphicInfo = reinterpret_cast<RPsd3GraphicInfo*>(pBuffer);
			uBYTE* pGraphicData = pBuffer + sizeof(RPsd3GraphicInfo);
			
			// m_pGraphic should be NULL, but just to b safe...
			TpsAssert(!m_pGraphic, "Deleting previous graphic.");
			delete m_pGraphic;

			RPsd3Graphic* pNewSingleGraphic = new RPsd3SingleGraphic;
			if (!pNewSingleGraphic->Load(pGraphicData, pGraphicInfo->m_GraphicSize, pGraphicInfo->m_Mono)) throw kClipboardError;
			
			// Set the graphic into the document
			RGraphic* pGraphic = dynamic_cast<RGraphic*>(pNewSingleGraphic);
			TpsAssert(pGraphic, "Bad graphic in RGraphicDocument::Paste().");
			SetGraphic(pGraphic);
			InvalidateAllViews();			

			// Clean up
			delete pBuffer;
		}
		catch(YException)
		{
			delete pBuffer;
			throw;
		}
	}
	else
	{
		TpsAssertAlways("Unsupported format given in RGraphicDocument::Paste().");
	}
}

// ****************************************************************************
//
//  Function Name:	RGraphicDocument::IsFrameable( )
//
//  Description:		Return TRUE if this document can be framed
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGraphicDocument::IsFrameable( ) const
{
	YComponentType hType = GetComponentType();
	return (hType == kHorizontalLineComponent || hType == kVerticalLineComponent || hType == kBorderComponent || hType == kMiniBorderComponent) ? FALSE : RComponentDocument::IsFrameable();
}

// ****************************************************************************
//
//  Function Name:	RGraphicDocument::SetLinkedData()
//
//  Description:		Notifies the component not to save it's data, because the
//                   data originates from a linked source. 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGraphicDocument::SetLinkedData( BOOLEAN fLinkData )
{
	m_fLinkedData = fLinkData;
}

// ****************************************************************************
//
//  Function Name:	RGraphicDocument::ResetToDefaultSize()
//
//  Description:		Resets this component to be the default size for the
//							loaded graphic
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGraphicDocument::ResetToDefaultSize( RDocument* pParentDocument )
{
	// CGM graphics fill their rectangle, so we need to have our size created
	// in an aspect correct fashion.
	if (GetComponentType() == kCGMGraphicComponent)
	{
		// Get our default size.  This should dump our graphic into a 2"x2" square.
		RRealSize sizeDefault;
		sizeDefault.m_dx = kSystemDPI * 2;
		sizeDefault.m_dy = kSystemDPI * 2;

		// Get our aspect, and set the Y component based on it.
		YFloatType fAspectRatio = GetGraphic()->GetAspectRatio();
		if (fAspectRatio < 1.0)
		{
			// X is the smaller side, so scale it down.
			sizeDefault.m_dx = sizeDefault.m_dy * fAspectRatio;	// Aspect is dx/dy.
		}
		else
		{
			// Y is the smaller side, so scale it down.
			sizeDefault.m_dy = sizeDefault.m_dx / fAspectRatio;	// Aspect is dx/dy.
		}

		// Create a bounding rect of the default object size.
		YComponentBoundingRect rectBounding( sizeDefault );

		// Get the view so we can re-center ourselves.
		RComponentView* pView = static_cast<RComponentView*>( GetActiveView( ) );

		// Offset to put it back at the old center
		rectBounding.Offset( pView->GetBoundingRect( ).GetCenterPoint( ) - rectBounding.GetCenterPoint( ) );

		// Set the new bounding rect
		pView->SetBoundingRect( rectBounding );
	}
	else
	{
		RComponentDocument::ResetToDefaultSize( pParentDocument );
	}
}


// ****************************************************************************
//
//  Function Name:	RGraphicDocument::GetDefaultSize()
//
//  Description:		Gets the default size for the loaded graphic if known.
//
//  Returns:			TRUE if the size can be calculated, FALSE if not.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGraphicDocument::GetDefaultSize( const RRealSize &sizePanel, RRealSize *psizeDefault )
{
	// CGM graphics fill their rectangle, so we need to have our size created
	// in an aspect correct fashion.
	if (GetComponentType() == kCGMGraphicComponent)
	{
		// Get our default size.  This should dump our graphic into a 2"x2" square.
		psizeDefault->m_dx = sizePanel.m_dx * .25;	// 1/4 of an 8 inch panel is 2" which is our goal size.
		psizeDefault->m_dy = psizeDefault->m_dx;	// We want to be square.

		// Get our aspect, and set the Y component based on it.
		YFloatType fAspectRatio = GetGraphic()->GetAspectRatio();
		if (fAspectRatio < 1.0)
		{
			// X is the smaller side, so scale it down.
			psizeDefault->m_dx = psizeDefault->m_dy * fAspectRatio;	// Aspect is dx/dy.
		}
		else
		{
			// Y is the smaller side, so scale it down.
			psizeDefault->m_dy = psizeDefault->m_dx / fAspectRatio;	// Aspect is dx/dy.
		}

		return TRUE;
	}
	else
	{
		// We don't know how to detect the size of a PS Graphic, so it must be
		// computed externally at this time.
		return FALSE;
	}
}


#ifdef	TPSDEBUG
// ****************************************************************************
//
//  Function Name:	RGraphicDocument::Validate( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RGraphicDocument::Validate( ) const
{
	RComponentDocument::Validate( );
	TpsAssertIsObject( RGraphicDocument, this );
}
									
#endif	//	TPSDEBUG
