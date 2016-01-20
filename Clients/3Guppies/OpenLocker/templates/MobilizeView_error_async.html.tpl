{if $MobilizeError}
{ldelim}
    status: 0,
    {include file="ExternalMobilizeView_error_base.async.tpl" assign="contentsTemplate"}
	content : "{$contentsTemplate|regex_replace:"/[\r\n]+/":" "|replace:'"':'\"'}",
    omniture: {ldelim}
        s_pageName: "{$OmniturePageName}",
        s_hier1: "{$OmniturePageName}",
        s_channel: "{$OmnitureChannel}",
        s_prop2: ""
    {rdelim},
    yuiWidth: '420px'
{rdelim}
{/if}
