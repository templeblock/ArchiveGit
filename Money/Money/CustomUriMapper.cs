using System;
using System.Collections.ObjectModel;
using System.Windows.Markup;
using System.Windows.Navigation;

namespace Money
{
	// In XAML:
	//	<app:CustomUriMapper>
	//		<app:CustomUriMapping Uri="/{search}" MappedUri="/pages/searchpage.xaml?searchfor={search}"/>
	//	</app:CustomUriMapper>

	public class CustomUriMapping
	{
		public Uri Uri { get; set; }
		public Uri MappedUri { get; set; }

		public Uri MapUri(Uri uri)
		{
			// Do the uri mapping without regard to upper or lower case
			UriMapping _uriMapping = new UriMapping() { Uri = (Uri == null || string.IsNullOrEmpty(Uri.OriginalString) ? null : new Uri(Uri.OriginalString.ToLower(), UriKind.RelativeOrAbsolute)), MappedUri = MappedUri };
			return _uriMapping.MapUri(uri == null || string.IsNullOrEmpty(uri.OriginalString) ? null : new Uri(uri.OriginalString.ToLower(), UriKind.RelativeOrAbsolute));
		}
	}

	[ContentProperty("UriMappings")]
	public class CustomUriMapper : UriMapperBase
	{
		public ObservableCollection<CustomUriMapping> UriMappings { get { return m_UriMappings; } private set { m_UriMappings = value; } }
		private ObservableCollection<CustomUriMapping> m_UriMappings = new ObservableCollection<CustomUriMapping>();

		public override Uri MapUri(Uri uri)
		{
			if (m_UriMappings == null)
				return uri;

			foreach (CustomUriMapping mapping in m_UriMappings)
			{
				Uri mappedUri = mapping.MapUri(uri);
				if (mappedUri != null)
					return mappedUri;
			}

			return uri;
		}
	}
}