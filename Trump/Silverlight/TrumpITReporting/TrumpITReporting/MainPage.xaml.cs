using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using ClassLibrary;

namespace TrumpITReporting
{
	public partial class MainPage : UserControl
	{
		internal NavigationHelper NavigationHelper { get { return m_NavigationHelper; } }
		private NavigationHelper m_NavigationHelper = new NavigationHelper();

		private IDictionary<string, string> m_InitParams;

		public MainPage() : this(null)
		{
		}

		public MainPage(IDictionary<string, string> initParams)
		{
			InitializeComponent();
			base.Loaded += OnLoaded;

			m_InitParams = initParams;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			m_NavigationHelper.Init(x_NavigationFrame, x_NavigationSelector);

			// Handle resizing
			Application.Current.Host.Content.Resized += BrowserHost_Resized;
			Resize();
		}

		private void BrowserHost_Resized(object sender, EventArgs e)
		{
			Resize();
		}

		private void Resize()
		{
			double newWidth = Application.Current.Host.Content.ActualWidth;
			double newHeight = Application.Current.Host.Content.ActualHeight;
			if ((base.Width != newWidth || base.Height != newHeight) && (newHeight > 100 && newWidth > 100))
			{
				// Change the size of the UserControl/Page, the LayoutRoot, and the LayoutRoot's main child
				base.Width = newWidth;
				base.Height = newHeight;
				ApplicationEx.LayoutRoot.Width = newWidth;
				ApplicationEx.LayoutRoot.Height = newHeight;
				Panel childPanel = null;
				if (ApplicationEx.LayoutRoot.Children.Count > 0 && (childPanel = ApplicationEx.LayoutRoot.Children[0] as Panel) != null)
				{
					childPanel.Width = newWidth;
					childPanel.Height = newHeight;
				}
			}
		}

		private void OnFullScreenClick(object sender, RoutedEventArgs e)
		{
			Application.Current.Host.Content.FullScreenChanged += OnFullScreenChanged;
			Application.Current.Host.Content.IsFullScreen = Application.Current.Host.Content.IsFullScreen ? false : true;
		}

		private void OnFullScreenChanged(object sender, EventArgs e)
		{
			bool bIsFullScreen = Application.Current.Host.Content.IsFullScreen;
			Resize();
		}

		private void OnNavigationButtonClick(object sender, RoutedEventArgs e)
		{
			m_NavigationHelper.NavigateViaElement(sender as FrameworkElement);
		}

		private void OnNavigationTabChanged(object sender, SelectionChangedEventArgs e)
		{
			TabControl tabControl = sender as TabControl;
			if (tabControl == null)
				return;
			if (tabControl.SelectedContent is TabControl)
				tabControl = tabControl.SelectedContent as TabControl;
			TabItem tabItem = tabControl.SelectedItem as TabItem;
			m_NavigationHelper.NavigateViaElement(tabItem);
		}

		public void OnHyperlinkClick(object sender, RoutedEventArgs e)
		{
			// Absolute links will be launched automatically by the HtmlContent control
			// Relative Uri's will be passed to this HyperlinkClick event in the Tag property
			HyperlinkButton button = sender as HyperlinkButton;
			if (button == null || button.NavigateUri != null)
				return;

			string contentUri = button.Tag as string;
			if (contentUri.IsEmpty())
				return;

			// To maintain compatibility with the way ASP.NET would process the link with URL rewriting...
			string prefix = "/app/";
			int index = contentUri.ToLower().IndexOf(prefix);
			if (index == 0)
				contentUri = contentUri.Substring(prefix.Length - 1); // Remove it

			m_NavigationHelper.Navigate(contentUri);
		}

		private void OnDataAccessClick(object sender, RoutedEventArgs e)
		{
			new DataAccessDialog();
		}

		private void OnCloseWindow(object sender, RoutedEventArgs e)
		{
		}
	}
}
