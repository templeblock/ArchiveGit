using System;
using System.Windows;
using ClassLibrary;

namespace Money
{
	public abstract class ViewPanel : GridEx, IDisposable
	{
		public bool IsVisible { get; set; }
		public bool IsLoaded { get; set; }
		//public bool IsOrphaned { get; set; }
		public event Action<bool> VisibilityChange;

		public ViewPanel()
		{
			if (this is IDropTarget)
				base.Loaded += OnLoaded;
			base.LayoutUpdated += OnLayoutUpdated;
		}

		public virtual void Dispose()
		{
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
		    base.Loaded -= OnLoaded;
			IsLoaded = true;
			if (this is IDropTarget)
			{
				// Add the event listener
				DragDrop dragDrop = App.MainPage.DragDrop;
				dragDrop.EventListeners += (this as IDropTarget).OnDropTargetEvent;
				dragDrop.AddTarget(this);
			}
		}

		void OnLayoutUpdated(object sender, EventArgs e)
		{
			if (VisibilityChange == null)
				return;

			//if (IsOrphaned != (!this.IsDescendentOf(App.MainPage)))
			//	IsOrphaned = !IsOrphaned;

			if (IsVisible != this.IsVisibleInTree())
			{
				IsVisible = !IsVisible;
				VisibilityChange(IsVisible);
			}
		}
	}
}
