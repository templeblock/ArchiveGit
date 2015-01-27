using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.UI.Popups;

namespace ClassLibrary
{
    public static class Dialog
    {
		public static async Task Alert(string content, string title)
		{
			await OKMessage(content, title);
		}

	    public static async Task OKMessage(string content, string title)
	    {
		    int index = await GetResultAsync(content, title, new[] { "OK" }, defaultIndex:0, cancelIndex:0);
	    }

		public static async Task<bool> OKCancelMessage(string content, string title)
		{
			int index = await GetResultAsync(content, title, new[] { "OK", "Cancel" }, defaultIndex:0, cancelIndex:1);
			return (index == 0);
		}

		public static async Task<bool> YesNoMessage(string content, string title)
		{
			int index = await GetResultAsync(content, title, new[] { "Yes", "No" }, defaultIndex:0, cancelIndex:1);
			return (index == 0);
		}

        public static async Task<int> GetResultAsync(string content, string title, IEnumerable<string> labels, int defaultIndex = -1, int cancelIndex = -1)
        {
            try
            {
                var dialog = new MessageDialog(content, title);
				if (defaultIndex >= 0)
					dialog.DefaultCommandIndex = (uint)defaultIndex;
				if (cancelIndex >= 0)
					dialog.CancelCommandIndex = (uint)cancelIndex;

	            int index = 0;
                foreach (var label in labels)
                {
					var prefix = (label != null && !label.StartsWith("&") ? "&" : "");
                    dialog.Commands.Add(new UICommand { Label = prefix + label, Id = index++ });
                }

                var result = await dialog.ShowAsync();
	            index = (result.Id is int ? (int)result.Id : -1);
				return index;
            }
            catch (Exception ex)
            {
                ex.GetType();
                return -1;
            }
        }
    }
}
