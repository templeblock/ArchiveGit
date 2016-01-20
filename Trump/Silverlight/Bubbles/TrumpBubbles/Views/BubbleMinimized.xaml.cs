using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using ClassLibrary;
using System.Windows.Threading;

namespace TrumpBubbles
{
	public partial class BubbleMinimized : UserControl
	{
		public event EventHandler ShowComplete;
		public event EventHandler HideComplete;
		public event MouseButtonEventHandler Click;

		AnimationBase _Animation = new BubbleMinimizedAnimation();
		AnimationBase _AnimationTitle = new BubbleMinimizedAnimation();

		// The Title dependency property
		#region The Title dependency property
		public static readonly DependencyProperty TitleProperty =
			DependencyProperty.Register("Title", typeof(string), typeof(BubbleMinimized),
			null);

		public string Title
		{
			get { return (string)GetValue(TitleProperty); }
			set { SetValue(TitleProperty, value); }
		}
		#endregion

		// The ContentTag dependency property
		#region The ContentTag dependency property
		public static readonly DependencyProperty ContentTagProperty =
			DependencyProperty.Register("ContentTag", typeof(string), typeof(BubbleMinimized),
			null);

		public string ContentTag
		{
			get { return (string)GetValue(ContentTagProperty); }
			set { SetValue(ContentTagProperty, value); }
		}
		#endregion

		// The IconSourceUri dependency property
		#region The IconSourceUri dependency property
		public static readonly DependencyProperty IconSourceUriProperty =
			DependencyProperty.Register("IconSourceUri", typeof(string), typeof(BubbleMinimized),
			null);

		public string IconSourceUri
		{
			get { return (string)GetValue(IconSourceUriProperty); }
			set { SetValue(IconSourceUriProperty, value); }
		}
		#endregion

		// The IsShown dependency property
		#region The IsShown dependency property
		public static readonly DependencyProperty IsShownProperty =
			DependencyProperty.Register("IsShown", typeof(bool), typeof(BubbleMinimized),
			new PropertyMetadata(false, (d, e) => ((BubbleMinimized)d).OnIsShownPropertyChanged((bool)e.OldValue, (bool)e.NewValue)));

		public bool IsShown
		{
			get { return (bool)GetValue(IsShownProperty); }
			set { SetValue(IsShownProperty, value); }
		}

		private void OnIsShownPropertyChanged(bool oldValue, bool newValue)
		{
			if (oldValue == newValue)
				return;
			
			if (newValue == true)
			{
				base.Visibility = Visibility.Visible;
				_Animation.Forward();
			}
			else
			{
				_Animation.Reverse();
			}
		}
		#endregion

		public BubbleMinimized()
		{
			InitializeComponent();
			base.Loaded += OnLoaded;
			
			_Animation.SetTarget(this);
			_Animation.ForwardCompleted += ForwardCompleted;
			_Animation.ReverseCompleted += ReverseCompleted;
			FrameworkElement showHidePanel = ((base.Content as FrameworkElement)).FindName("x_ShowHidePanel") as FrameworkElement;
			_AnimationTitle.SetTarget(showHidePanel);

			IsShown = true;//(base.Visibility == Visibility.Visible);
		}

		private void ForwardCompleted(object sender, EventArgs e)
		{
			if (ShowComplete != null)
				ShowComplete(this, e);
		}
	
		private void ReverseCompleted(object sender, EventArgs e)
		{
			base.Visibility = Visibility.Collapsed;

			if (HideComplete != null)
				HideComplete(this, e);
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			Image icon = ((base.Content as ContentScaler)).FindName("x_Icon") as Image;
			//Image icon = x_Icon;
			if (icon != null)
				icon.Source = ResourceFile.GetBitmap(IconSourceUri, From.This);
			TextBlock title = ((base.Content as ContentScaler)).FindName("x_Title") as TextBlock;
			//TextBlock title = x_Title;
			if (title != null)
				title.Text = Title;
		}
	
		private void OnBubbleOpenClick(object sender, MouseButtonEventArgs e)
		{
			if (Click != null)
				Click(this, e);
		}

#if false
		private DispatcherTimer _EnterTimer = null;
		private FrameworkElement _ElementEnterPending = null;

		private void CreateTimer()
		{
			_EnterTimer = new DispatcherTimer();
			_EnterTimer.Interval = new TimeSpan(100 * TimeSpan.TicksPerMillisecond);
			_EnterTimer.Tick += OnBubbleEnterTick;
		}

		private void OnBubbleEnterTick(object sender, EventArgs e)
		{
			if (_EnterTimer == null)
				return;
			_EnterTimer.Stop();
		    _ElementEnterPending.Visibility = Visibility.Visible;
			_AnimationTitle.Forward();
		}
#endif
		private void OnMouseEnter(object sender, MouseEventArgs e)
		{
			FrameworkElement showHidePanel = ((base.Content as FrameworkElement)).FindName("x_ShowHidePanel") as FrameworkElement;
			if (showHidePanel == null)
				return;
#if false
			_ElementEnterPending = showHidePanel;
			if (_EnterTimer == null)
				CreateTimer();
			_EnterTimer.Start();
#else
		    showHidePanel.Visibility = Visibility.Visible;
			_AnimationTitle.Forward();
#endif
		}

		private void OnMouseLeave(object sender, MouseEventArgs e)
		{
			FrameworkElement showHidePanel = ((base.Content as FrameworkElement)).FindName("x_ShowHidePanel") as FrameworkElement;
			if (showHidePanel == null)
				return;
#if false
			if (_EnterTimer != null && _EnterTimer.IsEnabled)
				_EnterTimer.Stop();
			if (showHidePanel != null)
			    showHidePanel.Visibility = Visibility.Collapsed;
			_AnimationTitle.Reverse();
#else
		    showHidePanel.Visibility = Visibility.Collapsed;
			_AnimationTitle.Reverse();
#endif
		}
	}

	internal class BubbleMinimizedAnimation : TransformAnimationBase
	{
		public BubbleMinimizedAnimation()
		{
			SecondsForward = 0.35;
			SecondsReverse = 0.35;
			Direction = Direction2D.XY;
			Origin = new Point(0, 0);
			Scale = new Point(0, 0);
			Offset = new Point(0, 0);
			EasingFunction = null;	
		}
	}
}
