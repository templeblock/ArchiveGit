using System.Windows.Controls;
using System.Windows.Navigation;
using ClassLibrary;

namespace Money
{
	public partial class DebtPage : Page
	{
		public DebtPage()
		{
			InitializeComponent();
			HtmlTextBlock content = base.Content.ChildOfType<HtmlTextBlock>();
			content.HyperlinkClick += App.MainPage.OnHyperlinkClick;
		}
	}
}
