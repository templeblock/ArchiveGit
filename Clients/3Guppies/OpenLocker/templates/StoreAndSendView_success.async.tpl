{* TEMP START: StoreAndSendView_success.async.tpl *}
{ldelim}
    status: 1,
    {include file="StoreAndSendView_success_base.async.tpl" assign="successPage"}
    content: "{$successPage|regex_replace:"/[\r\n]+/":" "|replace:'"':'\"'}",
    cancelBtn: "mobilizeAgain",
    omniture: {ldelim}
        s_pageName: "{$OmniturePageName}",
        s_channel: "{$OmnitureChannel}",
        s_prop2: "{$OmnitureProp2}",
        s_events: "event3",
        s_hier1: "{$OmniturePageName}"
        {rdelim},
	yuiWidth: '550px',
    toPhone: '{$ToPhoneNumber}'
{rdelim}
{* TEMP END: StoreAndSendView_success.async.tpl *}
