///////////////////////////////////////////////////////////////////////////////
//
// Jail
// A chrooted login shell implementation
// Released under GNU Public License v.2.0
// See LICENSE file for details
//
// Juan M. Casillas <assman@gsyc.inf.uc3m.es>
// 3/April/ 2001 09:22
// 
///////////////////////////////////////////////////////////////////////////////
//
// $Id: globals.h,v 1.1.1.1 2001/10/26 09:36:09 assman Exp $
//
// $Log: globals.h,v $
// Revision 1.1.1.1  2001/10/26 09:36:09  assman
// Added support for new platforms: FreeBSD, Solaris, IRIX. Now some options
// can be selected from the Makefile script: DEBUG on/off, install path,
// install permissions, etc. The perl scripts have been rewritten so they
// support platform-specific code, so port Jail to another platform should
// be an easy task.
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __JAIL_GLOBALS_H__
#define __JAIL_GLOBALS_H__

#define VERSION         "$Id: globals.h,v 1.1.1.1 2001/10/26 09:36:09 assman Exp $"
#define BANNER          "\nJail version " VERSION " for " ARCH "\n\n" \
	                "This program is released under the GNU License\n" \
	                "Juan M. Casillas <assman@gsyc.inf.uc3m.es>\n" \
	                "See http://www.gsyc.inf.uc3m.es/~assman/jail\n" \
	                "for more info, updates and other projects\n\n"
#define HOME_VAR        "HOME="
#define SHELL_VAR       "SHELL="
#define TERMINAL_GROUP  "tty"
#define PASSWD_FILE     "/etc/passwd"
#define LINE_SZ         256
#define PASSFILESZ      8192*4
#define DEV_PERM        S_IRUSR | S_IWUSR | S_IWGRP
#define DIR_PERM_NUM    0755
#define DIR_PERM        S_IRUSR|S_IXUSR|S_IWUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH

//
// to support IRIX MAXPATHLEN
//

#ifdef MAXPATHLEN
 #define PATH_LEN MAXPATHLEN
#else
 #define PATH_LEN PATH_MAX
#endif


// #define DEBUG           1

#endif
