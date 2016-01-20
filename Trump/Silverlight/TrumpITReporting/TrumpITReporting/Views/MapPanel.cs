using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using ClassLibrary;
using Microsoft.Maps.MapControl;
using Microsoft.Maps.MapControl.Core;
#if TELERIK_MAPS
using Telerik.Windows.Controls.Map;
#endif

namespace TrumpITReporting
{
	public class OpenStreetMapTileSource : TileSource
	{
		public OpenStreetMapTileSource()
			: base("http://tile.openstreetmap.org/{2}/{0}/{1}.png")
		{
		}

		public override Uri GetUri(int x, int y, int zoomLevel)
		{
			return new Uri(string.Format(base.UriFormat, x, y, zoomLevel));
		}
	}

	public class YahooStreetTileSource : TileSource
	{
		public YahooStreetTileSource()
			: base("http://us.maps2.yimg.com/us.png.maps.yimg.com/png?v=3.52&t=m&x={0}&y={1}&z={2}")
		{
		}

		public override Uri GetUri(int x, int y, int zoomLevel)
		{
			// The math used here was copied from the DeepEarth Project (http://deepearth.codeplex.com) 
			double posY;
			double zoom = 18 - zoomLevel;
			double num4 = Math.Pow(2.0, zoomLevel) / 2.0;

			if (y < num4)
				posY = (num4 - Convert.ToDouble(y)) - 1.0;
			else
				posY = ((Convert.ToDouble(y) + 1) - num4) * -1.0;

			return new Uri(String.Format(this.UriFormat, x, posY, zoom));
		}
	}

	public class YahooAerialTileSource : TileSource
	{
		public YahooAerialTileSource()
			: base("http://us.maps3.yimg.com/aerial.maps.yimg.com/tile?v=1.7&t=a&x={0}&y={1}&z={2}")
		{
		}

		public override Uri GetUri(int x, int y, int zoomLevel)
		{
			// The math used here was copied from the DeepEarth Project (http://deepearth.codeplex.com) 
			double posY;
			double zoom = 18 - zoomLevel;
			double num4 = Math.Pow(2.0, zoomLevel) / 2.0;

			if (y < num4)
				posY = (num4 - Convert.ToDouble(y)) - 1.0;
			else
				posY = ((Convert.ToDouble(y) + 1) - num4) * -1.0;

			return new Uri(String.Format(this.UriFormat, x, posY, zoom));
		}
	}

    public class YahooHybridTileSource : TileSource 
    { 
        public YahooHybridTileSource() 
            : base("http://us.maps3.yimg.com/aerial.maps.yimg.com/png?v=2.2&t=h&x={0}&y={1}&z={2}") 
        { 
        }

        public override Uri GetUri(int x, int y, int zoomLevel) 
        { 
            // The math used here was copied from the DeepEarth Project (http://deepearth.codeplex.com) 
            double posY; 
            double zoom = 18 - zoomLevel; 
            double num4 = Math.Pow(2.0, zoomLevel) / 2.0;

            if (y < num4) 
                posY = (num4 - Convert.ToDouble(y)) - 1.0; 
            else 
                posY = ((Convert.ToDouble(y) + 1) - num4) * -1.0; 

            return new Uri(String.Format(this.UriFormat, x, posY, zoom)); 
        } 
    }

	public class CustomMapMode : AerialMode
	{
		private Range<double> validLatitudeRange;
		private Range<double> validLongitudeRange;

		public CustomMapMode()
		{
			// The latitude value range (From = bottom most latitude, To = top most latitude)
			validLatitudeRange = new Range<double>(24, 50);
			// The longitude value range (From = left most longitude, To = right most longitude)
			validLongitudeRange = new Range<double>(-122, -66);
		}

		// Restricts the map zoom level
		protected override Range<double> GetZoomRange(Location center)
		{
			// The allowable zoom levels - 14 to 21.
			return new Range<double>(4, 21);
		}


		// This method is called when the map view changes on Keyboard and Navigation Bar events.
		public override bool ConstrainView(Location center, ref double zoomLevel, ref double heading, ref double pitch)
		{
			bool isChanged = base.ConstrainView(center, ref zoomLevel, ref heading, ref pitch);

			double newLatitude = center.Latitude;
			double newLongitude = center.Longitude;

			// If the map view is outside the valid longitude range, move the map back within range.
			if (center.Longitude > validLongitudeRange.To)
				newLongitude = validLongitudeRange.To;
			else
			if (center.Longitude < validLongitudeRange.From)
				newLongitude = validLongitudeRange.From;

			// If the map view is outside the valid latitude range, move the map back within range.
			if (center.Latitude > validLatitudeRange.To)
				newLatitude = validLatitudeRange.To;
			else
			if (center.Latitude < validLatitudeRange.From)
				newLatitude = validLatitudeRange.From;

			// The new map view location.
			if (newLatitude != center.Latitude || newLongitude != center.Longitude)
			{
				center.Latitude = newLatitude;
				center.Longitude = newLongitude;
				isChanged = true;
			}

			// The new zoom level.
			Range<double> range = GetZoomRange(center);
			if (zoomLevel > range.To)
			{
				zoomLevel = range.To;
				isChanged = true;
			}
			else
			if (zoomLevel < range.From)
			{
				zoomLevel = range.From;
				isChanged = true;
			}

			return isChanged;
		}
	}

	public class MapPanel : GridEx
	{
		private Map m_Map;
		private MapLayer m_PushpinLayer;
		
		public MapPanel()
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

#if TELERIK_MAPS
			m_Map.Provider = new EmptyProvider();
			m_Map.InitializeCompleted += OnMapInitializeCompleted;
#endif

#if true //j
			// hard-coded latitude, longitude and image size
			AddPushpin(42.658, -71.137);
#endif
			// Set the map view
			Location center = new Location() { Latitude = 39.36830, Longitude = -95.27340 };
			m_Map.MapForeground.Copyright.Attributions.Add(new AttributionInfo("TrumpNetwork.com"));

//			double zoom = 4.0;
//			x_Map.SetView(center, zoom);
//			LocationRect boundingRectangle = new LocationRect(0/*North*/, 0/*West*/, 0/*South*/, 0/*East*/);
//			x_Map.SetView(boundingRectangle);

//			// Sets the map to the custom map mode.
//			x_Map.Mode = new CustomMapMode();
		}

		private void OnMouseClick(object sender, MapMouseEventArgs e)
		{
			Point clickLocation = e.ViewportPoint;
			Location location = m_Map.ViewportPointToLocation(clickLocation);
#if NOTUSED //j
			AddPushpin(location.Latitude, location.Longitude);
#endif
		}

		private void AddPushpin(double latitude, double longitude)
		{
#if NOTUSED //j
			Image image = new Image();
			image.Source = ResourceFile.GetBitmap("../Images/AppIcon064.png", From.This);
			double size = 40;
			image.Width = size;
			image.Height = size;
			image.MouseEnter += new MouseEventHandler(OnMouseEnter);
			image.MouseLeave += new MouseEventHandler(OnMouseLeave);
			m_PushpinLayer.AddChild(image, new Location(latitude, longitude), PositionOrigin.Center);
#else
			Pushpin pushpin = new Pushpin();
			pushpin.MouseEnter += new MouseEventHandler(OnMouseEnter);
			pushpin.MouseLeave += new MouseEventHandler(OnMouseLeave);
			m_PushpinLayer.AddChild(pushpin, new Location(latitude, longitude), PositionOrigin.BottomCenter);
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

#if TELERIK_MAPS
		private void OnMapInitializeCompleted(object sender, EventArgs e)
		{
			// We can set map properties when control is initialized.
			m_MapInformationLayer.Visibility = Visibility.Visible;
		}
#endif
	}
}
