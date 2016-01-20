{* using variables:
	$arrUsers[n].userid
	$arrUsers[n].numPhotos
	$arrUsers[n].numAlbums
*}

{foreach item=usr from=$arrUsers}
<span>
    {$usr.numPhotos} Photo{if $usr.numPhotos != 1}s{/if}<br/>
    {$usr.numAlbums} Album{if $usr.numAlbums != 1}s{/if}<br/>
</span>
{/foreach}
