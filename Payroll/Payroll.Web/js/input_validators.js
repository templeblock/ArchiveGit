function checkBusinessType(value)
{
	return (value == "SB" || value == "HH");
}

function checkState(value)
{
	return (value.length == 2);
}

function checkEmail(value)
{
	if (value != "")
	{
		var atIndex = value.indexOf('@', 0);
		var at2Index = value.indexOf('@', atIndex + 1);
		var dotIndex = value.indexOf('.', atIndex + 2);
		var spaceIndex = value.indexOf(' ', 0);
		var commaIndex = value.indexOf(',', 0);
		var semIndex = value.indexOf(';', 0);
		if (atIndex < 1 || dotIndex == -1 || spaceIndex != -1
			|| commaIndex >= 0 || semIndex >= 0 || at2Index >= 0
			|| value.charAt(value.length - 1) == '.')
		{
			return false;
		}

		// We know tld isn't empty because of above tests
		// This list of TLD's is accurate as of 3/1/07
		// See http://www.icann.org/registries/top-level-domains.htm
		var tld = value.substring(value.lastIndexOf('.') + 1).toLowerCase();
		if ((tld.length != 2) &&
			((tld != "aero") && (tld != "arpa") && (tld != "biz") && (tld != "cat") &&
				(tld != "com") && (tld != "coop") && (tld != "edu") && (tld != "gov") &&
				(tld != "info") && (tld != "int") && (tld != "jobs") && (tld != "mil") &&
				(tld != "mobi") && (tld != "museum") && (tld != "name") && (tld != "net") &&
				(tld != "org") && (tld != "pro") && (tld != "travel")))
			return false;
	}

	return true;
}

// isSensitized - return true if the value is in the form of a sensitized data
// otherwise return false.  Sensitized data looks like "....3453".
function isSensitized(value)
{
	return ((value.substring(0, 4) == "...."));
}

// checkAllDigits - return true if value is all digits and nothing else, otherwise return false
function checkAllDigits(value)
{
	for (var i = 0; i < value.length; i++)
	{
		var check_char = value.charAt(i);
		if (check_char < '0' || check_char > '9')
			return false;
	}
	return true;
}

// checkNumberRange -- check to see if a number is with a range
// This function was taken from CFUSION/scripts/cfform.js
function checkNumberRange(value, min_value, max_value)
{
	// check minimum
	if (min_value != null)
	{
		if (value < min_value)
			return false;
	}

	// check maximum
	if (max_value != null)
	{
		if (value > max_value)
			return false;
	}

	//All tests passed, so...
	return true;
}

// checkCreditCard - check to see if a credit card number is valid
// This function was taken from CFUSION/scripts/cfform.js and modified
// to handle "sensitized" numbers.
function checkCreditCard(value)
{
	var white_space = " -";
	var creditcard_string = "";
	var check_char;

	if (value.length == 0)
		return true;

	// check for sensitized data - four dots followed by 4 characters eg "....3403"
	if (isSensitized(value))
		return true;

	// squish out the white space
	for (var n = 0; n < value.length; n++)
	{
		check_char = white_space.indexOf(value.charAt(n));
		if (check_char < 0)
			creditcard_string += value.substring(n, (n + 1));
	}

	// if all white space return error
	if (creditcard_string.length == 0)
		return false;

	// make sure number is a valid integer
	if (creditcard_string.charAt(0) == "+")
		return false;

	if (!checkAllDigits(creditcard_string))
		return false;

	// now check mod10

	var doubledigit = creditcard_string.length % 2 == 1 ? false : true;
	var checkdigit = 0;
	var tempdigit;

	for (var i = 0; i < creditcard_string.length; i++)
	{
		tempdigit = eval(creditcard_string.charAt(i));

		if (doubledigit)
		{
			tempdigit *= 2;
			checkdigit += (tempdigit % 10);

			if ((tempdigit / 10) >= 1.0)
				checkdigit++;

			doubledigit = false;
		}
		else
		{
			checkdigit += tempdigit;
			doubledigit = true;
		}
	}

	return (checkdigit % 10) == 0 ? true : false;
}

function checkCreditCardCID(value)
{
	if (value.length == 0)
		return true;

	//cid value must be either a 3 or 4 digit number
	return /^[0-9]{3,4}$/.test(value);
}

function checkRoutingNumber(value)
{
	var white_space = " -+.";
	var routingString = "";
	var check_char;

	// check for sensitized data - four dots followed by 4 characters eg "....3403"
	if (isSensitized(value))
		return true;

	// squish out the white space
	for (var i = 0; i < value.length; i++)
	{
		check_char = white_space.indexOf(value.charAt(i));
		if (check_char < 0)
			routingString += value.substring(i, (i + 1));
	}

	// if not nine characters return error
	if (routingString.length != 9)
		return false;

	// make sure number is only digits
	return checkAllDigits(routingString);
}

// checkFractionalRoutingNumber - return true if value contains only digits and hyphens.
function checkFractionalRoutingNumber(value)
{
	var check_char;

	for (var i = 0; i < value.length; i++)
	{
		check_char = value.charAt(i);
		if ((check_char < '0' || check_char > '9') && check_char != '-' && check_char != ' ')
			return false;
	}
	return true;
}

// checkAccountName - return true if the first character is a digit, otherwise return false
function checkAccountName(value)
{
	var check_char = value.charAt(0);
	if (check_char == '0' || check_char == '1' || check_char == '2' || check_char == '3' || check_char == '4' || check_char == '5' || check_char == '6' || check_char == '7' || check_char == '8' || check_char == '9')
		return false;
	return true;
}

function checkAccountNumber(value)
{
	// check for sensitized data - four dots followed by 4 characters eg "....3403"
	if (isSensitized(value))
		return true;

	// make sure number is a valid integer
	return checkAllDigits(value);
}

// checkSSN - check to see if a Social Security number is valid
// This function was taken from CFUSION/scripts/cfform.js and modified
// to handle "sensitized" numbers.
function checkSSN(value)
{
	var white_space = " -+.";
	var ssc_string = "";
	var check_char;

	if (value.length == 0)
		return true;

	// check for sensitized data - four dots followed by 4 characters eg "....3403"
	if (isSensitized(value))
		return true;

	// if SSN in xxx-xx-xxxx format
	if (value.length == 11)
	{
		// make sure white space is valid
		if (value.charAt(3) != "-" && value.charAt(3) != " ")
			return false;

		if (value.charAt(6) != "-" && value.charAt(6) != " ")
			return false;

		// squish out the white space
		for (var i = 0; i < value.length; i++)
		{
			check_char = white_space.indexOf(value.charAt(i));
			if (check_char < 0)
				ssc_string += value.substring(i, (i + 1));
		}

		// if all white space return error
		if (ssc_string.length != 9)
			return false;
	}
	// if SSN in xxxxxxxxx format
	else
	if (value.length == 9)
		ssc_string = value;
	// Does not support any other format
	else
		return false;

	// make sure number is a valid integer
	if (!checkAllDigits(ssc_string))
		return false;

	// Make sure it matches the regex pattern
	var validRegex1 = /^([1-7]\d{2}|(?:(?!000)\d{3}))([ |-]?)(?:(?!00)\d{2})([ |-]?)(?:(?!0000)\d{4})$/;
	var invalidRegex1 = /^111([ |-]?)11([ |-]?)1111|222([ |-]?)22([ |-]?)2222|333([ |-]?)33([ |-]?)3333|444([ |-]?)44([ |-]?)4444|555([ |-]?)55([ |-]?)5555|666([ |-]?)66([ |-]?)6666|777([ |-]?)77([ |-]?)7777$/;
	var invalidRegex2 = /^123([ |-]?)45([ |-]?)6789$/;
	var invalidRegex3 = /^[89]\d{2}([ |-]?)\d{2}([ |-]?)\d{4}$/;

	if (invalidRegex1.test(ssc_string) ||
			invalidRegex2.test(ssc_string) ||
			invalidRegex3.test(ssc_string))
		return false;

	if (!validRegex1.test(ssc_string))
		return false;

	return true;
}

function checkITIN(value)
{
	var white_space = " -+.";
	var ssc_string = "";
	var check_char;

	if (value.length == 0)
		return true;

	// check for sensitized data - four dots followed by 4 characters eg "....3403"
	if (isSensitized(value))
		return true;

	// if SSN in xxx-xx-xxxx format
	if (value.length == 11)
	{
		// make sure white space is valid
		if (value.charAt(3) != "-" && value.charAt(3) != " ")
			return false;

		if (value.charAt(6) != "-" && value.charAt(6) != " ")
			return false;

		// squish out the white space
		for (var i = 0; i < value.length; i++)
		{
			check_char = white_space.indexOf(value.charAt(i));
			if (check_char < 0)
				ssc_string += value.substring(i, (i + 1));
		}

		// if all white space return error
		if (ssc_string.length != 9)
			return false;
	}
	// if SSN in xxxxxxxxx format
	else
	if (value.length == 9)
		ssc_string = value;
	// Does not support any other format
	else
		return false;

	// make sure number is a valid integer
	if (!checkAllDigits(ssc_string))
		return false;

	var validRegex1 = /^[9]\d{2}([ |-]?)(7[0-9]|8[0-8])([ |-]?)\d{4}$/;
	if (checkSSN(value))
		return true;

	if (validRegex1.test(ssc_string))
		return true;

	return false;
}

function checkLast6DigitsOfSSN(value)
{
	var white_space = " -+.";
	var ssc_string = "";
	var check_char;

	if (value.length == 0)
		return true;

	// if SSN in xx-xxxx format
	if (value.length == 7)
	{
		if (value.charAt(2) != "-" && value.charAt(2) != " ")
			return false;

		// squish out the white space
		for (var i = 0; i < value.length; i++)
		{
			check_char = white_space.indexOf(value.charAt(i));
			if (check_char < 0)
				ssc_string += value.substring(i, (i + 1));
		}

		// if all white space return error
		if (ssc_string.length != 6)
			return false;
	}
	// if SSN in xxxxxx format
	else
	if (value.length == 6)
		ssc_string = value;
	// Does not support any other format
	else
		return false;

	// make sure number is a valid integer
	if (!checkAllDigits(ssc_string))
		return false;

	return true;
}

// checkPhone - check to see if a phone number is valid
// This function was taken from CFUSION/scripts/cfform.js and modified
// to be more generous.
function checkPhone(value)
{
	if (value.length == 0)
		return true;

	if (value.length == 10)
	{
		if (!checkAllDigits(value))
			return false;

		// check if first 3 characters represent a valid area code
		if (!checkNumberRange((eval(value.substring(0, 3))), 100, 1000))
			return false;

		// check if characters 4 - 6 represent a valid exchange
		if (!checkAllDigits((eval(value.substring(3, 6))), 100, 1000))
			return false;

		return true;
	}

	if (value.length != 12)
		return false;

	// now we know the number is 12 characters so we allow '-' ' ' '.' as separators

	// characters 4 and 8 must be "-"
	if (value.charAt(3) != "-" || value.charAt(7) != "-")
		return false;

	// check if first 3 characters represent a valid area code
	if (!checkAllDigits(value.substring(0, 3)))
		return false;

	if (!checkNumberRange((eval(value.substring(0, 3))), 100, 1000))
		return false;

	// check if area code/exchange separator is either a'-' or ' ' or '.'
	if (value.charAt(3) != "-" && value.charAt(3) != " " && value.charAt(3) != ".")
		return false; // check if characters 5 - 7 represent a valid exchange
	if (!checkAllDigits(value.substring(4, 7)))
		return false;

	if (!checkAllDigits((eval(value.substring(4, 7))), 100, 1000))
		return false;

	// check if exchange/number separator is either a'-' or ' ' or '.'
	if (value.charAt(7) != "-" && value.charAt(7) != " " && value.charAt(7) != ".")
		return false;

	return (checkAllDigits(value.substring(8, 12)));
}

//check extension
function checkPhoneExtn(value)
{
	if (value.length == 0)
		return true;
	if (value.length > 10)
		return false;

	return (checkAllDigits(value));
}

// checkFedEIN - check to see if a federal EIN number is valid
function checkFedEIN(value)
{
	var white_space = " -";
	var ein_string = "";
	var check_char;

	if (value.length == 0)
		return true;

	if (value.length != 10)
		return false;

	// make sure white space is valid
	if (value.charAt(2) != "-" && value.charAt(2) != " ")
		return false;

	// squish out the white space
	for (var i = 0; i < value.length; i++)
	{
		check_char = white_space.indexOf(value.charAt(i));
		if (check_char < 0)
			ein_string += value.substring(i, (i + 1));
	}

	// if all white space return error
	if (ein_string.length != 9)
		return false;

	// make sure number is a valid integer
	if (!checkAllDigits(ein_string))
		return false;

	// Make sure it matches the regex pattern
	var validRegex = /^(?:[123456789][0-6]|[0][1-6]|[48][78]|[27][7]|[3569][7-9])([ |-]?)\d{7}$/;
	if (!validRegex.test(ein_string))
		return false;

	return true;
}

// checkFedTIN - check to see if a federal TIN number is valid
function checkFedTIN(value)
{
	var white_space = " -+.";
	var tin_string = "";
	var check_char;

	if (value.length != 11 && value.length != 10)
		return false;

	// make sure white space is valid
	if ((value.charAt(3) != "-" && value.charAt(3) != " " ||
		value.charAt(6) != "-" && value.charAt(6) != " ") &&
	value.charAt(2) != "-" && value.charAt(2) != " ")
		return false;

	// squish out the white space
	for (var i = 0; i < value.length; i++)
	{
		check_char = white_space.indexOf(value.charAt(i));
		if (check_char < 0)
			tin_string += value.substring(i, (i + 1));
	}

	if (tin_string.length != 9 || !checkAllDigits(tin_string))
		return false;

	return true;
}

// checkIAWPN - check to see if a IA Withhodling Permit Number is valid
function checkIAWPN(value)
{
	// empty string and 'applied for' are allowed:
	if (value.length == 0 || value.toLowerCase() == "applied for")
		return true;
	// has to be a 12 digit number but not the example number '123456789001'
	if (value.length != 12 || value == "123456789001" || !checkAllDigits(value))
		return false;

	return true;
}

function checkTime(value)
{
	var white_space = " ";
	var ampm_string = "";
	var check_char;

	// Returns true if value is a number or is NULL
	if (value.length == 0)
		return true;

	// the colon must have an index of 1 or 2
	var colon = value.indexOf(":", 0);
	if (colon < 1 || colon > 2)
		return false;

	// get the hour and make sure it's between 1 and 12
	var hour = value.substring(0, colon);
	if (!checkAllDigits(hour) || parseInt(hour, 10) < 1 || parseInt(hour, 10) > 12)
		return false;

	// get the minutes and make sure it's between 0 and 59
	var minutes = value.substring(colon + 1, colon + 3);
	if (!checkAllDigits(minutes) || parseInt(minutes, 10) < 0 || parseInt(minutes, 10) > 59)
		return false;

	// get the rest and it has to be either AM or PM
	var rest = value.substring(colon + 3, value.length);

	// squish out the white space, maximum of 2 white spaces
	for (var i = 0; i < rest.length; i++)
	{
		check_char = white_space.indexOf(rest.charAt(i));
		if (check_char < 0)
			ampm_string += rest.substring(i, (i + 1));
	}

	// if all white space return error
	if (ampm_string.length != 2)
		return false;

	// if not AM or PM, return error
	if (ampm_string.charAt(1) != "M" && ampm_string.charAt(1) != "m")
		return false;

	if (ampm_string.charAt(0) != "A" && ampm_string.charAt(0) != "a" && ampm_string.charAt(0) != "P" && ampm_string.charAt(0) != "p")
		return false;

	// if AM or PM not consecutive characters, return error
	if (value.indexOf(ampm_string) < 0)
		return false;

	return true;
}

function checkDate(value)
{
	// Returns true if value is a number or is NULL
	if (value.length == 0)
		return true;

	value = value.replace(/[-.]/g, "/");

	// the first dash must have an index of 1 or 2
	var firstDash = value.indexOf("/", 0);
	if (firstDash < 1 || firstDash > 2)
		return false;

	// get the month and make sure it's between 1 and 12
	var month = value.substring(0, firstDash);
	if (!checkAllDigits(month) || parseInt(month, 10) < 1 || parseInt(month, 10) > 12)
		return false;

	// the second dash must have an index 3-5
	var secondDash = value.indexOf("/", firstDash + 1);
	if (secondDash < 3 || secondDash > 5)
		return false;

	// get the day and make sure it's between 1 and 31.
	// we won't check anything more complicated, such as 4/31 or 2/30
	var day = value.substring(firstDash + 1, secondDash);
	if (!checkAllDigits(day) || parseInt(day, 10) < 1 || parseInt(day, 10) > 31)
		return false;

	// make sure the year is in either yy or yyyy format
	var year = value.substring(secondDash + 1);
	if (year.length != 2 && year.length != 4)
		return false;

	// make sure the year is all digits
	if (!checkAllDigits(year))
		return false;

	var intMonth = parseInt(month, 10);
	var intDay = parseInt(day, 10);
	var intYear = parseInt(year, 10);
	if (intMonth == 2)
	{
		if (window.isLeapYear(intYear))
		{
			if (intDay > 29)
				return false;
		}
		else
		{
			if (intDay > 28)
				return false;
		}
	}
	else
	if (intMonth == 1 || intMonth == 3 || intMonth == 5 || intMonth == 7 || intMonth == 8 || intMonth == 10 || intMonth == 12)
	{
		if (intDay > 31)
			return false;
	}
	else
	{
		if (intDay > 30)
			return false;
	}

	return true;
}

// Note that this function does NOT do any validation.  That is up to you.
// Call checkDate() if you need it. Returns 1 if date1>date2, -1 if date1<date2
// or 0 if they are equal.
function compareDate(date1, date2)
{
	var firstDash1 = date1.indexOf("/", 0);
	var firstDash2 = date2.indexOf("/", 0);
	var secondDash1 = date1.indexOf("/", firstDash1 + 1);
	var secondDash2 = date2.indexOf("/", firstDash2 + 1);

	var month1 = eval(date1.substring(0, firstDash1));
	var day1 = eval(date1.substring(firstDash1 + 1, secondDash1));
	var month2 = eval(date2.substring(0, firstDash2));
	var day2 = eval(date2.substring(firstDash2 + 1, secondDash2));

	var year1 = date1.substring(secondDash1 + 1);
	var year2 = date2.substring(secondDash2 + 1);
	if (year1.length == 2 && year2.length == 4) year1 = year2.substring(0, 2) + year1;
	if (year2.length == 2 && year1.length == 4) year2 = year1.substring(0, 2) + year2;

	year1 = eval(year1);
	year2 = eval(year2);

	if (year1 < year2) return -1;
	if (year1 > year2) return 1;
	if (month1 < month2) return -1;
	if (month1 > month2) return 1;
	if (day1 < day2) return -1;
	if (day1 > day2) return 1;

	return 0;
}

function checkPositiveNumber(value)
{
	if (value.length == 0)
		return true;

	var minusSign = "-";
	if (value.indexOf(minusSign) >= 0)
		return false;

	return checkNumber(value);
}

function checkPositiveInteger(value)
{
	if (value.length == 0)
		return true;

	var minusSign = "-";
	if (value.indexOf(minusSign) >= 0)
		return false;

	return checkInteger(value);
}

function checkInteger(value)
{
	//Returns true if value is a number or is NULL
	//otherwise returns false

	if (value.length == 0)
		return true;

	//Returns true if value is an integer defined as
	//   having an optional leading + or -.
	//   otherwise containing only the characters 0-9.
	var decimal_format = ".";
	var check_char;

	//The first character can be + - blank or a digit.
	check_char = value.indexOf(decimal_format); //Was it a decimal?
	if (check_char < 1)
		return checkNumber(value);

	return false;
}

function checkDecimal(value)
{
	//Returns true if value is a decimal number
	//otherwise returns false

	if (value.length == 0)
		return false;

	//Returns true if value is an integer defined as
	//   having an optional leading + or - .
	//   otherwise containing only the characters 0-9.
	var decimal_format = ".";
	var check_char;

	check_char = value.indexOf(decimal_format); //if no decimal was found, return false
	if (check_char < 0)
		return false;

	return checkNumber(value);
}

function checkPercentage(value)
{
	return checkNumber(value) && checkNumberRange(value, 0.0, 100.0);
}


function checkWholePositiveDollar(value)
{
	//Returns true if value is a number or is NULL
	//otherwise returns false

	while (value.charAt(0) == " ")
	{
		value = value.substring(1);
	}
	if (value.length == 0)
		return true;

	//Returns true if value is a number defined as
	//   having an optional leading + or -.
	//   having at most 1 decimal point.
	//   otherwise containing only the characters 0-9.
	var start_format = "0123456789";
	var number_format = " .0123456789";
	var check_char;
	var decimal = false;
	var trailing_blank = false;
	var digits = false;

	//The first character can be a digit.
	check_char = start_format.indexOf(value.charAt(0));
	if (check_char < 1)
		return false;

	//Remaining characters can be only . or a digit, but only one decimal.
	for (var i = 1; i < value.length; i++)
	{
		check_char = number_format.indexOf(value.charAt(i));
		if (check_char < 0)
			return false;

		if (check_char == 1)
		{
			if (decimal)		// Second decimal.
				return false;
			decimal = true;
		}
		else
		if (check_char == 0)
		{
			if (decimal || digits)
				trailing_blank = true;
			// ignore leading blanks
		}
		else
		if (trailing_blank)
			return false;
		else
		if (decimal && check_char != 2) // don't allow any non 0's after the decimal
			return false;
		else
			digits = true;
	}

	//All tests passed, so...
	return true;
}

function checkNumber(value)
{
	//Returns true if value is a number or is NULL
	//otherwise returns false

	while (value.charAt(0) == " ")
	{
		value = value.substring(1);
	}
	if (value.length == 0)
		return true;

	//Returns true if value is a number defined as
	//   having an optional leading + or -.
	//   having at most 1 decimal point.
	//   otherwise containing only the characters 0-9.
	var start_format = " .+-0123456789";
	var number_format = " .0123456789";
	var check_char;
	var decimal = false;
	var trailing_blank = false;
	var digits = false;

	//The first character can be + - . blank or a digit.
	check_char = start_format.indexOf(value.charAt(0)); // if there are only +, - or . char in the value object than return false
	if (value.length == 1 && (check_char == 1 || check_char == 2 || check_char == 3))
		return false;

	//Was it a decimal?
	if (check_char == 1)
		decimal = true;
	else
	if (check_char < 1)
		return false;

	//Remaining characters can be only . or a digit, but only one decimal.
	for (var i = 1; i < value.length; i++)
	{
		check_char = number_format.indexOf(value.charAt(i));
		if (check_char < 0)
			return false;

		if (check_char == 1)
		{
			if (decimal)		// Second decimal.
				return false;
			decimal = true;
		}
		else
		if (check_char == 0)
		{
			if (decimal || digits)
				trailing_blank = true;
			// ignore leading blanks

		}
		else
		if (trailing_blank)
			return false;
		else
			digits = true;
	}

	//All tests passed, so...
	return true;
}

function checkZip(value)
{
	if (value.length == 0)
		return true;

	if (value.length != 5 && value.length != 10)
		return false;

	// make sure first 5 digits are a valid integer
	if (value.charAt(0) == "-" || value.charAt(0) == "+")
		return false;

	if (!checkInteger(value.substring(0, 5)))
		return false;

	if (value.length == 5)
		return true;

	// make sure

	// check if separator is either a'-' or ' '
	if (value.charAt(5) != "-" && value.charAt(5) != " ")
		return false;

	// check if last 4 digits are a valid integer
	if (value.charAt(6) == "-" || value.charAt(6) == "+")
		return false;

	return (checkInteger(value.substring(6, 10)));
}

//check if field is empty.
function hasValue(input)
{
	if (input == null)
		return false;
	if (trim(input.value).length == 0)
		return false;
	return true;
}

//Check if a select box has a selected value.
// Value of -136 below must match SelectTag.CHOOSEITEMID
function hasSelectedValue(object, value)
{
	value = trim(value);
	if ((value.length == 0) || (value == '-136'))
		return false;

	return true;
}

// prevent double click
var form_submitted = false;
function oneClick()
{
	if (form_submitted)
		return false;

	form_submitted = true;
	return true;
}

// allow only one click from the page
// This is different from oneClick(), since that gets reset by pc:form - See taglib code.
var oneForm_submitted = false;
function oneClickOnly()
{
	if (oneForm_submitted)
		return false;

	oneForm_submitted = true;
	return true;
}

//Set the form submitted variable. Return the parameter.
function setFormSubmitted(val)
{
	form_submitted = val;
	return val;
}

function checkIrsPin(value)
{
	if (value.length == 0)
		return true;

	if (value.length != 10)
		return false;

	if (!checkInteger(value))
		return false;

	return true;
}

function checkDesigneePin(value)
{
	if (value.length != 5)
		return false;

	if (!checkAlphaNumeric(value) && value != null)
		return false;

	return true;
}

function confirmInteger(objValue, objMin, objMax)
{
	if (objValue.length == 0)
		return true;

	var value;
	if (!checkInteger(objValue))
		value = 0;

	if (objMin != null && objMin != "null" && objMin > objValue)
		return false;

	if (objMax != null && objMax != "null" && objMax < objValue)
		return false;

	return true;
}

function confirmNumber(objValue, objMin, objMax)
{
	if (objValue.length == 0)
		return true;

	var value;
	if (!checkNumber(objValue))
		value = 0;

	if (objMin != null && objMin != "null" && objMin > objValue)
		return false;

	if (objMax != null && objMax != "null" && objMax < objValue)
		return false;

	return true;
}

function confirmDate(objValue, objMin, objMax)
{
	if (objValue.length == 0)
		return true;

	if (!checkDate(objValue))
		return true;

	if (objMin != null && objMin != "null" && compareDate(objValue, objMin) < 0)
		return false;

	if (objMax != null && objMax != "null" && compareDate(objValue, objMax) > 0)
		return false;

	return true;
}

// Trims a string to get rid of leading or trailing spaces..
function trim(value)
{
	var temp = value;
	var obj = /^(\s*)([\W\w]*)(\b\s*$)/;
	if (obj.test(temp))
		temp = temp.replace(obj, '$2');

	obj = / +/g;
	temp = temp.replace(obj, " ");
	if (temp == " ")
		temp = "";

	return temp;
}

function checkHours(value)
{
	if ((value == "hours") || (value == "hrs"))
		value = "";

	// Shouldn't this be checkPositiveNumber?  Not sure why we allow negative hours
	return checkNumber(value);
}

function checkQuarterStartDate(value)
{
	// must be a date:
	if (!checkDate(value))
		return false;

	// get the first dash and the second dash:
	var firstDash = value.indexOf("/", 0);
	var secondDash = value.indexOf("/", firstDash + 1);
	var MonthDay = value.substring(0, secondDash);
	if (MonthDay == "1/1" || MonthDay == "01/01" || MonthDay == "01/1" || MonthDay == "1/01" ||
		MonthDay == "4/1" || MonthDay == "04/01" || MonthDay == "04/1" || MonthDay == "4/01" ||
		MonthDay == "7/1" || MonthDay == "07/01" || MonthDay == "07/1" || MonthDay == "7/01" ||
		MonthDay == "10/1" || MonthDay == "10/01")
		return true;
	return false;
}

function checkAlphaNumeric(value)
{
	//Returns true if value is alphanumeric or is NULL
	//otherwise returns false

	value = trim(value);
	if (value.length == 0)
		return true;

	//Returns true if value is a number defined as
	//   having  containing only the characters a-z and 0-9.
	var name_format = "abcdefghijklmnopqrstuvwxyzABCEDFGHIJKLMNOPQRSTUVWXYZ0123456789";
	var check_char;

	//Remaining characters can be only . or a digit, but only one decimal.
	for (var i = 0; i < value.length; i++)
	{
		check_char = name_format.indexOf(value.charAt(i));
		if (check_char < 0) return false;
	}

	//All tests passed, so...
	return true;
}
