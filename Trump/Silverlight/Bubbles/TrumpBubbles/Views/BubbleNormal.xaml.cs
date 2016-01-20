using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Browser;
using ClassLibrary;

namespace TrumpBubbles
{
	public partial class BubbleNormal : UserControl
	{
		//public event EventHandler ShowComplete;
		//public event EventHandler HideComplete;
		public event MouseButtonEventHandler Click;
	
		//AnimationBase _Animation = new BubbleNormalAnimation();

		// The Title dependency property
		#region The Title dependency property
		public static readonly DependencyProperty TitleProperty =
			DependencyProperty.Register("Title", typeof(string), typeof(BubbleNormal),
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
			DependencyProperty.Register("ContentTag", typeof(string), typeof(BubbleNormal),
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
			DependencyProperty.Register("IconSourceUri", typeof(string), typeof(BubbleNormal),
			null);

		public string IconSourceUri
		{
			get { return (string)GetValue(IconSourceUriProperty); }
			set { SetValue(IconSourceUriProperty, value); }
		}
		#endregion

		// The CallToActionTextProperty dependency property
		#region The CallToActionTextProperty dependency property
		public static readonly DependencyProperty CallToActionTextProperty =
			DependencyProperty.Register("CallToActionText", typeof(string), typeof(BubbleNormal),
			null);

		public string CallToActionText
		{
			get { return (string)GetValue(CallToActionTextProperty); }
			set { SetValue(CallToActionTextProperty, value); }
		}
		#endregion

		// The IsShown dependency property
		#region The IsShown dependency property
		public static readonly DependencyProperty IsShownProperty =
			DependencyProperty.Register("IsShown", typeof(bool), typeof(BubbleNormal),
			new PropertyMetadata(false, (d, e) => ((BubbleNormal)d).OnIsShownPropertyChanged((bool)e.OldValue, (bool)e.NewValue)));

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
				//_Animation.Forward();
				VisualStateManager.GoToState(this, "Shown", true);
			}
			else
			{
				//_Animation.Reverse();
				VisualStateManager.GoToState(this, "NotShown", true);
			}
		}
		#endregion

		// The ItemContent dependency property
		#region The ItemContent dependency property
		public static readonly DependencyProperty ItemContentProperty =
			DependencyProperty.Register("ItemContent", typeof(object), typeof(BubbleNormal),
			null);

		public object ItemContent
		{
			set { SetValue(ItemContentProperty, value); }
			get { return GetValue(ItemContentProperty); }
		}
		#endregion

		// The HideOpen dependency property
		#region The HideOpen dependency property
		public static readonly DependencyProperty HideOpenProperty =
			DependencyProperty.Register("HideOpen", typeof(bool), typeof(BubbleNormal),
			null);

		public bool HideOpen
		{
			get { return (bool)GetValue(HideOpenProperty); }
			set { SetValue(HideOpenProperty, value); }
		}
		#endregion


        // The External Link dependency property
        #region The External Link dependency property
        public static readonly DependencyProperty ExternalLinkProperty =
            DependencyProperty.Register("ExternalLink", typeof(string), typeof(BubbleNormal),
            null);

        public string ExternalLink
        {
            get { return (string)GetValue(ExternalLinkProperty); }
            set { SetValue(ExternalLinkProperty, value); }
        }
        #endregion

		public BubbleNormal()
		{
			InitializeComponent();
			base.Loaded += OnLoaded;
			
			//_Animation.SetTarget(this);
			//_Animation.ForwardCompleted += ForwardCompleted;
			//_Animation.ReverseCompleted += ReverseCompleted;

			base.MouseEnter += (sender,e) =>
			{
			    VisualStateManager.GoToState(this, "MouseOver", true);
			};

			base.MouseLeave += (sender,e) =>
			{
			    VisualStateManager.GoToState(this, "Normal", true);
			};
		}
	
		//private void ForwardCompleted(object sender, EventArgs e)
		//{
		//    //if (ShowComplete != null)
		//    //    ShowComplete(this, e);
		//}
	
		private void ReverseCompleted(object sender, EventArgs e)
		{
			base.Visibility = Visibility.Collapsed;

			//if (HideComplete != null)
			//    HideComplete(this, e);
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			x_Title.Text = Title;
			x_Icon.Source = ResourceFile.GetBitmap(IconSourceUri, From.This);

			#region ExternalLink / OpenBubble Processing
			// x_TextContent is used to show text for opening bubble or opening a new window

			//this is common to ExternalLink and OpenBubble
			if (CallToActionText == null)
				x_CallToActionText.Visibility = Visibility.Collapsed;
			else
				x_CallToActionText.Text = CallToActionText;

			if (ExternalLink == null)
			{
				x_CallToActionSP.MouseLeftButtonUp += OnBubbleOpenClick;
				x_Header.MouseLeftButtonUp += OnBubbleOpenClick;
			}
			else
			{
				x_CallToActionSP.MouseLeftButtonUp += LaunchNewWindowClick;
				x_Header.MouseLeftButtonUp += LaunchNewWindowClick;
				//x_CallToAction.Text = CallToAction;
				//disable open image and text at the top so they are forced to use external link
				//x_OpenImage.MouseLeftButtonUp -= OnBubbleOpenClick;
				////x_OpenImage.MouseLeftButtonUp += LaunchNewWindowClick;
				//x_OpenText.MouseLeftButtonUp -= OnBubbleOpenClick;
				////x_OpenText.MouseLeftButtonUp += LaunchNewWindowClick;
				//x_Header.MouseLeftButtonUp -= OnBubbleOpenClick;
				//x_Header.MouseLeftButtonUp += LaunchNewWindowClick;
			}
			#endregion

			if (ItemContent == null)
				x_ItemContent.Visibility = Visibility.Collapsed;
			else
				x_ItemContent.Content = ItemContent;

			if (HideOpen)
			{
				x_Header.Cursor = Cursors.Arrow; 
				x_OpenImage.Visibility = Visibility.Collapsed; 
			}
		}
	
		private void OnBubbleOpenClick(object sender, MouseButtonEventArgs e)
		{
			if (HideOpen)
				return;
			if (Click != null)
				Click(this, e);
		}

        private void LaunchNewWindowClick(object sender, MouseButtonEventArgs e)
        {
            HtmlPage.Window.Eval("javascript:window.open('" + ExternalLink + "', '_blank', 'height=' + window.screen.availHeight + ',width=' + window.screen.width + ',top=0,left=0,scrollbars=1');");
        }
	}

	internal class BubbleNormalAnimation : TransformAnimationBase
	{
		public BubbleNormalAnimation()
		{
            DelayForward = 0;
            DelayReverse = 0;
			SecondsForward = 5.05;
            SecondsReverse = 5.05;
            Direction = Direction2D.XY;
			Origin = new Point(.5, .5);
			Scale = new Point(0, 0);
			Offset = new Point(0, 0);
            EasingFunction = null;
		}
	}
}
