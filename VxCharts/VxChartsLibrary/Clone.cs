//#define DEBUG_TRACE
//#define TEST
using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Resources;
#if TEST
using System.Collections.Generic;
using System.Collections.ObjectModel;
#endif

namespace ClassLibrary
{
	public static class CloneObject
	{
		private static List<FieldInfo> _attachedProperties = null;

		// Extension for any class object
		internal static TT DeepClone<TT>(this TT source, bool? cloneAttachedProperties = null)
		{ // Jim McCurdy's DeepClone
			if (cloneAttachedProperties == null)
				cloneAttachedProperties = source is DependencyObject; 

			// The first time this method is called, compute a list of all
			// attached properties that exist in this XAP's assemblies
			if (cloneAttachedProperties == true && _attachedProperties == null)
			{
				_attachedProperties = new List<FieldInfo>();
				List<Assembly> assemblies = GetLoadedAssemblies();
				foreach (Assembly assembly in assemblies)
					GetAttachedProperties(_attachedProperties, assembly);
			}

			TT clone = CloneRecursive(source);

			if (clone is FrameworkElement)
			{
				FrameworkElement cloneElement = (clone as FrameworkElement);
				cloneElement.Arrange(new Rect(0, 0, cloneElement.ActualWidth, cloneElement.ActualHeight));
			}

			return clone;
		}

		private static TT CloneRecursive<TT>(TT source)
		{
			if (source == null || source.GetType().IsValueType)
				return source;

			// Common types that do not have parameterless constructors
			if (source is string || source is Type || source is Uri || source is DependencyProperty)
				return source;

			TT clone = CloneCreate(source);
			if (clone == null)
				return source;

			if (source is IList)
				CloneList(source as IList, clone as IList);

			CloneProperties(source, clone);

			return clone;
		}

		private static TT CloneCreate<TT>(TT source)
		{
			try
			{
#if DEBUG_TRACE
				string.Format("Clone create object Type={0}", SimpleType(source.GetType())).Trace();
#endif
				Array sourceArray = (source as Array);
				if (sourceArray == null)
					return (TT)Activator.CreateInstance(source.GetType());
				if (sourceArray.Rank == 1)
					return (TT)(object)Array.CreateInstance(source.GetType().GetElementType(),
						sourceArray.GetLength(0));
				if (sourceArray.Rank == 2)
					return (TT)(object)Array.CreateInstance(source.GetType().GetElementType(),
						sourceArray.GetLength(0), sourceArray.GetLength(1));
			}
			catch (Exception ex)
			{
				if (ex.Message.Contains("No parameterless constructor"))
					return default(TT);
				string.Format("Can't create object Type={0}", source.GetType().ShortName()).Trace();
				ex.DebugOutput(true);
			}

			return default(TT);
		}

		private static void CloneProperties(object source, object clone)
		{
			// The binding flags indicate what properties we will clone
			// Unfortunately, we cannot clone "internal" or "protected" properties
			BindingFlags flags = 
				BindingFlags.Instance | BindingFlags.FlattenHierarchy | BindingFlags.Public;

			if (source is DependencyObject)
			{
				DependencyObject sourcedp = source as DependencyObject;
				DependencyObject clonedp = clone as DependencyObject;

				// Clone attached properties
				if (_attachedProperties != null && _attachedProperties.Count > 0)
					foreach (FieldInfo field in _attachedProperties)
						CloneDependencyProperty(sourcedp, clonedp, field, true);

				// Clone dependency properties
				FieldInfo[] fields = source.GetType().GetFields(flags | BindingFlags.Static);
				foreach (FieldInfo field in fields)
					CloneDependencyProperty(sourcedp, clonedp, field, false);
			}

			// Clone CLR properties
			PropertyInfo[] properties = source.GetType().GetProperties(flags);
			foreach (PropertyInfo property in properties)
				CloneProperty(source, clone, property);
		}

		private static void CloneDependencyProperty(DependencyObject sourceObject, 
			DependencyObject cloneObject, FieldInfo field, bool isAttached)
		{
			try
			{
				// Blacklisted properties that can't (or shouldn't) be set
				if (field.Name == "NameProperty" && sourceObject is FrameworkElement) return;

				DependencyProperty dp = field.GetValue(sourceObject) as DependencyProperty;
				if (dp == null) // Event DependencyProperties will be null
					return;

				object sourceValue = null;
				try
				{
					sourceValue = sourceObject.GetValue(dp);
				}
				catch (Exception)
				{
				}

				if (sourceValue == null)
					return;
				// Don't set attached properties if we don't have to
				if (isAttached)
				{
					Type sourceType = sourceValue.GetType();
					if (sourceType.IsValueType && sourceValue.Equals(Activator.CreateInstance(sourceType)))
						return;
				}
#if DEBUG_TRACE
				string.Format("Clone dependency property Name={0}, Value={2} for source Type={1}", 
					field.Name, SimpleType(sourceObject.GetType()), sourceValue).Trace();
#endif
				// Blacklisted properties that can't (or don't need to be) cloned
				bool doClone = true;
				if (field.Name == "DataContextProperty") doClone = false;
				//if (field.Name == "TargetPropertyProperty") doClone = false;

				object cloneValue = (doClone ? CloneRecursive(sourceValue) : sourceValue);
				cloneObject.SetValue(dp, cloneValue);
			}
			catch (Exception ex)
			{
				if (ex.Message.Contains("read-only"))
					return;
				if (ex.Message.Contains("read only"))
					return;
				if (ex.Message.Contains("does not fall within the expected range"))
					return;
				string.Format("Can't clone dependency property Name={0}, for source Type={1}",
					field.Name, sourceObject.GetType().ShortName()).Trace();
				ex.DebugOutput(true);
			}
		}

		private static void CloneProperty(object source, object clone, PropertyInfo property)
		{
			try
			{
				if (!property.CanRead || !property.CanWrite || property.GetIndexParameters().Length != 0)
					return;

				// Blacklisted properties that can't (or shouldn't) be set
				if (property.Name == "Name" && source is FrameworkElement) return;
				if (property.Name == "InputScope" && source is TextBox) return; // Can't get
				if (property.Name == "Watermark" && source is TextBox) return; // Can't get
				if (property.Name == "Source" && source is ResourceDictionary) return; // Can't set
				if (property.Name == "TargetType" && source is ControlTemplate) return; // Can't set

				bool publicSetter = (source.GetType().GetMethod("set_" + property.Name) != null);
				bool isList = (property.PropertyType.GetInterface("IList", true) != null);
				if (!publicSetter && !isList)
					return;

				object sourceValue = property.GetValue(source, null);
				if (sourceValue == null)
					return;

				if (!publicSetter && isList)
				{
					IList cloneList = property.GetValue(clone, null) as IList;
					if (cloneList != null)
						CloneList(sourceValue as IList, cloneList);
					return;
				}
#if DEBUG_TRACE
				string.Format("Clone property Type={0}, Name={1}, Value={3} for source Type={2}", 
					SimpleType(property.PropertyType), property.Name, SimpleType(source.GetType()), 
					sourceValue).Trace();
#endif
				// Blacklisted properties that can't (or don't need to be) cloned
				bool doClone = true;
				if (source is FrameworkElement && property.Name == "DataContext") doClone = false;
				//if (property.Name == "TargetProperty") doClone = false;

				object cloneValue = (doClone ? CloneRecursive(sourceValue) : sourceValue);
				property.SetValue(clone, cloneValue, null); // possible MethodAccessException
			}
			catch (Exception ex)
			{
				string.Format("Can't clone property Type={0}, Name={1}, for source Type={2}",
					property.PropertyType.ShortName(), property.Name, source.GetType().ShortName()).Trace();
				ex.DebugOutput(true);
			}
		}

		private static void CloneList(IList sourceList, IList cloneList)
		{
			try
			{
				IEnumerator sourceEnumerator = sourceList.GetEnumerator();
				Array sourceArray = sourceList as Array;
				Array cloneArray = cloneList as Array;
				int dim0 = (sourceArray != null && sourceArray.Rank > 0 ? sourceArray.GetLowerBound(0) : 0);
				int dim1 = (sourceArray != null && sourceArray.Rank > 1 ? sourceArray.GetLowerBound(1) : 0);

				while (sourceEnumerator.MoveNext())
				{
					object sourceValue = sourceEnumerator.Current;
#if DEBUG_TRACE
					string.Format("Clone IList item {0}", sourceValue).Trace();

#endif
					object cloneValue = CloneRecursive(sourceValue);
					if (sourceArray == null)
						cloneList.Add(cloneValue);
					else
					if (sourceArray.Rank == 1)
						cloneArray.SetValue(cloneValue, dim0++);
					else
					if (sourceArray.Rank == 2)
					{
						cloneArray.SetValue(cloneValue, dim0, dim1);
						if (++dim1 > sourceArray.GetUpperBound(1))
						{
							dim1 = sourceArray.GetLowerBound(1);
							if (++dim0 > sourceArray.GetUpperBound(0))
								dim0 = sourceArray.GetLowerBound(0);
						}
					}
				}
			}
			catch (Exception ex)
			{
				string.Format("Can't clone IList item Type={0}", sourceList.GetType().ShortName()).Trace();
				ex.DebugOutput(true);
			}
		}
#if TEST
		private class Testing
		{
			//private string[] dim1 = new string[] { "one", "two" };
			//public string[] Dim1 { get { return dim1; } set { dim1 = value; } }

			//private int[,] dim2 = new int[,] { { 1, 2, 3 }, { 4, 5, 6 } };
			//public int[,] Dim2 { get { return dim2; } set { dim2 = value; } }

			//private int[][] jagged = new int[][] { new int[] { 1, 2, 3 }, new int[] { 4, 5, 6 } };
			//public int[][] Jagged { get { return jagged; } set { jagged = value; } }

			//List<string> list = new List<string>();
			//public List<string> List { get { return list; } set { list = value; } }

			//Collection<string> coll = new Collection<string>();
			//public Collection<string> Coll { get { return coll; } set { coll = value; } }

			//public Testing()
			//{
			//    list.Add("list one");
			//    list.Add("list two");
			//    coll.Add("coll one");
			//    coll.Add("coll two");
			//}
		}

		private static bool _test = Test();

		internal static bool Test()
		{
			//List<string> list = new List<string>();
			//list.Add("list one");
			//list.Add("list two");
			//object listclone = list.DeepClone();

			//Collection<string> coll = new Collection<string>();
			//coll.Add("coll one");
			//coll.Add("coll two");
			//object collclone = list.DeepClone();

			//Testing testing = new Testing();
			//object testingclone = testing.DeepClone();

			//string[] dim1 = new string[] { "one", "two" };
			//object dim1clone = dim1.DeepClone();

			//int[,] dim2 = new int[,] { { 1, 2, 3 }, { 4, 5, 6 } };
			//object dim2clone = dim2.DeepClone();

			//int[][] jagged = new int[][] { new int[] { 1, 2, 3 }, new int[] { 4, 5, 6 } };
			//object jaggedclone = jagged.DeepClone();

			//object root = Application.Current.RootVisual;
			//object rootclone = root.DeepClone();
			return true;
		}
#endif
		private static List<Assembly> GetLoadedAssemblies()
		{
			List<Assembly> assemblies = new List<Assembly>();

			foreach (AssemblyPart part in Deployment.Current.Parts)
			{
				StreamResourceInfo sri = 
					Application.GetResourceStream(new Uri(part.Source, UriKind.Relative));
				if (sri == null)
					continue;
				Assembly assembly = new AssemblyPart().Load(sri.Stream);
				if (assembly != null && !assemblies.Contains(assembly))
					assemblies.Add(assembly);
			}

			// Additional assemblies that are not found when examining of Deployment.Current.Parts above
			Type[] types =
			{
				typeof(System.Windows.Application), // System.Windows.dll,
				#if INCLUDE_ASSEMBLIES_WITHOUT_ATTACHED_PROPERTIES
				typeof(System.Action), // mscorlib.dll,
				typeof(System.Uri), // System.dll,
				typeof(System.Lazy<int>), // System.Core.dll,
				typeof(System.Net.Cookie), // System.Net.dll,
				typeof(System.Runtime.Serialization.StreamingContext), // System.Runtime.Serialization.dll,
				typeof(System.ServiceModel.XmlSerializerFormatAttribute), // System.ServiceModel.dll,
				typeof(System.Windows.Browser.BrowserInformation), // System.Windows.Browser.dll,
				typeof(System.Xml.ConformanceLevel), // System.Xml.dll,
				#endif
			};

			foreach (Type type in types)
			{
				Assembly assembly = type.Assembly;
				if (assembly != null && !assemblies.Contains(assembly))
					assemblies.Add(assembly);
			}

			return assemblies;
		}

		private static bool GetAttachedProperties(List<FieldInfo> attachedProperties, Assembly assembly)
		{
			BindingFlags flags = BindingFlags.DeclaredOnly | BindingFlags.Public | BindingFlags.Static;
			foreach (Type type in assembly.GetTypes())
			{
				FieldInfo[] fields = type.GetFields(flags);
				MethodInfo[] methods = null;
				foreach (FieldInfo field in fields)
				{
					if (!field.FieldType.Is(typeof(DependencyProperty)))
						continue;
					if (!field.Name.EndsWith("Property"))
						continue;

					string fieldName = field.Name.Replace("Property", "");
					string getName = "Get" + fieldName;
					string setName = "Set" + fieldName;
					bool foundGet = false;
					bool foundSet = false;
					if (methods == null)
						methods = type.GetMethods(flags);
					foreach (MethodInfo method in methods)
					{
						if (method.Name == getName && method.GetParameters().Length == 1 && 
							method.GetParameters()[0].ParameterType.Is(typeof(DependencyObject)))
							foundGet = true;
						else
						if (method.Name == setName && method.GetParameters().Length == 2 && 
							method.GetParameters()[0].ParameterType.Is(typeof(DependencyObject)))
							foundSet = true;
						if (foundGet && foundSet)
							break;
					}

					if (!(foundGet && foundSet))
						continue;

					try
					{
						DependencyProperty dp = field.GetValue(null) as DependencyProperty;
					}
					catch (Exception)
					{
						continue;
					}

					// Found an attached Dependency Property
					attachedProperties.Add(field);
				}
			}

			return true;
		}
	}

	internal static class Extensions
	{
		// Extension for Type
		internal static string ShortName(this Type type)
		{
			string typeName = type.ToString();
			int index = typeName.LastIndexOf('[');
			if (index < 0)
				return typeName.Substring(typeName.LastIndexOf('.') + 1);

			string collectionName = typeName.Substring(index);
			collectionName = collectionName.Substring(collectionName.LastIndexOf('.') + 1);
			typeName = typeName.Substring(0, index);
			typeName = typeName.Substring(typeName.LastIndexOf('.') + 1);
			return typeName + '[' + collectionName;
		}
	}
}
