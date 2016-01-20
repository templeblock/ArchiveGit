{ldelim} 
    status : {$openl_exter_widgetcode},
    
    {if $openl_exter_widgetcode == $smarty.const.WIDGET_MYSPACE_PRIVATE_PROFILE}
        {include file="ExternalMobilizeViewSuccess_PrivateMySpaceProfile.html.tpl" assign="contentsTemplate"}
        content : "{$contentsTemplate|regex_replace:"/[\r\n]+/":" "|replace:'"':'\"'}"
    {else}
        content : '{$openl_exter_contents}'
    {/if}
    
    {if $openl_exter_prettyUrl}
        , prettyUrl: '{$openl_exter_prettyUrl}'
    {/if}
    
    {if $openl_exter_myspaceUserName}
        , myspaceUserName: '{$openl_exter_myspaceUserName}'
    {/if}
    
    {if $openl_exter_myspaceAvatarUrl}
        , avatarUrl: '{$openl_exter_myspaceAvatarUrl}'
    {/if}
    
    {if $openl_exter_profileIsPrivate}
        , privateProfile: true
    {/if}
    
    {if $openl_exter_myspaceLoginError}
        , myspaceLoginError: true
    {/if}
    
    {if $openl_exter_serverEls}
    , els: [
        {foreach from=$openl_exter_serverEls item=el name='serverElList'}
            {ldelim}
                src: "{$el.src|replace:'"':'\"'}"
                {if $el.type}
                    , type: {$el.type}
                {/if}
                {if $el.title}
                    , title: "{$el.title|replace:'"':'\"'}"
                {/if}
                {if $el.flashAttrs}
                    , flashAttrs: {ldelim}
                        {foreach from=$el.flashAttrs item=val key=name name='flashAttrList'}
                            {$name}: "{$val|replace:'"':'\"'}"{if !$smarty.foreach.flashAttrList.last}, {/if}
                        {/foreach}
                    {rdelim}
                {/if}
            {rdelim} {if !$smarty.foreach.serverElList.last}, {/if}
        {/foreach}
    ]
    {/if}
{rdelim}
