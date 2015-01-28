$(document).ready(function ()
{
//	// This script is for the validationEngine.jquery.css panels
	//	$('#_EnrollForm').validationEngine({
//		inlineValidation: false,
//		success: false,
//		failure: function () { validationSuccess = false; }
//	});

	$('#_EnrollForm').submit(function ()
	{
		$('#_Error').hide();
		var isValid = EnrollValidator(this);
		if (!setFormSubmitted(isValid))
			return false;

		$('.ui-dialog-titlebar').hide();
		$('#_ProgressBarDialog').dialog('open');
		$.post('EnrollPost.aspx', $(this).serialize(), function (response)
		{
			$('#_ProgressBarDialog').dialog('close');
			$('.ui-dialog-titlebar').show();
			if (parseInt(response) == -1)
			{
				response = 'Sorry, but error ' + response + ' occurred.  Please try again.';
				//$('#_Dialog').html(response).dialog('open');
				$('#_Error').html(response).show();
			}
			else
			if (response.indexOf('Error:') >= 0)
			{
				$('#_Error').html(response).show();
			}
			else
			{
				$('#_EnrollForm').hide();
				$('#_ResultValue').html(response);
				$('#_Result').show();
			}
		});

		return false;
	});

	$('#_CreateAccountForm').submit(function ()
	{
		$('#_Error').hide();
		var isValid = CreateAccountValidator(this);
		if (!setFormSubmitted(isValid))
			return false;

		$('.ui-dialog-titlebar').hide();
		$('#_ProgressBarDialog').dialog('open');
		$.post('CreateAccountPost.aspx', $(this).serialize(), function (response)
		{
			$('#_ProgressBarDialog').dialog('close');
			$('.ui-dialog-titlebar').show();
			if (parseInt(response) == -1)
			{
				response = 'Sorry, but error ' + response + ' occurred.  Please try again.';
				//$('#_Dialog').html(response).dialog('open');
				$('#_Error').html(response).show();
			}
			else
			if (response.indexOf('Error:') >= 0)
			{
				$('#_Error').html(response).show();
			}
			else
			{
				$('#_CreateAccountForm').hide();
				$('#_ResultValue').html(response);
				$('#_Result').show();
			}
		});

		return false;
	});

	$('#_Dialog').dialog({
		modal: true,
		autoOpen: false,
		width: 600,
		buttons: {
			'Ok': function ()
			{
				$(this).dialog('close');
			}
		}
	});

	$('#_ProgressBarDialog').dialog({
		modal: true,
		autoOpen: false,
		width: 300,
		height: 40,
		draggable: false,
		resizable: false,
		title: null
	});

	$('#_ProgressBar').progressbar({
		value: 0
	});

	$(function ()
	{
		setInterval(update, 25);
	});

	function update()
	{
		var val = $('#_ProgressBar').progressbar('option', 'value');
		val = parseInt(val);
		val++; if (val >= 100) val = 0;
		$('#_ProgressBar').progressbar({
			value: val
		});
	}
});

function CreateAccountValidator(form)
{
	try
	{
//		if (hasValue(form._UserId) &&
//			hasValue(form._UserId2) &&
//			form._UserId.value != form._UserId2.value))
//		{
//			alert('Your user ids do not match.');
//			return false;
//		}

		if (!hasValue(form._UserId))
		{
			alert('Please enter your user id with at least 6 characters and at least 1 non-letter.');
			return false;
		}

		return EnrollValidator(form);
	}
	catch (e)
	{
		alert('Validation exception ' + e);
		return false;
	}
}

function EnrollValidator(form)
{
	try
	{
		if (!hasValue(form._BusinessType) ||
			!checkBusinessType(form._BusinessType.value))
		{
			alert('Please select your business type.');
			return false;
		}

		var isHousehold = (form._BusinessType.value == 'HH');
		if (isHousehold && hasValue(form._State))
		{
			var supported = true;
			switch (form._State.value)
			{
				case 'AZ': case 'AR': case 'DE': case 'IL': case 'KS':
				case 'MA': case 'MT': case 'NJ': case 'NC': case 'OH':
				case 'OR': case 'PA': case 'RI': case 'WA': case 'WI':
					{
						supported = false;
						break;
					}
				default:
					break;
			}

			if (!supported)
			{
				alert('Sorry, but our household employee service is not currently available in AZ, AR, DE, IL, KS, MA, MT, NJ, NC, OH, OR, PA, RI, WA, WI');
				return false;
			}
		}

		var bizType = (isHousehold ? "household" : "business");
		if (!hasValue(form._BizName))
		{
			alert('Please enter the name of your ' + bizType + '.');
			return false;
		}

		if (!hasValue(form._Address))
		{
			alert('Please enter your ' + bizType + ' address.');
			return false;
		}

		if (!hasValue(form._City))
		{
			alert('Please enter your ' + bizType + ' city.');
			return false;
		}

		if (!hasValue(form._State) ||
			!checkState(form._State.value))
		{
			alert('Please select your ' + bizType + ' state.');
			return false;
		}

		if (!hasValue(form._ZipCode) ||
			!checkZip(form._ZipCode.value))
		{
			alert('Please enter your ' + bizType + ' zip code as a 5 digit number.');
			return false;
		}

		if (!hasValue(form._FirstName))
		{
			alert('Please enter your first name.');
			return false;
		}

		if (!hasValue(form._LastName))
		{
			alert('Please enter your last name.');
			return false;
		}

		if (!hasValue(form._Phone) ||
			!checkPhone(form._Phone.value))
		{
			alert('Please enter a valid phone number.');
			return false;
		}

		if (!hasValue(form._EmailAddr) ||
			!checkEmail(form._EmailAddr.value))
		{
			alert('You must enter a valid e-mail address.');
			return false;
		}

		if (typeof(form._EmailAddr2) != 'undefined')
		{
			if (!hasValue(form._EmailAddr2))
			{
				alert('You must confirm your e-mail address.');
				return false;
			}

			if (form._EmailAddr.value != form._EmailAddr2.value)
			{
				alert('Your e-mail addresses must match.');
				return false;
			}
		}

		return true;
	}
	catch (e)
	{
		alert('Validation exception ' + e);
		return false;
	}
}
