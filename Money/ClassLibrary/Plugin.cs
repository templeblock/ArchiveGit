using System;
using System.Windows;
using System.Windows.Browser;
using System.Windows.Interop;

namespace ClassLibrary
{
	public class Plugin
	{
		private static HtmlElement _Plugin = HtmlPage.Plugin;
		private static HtmlElement _PluginParent = HtmlPage.Plugin.Parent;

		public static double Width { get { return _Plugin.GetElementDimension("width"); } set { _Plugin.SetElementDimension("width", value); } }
		public static double ParentWidth { get { return _PluginParent.GetElementDimension("width"); } set { _PluginParent.SetElementDimension("width", value); } }
		public static double Height { get { return _Plugin.GetElementDimension("height"); } set { _Plugin.SetElementDimension("height", value); } }
		public static double ParentHeight { get { return _PluginParent.GetElementDimension("height"); } set { _PluginParent.SetElementDimension("height", value); } }
	}
}
