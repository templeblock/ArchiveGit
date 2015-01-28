using System;

namespace Pivot
{
	// So we can access the resources from C#, as in:
	// string text = ResxResources.AppName;
	public class ResxResources : ResX.ResxResources
	{
		public static ResX.ResxResources m_Resources = new ResX.ResxResources();

		// So we can access the resources from XAML, as in:
		//<ResourceDictionary ... xmlns:app="clr-namespace:Pivot">
		//	<app:ResxResources x:Key="ResxResources"/>
		//</ResourceDictionary
		// <TextBlock Text="{Binding Resource.AppName, Source={StaticResource ResxResources}}"
		public static ResX.ResxResources Resource
		{
			get { return m_Resources; }
		}
	}
}
