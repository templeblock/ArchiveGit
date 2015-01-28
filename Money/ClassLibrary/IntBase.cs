
namespace ClassLibrary
{
	internal class IntBase
	{
		public int Value;

		public IntBase()
		{
		}

		public IntBase(int value)
		{
			Value = value;
		}

		public static implicit operator IntBase(int i)
		{
			return new IntBase(i);
		}

		public static implicit operator IntBase(uint i)
		{
			return new IntBase((int)i);
		}

		public static implicit operator int(IntBase obj)
		{
			return obj.Value;
		}

		public static implicit operator uint(IntBase obj)
		{
			return (uint)obj.Value;
		}

		public static bool operator ==(IntBase obja, IntBase objb)
		{
			return obja.Value == objb.Value;
		}

		public static bool operator !=(IntBase obja, IntBase objb)
		{
			return obja.Value != objb.Value;
		}

		public static bool operator <(IntBase obja, IntBase objb)
		{
			return obja.Value < objb.Value;
		}

		public static bool operator >(IntBase obja, IntBase objb)
		{
			return obja.Value > objb.Value;
		}

		public static bool operator <=(IntBase obja, IntBase objb)
		{
			return obja.Value <= objb.Value;
		}

		public static bool operator >=(IntBase obja, IntBase objb)
		{
			return obja.Value >= objb.Value;
		}

		public override bool Equals(object obj)
		{
			if (obj is IntBase)
				return Value == (obj as IntBase).Value;
			return false;
		}

		public override int GetHashCode()
		{
			return Value.GetHashCode();
		}

		public override string ToString()
		{
			return Value.ToString();
		}
	}
}
