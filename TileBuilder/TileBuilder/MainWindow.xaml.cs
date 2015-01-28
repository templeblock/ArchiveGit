using System;
using System.Windows;
using System.Windows.Input;
using TileLibrary;

namespace TileBuilderApp
{
	public partial class MainWindow
	{
		public MainWindow()
		{
			InitializeComponent();
		}

		// SQL Query
		/*
			SELECT DISTINCT lahmanID, MIN(fileID) AS fileID
			FROM bb_MasterImage AS i
			WHERE (CHARINDEX('C', fileID) = 1) AND (lahmanID IS NOT NULL)
			GROUP BY lahmanID
			ORDER BY lahmanID
		*/
		private void OnBuildDziTilesClick(object sender, RoutedEventArgs e)
		{
			BuildTiles(false);
		}

		private void OnBuildCollectionTilesClick(object sender, RoutedEventArgs e)
		{
			BuildTiles(true);
		}

		private void BuildTiles(bool buildCollectionTiles)
		{
			Mouse.OverrideCursor = Cursors.Wait;

			if (string.IsNullOrWhiteSpace(x_CollectionName.Text))
				x_CollectionName.Text = "Untitled";
			if (!x_OutputFolder.Text.EndsWith("\\") && !x_OutputFolder.Text.EndsWith("/"))
				x_OutputFolder.Text += "\\";
			string outputPath = x_OutputFolder.Text + x_CollectionName.Text;
			System.Drawing.Size? commonSize = null;
			if (x_CommonSize.IsChecked == true)
				commonSize = new System.Drawing.Size(Convert.ToInt32(x_TileWidth.Text), Convert.ToInt32(x_TileHeight.Text));

			string idText = x_ListOfIDs.Text;
			string[] lines = idText.Split(new[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);

			TileBuilder builder = new TileBuilder(x_MissingTilesOnly.IsChecked ?? false, buildCollectionTiles, outputPath, commonSize);

			int total = 0;
			int errors = 0;
			int lineCount = 0;
			string progress = null;
			bool publish = true;
			foreach (string line in lines)
			{
				int count;
				try
				{
					lineCount++;
					string[] values = line.Split(new[] { '\t', ' ', ',' }, StringSplitOptions.RemoveEmptyEntries);
					if (values.Length < 1)
						continue;
					int imageID = Convert.ToInt32(values[0]);
					string sourcePath = string.Format(x_SourcePath.Text, values);
					bool sourceIsFile = !sourcePath.StartsWith("http://");
					count = builder.UpdateTiles(imageID, sourcePath, sourceIsFile);
				}
				catch (Exception ex)
				{
					ex.GetType();
					MessageBox.Show(string.Format("Invalid ID data on line {0}", lineCount), "Build Error");
					publish = false;
					break;
				}
	
				if (count < 0)
					errors++;
				else
					total += count;

				progress = string.Format("{0} tiles were built.\r\n{1} errors occurred.", total, errors);
				x_Progress.Text = progress;
			}

			if (buildCollectionTiles)
			{
				int publishCount = (publish ? builder.PublishCollectionTiles() : 0);
			}

			Mouse.OverrideCursor = Cursors.Arrow;
			if (progress != null)
				MessageBox.Show(progress, "Build Complete");
		}
#if XAML_DRAW
		private void func(string xamlTemplate, string hotel)
		{
			Canvas oCanvas = (Canvas)XamlReader.Parse(InjectData(xamlTemplate, hotel));
			int iWidth = (int)oCanvas.Width;
			int iHeight = (int)oCanvas.Height;
			
			oCanvas.Arrange(new Rect(0, 0, iWidth, iHeight));
			
			RenderTargetBitmap oRenderTargetBitmap = new RenderTargetBitmap(iWidth, iHeight, 96, 96, PixelFormats.Default);
			
			oRenderTargetBitmap.Render(oCanvas);
			
			// Save image using JpegBitmapEncoder 
		}
#endif
	}
}
