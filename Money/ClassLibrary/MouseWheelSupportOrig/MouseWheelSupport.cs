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

namespace Cooper.Silverlight.Controls
{
    internal class MouseWheelSupport
    {
        private static Point currentMousePosition;
        private static BrowserMouseWheelEventListener browserListener;
        private static ElementTree elementStateTree = new ElementTree();
        
        private ElementNode elementState;

        public event EventHandler<MouseWheelEventArgs> MouseWheelMoved;

        internal static bool UseFocusBehaviorModel { get; set; }

        internal static void RegisterRootVisual(FrameworkElement rootVisual)
        {
            rootVisual.MouseMove += new System.Windows.Input.MouseEventHandler(RootVisualMouseMove);
        }

        static void RootVisualMouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
            currentMousePosition = e.GetPosition(null);
        }

        public MouseWheelSupport(FrameworkElement elementToAddMouseWheelSupportTo, FrameworkElement parentElementWithMouseWheelSupport)
        {
            //Make sure the browser listener is setup
            if (browserListener == null)
                browserListener = new BrowserMouseWheelEventListener();

            //Add an event handler to the browser listener for this particular Silverlight element
            browserListener.Moved += this.HandleBrowserMouseWheelMoved;
            
            //Setup the focus/blur handlers for the Silverlight element
            elementToAddMouseWheelSupportTo.GotFocus += new RoutedEventHandler(ElementGotFocus);
            elementToAddMouseWheelSupportTo.LostFocus += new RoutedEventHandler(ElementLostFocus);

            //Setup mouse move for the Silverlight element
            elementToAddMouseWheelSupportTo.MouseMove += new System.Windows.Input.MouseEventHandler(ElementMouseMove);
            
            //Add a new node to our tree and save a reference to the node for this element
            elementState = elementStateTree.Add(elementToAddMouseWheelSupportTo, parentElementWithMouseWheelSupport);
        }

        private void HandleBrowserMouseWheelMoved(object sender, MouseWheelEventArgs e)
        {
            //Only fire the mouse wheel moved event if this is the top-most
            //scrolling element in the UI tree.
            if (IsActiveScrollViewer())
                this.MouseWheelMoved(this, e);
        }

        bool IsActiveScrollViewer()
        {
            //Use focus behavior model (mouse wheel affects the focused element with the highest z-index)
            if (UseFocusBehaviorModel)
                return elementState.HasFocus && elementState.IsFocusedLeaf;
            //Use hover behavior model (mouse wheel affects the hovered element with the highest z-index)
            else
                return elementState.IsHoverLeaf(currentMousePosition);
        }

        void ElementLostFocus(object sender, RoutedEventArgs e)
        {
            elementState.HasFocus = false;
        }

        void ElementGotFocus(object sender, RoutedEventArgs e)
        {
            elementState.HasFocus = true;
        }

        void ElementMouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
            elementState.MousePosition = e.GetPosition(null);
        }
        
        private class BrowserMouseWheelEventListener
        {
            public event EventHandler<MouseWheelEventArgs> Moved;

            public BrowserMouseWheelEventListener()
            {

                if (HtmlPage.IsEnabled)
                {
                    HtmlPage.Window.AttachEvent("DOMMouseScroll", this.HandleMouseWheel);
                    HtmlPage.Window.AttachEvent("onmousewheel", this.HandleMouseWheel);
                    HtmlPage.Document.AttachEvent("onmousewheel", this.HandleMouseWheel);
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
                else if (eventObj.GetProperty("detail") != null)
                {
                    delta = -((double)eventObj.GetProperty("detail")) / 3;

                    if (HtmlPage.BrowserInformation.UserAgent.IndexOf("Macintosh") != -1)
                        delta = delta * 3;
                }

                if (delta != 0 && this.Moved != null)
                {
                    MouseWheelEventArgs wheelArgs = new MouseWheelEventArgs(delta);
                    this.Moved(this, wheelArgs);

                    if (wheelArgs.BrowserEventHandled)
                        args.PreventDefault();
                }
            }
        }

        #region ElementTree

        protected class ElementTree
        {
            private static IList<ElementNode> nodes = new List<ElementNode>();

            public ElementNode Add(FrameworkElement element)
            {
                return Add(element, null);
            }

            public ElementNode Add(FrameworkElement element, FrameworkElement parentElement)
            {
                var newNode = new ElementNode(element);

                if (parentElement == null)
                    nodes.Add(newNode);
                else
                    FindNodeContaining(parentElement).Children.Add(newNode);

                return newNode;
            }

            public ElementNode FindNodeContaining(FrameworkElement element)
            {
                foreach (var root in nodes)
                {
                    var node = root.FindNodeContaining(element);

                    if (node != null)
                        return node;
                }

                return null;
            }
        }

        #endregion

        #region ElementNode

        protected class ElementNode
        {
            private readonly FrameworkElement _element;
            private IList<ElementNode> _children = new List<ElementNode>();

            public ElementNode(FrameworkElement element)
            {
                _element = element;
            }

            public FrameworkElement Element
            {
                get { return _element; }
            }

            public bool HasFocus { get; set; }
            public bool IsMouseOver { get; set; }
            public Point MousePosition { get; set; }

            public IList<ElementNode> Children
            {
                get { return _children; }
            }

            public bool IsFocusedLeaf
            {
                get
                {
                    if (!HasFocus)
                        return false;
                    
                    bool childHasFocus = false;

                    foreach (var child in Children)
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
                else
                {
                    //If the mouse is hovering over any of this nodes children, then
                    //this node is not the leaf node the user is hovering over, so
                    //we return false.
                    foreach (var child in Children)
                    {
                        if (child.MousePosition == currentMousePosition)
                            return false;
                    }
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
                else if (Children.Count == 0)
                    return null;
                else
                {
                    foreach (var child in Children)
                    {
                        var node = child.FindNodeContaining(element);

                        if (node != null)
                            return node;
                    }

                    return null;
                }
            }
        }

        #endregion
    }

    // Code ported by Pete blois from Javascript version at http://adomas.org/javascript-mouse-wheel/
    public class MouseWheelEventArgs : EventArgs
    {
        private double wheelDelta;
        private bool browserEventHandled = false;

        public MouseWheelEventArgs(double delta)
        {
            this.wheelDelta = delta;
        }

        public double WheelDelta
        {
            get { return this.wheelDelta; }
        }

        public bool BrowserEventHandled
        {
            get { return this.browserEventHandled; }
            set { this.browserEventHandled = value; }
        }
    }
}
