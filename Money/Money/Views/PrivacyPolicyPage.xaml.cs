using System.Windows.Controls;
using System.Windows.Navigation;
using ClassLibrary;

namespace Money
{
	public partial class PrivacyPolicyPage : Page
	{
		public PrivacyPolicyPage()
		{
			InitializeComponent();
			HtmlTextBlock content = base.Content.ChildOfType<HtmlTextBlock>();
			content.HyperlinkClick += App.MainPage.OnHyperlinkClick;
		}
	}
}
