using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;

namespace ClassLibrary
{
	public class GridEx : Grid
	{
		public string Columns
		{
			get
			{
				ColumnDefinitionCollection columns = base.ColumnDefinitions;
				StringBuilder sb = new StringBuilder();

				bool first = true;
				foreach (ColumnDefinition cd in columns)
				{
					if (first == false)
						sb.Append(",");
					sb.Append(cd.Width.ToString());
					first = false;
				}

				return sb.ToString();
			}
			set
			{
				ColumnDefinitionCollection columns = base.ColumnDefinitions;
				columns.Clear();

				List<GridLength> widths = ParseLengths(value);
				for (int i = 0; i < widths.Count; i++)
				{
					ColumnDefinition cd = new ColumnDefinition();
					cd.Width = widths[i];
					columns.Add(cd);
				}
			}
		}

		public string Rows
		{
			get
			{
				RowDefinitionCollection rows = base.RowDefinitions;
				StringBuilder sb = new StringBuilder();

				bool first = true;
				foreach (RowDefinition rd in rows)
				{
					if (first == false)
						sb.Append(",");
					sb.Append(rd.Height.ToString());
					first = false;
				}

				return sb.ToString();
			}
			set
			{
				RowDefinitionCollection rows = base.RowDefinitions;
				rows.Clear();

				List<GridLength> heights = ParseLengths(value);
				for (int i = 0; i < heights.Count; i++)
				{
					RowDefinition rd = new RowDefinition();
					rd.Height = heights[i];
					rows.Add(rd);
				}
			}
		}

		private List<GridLength> ParseLengths(string lengthText)
		{
			List<GridLength> lengths = new List<GridLength>();
			string[] parts = lengthText.Split(new char[] { ',', ' ' }, StringSplitOptions.RemoveEmptyEntries);
			if (parts == null || parts.Length == 0)
				return lengths;

			for (int i = 0; i < parts.Length; i++)
			{
				string lengthPart = parts[i];
				if (String.Compare(lengthPart, "Auto", StringComparison.OrdinalIgnoreCase) == 0)
					lengths.Add(new GridLength(0, GridUnitType.Auto));
				else
				if (lengthPart.EndsWith("*", StringComparison.Ordinal))
				{
					double value = (lengthPart.Length == 1 ? 1 : Double.Parse(lengthPart.Substring(0, lengthPart.Length - 1)));
					lengths.Add(new GridLength(value, GridUnitType.Star));
				}
				else
				{
					double value = Double.Parse(lengthPart);
					lengths.Add(new GridLength(value, GridUnitType.Pixel));
				}
			}

			return lengths;
		}
	}
}
