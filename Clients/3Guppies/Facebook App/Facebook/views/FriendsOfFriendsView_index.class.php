<?php

// +---------------------------------------------------------------------------+
// | Copyright (c) 2007 Mixxer.                                                |
// +---------------------------------------------------------------------------+

/**
 * General Documentation Goes Here
 *
 * <br/><br/>
 *
 * <note>
 *     Notes here.
 * </note>
 *
 * @author  Jim McCurdy
 */

class FriendsOfFriendsView extends View
{

	/**
	 * Render the presentation.
	 *
	 * <br/><br/>
	 *
	 * <note>
	 *     This method should never be called manually.
	 * </note>
	 *
	 * @param Controller A Controller instance.
	 * @param Request    A Request instance.
	 * @param User       A User instance.
	 *
	 * @return Renderer A Renderer instance.
	 *
	 * @access public
	 * @since  1.0
	 */
	function & execute (&$controller, &$request, &$user)
	{
		$renderer =& $request->getAttribute('SmartyRenderer');

		$renderer->setTemplateDir($controller->getModuleDir() . 'templates/');
		
		// since this is a facebook-only action, no need for different possible templates
		$renderer->setTemplate("FriendsOfFriendsView_index.facebook.tpl");

		$renderer->setAttribute('userName', $request->getAttribute('userName'));
		$renderer->setAttribute('arrUsersMe', $request->getAttribute('arrUsersMe'));
		$renderer->setAttribute('arrUsersHasTagged', $request->getAttribute('arrUsersHasTagged'));
		$renderer->setAttribute('arrUsersTaggedBy', $request->getAttribute('arrUsersTaggedBy'));
		$renderer->setAttribute('arrUsersTaggedWith', $request->getAttribute('arrUsersTaggedWith'));
		$renderer->setAttribute('photoCountsAjaxUrl', $request->getAttribute('photoCountsAjaxUrl'));
		
        $OmnitureInfo =& OmnitureInfo::getInstance();
        $OmnitureInfo->setHier1( "Facebook:Friend of Friends" );
        $OmnitureInfo->setPageName( $OmnitureInfo->getHier1() );
        $OmnitureInfo->setChannel( "Facebook" );
        
		return $renderer;
	}
}

?>
