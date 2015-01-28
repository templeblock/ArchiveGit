using System;
using System.Windows;
using System.Windows.Data;

// The DataContextProxy class grabs the parent DataContext that flows down to the child 
// UserControl and binds it to a dependency property named DataSourceProperty.\
//
// The DataContextProxy object can be created in the UserControl’s Resource section as shown:
//
//		<UserControl.Resources>
//			<controls:DataContextProxy x:Key="DataContextProxy" />
//		</UserControl.Resources>
//
// This allows the parent DataContext to be accessed by the child UserControl in XAML.
// For example, a ComboBox control nested in a DataGrid can easily get to the Branches property 
// using the following binding syntax:
//
//		<ComboBox DisplayMemberPath="Title"
//			ItemsSource="{Binding Source={StaticResource DataContextProxy}, Path=DataSource.Branches}" />
//
// The binding syntax tells the ComboBox to bind to the DataContextProxy’s DataSource property.  
// This allows the control to get to the original ViewModel object defined in the parent.
// The syntax then says to bind to the Branches property of the ViewModel by using the Path property.

namespace ClassLibrary
{
	public class DataContextProxy : FrameworkElement
	{
		public string BindingPropertyName { get; set; }
		public BindingMode BindingMode { get; set; }

		// The DataSource dependency property
		public static readonly DependencyProperty DataSourceProperty =
			DependencyProperty.Register("DataSource", typeof(Object), typeof(DataContextProxy), null);

		public Object DataSource
		{
			get { return (Object)GetValue(DataSourceProperty); }
			set { SetValue(DataSourceProperty, value); }
		}

		public DataContextProxy()
		{
			base.Loaded += OnLoaded;
		}

		void OnLoaded(object sender, RoutedEventArgs e)
		{
			Binding binding = new Binding();
			if (!String.IsNullOrEmpty(BindingPropertyName))
				binding.Path = new PropertyPath(BindingPropertyName);

			binding.Source = base.DataContext;
			binding.Mode = BindingMode;
			base.SetBinding(DataContextProxy.DataSourceProperty, binding);
		}
	}
}
