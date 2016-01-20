/*	$Header: /PM8/Framework/Include/GifAnimator.h 1     3/03/99 6:15p Gbeddow $
//
//	Definition of the <c RGifReader> class.
//
//	This class provides an interface to a GIF file.  At this time it is
//	purely a wrapper for reading the transparent index from a memory buffer.
//	
//	(c)Copyright 1999 The Learning Company, Inc., all rights reserved
//
// $Log: /PM8/Framework/Include/GifAnimator.h $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 2     2/01/99 11:21a Rgrenfel
// Added a test for the supplied buffer being a GIF animation.
// 
// 1     1/25/99 3:36p Rgrenfell
// Supports GIF Animation
*/

#ifndef GIFANIMATOR_H
#define GIFANIMATOR_H

class RGifReader;

/* The GIF animator class provides a means of animating a GIF image when
	provided with a device context and the location within it to render.  It
	requires the caller to call it periodically to allow it to animate the
	frames.
*/
class FrameworkLinkage CPGIFAnimator
{
public:
	// Class constructor - Requires a buffer to the raw GIF image data.
	CPGIFAnimator( UCHAR *pData, long lBufferSize );
	// Destructor. Performs necessary cleanup.
	~CPGIFAnimator();

	// Informs the caller if enough time has passed since the last frame was
	// rendered to require a new update.
	bool NeedsUpdate();

	// Render the current frame, or if it is time for the next frame, then it
	// will be rendered instead.
	bool Update( CDC *pDC, const CRect & rcBounding );

	// Determine the size of the GIF.
	CSize GetGIFScreenSize();

	// Informs the caller whether the specified image is an animation.
	bool IsAnimation();

protected:
	// Record the current screen area into our offscreen bitmap.
	void InitializeOffscreen( CDC *pDC, const CRect & rcBounding );
	// Performs the specified erasure instructions for the current frame.
	void ClearFrame( const CRect & rcBounding );
	// Render the current frame into the offscreen bitmap.
	void RenderFrame( const CRect & rcBounding );
	// Render the offscreen bitmap onto the output DC.
	void RenderToScreen( CDC *pDC, const CRect & rcBounding );

private:
	// Stores the Gif reader which is used to process the raw data bits.
	RGifReader *m_pGifReader;

	// Stores the number of the last frame rendered.
	int m_nAnimFrame;

	// Stores how many times the animation has looped.
	int m_nLoopsRun;
	
	// Stores the last clock value of the last render completion.
	clock_t m_ctLastFrameRender;

	// An offscreen bitmap used to hold the image until we are ready to drop it
	// onto the screen.  This ensures no flicker and allows us to quickly perform
	// an update without reconstructing our state from scratch.
	CBitmap m_bmOffscreen;

	// Stores the original background for use in erasing frames.
	CBitmap m_bmBackground;

	// Stores the previous final image for use in erasure processes when specified.
	CBitmap m_bmPrevious;

	// Stores the internal scaling size for the animation.
	double m_fScalingFactor;

	// Stores whether the GIF has yet been read.
	bool m_bReadGIF;
};


#endif	// GIFANIMATOR_H