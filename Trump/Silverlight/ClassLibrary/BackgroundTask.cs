using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Threading;

namespace ClassLibrary
{
	internal class BackgroundTask
	{
		private BackgroundWorker m_worker;
		private DispatcherTimer m_RunDelayTimer;
		private object m_Argument;

		internal bool IsBusy { get { return m_worker.IsBusy; } }

		// Offer 4 different cascading contructors
		public BackgroundTask(FrameworkElement optionalRunAsElement, DoWorkEventHandler doWork)
		{
			m_worker = new BackgroundWorker();
			m_worker.WorkerSupportsCancellation = true;
			if (optionalRunAsElement == null)
				m_worker.DoWork += doWork;
			else
				m_worker.DoWork += delegate(object sender, DoWorkEventArgs e)
				{
					optionalRunAsElement.Dispatcher.BeginInvoke(delegate() { doWork(sender, e); });
				};
		}

		public BackgroundTask(FrameworkElement optionalRunAsElement, int runDelay, DoWorkEventHandler doWork)
			: this(optionalRunAsElement, doWork)
		{
			if (runDelay > 0)
			{
				// Create the run delay timer but don't start it
				m_RunDelayTimer = new DispatcherTimer();
				m_RunDelayTimer.Interval = new TimeSpan(runDelay * TimeSpan.TicksPerMillisecond);
				m_RunDelayTimer.Tick += RunDelayCompleted;
			}
		}

		public BackgroundTask(FrameworkElement optionalRunAsElement, int runDelay, DoWorkEventHandler doWork, RunWorkerCompletedEventHandler runWorkerCompleted)
			: this(optionalRunAsElement, runDelay, doWork)
		{
			if (runWorkerCompleted != null)
				m_worker.RunWorkerCompleted += runWorkerCompleted;
		}

		public BackgroundTask(FrameworkElement optionalRunAsElement, int runDelay, DoWorkEventHandler doWork, RunWorkerCompletedEventHandler runWorkerCompleted, ProgressChangedEventHandler progressChanged)
			: this(optionalRunAsElement, runDelay, doWork, runWorkerCompleted)
		{
			if (progressChanged != null)
				m_worker.ProgressChanged += progressChanged;
		}

		internal void Cancel()
		{
			if (m_worker.WorkerSupportsCancellation)
				m_worker.CancelAsync();
		}

		internal bool Run(object argument, bool bCancelPrevious)
		{
			m_Argument = argument;

			// If requested, cancel any pending operation before we continue
			if (bCancelPrevious)
				Cancel();

			// If no timer is involved, go ahead and run the task
			if (m_RunDelayTimer == null)
				return RunWorker();

			// Restart the timer
			if (m_RunDelayTimer.IsEnabled)
				m_RunDelayTimer.Stop();
			m_RunDelayTimer.Start();
			return false;
		}

		private void RunDelayCompleted(object sender, EventArgs e)
		{
			m_RunDelayTimer.Stop();
			RunWorker();
		}

		private bool RunWorker()
		{
			// RunWorkerAsync will raise an exception if the BackgroundWorker is busy
			if (m_worker.IsBusy)
				return false;
			m_worker.RunWorkerAsync(m_Argument);
			return true;
		}
	}
}
