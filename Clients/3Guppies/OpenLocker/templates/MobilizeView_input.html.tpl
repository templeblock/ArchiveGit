<!-- BEGIN MobilizeView_input.html.tpl -->
{addheaderfile file=$URL_BASE|cat:'javascript/yui/connection/connection.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/yui/mixxer/classes/carriers_devices.js'}
<script type="text/javascript">
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

carsAndDevices = new YAHOO.mixxer.util.CarriersDevices();

{/literal}
s_pageName = "{$OmniturePageName}";
s_channel = "{$OmnitureChannel}";
s_prop2 = "{$OmnitureProp2}";
s_hier1 = "{$OmniturePageName}";
</script>

<div class="moduleWide">
	<span id="mediaHeader"></span><div id="aj_mediaHeader">{$media_heade_contents}</div>
	<div class="moduleBody white" style="padding: 10px;">
				
		{if isset($MobilizeError)}
		<strong><span class="error">{$MobilizeError}</span></strong><br />
		{/if}

		{if isset($MobilizeSuccess)}
		<strong>Upload finished! Add more...</strong><br />
		{else}
		{/if}
		
		{include file="locker_upload.html.tpl"}
		
	</div>
</div>
<!-- END MobilizeView_input.html.tpl -->
