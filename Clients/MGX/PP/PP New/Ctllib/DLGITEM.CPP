//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "ctlutil.h"
#include "framelib.h"
#include "dlgitem.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

/***********************************************************************/
LOCAL void DoubleToString(double d, LPTSTR lpString, int iPrecision);
LOCAL BOOL isValidFixed(LPTSTR lpFixed);

static UNIT_TYPE Units;
static int UnitRes;
static int Points;
static int cDecimal = 0;

/***********************************************************************/

void SetUnitInfo( UNIT_TYPE UnitType, int res, int pts )
{
	 Units = UnitType;
	 UnitRes = res;
	 Points = pts;
}

void GetUnitInfo( LPUNIT_TYPE UnitType, LPINT res, LPINT pts )
{
	 *UnitType = Units;
	 *res = UnitRes;
	 *pts = Points;
}

VOID AstralSetDlgItemFixed( HWND hDlg, int idDlg, LFIXED Fixed,
                              BOOL bNoConvert )
{
	TCHAR szFixed[32];

	if ( bNoConvert )
		FixedAscii( Fixed, szFixed, -1 );
	else
		Unit2String( Fixed, szFixed );

	SetDlgItemText( hDlg, idDlg, szFixed );
}

/***********************************************************************/

LFIXED AstralGetDlgItemFixed( HWND hDlg, int idDlg, BOOL FAR *lpTranslated,
                              BOOL bNoConvert )
{
	LFIXED Fixed;
	double d;
	long   picas, points;
	int    units;
	TCHAR  szFixed[32];
	LPTSTR psz;

	*lpTranslated = TRUE;

	if ( bNoConvert )
		units = UT_INCHES;
	else
		units = Units;

	GetDlgItemText( hDlg, idDlg, szFixed, sizeof(szFixed) );

	if (!isValidFixed(szFixed))
	{
		*lpTranslated = FALSE;

		return((LFIXED)0);
	}

	switch ( units )
	{
		case UT_MM:
			Fixed = AsciiFixed( szFixed );
			d = DOUBLE(Fixed) / (double)25.4;
			Fixed = DBL2FIX(d);
		break;

		case UT_CM:
			Fixed = AsciiFixed( szFixed );
			d = DOUBLE(Fixed) / (double)2.54;
			Fixed = DBL2FIX(d);
		break;

		case UT_PICAS:
			if( MBStrchr( szFixed, '.' ))
			{
				Fixed = AsciiFixed(szFixed);
				d = DOUBLE(Fixed) * (double)12.0;
				if (Points != 0)
					d = 10.0 * (d / (double)Points);
				Fixed = DBL2FIX(d);
			}
			else
			{
				psz = MBStrchr( szFixed, ',' );
				points = 0;
				if( psz )
				{
					*psz = '\0';
					psz++;

					if( MBStrlen( psz ))
					{
						points = atol( psz );
					}
				}

				picas = atol(szFixed);
				points += 12 * picas;
				d = ((double)points / (double)Points) * 10.0;
				Fixed = DBL2FIX(d);
			}
		break;

		case UT_PIXELS:
			Fixed = AsciiFixed( szFixed );
			if (UnitRes != 0)
				d = DOUBLE(Fixed) / (double)UnitRes;
			Fixed = DBL2FIX(d);
		break;

		case UT_INCHES:
		default:
			Fixed = AsciiFixed(szFixed);
		break;
	}

	return( Fixed );
}

/***********************************************************************/

LPTSTR Unit2String( LFIXED Fixed, LPTSTR lpFixed )
{
	long points, picas;
	double d;

	switch ( Units )
	{
		case UT_MM:
			d = (DOUBLE(Fixed) *  (double)25.4);
			DoubleToString(d, lpFixed, 2);
		break;

		case UT_PICAS:
			// d is now in points
			d = (DOUBLE(Fixed) * (double)Points)/10.0;
			points  = (long)(d + 0.5);
			picas   = points/12L;
			points -= (picas*12L);
			wsprintf(lpFixed, "%ld,%ld", picas, points);
		break;

		case UT_CM:
			d  = (DOUBLE(Fixed) * (double)2.54);
			DoubleToString(d, lpFixed, 3);
		break;

		case UT_PIXELS:
			d = (DOUBLE(Fixed) * (double)UnitRes);
			DoubleToString(d, lpFixed, 0);
		break;

		case UT_INCHES:
		default:
			FixedAscii( Fixed, lpFixed, 3 );
		break;
	}

	return( lpFixed );
}

//************************************************************************
void SetDlgItemIntExt( HWND hDlg, ITEMID id, int iValue, LPTSTR lpString )
//************************************************************************
{
STRING szString;

wsprintf( szString, "%d%ls", iValue, lpString );
SetDlgItemTextNow( hDlg, id, szString );
}


/************************************************************************/
void SetDlgItemTextNow(HWND hDlg, int idCtl, LPTSTR lpText)
/************************************************************************/
{
HWND hControl;

if (hControl = GetDlgItem(hDlg, idCtl))
	{
	if ( lpText && *lpText )
		SetDlgItemText(hDlg, idCtl, lpText);
	else
		SetDlgItemText(hDlg, idCtl, _T(" ")); 	// This overcomes an RX deficiency
	InvalidateRect(hControl, NULL, YES);
	UpdateWindow(hControl);
	}
}

/************************************************************************/
void SetDlgItemTextMultiple(HWND hDlg, int idFirstCtl, int idLastCtl, LPTSTR lpText)
/************************************************************************/
{
int i;

for (i = idFirstCtl; i <= idLastCtl; ++i)
	SetDlgItemText(hDlg, i, lpText);
}

/************************************************************************/
void SetDlgItemIntNow(HWND hDlg, int idCtl, UINT uValue, BOOL fSigned)
/************************************************************************/
{
HWND hControl;

SetDlgItemInt(hDlg, idCtl, uValue, fSigned);
hControl = GetDlgItem(hDlg, idCtl);
InvalidateRect(hControl, NULL, YES);
UpdateWindow(hControl);
}

/***********************************************************************/

LOCAL BOOL isValidFixed(LPTSTR lpFixed)
{
	BOOL bValid = TRUE;
	int iCount = lstrlen(lpFixed);

	if (!cDecimal)
	{
		TCHAR szDecimal[4];

		GetProfileString( _T("intl"), _T("sDecimal"), _T("."), szDecimal, 2 * sizeof( TCHAR ));

		cDecimal = *szDecimal;
	}

	while(iCount--)
	{
		if (*lpFixed != cDecimal)
		{
			switch(*lpFixed)
			{
				case '0' :
				case '1' :
				case '2' :
				case '3' :
				case '4' :
				case '5' :
				case '6' :
				case '7' :
				case '8' :
				case '9' :
				case '.' :
				case ',' :
				case '+' :
				case '-' :
				break;

				default :
					bValid = FALSE;
				break;
			}
		}
		lpFixed++;
	}

	return(bValid);
}

/***********************************************************************/

LOCAL void DoubleToString(double d, LPTSTR lpString, int iPrecision)
{
	double dScale, dPrecision;
	LPTSTR lpData = lpString;

	if( d < 0 )
    {
      	*lpData++ = '-';
      	d = -d;
	}
	if( iPrecision > 0 )
	{
		dPrecision = 1.0 / pow( 10.0, ( double )iPrecision );
	}
	else
	{
		dPrecision = 1.0;
	}

	d += ( dPrecision / 2.0 );

	dScale = 1.0;

	while( dScale < ( d / 10.0 ))
		dScale *= 10.0;

	while( dScale >= 1.0 )
	{
		if( dScale <= d )
		{
			*lpData++ = ( int )'0' + ( int )( d / dScale );

			while( d >= dScale )
				d -= dScale;
		}
		else
		{
			*lpData++ = '0';
		}
		dScale /= 10.0;
	}

	if( iPrecision > 0 )
	{
		if( !cDecimal )
		{
			TCHAR szDecimal[4];

			GetProfileString( _T("intl"), _T("sDecimal"), _T("."), szDecimal, 2 * sizeof( TCHAR ));

			cDecimal = *szDecimal;
		}

		*lpData++ = cDecimal;

		dScale = dPrecision;
	
		while( dScale < ( d / 10.0 ))
			dScale *= 10.0;

		while( dScale >= dPrecision )
		{
			if( dScale <= d )
			{
				*lpData++ = ( int )'0' + ( int )( d / dScale );

				while( d >= dScale )
					d -= dScale;
			}
			else
			{
				*lpData++ = '0';
			}
			dScale /= 10.0;
		}
	}

	*lpData++ = 0;
}
