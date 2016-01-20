{* using variables:
	$userid
	$smarty.const.FACEBOOK_CANVAS_URL
	$arrUsers[n].userid
	$arrUsers[n].name
	$arrUsers[n].bIsAppUser
	$arrUsers[n].picUrl
*}

{addheaderfile file="modules/Facebook/javascript/InviteView.js"}
{addheaderfile file="modules/Facebook/css/InviteView.css"}


<div class="FBbody">
    <form id="form_id" class="inviteFriends" method="get" target="_parent" onsubmit=" return ConfirmSubmit('form_id')" action="{$smarty.const.FACEBOOK_CANVAS_URL}Send">
        <h2 style="display: inline;">Invite friends selected below </h2> <span style="display: inline;"><em>(Limit 10 per day)</em></span>
        <br />
        <a href="#" onclick="SelectAll('form_id', 'inviteSelect', true);">Select 10</a>
        &nbsp;&nbsp;|&nbsp;&nbsp;
        <a href="#" onclick="SelectAll('form_id', 'inviteSelect', false);">Unselect All</a>
        <br />
        <br />
        <div id="selectFriends">
            {foreach item=usr from=$arrUsers}
                {if (!$usr.bIsAppUser)}
                    <div class="inviteUserThumb">
                        <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Send?invitation&user={$usr.userid}">
                        <img style="width: 50px;" src="{$usr.picUrl}" alt="" /></a>
                        <input type="checkbox" name="inviteSelect[]" value="{$usr.userid}" checked="checked" />
                        <div style="margin-top: 3px;">{$usr.name}</div>
                    </div>
                {/if}
            {/foreach}
        </div>
        <div style="clear: both;">&nbsp;</div>
        <input type="hidden" name="invitation" value="1" />
        
        <button type="submit" class="globalBtn green" style="width: 60px;">Invite</button>
        <br class="clear" />
    </form>
    
    
    
    <div id="guppyFriends">
        <h2>Friends who have added {$smarty.const.FACEBOOK_APPNAME}</h2>
        <br />
        <br />
        {foreach item=usr from=$arrUsers}
            {if ($usr.bIsAppUser)}
                <div class="guppyUserThumb">
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Mobilize?user={$usr.userid}">
                    <img style="width: 50px;" src="{$usr.picUrl}" alt="" /></a>
                    <div style="margin-top: 3px;">{$usr.name}</div>
                </div>
            {/if}
        {/foreach}
    </div>
    
    <div style="clear: both;"></div>
</div>
