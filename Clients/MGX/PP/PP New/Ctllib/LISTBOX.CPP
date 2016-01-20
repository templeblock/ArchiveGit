//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "imgutil.h"
#include <windowsx.h>
#include "listbox.h"

#ifdef DLGEDIT
void Control_GetClassInfo(LPTSTR lpClassName, LPWORD lpWidth, LPWORD lpHeight, LPDWORD lpStyle)
{
lstrcpy(lpClassName, MGXCTL_LISTBOX);
*lpWidth = 256;
*lpHeight = 256;
*lpStyle = 0;
}
BOOL Control_RegisterClass(HINSTANCE hInstance, UINT ClassScope)
{
return(ListBox_RegisterClass(hInstance, ClassScope));
}
#define ListBox_Control DlgEdit_Control
extern "C"{
LONG WINPROC ListBox_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam );
}
#else
LONG WINPROC ListBox_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam );
#endif

/***********************************************************************/
/***********************************************************************/
// LISTBOX CONTROL
/***********************************************************************/
/***********************************************************************/
#define LINE_WIDTH 2

typedef struct _listboxdata
	{
	BOOL	fEnableMove;
	int		iSelectDir;
	BOOL	fCapture;
	BOOL	fDragging;
	RECT	rDragCheck;
	int 	iItemHeight;
	int		iCount;
	BOOL	fSuccess;  
	int		iCurPos;
	int		iOldPos;
	int		iNewPos;
	int		idTimer;
	int		iVisible;
	HCURSOR hCursor;
	HCURSOR hOldCursor;
	} LISTBOXDATA, FAR *LPLISTBOXDATA;

LOCAL LRESULT ListBox_CallWindowProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);
LOCAL BOOL ListBox_OnCreate(HWND hWindow, CREATESTRUCT FAR* lpCreateStruct);
LOCAL void ListBox_OnDestroy(HWND hWindow);
LOCAL void ListBox_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void ListBox_OnCancelMode(HWND hWindow);
LOCAL void ListBox_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void ListBox_DoDrag(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void ListBox_DoSpecialSelect(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL LPLISTBOXDATA ListBox_GetData(HWND hWindow);
LOCAL LPLISTBOXDATA DragOK(HWND hWindow);
LOCAL void DragBegin(HWND hWindow, int x, int y);
LOCAL void DragEnd(HWND hWindow);
LOCAL void Drag_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Drag_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Drag_OnTimer(HWND hWindow, UINT id);
LOCAL void DrawBar(HWND hWindow, int iPos);
LOCAL void SelectBegin(HWND hWindow, int x, int y);
LOCAL void SelectEnd(HWND hWindow);
LOCAL void Select_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Select_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Select_OnTimer(HWND hWindow, UINT id);
LOCAL void Select_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void SelectItems(HWND hWindow, int iSelect);

static WNDPROC ListBox_lpWndProc;
static int ListBox_WindowExtra;

BOOL ListBox_RegisterClass (
HINSTANCE hInstance, UINT ClassScope)
{
WNDCLASS WndClass;

// subclass the standard Windows listbox control
clr( (LPTR)&WndClass, sizeof(WndClass) );
if ( GetClassInfo( NULL, _T("listbox"), &WndClass ) )
	{
	WNDPROC lpProc;

	ListBox_lpWndProc = (WNDPROC)WndClass.lpfnWndProc;
	lpProc = (WNDPROC)MakeProcInstance((FARPROC)ListBox_Control, hInstance);
	WndClass.lpfnWndProc = lpProc;
	WndClass.lpszClassName = MGXCTL_LISTBOX;
	WndClass.hInstance = hInstance;
	ListBox_WindowExtra = WndClass.cbWndExtra;
	WndClass.cbWndExtra += sizeof(LPVOID);

	return(RegisterClass( &WndClass ));
	}
return(FALSE);
}

BOOL ListBox_UnRegisterClass (HINSTANCE hInstance)
{
    return (UnregisterClass ((LPTSTR)MGXCTL_LISTBOX, hInstance));
}

// called in response to a LBN_MOVEITEM notification
/***********************************************************************/
void ListBox_GetMoveItem(HWND hWindow, LPINT lpOldPos, LPINT lpNewPos)
/***********************************************************************/
{
// get the listbox data pointer
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (lpData)
	{
	*lpOldPos = lpData->iOldPos;
	*lpNewPos = lpData->iNewPos;
	}
else
	{
	*lpOldPos = *lpNewPos = 0;
	}
}

/***********************************************************************/
void ListBox_MoveEnable(HWND hWindow, BOOL fEnable)
/***********************************************************************/
{
// get the listbox data pointer
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (lpData)
	{
	lpData->fEnableMove = fEnable;
	}
}

/***********************************************************************/
void ListBox_SetSelectDir(HWND hWindow, int iSelectDir)
/***********************************************************************/
{
// get the listbox data pointer
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (lpData)
	{
	lpData->iSelectDir = iSelectDir;
	if (iSelectDir)
		lpData->fEnableMove = FALSE;
	}
}

/***********************************************************************/
void ListBox_SetMoveCursor(HWND hWindow, HCURSOR hCursor)
/***********************************************************************/
{
// get the listbox data pointer
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (lpData)
	{
	lpData->hCursor = hCursor;
	}
}

/***********************************************************************/
#ifdef DLGEDIT
extern "C" {
#endif
LONG WINPROC EXPORT ListBox_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_CREATE, ListBox_OnCreate);
	HANDLE_MSG(hWindow, WM_DESTROY, ListBox_OnDestroy);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, ListBox_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_KEYDOWN, ListBox_OnKeyDown);
	HANDLE_MSG(hWindow, WM_CANCELMODE, ListBox_OnCancelMode);

	default:
	return CallWindowProc( ListBox_lpWndProc, hWindow, message, wParam, lParam );
	}
}
#ifdef DLGEDIT
}
#endif

/***********************************************************************/
LOCAL BOOL ListBox_OnCreate(HWND hWindow, CREATESTRUCT FAR* lpCreateStruct)
/***********************************************************************/
{
LPLISTBOXDATA lpData;
LONG lResult;

lpData = (LPLISTBOXDATA)Alloc(sizeof(LISTBOXDATA));
if (!lpData)
	return(FALSE);
clr((LPTR)lpData, sizeof(LISTBOXDATA));
lpData->fEnableMove = TRUE;
SetWindowLong(hWindow, ListBox_WindowExtra, (long)lpData);
lResult = FORWARD_WM_CREATE(hWindow, lpCreateStruct, ListBox_CallWindowProc);
return(lResult >= 0);
}

/***********************************************************************/
LOCAL void ListBox_OnDestroy(HWND hWindow)
/***********************************************************************/
{
// get the listbox data pointer
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (lpData)
	{
	if (lpData->hCursor)
		DestroyCursor(lpData->hCursor);
	FreeUp(lpData);
	}
}

/***********************************************************************/
LOCAL LRESULT ListBox_CallWindowProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
return(CallWindowProc(ListBox_lpWndProc, hWindow, uMsg, wParam, lParam));
}

/***********************************************************************/
LOCAL void ListBox_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
// get the listbox data pointer
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	{
	FORWARD_WM_KEYDOWN(hWindow, vk, cRepeat, flags, ListBox_CallWindowProc);
	return;
	}
if (lpData->iSelectDir)
	Select_OnKeyDown(hWindow, vk, fDown, cRepeat, flags);
else
	FORWARD_WM_KEYDOWN(hWindow, vk, cRepeat, flags, ListBox_CallWindowProc);
}

/***********************************************************************/
LOCAL void ListBox_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
// get the listbox data pointer
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	{
	FORWARD_WM_LBUTTONDOWN(hWindow, fDoubleClick, x, y, keyFlags, ListBox_CallWindowProc);
	return;
	}

if (lpData->fEnableMove)
	ListBox_DoDrag(hWindow, fDoubleClick, x, y, keyFlags);
else
if (lpData->iSelectDir)
	ListBox_DoSpecialSelect(hWindow, fDoubleClick, x, y, keyFlags);
}

/***********************************************************************/
LOCAL void ListBox_DoDrag(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
// Let the listbox control handle the mouse down so a selection can happen
FORWARD_WM_LBUTTONDOWN(hWindow, fDoubleClick, x, y, keyFlags, ListBox_CallWindowProc);

// see if it's cool for us to try to drag
LPLISTBOXDATA lpData = DragOK(hWindow);
if (!lpData)
	return;

// make the listbox think it got a mouse up so we can take over 
FORWARD_WM_LBUTTONUP(hWindow, x, y, keyFlags, ListBox_CallWindowProc);

// initialize the dragging
DragBegin(hWindow, x, y);

// dragging loop                      
while (lpData->fCapture)
	{  
	MSG msg;
	// Look for mouse, keyboard, and timer messages. 
	// keystroke messages are retrieved to avoid the message
	// queue from getting full
	while ( !PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE) &&
			!PeekMessage(&msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE) &&
       		!PeekMessage(&msg, NULL, WM_TIMER, WM_TIMER, PM_REMOVE)) 
	   	WaitMessage();
	switch(msg.message)
 		{
    	case WM_MOUSEMOVE:
			HANDLE_WM_MOUSEMOVE(msg.hwnd, msg.wParam, msg.lParam, Drag_OnMouseMove);
		break;
		case WM_TIMER:
			HANDLE_WM_TIMER(msg.hwnd, msg.wParam, msg.lParam, Drag_OnTimer);
		break;
	 	case WM_LBUTTONUP: 
			HANDLE_WM_LBUTTONUP(msg.hwnd, msg.wParam, msg.lParam, Drag_OnLButtonUp);
		break;
	 	default:
    		// Process messages we don't handle
	   		TranslateMessage(&msg);
	   		DispatchMessage(&msg);
		break;      
		}
	}

// process the drag and cleanup
DragEnd(hWindow);
}

/***********************************************************************/
LOCAL void ListBox_DoSpecialSelect(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
// get the listbox data pointer
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	{
	FORWARD_WM_LBUTTONDOWN(hWindow, fDoubleClick, x, y, keyFlags, ListBox_CallWindowProc);
	return;
	}

// give us focus on a mouse down
SetFocus(hWindow);

// initialize the dragging
SelectBegin(hWindow, x, y);

// start as if we had a mouse move
Select_OnMouseMove(hWindow, x, y, keyFlags);

// dragging loop                      
while (lpData->fCapture)
	{  
	MSG msg;
	// Look for mouse, keyboard, and timer messages. 
	// keystroke messages are retrieved to avoid the message
	// queue from getting full
	while ( !PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE) &&
			!PeekMessage(&msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE) &&
       		!PeekMessage(&msg, NULL, WM_TIMER, WM_TIMER, PM_REMOVE)) 
	   	WaitMessage();
	switch(msg.message)
 		{
    	case WM_MOUSEMOVE:
			HANDLE_WM_MOUSEMOVE(msg.hwnd, msg.wParam, msg.lParam, Select_OnMouseMove);
		break;
		case WM_TIMER:
			HANDLE_WM_TIMER(msg.hwnd, msg.wParam, msg.lParam, Select_OnTimer);
		break;
	 	case WM_LBUTTONUP: 
			HANDLE_WM_LBUTTONUP(msg.hwnd, msg.wParam, msg.lParam, Select_OnLButtonUp);
		break;
	 	default:
    		// Process messages we don't handle
	   		TranslateMessage(&msg);
	   		DispatchMessage(&msg);
		break;      
		}
	}

// process the drag and cleanup
SelectEnd(hWindow);
}

/***********************************************************************/
LOCAL void ListBox_OnCancelMode(HWND hWindow)
/***********************************************************************/
{
// WM_CANCELMODE is sent to the window that has captured the mouse before
// a message box or modal dialog is displayed. If we were dragging the item
// cancel the drag.
// get the listbox data pointer
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (lpData)
	{
	if (lpData->fCapture)
		{
       lpData->fCapture = FALSE;
       ReleaseCapture();
       if (lpData->fDragging && lpData->hCursor)
          SetCursor(lpData->hOldCursor);    
		}
	}
FORWARD_WM_CANCELMODE(hWindow, ListBox_CallWindowProc);
}

/***********************************************************************/
LOCAL LPLISTBOXDATA ListBox_GetData(HWND hWindow)
/**********************************************************************/
{
return((LPLISTBOXDATA)GetWindowLong(hWindow, ListBox_WindowExtra));
}


/***********************************************************************/
LOCAL LPLISTBOXDATA DragOK(HWND hWindow)
/***********************************************************************/
{
// get the listbox data pointer, if we don't have one, get out
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	return(NULL);

if (!lpData->fEnableMove)
	return(NULL);

// the number of items in the list box
lpData->iCount = ListBox_GetCount(hWindow);         
if (lpData->iCount == 0 ) // don't do anything to and empty list box
   return(NULL);

// see if this is a multiple selection listbox
DWORD dwStyle = GetWindowLong (hWindow, GWL_STYLE);
if (dwStyle & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL))
	{
	// see how many items are selected we only work if one is selected
	int iSelCount = ListBox_GetSelCount(hWindow);
	if (iSelCount != 1)
		return(NULL);
	ListBox_GetSelItems(hWindow, 1, &lpData->iOldPos);
	}
else
	// the current item
	lpData->iOldPos = ListBox_GetCurSel(hWindow);
return(lpData);
}


/***********************************************************************/
LOCAL void DragBegin(HWND hWindow, int x, int y)
/***********************************************************************/
{
RECT rect;

// get the listbox data pointer, if we don't have one, get out
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	return;

// get the client area
GetClientRect(hWindow, &rect);        
  
// the height of each item   
lpData->iItemHeight = ListBox_GetItemHeight(hWindow, 0);          

// the number of visible items                  
lpData->iVisible = (rect.bottom - rect.top) / lpData->iItemHeight;

// setup a timer for scrolling the list
lpData->idTimer = NULL;
lpData->idTimer = SetTimer(hWindow, 1, 100, NULL);  

// start dragging if user moves outside rectangle
SetRect(&lpData->rDragCheck, x-1, y-1, x+1, y+1);

// make sure we get all messages
SetCapture(hWindow);

// init values for dragging
lpData->fCapture = TRUE;
lpData->iCurPos = -1;
lpData->fSuccess = FALSE;
}

/***********************************************************************/
LOCAL void DragEnd(HWND hWindow)
/***********************************************************************/
{
// get the listbox data pointer, if we don't have one, get out
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	return;

// see if we ever started dragging
if (lpData->fDragging)
	{
	// if we had a cursor, reset it
	if (lpData->hCursor)
		SetCursor(lpData->hOldCursor);
    // do the move if we successfully dragged
    if (lpData->fSuccess) 
    	{
        int nItem, iTopItem;       
        TCHAR szString[256];  
        
		// save the top index, so we can reset it
		iTopItem = ListBox_GetTopIndex(hWindow);
        // turn of painting while working with list
		SetWindowRedraw(hWindow, FALSE);
        // save text of item
		ListBox_GetText(hWindow, lpData->iOldPos, szString);
        
        // delete the item
		ListBox_DeleteString(hWindow, lpData->iOldPos);
             
		// adjust index because of DeleteString
		int iNewPos = lpData->iNewPos;
        if (iNewPos > lpData->iOldPos)
        	iNewPos--;
      	// add the new string at the right spot                       
		nItem = ListBox_InsertString(hWindow, iNewPos, szString);

		// select the item we moved
		DWORD dwStyle = GetWindowLong(hWindow, GWL_STYLE);
		if (dwStyle & (LBS_MULTIPLESEL, LBS_EXTENDEDSEL))
	        ListBox_SetSel(hWindow, TRUE, nItem);
		else
	        ListBox_SetCurSel(hWindow, nItem);

		// reset the top index
		ListBox_SetTopIndex(hWindow, iTopItem);
		// turn drawing back on
		SetWindowRedraw(hWindow, TRUE);
		// inform the dialog of the move
		FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
							hWindow, LBN_MOVEITEM, SendMessage);
       	}
   	}
// turn off dragging
lpData->fDragging = FALSE;    
KillTimer(hWindow, lpData->idTimer);    
}

/***********************************************************************/
LOCAL void Drag_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
RECT rect;

LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	return;

if (!lpData->fDragging)
	{
	// Check if the user has moved out of the Drag rect. 
	// in the vertical direction.  This indicates that 
	// the drag has started.
	if ((y > lpData->rDragCheck.bottom) || 
	   	(y < lpData->rDragCheck.top))
		{
		if (lpData->hCursor)
	 		lpData->hOldCursor = SetCursor(lpData->hCursor);      
 		lpData->fDragging = TRUE;     // Drag has started                           
		}
	}
               
               
if (lpData->fDragging)
	{  
	if (lpData->hCursor)
		SetCursor(lpData->hCursor);  
	// if we are above or below the list box, then we are scrolling it, and
	// we shouldn't be drawing here              
	GetClientRect(hWindow, &rect);        

	// see if we are outside the listbox
	if ( y < rect.top)
		{
		// see if it's possible to scroll up
		int iTopItem = ListBox_GetTopIndex(hWindow);
		// if it's not possible, enable a new position to be set
		if (iTopItem == 0)
			y = rect.top;
		}
	else
	if (y > rect.bottom)
		{
		// see if it's possible to scroll down
		int iTopItem = ListBox_GetTopIndex(hWindow);
		// if it's not possible, enable a new position to be set
	  	if ((lpData->iVisible + iTopItem) >= lpData->iCount)
			y = rect.bottom;
		}
  
    if ((y >= rect.top) && (y <= rect.bottom))
    	{
		// new position changes half way thru items
		int iNewPos = (y + (lpData->iItemHeight/2)) / lpData->iItemHeight;
		if (iNewPos < 0)
			iNewPos = 0;

		// can never be greater than 1 more the last item
		if (iNewPos > lpData->iCount)
			iNewPos = lpData->iCount;

        // if it is a new item
        // and we are not past the end in a listbox with scrollbars                        
        if ( (lpData->iCurPos != iNewPos) && 
           	 ( (iNewPos < lpData->iCount + 1) ||
			   (lpData->iVisible >= lpData->iCount) ) )
	        {
         	if (lpData->iCurPos != -1)
    	      	{
              	// erase the old one                                
              	DrawBar(hWindow, lpData->iCurPos);
        	  	}  
                  
          	lpData->iCurPos = iNewPos;                            
          	DrawBar(hWindow, lpData->iCurPos);  
	        }
       	}
   	}
}


/***********************************************************************/
LOCAL void Drag_OnTimer(HWND hWindow, UINT id)
/***********************************************************************/
{
POINT pt;                  
int iTopItem;
RECT rect;

LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	return;
GetCursorPos(&pt); 
// convert the point to client coordinates
ScreenToClient(hWindow, &pt);
iTopItem = ListBox_GetTopIndex(hWindow);
GetClientRect(hWindow, &rect);        
if (pt.y < rect.top) // scroll up
	{
 	if (iTopItem > 0)
 		{
      	iTopItem--;
		// undraw the bar before we scroll up
      	DrawBar(hWindow, lpData->iCurPos);
		// scroll the listbox and make sure it gets repainted
		ListBox_SetTopIndex(hWindow, iTopItem);
		UpdateWindow(hWindow);
	   	// draw the new one at y = 0
     	lpData->iCurPos = 0;                           
      	DrawBar(hWindow, lpData->iCurPos);  
 		}                  
	}
else if (pt.y > rect.bottom) // scroll down
	{                       
	// see if there is anything to scroll
  	if (lpData->iVisible + iTopItem < lpData->iCount)
  		{                                
		// undraw the bar before we scroll down
      	DrawBar(hWindow, lpData->iCurPos);

	  	// scroll down and make sure it gets repainted
      	iTopItem++;
		ListBox_SetTopIndex(hWindow, iTopItem);
		UpdateWindow(hWindow);
  
		// always place the bar at the bottom when we scroll down
       	lpData->iCurPos = lpData->iVisible;                            

  		// draw the new one
      	DrawBar(hWindow, lpData->iCurPos);
	  	}
	}               
}

/***********************************************************************/
LOCAL void Drag_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
int iTopItem;

// End of Drag                             
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	return;
iTopItem = ListBox_GetTopIndex(hWindow);
if (lpData->fDragging) 
	{       
    // undraw the bar             
	DrawBar(hWindow, lpData->iCurPos);

	lpData->iNewPos = lpData->iCurPos + iTopItem;                     
	// the old position can't equal the new one                                        
	if (lpData->iNewPos != lpData->iOldPos)
  		lpData->fSuccess = TRUE;
	}
// turn off the mouse capture
ReleaseCapture();
lpData->fCapture = FALSE;
}


/***********************************************************************/
void DrawBar(HWND hWindow, int iPos)
/***********************************************************************/
{      
HRGN hClipRgn;                 // the clipping region    
RECT rClient;

LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	return;

HDC hDC = GetDC(hWindow);

// make sure we are clipped to client rect
GetClientRect(hWindow, &rClient);         
hClipRgn = CreateRectRgn(0,0, rClient.right - rClient.left, rClient.bottom - rClient.top);
SelectClipRgn(hDC, hClipRgn);
DeleteObject(hClipRgn); 
 
// draw the bar  
PatBlt(hDC, rClient.left,
			rClient.top + (iPos*lpData->iItemHeight)-(LINE_WIDTH/2),
       		rClient.right-rClient.left,
       		LINE_WIDTH,
       		PATINVERT);   

ReleaseDC(hWindow, hDC);
}

/***********************************************************************/
LOCAL void SelectBegin(HWND hWindow, int x, int y)
/***********************************************************************/
{
RECT rect;

// get the listbox data pointer, if we don't have one, get out
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	return;

// the number of items in the list box
lpData->iCount = ListBox_GetCount(hWindow);         

// get the client area
GetClientRect(hWindow, &rect);        
  
// the height of each item   
lpData->iItemHeight = ListBox_GetItemHeight(hWindow, 0);          

// the number of visible items                  
lpData->iVisible = (rect.bottom - rect.top) / lpData->iItemHeight;

// setup a timer for scrolling the list
lpData->idTimer = NULL;
lpData->idTimer = SetTimer(hWindow, 1, 100, NULL);  

// make sure we get all messages
SetCapture(hWindow);

// init values for dragging
lpData->fCapture = TRUE;
lpData->iCurPos = -1;
lpData->fSuccess = FALSE;
}

/***********************************************************************/
LOCAL void SelectEnd(HWND hWindow)
/***********************************************************************/
{
// get the listbox data pointer, if we don't have one, get out
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	return;

FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
					hWindow, LBN_SELCHANGE, SendMessage);
KillTimer(hWindow, lpData->idTimer);    
}

/***********************************************************************/
LOCAL void Select_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
RECT rect;

LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	return;

// if we are above or below the list box, then we are scrolling it, and
// we shouldn't be drawing here              
GetClientRect(hWindow, &rect);        

// see if we are outside the listbox
if ( y < rect.top)
	y = rect.top;
else
if (y >= rect.bottom)
	y = rect.bottom - 1;

// new position changes half way thru items
int iSelect = y / lpData->iItemHeight;

// add top index to get a real index
iSelect += ListBox_GetTopIndex(hWindow);
SelectItems(hWindow, iSelect);
UpdateWindow(hWindow);
}


/***********************************************************************/
LOCAL void Select_OnTimer(HWND hWindow, UINT id)
/***********************************************************************/
{
POINT pt;                  
int iTopItem;
RECT rect;

LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	return;
GetCursorPos(&pt); 
// convert the point to client coordinates
ScreenToClient(hWindow, &pt);
iTopItem = ListBox_GetTopIndex(hWindow);
GetClientRect(hWindow, &rect);        
if (pt.y < rect.top) // scroll up
	{
 	if (iTopItem > 0)
 		{
      	iTopItem--;
		// make sure the new top item is selected
		SelectItems(hWindow, iTopItem);
		// scroll the listbox and make sure it gets repainted
		ListBox_SetTopIndex(hWindow, iTopItem);
		UpdateWindow(hWindow);
 		}                  
	}
else if (pt.y > rect.bottom) // scroll down
	{                       
	// see if there is anything to scroll
  	if (lpData->iVisible + iTopItem < lpData->iCount)
  		{                                
	  	// scroll down and make sure it gets repainted
      	iTopItem++;
		// make sure the new bottom item is selected
		SelectItems(hWindow, iTopItem+lpData->iVisible-1);
		// scroll the listbox and make sure it gets repainted
		ListBox_SetTopIndex(hWindow, iTopItem);
		UpdateWindow(hWindow);
	  	}
	}
}

/***********************************************************************/
LOCAL void Select_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
// End of select
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	return;

// turn off the mouse capture
ReleaseCapture();
lpData->fCapture = FALSE;
}

/***********************************************************************/
LOCAL void Select_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
int iCaretIndex, iTopItem, nItems;

// get the listbox data pointer
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	{
	FORWARD_WM_KEYDOWN(hWindow, vk, cRepeat, flags, ListBox_CallWindowProc);
	}
else
if (vk == VK_UP)
	{
	iTopItem = ListBox_GetTopIndex(hWindow);
	iCaretIndex = ListBox_GetCaretIndex(hWindow);
	--iCaretIndex;
	if (iCaretIndex < 0)
		return;
	// make sure the new top item is selected
	SelectItems(hWindow, iCaretIndex);
	// scroll the listbox and make sure it gets repainted
	if (iCaretIndex < iTopItem)
		ListBox_SetTopIndex(hWindow, iCaretIndex);
	UpdateWindow(hWindow);
	FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
					hWindow, LBN_SELCHANGE, SendMessage);
	}
else
if (vk == VK_DOWN)
	{
	RECT rect;
	int iItemHeight, iVisible;

	iTopItem = ListBox_GetTopIndex(hWindow);
	iCaretIndex = ListBox_GetCaretIndex(hWindow);
	++iCaretIndex;
	nItems = ListBox_GetCount(hWindow);
	if (iCaretIndex >= nItems)
		return;
	// make sure the new top item is selected
	SelectItems(hWindow, iCaretIndex);

	// get the client area
	GetClientRect(hWindow, &rect);        
  
	// the height of each item   
	iItemHeight = ListBox_GetItemHeight(hWindow, 0);          

	// the number of visible items                  
	iVisible = (rect.bottom - rect.top) / iItemHeight;

	// see if there is anything to scroll
	if (iCaretIndex >= (iTopItem+iVisible))	
		{
		++iTopItem;
		ListBox_SetTopIndex(hWindow, iTopItem);
		}
	UpdateWindow(hWindow);
	FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow),
					hWindow, LBN_SELCHANGE, SendMessage);
	}
// ignore all other keystrokes
}

/***********************************************************************/
LOCAL void SelectItems(HWND hWindow, int iSelect)
/***********************************************************************/
{
LPLISTBOXDATA lpData = ListBox_GetData(hWindow);
if (!lpData)
	return;

if (lpData->iSelectDir > 0) // select downwards
	{
	ListBox_SelItemRange(hWindow, FALSE, 0, iSelect-1);
	ListBox_SelItemRange(hWindow, TRUE, iSelect, ListBox_GetCount(hWindow)-1);
	}
else	// select upwards
	{
	ListBox_SelItemRange(hWindow, FALSE, iSelect+1, ListBox_GetCount(hWindow)-1);
	ListBox_SelItemRange(hWindow, TRUE, 0, iSelect);
	}
ListBox_SetCaretIndex(hWindow, iSelect);
}

