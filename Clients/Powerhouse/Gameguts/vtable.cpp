#include <windows.h>
#include <mmsystem.h>
#include "proto.h"
#include "control.h"
#include "commonid.h"
#include "parser.h"
#include "limits.h"

class CVTableParser : public CParser
{
	public:
		CVTableParser(LPSTR lpTableData)
			: CParser(lpTableData) {}

	protected:
		BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
};	

/***********************************************************************/
BOOL CVTableParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
/***********************************************************************/
{
	BOOL fError = TRUE; // always keep trying on errors
	BOOL bSubShot, bError;
	LPSTR lpValue;
	long l, limit;
	LPSHOT lpShots;
	
	lpShots = (LPSHOT)dwUserData;
	lpShots += nIndex;
	
	/***********************************************************************/
	// The 'shot-id' command comes first
	/***********************************************************************/
	if (!lpShots->lShotID)
	{
		if ( !(l = GetShotID(lpEntry, lpKey)) )
			return(fError);
	
		lstrcpy(lpShots->szShotName, lpKey);
		lpShots->lShotID = l;
		lpShots->aShotID = (ATOM)l;
	
		if (nValues != 2 && nValues != 3)
		{
			Print("'%s'\n Expected start and end (and disk) values", lpEntry);
			return(fError);
		}
	
		/***********************************************************************/
		// Get the start frame value
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues);
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad start frame at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lStartFrame = l;
	
		/***********************************************************************/
		// Get the end frame value
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues);
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad end frame at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lEndFrame = l;
		if ( lpShots->lEndFrame < lpShots->lStartFrame )
		{
			Print("'%s'\n End frame is less than the start frame at '%s'", lpEntry, lpValue);
		}
	
		/***********************************************************************/
		// Get the disk value
		/***********************************************************************/
		if ( nValues != 3 )
			lpShots->wDisk = 1;
		else
		{
			lpValue = GetNextValue(&lpValues);
			l = latol( lpValue, &bError );
			if ( bError || l < 0 )
			{
				Print("'%s'\n Bad disk number at '%s'", lpEntry, lpValue);
				return(fError);
			}

			lpShots->wDisk = (WORD)l;
		}
	}
	else
	/***********************************************************************/
	// The end command is second
	/***********************************************************************/
	if (!lpShots->lEndShot)
	{
		/***********************************************************************/
		// See if we have the 'end' command
		/***********************************************************************/
		if (lstrcmpi(lpKey, "end"))
		{
			Print("'%s'\n Expected end command at '%s'", lpEntry, lpKey);
			return(fError);
		}
		// Got an 'end' command
	
		/***********************************************************************/
		// See if 'loop' command is specified
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues);
		if ( lstrcmpi( lpValue, "loop" ) )
			goto SkipLoop;
		// Got a 'loop' command
	
		/***********************************************************************/
		// Get numeric argument; could be #frames or #seconds
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues);
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad loop value at '%s'", lpEntry, lpValue);
			return(fError);
		}
		// Got a numeric argument; could be #frames or #seconds
	
		/***********************************************************************/
		// See if 'frames' command is specified
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues);
		if ( !lpValue || lstrcmpi( lpValue, "frames" ) )
			goto SkipFrames;
		// Got a 'frames' command
		limit = lpShots->lEndFrame - lpShots->lStartFrame;
		if ( l > limit ) l = limit;
		lpShots->lEndLoop |= (l<<16); // Stuff the number of frames into the HIWORD
	
		/***********************************************************************/
		// See if a second numeric argument; could be #seconds
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues);
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
			goto SkipLoop;
		// Got a numeric argument
	
		/***********************************************************************/
		// See if 'seconds' command is specified
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
	SkipFrames:
		// At this point, assume the numeric argument is seconds
		lpShots->lEndLoop |= LOWORD(l); // Stuff the number of seconds into the LOWORD
		if ( !lpValue || lstrcmpi( lpValue, "seconds" ) )
			goto SkipLoop;
		// Got a 'seconds' command
	
		/***********************************************************************/
		// Get 'goto' argument; the shot id
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
	SkipLoop:
		if ( !lpValue ) // if none specified, goto ourselves
			l = lpShots->lShotID;
		else
		{
			if (! (l = GetShotID(lpEntry, lpValue)) )
				return(fError);
		}
		lpShots->lEndShot = l;
	
		/***********************************************************************/
		// See if 'count' or 'index' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		if ( lpValue && !lstrcmpi( lpValue, "count" ) )
		{
			// Got a 'count' command
	
			/***********************************************************************/
			// Get 'count' argument; # of shot to select from
			/***********************************************************************/
			lpValue = GetNextValue(&lpValues); 
			l = latol( lpValue, &bError );
			if ( bError || l < 0 )
			{
				Print("'%s'\n Bad shot count at '%s'", lpEntry, lpValue);
				return(fError);
			}
			lpShots->lEndCount = l;
			lpValue = GetNextValue(&lpValues); 
		}
	    else
	    if ( lpValue && !lstrcmpi(lpValue, "index"))
	    {
			// Got an 'index' command
	
			/***********************************************************************/
			// Get 'index' argument; Register # used to offset the specified shot
			/***********************************************************************/
			lpValue = GetNextValue(&lpValues); 
			l = latol( lpValue, &bError );
			if ( bError || l < 0 )
			{
				Print("'%s'\n Bad index register at '%s'", lpEntry, lpValue);
				return(fError);
			}
			lpShots->lEndCount = -l;
			lpValue = GetNextValue(&lpValues); 
	    }
	    else
	    if ( lpValue && !lstrcmpi(lpValue, "compare"))		// Check for a 'compare' command
	    {
			/***********************************************************************/
			// Get 'compare' arguments; Register # and constant value to compare against
			/***********************************************************************/
			lpValue = GetNextValue(&lpValues); 
			l = latol( lpValue, &bError );
			if ( bError || l < 0 )
			{
				Print("'%s'\n Bad compare register at '%s'", lpEntry, lpValue);
				return(fError);
			}
			lpShots->lCompReg = l;
			
			lpValue = GetNextValue(&lpValues); 
			l = latol( lpValue, &bError );
			if ( bError || l < 0 )
			{
				Print("'%s'\n Bad compare value at '%s'", lpEntry, lpValue);
				return(fError);
			}
			lpShots->lCompValue = l;
			lpValue = GetNextValue(&lpValues); 
	    }
	
		/***********************************************************************/
		// Check for excess arguments
		/***********************************************************************/
		if ( lpValue )
		{
			Print("'%s'\n Extra 'end' argument at '%s'", lpEntry, lpValue);
			return(fError);
		}
	}
	else
	/***********************************************************************/
	// See if 'left' or 'leftsub' command is specified
	/***********************************************************************/
	if ( (bSubShot = !lstrcmpi(lpKey, "leftsub")) || !lstrcmpi(lpKey, "left") )
	{ // Got a 'left' command
	
		lpShots->bLeftIsSubShot = bSubShot;
	
		/***********************************************************************/
		// Get left 'goto' argument; the shot id
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		if (! (l = GetShotID(lpEntry, lpValue)) )
			return(fError);
		lpShots->lLeftShot = l;
	
		/***********************************************************************/
		// See if left goto 'count' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		if ( !lpValue || lstrcmpi( lpValue, "count" ) )
			goto SkipLeftCount;
		// Got a 'count' command
	
		/***********************************************************************/
		// Get left goto 'count' argument; # of shot to select from
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad shot count at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lLeftCount = l;
	
		/***********************************************************************/
		// See if left goto 'index' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipLeftCount:
		if ( !lpValue || lstrcmpi( lpValue, "index" ) )
			goto SkipLeftIndex;
		// Got an 'index' command
	
		/***********************************************************************/
		// Get left goto 'index' argument; Register # used to offset the specified shot
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad index register at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lLeftCount = -l;
	
		/***********************************************************************/
		// See if left 'hot' argument; row and column range
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipLeftIndex:
		if ( lpValue )
		{
			BOOL bError;
			l = latol( lpValue, &bError );
			if ( bError )
			{
				Print("'%s'\n Bad hotspot at '%s'", lpEntry, lpValue);
				return(fError);
			}
			lpShots->lLeftHotspot = l;
		}
	
		/***********************************************************************/
		// Check for excess arguments
		/***********************************************************************/
		if ( lpValue = GetNextValue( &lpValues ) )
		{
			Print("'%s'\n Extra 'left' argument at '%s'", lpEntry, lpValue);
			return(fError);
		}
	}
	else
	/***********************************************************************/
	// See if 'right' or 'rightsub' command is specified
	/***********************************************************************/
	if ( (bSubShot = !lstrcmpi(lpKey, "rightsub")) || !lstrcmpi(lpKey, "right") )
	{ // Got a 'right' command
	
		lpShots->bRightIsSubShot = bSubShot;
	
		/***********************************************************************/
		// Get right 'goto' argument; the shot id
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		if (! (l = GetShotID(lpEntry, lpValue)) )
			return(fError);
		lpShots->lRightShot = l;
	
		/***********************************************************************/
		// See if right goto 'count' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		if ( !lpValue || lstrcmpi( lpValue, "count" ) )
			goto SkipRightCount;
		// Got a 'count' command
	
		/***********************************************************************/
		// Get right goto 'count' argument; # of shot to select from
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad shot count at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lRightCount = l;
	
		/***********************************************************************/
		// See if right goto 'index' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipRightCount:
		if ( !lpValue || lstrcmpi( lpValue, "index" ) )
			goto SkipRightIndex;
		// Got an 'index' command
	
		/***********************************************************************/
		// Get right goto 'index' argument; Register # used to offset the specified shot
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad index register at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lRightCount = -l;
	
		/***********************************************************************/
		// See if right 'hot' argument; row and column range
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipRightIndex:
		if ( lpValue )
		{
			BOOL bError;
			l = latol( lpValue, &bError );
			if ( bError )
			{
				Print("'%s'\n Bad hotspot at '%s'", lpEntry, lpValue);
				return(fError);
			}
			lpShots->lRightHotspot = l;
		}
	
		/***********************************************************************/
		// Check for excess arguments
		/***********************************************************************/
		if ( lpValue = GetNextValue( &lpValues ) )
		{
			Print("'%s'\n Extra 'right' argument at '%s'", lpEntry, lpValue);
			return(fError);
		}
	}
	else
	/***********************************************************************/
	// See if 'up' or 'upsub' command is specified
	/***********************************************************************/
	if ( (bSubShot = !lstrcmpi(lpKey, "upsub")) || !lstrcmpi(lpKey, "up") )
	{ // Got a 'up' command
	
		lpShots->bUpIsSubShot = bSubShot;
	
		/***********************************************************************/
		// Get up 'goto' argument; the shot id
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		if (! (l = GetShotID(lpEntry, lpValue)) )
			return(fError);
		lpShots->lUpShot = l;
	
		/***********************************************************************/
		// See if up goto 'count' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		if ( !lpValue || lstrcmpi( lpValue, "count" ) )
			goto SkipUpCount;
		// Got a 'count' command
	
		/***********************************************************************/
		// Get up goto 'count' argument; # of shot to select from
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad shot count at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lUpCount = l;
	
		/***********************************************************************/
		// See if up goto 'index' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipUpCount:
		if ( !lpValue || lstrcmpi( lpValue, "index" ) )
			goto SkipUpIndex;
		// Got an 'index' command
	
		/***********************************************************************/
		// Get up goto 'index' argument; Register # used to offset the specified shot
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad index register at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lUpCount = -l;
	
		/***********************************************************************/
		// See if up 'hot' argument; row and column range
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipUpIndex:
		if ( lpValue )
		{
			BOOL bError;
			l = latol( lpValue, &bError );
			if ( bError )
			{
				Print("'%s'\n Bad hotspot at '%s'", lpEntry, lpValue);
				return(fError);
			}
			lpShots->lUpHotspot = l;
		}
	
		/***********************************************************************/
		// Check for excess arguments
		/***********************************************************************/
		if ( lpValue = GetNextValue( &lpValues ) )
		{
			Print("'%s'\n Extra 'up' argument at '%s'", lpEntry, lpValue);
			return(fError);
		}
	}
	else
	/***********************************************************************/
	// See if 'down' or 'downsub' command is specified
	/***********************************************************************/
	if ( (bSubShot = !lstrcmpi(lpKey, "downsub")) || !lstrcmpi(lpKey, "down") )
	{ // Got a 'down' command
	
		lpShots->bDownIsSubShot = bSubShot;
	
		/***********************************************************************/
		// Get down 'goto' argument; the shot id
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		if (! (l = GetShotID(lpEntry, lpValue)) )
			return(fError);
		lpShots->lDownShot = l;
	
		/***********************************************************************/
		// See if down goto 'count' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		if ( !lpValue || lstrcmpi( lpValue, "count" ) )
			goto SkipDownCount;
		// Got a 'count' command
	
		/***********************************************************************/
		// Get down goto 'count' argument; # of shot to select from
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad shot count at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lDownCount = l;
	
		/***********************************************************************/
		// See if down goto 'index' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipDownCount:
		if ( !lpValue || lstrcmpi( lpValue, "index" ) )
			goto SkipDownIndex;
		// Got an 'index' command
	
		/***********************************************************************/
		// Get down goto 'index' argument; Register # used to offset the specified shot
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad index register at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lDownCount = -l;
	
		/***********************************************************************/
		// See if down 'hot' argument; row and column range
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipDownIndex:
		if ( lpValue )
		{
			BOOL bError;
			l = latol( lpValue, &bError );
			if ( bError )
			{
				Print("'%s'\n Bad hotspot at '%s'", lpEntry, lpValue);
				return(fError);
			}
			lpShots->lDownHotspot = l;
		}
	
		/***********************************************************************/
		// Check for excess arguments
		/***********************************************************************/
		if ( lpValue = GetNextValue( &lpValues ) )
		{
			Print("'%s'\n Extra 'down' argument at '%s'", lpEntry, lpValue);
			return(fError);
		}
	}
	else
	/***********************************************************************/
	// See if 'home' or 'homesub' command is specified
	/***********************************************************************/
	if ( (bSubShot = !lstrcmpi(lpKey, "homesub")) || !lstrcmpi(lpKey, "home") )
	{ // Got a 'home' command
	
		lpShots->bHomeIsSubShot = bSubShot;
	
		/***********************************************************************/
		// Get home 'goto' argument; the shot id
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		if (! (l = GetShotID(lpEntry, lpValue)) )
			return(fError);
		lpShots->lHomeShot = l;
	
		/***********************************************************************/
		// See if home goto 'count' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		if ( !lpValue || lstrcmpi( lpValue, "count" ) )
			goto SkipHomeCount;
		// Got a 'count' command
	
		/***********************************************************************/
		// Get home goto 'count' argument; # of shot to select from
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad shot count at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lHomeCount = l;
	
		/***********************************************************************/
		// See if home goto 'index' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipHomeCount:
		if ( !lpValue || lstrcmpi( lpValue, "index" ) )
			goto SkipHomeIndex;
		// Got an 'index' command
	
		/***********************************************************************/
		// Get home goto 'index' argument; Register # used to offset the specified shot
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad index register at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lHomeCount = -l;
	
		/***********************************************************************/
		// See if home 'hot' argument; row and column range
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipHomeIndex:
		if ( lpValue )
		{
			BOOL bError;
			l = latol( lpValue, &bError );
			if ( bError )
			{
				Print("'%s'\n Bad hotspot at '%s'", lpEntry, lpValue);
				return(fError);
			}
			lpShots->lHomeHotspot = l;
		}
	
		/***********************************************************************/
		// Check for excess arguments
		/***********************************************************************/
		if ( lpValue = GetNextValue( &lpValues ) )
		{
			Print("'%s'\n Extra 'home' argument at '%s'", lpEntry, lpValue);
			return(fError);
		}
	}
	else
	/***********************************************************************/
	// See if 'ins' or 'inssub' command is specified
	/***********************************************************************/
	if ( (bSubShot = !lstrcmpi(lpKey, "inssub")) || !lstrcmpi(lpKey, "ins") )
		{ // Got a 'ins' command
	
		lpShots->bInsIsSubShot = bSubShot;
	
		/***********************************************************************/
		// Get ins 'goto' argument; the shot id
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		if (! (l = GetShotID(lpEntry, lpValue)) )
			return(fError);
		lpShots->lInsShot = l;
	
		/***********************************************************************/
		// See if ins goto 'count' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		if ( !lpValue || lstrcmpi( lpValue, "count" ) )
			goto SkipInsCount;
		// Got a 'count' command
	
		/***********************************************************************/
		// Get ins goto 'count' argument; # of shot to select from
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad shot count at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lInsCount = l;
	
		/***********************************************************************/
		// See if ins goto 'index' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipInsCount:
		if ( !lpValue || lstrcmpi( lpValue, "index" ) )
			goto SkipInsIndex;
		// Got an 'index' command
	
		/***********************************************************************/
		// Get ins goto 'index' argument; Register # used to offset the specified shot
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad index register at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lInsCount = -l;
	
		/***********************************************************************/
		// See if ins 'hot' argument; row and column range
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipInsIndex:
		if ( lpValue )
		{
			BOOL bError;
			l = latol( lpValue, &bError );
			if ( bError )
			{
				Print("'%s'\n Bad hotspot at '%s'", lpEntry, lpValue);
				return(fError);
			}
			lpShots->lInsHotspot = l;
		}
	
		/***********************************************************************/
		// Check for excess arguments
		/***********************************************************************/
		if ( lpValue = GetNextValue( &lpValues ) )
		{
			Print("'%s'\n Extra 'ins' argument at '%s'", lpEntry, lpValue);
			return(fError);
		}
	}
	else
	/***********************************************************************/
	// See if 'del' or 'delsub' command is specified
	/***********************************************************************/
	if ( (bSubShot = !lstrcmpi(lpKey, "delsub")) || !lstrcmpi(lpKey, "del") )
		{ // Got a 'del' command
	
		lpShots->bDelIsSubShot = bSubShot;
	
		/***********************************************************************/
		// Get del 'goto' argument; the shot id
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		if (! (l = GetShotID(lpEntry, lpValue)) )
			return(fError);
		lpShots->lDelShot = l;
	
		/***********************************************************************/
		// See if del goto 'count' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		if ( !lpValue || lstrcmpi( lpValue, "count" ) )
			goto SkipDelCount;
		// Got a 'count' command
	
		/***********************************************************************/
		// Get del goto 'count' argument; # of shot to select from
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad shot count at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lDelCount = l;
	
		/***********************************************************************/
		// See if del goto 'index' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipDelCount:
		if ( !lpValue || lstrcmpi( lpValue, "index" ) )
			goto SkipDelIndex;
		// Got an 'index' command
	
		/***********************************************************************/
		// Get del goto 'index' argument; Register # used to offset the specified shot
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad index register at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lDelCount = -l;
	
		/***********************************************************************/
		// See if del 'hot' argument; row and column range
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipDelIndex:
		if ( lpValue )
		{
			BOOL bError;
			l = latol( lpValue, &bError );
			if ( bError )
			{
				Print("'%s'\n Bad hotspot at '%s'", lpEntry, lpValue);
				return(fError);
			}
			lpShots->lDelHotspot = l;
		}
	
		/***********************************************************************/
		// Check for excess arguments
		/***********************************************************************/
		if ( lpValue = GetNextValue( &lpValues ) )
		{
			Print("'%s'\n Extra 'del' argument at '%s'", lpEntry, lpValue);
			return(fError);
		}
	}
	else
	/***********************************************************************/
	// See if 'flag' command is specified
	/***********************************************************************/
	if (!lstrcmpi(lpKey, "flag"))
		{ // Got a 'flag' command
	
		/***********************************************************************/
		// Get 'flag' argument; any number of flags from 1 to 32
		/***********************************************************************/
		while (--nValues >= 0)
			{
			lpValue = GetNextValue( &lpValues );
			l = latol( lpValue, &bError );
			if ( bError || l < 0 )
				{
				Print("'%s'\n Bad flag at '%s'", lpEntry, lpValue);
				break;
				}
			lpShots->lFlags |= FLAGBIT(l);
			}
		}
	else
	/***********************************************************************/
	// See if 'value' command is specified
	/***********************************************************************/
	if (!lstrcmpi(lpKey, "value"))
		{ // Got a 'value' command
	
		/***********************************************************************/
		// Get 'value' argument; any number of values to be OR'd together
		/***********************************************************************/
		while (--nValues >= 0)
			{
			lpValue = GetNextValue( &lpValues );
			l = latol( lpValue, &bError );
			if ( bError || l < 0 )
				{
				Print("'%s'\n Bad value at '%s'", lpEntry, lpValue);
				break;
				}
			lpShots->lValue |= l;
			}
		}
	else
	/***********************************************************************/
	// See if 'set' command is specified
	/***********************************************************************/
	if (!lstrcmpi(lpKey, "set"))
	{ // Got a 'set' command
	
		/***********************************************************************/
		// Get 'set' argument; any register number from 1 to 32
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		long Reg = latol( lpValue, &bError );
		if ( bError || (Reg < 1) || (Reg > MAX_REGISTERS) )
			Print("'%s'\n Bad set register at '%s'", lpEntry, lpValue);
        else
        {
        	long lValue;
        	
			// Get the value
			lpValue = GetNextValue( &lpValues );
			if (!lstrcmpi(lpValue, "++"))  {
				lValue = 9999999;
				bError = FALSE;
			}	
			else
			if (!lstrcmpi(lpValue, "--"))  {
				lValue = -9999999;
				bError = FALSE;
			}	
			else		
				lValue = latol( lpValue, &bError );
				
			if ( bError )
				Print("'%s'\n Bad set value at '%s'", lpEntry, lpValue);
			else  {
				lpShots->lRegNum   = Reg;
				lpShots->lRegValue = lValue;
			}	
		}
	}
	else
	if (!lstrcmpi(lpKey, "prompt")) // Check for a 'prompt' command
	{
		/***********************************************************************/
		// Get 'prompt' arguments; Shot ID and timer value in seconds
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		l = GetShotID(lpEntry, lpValue);
		if ( l < 0 )
		{
			Print("'%s'\n Bad prompt shot at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lPromptShot = l;
		
		lpValue = GetNextValue(&lpValues); 
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad prompt time at '%s'", lpEntry, lpValue);
			return(fError);
		}
		lpShots->lPromptTime = l;
	}
	else
	/***********************************************************************/
	// Check for extra commands
	/***********************************************************************/
	{
		Print("'%s'\n Bad command at the end of line at '%s'", lpEntry, lpKey);
		return(fError);
	}
	return(TRUE);
}

/***********************************************************************/
static void Video_GetShotTableFileName(ITEMID id, LPSTR lpFileName)
/***********************************************************************/
{
	GetModuleFileName( GetApp()->GetInstance(), lpFileName, MAX_FNAME_LEN );
	StripFile( lpFileName );
	STRING szName;
	wsprintf( szName, "video%03d.tbl", id );
	lstrcat( lpFileName, szName );
//	lstrcat( lpFileName, "videojmp.tbl" );
}

/***********************************************************************/
static LPSHOT Video_LoadShotTable(ITEMID id, LPINT lpNumShots)
/***********************************************************************/
{
	HFILE hFile;
	HPTR hpData;
	UINT uBytes;
	LPSHOT lpShots;
	FNAME szFileName;
	FNAME szModuleName;

	Video_GetShotTableFileName(id, szFileName);
	GetModuleFileName(GetApp()->GetInstance(), szModuleName, sizeof(szModuleName));
	 
	//int iResult;
	//if (CompareFileTimes(szModuleName, szFileName, &iResult) && (iResult > 0))
	//{
	//	FileDelete(szFileName);
	//	return(NULL);
	//}

	hFile = _lopen(szFileName, OF_READ);
	if ( hFile == HFILE_ERROR )
		return(NULL);
	DWORD dwSize = _llseek(hFile, 0L, 2);
	_llseek(hFile, 0L, 0);
	*lpNumShots = (int)(dwSize / (DWORD)sizeof(SHOT));
	// Allocate the shot table
	if ( !(hpData = (HPTR)AllocX( (long)*lpNumShots * sizeof(SHOT), GMEM_ZEROINIT|GMEM_SHARE )) )
	{
		_lclose(hFile);
		return(NULL);
	}

	lpShots = (LPSHOT)hpData;
	while (dwSize)
	{
		#ifdef WIN32
		uBytes = INT_MAX;
		#else
		uBytes = 32768; // 32K chunks to we don't span 64K
		#endif
		if (uBytes > dwSize)
			uBytes = (UINT)dwSize;
		if (_lread(hFile, (LPVOID)hpData, uBytes) == HFILE_ERROR)
		{
			Print("Error write to file: '%s'", (LPSTR)szFileName);
			FreeUp(lpShots);
			lpShots = NULL;
			break;
		}
		dwSize -= uBytes;
		hpData += uBytes;
	}
	_lclose(hFile);
	#ifndef WIN32
	long lRem = 65536L % sizeof(SHOT);
	if (lRem) // extra amount to prevent straddling
	{
		--*lpNumShots; // there is really one less shot
		LPTR lp = (LPTR)lpShots; 
		lp += lRem; // bump pointer to prevent straddling
		lpShots = (LPSHOT)lp;
	}
	#endif
	return(lpShots);
}

/***********************************************************************/
BOOL Video_DumpShots(ITEMID id, LPSHOT lpShots, int iNumShots)
/***********************************************************************/
{     
	HFILE hFile;
	FNAME szFileName;
	HPTR hpData;
	UINT uBytes;

	Video_GetShotTableFileName(id, szFileName);

	hFile = _lcreat(szFileName, 0);
	if ( hFile == HFILE_ERROR )
		return(FALSE);
	
	// TEMPORARY UPDATE OF TABLE
	//NEWSHOT NewShot;
	//int i;
	//for(i=0; i<iNumShots; i++)
	//{
	//	memcpy(&NewShot, &lpShots[i], sizeof(SHOT));
	//	NewShot.EventRange.nStart = 0;
	//	NewShot.EventRange.nEnd = 0;
	//	_lwrite(hFile, (LPCSTR)&NewShot, sizeof(NEWSHOT));
	//}
	//_lclose(hFile);
	//return TRUE;
	// END TEMPORARY

	hpData = (HPTR)lpShots;
	#ifndef WIN32
	long lRem = 65536L % sizeof(SHOT);
	if (lRem) // extra amount to prevent segment straddling
	{
		++iNumShots; // one more structure needed for straddling
		hpData -= lRem; // get pointer back to start of buffer
	}
	#endif
	DWORD dwSize = (DWORD)sizeof(SHOT) * (DWORD)iNumShots;
	while (dwSize)
	{
		#ifdef WIN32
		uBytes = INT_MAX;
		#else
		uBytes = 32768; // 32K chunks to we don't span 64K
		#endif
		if (uBytes > dwSize)
			uBytes = (UINT)dwSize;
		if (_lwrite(hFile, (LPCSTR)hpData, uBytes) == HFILE_ERROR)
		{
			Print("Error write to file: '%s'", szFileName);
			_lclose(hFile);
			return(FALSE);
		}
		dwSize -= (DWORD)uBytes;
		hpData += uBytes;
	}
	_lclose(hFile);
	return(TRUE);
}

/***********************************************************************/
void Video_DumpShotTable(ITEMID id, LPSHOT lpShots, int iNumShots)
/***********************************************************************/
{
	FNAME szFileName;

	if ( !lpShots || !iNumShots )
		return;
	Video_GetShotTableFileName(id, szFileName);
	if (!FileExistsExpand(szFileName, NULL))
		Video_DumpShots(id, lpShots, iNumShots);
}

/***********************************************************************/
LPSHOT Video_GetShotData( ITEMID id, LPINT lpiNumShots, LPBOOL lpfRemap )
/***********************************************************************/
{
	ITEMID ResID;
	HINSTANCE hInst;
	HRSRC hResource;
	HGLOBAL hData;
	HPTR hpShotData;
	HPTR hpResData;
	LPSHOT lpShots, lpShot;
	DWORD dwSize, dwCurrentSize;
	int iNumShots, iCount;
	static BOOL bInit;


	hInst = GetApp()->GetInstance();
		
	// We have to know there's a shot table, before we look for a binary
	if ( !(hResource = FindResource( hInst, MAKEINTRESOURCE(id), RT_RCDATA )) )
		return( NULL );

	if (lpShots = Video_LoadShotTable(id, &iNumShots))
	{ // See if there's a binary
		CVTableParser parser(NULL);
		lpShot = lpShots;
		iCount = iNumShots;
		while (--iCount >= 0)
		{
			lpShot->aShotID = (ATOM)parser.MakeShotID(lpShot->szShotName,!bInit);
			++lpShot;
		}
		*lpfRemap = FALSE;
	}
	else
	{ // Load the ascii table and parse it
		// Look for multiple resources and concatenate them.
		
		// Find total size of all resources
		dwSize = 0;
		ResID = id;
		while (hResource = FindResource( hInst, MAKEINTRESOURCE(ResID), RT_RCDATA ))
		{
			dwSize += SizeofResource( hInst, hResource );
			++ResID;
		}
			
		if (dwSize == 0)
			return(NULL);
		
		if ( !(hpShotData = (HPTR)AllocX( dwSize, GMEM_ZEROINIT )) )
			return(NULL);
        
        // Now get each chunk and copy it into lpShotData
        ResID = id;
        HPTR hpData = hpShotData;
		while (hResource = FindResource( hInst, MAKEINTRESOURCE(ResID), RT_RCDATA ))
		{
			if ( !(hData = LoadResource( hInst, hResource )) )
			{
				FreeUp(hpShotData);
				return( NULL );
			}	
				
			if ( !(hpResData = (HPTR)LockResource( hData )) )
			{
				FreeResource( hData );
				FreeUp(hpShotData);
				return( NULL );
			}
			
			dwCurrentSize = SizeofResource( hInst, hResource );
			hmemcpy(hpData, hpResData, dwCurrentSize);
			hpData += dwCurrentSize;
 
 			while (*hpData == 0)	// Backup the ptr to remove the alignment padding
 				--hpData;

 			++hpData;				// Advance to the next available byte
			
            UnlockResource(hData);
			FreeResource(hData);
						
			++ResID;
	    }    
        
			
		CVTableParser parser((LPSTR)hpShotData);
		iNumShots = parser.GetNumEntries();
		// Allocate the shot table
	#ifdef WIN32
		if ( !(lpShots = (LPSHOT)AllocX( (long)iNumShots * sizeof(SHOT), GMEM_ZEROINIT|GMEM_SHARE )) )
		{
			FreeUp( hpShotData );
			return( NULL );
		}
	#else
		{
			LPTR lp;
			long lRem;
			int iShots;
	
			// For Win16 we make sure none of the structures straddle
			// a segment boundary.  This is done by allocating one more
			// structure than is needed and incrementing the initial
			// pointer so that none straddle.
			lRem = 65536L % sizeof(SHOT); // amount to increment pointer
			iShots = iNumShots;
			if (lRem)
				++iShots; // allocate one more structure than needed
	
			if ( !(lp = AllocX( (long)iShots * sizeof(SHOT), GMEM_ZEROINIT|GMEM_SHARE )) )
			{
				FreeUp( hpShotData );
				return( NULL );
			}
			
			lp += lRem; // increment pointer to prevent straddling
			lpShots = (LPSHOT)lp;
		}
	#endif
		
		// Process the resource data and stuff the shot table
		parser.ParseTable((DWORD)lpShots);
		
		FreeUp( hpShotData );
		*lpfRemap = TRUE;
	}
	
	if ( lpiNumShots )
		*lpiNumShots = iNumShots;
	
	bInit = YES;
	return( lpShots );
}
