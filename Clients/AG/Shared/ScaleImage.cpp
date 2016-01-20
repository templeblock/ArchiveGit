#include "stdafx.h"
#include "ScaleImage.h"
#include "Filter.h"

/////////////////////////////////////////////////////////////////////////////
void ScaleImage(const BITMAPINFOHEADER* pDibSrc, const BITMAPINFOHEADER* pDibDst)
{
	CFilter::Lanczos3Filter filter;
	CFilter Filter;
	Filter.Resample(pDibSrc, pDibDst, (CFilter::CResamplingFilter&)filter);
}
