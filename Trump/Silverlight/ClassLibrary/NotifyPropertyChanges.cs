using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Windows;

namespace ClassLibrary
{
#if TEST_DEPENDON_ATTRIBUTE //j
	// [DependsOn("")] Usage: http://houseofbilz.com/archive/2009/11/14/adventures-in-mvvm----dependant-properties-with-inotifypropertychanged.aspx
	public class TestCase : NotifyPropertyChanges
	{
		[DependsOn("D")]
		public int A { get { return D * 2; } set { D = value / 2; } }

		[DependsOn("C")]
		public int B { get { return C * 2; } set { C = value / 2; } }

		[DependsOn("A")]
		public int C { get { return A * 2; } set { A = value / 2; } }

		[DependsOn("D")] // just to test that no infinite loops result
		public int D { get { return m_D; } set { SetProperty(ref m_D, value); } }
		private int m_D;

		[DependsOn("A")]
		[DependsOn("B")]
		public int E { get { return A * B; } }

		public static void StaticTest()
		{
			TestCase test = new TestCase();
			test.PropertyChanged += delegate(object sender, PropertyChangedEventArgs e)
			{
				e.PropertyName.Trace();
			};

			"A Changing".Trace();
			test.A = 123;
			"B Changing".Trace();
			test.B = 234;
			"C Changing".Trace();
			test.C = 345;
			"D Changing".Trace();
			test.D = 456;
			"Done".Trace();
		}
	}
#endif

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
	public class NotifyPropertyChangesImpl : NotifyPropertyChanges
	{
		public void Forward(PropertyChangedEventHandler handler)
		{
			m_BaseType = handler.Target.GetType();
			base.PropertyChanged += delegate(object sender, PropertyChangedEventArgs e)
			{
				handler(this, e);
			};
		}
	}

	// For use as a base class
	//j	public abstract class ViewModelBase : INotifyPropertyChanged
	public abstract class NotifyPropertyChanges : INotifyPropertyChanged, IDisposable
	{
		public event PropertyChangedEventHandler PropertyChanged;
		protected Type m_BaseType { get; set; }
		private List<DependsOnItem> m_DependsOnItems;

		public NotifyPropertyChanges()
		{
			m_BaseType = base.GetType();
		}

		public virtual void Dispose()
		{
			PropertyChanged = null;
		}

		public bool SetProperty<TValue>(ref TValue oldValue, TValue newValue)
		{
			return SetProperty(ref oldValue, newValue, 1, null/*successHandler*/, null/*validateHandler*/);
		}

		public bool SetProperty<TValue>(ref TValue oldValue, TValue newValue, Action<TValue> successHandler)
		{
			return SetProperty(ref oldValue, newValue, 1, successHandler, null/*validateHandler*/);
		}

		public bool SetProperty<TValue>(ref TValue oldValue, TValue newValue, Action<TValue> successHandler, Action<TValue> validateHandler)
		{
			return SetProperty(ref oldValue, newValue, 1, successHandler, validateHandler);
		}

		private bool SetProperty<TValue>(ref TValue oldValue, TValue newValue, int stackDepth, Action<TValue> successHandler, Action<TValue> validateHandler)
		{
			if (oldValue == null && newValue == null)
				return false;
			if (oldValue != null && oldValue.Equals(newValue))
				return false;

			if (validateHandler != null)
				validateHandler(newValue);

			if (oldValue is IDisposable)
				(oldValue as IDisposable).Dispose();
			oldValue = newValue;

			stackDepth++;
			if (PropertyChanged != null)
				RaisePropertyChanged(stackDepth);

			if (successHandler != null)
				successHandler(newValue);

			return true;
		}

		private void RaisePropertyChanged(int stackDepth)
		{
			// Check if we are on the UI thread; if not switch
			if (!Deployment.Current.Dispatcher.CheckAccess())
			{
				Deployment.Current.Dispatcher.BeginInvoke(delegate() { RaisePropertyChanged(stackDepth); });
				return;
			}

			// This works because the derived class's setter calls SetProperty<TValue>() above,
			// who calls this method - therefore the setter is 3 back on the call stack
			// If there are problems with the StackFrame approach below,
			// have the setter pass through MethodBase.GetCurrentMethod()
			stackDepth++;
			MethodBase setter = new StackFrame(stackDepth).GetMethod();
			string propertyName = GetPropertyName(setter.Name);
			PropertyChanged(this, new PropertyChangedEventArgs(propertyName));

			// One time creation
			if (m_DependsOnItems == null)
				m_DependsOnItems = GetDependsOnItems().ToList();

			if (m_DependsOnItems.Count() > 0)
			{
				foreach (DependsOnItem item in m_DependsOnItems)
					item.Ignore = (item.PropertyName == propertyName);
				RaiseDependsOnPropertyChanged(propertyName);
			}
		}

		private void RaiseDependsOnPropertyChanged(string propertyName)
		{
			foreach (DependsOnItem item in m_DependsOnItems)
			{
				if (!item.Ignore && item.DependsOnName == propertyName)
				{
					PropertyChanged(this, new PropertyChangedEventArgs(item.PropertyName));
					// Set ignore on this item before we call RaiseDependsOnPropertyChanged recursively
					item.Ignore = true;
					RaiseDependsOnPropertyChanged(item.PropertyName);
				}
			}
		}

		private IEnumerable<DependsOnItem> GetDependsOnItems()
		{
			foreach (PropertyInfo property in m_BaseType.GetProperties())
			{
				object[] attributes = property.GetCustomAttributes(typeof(DependsOnAttribute), true);
				foreach (DependsOnAttribute attribute in attributes)
					yield return new DependsOnItem() { PropertyName = property.Name, DependsOnName = attribute.DependsOnName };
			}
		}

		private string GetPropertyName(string setterName)
		{
			if (setterName.StartsWith("set_"))
				return setterName.Substring(4);
			throw new MemberAccessException(setterName);
		}
	}
}
