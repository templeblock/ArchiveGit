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

namespace PicaPages
{
	public class User
	{
		int m_CustomerID;
		bool m_sEmailVerified;
		string m_sEmailAddress;
		string m_sPassword;

		public int CustomerID
		{
			get { return m_CustomerID; }
			set { m_CustomerID = value; }
		}

		public bool EmailVerified
		{
			get { return m_sEmailVerified; }
			set { m_sEmailVerified = value; }
		}

		public string EmailAddress
		{
			get { return m_sEmailAddress; }
			set { m_sEmailAddress = value; }
		}

		public string Password
		{
			get { return m_sPassword; }
			set { m_sPassword = value; }
		}
	}

	public partial class Page : UserControl
	{
		private DocumentHolder m_DocumentHolder;
		private DragDrop m_DragDrop;

		public DocumentHolder DocumentHolderProp { get { return m_DocumentHolder; } }
		public DragDrop DragDropProp { get { return m_DragDrop; } }
		
		/////////////////////////////////////////////////////////////////////////////
		public Page()
		{
			base.Loaded += Page_Loaded;

			// Required to create class member variables from named XAML elements
			InitializeComponent();
		}

		/////////////////////////////////////////////////////////////////////////////
		private void Page_Loaded(object sender, EventArgs args)
		{
//j			WebResources.DownloadUsers();
//j			WebResources.DownloadAccounts();
			WebResources.DownloadLayouts();
			WebResources.DownloadCovers();
			WebResources.DownloadBackgrounds();
			WebResources.DownloadCorners();
			WebResources.DownloadPhotos();
			MouseMove += WebResourcesCheck;

			m_DocumentHolder = new DocumentHolder();
//j			x_AccountsPanel.InitDocumentListener(m_DocumentHolder);
//j			x_ToolsPanel.InitDocumentListener(m_DocumentHolder);
			x_LayoutsPanel.InitDocumentListener(m_DocumentHolder);
			x_CoversPanel.InitDocumentListener(m_DocumentHolder);
			x_BackgroundsPanel.InitDocumentListener(m_DocumentHolder);
			x_CornersPanel.InitDocumentListener(m_DocumentHolder);
			x_PhotosPanel.InitDocumentListener(m_DocumentHolder);
//j			x_AdsPanel.InitDocumentListener(m_DocumentHolder);
//j			x_MainPanel.InitDocumentListener(m_DocumentHolder);
//j			x_PropertiesPanel.InitDocumentListener(m_DocumentHolder);

			// Initialize drag and drop
			m_DragDrop = new DragDrop();
			m_DragDrop.Init();
//j			m_DragDrop.AddTarget(x_MainPanel);
//j			m_DragDrop.DropTargetListeners += x_MainPanel.DropTarget;

			// Handle resizing
			Application.Current.Host.Content.Resized += BrowserHost_Resized;
			Resize();
			
			// Initialize the static scroll viewer mouse wheel support class
			ScrollViewerMouseWheelSupport.Initialize(App.PageProp, MouseWheelAssociationMode.OnHover);
			SliderMouseWheelSupport.Initialize(App.PageProp, MouseWheelAssociationMode.OnHover);

			// Register a single scroll viewer
			x_LayoutsScrollViewer.AddMouseWheelSupport();
		}

		/////////////////////////////////////////////////////////////////////////////
		private void WebResourcesCheck(object sender, MouseEventArgs args)
		{
			LayoutResourcesCheck();
			CoverResourcesCheck();
			BackgroundResourcesCheck();
			CornerResourcesCheck();
			PhotoResourcesCheck();
//j			AccountsResourcesCheck();
		}

		/////////////////////////////////////////////////////////////////////////////
		private void LayoutResourcesCheck()
		{
			if (WebResources.Layouts == null || !x_LayoutsPanel.IsEmpty)
				return;

			// Load the spending Layouts into the UI panel
			foreach (Layout layout in WebResources.Layouts)
				x_LayoutsPanel.AddLayout(layout, OnLayoutClick);
		}

		/////////////////////////////////////////////////////////////////////////////
		private void CoverResourcesCheck()
		{
			if (WebResources.Covers == null || !x_CoversPanel.IsEmpty)
				return;

			// Load the spending Covers into the UI panel
			foreach (Cover cover in WebResources.Covers)
				x_CoversPanel.AddCover(cover, OnLayoutClick);
		}

		/////////////////////////////////////////////////////////////////////////////
		private void BackgroundResourcesCheck()
		{
			if (WebResources.Backgrounds == null || !x_BackgroundsPanel.IsEmpty)
				return;

			// Load the spending Backgrounds into the UI panel
			foreach (Background background in WebResources.Backgrounds)
				x_BackgroundsPanel.AddBackground(background, OnLayoutClick);
		}

		/////////////////////////////////////////////////////////////////////////////
		private void CornerResourcesCheck()
		{
			if (WebResources.Corners == null || !x_CornersPanel.IsEmpty)
				return;

			// Load the spending Corners into the UI panel
			foreach (Corner corner in WebResources.Corners)
				x_CornersPanel.AddCorner(corner, OnLayoutClick);
		}

		/////////////////////////////////////////////////////////////////////////////
		private void PhotoResourcesCheck()
		{
			if (WebResources.Photos == null || !x_PhotosPanel.IsEmpty)
				return;

			// Load the spending Photos into the UI panel
			foreach (Photo photo in WebResources.Photos)
				x_PhotosPanel.AddPhoto(photo, OnLayoutClick);
		}

		/////////////////////////////////////////////////////////////////////////////
		private void AccountsResourcesCheck()
		{
			if (WebResources.Accounts == null || m_DocumentHolder.DocumentProp != null)
				return;

			// Create a empty document
			m_DocumentHolder.Open(null);
		}

		/////////////////////////////////////////////////////////////////////////////
		private FileInfo OpenFile(string filter)
		{
			OpenFileDialog ofd = new OpenFileDialog();
			ofd.Multiselect = false;
			if (!String.IsNullOrEmpty(filter))
				ofd.Filter = filter;

			if (ofd.ShowDialog() == false)
				return null;

			return ofd.File;
		}

		/////////////////////////////////////////////////////////////////////////////
		private Layout FindLayout(string name)
		{
			if (WebResources.Layouts == null)
				return null;

			foreach (Layout layout in WebResources.Layouts)
			{
				if (layout.Name == name)
					return layout;
			}

			return null;
		}

		/////////////////////////////////////////////////////////////////////////////
		private void OnLayoutClick(object sender, EventArgs args)
		{
			if (!(sender is FrameworkElement))
				return;

			FrameworkElement element = sender as FrameworkElement;
			string name = element.Tag as string;
			Layout layout = FindLayout(name);
		}

		/////////////////////////////////////////////////////////////////////////////
		private void WebService_Click(object sender, EventArgs args)
		{
			if (WebResources.Users == null)
				return;

			string output = "";
			foreach (User user in WebResources.Users)
			    output += user.CustomerID + " " + user.EmailAddress + " " + user.EmailVerified + " " + user.Password + "\r\n";
			MessageBoxEx.Show("Alert", output, null);
		}

		/////////////////////////////////////////////////////////////////////////////
		private void BrowserHost_Resized(object sender, EventArgs args)
		{
			Resize();
		}

		/////////////////////////////////////////////////////////////////////////////
		private void Resize()
		{
#if true//SCALABLE_SILVERLIGHT_PLUGIN
			double newWidth = Application.Current.Host.Content.ActualWidth;
			double newHeight = Application.Current.Host.Content.ActualHeight;
			if ((this.Width != newWidth || this.Height != newHeight) && (newHeight > 100 && newWidth > 100))
			{
#if false//SCALABLE_CONTROLS
				double scalex = newWidth / 1024; //j m_UserControl.Width original design value
				double scaley = newHeight / 768; //j m_UserControl.Height original design value
				double scale = Math.Min(scalex, scaley);
				ScaleTransform scaleTransform = new ScaleTransform();
				scaleTransform.ScaleX = scale;
				scaleTransform.ScaleY = scale;
				x_LayoutRoot.RenderTransform = scaleTransform;
#endif
				base.Width = newWidth;
				base.Height = newHeight;
			}
#endif
		}

		/////////////////////////////////////////////////////////////////////////////
		private void Open_Click(object sender, RoutedEventArgs e)
		{
			string filter = "Txt Files|*.txt| Word Files|*.doc| JPEG Files (*.jpg;*.jpeg)|*.jpg;*.jpeg| All Files (*.*)|*.*";
			FileInfo file = OpenFile(filter);
			if (file == null)
				return;
				
			string filename = file.Name;
			filename = "Document.xml";
			m_DocumentHolder.Open(filename);
			MessageBoxEx.Show("Alert", String.Format("Open of '{0}' complete", filename), null);
		}

		private void Close_Click(object sender, RoutedEventArgs e)
		{
			m_DocumentHolder.Close();
			MessageBoxEx.Show("Alert", String.Format("Close complete"), null);
		}

		private void Save_Click(object sender, RoutedEventArgs e)
		{
			string filename = "Document.xml";
			m_DocumentHolder.Save(filename);
			MessageBoxEx.Show("Alert", String.Format("Save to '{0}' complete", filename), null);
		}
	}

	public class ToolsPanel : ViewPanel, IDropTarget
	{
		public ToolsPanel()
		{
		}

		public override void InitDocumentListener(DocumentHolder documentHolder)
		{
			base.InitDocumentListener(documentHolder);
		}

		public override void DocumentClose(Document document, EventArgs e)
		{
		}

		public override void DocumentChange(Document document, EventArgs e)
		{
		}

		/////////////////////////////////////////////////////////////////////////////
		public void DropTarget(DropTargetEventType type, DragDrop dragDrop)
		{
			if (dragDrop.Target != this)
				return;
			if (!(dragDrop.Payload is Layout))
				return;
			Layout layout = dragDrop.Payload as Layout;
			Point mousePoint = dragDrop.GetPosition(dragDrop.Target);
			if (type == DropTargetEventType.Enter)
				dragDrop.DefaultEnterEventHandler(dragDrop.Target);
			else
			if (type == DropTargetEventType.Over)
				dragDrop.DefaultOverEventHandler(dragDrop.Target);
			else
			if (type == DropTargetEventType.Leave)
				dragDrop.DefaultLeaveEventHandler(dragDrop.Target);
			else
			if (type == DropTargetEventType.Drop)
				dragDrop.DefaultDropEventHandler(dragDrop.Target);
		}
	}

	public class LayoutsPanel : ViewPanel
	{
		private Panel m_LayoutsIconPanel;
		private bool m_bEmpty;
		public bool IsEmpty { get { return m_bEmpty; } }

		public LayoutsPanel()
		{
			m_bEmpty = true;
		}

		public override void InitDocumentListener(DocumentHolder documentHolder)
		{
			base.InitDocumentListener(documentHolder);
			m_LayoutsIconPanel = App.PageProp.FindName("x_LayoutsIconPanel") as Panel;
		}

		public override void DocumentClose(Document document, EventArgs e)
		{
		}

		public override void DocumentChange(Document document, EventArgs e)
		{
		}

		public void AddLayout(Layout layout, RoutedEventHandler clickHandler)
		{
			m_bEmpty = false;

			StackPanel panel = new StackPanel();
			panel.Orientation = Orientation.Horizontal;
			panel.VerticalAlignment = VerticalAlignment.Stretch;
			panel.HorizontalAlignment = HorizontalAlignment.Stretch;

			// Find the images relative to the /ClientBin folder which contains the XAP
			BitmapImage bm = new BitmapImage();
			bm.UriSource = Helper.HttpImagesPath(layout.Icon);

			Image image = new Image(); 
			image.Source = bm;
			image.Width = 57; //j was 63;
			image.Height = 39;
			image.VerticalAlignment = VerticalAlignment.Center;
			image.HorizontalAlignment = HorizontalAlignment.Left;
			panel.Children.Add(image);

			HyperlinkButton iconButton = new HyperlinkButton();
			iconButton.Content = panel;
			iconButton.Tag = layout.Name;
			iconButton.SetToolTip(layout.Name);
			iconButton.Width = 59; //j was 65;
			iconButton.Height = 41;
			m_LayoutsIconPanel.Children.Add(iconButton);

			iconButton.Click += clickHandler;

			DragDrop dragDrop = App.PageProp.DragDropProp;
			dragDrop.AddSource(image, layout);
		}
	}

	public class CoversPanel : ViewPanel
	{
		private Panel m_CoversIconPanel;
		private bool m_bEmpty;
		public bool IsEmpty { get { return m_bEmpty; } }

		public CoversPanel()
		{
			m_bEmpty = true;
		}

		public override void InitDocumentListener(DocumentHolder documentHolder)
		{
			base.InitDocumentListener(documentHolder);
			m_CoversIconPanel = App.PageProp.FindName("x_CoversIconPanel") as Panel;
		}

		public override void DocumentClose(Document document, EventArgs e)
		{
		}

		public override void DocumentChange(Document document, EventArgs e)
		{
		}

		public void AddCover(Cover cover, RoutedEventHandler clickHandler)
		{
			m_bEmpty = false;

			StackPanel panel = new StackPanel();
			panel.Orientation = Orientation.Horizontal;
			panel.VerticalAlignment = VerticalAlignment.Stretch;
			panel.HorizontalAlignment = HorizontalAlignment.Stretch;

			// Find the images relative to the /ClientBin folder which contains the XAP
			BitmapImage bm = new BitmapImage();
			bm.UriSource = Helper.HttpImagesPath(cover.Icon);

			Image image = new Image(); 
			image.Source = bm;
			image.Width = 57;
			image.Height = 50;
			image.VerticalAlignment = VerticalAlignment.Center;
			image.HorizontalAlignment = HorizontalAlignment.Left;
			panel.Children.Add(image);

			HyperlinkButton iconButton = new HyperlinkButton();
			iconButton.Content = panel;
			iconButton.Tag = cover.Name;
			iconButton.SetToolTip(cover.Name);
			iconButton.Width = 59;
			iconButton.Height = 52;
			m_CoversIconPanel.Children.Add(iconButton);

			iconButton.Click += clickHandler;

			DragDrop dragDrop = App.PageProp.DragDropProp;
			dragDrop.AddSource(image, cover);
		}
	}

	public class BackgroundsPanel : ViewPanel
	{
		private Panel m_BackgroundsIconPanel;
		private bool m_bEmpty;
		public bool IsEmpty { get { return m_bEmpty; } }

		public BackgroundsPanel()
		{
			m_bEmpty = true;
		}

		public override void InitDocumentListener(DocumentHolder documentHolder)
		{
			base.InitDocumentListener(documentHolder);
			m_BackgroundsIconPanel = App.PageProp.FindName("x_BackgroundsIconPanel") as Panel;
		}

		public override void DocumentClose(Document document, EventArgs e)
		{
		}

		public override void DocumentChange(Document document, EventArgs e)
		{
		}

		public void AddBackground(Background background, RoutedEventHandler clickHandler)
		{
			m_bEmpty = false;

			StackPanel panel = new StackPanel();
			panel.Orientation = Orientation.Horizontal;
			panel.VerticalAlignment = VerticalAlignment.Stretch;
			panel.HorizontalAlignment = HorizontalAlignment.Stretch;

			// Find the images relative to the /ClientBin folder which contains the XAP
			BitmapImage bm = new BitmapImage();
			bm.UriSource = Helper.HttpImagesPath(background.Icon);

			Image image = new Image(); 
			image.Source = bm;
			image.Width = 57;
			image.Height = 50;
			image.VerticalAlignment = VerticalAlignment.Center;
			image.HorizontalAlignment = HorizontalAlignment.Left;
			panel.Children.Add(image);

			HyperlinkButton iconButton = new HyperlinkButton();
			iconButton.Content = panel;
			iconButton.Tag = background.Name;
			iconButton.SetToolTip(background.Name);
			iconButton.Width = 59;
			iconButton.Height = 52;
			m_BackgroundsIconPanel.Children.Add(iconButton);

			iconButton.Click += clickHandler;

			DragDrop dragDrop = App.PageProp.DragDropProp;
			dragDrop.AddSource(image, background);
		}
	}

	public class CornersPanel : ViewPanel
	{
		private Panel m_CornersIconPanel;
		private bool m_bEmpty;
		public bool IsEmpty { get { return m_bEmpty; } }

		public CornersPanel()
		{
			m_bEmpty = true;
		}

		public override void InitDocumentListener(DocumentHolder documentHolder)
		{
			base.InitDocumentListener(documentHolder);
			m_CornersIconPanel = App.PageProp.FindName("x_CornersIconPanel") as Panel;
		}

		public override void DocumentClose(Document document, EventArgs e)
		{
		}

		public override void DocumentChange(Document document, EventArgs e)
		{
		}

		public void AddCorner(Corner corner, RoutedEventHandler clickHandler)
		{
			m_bEmpty = false;

			StackPanel panel = new StackPanel();
			panel.Orientation = Orientation.Horizontal;
			panel.VerticalAlignment = VerticalAlignment.Stretch;
			panel.HorizontalAlignment = HorizontalAlignment.Stretch;

			// Find the images relative to the /ClientBin folder which contains the XAP
			BitmapImage bm = new BitmapImage();
			bm.UriSource = Helper.HttpImagesPath(corner.Icon);

			Image image = new Image(); 
			image.Source = bm;
			image.Width = 57; //j was 128;
			image.Height = 57; //j was 128;
			image.VerticalAlignment = VerticalAlignment.Center;
			image.HorizontalAlignment = HorizontalAlignment.Left;
			image.Stretch = Stretch.None;
			panel.Children.Add(image);

			HyperlinkButton iconButton = new HyperlinkButton();
			iconButton.Content = panel;
			iconButton.Tag = corner.Name;
			iconButton.SetToolTip(corner.Name);
			iconButton.Width = 59; //j was 130;
			iconButton.Height = 59; //j was 130;
			m_CornersIconPanel.Children.Add(iconButton);

			iconButton.Click += clickHandler;

			DragDrop dragDrop = App.PageProp.DragDropProp;
			dragDrop.AddSource(image, corner);
		}
	}

	public class PhotosPanel : ViewPanel
	{
		private Panel m_PhotosIconPanel;
		private bool m_bEmpty;
		public bool IsEmpty { get { return m_bEmpty; } }

		public PhotosPanel()
		{
			m_bEmpty = true;
		}

		public override void InitDocumentListener(DocumentHolder documentHolder)
		{
			base.InitDocumentListener(documentHolder);
			m_PhotosIconPanel = App.PageProp.FindName("x_PhotosIconPanel") as Panel;
		}

		public override void DocumentClose(Document document, EventArgs e)
		{
		}

		public override void DocumentChange(Document document, EventArgs e)
		{
		}

		public void AddPhoto(Photo photo, RoutedEventHandler clickHandler)
		{
			m_bEmpty = false;

			StackPanel panel = new StackPanel();
			panel.Orientation = Orientation.Horizontal;
			panel.VerticalAlignment = VerticalAlignment.Stretch;
			panel.HorizontalAlignment = HorizontalAlignment.Stretch;

			// Find the images relative to the /ClientBin folder which contains the XAP
			BitmapImage bm = new BitmapImage();
			bm.UriSource = Helper.HttpImagesPath(photo.Icon);

			Image image = new Image(); 
			image.Source = bm;
			image.Width = 100;
			image.Height = 100;
			image.VerticalAlignment = VerticalAlignment.Center;
			image.HorizontalAlignment = HorizontalAlignment.Left;
			panel.Children.Add(image);

			HyperlinkButton iconButton = new HyperlinkButton();
			iconButton.Content = panel;
			iconButton.Tag = photo.Name;
			iconButton.SetToolTip(photo.Name);
//j			iconButton.Width = 100;
//j			iconButton.Height = 100;
			m_PhotosIconPanel.Children.Add(iconButton);

			iconButton.Click += clickHandler;

			DragDrop dragDrop = App.PageProp.DragDropProp;
			dragDrop.AddSource(image, photo);
		}
	}

	public class AdsPanel : ViewPanel
	{
		public AdsPanel()
		{
		}

		public override void InitDocumentListener(DocumentHolder documentHolder)
		{
			base.InitDocumentListener(documentHolder);
		}

		public override void DocumentClose(Document document, EventArgs e)
		{
		}

		public override void DocumentChange(Document document, EventArgs e)
		{
		}
	}

	public class MainPanel : ViewPanel, IDropTarget
	{
		public MainPanel()
		{
		}

		public override void InitDocumentListener(DocumentHolder documentHolder)
		{
			base.InitDocumentListener(documentHolder);
		}

		public override void DocumentClose(Document document, EventArgs e)
		{
		}

		public override void DocumentChange(Document document, EventArgs e)
		{
		}

		/////////////////////////////////////////////////////////////////////////////
		public void DropTarget(DropTargetEventType type, DragDrop dragDrop)
		{
			if (dragDrop.Target != this)
				return;
			if (!(dragDrop.Payload is Layout))
				return;
			Layout layout = dragDrop.Payload as Layout;
			Point mousePoint = dragDrop.GetPosition(dragDrop.Target);
			if (type == DropTargetEventType.Enter)
				dragDrop.DefaultEnterEventHandler(dragDrop.Target);
			else
			if (type == DropTargetEventType.Over)
				dragDrop.DefaultOverEventHandler(dragDrop.Target);
			else
			if (type == DropTargetEventType.Leave)
				dragDrop.DefaultLeaveEventHandler(dragDrop.Target);
			else
			if (type == DropTargetEventType.Drop)
			{
				WebResources.DownloadLayoutPayload(layout.PayloadId, DownloadLayoutPayloadCompleted);
				dragDrop.DefaultDropEventHandler(dragDrop.Target);
			}
		}

		/////////////////////////////////////////////////////////////////////////////
		private void DownloadLayoutPayloadCompleted(object sender, EventArgs args)
		{
			if (sender is string)
			{
				string error = sender as string;
				MessageBoxEx.Show("Error", error, null);
				return;
			}

			if (!(sender is XDocument))
				return;

			XDocument xmlDoc = sender as XDocument; // xmlDoc.Root.Name should be "Root" or null
			if (xmlDoc.Root == null) // An valid empty result
				return;
			XElement elem = xmlDoc.Root.Descendants().First<XElement>();
			int payloadId = elem.Attribute("PayloadId").Value.ToInt();
			string name = elem.Attribute("Name").Value;
			string payload = elem.Attribute("Payload").Value;

			// Do the conditional Value replacement here
			Dictionary<string, string> replacementDictionary = new Dictionary<string, string>();
			replacementDictionary.Add("zipCode", "01810");
			replacementDictionary.Add("comcast", "VERIZON");
			//replacementDictionary.Add("\\\"", "\"");
			//replacementDictionary.Add("amp", "&");
			string pattern = "([a-zA-Z]+)"; // "(&amp;)([a-zA-Z]+)([;])";
			int count = 0;
			payload = payload.ReplacePattern(pattern, 1, replacementDictionary, out count);

			XDocument xmlPayload = XDocument.Parse(payload); // xmlDoc.Root.Name should be "Layout" or null
			if (xmlDoc.Root == null) // An valid empty result
				return;
			var query = from item in xmlPayload.Root.Descendants()
			select new Transaction
			{
				Name = item.Attribute("Name").Value,
				Value = item.Attribute("Value").Value.ToInt(),
				Type = item.Attribute("Type").Value.ToEnum<ETransactionType>(),
			};

			List<Transaction> transactions = query.ToList<Transaction>();
			AddPackageDialog dialog = new AddPackageDialog(name, transactions, AddPackageDialogClosed);
		}

		/////////////////////////////////////////////////////////////////////////////
		private void AddPackageDialogClosed(object sender, EventArgs args)
		{
			AddPackageDialog dialog = sender as AddPackageDialog;
			MessageBoxResult result = (dialog != null ? dialog.Result : MessageBoxResult.Cancel);
			if (result == MessageBoxResult.Cancel)
				return;

			JulianDay lStartDate = dialog.StartDate.DisplayDate.ToJulian();
			JulianDay lEndDate = dialog.EndDate.DisplayDate.ToJulian();
			TransactionOccurs transactionOccurs = new TransactionOccurs(lStartDate, lEndDate, dialog.Transactions);

			Document document = App.PageProp.DocumentHolderProp.DocumentProp;
			document.AddTransactionOccurs(transactionOccurs);
			document.Run(lStartDate, lEndDate);
		}
	}

	public class PropertiesPanel : ViewPanel
	{
		public PropertiesPanel()
		{
		}

		public override void InitDocumentListener(DocumentHolder documentHolder)
		{
			base.InitDocumentListener(documentHolder);
		}

		public override void DocumentClose(Document document, EventArgs e)
		{
		}

		public override void DocumentChange(Document document, EventArgs e)
		{
		}
	}
}
