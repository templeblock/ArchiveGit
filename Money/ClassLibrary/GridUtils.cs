using System.Windows;
using System.Windows.Controls;

namespace ClassLibrary
{
	// Usage: <Grid c:GridUtils.ColumnDefinitions="100," c:GridUtils.RowDefinitions="Auto,,,,,,2*">

	public class GridUtils
	{
		public static readonly DependencyProperty RowDefinitionsProperty =
			DependencyProperty.RegisterAttached("RowDefinitions", typeof(string), typeof(GridUtils),
				new PropertyMetadata("", new PropertyChangedCallback(OnRowDefinitionsPropertyChanged)));

		public static string GetRowDefinitions(DependencyObject d)
		{
			return (string)d.GetValue(RowDefinitionsProperty);
		}

		public static void SetRowDefinitions(DependencyObject d, string value)
		{
			d.SetValue(RowDefinitionsProperty, value);
		}

		private static void OnRowDefinitionsPropertyChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
		{
			Grid targetGrid = d as Grid;

			// construct the required row definitions
			targetGrid.RowDefinitions.Clear();
			string rowDefs = e.NewValue as string;
			var rowDefArray = rowDefs.Split(',');
			foreach (string rowDefinition in rowDefArray)
			{
				if (rowDefinition.Trim() == "")
					targetGrid.RowDefinitions.Add(new RowDefinition());
				else
				{
					targetGrid.RowDefinitions.Add(new RowDefinition()
					{
						Height = ParseLength(rowDefinition)
					});
				}
			}
		}

		public static readonly DependencyProperty ColumnDefinitionsProperty =
			DependencyProperty.RegisterAttached("ColumnDefinitions", typeof(string), typeof(GridUtils),
				new PropertyMetadata("", new PropertyChangedCallback(OnColumnDefinitionsPropertyChanged)));

		public static string GetColumnDefinitions(DependencyObject d)
		{
			return (string)d.GetValue(ColumnDefinitionsProperty);
		}

		public static void SetColumnDefinitions(DependencyObject d, string value)
		{
			d.SetValue(ColumnDefinitionsProperty, value);
		}

		private static void OnColumnDefinitionsPropertyChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
		{
			Grid targetGrid = d as Grid;

			// construct the required column definitions
			targetGrid.ColumnDefinitions.Clear();
			string columnDefs = e.NewValue as string;
			var columnDefArray = columnDefs.Split(',');
			foreach (string columnDefinition in columnDefArray)
			{
				if (columnDefinition.Trim() == "")
					targetGrid.ColumnDefinitions.Add(new ColumnDefinition());
				else
				{
					targetGrid.ColumnDefinitions.Add(new ColumnDefinition()
					{
						Width = ParseLength(columnDefinition)
					});
				}
			}
		}

		private static GridLength ParseLength(string length)
		{
			length = length.Trim();

			if (length.ToLowerInvariant().Equals("auto"))
				return new GridLength(0, GridUnitType.Auto);

			if (length.Contains("*"))
			{
				length = length.Replace("*", "");
				return new GridLength(double.Parse(length), GridUnitType.Star);
			}

			return new GridLength(double.Parse(length), GridUnitType.Pixel);
		}
	}
}
