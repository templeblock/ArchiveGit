<div style="padding:10px;">
	<table border="0" cellspacing="0">
		<tr>
			<td>
				<h4>Now viewing:&nbsp;&nbsp;&nbsp;&nbsp;</h4>
			</td>
			<td>
				<form id="searchTypeForm" name="searchTypeForm" method="get" target="_parent" action="<?= $canvasPageUrl ?>friends.php">
					<select name="searchType" onchange="document.getElementById('searchTypeForm').submit();">
						<option value="-all"     <?= ($searchType == '-all'     ? 'selected=""' : '') ?> >All Friends</option>
						<option value="-updated" <?= ($searchType == '-updated' ? 'selected=""' : '') ?> >Friends with Updated Profiles</option>
						<option value="-with"    <?= ($searchType == '-with'    ? 'selected=""' : '') ?> >Friends with 3Guppies</option>
						<option value="-without" <?= ($searchType == '-without' ? 'selected=""' : '') ?> >Friends without 3Guppies</option>
						<option value="-college" <?= ($searchType == '-college' ? 'selected=""' : '') ?> >College Friends</option>
						<option value="-hs"      <?= ($searchType == '-hs'      ? 'selected=""' : '') ?> >High School Friends</option>
						<option value="-work"    <?= ($searchType == '-work'    ? 'selected=""' : '') ?> >Work Friends</option>
						<option value="-home"    <?= ($searchType == '-home'    ? 'selected=""' : '') ?> >Hometown Friends</option>
						<option value="-city"    <?= ($searchType == '-city'    ? 'selected=""' : '') ?> >Current Location Friends</option>
						<option value="-region"  <?= ($searchType == '-region'  ? 'selected=""' : '') ?> >Regional Friends</option>
					<?php if ($searchTerm): ?>
						<option value="-custom" selected="">Custom Search <?= '"' . $searchTerm . '"' ?></option>
					<?php endif; ?>
					</select>
					<input type="hidden" name="user" value="<?= $userid ?>" />
				</form>
			</td>
			<td>
				<form name="searchTermForm" method="get" target="_parent" action="<?= $canvasPageUrl ?>friends.php">
					<input name="searchTerm" id="searchTerm" type="search" class="inputtext inputsearch" placeholder="Search within all friends" value="" />
					<script type="text/javascript">placeholderSetup('searchTerm');</script>
					<input type="submit" value="Search" class="inputsubmit" />
					<input type="hidden" name="user" value="<?= $userid ?>" />
				</form>
			</td>
		</tr>
	</table>
</div>
<div style="padding:10px;">
		You have <?= count($users); ?> friend(s) that match your search.
</div>
<hr/>