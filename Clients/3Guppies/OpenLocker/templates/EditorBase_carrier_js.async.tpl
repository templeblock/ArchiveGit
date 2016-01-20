{if $user->isAuthenticated()}
    isAnonymous: false
{else}
    isAnonymous: true,
    myPhone: {if $MyPhone}true{else}false{/if},
    {if $deviceId}
        selectedDeviceId: {$deviceId},
    {/if}
        
    carrierSelectId: 'carrierSelect',
    phoneModelSelectId: 'phoneModelSelect',
    
    {* builds a list of carriers assigned to 'name' and 'devices'
       if the $carrierId (resolved carrier id from phone number) and 
       $DeviceInfo (all devices for the carrier) variables exist then that
       carriers devices will be populated *}
    
    carriers: {ldelim}
        {foreach from=$CarrierInfo key=curCarrierId item=carrierName name=carriers}
            {$curCarrierId}: {ldelim} name: "{$carrierName|replace:'"':'\"'}", devices: [
                {if $DeviceInfo && $carrierId && $carrierId == $curCarrierId}
            
                    {foreach from=$DeviceInfo item=info name=devices}
                        {ldelim}
                            deviceId: {$info.mobileDevice_id},
                            carrierId: {$carrierId},
                            name: "{$info.mobileDevice_model}",
                            width: {$info.mobileDevice_display_width},
                            height: {$info.mobileDevice_display_height},
                            image: "{$IMAGES_DIR}gallery/devices/{$info.mobileDevice_image_file}"
                        {rdelim}
                        {if !$smarty.foreach.devices.last}, {/if}
                    {/foreach}
    
                {/if}
            ] {rdelim} {if !$smarty.foreach.carriers.last}, {/if}
        {/foreach}
    {rdelim}
{/if}
