{addheaderfile file=$URL_BASE|cat:"css/openlocker/locker.css"}
{addheaderfile file=$URL_BASE|cat:"css/ibox.css"}
{addheaderfile file=$URL_BASE|cat:"javascript/openlocker.js"}
{addheaderfile file=$URL_BASE|cat:"javascript/ibox.js"}
{addheaderfile file=$URL_BASE|cat:"javascript/swfobject.js"}
{addheaderfile file=$URL_BASE|cat:"javascript/yui/mixxer/omniture.js"}

<script type="text/javascript">
var media_descr = new AjaxObject();
media_descr.postURL = "{genurl module='Media' action='Description' media_descr_personalMediaId=$entry.Id NoCruft='1'}";
media_descr.action = "get";
media_descr.type = "div";
{literal}

function submitUpload( form, submitBtnName )
{
    document.getElementById( 'olUpload_error' ).style.display = 'none';
    var submitBtn = form.elements[ submitBtnName ];

    // don't allow user to try to upload nothing
    if( document.uploadForm.elements.userfile.value == '' )
    {
        document.getElementById( 'olUpload_error' ).style.display = 'block';
        return false;
    }

    form.submit();
    form.onsubmit = function() { return false; };

    submitBtn.className +=' disabled';
    document.getElementById('mobilizeSubmit').disabled = 'disabled';
    showHide('olUpload_wait');
    return true;
}

{/literal}

{* preload thumbnail images for mousePop to prevent the empty box *}

var thumbPreload = new Object();
var lockerItems = new Object();

{* standard mouseout action after hovering over the filename span of a row 
   also standard mouseover action for an expanded row, to collapse it *}
var mouseOutAction = function() {ldelim} mousePop('hide',this,'lockerTooltip'); {rdelim};
var collMouseOverAction = function() {ldelim} mousePop('here',this,'lockerTooltip',50,30,'close','tip'); {rdelim};

{foreach from=$LockerEntries item=entry}

{if $entry.ContentType eq 'picture' and $entry.Type eq "Unlicensed"}

thumbPreload[ '{$entry.Id}' ] = new Image();
thumbPreload[ '{$entry.Id}' ].src = '{genurl NoRingtones="true"}resize.php?IMAGE={$entry.Location}&WIDTH=128&HEIGHT=128';

{elseif $entry.ContentType eq 'picture' and $entry.Type eq "Licensed"}

thumbPreload[ '{$entry.Id}' ] = new Image();
thumbPreload[ '{$entry.Id}' ].src = '{genurl NoRingtones="true"}resize.php?IMAGE={$entry.Preview}&WIDTH=128&HEIGHT=128&GLOBAL=true';

{/if}

lockerItems[ '{$entry.Id}' ] = {ldelim}
    conType : '{$entry.ContentType}',         {* content type *}
    licType : '{$entry.Type}',                {* license type *}
    
    {if $entry.Type == 'Unlicensed'}
        {if $entry.ContentType == 'picture'}
            clickAction     : function() {ldelim} fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_personalMediaId=$entry.Id NoCruft='1'}','{genurl module='Media' action='Description' media_descr_personalMediaId=$entry.Id NoCruft='1'}','{genurl module='Media' action='Tag' media_tag_personalMediaId=$entry.Id NoCruft='1'}'); {rdelim},
            mouseOverAction : function() {ldelim} mousePop('here',this,'lockerTooltip',128,130,thumbPreload[ '{$entry.Id}' ],'imgtip'); {rdelim}
        {elseif $entry.ContentType == 'video'}
            clickAction     : function() {ldelim}fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_personalMediaId=$entry.Id NoCruft='1'}','{genurl module='Media' action='Description' media_descr_personalMediaId=$entry.Id NoCruft='1'}','{genurl module='Media' action='Tag' media_tag_personalMediaId=$entry.Id NoCruft='1'}'); {rdelim},
            mouseOverAction : function() {ldelim} mousePop('here',this,'lockerTooltip',128,130,'<img src=\'{genmedia origFilename=$entry.Preview type="gif128"}\'/>','imgtip') {rdelim}
        {elseif $entry.ContentType == 'audio'}
            clickAction     : function() {ldelim}fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_personalMediaId=$entry.Id NoCruft='1'}','{genurl module='Media' action='Description' media_descr_personalMediaId=$entry.Id NoCruft='1'}','{genurl module='Media' action='Tag' media_tag_personalMediaId=$entry.Id NoCruft='1'}'); {rdelim},
            mouseOverAction : function() {ldelim} mousePop('here',this,'lockerTooltip',50,30,'details','tip'); {rdelim}
        {elseif $entry.ContentType == 'other'}
            clickAction     : function() {ldelim}fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_personalMediaId=$entry.Id NoCruft='1'}','{genurl module='Media' action='Description' media_descr_personalMediaId=$entry.Id NoCruft='1'}','{genurl module='Media' action='Tag' media_tag_personalMediaId=$entry.Id NoCruft='1'}'); {rdelim},
            mouseOverAction : function() {ldelim} mousePop('here',this,'lockerTooltip',50,30,'details','tip'); {rdelim}
        {else}
            clickAction     : function() {ldelim} fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_lockerMediaId=$entry.Id NoCruft='1'}'); {rdelim},
            mouseOverAction : function() {ldelim} mousePop('here',this,'lockerTooltip',50,30,'details','tip'); {rdelim}
        {/if}

    {elseif $entry.Type == 'Licensed'}
        {if $entry.ContentType == 'picture'}
            clickAction     : function() {ldelim} fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_lockerMediaId=$entry.Id NoCruft='1'}'); {rdelim},
            mouseOverAction : function() {ldelim} mousePop('here',this,'lockerTooltip',128,130,thumbPreload[ '{$entry.Id}' ],'imgtip'); {rdelim}
        {elseif $entry.ContentType == 'video'}
            clickAction     : function() {ldelim} fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_lockerMediaId=$entry.Id NoCruft='1'}'); {rdelim},
            mouseOverAction : function() {ldelim} mousePop('here',this,'lockerTooltip',50,30,'details','tip'); {rdelim}
        {elseif $entry.ContentType == 'audio'}
            clickAction     : function() {ldelim} fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_lockerMediaId=$entry.Id NoCruft='1'}'); {rdelim},
            mouseOverAction : function() {ldelim} mousePop('here',this,'lockerTooltip',50,30,'details','tip'); {rdelim}
        {elseif $entry.ContentType == 'other'}
            clickAction     : function() {ldelim} fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_lockerMediaId=$entry.Id NoCruft='1'}'); {rdelim},
            mouseOverAction : function() {ldelim} mousePop('here',this,'lockerTooltip',50,30,'details','tip'); {rdelim}
        {else}
            clickAction     : function() {ldelim} fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_lockerMediaId=$entry.Id NoCruft='1'}'); {rdelim},
            mouseOverAction : function() {ldelim} mousePop('here',this,'lockerTooltip',50,30,'details','tip'); {rdelim}

        {/if}
        
    {elseif $entry.Type == 'Premium'}
        {if $entry.ContentType == 'picture'}
            clickAction     : function() {ldelim} fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_lockerMediaId=$entry.Id NoCruft='1'}'); {rdelim},
            mouseOverAction : function() {ldelim} mousePop('here',this,'lockerTooltip',128,130,thumbPreload[ '{$entry.Id}' ],'imgtip'); {rdelim}
        {elseif $entry.ContentType == 'video'}
            clickAction     : function() {ldelim} fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_lockerMediaId=$entry.Id NoCruft='1'}'); {rdelim},
            mouseOverAction : function() {ldelim} mousePop('here',this,'lockerTooltip',50,30,'details','tip'); {rdelim}
        {elseif $entry.ContentType == 'audio'}
            clickAction     : function() {ldelim} fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_lockerMediaId=$entry.Id NoCruft='1'}'); {rdelim},
            mouseOverAction : function() {ldelim} mousePop('here',this,'lockerTooltip',50,30,'details','tip'); {rdelim}
        {elseif $entry.ContentType == 'other'}
            clickAction     : function() {ldelim} fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_lockerMediaId=$entry.Id NoCruft='1'}'); {rdelim},
            mouseOverAction : function() {ldelim} mousePop('here',this,'lockerTooltip',50,30,'details','tip'); {rdelim}
        {else}
            clickAction     : function() {ldelim} fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_lockerMediaId=$entry.Id NoCruft='1'}'); {rdelim},
            mouseOverAction : function() {ldelim} mousePop('here',this,'lockerTooltip',50,30,'details','tip'); {rdelim}

        {/if}
    {else}
        clickAction     : function() {ldelim} fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_lockerMediaId=$entry.Id NoCruft='1'}'); {rdelim},
        mouseOverAction : function() {ldelim} mousePop('here',this,'lockerTooltip',50,30,'details','tip'); {rdelim}

    {/if}
{rdelim};

{/foreach}
var media_setAvatar = new AjaxObject();
media_setAvatar.type="refresh";
</script>


<div class="moduleWide" id="openLocker">
	<span id="mediaHeader"></span><div id="aj_mediaHeader">{$media_heade_contents}</div>
    <div class="moduleBody white" style="padding: 10px 0;">
		<div style="padding: 0 10px;">
			<div align="center"><a href="{genurl module="OpenLocker" action="Mobilize"}"><strong>Upload files...</strong></a></div>
			<h3 style="padding: 0;">{$LockerTitle}<!--My Locker--></h4>
            {$LockerDescription}
			<!-- Your Locker is the simplified view of all the files in your Mixx.  Changes you make to files here will also be reflected in your gallery and library views.-->
		</div>
		<br /><br />
	{if $ContentType ne 'Help'}
<!--
        <div class="olInfo">
            <div class="olTotalFiles" style="padding-left: 5px;"><span id="lockerCount">Total Files: <strong>{if $LockerCount eq 0}0{else}{$LockerCount}{/if}</strong></span> 
            <span id="spaceRemaining">Space Remaining: <strong>{if $LockerCount eq 0}100 MB{else}{$SpaceRemaining}{/if}</strong></span></div>
        </div>
-->
		<!-- ------------------------ TABLE COLUMN HEADERS ------------------------ -->
        <table cellpadding="0" cellspacing="0" border="0" id="openLockerFiles">
            <tr class="openLockerFilesTitle">
                <td colspan="2" width="355" class="openLockerFilesTitleName">
                    {if $SortedBy ne 'Name.Desc'}<a href="{genurl module="OpenLocker" action="IndexContainer" ContentType=$ContentType SortBy="Name.Desc"}">{/if}<span style="padding-left: 6px;">Name</span>{if $SortedBy ne'Name.Desc'}</a>{/if}
                </td>
                <td width="60" class="openLockerFilesTitleAdded">
                    {if $SortedBy ne 'DateAdded.Desc'}<a href="{genurl module="OpenLocker" action="IndexContainer" ContentType=$ContentType SortBy="DateAdded.Desc"}">{/if}Added{if $SortedBy ne'DateAdded.Desc'}</a>{/if}
                </td>
                <td width="60" class="openLockerFilesTitleType">
                    {if $SortedBy ne 'Type.Desc'}<a href="{genurl module="OpenLocker" action="IndexContainer" ContentType=$ContentType SortBy="Type.Desc"}">{/if}Type{if $SortedBy ne'Type.Desc'}</a>{/if}
                </td>
                <td width="60" class="openLockerFilesTitleSharing">
                    {if $SortedBy ne 'Sharing.Desc'}<a href="{genurl module="OpenLocker" action="IndexContainer" ContentType=$ContentType SortBy="Sharing.Desc"}">{/if}Privacy{if $SortedBy ne'Sharing.Desc'}</a>{/if}
                </td>
                <td width="60" class="openLockerFilesTitleSize">
                    {if $SortedBy ne 'Size.Desc'}<a href="{genurl module="OpenLocker" action="IndexContainer" ContentType=$ContentType SortBy="Size.Desc"}">{/if}Size{if $SortedBy ne'Size.Desc'}</a>{/if}
                </td>
            </tr>
			<!-- ------------------------ OnEnter MESSAGE HOLDER  ------------------------ -->
			<tr id="onEnterMsg">
				<td colspan="5"><div id="onEnterMsg_text"></td>
			</tr>
			
			<!-- ------------------------ BEGIN FOREACH FOR FILES ------------------------ -->
            {foreach name=entry_id from=$LockerEntries item=entry}
            <tr class="openLockerFileDetails" id="ol{$entry.Id}">
                <td class="openLockerFileDetailName" width="150"><!-- {$entry.Id} - -->
					<!-- ------------------------ File Name or Title ------------------------ -->

                    <div id="olFileName{$entry.Id}" class="olFileName" style="padding-bottom:6px;">
						{if $entry.ContentType eq 'picture' and $entry.Type eq "Unlicensed"}
							<span class="expandControls" id="expandControls{$entry.Id}"><img src="{$IMAGES_DIR}ui/icons/icon_expand.gif" border="0" style="margin-bottom:-3px;"/><span class="fileName mediaId_{$entry.Id}">{$entry.Title|truncate:22:'...':true}</span></span>
						{elseif $entry.ContentType eq 'picture' and $entry.Type eq "Licensed"}
							<span class="expandControls" id="expandControls{$entry.Id}"><img src="{$IMAGES_DIR}ui/icons/icon_expand.gif" border="0" style="margin-bottom:-3px;"/><span class="fileName mediaId_{$entry.Id}">{$entry.Title|truncate:22:'...':true}</span></span>
						{elseif $entry.ContentType eq 'video' and $entry.Type eq "Unlicensed"}
							<!--<span class="expandControls" id="expandControls{$entry.Id}" onclick="setOpenLockerRow('{$entry.Id}');fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_personalMediaId=$entry.Id NoCruft='1'}','{genurl module='Media' action='Description' media_descr_personalMediaId=$entry.Id NoCruft='1'}','{genurl module='Media' action='Tag' media_tag_personalMediaId=$entry.Id NoCruft='1'}'); "><img src="{$IMAGES_DIR}ui/icons/icon_expand.gif" border="0" style="margin-bottom:-3px;"/><span class="fileName mediaId_{$entry.Id}">{$entry.Title|truncate:22:'...':true}</span></span>-->
							<span class="expandControls" id="expandControls{$entry.Id}"><img src="{$IMAGES_DIR}ui/icons/icon_expand.gif" border="0" style="margin-bottom:-3px;"/><span class="fileName mediaId_{$entry.Id}">{$entry.Title|truncate:22:'...':true}</span></span>
						{elseif $entry.ContentType neq 'picture' or $entry.ContentType neq 'video' and $entry.Type eq "Unlicensed"}
							<span class="expandControls" id="expandControls{$entry.Id}"><img src="{$IMAGES_DIR}ui/icons/icon_expand.gif" border="0" style="margin-bottom:-3px;" /><span class="fileName mediaId_{$entry.Id}">{$entry.Title|truncate:22:'...':true}</span></span>
						{elseif $entry.ContentType neq 'picture' or $entry.ContentType neq 'video' and $entry.Type eq "Licensed"}
							<span class="expandControls" id="expandControls{$entry.Id}"><img src="{$IMAGES_DIR}ui/icons/icon_expand.gif" border="0" style="margin-bottom:-3px;" /><span class="fileName mediaId_{$entry.Id}">{$entry.Title|truncate:22:'...':true}</span></span>
						{elseif $entry.ContentType neq 'picture' or $entry.ContentType neq 'video' and $entry.Type eq "Premium"}
							<span class="expandControls" id="expandControls{$entry.Id}"><img src="{$IMAGES_DIR}ui/icons/icon_expand.gif" border="0" style="margin-bottom:-3px;" /><span class="fileName mediaId_{$entry.Id}">{$entry.Title|truncate:22:'...':true}</span></span>
						{/if}
							<span class="expandControls" id="collapseControls{$entry.Id}" style="display: none;"><img src="{$IMAGES_DIR}ui/icons/icon_collapse.gif" border="0" style="margin-bottom:-3px;" /><span class="fileName mediaId_{$entry.Id}">{$entry.Title|truncate:22:'...':true}</span></span>
					</div>
                    
				</td>
				<td width="210">
                    <div id="olControls{$entry.Id}" class="olControls">
					<!-- ------------------------ Unlicensed File Actions | listen | view | send to friend | mobilize | avatar | delete | ------------------------ -->
					{if $entry.Type eq "Unlicensed"}
						{if $entry.ContentType eq 'audio'}
							<img onclick="openLockerRow('{$entry.Id}', DETAILS_PANEL);fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_personalMediaId=$entry.Id NoCruft='1'}');" src="{$IMAGES_DIR}ui/icons/icon-play.gif" 
                            {if $entry.PreviewUrl}
                                onmouseover="this.src='{$IMAGES_DIR}ui/icons/icon-playcolor.gif';{$smarty.config.mousePop_listen}" onmouseout="this.src='{$IMAGES_DIR}ui/icons/icon-play.gif';{$smarty.config.mousePop_out}" 
                            {else}
                                onmouseover="{$smarty.config.mousePop_cannotListen}" onmouseout="{$smarty.config.mousePop_out}" 
                            {/if}
                            border="0"/>
						{elseif $entry.ContentType eq 'picture'}
							<a href="{genurl NoRingtones='true' download='personal'}/{$entry.Location}/{$entry.Location}" rel="ibox"><img onmouseover="this.src='{$IMAGES_DIR}ui/icons/icon_enlarge_color.gif';{$smarty.config.mousePop_picZoom}" onmouseout="this.src='{$IMAGES_DIR}ui/icons/icon_enlarge_mono.gif';{$smarty.config.mousePop_out}" src="{$IMAGES_DIR}ui/icons/icon_enlarge_mono.gif" onclick="openLockerRow('{$entry.Id}', DETAILS_PANEL);fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_personalMediaId=$entry.Id NoCruft='1'}','{genurl module='Media' action='Description' media_descr_personalMediaId=$entry.Id NoCruft='1'}','{genurl module='Media' action='Tag' media_tag_personalMediaId=$entry.Id NoCruft='1'}');" border="0" /></a>
						{else}
							<img src="{$IMAGES_DIR}ui/icons/icon_enlarge_disabled.gif" border="0" style="cursor: default;" />
						{/if}
						{if $entry.ContentType eq 'picture' or $entry.ContentType eq 'video'}
						<img onclick="openLockerRow('{$entry.Id}', FRIEND_PANEL);" onmouseover="this.src='{$IMAGES_DIR}ui/icons/icon_sendtofriend_color.gif';{$smarty.config.mousePop_sendToFriend}" onmouseout="this.src='{$IMAGES_DIR}ui/icons/icon_sendtofriend_mono.gif';{$smarty.config.mousePop_out}" src="{$IMAGES_DIR}ui/icons/icon_sendtofriend_mono.gif" border="0" />
						{else}
						<img src="{$IMAGES_DIR}ui/icons/icon_sendtofriend_disabled.gif" border="0" style="cursor: default;" onmouseover="{$smarty.config.mousePop_cannotSend}" onmouseout="{$smarty.config.mousePop_out}"/>
						{/if}
						<img onclick="openLockerRow('{$entry.Id}', MOBILIZE_PANEL);createAO('mobilize_{$entry.Id}','send!yes','type!div','id!olPanelMobilize{$entry.Id}_response','succAction!feather_fireMobilizeEvent', 'postURL!{genurl module="OpenLocker" action="Share" PersonalId=$entry.Id NoCruft='1'}?page='+encodeURI(window.location));" onmouseover="this.src='{$IMAGES_DIR}ui/icons/icon_mobilize_color.gif';{$smarty.config.mousePop_mobilize}" onmouseout="this.src='{$IMAGES_DIR}ui/icons/icon_mobilize_mono.gif';{$smarty.config.mousePop_out}" src="{$IMAGES_DIR}ui/icons/icon_mobilize_mono.gif" border="0" />
							{if $entry.IsMyAvatar eq 1}
							<img id="avatar_is_{$entry.Id}" src="{$IMAGES_DIR}ui/icons/icon_avatar_color.gif" />
							{elseif $entry.ContentType eq 'picture'}
							<img id="avatar_make_{$entry.Id}" class="icon" src="{$IMAGES_DIR}ui/icons/icon_avatar_mono.gif" onmouseover="this.src='{$IMAGES_DIR}ui/icons/icon_avatar_color.gif'; mousePop('show',this,'lockerTooltip',100,30,'make my avatar','tip');" onmouseout="this.src='{$IMAGES_DIR}ui/icons/icon_avatar_mono.gif'; mousePop('hide',this,'lockerTooltip');" onclick="media_setAvatar.postURL='{genurl module=Media action=Avatar media_avata_personalMediaId=$entry.Id}/NoCruft/1/'; media_setAvatar.sndReq();" />
							{/if}
						<img onclick="openLockerRow('{$entry.Id}', DELETE_PANEL);" onmouseover="this.src='{$IMAGES_DIR}ui/icons/icon_delete_color.gif';{$smarty.config.mousePop_delete}" onmouseout="this.src='{$IMAGES_DIR}ui/icons/icon_delete_mono.gif';{$smarty.config.mousePop_out}" src="{$IMAGES_DIR}ui/icons/icon_delete_mono.gif" border="0" />
					<!-- ------------------------ Licensed File Actions | listen | view | send to friend | mobilize | delete | ------------------------ -->
                    {elseif $entry.Type eq "Licensed"}
						{if $entry.ContentType eq 'audio'}
							<img onclick="openLockerRow('{$entry.Id}', DETAILS_PANEL);fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_lockerMediaId=$entry.Id NoCruft='1'}');PopUp('{$entry.PreviewUrl}','midiplayer','toolbar=no,location=no,status=no,menubar=no,scrollbars=no,resizable=no,copyhistory=no,width=330,height=252');" onmouseover="this.src='{$IMAGES_DIR}ui/icons/icon-playcolor.gif';{$smarty.config.mousePop_listen}" onmouseout="this.src='{$IMAGES_DIR}ui/icons/icon-play.gif';{$smarty.config.mousePop_out}" src="{$IMAGES_DIR}ui/icons/icon-play.gif" border="0"/>
						{elseif $entry.ContentType eq 'picture'}
							<a href="{genurl NoRingtones='true' download='global'}/{$entry.Preview}/{$entry.Preview}" rel="ibox"><img onmouseover="this.src='{$IMAGES_DIR}ui/icons/icon_enlarge_color.gif';{$smarty.config.mousePop_picZoom}" onmouseout="this.src='{$IMAGES_DIR}ui/icons/icon_enlarge_mono.gif';{$smarty.config.mousePop_out}" src="{$IMAGES_DIR}ui/icons/icon_enlarge_mono.gif" onclick="openLockerRow('{$entry.Id}', DETAILS_PANEL);fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_lockerMediaId=$entry.Id NoCruft='1'}','{genurl module='Media' action='Description' media_descr_lockerMediaId=$entry.Id NoCruft='1'}','{genurl module='Media' action='Tag' media_tag_lockerMediaId=$entry.Id NoCruft='1'}');" border="0" /></a>
						{else}
							<img src="{$IMAGES_DIR}ui/icons/icon_enlarge_disabled.gif" border="0" style="cursor: default;" />
						{/if}
						<img src="{$IMAGES_DIR}ui/icons/icon_sendtofriend_disabled.gif" border="0" style="cursor: default;" onmouseover="{$smarty.config.mousePop_cannotSend}" onmouseout="{$smarty.config.mousePop_out}"/>
						<img onclick="openLockerRow('{$entry.Id}', MOBILIZE_PANEL);createAO('mobilize_{$entry.Id}','send!yes','type!div','id!olPanelMobilize{$entry.Id}_response','postURL!{genurl module="OpenLocker" action="Share" MediaId=$entry.Id NoCruft='1'}');" onmouseover="this.src='{$IMAGES_DIR}ui/icons/icon_mobilize_color.gif';{$smarty.config.mousePop_mobilize}" onmouseout="this.src='{$IMAGES_DIR}ui/icons/icon_mobilize_mono.gif';{$smarty.config.mousePop_out}" src="{$IMAGES_DIR}ui/icons/icon_mobilize_mono.gif" border="0" />
						<img onclick="openLockerRow('{$entry.Id}', DELETE_PANEL);" onmouseover="this.src='{$IMAGES_DIR}ui/icons/icon_delete_color.gif';{$smarty.config.mousePop_delete}" onmouseout="this.src='{$IMAGES_DIR}ui/icons/icon_delete_mono.gif';{$smarty.config.mousePop_out}" src="{$IMAGES_DIR}ui/icons/icon_delete_mono.gif" border="0" />
					<!-- ------------------------ Premium File Actions | listen | view | send to friend | mobilize | delete | ------------------------ -->
                    {else}
						{if $entry.ContentType eq 'audio'}
							<img onclick="openLockerRow('{$entry.Id}', DETAILS_PANEL);fetchMediaData({$entry.Id},'{genurl module='Media' action='Title' media_title_lockerMediaId=$entry.Id NoCruft='1'}');PopUp('{$entry.PreviewUrl}','midiplayer','toolbar=no,location=no,status=no,menubar=no,scrollbars=no,resizable=no,copyhistory=no,width=330,height=252');" onmouseover="this.src='{$IMAGES_DIR}ui/icons/icon-playcolor.gif';{$smarty.config.mousePop_listen}" onmouseout="this.src='{$IMAGES_DIR}ui/icons/icon-play.gif';{$smarty.config.mousePop_out}" src="{$IMAGES_DIR}ui/icons/icon-play.gif" border="0" />
						{elseif $entry.ContentType eq 'picture'}
							<a href="{genurl NoRingtones='true' download='global'}/{$entry.Preview}/{$entry.Preview}" rel="ibox"><img onmouseover="this.src='{$IMAGES_DIR}ui/icons/icon_enlarge_color.gif';{$smarty.config.mousePop_picZoom}" onmouseout="this.src='{$IMAGES_DIR}ui/icons/icon_enlarge_mono.gif';{$smarty.config.mousePop_out}" src="{$IMAGES_DIR}ui/icons/icon_enlarge_mono.gif" onclick="openLockerRow('{$entry.Id}', DETAILS_PANEL);" border="0" /></a>
						{/if}
						<img src="{$IMAGES_DIR}ui/icons/icon_sendtofriend_disabled.gif" border="0" style="cursor: default;" onmouseover="{$smarty.config.mousePop_cannotSend}" onmouseout="{$smarty.config.mousePop_out}"/>
						<img onclick="openLockerRow('{$entry.Id}', MOBILIZE_PANEL);createAO('mobilize_{$entry.Id}','send!yes','type!div','id!olPanelMobilize{$entry.Id}_response','postURL!{genurl module="OpenLocker" action="Share" MediaId=$entry.Id NoCruft='1'}');" onmouseover="this.src='{$IMAGES_DIR}ui/icons/icon_mobilize_color.gif';{$smarty.config.mousePop_mobilize}" onmouseout="this.src='{$IMAGES_DIR}ui/icons/icon_mobilize_mono.gif';{$smarty.config.mousePop_out}" src="{$IMAGES_DIR}ui/icons/icon_mobilize_mono.gif" border="0" />
						<img src="{$IMAGES_DIR}ui/icons/icon_delete_disabled.gif" border="0" style="cursor: default;" onmouseover="{$smarty.config.mousePop_cannotDelete}" onmouseout="{$smarty.config.mousePop_out}"/>
					{/if}
                    </div>
					<div class="clear"></div>
                </td>
				<!-- ------------------------ Date Added | File Type | Privacy | File Size ------------------------ -->
                <td class="openLockerFileDetailAdded">
                    {$entry.DateAdded}
                </td>
                <td  class="openLockerFileDetailType">
                    {$entry.ContentType}
                </td>
                <td class="openLockerFileDetailSharing textCenter">
                    {if $entry.ContentType eq 'picture' && $entry.Type eq 'Unlicensed'}
                        <img style="margin:0 auto;{if $entry.Sharing neq "Private"}display:none"{/if}" id="setPublic_{$entry.Id}" onclick="setPermission({$entry.Id},'{genurl module="OpenLocker" action="Permission" PersonalId=$entry.Id}/Sharing/Public');" onmouseover="this.src='{$IMAGES_DIR}ui/icons/icon_private_color.gif';{$smarty.config.mousePop_private}" onmouseout="this.src='{$IMAGES_DIR}ui/icons/icon_private_mono.gif';{$smarty.config.mousePop_out}" src="{$IMAGES_DIR}ui/icons/icon_private_mono.gif" border="0"/>
                        <img style="margin:0 auto;{if $entry.Sharing neq "Public"}display:none"{/if}" id="setPrivate_{$entry.Id}" onclick="setPermission({$entry.Id},'{genurl module="OpenLocker" action="Permission" PersonalId=$entry.Id}/Sharing/Private');" onmouseover="this.src='{$IMAGES_DIR}ui/icons/icon_public_color.gif';{$smarty.config.mousePop_public}" onmouseout="this.src='{$IMAGES_DIR}ui/icons/icon_public_mono.gif';{$smarty.config.mousePop_out}" src="{$IMAGES_DIR}ui/icons/icon_public_mono.gif" border="0"/>
                    {else}
                        <img style="margin:0; cursor: default" src="{$IMAGES_DIR}ui/icons/icon_private_disabled.gif" onmouseover="{$smarty.config.mousePop_noPub}" onmouseout="{$smarty.config.mousePop_out}" border="0"/>
                    {/if}
				</td>
                <td class="openLockerFileDetailSize">
                    {$entry.Size}
                </td>
            </tr>
			<!-- ------------------------ CONTENT DROP-DOWN | Thumbnail | Send to Friend Module | Title | Description | Tags ------------------------ -->
            <tr class="olPanel" id="olPanel{$entry.Id}" style="display: none;padding:10px; border-bottom:2px solid white;width:550px;">
                <td valign="top" colspan="1" style="padding:10px 0 10px 10px;">
				 {if $entry.ContentType eq 'video'}
				 <div id="playerDiv{$entry.Id}" style="width:150px">
				   <div style="padding: 20px; font-size:14px; font-weight: bold;">
				    Hello, you either have JavaScript turned off or an old version of Macromedia's Flash Player. <a href="http://www.macromedia.com/go/getflashplayer/">Click here</a> to get the latest flash player.
				   </div>
				  </div>
					<script type="text/javascript">
				   // <![CDATA[
				   var movie = "{genmedia origFilename=$entry.Preview type="flvplay"}";
				   var fo = new SWFObject(movie, "movie_player", "160", "120", 7, "#FFFFFF");
				   fo.write("playerDiv{$entry.Id}");
				   // ]]>
					</script>
                  {elseif $entry.ContentType eq 'audio'}
                    {if $entry.Type eq "Unlicensed"}
                        {if $entry.PreviewUrl}
                            <div style='text-align: left'>
                                <div style='font-weight: bold'>Preview</div>
                                <div id="mp3Player{$entry.Id}">
                                   <embed src="{genurl NoRingtones=''}mp3player.swf" flashvars="file={$entry.PreviewUrl}" type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer" height="20" width="170" />
                                </div>
                            </div>
                            {* not sure why, but in IE it was only playing the last song for every entry
                            <script type="text/javascript">
                                var mp3PlayerUrl = "{genurl NoRingtones=''}mp3player.swf";
                                var fo = new SWFObject(mp3PlayerUrl, "mp3Flash{$entry.Id}", "170", "20", 7, "#FFFFFF");
                                fo.addVariable( 'file', '{$entry.PreviewUrl}' );
                                fo.write("mp3Player{$entry.Id}");
                            </script> *}
                        {else}
                            Preview unavailable.
                        {/if}
                    {else}
                    <div style='text-align: center;'>
                        <img src="{genurl NoRingtones='true'}images/ui/icons/icon_filetype_audio.gif" border="0" onclick="PopUp('{$entry.PreviewUrl}','midiplayer','toolbar=no,location=no,status=no,menubar=no,scrollbars=no,resizable=no,copyhistory=no,width=330,height=252');" style="cursor:pointer;" />
                    </div>
                    {/if}
				  {else}
					<!-- ------------------------ File Thumbnail or Generic Icon ------------------------ -->
						<!-- ------------------------ unlicensed file ------------------------ -->
						{if $entry.Type eq "Unlicensed"}
							{if $entry.ContentType eq 'picture'}
								<img style="border: 1px solid #000;" src="{genurl NoRingtones=''}resize.php?IMAGE={$entry.Location}&WIDTH=128&HEIGHT=128" border="0"/>									
							{elseif $entry.ContentType eq 'audio'}
								<img src="{genurl NoRingtones='true'}images/ui/icons/icon_filetype_audio.gif" border="0" onclick="PopUp('{$entry.PreviewUrl}','midiplayer','toolbar=no,location=no,status=no,menubar=no,scrollbars=no,resizable=no,copyhistory=no,width=330,height=252');" style="cursor:pointer"/>
							{elseif $entry.ContentType eq 'other'}
								<img src="{genurl NoRingtones='true'}images/ui/icons/icon_filetype_other.gif" border="0" />
							{/if}
						<!-- ------------------------ licensed file or other ------------------------ -->
						{else}
							{if $entry.ContentType eq 'picture'}
								<img style="border: 1px solid #000;" src="{genurl NoRingtones=''}resize.php?IMAGE={$entry.Preview}&WIDTH=128&HEIGHT=128&GLOBAL=true" border="0"/>									
							{elseif $entry.ContentType eq 'audio'}
								<img src="{genurl NoRingtones='true'}images/ui/icons/icon_filetype_audio.gif" border="0" onclick="PopUp('{$entry.PreviewUrl}','midiplayer','toolbar=no,location=no,status=no,menubar=no,scrollbars=no,resizable=no,copyhistory=no,width=330,height=252');" style="cursor:pointer" />
							{elseif $entry.ContentType eq 'video'}
								<img src="{genurl NoRingtones='true'}images/ui/icons/icon_filetype_video.gif" border="0" />
							{elseif $entry.ContentType eq 'other'}
								<img src="{genurl NoRingtones='true'}images/ui/icons/icon_filetype_other.gif" border="0" />
							{/if}
						{/if}
					{/if}
				</td>
				<td valign="top" colspan="5" style="padding:10px 10px 10px 5px;">
					<!-- ------------------------ File Information | Title | Description | Tags ------------------------ -->
                    <div id="olPanelDetails{$entry.Id}" class="olPanelDetails" style="display: none;">
                        <div style="float: left; width: 370px;">
                            <div style="font-weight: bold;">Title</div>
                            <div style=""><span id="mediaTitle_{$entry.Id}"></span></div>
                            {if $entry.ContentType eq 'picture' or $entry.ContentType eq 'video' and $entry.Type eq 'Unlicensed'}
							<div style="font-weight: bold;">Tags</div>
                            <div style=""><span id="mediaTag_{$entry.Id}"></span></div>
							<div style="font-weight: bold;">Description</div>
                            <div style=""><span id="mediaDescr_{$entry.Id}"></span></div>
							{/if}
                        </div>
					<!--<div style="width:560px; text-align:right;"><button class="globalBtn burg" onclick="olShowHide('{$entry.Id}', 'cancel');">Close</button></div>-->
					<button class="globalBtn burg" onclick="closeSelfWrapper()">Close</button>
					</div>
					<!-- ------------------------ Send To Friend Module ------------------------ -->
					<div id="olPanelSendtofriend{$entry.Id}" style="display: none;">
						<div class="popupForm_row"><strong>Send To:</strong> (phone numbers separated by commas; you can also enter email addresses)</div>
						<div class="popupForm_row"><input id="olPanelSendtofriend{$entry.Id}_recip" type="text" style="width:200px" /></div>
						<div class="popupForm_row"><strong>From:</strong> (your name as seen by your friends)</div>
						<div class="popupForm_row">
						<input id="olPanelSendtofriend{$entry.Id}_msg" type="text" style="width:200px;margin-right:5px" onkeypress="charCount(20,'olPanelSendtofriend{$entry.Id}_msg','fromMessage_{$entry.Id}');" onkeyup="charCount(20,'olPanelSendtofriend{$entry.Id}_msg','fromMessage_{$entry.Id}');" onblur="hideCharCount('fromMessage_{$entry.Id}');"/>
							{if $entry.Type eq 'Premium'}
								<button class="globalBtn burg" onclick="javascript:Message=document.getElementById('olPanelSendtofriend{$entry.Id}_msg');Recipients=document.getElementById('olPanelSendtofriend{$entry.Id}_recip');createAO('Sendtofriend_{$entry.Id}','send!yes','type!div','id!olPanelSendtofriend{$entry.Id}_response','succAction!feather_fireMobilizeEvent','postURL!{genurl module="OpenLocker" action="Share" MediaId=$entry.Id NoCruft="1"}/Subject/'+escape(Message.value)+'/Recipients/'+escape(Recipients.value)+'?page='+encodeURI(window.location));">Send</button>&nbsp;
							{else}
								<button class="globalBtn burg" onclick="javascript:Message=document.getElementById('olPanelSendtofriend{$entry.Id}_msg');Recipients=document.getElementById('olPanelSendtofriend{$entry.Id}_recip');createAO('Sendtofriend_{$entry.Id}','send!yes','type!div','id!olPanelSendtofriend{$entry.Id}_response','succAction!feather_fireMobilizeEvent','postURL!{genurl module="OpenLocker" action="Share" PersonalId=$entry.Id NoCruft="1"}/Subject/'+escape(Message.value)+'/Recipients/'+escape(Recipients.value)+'?page='+encodeURI(window.location));">Send</button>&nbsp;
							{/if}
							<button class="globalBtn burg" onclick="closeSelfWrapper()">Close</button>
						</div>
						<div id="fromMessage_{$entry.Id}" class="messageDiv">&nbsp;</div>
						<span id="olPanelSendtofriend{$entry.Id}_response"></span>
						<!-- Send to Friend "Sending..." -->
						<div id="olPanelSendtofriend{$entry.Id}_response_wait" style="display:none">
							<div class="waitBox">
								<img src="{$IMAGES_DIR}ui/groups/loading.gif"/> Sending...
							</div>
						</div>
					</div>
					<!-- ------------------------ Mobilize Module ------------------------ -->
					<div id="olPanelMobilize{$entry.Id}" style="text-align:left; display:none;">
						<div style="text-align:center; width:190px;">
							<span id="olPanelMobilize{$entry.Id}_response"></span>
							<div id="olPanelMobilize{$entry.Id}_response_wait" style="display:none;">
								<div class="waitBox" style="width: 200px;">
									<img src="{$IMAGES_DIR}ui/groups/loading.gif"/> Sending...
								</div>
								<br /><br />
							</div>
							<button class="globalBtn burg" onclick="closeSelfWrapper()">Close</button>
						</div>
					</div>						
					<!-- ------------------------ Delete Module ------------------------ -->
					<div id="olPanelDelete{$entry.Id}" style="text-align:left; display:none;">
						<div style="text-align:center; width:190px;">
							{if $entry.Type eq 'Premium'}
								Premium content cannot be deleted.
							{else}
								<div class="popupForm_row">Delete this item from your Locker?</div>
								{if $entry.Type eq 'Unlicensed'}
									<button class="globalBtn burg" onclick="if (document.getElementById('olDelete_wait{$entry.Id}')) document.getElementById('olDelete_wait{$entry.Id}').style.display='block';createAO('delete_{$entry.Id}','send!yes','type!refresh','action!get','id!olPanelDelete{$entry.Id}','postURL!{genurl module="OpenLocker" action="Delete" PersonalId=$entry.Id NoCruft='1'}');">Delete</button>&nbsp;<button class="globalBtn burg" onclick="closeSelfWrapper()">Cancel</button>						
								{elseif $entry.Type eq 'Licensed'}
									<button class="globalBtn burg" onclick="if (document.getElementById('olDelete_wait{$entry.Id}')) document.getElementById('olDelete_wait{$entry.Id}').style.display='block';createAO('delete_{$entry.Id}','send!yes','type!refresh','action!get','id!olPanelDelete{$entry.Id}','postURL!{genurl module="OpenLocker" action="Delete" MediaId=$entry.Id NoCruft='1'}');">Delete</button>&nbsp;<button class="globalBtn burg" onclick="closeSelfWrapper()">Cancel</button>
								{/if}
							{/if}
							<div id="olDelete_wait{$entry.Id}" style="display:none">
								<div class="waitBox"><img src="{$IMAGES_DIR}ui/groups/loading.gif"/> Deleting Your File...</div>
							</div>
						</div>
					</div>
                </td>
            </tr>
            {foreachelse}
            <tr class="openLockerFileError"><td colspan="6">Your Locker is currently empty.<br />Click "Upload Files..." to add content.<br /><br /><td></tr>
            {/foreach}
			{if isset($Pager)}
				<tr><td colspan="5">{$Pager}</td></tr>
			{/if}
        </table>
<!-- ------------------------ IF HELP ------------------------ -->
	{else}
		<div class="olHelpText">{$HelpText}</div>
	{/if}
	</div>
</div>

{if $MobilizeContentData.mediaId}
<script type="text/javascript">
panel = DETAILS_PANEL;
{if $MobilizeContentData.invitation_code}
	panel = FRIEND_PANEL;
	document.getElementById('onEnterMsg_text').innerHTML="Share with another friend!";
	document.getElementById('onEnterMsg').style.height='30px';
{/if}
/** 
 * checks to see if user has just uploaded a new locker item, OR, came in via the code-based share mechanism. 
 * If so, expand the uploaded/just-added item. 
 * Smarty var "$MobilizeContentData.mediaId" should always be present and indicate if something was just mobilized/shared
 */
/*
<script type="text/javascript">
var query = window.location.href;
if (query.indexOf('Upload') != -1) {ldelim}
	document.getElementById('olUpload').style.display='';
	document.getElementById('olUploadControl').style.display='none';
{rdelim}
*/
/*
///////// OLD COMMENT /////// 
 * checks to see if user has just uploaded a new locker item. If so then
 * expand that item. 
 * ASSUMPTION: newly uploaded item is first row in the table
 * w/ id 'openLockerFiles'
 */
	/** locker code share version **/
	YUIEvent.addListener( window, 'load', function() {ldelim}
    // nothing to see here
    if( !location.href || 
        !document.getElementById( 'openLockerFiles' ) )
        return;
				// open the locker row with the sendtofriend panel activated
                if( panel == DETAILS_PANEL )
                {ldelim}
                    fetchMediaData({$MobilizeContentData.mediaId} ,'{genurl module='Media' action='Title'}/media_title_personalMediaId/'+{$MobilizeContentData.mediaId}+'/NoCruft/1','{genurl module='Media' action='Description'}/media_descr_personalMediaId/'+{$MobilizeContentData.mediaId}+'/NoCruft/1','{genurl module='Media' action='Tag'}/media_tag_personalMediaId/'+{$MobilizeContentData.mediaId}+'/NoCruft/1');
					//setOpenLockerRow( {$MobilizeContentData.mediaId} , panel );
                    //setOpenLockerRow( match[1], panel );
                {rdelim}
                openLockerRow( {$MobilizeContentData.mediaId}, panel );
                // default action: open the locker row, fetching the title, desc, and tags
                
				/*
				else
                {ldelim}
                    //fetchMediaData(match[1],'{genurl module='Media' action='Title'}/media_title_personalMediaId/'+match[1]+'/NoCruft/1','{genurl module='Media' action='Description'}/media_descr_personalMediaId/'+match[1]+'/NoCruft/1','{genurl module='Media' action='Tag'}/media_tag_personalMediaId/'+match[1]+'/NoCruft/1'); 
                    fetchMediaData({$MobilizeContentData.mediaId} ,'{genurl module='Media' action='Title'}/media_title_personalMediaId/'+{$MobilizeContentData.mediaId}+'/NoCruft/1','{genurl module='Media' action='Description'}/media_descr_personalMediaId/'+{$MobilizeContentData.mediaId}+'/NoCruft/1','{genurl module='Media' action='Tag'}/media_tag_personalMediaId/'+{$MobilizeContentData.mediaId}+'/NoCruft/1');
					setOpenLockerRow( {$MobilizeContentData.mediaId} );
					//setOpenLockerRow( match[1] );
                    return; 
                {rdelim}
				*/
				
	{rdelim} );
	

</script>
{/if}
