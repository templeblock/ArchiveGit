using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Pivot;
using Microsoft.Pivot.Internal.Views;
using Microsoft.Pivot.Internal.Controls;
using Microsoft.Pivot.Internal.Model;
using Microsoft.Pivot.Internal.ViewModels;
using System.ComponentModel;

namespace Pivot
{
	public enum ViewerStateType
	{
		Grid = 1,
		Graph = 2,
	}

	public class PivotViewerStateEventArgs : EventArgs
	{
		public string ArticleHref { get; set; } 
		public string CodeHref { get; set; } 
		public string FilterCategories { get; set; } 
		public string SortFacet { get; set; } 
		public string State { get; set; } 
		public string View { get; set;
		}
	} 

	public class PivotViewerEx : PivotViewer
	{
		private string _viewerStateGrid = string.Format("$view$={0}", ViewerStateType.Grid);
		private string _viewerStateGraph = string.Format("$view$={0}", ViewerStateType.Graph);

		public string SortFacet { get; set; }
		public ViewerStateType ViewerStateType { get; set; }
		public StepLogSliderViewModel ZoomSlider { get; set; }
		public List<CustomAction> CustomActions { get; set; }

		/// Callback that this viewer will call to get custom actions for a specific item
		public Func<string, List<CustomAction>> GetCustomActionsForItemCallback { get; set; }

		public event EventHandler<PivotViewerStateEventArgs> ArticleButtonClicked;
		public event EventHandler<PivotViewerStateEventArgs> ArticleLinkClicked;
		public event EventHandler<PivotViewerStateEventArgs> CodeButtonClicked;
		public event EventHandler<PivotViewerStateEventArgs> FiltersChanged;
		public event EventHandler<PivotViewerStateEventArgs> SortChanged;
		public event EventHandler<PivotViewerStateEventArgs> ViewChanged;

		private Grid CollectionContainer { get; set; }
		private UIElement TileContainer { get; set; }
		private UIElement FilterContainer { get; set; }
		private ColumnDefinition FilterColumnDefinition { get; set; }
		private ColumnDefinition InfoColumnDefinition { get; set; }
		
		public PivotViewerEx()
		{
			CustomActions = new List<CustomAction>();
		}

		private string GetViewerState()
		{
			return string.Format("$view$={0}&$facet0$={1}", ViewerStateType, SortFacet);
		}

		private ViewerStateType GetViewerStateType()
		{
			return (base.ViewerState.IndexOf(_viewerStateGraph) >= 0 ? ViewerStateType.Graph : ViewerStateType.Grid);
		}
	
		protected virtual void OnArticleButtonClicked(string articleUrl)
		{
			if (ArticleButtonClicked != null)
				ArticleButtonClicked(this, new PivotViewerStateEventArgs { ArticleHref = articleUrl });
		}

		protected virtual void OnArticleLinkClicked(string articleUrl)
		{
			if (ArticleLinkClicked != null)
				ArticleLinkClicked(this, new PivotViewerStateEventArgs { ArticleHref = articleUrl });
		}
	
		protected virtual void OnCodeButtonClicked(string articleUrl, string codeUrl)
		{
			if (CodeButtonClicked != null)
				CodeButtonClicked(this, new PivotViewerStateEventArgs { ArticleHref = articleUrl, CodeHref = codeUrl });
		}
		
		protected virtual void OnFiltersChanged(string filterString)
		{
			if (FiltersChanged != null)
				FiltersChanged(this, new PivotViewerStateEventArgs { FilterCategories = filterString });
		}
		
		protected virtual void OnSortChanged(string sortFacet)
		{
			if (SortChanged != null)
				SortChanged(this, new PivotViewerStateEventArgs { SortFacet = sortFacet });
		}
		
		protected virtual void OnViewChanged(string view)
		{
			if (ViewChanged != null)
				ViewChanged(this, new PivotViewerStateEventArgs { View = view });
		} 

		public Visibility TileVisibility
		{
			get { return TileContainer.Visibility; }
			set { TileContainer.Visibility = value; }
		}

		public Visibility FilterVisibility
		{
			get { return FilterContainer.Visibility; }
			set {
				FilterContainer.Visibility = value;
				FilterColumnDefinition.Width = (value == Visibility.Collapsed ? new GridLength(0) : GridLength.Auto);
			}
		}

		public Visibility InfoVisibility
		{
			get { return (InfoColumnDefinition.Width.Value == 0 ? Visibility.Collapsed : Visibility.Visible); }
			set { InfoColumnDefinition.Width = (value == Visibility.Collapsed ? new GridLength(0) : GridLength.Auto); }
		}

		public Brush CollectionBackgroundBrush
		{
			get { return CollectionContainer.Background; }
			set { CollectionContainer.Background = value; }
		}

		public void ZoomOut()
		{
			ZoomSlider.Value = ZoomSlider.Minimum;
		}

		public void ZoomIn()
		{
			ZoomSlider.Value = ZoomSlider.Maximum;
		}

		public void ViewAsGrid()
		{
			if (GetViewerStateType() == ViewerStateType.Grid)
				return;
			ViewerStateType = ViewerStateType.Grid;
			if (base.CollectionUri != null)
				base.LoadCollection(base.CollectionUri.ToString(), GetViewerState());
		}

		private void ViewAsGraph()
		{
			if (GetViewerStateType() == ViewerStateType.Graph)
				return;
			ViewerStateType = ViewerStateType.Graph;
			if (base.CollectionUri != null)
				base.LoadCollection(base.CollectionUri.ToString(), GetViewerState());
		}

		public override void OnApplyTemplate()
		{
			base.OnApplyTemplate();

			// Locate UI Elements
			Grid partContainer = (Grid)base.GetTemplateChild("PART_Container");
			CollectionViewerView collectionViewer = ((CollectionViewerView)(partContainer).Children[0]);
			CollectionContainer = collectionViewer.Content as Grid;
			Grid ViewerContainer = CollectionContainer.Children[1] as Grid;
			ControlBarView ControlBar = ((UIElement)CollectionContainer.Children[2]) as ControlBarView;

			TileContainer = ViewerContainer.Children[0];
			FilterContainer = ViewerContainer.Children[2];
			FilterColumnDefinition = ViewerContainer.ColumnDefinitions[0];
			InfoColumnDefinition = ViewerContainer.ColumnDefinitions[2];

			// Find the StepLogSlider
			Grid controlBarContent = (Grid)ControlBar.Content;
			DockPanel controlBarDockPanel = (DockPanel)controlBarContent.Children[2];
			ContentControl zoomSlider = (ContentControl)controlBarDockPanel.Children[2];
			ContentControl zoomSliderContent = (ContentControl)zoomSlider.Content;
			ZoomSlider = (StepLogSliderViewModel)zoomSliderContent.Content;

			// PivotViewer background
			GradientStopCollection gsc = new GradientStopCollection {
				new GradientStop { Color = Colors.Orange, Offset = 0.0 },
				new GradientStop { Color = Colors.Yellow, Offset = 0.5 },
				new GradientStop { Color = Colors.Green, Offset = 1.0 }
			};

			CollectionBackgroundBrush = new RadialGradientBrush(gsc)
			{
				Center = new Point(1.0, 1.0),
				GradientOrigin = new Point(1.0, 1.0),
				RadiusX = 1.0,
				RadiusY = 1.0
			};

			// PivotViewer background image
			//StreamResourceInfo sr = Application.GetResourceStream(new Uri("PivotViewerSample;component/Resources/bgrd.jpg", UriKind.Relative));
			//BitmapImage bmp = new BitmapImage();
			//bmp.SetSource(sr.Stream);
			//GridBackground = new ImageBrush() { ImageSource = bmp };
			//sr.Stream.Close();
		}

		/// Return the custom action for the item
		protected override List<CustomAction> GetCustomActionsForItem(string itemId)
		{
			if (GetCustomActionsForItemCallback != null)
				CustomActions = GetCustomActionsForItemCallback(itemId);

			return CustomActions;
		}
	}
}
