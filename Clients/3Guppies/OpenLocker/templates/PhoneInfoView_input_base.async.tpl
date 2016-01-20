<!-- START PhoneInfoView_input_base.async.tpl -->

<div class="moduleWide">
    <div class="moduleBody">
        <form id="anonPhoneWrapper" method="POST" id="phoneForm" name="phoneForm" action="{genurl module="OpenLocker" action="Mobilize"}">
{if $user->isAuthenticated()}
                <h1>My phone number</h1> <!-- &nbsp; (<a href="{genurl module="User" action="Logout"}">log out</a>) -->
				<br />
                <h3 class="myPhone">({$myPhone1}) {$myPhone2} - {$myPhone3}</h3>
				<br />
                <span>Send to</span>
				<br />
                <input class="isMyPhone" id="myPhoneYes" type="radio" name="myPhone" value="yes" {if $myPhone}checked="checked" {/if}/><label for="myPhoneYes">My phone</label><br />
                <input class="isMyPhone" id="myPhoneNo" type="radio" name="myPhone" value="no" {if !$myPhone}checked="checked" {/if}/><label for="myPhoneNo">My friend's phone</label><br />
				
                <div id="myPhoneInfo"{if !$myPhone} style="display:block;"{/if}>
					<span>Enter your own name so your friend can recognize you and reply</span><br />
					
					<input type="text" id="fromName" name="fromName" maxlength="20" style="width: 200px; border: 1px solid #900; font-weight: bold; padding: 1px;"  onkeypress="charCount(20,'fromName','fromNameMsg');return preventEnterKey(event)" onkeyup="charCount(20,'fromName','fromNameMsg');" onblur="hideCharCount('fromNameMsg');" />
					<br />
					<span id="fromNameMsg" class="charLimit">&#160;</span>
					
					<br />
					
					<span>My Friend's Phone Number</span><br />
                    (<input type="text" id="phoneAreaCode2" class="phoneField" name="toPhone1" maxlength="3" style="width: 30px;"/>)
                    <input type="text" name="toPhone2" class="phoneField" maxlength="3" style="width: 30px;"/>-
                    <input type="text" name="toPhone3" class="phoneField" maxlength="4" style="width: 40px;"/>
                </div>
{else}
                <h1>Send to this number</h1> <!-- &nbsp; (<a href="#" id="anonLoginLink">log in</a>)-->
				<br />
                {if $LoginError}<span style="color: red;">{$LoginError}</span><br />{/if}
                {if $PhoneError}<span style="color: red;">{$PhoneError}</span><br />{/if}
				
				<div id="phoneNumber">
					(<input type="text" id="phoneAreaCode1" class="phoneField" name="toPhone1" maxlength="3" style="width: 2.1em;" value="{if $isShareable}{$mobilizePhone1}{else}{$myPhone1}{/if}" />){*MyPhone*} 
					<input type="text" id="phonePrefix1" class="phoneField" name="toPhone2" maxlength="3" style="width: 2.1em;" value="{if $isShareable}{$mobilizePhone2}{else}{$myPhone2}{/if}" />-<input 
					type="text" id="phoneSuffix1" class="phoneField" name="toPhone3" maxlength="4" style="width: 2.5em;" value="{if $isShareable}{$mobilizePhone3}{else}{$myPhone3}{/if}" />
				 </div>
				 
				 <br />
				 
                {if $isShareable}
                <span>This is</span>
				<br />
                <input class="isMyPhone" id="myPhoneYes" type="radio" name="myPhone" value="yes" {if $myPhone}checked="checked" {/if}/><label for="myPhoneYes">My phone</label>&nbsp;&nbsp;&nbsp;&nbsp;
                <span id="rememberMyPhone1Wrapper" {if !$myPhone}style="visibility: hidden"{/if}><input type="checkbox" id="rememberMyPhone1" name="rememberMyPhone1" {if $rememberMyPhone1}checked="checked" {/if} style="margin-right: 8px;" /><label for="rememberMyPhone1">Remember me</label></span><br>
                <input class="isMyPhone" id="myPhoneNo" type="radio" name="myPhone" value="no" {if !$myPhone}checked="checked" {/if}/><label for="myPhoneNo">My friend's phone</label><br />
                
                <div id="myPhoneInfo"{if !$myPhone} style="display: block;"{/if}>
                    <span>Enter your own name so your friend can recognize you and reply</span><br />
                    <input type="text" id="fromName" name="fromName" maxlength="20" style="width: 200px; border: 1px solid #900; font-weight: bold; padding: 1px;"  onkeypress="charCount(20,'fromName','fromNameMsg');return preventEnterKey(event)" onkeyup="charCount(20,'fromName','fromNameMsg');" onblur="hideCharCount('fromNameMsg');" />
					<br />
					<span id="fromNameMsg" class="charLimit">&#160;</span>
					
					<br />
                    
                    (<input type="text" id="phoneAreaCode2" class="phoneField" name="fromPhone1" maxlength="3" style="width: 30px;" value="{$myPhone1}"/>)
                    <input type="text" name="fromPhone2" class="phoneField" maxlength="3" style="width: 30px;" value="{$myPhone2}"/>-
                    <input type="text" name="fromPhone3" class="phoneField" maxlength="4" style="width: 40px;" value="{$myPhone3}"/>&nbsp;<input type="checkbox" id="rememberMyPhone2" name="rememberMyPhone2" {if $rememberMyPhone2}checked="checked" {/if} /><label for="rememberMyPhone"><label for="rememberMyPhone2">Remember me</label></label>
                </div>
                {else}
                <input type="hidden" name="myPhone" value="yes" />
                <span id="rememberMyPhone1Wrapper"><input type="checkbox" id="rememberMyPhone1" name="rememberMyPhone1" {if $rememberMyPhone1}checked="checked" {/if}style="margin-right: 8px;" /><label for="rememberMyPhone1">Remember me</label></span>
				<br />
                {/if}
{/if}

            <div id="rightColumn" class="tags">
				{if $user->isAuthenticated()}
					<input type="hidden" name="agreeTC" value="on" />
					<br />
				{else}
					<input type="checkbox" id="agreeTC" name="agreeTC" style="margin-right: 8px;" /><label for="agreeTC">I have read and agree to the </label>
					<a href="{genurl module='Misc' action='Tos'}" target="_blank" id="tosLink">Terms of Use</a>.
					<br />
					<br />
				{/if}

				<div class="textCenter">
					<div id="buttonContainer2">
						{if $user->isAuthenticated()}
							<input class="globalBtn burg" id="phoneInfoSubmit" type="submit" style="width: 70px; padding: 0;" value="Continue &raquo;" />&nbsp;
						{else}
							<input class="globalBtn burg inactive" id="phoneInfoSubmit" type="submit" style="width: 70px; padding: 0;" value="Continue &raquo;" disabled="true" />&nbsp;
						{/if}
						<a id="cancelAnonMobilize" href="{genurl module="OpenLocker" action="Mobilize"}"><input class="globalBtn burg"  style="width: 60px;" value="Cancel" /></a>
					</div>
				</div>
				
				<br />
				
				<p class="finePrint">Mixxer will NEVER sell your personal info or send you spam. Read our <a href="{genurl module='Misc' action='Privacy'}" target="_blank" id="privacyLink">Privacy Policy</a>...</p>

            </div>
            
            <input type="hidden" name="{$InputType}" value="{$InputValue}" />
            <input type="hidden" name="Filename" value="{$Filename}" />
            <input type="hidden" name="InputMethod" value="{$InputMethod}" />
            <input type="hidden" name="SourceUrl" value="{$SourceUrl}" />
            <input type="hidden" name="Affiliate" value="{$Affiliate}" /> 
            <input type="hidden" name="Title" value="{$Title}" /> 
            <input type="hidden" id="loginViaUserName" name="loginViaUserName" value="" />
            {* if user selects the 'log in' option then these will be populated *}
            <input type="hidden" id="loginUsername" name="anonUsername" value="" />
            <input type="hidden" id="loginPassword" name="anonPassword" value="" />
        </form>
    </div>
</div>

<!-- END PhoneInfoView_input_base.async.tpl -->
