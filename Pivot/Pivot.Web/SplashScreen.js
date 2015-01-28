function onSourceDownloadProgressChanged(sender, eventArgs)
{
	if (sender == null)
		return;

	var progress = (eventArgs != null ? eventArgs.progress : 1.0);
	var progressText = Math.round(progress * 100) + "%";

	var label = sender.findName("x_Label");
	if (label != null)
		label.Text = progressText;

	var rectBar = sender.findName("x_RectBar");
	var rectBorder = sender.findName("x_RectBorder");
	if (rectBar != null && rectBorder != null)
		rectBar.Width = progress * rectBorder.Width;
}

function onSourceDownloadComplete(sender, eventArgs)
{
	if (sender == null)
		return;

	var label = sender.findName("x_Label");
	if (label != null)
		label.Text = "Complete!";
//	var storyboard = sender.FindName("x_Storyboard");
//	if (storyboard != null)
//		storyboard.Stop();
}
