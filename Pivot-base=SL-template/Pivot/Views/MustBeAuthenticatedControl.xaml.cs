using System.Windows.Controls;
using ClassLibrary;

namespace Pivot
{
	public partial class MustBeAuthenticatedControl : UserControl
	{
		public MustBeAuthenticatedControl()
		{
			InitializeComponent();
			//HtmlTextBlock content = this.ChildOfType<HtmlTextBlock>();
			//content.HyperlinkClick += App.MainPage.OnHyperlinkClick;
		}
	}
}
