/****************************************************************

	File:		SC-SpecChng.c

	$Header: /Projects/Toolbox/ct/SC_SPCHG.CPP 2	 5/30/97 8:45a Wmanis $
		
	Contains:

	When type specs change their are certain types of things that need 
	to be done to bring the world back into equilibrium. These tasks 
	typically involve REFORMATTING and REPAINT. Since a certain number 
	of the formating computations are held with the characters themselves
	the reformatting requires two operations. We will call these
	RETABULATION - correcting the escapement stored with the characters -
	and the LINEBREAKING - the act of breaking text into lines.
	
	Therefore when a spec changes one or more tasks may need to 
	be performed, we want to determine the minimum set of tasks to
	perform to return the world to equilibrium.
	
	The tasks are performed in the following order:
	
		TABULATION
		LINE BREAKING
		PAINTING
		
	Here a few examples of spec changes and what they should cause:
	
		color change				- scREPAINT
		word space change			- scREBREAK & scREPAINT
		lead change 				- scREBREAK & scREPAINT
		font change 				- scRETABULATE, scREBREAK & scREPAINT
		pointsize change			- scRETABULATE, scREBREAK & scREPAINT
		setsize change				- scRETABULATE, scREBREAK & scREPAINT
		pair/track kerning change	- scRETABULATE, scREBREAK & scREPAINT
		
		hyphenation language change - scRETABULATE, scREBREAK & scREPAINT
		# of consecutive hyph change- scREBREAK & scREPAINT

		
	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	

 ************************************************************************/ 

#include "sccallbk.h"

eSpecTask	SpecTaskCalculate( eSpecChange changeType )
{
	switch ( changeType ) {
		case scHoblique:
		case scVoblique:
			return (eSpecTask)((int)eSCRetabulate | (int)eSCRepaint);
			
		case scLanguage:
		case scFont:			
		case scCharTransform:
		case scPointSize:
		case scSetSize:
		case scRotation:
		case scKern:
		case scTrack:
		case scMinLsp:
		case scOptLsp:	
		case scMaxLsp:
		case scMinWsp:
		case scOptWsp:
		case scMaxWsp:	
		case scHyphenation:
		case scHyphLines:
		case scHyphExcep:
		case scHyphMinSize:
		case scPreHyphs:
		case scPostHyphs:
		case scHyphPropensity:
		case scHyphCaps:
		case scHyphAcros:
		case scHyphExtra1:
		case scHyphExtra2:
		default:
			return eSCRetabulate;
			
		case scColor:
		case scRenderAttribute:
//		case scULShow:
//		case scULpos:
//		case scULthick:
//			return eSCRepaint;
			
		case scLead:
		case scBaseline:
		case scAboveLead:
		case scBelowLead:
		case scIndLines:
		case scIndAmount:
		case scIndDepth:
		case scIndLeftBL:
		case scIndRightBL:
		case scIndentExtra1:
		case scIndentExtra2:
		case scColNoBreak:
		case scKeepNext:
		case scLinesBefore:
		case scLinesAfter:
		case scWidowOrphanExtra1:
		case scWidowOrphanExtra2:
		case scRag:
		case scForceJust:
		case scRagPattern:	
		case scRagZone: 	
		case scKernMargins: 
		case scHLeft:
		case scHRight:
		case scHLeftAmount:
		case scHRightAmount:
		case scRagExtra1:
		case scRagExtra2:
		case scHPuncExtra1:
		case scHPuncExtra2: 	
		case scMaxFillChars:
		case scFillPos:
		case scFillChar:
		case scFillAlign:
		case scMaxTabs:
		case scTabPos:
		case scTabAlign:
		case scTabChar:
		case scTabFillAlign:
		
		case scMinMeasure:
		case scRunAroundBorder:
		case scFirstLine:
		
			return eSCRebreak;	
	}
}

/************************************************************************/
