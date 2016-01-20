{ldelim}
    status: 3,
    {include file="VideoEditorView_index_base.async.tpl" assign="editorTemplate"}
    content: "{$editorTemplate|regex_replace:"/[\r\n]+/":" "|replace:'"':'\"'}",
    type: 'video',
    
    initScript: 'initMobilizeDisplay',
    
    initScriptArgs: {ldelim}
        contentType: 'video',
        
        {include file="EditorBase_carrier_js.async.tpl"}
    
    {rdelim}, {* /initScriptArgs *}

    omniture: {ldelim}
        s_pageName: "{$OmniturePageName}",
        s_channel: "{$OmnitureChannel}",
        s_prop2: "{$OmnitureProp2}",
        s_hier1: "{$OmniturePageName}"
    {rdelim},
	yuiWidth: '400px'

{rdelim}
