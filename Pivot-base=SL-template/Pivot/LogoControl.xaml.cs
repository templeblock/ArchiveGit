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

namespace Pivot
{
	public partial class LogoControl : UserControl
	{
		public LogoControl()
		{
			InitializeComponent();
			base.Loaded += OnLoaded;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			double scalex = base.Width / 116;
			double scaley = base.Height / 116;
			double scale = Math.Min(scalex, scaley);
			ScaleTransform scaleTransform = new ScaleTransform();
//			scaleTransform.CenterX = 58;
//			scaleTransform.CenterY = 58;
			scaleTransform.ScaleX = scalex;
			scaleTransform.ScaleY = scaley;
			x_LayoutRoot.RenderTransform = scaleTransform;
		}
	}
}
