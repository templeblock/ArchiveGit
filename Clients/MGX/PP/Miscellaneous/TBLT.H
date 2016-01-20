/**************************************************************************

    TBLT.H - Transparent BitBlt

 **************************************************************************/

#ifndef _TBLT_H
#define _TBLT_H

#ifdef __cplusplus
extern "C" {
#endif 

BOOL TransparentDIBits( BITMAPINFO far *        pBufferHeader,
                        void huge *             pBufferBits,
                        int                     nXOriginDest, 
                        int                     nYOriginDest, 
                        void const far *        pBits,
                        BITMAPINFO const far *  pBitmapInfo, 
                        int                     nXOriginSrc, 
                        int                     nYOriginSrc,
                        int                     iUsage, 
                        char unsigned           TransparentColor );
  
BOOL CopyFromSurfaceDIBits( void const far *        pBits,
                            BITMAPINFO const far *  pBitmapInfo,
                            int                     nXOriginSrc, 
                            int                     nYOriginSrc,
                            BITMAPINFO far *        pBufferHeader,
                            void huge *             pBufferBits,
                            int                     nXOriginDest, 
                            int                     nYOriginDest, 
                            int                     iUsage );

BOOL CopyToSurfaceDIBits(   BITMAPINFO far *        pBufferHeader,
                        void huge *             pBufferBits,
                        int                     nXOriginDest, 
                        int                     nYOriginDest, 
                        void const far *        pBits,
                        BITMAPINFO const far *  pBitmapInfo, 
                        int                     nXOriginSrc, 
                        int                     nYOriginSrc,
                        int                     iUsage );
                            
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _TBLT_H
