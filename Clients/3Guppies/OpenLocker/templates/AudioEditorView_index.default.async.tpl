{ldelim}
    status: 3,
    {include file="AudioEditorView_index_base.default.async.tpl" assign="editorTemplate"}
    content: "{$editorTemplate|regex_replace:"/[\r\n]+/":" "|replace:'"':'\"'}",
    type: 'audio',
    omniture: {ldelim}
        s_pageName: "{$OmniturePageName}",
        s_channel: "{$OmnitureChannel}",
        s_prop2: "",
        s_hier1: "{$OmniturePageName}"
        {rdelim},
    
    initScript: 'initMobilizeDisplay',
    initScriptArgs: {ldelim}
        contentType: 'audio',
        
        {include file="EditorBase_carrier_js.async.tpl"}
    {rdelim},
	yuiWidth: '330px'
{rdelim}
