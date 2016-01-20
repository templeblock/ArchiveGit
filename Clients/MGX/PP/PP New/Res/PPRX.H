//************************************************************************
// Border Macros
//************************************************************************
#define DLGPANE frame THIN BEVELOUT TOP2BOTTOM FLUSH

//************************************************************************
// Control Macros
//************************************************************************
#define RICON(id) \
	control /U "ricon" IDX("",id) PWH(34,34) TAB MID
#define RADIO(n,id,x,y) RADIORX(n,id) XY(x,y)

#ifdef CHECK
#undef CHECK
#endif // CHECK

#define CHECK(n,id,x,y) CHECKRX(n,id) XY(x,y)

#define STICK(id) \
	control /U "joystick" IDX("",id) TAB HEIGHT(SLY) BORDER
#define LEVER(id) \
	control /U "lever" IDX("",id) TAB HEIGHT(SLY) BORDER
#define PMAP(id) \
	control /U "map" IDX("",id) TAB HEIGHT(SLY) BORDER /E+ WS_NOSHADOWS
#define PMAP2(id) \
	control /U "map" IDX("",id) TAB HEIGHT(SLY) BORDER /E+ WS_NOSHADOWS /E+ WS_TOOL
#define MAP(id) \
	control /U "map" IDX("",id) TAB HEIGHT(SLY) BORDER
#define HISTO( n, id ) \
	control /U "histo" IDX(n,id) TAB
#define EDIT( id, x, y, dx, dy ) EDITRX(id) XY(x,y) WH(dx,dy)
#define RTEXT(n,id,x,y,dx,dy) RTEXTRX(n,id) XY(x,y) WH(dx,dy)
#define CTEXT(n,id,x,y,dx,dy) CTEXTRX(n,id) XY(x,y) WH(dx,dy)


