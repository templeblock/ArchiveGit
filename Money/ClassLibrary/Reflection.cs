using System;
using System.Diagnostics;
using System.Reflection;
using System.Windows.Browser;

namespace ClassLibrary
{
	public class Reflection
	{
		public static void FindScriptableTypes(Type baseType)
		{
			Assembly assembly = baseType.Assembly;
			Debug.WriteLine(string.Format("Assembly: {0}\n", assembly.ManifestModule.ToString()));
			foreach (Type type in assembly.GetTypes())
			{
				bool baseTypeIsOutput = false;
				//if (type.GetCustomAttributes(typeof(ScriptableTypeAttribute), true).Length > 0)
				//{
				//    Debug.WriteLine(string.Format("[ScriptableType] Type: {0}", type.TypeName()));
				//    baseTypeIsOutput = true;
				//}

				FindScriptableMembers(type, baseTypeIsOutput);
			}

			Debug.WriteLine("\n");
		}

		private static void FindScriptableMembers(Type baseType, bool typeIsOutput)
		{
			// Unfortunately, we cannot examine "internal" or "protected" properties
			BindingFlags flags = BindingFlags.Instance | BindingFlags.FlattenHierarchy | BindingFlags.Public;
			bool baseTypeIsOutput = typeIsOutput;

			PropertyInfo[] properties = baseType.GetProperties(flags);
			foreach (PropertyInfo property in properties)
			{
				if (typeIsOutput || property.GetCustomAttributes(typeof(ScriptableMemberAttribute), true).Length > 0)
				{
					if (!baseTypeIsOutput)
					{
						Debug.WriteLine(string.Format("\tType: {0}\n", baseType.FriendlyName()));
						baseTypeIsOutput = true;
					}

					Debug.WriteLine(string.Format("\t\t{0} {1} {{ {2}{3}}}",
						property.PropertyType.FriendlyName(), property.Name,
						(property.CanRead ? "get; " : string.Empty),
						(property.CanWrite ? "set; " : string.Empty)));
				}
			}

			MethodInfo[] methods = baseType.GetMethods(flags);
			foreach (MethodInfo method in methods)
			{
				string prefix = method.Name.Substring(0, method.Name.IndexOf('_') + 1);
				if (prefix == "get_" || prefix == "set_" || prefix == "add_" || prefix == "remove_")
					continue;
				if (typeIsOutput || method.GetCustomAttributes(typeof(ScriptableMemberAttribute), true).Length > 0)
				{
					if (!baseTypeIsOutput)
					{
						Debug.WriteLine(string.Format("\tType: {0}\n", baseType.FriendlyName()));
						baseTypeIsOutput = true;
					}

					string parameters = "(";
					foreach (ParameterInfo parameter in method.GetParameters())
					{
						if (parameters.Length > 1)
							parameters += ", ";
						parameters += parameter.ParameterType.FriendlyName();
						parameters += " ";
						parameters += parameter.Name;
					}
					parameters += ")";

					Debug.WriteLine(string.Format("\t\t{0} {1}{2};",
						method.ReturnType.FriendlyName(), method.Name, parameters));
				}
			}

			EventInfo[] events = baseType.GetEvents(flags);
			foreach (EventInfo evnt in events)
			{
				if (typeIsOutput || evnt.GetCustomAttributes(typeof(ScriptableMemberAttribute), true).Length > 0)
				{
					if (!baseTypeIsOutput)
					{
						Debug.WriteLine(string.Format("\tType: {0}\n", baseType.FriendlyName()));
						baseTypeIsOutput = true;
					}

					Debug.WriteLine(string.Format("\t\tevent {0} {1};",
						evnt.EventHandlerType.FriendlyName(), evnt.Name));
				}
			}

			Debug.WriteLine("\n");
		}
	}
}
