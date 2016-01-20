{ldelim} 
    status : {$openl_exter_errorcode},
    
	{include file="ExternalMobilizeView_error_base.async.tpl" assign="contentsTemplate"}
	content : "{$contentsTemplate|regex_replace:"/[\r\n]+/":" "|replace:'"':'\"'}"
{rdelim},
    yuiWidth: '420px'
