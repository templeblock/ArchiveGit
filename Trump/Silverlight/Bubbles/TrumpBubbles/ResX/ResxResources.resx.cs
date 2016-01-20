using System;

namespace TrumpBubbles
{
	// So we can access the resources from C#, as in:
	// string text = ResxResources.AppName;
	public class ResxResources : Resx.ResxResources
	{
		public static Resx.ResxResources _Resources = new Resx.ResxResources();

		// So we can access the resources from XAML, as in:
		//<ResourceDictionary ... xmlns:app="clr-namespace:TrumpITReporting">
		//	<app:ResxResources x:Key="ResxResources"/>
		//</ResourceDictionary
		// <TextBlock Text="{Binding Resource.AppName, Source={StaticResource ResxResources}}"
		public static Resx.ResxResources Resource
		{
			get { return _Resources; }
		}
	}
}
