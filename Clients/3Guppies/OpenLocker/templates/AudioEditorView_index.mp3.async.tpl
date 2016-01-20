{ldelim}

    status: 3,
    {include file="AudioEditorView_index_base.mp3.async.tpl" assign="editorTemplate"}
    content: "{$editorTemplate|regex_replace:"/[\r\n]+/":" "|replace:'"':'\"'}",
    
    initScript: 'initMobilizeDisplay',
    initScriptArgs: {ldelim}
        contentType: 'mp3Audio',
        imagesUrl: '{$IMAGES_DIR}',
        previewUrl: '{$ContentInfo.PreviewUrl}',
        trackSeconds: {$TrackInSeconds},
        swfPath: '{$URL_BASE}',
        
        {include file="EditorBase_carrier_js.async.tpl"}
    {rdelim},
    omniture: {ldelim}
        s_pageName: "{$OmniturePageName}",
        s_channel: "{$OmnitureChannel}",
        s_prop2: "",
        s_hier1: "{$OmniturePageName}"
    {rdelim},
	yuiWidth: '550px'
    
{rdelim}
