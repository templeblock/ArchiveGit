// ****************************************************************************
//
//  File Name:			StandaloneDocument.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas, M. Houle
//
//  Description:		Declaration of the RStandaloneDocument class
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
//  $Logfile:: /PM8/Framework/Include/StandaloneDocument.h                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_STANDALONEDOCUMENT_H_
#define		_STANDALONEDOCUMENT_H_

#include		"Document.h"
#include		"VersionInfo.h"

#ifdef		_WINDOWS
	#define		FrameworkDocument	COleDocument
#else
	#define		FrameworkDocument	LSingleDoc
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RApplication;
class RDrawingSurface;
class RMergeData;
class RUndoManager;
class RView;
class RSaveAsData;
class RFileFormat;
class RFileFormatCollection;
class RLayoutInfo;
class RReadyMadeInfo;
class RFontSizeInfo;
class RBitmapImage;
class RCancelPrintingSignal;


// ****************************************************************************
//
//  Class Name:		RStandaloneDocument
//
//  Description:		The base class for standalone Renaissance documents.
//
//							The document class is responsible for providing an interface
//							to its data.
//
// ****************************************************************************
//
class FrameworkLinkage RStandaloneDocument : public FrameworkDocument, public RDocument
	{
	// Construction & Destruction & Intialization
	public :
													RStandaloneDocument( );
		virtual									~RStandaloneDocument( );
		virtual void							Initialize( RApplication* pApp, YDocumentType documentType );
		void										XInitialize( RApplication* pApp, YDocumentType documentType );
		virtual RStandaloneDocument*		CreateCopy( ) const;

	// Operations
	public :
		virtual BOOLEAN						SendAction( RAction* pAction );
		virtual void							BeginMultiUndo( );
		virtual void							CancelMultiUndo( );
		virtual void							EndMultiUndo( uWORD uwUndoStringId, uWORD uwRedoStringId );
		virtual const YActionId&			GetNextActionId( ) const;
		virtual RAction*						CreateScriptAction( const YActionId& actionId, RScript& script );
		virtual void							Read( RChunkStorage& storage, EReadReason reason );
		virtual void							Write( RChunkStorage& storage, EWriteReason reason ) const;
		RMBCString								GetDocumentFilename( ) const;
		void										SetDocumentFilename( const RMBCString& filename );
		virtual const RFontSizeInfo*		GetFontSizeInfo( ) const = 0;
		virtual BOOLEAN						CanIncorporateData( const RDataTransferSource& dataSource ) const;
		virtual RStandaloneDocument*		GetTopLevelDocument( ) const;
		YAlertValues							OnFileSave( BOOLEAN fSaveAs, RSaveAsData* pSaveData = NULL );
		void										OpenPlaceholder( const RMBCString& filename );
		virtual void							GetSaveFileFormats( RFileFormatCollection& fileFormatCollection ) const = 0;
		virtual void							SetDefaultFileFormat( ) = 0;
		virtual void							FreeDocumentContents( );
		void										Activate( );
		RChunkStorage*							GetStorage( ) const;
		virtual void							OnXCloseDocument( );
		virtual RBitmapImage*				GetPreviewBitmap( const RIntSize& previewSize, const RSolidColor& backgroundColor, int colorDepth ) const;
		virtual BOOLEAN						PreparePrinting( );
		virtual void							Print( RCancelPrintingSignal* pCancelPrintingSignal );
		virtual void							PrinterChanged( const RPrinter* pPrinter, const uLONG ulReason );
		virtual BOOLEAN						CanInsertComponent( const YComponentType& componentType ) const;
		YDocumentType							GetDocumentType( ) const;
		RPrinter*								GetPrinter( ) const;
		void										SetPrinter( RPrinter* pPrinter );
		virtual BOOLEAN						CanPrint( ) const;
		const RVersionInfo&					GetVersion( ) const;
		virtual void							InitialUpdate( );
		virtual void							SetDefaultTitle( );
		void										DoSelectDirectoryDialog( RSaveAsData* pSaveAsData, BOOLEAN *pbUseFullHTMLExtension, YFloatType *pfPageSizePercentage ) const;

	//	For allowing Merge information
	public :
		BOOLEAN									SelectMergeField( RMergeFieldContainer& container );
		virtual RMergeData*					GetMergeData( BOOLEAN fQuery );

	// Implementation
	private :
		void										DoFileSaveDialog( RSaveAsData* pSaveAsData ) const;

	// Public Command handlers
	public :
		void										OnClose( );

	// Protected Command handlers
	protected:
		virtual void							OnSave( );

	// Private Command handlers
	private :
		void										OnUpdateSave( RCommandUI& commandUI ) const;
		void										OnSaveAs( );
		void										OnUndo( );
		void										OnRedo( );
		void										OnRevertToSaved( );
		void										OnUpdateRevertToSaved( RCommandUI& commandUI ) const;
		void										OnPrint( );
		void										OnFastPrint( );
		void										OnUpdateFastPrint( RCommandUI& commandUI ) const;

	//	Member data
	protected :
		RMergeData*								m_pMergeData;
		RFileFormat*							m_pFileFormat;
		RUndoManager*							m_pUndoManager;

	//	Member data
	private :
		RChunkStorage*							m_pDocumentStorage;
		RFontSizeInfo*							m_pFontSizeInfo;
		YDocumentType							m_DocumentType;
		RPrinter*								m_pPrinter;
		RVersionInfo 							m_Version;

	// Command map
	public :
		virtual RCommandMapBase*				GetRCommandMap( ) const;

	private :
		static RCommandMap<RStandaloneDocument, RDocument> m_CommandMap;
		friend class RCommandMap<RStandaloneDocument, RDocument>;

	friend class RNativeFileFormat;

#ifdef	_WINDOWS
	//	ReScope some public CObject functionality
	public:
		//using FrameworkDocument::operator new;
		//using FrameworkDocument::operator delete;
		//using FrameworkDocument::Dump;

	public :
		virtual CDocument*					GetCDocument( ) const;
		virtual void							DeleteContents( );
		virtual BOOL							OnOpenDocument( LPCTSTR lpszPathName );
		virtual BOOL							SaveModified( );
		virtual HMENU							GetDefaultMenu( );
		virtual void							SetPathName( LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE );
		virtual void							OnCloseDocument( );

#endif	// _WINDOWS

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void							Validate( ) const = 0;
#endif
	};

// ****************************************************************************
//
//  Class Name:	RAutoCleanupAction
//
//  Description:	Class to automatically clean up actions.
//
// ****************************************************************************
//
class RAutoCleanupAction 
	{
	//	Constructor & destructor
	public :
												RAutoCleanupAction( RAction* pAction );
												~RAutoCleanupAction( );

	//	Operations
	public :
		void									Release( );

	// Members
	private :
		RAction*								m_pAction;
	} ;

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef		_WINDOWS

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::GetRCommandMap( )
//
//  Description:		Gets a pointer to this classes command map.
//
//  Returns:			See above
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCommandMapBase* RStandaloneDocument::GetRCommandMap( ) const
	{
	return &m_CommandMap;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::GetCDocument( )
//
//  Description:		Retrieves the underlying MFC object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline CDocument* RStandaloneDocument::GetCDocument( ) const
	{
	return (CDocument*)this;
	}

#endif		// _WINDOWS

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _STANDALONEDOCUMENT_H_
