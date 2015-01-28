using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Animation;

namespace ClassLibrary
{
	public enum Direction2D { X, Y, XY }

	internal class ScaleAnimation : TransformAnimationBase
	{
		public ScaleAnimation()
		{
			DelayForward = 0;
			DelayReverse = 0;
			SecondsForward = 0.75;
			SecondsReverse = 0.75;
			Direction = Direction2D.XY;
			Origin = new Point(.5, .5);
			Scale = new Point(0, 0);
			Offset = new Point(0, 0);
			EasingFunctionForward = base.EasingFunctionForward;
			EasingFunctionReverse = base.EasingFunctionReverse;
		}
	}

	internal class TranslateScaleAnimation : TransformAnimationBase
	{
		public TranslateScaleAnimation()
		{
			DelayForward = 0;
			DelayReverse = 0;
			SecondsForward = 0.75;
			SecondsReverse = 0.75;
			Direction = Direction2D.XY;
			Origin = new Point(.5, .5);
			Scale = new Point(0, 0);
			Offset = new Point(200, 200);
			EasingFunctionForward = base.EasingFunctionForward;
			EasingFunctionReverse = base.EasingFunctionReverse;
		}
	}

	internal abstract class TransformAnimationBase : AnimationBase
	{
		public double DelayForward = 0;
		public double DelayReverse = 0;
		public double SecondsForward = 0;
		public double SecondsReverse = 0;
		public Direction2D Direction = Direction2D.XY;
		public Point Origin = new Point(.5, .5);
		public Point Scale = new Point(1, 1);
		public Point Offset = new Point(0, 0);
		public IEasingFunction EasingFunctionForward = new BackEase() { EasingMode = EasingMode.EaseOut, Amplitude = .75, };
		public IEasingFunction EasingFunctionReverse = new BackEase() { EasingMode = EasingMode.EaseIn, Amplitude = .75, };

		protected override void PrepareTarget(FrameworkElement animationElement)
		{
			animationElement.RenderTransformOrigin = Origin;

			TransformGroup group = animationElement.RenderTransform as TransformGroup;
			if (group != null)
				return;

			ScaleTransform scaleTransform = new ScaleTransform();
			if (Direction == Direction2D.X || Direction == Direction2D.XY)
				scaleTransform.ScaleX = Scale.X;
			if (Direction == Direction2D.Y || Direction == Direction2D.XY)
				scaleTransform.ScaleY = Scale.Y;

			TranslateTransform translateTransform = new TranslateTransform();
			if (Direction == Direction2D.X || Direction == Direction2D.XY)
				translateTransform.X = Offset.X;
			if (Direction == Direction2D.Y || Direction == Direction2D.XY)
				translateTransform.Y = Offset.Y;

			group = new TransformGroup();
			group.Children.Add(scaleTransform);
			group.Children.Add(translateTransform);
			//group.Children.Add(new RotateTransform());
			//group.Children.Add(new SkewTransform());
			animationElement.RenderTransform = group;
		}

		protected override void SetStoryboards(FrameworkElement animationElement)
		{
			SetStoryboards(CreateStoryboard(true/*forward*/), CreateStoryboard(false/*reverse*/));
		}

		private Storyboard CreateStoryboard(bool forward)
		{
			Storyboard storyboard = new Storyboard();
			//storyboard.Children.Add(FadeAnimation(forward));
			if ((Direction == Direction2D.X || Direction == Direction2D.XY) && Scale.X != 1)
				storyboard.Children.Add(ScaleAnimation(forward, true/*horizontal*/, Scale.X));
			if ((Direction == Direction2D.Y || Direction == Direction2D.XY) && Scale.Y != 1)
				storyboard.Children.Add(ScaleAnimation(forward, false/*vertical*/, Scale.Y));
			if ((Direction == Direction2D.X || Direction == Direction2D.XY) && Offset.X != 0)
				storyboard.Children.Add(TranslateAnimation(forward, true/*horizontal*/, Offset.X));
			if ((Direction == Direction2D.Y || Direction == Direction2D.XY) && Offset.Y != 0)
				storyboard.Children.Add(TranslateAnimation(forward, false/*vertical*/, Offset.Y));
			return storyboard;
		}

		private DoubleAnimation ScaleAnimation(bool forward, bool horizontal, double scale)
		{
			DoubleAnimation animation = new DoubleAnimation();
			animation.EasingFunction = (forward ? EasingFunctionForward : EasingFunctionReverse);
			animation.BeginTime = (forward ? TimeSpan.FromSeconds(DelayForward) : TimeSpan.FromSeconds(DelayReverse));
			animation.Duration = (forward ? TimeSpan.FromSeconds(SecondsForward) : TimeSpan.FromSeconds(SecondsReverse));
			//animation.From = 0;
			animation.To = (forward ? 1 : scale);
			string target = (horizontal ? "X" : "Y");
			animation.SetTargetProperty(string.Format("(UIElement.RenderTransform).(TransformGroup.Children)[0].(ScaleTransform.Scale{0})", target));
			return animation;
		}

		private DoubleAnimation TranslateAnimation(bool forward, bool horizontal, double offset)
		{
			DoubleAnimation animation = new DoubleAnimation();
			animation.EasingFunction = (forward ? EasingFunctionForward : EasingFunctionReverse);
			animation.BeginTime = (forward ? TimeSpan.FromSeconds(DelayForward) : TimeSpan.FromSeconds(DelayReverse));
			animation.Duration = (forward ? TimeSpan.FromSeconds(SecondsForward) : TimeSpan.FromSeconds(SecondsReverse));
			//animation.From = (forward ? offset : 0);
			animation.To = (forward ? 0 : offset);
			string target = (horizontal ? "X" : "Y");
			animation.SetTargetProperty(string.Format("(UIElement.RenderTransform).(TransformGroup.Children)[1].(TranslateTransform.{0})", target));
			return animation;
		}

		//private DoubleAnimation FadeAnimation(bool forward)
		//{
		//    DoubleAnimation animation = new DoubleAnimation();
		//    //animation.EasingFunction = EasingFunction;
		//    //animation.BeginTime = (forward ? TimeSpan.FromSeconds(DelayForward) : TimeSpan.FromSeconds(DelayReverse));
		//    //animation.Duration = (forward ? TimeSpan.FromSeconds(SecondsForward) : TimeSpan.FromSeconds(SecondsReverse));
		//    animation.Duration = TimeSpan.FromSeconds(1);
		//    //animation.From = (forward ? 0 : 1);
		//    animation.To = (forward ? 1 : 0);
		//    animation.SetTargetProperty("Opacity");
		//    return animation;
		//}
	}
}
