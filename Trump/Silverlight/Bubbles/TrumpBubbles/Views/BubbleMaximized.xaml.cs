using System;
using System.Windows;
using System.Windows.Browser;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Threading;
using ClassLibrary;
using Telerik.Windows.Controls;

namespace TrumpBubbles
{
	public partial class BubbleMaximized : UserControl
	{
		private const double _ExtraIFrameHeight = 22 + 8; // Extra height added to the size of each HtmlPlaceholder (iframe) to prevent scrollbars
		private const double _SmallChange = _ExtraIFrameHeight + 18;
		private const double _DefaultHeight = 0;
		private const double _MinHeightNavigator = 500; // Set this to 0 when pages get their min-height attributes
		private double _MinHeight = 0;

		//public event EventHandler ShowComplete;
		public event EventHandler HideComplete;
		public event MouseButtonEventHandler Click;
		//public event EventHandler SourceUrlChanged;
		public event EventHandler HtmlHeightChanged;
	
		public RadHtmlPlaceholder HtmlPlaceholder { get { return x_HtmlPlaceholder; } }
		public HtmlElement HtmlPresenter { get { return x_HtmlPlaceholder.HtmlPresenter; } }

		// Note: The iframe's document and body will be null if the source url is outside of the domain
		public HtmlElement IFrame { get { return (HtmlPresenter != null ? HtmlPresenter.Children[0] as HtmlElement : null); } }
		public string IFrameSrc { get { return (IFrame != null ? IFrame.GetProperty("src") as string : null); } }
		public HtmlWindow IFrameWindow { get { return (IFrame != null ? IFrame.GetProperty("contentWindow") as HtmlWindow : null); } }
		public HtmlDocument IFrameDocument { get { return (_contentDocument != null ? _contentDocument : _document); } }
		public HtmlElement IFrameBody { get { return (IFrameDocument != null ? IFrameDocument.GetProperty("body") as HtmlElement : null); } }

		private HtmlDocument _document { get { return (IFrameWindow != null ? IFrameWindow.GetProperty("document") as HtmlDocument : null); } }
		private HtmlDocument _contentDocument { get { return (IFrame != null ? IFrame.GetProperty("contentDocument") as HtmlDocument : null); } }

		private AnimationBase _Animation = new BubbleMaximizedAnimation();
		private DispatcherTimer _IFramePeekTimer = null;
		private const int _IFramePeekTimerIntervalMS = 1000;

		// The Title dependency property
		#region The Title dependency property
		public static readonly DependencyProperty TitleProperty =
			DependencyProperty.Register("Title", typeof(string), typeof(BubbleMaximized),
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
			DependencyProperty.Register("ContentTag", typeof(string), typeof(BubbleMaximized),
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
			DependencyProperty.Register("IconSourceUri", typeof(string), typeof(BubbleMaximized),
			null);

		public string IconSourceUri
		{
			get { return (string)GetValue(IconSourceUriProperty); }
			set { SetValue(IconSourceUriProperty, value); }
		}
		#endregion

		// The SourceUrl dependency property
		#region The SourceUrl dependency property
		public static readonly DependencyProperty SourceUrlProperty =
			DependencyProperty.Register("SourceUrl", typeof(string), typeof(BubbleMaximized),
			null);

		public string SourceUrl
		{
			get { return (string)GetValue(SourceUrlProperty); }
			set { SetValue(SourceUrlProperty, value); }
		}
		#endregion

		// The HtmlHeight dependency property
		#region The HtmlHeight dependency property
		public static readonly DependencyProperty HtmlHeightProperty =
			DependencyProperty.Register("HtmlHeight", typeof(double), typeof(BubbleMaximized),
			null);

		public double HtmlHeight
		{
			get { return (double)GetValue(HtmlHeightProperty); }
			set { SetValue(HtmlHeightProperty, value); }
		}
		#endregion

		// The ExtraControlHeight dependency property
		#region The ExtraControlHeight dependency property
		public static readonly DependencyProperty ExtraControlHeightProperty =
			DependencyProperty.Register("ExtraControlHeight", typeof(double), typeof(BubbleMaximized),
			null);

		public double ExtraControlHeight
		{
			get { return (double)GetValue(ExtraControlHeightProperty); }
			set { SetValue(ExtraControlHeightProperty, value); }
		}
		#endregion

		// The SelectedItem dependency property
		#region The SelectedItem dependency property
		public static readonly DependencyProperty SelectedItemProperty =
			DependencyProperty.Register("SelectedItem", typeof(object), typeof(BubbleMaximized),
			new PropertyMetadata(null, (d, e) => ((BubbleMaximized)d).OnSelectedItemPropertyChanged((object)e.OldValue, (object)e.NewValue)));

		public object SelectedItem
		{
			get { return (object)GetValue(SelectedItemProperty); }
			set { SetValue(SelectedItemProperty, value); }
		}

		private void OnSelectedItemPropertyChanged(object oldValue, object newValue)
		{
			IsShown = ((newValue as Control) == this);
		}
		#endregion

		// The IsShown dependency property
		#region The IsShown dependency property
		public static readonly DependencyProperty IsShownProperty =
			DependencyProperty.Register("IsShown", typeof(bool), typeof(BubbleMaximized),
			new PropertyMetadata(false, (d, e) => ((BubbleMaximized)d).OnIsShownPropertyChanged((bool)e.OldValue, (bool)e.NewValue)));

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
				StartIFramePeekTimer();

				// When being shown, hide the html because it doesn't animate well
				x_HtmlPlaceholder.Visibility = Visibility.Collapsed;
				x_HtmlPlaceholder.UpdateLayout();

				base.Visibility = Visibility.Visible;
				_Animation.Forward();
			}
			else
			{
				StopIFramePeekTimer();

				// When being hidden, hide the html because it doesn't animate well
				x_HtmlPlaceholder.Visibility = Visibility.Collapsed;
				x_HtmlPlaceholder.UpdateLayout();

				_Animation.Reverse();
			}
		}
		#endregion

		// The HideClose dependency property
		#region The HideClose dependency property
		public static readonly DependencyProperty HideCloseProperty =
			DependencyProperty.Register("HideClose", typeof(bool), typeof(BubbleMaximized),
			null);

		public bool HideClose
		{
			get { return (bool)GetValue(HideCloseProperty); }
			set { SetValue(HideCloseProperty, value); }
		}
		#endregion

		public BubbleMaximized()
		{
			InitializeComponent();
			base.Loaded += OnLoaded;
			HtmlPage.Window.AttachEvent("onresize", OnBrowserResize);

			bool isIE = BrowserScreenInformation.IsInternetExplorer;
			bool isChrome = BrowserScreenInformation.IsChrome;
			bool isFirefox = BrowserScreenInformation.IsFirefox;
			bool needMinHeight = isFirefox;
			if (needMinHeight)
				_MinHeight = _MinHeightNavigator;
			
			_Animation.SetTarget(this);
			_Animation.ForwardCompleted += ForwardCompleted;
			_Animation.ReverseCompleted += ReverseCompleted;
		}

		private void OnBrowserResize(object sender, HtmlEventArgs e)
		{
			if (x_HtmlPlaceholder.Visibility == Visibility.Visible)
				x_HtmlPlaceholder.InvalidateArrange();
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			x_HtmlPlaceholder.SourceUrl = new Uri(SourceUrl, (SourceUrl.IndexOf("http") >= 0 ? UriKind.Absolute : UriKind.Relative));
			x_HtmlPlaceholder.Height = (HtmlHeight != 0 ? HtmlHeight + _ExtraIFrameHeight : _DefaultHeight);

			x_Title.Text = Title;
			x_Icon.Source = ResourceFile.GetBitmap(IconSourceUri, From.This);

			if (HideClose)
			{
				x_Header.Cursor = Cursors.Arrow;
				x_CloseImage.Visibility = Visibility.Collapsed;
				x_CloseText.Visibility = Visibility.Collapsed;
			}

			if (IFrame != null)
			{
				IFrame.SetProperty("allowTransparency", true);
				IFrame.SetProperty("frameborder", "0");
			}
		}

		private void StartIFramePeekTimer()
		{
			if (_IFramePeekTimer == null)
			{
				_IFramePeekTimer = new DispatcherTimer();
				_IFramePeekTimer.Interval = new TimeSpan(_IFramePeekTimerIntervalMS * TimeSpan.TicksPerMillisecond);
				_IFramePeekTimer.Tick += OnIFramePeekTimerTick;
			}

			OnIFramePeekTimerTick(null, new EventArgs());
			_IFramePeekTimer.Start();
		}

		private void StopIFramePeekTimer()
		{
			if (_IFramePeekTimer == null)
				return;

			_IFramePeekTimer.Stop();
		}

		private void OnIFramePeekTimerTick(object sender, EventArgs e)
		{
			// The IFrameDocument and IFrameBody will be null if SourceUrl is outside of the domain (security violation)
			HtmlDocument document = IFrameDocument;
			if (document != null)
			{
				ScriptObject location = document.GetProperty("location") as ScriptObject;
				string href = (location != null ? location.GetProperty("href") as string : null);
				if (href != null && SourceUrl != href)
				{
					SourceUrl = href;
					//if (SourceUrlChanged != null)
					//    SourceUrlChanged(this, e);
				}
			}

			HtmlElement body = IFrameBody;
			if (body != null)
			{
				double? scrollHeight = body.GetProperty("scrollHeight") as double?;
				if (scrollHeight == null || scrollHeight == 0)
					scrollHeight = body.GetProperty("offsetHeight") as double?;
				//if (scrollHeight == null || scrollHeight == 0)
				//	scrollHeight = body.GetElementDimension("height");
				if (scrollHeight == null || scrollHeight == 0)
					scrollHeight = HtmlHeight;

				double height = Math.Max((double)scrollHeight, _MinHeight);
				if (height != HtmlHeight /*&& height != Plugin.Height - ExtraControlHeight*/)
				{
					double oldHtmlHeight = HtmlHeight;
					double delta = Math.Abs(height - HtmlHeight);
					HtmlHeight = height;
					// ignore small changes to avoid the posibility of a constantly growing document
					if (oldHtmlHeight == 0 || delta > _SmallChange)
					{
						x_HtmlPlaceholder.Height = HtmlHeight + _ExtraIFrameHeight; //(hasScrollbar ? 21 : 0);
						if (HtmlHeightChanged != null)
							HtmlHeightChanged(this, e);
					}
				}
#if DEBUG
				x_Debug.Text = string.Format("{0} {1}", GetPageName(), scrollHeight);
#endif
			}
		}

		private string GetSizes()
		{
			string name = null;
			return name;
		}

		private string GetPageName()
		{
			string name = null;
			if (SourceUrl != null)
			{
				int index = SourceUrl.LastIndexOf("/");
				if (index >= 0)
					name = SourceUrl.Substring(index);
			}
			return name;
		}

		private void ForwardCompleted(object sender, EventArgs e)
		{
			x_HtmlPlaceholder.Visibility = Visibility.Visible;
			x_HtmlPlaceholder.UpdateLayout();
			//if (ShowComplete != null)
			//    ShowComplete(this, e);
		}
	
		private void ReverseCompleted(object sender, EventArgs e)
		{
			base.Visibility = Visibility.Collapsed;

			if (HideComplete != null)
				HideComplete(this, e);
		}

		private void OnBubbleCloseClick(object sender, MouseButtonEventArgs e)
		{
			if (HideClose)
				return;
			if (Click != null)
				Click(this, e);
		}
	}

	internal class BubbleMaximizedAnimation : TransformAnimationBase
	{
		public BubbleMaximizedAnimation()
		{
            DelayForward = 5;
            DelayReverse = 0;
			SecondsForward = 5.0;
			SecondsReverse = 5.0;
			Direction = Direction2D.XY;
			Origin = new Point(0.5, 0.5);
			Scale = new Point(0, 0);
			Offset = new Point(0, 0);
			EasingFunction = null;	
		}
	}
}
