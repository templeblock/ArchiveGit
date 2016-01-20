<?php
	$bFishTank = (strpos(URL_BASE, 'www.') <= 0);
	define( 'FACEBOOK_FISHTANK', $bFishTank );
	// Get these from http://developers.facebook.com
	if (!FACEBOOK_FISHTANK)
	{ // The is the "real" 3Guppies app
		define( 'FACEBOOK_API_KEY', '9e7c41632d72e584593d3174e660ce6c' );
		define( 'FACEBOOK_SECRET', '28a8162e6d52bae69b13f811454f4819' );
		define( 'FACEBOOK_APPID', '3097070316' );
		define( 'FACEBOOK_APPNAME', '3Guppies Mobile' );
		define( 'FACEBOOK_CANVAS_URL', 'http://apps.facebook.com/guppies/');
	}
	else
	{ // The is the "dev" 3Guppies app
		define( 'FACEBOOK_API_KEY', 'fefb5c21111a9e7d8d2acf6f9737b009' );
		define( 'FACEBOOK_SECRET', '65cc66495c22083e869e5fbea96a6baf' );
		define( 'FACEBOOK_APPID', '5503158802' );
		define( 'FACEBOOK_APPNAME', '3Guppies Fishtank' );
		define( 'FACEBOOK_CANVAS_URL', 'http://apps.facebook.com/guppies_fishtank/');
	}

	define( 'FACEBOOK_IMAGES_URL', URL_BASE . 'images/facebook/' );

	define( 'CALLBACK_URL_PREFIX', URL_BASE . 'ringtones/module/' );
	define( 'FACEBOOK_CALLBACK_URL', CALLBACK_URL_PREFIX . 'Facebook/fb/1/action/' );
	define( 'FACEBOOK_OPENLOCKER_URL', CALLBACK_URL_PREFIX . 'OpenLocker/fb/1/action/' );

	define( 'FACEBOOK_URL', 'http://www.facebook.com/' );
	define( 'FACEBOOK_ABOUT_URL', FACEBOOK_URL . 'apps/application.php?id=' . FACEBOOK_APPID );
	define( 'FACEBOOK_ACTIVITY_URL', FACEBOOK_URL . 'minifeed.php?filter=23&app_id=' . FACEBOOK_APPID . '&id=' );
	define( 'FACEBOOK_DISCUSSION_URL', FACEBOOK_URL . 'board.php?uid=' . FACEBOOK_APPID );
	define( 'FACEBOOK_DEFAULT_PIC_URL', FACEBOOK_URL . 'gpic/s_default.jpg' );
//	define( 'FACEBOOK_DEFAULT_PIC_URL', 'http://static.ak.facebook.com/pics/s_default.jpg' );

	$GLOBALS['facebook_config']['debug'] = false;
?>
