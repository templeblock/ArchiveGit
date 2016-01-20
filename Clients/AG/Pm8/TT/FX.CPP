#include "fscdefs.h"
#include "fontmath.h"
#include "fnt.h"
#include "sc.h"
#include "effects.h"
#include "fsglue.h"

#include <math.h>

extern int MulDiv(int A, int B, int C);

FX Fx;

static uint32 fx_process_black_pattern(uint32 m, int32 x, int32 y);
static uint32 fx_process_white_pattern(uint32 m, int32 x, int32 y);
static uint32 fx_process_function_pattern(uint32 m, int32 x, int32 y);
static uint32 fx_process_bitmap_pattern(uint32 m, int32 x, int32 y);

static void get_contour_offset(int32 h);
extern void get_shadow_offset(int32 h, int32 w);
extern void rotate_pen(OutlineDescriptor* pPen);

// Macros for masking bits with "consecutive" coordinates within a 32-bit number.
#ifndef FSCFG_BIG_ENDIAN

static uint32 aulPixMask[32] =
{
  0x00000080L, 0x00000040L, 0x00000020L, 0x00000010L,
  0x00000008L, 0x00000004L, 0x00000002L, 0x00000001L,
  0x00008000L, 0x00004000L, 0x00002000L, 0x00001000L,
  0x00000800L, 0x00000400L, 0x00000200L, 0x00000100L,
  0x00800000L, 0x00400000L, 0x00200000L, 0x00100000L,
  0x00080000L, 0x00040000L, 0x00020000L, 0x00010000L,
  0x80000000L, 0x40000000L, 0x20000000L, 0x10000000L,
  0x08000000L, 0x04000000L, 0x02000000L, 0x01000000L
};

// PC
#define PIXEL_MASK(x)	(aulPixMask[(x)])

#else

// Mac
#define PIXEL_MASK(x)	(((uint32)0x80000000L)>>(x))

#endif


//////////////////////////////////////////////////////////////////////////
//
// set_pattern
//
//		Initialize the variables used for filling with a pattern.
//
// Entry:
//
//		pattern = pattern number
//
// Exit:
//
//    Effects variables initialized:
//
//		fx_pattern_processor
//
//			The near address of the function used to retrieve
//			pattern data.
//
//		fx_pattern_descriptor
//
//			The pattern header. If the pattern has an initialization
//			function, then it is called to further initialize this data.
//
//		fx_pattern_data
//
//			The near address of the bitmap data for a bitmap pattern.
//

void set_pattern(int16 pattern)
{
	Fx.fx_pattern_processor = NULL;

	Fx.fx_pattern_descriptor.nXSize = 0;
	Fx.fx_pattern_descriptor.nYSize = 0;
	Fx.fx_pattern_descriptor.nXOffset = 0;
	Fx.fx_pattern_descriptor.nYOffset = 0;

	Pattern* pPattern = NULL;

	// Get the address of the pattern descriptor.

	if (pattern != FillFormat::None)
	{
		// Get descriptor address from pattern number.

		extern Pattern* Patterns[];
		pPattern = Patterns[pattern];

		if (pPattern != (Pattern*)NO_PATTERN)
		{
			if (pPattern == (Pattern*)BLACK_PATTERN)
			{
				// Special all-black pattern.
				Fx.fx_pattern_processor = fx_process_black_pattern;
			}
			else if (pPattern == (Pattern*)WHITE_PATTERN)
			{
				// Special all-white pattern.
				Fx.fx_pattern_processor = fx_process_white_pattern;
			}
			else
			{
				// We have a real pattern descriptor, copy it to the context so it
				// can be changed if needed.

				Fx.fx_pattern_descriptor = pPattern->Descriptor;

				// Save the data address.
				Fx.fx_pattern_data = &(pPattern->bData[0]);

				// Check if we have a bitmap or a function pattern.
				if (Fx.fx_pattern_descriptor.pFunction != NULL)
				{
					// Use function pattern processor.
					Fx.fx_pattern_processor = fx_process_function_pattern;
				}
				else
				{
					// Use bitmap pattern processor.
					Fx.fx_pattern_processor = fx_process_bitmap_pattern;
				}

				// Call the initialization function if there is one.
				if (Fx.fx_pattern_descriptor.pInitialize != NULL)
				{
					(*(Fx.fx_pattern_descriptor.pInitialize))();
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//
// set_pen
//
//		Initialize the variables used for stroking with a pen.
//
// Entry:
//
//		pen = pen number
//
// Exit:
//
//    Effects variables initialized:
//
//		fx_pen_descriptor
//
//			The address of the pen descriptor.
//

void set_pen(int16 pen)
{
	extern Outline* Pens[];
	
	// Get the address of the pen descriptor
	
	switch (pen)
	{
		case ((int16)MLS_OUTLINE_no_pen):
		{
			Fx.fx_pen_descriptor = NULL;
			break;
		}
		case ((int16)MLS_OUTLINE_contour_outline_pen):
		{
			Fx.fx_pen_descriptor = &Fx.fx_contour_outline_pen;
			break;
		}
		case ((int16)MLS_OUTLINE_contour_gap_pen):
		{
			Fx.fx_pen_descriptor = &Fx.fx_contour_gap_pen;
			break;
		}
		case ((int16)MLS_OUTLINE_shadow_pen):
		{
			Fx.fx_pen_descriptor = &Fx.fx_shadow_pen;
			break;
		}
	
		default:
		{
			if (Pens[pen] == NULL)
			{
				Fx.fx_pen_descriptor = NULL;
			}
			else
			{
				if (Pens[pen]->Descriptor.nType == OUTLINE_diagonal)
				{
					// Rotate diagonal pens.
					Fx.fx_diagonal_pen = Pens[pen]->Descriptor;
					Fx.fx_pen_descriptor = &Fx.fx_diagonal_pen;
					rotate_pen(Fx.fx_pen_descriptor);
				}
				else
				{
					Fx.fx_pen_descriptor = &Pens[pen]->Descriptor;
				}
			}
			break;
		}
	}

}

//////////////////////////////////////////////////////////////////////////
//
// make_contour_pens
//
//		Initialize the two pens used to make contour outlines:
//
//			contour_outline_pen
//
//				This pen is stroked in black to form the outline
//				portion of the contour.
//
//			contour_gap_pen
//
//				This pen is stroked in white to form the gap
//				between the character body and the outline.
//
//		Both pens are type OUTLINE_rectangle and their dimensions
//		depend on the point size of the font is device dots (X and Y
//		pixels per Em), and the ratio of the X and Y device resolutions.
//
// Exit:
//
//		Effects variables initialized:
//
//			fx_contour_gap_pen
//
//				The pen descriptor for the contour gap pen.
//
//			fx_contour_outline_pen
//
//				The pen descriptor for the contour outline pen.
//

void make_contour_pens(void)
{
	// Compute the contour outline dimensions
	
	get_contour_offset(Fx.fx_y_pixels_per_em);
	
	// Set the X and Y sizes of the outline pen
	
	Fx.fx_contour_outline_pen.nXOffset =   Fx.fx_x_offset;
	Fx.fx_contour_outline_pen.nXSize   = 2*Fx.fx_x_offset + 1;
	
	Fx.fx_contour_outline_pen.nYOffset =   Fx.fx_y_offset;
	Fx.fx_contour_outline_pen.nYSize   = 2*Fx.fx_y_offset + 1;
	
	// Now compute the contour gap sizes. These are fractions of the 
	// contour outline sizes defined by the ratio:
	//
	//      CONTOUR_GAP_NUMERATOR/CONTOUR_GAP_DENOMINATOR
	
	// Compute gap pen X sizes
	
	long x;
	
	x = LongMulDiv(Fx.fx_contour_outline_pen.nXOffset, CONTOUR_GAP_NUMERATOR, CONTOUR_GAP_DENOMINATOR);
	if (x == Fx.fx_contour_outline_pen.nXOffset)
	{
		--x;
	}
	
	if (x == 0)
	{
		x = 1;
	}
	
	Fx.fx_contour_gap_pen.nXOffset =   x;
	Fx.fx_contour_gap_pen.nXSize   = 2*x + 1;
	
	// Compute gap pen Y sizes
	
	x = LongMulDiv(Fx.fx_contour_outline_pen.nYOffset, CONTOUR_GAP_NUMERATOR, CONTOUR_GAP_DENOMINATOR);
	if (x == Fx.fx_contour_outline_pen.nYOffset)
	{
		--x;
	}
	
	if (x == 0)
	{
		x = 1;
	}
	
	Fx.fx_contour_gap_pen.nYOffset =   x;
	Fx.fx_contour_gap_pen.nYSize   = 2*x + 1;
	
	// Set the type fields of all the pens to OUTLINE_rectangle.
	
	Fx.fx_contour_gap_pen.nType     = OUTLINE_rectangle;
	Fx.fx_contour_outline_pen.nType = OUTLINE_rectangle;
}

//////////////////////////////////////////////////////////////////////////
//
// get_contour_offset
//
//		Compute the maximum X and Y offsets for a contour pen given a font
//		height, X device resolution, and Y device resolution.
//
// Entry:
//
//		              h = dot height of font (passed on stack)
//		fx_x_resolution = X device resolution (passed through context)
//		fx_y_resolution = Y device resolution (passed through context)
//
// Exit:
//
//		fx_x_offset = maximum X offset
//		fx_y_offset = maximum y offset
//

void get_contour_offset(int32 h)
{
	// Compute the contour outline dimension. This is the maximum distance
	// of the contour outline from the character. This is computed as a
	// linear function of the point size of the font in device dots:
	// 
	// 	contour_outline_offset = ((D+B-A)/B)+2
	// 	     
	// 	where
	// 	     
	// 	    D is the font height expressed in device dots -- this
	// 	            is the value in fx_y_pixels_per_em
	// 	    A is the value CONTOUR_SCALE_ORIGIN -- this is dot
	// 	            height at which the contour outline offset
	// 	            jumps from its minimum value of 2 to 3
	// 	    B is the value CONTOUR_SCALE_SLOPE -- this is the
	// 	            number of additional dots required to increment
	// 	            the contour outline offset
	
	int32   yOffset;
	
	yOffset = h + CONTOUR_SCALE_SLOPE - CONTOUR_SCALE_ORIGIN;
	
	if (yOffset <= 0)
	{
	       // The dot height is too small. Force contour outline 
	       // offset to minimum distance of two dots:
	
	       yOffset = 2;
	}
	else
	{
	       // Scale the adjusted dot height to get the contour 
	       // outline offset:
	
	       yOffset = yOffset / CONTOUR_SCALE_SLOPE + 2;
	}
	
	// We now have the contour outline Y offset:  Put it in the 
	// Fx structure:
	
	Fx.fx_y_offset = yOffset;
	
	// We have now computed the contour outline offset based on the 
	// height of the font. We now need to compute the corresponding 
	// offset in the horizontal direction. These may differ because 
	// the X and Y resolutions of the coordinate system may be 
	// different. To compute the horizontal offset we scale the 
	// vertical offset by the ratio:
	//
	//      device X resolution/device Y resolution
	
	Fx.fx_x_offset = LongMulDiv(yOffset, Fx.fx_x_resolution, Fx.fx_y_resolution);

}

//////////////////////////////////////////////////////////////////////////
//
// make_shadow_pens
//
//		Initialize the pen used to make shadow outlines. This pen will
//		have the type OUTLINE_diagonal and be a diagonal line whose
//		direction is determined by the shadow type (DR, DL, UR, or UL),
//		and whose size is determined by the font cell size and the ratio
//		of the device X and Y resolutions.
//
// Exit:
//
//		Effects variables initialized:
//
//			fx_shadow_pen
//
//				The pen descriptor for the shadow pen.
//

void make_shadow_pens(void)
{
	int32 x_offset;
	int32 y_offset;
	
	// Get the shadow offsets:
	
	get_shadow_offset(Fx.fx_em_cell_height, Fx.fx_em_cell_width);
	
	// Adjust the shadow offsets by the offsets of the current pen.
	
	x_offset = Fx.fx_x_offset;
	y_offset = Fx.fx_y_offset;

	if (Fx.fx_pen_descriptor != NULL)
	{
		// The offset vector should be parallel with the shadow vector.
		// So, we need to get its angle and find the corresponding point
		// on the ellipse formed by the X and Y pen sizes.
		if ((x_offset != 0) || (y_offset != 0))
		{
			double dX = (double)x_offset;
			double dY = (double)y_offset;
			double dR = sqrt(dX*dX+dY*dY);
			double dCos = dX/dR;
			double dSin = dY/dR;

			if (dCos > 0.0)
			{
				x_offset += (int32)floor(dCos*((double)(Fx.fx_pen_descriptor->nXSize-Fx.fx_pen_descriptor->nXOffset-1))+0.5);
			}
			else
			{
				x_offset += (int32)floor(dCos*((double)(Fx.fx_pen_descriptor->nXOffset))+0.5);
			}
			if (dSin > 0.0)
			{
				y_offset += (int32)floor(dSin*((double)(Fx.fx_pen_descriptor->nYSize-Fx.fx_pen_descriptor->nYOffset-1))+0.5);
			}
			else
			{
				y_offset += (int32)floor(dSin*((double)(Fx.fx_pen_descriptor->nYOffset))+0.5);
			}
		}
	}
	
	// Fill in the pen descriptors:
	
	switch (Fx.fx_Shadow.m_nStyle)
	{
//		case ShadowFormat::Hairline:
		case ShadowFormat::Drop:
		{
			// Make hairline shadow pen.
			Fx.fx_shadow_pen.nType    = OUTLINE_rectangle;
			Fx.fx_shadow_pen.nXSize   = 1;
			Fx.fx_shadow_pen.nYSize   = 1;
			Fx.fx_shadow_pen.nXOffset = -x_offset;
			Fx.fx_shadow_pen.nYOffset = -y_offset;
			break;
		}
		case ShadowFormat::Extruded:
		{
			// Make the extruded shadow pen.
			Fx.fx_shadow_pen.nType    = OUTLINE_diagonal;
			Fx.fx_shadow_pen.nXSize   = x_offset;
			Fx.fx_shadow_pen.nYSize   = y_offset;
			Fx.fx_shadow_pen.nXOffset = 0;
			Fx.fx_shadow_pen.nYOffset = 0;
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//
// compute_eae_values
//
//		Compute the extended attribute escapements for the current character.
//
// Exit:
//
//    Effects variables initialized:
//
//		fx_eae (BBOX)
//
//			The extended attribute escapement (EAE) values.
//
//		fx_eae_width
//
//			The contribution that extended attribute escapements make
//			to a horizontal width metric such as an advance width or
//			space width. This value include the "minimum" contributions
//			from Contour 1 and Contour 2 pens.
//

void compute_eae_values(fsg_SplineKey* key)
{
	extern Outline* Pens[];

	// Clear out any existing extended attribute escapements.

	Fx.fx_eae.xMin = 0;
	Fx.fx_eae.yMin = 0;
	Fx.fx_eae.xMax = 0;
	Fx.fx_eae.yMax = 0;

	Fx.fx_eae_width = 0;

	OutlineDescriptor* pOD;
	int32 x_offset;
	int32 y_offset;
	int32 size;
	int32 adjust;
	int32 minValue;

	// Compute the extended attribute escapements due to pens

	if ((Fx.fx_Outline.m_nStyle != OutlineFormat::None)
	 && (Fx.fx_Outline.m_nStyle != OutlineFormat::Solid))
	{
		// Check for a static pen or a contour pen

		if (Fx.fx_Outline.m_nStyle < OutlineFormat::Contour1)
		{
			// The pen is a static pen. In this case, we use 
			// the X and Y sizes of the pen to determine the 
			// EAE values. Since the pen is static, these EAE 
			// values apply to both the dot and Em values. 
			// Furthermore, static pen X EAE's also contribute 
			// to the dot and Em "width" EAE's.

			pOD = &Pens[Fx.fx_Outline.m_nStyle]->Descriptor;

			// Do X:
				
			x_offset = pOD->nXOffset;
			size     = pOD->nXSize;

			// Do xMax
			
			adjust = - x_offset;
			
			if (size > 0)
			{
				adjust += size;
			}

			--adjust;

			if (adjust > 0)
			{
				Fx.fx_eae.xMax  += adjust;
				Fx.fx_eae_width += adjust;
			}

			// Do xMin

			adjust = x_offset;

			if (size < 0)
			{
				adjust -= size;
			}

			if (adjust > 0)
			{
				Fx.fx_eae.xMin  += adjust;
				Fx.fx_eae_width += adjust;
			}

			// Do Y:
			
			y_offset = pOD->nYOffset;
			size     = pOD->nYSize;

			// Do yMax
			
			adjust = y_offset;

			if (size < 0)
			{
				adjust -= size;
			}

			if (adjust > 0)
			{
				Fx.fx_eae.yMax += adjust;
			}

			// Do yMin
			
			adjust = - y_offset;
			
			if (size > 0)
			{
				adjust += size;
			}
			
			--adjust;
			
			if (adjust >= 0)
			{
				Fx.fx_eae.yMin += adjust;
			}
		}
		else
		{
			// The pen is a contour pen. These pens are 
			// scaled in size along with the point size. This 
			// means the dot and Em EAE's will be different. 
			// Contour pens contribute a "minimum value" 
			// to the "width" EAE's.
			
			// First, get the dot EAE values:
			
			get_contour_offset(Fx.fx_y_pixels_per_em);
	
			x_offset = Fx.fx_x_offset;
			y_offset = Fx.fx_y_offset;
			minValue = 4;
	
			// If there is a 3D shadow, then use the contour 
			// gap offset instead of the contour outline 
			// offset.
	
			if (Fx.fx_Shadow.m_nStyle == ShadowFormat::Extruded)
			{
				x_offset = LongMulDiv(x_offset, CONTOUR_GAP_NUMERATOR, CONTOUR_GAP_DENOMINATOR);
				y_offset = LongMulDiv(y_offset, CONTOUR_GAP_NUMERATOR, CONTOUR_GAP_DENOMINATOR);
				minValue = 2;
			}
	
			Fx.fx_eae_width += minValue;
			
			Fx.fx_eae.xMin += x_offset;
			Fx.fx_eae.xMax += x_offset;
			Fx.fx_eae.yMin += y_offset;
			Fx.fx_eae.yMax += y_offset;
		}
	}

	// Add in the extended attribute escapements due to shadows:
	
	if (Fx.fx_Shadow.m_nStyle != ShadowFormat::None)
	{
		// Compute the dot EAE values for the shadow:
	
		get_shadow_offset(Fx.fx_em_cell_height, Fx.fx_em_cell_width);
	
		x_offset = Fx.fx_x_offset;
		y_offset = Fx.fx_y_offset;
	
		if (x_offset < 0)
		{
			Fx.fx_eae.xMin  -= x_offset;
			Fx.fx_eae_width -= x_offset;
		}
		else
		{
			Fx.fx_eae.xMax  += x_offset;
			Fx.fx_eae_width += x_offset;
		}
	
		if (y_offset < 0)
		{
			Fx.fx_eae.yMax  -= y_offset;
		}
		else
		{
			Fx.fx_eae.yMin  += y_offset;
		}
	}
}

uint32 fx_process_black_pattern(uint32 m, int32 x, int32 y)
{
	return m;
}

uint32 fx_process_white_pattern(uint32 m, int32 x, int32 y)
{
	return 0;
}

uint32 fx_process_function_pattern(uint32 m, int32 x, int32 y)
{
	uint32 dwPattern = 0;

	CPatternCoordinates PatternCoordinates(x, y);

	for (int nBit = 0; nBit < 32; nBit++)
	{
		// Get the current coordinates.
		PatternCoordinates.GetXY(Fx.fx_fill_x, Fx.fx_fill_y);

		// Get the mask for the current pixel.
		uint32 dwFillMask = PIXEL_MASK(nBit);

		// Check if we need to compute this bit.
		if ((m & dwFillMask) != 0)
		{
			// Get the bit from the pattern function.
			if (Fx.fx_pattern_descriptor.pFunction())
			{
				dwPattern |= dwFillMask;
			}
		}
	}
	
	return dwPattern;
}

uint32 fx_process_bitmap_pattern(uint32 m, int32 x, int32 y)
{
	uint32          fill_count;
	unsigned char*  row_address;
	uint32          pattern_count;
	uint32          fill_buffer;
	
	unsigned char*  pFillBuffer;    // Pointer to fill_buffer
	unsigned char*  pPatternByte;   // Point to a pattern byte
	int32           t;              // Temp working variable
	unsigned char   fillByte;       // Output fill byte built here
	int32           bitIndexInFill; // Bit index of bit in fill run
	int32           bitIndexInPat;  // Bit index of bit in pattern
	int32           numBitsToFill;
	int32           numBitsToCopy;
	uint16          shiftWord;      // Two bytes placed here for shifting
	
	fill_count = 32;                // Always build a 32 bit chunk
	
	pFillBuffer = (unsigned char *)&fill_buffer;

	// Create coordinate generator.
	CPatternCoordinates PatternCoordinates(x, y);

	// Get starting coordinates.
	PatternCoordinates.GetXY(x, y);

	if (PatternCoordinates.IsRotatedOrExpanded())
	{
		// Rotate pattern data. In this case, we march through the pattern data and
		// extract each bit individually.

		fill_buffer = 0;

		for (uint32 nBit = 0; nBit < 32; nBit++)
		{
			// Get the mask for this bit.
			uint32 dwFillMask = PIXEL_MASK(nBit);

			// Check if we need to compute this bit.
			if ((m & dwFillMask) != 0)
			{
				// Get X and Y of pattern data.
				int32 nPatternX = x % Fx.fx_pattern_descriptor.nXSize;
				if (nPatternX < 0)
				{
					nPatternX += Fx.fx_pattern_descriptor.nXSize;
				}
				int32 nPatternY = y % Fx.fx_pattern_descriptor.nYSize;
				if (nPatternY < 0)
				{
					nPatternY += Fx.fx_pattern_descriptor.nYSize;
				}

				// Get the bit out of the pattern data.
				unsigned char *pPatternByte = Fx.fx_pattern_data + nPatternY*((Fx.fx_pattern_descriptor.nXSize+7)/8) + (nPatternX/8);
				if ((*pPatternByte & (0x80 >> (nPatternX % 8))) != 0)
				{
					fill_buffer |= dwFillMask;
				}
			}

			// Get next coordinates.
			PatternCoordinates.GetXY(x, y);
		}
	}
	else
	{
		// No rotation.
		PatternCoordinates.GetXY(x, y);

		// Compute the address of the row containing the pattern data.
		
		t = y % Fx.fx_pattern_descriptor.nYSize;
		if (t < 0)
		{
			t += Fx.fx_pattern_descriptor.nYSize;
		}
		
		row_address = Fx.fx_pattern_data + t*((Fx.fx_pattern_descriptor.nXSize+7)/8);

		// Compute the bit index of the first bit in the fill run. It is 
		// always zero because we are filling an entire 32 bit chunk.
		
		bitIndexInFill = 0;
		
		// Compute the bit offset from the start of the pattern row data 
		// to the bit to place in the first bit of the run.
		
		t = x % Fx.fx_pattern_descriptor.nXSize;
		if (t < 0)
		{
			t += Fx.fx_pattern_descriptor.nXSize;
		}
		
		// Compute the number of bits available in the pattern row starting 
		// at the first bit we have decided to use.
		
		pattern_count = Fx.fx_pattern_descriptor.nXSize - t;
		
		// Compute the address of the byte containing the first bit to use 
		// in the pattern.
		
		pPatternByte = row_address + (t/8);

		// Compute the bit index of the first bit being used from the 
		// pattern.
		
		bitIndexInPat = t % 8;
		
		// This is the top of the main loop which collects portions of the 
		// pattern data into bytes and stores these bytes in the fill run.
		//
		// We are certain at this point that there is at least one more 
		// bit to fill in the fill run.

		do
		{
			// Compute how many of the bits in the fill run we want to 
			// fill with the current byte. This will be the minimum of 
			// the number of bits that will fit in the current byte and 
			// the number of bits that are left to be filled in the 
			// fill run.

			numBitsToFill = 8 - bitIndexInFill;
			
			if (numBitsToFill > (long)fill_count)
			{
				numBitsToFill = fill_count;
			}
			
			// We have the number of bits we are going to fill in the 
			// current byte.  Adjust the fill run length by the same 
			// amount.
			
			fill_count -= numBitsToFill;
			fillByte = 0;           // Collect the byte in fillByte
			
			// This is the top of the inner loop which copies data 
			// from the pattern row until it contains the number of 
			// bits computed above.

			do
			{
				// Compute how many bits we should copy from the 
				// current pattern row into the fill byte which is 
				// being built. This will be the minimum of the 
				// number which are available from the fill row 
				// and the number needed to complete the fill byte.

				numBitsToCopy = numBitsToFill;

				if (numBitsToCopy > (long)pattern_count)
				{
					numBitsToCopy = pattern_count;
				}

				// Move the data from the pattern to the fill 
				// byte. Since we are going to have to shift the 
				// data, we load a whole word so that bits from 
				// the next byte can be shifted into the first 
				// byte. We then shift the data left so that the
				// first bit we are going to use is in the high
				// bit. This gets rid of any one bits to the left 
				// of the first bit we are going to use so we can 
				// OR the final byte into the partially collected 
				// fill byte without disturbing previously 
				// collected bits.
				
				shiftWord = MAKEWORD(*(pPatternByte+1), *pPatternByte) << bitIndexInPat;
				
				
				// Now we shift the pattern data to the correct 
				// position for ORing with the current collected 
				// byte.  And we OR the pattern data into the data
				// already collected in the fill byte.

				fillByte |= (HIBYTE(shiftWord) >> bitIndexInFill);
				
				// Advance the index of the next free bit in the 
				// fill byte being collected by the number of bits 
				// which were just copied to it.
				
				bitIndexInFill += numBitsToCopy;
				bitIndexInFill &= 0x7;
				
				// Advance the index of the next unused bit in the 
				// pattern data.  If the new index moves to another 
				// byte, advance the pattern data address.
				
				bitIndexInPat += numBitsToCopy;
				
				if (bitIndexInPat >= 8)
				{
					// We moved to a new pattern byte.
					
					bitIndexInPat &= 0x7;
					++pPatternByte;
				}
				
				// Adjust the number of pattern bits available by 
				// the number of bits we copied from the pattern 
				// to the fill byte on this pass.  If we exhausted 
				// the pattern row data, move back to the start of 
				// the pattern row.

				pattern_count -= numBitsToCopy;
				if (pattern_count == 0)
				{
					// We used up the entire pattern row, move 
					// back to the start of the row.
					
					pPatternByte = row_address;
					bitIndexInPat = 0;
					
					// It is possible that we pulled some un-
					// wanted bits into the fill byte since we 
					// used the last bit in the pattern row.  
					// To correct this we need to mask off all 
					// the unfilled bits in the current fill 
					// byte. 
					
					if (bitIndexInFill >= 1)
					{
						fillByte &= (0xff << (8-bitIndexInFill));
					}
					
					// Reset the pattern address back to the 
					// start of the row.
					
					pattern_count = Fx.fx_pattern_descriptor.nXSize;
				}

				// Adjust the number of bits needed to complete 
				// the current fill byte by the number of bits we 
				// copied from the pattern to the fill byte on this 
				// pass.  If we have completed the fill byte, go 
				// save it in the fill buffer.
				
				numBitsToFill -= numBitsToCopy;

			} while (numBitsToFill != 0);

			// We have the byte to place in the fill buffer.
			
			*pFillBuffer++ = fillByte;
			
		} while (fill_count != 0);

		fill_buffer &= m;
	}
		
	return fill_buffer;
}


// Pattern Coordinates Generator
//
// This object generates the logical coordinates for accessing (possibly)
// rotated pattern data.

// Get the current X and Y and the advance to the next coordinates.
void CPatternCoordinates::GetXY(int32& nX, int32& nY)
{
	// Fill in the output variables with the current values.
	nX = (int32)(floor(m_dX+0.5));
	nY = (int32)(floor(m_dY+0.5));

	// Advance to the next points.
	m_dX += m_dXIncrement;
	m_dY += m_dYIncrement;
}

// Constructor.
CPatternCoordinates::CPatternCoordinates(int32 nX, int32 nY)
{
	// Adjust the X and Y by the pattern offset.
	nX =  nX + Fx.fx_pattern_descriptor.nXOffset;
	nY = -nY + Fx.fx_pattern_descriptor.nYOffset;

	// PMGTODO: nXOffset and nYOffset are *always* zero.
	// This might lead to some optimizations.

	// Check if we're rotated.
	m_fRotated = Fx.fx_pattern_descriptor.fRotate && (Fx.fx_dRotation != 0.0);
	if (m_fRotated)
	{
		// Compute the starting coordinates.
		double dX = (double)nX;
		double dY = (double)nY;

		// Rotate through negative angle.
		m_dX = dX*Fx.fx_dCosRotation-dY*Fx.fx_dSinRotation;
		m_dY = dX*Fx.fx_dSinRotation+dY*Fx.fx_dCosRotation;

		// The increments are the sin and cos of the rotation angle.
		m_dXIncrement = Fx.fx_dCosRotation;
		m_dYIncrement = Fx.fx_dSinRotation;
	}
	else
	{
		// Not rotated.
		m_dX = (double)nX;
		m_dY = (double)nY;
		m_dXIncrement = 1.0;
		m_dYIncrement = 0.0;
	}

	// Apply pattern expansion. Pattern expansion is based entirely on
	// resolution, not on character size.
	m_fExpanded = 0;
	if (Fx.fx_pattern_descriptor.fExpand)
	{
		// 150 dpi is a magic number.
		double dXExpansion = (double)(Fx.fx_x_resolution/150);
		double dYExpansion = (double)(Fx.fx_y_resolution/150);
		if ((dXExpansion >= 1.0) && (dYExpansion >= 1.0))
		{
			// Round to nearest 0.25.
			dXExpansion = floor(dXExpansion*4.0+0.5)/4.0;
			dYExpansion = floor(dYExpansion*4.0+0.5)/4.0;

			if ((dXExpansion > 1.0) || (dYExpansion > 1.0))
			{
				// Expand pattern.
				m_dX /= dXExpansion;
				m_dY /= dYExpansion;
				m_dXIncrement /= dXExpansion;
				m_dYIncrement /= dYExpansion;
				m_fExpanded = 1;
			}
		}
	}
}
