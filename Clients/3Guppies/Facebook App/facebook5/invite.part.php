<div style="margin:2px; padding:2px; width:90px; height:75px; float:left; text-align:left; vertical-align:top; ">
	<a target="_parent" href="<?= $canvasPageUrl ?>send.php?invitation&user=<?= $userid ?>">
		<img style="width:50px;" src="<?= $picUrl ?>" alt="" /></a>
	<?php if (!$bIsAppUser): ?>
		<input type="checkbox" name="inviteSelect[]" value="<?= $userid ?>" <?= ($bIsAppUser ? '' : 'checked="checked"') ?> />
	<?php endif; ?>
	<div style="margin-top:3px;"><?= $name ?></div>
</div>






