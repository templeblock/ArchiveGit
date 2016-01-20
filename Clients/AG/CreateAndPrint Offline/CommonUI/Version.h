//
// Don't forget to update the version number in Package/IE/AxCtp.inf and Package/Netscape/install.js
//

#define VER_COMPANY 			"AG Interactive\0"
#define VER_COPYRIGHT			"Copyright © 2000-2005 AG.com, Inc.\0"

// Version format is as follows: [Major],[Minor],[Build],0
// [Build] number is a concatenation of [Year(1digit)][Month(2Digit)][Week(1digit)] of the build
#define VER_PRODUCT_VERSION 	 3,5,5065,0
#define VER_PRODUCT_VERSION_STR "3.5 (build 5065)"
#define VER_FILE_VERSION		VER_PRODUCT_VERSION
#define VER_FILE_VERSION_STR	VER_PRODUCT_VERSION_STR

#ifndef NETSCAPE
#define VER_INTERNAL_NAME		"AxCtp\0"
#define VER_FILE_NAME			"AxCtp.dll\0"
#define VER_MIMETYPE
#else
#define VER_INTERNAL_NAME		"NpCtp\0"
#define VER_FILE_NAME			"NpCtp.dll\0"
#define VER_MIMETYPE VALUE		"MIMEType", "application/x-ctp-plugin\0"
#endif
