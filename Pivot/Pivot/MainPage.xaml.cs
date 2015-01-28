using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Interop;
using ClassLibrary;

namespace Pivot
{
	public partial class MainPage : UserControl
	{
		internal IDictionary<string, string> InitParams { get; set; }
		internal NavigationHelper NavigationHelper { get { return _NavigationHelper; } }
		private NavigationHelper _NavigationHelper = new NavigationHelper();

		public MainPage() : this(null)
		{
		}

		public MainPage(IDictionary<string, string> initParams)
		{
			InitializeComponent();
			base.Loaded += OnLoaded;
			InitParams = initParams;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			_NavigationHelper.Init(x_NavigationFrame, x_MainTabControl);

			// This is the key to all XAML Binding; everything in the viual tree inherits this DataContext
			base.DataContext = App.Model;

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
#if SCALABLE_CONTROLS //j
				double scalex = newWidth / 1024;
				double scaley = newHeight / 768;
				double scale = Math.Min(scalex, scaley);
				ScaleTransform scaleTransform = new ScaleTransform();
				scaleTransform.ScaleX = scale;
				scaleTransform.ScaleY = scale;
				ApplicationEx.LayoutRoot.RenderTransform = scaleTransform;
#endif
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
			Application.Current.Host.Content.IsFullScreen = !Application.Current.Host.Content.IsFullScreen;
			bool AllowFullScreenPinning = false;
			Application.Current.Host.Content.FullScreenOptions = (AllowFullScreenPinning ? FullScreenOptions.StaysFullScreenWhenUnfocused : FullScreenOptions.None);
		}

		private void OnFullScreenChanged(object sender, EventArgs e)
		{
			bool bIsFullScreen = Application.Current.Host.Content.IsFullScreen;
			Resize();
		}

		internal void OnSigninClick(object sender, RoutedEventArgs e)
		{
			AuthenticationDialog dialog = new AuthenticationDialog(AuthenticationDialog.DialogType.SignIn);
			dialog.Closed += AuthenticationDialogClosed;
		}

		private void OnRegisterClick(object sender, RoutedEventArgs e)
		{
			AuthenticationDialog dialog = new AuthenticationDialog(AuthenticationDialog.DialogType.Register);
			dialog.Closed += AuthenticationDialogClosed;
		}

		private void OnRecoverClick(object sender, RoutedEventArgs e)
		{
			AuthenticationDialog dialog = new AuthenticationDialog(AuthenticationDialog.DialogType.RecoverSignIn);
			dialog.Closed += AuthenticationDialogClosed;
		}

		private void AuthenticationDialogClosed(object sender, EventArgs e)
		{
			AuthenticationDialog dialog = sender as AuthenticationDialog;
			if (dialog == null)
				return;
			if (dialog.DialogResult == MessageBoxResult.Cancel)
				return;
		}

		private void OnMyAccountClick(object sender, RoutedEventArgs e)
		{
			x_NavigationFrame.Navigate("/Home/My_Account");
		}

		private void OnSignoutClick(object sender, RoutedEventArgs e)
		{
			App.Model.UserServices.Logout(null, null);
		}

		private void OnMainTabControlChanged(object sender, SelectionChangedEventArgs e)
		{
			TabControl mainTabControl = sender as TabControl;
			if (mainTabControl == null)
				return;
			TabControl subTabControl = mainTabControl.SelectedContent as TabControl;
			_NavigationHelper.NavigateTabControl(subTabControl);
		}

		private void OnSubTabControlChanged(object sender, SelectionChangedEventArgs e)
		{
			TabControl subTabControl = sender as TabControl;
			_NavigationHelper.NavigateTabControl(subTabControl);
		}
		
		public void OnHyperlinkClick(object sender, RoutedEventArgs e)
		{
			// Absolute links will be launched automatically by the Hyperlink control
			// Relative Uri's will be passed to this HyperlinkClick event in the CommandParameter property
			Hyperlink button = sender as Hyperlink;
			if (button == null || button.CommandParameter == null)
				return;

			string contentUri = button.CommandParameter as string;
			if (contentUri.IsEmpty())
				return;

			if (contentUri.ContainsIgnoreCase("#Feedback"))
			{
				OnFeedbackClick(null, null);
				return;
			}

			if (contentUri.ContainsIgnoreCase("#SignIn"))
			{
				OnSigninClick(null, null);
				return;
			}

			if (contentUri.ContainsIgnoreCase("#Register"))
			{
				OnRegisterClick(null, null);
				return;
			}

			if (contentUri.ContainsIgnoreCase("#Recover"))
			{
				OnRecoverClick(null, null);
				return;
			}

			// To maintain compatibility with the way ASP.NET would process the link with URL rewriting...
			string prefix = "/app/";
			int index = contentUri.ToLower().IndexOf(prefix);
			if (index == 0)
				contentUri = contentUri.Substring(prefix.Length - 1); // Remove it

			x_NavigationFrame.Navigate(contentUri);
		}

		private void OnFeedbackClick(object sender, RoutedEventArgs e)
		{
			new FeedbackDialog();
		}

		private void OnUpgradePremiumClick(object sender, RoutedEventArgs e)
		{
			//MyAccountPage page = new MyAccountPage();
			//page.OnUpgradePremiumClick(sender, e);
		}

		private void OnLogoClick(object sender, RoutedEventArgs e)
		{
		}
	}
}
