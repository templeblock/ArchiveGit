{* using variables:
	$userid
	$searchTerm
	$searchType
	$photoCountsAjaxUrl
	$smarty.const.FACEBOOK_CANVAS_URL
	$arrUsers[n].userid
	$arrUsers[n].name
	$arrUsers[n].firstName
	$arrUsers[n].bIsAppUser
	$arrUsers[n].picUrl
	$arrUsers[n].hisHerTheir
	$arrUsers[n].himHerThem
	$arrUsers[n].numPhotos
	$arrUsers[n].numAlbums
	$arrUsers[n].networks
*}

{addheaderfile file="http://yui.yahooapis.com/2.3.1/build/yahoo/yahoo-min.js"}
{addheaderfile file="http://yui.yahooapis.com/2.3.1/build/event/event-min.js"}
{addheaderfile file="http://yui.yahooapis.com/2.3.1/build/connection/connection-min.js"}
{addheaderfile file="modules/Facebook/javascript/FriendsView.js"}
{addheaderfile file="modules/Facebook/css/FacebookGlobal.css"}
{addheaderfile file="modules/Facebook/css/FriendsView.css"}

<input type="hidden" id="param_photoCountsAjaxUrl" value="{$photoCountsAjaxUrl}" />

<div id="friendtables">
    <table cellspacing="0" cellpadding="0" border="0">
        <tr>
            <td>
                <h4>Select a network:&nbsp;&nbsp;&nbsp;&nbsp;</h4>
            </td>
            <td>
                <form id="searchTypeForm" name="searchTypeForm" method="get" target="_parent" action="{$smarty.const.FACEBOOK_CANVAS_URL}Friends">
                    <select name="searchType" onchange="document.getElementById('searchTypeForm').submit();">
                        <option value="-all"		{if $searchType == '-all'		} selected="" {/if} >All Friends</option>
                        <option value="-updated"	{if $searchType == '-updated'	} selected="" {/if} >Friends with Updated Profiles</option>
                        <option value="-hastagged"	{if $searchType == '-hastagged'	} selected="" {/if} >Friends you have tagged</option>
                        <option value="-taggedby"	{if $searchType == '-taggedby'	} selected="" {/if} >Friends you are tagged by</option>
                        <option value="-taggedwith"	{if $searchType == '-taggedwith'} selected="" {/if} >Friends you are tagged with</option>
                        <option value="-college"	{if $searchType == '-college'	} selected="" {/if} >College Friends</option>
                        <option value="-hs"			{if $searchType == '-hs'		} selected="" {/if} >High School Friends</option>
                        <option value="-work"		{if $searchType == '-work'		} selected="" {/if} >Work Friends</option>
                        <option value="-home"		{if $searchType == '-home'		} selected="" {/if} >Hometown Friends</option>
                        <option value="-city"		{if $searchType == '-city'		} selected="" {/if} >Current Location Friends</option>
                        <option value="-region"		{if $searchType == '-region'	} selected="" {/if} >Regional Friends</option>
                        <!--option value="-with"    {if $searchType == '-with'		} selected="" {/if} >Friends with 3Guppies</option-->
                        <!--option value="-without" {if $searchType == '-without'	} selected="" {/if} >Friends without 3Guppies</option-->
                        {if $searchTerm}
                            <option value="-custom" selected="">Custom Search "{$searchTerm}"</option>
                        {/if}
                    </select>
                    <input type="hidden" name="user" value="{$userid}" />
                </form>
            </td>
            <td>
                <h4>&nbsp;&nbsp; or &nbsp;&nbsp;</h4>
            </td>
            <td>
                <form name="searchTermForm" method="get" target="_parent" action="{$smarty.const.FACEBOOK_CANVAS_URL}Friends">
                    <input name="searchTerm" id="searchTerm" type="search" class="textInput" style="font-size: 10px; padding: 3px;" placeholder="Search within all friends" value="" />
                    <script type="text/javascript">placeholderSetup('searchTerm');</script>
                    <button type="submit" class="globalBtn green" style="margin-bottom: 5px;">Search</button>
                    <input type="hidden" name="user" value="{$userid}" />
                </form>
            </td>
        </tr>
    </table>
    
    <p>You have {$arrUsers|@count} friend(s) that match your search.</p>

    <hr />
    
    {foreach item=usr from=$arrUsers}
        <table cellspacing="0" cellpadding="4" border="0">
            <tr style="vertical-align:top; ">
                <td style="width: 100px; min-height: 100px; ">
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Mobilize?user={$usr.userid}"><img src="{$usr.picUrl}" alt="" /></a>
                </td>
                <td style="width: 200px;">
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Mobilize?user={$usr.userid}">{$usr.name}</a><br/>
					<span id="idPhotoCounts" user="{$usr.userid}"> <!-- Ajax calls will replace this span's inner html -->
	                    {$usr.numPhotos} Photo{if $usr.numPhotos != 1}s{/if}<br/>
	                    {$usr.numAlbums} Album{if $usr.numAlbums != 1}s{/if}<br/>
					</span>
                </td>
                <td style="width: 250px; text-align: right;">
                    {if ($usr.bIsAppUser)}
                        <a target="_parent" href="{$smarty.const.FACEBOOK_ACTIVITY_URL}{$usr.userid}">View {$usr.firstName}'s recent 3Guppies activity</a>
                    {else}
                        <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}Invite?user={$usr.userid}">Invite {$usr.firstName} to 3Guppies!</a>
                    {/if}
                    <br />
                    <!--a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}FriendsOfFriends?user={$usr.userid}">Browse {$usr.hisHerTheir} Tagged Photo Friends</a><br/-->
                    <a target="_parent" href="{$smarty.const.FACEBOOK_CANVAS_URL}FriendsOfFriends?user={$usr.userid}" style="text-decoration: none; color: black;">
	                    {$usr.networks}
					</a>
                </td>
            </tr>
            <tr><td colspan="3"><hr /></td></tr>
        </table>
    {/foreach}
</div>
