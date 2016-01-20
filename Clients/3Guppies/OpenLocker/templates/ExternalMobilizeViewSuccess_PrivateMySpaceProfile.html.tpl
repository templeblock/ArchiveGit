<div id='requireAuth'>
    <div id='avatar' style='float: left; width: 130px'>
        <img src='{$openl_exter_myspaceAvatarUrl}' width='128'>
    </div>
    
    <div id='login' style='float: left; width: 430px; margin-left: 1em;'>
        <div id='privateExplain' style='font-size: 13px;'>
            {if $openl_exter_myspaceLoginError}
                <div style='color: red; margin-bottom: 1em'>
                    <strong>Uh oh!</strong> We had trouble logging you in!
                    <br><br>
                    You may have entered an incorrect email or password, or you
                    may not have permission to access this profile.
                </div>
            {/if}
            
            {if $openl_exter_myspaceUserName != 'the user'}
                It looks like <strong>{$openl_exter_myspaceUserName}'s</strong> profile is set to private.
                You have to enter your MySpace email and password to send 
                <strong>{$openl_exter_myspaceUserName}'s</strong> stuff to your phone.<br><br>
            
            {else}
                It looks like {$openl_exter_myspaceUserName}'s profile is set to private.
                You have to enter your MySpace email and password to send 
                {$openl_exter_myspaceUserName}'s stuff to your phone.<br><br>
            {/if}

        </div>
        
        <form method="POST" 
            action="{genurl module='OpenLocker' action='ProxyAuth'}"
            onsubmit="return myspaceProxy_validateForm(this)">

            <input type='hidden' name='url' value='{$openl_exter_scrapeUrl}'>
            <input type='hidden' name='returnTo' 
                value='{genurl module="OpenLocker" action="ExternalMobilize"}?sUrl={$openl_exter_scrapeUrl|escape:'url'}'>
            
            <table id='loginTable' style='font-size: 13px; font-weight: bold;'>
                <tr>
                    <td></td>
                    <td>Your MySpace Login</td>
                </tr>
                <tr>
                    <td style='text-align: right'>Email:</td>
                    <td><input type='text' name='authUsername' style='width: 204px; border: 1px solid #7f9db9'></td>
                </tr>
                <tr>
                    <td style='text-align: right'>Password:</td>
                    <td><input type='password' name='authPassword' style='width: 204px; border: 1px solid #7f9db9'></td>
                </tr>
                <tr>
                    <td colspan='2' style='text-align: right'>
                        <input type='submit' class='globalBtn lightBtn' value='Login'>
                    </td>
                </tr>
            </table>
        
        </form>
    
        <div id='mixxerExplain' style='border: 1px solid black; margin-top: 2em; padding: .5em'>
            We only need your MySpace login so we can mobilize the media from this
            profile. We will not store your login or use it for any other purpose.
            <br><br>
            Read our <a href="{genurl module='Misc' action='Privacy'}" target="_blank">privacy policy...</a>
        </div>
        
    </div> <!-- /id='login' -->
    <div style='clear:both'></div>
       
</div>
