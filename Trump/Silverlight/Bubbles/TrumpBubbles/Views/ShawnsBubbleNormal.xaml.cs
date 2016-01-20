using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using ClassLibrary;

namespace TrumpBubbles.Views
{
    public partial class ShawnsBubbleNormal : UserControl
    {
		public event EventHandler ShowComplete;
		public event EventHandler HideComplete;
		public event MouseButtonEventHandler Click;
	
		AnimationBase _Animation = new BubbleNormalAnimation();

		// The Title dependency property
		#region The Title dependency property
		public static readonly DependencyProperty TitleProperty =
			DependencyProperty.Register("Title", typeof(string), typeof(ShawnsBubbleNormal),
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
			DependencyProperty.Register("ContentTag", typeof(string), typeof(ShawnsBubbleNormal),
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
            DependencyProperty.Register("IconSourceUri", typeof(string), typeof(ShawnsBubbleNormal),
			null);

		public string IconSourceUri
		{
			get { return (string)GetValue(IconSourceUriProperty); }
			set { SetValue(IconSourceUriProperty, value); }
		}
		#endregion

		// The TextContent dependency property
		#region The TextContent dependency property
		public static readonly DependencyProperty TextContentProperty =
            DependencyProperty.Register("TextContent", typeof(string), typeof(ShawnsBubbleNormal),
			null);

		public string TextContent
		{
			get { return (string)GetValue(TextContentProperty); }
			set { SetValue(TextContentProperty, value); }
		}
		#endregion

		// The IsShown dependency property
		#region The IsShown dependency property
		public static readonly DependencyProperty IsShownProperty =
            DependencyProperty.Register("IsShown", typeof(bool), typeof(ShawnsBubbleNormal),
			new PropertyMetadata(false, (d, e) => ((ShawnsBubbleNormal)d).OnIsShownPropertyChanged((bool)e.OldValue, (bool)e.NewValue)));

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

		// The ItemContent dependency property
		#region The ItemContent dependency property
		public static readonly DependencyProperty ItemContentProperty =
			DependencyProperty.Register("ItemContent", typeof(object), typeof(ShawnsBubbleNormal),
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
			DependencyProperty.Register("HideOpen", typeof(bool), typeof(ShawnsBubbleNormal),
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
            DependencyProperty.Register("ExternalLink", typeof(string), typeof(ShawnsBubbleNormal),
            null);

        public string ExternalLink
        {
            set { SetValue(ExternalLinkProperty, value); }
            get { return (string)GetValue(ExternalLinkProperty); }
        }
        #endregion

		public ShawnsBubbleNormal()
		{
			InitializeComponent();
			base.Loaded += OnLoaded;
			
			_Animation.SetTarget(this);
			_Animation.ForwardCompleted += ForwardCompleted;
			_Animation.ReverseCompleted += ReverseCompleted;
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
			x_Title.Text = Title;
			x_Icon.Source = ResourceFile.GetBitmap(IconSourceUri, From.This);
			if (TextContent == null)
				x_TextContent.Visibility = Visibility.Collapsed;
			else
				x_TextContent.Text = TextContent;
			if (ItemContent == null)
				x_ItemContent.Visibility = Visibility.Collapsed;
			else
				x_ItemContent.Content = ItemContent;

            if (ExternalLink == null)
                x_ExternalLink.Visibility = Visibility.Collapsed;
            else
                x_ExternalLink.Text = ExternalLink;


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
	}

	internal class BubbleNormalAnimation : TransformAnimationBase
	{
		public BubbleNormalAnimation()
		{
			SecondsForward = 0.75;
			SecondsReverse = 0.75;
			Direction = Direction2D.XY;
			Origin = new Point(0, 0);
			Scale = new Point(0, 0);
			Offset = new Point(0, 0);
			EasingFunction = null;	
		}
	}
}
