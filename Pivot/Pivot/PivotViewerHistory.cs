using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Interop;
using System.Windows.Pivot;

namespace Pivot
{
	/// <summary>
	/// The travel log feature integrates the browser back/forward buttons with the ViewerState of the PivotViewer
	/// control, so that hitting the "Back" button will take you back to previous ViewerStates (and will also take you
	/// back to the current item, if there was one when you last left that ViewerState). The feature works through two
	/// event handlers, one hooked to the PivotViewer, and one hooked to the browser:
	///
	/// 1) Handle an event from the PivotViewer indicating that the ViewerState has changed by creating unique
	///    navigation states and mapping them to the ViewerStates (ViewerState changes as a result of applying
	///    filters, changing the view, changing the sort order, etc)
	/// 2) Handle a navigation state change by finding the mapped ViewerState and applying it
	///    back to the PivotViewer control
	///
	/// Because changing the ViewerState can cause a navigation state change, and changing the navigation state
	/// can cause a ViewerState change, flags indicating that a navigate is in progress are necessary to prevent
	/// an infinite loop.
	///
	/// Note that this sample travel log implementation does NOT survive the Silverlight app getting unloaded. When the
	/// app is unloaded, the navigation state-to-ViewerState mappings are not persisted to any sort of storage, and so
	/// they are lost. Thus, after that navigating forward or back will not take you to the correct states.
	/// </summary>
	public class PivotViewerHistory
	{
		private static PivotViewerHistory _instance;
		private PivotViewer _pivotViewer;
		private Dictionary<string, PivotViewerHistoryEntry> _history;
		private PivotViewerHistoryEntry _currentHistoryEntry;
		private string _focusedItem;
		private string _lastFocusedItem;
		private bool _isNavigating;
		private bool _hasItemIdBeenUpdatedFromCollectionLoad;
		private bool _viewerStateChangedHandlerQueued;

		/// <summary>
		/// Create a singleton instance of the PivotViewerHistory and connect it to a PivotViewer. Because PivotViewerHistory
		/// reads/writes App.Current.Host.NavigationState, there should only be one instance of PivotViewerHistory in
		/// an application.
		/// </summary>
		public static void Create(PivotViewer pivotViewer, Uri initialCollectionUri)
		{
			if (_instance != null)
				throw new InvalidOperationException("PivotViewerHistory interacts with the application state, there should only be one instance in an application");

			if (pivotViewer.CollectionUri != null)
				throw new InvalidOperationException("PivotViewerHistory should be connected to a PivotViewer before it has loaded a collection");

			_instance = new PivotViewerHistory(pivotViewer, initialCollectionUri);
		}

		private PivotViewerHistory(PivotViewer pivotViewer, Uri initialCollectionUri)
		{
			_pivotViewer = pivotViewer;
			_history = new Dictionary<string, PivotViewerHistoryEntry>();
			_currentHistoryEntry = new PivotViewerHistoryEntry(initialCollectionUri, string.Empty);

			_pivotViewer.PropertyChanged += OnPropertyChanged;
			App.Current.Host.NavigationStateChanged += OnHostNavigationStateChanged;
		}

		private void OnHostNavigationStateChanged(object sender, NavigationStateChangedEventArgs args)
		{
			// Update the travel log using the new and old states
			PivotViewerNavigation(args.PreviousNavigationState, args.NewNavigationState);
		}

		private void OnPropertyChanged(object sender, PropertyChangedEventArgs args)
		{
			if (args.PropertyName == "ViewerState")
			{
				// A ViewerState change happens in sequence with a lot of other property changes
				// and may even happen twice in response to one user action. To account for this,
				// delay handling so everything has a chance to settle first.
				QueueDelayedViewerStateChangedHandler();
			}
			if (args.PropertyName == "CurrentItemId")
			{
				// Record current and previous values for CurrentItemId, and note
				// that it has been updated during the handling of LoadCollection
				_lastFocusedItem = _focusedItem;
				_focusedItem = _pivotViewer.CurrentItemId;
				_hasItemIdBeenUpdatedFromCollectionLoad = true;
			}
		}

		private void OnViewerStateChanged()
		{
			// If the new ViewerState is empty, it will be resolved to a non-empty default  ViewerState when the
			// collection is finished loading, and a new travel log entry will get created for that state, so
			// don't create one yet.
			if (string.IsNullOrEmpty(_pivotViewer.ViewerState))
				return;

			// Don't navigate when _currentHistoryEntry has an empty ViewerState, because that happens
			// when loading the first collection, and the ViewerState will soon transition from empty to default
			// when the collection finishes loading - navigating for both ViewerState changes would result in
			// two travel log entries for the same state.
			if (string.IsNullOrEmpty(_currentHistoryEntry.ViewerState))
			{
				_currentHistoryEntry.ViewerState = _pivotViewer.ViewerState;
			}
			else
			if (!_isNavigating)
			{
				// If a navigation isn't already in progress, this ViewerState change is from user input, so
				// navigate to a new travel log entry for the new state.
				App.Current.Host.NavigationState = GetUniqueNavigationStateKey();
			}
			else
			{
				// Otherwise skip setting the navigation state (which would cause another navigation) and an
				// infinite feedback loop, and record that the navigation is finished
				_isNavigating = false;
			}

			if ((_lastFocusedItem != null) && (_focusedItem == null) && (_pivotViewer.CurrentItemId == null))
			{
				// Items in focus are reset on ViewerState resets
				_lastFocusedItem = null;
				_focusedItem = null;
			}
		}

		private void PivotViewerNavigation(string oldNavigationState, string newNavigationState)
		{
			if (!newNavigationState.StartsWith("/viewerStateKey/") && !string.IsNullOrEmpty(newNavigationState))
				return;

			PivotViewerHistoryEntry newEntry = null;

			if (_history.TryGetValue(newNavigationState, out newEntry)
				&& (_pivotViewer.CollectionUri != newEntry.CollectionUri
					|| _pivotViewer.ViewerState != newEntry.ViewerState))
			{
				// There is a mapping from the navigation state to a travel log entry, and the state in the travel log entry
				// is different from the current PivotViewer state, so apply the travel log entry state to get the appropriate
				// forward/back effect

				_isNavigating = true;
				_hasItemIdBeenUpdatedFromCollectionLoad = false;

				// Before updating the ViewerState, update the mapping from navigation state to refer to the current ViewerState
				AddCurrentStateToTravelLog(oldNavigationState);

				// Load the collection and ViewerState in the travel log entry, and set the targeted focus item which will be focused
				// once the new ViewerState has settled
				_pivotViewer.LoadCollection(newEntry.CollectionUri.ToString(), newEntry.ViewerState);
				_pivotViewer.CurrentItemId = newEntry.CurrentItem;

				// Build up a new current entry for the state that will be loaded
				_currentHistoryEntry = new PivotViewerHistoryEntry(newEntry.CollectionUri, newEntry.ViewerState);
			}
			else
			{
				// There is no mapping, or the state is the same as the existing state (for example, user moved from grid view
				// to graph view, and back to grid view, then uses the travel log dropdown to back up two entries in one step,
				// this should result in no state change to PivotViewer, except for possibly a current item change)
				if (newEntry != null)
				{
					AddCurrentStateToTravelLog(oldNavigationState, _pivotViewer.CurrentItemId);
					_pivotViewer.CurrentItemId = newEntry.CurrentItem;
				}
				else
					AddCurrentStateToTravelLog(oldNavigationState);

				// Build up a new current entry for the existing current state
				_currentHistoryEntry = new PivotViewerHistoryEntry(_pivotViewer.CollectionUri, _pivotViewer.ViewerState);
			}
		}

		private void QueueDelayedViewerStateChangedHandler()
		{
			// Only queue one delegate at a time, it will handle the final ViewerState after things settle, 
			// but skip intermediate states
			if (_viewerStateChangedHandlerQueued)
				return;

			_viewerStateChangedHandlerQueued = true;
			_pivotViewer.Dispatcher.BeginInvoke((Action)delegate()
			{
				OnViewerStateChanged();
				_viewerStateChangedHandlerQueued = false;
			});
		}

		private void AddCurrentStateToTravelLog(string navigationStateKey)
		{
			string currentItemId = (_hasItemIdBeenUpdatedFromCollectionLoad ? _lastFocusedItem : _focusedItem);
			AddCurrentStateToTravelLog(navigationStateKey, currentItemId);
		}

		private void AddCurrentStateToTravelLog(string navigationStateKey, string currentItemId)
		{
			_currentHistoryEntry.CurrentItem = currentItemId;
			_history[navigationStateKey] = _currentHistoryEntry;
		}

		private string GetUniqueNavigationStateKey()
		{
			return string.Format("/viewerStateKey/{0}", Guid.NewGuid());
		}
	}

	public class PivotViewerHistoryEntry
	{
		public string ViewerState { get; set; }
		public Uri CollectionUri { get; set; }
		public string CurrentItem { get; set; }

		public PivotViewerHistoryEntry(Uri collection, string viewerState)
		{
			CollectionUri = collection;
			ViewerState = viewerState;
		}
	}
}
