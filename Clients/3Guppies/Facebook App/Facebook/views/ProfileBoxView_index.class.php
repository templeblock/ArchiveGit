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

class ProfileBoxView extends View
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
		$renderer->setTemplate("ProfileBoxView_index.facebook.tpl");

		$renderer->setAttribute('userid', $request->getAttribute('userid'));
		$renderer->setAttribute('lastVisited', $request->getAttribute('lastVisited'));
		$renderer->setAttribute('lastVisitedDaysAgo', $request->getAttribute('lastVisitedDaysAgo'));
		$renderer->setAttribute('widgetCode', $request->getAttribute('widgetCode'));

		return $renderer;
	}
}

?>
