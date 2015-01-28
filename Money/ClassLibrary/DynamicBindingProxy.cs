using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Dynamic;
using System.Linq;
using System.Reflection;

namespace ClassLibrary
{
	public class DynamicBindingProxy<TModel> : DynamicObject, INotifyPropertyChanged
	{ // http://www.deanchalk.me.uk/post/WPF-e28093-Easy-INotifyPropertyChanged-Via-DynamicObject-Proxy.aspx

		public event PropertyChangedEventHandler PropertyChanged;
		private readonly TModel _model;
		private static readonly Dictionary<string, Dictionary<string, PropertyInfo>> _propertyList = new Dictionary<string, Dictionary<string, PropertyInfo>>();
		private readonly string _typeName;

		public DynamicBindingProxy(TModel model)
		{
			_model = model;
			Type type = typeof(TModel);
			_typeName = type.FullName;
			if (!_propertyList.ContainsKey(_typeName))
			{
				PropertyInfo[] properties = type.GetProperties(BindingFlags.Public | BindingFlags.Instance);
				var dictionary = properties.ToDictionary(prop => prop.Name);
				_propertyList.Add(_typeName, dictionary);
			}
		}

		public override bool TryGetMember(GetMemberBinder binder, out object result)
		{
			result = null;
			if (!_propertyList[_typeName].ContainsKey(binder.Name))
				return false;

			result = _propertyList[_typeName][binder.Name].GetValue(_model, null);
			return true;
		}

		public override bool TrySetMember(SetMemberBinder binder, object value)
		{
			if (!_propertyList[_typeName].ContainsKey(binder.Name))
				return false;

			_propertyList[_typeName][binder.Name].SetValue(_model, value, null);
			if (PropertyChanged != null)
				PropertyChanged(this, new PropertyChangedEventArgs(binder.Name));
			return true;
		}
	}
}
