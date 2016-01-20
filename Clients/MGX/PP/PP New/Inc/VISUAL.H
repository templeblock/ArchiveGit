//
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// for visual.c provides visual calibration functionality

/*******************************************************************
	To use, create a dialog with the appropraite items and id's.
	 	Set up the FBC(Fill Before Calling) parts of a VISUAL structure.
	Call HandleVisualMap on appropraite messages.
	Must Call HandleVisualMap for WM_INITDIALOG.
	Must Call HandleVisualMap with WM_CLOSE before closing the dialog.
	The dialog proc ('caller') may do processing before or after the 
		calls to HandleVisualMap.
		
	Call SetVisual when you want to set/reset the state of the controls.	
	Call MoveVisual when you want to move to a new position.	

	The caller's DRAWVISUAL will be called to display the button.
	The call is: *DrawVisual(
				hDC,		 	--The HDC to draw into.
				lpRect, 		--The rectangle to draw into 
				xpos, 			--The x button locaion relative to the "after" frame. 
									will be VISUAL_BEFORE for the 'before' frame.
				ypos, 			--The y button location relative to the "after" frame
				lpVisualData)	--The current LPVISUAL structure
	Note the lpBasisFrame is the base to which effect should be applied.

	The caller's MOVEVISUAL will be called when an image button is pressed.
	The call is: *MoveVisual(
				xpos, 			--The x position relative to the "after" frame 
									will be VISUAL_BEFORE for the 'before' frame.
				ypos, 			--The y position relative to the "after" frame
				lpVisualData)	--The current LPVISUAL structure
	MoveVisual should update any of its data, and return TRUE if 
		the move should continue.
		
	When a button is clicked, only the images which need to be updated
		will be re-calculated.  This is based ob lpVisual->moveType.
		moveType = 0; // ALL are updated
		moveType = 1; // 2-d Directional (ie: its like a 2-d joystick)
		moveType = 2; // Pushed becomes center, all others updated
				
*******************************************************************/
//***************************Defines******************************
#define VISUAL_BEFORE	1000


//***************************Types******************************

typedef void (*DRAWVISUAL) (HDC, LPRECT, int, int, struct _visual far * /*LPVISUAL*/);
typedef BOOL (*MOVEVISUAL) (int, int, struct _visual far * /*LPVISUAL*/);

// FBC => fill before calling

typedef struct _visual {
 	void (* pDrawVisual) (HDC, LPRECT, int, int, struct _visual far * /*LPVISUAL*/);
	BOOL (* pMoveVisual) (int, int, struct _visual far * /*LPVISUAL*/);
	LPIMAGE lpImage;		//FBC lpImage OR lpOrgFrame
	LPFRAME lpOrgFrame;		//FBC, the original frame  (NULL if lpImage is set)
	LPTR lpUser;			//FBC, any user data 
	int step;				// FBC, size of steps
	WORD moveType;			// FBC, see above.
	LPFRAME lpBasisFrame;	// do not fill, the base frame (correct size)
	LPFRAME lpWorkFrame;	// do not fill, same size as base frame (can use)
} VISUAL;
typedef VISUAL FAR *LPVISUAL;


//***************************Routines******************************
BOOL VisualMap_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam, LPVISUAL lpVisual);
void VisualMap_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify, LPVISUAL lpVisual);
void VisualMap_OnDestroy(HWND hDlg, LPVISUAL lpVisual);
BOOL SetVisual(HWND hDlg, int x, int y, LPVISUAL lpVisual);
BOOL MoveVisual(HWND hDlg, int x, int y, LPVISUAL lpVisual);
BOOL GetPosFromID(ITEMID id, LPINT lpx, LPINT lpy);
int GetIDFromPos(int x, int y);
	
	

