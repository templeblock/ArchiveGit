using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Animation;

namespace ClassLibrary
{
	internal abstract class AnimationBase : IDisposable
	{
		protected abstract void PrepareTarget(FrameworkElement animationElement);
		protected abstract void SetStoryboards(FrameworkElement animationElement);

		private FrameworkElement m_AnimationElement;
		private Storyboard m_ForwardStoryboard;
		private Storyboard m_ReverseStoryboard;

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

		internal void SetTarget(FrameworkElement animationElement)
		{
			if (m_AnimationElement != null)
				return;

			m_AnimationElement = animationElement;

			PrepareTarget(animationElement);
			SetStoryboards(animationElement);
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

		internal void Forward()
		{
			Transition(m_ReverseStoryboard, m_ForwardStoryboard);
		}

		internal void Reverse()
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
}
