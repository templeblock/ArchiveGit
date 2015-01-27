using System;
using System.Windows;
using System.Windows.Browser;
using System.Windows.Controls;

// Written by: Jim McCurdy (jmccurdy@facetofacesoftware.com)

namespace ClassLibrary
{
	public class ContextMenuBase : ContextMenu
	{
		protected override void OnOpened(RoutedEventArgs e)
		{
			ManageEventHandlers(true/*add*/);
			base.OnOpened(e);
		}

		protected override void OnClosed(RoutedEventArgs e)
		{
			ManageEventHandlers(false/*add*/);
			base.OnClosed(e);
		}

		private void OnCancellationEvent(object sender, EventArgs e)
		{
			base.IsOpen = false;
		}

		private void ManageEventHandlers(bool add)
		{
			ScriptObjectCollection element = HtmlPage.Document.GetElementsByTagName("body");
			HtmlElement body = (element != null && element.Count > 0 ? element[0] as HtmlElement : null);
			if (add)
			{
				if (body != null) body.AttachEvent("mousedown", (EventHandler)OnCancellationEvent);
				Application.Current.RootVisual.MouseLeftButtonDown += OnCancellationEvent;
				Application.Current.Host.Content.Resized += OnCancellationEvent;
			}
			else
			{
				if (body != null) body.DetachEvent("mousedown", (EventHandler)OnCancellationEvent);
				Application.Current.RootVisual.MouseLeftButtonDown -= OnCancellationEvent;
				Application.Current.Host.Content.Resized -= OnCancellationEvent;
			}
		}
	}
}
