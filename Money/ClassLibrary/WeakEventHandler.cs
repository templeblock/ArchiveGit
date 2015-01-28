using System;
using System.Reflection;
using System.Windows;

namespace ClassLibrary
{
	// Reference: http://diditwith.net/2007/03/23/SolvingTheProblemWithEventsWeakEventHandlers.aspx
	// Reference: http://www.paulstovell.com/weakevents

	// Usage in an event subscriber:
	//public class EventSubscriber
	//{
	//    public EventSubscriber(EventProvider provider)
	//    {
	//        provider.MyEvent += new WeakEventHandler<EventHandler, EventArgs>(MyWeakEventHandler, e => provider.MyEvent -= e);
	//    }

	//    private void MyWeakEventHandler(object sender, EventArgs e)
	//    {
	//    }
	//}

	// Usage in an event provider:
	//public class EventProvider
	//{
	//    private EventHandler _myEvent;
	//    public event EventHandler MyEvent
	//    {
	//        add { _myEvent += new WeakEventHandler<EventHandler, EventArgs>(value, e => _myEvent -= e); }
	//        remove { }
	//    }
	//}

	public delegate void UnregisterCallback<TEventArgs>(EventHandler<TEventArgs> eventHandler) where TEventArgs : EventArgs;

	public static class EventHandlerExt
	{
		public static TT CastDelegate<TT>(this Delegate source) where TT : class
		{
			if (source == null)
				return null;

			Delegate[] delegates = source.GetInvocationList();
			if (delegates.Length == 1)
				return Delegate.CreateDelegate(typeof(TT), delegates[0].Target, delegates[0].Method) as TT;

			//if (source.GetInvocationList().Length > 1)
			//	throw new ArgumentException("Cannot safely convert MulticastDelegate");
			for (int i = 0; i < delegates.Length; i++)
				delegates[i] = Delegate.CreateDelegate(typeof(TT), delegates[i].Target, delegates[i].Method);

			Delegate ret = delegates[0];
			foreach (Delegate d in delegates)
				Delegate.Combine(ret, d);
			return ret as TT;
		}
	}

	public sealed class WeakEventHandler<TEventHandler, TEventArgs> where TEventHandler : class where TEventArgs : EventArgs 
	{
		private readonly WeakReference _targetReference;
		private readonly EventHandler<TEventArgs> _handler;
		private UnregisterCallback<TEventArgs> _unregister;
		private Action<object, TEventArgs> _eventHandler;
		private readonly MethodInfo _method;

		public WeakEventHandler(EventHandler<TEventArgs> eventHandler, UnregisterCallback<TEventArgs> unregister)
		{
			_targetReference = new WeakReference(eventHandler.Target, true);
			_handler = Invoke;
			_unregister = unregister;
			_method = eventHandler.Method;
		} 

		public void Invoke(object sender, TEventArgs e)
		{
			object target = _targetReference.Target;
			if (target != null)
			{
				if (!_method.IsPublic)
				{
					Helper.Alert(string.Format("Event handler '{0}' must be public", _method.Name));
					return;
				}
				if (_eventHandler == null)
					_eventHandler = (Action<object, TEventArgs>)Delegate.CreateDelegate(typeof(Action<object, TEventArgs>), target, _method, false);
				if (_eventHandler != null)
				    _eventHandler(sender, e); 
			}
			else
			if (_unregister != null)
			{
				_unregister(_handler);
				_unregister = null;
			}
		}

		public static implicit operator TEventHandler(WeakEventHandler<TEventHandler, TEventArgs> weh)
		{
			return EventHandlerExt.CastDelegate<TEventHandler>(weh._handler);
		}
	}

	//public sealed class WeakEventHandler2<TT, TEventHandler, TEventArgs> : IWeakEventHandler<TEventArgs> 
	//    where TEventHandler : EventHandler<TEventArgs>
	//    where TEventArgs : EventArgs
	//{
	//    private delegate void OpenEventHandler(TT @this, object sender, TEventArgs e);

	//    private readonly WeakReference _targetReference;
	//    private readonly object _handler;
	//    private UnregisterCallback<TEventArgs> _unregister;
	//    private readonly OpenEventHandler _openHandler;
	//    //private readonly MethodInfo _method;

	//    public WeakEventHandler2(TEventHandler eventHandler, UnregisterCallback<TEventArgs> unregister)
	//    {
	//        _targetReference = new WeakReference(eventHandler.Target, true);
	//        //_method = eventHandler.Method;
	//        _handler = Invoke;
	//        _unregister = unregister;
	//        _openHandler = (OpenEventHandler)Delegate.CreateDelegate(typeof(OpenEventHandler), null, eventHandler.Method);
	//    }

	//    public void Invoke(object sender, TEventArgs e)
	//    {
	//        TT target = (TT)_targetReference.Target;

	//        if (target != null)
	//        {
	//            _openHandler.Invoke(target, sender, e);
	//            //var eventHandler = (Action<object, TEventArgs>)Delegate.CreateDelegate(typeof(Action<object, TEventArgs>), target, _method, true);
	//            //if (eventHandler != null)
	//            //    eventHandler(sender, e); 
	//        }
	//        else
	//        if (_unregister != null)
	//        {
	//            _unregister(_handler);
	//            _unregister = null;
	//        }
	//    }

	//    public EventHandler<TEventArgs> Handler
	//    {
	//        get { return _handler; }
	//    }

	//    public static implicit operator TEventHandler(WeakEventHandler2<TT, TEventHandler, TEventArgs> weh)
	//    {
	//        return (TEventHandler)weh._handler;
	//    }
	//}
}
