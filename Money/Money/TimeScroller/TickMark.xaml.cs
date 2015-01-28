using System;
using System.Windows.Controls;

namespace Money
{
	public partial class TickMark : UserControl
    {
		public string Label { get { return x_Label.Text; } set { x_Label.Text = value; } }

		public TickMark()
		{
			InitializeComponent();
		}

		public TickMark(String text)
			: base()
        {
			Label = text;
		}
    }
}
