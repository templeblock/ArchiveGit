using System.Windows.Controls;
using ClassLibrary;

namespace Money
{
	public partial class BuzzPage : Page
	{
		public BuzzPage()
		{
			InitializeComponent();
			HtmlTextBlock content = this.ChildOfType<HtmlTextBlock>();
			content.HyperlinkClick += App.MainPage.OnHyperlinkClick;
		}
	}
}
