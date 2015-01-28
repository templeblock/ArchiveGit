using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace Money
{
	public partial class WaitingWindow
	{
		public WaitingWindow()
		{
			InitializeComponent();
		}

		public string NoticeText
		{
			set
			{
				TextBlock_Notice.Text = value;
			}
		}
	}
}

