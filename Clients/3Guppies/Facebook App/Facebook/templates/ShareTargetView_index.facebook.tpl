{* using variables:
	$smarty.const.FACEBOOK_APPNAME
	$smarty.const.FACEBOOK_CANVAS_URL
	$smarty.const.FACEBOOK_IMAGES_URL
*}

{* This page immediately redirects to the 3Guppies Facebook app, so no content is every displayed *}
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" >
<head>
    <title>{$smarty.const.FACEBOOK_APPNAME}</title>
	<meta name="title" content="{$smarty.const.FACEBOOK_APPNAME} - Check it out!" />
	<meta name="description" content="{$smarty.const.FACEBOOK_APPNAME} let's you send photos from Facebook to any mobile phone in the US -- FREE. You can make a wallpaper or screensaver for yourself or surprise a friend with a picture message! You can even crop the photos first to better fit your phone. So start telling your stories with photos. After all, a picture is worth a thousand words." />
	<meta http-equiv="refresh" content="0;url={$smarty.const.FACEBOOK_CANVAS_URL}Mobilize" />
	<link rel="image_src" href="{$smarty.const.FACEBOOK_IMAGES_URL}FB_hdr_logo.gif" />
</head>
<body>
</body>
</html>
