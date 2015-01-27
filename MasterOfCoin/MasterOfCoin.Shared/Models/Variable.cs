using System;
using GalaSoft.MvvmLight;

namespace MasterOfCoin
{
	public class Variable : ViewModelBase
	{
		private double? _value;
		public double? Value { get { return _value; } set { Set(() => Value, ref _value, value); } }

		private string _label;
		public string Label { get { return _label; } set { Set(() => Label, ref _label, value); } }

		private bool _isVisible;
		public bool IsVisible { get { return _isVisible; } set { Set(() => IsVisible, ref _isVisible, value); } }

		private bool _isSelectable;
		public bool IsSelectable { get { return _isSelectable; } set { Set(() => IsSelectable, ref _isSelectable, value); } }

		private bool _isSelected;
		public bool IsSelected { get { return _isSelected; } set { Set(() => IsSelected, ref _isSelected, value);
			if (_isSelected && _onSelectedChanged != null)
				_onSelectedChanged(this);} }

		private readonly Action<Variable> _onSelectedChanged;

		public Variable(Action<Variable> onSelectedChanged)
		{
			_onSelectedChanged = onSelectedChanged;
		}

		public Variable(string label, double? value, bool isVisible, bool isSelectable, bool isSelected)
		{
			Value = value;
			Label = label;
			IsVisible = isVisible;
			IsSelectable = isSelectable;
			IsSelected = isSelected;
		}

		public void CopyFrom(Variable variable)
		{
			Value = variable.Value;
			Label = variable.Label;
			IsVisible = variable.IsVisible;
			IsSelectable = variable.IsSelectable;
			IsSelected = variable.IsSelected;
		}

		public double GetSafeValue()
		{
			return (Value ?? 0);
		}
	}
}