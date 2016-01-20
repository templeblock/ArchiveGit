function onSourceDownloadProgressChanged(sender, eventArgs)
{
	var aspSilverlightControl = false; // No longer used
	var root = (!aspSilverlightControl ? sender : sender.get_element().content);
	var label = root.findName("x_Label");
	var rectBar = root.findName("x_RectBar");
	var rectBorder = root.findName("x_RectBorder");
	if (label == null || rectBar == null || rectBar == null)
		return;

	var progress = (!aspSilverlightControl ? eventArgs.progress : eventArgs.get_progress());
	label.Text = Math.round(progress * 100) + "%";
	rectBar.Width = progress * rectBorder.Width;
}

function onSourceDownloadComplete(sender, eventArgs)
{
	var aspSilverlightControl = false; // No longer used
	var root = (!aspSilverlightControl ? sender : sender.get_element().content);
	var label = root.findName("x_Label");
	var rectBar = root.findName("x_RectBar");
	var rectBorder = root.findName("x_RectBorder");
	if (label == null || rectBar == null || rectBar == null)
		return;

	label.Text = "Complete!";
	var storyboard = root.FindName("x_Storyboard");
	if (storyboard != null)
		storyboard.Stop();
}
