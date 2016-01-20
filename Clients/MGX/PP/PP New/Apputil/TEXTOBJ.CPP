//®PL1¯®FD1¯®TP0¯®BT0¯®RM255¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "stdafx.h"
#include "appdata.h"
#include "imgutil.h"

/************************************************************************/
/*                              CTextObject                             */
/************************************************************************/
CTextObject::CTextObject(const CTextObject &Src)
   {
   lpText       = Src.lpText;
   copy((LPTR)&Src.FontID, (LPTR)&FontID, sizeof(FONTID));
   Dirty        = Src.Dirty;
	AntiAlias    = Src.AntiAlias;
	Color        = Src.Color;
	iBaseRes 	= Src.iBaseRes;
   DestPos      = Src.DestPos;
   Justification= Src.Justification;
   }

CTextObject::CTextObject(CString &Text, BOOL inDirty, int inAntiAlias,
	  			LPCOLORINFO lpColor, int inJustification, LPFONTID lpFontID, LPPOINT Pos, int iRes)
   {
   lpText       = Text;
   copy((LPTR)lpFontID, (LPTR)&FontID, sizeof(FONTID));

   Dirty        = inDirty;
   AntiAlias    = inAntiAlias;
   Color        = *lpColor;
   Justification= inJustification;
	iBaseRes 	= iRes;
   DestPos      = *Pos;
   }

CTextObject::~CTextObject()
   {
   }


