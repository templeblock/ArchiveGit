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

class HomeView extends View
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
		
		// since this is a facebook-only action, no need for different possible
		// templates
		$renderer->setTemplate("MobilizeView_index.facebook.tpl");

		$renderer->setAttribute( 'fbUserName', 
			$request->getAttribute( 'fbUserName' ) );
		
		return $renderer;
	}

}

?>
