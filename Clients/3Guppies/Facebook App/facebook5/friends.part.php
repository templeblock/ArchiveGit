<div>
	<table border="0" cellspacing="0">
		<tr style="vertical-align:top; ">
			<td style="width:100px; min-height:100px; ">
				<a target="_parent" href="<?= $canvasPageUrl ?>mobilize.php?user=<?= $userid ?>"><img src="<?= $picUrl ?>" alt="" /></a>
			</td>
			<td style="width:160px;">
				<a target="_parent" href="<?= $canvasPageUrl ?>mobilize.php?user=<?= $userid ?>"><?= $name ?></a><br/>
				<a target="_parent" href="<?= $canvasPageUrl ?>mobilize.php?user=<?= $userid ?>">Mobilize <?= $hisHerTheir ?> Photos (<?= $numPhotos ?>)</a><br/>
				<a target="_parent" href="<?= $canvasPageUrl ?>friendsofriends.php?user=<?= $userid ?>">View <?= $hisHerTheir ?> Tagged Photo Friends</a><br/>
				<?= $inviteOrAnnounce ?><br/>
			</td>
			<td style="width:200px;">
				<?= $networks ?>
			</td>
		</tr>
	</table>
</div>
