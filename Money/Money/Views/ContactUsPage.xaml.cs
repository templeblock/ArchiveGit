using System.Windows.Controls;
using ClassLibrary;

namespace Money
{
	public partial class ContactUsPage : Page
	{
		public ContactUsPage()
		{
			InitializeComponent();
			HtmlTextBlock content = this.ChildOfType<HtmlTextBlock>();
			content.HyperlinkClick += App.MainPage.OnHyperlinkClick;
		}
	}
}
