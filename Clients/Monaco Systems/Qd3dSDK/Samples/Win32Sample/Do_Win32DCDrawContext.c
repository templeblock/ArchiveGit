// Win32DCDrawContext - this module creates the Pixmap draw context
// in this version is it creates an offscreen pixmap draw context with a Win32 GDI dib section attached

#include "Box3DSupport.h"



//----------------------------------------------------------------------------------

TQ3DrawContextObject NewWin32DCDrawContext(DocumentPtr theDocument)
{
	TQ3DrawContextData			myDrawContextData;
	TQ3Win32DCDrawContextData	myWin32DCDrawContextData;
	TQ3DrawContextObject		myDrawContext ;
	DWORD						wndClassStyle;

		//	fill in draw context data.
	myDrawContextData.clearImageMethod = kQ3ClearMethodWithColor;
		//	Set the background color.
	myDrawContextData.clearImageColor.a = 1.0F;
	myDrawContextData.clearImageColor.r = (float)theDocument->clearColorR/(float)255;
	myDrawContextData.clearImageColor.g = (float)theDocument->clearColorG/(float)255;
	myDrawContextData.clearImageColor.b = (float)theDocument->clearColorB/(float)255;

	myDrawContextData.paneState = kQ3False;
	myDrawContextData.maskState = kQ3False;
	myDrawContextData.doubleBufferState = kQ3True;	/* double buffer required */
	
	/* Assertion: window MUST be CS_OWNDC */
	wndClassStyle = GetClassLong( theDocument->fWindow, GCL_STYLE );
	if( CS_OWNDC != ( wndClassStyle & CS_OWNDC ) ) 
		return NULL;	

	/* set up the win32DCDrawContext */
	myWin32DCDrawContextData.hdc = GetDC( theDocument->fWindow );

	myWin32DCDrawContextData.drawContextData = myDrawContextData;

	//	Create draw context and return it, if it's NULL the caller must handle
	myDrawContext = Q3Win32DCDrawContext_New(&myWin32DCDrawContextData) ;

	return myDrawContext ;
}
