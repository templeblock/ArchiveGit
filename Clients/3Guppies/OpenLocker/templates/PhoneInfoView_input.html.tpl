<!-- TEMP START: PhoneInfoView_index.html.tpl -->


{addheaderfile file=$URL_BASE|cat:"javascript/yui/mixxer/form.js"}
{addheaderfile file=$URL_BASE|cat:"javascript/openlocker/anon_mobilize_form.js"}
{addheaderfile file=$URL_BASE|cat:"javascript/yui/container/container.js"}
{addheaderfile file=$URL_BASE|cat:"javascript/yui/dragdrop/dragdrop.js"}
{addheaderfile file=$URL_BASE|cat:"javascript/yui/connection/connection.js"}
{addheaderfile file=$URL_BASE|cat:"javascript/yui/mixxer/container/async_panel.js"}

{addheaderfile file=$URL_BASE|cat:"javascript/yui/container/assets/container.css"}
{addheaderfile file=$URL_BASE|cat:"css/openlocker/anon_mobilize_form.css"}
{addheaderfile file=$URL_BASE|cat:"css/yui/container/mixxer_container.css"}

<script type="text/javascript">
YUIEvent.addListener( window, 'load', initAnonMobilizeForm );
s_pageName = "{$OmniturePageName}";
s_channel = "{$OmnitureChannel}";
s_prop2 = "{$OmnitureProp2}";
s_hier1 = "{$OmniturePageName}";
</script>

{include file="PhoneInfoView_input_base.html.tpl"}
<!-- TEMP END: PhoneInfoView_index.html.tpl -->
