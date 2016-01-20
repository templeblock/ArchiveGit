using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using ClassLibrary;

namespace TrumpBubbles
{
	public partial class MainPage : UserControl
	{
		private IDictionary<string, string> _InitParams;

		public MainPage() : this(null)
		{
		}

		public MainPage(IDictionary<string, string> initParams)
		{
			InitializeComponent();
			base.Loaded += OnLoaded;

			_InitParams = initParams;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			string viewPageKey = "ViewPage";
			if (_InitParams != null && _InitParams.ContainsKey(viewPageKey))
			{
				string viewPage = _InitParams[viewPageKey];
				if (!viewPage.StartsWith("/"))
				    viewPage = "/" + viewPage;
				x_NavigationFrame.Source = new Uri(viewPage, UriKind.Relative);
			}
		}

		// After the Frame navigates, ensure the DEBUG HyperlinkButton representing the current page is selected
		private void OnFrameNavigated(object sender, NavigationEventArgs e)
		{
#if DEBUG
			x_LinksPanel.Visibility = Visibility.Visible;
#endif
			foreach (UIElement child in x_LinksPanel.Children)
			{
				HyperlinkButton hb = child as HyperlinkButton;
				if (hb != null && hb.NavigateUri != null)
				{
					if (hb.NavigateUri.ToString().Equals(e.Uri.ToString()))
						VisualStateManager.GoToState(hb, "ActiveLink", true);
					else
						VisualStateManager.GoToState(hb, "InactiveLink", true);
				}
			}
		}

		private void OnFrameNavigationFailed(object sender, NavigationFailedEventArgs e)
		{
			string text = (e.Uri != null ? e.Uri.ToString() : null);
			if (!text.IsEmpty())
				Helper.Alert(string.Format("Cannot locate internal page '{0}'", text));
			e.Handled = true;
		}
	}
}
