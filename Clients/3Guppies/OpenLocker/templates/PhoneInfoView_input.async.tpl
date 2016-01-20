{ldelim}
    status: 2,
    {include file="PhoneInfoView_input_base.async.tpl" assign="phoneTemplate"}
    content: "{$phoneTemplate|regex_replace:"/[\r\n]+/":" "|replace:'"':'\"'}",
    initScript: "initAnonMobilizeForm",
    initScriptArgs: {ldelim}
        sourceForm: "async"
    {rdelim},
    cancelBtn: "cancelAnonMobilize",
    omniture: {ldelim}
        s_pageName: "{$OmniturePageName}",
        s_channel: "{$OmnitureChannel}",
        s_prop2: "",
        s_hier1: "{$OmniturePageName}"
    {rdelim},
    yuiWidth: '300px',
    yuiMarginTop: '60px'
{rdelim}
