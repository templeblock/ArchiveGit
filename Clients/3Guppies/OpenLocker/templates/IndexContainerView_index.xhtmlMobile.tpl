<!-- TEMP START IndexContainerView_index.xhtmlMobile.tpl -->

<p>
{if $DownloadError}{$DownloadError}<br/><br/>{/if}
<a href="{$URL_BASE}">Home</a><br/>
<a href="#ShowMe">Change Category</a><br/>
<br/>
Currently in: {$ContentType}<br/>
</p>

<p>

{foreach name=entry_id from=$LockerEntries item=entry}
    <a href="{$entry.DownloadUrl}">{$entry.Title|escape:'htmlall'}</a><br/>
{foreachelse}
No content found
{/foreach}

{if isset($Pager)}
{$Pager}
{/if}

</p>

<p>
<a id="ShowMe">&nbsp;</a><br/>
{if $ContentType ne 'All'}<a href="{genurl module="OpenLocker" action="IndexContainer" ContentType="All"}">{/if}All{if $ContentType ne 'All'}</a>{/if}<br/>
{if $ContentType ne 'Audio'}<a href="{genurl module="OpenLocker" action="IndexContainer" ContentType="Audio"}">{/if}Audio{if $ContentType ne 'Audio'}</a>{/if}<br/>
{if $ContentType ne 'Picture'}<a href="{genurl module="OpenLocker" action="IndexContainer" ContentType="Picture"}">{/if}Pictures{if $ContentType ne 'Picture'}</a>{/if}<br/>
{if $ContentType ne 'Video'}<a href="{genurl module="OpenLocker" action="IndexContainer" ContentType="Video"}">{/if}Video{if $ContentType ne 'Video'}</a>{/if}<br/>
{if $ContentType ne 'Other'}<a href="{genurl module="OpenLocker" action="IndexContainer" ContentType="Other"}">{/if}Other Stuff{if $ContentType ne 'Other'}</a>{/if}<br/>
</p>

<!-- TEMP END IndexContainerView_index.xhtmlMobile.tpl -->
