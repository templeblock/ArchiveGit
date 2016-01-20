<!-- TEMP START IndexContainerView_index.wml.tpl -->

<card title="MyLocker">
<p>
<a href="{$URL_BASE}">Home</a><br/>
<a href="#ShowMe">Change Category</a><br/>
<br/>
Currently in: {$ContentType}<br/>
{foreach name=entry_id from=$LockerEntries item=entry}
    <a href="{$entry.DownloadUrl}">{$entry.Title|escape:'htmlall'}</a><br/>
{foreachelse}
No content found
{/foreach}

{if isset($Pager)}
{$Pager}
{/if}

</p>
</card>

<card id="ShowMe" title="My Locker Categories">
<p>
{if $ContentType ne 'All'}<a href="{genurl module="OpenLocker" action="IndexContainer" ContentType="All"}">{/if}All{if $ContentType ne 'All'}</a>{/if}<br/>
{if $ContentType ne 'Audio'}<a href="{genurl module="OpenLocker" action="IndexContainer" ContentType="Audio"}">{/if}Audio{if $ContentType ne 'Audio'}</a>{/if}<br/>
{if $ContentType ne 'Picture'}<a href="{genurl module="OpenLocker" action="IndexContainer" ContentType="Picture"}">{/if}Pictures{if $ContentType ne 'Picture'}</a>{/if}<br/>
{if $ContentType ne 'Video'}<a href="{genurl module="OpenLocker" action="IndexContainer" ContentType="Video"}">{/if}Video{if $ContentType ne 'Video'}</a>{/if}<br/>
{if $ContentType ne 'Other'}<a href="{genurl module="OpenLocker" action="IndexContainer" ContentType="Other"}">{/if}Other Stuff{if $ContentType ne 'Other'}</a>{/if}<br/>
</p>
</card>
<!-- TEMP END IndexContainerView_index.xml.tpl -->
