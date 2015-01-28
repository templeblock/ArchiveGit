using System.Windows;
using System.Windows.Controls;
using Millicents = System.Int64;
using System.Windows.Media;

namespace ClassLibrary
{
	public class LegendData : ViewModelBase
	{
		public string Label { get { return m_Label; } set { SetProperty(ref m_Label, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string m_Label;

		public string Title { get { return m_Title; } set { SetProperty(ref m_Title, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string m_Title;

		public Brush BalanceBrush { get { return m_BalanceBrush; } set { SetProperty(ref m_BalanceBrush, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Brush m_BalanceBrush;

		public Millicents Balance { get { return m_Balance; } set { SetProperty(ref m_Balance, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Millicents m_Balance;

		public Millicents TrendBalance { get { return m_TrendBalance; } set { SetProperty(ref m_TrendBalance, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Millicents m_TrendBalance;
	}

	public partial class LegendDialog : DialogPanel
	{
		public LegendData Data = new LegendData();

		public LegendDialog(Panel parent)
		{
			InitializeComponent();
			InitializeDialogPanel(false/*bModal*/, null/*focusControl*/, parent);
			base.Loaded += OnLoaded;
			base.SizeChanged += OnSizeChanged;
			base.DataContext = Data;
		}

		public override void Dispose()
		{
			Data.Dispose();
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
		}

		private void OnSizeChanged(object sender, SizeChangedEventArgs e)
		{
			// Since the dialog now has an initial size, we can center it over the PlotArea
			this.SetLeft((base.Parent as FrameworkElement).ActualWidth - base.ActualWidth - 10);
			this.SetTop(10);
		}
	}
}
