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

	addEventBase(el, 'focus', placeholderFocus);
	addEventBase(el, 'blur', placeholderBlur);
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

function addEventBase(obj, type, fn, name_hash)
{
	if (obj.addEventListener)
		obj.addEventListener(type,fn,false);
	else
	if(obj.attachEvent)
	{
		obj['e' + type + fn + name_hash] = fn;
		obj[type+fn+name_hash] = function() { obj['e' + type + fn + name_hash](window.event); }

		obj.attachEvent('on' + type, obj[type+fn+name_hash]);
	}
}
