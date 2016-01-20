function SelectAll(form_id, name, bChecked)
{
	var formblock = document.getElementById(form_id);
	var forminputs = formblock.getElementsByTagName('input');
	var iCount = (bChecked ?  Math.min(10, forminputs.length) : forminputs.length);
	for (i = 0; i < iCount; i++)
	{
		// regex here to check name attribute
		var regex = new RegExp(name, "i");
		if (regex.test(forminputs[i].getAttribute('name')))
			forminputs[i].checked = bChecked;
	}
}
function ConfirmSubmit(form_id)
{
	var formblock = document.getElementById(form_id);
	var forminputs = formblock.getElementsByTagName('input');
	var iCount = 0;
	for (i = 0; i < forminputs.length; i++)
	{
		if (forminputs[i].checked)
			iCount++;
	}

	if (iCount > 10)
	{
		var sMessage = 'You can only invite 10 friends per day, and you have selected ' + iCount + ' names.\n\nPlease unselect ' + (iCount-10) + ' names and try again.';
		alert(sMessage);
		return false;
	}

	return true;
}
