{*
    This template used in the anonymous mobilize workflow in two instances:
    1. the widget - OpenLocker/ExternalMobilizeView_input.html.tpl
    2. anonymous file upload - OpenLocker/PhoneInfoView_input.html.tpl
*}

<div class="moduleWide" id="anonPhoneWrapper">
    <div class="moduleHead userPanel"><h2>Send To Phone</h2></div>
    <div class="moduleBody white">
        <form method="POST" name="phoneForm" id="phoneForm" action="{genurl module="OpenLocker" action="Mobilize"}" style="margin: 0 10px 10px 10px;">
            <div class="column">
{if $user->isAuthenticated()}
                <strong>My Phone Number</strong> (<a href="{genurl module="User" action="Logout"}">log out</a>)<br />
                ({$myPhone1}) {$myPhone2}-{$myPhone3}<br />
                <div id="thisNumExplain" style="margin-top: 20px;">Send To</div>
                <input class="isMyPhone" id="myPhoneYes" type="radio" name="myPhone" value="yes" {if $myPhone}checked="checked" {/if}/><label for="myPhoneYes">My phone</label><br />
                <input class="isMyPhone" id="myPhoneNo" type="radio" name="myPhone" value="no" {if !$myPhone}checked="checked" {/if}/><label for="myPhoneNo">My friend's phone</label><br />
                <div id="myPhoneInfo"{if !$myPhone} style="display:block;"{/if}>
                    <strong>My Friend's Phone Number*</strong><br />
                    (<input type="text" id="phoneAreaCode2" class='phoneField' name="toPhone1" maxlength="3" style="width: 30px;"/>)
                    <input type="text" name="toPhone2" class="phoneField" maxlength="3" style="width: 30px;"/>-
                    <input type="text" name="toPhone3" class="phoneField" maxlength="4" style="width: 40px;"/><br />

                    <p style="line-height: 12pt;">Also enter your name so your friend can recognize and reply to you.</p>

                    <strong>My Name*</strong> (20 characters max.)<br />

                    <input type="text" id="fromName" name="fromName" maxlength="20" style="width: 200px;" onkeypress="charCount(20,'fromName','fromNameMsg');" onkeyup="charCount(20,'fromName','fromNameMsg');" onblur="hideCharCount('fromNameMsg');"/>
                    <div id="fromNameMsg">&#160;</div>

                    <p id="requiredFields">* Required Field</p>
                </div>
{else}
                <strong>Send To Phone Number*</strong> (or <a href='#' id='anonLoginLink'>log in</a>)<br />
                {if $LoginError}<span style="color: red;">{$LoginError}</span><br />{/if}
                {if $PhoneError}<span style="color: red;">{$PhoneError}</span><br />{/if}
                (<input type="text" id="phoneAreaCode1" class='phoneField' name="toPhone1" maxlength="3" style="width: 30px" value="{if $isShareable}{$mobilizePhone1}{else}{$myPhone1}{/if}"/>){*MyPhone*}
                 <input type="text" id="phonePrefix1"   class='phoneField' name="toPhone2" maxlength="3" style="width: 30px" value="{if $isShareable}{$mobilizePhone2}{else}{$myPhone2}{/if}"/>-
                 <input type="text" id="phoneSuffix1"   class='phoneField' name="toPhone3" maxlength="4" style="width: 40px" value="{if $isShareable}{$mobilizePhone3}{else}{$myPhone3}{/if}"/><br />
                {if $isShareable}
                <div id="thisNumExplain" style="margin-top: 20px;">This Phone Number Is</div>
                <input class="isMyPhone" id="myPhoneYes" type="radio" name="myPhone" value="yes" {if $myPhone}checked="checked" {/if}/><label for="myPhoneYes">My phone</label>&nbsp;&nbsp;
                <span id="rememberMyPhone1Wrapper" {if !$myPhone}style="visibility: hidden"{/if}><input type="checkbox" id="rememberMyPhone1" name="rememberMyPhone1" {if $rememberMyPhone1}checked="checked" {/if}/><label for="rememberMyPhone1">Remember My Phone</label></span><br>
                <input class="isMyPhone" id="myPhoneNo" type="radio" name="myPhone" value="no" {if !$myPhone}checked="checked" {/if}/><label for="myPhoneNo">My friend's phone</label><br />
                
                <div id="myPhoneInfo"{if !$myPhone} style="display: block;"{/if}>
                    <p style="line-height: 12pt;">Also enter your name and phone number so your friend can recognize and reply to you.</p>
                    
                    <strong>My Name*</strong> (20 characters max.)<br />
                    <input type="text" id="fromName" name="fromName" maxlength="20" style="width: 200px;" onkeyup="charCount(20,'fromName','fromNameMsg');" onblur="hideCharCount('fromNameMsg');"/>
                    <div id="fromNameMsg">&#160;</div>
                    
                    <strong>My Phone Number*</strong><br />
                    (<input type="text" id="phoneAreaCode2" class='phoneField' name="fromPhone1" maxlength="3" style="width: 30px;" value="{$myPhone1}"/>)
                    <input type="text" name="fromPhone2" class='phoneField' maxlength="3" style="width: 30px;" value="{$myPhone2}"/>-
                    <input type="text" name="fromPhone3" class='phoneField' maxlength="4" style="width: 40px;" value="{$myPhone3}"/><br />
                
                    <input type="checkbox" id="rememberMyPhone2" name="rememberMyPhone2" {if $rememberMyPhone2}checked="checked" {/if}/><label for="rememberMyPhone"><label for="rememberMyPhone2">Remember My Phone</label></label>
                </div>
                {else}
                <input type="hidden" name="myPhone" value="yes" />
                <span id="rememberMyPhone1Wrapper"><input type="checkbox" id="rememberMyPhone1" name="rememberMyPhone1" {if $rememberMyPhone1}checked="checked" {/if}/><label for="rememberMyPhone1">Remember My Phone</label></span><br>
                {/if}

                <p id="requiredFields">* Required Field</p>
{/if}
            </div> <!-- /class='column' -->
            
            <div id="rightColumn" class="column tags">
                <p style="margin-left: 0;">Mixxer will NEVER sell your personal info or send you spam. Read our <a href="{genurl module='Misc' action='Privacy'}" target="_blank" id="privacyLink">Privacy Policy...</a></p>
                {if $user->isAuthenticated()}
                    <input type="hidden" name="agreeTC" value="on" />
                {else}
                    <input type="checkbox" id="agreeTC" name="agreeTC" style="margin: 0 4px -1px 0;" /><label for="agreeTC">I have read and agree to the </label><a href="{genurl module='Misc' action='Tos'}" target="_blank" id="tosLink">Terms of Use</a>.<br />
                {/if}
                
                <div id="buttonContainer">
                    {if $user->isAuthenticated()}
                        <button class="globalBtn burg" id="phoneInfoSubmit" type="submit">Send</button>
                    {else}
                        <button class="globalBtn burg inactive" id="phoneInfoSubmit" type="submit" disabled="true">Send</button>
                    {/if}
                    <a id="cancelAnonMobilize" class="globalBtn burg" style="display: block; width: 60px;"  href="{genurl module="OpenLocker" action="Mobilize"}">Cancel</a>
					<div class="clear"></div>
                </div>

            </div>  <!-- /class="column tags" -->
            <div style="clear: both;"></div>
            
            <input type="hidden" name="{$InputType}" value="{$InputValue}" />
            <input type="hidden" name="Filename" value="{$Filename}" />
            <input type="hidden" name="InputMethod" value="{$InputMethod}" />
            <input type="hidden" name="SourceUrl" value="{$SourceUrl}" />
            <input type="hidden" name="Affiliate" value="{$Affiliate}" /> 
            <input type="hidden" id="loginViaUserName" name="loginViaUserName" value="" />
            {* if user selects the 'log in' option then these will be populated *}
            <input type="hidden" id="loginUsername" name="anonUsername" value="" />
            <input type="hidden" id="loginPassword" name="anonPassword" value="" />
        </form>
        
    </div> <!-- /class="box_content" -->
</div> <!-- /class='box' -->
