using System;
using System.Linq;

namespace AutoNotifyPropertyChange
{
	class NothingToDo
	{
		public static bool With(Type t)
		{
			if (!t.IsSealed && t.IsPublic )
				return !t.GetProperties().Any(q => q.IsVirtual() && q.CanRead && q.CanWrite );

			return true;
		}
	}
}
