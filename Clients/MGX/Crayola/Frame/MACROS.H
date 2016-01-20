#ifndef __macros
#define __macros

/************************************************************************/
/* Macro Subroutines                                                    */
/************************************************************************/

#define RectWidth( lpRect )  ( (lpRect)->right - (lpRect)->left + 1 )
#define RectHeight( lpRect ) ( (lpRect)->bottom - (lpRect)->top + 1 )


#define LOWERCASE(c) \
	(( (c) >= 'A' && (c) <= 'Z' ) ? ((c)-('A'-'a')): (c))
#define UPPERCASE(c) \
	(( (c) >= 'a' && (c) <= 'z' ) ? ((c)+('A'-'a')): (c))

#ifdef WIN32
#define SPAN64K(lp,x) (FALSE)
#else
#define SPAN64K(lp,x) (((long)(((long)lp) & 0xFFFFL) + ((long)x)) >= 65536L)
#endif
#define RGB2long(rgb) RGB( (rgb).red, (rgb).green, (rgb).blue )
#define DN(v) ((WORD)(v)>>3)

#define AVG(a,b) (((a)+(b))>>1)
#define TOLUM(r,g,b) ( r<g ? ( r>b ? AVG(b,g) : (g>b ? AVG(g,r) : AVG(b,r)) ) \
			   : ( g>b ? AVG(b,r) : (r>b ? AVG(g,r) : AVG(b,g)) ) )
#define sign( num ) \
	( (num) == 0 ? 0 : ( (num) < 0 ? -1 : 1 ) )
#define abs( num ) \
	( (num) < 0  ? -(num) : (num) )
#define Min(a,b) ((a)<(b) ?  (a) : (b))
#define Max(a,b) ((a)>(b) ?  (a) : (b))
#define Bound(x,a,b) max(min(b,x),a)
#define mbound Bound
#define WITHIN(val,lower,upper) \
	( (val) < (lower) ? 0 : ( (val) > (upper) ? 0 : 1 ) )

#define CopyRGB(s, d)                                                   \
{                                                                       \
*((LPTR)(d)) = *((LPTR)(s));                                        \
*((LPTR)(d)+1) = *((LPTR)(s)+1);                                        \
*((LPTR)(d)+2) = *((LPTR)(s)+2);                                        \
}

#define CopyRGBHuge(s, d)                                                   \
{                                                                       \
*((HPTR)(d)) = *((HPTR)(s));                                        \
*((HPTR)(d)+1) = *((HPTR)(s)+1);                                        \
*((HPTR)(d)+2) = *((HPTR)(s)+2);                                        \
}

// check for identity transform
#define ISNULLTFORM(lpTForm) \
	(!lpTForm->bx && !lpTForm->cx  && !lpTForm->ay  && !lpTForm->cy && lpTForm->ax == FUNITY && lpTForm->by == FUNITY)

#define CopyCMYK(s, d)                                                  \
{                                                                       \
*((LPDWORD)(d)) = *((LPDWORD)(s));                                      \
}

#define CopyCMYKHuge(s, d)                                                  \
{                                                                       \
*((HPDWORD)(d)) = *((HPDWORD)(s));                                      \
}

#define CONTROL ( GetAsyncKeyState( VK_CONTROL )<0 )
#define ALT ( GetAsyncKeyState( VK_MENU )<0 )
#define SHIFT ( GetAsyncKeyState( VK_SHIFT )<0 )
#define ESCAPE ( GetAsyncKeyState( VK_ESCAPE )<0 )
#define CANCEL ( GetAsyncKeyState( VK_ESCAPE )<0 )
#define BACKSPACE ( GetAsyncKeyState( VK_BACK )<0 )
#define SPACE ( GetAsyncKeyState( VK_SPACE )<0 )
#define CAPSLOCK ( GetKeyState( VK_CAPITAL )&1 )
#define NUMLOCK ( GetKeyState( VK_NUMLOCK )&1 )
#define LBUTTON ( GetAsyncKeyState( VK_LBUTTON)<0 )
#define MBUTTON ( GetAsyncKeyState( VK_MBUTTON)<0 )
#define RBUTTON ( GetAsyncKeyState( VK_RBUTTON)<0 )
#define OPTION1 (SHIFT && !CONTROL)
#define OPTION2 (CONTROL && !SHIFT)
#define OPTION3 (SHIFT && CONTROL)
#define CONSTRAINXY 	OPTION2
#define CONSTRAINASPECT CONTROL
#define FROMCENTER 		SHIFT
#define MAKEWORD(a,b) ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define PointIsNull(point) (!(point)->x && !(point)->y)

#ifdef NOT_PORTABLE
/* These are non-portable macros */
#undef  HIWORD
#define HIWORD(l) (*((WORD*)&l+1))
#undef  LOWORD
#define LOWORD(l) (*((WORD*)&l))
#undef  HIBYTE
#define HIBYTE(w) (*((BYTE*)&w+1))
#undef  LOBYTE
#define LOBYTE(w) (*((BYTE*)&w))
#endif // NOT_PORTABLE

// Default macros
#define GetDefTmp(N,STR) GetDefaultString(#N,STR,t,sizeof(t))
#define GetDefStr(N,STR) GetDefaultString(#N,STR,N,sizeof(N))
#define GetDefInt(N,INT) (N=GetDefaultInt(#N,INT))
#define GetDefFix(N,FIX) { char t[50]; GetDefTmp(N,FixedAscii(FIX, t, 45)); N=AsciiFixed(t); }
#define GetDefHex(N,INT) { char t[40]; GetDefTmp(N,itoh(INT)); N=htoi(t); }
#define GetDefRGB(N,RGB) { char t[40]; GetDefTmp(N,RGB); AsciiRGB(t,&N); }
#define PutDefStr(N,STR) PutDefaultString(#N,STR)
#define PutDefInt(N,INT) PutDefaultInt(#N,INT)
#define PutDefFix(N,FIX) { char t[40]; PutDefaultString(#N,FixedAscii(FIX,t,-1)); }
#define PutDefRGB(N,RGB) { char t[40]; PutDefaultString(#N,RGBAscii(&RGB,t)); }
#define PutDefHex(N,INT) PutDefaultString(#N,itoh(INT))

#define PlaceWindow( hWnd, x, y ) \
	SetWindowPos( hWnd, NULL, (x), (y), 0, 0, \
		SWP_NOREDRAW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE )
#define SizeWindow( hWnd, dx, dy ) \
	SetWindowPos( hWnd, NULL, 0, 0, (dx), (dy), \
		SWP_NOREDRAW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE )
#define PlaceAndSizeWindow( hWnd, x, y, dx, dy ) \
	SetWindowPos( hWnd, NULL, (x), (y), (dx), (dy), \
		SWP_NOREDRAW | SWP_NOZORDER | SWP_NOACTIVATE )

#endif



