using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Pivot;
using Microsoft.Pivot.Internal.Views;

namespace Pivot
{
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
		public event EventHandler<PivotViewerStateEventArgs> ArticleButtonClicked;
		public event EventHandler<PivotViewerStateEventArgs> ArticleLinkClicked;
		public event EventHandler<PivotViewerStateEventArgs> CodeButtonClicked;
		public event EventHandler<PivotViewerStateEventArgs> FiltersChanged;
		public event EventHandler<PivotViewerStateEventArgs> SortChanged;
		public event EventHandler<PivotViewerStateEventArgs> ViewChanged;
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

		private Grid GridContainer { get; set; }
		private Grid GridViews { get; set; }

		public Visibility TileVisibility {
			get { return GridViews.Children[0].Visibility; }
			set { GridViews.Children[0].Visibility = value; }
		}

		public Visibility FilterVisibility
		{
			get { return GridViews.Children[2].Visibility; }
			set {
				GridViews.Children[2].Visibility = value;
				GridViews.ColumnDefinitions[0].Width = (value == Visibility.Collapsed ? new GridLength(0) : GridLength.Auto);
			}
		}

		public Visibility InfoVisibility
		{
			get { return (GridViews.ColumnDefinitions[2].Width.Value == 0 ? Visibility.Collapsed : Visibility.Visible); }
			set { GridViews.ColumnDefinitions[2].Width = (value == Visibility.Collapsed ? new GridLength(0) : GridLength.Auto); }
		}

		public Brush GridBackground
		{
			get { return GridContainer.Background; }
			set { GridContainer.Background = value; }
		}

		public override void OnApplyTemplate()
		{
			base.OnApplyTemplate();

			// Locate UI Elements
			Grid partContainer = (Grid)base.GetTemplateChild("PART_Container");
			CollectionViewerView cvv = ((CollectionViewerView)(partContainer).Children[0]);
			Grid container = cvv.Content as Grid;
			Grid viewerGrid = container.Children[1] as Grid;

			GridContainer = container;
			GridViews = viewerGrid;

			//FilterVisibility = Visibility.Collapsed;
			//InfoVisibility = Visibility.Collapsed;
			//TileVisibility = Visibility.Collapsed;

			// PivotViewer background
			GradientStopCollection gsc = new GradientStopCollection {
				new GradientStop { Color = Colors.Orange, Offset = 0.0 },
				new GradientStop { Color = Colors.Yellow, Offset = 0.5 },
				new GradientStop { Color = Colors.Green, Offset = 1.0 }
			};

			GridBackground = new RadialGradientBrush(gsc)
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

		/// <summary>
		/// Callback that this viewer will call to get custom actions for a specific item
		/// </summary>
		public Func<string, List<CustomAction>> GetCustomActionsForItemCallback { get; set; }

		public List<CustomAction> CustomActions { get; set; }

		public PivotViewerEx()
		{
			CustomActions = new List<CustomAction>();
		}

		/// <summary>
		/// Return an add or remove custom action for the item, depending on whether it is in the cart
		/// </summary>
		protected override List<CustomAction> GetCustomActionsForItem(string itemId)
		{
			if (GetCustomActionsForItemCallback != null)
				CustomActions = GetCustomActionsForItemCallback(itemId);

			return CustomActions;
		}
	}
}
