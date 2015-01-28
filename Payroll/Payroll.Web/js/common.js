	$(document).ready(function ()
	{
		// This script is for the navigation menu - See http://users.tpg.com.au/j_birch/plugins/superfish/ for documentation
		$('ul.sf-menu').supersubs().superfish({
			animation: {height:'show'},		// slide-down effect without fade-in
//			hoverClass: 'sfHover',			// the class applied to hovered list items 
//			pathClass: 'overideThisToUse',	// the class you have applied to list items that lead to the current page 
//			pathLevels: 1,					// the number of levels of submenus that remain open or are restored using pathClass 
//			animation: { opacity: 'show' },	// an object equivalent to first parameter of jQuery’s .animate() method 
//			speed: 'normal',				// speed of the animation. Equivalent to second parameter of jQuery’s .animate() method 
//			autoArrows: true,				// if true, arrow mark-up generated automatically = cleaner source code at expense of initialisation performance 
//			dropShadows: true,				// completely disable drop shadows by setting this to false 
//			disableHI: false,				// set to true to disable hoverIntent detection 
//			onInit: function () { },		// callback function fires once Superfish is initialised – 'this' is the containing ul 
//			onBeforeShow: function () { },	// callback function fires just before reveal animation begins – 'this' is the ul about to open 
//			onShow: function () { },		// callback function fires once reveal animation completed – 'this' is the opened ul 
//			onHide: function () { }			// callback function fires after a sub-menu has closed – 'this' is the ul that just closed 
			delay: 800						// the delay in milliseconds that the mouse can remain outside a submenu without it closing 
		});

		// This script is for the scale9grids - See http://hempton.com/examples/scale9grid/ for documentation
		$('.scale9Grid').scale9Grid({ top: 15, bottom: 15, left: 15, right: 15 });

		// This script is for the accordion - See http://docs.jquery.com/UI/Accordion for documentation
//		$('#accordion').accordion({
//			active: false,
//			autoHeight: false,
//			collapsible: true,
//			icons: {'header': 'ui-icon-plus', 'headerSelected': 'ui-icon-minus' }
//		});

	});
