/*
// $Header: /PM8/App/TYPEFACE.CPP 1     3/03/99 6:12p Gbeddow $
//
// Typeface server implementation.
//
// $Log: /PM8/App/TYPEFACE.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 5     9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 4     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 3     4/29/98 4:27p Fredf
// New PMW fonts. Required changes to PMW.FFL.
// 
// 2     12/18/97 3:23p Dennis
// Added Font Origin logic used by HTML generator to determine if font is
// a standard windows font.
// 
//    Rev 1.0   14 Aug 1997 15:26:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:52   Fred
// Initial revision.
// 
//    Rev 1.5   09 Apr 1997 14:51:44   Fred
// Start of new font caching (fonts and metrics)
// 
//    Rev 1.4   03 Apr 1997 09:32:16   Fred
// More text cleanup
// 
//    Rev 1.3   26 Mar 1997 15:58:18   Fred
// Removed old geronimo stuff
// 
//    Rev 1.2   07 Feb 1997 08:55:26   Fred
// Removed font types
// 
//    Rev 1.1   27 Aug 1996 17:46:12   Fred
// Fixed bug where WM_FONTCHANGE could crash PM
// 
//    Rev 1.0   14 Mar 1996 13:35:12   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:31:22   FRED
// Initial revision.
// 
//    Rev 2.7   14 Feb 1996 12:15:52   JAY
// New mechanism for Free Fonts
// 
//    Rev 2.6   31 Jan 1996 08:21:16   JAY
// More configuration stuff
// 
//    Rev 2.5   04 Oct 1995 10:58:12   FRED
// 32 bit TrueType engine
// 
//    Rev 2.4   19 Sep 1995 15:46:10   JAY
// Re-instated Windows font reading.
// 
//    Rev 2.3   13 Sep 1995 11:47:12   JAY
// New 32-bit changes
// 
//    Rev 2.2   15 Mar 1995 10:49:52   JAY
// Support for new text editing
// 
//    Rev 2.1   08 Feb 1995 13:35:16   JAY
// Reverted. New series.
// 
//    Rev 1.13   16 Sep 1994 14:17:36   JAY
// Only "stuffs" font file lists if CD_VERSION.
// 
//    Rev 1.12   04 Aug 1994 08:30:26   JAY
// Large model and 32-bit changes
// 
//    Rev 1.11   22 Jun 1994 14:13:04   JAY
// Changed the Windows 4.0 check.
// 
//    Rev 1.10   10 Jun 1994 10:40:10   JAY
// Registration font support.
// 
//    Rev 1.9   09 Jun 1994 11:18:46   JAY
// Now uses the CD cache.
// Replaces '|' characters in FFL font names with ' ' (to allow spaces).
// 
//    Rev 1.8   01 Jun 1994 16:47:22   JAY
// Added new variations to the face entry structure and the code to allow
// virtual file (from Font File Lists) and a preview font.
// 
//    Rev 1.7   17 Mar 1994 16:46:24   JAY
// Some minor adjustments to return values.
// 
//    Rev 1.6   11 Mar 1994 14:27:56   JAY
// Now specified TT_ONLY when creating a log font.
// 
//    Rev 1.5   09 Mar 1994 09:22:50   MACDUFF
// Another check for Windows NT.
// 
//    Rev 1.4   04 Mar 1994 12:00:52   MACDUFF
// Under Windows 4.0 and above, the typeface callback returns a logfont
// structure rather than a pathname, because the undocumented Windows calls
// used to get the pathname are not available.
// 
//    Rev 1.3   31 Jan 1994 07:25:10   JAY
// Removed extra od('s for release version.
// 
//    Rev 1.2   19 Jan 1994 17:09:28   JAY
//  
// 
//    Rev 1.1   13 Jan 1994 17:03:40   JAY
//  
//
//   Rev 1.0   16 Dec 1993 16:28:30   JAY
// 
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwini.h"

#include <string.h>

#include "system.h"
#include "pmgdb.h"

#include "typeface.h"
#include "truetype.h"
#include "vfl.h"
#include "utils.h"
#include "pmwcfg.h"
#include "cdcache.h"

#define WF_WINNT 0x4000

static char near ENTRY_FontPreviewFile[] = "FontPreviewFile";
static LPCSTR s_szaWindows95StandardFontName[] = 
{
   "Arial",
   "Courier New",
   "Symbol",
   "Times New Roman",
   "WingDings",
   NULL        // Last entry must be NULL which indicates end of list
};

/*
// The global typeface server.
// We only need one (I strongly believe).
*/

PUBLIC TypefaceServer near typeface_server;


// In some cases, the typeface number is composited with the bold and italic flags.
// These flags are used by get_typeface_path().

SHORT typeface_for_fstyle(SHORT index, FONT_STYLE fstyle)
{
	// We encode the font style as follows:
	// bits           meaning
	// ----           -------
	//  15            italic
	//  14            bold
	// 13-0           font index (0-16383)    <- hopefully enough

   USHORT val = index & 0x3FFF;

   if (fstyle & FONT_STYLE_Bold)
   {
      val |= 0x4000;
   }
   if (fstyle & FONT_STYLE_Italic)
   {
      val |= 0x8000;
   }

   return (SHORT)val;
}

SHORT typeface_index(SHORT typeface)
{
	// Extract the typeface index from the composite value created
	// by the typeface_for_fstyle() method.
	return typeface & 0x3FFF;
}

FONT_STYLE typeface_style(SHORT typeface)
{
	// Extract the typeface style flags from the composite value created
	// by the typeface_for_fstyle() method.
	SHORT Style = 0;
	if (((USHORT)typeface & 0x4000) != 0)
	{
		Style |= FONT_STYLE_Bold;
	}
	if (((USHORT)typeface & 0x8000) != 0)
	{
		Style |= FONT_STYLE_Italic;
	}
	return (FONT_STYLE)Style;
}

/***************************/
/* FaceEntry class support */
/***************************/

/*
// The standard variation type.
*/

VARIATION_TYPE FaceEntry::standard_variation_type = VARIATION_TYPE_WIN40;

/*
// Constructor.
*/

FaceEntry::FaceEntry()
{
	// Start us out blank.
	memset(variation, 0, sizeof(variation));

	// Clear extended fields. They will have to be set by someone
	// else once the information is available (GetFontCacheItem).
	m_fIsExtendedInformationValid = FALSE;
	m_uMinimumPixelHeight = 0;
	m_fIsTextFont = FALSE;
}

/*
// Destructor.
*/

FaceEntry::~FaceEntry()
{
/* Free the file paths. */

	for (SHORT i = 0; i < FEStyle_MAX; i++)
	{
		if (((variation[i].vtype == VARIATION_TYPE_VIRTUAL)
		  || (variation[i].vtype == VARIATION_TYPE_REGISTRATION))
		 && (variation[i].v.file_name != NULL))
		{
			free(variation[i].v.file_name);
		}
	}
}

/*
// Set the face name.
*/

BOOL FaceEntry::set_name(LPCSTR new_name)
{
	if (new_name == NULL)
	{
		return FALSE;
	}
	else
	{
		m_csName = new_name;
		return TRUE;
	}
}

/*
// Set a file name (for PMW.FFL fonts only.)
*/

BOOL FaceEntry::set_file_name(LPCSTR new_file_name, SHORT i)
{
	free(variation[i].v.file_name);
	if ((variation[i].v.file_name = strdup(new_file_name)) == NULL)
	{
		if (new_file_name != NULL)
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*
// Return the file path for a variation, reading it if necessary.
*/

int FaceEntry::setup_file_variation(SHORT i, LOGFONT* plf, StorageFilePtr file)
{
	switch (variation[i].vtype)
	{
		case VARIATION_TYPE_WIN40:
		{
		/* Windows 4.0: return LOGFONT */
			BuildLogFont(i, plf);
			return GFC_RESULT_logfont;
		}
		case VARIATION_TYPE_VIRTUAL:
		case VARIATION_TYPE_REGISTRATION:
		{
		/* Build the font name. */
			char buffer[_MAX_PATH];
			build_path_name(buffer, typeface_server.m_csVirtualFontPath, variation[i].v.file_name);

			ERRORCODE error;
			BOOL cacheable = cd_cache.file_is_cacheable(buffer);

			if (cacheable)
			{
				error = cd_cache.prep_font_file(buffer, file);
				od("Cacheable with error %d\r\n", error);
				if (error == ERRORCODE_None)
				{
				/* We got a font in the cache! */
					od("Using cached font '%s'!\r\n", (LPCSTR)buffer);
					return GFC_RESULT_file;
				}
			}

			if (GetGlobalPathManager()->FileExists(buffer, TRUE))
			{
				if (cacheable)
				{
					od("Caching font '%s'\r\n", (LPCSTR)buffer);
					if ((error = cd_cache.cache_font(buffer)) == ERRORCODE_None)
					{
					/* The font was cached. Try to use that one. */
						if ((error = cd_cache.prep_font_file(buffer, file)) == ERRORCODE_None)
						{
							od("Using newly cached font '%s'!\r\n", (LPCSTR)buffer);
							return GFC_RESULT_file;
						}
					}
				}
				file->set_name(buffer);
				return GFC_RESULT_file;
			}
			break;
		}
		case VARIATION_TYPE_PREVIEW:
		{
			PCSTR pfn;

			if ((pfn = typeface_server.font_preview_filename()) != NULL)
			{
				file->set_name(pfn);
				return GFC_RESULT_file;
			}

			break;
		}
		default:
		{
			break;
		}
	}
	return GFC_RESULT_failure;
}

/*
// Build the style preference array.
*/

BOOL FaceEntry::build_preference_array(SHORT style_index, SHORT preference[])
{
	preference[0] = style_index;

	switch (style_index)
	{
		case FONT_STYLE_Normal:
		{
			preference[1] = FONT_STYLE_Bold;
			preference[2] = FONT_STYLE_Italic;
			preference[3] = FONT_STYLE_BoldItalic;
			break;
		}
		case FONT_STYLE_Bold:
		{
			preference[1] = FONT_STYLE_Normal;
			preference[2] = FONT_STYLE_BoldItalic;
			preference[3] = FONT_STYLE_Italic;
			break;
		}
		case FONT_STYLE_Italic:
		{
			preference[1] = FONT_STYLE_BoldItalic;
			preference[2] = FONT_STYLE_Normal;
			preference[3] = FONT_STYLE_Bold;
			break;
		}
		case FONT_STYLE_BoldItalic:
		{
			preference[1] = FONT_STYLE_Italic;
			preference[2] = FONT_STYLE_Bold;
			preference[3] = FONT_STYLE_Normal;
			break;
		}
		default:
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*
// Get a file path name for this variation.
*/

int FaceEntry::setup_file(SHORT style_index, LOGFONT* plf, StorageFilePtr file)
{
	SHORT preference[FEStyle_MAX];

	if (!build_preference_array(style_index, preference))
	{
		return GFC_RESULT_failure;
	}

	int result;

	for (SHORT i = 0; i < FEStyle_MAX; i++)
	{
		if ((result = setup_file_variation(preference[i], plf, file)) != GFC_RESULT_failure)
		{
			break;
		}
	}
	return result;
}

/*
// Add a variation (while processing variations).
*/

void FaceEntry::add_standard_variation(SHORT style_index, LOGFONT* plf)
{
	if (variation[style_index].vtype != VARIATION_TYPE_NONE)
	{
	/* Already got one! What should we do? */
		return;
	}

/* Setup the variation. */

	variation[style_index].vtype = standard_variation_type;
	variation[style_index].s.italic = plf->lfItalic;
	variation[style_index].s.underline = plf->lfUnderline;
	variation[style_index].s.strikeout = plf->lfStrikeOut;
	variation[style_index].s.quality = plf->lfQuality;
	variation[style_index].s.outPrecision = plf->lfOutPrecision;
	variation[style_index].s.charSet = plf->lfCharSet;
	variation[style_index].s.pitchAndFamily = plf->lfPitchAndFamily;
	variation[style_index].s.clipPrecision = plf->lfClipPrecision;
	variation[style_index].s.weight = (short)(plf->lfWeight);
	variation[style_index].s.width = (short)(plf->lfWidth);
	variation[style_index].s.height = (short)(plf->lfHeight);
}

/*
// Add a virtual variation (from PMW.FFL -- while processing font file lists).
*/

void FaceEntry::add_virtual_variation(SHORT style_index, LPCSTR pszFileName, int nPreviewFontNumber, BOOL registration)
{
	if (variation[style_index].vtype != VARIATION_TYPE_NONE)
	{
	/* Already got one! What should we do? Skip it! */
		return;
	}

	if (set_file_name(pszFileName, style_index))
	{
	/* Setup the variation. */

		variation[style_index].v.preview_font_number = nPreviewFontNumber;

		variation[style_index].vtype = registration
					? VARIATION_TYPE_REGISTRATION
					: VARIATION_TYPE_VIRTUAL;
	}
}

/*
// Set all variations to the preview type.
*/

void FaceEntry::set_preview_variations(void)
{
	variation[0].vtype =
	variation[1].vtype =
	variation[2].vtype =
	variation[3].vtype = VARIATION_TYPE_PREVIEW;
}

/*
// Get the path for a font variation.
*/

void FaceEntry::BuildLogFont(int nStyleIndex, LOGFONT* plf)
{
	memset(plf, 0, sizeof(*plf));

	plf->lfItalic = (unsigned char)(variation[nStyleIndex].s.italic);
	plf->lfUnderline = (unsigned char)(variation[nStyleIndex].s.underline);
	plf->lfStrikeOut = (unsigned char)(variation[nStyleIndex].s.strikeout);
	plf->lfQuality = (unsigned char)(variation[nStyleIndex].s.quality);
	plf->lfOutPrecision = (unsigned char)(variation[nStyleIndex].s.outPrecision);
	plf->lfCharSet = variation[nStyleIndex].s.charSet;
	plf->lfPitchAndFamily = variation[nStyleIndex].s.pitchAndFamily;
	plf->lfClipPrecision = variation[nStyleIndex].s.clipPrecision;
	plf->lfWeight = variation[nStyleIndex].s.weight;
	plf->lfWidth = variation[nStyleIndex].s.width;
	plf->lfHeight = variation[nStyleIndex].s.height;

	strcpy(plf->lfFaceName, m_csName);
}

/*
// Get the type of a variation.
*/

VARIATION_TYPE FaceEntry::get_variation_type(SHORT style_index)
{
	SHORT preference[FEStyle_MAX];

	if (build_preference_array(style_index, preference))
	{
		for (int i = 0; i < FEStyle_MAX; i++)
		{
			int var = preference[i];
			if (variation[var].vtype != VARIATION_TYPE_NONE)
			{
				return variation[var].vtype;
			}
		}
	}

	return VARIATION_TYPE_NONE;
}

/*
// Get the logical index for this face and style in the preview font.
// If the face and style are not represented in the preview font, then
// -1 is returned.
*/

int FaceEntry::get_preview_font_number(SHORT style_index)
{
	SHORT preference[FEStyle_MAX];

	if (build_preference_array(style_index, preference))
	{
		for (int i = 0; i < FEStyle_MAX; i++)
		{
			int var = preference[i];
			if (variation[var].vtype != VARIATION_TYPE_NONE)
			{
				if ((variation[var].vtype == VARIATION_TYPE_VIRTUAL)
				 || (variation[var].vtype == VARIATION_TYPE_REGISTRATION))
				{
					return variation[var].v.preview_font_number;
				}
			/* Not virtual! */
				break;
			}
		}
	}

	return -1;
}

/*
// Fake the registration of a face.
// This makes each variation "virtual" instead of "registration".
*/

BOOL FaceEntry::fake_register(void)
{
	BOOL has_reg = FALSE;
	for (int var = 0; var < FEStyle_MAX; var++)
	{
		if (variation[var].vtype == VARIATION_TYPE_REGISTRATION)
		{
			variation[var].vtype = VARIATION_TYPE_VIRTUAL;
			has_reg = TRUE;
		}
	}
	return has_reg;
}

/*
// Is the font one chooseable from the list?
// A font is chooseable if it is not the preview font and if it has
// a registered variation.
*/

BOOL FaceEntry::is_chooseable(BOOL is_registered) const
{
	BOOL chooseable = TRUE;

	if (variation[0].vtype == VARIATION_TYPE_PREVIEW)
	{
	/* Definitely no. */
		return FALSE;
	}

	if (!is_registered)
	{
		for (int i = 0; i < FEStyle_MAX; i++)
		{
			if (variation[i].vtype == VARIATION_TYPE_REGISTRATION)
			{
				chooseable = FALSE;
			}
			else if (variation[i].vtype != VARIATION_TYPE_NONE)
			{
			/* This one must be allowed. */
				chooseable = TRUE;
			/* And stop looking. */
				break;
			}
		}
	}
	return chooseable;
}

/**************************/
/* FaceList class support */
/**************************/

/*
// The criterion function for inserting a typeface name.
*/

LIST_INSERT_ENUM typeface_criterion(ListElementPtr element, ListElementPtr search_element, void* data)
{
	LPCSTR name = ((FaceEntry*)element)->get_name();
	LPCSTR search_name = ((FaceEntry*)search_element)->get_name();

	SHORT ret = stricmp(name, search_name);

	if (ret < 0)
	{
	/* Insert here. */
		return INSERT_BEFORE;
	}
	else if (ret == 0)
	{
	/* Already in list. */
		return INSERT_ABORT;
	}

/* Keep going. */

	return INSERT_NOT_YET;
}

/*
// Add an entry to the face list.
// The entry created can be returned if so desired.
*/

BOOL FaceList::add_entry(LPCSTR name, FaceEntry** pentry)
{
	FaceEntry* entry;

/* Create the font entry. */

	if ((entry = new FaceEntry) == NULL)
	{
		return FALSE;
	}

	if (!entry->set_name(name))
	{
		delete entry;
		return FALSE;
	}

/* Insert in the list. */

	if (!typeface_server.face_list.insert(entry, typeface_criterion))
	{
	/* Not inserted (because we aborted - already in the list). */
		delete entry;
		entry = NULL;
	}

   // Determine what type of font this is (Standard Windows 95, ect.)
   

/* Pass the entry back if requested. */

	if (pentry != NULL)
	{
		*pentry = entry;
	}

	return TRUE;
}

/*
// Find a face in the list.
*/

SHORT FaceList::find_face(LPCSTR face_name)
{
	FaceEntry*  entry;
	SHORT index;

	for (index = 0, entry = first_face(); entry != NULL; entry = entry->next_face(), index++)
	{
		if (stricmp(face_name, entry->get_name()) == 0)
		{
		/* This is it! */
			return index;
		}
	}
	return -1;
}

/*
// Build the face list.
*/

int CALLBACKPROC typeface_collect_family(ENUMLOGFONT* pLogFont, NEWTEXTMETRIC* pTextMetric, short nFontType, LPSTR lpData)
{
	if ((nFontType & TRUETYPE_FONTTYPE) != 0)
	{
		if (!typeface_server.face_list.add_entry(pLogFont->elfLogFont.lfFaceName))
		{
			return 0;
		}
	}
	return 1;
}

/*
// Handle a variation.
*/

int CALLBACKPROC typeface_collect_variation(ENUMLOGFONT* pLogFont, NEWTEXTMETRIC* pTextMetric, short nFontType, LPSTR pData)
{
/* We have a variation. */

	FaceEntry* entry = (FaceEntry*)pData;

	if ((nFontType & TRUETYPE_FONTTYPE) == 0)
	{
	/*
 	// This would be really wierd, but let's check anyway.
	// Just skip it.
 	*/
		return 1;
	}

/* See what variation this is. */

	DWORD ntmFlags = pTextMetric->ntmFlags;
	FLAGS new_style = 0;

	if ((ntmFlags & NTM_BOLD) != 0)
	{
		new_style++;
	}
	if ((ntmFlags & NTM_ITALIC) != 0)
	{
		new_style += 2;
	}

#if 0
	od("Got variation %s (S:%s) (fl: %lx, ns: %d) (wt:%d, it:%d)\r\n",
					(LPSTR)pLogFont->elfLogFont.lfFaceName,
					(LPSTR)pLogFont->elfStyle,
					ntmFlags,
					new_style,
					pLogFont->elfLogFont.lfWeight,
					pLogFont->elfLogFont.lfItalic);
#endif

/* Add the style. */

	entry->add_standard_variation(new_style, (LOGFONT*)pLogFont);

	return 1;
}

/*
// Build the family list.
*/

SHORT FaceList::build_family_list(HDC hdc)
{
	FONTENUMPROC lpFamilyProc;

	if ((lpFamilyProc = (FONTENUMPROC)MakeProcInstance((FARPROC)typeface_collect_family, AfxGetInstanceHandle())) == NULL)
	{
		return 0;
	}

	SHORT ret = EnumFontFamilies(hdc, NULL, lpFamilyProc, NULL);

	FreeProcInstance((FARPROC)lpFamilyProc);

	return ret;
}

/*
// Get the variations.
*/

SHORT FaceList::get_face_variations(HDC hdc)
{
	FONTENUMPROC lpVariationProc;

	if ((lpVariationProc = (FONTENUMPROC)MakeProcInstance((FARPROC)typeface_collect_variation, AfxGetInstanceHandle())) == NULL)
	{
		return 0;
	}

	for (FaceEntry* entry = (FaceEntry*)first_element(); entry != NULL; entry = entry->next_face())
	{
		EnumFontFamilies(hdc, entry->get_name(), lpVariationProc, (LPARAM)entry);
	}

	FreeProcInstance((FARPROC)lpVariationProc);

	return 1;
}

/*
// Stuff an individual font file list.
*/

BOOL FaceList::stuff_font_file_list(PCSTR name)
{
	FontFileList ffl(name);

	if (ffl.open() == ERRORCODE_None)
	{
		PCSTR pcstr;

	/* Read the virtual path. */

		if ((pcstr = ffl.read_virtual_path()) == NULL)
		{
		/* Illegally formed file. */
			return FALSE;
		}

	/* Save the virtual path. */

		typeface_server.m_csVirtualFontPath = pcstr;

	/* Search through all file names for the base name. */

		while ((pcstr = ffl.read_next_name()) != NULL)
		{
			int nVersion = ffl.last_line_version();
			if (GetConfiguration()->SupportsVirtualFile(nVersion))
			{
				int flags = ffl.last_line_flags();
				char szVariation[FEStyle_MAX][80];
				int nPreviewFontNumber[FEStyle_MAX];
				char szName[80];

				if (sscanf(pcstr, "%s %d %s %d %s %d %s %d %s",
							  szVariation[0],
							  &nPreviewFontNumber[0],
							  szVariation[1],
							  &nPreviewFontNumber[1],
							  szVariation[2],
							  &nPreviewFontNumber[2],
							  szVariation[3],
							  &nPreviewFontNumber[3],
							  szName) != 9)
				{
					continue;
				}

			/*
			// Replace all '|' characters with spaces.
			// (Is this considered a "kludge" or a "convention"?)
			*/
				LPSTR p;
				while ((p = strchr(szName, '|')) != NULL)
				{
					*p = ' ';
				}

			/* Try to find the entry in the list. */

				FaceEntry*  entry;
				int16 index;

				if ((index = find_face(szName)) == -1)
				{
				/* No entry yet. Make one now. */
					if (!add_entry(szName, &entry))
					{
						continue;
					}
				}
				else
				{
					entry = get_face(index);
				}

				if (entry == NULL)
				{
					continue;
				}

			/*
			// Now we have the entry.
			// Add the variations.
			*/

				BOOL fRegFace = GetConfiguration()->IsRegistrationFace(flags);

				for (int i = 0; i < FEStyle_MAX; i++)
				{
					if (strcmp(szVariation[i], ".") != 0)
					{
						entry->add_virtual_variation(i, szVariation[i], nPreviewFontNumber[i], fRegFace);
					}
				}
			}
		}
	}
	return TRUE;
}

/*
// Stuff all font file lists.
*/

void FaceList::stuff_font_file_lists(void)
{
	stuff_font_file_list(GetGlobalPathManager()->ExpandPath("PMW.FFL"));
}

/*
// Build the face list.
*/

BOOL FaceList::build(void)
{
/* Make sure our list is empty. */

	empty();

	HDC hdc = ::GetDC(NULL);

/*
// Enumerate the font families.
*/

	if (build_family_list(hdc))
	{
		if (get_face_variations(hdc))
		{
		}
	}

/* Stuff any font file lists into the master list. */

	stuff_font_file_lists();

   // Run through all font families and determine font source for each family
   DetermineFontSources();

	::ReleaseDC(NULL, hdc);

	return TRUE;
}

// Run through all fonts and determine font source for a family
void FaceList::DetermineFontSources()
{
   BOOL  bFoundFont = FALSE;
   int   i;

	for (FaceEntry* entry = (FaceEntry*)first_element(); entry != NULL; entry = entry->next_face())
	{
      bFoundFont = FALSE;
      // Run through all variations and determine if any are PrintMaster type
      // If one or more is, assume family originates from PrintMaster
      for(int nVariation=0;
				nVariation <= FEStyle_MAX 
            && entry->get_variation_type(nVariation) != VARIATION_TYPE_NONE 
            && !bFoundFont;
         nVariation++)
         {
            if(entry->get_variation_type(nVariation) == VARIATION_TYPE_VIRTUAL ||
               entry->get_variation_type(nVariation) == VARIATION_TYPE_REGISTRATION)
               {
                  entry->SetFontOrigin(FaceEntry::fotPrintMaster);
                  bFoundFont = TRUE;
                  continue;
               }
         }
      if(bFoundFont)
         continue;

      // If we made it here the font type is a Windows true type
      // Now determine if it's a standard Windows 95 font
      CString  csFontName = entry->get_name();
      for(i=0;s_szaWindows95StandardFontName[i] != NULL && !bFoundFont;i++)
      {
         if(csFontName.CompareNoCase(s_szaWindows95StandardFontName[i]) == 0)
         {
            entry->SetFontOrigin(FaceEntry::fotWindows95Standard);
            bFoundFont = TRUE;
         }
      }
      if(!bFoundFont)
         entry->SetFontOrigin(FaceEntry::fotWindows95Extra);
	}
}

/********************************/
/* TypefaceServer class support */
/********************************/

/*
// Constructor.
*/

TypefaceServer::TypefaceServer()
{
	m_fInitialized = FALSE;

	replacement_face = -1;

	FaceEntry::standard_variation_type = VARIATION_TYPE_WIN40;
}

/*
// Destructor.
*/

TypefaceServer::~TypefaceServer()
{
}

/*
// Add the preview face.
*/

void TypefaceServer::add_preview_face(void)
{
	PCSTR pfn = preview_face_name();

	if (pfn != NULL)
	{
		FaceEntry*  entry;

		if (face_list.add_entry(pfn, &entry))
		{
		/*
		// We need to add a "preview" variation to this entry.
		// Even though we only need to add it for one variation (because all
		// get mapped there eventually), it makes more sense to set them all
		// to allow the code to find something valid on the first try.
		*/
			entry->set_preview_variations();
		}
	}
}

/*
// Build the face list.
*/

BOOL TypefaceServer::build_face_list(void)
{
	if (!m_fInitialized)
	{
		return FALSE;
	}

/* Have the face list build itself. */

	if (!face_list.build())
	{
		return FALSE;
	}

/*
// We need to add the preview face.
*/

	add_preview_face();

/*
// Now try to set the default face as the replacement face.
*/

	ReadOnlyFile file;
	LOGFONT lf;

	if (((replacement_face = find_face(default_face_name, FALSE)) == -1)
	 || (!get_face_path(replacement_face, &lf, &file)))
	{
	/*
 	// The default face is not a good one. Try to find a good one.
	// Brute-force search the list top to bottom to find an existing font.
 	*/

		for (SHORT index = 0; index < face_list.count(); index++)
		{
		/* Make sure the face is one the user can choose. */

			FaceEntry* entry;

			if (((entry = face_list.get_face(index)) == NULL)
			 || (!entry->is_chooseable(FALSE)))
			{
				continue;
			}

		/* If we can address this font, then we're OK. */

			if (get_face_path(index, &lf, &file))
			{
				replacement_face = index;
				break;
			}
		}
	}

	if (replacement_face == -1)
	{
	/* We couldn't find any font files! What do we do? */
		return FALSE;
	}

	return TRUE;
}

/*
// Find a face.
*/

SHORT TypefaceServer::find_face(LPCSTR face_name, BOOL fReplace /*=TRUE*/)
{
	SHORT index; 

	if ((index = face_list.find_face(face_name)) == -1 && fReplace)
	{
	/*
	// The face is missing from the list. Use the default face.
	*/
		index = replacement_face;
	}
	return index;
}

/*
// Routine to initialize the font server.
*/

BOOL TypefaceServer::startup(char const *default_face)
{
/*
// Get the font preview file.
*/
	char *sb = m_font_preview_filename.GetBuffer(_MAX_PATH);
	int cb = ::GetPrivateProfileString(
							SECTION_Configuration,
							ENTRY_FontPreviewFile,
							"PMFONTS.FPV",
							sb,
							_MAX_PATH,
							GET_PMWAPP()->m_csMainIni);
	m_font_preview_filename.ReleaseBuffer(cb);

	if (!GetGlobalPathManager()->FileExists(m_font_preview_filename))
	{
		m_font_preview_filename.Empty();
	}

/*
// Make sure we got our font handle entry point.
*/

	default_face_name = default_face;

	m_fInitialized = TRUE;

/*
// Build the face list.
*/

	if (!build_face_list())
	{
		return FALSE;
	}

/* Success. */

	return TRUE;
}

/*
// This is a routine we can pass the the TrueType engine constructor
// to enable it to get the path name for a font.
//
// The typeface number should be as follows:
// bits			meaning
// ----			-------
//  15			 italic
//  14			 bold
// 13-0			 face list index
*/

int AFX_EXPORT TypefaceServer::get_face_path(int typeface, LOGFONT* plf, StorageFilePtr file)
{
//	od("Get face path: %x\r\n", typeface);

/*
// This is kinda' a kludge.
// We know that we only have the global typeface_server, so we just use it.
//
// If we change the callback to pass back a voidPTR for user data, then
// we pass back the address of typeface_server and thereby avoid violating
// the universe.
*/

	if (typeface == (int)TRUETYPE_TYPEFACE_MLSSYM)
	{
		file->set_name("MLSSYM.TT");
		return GFC_RESULT_file;
	}

	if (typeface == (int)TRUETYPE_TYPEFACE_ZBATS)
	{
		file->set_name("MLSZA.TT");
		return GFC_RESULT_file;
	}

	// Unpack the face index and style.

	FONT_STYLE TypefaceStyle = typeface_style(typeface);
	SHORT nTypefaceIndex = typeface_index(typeface);

/* Get the face entry. */

	FaceEntry* entry;
	if ((entry = typeface_server.face_list.get_face(nTypefaceIndex)) == NULL)
	{
		return GFC_RESULT_failure;
	}

/* Set up the file for the font that best matches this style. */

	int result;
	if ((result = entry->setup_file(TypefaceStyle, plf, file)) != GFC_RESULT_failure)
	{
		return result;
	}

/* Couldn't get the path for this face. Get the one for the replacement. */

	if ((typeface_server.replacement_face == -1)
	 || (nTypefaceIndex == typeface_server.replacement_face))
	{
	/* The second part should never happen! */
		return GFC_RESULT_failure;
	}

/* Return the info for the default face. */

	return get_face_path(typeface_for_fstyle(typeface_server.replacement_face, TypefaceStyle), plf, file);
}

/*
// This returns the name of the preview font.
// If will return NULL if there is none (so check!)
*/

PCSTR TypefaceServer::preview_face_name(void)
{
	if (!m_font_preview_filename.IsEmpty())
	{
		return "\x01Preview";
	}
	return NULL;
}

/*
// This returns the name of the font preview file.
// If will return NULL if there is none (so check!)
*/

PCSTR TypefaceServer::font_preview_filename(void)
{
	if (!m_font_preview_filename.IsEmpty())
	{
		return m_font_preview_filename;
	}
	return NULL;
}
