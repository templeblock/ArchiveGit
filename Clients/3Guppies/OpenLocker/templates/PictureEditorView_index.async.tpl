{ldelim}
    status: 3,
    type: 'picture',
    
    {include file="PictureEditorView_index_base.async.tpl" assign="editorBase"}
    content: "{$editorBase|regex_replace:"/[\r\n]+/":" "|replace:'"':'\"'}",
    
    initScript: 'initMobilizeDisplay',
    
    initScriptArgs: {ldelim}

        contentType: 'picture',
        origFilename: "{$ContentInfo.PreviewUrl}",
        origWidth: {$Dimensions.original.width},
        origHeight: {$Dimensions.original.height},
        deviceWidth: {$DeviceDim.width},
        deviceHeight: {$DeviceDim.height},
        {if $NoCroppingAllowed}noCrop: "true",{/if}
        
        {include file="EditorBase_carrier_js.async.tpl"}
    {rdelim}, {* /initScriptArgs *}
    
    omniture: {ldelim}
        s_pageName: "{$OmniturePageName}",
        s_channel: "{$OmnitureChannel}",
        s_prop2: "",
        s_hier1: "{$OmniturePageName}"
    {rdelim},
	yuiWidth: '420px'
{rdelim}
