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

class MobilizeView extends View
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
		
		// since this is a facebook-only action, no need for different possibletemplates
		$renderer->setTemplate("MobilizeView_index.facebook.tpl");

		$renderer->setAttribute('userid', $request->getAttribute('userid'));
		$renderer->setAttribute('name', $request->getAttribute('name'));
		$renderer->setAttribute('firstName', $request->getAttribute('firstName'));
		$renderer->setAttribute('aid', $request->getAttribute('aid'));
		$renderer->setAttribute('picUrl', $request->getAttribute('picUrl'));
		$renderer->setAttribute('bAdded', $request->getAttribute('bAdded'));
		$renderer->setAttribute('bIsAppUser', $request->getAttribute('bIsAppUser'));
		$renderer->setAttribute('hisHerTheir', $request->getAttribute('hisHerTheir'));
		$renderer->setAttribute('himHerThem', $request->getAttribute('himHerThem'));
		$renderer->setAttribute('openlockerMobilizeUrl', $request->getAttribute('openlockerMobilizeUrl'));
		$renderer->setAttribute('usersAjaxUrl', $request->getAttribute('usersAjaxUrl'));
		$renderer->setAttribute('thumbsAjaxUrl', $request->getAttribute('thumbsAjaxUrl'));
		$renderer->setAttribute('albumsForDropDown', $request->getAttribute('albumsForDropDown'));
		$renderer->setAttribute('arrUsers', $request->getAttribute('arrUsers'));

        // Omniture Variables
        $OmnitureInfo =& OmnitureInfo::getInstance();
        $OmnitureInfo->setChannel( 'Facebook' );
        $OmnitureInfo->setHier1( 'Facebook:Photos' );
        $OmnitureInfo->setPageName( $OmnitureInfo->getHier1() );
        
		return $renderer;
	}
}

?>
