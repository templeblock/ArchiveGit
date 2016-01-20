using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Media.Imaging;
using Telerik.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

namespace TrumpVideo
{
	public partial class MainPage : UserControl
	{
		public MainPage()
		{
			InitializeComponent();
			base.Loaded += OnLoaded;
			List<VideoDataItem> videoDataItems = new List<VideoDataItem>()
			{
				new VideoDataItem () { 
					ImageSource = new Uri("../Images/VideoPage/AndreaBarone.jpg", UriKind.Relative), 
					Description = "Video 1", Title = "Andrea Barone is a Marketer, Chef – Lost 47lbs, maintained weight since Miami.",
					Link = new Uri("http://www.idealhealthhosting.com/SiloMP4/AndreaBarone.mp4")},
				new VideoDataItem () {
					ImageSource = new Uri("../Images/VideoPage/CarolineRawlinson.jpg", UriKind.Relative), 
					Description = "Video 2", Title = "Caroline Rawlinson is a Marketer – Lost 47 lbs. Maintained weight since Miami.",
					Link = new Uri("http://www.idealhealthhosting.com/SiloMP4/CarolineRawlinson.mp4")},
				new VideoDataItem () {
					ImageSource = new Uri("../Images/VideoPage/ElliotLondon.jpg", UriKind.Relative), 
					Description = "Video 3", Title = "Elliot London is an Insurance Agent, Marketer – Lost 70 lbs.",
					Link = new Uri("http://www.idealhealthhosting.com/SiloMP4/ElliotLondon.mp4")},
				new VideoDataItem () {
					ImageSource = new Uri("../Images/VideoPage/JaniceLove.jpg", UriKind.Relative), 
					Description = "Video 4", Title = "Janice Love is a Customer Service Rep, Marketer –Lost 70 lbs.",
					Link = new Uri("http://www.idealhealthhosting.com/SiloMP4/JaniceLove.mp4")},
				new VideoDataItem () {
					ImageSource = new Uri("../Images/VideoPage/JudyStyborski.jpg", UriKind.Relative), 
					Description = "Video 5", Title = "Judy Styborski is a Marketer - Lost 39 lbs in 2008, maintained weight since.",
					Link = new Uri("http://www.idealhealthhosting.com/SiloMP4/JudyStyborski.mp4")},
				new VideoDataItem () {
					ImageSource = new Uri("../Images/VideoPage/KimHugron.jpg", UriKind.Relative), 
					Description = "Video 6", Title = "Kim Hugron",
					Link = new Uri("http://www.idealhealthhosting.com/SiloMP4/KimHugron.mp4")},
				new VideoDataItem () {
					ImageSource = new Uri("../Images/VideoPage/PeterLentini.jpg", UriKind.Relative), 
					Description = "Video 7", Title = "Peter Lentini",
					Link = new Uri("http://www.idealhealthhosting.com/SiloMP4/PeterLentini.mp4")},
				new VideoDataItem () {
					ImageSource = new Uri("../Images/VideoPage/RoxanneEdwards.jpg", UriKind.Relative), 
					Description = "Video 8", Title = "Roxanne Edwards",
					Link = new Uri("http://www.idealhealthhosting.com/SiloMP4/RoxanneEdwards.mp4")},
				new VideoDataItem () {
					ImageSource = new Uri("../Images/VideoPage/VernonMcGee.jpg", UriKind.Relative), 
					Description = "Video 9", Title = "Vernon McGee owns a Sears outlet Store & Retired from Air Force – Lost 85 lbs.",
					Link = new Uri("http://www.idealhealthhosting.com/SiloMP4/VernonMcGee.mp4")},
				new VideoDataItem () {
					ImageSource = new Uri("../Images/VideoPage/WaltPrice.jpg", UriKind.Relative), 
					Description = "Video 10", Title = "Walt Price",
					Link = new Uri("http://www.idealhealthhosting.com/SiloMP4/WaltPrice.mp4")},
				new VideoDataItem () {
					ImageSource = new Uri("../Images/VideoPage/ZedaSpiegal.jpg", UriKind.Relative), 
					Description = "Video 11", Title = "Zeda Spiegal - Lost 30 lbs weight maintained.",
					Link = new Uri("http://www.idealhealthhosting.com/SiloMP4/ZedaSpiegal.mp4")},
			};

			base.DataContext = videoDataItems;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
		}

		private void OnInfoItemLoaded(object sender, RoutedEventArgs e)
		{
			VideoDataItem videoDataItem = (sender as FrameworkElement).DataContext as VideoDataItem;
			RadCoverFlowItem radCoverFlowItem = (sender as UIElement).ParentOfType<RadCoverFlowItem>();
			FrameworkElement shadowElement = (radCoverFlowItem as FrameworkElement).ChildOfType<Rectangle>();
			videoDataItem.InfoItemElement = (sender as FrameworkElement);
			bool visible = radCoverFlowItem.IsSelected;
			videoDataItem.InfoItemElement.Visibility = (visible ? Visibility.Visible : Visibility.Collapsed);
			shadowElement.Visibility = (visible ? Visibility.Collapsed : Visibility.Visible);
		}

		private void OnCoverFlowSelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
		{
			RadCoverFlow coverFlow = sender as RadCoverFlow;
			foreach (VideoDataItem videoDataItem in e.RemovedItems)
			{
				if (videoDataItem.InfoItemElement == null)
					continue;
				RadCoverFlowItem radCoverFlowItem = (videoDataItem.InfoItemElement as UIElement).ParentOfType<RadCoverFlowItem>();
				FrameworkElement shadowElement = (radCoverFlowItem as FrameworkElement).ChildOfType<Rectangle>();
				videoDataItem.InfoItemElement.Visibility = Visibility.Collapsed;
				shadowElement.Visibility = Visibility.Visible;
			}

			foreach (VideoDataItem videoDataItem in e.AddedItems)
			{
				if (videoDataItem.InfoItemElement == null)
					continue;
				RadCoverFlowItem radCoverFlowItem = (videoDataItem.InfoItemElement as UIElement).ParentOfType<RadCoverFlowItem>();
				FrameworkElement shadowElement = (radCoverFlowItem as FrameworkElement).ChildOfType<Rectangle>();
				videoDataItem.InfoItemElement.Visibility = Visibility.Visible;
				shadowElement.Visibility = Visibility.Collapsed;
			}
		}

		private void OnPlayClick(object sender, RoutedEventArgs e)
		{
			x_MediaPlayerContainer.Visibility = Visibility.Visible;

			VideoDataItem videoDataItem = x_CoverFlow.SelectedItem as VideoDataItem;
			Uri video = new Uri((sender as FrameworkElement).Tag.ToString(), UriKind.RelativeOrAbsolute);
#if RAD
			RadMediaItem item = new RadMediaItem();
			item.Source = videoDataItem.Link;
			item.ImageSource = new BitmapImage(videoDataItem.ImageSource);
			//item.Title = videoDataItem.Title;
			//item.Description = videoDataItem.Description;
			
			RadMediaPlayer player = App.Current.RootVisual.ChildOfType<RadMediaPlayer>();
			player.Items.Clear();
			player.Items.Add(item);
			player.CurrentItem = item;
#else
			VideoControl videoControl= App.Current.RootVisual.ChildOfType<VideoControl>();
			MediaElement player = videoControl.ChildOfType<MediaElement>();
			player.Source = video;
			//player.Title = videoDataItem.Title;
			//player.Description = videoDataItem.Description;
#endif
		}

		private void OnCloseClick(object sender, RoutedEventArgs e)
		{
			if (Application.Current.Host.Content.IsFullScreen)
				Application.Current.Host.Content.IsFullScreen = false;

#if RAD
			RadMediaPlayer player = App.Current.RootVisual.ChildOfType<RadMediaPlayer>();
#else
			VideoControl videoControl = App.Current.RootVisual.ChildOfType<VideoControl>();
			MediaElement player = videoControl.ChildOfType<MediaElement>();
#endif
			if (player != null)
				player.Stop();

			x_MediaPlayerContainer.Visibility = Visibility.Collapsed;
		}
	}

	public class VideoDataItem
	{
		public Uri ImageSource { get; set; }
		public string Title { get; set; }
		public string Description { get; set; }
		public Uri Link { get; set; }
		public FrameworkElement InfoItemElement;
	}

	public class IntToIntValueConverter : IValueConverter
	{
		object IValueConverter.Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			int val;
			int param;
			if ((int.TryParse(value.ToString(), out val) && (int.TryParse(parameter.ToString(), out param))))
				return val + param;
			return value;
		}

		object IValueConverter.ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			if (value is int && parameter is int)
				return (int)value + (int)parameter;
			return DependencyProperty.UnsetValue;
		}
	}

	internal static class DependencyObjectExt
	{
		// Extension for DependencyObject
		// Return the first child of a given type
		internal static TT ChildOfType<TT>(this DependencyObject element) where TT : DependencyObject
		{
			return ChildOfType(element, delegate(TT obj)
			{
				return true;
			});
		}

		// Private Extension for DependencyObject
		// Return the first child of a given type, and passing the Checker delegate
		private static TT ChildOfType<TT>(this DependencyObject element, Predicate<TT> checker) where TT : DependencyObject
		{
			if (element == null)
				return null;

			if (element is ContentControl)
			{
				DependencyObject child = (element as ContentControl).Content as DependencyObject;
				TT childFound = child.ChildOfTypeSearch(checker);
				if (childFound != null)
					return childFound;
			}

			for (int i = 0; i < VisualTreeHelper.GetChildrenCount(element); i++)
			{
				DependencyObject child = VisualTreeHelper.GetChild(element, i);
				TT childFound = child.ChildOfTypeSearch(checker);
				if (childFound != null)
					return childFound;
			}

			return null;
		}

		// Private Extension for DependencyObject
		// Return the first child of a given type, and passing the Checker delegate
		private static TT ChildOfTypeSearch<TT>(this DependencyObject child, Predicate<TT> checker) where TT : DependencyObject
		{
			TT childFound = child as TT;
			if (childFound != null && checker(childFound))
				return childFound;
			childFound = ((DependencyObject)child).ChildOfType(checker);
			if (childFound != null && checker(childFound))
				return childFound;
			return null;
		}

		// Extension for DependencyObject
		// Return a list of all children of a given type
		internal static IList<TT> ChildrenOfType<TT>(this DependencyObject element) where TT : DependencyObject
		{
			List<TT> list = new List<TT>();
			if (element != null)
				element.ChildrenOfType(ref list);

			return list;
		}

		// Extension for DependencyObject
		private static IList<TT> ChildrenOfType<TT>(this DependencyObject element, ref List<TT> list) where TT : DependencyObject
		{
			if (element is ContentControl)
			{
				DependencyObject child = (element as ContentControl).Content as DependencyObject;
				if (child is TT)
					list.Add((TT)child);
				if (child is DependencyObject)
					((DependencyObject)child).ChildrenOfType(ref list);
			}

			for (int i = 0; i < VisualTreeHelper.GetChildrenCount(element); i++)
			{
				DependencyObject child = VisualTreeHelper.GetChild(element, i);
				if (child is TT)
					list.Add((TT)child);
				if (child is DependencyObject)
					((DependencyObject)child).ChildrenOfType(ref list);
			}

			return list;
		}
	}
}
