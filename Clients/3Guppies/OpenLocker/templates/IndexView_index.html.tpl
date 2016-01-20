{if !isset($HelpText)}
<table>
{foreach name=entry_id from=$LockerEntries item=entry}
<tr>
    <td>{$entry.Id} - <a href="{$entry.PreviewUrl}">{$entry.Title|truncate:25:'...':true}</a></td>

    <td>
{if $entry.Type eq "Unlicensed"}
    details,send,download,delete
{else}
    details,download
{/if}
    </td>

    <td>{$entry.DateAdded}</td>

    <td>{$entry.ContentType}</td>

    <td>
{if $entry.Sharing eq "Private"}
    LockedImage
{elseif $entry.Sharing eq "Public"}
    UnlockedImage
{else}
    --
{/if}
    </td>

    <td>{$entry.Size}</td>
</tr>
{foreachelse}
<tr><td colspan="6">No bloody content found<td></tr>
{/foreach}
</table>
{else}
    {if !empty($HelpText)}
        {$HelpText}
    {else}
<b>Go to mixxer.com with your phone</b></br>
<ol>
<li>Log in to your locker
<li>Once your locker loads up, select the category of your choice ( ringers, screensavers, etc. )<br>
<li>Select an item you would like to download and click on it.<br>
<li>Select YES if the unsafe content box pops up<br>
</ol>
If you do not receive a text message, <a href="">CLICK HERE</a> for the manual process of visiting your locker.<br>
    {/if}
{/if}
