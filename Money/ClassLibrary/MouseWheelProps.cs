using System;
using System.Windows;
using System.Windows.Automation.Peers;
using System.Windows.Automation.Provider;
using System.Windows.Controls;
using System.Windows.Input;

namespace ClassLibrary
{
	public static class MouseWheelProps
	{
		// The Enabled attached property
		public static readonly DependencyProperty EnabledProperty = 
			DependencyProperty.RegisterAttached("Enabled", typeof(bool), typeof(MouseWheelProps),
				new PropertyMetadata(false, OnEnabledPropertyChanged));

		public static bool GetEnabled(DependencyObject obj)
		{
			return (bool)obj.GetValue(EnabledProperty);
		}

		public static void SetEnabled(DependencyObject obj, bool value)
		{
			obj.SetValue(EnabledProperty, value);
		}

		private static void OnEnabledPropertyChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
		{
			UIElement element = sender as UIElement;
			if (element == null)
				return;
			bool enabled = (bool)e.NewValue;
			if (enabled)
				element.MouseWheel += OnMouseWheel;
			else
				element.MouseWheel -= OnMouseWheel;
		}

		private static void OnMouseWheel(object sender, MouseWheelEventArgs e)
		{
			if (e.Handled)
				return;

			UIElement element = sender as UIElement;
			if (element == null)
				return;

			AutomationPeer peer = FrameworkElementAutomationPeer.FromElement(element);
			if (peer == null)
				peer = FrameworkElementAutomationPeer.CreatePeerForElement(element);
			if (peer == null)
				return;

			// try to get the scroll provider or the range provider
			IScrollProvider m_ScrollProvider = peer.GetPattern(PatternInterface.Scroll) as IScrollProvider;
			IRangeValueProvider m_RangeValueProvider = null;
			if ((element is Control) && (element as Control).HasFocus())
				m_RangeValueProvider = peer.GetPattern(PatternInterface.RangeValue) as IRangeValueProvider;
			if (m_ScrollProvider == null && m_RangeValueProvider == null)
				return;

			// set scroll amount
			const double kMultiplier = 5.0; // x times the default
			const double kFactor = kMultiplier / (30 * 120);
			double delta = e.Delta;
			delta *= kFactor;
			int direction = Math.Sign(delta);

			bool shiftKey = (Keyboard.Modifiers & ModifierKeys.Shift) != 0;
			bool controlKey = (Keyboard.Modifiers & ModifierKeys.Control) != 0;

			if (m_ScrollProvider != null)
			{
				e.Handled = true;
#if true
				if (m_ScrollProvider.VerticallyScrollable && !controlKey)
				{
					double percent = m_ScrollProvider.VerticalScrollPercent - (delta * m_ScrollProvider.VerticalViewSize);
					if (percent < 0) percent = 0;
					if (percent > 100) percent = 100;
					m_ScrollProvider.SetScrollPercent(m_ScrollProvider.HorizontalScrollPercent, percent);
				}
				else
				if (m_ScrollProvider.HorizontallyScrollable && controlKey)
				{
					double percent = m_ScrollProvider.HorizontalScrollPercent - (delta * m_ScrollProvider.HorizontalViewSize);
					if (percent < 0) percent = 0;
					if (percent > 100) percent = 100;
					m_ScrollProvider.SetScrollPercent(percent, m_ScrollProvider.VerticalScrollPercent);
				}
#else
				ScrollAmount scrollAmount = (direction < 0) ? ScrollAmount.SmallIncrement : ScrollAmount.SmallDecrement;
				if (m_ScrollProvider.VerticallyScrollable && !controlKey)
					m_ScrollProvider.Scroll(ScrollAmount.NoAmount, scrollAmount);
				else
				if (m_ScrollProvider.HorizontallyScrollable && controlKey)
					m_ScrollProvider.Scroll(scrollAmount, ScrollAmount.NoAmount);
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
