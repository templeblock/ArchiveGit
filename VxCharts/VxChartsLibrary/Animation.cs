using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Animation;

namespace ClassLibrary
{
	public abstract class AnimationBase : IDisposable
	{
		protected abstract void PrepareTarget(FrameworkElement animationElement);
		protected abstract void SetStoryboards(FrameworkElement animationElement);

		protected FrameworkElement m_AnimationElement;
		protected Storyboard m_ForwardStoryboard;
		protected Storyboard m_ReverseStoryboard;

		public event EventHandler ForwardCompleted;
		public event EventHandler ReverseCompleted;

		public AnimationBase()
		{
		}

		public void Dispose()
		{
			if (m_ForwardStoryboard != null)
			{
				m_ForwardStoryboard.Stop();
				m_ForwardStoryboard.Completed -= ForwardCompletedHandler;
				m_ForwardStoryboard = null;
			}

			if (m_ReverseStoryboard != null)
			{
				m_ReverseStoryboard.Stop();
				m_ReverseStoryboard.Completed -= ReverseCompletedHandler;
				m_ReverseStoryboard = null;
			}

			m_AnimationElement = null;
		}

		public void SetTarget(FrameworkElement animationElement)
		{
			if (m_AnimationElement != null)
				return;

			m_AnimationElement = animationElement;

			PrepareTarget(animationElement);
			SetStoryboards(animationElement);
		}

		protected void SetStoryboards(string forwardStoryboardResourceName, string reverseStoryboardResourceName)
		{
			if (m_AnimationElement == null)
				return;

			Storyboard forwardStoryboard = m_AnimationElement.FindResource(forwardStoryboardResourceName) as Storyboard;
			Storyboard reverseStoryboard = m_AnimationElement.FindResource(reverseStoryboardResourceName) as Storyboard;

			Storyboard newForwardStoryboard = forwardStoryboard.DeepClone();
			Storyboard newReverseStoryboard = reverseStoryboard.DeepClone();

			SetStoryboards(newForwardStoryboard, newReverseStoryboard);
		}

		protected void SetStoryboards(Storyboard forwardStoryboard, Storyboard reverseStoryboard)
		{
			m_ForwardStoryboard = forwardStoryboard;
			m_ReverseStoryboard = reverseStoryboard;

			if (m_ForwardStoryboard != null)
				m_ForwardStoryboard.Completed += ForwardCompletedHandler;
			if (m_ReverseStoryboard != null)
				m_ReverseStoryboard.Completed += ReverseCompletedHandler;
		}

		private void ForwardCompletedHandler(object sender, EventArgs e)
		{
			if (ForwardCompleted == null)
				return;

			Storyboard storyboard = sender as Storyboard;
			string name = storyboard.GetTargetName();
			if (m_AnimationElement.Tag == null || (string)m_AnimationElement.Tag != name)
				return;

			ForwardCompleted(sender, e);
		}

		private void ReverseCompletedHandler(object sender, EventArgs e)
		{
			if (ReverseCompleted == null)
				return;

			Storyboard storyboard = sender as Storyboard;
			string name = storyboard.GetTargetName();
			if (m_AnimationElement.Tag == null || (string)m_AnimationElement.Tag != name)
				return;

			ReverseCompleted(sender, e);
		}

		public void Forward()
		{
			Transition(m_ReverseStoryboard, m_ForwardStoryboard);
		}

		public void Reverse()
		{
			Transition(m_ForwardStoryboard, m_ReverseStoryboard);
		}

		private void Transition(Storyboard aStoryboard, Storyboard bStoryboard)
		{
			if (m_AnimationElement == null || bStoryboard == null)
				return;

			bStoryboard.Stop();

			// Give the target a unique name so we can find it later
			string name = Guid.NewGuid().ToString();
			bStoryboard.SetTargetName(name);
			m_AnimationElement.Tag = name;

			bStoryboard.SetTarget(m_AnimationElement);
			foreach (Timeline animation in bStoryboard.Children)
				animation.SetTarget(m_AnimationElement);

			//m_AnimationElement.Visibility = Visibility.Visible;
			m_AnimationElement.UpdateLayout();
			if (aStoryboard != null)
			{
				aStoryboard.Pause();
				Duration duration = bStoryboard.Duration;
				TimeSpan timeSpan = aStoryboard.GetCurrentTime();
				bStoryboard.Begin();
				if (timeSpan.TotalMilliseconds != 0 && duration.HasTimeSpan)
					bStoryboard.Seek(duration.TimeSpan - timeSpan);
			}
			else
				bStoryboard.Begin();
		}
	}

	public class MyOpacityAnimation : AnimationBase
	{
		protected override void PrepareTarget(FrameworkElement animationElement)
		{
			animationElement.Opacity = 0;
		}

		protected override void SetStoryboards(FrameworkElement animationElement)
		{
			base.SetStoryboards("OpacityForward", "OpacityReverse");
		}
	}

	public class MyRotateZAnimation : AnimationBase
	{
		protected override void PrepareTarget(FrameworkElement animationElement)
		{
			PlaneProjection planeProjection = animationElement.Projection as PlaneProjection;
			if (planeProjection == null)
			{
				planeProjection = new PlaneProjection();
				planeProjection.RotationY = 90;
				animationElement.Projection = planeProjection;
			}
		}

		protected override void SetStoryboards(FrameworkElement animationElement)
		{
			base.SetStoryboards("RotateZForward", "RotateZReverse");
		}
	}
}
