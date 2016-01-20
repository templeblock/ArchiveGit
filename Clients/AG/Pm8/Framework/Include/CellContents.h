// ****************************************************************************
// @Doc
// @Module CellContents.h |
//
//  File Name:			CellContents.h
//
//  Project:			Calendar Application
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RCellContents and supporting classes
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/CellContents.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_CELLCONTENTS_H_
#define		_CELLCONTENTS_H_

#pragma pack(push, BYTE_ALIGNMENT)

#ifndef _APPLICATIONGLOBALS_H_
#include	<ApplicationGlobals.h>
#endif

#ifndef _STANDALONEAPPLICATIONBASE_H_
#include	<StandaloneApplication.h>
#endif

#ifndef _COMPONENTDOCUMENT_H_
#include	<ComponentDocument.h>
#endif

#ifndef _COMPONENTTYPES_H_
#include	<ComponentTypes.h>
#endif

#ifndef _COMPONENTVIEW_H_
#include	<ComponentView.h>
#endif

#ifndef _TEXTINTERFACE_H_
#include	<TextInterface.h>
#endif

#ifndef _GRAPHICINTERFACE_H_
#include	<GraphicInterface.h>
#endif

#ifndef _THREADSUPPORT_H_
#include	<ThreadSupport.h>
#endif

#ifndef _CHUNKYSTORAGE_H_
#include	<ChunkyStorage.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class RGrid;

//
// @Class Holder for a single cell content data item
template<class componentInterface>
class FrameworkLinkage RCellContentItem
{
//
// @Access Internal data
protected:
	//
	// @CMember pointer to the data
	RComponentDataHolder*		m_pData;
	//
	// @CMember pointer to the view used in the current interface
	RComponentView*				m_pView;
	//
	// @CMember pointer to the document used in the current interface
	RComponentDocument*			m_pDoc;
	//
	// @CMember flag indicating the global doc/view is in use
	static BOOLEAN					m_gfDocViewInUse;
	//
	// @CMember identifier of the component type
	static YComponentType		m_gComponentType;
	//
	// @CMember identifier of the interface for this type
	static YInterfaceId			m_gInterfaceId;
	//
	// @CMember number of items of this type
	static uLONG					m_gNumItems;
	//
	// @CMember pointer to the view that supports this data
	static RComponentView*		m_gpView;
	//
	// @CMember pointer to the document that supports this data
	static RComponentDocument*	m_gpDocument;
	//
	// @CMember lock object for exclusive access to code fragments
	static RCriticalSection		m_gCriticalSection;
//
// @Access Construction/Destruction
public:
	//
	// @CMember default ctor
	RCellContentItem();
	//
	// @CMember copy ctor
	RCellContentItem( const RCellContentItem& item );
	//
	// @CMember dtor
	~RCellContentItem();
//
// @Access Internal support
protected:
	//
	// @CMember Get an interface to the item's component
	componentInterface*	GetInterface( const RRealRect& rcBounds, RView* pParent=NULL ) const;
	//
	// @CMember Release the interface to this item's component
	void						ReleaseInterface( componentInterface* pInterface ) const;
	//
	// @CMember Get the supporting view for this item
	RComponentView*		GetView( const RRealRect& rcBounds, RView* pParent ) const;
	//
	// @CMember Release the supporting view
	void						ReleaseView() const;
	//
	// @CMember Get the supporting document for this item
	RComponentDocument*	GetDocument() const;
	//
	// @CMember Release the supporting document
	void						ReleaseDocument() const;

//
// @Access Operations
public:
	//
	// @CMember assignment operator
	const RCellContentItem& operator=( const RCellContentItem& rhs );
	//
	// @CMember Set the data for this item
	void						SetData( RComponentDataHolder* pData);
	//
	// @CMember Get the data for this item
	const RComponentDataHolder*	GetData() const;
	//
	// @CMember Read this item from the given storage object
	void						Read( RChunkStorage& storage );
	//
	// @CMember Write this item to the given storage object
	void						Write( RChunkStorage& storage ) const;
	//
	// @CMember Render this item
	void						Render( RDrawingSurface& ds, const R2dTransform& transform, const RIntRect& rcRender, const RRealSize& nominalSize, const RRealRect& rcBounds ) const;
	//
	// @CMember Edit this item
	void						Edit( RIntRect& rcBounds, RView* pParent=NULL );
};


//
// @MFunc Construct an RCellContentItem
// @Syntax RCellContentItem<interface,data>()
// @Syntax RCellContentItem<interface,data>( const RCellContentItem<interface,data>& item )
// @Parm const RCellContentItem<interface,data>& | item | const reference to the content item to copy
//
template<class componentInterface>
RCellContentItem<componentInterface>::RCellContentItem() :
	m_pData( NULL ),
	m_pDoc( NULL ),
	m_pView( NULL )

{
	++m_gNumItems;
}

template<class componentInterface>
RCellContentItem<componentInterface>::RCellContentItem(
			const RCellContentItem<componentInterface>& item ) :
	m_pData( NULL ),
	m_pDoc( NULL ),
	m_pView( NULL )

{
	if( item.m_pData )
		{
		componentInterface*	pInterface	= GetInterface( RRealRect(RRealSize(100.0,100.0)) );
		pInterface->SetData( item.m_pData );
		m_pData	= pInterface->GetData();	
		ReleaseInterface( pInterface );
		}
	++m_gNumItems;
}

//
// @MFunc Destruct an RCellContentItem
// @Comm Destroys the document and view if this is the last one
//
template<class componentInterface>
RCellContentItem<componentInterface>::~RCellContentItem()
{
	delete m_pData;
	if( m_pDoc != m_gpDocument )
		{
		TpsAssert( m_pDoc==NULL, "An interface was deleted without calling ReleaseInterface" );
		delete m_pDoc;
		}
	TpsAssert( m_gNumItems>0, "In the destructor when we have no knowledge of any items!" );
	if( (--m_gNumItems) <= 0 )
		{
		//
		// NOTE: the document destructor will also delete the views
		//	Get an interface and tell it to clear its data
		RComponentDataInterface*	pInterface	= NULL;
		if ( m_gpView )
			pInterface	= static_cast<RComponentDataInterface*>( m_gpView->GetInterface( m_gInterfaceId ) );
		if( pInterface )
			pInterface->ClearData( );
		delete pInterface;
		delete m_gpDocument;
		m_gpDocument	= NULL;
		m_gpView			= NULL;
		}
}

//
// @MFunc Get an interface to this types component
// @RDesc pointer to the interface
//
template<class componentInterface>
componentInterface* RCellContentItem<componentInterface>::GetInterface(
			const RRealRect&	rcBounds,	// @Parm bounding rect for the view
			RView*				pParent )	// @Parm parent view
			const
{
	RLock	exclusiveExecute( m_gCriticalSection );
	componentInterface*	pInterface	= dynamic_cast<componentInterface*>( GetView(rcBounds,pParent)->GetInterface(m_gInterfaceId) );
	if( !pInterface )
		{
		TpsAssertAlways( "Failed to get an interface" );
		ThrowException( kUnknownError );
		}
	return pInterface;
}

//
// @MFunc Release the interface to this types component
// @RDesc nothing
//
template<class componentInterface>
void RCellContentItem<componentInterface>::ReleaseInterface(
			componentInterface*	pInterface )	// @Parm pointer to the interface
			const
{
	delete pInterface;
	ReleaseDocument();
	ReleaseView();
}

//
// @MFunc assignment operator
// @RDesc const reference to this RCellContentItem
//
template<class componentInterface>
const RCellContentItem<componentInterface>&
	RCellContentItem<componentInterface>::operator=(
			const RCellContentItem<componentInterface>& rhs )	// @Parm const reference to the item to copy
{
	delete m_pData;
	m_pData	= NULL;
	if( rhs.m_pData )
		{
		componentInterface*	pInterface = GetInterface( RRealRect(RRealSize(0.0,0.0)) );
		pInterface->SetData( rhs.m_pData );
		m_pData = pInterface->GetData();
		ReleaseInterface( pInterface );
		}
	return *this;
}

//
// @MFunc Get the supporting document for this item
// @RDesc pointer to the document
// @DevNote since a single document is used to support all instances of a given item type, access
//          to the document should be wrapped in critical sections so that nothing can happen to
//          it behind the callers back.  This may actually be implemented within the class at
//          a later date to simplify the usage of this class...
//
template<class componentInterface>
RComponentDocument* RCellContentItem<componentInterface>::GetDocument() const
{
	RComponentDocument*	pDoc	= m_gpDocument;
	if( !pDoc || m_gfDocViewInUse )
		{
		pDoc = ::GetComponentManager().CreateNewComponent( m_gComponentType, NULL, RComponentAttributes(), FALSE );

		if( !pDoc)
			{
			TpsAssertAlways( "Failed to create a component document" );
			ThrowException( kUnknownError );
			}
		if( !m_gfDocViewInUse )
			m_gpDocument = pDoc;
		}
	//
	// store the doc pointer
	RCellContentItem<componentInterface>* pme = const_cast<RCellContentItem<componentInterface>*>( this );
	pme->m_pDoc	= pDoc;

	return pDoc;
}

//
// @MFunc Release the supporting view
// @RDesc nothing
//
template<class componentInterface>
void RCellContentItem<componentInterface>::ReleaseDocument() const
{
	RCellContentItem<componentInterface>*	pme = const_cast<RCellContentItem<componentInterface>*>( this );
	if( m_pDoc != m_gpDocument )
		delete pme->m_pDoc;	// deletes the views too...
	else
		m_gfDocViewInUse = FALSE;
	pme->m_pDoc		= NULL;
	pme->m_pView	= NULL;
}

//
// @MFunc Get the supporting view for this type of content
// @RDesc pointer to the view
// @DevNote since a single view is used to support all instances of a given item type, access
//          to the view should be wrapped in critical sections so that nothing can happen to
//          it behind the callers back.  This may actually be implemented within the class at
//          a later date to simplify the usage of this class...
//
template<class componentInterface>
RComponentView* RCellContentItem<componentInterface>::GetView(
			const RRealRect&	rcBounds,	// @Parm bounding rect for the view
			RView*				pParent )	// @Parm parent view
			const
{
	RComponentView*	pView	= m_gpView;
	if( !pView || m_gfDocViewInUse )
		{
		//
		// We don't have a view yet, so create one
		// First, get the document...
		RComponentDocument*	pDoc	= GetDocument();
		//
		// Now create a view for it
		pView	= pDoc->CreateView( rcBounds, pParent );
		//
		// Setup for rendering
		RTextDefaultsInterface*	pTextDefaults = dynamic_cast<RTextDefaultsInterface*>( pView->GetInterface(kTextDefaultsInterfaceId) );
		if( pTextDefaults )
			{
			pTextDefaults->SetRenderEmptyContainer( FALSE );
			pTextDefaults->SetLeftRightIndents( 0, 0 );
			delete pTextDefaults;
			}
		//
		// Make sure we don't continue if we didn't succeed!
		if( !pView )
			ThrowException( kUnknownError );
		//
		// Add the view to the document's view list
		pDoc->AddRView( pView );
		if( !m_gfDocViewInUse )
			{
			m_gpView				= pView;
			m_gfDocViewInUse	= TRUE;
			}
		}
	else
		{
		//
		// Adjust the bounding rect of the current view to match the requested
		// bounding rect...
		pView->SetBoundingRect( YComponentBoundingRect(rcBounds) );
		m_gfDocViewInUse	= TRUE;
		}
	//
	// store the view pointer
	RCellContentItem<componentInterface>* pme = const_cast<RCellContentItem<componentInterface>*>( this );
	pme->m_pView	= pView;
	//
	// return the view
	return pView;
}


//
// @MFunc Release the supporting view
// @RDesc nothing
//
template<class componentInterface>
void RCellContentItem<componentInterface>::ReleaseView() const
{
	TpsAssert( m_pView==NULL || m_pView==m_gpView, "Releasing a view without releasing the document!" );
}

//
// @MFunc Set the data for this item
// @RDesc nothing
//
template<class componentInterface>
void RCellContentItem<componentInterface>::SetData(
			RComponentDataHolder*	pData )	// @Parm pointer to the item's new data
{
	//
	// First delete what we have now...
	delete m_pData;
	//
	// Now we can set it...
	m_pData	= pData;
}

//
// @MFunc Get the data for this item
// @RDesc const pointer to the data
template<class componentInterface>
const RComponentDataHolder* RCellContentItem<componentInterface>::GetData() const
{
	return m_pData;
}


//
// @MFunc Read this item from the given storage object
// @RDesc nothing
//
template<class componentInterface>
void RCellContentItem<componentInterface>::Read(
			RChunkStorage&	storage )	// @Parm the storage object to read from
{
	uWORD	uwTag;
	//
	// Delete the current contents...
	delete m_pData;
	m_pData = NULL;
	//
	// Load the 'data present' tag
	storage	>> uwTag;
	//
	// If data is present, then load it...
	if( uwTag )
		{
		componentInterface*	pInterface	= GetInterface( RRealRect(RRealSize(100.0,100.0)) );
		pInterface->Read( storage );
		m_pData	= pInterface->GetData();
		ReleaseInterface( pInterface );
		}
}


//
// @MFunc Write this item to the given storage object
// @RDesc nothing
//
template<class componentInterface>
void RCellContentItem<componentInterface>::Write(
			RChunkStorage&	storage )	// @Parm the storage object to write into
			const
{
	//
	// Write the 'data present' tag
	uWORD	uwTag	= uWORD( (m_pData==NULL)? FALSE : TRUE );
	storage << uwTag;
	//
	// Write the data if it's present...
	if( uwTag )
		{
		componentInterface*	pInterface	= GetInterface( RRealRect(RRealSize(100.0,100.0)) );
		pInterface->SetData( m_pData );
		pInterface->Write( storage );
		ReleaseInterface( pInterface );
		}
}


//
// @MFunc Render this item
// @RDesc nothing
//
template<class componentInterface>
void RCellContentItem<componentInterface>::Render(
			RDrawingSurface&		ds,				// @Parm the drawing surface to render into
			const R2dTransform&	transform,		// @Parm the transformation to apply
			const RIntRect&		rcRender,		// @Parm the device bounding rect (for clipping?)
			const RRealSize&		nominalSize,	// @Parm the nominal size of the item
			const RRealRect&		rcBounds )		// @Parm the target logical bounding rect
			const
{
	if( m_pData )
		{
		//
		// Compute the required scale factors to get from the nominal size to the final size
		RRealSize		finalSize( rcBounds.WidthHeight() );
		YScaleFactor	xScale( finalSize.m_dx / nominalSize.m_dx );
		YScaleFactor	yScale( finalSize.m_dy / nominalSize.m_dy );
		//
		// Create the nominal bounding rectangle
		RRealRect		rcNominal( rcBounds );
		rcNominal.Inflate( RRealSize((nominalSize.m_dx-finalSize.m_dx)/2,(nominalSize.m_dy-finalSize.m_dy)/2) );
		//
		// Create the transformation matrix
		R2dTransform	xform( transform );
		xform.PreScaleAboutPoint( rcNominal.m_Left+nominalSize.m_dx/2, rcNominal.m_Top+nominalSize.m_dy/2, xScale, yScale );
		//
		// Get an interface to the component
		componentInterface*	pInterface	= GetInterface( rcNominal );
		//
		// Set the data into the component...
		pInterface->SetData( m_pData );
		//
		// Render the component
		pInterface->Render( ds, xform, rcRender, rcNominal );
		//
		// We're done with the interface...
		ReleaseInterface( pInterface );
		}
}

//
// @MFunc Edit this item
//
// @RDesc nothing
//
template<class componentInterface>
void RCellContentItem<componentInterface>::Edit(
				RIntRect&	rcBounds,	// @Parm the bounding rect for the component view
				RView*		pParent )	// @Parm parent view
{
	//
	// Get the interface.  This gives us access to the component
	componentInterface*	pInterface	= GetInterface( rcBounds, pParent );
	//
	// Set our graphic into the component...
	pInterface->SetData( const_cast< RComponentDataHolder* >( GetData() ) );
	//
	// Edit the component...
	pInterface->EditData( );
	//
	// Store the new graphic from the component back into the cell contents...
	SetData( pInterface->GetData() );
	//
	// We're done with the interface...
	ReleaseInterface( pInterface );
}

typedef RCellContentItem<RTextInterface>		RCellText;

//
// @Class Holder for cell content data
class FrameworkLinkage RCellContents
{
//
// @Access Content data
public:
	//
	// @CMember the index of the cell to which this content belongs
	int					m_nCellIndex;
	//
	// @CMember generic flags
	uLONG					m_ulFlags;
	//
	// @CMember User text
	RCellText			m_Text;
	//
	// @CMember User graphic (either a square graphic OR a PSD Photo )
	RComponentView*	m_pSquareView;
	//
	// @CMember ID of the square component for us in the UI.
	uLONG					m_SquareComponentId;
//
// @Access Construction/Destruction
public:
	//
	// @CMember default ctor
	RCellContents();
	//
	// @CMember copy ctor
	RCellContents( const RCellContents& rhs );
	//
	// @CMember dtor
	~RCellContents( );

//
// @Access Operators
public:
	//
	// @CMember assignment operator
	const RCellContents& operator=( const RCellContents& rhs );
//
// @Access Accessors
public:
	//
	// @CMember get the setting of the given flag
	BOOLEAN	GetFlag( uLONG ulFlag ) const;
	//
	// @CMember Set or clear the given flag
	void		SetFlag( uLONG ulFlag, BOOLEAN fSet );

//
// @Access Operations
public:
	//
	// @CMember Read the contents from the given storage object
	virtual void	Read( RChunkStorage& storage );
	//
	// @CMember Write the contents to the given storage object
	virtual void	Write( RChunkStorage& storage ) const;
	//
	// @CMember Render the contents to the given drawing surface
	virtual void	Render( RDrawingSurface& ds, const R2dTransform& transform, const RGrid* pGrid, const RRealSize& nominalSize, const RRealRect& finalCellRect ) const;
};


//
// @MFunc get the setting of the given flag
// @RDesc TRUE if the flag is set else FALSE
//
inline BOOLEAN RCellContents::GetFlag(
			uLONG	ulFlag )	// @Parm the flag to get
			const
{
	return BOOLEAN( (m_ulFlags&ulFlag)? TRUE : FALSE );
}

//
// @MFunc Set or clear the given flag
// @RDesc nothing
//
inline void RCellContents::SetFlag(
			uLONG		ulFlag,	// @Parm the flag to set or clear
			BOOLEAN	fSet )	// @Parm the setting for the flag
{
	if( fSet )
		m_ulFlags |= ulFlag;
	else
		m_ulFlags	&= ~ulFlag;
}

//
// @Class Container for the array of cell contents
class FrameworkLinkage RCellContentArray : public RArray<RCellContents*>
{
protected:
	//
	// @CMember Find the slot where the given cell's contents is located
	YIterator		FindCellContents( int nCell ) const;

public:
	//
	// @CMember Get the contents for a particular cell
	RCellContents*	GetCellContents( int nCell );
	//
	// @CMember Set the contents for a particular cell
	RCellContents*	SetCellContents( int nCell, RCellContents* pContents );
	//
	// @CMember destructor
	~RCellContentArray();
};


//-------------------------------
// Inlines
//-------------------------------

//
// @MFunc Find the slot where the given cell's contents is located
// @RDesc the iterator
inline RCellContentArray::YIterator RCellContentArray::FindCellContents(
			int	nCell )	// @Parm the index of the cell
			const
{
	YIterator	itr	= Start();
	while( itr != End() )
		{
		if( (*itr)->m_nCellIndex == nCell )
			break;
		++itr;
		}
	return itr;
}

//
// @MFunc Get the contents for a particular cell
// @RDesc pointer to the contents or NULL if there are none
//
inline RCellContents* RCellContentArray::GetCellContents(
			int					nCell )				// @Parm index of the cell
{
	YIterator	itr	= FindCellContents( nCell );
	if( itr != End() )
		return *itr;
	return NULL;
}

//
// @MFunc Set the contents for a particular cell
// @RDesc pointer to the old cell contents or NULL if there were none...
inline RCellContents* RCellContentArray::SetCellContents(
			int nCell, RCellContents* pContents )
{
	YIterator		itr	= FindCellContents( nCell );
	RCellContents*	pOld;
	if( itr != End() )
		{
		//
		// already present...
		pOld	= *itr;
		//
		// Now, if we're setting content, do it.  otherwise remove it...
		if( pContents )
			{
			pContents->m_nCellIndex = nCell;
			*itr = pContents;
			}
		else
			RemoveAt( itr );
		}
	else
		{
		//
		// not already there...
		TpsAssert( pContents!=NULL, "Attempting to remove nonexistent cell contents" );
		pOld	= NULL;
		pContents->m_nCellIndex = nCell;
		InsertAtEnd( pContents );
		}
	return pOld;
}

//
// @MFunc RCellContentArray destructor
//
inline RCellContentArray::~RCellContentArray()
{
	//
	// Delete all entries...
	YIterator	itr	= Start();
	while( itr != End() )
		{
		delete *itr;
		++itr;
		}
}

inline RChunkStorage& operator<<( RChunkStorage& storage, const RCellContents& cell )
{
	cell.Write( storage );
	return storage;
}

inline RChunkStorage& operator>>( RChunkStorage& storage, RCellContents& cell )
{
	cell.Read( storage );
	return storage;
}

#pragma pack(pop)

#endif // !defined(_CELLCONTENTS_H_)
