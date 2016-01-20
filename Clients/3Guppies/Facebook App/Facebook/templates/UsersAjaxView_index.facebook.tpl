{* using variables:
	$arrUsers[n].userid
	$arrUsers[n].name
	$arrUsers[n].selected
	$arrUsers[n].num_photos
*}

{foreach item=usr from=$arrUsers}
	<option value="{$usr.userid}" {$usr.selected}>{$usr.name}{$usr.num_photos}</option>
{/foreach}
