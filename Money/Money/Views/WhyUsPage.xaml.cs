using System.Windows.Controls;
using System.Windows.Navigation;
using ClassLibrary;

namespace Money
{
	public partial class WhyUsPage : Page
	{
		public WhyUsPage()
		{
			InitializeComponent();
			HtmlTextBlock content = this.ChildOfType<HtmlTextBlock>();
			content.HyperlinkClick += App.MainPage.OnHyperlinkClick;
		}
	}
}
