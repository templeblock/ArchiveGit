// ****************************************************************************
//
//  File Name:			RURL.h
//
//  Project:			Framework
//
//  Author:				Claire Schendel
//
//  Description:		Declaration of the RURL class
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1997 Broderbund Software. All Rights Reserved.
//
// ****************************************************************************
#ifndef _RURL_H_
#define _RURL_H_

typedef	uLONG	YURLId;
const YURLId	kInvalidURLId = ((YURLId)-1);

enum eURLProtocol
{
	PROTOCOL_NONE = 0,
	PROTOCOL_HTTP,
	PROTOCOL_FILE,
	PROTOCOL_MAIL,
	PROTOCOL_FTP,
	PROTOCOL_GOPHER,
	PROTOCOL_PAGE,
	PROTOCOL_SOUNDFILE
};

typedef RList<RMBCString> YHTMLFileNamesCollection;

// ****************************************************************************
//
//  Class Name:	RURL
//
//  Description:	Definition of an HTML URL. 
//
// ****************************************************************************
//
class FrameworkLinkage RURL
{
	public:

		RURL();

		RURL( const RMBCString &strAdddress, eURLProtocol protocol, uLONG pageID = 0 );

		RURL( const RURL& rUrl ) { *this = rUrl; }

		virtual ~RURL() {}

	// Operations
		void					operator=( const RURL& url );
		BOOLEAN				operator==(const RURL& ) const; 

		const RMBCString&	GetURL();
		void					SetAddress( const RMBCString &strAddress ) { m_strAddress = strAddress; }
		const RMBCString&	GetAddress() const { return m_strAddress; }
		void					SetProtocol( eURLProtocol eProtocol ) { m_eProtocol = eProtocol; }
		eURLProtocol		GetProtocol() const { return m_eProtocol; }
		uLONG					GetPageID()	const { return m_ulPageID; }
		uLONG					GetID() const { return m_ulID; }

		BOOLEAN				CopyAttachedFiles( const RMBCString &strDestDirectory, YHTMLFileNamesCollection &rLogList ) const;

		void				Write( RArchive& archive ) const;
		void				Read( RArchive& archive );

		// Presswriter and WSD style serialization...
		void Serialize( CArchive &ar );
		friend FrameworkLinkage CArchive& operator <<( CArchive& ar, const RURL& rURL );
		friend FrameworkLinkage CArchive& operator >>( CArchive& ar, RURL& rURL );

	private:

		// URL address (i.e. broder.com) (redundant but needed by the application)
		RMBCString		m_strAddress;
		// type of URL (i.e. http://)
		eURLProtocol	m_eProtocol;
		// URL string Type + address 
		RMBCString		m_strURL;
		// page ID for PAGE_TYPE URL
		uLONG				m_ulPageID;
		// unique ID for the object
		uLONG				m_ulID;
		// sets the URL string from the parts
		void				SetURL();
		// sets the ID of the object
		void				SetID( uLONG ID ) {	m_ulID = ID; }
		// ID is set by the list, each URL has a unique ID in the list
		friend class 	RURLList;
};


inline void RURL::operator=( const RURL& url ) 
{
	m_strAddress = url.GetAddress();
	m_eProtocol = url.GetProtocol();
	m_ulPageID = url.GetPageID();
	m_ulID = url.GetID();
	SetURL();
}

inline BOOLEAN RURL::operator==(const RURL& url) const
{
	if( m_eProtocol == url.GetProtocol() &&
		m_ulPageID == url.GetPageID() &&
		!m_strAddress.iCompare( url.GetAddress() )  )
		return TRUE;
	else
		return FALSE;
}

#endif 