using System.ComponentModel.Composition;
using System.Reflection;
using System.Windows.Input;
using ClassLibrary;

namespace CharityRace
{
	[Export]
	public class MainViewModel : ViewModelBase
	{
		public ICommand WelcomeActivateCommand { get; private set; }
		public ICommand RiderActivateCommand { get; private set; }
		public ICommand DonorActivateCommand { get; private set; }
		public ICommand DonationActivateCommand { get; private set; }
		public ICommand AboutActivateCommand { get; private set; }

		public bool IsWelcomeViewActive { get { return _IsWelcomeViewActive; } set { SetProperty(ref _IsWelcomeViewActive, value, MethodBase.GetCurrentMethod().Name); } }
		private bool _IsWelcomeViewActive;

		public bool IsRiderViewActive { get { return _IsRiderViewActive; } set { SetProperty(ref _IsRiderViewActive, value, MethodBase.GetCurrentMethod().Name); } }
		private bool _IsRiderViewActive;

		public bool IsDonorViewActive { get { return _IsDonorViewActive; } set { SetProperty(ref _IsDonorViewActive, value, MethodBase.GetCurrentMethod().Name); } }
		private bool _IsDonorViewActive;

		public bool IsDonationViewActive { get { return _IsDonationViewActive; } set { SetProperty(ref _IsDonationViewActive, value, MethodBase.GetCurrentMethod().Name); } }
		private bool _IsDonationViewActive;

		public bool IsAboutViewActive { get { return _IsAboutViewActive; } set { SetProperty(ref _IsAboutViewActive, value, MethodBase.GetCurrentMethod().Name); } }
		private bool _IsAboutViewActive;

		public MainViewModel()
		{
			WelcomeActivateCommand = new Command(WelcomeActivate, null/*canExecuteFunction*/);
			RiderActivateCommand = new Command(RiderActivate, null/*canExecuteFunction*/);
			DonorActivateCommand = new Command(DonorActivate, null/*canExecuteFunction*/);
			DonationActivateCommand = new Command(DonationActivate, null/*canExecuteFunction*/);
			AboutActivateCommand = new Command(AboutActivate, null/*canExecuteFunction*/);

			WelcomeActivate(null);
		}

		private void WelcomeActivate(object parameter)
		{
			DeactivateViews();
			IsWelcomeViewActive = true;
		}

		private void RiderActivate(object parameter)
		{
			DeactivateViews();
			IsRiderViewActive = true;
		}

		private void DonorActivate(object parameter)
		{
			DeactivateViews();
			IsDonorViewActive = true;
		}

		private void DonationActivate(object parameter)
		{
			DeactivateViews();
			IsDonationViewActive = true;
		}

		private void AboutActivate(object parameter)
		{
			DeactivateViews();
			IsAboutViewActive = true;
		}

		private void DeactivateViews()
		{
			IsWelcomeViewActive = false;
			IsRiderViewActive = false;
			IsDonorViewActive = false;
			IsDonationViewActive = false;
			IsAboutViewActive = false;
		}
	}
}
