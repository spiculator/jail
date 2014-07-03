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
// $Id: terminal_helpers.c,v 1.1.1.1 2001/10/26 09:36:09 assman Exp $
//
// $Log: terminal_helpers.c,v $
// Revision 1.1.1.1  2001/10/26 09:36:09  assman
// Added support for new platforms: FreeBSD, Solaris, IRIX. Now some options
// can be selected from the Makefile script: DEBUG on/off, install path,
// install permissions, etc. The perl scripts have been rewritten so they
// support platform-specific code, so port Jail to another platform should
// be an easy task.
//
//
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#if defined(__SOLARIS__) || defined(__IRIX__)
   #include <sys/types.h>
   #include <sys/mkdev.h>
#endif

#include "types.h"

//////////////////////////////////////////////////////////////////////////////
//
// get_tid_info()
// get the terminal info (path, major and minor numbers here)
//
// INPUT args:
//
//      nothing
//  
// OUTPUT args:
//
//      NULL           if the terminal isn't found
//      device_data*   a pointer to a struct with the data
//
//////////////////////////////////////////////////////////////////////////////

struct device_data *get_tid_info(void) {

    struct stat st_data;
    char *term_tid;
    
    struct device_data *ddata;

    term_tid = ctermid(NULL);
    if (term_tid == NULL)
	return(NULL);

    stat(term_tid, &st_data);
    if (!S_ISCHR(st_data.st_mode)) 
	return(NULL);

    ddata = (struct device_data *)malloc(sizeof(struct device_data));
    ddata->path=(char *)malloc(strlen(term_tid)+1);
    
    memset(ddata->path,'\0',strlen(term_tid)+1);
    strcpy(ddata->path,term_tid);
    
    ddata->major = major(st_data.st_rdev);
    ddata->minor = minor(st_data.st_rdev);

    return(ddata);
}


//////////////////////////////////////////////////////////////////////////////
//
// get_tty_info()
// get the terminal info (path, major and minor numbers here)
//
// INPUT args:
//
//      nothing
//  
// OUTPUT args:
//
//      NULL           if the terminal isn't found
//      device_data*   a pointer to a struct with the data
//
//////////////////////////////////////////////////////////////////////////////

struct device_data *get_tty_info(void) {

    struct stat st_data;
    char *term_tty;
    
    struct device_data *ddata;

    term_tty = ttyname(0);

    if (term_tty == NULL)
	return(NULL);

    stat(term_tty, &st_data);
    if (!S_ISCHR(st_data.st_mode)) 
	return(NULL);

    ddata = (struct device_data *)malloc(sizeof(struct device_data));
    ddata->path=(char *)malloc(strlen(term_tty)+1);

    memset(ddata->path,'\0',strlen(term_tty)+1);
    strcpy(ddata->path,term_tty);
    
    ddata->major = major(st_data.st_rdev);
    ddata->minor = minor(st_data.st_rdev);

    return(ddata);
}
