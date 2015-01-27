using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Linq.Expressions;
using System.Reflection;
using System.Windows;

// Written by: Jim McCurdy (jmccurdy@facetofacesoftware.com)

namespace ClassLibrary
{
	[AttributeUsage(AttributeTargets.Property, AllowMultiple = true)]
	public class DependsOnAttribute : Attribute
	{
		public string DependsOnName { get; private set; }

		public DependsOnAttribute(string propertyName)
		{
			DependsOnName = propertyName;
		}
	}

	public class DependsOnItem
	{
		public string PropertyName;
		public string DependsOnName;
		public bool Ignore;
	}

	// For use as a member variable
	public class ViewModelImpl<TT> : ViewModelBase
	{
		public void Initialize(PropertyChangedEventHandler forwardPropertyChanged)
		{
			_BaseType = typeof(TT);
			base.PropertyChanged += delegate(object sender, PropertyChangedEventArgs e)
			{
				forwardPropertyChanged(sender, e);
			};
		}
	}

	// For use as a base class
	public abstract class ViewModelBase : INotifyPropertyChanged, IDisposable
	{
		public event PropertyChangedEventHandler PropertyChanged;
		protected Type _BaseType { get; set; }
		private List<DependsOnItem> _DependsOnItems;

		public ViewModelBase()
		{
			_BaseType = base.GetType();
		}

		public virtual void Dispose()
		{
			PropertyChanged = null;
		}

		public bool SetProperty<TValue, TSetter>(ref TValue oldValue, TValue newValue, Expression<Func<TSetter>> setter, Action<TValue> successAction = null, Action<TValue> validateAction = null)
		{
			string propertyName = ((MemberExpression)setter.Body).Member.Name;
			return SetPropertyInternal<TValue>(ref oldValue, newValue, propertyName, successAction, validateAction);
		}

		public bool SetProperty<TValue>(ref TValue oldValue, TValue newValue, string setterName)
		{
			return SetPropertyInternal(ref oldValue, newValue, GetPropertyName(setterName), null/*successAction*/, null/*validateAction*/);
		}

		//[MethodImpl(MethodImplOptions.NoInlining)]
		public bool SetProperty<TValue>(ref TValue oldValue, TValue newValue, string setterName, Action<TValue> successAction)
		{
			return SetPropertyInternal(ref oldValue, newValue, GetPropertyName(setterName), successAction, null/*validateAction*/);
		}

		//[MethodImpl(MethodImplOptions.NoInlining)]
		public bool SetProperty<TValue>(ref TValue oldValue, TValue newValue, string setterName, Action<TValue> successAction, Action<TValue> validateAction)
		{
			return SetPropertyInternal(ref oldValue, newValue, GetPropertyName(setterName), successAction, validateAction);
		}

		private bool SetPropertyInternal<TValue>(ref TValue oldValue, TValue newValue, string propertyName, Action<TValue> successAction, Action<TValue> validateAction)
		{
			if (oldValue == null && newValue == null)
				return false;
			if (oldValue != null && oldValue.Equals(newValue))
				return false;

			if (validateAction != null)
				validateAction(newValue);

			if (oldValue is IDisposable)
				(oldValue as IDisposable).Dispose();
			oldValue = newValue;

			if (PropertyChanged != null)
				RaisePropertyChanged(propertyName);

			if (successAction != null)
				successAction(newValue);

			return true;
		}

		private string GetPropertyName(string setterName)
		{
			if (setterName != null && setterName.StartsWith("set_"))
				return setterName.Substring(4);
			throw new MemberAccessException(string.Format("GetPropertyName can't access setter '{0}'", setterName));
		}

		//[MethodImpl(MethodImplOptions.NoInlining)]
		//private string GetPropertyName(int stackDepth)
		//{
		//    // If there are problems with the StackFrame approach below, use:
		//    // string setterName = MethodBase.GetCurrentMethod().Name;
		//    MethodBase setter = (new StackFrame(stackDepth)).GetMethod();
		//    return GetPropertyName(setter.Name);
		//}

		private void RaisePropertyChanged(string propertyName)
		{
			// Check if we are on the UI thread; if not switch
			if (!Deployment.Current.Dispatcher.CheckAccess())
			{
				Deployment.Current.Dispatcher.BeginInvoke(delegate() { RaisePropertyChanged(propertyName); });
				return;
			}

			PropertyChanged(this, new PropertyChangedEventArgs(propertyName));

			// One time creation
			if (_DependsOnItems == null)
				_DependsOnItems = GetDependsOnItems().ToList();

			if (_DependsOnItems.Count() > 0)
			{
				foreach (DependsOnItem item in _DependsOnItems)
					item.Ignore = (item.PropertyName == propertyName);
				RaiseDependsOnPropertyChanged(propertyName);
			}
		}

		private void RaiseDependsOnPropertyChanged(string propertyName)
		{
			foreach (DependsOnItem item in _DependsOnItems)
			{
				if (item.Ignore || item.DependsOnName != propertyName)
					continue;

				PropertyChanged(this, new PropertyChangedEventArgs(item.PropertyName));
				// Set ignore on this item before we call RaiseDependsOnPropertyChanged recursively
				item.Ignore = true;
				RaiseDependsOnPropertyChanged(item.PropertyName);
			}
		}

		private IEnumerable<DependsOnItem> GetDependsOnItems()
		{
			foreach (PropertyInfo property in _BaseType.GetProperties())
			{
				object[] attributes = property.GetCustomAttributes(typeof(DependsOnAttribute), true);
				foreach (DependsOnAttribute attribute in attributes)
					yield return new DependsOnItem() { PropertyName = property.Name, DependsOnName = attribute.DependsOnName };
			}
		}
	}
}
