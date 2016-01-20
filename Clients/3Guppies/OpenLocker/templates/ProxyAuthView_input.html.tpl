<div class="moduleWide">

    <div class="moduleHead"><h2>Please enter your myspace.com username and password</h2></div>

    
    <div class="moduleBody white">
        <form method={if $openl_proxy_Instance}"post"{elseif $openl_proxy_needUrl}"get"{/if}>
            
            {if $openl_proxy_returnTo}
                <input type="hidden" name="returnTo" value="{$openl_proxy_returnTo}">
            {/if}
            
            <div class="error">
                {if $errors}
                    {foreach from=$errors item=error}
                        {$error}<br>
                    
                    {/foreach}
                {/if}
            </div>
            
            {if $openl_proxy_success}
                <div class="success">
                    You are now logged in
                </div>
            {/if}
            
            <table>
                {if $openl_proxy_showUrl}
                    <tr>
                        <td>Website url you are trying to login to</td>
                        <td><input type="text" name="url" 
                                {if $openl_proxy_url}value="{$openl_proxy_url}"{/if} /></td>
                    </tr>
                {/if}
                <tr>
                    <td>{if $openl_proxy_authUserName}{$openl_proxy_authUserName}{else}email{/if}</td>
                    <td><input type="text" name="authUsername"></td>
                </tr>
                
                <tr>
                    <td>{if $openl_proxy_authPassName}{$openl_proxy_authPassName}{else}password{/if}</td>
                    <td><input type="password" name="authPassword"></td>
                </tr>
                
                <tr>
                    <td><input class="globalBtn grey" type="submit" 
                         name="proxyAuthSubmit" value="submit"></td>
                </tr>
                
            </table>
        </form>
    
    </div>

</div>
