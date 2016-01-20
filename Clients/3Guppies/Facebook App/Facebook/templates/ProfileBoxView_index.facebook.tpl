{* using variables:
	$userid
*}

{*<fbml>*}

{* Profile action link *}
{if (!$smarty.const.FACEBOOK_FISHTANK)} {* Only add a profile action link for the production app *}
	<fb:profile-action url="{$smarty.const.FACEBOOK_CANVAS_URL}Mobilize">{$smarty.const.FACEBOOK_APPNAME}</fb:profile-action>
{/if}

{if ($smarty.const.FACEBOOK_FISHTANK)} {* No content for the live app *}

{*<fb:wide>*}

{* Dashboard bar *}
{*
<fb:dashboard>
	<fb:help href="{$smarty.const.FACEBOOK_CANVAS_URL}Help" title="Click here for Help on the {$smarty.const.FACEBOOK_APPNAME}">Help</fb:help>
</fb:dashboard>
*}

{* Tabs bar *}
{*
<fb:tabs>
	<fb:tab-item href="#" title="Friends" selected="true" />
	<fb:tab-item href="#" title="Invite" />
</fb:tabs>
<br/>
*}

{* Subtitle bar *}
<fb:subtitle>
	Last visited {$lastVisited} PST
	<fb:action href="{$smarty.const.FACEBOOK_CANVAS_URL}Mobilize">Send Photos</fb:action>
	<fb:action href="{$smarty.const.FACEBOOK_CANVAS_URL}Invite">Invite Friends</fb:action>
{*
	<fb:action href="{$smarty.const.FACEBOOK_CANVAS_URL}Help">Help</fb:action>
	<fb:action target="_blank" href="{$smarty.const.FACEBOOK_ABOUT_URL}">About</fb:action>
*}
</fb:subtitle>

{* Info *}
<div>
{*
	Go to the <a href="{$smarty.const.FACEBOOK_CANVAS_URL}Mobilize">{$smarty.const.FACEBOOK_APPNAME}</a> application now.<br/>
	<br/>
 *}
{*
	You last visited {$smarty.const.FACEBOOK_APPNAME} on {$lastVisited} PST<br/>
	<br/>
*}

{*
	<a href="{$smarty.const.FACEBOOK_CANVAS_URL}Mobilize">Send Photos</a> to a mobile phone.<br/>
	<a href="{$smarty.const.FACEBOOK_CANVAS_URL}Invite">Invite Friends</a> to try it!<br/>
	<a href="{$smarty.const.FACEBOOK_CANVAS_URL}Help">Help</a>!&nbsp; I'm not sure what to do.<br/>
	<a target="_blank" href="{$smarty.const.FACEBOOK_ABOUT_URL}">About</a> {$smarty.const.FACEBOOK_APPNAME}.<br/>
	<br/>
*}
	Visit our main web site <a target="_blank" href="http://www.3guppies.com">3Guppies.com</a> where you can do so much more.<br/>
</div>

{* Widget *}
<a id="toggleswf" href="#" clicktotoggle="swf">Show/Hide the widget below</a><br/>
<br/>
<div id="swf" style="display:block;">
{$widgetCode}
</div>
<br/>

{* Share button *}
<fb:share-button class="url" href="{$smarty.const.FACEBOOK_CALLBACK_URL}ShareTarget" />

{* Invite friend *}
{*
<form method="get" action="{$smarty.const.FACEBOOK_CANVAS_URL}Send">
	Invite a friend: <fb:friend-selector idname="id" />
	<fb:submit>Submit</fb:submit>
	<input type='hidden' name='invitation' value='1'>
</form>
*}

{*</fb:wide*}
{/if}

{*</fbml>*}
