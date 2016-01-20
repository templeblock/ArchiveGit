using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Animation;
using System.Windows.Media;
using System.Windows.Input;
using System.Windows.Browser;

namespace ClassLibrary
{
	public interface IDialogPanel
	{
		void DialogPanelInit(FrameworkElement panel, bool bModal, EventHandler closeHandler);
		void DialogPanelShow(bool bShow);
	}

	public class DialogPanelImpl : IDialogPanel
	{
		private FrameworkElement m_this;
		private Canvas m_BackgroundElement;
		private UIElement m_RootDialogElement;
		private EventHandler m_CloseHandler;
		private bool m_bCaptured = false;
		private Point m_DragPoint;
		private double m_left;
		private double m_top;

		public DialogPanelImpl()
		{
		}

		public void DialogPanelInit(FrameworkElement thisElement, bool bModal, EventHandler closeHandler)
		{
			m_CloseHandler = closeHandler;

			m_this = thisElement;
			m_this.SizeChanged += SizeChanged;
			m_this.Visibility = Visibility.Collapsed;
			m_this.SetZIndex(99);
			m_this.MouseLeftButtonDown += MouseLeftButtonDown;
			m_this.MouseLeftButtonUp += MouseLeftButtonUp;
			m_this.MouseMove += MouseMove;

			if (bModal)
			{
				m_BackgroundElement = new Canvas();
				m_BackgroundElement.Visibility = Visibility.Collapsed;
				m_BackgroundElement.SetZIndex(99);
				m_BackgroundElement.Background = new SolidColorBrush("#AAFFFFFF".ToColor());
				m_BackgroundElement.Width = BrowserScreenInformation.ScreenWidth;
				m_BackgroundElement.Height = BrowserScreenInformation.ScreenHeight;
				m_BackgroundElement.Children.Add(m_this);
				m_RootDialogElement = m_BackgroundElement;
			}
			else
			{
				m_BackgroundElement = null;
				m_RootDialogElement = m_this;
			}

			ApplicationEx.RootPanel.Children.Add(m_RootDialogElement);
			DialogPanelShow(true);
		}

		public void DialogPanelShow(bool bShow)
		{
			if (bShow) // force visibility since you can't animate the Visible property
			{
				m_this.Visibility = Visibility.Visible;
				if (m_BackgroundElement != null)
					m_BackgroundElement.Visibility = Visibility.Visible;
			}

			Storyboard storyBoard = new Storyboard();
//j			m_this.Resources.Add("SomeSortofKeyName", storyBoard);

			DoubleAnimation animation = new DoubleAnimation();
			animation.From = (bShow ? 0 : 1);
			animation.To = (bShow ? 1 : 0);
			animation.Duration = TimeSpan.FromSeconds(0.5);
			animation.SetTarget(m_RootDialogElement);
			animation.SetTargetProperty("(UIElement.Opacity)");
			storyBoard.Children.Add(animation);
			if (bShow) storyBoard.Completed += ShowCompleted; else storyBoard.Completed += HideCompleted;
			storyBoard.Begin();
		}

		protected void ShowCompleted(object sender, EventArgs args)
		{
		}

		protected void HideCompleted(object sender, EventArgs args)
		{
			// Force visibility since you can't animate the Visible property
			m_this.Visibility = Visibility.Collapsed;
			ApplicationEx.RootPanel.Children.Remove(m_RootDialogElement);
			if (m_BackgroundElement != null)
			{
				m_BackgroundElement.Children.Remove(m_this);
				m_BackgroundElement = null;
			}
			if (m_CloseHandler != null)
				m_CloseHandler.Invoke(m_this, null);
		}
		
		protected void SizeChanged(object sender, EventArgs args)
		{
			// Once the dialog gets it initial size, we can center it over the browser client area
			// This will occur before the ShowCompleted()
			m_this.SetLeft((BrowserScreenInformation.ClientWidth - m_this.ActualWidth) / 2);
			m_this.SetTop((BrowserScreenInformation.ClientHeight - m_this.ActualHeight) / 2);
		}

		protected void MouseLeftButtonDown(object sender, MouseEventArgs args)
		{
			if (m_bCaptured)
				return;

			m_DragPoint = args.GetPosition(null);
			m_left = m_this.GetLeft();
			m_top = m_this.GetTop();
			m_this.CaptureMouse();
			m_bCaptured = true;
		}

		protected void MouseMove(object sender, MouseEventArgs args)
		{
			if (!m_bCaptured)
				return;
			if (sender != m_this)
				return;

			Point dragPoint = args.GetPosition(null);
			double dx = dragPoint.X - m_DragPoint.X;
			double dy = dragPoint.Y - m_DragPoint.Y;

			if (dx != 0 || dy != 0)
			{
				m_this.SetLeft(m_left + dx);
				m_this.SetTop(m_top + dy);
			}
		}

		protected void MouseLeftButtonUp(object sender, MouseEventArgs args)
		{
			if (!m_bCaptured)
				return;
			if (sender != m_this)
				return;

			m_bCaptured = false;
			m_this.ReleaseMouseCapture();
		}
	}
}
