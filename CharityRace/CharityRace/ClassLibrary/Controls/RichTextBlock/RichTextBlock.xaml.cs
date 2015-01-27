using System.Windows.Controls;

// Written by: Jim McCurdy (jmccurdy@facetofacesoftware.com)

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
