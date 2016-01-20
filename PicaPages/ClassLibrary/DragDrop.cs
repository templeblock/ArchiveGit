using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Controls.Primitives;
using System.Diagnostics;

namespace ClassLibrary
{
	public enum DropTargetEventType { Enter, Over, Leave, Drop };

	public interface IDropTarget
	{
		void DropTarget(DropTargetEventType type, DragDrop dragDrop);
	}

	public class DragDrop
	{
		private bool m_bCaptured = false;
		private Panel m_LayoutRoot;
		private FrameworkElement m_DragSource;
//j		private Panel m_DragSourceParentPanel;
		private FrameworkElement m_DragSourceClone;
		private object m_DragSourcePayload;
		private Dictionary<FrameworkElement, object> m_DragSourcePayloadList = new Dictionary<FrameworkElement, object>();
		private UIElement m_DropTarget;
		private List<UIElement> m_DropTargetList = new List<UIElement>();
		private ContentControl m_FeedbackContent = new ContentControl();
		private Popup m_FeedbackPopup = new Popup();
		private Brush m_OldDropTargetBackground;
		private MouseEventArgs m_MouseEventArgs;

		public delegate void DropTargetEventHandler(DropTargetEventType type, DragDrop dragSource);
		public event DropTargetEventHandler DropTargetListeners;
		public FrameworkElement Source { get { return m_DragSource; } }
		public object Payload { get { return m_DragSourcePayload; } }
		public UIElement Target { get { return m_DropTarget; } }

		public DragDrop()
		{
		}

		public void Init()
		{
			m_LayoutRoot = ApplicationEx.RootPanel;
		}

		public void AddSource(FrameworkElement dragSource, object payload)
		{
			Debug.Assert(dragSource != null);
			dragSource.MouseLeftButtonDown += DragSource_MouseLeftButtonDown;
			m_DragSourcePayloadList.Add(dragSource, payload);
		}

		public void RemoveSource(FrameworkElement dragSource)
		{
			Debug.Assert(dragSource != null);
			dragSource.MouseLeftButtonDown -= DragSource_MouseLeftButtonDown;
			m_DragSourcePayloadList.Remove(dragSource);
		}

		public void AddTarget(UIElement dropTarget)
		{
			Debug.Assert(dropTarget != null);
			m_DropTargetList.Add(dropTarget);
		}

		public void RemoveTarget(UIElement dropTarget)
		{
			Debug.Assert(dropTarget != null);
			m_DropTargetList.Remove(dropTarget);
		}

		public Point GetPosition(UIElement relativeTo)
		{
			if (m_MouseEventArgs == null)
				return new Point(0, 0);
			return m_MouseEventArgs.GetPosition(relativeTo);
		}

		private void DragSource_MouseLeftButtonDown(object sender, MouseEventArgs args)
		{
			if (m_bCaptured)
				return;

			m_MouseEventArgs = args;
			m_DragSource = sender as FrameworkElement;
//j			m_DragSourceParentPanel = m_DragSource.Parent as Panel;
			m_DragSourceClone = m_DragSource.Clone2<FrameworkElement>();
			m_DragSourcePayload = m_DragSourcePayloadList[m_DragSource];

			// Create the feedback popup
			m_FeedbackContent.Content = m_DragSourceClone;
			m_FeedbackContent.Opacity = 0.5;
			m_FeedbackPopup.IsOpen = false;
			m_FeedbackPopup.Child = m_FeedbackContent;
			m_LayoutRoot.Children.Add(m_FeedbackPopup);
			Point dragPoint = args.GetPosition(null);
			m_FeedbackPopup.HorizontalOffset = dragPoint.X - (m_DragSourceClone.ActualWidth / 2);
			m_FeedbackPopup.VerticalOffset = dragPoint.Y - (m_DragSourceClone.ActualHeight / 2);
			m_FeedbackPopup.MouseLeftButtonUp += FeedbackPopup_MouseLeftButtonUp;
			m_FeedbackPopup.MouseMove += FeedbackPopup_MouseMove;
			m_FeedbackPopup.IsOpen = true;
			m_FeedbackPopup.CaptureMouse();
			m_bCaptured = true;
		}

		private void FeedbackPopup_MouseMove(object sender, MouseEventArgs args)
		{
			if (!m_bCaptured)
				return;

			if (sender != m_FeedbackPopup)
				return;

			m_MouseEventArgs = args;
			Point dragPoint = args.GetPosition(null);
			m_FeedbackPopup.HorizontalOffset = dragPoint.X - (m_DragSourceClone.ActualWidth / 2);
			m_FeedbackPopup.VerticalOffset = dragPoint.Y - (m_DragSourceClone.ActualHeight / 2);

			UIElement dropTarget = FindDropTarget(m_FeedbackPopup, args);
//j			if (dropTarget == null)
//j				dropTarget = m_DragSourceParentPanel;
			if (DropTargetListeners != null)
			{
				if (m_DropTarget != dropTarget)
				{
					DropTargetListeners.Invoke(DropTargetEventType.Leave, this);
					m_DropTarget = dropTarget;
					DropTargetListeners.Invoke(DropTargetEventType.Enter, this);
				}
				else
					DropTargetListeners.Invoke(DropTargetEventType.Over, this);
			}
		}

		private void FeedbackPopup_MouseLeftButtonUp(object sender, MouseEventArgs args)
		{
			if (!m_bCaptured)
				return;

			if (sender != m_FeedbackPopup)
				return;

			m_bCaptured = false;
			m_MouseEventArgs = args;

			// Get rid of the feedback popup
			m_FeedbackPopup.ReleaseMouseCapture();
			m_FeedbackPopup.MouseLeftButtonUp -= FeedbackPopup_MouseLeftButtonUp;
			m_FeedbackPopup.MouseMove -= FeedbackPopup_MouseMove;
			m_FeedbackPopup.IsOpen = false;
			m_LayoutRoot.Children.Remove(m_FeedbackPopup);
			m_FeedbackContent.Content = null;
			m_FeedbackPopup.Child = null;

			m_DropTarget = FindDropTarget(m_FeedbackPopup, args);
//j			if (dropTarget == null)
//j				dropTarget = m_DragSourceParentPanel;
			if (DropTargetListeners != null)
			{
				DropTargetListeners.Invoke(DropTargetEventType.Leave, this);
				DropTargetListeners.Invoke(DropTargetEventType.Drop, this);
			}

			m_DragSourceClone = null;
			m_DragSource = null;
//j			m_DragSourceParentPanel = null;
			m_DragSourcePayload = null;
			m_DropTarget = null;
		}

		private UIElement FindDropTarget(FrameworkElement dragSource, MouseEventArgs args)
		{
			Point position = args.GetPosition(null);
			List<UIElement> elementsFound = m_LayoutRoot.HitTest(position) as List<UIElement>;
			if (elementsFound.Contains(dragSource))
				return null;

			foreach (UIElement element in elementsFound)
			{
				if (m_DropTargetList.Contains(element))
					return element;
			}

			return null;
		}

		public void DefaultEnterEventHandler(UIElement dropTarget)
		{
			if (dropTarget is Panel)
			{
				Panel panel = dropTarget as Panel;
				m_OldDropTargetBackground = panel.Background;
				panel.Background = Colors.Green.ToBrush();
			}
		}

		public void DefaultLeaveEventHandler(UIElement dropTarget)
		{
			if (dropTarget is Panel)
			{
				Panel panel = dropTarget as Panel;
				panel.Background = m_OldDropTargetBackground;
			}
		}

		public void DefaultOverEventHandler(UIElement dropTarget)
		{
		}

		public void DefaultDropEventHandler(UIElement dropTarget)
		{
			if (dropTarget is Panel)
			{
				// Reparent the drag source to the drop target
				Panel panel = dropTarget as Panel;
				AddElementToPanel(m_DragSourceClone, panel, GetPosition(dropTarget));
			}
		}

		private void AddElementToPanel(FrameworkElement element, Panel panel, Point point)
		{
			try
			{
				if (element.Parent != null && element.Parent is Panel)
					(element.Parent as Panel).Children.Remove(element);

				if (panel is StackPanel)
					AddElementToStackPanel(element, panel as StackPanel, point);
				else
				if (panel is Grid)
					AddElementToGrid(element, panel as Grid, point);
				else
				if (panel is Canvas)
					AddElementToCanvas(element, panel as Canvas, point);
				else
					panel.Children.Add(element);
			}
			catch (Exception e)
			{
				e.Assert();
			}
		}

		private void AddElementToCanvas(FrameworkElement element, Canvas canvas, Point point)
		{
			double x = point.X - element.ActualWidth / 2;
			double y = point.Y - element.ActualHeight / 2;
			if (x < 0) x = 0;
			if (y < 0) y = 0;
			if (x > canvas.Width - element.Width) x = canvas.Width - element.Width;
			if (y > canvas.Height - element.Height) y = canvas.Height - element.Height;
			element.SetLeft(x);
			element.SetTop(y);
			canvas.Children.Add(element);
		}

		private void AddElementToGrid(FrameworkElement element, Grid grid, Point point)
		{
			// Determine the correct Row and Column to insert the element into
			double x = 0;
			int column = 0;
			while (column < grid.ColumnDefinitions.Count)
			{
				if (point.X < (x += grid.ColumnDefinitions[column].ActualWidth))
					break;
				column++;
			}

			double y = 0;
			int row = 0;
			while (row < grid.RowDefinitions.Count)
			{
				if (point.Y < (y += grid.RowDefinitions[row].ActualHeight))
					break;
				row++;
			}

			grid.Children.Add(element);
			Grid.SetColumn(element, column);
			Grid.SetRow(element, row);
		}

		private void AddElementToStackPanel(FrameworkElement element, StackPanel stackPanel, Point point)
		{
			// Determine the correct index to insert the element into the StackPanel
			bool bVertical = (stackPanel.Orientation == Orientation.Vertical);
			int index = 0;
			while (index < stackPanel.Children.Count)
			{
				UIElement child = stackPanel.Children[index];
				Rect slot = LayoutInformation.GetLayoutSlot(child as FrameworkElement);
				if (bVertical && point.Y < slot.Bottom - (child as FrameworkElement).Margin.Bottom)
					break;
				if (!bVertical && point.X < slot.Right - (child as FrameworkElement).Margin.Right)
					break;
				index++;
			}

			stackPanel.Children.Insert(index, element);
		}
	}
}
