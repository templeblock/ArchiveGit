<!-- TEMP START: AudioEditorView_index.default.html.tpl -->
{addheaderfile file=$URL_BASE|cat:'javascript/yui/connection/connection.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/yui/mixxer/classes/carriers_devices.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/openlocker/mobilize_funcs.js'}
{addheaderfile file=$URL_BASE|cat:'css/openlocker/mobilize_editor.css'}

<script type="text/javascript">
    
    initArgs = {ldelim}
        contentType: 'audio',
        imagesUrl: '{$IMAGES_DIR}',
        previewUrl: '{$ContentInfo.PreviewUrl}',
        trackSeconds: {$TrackInSeconds}
    {rdelim};
    
    {include file="EditorBase_carrier_js.html.tpl"}

    s_pageName = "{$OmniturePageName}";
    s_channel = "{$OmnitureChannel}";
    s_prop2 = "{$OmnitureProp2}";
    s_hier1 = "{$OmniturePageName}";
</script>

{include file="AudioEditorView_index_base.html.tpl"}
