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

class FriendsView extends View
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
		$renderer->setTemplate("FriendsView_index.facebook.tpl");

		$renderer->setAttribute('userid', $request->getAttribute('userid'));
		$renderer->setAttribute('searchTerm', $request->getAttribute('searchTerm'));
		$renderer->setAttribute('searchType', $request->getAttribute('searchType'));
		$renderer->setAttribute('arrUsers', $request->getAttribute('arrUsers'));
		$renderer->setAttribute('photoCountsAjaxUrl', $request->getAttribute('photoCountsAjaxUrl'));
        
        $OmnitureInfo =& OmnitureInfo::getInstance();
        $OmnitureInfo->setHier1( "Facebook:Friend Search" );
        $OmnitureInfo->setPageName( $OmnitureInfo->getHier1() );
        $OmnitureInfo->setChannel( "Facebook" );
        
		return $renderer;
	}
}

?>
