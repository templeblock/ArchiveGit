{* using variables:
	$userName
	$arrUsersMe
	$arrUsersHasTagged
	$arrUsersTaggedBy
	$arrUsersTaggedWith
	$photoCountsAjaxUrl
	$smarty.const.FACEBOOK_CANVAS_URL
	$arrUsers[n].userid
	$arrUsers[n].name
	$arrUsers[n].firstName
	$arrUsers[n].bIsAppUser
	$arrUsers[n].picUrl
	$arrUsers[n].hisHerTheir
	$arrUsers[n].himHerThem
	$arrUsers[n].numPhotos
	$arrUsers[n].numAlbums
	$arrUsers[n].networks
*}

{addheaderfile file="http://yui.yahooapis.com/2.2.2/build/yahoo/yahoo-min.js"}
{addheaderfile file="http://yui.yahooapis.com/2.2.2/build/connection/connection-min.js"}
{addheaderfile file="modules/Facebook/javascript/FriendsView.js"}
{addheaderfile file="modules/Facebook/css/FacebookGlobal.css"}
{addheaderfile file="modules/Facebook/css/FriendsView.css"}

<input type="hidden" id="param_photoCountsAjaxUrl" value="{$photoCountsAjaxUrl}" />

<div id="friendtables">
	{foreach item=usr from=$arrUsersMe}
        <table cellspacing="0" cellpadding="4" border="0">
            <tr style="vertical-align:top; ">
                <td style="width: 100px; min-height: 100px; ">
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Mobilize?user={$usr.userid}"><img src="{$usr.picUrl}" alt="" /></a>
                </td>
                <td style="width: 200px;">
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Mobilize?user={$usr.userid}">{$usr.name}</a><br/>
					<span id="idPhotoCounts" user="{$usr.userid}"> <!-- Ajax calls will replace this span's inner html -->
	                    {$usr.numPhotos} Photo{if $usr.numPhotos != 1}s{/if}<br/>
	                    {$usr.numAlbums} Album{if $usr.numAlbums != 1}s{/if}<br/>
					</span>
                </td>
                <td style="width: 250px; text-align: right;">
                    {if ($usr.bIsAppUser)}
                        <a target="_parent" href="{$smarty.const.FACEBOOK_ACTIVITY_URL}{$usr.userid}">View {$usr.firstName}'s recent 3Guppies activity</a>
                    {else}
                        <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Invite?user={$usr.userid}">Invite {$usr.firstName} to 3Guppies!</a>
                    {/if}
                    <br />
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}FriendsOfFriends?user={$usr.userid}">Browse {$usr.hisHerTheir} Tagged Photo Friends</a><br/>
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}FriendsOfFriends?user={$usr.userid}" style="text-decoration: none; color: black;">
	                    {$usr.networks}
					</a>
                </td>
            </tr>
            <tr><td colspan="3"><hr /></td></tr>
        </table>
	{/foreach}

	<div style="width:100%; height:16px; font:bold 11pt Arial; text-align:center;">
		{$userName} has tagged the following people:
	</div>
	<br/>
	{foreach item=usr from=$arrUsersHasTagged}
        <table cellspacing="0" cellpadding="4" border="0">
            <tr style="vertical-align:top; ">
                <td style="width: 100px; min-height: 100px; ">
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Mobilize?user={$usr.userid}"><img src="{$usr.picUrl}" alt="" /></a>
                </td>
                <td style="width: 200px;">
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Mobilize?user={$usr.userid}">{$usr.name}</a><br/>
					<span id="idPhotoCounts" user="{$usr.userid}"> <!-- Ajax calls will replace this span's inner html -->
	                    {$usr.numPhotos} Photo{if $usr.numPhotos != 1}s{/if}<br/>
	                    {$usr.numAlbums} Album{if $usr.numAlbums != 1}s{/if}<br/>
					</span>
                </td>
                <td style="width: 250px; text-align: right;">
                    {if ($usr.bIsAppUser)}
                        <a target="_parent" href="{$smarty.const.FACEBOOK_ACTIVITY_URL}{$usr.userid}">View {$usr.firstName}'s recent 3Guppies activity</a>
                    {else}
                        <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Invite?user={$usr.userid}">Invite {$usr.firstName} to 3Guppies!</a>
                    {/if}
                    <br />
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}FriendsOfFriends?user={$usr.userid}">Browse {$usr.hisHerTheir} Tagged Photo Friends</a><br/>
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}FriendsOfFriends?user={$usr.userid}" style="text-decoration: none; color: black;">
	                    {$usr.networks}
					</a>
                </td>
            </tr>
            <tr><td colspan="3"><hr /></td></tr>
        </table>
	{/foreach}

	<div style="width:100%; height:16px; font:bold 11pt Arial; text-align:center;">
		{$userName} has been tagged BY the following people:
	</div>
	<br/>
	{foreach item=usr from=$arrUsersTaggedBy}
        <table cellspacing="0" cellpadding="4" border="0">
            <tr style="vertical-align:top; ">
                <td style="width: 100px; min-height: 100px; ">
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Mobilize?user={$usr.userid}"><img src="{$usr.picUrl}" alt="" /></a>
                </td>
                <td style="width: 200px;">
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Mobilize?user={$usr.userid}">{$usr.name}</a><br/>
					<span id="idPhotoCounts" user="{$usr.userid}"> <!-- Ajax calls will replace this span's inner html -->
	                    {$usr.numPhotos} Photo{if $usr.numPhotos != 1}s{/if}<br/>
	                    {$usr.numAlbums} Album{if $usr.numAlbums != 1}s{/if}<br/>
					</span>
                </td>
                <td style="width: 250px; text-align: right;">
                    {if ($usr.bIsAppUser)}
                        <a target="_parent" href="{$smarty.const.FACEBOOK_ACTIVITY_URL}{$usr.userid}">View {$usr.firstName}'s recent 3Guppies activity</a>
                    {else}
                        <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Invite?user={$usr.userid}">Invite {$usr.firstName} to 3Guppies!</a>
                    {/if}
                    <br />
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}FriendsOfFriends?user={$usr.userid}">Browse {$usr.hisHerTheir} Tagged Photo Friends</a><br/>
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}FriendsOfFriends?user={$usr.userid}" style="text-decoration: none; color: black;">
	                    {$usr.networks}
					</a>
                </td>
            </tr>
            <tr><td colspan="3"><hr /></td></tr>
        </table>
	{/foreach}

	<div style="width:100%; height:16px; font:bold 11pt Arial; text-align:center;">
		{$userName} has been tagged WITH the following people:
	</div>
	<br/>
	{foreach item=usr from=$arrUsersTaggedWith}
        <table cellspacing="0" cellpadding="4" border="0">
            <tr style="vertical-align:top; ">
                <td style="width: 100px; min-height: 100px; ">
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Mobilize?user={$usr.userid}"><img src="{$usr.picUrl}" alt="" /></a>
                </td>
                <td style="width: 200px;">
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Mobilize?user={$usr.userid}">{$usr.name}</a><br/>
					<span id="idPhotoCounts" user="{$usr.userid}"> <!-- Ajax calls will replace this span's inner html -->
	                    {$usr.numPhotos} Photo{if $usr.numPhotos != 1}s{/if}<br/>
	                    {$usr.numAlbums} Album{if $usr.numAlbums != 1}s{/if}<br/>
					</span>
                </td>
                <td style="width: 250px; text-align: right;">
                    {if ($usr.bIsAppUser)}
                        <a target="_parent" href="{$smarty.const.FACEBOOK_ACTIVITY_URL}{$usr.userid}">View {$usr.firstName}'s recent 3Guppies activity</a>
                    {else}
                        <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Invite?user={$usr.userid}">Invite {$usr.firstName} to 3Guppies!</a>
                    {/if}
                    <br />
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}FriendsOfFriends?user={$usr.userid}">Browse {$usr.hisHerTheir} Tagged Photo Friends</a><br/>
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}FriendsOfFriends?user={$usr.userid}" style="text-decoration: none; color: black;">
	                    {$usr.networks}
					</a>
                </td>
            </tr>
            <tr><td colspan="3"><hr /></td></tr>
        </table>
	{/foreach}
</div>
