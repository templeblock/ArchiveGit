AddEventBase(window, 'load', BodyOnload)

function BodyOnload()
{
	GetPhotoCountsFromServer();
	return false;
}

function GetPhotoCountsFromServer()
{
	var oTargetElement = document.getElementById('idPhotoCounts');
	if (!oTargetElement)
		return;

	oTargetElement.removeAttribute('id'); // So we don't process this element again
	var photoCountsAjaxUrl = document.getElementById('param_photoCountsAjaxUrl').value;
	var sUrl = photoCountsAjaxUrl + '&user=' + oTargetElement.getAttribute('user');
	var callbacks = {
		success: function(oResponse)
		{
			//alert(oResponse.responseText);
			//alert(oTargetElement.innerHTML);
			if (oResponse.responseText.indexOf('<') > 0)
				oTargetElement.innerHTML = oResponse.responseText;

			setTimeout('GetPhotoCountsFromServer()', 100);
		},
		failure: function(oResponse)
		{
			//alert('GetPhotoCountsFromServer(): AJAX failure: ' + 
			//	' tId=' + oResponse.tId +
			//	' status='+ oResponse.status +
			//	' statusText=' + oResponse.statusText
			//	);
		}
	}

	try
	{
		var transaction = YAHOO.util.Connect.asyncRequest('GET', sUrl, callbacks, null);
		//YAHOO.util.Connect.abort(transaction);
	}
	catch(e)
	{
		alert('Error updating the photo counts: ' + e.description);
	}
	return false;
}

function placeholderSetup(id)
{
	var el = document.getElementById(id);
	if (!el)
		return;

	var ph = el.getAttribute('placeholder');
	if (!ph || ph == '')
		return;

	if (el.value == ph)
		el.value = '';
	el.is_focused = (el.value != '');
	if (!el.is_focused)
	{
		el.value = ph;
		el.style.color = '#777';
		el.is_focused = 0;
	}

	AddEventBase(el, 'focus', placeholderFocus);
	AddEventBase(el, 'blur', placeholderBlur);
}

function placeholderFocus()
{
	if (this.is_focused)
		return;

	this.is_focused = 1;
	this.value = '';
	this.style.color = '#000';

	var rs = this.getAttribute('radioselect');
	if (!rs || rs == '')
		return;

	var re = document.getElementById(rs);
	if (!re)
		return;

	if (re.type != 'radio')
		return;

	re.checked = true;
}

function placeholderBlur()
{
	var ph = this.getAttribute('placeholder')
	if (this.is_focused && ph && this.value == '')
	{
		this.is_focused = 0;
		this.value = ph;
		this.style.color = '#777';
	}
}

function AddEventBase(object, type, myfunc)
{
	if (object.addEventListener)
		object.addEventListener(type, myfunc, false);
	else
	if (object.attachEvent)
		object.attachEvent('on' + type, myfunc);
}
