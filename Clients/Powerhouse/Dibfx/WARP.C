/* Draws a bitmap, mapped to a convex polygon (draws a texture-mapped
   polygon). "Convex" means that every horizontal line drawn through
   the polygon at any point would cross exactly two active edges
   (neither horizontal lines nor zero-length edges count as active
   edges; both are acceptable anywhere in the polygon), and that the
   right & left edges never cross. Nonconvex polygons won't be drawn
   properly. Can't fail. */

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include"dib.h"
#include "wing.h"

#include "dibfx.h"

/* Describes the current location and stepping, in both the source and
   the destination, of an edge. Mirrors structure in DRAWTEXP.C. */
typedef struct {
   int Direction;    /* through edge list; 1 for a right edge (forward
			through vertex list), -1 for a left edge (backward
			through vertex list) */
   int RemainingScans;  /* height left to scan out in dest */
   int CurrentEnd;      /* vertex # of end of current edge */
   Fixed SourceX;     /* current X location in source for this edge */
   Fixed SourceY;     /* current Y location in source for this edge */
   Fixed SourceStepX; /* X step in source for Y step in dest of 1 */
   Fixed SourceStepY; /* Y step in source for Y step in dest of 1 */
			/* variables used for all-integer Bresenham's-type
			   X stepping through the dest, needed for precise
			   pixel placement to avoid gaps */
   int DestX;           /* current X location in dest for this edge */
   int DestXIntStep;    /* whole part of dest X step per scan-line Y step */
   int DestXDirection;  /* -1 or 1 to indicate which way X steps
			   (left/right) */
   int DestXErrTerm;    /* current error term for dest X stepping */
   int DestXAdjUp;      /* amount to add to error term per scan line move */
   int DestXAdjDown;    /* amount to subtract from error term when the
			   error term turns over */
} EdgeScan;

int StepEdge(EdgeScan far *);
int SetUpEdge(EdgeScan far *, int);

extern void PASCAL far ScanOutLine( long StartX, long EndX,
	Fixed SourceStartX, Fixed SourceStartY, Fixed SourceEndX,
	Fixed SourceEndY, void huge *pSource, Fixed SourceWidthBytes,
	int unsigned DestinationSegment, long unsigned DestinationOffset );

extern void PASCAL far ScanOutTransparentLine( long StartX, long EndX,
	Fixed SourceStartX, Fixed SourceStartY, Fixed SourceEndX,
	Fixed SourceEndY, void huge *pSource, Fixed SourceWidthBytes,
	int unsigned DestinationSegment, long unsigned DestinationOffset,
	BYTE TransparentColor);

	
/* Statics to save time that would otherwise be spent passing them to
   subroutines. */
int DestY;
static int MaxVert, NumVerts;
static POINT far *VertexPtr;
static POINT far *TexVertsPtr;

long ClipMinX = 0;
long ClipMinY = 0;
long ClipMaxX = 320;
long ClipMaxY = 240;

/* Draws a texture-mapped polygon, given a list of destination polygon
   vertices, a list of corresponding source texture polygon vertices, and a
   pointer to the source texture's descriptor. */

BOOL FAR PASCAL DibWarp(PDIB pdibDst, LPVOID pDest,
	POINT FAR *PolyPoints,
	PDIB pdibTexture, LPVOID pTextureBits,
	POINT FAR *TexturePoints,
	int nVerts)
{
	int MinY, MaxY, MinVert, i;
	EdgeScan LeftEdge, RightEdge;
	int unsigned DestinationSegment;
	long unsigned DestinationOffset;
	int unsigned DestinationWidth;
	long TexMapWidth;

	NumVerts = nVerts;
   VertexPtr = PolyPoints;
   TexVertsPtr = TexturePoints;
  TexMapWidth = DibWidthBytes(pdibTexture);

	ClipMinX = 0;
	ClipMinY = 0;
	ClipMaxX = DibWidth(pdibDst)-1;
	ClipMaxY = DibHeight(pdibDst)-1;

   /* Nothing to draw if less than 3 vertices */
   if (NumVerts < 3) {
      return TRUE;
   }

   /* Scan through the destination polygon vertices and find the top of
      the left and right edges, taking advantage of our knowledge that
      vertices run in a clockwise direction (else this polygon wouldn't
      be visible due to backface removal) */
   MinY = 32767;
   MaxY = -32768;
   for (i=0; i<NumVerts; i++) {
		
      if (VertexPtr[i].y < MinY) {
	 MinY = VertexPtr[i].y;
	 MinVert = i;
      }
      if (VertexPtr[i].y > MaxY) {
	 MaxY = VertexPtr[i].y;
	 MaxVert = i;
      }
   }

   /* Reject flat (0-pixel-high) polygons */
   if (MinY >= MaxY) {
      return FALSE;
   }

   /* The destination Y coordinate is not edge specific; it applies to
      both edges, since we always step Y by 1 */
   DestY = MinY;

   /* Set up to scan the initial left and right edges of the source and
      destination polygons. We always step the destination polygon edges
      by one in Y, so calculate the corresponding destination X step for
      each edge, and then the corresponding source image X and Y steps */
   LeftEdge.Direction = -1;   /* set up left edge first */
   SetUpEdge(&LeftEdge, MinVert);
   RightEdge.Direction = 1;   /* set up right edge */
   SetUpEdge(&RightEdge, MinVert);

   /* Step down destination edges one scan line at a time. At each scan
      line, find the corresponding edge points in the source image. Scan
      between the edge points in the source, drawing the corresponding
      pixels across the current scan line in the destination polygon. (We
      know which way the left and right edges run through the vertex list
      because visible (non-backface-culled) polygons always have the vertices
      in clockwise order as seen from the viewpoint) */

	DestinationSegment = (int unsigned)((long unsigned)pDest)>>16;
	DestinationOffset = ((long unsigned)pDest)&0xFFFFL;
	DestinationWidth = DibWidthBytes(pdibDst);

	// prestep to current scanline

	DestinationOffset += (long)DestinationWidth * DestY;
	
	TexMapWidth <<= 16;

   for (;;) {
      /* Done if off bottom of clip rectangle */
      if (DestY >= ClipMaxY) {
	 return TRUE;
      }

      /* Draw only if inside Y bounds of clip rectangle */
      if (DestY >= ClipMinY)
	  {
	 /* Draw the scan line between the two current edges */

//                      ScanOutLine(LeftEdge.DestX,RightEdge.DestX,LeftEdge.SourceX,
//                              LeftEdge.SourceY,RightEdge.SourceX,RightEdge.SourceY);

			ScanOutLine(LeftEdge.DestX,RightEdge.DestX,LeftEdge.SourceX,
				LeftEdge.SourceY,RightEdge.SourceX,RightEdge.SourceY,
				pTextureBits,TexMapWidth,DestinationSegment,DestinationOffset);

//        ScanOutLine(&LeftEdge, &RightEdge);
      }

      /* Advance the source and destination polygon edges, ending if we've
	 scanned all the way to the bottom of the polygon */
      if (!StepEdge(&LeftEdge)) {
	 break;
      }
      if (!StepEdge(&RightEdge)) {
	 break;
      }
      DestY++;
      DestinationOffset += DestinationWidth;
   }

	 return TRUE;
}

BOOL FAR PASCAL DibTransparentWarp(PDIB pdibDst, LPVOID pDest,
	POINT FAR *PolyPoints,
	PDIB pdibTexture, LPVOID pTextureBits,
	POINT FAR *TexturePoints,
	int nVerts, BYTE TransparentColor)
{
	int MinY, MaxY, MinVert, i;
	EdgeScan LeftEdge, RightEdge;
	int unsigned DestinationSegment;
	long unsigned DestinationOffset;
	int unsigned DestinationWidth;
	long TexMapWidth;

	NumVerts = nVerts;
   VertexPtr = PolyPoints;
   TexVertsPtr = TexturePoints;
  TexMapWidth = DibWidthBytes(pdibTexture);

	ClipMinX = 0;
	ClipMinY = 0;
	ClipMaxX = DibWidth(pdibDst)-1;
	ClipMaxY = DibHeight(pdibDst)-1;

   /* Nothing to draw if less than 3 vertices */
   if (NumVerts < 3) {
      return TRUE;
   }

   /* Scan through the destination polygon vertices and find the top of
      the left and right edges, taking advantage of our knowledge that
      vertices run in a clockwise direction (else this polygon wouldn't
      be visible due to backface removal) */
   MinY = 32767;
   MaxY = -32768;
   for (i=0; i<NumVerts; i++) {
		
      if (VertexPtr[i].y < MinY) {
	 MinY = VertexPtr[i].y;
	 MinVert = i;
      }
      if (VertexPtr[i].y > MaxY) {
	 MaxY = VertexPtr[i].y;
	 MaxVert = i;
      }
   }

   /* Reject flat (0-pixel-high) polygons */
   if (MinY >= MaxY) {
      return FALSE;
   }

   /* The destination Y coordinate is not edge specific; it applies to
      both edges, since we always step Y by 1 */
   DestY = MinY;

   /* Set up to scan the initial left and right edges of the source and
      destination polygons. We always step the destination polygon edges
      by one in Y, so calculate the corresponding destination X step for
      each edge, and then the corresponding source image X and Y steps */
   LeftEdge.Direction = -1;   /* set up left edge first */
   SetUpEdge(&LeftEdge, MinVert);
   RightEdge.Direction = 1;   /* set up right edge */
   SetUpEdge(&RightEdge, MinVert);

   /* Step down destination edges one scan line at a time. At each scan
      line, find the corresponding edge points in the source image. Scan
      between the edge points in the source, drawing the corresponding
      pixels across the current scan line in the destination polygon. (We
      know which way the left and right edges run through the vertex list
      because visible (non-backface-culled) polygons always have the vertices
      in clockwise order as seen from the viewpoint) */

	DestinationSegment = (int unsigned)((long unsigned)pDest)>>16;
	DestinationOffset = ((long unsigned)pDest)&0xFFFFL;
	DestinationWidth = DibWidthBytes(pdibDst);

	// prestep to current scanline

	DestinationOffset += (long)DestinationWidth * DestY;
	
	TexMapWidth <<= 16;

   for (;;) {
      /* Done if off bottom of clip rectangle */
      if (DestY >= ClipMaxY) {
	 return TRUE;
      }

      /* Draw only if inside Y bounds of clip rectangle */
      if (DestY >= ClipMinY)
	  {
	 /* Draw the scan line between the two current edges */

//                      ScanOutLine(LeftEdge.DestX,RightEdge.DestX,LeftEdge.SourceX,
//                              LeftEdge.SourceY,RightEdge.SourceX,RightEdge.SourceY);

			ScanOutTransparentLine(LeftEdge.DestX,RightEdge.DestX,LeftEdge.SourceX,
				LeftEdge.SourceY,RightEdge.SourceX,RightEdge.SourceY,
				pTextureBits,TexMapWidth,DestinationSegment,DestinationOffset,
				TransparentColor);

//        ScanOutLine(&LeftEdge, &RightEdge);
      }

      /* Advance the source and destination polygon edges, ending if we've
	 scanned all the way to the bottom of the polygon */
      if (!StepEdge(&LeftEdge)) {
	 break;
      }
      if (!StepEdge(&RightEdge)) {
	 break;
      }
      DestY++;
      DestinationOffset += DestinationWidth;
   }

	 return TRUE;
}

/* Steps an edge one scan line in the destination, and the corresponding
   distance in the source. If an edge runs out, starts a new edge if there
   is one. Returns 1 for success, or 0 if there are no more edges to scan. */
int StepEdge(EdgeScan far * Edge)
{
   /* Count off the scan line we stepped last time; if this edge is
      finished, try to start another one */
   if (--Edge->RemainingScans == 0) {
      /* Set up the next edge; done if there is no next edge */
      if (SetUpEdge(Edge, Edge->CurrentEnd) == 0) {
	 return(0);  /* no more edges; done drawing polygon */
      }
      return(1);     /* all set to draw the new edge */
   }

   /* Step the current source edge */
   Edge->SourceX += Edge->SourceStepX;
   Edge->SourceY += Edge->SourceStepY;

   /* Step dest X with Bresenham-style variables, to get precise dest pixel
      placement and avoid gaps */
   Edge->DestX += Edge->DestXIntStep;  /* whole pixel step */
   /* Do error term stuff for fractional pixel X step handling */
   if ((Edge->DestXErrTerm += Edge->DestXAdjUp) > 0) {
      Edge->DestX += Edge->DestXDirection;
      Edge->DestXErrTerm -= Edge->DestXAdjDown;
   }

   return(1);
}

/* Sets up an edge to be scanned; the edge starts at StartVert and proceeds
   in direction Edge->Direction through the vertex list. Edge->Direction must
   be set prior to call; -1 to scan a left edge (backward through the vertex
   list), 1 to scan a right edge (forward through the vertex list).
   Automatically skips over 0-height edges. Returns 1 for success, or 0 if
   there are no more edges to scan. */
int SetUpEdge(EdgeScan far * Edge, int StartVert)
{
   int NextVert, DestXWidth;
   Fixed DestYHeight;

   for (;;) {
      /* Done if this edge starts at the bottom vertex */
      if (StartVert == MaxVert) {
	 return(0);
      }

      /* Advance to the next vertex, wrapping if we run off the start or end
	 of the vertex list */
      NextVert = StartVert + Edge->Direction;
      if (NextVert >= NumVerts) {
	 NextVert = 0;
      } else if (NextVert < 0) {
	 NextVert = NumVerts - 1;
      }

      /* Calculate the variables for this edge and done if this is not a
	 zero-height edge */
      if ((Edge->RemainingScans =
	    VertexPtr[NextVert].y - VertexPtr[StartVert].y) != 0) {
	 DestYHeight = IntToFixed(Edge->RemainingScans);
	 Edge->CurrentEnd = NextVert;
	 Edge->SourceX = IntToFixed(TexVertsPtr[StartVert].x);
	 Edge->SourceY = IntToFixed(TexVertsPtr[StartVert].y);
	 Edge->SourceStepX = FixedDivide(IntToFixed(TexVertsPtr[NextVert].x) -
	       Edge->SourceX, DestYHeight);
	 Edge->SourceStepY = FixedDivide(IntToFixed(TexVertsPtr[NextVert].y) -
	       Edge->SourceY, DestYHeight);

	 /* Set up Bresenham-style variables for dest X stepping */
	 Edge->DestX = VertexPtr[StartVert].x;
	 if ((DestXWidth =
	       (VertexPtr[NextVert].x - VertexPtr[StartVert].x)) < 0) {
	    /* Set up for drawing right to left */
	    Edge->DestXDirection = -1;
	    DestXWidth = -DestXWidth;
	    Edge->DestXErrTerm = 1 - Edge->RemainingScans;
	    Edge->DestXIntStep = -(DestXWidth / Edge->RemainingScans);
	 } else {
	    /* Set up for drawing left to right */
	    Edge->DestXDirection = 1;
	    Edge->DestXErrTerm = 0;
	    Edge->DestXIntStep = DestXWidth / Edge->RemainingScans;
	 }
	 Edge->DestXAdjUp = DestXWidth % Edge->RemainingScans;
	 Edge->DestXAdjDown = Edge->RemainingScans;
	 return(1);  /* success */
      }
      StartVert = NextVert;   /* keep looking for a non-0-height edge */
   }
}
