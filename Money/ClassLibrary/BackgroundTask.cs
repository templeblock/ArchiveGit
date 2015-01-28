using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Threading;

namespace ClassLibrary
{
	internal class BackgroundTask : BackgroundWorker
	{
		private DispatcherTimer _RunDelayTimer;
		private object _Argument;

		// Offer 4 different cascading constructors
		public BackgroundTask(bool runOnUIThread, DoWorkEventHandler doWork)
		{
			base.WorkerSupportsCancellation = true;
			if (!runOnUIThread)
				base.DoWork += doWork;
			else
				base.DoWork += delegate(object sender, DoWorkEventArgs e)
				{
					Deployment.Current.Dispatcher.BeginInvoke(delegate() { doWork(sender, e); });
				};
		}

		public BackgroundTask(bool runOnUIThread, int runDelay, DoWorkEventHandler doWork)
			: this(runOnUIThread, doWork)
		{
			if (runDelay <= 0)
				return;

			// Create the run delay timer but don't start it
			_RunDelayTimer = new DispatcherTimer();
			_RunDelayTimer.Interval = new TimeSpan(runDelay * TimeSpan.TicksPerMillisecond);
			_RunDelayTimer.Tick += RunDelayCompleted;
		}

		public BackgroundTask(bool runOnUIThread, int runDelay, DoWorkEventHandler doWork, RunWorkerCompletedEventHandler runWorkerCompleted)
			: this(runOnUIThread, runDelay, doWork)
		{
			if (runWorkerCompleted != null)
				base.RunWorkerCompleted += runWorkerCompleted;
		}

		public BackgroundTask(bool runOnUIThread, int runDelay, DoWorkEventHandler doWork, RunWorkerCompletedEventHandler runWorkerCompleted, ProgressChangedEventHandler progressChanged)
			: this(runOnUIThread, runDelay, doWork, runWorkerCompleted)
		{
			if (progressChanged != null)
				base.ProgressChanged += progressChanged;
		}

		internal void Cancel()
		{
			if (base.WorkerSupportsCancellation)
				base.CancelAsync();
		}

		internal bool Run(object argument, bool bCancelPrevious)
		{
			_Argument = argument;

			// If requested, cancel any pending operation before we continue
			if (bCancelPrevious)
				Cancel();

			// If no timer is involved, go ahead and run the task
			if (_RunDelayTimer == null)
				return RunWorker();

			// Restart the timer
			if (_RunDelayTimer.IsEnabled)
				_RunDelayTimer.Stop();
			_RunDelayTimer.Start();
			return false;
		}

		private void RunDelayCompleted(object sender, EventArgs e)
		{
			_RunDelayTimer.Stop();
			RunWorker();
		}

		private bool RunWorker()
		{
			// RunWorkerAsync will raise an exception if IsBusy is true
			if (base.IsBusy)
				return false;

			base.RunWorkerAsync(_Argument);
			return true;
		}
	}
}
