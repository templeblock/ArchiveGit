{* using variables:
	$userid
	$arrPhotos[n].src_thumb
	$arrPhotos[n].src_fullsize
	$arrPhotos[n].caption
*}

<div>
	{foreach item=photo from=$arrPhotos}
		<span class="unselectedThumb" id="mxr-thumb" onclick="SelectImage(this)" ondblclick="PreviewImage()" >
			<img class="imgThumb" src="{$photo.src_thumb}" title="{$photo.caption|escape}" photo="{$photo.src_fullsize}" alt="" />
		</span>
	{/foreach}
</div>
<div style="clear: both;" />
