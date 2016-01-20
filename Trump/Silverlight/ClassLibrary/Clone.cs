#define CLONE_JIM
//#define DEBUG_TRACE
//#define CLONE_ANGEL
//#define CLONE_KHASON
//#define EXTRA
using System;
using System.Collections;
using System.Reflection;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Animation;

namespace ClassLibrary
{
	internal static class CloneObject
	{
#if CLONE_JIM
		// Extension for any class object
		internal static TT Clone<TT>(this TT source)
		{
			return Clone<TT>(source, null/*attachedType*/);
		}

		internal static TT Clone<TT>(this TT source, Type attachedType)
		{ // Jim McCurdy's hybrid Clone
			if (source == null)
				return source;

			// Blacklisted types that can't (or don't need to be) cloned
			Type sourceType = source.GetType();
			if (sourceType.IsValueType ||
				(source is Type) ||
				(source is string) || 
				(source is Uri) ||
				(source is FontFamily) ||
				(source is XmlLanguage) ||
				(source is Cursor) ||
				(source is PropertyPath) ||
				false)
					return source;

			TT clone = default(TT);
			try
			{
				clone = (TT)Activator.CreateInstance(sourceType);
				//clone = (TT)typeof(TT).GetConstructor(BindingFlags.Instance | BindingFlags.FlattenHierarchy | BindingFlags.Public | BindingFlags.NonPublic/*include internal's*/, null, Type.EmptyTypes, null).Invoke(new object[] { });
#if DEBUG_TRACE
				string.Format("Clone object Type={0}", sourceType.ToString()).Trace();
#endif
			}
			catch (Exception ex)
			{
				string.Format("Can't clone object Type={0}", sourceType.ToString()).Trace();
				ex.DebugOutput();
				return source;
			}

			if (source is IEnumerable)
				CloneEnumerableObject(source, clone, sourceType, attachedType);

			CloneProperties(source, clone, sourceType, attachedType);

			if (clone is FrameworkElement)
			{
				FrameworkElement sourceElement = (source as FrameworkElement);
				FrameworkElement cloneElement = (clone as FrameworkElement);
				cloneElement.Arrange(new Rect(0, 0, sourceElement.ActualWidth, sourceElement.ActualHeight));
			}

			return clone;
		}

#if false
		// Gets a list of locally applied attached DPs on an element.
		public static List<DependencyProperty> GetAttachedProperties(Object element)
		{
			List<DependencyProperty> attachedProperties = new List<DependencyProperty>();
			MarkupObject markupObject = MarkupWriter.GetMarkupObjectFor(element);
			if (markupObject != null)
			{
				foreach (MarkupProperty mp in markupObject.Properties)
				{
					if (mp.IsAttached)
						attachedProperties.Add(mp.DependencyProperty);
				}
			}

			return attachedProperties;
		}
#endif
		private static void CloneProperties(object source, object clone, Type sourceType, Type attachedType)
		{
			// The binding flags indicate what properties we will clone
			// Unfortunately, we cannot clone "internal" or "protected" properties
			BindingFlags flags = BindingFlags.Instance | BindingFlags.FlattenHierarchy | BindingFlags.Public /*| BindingFlags.NonPublic*//*include internal's*/;

#if true //j Not a great solution for attached properties
			// Clone any attached dependency properties first
			Type loopType = attachedType;
			while (loopType != null && loopType != typeof(DependencyObject))
			{
				FieldInfo[] fieldInfoArray = loopType.GetFields(flags | BindingFlags.Static);
				foreach (FieldInfo field in fieldInfoArray)
					CloneDependencyProperty(source, clone, attachedType, field);

				loopType = loopType.BaseType;
			}
#endif
#if true //j Not sure we need to worry about dependency properties since should be handled by normal properties
			// Clone any dependency properties next
			loopType = source.GetType();
			while (loopType != null && loopType != typeof(DependencyObject))
			{
				FieldInfo[] fieldInfoArray = loopType.GetFields(flags | BindingFlags.Static);
				foreach (FieldInfo field in fieldInfoArray)
					CloneDependencyProperty(source, clone, attachedType, field);

				loopType = loopType.BaseType;
			}
#endif
			PropertyInfo[] properties = sourceType.GetProperties(flags);

			// Clone any enumerable properties next
			foreach (PropertyInfo property in properties)
			{
				bool isEnumerableProperty = (property.PropertyType.GetInterface("IList", true) != null);
				if (isEnumerableProperty)
					CloneEnumerableProperty(source, clone, sourceType, attachedType, property);
			}

			// Clone any normal properties last
			foreach (PropertyInfo property in properties)
			{
				bool isEnumerableProperty = (property.PropertyType.GetInterface("IList", true) != null);
				if (!isEnumerableProperty)
					CloneProperty(source, clone, sourceType, attachedType, property);
			}
		}

		private static void CloneProperty(object source, object clone, Type sourceType, Type attachedType, PropertyInfo property)
		{
			try
			{
				if (!property.CanRead || !property.CanWrite)
					return;

				if (property.GetIndexParameters().Length != 0)
					return;

				// Blacklisted properties that can't (or don't need to be) cloned
				Type propertyType = property.PropertyType;
				if (propertyType.IsArray ||
					(source is FrameworkElement && property.Name == "Name") ||
					(source is FrameworkElement && property.Name == "IsDataContextBound") || // internal
					(source is FrameworkElement && property.Name == "TagInternal") || // internal
					(source is ResourceDictionary && property.Name == "Source") || // is Uri
					(source is Control && property.Name == "ImplementationRoot") || // internal
					(source is UserControl && property.Name == "Content") || // protected
					(source is Control && property.Name == "DefaultStyleKey") || // protected
					false)
						return;

				object sourceObject = property.GetValue(source, null);
				if (sourceObject == null)
					return;
#if DEBUG_TRACE
				string.Format("Clone property Name={1}, Type={2}, Value={0} for source object Type={3}", sourceObject, property.Name, property.PropertyType.ToString(), sourceType.ToString()).Trace();
#endif
				object cloneObject = Clone(sourceObject, attachedType);
				property.SetValue(clone, cloneObject, null); // possible MethodAccessException
			}
			catch (Exception ex)
			{
				string.Format("Can't clone property Name={1}, Type={2} for source object Type={3}", null, property.Name, property.PropertyType.ToString(), sourceType.ToString()).Trace();
				ex.DebugOutput();
			}
		}

		private static void CloneEnumerableProperty(object source, object clone, Type sourceType, Type attachedType, PropertyInfo property)
		{
			try
			{
				object sourceObject = property.GetValue(source, null);
				if (sourceObject == null)
					return;
				Type sourceObjectType = sourceObject.GetType();
				object cloneObject = property.GetValue(clone, null);
				if (cloneObject == null)
					return;
#if DEBUG_TRACE
				string.Format("Clone enumerable property Name={1}, Type={2}, Value={0} for source object Type={3}", cloneObject, property.Name, property.PropertyType.ToString(), sourceType.ToString()).Trace();
#endif
				CloneEnumerableObject(sourceObject, cloneObject, sourceObjectType, attachedType);
			}
			catch (Exception ex)
			{
				string.Format("Can't clone enumerable property Name={1}, Type={2} for source object Type={3}", null, property.Name, property.PropertyType.ToString(), sourceType.ToString()).Trace();
				ex.DebugOutput();
			}
		}

		private static void CloneEnumerableObject(object source, object clone, Type sourceType, Type attachedType)
		{
			try
			{
				// Blacklisted enumerable objects that can't (or don't need to be) cloned
				if ((source is SetterBaseCollection) ||
					(source is ResourceDictionary) ||
					false)
						return;

				IEnumerator enumerator = (IEnumerator)sourceType.InvokeMember("GetEnumerator", BindingFlags.InvokeMethod, null, source, null);
				int i = 0;
				while ((bool)typeof(IEnumerator).InvokeMember("MoveNext", BindingFlags.InvokeMethod, null, enumerator, null))
				{
					try
					{
#if DEBUG_TRACE
						string.Format("Clone enumerable {0}", i).Trace();
#endif
						object sourceValue = typeof(IEnumerator).InvokeMember("get_Current", BindingFlags.InvokeMethod, null, enumerator, null);
						object cloneValue = Clone(sourceValue, attachedType);
						sourceType.InvokeMember("Add", BindingFlags.InvokeMethod, null, clone, new object[] { cloneValue });
						i++;
					}
					catch (Exception ex)
					{
						string.Format("Can't clone enumerable object Type={0}", sourceType.ToString()).Trace();
						ex.DebugOutput();
					}
				}
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
			}
		}

		private static void CloneDependencyProperty(object source0, object clone0, Type attachedType, FieldInfo field)
		{
			DependencyObject source = source0 as DependencyObject;
			DependencyObject clone = clone0 as DependencyObject;
			try
			{
				if (source == null || clone == null)
					return;

				if (field.IsPrivate || (field.Attributes & FieldAttributes.Private) != 0)
					return;

				if (!field.IsStatic || (field.Attributes & FieldAttributes.Static) == 0)
					return;

				// Blacklisted properties that can't (or don't need to be) cloned
				if (field.Name == "NameProperty" ||
					field.Name == "CountProperty" ||
					field.Name == "GeometryTransformProperty" ||
					field.Name == "ActualWidthProperty" ||
					field.Name == "ActualHeightProperty" ||
					//field.Name == "MaxWidthProperty" ||
					//field.Name == "MaxHeightProperty" ||
					//field.Name == "StyleProperty" ||
					//field.Name == "TemplateProperty" ||
					false)
					return;

				DependencyProperty dp = field.GetValue(source) as DependencyProperty;
				if (dp == null) // Events will be null
					return;

				//j Why?  Find a generic way to avoid this
				if (source is BackEase)
					return;

				object cloneObject = source.GetValue(dp);
				if (cloneObject == null)
					return;
#if DEBUG_TRACE
				string.Format("Clone dependency property Name={1}, Type={2}, Value={0} for source object Type={3}", cloneObject, field.Name, field.FieldType.ToString(), source.GetType().ToString()).Trace();
#endif
				if (cloneObject is DependencyObject)
					cloneObject = Clone((cloneObject as DependencyObject), attachedType);

				clone.SetValue(dp, cloneObject);
			}
			catch (Exception ex)
			{
				string.Format("Can't clone dependency property Name={1}, Type={2} for source object Type={3}", null, field.Name, field.FieldType.ToString(), source.GetType().ToString()).Trace();
				ex.DebugOutput();
			}
		}

#if DONT_NEED
		// Extension for any class object
		internal static void CloneListProperty<TT>(TT source, TT clone, PropertyInfo listProperty)
		{
			try
			{
				Type listPropertyType = listProperty.PropertyType;
				object listPropertyValue = listProperty.GetValue(source, null);
				if (listPropertyValue == null)
					return;
				object cloneListPropertyValue = listProperty.GetValue(clone, null);
				if (cloneListPropertyValue == null)
				{
					cloneListPropertyValue = Activator.CreateInstance(listPropertyType);
					listProperty.SetValue(clone, cloneListPropertyValue, null);
				}

				int count = (int)listPropertyType.InvokeMember("get_Count", BindingFlags.InvokeMethod, null, listPropertyValue, null);
				for (int i = 0; i < count; i++)
				{
					try
					{
						// get property from the collection
						object propertyValue = listPropertyType.InvokeMember("get_Item", BindingFlags.InvokeMethod, null, listPropertyValue, new object[] { i });
						if (propertyValue == null)
							continue;
						propertyValue = Clone(propertyValue);
						listPropertyType.InvokeMember("Add", BindingFlags.InvokeMethod, null, cloneListPropertyValue, new object[] { propertyValue });
					}
					catch (Exception ex)
					{
						e.DebugOutput();
					}
				}
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
			}
		}
#endif

#elif CLONE_ANGEL
		internal static TT Clone<TT>(this TT source)
		{ // Justin Angel's Clone
			TT cloned = (TT)Activator.CreateInstance(source.GetType());
			foreach (PropertyInfo curPropInfo in source.GetType().GetProperties())
			{
				if (curPropInfo.GetGetMethod() == null || curPropInfo.GetSetMethod() == null)
					continue;

				// Handle Non-indexer properties
				if (curPropInfo.Name != "Item")
				{
					// get property from source
					object getValue = curPropInfo.GetGetMethod().Invoke(source, new object[] { });

					// clone if needed
					if (getValue != null && getValue is DependencyObject)
						getValue = Clone((DependencyObject)getValue);

					// set property on cloned
					if (getValue != null)
						curPropInfo.GetSetMethod().Invoke(cloned, new object[] { getValue });
				}
				// handle indexer
				else
				{
					// get count for indexer
					int numberofItemInColleciton = (int)
						curPropInfo.ReflectedType.GetProperty("Count").GetGetMethod().Invoke(source, new object[] { });

					// run on indexer
					for (int i = 0; i < numberofItemInColleciton; i++)
					{
						// get item through Indexer
						object getValue = curPropInfo.GetGetMethod().Invoke(source, new object[] { i });

						// clone if needed
						if (getValue != null && getValue is DependencyObject)
							getValue = Clone((DependencyObject)getValue);

						// add item to collection
						if (getValue != null)
							curPropInfo.ReflectedType.GetMethod("Add").Invoke(cloned, new object[] { getValue });
					}
				}
			}

			return cloned;
		}
#elif CLONE_KHASON
		// Extension for any class object
		internal static TT Clone<TT>(this TT source) where TT : DependencyObject
		{ // Tamir Khason's Clone
			Type type = source.GetType();
			TT cloned = (TT)Activator.CreateInstance(type);

			Type wtype = type;
			while (wtype.BaseType != typeof(DependencyObject))
			{
				FieldInfo[] fieldInfo = wtype.GetFields(BindingFlags.Static | BindingFlags.Public);
				for (int i = 0; i < fieldInfo.Length; i++)
				{
					DependencyProperty property = fieldInfo[i].GetValue(source) as DependencyProperty;
					if (property == null || fieldInfo[i].Name == "NameProperty")
						continue;

					DependencyObject obj = source.GetValue(property) as DependencyObject;
					if (obj != null)
					{
						object o = Clone(obj);
						cloned.SetValue(property, o);
					}
					else
					{
						if (fieldInfo[i].Name != "CountProperty" &&
							fieldInfo[i].Name != "GeometryTransformProperty" &&
							fieldInfo[i].Name != "ActualWidthProperty" &&
							fieldInfo[i].Name != "ActualHeightProperty" &&
							fieldInfo[i].Name != "MaxWidthProperty" &&
							fieldInfo[i].Name != "MaxHeightProperty" &&
							fieldInfo[i].Name != "StyleProperty")
						{
							cloned.SetValue(property, source.GetValue(property));
						}
					}
				}

				wtype = wtype.BaseType;
			}

			foreach (PropertyInfo propertyInfo in type.GetProperties())
			{
				if (propertyInfo.Name != "Name" &&
					propertyInfo.Name != "Parent" &&
					propertyInfo.CanRead && 
					propertyInfo.CanWrite &&
					!propertyInfo.PropertyType.IsArray &&
					!propertyInfo.PropertyType.IsSubclassOf(typeof(DependencyObject)) &&
					propertyInfo.GetIndexParameters().Length == 0 &&
					propertyInfo.GetValue(source, null) != null &&
					propertyInfo.GetValue(source, null) == (object)default(int) &&
					propertyInfo.GetValue(source, null) == (object)default(double) &&
					propertyInfo.GetValue(source, null) == (object)default(float))
						propertyInfo.SetValue(cloned, propertyInfo.GetValue(source, null), null);
				else
				if (propertyInfo.PropertyType.GetInterface("IList", true) != null)
				{
					int count = (int)propertyInfo.PropertyType.InvokeMember("get_Count", BindingFlags.InvokeMethod, null, propertyInfo.GetValue(source, null), null);
					for (int c = 0; c < count; c++)
					{
						object val = propertyInfo.PropertyType.InvokeMember("get_Item", BindingFlags.InvokeMethod, null, propertyInfo.GetValue(source, null), new object[] { c });
						object nVal = val;
						DependencyObject obj2 = val as DependencyObject;
						if (obj2 != null)
							nVal = Clone(obj2);
						if (propertyInfo.GetValue(cloned, null) == null)
						{
							object obj = Activator.CreateInstance(propertyInfo.PropertyType);
							propertyInfo.SetValue(cloned, obj, null);
						}

						propertyInfo.PropertyType.InvokeMember("Add", BindingFlags.InvokeMethod, null, propertyInfo.GetValue(cloned, null), new object[] { nVal });
					}
				}
			}

			return cloned;
		}

#elif EXTRA
		internal static T Clone<T>(this T source) where T : DependencyObject
		{
			T clone = (T)Activator.CreateInstance(source.GetType());
			CloneAllDependencyProperties<T>(source, clone);
			CloneAllProperties<T>(source, clone);
			return clone;
		}

		private static void CloneAllProperties<T>(T source, T clone) where T : DependencyObject
		{
			Type sourceType = source.GetType();
			foreach (PropertyInfo property in sourceType.GetProperties())
			{
				Type propertyType = property.PropertyType;

				if (property.CanRead && property.CanWrite &&
					property.Name != "Name" &&
					!propertyType.IsArray &&
					property.GetIndexParameters().Length == 0 &&
					property.GetValue(source, null) != null)
				{
					try
					{
						property.SetValue(clone, property.GetValue(source, null), null);
					}
					catch (Exception ex)
					{ // could be TargetInvocationException or MethodAccessException
						ex.DebugOutput();
					}
				}
				else
				if (propertyType.GetInterface("IList", true) != null)
				{
					int count = (int)propertyType.InvokeMember("get_Count", BindingFlags.InvokeMethod, null, property.GetValue(source, null), null);
					for (int i = 0; i < count; i++)
					{
						try
						{
							object propertyValue = propertyType.InvokeMember("get_Item", BindingFlags.InvokeMethod, null, property.GetValue(source, null), new object[] { i });
							if (propertyValue is DependencyObject)
								propertyValue = Clone(propertyValue as DependencyObject);

							propertyType.InvokeMember("Add", BindingFlags.InvokeMethod, null, property.GetValue(clone, null), new object[] { propertyValue });
						}
						catch (TargetInvocationException ex)
						{ // could be TargetInvocationException
							// Some properties (e.g. TemplateForm) cannot be cloned because of misssing setters.
							ex.DebugOutput();
						}
					}
				}
			}
		}

		private static void CloneAllDependencyProperties<T>(T source, T clone) where T : DependencyObject
		{
			Type type = source.GetType();
			while (type.BaseType != typeof(DependencyObject))
			{
				FieldInfo[] fi = type.GetFields(BindingFlags.Static | BindingFlags.Public);
				for (int i = 0; i < fi.Length; i++)
				{
					DependencyProperty dp = fi[i].GetValue(source) as DependencyProperty;
					if (dp != null && fi[i].Name != "NameProperty")
					{
						DependencyObject obj = source.GetValue(dp) as DependencyObject;
						if (obj != null)
						{
							object o = Clone(obj);
							clone.SetValue(dp, o);
						}
						else
						{
							if (fi[i].Name != "CountProperty" &&
								fi[i].Name != "GeometryTransformProperty" &&
								fi[i].Name != "ActualWidthProperty" &&
								fi[i].Name != "ActualHeightProperty" &&
								fi[i].Name != "MaxWidthProperty" &&
								fi[i].Name != "MaxHeightProperty" &&
								fi[i].Name != "StyleProperty" &&
								fi[i].Name != "TemplateProperty")
							{
								try
								{
									clone.SetValue(dp, source.GetValue(dp));
								}
								catch (InvalidOperationException ex)
								{
									// TemplateControl of a user control cannot be set: 
									// http://msdn.microsoft.com/en-us/library/system.windows.controls.usercontrol(VS.95).aspx
									ex.DebugOutput();
									throw;
								}
							}
						}
					}
				}

				type = type.BaseType;
			}
		}
#endif
	}
}
