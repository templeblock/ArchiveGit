//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/UIImage.h 1     3/03/99 6:12p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/UIImage.h $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 1     2/05/99 8:51a Mwilson
// 
// 2     5/31/98 5:19p Hforman
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __UIIMAGE_H__
#define __UIIMAGE_H__

class CPUIImageInfo
{
public:
   // Operations
   enum ImageType
   {
      typePNG,
      typeDIB
   };

   CPUIImageInfo()
		{ SetType(typePNG); }

   // Get Operations
   ImageType GetType() const
	   { return m_enType;}

   long GetSize() const
	   { return m_lSize;}

   void GetResolution(CSize *pcsizeDims) const
	   { *pcsizeDims = m_csizeDims; }

	// Set Operations
	void SetType(ImageType enType)
		{ m_enType = enType;}

	void SetSize(long lSize)
		{ m_lSize = lSize; }

	void SetResolution(const CSize & csizeDims)
		{ m_csizeDims = csizeDims; }
private:
	ImageType		m_enType;
	long		m_lSize;
	CSize		m_csizeDims;
};

inline CPUIImageInfo::ImageType operator++(CPUIImageInfo::ImageType &refType,int)
{
   return refType = (CPUIImageInfo::ImageType)(refType+1);
}


#endif
