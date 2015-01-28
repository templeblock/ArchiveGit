using System;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.ComponentModel;

namespace ClassLibrary
{
	public class DialogPanel : ContentControl, IDisposable
	{
		public event RoutedEventHandler EnterKey;
		public event RoutedEventHandler EscapeKey;
		public event EventHandler Closed;

		public virtual void Dispose()
		{
			m_Animation.Dispose();
		}

		private static object m_Lock = new object();
		private static Canvas m_ModalBackground;
		private static AnimationBase m_ModalBackgroundAnimation;
		private static int m_ModalBackgroundCount = 0;
		private static int m_RunningZIndex = 100;
		private const int m_BackgroundZIndex = 99;

		private static int g_WhichAnimation = 0;

		private Panel m_Parent;
		private Control m_FocusControl;
		private bool m_Show;
		private bool m_Captured;
		private Point m_DragStart;
		private double m_DragLeft;
		private double m_DragTop;
		private AnimationBase m_Animation;
		private Canvas m_ModelessBackground;

		private MessageBoxResult m_DialogResult;
		public MessageBoxResult DialogResult
		{ get { return m_DialogResult; } set { if (m_DialogResult == value) return; m_DialogResult = value; Close(); } }
 
		public DialogPanel()
		{
		}

		private void DisposeInternal()
		{
			base.KeyDown -= OnKeyDown;
			EnterKey = null;
			EscapeKey = null;
			Closed = null;

			if (m_Animation != null)
			{
				m_Animation.ForwardCompleted -= OnShowCompleted;
				m_Animation.ReverseCompleted -= OnHideCompleted;
				m_Animation.Dispose();
			}

			Dispose();
		}

		internal void InitializeDialogPanel(bool modal, Control focusControl)
		{
		lock (m_Lock)
		{
			InitializeDialogPanel(modal, focusControl, ApplicationEx.LayoutRoot/*parent*/);
		}
		}

		internal void InitializeDialogPanel(bool modal, Control focusControl, Panel parent)
		{
		lock (m_Lock)
		{
			m_Parent = parent;
			m_FocusControl = focusControl;
	
			base.Loaded += OnLoaded;
			base.SizeChanged += OnSizeChanged; // Sent when the dialog is first created
			base.Visibility = Visibility.Collapsed;
			this.SetZIndex((int)Interlocked.Increment(ref m_RunningZIndex));

			if (modal)
			{
				if (m_ModalBackground == null)
				{
					m_ModalBackground = new Canvas();
					m_ModalBackground.SetZIndex(m_BackgroundZIndex);
					m_ModalBackground.Visibility = Visibility.Collapsed;
					m_ModalBackground.Background = new SolidColorBrush(Color.FromArgb(0xAA, 0xFF, 0xFF, 0xFF/*#AAFFFFFF*/));
					m_ModalBackground.Width = BrowserScreenInformation.ScreenWidth;
					m_ModalBackground.Height = BrowserScreenInformation.ScreenHeight;
					parent.Children.Add(m_ModalBackground);
				}

				m_ModalBackground.Children.Add(this);
				Interlocked.Increment(ref m_ModalBackgroundCount);
			}
			else
			{ // Modeless
				if (parent is Canvas)
					parent.Children.Add(this);
				else
				{
					m_ModelessBackground = new Canvas();
					m_ModelessBackground.SetZIndex(m_BackgroundZIndex);
					parent.Children.Add(m_ModelessBackground);
					m_ModelessBackground.Children.Add(this);
				}
			}

			SetupAnimation();
		}
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			Show();
		}

		private void SetupAnimation()
		{
			if (g_WhichAnimation == 0)
				m_Animation = new ScaleAnimation() { SecondsForward = 1.0, SecondsReverse = 1.0 }; // Scale from center
			else
			if (g_WhichAnimation == 1)
				m_Animation = new ScaleAnimation() { SecondsForward = 1.0, SecondsReverse = 1.0, Offset = new Point(0, 0) }; // Scale from UL corner
			else
				return;

			FrameworkElement animationElement = this as FrameworkElement;
			m_Animation.SetTarget(animationElement);
			m_Animation.ForwardCompleted += OnShowCompleted;
			m_Animation.ReverseCompleted += OnHideCompleted;
		}

		internal void Show()
		{
		lock (m_Lock)
		{
			if (m_Show)
				return;

			m_Show = true;
			base.KeyDown += OnKeyDown;

			if (m_ModalBackground != null)
				m_ModalBackground.Visibility = Visibility.Visible;
			bool firstModal = (base.Parent == m_ModalBackground && Interlocked.Equals(m_ModalBackgroundCount, 1));
			if (firstModal && m_ModalBackgroundAnimation != null)
				m_ModalBackgroundAnimation.Forward();
			else
				OnShowBackgroundCompleted(null, null);
		}
		}

		private void Close()
		{
		lock (m_Lock)
		{
			if (!m_Show)
				return;

			m_Show = false;
			base.KeyDown -= OnKeyDown;

			if (m_Animation != null)
				m_Animation.Reverse();
			else
				OnHideCompleted(null, null);
		}
		}

		private void OnShowBackgroundCompleted(object sender, EventArgs e)
		{
		lock (m_Lock)
		{
			base.Visibility = Visibility.Visible;
			if (m_Animation != null)
				m_Animation.Forward();
			else
				OnShowCompleted(sender, e);
		}
		}

		private void OnShowCompleted(object sender, EventArgs e)
		{
		lock (m_Lock)
		{
			m_FocusControl.FocusEx();
		}
		}

		private void OnHideCompleted(object sender, EventArgs e)
		{
		lock (m_Lock)
		{
			base.Visibility = Visibility.Collapsed;
			bool lastModal = (base.Parent == m_ModalBackground && Interlocked.Equals(m_ModalBackgroundCount, 1));
			if (lastModal && m_ModalBackgroundAnimation != null)
				m_ModalBackgroundAnimation.Reverse();
			else
				OnHideBackgroundCompleted(sender, e);
		}
		}

		private void OnHideBackgroundCompleted(object sender, EventArgs e)
		{
		lock (m_Lock)
		{
			if (base.Parent == m_Parent)
			{
				m_Parent.Children.Remove(this);
			}
			else
			if (base.Parent == m_ModelessBackground)
			{
				m_ModelessBackground.Children.Remove(this);
				m_Parent.Children.Remove(m_ModelessBackground);
				m_ModelessBackground = null;
			}
			else
			if (base.Parent == m_ModalBackground)
			{
				m_ModalBackground.Children.Remove(this);
				if (Interlocked.Decrement(ref m_ModalBackgroundCount) == 0)
				{
					m_Parent.Children.Remove(m_ModalBackground);
					m_ModalBackground = null;

					if (m_ModalBackgroundAnimation != null)
					{
						m_ModalBackgroundAnimation.ForwardCompleted -= OnShowBackgroundCompleted;
						m_ModalBackgroundAnimation.ReverseCompleted -= OnHideBackgroundCompleted;
						m_ModalBackgroundAnimation.Dispose();
						m_ModalBackgroundAnimation = null;
					}
				}
			}
			
//j			// Do this in a BackgroundTask so the UI is more responsive
//j			BackgroundTask task = new BackgroundTask(Application.Current.RootVisual as FrameworkElement, 200/*runDelay*/, delegate(object sender1, DoWorkEventArgs e1)
//j			{
				if (Closed != null)
					Closed(this, null);

				DisposeInternal();
//j			});
//j			task.Run(null, false/*bCancelPrevious*/);
		}
		}

		private void OnKeyDown(object sender, KeyEventArgs e)
		{
			if (e.Key == Key.Enter && EnterKey != null)
				EnterKey(sender, e);
			else
			if (e.Key == Key.Escape && EscapeKey != null)
				EscapeKey(sender, e);
		}

		private void OnSizeChanged(object sender, SizeChangedEventArgs e)
		{
			// Sent when the dialog is first created
			base.SizeChanged -= OnSizeChanged;

			// Since the dialog now has an initial size, we can center it over the browser client area
			// note: force the values to ints first for better performance
			this.SetLeft((int)((BrowserScreenInformation.ClientWidth - base.ActualWidth) / 2));
			this.SetTop((int)((BrowserScreenInformation.ClientHeight - base.ActualHeight) / 2));
		}

		protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e)
		{
			if (m_Captured)
				return;

			m_Captured = base.CaptureMouse();
			if (!m_Captured)
				return;

			this.SetZIndex((int)Interlocked.Increment(ref m_RunningZIndex));
			if (this is Control)
				(this as Control).Focus();

			m_DragStart = e.GetPosition(null);
			m_DragLeft = this.GetLeft();
			m_DragTop = this.GetTop();
			//base.Cursor = Cursors.SizeWE;
		}

		protected override void OnMouseMove(MouseEventArgs e)
		{
			if (!m_Captured)
				return;

			Point dragPoint = e.GetPosition(null);
			double dx = dragPoint.X - m_DragStart.X;
			double dy = dragPoint.Y - m_DragStart.Y;

			if (dx != 0 || dy != 0)
			{
				this.SetLeft(m_DragLeft + dx);
				this.SetTop(m_DragTop + dy);
			}
		}

		protected override void OnMouseLeftButtonUp(MouseButtonEventArgs e)
		{
			if (!m_Captured)
				return;

			m_Captured = false;
			base.ReleaseMouseCapture();
		}
	}
}
