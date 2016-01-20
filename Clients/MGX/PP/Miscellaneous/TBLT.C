/**************************************************************************

    TBLT.C - transparent blt

 **************************************************************************/
/**************************************************************************

    (C) Copyright 1994 Microsoft Corp.  All rights reserved.

    You have a royalty-free right to use, modify, reproduce and 
    distribute the Sample Files (and/or any modified version) in 
    any way you find useful, provided that you agree that 
    Microsoft has no warranty obligations or liability for any 
    Sample Application Files which are modified. 
                                                                            
    NOTE : The assambly routine TransCopyDIBBits will not work
           unless the linker uses /NOPACKCODE, to set this flag
           use Options menu, Project menu option, then Linker
           button and select "Miscellaneous", then write in 
           front of "Other options:" /NOPACKCODE
                                                                                
 **************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <wing.h>

#include "utils.h"

extern  void PASCAL far TransCopyDIBBits( WORD            wDestSelector, 
                                          DWORD           dwDestOffset,
                                          void const far *pSource, 
                                          DWORD           Width, 
                                          DWORD           Height, 
                                          long            DestWidth,
                                          long            SourceWidth, 
                                          char unsigned   TransparentColor );
                                          
extern  void PASCAL far CopySurfaceDIBits( WORD            wDestSelector, 
                                           DWORD           dwDestOffset,
                                           void const far *pSource, 
                                           DWORD           Width, 
                                           DWORD           Height, 
                                           long            DestWidth,
                                           long            SourceWidth );

extern  void PASCAL far CopyToSurfDIBits( WORD            wDestSelector, 
                                          DWORD           dwDestOffset,
                                          void const far *pSource, 
                                          DWORD           Width, 
                                          DWORD           Height, 
                                          long            DestWidth,
                                          long            SourceWidth );
/*----------------------------------------------------------------------------

This function ignores the source origin and blts the entire source.

*/

BOOL TransparentDIBits( BITMAPINFO far *        pBufferHeader,
                        void huge *             pBufferBits,
                        int                     nXOriginDest, 
                        int                     nYOriginDest, 
                        void const far *        pBits,
                        BITMAPINFO const far *  pBitmapInfo, 
                        int                     nXOriginSrc, 
                        int                     nYOriginSrc,
                        int                     iUsage, 
                        char unsigned           TransparentColor )
{
  BOOL bReturnValue = FALSE;

  int NewDestinationXOrigin;
  int NewDestinationYOrigin;
  int DestinationDeltaX;
  int DestinationDeltaY;
  int Width;
  int Height;
  int NewSourceXOrigin;
  int NewSourceYOrigin;
  int Orientation = 1;
  int DestinationHeight = DibHeight( &pBufferHeader->bmiHeader );
  int SourceWidth       = DibWidth(  &pBitmapInfo->bmiHeader   );
  int SourceHeight      = DibHeight( &pBitmapInfo->bmiHeader   );

  char unsigned huge *pSource;
  WORD                wDestSelector;
  DWORD               dwDestOffset;
                   
  //
  //    Calcula el rectángulo al que se pide copiar el bitmap
  //    si tomar en cuenta si se sale de la pantalla.
  //                 
  RECT rSourceRect = {  nXOriginDest, 
                        nYOriginDest,
                        nXOriginDest + SourceWidth, 
                        nYOriginDest + SourceHeight };

  RECT rDestRect;
  RECT rClippedRect;
  
  //
  // Si el alto es negativo nos dice que el bitmap es top down
  // guardar en otra variable la orientacion y hacer positiva
  // la altura.
  //
  if ( DestinationHeight < 0 )   // check for top-down DIB
     {
     Orientation = -1;
     DestinationHeight = -DestinationHeight;
     }
  
  //
  // El area visible se calcula
  //
  rDestRect.top    = 0;
  rDestRect.left   = 0;
  rDestRect.bottom = DestinationHeight;
  rDestRect.right  = DibWidth( &pBufferHeader->bmiHeader );

  //
  // Calcular la interseccion de los dos rectángulos, es decir
  // el area en que vamos a copiar el bitmap (o parte de él)
  //
  
  //
  // En clip queda el resultado de la interseccion
  // si no hay interseccion no hay nada que hacer
  // retornamos FALSE
  //
  if ( IntersectRect( &rClippedRect, &rSourceRect, &rDestRect ) )
     {
     // default delta scan to width in bytes
     long DestinationScanDelta = DibWidthBytes( &pBufferHeader->bmiHeader );

     NewDestinationXOrigin = rClippedRect.left;
     NewDestinationYOrigin = rClippedRect.top;
     
     //
     // Dest delta representa la porcion del source que está
     // fuera de la pantalla (o bien de la superficie destino)
     //
     DestinationDeltaX = NewDestinationXOrigin - nXOriginDest;
     DestinationDeltaY = NewDestinationYOrigin - nYOriginDest;
                                  
     //
     // El ancho y alto del rectángulo recortado (bitmap recortado)
     //                             
     Width  = rClippedRect.right  - rClippedRect.left;
     Height = rClippedRect.bottom - rClippedRect.top;

     pSource = (char unsigned huge *) pBits;
     
     //
     // El origen del source equivale al delta del destino
     //
     NewSourceXOrigin = DestinationDeltaX;
     NewSourceYOrigin = DestinationDeltaY;
                  
     // 
     // El primer del origen byte esta con un offset :
     // renglones por fuera de la pantalla * bytes por renglon
     // + desplaz en x fuera de la pantalla.
     //             
     pSource += ( (long) SourceHeight - ( NewSourceYOrigin + Height ) ) *
                  (long) DibWidthBytes( &pBitmapInfo->bmiHeader )
                  + NewSourceXOrigin;

    // now we'll calculate the starting destination pointer taking into
    // account we may have a top-down destination
                                       
    //
    // El destino se forma de una direccion de 48 bits :
    // Selector - WORD   16 bits
    // offset   - DWORD  32 bits
    //                  --------
    //                   48 bits                                 
    //
    wDestSelector = HIWORD( pBufferBits );

    if ( Orientation < 0 )
       {
       //
       // destination is top-down : en el destino se recorren
       // los bytes para atrás.
       //
       DestinationScanDelta *= -1;
       
       //
       // Lo que está de cabeza es la superficie de destino
       // por tanto el offset es, desde el final de la memoria
       // que ocupa el buffer :
       // 
       // ( Renglones por fuera + alto del bitmap ) * bytes en scan
       // - un renglon completo
       // + el desplz. en x
       //
       dwDestOffset = (long)( NewDestinationYOrigin + Height - 1 ) *
                      (long)DibWidthBytes( &pBufferHeader->bmiHeader ) +
                      NewDestinationXOrigin;
       }
    else
       {
       // destination is bottom-up : en el destino la memoria es
       // normal, ascendiente.
       //
       // El offset es :
       // Renglones que no se pintan * bytes por renglon
       // + despl. en x
       //
       dwDestOffset = ( (long) DestinationHeight - ( NewDestinationYOrigin + Height ) ) * 
                        (long) DibWidthBytes( &pBufferHeader->bmiHeader ) +
                        NewDestinationXOrigin;
       }

    TransCopyDIBBits( wDestSelector,
                      dwDestOffset,
                      pSource,
                      Width,
                      Height,
                      DestinationScanDelta, // puede ser negativo ...
                      DibWidthBytes ( &pBitmapInfo->bmiHeader  ),
                      TransparentColor);

    bReturnValue = TRUE;
    }

  return bReturnValue;
}

BOOL CopyFromSurfaceDIBits( void const far *        pBits,
                            BITMAPINFO const far *  pBitmapInfo,
                            int                     nXOriginSrc, 
                            int                     nYOriginSrc,
                            BITMAPINFO far *        pBufferHeader,
                            void huge *             pBufferBits,
                            int                     nXOriginDest, 
                            int                     nYOriginDest, 
                            int                     iUsage ) 
{
  BOOL bReturnValue = FALSE;

  int NewDestinationXOrigin;
  int NewDestinationYOrigin;
  int DestinationDeltaX;
  int DestinationDeltaY;
  int Width;
  int Height;
  int NewSourceXOrigin;
  int NewSourceYOrigin;
  int Orientation = 1;
  int DestinationHeight = DibHeight(&pBufferHeader->bmiHeader);
  int SourceWidth = DibWidth(&pBitmapInfo->bmiHeader);
  int SourceHeight = DibHeight(&pBitmapInfo->bmiHeader);

  char unsigned huge *pSource;
  WORD wDestSelector;
  DWORD dwDestOffset;

  RECT rSourceRect = { nXOriginDest, nYOriginDest,
    nXOriginDest + SourceWidth, nYOriginDest + SourceHeight };

  RECT rDestRect;

  RECT rClippedRect;

  if(DestinationHeight < 0)   // check for top-down DIB
  {
    Orientation = -1;
    DestinationHeight = -DestinationHeight;
  }

  rDestRect.top = 0;
  rDestRect.left = 0;
  rDestRect.bottom = DestinationHeight;
  rDestRect.right = DibWidth(&pBufferHeader->bmiHeader);

  // intersect the destination rectangle with the destination DIB

  if(IntersectRect(&rClippedRect,&rSourceRect,
    &rDestRect))
  {
    // default delta scan to width in bytes
    long DestinationScanDelta = DibWidthBytes(&pBufferHeader->bmiHeader);


    NewDestinationXOrigin = rClippedRect.left;
    NewDestinationYOrigin = rClippedRect.top;

    DestinationDeltaX = NewDestinationXOrigin - nXOriginDest;
    DestinationDeltaY = NewDestinationYOrigin - nYOriginDest;

    Width = rClippedRect.right - rClippedRect.left;
    Height = rClippedRect.bottom - rClippedRect.top;

    pSource = (char unsigned huge *)pBits;

    NewSourceXOrigin = DestinationDeltaX;
    NewSourceYOrigin = DestinationDeltaY;

    pSource += ((long)SourceHeight - (NewSourceYOrigin + Height)) *
      (long)DibWidthBytes(&pBitmapInfo->bmiHeader)
      + NewSourceXOrigin;

    // now we'll calculate the starting destination pointer taking into
    // account we may have a top-down destination

    wDestSelector = HIWORD(pBufferBits);

    if(Orientation < 0)
    {
      // destination is top-down

      DestinationScanDelta *= -1;

      dwDestOffset = (long)(NewDestinationYOrigin + Height - 1) *
        (long)DibWidthBytes(&pBufferHeader->bmiHeader) +
        NewDestinationXOrigin;
    }
    else
    {
      // destination is bottom-up

      dwDestOffset = ((long)DestinationHeight -
        (NewDestinationYOrigin + Height))
        * (long)DibWidthBytes(&pBufferHeader->bmiHeader) +
        NewDestinationXOrigin;
    }

    CopySurfaceDIBits( wDestSelector,
                       dwDestOffset,
                       pSource,
                       Width,
                       Height,
                       DestinationScanDelta,
                       DibWidthBytes( &pBitmapInfo->bmiHeader ) );

    bReturnValue = TRUE;
  }

  return bReturnValue;
}


BOOL CopyToSurfaceDIBits(   BITMAPINFO far *        pBufferHeader,
                            void huge *             pBufferBits,
                            int                     nXOriginDest, 
                            int                     nYOriginDest, 
                            void const far *        pBits,
                            BITMAPINFO const far *  pBitmapInfo, 
                            int                     nXOriginSrc, 
                            int                     nYOriginSrc,
                            int                     iUsage )
{
  BOOL bReturnValue = FALSE;

  int NewDestinationXOrigin;
  int NewDestinationYOrigin;
  int DestinationDeltaX;
  int DestinationDeltaY;
  int Width;
  int Height;
  int NewSourceXOrigin;
  int NewSourceYOrigin;
  int Orientation = 1;
  int DestinationHeight = DibHeight(&pBufferHeader->bmiHeader);
  int SourceWidth = DibWidth(&pBitmapInfo->bmiHeader);
  int SourceHeight = DibHeight(&pBitmapInfo->bmiHeader);

  char unsigned huge *pSource;
  WORD wDestSelector;
  DWORD dwDestOffset;

  RECT rSourceRect = { nXOriginDest, nYOriginDest,
    nXOriginDest + SourceWidth, nYOriginDest + SourceHeight };

  RECT rDestRect;

  RECT rClippedRect;

  if(DestinationHeight < 0)   // check for top-down DIB
  {
    Orientation = -1;
    DestinationHeight = -DestinationHeight;
  }

  rDestRect.top = 0;
  rDestRect.left = 0;
  rDestRect.bottom = DestinationHeight;
  rDestRect.right = DibWidth(&pBufferHeader->bmiHeader);

  // intersect the destination rectangle with the destination DIB

  if(IntersectRect(&rClippedRect,&rSourceRect,
    &rDestRect))
  {
    // default delta scan to width in bytes
    long DestinationScanDelta = DibWidthBytes(&pBufferHeader->bmiHeader);


    NewDestinationXOrigin = rClippedRect.left;
    NewDestinationYOrigin = rClippedRect.top;

    DestinationDeltaX = NewDestinationXOrigin - nXOriginDest;
    DestinationDeltaY = NewDestinationYOrigin - nYOriginDest;

    Width = rClippedRect.right - rClippedRect.left;
    Height = rClippedRect.bottom - rClippedRect.top;

    pSource = (char unsigned huge *)pBits;

    NewSourceXOrigin = DestinationDeltaX;
    NewSourceYOrigin = DestinationDeltaY;

    pSource += ((long)SourceHeight - (NewSourceYOrigin + Height)) *
      (long)DibWidthBytes(&pBitmapInfo->bmiHeader)
      + NewSourceXOrigin;

    // now we'll calculate the starting destination pointer taking into
    // account we may have a top-down destination

    wDestSelector = HIWORD(pBufferBits);

    if(Orientation < 0)
    {
      // destination is top-down

      DestinationScanDelta *= -1;

      dwDestOffset = (long)(NewDestinationYOrigin + Height - 1) *
        (long)DibWidthBytes(&pBufferHeader->bmiHeader) +
        NewDestinationXOrigin;
    }
    else
    {
      // destination is bottom-up

      dwDestOffset = ((long)DestinationHeight -
        (NewDestinationYOrigin + Height))
        * (long)DibWidthBytes(&pBufferHeader->bmiHeader) +
        NewDestinationXOrigin;
    }

    CopyToSurfDIBits( wDestSelector,
                      dwDestOffset,
                      pSource,
                      Width,
                      Height,
                      DestinationScanDelta,
                      DibWidthBytes(&pBitmapInfo->bmiHeader)
                    );

    bReturnValue = TRUE;
  }

  return bReturnValue;
}                            
