using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Interop;
using System.Xml.Linq;
using ClassLibrary;
//t using Money.TranslatorService;

namespace Money
{
	public partial class MainPage : UserControl
	{
		internal DragDrop DragDrop { get { return m_DragDrop; } }
		private DragDrop m_DragDrop = new DragDrop();

		internal NavigationHelper NavigationHelper { get { return m_NavigationHelper; } }
		private NavigationHelper m_NavigationHelper = new NavigationHelper();
//t		private Translator m_Translator = new Translator();

#if IPLOOKUP //j
		public IPAddress IpAddress { get { return m_IpAddress; } }
		private IPAddress m_IpAddress;
		private string m_CountryCode;
		private string m_CountryName;
#endif
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

			m_NavigationHelper.Init(x_NavigationFrame, x_MainTabControl);

			// This is the key to all XAML Binding; everything in the viual tree inherits this DataContext
			base.DataContext = App.Model;

			// Handle resizing
			Application.Current.Host.Content.Resized += BrowserHost_Resized;
			Resize();

//t			m_Translator.Init(x_LayoutRoot);

#if IPLOOKUP //j
			HtmlElement element = HtmlPage.Document.GetElementById("IP");
			if (element != null)
			{
				string ipAddress = element.GetAttribute("value");
				if (ipAddress == "127.0.0.1") ipAddress = "71.174.72.7";
				m_IpAddress = IPAddress.Parse(ipAddress);
				DownloadCountryCode();
			}
#endif
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
			m_NavigationHelper.NavigateTabControl(subTabControl);
		}

		private void OnSubTabControlChanged(object sender, SelectionChangedEventArgs e)
		{
			TabControl subTabControl = sender as TabControl;
			m_NavigationHelper.NavigateTabControl(subTabControl);
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
			MyAccountPage page = new MyAccountPage();
			page.OnUpgradePremiumClick(sender, e);
		}

		private void OnLogoClick(object sender, RoutedEventArgs e)
		{
			PageMethodInvoker pageMethod = new PageMethodInvoker();
			pageMethod.Completed += delegate(object userContext, string methodName, bool success, object result)
			{
				Helper.Trace("{0} = '{1}'", methodName, (result != null ? result.ToString() : "(null)"));
				string ipAddress = result as string;
				Uri url = new Uri(string.Format("http://ipinfodb.com/ip_query.php?ip={0}", ipAddress));

				WebClient client = new WebClient();
				client.DownloadStringCompleted += delegate(object sender1, DownloadStringCompletedEventArgs e1)
				{
					XDocument xml = XDocument.Parse(e1.Result);
					XElement response = xml.Element("Response");
					string ipa = response.Element("Ip").Value;
					string countryCode = response.Element("CountryCode").Value;
					string countryName = response.Element("CountryName").Value;
					string regionCode = response.Element("RegionCode").Value;
					string regionName = response.Element("RegionName").Value;
					string city = response.Element("City").Value;
					string postalCode = response.Element("ZipPostalCode").Value;
					string timeZone = response.Element("Timezone").Value;
					string gmtOffset = response.Element("Gmtoffset").Value;
					string dstOffset = response.Element("Dstoffset").Value;
					string latitude = response.Element("Latitude").Value;
					string longitude = response.Element("Longitude").Value;
				};
				client.DownloadStringAsync(url);
			};
			pageMethod.Invoke(null, "GetRequesterIP");

			PageMethodInvoker pageMethod2 = new PageMethodInvoker();
			pageMethod2.Completed += delegate(object userContext, string methodName, bool success, object result)
			{
				string text = result as string;
			};
			pageMethod2.Invoke(null, "GetRequesterProperties");
#if NOTUSED //j DEBUG
			XigniteServiceHelper xignite = new XigniteServiceHelper();
			xignite.GetHistoricalQuote("MSFT", "6/13/2008", GetHistoricalQuoteCompleted);
#endif
		}

		private void OnOpenProfileISOClick(object sender, RoutedEventArgs e)
		{
			DebugFunctionality.OnOpenProfileISOClick(sender, e);
		}

		private void OnOpenProfileFileClick(object sender, RoutedEventArgs e)
		{
			DebugFunctionality.OnOpenProfileFileClick(sender, e);
		}

		private void OnSaveProfileISOClick(object sender, RoutedEventArgs e)
		{
			DebugFunctionality.OnSaveProfileISOClick(sender, e);
		}

		private void OnCreateProfileClick(object sender, RoutedEventArgs e)
		{
			DebugFunctionality.OnCreateProfileClick(sender, e);
		}

		private void OnOpenDocumentISOClick(object sender, RoutedEventArgs e)
		{
			DebugFunctionality.OnOpenDocumentISOClick(sender, e);
		}

		private void OnOpenDocumentFileClick(object sender, RoutedEventArgs e)
		{
			DebugFunctionality.OnOpenDocumentFileClick(sender, e);
		}

		private void OnSaveDocumentISOClick(object sender, RoutedEventArgs e)
		{
			DebugFunctionality.OnSaveDocumentISOClick(sender, e);
		}

		private void OnCreateDocumentClick(object sender, RoutedEventArgs e)
		{
			DebugFunctionality.OnCreateDocumentClick(sender, e);
		}

		private void OnCreateDummyDocumentClick(object sender, RoutedEventArgs e)
		{
			DebugFunctionality.OnCreateDummyDocumentClick(sender, e);
		}

		private void OnCreateDatabasePackageClick(object sender, RoutedEventArgs e)
		{
			DebugFunctionality.OnCreateDatabasePackageClick(sender, e);
		}

#if NOTUSED //j
		private void OnSurveyClick(object sender, RoutedEventArgs e)
		{
			DebugFunctionality.OnSurveyClick(sender, e);
		}

		private void OnProfileBackClick(object sender, RoutedEventArgs e)
		{
			this.ChildOfType<InterviewPanel>().OnProfileBackClick(sender, e);
		}

		private void OnProfileNextClick(object sender, RoutedEventArgs e)
		{
			this.ChildOfType<InterviewPanel>().OnProfileNextClick(sender, e);
		}
#endif
		private void OnRunClick(object sender, RoutedEventArgs e)
		{
			App.Model.DocumentHolder.Document.BackgroundRun(0, 0);
			MessageBoxEx.ShowInfo("Document Run", "The document run has been initiated", null);
		}

#if IPLOOKUP //j
		private class CountryHolder
		{
			public string CountryCode { get; set; }
			public string CountryName { get; set; }
		}

		private void DownloadCountryCode()
		{	// Latest database is at: http://ip-to-country.webhosting.info/downloads/ip-to-country.csv.zip
			// Use the TxtToSql or MS SQL Data Wizard (Maestro) utilities to import it into an SQL table
			// Convert reversal:
			//int ipnum = 1024344063;
			//int w = (ipnum / 16777216) % 256;
			//int x = (ipnum / 65536) % 256;
			//int y = (ipnum / 256) % 256;
			//int z = (ipnum) % 256;
			byte[] bytes = m_IpAddress.GetAddressBytes();
			int w = bytes[0];
			int x = bytes[1];
			int y = bytes[2];
			int z = bytes[3];
			int ipNum = 16777216*w + 65536*x + 256*y + z;
			//int ipNum = m_IpAddress.GetHashCode(); // computed as z.y.x.w
			StringBuilder sql = new StringBuilder();
			Sql.SelectCountryCode(sql, ipNum);
			Sql.Get(sql, SelectCountryCodeCompleted);
		}
#endif

		private void GetHistoricalQuoteCompleted(object sender, DoWorkEventArgs e)
		{
			// e.Result is successful result object
			// e.Argument is an error string
			// e.Cancel = cancelled
			if (e.Argument != null)
			{
				string message = e.Argument as string;
				MessageBoxEx.ShowError("Get Historical Quote", message, null);
				return;
			}

			double price = (double)e.Result;
		}

#if GEO_ADDRESS //j
		private void OnGeoAddressClick(object sender, RoutedEventArgs e)
		{
			GeoAddress geo = new GeoAddress();
			geo.Request(""/*street*/, ""/*city*/, ""/*stateCode*/, "01810"/*zipCode*/, GeoAddress_RequestCompleted);
		}

		private static void GeoAddress_RequestCompleted(object sender, EventArgs e)
		{
			if (sender is GeoAddress)
			{
				GeoAddress geo = sender as GeoAddress;
				string str;
				if (!geo.ErrorMessage.IsEmpty())
					str = geo.ErrorMessage;
				else
					str = string.Format("Latitude: {0}, Longitude: {1}", geo.Latitude, geo.Longitude);

				MessageBoxEx.ShowOK("Geo address", str, null);
			}
		}
#endif
	}
}
