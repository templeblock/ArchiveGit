{* using variables:
	$bOK
	$command
	$nameRecipient
*}

<div style="padding: 10px; font-family: Verdana, Arial; font-size: 10px;">
	<br/>
{if ($bOK)}
	<h3>
	You just posted {$command} to {$nameRecipient}.
	</h3>
{else}
	<h3>
	We were unable to post {$command} to {$nameRecipient}.<br/><br/>
	This is probably due to the fact that Facebook only allows 10 to be sent in a 24 hour period.<br/><br/>
	Please try again later.
	</h3>
{/if}
</div>
