using System;
using System.Windows;

namespace ClassLibrary
{
	public abstract class ViewPanel : GridEx, IDisposable
	{
		public bool IsVisible { get; set; }
		public bool IsLoaded { get; set; }
		//public bool IsOrphaned { get; set; }
		public event Action<bool> VisibilityChange;

		public ViewPanel()
		{
			base.LayoutUpdated += OnLayoutUpdated;
		}

		public virtual void Dispose()
		{
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
