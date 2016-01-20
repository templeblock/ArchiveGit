/*	$Header: /PM8/Framework/Source/GifAnimator.cpp 1     3/03/99 6:17p Gbeddow $
//
//	Definition of the <c RGifReader> class.
//
//	This class provides an interface to a GIF file.  At this time it is
//	purely a wrapper for reading the transparent index from a memory buffer.
//	
//	(c)Copyright 1999 The Learning Company, Inc., all rights reserved
//
// $Log: /PM8/Framework/Source/GifAnimator.cpp $
// 
// 1     3/03/99 6:17p Gbeddow
// 
// 2     2/01/99 11:21a Rgrenfel
// Added a test for the supplied buffer being a GIF animation.
// 
// 1     1/25/99 3:36p Rgrenfell
// New file for support of GIF animation
*/

/*	$Header: /PM8/Framework/Source/GifAnimator.cpp 1     3/03/99 6:17p Gbeddow $
//
//	Definition of the <c RGifReader> class.
//
//	This class provides an interface to a GIF file.  At this time it is
//	purely a wrapper for reading the transparent index from a memory buffer.
//	
//	(c)Copyright 1999 The Learning Company, Inc., all rights reserved
//
// $Log: /PM8/Framework/Source/GifAnimator.cpp $
// 
// 1     3/03/99 6:17p Gbeddow
// 
// 2     2/01/99 11:21a Rgrenfel
// Added a test for the supplied buffer being a GIF animation.
// 
// 1     1/25/99 3:36p Rgrenfell
// New file for support of GIF animation
*/
#include "FrameworkIncludes.h"
#include "GifAnimator.h"
#include "GifReader.h"

/* Class constructor.  It takes a pointer to the raw data buffer and its size
	and sets itself up to be able to process the raw GIF data into an animation.
*/
CPGIFAnimator::CPGIFAnimator( UCHAR *pData, long lBufferSize )
{
	TRY
	{
		m_pGifReader = new RGifReader( pData, lBufferSize );
	}
	CATCH_ALL(e)
	{
		m_pGifReader = NULL;
	}
	END_CATCH_ALL

	m_nAnimFrame = -1;	// No frames have yet been rendered.
	m_nLoopsRun = 0;

	m_bReadGIF = FALSE;
}


/* Class Destructor.  Performs necessary cleanup.
*/
CPGIFAnimator::~CPGIFAnimator()
{
	if (m_pGifReader != NULL)
	{
		delete m_pGifReader;
	}
}


/* Determines if an update is needed.

	Returns : TRUE if an update is needed, FALSE if not.
*/
bool CPGIFAnimator::NeedsUpdate()
{
	bool bNeedUpdate = FALSE;

	if (!m_bReadGIF)
	{
		m_pGifReader->ReadGIF();
		m_bReadGIF = TRUE;
	}

	if (m_nAnimFrame < 0)
	{
		// Nothing has been rendered yet, so obviously we need an update.
		bNeedUpdate = TRUE;
	}
	else
	{
		clock_t ctNewTime = clock();

		clock_t ctDif = ctNewTime - m_ctLastFrameRender;
		if (ctDif > (clock_t)((double)CLOCKS_PER_SEC * ((double)m_pGifReader->GetDelay(m_nAnimFrame) / 100.0)) )		{
			bNeedUpdate = TRUE;
		}
	}

	return bNeedUpdate;
}

/* Renders a frame of the animation.  If sufficient time has not yet passed to
	require a new frame to be rendered, the current frame is restored.

	Returns : TRUE if there is more to the animation, FALSE if the last frame of
				 the animation has been rendered and there are no further loops.
*/
bool CPGIFAnimator::Update(
	CDC *pDC,						// The device to draw on.
	const CRect & rcBounding )	// The bounding rectangle to update.
{
	bool bContinue = TRUE;

	if (!m_bReadGIF)
	{
		m_pGifReader->ReadGIF();
		m_bReadGIF = TRUE;
	}

	// Are we rendering a new frame?
	if (NeedsUpdate())
	{
		// Yes, render our new frame into the offscreen.

		if (m_nAnimFrame < 0)
		{
			// Start off our offscreen if the animation is just beginning.
			InitializeOffscreen( pDC, rcBounding );

			// Determine the render scaling factor based on the GIF size and our
			// bounding rectangle.  We'll only do this once for the animation.
			CSize sizeGIF = m_pGifReader->GetGIFScreenSize();
			if ((sizeGIF.cx - rcBounding.Width()) > (sizeGIF.cy - rcBounding.Height()))
			{
				if (sizeGIF.cx > rcBounding.Width())
				{
					// We need to scale down, so build a scaling factor.
					m_fScalingFactor = (double)rcBounding.Width() / (double)sizeGIF.cx;
				}
				else
				{
					m_fScalingFactor = 1.0;
				}
			}
			else
			{
				if (sizeGIF.cy > rcBounding.Height())
				{
					// We need to scale down, so build a scaling factor.
					m_fScalingFactor = (double)rcBounding.Height() / (double)sizeGIF.cy;
				}
				else
				{
					m_fScalingFactor = 1.0;
				}
			}
		}
		else
		{
			// Clear the last frame so we can render the next.
			ClearFrame( rcBounding );
		}

		// Advance the frame index to the new frame.
		m_nAnimFrame++;

		if (m_nAnimFrame >= m_pGifReader->GetFrameCount())
		{
			// We have completed a loop, so increment the count of loops run.
			m_nLoopsRun++;
			int nLoopCount = m_pGifReader->GetLoopCount();
			if ((m_nLoopsRun >= nLoopCount) && (nLoopCount != 0))
			{
				// We have completed the number of loops we are supposed to run,
				// so mark that we should stop.
				bContinue = FALSE;
			}
			// Set the animation to start over at frame 0.
			m_nAnimFrame = 0;
		}

		// Update the offscreen with the new frame.
		RenderFrame( rcBounding );
	}

	// Render the offscreen bitmap onto the DC.
	RenderToScreen( pDC, rcBounding );

	return bContinue;
}


/* Create the offscreen bitmap and copy the current look of the screen.
*/
void CPGIFAnimator::InitializeOffscreen(
	CDC *pDC,						// The DC that the animation will play in.
	const CRect & rcBounding )	// The area the animation will render to.
{
	int nImageWidth = rcBounding.Width();
	int nImageHeight = rcBounding.Height();

	// Set up our working bitmaps.
	m_bmPrevious.CreateCompatibleBitmap( pDC, nImageWidth, nImageHeight );
	m_bmBackground.CreateCompatibleBitmap( pDC, nImageWidth, nImageHeight );
	m_bmOffscreen.CreateCompatibleBitmap( pDC, nImageWidth, nImageHeight );

	// Copy the contents of the DC into our background bitmap.
	CDC dcBackgroundOffscreen;
	dcBackgroundOffscreen.CreateCompatibleDC( pDC );

	// Place our bitmap into the offscreen.
	CBitmap *pOldBitmap = dcBackgroundOffscreen.SelectObject( &m_bmBackground );
	// Copy the contents of the provided DC into our bitmap.
	dcBackgroundOffscreen.BitBlt( 0, 0, nImageWidth, nImageHeight, pDC, 0, 0, SRCCOPY );

	// Initialize the offscreen from the background bitmap.
	CDC dcOffscreen;
	dcOffscreen.CreateCompatibleDC( pDC );
	CBitmap *pOldOffscreen = dcOffscreen.SelectObject( &m_bmOffscreen );
	dcOffscreen.BitBlt( 0, 0, nImageWidth, nImageHeight, &dcBackgroundOffscreen, 0, 0, SRCCOPY );
	dcOffscreen.SelectObject( pOldOffscreen );
	dcOffscreen.DeleteDC();

	// Release our background bitmap
	dcBackgroundOffscreen.SelectObject( pOldBitmap );
	// Clean up the DC we created.
	dcOffscreen.DeleteDC();
}


/* This method checks the frame's erasure instructions and performs the proper
	action based on the instructions embedded in the GIF file.
*/
void CPGIFAnimator::ClearFrame(
	const CRect & rcBounding )	// The region to draw the animation within.
{
	// Get the position of the frame to clear.
	CPoint ptFrame = m_pGifReader->GetFramePosition( m_nAnimFrame );
	CSize sizeFrame = m_pGifReader->GetFrameSize( m_nAnimFrame );

	// Scale our position values.
	ptFrame.x = (int)((double)ptFrame.x * m_fScalingFactor);
	ptFrame.y = (int)((double)ptFrame.y * m_fScalingFactor);
	sizeFrame.cx = (int)((double)sizeFrame.cx * m_fScalingFactor);
	sizeFrame.cy = (int)((double)sizeFrame.cy * m_fScalingFactor);

	EPImageClearState eClearRequest = m_pGifReader->GetFrameClearRequest( m_nAnimFrame );
	switch (eClearRequest)
	{
		case CS_ERASE_TO_BACKGROUND:
			{
				TRACE( "Erase to Background\n" );
				// Configure the offscreen devices.
				CDC dcBackground;
				CDC dcOffscreen;
				CDC dcPrevious;
				dcPrevious.CreateCompatibleDC( NULL );
				dcOffscreen.CreateCompatibleDC( NULL );
				dcBackground.CreateCompatibleDC( NULL );

				// Select in the bitmaps.
				CBitmap *pbmBackground = dcBackground.SelectObject( &m_bmBackground );
				CBitmap *pbmOffscreen = dcOffscreen.SelectObject( &m_bmOffscreen );
				CBitmap *pbmPrevious = dcPrevious.SelectObject( &m_bmPrevious );

				// Copy the entire offscreen into the previous bitmap.
				dcPrevious.BitBlt( 0, 0, rcBounding.Width(), rcBounding.Height(), &dcOffscreen, 0, 0, SRCCOPY );
				// Copy over the background in the area the frame used to cover.
				dcOffscreen.BitBlt( ptFrame.x, ptFrame.y, sizeFrame.cx, sizeFrame.cy, &dcBackground, ptFrame.x, ptFrame.y, SRCCOPY );

				// Restore the original bitmaps to avoid leaks.
				dcPrevious.SelectObject( pbmPrevious );
				dcOffscreen.SelectObject( pbmOffscreen );
				dcBackground.SelectObject( pbmBackground );

				// Delete the devices
				dcPrevious.DeleteDC();
				dcOffscreen.DeleteDC();
				dcBackground.DeleteDC();
			}
			break;
		case CS_ERASE_TO_PREV_IMAGE:
			{
				TRACE( "Erase to Previous Image\n" );
				CBitmap bmTemporary;
				// Create the necessary devices.
				CDC dcOffscreen;
				CDC dcPrevious;
				CDC dcTemporary;
				dcOffscreen.CreateCompatibleDC( NULL );
				dcPrevious.CreateCompatibleDC( NULL );
				dcTemporary.CreateCompatibleDC( NULL );

				// Create our temporary bitmap.
				bmTemporary.CreateCompatibleBitmap( &dcTemporary, rcBounding.Width(), rcBounding.Height() );

				// Select in the bitmaps.
				CBitmap *pbmOffscreen = dcOffscreen.SelectObject( &m_bmOffscreen );
				CBitmap *pbmPrevious = dcPrevious.SelectObject( &m_bmPrevious );
				CBitmap *pbmTemporary = dcTemporary.SelectObject( &bmTemporary );

				// Copy the current offscreen to the temporary.
				dcTemporary.BitBlt( 0, 0, rcBounding.Width(), rcBounding.Height(), &dcOffscreen, 0, 0, SRCCOPY );
				// Copy the previous bitmap region to the offscreen.
				dcOffscreen.BitBlt( ptFrame.x, ptFrame.y, sizeFrame.cx, sizeFrame.cy, &dcPrevious, ptFrame.x, ptFrame.y, SRCCOPY );
				// Copy the temporary bitmap into the previous bitmap.
				dcPrevious.BitBlt( 0, 0, rcBounding.Width(), rcBounding.Height(), &dcTemporary, 0, 0, SRCCOPY );

				// Restore the bitmaps.
				dcOffscreen.SelectObject( pbmOffscreen );
				dcPrevious.SelectObject( pbmPrevious );
				dcTemporary.SelectObject( pbmTemporary );

				// Delete the DCs
				dcOffscreen.DeleteDC();
				dcPrevious.DeleteDC();
				dcTemporary.DeleteDC();

				bmTemporary.DeleteObject();
			}
			break;
		case CS_LEAVE_IMAGE:
		case CS_UNSPECIFIED:
		default:
			{
				TRACE( "Clear Request : %s\n", (eClearRequest == CS_LEAVE_IMAGE) ? "Leave Image" : "Unspecified" );

				// Simply copy over the current offscreen to the previous offscreen.
				CDC dcOffscreen;
				CDC dcPrevious;
				dcOffscreen.CreateCompatibleDC( NULL );
				dcPrevious.CreateCompatibleDC( NULL );

				// Select in the bitmaps.
				CBitmap *pbmOffscreen = dcOffscreen.SelectObject( &m_bmOffscreen );
				CBitmap *pbmPrevious = dcPrevious.SelectObject( &m_bmPrevious );
				
				// Copy the entire offscreen into the previous bitmap.
				dcPrevious.BitBlt( 0, 0, rcBounding.Width(), rcBounding.Height(), &dcOffscreen, 0, 0, SRCCOPY );
				
				// Restore the bitmaps.
				dcOffscreen.SelectObject( pbmOffscreen );
				dcPrevious.SelectObject( pbmPrevious );

				// Delete the DCs
				dcOffscreen.DeleteDC();
				dcPrevious.DeleteDC();
			}
			break;
	}
}


//int gnOffsetAddon = 0;
/* This method renders the current frame into the offscreen bitmap.  We render
	to the offscreen to allow us to have it for screen refreshes and to avoid
	screen flicker problems.
*/
void CPGIFAnimator::RenderFrame(
	const CRect & rcBounding )	// The region to draw the animation within.
{
	// Get the position of the frame to clear.
	CPoint ptFrame = m_pGifReader->GetFramePosition( m_nAnimFrame );
	CSize sizeFrame = m_pGifReader->GetFrameSize( m_nAnimFrame );

	// Scale our position values.
	ptFrame.x = (int)((double)ptFrame.x * m_fScalingFactor);
	ptFrame.y = (int)((double)ptFrame.y * m_fScalingFactor);
	int nScaledX = (int)((double)sizeFrame.cx * m_fScalingFactor);
	int nScaledY = (int)((double)sizeFrame.cy * m_fScalingFactor);

	// Create our DC
	CDC dcOffscreen;
	dcOffscreen.CreateCompatibleDC( NULL );

	// Select in the bitmaps.
	CBitmap *pbmOffscreen = dcOffscreen.SelectObject( &m_bmOffscreen );

	// Copy over the background in the area the frame used to cover.
	CBitmap *pbmFrame;
	CBitmap *pbmMask;
	m_pGifReader->GetBitmaps( m_nAnimFrame, &pbmFrame, &pbmMask );

	// Create devices to hold the frame and mask bitmaps so we can blit them
	// to the device.
	CDC dcFrame;
	CDC dcMask;
	dcFrame.CreateCompatibleDC( NULL );
	dcMask.CreateCompatibleDC( NULL );

	// Put in place the palette used for the frame.
	CPalette palFrame;
	m_pGifReader->CreatePalette( m_nAnimFrame, &palFrame );
	CPalette *pOldPalette = dcOffscreen.SelectPalette( &palFrame, FALSE );
	dcOffscreen.RealizePalette();
	dcOffscreen.SelectPalette( pOldPalette, FALSE );
	pOldPalette = dcFrame.SelectPalette( &palFrame, FALSE );
	dcFrame.RealizePalette();
	dcFrame.SelectPalette( pOldPalette, FALSE );
	pOldPalette = dcMask.SelectPalette( &palFrame, FALSE );
	dcMask.RealizePalette();
	dcMask.SelectPalette( pOldPalette, FALSE );

	// Put the bitmaps into the DCs
	CBitmap *pOldFrame = dcFrame.SelectObject( pbmFrame );
	CBitmap *pOldMask = dcMask.SelectObject( pbmMask );

	// Transparently blit the new bitmap.
	dcOffscreen.StretchBlt( ptFrame.x, ptFrame.y, nScaledX, nScaledY, &dcMask, 0, 0, sizeFrame.cx, sizeFrame.cy, SRCAND );
	dcOffscreen.StretchBlt( ptFrame.x, ptFrame.y, nScaledX, nScaledY, &dcFrame, 0, 0, sizeFrame.cx, sizeFrame.cy, SRCPAINT );

	// Restore the original bitmaps to avoid leaks.
	dcOffscreen.SelectObject( pbmOffscreen );
	dcFrame.SelectObject( pOldFrame );
	dcMask.SelectObject( pOldMask );

	// Delete the devices
	dcOffscreen.DeleteDC();
	dcFrame.DeleteDC();
	dcMask.DeleteDC();

	// Record the time of render completion!
	m_ctLastFrameRender = clock();
}


/* Draws the offscreen bitmap onto the device.  Depending on whether an update
	was needed, this may still be the previous frame or the new animation frame.
*/
void CPGIFAnimator::RenderToScreen(
	CDC *pDC,						// The device to render on.
	const CRect & rcBounding )	// The region to draw the frame in.
{
	// Create our offscreen DC
	CDC dcOffscreen;
	dcOffscreen.CreateCompatibleDC( pDC );

	// Select in the bitmaps.
	CBitmap *pbmOffscreen = dcOffscreen.SelectObject( &m_bmOffscreen );

	// Copy over the background in the area the frame used to cover.
	pDC->BitBlt( 0, 0, rcBounding.Width(), rcBounding.Height(), &dcOffscreen, 0, 0, SRCCOPY );

	// Restore the original bitmaps to avoid leaks.
	dcOffscreen.SelectObject( pbmOffscreen );

	// Delete the devices
	dcOffscreen.DeleteDC();
}


/* Determines the size of the GIF animation and provides it to the caller.

	Returns : The size of the GIF animation.
*/
CSize CPGIFAnimator::GetGIFScreenSize()
{
	ASSERT( m_pGifReader != NULL );

	// Ensure we have read the GIF data before attempting to report its size.
	if (!m_bReadGIF)
	{
		m_pGifReader->ReadGIF();
		m_bReadGIF = TRUE;
	}

	return m_pGifReader->GetGIFScreenSize();
}


/* Checks the internal buffer to check if the data represents an animated GIF.
*/
bool CPGIFAnimator::IsAnimation()
{
	ASSERT( m_pGifReader != NULL );

	// Ensure we have read the GIF data before attempting to report its size.
	if (!m_bReadGIF)
	{
		m_pGifReader->ReadGIF();
		m_bReadGIF = TRUE;
	}

	// We are sure we have an animation if the frame count is greater than 1.
	return (m_pGifReader->GetFrameCount() > 1 );
}
