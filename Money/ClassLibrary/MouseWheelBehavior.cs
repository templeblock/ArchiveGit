using System;
using System.Windows.Automation;
using System.Windows.Automation.Peers;
using System.Windows.Automation.Provider;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Interactivity;

namespace ClassLibrary
{
	public class MouseWheelBehavior : Behavior<Control>
	{
		private IScrollProvider m_ScrollProvider;
		private IRangeValueProvider m_RangeValueProvider;

		protected override void OnAttached()
		{
			Control element = base.AssociatedObject;
			AutomationPeer peer = FrameworkElementAutomationPeer.FromElement(element);
			if (peer == null)
				peer = FrameworkElementAutomationPeer.CreatePeerForElement(element);
			if (peer != null)
			{
				// try to get the scroll provider or the range provider
				m_ScrollProvider = peer.GetPattern(PatternInterface.Scroll) as IScrollProvider;
				m_RangeValueProvider = peer.GetPattern(PatternInterface.RangeValue) as IRangeValueProvider;
			}

			if (m_ScrollProvider != null || m_RangeValueProvider != null)
				element.MouseWheel += OnMouseWheel;
			base.OnAttached();
		}

		protected override void OnDetaching()
		{
			Control element = base.AssociatedObject;
			if (m_ScrollProvider != null || m_RangeValueProvider != null)
				element.MouseWheel -= OnMouseWheel;
			base.OnDetaching();
		}

		void OnMouseWheel(object sender, MouseWheelEventArgs e)
		{
			if (e.Handled)
				return;
			if (m_ScrollProvider == null && m_RangeValueProvider == null)
				return;

			Control element = base.AssociatedObject;
			bool hasFocus = element.HasFocus();

			// set scroll amount
			const double kMultiplier = 5.0; // x times the default
			const double kFactor = kMultiplier / (30 * 120);
			double delta = e.Delta;
			delta *= kFactor;
			int direction = Math.Sign(delta);

			bool shiftKey = (Keyboard.Modifiers & ModifierKeys.Shift) == ModifierKeys.Shift;
			bool ctrlKey = (Keyboard.Modifiers & ModifierKeys.Control) == ModifierKeys.Control;

			if (m_ScrollProvider != null)
			{
				e.Handled = true;
#if true
				if (m_ScrollProvider.VerticallyScrollable && !ctrlKey)
				{
					double percent = m_ScrollProvider.VerticalScrollPercent - (delta * m_ScrollProvider.VerticalViewSize);
					if (percent < 0) percent = 0;
					if (percent > 100) percent = 100;
					m_ScrollProvider.SetScrollPercent(m_ScrollProvider.HorizontalScrollPercent, percent);
				}
				else
				if (m_ScrollProvider.HorizontallyScrollable && ctrlKey)
				{
					double percent = m_ScrollProvider.HorizontalScrollPercent - (delta * m_ScrollProvider.HorizontalViewSize);
					if (percent < 0) percent = 0;
					if (percent > 100) percent = 100;
					m_ScrollProvider.SetScrollPercent(percent, m_ScrollProvider.VerticalScrollPercent);
				}
#else
				ScrollAmount scrollAmount = (direction < 0) ? ScrollAmount.SmallIncrement : ScrollAmount.SmallDecrement;
				if (scrollProvider.VerticallyScrollable && !ctrlKey)
				{
					scrollProvider.Scroll(ScrollAmount.NoAmount, scrollAmount);
				}
				else
				if (scrollProvider.HorizontallyScrollable && ctrlKey)
				{
					scrollProvider.Scroll(scrollAmount, ScrollAmount.NoAmount);
				}
#endif
			}

			if (m_RangeValueProvider != null)
			{
				e.Handled = true;
				double newValue = m_RangeValueProvider.Value + (direction < 0 ? -m_RangeValueProvider.LargeChange : m_RangeValueProvider.LargeChange);
				if (newValue >= m_RangeValueProvider.Minimum && newValue <= m_RangeValueProvider.Maximum)
					m_RangeValueProvider.SetValue(newValue);
			}
		}
	}
}
