using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace ClassLibrary
{
	public class TriSelector : GridEx
	{
		public FrameworkElement ContentA { get { return m_ContentA; } set { m_ContentA = SetContent(value); } }
		private FrameworkElement m_ContentA;

		public FrameworkElement ContentB { get { return m_ContentB; } set { m_ContentB = SetContent(value); } }
		private FrameworkElement m_ContentB;

		public FrameworkElement ContentC { get { return m_ContentC; } set { m_ContentC = SetContent(value); } }
		private FrameworkElement m_ContentC;

		// The Value dependency property
		public static readonly DependencyProperty ValueProperty =
			DependencyProperty.Register("Value", typeof(string), typeof(TriSelector),
			new PropertyMetadata((d, e) => ((TriSelector)d).OnValuePropertyChanged((string)e.OldValue, (string)e.NewValue)));

		public string Value
		{
			get { return (string)GetValue(ValueProperty); }
			set { SetValue(ValueProperty, value); }
		}

		private void OnValuePropertyChanged(string oldValue, string newValue)
		{
			if (oldValue != newValue)
				SelectContent();
		}

		public TriSelector()
		{
			base.Loaded += OnLoaded;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			SelectContent();
		}

		private FrameworkElement SetContent(FrameworkElement element)
		{
			element.SetVisible(false);
			if (!base.Children.Contains(element))
				base.Children.Add(element);
			return element;
		}

		private void SelectContent()
		{
			FrameworkElement inactiveElement = (m_ContentA.IsVisible() ? m_ContentA : (m_ContentB.IsVisible() ? m_ContentB : (m_ContentC.IsVisible() ? m_ContentC : null)));
			FrameworkElement activeElement = (Value == "A" ? m_ContentA : (Value == "B" ? m_ContentB : (Value == "C" ? m_ContentC : null)));
			if (inactiveElement == activeElement) inactiveElement = null;
			if (activeElement != null)
			{
				activeElement.SetVisible(true);
				bool activeTakesFocus = (inactiveElement != null && inactiveElement.HasFocus());
				if (activeTakesFocus)
					activeElement.TakeFocus();
			}

			if (inactiveElement != null)
			{
				inactiveElement.ReleaseMouseCapture();
				inactiveElement.SetVisible(false);
			}
		}
	}

	public class Selector : GridEx
	{
		public FrameworkElement WhenTrue { get { return m_WhenTrue; } set { m_WhenTrue = SetContent(value); } }
		private FrameworkElement m_WhenTrue;

		public FrameworkElement WhenFalse { get { return m_WhenFalse; } set { m_WhenFalse = SetContent(value); } }
		private FrameworkElement m_WhenFalse;

		// The Condition dependency property
		public static readonly DependencyProperty ConditionProperty =
			DependencyProperty.Register("Condition", typeof(bool), typeof(Selector),
			new PropertyMetadata((d, e) => ((Selector)d).OnConditionPropertyChanged((bool)e.OldValue, (bool)e.NewValue)));

		public bool Condition
		{
			get { return (bool)GetValue(ConditionProperty); }
			set { SetValue(ConditionProperty, value); }
		}

		private void OnConditionPropertyChanged(bool oldValue, bool newValue)
		{
			if (oldValue != newValue)
				SelectContent();
		}

		public Selector()
		{
			base.Loaded += OnLoaded;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			SelectContent();
		}

		private FrameworkElement SetContent(FrameworkElement element)
		{
			element.SetVisible(false);
			if (!base.Children.Contains(element))
				base.Children.Add(element);
			return element;
		}

		private void SelectContent()
		{
			FrameworkElement inactiveElement = (m_WhenTrue.IsVisible() ? m_WhenTrue : (m_WhenFalse.IsVisible() ? m_WhenFalse : null));
			FrameworkElement activeElement = (Condition ? m_WhenTrue : m_WhenFalse);
			if (inactiveElement == activeElement) inactiveElement = null;
			if (activeElement != null)
			{
				activeElement.SetVisible(true);
				bool activeTakesFocus = (inactiveElement != null && inactiveElement.HasFocus());
				if (activeTakesFocus)
					activeElement.TakeFocus();
			}

			if (inactiveElement != null)
			{
				inactiveElement.ReleaseMouseCapture();
				inactiveElement.SetVisible(false);
			}
		}
	}

	public class SelectorOnFocus : Selector
	{
		public SelectorOnFocus()
		{
			base.Loaded += OnLoaded;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			base.LostFocus += OnLostFocus;
			base.GotFocus += OnGotFocus;
		}

		private void OnGotFocus(object sender, RoutedEventArgs e)
		{
			if (Condition)
				return;

			DependencyObject elementOld = e.OriginalSource as DependencyObject;
			DependencyObject element = FocusManager.GetFocusedElement() as DependencyObject;
			if (!element.IsDescendentOf(this))
				return;

			Condition = true;
		}

		private void OnLostFocus(object sender, RoutedEventArgs e)
		{
			if (!Condition)
				return;

			DependencyObject elementOld = e.OriginalSource as DependencyObject;
			DependencyObject element = FocusManager.GetFocusedElement() as DependencyObject;
			bool specialCase = (elementOld is ComboBox && element is ComboBoxItem);
			if (specialCase || element.IsDescendentOf(this))
				return;

			Condition = false;
		}
	}
}
