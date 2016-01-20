/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                  gallery.c

******************************************************************************
******************************************************************************

  Description:  Gallery, Who Are You, and Slideshow INI functions
 Date Created:  01/11/94
       Author:  Stephen M. Suggs

*****************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <share.h>
#include <errno.h>
#include <ctype.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "gallery.h"

extern HINSTANCE hInstAstral;

#ifdef _MAC
  #define szBackSlash ":"
  int  iEntrySize;			// number of bytes in the entry buffer
  int  iEntryPos;			// The position within the buffer to read next entry name
  char szEntries[1000];		// all entries for the specified section.
  STRING szSection;			// The current section these entries are from
  FNAME szINIFileName;		// This the current INI filename
#else
  #define szBackSlash "\\"
#endif

/***********************************************************************/
// Saves the entry into the gallery .INI file in the "Files" section
//   in the form - lpFilename=lpTitle
// if lpTitle = NULL, the title will be set to "Untitled"
// returns: 1 if successful, or 0 it not
/***********************************************************************/
BOOL SaveToIniEntry (LPSTR lpFileName, LPSTR lpTitle)
{
	STRING szIniName;
	STRING szSection;
	STRING szTitle;
	STRING szUntitled;
	FNAME  szPath;
  
	if (lpFileName == NULL)
		return FALSE;
	if (! LoadString (hInstAstral, IDS_GALLERY_INIFILE, szIniName, sizeof (szIniName)) )
		return FALSE;
	if (! LoadString (hInstAstral, IDS_GALLERY_FILE_SECTION, szSection, sizeof (szSection)) )
		return FALSE;
	if (! LoadString (hInstAstral, IDS_UNTITLED, szUntitled, sizeof (szUntitled)) )
		return FALSE;
	if (lpTitle == NULL)
		lstrcpy (szTitle, szUntitled);
	else
		lstrcpy (szTitle, lpTitle);

	lstrcpy (szPath, Control.WorkPath);
  
	// if a user is signed in, append the name to the path
	if (Control.CurrentUser[0] != 0)
	{
		lstrcat (szPath, Control.CurrentUser);
#ifdef _MAC
		// ALL INI files MUST be located in System:Preferences folder 
		// Make the filename unique by adding a dash (-)		
		lstrcat (szPath, "-");
#else
		lstrcat (szPath, szBackSlash);
#endif
	}
	lstrcat (szPath, szIniName);
      
	if (! (WritePrivateProfileString (szSection, lpFileName, szTitle, szPath)))
	{
		return FALSE;
	}
	return TRUE;
}


/***********************************************************************/
// Gets the next available PICxx filename to use for this INI file
// returns: 1 if successful, or 0 it not
/***********************************************************************/
BOOL GetNextIniFilename (LPSTR lpFileName)
{
	STRING szIniName;
	STRING szSection;
	STRING szPrefix;
	STRING szExtension;
	STRING szNum;
	FNAME  szIniPath;
	int    iFileNum = 0;

	if (! LoadString (hInstAstral, IDS_GALLERY_INIFILE, szIniName, sizeof (szIniName)) )
		return FALSE;
	if (! LoadString (hInstAstral, IDS_GALLERY_FILE_SECTION, szSection, sizeof (szSection)) )
		return FALSE;
	if (! LoadString (hInstAstral, IDS_GALLERY_PREFIX, szPrefix, sizeof (szPrefix)) )
		return FALSE;
	if (! LoadString (hInstAstral, IDS_GALLERY_EXT, szExtension, sizeof (szExtension)) )
		return FALSE;

    lstrcpy (lpFileName, Control.WorkPath);
	lstrcpy (szIniPath, Control.WorkPath);
	// if a user is signed in, append the name to the path
	if (Control.CurrentUser[0] != 0)
	{
		lstrcat (szIniPath, Control.CurrentUser);
#ifdef _MAC
		// ALL INI files MUST be located in System:Preferences folder 
		// Make the filename unique by adding a dash (-)		
		lstrcat (szIniPath, "-");
#else
		lstrcat (szIniPath, szBackSlash);
#endif
		lstrcat (lpFileName, Control.CurrentUser);
		lstrcat (lpFileName, szBackSlash);
	}
	lstrcat (szIniPath, szIniName);
    
	iFileNum = GetNextIniEntry (szIniPath, szSection);
	wsprintf (szNum, "%d", iFileNum);
	lstrcat (lpFileName, szPrefix);
	lstrcat (lpFileName, szNum);
	lstrcat (lpFileName, szExtension);
	return TRUE;
}


/***********************************************************************/
// returns the next integer to use in creating a unique filename
/***********************************************************************/
int GetNextIniEntry (FNAME IniName, LPSTR lpSection)
{
	INIPTR hIni;
	STRING szBuff;
	int iFileNum = 0;
	int iRetCnt = 0;
	int iTmp;

	if ((hIni = FindFirstINIKey (IniName, lpSection, szBuff, sizeof(szBuff))) != NULL)
	{
		iFileNum = _ConvertFileNum (szBuff);
		while ((FindNextINIKey (hIni, szBuff, sizeof(szBuff))) != NULL)
		{
			iTmp = _ConvertFileNum (szBuff);
			if (iTmp > iFileNum)
  				iFileNum = iTmp;
		}
	}
	// iFileNum will be equal to the highest number used so bump it by one
	return ++iFileNum;
}


/***********************************************************************/
// Gets the number of entries in the CGALLERY.INI file
// returns: number of entries or zero if no file
/***********************************************************************/
int far GetNumberIniEntries (void)
{
	INIPTR hIni;
	STRING szIniName;
	STRING szSection;
	STRING szBuff;
	FNAME  szPath;
	int    iFileCnt = 0;

	if (! LoadString (hInstAstral, IDS_GALLERY_INIFILE, szIniName, sizeof (szIniName)) )
		return 0;
	if (! LoadString (hInstAstral, IDS_GALLERY_FILE_SECTION, szSection, sizeof (szSection)) )
		return 0;

	lstrcpy (szPath, Control.WorkPath);
	// if a user is signed in, append the name to the path
	if (Control.CurrentUser[0] != 0)
	{
		lstrcat (szPath, Control.CurrentUser);
#ifdef _MAC
		// ALL INI files MUST be located in System:Preferences folder 
		// Make the filename unique by adding a dash (-)		
		lstrcat (szPath, "-");
#else
		lstrcat (szPath, szBackSlash);
#endif
	}
	lstrcat (szPath, szIniName);
    
	if ((hIni = FindFirstINIKey (szPath, szSection, szBuff, sizeof(szBuff))) != NULL)
	{
		iFileCnt++;
		while ((FindNextINIKey (hIni, szBuff, sizeof(szBuff))) != NULL)
		{
			iFileCnt++;
		}
	}
	if (iFileCnt > 32767)
		iFileCnt = 32767;
	return iFileCnt;
}


/***********************************************************************/
// Gets the filename referenced by Index from CGALLERY.INI
// returns: TRUE if successfull and puts filename in lpFileName
/***********************************************************************/
BOOL GetFileNameByIndex (LPSTR lpFileName, int Index)
{
	INIPTR hIni;
	STRING szIniName;
	STRING szSection;
	STRING szBuff;
	STRING szScratch;
	FNAME  szPath;
	int    iIdx;
	int    iCnt = 0;

	if (! LoadString (hInstAstral, IDS_GALLERY_INIFILE, szIniName, sizeof (szIniName)) )
		return 0L;
	if (! LoadString (hInstAstral, IDS_GALLERY_FILE_SECTION, szSection, sizeof (szSection)) )
		return 0L;

	lstrcpy (szPath, Control.WorkPath);

	// if a user is signed in, append the name to the path
	if (Control.CurrentUser[0] != 0)
	{
		lstrcat (szPath, Control.CurrentUser);
#ifdef _MAC
		// ALL INI files MUST be located in System:Preferences folder 
		// Make the filename unique by adding a dash (-)		
		lstrcat (szPath, "-");
#else
		lstrcat (szPath, szBackSlash);
#endif
	}
	lstrcat (szPath, szIniName);
	clr ((LPTR)szScratch, sizeof(STRING));
    
	if ((hIni = FindFirstINIKey (szPath, szSection, szBuff, sizeof(szBuff))) != NULL)
	{
		if (iCnt == Index)
		{
			for (iIdx = 0 ; szBuff[iIdx] != '=' ; iIdx++)
				szScratch[iIdx] = szBuff[iIdx];

			szScratch[iIdx] = 0;
#ifdef _MAC
			lstrcpy(lpFileName+1, szScratch);
			lpFileName[0] = ':';
#else
			lstrcpy(lpFileName, szScratch);
#endif		
      		_CloseINIFile (hIni);
			return TRUE;
		}
		iCnt++;
		while ((FindNextINIKey (hIni, szBuff, sizeof(szBuff))) != NULL)
		{
			if (iCnt == Index)
			{
				for (iIdx = 0 ; szBuff[iIdx] != '=' ; iIdx++)
					szScratch[iIdx] = szBuff[iIdx];

				szScratch[iIdx] = 0;
#ifdef _MAC
			lstrcpy(lpFileName+1, szScratch);
			lpFileName[0] = ':';
#else
			lstrcpy(lpFileName, szScratch);
#endif		
      			_CloseINIFile (hIni);
				return TRUE;
			}
			iCnt++;
		}
	}
	return FALSE;
}


/***********************************************************************/
// Deletes the entry in the gallery .INI file in the "Files" section
//   and "Slideshow" section and the file specified by lpFileName.
// returns: 1 if successful, or 0 it not
/***********************************************************************/
BOOL DeleteFileAndIniEntry (LPSTR lpFileName)
{
	STRING	szIniName;
	STRING	szSection;
	STRING	szNumber;
	STRING	szSlideEntry;
	STRING	szScratch;
	FNAME	szIniPath;
	char	* src;
	char	* dst;
	INIPTR	hIni;
  
	if (lpFileName == NULL)
		return FALSE;
	if (! LoadString (hInstAstral, IDS_GALLERY_INIFILE, szIniName, sizeof (STRING)) )
		return FALSE;
	if (! LoadString (hInstAstral, IDS_GALLERY_FILE_SECTION, szSection, sizeof (STRING)) )
		return FALSE;

	lstrcpy (szIniPath, Control.WorkPath);
	// if a user is signed in, append the name to the path
	if (Control.CurrentUser[0] != 0)
	{
		lstrcat (szIniPath, Control.CurrentUser);
#ifdef _MAC
		// ALL INI files MUST be located in System:Preferences folder 
		// Make the filename unique by adding a dash (-)		
		lstrcat (szIniPath, "-");
#else
		lstrcat (szIniPath, szBackSlash);
#endif
	}
	lstrcat (szIniPath, szIniName);

	FileDelete (lpFileName);
    
	if (! (WritePrivateProfileString (szSection, lpFileName, NULL, szIniPath)))
	{
		return FALSE;
	}

	//delete the Slideshow entry also
	LoadString (hInstAstral, IDS_SLIDE_FILE_SECTION, szSection, sizeof (STRING));
	clr ((LPTR)szNumber, sizeof(STRING));
	clr ((LPTR)szScratch, sizeof(STRING));
	lstrcpy (szScratch, lpFileName);
	src = (char *)szScratch;
	dst = szNumber;
	src = strchr (szScratch, '.');
	src--;
	while (isdigit (*src))
	{
		src--;
	}
	src++;
	while (isdigit (*src))
	{
		*dst = *src;
		dst++;
		src++;
	}
	if ((hIni = FindFirstINIKey (szIniPath, szSection, szSlideEntry,
	  sizeof(szSlideEntry))) != NULL)
	{
		src = strchr (szSlideEntry, '=');
		*src = 0;
		src++;
		if (! lstrcmp (src, szNumber))
		{
      		_CloseINIFile (hIni);
		}
		else
		{
			while ((FindNextINIKey (hIni, szSlideEntry,
			  sizeof(szSlideEntry))) != NULL)
			{
				src = strchr (szSlideEntry, '=');
				*src = 0;
				src++;
				if (! lstrcmp (src, szNumber))
				{
      				_CloseINIFile (hIni);
					break;
				}
			}
		}
	}
	if (! (WritePrivateProfileString (szSection, szSlideEntry, NULL, szIniPath)))
	{
		return FALSE;
	}
	return TRUE;
}


/***********************************************************************/
// Gets the title associated with this filename
// returns: 1 if successful, or 0 it not
/***********************************************************************/
BOOL GetFilenameTitle (LPSTR lpFileName, LPSTR lpTitle)
{
	INIPTR	hIni;
	STRING	szIniName;
	STRING	szSection;
	char	szBuff[128];	// STRING wasn't quite big enough....
	FNAME	szIniPath;
	char  *	ptr;
  
	if (lpFileName == NULL)
		return FALSE;
	if (! LoadString (hInstAstral, IDS_GALLERY_INIFILE, szIniName, sizeof (szIniName)) )
		return FALSE;
	if (! LoadString (hInstAstral, IDS_GALLERY_FILE_SECTION, szSection, sizeof (szSection)) )
		return FALSE;

	lstrcpy (szIniPath, Control.WorkPath);
	// if a user is signed in, append the name to the path
	if (Control.CurrentUser[0] != 0)
	{
		lstrcat (szIniPath, Control.CurrentUser);
#ifdef _MAC
		// ALL INI files MUST be located in System:Preferences folder 
		// Make the filename unique by adding a dash (-)		
		lstrcat (szIniPath, "-");
#else
		lstrcat (szIniPath, szBackSlash);
#endif
	}
	lstrcat (szIniPath, szIniName);

	clr ((LPTR)szBuff, sizeof(szBuff));
	if ((hIni = FindFirstINIKey (szIniPath, szSection, szBuff, sizeof(szBuff))) != NULL)
	{
		ptr = strchr (szBuff, '=');
		*ptr = 0;
		if (! lstrcmp(lpFileName, szBuff))
		{
			ptr++;
			lstrcpy (lpTitle, ptr);
      		_CloseINIFile (hIni);
			return TRUE;
		}
		while ((FindNextINIKey (hIni, szBuff, sizeof(szBuff))) != NULL)
		{
			ptr = strchr (szBuff, '=');
			*ptr = 0;
			if (! lstrcmp(lpFileName, szBuff))
			{
				ptr++;
				lstrcpy (lpTitle, ptr);
      			_CloseINIFile (hIni);
				return TRUE;
			}
		}
	}
	return FALSE;
}


/***********************************************************************/
// Sets the title associated with this filename
// returns: 1 if successful, or 0 it not
/***********************************************************************/
BOOL SetFilenameTitle (LPSTR lpFileName, LPSTR lpTitle)
{
	STRING szIniName;
	STRING szSection;
	FNAME  szIniPath;
  
	if (lpFileName == NULL)
		return FALSE;
	if (! LoadString (hInstAstral, IDS_GALLERY_INIFILE, szIniName,
	  sizeof (szIniName)) )
		return FALSE;
	if (! LoadString (hInstAstral, IDS_GALLERY_FILE_SECTION, szSection,
	  sizeof (szSection)) )
		return FALSE;

	lstrcpy (szIniPath, Control.WorkPath);
	// if a user is signed in, append the name to the path
	if (Control.CurrentUser[0] != 0)
	{
		lstrcat (szIniPath, Control.CurrentUser);
#ifdef _MAC
		// ALL INI files MUST be located in System:Preferences folder 
		// Make the filename unique by adding a dash (-)		
		lstrcat (szIniPath, "-");
#else
		lstrcat (szIniPath, szBackSlash);
#endif
	}
	lstrcat (szIniPath, szIniName);

	if (! (WritePrivateProfileString (szSection, lpFileName, lpTitle,
	  szIniPath)))
	{
		return FALSE;
	}
	return TRUE;
}


/***********************************************************************/
// Gets the number of users from the user ini file
// returns: number of entries or zero if no file
/***********************************************************************/
int far GetNumberOfUsers (void)
{
	INIPTR hIni;
	STRING szIniName;
	STRING szSection;
	STRING szBuff;
	FNAME  szPath;
	int    iCnt = 0;

	if (! LoadString (hInstAstral, IDS_USER_INIFILE, szIniName, sizeof (szIniName)) )
		return 0;
	if (! LoadString (hInstAstral, IDS_USER_FILE_SECTION, szSection, sizeof (szSection)) )
		return 0;

	lstrcpy (szPath, Control.WorkPath);
	lstrcat (szPath, szIniName);
		    
	if ((hIni = FindFirstINIKey (szPath, szSection, szBuff, sizeof(szBuff))) != NULL)
	{
		iCnt++;
		while ((FindNextINIKey (hIni, szBuff, sizeof(szBuff))) != NULL)
		{
			iCnt++;
		}
	}
	if (iCnt > 32767)
		iCnt = 32767;
	return iCnt;
}


/***********************************************************************/
// Gets the number of users from the user ini file
// returns: number of entries or zero if no file
/***********************************************************************/
BOOL GetUserNameString (int Index, LPSTR lpBuff)
{
	INIPTR hIni;
	STRING szIniName;
	STRING szSection;
	STRING szBuff;
	STRING szScratch;
	FNAME  szPath;
	int    iIdx;
	int    iCnt = 0;

	if (! LoadString (hInstAstral, IDS_USER_INIFILE, szIniName, sizeof (szIniName)) )
		return FALSE;
	if (! LoadString (hInstAstral, IDS_USER_FILE_SECTION, szSection, sizeof (szSection)) )
		return FALSE;

	lstrcpy (szPath, Control.WorkPath);
	lstrcat (szPath, szIniName);
	clr ((LPTR)szScratch, sizeof(STRING));
    
	if ((hIni = FindFirstINIKey (szPath, szSection, szBuff, sizeof(szBuff))) != NULL)
	{
		if (iCnt == Index)
		{
			for (iIdx = 0 ; szBuff[iIdx] != '=' ; iIdx++)
				szScratch[iIdx] = szBuff[iIdx];

			szScratch[iIdx] = 0;
			lstrcpy(lpBuff, szScratch);
      		_CloseINIFile (hIni);
			return TRUE;
		}
		iCnt++;
		while ((FindNextINIKey (hIni, szBuff, sizeof(szBuff))) != NULL)
		{
			if (iCnt == Index)
			{
				for (iIdx = 0 ; szBuff[iIdx] != '=' ; iIdx++)
					szScratch[iIdx] = szBuff[iIdx];

				szScratch[iIdx] = 0;
				lstrcpy(lpBuff, szScratch);
      			_CloseINIFile (hIni);
				return TRUE;
			}
			iCnt++;
		}
	}
	return FALSE;
}


/***********************************************************************/
// Gets the users name from the user ini file given the directory name
// returns: number of entries or zero if no file
/***********************************************************************/
BOOL GetUserNameByDir (LPSTR lpDir, LPSTR lpBuff)
{
	INIPTR	hIni;
	STRING	szIniName;
	STRING	szSection;
	STRING	szBuff;
	FNAME	szPath;
	char   *ptr;

	if (! LoadString (hInstAstral, IDS_USER_INIFILE, szIniName, sizeof (szIniName)) )
		return FALSE;
	if (! LoadString (hInstAstral, IDS_USER_FILE_SECTION, szSection, sizeof (szSection)) )
		return FALSE;

	lstrcpy (szPath, Control.WorkPath);
	lstrcat (szPath, szIniName);
	clr ((LPTR)szBuff, sizeof(STRING));
    
	if ((hIni = FindFirstINIKey (szPath, szSection, szBuff, sizeof(szBuff))) != NULL)
	{
		ptr = strchr (szBuff, '=');
		*ptr = 0;
		ptr++;
		if (! lstrcmp (ptr, lpDir))
		{
			lstrcpy(lpBuff, szBuff);
      		_CloseINIFile (hIni);
			return TRUE;
		}
		while ((FindNextINIKey (hIni, szBuff, sizeof(szBuff))) != NULL)
		{
			ptr = strchr (szBuff, '=');
			*ptr = 0;
			ptr++;
			if (! lstrcmp (ptr, lpDir))
			{
				lstrcpy(lpBuff, szBuff);
      			_CloseINIFile (hIni);
				return TRUE;
			}
		}
	}
	return FALSE;
}


/***********************************************************************/
// Deletes a user from the user ini file
// returns: TRUE on success or FALSE if not
/***********************************************************************/
BOOL DeleteUserName (LPSTR lpName)
{
	STRING szIniName;
	STRING szSection;
	FNAME  szPath;

	if (! LoadString (hInstAstral, IDS_USER_INIFILE, szIniName, sizeof (szIniName)) )
		return FALSE;
	if (! LoadString (hInstAstral, IDS_USER_FILE_SECTION, szSection, sizeof (szSection)) )
		return FALSE;

	lstrcpy (szPath, Control.WorkPath);
	lstrcat (szPath, szIniName);

	if (! (WritePrivateProfileString (szSection, lpName, NULL, szPath)))
	{
		return FALSE;
	}
	return TRUE;
}


/***********************************************************************/
// Saves the entry into the USERLIST.INI file in the "Users" section
//   in the form - lpUsername=lpTitle
// if lpFullName = NULL, the title will be set to "Untitled"
// returns: 1 if successful, or 0 it not
/***********************************************************************/
BOOL SaveUsername (LPSTR lpUserName, LPSTR lpFullName)
{
	STRING szIniName;
	STRING szSection;
	STRING szTitle;
	STRING szUntitled;
	FNAME  szPath;
  
	if (lpUserName == NULL)
		return FALSE;
	if (! LoadString (hInstAstral, IDS_USER_INIFILE, szIniName, sizeof (szIniName)) )
		return FALSE;
	if (! LoadString (hInstAstral, IDS_USER_FILE_SECTION, szSection, sizeof (szSection)) )
		return FALSE;

	if (! LoadString (hInstAstral, IDS_UNTITLED, szUntitled, sizeof (szUntitled)) )
		return FALSE;
	if (lpFullName == NULL)
		lstrcpy (szTitle, szUntitled);
	else
		lstrcpy (szTitle, lpFullName);

	lstrcpy (szPath, Control.WorkPath);
	lstrcat (szPath, szIniName);
      
	if (! (WritePrivateProfileString (szSection, lpUserName, szTitle, szPath)))
	{
		return FALSE;
	}
	return TRUE;
}


/***********************************************************************/
// Gets the Directory associated with this Username
// returns: 1 if successful, or 0 it not
/***********************************************************************/
BOOL GetUserDir (LPSTR lpUsername, LPSTR lpDir)
{
	INIPTR hIni;
	STRING szIniName;
	STRING szSection;
	STRING szBuff;
	FNAME  szIniPath;
	char * ptr;
  
	if (lpUsername == NULL)
		return FALSE;
	if (! LoadString (hInstAstral, IDS_USER_INIFILE, szIniName, sizeof (szIniName)) )
		return FALSE;
	if (! LoadString (hInstAstral, IDS_USER_FILE_SECTION, szSection, sizeof (szSection)) )
		return FALSE;

	lstrcpy (szIniPath, Control.WorkPath);
	lstrcat (szIniPath, szIniName);

	if ((hIni = FindFirstINIKey (szIniPath, szSection, szBuff, sizeof(szBuff))) != NULL)
	{
		ptr = strchr (szBuff, '=');
		*ptr = 0;
		if (! lstrcmp(lpUsername, szBuff))
		{
			ptr++;
			lstrcpy (lpDir, ptr);
      		_CloseINIFile (hIni);
			return TRUE;
		}
		while ((FindNextINIKey (hIni, szBuff, sizeof(szBuff))) != NULL)
		{
			ptr = strchr (szBuff, '=');
			*ptr = 0;
			if (! lstrcmp(lpUsername, szBuff))
			{
				ptr++;
				lstrcpy (lpDir, ptr);
      			_CloseINIFile (hIni);
				return TRUE;
			}
		}
	}
	return FALSE;
}


/***********************************************************************/
// Saves the entry into the gallery .INI file in the "Slideshow" section
//   in the form - lpFilename=Untitled
// returns: 1 if successful, or 0 it not
/***********************************************************************/
BOOL SaveToSlideshow (LPSTR lpFileName, LPSTR lpNum)
{
	STRING szIniName;
	STRING szSection;
	STRING szNum;
	FNAME  szPath;
  
	if (lpFileName == NULL)
		return FALSE;
	if (! LoadString (hInstAstral, IDS_GALLERY_INIFILE, szIniName, sizeof (STRING)) )
		return FALSE;

	if (! LoadString (hInstAstral, IDS_SLIDE_FILE_SECTION, szSection, sizeof (STRING)) )
		return FALSE;

	if (lpNum == NULL)
		lstrcpy (szNum, "0");
	else
		lstrcpy (szNum, lpNum);

	lstrcpy (szPath, Control.WorkPath);
  
	// if a user is signed in, append the name to the path
	if (Control.CurrentUser[0] != 0)
	{
		lstrcat (szPath, Control.CurrentUser);
#ifdef _MAC
		// ALL INI files MUST be located in System:Preferences folder 
		// Make the filename unique by adding a dash (-)		
		lstrcat (szPath, "-");
#else
		lstrcat (szPath, szBackSlash);
#endif
	}
	lstrcat (szPath, szIniName);
      
	if (! (WritePrivateProfileString (szSection, lpFileName, szNum, szPath)))
	{
		return FALSE;
	}
	return TRUE;
}


/***********************************************************************/
// Saves slide show entry number
// returns: 1 if successful, or 0 it not
/***********************************************************************/
BOOL SaveSlideNumber (LPSTR lpNumber)
{
	STRING szIniName;
	STRING szSection;
	STRING szEntry;
	FNAME  szPath;
  
	if (lpNumber == NULL)
		return FALSE;
	if (! LoadString (hInstAstral, IDS_GALLERY_INIFILE, szIniName, sizeof (STRING)) )
		return FALSE;

	if (! LoadString (hInstAstral, IDS_SLIDE_FILE2_SECTION, szSection, sizeof (STRING)) )
		return FALSE;

	if (! LoadString (hInstAstral, IDS_SLIDE_FILE2_ENTRY, szEntry, sizeof (STRING)) )
		return FALSE;

	lstrcpy (szPath, Control.WorkPath);
  
	// if a user is signed in, append the name to the path
	if (Control.CurrentUser[0] != 0)
	{
		lstrcat (szPath, Control.CurrentUser);
#ifdef _MAC
		// ALL INI files MUST be located in System:Preferences folder 
		// Make the filename unique by adding a dash (-)		
		lstrcat (szPath, "-");
#else
		lstrcat (szPath, szBackSlash);
#endif
	}
	lstrcat (szPath, szIniName);
      
	if (! (WritePrivateProfileString (szSection, szEntry, lpNumber, szPath)))
	{
		return FALSE;
	}
	return TRUE;
}


/***********************************************************************/
// Get new slide show entry number
// returns: 1 if successful, or 0 it not
/***********************************************************************/
BOOL GetSlideNumber (LPSTR lpNumber)
{
	STRING szIniName;
	STRING szSection;
	STRING szEntry;
	STRING szBuff;
	FNAME  szPath;
  
	if (lpNumber == NULL)
		return FALSE;
	if (! LoadString (hInstAstral, IDS_GALLERY_INIFILE, szIniName, sizeof(STRING)) )
		return FALSE;

	if (! LoadString (hInstAstral, IDS_SLIDE_FILE2_SECTION, szSection, sizeof(STRING)) )
		return FALSE;

	if (! LoadString (hInstAstral, IDS_SLIDE_FILE2_ENTRY, szEntry, sizeof(STRING)) )
		return FALSE;

	lstrcpy (szPath, Control.WorkPath);
  
	// if a user is signed in, append the name to the path
	if (Control.CurrentUser[0] != 0)
	{
		lstrcat (szPath, Control.CurrentUser);
#ifdef _MAC
		// ALL INI files MUST be located in System:Preferences folder 
		// Make the filename unique by adding a dash (-)		
		lstrcat (szPath, "-");
#else
		lstrcat (szPath, szBackSlash);
#endif
	}
	lstrcat (szPath, szIniName);
      
	if (! (GetPrivateProfileString (szSection, szEntry, "", szBuff,
	  sizeof(STRING), szPath)))
	{
		return FALSE;
	}
	if (szBuff[0] != 0)
		lstrcpy (lpNumber, szBuff);

	return TRUE;
}


/***********************************************************************/
// Gets the number of entries in the CGALLERY.INI file "Slideshow" section
// returns: number of entries or zero if no file
/***********************************************************************/
int far GetNumberSlides (void)
{
	INIPTR hIni;
	STRING szIniName;
	STRING szSection;
	STRING szBuff;
	FNAME  szPath;
	int    iFileCnt = 0;

	if (! LoadString (hInstAstral, IDS_GALLERY_INIFILE, szIniName, sizeof(STRING)) )
		return 0;
	if (! LoadString (hInstAstral, IDS_SLIDE_FILE_SECTION, szSection, sizeof(STRING)) )
		return 0;

	lstrcpy (szPath, Control.WorkPath);
	// if a user is signed in, append the name to the path
	if (Control.CurrentUser[0] != 0)
	{
		lstrcat (szPath, Control.CurrentUser);
#ifdef _MAC
		// ALL INI files MUST be located in System:Preferences folder 
		// Make the filename unique by adding a dash (-)		
		lstrcat (szPath, "-");
#else
		lstrcat (szPath, szBackSlash);
#endif
	}
	lstrcat (szPath, szIniName);
    
	if ((hIni = FindFirstINIKey (szPath, szSection, szBuff, sizeof(szBuff))) != NULL)
	{
		iFileCnt++;
		while ((FindNextINIKey (hIni, szBuff, sizeof(szBuff))) != NULL)
		{
			iFileCnt++;
		}
	}
	if (iFileCnt > 32767)
		iFileCnt = 32767;
	return iFileCnt;
}


/***********************************************************************/
// Gets the filename referenced by Index from CGALLERY.INI
// returns: TRUE if successfull and puts filename in lpFileName
/***********************************************************************/
BOOL GetSlideFileIniEntry (LPSTR lpIniEntry, int Index)
{
	INIPTR hIni;
	STRING szIniName;
	STRING szSection;
	STRING szBuff;
	STRING szScratch;
	FNAME  szPath;
	int    iCnt = 0;

	if (! LoadString (hInstAstral, IDS_GALLERY_INIFILE, szIniName, sizeof(STRING)) )
		return 0L;
	if (! LoadString (hInstAstral, IDS_SLIDE_FILE_SECTION, szSection, sizeof(STRING)) )
		return 0L;

	lstrcpy (szPath, Control.WorkPath);

	// if a user is signed in, append the name to the path
	if (Control.CurrentUser[0] != 0)
	{
		lstrcat (szPath, Control.CurrentUser);
#ifdef _MAC
		// ALL INI files MUST be located in System:Preferences folder 
		// Make the filename unique by adding a dash (-)		
		lstrcat (szPath, "-");
#else
		lstrcat (szPath, szBackSlash);
#endif
	}
	lstrcat (szPath, szIniName);
	clr ((LPTR)szScratch, sizeof(STRING));
    
	if ((hIni = FindFirstINIKey (szPath, szSection, szBuff, sizeof(szBuff))) != NULL)
	{
		if (iCnt == Index)
		{
			lstrcpy (lpIniEntry, szBuff);
      		_CloseINIFile (hIni);
			return TRUE;
		}
		iCnt++;
		while ((FindNextINIKey (hIni, szBuff, sizeof(szBuff))) != NULL)
		{
			if (iCnt == Index)
			{
				lstrcpy (lpIniEntry, szBuff);
      			_CloseINIFile (hIni);
				return TRUE;
			}
			iCnt++;
		}
	}
	return FALSE;
}


/***********************************************************************/
// Saves slide show image delay value
// returns: 1 if successful, or 0 it not
/***********************************************************************/
BOOL SaveImageDelay (LPSTR lpImage, LPSTR lpDelay)
{
	STRING szIniName;
	STRING szSection;
	STRING szEntry;
	FNAME  szPath;
  
	if (lpImage == NULL || lpDelay == NULL)
		return FALSE;

	if (! LoadString (hInstAstral, IDS_GALLERY_INIFILE, szIniName, sizeof (STRING)) )
		return FALSE;

//	if (! LoadString (hInstAstral, IDS_SLIDE_FILE2_SECTION, szSection, sizeof (STRING)) )
//		return FALSE;
lstrcpy (szSection, "SlideshowDelay");

	lstrcpy (szEntry, lpImage);
	lstrcpy (szPath, Control.WorkPath);
  
	// if a user is signed in, append the name to the path
	if (Control.CurrentUser[0] != 0)
	{
		lstrcat (szPath, Control.CurrentUser);
#ifdef _MAC
		// ALL INI files MUST be located in System:Preferences folder 
		// Make the filename unique by adding a dash (-)		
		lstrcat (szPath, "-");
#else
		lstrcat (szPath, szBackSlash);
#endif
	}
	lstrcat (szPath, szIniName);
      
	if (! (WritePrivateProfileString (szSection, szEntry, lpDelay, szPath)))
	{
		return FALSE;
	}
	return TRUE;
}


/***********************************************************************/
// Get delay for a slide show entry
// returns: 1 if successful, or 0 it not
/***********************************************************************/
BOOL GetImageDelay (int Index, LPSTR lpDelay)
{
	STRING szIniName;
	STRING szSection;
	STRING szEntry;
	STRING szBuff;
	FNAME  szPath;
  
	if (! LoadString (hInstAstral, IDS_GALLERY_INIFILE, szIniName, sizeof(STRING)) )
		return FALSE;

//	if (! LoadString (hInstAstral, IDS_SLIDE_FILE2_SECTION, szSection, sizeof(STRING)) )
//		return FALSE;
lstrcpy (szSection, "SlideshowDelay");

	wsprintf (szEntry, "%d", Index);

	lstrcpy (szPath, Control.WorkPath);
  
	// if a user is signed in, append the name to the path
	if (Control.CurrentUser[0] != 0)
	{
		lstrcat (szPath, Control.CurrentUser);
#ifdef _MAC
		// ALL INI files MUST be located in System:Preferences folder 
		// Make the filename unique by adding a dash (-)		
		lstrcat (szPath, "-");
#else
		lstrcat (szPath, szBackSlash);
#endif
	}
	lstrcat (szPath, szIniName);
      
	if (! (GetPrivateProfileString (szSection, szEntry, "10", szBuff,
	  sizeof(STRING), szPath)))
	{
		lstrcpy (lpDelay, "10");
		return TRUE;
	}
	lstrcpy (lpDelay, szBuff);

	return TRUE;
}


/***********************************************************************/
// Gets the .ART file associated with this slideshow image control
// returns: TRUE if successfull and puts filename in lpFileName
/***********************************************************************/
BOOL GetSlideArtFileName (int Index, LPSTR lpFileName)
{
	INIPTR hIni;
	STRING szIniName;
	STRING szSection;
	STRING szExtension;
	STRING szBuff;
	STRING szScratch;
	STRING szArtNumber;
	FNAME  szPath;
	char * src;
	char * dst;
	int    iCnt = 0;

	if (! LoadString (hInstAstral, IDS_GALLERY_INIFILE, szIniName, sizeof(STRING)) )
		return 0L;
	if (! LoadString (hInstAstral, IDS_SLIDE_FILE_SECTION, szSection, sizeof(STRING)) )
		return 0L;
	if (! LoadString (hInstAstral, IDS_GALLERY_EXT, szExtension, sizeof(STRING)) )
		return FALSE;

	lstrcpy (szPath, Control.WorkPath);

	// if a user is signed in, append the name to the path
	if (Control.CurrentUser[0] != 0)
	{
		lstrcat (szPath, Control.CurrentUser);
#ifdef _MAC
		// ALL INI files MUST be located in System:Preferences folder 
		// Make the filename unique by adding a dash (-)		
		lstrcat (szPath, "-");
#else
		lstrcat (szPath, szBackSlash);
#endif
	}
	lstrcat (szPath, szIniName);
	clr ((LPTR)szScratch, sizeof(STRING));
	clr ((LPTR)szArtNumber, sizeof(STRING));
    
	if ((hIni = FindFirstINIKey (szPath, szSection, szBuff, sizeof(szBuff))) != NULL)
	{
		if ( (_ConvertFileNum (szBuff)) == Index)
		{
			// copy the .ART file number
			dst = szArtNumber;
			src = strchr (szBuff, '=');
			src++;
			while (isdigit (*src))
			{
				*dst = *src;
				dst++;
				src++;
			}
			src = strchr (szBuff, '.');
			*src = 0;
			src--;
			while (isdigit (*src))
			{
				*src = 0;
				src--;
			}
			lstrcpy (szScratch, szBuff);
			lstrcat (szScratch, szArtNumber);
			lstrcat (szScratch, szExtension);
			lstrcpy (lpFileName, szScratch);
      		_CloseINIFile (hIni);
			return TRUE;
		}
		iCnt++;
		while ((FindNextINIKey (hIni, szBuff, sizeof(szBuff))) != NULL)
		{
			if ( (_ConvertFileNum (szBuff)) == Index)
			{
				// copy the .ART file number
				dst = szArtNumber;
				src = strchr (szBuff, '=');
				src++;
				while (isdigit (*src))
				{
					*dst = *src;
					dst++;
					src++;
				}
				src = strchr (szBuff, '.');
				*src = 0;
				src--;
				while (isdigit (*src))
				{
					*src = 0;
					src--;
				}
				lstrcpy (szScratch, szBuff);
				lstrcat (szScratch, szArtNumber);
				lstrcat (szScratch, szExtension);
				lstrcpy (lpFileName, szScratch);
      			_CloseINIFile (hIni);
				return TRUE;
			}
			iCnt++;
		}
	}
	return FALSE;
}



void GetNextKey(LPSTR lpBuff, UINT uBuffLen)
{
// Return the next key in the following format:
//	Entry=value
//	Example:	Guest=C:\CRAYOLA\GUEST
// szEntries holds a list of all the entries in this section.

	STRING	szKey;
	int		i;
	int		iLen;
	
	lstrcpy(szKey, &szEntries[iEntryPos]);
	lstrcat(szKey, "=");
	iLen = lstrlen(szKey);

// Get the value for this entry	
	GetPrivateProfileString(
		szSection,
		&szEntries[iEntryPos],
		"",
		&szKey[iLen],
		sizeof(szKey) - iLen,
		szINIFileName);
	iLen = lstrlen(szKey);

// Copy only the requested number of bytes to the output.
	iLen = min(uBuffLen-1, iLen);		
	for (i=0; i<iLen; i++)
		lpBuff[i] = szKey[i];
	lpBuff[iLen] = 0;

// Get the start of the next entry string
	iEntryPos = lstrlen(&szEntries[iEntryPos]) + 1 + iEntryPos;
}


////////////////////////////////////////////////////////////////////////
// Function    : HANDLE FindFirstINIKey(lpszIniFile, lpszSection, lpBuff,
//                                    uBuffLen)
//
// Description : Return the first key in an .INI section
//
// Inputs      : LPSTR  lpszIni     -     .INI filename
//               LPSTR  lpszSection     -     Section to find
//               LPCHAR lpBuff          -     Buffer
//               UINT   uBuffLen        -     Buffer length
//
// Outputs     : FILE *
////////////////////////////////////////////////////////////////////////

INIPTR FindFirstINIKey (LPSTR lpszFile, LPSTR lpszSection, LPSTR lpBuff, UINT uBuffLen)
{
#ifdef _MAC
	lstrcpy(szINIFileName, lpszFile);		// save this for later use
	lstrcpy(szSection,     lpszSection);	// The section these entries are from
	
	iEntrySize = GetPrivateProfileString(
		szSection,
		NULL,			// gather all the entries into one buffer
		"",
		szEntries,
		sizeof(szEntries),
		szINIFileName);
	if (iEntrySize)
	{
		iEntryPos = 0;
		GetNextKey(lpBuff, uBuffLen);
		return (INIPTR)1;
	}		
	return (INIPTR)0;	// this isn't used any more
	
#else	// _MAC

   INIPTR iPtr;
   int    iReturn = FALSE;

   if (!lpszFile || !lpszSection || !lpBuff || !uBuffLen)
     return(0);

   clr ((LPTR)lpBuff, uBuffLen);

   if ((iPtr = _OpenINIFile(lpszFile, OF_READ)) == (FILE *)NULL)
     return(0);
   
   if (_FindINISection(iPtr, lpszSection))
   {
     iReturn = _ReadINIKey(iPtr, NULL, lpBuff, uBuffLen);
   }

   if (iReturn)
      return(iPtr);
   else
   {
      _CloseINIFile(iPtr);
      return(NULL);
   }
#endif	// _MAC  
}


////////////////////////////////////////////////////////////////////////
// Function    : BOOL FindNextINIKey(hIni, lpBuff, uBuffLen)
//
// Description : Returns the next Section in an INI file.
//
// Inputs      : HANDLE hIni      -     Handle from FindFirstINIKey
//               LPCHAR lpBuff    -     Pointer to Buffer
//               UINT   uBuffLen  -     Length of buffer
//
// Outputs     : 0 = end of section, 1 = found another key.
////////////////////////////////////////////////////////////////////////

BOOL FindNextINIKey(INIPTR hIni, LPSTR lpBuff, UINT uBuffLen)
{
#ifdef _MAC
	if (iEntrySize && szEntries[iEntryPos])
	{
		GetNextKey(lpBuff, uBuffLen);
		return TRUE;
	}
	return FALSE;		
#else  // _MAC

   INIPTR iPtr = (INIPTR)hIni;
   int    iReturn;

   if (!iPtr || !lpBuff || !uBuffLen)
     return(0);

   clr ((LPTR)lpBuff, uBuffLen);

   iReturn = _ReadINIKey(iPtr, NULL, lpBuff, uBuffLen);

   if (iReturn)
      return(TRUE);
   else
   {
      _CloseINIFile(iPtr);
      return(FALSE);
   }
#endif // _MAC   
}


////////////////////////////////////////////////////////////////////////
// Function    : FILE * _OpenINIFile(lpszIniFile, uMode)
//
// Description : Open a .INI datafile and return a handle to the beginning
//               of the file.
//
// Inputs      : LPSTR lpszIniFile   -  Filename to open.
//               UINT  uMode         -  Filemode to open in.
//
// Outputs     : Pointer to file handle.
////////////////////////////////////////////////////////////////////////

static INIPTR _OpenINIFile(LPSTR lpszIniFile, UINT uMode)
{
   FILE  *fp;
#ifndef _MAC
   char  szFile[MAX_PATH];
   char  *szMode;

   switch(uMode)
   {
      case OF_READ:  szMode = "rt"; break;
      case OF_WRITE: szMode = "r+t"; break;
      default:    return((INIPTR)NULL);
   }

   lstrcpy (szFile,lpszIniFile); // _fsopen needs a near ptr just to be safe!!
   fp = _fsopen(szFile, szMode, _SH_DENYNO);
#endif //#ifndef _MAC
   return ((INIPTR)fp);
}


////////////////////////////////////////////////////////////////////////
// Function    : VOID _CloseINIFile(iPtr)
// Description : Closes a .INI file pointer
//
// Inputs      : FILE * iPtr   -  .INI file pointer
//
// Outputs     : None
////////////////////////////////////////////////////////////////////////

static VOID _CloseINIFile (INIPTR iPtr)
{
#ifndef _MAC
   if (iPtr)
      fclose ((FILE*)iPtr);
#endif
}


////////////////////////////////////////////////////////////////////////
// Function    : UINT _ReadINIKey(iPtr, lpszKey, lpBuff, uBuffLen)
//
// Description : This function locates and reads a key definition from
//               the current section in the .INI file referenced by 'iPtr'.
//
// Inputs      : FILE * iPtr      -     .INI file pointer
//               LPSTR  lpszKey   -     Key to search for
//               LPSTR  lpBuff    -     Buffer to store definition into
//               UINT   uBuffLen  -     Length of above buffer.   
//
// Outputs     : Count of bytes written to buffer
////////////////////////////////////////////////////////////////////////

#ifndef _MAC
static UINT _ReadINIKey(INIPTR iPtr, LPSTR lpszKey, LPSTR lpBuff, UINT uBuffLen)
{
   BOOL bFound = FALSE, bEOF = FALSE;
   fpos_t fpos;
   int len;

   if (iPtr)
   {
      if (lpszKey)
      {
        len = _fstrlen (lpszKey);
        fgetpos ((FILE*)iPtr,&fpos);
      }

      while (!bFound && !bEOF)
      {
         char szBuffer[MAX_INI_LINELEN];
         char *p;

         if (fgets (szBuffer, sizeof(szBuffer), (FILE*)iPtr) != NULL)
         {
            p = szBuffer;
            _fstrtok (szBuffer, "\n");

            // Blank line?
            if (*p == '\r' || *p == '\0' || *p == '\n' || *p == ' ')
              continue;

            // If this is the start of a new section,
            // then we did not find the key - blow out.
            if (*p == '[')
              bEOF = TRUE;
            else if (lpszKey == NULL)  /* Always find */
            {
               bFound = TRUE;
               _fstrncpy (lpBuff, p, uBuffLen);
            }
            else if (!_fstrnicmp (lpszKey, p, len))
            {
               // There must be an '=' directly following
               // the key to be found.
               if (*(p + len) == '=')
               {
                  bFound = TRUE;
                  *(p+len) = '\0';
                  p += (len+1);
                  _fstrncpy (lpBuff, p, uBuffLen);
               }
            }
         }
         else
            bEOF = TRUE;
      }

      // Reset the file position pointer.
      if (lpszKey)
        fsetpos((FILE*)iPtr,&fpos);
   }
   return (bFound) ? _fstrlen(lpBuff) : FALSE;
}
#endif //#ifndef _MAC


////////////////////////////////////////////////////////////////////////
// Function    : BOOL _FindINISection(iPtr, lpszSection)
//
// Description : This function locates a section within the .INI file
//               that is described by iPtr.  The file position is updated
//               to be on the line after the section if it is found.  If the
//               section is not located, the file pointer is not changed.
//
// Inputs      : INIPTR iPtr        -     .INI file pointer
//               LPSTR  lpszSection -     Pointer to section to find.
//
// Outputs     : 0 = Did not find section
//               1 = Found section, file ptr adjusted.
////////////////////////////////////////////////////////////////////////

#ifndef _MAC
static BOOL _FindINISection(INIPTR iPtr, LPSTR lpszSection)
{
   BOOL bFound = FALSE, bEOF = FALSE;
   fpos_t fpos;

   if (iPtr)
   {
      int  len = _fstrlen(lpszSection);

      // Save our current position
      fgetpos ((FILE*)iPtr, &fpos);

      // Move to the beginning of the file
      fseek ((FILE*)iPtr, 0L, SEEK_SET);

      // Read each line and see if it is our target.
      while (!bFound && !bEOF)
      {
         char szBuffer[MAX_INI_LINELEN];
         char *p;

         if (fgets(szBuffer,sizeof(szBuffer),(FILE *)iPtr) != NULL)
         {
            p = szBuffer;
            if (*p == '[')  /* Section beginning? */
            {
              if (!_fstrnicmp (lpszSection, ++p, len))
              {
                if (*(p + len) == ']')
                  bFound = TRUE;
              } 
            }
         }
         else
            bEOF = TRUE;
      }

      // If we did not find the section, then reset 
      // the file pointer to where we found it.
      if (! bFound)
        fsetpos ((FILE *)iPtr, &fpos);
   }
   return (bFound);
}
#endif //#ifndef _MAC


// down and dirty routine to find the numeric part of the filename,
// convert and return it
static int _ConvertFileNum (LPSTR lpBuff)
{
	int iRet;
	STRING szScratch;
	char * src;
//	int atoi (const char *);

	lstrcpy (szScratch, lpBuff);
	src = strchr (szScratch, '.');
	*src = 0;
	src--;
	while (isdigit (*src))
	{
		src--;
	}
	src++;
	iRet = atoi (src);
	return iRet;
}


