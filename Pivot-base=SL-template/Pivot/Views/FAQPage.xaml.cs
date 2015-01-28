using System.Windows.Controls;
using System.Windows.Navigation;
using ClassLibrary;

namespace Pivot
{
	public partial class FAQPage : Page
	{
		public FAQPage()
		{
			InitializeComponent();
			HtmlTextBlock content = this.ChildOfType<HtmlTextBlock>();
			content.HyperlinkClick += App.MainPage.OnHyperlinkClick;
		}

		// Called when a page becomes the active page in a frame
		protected override void OnNavigatedTo(NavigationEventArgs e)
		{
		}

		// Called just before a page is no longer the active page in a frame
		protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
		{
		}

		// Called when a page is no longer the active page in a frame
		protected override void OnNavigatedFrom(NavigationEventArgs e)
		{
		}

		// Called when navigating to a fragment on a page
		protected override void OnFragmentNavigation(FragmentNavigationEventArgs e)
		{
		}
	}
}
