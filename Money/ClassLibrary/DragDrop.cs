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
	public delegate void DropTargetEvent(DragDrop dragDrop, DropTargetEventType type);
	internal interface IDropTarget
	{
		void OnDropTargetEvent(DragDrop dragDrop, DropTargetEventType type);
	}

	public class DragDrop
	{
		private bool m_bCaptured;
		private bool m_bDragging;
		private FrameworkElement m_DragSource;
//j		private Panel m_DragSourceParentPanel;
		private FrameworkElement m_DragSourceClone;
		private object m_DragSourcePayload;
		private readonly Dictionary<FrameworkElement, object> m_DragSourcePayloadList = new Dictionary<FrameworkElement, object>();
		private UIElement m_DropTarget;
		private readonly List<UIElement> m_DropTargetList = new List<UIElement>();
		private readonly Popup m_FeedbackPopup = new Popup();
		private Brush m_OldDropTargetBackground;
		private MouseEventArgs m_MouseEventArgs;
		private Point m_LeftButtonDownPoint;

		public event DropTargetEvent EventListeners;
		public FrameworkElement Source { get { return m_DragSource; } }
		public object Payload { get { return m_DragSourcePayload; } }
		public UIElement Target { get { return m_DropTarget; } }

		public DragDrop()
		{
		}

		internal void AddSource(FrameworkElement dragSource, object payload)
		{
			Debug.Assert(dragSource != null);
			dragSource.MouseLeftButtonDown += DragSource_MouseLeftButtonDown;
			dragSource.MouseLeftButtonUp += DragSource_MouseLeftButtonUp;
			dragSource.MouseMove += DragSource_MouseMove;
			m_DragSourcePayloadList.Add(dragSource, payload);
		}

		internal void RemoveSource(FrameworkElement dragSource)
		{
			Debug.Assert(dragSource != null);
			dragSource.MouseLeftButtonDown -= DragSource_MouseLeftButtonDown;
			dragSource.MouseLeftButtonUp -= DragSource_MouseLeftButtonUp;
			dragSource.MouseMove -= DragSource_MouseMove;
			m_DragSourcePayloadList.Remove(dragSource);
		}

		internal void AddTarget(UIElement dropTarget)
		{
			Debug.Assert(dropTarget != null);
			m_DropTargetList.Add(dropTarget);
		}

		internal void RemoveTarget(UIElement dropTarget)
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

		private void DragSource_MouseLeftButtonDown(object sender, MouseEventArgs e)
		{
			if (m_bCaptured)
				DragSource_MouseLeftButtonUp(sender, e);

			m_bCaptured = (sender as UIElement).CaptureMouse();
			if (!m_bCaptured)
				return;

			m_LeftButtonDownPoint = e.GetPosition(null);
			m_bDragging = false;
		}

		private void DragSource_MouseLeftButtonUp(object sender, MouseEventArgs e)
		{
			if (!m_bCaptured)
				return;

			if (m_bDragging)
				DraggingMouseLeftButtonUp(sender, e);

			m_bDragging = false;
			m_bCaptured = false;
			(sender as UIElement).ReleaseMouseCapture();
		}

		private void DragSource_MouseMove(object sender, MouseEventArgs e)
		{
			if (!m_bCaptured)
				return;

			if (m_bDragging)
				DraggingMouseMove(sender, e);
			else
				m_bDragging = DraggingStart(sender, e);
		}

		private bool DraggingStart(object sender, MouseEventArgs e)
		{
			Point dragPoint = e.GetPosition(null);
			double dx = dragPoint.X - m_LeftButtonDownPoint.X;
			double dy = dragPoint.Y - m_LeftButtonDownPoint.Y;
			if (dx <= 3 && dx >= -3 && dy <= 3 && dy >= -3)
				return false;

			// We have made an initial movement, so start dragging

			m_MouseEventArgs = e;
			m_DragSource = sender as FrameworkElement;
//j			m_DragSourceParentPanel = m_DragSource.Parent as Panel;
			m_DragSourceClone = m_DragSource.DeepClone();
			if (m_DragSource != null) m_DragSourcePayload = m_DragSourcePayloadList[m_DragSource];

			// Create the feedback popup
			ContentControl contentControl = new ContentControl();
			contentControl.Content = m_DragSourceClone;
			contentControl.Opacity = 0.5;
			m_FeedbackPopup.IsOpen = false;
			m_FeedbackPopup.Child = contentControl;
			ApplicationEx.LayoutRoot.Children.Add(m_FeedbackPopup);
			m_FeedbackPopup.HorizontalOffset = dragPoint.X - (m_DragSourceClone.ActualWidth / 2);
			m_FeedbackPopup.VerticalOffset = dragPoint.Y - (m_DragSourceClone.ActualHeight / 2);
			m_FeedbackPopup.IsOpen = true;
			return true;
		}

		private void DraggingMouseMove(object sender, MouseEventArgs e)
		{
			m_MouseEventArgs = e;
			Point dragPoint = e.GetPosition(null);
			m_FeedbackPopup.HorizontalOffset = dragPoint.X - (m_DragSourceClone.ActualWidth / 2);
			m_FeedbackPopup.VerticalOffset = dragPoint.Y - (m_DragSourceClone.ActualHeight / 2);

			UIElement dropTarget = FindDropTarget(m_FeedbackPopup, e);
//j			if (dropTarget == null)
//j				dropTarget = m_DragSourceParentPanel;
			if (EventListeners != null)
			{
				if (m_DropTarget != dropTarget)
				{
					EventListeners(this, DropTargetEventType.Leave);
					m_DropTarget = dropTarget;
					EventListeners(this, DropTargetEventType.Enter);
				}
				else
					EventListeners(this, DropTargetEventType.Over);
			}
		}

		private void DraggingMouseLeftButtonUp(object sender, MouseEventArgs e)
		{
			m_MouseEventArgs = e;

			// Get rid of the feedback popup
			m_FeedbackPopup.IsOpen = false;
			ApplicationEx.LayoutRoot.Children.Remove(m_FeedbackPopup);
			m_FeedbackPopup.Child = null;

			m_DropTarget = FindDropTarget(m_FeedbackPopup, e);
//j			if (dropTarget == null)
//j				dropTarget = m_DragSourceParentPanel;
			if (EventListeners != null)
			{
				EventListeners(this, DropTargetEventType.Leave);
				EventListeners(this, DropTargetEventType.Drop);
			}

//j Don't wack these values anymore since some drop operations are asynchronous
//j			m_DragSourceClone = null;
//j			m_DragSource = null;
//j//j			m_DragSourceParentPanel = null;
//j			m_DragSourcePayload = null;
//j			m_DropTarget = null;
		}

		private UIElement FindDropTarget(UIElement dragSource, MouseEventArgs e)
		{
			ICollection<UIElement> elementsFound = ApplicationEx.LayoutRoot.HitTest(e);
			if (elementsFound.Contains(dragSource))
				return null;

			foreach (UIElement element in elementsFound)
			{
				if (m_DropTargetList.Contains(element))
					return element;
			}

			return null;
		}

		internal void DefaultEnterEventHandler(UIElement dropTarget)
		{
			Panel panel = dropTarget as Panel;
			if (panel != null)
			{
				m_OldDropTargetBackground = panel.Background;
				panel.Background = Colors.Green.ToBrush();
			}
		}

		internal void DefaultLeaveEventHandler(UIElement dropTarget)
		{
			Panel panel = dropTarget as Panel;
			if (panel != null)
			{
				panel.Background = m_OldDropTargetBackground;
			}
		}

		internal void DefaultOverEventHandler(UIElement dropTarget)
		{
		}

		internal void DefaultDropEventHandler(UIElement dropTarget)
		{
			Panel panel = dropTarget as Panel;
			if (panel != null)
			{
				// Reparent the drag source to the drop target
				AddElementToPanel(m_DragSourceClone, panel, GetPosition(dropTarget));
			}
		}

		private static void AddElementToPanel(FrameworkElement element, Panel panel, Point point)
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
			catch (Exception ex)
			{
				ex.Alert();
			}
		}

		private static void AddElementToCanvas(FrameworkElement element, Panel canvas, Point point)
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

		private static void AddElementToGrid(FrameworkElement element, Grid grid, Point point)
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

		private static void AddElementToStackPanel(UIElement element, StackPanel stackPanel, Point point)
		{
			// Determine the correct index to insert the element into the StackPanel
			bool bVertical = (stackPanel.Orientation == Orientation.Vertical);
			int index = 0;
			while (index < stackPanel.Children.Count)
			{
				UIElement child = stackPanel.Children[index];
				Rect slot = LayoutInformation.GetLayoutSlot(child as FrameworkElement);
				if (bVertical && point.Y < slot.Bottom - ((FrameworkElement)child).Margin.Bottom)
					break;
				if (!bVertical && point.X < slot.Right - ((FrameworkElement) child).Margin.Right)
					break;
				index++;
			}

			stackPanel.Children.Insert(index, element);
		}
	}
}
