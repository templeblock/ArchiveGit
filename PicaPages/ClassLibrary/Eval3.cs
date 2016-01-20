using System.Reflection;
using System;
using System.Collections;
using System.Text;
using System.Globalization;
using System.Collections.Generic;

public delegate void ValueChangedEventHandler(object Sender, EventArgs e);

public interface iEvalFunctions
{
	iEvalFunctions InheritedFunctions();
}

public interface iVariableBag
{
	iEvalTypedValue GetVariable(string varname);
}

public interface iEvalHasDescription
{
	string Name
	{
		get;
	}
	string Description
	{
		get;
	}
}

public interface iEvalTypedValue : iEvalValue
{
	Type SystemType
	{
		get;
	}
	EvalType EvalType
	{
		get;
	}
}

public interface iEvalValue
{
	object Value
	{
		get;
	}
	event ValueChangedEventHandler ValueChanged;
}

public class Evaluator
{
	internal List<object> mEnvironmentFunctionsList;
	public bool RaiseVariableNotFoundException;
	public readonly eParserSyntax Syntax;
	public readonly bool CaseSensitive;

	public Evaluator(eParserSyntax syntax, bool caseSensitive)
	{
		this.Syntax = syntax;
		this.CaseSensitive = caseSensitive;
		mEnvironmentFunctionsList = new List<object>();
	}

	public void AddEnvironmentFunctions(object obj)
	{
		if (obj == null)
			return;

		if (!mEnvironmentFunctionsList.Contains(obj))
		{
			mEnvironmentFunctionsList.Add(obj);
		}
	}

	public void RemoveEnvironmentFunctions(object obj)
	{
		if (mEnvironmentFunctionsList.Contains(obj))
		{
			mEnvironmentFunctionsList.Remove(obj);
		}
	}

	public opCode Parse(string str)
	{
		return new parser(this).Parse(str);
	}

	public static string ConvertToString(object value)
	{
		if (value is string)
		{
			return (string)value;
		}
		else
		if (value == null)
		{
			return string.Empty;
		}
		else
		if (value is DateTime)
		{
			DateTime d = (DateTime)value;
			if (d.TimeOfDay.TotalMilliseconds > 0)
			{
				return d.ToString();
			}
			else
			{
				return d.ToShortDateString();
			}
		}
		else
		if (value is decimal)
		{
			decimal d = (decimal)value;
			if ((d % 1) != 0)
			{
				return d.ToString("#,##0.00");
			}
			else
			{
				return d.ToString("#,##0");
			}
		}
		else
		if (value is double)
		{
			double d = (double)value;
			if ((d % 1) != 0)
			{
				return d.ToString("#,##0.00");
			}
			else
			{
				return d.ToString("#,##0");
			}
		}
		else
		if (value is object)
		{
			return value.ToString();
		}
		
		return string.Empty;
	}

	public class parserException : Exception
	{
		public readonly string formula;
		public readonly int pos;

		internal parserException(string str, string formula, int pos)
			: base(str)
		{
			this.formula = formula;
			this.pos = pos;
		}

	}

}

public class EvalVariable : iEvalTypedValue, iEvalHasDescription
{

	private object mValue;
	private string mDescription;
	private string mName;
	private Type mSystemType;
	private EvalType mEvalType;

	#region "iEvalTypedValue implementation"
	public string Description
	{
		get { return mDescription; }
	}

	public string Name
	{
		get { return mName; }
	}

	public object iEvalTypedValue_value
	{
		get { return mValue; }
	}

	public EvalType EvalType
	{
		get { return mEvalType; }
	}

	public Type SystemType
	{
		get { return mSystemType; }
	}

	#endregion

	public EvalVariable(string name, object originalValue, string description, Type systemType)
	{
		mName = name;
		mValue = originalValue;
		mDescription = description;
		mSystemType = systemType;
		mEvalType = Globals.GetEvalType(systemType);
	}

	public object Value
	{
		get { return mValue; }
		set
		{
			if ((!object.ReferenceEquals(value, mValue)))
			{
				mValue = value;
				if (ValueChanged != null)
				{
					ValueChanged(this, new EventArgs());
				}
			}
		}
	}

	public event ValueChangedEventHandler ValueChanged;
//j	public delegate void ValueChangedEventHandler(object Sender, EventArgs e);
}

public abstract class opCode : iEvalTypedValue, iEvalHasDescription
{

	protected ValueDelegate mValueDelegate;
	protected delegate object ValueDelegate();

	public delegate void RunDelegate();

	protected opCode()
	{

	}

	protected void RaiseEventValueChanged(object sender, EventArgs e)
	{
		if (ValueChanged != null)
		{
			ValueChanged(sender, e);
		}
	}

	public abstract EvalType EvalType
	{
		get;
	}

	public bool CanReturn(EvalType type)
	{
		return true;
	}

	public virtual string Description
	{
		get { return "opCode " + this.GetType().Name; }
	}

	public virtual string Name
	{
		get { return "opCode " + this.GetType().Name; }
	}

	public virtual object Value
	{
		get { return mValueDelegate(); }
	}

	public virtual Type SystemType
	{
		get { return Globals.GetSystemType(this.EvalType); }
	}


	protected internal void Convert(tokenizer tokenizer, ref opCode param1, EvalType EvalType)
	{
		if (param1.EvalType != EvalType)
		{
			if (param1.CanReturn(EvalType))
			{
				param1 = new opCodeConvert(tokenizer, param1, EvalType);
			}
			else
			{
				tokenizer.RaiseError("Cannot convert " + param1.Name + " into " + EvalType);
			}
		}
	}

	protected static void ConvertToSystemType(ref iEvalTypedValue param1, Type SystemType)
	{
		if ((!object.ReferenceEquals(param1.SystemType, SystemType)))
		{
			if (object.ReferenceEquals(SystemType, typeof(object)))
			{
			}
			//ignore 
			else
			{
				param1 = new opCodeSystemTypeConvert(param1, SystemType);
			}
		}
	}

	protected void SwapParams(ref opCode Param1, ref opCode Param2)
	{
		opCode swp = Param1;
		Param1 = Param2;
		Param2 = swp;
	}

	public event ValueChangedEventHandler ValueChanged;
//j	public delegate void ValueChangedEventHandler(object Sender, EventArgs e);
}

internal class opCodeVariable : opCode
{

	EvalVariable mVariable;

	public opCodeVariable(EvalVariable variable)
	{
		mVariable = variable;
	}

	public override object Value
	{
		get { return mVariable; }
	}

	public override EvalType EvalType
	{
		get { return mVariable.EvalType; }
	}

	private void mVariable_ValueChanged(object Sender, EventArgs e)
	{
		base.RaiseEventValueChanged(Sender, e);
	}
}

internal class opCodeImmediate : opCode
{

	private object mValue;
	private EvalType mEvalType;

	public opCodeImmediate(EvalType EvalType, object value)
	{
		mEvalType = EvalType;
		mValue = value;
	}

	public override object Value
	{
		get { return mValue; }
	}


	public override EvalType EvalType
	{
		get { return mEvalType; }
	}
}

internal class opCodeUnary : opCode
{

	opCode mParam1;
	private EvalType mEvalType;

	public opCodeUnary(eTokenType tt, opCode param1)
	{
		mParam1 = param1;
		EvalType v1Type = mParam1.EvalType;

		switch (tt)
		{
			case eTokenType.operator_not:
				if (v1Type == EvalType.Boolean)
				{
					mValueDelegate = BOOLEAN_NOT;
					mEvalType = EvalType.Boolean;
				}

				break;
			case eTokenType.operator_minus:
				if (v1Type == EvalType.Number)
				{
					mValueDelegate = NUM_CHGSIGN;
					mEvalType = EvalType.Number;
				}

				break;
		}
	}

	private object BOOLEAN_NOT()
	{
		return !(bool)mParam1.value;
	}

	private object NUM_CHGSIGN()
	{
		return -(double)mParam1.value;
	}

	public override EvalType EvalType
	{
		get { return mEvalType; }
	}

	private void mParam1_ValueChanged(object Sender, EventArgs e)
	{
		base.RaiseEventValueChanged(Sender, e);
	}
}

internal class opCodeConvert : opCode
{
	iEvalTypedValue mParam1;
	private EvalType mEvalType = EvalType.Unknown;

	public opCodeConvert(tokenizer tokenizer, iEvalTypedValue param1, EvalType EvalType)
	{
		mParam1 = param1;
		switch (EvalType)
		{
			case EvalType.Boolean:
				mValueDelegate = TBool;
				mEvalType = EvalType.Boolean;
				break;
			case EvalType.Date:
				mValueDelegate = TDate;
				mEvalType = EvalType.Date;
				break;
			case EvalType.Number:
				mValueDelegate = TNum;
				mEvalType = EvalType.Number;
				break;
			case EvalType.String:
				mValueDelegate = TStr;
				mEvalType = EvalType.String;
				break;
			default:
				tokenizer.RaiseError("Cannot convert " + param1.SystemType.Name + " to " + EvalType);
				break;
		}
	}

	private object TBool()
	{
		return Globals.TBool(mParam1);
	}

	private object TDate()
	{
		return Globals.TDate(mParam1);
	}

	private object TNum()
	{
		return Globals.TNum(mParam1);
	}

	private object TStr()
	{
		return Globals.TStr(mParam1);
	}

	public override EvalType EvalType
	{
		get { return mEvalType; }
	}

	private void mParam1_ValueChanged(object Sender, EventArgs e)
	{
		base.RaiseEventValueChanged(Sender, e);
	}
}

internal class opCodeSystemTypeConvert : opCode
{
	iEvalTypedValue mParam1;
	private EvalType mEvalType = EvalType.Unknown;
	private Type mSystemType;

	public opCodeSystemTypeConvert(iEvalTypedValue param1, Type Type)
	{
		mParam1 = param1;
		mValueDelegate = CType;
		mSystemType = Type;
		mEvalType = Globals.GetEvalType(Type);
	}

	private object CType()
	{
		return Convert.ChangeType(mParam1.Value, mSystemType);
	}

	public override EvalType EvalType
	{
		get { return mEvalType; }
	}

	public override Type SystemType
	{
		get { return mSystemType; }
	}

	private void mParam1_ValueChanged(object Sender, EventArgs e)
	{
		base.RaiseEventValueChanged(Sender, e);
	}
}

internal class opCodeBinary : opCode
{

	opCode mParam1;
	opCode mParam2;
	private EvalType mEvalType;

	public opCodeBinary(tokenizer tokenizer, opCode param1, eTokenType tt, opCode param2)
	{
		mParam1 = param1;
		mParam2 = param2;

		EvalType v1Type = mParam1.EvalType;
		EvalType v2Type = mParam2.EvalType;

		switch (tt)
		{
			case eTokenType.operator_plus:
				if (v1Type == EvalType.Number & v2Type == EvalType.Number)
				{
					mValueDelegate = NUM_PLUS_NUM;
					mEvalType = EvalType.Number;
				}
				else if (v1Type == EvalType.Number & v2Type == EvalType.Date)
				{
					SwapParams(ref mParam1, mParam2);
					mValueDelegate = DATE_PLUS_NUM;
					mEvalType = EvalType.Date;
				}
				else if (v1Type == EvalType.Date & v2Type == EvalType.Number)
				{
					mValueDelegate = DATE_PLUS_NUM;
					mEvalType = EvalType.Date;
				}
				else if (mParam1.CanReturn(EvalType.String) & mParam2.CanReturn(EvalType.String))
				{
					Convert(tokenizer, param1, ref EvalType.String);
					mValueDelegate = STR_CONCAT_STR;
					mEvalType = EvalType.String;
				}

				break;
			case eTokenType.operator_minus:
				if (v1Type == EvalType.Number & v2Type == EvalType.Number)
				{
					mValueDelegate = NUM_MINUS_NUM;
					mEvalType = EvalType.Number;
				}
				else if (v1Type == EvalType.Date & v2Type == EvalType.Number)
				{
					mValueDelegate = DATE_MINUS_NUM;
					mEvalType = EvalType.Date;
				}
				else if (v1Type == EvalType.Date & v2Type == EvalType.Date)
				{
					mValueDelegate = DATE_MINUS_DATE;
					mEvalType = EvalType.Number;
				}

				break;
			case eTokenType.operator_mul:
				if (v1Type == EvalType.Number & v2Type == EvalType.Number)
				{
					mValueDelegate = NUM_MUL_NUM;
					mEvalType = EvalType.Number;
				}

				break;
			case eTokenType.operator_div:
				if (v1Type == EvalType.Number & v2Type == EvalType.Number)
				{
					mValueDelegate = NUM_DIV_NUM;
					mEvalType = EvalType.Number;
				}

				break;
			case eTokenType.operator_percent:
				if (v1Type == EvalType.Number & v2Type == EvalType.Number)
				{
					mValueDelegate = NUM_PERCENT_NUM;
					mEvalType = EvalType.Number;
				}

				break;
			case eTokenType.operator_and:
			case eTokenType.operator_or:
				Convert(tokenizer, mParam1, ref EvalType.Boolean);
				Convert(tokenizer, mParam2, ref EvalType.Boolean);
				switch (tt)
				{
					case eTokenType.operator_or:
						mValueDelegate = BOOL_OR_BOOL;
						mEvalType = EvalType.Boolean;
						break;
					case eTokenType.operator_and:
						mValueDelegate = BOOL_AND_BOOL;
						mEvalType = EvalType.Boolean;
						break;
				}
				break;
		}

		if (mValueDelegate == null)
		{
			tokenizer.RaiseError("Cannot apply the operator " + tt.ToString().Replace("operator_", "") + " on " + v1Type.ToString() + " and " + v2Type.ToString());
		}
	}

	private object BOOL_AND_BOOL()
	{
		return (bool)mParam1.Value & (bool)mParam2.Value;
	}

	private object BOOL_OR_BOOL()
	{
		return (bool)mParam1.Value | (bool)mParam2.Value;
	}

	private object BOOL_XOR_BOOL()
	{
		return (bool)mParam1.Value ^ (bool)mParam2.Value;
	}

	private object NUM_EQ_NUM()
	{
		return (double)mParam1.Value == (double)mParam2.Value;
	}

	private object NUM_LT_NUM()
	{
		return (double)mParam1.Value < (double)mParam2.Value;
	}

	private object NUM_GT_NUM()
	{
		return (double)mParam1.Value > (double)mParam2.Value;
	}

	private object NUM_GE_NUM()
	{
		return (double)mParam1.Value >= (double)mParam2.Value;
	}

	private object NUM_LE_NUM()
	{
		return (double)mParam1.Value <= (double)mParam2.Value;
	}

	private object NUM_NE_NUM()
	{
		return (double)mParam1.Value != (double)mParam2.Value;
	}

	private object NUM_PLUS_NUM()
	{
		return (double)mParam1.Value + (double)mParam2.Value;
	}

	private object NUM_MUL_NUM()
	{
		return (double)mParam1.Value * (double)mParam2.Value;
	}

	private object NUM_MINUS_NUM()
	{
		return (double)mParam1.Value - (double)mParam2.Value;
	}

	private object DATE_PLUS_NUM()
	{
		return ((DateTime)mParam1.Value).AddDays((double)mParam2.Value);
	}

	private object DATE_MINUS_DATE()
	{
		return ((DateTime)mParam1.Value).Subtract((DateTime)mParam2.Value).TotalDays;
	}

	private object DATE_MINUS_NUM()
	{
		return ((DateTime)mParam1.Value).AddDays(-(double)mParam2.Value);
	}

	private object STR_CONCAT_STR()
	{
		return mParam1.Value.ToString() + mParam2.Value.ToString();
	}

	private object NUM_DIV_NUM()
	{
		return (double)mParam1.Value / (double)mParam2.Value;
	}

	private object NUM_PERCENT_NUM()
	{
		return (double)mParam2.Value * ((double)mParam1.Value / 100);
	}

	public override EvalType EvalType
	{
		get { return mEvalType; }
	}

	private void mParam1_ValueChanged(object Sender, EventArgs e)
	{
		base.RaiseEventValueChanged(Sender, e);
	}

	private void mParam2_ValueChanged(object Sender, EventArgs e)
	{
		base.RaiseEventValueChanged(Sender, e);
	}
}

public class opCodeGetVariable : opCode
{

	iEvalTypedValue mParam1;

	public opCodeGetVariable(iEvalTypedValue value)
	{
		mParam1 = value;
	}


	public override object Value
	{
		get { return mParam1.Value; }
	}

	public override Type SystemType
	{
		get { return mParam1.SystemType; }
	}

	public override EvalType EvalType
	{
		get { return mParam1.EvalType; }
	}

	private void mParam1_ValueChanged(object Sender, EventArgs e)
	{
		base.RaiseEventValueChanged(Sender, e);
	}

}

public class opCodeCallMethod : opCode
{

	private object mBaseObject;
	private Type mBaseSystemType;
	private EvalType mBaseEvalType;
	// for the events only 
	iEvalValue mBaseValue;
	private object mBaseValueObject;

	private MemberInfo mMethod;
	private iEvalTypedValue[] mParams;
	private object[] mParamValues;

	private Type mResultSystemType;
	private EvalType mResultEvalType;
	// just for some 
	iEvalValue mResultValue;

	internal opCodeCallMethod(object baseObject, MemberInfo method, IList @params) 
	{ 
		if (@params == null) 
			@params = new iEvalTypedValue[]; 
		iEvalTypedValue[] newParams = new iEvalTypedValue[@params.Count]; 
		object[] newParamValues = new object[@params.Count]; 
		
		@params.CopyTo(newParams, 0); 
		
		foreach (iEvalTypedValue p in newParams) { 
			p.ValueChanged += mParamsValueChanged; 
		} 
		mParams = newParams; 
		mParamValues = newParamValues; 
		mBaseObject = baseObject; 
		mMethod = method; 
		
		if (mBaseObject is iEvalValue) { 
			if (mBaseObject is iEvalTypedValue) { 
				{ 
					mBaseSystemType = ((iEvalTypedValue)mBaseObject).SystemType; 
					mBaseEvalType = ((iEvalTypedValue)mBaseObject).EvalType; 
				} 
			} 
			else { 
				mBaseSystemType = mBaseObject.GetType(); 
				mBaseEvalType = Globals.GetEvalType(mBaseSystemType); 
			} 
		} 
		else { 
			mBaseSystemType = mBaseObject.GetType; 
			mBaseEvalType = Globals.GetEvalType(mBaseSystemType); 
		} 
		
		ParameterInfo[] paramInfo; 
		if (method is Reflection.PropertyInfo) { 
			{ 
				mResultSystemType = ((Reflection.PropertyInfo)method).PropertyType; 
				paramInfo = ((Reflection.PropertyInfo)method).GetIndexParameters(); 
			} 
			mValueDelegate = GetProperty; 
		} 
		else if (method is Reflection.MethodInfo) { 
			{ 
				mResultSystemType = ((Reflection.MethodInfo)method).ReturnType; 
				paramInfo = ((Reflection.MethodInfo)method).GetParameters(); 
			} 
			mValueDelegate = GetMethod; 
		} 
		else if (method is Reflection.FieldInfo) { 
			{ 
				mResultSystemType = ((Reflection.FieldInfo)method).FieldType; 
				paramInfo = new ParameterInfo[]; 
			} 
			mValueDelegate = GetField; 
		} 
		
		for (int i = 0; i <= mParams.Length - 1; i++) { 
			if (i < paramInfo.Length) { 
				ConvertToSystemType(mParams(i), paramInfo(i).ParameterType); 
			} 
		} 
		
		if (typeof(iEvalValue).IsAssignableFrom(mResultSystemType)) { 
			mResultValue = (iEvalValue)InternalValue(); 
			if (mResultValue is iEvalTypedValue) { 
				{ 
					mResultSystemType = ((iEvalTypedValue)mResultValue).SystemType; 
					mResultEvalType = ((iEvalTypedValue)mResultValue).EvalType; 
				} 
			} 
			else if (mResultValue == null) { 
				mResultSystemType = typeof(object); 
				mResultEvalType = EvalType.Object; 
			} 
			else { 
				object v = mResultValue.Value; 
				if (v == null) { 
					mResultSystemType = typeof(object); 
					mResultEvalType = EvalType.Object; 
				} 
				else { 
					mResultSystemType = v.GetType; 
					mResultEvalType = Globals.GetEvalType(mResultSystemType); 
				} 
			} 
		} 
		else { 
			mResultSystemType = systemType; 
			mResultEvalType = Globals.GetEvalType(systemType); 
		} 
	}

	protected static internal opCode GetNew(tokenizer tokenizer, object baseObject, MemberInfo method, IList @params)
	{
		opCode o;
		o = new opCodeCallMethod(baseObject, method, @params);

		if (o.EvalType != Eval3.EvalType.Object && (!object.ReferenceEquals(o.systemType, Globals.GetSystemType(o.EvalType))))
		{
			return new opCodeConvert(tokenizer, o, o.EvalType);
		}
		else
		{
			return o;
		}
	}

	private object GetProperty()
	{
		object res = ((Reflection.PropertyInfo)mMethod).GetValue(mBaseValueObject, mParamValues);
		return res;
	}

	private object GetMethod()
	{
		object res = ((Reflection.MethodInfo)mMethod).Invoke(mBaseValueObject, mParamValues);
		return res;
	}

	private object GetField()
	{
		object res = ((Reflection.FieldInfo)mMethod).GetValue(mBaseValueObject);
		return res;
	}

	private object InternalValue()
	{
		for (int i = 0; i <= mParams.Length - 1; i++)
		{
			mParamValues(i) = mParams[i].Value;
		}
		if (mBaseObject is iEvalValue)
		{
			mBaseValue = (iEvalValue)mBaseObject;
			mBaseValueObject = mBaseValue.Value;
		}
		else
		{
			mBaseValueObject = mBaseObject;
		}
		return base.mValueDelegate();
	}

	public override object Value
	{
		get
		{
			object res = InternalValue();
			if (res is iEvalValue)
			{
				mResultValue = (iEvalValue)res;
				res = mResultValue.Value;
			}
			return res;
		}
	}

	public override Type SystemType
	{
		get { return mResultSystemType; }
	}

	public override EvalType EvalType
	{
		get { return mResultEvalType; }
	}

	private void mParamsValueChanged(object Sender, EventArgs e)
	{
		base.RaiseEventValueChanged(Sender, e);
	}

	private void mBaseVariable_ValueChanged(object Sender, EventArgs e)
	{
		base.RaiseEventValueChanged(Sender, e);
	}

	private void mResultVariable_ValueChanged(object Sender, EventArgs e)
	{
		base.RaiseEventValueChanged(Sender, e);
	}
}

public class opCodeGetArrayEntry : opCode
{

	opCode mArray;

	private iEvalTypedValue[] mParams;
	private int[] mValues;
	private EvalType mResultEvalType;
	private Type mResultSystemType;

	public opCodeGetArrayEntry(opCode array, IList @params)
	{
		iEvalTypedValue[] newParams = new iEvalTypedValue[@params.Count];
		int[] newValues = new int[@params.Count];
		@params.CopyTo(newParams, 0);
		mArray = array;
		mParams = newParams;
		mValues = newValues;
		mResultSystemType = array.SystemType.GetElementType;
		mResultEvalType = Globals.GetEvalType(mResultSystemType);
	}

	public override object Value
	{
		get
		{
			object res;
			List<object> arr = (List<object>)mArray.Value;
			for (int i = 0; i <= mValues.Length - 1; i++)
			{
				mValues[i] = (int)mParams[i].Value;
			}
			res = arr.GetValue(mValues);
			return res;
		}
	}

	public override Type SystemType
	{
		get { return mResultSystemType; }
	}

	public override EvalType EvalType
	{
		get { return mResultEvalType; }
	}

	private void mBaseVariable_ValueChanged(object Sender, EventArgs e)
	{
		base.RaiseEventValueChanged(Sender, e);
	}

}


public class tokenizer
{
	private string mString;
	private int mLen;
	private int mPos;
	private char mCurChar;
	private parser mParser;
	private eParserSyntax mSyntax;

	public int startpos;
	public eTokenType type;
	public StringBuilder value = new StringBuilder();

	internal tokenizer(parser Parser, string str, eParserSyntax syntax)
	{
		mString = str;
		mLen = str.Length;
		mSyntax = syntax;
		mPos = 0;
		mParser = Parser;
		NextChar();
		// start the machine 
	}

	internal void RaiseError(string msg) 
	{ 
		Exception ex = null; //j
		if (ex is Evaluator.parserException) { 
			msg // ERROR: Unknown assignment operator ConcatString ; 
		} 
		else { 
			msg // ERROR: Unknown assignment operator ConcatString ; 
			if ((ex != null)) { 
				msg // ERROR: Unknown assignment operator ConcatString ; 
			} 
		} 
		throw new Evaluator.parserException(msg, this.mString, this.mPos); 
	}

	internal void RaiseUnexpectedToken(string msg) 
	{ 
		if (Strings.Len(msg) == 0) { 
			msg = ""; 
		} 
		else { 
			msg // ERROR: Unknown assignment operator ConcatString ; 
		} 
		RaiseError(msg + "Unexpected " + type.ToString().Replace('_', ' ') + " : " + value.ToString()); 
	}

	internal void RaiseWrongOperator(eTokenType tt, object ValueLeft, object valueRight, string msg) 
	{ 
		if (Strings.Len(msg) > 0) { 
			msg.Replace("[op]", tt.GetType().ToString()); 
			msg // ERROR: Unknown assignment operator ConcatString ; 
		} 
		msg = "Cannot apply the operator " + tt.ToString(); 
		if (ValueLeft == null) { 
			msg // ERROR: Unknown assignment operator ConcatString ; 
		} 
		else { 
			msg // ERROR: Unknown assignment operator ConcatString ; 
		} 
		if ((valueRight != null)) { 
			msg // ERROR: Unknown assignment operator ConcatString ; 
		} 
		RaiseError(msg); 
	}

	private bool IsOp()
	{
		return mCurChar == '+' | mCurChar == '-' | mCurChar == '–' | mCurChar == '%' | mCurChar == '/' | mCurChar == '(' | mCurChar == ')' | mCurChar == '.';
	}

	public void NextToken()
	{
		value.Length = 0;
		type = eTokenType.none;
		do
		{
			startpos = mPos;
			switch (mCurChar)
			{
				case 0:
					type = eTokenType.end_of_formula;
					break;
				case '0': // TODO: to '9' 
					ParseNumber();
					break;
				case '-':
				case '–':
					NextChar();
					type = eTokenType.operator_minus;
					break;
				case '+':
					NextChar();
					type = eTokenType.operator_plus;
					break;
				case '*':
					NextChar();
					type = eTokenType.operator_mul;
					break;
				case '/':
					NextChar();
					type = eTokenType.operator_div;
					break;
				case '%':
					NextChar();
					type = eTokenType.operator_percent;
					break;
				case '(':
					NextChar();
					type = eTokenType.open_parenthesis;
					break;
				case ')':
					NextChar();
					type = eTokenType.close_parenthesis;
					break;
				case '<':
					NextChar();
					if (mCurChar == '=')
					{
						NextChar();
						type = eTokenType.operator_le;
					}
					else if (mCurChar == '>')
					{
						NextChar();
						type = eTokenType.operator_ne;
					}
					else
					{
						type = eTokenType.operator_lt;
					}

					break;
				case '>':
					NextChar();
					if (mCurChar == '=')
					{
						NextChar();
						type = eTokenType.operator_ge;
					}
					else
					{
						type = eTokenType.operator_gt;
					}

					break;
				case ',':
					NextChar();
					type = eTokenType.comma;
					break;
				case '=':
					NextChar();
					type = eTokenType.operator_eq;
					break;
				case '.':
					NextChar();
					type = eTokenType.dot;
					break;
				case '\'':
				case '"':
					ParseString(true);
					type = eTokenType.value_string;
					break;
				case '#':
					ParseDate();
					break;
				case '&':
					NextChar();
					type = eTokenType.operator_concat;
					break;
				case '[':
					NextChar();
					type = eTokenType.open_bracket;
					break;
				case ']':
					NextChar();
					type = eTokenType.close_bracket;
					break;
				case '\0': // TODO: to ' ' 
					break;
				// do nothing 
				default:
					ParseIdentifier();
					break;
			}
			if (type != eTokenType.none)
				break;

			NextChar();
		}
		while (true);
	}

	private void NextChar()
	{
		if (mPos < mLen)
		{
			mCurChar = mString[mPos];
			if (mCurChar == (147) | mCurChar == (148))
			{
				mCurChar = '"';
			}
			if (mCurChar == (145) | mCurChar == (146))
			{
				mCurChar = '\'';
			}
			mPos += 1;
		}
		else
		{
			mCurChar = null;
		}
	}

	private void ParseNumber()
	{
		type = eTokenType.value_number;
		while (mCurChar >= '0' & mCurChar <= '9')
		{
			value.Append(mCurChar);
			NextChar();
		}
		if (mCurChar == '.')
		{
			value.Append(mCurChar);
			NextChar();
			while (mCurChar >= '0' & mCurChar <= '9')
			{
				value.Append(mCurChar);
				NextChar();
			}
		}
	}

	private void ParseIdentifier()
	{
		while ((mCurChar >= '0' & mCurChar <= '9') | (mCurChar >= 'a' & mCurChar <= 'z') | (mCurChar >= 'A' & mCurChar <= 'Z') | (mCurChar >= 'A' & mCurChar <= 'Z') | (mCurChar >= (128)) | (mCurChar == '_'))
		{
			value.Append(mCurChar);
			NextChar();
		}
		switch (value.ToString())
		{
			case "and":
				type = eTokenType.operator_and;
				break;
			case "or":
				type = eTokenType.operator_or;
				break;
			case "not":
				type = eTokenType.operator_not;
				break;
			case "true":
			case "yes":
				type = eTokenType.value_true;
				break;
			case "if":
				type = eTokenType.operator_if;
				break;
			case "false":
			case "no":
				type = eTokenType.value_false;
				break;
			default:
				type = eTokenType.value_identifier;
				break;
		}
	}

	private void ParseString(bool InQuote)
	{
		char OriginalChar;
		if (InQuote)
		{
			OriginalChar = mCurChar;
			NextChar();
		}

		char PreviousChar;
		while (mCurChar != null)
		{
			if (InQuote && mCurChar == OriginalChar)
			{
				NextChar();
				if (mCurChar == OriginalChar)
				{
					value.Append(mCurChar);
				}
				else //End of String 
					return;
			}
			else
			if (mCurChar == '%')
			{
				NextChar();
				if (mCurChar == '[')
				{
					NextChar();
					StringBuilder SaveValue = value;
					int SaveStartPos = startpos;
					this.value = new StringBuilder();
					this.NextToken();
					// restart the tokenizer for the subExpr 
					object subExpr;
					try
					{
						// subExpr = mParser.ParseExpr(0, ePriority.none) 
						if (subExpr == null)
						{
							this.value.Append("<nothing>");
						}
						else
						{
							this.value.Append(Evaluator.ConvertToString(subExpr));
						}
					}
					catch (Exception ex)
					{
						// XML don't like < and > 
						this.value.Append("[Error " + ex.Message + "]");
					}
					SaveValue.Append(value.ToString());
					value = SaveValue;
					startpos = SaveStartPos;
				}
				else
				{
					value.Append('%');
				}
			}
			else
			{
				value.Append(mCurChar);
				NextChar();
			}
		}
		if (InQuote)
		{
			RaiseError("Incomplete string, missing " + OriginalChar + "; String started");
		}
	}

	private void ParseDate()
	{
		NextChar();
		// eat the # 
		int zone = 0;
		while ((mCurChar >= '0' & mCurChar <= '9') | (mCurChar == '/') | (mCurChar == ':') | (mCurChar == ' '))
		{
			value.Append(mCurChar);
			NextChar();
		}
		if (mCurChar != "#")
		{
			RaiseError("Invalid date should be #dd/mm/yyyy#");
		}
		else
		{
			NextChar();
		}
		type = eTokenType.value_date;
	}

}

internal class parser
{
	private tokenizer mTokenizer;
	private Evaluator mEvaluator;

	public parser(Evaluator evaluator)
	{
		mEvaluator = evaluator;
	}

	public opCode Parse(string str)
	{
		if (str == null)
			str = string.Empty;
		mTokenizer = new tokenizer(this, str, eParserSyntax.cSharp);
		mTokenizer.NextToken();
		opCode res = ParseExpr(null, ePriority.none);
		if (mTokenizer.type == eTokenType.end_of_formula)
		{
			if (res == null)
				res = new opCodeImmediate(EvalType.String, string.Empty);
			return res;
		}
		else
		{
			mTokenizer.RaiseUnexpectedToken("");
		}
	}

	private opCode ParseExpr(opCode Acc, ePriority priority)
	{
		opCode ValueLeft = null;
		opCode valueRight = null;
		do
		{
			switch (mTokenizer.type)
			{
				case eTokenType.operator_minus:
					// unary minus operator 
					mTokenizer.NextToken();
					ValueLeft = ParseExpr(null, ePriority.unaryminus);
					ValueLeft = new opCodeUnary(eTokenType.operator_minus, ValueLeft);
					break; // TODO: might not be correct. Was : Exit Do 
				case eTokenType.operator_plus:
					// unary minus operator 
					mTokenizer.NextToken();
					break;
				case eTokenType.operator_not:
					mTokenizer.NextToken();
					ValueLeft = ParseExpr(null, ePriority.not);
					ValueLeft = new opCodeUnary(eTokenType.operator_not, ValueLeft);
					break; // TODO: might not be correct. Was : Exit Do 
				case eTokenType.value_identifier:
					ParseIdentifier(ref ValueLeft);
					break; // TODO: might not be correct. Was : Exit Do 
				case eTokenType.value_true:
					ValueLeft = new opCodeImmediate(EvalType.Boolean, true);
					mTokenizer.NextToken();
					break; // TODO: might not be correct. Was : Exit Do 
				case eTokenType.value_false:
					ValueLeft = new opCodeImmediate(EvalType.Boolean, false);
					mTokenizer.NextToken();
					break; // TODO: might not be correct. Was : Exit Do 
				case eTokenType.value_string:
					ValueLeft = new opCodeImmediate(EvalType.String, mTokenizer.value.ToString());
					mTokenizer.NextToken();
					break; // TODO: might not be correct. Was : Exit Do 
				case eTokenType.value_number:
					try
					{
						ValueLeft = new opCodeImmediate(EvalType.Number, double.Parse(mTokenizer.value.ToString(), NumberStyles.Float, CultureInfo.InvariantCulture));
					}
					catch (Exception)
					{
						mTokenizer.RaiseError(string.Format("Invalid number {0}", mTokenizer.value.ToString()));
					}

					mTokenizer.NextToken();
					break; // TODO: might not be correct. Was : Exit Do 
				case eTokenType.value_date:
					try
					{
						ValueLeft = new opCodeImmediate(EvalType.Date, mTokenizer.value.ToString());
					}
					catch (Exception)
					{
						mTokenizer.RaiseError(string.Format("Invalid date {0}, it should be #DD/MM/YYYY hh:mm:ss#", mTokenizer.value.ToString()));
					}

					mTokenizer.NextToken();
					break; // TODO: might not be correct. Was : Exit Do 
				case eTokenType.open_parenthesis:
					mTokenizer.NextToken();
					ValueLeft = ParseExpr(null, ePriority.none);
					if (mTokenizer.type == eTokenType.close_parenthesis)
					{
						// good we eat the end parenthesis and continue ... 
						mTokenizer.NextToken();
						break; // TODO: might not be correct. Was : Exit Do 
					}
					else
					{
						mTokenizer.RaiseUnexpectedToken("End parenthesis not found");
					}

					break;
				case eTokenType.operator_if:
					// first check functions 
					List<object> parameters = new List<object>();
					// parameters... 
					mTokenizer.NextToken();

					bool brackets = false;
					parameters = ParseParameters(ref brackets);
					break; // TODO: might not be correct. Was : Exit Do 
				default:
					break; // TODO: might not be correct. Was : Exit Do 
			}
		}
		while (true);

		if (ValueLeft == null)
		{
			mTokenizer.RaiseUnexpectedToken("No Expression found");
		}
		ParseDot(ref ValueLeft);
		do
		{
			eTokenType tt;
			tt = mTokenizer.type;
			switch (tt)
			{
				case eTokenType.end_of_formula:
					// end of line 
					return ValueLeft;
				case eTokenType.value_number:
					mTokenizer.RaiseUnexpectedToken("Unexpected number without previous opterator");
					break;
				case eTokenType.operator_plus:
					if (priority < ePriority.plusminus)
					{
						mTokenizer.NextToken();
						valueRight = ParseExpr(ValueLeft, ePriority.plusminus);
						ValueLeft = new opCodeBinary(mTokenizer, ValueLeft, tt, valueRight);
					}
					else
					{
						break; // TODO: might not be correct. Was : Exit Do 
					}

					break;
				case eTokenType.operator_minus:
					if (priority < ePriority.plusminus)
					{
						mTokenizer.NextToken();
						valueRight = ParseExpr(ValueLeft, ePriority.plusminus);
						ValueLeft = new opCodeBinary(mTokenizer, ValueLeft, tt, valueRight);
					}
					else
					{
						break; // TODO: might not be correct. Was : Exit Do 
					}

					break;
				case eTokenType.operator_concat:
					if (priority < ePriority.concat)
					{
						mTokenizer.NextToken();
						valueRight = ParseExpr(ValueLeft, ePriority.concat);
						ValueLeft = new opCodeBinary(mTokenizer, ValueLeft, tt, valueRight);
					}
					else
					{
						break; // TODO: might not be correct. Was : Exit Do 
					}

					break;
				case eTokenType.operator_mul:
				case eTokenType.operator_div:
					if (priority < ePriority.muldiv)
					{
						mTokenizer.NextToken();
						valueRight = ParseExpr(ValueLeft, ePriority.muldiv);
						ValueLeft = new opCodeBinary(mTokenizer, ValueLeft, tt, valueRight);
					}
					else
					{
						break; // TODO: might not be correct. Was : Exit Do 
					}

					break;
				case eTokenType.operator_percent:
					if (priority < ePriority.percent)
					{
						mTokenizer.NextToken();
						ValueLeft = new opCodeBinary(mTokenizer, ValueLeft, tt, Acc);
					}
					else
					{
						break; // TODO: might not be correct. Was : Exit Do 
					}

					break;
				case eTokenType.operator_or:
					if (priority < ePriority.or)
					{
						mTokenizer.NextToken();
						valueRight = ParseExpr(ValueLeft, ePriority.or);
						ValueLeft = new opCodeBinary(mTokenizer, ValueLeft, tt, valueRight);
					}
					else
					{
						break; // TODO: might not be correct. Was : Exit Do 
					}

					break;
				case eTokenType.operator_and:
					if (priority < ePriority.and)
					{
						mTokenizer.NextToken();
						valueRight = ParseExpr(ValueLeft, ePriority.and);
						ValueLeft = new opCodeBinary(mTokenizer, ValueLeft, tt, valueRight);
					}
					else
					{
						break; // TODO: might not be correct. Was : Exit Do 
					}

					break;
				case eTokenType.operator_ne:
				case eTokenType.operator_gt:
				case eTokenType.operator_ge:
				case eTokenType.operator_eq:
				case eTokenType.operator_le:
				case eTokenType.operator_lt:
					if (priority < ePriority.equality)
					{
						tt = mTokenizer.type;
						mTokenizer.NextToken();
						valueRight = ParseExpr(ValueLeft, ePriority.equality);
						ValueLeft = new opCodeBinary(mTokenizer, ValueLeft, tt, valueRight);
					}
					else
					{
						break; // TODO: might not be correct. Was : Exit Do 
					}

					break;
				default:

					break; // TODO: might not be correct. Was : Exit Do 

					break;
			}
		}
		while (true);

		return ValueLeft;
	}

	[Flags()]
	private enum eCallType
	{
		field = 1,
		method = 2,
		property = 4,
		all = 7
	}

	private bool EmitCallFunction(ref opCode valueLeft, string funcName, List<object> parameters, eCallType CallType, bool ErrorIfNotFound) 
	{ 
		opCode newOpcode; 
		if (valueLeft == null) { 
			foreach (object functions in mEvaluator.mEnvironmentFunctionsList)
			{ 
				while ((functions != null)) { 
					newOpcode = GetLocalFunction(functions, functions.GetType(), funcName, parameters, CallType); 
					if ((newOpcode != null)) 
						break; // TODO: might not be correct. Was : Exit For 

					if (functions is iEvalFunctions)
					{ 
						functions = ((iEvalFunctions)functions).InheritedFunctions; 
					} 
					else
					{ 
						break; // TODO: might not be correct. Was : Exit Do 
					} 
				} 
			} 
		} 
		else { 
			newOpcode = GetLocalFunction(valueLeft, valueLeft.SystemType, funcName, parameters, CallType); 
		} 
		if ((newOpcode != null)) { 
			valueLeft = newOpcode; 
			return true; 
		} 
		else { 
			if (ErrorIfNotFound) 
				mTokenizer.RaiseError("Variable or method " + funcName + " was not found"); 
			return false; 
		} 
	}

	private opCode GetLocalFunction(object @base, Type baseType, string funcName, List<object> parameters, eCallType CallType)
	{
		MemberInfo mi;
		mi = GetMemberInfo(baseType, funcName, parameters);
		if ((mi != null))
		{
			switch (mi.MemberType)
			{
				case MemberTypes.Field:
					if ((CallType & eCallType.field) == 0)
						mTokenizer.RaiseError("Unexpected Field");

					break;
				case MemberTypes.Method:
					if ((CallType & eCallType.method) == 0)
						mTokenizer.RaiseError("Unexpected Method");

					break;
				case MemberTypes.Property:
					if ((CallType & eCallType.property) == 0)
						mTokenizer.RaiseError("Unexpected Property");

					break;
				default:
					mTokenizer.RaiseUnexpectedToken(mi.MemberType.ToString() + " members are not supported");
					break;
			}

			return opCodeCallMethod.GetNew(mTokenizer, @base, mi, parameters);
		}
		if (@base is iVariableBag)
		{
			iEvalTypedValue val = ((iVariableBag)@base).GetVariable(funcName);
			if ((val != null))
			{
				return new opCodeGetVariable(val);
			}
		}
		return null;
	}

	private MemberInfo GetMemberInfo(Type objType, string func, List<object> parameters) 
	{ 
		BindingFlags bindingAttr; 
		bindingAttr = BindingFlags.GetProperty | BindingFlags.GetField | BindingFlags.Public | BindingFlags.InvokeMethod | BindingFlags.Instance | BindingFlags.Static; 
		if (this.mEvaluator.CaseSensitive == false) { 
			bindingAttr = bindingAttr | BindingFlags.IgnoreCase; 
		} 
		MemberInfo[] mis; 
		
		if (func == null) { 
			mis = objType.GetDefaultMembers(); 
		} 
		else { 
			mis = objType.GetMember(func, bindingAttr); 
		} 
		
		
		// There is a bit of cooking here... 
		// lets find the most acceptable Member 
		int Score; 
		int BestScore; 
		MemberInfo BestMember; 
		ParameterInfo[] plist; 
		int idx; 
		
		MemberInfo mi; 
		for (int i = 0; i <= mis.Length - 1; i++) { 
			mi = mis[i]; 
			
			if (mi is MethodInfo) { 
				plist = ((MethodInfo)mi).GetParameters(); 
			} 
			else if (mi is PropertyInfo) { 
				plist = ((PropertyInfo)mi).GetIndexParameters(); 
			} 
			else if (mi is FieldInfo) { 
				plist = null; 
			} 
			Score = 10; 
			// by default 
			idx = 0; 
			if (plist == null) 
				plist = new ParameterInfo(); 
			if (parameters == null) 
				parameters = new List<object>(); 
			
			ParameterInfo pi; 
			if (parameters.Count > plist.Length) { 
				Score = 0; 
			} 
			else { 
				for (int index = 0; index <= plist.Length - 1; index++) { 
					pi = plist[index]; 
					//For Each pi As ParameterInfo In plist 
					if (idx < parameters.Count) { 
						Score += ParamCompatibility(parameters[idx], pi.ParameterType); 
					} 
					else if (pi.IsOptional) { 
						Score += 10; 
					} 
					else { 
						// unknown parameter 
						Score = 0; 
					} 
					idx += 1; 
				} 
			} 
			if (Score > BestScore) { 
				BestScore = Score; 
				BestMember = mi; 
			} 
		} 
		return BestMember; 
	}

	private static int ParamCompatibility(object value, Type type)
	{
		// This function returns a score 1 to 10 to this question 
		// Can this value fit into this type ? 
		if (value == null)
		{
			if (object.ReferenceEquals(type, typeof(object)))
				return 10;
			if (object.ReferenceEquals(type, typeof(string)))
				return 8;
			return 5;
		}
		else if (object.ReferenceEquals(type, value.GetType()))
		{
			return 10;
		}
		else
		{
			return 5;
		}
	}

	private void ParseDot(ref opCode ValueLeft)
	{
		do
		{
			switch (mTokenizer.type)
			{
				case eTokenType.dot:
					mTokenizer.NextToken();
					break;
				case eTokenType.open_parenthesis:
					break;
				// fine this is either an array or a default property 
				default:
					return;
			}
			ParseIdentifier(ref ValueLeft);
		}
		while (true);
	}

	private void ParseIdentifier(ref opCode ValueLeft)
	{
		// first check functions 
		List<object> parameters;
		// parameters... 
		//Dim types As New List<object> 
		string func = mTokenizer.value.ToString();
		mTokenizer.NextToken();
		bool isBrackets;
		parameters = ParseParameters(ref isBrackets);
		if ((parameters != null))
		{
			List<object> EmptyParameters = new List<object>();
			bool ParamsNotUsed;
			if (mEvaluator.Syntax == eParserSyntax.Vb)
			{
				// in vb we don't know if it is array or not as we have only parenthesis 
				// so we try with parameters first 
				if (!EmitCallFunction(ValueLeft, func, parameters, eCallType.all, ErrorIfNotFound = false))
				{
					// and if not found we try as array or default member 
					EmitCallFunction(ValueLeft, func, EmptyParameters, eCallType.all, ErrorIfNotFound = true);
					ParamsNotUsed = true;
				}
			}
			else
			{
				if (isBrackets)
				{
					if (!EmitCallFunction(ValueLeft, func, parameters, eCallType.property, ErrorIfNotFound = false))
					{
						EmitCallFunction(ValueLeft, func, EmptyParameters, eCallType.all, ErrorIfNotFound = true);
						ParamsNotUsed = true;
					}
				}
				else
				{
					if (!EmitCallFunction(ValueLeft, func, parameters, eCallType.field | eCallType.method, ErrorIfNotFound = false))
					{
						EmitCallFunction(ValueLeft, func, EmptyParameters, eCallType.all, ErrorIfNotFound = true);
						ParamsNotUsed = true;
					}
				}
			}
			// we found a function without parameters 
			// so our parameters must be default property or an array 
			Type t = ValueLeft.SystemType;
			if (ParamsNotUsed)
			{
				if (t.IsArray)
				{
					if (parameters.Count == t.GetArrayRank())
					{
						ValueLeft = new opCodeGetArrayEntry(ValueLeft, parameters);
					}
					else
					{
						mTokenizer.RaiseError("This array has " + t.GetArrayRank() + " dimensions");
					}
				}
				else
				{
					MemberInfo mi;
					mi = GetMemberInfo(t, null, parameters);
					if ((mi != null))
					{
						ValueLeft = opCodeCallMethod.GetNew(mTokenizer, ValueLeft, mi, parameters);
					}
					else
					{
						mTokenizer.RaiseError("Parameters not supported here");
					}
				}
			}
		}
		else
		{
			EmitCallFunction(ValueLeft, func, parameters, eCallType.all, ErrorIfNotFound = true);
		}
	}

	private List<object> ParseParameters(ref bool brackets)
	{
		List<object> parameters = new List<object>();
		opCode valueleft;
		eTokenType lClosing = eTokenType.none;

		if (mTokenizer.type == eTokenType.open_parenthesis || (mTokenizer.type == eTokenType.open_bracket & mEvaluator.Syntax == eParserSyntax.cSharp))
		{
			switch (mTokenizer.type)
			{
				case eTokenType.open_bracket:
					lClosing = eTokenType.close_bracket;
					brackets = true;
					break;
				case eTokenType.open_parenthesis:
					lClosing = eTokenType.close_parenthesis;
					break;
			}

			mTokenizer.NextToken();
			//eat the parenthesis 
			do
			{
				if (mTokenizer.type == lClosing)
				{
					// good we eat the end parenthesis and continue ... 
					mTokenizer.NextToken();
					break;
				}
				valueleft = ParseExpr(null, ePriority.none);
				parameters.Add(valueleft);

				if (mTokenizer.type == lClosing)
				{
					// good we eat the end parenthesis and continue ... 
					mTokenizer.NextToken();
					break;
				}
				else if (mTokenizer.type == eTokenType.comma)
				{
					mTokenizer.NextToken();
				}
				else
				{
					mTokenizer.RaiseUnexpectedToken(lClosing.ToString() + " not found");
				}
			}
			while (true);
		}
		return parameters;
	}

}

public class Globals
{
	internal bool varEq(string v1, string v2)
	{
		int lv1;
		int lv2;
		if (v1 == null)
			lv1 = 0;
		else
			lv1 = v1.Length;
		if (v2 == null)
			lv2 = 0;
		else
			lv2 = v2.Length;

		if (lv1 != lv2)
			return false;
		if (lv1 == 0)
			return true;

		char c1;
		char c2;

		for (int i = 0; i <= lv1 - 1; i++)
		{
			c1 = v1[i];
			c2 = v2[i];
			switch (c1)
			{
				case 'a': // TODO: to 'z' 
					if (c2 != c1 && c2 != (c1 - 32))
					{
						return false;
					}

					break;
				case 'A': // TODO: to 'Z' 
					if (c2 != c1 && c2 != (c1 + 32))
					{
						return false;
					}

					break;
				case '-':
				case '_':
				case '.':
					if (c2 != c1 && c2 != '_' && c2 != '.')
					{
						return false;
					}

					break;
				//case '_':
				//    if (c2 != c1 && c2 != '-')
				//    {
				//        return false;
				//    }

				//    break;
				default:
					if (c2 != c1)
						return false;

					break;
			}
		}
		return true;
	}

	internal EvalType GetObjectType(object o)
	{
		if (o == null)
		{
			return EvalType.Unknown;
		}
		else
		{
			Type t = o.GetType();
			return GetEvalType(t);
		}
	}

	internal EvalType GetEvalType(Type t)
	{
		if (object.ReferenceEquals(t, typeof(float)) | object.ReferenceEquals(t, typeof(double)) | object.ReferenceEquals(t, typeof(decimal)) | object.ReferenceEquals(t, typeof(Int16)) | object.ReferenceEquals(t, typeof(Int32)) | object.ReferenceEquals(t, typeof(Int64)) | object.ReferenceEquals(t, typeof(byte)) | object.ReferenceEquals(t, typeof(UInt16)) | object.ReferenceEquals(t, typeof(UInt32)) | object.ReferenceEquals(t, typeof(UInt64)))
		{
			return EvalType.Number;
		}
		else if (object.ReferenceEquals(t, typeof(DateTime)))
		{
			return EvalType.Date;
		}
		else if (object.ReferenceEquals(t, typeof(bool)))
		{
			return EvalType.Boolean;
		}
		else if (object.ReferenceEquals(t, typeof(string)))
		{
			return EvalType.String;
		}
		else
		{
			return EvalType.Object;
		}
	}

	internal Type GetSystemType(EvalType t)
	{
		switch (t)
		{
			case EvalType.Boolean:
				return typeof(bool);
			case EvalType.Date:
				return typeof(DateTime);
			case EvalType.Number:
				return typeof(double);
			case EvalType.String:
				return typeof(string);
			default:
				return typeof(object);
		}
	}

	public bool TBool(iEvalTypedValue o)
	{
		return (bool)o.Value;
	}

	public DateTime TDate(iEvalTypedValue o)
	{
		return (DateTime)o.Value;
	}

	public double TNum(iEvalTypedValue o)
	{
		return (double)o.Value;
	}

	public string TStr(iEvalTypedValue o)
	{
		return (string)o.Value;
	}

}

internal enum ePriority
{
	none = 0,
	or = 1,
	and = 2,
	not = 3,
	equality = 4,
	concat = 5,
	plusminus = 6,
	muldiv = 7,
	percent = 8,
	unaryminus = 9
}

public enum VariableComplexity
{
	normal
}

public enum EvalType
{
	Unknown,
	Number,
	Boolean,
	String,
	Date,
	Object
}

public enum eParserSyntax
{
	cSharp,
	Vb
}

public enum eTokenType
{
	none,
	end_of_formula,
	operator_plus,
	operator_minus,
	operator_mul,
	operator_div,
	operator_percent,
	open_parenthesis,
	comma,
	dot,
	close_parenthesis,
	operator_ne,
	operator_gt,
	operator_ge,
	operator_eq,
	operator_le,
	operator_lt,
	operator_and,
	operator_or,
	operator_not,
	operator_concat,
	operator_if,

	value_identifier,
	value_true,
	value_false,
	value_number,
	value_string,
	value_date,
	open_bracket,
	close_bracket
} 
