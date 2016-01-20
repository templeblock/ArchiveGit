<!-- BEGIN VideoEditorView_index.html.tpl -->
{addheaderfile file=$URL_BASE|cat:'javascript/yui/connection/connection.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/yui/mixxer/classes/carriers_devices.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/openlocker/mobilize_funcs.js'}
{addheaderfile file=$URL_BASE|cat:'css/openlocker/mobilize_editor.css'}

<script type="text/javascript">
    // set known initialization arguments
    var initArgs = {ldelim}
        contentType: 'video'
    {rdelim};
    
    {include file="EditorBase_carrier_js.html.tpl"}

    s_pageName = "{$OmniturePageName}";
    s_channel = "{$OmnitureChannel}";
    s_prop2 = "{$OmnitureProp2}";
    s_hier1 = "{$OmniturePageName}";
</script>

{include file="VideoEditorView_index_base.html.tpl"}
<!-- END VideoEditorView_index.html.tpl -->
