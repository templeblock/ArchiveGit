using System;
using System.Windows.Input;

// Written by: Jim McCurdy (jmccurdy@facetofacesoftware.com)

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
	}
}
