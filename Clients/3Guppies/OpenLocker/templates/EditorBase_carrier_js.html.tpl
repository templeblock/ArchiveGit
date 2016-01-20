initArgs.isAnonymous = false;
initArgs.carriers = null;
initArgs.uploadFormId = 'uploadForm';

{if $deviceId}
initArgs.selectedDeviceId = {$deviceId};
{/if}

{if $user->isAuthenticated()}
    initArgs.deviceWidth = {$DeviceDim.width};
    initArgs.deviceHeight = {$DeviceDim.height};
    
{elseif !$user->isAuthenticated()}

    // set known initialization arguments
    initArgs.isAnonymous = true;
    initArgs.myPhone = {if $MyPhone}true{else}false{/if};
    
    initArgs.phoneModelSelectId = 'phoneModelSelect';
    initArgs.viewPicsAnchorId   = 'showPhonePicMap';

    var carriers = {ldelim}
        {foreach from=$CarrierInfo key=carrierId item=carrierName name=carriers}
            {$carrierId}: {ldelim} name: "{$carrierName|replace:'"':'\"'}", devices: [] {rdelim} {if !$smarty.foreach.carriers.last}, {/if}
        {/foreach}
    {rdelim};
    
    {if $carrierId && $DeviceInfo}
        initArgs.carrierId = {$carrierId};
        
        {foreach from=$DeviceInfo item=info}
            carriers[ {$carrierId} ].devices.push( {ldelim}
                deviceId: {$info.mobileDevice_id},
                carrierId: {$carrierId},
                name: "{$info.mobileDevice_model}",
                width: {$info.mobileDevice_display_width},
                height: {$info.mobileDevice_display_height},
                image: "{$IMAGES_DIR}gallery/devices/{$info.mobileDevice_image_file}" 
            {rdelim} );
        {/foreach}
    {/if}

    initArgs.carriers = carriers;
    
    {if $PromptCarrier}
        initArgs.carrierSelectId = 'carrierSelect';
    {/if}
    
    
    
    
{/if} {* !$user->isAuthenticated() *}

initMobilizeDisplay( initArgs );

{* :mode=javascript: *}
