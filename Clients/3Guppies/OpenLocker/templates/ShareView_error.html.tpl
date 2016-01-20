{* 
    $openl_share_OutputContext can be
    'HoverOverMedia' where the overlay is shown over the thumbnail of the media

*}

{* do we need to differentiate between mobilize and send2friend here? *}

<div class="textCenter redText" 
    {if $openl_share_OutputContext == 'HoverOverMedia'}style="width: 185px; background-color: white; border: 1px solid #990000; text-align: center;"{/if}>
    <strong>Error: {$ShareError}</strong>
</div>
