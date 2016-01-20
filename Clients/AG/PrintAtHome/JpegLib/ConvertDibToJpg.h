#pragma once

#undef FAR
#define XMD_H
extern "C" {
#pragma pack(8)
#include "jpeglib.h"
#pragma pack(1)
}

bool ConvertDibToJpg(BITMAPINFOHEADER* pDIB, int nQuality, LPCSTR strFileName);
static bool DibToRGBArray(BITMAPINFOHEADER* pDIB, int nSampsPerRow, struct jpeg_compress_struct cinfo, JSAMPARRAY jsmpPixels);
