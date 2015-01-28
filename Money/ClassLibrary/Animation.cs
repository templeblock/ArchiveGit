using System;
using System.Windows;
using System.Windows.Media.Animation;

namespace ClassLibrary
{
	internal abstract class AnimationBase : IDisposable
	{
		protected abstract void PrepareTarget(FrameworkElement animationElement);
		protected abstract void SetStoryboards(FrameworkElement animationElement);

		private FrameworkElement _animationElement;
		private Storyboard _forwardStoryboard;
		private Storyboard _reverseStoryboard;

		public event EventHandler ForwardCompleted;
		public event EventHandler ReverseCompleted;

		protected AnimationBase()
		{
		}

		public void Dispose()
		{
			if (_forwardStoryboard != null)
			{
				_forwardStoryboard.Stop();
				_forwardStoryboard.Completed -= ForwardCompletedHandler;
				_forwardStoryboard = null;
			}

			if (_reverseStoryboard != null)
			{
				_reverseStoryboard.Stop();
				_reverseStoryboard.Completed -= ReverseCompletedHandler;
				_reverseStoryboard = null;
			}

			_animationElement = null;
		}

		internal void SetTarget(FrameworkElement animationElement)
		{
			if (_animationElement != null)
				return;

			_animationElement = animationElement;

			PrepareTarget(animationElement);
			SetStoryboards(animationElement);
		}

		protected void SetStoryboards(Storyboard forwardStoryboard, Storyboard reverseStoryboard)
		{
			_forwardStoryboard = forwardStoryboard;
			_reverseStoryboard = reverseStoryboard;

			if (_forwardStoryboard != null)
				_forwardStoryboard.Completed += ForwardCompletedHandler;
			if (_reverseStoryboard != null)
				_reverseStoryboard.Completed += ReverseCompletedHandler;
		}

		private void ForwardCompletedHandler(object sender, EventArgs e)
		{
			if (ForwardCompleted == null)
				return;

			Storyboard storyboard = sender as Storyboard;
			string name = storyboard.GetTargetName();
			if (_animationElement.Tag == null || (string)_animationElement.Tag != name)
				return;

			ForwardCompleted(sender, e);
		}

		private void ReverseCompletedHandler(object sender, EventArgs e)
		{
			if (ReverseCompleted == null)
				return;

			Storyboard storyboard = sender as Storyboard;
			string name = storyboard.GetTargetName();
			if (_animationElement.Tag == null || (string)_animationElement.Tag != name)
				return;

			ReverseCompleted(sender, e);
		}

		internal void Forward()
		{
			Transition(_reverseStoryboard, _forwardStoryboard);
		}

		internal void Reverse()
		{
			Transition(_forwardStoryboard, _reverseStoryboard);
		}

		private void Transition(Storyboard aStoryboard, Storyboard bStoryboard)
		{
			if (_animationElement == null || bStoryboard == null)
				return;

			bStoryboard.Stop();

			// Give the target a unique name so we can find it later
			string name = Guid.NewGuid().ToString();
			bStoryboard.SetTargetName(name);
			_animationElement.Tag = name;

			bStoryboard.SetTarget(_animationElement);
			foreach (Timeline animation in bStoryboard.Children)
				animation.SetTarget(_animationElement);

			//m_AnimationElement.Visibility = Visibility.Visible;
			_animationElement.UpdateLayout();
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
