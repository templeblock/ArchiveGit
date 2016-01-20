using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace TrumpVideo
{
	// For best full-screen video performance, add
	// <param name="enableGPUAcceleration" value="true" /> under the <object> tag.

	public partial class VideoControl : UserControl
	{
		public bool IsVisible { get; set; }
		// mediaElement.Position updates TimelineSlider.Value, and
		// updating TimelineSlider.Value updates mediaElement.Position, 
		// this variable helps us break the infinite loop
		private bool _InTickEvent;
		private bool _TemporaryPause;
		private double _OriginalWidth;
		private double _OriginalHeight;

		public VideoControl()
		{
			InitializeComponent();
			base.Loaded += OnLoaded;
			base.LayoutUpdated += OnLayoutUpdated;

			CompositionTarget.Rendering += OnCompositionTargetRendering;
			Application.Current.Host.Content.FullScreenChanged += OnFullScreenChanged;
		}

		void OnLoaded(object sender, RoutedEventArgs e)
		{
			// Handle resizing
			Application.Current.Host.Content.Resized += OnBrowserHostResized;
			Resize();

			x_PlayPause.IsChecked = x_MediaElement.AutoPlay;
			x_BigPlay.Visibility = (x_MediaElement.AutoPlay ? Visibility.Collapsed : Visibility.Visible);
		}

		void OnLayoutUpdated(object sender, EventArgs e)
		{
			if (IsVisible == this.IsVisibleInTree())
				return;

			IsVisible = !IsVisible;
			if (!IsVisible)
				x_MediaElement.Stop();
		}

		private void OnMediaCurrentStateChanged(object sender, RoutedEventArgs e)
		{
			MediaElement media = sender as MediaElement;
			switch (media.CurrentState)
			{
				case MediaElementState.AcquiringLicense:
					break;
				case MediaElementState.Buffering:
					break;
				case MediaElementState.Closed:
					break;
				case MediaElementState.Individualizing:
					break;
				case MediaElementState.Opening:
					break;
				case MediaElementState.Paused:
					if (!_TemporaryPause)
						x_PlayPause.IsChecked = false;
					break;
				case MediaElementState.Playing:
					x_PlayPause.IsChecked = true;
					break;
				case MediaElementState.Stopped:
					x_PlayPause.IsChecked = false;
					break;
			}
		}

		private void OnMediaOpened(object sender, RoutedEventArgs e)
		{
			MediaElement media = sender as MediaElement;
			TimeSpan duration = media.NaturalDuration.TimeSpan;
			x_TotalTime.Text = TimeSpanToString(duration);
			Resize();
		}

		private void OnMediaEnded(object sender, RoutedEventArgs e)
		{
			MediaElement media = sender as MediaElement;
			media.Stop();
		}

		#region play button

		private void OnBigPlayClick(object sender, RoutedEventArgs e)
		{
		    x_PlayPause.IsChecked = true;
		    OnPlayPauseClick(sender, e);
		}

		private void OnPlayPauseClick(object sender, RoutedEventArgs e)
		{
			x_BigPlay.Visibility = Visibility.Collapsed;
			if (x_PlayPause.IsChecked == true)
				x_MediaElement.Play();
			else
				x_MediaElement.Pause();
		}

		private void OnPlayPauseChecked(object sender, RoutedEventArgs e)
		{
		}

		#endregion

		#region timelineSlider

		private void Seek(double percentComplete)
		{
			if (_InTickEvent)
				return; // throw new Exception("Can't call Seek() now, you'll get an infinite loop");

			TimeSpan duration = x_MediaElement.NaturalDuration.TimeSpan;
			int newPosition = (int)(duration.TotalSeconds * percentComplete);
			x_MediaElement.Position = new TimeSpan(0, 0, newPosition);

			// let the next CompositionTarget.Rendering take care of updating the text blocks
		}

		private Slider GetSliderParent(object sender)
		{
			FrameworkElement element = (FrameworkElement)sender;
			do
			{
				element = (FrameworkElement)VisualTreeHelper.GetParent(element);
			} while (!(element is Slider));
			return (Slider)element;
		}

		private void OnLeftTrackMouseDown(object sender, MouseButtonEventArgs e)
		{
			e.Handled = true;
			FrameworkElement lefttrack = (sender as FrameworkElement).FindName("LeftTrack") as FrameworkElement;
			FrameworkElement righttrack = (sender as FrameworkElement).FindName("RightTrack") as FrameworkElement;
			double position = e.GetPosition(lefttrack).X;
			double width = righttrack.TransformToVisual(lefttrack).Transform(new Point(righttrack.ActualWidth, righttrack.ActualHeight)).X;
			double percent = position / width;
			Slider slider = GetSliderParent(sender);
			slider.Value = percent;
		}

		private void OnThumbDragStarted(object sender, System.Windows.Controls.Primitives.DragStartedEventArgs e)
		{
			if (x_Timeline != GetSliderParent(sender))
				return;

			_TemporaryPause = (x_MediaElement.CurrentState == MediaElementState.Playing);
			if (_TemporaryPause)
				x_MediaElement.Pause();
		}

		private void OnThumbDragCompleted(object sender, System.Windows.Controls.Primitives.DragCompletedEventArgs e)
		{
			if (x_Timeline != GetSliderParent(sender))
				return;

			if (_TemporaryPause)
				x_MediaElement.Play();
			_TemporaryPause = false;
		}

		private void OnTimelineValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
		{
			if (_InTickEvent)
				return;

			Seek(x_Timeline.Value);
		}

		#endregion

		#region updating current time

		private void OnCompositionTargetRendering(object sender, EventArgs e)
		{
			_InTickEvent = true;

			TimeSpan duration = x_MediaElement.NaturalDuration.TimeSpan;
			if (duration.TotalSeconds != 0)
			{
				double percentComplete = (x_MediaElement.Position.TotalSeconds / duration.TotalSeconds);
				x_Timeline.Value = percentComplete;
				string text = TimeSpanToString(x_MediaElement.Position);
				if (x_CurrentTime.Text != text)
					x_CurrentTime.Text = text;
			}

			_InTickEvent = false;
		}

		private static string TimeSpanToString(TimeSpan time)
		{
			return string.Format("{0:00}:{1:00}", (time.Hours * 60) + time.Minutes, time.Seconds);
		}

		#endregion

		private void OnMuteClick(object sender, RoutedEventArgs e)
		{
			x_MediaElement.IsMuted = (bool)x_Mute.IsChecked;
		}

		#region fullscreen mode

		private void OnFullScreenClick(object sender, RoutedEventArgs e)
		{
			Application.Current.Host.Content.IsFullScreen = !Application.Current.Host.Content.IsFullScreen;
		}

		private void OnFullScreenChanged(object sender, EventArgs e)
		{
			Resize();
		}

		private void OnBrowserHostResized(object sender, EventArgs e)
		{
			Resize();
		}

		private void Resize()
		{
			if (_OriginalWidth == 0 && _OriginalHeight == 0)
			{
				_OriginalWidth = base.Width;
				_OriginalHeight = base.Height;
			}

			if (App.Current.Host.Content.IsFullScreen)
			{
				base.Width = Application.Current.Host.Content.ActualWidth;
				base.Height = Application.Current.Host.Content.ActualHeight;
			}
			else
			{
				base.Width = _OriginalWidth;
				base.Height = _OriginalHeight;
			}
		}

		#endregion
	}

	internal static class UIElementExt
	{
		// Extension for UIElement
		internal static bool IsVisibleInTree(this UIElement uiElement)
		{
			DependencyObject element = uiElement;
			if (element == null)
				return false;

			do
			{
				if ((element is UIElement) && (element as UIElement).Visibility != Visibility.Visible)
					return false;
			}
			while ((element = VisualTreeHelper.GetParent(element)) != null);

			return true;
		}
	}
}
