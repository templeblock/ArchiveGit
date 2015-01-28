using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Media.Imaging; // BitmapImage
using ClassLibrary;
using System.ServiceModel;
using System.Linq;
using System.Xml.Linq;
using System.Windows.Media;
using System.Windows.Input;
using System.Windows.Browser;
using System.CodeDom;
using System.CodeDom.Compiler;
using System.IO;
using System.Collections;

#if false
namespace System.Windows.Xps
{
	//	[AttributeUsage(AttributeTargets.Delegate | AttributeTargets.Interface | AttributeTargets.Property | AttributeTargets.Method | AttributeTargets.Constructor | AttributeTargets.Enum | AttributeTargets.Struct | AttributeTargets.Class, AllowMultiple=false, Inherited=true)]
	internal sealed class FriendAccessAllowedAttribute : Attribute
	{
		// Methods
		public FriendAccessAllowedAttribute();
	}

	internal class VisualsToXpsDocument : SerializerWriterCollator
	{
		// Fields
		private object _currentUserState;
		private PrintTicket _documentPrintTicket;
		private PrintTicket _documentSequencePrintTicket;
		private PackageSerializationManager _manager;
		private MXDWSerializationManager _mxdwManager;
		private Package _mxdwPackage;
		private int _numberOfVisualsCollated;
		private ArrayList _printTicketSequences;
		private Hashtable _printTicketsTable;
		private PrintSystemDispatcherObject accessVerifier;
		private VisualsCollaterState currentState;
		private XpsDocument destinationDocument;
		private PrintQueue destinationPrintQueue;
		private bool isCompletionEventHandlerSet;
		private bool isPrintTicketEventHandlerSet;
		private bool isProgressChangedEventHandlerSet;
		private XpsDocumentWriter parentWriter;

		// Methods
		internal VisualsToXpsDocument(XpsDocumentWriter writer, PrintQueue printQueue);
		internal VisualsToXpsDocument(XpsDocumentWriter writer, XpsDocument document);
		internal VisualsToXpsDocument(XpsDocumentWriter writer, PrintQueue printQueue, PrintTicket documentSequencePrintTicket, PrintTicket documentPrintTicket);
		internal VisualsToXpsDocument(XpsDocumentWriter writer, XpsDocument document, PrintTicket documentSequencePrintTicket, PrintTicket documentPrintTicket);
		public override void BeginBatchWrite();
		public override void Cancel();
		public override void CancelAsync();
		//	  [SecurityCritical]
		private void CreateXPSDocument(string documentName);
		public override void EndBatchWrite();
		internal void ForwardUserPrintTicket(object sender, XpsSerializationPrintTicketRequiredEventArgs args);
		private void InitializeSequences();
		//	  [return: MarshalAs(UnmanagedType.U1)]
		//	  [SecurityCritical, SecurityTreatAsSafe]
		private bool MxdwConversionRequired(PrintQueue printQueue);
		private string MxdwInitializeOptimizationConversion(PrintQueue printQueue);
		internal void SetPrintTicketEventHandler(PackageSerializationManager manager);
		private void VerifyAccess();
		public override void Write(Visual visual);
		public override void Write(Visual visual, PrintTicket printTicket);
		public override void WriteAsync(Visual visual);
		public override void WriteAsync(Visual visual, object userSuppliedState);
		public override void WriteAsync(Visual visual, PrintTicket printTicket);
		public override void WriteAsync(Visual visual, PrintTicket printTicket, object userSuppliedState);
		//	  [return: MarshalAs(UnmanagedType.U1)]
		//	  [SecurityCritical, SecurityTreatAsSafe]
		private bool WriteVisual([MarshalAs(UnmanagedType.U1)] bool asyncMode, PrintTicket printTicket, PrintTicketLevel printTicketLevel, Visual visual);

		// Nested Types
		private enum VisualsCollaterState
		{
			kUninit,
			kSync,
			kAsync,
			kDone,
			kCancelled
		}
	}

	public enum XpsDocumentNotificationLevel
	{
		None,
		ReceiveNotificationEnabled,
		ReceiveNotificationDisabled
	}

	internal class XpsDocumentWriter : SerializerWriter
	{
		// Fields
		private object _currentUserState;
		private bool _isDocumentCloned;
		private PackageSerializationManager _manager;
		private MXDWSerializationManager _mxdwManager;
		private Package _mxdwPackage;
		private ArrayList _printTicketSequences;
		private Package _sourcePackage;
		private XpsDocument _sourceXpsDocument;
		private IXpsFixedDocumentSequenceReader _sourceXpsFixedDocumentSequenceReader;
		private int _writingCancelledEventHandlersCount;
		private ArrayList _writingProgressSequences;
		private WritingCancelledEventHandler<backing_store> _WritingCancelled;
		private WritingCompletedEventHandler<backing_store> _WritingCompleted;
		private WritingPrintTicketRequiredEventHandler<backing_store> _WritingPrintTicketRequired;
		private WritingProgressChangedEventHandler<backing_store> _WritingProgressChanged;
		private PrintSystemDispatcherObject accessVerifier;
		private DocumentWriterState currentState;
		private PrintTicket currentUserPrintTicket;
		private PrintTicketLevel currentWriteLevel;
		private XpsDocument destinationDocument;
		private PrintQueue destinationPrintQueue;

		// Events
		public event WritingCancelledEventHandler _WritingCancelled;
		public event WritingCompletedEventHandler _WritingCompleted;
		public event WritingPrintTicketRequiredEventHandler _WritingPrintTicketRequired;
		public event WritingProgressChangedEventHandler _WritingProgressChanged;
		public event WritingCancelledEventHandler WritingCancelled;
		public event WritingCompletedEventHandler WritingCompleted;
		public event WritingPrintTicketRequiredEventHandler WritingPrintTicketRequired;
		public event WritingProgressChangedEventHandler WritingProgressChanged;

		// Methods
		//	  [FriendAccessAllowed, PrintingPermission(SecurityAction.Demand, Level=PrintingPermissionLevel.DefaultPrinting)]
		internal XpsDocumentWriter(PrintQueue printQueue);
		//	  [FriendAccessAllowed, PrintingPermission(SecurityAction.Demand, Level=PrintingPermissionLevel.DefaultPrinting)]
		internal XpsDocumentWriter(XpsDocument document);
		//	  [SecurityCritical]
		internal XpsDocumentWriter(PrintQueue printQueue, object bogus);
		//	  [SecurityTreatAsSafe, SecurityCritical]
		//	  internal unsafe void BeginPrintFixedDocumentSequence(FixedDocumentSequence fixedDocumentSequence, int* modopt(IsImplicitlyDereferenced) printJobIdentifier);
		//	  [return: MarshalAs(UnmanagedType.U1)]
		//	  [SecurityCritical, SecurityTreatAsSafe]
		private bool BeginWrite([MarshalAs(UnmanagedType.U1)] bool batchMode, [MarshalAs(UnmanagedType.U1)] bool asyncMode, [MarshalAs(UnmanagedType.U1)] bool setPrintTicketHandler, PrintTicket printTicket, PrintTicketLevel printTicketLevel, [MarshalAs(UnmanagedType.U1)] bool printJobIdentifierSet);
		public override void CancelAsync();
		internal void CloneSourcePrintTicket(object sender, XpsSerializationPrintTicketRequiredEventArgs args);
		public override SerializerWriterCollator CreateVisualsCollator();
		public override SerializerWriterCollator CreateVisualsCollator(PrintTicket documentSequencePrintTicket, PrintTicket documentPrintTicket);
		//	  [SecurityCritical]
		private void CreateXPSDocument(string documentName);
		internal void EndBatchMode();
		internal void EndPrintFixedDocumentSequence();
		private void EndWrite([MarshalAs(UnmanagedType.U1)] bool disposeManager);
		private void EndWrite([MarshalAs(UnmanagedType.U1)] bool disposeManager, [MarshalAs(UnmanagedType.U1)] bool abort);
		internal void ForwardProgressChangedEvent(object sender, XpsSerializationProgressChangedEventArgs args);
		internal void ForwardUserPrintTicket(object sender, XpsSerializationPrintTicketRequiredEventArgs args);
		internal void ForwardWriteCompletedEvent(object sender, XpsSerializationCompletedEventArgs args);
		private void InitializeSequences();
		//	  [return: MarshalAs(UnmanagedType.U1)]
		//	  [SecurityCritical, SecurityTreatAsSafe]
		private bool MxdwConversionRequired(PrintQueue printQueue);
		//	  [SecurityCritical]
		private string MxdwInitializeOptimizationConversion(PrintQueue printQueue);
		//	  [return: MarshalAs(UnmanagedType.U1)]
		internal bool OnWritingCanceled(object sender, Exception exception);
		internal void OnWritingPrintTicketRequired(object sender, WritingPrintTicketRequiredEventArgs args);
		private void SaveAsXaml(object serializedObject, [MarshalAs(UnmanagedType.U1)] bool isSync);
		internal void SetCompletionEventHandler(PackageSerializationManager manager, object userState);
		internal void SetPrintTicketEventHandler(PackageSerializationManager manager, XpsSerializationPrintTicketRequiredEventHandler eventHandler);
		internal void SetProgressChangedEventHandler(PackageSerializationManager manager, object userState);
		internal WritingProgressChangeLevel TranslateProgressChangeLevel(XpsWritingProgressChangeLevel xpsChangeLevel);
		private void VerifyAccess();
		public void Write(string documentPath);
		public override void Write(DocumentPaginator documentPaginator);
		public override void Write(FixedDocument fixedDocument);
		public override void Write(FixedDocumentSequence fixedDocumentSequence);
		public override void Write(FixedPage fixedPage);
		public override void Write(Visual visual);
		public void Write(string documentPath, XpsDocumentNotificationLevel notificationLevel);
		public override void Write(DocumentPaginator documentPaginator, PrintTicket printTicket);
		public override void Write(FixedDocument fixedDocument, PrintTicket printTicket);
		public override void Write(FixedDocumentSequence fixedDocumentSequence, PrintTicket printTicket);
		public override void Write(FixedPage fixedPage, PrintTicket printTicket);
		public override void Write(Visual visual, PrintTicket printTicket);
		public void WriteAsync(string documentPath);
		public override void WriteAsync(DocumentPaginator documentPaginator);
		public override void WriteAsync(FixedDocument fixedDocument);
		public override void WriteAsync(FixedDocumentSequence fixedDocumentSequence);
		public override void WriteAsync(FixedPage fixedPage);
		public override void WriteAsync(Visual visual);
		public void WriteAsync(string documentPath, XpsDocumentNotificationLevel notificationLevel);
		public override void WriteAsync(DocumentPaginator documentPaginator, object userSuppliedState);
		public override void WriteAsync(DocumentPaginator documentPaginator, PrintTicket printTicket);
		public override void WriteAsync(FixedDocument fixedDocument, object userSuppliedState);
		public override void WriteAsync(FixedDocument fixedDocument, PrintTicket printTicket);
		public override void WriteAsync(FixedDocumentSequence fixedDocumentSequence, object userSuppliedState);
		public override void WriteAsync(FixedDocumentSequence fixedDocumentSequence, PrintTicket printTicket);
		public override void WriteAsync(FixedPage fixedPage, object userSuppliedState);
		public override void WriteAsync(FixedPage fixedPage, PrintTicket printTicket);
		public override void WriteAsync(Visual visual, object userSuppliedState);
		public override void WriteAsync(Visual visual, PrintTicket printTicket);
		public override void WriteAsync(DocumentPaginator documentPaginator, PrintTicket printTicket, object userSuppliedState);
		public override void WriteAsync(FixedDocument fixedDocument, PrintTicket printTicket, object userSuppliedState);
		public override void WriteAsync(FixedDocumentSequence fixedDocumentSequence, PrintTicket printTicket, object userSuppliedState);
		public override void WriteAsync(FixedPage fixedPage, PrintTicket printTicket, object userSuppliedState);
		public override void WriteAsync(Visual visual, PrintTicket printTicket, object userSuppliedState);

		// Properties
		internal PrintTicket CurrentUserPrintTicket { set; }
		internal PrintTicketLevel CurrentWriteLevel { set; }

		// Nested Types
		private enum DocumentWriterState
		{
			kRegularMode,
			kBatchMode,
			kDone,
			kCancelled
		}
	}

	[Serializable]
	internal class XpsWriterException : Exception
	{
		// Methods
		public XpsWriterException();
		public XpsWriterException(string message);
		protected XpsWriterException(SerializationInfo info, StreamingContext context);
		public XpsWriterException(string message, Exception innerException);
		internal static void ThrowException(string message);
	}
}



public enum CertificateEmbeddingOption
{
	InCertificatePart,
	InSignaturePart,
	NotEmbedded
}

public enum CompressionOption
{
	Fast = 2,
	Maximum = 1,
	Normal = 0,
	NotCompressed = -1,
	SuperFast = 3
}

public enum EncryptionOption
{
	None,
	RightsManagement
}

public enum PackageRelationshipSelectorType
{
	Id,
	Type
}

public enum TargetMode
{
	Internal,
	External
}

public enum VerifyResult
{
	Success,
	InvalidSignature,
	CertificateRequired,
	InvalidCertificate,
	ReferenceNotFound,
	NotSigned
}

internal interface IDataTransform
{
	// Methods
	Stream GetTransformedStream(Stream encodedDataStream, IDictionary transformContext);

	// Properties
	bool FixedSettings { get; }
	bool IsReady { get; }
	object TransformIdentifier { get; }
}

internal interface INode
{
	// Methods
	void CommitInternal();
	void Flush();
	PackagePart GetPart();

	// Properties
	Uri Uri { get; }
}




public abstract class PackagePart
{
	// Fields
	private CompressionOption _compressionOption;
	private Package _container;
	private ContentType _contentType;
	private bool _deleted;
	private bool _disposed;
	private bool _isRelationshipPart;
	private InternalRelationshipCollection _relationships;
	private List<Stream> _requestedStreams;
	private PackUriHelper.ValidatedPartUri _uri;

	// Methods
	protected PackagePart(Package package, Uri partUri)
		: this(package, partUri, null, CompressionOption.NotCompressed)
	{
	}

	protected PackagePart(Package package, Uri partUri, string contentType)
		: this(package, partUri, contentType, CompressionOption.NotCompressed)
	{
	}

	protected PackagePart(Package package, Uri partUri, string contentType, CompressionOption compressionOption)
	{
		this._compressionOption = CompressionOption.NotCompressed;
		if (package == null)
		{
			throw new ArgumentNullException("package");
		}
		if (partUri == null)
		{
			throw new ArgumentNullException("partUri");
		}
		Package.ThrowIfCompressionOptionInvalid(compressionOption);
		this._uri = PackUriHelper.ValidatePartUri(partUri);
		this._container = package;
		if (contentType == null)
		{
			this._contentType = null;
		}
		else
		{
			this._contentType = new ContentType(contentType);
		}
		this._requestedStreams = null;
		this._compressionOption = compressionOption;
		this._isRelationshipPart = PackUriHelper.IsRelationshipPartUri(partUri);
	}

	private void CheckInvalidState()
	{
		this.ThrowIfPackagePartDeleted();
		this.ThrowIfParentContainerClosed();
	}

	private void CleanUpRequestedStreamsList()
	{
		if (this._requestedStreams != null)
		{
			for (int i = this._requestedStreams.Count - 1; i >= 0; i--)
			{
				if (this.IsStreamClosed(this._requestedStreams[i]))
				{
					this._requestedStreams.RemoveAt(i);
				}
			}
		}
	}

	internal void ClearRelationships()
	{
		if (this._relationships != null)
		{
			this._relationships.Clear();
		}
	}

	internal void Close()
	{
		if (!this._disposed)
		{
			try
			{
				if (this._requestedStreams != null)
				{
					if (!this._deleted)
					{
						foreach (Stream stream in this._requestedStreams)
						{
							stream.Close();
						}
					}
					this._requestedStreams.Clear();
				}
				else if (this._container.InStreamingCreation)
				{
					this.GetStream(FileMode.CreateNew, this._container.FileOpenAccess).Close();
				}
			}
			finally
			{
				this._requestedStreams = null;
				this._relationships = null;
				this._container = null;
				this._disposed = true;
			}
		}
	}

	internal void CloseRelationships()
	{
		if (!this._deleted)
		{
			if (this._container.InStreamingCreation)
			{
				this.CloseStreamingRelationships();
			}
			else
			{
				this.FlushRelationships();
			}
		}
	}

	private void CloseStreamingRelationships()
	{
		if (!this._container.InStreamingCreation)
		{
			throw new IOException(SR.Get("MethodAvailableOnlyInStreamingCreation", new object[] { "CloseRelationships" }));
		}
		if (this._relationships != null)
		{
			this._relationships.CloseInStreamingCreationMode();
		}
	}

	public PackageRelationship CreateRelationship(Uri targetUri, TargetMode targetMode, string relationshipType)
	{
		return this.CreateRelationship(targetUri, targetMode, relationshipType, null);
	}

	public PackageRelationship CreateRelationship(Uri targetUri, TargetMode targetMode, string relationshipType, string id)
	{
		this.CheckInvalidState();
		this._container.ThrowIfReadOnly();
		this.EnsureRelationships();
		return this._relationships.Add(targetUri, targetMode, relationshipType, id);
	}

	public void DeleteRelationship(string id)
	{
		this.CheckInvalidState();
		this._container.ThrowIfReadOnly();
		this._container.ThrowIfInStreamingCreation("DeleteRelationship");
		if (id == null)
		{
			throw new ArgumentNullException("id");
		}
		InternalRelationshipCollection.ThrowIfInvalidXsdId(id);
		this.EnsureRelationships();
		this._relationships.Delete(id);
	}

	private void EnsureRelationships()
	{
		if (this._relationships == null)
		{
			this.ThrowIfRelationship();
			this._relationships = new InternalRelationshipCollection(this);
		}
	}

	internal void Flush()
	{
		if (this._requestedStreams != null)
		{
			foreach (Stream stream in this._requestedStreams)
			{
				if (stream.CanWrite)
				{
					stream.Flush();
				}
			}
		}
	}

	internal void FlushRelationships()
	{
		if ((this._relationships != null) && (this._container.FileOpenAccess != FileAccess.Read))
		{
			this._relationships.Flush();
		}
	}

	protected virtual string GetContentTypeCore()
	{
		throw new NotSupportedException(SR.Get("GetContentTypeCoreNotImplemented"));
	}

	public PackageRelationship GetRelationship(string id)
	{
		PackageRelationship relationshipHelper = this.GetRelationshipHelper(id);
		if (relationshipHelper == null)
		{
			throw new InvalidOperationException(SR.Get("PackagePartRelationshipDoesNotExist"));
		}
		return relationshipHelper;
	}

	private PackageRelationship GetRelationshipHelper(string id)
	{
		this.CheckInvalidState();
		this._container.ThrowIfWriteOnly();
		if (id == null)
		{
			throw new ArgumentNullException("id");
		}
		InternalRelationshipCollection.ThrowIfInvalidXsdId(id);
		this.EnsureRelationships();
		return this._relationships.GetRelationship(id);
	}

	public PackageRelationshipCollection GetRelationships()
	{
		return this.GetRelationshipsHelper(null);
	}

	public PackageRelationshipCollection GetRelationshipsByType(string relationshipType)
	{
		this.CheckInvalidState();
		this._container.ThrowIfWriteOnly();
		if (relationshipType == null)
		{
			throw new ArgumentNullException("relationshipType");
		}
		InternalRelationshipCollection.ThrowIfInvalidRelationshipType(relationshipType);
		return this.GetRelationshipsHelper(relationshipType);
	}

	private PackageRelationshipCollection GetRelationshipsHelper(string filterString)
	{
		this.CheckInvalidState();
		this._container.ThrowIfWriteOnly();
		this.EnsureRelationships();
		return new PackageRelationshipCollection(this._relationships, filterString);
	}

	public Stream GetStream()
	{
		this.CheckInvalidState();
		return this.GetStream(FileMode.OpenOrCreate, this._container.FileOpenAccess);
	}

	public Stream GetStream(FileMode mode)
	{
		this.CheckInvalidState();
		return this.GetStream(mode, this._container.FileOpenAccess);
	}

	public Stream GetStream(FileMode mode, FileAccess access)
	{
		this.CheckInvalidState();
		this.ThrowIfOpenAccessModesAreIncompatible(mode, access);
		Stream streamCore = this.GetStreamCore(mode, access);
		if (streamCore == null)
		{
			throw new IOException(SR.Get("NullStreamReturned"));
		}
		if (this._requestedStreams == null)
		{
			this._requestedStreams = new List<Stream>();
		}
		this.CleanUpRequestedStreamsList();
		this._requestedStreams.Add(streamCore);
		return streamCore;
	}

	protected abstract Stream GetStreamCore(FileMode mode, FileAccess access);
	private bool IsStreamClosed(Stream s)
	{
		return ((!s.CanRead && !s.CanSeek) && !s.CanWrite);
	}

	public bool RelationshipExists(string id)
	{
		return (this.GetRelationshipHelper(id) != null);
	}

	private void ThrowIfOpenAccessModesAreIncompatible(FileMode mode, FileAccess access)
	{
		Package.ThrowIfFileModeInvalid(mode);
		Package.ThrowIfFileAccessInvalid(access);
		if ((access == FileAccess.Read) && (((mode == FileMode.Create) || (mode == FileMode.CreateNew)) || ((mode == FileMode.Truncate) || (mode == FileMode.Append))))
		{
			throw new IOException(SR.Get("UnsupportedCombinationOfModeAccess"));
		}
		if (((this._container.FileOpenAccess == FileAccess.Read) && (access != FileAccess.Read)) || ((this._container.FileOpenAccess == FileAccess.Write) && (access != FileAccess.Write)))
		{
			throw new IOException(SR.Get("ContainerAndPartModeIncompatible"));
		}
	}

	private void ThrowIfPackagePartDeleted()
	{
		if (this._deleted)
		{
			throw new InvalidOperationException(SR.Get("PackagePartDeleted"));
		}
	}

	private void ThrowIfParentContainerClosed()
	{
		if (this._container == null)
		{
			throw new InvalidOperationException(SR.Get("ParentContainerClosed"));
		}
	}

	private void ThrowIfRelationship()
	{
		if (this.IsRelationshipPart)
		{
			throw new InvalidOperationException(SR.Get("RelationshipPartsCannotHaveRelationships"));
		}
	}

	// Properties
	public CompressionOption CompressionOption
	{
		get
		{
			this.CheckInvalidState();
			return this._compressionOption;
		}
	}

	public string ContentType
	{
		get
		{
			this.CheckInvalidState();
			if (this._contentType == null)
			{
				string contentTypeCore = this.GetContentTypeCore();
				if (contentTypeCore == null)
				{
					throw new InvalidOperationException(SR.Get("NullContentTypeProvided"));
				}
				this._contentType = new ContentType(contentTypeCore);
			}
			return this._contentType.ToString();
		}
	}

	internal bool IsClosed
	{
		get
		{
			return this._disposed;
		}
	}

	internal bool IsDeleted
	{
		get
		{
			return this._deleted;
		}
		set
		{
			this._deleted = value;
		}
	}

	internal bool IsRelationshipPart
	{
		get
		{
			return this._isRelationshipPart;
		}
	}

	public Package Package
	{
		get
		{
			this.CheckInvalidState();
			return this._container;
		}
	}

	public Uri Uri
	{
		get
		{
			this.CheckInvalidState();
			return this._uri;
		}
	}

	internal ContentType ValidatedContentType
	{
		get
		{
			return this._contentType;
		}
	}
}


internal class XpsManager : IDisposable
{
	// Fields
	private Dictionary<Uri, PackagePart> _cachedParts;
	private CompressionOption _compressionOption;
	private Dictionary<string, int> _contentTypes;
	internal static object _globalLock = new object();
	private Package _metroPackage;
	private bool _ownsPackage;
	internal static Dictionary<Uri, int> _packageCache = new Dictionary<Uri, int>();
	private bool _streaming;
	private Uri _uri;
	private XpsDocument _xpsDocument;

	// Methods
	internal XpsManager(Package metroPackage, CompressionOption compressionOption)
	{
		bool streaming = false;
		this._ownsPackage = false;
		if (metroPackage != null)
		{
			streaming = metroPackage.InStreamingCreation;
		}
		this.Initialize(metroPackage, compressionOption, streaming);
	}

	internal XpsManager(string path, FileAccess packageAccess, CompressionOption compressionOption)
	{
		this._ownsPackage = true;
		this._uri = new Uri(path, UriKind.RelativeOrAbsolute);
		if (!this._uri.IsAbsoluteUri)
		{
			this._uri = new Uri(new Uri(Directory.GetCurrentDirectory() + "/"), path);
		}
		Package package = PackageStore.GetPackage(this._uri);
		bool streaming = false;
		if (package == null)
		{
			if (packageAccess == FileAccess.Write)
			{
				package = Package.Open(path, FileMode.Create, packageAccess, FileShare.None, true);
				streaming = true;
			}
			else
			{
				package = Package.Open(path, (packageAccess == FileAccess.Read) ? FileMode.Open : FileMode.OpenOrCreate, packageAccess, (packageAccess == FileAccess.Read) ? FileShare.Read : FileShare.None);
			}
			this.AddPackageToCache(this._uri, package);
		}
		else
		{
			if ((packageAccess != FileAccess.Read) || (package.FileOpenAccess != FileAccess.Read))
			{
				throw new UnauthorizedAccessException();
			}
			this.AddPackageReference(this._uri);
		}
		this.Initialize(package, compressionOption, streaming);
	}

	public PackagePart AddDocumentPropertiesPart()
	{
		PackagePart documentPropertiesPart = this.GetDocumentPropertiesPart();
		if (documentPropertiesPart == null)
		{
			documentPropertiesPart = this.GenerateUniquePart(XpsS0Markup.CoreDocumentPropertiesType);
			this._metroPackage.CreateRelationship(documentPropertiesPart.Uri, TargetMode.Internal, XpsS0Markup.CorePropertiesRelationshipType);
		}
		return documentPropertiesPart;
	}

	private void AddPackageReference(Uri uri)
	{
		lock (_globalLock)
		{
			_packageCache[uri] += 1;
		}
	}

	private void AddPackageToCache(Uri uri, Package package)
	{
		lock (_globalLock)
		{
			_packageCache[uri] = 1;
		}
		PackageStore.AddPackage(uri, package);
	}

	public PackagePart AddSignatureDefinitionPart(PackagePart documentPart)
	{
		PackagePart signatureDefinitionPart = this.GetSignatureDefinitionPart(documentPart.Uri);
		if (signatureDefinitionPart == null)
		{
			signatureDefinitionPart = this.GenerateUniquePart(XpsS0Markup.SignatureDefintionType);
			documentPart.CreateRelationship(signatureDefinitionPart.Uri, TargetMode.Internal, XpsS0Markup.SignatureDefinitionRelationshipName);
		}
		return signatureDefinitionPart;
	}

	public XpsThumbnail AddThumbnail(XpsImageType imageType, INode parent, XpsThumbnail oldThumbnail)
	{
		if (oldThumbnail != null)
		{
			throw new XpsPackagingException(SR.Get("ReachPackaging_AlreadyHasThumbnail"));
		}
		if ((imageType != XpsImageType.JpegImageType) && (imageType != XpsImageType.PngImageType))
		{
			throw new XpsPackagingException(SR.Get("ReachPackaging_UnsupportedThumbnailImageType"));
		}
		return new XpsThumbnail(this, parent, this.GenerateUniquePart(ImageTypeToString(imageType)));
	}

	public void Close()
	{
		if (this._metroPackage != null)
		{
			if (this.IsWriter)
			{
				this._metroPackage.Flush();
			}
			if (this._ownsPackage)
			{
				this.RemovePackageReference(this._uri, this._metroPackage);
			}
			this._metroPackage = null;
			this._cachedParts = null;
			this._contentTypes = null;
		}
	}

	public void CollectPropertiesForSigning(Dictionary<Uri, Uri> dependents, XpsDigSigPartAlteringRestrictions restrictions)
	{
		PackagePart documentPropertiesPart = this.GetDocumentPropertiesPart();
		if (((restrictions & XpsDigSigPartAlteringRestrictions.CoreMetadata) != XpsDigSigPartAlteringRestrictions.None) && (documentPropertiesPart != null))
		{
			dependents[documentPropertiesPart.Uri] = documentPropertiesPart.Uri;
		}
	}

	public void CollectSignitureOriginForSigning(List<PackageRelationshipSelector> selectorList, XpsDigSigPartAlteringRestrictions restrictions)
	{
		if ((restrictions & XpsDigSigPartAlteringRestrictions.SignatureOrigin) != XpsDigSigPartAlteringRestrictions.None)
		{
			Uri signatureOriginUri = this.GetSignatureOriginUri();
			selectorList.Add(new PackageRelationshipSelector(signatureOriginUri, PackageRelationshipSelectorType.Type, XpsS0Markup.DitialSignatureRelationshipType));
		}
	}

	internal Uri CreateFragmentUri(int pageNumber)
	{
		string contentCounterKey = this.GetContentCounterKey(XpsS0Markup.FixedDocumentContentType);
		int num = 0;
		if (this._contentTypes.ContainsKey(contentCounterKey))
		{
			num = this._contentTypes[contentCounterKey] - 1;
		}
		return new Uri(string.Concat(new object[] { "/Documents/", num, "/Structure/Fragments/", pageNumber, ".frag" }), UriKind.Relative);
	}

	internal Uri CreateStructureUri()
	{
		string contentCounterKey = this.GetContentCounterKey(XpsS0Markup.FixedDocumentContentType);
		int num = 0;
		if (this._contentTypes.ContainsKey(contentCounterKey))
		{
			num = this._contentTypes[contentCounterKey] - 1;
		}
		return new Uri("/Documents/" + num + "/Structure/DocStructure.struct", UriKind.Relative);
	}

	public PrintTicket EnsurePrintTicket(Uri partUri)
	{
		PrintTicket ticket = null;
		PackagePart printTicketPart = this.GetPrintTicketPart(partUri);
		if (printTicketPart != null)
		{
			ticket = new PrintTicket(printTicketPart.GetStream());
		}
		return ticket;
	}

	public XpsThumbnail EnsureThumbnail(INode parent, PackagePart part)
	{
		XpsThumbnail thumbnail = null;
		PackagePart thumbnailPart = this.GetThumbnailPart(part);
		if (thumbnailPart != null)
		{
			thumbnail = new XpsThumbnail(this, parent, thumbnailPart);
		}
		return thumbnail;
	}

	public PackagePart GenerateObfuscatedFontPart()
	{
		if (this._metroPackage == null)
		{
			throw new ObjectDisposedException("XpsManager");
		}
		if (!this.IsWriter)
		{
			throw new XpsPackagingException(SR.Get("ReachPackaging_OnlyWriters"));
		}
		Uri partUri = PackUriHelper.CreatePartUri(new Uri("/Resources/" + Guid.NewGuid().ToString() + XpsS0Markup.ObfuscatedFontExt, UriKind.Relative));
		PackagePart part = this._metroPackage.CreatePart(partUri, XpsS0Markup.FontObfuscatedContentType.ToString(), this._compressionOption);
		this._cachedParts[partUri] = part;
		return part;
	}

	public PackagePart GeneratePart(ContentType contentType, Uri partUri)
	{
		if (this._metroPackage == null)
		{
			throw new ObjectDisposedException("XpsManager");
		}
		if (!this.IsWriter)
		{
			throw new XpsPackagingException(SR.Get("ReachPackaging_OnlyWriters"));
		}
		if (contentType == null)
		{
			throw new ArgumentNullException("contentType");
		}
		if (contentType.ToString().Length == 0)
		{
			throw new ArgumentException("contentType");
		}
		CompressionOption compressionOption = this._compressionOption;
		if ((contentType.AreTypeAndSubTypeEqual(XpsS0Markup.JpgContentType) || contentType.AreTypeAndSubTypeEqual(XpsS0Markup.PngContentType)) || (contentType.AreTypeAndSubTypeEqual(XpsS0Markup.TifContentType) || contentType.AreTypeAndSubTypeEqual(XpsS0Markup.WdpContentType)))
		{
			compressionOption = CompressionOption.NotCompressed;
		}
		PackagePart part = this._metroPackage.CreatePart(partUri, contentType.ToString(), compressionOption);
		this._cachedParts[partUri] = part;
		return part;
	}

	private Uri GeneratePrintTicketUri(object relatedPart)
	{
		if (relatedPart == null)
		{
			throw new ArgumentNullException("relatedPart");
		}
		string uriString = "";
		if (relatedPart is XpsFixedDocumentSequenceReaderWriter)
		{
			uriString = "/MetaData/Job_PT.xml";
		}
		else if (relatedPart is XpsFixedDocumentReaderWriter)
		{
			XpsFixedDocumentReaderWriter writer = relatedPart as XpsFixedDocumentReaderWriter;
			uriString = "/Documents/" + writer.DocumentNumber + "/Document_PT.xml";
		}
		else if (relatedPart is XpsFixedPageReaderWriter)
		{
			XpsFixedPageReaderWriter writer2 = relatedPart as XpsFixedPageReaderWriter;
			XpsFixedDocumentReaderWriter parent = (relatedPart as XpsFixedPageReaderWriter).Parent;
			uriString = string.Concat(new object[] { "/Documents/", parent.DocumentNumber, "/Page", writer2.PageNumber, "_PT.xml" });
		}
		return PackUriHelper.CreatePartUri(new Uri(uriString, UriKind.Relative));
	}

	public PackagePart GenerateUniquePart(ContentType contentType)
	{
		if (this._metroPackage == null)
		{
			throw new ObjectDisposedException("XpsManager");
		}
		if (!this.IsWriter)
		{
			throw new XpsPackagingException(SR.Get("ReachPackaging_OnlyWriters"));
		}
		if (contentType == null)
		{
			throw new ArgumentNullException("contentType");
		}
		if (ContentType.Empty.AreTypeAndSubTypeEqual(contentType))
		{
			throw new ArgumentException(SR.Get("ReachPackaging_InvalidType"));
		}
		Uri partUri = this.GenerateUniqueUri(contentType);
		return this.GeneratePart(contentType, partUri);
	}

	private Uri GenerateUniqueUri(ContentType contentType)
	{
		string str3;
		string contentCounterKey = this.GetContentCounterKey(contentType);
		string key = this.GetContentCounterKey(XpsS0Markup.FixedDocumentContentType);
		int num = this._contentTypes[contentCounterKey];
		int num2 = 0;
		Guid guid = Guid.NewGuid();
		if (this._contentTypes.ContainsKey(key))
		{
			num2 = this._contentTypes[key] - 1;
		}
		if (contentType.AreTypeAndSubTypeEqual(XpsS0Markup.DocumentSequenceContentType))
		{
			str3 = string.Format(CultureInfo.InvariantCulture, "{0}.fdseq", new object[] { contentCounterKey });
		}
		else if (contentType.AreTypeAndSubTypeEqual(XpsS0Markup.FixedDocumentContentType))
		{
			str3 = string.Format(CultureInfo.InvariantCulture, "/Documents/{0}/FixedDocument.fdoc", new object[] { num });
			string str4 = this.GetContentCounterKey(XpsS0Markup.FixedPageContentType);
			this._contentTypes[str4] = 1;
		}
		else if (contentType.AreTypeAndSubTypeEqual(XpsS0Markup.FixedPageContentType))
		{
			str3 = string.Format(CultureInfo.InvariantCulture, "/Documents/{0}/Pages/{1}.fpage", new object[] { num2, num });
		}
		else if (contentCounterKey.Equals("Dictionary", StringComparison.OrdinalIgnoreCase))
		{
			str3 = string.Format(CultureInfo.InvariantCulture, "/Resources/{0}.dict", new object[] { guid });
		}
		else if (contentCounterKey.Equals("Font", StringComparison.OrdinalIgnoreCase))
		{
			str3 = string.Format(CultureInfo.InvariantCulture, "/Resources/{0}.ttf", new object[] { guid });
		}
		else if (contentCounterKey.Equals("ColorContext", StringComparison.OrdinalIgnoreCase))
		{
			str3 = string.Format(CultureInfo.InvariantCulture, "/Resources/{0}.icc", new object[] { guid });
		}
		else if (contentCounterKey.Equals("ResourceDictionary", StringComparison.OrdinalIgnoreCase))
		{
			str3 = string.Format(CultureInfo.InvariantCulture, "/Resources/{0}.dict", new object[] { guid });
		}
		else if (contentCounterKey.Equals("Image", StringComparison.OrdinalIgnoreCase))
		{
			str3 = string.Format(CultureInfo.InvariantCulture, "/Resources/{0}.{1}", new object[] { guid, LookupImageExtension(contentType) });
		}
		else
		{
			str3 = string.Format(CultureInfo.InvariantCulture, "/{0}s/{0}_{1}.xaml", new object[] { contentCounterKey, num });
		}
		num++;
		this._contentTypes.Remove(contentCounterKey);
		this._contentTypes[contentCounterKey] = num;
		return PackUriHelper.CreatePartUri(new Uri(str3, UriKind.Relative));
	}

	private string GetContentCounterKey(ContentType contentType)
	{
		string subTypeComponent;
		if (ContentType.Empty.AreTypeAndSubTypeEqual(contentType))
		{
			throw new ArgumentException("contentType");
		}
		if (contentType.AreTypeAndSubTypeEqual(XpsS0Markup.DocumentSequenceContentType))
		{
			subTypeComponent = "FixedDocumentSequence";
		}
		else if (contentType.AreTypeAndSubTypeEqual(XpsS0Markup.FixedDocumentContentType))
		{
			subTypeComponent = "FixedDocument";
		}
		else if (contentType.AreTypeAndSubTypeEqual(XpsS0Markup.FixedPageContentType))
		{
			subTypeComponent = "FixedPage";
		}
		else if (contentType.AreTypeAndSubTypeEqual(XpsS0Markup.DocumentStructureContentType))
		{
			subTypeComponent = "DocumentStructure";
		}
		else if (contentType.AreTypeAndSubTypeEqual(XpsS0Markup.PrintTicketContentType))
		{
			subTypeComponent = "PrintTicket";
		}
		else if (contentType.AreTypeAndSubTypeEqual(XpsS0Markup.FontContentType))
		{
			subTypeComponent = "Font";
		}
		else if (contentType.AreTypeAndSubTypeEqual(XpsS0Markup.ColorContextContentType))
		{
			subTypeComponent = "ColorContext";
		}
		else if (contentType.AreTypeAndSubTypeEqual(XpsS0Markup.ResourceDictionaryContentType))
		{
			subTypeComponent = "ResourceDictionary";
		}
		else if (string.CompareOrdinal(contentType.TypeComponent.ToUpper(CultureInfo.InvariantCulture), "Image".ToUpper(CultureInfo.InvariantCulture)) == 0)
		{
			subTypeComponent = "Image";
		}
		else
		{
			subTypeComponent = contentType.SubTypeComponent;
		}
		if (!this._contentTypes.ContainsKey(subTypeComponent))
		{
			this._contentTypes[subTypeComponent] = 1;
		}
		return subTypeComponent;
	}

	public PackagePart GetDocumentPropertiesPart()
	{
		PackageRelationship documentPropertiesReationship = this.GetDocumentPropertiesReationship();
		PackagePart part = null;
		Package package = this._metroPackage;
		if (documentPropertiesReationship != null)
		{
			Uri partUri = PackUriHelper.ResolvePartUri(documentPropertiesReationship.SourceUri, documentPropertiesReationship.TargetUri);
			if (package.PartExists(partUri))
			{
				part = package.GetPart(partUri);
			}
		}
		return part;
	}

	private PackageRelationship GetDocumentPropertiesReationship()
	{
		PackageRelationship relationship = null;
		foreach (PackageRelationship relationship2 in this._metroPackage.GetRelationshipsByType(XpsS0Markup.CorePropertiesRelationshipType))
		{
			if (relationship != null)
			{
				throw new InvalidDataException(SR.Get("ReachPackaging_MoreThanOneMetaDataParts"));
			}
			relationship = relationship2;
		}
		return relationship;
	}

	public PackagePart GetPart(Uri uri)
	{
		if (this._metroPackage == null)
		{
			throw new ObjectDisposedException("XpsManager");
		}
		if (this._cachedParts.ContainsKey(uri))
		{
			return this._cachedParts[uri];
		}
		if (!this._metroPackage.PartExists(uri))
		{
			return null;
		}
		PackagePart part = this._metroPackage.GetPart(uri);
		this._cachedParts[uri] = part;
		return part;
	}

	public PackagePart GetPrintTicketPart(Uri documentUri)
	{
		PackagePart part = this._metroPackage.GetPart(documentUri);
		PackagePart part2 = null;
		if (part == null)
		{
			throw new InvalidDataException(SR.Get("ReachPackaging_InvalidDocUri"));
		}
		string printTicketRelationshipName = XpsS0Markup.PrintTicketRelationshipName;
		PackageRelationship relationship = null;
		foreach (PackageRelationship relationship2 in part.GetRelationshipsByType(printTicketRelationshipName))
		{
			if (relationship != null)
			{
				throw new InvalidDataException(SR.Get("ReachPackaging_MoreThanOnePrintTicketPart"));
			}
			relationship = relationship2;
		}
		if (relationship != null)
		{
			Uri partUri = PackUriHelper.ResolvePartUri(documentUri, relationship.TargetUri);
			part2 = this._metroPackage.GetPart(partUri);
		}
		return part2;
	}

	public PackagePart GetSignatureDefinitionPart(Uri documentUri)
	{
		PackagePart part = this._metroPackage.GetPart(documentUri);
		PackagePart part2 = null;
		if (part == null)
		{
			throw new InvalidDataException(SR.Get("ReachPackaging_InvalidDocUri"));
		}
		ContentType signatureDefintionType = XpsS0Markup.SignatureDefintionType;
		PackageRelationship relationship = null;
		foreach (PackageRelationship relationship2 in part.GetRelationshipsByType(XpsS0Markup.SignatureDefinitionRelationshipName))
		{
			if (relationship != null)
			{
				throw new InvalidDataException(SR.Get("ReachPackaging_MoreThanOneSigDefParts"));
			}
			relationship = relationship2;
		}
		if (relationship != null)
		{
			part2 = this._metroPackage.GetPart(PackUriHelper.ResolvePartUri(relationship.SourceUri, relationship.TargetUri));
		}
		return part2;
	}

	public Uri GetSignatureOriginUri()
	{
		PackageDigitalSignatureManager manager = new PackageDigitalSignatureManager(this._metroPackage);
		return manager.SignatureOrigin;
	}

	public PackagePart GetThumbnailPart(PackagePart parent)
	{
		PackageRelationship relationship = null;
		PackagePart part = null;
		PackageRelationshipCollection relationshipsByType = null;
		if (parent != null)
		{
			relationshipsByType = parent.GetRelationshipsByType(XpsS0Markup.ThumbnailRelationshipName);
		}
		else
		{
			relationshipsByType = this._metroPackage.GetRelationshipsByType(XpsS0Markup.ThumbnailRelationshipName);
		}
		foreach (PackageRelationship relationship2 in relationshipsByType)
		{
			if (relationship != null)
			{
				throw new InvalidDataException(SR.Get("ReachPackaging_MoreThanOneThumbnailPart"));
			}
			relationship = relationship2;
		}
		if (relationship != null)
		{
			part = this._metroPackage.GetPart(PackUriHelper.ResolvePartUri(relationship.SourceUri, relationship.TargetUri));
		}
		return part;
	}

	internal static PackagePart GetXpsDocumentStartingPart(Package package)
	{
		PackageRelationship relationship = null;
		PackagePart part = null;
		foreach (PackageRelationship relationship2 in package.GetRelationshipsByType(XpsS0Markup.ReachPackageStartingPartRelationshipType))
		{
			if (relationship != null)
			{
				throw new InvalidDataException(SR.Get("ReachPackaging_MoreThanOneStartingParts"));
			}
			relationship = relationship2;
		}
		if (relationship != null)
		{
			Uri partUri = PackUriHelper.ResolvePartUri(relationship.SourceUri, relationship.TargetUri);
			if (package.PartExists(partUri))
			{
				part = package.GetPart(partUri);
			}
		}
		return part;
	}

	public static ContentType ImageTypeToString(XpsImageType imageType)
	{
		ContentType type = new ContentType("");
		switch (imageType)
		{
			case XpsImageType.PngImageType:
				return XpsS0Markup.PngContentType;

			case XpsImageType.JpegImageType:
				return XpsS0Markup.JpgContentType;

			case XpsImageType.TiffImageType:
				return XpsS0Markup.TifContentType;

			case XpsImageType.WdpImageType:
				return XpsS0Markup.WdpContentType;
		}
		return type;
	}

	private void Initialize(Package metroPackage, CompressionOption compressionOption, bool streaming)
	{
		if (metroPackage == null)
		{
			throw new ArgumentNullException("Package");
		}
		this._xpsDocument = null;
		this._metroPackage = metroPackage;
		this._compressionOption = compressionOption;
		this._streaming = streaming;
		this._contentTypes = new Dictionary<string, int>(11);
		this._cachedParts = new Dictionary<Uri, PackagePart>(11);
	}

	private static string LookupImageExtension(ContentType contentType)
	{
		string pngExtension = XpsS0Markup.PngExtension;
		if (contentType.AreTypeAndSubTypeEqual(XpsS0Markup.JpgContentType))
		{
			return XpsS0Markup.JpgExtension;
		}
		if (contentType.AreTypeAndSubTypeEqual(XpsS0Markup.PngContentType))
		{
			return XpsS0Markup.PngExtension;
		}
		if (contentType.AreTypeAndSubTypeEqual(XpsS0Markup.TifContentType))
		{
			return XpsS0Markup.TifExtension;
		}
		if (contentType.AreTypeAndSubTypeEqual(XpsS0Markup.WdpContentType))
		{
			pngExtension = XpsS0Markup.WdpExtension;
		}
		return pngExtension;
	}

	public static string MakeRelativePath(Uri baseUri, Uri fileUri)
	{
		Uri uri = new Uri("http://dummy");
		if (!baseUri.IsAbsoluteUri)
		{
			baseUri = new Uri(uri, baseUri);
		}
		if (!fileUri.IsAbsoluteUri)
		{
			fileUri = new Uri(uri, fileUri);
		}
		Uri uri3 = new Uri(baseUri.MakeRelativeUri(fileUri).GetComponents(UriComponents.SerializationInfoString, UriFormat.SafeUnescaped), UriKind.RelativeOrAbsolute);
		return uri3.GetComponents(UriComponents.SerializationInfoString, UriFormat.UriEscaped);
	}

	private void RemovePackageReference(Uri uri, Package package)
	{
		int num = 0;
		lock (_globalLock)
		{
			num = _packageCache[uri];
			num--;
			if (num > 0)
			{
				_packageCache[uri] = num;
			}
			else
			{
				_packageCache.Remove(uri);
			}
		}
		if (num <= 0)
		{
			PackageStore.RemovePackage(uri);
			package.Close();
		}
	}

	internal static void SetXpsDocumentStartingPart(Package package, PackagePart startingPart)
	{
		if (package.FileOpenAccess == FileAccess.Read)
		{
			throw new IOException(SR.Get("ReachPackaging_CannotModifyReadOnlyContainer"));
		}
		if (startingPart == null)
		{
			throw new ArgumentNullException("startingPart");
		}
		if (startingPart.Package != package)
		{
			throw new ArgumentException(SR.Get("ReachPackaging_PartFromDifferentContainer"));
		}
		package.CreateRelationship(startingPart.Uri, TargetMode.Internal, XpsS0Markup.ReachPackageStartingPartRelationshipType);
	}

	public PackageDigitalSignature Sign(IEnumerable<Uri> partList, X509Certificate certificate, bool embedCertificate, IEnumerable<PackageRelationshipSelector> relationshipSelectors, string id)
	{
		if (this._metroPackage == null)
		{
			throw new ObjectDisposedException("XpsManager");
		}
		PackageDigitalSignatureManager manager = new PackageDigitalSignatureManager(this._metroPackage);
		if (embedCertificate)
		{
			manager.CertificateOption = CertificateEmbeddingOption.InCertificatePart;
		}
		else
		{
			manager.CertificateOption = CertificateEmbeddingOption.NotEmbedded;
		}
		if (id != null)
		{
			return manager.Sign(partList, certificate, relationshipSelectors, id);
		}
		return manager.Sign(partList, certificate, relationshipSelectors);
	}

	public static bool SupportedImageType(ContentType imageContentType)
	{
		bool flag = false;
		if ((!imageContentType.AreTypeAndSubTypeEqual(XpsS0Markup.JpgContentType) && !imageContentType.AreTypeAndSubTypeEqual(XpsS0Markup.PngContentType)) && (!imageContentType.AreTypeAndSubTypeEqual(XpsS0Markup.TifContentType) && !imageContentType.AreTypeAndSubTypeEqual(XpsS0Markup.WdpContentType)))
		{
			return flag;
		}
		return true;
	}

	void IDisposable.Dispose()
	{
		this.Close();
	}

	public void WriteEmptyPrintTicket(XpsPartBase relatedPart, PackagePart metroPart)
	{
		if (relatedPart == null)
		{
			throw new ArgumentNullException("relatedPart");
		}
		if (metroPart == null)
		{
			throw new ArgumentNullException("metroPart");
		}
		if (this.Streaming)
		{
			Uri partUri = this.GeneratePrintTicketUri(relatedPart);
			PackagePart part = this.GeneratePart(XpsS0Markup.PrintTicketContentType, partUri);
			string uriString = MakeRelativePath(metroPart.Uri, part.Uri);
			metroPart.CreateRelationship(new Uri(uriString, UriKind.Relative), TargetMode.Internal, XpsS0Markup.PrintTicketRelationshipName);
			Stream outStream = part.GetStream(FileMode.Create);
			new PrintTicket().SaveTo(outStream);
			outStream.Close();
		}
	}

	public void WritePrintTicket(XpsPartBase relatedPart, PackagePart metroPart, PrintTicket printTicket)
	{
		Stream stream;
		if (relatedPart == null)
		{
			throw new ArgumentNullException("relatedPart");
		}
		if (metroPart == null)
		{
			throw new ArgumentNullException("metroPart");
		}
		if (printTicket == null)
		{
			throw new ArgumentNullException("printTicket");
		}
		Uri partUri = this.GeneratePrintTicketUri(relatedPart);
		PackagePart part = this.GeneratePart(XpsS0Markup.PrintTicketContentType, partUri);
		string uriString = MakeRelativePath(metroPart.Uri, part.Uri);
		metroPart.CreateRelationship(new Uri(uriString, UriKind.Relative), TargetMode.Internal, XpsS0Markup.PrintTicketRelationshipName);
		if (this._metroPackage.FileOpenAccess == FileAccess.Write)
		{
			stream = part.GetStream(FileMode.Create);
		}
		else
		{
			stream = part.GetStream(FileMode.OpenOrCreate);
		}
		printTicket.SaveTo(stream);
		stream.Close();
	}

	// Properties
	public bool IsReader
	{
		get
		{
			if (this._metroPackage == null)
			{
				return false;
			}
			return (this._metroPackage.FileOpenAccess != FileAccess.Write);
		}
	}

	public bool IsWriter
	{
		get
		{
			if (this._metroPackage == null)
			{
				return false;
			}
			return (this._metroPackage.FileOpenAccess != FileAccess.Read);
		}
	}

	public Package MetroPackage
	{
		get
		{
			return this._metroPackage;
		}
	}

	public PackagePart StartingPart
	{
		get
		{
			PackagePart xpsDocumentStartingPart = null;
			if (this._metroPackage != null)
			{
				xpsDocumentStartingPart = GetXpsDocumentStartingPart(this._metroPackage);
			}
			return xpsDocumentStartingPart;
		}
		set
		{
			if (this._metroPackage == null)
			{
				throw new ObjectDisposedException("XpsManager");
			}
			if (!this.Streaming && (GetXpsDocumentStartingPart(this._metroPackage) != null))
			{
				throw new XpsPackagingException(SR.Get("ReachPackaging_AlreadyHasStartingPart"));
			}
			SetXpsDocumentStartingPart(this._metroPackage, value);
		}
	}

	public bool Streaming
	{
		get
		{
			return this._streaming;
		}
	}

	public XpsDocument XpsDocument
	{
		get
		{
			return this._xpsDocument;
		}
		set
		{
			this._xpsDocument = value;
		}
	}
}


public abstract class XpsPartBase
{
	// Fields
	private Uri _uri;
	private XpsManager _xpsManager;

	// Methods
	internal XpsPartBase(XpsManager xpsManager)
	{
		if (xpsManager == null)
		{
			throw new ArgumentNullException("xpsManager");
		}
		this._xpsManager = xpsManager;
	}

	internal virtual void CommitInternal()
	{
		if (this._xpsManager != null)
		{
			this._xpsManager = null;
		}
	}

	// Properties
	internal XpsManager CurrentXpsManager
	{
		get
		{
			return this._xpsManager;
		}
	}

	public Uri Uri
	{
		get
		{
			return this._uri;
		}
		set
		{
			this._uri = value;
		}
	}
}



internal class XpsDocument : XpsPartBase, INode, IDisposable
{
	// Fields
	private bool _disposed;
	private bool _isInDocumentStage;
	private Package _opcPackage;
	private ReadOnlyCollection<XpsDigitalSignature> _reachSignatureList;
	private Collection<XpsDigitalSignature> _reachSignatures;
	private XpsThumbnail _thumbnail;

	// Methods
	public XpsDocument(Package package)
		: this(package, CompressionOption.Maximum)
	{
	}

	public XpsDocument(Package package, CompressionOption compressionOption)
		: this(package, compressionOption, null)
	{
	}

	public XpsDocument(string path, FileAccess packageAccess)
		: this(path, packageAccess, CompressionOption.Maximum)
	{
	}

	public XpsDocument(Package package, CompressionOption compressionOption, string path)
		: base(new XpsManager(package, compressionOption))
	{
		if (path != null)
		{
			base.Uri = new Uri(path, UriKind.RelativeOrAbsolute);
		}
		base.CurrentXpsManager.XpsDocument = this;
		this.Initialize();
	}

	public XpsDocument(string path, FileAccess packageAccess, CompressionOption compressionOption)
		: base(new XpsManager(path, packageAccess, compressionOption))
	{
		if (path == null)
		{
			throw new ArgumentNullException();
		}
		base.Uri = new Uri(path, UriKind.RelativeOrAbsolute);
		if (!base.Uri.IsAbsoluteUri)
		{
			base.Uri = new Uri(new Uri(Directory.GetCurrentDirectory() + "/"), base.Uri);
		}
		base.CurrentXpsManager.XpsDocument = this;
		this.Initialize();
	}

	public IXpsFixedDocumentSequenceWriter AddFixedDocumentSequence()
	{
		this.CheckDisposed();
		if (base.CurrentXpsManager == null)
		{
			throw new InvalidOperationException(SR.Get("ReachPackaging_DocumentWasClosed"));
		}
		if (this._isInDocumentStage)
		{
			throw new XpsPackagingException(SR.Get("ReachPackaging_AlreadyHasRootSequenceOrDocument"));
		}
		if (!base.CurrentXpsManager.Streaming && (base.CurrentXpsManager.StartingPart != null))
		{
			throw new XpsPackagingException(SR.Get("ReachPackaging_AlreadyHasRootSequenceOrDocument"));
		}
		PackagePart part = base.CurrentXpsManager.GenerateUniquePart(XpsS0Markup.DocumentSequenceContentType);
		XpsFixedDocumentSequenceReaderWriter writer = new XpsFixedDocumentSequenceReaderWriter(base.CurrentXpsManager, this, part);
		base.CurrentXpsManager.StartingPart = ((INode)writer).GetPart();
		return writer;
	}

	private XpsDigitalSignature AddSignature(PackageDigitalSignature packSignature)
	{
		XpsDigitalSignature item = new XpsDigitalSignature(packSignature, this);
		this._reachSignatures.Add(item);
		return item;
	}

	public XpsThumbnail AddThumbnail(XpsImageType imageType)
	{
		this.CheckDisposed();
		if (base.CurrentXpsManager == null)
		{
			throw new InvalidOperationException(SR.Get("ReachPackaging_DocumentWasClosed"));
		}
		this._thumbnail = base.CurrentXpsManager.AddThumbnail(imageType, this, this.Thumbnail);
		base.CurrentXpsManager.MetroPackage.CreateRelationship(this._thumbnail.Uri, TargetMode.Internal, XpsS0Markup.ThumbnailRelationshipName);
		return this._thumbnail;
	}

	private void CheckDisposed()
	{
		if (this._disposed)
		{
			throw new ObjectDisposedException("XpsDocument");
		}
	}

	public void Close()
	{
		this.Dispose(true);
		GC.SuppressFinalize(this);
	}

	internal void CollectSelfAndDependents(Dictionary<Uri, Uri> dependentList, List<PackageRelationshipSelector> selectorList, XpsDigSigPartAlteringRestrictions restrictions)
	{
		(this.FixedDocumentSequenceReader as XpsFixedDocumentSequenceReaderWriter).CollectSelfAndDependents(dependentList, selectorList, restrictions);
		this.EnsureThumbnail();
		if (this._thumbnail != null)
		{
			dependentList[this._thumbnail.Uri] = this._thumbnail.Uri;
		}
		selectorList.Add(new PackageRelationshipSelector(PackUriHelper.PackageRootUri, PackageRelationshipSelectorType.Type, XpsS0Markup.ReachPackageStartingPartRelationshipType));
		selectorList.Add(new PackageRelationshipSelector(PackUriHelper.PackageRootUri, PackageRelationshipSelectorType.Type, XpsS0Markup.ThumbnailRelationshipName));
		if ((restrictions & XpsDigSigPartAlteringRestrictions.CoreMetadata) != XpsDigSigPartAlteringRestrictions.None)
		{
			base.CurrentXpsManager.MetroPackage.Flush();
			selectorList.Add(new PackageRelationshipSelector(PackUriHelper.PackageRootUri, PackageRelationshipSelectorType.Type, XpsS0Markup.CorePropertiesRelationshipType));
		}
		base.CurrentXpsManager.CollectSignitureOriginForSigning(selectorList, restrictions);
		base.CurrentXpsManager.CollectPropertiesForSigning(dependentList, restrictions);
	}

	internal override void CommitInternal()
	{
		if (base.CurrentXpsManager == null)
		{
			throw new InvalidOperationException(SR.Get("ReachPackaging_DocumentWasClosed"));
		}
		base.CommitInternal();
	}

	[FriendAccessAllowed]
	internal PackageSerializationManager CreateAsyncSerializationManager(bool bBatchMode)
	{
		return new XpsSerializationManagerAsync(new XpsPackagingPolicy(this), bBatchMode);
	}

	[FriendAccessAllowed]
	internal PackageSerializationManager CreateSerializationManager(bool bBatchMode)
	{
		PackageSerializationManager manager = null;
		XpsPackagingPolicy packagingPolicy = new XpsPackagingPolicy(this);
		if (packagingPolicy != null)
		{
			manager = new XpsSerializationManager(packagingPolicy, bBatchMode);
		}
		return manager;
	}

	[FriendAccessAllowed]
	internal static XpsDocument CreateXpsDocument(Stream dataStream)
	{
		Package package = Package.Open(dataStream, FileMode.CreateNew, FileAccess.Write, true);
		return new XpsDocument(package) { OpcPackage = package };
	}

	public static XpsDocumentWriter CreateXpsDocumentWriter(XpsDocument xpsDocument)
	{
		return new XpsDocumentWriter(xpsDocument);
	}

	protected virtual void Dispose(bool disposing)
	{
		if (!this._disposed)
		{
			if (disposing)
			{
				this._thumbnail = null;
				this._reachSignatures = null;
				this._reachSignatureList = null;
				this._opcPackage = null;
				base.CurrentXpsManager.Close();
				this.CommitInternal();
			}
			this._disposed = true;
		}
	}

	[FriendAccessAllowed]
	internal void DisposeSerializationManager()
	{
	}

	[FriendAccessAllowed]
	internal void DisposeXpsDocument()
	{
		if (this._opcPackage != null)
		{
			this._opcPackage.Close();
		}
	}

	private void EnsureSignatures()
	{
		if (this._reachSignatures == null)
		{
			this._reachSignatures = new Collection<XpsDigitalSignature>();
			PackageDigitalSignatureManager manager = new PackageDigitalSignatureManager(base.CurrentXpsManager.MetroPackage);
			foreach (PackageDigitalSignature signature in manager.Signatures)
			{
				XpsDigitalSignature item = new XpsDigitalSignature(signature, this);
				if (item.SignedDocumentSequence != null)
				{
					this._reachSignatures.Add(item);
				}
			}
		}
	}

	private void EnsureThumbnail()
	{
		if (this._thumbnail == null)
		{
			this._thumbnail = base.CurrentXpsManager.EnsureThumbnail(this, null);
		}
	}

	public FixedDocumentSequence GetFixedDocumentSequence()
	{
		this.CheckDisposed();
		if (base.CurrentXpsManager == null)
		{
			throw new InvalidOperationException(SR.Get("ReachPackaging_DocumentWasClosed"));
		}
		if (!this.IsReader)
		{
			throw new XpsPackagingException(SR.Get("ReachPackaging_NotOpenForReading"));
		}
		if (null == base.Uri)
		{
			throw new XpsPackagingException(SR.Get("ReachPackaging_PackageUriNull"));
		}
		if (base.CurrentXpsManager.StartingPart == null)
		{
			return null;
		}
		if (!base.CurrentXpsManager.StartingPart.ValidatedContentType.AreTypeAndSubTypeEqual(XpsS0Markup.DocumentSequenceContentType))
		{
			throw new XpsPackagingException(SR.Get("ReachPackaging_InvalidStartingPart"));
		}
		ParserContext parserContext = new ParserContext
		{
			BaseUri = PackUriHelper.Create(base.Uri, base.CurrentXpsManager.StartingPart.Uri)
		};
		object obj2 = XamlReader.Load(base.CurrentXpsManager.StartingPart.GetStream(), parserContext);
		if (!(obj2 is FixedDocumentSequence))
		{
			throw new XpsPackagingException(SR.Get("ReachPackaging_NotAFixedDocumentSequence"));
		}
		return (obj2 as FixedDocumentSequence);
	}

	private void Initialize()
	{
		this._isInDocumentStage = false;
		this._opcPackage = null;
	}

	public void RemoveSignature(XpsDigitalSignature signature)
	{
		this.CheckDisposed();
		if (signature == null)
		{
			throw new ArgumentNullException("signature");
		}
		if (signature.PackageSignature == null)
		{
			throw new NullReferenceException("signature.PackageSignature");
		}
		if (signature.PackageSignature.SignaturePart == null)
		{
			throw new NullReferenceException("signature.PackageSignature.SignaturePart");
		}
		if (base.CurrentXpsManager == null)
		{
			throw new InvalidOperationException(SR.Get("ReachPackaging_DocumentWasClosed"));
		}
		new PackageDigitalSignatureManager(base.CurrentXpsManager.MetroPackage).RemoveSignature(signature.PackageSignature.SignaturePart.Uri);
		this._reachSignatures = null;
		this._reachSignatureList = null;
		this.EnsureSignatures();
	}

	[SecurityCritical]
	internal static void SaveWithUI(IntPtr parent, Uri source, Uri target)
	{
		AttachmentService.SaveWithUI(parent, source, target);
	}

	public XpsDigitalSignature SignDigitally(X509Certificate certificate, bool embedCertificate, XpsDigSigPartAlteringRestrictions restrictions)
	{
		this.CheckDisposed();
		return this.SignDigitally(certificate, embedCertificate, restrictions, (string)null, true);
	}

	public XpsDigitalSignature SignDigitally(X509Certificate certificate, bool embedCertificate, XpsDigSigPartAlteringRestrictions restrictions, Guid id)
	{
		this.CheckDisposed();
		return this.SignDigitally(certificate, embedCertificate, restrictions, XmlConvert.EncodeName(id.ToString()), true);
	}

	public XpsDigitalSignature SignDigitally(X509Certificate certificate, bool embedCertificate, XpsDigSigPartAlteringRestrictions restrictions, Guid id, bool testIsSignable)
	{
		this.CheckDisposed();
		return this.SignDigitally(certificate, embedCertificate, restrictions, XmlConvert.EncodeName(id.ToString()), testIsSignable);
	}

	private XpsDigitalSignature SignDigitally(X509Certificate certificate, bool embedCertificate, XpsDigSigPartAlteringRestrictions restrictions, string signatureId, bool testIsSignable)
	{
		if (certificate == null)
		{
			throw new ArgumentNullException("certificate");
		}
		if (base.CurrentXpsManager == null)
		{
			throw new InvalidOperationException(SR.Get("ReachPackaging_DocumentWasClosed"));
		}
		if (testIsSignable && !this.IsSignable)
		{
			throw new InvalidOperationException(SR.Get("ReachPackaging_SigningDoesNotMeetPolicy"));
		}
		this.EnsureSignatures();
		List<PackageRelationshipSelector> selectorList = new List<PackageRelationshipSelector>();
		Dictionary<Uri, Uri> dependentList = new Dictionary<Uri, Uri>();
		this.CollectSelfAndDependents(dependentList, selectorList, restrictions);
		PackageDigitalSignature packSignature = base.CurrentXpsManager.Sign(dependentList.Keys, certificate, embedCertificate, selectorList, signatureId);
		return this.AddSignature(packSignature);
	}

	private bool StreamContainsVersionExtensiblity(Stream stream)
	{
		XmlReader reader = new XmlTextReader(stream);
		bool flag = false;
		while (reader.Read())
		{
		}
		if (reader.NameTable.Get(XpsS0Markup.VersionExtensiblityNamespace) != null)
		{
			flag = true;
		}
		return flag;
	}

	void IDisposable.Dispose()
	{
		this.Close();
	}

	void INode.CommitInternal()
	{
		this.Close();
	}

	void INode.Flush()
	{
		if (base.CurrentXpsManager.Streaming)
		{
			base.CurrentXpsManager.MetroPackage.FlushPackageRelationships();
		}
	}

	PackagePart INode.GetPart()
	{
		return null;
	}

	// Properties
	public PackageProperties CoreDocumentProperties
	{
		get
		{
			this.CheckDisposed();
			return base.CurrentXpsManager.MetroPackage.PackageProperties;
		}
	}

	public IXpsFixedDocumentSequenceReader FixedDocumentSequenceReader
	{
		get
		{
			this.CheckDisposed();
			XpsFixedDocumentSequenceReaderWriter writer = null;
			PackagePart startingPart = base.CurrentXpsManager.StartingPart;
			if (this.IsReader && (startingPart != null))
			{
				writer = new XpsFixedDocumentSequenceReaderWriter(base.CurrentXpsManager, null, startingPart);
			}
			return writer;
		}
	}

	public bool IsReader
	{
		get
		{
			this.CheckDisposed();
			return base.CurrentXpsManager.IsReader;
		}
	}

	public bool IsSignable
	{
		get
		{
			this.CheckDisposed();
			List<PackagePart> xmlPartList = new List<PackagePart>();
			(this.FixedDocumentSequenceReader as XpsFixedDocumentSequenceReaderWriter).CollectXmlPartsAndDepenedents(xmlPartList);
			foreach (PackagePart part in xmlPartList)
			{
				Stream stream = part.GetStream(FileMode.Open, FileAccess.Read);
				if (stream.Length != 0L)
				{
					try
					{
						if (!this.StreamContainsVersionExtensiblity(stream))
						{
							continue;
						}
						return false;
					}
					catch (XmlException)
					{
						return false;
					}
				}
			}
			return true;
		}
	}

	public bool IsWriter
	{
		get
		{
			this.CheckDisposed();
			return base.CurrentXpsManager.IsWriter;
		}
	}

	internal Package OpcPackage
	{
		set
		{
			this._opcPackage = value;
		}
	}

	public ReadOnlyCollection<XpsDigitalSignature> Signatures
	{
		get
		{
			this.CheckDisposed();
			this.EnsureSignatures();
			if (this._reachSignatureList == null)
			{
				this._reachSignatureList = new ReadOnlyCollection<XpsDigitalSignature>(this._reachSignatures);
			}
			return this._reachSignatureList;
		}
	}

	public XpsThumbnail Thumbnail
	{
		get
		{
			this.CheckDisposed();
			this.EnsureThumbnail();
			return this._thumbnail;
		}
		set
		{
			this.CheckDisposed();
			this._thumbnail = value;
		}
	}
}
#endif
