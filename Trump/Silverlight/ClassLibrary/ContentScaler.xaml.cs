using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

//j To make into a ContentControl,
//1. Derive from ContentControl insetad of UserControl
//2. Remove InitializeComponent(); and replace with base.DefaultStyleKey = typeof(ContentScaler);
//3. Remove ItemContent
//4. Add generic.xaml below:
//<ResourceDictionary 
// xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
// xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
// xmlns:local="clr-namespace:UserControls.Ventana;assembly=UserControls.Ventana">
//    <Style TargetType="local:Ventana">
//        <Setter Property="Background" Value="Cyan" />
//        <!-- Default Size Constraints -->
//        <Setter Property="Template">  
//            <Setter.Value>
//                <ControlTemplate TargetType="local:Ventana">
//                    <ContentPresenter Content="{TemplateBinding Content}"/>
//                </ControlTemplate>
//            </Setter.Value>
//        </Setter>
//    </Style>
//</ResourceDictionary> 

namespace ClassLibrary
{
	public partial class ContentScaler : UserControl
	{
		public static readonly DependencyProperty ItemContentProperty =
			DependencyProperty.Register("ItemContent", typeof(object),
			typeof(ContentScaler), new PropertyMetadata(OnItemContentChanged));

		public object ItemContent
		{
			set { SetValue(ItemContentProperty, value); }
			get { return GetValue(ItemContentProperty); }
		}

		static void OnItemContentChanged(DependencyObject sender, DependencyPropertyChangedEventArgs args)
		{
			(sender as ContentScaler).presenter.Content = args.NewValue;
		}

		public ContentScaler()
		{
			InitializeComponent();
		}

		protected override void OnMouseEnter(MouseEventArgs e)
		{
			VisualStateManager.GoToState(this, "MouseOver", true);
			base.OnMouseEnter(e);
		}

		protected override void OnMouseLeave(MouseEventArgs e)
		{
			VisualStateManager.GoToState(this, "Normal", true);
			base.OnMouseLeave(e);
		}
	}
}
