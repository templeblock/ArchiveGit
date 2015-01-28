using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Dynamic;
using System.Linq;
using System.Reflection;
using System.Threading;

namespace ClassLibrary
{
	public sealed class DynamicViewModel<TModel> : DynamicObject, INotifyPropertyChanged where TModel : class
	{ // http://www.codeproject.com/Articles/139630/MVVM-using-POCOs-with-NET-4-0-and-the-DynamicViewM.aspx

		public event PropertyChangedEventHandler PropertyChanged;
		private readonly TModel _model;
		private static readonly IDictionary<string, MethodInfo> _methodList = GetMethodList();
		private static readonly IDictionary<string, PropertyInfo> _propertyList = GetPropertyList();
		private IDictionary<string, object> _propertyValues;

		public DynamicViewModel(TModel model)
		{
			_model = model;
			NotifyChangedProperties();
		}

		public DynamicViewModel(Func<TModel> @delegate) : this(@delegate.Invoke())
		{
		}

		public override Boolean TryInvokeMember(InvokeMemberBinder binder, object[] args, out object result)
		{
			result = null;

			MethodInfo methodInfo;
			if (!_methodList.TryGetValue(binder.Name, out methodInfo))
				return false;

			methodInfo.Invoke(_model, args);
			NotifyChangedProperties();
			return true;
		}

		public override Boolean TryGetMember(GetMemberBinder binder, out object result)
		{
			var propertyValues = Interlocked.CompareExchange(ref _propertyValues, GetPropertyValues(), null);
			return propertyValues.TryGetValue(binder.Name, out result);
		}

		public override Boolean TrySetMember(SetMemberBinder binder, object value)
		{
			PropertyInfo propInfo = _propertyList[binder.Name];
			propInfo.SetValue(_model, value, null);
			NotifyChangedProperties();
			return true;
		}

		/// Setting a property sometimes results in multiple properties with changed values too. 
		/// For ex.: By changing the FirstName and the LastName the FullName will get updated. 
		/// This method compares the _propertyValues dictionary with the one that is obtained inside this 
		/// method body. For each changed prop the PropertyChanged event is raised, notifying the callers.
		private void NotifyChangedProperties()
		{
			Interlocked.CompareExchange(ref _propertyValues, GetPropertyValues(), null);
			IDictionary<string, object> previousPropValues = _propertyValues;
			IDictionary<string, object> currentPropValues = GetPropertyValues();

			// Since we will be raising the PropertyChanged event we want the caller to bind in the current 
			// values and not the previous.
			_propertyValues = currentPropValues;

			foreach (KeyValuePair<string, object> propValue in currentPropValues.Except(previousPropValues))
			{
				PropertyChangedEventHandler handler = Interlocked.CompareExchange(ref PropertyChanged, null, null);
				if (handler != null)
					handler(this, new PropertyChangedEventArgs(propValue.Key));
			}
		}

		private static IDictionary<string, MethodInfo> GetMethodList()
		{
			var methodList = new Dictionary<string, MethodInfo>();
			MethodInfo[] methods = typeof(TModel).GetMethods(BindingFlags.Public | BindingFlags.Instance);
			foreach (MethodInfo method in methods)
			{
				if (!method.Name.StartsWith("get_") && !method.Name.StartsWith("set_"))
					methodList.Add(method.Name, method);
			}

			return methodList;
		}

		private static IDictionary<string, PropertyInfo> GetPropertyList()
		{
			var propertyList = new Dictionary<string, PropertyInfo>();
			PropertyInfo[] properties = typeof(TModel).GetProperties(BindingFlags.Public | BindingFlags.Instance);
			foreach (PropertyInfo property in properties)
			{
				propertyList.Add(property.Name, property);
			}

			return propertyList;
		}

		private IDictionary<string, object> GetPropertyValues()
		{
			var propertyValues = new Dictionary<string, object>();
			PropertyInfo[] properties = typeof(TModel).GetProperties(BindingFlags.Public | BindingFlags.Instance);
			foreach (PropertyInfo property in properties)
			{
				propertyValues.Add(property.Name, property.GetValue(_model, null));
			}

			return propertyValues;
		}
	}
}
