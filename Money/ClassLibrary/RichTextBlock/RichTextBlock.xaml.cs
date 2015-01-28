using System.Windows.Controls;

namespace ClassLibrary
{
	public class RichTextBlock : RichTextBox
	{
		public RichTextBlock()
		{
			base.DefaultStyleKey = base.GetType();
		}
	}
}
