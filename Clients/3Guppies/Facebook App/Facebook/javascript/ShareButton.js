// Usage:
//	<div class="share_and_hide clearfix s_and_h_big">
//		<a href="{YourShareUrl}" title="{YourShareTitle}" class="share" onclick="return ShareButtonClick(this)" target="_blank">Share</a>
//	</div>

function ShareButtonClick(element)
{
	u = encodeURIComponent(element.href);
	t = encodeURIComponent('3Guppies Mobile');
	window.open('http://www.facebook.com/sharer.php?u=' + u + '&t=' + t, 'sharer', 'toolbar=0,status=0,resizable=1,width=460,height=380');
	return false;
}
