/*
// $Header: /PM8/App/CreateData.h 1     3/03/99 6:04p Gbeddow $
//
// record creation sturtures.
//
// $Log: /PM8/App/CreateData.h $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 1     2/25/99 4:00p Lwilson
// 
*/

#ifndef _CREATEDATA_H_
#define _CREATEDATA_H_

const WORD kUndefinedData	= 0;
const WORD kGraphicData		= 1;
const WORD kComponentData	= 2;

class CCreationData
{
public:

	WORD		cdDataType;
	WORD		cdDataSize;
};

class CGraphicData : public CCreationData
{
public:
	CGraphicData( GRAPHIC_CREATE_STRUCT* pData );

	GRAPHIC_CREATE_STRUCT*	cdData;
};

class CComponentData : public CCreationData
{
public:

	CComponentData( RComponentDocument* pData );

	RComponentDocument*		cdData;

};

#endif
