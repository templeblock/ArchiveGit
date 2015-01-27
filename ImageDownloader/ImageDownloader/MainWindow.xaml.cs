using System.Windows;
using System.Windows.Input;

namespace ImageDownloaderApp
{
	public partial class MainWindow
	{
		public MainWindow()
		{
			InitializeComponent();
		}

		private void OnBuildClick(object sender, RoutedEventArgs e)
		{
			Mouse.OverrideCursor = Cursors.Wait;

			DownloadCardImages.LookupPlayerIDs();
			//DownloadCardImages.CsvReformatter();
			//DownloadCardImages.MoveAndRenameFiles();
			//DownloadCardImages.DownloadFromBBCyberMuseum();
			//DownloadCardImages.DownloadFromBBSimulator();

			Mouse.OverrideCursor = Cursors.Arrow;
		}
#if XAML_DRAW
		private void func(string xamlTemplate, string hotel)
		{
			Canvas oCanvas = (Canvas)XamlReader.Parse(InjectData(xamlTemplate, hotel));   
			int  iWidth  =  (int)oCanvas.Width;   
			int  iHeight  =  (int)oCanvas.Height;   
			
			oCanvas.Arrange(new  Rect(0,  0,  iWidth,  iHeight));   
			
			RenderTargetBitmap  oRenderTargetBitmap  =  new  RenderTargetBitmap(iWidth,  iHeight,  96,  96, PixelFormats.Default);   
			
			oRenderTargetBitmap.Render(oCanvas);  
			
			// Save image using JpegBitmapEncoder 
		}
#endif
	}
}
