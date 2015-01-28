using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using ClassLibrary;
using Microsoft.Maps.MapControl;

namespace Money
{
	public class CommunityMapPanel : ViewPanel
	{
		private Map m_Map;
		private MapLayer m_PushpinLayer;
		
		public CommunityMapPanel()
		{
			base.Loaded += OnLoaded;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			m_Map = this.ChildOfType<Map>();
            m_PushpinLayer = new MapLayer();
            m_Map.Children.Add(m_PushpinLayer);
			m_Map.MouseClick += OnMouseClick;

#if true //j
			// hard-coded latitude, longitude and image size
			AddPushpin(42.658, -71.137);
#endif
		}

		private void OnMouseClick(object sender, MapMouseEventArgs e)
		{
			Point clickLocation = e.ViewportPoint;
			Microsoft.Maps.MapControl.Location location = m_Map.ViewportPointToLocation(clickLocation);

#if NOTUSED //j
			AddPushpin(location.Latitude, location.Longitude);
#endif

#if NOTUSED //j
	       // move map to last location 
			m_Map.View = new MapViewSpecification(location, 3);
#endif
		}

		private void AddPushpin(double latitude, double longitude)
		{
#if NOTUSED //j
			Image image = new Image();
			image.Source = ResourceFile.GetBitmap("Images/Me.png", From.This);
			double size = 40;
			image.Width = size;
			image.Height = size;
//			image.MouseEnter += new MouseEventHandler(OnMouseEnter);
//			image.MouseLeave += new MouseEventHandler(OnMouseLeave);
//			m_PushpinLayer.AddChild(image, new Microsoft.Maps.MapControl.Location(latitude, longitude), PositionOrigin.Center);
#else
//			Pushpin pushpin = this.FindResource("MyPushPin") as Pushpin;
			Pushpin pushpin = new Pushpin();
			pushpin.Template = this.FindResource("MyPushPinTemplate") as ControlTemplate;
			pushpin.MouseEnter += new MouseEventHandler(OnMouseEnter);
			pushpin.MouseLeave += new MouseEventHandler(OnMouseLeave);
			m_PushpinLayer.AddChild(pushpin, new Microsoft.Maps.MapControl.Location(latitude, longitude), PositionOrigin.BottomCenter);
#endif
		}

		private void OnMouseLeave(object sender, MouseEventArgs e)
		{
			// remove the pushpin transform when mouse leaves
			Pushpin pushpin = sender as Pushpin;
			Image image = sender as Image;
			UIElement element = sender as UIElement;
			element.RenderTransform = null;
		}

		private void OnMouseEnter(object sender, MouseEventArgs e)
		{
			Pushpin pushpin = sender as Pushpin;
			Image image = sender as Image;
			UIElement element = sender as UIElement;

			// scaling will shrink (less than 1) or enlarge 
			// (greater than 1) source element
			ScaleTransform st = new ScaleTransform();
			st.ScaleX = 1.4;
			st.ScaleY = 1.4;

			// set center of scaling to center of pushpin
			st.CenterX = (element as FrameworkElement).Height / 2;
			st.CenterY = (element as FrameworkElement).Height / 2;

			element.RenderTransform = st;
		}
	}
}
