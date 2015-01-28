using System;
using System.Windows;
using System.Windows.Browser;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Pivot;
using System.Windows.Resources;
using ClassLibrary;

namespace Pivot
{
	public partial class ViewerPage : Page
	{
		private int _defaultCollectionToLoad = -1;
		private string[] _collectionUrls;

		public ViewerPage()
		{
			InitializeComponent();
			base.Loaded += OnLoaded;
			x_PivotViewer.CollectionLoadingFailed += OnPivotViewerCollectionLoadingFailed;
			x_PivotViewer.Loaded += OnPivotViewerLoaded;
			x_PivotViewer.CollectionLoadingCompleted += OnPivotViewerCollectionLoadingCompleted;
			x_PivotViewer.ItemDoubleClicked += OnPivotViewerItemDoubleClicked;
			x_PivotViewer.LinkClicked += OnPivotViewerLinkClicked;
			x_PivotViewer.ItemActionExecuted += OnPivotViewerItemActionExecuted;

			//x_msi.Source = new DynamicTileSource(UriHelper.UriImages("Monroe.jpg"), 1024, 768, 128, 128);
			try
			{
				string value;
				if (App.MainPage.InitParams != null && App.MainPage.InitParams.TryGetValue("cxml", out value))
					_defaultCollectionToLoad = value.ToInt();
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			LoadCollectionUrls();
			LoadCollectionComboBox();
			LoadDefaultCollection();
		}

		private void OnPivotViewerLoaded(object sender, RoutedEventArgs e)
		{
			x_PivotViewer.CustomActions.Add(
				new CustomAction("Add", UriHelper.UriImages("Add.png"), "Add", "Add"));
			x_PivotViewer.CustomActions.Add(
				new CustomAction("Details", UriHelper.UriImages("Info.png"), "Details", "Details"));
		}

		private void OnPivotViewerCollectionLoadingCompleted(object sender, EventArgs e)
		{
		}

		private void OnPivotViewerCollectionLoadingFailed(object sender, CollectionErrorEventArgs e)
		{
			string caption = e.ErrorCode.ToString();
			string message = string.Format("{0} while parsing {0}", e.Exception.Details(), x_PivotViewer.CollectionUri);
			MessageBox.Show(message, caption, MessageBoxButton.OK);
			//ErrorWindow errorWin = new ErrorWindow(caption, details);
			//errorWin.Show();
		}

		private void OnPivotViewerItemDoubleClicked(object sender, ItemEventArgs e)
		{
			PivotItem item = x_PivotViewer.GetItem(e.ItemId);
			string link = item.Href;
			if (string.IsNullOrWhiteSpace(link))
			{
				string caption = "No Associated Web Page";
				string message = "The item that was double-clicked has no value for the 'Href' field";
				MessageBox.Show(message, caption, MessageBoxButton.OK);
				//ErrorWindow errorWin = new ErrorWindow(caption, details);
				//errorWin.Show();
				return;
			}

			x_PivotViewer.CurrentItemId = e.ItemId;
			OpenLink(new Uri(link));
		}

		private void OnPivotViewerLinkClicked(object sender, LinkEventArgs e)
		{
			OpenLink(e.Link);
		}

		private void LoadCollectionUrls()
		{
			try
			{
				//string info = HtmlPage.Window.Invoke("CollectionInfoHtml") as string;
				var scriptObject = HtmlPage.Window.Invoke("GetCollectionUrls") as ScriptObject;
				if (scriptObject == null)
					return;
				_collectionUrls = scriptObject.ConvertTo<string[]>();
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		private void LoadCollectionComboBox()
		{
			try
			{
				x_CollectionComboBox.Items.Clear();
				if (_collectionUrls == null)
					return;

				foreach (string url in _collectionUrls)
					x_CollectionComboBox.Items.Add(new ComboBoxItem() { Content = url });
			}
			catch (Exception ex)
			{
				ex.GetType();
			}
		}

		private void LoadDefaultCollection()
		{
			if (_collectionUrls != null && _defaultCollectionToLoad >= 0 && _defaultCollectionToLoad < _collectionUrls.Length)
			{
				string collectionName = _collectionUrls[_defaultCollectionToLoad];
				LoadCollection(collectionName);
			}
		}

		private void LoadCollection(string collectionName)
		{
			if (string.IsNullOrWhiteSpace(collectionName))
				return;

			// Note: this assumes that page and the CXML file are in the same folder
			string collectionUri = UriHelper.UriAppRelative(collectionName).ToString();

			string baseCollectionUri;
			string viewerState;
			SplitCollectionUri(collectionUri, out baseCollectionUri, out viewerState);
			x_PivotViewer.LoadCollection(baseCollectionUri, viewerState);

			// Enable the travel log (comment this line to disable the travel log)
			//TravelLog.Create(Pivot, new Uri(collectionUri));
		}

		// Break a collection Uri into the base Uri and the viewerState fragment
		private void SplitCollectionUri(string collectionUri, out string baseCollectionUri, out string viewerState)
		{
			string[] linkParts = collectionUri.Split(new char[] { '#' });
			baseCollectionUri = linkParts[0];
			viewerState = (linkParts.Length > 1) ? linkParts[1] : string.Empty;
		}

		private void OpenLink(Uri targetUri)
		{
			string target = targetUri.LocalPath;
			if (target.EndsWith(".cxml", StringComparison.InvariantCultureIgnoreCase))
			{
				// If the link is to a CXML file, show it in this PivotViewer
				LoadCollection(targetUri.ToString());
				return;
			}

			int startIndex = target.LastIndexOf('/') + 1;
			string data = target.Substring(startIndex);

			string link = null;
			if (target.Contains("BR/"))
				link = string.Format("http://www.baseball-reference.com/players/{0}/{1}.shtml", data[0], data);
			else
			if (target.Contains("TBP/"))
				link = string.Format("http://www.thebaseballpage.com/players/{0}.php", data);
			else
			if (target.Contains("BA/"))
				link = string.Format("http://www.baseball-almanac.com/players/player.php?p={0}", data);
			else
			if (target.Contains("RW/"))
				link = string.Format("http://www.rotowire.com/baseball/c_stats.htm?id={0}", data);
			else
			if (target.Contains("BP/"))
				link = string.Format("http://www.baseballprospectus.com/dt/{0}.shtml", data);
			else
			if (target.Contains("SM/"))
				link = string.Format("http://www.sportsmogul.com/Encyclopedia/Players/{0}/{1}.html", data[0], data);
			else
			if (target.Contains("AR/"))
				link = string.Format("http://www.alexreisner.com/baseball/stats/player?p={0}", data);
			else
			if (target.Contains("GU/"))
				link = string.Format("http://www.guitar9.com/bc/displaybaseballcards.pl?action=List+All&display=1x1f&position={0}", data);
			else
				link = targetUri.ToString();

			HtmlPage.Window.Navigate(new Uri(link), "_blank");
		}

		private void OnPivotViewerItemActionExecuted(object sender, ItemActionEventArgs e)
		{
			PivotItem item = x_PivotViewer.GetItem(e.ItemId);
			if (e.CustomActionId == "Add")
				MessageBox.Show(string.Format("Add {0}", item.Name));
			if (e.CustomActionId == "Details")
				MessageBox.Show(string.Format("Details of {0}", item.Name));
		}

		private void OnToggleTilesClick(object sender, RoutedEventArgs e)
		{
			Button button = sender as Button;
			if (x_PivotViewer.TileVisibility == Visibility.Visible)
			{
				button.Content = "Show Tiles";
				x_PivotViewer.TileVisibility = Visibility.Collapsed;
			}
			else
			{
				button.Content = "Hide Tiles";
				x_PivotViewer.TileVisibility = Visibility.Visible;
			}
		}

		private void OnToggleFiltersClick(object sender, RoutedEventArgs e)
		{
			Button button = sender as Button;
			if (x_PivotViewer.FilterVisibility == Visibility.Visible)
			{
				button.Content = "Show Filters";
				x_PivotViewer.FilterVisibility = Visibility.Collapsed;
			}
			else
			{
				button.Content = "Hide Filters";
				x_PivotViewer.FilterVisibility = Visibility.Visible;
			}
		}

		private void OnToggleInfoClick(object sender, RoutedEventArgs e)
		{
			Button button = sender as Button;
			if (x_PivotViewer.InfoVisibility == Visibility.Visible)
			{
				button.Content = "Show Info";
				x_PivotViewer.InfoVisibility = Visibility.Collapsed;
			}
			else
			{
				button.Content = "Hide Info";
				x_PivotViewer.InfoVisibility = Visibility.Visible;
			}
		}

		private string _bgrd = "";
		private void OnToggleBackgroundClick(object sender, RoutedEventArgs e)
		{
			Brush background;
			switch (_bgrd)
			{
				case "":
				case "ImageBrush":
					background = new SolidColorBrush(Colors.Black);
					_bgrd = "Colors.Black";
					break;

				case "Colors.Black":
					background = new SolidColorBrush(Colors.Transparent);
					_bgrd = "Colors.Transparent";
					break;

				case "Colors.Transparent":
					GradientStopCollection gsc1 = new GradientStopCollection {
						new GradientStop { Color = Colors.Orange, Offset = 0.0 },
						new GradientStop { Color = Colors.Yellow, Offset = 0.5 },
						new GradientStop { Color = Colors.Green, Offset = 1.0 }
					};
					background = new RadialGradientBrush(gsc1)
					{
						Center = new Point(1.0, 1.0),
						GradientOrigin = new Point(1.0, 1.0),
						RadiusX = 1.0,
						RadiusY = 1.0
					};
					_bgrd = "RadialGradientBrush";
					break;

				case "RadialGradientBrush":
					GradientStopCollection gsc2 = new GradientStopCollection {
						new GradientStop { Color = Colors.Purple, Offset = 0.0 },
						new GradientStop { Color = Colors.Yellow, Offset = 0.5 },
						new GradientStop { Color = Colors.Cyan, Offset = 1.0 }
					};
					background = new LinearGradientBrush(gsc2, 45)
					{
						StartPoint = new Point(0.5, 0.5)
					};
					_bgrd = "LinearGradient";
					break;

				case "LinearGradient":
					// Main tile view background - ImageBrush
					StreamResourceInfo sr = Application.GetResourceStream(new Uri("PimpThePivotViewer;component/Resources/Island.jpg", UriKind.Relative));
					BitmapImage bmp = new BitmapImage();
					bmp.SetSource(sr.Stream);
					background = new ImageBrush() { ImageSource = bmp };
					sr.Stream.Close();
					_bgrd = "ImageBrush";
					break;

				default:
					background = new SolidColorBrush(Colors.Transparent);
					_bgrd = "Colors.Transparent";
					break;
			}

			x_PivotViewer.GridBackground = background;
		}

		private void OnCollectionLoadClick(object sender, RoutedEventArgs e)
		{
			LoadCollection(x_CollectionTextBox.Text);
		}

		private void OnCollectionSelect(object sender, SelectionChangedEventArgs e)
		{
			int indexSamples = x_CollectionComboBox.SelectedIndex;
			ComboBoxItem item = x_CollectionComboBox.Items[indexSamples] as ComboBoxItem;
			x_CollectionTextBox.Text = item.Content.ToString();
			//LoadCollection(x_CollectionTextBox.Text);
		}
	}
}
