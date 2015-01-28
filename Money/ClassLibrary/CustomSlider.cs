using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Threading;

namespace ClassLibrary
{
	// Derived from http://on10.net/blogs/benwagg/New-Custom-Slider-for-Silverlight-2-media-players/
	public class CustomSlider : Slider
	{
		public event Action<double> ValueChangedComplete;
		public event Action<double> ValueChangedInDrag;
		public event EventHandler DragStarted;
		public event EventHandler DragCompleted;

		private Thumb x_Thumb;

		private DispatcherTimer m_DragTimer = new DispatcherTimer();
		private double m_Value;
		private double m_PrevValue;
		private double m_DragTimeElapsed = 0;
		private const short k_DragWaitThreshold = 200;
		private const short k_DragWaitInterval = 100;

		public CustomSlider()
		{
			base.ValueChanged += OnSliderValueChanged;
			m_DragTimer.Interval = new TimeSpan(0, 0, 0, 0, k_DragWaitInterval);
			m_DragTimer.Tick += OnDragTimerTick;
		}

		public override void OnApplyTemplate()
		{
			base.OnApplyTemplate();

			x_Thumb = base.GetTemplateChild("HorizontalThumb") as Thumb;
			if (x_Thumb != null)
			{
				x_Thumb.DragStarted += OnDragStarted;
				x_Thumb.DragCompleted += OnDragCompleted;
			}

			RepeatButton trackLeft = base.GetTemplateChild("HorizontalTrackLargeChangeDecreaseRepeatButton") as RepeatButton;
			RepeatButton trackRight = base.GetTemplateChild("HorizontalTrackLargeChangeIncreaseRepeatButton") as RepeatButton;
			// For some reason, the code below does not work, so we must call AddHandler() directly
			//if (trackLeft != null) trackLeft.MouseLeftButtonDown += new MouseButtonEventHandler(OnMoveThumbToMouse);
			if (trackLeft != null) trackLeft.AddHandler(Button.MouseLeftButtonDownEvent, new MouseButtonEventHandler(OnMoveThumbToMouse), true);
			if (trackRight != null) trackRight.AddHandler(Button.MouseLeftButtonDownEvent, new MouseButtonEventHandler(OnMoveThumbToMouse), true);
		}

		void OnDragTimerTick(object sender, EventArgs e)
		{
			m_DragTimeElapsed += k_DragWaitInterval;
			if (m_DragTimeElapsed < k_DragWaitThreshold)
				return;

			m_DragTimeElapsed = 0;
			if (ValueChangedInDrag != null && m_Value != m_PrevValue)
			{
				//Debug.WriteLine(string.Format("ValueChangedInDrag {0}", m_Value));
				ValueChangedInDrag(m_Value);
				m_PrevValue = m_Value;
			}
		}

		void OnSliderValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
		{
			m_Value = e.NewValue;
			bool thumbIsDragging = (x_Thumb != null && x_Thumb.IsDragging);
			//Debug.Assert(e.NewValue == base.Value);
			//Debug.Assert(thumbIsDragging == m_DragTimer.IsEnabled);
			if (thumbIsDragging)
				return;

			if (ValueChangedComplete != null && m_Value != m_PrevValue)
			{
				//Debug.WriteLine(string.Format("ValueChangedComplete {0}", m_Value));
				ValueChangedComplete(m_Value);
				m_PrevValue = m_Value;
			}
		}

		private void OnMoveThumbToMouse(object sender, MouseButtonEventArgs e)
		{
			if (x_Thumb == null || base.Orientation != Orientation.Horizontal)
				return;

			e.Handled = true;
			Point p = e.GetPosition(this);
			double scale = (p.X - (x_Thumb.ActualWidth / 2)) / (ActualWidth - x_Thumb.ActualWidth);
			if (scale < 0) scale = 0;
			if (scale > 1) scale = 1;
			base.Value = Minimum + Math.Floor((Maximum - Minimum) * scale);
		}

		public virtual void OnDragStarted(object sender, DragStartedEventArgs e)
		{
			//Debug.WriteLine(string.Format("OnDragStarted"));
			if (DragStarted != null)
				DragStarted(this, e);

			m_DragTimer.Start();
		}

		public virtual void OnDragCompleted(object sender, DragCompletedEventArgs e)
		{
			m_DragTimer.Stop();
			m_DragTimeElapsed = 0;

			base.Value = m_Value;

			//Debug.WriteLine(string.Format("OnDragCompleted"));
			if (DragCompleted != null)
				DragCompleted(this, e);
		}
	}
}
