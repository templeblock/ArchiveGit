// http://www.codeproject.com/KB/silverlight/SLUnsavedData.aspx
function SilverlightBeforeUnload(e)
{
	var silverlightControl = document.getElementById("SilverlightApplication").Content;
	if (!silverlightControl)
		return;

	var applicationState = silverlightControl.ApplicationState;
	if (applicationState && !applicationState.IsDirty)
		return;
	var message = (applicationState ? applicationState.Message : 'You may lose all unsaved data in the application.');
	if (!message)
		return;

	if (!e)
		e = window.event;
	e.returnValue = message;

	// IE
	e.cancelBubble = true;

	//e.stopPropagation works in Firefox.
	if (e.stopPropagation)
	{
		e.stopPropagation();
		e.preventDefault();
	}

	// Chrome
	return message;
}
window.onbeforeunload = SilverlightBeforeUnload;