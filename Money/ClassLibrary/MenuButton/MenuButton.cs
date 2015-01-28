using System;
using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace ClassLibrary
{
	[TemplatePart(Name = SplitElementName, Type = typeof(UIElement))]
	[TemplatePart(Name = ArrowElementName, Type = typeof(UIElement))]
	public class MenuButton : Button
	{
		private const string SplitElementName = "SplitElement";
		private const string ArrowElementName = "ArrowElement";

		// The IsSplit dependency property
		public static readonly DependencyProperty IsSplitProperty =
			DependencyProperty.Register("IsSplit", typeof(bool), typeof(MenuButton),
			new PropertyMetadata(false));

		public bool IsSplit
		{
			get { return (bool)GetValue(IsSplitProperty); }
			set { SetValue(IsSplitProperty, value); }
		}

		public Collection<object> Items { get { return _items; } }
		private ObservableCollection<object> _items = new ObservableCollection<object>();

		private UIElement _arrowElement;
		private ContextMenu _contextMenu;
		private bool _isMouseOverArrowElement;
		private Point _contextMenuInitialOffset;

		public MenuButton()
		{
			base.DefaultStyleKey = base.GetType();
		}

		public override void OnApplyTemplate()
		{
			// Unhook existing handlers
			if (_arrowElement != null)
			{
				if (IsSplit)
				{
					_arrowElement.MouseEnter -= OnArrowElementMouseEnter;
					_arrowElement.MouseLeave -= OnArrowElementMouseLeave;
				}

				_arrowElement = null;
			}
			if (_contextMenu != null)
			{
				_contextMenu.Opened -= OnContextMenuOpened;
				_contextMenu.Closed -= OnContextMenuClosed;
				_contextMenu = null;
			}

			// Apply new template
			base.OnApplyTemplate();

			// Hide the split element (separator) if not a split button
			if (!IsSplit)
			{
				UIElement split = GetTemplateChild(SplitElementName) as UIElement;
				if (split != null)
					split.Visibility = Visibility.Collapsed;
			}

			// Hook new event handlers
			_arrowElement = GetTemplateChild(ArrowElementName) as UIElement;
			if (_arrowElement != null)
			{
				if (IsSplit)
				{
					_arrowElement.MouseEnter += OnArrowElementMouseEnter;
					_arrowElement.MouseLeave += OnArrowElementMouseLeave;
				}

				_contextMenu = ContextMenuService.GetContextMenu(_arrowElement);
				if (_contextMenu != null)
				{
					_contextMenu.Opened += OnContextMenuOpened;
					_contextMenu.Closed += OnContextMenuClosed;
				}
			}
		}

		protected override void OnClick()
		{
			if (!IsSplit || _isMouseOverArrowElement)
				OpenButtonMenu();
			else
				base.OnClick();
		}

		protected override void OnKeyDown(KeyEventArgs e)
		{
			if (e.Key == Key.Down || e.Key == Key.Up)
				OpenButtonMenu();
			else
				base.OnKeyDown(e);
		}

		private void OpenButtonMenu()
		{
			if (_items.Count > 0 && _contextMenu != null)
			{
				_contextMenu.HorizontalOffset = 0;
				_contextMenu.VerticalOffset = 0;
				_contextMenu.IsOpen = true;
			}
		}

		private void OnArrowElementMouseEnter(object sender, MouseEventArgs e)
		{
			_isMouseOverArrowElement = true;
		}

		private void OnArrowElementMouseLeave(object sender, MouseEventArgs e)
		{
			_isMouseOverArrowElement = false;
		}

		private void OnContextMenuOpened(object sender, RoutedEventArgs e)
		{
			_contextMenuInitialOffset = _contextMenu.TransformToVisual(null).Transform(new Point());
			UpdateContextMenuOffsets();

			// Hook LayoutUpdated to handle application resize and zoom changes
			LayoutUpdated += OnLayoutUpdated;
		}

		private void OnContextMenuClosed(object sender, RoutedEventArgs e)
		{
			LayoutUpdated -= OnLayoutUpdated;

			Focus();
		}

		private void OnLayoutUpdated(object sender, EventArgs e)
		{
			UpdateContextMenuOffsets();
		}
		
		private void UpdateContextMenuOffsets()
		{
			// Calculate desired offset to put the ContextMenu below and left-aligned to the Button
			Point desiredOffset = TransformToVisual(Application.Current.RootVisual).Transform(new Point(0, ActualHeight));

			_contextMenu.HorizontalOffset = desiredOffset.X - _contextMenuInitialOffset.X;
			_contextMenu.VerticalOffset = desiredOffset.Y - _contextMenuInitialOffset.Y;

			// Adjust for RTL
			if (FlowDirection.RightToLeft == FlowDirection)
			{
				_contextMenu.UpdateLayout();
				_contextMenu.HorizontalOffset -= _contextMenu.ActualWidth;
			}
		}
	}
}
