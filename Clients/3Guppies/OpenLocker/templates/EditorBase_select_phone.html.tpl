<div id="editInstructions">
	<p>To format your {$formatMess}, select your{if !$carrierId} carrier and{/if} phone model.</p>
</div>

{* commenting out for now 
<div id="phonePictureWrapper">
    <a href="#" id="showPhonePicMap">Or see phone pictures...</a>
</div>

*}

{if !$carrierId}
<div class="stepWrapper">
    <!-- <img class="editStep" src="{$URL_BASE}images/crop_step_{$stepCount++}.gif" /> -->
    <select name="carrier" id="carrierSelect">
        <option value="-1">Select carrier...</option>
        <option value="-2">I don't see my carrier listed</option>
        {if $carrierId}
            {html_options options=$CarrierInfo selected=$carrierId}
        {else}
            {html_options options=$CarrierInfo}
        {/if}
    </select>
</div>
<br />
{else}
<input type="hidden" name="carrier" value="{$carrierId}" />
{/if}

{if $myPhone}
<div class="stepWrapper">

    <span id="selectPhone">
        <!-- <img class="editStep" src="{$URL_BASE}images/crop_step_{$stepCount++}.gif" /> -->
        <select name="EditOptions[deviceId]" id="phoneModelSelect">
            <option value="-1" selected="1">Select phone...</option>
            <option value="-3">- I don't want to select a phone -</option>
            <option value="-2">- I don't see my phone listed -</option>
            {html_options options=$DeviceIdNameMap selected=$deviceId}
        </select>
    </span>

    <div id="suggestCarrierAndPhone">
        <!-- <img class="editStep" src="{$URL_BASE}images/crop_step_{$stepCount++}.gif" /> -->

        <div id="carrierPhoneContainer">
            <div id="enterPhoneCarrier">
                Tell us your phone carrier.<br />
                <input type="text" name="suggestCarrier" class="mobilizeTitle" />
            </div>
            
            <div id="enterModelManufacturer">
                Tell us your phone manufacturer and model.<br />
                <input type="text" name="suggestPhone" class="mobilizeTitle" />
            </div>
        </div>
    </div>
</div>
{/if}
