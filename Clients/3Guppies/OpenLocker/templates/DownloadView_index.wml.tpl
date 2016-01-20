<card id="Download">
<p>
{if $DownloadError}
{$DownloadError}<br/><br/>Need help? Give us a call, MON-FRI 10AM-8PM EST at 1-877-464-9937.
{elseif empty($DownloadData.DownloadUrl)}
We're sorry, the file you are attempting to download is no longer available.
{else}
You are about to download {$DownloadData.Title}.<br/>If your download does not begin immediately, click <a href="{$DownloadData.DownloadUrl}">here</a>
{/if}
</p>
</card>
