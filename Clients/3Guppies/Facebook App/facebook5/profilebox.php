<?php
	// The facebook client library
	include_once 'facebook-platform/php4client/facebook.php';

	// This defines some of your basic setup
	include_once 'config.php';
	include_once 'facebook.lib.php';

	$facebook = new Facebook($api_key, $secret);
	$user = $facebook->get_loggedin_user();
	$userid = (int)Param('id', $user);
//j	$firstName = GetUserInfo($userid, 'first_name');
?>

<!--fbml-->
<fb:profile-action url="<?= $canvasPageUrl; ?>mobilize.php">3Guppies Mobilizer</fb:profile-action>
<!--fb:wide-->

<!-- Set the canvas page title -->
<fb:title>3Guppies Mobilizer Title</fb:title>

<!--
<fb:subtitle seeallurl="<?= $canvasPageUrl ?>mobilize.php">
	<fb:action href="<?= $aboutPageUrl ?>">About</fb:action>
	<fb:action href="<?= $canvasPageUrl ?>help.php">Help</fb:action>
	Displaying your 3Guppies profile
</fb:subtitle>
-->

<div>
	Enter the <a href="<?= $canvasPageUrl; ?>mobilize.php">3Guppies Mobilizer</a> application now.<br/>
	<br/>
	You last visited on <?= StdDate(); ?> PST<br/>
	<br/>
	Visit our main web site <a target="_blank" href="http://www.3guppies.com">3Guppies.com</a> where you can do so much more.<br/>
	<br/>
	<ul>
		<li>Upload photos from your desktop computer and send then to any mobile phone.</li>
		<li>Upload music from your desktop computer and send it to any mobile phone to use as a ringtone.</li>
		<li>All for FREE!</li>
	</ul>
</div>

<!--
<a id="toggleswf" href="#" clicktotoggle="swf">Show/Hide the widget below</a><br/>
<div id="swf" style="display:block;">
<fb:swf width="388" height="255" 
	swfsrc="http://gg.internal.mixxer.com/~cbabcock/146563/images/flash/widget/base.swf"
	swfbgcolor="8080ff"
	waitforclick="false"
	salign="t"
	imgsrc="http://gg.internal.mixxer.com/~jmccurdy/146004/facebook/widget_preloaded.jpg"
	imgstyle="border:0px;"
	imgclass=""
	flashvars="dir_path=http://gg.internal.mixxer.com/~cbabcock/146563/images/flash/widget/&site_main=http://gg.internal.mixxer.com/~cbabcock/146563/&load_debug=true&g3_widgetid=123123&fb_userid=<?= $userid ?>&unused_g3_userid=4336837&unused_g3_directorscut=false"
	/>
<img src="http://gg.internal.mixxer.com/~cbabcock/146563/images/flash/widget/widget_html_foot.gif" style="width:388px;" />
</div>
<br/>
-->

<!--
<fb:tabs>
	<fb:tab-item href="#" title="Friends" selected="true" />
	<fb:tab-item href="#" title="Invite" />
</fb:tabs>
<br/>
-->

<!--
<fb:share-button class="meta">
	<meta name="medium" content="blog"/>
	<meta name="title" content="Leonidas in All of Us"/>
	<meta name="video_type" content="application/x-shockwave-flash"/>
	<meta name="video_height" content="345"/>
	<meta name="video_width" content="473"/>
	<meta name="description" content="That's the lesson 300 teaches us."/>
	<link rel="image_src" href="http://9.content.collegehumor.com/d1/ch6/f/6/collegehumor.b38e345f621621dfa9de5456094735a0.jpg"/>
	<link rel="video_src" href="http://www.collegehumor.com/moogaloop/moogaloop.swf?clip_id=1757757&autoplay=true"/>
	<link rel="target_url" href="http://www.collegehumor.com/video:1757757"/>
</fb:share-button>
-->

<!--/fb:wide-->

<!--
<fb:dashboard>
	<fb:help href="<?= $canvasPageUrl ?>help.php" title="Click here for Help on the 3Guppies Mobilizer">Help</fb:help>
</fb:dashboard>
-->

<form method="get" action="<?= $canvasPageUrl ?>send.php">
	Invite a friend: <fb:friend-selector idname="id" />
	<fb:submit>Submit</fb:submit>
	<input type='hidden' name='invitation' value='1'>
</form>

<!--/fbml-->
