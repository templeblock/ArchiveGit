<!-- BEGIN PictureEditorView_index.html.tpl -->

{addheaderfile file=$URL_BASE|cat:'javascript/MobilizeCrop.js'}
{addheaderfile file=$URL_BASE|cat:'css/MobilizeCrop.css'}

{addheaderfile file=$URL_BASE|cat:'javascript/yui/connection/connection.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/yui/container/container.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/yui/dragdrop/dragdrop.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/yui/container/assets/container.css'}
{addheaderfile file=$URL_BASE|cat:'javascript/yui/mixxer/classes/carriers_devices.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/openlocker/mobilize_funcs.js'}


{addheaderfile file=$URL_BASE|cat:'css/openlocker/mobilize_editor.css'}

<script type="text/javascript">
    var origFilename = "{$ContentInfo.PreviewUrl}";
    var origWidth  = {$Dimensions.original.width};
    var origHeight = {$Dimensions.original.height};

    // var MobilizeCrop = FactoryCropper(origFilename, origWidth, origHeight, {$DeviceDim.width}, {$DeviceDim.height} );
    
    // YUIEvent.addListener( window, 'load', MobilizeCrop.Initialize );
    
    /** 
     * these initialization arguments are passed to the initialization function
     * which will add event listeners to different objects on the page depending
     * on which values are set. This allows us to integrate the image cropper
     * into the widget page
     * @see javascript/openlocker/mobilize_funcs.js for the initialization function
     * @see EditorBase_carrier_js.html.tpl for more of these initialization params
     */
    initArgs = {ldelim}
        contentType: 'picture',
        origWidth: origWidth,
        origHeight: origHeight,
        deviceWidth: {$DeviceDim.width},
        deviceHeight: {$DeviceDim.height},
        {if $CannotDetectCarrier}noShow: "asdfasdfasdf",{/if}
        {if $NoCroppingAllowed}noCrop: "true",{/if}
        origFilename: origFilename
    {rdelim};
    
    {include file="EditorBase_carrier_js.html.tpl"}
    
    s_pageName = "{$OmniturePageName}";
    s_channel = "{$OmnitureChannel}";
    s_prop2 = "{$OmnitureProp2}";
    s_hier1 = "{$OmniturePageName}";
</script>

{include file="PictureEditorView_index_base.html.tpl"}
<!-- END PictureEditorView_index.html.tpl -->
