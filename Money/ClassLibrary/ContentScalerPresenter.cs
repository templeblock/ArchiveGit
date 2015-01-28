using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace ClassLibrary
{
	public class ContentScalerPresenter : ContentControl
	{
		// The ScaleX dependency property
		public static readonly DependencyProperty ScaleXProperty =
			DependencyProperty.Register("ScaleX", typeof(double), typeof(ContentScalerPresenter),
			new PropertyMetadata(1.0, OnScaleChanged));

		public double ScaleX
		{
			set { SetValue(ScaleXProperty, value); }
			get { return (double)GetValue(ScaleXProperty); }
		}

		// The ScaleY dependency property
		public static readonly DependencyProperty ScaleYProperty =
			DependencyProperty.Register("ScaleY", typeof(double), typeof(ContentScalerPresenter),
			new PropertyMetadata(1.0, OnScaleChanged));

		public double ScaleY
		{
			set { SetValue(ScaleYProperty, value); }
			get { return (double)GetValue(ScaleYProperty); }
		}

		public ContentScalerPresenter()
		{
			RenderTransform = new ScaleTransform();
		}

		static void OnScaleChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
		{
			ContentScalerPresenter scaler = sender as ContentScalerPresenter;
			(scaler.RenderTransform as ScaleTransform).ScaleX = scaler.ScaleX;
			(scaler.RenderTransform as ScaleTransform).ScaleY = scaler.ScaleY;
			scaler.InvalidateMeasure();
		}

		protected override Size MeasureOverride(Size availableSize)
		{
			Size desiredSize = base.MeasureOverride(availableSize);
			desiredSize.Width *= ScaleX;
			desiredSize.Height *= ScaleY;
			return desiredSize;
		}
	}
}
