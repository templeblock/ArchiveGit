/******************************************************************************************
 * 
 * Adam J. Cooper - 2008-09-09
 * 
 * See http://adamjcooper.com/blog/post/Mouse-Wheel-Scrolling-in-Silverlight-2-Beta-2.aspx
 * 
 ******************************************************************************************/
using System.Windows.Controls;
using System.Windows;

namespace ClassLibrary
{
	public enum MouseWheelAssociationMode
	{
		/// <summary>
		/// The mouse wheel will affect a <see cref="ScrollViewer" /> whenever the cursor rolls over it.
		/// </summary>
		OnHover,

		/// <summary>
		/// The mouse wheel will only affect a <see cref="ScrollViewer" /> when it is explicitly assigned focus by a mouse click or a keyboard tab.
		/// </summary>
		OnFocus
	}

	/// <summary>
	/// Utility class for adding mouse wheel scrolling to ScrollViewer controls in Silverlight 2 Beta 2.
	/// See http://adamjcooper.com/blog/post/Mouse-Wheel-Scrolling-in-Silverlight-2-Beta-2.aspx for more details.
	/// </summary>
	public static class ScrollViewerMouseWheelSupport
	{
		private const int DEFAULT_SCROLL_AMOUNT = 30;

		/// <summary>
		/// This should be called exactly one time in the constructor of the application's main XAML
		/// code behind file, immediately after the call to InitializeComponent().
		/// </summary>
		/// <param name="applicationRootVisual">The application root visual. Just give a name to the root element of your application and pass it in here.</param>
		/// <param name="activationMode">Specifies whether the mouse wheel is associated with a <see cref="ScrollViewer" /> by simply hovering over it (OnHover) or if the user must explicitly assign focus to it before the wheel affects it (OnFocus).</param>
		public static void Initialize(FrameworkElement applicationRootVisual, MouseWheelAssociationMode activationMode)
		{
			MouseWheelSupport.RegisterRootVisual(applicationRootVisual);
			MouseWheelSupport.UseFocusBehaviorModel = (activationMode == MouseWheelAssociationMode.OnFocus);
		}

		/// <summary>
		/// Adds mouse wheel support to a <see cref="ScrollViewer"/>.
		/// As long as the <see cref="ScrollViewer"/> has focus, the mouse wheel can be used to scroll up and down.
		/// </summary>
		/// <param name="scrollViewer">The scroll viewer.</param>
		/// <returns>The <see cref="ScrollViewer"/>.</returns>
		public static ScrollViewer AddMouseWheelSupport(this ScrollViewer scrollViewer)
		{
			return AddMouseWheelSupport(null, scrollViewer, DEFAULT_SCROLL_AMOUNT);
		}

		/// <summary>
		/// Adds mouse wheel support to a <see cref="ScrollViewer"/>.
		/// As long as the <see cref="ScrollViewer"/> has focus, the mouse wheel can be used to scroll up and down.
		/// </summary>
		/// <param name="scrollViewer">The scroll viewer.</param>
		/// <param name="scrollAmount">The amount to scroll by when the mouse wheel is moved.</param>
		/// <returns>The <see cref="ScrollViewer"/>.</returns>
		public static ScrollViewer AddMouseWheelSupport(this ScrollViewer scrollViewer, double scrollAmount)
		{
			return AddMouseWheelSupport(null, scrollViewer, scrollAmount);
		}

		/// <summary>
		/// Adds mouse wheel support to a <see cref="ScrollViewer"/>.
		/// As long as the <see cref="ScrollViewer"/> has focus, the mouse wheel can be used to scroll up and down.
		/// </summary>
		/// <param name="parentScrollViewer">The parent <see cref="ScrollViewer"/> which contains another <see cref="ScrollViewer"/> which should have mouse wheel scrolling support.</param>
		/// <param name="childScrollViewer">A child <see cref="ScrollViewer"/> to add mouse wheel scrolling support to.</param>
		/// <returns>The child <see cref="ScrollViewer"/>.</returns>
		public static ScrollViewer AddMouseWheelSupport(this ScrollViewer parentScrollViewer, ScrollViewer childScrollViewer)
		{
			return parentScrollViewer.AddMouseWheelSupport(childScrollViewer, DEFAULT_SCROLL_AMOUNT);
		}

		/// <summary>
		/// Adds mouse wheel support to a <see cref="ScrollViewer"/>.
		/// As long as the <see cref="ScrollViewer"/> has focus, the mouse wheel can be used to scroll up and down.
		/// </summary>
		/// <param name="parentScrollViewer">The parent <see cref="ScrollViewer"/> which contains another <see cref="ScrollViewer" /> which should have mouse wheel scrolling support.</param>
		/// <param name="childScrollViewer">A child <see cref="ScrollViewer"/> to add mouse wheel scrolling support to.</param>
		/// <param name="scrollAmount">The amount to scroll by when the mouse wheel is moved.</param>
		/// <returns>The child <see cref="ScrollViewer"/>.</returns>
		public static ScrollViewer AddMouseWheelSupport(this ScrollViewer parentScrollViewer, ScrollViewer childScrollViewer, double scrollAmount)
		{
			MouseWheelSupport mouseWheelHelper = new MouseWheelSupport(childScrollViewer, parentScrollViewer);

			mouseWheelHelper.MouseWheelMoved += (source, eventArgs) =>
				{
					double delta = eventArgs.WheelDelta;

					delta *= scrollAmount;

					double newOffset = childScrollViewer.VerticalOffset - delta;
					if (newOffset > childScrollViewer.ScrollableHeight)
						newOffset = childScrollViewer.ScrollableHeight;
					else
					if (newOffset < 0)
						newOffset = 0;

					childScrollViewer.ScrollToVerticalOffset(newOffset);

					eventArgs.BrowserEventHandled = true;
				};

			return childScrollViewer;
		}
	}

	/// <summary>
	/// Utility class for adding mouse wheel scrolling to Slider controls in Silverlight 2 Beta 2.
	/// See http://adamjcooper.com/blog/post/Mouse-Wheel-Scrolling-in-Silverlight-2-Beta-2.aspx for more details.
	/// </summary>
	public static class SliderMouseWheelSupport
	{
		private const int DEFAULT_SCROLL_AMOUNT = 30;

		/// <summary>
		/// This should be called exactly one time in the constructor of the application's main XAML
		/// code behind file, immediately after the call to InitializeComponent().
		/// </summary>
		/// <param name="applicationRootVisual">The application root visual. Just give a name to the root element of your application and pass it in here.</param>
		/// <param name="activationMode">Specifies whether the mouse wheel is associated with a <see cref="Slider" /> by simply hovering over it (OnHover) or if the user must explicitly assign focus to it before the wheel affects it (OnFocus).</param>
		public static void Initialize(FrameworkElement applicationRootVisual, MouseWheelAssociationMode activationMode)
		{
			MouseWheelSupport.RegisterRootVisual(applicationRootVisual);
			MouseWheelSupport.UseFocusBehaviorModel = (activationMode == MouseWheelAssociationMode.OnFocus);
		}

		/// <summary>
		/// Adds mouse wheel support to a <see cref="Slider"/>.
		/// As long as the <see cref="Slider"/> has focus, the mouse wheel can be used to scroll up and down.
		/// </summary>
		/// <param name="slider">The slider.</param>
		/// <returns>The <see cref="Slider"/>.</returns>
		public static Slider AddMouseWheelSupport(this Slider slider)
		{
			return AddMouseWheelSupport(slider, DEFAULT_SCROLL_AMOUNT);
		}

		/// <summary>
		/// Adds mouse wheel support to a <see cref="Slider"/>.
		/// As long as the <see cref="Slider"/> has focus, the mouse wheel can be used to scroll up and down.
		/// </summary>
		/// <param name="slider">The slider.</param>
		/// <param name="scrollAmount">The amount to scroll by when the mouse wheel is moved.</param>
		/// <returns>The <see cref="Slider"/>.</returns>
		public static Slider AddMouseWheelSupport(this Slider slider, double scrollAmount)
		{
			MouseWheelSupport mouseWheelHelper = new MouseWheelSupport(slider, null/*parentSlider*/);

			mouseWheelHelper.MouseWheelMoved += (source, eventArgs) =>
				{
					double delta = eventArgs.WheelDelta;

					delta *= scrollAmount;

					double newOffset = slider.Value - delta;
					if (newOffset < slider.Minimum)
						newOffset = slider.Minimum;
					else
					if (newOffset > slider.Maximum)
						newOffset = slider.Maximum;

					slider.Value = newOffset;

					eventArgs.BrowserEventHandled = true;
				};

			return slider;
		}
	}
}
