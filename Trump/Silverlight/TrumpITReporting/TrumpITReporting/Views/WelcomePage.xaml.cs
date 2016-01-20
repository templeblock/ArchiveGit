using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;

namespace TrumpITReporting
{
	public partial class WelcomePage : Page
	{
		public WelcomePage()
		{
			InitializeComponent();
			base.Loaded += OnLoaded;
		}

		// Executes when the user navigates to this page.
		protected override void OnNavigatedTo(NavigationEventArgs e)
		{
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
		}
	}
}
