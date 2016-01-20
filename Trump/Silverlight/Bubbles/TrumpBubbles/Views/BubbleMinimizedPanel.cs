using System;
using System.Windows;
using System.Windows.Controls;
using ClassLibrary;

namespace TrumpBubbles
{
	public class BubbleMinimizedPanel : StretchPanel
	{
		AnimationBase _Animation = new BubbleMinimizedPanelAnimation();

		// The IsShown dependency property
		#region The IsShown dependency property
		public static readonly DependencyProperty IsShownProperty =
			DependencyProperty.Register("IsShown", typeof(bool), typeof(BubbleMinimizedPanel),
			new PropertyMetadata(false, (d, e) => ((BubbleMinimizedPanel)d).OnIsShownPropertyChanged((bool)e.OldValue, (bool)e.NewValue)));

		public bool IsShown
		{
			get { return (bool)GetValue(IsShownProperty); }
			set { SetValue(IsShownProperty, value); }
		}

		private void OnIsShownPropertyChanged(bool oldValue, bool newValue)
		{
			if (oldValue == newValue)
				return;
			
			if (newValue == true)
			{
				base.Visibility = Visibility.Visible;
				_Animation.Forward();
			}
			else
			{
				_Animation.Reverse();
			}
		}
		#endregion

		public BubbleMinimizedPanel()
		{
			_Animation.SetTarget(this);
			_Animation.ForwardCompleted += ForwardCompleted;
			_Animation.ReverseCompleted += ReverseCompleted;
		}

		private void ForwardCompleted(object senderForward, EventArgs eForward)
		{
		}
	
		private void ReverseCompleted(object senderReverse, EventArgs eReverse)
		{
			base.Visibility = Visibility.Collapsed;
		}
	}

	internal class BubbleMinimizedPanelAnimation : TransformAnimationBase
	{
		public BubbleMinimizedPanelAnimation()
		{
			SecondsForward = 0.75;
			SecondsReverse = 0.75;
			Direction = Direction2D.XY;
			Origin = new Point(.5, 0);
			Scale = new Point(0, 0);
			Offset = new Point(0, 0);
			EasingFunction = null;	
		}
	}
}
