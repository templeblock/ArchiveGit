// ****************************************************************************
//
//  File Name:			URLList.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				C. Schendel
//
//  Description:		Definition of the RURL class 
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software, Inc. 
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME
#include "URL.h"
#include "FrameworkResourceIDs.h"

// this message must be handled and defined the same in the application
#define UM_GET_PAGE_FILENAME (WM_USER + 147)

// ****************************************************************************
//
//				RURL
//
// ****************************************************************************
//

RURL::RURL( )
		:	m_eProtocol( PROTOCOL_NONE ),
			m_ulPageID( 0 ),
			m_ulID( kInvalidURLId )
{	

}

RURL::RURL(  const RMBCString &strAddress, eURLProtocol protocol, uLONG pageID )
		:	m_strAddress( strAddress ),
			m_eProtocol( protocol ),
			m_ulPageID( pageID ),
			m_ulID( kInvalidURLId )
{	
	SetURL();
}

void RURL::SetURL()
{

	RMBCString Service;

	switch ( m_eProtocol )
	{
		case PROTOCOL_NONE:
			Service = "";
			break;

		case PROTOCOL_PAGE:
			Service = "";
#ifdef _WINDOWS
			{
				CString pagename;
				// get the current page address this can change
				AfxGetMainWnd()->SendMessage(UM_GET_PAGE_FILENAME, GetPageID(), (LPARAM)&pagename);
				m_strURL = (LPCSTR)pagename;
			}
#else
			UnimplementedCode( );
#endif // _WINDOWS
			break;

		case PROTOCOL_HTTP:
			Service = "http://";
			break;

		case PROTOCOL_FILE:
		case PROTOCOL_SOUNDFILE:
#ifdef _WINDOWS
			{	
				CString str = (LPCSTR)m_strAddress;
				// Create a block to scope the local variable.
				// Determine the name of the file we are linked to.
				int nFileNameStartsAt = str.ReverseFind( '\\' ) + 1;
				if (nFileNameStartsAt == -1)
				{
					// There was no path attached, so use the full file reference.
					m_strURL = str;
				}
				else
				{
					// Extract the filename section.
					m_strURL = str.Mid( nFileNameStartsAt );
				}
			}
#else
			UnimplementedCode( );
#endif // _WINDOWS

			return;

		case PROTOCOL_MAIL:
			Service = "mailto:";
			break;

		case PROTOCOL_FTP:
			Service = "ftp://";
			break;

		case PROTOCOL_GOPHER:
			Service = "gopher://";
			break;

		default:
			Service = "http://";
			break;
	}

	// if the address doesn't have the protocol add it in to create m_strURL
	// What about case sensitivity?
	if ( !strstr( (LPCSTR)m_strAddress, "://" ) )
		m_strURL = Service + m_strAddress;
	else
		m_strURL = m_strAddress;
}

// ****************************************************************************
//
//  Function Name:	RURLList::GetURL( )
//
//  Description:	Returns a string to the URL
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
const RMBCString &	RURL::GetURL()
{
	// Get the current name from the application
	if( m_eProtocol == PROTOCOL_PAGE )
	{
		m_strURL.Empty();
#ifdef _WINDOWS
			// Update page address
			AfxGetMainWnd()->SendMessage(UM_GET_PAGE_FILENAME, GetPageID(), (LPARAM)&m_strURL);
#else
			UnimplementedCode( );
#endif // _WINDOWS
	}
	else if( m_strURL.IsEmpty() )
		SetURL();

	return m_strURL;
}

void RURL::Write( RArchive& archive ) const
{
	archive << m_strAddress;
	long l = m_eProtocol;
	archive << l;
	archive << m_ulPageID;
	archive << m_ulID;	
}

void RURL::Read( RArchive& archive )
{
	archive >> m_strAddress;
	long l;
	archive >> l;
	m_eProtocol = (eURLProtocol)l;
	archive >> m_ulPageID;
	archive >> m_ulID;	
	SetURL();
}

// Copies an attached files into the specified directory, thus encapsulating
// the need for the owner to deal with this piece of the output generation process.
//
// The Log list is a list of strings which store the files which have been
// copied formated for output to the log file.
BOOLEAN RURL::CopyAttachedFiles( const RMBCString &strDestDirectory, YHTMLFileNamesCollection &rLogList ) const
{
	// Only check the file if this is a file type URL linkage.
	if (m_eProtocol == PROTOCOL_FILE || m_eProtocol == PROTOCOL_SOUNDFILE)
	{
		CFileStatus LinkedFileStatus;
		BOOL bFileExists = CFile::GetStatus( m_strAddress.operator CString(), LinkedFileStatus );
		if (!bFileExists)
		{
			CString strPrompt, strLoad;
			strLoad.LoadString( STRING_URL_FILE_NOT_FOUND );
			strPrompt.Format( strLoad, m_strAddress.operator CString() );
			RAlert rAlert;
 			if (rAlert.QueryUser((LPCSTR)strPrompt) == kAlertYes)
 			{
				return TRUE;
 			}
			else
			{
				// The file was not found.
				return FALSE;
			}
		}
		else
		{
			// Determine the name of the file we are linked to.
			int nFileNameStartsAt = m_strAddress.operator CString().ReverseFind( '\\' ) + 1;
			CString strFileName;
			if (nFileNameStartsAt == -1)
			{
				// There was no path attached, so use the full file reference.
				strFileName = m_strAddress.operator CString();
			}
			else
			{
				// Extract the filename section.
				strFileName = m_strAddress.operator CString().Mid( nFileNameStartsAt );
			}

			// Build the output filename including its path.
			CString strNewFile;
			strNewFile.Format( "%s\\%s", strDestDirectory.operator CString(), strFileName);

			// Copy the file to the publishing folder.
			if ( ::CopyFile( m_strAddress.operator CString(), strNewFile, FALSE ) == 0 )
			{
				TRACE( "Failure to copy file to the publishing folder.\n" );
				CString strPrompt, strLoad;
				strLoad.LoadString( STRING_URL_FILE_COPY_PROBLEM );
				strPrompt.Format( strLoad, m_strAddress );
				RAlert rAlert;
	 			if (rAlert.QueryUser((LPCSTR)strPrompt) == kAlertYes)
	 			{
					return TRUE;
	 			}
				else
				{
					return FALSE;
				}
			}
			// Add this file to the list of files to publish.
			CString strLogData;
			strLogData.Format( "%s\t%ld", (LPCTSTR)strFileName, LinkedFileStatus.m_mtime );
			rLogList.InsertAtEnd( strLogData );
		}
	}

	return TRUE;
}


// Serializes data in and out of a WSD or PressWriter compatible stream.
void RURL::Serialize( CArchive &ar )
{
	if (ar.IsLoading())
	{
		ar >> *this;
	}
	else
	{
		ar << *this;
	}
}


// Writes the URL data to a WSD or PressWriter compatible stream.
FrameworkLinkage CArchive& operator <<( CArchive& ar, const RURL& rURL )
{
	// Store a localized version number to avoid crashing PressWriter Schema.
	uWORD nVersion = 1;

	ar << nVersion;

	ar << rURL.m_strAddress.operator CString();
	ar << (uWORD)rURL.m_eProtocol;
	ar << rURL.m_ulPageID;
	ar << rURL.m_ulID;	

	return ar;
}

// Reads the URL data from a WSD or PressWriter compatible stream.
FrameworkLinkage CArchive& operator >>( CArchive& ar, RURL& rURL )
{
	// Load our localized version number.
	uWORD nVersion;
	ar >> nVersion;

	CString strAddress;
	ar >> strAddress;
	rURL.m_strAddress = strAddress;
	ar >> (uWORD&)rURL.m_eProtocol;
	ar >> rURL.m_ulPageID;
	ar >> rURL.m_ulID;

	return ar;
}
