using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Browser;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Navigation;
using ClassLibrary;

namespace TrumpBubbles
{
	public class BubblePageBase : Page
	{
		private const int _ExtraControlHeight = 180; // The height of silverlight control, not including the HtmlPlaceholder (iframe)
		private static double _StartupPluginHeight = Plugin.Height; // Must be static
		private double _SavedPluginHeight = 0;

		// The MinimizedBubblesAreShown dependency property
		#region The MinimizedBubblesAreShown dependency property
		public static readonly DependencyProperty MinimizedBubblesAreShownProperty =
			DependencyProperty.Register("MinimizedBubblesAreShown", typeof(bool), typeof(BubblePageBase),
			null);

		public bool MinimizedBubblesAreShown
		{
			get { return (bool)GetValue(MinimizedBubblesAreShownProperty); }
			set { SetValue(MinimizedBubblesAreShownProperty, value); }
		}
		#endregion

		// The NormalBubblesAreShown dependency property
		#region The NormalBubblesAreShown dependency property
		public static readonly DependencyProperty NormalBubblesAreShownProperty =
			DependencyProperty.Register("NormalBubblesAreShown", typeof(bool), typeof(BubblePageBase),
			null);

		public bool NormalBubblesAreShown
		{
			get { return (bool)GetValue(NormalBubblesAreShownProperty); }
			set { SetValue(NormalBubblesAreShownProperty, value); }
		}
		#endregion

		// The MaximizedContent dependency property
		#region The MaximizedContent dependency property
		public static readonly DependencyProperty MaximizedContentProperty =
			DependencyProperty.Register("MaximizedContent", typeof(object), typeof(BubblePageBase),
			null);

		public object MaximizedContent
		{
			get { return (object)GetValue(MaximizedContentProperty); }
			set { SetValue(MaximizedContentProperty, value); }
		}
		#endregion

		public BubblePageBase()
		{
			base.Loaded += OnLoaded;

			base.DataContext = this;
		}

		// Executes when the user navigates to this page.
		protected override void OnNavigatedTo(NavigationEventArgs e)
		{
			Plugin.Height = (_SavedPluginHeight != 0 ? _SavedPluginHeight : _StartupPluginHeight);
		}

		// Executes when the user navigates away from this page.
		protected override void OnNavigatedFrom(NavigationEventArgs e)
		{
			_SavedPluginHeight = Plugin.Height;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			// Set the BubbleMinimized event handlers
			IEnumerable<BubbleMinimized> bubblesMinimized = this.Content.ChildListOfType<BubbleMinimized>();
			foreach (BubbleMinimized bubble in bubblesMinimized)
			{
				//bubble.ShowComplete += OnMinimizedShowComplete;
				//bubble.HideComplete += OnMinimizedHideComplete; 
				bubble.Click += OnMinimizedClick;
			}

			// Set the BubbleNormal event handlers
			IEnumerable<BubbleNormal> bubblesNormal = this.Content.ChildListOfType<BubbleNormal>();
			foreach (BubbleNormal bubble in bubblesNormal)
			{
				//bubble.ShowComplete += OnNormalShowComplete;
				//bubble.HideComplete += OnNormalHideComplete; 
				bubble.Click += OnNormalClick;
			}

			// Set the BubbleMaximized event handlers
			IEnumerable<BubbleMaximized> bubblesMaximized = this.Content.ChildListOfType<BubbleMaximized>();
			foreach (BubbleMaximized bubble in bubblesMaximized)
			{
				//bubble.ShowComplete += OnMaximizedShowComplete;
				bubble.HideComplete += OnMaximizedHideComplete; 
				bubble.Click += OnMaximizedClick;
				//bubble.SourceUrlChanged += OnMaximizedSourceUrlChanged;
				bubble.HtmlHeightChanged += OnMaximizedHtmlHeightChanged;
				bubble.ExtraControlHeight = _ExtraControlHeight;
			}

			// Hide the minimized bubbles
			// Show the normal bubbles

			MinimizedBubblesAreShown = false;
			NormalBubblesAreShown = true;
		}

		//protected virtual void OnMaximizedSourceUrlChanged(object sender, EventArgs e)
		//{
		//    BubbleMaximized bubbleMaximized = sender as BubbleMaximized;
		//}

		protected virtual void OnMaximizedHtmlHeightChanged(object sender, EventArgs e)
		{
			BubbleMaximized bubbleMaximized = sender as BubbleMaximized;

			// Set the plugin size
			if (bubbleMaximized.HtmlPlaceholder.Height > 0)
				Plugin.Height = bubbleMaximized.HtmlPlaceholder.Height + _ExtraControlHeight;
		}

		//protected virtual void OnMinimizedShowComplete(object sender, EventArgs e)
		//{
		//    BubbleMinimized bubbleMinimized = sender as BubbleMinimized;
		//}

		//protected virtual void OnMinimizedHideComplete(object sender, EventArgs e)
		//{
		//    BubbleMinimized bubbleMinimized = sender as BubbleMinimized;
		//}

		//protected virtual void OnNormalShowComplete(object sender, EventArgs e)
		//{
		//    BubbleNormal bubbleNormal = sender as BubbleNormal;
		//}

		//protected virtual void OnNormalHideComplete(object sender, EventArgs e)
		//{
		//    BubbleNormal bubbleNormal = sender as BubbleNormal;
		//}

		//protected virtual void OnMaximizedShowComplete(object sender, EventArgs e)
		//{
		//    BubbleMaximized bubbleMaximized = sender as BubbleMaximized;
		//}

		protected virtual void OnMaximizedHideComplete(object sender, EventArgs e)
		{
			BubbleMaximized bubbleMaximized = sender as BubbleMaximized;

			// If no new maximized bubble is opening
			if (MaximizedContent == null)
			{
				Plugin.Height = _StartupPluginHeight;

				// Hide the minimized bubbles
				// Show the normal bubbles

				MinimizedBubblesAreShown = false;
				NormalBubblesAreShown = true;
			}
		}

		protected virtual void OnMinimizedClick(object sender, MouseButtonEventArgs e)
		{
			BubbleMinimized bubbleMinimized = sender as BubbleMinimized;
			BubbleMaximized bubbleMaximized = FindContent(bubbleMinimized.ContentTag);
			if (bubbleMaximized == null)
				return;

			// Set the plugin size
			if (bubbleMaximized.HtmlPlaceholder.Height > 0)
				Plugin.Height = bubbleMaximized.HtmlPlaceholder.Height + _ExtraControlHeight;

			// Show the minimized bubbles (redundant)
			// Hide the normal bubbles (redundant)
			// Hide the current maximized bubble
			// Show the new maximized bubble

			MinimizedBubblesAreShown = true;
			NormalBubblesAreShown = false;
			MaximizedContent = bubbleMaximized;
		}

		protected virtual void OnNormalClick(object sender, MouseButtonEventArgs e)
		{
			BubbleNormal bubbleNormal = sender as BubbleNormal;
			BubbleMaximized bubbleMaximized = FindContent(bubbleNormal.ContentTag);
			if (bubbleMaximized == null)
				return;

			// Set the plugin size
			if (bubbleMaximized.HtmlPlaceholder.Height > 0)
				Plugin.Height = bubbleMaximized.HtmlPlaceholder.Height + _ExtraControlHeight;

			// Hide the normal bubbles
			// Show the minimized bubbles
			// Show the maximized bubble

			NormalBubblesAreShown = false;
			MinimizedBubblesAreShown = true;
			MaximizedContent = bubbleMaximized;
		}

		protected virtual void OnMaximizedClick(object sender, MouseButtonEventArgs e)
		{
			BubbleMaximized bubbleMaximized = sender as BubbleMaximized;

			// Hide the maximized bubble

			MaximizedContent = null;
		}

		private BubbleMaximized FindContent(string contentTag)
		{
			IEnumerable<BubbleMaximized> bubbles = this.ChildListOfType<BubbleMaximized>();
			if (bubbles == null)
				return null;

			foreach (BubbleMaximized bubbleMaximized in bubbles)
			{
				if (bubbleMaximized.ContentTag == contentTag)
					return bubbleMaximized;
			}

			return null;
		}
	}
}
