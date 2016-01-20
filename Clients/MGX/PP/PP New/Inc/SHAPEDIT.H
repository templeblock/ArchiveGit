// Source file: shape.c

// structure that describes the location of a point
// for internal use only
typedef struct _shape_pos {
	POSITION pos;
	int num;
} SEPOS, FAR *LPSEPOS;

// structure to bufffer renders
// for internal use only
typedef struct _se_delay_render{
	LPPOINT lpBaseLines;
	LPPOINT lpLines;
	int iNumLines;
	int iMaxNumLines;
	LPPOINT lpHandles;
	int iNumHandles;
	int iMaxNumHandles;
	LPPOINT lpBezPairs;
	int iNumBezPairs;
	int iMaxNumBezPairs;
} SE_RENDER, FAR *LPSERENDER;


class CShapeEdit : public CPPRoot
{
public:
	CShapeEdit();	
	~CShapeEdit();	
// base functions
	BOOL Init(HWND hWnd, LPCPoly lpPoly);
	BOOL Init(HWND hWnd, LPCPolyList lpPoly);
	BOOL AddPolyList(LPCPolyList lpPolyList);
	LPCEditPolyList PeekPolyList();
	void CloseShape(BOOL bClosed);
	LPCPolyList Done();
	VOID SetBaseFunction(int function);
	int GetExtendedFunction();
	VOID MouseDown(int x, int y);
	VOID MouseUp(int x, int y);
	VOID MouseMove(int x, int y);
	VOID KeyDown(WORD wKey);
	VOID KeyUp(WORD wKey);
	VOID MakeCurves();
	VOID MakeCorners();
	VOID Draw(HDC hDC, BOOL on);
// extra functions
	VOID GetBounds(LPRECT lpRect);

//private:
	HWND hWnd;						// used to get display rate info
	// delayed render
	SE_RENDER RenderOff;
	SE_RENDER RenderOn;
private:
	LPCEditPolyList lpPolyList;		// polygons to edit (will be CEditPolys)
	int Function;					// base function
	POSITION Position;				// Active shape in shape list
	BOOL bClosed;					// Used when creating new shapes.
	
	// MOUSE DOWN - following block valid for bLDown == TRUE only
	BOOL bLDown;					// Recorded a left button down?
	POINT ptDown;					// the point where button went down.
	POINT ptLast;					// the last point moved to (point move mode only).
	BOOL bPointHit;					// Recorded at button down
	BOOL bDoSelect;					// should we do any further selection/deselection on mouse up?
	BOOL bMoveOnePoint;				// should we only move PointHit?
	SEPOS PointHit;					// Which polygon & point was hit? (for bPointHit only)
	BOOL bSelect;					// selecting?
	RECT rSelect;					// rect selecting.
	POINT ptHandle1;				// original position of grabbed handle pair.
	POINT ptHandle2;				// original position of grabbed handle pair.
	int downFunction;				// function to perform while down
	// MOUSE DOWN - above block valid for bLDown == TRUE only
	
	
	// local functions
	BOOL find_hit(int x, int y, LPSEPOS lpFound);
	BOOL find_close_line(int x, int y, LPSEPOS lpFound);
	BOOL add_point(LPPOINT lpAddPoint, LPSEPOS lpPos);
	void move_selected(int dx, int dy, BOOL bMoveHandles);
	void move_point(int dx, int dy, BOOL bMoveHandles, LPSEPOS lpPos);
	void move_shape(POSITION Shape, int dx, int dy, BOOL bMoveHandles);
	void start_move_handle(LPSEPOS lpPos);
	void move_handle(int dx, int dy, WORD wFlags, LPSEPOS lpPos);
	void select_rect(LPRECT lpSelect, BOOL bAdd);
	void select_point(LPSEPOS lpPos, BOOL bSelect, BOOL bAdd);
	void select_it(LPCEditPoly lpPoly, BOOL bOn, HDC hDC = NULL, LPTFORM lpTForm = NULL);
	void select_all(BOOL bSelect, LPCEditPoly lpPoly = NULL);
	int delete_rect(LPRECT lpArea);
	int delete_selected();
	void delete_point(LPSEPOS lpPos);
	
	// local draw functions
	void draw_range(LPCEditPoly lpPoly, int from, int to, BOOL on, HDC hDC = NULL, LPTFORM lpTForm = NULL);
};
typedef CShapeEdit FAR *LPCShapeEdit;


//void ShapeNewShapes(LPSHAPE lpShapes);	// setup rect bounds

