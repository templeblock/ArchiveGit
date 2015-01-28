using System;
using System.Windows.Input;

// usage:

//	class CustomerViewModel
//	{
//		public ICommand LoadCustomersCommand { get { return _LoadCustomersCommand; } }
//		private ICommand _LoadCustomersCommand = new Command(LoadCustomers, CanLoadCustomers);
//		public ObservableCollection<Customer> CustomerCollection { get { return _CustomerCollection; } set { SetProperty(ref _CustomerCollection, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
//		private ObservableCollection<Customer> _CustomerCollection;

//		private void LoadCustomers(object parameter)
//		{
//			CustomerCollection = CustomerProvider.LoadCustomers();
//		}

//		private bool CanLoadCustomers(object parameter)
//		{
//			return true;
//		}
//	}

//	<UserControl.Resources>
//		<local:CustomerViewModel x:Key="CustomerViewModel"/>
//	</UserControl.Resources>

//	<Button Command="{Binding LoadCustomersCommand, Source={StaticResource CustomerViewModel}}" Content="Load Customers" />
//	<ListBox ItemsSource="{Binding CustomerCollection, Source={StaticResource CustomerViewModel}}" />

namespace ClassLibrary
{
	public class Command : ICommand
	{
		public event EventHandler CanExecuteChanged;

		private Func<object, bool> _CanExecuteFunction;
		private Action<object> _ExecuteAction;
		private bool _CanExecute;

		public Command(Action<object> executeAction, Func<object, bool> canExecuteFunction = null)
		{
			_ExecuteAction = executeAction;
			if (canExecuteFunction == null)
				canExecuteFunction = (p) => { return true; };
			_CanExecuteFunction = canExecuteFunction;
		}

		public bool CanExecute(object parameter)
		{
			bool canExecute = (_CanExecuteFunction != null ? _CanExecuteFunction(parameter) : true);
			if (_CanExecute != canExecute)
			{
				_CanExecute = canExecute;
				if (CanExecuteChanged != null)
					CanExecuteChanged(this, new EventArgs());
			}

			return canExecute;
		}

		public void Execute(object parameter)
		{
			_ExecuteAction(parameter);
		}

		//public event EventHandler CanExecuteChanged
		//{
		//    add { CommandManager.RequerySuggested += value; }
		//    remove { CommandManager.RequerySuggested -= value; }
		//}
	}
}
