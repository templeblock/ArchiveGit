<?php
	// The facebook client library
	include_once 'facebook-platform/php4client/facebook.php';

	// This defines some of your basic setup
	include_once 'config.php';
	include_once 'facebook.lib.php';

	$pageName = 'Terms of Service';

	$facebook = new Facebook($api_key, $secret);
	$user = $facebook->get_loggedin_user();
	$userid = (int)Param('user', $user);
	$firstName = GetUserInfo($userid, 'first_name');
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
	<title></title>
	<?php include('fbstyles.part.htm'); ?>
	<style type="text/css"><?php include('styles.css'); ?></style>
	<script type="text/javascript"><?php include('fbscript.js'); ?></script>
</head>
<body class="fbframe">
	<?php include('header.part.php'); ?>

	<!-- ----------------------- MAIN CONTENT ----------------------- -->
	<div id="contentSpace">
		<!-- TEMP START: tos.tpl -->
		<div class="moduleWide" id="tos_success">
			<div class="moduleBody" style="padding: 10px;">
				<strong>Acceptance</strong><br />
				<br />
				Welcome to 3Guppies, Inc.'s Web site. These Terms of Use govern your use of www.3guppies.com
				(the "Site") and the services made available on the Site (the "<strong><i>Services</i></strong>").
				3Guppies is the definitive web-to-mobile community, offering an extensive catalog
				of mobile content and the tools you need to discover, connect and share. By using
				the Services or visiting or browsing the Site, you acknowledge that you have read,
				understood, and agreed to be bound by these Terms of Use and any modifications that
				may be made to these Terms of Use from time to time. If you do not agree to these
				Terms of Use, you should not use the Services or visit or browse the Site.
				<br />
				<br />
				These Terms of Use constitute a binding legal agreement between you and 3Guppies,
				Inc. ("<strong>3Guppies</strong>," "<strong>we,</strong>" "<strong>us,</strong>"
				and "<strong>our</strong>"). Please read them carefully before accessing or using
				the Site or the Services.
				<br />
				<br />
				<br />
				<strong>Customer Support</strong>
				<br />
				<br />
				3Guppies is dedicated to providing high-quality customer service. You may contact
				our customer service department in the following ways:
				<br />
				<ol>
					<li>Email to <a href="mailto:customer-support@support.3Guppies.com">customer-support@support.3Guppies.com</a>;</li>
					<li>Submit a ticket to customer support by going to <a href="http://www.3guppies.com/ringtones/module/Help/action/Ticket">
						http://www.3guppies.com/ringtones/module/Help/action/Ticket</a> and our experts
						will respond within 72 hours; or</li>
					<li>Phone contact us directly at 1-877-464-9937 (1-877-464-9937) from 10:00am to 8:00pm
						Eastern Time Monday through Friday.</li>
				</ol>
				<br />
				<strong>Registration</strong>
				<br />
				<br />
				Before you can use certain parts of the Services, you may be required to register
				as a member through the Site. You must be 14 years of age or older to register as
				a member and use the Services. You agree to provide true, accurate, current, and
				complete information about yourself as prompted by the Site's registration form
				(such information, being the "<strong>Registration Data</strong>"), and you agree
				to maintain and promptly update the Registration Data to keep it true, accurate,
				current, and complete. If you provide any information that is untrue, inaccurate,
				not current, or incomplete, or we have any reasonable grounds to suspect that such
				information is untrue, inaccurate, not current, or incomplete, we may suspend or
				terminate your account and refuse to offer you any and all current or future use
				of the Services and the Site. You are responsible for maintaining the confidentiality
				of your account and password and for restricting access to your computer. You are
				solely responsible for any activity related to your account and will not sell or
				otherwise transfer your account. If you suspect any unauthorized use of your account,
				notify us immediately. You acknowledge and agree that we may preserve user information
				and Content and may also disclose user information and Content, if required to do
				so by law or if we believe, in good faith, that such preservation or disclosure
				is reasonably necessary to: (a) comply with legal process; (b) enforce these Terms
				of Use; (c) respond to claims that any Content violates the rights of third parties;
				or (d) protect the rights, property, or personal safety of 3Guppies, its users,
				or the public.
				<br />
				<br />
				<br />
				<strong>Fees and Payment</strong>
				<br />
				<br />
				Subscriptions and other purchases may be paid by credit card or using a Paypal account.
				In order to use the Service, your mobile device(s) must have Internet capabilities
				to allow you to access and use the Content. You will pay all fees and charges incurred
				through your account, including, but not limited to, subscription charges and charges
				for any products or services offered for sale through the Services by 3Guppies or
				by any vendor or service provider. You will also pay any applicable taxes relating
				to use of the Services through your account. At its sole discretion, 3Guppies reserves
				the right at any time to offer subscriptions to the Services at a discount or to
				provide premiums and promotions as a subscription incentive. All fees and charges
				will be charged to the credit card or Paypal account provided as part of the registration
				or purchase process and you agree to pay such charges.
				<br />
				<br />
				For your convenience and to ensure that your access to the Services remains uninterrupted,
				3Guppies will automatically renew your subscription(s) at the expiration of the
				then-current term. If you elected to pay for the Services by providing 3Guppies
				with your credit card, 3Guppies will charge your card on file for the cost of another
				term at the then-current rate using the information you previously provided to us
				by phone or on our website. Please be aware that you may not be notified in advance
				of your impending renewal. 3Guppies reserves the right to increase the price of
				your subscription(s) to the Services upon renewal without notice to you. For subscriptions
				billed to your credit card or Paypal account, you may contact 3Guppies at any time
				during your subscription or within 30 days of the date of billing to cancel your
				subscription. You may request a refund on an initial subscription charge or on a
				subscription renewal charge within 30 days of the date of billing if you have not
				downloaded any Downloadable Content under the subscription. In the event you incur
				an overdraft charge, late fee, or finance charge, 3Guppies may assist you in attempting
				to reverse the charge but is not responsible for the charge. If you have any questions
				or concerns regarding a charge or questions regarding the security of using your
				credit card with 3Guppies' Service, please contact 3Guppies.
				<br />
				<br />
				<br />
				<strong>Content</strong>
				<br />
				<br />
				The Site and the Services may allow you and other third parties to post data, music,
				digital files, text, code, messages, opinions, advice, statements, reviews, comments,
				and other materials and information (collectively, "<strong>User-Generated Content</strong>").
				All User-Generated Content, whether publicly posted on or privately transmitted
				via the Site or the Services, is the sole responsibility of the person from whom
				the User-Generated Content originated and not of 3Guppies, or its shareholders,
				directors, officers, or employees. 3Guppies may review and delete any User-Generated
				Content, in whole or in part, that in the sole judgment of 3Guppies violates these
				Terms of Use or that might be offensive, illegal, or that might violate the rights
				of or harm any third parties. The Site and the Services also include MP3 ringtones,
				voice tones and sound fx, premium graphics/screensavers, premium games, and other
				content that you can download (collectively, "<strong>Downloadable Content</strong>").
				There may be a fee associated with Downloadable Content, and you agree to pay 3Guppies
				all charges for the Downloadable Content as listed in your order confirmation. 3Guppies
				does not give refunds for Downloadable Content. Nonetheless, under no circumstances
				will 3Guppies or its stockholders, directors, officers, employees, agents, representatives,
				partners, or affiliates be held liable for any loss or damage caused by your reliance
				on User-Generated Content or Downloadable Content (collectively, "<strong>Content</strong>")
				obtained through the Site or the Services. It is your responsibility to evaluate
				the Content available through the Services or the Site. Although User-Generated
				Content will not be pre-screened, we may review it at our discretion and we reserve
				the right to refuse or delete any Content. Be aware that while the Services are
				intended for general audiences, because 3Guppies' website contains User-Generated
				Content, objectionable material may appear on the Site or the Services from time
				to time. Individuals who are under the age of 18 or who are otherwise prohibited
				by law to access such content may not access, view or download such content.
				<br />
				<br />
				<br />
				<strong>Licenses</strong>
				<br />
				<br />
				Unless you post User-Generated Content under the Indie Tracks feature, by posting
				User-Generated Content you hereby grant (or warrant that the owner of the User-Generated
				Content grants) us and each user of the Site or the Services a royalty-free, perpetual,
				irrevocable, fully sublicensable, worldwide, non-exclusive right to use, reproduce,
				modify, translate, adapt, publish, create derivative works of, transmit, distribute,
				perform, display, delete (in whole or in part), and incorporate the User-Generated
				Content for any purpose and without acknowledgement to you. By posting or providing
				User-Generated Content, you represent and warrant that public posting and use of
				the User-Generated Content by us and any of our users will not infringe on or violate
				the rights of any third party.
				<br />
				<br />
				If you post User-Generated Content under the Indie Tracks feature, by posting the
				User-Generated Content you hereby grant (and you represent and warrant that you
				have the right to grant) to 3Guppies a royalty-free, perpetual, irrevocable, worldwide,
				non-exclusive right to use, reproduce, modify, translate, adapt, publish, create
				derivative works of, transmit, distribute, perform, display, and delete (in whole
				or in part) the User-Generated Content in connection with the Site and the Services.
				The foregoing license is sublicensable by 3Guppies to permit 3Guppies'sacute; users
				to view, hear, read, stream, download, and link to your User-Generated Content.
				3Guppies'sacute; users may not transfer the foregoing rights. At all times you will
				retain full ownership of your User-Generated Content subject to the non-exclusive
				rights granted to 3Guppies under these Terms of Use. By posting or providing User-Generated
				Content, you represent and warrant that public posting and use of the User-Generated
				Content by us and any of our users will not infringe on or violate the rights of
				any third party.
				<br />
				<br />
				Subject to these Terms of Use, including payment, as applicable, 3Guppies grants
				you a limited, non-exclusive, non-transferable, revocable right to download the
				Downloadable Content to your mobile device solely for your own non-commercial use.
				The Downloadable Content may only be downloaded once from your locker. You understand
				and agree that you may not sell, copy, revise, download, reproduce, modify, publicly
				display, publicly perform, transfer, distribute, create derivative works from, or
				otherwise use the Downloadable Content except as expressly provided in these Terms
				of Use. You understand and agree that you may not authorize, encourage or allow
				any Downloadable Content used or obtained by you to be reproduced, modified, displayed,
				performed, transferred, distributed or otherwise used by any other party, and you
				agree that you will use your best efforts and take all reasonable steps to prevent
				any unauthorized reproduction, distribution, and/or other use of them, in whole
				or in part, and in any medium. You acknowledge that the Downloadable Content is
				owned by 3Guppies and its licensors, is protected by intellectual property laws,
				contains valuable confidential information and trade secrets, and that any unauthorized
				use and/or copying of the Downloadable Content will be harmful to 3Guppies, its
				content providers, and licensors.
				<br />
				<br />
				<br />
				<strong>Claims of Copyright Infringement</strong>
				<br />
				<br />
				It is 3Guppies' policy to respond expeditiously to claims of intellectual property
				infringement. 3Guppies will promptly process and investigate notices of alleged
				infringement and will take appropriate actions under the Digital Millennium Copyright
				Act ("DMCA") and other applicable intellectual property laws. Please read this section
				carefully if you are submitting notice of alleged infringement by a user. Upon receipt
				of notices complying or substantially complying with the DMCA, 3Guppies will act
				expeditiously and remove or disable access to any material claimed to be infringing
				or claimed to be the subject of infringing activity and will act expeditiously to
				remove or disable access to any reference or link to material or activity that is
				claimed to be infringing. 3Guppies will terminate access for subscribers and account
				holders who are repeat infringers.
				<br />
				<br />
				You may submit a notice electronically by emailing <a href="mailto:licensing@3Guppies.com">
					licensing@3Guppies.com</a>, or in writing. Written notices of claimed infringement
				should be directed to:
				<br />
				<br />
				<pre>
Copyright Compliance Officer
3Guppies, Inc., 9775 Crosspoint Blvd. Suite 100 A, Indianapolis, IN 46256
</pre>
				<br />
				All written notices must include, at a minimum, the following:
				<ol>
					<li>Identification of the copyrighted work claimed to have been infringed;</li>
					<li>Identification of the material that is claimed to be infringing or to be the subject
						of infringing activity</li>
					<li>Information reasonably sufficient to permit 3Guppies to contact you, such as an
						address, telephone number, and, if available, an electronic mail address;</li>
					<li>A statement that the complaining party has a good faith belief that use of the material
						in the matter complained of is not authorized by the copyright owner, its agent,
						or the law;</li>
					<li>A statement that the information in the notice is accurate and, under penalty of
						perjury, that the complaining party is authorized to act on behalf of the owner
						of an exclusive right that is allegedly infringed; and</li>
					<li>An electronic or physical signature of the copyright owner or a person authorized
						to act on behalf of the owner of an exclusive right that is allegedly infringed.</li>
				</ol>
				Upon receipt of notification of claimed copyright infringement, 3Guppies will follow
				the procedures outlined in Title II of the Digital Millennium Copyright Act (17
				U.S.C. §512). Failure to include all of the above-listed information may result
				in a delay in processing the complaint.
				<br />
				<br />
				When 3Guppies removes or disables access to any material claimed to be infringing,
				3Guppies may attempt to contact the user who has posted such material in order to
				give that user an opportunity to respond to the notification, although 3Guppies
				makes no promise to do so. You acknowledge that, in cases where you have supplied
				infringing Content, 3Guppies may disclose your identity if 3Guppies believes in
				good faith that such disclosure is required by law.
				<br />
				<br />
				Any and all counter notifications submitted by the user will be furnished to the
				complaining party. 3Guppies will give the complaining party an opportunity to seek
				judicial relief in accordance with the DMCA before 3Guppies replaces or restores
				access to any material as a result of any counter notification.
				<br />
				<br />
				3Guppies occasionally receives requests to remove Content. You acknowledge that
				we may remove any Content from the Service, including your User-Generated Content,
				if the posting of that User-Generated Content violates these Terms of Use, or if
				you have violated these Terms of Use.
				<br />
				<br />
				<br />
				<strong>Usage Guidelines and Restrictions</strong>
				<br />
				<br />
				You agree not to use the Site or the Services to:
				<ol>
					<li>upload, post, e-mail, or otherwise transmit any User-Generated Content that is unlawful,
						harmful, threatening, abusive, harassing, tortious, defamatory, vulgar, obscene,
						contains nudity or any other material that may be considered "adult", libelous,
						invasive of another's privacy, hateful, or racially, ethnically, or otherwise objectionable;</li>
					<li>harm minors in any way or impersonate any person or entity, or otherwise misrepresent
						your affiliation with a person or entity;</li>
					<li>upload, post, e-mail, or otherwise transmit User-Generated Content that you do not
						have a right to transmit under any law or under contractual or fiduciary relationships
						(such as inside information, proprietary and confidential information learned or
						disclosed as part of employment relationships, or under nondisclosure agreements);</li>
					<li>upload, post, e-mail, or otherwise transmit User-Generated Content that infringes
						any patent, trademark, trade secret, copyright, or other proprietary rights of any
						party;</li>
					<li>upload, post, e-mail, or otherwise transmit any unsolicited or unauthorized advertising,
						promotional materials, "junk mail," "spam," "chain letters," "pyramid schemes,"
						or any other forms of solicitation;</li>
					<li>upload, post, e-mail, or otherwise transmit any material that contains software
						viruses or any other computer code, files, or programs designed to interrupt, destroy,
						or limit the functionality of any computer software or hardware or telecommunications
						equipment;</li>
					<li>interfere with or disrupt the Services, the Site, or servers or networks connected
						to the Site, or disobey any requirements, procedures, policies, or regulations of
						networks connected to the Site;</li>
					<li>circumvent any security measure or any payment collection method employed on the
						Services, or access the Services after your account or access has been terminated
						by 3Guppies;</li>
					<li>intentionally or unintentionally violate any applicable local, state, national,
						or international law, "stalk" or otherwise harass another, or collect or store personal
						data about other users;</li>
					<li>use any automated means to access the Site or the Services or collect any information
						from the Site or the Services (including, without limitation, robots, spiders, or
						scripts);</li>
					<li>frame the Site or the Services, utilize framing techniques to enclose any service
						mark, logo, or other proprietary information, place pop-up windows over its pages,
						or otherwise affect the display of its pages. This means, among other activities,
						that you must not engage in the practices of "screen scraping," "database scraping,"
						or any other activity with the purpose of obtaining lists of users or other information;
						or</li>
					<li>cover or obscure banner advertisements on any 3Guppies Web page including your personal
						profile page.</li>
				</ol>
				The Site and the Services are for your personal use only and may not be used in
				connection with any commercial endeavors except those that are specifically endorsed
				or approved by 3Guppies. Illegal and/or unauthorized use of the Site or the Services,
				including collecting usernames and/or email addresses of other users by electronic
				or other means for the purpose of sending unsolicited email or unauthorized framing
				of or linking to the Site or the Services is prohibited. Commercial advertisements,
				affiliate links, and other forms of solicitation may be removed from the Site and
				the Services without notice and may result in termination of your account and user
				privileges. Appropriate legal action will be taken for any illegal or unauthorized
				use of the Site or the Services.
				<br />
				<br />
				<br />
				<strong>Termination</strong>
				<br />
				<br />
				You agree that 3Guppies, in its sole discretion, may terminate your password, account
				(or any part thereof), and use of the Site and the Services, and remove and discard
				any Content within the Site, for any reason, including, without limitation, for
				lack of use or if 3Guppies believes that you have violated or acted inconsistently
				with the letter or spirit of these Terms of Use. 3Guppies may also in its sole discretion
				and at any time modify or discontinue providing the Site or the Services, or any
				part thereof, with or without notice. You agree that any modification or termination
				of your access to the Site or the Services may be effected without prior notice,
				and you acknowledge and agree that 3Guppies may immediately deactivate or delete
				your account and all related information and Content in your account and bar any
				further access to such information or to the Site or the Services. Further, you
				agree that 3Guppies will not be liable to you or any third party for any modification
				or termination of your access to the Site or the Services. Even after termination
				of these Terms of Use, the Sections on Licenses, Usage Guidelines and Restrictions,
				Termination, Modifications, Disclaimers of Warranties; Limitations of Liability,
				Indemnity, Governing Law and Arbitration, Release, Claims of Copyright Infringement,
				and General remain in effect.
				<br />
				<br />
				<br />
				<strong>Privacy</strong>
				<br />
				<br />
				Any information submitted on the Site or the Services is subject to our Privacy
				Policy, the terms of which are incorporated into these Terms of Use. Our Privacy
				Policy may be found by clicking <a href="Privacy">here</a>. Please review our Privacy
				Policy carefully.
				<br />
				<br />
				<br />
				<strong>Modifications</strong>
				<br />
				<br />
				We reserve the right to modify these Terms of Use at any time and in any manner
				at our sole discretion. Notice of any modification of these Terms of Use will be
				posted in the Terms of Use section of the Site, and any such modifications will
				be effective upon the posting of such notice. Your continued use of the Site or
				the Services constitutes your binding acceptance of such modifications. Please check
				this section of the Site before using the Site or the Services to determine whether
				a change has been made to these Terms of Use. If you do not agree to any changes
				in the Terms of Use as they may occur, please arrange to terminate your registration
				with the Site immediately and discontinue your use of the Services and the Site.
				You agree that we are not liable to you or to any third party for any modification
				of the Terms of Use.
				<br />
				<br />
				<br />
				<strong>Disclaimers of Warranties; Limitations of Liability</strong>
				<br />
				<br />
				The Site, the Services, and all Content (including licensed databases) are provided
				"as is"; 3Guppies and its licensors make no representations or warranties of any
				kind with respect to the Services, the Content, the Site, or any contents therein.
				3Guppies and its licensors assume no liability or responsibility for any errors
				or omissions in providing the Services, the Site, or the Content, any losses or
				damages arising from the use of the Content, or any conduct by users of the Site.
				3GUPPIES AND ITS LICENSORS EXPRESSLY DISCLAIM ALL REPRESENTATIONS AND WARRANTIES
				RELATING TO THE SERVICES, THE CONTENT, AND THE SITE, INCLUDING, BUT NOT LIMITED
				TO, WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUALITY, ACCURACY,
				AUTHENTICITY, TITLE, AND NON-INFRINGEMENT. In addition, 3Guppies and its licensors
				do not represent or warrant that the information on or accessible via the Site or
				through the Services is complete or current.
				<br />
				<br />
				3GUPPIES'Sacute; AND ITS AFFILATES', SUPPLIERS', AND THEIR RESPECTIVE OFFICERS',
				DIRECTORS', STOCKHOLDERS', EMPLOYEES', AGENTS', AND REPRESENTATIVES' LIABILITY UNDER
				THESE TERMS OF USE IS LIMITED TO DIRECT, OBJECTIVELY MEASURABLE DAMAGES. 3GUPPIES
				AND ITS AFFILATES, SUPPLIERS, AND THEIR RESPECTIVE OFFICERS, DIRECTORS, STOCKHOLDERS,
				EMPLOYEES, AGENTS, AND REPRESENTATIVES WILL NOT BE HELD LIABLE FOR ANY INDIRECT
				OR SPECULATIVE DAMAGES (INCLUDING, WITHOUT LIMITING THE FOREGOING, CONSEQUENTIAL,
				INCIDENTAL, AND SPECIAL DAMAGES) INCLUDING, BUT NOT LIMITED TO, LOSS OF USE, BUSINESS
				INTERRUPTIONS, LOSS OF DATA, AND LOSS OF PROFITS, REGARDLESS OF WHETHER THESE PARTIES
				HAD ADVANCE NOTICE OF THE POSSIBILITY OF ANY SUCH DAMAGES. 3GUPPIES'Sacute; AND
				ITS AFFILATES', SUPPLIERS', AND THEIR RESPECTIVE OFFICERS', DIRECTORS', STOCKHOLDERS',
				EMPLOYEES', AGENTS', AND REPRESENTATIVES' TOTAL LIABILITY TO YOU FOR ANY CLAIM ARISING
				OUT OF OR RELATING TO THE SERVICES, THE CONTENT, THE SITE OR THESE TERMS OF USE,
				WHETHER IN CONTRACT OR IN TORT, SHALL NOT EXCEED $100. EACH PROVISION OF THESE TERMS
				OF USE THAT PROVIDES FOR A LIMITATION OF LIABILITY, DISCLAIMER OF WARRANTIES, OR
				EXCLUSION OF DAMAGES IS TO ALLOCATE THE RISKS UNDER THESE TERMS OF USE BETWEEN THE
				PARTIES. THIS ALLOCATION IS REFLECTED IN THE PRICING AND TERMS OFFERED TO YOU BY
				3GUPPIES AND IS AN ESSENTIAL ELEMENT OF THE BASIS OF THE BARGAIN BETWEEN THE PARTIES.
				EACH OF THESE PROVISIONS IS SEVERABLE AND INDEPENDENT OF ALL OTHER PROVISIONS OF
				THESE TERMS OF USE. THE LIMITATIONS IN THIS SECTION WILL APPLY NOTWITHSTANDING THE
				FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY UNDER THESE TERMS OF USE. Some
				states do not allow the foregoing limitations of liability, so they may not apply
				to you.
				<br />
				<br />
				<br />
				<strong>Indemnity</strong>
				<br />
				<br />
				You agree to indemnify and hold 3Guppies or its stockholders, directors, officers,
				employees, agents, representatives, partners, or affiliates harmless from any loss,
				liability, claim, or demand, including reasonable attorneys' fees, arising out of,
				resulting from or connected with the use, modification, misinterpretation, misuse,
				or reuse by you of the Site, the Services, or Content, including without limitation
				these Terms of Use.
				<br />
				<br />
				<br />
				<strong>Links and Advertising</strong>
				<br />
				<br />
				We may provide, or third parties may provide, links to other Web sites or resources
				that are beyond our control. We make no representations as to the quality, suitability,
				functionality, or legality of any sites to which links may be provided, and you
				hereby waive any claim you might have against us, with respect to such sites. 3GUPPIES
				IS NOT RESPONSIBLE FOR THE CONTENT ON THE INTERNET OR WEB PAGES THAT ARE CONTAINED
				OUTSIDE THE SITE.
				<br />
				<br />
				Your correspondence or business dealings with, or participation in promotions of,
				other users, advertisers, or partners found on or through the Site or the Services,
				and any other terms, conditions, warranties, or representations associated with
				such dealings, are solely between you and such user, advertiser, or partner. You
				agree that we are not responsible or liable for any loss or damage of any sort incurred
				as the result of any such dealings or as the result of the presence of such advertisers
				or partners on the Site or the Services.
				<br />
				<br />
				<br />
				<strong>Governing Law and Arbitration</strong>
				<br />
				<br />
				These Terms of Use are governed in all respects by the laws of the State of Washington,
				as such laws are applied to agreements entered into and to be performed entirely
				within Washington between Washington residents. Any controversy or claim arising
				out of or relating to these Terms of Use, the Services, the Products, or the Site
				will be settled by binding arbitration in accordance with the commercial arbitration
				rules of the American Arbitration Association. Any such controversy or claim will
				be arbitrated on an individual basis, and will not be consolidated in any arbitration
				with any claim or controversy of any other party. The arbitration will be conducted
				in King County, Washington, and judgment on the arbitration award may be entered
				into any court having jurisdiction thereof. The award of the arbitrator will be
				final and binding upon the parties without appeal or review except as permitted
				by Washington law. Notwithstanding the foregoing, either party may seek any interim
				or preliminary injunctive relief from any court of competent jurisdiction, as necessary
				to protect the party's rights or property pending the completion of arbitration.
				By using the Site or the Services, you consent and submit to the exclusive jurisdiction
				and venue of the state and federal courts located in King County, Washington.
				<br />
				<br />
				<br />
				<strong>Release</strong>
				<br />
				<br />
				In the event that you have a dispute with one or more users of the Site or the Services,
				you release 3Guppies and its stockholders, directors, officers, employees, agents,
				representatives, partners, and affiliatesfrom claims, demands, and damages (actual
				and consequential) of every kind and nature, known and unknown, suspected and unsuspected,
				disclosed and undisclosed, arising out of or in any way connected with such disputes.
				If you are a California resident, you waive California Civil Code §1542, which says:
				"A general release does not extend to claims which the creditor does not know or
				suspect to exist in his favor at the time of executing the release, which if known
				by him must have materially affected his settlement with the debtor."
				<br />
				<br />
				<br />
				<strong>Gift Memberships</strong>
				<br />
				<br />
				In order to purchase, receive, or make use of 3Guppies'sacute; gift membership service,
				you must agree to the following terms:
				<ol>
					<li>If you purchase a 3Guppies gift membership, you agree to provide to the best of
						your knowledge true, accurate and complete information about the recipient of the
						gift (the "Recipient") as prompted by the gift membership form ("Gift Registration
						Data"). 3Guppies will then create a membership account and notify the Recipient
						that a 3Guppies Locker has been created on Recipient's behalf and provide the Recipient
						with an identification code. If 3Guppies has reasonable grounds to suspect that
						the Gift Registration Data is untrue, inaccurate or incomplete, 3Guppies has the
						right to suspend or terminate the gift membership account. You acknowledge and agree
						that 3Guppies will have no liability associated with or arising from your failure
						to provide accurate Gift Registration Data for the gift membership Recipient.</li>
					<li>Only one Recipient may be designated for each gift membership. Only a designated
						Recipient may make use of a gift membership account. There is no limit on the number
						of gift memberships a particular Recipient may receive. </li>
					<li>No substitutions are permitted. By way of example, an accessory may not be substituted
						for a ringtone of equivalent value. </li>
					<li>Unused portions of gift memberships are nontransferable and nonrefundable. </li>
					<li>The membership term begins on the date the gift membership is purchased and ends
						on the last day of the purchased term. </li>
					<li>Gift memberships are not redeemable for cash and cannot be returned for a cash refund.
					</li>
					<li>Recipient permits 3Guppies to provide the donor of Recipient's gift membership with
						information concerning Recipient's usage of the gift membership. </li>
				</ol>
				3GUPPIES, ITS LICENSORS, AND ITS AFFILIATES MAKE NO WARRANTIES, EXPRESS OR IMPLIED,
				WITH RESPECT TO GIFT MEMBERSHIPS, INCLUDING WITHOUT LIMITATION, ANY EXPRESS OR IMPLIED
				WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. IN THE EVENT A
				GIFT MEMBERSHIP ACCOUNT IS NON-FUNCTIONAL, YOUR SOLE REMEDY, AND OUR SOLE LIABILITY,
				SHALL BE THE REPLACEMENT OF SUCH GIFT MEMBERSHIP. CERTAIN STATE LAWS DO NOT ALLOW
				LIMITATIONS ON IMPLIED WARRANTIES OR THE EXCLUSION OR LIMITATION OF CERTAIN DAMAGES.
				IF THESE LAWS APPLY TO YOU, SOME OR ALL OF THE ABOVE DISCLAIMERS, EXCLUSIONS, OR
				LIMITATIONS MAY NOT APPLY TO YOU, AND YOU MIGHT HAVE ADDITIONAL RIGHTS.
				<br />
				<br />
				<br />
				<strong>General</strong>
				<br />
				<br />
				3Guppies'sacute; failure to act in a particular circumstance does not waive its
				ability to act with respect to that circumstance or similar circumstances. By using
				the Site or the Services, you consent to receiving electronic communications from
				3Guppies. These communications will include notices about your account and information
				concerning or related to the Services. You agree that any notice, agreements, disclosure,
				or other communications that we send to you electronically will satisfy any legal
				communication requirements, including that such communications be in writing. 3Guppies
				is excused for any failure to perform to the extent that its performance is prevented
				by any reason outside of its control. These Terms of Use, together with our Privacy
				Policy, comprise the entire agreement between you and 3Guppies and supersede all
				prior agreements between the parties regarding the subject matter contained herein.
				<br />
				<br />
				Please direct any questions regarding these Terms of Use to: <a href="mailto:termsofuse@3Guppies.com">
					termsofuse@3Guppies.com</a>.
				<br />
				<br />
			</div>
		</div>
		<!-- TEMP END: tos.tpl -->
	</div>
	</td>
	<!-- /END Main Content -->

	<div style="position: relative; bottom: 0;">
		<?php include('footer.part.php'); ?>
	</div>
</body>
</html>
