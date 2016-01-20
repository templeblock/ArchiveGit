#define WS_NOCLIPSIBLINGS 0
#define LMD loadoncall moveable discardable
#define MODELESS WS_CHILD | WS_VISIBLE | WS_NOCLIPSIBLINGS | DS_NOIDLEMSG
#define SCENE dialog LMD 0, 0, 640, 480 class "scene"
#define VIDEO( n, id, x, y ) control n,	 id, "video", WS_CHILD | WS_VISIBLE | WS_TABSTOP, x, y, 1, 1
#define DIB( id, x, y ) control "", id, "dib", WS_CHILD | WS_VISIBLE | WS_NOCLIPSIBLINGS | WS_TABSTOP, x, y, 1, 1
// Dual-bitmap dib control
#define DDIB( id, x, y ) control "", id, "dib", BS_DUAL | WS_CHILD | WS_VISIBLE | WS_NOCLIPSIBLINGS | WS_TABSTOP, x, y, 1, 1
// Transparent dib control
#define TDIB( id, x, y )	control "", id, "dib", BS_TRANSPARENT | WS_CHILD | WS_VISIBLE | WS_NOCLIPSIBLINGS | WS_TABSTOP, x, y, 1, 1
#define HDIB( id, x, y )	control "", id, "dib", BS_HIGHLIGHT | BS_TRANSPARENT | WS_CHILD | WS_VISIBLE | WS_NOCLIPSIBLINGS | WS_TABSTOP, x, y, 1, 1
#define PICBUTTON HDIB
#define HOTSPOT( id, x, y, dx, dy ) control "", id, "hot", /*WS_BORDER |*/ WS_CHILD | WS_VISIBLE | WS_NOCLIPSIBLINGS | WS_TABSTOP, x, y, dx, dy
#define GOTOSCENE( n, id, x, y, dx, dy ) control n, id, "hot", /*WS_BORDER |*/ WS_CHILD | WS_VISIBLE | WS_NOCLIPSIBLINGS | WS_TABSTOP, x, y, dx, dy
#define BUTTON( n, id, x, y, dx, dy ) pushbutton n, id, x, y, dx, dy, WS_VISIBLE
#define DUTTON( n, id, x, y, dx, dy ) defpushbutton n, id, x, y, dx, dy, WS_VISIBLE
#define ITEM( n, id ) pushbutton n, id, 0, 0, 1, 1,
#define EDIT( id, x, y, dx, dy ) control "",  id, "edit", WS_BORDER | WS_VISIBLE | WS_TABSTOP, x, y, dx, dy
#define EDITX( id, x, y, dx, dy ) control "",  id, "edit", WS_VISIBLE | WS_TABSTOP, x, y, dx, dy
#define CHECK( n, id, x, y, dx, dy ) CHECKBOX n, id, x, y, dx, dy, WS_TABSTOP
#define LTEXT( n, id, x, y, dx, dy ) ltext n, id, x, y, dx, dy, SS_NOPREFIX
#define CTEXT( n, id, x, y, dx, dy ) ctext n, id, x, y, dx, dy, SS_NOPREFIX
#define RTEXT( n, id, x, y, dx, dy ) rtext n, id, x, y, dx, dy, SS_NOPREFIX
#define HSLIDER( id, x, y, dx, dy ) control "", id, "hslider", BS_HIGHLIGHT | BS_TRANSPARENT | WS_CHILD | WS_VISIBLE | WS_NOCLIPSIBLINGS | WS_TABSTOP, x, y, dx, dy
#define GAUGE( id, x, y, dx, dy) control "", id, "gauge", WS_CHILD | WS_VISIBLE | WS_NOCLIPSIBLINGS, x, y, dx, dy
#define RADAR( id, x, y, dx, dy) control "", id, "radar", WS_CHILD | WS_VISIBLE | WS_NOCLIPSIBLINGS, x, y, dx, dy
#define METER( id, x, y) control "", id, "meter", WS_CHILD | WS_VISIBLE | WS_NOCLIPSIBLINGS, x, y, 1, 1
#define METERX( id, x, y) control "", id, "meter", WS_CHILD | WS_VISIBLE | WS_NOCLIPSIBLINGS | WS_HINTCAPTION, x, y, 1, 1
#define BMULTI( id, x, y ) control "", id, "dib2", WS_CHILD | WS_VISIBLE | WS_NOCLIPSIBLINGS | WS_TABSTOP, x, y, 1, 1
#define STAGE caption
#define PROPS style MODELESS


