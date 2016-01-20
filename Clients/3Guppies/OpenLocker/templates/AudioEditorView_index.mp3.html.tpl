<!-- BEGIN AudioEditorView_index.html.tpl -->

{addheaderfile file=$URL_BASE|cat:'javascript/audio_mobilize/MobilizeClip.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/audio_mobilize/EmbedFlash.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/audio_mobilize/EmbedWMPlayer.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/audio_mobilize/EmbedQTPlayer.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/audio_mobilize/PlayMP3Clip.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/audio_mobilize/PlayWMPClip.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/audio_mobilize/PlayQTClip.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/audio_mobilize/InitializeClip.js'}
{addheaderfile file=$URL_BASE|cat:'css/MobilizeAudioCrop.css'}

{addheaderfile file=$URL_BASE|cat:'javascript/yui/connection/connection.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/yui/container/container.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/yui/dragdrop/dragdrop.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/yui/container/assets/container.css'}
{addheaderfile file=$URL_BASE|cat:'javascript/yui/mixxer/classes/carriers_devices.js'}
{addheaderfile file=$URL_BASE|cat:'javascript/openlocker/mobilize_funcs.js'}


{addheaderfile file=$URL_BASE|cat:'css/openlocker/mobilize_editor.css'}

<script type="text/javascript">
    
    initArgs = {ldelim}
        contentType: 'mp3Audio',
        imagesUrl: '{$IMAGES_DIR}',
        previewUrl: '{$ContentInfo.PreviewUrl}',
        trackSeconds: {$TrackInSeconds},
        swfPath: '{$URL_BASE}'
    {rdelim};
    
    {include file="EditorBase_carrier_js.html.tpl"}

    s_pageName = "{$OmniturePageName}";
    s_channel = "{$OmnitureChannel}";
    s_prop2 = "{$OmnitureProp2}";
    s_hier1 = "{$OmniturePageName}";
</script>

{include file="AudioEditorView_index_base.mp3.html.tpl"}
<!-- END AudioEditorView_index.html.tpl -->
