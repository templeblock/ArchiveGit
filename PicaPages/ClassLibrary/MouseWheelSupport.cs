/******************************************************************************************
 * 
 * Adam J. Cooper - 2008-09-09
 * 
 * See http://adamjcooper.com/blog/post/Mouse-Wheel-Scrolling-in-Silverlight-2-Beta-2.aspx
 * 
 * Special thanks to Peter Blois and Jeff Prosise whose work this is based on.
 * 
 ******************************************************************************************/
using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Browser;

namespace ClassLibrary
{
	internal class MouseWheelSupport
	{
		private static Point m_currentMousePosition;
		private static BrowserMouseWheelEventListener m_browserListener;
		private static ElementTree m_elementStateTree = new ElementTree();

		private ElementNode m_elementState;

		public event EventHandler<MouseWheelEventArgs> MouseWheelMoved;

		internal static bool UseFocusBehaviorModel { get; set; }

		internal static void RegisterRootVisual(FrameworkElement rootVisual)
		{
			rootVisual.MouseMove += new System.Windows.Input.MouseEventHandler(RootVisualMouseMove);
		}

		static void RootVisualMouseMove(object sender, System.Windows.Input.MouseEventArgs e)
		{
			m_currentMousePosition = e.GetPosition(null);
		}

		public MouseWheelSupport(FrameworkElement elementToAddMouseWheelSupportTo, FrameworkElement parentElementWithMouseWheelSupport)
		{
			//Make sure the browser listener is setup
			if (m_browserListener == null)
				m_browserListener = new BrowserMouseWheelEventListener();

			//Add an event handler to the browser listener for this particular Silverlight element
			m_browserListener.Moved += HandleBrowserMouseWheelMoved;

			//Setup the focus/blur handlers for the Silverlight element
			elementToAddMouseWheelSupportTo.GotFocus += new RoutedEventHandler(ElementGotFocus);
			elementToAddMouseWheelSupportTo.LostFocus += new RoutedEventHandler(ElementLostFocus);

			//Setup mouse move for the Silverlight element
			elementToAddMouseWheelSupportTo.MouseMove += new System.Windows.Input.MouseEventHandler(ElementMouseMove);

			//Add a new node to our tree and save a reference to the node for this element
			m_elementState = m_elementStateTree.Add(elementToAddMouseWheelSupportTo, parentElementWithMouseWheelSupport);
		}

		private void HandleBrowserMouseWheelMoved(object sender, MouseWheelEventArgs e)
		{
			// Only fire the mouse wheel moved event if this is the top-most scrolling element in the UI tree.
			if (IsActiveFrameworkElement())
				MouseWheelMoved(this, e);
		}

		private bool IsActiveFrameworkElement()
		{
			// Use focus behavior model (mouse wheel affects the focused element with the highest z-index)
			if (UseFocusBehaviorModel)
				return m_elementState.HasFocus && m_elementState.IsFocusedLeaf;

			// Use hover behavior model (mouse wheel affects the hovered element with the highest z-index)
			return m_elementState.IsHoverLeaf(m_currentMousePosition);
		}

		private void ElementLostFocus(object sender, RoutedEventArgs e)
		{
			m_elementState.HasFocus = false;
		}

		private void ElementGotFocus(object sender, RoutedEventArgs e)
		{
			m_elementState.HasFocus = true;
		}

		private void ElementMouseMove(object sender, System.Windows.Input.MouseEventArgs e)
		{
			m_elementState.MousePosition = e.GetPosition(null);
		}

		private class BrowserMouseWheelEventListener
		{
			public event EventHandler<MouseWheelEventArgs> Moved;

			public BrowserMouseWheelEventListener()
			{
				if (HtmlPage.IsEnabled)
				{
					HtmlPage.Window.AttachEvent("DOMMouseScroll", HandleMouseWheel);
					HtmlPage.Window.AttachEvent("onmousewheel", HandleMouseWheel);
					HtmlPage.Document.AttachEvent("onmousewheel", HandleMouseWheel);
				}
			}

			private void HandleMouseWheel(object sender, HtmlEventArgs args)
			{
				double delta = 0;

				ScriptObject eventObj = args.EventObject;

				if (eventObj.GetProperty("wheelDelta") != null)
				{
					delta = ((double)eventObj.GetProperty("wheelDelta")) / 120;
					if (HtmlPage.Window.GetProperty("opera") != null)
						delta = -delta;
				}
				else
				if (eventObj.GetProperty("detail") != null)
				{
					delta = -((double)eventObj.GetProperty("detail")) / 3;
					if (HtmlPage.BrowserInformation.UserAgent.IndexOf("Macintosh") != -1)
						delta = delta * 3;
				}

				if (delta != 0 && Moved != null)
				{
					MouseWheelEventArgs wheelArgs = new MouseWheelEventArgs(delta);
					Moved(this, wheelArgs);

					if (wheelArgs.BrowserEventHandled)
						args.PreventDefault();
				}
			}
		}

		#region ElementTree

		private class ElementTree
		{
			private static IList<ElementNode> m_nodes = new List<ElementNode>();

			public ElementNode Add(FrameworkElement element)
			{
				return Add(element, null);
			}

			public ElementNode Add(FrameworkElement element, FrameworkElement parentElement)
			{
				ElementNode newNode = new ElementNode(element);

				if (parentElement == null)
					m_nodes.Add(newNode);
				else
					FindNodeContaining(parentElement).Children.Add(newNode);

				return newNode;
			}

			public ElementNode FindNodeContaining(FrameworkElement element)
			{
				foreach (ElementNode root in m_nodes)
				{
					ElementNode node = root.FindNodeContaining(element);
					if (node != null)
						return node;
				}

				return null;
			}
		}

		#endregion

		#region ElementNode

		private class ElementNode
		{
			private readonly FrameworkElement m_element;
			private IList<ElementNode> m_children = new List<ElementNode>();

			public ElementNode(FrameworkElement element)
			{
				m_element = element;
			}

			public FrameworkElement Element
			{
				get { return m_element; }
			}

			public bool HasFocus { get; set; }
			public bool IsMouseOver { get; set; }
			public Point MousePosition { get; set; }

			public IList<ElementNode> Children
			{
				get { return m_children; }
			}

			public bool IsFocusedLeaf
			{
				get
				{
					if (!HasFocus)
						return false;

					bool childHasFocus = false;
					foreach (ElementNode child in Children)
					{
						if (child.HasFocus)
							childHasFocus = true;
					}

					return !childHasFocus;
				}
			}

			public bool IsHoverLeaf(Point layoutRootMousePosition)
			{
				//If the mouse position of the layout root is different than the mouse position
				//assigned to this node, then we know that the mouse is no longer moving
				//over this node and it cannot be the current the leaf the user is hovering over.
				if (MousePosition != layoutRootMousePosition)
					return false;
				//But if the mouse position of this node and the layout root are the same,
				//then we know the mouse is hovering over this node. The only question is
				//whether or not the mouse is also hovering over any children of this node.

				//If the mouse is hovering over any of this nodes children, then
				//this node is not the leaf node the user is hovering over, so
				//we return false.
				foreach (ElementNode child in Children)
				{
					if (child.MousePosition == m_currentMousePosition)
						return false;
				}

				//If the mouse position is the same as the layout root and 
				//none of the children share the same mouse position then we've found the leaf
				//the user is hovering over.
				return true;
			}

			public ElementNode FindNodeContaining(FrameworkElement element)
			{
				if (Element == element)
					return this;
				if (Children.Count == 0)
					return null;

				foreach (ElementNode child in Children)
				{
					ElementNode node = child.FindNodeContaining(element);
					if (node != null)
						return node;
				}

				return null;
			}
		}

		#endregion
	}

	// Code ported by Pete Blois from Javascript version at http://adomas.org/javascript-mouse-wheel/
	public class MouseWheelEventArgs : EventArgs
	{
		private double m_wheelDelta;
		private bool m_browserEventHandled = false;

		public MouseWheelEventArgs(double delta)
		{
			m_wheelDelta = delta;
		}

		public double WheelDelta
		{
			get { return m_wheelDelta; }
		}

		public bool BrowserEventHandled
		{
			get { return m_browserEventHandled; }
			set { m_browserEventHandled = value; }
		}
	}
}
